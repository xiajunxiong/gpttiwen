CSColosseumReq = CSColosseumReq or BaseClass(BaseProtocolStruct)
function CSColosseumReq:__init()
    self.msg_type = 2600
end

function CSColosseumReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
end

--个人信息
SCColosseumRoleInfo = SCColosseumRoleInfo or BaseClass(BaseProtocolStruct)
function SCColosseumRoleInfo:__init()
    self.msg_type = 2601
    self.rank = 0
    self.today_fight_count = 0
    self.fetch_exp_flag = {}
end

function SCColosseumRoleInfo:Decode()
    self.is_out_of_rank_list = MsgAdapter.ReadInt()
    self.rank = MsgAdapter.ReadInt()
    self.today_fight_count = MsgAdapter.ReadInt()
    self.fetch_exp_flag = MsgAdapter.IntFlag(1,true)
    self.player_num_in_pool = MsgAdapter.ReadInt()
    self.can_get_coin = MsgAdapter.ReadInt()
    self.get_colosseum_credits = MsgAdapter.ReadInt()
    self.capabolity = MsgAdapter.ReadInt()  -- 综合评分
    self.history_highest_rank = MsgAdapter.ReadInt() --历史最高排名
end

--前4信息
SCColosseumTopInfo = SCColosseumTopInfo or BaseClass(BaseProtocolStruct)
function SCColosseumTopInfo:__init()
    self.msg_type = 2602
    self.top_list = {}
end

function SCColosseumTopInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    for i = 1, self.count do
        self.top_list[i] = self:Top(i)
    end
end
function SCColosseumTopInfo:Top(i)
    local t = {}
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.profession_id = MsgAdapter.ReadInt()
    -- if profession_id < 0 then profession_id = -profession_id end
    t.profession = math.modf(t.profession_id / 100)
    t.avatar_type = MsgAdapter.ReadShort()
    t.level = MsgAdapter.ReadShort()
    t.worship_count = MsgAdapter.ReadInt()
    t.top_level = MsgAdapter.ReadInt()
    t.rank = i
    return t
end
--对手信息
SCColosseumTargetInfo = SCColosseumTargetInfo or BaseClass(BaseProtocolStruct)
function SCColosseumTargetInfo:__init()
    self.msg_type = 2603
    self.target_list = {}
end

function SCColosseumTargetInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    for i = 1, self.count do
        self.target_list[i] = self:Target(i - 1)
    end
end
function SCColosseumTargetInfo:Target(index)
    local t = {}
    t.buff_id_list = {}
    for i = 1, COLOSSEUM_DEFENDER_BUFF_NUM, 1 do
        t.buff_id_list[i] = MsgAdapter.ReadInt()
    end
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.rank = MsgAdapter.ReadShort()
    t.level = MsgAdapter.ReadShort()
    t.appearance = Appearance()
    t.capabolity = MsgAdapter.ReadInt()    -- 综合评分
    t.top_level = MsgAdapter.ReadInt()
    t.is_can_sweep = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()  -- 预留位
    t.index = index
    return t
end

--排行榜信息
SCColosseumRankInfo = SCColosseumRankInfo or BaseClass(BaseProtocolStruct)
function SCColosseumRankInfo:__init()
    self.msg_type = 2604
    self.rank_list = {}
end

function SCColosseumRankInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    for i = 1, self.count do
        self.rank_list[i] = self:Top(i)
    end
end
function SCColosseumRankInfo:Top(i)
    local t = {}
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.profession_id = MsgAdapter.ReadShort()
    -- if profession_id < 0 then profession_id = -profession_id end
    t.profession = math.modf(t.profession_id / 100)
    t.level = MsgAdapter.ReadShort()
    t.top_level = MsgAdapter.ReadInt()
    t.capabolity = MsgAdapter.ReadInt()
    t.uid = MsgAdapter.ReadShort() --新增头像
    t.avatar_type = MsgAdapter.ReadShort() --新增头像
    t.rank = i
    t.rank_value = i
    t.avatar_id = 0
    return t
end

--战报信息
SCColosseumReportInfo = SCColosseumReportInfo or BaseClass(BaseProtocolStruct)
function SCColosseumReportInfo:__init()
    self.msg_type = 2605
    self.report_list = {}
