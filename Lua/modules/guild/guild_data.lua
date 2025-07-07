GuildData = GuildData or BaseClass()

-- 家族成员职位类型
GuildMemberType = {
    Leader = 0,         --族长
    DeputyLeader = 1,   --副族长
    Elder = 2,          --长老
    KeyMember = 3,      --骨干
    Elite = 4,          --精英
    Member = 5,         --成员
}

function GuildData:__init()
    if GuildData.Instance ~= nil then
        Debuger.LogError("[GuildData] attempt to create singleton twice!")
        return
    end
    GuildData.Instance = self
    self.guild_list = SmartData.New()
    self.guild_notice = SmartData.New()
    self.guild_info = SmartData.New()
    self.member_info = SmartData.New({member_count = 0, member_list = {}})
    self.apply_list = SmartData.New()
    self.shop_buy_list = SmartData.New()
    self.guild_pet_list = SmartData.New()
    self.guild_train_list = SmartData.New()
    self.guild_task_list = SmartData.New()
    self.gwt_all_info = SmartData.New()
    self.apply_count = SmartData.New({count = 0})
	self.guild_jiyuan_list = SmartData.New()
	self.guild_jiyuan_jilu_get_list = SmartData.New()
	self.guild_jiyuan_jilu_send_list = SmartData.New()
	self.guild_jiyuan_fabu_list = SmartData.New()
    self.guild_jiyuan_get_point = SmartData.New({count = 0})
    self.guild_top_info = SmartData.New()
	self.publish_is_send_flag = 0
	self.today_is_publish_gather_dream_flag = -1
	self.today_is_publish_gather_dream_red = false
	self.guild_jiyuan_open_index_id = 0
    self.guild_jiyuan_haven_give = 0
    -- 本次角色登录的时候，如果活动开了，就会显示一次红点
    self.guild_info_is_active_remind = true
    self.guild_trial_point = SmartData.New({count = 0, active_count = 0, flush = false})
	self.guild_trial_point_have_show = false
    self.guild_shop_first_item = SmartData.New({count = 1, flush = false})
    self.guild_honor_info = {
        state = 0,
        next_match_state = 0,
        result_type = 0
    }
    self.honor_role_info_zone = SmartData.New()
    self.honor_role_info_id = SmartData.New()
    self.select_zone_id = 1
    self.embattle =
        SmartData.New(
        {
            flushview = false,
            flushstate = false
        }
    )
    self.honor_zone_info = SmartData.New()
    self.honor_zone_nums = SmartData.New()
    self.honor_battle_info = {}

    self.guild_boss_info = SmartData.New()
    self.cache_guild_boss_seq = 0
    self.guild_trial_page = SmartData.New({index = 0})
    self.guild_trial_reward_item = SmartData.New({item = 0})
    self.guild_trial_total_rank =SmartData.New()
    self.boss =
        SmartData.New(
        {
            flushselect = false,
            flushRewardRedPoint = false,
        }
    )
    self.guild_boss_rank = SmartData.New()
    self.guild_boss_rank_count = 0
    self.defeated_times = 0
    self.guild_question_info = SmartData.New({question_id = 0})
	
	self.guild_active_point = SmartData.New({count = 0})
	self.guild_active_info = SmartData.New()
	self.guild_active_info_flush = SmartData.New({flag = false})
	self.guild_active_total_rank =SmartData.New()
	self.guild_active_total_rank_my_index = -1
	self.guild_active_person_rank_my_index = -1
	self.guild_active_person_rank =SmartData.New()
	self.guild_active_box_flag = SmartData.New({flag = 0,seq = -1})
	self.guild_active_box_seq = SmartData.New({seq = 0})

    self.task_list = {}
    self.pre_task_progress = {}
    self.guild_id = 0
    self.family_auto = Config.family_auto
    self.family_war_auto = Config.family_war_auto
    self.family_war_other = self.family_war_auto.other[1]
    self.family_room = self.family_auto.family_room
    self.honour_reward = self.family_war_auto.honour_reward
    -- self.family_activity = self.family_auto.family_activity
    self.member_type = self.family_auto.member_type
    self.family_shop = self.family_auto.family_shop
    self.guild_other = self.family_auto.other[1]
    self.train_up = self.family_auto.train_up
    self.train_exp = self.family_auto.exp
    self.family_build = self.family_auto.family_build
    self.build_donate = self.family_auto.build_donate
    self.family_inborn = self.family_auto.family_inborn
    --
    self.family_mission = Config.family_mission_auto
    self.family_mission_mission = self.family_mission.mission
    self.family_mission_reward = self.family_mission.reward
    self.family_mission_gongxian_reward = self.family_mission.gongxian_reward
    self.family_mission_other = self.family_mission.other[1]

    self.family_boss = self.family_auto.family_boss
    self.family_boss_info = {}
    self.family_boss_reward = {}
    self.family_question_cfg_auto = Config.family_question_cfg_auto
    self.family_question = Config.family_question_cfg_auto.question_bank
    self.family_question_other = Config.family_question_cfg_auto.other[1]

    self.places_index = 0
    self.build_donate_list = {}

    self.build_flag = {}
    self.build_value = {}
    self.build_donate_count = {}
    self.build_goods_flush = SmartData.New({num = 0})
    self.build_info_flush = SmartData.New({num = 0})
    self.melee_rank_all_info = SmartData.New()
    self.melee_my_rank = SmartData.New()
    self.melee_nums_info = SmartData.New({resi_role = 0, resi_guild = 0, reward_flush = false})
    self.melee_rank_reward = {}
    self.build_goods = {}
    self:check_config()

    self.guild_pet_point = SmartData.New({count = 0})
    self.player_apply_list = SmartData.New()

    --灵脉
    self.guild_inborn_info = SmartData.New({max_guild_level = 0, use_skill_points = 0, learn_count = 0, learn_list = {}})
    self.guild_inborn_point = SmartData.New({count = 0})
    self.guild_other_info = {}

    self.guild_task_cfg = Config.jobtask_auto
    -- self.need_check_follow = true

    self.act_finish_times = {}
    self.question_show = SmartData.New({show_panel = false})
    self.failyboss_battle_map_id = nil
	
	self.login_base_rs_care = LoginData.Instance:BaseData():Care(function (  )
        if LoginCtrl.Instance.data:LoginState() == LoginStateType.RoleSelecting then
			self.today_is_publish_gather_dream_flag = -1
			self.today_is_publish_gather_dream_red = false
			self.guild_trial_point_have_show = false
		end
    end, "login_state")

    self.select_view_open_flag = 1     -- 1代表从选择章节打开，2代表从章节奖励预览打开

    ActivityData.Instance:RegisterRewardGetFunc(ActType.GuildEmbattle, function ()
        return self:GetEmbattleReward()
    end)
    ActivityData.Instance:RegisterRewardGetFunc(ActType.GuildAnswer, function ()
        return self:GetAnswerTotalPreReward()
    end)
    ActivityData.Instance:RegisterRewardGetFunc(ActType.GuildMelee, function ()
        return self:GetMeleeTotalPreRewardShow()
    end)

    self.zone_battle_result = {}
    self.guild_recommend_info = {}
	self.damage_data = SmartData.New({flag = false})
	self.guild_trial_total_damage = 0

    self.cooperate_data = SmartData.New({flush = 0})
    self.cooperate_operate = SmartData.New({flush = 0,help_flush = 0, index = 0})
end
function GuildData:check_config()
    for i, v in ipairs(self.build_donate) do
        if self.build_donate_list[v.build_seq] == nil then
            self.build_donate_list[v.build_seq] = {}
        end
        self.build_donate_list[v.build_seq][v.index] = v
    end
    for _, v in ipairs(self.family_war_auto.rank_reward) do
        for i = v.rank_min, v.rank_max do
            -- self.melee_rank_reward[i] = v
            self.melee_rank_reward[i] = TableCopy(v)
            --加上参与奖励
            if self.melee_rank_reward[i].reward_show and self.family_war_other.join_reward then
                for _, value in pairs(self.family_war_other.join_reward) do
                    table.insert(self.melee_rank_reward[i].reward_show, value)
                end
            end
            self.melee_rank_reward[i].gongxian = (self.melee_rank_reward[i].gongxian or 0) + self.family_war_other.gongxian
            self.melee_rank_reward[i].coin = (self.melee_rank_reward[i].coin or 0) + self.family_war_other.coin
        end
    end

    for _, v in ipairs(self.family_boss) do
        if self.family_boss_info[v.chapter_id] == nil then
            self.family_boss_info[v.chapter_id] = {}
        end
        self.family_boss_info[v.chapter_id][v.seq] = v
    end
    for k,v in pairs(self.family_auto.boss_reward) do
        self.family_boss_reward[v.chapter_id] = self.family_boss_reward[v.chapter_id] or {}
        self.family_boss_reward[v.chapter_id][v.max_level] = self.family_boss_reward[v.chapter_id][v.max_level] or {min_level = v.min_level ,item_list = {}}

        table.insert(self.family_boss_reward[v.chapter_id][v.max_level].item_list, v.reward_item)
    end
    self.family_war_box = KeyList(self.family_war_auto.box, "box_id")

    self.family_activity = {}
    for k, v in ipairs(self.family_auto.family_activity) do
        if v.is_show == 1 then
            table.insert(self.family_activity, v)
        end
    end
    --家族试炼伤害排行榜奖励
    self.guild_trial_rank_reward = {}
    for k,v in ipairs(self.family_auto.injury_reward) do
        self.guild_trial_rank_reward[v.chapter_id] = self.guild_trial_rank_reward[v.chapter_id] or {}
        table.insert(self.guild_trial_rank_reward[v.chapter_id], v)
    end
    --家族试炼选择章节奖励预览
    self.guild_trial_select_reward = {}
--[[    for i,v in ipairs(self.family_auto.kill_reward) do
        self.guild_trial_select_reward[v.chapter_id] = {}
        local reward_list_str_t = string.split(v.reward_show,"|")
        for j = 1, #reward_list_str_t do
            local reward_item_str_t = string.split(reward_list_str_t[j],":")
            table.insert(self.guild_trial_select_reward[v.chapter_id], {item_id = tonumber(reward_item_str_t[1]), num = tonumber(reward_item_str_t[2]), is_bind = tonumber(reward_item_str_t[3])})
        end
    end--]]

    local page_type_t = {}
    for k,v in ipairs(self.family_auto.page_type) do
        if v.is_show == 1 then
            table.insert(page_type_t, v)
        end
    end
    self.family_auto.page_type = page_type_t

    self.family_inborn_auto = TabGroup(self.family_inborn, "inborn_type", "type_seq", "skill_level")
end

function GuildData:SetGuildList(list)
    self.guild_list:Set(list)
end

function GuildData:GetGuildList()
    return self.guild_list
end

function GuildData:SetGuildNotice(protocol)
    self.guild_notice[protocol.guild_id] = protocol.notice
    if self:GetGuildID() == protocol.guild_id then
        self.guild_info.guild_notice = protocol.notice
    end
end

--[[function GuildData:SetGuildEvent(protocol)
    self.guild_info.eventhandler_list = protocol.eventhandler_list
end--]]

function GuildData:GetGuildNotcie(guild_id)
    return self.guild_notice[guild_id] or ""
end

function GuildData:SetGuildInfo(protocol)
    self.guild_id = protocol.guild_id
    self.guild_info.guild_id = protocol.guild_id
    self.guild_info.guild_name = protocol.guild_name
    self.guild_info.guild_notice = protocol.guild_notice
    self.guild_info.level = protocol.level
    self.guild_info.exp = protocol.exp
    self.guild_info.create_time = protocol.create_time
    self.guild_info.join_time = protocol.join_time
    self.guild_info.patriarch_name = protocol.patriarch_name
    --self.guild_info.eventhandler_list = protocol.eventhandler_list
    self.guild_info.is_need_approval = protocol.is_need_approval or 0
    self.guild_info.arms = protocol.arms
    self.guild_info.patriarch_last_login_time = protocol.patriarch_last_login_time
    self.guild_info.is_join_tan_he_patriarch = protocol.is_join_tan_he_patriarch
    self.guild_info.can_tan_he_patriarch = protocol.can_tan_he_patriarch
    self.guild_info.patriarch_tan_he_start_time = protocol.patriarch_tan_he_start_time
    self.guild_info.guild_buff_rank = 0 -- protocol.guild_buff_rank

    self:GetGuildBuff()
