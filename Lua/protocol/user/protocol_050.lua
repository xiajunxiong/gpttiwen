-- ================== 天下第一比武大会 ===========================

--读取ID
local uta_read_id_ll_func = function ()
    local id1 = MsgAdapter.ReadInt()
    local id2 = MsgAdapter.ReadInt()
    local id = id1 .. id2
    local id_t = {
        [1] = id1,
        [2] = id2,
    }
    return id,id_t
end

--发送id
local uta_send_id_ll_func = function (id_t)
    MsgAdapter.WriteInt(id_t[1])
    MsgAdapter.WriteInt(id_t[2])
end

-- 解析队伍成员信息
local decode_uta_member_info_func = function ()
    local info = {}
    info.plat_type = MsgAdapter.ReadInt()       --//!< 渠道号
    info.role_id = MsgAdapter.ReadInt()         --//!< 角色ID
    info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)   --//!< 玩家名
    info.level = MsgAdapter.ReadShort()         --//!< 等级
    info.headshot_id = MsgAdapter.ReadShort()   --//!< 头像id
    info.prof_base_type = MsgAdapter.ReadShort()--//!< 职业基础类型[1, 9]
    info.avatar_type = MsgAdapter.ReadShort()   --//!< 模型类型 [1, 6]
    info.rank_prof = MsgAdapter.ReadInt()       --//!< 职业排名
    info.rank_capability = MsgAdapter.ReadInt() --//!< 评分排名
    info.rank_level = MsgAdapter.ReadInt()      --//!< 等级排名
    return info
end

-- 解析队伍信息
local decode_uta_team_info_func = function ()
    local info = {}
    local team_id,id_t = uta_read_id_ll_func()
    info.team_id = team_id                  -- 队伍id
    info.team_id_t = id_t
    info.team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    info.plat_type = MsgAdapter.ReadInt()               --//!< 渠道号
    info.server_id = MsgAdapter.ReadInt()               -- 队长的服务器号
    info.hero_team_arena_score = MsgAdapter.ReadInt()   -- 英雄会武积分
    info.qualification_score = MsgAdapter.ReadInt()     -- 资格赛积分
    info.record_type = MsgAdapter.ReadShort()           -- 当前战绩类型 (0:淘汰赛，1:正赛)
    info.record_value = MsgAdapter.ReadShort()          -- 淘汰赛第几名，或正赛xx强
    return info
end

-- 解析积分排行的队伍信息
local decode_uta_team_score_rank_info_func = function ()
    local info = {}
    local team_id,id_t = uta_read_id_ll_func()
    info.team_id = team_id                  -- 队伍id
    info.team_id_t = id_t
    info.team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    info.plat_type = MsgAdapter.ReadInt()               --//!< 渠道号
    info.server_id = MsgAdapter.ReadInt()               -- 队长的服务器号
    info.qualification_score = MsgAdapter.ReadShort()       -- 资格赛积分
    info.win_times = MsgAdapter.ReadShort()             -- 胜利次数
    info.battle_times = MsgAdapter.ReadShort()          -- 战斗次数
    info.is_enter_main_phase = MsgAdapter.ReadShort()   -- 是否进入晋级赛
    return info
end

--//!< 5001 赛季信息
SCUTABaseInfo = SCUTABaseInfo or BaseClass(BaseProtocolStruct)
function SCUTABaseInfo:__init()
    self.msg_type = 5001
end

function SCUTABaseInfo:Decode()
    local info = {}
    info.cur_status = MsgAdapter.ReadShort()                --//!< 当前活动状态 1:准备 2:资格赛 3:正赛 4:结算 5:休赛
    info.round_index = MsgAdapter.ReadShort()               --//!< 当前比赛轮数
    info.season_idx = MsgAdapter.ReadShort()                --//!< 赛季号
    info.is_in_progress = MsgAdapter.ReadShort()            -- 是否进行中
    info.next_battle_timestamp = MsgAdapter.ReadUInt()      --//!< 下场战斗的时间戳
    info.end_timestamp = MsgAdapter.ReadUInt()              --//赛季结束时间，当前休赛结算且超过改是时间段后就不显示图标
    self.info = info

    UniverseTeamArenaCtrl.Log("天下第一比武大会赛季信息", info)
end