end

function SCColosseumReportInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    for i = 1, self.count do
        local info = {}
        info.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        info.rank = MsgAdapter.ReadInt()
        info.profession = math.modf(MsgAdapter.ReadShort() / 100)
        info.level = MsgAdapter.ReadChar()
        info.is_win = MsgAdapter.ReadChar()
        info.avatar_type = MsgAdapter.ReadShort()
        info.is_attacker = MsgAdapter.ReadShort()
        info.avatar_id = MsgAdapter.ReadShort()--人物头像
        MsgAdapter.ReadShort()
        self.report_list[i] = info
    end
end

SCColosseumRankRewardInfo = SCColosseumRankRewardInfo or BaseClass(BaseProtocolStruct)
function SCColosseumRankRewardInfo:__init()
    self.msg_type = 2606
end

function SCColosseumRankRewardInfo:Decode()
    self.history_highest_rank = MsgAdapter.ReadInt()
    self.rank_reward_fetch_falg = MsgAdapter.IntFlag(2)
end

SCColosseumPvPsResultInfo = SCColosseumPvPsResultInfo or BaseClass(BaseProtocolStruct)
function SCColosseumPvPsResultInfo:__init()
    self.msg_type = 2607
end

function SCColosseumPvPsResultInfo:Decode()
    self.up_rank = MsgAdapter.ReadInt()
    self.new_rank = MsgAdapter.ReadInt()
    self.add_score = MsgAdapter.ReadInt()
    self.score = MsgAdapter.ReadInt()
    self.is_sweep = MsgAdapter.ReadInt()    --是否是扫荡
end

--古神宝库开关
SCColosseumOpenStatusInfo = SCColosseumOpenStatusInfo or BaseClass(BaseProtocolStruct)
function SCColosseumOpenStatusInfo:__init()
    self.msg_type = 2608
end

function SCColosseumOpenStatusInfo:Decode()
    self.open_status = MsgAdapter.ReadInt() --0--关闭 1--开启
    self.next_switch_timestamp = MsgAdapter.ReadLL() --本次结算结束的时间戳
end

SCColosseumInPoolListInfo = SCColosseumInPoolListInfo or BaseClass(BaseProtocolStruct)
function SCColosseumInPoolListInfo:__init()
    self.msg_type = 2609
end
--type类型
--0 整张表
--1 列表尾部新增
function SCColosseumInPoolListInfo:Decode()
    self.type = MsgAdapter.ReadUShort()
    local count = MsgAdapter.ReadUShort()
    self.top_list = {}
    for i = 1, count do
        table.insert(self.top_list, self:Top(i))
    end
end
function SCColosseumInPoolListInfo:Top(i)
    local t = {}
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.profession_id = MsgAdapter.ReadShort()
    t.profession = math.modf(t.profession_id / 100)
    t.level = MsgAdapter.ReadShort()
    t.top_level = MsgAdapter.ReadInt()
    t.rank = i
    return t
end

SCColosseumNewReport = SCColosseumNewReport or BaseClass(BaseProtocolStruct)
function SCColosseumNewReport:__init()
    self.msg_type = 2610
end

function SCColosseumNewReport:Decode()
    
end

SCColosseumEveryDayRewardInfo = SCColosseumEveryDayRewardInfo or BaseClass(BaseProtocolStruct)
function SCColosseumEveryDayRewardInfo:__init()
    self.msg_type = 2611
end

function SCColosseumEveryDayRewardInfo:Decode()
    local count = MsgAdapter.ReadInt()
    self.rank_pos_list = {}
    for i=1, count do
        self.rank_pos_list[i] = MsgAdapter.ReadInt()
    end
end

SCColosseumClientRefreshTargetNotice = SCColosseumClientRefreshTargetNotice or BaseClass(BaseProtocolStruct)
function SCColosseumClientRefreshTargetNotice:__init()
    self.msg_type = 2613
end

function SCColosseumClientRefreshTargetNotice:Decode()
    
end
--竞技场历史最高排名
SCHistoryHighestRankChangeNotice = SCHistoryHighestRankChangeNotice or BaseClass(BaseProtocolStruct)
function SCHistoryHighestRankChangeNotice:__init()
    self.msg_type = 2617
