
UniverseTeamArenaData = UniverseTeamArenaData or BaseClass()

function UniverseTeamArenaData:__init()
    if UniverseTeamArenaData.Instance ~= nil then
		Debuger.LogError("[UniverseTeamArenaData] attempt to create singleton twice!")
		return
    end
    UniverseTeamArenaData.Instance = self
    self.cfg = Config.universe_arena_auto
    self.other_cfg = Config.universe_arena_auto.other[1]

    -- 赛季信息
    self.base_data = SmartData.New({
        cur_status = UTACfg.ActStatus.Nothing,    --//!< 当前活动状态 1:准备 2:资格赛 3:正赛 4:结算 5:休赛
        round_index = 0,            --//!< 当前资格赛轮数
        season_idx = 0,                 --//!< 赛季号
        is_in_progress = 0,         -- 是否进行中
        next_battle_timestamp = 0,      --//!< 下场战斗的时间戳
    })

    -- 比赛时的活动状态
    self.match_act_info = {
        act_type = ActType.UTArena,
        status = ActStatusType.Close,
    }

    -- 准备进场通知信息 -- 仅有资格的玩家才收的到 {} === SCUTAPrepareEnterSceneNotice.Info
    self.enter_scene_notice_data = SmartData.New({})
    -- 准备场景内的信息 SCUTASceneInfo
    self.ready_scene_data = SmartData.New({})
    -- VS信息
    self.vs_data = SmartData.New({})

    -- 我的队伍基本信息
    self.self_team_base_data = SmartData.New({})
    -- 我的资格赛赛程
    self.self_out_match_list = SmartData.New({})
    -- 全部资格赛赛程
    self.all_out_match_data = SmartData.New({})
    -- 资格赛排名信息
    self.out_rank_data = SmartData.New({})


    -- 晋级赛参赛赛程
    self.all_main_match_data = SmartData.New({})
    
    -- 赛程总览
    self.all_match_preview_data = SmartData.New({})

    -- 最终榜单
    self.end_rank_data = SmartData.New({})

    -- 冠军队伍数据
    self.champion_data = SmartData.New({})
    -- 冠军雕像数据
    self.cur_champion_data = SmartData.New({})
    

    -- 参赛名单数据
    self.team_all_list_data = SmartData.New()

    -- 查询的选手阵容信息
    self.battle_array_data = SmartData.New()

    -- 查询到的某一个比赛的竞猜信息
    self.guess_info = SmartData.New()    -- match_id : 比赛序号  l_bet_info : GuessInfoClass 左边信息 r_bet_info:右边信息

    -- 玩家下注记录列表
    self.guess_bet_data = SmartData.New({record_list = {}})

    -- 竞猜累计收益
    self.guedd_profit_data = SmartData.New({num = 0})

    -- 红点数据
    self.remind_data = SmartData.New({can_guess_num = 0})

    -- 请求竞猜的比赛，做个记录
    self.req_guess_match = nil

    -- 是否跳到对应的比赛
    self.is_jump_match = false

    -- 是否是查看上一届赛况总览
    self.is_look_last_all_match = false

end

function UniverseTeamArenaData:__delete()
end

function UniverseTeamArenaData:ResetData()
    self:TeamAllListData({})
    self:ClearBetRecord()
    self.enter_scene_notice_data:Set({})
    self.ready_scene_data:Set({})
    self.self_team_base_data:Set({})
    self.remind_data:Set({can_guess_num = 0})
end

function UniverseTeamArenaData:BaseData(base_data)
    if base_data then
        self.base_data:Set(base_data)
        if self.base_data.cur_status == UTACfg.ActStatus.Ready then
            self:ResetData()
        end
    end
    return self.base_data
end

function UniverseTeamArenaData:OtherCfg()
    return self.cfg.other[1]
end