end

-- 主动还是被动退出家族或者解散的时候调用
function GuildData:ClearGuildInfo()
    self.guild_id = 0
    self.guild_info_is_active_remind = true
	self.guild_trial_point.count = 0
	self.guild_trial_point.active_count = 0
    self.guild_shop_first_item.count = 1
	self.guild_jiyuan_get_point.count = 0
    RoleData.Instance:GetBaseData().guild_name = ""
    MainPlayer:Vo().guild_name = ""
    RoleData.Instance:GetBaseData().arms = ""
    MainPlayer:Vo().arms = ""
    MainPlayer:Vo().guild_post = -1
    self.guild_boss_info:Set({})
    self.guild_info.guild_buff_rank = 0
    self.zone_battle_result = {}
    self.guild_inborn_point.count = 0
    GuildHofData.Instance:ResetData()
    GuildBuildingData.Instance:ResetData()
end

function GuildData:GetGuildID()
    return self.guild_id
end
function GuildData:GetGuildName()
    return self.guild_info.guild_name or ""
end

function GuildData:GetGuildInfo()
    return self.guild_info
end

function GuildData:GetLevelInfo(level)
    level = level or self.guild_info.level or 1
    if level < 1 then
        level = 1
    end
    if level > self.guild_other.max_level then
        level = self.guild_other.max_level
    end
    return self.family_room[level]
end

-- 获取当前家族维护资金
function GuildData:GetServeMoney()
    local level_info = self:GetLevelInfo()
    return level_info.cost + GuildBuildingData.Instance:BuildingServeMoney()
end

-- 获取当前家族人数上限（是否加上厢房上限）
function GuildData:GetMaxPerson(is_add)
    is_add = is_add ~= false
    local level_info = self:GetLevelInfo()
    return level_info.max_member + (is_add and GuildBuildingData.Instance:AddPersonMax() or 0)
end

-- 获取当前家族人数
function GuildData:GetNowPerson()
    local member_info = self:GetMemberInfo()
    return member_info.member_count or 1
end

-- 获取当前家族是否满，当前人数，上限人数
function GuildData:GetIsFullPerson()
    local member_info = self:GetMemberInfo()
    local now_num = self:GetNowPerson()
    local max_num = self:GetMaxPerson()
    return now_num == max_num, now_num, max_num
end

-- 获取当前家族资金是否满，当前资金，满的资金
function GuildData:GetIsFullMoney()
    local info = self:GetGuildInfo()
    local now = info.exp
    local full = GuildBuildingData.Instance:FullMoney()
    return now >= full, now, full
end

-- 获取当前家族限制的生活等级
function GuildData:GetLiveLimitLevel()
    return GuildBuildingData.Instance:LiveLimitLevel()
end

function GuildData:SetMemberInfo(protocol)
    self.member_info.member_count = protocol.member_count
    self.member_info.member_list = protocol.member_list
end

function GuildData:SetMemberInfoChange(protocol)
    local is_complete = false
    for k,v in pairs(self.member_info.member_list) do 
        if v.uid == protocol.member_info.uid then
            self.member_info.member_list[k] = protocol.member_info
            is_complete = true
            break
        end
    end 

    if not is_complete then
        table.insert(self.member_info.member_list,protocol.member_info)
        self.member_info.member_count = self.member_info.member_count + 1
    end

    self.cooperate_operate.help_flush = self.cooperate_operate.help_flush + 1
end

function GuildData:GetMemberInfo()
    return self.member_info
end

function GuildData:GetGuildMasterInfo()
    local items = nil
    for k, v in pairs(self.member_info.member_list) do
        if v.post == 0 then
            items = v
        end
    end
    return items
end

function GuildData:GetMemberListOnline(include_self)
    local uid = RoleData.Instance:GetBaseData().role_id
    local limit_level = TeamData.Instance:InTeam() and TeamData.Instance:GetTeamInfo():LimitLevelLow() or TeamData.Instance:LimitLevelLow()
    local list = {}
    for _, member in pairs(self.member_info.member_list) do
        if member.online >= 1 and (include_self or uid ~= member.uid) then
            if member.level >= limit_level then
                table.insert(list, member)
            end
        end
    end
    return list
end

function GuildData:GetMemberListWeddingInvite()
    local list = {}
    if 0 == self:GetGuildID() then return list end
    local invite_list = MarryData.Instance:GetWeddingInviteList()
    local invite_info = MarryData.Instance:GetWeddingInviteInfo()
    local uid = RoleData.Instance:GetBaseData().role_id
    local limit_level = MarryData.Instance:ConfigOtherPartakeLevelLimit()
    for _, member in pairs(self.member_info.member_list) do
        if not invite_list[member.uid] and uid ~= member.uid then
            if not invite_info.hosts[member.uid] and member.level >= limit_level then
                table.insert(list, member)
            end
        end
    end
    return list
end

function GuildData:GetActList()
    return self.family_activity
end

function GuildData:GetActListWithSort(position_type)
    position_type = position_type or 0
    local t = {}
    for k, v in pairs(self.family_activity) do
        if position_type == 0 or position_type == v.position_type then
            table.insert(t, v)
        end
    end

    local sorted_list = TableCopy(t)
    -- 正下方的活动，需要根据活动开启来排序
    if position_type == 2 then
        table.sort(sorted_list, DataHelper.WeightSort("sort_order", function(data)
            return ActivityData.IsToday(data.act_type) and -100 or 0
        end))
    else
        table.sort(sorted_list, DataSorter.KeyLowerSorter("sort_order"))
    end
    return sorted_list
end

-- 家族信息界面红点
function GuildData:GuildInfoRemind()
    if self:ShowTrialPoint() == 1 then
        return 1
    elseif self.guild_trial_point.active_count > 0 then
        return 1
    elseif GuildRedbagData.Instance:HongBaoPointNum() > 0 then
        return 1
    elseif self:GuildInfoActRemind() > 0 then
        return 1
    elseif GuildHofData.Instance:AllRemind() > 0 then
        return 1
    elseif GuildBuildingData.Instance:AllRemind() > 0 then
        return 1
    end
    return 0
end

-- 家族信息界面下方活动红点
function GuildData:GuildInfoActRemind()
    if self.guild_info_is_active_remind then
        local list = self:GetActListWithSort(2)
        for k, v in pairs(list) do
            if ActivityData.IsOpen(v.act_type) then
                return 1
            end
        end
    end
    return 0
end

function GuildData:CanAccuseGuildLeader()
    return self.guild_info.can_tan_he_patriarch == 1
end

function GuildData:SetApplyList(protocol)
    self.apply_count.count = protocol.count
    self.apply_list:Set(protocol.apply_list)
end
function GuildData:GetApplyList()
    return self.apply_list:Val()
end

-- 获取对应职位的成员信息权限表
function GuildData:GetJobInfo(m_type)
    if m_type == nil then
        m_type = GuildMemberType.Member
        if self:GetMemberVo() then
            m_type = self:GetMemberVo().post
        end
    end
    if self.member_type and self.member_type[m_type + 1] then
        return self.member_type[m_type + 1]
    end
    return self.member_type[m_type] or {}
end

-- 获得对应职位的名称
function GuildData:GetJobName(m_type)
    if m_type then
        local cfg = self:GetJobInfo(m_type)
        if not TableIsEmpty(cfg) then
            return cfg.member_name
        end
    end
    return ""
end

-- 获取成员信息权限表
function GuildData:GetPostList()
    return self.member_type
end
function GuildData:GetSelectPost()
    return self.select_post_data
end
function GuildData:SetSelectPost(data)
    self.select_post_data = data
end
function GuildData:SetSelectMember(data)
    self.select_member = data
end
function GuildData:GetSelectMember()
    return self.select_member
end
-- 获取自己的身份
function GuildData:GetMemberVo(uid)
    uid = uid or RoleData.Instance:GetBaseData().role_id
    for k, vo in pairs(self.member_info.member_list) do
        if vo.uid == uid then
            return vo
        end
    end
    return nil
end

function GuildData:GetShopInfo(index)
    --[[ local temp = {}
    for i,v in ipairs(self.family_shop) do
        if v.page == index then
            table.insert(temp, v)
        end
    end
    return temp ]]
    local t = CfgTableCopy(self.family_shop, 1)
    for k, v in pairs(t) do
        if v.cost_discount == 100 then
            v.gongxian_cost = math.floor(v.gongxian_cost * GuildBuildingData.Instance:ShopConst() / 100)
            v.cost_discount = GuildBuildingData.Instance:ShopConst()
            v.is_fixed_discount = false
        else
            v.is_fixed_discount = true
        end
    end
    return t
end

function GuildData:SetShopBuyList(list)
    if table.nums(list) > 1 then
        self.shop_buy_list:Set(list)
    else
        for seq, v in pairs(list) do
            self.shop_buy_list[seq] = v
        end
    end
    self.guild_shop_first_item.flush = not self.guild_shop_first_item.flush
end

function GuildData:GetShopBuyCount(seq)
    if self.shop_buy_list[seq] then
        return self.shop_buy_list[seq].buy_count
    end
    return 0
end

function GuildData:SetGuildPetInfo(list)
    self.guild_pet_list:Set(list)
end

function GuildData:GetGuildPetList()
    return self.guild_pet_list
end

function GuildData:HasPetOnTraining()
    for k,v in pairs(self.guild_pet_list) do
        if v.pet_index > -1 then
            return true
        end
    end
end

function GuildData:SetGuildTrainInfo(list)
    self.guild_train_list:Set(list)
end
function GuildData:GetGuildTrainList()
    return self.guild_train_list
end

function GuildData:GetLockInfo(index)
    return self.train_up[index]
end
function GuildData:TrainSkillInfo(index)
    return self.guild_train_list[index]
end
function GuildData:CheckTrainInfo()
    self.train_up_info = {}
    for i, v in ipairs(self.train_up) do
        if self.train_up_info[v.type] == nil then
            self.train_up_info[v.type] = {}
        end
        self.train_up_info[v.type][v.level] = v
    end
end
function GuildData:GetTrainTime()
    -- 默认和加成
    local skill = self.train_up_info[3][self:TrainSkillInfo(3).level]
    if skill == nil then
        return self.guild_other.train_time
    end
    return self.guild_other.train_time + skill.addition
end

function GuildData:GetTrainUpInfo(type, level)
    return self.train_up_info[type][level]
end
function GuildData:GetTrainMaxLevel(type)
    return table.nums(self.train_up_info[type])
end
function GuildData:GetTrainExpInfo(level)
    return self.train_exp[level]
end

function GuildData:SetTrainResult(protocol)
    self.train_result = {
        pet_index = protocol.pet_index,
        time = protocol.time,
        exp = protocol.exp
    }
end
function GuildData:GetTrainResult()
    return self.train_result
end

function GuildData:GetBuildList()
    return self.family_build
end

function GuildData:GetDonateList(build_seq)
    return self.build_donate_list[build_seq]
end
function GuildData:SetBuildFlag(list)
    self.build_flag = list
end
function GuildData:SetBuildValue(list)
    self.build_value = list
end
function GuildData:SetBuildDonate(list)
    self.build_donate_count = list
    self.build_info_flush.num = self.build_info_flush.num + 1
end
function GuildData:GetBuildFinish(build_seq)
    if build_seq == 0 then
        return true
    end
    return ((self.build_flag[32 - build_seq] or 0) == 1)
end

function GuildData:GetBuildValue(build_seq)
    return self.build_value[build_seq] or 0
end
function GuildData:GetDonateCount(index)
    return self.build_donate_count[index] or 0
end

function GuildData:CalcuDonateResult(index, num)
    self.build_goods[index] = num
    self.build_goods_flush.num = self.build_goods_flush.num + 1
end

function GuildData:GetGoodsInfo()
    return self.build_goods
end