end

function SCHistoryHighestRankChangeNotice:Decode()
    self.cur_rank = MsgAdapter.ReadInt()
    self.change_rank_num = MsgAdapter.ReadInt()
end

--竞技场扫荡结果通知
SCSweepResultNotice = SCSweepResultNotice or BaseClass(BaseProtocolStruct)
function SCSweepResultNotice:__init()
    self.msg_type = 2618
end

function SCSweepResultNotice:Decode()
    self.is_sweep_success = MsgAdapter.ReadInt()
end

SCBattleRewardNotice = SCBattleRewardNotice or BaseClass(BaseProtocolStruct)
function SCBattleRewardNotice:__init()
    self.msg_type = 2619
end
function SCBattleRewardNotice:Decode()
    self.reward_group = MsgAdapter.ReadInt()
end

--==========================首席竞选==========================

--信息请求 req_type 0角色信息 1 比赛信息 2报名 3准备 4排行榜信息
CSChiefElectionReq = CSChiefElectionReq or BaseClass(BaseProtocolStruct)
function CSChiefElectionReq:__init()
    self.msg_type = 2650
end

function CSChiefElectionReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

--战斗信息 是通知回合的变化的
SCChiefElectionNotice = SCChiefElectionNotice or BaseClass(BaseProtocolStruct)
function SCChiefElectionNotice:__init()
    self.msg_type = 2651
end

function SCChiefElectionNotice:Decode()
    self.curr_round = MsgAdapter.ReadInt() --当前回合
    self.round_state = MsgAdapter.ReadInt()--0是回合准备状态 ，1 是开始
    self.end_time = MsgAdapter.ReadUInt() --回合结束时间
    BuglyReport("2651 战斗信息 是通知回合的变化的 ",self.curr_round,self.round_state,self.end_time)

    --客户端log请求
    ChiefCtrl.Instance:SendElectionReq(CHIEFELECTION_REQ.LOG,1,self.round_state)
end

--战斗排行信息 是所有参赛的人员的信息
SCChiefElectionMatchInfo = SCChiefElectionMatchInfo or BaseClass(BaseProtocolStruct)
function SCChiefElectionMatchInfo:__init()
    self.msg_type = 2652
end

function SCChiefElectionMatchInfo:Decode()
    self.curr_round = MsgAdapter.ReadInt() --当前回合
    self.round_state = MsgAdapter.ReadInt()
     --0是回合准备状态 ，1 是开始
    self.end_time = MsgAdapter.ReadUInt() --回合结束时间
    self.is_finish = MsgAdapter.ReadInt()
    self.match_info = {}
    for i = 1, MsgAdapter.ReadInt() do
        local info = {}
        info.prof = math.floor(MsgAdapter.ReadInt() / 100)
        info.role_id = MsgAdapter.ReadInt() --人物id 0 说明无效
        info.level = MsgAdapter.ReadInt()
        info.avatar_type = MsgAdapter.ReadShort()
        info.avatar_id = MsgAdapter.ReadShort()
        info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        info.match_round = MsgAdapter.ReadInt()--到达的回合
        info.state = MsgAdapter.ReadInt() --1 输了 0 赢了 或match_round == curr_round 未分出胜负
        self.match_info[i] = info
    end
    if self.is_finish == 1 then
        self.curr_round = 5
    end
end

--个人报名状态
SCChiefElectionRoleInfo = SCChiefElectionRoleInfo or BaseClass(BaseProtocolStruct)
function SCChiefElectionRoleInfo:__init()
    self.msg_type = 2653
end

function SCChiefElectionRoleInfo:Decode()
    self.is_join = MsgAdapter.ReadInt()
end

--胜出的人
SCChiefElectionTopInfo = SCChiefElectionTopInfo or BaseClass(BaseProtocolStruct)
function SCChiefElectionTopInfo:__init()
    self.msg_type = 2654
end