-- 活动是否开启
function UniverseTeamArenaData:IsOpen()
    -- 开启的那天到结束
    -- local start_time, end_time = self:GetStartAndEndTime()
    -- end_time = end_time + 86400 * 3
    -- local server_time = TimeCtrl.Instance:GetServerTime()
    -- if server_time >= start_time and server_time <= end_time then
    --     return true
    -- end
    -- return false
    
    local level = RoleData.Instance:GetRoleLevel() --RoleData.Instance:GetWorldLevel()
    if level < self.other_cfg.level then
        return false
    end
    if self.base_data.season_idx == 0 then
        return false
    end
    if self.base_data.cur_status and self.base_data.cur_status ~= UTACfg.ActStatus.Nothing and self.base_data.cur_status ~= UTACfg.ActStatus.Ready then

        --//赛季结束时间，当前休赛结算且超过改是时间段后就不显示图标
        if self.base_data.cur_status == UTACfg.ActStatus.Pause and self.base_data.end_timestamp then
            local cur_time = TimeCtrl.Instance:GetServerTime()
            if cur_time >= self.base_data.end_timestamp then
                return false
            end
        end
        return true
    end
    return false
end

-- ActType.UTArena2这个活动是否开启
function UniverseTeamArenaData:UTArena2IsOpen()
    if self.base_data.season_idx == 0 then
        return false
    end
    local status = self:ActState()
	if status == UTACfg.ActStatus.Nothing or status == UTACfg.ActStatus.Pause or status == UTACfg.ActStatus.End or status == UTACfg.ActStatus.Ready then
		return false
    end
    return true
end

-- 活动状态
function UniverseTeamArenaData:ActState()
    return self.base_data.cur_status or UTACfg.ActStatus.Nothing
end

-- 赛季号
function UniverseTeamArenaData:SeasonIndex()
    local season_idx = self.base_data.season_idx or 0
    return season_idx
end

-- 是否参数人数不足或者打跨服没开，或者渠道不让开
function UniverseTeamArenaData:IsNeedClose()
    local state = self:ActState()
    local season_idx = self:SeasonIndex()
    if state == UTACfg.ActStatus.Nothing or season_idx == 0 then
        return true
    end
    return false
end

function UniverseTeamArenaData:SetMatchActInfo(protocol)
    local info = protocol.info
    local end_time = info.stop_enter_scene_time
    local cur_time = TimeCtrl.Instance:GetServerTime()
    if cur_time >= info.start_prepare_time and cur_time < info.start_enter_scene_time then
        self.match_act_info.status = ActStatusType.Standy
    elseif cur_time >= info.start_enter_scene_time and cur_time < info.stop_enter_scene_time then
        self.match_act_info.status = ActStatusType.Open
    else
        self.match_act_info.status = ActStatusType.Close
    end
end

-- 比赛状态信息
function UniverseTeamArenaData:GetMatchActInfo()
    return self.match_act_info
end

-- 比赛战斗是否进行中
function UniverseTeamArenaData:IsInProgress()
    return self.base_data.is_in_progress == 1
end

-- 活动入口NpcID
function UniverseTeamArenaData:MainNpcId()
    return self.cfg.other[1].npc_id
end

-- 冠军称号
function UniverseTeamArenaData:TitleId()
    return self.other_cfg.title_of_champion
end
-- 冠军坐骑
function UniverseTeamArenaData:RideId()
    return 1041
end

-- 是否开放晋级赛标签页
function UniverseTeamArenaData:IsSecond()
    return self:IsMatchOpen(UTACfg.MatchType.MainFail)
end

-- 获取服务器名称
function UniverseTeamArenaData:GetServerName(server_id, plat_id)
    local server_info = LoginData.Instance:GetServerInfoById(server_id, plat_id) or {}
    return server_info.name or ""
end

-- 获取月初第一周最开始的时间，不会跨到上一月
function UniverseTeamArenaData:GetMonthStartTime(server_time)
    server_time = server_time or TimeCtrl.Instance:GetServerTime()
    -- 月初1号时间
    local month_time_0 = TimeManager.MonthStart(server_time) 
    -- 月初那一刻周几
    local week_num = tonumber(os.date("%w", month_time_0)) or 0
    week_num = week_num == 0 and 7 or week_num
    -- 第一周最开始的时间
    local start_time = month_time_0 - (week_num - 1) * 86400

    -- 加这个行代码就不跨上月，不加就跨上月
    start_time = start_time + 86400 * 7
    return start_time
end

