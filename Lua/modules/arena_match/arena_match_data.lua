
ArenaMatchData = ArenaMatchData or BaseClass()

function ArenaMatchData:__init()
	if ArenaMatchData.Instance ~= nil then
		Debuger.LogError("[ArenaMatchData] attempt to create singleton twice!")
		return
    end
    ArenaMatchData.Instance = self

	self:ArenaMatchConfig()
	self:ArenaMatchData()
	self:InitAchievementCfg()

	self.partner_data = PartnerCtrl.Instance:Data()
	self.pet_data = PetCtrl.Instance:Data()

	self.achievement_data = SmartData.New({flags = {},type_count_info = {},next_reset_time = 0})

	-- 结算时间
	self.EndTime = SmartData.New({time = 0})

	-- 选择显示的擂台
	self.SelMatch = SmartData.New({pos = 0})
end

function ArenaMatchData:ArenaMatchConfig()
	self.config = Config.leitai_cfg_auto
	self.config_pos = self.config.pos
	self.config_leitai = self.config.leitai
	self.config_reward = self.config.reward
end

function ArenaMatchData:ArenaMatchData()
	self:ArenaRoleInfo()
	self:ArenaRankInfo()
	self:ArenaPosList()
	self:ArenaList()
	self:RewardList()

	self.arena_show_list = SmartData.New()
	self.cur_sel_preview = SmartData.New{val = -1}
	self.record_infos = SmartData.New({}) -- 擂台战报
	self.platform_notice_data = SmartData.New({}) -- 擂台红点通知数据
end

function ArenaMatchData:SetPlatformBattleRoleInfo(protocol)
	self.role_arena_seqs = {}
	local role_info_list = {}
	local role_info_nums = {
		arena = 0,
		pet = 0,
		partner = 0,
	}
	for i = 1, protocol.count do
		local role_info = protocol.role_info_list[i]
		local platform_number = role_info.platform_number
		local arena_info = self:GetArenaBySeq(platform_number)
        role_info_list[platform_number] = ArenaMatchRoleInfo.New{
			role_info = role_info,
			arena_info = arena_info,
		}
		local pet_num = role_info_list[platform_number]:PetNum()
		local partner_num = role_info_list[platform_number]:PartnerNum()
		role_info_nums.arena = role_info_nums.arena + 1
		role_info_nums.pet = role_info_nums.pet + pet_num
		role_info_nums.partner = role_info_nums.partner + partner_num
		self.role_arena_seqs[arena_info.npc_seq] = role_info.rank
	end
	self.role_info_list:Set(role_info_list)
	self.role_info_nums:Set(role_info_nums)
end

function ArenaMatchData:SetPlatformBattleRankInfo(protocol)
	local rank_info_list = {}
	local arena_info = self:GetArenaBySeq(protocol.platform_number)
	for i = 1, protocol.count do
		local rank_info = protocol.rank_info_list[i]
		rank_info_list[i] = ArenaMatchRankInfo.New{
			role_info = rank_info,
			arena_info = arena_info
		}
	end
	self.rank_info_list:Set(rank_info_list)
end

function ArenaMatchData:SetPlatformBattleRankFirstInfo(protocol)
	if protocol.count > 1 then
		local arena_show_list = {}
		for platform_number, rank_info in pairs(protocol.arena_show_list) do
			local arena_info = self:GetArenaBySeq(platform_number)
			local rank_info = rank_info
			arena_show_list[platform_number] = ArenaMatchRankInfo.New{
				role_info = rank_info,
				arena_info = arena_info
			}
		end
		self.arena_show_list:Set(arena_show_list)
	else
		for platform_number, rank_info in pairs(protocol.arena_show_list) do
			local arena_info = self:GetArenaBySeq(platform_number)
			local rank_info = rank_info
			self.arena_show_list[platform_number] = ArenaMatchRankInfo.New{
				role_info = rank_info,
				arena_info = arena_info
			}
		end
	end
end

function ArenaMatchData:ArenaShowListClear()
	self.arena_show_list:Set{}
end

function ArenaMatchData:ArenaRoleInfo()
	self.role_info_list = SmartData.New()
	self.role_info_nums = SmartData.New{
		arena = 0,
		pet = 0,
		partner = 0,
	}

	self.role_arena_seqs = {}
	self.cur_sel_info = SmartData.New()
	self.cur_sel_pos = {}
	self.cur_sel_arena = {}
	self.cur_sel_info_to_pos = 0
	self.match_info_smart = SmartData.New({cur_pet_index = -1})