function GuildData:ClearGoodsInfo()
    self.build_goods = {}
    self.build_goods_flush.num = 0
    self.build_info_flush.num = 0
end

function GuildData:SetGuildTaskAllInfo(protocol)
    local list = {}
    -- local finished = 0
    TaskData.Instance:DelAllTypeTask(TaskConfig.TaskType.guild)
    self.guild_task_list = SmartData.New()
    for i, v in ipairs(protocol.task_list) do
        -- if v.status == DAILY_GUILD_TASK_STATUS.FINISHED then
        --     finished = finished + 1
        -- end
        if self.guild_task_list[i] then
            self.guild_task_list[i]:Set(v)
        else
            self.guild_task_list[i] = SmartData.New(v)
        end
        self:UpdateGuildTask(v, false)
        -- table.insert(list, v)
    end

    self:UpdateTodayCount()
    -- ActivityData.Instance:SetActivityFinishInfo(ActType.GuildTask, finished, #protocol.task_list)
    -- if self.need_check_follow then
    --     table.sort(
    --         list,
    --         function(a, b)
    --             return a.id < b.id
    --         end
    --     )
    -- end
    -- for i, v in ipairs(list) do
    --     if self.need_check_follow and v.status == DAILY_GUILD_TASK_STATUS.ACCEPTED then
    --         self:UpdateGuildTask(v, true)
    --         self.need_check_follow = false
    --     else
    --         self:UpdateGuildTask(v, false)
    --     end
    -- end

    -- local before = #self.guild_task_list
    -- local now = #protocol.task_list
    -- if before > now then
    --     for i = now + 1, before do
    --         self.guild_task_list[i] = nil
    --     end
    -- end
end

function GuildData:SetGuildTaskSingleInfo(protocol)
    local task = {
        id = protocol.id,
        status = protocol.status,
        progress = protocol.progress,
        is_new = protocol.is_new == 1
    }
    local flag = false
    for i, v in ipairs(self.guild_task_list) do
        if v.id == task.id then
            self.guild_task_list[i]:Set(task)
            flag = true
        end
    end
    if not flag then
        table.insert(self.guild_task_list, SmartData.New(task))
    end

    self:UpdateGuildTask(task, true)
    self:UpdateTodayCount()
end

function GuildData:GetGuildTaskIndexById(id)
    for k, v in pairs(self.guild_task_list) do
        if v.id == id then
            return k - 1
        end
    end
end

function GuildData:GetGuildTaskShowList()
    local list = {}
    for i, v in ipairs(self.guild_task_list) do
        if v.id > 0 then
            table.insert(list, v)
        end
    end
    table.sort(
        list,
        function(a, b)
            local function get_score(data)
                local score = data.id
                if data.status == DAILY_GUILD_TASK_STATUS.ACCEPTABLE then
                    score = score + 1000000
                elseif data.status == DAILY_GUILD_TASK_STATUS.ACCEPTED then
                    score = score + 10000
                elseif data.status == DAILY_GUILD_TASK_STATUS.FINISHED then
                    score = score + 100
                end
                return score
            end
            return get_score(a) > get_score(b)
        end
    )
    return list
end

function GuildData:GetGuildTaskNum()
    return self.guild_task_cfg.other[1].daily_num
end

function GuildData:UpdateTodayCount()
    local remain = 0
    local finished = 0
    for _, task in pairs(self.guild_task_list) do
        if task.status == DAILY_GUILD_TASK_STATUS.FINISHED then
            finished = finished + 1
        end
    end
    ActivityData.Instance:SetActivityFinishInfo(ActType.GuildTask, finished, self.guild_task_list:Length())
end

function GuildData:UpdateFakeGuildTask()
    local npc_id = self:GetGuildTaskNpcId()
    local taskvo = CommonStruct.CustomTask()
    local id = CUSTOM_OBJID.GUILD_TASK_BEGIN
    local task_list = TaskCtrl.Instance.data.task_list
    if not task_list[id] then
        task_list[id] = SmartData.New(TaskInfoGuild.New(true):TaskInfo())
    end
    local task = task_list[id]

    taskvo.task_id = id
    taskvo.npc_id = npc_id
    task:TaskVo(taskvo)
    task:SetCustomStr(Language.DailyGuildTask.Title)
    task.dont_show = true
    task.sort_key = 0
    -- task.to_npc = true
    TaskCtrl.Instance.data:TaskInfoUpdateByTaskId(id, task)

    ActivityData.Instance:CustomClickHandle(
        ActType.GuildTask,
        function()
            local cfg = BehaviorCtrl.GetCfg(4, true)
            local npc_id = GuildCtrl.Instance.data:GetGuildTaskNpcId()
            if npc_id then
                cfg.param1 = npc_id
                cfg.behavior_type = 6
                BehaviorCtrl.ExeByCfgVo(cfg)
            else
                PublicPopupCtrl.Instance:Center(ErrorText[52])
            end
        end
    )
end

function GuildData:UpdateGuildTask(data, is_continue)
    if data.id == nil or data.id == 0 or data.status == DAILY_GUILD_TASK_STATUS.ACCEPTABLE then
        return
    end

    local id = CUSTOM_OBJID.GUILD_TASK_BEGIN + data.id

    if data.status == DAILY_GUILD_TASK_STATUS.FINISHED then
        TaskCtrl.Instance.data:TaskInfoDeleteByTaskId(id, TaskConfig.TaskType.guild)
        -- if TaskData.Instance.task_trace_list[TaskConfig.TaskType.guild] == id then
        --     for i, _ in pairs(self.task_list) do
        --         TaskData.Instance.task_trace_list[TaskConfig.TaskType.guild] = i
        --         break
        --     end
        -- end
        -- TaskCtrl.Instance.data:TaskInfoDeleteByTaskId(id)
        if is_continue then
            if not TaskData.Instance:GoTraceTaskByTaskType(TaskConfig.TaskType.guild) then
                SceneLogic.Instance:ClearMoveCache()
            end
        end
        return
    elseif data.status == DAILY_GUILD_TASK_STATUS.ACCEPTED then
        TaskData.Instance:TraceTaskInit(TaskConfig.TaskType.guild, id)
    end

    local cfg = self.guild_task_cfg.task[data.id]
    if cfg == nil then
        return
    end
    local taskvo = CommonStruct.CustomTask()

    local task_list = TaskCtrl.Instance.data.task_list
    -- local is_new = false
    if not task_list[id] then
        task_list[id] = SmartData.New(TaskInfoGuild.New():TaskInfo())
    -- is_new = true
    end
    local task = task_list[id]

    taskvo.task_id = id
    taskvo.task_name = cfg.task_name

    local monster_name = Cfg.MonsterById(cfg.c_param1, false).name
    local scene_name = SceneData.Instance:GetSceneCfg(cfg.target_scene).name
    local goal_desc = string.format(Language.DailyGuildTask.GoalDesc, cfg.c_param2, monster_name)
    local goal_desc_menu = string.format(Language.DailyGuildTask.GoalDescMenu, cfg.c_param2, monster_name)
    local task_desc = string.format(Language.DailyGuildTask.TaskDesc, scene_name, cfg.c_param2, monster_name)
    local finish_desc = string.format(Language.DailyGuildTask.FinishDesc, cfg.c_param2, monster_name)

    taskvo.task_desc = task_desc

    if data.progress >= cfg.c_param2 then
        if TaskData.Instance.task_trace_list[TaskConfig.TaskType.guild] == id and self.pre_task_progress[id] and self.pre_task_progress[id] < cfg.c_param2 then
            if SceneLogic.Instance.move_to_pos_cache.guaji then
                SceneLogic.Instance.move_to_pos_cache.guaji = false
                if not MainViewData.Instance:IsBattle() then
                    SceneLogic.Instance:AutoToNpc(GuildCtrl.Instance.data:GetGuildTaskNpcId(), nil, false, true)
                else
                    self.auto_commit_after_battle =
                        EventSys:Bind(
                        GlobalEventKeys.BattleEnd,
                        function()
                            self.need_switch_scene = true
                            EventSys:UnBind(self.auto_commit_after_battle)
                            self.auto_commit_after_battle = nil
                        end
                    )
                end
            end
        end
        local npc_id = self:GetGuildTaskNpcId()
        taskvo.npc_id = npc_id
        local npc_name = ""
        for k, v in pairs(Config.npc_auto.npc_list) do
            if v.id == npc_id then
                npc_name = v.name
                break
            end
        end
        taskvo.task_goal = string.format(Language.DailyGuildTask.TalkTo, npc_name)
        -- task.to_npc = true
        task.is_finished = true
        task:SetCustomStr(cfg.task_name)
    else
        local goal =
            string.format(
            "%s(%d/%d)",
            goal_desc,
            math.min(data.progress, cfg.c_param2),
            -- data.progress,
            cfg.c_param2
        )
        local goal_desc_menu =
        string.format(
        "%s(%d/%d)",
        goal_desc_menu,
        math.min(data.progress, cfg.c_param2),
        -- data.progress,
        cfg.c_param2
    )
        taskvo.task_goal = goal_desc
        taskvo.task_goal_menu = goal_desc_menu
        task.is_finished = false
    end

    local reward_list = {}
    for k, v in pairs(self.guild_task_cfg.reward_group) do
        if v.group_id == cfg.reward_group then
            local item = Item.New()
            item.item_id = v.reward.item_id
            item.is_bind = v.reward.is_bind == 1
            item.num = 1
            table.insert(reward_list, item)
        end
    end
    taskvo.reward_list = reward_list
    task:TaskVo(taskvo)
    task.target_scene = cfg.target_scene

    if not TaskData.Instance.task_trace_list[TaskConfig.TaskType.guild] then
        TaskData.Instance.task_trace_list[TaskConfig.TaskType.guild] = id
    end
    task.task_finish_desc = finish_desc
    task.real_id = data.id
    task.sort_key = data.id
    task.is_new.val = data.is_new
    task.can_ab = false
    -- task:SetCustomStr(Language.DailyGuildTask.Title)
    TaskCtrl.Instance.data:TaskInfoUpdateByTaskId(id, task)
    self.task_list[id] = task
    self.pre_task_progress[id] = data.progress
end

function GuildData:GetGuildTaskNpcId()
    local prof = RoleData.Instance:GetBaseData().profession
    for k, v in pairs(self.guild_task_cfg.npc) do
        if v.job_type == prof then
            return Config.npc_dynamic_auto.npc_dynamic_cfg[v.accept_npc].id
        end
    end
end

function GuildData:GetActivityBackRewardList()
    return RewardData.Instance:GetActivityBackRewardList(Config.jobtask_auto.reward_find, Config.jobtask_auto.reward_group)
end
function GuildData:SetMeleeRankAllInfo(protocol)
    self.melee_rank_all_info:Set(protocol.rank_all_info)
end
function GuildData:MeleeRankAllInfo()
    return self.melee_rank_all_info
end
function GuildData:SetMeleeMyRankInfo(protocol)
    self.melee_my_rank:Set(protocol.my_family_info)
end
function GuildData:MeleeMyRankInfo()
    return self.melee_my_rank
end
function GuildData:SetMeleeMyGuildNum(num)
    self.melee_join_member_num = num
end
function GuildData:IsInGuildFightFB()
    return SceneData.Instance:SceneId() == Config.family_war_auto.other[1].map
end
function GuildData:IsInGuildHonorFB(in_scene, prepare)
    if in_scene == true then
        --[[ if prepare == true then
            --布阵期间
            local info = GuildData.Instance:GuildHonorInfo()
            return (info.state == GUILD_HONOR_BATTLE_STATE.ENTER or info.state == GUILD_HONOR_BATTLE_STATE.SET) and SceneData.Instance:GetMode() == SCENE_TYPE.CHIEF_HONOR
        end ]]
        return SceneData.Instance:GetMode() == SCENE_TYPE.CHIEF_HONOR
    else
        local state = ActivityData.Instance:GetActivityStatus(ActType.GuildEmbattle)
        return state == ActStatusType.Open and SceneData.Instance:GetMode() == SCENE_TYPE.CHIEF_HONOR
    end
end
function GuildData:BosConfig(id)
    return self.family_war_box[id]
end
function GuildData:SetDefeatedTimes(time)
    if time == 0 then
        self.defeated_times = 0
    end
    self.defeated_times = self.defeated_times + time
end
function GuildData:GetMeleeReward(rank)
    local rank_reward_list = {}
    local role_level = RoleData.Instance:GetRoleLevel()
    for index, value in ipairs(self.family_war_auto.rank_reward) do
        if value.level_min <= role_level and value.level_max >= role_level then
            for i = value.rank_min, value.rank_max do
                rank_reward_list[i] = {}
                rank_reward_list[i].reward_show = TableCopy(value.reward_show)
                --[[ for _, reward in pairs(self.family_war_other.join_reward) do
                    table.insert(rank_reward_list[i].reward_show, reward)
                end ]]
                rank_reward_list[i].gongxian = (value.gongxian or 0) --[[ + self.family_war_other.gongxian ]]
                rank_reward_list[i].coin = (value.coin or 0) --[[ + self.family_war_other.coin ]]
            end
        end
    end
    return rank_reward_list[rank] or {reward_show = self.family_war_other.join_reward, title = 0, gongxian = self.family_war_other.gongxian, coin = self.family_war_other.coin}
end
function GuildData:IsOpenGuildMelee(in_scene, prepare)
    if in_scene == true then
        if prepare == true then
            local state = ActivityData.Instance:GetActivityStatus(ActType.GuildMelee)
            return state == ActStatusType.Open and SceneData.Instance:GetMode() == SCENE_TYPE.GUILD_FIGHT
        end
        return SceneData.Instance:GetMode() == SCENE_TYPE.GUILD_FIGHT
    else
        local enter_info = ActivityData.Instance:GetActivityEnterInfo(ActType.GuildMelee)
        local stamp_time = enter_info.enter_end_timestamp or 0
        local state = ActivityData.Instance:GetActivityStatus(ActType.GuildMelee)
        return state == ActStatusType.Open and SceneData.Instance:GetMode() == SCENE_TYPE.GUILD_FIGHT and stamp_time == 0
    end
end

function GuildData:SetPlayerApplyList(list)
    self.player_apply_list:Set(list)
end

function GuildData:SetNeedApproval(is_need_approval)
    self.guild_info.is_need_approval = is_need_approval or 0
end

function GuildData:CheckIsApply(guild_id)
    return self.player_apply_list[guild_id] ~= nil
end
-------------------红点-----------------
function GuildData:CheckApplyCount()
    --判断权限
    local info = self:GetJobInfo()
    if next(info) then
        if info.approval == 1 then
            return self.apply_count.count > 0 and 1 or 0
        end
    end
    return 0
end
function GuildData:CheckPetTrain()
    -- 如果有已完成那么就显示红点
    local num = 0
    for _, info in ipairs(self.guild_pet_list) do
        if info.pet_index ~= -1 then
            local finish_time = info.begin_time + self:GetTrainTime()
            if (finish_time - TimeHelper.GetServerTime()) < 0 then
                num = num + 1
                return num
            end
        end
    end
    return num
end
----------------家族荣誉战-------------
--活动状态
function GuildData:SetGuildHonorInfo(protocol)
    self.guild_honor_info = protocol
end
function GuildData:GuildHonorInfo()
    return self.guild_honor_info
end
--设置信息
function GuildData:SetHonorRoleInfo(protocol)
    self.honor_role_info_id:Set(protocol.role_info)
    self.honor_role_info_zone:Set(protocol.zone_info)
end
--删除
function GuildData:DeleteHonorRoleInfo(role_id, zone_id, move)
    if move ~= true then
        self.honor_role_info_id[role_id] = nil
    end
    if self.honor_role_info_zone:Val()[zone_id] == nil then
        self.honor_role_info_zone[zone_id] = {}
    end
    if move == true then
        local temp_list = self.honor_role_info_zone[zone_id]
        for index, value in ipairs(temp_list) do
            if value.role_id == role_id then
                table.remove(self.honor_role_info_zone[zone_id], index)
                --self.honor_role_info_zone[zone_id][index] = nil
            end
        end
    else
        for index, value in pairs(self.honor_role_info_zone) do
            for i,v in ipairs(value) do
                if v.role_id == role_id then
                    table.remove(self.honor_role_info_zone[v.zone_id], i)
                    --self.honor_role_info_zone[zone_id][index] = nil
                end
            end
        end
    end
end
--移动
function GuildData:MoveHonorRoleInfo(role_id, zone_id)
    local temp = self.honor_role_info_id[role_id]
    self:DeleteHonorRoleInfo(temp.role_id, temp.zone_id, true)
    --删除原来的
    -- self.honor_role_info_zone[temp.zone_id][temp.role_id] = nil
    temp.zone_id = zone_id
    if self.honor_role_info_zone:Val()[zone_id] == nil then
        self.honor_role_info_zone[zone_id] = {}
    end
    table.insert(self.honor_role_info_zone[zone_id], temp)
end
function GuildData:AddHonorRoleInfo(role_info)
    self.honor_role_info_id[role_info.role_id] = role_info
    if self.honor_role_info_zone:Val()[role_info.zone_id] == nil then
        self.honor_role_info_zone[role_info.zone_id] = {}
    end
    table.insert(self.honor_role_info_zone[role_info.zone_id], role_info)
end
-- 在场未分配列表 0
-- 各区列表
function GuildData:GetHonorRoleInfo(zone_id)
    return self.honor_role_info_zone[zone_id]
end

function GuildData:GetHonorShiLianScore()
    local world_level = RoleData.Instance:GetWorldLevel()
    local score = 0
    for _, v in ipairs(self.family_boss) do
        if world_level >= v.world_levels then
            score = v.join_gongxian
        else
            break
        end
    end
    return score
end

function GuildData:GetMyZone()
    local role_id = RoleData.Instance:GetRoleId()
    local res = 0
    for k, v in pairs(self.honor_role_info_zone) do
        for k2, v2 in pairs(v) do
            if role_id == v2.role_id then
                res = k
                break
            end
        end
    end
    return res
end
--区域结果
function GuildData:SetZoneInfo(info)
    for i = 1, 3 do
        if self:GetGuildID() == info[i] then
            info[i] = 2
        else
            --判断是不是对方赢了
            if self.honor_battle_info then
                if self.honor_battle_info.ey_guild == info[i] then
                    info[i] = 3
                end
            end
        end
    end
    self.honor_zone_info:Set(info)
end
--修改
function GuildData:ChangeZoneInfo(id, info)
    local guild_info = info
    if self:GetGuildID() == info then
        guild_info = 2
    else
        --判断是不是对方赢了
        if self.honor_battle_info then
            if self.honor_battle_info.ey_guild == info then
                guild_info = 3
            end
        end
    end
    self.honor_zone_info[id] = guild_info
end
function GuildData:GetZoneInfo(id)
    return self.honor_zone_info[id] or 0
end
function GuildData:ZoneInfo()
    return self.honor_zone_info
end
--人数
function GuildData:SetZoneNum(info)
    self.honor_zone_nums:Set(info)
end
function GuildData:GetZoneNum()
    self.show_zone_id = self.show_zone_id or self:GetMyZone()
    return self.show_zone_id, self.honor_zone_nums[self.show_zone_id]
end
function GuildData:SetShowZoneId(id)
    self.show_zone_id = id
end
--对战信息
function GuildData:SetHonorBattleInfo(info)
    if info.guild_id1 == self:GetGuildID() then
        info.my_guild = info.guild_id1
        info.my_guild_name = info.guild_name1
        info.ey_guild = info.guild_id2
        info.ey_guild_name = info.guild_name2
    else
        info.my_guild = info.guild_id2
        info.my_guild_name = info.guild_name2
        info.ey_guild = info.guild_id1
        info.ey_guild_name = info.guild_name1
    end
    self.honor_battle_info = info
end
function GuildData:HonorBattleInfo()
    return self.honor_battle_info
end

-----------------周任务------------
function GuildData:SetGWTAllInfo(protocol)
    self.gwt_all_info:Set(protocol)
    local all_info, task_info = self:GWTTaskInfo()
    if all_info and task_info and all_info.task_prog >= task_info.param_2 then
        self.act_finish_times[ActType.GuildWeekTask] = 1
    else
        self.act_finish_times[ActType.GuildWeekTask] = 0
    end
end

function GuildData:SetGWTRankList(protocol)
    self.gwt_rank_info = protocol
end

function GuildData:GWTTaskInfo()
    return self.gwt_all_info, self.family_mission_mission[self.gwt_all_info.task_id]
end

function GuildData:GWTRankInfo()
    return self.gwt_rank_info.count, self.gwt_rank_info.rank_list
end

function GuildData:GWTRewardInfo(times)
    times = times == 0 and 1 or times
    return self.family_mission_reward[times]
end

function GuildData:GWTRankReward(gongxian)
    local seq = 99
    local min = math.min
    for index, value in ipairs(self.family_mission_gongxian_reward) do
        if value.gongxian_limit <= gongxian then
            seq = min(seq, value.seq)
        end
    end
    return self.family_mission_gongxian_reward[seq]
end
------------家族试炼BOSS-------------
function GuildData.GetChapterDefaultDate(chapter_id)
    local data = {}
    data.guild_boss_chapter_id = chapter_id or 0 --今天章节 从0开始 0~5
    data.guild_boss_seq = 0 --boss位置 下标
    data.guild_boss_play_times = 0 --挑战次数
    data.guild_max_chapter = 0 --最高通关章节
    data.remain_hp_percent = 10000 --剩余百分比 统一万分比
    data.has_open = false
    return data
end

function GuildData:SetGuildTrialPage(index)
    self.guild_trial_page.index = index
end

function GuildData:GetTrialSelectRewardList(chapter_id)
    return self.guild_trial_select_reward[chapter_id]
end

function GuildData:GetTrialRankRewardList(rank_index)
    local chapter_reward = self:GetTrialRankRewardGroup()
    for i,v in ipairs(chapter_reward) do
        if rank_index <= v.max_rank then
            return v.reward_show
        end
    end
    return {}
end

function GuildData:GetTrialRankRewardGroup(chapter_id)
    chapter_id = chapter_id or self.guild_boss_info.guild_boss_chapter_id
    return self.guild_trial_rank_reward[chapter_id]
end

function GuildData:SetGuildBossInfo(info)
    self.guild_boss_info:Set(info)
--[[    if self.cache_guild_boss_seq == 0 then
        self.cache_guild_boss_seq = self.guild_boss_info.guild_boss_seq
    end--]]
    self.act_finish_times[ActType.GuidlTrial] = self.guild_boss_info.boss_participate_reward_fetch_flag
end

function GuildData:SetGuildBossRank(info)
    LogDG(info)
    self.guild_boss_rank:Set(info.rank_list)
    self.guild_boss_rank_count = info.count
end

function GuildData:GetMyRankData()
    local role_id = RoleData.Instance:GetRoleId()
    for i=1,self.guild_boss_rank_count do
        if self.guild_boss_rank[i].role_id == role_id then
            return false, {index = i, total_damage = self.guild_boss_rank[i].total_damage, role_name = self.guild_boss_rank[i].role_name, prof_base = self.guild_boss_rank[i].prof_base}
        end
    end
    return true, {total_damage = 0, role_name = RoleData.Instance:GetRoleName(), prof_base = RoleData.Instance:ProfType()}
end

function GuildData:GuildBossInfo()
    return self.guild_boss_info
end

function GuildData:GuildBossConfig(id, seq)
    return self.family_boss_info[id or 0][seq or 0]
end

--累计击杀的boss奖励合并
function GuildData:GetGuildBossReward(chapter_id, seq)
    local reward_list = self.family_boss_reward[chapter_id]
    local role_level = RoleData.Instance:GetRoleLevel()
    local result = nil
    for k,v in pairs(reward_list) do
        if role_level <= k and role_level >= v.min_level then
            result = v.item_list
        end
    end

    if result == nil then result = {} end 

    return result
end

function GuildData:ChapterConfig(id)
    return self.family_boss_info[id or 0]
end

function GuildData:BossBlood()
    -- 返回总血
    local cfg = self:GuildBossConfig(self.guild_boss_info.guild_boss_chapter_id, self.guild_boss_info.guild_boss_seq)
    if cfg == nil then
        return 0, 0
    end
    local monster_group_cfg = Cfg.MonsterGroupById(cfg.boss_id)
    local total_hp = 0
    for i = 0, 9 do
        if monster_group_cfg["monster_id_"..tostring(i)] ~= 0 and monster_group_cfg["monster_id_"..tostring(i)] ~= "" then
            total_hp = Cfg.MonsterById(monster_group_cfg["monster_id_"..tostring(i)]).maxhp + total_hp
        end
    end
    --local boss_cfg = Cfg.MonsterById(Cfg.MonsterGroupById(cfg.boss_id).show_monster) --Config.monster_auto.monster[cfg.boss_id]
    local cur_blood = self.guild_boss_info.remain_hp_percent / 10000 * total_hp -- boss_cfg.maxhp * 
    return cur_blood, total_hp -- boss_cfg.maxhp
end

function GuildData:ShowTrialPoint()
    local level = RoleData.Instance:GetRoleLevel()
    local list = self:GetActList()
    local cfg = nil
    local limit_level = 0
    for i, v in ipairs(list) do
        if v.act_type == ActType.GuidlTrial then
            cfg = ActivityData.GetConfig(v.act_type)
            limit_level = v.open_level
            -- 开启等级，如果活动有就拿活动的
            if not TableIsEmpty(cfg) then
                limit_level = cfg.level_restrict
            end
            break
        end
    end
    if level >= limit_level
    and self.guild_boss_info.boss_participate_reward_fetch_flag
    and self.guild_boss_info.boss_participate_reward_fetch_flag ~= 1  then
        return 1
    end
    return 0
	--屏蔽家族试炼红点
--[[	local condition_1 = false
	local count = self:GetActCount(ActType.GuidlTrial)
	local max_count = self:TrialMaxTimes()
	local have_count = max_count - count
	if have_count > 0 then
		condition_1 = true
	end
	local condition_2 = true
	if self.guild_boss_info.guild_boss_chapter_id then
		local boss_count = #self.family_boss_info[self.guild_boss_info.guild_boss_chapter_id]
		if self.guild_boss_info.guild_boss_seq > boss_count then
			condition_2 = false
		end
	end
	if condition_1 and condition_2 and not self.guild_trial_point_have_show then
		GuildData.Instance.guild_trial_point.count = 1
	else
		GuildData.Instance.guild_trial_point.count = 0
	end--]]
end

function GuildData:TrialMaxTimes()
    return self.family_auto.family_activity[4].activity_num
end

function GuildData:BossRankList()
    return self.guild_boss_rank
end

function GuildData:SetGuildTrialTotalRank(protocol)
    self.guild_trial_total_rank:Set(protocol)
end

function GuildData:GetTotalRankList()
    return self.guild_trial_total_rank
end

function GuildData:SetQuestionInfo(info)
    self.guild_question_info.question_id = info.question_id
    self.guild_question_info.cur_seq = info.cur_seq
    self.guild_question_info.next_time = info.next_time
end

function GuildData:QuestionInfo()
    return self.guild_question_info
end

function GuildData:QuestionConfig(id)
    return self.family_question[id]
end

function GuildData:TotalQuestionNum()
    return self.family_question_other.question_num
end

function GuildData:AnswerTime()
    return self.family_question_other.question_time
end

function GuildData:IsInAnswerScene()
    return SceneData.Instance:SceneId() == self.family_question_other.scene_id
end
function GuildData:IsInAnswerScene2()
    return  SceneData.Instance:GetMode() == SCENE_TYPE.GUILD_ANSWER
end
function GuildData:GetMeleeFailTip()
    GuildData.Instance:SetDefeatedTimes(1)
    return Language.Guild.MeleeFail[self.defeated_times or 1]
end

function GuildData:GetMeleeSuccReward()
    return {{name = Language.Guild.MeleeReward, num = string.format("+%s", Config.family_war_auto.other[1].win_score)}}
end

function GuildData:GetActCount(act_type)
    return self.act_finish_times[act_type] or 0
end

function GuildData:SetInvitationInfo(info)
    self.invitation_info = info
end

--获取入帮邀请
function GuildData:GetInvitationInfo()
    return self.invitation_info or {}
end

function GuildData:SetInvitationFlag(flag)
    self.is_invitation_flag = flag
end

function GuildData:SetFamilyBossMapId(map_id)
    self.failyboss_battle_map_id = map_id
end

function GuildData:GetFamilyBossMapId()
    return self.failyboss_battle_map_id
end

function GuildData:ClearFamilyBossMapId()
    self.failyboss_battle_map_id = nil
end

function GuildData:ChangeDreamInfoList()
	if self.dreaminfolist then
		if self:GetGuildInfo().level ~= nil and self:GetGuildInfo().level > 1 then
			if not self.today_is_publish_gather_dream_red then
				self.guild_jiyuan_get_point.count = 0
			else
				self.guild_jiyuan_get_point.count = 1
			end
		else
			self.guild_jiyuan_get_point.count = 0
		end
		for k, v in pairs(self.dreaminfolist) do
			local sort_id = 0
			local b_type = 1
			for k1, v2 in pairs(self.gift_uid_list) do
				if v.role_id == v2.gift_uid then
					b_type = 3
					break
				end
			end
			
			local role_id = RoleData.Instance:GetRoleId()
			if role_id == v.role_id then
				sort_id = 1				
				if v.gather_num > 0 and v.gather_num > v.fetch_num then
					self.guild_jiyuan_get_point.count = 1
				else
					self.guild_jiyuan_get_point.count = 0
				end
			end
			v.b_type = b_type
			v.sort_id = sort_id
		end
		table.sort(self.dreaminfolist, function(a, b)
			return  a.sort_id > b.sort_id 
		end)
		local fresh_list = {}
		for k, v in pairs(self.dreaminfolist) do
			table.insert(fresh_list,v)
		end
		self.guild_jiyuan_list:Set(fresh_list)	
	end
end

function GuildData:SetRoleGiftGatherDreamList(gift_uid_list)
	self.gift_uid_list = gift_uid_list
	self:ChangeDreamInfoList()
end

function GuildData:SetGatherDreamInfoList(list)
	self.dreaminfolist = list
	self:ChangeDreamInfoList()

end

function GuildData:GetGatherDreamInfoList()
    return self.guild_jiyuan_list
end

function GuildData:SetGatherDreamSignleInfo(dreaminfo)
	local need_add = true
	for k, v in pairs(self.dreaminfolist) do
		if v.role_id == dreaminfo.role_id then
			v.from_name = dreaminfo.from_name
			v.item_id = dreaminfo.item_id
			v.avatar_id = dreaminfo.avatar_id
			v.gather_num = dreaminfo.gather_num
			v.fetch_num = dreaminfo.fetch_num
			v.need_max_num = dreaminfo.need_max_num
			v.help_num = dreaminfo.help_num
			v.publish_timestamp = dreaminfo.publish_timestamp
			need_add = false
			break
		end
	end
	if need_add then
		table.insert(self.dreaminfolist,dreaminfo)
	end
	GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_ROLE_GIFT_GATHER_DRAEM_LIST})
