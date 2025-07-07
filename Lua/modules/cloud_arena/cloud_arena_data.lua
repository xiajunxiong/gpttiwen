CloudArenaData = CloudArenaData or BaseClass()
CLOUD_ARENA_RANK_TYPE = {
    TotalRank = 0,      --总榜
    ServerRank = 1,     --本服榜
    LevelRank = 2,      --段位榜
}

CloudArenaRankName = {
    "XinRui",
    "JingYing",
    "ZhanJiang",
    "TongShuai",
    "ChuanQi",
    "DouZhanDianTang",
}

CloudArenaOperaAngles = {
    [1] = 0.8,
    [2] = 17.4,
    [3] = 40,
    [4] = 59,
}
CLOUD_ARENA_ACHIEVEMENT = 
{
    CLOUD_ARENA_ACHIEVEMENT_RANK = 0,             --//!< 排名
    CLOUD_ARENA_ACHIEVEMENT_CHALLENGE_TIMES = 1,  --//!< 挑战
    CLOUD_ARENA_ACHIEVEMENT_WIN_TIMES = 2,        --//!< 挑战成功次数
    -- CLOUD_ARENA_ACHIEVEMENT_LOSE_TIMES = 3,       --//!< 挑战失败次数
    CLOUD_ARENA_ACHIEVEMENT_CONTINUOUS_WIN = 3,   --//!< 连胜次数
    CLOUD_ARENA_ACHIEVEMENT_WIN_HIGHER_RANK = 4,  --//!< 挑战更高段位的玩家并获胜次数
    CLOUD_ARENA_ACHIEVEMENT_TOTAL_DAMAGE = 5,     --//!< 单场总伤害
    CLOUD_ARENA_ACHIEVEMENT_USE_MEDICINE = 6,     --//!< 使用药品次数
    CLOUD_ARENA_ACHIEVEMENT_REALIVE_TIMES = 7,    --//!< 复活次数
    CLOUD_ARENA_ACHIEVEMENT_WIN_ROUND_NO_LARGER_THAN_5 = 8, --胜利回合不超过5回合
}

CloudArenaData.RedpointNumFunc = {
    "HasNewFightReport",
    "HasRemainMaxTimes",
    "GetHasAchieveReward",
    "HasRankingRewardCanGet",
    "HasDayReward",
}

CloudArenaData.RedpointPos = {
    Vector3.New(16,57,0),
    Vector3.New(53,18,0),
    Vector3.New(28,33,0),
    Vector3.New(28,33,0),
    Vector3.New(24,15,0),
}

CloudArenaData.RankingEffId = {
    2166010,
    2166009,
    2166008,
    2166007,
    2166006,
    2166005,
}

CloudArenaData.DayRewardItem = {
    65682,
    65532,
}

CloudArenaData.FormationType = {
    Empty = 0,
    MainRole = 1,
    Pet = 2,
    Partner = 3,
}

local STATE = {
    Empty = -1,
    Fight = 1,
    Assist = 2,
    StandBy = 3,
    Relax = 4,
}

CloudArenaData.TargetCount = 3

CloudArenaData.EmptyFormationGrid = {id = 0, type = 0, unique_id_1 = 0, unique_id_2 = 0}

function CloudArenaData:__init()
    if CloudArenaData.Instance ~= nil then
        Debuger.LogError("[CloudArenaData] attempt to create singleton twice!")
        return
    end
    self.rank_list = {}
    self.my_rank_info = {}
    CloudArenaData.Instance = self
    self.mySelfInfo = SmartData.New()
    self.notice_smart = SmartData.New({
        target_refresh = false,
        new_record = false,
        TotalRank = false,
        ServerRank = false,
        LevelRank = false,
        cur_target_index = 1,
        rank_type = 0,
        reward_type = 1,
        achievement_update = false,
        selfInfoFlush = false,
        dan_change = false,
        first_open = false,
        reward_red = false,
        open_initial_view = false,
        refresh_left = 0,})

    self.formation_smart = SmartData.New({
        cur_is_pet = 0,         --0是伙伴 1是宠物
        partner = 0,
        pet_index = -1,
        click_card = false,
        pos_change = {
            arrow = 0,
            lights = {}
        },
        pos_changed = false,
        cur_select_state = -1,
        auto_save = false,
    })
    self.main_view_btn_check = SmartData.New({val = false})
    self.other_cfg = Config.cloud_battlefield_auto.others[1]
    -- self.dan_change_data = {old_dan = 1, new_dan = 2}
    self:initConfig()
end

function CloudArenaData:__delete()
    CloudArenaData.Instance = nil
end

function CloudArenaData:initConfig()
    self.achievement_cfg = {}
    for k,v in pairs(Config.cloud_battlefield_auto.achievement) do
        if v.hide == 0 then
            if self.achievement_cfg[v.achieve_type] == nil then
                self.achievement_cfg[v.achieve_type] = {}
            end
            self.achievement_cfg[v.achieve_type][v.ach_level] = v
        end
    end
    self.max_ranking = 0
    for k,v in pairs(Config.cloud_battlefield_auto.ranking_cfg) do
        if v.ranking > self.max_ranking then
            self.max_ranking = v.ranking
        end
    end
    self.max_ranking = 0
    for k,v in pairs(Config.cloud_battlefield_auto.ranking_cfg) do
        if v.ranking > self.max_ranking then
            self.max_ranking = v.ranking
        end
    end
