
ArenaData = ArenaData or BaseClass()

function ArenaData:__init()
    if ArenaData.Instance ~= nil then
        Debuger.LogError("[ArenaData] attempt to create singleton twice===>AchieveData:__init")
        return 
    end
    ArenaData.Instance = self
    self.cfg = Config.arena_cfg_auto
    self.is_showed_first_in = false
    self.RoleInfo = SmartData.New({
        is_out_of_rank_list = 0,
        rank = 0,
        today_fight_count = 0,
        fetch_exp_flag = {},
        is_change = false,
        player_num_in_pool = 0,
        can_get_coin = 0,
        get_colosseum_credits = 0,    --宝库结算后可分得的积分数
        next_switch_timestamp = 0,    --本次结算结束的时间戳
        score = 0,
        capabolity = 0,
    })

    self.target_list = SmartData.New({data = {},is_change = false})

    self.top_list = SmartData.New({data = {},is_change = false})

    self.record_list = {}

    self.rank_list = {}

    self.rank_list_data=SmartData.New()

    self.rank_in_pool_list = {}

    self.history_highest_rank = 0
    self.rank_reward_fetch_falg = SmartData.New({data = {},is_change = false})
    self:GetFlipConfig()
    self.total_challenge_times = Config.arena_cfg_auto.other[1].challenge_times

    self.up_rank = nil
    self.has_new_report = false

    self.rank_pos_list = SmartData.New({data = {}, is_change = false})

    self:InitAchievementCfg()
    self.achievement_data = SmartData.New({flags = {},type_count_info = {},next_reset_time = 0})

    self.history_highest_info = SmartData.New()

    self.shop_smart = SmartData.New({is_change = false})
    self:initConfig()
end

function ArenaData:initConfig()
    self.history_rank_reward = {}
    for i = #Config.arena_cfg_auto.history_rank_reward, 1, -1 do
        local t = Config.arena_cfg_auto.history_rank_reward[i]
        t.quick_add = (t.rank_low - t.rank_high) * t.reward
        table.insert(self.history_rank_reward, t)
    end
end

function ArenaData:ClearData()
    self.is_showed_first_in = false
    self.rank_pos_list:Set({data = {},is_change = false})
    self.achievement_data:Set({flags = {},type_count_info = {},next_reset_time = 0})
    self.rank_reward_fetch_falg:Set({data = {},is_change = false})
    self.top_list:Set({data = {},is_change = false})
    self.rank_in_pool_list = {}
    self.history_highest_info:Clear()
end

function ArenaData:__delete()
    if ArenaData.Instance == self then
        ArenaData.Instance = nil
    end
    self.rank_list_data:Set({})
end

function ArenaData:GetFlipConfig()
    self.config = Config.arena_cfg_auto.reward
    if self.flip_data == nil then
        self.flip_data = {}
    end
    if self.flip_data[1] ~= nil then
        return self.flip_data
    end
    local index = #self.config
    for i=1,#self.config do
        self.flip_data[i] = self.config[index]
        self.flip_data[i].seq2 = i
        index = index - 1
    end
    return self.flip_data
end

function ArenaData:AnalysisRewardConfig()
    if self.flip_data == nil then
        Debuger.LogError("[ERROR] flip_data is nil")
        return {}
    end
    self.reward_list = {}
    for k, v in pairs(self.flip_data) do
        for x,y in pairs(v.reward) do
            table.insert(self.reward_list, y)
        end
        table.insert(self.reward_list,{item_id = CommonItemId.Coin,num = v.bind_coin})
    end
    return self.reward_list
end

function ArenaData:GetChallengeLeftTimes()
    return self.total_challenge_times - self.RoleInfo.today_fight_count > 0 and self.total_challenge_times - self.RoleInfo.today_fight_count or 0
end

function ArenaData:GotRewardList()
    if self.flip_data[1] == nil then return end
    local got_reward_list = {}
    local function isHave(item_id)
        for k, v in pairs(got_reward_list)do
            if v.item_id == item_id then
                return {res = true,value = v}
            end
        end
        return {res = false}
    end
    local function TableInsert(item)
        local result = isHave(item.item_id)
        if result.res then
            result.value.num = result.value.num + item.num
        else
            local tmp = TableCopy(item)
            table.insert(got_reward_list,tmp)
        end
    end
    for k, v in pairs(self.flip_data)do
        if self.rank_reward_fetch_falg.data[v.seq] == 1 then
            TableInsert(v.reward[0])
            TableInsert(v.reward[1])
            TableInsert(v.reward[2])
            TableInsert({item_id = CommonItemId.Coin ,num = v.bind_coin})
        end
    end
    return got_reward_list
end

function ArenaData:NextSwitchTS()
    return self.RoleInfo.next_switch_timestamp
end