end

function GuildData:SetRoleGatherDreamGetList(list)
	for k, v in pairs(list) do
		v.info_type = 0
	end
	self.guild_jiyuan_jilu_get_list:Set(list)
end

function GuildData:GetRoleGatherDreamGetList()
    return self.guild_jiyuan_jilu_get_list
end

function GuildData:SetRoleGatherDreamSendList(list)
	for k, v in pairs(list) do
		v.info_type = 1
	end
	self.guild_jiyuan_jilu_send_list:Set(list)
end

function GuildData:GetRoleGatherDreamSendList()
    return self.guild_jiyuan_jilu_send_list
end

function GuildData:SetRoleCanPublishGatherDreamList(list)
	self.guild_jiyuan_fabu_list:Set(list)
end

function GuildData:GetRoleCanPublishGatherDreamList()
    return self.guild_jiyuan_fabu_list
end

function GuildData:GetPetPublishGatherDreamListByQuality(quality)
	local s_quality = quality or 0
	local show_list = {}
	local pet_cfg = Config.pet_cfg_auto.pet_info_2
	local c_family = {}
	for k, v in pairs(Config.family_auto.family_pray) do
		if v.type == 0 then
			table.insert(c_family,v)
		end
	end
	for k, v in pairs(pet_cfg) do
		local need_add = true
		if s_quality ~= 0 and v.quality ~= s_quality then
			need_add = false
		end
		if need_add then
			local patch_id = PetData.Instance:GetPatchIdByPetId(v.id)
            
			local is_can_pray = 0
			for k1, v1 in pairs(c_family) do
				if v.quality == v1.quality then
					is_can_pray = v1.is_can_pray
					break
				end
			end
			if is_can_pray == 1 then
				local is_add = false
				for k, v in pairs(show_list) do
					if patch_id == v then
						is_add = true
						break
					end
				end
                -- if patch_id == 0 and not is_add then
                --     LogError("pet_cfg",k,v)
                -- end
				if not is_add then
					table.insert(show_list,patch_id)
				end
			end
		end
	end
	
	local patch_id_list = {}
    for i=1,#show_list do
        if show_list[i] ~= 0 then
            table.insert(patch_id_list,{patch_id = show_list[i], patch_type = 0})
        end
        -- patch_id_list[i] = {}
        -- patch_id_list[i].patch_id = show_list[i]
		-- patch_id_list[i].patch_type= 0
        -- LogError("show_list",i,patch_id_list[i])
    end
	
	table.sort(patch_id_list,function(a,b)
		local color_a =  Item.GetConfig(a.patch_id).color
		local color_b = Item.GetConfig(b.patch_id).color
		return color_a > color_b 
	end)
		
	return patch_id_list
