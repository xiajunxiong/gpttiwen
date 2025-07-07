CoupletView = CoupletView or DeclareView("CoupletView", "anecdote/couplet_view", Mod.Anecdote.Couplet)

function CoupletView:CoupletView()
	self.data = CoupletData.Instance
end
--[[param_t = 
	config = 外部自己的配置 相互独立,
	img_type = 外部图片类型,
	send_func = 外部自己的完成请求
	close_func = 自己的关闭提示
	now_group = 外部控制显示第几组
}]]
function CoupletView:LoadCallback(param_t)
	self.data:DataReset(AnecdoteType.Couplet, param_t.config or Config.world_couplet_auto,param_t.now_group)
	self.TopBigList:SetData(self.data:GetTopListData())
	self.DownList:SetData(self.data:GetDownListData())
	self.data:SetTopListCallback(BindTool.Bind1(self.SetSelectMono, self))
	self.Timer3 = TimeHelper:AddDelayTimer(BindTool.Bind1(self.AddDelayTimer, self), 1)
	AnecdoteData.Instance:CustomAutoHandle(AnecdoteType.Couplet,BindTool.Bind1(self.OnAutoFunc, self))
	
	local img_type = param_t.img_type or 0
	UH.SpriteName(self.ImgLeft, [[Left]] .. img_type)
	UH.SpriteName(self.ImgRight,[[_LocRight]] .. img_type)
	UH.SpriteName(self.ImgComplete,[[_LocComplete]] .. img_type)

	self.send_func = param_t.send_func
	self.close_func = param_t.close_func
	self.need_show_talk_id = -1
	self:FlushFinishWord()
end

function CoupletView:FlushFinishWord()
	for i=1,self.WordList:Length() do
		local word = self.data:GetFinishWord(i-1)
		UH.SetText(self.WordList[i],word or "")
		if word ~= nil then
			self.IsHasWord = true
		end
	end
end

function CoupletView:AddDelayTimer()
	self.ObjTopBigList:SetActive(true)
	self.DownList.gameObject:SetActive(true)
end

--主线任务自动完成行为
function CoupletView:OnAutoFunc()
	if self.data:GetIsFinish() then
		return
	end
	ViewMgr:CloseView(DialogTipsView)
	self.data:FastFinish()
	self:OnCompleteClick(CoupletView)
end

-- 将需要填的字的mono存储起来
function CoupletView:SetSelectMono()
	local can_select_mono = {}
	for i, v in ipairs(self.data:GetCanSelectIdList()) do
		local num1 = math.floor(v.pos / 100)
		local num2 = v.pos % 100
		local list1 = self.TopBigList:GetItem(num1)
		local t = {}
		t.top_mono = list1.TopList:GetItem(num2)-- 用于存储下方字的mono
		t.down_mono = nil
		table.insert(can_select_mono, t)
	end
	self.data:SetCanSelectMono(can_select_mono)
end

-- 点击关闭
function CoupletView:OnCloseClick()
	if self.data:GetIsFinish() then
		return
	end
	-- 主线过来的直接关闭不需要提示框
	if self.send_func then
		if self.close_func then
			self.close_func()
		else
			ViewMgr:CloseView(CoupletView)
		end
		return
	end
	local param_t = {
		content = Format(Language.Anecdote.CoupletExitTip,self.data:GetNpcName()),
		confirm = {
			func = function()
				ViewMgr:CloseView(DialogTipsView)
				self.need_show_talk_id = self.data:GetFailTalkId()
				ViewMgr:CloseView(CoupletView)
				self.DownList.gameObject:SetActive(false)
				self.ObjTopBigList:SetActive(false)
			end
		}
	}
	PublicPopupCtrl.Instance:DialogTips(param_t)
end

