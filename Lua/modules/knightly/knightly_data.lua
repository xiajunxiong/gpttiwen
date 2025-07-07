
KnightlyData = KnightlyData or BaseClass()

function KnightlyData:__init()
    self:KnightlyConfig()
    self:KnightlyData()
end

function KnightlyData:KnightlyConfig()
    self.config_chivalrous_achievement = Config.sundries_auto.chivalrous_achievement
end

function KnightlyData:KnightlyData()
    self.knightly_info = SmartData.New{
        history_chivalrous = 0,
        chivalrous_reward_seq = 0,
    }

    self.team_list = SmartData.New()
    self.knightly_reward_red_point = SmartData.New{val = false}
    self.team_list_empty = true

    self.reward_list = {}
    for _, co in ipairs(self.config_chivalrous_achievement) do
        local item_data = co.achievement_reward[0]
        co.rewards = {Item.Init(item_data.item_id, item_data.num, item_data.is_bind)}
        co.get_flag = 0
        table.insert(self.reward_list, co)
    end

    self.rank_type = 0
    self.rank_info = SmartData.New()
    
    self.flush_grey = SmartData.New{val = 0}
end

function KnightlyData:SetChivalrousTeamList(protocol)
    self.team_list:Set{}
    
    self.team_list_empty = true
    for _, info in pairs(protocol.team_list) do
        self.team_list_empty = false
        local index = info.team_index
        self.team_list[index] = TeamInfo.New()
        self.team_list[index]:TeamVo(info)
    end
end

function KnightlyData:SetRoleHistoryChivalrous(protocol)
    local info = protocol.info
    self.knightly_info:Set(protocol.info)
    local red_point = false
    for _, reward in ipairs(self.reward_list) do
        reward.get_flag = bit:_rshift(info.chivalrous_reward_flag, reward.seq) % 2
        if 0 == reward.get_flag and info.history_chivalrous >= reward.chivalrous_value then
            red_point = true
        end
    end
    self.knightly_reward_red_point:Set{val = red_point}
end

function KnightlyData:GetTeamList()
    local list = {}
    self.team_list_empty = true
    for k, v in pairs(self.team_list) do
        self.team_list_empty = false
        list[k] = v
    end
    table.sort(list, function (a, b)
        if not a or not b then return a end
        local ai = a.info
        local bi = b.info
        if ai.add_chivalrous_value == bi.add_chivalrous_value then
            local atg = TeamData.Instance:GetTeamGoalWithId(ai.team_type)
            local btg = TeamData.Instance:GetTeamGoalWithId(ai.team_type)
            local acs = atg and atg.chivalrous_seq or 999
            local bcs = btg and btg.chivalrous_seq or 999
            if acs == bcs then
                if ai.is_cross == bi.is_cross then
                    return ai.create_team_timestamp < bi.create_team_timestamp
                end
                return ai.is_cross > bi.is_cross
            end
            return acs < bcs
        end
        return ai.add_chivalrous_value > bi.add_chivalrous_value
    end)
    return list
end

function KnightlyData:GetHistoryInfo()
    local history_chivalrous = self.knightly_info.history_chivalrous
    for _, reward in ipairs(self.reward_list) do
        if history_chivalrous < reward.chivalrous_value then
            return reward
        end
    end
end

function KnightlyData:GetRewardList()
    return self.reward_list
end

function KnightlyData:SetChivalrousRankList(protocol)
    self.rank_type = protocol.rank_type
    self.rank_info:Set(protocol.rank_list)
end

function KnightlyData:SetChivalrousRankList(protocol)
    self.rank_type = protocol.rank_type
    self.rank_info:Set(protocol.rank_list)
end

function KnightlyData:FlushGrey()
    TimeHelper:CancelTimer(self.timer_tt_flush)
    self.flush_grey.val = KnightlyConfig.INTERVAL_FLUSH + 1
    self.timer_tt_flush = TimeHelper:AddCountDownTT(function ()
        self.flush_grey.val = self.flush_grey.val - 1
    end, function ()
        self.flush_grey.val = 0
    end, KnightlyConfig.INTERVAL_FLUSH, 1)
end