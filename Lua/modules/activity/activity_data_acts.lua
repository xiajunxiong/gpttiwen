
function ActivityData:ActivityDataActs()
    self.acts_npc_seqs = {}
    self:ActsWish()
    self:InitActiveSystemData()

    self.sign_up_data = SmartData.New({list = {}})
    self.enter_data = SmartData.New({info = {}, flush = false})
    self.acts_tips_data = SmartData.New({val = false})

    self.time_limit_reward_list_index = SmartData.New{index = 1}
	self.time_limit_reward_point = SmartData.New{val = false}
	self.time_limit_reward_btn_effect = SmartData.New()
end

-- 许愿池
function ActivityData:ActsWish()
    self.acts_config_wish = Config.trevi_fountain_auto
    self.acts_config_wish_wish_reward = self.acts_config_wish.wish_reward
    self.acts_config_wish_reward_find = self.acts_config_wish.reward_find
    self.acts_config_wish_reward_group = self.acts_config_wish.reward_group
    self.acts_config_wish_other = self.acts_config_wish.other[1]
    self.act_wish_max_num = self.acts_config_wish_other.wish_num
    self.acts_npc_seqs.trevi_fountain = self.acts_config_wish_other.npc_seq

    self.act_wish_info = {}
    self.trevi_fountain_list = {}
    self.wst_config = Config.wst_question_cfg_auto
    self.wst_reward_find = self.wst_config.reward_find
    self.wst_reward_group = self.wst_config.reward_group
end

function ActivityData:SetTreviFountainInfo(protocol)
    self.act_wish_info = protocol.info
    self.trevi_fountain_list = protocol.trevi_fountain_list
    ActivityData.Instance:SetActivityFinishInfo(ActType.TreviFountain, self.act_wish_info.use_count, self.act_wish_max_num)
end

function ActivityData:GetTreviFountainInfo(end_time)
    return self.trevi_fountain_list[end_time] or {}
end

function ActivityData:GetActivityBackRewardListWish()
    return RewardData.Instance:GetActivityBackRewardList(self.acts_config_wish_reward_find, self.acts_config_wish_reward_group)
end

function ActivityData:GetWSTRewardList()
    return RewardData.Instance:GetActivityBackRewardList(self.wst_reward_find,self.wst_reward_group)
end

function ActivityData:GetSilkRoadBackRewardList()
    return RewardData.Instance:GetActivityBackRewardList(Config.transaction_task_auto.reward_find,Config.transaction_task_auto.reward_find_group)
end
function ActivityData:GetGuildAnswerBackRewardList()
    return RewardData.Instance:GetActivityBackRewardList(Config.family_question_cfg_auto.reward_find, Config.family_question_cfg_auto.reward_find_group)
end

function ActivityData:GetGuildMeleeBackRewardList()
    return RewardData.Instance:GetActivityBackRewardList(Config.family_war_auto.reward_find, Config.family_war_auto.reward_find_group)
end

function ActivityData:GetOnlyFightBackRewardList()
    return RewardData.Instance:GetActivityBackRewardList(Config.enemyhundred_war_auto.reward_find, Config.enemyhundred_war_auto.reward_find_group)
end

function ActivityData:GetTeamSportBackRewardList()
    return RewardData.Instance:GetActivityBackRewardList(Config.team_fight_cfg_auto.reward_find, Config.team_fight_cfg_auto.reward_find_group)
end

function ActivityData:GetFightEnergyRewardList()
    return {}
end

function ActivityData:GetWanLingActiveRewardList()
    return ActiveData.Instance:GetActiveCfgByFindType(FindType.WanLingActive)
end

function ActivityData:GetCrystalXiYouActiveRewardList()
    return ActiveData.Instance:GetActiveCfgByFindType(FindType.CrystalXiYouActive)
end

function ActivityData:GetClourArenaActiveRewardList()
    return ActiveData.Instance:GetActiveCfgByFindType(FindType.ClourArenaActive)
end

function ActivityData:GetResearchTaskActiveRewardList()
    return ActiveData.Instance:GetActiveCfgByFindType(FindType.ResearchTaskActive)