--//!< 5002 参赛队伍列表（已有序）
SCUTATeamList = SCUTATeamList or BaseClass(BaseProtocolStruct)
function SCUTATeamList:__init()
    self.msg_type = 5002
end

function SCUTATeamList:Decode()
    local count = MsgAdapter.ReadInt()
    self.team_list = {}
    for i = 1, count do
        local team_info = decode_uta_team_info_func()
        team_info.rank = i
        table.insert(self.team_list,team_info)
    end

    UniverseTeamArenaCtrl.Log("5002 参赛队伍列表（已有序）", self)
end

-- //!< 5003 我的队伍资格赛赛程
SCUTAMyTeamQualificationSchedules = SCUTAMyTeamQualificationSchedules or BaseClass(BaseProtocolStruct)
function SCUTAMyTeamQualificationSchedules:__init()
    self.msg_type = 5003
end

function SCUTAMyTeamQualificationSchedules:Decode()
    local schedule_list = {}
    for i = 0, 9 do
        local info = {}
        local team_id,id_t = uta_read_id_ll_func()
        info.round_idx = i   -- 轮数
        info.session = i % 2 == 0 and 0 or 1    -- 场次，一天两场
        info.team_id = team_id                  -- 队伍id
        info.team_id_t = id_t
        info.team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)   -- // 战队名
        info.server_id = MsgAdapter.ReadShort()   -- // 用于查询服务器名
        info.avatar_type = MsgAdapter.ReadShort()
        info.head_id = MsgAdapter.ReadShort()            -- // 队长头像ID
        info.fight_result = MsgAdapter.ReadShort()                  -- // 0:未赛  1:胜  2:负  3:弃权 4:平局
        table.insert(schedule_list, info)
    end
    self.schedule_list = schedule_list
    UniverseTeamArenaCtrl.Log("-- //!< 5003 我的队伍资格赛赛程",self)
end



--//!< 5004 查询队伍详情回包
SCUTATeamDetailInfo = SCUTATeamDetailInfo or BaseClass(BaseProtocolStruct)
function SCUTATeamDetailInfo:__init()
    self.msg_type = 5004
end

function SCUTATeamDetailInfo:Decode()
    local info = {}
    info.member_list = {}
    for i = 1, 5 do
        local member_info = decode_uta_member_info_func()
        table.insert(info.member_list, member_info)
    end
    self.info = info
    UniverseTeamArenaCtrl.Log("--//!< 5004 查询队伍详情回包",self)
end


--//!< 5005 我的队伍基本信息
SCUTAMyTeamBaseInfo = SCUTAMyTeamBaseInfo or BaseClass(BaseProtocolStruct)
function SCUTAMyTeamBaseInfo:__init()
    self.msg_type = 5005
end

function SCUTAMyTeamBaseInfo:Decode()
    local info = {}
    info.team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)   --//!< 我的队伍名
    info.qualification_score = MsgAdapter.ReadShort()           --//!< 当前积分
    info.cur_rank = MsgAdapter.ReadShort()                      --//!< 当前排名
    info.win_num = MsgAdapter.ReadShort()                       --//!< 胜场数
    info.lose_num = MsgAdapter.ReadShort()                      --//!< 负场数
    info.draw_num = MsgAdapter.ReadShort()                      --//!< 平局数
    info.giveup_num = MsgAdapter.ReadShort()                    --//!< 弃权数
    info.today_reward_num = MsgAdapter.ReadShort()              --//!< 今日已获得参与奖励数
    info.today_total_reward_num = MsgAdapter.ReadShort()        --//!< 今日最大可获得参与奖励数
    info.win_box_fetch_flag = MsgAdapter.IntFlag(1)        --//!< 胜场奖励领取标记（二进制）
    info.all_num = info.win_num + info.lose_num + info.draw_num
    self.info = info
    UniverseTeamArenaCtrl.Log("--//!< 5005 我的队伍基本信息",self)
end


-- 5006 战队当前积分排名（已有序）
SCUTATeamScoreRankList = SCUTATeamScoreRankList or BaseClass(BaseProtocolStruct)
function SCUTATeamScoreRankList:__init()
    self.msg_type = 5006
end