end

function CloudArenaData:ClearCache()
    self.notice_smart:Set({
        target_refresh = false, 
        new_record = false, 
        TotalRank = false, 
        ServerRank = false, 
        LevelRank = false, 
        cur_target_index = 1,
        rank_type = 0,
        reward_type = 1,
        achievement_update = false,
        selfInfoFlush = false,
        dan_change = false,
        first_open = false,
        refresh_left = 0,})
    -- self.mySelfInfo:Clear()
    self.cur_rank_cfg = nil
    self.achievement_info = nil
end

function CloudArenaData:SetRealTimeInfo(protocol)
    if protocol.cur_rank < 0 then
        self:ClearCache()
        self.mySelfInfo.cur_rank = protocol.cur_rank
        self.mySelfInfo.cur_score = protocol.cur_score
        return
    else
        self.mySelfInfo.cur_rank = protocol.cur_rank
        self.mySelfInfo.cur_score = protocol.cur_score
    end
    local rank_cfg = self:GetRankConfig()
    if self.cur_rank_cfg ~= nil then
        if self.cur_rank_cfg.rank < rank_cfg.rank then
            if self.notice_smart.dan_change then
                self.notice_smart:SetDirty("dan_change")
            else
                self.notice_smart.dan_change = true
            end
        end
    end
    self.cur_rank_cfg = self:GetRankConfig()
end

function CloudArenaData:SetSelfInfo(protocol)
    if self.mySelfInfo.cur_score ~= nil then
        self.finish_score_change = protocol.cur_score - self.mySelfInfo.cur_score
    else
        CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_ACHIEVEMENT_INFO)
    end
    if protocol.cur_rank < 0 then
        self:ClearCache()
        self.mySelfInfo:Set(TableCopy(protocol))
        return
    else
        self.mySelfInfo:Set(TableCopy(protocol))
    end
    local rank_cfg = self:GetRankConfig()
    if self.cur_rank_cfg ~= nil then
        if self.cur_rank_cfg.rank == rank_cfg.rank and self.cur_rank_cfg.level ~= rank_cfg.level then
            self.finish_rank_change = rank_cfg.level - self.cur_rank_cfg.level
        elseif self.cur_rank_cfg.rank ~= rank_cfg.rank then
            self.finish_rank_change = rank_cfg.rank - self.cur_rank_cfg.rank
        end
    end
    self.cur_rank_cfg = self:GetRankConfig()
end

function CloudArenaData:GetSelfInfo()
    return self.mySelfInfo
end

function CloudArenaData:GetCurRanking()
    if self.cur_rank_cfg then
        return self.cur_rank_cfg.ranking_title
    else
        return Language.CloudArena.NoRankTemp
    end
end

function CloudArenaData:GetSeasonIdx()
    return self.mySelfInfo.season_idx
end

function CloudArenaData:ChallengeCanBuyToday()
    return self.other_cfg.recovery_limit - (self.mySelfInfo.today_buy_times or 0)
end

function CloudArenaData:GetLatestCanGet()
    for i,v in ipairs(self.mySelfInfo.reach_ranking_flag) do
        if v == 1 and self.mySelfInfo.fetch_ranking_reward_flag[i] == 0 then
            return true, i
        elseif v == 0 and i <= self.max_ranking + 1 then
            return false, i
        end
    end
    return false,0
end

function CloudArenaData:GetRankingRewardFlag(ranking_index)
    return {reach_flag = self.mySelfInfo.reach_ranking_flag[ranking_index], get_flag = self.mySelfInfo.fetch_ranking_reward_flag[ranking_index]}
end

function CloudArenaData:SetTargetInfo(protocol)
    self.target_list = protocol.target_list
    self.next_manual_refresh_timestamp = protocol.next_manual_refresh_timestamp
    self.notice_smart.refresh_left = self.other_cfg.refresh_time - protocol.today_already_refresh_times <= 0 and 0 or self.other_cfg.refresh_time - protocol.today_already_refresh_times
    self.notice_smart:SetDirty("target_refresh")
end

function CloudArenaData:GetTargetList()
    return self.target_list
end

function CloudArenaData:GetCurTargetInfo()
    if self.target_list ~= nil then
        return self.target_list[self.notice_smart.cur_target_index] 
    end
end

function CloudArenaData:SetNewFightRecordNotice(protocol)
    if self.record_list ~= nil then
        CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_RECORD_LIST, 0, 20)
    end
end

