RewardData = RewardData or BaseClass()

function RewardData:__init()
    if RewardData.Instance ~= nil then
        Debuger.LogError("[RewardData] attempt to create singleton twice!")
        return
    end
    RewardData.Instance = self

    self:RewardConfig()
    self:RewardData()
    self.activity_back_reward_groups = {}
    self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,
    BindTool.Bind(self.RoleNoticeComplete, self))
    self.level_change_handle = EventSys:Bind(GlobalEventKeys.RoleLevelChange,
    BindTool.Bind(self.RoleLevelChange, self))
	self.minute_change_handle = EventSys:Bind(GlobalEventKeys.TimeMinuteChange,
	BindTool.Bind(self.ChangeRedPointValue, self))

    self.reward_list_data = SmartData.New({val = false})
    self.open_reward_data = SmartData.New({val = false})

    -- 主界面是否显示找回按钮
    self.main_find_flag = true
end

function RewardData:__delete()
	EventSys:UnBind(self.complete_handle)
	EventSys:UnBind(self.level_change_handle)
	EventSys:UnBind(self.minute_change_handle)
end

function RewardData:RoleNoticeComplete()
    self:CheckLevelReward()
end

function RewardData:RoleLevelChange()
    self:CheckLevelReward()
    self:CheckImmortalFund()
end

function RewardData:ClearData()
    self:ClearUpgrade()
    self:ClearActivityBack()
    self:ClearPointData()
    self:ClearRebateData()
    self:ClearMainFindFlag()
end

function RewardData:RewardData()
    self:RewardShowList()
    self:RewardDataReward()
end

-- 主界面找回按钮标志重置
function RewardData:ClearMainFindFlag()
    self:MainFindFlag(true)
end

-- 主界面找回按钮标志
function RewardData:MainFindFlag(value)
    if value ~= nil then
        self.main_find_flag = value
    else
        return self.main_find_flag
    end
end

-- 让主界面的找回按钮本次登录都不显示了
function RewardData:SetMainFindNotShow()
    if self:MainFindFlag() then
        self:MainFindFlag(false)
        self.activity_back_red_point:Notify()
    end
end

function RewardData:RewardShowList()
    self.reward_show_list = {}
    for _, info in pairs(self.reward_config_reward_list) do
        info.mod = TabMod(info.open)
        if info.is_show == 1 then
            table.insert(self.reward_show_list, info)
        end
    end
    for _, info in pairs(Config.activity_main_auto.reward_list) do
        info.mod = TabMod(info.open)
        if info.is_show == 1 then
            table.insert(self.reward_show_list, info)
        end
    end
end

function RewardData:GetActivityBackRewardList(reward_find, reward_group)
    local role_level = RoleData.Instance:GetRoleLevel()
    self:CheckActivityBackRewardGroup(reward_group)
    local t = {}
    for k, v in pairs(reward_find) do
        if v.min_level <= role_level and v.max_level >= role_level then
            t[v.find_type] = t[v.find_type] or {}
            RewardData.CheckReward(t[v.find_type], {item_id = CommonItemId.Exp, num = v.exp})
            RewardData.CheckReward(t[v.find_type], {item_id = CommonItemId.Coin, num = v.coin})
            RewardData.CheckReward(t[v.find_type], {item_id = CommonItemId.GongXian, num = v.family_gongxian})
            if v.reward_group and v.reward_group > -1 then
                local groups = self.activity_back_reward_groups[reward_group][v.reward_group]
                if groups then
                    for y, z in pairs(groups) do
                        RewardData.CheckReward(t[v.find_type], z.reward)
                    end
                end
            end
            if v.group_id and v.group_id > -1 then --失误 宝图
                for y, z in pairs(self.activity_back_reward_groups[reward_group][v.group_id]) do
                    RewardData.CheckReward(t[v.find_type], z.reward)
                end
            end
            if v.reward_show then --门派任务
                RewardData.CheckRewardSplit(t[v.find_type], v.reward_show)
            end
        end
    end
    return t
end

