-- 新奇闻天下绝对，对联玩法
CoupletGameView = CoupletGameView or DeclareView("CoupletGameView", "anecdote_new/couplet_game_view", Mod.AnecdoteNew.Couplet)

-- key = seq, value = group_num
CoupletGameView.group_data = {

}

function CoupletGameView:CoupletGameView()
	self.data = CoupletData.Instance
	self.effect_id_list = {
		[1] = 7165507,
		[2] = 7165508,
		[3] = 7165509,
    }
    self.type = NewAnecdoteType.TXJD
end

function CoupletGameView:GetCfg(seq)
    local config = Config.anecdotes_config_auto.couplet_list
    local group = DataHelper.ConvertGroup(config, "match_seq")
    local lattice = group[seq]
    local group_2 = DataHelper.ConvertGroup(Config.anecdotes_config_auto.couplet_word, "match_seq")
    local word_group = group_2[seq]
    local max_gourp = DataHelper.ConvertGroup(word_group, "group")
    local cfg = {
        lattice = lattice,
        word_group = word_group,
        group_max = #max_gourp,
    }
    return cfg
end

function CoupletGameView:OnSendFunc()
    -- 发送成功请求
    AnecdoteData.Instance:NewAnecdoteSucc(self.seq)
end

function CoupletGameView:LoadCallback(param_t)
    local param = AnecdoteData.Instance:GetNewParam(self.type, false)
    if TableIsEmpty(param) then
        LogError("天下绝对玩法无参数")
        ViewMgr:CloseView(CoupletGameView)
        return
    end
    local seq = param.seq
    self.seq = seq
    local cfg = self:GetCfg(seq)
    if not cfg then
        LogError("天下绝对玩法无配置")
        ViewMgr:CloseView(CoupletGameView)
        return
    end

    if not CoupletGameView.group_data[self.seq] then
        CoupletGameView.group_data[seq] = 1
    end
    local group_num = CoupletGameView.group_data[seq]
	self.data:DataReset(nil, cfg, group_num)
	self.TopBigList:SetData(self.data:GetTopListData())
	self.DownList:SetData(self.data:GetDownListData())
	self.data:SetTopListCallback(BindTool.Bind1(self.SetSelectMono, self))
	self.Timer3 = TimeHelper:AddDelayTimer(BindTool.Bind1(self.AddDelayTimer, self), 1)
	--AnecdoteData.Instance:CustomAutoHandle(AnecdoteType.Couplet3, BindTool.Bind1(self.OnAutoFunc, self))
	
	local img_type = param_t.img_type or 0
	UH.SpriteName(self.ImgLeft, [[Left]] .. img_type)
	UH.SpriteName(self.ImgRight,[[_LocRight]] .. img_type)
	UH.SpriteName(self.ImgComplete,[[_LocComplete]] .. img_type)

	self.send_func = BindTool.Bind(self.OnSendFunc, self)
	self.close_func = param_t.close_func
	self.need_show_talk_id = -1
	self:FlushFinishWord()
end

function CoupletGameView:FlushFinishWord()
	for i=1,self.WordList:Length() do
		local word = self.data:GetFinishWord(i-1)
		UH.SetText(self.WordList[i],word or "")
		if word ~= nil then
			self.IsHasWord = true
		end
	end
end

function CoupletGameView:AddDelayTimer()
	self.ObjTopBigList:SetActive(true)
	self.DownList.gameObject:SetActive(true)
end

--主线任务自动完成行为
-- function CoupletGameView:OnAutoFunc()
-- 	if self.data:GetIsFinish() then
-- 		return
-- 	end
-- 	ViewMgr:CloseView(DialogTipsView)
-- 	self.data:FastFinish()
-- 	self:OnCompleteClick(CoupletGameView)
-- end

-- 将需要填的字的mono存储起来
function CoupletGameView:SetSelectMono()
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
function CoupletGameView:OnCloseClick()
	if self.data:GetIsFinish() then
		return
	end
	-- 主线过来的直接关闭不需要提示框
	if self.send_func then
		if self.close_func then
			self.close_func()
		else
			ViewMgr:CloseView(CoupletGameView)
		end
		return
	end
	local param_t = {
		content = Format(Language.Anecdote.CoupletExitTip,self.data:GetNpcName()),
		confirm = {
			func = function()
				ViewMgr:CloseView(DialogTipsView)
				self.need_show_talk_id = self.data:GetFailTalkId()
				ViewMgr:CloseView(CoupletGameView)
				self.DownList.gameObject:SetActive(false)
				self.ObjTopBigList:SetActive(false)
			end
		}
	}
	PublicPopupCtrl.Instance:DialogTips(param_t)
end

function CoupletGameView:CloseCallback()
	TimeHelper:CancelTimer(self.Timer1)
	TimeHelper:CancelTimer(self.Timer2)
	TimeHelper:CancelTimer(self.Timer3)
	TimeHelper:CancelTimer(InteractionAnecdote.ExeTimer)
	ViewMgr:CloseView(DialogTipsView)
	-- if self.need_show_talk_id ~= -1 then
	-- 	if self.need_show_talk_id == self.data:GetFailTalkId() then
	-- 		-- 失败重开
	-- 		NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.need_show_talk_id})
	-- 	else
	-- 		-- 成功
    --         if self.data:GetNowGroup() >= self.data.GROUP_MAX then
	-- 			AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.Couplet3)
	-- 		else
	-- 			NpcDialogCtrl.Instance:NpcDialogView(
	-- 				{talk_id = self.need_show_talk_id}, 
	-- 				function ()
	-- 					ViewMgr:OpenView(CoupletGameView)
	-- 				end)
	-- 		end
	-- 		self.data:SetNowGroup(self.data:GetNowGroup() + 1)
	-- 		self.data:SendTaskSpecialReq()
	-- 	end
	-- else
	-- 	if self.data:GetIsFinish() then
	-- 		if self.send_func then
	-- 			self.send_func()
	-- 		end
	-- 	end
    -- end
    
    if self.data:GetIsFinish() then
        if CoupletGameView.group_data[self.seq] >= self.data.GROUP_MAX then
            if self.send_func then
                self.send_func()
            end
        else
            CoupletGameView.group_data[self.seq] = CoupletGameView.group_data[self.seq] + 1
            ViewMgr:OpenView(CoupletGameView)
        end
    end

	self.data:DataReset()
end

-- 点击完成按钮
function CoupletGameView:OnCompleteClick()
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
				self.EffectTool:Play(self.effect_id_list[now_group])
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
			ViewMgr:CloseView(CoupletGameView)
		end, (self.send_func and self.IsHasWord == nil) and 2 or 3.5)
	else
		PublicPopupCtrl.Instance:Center(Language.Anecdote.CoupletCompleteTip)
	end
end