-- 获取整个比赛周期时间 第一轮淘汰赛那一天 到 决赛那一天
function UniverseTeamArenaData:GetStartAndEndTime()
    local month_start_time = self:GetMonthStartTime()
    local start_time = month_start_time + 86400 * 11 + 1
    local end_time = month_start_time + 86400 * 21 - 1
    return start_time, end_time
end

-- 竞猜开启时间戳 也就是活动图标开启那天
function UniverseTeamArenaData:GuessOpenTime(server_time)
    local month_start_time = self:GetMonthStartTime(server_time)
    local start_time = month_start_time + 86400 * 9 + 1
    return start_time
end

-- 获取排名奖励 _type 0淘汰赛, 1晋级赛
function UniverseTeamArenaData:GetRankReward(_type, rank)
    if rank == nil or rank < 1 then
        return
    end
    for i,v in pairs(self.cfg.rank_reward) do
        if _type == v.type and rank >= v.min_rank and rank <= v.max_rank then
            local reward = {}
            if v.title_item > 0 then
                local item = Item.Create({item_id = v.title_item})
                table.insert(reward, item)
            end
            reward = AndList(reward, DataHelper.FormatItemList(v.show))
            -- local data = {}
            -- data.reward = reward
            -- data.cfg = v
            return reward
        end
    end
    return {}
end

-- 比赛是否开启 match_type == UTACfg.MatchType.
function UniverseTeamArenaData:IsMatchOpen(match_type, round_index)
    round_index = round_index or 0
    local state = self:ActState()
    if state == UTACfg.ActStatus.Pause or state == UTACfg.ActStatus.End then
        return true
    end
    local cur_roung_index = self.base_data.round_index
    if match_type == UTACfg.MatchType.Out then
        if state >= UTACfg.ActStatus.Knockout then
            return cur_roung_index >= round_index
        end
    elseif match_type == UTACfg.MatchType.MainFail or match_type == UTACfg.MatchType.MainWin then
        if state >= UTACfg.ActStatus.Second then
            return cur_roung_index >= round_index
        end
    end
    return false
end

-- ======================== 竞猜数据 ======================

--竞猜币道具id
function UniverseTeamArenaData:GetGuessItemId()
    return self.cfg.other[1].lottery_id
end

-- 获取玩家竞猜币
function UniverseTeamArenaData:GetGuessCoinNum()
    local item_id = self:GetGuessItemId()
    return Item.GetNum(item_id)
end

-- 计算赔率
function UniverseTeamArenaData:CalculateGuessRate(num)
    num = num or 0
    local other_cfg = self:OtherCfg()
    local start_num = other_cfg.min_odds
    local add_num = math.floor(num / other_cfg.exchange_rate) * 1
    return (start_num + add_num) / 100
end


-- 设置查询到的竞猜数据
function UniverseTeamArenaData:SetGuessInfo(info)
    self.guess_info:Set(info)
end
function UniverseTeamArenaData:GetGuessInfo()
    return self.guess_info
end

-- 添加下注记录 ** 会覆盖旧的
function UniverseTeamArenaData:SetBetRecord(bet_record)
    local match_id = bet_record.match_id
    local is_has = false
    for i,v in pairs(self.guess_bet_data.record_list or {}) do
        if v.match_id == match_id then
            self.guess_bet_data.record_list[i] = bet_record
            is_has = true
        end
    end
    if not is_has then
        if not self.guess_bet_data.record_list then
            self.guess_bet_data.record_list = {}
        end
        table.insert(self.guess_bet_data.record_list, bet_record)
    end
end
function UniverseTeamArenaData:GetBetRecord(match_id)
    if match_id then
        for i,v in pairs(self.guess_bet_data.record_list or {}) do
            if v.match_id == match_id then
               return v
            end
        end
        return nil
    end
    table.sort(self.guess_bet_data.record_list,function (a, b)
        return a.round_index > b.round_index
    end)
    for i,v in pairs(self.guess_bet_data.record_list) do
        v.index = i
    end
    return self.guess_bet_data.record_list
end
function UniverseTeamArenaData:ClearBetRecord()
    self.guess_bet_data.record_list = {}
end