end

function ArenaMatchData:ArenaRankInfo()
	self.rank_info_list = SmartData.New()
end

function ArenaMatchData:RoleInfoList()
	local role_info_list = {}
	for k, v in pairs(self.role_info_list:Val()) do
		if v:Rank() <= 10 then
			table.insert(role_info_list, v)
		end
	end
	table.sort(role_info_list, function (a, b)
		if a:ArenaSeq() == b:ArenaSeq() or (a.arena_info.position < 2 and b.arena_info.position < 2) then
			return a:Rank() < b:Rank()
		end
		return a:ArenaSeq() < b:ArenaSeq()
	end)
	for i, info in ipairs(role_info_list) do
		info.index = i
		info.bg_type = i % 2
	end
	return role_info_list
end

function ArenaMatchData.RewardsShow(role_info_list)
	local rewards = {}
	for k, v in pairs(role_info_list) do
		if k <= 2 then
			for _, reward in pairs(v:Rewards()) do
				local item_data = rewards[reward.item_id]
				if nil == item_data then 
					item_data = Item.New()
					item_data.item_id = reward.item_id
					item_data.num = reward.num
					item_data.is_bind = reward.is_bind
				else
					item_data.num = item_data.num + reward.num
				end
				rewards[reward.item_id] = item_data
			end
		end
	end
	return ArenaMatchData.RewardsSort(rewards)
end

function ArenaMatchData.RewardsSort(rewards)
	local list = {}
	for _, item in pairs(rewards) do
		table.insert(list, item)
	end
	table.sort(list, function (a, b)
		if not a or not b then return a end
		local ac = a:Config()
		local bc = b:Config()
		local ait = ac.item_type or (CommonItemId.Coin == ac.id and -1 or 0)
		local bit = bc.item_type or (CommonItemId.Coin == bc.id and -1 or 0)
		local ast = ac.show_type or 0
		local bst = bc.show_type or 0
		if ait == bit then
			return ast == bst and ac.color > bc.color or ast > bst
		end
		return ait > bit
	end)
	return list
end

function ArenaMatchData:RoleInfoBySeq(seq)
	return self.role_info_list[seq]
end

function ArenaMatchData:RoleInfoNums()
	return self.role_info_nums:Val()
end

function ArenaMatchData:RankInfoList()
	return self.rank_info_list:Val()
end

function ArenaMatchData:ArenaList()
	self.arena_list_s = {}
	self.arena_list_p = {}
	self.arena_list_ns = {}
	for _, v in pairs(self.config_leitai) do
		v.pet_num_max = self.arena_pos_list_p[v.position].pet_num
		v.partner_num_max = self.arena_pos_list_p[v.position].parnter_num
		self.arena_list_s[v.seq] = v
		self.arena_list_p[v.position] = self.arena_list_p[v.position] or {}
		self.arena_list_ns[v.npc_seq] = v
		table.insert(self.arena_list_p[v.position], v)
	end
	for pos, _ in pairs(self.arena_list_p) do
		table.sort(self.arena_list_p[pos], function (a, b)
			return a.seq < b.seq
		end)
	end
end

function ArenaMatchData:GetArenaBySeq(seq)
	return self.arena_list_s[seq]
end

function ArenaMatchData:GetArenaListByPos(position)
	return self.arena_list_p[position]
end

function ArenaMatchData:GetArenaListByNpcSeq(npc_seq)
	return self.arena_list_ns[npc_seq]
end

function ArenaMatchData:RewardList()
	self.reward_list = {}
	for _, v in pairs(self.config_reward) do
		local rewards = {}
		for _, v in pairs(v.reward) do
			table.insert(rewards, Item.New(v))
		end
		if v.points ~= 0 then
			local item_data1 = Item.New()
			item_data1.item_id = 65568
			item_data1.num = v.points
			item_data1.is_bind = 0
			table.insert(rewards, item_data1)
		end
		local item_data2 = Item.New()
        item_data2.item_id = CommonItemId.Coin
        item_data2.num = v.bind_coin
        item_data2.is_bind = 0
		table.insert(rewards, item_data2)
		v.rewards = rewards
		self.reward_list[v.reward_id] = self.reward_list[v.reward_id] or {}
		table.insert(self.reward_list[v.reward_id], v)
	end
