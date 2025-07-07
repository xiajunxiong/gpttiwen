RoleSkillData = RoleSkillData or BaseClass() 

RoleSkillData.RoleXiuLianIconIndex = {
	[1001] = 7,
	[1002] = 7,
	[1003] = 7,
	[2001] = 8,
	[2002] = 8,
	[2003] = 8,
}

function RoleSkillData:__init()
	if RoleSkillData.Instance ~= nil then
        Debuger.LogError("[RoleSkillData] attempt to create singleton twice!")
        return
    end
	RoleSkillData.Instance = self
	
 	self.smart_data = SmartData.New({current_train_pos = 0, current_train_type = 0, 
 		flush_promoted = true, first_learn = false, 
 		xiulian_remind = false, assist_remind = false})
 	self.current_train_data = SmartData.New({train_type = 0,level = 0,exp = 0})
	self.cur_select_cost_type = SmartData.New({typeIndex = 1})
	self.train_red_dot_flag = SmartData.New()
	self.use_glass_tripod = false
	self.promoted_info = {}
	self.show_list = {}
	self.assist_list = {}
	self.prof_skill_red_notice = SmartData.New({clicked = false})

	self:initConfig()
end

function RoleSkillData:initConfig()
	for k,v in pairs(Config.train_auto.train_list) do
		if v.is_show == 1 then
			if v.skill_type == 0 then
				table.insert(self.assist_list, v)
			else
				table.insert(self.show_list, v)
			end
		end
	end
	function sort_func(a, b)
		return a.sn < b.sn
	end
	table.sort(self.assist_list, sort_func)
	table.sort(self.show_list, sort_func)
	--对train_type操作，根据技能类型、职业、技能等级序列化并赋值给表
	local train_type_attr_value_list = {}
	for k,v in pairs(Config.train_auto.train_type) do
		train_type_attr_value_list[v.train_type] = train_type_attr_value_list[v.train_type] or {}
		local job_id = v.job_id / 100
		train_type_attr_value_list[v.train_type][job_id] = train_type_attr_value_list[v.train_type][job_id] or {}
		train_type_attr_value_list[v.train_type][job_id][v.train_level] = v
	end
	Config.train_auto.train_type = train_type_attr_value_list
	----

	--对unlock_train_level操作，根据技能类型序列化并赋值给表
	local unlock_train_level_t = {}
	for k,v in ipairs(Config.train_auto.unlock_train_level) do
		unlock_train_level_t[v.train_type] = unlock_train_level_t[v.train_type] or {}
		table.insert(unlock_train_level_t[v.train_type], v)
	end
	Config.train_auto.unlock_train_level = unlock_train_level_t
	----

	self.train_type_list = {}
	for k, v in ipairs(Config.train_auto.character_train) do
		self.train_type_list[v.train_type] = self.train_type_list[v.train_type] or {}
		self.train_type_list[v.train_type][v.train_level] = v
	end
	local pet_train_unlock = {}
	for k, v in pairs(Config.train_auto.pet_train) do
		self.train_type_list[1000] = self.train_type_list[1000] or {}
		self.train_type_list[1000][v.skill_level] = v
		table.insert(pet_train_unlock, v)
	end

	local pet_attr_train_unlock = {}
	for k,v in pairs(Config.train_auto.pet_tattr_rain) do
		self.train_type_list[v.train_type] = self.train_type_list[v.train_type] or {}
		self.train_type_list[v.train_type][v.train_level] = v

		pet_attr_train_unlock[v.train_type] = pet_attr_train_unlock[v.train_type] or {}
		table.insert(pet_attr_train_unlock[v.train_type], v)
	end
	for k,v in pairs(pet_attr_train_unlock) do
		Config.train_auto.unlock_train_level[k] = v
	end

	local partner_attr_train_unlock = {}
	for k,v in pairs(Config.train_auto.partner_tattr_rain) do
		self.train_type_list[v.train_type] = self.train_type_list[v.train_type] or {}
		self.train_type_list[v.train_type][v.train_level] = v

		partner_attr_train_unlock[v.train_type] = partner_attr_train_unlock[v.train_type] or {}
		table.insert(partner_attr_train_unlock[v.train_type], v)
	end
	for k,v in pairs(partner_attr_train_unlock) do
		Config.train_auto.unlock_train_level[k] = v
	end

	Config.train_auto.unlock_train_level[1000] = pet_train_unlock

	Cfg.Register("RolePetTrainLevel", Cfg.CacheGet(Config.train_auto.pet_train, "skill_level"))
	Cfg.Register("RolePartnerAttrTrainLevel", Cfg.RealTimeSearch2Key(Config.train_auto.partner_tattr_rain,"train_type", "train_level"))
	Cfg.Register("RolePetAttrTrainLevel", Cfg.RealTimeSearch2Key(Config.train_auto.pet_tattr_rain, "train_type", "train_level"))
	Cfg.Register("RoleXiuLianCharcterLevel",Cfg.RealTimeSearch2Key(Config.train_auto.character_train,"train_type","train_level"))
	Cfg.Register("RoleXiuLianDescByType",Cfg.CacheGet(Config.train_auto.train_list, "train_type"))