-- 获取比赛下注记录列表
function UniverseTeamArenaData:GetMatchBetRecordList()
    return self:GetBetRecord() or {}
end

-- ====================================================================
-- ====================================================================


-- ======================== 参赛名单标签页数据 ======================
-- 参赛名单数据 如没有需手动请求
function UniverseTeamArenaData:TeamAllListData(value_data)
    if value_data then
        self.team_all_list_data:Set(value_data)
    end
    return self.team_all_list_data
end

-- 查询的选手阵容信息 如没有需手动请求
function UniverseTeamArenaData:BattleArrayData(value_data)
    if value_data then
        self.battle_array_data:Set(value_data)
    end
    return self.battle_array_data 
end
-- ====================================================================
-- ====================================================================



-- ======================== 淘汰赛标签页数据 ======================

-- 资格淘汰赛——每个自然月第三周周五开始，共五天，每晚20:00 - 21:00，
function UniverseTeamArenaData:GetKnockoutTime(knockout)
    local start_time = self:GetMonthStartTime()
    local add_time1 = 0
    local add_time2 = 0
    local add_time3 = 0
    local add_time4 = 0
    if knockout == 0 then
        add_time1 = 86400 * 11 + 3600 * 20
        add_time2 = add_time1 + 3600 * 0.5
        add_time3 = 86400 * 11 + 3600 * 20.5
        add_time4 = add_time3 + 3600 * 0.5
    elseif knockout == 1 then
        add_time1 = 86400 * 12 + 3600 * 20
        add_time2 = add_time1 + 3600 * 0.5
        add_time3 = 86400 * 12 + 3600 * 20.5
        add_time4 = add_time3 + 3600 * 0.5
    elseif knockout == 2 then
        add_time1 = 86400 * 13 + 3600 * 20
        add_time2 = add_time1 + 3600 * 0.5
        add_time3 = 86400 * 13 + 3600 * 20.5
        add_time4 = add_time3 + 3600 * 0.5
    elseif knockout == 3 then
        add_time1 = 86400 * 14 + 3600 * 20
        add_time2 = add_time1 + 3600 * 0.5
        add_time3 = 86400 * 14 + 3600 * 20.5
        add_time4 = add_time3 + 3600 * 0.5
    elseif knockout == 4 then
        add_time1 = 86400 * 15 + 3600 * 20
        add_time2 = add_time1 + 3600 * 0.5
        add_time3 = 86400 * 15 + 3600 * 20.5
        add_time4 = add_time3 + 3600 * 0.5
    end
    local time_info1 = {
        start_time = start_time + add_time1,
        end_time = start_time + add_time2,
    }
    local time_info2 = {
        start_time = start_time + add_time3,
        end_time = start_time + add_time4,
    }
    return time_info1,time_info2
end

-- 我的队伍基本信息
function UniverseTeamArenaData:SelfTeamBaseInfo()
    if TableIsEmpty(self.self_team_base_data) then
        return nil
    end
    return self.self_team_base_data
end

-- 我的队伍名 没有则为空
function UniverseTeamArenaData:GetSelfTeamName()
    local self_info = self:SelfTeamBaseInfo()
    if self_info then
        return self_info.team_name
    end
end

-- 我的淘汰赛赛程
function UniverseTeamArenaData:SelfOutMatchList()
    return self.self_out_match_list
end

-- 淘汰赛赛程宝箱标记 1可领取 0不可领取，-1已领取
function UniverseTeamArenaData:GetOutBoxFlag(data)
    if self.self_team_base_data.win_box_fetch_flag and self.self_team_base_data.win_box_fetch_flag[data.sn] == 1 then
        return -1
    end
    local cur_win_count = self.self_team_base_data.win_num or 0
    if cur_win_count >= data.victory_times then
        return 1
    else
        return 0
    end
end

-- 获取淘汰赛排名奖励列表
function UniverseTeamArenaData:GetKnockoutRewardList()
    if not self.knockout_reward_cfg then
        self.knockout_reward_cfg = {}
        for i,v in pairs(self.cfg.rank_reward) do
            if v.type == 0 then
                table.insert(self.knockout_reward_cfg, v)
            end
        end
    end
    return self.knockout_reward_cfg