end

function ArenaMatchData:GetRewardList(pos)
	pos = pos or self.cur_sel_preview.val
	if pos == nil or pos < 0 then
		return {}
	end
	local reward_id = 0
	local co = self.arena_list_p[pos]
	if co then
		reward_id = co[1].reward_id
	end
	return self.reward_list[reward_id]
end

function ArenaMatchData:GetShowRewardList(pos)
	local reward_list = self:GetRewardList(pos)
	if TableIsEmpty(reward_list) then
		return {}
	end
	local rewards = reward_list[1].rewards
	local list_data = {}
	for i = 1, 3 do
		if rewards[i] then
			local item_id = rewards[i].item_id
			table.insert(list_data, Item.Create({item_id = item_id}))
		end
	end
	return list_data
end

function ArenaMatchData:GetRewardByIdRank(reward_id, rank)
	for _, v in pairs(self.reward_list[reward_id]) do
		if v.min_rank <= rank and rank < v.max_rank then
			return v
		end
	end
	return {}
end

function ArenaMatchData:ArenaPosList()
	self.arena_pos_list_s = {}
	self.arena_pos_list_p = {}
	self.arena_pos_list_i = {}
	for _, v in pairs(self.config_pos) do
		self.arena_pos_list_s[v.seq] = v
		self.arena_pos_list_p[v.position] = v
		self.arena_pos_list_i[v.scene_id] = v
	end
end

function ArenaMatchData:GetArenaPosSeqList()
	return self.arena_pos_list_s
end

function ArenaMatchData:GetArenaPosSceneInfo(scene_id)
	return self.arena_pos_list_i[scene_id]
end

function ArenaMatchData:GetFightList()
	local pet_list = {}
	local partner_list = {}
	local role_level = RoleData.Instance:GetRoleLevel()
	local arena_seq = self.cur_sel_info:ArenaSeq()
	local arena_info = self.role_info_list[arena_seq]
	local show_pet, show_partner = self.cur_sel_info:CheckFight(self.cur_sel_info_to_pos)
	if show_pet then self:GetPetList(pet_list, nil ~= arena_info and arena_info:PetList() or {}) end
	if show_partner then self:GetPartnerList(partner_list, role_level, nil ~= arena_info and arena_info:PartnerList() or {}) end
	return pet_list, partner_list, show_pet, show_partner
end

function ArenaMatchData:GetPetList(pet_list, arena_pets)
	for _, data in pairs(arena_pets) do
		local pet = data.pet
		local pos = self.cur_sel_info:CheckPet(pet.index)
		if 0 == pos then
			local pet_state = pet:State()
			local data = ArenaMatchInfo.NewPet(pet, {
				is_zhan = PetState.Fight == pet_state,
				is_dai = PetState.StandBy == pet_state,
				is_lei = true,
			})
			table.insert(pet_list, data)
		end
	end
	for _, pet in pairs(self.pet_data.pet_list) do
		local pet_state = pet:State()
		if PetState.Train ~= pet_state and not pet:ArenaState() then
			local pos = self.cur_sel_info:CheckPet(pet.index)
			local data = ArenaMatchInfo.NewPet(pet, {
				is_zhan = PetState.Fight == pet_state,
				is_dai = PetState.StandBy == pet_state,
			})
			if 0 == pos then table.insert(pet_list, data) end
		end
	end
	for _, pet_data in pairs(self.cur_sel_info.pet_t_list) do
		local pet_state = pet_data.pet:State()
		pet_data.vo = {
			is_zhan = PetState.Fight == pet_state,
			is_dai = PetState.StandBy == pet_state,
		}
		table.insert(pet_list, pet_data)
	end
	table.sort(pet_list, function (a, b)
		if not a or not b then return a end
		-- local ail = a.vo.is_lei and 1 or 0
		-- local bil = b.vo.is_lei and 1 or 0
		-- if ail == bil then return a.pet:Level() > b.pet:Level() end
		return a.pet:GetScore() > b.pet:GetScore()
	end)
	return pet_list
end