end

function ActivityData:GetBountyTaskActiveRewardList()
    return ActiveData.Instance:GetActiveCfgByFindType(FindType.BountyTaskActive)
end

-- 用于找回的，丝绸之路中间道具
function ActivityData:GetSkillRoadActFindCenterRewardList()
    local t = {}
    local t1 = {Item.Init(CommonItemId.Coin, SilkRoadData.Instance:FindBackCoin())}
    t[0] = t1
    return t
end

function ActivityData:GetDetailInfoWish()
    local role_level = RoleData.Instance:GetRoleLevel()
    local list = {}
    local co = self.acts_config_wish_wish_reward[role_level]
    if nil == co then return list end
    local reward = co.reward
    table.insert(list, Item.Init(reward.item_id, reward.num, reward.is_bind))
    table.insert(list, Item.Init(CommonItemId.Exp, co.exp or 0))
    table.insert(list, Item.Init(CommonItemId.Coin, co.coin or 0))
    return list
end

--当天是否开启
function ActivityData:IsDayOpen(open_day)
	local week = self:GetWeekNum()
	if type(open_day) == "number" then
		return week == open_day
	end
	for k,v in pairs(Split(open_day,":") or {}) do
		if tonumber(v) == week then
			return true
		end
	end
	return false
end

--获取今天的开启列表
-- function ActivityData:GetTodayListConfig()
--     local function Time(time)
--         local list = {}
--         local split_list = Split(time,",")
--         if split_list == nil then
--             local split = Split(time,":")
--             table.insert(list,{key = time,value = tonumber(split[1]) * 100 + tonumber(split[2]),scene = 1})
--         else
--             for i=1,#split_list do
--                 local split = Split(split_list[i],":")
--                 list[i] = {key = split_list[i],value = tonumber(split[1]) * 100 + tonumber(split[2]),scene = i}
--             end
--         end
--         return list
--     end
--     local list = {}
--     for k,v in pairs(Config.daily_activity_auto.daily) do
--         if v.activity_group == 2 and self:IsDayOpen(v.open_day)then
--             table.insert(list,v)
--         end
--     end
--     for k,v in pairs(list) do
--         v.o_time = Time(v.begin_time)
--         v.e_time = Time(v.end_time)
--     end
--     return list
-- end

--获取提醒列表
-- function ActivityData:GetRemindList()
--     if self.today_config == nil then
--         self.today_config = self:GetTodayListConfig()
--     end
--     local function Copy(v,list)
--         local info = TableCopy(v)
--         info.open_time = list.value
--         info.open_keys = list.key
--         info.scene = list.scene
--         return info
--     end
--     local function Time(time)
--         local split = Split(time,":")
--         return tonumber(split[1]) * 100 + tonumber(split[2])
--     end
--     local time = Time(os.date("%H:%M", TimeManager:GetServerTime()))
--     local list = {}
--     for k,v in pairs(self.today_config or {}) do
--         for i=1,#v.o_time do
--             if not self:IsNoEnoughLevel(v.act_type,true) and v.o_time[i].value >= time then
--                 table.insert(list,Copy(v,v.o_time[i]))
--             else
--                 if time <= v.e_time[i].value then
--                     if not self:IsNoEnoughLevel(v.act_type,true) and self:GetActivityStatus(v.act_type) ~= ActStatusType.Close then
--                         table.insert(list,Copy(v,v.o_time[i]))
--                     end
--                 end
--             end
--         end
--     end
--     table.sort(list,function(a,b)return a.open_time < b.open_time end)
--     return list
-- end

-- function ActivityData.GetRemindOpenTime(act_type)
--     local function Time(time)
--         local split = Split(time,":")
--         return tonumber(split[1]) * 100 + tonumber(split[2])
--     end
--     local config = ActivityData.GetConfig(act_type)
--     return Time(config.begin_time),Time(config.end_time),Time(os.date("%H:%M", TimeManager:GetServerTime()))
-- end