function CoupletView:CloseCallback()
	TimeHelper:CancelTimer(self.Timer1)
	TimeHelper:CancelTimer(self.Timer2)
	TimeHelper:CancelTimer(self.Timer3)
	TimeHelper:CancelTimer(InteractionAnecdote.ExeTimer)
	ViewMgr:CloseView(DialogTipsView)
	if self.need_show_talk_id ~= -1 then
		if self.need_show_talk_id == self.data:GetFailTalkId() then
			-- 失败重开
			NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.need_show_talk_id})
		else
			-- 成功
			if self.data:GetNowGroup() >= self.data.GROUP_MAX then
				AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.Couplet)
			else
				NpcDialogCtrl.Instance:NpcDialogView(
					{talk_id = self.need_show_talk_id}, 
					function ()
						ViewMgr:OpenView(CoupletView)
					end)
			end
			self.data:SetNowGroup(self.data:GetNowGroup() + 1)
			self.data:SendTaskSpecialReq()
		end
	else
		if self.data:GetIsFinish() then
			if self.send_func then
				self.send_func()
			end
		end
	end
	self.data:DataReset()
end

-- 点击完成按钮
function CoupletView:OnCompleteClick()
	if self.data:GetIsFinish() then
		return
	end
	-- 判断是否已经回答完所有对联了
	if self.data:JudgeIsFinish() then
		self.data:SetAllItemWordState()
		self.DownList.gameObject:SetActive(false)
		local now_group = self.data:GetNowGroup()
		-- 主线过来就不处理该id
		if not self.send_func then
			self.need_show_talk_id = self.data:GetSubjectTalkId(now_group + 1)
		end
		-- 填对后播放特效
		-- 播放下来特效
		self.TopListAnimator:SetBool("is_finish", true)
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
		-- 延时1秒播放扫光特效，主线过来就不播扫光
		if not self.send_func then
			self.Timer1 = TimeHelper:AddDelayTimer(function()
				self.EffectTool:Play(10170011 + now_group)
			end, 1)
		else
			if self.IsHasWord then
				self.Word:SetActive(true)
			end
		end
		-- 延时4秒关闭界面，主线过来就延时2秒
		self.Timer2 = TimeHelper:AddDelayTimer(function()
			if self.IsHasWord then
				self.ObjPanel:SetActive(false)
			end
			self.Word:SetActive(false)
			ViewMgr:CloseView(CoupletView)
		end, (self.send_func and self.IsHasWord == nil) and 2 or 3.5)
	else
		PublicPopupCtrl.Instance:Center(Language.Anecdote.CoupletCompleteTip)
	end
end

---------------------------------CoupletTopList---------------------------------
CoupletTopList = CoupletTopList or DeclareMono("CoupletTopList", UIWidgetBaseItem)

function CoupletTopList:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.TopList:SetCompleteCallBack(function()
		CoupletData.Instance:OneTopListCallBack()
	end)
	self.TopList:SetData(data.list)
end
---------------------------------CoupletTopItem---------------------------------
CoupletTopItem = CoupletTopItem or DeclareMono("CoupletTopItem", UIWidgetBaseItem)

function CoupletTopItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	if data.word then
		if data.word == "," or data.word == "，" then
			self.RectItem:SetSizeWithCurrentAnchors(0, 15)
		end
		self:SetWordText(data.word)
	else
		self.EmptyObj:SetActive(true)
	end
end
-- 给框赋字和字的标号
function CoupletTopItem:SetWordText(str, seq)
	UH.SetText(self.Word1, str)
	UH.SetText(self.Word2, str)
	self.data.now_select = seq or -1
end

function CoupletTopItem:OnItemClick()
	CoupletData.Instance:OnTopItemClick(self)
end
---------------------------------CoupletDownItem---------------------------------
CoupletDownItem = CoupletDownItem or DeclareMono("CoupletDownItem", UIWidgetBaseItem)

function CoupletDownItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	UH.SetText(self.SelectWord, data.word)
	UH.SetText(self.SelectedWord, data.word)
end

-- 设置是否被选中，选中状态要记录下index
function CoupletDownItem:SetUsedItemActive(value, index)
	self.UsedObj:SetActive(value)
	self.data.select_id = index or -1
end

function CoupletDownItem:OnItemClick()
	CoupletData.Instance:OnDownItemClick(self)
end