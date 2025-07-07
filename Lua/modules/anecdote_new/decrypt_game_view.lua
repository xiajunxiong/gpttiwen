-- 新奇闻，宝箱解密玩法
DecryptGameView = DecryptGameView or DeclareView("DecryptGameView", "anecdote_new/decrypt_game_view", Mod.AnecdoteNew.Decrypt)
--海底
VIEW_DECLARE_MASK(DecryptGameView, ViewMask.None)
function DecryptGameView:DecryptGameView()
    self.data = CoupletData.Instance
    self.type = NewAnecdoteType.BXJM
end

function DecryptGameView:GetCfg(seq)
    local config = Config.anecdotes_config_auto.decrypt_list
	local cfg = {}
	local seq_cfg
    for i,v in pairs(config) do
        if v.match_seq == seq then
			cfg.lattice = {v}
			seq_cfg = v
        end
    end
    cfg.group_max = 1
    if Config.anecdotes_config_auto.decrypt_word[1].match_seq then
        local group = DataHelper.ConvertGroup(Config.anecdotes_config_auto.decrypt_word, "match_seq")
        cfg.word_group = group[seq]
    else    
        cfg.word_group = Config.anecdotes_config_auto.decrypt_word
    end
    return cfg, seq_cfg
end

function DecryptGameView:LoadCallback()

    local param = AnecdoteData.Instance:GetNewParam(self.type, true)
    if TableIsEmpty(param) then
        LogError("宝箱解密玩法无参数")
        ViewMgr:CloseView(DecryptGameView)
        return
    end
    local seq = param.seq
    self.seq = seq
	local cfg,seq_cfg = self:GetCfg(seq)
    if not cfg then
        LogError("宝箱解密玩法无配置")
        ViewMgr:CloseView(DecryptGameView)
        return
    end

	self.data:DataReset(nil, cfg, 1)
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

	local text_cfg = seq_cfg.riddle
	local text_list = Split(text_cfg, "|")
	for i = 1, self.TextList:Length() do
		local text = text_list[i] or ""
		UH.SetText(self.TextList[i], text)
	end
end

-- 将需要填的字的mono存储起来
function DecryptGameView:SetSelectMono()
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
function DecryptGameView:OnCloseClick()
	if self.data:GetIsFinish() then
		return
	end
	-- local param_t = {
	-- 	content = Language.Anecdote.DecryptExitTip,
	-- 	confirm = {
	-- 		func = function()
	-- 			ViewMgr:CloseView(DialogTipsView)
	-- 			self.need_show_talk_id = self.data:GetFailTalkId()
	-- 			ViewMgr:CloseView(DecryptGameView)
	-- 		end
	-- 	}
	-- }
    -- PublicPopupCtrl.Instance:DialogTips(param_t)
    
    ViewMgr:CloseView(DecryptGameView)
end

function DecryptGameView:CloseCallback()
	TimeHelper:CancelTimer(self.Timer1)
	TimeHelper:CancelTimer(self.Timer2)
	
	-- if self.need_show_talk_id ~= -1 then
	-- 	if self.need_show_talk_id == self.data:GetFailTalkId() then
	-- 		-- 失败重开
	-- 		NpcDialogData.Instance:SetCurTaskInfo(nil)
	-- 		NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.need_show_talk_id})
	-- 	else
	-- 		-- 成功
	-- 		if self.data:GetNowGroup() >= self.data.GROUP_MAX then
    --             LogDG("成功了")
    --             --AnecdoteData.Instance:NewAnecdoteSucc(self.seq)
	-- 		else
	-- 			if self.need_show_talk_id > 0 then
	-- 				NpcDialogCtrl.Instance:NpcDialogView(
	-- 					{talk_id = self.need_show_talk_id}, 
	-- 					function ()
	-- 						ViewMgr:OpenView(DecryptGameView)
	-- 					end)
	-- 			end
	-- 		end
	-- 		self.data:SetNowGroup(self.data:GetNowGroup() + 1)
	-- 		self.data:SendTaskSpecialReq()
	-- 	end
    -- end
    if self.is_finish then
        --LogDG("成功了")
        AnecdoteData.Instance:NewAnecdoteSucc(self.seq)
    end

	self.data:DataReset()
end

-- 点击完成按钮
function DecryptGameView:OnCompleteClick()
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
		--self.need_show_talk_id = self.data:GetSubjectTalkId(now_group + 1)
		-- 播放下来特效
		self.PanelAnimator:SetBool("is_finish", true)
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ComposeSucc)
		-- 延迟3秒播放音效
		self.Timer2 = TimeHelper:AddDelayTimer(function()
			AudioMgr:PlayEffect(AudioType.UI, CommonAudio.PartnerLockOpen)
		end, 3)
		-- 延时5秒关闭界面
		self.Timer1 = TimeHelper:AddDelayTimer(function()
			ViewMgr:CloseView(DecryptGameView)
		end, 5)
		self.is_finish = true
	else
		PublicPopupCtrl.Instance:Center(Language.Anecdote.DecryptFaildTip)
	end
end