function SCUTATeamScoreRankList:Decode()
    local info = {}
    local team_id,id_t = uta_read_id_ll_func()
    info.my_team_id = team_id    --"我的队伍唯一ID"
    info.my_team_id_t = id_t
    local count = MsgAdapter.ReadInt()
    info.rank_list = {}
    for i = 1, count do
        local team_info = decode_uta_team_score_rank_info_func()
        team_info.rank = i
        table.insert(info.rank_list, team_info)
    end
    self.info = info
    UniverseTeamArenaCtrl.Log("-- 5006 战队当前积分排名（已有序）count == ",count,self)
end


-- //!< 5007 正赛赛程下发
SCUTAMainSchedules = SCUTAMainSchedules or BaseClass(BaseProtocolStruct)
function SCUTAMainSchedules:__init()
    self.msg_type = 5007
end

function SCUTAMainSchedules:Decode()
    local info = {}
    info.round_idx = MsgAdapter.ReadShort() --//!< 第X轮正赛 (0:32进16  1:16进8  2:8进4  3：4进2  4：总决赛  
    info.count = MsgAdapter.ReadShort()     --//!< 列表数量
    info.match_list = {}

    -- 比赛队伍信息
    local team_info_func = function ()
        local t = {}
        t.team_id,t.team_id_t = uta_read_id_ll_func()                 --  //!< 队伍名
        t.team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)  -- //!< 渠道类型
        t.plat_type = MsgAdapter.ReadInt()              -- //!< 队长服务器ID
        t.server_id = MsgAdapter.ReadShort()
        t.avatar_type = MsgAdapter.ReadShort()
        t.head_id = MsgAdapter.ReadShort()   -- //!< 队长头像
        t.support_rate = MsgAdapter.ReadShort()         -- //!< 支持率 万分比
        return t
    end

    -- 赛程信息
    local match_info_func = function ()
        local t = {}
        t.match_id,t.match_id_t = uta_read_id_ll_func()        -- //!< 赛事ID
        t.match_type = MsgAdapter.ReadShort() == 0 and UTACfg.MatchType.MainWin or UTACfg.MatchType.MainFail
        t.fight_result = MsgAdapter.ReadShort() -- //!< 0 未有结果  1 攻方胜  2 守方胜  3 已开始，等待结果中
        t.atk_team_info = team_info_func()      -- //!< 攻方队伍信息
        t.def_team_info = team_info_func()      -- //!< 守方队伍信息
        t.video_id,t.video_id_t = uta_read_id_ll_func()        -- 录像id
        return t
    end

    for i = 1, info.count do
        local match_info = match_info_func()
        match_info.show_index = i
        table.insert(info.match_list, match_info)
    end
    self.info = info
    UniverseTeamArenaCtrl.Log("-- //!< 5007 正赛赛程下发",self)
end

--//!< 5008 最终榜单(已有序)
SCUTAFinalRankList = SCUTAFinalRankList or BaseClass(BaseProtocolStruct)
function SCUTAFinalRankList:__init()
    self.msg_type = 5008
end

function SCUTAFinalRankList:Decode()
    self.season_index = MsgAdapter.ReadInt()    -- 赛季号
    MsgAdapter.ReadInt()
    self.final_rank_list = {}
    local count = MsgAdapter.ReadInt()
    for i = 1, count do
        local info = {}
        info.rank = i
        info.team_id,info.team_id_t = uta_read_id_ll_func()
        info.team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)   --//!< 队伍名
        info.plat_id = MsgAdapter.ReadInt()           --//!< 渠道号
        info.server_id = MsgAdapter.ReadInt()           --//!< 服务器
        info.total_win_num = MsgAdapter.ReadShort()     --//!< 总胜场数
        info.total_battle_num = MsgAdapter.ReadShort()  --//!< 总战斗场数
        table.insert(self.final_rank_list, info)
    end
    UniverseTeamArenaCtrl.Log("--//!< 5008 最终榜单(已有序)",count,self)
end

-- //!< 5009 晋级赛赛况总览
SCUTAMainOverviewSchedules = SCUTAMainOverviewSchedules or BaseClass(BaseProtocolStruct)
function SCUTAMainOverviewSchedules:__init()
    self.msg_type = 5009
end