function ArenaData:SetNextSwitchTS(ts)
    self.RoleInfo.next_switch_timestamp = ts
end

function ArenaData:SetHistoryHighestRank(protocol)
    if self.history_highest_info.cur_rank ~= nil and protocol.change_rank_num > 0 then
        self.history_highest_info.changed = true
    end
    self.history_highest_info.cur_rank = protocol.cur_rank
    self.history_highest_info.change_rank_num = protocol.change_rank_num
end

function ArenaData:HistoryHighestRankChanged()
    return self.history_highest_info.changed
end

function ArenaData:GetHistoryHighestRank()
    return self.history_highest_info
end

function ArenaData:GetHistoryHighestRankScore()
    local total_add = 0
    local pre_rank = self.history_highest_info.cur_rank + self.history_highest_info.change_rank_num
    local cur_rank = self.history_highest_info.cur_rank
    --旧排行大于最低段
    if pre_rank > self.history_rank_reward[1].rank_low then
        for _, v in ipairs(self.history_rank_reward) do
            if cur_rank < v.rank_high then
                total_add = total_add + v.quick_add
            else
                total_add = total_add + (v.rank_low - cur_rank) * v.reward
                break
            end
        end
        return total_add
    end
    for _, v in ipairs(self.history_rank_reward) do
        --变化未跨阶段
        if pre_rank <= v.rank_low and self.history_highest_info.cur_rank >= v.rank_high then
            total_add = self.history_highest_info.change_rank_num * v.reward
            break

        --跨阶段下段积分
        elseif pre_rank >= v.rank_high and cur_rank < v.rank_high then
            total_add = (pre_rank - v.rank_high) * v.reward
            
        --跨阶段中段积分
        elseif pre_rank > v.rank_low and cur_rank < v.rank_high then
            total_add = total_add + v.quick_add

        --跨阶段上段积分
        elseif cur_rank <= v.rank_low and cur_rank > v.rank_high then
            total_add = total_add + (v.rank_low - cur_rank) * v.reward
        end
    end
    return total_add
end

--是否已经领取过
function ArenaData:IsRewardFetchFalg(seq)
    return self.rank_reward_fetch_falg.data[seq] == 1
end

--获取古神排名奖励红点
function ArenaData:GetRemindNum()
    if self:HasRedPoint() then
        return 1
    end
    return 0
end

function ArenaData:GetRink()
    return self.history_highest_rank or 0
end

function ArenaData:IsInPool()
    for k,v in pairs(self.rank_in_pool_list) do
        if v.name == RoleData.Instance:GetRoleName() then
            return true
        end
    end
    return false
end

function ArenaData:GetConditionTip()
    return ""--Language.Arena.ConditionTip[self:IsInPool() and 2 or 1]
end

function ArenaData:GetShowFirstIn()
    return self.RoleInfo.is_out_of_rank_list == 1 and not self.is_showed_first_in
end

function ArenaData:GetMyRankReward()
    if self.RoleInfo.rank == 0 then
        return Language.Arena.NoneReward
    end
    for k,v in pairs(ArenaData.RewardRuleInfoList()) do
        if self.RoleInfo.rank >= v.min_rank and self.RoleInfo.rank <= v.max_rank then
            return string.format(Language.Arena.RewardRuleRewardFormat, v.rank_price, v.bind_coin)
        end
    end
    return Language.Arena.NoneReward
end

function ArenaData:SetResultInfo(protocol)
    self.new_rank = protocol.new_rank
    self.up_rank = protocol.up_rank > 0 and protocol.up_rank or 0
    self.up_score = 0--protocol.add_score > 0 and protocol.add_score or 0
    self.RoleInfo.score = protocol.score
end

function ArenaData:GetResultInfo()
    return {[1] = string.format(Language.Common.Xstr,COLORSTR.Green7, tostring(self.up_rank)),
            [2] = "",--tostring(self.up_score),
            [3] = tostring(self.RoleInfo.rank),
            [4] = tostring(self.RoleInfo.score)}
end

function ArenaData:GetRankNum()
    return self.RoleInfo.rank
end

function ArenaData.RewardRuleInfoList()
    return Config.arena_cfg_auto.reward
end

function ArenaData:GetHasLiBao()
    if self.rank_pos_list then
        local count = 0
        for k,v in pairs(self.rank_pos_list.data) do
            if v ~= 0 then
                count = count + 1
            end
        end
        return count >= 1, count
    end
    return false, 0
end

function ArenaData:GetShopRedPoint()
    if StoreData.Instance:GetBtnPageRedNum(2) > 0 then
        return 1
    end
    return 0
end

function ArenaData:HasRedPoint()
    if self.has_new_report == true then
        return true
    end
    if self:GetHasLiBao() == true then
        return true
    end
    if self:GetAchievementRemind() > 0 then
        return true
    end
    if self:GetShopRedPoint() > 0 then
        return true
    end
    return false