function SCChiefElectionTopInfo:Decode()
    local prof = RoleData.Instance:GetRoleProf()
    self.winner_id = 0
    self.winner_list = {}
    for i = 1, CAMPAIGN_RINK_MAX_NUM do
        local info = {}
        info.prof = math.floor(MsgAdapter.ReadInt() / 100)
        info.role_id = MsgAdapter.ReadInt()
        self.winner_list[i] = info
        if prof == info.prof then
            self.winner_id = info.role_id
        end
    end
end

--战力排行榜
SCChiefElectionJoinRoleInfo = SCChiefElectionJoinRoleInfo or BaseClass(BaseProtocolStruct)
function SCChiefElectionJoinRoleInfo:__init()
    self.msg_type = 2655
end

function SCChiefElectionJoinRoleInfo:Decode()
    self.match_info = {}
    for i = 1, CAMPAIGN_RINK_MAX_NUM * 2 do
        local info = {}
        info.rink = i
        info.role_id = MsgAdapter.ReadInt() -- 0说明无效
        info.capability = MsgAdapter.ReadInt()
        info.avatar_type = MsgAdapter.ReadShort()
        info.avatar_id = MsgAdapter.ReadShort()--人物头像
        info.level = MsgAdapter.ReadInt()
        info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        self.match_info[i] = info
    end
end

--通知哪个人打完了，有人分出胜负
SCChiefElectionFighterFinishNotice = SCChiefElectionFighterFinishNotice or BaseClass(BaseProtocolStruct)
function SCChiefElectionFighterFinishNotice:__init()
    self.msg_type = 2656
end

function SCChiefElectionFighterFinishNotice:Decode()
    self.role_id = MsgAdapter.ReadInt()
    self.is_win = MsgAdapter.ReadInt()
end

--首席诞生下发
SCChiefElectionChampion = SCChiefElectionChampion or BaseClass(BaseProtocolStruct)
function SCChiefElectionChampion:__init()
    self.msg_type = 2657
end

function SCChiefElectionChampion:Decode()
end

--混战角色信息
SCChiefElectionRoleSeaInfo = SCChiefElectionRoleSeaInfo or BaseClass(BaseProtocolStruct)
function SCChiefElectionRoleSeaInfo:__init()
    self.msg_type = 2658
end

function SCChiefElectionRoleSeaInfo:Decode()
    self.socre = MsgAdapter.ReadInt()
    self.win_times = MsgAdapter.ReadInt()
    self.lose_times = MsgAdapter.ReadInt()
    self.total_times = MsgAdapter.ReadInt()
end

--混战场景人数信息
SCChiefElectionSeaSceneInfo = SCChiefElectionSeaSceneInfo or BaseClass(BaseProtocolStruct)
function SCChiefElectionSeaSceneInfo:__init()
    self.msg_type = 2659
end

function SCChiefElectionSeaSceneInfo:Decode()
    self.role_num = MsgAdapter.ReadInt()
end

--首席活动状态
SCChiefElectionStateInfo = SCChiefElectionStateInfo or BaseClass(BaseProtocolStruct)
function SCChiefElectionStateInfo:__init()
    self.msg_type = 2660
end

function SCChiefElectionStateInfo:Decode()
    self.prof_match = MsgAdapter.ReadInt()
    self.match_state = MsgAdapter.ReadInt()--0混战准备 1混战中 2混战完成 3 16强淘汰赛
    self.next_match_state_time = MsgAdapter.ReadUInt()--切换活动状态时间戳
end

--混战排行榜下发
SCChiefElectionSeaRank = SCChiefElectionSeaRank or BaseClass(BaseProtocolStruct)
function SCChiefElectionSeaRank:__init()
    self.msg_type = 2661
end

function SCChiefElectionSeaRank:Decode()
    self.rank_list = {}
    self.count = MsgAdapter.ReadInt()
    for i=1,self.count do
        local info = {}
        info.rank = i
        info.rank_count = self.count
        info.role_id = MsgAdapter.ReadInt()
        info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        info.socre = MsgAdapter.ReadInt()
        info.win_times = MsgAdapter.ReadInt()
        info.lose_times = MsgAdapter.ReadInt()
        info.total_times = MsgAdapter.ReadInt()
        self.rank_list[i] = info
    end
end