-----------------------活跃系统------------------------
function ActivityData:InitActiveSystemData()
    self.active_data = SmartData.New{active_num = 0,reward_flag = {},progress_list = {},flag_list = {},specia_list = {}}
    self.active_linkage_list = {}
    for k,v in pairs(Config.activedegree_auto.linkage_act) do
        if self.active_linkage_list[v.seq] == nil then
            self.active_linkage_list[v.seq] = {}
        end
        table.insert(self.active_linkage_list[v.seq],v)
    end
end

function ActivityData:SetActiveSystemInfo(protocol)
    self.active_data.active_num = protocol.active_num
    self.active_data.level = protocol.level
    self.active_data.reward_flag = bit:d2b(protocol.reward_flag)
    self.active_data.progress_list = protocol.progress_list
    self.active_data.flag_list = protocol.flag_list
    self.daily_red_point.val = self:CheckActiveRewardNum()
end

function ActivityData:SetSpecialActiveInfo(protocol)
    self.active_data.specia_list = protocol.special_active_reward_flag
    self.daily_red_point.val = self:CheckActiveRewardNum()
    self.daily_red_point.flush = not self.daily_red_point.flush
end

function ActivityData:SetActiveSystemSingleInfo(protocol)
    self.active_data.active_num = protocol.active_num
    self.active_data.level = protocol.level
    self.active_data.reward_flag = bit:d2b(protocol.reward_flag)
    local type = protocol.type or 0
    self.active_data.progress_list[type] = protocol.progress
    self.active_data.flag_list[type] = protocol.flag
    self.daily_red_point.val = self:CheckActiveRewardNum()
    self.daily_red_point.flush = not self.daily_red_point.flush
end

function ActivityData:FlushDailyRed()
    self.daily_red_point.val = self:CheckActiveRewardNum()
    self.daily_red_point.flush = not self.daily_red_point.flush
end

function ActivityData:GetTopNextUncompleteDegree()
    local active_info = nil
    local next_act_type = -1
    for k, v in pairs(self.active_data.flag_list) do
        if v == 0 then
            local config = self:GetActiveDataByType(k)
            if config then
                local not_enough = self:IsNoEnoughLevel(config.act_type,false)
                if not not_enough then
                    if active_info == nil then
                        if config.recom_order >= 0 then
                            active_info = config
                            next_act_type = config.act_type
                        end
                    elseif config.recom_order >= 0 and config.recom_order < active_info.recom_order then
                        active_info = config
                        next_act_type = config.act_type
                    end
                end
            end
        end
    end
    local next_active_info = nil
    if active_info ~= nil then
        next_active_info = self:GetActivityInfo(next_act_type)
    end
    return next_active_info
end

function ActivityData:GetProgressNum(type)
    return self.active_data.progress_list[type] or 0
end

function ActivityData:GetSpecialIsGetReward(type)
    return self.active_data.specia_list[type] == 1
end

function ActivityData:GetActiveFlag(type)
    return self.active_data.flag_list[type] or 0
end

function ActivityData:GetActiveFlagByActType(act_type)
    local config = self:GetActiveData(act_type) or {}
    return self.active_data.flag_list[config.type or 0] or 0
end

function ActivityData:GetActiveFlagList()
    return self.active_data.flag_list
end

function ActivityData:GetActiveRewardData()
    local level = self:GetActiveLevel()
    local config,list = {},{0}
    for k,v in pairs(Config.activedegree_auto.active_reward) do
		if level >= v.level_min and level <= v.level_max then
            table.insert(config,v)
		end
    end
    table.sort(config,function(a,b)return a.seq < b.seq end)
    for i=1,#config do
        list[i + 1] = config[i].integral_active
    end
    return config,list
end

function ActivityData:GetActiveIntegral(act_type)
    local config = self:GetActiveData(act_type)
    return config and config.integral_active or 0
end

function ActivityData:GetActiveDataByType(type)
    local role_level = self:GetActiveLevel()
    for k,v in pairs(Config.activedegree_auto.active) do
        if v.type == type and role_level >= v.level_min and role_level <= v.level_max then
            return v
        end
    end
