OnlyFightData = OnlyFightData or BaseClass()
function OnlyFightData:__init()
    if OnlyFightData.Instance ~= nil then
        Debuger.LogError("[OnlyFightData] attempt to create singleton twice!")
        return
    end
    OnlyFightData.Instance = self
    self.list_info = SmartData.New()
    self.myrank_info = SmartData.New()
    self.nums_info = SmartData.New({role_num = 0})
    self.all_auto = Config.enemyhundred_war_auto
    self.other_auto = self.all_auto.other[1]
    self.match_level = self.all_auto.match_level
    self.participation_reward = self.all_auto.participation_reward

    self.rank_reward = {}
    self.rank_reward_group = {}
    for _, v in ipairs(self.all_auto.rank_reward) do
        self.rank_reward[v.level_max] = self.rank_reward[v.level_max] or {}
        self.rank_reward_group[v.level_max]  = self.rank_reward_group[v.level_max] or {}
        for i = v.rank_min, v.rank_max do
            self.rank_reward[v.level_max][i] = v
        end
        table.insert(self.rank_reward_group[v.level_max], v) 
    end
    self.defeated_times = 0

    self.level_reward = {}
    for _,v in ipairs(self.participation_reward) do
        for i = v.level_min, v.level_max do
            self.level_reward[i] = v
        end
    end

    local live_reward = {}
    local index = 1
    for k,v in pairs(self.all_auto.live_reward) do
        live_reward[index] = live_reward[index] or {}
        if live_reward[index][1] then
            if v.level_min ~= live_reward[index][1].level_min then
                index = index + 1
                live_reward[index] = {}
            end
        end
        table.insert(live_reward[index], v)
    end
    self.all_auto.live_reward = live_reward
    ActivityData.Instance:RegisterRewardGetFunc(ActType.OnlyFight, function ()
        return self:GetTotalPreviewRewardShowOnly()
    end)
end
function OnlyFightData:SetRankAllInfo(info)
	self.list_info:Set(info)
    self:FlushRewardFlags()
end
function OnlyFightData:SetMyRankInfo(info)
    self.myrank_info:Set(info)
end

function OnlyFightData:RankAllInfo()
    return self.list_info
end
function OnlyFightData:MyRankInfo()
    return self.myrank_info
end

--排名奖励
function OnlyFightData:GetReward(rank)
    local level = self.join_level or RoleData.Instance:GetRoleLevel()
    local data = {}--TableCopy(self.rank_reward[rank])
    for k,v in pairs(self.rank_reward) do
        if v[rank] ~= nil and level <= tonumber(k) and level >= v[rank].level_min then
            -- data = TableCopy(self.rank_reward[k][rank]) or {}
            data.exp = self.rank_reward[k][rank].exp or 0
            data.coin = self.rank_reward[k][rank].coin or 0
            data.reward = self.rank_reward[k][rank].reward_show
            break
        end
    end
    -- local info = self.level_reward[level]
    -- if next(data) and info then
    --     data.exp = (data.exp or 0) + info.exp
    --     data.coin = (data.coin or 0) + info.coin
    --     for _,v in pairs(data.reward_show) do
    --         table.insert(data.reward, v)
    --     end
    -- end
    return data --next(data) ~= nil and data or {reward = {}, title = 0, coin = info.coin, exp = info.exp}-- self.other_auto.join_reward
end
function OnlyFightData:GetSuccReward()
    return {{name = Language.Guild.MeleeReward, num = string.format("+%s", self.other_auto.win_score)}}
end
function OnlyFightData:GetFailTip()
    OnlyFightData.Instance:SetDefeatedTimes(1)
    return Language.OnlyFight.FailTip[self.defeated_times or 1]
end
function OnlyFightData:SetDefeatedTimes(time)
    if time == 0 then
        self.defeated_times = 0
    end
    self.defeated_times = self.defeated_times + time
end
function OnlyFightData:IsInOnlyFight(in_scene, prepare)
    if in_scene == true then
        if prepare == true then
            local state = ActivityData.Instance:GetActivityStatus(ActType.OnlyFight)
            return state == ActStatusType.Open and SceneData.Instance:GetMode() == SCENE_TYPE.ONLY_FIGHT
        end
        return SceneData.Instance:GetMode() == SCENE_TYPE.ONLY_FIGHT
    else
        local state = ActivityData.Instance:GetActivityStatus(ActType.OnlyFight)
        return state == ActStatusType.Open and SceneData.Instance:GetMode() == SCENE_TYPE.ONLY_FIGHT
    end
end
function OnlyFightData:MatchLevel()
    local level = self.join_level or RoleData.Instance:GetRoleLevel()
    for i,v in ipairs(self.match_level) do
        if level >= v.min_level and level <= v.max_level then
            return v.min_level
        end
    end
    return 0
end

function OnlyFightData:SetRewardFlags(protocol)
    self.join_level = protocol.level
    -- LogError("SetRewardFlags",protocol.level)
    self.particip_award_flag = protocol.particip_award_flag
    self.survival_award_flag = protocol.survival_award_flag
    self:FlushRewardFlags()
end

