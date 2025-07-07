ActiveData = ActiveData or BaseClass()

function ActiveData:__init()
    if ActiveData.Instance ~= nil then
        Debuger.LogError("[ActiveData] attempt to create singleton twice!")
        return
    end
    ActiveData.Instance = self
    self:CheckTimeLimitData()
    self.config = Config.activedegree_auto
    self.other = Config.activedegree_auto.other[1]
    self.time_limit_data = SmartData.New({can_flag = {},get_flag = {},val = false})
    self.click_goto_limit_act = SmartData.New({list = {}, flush_flag = false})

    self:InitWeekRewardData()
    self:InitRollerData()
end
function ActiveData:Clear()
    self.click_goto_limit_act.list = {}
end

--获取活跃列表
function ActiveData:GetDailyList()
    local list = self:CheckDailyData()
    table.sort(list,DataHelper.WeightSort("sort",function(data)
        if data.act_type == ActType.Hint then
            return -2000
        end
        if ActivityData.Instance:GetActiveFlag(data.type) == ActiveConfig.FlagType.CanReward then
            return -1000
        end
        if ActivityData.Instance:GetActiveFlag(data.type) == ActiveConfig.FlagType.Rewarded then
            return 1000
        end
        if RoleData.Instance:GetRoleLevel() < ActivityData.GetConfig(data.act_type).level_restrict then
            return ActivityData.GetConfig(data.act_type).level_restrict * 10
        end
        if RoleData.Instance:GetRoleLevel() >= ActivityData.GetConfig(data.act_type).level_restrict and RoleData.Instance:CreatedDay() < data.open_day then
            return 100
        end
        local act_info = ActivityData.Instance:GetActivityInfo(data.act_type)
        if act_info and act_info:IsComplete() and data.is_special == 0 then
            return 1000
        end
        if act_info and act_info:IsComplete() and data.is_special == 1 then
            local reward_flag = ActivityData.Instance:GetSpecialIsGetReward(data.type)
            if reward_flag then 
                return 1000
            else 
                return -1000
            end 
            
        end
    end))
    return self:CheckDaliyList(list)
end

--获取活动限时列表
function ActiveData:GetTimeLimitList()
    if self.time_limit_config == nil then
        self:CheckTimeLimitData()
    end
    local list = self:FixActList(self.time_limit_config or {})

    table.sort(list,DataHelper.WeightSort("act_seq",function(data)
        local act_info = ActivityData.Instance:GetActivityInfo(data.act_type)

        if data.act_type == ActType.UTArena2 and UniverseTeamArenaData.Instance:UTArena2IsOpen() then
            return -2200
        end

        if HeroVanBattleData.Instance:HeroVanHighMark(data.act_type) then 
            return -2100
        end 
        if self:GetSignUpFlag(data.act_type) then
            return -2000
        end
        if ActivityData.IsOpen(data.act_type) and RoleLevel() >= data.level_restrict then
            return -1000
        end
        if act_info and act_info:IsToday() then
            return -100
        end
        if RoleLevel() < data.level_restrict then
            return data.level_restrict
        end
    end))
    for i,v in ipairs(list) do
        v.index = i
    end
    return list
end

function ActiveData:FixActList(list)
    local fix_list = {}
    for k,v in pairs(list) do 
        if ActiveConfig.HeroType[v.act_type] then 
            if HeroVanBattleData.Instance and HeroVanBattleData.Instance:CheckWorldLevel() then 
                table.insert( fix_list, v)
            end 
        elseif v.act_type == ActType.WorldBoss3 then 
            if RoleData.Instance:GetWorldLevel() >= Config.monster_tianshen_auto.other[1].open_level then 
                table.insert( fix_list, v)
            end 
        elseif v.act_type == ActType.UTArena2 then
            if RoleData.Instance:GetWorldLevel() >= Config.universe_arena_auto.other[1].level then
                table.insert( fix_list, v)
            end
        else 
            table.insert(fix_list,v)
        end 
    end 
    return fix_list
end