function CloudArenaData:SetFightRecordList(protocol)
    self.record_list = protocol.record_list
    local last_record = self.record_list[#self.record_list]
    local main_role_id = RoleData.Instance:GetRoleId()
    if last_record ~= nil and main_role_id == last_record.defender_info.uid then
        if self.notice_smart.new_record then
            self.notice_smart:SetDirty("new_record")
        else
            self.notice_smart.new_record = true
        end
    end
end

function CloudArenaData:GetRecordList()
    return self.record_list
end

function CloudArenaData:SetRankList(protocol)
    self.rank_list[protocol.rank_type] = protocol.rank_list
    self.my_rank_info[protocol.rank_type] = protocol.my_rank
    self.notice_smart:SetDirty("rank_refresh")
end

function CloudArenaData:GetCurRankList()
    return self.rank_list[self.notice_smart.rank_type]
end

function CloudArenaData:GetCurMyRankInfo()
    return self.my_rank_info[self.notice_smart.rank_type]
end

function CloudArenaData:GetShowSeasonLeftDay()
    if not FunOpen.Instance:GetFunIsOpened(Mod.CloudArena.Main) then
        return false
    end
    if self.mySelfInfo.season_finish_timestamp ~= nil then
        local time_offset = self.mySelfInfo.season_finish_timestamp - TimeCtrl.Instance:GetServerTime()
        if time_offset > 0 and time_offset <= 86400 * 3 then
            return true
        end
    end
    return false
end

function CloudArenaData:SetSeasonInitialScore(protocol_score)
    self.season_initial_score = protocol_score
    if GuideData.Instance:GetGuideFlag(FuncGuideFlagType.FUNC_GUIDE_CLOUD_ARENA) == 1 and protocol_score >= 1000 then
        self.notice_smart.open_initial_view = true
    end
end

function CloudArenaData:SetDanChange(protocol)
    self.dan_change_data = protocol
    if self.notice_smart.dan_change then
        self.notice_smart:SetDirty("dan_change")
    else
        self.notice_smart.dan_change = true
    end
end

function CloudArenaData:SetAchievementInfo(protocol)
    self.achievement_info = self.achievement_info or {}
    for k,v in pairs(protocol.achieve_list) do
        if self.achievement_cfg[v.achieve_type] ~= nil then
            self.achievement_info[v.achieve_type] = TableCopy(v)
        end
    end
    -- self.achievement_info = TableCopy(protocol.achieve_list)
    for k,v in pairs(self.achievement_info) do
        v.config = self:GetAchievementConfig(v.achieve_type, v.cur_level)
    end
end

function CloudArenaData:GetAchievementInfo()
    return self.achievement_info or {}
end

function CloudArenaData:GetHasAchieveReward()
    if self.achievement_info == nil then
        return 0
    end
    for k,v in pairs(self.achievement_info) do
        if v.reward_status == 1 then
            return 1
        end
    end
    return 0
end

function CloudArenaData:SetAchievementUpdate(protocol)
    self.achievement_info = self.achievement_info or {}
    self.achievement_info[protocol.achieve_type] = TableCopy(protocol)
    self.achievement_info[protocol.achieve_type].config = self:GetAchievementConfig(protocol.achieve_type, protocol.cur_level)
    -- self.achievement_info[CLOUD_ARENA_ACHIEVEMENT[protocol.achieve_type]] = protocol
    self.notice_smart:SetDirty("achievement_update")
end

function CloudArenaData:SetSeasonHighestData(protocol)
    self.season_highest_rank = protocol.season_highest_rank
    self.season_highest_dan_id = protocol.season_highest_dan_id
    self.season_shop_type = protocol.season_shop_type
end

function CloudArenaData:GetAchievementConfig(achieve_type, level)
    if self.achievement_cfg[achieve_type] then
        if self.achievement_cfg[achieve_type][level] == nil then
            local max_level = 0
            for k,v in pairs(self.achievement_cfg[achieve_type]) do
                max_level = v.ach_level > max_level and v.ach_level or max_level
            end
            return self.achievement_cfg[achieve_type][max_level]
        else
            return self.achievement_cfg[achieve_type][level]
        end
    end
end

function CloudArenaData:GetCurRewardList()
    if self.notice_smart.reward_type == 0 then
        return Config.cloud_battlefield_auto.ranking_cfg
    else
        return Config.cloud_battlefield_auto.ranking_reward
    end
end

function CloudArenaData:GetCurRankReward(rank)
    rank = rank or self.mySelfInfo.cur_rank + 1
    for k,v in pairs(Config.cloud_battlefield_auto.ranking_reward) do
        if v.min_rank_pos == v.max_rank_pos and v.min_rank_pos == rank      --独立排名奖励
            or v.min_rank_pos < v.max_rank_pos and v.min_rank_pos <= rank and v.max_rank_pos >= rank then --排名段奖励
            return v
        end
    end
end

function CloudArenaData:GetRefreshCountDown()
    return self.other_cfg.refresh_time
end

function CloudArenaData:GetRankConfig(score, index)
    if index ~= nil then
        return Config.cloud_battlefield_auto.ranking_cfg[index]
    end
    local rank_cfg = nil--Config.cloud_battlefield_auto.ranking_cfg[1]
    score = score or self.mySelfInfo.cur_score
    for i,v in ipairs(Config.cloud_battlefield_auto.ranking_cfg) do
        if score >= v.score then
            rank_cfg = v
        else
            return rank_cfg
        end
    end
    return rank_cfg
end

function CloudArenaData:GetFinishInfo()
    local finish_data = FinishData.Instance:GetFinishInfo()

    local finish_info = {}
    local str_color = self.finish_score_change <= 0 and COLORSTR.Red8 or COLORSTR.Green4
    local change_score = self.finish_score_change <= 0 and self.finish_score_change or "+"..self.finish_score_change
    change_score = change_score == 0 and "--" or change_score
    finish_info.score_change = string.format(Language.CloudArena.FinishScoreChange,self.mySelfInfo.cur_score, str_color, change_score)
    finish_info.coin_change = 0
    --平局
    if FinishData.Instance:IsDraw() then
        for k,v in pairs(Config.cloud_battlefield_auto.challenge_integral) do
            if self.finish_score_change > 0 and v.drawn_price == self.finish_score_change then
                finish_info.coin_change = v.drawn_money
                break
            end
        end
    else
        --胜利
        if finish_data.is_win == 1 then
            for k,v in pairs(Config.cloud_battlefield_auto.challenge_integral) do
                if self.finish_score_change > 0 and v.win_price == self.finish_score_change then
                    finish_info.coin_change = v.win_money
                    break
                end
            end
        else
            --失败
            finish_info.coin_change = Config.cloud_battlefield_auto.challenge_integral[1].lose_money
        end
    end
    --LogError("CloudArenaData finish_info ",finish_info, finish_data)
    return finish_info
end

function CloudArenaData:CloudArenaRemind()
    if self.notice_smart.dan_change then
        return 1
    elseif self.mySelfInfo.day_reward_status == nil then
        return 0
    end
    if self.mySelfInfo.day_reward_status == 1
        or self.mySelfInfo.remain_fight_times >= self.other_cfg.max_challenge
        or self:GetHasAchieveReward() > 0 then
        return 1
    end
    local get_flag = self:GetLatestCanGet()
    if get_flag == true then
        return 1
    end
    return 0
end

function CloudArenaData:HasNewFightReport()
    return self.notice_smart.new_record and 1 or 0
end

function CloudArenaData:HasRemainMaxTimes()
    return self.mySelfInfo.remain_fight_times >= self.other_cfg.max_challenge and 1 or 0
end

function CloudArenaData:HasDayReward()
    return self.mySelfInfo.day_reward_status == 1 and 1 or 0
end

function CloudArenaData:HasRankingRewardCanGet()
    local get_flag = self:GetLatestCanGet()
    if get_flag == true then
        return 1
    else
        return 0
    end
end

function CloudArenaData.GetRankingTitle(rank_level)
    for k,v in pairs(Config.cloud_battlefield_auto.ranking_cfg) do
        if v.ranking == rank_level then
            return v.ranking_title
        end
    end
end

function CloudArenaData:GetShopRequire(limit_type, value)
    if self.cur_rank_cfg == nil then
        return true
    end
    if limit_type == 4 then
        return self.season_highest_dan_id < value
    else
        return self.season_highest_rank == nil or self.season_highest_rank < 0 or self.season_highest_rank + 1 > value
    end
end

function CloudArenaData:SetFormationInfo(protocol)
    self.formation_origin_info = TableCopy(protocol)
    self.formation_grid_list = TableCopy(protocol.grid_list)
    self.partner_id_list = {}
    self.pet_on_list = {}
    self.main_pet_index = -1
    for k,v in pairs(protocol.grid_list) do
        if v.type == CloudArenaData.FormationType.Partner then
            self.partner_id_list[v.id] = v
        elseif v.type == CloudArenaData.FormationType.Pet then
            local index = PetData.Instance:GetPetIndexByUniqueId(v.unique_id_1, v.unique_id_2)
            if index ~= -1 then
                if k == 3 or k == 8 then
                    self.main_pet_index = index
                end
                self.pet_on_list[index] = v
            end
        end
    end
    self.standby_index_list = {}
    self.origin_standby_index_list = {}
    for k,v in pairs(protocol.standby_list) do
        if v.pet_id ~= 0 then
            local cur_pet_index = PetData.Instance:GetPetIndexByUniqueId(v.unique_id_1, v.unique_id_2)
            if cur_pet_index ~= -1 then
                self.standby_index_list[cur_pet_index] = PetData.Instance:GetPetInfo(cur_pet_index)
                self.origin_standby_index_list[cur_pet_index] = self.standby_index_list[cur_pet_index]
            end
        end
    end
    self:FormationParse()
end

function CloudArenaData:StandbyCount()
    local count = 0
    if self.standby_index_list then
        for k,v in pairs(self.standby_index_list) do
            count = count + 1
        end
    end
    return count
end

function CloudArenaData:GetFormationInfo()
    local formation_info = {}
    formation_info.grid_list = self.formation_grid_list

    formation_info.standby_list = {}
    for k,v in pairs(self.standby_index_list) do
        table.insert(formation_info.standby_list,{pet_id = v:ID(), unique_id_1 = v.info.pet_unique_id1, unique_id_2 = v.info.pet_unique_id2})
    end
    for i=#formation_info.standby_list,4 do
        table.insert(formation_info.standby_list,{})
    end
    return formation_info
end

function CloudArenaData:CheckFormationChange()
    local origin = self.formation_origin_info
    for k,v in pairs(self.formation_grid_list) do
        if v.type ~= origin.grid_list[k].type
            or v.id ~= origin.grid_list[k].id
            or v.unique_id_1 ~= origin.grid_list[k].unique_id_1 
            or v.unique_id_2 ~= origin.grid_list[k].unique_id_2 then
            return true
        end
    end

    for k,v in pairs(self.origin_standby_index_list) do
        if self.standby_index_list[k] == nil then
            return true
        end
    end
    for k,v in pairs(self.standby_index_list) do
        if self.origin_standby_index_list[k] == nil then
            return true
        end
    end
end

function CloudArenaData:FormationInfo()
    return self.formation_info
end

function CloudArenaData:PartnerOnFight(partner_id)
    return self.partner_id_list and self.partner_id_list[partner_id]
end

function CloudArenaData:PetOnFight(pet_index)
    return self.pet_on_list and self.pet_on_list[pet_index]
end

function CloudArenaData:PetFormationStatus(index)
    if index == self.main_pet_index then
        --主战
        return PPStateSp.main, STATE.Fight
    elseif self.standby_index_list and self.standby_index_list[index] then
        --待机
        return PPStateSp.dai, STATE.StandBy
    elseif self.pet_on_list and self.pet_on_list[index] then
        --助战
        return PPStateSp.zhu, STATE.Assist
    end
end

function CloudArenaData:GetPartnerList()
    local function sort_lock(a,b)
        local av = a.vo
        local bv = b.vo
        if av.is_unlocked == bv.is_unlocked then
            if self:PartnerOnFight(a.id) and self:PartnerOnFight(b.id) or (self:PartnerOnFight(a.id) == nil and self:PartnerOnFight(b.id) == nil) then
                local arp = a.acti_red_point.val and 1 or 0
                local brp = b.acti_red_point.val and 1 or 0
                if arp == brp then
                    if a.partner_quality == b.partner_quality then
                        return a.id < b.id
                    end
                    return a.partner_quality > b.partner_quality
                end
                return arp > brp
            else
                return self:PartnerOnFight(a.id) ~= nil
            end
        end
        return av.is_unlocked > bv.is_unlocked
    end
    local list = {}
    for _, info in pairs(PartnerData.Instance.partner_list) do
        -- if self.partner_id_list == nil or self.partner_id_list[info.id] == nil then
            table.insert(list, info)
        -- end
    end
    table.sort(list, sort_lock)
    return list
end

function CloudArenaData:GetPetList()
    local pet_list = PetData.Instance:GetPetList()
    pet_list = table.rearrange(pet_list)
    local function sort_func(a,b)
        local order_a = 0
        local order_b = 0
        --排序优先级（出战>助战>待机>评分>等级>品质>强化等级>序号）
        if a.index == self.main_pet_index then
            order_a = order_a + 1000000
        elseif b.index == self.main_pet_index then
            order_b = order_b + 1000000
        elseif self:PetOnFight(a.index) or self:PetOnFight(b.index) then
            order_a = order_a + (self:PetOnFight(a.index) and 500000 or 0)
            order_b = order_b + (self:PetOnFight(b.index) and 500000 or 0)
        elseif self.standby_index_list[a.index] ~= nil or self.standby_index_list[b.index] ~= nil then
            order_a = order_a + (self.standby_index_list[a.index] ~= nil and 100000 or 0)
            order_b = order_b + (self.standby_index_list[b.index] ~= nil and 100000 or 0)
        end
        ---------------
        if a:Quality() > b:Quality() then
            order_a = order_a + 100
        elseif a:Quality() < b:Quality() then
            order_b = order_b + 100
        end
        if a:Level() > b:Level() then
            order_a = order_a + 1000
        elseif a:Level() < b:Level() then
            order_b = order_b + 1000
        end
        if a:GetScore() > b:GetScore() then
            order_a = order_a + 10000
        elseif a:GetScore() < b:GetScore() then
            order_b = order_b + 10000
        end
        if a:StrLevel() > b:StrLevel() then
            order_a = order_a + 10
        elseif a:StrLevel() < b:StrLevel() then
            order_b = order_b + 10
        end
        if a.index < b.index then
            order_a = order_a + 1
        elseif a.index > b.index then
            order_b = order_b + 1
        end
        return order_a > order_b
    end
    table.sort(pet_list, sort_func)
    return pet_list
end

function CloudArenaData:RoleAndPartnerPosChangeLights(pos)
    local lights = {}
    if 3 == pos or 8 == pos then
        lights = {[3] = true, [8] = true}
    else
        local info_p = self.formation_grid_list[pos]
        local oppo_is_pet = self.formation_grid_list[pos > 5 and pos - 5 or pos + 5].type == CloudArenaData.FormationType.Pet
        if info_p.type == CloudArenaData.FormationType.Partner then
            for i = 1, 10 do
                if 3 ~= i and 8 ~= i then
                    local oppo = i > 5 and i - 5 or i + 5
                    if self.formation_grid_list[i].type ~= CloudArenaData.FormationType.Pet or (i == pos or oppo == pos) or not oppo_is_pet then
                        lights[i] = ((self.formation_grid_list[oppo].type ~= CloudArenaData.FormationType.Partner) or (i == pos or oppo == pos))
                    end
                end
            end
        end
    end
    return lights
end

function CloudArenaData:PetPosChangeLights(pos)
    local lights = {}
    local info_p = self.formation_grid_list[pos]
    if info_p.type == CloudArenaData.FormationType.Pet then
        if pos == 3 or pos == 8 and not self.formation_smart.click_card then
            self.formation_smart.cur_select_state = STATE.Fight
        end
        --点击助战位时
        if pos ~= 3 and pos ~= 8 then
            self.formation_smart.cur_select_state = STATE.Assist
        end
        for i = 1, 10 do
            local oppo = i > 5 and i - 5 or i + 5
            --助战，当是点击角色卡时包括主战位
            if 3 ~= i and 8 ~= i or self.formation_smart.click_card then
                lights[i] = ((self.formation_grid_list[oppo].type ~= CloudArenaData.FormationType.Pet and self.formation_grid_list[i].type ~= CloudArenaData.FormationType.Partner)
                             or (i == pos or oppo == pos))
            elseif 3 == i or 8 == i then
                lights[i] = self.formation_grid_list[i].type ~= CloudArenaData.FormationType.MainRole
            end
        end
    end
    return lights
end

function CloudArenaData:FormationParse()
    local detail = self.formation_grid_list
    if nil == detail then return end
    local formation_info = {}
    for k,v in pairs(detail) do
        if v.type == CloudArenaData.FormationType.MainRole then
            formation_info[k] = FormationInfo.RoleParse(k)
        elseif v.type == CloudArenaData.FormationType.Partner then
            formation_info[k] = FormationInfo.PartnerParse(v.id, k)
        elseif v.type == CloudArenaData.FormationType.Pet then
            local pet_index = PetData.Instance:GetPetIndexByUniqueId(v.unique_id_1, v.unique_id_2)
            if pet_index and pet_index > -1 then
                local pet = PetData.Instance:GetPetInfo(pet_index)
                if pet ~= nil then
                    formation_info[k] = FormationInfo.PetParse(pet_index, k)
                end
            end
        end
    end
    self.formation_info = SmartData.New(formation_info)
end

function CloudArenaData:PartnerAddBegin()
    local lights = self:PatnerAddLights()
    self.formation_smart.cur_select_state = self.formation_smart.cur_select_state == STATE.Empty and STATE.Relax or self.formation_smart.cur_select_state
    self.formation_smart.pos_change = {
        arrow = 0,
        lights = lights,
    }
end

function CloudArenaData:PetAddBegin()
    local lights = self:PetAddLights()
    if self.standby_index_list[self.formation_smart.pet_index] ~= nil then
        self.formation_smart.cur_select_state = self.formation_smart.cur_select_state == STATE.Empty and STATE.StandBy or self.formation_smart.cur_select_state
    elseif self:PetOnFight(self.formation_smart.pet_index) then
        self.formation_smart.cur_select_state = self.main_pet_index == self.formation_smart.pet_index and STATE.Fight or STATE.Assist
    else
        self.formation_smart.cur_select_state = self.formation_smart.cur_select_state == STATE.Empty and STATE.Relax or self.formation_smart.cur_select_state
    end

    local cur_pos = 0
    if self.pet_on_list[self.formation_smart.pet_index] ~= nil then
        for k,v in pairs(self.formation_info) do
            if v.pet and v.pet_index == self.formation_smart.pet_index then
                cur_pos = k
                break
            end
        end
    end
    if cur_pos == 3 or cur_pos == 8 then
        lights[3] = true
        lights[8] = true
    else
        lights[3] = self.formation_info[3] == nil or not self.formation_info[3].role
        lights[8] = self.formation_info[8] == nil or not self.formation_info[8].role
    end
    self.formation_smart.pos_change = {
        arrow = cur_pos,
        lights = lights,
    }
end

function CloudArenaData:PatnerAddLights()
    local lights = {}
    for i = 1, 10 do
        if 3 ~= i and 8 ~= i then
            local oppo = i > 5 and i - 5 or i + 5
            local info_i = self.formation_info[i]
            lights[i] = (self.formation_info[oppo] == nil or not self.formation_info[oppo].partner) and (info_i == nil or info_i.partner)
        end
    end
    return lights
end

function CloudArenaData:PetAddLights()
    local lights = {}
    -- if self.formation_smart.click_card or self.formation_smart.cur_select_state == STATE.Assist then
        for i = 1, 10 do
            if i ~= 3 and i ~= 8 or self.formation_smart.click_card then
                local oppo = i > 5 and i - 5 or i + 5
                local info_i = self.formation_info[i]
                lights[i] = (self.formation_info[oppo] == nil or not self.formation_info[oppo].pet) and (info_i == nil or info_i.pet)
            end
        end
    -- elseif self.formation_smart.cur_select_state == STATE.Fight then
    --     if self.formation_info[3] == nil or self.formation_info[3].pet then
    --         lights[3] = true
    --     else
    --         lights[8] = true
    --     end
    -- end
    return lights
end

function CloudArenaData:PartnerAddEnd(pos, partner)
    local pos_change = self.formation_smart.pos_change
    if pos_change.lights[pos] then
        self.formation_smart.partner = 0
        if self.formation_info[pos] ~= nil then
            self.partner_id_list[self.formation_info[pos].partner_id] = nil
        end
        self.formation_info[pos] = FormationInfo.PartnerParse(partner, pos)
        self.formation_grid_list[pos] = {id = partner, type = CloudArenaData.FormationType.Partner, unique_id_1 = 0, unique_id_2 = 0}
        self.partner_id_list[partner] = {id = partner, type = CloudArenaData.FormationType.Partner, unique_id_1 = 0, unique_id_2 = 0}

        self.formation_smart:SetDirty("pos_changed")
        self:PosChangeReset()
        self.formation_smart.cur_select_state = STATE.Empty
    else
        PublicPopupCtrl.Instance:Center(Language.Formation.SelNoLightTips)
    end
end

function CloudArenaData:PetAddEnd(pos, pet_index)
    local pos_change = self.formation_smart.pos_change
    if pos_change.lights[pos] then
        self.formation_smart.pet_index = 0
        if self.formation_info[pos] ~= nil then
            self.pet_on_list[self.formation_info[pos].pet_index] = nil
        end
        local pet_info = PetData.Instance:GetPetInfo(pet_index)
        self.formation_info[pos] = FormationInfo.PetParse(pet_index, pos)
        self.formation_grid_list[pos] = {id = pet_info:ID(), type = CloudArenaData.FormationType.Pet, unique_id_1 = pet_info.info.pet_unique_id1, unique_id_2 = pet_info.info.pet_unique_id2}
        self.pet_on_list[pet_index] = {id = pet_info:ID(), type = CloudArenaData.FormationType.Pet, unique_id_1 = pet_info.info.pet_unique_id1, unique_id_2 = pet_info.info.pet_unique_id2}
        self.standby_index_list[pet_index] = nil
        if pos == 3 or pos == 8 then
            self.main_pet_index = pet_index
        end
        self.formation_smart:SetDirty("pos_changed")
        self:PosChangeReset()
        self.formation_smart.cur_select_state = STATE.Empty
    else
        PublicPopupCtrl.Instance:Center(Language.Formation.SelNoLightTips)
    end
end

function CloudArenaData:PartnerChangePos(id)
    local pos = 0
    for k,v in pairs(self.formation_grid_list) do
        if v.type == CloudArenaData.FormationType.Partner and v.id == id then
            self.formation_smart.cur_select_state = STATE.Fight
            pos = k
            break
        end
    end
    if pos ~= 0 then
        self:PosChange(pos)
    end
end

function CloudArenaData:PetChangePos(pet_index)
    local pos = 0
    for k,v in pairs(self.formation_grid_list) do
        if v.type == CloudArenaData.FormationType.Pet then 
            local index = PetData.Instance:GetPetIndexByUniqueId(v.unique_id_1, v.unique_id_2)
            if index == pet_index then
                self.formation_smart.cur_select_state = (k == 3 or k == 8) and STATE.Fight or STATE.Assist
                pos = k
                break
            end
        end
    end
    if pos ~= 0 then
        self:PosChange(pos)
    end
end

function CloudArenaData:PosChange(pos)
    local pos_change = self.formation_smart.pos_change
    if 0 == pos_change.arrow then
        if self.formation_info[pos] then
            local lights = self.formation_info[pos].pet and self:PetPosChangeLights(pos) or self:RoleAndPartnerPosChangeLights(pos)
            self.formation_smart.pos_change = {
                arrow = pos,
                lights = lights,
            }
        else
            if pos == 3 or pos == 8 then
                PublicPopupCtrl.Instance:Center(Language.Formation.SelNoPetTips)
            else
                local oppo = pos > 5 and pos - 5 or pos + 5
                if self.formation_info[oppo] then
                    if self.formation_info[oppo].partner then
                        PublicPopupCtrl.Instance:Center(Language.Formation.SelNoPetTips)
                    elseif self.formation_info[oppo].pet then
                        PublicPopupCtrl.Instance:Center(Language.Formation.SelNoPartnerTips)
                    end
                else
                    PublicPopupCtrl.Instance:Center(Language.Formation.SelNoPetTips)
                end
            end
        end
    elseif pos_change.arrow ~= pos then
        if pos_change.lights[pos] then
            local info = self.formation_info[pos]
            self.formation_info[pos] = self.formation_info[pos_change.arrow]
            self.formation_info[pos_change.arrow] = info
            if self.formation_info[pos] then self.formation_info[pos].pos = pos end
            if self.formation_info[pos_change.arrow] then self.formation_info[pos_change.arrow].pos = pos_change.arrow end

            local grid_info = self.formation_grid_list[pos]
            self.formation_grid_list[pos] = self.formation_grid_list[pos_change.arrow]
            self.formation_grid_list[pos_change.arrow] = grid_info
            if self.formation_info[pos].pet then
                if pos == 3 or pos == 8 then
                    self.main_pet_index = self.formation_info[pos].pet_index
                elseif pos_change.arrow == 3 or pos_change.arrow == 8 then
                    self.main_pet_index = self.formation_info[pos_change.arrow] and self.formation_info[pos_change.arrow].pet and self.formation_info[pos_change.arrow].pet_index or -1
                end
            end
            self.formation_smart:SetDirty("pos_changed")
            self:PosChangeReset()
        else
            PublicPopupCtrl.Instance:Center(Language.Formation.SelNoLightTips)
        end
    else
        self:PosChangeReset()
    end
end

function CloudArenaData:PosChangeReset()
    self.formation_smart.pos_change = {
        arrow = 0,
        lights = {},
    }
    self.formation_smart.partner = 0
    self.formation_smart.pet_index = -1
    self.formation_smart.cur_select_state = -1
    self.formation_smart.click_card = false
end

function CloudArenaData:SetSelectPartner(id)
    self.formation_smart.cur_select_state = self:PartnerOnFight(id) and STATE.Fight or STATE.Relax
    if id == self.formation_smart.partner and id ~= 0 then
        self.formation_smart.partner = 0
        self:PosChangeReset()
    else
        self.formation_smart.partner = id
    end
end

function CloudArenaData:SetSelectPet(index)
    local _, state = self:PetFormationStatus(index)
    self.formation_smart.cur_select_state = state ~= nil and state or STATE.Relax
    if self.formation_smart.pet_index == index then
        self.formation_smart.pet_index = -1
        self:PosChangeReset()
    else
        self.formation_smart.pet_index = index
    end
end

function CloudArenaData:SetSelectState(state)
    if state == STATE.Relax or state == STATE.StandBy then
        if state == STATE.StandBy then
            local count_limit = self.main_pet_index == -1 or self.formation_smart.pet_index == self.main_pet_index
            if self:StandbyCount() >= (count_limit and 5 or 4) then
                PublicPopupCtrl.Instance:Center(string.format(Language.Pet.PetOnFightMax,5))
                self.formation_smart:SetDirty("cur_select_state")
                return
            end
        end
        if self.formation_smart.partner > 0 or self.formation_smart.pet_index then
            for k,v in pairs(self.formation_info) do
                if v.pet and v.pet_index == self.formation_smart.pet_index or v.partner and v.partner_id == self.formation_smart.partner then
                    if v.partner then
                        self.partner_id_list[v.partner_id] = nil
                    elseif v.pet then
                        if k == 3 or k == 8 then
                            self.main_pet_index = -1
                        end
                        self.pet_on_list[v.pet_index] = nil
                    end
                    self.formation_info[k] = nil
                    self.formation_grid_list[k] = CloudArenaData.EmptyFormationGrid
                end
            end
            self.standby_index_list[self.formation_smart.pet_index] = state == STATE.StandBy and PetData.Instance:GetPetInfo(self.formation_smart.pet_index) or nil
        end
        self.formation_smart.pos_change = {
            arrow = 0,
            lights = {},
        }
        -- local need_flush_model = self.formation_smart.cur_select_state ~= Relax
        self.formation_smart.cur_select_state = state
        -- if need_flush_model then
            self.formation_smart:SetDirty("pos_changed")
        -- end
    elseif state == STATE.Assist then
        if self.formation_smart.cur_select_state ~= STATE.Assist then
            --顺序不可调换----------------------------------------
            self.formation_smart.click_card = false             --
            self.formation_smart.cur_select_state = STATE.Assist--
            self:PetAddBegin()                                  --
            ------------------------------------------------------
        end
    elseif state == STATE.Fight then
        if self.formation_smart.cur_select_state ~= STATE.Fight then
            self.formation_smart.click_card = false
            if self.formation_smart.cur_is_pet == 0 then
                --顺序不可调换----------------------------------------
                self:PartnerAddBegin()                              --
                self.formation_smart.cur_select_state = STATE.Fight --
                ------------------------------------------------------
            else
                self.formation_smart.cur_select_state = STATE.Fight
                local pos = 8
                if self.formation_info[3] == nil or self.formation_info[3].pet then
                    pos = 3
                end
                if self:PetOnFight(self.formation_smart.pet_index) then
                    self:PetAddBegin()
                    self:PosChange(pos)
                else
                    self:PetAddBegin()
                    self:PetAddEnd(pos, self.formation_smart.pet_index)
                end
            end
            
        end
    end
end

function CloudArenaData:GetAssistCount()
    local count = 0
    for k,v in pairs(self.formation_info) do
        if k ~= 3 and k ~= 8 then
            if v.pet then
                count = count + 1
            end
        end
    end
    return count
end

function CloudArenaData:GetSeasonShopType()
    return self.season_shop_type or 0
end

--赛季剩余时间
function CloudArenaData:GetTopConditionTip()
	if ActivityData.Instance:IsNoEnoughLevel(ActType.CloudArena,true) or not self.mySelfInfo.season_finish_timestamp then
		return
	end
    local countdown = self.mySelfInfo.season_finish_timestamp - TimeCtrl.Instance:GetServerTime()
    countdown = math.floor(countdown)
    local time_info = TimeHelper.FormatDHMS(countdown)
    return string.format(Language.CloudArena.EndTip, time_info.day, time_info.hour)
end

function CloudArenaData:GetWinAddPoint(target_index)
    if Config.cloud_battlefield_auto.challenge_integral[target_index] then
        return Config.cloud_battlefield_auto.challenge_integral[target_index].win_price
    else
        BuglyReport("GetWinAddPoint index:",target_index,"config is nil")
    end
end