function ArenaMatchData:GetPartnerList(partner_list, role_level, arena_partners)
	for _, data in pairs(arena_partners) do
		local pos = self.cur_sel_info:CheckPartner(data.partner.partner_id)
		local partner = self.partner_data:GetPartnerInfoById(data.partner.partner_id)
		if 0 == pos then
			local data = ArenaMatchInfo.NewPartner(data.partner, {
				level = partner.vo.level,
				is_lei = true,
				skin_id = data.vo.skin_id,
				intensify_level = partner.vo.intensify_level,
			})
			table.insert(partner_list, data)
		end
	end
	for _, partner in pairs(self.partner_data.partner_list) do
		local vo = partner.vo
		if vo.is_unlocked == PartnerConfig.LockState.unlock and 0 == vo.is_arena_match then
			local pos = self.cur_sel_info:CheckPartner(partner.info.partner_id)
			local data = ArenaMatchInfo.NewPartner(partner.info, {
				level = partner.vo.level,
				is_zhan = PartnerConfig.FightState.fight == vo.fight_state,
				skin_id = partner:SkinId(),
				intensify_level = partner.vo.intensify_level,
			})
			if 0 == pos then table.insert(partner_list, data) end
		end
	end
	for _, partner_data in pairs(self.cur_sel_info.partner_t_list) do
		local partner = self.partner_data:GetPartnerInfoById(partner_data.partner.partner_id)
		partner_data.vo = {
			level = partner.vo.level,
			is_zhan = PartnerConfig.FightState.fight == partner.vo.fight_state,
			intensify_level = partner.vo.intensify_level,
		}
		table.insert(partner_list, partner_data)
	end
	table.sort(partner_list, function (a, b)
		if not a or not b then return a end
		local a_info = PartnerData.Instance:GetPartnerInfoById(a.partner.partner_id)
		local b_info = PartnerData.Instance:GetPartnerInfoById(b.partner.partner_id)
		return a_info.vo.cap > b_info.vo.cap
	end)
	return partner_list
end

function ArenaMatchData:SetCurPetInfo(data)
	self.match_info_cur_pet_data = data
	self.match_info_smart.cur_pet_index = data.pet.index
end

function ArenaMatchData:GetMatchInfoPetList()
	local pet_list = {}
	for key, value in pairs(self.cur_sel_info:PetList()) do
		table.insert(pet_list, value)
	end
	return pet_list
end