function RewardData.CheckReward(t, item_data)
    if item_data.num and item_data.num > 0 then
        local item = Item.New()
        item.item_id = item_data.item_id
        item.num = item_data.num
        item.is_bind = item_data.is_bind or 0
        table.insert(t, item)
    end
end

function RewardData.CheckRewardSplit(t, group_str)
    for _, item_str in ipairs(Split(group_str, "|")) do
        local item_data = Split(item_str, ":")
        local item = Item.New()
        item.item_id = tonumber(item_data[1]) 
        item.num = tonumber(item_data[2]) 
        item.is_bind = tonumber(item_data[3]) 
        table.insert(t, item)
    end
end

function RewardData:CheckActivityBackRewardGroup(reward_group)
    if nil == self.activity_back_reward_groups[reward_group] then
        local t = {}
        for _, v in pairs(reward_group) do
            t[v.group_id] = t[v.group_id] or {}
            table.insert(t[v.group_id], v)
        end
        self.activity_back_reward_groups[reward_group] = t
    end
end

function RewardData:ActivityBackFunc()
    self.activity_back_func = {
        [FindType.ResearchTask] = BindTool.Bind(ResearchTaskData.Instance.GetActivityBackRewardList, ResearchTaskData.Instance),
        [FindType.TreviFountain] = BindTool.Bind(ActivityCtrl.Instance.GetActivityBackRewardListWish, ActivityCtrl.Instance),
        [FindType.GuildTask] = BindTool.Bind(GuildData.Instance.GetActivityBackRewardList, GuildData.Instance),
        [FindType.WSTQuesten] = BindTool.Bind(ActivityCtrl.Instance.GetWSTRewardList, ActivityCtrl.Instance),
        [FindType.TreasureMap] = BindTool.Bind(TreasureMapData.Instance.GetActivityBackRewardList, TreasureMapData.Instance),
        [FindType.BountyTask] = BindTool.Bind(TaskData.Instance.GetBountyTaskRewardList, TaskData.Instance),
        [FindType.FantasyFB] = BindTool.Bind(FightData.Instance.GetFantasyFBBackRewardList, FightData.Instance),
        [FindType.GuildAnswer] = BindTool.Bind(ActivityData.Instance.GetGuildAnswerBackRewardList, ActivityData.Instance),
        [FindType.GuildMelee] = BindTool.Bind(ActivityData.Instance.GetGuildMeleeBackRewardList, ActivityData.Instance),
        [FindType.OnlyFight] = BindTool.Bind(ActivityData.Instance.GetOnlyFightBackRewardList, ActivityData.Instance),
        [FindType.TeamSport] = BindTool.Bind(ActivityData.Instance.GetTeamSportBackRewardList, ActivityData.Instance),
        [FindType.SilkRoad] = BindTool.Bind(ActivityCtrl.Instance.GetSilkRoadBackRewardList, ActivityCtrl.Instance),
        [FindType.FightEnergy] = BindTool.Bind(ActivityData.Instance.GetFightEnergyRewardList, ActivityData.Instance),
        [FindType.WanLingActive] = BindTool.Bind(ActivityData.Instance.GetWanLingActiveRewardList, ActivityData.Instance),
        [FindType.CrystalXiYouActive] = BindTool.Bind(ActivityData.Instance.GetCrystalXiYouActiveRewardList, ActivityData.Instance),
        [FindType.ClourArenaActive] = BindTool.Bind(ActivityData.Instance.GetClourArenaActiveRewardList, ActivityData.Instance),
        [FindType.ResearchTaskActive] = BindTool.Bind(ActivityData.Instance.GetResearchTaskActiveRewardList, ActivityData.Instance),
        [FindType.BountyTaskActive] = BindTool.Bind(ActivityData.Instance.GetBountyTaskActiveRewardList, ActivityData.Instance),
        [FindType.SkillRoadAct] = BindTool.Bind(ActivityData.Instance.GetSkillRoadActFindCenterRewardList, ActivityData.Instance),
    }
end