end

function RoleSkillData:GetAllowMaxLevel(role_level, train_type)
	local config = Config.train_auto.unlock_train_level[train_type]
	if config == nil then return 1 end
	local num = #config
	while(num > 0) do
		--角色修炼（修炼+辅助）character_train
		if train_type <= 999 then
			if config[num].character_level <= role_level then
				return config[num].train_level, config[num + 1]
			elseif config[num].train_level == 1 then
				return 0
			end

		--宠物修炼（辅助）pet_train
		elseif train_type == 1000 then
			if config[num].skill_learn_level <= role_level then
				return config[num].skill_level, config[num + 1]
			elseif config[num].skill_level == 1 then
				return 0
			end

		--宠物及伙伴修炼（辅助）pet_tattr_rain + partner_tattr_rain
		else
			if config[num].need_role_level <= role_level then
				return config[num].train_level, config[num + 1]
			elseif config[num].train_level == 1 then
				return 0
			end
		end
		num = num - 1
	end
	return 1
end

-- 用于打开技能页签自动选中该职业技能id，只适用职业技能
function RoleSkillData:SetNeedJumpSkillId(value)
	self.need_jump_skill_id = value
end

function RoleSkillData:GetNeedJumpSkillId()
	return self.need_jump_skill_id
end

-- 用于技能页面的toggle勾选
function RoleSkillData:ToggleAllLearn(value)
	if value then
		PrefsInt(PrefKeys.RoleSkillAllLearn(), value)
	else
		return PrefsInt(PrefKeys.RoleSkillAllLearn())
	end
end

--0=辅助技能
--1=修炼技能
function RoleSkillData:GetTrainCanLearn(train_type, cur_level, role_level)
	role_level = role_level or RoleData.Instance:GetBaseData().level
	local config = Config.train_auto.unlock_train_level
	if config[train_type] then
	for i,v in ipairs(config[train_type]) do
		if v.train_level >= (cur_level + 1) then
				if v.character_level then
			if role_level >= v.character_level then
				return true, v.character_level
			else
				return false, v.character_level
			end
				elseif v.skill_learn_level then
					if role_level >= v.skill_learn_level then
						return true, v.skill_learn_level
					else
						return false, v.skill_learn_level
		end
				else
					if role_level >= v.need_role_level then
						return true, v.need_role_level
					else
						return false, v.need_role_level
	end
				end
			end
		end
	end
end

function RoleSkillData:CanTrainIndex()
	local can_index = 0
	local step_index = 1
	local item_num = Item.GetNum(Config.train_auto.other[1].item_id)
	for k,v in pairs(self.show_list) do
		if RoleSkillData.Instance:GetTrainTypeRemind(v.train_type, item_num) > 0 then
			if self:GetReddotNoticeFlag(v.train_type) then
				--LogError("notice train step_index", step_index)
				return step_index
			elseif can_index == 0 then
				can_index = step_index
			end
		end
		step_index = step_index + 1
	end
	--LogError("can_index",can_index)
	return can_index == 0 and 1 or can_index
end