function ActiveData:CheckDailyData()
    local list = {}
    local role_level = RoleData.Instance:GetRoleLevel()
    for k,v in pairs(Config.activedegree_auto.active) do
        if ActType.TreasureTask == v.act_type then
            if FunOpen.Instance:IsFunOpen(Mod.TreasureTask.Main) then
                local config = ActivityData.GetConfig(v.act_type) or {}
                if config.is_show == 1 and role_level >= v.level_min and role_level <= v.level_max and v.is_off == 0 then
                    table.insert(list,v)
                end
            end
        -- elseif ActType.Prestige == v.act_type then
        --     if RoleData.Instance:CreatedDay() > 3 then
        --         local config = ActivityData.GetConfig(v.act_type) or {}
        --         if config.is_show == 1 and role_level >= v.level_min and role_level <= v.level_max and v.is_off == 0 then
        --             table.insert(list,v)
        --         end
        --     end
        else
            local config = ActivityData.GetConfig(v.act_type) or {}
            if config.is_show == 1 and role_level >= v.level_min and role_level <= v.level_max and v.is_off == 0 then
                table.insert(list,v)
            end
        end
    end
    return list
end

function ActiveData:CheckDailyDataWithSp()
    local list = {}
    local role_level = RoleData.Instance:GetRoleLevel()
    for k,v in pairs(Config.activedegree_auto.active) do
        if ActType.TreasureTask == v.act_type then
            if FunOpen.Instance:IsFunOpen(Mod.TreasureTask.Main) then
                local config = ActivityData.GetConfig(v.act_type) or {}
                if config.is_show == 1 and role_level >= v.level_min and role_level <= v.level_max and v.is_off == 0 and v.is_special and v.is_special == 1 then
                    table.insert(list,v)
                end
            end
        else
            local config = ActivityData.GetConfig(v.act_type) or {}
            if config.is_show == 1 and role_level >= v.level_min and role_level <= v.level_max and v.is_off == 0 and v.is_special and v.is_special == 1 then
                table.insert(list,v)
            end
        end
    end
    return list
end

function ActiveData:CheckTimeLimitData()
    self.time_limit_config = {}
    for k,v in pairs(Config.daily_activity_auto.daily) do
        if v.activity_group == ActivityConfig.ActivityGroupType.time_limit and v.is_show == 1 then
            -- 特殊·英雄会武需要在世界等级足够的情况下才可视
            -- if ActiveConfig.HeroType[v.act_type] then 
            --     LogError("!?@entyer?!")
            --    if HeroVanBattleData.Instance and HeroVanBattleData.Instance:CheckWorldLevel() then table.insert(self.time_limit_config,v) end 
            -- else
            table.insert(self.time_limit_config,v)
            -- end 
        end
    end
end

--获取活跃奖励
function ActiveData:GetItemReardData(config)
    local list = {}
    if not next(config) then return {} end
    for i,v in ipairs(ActiveConfig.RewardKey) do
        local value = config[v.key] or 0
        if value ~= 0 then
            table.insert(list,{item_id = v.item_id,num = value})
        end
    end
    if config.reward_item then
        for k,v in pairs(config.reward_item) do
            table.insert(list,v)
        end
    end
    if config.coin and config.coin ~= 0 then
        table.insert(list,{item_id = CommonItemId.Coin,num = config.coin})
    end
    return DataHelper.FormatItemList(list)
end

function ActiveData:GetItemLimitReardData(config)
    local list = {}
    if not next(config) then return {} end
    for k,v in pairs(config.reward_item) do
        table.insert(list,v)
    end
    if config.coin and config.coin ~= 0 then
        table.insert(list,{item_id = CommonItemId.Coin,num = config.coin})
    end
    for i,v in ipairs(ActiveConfig.RewardKey) do
        local value = config[v.key] or 0
        if value ~= 0 then
            table.insert(list,{item_id = v.item_id,num = value})
        end
    end
    return DataHelper.FormatItemList(list)
end