-- 秘境奇闻
SCMiJingQiWenFbObjInfo = SCMiJingQiWenFbObjInfo or BaseClass(BaseProtocolStruct)
function SCMiJingQiWenFbObjInfo:__init()
    self.msg_type = 2620
end

function SCMiJingQiWenFbObjInfo:Decode()
    local t = {}
    t.level_seq = MsgAdapter.ReadInt()
    t.layer = MsgAdapter.ReadInt()
    t.scene_id = MsgAdapter.ReadInt()

    t.box_num = MsgAdapter.ReadInt()
    t.m_box_num_max = MsgAdapter.ReadInt()

    t.monster_num = MsgAdapter.ReadInt()
    t.m_monster_num_max = MsgAdapter.ReadInt()

    t.npc_num = MsgAdapter.ReadInt()
    t.m_npc_num_max = MsgAdapter.ReadInt()

    t.gouhuo_num = MsgAdapter.ReadInt()
    t.m_gouhuo_num_max = MsgAdapter.ReadInt()

    t.coin_num = MsgAdapter.ReadInt()
    t.m_coin_num_max = MsgAdapter.ReadInt()

    t.gather_num = MsgAdapter.ReadInt()
    t.m_gather_num_max = MsgAdapter.ReadInt()
    self.fight_fantasy_info = t
    
    self.obj_count = MsgAdapter.ReadInt()
    self.obj_list = {}
    for i = 1,self.obj_count do 
        local vo = {}
        vo.obj_type = MsgAdapter.ReadInt()
        vo.npc_id = MsgAdapter.ReadInt()
        vo.pos_x = MsgAdapter.ReadShort()
        vo.pos_y = MsgAdapter.ReadShort()
        self.obj_list[i] = vo
    end 
end



--==========================首席巅峰战==========================
--角色信息 比赛信息 下注 p1 seq p2 figher_index 准备
CSChiefPeakBattleReq = CSChiefPeakBattleReq or BaseClass(BaseProtocolStruct)
function CSChiefPeakBattleReq:__init()
    self.msg_type = 2700
end

function CSChiefPeakBattleReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteStrN(self.declara, 128)
end

--战斗信息 是通知回合的变化的
SCChiefPeakBattleNotice = SCChiefPeakBattleNotice or BaseClass(BaseProtocolStruct)
function SCChiefPeakBattleNotice:__init()
    self.msg_type = 2701
end

function SCChiefPeakBattleNotice:Decode()
    self.curr_round = MsgAdapter.ReadInt() --当前回合
    self.round_state = MsgAdapter.ReadInt()
     --0是回合准备状态 ，1 是开始
    self.end_time = MsgAdapter.ReadUInt() --回合结束时间
end

--战斗排行信息 是所有参赛的人员的信息
SCChiefPeakBattleMatchInfo = SCChiefPeakBattleMatchInfo or BaseClass(BaseProtocolStruct)
function SCChiefPeakBattleMatchInfo:__init()
    self.msg_type = 2702
end

function SCChiefPeakBattleMatchInfo:Decode()
    self.curr_round = MsgAdapter.ReadInt() --当前回合
    self.round_state = MsgAdapter.ReadInt()
     --0是回合准备状态 ，1 是开始
    self.end_time = MsgAdapter.ReadUInt() --回合结束时间
    self.is_finish = MsgAdapter.ReadInt()
    self.match_info = {}
    for i = 1, CAMPAIGN_RINK_MAX_NUM do
        local info = {}
        info.figher_index = i - 1
        info.role_id = MsgAdapter.ReadInt() --人物id 0 说明无效
        info.level = MsgAdapter.ReadInt()
        info.avatar_type = MsgAdapter.ReadShort()
        info.avatar_id = MsgAdapter.ReadShort()--人物头像
        info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        info.match_round = MsgAdapter.ReadInt()--到达的回合
        info.state = MsgAdapter.ReadInt() --1 输了 0 赢了 或match_round == curr_round 未分出胜负
        info.prof = ChiefElectionData.Instance:GetMatchProf(info.role_id)
        self.match_info[i] = info
    end
    if self.is_finish == 1 then
        self.curr_round = 4
    end
end

--角色信息返回
SCChiefPeakBattleRoleInfo = SCChiefPeakBattleRoleInfo or BaseClass(BaseProtocolStruct)
function SCChiefPeakBattleRoleInfo:__init()
    self.msg_type = 2703