function RoleSkillData:SetAllInfo(protocol)
	self.smart_data.trainData = protocol.trainData
	RoleSkillData.Instance.smart_data.first_learn = false
	self:SetCurTrainData()
	self:DoXiuLianRemind()
	if self.xiulian_item_care == nil then
		self.xiulian_item_care = BagData.Instance.item_grid[ItemColumnType.Item]:Care(BindTool.Bind1(self.DoXiuLianRemind, self))
	end
end

function RoleSkillData:SetCurTrainType(train_type)
	self.current_train_data.train_type = train_type
end

function RoleSkillData:GetTrainAttrName(train_type, job_id)
	local train_cfg = Config.train_auto.train_type[train_type]
	if train_cfg then
		local train_job_cfg = train_cfg[job_id]
		if train_job_cfg and train_job_cfg[1] then
			local attr_type = train_job_cfg[1].attribute_type
			return Language.Role.RoleSkillAttrList[attr_type]
		-- else
		-- 	LogError("train_job_cfg", job_id,train_job_cfg)
		end
	-- else
	-- 	LogError("train_type",train_type)
	end
end

function RoleSkillData:GetTrainAttrValue(train_type, job_id, train_level)
	local train_cfg = Config.train_auto.train_type[train_type]
	if train_cfg then
		local train_job_cfg = train_cfg[job_id]
		if train_job_cfg and train_job_cfg[train_level] then
			return train_job_cfg[train_level]
		-- else
		-- 	LogError("train_job_cfg",job_id, train_level)
		end
	-- else
	-- 	LogError("train_type",train_type)
	end
end

function RoleSkillData:TrainSkillCanAndNoActive()
	local role_level = RoleData.Instance:GetRoleLevel()
	for k,v in pairs(self.show_list) do
		local train_data = self:getTrainDataByType(v.train_type)
		local canLearn = self:GetTrainCanLearn(v.train_type, train_data.level, role_level)
		if train_data.level > 0 or not canLearn then
			return false
		end
	end
	return true
end

--仅能获得character_train分表的
function RoleSkillData:GetTrainTypeIsOpen(train_type)
	local role_level = RoleData.Instance:GetRoleLevel()
	local limit_level = self:GetTrainTypeOpenLevel(train_type)
	return role_level >= limit_level, string.format(ErrorText[90], limit_level)
end

--仅能获得character_train分表的
function RoleSkillData:GetTrainTypeOpenLevel(train_type)
	local train_open_cfg = Config.train_auto.unlock_train_level[train_type]
	return train_open_cfg and train_open_cfg[1].character_level or 0
end

function RoleSkillData:SetCurTrainData(train_type)
	local train_data = {}
	if train_type ~= nil then
		train_data = self:getTrainDataByType(train_type)
	else
		train_data = self:GetCurTrainData()
	end
	if train_data == nil then
		return
	end
	self.current_train_data:Set(train_data)
	-- self.current_train_data.train_type = train_data.train_type
	-- self.current_train_data.level = train_data.level
	-- self.current_train_data.exp = train_data.exp
end

function RoleSkillData:GetCurTrainData()
	if self.current_train_data.train_type ~= nil then
		return self:getTrainDataByType(self.current_train_data.train_type)
	-- else
	-- 	LogError("self.current_train_data.train_type = nil")
	end
end

function RoleSkillData:getTrainDataByType(train_type)
	if self.smart_data.trainData then
	return self.smart_data.trainData[train_type]
	end
end

function RoleSkillData:GetRolePetXiulianLevel()
	return self:getTrainDataByType(1000).level
end

function RoleSkillData:GetCharacterData(train_type)
	return self.train_type_list[train_type]
end

function RoleSkillData:GetCharacterLevelData(train_type, level)
	if train_type == 1000 then
		return Cfg.RolePetTrainLevel(level)
	elseif train_type > 2000 then
		return Cfg.RolePartnerAttrTrainLevel(train_type,level)
	elseif train_type > 1000 then
		return Cfg.RolePetAttrTrainLevel(train_type,level)
	else
		local train_attr_value_cfg = self:GetTrainAttrValue(train_type, RoleData.Instance:ProfType(), level)
		local train_cfg = Cfg.RoleXiuLianCharcterLevel(train_type, level)
		if train_attr_value_cfg then
			train_cfg.attribute_value = train_attr_value_cfg.attribute_value
			train_cfg.attr_name = Language.Role.RoleSkillAttrList[train_attr_value_cfg.attribute_type]
		end
		return Cfg.RoleXiuLianCharcterLevel(train_type, level)
	end
