AnswerData = AnswerData or BaseClass()

AnswerType = {
	Question = 0,
	Result = 1,
}
function AnswerData:__init()
	if AnswerData.Instance ~= nil then
		AnswerData.LogError("[AnswerData] attempt to create singleton twice!")
		return
	end
	AnswerData.Instance = self
	self.sm_data = SmartData.New({phase = AnswerType.Question})
	self.reward_info_data = SmartData.New({add_coin = 0,add_exp = 0})
end

function AnswerData:__delete()
	if AnswerData.Instance == self then
		AnswerData.Instance = nil
	end	
end

function AnswerData:SMData()
	return self.sm_data
end

function AnswerData:ViewPhase()
	return self.sm_data.phase
end

function AnswerData:SetViewPhase(phase)
	self.sm_data.phase = phase
end

function AnswerData:CurQuestionId()
	return self.cur_question_id or 1
end

function AnswerData:CurAnswerNum()
	return self.cur_answer_num or 1
end

function AnswerData:GetAnswerNum()
	return Config.wst_question_cfg_auto.other[1].question_num
end

function AnswerData:GetNpcId()
	return Config.wst_question_cfg_auto.other[1].npc_id
end

function AnswerData:SelectedIdx()
	return self.last_selected or 0
end

function AnswerData:SetSelectedIdx(idx)
	self.last_selected = idx
end

function AnswerData:OnServerData(protocol)
	self.cur_question_id = protocol.question_id
	self.cur_answer_num = protocol.answer_num
end

function AnswerData:GetDataById(question_id)
	for k,v in pairs(Config.wst_question_cfg_auto.question_bank) do
		if v.question_id == question_id then
			return v
		end
	end
end

function AnswerData:GetRewardCfg(role_lv)
	for k,v in pairs(Config.wst_question_cfg_auto.reward) do
		if v.role_level == role_lv then
			return v
		end
	end
end

function AnswerData:GetCurAnswerNum()
	return self:GetAnswerNum() - self:CurAnswerNum()
end

function AnswerData:GetLevel()
	return RoleData.Instance:GetRoleLevel()
end

--找回奖励
function AnswerData:GetFindRewardData()
	local list = {}
	local level = self:GetLevel()
	for k,v in pairs(Config.wst_question_cfg_auto.reward_find) do
		if level >= v.min_level and level <= v.max_level then
			table.insert(list,Item.New({item_id = CommonItemId.Exp,num = v.exp}))
			local config = Config.wst_question_cfg_auto.reward_group[v.group_id] or {}
			local reward = config.reward or {}
			for i=0,#reward do
				if reward[i] then
					table.insert(list,Item.New(reward[i]))
				end
			end
			return list,v.find_type or 0
		end
	end
end

-- 奖励累计获得信息记录
function AnswerData:SetRewardInfo(protocol)
	self.reward_info_data.add_exp = protocol.add_exp
	self.reward_info_data.add_coin = protocol.add_coin
end

function  AnswerData:GetRewardInfo()
	return self.reward_info_data
end