end

-- 设置淘汰赛赛程
function UniverseTeamArenaData:SetKnockoutAllMatchList(protocol)
    local info = protocol.info
    local round_index = info.round_idx
    local match_list = {}
    for i,v in pairs(info.match_list) do
        local t = {}
        v.atk_team = UTAMatchTeamClass.New(v.atk_team_info)
        v.def_team = UTAMatchTeamClass.New(v.def_team_info)
        local match_obj = UTAMatchClass.New(v)
        table.insert(match_list, match_obj)
    end
    self.all_out_match_data[round_index] = match_list
end

-- 获取淘汰赛全服战队赛程   round_index :::: 第几轮
function UniverseTeamArenaData:GetKnockoutAllMatchList(round_index)
    return self.all_out_match_data[round_index] or {}
end

-- 资格赛排名信息
function UniverseTeamArenaData:SetOutRankData(protocol)
    local info = protocol.info
    self.out_rank_data:Set(info)
end

function UniverseTeamArenaData:GetOutRankList()
    return self.out_rank_data.rank_list or {}
end

-- 是否晋级
function UniverseTeamArenaData:IsPass(rank)
    return rank >= 1 and rank <= 32
end

-- 获取淘汰赛胜利积分 0:未赛  1:胜  2:负  3:弃权
function UniverseTeamArenaData:GetOutWinScore(result)
    if result == 1 then
        return self.other_cfg.win_score
    elseif result == 2 then
        return self.other_cfg.lose_score
    elseif result == 3 then
        return self.other_cfg.waiver_score
    elseif result == 4 then
        return self.other_cfg.draw_score
    end
    return 0
end

-- ====================================================================
-- ====================================================================




-- ======================== 晋级赛标签页数据 ======================


-- 往淘汰赛后面接着 因为时间逻辑改版过多次，数值不好定，写死了
function UniverseTeamArenaData:GetSecondTime(second)
    local start_time = self:GetMonthStartTime()
    local add_time1 = 0
    local add_time2 = 0
    if second == UTACfg.MAIN_ROUND_TYPE.TOP32 then
        add_time1 = 86400 * 16 + 3600 * 20
        add_time2 = add_time1 + 3600 * 1.5
    elseif second == UTACfg.MAIN_ROUND_TYPE.TOP16 then
        add_time1 = 86400 * 17 + 3600 * 20
        add_time2 = add_time1 + 3600 * 1.5
    elseif second == UTACfg.MAIN_ROUND_TYPE.TOP8 then
        add_time1 = 86400 * 18 + 3600 * 20
        add_time2 = add_time1 + 3600 * 1.5
    elseif second == UTACfg.MAIN_ROUND_TYPE.TOP4 then
        add_time1 = 86400 * 19 + 3600 * 20
        add_time2 = add_time1 + 3600 * 1.5
    elseif second == UTACfg.MAIN_ROUND_TYPE.TOP2 then
        add_time1 = 86400 * 20 + 3600 * 20
        add_time2 = add_time1 + 3600 * 1.5
    end
    local time = start_time + add_time1
    local end_time = start_time + add_time2
    return time,end_time
end

-- 设置晋级赛赛程
function UniverseTeamArenaData:SetMainAllMatchList(protocol)
    local info = protocol.info
    local round_index = info.round_idx
    local match_list = {}
    for i,v in pairs(info.match_list) do
        local t = {}
        v.atk_team = UTAMatchTeamClass.New(v.atk_team_info)
        v.def_team = UTAMatchTeamClass.New(v.def_team_info)
        local match_obj = UTAMatchClass.New(v)
        table.insert(match_list, match_obj)
    end
    self.all_main_match_data[round_index] = match_list
end

-- 获取晋级赛赛程
function UniverseTeamArenaData:GetMainAllMatchList(round_index)
    return self.all_main_match_data[round_index] or {}
end

-- ====================================================================
-- ====================================================================




-- ========================== 其它 ==========================================
-- 设置赛况总览
function UniverseTeamArenaData:SetPreviewAllMatchList(protocol)
    local info = protocol.info
    local overview_type = info.overview_type
    self.all_match_preview_data[overview_type] = info.match_list