end

function ArenaData:GetBuffList()
    local buff_list = {}
    if self.target_list == nil or
        self.target_list.data == nil or 
        #self.target_list.data <= 0 then
        return buff_list
    end
    local buff_id_list = self.target_list.data[1].buff_id_list
    if buff_id_list == nil or #buff_id_list <= 0 then
        return buff_list
    end
    for i=1,COLOSSEUM_DEFENDER_BUFF_NUM do
        if i > #buff_id_list then
            break
        end
        local buff_cfg = Config.buff_list_auto.buff_list[buff_id_list[i]]
        if buff_cfg then
            table.insert(buff_list, buff_cfg)
        end
    end
    return buff_list
end

-------------- 竞技场成就 ----------

-- 整理竞技场成就
function ArenaData:InitAchievementCfg()
    self.achievement_group_cfg = {}
    for i,v in ipairs(self.cfg.arena_achievement) do
        if self.achievement_group_cfg[v.ach_group] == nil then
            self.achievement_group_cfg[v.ach_group] = {}
        end
        table.insert(self.achievement_group_cfg[v.ach_group],v)
    end
end

-- 设置竞技场成就标记
function ArenaData:SetAchievementFlag(protocol)
    self.achievement_data.next_reset_time = protocol.next_reset_time
    self.achievement_data.flags = protocol.flags
end

-- 设置竞技场成就进度数量
function ArenaData:SetAchievementCount(protocol)
    self.achievement_data.type_count_info = protocol.type_count_info
end

-- 提取成就组
function ArenaData:GetAchievementGroupFinishData(group_id)
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

-- 获取竞技成就数据
function ArenaData:GetAchievementData()
    local list_data = {}
    for i,v in ipairs(self.achievement_group_cfg) do
        local list = self:GetAchievementGroupFinishData(i)
        AndList(list_data,list)
    end
    local sort_func = function (a,b)
        a.flag = self:GetAchievementFlag(a)
        b.flag = self:GetAchievementFlag(b)
        a.progress_num = self:GetAchievementGroupCount(a.achieve_type)
        b.progress_num = self:GetAchievementGroupCount(b.achieve_type)
        if a.flag == b.flag then
            return a.sn < b.sn
        else
            return a.flag > b.flag
        end
    end
    table.sort(list_data,sort_func)
    return list_data
end

-- 竞技场成就标记 -- 0 未达成 1 可领取 -1 已领取 
function ArenaData:GetAchievementFlag(data)
    if self.achievement_data.flags[data.sn] == 1 then
        return -1
    end
    local progress_num = self:GetAchievementGroupCount(data.achieve_type)
    if data.achieve_type == 0 then
        if progress_num > 0 and progress_num <= data.ach_param1 then
            return 1
        end
    else
        if progress_num >= data.ach_param1 then
            return 1
        end
    end
   
    return 0
end

-- 竞技成就组完成的次数
function ArenaData:GetAchievementGroupCount(type_id)
    return self.achievement_data.type_count_info[type_id] or 0 
end

-- 成就红点逻辑
function ArenaData:GetAchievementRemind()
    if TableIsEmpty(self.achievement_data.flags) or TableIsEmpty(self.achievement_data.type_count_info) then
        return 0
    end
    for i,v in ipairs(self.cfg.arena_achievement) do
        if self:GetAchievementFlag(v) == 1 then
            return 1
        end
    end
    return 0
end

function ArenaData:GetShopRequire(value)
    if self:GetRink() ~= 0 then
        return self:GetRink() > value
    else
        return true
    end
end

--获取竞技场排行榜
function ArenaData:GetRankList()
    return self.rank_list or {}
end

-- 竞技场每次挑战可获得积分
function ArenaData:GetBaseBattleRewardScore()
    return Config.arena_cfg_auto.other[1].battle_reward or 50
end

function ArenaData:GetBattleRewardScore()
    return Format(Language.Arena.BattleRewardScore, self:GetBaseBattleRewardScore())
end
--竞技场挑战奖励
function ArenaData:SetBattleReward(reward_group)
    local config = Config.arena_cfg_auto.battle_reward[reward_group]
    if config then
        local list = {}
        if config.bind_coin > 0 then
            table.insert(list, {item_id = CommonItemId.Coin, num = config.bind_coin})
        end
        if config.integral > 0 then
            table.insert(list, {item_id = CommonItemId.ArenaPoint, num = config.integral})
        end
        if config.item_list ~= nil then
            for k, v in pairs(config.item_list) do
                table.insert(list, v)
            end
        end
        self.arena_finish_reward = list
    else
        -- LogDG("没有获取到竞技场奖励")
        self.arena_finish_reward = nil
    end
end
function ArenaData:FinishBattleReward()
    return self.arena_finish_reward or {}
end