--获取活动标签列表
function ActiveData:GetLabelList(act_type)
    local list = {}
    local config = ActivityData.Instance:GetActRewardData(act_type)
    if config == nil then
        LogError(Format("活动类型 %s 的配置为空",act_type))
        return list
    end 
    for i,v in ipairs(DataHelper.SplitList(config.label_show,",")) do
        table.insert(list,Config.daily_activity_auto.activity_label_show[v])
    end
    return list
end

--获取活动描述
function ActiveData:GetDailyCondition(act_type)
    local condition = Language.Active.Condition
    local act_info = ActivityData.Instance:GetActivityInfo(act_type)
    if act_info then
        condition = act_info:GetTimesCondition()
    end
    local condition_func = ActivityData.Instance:GetConditionList(act_type)
    if condition_func then
        condition = condition_func() or ""
    end
    return condition
end

function ActiveData:GetDailyNeedCount(data)
    if data.act_type == ActType.DreamNotes then
        return DreamNotesData.Instance:GetHasMaxTimesDesc(data.need_count)
    end
    return data.need_count
end

--需求 需要过滤掉未开启的活动
function ActiveData:CheckDaliyList(data)
    local list = {}
    local limit_level = 0
    for i,v in ipairs(data) do
        if not (v.act_type == ActType.FlyUpDaily and not FlyUpData.Instance:IsCompletedTask()) then           
            local level_restrict = ActivityData.GetConfig(v.act_type).level_restrict
            if RoleData.Instance:GetRoleLevel() >= level_restrict then
                if AnecdoteHintData.IsOpened(v) then
                    table.insert(list,v)
                end
            else
                if level_restrict == limit_level or limit_level == 0 then
                    limit_level = level_restrict
                    if AnecdoteHintData.IsOpened(v) then
                        table.insert(list,v)
                    end
                end
            end
        end
    end
    for i,v in ipairs(list) do
        v.index = i
    end
    return list
end

function ActiveData:GetActiveConfig(type)
    local role_level = RoleData.Instance:GetRoleLevel()
    for k,v in pairs(Config.activedegree_auto.active) do
        if v.is_off == 0 and v.type == type and role_level >= v.level_min and role_level <= v.level_max then
            return v
        end
    end
end

function ActiveData:GetActiveCfg(act_type)
    for k,v in pairs(Config.activedegree_auto.active) do
        if v.act_type == act_type then
            return v
        end
    end
end

--参与奖励=============================================================================================
function ActiveData:SetActiveSignUpInfo(protocol)
    self.time_limit_data.can_flag = protocol.can_flag
    self.time_limit_data.get_flag = protocol.get_flag
    self.time_limit_data.val = not self.time_limit_data.val
end

--是否有参与奖励可领取
function ActiveData:GetSignUpFlag(act_type)
    return self.time_limit_data.can_flag[act_type] == 1 and self.time_limit_data.get_flag[act_type] == 0 and self:IsHasSignUpReward(act_type)
end

function ActiveData:IsHadSignUpFlag(act_type)
    return self.time_limit_data.get_flag[act_type] == 1
end

--是否存在参与奖励
function ActiveData:IsHasSignUpReward(act_type)
    local role_level = RoleData.Instance:GetRoleLevel()
    local config = DataHelper.GetSearchCond(Config.daily_activity_auto.join_reward,function(v)
        return v.activity_id == act_type and role_level >= v.min_level and role_level <= v.max_level
    end)
    return config ~= nil
end

--获取参与奖励
function ActiveData:GetSignUpRewardData(act_type)
    local role_level = RoleData.Instance:GetRoleLevel()
    local config = DataHelper.GetSearchCond(Config.daily_activity_auto.join_reward,function(v)
        return v.activity_id == act_type and role_level >= v.min_level and role_level <= v.max_level
    end)
    return config and self:GetItemLimitReardData(config) or {}
end

--限时红点
function ActiveData:GetTimeLimitNum()
    for k,flag in pairs(self.time_limit_data.can_flag) do
        if flag ~= 0 and self.time_limit_data.get_flag[k] == 0 and ActivityData.GetConfig(k) ~= nil and 
        ActivityData.GetConfig(k).is_show == 1 and self:IsHasSignUpReward(k) then
            return 1
        end
    end
    return self:ActOpenRemind()