end

-- 获取晋级赛赛程
function UniverseTeamArenaData:GetPreviewAllMatchList(overview_type)
    return self.all_match_preview_data[overview_type] or {}
end

-- 设置最终榜单数据
function UniverseTeamArenaData:SetEndRankData(protocol)
    self.end_rank_data.rank_list = protocol.final_rank_list
end

function UniverseTeamArenaData:GetEndRankData()
    return self.end_rank_data.rank_list or {}
end

-- 获取淘汰赛排名奖励列表
function UniverseTeamArenaData:GetRankRewardList(_type)
    _type = _type or 0
    self.out_rank_reward_cfg = {}
    for i,v in pairs(self.cfg.rank_reward) do
        if v.type == _type then
            local data = {}
            if v.min_rank == v.max_rank then
                data.text = string.format(Language.UTAPanel.Ctrl.Text10, v.min_rank)
            else
                data.text = string.format(Language.UTAPanel.Ctrl.Text11, v.min_rank, v.max_rank)
            end
            data.r_text = data.text
            data.reward_list = DataHelper.FormatItemList(v.show)
            table.insert(self.out_rank_reward_cfg, data)
        end
    end
    for i,v in pairs(self.out_rank_reward_cfg) do
        if i == 1 then
            v.text = ColorStr(v.text, COLORSTR.Yellow6)
        elseif i == 2 then
            v.text = ColorStr(v.text, COLORSTR.Purple4)
        elseif i == 3 then
            v.text = ColorStr(v.text, COLORSTR.Blue9)
        end
    end
    return self.out_rank_reward_cfg
end

-- 获取晋级赛奖励列表
function UniverseTeamArenaData:GetMainRewardList()
    if not self.main_rank_reward_cfg then
        self.main_rank_reward_cfg = {}
        for i,v in pairs(self.cfg.promotion_reward) do
            if v.type == 0 then
                local data = {}
                data.r_text = v.stage_name
                data.text = v.stage_name
                data.reward_list = DataHelper.FormatItemList(v.show)
                data.order = v.order
                table.insert(self.main_rank_reward_cfg, data)
            end
        end
        table.sort(self.main_rank_reward_cfg, function (a,b)
            return a.order < b.order
        end)
        for i,v in pairs(self.main_rank_reward_cfg) do
            if i == 1 then
                v.text = ColorStr(v.text, COLORSTR.Yellow6)
            elseif i == 2 then
                v.text = ColorStr(v.text, COLORSTR.Purple4)
            elseif i == 3 then
                v.text = ColorStr(v.text, COLORSTR.Blue9)
            end
        end
    end
    return self.main_rank_reward_cfg
end

