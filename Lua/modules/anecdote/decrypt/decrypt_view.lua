DecryptView = DecryptView or DeclareView("DecryptView", "anecdote/decrypt_view", Mod.Anecdote.Decrypt)
VIEW_DECLARE_MASK(DecryptView, ViewMask.None)
function DecryptView:DecryptView()
	self.data = CoupletData.Instance
end

function DecryptView:LoadCallback()
	self.data:DataReset(AnecdoteType.Decrypt, Config.decrypt_auto)
	self.TopBigList:SetData(self.data:GetTopListData())
	self.DownList:SetData(self.data:GetDownListData())

	-- 这里需要大list和小list创建完成才能存储可选字的mono
	self.data:SetTopListCallback(BindTool.Bind1(self.SetSelectMono, self))

	-- 用于在关闭时显示的对话id
	self.need_show_talk_id = -1
	-- 用于播放宝箱打开动画
	self.Timer1 = nil
	-- 用于播放宝箱打开音效
	self.Timer2 = nil
end

-- 将需要填的字的mono存储起来
function DecryptView:SetSelectMono()
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
function DecryptView:OnCloseClick()
	if self.data:GetIsFinish() then
		return
	end
	local param_t = {
		content = Language.Anecdote.DecryptExitTip,
		confirm = {
			func = function()
				ViewMgr:CloseView(DialogTipsView)
				self.need_show_talk_id = self.data:GetFailTalkId()
				ViewMgr:CloseView(DecryptView)
			end
		}
	}
	PublicPopupCtrl.Instance:DialogTips(param_t)
end

function DecryptView:CloseCallback()
	TimeHelper:CancelTimer(self.Timer1)
	TimeHelper:CancelTimer(self.Timer2)
	
	if self.need_show_talk_id ~= -1 then
		if self.need_show_talk_id == self.data:GetFailTalkId() then
			-- 失败重开
			NpcDialogData.Instance:SetCurTaskInfo(nil)
			NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.need_show_talk_id})
		else
			-- 成功
			if self.data:GetNowGroup() >= self.data.GROUP_MAX then
				AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.Decrypt)
			else
				if self.need_show_talk_id > 0 then
					NpcDialogCtrl.Instance:NpcDialogView(
						{talk_id = self.need_show_talk_id}, 
						function ()
							ViewMgr:OpenView(DecryptView)
						end)
				end
			end
			self.data:SetNowGroup(self.data:GetNowGroup() + 1)
			self.data:SendTaskSpecialReq()
		end
	end

	self.data:DataReset()
end

-- 点击完成按钮
function DecryptView:OnCompleteClick()
	if self.data:GetIsFinish() then
		return
	end
	if self.data:JudgeIsHasSpace() then
		PublicPopupCtrl.Instance:Center(Language.Anecdote.DecryptHasSpaceTip)
		return
	end
	-- 判断是否已经回答完所有对联了
	if self.data:JudgeIsFinish() then
		self.data:SetAllItemWordState()
		local now_group = self.data:GetNowGroup()
		self.need_show_talk_id = self.data:GetSubjectTalkId(now_group + 1)
		-- 播放下来特效
		self.PanelAnimator:SetBool("is_finish", true)
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ComposeSucc)
		-- 延迟3秒播放音效
		self.Timer2 = TimeHelper:AddDelayTimer(function()
			AudioMgr:PlayEffect(AudioType.UI, CommonAudio.PartnerLockOpen)
		end, 3)
		-- 延时5秒关闭界面
		self.Timer1 = TimeHelper:AddDelayTimer(function()
			ViewMgr:CloseView(DecryptView)
		end, 5)
	else
		PublicPopupCtrl.Instance:Center(Language.Anecdote.DecryptFaildTip)
	end
end