end

--周赏=================================================================================================
function ActiveData:InitWeekRewardData()
    self.week_reward_fast_config = {}
    self.week_reward_long_config = {}
    for k,v in pairs(Config.activedegree_auto.week_activity) do
        if v.show_type ~= 2 then
            table.insert(self.week_reward_fast_config,v)
        else
            table.insert(self.week_reward_long_config,v)
        end
    end
    table.sort(self.week_reward_fast_config,DataHelper.SortFunc([[seq]]))
    table.sort(self.week_reward_long_config,DataHelper.SortFunc([[seq]]))
    self.weed_reward_data = SmartData.New({reward_flag ={},finish_flag = {},integral_num = 0,val = false})
end

function ActiveData:GetWeekRewardData()
    local config,list = {},{}
    local level = RoleLevel()
    for k,v in pairs(Config.activedegree_auto.week_activity_reward) do
        if level >= v.level_min and level <= v.level_max then
            table.insert(config, v)
        end
    end
    table.sort(config,function(a,b)return a.seq < b.seq end)
    for i=1,#config do
        list[i] = config[i].integral_active
    end
    return config, list
end

function ActiveData:SetWeekActiveDegreeInfo(protocol)
    self.weed_reward_data.reward_flag = protocol.reward_flag
    self.weed_reward_data.finish_flag = protocol.finish_flag
    self.weed_reward_data.integral_num = protocol.integral_num
    self.weed_reward_data.val = not self.weed_reward_data.val
end

function ActiveData:GetWeekRewardFlag(index)
    return self.weed_reward_data.reward_flag[index] or 0
end

function ActiveData:GetWeekRewardFinishFlag(index)
    return self.weed_reward_data.finish_flag[index] or 0
end

function ActiveData:GetWeekRewardIntegralNum()
    return self.weed_reward_data.integral_num
end

function ActiveData:GetWeekRewardFastData()
    local list = self.week_reward_fast_config or {}
    table.sort(list,DataHelper.WeightSort("seq",function(data)
        if data.show_type ~= 0 then
            local act_info = ActivityData.Instance:GetActivityInfo(data.act_type)
            if act_info and act_info:IsToday() == false then
                return 1000
            end
        end
    end))
    return list
end

function ActiveData:GetWeekRewardLongData()
    return self.week_reward_long_config
end

function ActiveData:GetWeekRewardNum()
    local num = self:GetWeekRewardIntegralNum()
    if num ~= 0 then
        for k,v in pairs(Config.activedegree_auto.week_activity_reward) do
            if num >= v.integral_active and self:GetWeekRewardFlag(v.seq) == 0 then
                return 1
            end
        end
    end
    return 0
end


-----------------活跃转盘-----------------
function ActiveData:InitRollerData()
    self.roller_reward_config = {}
    self.active_roller_data = SmartData.New({can_roller_times = 0})
    self.active_roller_task_data = SmartData.New({day_task_flag = {}, week_task_flag = {}})
    self.active_roller_reward_data = SmartData.New({seq = 0, item_list = {}})
end

function ActiveData:SetActiveRollerAllInfo(protocol)
    self.active_roller_data.can_roller_times = protocol.can_roller_times
    self.active_roller_task_data.day_task_flag = protocol.day_task_flag
    self.active_roller_task_data.week_task_flag = protocol.week_task_flag
    self.active_roller_data:Notify()
    self.active_roller_task_data:Notify()
end

function ActiveData:SetActiveRollerRewardInfo(protocol)
    self.active_roller_data.can_roller_times = protocol.can_roller_times
    self.active_roller_reward_data.seq = protocol.seq
    self.active_roller_reward_data.item_list = protocol.item_list
    self.active_roller_data:Notify()
    self.active_roller_reward_data:Notify()
end

function ActiveData:GetActiveRollerDayTaskFlag(seq)
    return self.active_roller_task_data.day_task_flag[seq] or 0