end

function GuildData:GetPartnerCanPublishGatherDreamList()
	local partner_cfg = PartnerData.Instance:GetOriginalPartnerInfo()
	table.sort(partner_cfg,function(a,b)
		return a.partner_quality > b.partner_quality 
	end)
	local c_family = {}
	for k, v in pairs(Config.family_auto.family_pray) do
		if v.type == 1 then
			table.insert(c_family,v)
		end
	end
	
	local show_list = {}
	for k, v in pairs(partner_cfg) do
		local is_can_pray = 0
		for k1, v1 in pairs(c_family) do
			if v.partner_quality == v1.quality then
				is_can_pray = v1.is_can_pray
				break
			end
		end
		if is_can_pray == 1 then
			table.insert(show_list,v)
		end
	end
	local patch_id_list = {}
    for i=1,#show_list do
        patch_id_list[i] = {}
        patch_id_list[i].patch_id = show_list[i].activation_item
		patch_id_list[i].patch_type = 1
		patch_id_list[i].patch_compose = show_list[i].activation_num
    end
	
	return patch_id_list
end
function GuildData:SetRewardFlags(protocol)
    self.join_level = protocol.level
    self.particip_award_flag = protocol.particip_award_flag
    self.survival_award_flag = protocol.survival_award_flag
    self.melee_nums_info.reward_flush = not self.melee_nums_info.reward_flush
end
--生存奖励
function GuildData:MeleeAliveReward()
    local temp = {}
    local level = self.join_level or RoleData.Instance:GetRoleLevel()
    for i,v in ipairs(self.family_war_auto.live_reward) do
        if level >= v.level_min and level <= v.level_max then
            table.insert( temp, v)
        end
    end
    return temp
end
--参与奖励
function GuildData:GetParticipationReward()
    local item_list = {}
    local level = self.join_level or RoleData.Instance:GetRoleLevel()
    for i,v in ipairs(self.family_war_auto.participation_reward) do
        if level >= v.level_min and level <= v.level_max then
            if v.exp > 0 then
                table.insert(item_list, Item.Init(CommonItemId.Exp,v.exp))
            end
            if v.coin > 0 then
                table.insert(item_list, Item.Init(CommonItemId.Coin,v.coin))
            end
            for k,v in pairs(v.reward) do
                table.insert(item_list, Item.Init(v.item_id, v.num, v.is_bind))
            end
        end
    end
    return item_list
