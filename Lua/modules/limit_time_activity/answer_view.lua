AnswerView = AnswerView or DeclareView("AnswerView","answer", Mod.Answer.AnswerView)

function AnswerView:LoadCallback()
	AnswerData.Instance:SetViewPhase(AnswerType.Question)
end

function AnswerView:CloseCallback()
	AnswerData.Instance:SetViewPhase(AnswerType.Question)
end

function AnswerView:OnClickClose()
	ViewMgr:CloseView(AnswerView)
end

AnswerViewPanel = AnswerViewPanel or DeclareMono("AnswerViewPanel", UIWFlushPanel)

function AnswerViewPanel:AnswerViewPanel()
	self.data = AnswerData.Instance
    self.data_cares = {
		{data = self.data:SMData(), func = self.OnPhaseChanged,keys = {"phase"}},
		{data = self.data.reward_info_data, func = self.FlushRewardInfo,init_call = true}
   }
   self.left_tip_time = 0
   self.tip_timer = nil
   self.correct_show_timer = nil
   self.next_questioni_timer = nil
   self.is_ctrl_ing = false 		-- 是否在操作中
end

function AnswerViewPanel:OnPhaseChanged()
	if self.data:ViewPhase() == AnswerType.Question then
		self:OnQuestionPhase()
	elseif self.data:ViewPhase() == AnswerType.Result then
		self:OnResultPhase()
	end
end

function AnswerViewPanel:OnQuestionPhase()
	local answer_num = self.data:CurAnswerNum()
	local max_num = self.data:GetAnswerNum()
	if answer_num >= max_num then
		return
	end
	local question_id = self.data:CurQuestionId()
	self.EveBlock:SetActive(false)
	-- self.Progress.text = Format("%d/%d",answer_num + 1, max_num)
	-- local question_count_str = DataHelper.GetDaXie(answer_num + 1 > max_num and max_num or answer_num + 1)
	-- self.Title.text = Format(Language.Answer.title,question_count_str)
	local progress_str = string.format("（%d/%d）",answer_num + 1, max_num)
	UH.SetText(self.Title,string.format("%s%s",Language.Answer.title2,progress_str))
	local qest_cfg = self.data:GetDataById(question_id)
	if qest_cfg ~= nil then
		self.Question.text = qest_cfg.question
		for i=1,4 do
			self.AnswerBtn[i]:SetActive(qest_cfg["anwser_" .. i] ~= "")
			self.TextList[i].text = qest_cfg["anwser_" .. i]
		end
	end

	local reward_cfg = self.data:GetRewardCfg(RoleData.Instance:GetRoleLevel())
	if reward_cfg ~= nil then
		self.RewardGoldNum.text = Format("+%s",tostring(reward_cfg.coin))
		self.RewardExpNum.text = Format("+%s",tostring(reward_cfg.exp))
		self.RewardExpNum:SetObjActive(reward_cfg.exp ~= 0)
	end
	self:SetAllGobjFailse()

	self.left_tip_time = 10
	if self.tip_timer ~= nil then
		TimeHelper:CancelTimer(self.tip_timer)
	end
	self.tip_timer = TimeHelper:AddCountDownTT(function() 
		self.Tip.text = Format(Language.Answer.tip,self.left_tip_time)
		self.left_tip_time = self.left_tip_time - 1
	end,function() 
		self.Tip.text = qest_cfg.prompt
		self.tip_timer = nil
	end,10, 1,true)
	self.is_ctrl_ing = false
end

function AnswerViewPanel:SetAllGobjFailse(correct_index,save_fail)
	for i=1,4 do
		self.CorrectList[i]:SetActive(i == correct_index)
		if not save_fail then
			self.FailList[i]:SetActive(false)
		end
	end
end

function AnswerViewPanel:GetGObj(index)
	return self.FailList[index],self.CorrectList[index]
end