end

function ActiveData:GetActiveRollerWeekTaskFlag(seq)
    return self.active_roller_task_data.week_task_flag[seq] or 0
end

function ActiveData:GetRollerConfig()
    local group = 1
    local level = RoleLevel()
    for i, v in ipairs(Config.activedegree_auto.active_turntable) do
        if level >= v.min_level_turn and level <= v.max_level_turn then
            group = v.reward_group
            break
        end
    end
    if not self.roller_reward_config[group] then
        self.roller_reward_config[group] = {}
        for k, v in pairs(Config.activedegree_auto.turntable_reward_group) do
            if group == v.reward_group then
                self.roller_reward_config[group][v.index] = v
            end
        end
    end
    return self.roller_reward_config[group]
end

function ActiveData:GetRollerTaskConfig()
    local tab = {}
    for k, v in pairs(Config.activedegree_auto.turntable_task) do
        tab[v.index] = {}
        tab[v.index].task_info = v
        if v.task_type == 1 then
            tab[v.index].act_info = ActivityData.Instance:GetActivityInfo(v.type_param)
        end
    end
    local sort_fun = function(data)
        if data.act_info and data.act_info:IsToday() then
            return data.task_info.index - 1000
        end
        return data.task_info.index
    end
    table.sort(tab, function(a_data, b_data)
        return sort_fun(a_data) < sort_fun(b_data)
    end)
    for i, v in ipairs(tab) do
        v.sort_index = i
    end
    return tab
end

-- 活跃转盘红点
function ActiveData:GetRollerRemind()
    return self.active_roller_data.can_roller_times
end

-- 获取活跃特殊按钮名称 返回str
function ActiveData:GetActivitySpBtnName(act_type)
    local func = self.sp_btn_name_register_list ~= nil and self.sp_btn_name_register_list[act_type] or nil
    if func ~= nil then 
        return func()
    else
        return nil
    end 
end

-- 注册活跃特殊按钮名称
function ActiveData:RegisterActivitySpBtnName(act_type,func)
	if self.sp_btn_name_register_list == nil then
		self.sp_btn_name_register_list = {}
	end
	self.sp_btn_name_register_list[act_type] = func
end 

-- 获取活跃特殊置灰飘字 返回str
function ActiveData:GetActivitySpBtnUnAct(act_type)
    local func = self.sp_btn_unact_register_list ~= nil and self.sp_btn_unact_register_list[act_type] or nil
    if func ~= nil then 
        return func()
    else
        return nil
    end 
end

-- 注册活跃特殊按钮名称
function ActiveData:RegisterActivitySpBtnUnAct(act_type,func)
	if self.sp_btn_unact_register_list == nil then
		self.sp_btn_unact_register_list = {}
	end
	self.sp_btn_unact_register_list[act_type] = func
end 

--获得活跃中的可找回的奖励
function ActiveData:GetActiveCfgByFindType(find_type)--传入的find_type代表“找回枚举（FindType）”中的值
    local systemType = FindTypeToActivitySystemType[find_type]
    local t
    if systemType ~= nil then
        local role_level = RoleData.Instance:GetRoleLevel()
        for _, v in pairs(Config.activedegree_auto.active) do
            if v.type == systemType and role_level >= v.level_min and role_level <= v.level_max and v.is_off == 0 then
                if ActType.TreasureTask == v.act_type then
                    if FunOpen.Instance:IsFunOpen(Mod.TreasureTask.Main) then
                        local config = ActivityData.GetConfig(v.act_type) or {}
                        if config.is_show == 1 then
                            t = {}
                            t.exp = v.exp
                            t.max_level = v.level_max
                            t.min_level = v.level_min
                            --此find_type代表经验
                            t.find_type = 0
                            break
                        end
                    else
                        return {}
                    end
                else
                    local config = ActivityData.GetConfig(v.act_type) or {}
                    if config.is_show == 1 then
                        t = {}
                        t.exp = v.exp
                        t.max_level = v.level_max
                        t.min_level = v.level_min
                        --此find_type代表经验
                        t.find_type = 0
                        break
                    end
                end
            end
        end
    end
    if t then
        return RewardData.Instance:GetActivityBackRewardList({[0] = t},{})
    else
        return {}
    end