end

function ActivityData:GetActiveData(act_type)
    local role_level = self:GetActiveLevel()
    for k,v in pairs(Config.activedegree_auto.active) do
        if v.act_type == act_type and role_level >= v.level_min and role_level <= v.level_max then
            return v
        end
    end
end

function ActivityData:GetActiveIntegralNum()
    return self.active_data.active_num or 0
end

function ActivityData:GetActiveRewardFlag(index)
	return self.active_data.reward_flag[33 - index] or 0
end

function ActivityData:GetActiveLevel()
    return self.active_data.level or RoleData.Instance:GetRoleLevel()
end

function ActivityData:GetActiveActRewardList(seq)
    local list = {}
    for k,v in pairs(self.active_linkage_list[seq] or {}) do
        if self:GetActivityStatus(v.act_type) ~= ActStatusType.Close then
            for i=0,#v.reward_item do
                table.insert(list,v.reward_item[i])
            end
        end
    end
    return list
end

function ActivityData:IsActiveActReward(seq)
    for k,v in pairs(self.active_linkage_list[seq] or {}) do
        if self:GetActivityStatus(v.act_type) ~= ActStatusType.Close then
            return true
        end
    end
    return false
end

function ActivityData:GetActiveRewardList(reward_item,seq)
    local reward_list = {}
    local is_show_activity = false
    for k,v in pairs(reward_item) do
        table.insert(reward_list,Item.New(v))
    end
    for k,v in pairs(self:GetActiveActRewardList(seq)) do
        table.insert(reward_list,Item.New(v))
        is_show_activity = true
    end
    return reward_list,is_show_activity
end

--获取活跃系统红点
function ActivityData:CheckActiveRewardNum()
    for k,v in pairs(self.active_data.flag_list) do
        if v == 1 then
            local _,config = DataHelper.SimpleIndex(Config.activedegree_auto.active,"type",k)
            if config and config.is_off == 0 then
                return true
            end
        end
    end
    for k,v in pairs(ActiveData.Instance:CheckDailyDataWithSp()) do 
        local reward_flag = ActivityData.Instance:GetSpecialIsGetReward(v.type)
        local act_info = ActivityData.Instance:GetActivityInfo(v.act_type)

        if not reward_flag and act_info:IsComplete() then 
            return true
        end 
    end 
    -- for k,v in pairs(self.active_data.specia_list) do 
    --     if v == 0 then 

    --     end 
    -- end 
    local num = self:GetActiveIntegralNum()
    if num ~= 0 then
        for k,v in pairs(self:GetActiveRewardData()) do
            if num >= v.integral_active and self:GetActiveRewardFlag(v.seq) == 0 then
                return true
            end
        end
    end
    
    return false
end

function ActivityData:SetActivitySignUpInfo(protocol)
    self.sign_up_data.list = bit:d2b(protocol.sign_up_flag)
    self:CheckActivityListGroup()
end

function ActivityData:GetSignUpData(seq)
    return self.sign_up_data.list[32-seq] or 0
end

function ActivityData:GetPartakeData(act_type)
    for k,v in pairs(Config.daily_activity_auto.sign_up) do
        if v.activity_id == act_type and v.open == 1 then
            return v
        end
    end
end

function ActivityData:IsPartake(data)
    local partake_config = self:GetPartakeData(data.act_type)
    local is_sign_up = partake_config and self:GetSignUpData(partake_config.seq) == 0
    return data.group_info.is_in_time_limit and not data.group_info.is_in_future and ActStatusType.Close == data.status and data.is_today == 1 and is_sign_up
end

function ActivityData:GetPartakeConfigSeq(act_type)
    local config = self:GetPartakeData(act_type)
    return config and config.seq or -1
end

function ActivityData:GetIsDayOpen(act_type)
    if self.today_config == nil then return 0 end
    for k,v in pairs(self:GetRemindList()) do
        if v.act_type == act_type then
            return 1
        end
    end
    return 0
end

