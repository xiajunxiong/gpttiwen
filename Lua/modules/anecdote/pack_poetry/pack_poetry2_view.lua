PackPoetry2View = PackPoetry2View or DeclareView("PackPoetry2View", "anecdote/pack_poetry2_view", Mod.Anecdote.PackPoetry2)
VIEW_DECLARE_MASK(PackPoetry2View, ViewMask.None)
function PackPoetry2View:PackPoetry2View()
	self.data = CoupletData.Instance
end

function PackPoetry2View:LoadCallback()
	self.data:DataReset(AnecdoteType.PackPoetry2, Config.pack_poetry_two_auto)
	self.TopBigList:SetData(self.data:GetTopListData())
	self.DownList:SetData(self.data:GetDownListData())

	-- 这里需要大list和小list创建完成才能存储可选字的mono
	self.data:SetTopListCallback(BindTool.Bind1(self.SetSelectMono, self))

	-- 用于在关闭时显示的对话id
	self.need_show_talk_id = -1
	-- 用于对联扫光
	self.Timer1 = nil
	-- 用于关闭界面
	self.Timer2 = nil
	-- 用于显示对联和下方字
	self.Timer3 = TimeHelper:AddDelayTimer(function()
		self.ObjTopBigList:SetActive(true)
		self.DownList.gameObject:SetActive(true)
	end, 1)
end

-- 将需要填的字的mono存储起来
function PackPoetry2View:SetSelectMono()
	local can_select_mono = {}
	for i, v in ipairs(self.data:GetCanSelectIdList()) do
		local num1 = math.floor(v.pos / 100)
		local num2 = v.pos % 100
		local list1 = self.TopBigList:GetItem(num1)
		local t = {}
		t.top_mono = list1.TopList:GetItem(num2)
		-- 用于存储下方字的mono
		t.down_mono = nil
		table.insert(can_select_mono, t)
	end
	self.data:SetCanSelectMono(can_select_mono)
end

-- 点击关闭
function PackPoetry2View:OnCloseClick()
	if self.data:GetIsFinish() then
		return
	end
	local param_t = {
		content = Language.Anecdote.PackPoetryExitTip2,
		confirm = {
			func = function()
				ViewMgr:CloseView(DialogTipsView)
				self.need_show_talk_id = self.data:GetFailTalkId()
				ViewMgr:CloseView(PackPoetry2View)
				self.DownList.gameObject:SetActive(false)
				self.ObjTopBigList:SetActive(false)
			end
		}
	}
	PublicPopupCtrl.Instance:DialogTips(param_t)
end

function PackPoetry2View:CloseCallback()
	TimeHelper:CancelTimer(self.Timer1)
	TimeHelper:CancelTimer(self.Timer2)
	TimeHelper:CancelTimer(self.Timer3)
	
	if self.need_show_talk_id ~= -1 then
		if self.need_show_talk_id == self.data:GetFailTalkId() then
			-- 失败重开
			NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.need_show_talk_id})
		else
			-- 成功
			if self.data:GetNowGroup() >= self.data.GROUP_MAX then
				AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.PackPoetry2)
			else
				NpcDialogCtrl.Instance:NpcDialogView(
					{talk_id = self.need_show_talk_id}, 
					function ()
						ViewMgr:OpenView(PackPoetry2View)
					end)
			end
			self.data:SetNowGroup(self.data:GetNowGroup() + 1)
			self.data:SendTaskSpecialReq()
		end
	end

	self.data:DataReset()
end

-- 点击完成按钮
function PackPoetry2View:OnCompleteClick()
	if self.data:GetIsFinish() then
		return
	end
	-- 判断是否已经回答完所有对联了
	if self.data:JudgeIsFinish() then
		self.data:SetAllItemWordState()
		self.DownList.gameObject:SetActive(false)
		local now_group = self.data:GetNowGroup()
		self.need_show_talk_id = self.data:GetSubjectTalkId(now_group + 1)
		-- 填对后播放特效
		-- 播放下来特效
		self.TopListAnimator:SetBool("is_finish", true)
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
		-- 延时1秒播放扫光特效
		self.Timer1 = TimeHelper:AddDelayTimer(function()
			self.EffectTool:Play(100108206 + now_group)
		end, 1)
		-- 延时4秒关闭界面
		self.Timer2 = TimeHelper:AddDelayTimer(function()
			self.ObjPanel:SetActive(false)
			ViewMgr:CloseView(PackPoetry2View)
		end, 4)
	else
		PublicPopupCtrl.Instance:Center(Language.Anecdote.PackPoetryFaildTip)
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
	end
end