function SCUTAMainOverviewSchedules:Decode()
    local info = {}
    info.overview_type = MsgAdapter.ReadShort()   -- 晋级赛类型MAIN_OVERVIEW_TYPE
    info.is_last_season = MsgAdapter.ReadShort()    -- 是否是上一个赛季的
    info.match_list = {}
    local count = MsgAdapter.ReadInt()
    for i = 1, count do
        local t = {}
        t.atk_team_id,t.atk_team_id_t = uta_read_id_ll_func() --攻方队伍ID
        t.def_team_id,t.def_team_id_t = uta_read_id_ll_func() --守放id
        t.atk_team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)  --//!< 攻方队名
        t.def_team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)  --//!< 守方队名
        t.fight_result = MsgAdapter.ReadInt()   -- //!< 0:未打  1:攻方胜  2:守方胜  3:在打
        t.video_id,t.video_id_t = uta_read_id_ll_func()    -- 录像id
        table.insert(info.match_list,t)
    end
    self.info = info
    UniverseTeamArenaCtrl.Log("-- //!< 5009 晋级赛赛况总览 count == ",count,self)
end

--//!< 5010 淘汰赛赛程
SCUTAQualificationSchedules = SCUTAQualificationSchedules or BaseClass(BaseProtocolStruct)
function SCUTAQualificationSchedules:__init()
    self.msg_type = 5010
end

function SCUTAQualificationSchedules:Decode()
    local info = {}
    info.round_idx = MsgAdapter.ReadInt() --//!< 轮数 [0,5)
    info.match_list = {}
    local count = MsgAdapter.ReadInt()

    -- 队伍信息
    local team_info_func = function ()
        local t = {}
        t.team_id,t.team_id_t = uta_read_id_ll_func()
        t.team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        t.plat_type = MsgAdapter.ReadInt()
        t.server_id = MsgAdapter.ReadShort()   -- 对战服务器id
        t.avatar_type = MsgAdapter.ReadShort()
        t.head_id = MsgAdapter.ReadShort()   -- 队长头像id
        t.cur_score = MsgAdapter.ReadShort()        -- 当前积分
        return t
    end

    -- 比赛信息
    local match_info_func = function ()
        local t = {}
        t.match_id,t.match_id_t = uta_read_id_ll_func()    -- 比赛id
        t.fight_result = MsgAdapter.ReadInt()   -- //!< 0:未打  1:攻方胜  2:守方胜  3:在打
        t.video_id,t.video_id_t = uta_read_id_ll_func()    -- 录像id
        t.atk_team_info = team_info_func()       -- 功方信息
        t.def_team_info = team_info_func()       -- 守方信息
        return t
    end

    for i = 1, count do
        local match_info = match_info_func()
        match_info.show_index = i
        table.insert(info.match_list, match_info)
    end
    self.info = info
    UniverseTeamArenaCtrl.Log("--//!< 5010 淘汰赛赛程",self)
end

-- 5011 战斗场景内的信息
SCUTASceneInfo = SCUTASceneInfo or BaseClass(BaseProtocolStruct)
function SCUTASceneInfo:__init()
    self.msg_type = 5011
end

function SCUTASceneInfo:Decode()
    local info = {}
    info.is_main_match = MsgAdapter.ReadChar()      -- 0资格赛，1晋级赛
    info.is_win_group = MsgAdapter.ReadChar()       -- 1 胜者组，0败者组 仅晋级赛时有效
    if info.is_main_match == 0 then
        info.match_type = UTACfg.MatchType.Out
    else
        info.match_type = info.is_win_group == 0 and UTACfg.MatchType.MainFail or UTACfg.MatchType.MainWin
    end
    info.round_index = MsgAdapter.ReadShort()       -- 轮次
    info.start_battle_timestamp = MsgAdapter.ReadUInt()     -- 比赛开始时间戳
    info.side_team_id,info.side_team_id_t = uta_read_id_ll_func()         -- 对手队伍唯一id
    info.side_team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)      -- 对手队伍名
    self.info = info
    UniverseTeamArenaCtrl.Log("-- 5011 战斗场景内的信息",self)
end

-- 5012 对阵信息 （自行倒计时3秒）
SCUTABattleVSInfo = SCUTABattleVSInfo or BaseClass(BaseProtocolStruct)
function SCUTABattleVSInfo:__init()
    self.msg_type = 5012
end