end
function GuildData:GetMeleeTotalPreRewardShow()
    return self:GetMeleeTotalPreReward(true)
end
function GuildData:GetMeleeTotalPreReward(show_only)
    local reward_list = {
        [1] = {
            toggle_name = Language.OnlyFight.BaseReward,
            reward_item_list = {}
        },
        [2] = {
            toggle_name = Language.OnlyFight.RankReward,
            reward_item_list = {}
        },
        [3] = {
            toggle_name = Language.Activity.TipsNotice,
            reward_item_list = {{tips_act_type = ActType.GuildMelee}}
        }
    }

    local function sendGetReward(param)
        local enter_info = ActivityData.Instance:GetActivityEnterInfo(ActType.GuildMelee)
        local stamp_time = enter_info.enter_end_timestamp or 0
        if param.op_type == ActivityConfig.TIME_LIMIT_REWARD_GET_OP.JOIN_AWARD then
            if stamp_time > 0 then
                PublicPopupCtrl.Instance:Center(Language.OnlyFight.EnterCanGet)
                return
            end
        elseif param.op_type == ActivityConfig.TIME_LIMIT_REWARD_GET_OP.SURVIVAL_AWARD then
            if stamp_time > 0 or self.melee_nums_info.resi_role > param.param1 then
                PublicPopupCtrl.Instance:Center(Language.OnlyFight.ConditionNotMatch)
                return
            end
        end
        GuildCtrl.Instance:SendGetMeleeReward(param)
    end
    local enter_info = ActivityData.Instance:GetActivityEnterInfo(ActType.GuildMelee)
    local stamp_time = enter_info.enter_end_timestamp or 0
    stamp_time = stamp_time > 0 and 2 or 0
    --参与奖励
    local partic_temp = {}
    partic_temp.title_name = Language.OnlyFight.JoinReward
    -- partic_temp.btn_flag = self.particip_award_flag    --是否已领取，用于按钮的置灰
    -- partic_temp.click_func = sendGetReward
    -- partic_temp.func_param = {op_type = 0} --回调接口参数
    if not show_only then
        partic_temp.btn_flag = self.particip_award_flag == 0 and stamp_time or self.particip_award_flag    --是否已领取，用于按钮的置灰
        partic_temp.click_func = sendGetReward
        partic_temp.func_param = {op_type = ActivityConfig.TIME_LIMIT_REWARD_GET_OP.JOIN_AWARD, activity_type = ActType.GuildMelee} --回调接口参数
    end
    partic_temp.item_list = self:GetParticipationReward()
    table.insert(reward_list[1].reward_item_list, partic_temp)

    --存活奖励
    local alive_reward_item_list = self:MeleeAliveReward()
    local alive_temp = {}
    alive_temp.child_list = {}
    alive_temp.title_name = Language.OnlyFight.AliveReward
    local role_num = self.melee_nums_info.resi_role --self:MeleeRankAllInfo().resi_role and self:MeleeRankAllInfo().resi_role - 1 or 1
    for i,v in ipairs(alive_reward_item_list) do
        local child_temp = {}
        child_temp.child_title = string.format(Language.OnlyFight.AliveLeft, v.live_num)
        child_temp.item_list = {}
        if v.exp > 0 then
            table.insert(child_temp.item_list, Item.Init(CommonItemId.Exp,v.exp))
        end
        if v.coin > 0 then
            table.insert(child_temp.item_list, Item.Init(CommonItemId.Coin,v.coin))
        end
        for _,w in pairs(v.reward) do
            table.insert(child_temp.item_list, Item.Init(w.item_id, w.num, w.is_bind))
        end
        -- child_temp.click_func = sendGetReward
        -- child_temp.func_param = {op_type = 1, param1 = v.live_num}
        
        if not show_only then
            child_temp.click_func = sendGetReward
            child_temp.func_param = {op_type = ActivityConfig.TIME_LIMIT_REWARD_GET_OP.SURVIVAL_AWARD, param1 = v.live_num}
            if stamp_time > 0 then
                child_temp.btn_flag = stamp_time
            elseif self.survival_award_flag[#self.survival_award_flag + 1 - i] == 1 then
                child_temp.btn_flag = self.survival_award_flag[#self.survival_award_flag + 1 - i]
            else
                child_temp.btn_flag = role_num > v.live_num and 2 or 0
            end
            --[[ if self.survival_award_flag then
                child_temp.btn_flag = self.survival_award_flag[#self.survival_award_flag + 1 - i]
            end ]]
        end
        table.insert(alive_temp.child_list, child_temp)
    end
    table.insert(reward_list[1].reward_item_list, alive_temp)
    local role_level = RoleData.Instance:GetRoleLevel()
    for k,v in ipairs(self.family_war_auto.rank_reward) do
        if v.level_min <= role_level and v.level_max >= role_level then
            local temp = {}
            if v.rank_min == v.rank_max then
                temp.title_name = string.format(Language.OnlyFight.RankTitleFormat[1], v.rank_min)
            else
                temp.title_name = string.format(Language.OnlyFight.RankTitleFormat[2], v.rank_min, v.rank_max)
            end
            if self.survival_award_flag then
                temp.tip_str = Language.OnlyFight.RankRewardTip
            end
            temp.item_list = {}
            if v.coin > 0 then
                table.insert(temp.item_list, Item.Init(CommonItemId.Coin,v.coin))
            end
            if v.gongxian > 0 then
                table.insert( temp.item_list, Item.Init(CommonItemId.GongXian, v.gongxian))
            end
            for _,w in pairs(v.reward_show or RO_EMTPY_TAB) do
                table.insert(temp.item_list, Item.Init(w.item_id, w.num, w.is_bind))
            end
            table.insert(reward_list[2].reward_item_list, temp)
        end
    end
    return reward_list
end
function GuildData:GetAnswerRightReward()
    local join_level =  RoleData.Instance:GetRoleLevel()
    local cfg = self.family_question_cfg_auto.right_reward[join_level]
    local temp = {}
    if cfg.coin > 0 then
        table.insert(temp, Item.Init(CommonItemId.Coin,cfg.coin))
    end
    if cfg.gongxian > 0 then
        table.insert(temp, Item.Init(CommonItemId.GongXian, cfg.gongxian))
    end
    if cfg.family_coin > 0 then
        table.insert(temp, Item.Init(CommonItemId.GuildMoney, cfg.family_coin))
    end
    if cfg.exp > 0 then
        table.insert(temp, Item.Init(CommonItemId.Exp, cfg.exp))
    end
    local list = string.split(cfg.item_show_group, "|")
    for index, value in ipairs(list) do
        table.insert(temp, Item.Init(tonumber(value)))
    end
    return temp
end
function GuildData:GetAnswerErrorReward()
    local join_level =  RoleData.Instance:GetRoleLevel()
    local cfg = self.family_question_cfg_auto.wrong_reward[join_level]
    local temp = {}
    if cfg.coin > 0 then
        table.insert(temp, Item.Init(CommonItemId.Coin,cfg.coin))
    end
    if cfg.gongxian > 0 then
        table.insert(temp, Item.Init(CommonItemId.GongXian, cfg.gongxian))
    end
    if cfg.family_coin > 0 then
        table.insert(temp, Item.Init(CommonItemId.GuildMoney, cfg.family_coin))
    end
    if cfg.exp > 0 then
        table.insert(temp, Item.Init(CommonItemId.Exp, cfg.exp))
    end
    --[[ local list = string.split(cfg.item_show_group, "|")
    for index, value in ipairs(list) do
        table.insert(temp, Item.Init(tonumber(value)))
    end ]]
    return temp
end
function GuildData:GetAnswerTotalPreReward()
    local reward_list = {
        [1] = {
            toggle_name = Language.Guild.ActReward,
            reward_item_list = {}
        },
        [2] = {
            toggle_name = Language.Activity.TipsNotice,
            reward_item_list = {{tips_act_type = ActType.GuildAnswer}}
        }
    }
    --正确奖励
    --参与奖励
    local right_temp = {}
    right_temp.title_name = Language.Guild.RightReward
    right_temp.item_list = self:GetAnswerRightReward()
    table.insert(reward_list[1].reward_item_list, right_temp)

    local error_temp = {}
    error_temp.title_name = Language.Guild.ErrorReward
    error_temp.item_list = self:GetAnswerErrorReward()
    table.insert(reward_list[1].reward_item_list, error_temp)
    return reward_list
end

function GuildData:GetEmbattleReward()
    local reward_auto = Config.family_honor_auto.act_reward
    local guess_reward = Config.family_honor_auto.guess_reward
    local rank_reward = Config.family_honor_auto.rank_reward
    local reward_list = {
        [1] = {
            toggle_name = Language.Guild.ActReward,
            reward_item_list = {},
            tip_str = "",
            click_func = nil,
        },
        [2] = {
            toggle_name = Language.Activity.RankReward,
            reward_item_list = {},
            tip_str = "",
            click_func = nil,
        },
        [3] = {
            toggle_name = Language.Activity.TipsNotice,
            reward_item_list = {{tips_act_type = ActType.GuildEmbattle}},
            tip_str = "",
            click_func = nil,
        }
    }
    local battle_temp = {}
    battle_temp.child_list = {}
    battle_temp.title_name = Language.GuildEmbattle.RewardTitle[1]
    local temp = {}
    for i,v in ipairs(reward_auto) do
        if v.is_show == 1 then
            temp = {}
            temp.child_title = Language.Guild.EmbattleResultReward[v.result_type]
            temp.item_list = {}
            for _,j in pairs(v.reward) do
                table.insert(temp.item_list, Item.Init(j.item_id, j.num, j.is_bind))
            end
            if v.title_item > 0 then
                table.insert(temp.item_list, Item.Init(v.title_item, 1))
            end
            if v.gongxian > 0 then
                table.insert(temp.item_list, Item.Init(CommonItemId.GongXian, v.gongxian))
            end
            if v.bind_coin > 0 then
                table.insert(temp.item_list, Item.Init(CommonItemId.Coin, v.bind_coin, 1))
            end
            if v.family_coin > 0 then
                table.insert(temp.item_list, Item.Init(CommonItemId.GuildMoney, v.family_coin))
            end
            table.insert(battle_temp.child_list, temp)
        end
    end
    table.insert(reward_list[1].reward_item_list, battle_temp)
    local guess_temp = {}
    guess_temp.child_list = {}
    guess_temp.title_name = Language.GuildEmbattle.RewardTitle[2]
    temp = {}
    temp.child_title = Language.GuildEmbattle.GuessType[1]
    temp.item_list = {}
    if guess_reward[1].right_coin > 0 then
        table.insert(temp.item_list, Item.Init(CommonItemId.Coin, guess_reward[1].right_coin, 1))
    end
    if guess_reward[1].right_family_coin > 0 then
        table.insert(temp.item_list, Item.Init(CommonItemId.GongXian, guess_reward[1].right_family_coin, 1))
    end
    table.insert(guess_temp.child_list, temp)
    temp = {}
    temp.child_title = Language.GuildEmbattle.GuessType[2]
    temp.item_list = {}
    if guess_reward[1].wrong_coin > 0 then
        table.insert(temp.item_list, Item.Init(CommonItemId.Coin, guess_reward[1].wrong_coin, 1))
    end
    if guess_reward[1].wrong_family_coin > 0 then
        table.insert(temp.item_list, Item.Init(CommonItemId.GongXian, guess_reward[1].wrong_family_coin, 1))
    end
    table.insert(guess_temp.child_list, temp)
    table.insert(reward_list[1].reward_item_list, guess_temp)

    guess_temp = {}
    guess_temp.child_list = {}
    guess_temp.title_name = Language.GuildEmbattle.RewardTitle[3]
    temp = {}
    temp.child_title = Language.GuildEmbattle.GuessType[1]
    temp.item_list = {}
    if guess_reward[2].right_coin > 0 then
        table.insert(temp.item_list, Item.Init(CommonItemId.Coin, guess_reward[2].right_coin, 1))
    end
    if guess_reward[2].right_family_coin > 0 then
        table.insert(temp.item_list, Item.Init(CommonItemId.GongXian, guess_reward[2].right_family_coin, 1))
    end
    table.insert(guess_temp.child_list, temp)
    temp = {}
    temp.child_title = Language.GuildEmbattle.GuessType[2]
    temp.item_list = {}
    if guess_reward[2].wrong_coin > 0 then
        table.insert(temp.item_list, Item.Init(CommonItemId.Coin, guess_reward[2].wrong_coin, 1))
    end
    if guess_reward[2].wrong_family_coin > 0 then
        table.insert(temp.item_list, Item.Init(CommonItemId.GongXian, guess_reward[2].wrong_family_coin, 1))
    end
    table.insert(guess_temp.child_list, temp)
    table.insert(reward_list[1].reward_item_list, guess_temp)
    
    for i,v in ipairs(rank_reward) do
        temp = {}
        temp.title_name = string.format(Language.GuildEmbattle.RankNum, v.seq)
        if i < 4 then
            temp.title_name = temp.title_name .. Language.GuildEmbattle.RongYaoReward[i]
        end
        temp.title_type = 2
        temp.item_list = {}
        for _,j in pairs(v.reward) do
            table.insert(temp.item_list, Item.Init(j.item_id, j.num, j.is_bind))
        end
        if v.title_item > 0 then
            table.insert(temp.item_list, Item.Init(v.title_item, 1))
        end
        --[[ if v.gongxian > 0 then
            table.insert(temp.item_list, Item.Init(CommonItemId.GongXian, v.gongxian))
        end ]]
        if v.winner_coin > 0 then
            table.insert(temp.item_list, Item.Init(CommonItemId.Coin, v.winner_coin, 1))
        end
        --[[ if v.family_coin > 0 then
            table.insert(temp.item_list, Item.Init(CommonItemId.GuildMoney, v.family_coin))
        end ]]
        table.insert(reward_list[2].reward_item_list, temp)
    end
    return reward_list
end
function GuildData:GetGuildBuff(level)
    if level ~= nil then
        return self.honour_reward[level]
    end
    if self.guild_top_info then
        for i = 1, 3 do
            if self:GetGuildID() ~= 0 and self.guild_top_info[i] and self:GetGuildID() == self.guild_top_info[i].guild_id then
                self.guild_info.guild_buff_rank = i
            end
        end
    end
    if self.guild_info.guild_buff_rank and self.guild_info.guild_buff_rank > 0
    and  self.guild_info.guild_buff_rank < 4 then
        return self.honour_reward[self.guild_info.guild_buff_rank]
    end
end
function GuildData:GetRongYaoDesc(level)
    local attr_type
    local value
    local res = {}
    local cfg = self:GetGuildBuff(level)
    if cfg then
        for i = 1, 2 do
            local basic = BattleData.BuffConfig(cfg["buff_reward"..i])
            if basic and basic.buff_type == 3 then
                local cfg = BattleData.BuffLevelConfig(basic.buff_id, 1)
                local params = string.split(cfg.param_list, ",")
                local p2 = tonumber(params[2])
                attr_type = Language.Common.AttrList[100 + p2]
                value = DataHelper.IsPercent(100 + p2) and (tonumber(params[1]) / 10) .. "%" or tonumber(params[1])
                table.insert( res, {attr_type,value})
            elseif basic and basic.buff_type == 29 then
                local cfg = BattleData.BuffLevelConfig(basic.buff_id, 1)
                local params = string.split(cfg.param_list, ",")
                local p2 = tonumber(params[2])
                attr_type = Language.Common.AttrList[p2]
                value = DataHelper.IsPercent(p2) and (tonumber(params[3]) / 10) .. "%" or tonumber(params[3])
                table.insert( res, {attr_type,value})
            end
        end
    end
    return res
end
function GuildData:SetTopInfo(info)
    self.guild_top_info:Set(info)
    self:GetGuildBuff()
end
function GuildData:GetTopInfo(guild_name)
    local temp
    for i = 1, 3 do
        temp = self.guild_top_info[i]
        if temp and temp.guild_name == guild_name then
            return i
        end
    end
    return 0
end

function GuildData:SetZoneBattleResult(id,value)
    self.zone_battle_result[id] = value
end

function GuildData:GetZoneBattleResult(id)
    return self.zone_battle_result[id] or 0
end
function GuildData:GetShopRedPointNum()
    if self:GetGuildID() == 0 then
        -- LogDG("没家族")
        return 0
    end
    --第一个商品数量为0或者
    local count = self:GetShopBuyCount(1)
    local info = self:GetShopInfo()[1]
    -- LogDG(info.buy_num, count)
    if info.buy_num - count <= 0 then
        self.guild_shop_first_item.count = 0
    end
    if self.guild_shop_first_item.count > 0 then
        local num = UnityPlayerPrefs.GetInt(PrefKeys.GuildShopItem1Click2())
        -- LogDG(num, TimeManager.DayStart())
        if (num - TimeManager.DayStart()) == 2 then
            self.guild_shop_first_item.count = 0
        end
    end
    if info.gongxian_cost > RoleData.Instance:GetCurrency(CurrencyType.GuildCotribution) then
        self.guild_shop_first_item.count = 0
    end
    return self.guild_shop_first_item.count
end

function GuildData:SetGuildQuitInfo(protocol) 
     self.quit_count = protocol.quit_count
     self.next_join_text = protocol.next_join_text
     GuildRedbagData.Instance:ClearHongBaoGetList()
end

function GuildData:SetGuildActiveInfo(info)
    self.guild_active_info:Set(info)
	self.guild_active_info_flush.flag = not self.guild_active_info_flush.flag
end

function GuildData:SetGuildActiveTotalRank(rank_list)
    self.guild_active_total_rank:Set(rank_list)
end

function GuildData:SetGuildActivePersonRank(rank_list)
    self.guild_active_person_rank:Set(rank_list)
end

function GuildData:SetGuildActiveBoxFlag(flag)
    self.guild_active_box_flag.flag = flag
	self.guild_trial_point.active_count = 0
	for k, v in pairs(Config.family_auto.active_gift) do
		if self:GetGuildActiveRewardIsCanGetBySeq(v.seq) then
			self.guild_trial_point.active_count = 1
			break
		end
	end
end

function GuildData:GetGuildActiveBoxList()
	local list = {}
	local box_flag = bit:d2b(self.guild_active_box_flag.flag)  
	for k, v in pairs(Config.family_auto.active_gift) do
		if v.level_min == 1 then
			local ind = #box_flag - v.seq
			v.flag = box_flag[ind]
			table.insert(list,v)	
		end
	end
    return list
end

function GuildData:GetGuildAllActiveNum()
	local min = 0
	local max = 0
	for k, v in pairs(Config.family_auto.active_gift) do
		if v.seq == 0 then
			min = v.active_limit
		end
		if v.seq == #Config.family_auto.active_gift - 1 then
			max = v.active_limit
		end
	end
    return max - min
end

function GuildData:GetGuildActiveLimitBySeq(seq)
	local active_limit = 0
	for k, v in pairs(Config.family_auto.active_gift) do
		if v.seq == seq then
			active_limit = v.active_limit
			break
		end
	end
    return active_limit
end

function GuildData:GetGuildActiveRewardBySeq(seq)
	local reward = {}
	local exp = {}
	local level = RoleData.Instance:GetRoleLevel()
	for k, v in pairs(Config.family_auto.active_gift) do
		if v.seq == seq then
			if level == v.level_min or level == v.level_max or (v.level_min < level and level < v.level_max) then
				reward = v.reward
				exp = {item_id = 65535,num = v.exp,is_bind = 0}
				break
			end
		end
	end
	local f_reward = {}
	table.insert(f_reward,exp)
	for k, v in pairs(reward) do
		table.insert(f_reward,v)
	end
    return f_reward
end

function GuildData:GetGuildActiveRewardIsGetBySeq(seq)
	local is_get = false
	local box_flag = bit:d2b(self.guild_active_box_flag.flag)  
	local ind = #box_flag - seq
	if box_flag[ind] == 1 then
		is_get = true
	end
	return is_get
end

function GuildData:GetGuildActiveRewardIsCanGetBySeq(seq)
	local is_can_get = false
	local box_flag = bit:d2b(self.guild_active_box_flag.flag)  
	local ind = #box_flag - seq
	if box_flag[ind] == 0 then
		local active_limit = 0
		for k, v in pairs(Config.family_auto.active_gift) do
			if v.seq == seq then
				active_limit = v.active_limit
				break
			end
		end
		if self.guild_active_info.day_active_val and self.guild_active_info.day_active_val >= active_limit then
			is_can_get = true
		end
	end
	return is_can_get
end

function GuildData:GetGuildActiveRankRewardByIndex(index)
	local reward = {}
	local gongxian = {}
	for k, v in pairs(Config.family_auto.family_active_rank) do
		if v.rank_min == index or index == v.rank_max or (v.rank_min <index and index < v.rank_max ) then
			reward = v.reward
			gongxian = {item_id = 65536,num = v.gongxian,is_bind = 0}
			break
		end
	end

	local f_reward = {}
	if table.nums(gongxian) > 0 then
		table.insert(f_reward,gongxian)
	end
	if table.nums(reward) > 0 then
		for k, v in pairs(reward) do
			table.insert(f_reward,v)
		end	
	end	
    return f_reward
end

function GuildData:GetGuildPersonRankRewardByIndex(index)
	local reward = {}
	local gongxian = {}

	for k, v in pairs(Config.family_auto.personal_active_rank) do
		if v.rank_min == index or index == v.rank_max or (v.rank_min <index and index < v.rank_max ) then
			reward = v.reward_show
			gongxian = {item_id = 65536,num = v.gongxian,is_bind = 0}
			break
		end
	end
	
	local f_reward = {}
	if table.nums(gongxian) > 0 then
		table.insert(f_reward,gongxian)
	end
	if table.nums(reward) > 0 then
		for k, v in pairs(reward) do
			table.insert(f_reward,v)
		end	
	end
    return f_reward
end

---------------家族灵脉------------
function GuildData:SmartInbornInfo()
    return self.guild_inborn_info
end
function GuildData:InbornSumSkillPoints()
    return self:GetLevelInfo(self.guild_inborn_info.max_guild_level).skill_points
end
function GuildData:InbornRemainSkillPoints()
    return self:InbornSumSkillPoints() - self.guild_inborn_info.use_skill_points
end

function GuildData:SetGuildInbornAllInfo(protocol)
    self.guild_inborn_info.max_guild_level = protocol.max_guild_level
    self.guild_inborn_info.use_skill_points = protocol.use_skill_points
    local learn_list = {}
    for i, v in pairs(protocol.learn_list) do
        if learn_list[v.guild_inborn_type] == nil then
            learn_list[v.guild_inborn_type] = {}
        end
        learn_list[v.guild_inborn_type][v.guild_type_seq] = v
    end
    self.guild_inborn_info.learn_list = learn_list
    self:CheckInbornRemind()
end

function GuildData:SetGuildInbornSingleInfo(protocol)
    self.guild_inborn_info.use_skill_points = protocol.use_skill_points
    local learn_list = self.guild_inborn_info.learn_list or {}
    local learn_info = protocol.learn_info
    if learn_list[learn_info.guild_inborn_type] == nil then
        learn_list[learn_info.guild_inborn_type] = {}
    end
    learn_list[learn_info.guild_inborn_type][learn_info.guild_type_seq] = learn_info
    self.guild_inborn_info.learn_list = learn_list
    self:CheckInbornRemind()
end

function GuildData:SetGuildInbornSingleTypeInfo(protocol)
    -- self.guild_inborn_info.use_skill_points = protocol.use_skill_points
    local learn_list = self.guild_inborn_info.learn_list
    if learn_list ~= nil and learn_list[protocol.guild_inborn_type] ~= nil then
        for i, v in pairs(protocol.learn_level_list) do
            if v == 0 then
                learn_list[protocol.guild_inborn_type][i] = nil
            end
        end
    end
    self.guild_inborn_info:SetDirty("learn_list")
    self:CheckInbornRemind()
end

function GuildData:SetGuildInbornOtherInfo(protocol)
    self.guild_inborn_info.max_guild_level = protocol.max_guild_level
    self.guild_inborn_info.use_skill_points = protocol.use_skill_points
    self:CheckInbornRemind()
end

--灵脉单个标签页红点数量
function GuildData:InbornPageRedNum(inborn_type)
    if not self:IsInbornRemind() then
        return 0
    end
    for i, v in pairs(self:GetInbornShowList(inborn_type) or {}) do
        if self:InbornSkillRedNum(v.inborn_type, v.type_seq) > 0 then
            return 1
        end
    end
    return 0
end

--灵脉单个技能红点数量
function GuildData:InbornSkillRedNum(inborn_type, type_seq)
    if not self:IsInbornRemind() then
        return 0
    end
    local skill_level = self:GetInbornSkillLevel(inborn_type, type_seq)
    local next_cfg = self:GetInbornSkillCfg(inborn_type, type_seq, skill_level + 1)
    if next_cfg == nil then
        return 0
    end
    --技能未激活 且 前置未解锁
    if skill_level == 0 and next_cfg.prepose_seq ~= -1 and not self:IsInbornSkillActive(next_cfg.inborn_type, next_cfg.prepose_seq) then
        return 0
    end
    --判断玩家等级,家族等级,消耗材料
    if next_cfg.level_limit > RoleData.Instance:GetRoleLevel() then
        return 0
    end
    if next_cfg.family_level_limit > GuildData.Instance.guild_info.level then
        return 0
    end
    for i, v in pairs(next_cfg.inborn_consume_item) do
        if v.num > Item.GetNum(v.item_id) then
            return 0
        end
    end
    return 1
end

function GuildData:CheckInbornRemind()
    local function func_check()
        if self:GetGuildID() == 0 or not self:IsInbornRemind() then
            return 0
        end
        -- self:InbornRemainSkillPoints() < 1
        for inborn_type, type_list in pairs(self.family_inborn_auto) do
            if self:InbornPageRedNum(inborn_type) > 0 then
                return 1
            end
        end
        return 0
    end
    self.guild_inborn_point.count = func_check()
end

function GuildData:GetInbornShowList(inborn_type)
    local show_list = {}
    for type_seq, level_list in pairs(self.family_inborn_auto[inborn_type] or {}) do
        table.insert(show_list, level_list[1])
    end
    table.sort(show_list, DataSorter.KeyLowerSorter("skill_reorder"))
    return show_list
end

--技能是否激活
function GuildData:IsInbornSkillActive(inborn_type, type_seq)
    return self:GetInbornSkillLevel(inborn_type, type_seq) > 0
end

function GuildData:GetInbornPageList()
    local show_list = {}
    for i = 1, 6 do
        local t = {}
        t.name_t = Language.Guild.GuildInbornName[i]
        t.inborn_type = i - 1
        table.insert(show_list, t)
    end
    return show_list
end

--重置可获得技能点，(消耗贡献不叠加)
function GuildData:GetInbornResetPoints(inborn_type, type_seq)
    local item_list = {}
    local func_push = function(new_items)
        for i, v in pairs(item_list) do
            if v.item_id == new_items[0].item_id then
                v.num = v.num + new_items[0].num
                return
            end
        end
        table.insert(item_list, {item_id = new_items[0].item_id, num = new_items[0].num, is_bind = new_items[0].is_bind})
    end

    local function func_reset(_inborn_type, _type_seq)
        local skill_level = self:GetInbornSkillLevel(_inborn_type, _type_seq)
        if skill_level > 0 then
            for i, v in pairs(self.family_inborn_auto[inborn_type][type_seq]) do
                if v.skill_level <= skill_level then
                    func_push(v.inborn_consume_item)
                end
            end
            --去掉以它为前置的灵脉技能
            for i, v in pairs(self:GetInbornShowList(_inborn_type)) do
                if v.prepose_seq == _type_seq then
                    func_reset(_inborn_type, v.type_seq)                    
                end
            end
        end
    end
    --[[     if self:IsInbornSkillActive(inborn_type, type_seq) then
        local points = self.family_inborn_auto[inborn_type][type_seq].consume_skill_points
        for i, v in pairs(self.family_inborn_auto[inborn_type]) do
            if v.prepose_seq == type_seq then
                points = points + self:GetInbornResetPoints(inborn_type, v.type_seq)
            end
        end
        return points
    end ]]
    func_reset(inborn_type, type_seq)
    return item_list
end

function GuildData:GetInbornSkillCfg(inborn_type, type_seq, skill_level)
    local temp_list = self.family_inborn_auto[inborn_type]
    if temp_list ~= nil then
        temp_list = temp_list[type_seq]
        if temp_list ~= nil then
            return temp_list[skill_level]
        end
    end
end

function GuildData:GetInbornSkillLevel(inborn_type, type_seq)
    local learn_info = self.guild_inborn_info.learn_list[inborn_type]
    if learn_info ~= nil and learn_info[type_seq] ~= nil and learn_info[type_seq].skill_level >= 1 then
        return learn_info[type_seq].skill_level
    end
    return 0
end

function GuildData:GetInbornSkillLevelListByType(inborn_type, type_seq)
    local temp_list = self.family_inborn_auto[inborn_type]
    if temp_list ~= nil then
        return temp_list[type_seq]
    end
end

--灵脉是否红点提示
function GuildData:IsInbornRemind()
    return UnityPlayerPrefs.GetInt(PrefKeys.GuildSkillIsRemind()) ~= 1
end
function GuildData:SetIsInbornRemind(is_remind)
    UnityPlayerPrefs.SetInt(PrefKeys.GuildSkillIsRemind(), is_remind == true and 0 or 1)
    self:CheckInbornRemind()
end

function GuildData:GetSevenActiveNum()
	local num = 0
	local data = self:GetGuildList()
	for k, v in pairs(data) do
		if v.seven_active >= num then
			num = v.seven_active
		end
	end
	return num
end

function GuildData:EnterTrialFight()
	local info = self:GuildBossInfo()
	local param_t = {battle_mode = BATTLE_MODE.BATTLE_MODE_GUILD_BOSS}
	local config = self:GuildBossConfig(info.guild_boss_chapter_id, 1)
	if config ~= nil then self:SetFamilyBossMapId(config.map_id_cfg) end 
	BattleCtrl.BattleModeFightReq(param_t)
	ViewMgr:CloseView(GuildTrialNew)
	ViewMgr:CloseView(GuildView)
end

function GuildData:SetInbornSelectSkill(inborn_type, type_seq)
    self.guild_other_info.inborn_type = inborn_type
    self.guild_other_info.type_seq = type_seq
end

function GuildData:GetInbornSkillGainTarget()
    if self.guild_other_info.inborn_type ~= nil and self.guild_other_info.type_seq ~= nil then
        local skill_cfg = self:GetInbornSkillCfg(self.guild_other_info.inborn_type, self.guild_other_info.type_seq, 1)
        return skill_cfg.gain_target or 0
    end
    return 0
end

--target_type 1伙伴，2宠物 --需要对不同的类型加延迟飘评分和属性显示
function GuildData:GetDelayTime(target_type)
    local gain_target = self:GetInbornSkillGainTarget()
    if gain_target >= 5 then
        if gain_target == 5 then
            return 1.5
        elseif gain_target == 6 then
            if target_type == 2 then return 1.5 end
        elseif gain_target == 7 then
            if target_type == 1 then return 1.5 end
            if target_type == 2 then return 3 end
        end
    end
    return 0
end

function GuildData:SetRecommendInfo(protocol)
    self.guild_recommend_info = {guild_id = protocol.guild_id, guild_name = protocol.guild_name, role_name = protocol.role_name}
end
function GuildData:GetRecommendInfo()
    return self.guild_recommend_info
end

function GuildData:IsGuildLetterFlag()
    return self.guild_letter_flag or false
end
function GuildData:SetGuildLetterFlag()
    self.guild_letter_flag = true
end
function GuildData:GuildLetterFreeTime()
    return 30
end

function GuildData:GetGuildTrialNewBossConfig(chapter_id,boss_seq)
	local config = Config.family_auto.family_boss
	local boss_config = {}
	for k, v in pairs(config) do
		if v.chapter_id == chapter_id then
			boss_config = v
		end
	end
	return boss_config
end

function GuildData:GetGuildTrialNewBossTotalHp(boss_id)
	local total_hp = 0
	local cfg = Cfg.MonsterGroupById(boss_id)
	if cfg then
		local hp_1,hp_2,hp_3,hp_4,hp_5,hp_6,hp_7,hp_8,hp_9,hp_10
		if cfg.monster_id_0 > 0 then
			hp_1 = Cfg.MonsterById(cfg.monster_id_0)
		end
		if cfg.monster_id_1 > 0 then
			hp_2 = Cfg.MonsterById(cfg.monster_id_1)
		end
		if cfg.monster_id_2 > 0 then
			hp_3 = Cfg.MonsterById(cfg.monster_id_2)
		end
		if cfg.monster_id_3 > 0 then
			hp_4 = Cfg.MonsterById(cfg.monster_id_3)
		end
		if cfg.monster_id_4 > 0 then
			hp_5 = Cfg.MonsterById(cfg.monster_id_4)
		end
		if cfg.monster_id_5 > 0 then
			hp_6 = Cfg.MonsterById(cfg.monster_id_5)
		end
		if cfg.monster_id_6 > 0 then
			hp_7 = Cfg.MonsterById(cfg.monster_id_6)
		end
		if cfg.monster_id_7 > 0 then
			hp_8 = Cfg.MonsterById(cfg.monster_id_7)
		end
		if cfg.monster_id_8 > 0 then
			hp_9 = Cfg.MonsterById(cfg.monster_id_8)
		end
		if cfg.monster_id_9 > 0 then
			hp_10 = Cfg.MonsterById(cfg.monster_id_9)
		end

		if hp_1 then
			total_hp = total_hp + hp_1.maxhp
		end
		if hp_2 then
			total_hp = total_hp + hp_2.maxhp
		end
		if hp_3 then
			total_hp = total_hp + hp_3.maxhp
		end
		if hp_4 then
			total_hp = total_hp + hp_4.maxhp
		end
		if hp_5 then
			total_hp = total_hp + hp_5.maxhp
		end
		if hp_6 then
			total_hp = total_hp + hp_6.maxhp
		end
		if hp_7 then
			total_hp = total_hp + hp_7.maxhp
		end
		if hp_8 then
			total_hp = total_hp + hp_8.maxhp
		end
		if hp_9 then
			total_hp = total_hp + hp_9.maxhp
		end	
		if hp_10 then
			total_hp = total_hp + hp_10.maxhp
		end
	end
	return total_hp
end

function GuildData:GetGuildTrialNewKillReward(chapter_id,boss_seq)
	local config = Config.family_auto.kill_reward
	local reward_config = {}
	for k, v in pairs(config) do
		if v.chapter_id == chapter_id and v.boss_seq == boss_seq then
			reward_config = v.reward_show
		end
	end
	return reward_config
end

function GuildData:GetGuildTrialNewJoinReward(chapter_id,boss_seq)
	local config = Config.family_auto.family_boss
	local reward_config = {}
	for k, v in pairs(config) do
		if v.chapter_id == chapter_id then
			local reward_1 = {item_id = 65532 ,num = v.join_coin,is_bind=1}
			local reward_2 = {item_id = 65536 ,num = v.join_gongxian,is_bind=1}
			local reward_3 = {item_id = 65605 ,num = v.family_coin,is_bind=1}
			table.insert(reward_config,reward_1)
			table.insert(reward_config,reward_2)
			table.insert(reward_config,reward_3)
		end
	end
	return reward_config
end

function GuildData:CalculateGuildTrialDamage(popup_datas)
	for _, v in pairs(popup_datas) do
		if BattleData.IsEnemyIdx(v.behiter) then
			if v.num and v.num < 0 and v.type ~= BATTLE_HP_DESC.MP_DECREASE then
				self.guild_trial_total_damage  = self.guild_trial_total_damage - v.num
			end
		end
    end
	self.damage_data.flag = not self.damage_data.flag
end

function GuildData:GetTrialDamage()
	return self.guild_trial_total_damage
end