end

function RoleSkillData:DoXiuLianRemind()
	SkillData.Instance.change_data:SetDirty("red_dot_flag_change")
	self.smart_data:SetDirty("xiulian_remind")
	BianQiangData.Instance:FlushListRemind(BianQiangData.ListType.XiuLianJiNeng)
end

function RoleSkillData:GetXiulianSkillRemind()
	local role_level = RoleData.Instance:GetRoleLevel()
	local item_num = Item.GetNum(Config.train_auto.other[1].item_id)
	for k,v in pairs(Config.train_auto.train_list) do
		if v.is_show == 1 and v.skill_type == 1 then
			local fun_is_opend = false
			if v.train_type == 1000 then
				local pet_train_cfg = self:GetCharacterLevelData(1000, 1)
				fun_is_opend = role_level >= pet_train_cfg.skill_learn_level
			elseif v.train_type > 2000 then
				fun_is_opend = role_level >= Cfg.RolePartnerAttrTrainLevel(v.train_type,1).need_role_level
			elseif v.train_type > 1000 then
				fun_is_opend = role_level >= Cfg.RolePetAttrTrainLevel(v.train_type,1).need_role_level
			else
				fun_is_opend = role_level >= self:GetTrainTypeOpenLevel(v.train_type)
			end
			if fun_is_opend then
					local notice_flag = RoleSkillData.Instance:GetReddotNoticeFlag(v.train_type)
					if notice_flag and item_num > 0 and RoleSkillData.Instance:GetTrainTypeRemind(v.train_type, item_num) > 0 then
						return 1
					end
				end
			end
		end
	return self:GetAssistSkillRemind()
end

function RoleSkillData:GetAssistSkillRemind()
	local role_level = RoleData.Instance:GetRoleLevel()
	local has_coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind) or 0
	for k,v in pairs(Config.train_auto.train_list) do
		if v.is_show == 1 and v.skill_type == 0 then
			local level = self:GetAllowMaxLevel(role_level, v.train_type)
			local skill_data = self:getTrainDataByType(v.train_type)
			if level > 0 and skill_data and skill_data.level and level > skill_data.level then
				local assist_cfg_next = RoleSkillData.Instance:GetCharacterLevelData(v.train_type, skill_data.level + 1)
				local need_times = math.ceil((assist_cfg_next.need_exp - skill_data.exp) / assist_cfg_next.get_exp)
				need_times = need_times > 0 and need_times or 1
				if assist_cfg_next and assist_cfg_next.bind_coin_consume * need_times <= has_coin then
					return 1
	end
			end
		end
	end

	return 0
end
--仅用于获得character_train中的
function RoleSkillData:GetTrainTypeRemind(train_type, item_num)
	local allowMaxLevel = self:GetAllowMaxLevel(RoleData.Instance:GetRoleLevel(), train_type)
	local train_data = self:getTrainDataByType(train_type)
	if allowMaxLevel <= train_data.level then
		return 0
	end
	local train_level_cfg = self.train_type_list[train_type][train_data.level + 1]
	if train_level_cfg then
		return item_num >= train_level_cfg.train_ball_consume and 1 or 0
	else
		return 0
	end
end

function RoleSkillData.GetNameAndDesc(train_type)
	local config = Config.train_auto.train_list
	if config == nil then return end
	return Cfg.RoleXiuLianDescByType(train_type)
end

--手动屏蔽前2个修炼类型(共7个，取3~7)
function RoleSkillData:GetAssistConfig()
	return self.assist_list
end

function RoleSkillData:GetShowConfig()
	return self.show_list
end
function RoleSkillData:SetTrainPromotedInfo(info)
	self.promoted_info[2] = info.physical_dec_damage
	self.promoted_info[3] = info.magic_dec_damage
	self.smart_data.flush_promoted = not self.smart_data.flush_promoted
end
function RoleSkillData:getPromotedValue(train_type)
	return self.promoted_info[train_type] or 0
end

function RoleSkillData:SetTrainSkillReddotFlag(protocol)
	self.pet_train_red_dot_flag = protocol.pet_train_red_dot_flag
	self.train_red_dot_flag:Set(protocol.train_red_dot_flag)
	self:DoXiuLianRemind()