-- 超特殊找回客户端自定义处理
function RewardData:ActivityBackSpData(find_type)
    self.activity_back_sp_data = {
        [FindType.OfflineReward] = {
            handle = RoleData.Instance.break_info,
            desc_fun = BindTool.Bind(RoleData.Instance.BreakTimeDesc, RoleData.Instance),
            num_fun = BindTool.Bind(RoleData.Instance.GetBreakExp, RoleData.Instance),
            receive_fun = BindTool.Bind(RoleData.Instance.ReceiveBreakExp, RoleData.Instance),
            remind_fun = BindTool.Bind(RoleData.Instance.SingleBreakRemind, RoleData.Instance),
            sp_reward = BindTool.Bind(RoleData.Instance.GetBreakNowItem, RoleData.Instance, false, true),
        },
    }
    return find_type and self.activity_back_sp_data[find_type] or self.activity_back_sp_data
end

function RewardData:GetRewardPanelJumpPos(list)
    local jump_pos = 0
    if self:IsRewardActivityBackAuto() then
        for index, v in pairs(list) do
            if Mod.Activity.ActivityBack == v.open then
                jump_pos = index
                break
            end
        end
    else
        for index, v in pairs(list) do
            if v.mod and 0 == jump_pos then
                local handle = RemindCtrl.Instance:Remind():RegisterGroup(v.mod,function ()
                end, true)
                if RemindCtrl.Instance:Remind():GetGroupNum(v.mod) > 0 then jump_pos = index end
                RemindCtrl.Instance:Remind():UnRegisterGroup(v.mod, handle)
            end
        end
    end
    return jump_pos
end

function RewardData:LevelGiftNoticeShowCheck()
    local reward = RewardData.Instance:GetRewardUpgradeList()
    local level = RoleData.Instance:GetRoleLevel()
    for index, value in ipairs(reward) do
        if value.get_flag == 0 and level >= value.level and value.show == 1 then
            return true, value
        end
    end
    --如果没有可领取那么就按照显示等级来预告
    for index, value in ipairs(reward) do
        if value.get_flag == 0 and value.show == 1 and value.show_level ~= "" and level >= value.show_level then
            return true, value
        end
    end
    return false
end

function RewardData:OpenPanelData(value)
    self.open_panel_data = value
end

function RewardData:GetOpenPanelType()
    local config = DataHelper.GetSearch(self.reward_show_list,[[open]],self.open_panel_data)
    return config and config.type or 1
end

function RewardData:GetOpenPanelData(list)
    return DataHelper.SimpleIndex(list,[[open]],self.open_panel_data)
end

local reward_open_func = {
    [Mod.Reward.Score] = function()
        return RewardData.Instance:GetScoreEnd()
    end,
    -- [Mod.Reward.FundExchange] = function()
    --     return RewardData.Instance:GetFundExchangeEnd()
    -- end,
    [Mod.Reward.ImmortalFund] = function()
        return RewardData.Instance:GetImmortalFundIsShow()
    end,
    [Mod.Reward.SuperVip] = function()
        return SuperVipCtrl.Instance.data:GetSuperVipEnd()
    end,
    [Mod.Reward.SportsCards] = function()
        return true -- SuperVipCtrl.Instance.data:GetSuperVipEnd()
    end,
    [Mod.Reward.MarryRecharge] = function()
        return ActivityRandData.Instance:GetRegisterOpen(Mod.Reward.MarryRecharge)
    end,
    [Mod.Reward.GodBeastGrace] = function()
        return ActivityRandData.Instance:GetRegisterOpen(Mod.Reward.GodBeastGrace)
    end,
}

function RewardData:GetRewardShowList()
    local list = {}
    for k,v in pairs(self.reward_show_list) do
        if list[v.type] == nil then
            list[v.type] = {}
        end
        if FunOpen.Instance:IsFunOpen(v.open) and (v.act_type == 0 or ActivityData.IsOpen(v.act_type)) then
            local open_func = reward_open_func[v.open]
            if open_func then
                if open_func() == true then
                    table.insert(list[v.type],v)
                end
            else
                table.insert(list[v.type],v)
            end    
        end
    end
    for k,v in pairs(list) do
        table.sort(v,function(a,b)
            return a.seq < b.seq
        end)
    end
    return list
end