function ActivityData:GetOpenRemind(act_type)
    if RushDungeonData.GetOpenRemind(act_type) then --特殊红点处理 自己的特殊处理不要整合到公共一起
        return true
    end
    if self.open_remind_list and self.open_remind_list[act_type] then
        return self.open_remind_list[act_type]
    end
end

function ActivityData:SetOpenRemind(act_type)
    if self.open_remind_list == nil then
        self.open_remind_list = {}
    end
    self.open_remind_list[act_type] = true
    RushDungeonData.SetOpenRemind(act_type)
end

--获取奖励
function ActivityData:GetActRewardData(act_type)
    local role_level = self:GetActiveLevel()
    for k,v in pairs(Config.daily_activity_auto.reward_show) do
        if v.act_type == act_type and role_level >= v.min_level and role_level <= v.max_level then
            return v
        end
    end
end

function ActivityData:SetActivityEnterInfo(protocol)
    local act_type = protocol.enter_info.act_type
    self.enter_data.info[act_type] = protocol.enter_info
    self.enter_data.flush = not self.enter_data.flush
end

function ActivityData:GetActivityEnterInfo(act_type)
    return self.enter_data.info[act_type] or {}
end

function ActivityData:IsHasSign(act_type)
    local info = self:GetActivityStatusInfo(act_type)
    if info ~= nil and info.prefs_value ~= nil then
        info.prefs_value = (info.prefs_value == "") and "0" or info.prefs_value
		if tostring(info.prefs_value) == "0" or (tonumber(info.prefs_value) < (tonumber(info.param_2) or 0)) then
			return true
		end
	end
	return false
end

function ActivityData:SetHasSign(act_type)
    local info = self:GetActivityStatusInfo(act_type)
	if info ~= nil and info.prefs_value ~= nil then
		if tostring(info.prefs_value) ~= tostring(info.param_2) then
            info.prefs_value = info.param_2
            UnityPlayerPrefs.SetString(PrefKeys.ActNew(act_type), tostring(info.param_2))
            self.acts_tips_data.val = not self.acts_tips_data.val
		end
	end
end

function ActivityData:ResetHasSign(act_type)
    local info = self:GetActivityStatusInfo(act_type)
    if info ~= nil and info.prefs_value ~= nil then
        info.prefs_value = ""
        UnityPlayerPrefs.SetString(PrefKeys.ActNew(act_type), "")
    end
end

--各种限时活动中奖励的列表
function ActivityData:SetTimeLimitRewardList(reward_list)
	self.time_limit_reward_list = self:addTips(reward_list)
end

function ActivityData:addTips(reward_list)
	local act_type = -1
	for k,v in pairs(reward_list) do
		if v.reward_item_list[1].tips_act_type ~= nil then
			return reward_list
		elseif v.reward_item_list[1].func_param and v.reward_item_list[1].func_param.activity_type and act_type == -1 then
			act_type = v.reward_item_list[1].func_param.activity_type
		elseif v.reward_item_list[1].child_list then
			for i,j in pairs(v.reward_item_list[1].child_list) do
				if j.func_param and j.func_param.activity_type then
					act_type = j.func_param.activity_type
				end
			end
		end
	end
	if act_type ~= -1 then
		table.insert(reward_list,{
            toggle_name = Language.Activity.TipsNotice,
            reward_item_list = {{tips_act_type = act_type}}
        })
	end
	return reward_list
end

function ActivityData:GetCacheTimeLimitRewardList()
	return self.time_limit_reward_list or {}
end

function ActivityData:SetTimeLimitRewardGetFlag(protocol)
	self.time_limit_act_reward_get_flag[protocol.activity_type] = {particip_award_flag = protocol.particip_award_flag, survival_award_flag = protocol.survival_award_flag, levle = protocol.level}
	self.time_limit_reward_point.val = not self.time_limit_reward_point.val
end

function ActivityData:GetTimeLimitRewardGetFlag(activity_type)
	return self.time_limit_act_reward_get_flag[activity_type]
end

function ActivityData:SetTimeLimitRewardBtnEffFlag(activity_type,flag)
	self.time_limit_reward_btn_effect[activity_type] = flag