end

function SCChiefPeakBattleRoleInfo:Decode()
    self.bet_type = MsgAdapter.ReadInt() --下注类型
end

--角色冠军下发
SCChiefPeakBattleInfo = SCChiefPeakBattleInfo or BaseClass(BaseProtocolStruct)
function SCChiefPeakBattleInfo:__init()
    self.msg_type = 2704
end

function SCChiefPeakBattleInfo:Decode()
    self.winner_id = MsgAdapter.ReadInt()
end

--通知哪个人打完了，有人分出胜负
SCChiefPeakBattleFighterFinishNotice = SCChiefPeakBattleFighterFinishNotice or BaseClass(BaseProtocolStruct)
function SCChiefPeakBattleFighterFinishNotice:__init()
    self.msg_type = 2705
end

function SCChiefPeakBattleFighterFinishNotice:Decode()
    self.role_id = MsgAdapter.ReadInt()
    self.is_win = MsgAdapter.ReadInt()
end

--首席诞生下发
SCChiefPeakBattleChampion = SCChiefPeakBattleChampion or BaseClass(BaseProtocolStruct)
function SCChiefPeakBattleChampion:__init()
    self.msg_type = 2706
end

function SCChiefPeakBattleChampion:Decode()
end

-- 竞技场成就下发
SCArenaAchievementInfo = SCArenaAchievementInfo or BaseClass(BaseProtocolStruct)
function SCArenaAchievementInfo:__init()
    self.msg_type = 2614
end

function SCArenaAchievementInfo:Decode()
    self.next_reset_time = MsgAdapter.ReadUInt()
    self.flags = MsgAdapter.IntFlag(8)
end

-- 竞技场成就详细信息
SCArenaDetalInfo = SCArenaDetalInfo or BaseClass(BaseProtocolStruct)
function SCArenaDetalInfo:__init()
    self.msg_type = 2616
end

function SCArenaDetalInfo:Decode()
    self.type_count_info = {}
    for i = 0,21 do
        self.type_count_info[i] = MsgAdapter.ReadInt()
    end
end

-------------------------------------------幽冥炼狱
local function NetherWorldGatoryLevel()
    local vo = {}
    vo.round_num = MsgAdapter.ReadInt()
    vo.pass_map = MsgAdapter.IntFlag() -- 长度4
    vo.reward_map = MsgAdapter.IntFlag() -- 长度4
    return vo
end

local function NetherWorldGatoryChapters()
    local vo = {}
    vo.challenge_info = {}
    for i = 1,20 do 
        vo.challenge_info[i] = NetherWorldGatoryLevel() -- 实际长度19
    end 
    vo.silver_reward_flag = MsgAdapter.IntFlag() -- 实际长度19
    vo.gold_reward_flag = MsgAdapter.IntFlag() -- 实际长度19
    return vo
end


-- 下发
SCNetherWorldPurgatoryInfo = SCNetherWorldPurgatoryInfo or BaseClass(BaseProtocolStruct)
function SCNetherWorldPurgatoryInfo:__init()
    self.msg_type = 2630
end

function SCNetherWorldPurgatoryInfo:Decode()
    self.pass_challenge_id = MsgAdapter.ReadInt() -- 当前通关关卡
    self.pass_challenge_chapters = MsgAdapter.ReadInt() -- 当前通关章节
    self.low_key = MsgAdapter.ReadInt() -- 暂留，无用
    self.mid_key = MsgAdapter.ReadInt() -- 银钥匙
    self.high_key = MsgAdapter.ReadInt() -- 金钥匙
    self.show_video = MsgAdapter.ReadInt() -- 是否完成展示
    self.today_pass_times = MsgAdapter.ReadInt() -- 今日挑战次数

    -- self.pass_challenge_id = MsgAdapter.ReadInt() -- 当前通关
    -- self.set_level = MsgAdapter.ReadInt() -- 当前通关等级
    -- self.low_key = MsgAdapter.ReadInt() -- 暂留，无用
    -- self.mid_key = MsgAdapter.ReadInt() -- 银钥匙
    -- self.high_key = MsgAdapter.ReadInt() -- 金钥匙
    -- self.show_video = MsgAdapter.ReadInt() -- 是否完成展示
    -- self.week_flag = MsgAdapter.ReadInt() -- 是否到了第二周
    -- self.info_list = {}
    -- for i =1 ,MAX_NETHER_WORLD_GATORY_TASK_NUM do -- 
    --     local vo = {}
    --     vo.monster_id = MsgAdapter.ReadInt() -- 怪物组id
    --     vo.round_num = MsgAdapter.ReadInt() -- 回合数
    --     vo.pass_map = MsgAdapter.IntFlag() -- 是否完成任务
    --     vo.reward_map = MsgAdapter.IntFlag() -- 是否领取奖励

    --     table.insert( self.info_list,vo )
    -- end 