-- 设置冠军队伍数据
function UniverseTeamArenaData:SetChampionData(protocol)
    self.champion_data:Set(protocol.info_list)
    -- 设置当前冠军队伍数据
    if #protocol.info_list > 0 then
        local end_data = protocol.info_list[#protocol.info_list] or {}
        self:SetCurChampionData(end_data)
    end
end

function UniverseTeamArenaData:GetChampionData(index)
    if not index then
        return self.champion_data
    end
    for i,v in pairs(self.champion_data) do
        if v.index == index then
            return v
        end
    end
end

-- 设置当前冠军队伍数据
function UniverseTeamArenaData:SetCurChampionData(data)
    self.cur_champion_data:Set(data or {})
end

function UniverseTeamArenaData:GetCurChampionData()
    return self.cur_champion_data
end


-- 是否提醒海报
function UniverseTeamArenaData:IsShowPoster()
    local pref_keys = PrefKeys.UniverseTeamArenaPoster()
end

-- 海报提醒标记
function UniverseTeamArenaData:GetPosterPrefFlag()
    local pref_keys = PrefKeys.UniverseTeamArenaPoster()
    local timestamp = UnityPlayerPrefs.GetInt(pref_keys)
    local server_time = TimeCtrl.Instance:GetServerTime()
    if timestamp and timestamp > server_time then
        return true
    end
    return false
end

-- 记录海报提醒标记 有效时间为当周
function UniverseTeamArenaData:SetPosterPrefFlag(flag)
    local pref_keys = PrefKeys.UniverseTeamArenaPoster()
    if flag then
        local timestamp = TimeManager.WeekEndAny()
        UnityPlayerPrefs.SetInt(pref_keys, timestamp)
    else
        local timestamp = TimeManager.DayEnd()
        UnityPlayerPrefs.SetInt(pref_keys, timestamp)
    end
end

-- 获取赛程名字
function UniverseTeamArenaData:FormatMatchName(match_type, round_index)
    local match_name = Language.UTA.MatchTypeName[match_type] .. "·"
    if match_type == UTACfg.MatchType.Out then
        match_name = match_name .. (Language.UTA.OutRoundName[round_index] or "异常") 
    elseif match_type == UTACfg.MatchType.MainWin then
        match_name = match_name .. (Language.UTA.WinRoundName[round_index] or "异常") 
    elseif match_type == UTACfg.MatchType.MainFail then
        match_name = match_name .. (Language.UTA.FailRoundName[round_index] or "异常") 
    end
    return match_name
end

-- 淘汰赛转轮数 round_index 0 - 9
function UniverseTeamArenaData:ConvertRoundIndex(round_index)
    return math.floor(round_index / 2)
end

-- 显示那一届的雕像
function UniverseTeamArenaData:GetShowObjIndex()
    local state = self:ActState()
    local index
    if state == UTACfg.ActStatus.Pause then
        index = self.base_data.season_idx
    elseif self.base_data.season_idx > 1 then
        index = self.base_data.season_idx - 1
    end
    return index
end

function UniverseTeamArenaData:GetTopConditionTip()
    -- if ActivityData.Instance:IsNoEnoughLevel(ActType.UTArena,true) then
	-- 	return
    -- end
    -- local season_idx = self.base_data.season_idx or 0
    -- local str = ColorStr(string.format(Language.UTAPanel.Ctrl.Text12, DataHelper.GetDaXie(season_idx)), COLORSTR.Yellow6)
    -- local state = self:ActState()
    -- if state == UTACfg.ActStatus.Pause then
    --     str = str .. Language.UTAPanel.Ctrl.Text13
    -- elseif state == UTACfg.ActStatus.Ready then
    --     str = str .. Language.UTAPanel.Ctrl.Text14
    -- elseif self.base_data.is_in_progress == 1 then
    --     str = str .. Language.UTAPanel.Ctrl.Text15
    -- else
    --     str = str .. Language.UTAPanel.Ctrl.Text16
    -- end

    if self:ActState() == UTACfg.ActStatus.Nothing then
        return ""
    end

    local season_idx = self.base_data.season_idx or 0

    if self:UTArena2IsOpen() then
        local str = ColorStr(string.format(Language.UTAPanel.Ctrl.Text12, DataHelper.GetDaXie(season_idx)), COLORSTR.Yellow6)
        str = str .. Language.UTAPanel.Text24
        return str
    end

    local str = ColorStr(string.format(Language.UTAPanel.Ctrl.Text12, DataHelper.GetDaXie(season_idx + 1)), COLORSTR.Yellow6)
    -- 现在的月份
    local server_time = TimeCtrl.Instance:GetServerTime()
    local time_info = TimeManager.FormatUnixTime2Date(server_time)
    local cur_year = time_info.year
    local cur_month = time_info.month
    local s_time,end_time = self:GetStartAndEndTime()

    -- 下一月的时间戳
    if server_time > end_time then
        if cur_month == 12 then
            cur_year = cur_year + 1
            cur_month = 1
        else
            cur_month = cur_month + 1
        end
    end
    local t = {
        year = cur_year,
        month = cur_month,
        day = 1,
        hour = 1,
        minute = 0,
        second = 0,
    }

    local timestamp = os.time(t)
    local start_time = self:GuessOpenTime(timestamp)
    time_info = TimeManager.FormatUnixTime2Date(start_time)
    str = str .. string.format(Language.UTAPanel.Ctrl.Text17,time_info.month,time_info.day)
    return str
end






-- 创建雕像的逻辑
function UniverseTeamArenaData:CreateChampionObj()
	local scene_id = SceneData.Instance:SceneId()
	if scene_id ~= SceneLogic.Instance:MainCityID() then
        return
	end
	local champion_info = self:GetCurChampionData()
	if TableIsEmpty(champion_info) then
		return
    end
    UniverseTeamArenaCtrl.Log("执行了创建雕像的逻辑")

	for _, value in pairs(self.handle_runners or {}) do
        Runner.Instance:RemoveRunUntilTrue(value)
    end
    self.handle_runners = {}

    for _, value in pairs(self.state_objs or {}) do
        if value.vo then
            local obj_id = value.vo.obj_id
            SceneCtrl.Instance:Objs():DeleteClientObj(obj_id)
        end
    end
    self.state_objs = {}

	for i, info in pairs(champion_info.team_info.member_list) do
		if info.uid ~= 0 then
			self.handle_runners[#self.handle_runners+1] = Runner.Instance:RunUntilTrue(
				function()
					local role_appearance = UniverseTeamArenaCtrl.Instance:ChampionFormatAppearance(info)
					if not TableIsEmpty(role_appearance) then
						local pos_data = UTACfg.StatuePos[i]
						-- role_appearance.ride_id = self:RideId()
						-- role_appearance.bicycle_index = -1
                        -- role_appearance.model_card_flag = 0
                        local obj_id = CUSTOM_OBJID.UTA_STATUE + #self.state_objs
						local state_obj = SceneObj.CreateObj(
							SmartData.New(
								{
									obj_id = obj_id,
									dir_y = pos_data.dir_y,
									scene_id = scene_id,
									title_id = self:TitleId(),--string.format("<color=#%s>%s</color>", COLORSTR.Yellow1, "天下第一比武大会冠军"),
									name = info.role_name,
									obj_type = SceneObjType.UTAStatue,
									appearance = role_appearance,
									ignor_follow = true,
                                    --res_size = --pos_data.mod_size,
                                    role_info = info,
								}
							)
                        )
                        table.insert(self.state_objs,state_obj)
                        SceneCtrl.Instance:Objs().client_objs[obj_id] = state_obj
						state_obj:ResetPos(Vector3.New(pos_data.x, pos_data.y, pos_data.z))
						self.handle_runners[#self.handle_runners+1] =
							Runner.Instance:RunUntilTrue(
							function()
                                local body = state_obj.body
                                if body == nil then
                                    return true
                                end
								if not body:IsLoading() then
                                    --body.main_body:SetScale(pos_data.mod_size)
                                    state_obj.root_node.transform.localScale = Vector3(pos_data.mod_size,pos_data.mod_size,pos_data.mod_size)
									body.main_anim:SetAnimActive(true)
									body.main_body:SampleAnim("d_idle", 0.1)
                                    --body.main_anim:SetAnimActive(false)
                                    
									-- body:ForeachBodyObject(
                                    --     function(bo)
                                    --         if ChiefCtrl.statue_mat == nil then
                                    --             LogError("UniverseTeamArenaStatueMat === nil")
                                    --             return
                                    --         end
									-- 		bo:RemoveRendererMaterial(Game.RendererMaterialLayer.Base:ToInt())
									-- 		bo:SetRendererMaterial(ChiefCtrl.statue_mat, 1, {"_MainTex"})
									-- 	end
									-- )
									return true
								end
							end,
							true
						)
					end
				return true
			end)
		end
	end
end


-- 红点提醒
function UniverseTeamArenaData:RemindNum()
    local can_get_num = self.remind_data.can_guess_num or 0
    return can_get_num > 0 and 1 or 0
end

-- 淘汰赛红点
function UniverseTeamArenaData:OutRemind()
    local state = self:ActState()
    if state ~= UTACfg.ActStatus.Knockout then
        return 0
    end
    local can_get_num = self.remind_data.can_guess_num or 0
    return can_get_num > 0 and 1 or 0
end

-- 晋级赛红点
function UniverseTeamArenaData:MainRemind()
    local state = self:ActState()
    if state ~= UTACfg.ActStatus.Second then
        return 0
    end
    local can_get_num = self.remind_data.can_guess_num or 0
    return can_get_num > 0 and 1 or 0
end