end

function ActivityData:GetActTipsSeq(activity_type)
	return self.config.activity_show[activity_type]
end

function ActivityData:GetActActivityShow(activity_type)
    return self.activity_show_key_list[activity_type]
end

function ActivityData:GetActNeedCloseTips(activity_type)
    local cfg = self:GetActActivityShow(activity_type)
    if cfg ~= nil then 
        return cfg.is_popup == 1 and ActivityData.IsOpen(activity_type) and not DialogTipsData.Instance:TodayIsTrue(DTNewTodayNotRemind.ActCloseTips) 
    end
	return false
end

function ActivityData:RegisterRewardGetFunc(activity_type, func)
	self.time_limit_act_reward_func[activity_type] = func
end

function ActivityData:GetPreviewRewardFunc(activity_type)
	return self.time_limit_act_reward_func[activity_type]
end

function ActivityData:GetSignUpFlag(act_type)
    return self:GetSignUpData(self:GetPartakeConfigSeq(act_type))
end

--@G获取某个活动什么时候开启的时间戳
function ActivityData.GetRemindTimeStamp(act_type)
    local function Time(open_day,begin_time)
        local split = Split(begin_time,":")
        local week = ActivityData.Instance:GetWeekNum()
        local day_list = DataHelper.SplitList(open_day,":")
        for i,v in ipairs(day_list) do
            if v >= week then
                return v - week,tonumber(split[1]),tonumber(split[2])
            end
        end
        return (7 + day_list[1] - week),tonumber(split[1]),tonumber(split[2])
    end
    local config = ActivityData.GetConfig(act_type)
    local t = os.date("*t",TimeManager:GetServerTime())
    local remain_day,hour,min = Time(config.open_day,config.begin_time)
	return os.time{year=t.year,month=t.month,day=t.day+remain_day,hour=hour,min=min,sec=0},(remain_day + hour / 24)
end

--@G活动倒计时显示单独一个单位（天|时|分|秒）
function ActivityData.GetConditionTimeTip(t)
    local LimitTimeTips = Language.Activity.LimitTimeTips
	if t.day == 0 and t.hour ~= 0 then
		return Format(LimitTimeTips.Hour,t.hour)
	elseif t.day == 0 and t.hour == 0 then
		return Format(LimitTimeTips.Min,t.min)
	end
	return Format(LimitTimeTips.Day,t.day)
end

function ActivityData:IsRangeEnd(begin_type,end_type)
    local function OpenDay(open_day)
        local list = DataHelper.SplitList(open_day,":") or {}
        return list[#list] or 0
    end
    local week,time = self:GetWeekNum(),self:GetCurrTime()
    local begin,endin = ActivityData.GetConfig(begin_type),ActivityData.GetConfig(end_type)
    local begin_day,end_day = OpenDay(begin.open_day),OpenDay(endin.open_day)
    if week == begin_day and time > self:SplitTime(begin.end_time) then
        return true
    elseif week > begin_day and week < end_day then
        return true
    elseif week == end_day and time < self:SplitTime(endin.begin_time) then
        return true
    end
    return false
end

function ActivityData:IsNotRemindRange(act_type,day_num)
    local _,remain_day = ActivityData.GetRemindTimeStamp(act_type)
    return remain_day > day_num
end

--@G活动是否当天结束
function ActivityData:IsOpenEnd(act_type)
    local config = ActivityData.GetConfig(act_type)
    return self:IsDayOpen(config.open_day) and self:GetCurrTime() >= self:SplitTime(config.begin_time)
end

--@G获取当前时间 千时百分
function ActivityData:GetCurrTime()
    return self:SplitTime(os.date("%H:%M", TimeManager:GetServerTime()))
end

--@G处理配置时间
function ActivityData:SplitTime(time)
    local split = Split(time,":")
    return tonumber(split[1]) * 100 + tonumber(split[2])
end

function ActivityData:GetTimeData(time)
    local hour = math.floor(time / 100)
    return {hour = hour,min = time - hour * 100}
end