end
-- 请求
CSNetherWorldPurgatoryReq = CSNetherWorldPurgatoryReq or BaseClass(BaseProtocolStruct)
function CSNetherWorldPurgatoryReq:__init()
    self.msg_type = 2631
end 

function CSNetherWorldPurgatoryReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type) -- 0 信息下发 1 请求战斗 2 请求奖励 3 请求密令抽奖
    MsgAdapter.WriteInt(self.param1) --req1 对应关卡 req2 对应关卡 req3 2银3金
    MsgAdapter.WriteInt(self.param2) --req2 对应宝箱 req3 抽奖次数
    MsgAdapter.WriteInt(self.param3) --req2 对应宝箱 req3 抽奖次数
end

SCNetherWorldPurgatorySigleInfo = SCNetherWorldPurgatorySigleInfo or BaseClass(BaseProtocolStruct)
function SCNetherWorldPurgatorySigleInfo:__init()
    self.msg_type = 2632
end

function SCNetherWorldPurgatorySigleInfo:Decode()
    self.send_reason = MsgAdapter.ReadShort() -- 下发理由 0 领奖 1 战斗
    MsgAdapter.ReadChar()
    self.new_pass_flag = MsgAdapter.ReadUChar() -- 二进制任务完成
    self.pass_challenge_id = MsgAdapter.ReadInt() -- 覆盖信息
    self.pass_challenge_chapters = MsgAdapter.ReadInt() -- 覆盖章节
    self.today_pass_times = MsgAdapter.ReadInt() -- 今日挑战次数
    self.challeneg_info = NetherWorldGatoryLevel() -- {}
    
    -- self.challeneg_info.monster_id = MsgAdapter.ReadInt() -- 怪物组id
    -- self.challeneg_info.round_num = MsgAdapter.ReadInt() -- 回合数
    -- self.challeneg_info.pass_map =  MsgAdapter.IntFlag() -- 是否完成任务
    -- self.challeneg_info.reward_map = MsgAdapter.IntFlag() -- 是否领取奖励
end

-- 密令下发
SCNetherWorldPurgatoryMiLingInfo = SCNetherWorldPurgatoryMiLingInfo or BaseClass(BaseProtocolStruct)
function SCNetherWorldPurgatoryMiLingInfo:__init()
    self.msg_type = 2633
end

function SCNetherWorldPurgatoryMiLingInfo:Decode()
    self.low_key = MsgAdapter.ReadInt() -- 暂留，无用
    self.mid_key = MsgAdapter.ReadInt() -- 银钥匙
    self.high_key = MsgAdapter.ReadInt() -- 金钥匙
    
    self.silver_reward_flag = {}
    self.gold_reward_flag = {}
    for i = 1,20 do 
        self.silver_reward_flag[i] = MsgAdapter.IntFlag() -- 是否完成任务
        self.gold_reward_flag[i] = MsgAdapter.IntFlag() -- 是否领取奖励
    end 
end

SCNetherWorldPurgatoryChapterInfo = SCNetherWorldPurgatoryChapterInfo or BaseClass(BaseProtocolStruct)
function SCNetherWorldPurgatoryChapterInfo:__init()
    self.msg_type = 2634
end

function SCNetherWorldPurgatoryChapterInfo:Decode()
    self.chapter_id = MsgAdapter.ReadInt() -- 章节
    self.chapter_info = NetherWorldGatoryChapters()
end