function OnlyFightData:FlushRewardFlags()
    local enter_info = ActivityData.Instance:GetActivityEnterInfo(ActType.OnlyFight)
    local stamp_time = enter_info.enter_end_timestamp or 0
    if stamp_time > 0 then
        ActivityData.Instance:SetTimeLimitRewardBtnEffFlag(ActType.OnlyFight, false)
        return
    end
    if self.particip_award_flag ~= 1 then
        ActivityData.Instance:SetTimeLimitRewardBtnEffFlag(ActType.OnlyFight, true)
        return
    end
    if self:RankAllInfo().role_num ~= nil then
        local role_num = self:RankAllInfo().role_num - 1
        local alive_reward_item_list = self:AliveReward()
        for i,v in ipairs(alive_reward_item_list) do
            if self.survival_award_flag[#self.survival_award_flag + 1 - i] ~= 1 and role_num <= v.live_num then
                ActivityData.Instance:SetTimeLimitRewardBtnEffFlag(ActType.OnlyFight, true)
                return
            end
        end
    end
    ActivityData.Instance:SetTimeLimitRewardBtnEffFlag(ActType.OnlyFight, false)
end

--参与奖励
function OnlyFightData:GetParticipationReward(show_only)
    -- self.join_level = self.join_level and self.join_level or RoleData.Instance:GetRoleLevel()
    local level = show_only and RoleData.Instance:GetRoleLevel() or self.join_level
    local participation_reward = self.level_reward[level]
    local item_list = {}
    if participation_reward.exp > 0 then
        table.insert(item_list, Item.Init(CommonItemId.Exp,participation_reward.exp))
    end
    if participation_reward.coin > 0 then
        table.insert(item_list, Item.Init(CommonItemId.Coin,participation_reward.coin))
    end
    if next(participation_reward.join_reward) then
        for k,v in pairs(participation_reward.join_reward) do
            table.insert(item_list, Item.Init(v.item_id, v.num, v.is_bind))
        end
    end
    return item_list
end
--生存奖励
function OnlyFightData:AliveReward(show_only)
    local level = show_only and RoleData.Instance:GetRoleLevel() or self.join_level
    for i,v in ipairs(self.all_auto.live_reward) do
        if level >= v[1].level_min and level <= v[1].level_max then
            return v
        end
    end
end

function OnlyFightData:GetTotalPreviewRewardShowOnly()
    return self:GetTotalPreviewReward(true)
end

function OnlyFightData:GetTotalPreviewReward(show_only)
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
            reward_item_list = {{tips_act_type = ActType.OnlyFight}}
        },
    }
    local function sendGetReward(param)
        local enter_info = ActivityData.Instance:GetActivityEnterInfo(ActType.OnlyFight)
        local stamp_time = enter_info.enter_end_timestamp or 0
        if param.op_type == ActivityConfig.TIME_LIMIT_REWARD_GET_OP.JOIN_AWARD then
            if stamp_time > 0 then
                PublicPopupCtrl.Instance:Center(Language.OnlyFight.EnterCanGet)
                return
            end
        elseif param.op_type == ActivityConfig.TIME_LIMIT_REWARD_GET_OP.SURVIVAL_AWARD then
            if stamp_time > 0 or self:RankAllInfo().role_num - 1 > param.param1 then
                PublicPopupCtrl.Instance:Center(Language.OnlyFight.ConditionNotMatch)
                return
            end
        end
        OnlyFightCtrl.Instance:SendGetReward(param)
    end
    local enter_info = ActivityData.Instance:GetActivityEnterInfo(ActType.OnlyFight)
    local stamp_time = enter_info.enter_end_timestamp or 0
    stamp_time = stamp_time > 0 and 2 or 0
    --参与奖励
    local partic_temp = {}
    partic_temp.title_name = Language.OnlyFight.JoinReward
    if not show_only then
        partic_temp.btn_flag = self.particip_award_flag == 0 and stamp_time or self.particip_award_flag    --是否已领取，用于按钮的置灰
        partic_temp.click_func = sendGetReward
        partic_temp.func_param = {op_type = ActivityConfig.TIME_LIMIT_REWARD_GET_OP.JOIN_AWARD, activity_type = ActType.OnlyFight} --回调接口参数
    end
    partic_temp.item_list = self:GetParticipationReward(show_only)
    table.insert(reward_list[1].reward_item_list, partic_temp)

    --存活奖励
    local alive_reward_item_list = self:AliveReward(show_only)
    local alive_temp = {}
    alive_temp.child_list = {}
    alive_temp.title_name = Language.OnlyFight.AliveReward
    local role_num = self:RankAllInfo().role_num and self:RankAllInfo().role_num - 1 or 1
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
        end
        table.insert(alive_temp.child_list, child_temp)
    end
    table.insert(reward_list[1].reward_item_list, alive_temp)
    -- LogError("show_only",show_only)
    local level = show_only and RoleData.Instance:GetRoleLevel() or self.join_level
    local rank_data = {}
    for k,v in pairs(self.rank_reward_group) do
        if level <= tonumber(k) and level >= v[1].level_min then
            rank_data = v
            break
        end
    end
    --排行奖励
    for k,v in pairs(rank_data) do
        local temp = {}
        if v.rank_min == v.rank_max then
            temp.title_name = string.format(Language.OnlyFight.RankTitleFormat[1], v.rank_min)
        else
            temp.title_name = string.format(Language.OnlyFight.RankTitleFormat[2], v.rank_min, v.rank_max)
        end
        temp.title_type = 2
        if not show_only then
            temp.tip_str = Language.OnlyFight.RankRewardTip
        end
        temp.item_list = {}
        for _,w in pairs(v.reward_show) do
            table.insert(temp.item_list, Item.Init(w.item_id, w.num, w.is_bind))
        end
        if v.coin > 0 then
            table.insert(temp.item_list, Item.Init(CommonItemId.Coin,v.coin))
        end
        
        table.insert(reward_list[2].reward_item_list, temp)
    end
    return reward_list
end