function SCUTABattleVSInfo:Decode()
    local info = {}
    info.is_main_match = MsgAdapter.ReadChar()      -- 0资格赛，1晋级赛
    info.is_win_group = MsgAdapter.ReadChar()       -- 1 胜者组，0败者组 仅晋级赛时有效
    if info.is_main_match == 0 then
        info.match_type = UTACfg.MatchType.Out
    else
        info.match_type = info.is_win_group == 0 and UTACfg.MatchType.MainFail or UTACfg.MatchType.MainWin
    end
    info.round_index = MsgAdapter.ReadShort()       -- 轮次
    info.self_team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN) -- 我方队伍名
    info.side_team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)  -- 对手队伍名

    info.self_member_list = {}
    info.side_member_list = {}

    local member_info_func = function ()
        local t = {}
        t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)   --//!< 玩家名
        t.prof = MsgAdapter.ReadShort()
        t.avatar_type = MsgAdapter.ReadShort()
        t.level = MsgAdapter.ReadShort()
        t.head_id = MsgAdapter.ReadShort()
        if t.level < 1 then
            return {}
        end
        return t
    end

    for i = 1, 5 do
        table.insert(info.self_member_list, member_info_func())
    end
    for i = 1, 5 do
        table.insert(info.side_member_list, member_info_func())
    end
    self.info = info
    --UniverseTeamArenaCtrl.Log("-- 5012 对阵信息 （自行倒计时3秒）",self)
    LogDG("-- 5012 对阵信息 （自行倒计时3秒）",self)
end


-- // 5013 准备进场通知
SCUTAPrepareEnterSceneNotice = SCUTAPrepareEnterSceneNotice or BaseClass(BaseProtocolStruct)
function SCUTAPrepareEnterSceneNotice:__init()
    self.msg_type = 5013
end

function SCUTAPrepareEnterSceneNotice:Decode()
    local info = {}
    info.start_prepare_time = MsgAdapter.ReadUInt()     -- //!< 开始显示"准备中"的时间戳 当前时间超过$start_prepare_time时可以显示"准备中"的按钮
    info.start_enter_scene_time = MsgAdapter.ReadUInt() -- //!< 开始进场的时间戳 当前时间超过$start_enter_scene_time时可以显示"进场中"的按钮
    info.stop_enter_scene_time = MsgAdapter.ReadUInt()  -- //!< 停止进场的时间戳 当前时间超过$stop_enter_scene_time时可以关闭显示
    self.info = info
    UniverseTeamArenaCtrl.Log("-- // 5013 准备进场通知",self)
end

-- 5014天下第一战斗结果显示
SCUTABattleResultShow = SCUTABattleResultShow or BaseClass(BaseProtocolStruct)
function SCUTABattleResultShow:__init()
    self.msg_type = 5014
end

function SCUTABattleResultShow:Decode()
    local info = {}
    info.is_main_match = MsgAdapter.ReadChar()
    if info.is_main_match == 0 then
        info.match_type = UTACfg.MatchType.Out
    else
        info.match_type = info.is_win_group == 0 and UTACfg.MatchType.MainFail or UTACfg.MatchType.MainWin
    end
    info.is_win = MsgAdapter.ReadChar()         -- 0 输, 1 赢，2平
    info.win_reason = MsgAdapter.ReadShort()    -- 胜利的原因0 打赢了，1对手未准时入场
    info.modify_score = MsgAdapter.ReadShort()  -- 资格赛积分变化
    info.cur_score = MsgAdapter.ReadShort()     -- 当前资格赛积分
    self.info = info
    UniverseTeamArenaCtrl.Log("-- 5014天下第一战斗结果显示",self)
end





--//!< 5051 请求参赛队伍列表
CSUTATeamListReq = CSUTATeamListReq or BaseClass(BaseProtocolStruct)
function CSUTATeamListReq:__init()
    self.msg_type = 5051
end

function CSUTATeamListReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    UniverseTeamArenaCtrl.Log("【发送了5051 请求参赛队伍列表】",self)
end

--//!< 5052 请求某个队伍的详细信息 ::::::: 返回5004协议
CSUTATeamDetailReq = CSUTATeamDetailReq or BaseClass(BaseProtocolStruct)
function CSUTATeamDetailReq:__init()
    self.msg_type = 5052
end

function CSUTATeamDetailReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    uta_send_id_ll_func(self.team_id_t)        --//!< 队伍唯一ID
    UniverseTeamArenaCtrl.Log("【发送了5052 请求某个队伍的详细信息】",self)
end

-- 5054领取胜场宝箱
CSUTAWinBox = CSUTAWinBox or BaseClass(BaseProtocolStruct)
function CSUTAWinBox:__init()
    self.msg_type = 5054
end