end

function RoleSkillData:GetReddotNoticeFlag(train_type)
	return self.train_red_dot_flag[train_type + 1] == 1
end

--[变强]职业技能全部达到X级
function RoleSkillData:GetBianQiangProfSkillLevel(target_level)
	local profession = RoleData.Instance:GetRoleProf()
	local config = SkillData.Instance.careerProfSkillList[profession]
	local ret_data = {}
	for i = 1, 2 do
		if config[i - 1] ~= nil then
			ret_data[i] = config[i - 1]
		end
	end
	local skill_num = #ret_data[1]
	local match_num = 0
	for i, v in pairs(ret_data[1]) do
		local data = v
		if SkillData.RoleViewShowPassiveSkill[data.skill_id] then
			skill_num = skill_num - 1
		else
			local skill_data = RoleCtrl.Instance.skill_data:getSkillDataById(data.skill_id)
			if skill_data ~= nil and skill_data.skill_level >= target_level then
				match_num = match_num + 1
			end
		end
	end
	return skill_num, match_num
end
--[变强]X个修炼技能达到最高级
function RoleSkillData:GetBianQiangShowSkillLevel(target_level)
	local list = RoleSkillData.Instance:GetShowConfig()
	local skill_num = #list
	local match_num = 0
	for i, v in pairs(list) do
		local data = v
		local skill_data = RoleSkillData.Instance:getTrainDataByType(data.train_type)
		local max_skill_level = target_level --RoleSkillData.Instance:GetAllowMaxLevel(RoleData.Instance:GetRoleLevel(), data.train_type)
		if skill_data ~= nil and skill_data.level >= max_skill_level then
			match_num = match_num + 1
		end
	end
	return skill_num, match_num
end
--[变强]X个辅助技能达到最高级
function RoleSkillData:GetBianQiangAssistSkillLevel(target_level)
	local list = RoleSkillData.Instance:GetAssistConfig()
	local skill_num = #list
	local match_num = 0
	for i, v in pairs(list) do
		local data = v
		local skill_data = RoleSkillData.Instance:getTrainDataByType(data.train_type)
		local max_skill_level = target_level --RoleSkillData.Instance:GetAllowMaxLevel(RoleData.Instance:GetRoleLevel(), data.train_type)
		if skill_data ~= nil and skill_data.level == max_skill_level then
			match_num = match_num + 1
		end
	end
	return skill_num, match_num
end


function RoleSkillData:GetTrainCanMaxLevel()
	local skill_data = self:GetCurTrainData()
	local train_data = self:GetCharacterLevelData(self.current_train_data.train_type, skill_data.level)
	local can_max_level = self:GetAllowMaxLevel(RoleData.Instance:GetRoleLevel(), self.current_train_data.train_type)
	local learn_times = 0
	local left_num = Item.GetNum(train_data.item_id)
	local use_num = 0
	for i = skill_data.level, can_max_level do
		local train_data_next = self:GetCharacterLevelData(self.current_train_data.train_type, i + 1)
		if train_data_next then
			local need_exp = train_data_next.need_exp
			if i == skill_data.level then
				need_exp = train_data_next.need_exp - skill_data.exp
			end
			--剩余经验需要学习的次数与拥有的材料能支付的次数比较
			if math.ceil(need_exp / train_data_next.train_ball_exp) > math.floor(left_num / train_data_next.train_ball_consume) or i == skill_data.level + 10 then
				--use_num = use_num + math.min(math.ceil(need_exp / train_data_next.train_ball_exp), math.floor(left_num / train_data_next.train_ball_consume)) * train_data_next.train_ball_consume
				return i, learn_times, use_num
			else
				learn_times = learn_times + math.ceil(need_exp / train_data_next.train_ball_exp)
				left_num = left_num - math.ceil(need_exp / train_data_next.train_ball_exp) * train_data_next.train_ball_consume
				use_num = use_num + math.ceil(need_exp / train_data_next.train_ball_exp) * train_data_next.train_ball_consume
			end
		else
			return i, learn_times, use_num
		end
	end
	return can_max_level, learn_times, use_num
end