end

-- 是否显示右下角的限时抽金宠
function ActiveData:IsShowRoller()
    return ActivityData.IsOpen(ACTIVITY_TYPE.KF_LUCK_ROLLER) and FunOpen.Instance:IsFunOpen(Mod.ServerActivity.LuckRoller)
end

-- 是否显示右下角的一键完成
function ActiveData:IsShowAllFinish()
    -- 只有右下角没有金宠的时候才显示判断
    if not self:IsShowRoller() then
        -- 开服时间和角色等级是否足够
        if RoleLevel() >= self.other.one_click_level and TimeCtrl.Instance:GetCurOpenServerDay() >= self.other.one_click_open_time then
            return not TableIsEmpty(self:GetCanAllFinishList())
        end
    end
    return false
end

-- 获取当前可以一键完成的任务列表
function ActiveData:GetCanAllFinishList()
    local list = {}
    local list2 = self:GetDailyList()
    local open_fun = function(config, data, act_info)
        local is_enough, limit_level = ActivityData.Instance:IsNoEnoughLevel(config.act_type, true)
        local is_open_btn, open_btn_name = act_info:CustomButtonNameFunc()
        local is_open_day = RoleData.Instance:CreatedDay() >= data.open_day
        if not is_enough and open_btn_name ~= nil then
            return not is_enough and is_open_btn
        end
        return not is_enough and is_open_day
    end
    for k, v in pairs(self.config.one_click_list) do
        for l, w in pairs(list2) do
            if w.act_type == v.act_type then
                local act_info = ActivityData.Instance:GetActivityInfo(w.act_type)
                local is_open = open_fun(ActivityData.GetConfig(w.act_type), w, act_info)
                -- 声望任务特殊处理，选了村庄后就不显示了
                if is_open and v.act_type == 10039 then
                    if PrestigeData.Instance:GetSelectVillageId() ~= 0 then
                        is_open = false
                    end
                end
                -- 任务进度为0才能用
                local num = ActivityData.Instance:GetProgressNum(w.type)
                if num == 0 and is_open then
                    table.insert(list, w)
                end
                break
            end
        end
    end
    return list
end

-- 获取一键完成的任务消耗元宝
function ActiveData:GetAllFinishCostGold(act_type)
    for k, v in pairs(self.config.one_click_list) do
        if v.act_type == act_type then
            return v.cost_gold
        end
    end
    return 0
end

-- 获取一键完成的任务seq
function ActiveData:GetAllFinishSeq(act_type)
    for k, v in pairs(self.config.one_click_list) do
        if v.act_type == act_type then
            return v.seq
        end
    end
end

--获取限时活动列表判断这些活动是否在开启中，且没有点击过前往
function ActiveData:ActOpenRemind()
    local list = self:GetTimeLimitList()
    local is_open = false
    for k, v in pairs(list) do
        if self:CheckLimitActIsOpen(v) == 1 then
            return 1
        end
    end
    return 0
end
function ActiveData:CheckLimitActIsOpen(act_cfg)
    if self.click_goto_limit_act.list[act_cfg.act_type] == 1 then
        return 0
    end
    local is_open = false
    local role_level = RoleData.Instance:GetRoleLevel()
    is_open = role_level >= act_cfg.level_restrict
    local status = ActivityData.Instance:GetActivityStatus(act_cfg.act_type)
    if not is_open then
        return 0
    end
    if status == ActStatusType.Open then
        return 1
    end
    return 0
end
function ActiveData:SetLimitActClickGoto(act_type)
    self.click_goto_limit_act.list[act_type] = 1
    self.click_goto_limit_act.flush_flag = not self.click_goto_limit_act.flush_flag
end
function ActiveData:ClearLimitActClickGoto(act_type)
    self.click_goto_limit_act.list[act_type] = 0
end