function CSUTAWinBox:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.sn)
    UniverseTeamArenaCtrl.Log("【5054领取胜场宝箱】",self)
end

--//!< 5053 请求我的队伍资格赛赛程 ：：：： 返回 5003协议
CSUTAMyTeamQualificationScheduleReq = CSUTAMyTeamQualificationScheduleReq or BaseClass(BaseProtocolStruct)
function CSUTAMyTeamQualificationScheduleReq:__init()
    self.msg_type = 5053
end

function CSUTAMyTeamQualificationScheduleReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    UniverseTeamArenaCtrl.Log("【发送了5053 请求我的队伍资格赛赛程】",self)
end

CSUTATeamScoreRankListReq = CSUTATeamScoreRankListReq or BaseClass(BaseProtocolStruct)
--//!< 5055 请求淘汰资格赛积分排行榜
function CSUTATeamScoreRankListReq:__init()
    self.msg_type = 5055
end

function CSUTATeamScoreRankListReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    UniverseTeamArenaCtrl.Log("【发送了5055 请求淘汰资格赛积分排行榜】",self)
end

-- //!< 5056 请求正赛赛程
CSUTAMainSchedules = CSUTAMainSchedules or BaseClass(BaseProtocolStruct)
function CSUTAMainSchedules:__init()
    self.msg_type = 5056
end

function CSUTAMainSchedules:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.round_idx)     -- 第X轮正赛 (0:32进16  1:16进8  2:8进4  3：4进2  4：总决赛
    UniverseTeamArenaCtrl.Log("【发送了5056 请求正赛赛程】",self)
end

-- //!< 5057 请求最终榜单    
CSUTAFinalRankList = CSUTAFinalRankList or BaseClass(BaseProtocolStruct)
function CSUTAFinalRankList:__init(  )
    self.msg_type = 5057
end

function CSUTAFinalRankList:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    UniverseTeamArenaCtrl.Log("【发送了5057 请求最终榜单】",self)
end

-- //!< 5058 请求资格赛赛程
CSUTAQualificationSchedules = CSUTAQualificationSchedules or BaseClass(BaseProtocolStruct)
function CSUTAQualificationSchedules:__init()
    self.msg_type = 5058
end

function CSUTAQualificationSchedules:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.round_idx)
    UniverseTeamArenaCtrl.Log("【发送了5058 请求资格赛赛程】",self)
end


--//!< 5059 请求正赛赛程总览
CSUTAMainSchedulesOverview = CSUTAMainSchedulesOverview or BaseClass(BaseProtocolStruct)
function CSUTAMainSchedulesOverview:__init()
    self.msg_type = 5059
end

function CSUTAMainSchedulesOverview:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.overview_type)
    MsgAdapter.WriteShort(self.is_last_season or 0)  -- 1 获取上届，0获取本届
    UniverseTeamArenaCtrl.Log("【发送了5059 请求正赛赛程总览】",self)
end

--// 5060 进入战斗场景
CSUTAEnterScene = CSUTAEnterScene or BaseClass(BaseProtocolStruct)
function CSUTAEnterScene:__init()
    self.msg_type = 5060
end

function CSUTAEnterScene:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    UniverseTeamArenaCtrl.Log("【发送了5060 进入战斗场景】",self)
end

-- class CSUTAWatchVideo		//!< 5061 查看战报
-- 	{
-- 	public:
-- 		CSUTAWatchVideo() : header(MT_UTA_WATCH_VIDEO_CS) {}
-- 		MessageHeader header;

-- 		long long video_id;		//!< 战报的ID
-- 	};

-- //!< 5061 查看比赛录像
CSUTAWatchVideo = CSUTAWatchVideo or BaseClass(BaseProtocolStruct)
function CSUTAWatchVideo:__init()
    self.msg_type = 5061
end

function CSUTAWatchVideo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    uta_send_id_ll_func(self.video_id_t)
    UniverseTeamArenaCtrl.Log("【发送了5061 查看录像请求】",self)
end

-- 5062 实时观战
CSUTAStartOB = CSUTAStartOB or BaseClass(BaseProtocolStruct)
function CSUTAStartOB:__init()
    self.msg_type = 5062
end

function CSUTAStartOB:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    uta_send_id_ll_func(self.match_id_t)
    UniverseTeamArenaCtrl.Log("【发送了5062 实时观战请求】",self)
end