function AnswerViewPanel:OnResultPhase()
	self.EveBlock:SetActive(true)
	if self.tip_timer ~= nil then
		TimeHelper:CancelTimer(self.tip_timer)
		self.tip_timer = nil
	end
	local sel = self.data:SelectedIdx()
	local question_id = self.data:CurQuestionId()
	local answer_num = self.data:CurAnswerNum()
	local quest_cfg = self.data:GetDataById(question_id)
	if quest_cfg == nil then
		return
	end
	if self.next_questioni_timer ~= nil then
		TimeHelper:CancelTimer(self.next_questioni_timer)
		self.next_questioni_timer = nil
	end
	if sel == quest_cfg.answer_id then
		local fail_obj,correct_obj = self:GetGObj(sel)
		correct_obj:SetActive(true)
		self.next_questioni_timer = TimeHelper:AddDelayTimer(function ()
			self.data:SetViewPhase(AnswerType.Question)
		end,1)
		-- local reward_cfg = self.data:GetRewardCfg(RoleData.Instance:GetRoleLevel())
		-- PublicPopupCtrl.Instance:Reward2({Item.New({item_id = CommonItemId.Coin,num = reward_cfg.coin}),
		-- 	Item.New({item_id = CommonItemId.Exp,num = reward_cfg.exp})},"",function() 
		-- 	self.data:SetViewPhase(AnswerType.Question)
		-- end)
	else
		self.RetHintTxt.text = Language.Answer.Wrong
		self.RetHintAnim:SetTrigger(APH("show"))

		local fail_obj,correct_obj = self:GetGObj(sel)
		fail_obj:SetActive(true)
		if self.correct_show_timer ~= nil then
			TimeHelper:CancelTimer(self.correct_show_timer)
			self.correct_show_timer = nil
		end
		self.correct_show_timer = TimeHelper:AddDelayTimer(function ()
			self.correct_show_timer = nil
			local fail_obj_c,correct_obj_c = self:GetGObj(quest_cfg.answer_id)
			correct_obj_c:SetActive(true)
			--1秒后显示正确答案
			self.next_questioni_timer = TimeHelper:AddDelayTimer(function ()
			--  #43639 需求 G6-Line2（新版本开发线-2） 日常：神算子的考验优化-客户端需求(修改)
			-- 	local reward_cfg = self.data:GetRewardCfg(RoleData.Instance:GetRoleLevel())
			-- 	PublicPopupCtrl.Instance:Reward2({Item.New({item_id = CommonItemId.Coin,num = reward_cfg.coin/2}),
			-- 		Item.New({item_id = CommonItemId.Exp,num = reward_cfg.exp})},Language.Answer.WrongRewardTip,function() 
			-- 			self.data:SetViewPhase(AnswerType.Question)
			-- 		end)
				self.data:SetViewPhase(AnswerType.Question)
			end,1)
		end,1)
	end
end

function AnswerViewPanel:OnDestroy()
	if self.tip_timer ~= nil then
		TimeHelper:CancelTimer(self.tip_timer)
		self.tip_timer = nil
	end
	if self.next_questioni_timer ~= nil then
		TimeHelper:CancelTimer(self.next_questioni_timer)
		self.next_questioni_timer = nil
	end
	if self.correct_show_timer ~= nil then
		TimeHelper:CancelTimer(self.correct_show_timer)
		self.correct_show_timer = nil
	end
	UIWFlushPanel.OnDestroy(self)
end

function AnswerViewPanel:OnClickCheck(index)
	if self.is_ctrl_ing == true then
		return
	end
	self.is_ctrl_ing = true
	AnswerData.Instance:SetSelectedIdx(index)
	LimitTimeActivityCtrl.Instance:SendAnswer(1, index)
	AnswerData.Instance:SetViewPhase(AnswerType.Result)
end

function AnswerViewPanel:FlushRewardInfo()
	local info_data = self.data:GetRewardInfo()
	UH.SetText(self.RewardCoinText,info_data.add_coin)
	UH.SetText(self.RewardExpText,info_data.add_exp)
	self.RewardExp:SetActive(info_data.add_exp ~= 0)
end