function ArenaMatchData:GetMatchInfoCurPetMainSkillList()
	if self.match_info_smart.cur_pet_index > -1 then
        local skill_list = self.match_info_cur_pet_data.pet:GetMainSkillList({})
        local pet_main_skill_list = {}
        local main_skill = self.match_info_cur_pet_data.pet_exclusive_skill_disable_flag
        local lianyao_skill = self.match_info_cur_pet_data.lian_yao_exclusive_skill_disable_flag
        local exclusive_num = 0
        local lian_yao_num = 0
        for k, v in ipairs(skill_list) do
            local skill_type = SkillData.GetSkillType(v.skill_id)
			-- 筛除宠物被动技能
			if 7 ~= skill_type and 8 ~= skill_type then
                if not v.lian_yao_skill then
                    table.insert(pet_main_skill_list,{gift_info = v, is_forbid = main_skill[#main_skill - exclusive_num], main_skill_num = exclusive_num})
                    exclusive_num = exclusive_num + 1
                else
                    table.insert(pet_main_skill_list,{gift_info = v, is_forbid = lianyao_skill[#lianyao_skill - lian_yao_num], main_skill_num = lian_yao_num})
                    lian_yao_num = lian_yao_num + 1
                end
            end
        end
        return pet_main_skill_list
    end
end

function ArenaMatchData:SetPetSkillPermission(data)
    if not data.gift_info.lian_yao_skill then
        local pet_exclusive = self.match_info_cur_pet_data.pet_exclusive_skill_disable_flag
        pet_exclusive[#pet_exclusive - data.main_skill_num] = data.is_forbid
        self.match_info_cur_pet_data.pet_exclusive_skill_disable_flag = pet_exclusive
    else
        local lianyao_exclusive = self.match_info_cur_pet_data.lian_yao_exclusive_skill_disable_flag
        lianyao_exclusive[#lianyao_exclusive - data.main_skill_num] = data.is_forbid
        self.match_info_cur_pet_data.lian_yao_exclusive_skill_disable_flag = lianyao_exclusive
    end
end

function ArenaMatchData:GetConditionTip()
	return Format(Language.ArenaMatch.ConditionTip,self.role_info_nums.arena)
end

function ArenaMatchData:GetArenaPosListWithoutScene(scene_id)
	local list = {}
	for _, v in pairs(self.arena_pos_list_s) do
		if scene_id ~= v.scene_id then
			table.insert(list, v)
		end
	end
	return list
end

function ArenaMatchData:GetArenaPosSceneInfoSortRoleInfo(position)
	local list = TableCopy(self.arena_list_p[position])
	table.sort(list, function (a, b)
		if not a or not b then return a end
		local a1 = self.role_info_list[a.seq] and 1 or 0
		local b1 = self.role_info_list[b.seq] and 1 or 0
		if a1 == b1 then return a.seq < b.seq end
		return a1 > b1
	end)
	return list
end

function ArenaMatchData:GetRoleRankByNpcSeq(npc_seq)
	return self.role_arena_seqs[npc_seq] or 0
end

function ArenaMatchData:GetArenaHoldNumList()
	local list = {}
	for _, info in pairs(self.role_info_list) do
		local position = info:ArenaPosition()
		local seq = self.arena_pos_list_p[position].seq
		list[seq] = (list[seq] or 0) + 1
	end
	return list
end

function ArenaMatchData:GetOccupyList(pos)
	local list = {}
	for _, info in pairs(self.role_info_list) do
		local position = info:ArenaPosition()
		if pos == position then
			table.insert(list, info)
		end
	end
	return list
end

-- 设置获取擂台战报信息
function ArenaMatchData:SetRecordInfos(protocol)
	local sort_func = function (a,b)
        return a.time > b.time
    end
	local infos = {}
	for i,v in pairs(protocol.record_infos) do
		local cfg = self:GetArenaBySeq(v.platform_number)
		if cfg then
			if infos[cfg.position] == nil then
				infos[cfg.position] = {}
			end
			table.insert(infos[cfg.position],v)
		end
	end
	for i,v in pairs(infos) do
		table.sort(v,sort_func)
	end
	self.record_infos:Set(infos)
end

function ArenaMatchData:GetRecordInfos(platform_pos)
	if platform_pos == nil then
		return self.record_infos
	end
	return self.record_infos[platform_pos] or {}
end

--设置擂台红点通知数据
function ArenaMatchData:SetNoticeData(protocol)
	self.platform_notice_data:Set(protocol.info)
end

--擂台战报按钮红点
function ArenaMatchData:BtnRemind(pos)
	if not self.platform_notice_data.has_lose_record then
		return 0
	end
	return self.platform_notice_data.has_lose_record[16 - pos] or 0
end

-- 擂台红点逻辑
function ArenaMatchData:GetRemindNum()
	if FunOpen.Instance:CheckOpen(Mod.ArenaMatch.Arena) == false then
		return 0
	end
	-- 一次都没打过擂台也报红点
	if self.platform_notice_data.is_first_times and self.platform_notice_data.is_first_times ~= 0 then
		return 1
	end
	-- 成就红点
	if self:GetAchievementRemind() == 1 then
		return 1
	end
	return self:GetRecordRemindNum()
end

-- 擂台战报红点逻辑
function ArenaMatchData:GetRecordRemindNum()
	for i,v in pairs(self.platform_notice_data.has_lose_record or {}) do
		if v > 0 then
			return 1
		end
	end
	return 0
end

-------------- 擂台成就 ------------

-- 整理成就
function ArenaMatchData:InitAchievementCfg()
    self.achievement_group_cfg = {}
    for i,v in ipairs(self.config.leitai_achieve) do
        if self.achievement_group_cfg[v.ach_group] == nil then
            self.achievement_group_cfg[v.ach_group] = {}
        end
        table.insert(self.achievement_group_cfg[v.ach_group],v)
    end
end

-- 处理成绩协议数据
function ArenaMatchData:HandleAchievementProto(protocol)
	self.achievement_data.type_count_info = protocol.type_count_info
end

-- 设置成就进度数量
function ArenaMatchData:SetAchievementCount(protocol)
    self.achievement_data.type_count_info = protocol.type_count_info
end

-- 提取成就组
function ArenaMatchData:GetAchievementGroupFinishData(group_id)
    local list = {}
    local cfg = self.achievement_group_cfg[group_id]
    local end_flag = self:GetAchievementFlag(cfg[#cfg])
    if end_flag == -1 then
        table.insert(list,cfg[#cfg])
        return list 
    end
    for i,v in ipairs(cfg) do
        local flag = self:GetAchievementFlag(v)
        if flag ~= -1 then
            table.insert(list,v)
            break
        end
    end
    return list
end

-- 获取成就数据
function ArenaMatchData:GetAchievementData()
    local list_data = {}
    for i,v in ipairs(self.achievement_group_cfg) do
        local list = self:GetAchievementGroupFinishData(i)
        AndList(list_data,list)
    end
    local sort_func = function (a,b)
        a.flag = self:GetAchievementFlag(a)
        b.flag = self:GetAchievementFlag(b)
        a.progress_num = self:GetAchievementGroupCount(a.type)
        b.progress_num = self:GetAchievementGroupCount(b.type)
        if a.flag == b.flag then
            return a.sn < b.sn
        else
            return a.flag > b.flag
        end
    end
    table.sort(list_data,sort_func)
    return list_data
end

-- 成就标记 -- 0 未达成 1 可领取 -1 已领取 
function ArenaMatchData:GetAchievementFlag(data)
	if TableIsEmpty(self.achievement_data.type_count_info) then
        return 0
    end
	local type_info = self.achievement_data.type_count_info[data.type]
	if type_info.flags[16 - data.seq] == 1 then
		return -1
	end
    local progress_num = self:GetAchievementGroupCount(data.type)
    if data.type == 0 or (data.type >= 12 and data.type <= 16) then
        if progress_num > 0 and progress_num <= data.param then
            return 1
        end
    else
        if progress_num >= data.param then
            return 1
        end
    end
   
    return 0
end

-- 成就类型完成的次数
function ArenaMatchData:GetAchievementGroupCount(type_id)
	if TableIsEmpty(self.achievement_data.type_count_info) then
        return 0
    end
    return self.achievement_data.type_count_info[type_id].count or 0 
end

-- 成就红点逻辑
function ArenaMatchData:GetAchievementRemind()
    if TableIsEmpty(self.achievement_data.type_count_info) then
        return 0
    end
    for i,v in ipairs(self.config.leitai_achieve) do
        if self:GetAchievementFlag(v) == 1 then
            return 1
        end
    end
    return 0
end

-- 获取每周结算时间, 每周五13:00结算
function ArenaMatchData:GetDayEndTime()
	local week_num = ActivityData.Instance:GetWeekNum()
	local end_h = 13
	local end_w = 5
	local cut_time = os.date("%H-%M-%S",TimeCtrl.Instance:GetServerTime())
	local time_t = Split(cut_time,"-")
	time_t[1] = tonumber(time_t[1])
	time_t[2] = tonumber(time_t[2])
	time_t[3] = tonumber(time_t[3])
	local end_time = 0
	if week_num < end_w or (week_num == end_w and time_t[1] < end_h) then
		local time = (end_h - time_t[1])  * 3600 - time_t[2] * 60 - time_t[3]
		local week_time = (end_w - week_num) * 86400
		end_time = week_time + time
	else
		local week_time = (week_num - end_w) * 86400
		local time = (time_t[1] - end_h) * 3600 + time_t[2] * 60 + time_t[3]
		end_time = 7 * 86400 - time - week_time
	end
	return end_time
end

function ArenaMatchData:GetTopConditionTip()
	if ActivityData.Instance:IsNoEnoughLevel(ActType.ArenaMatch,true) then
		return
	end
    local time = self:GetDayEndTime()
	local time_info = TimeHelper.FormatDHMS(time)
	if time_info.day > 0 then
		return string.format(Language.ArenaMatch.RewardTime2, time_info.day, time_info.hour)
	else
		return string.format(Language.ArenaMatch.RewardTime, time_info.hour, time_info.min)
	end
end

function ArenaMatchData:GetArenacfgByNpcSeq(npc_seq)
	for i,v in pairs(self.config.leitai) do
		if v.npc_seq == npc_seq then
			return v
		end
	end
	return {}
end