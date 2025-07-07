-- 4962  客户端获取 冠军记录手册[组队]
CSGetUTAChampionRecord = CSGetUTAChampionRecord or BaseClass(BaseProtocolStruct)
function CSGetUTAChampionRecord:__init()
    self.msg_type = 4962
end

function CSGetUTAChampionRecord:Encode()
    -- 0 开始 
    -- 如果获取 1~5界  就是  0 ， 4
    -- 获取第5界 就是 4 ，4 
    -- 获取5界及之后 的 就  4 ， 0
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.begin_index)
    MsgAdapter.WriteInt(self.end_index)
    UniverseTeamArenaCtrl.Log("请求了4962协议 获取 冠军记录手册", self)
end


-- 4963 冠军记录手册[组队] 信息
SCGetUTAChampionRecordAck = SCGetUTAChampionRecordAck or BaseClass(BaseProtocolStruct)
function SCGetUTAChampionRecordAck:__init()
    self.msg_type = 4963
end

function SCGetUTAChampionRecordAck:Decode()
    local uta_team_info_func = function (season_idx, team_name)
        local t = {}
        t.member_list = {}  -- 5 个成员
        for i = 1,5 do
            local member_info = {}
            member_info.season_idx = season_idx
            member_info.team_name = team_name
            member_info.uid = MsgAdapter.ReadInt()    -- //!< 玩家UID
            member_info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)        
            member_info.plat_id = MsgAdapter.ReadInt()    -- 平台id
            member_info.server_id = MsgAdapter.ReadInt()  -- //!< 所属服务器ID
            member_info.level = MsgAdapter.ReadShort()    -- //!< 等级
            member_info.fly_flag = MsgAdapter.ReadShort() -- //!< 飞升阶段
            member_info.top_level = MsgAdapter.ReadInt()  -- //!< 巅峰等级
            member_info.guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)    -- //!< 家族名称
            member_info.guild_banner = MsgAdapter.ReadStrN(PROTC_GUILD_BANNER)    -- //!< 家族族徽
            member_info.title_id = MsgAdapter.ReadInt()   -- //!< 称号
            member_info.facescore_id = MsgAdapter.ReadInt()       -- //!< 颜值系统头衔资源ID
            member_info.prof_base = MsgAdapter.ReadChar()         -- //!< 职业[0,9)
            member_info.advance_time = MsgAdapter.ReadChar()     -- //!< 进阶次数[0,6]
            member_info.avatar_type = MsgAdapter.ReadChar()         -- //!< 角色模型[1,6]
            member_info.color = MsgAdapter.ReadChar()             -- //!< 颜色[0,4)
            member_info.weapon_id = MsgAdapter.ReadUShort()       -- //!< 武器ID
            member_info.sub_weapon_id = MsgAdapter.ReadUShort()   -- //!< 副武器ID
            member_info.headshot_id = MsgAdapter.ReadShort()      -- //!< 头像ID
            member_info.special_appearance_type = MsgAdapter.ReadShort()  -- //!< 特殊形象类型
            member_info.special_appearance_param = MsgAdapter.ReadInt()   -- //!< 特殊形象参数
            member_info.wear_surface = {}     -- //!< 已装备的的幻化资源id 数组下标为配置表的type
            for i = 0, 5 do
                member_info.wear_surface[i] = MsgAdapter.ReadShort()
            end
            member_info.bicycle_index = MsgAdapter.ReadChar()
            member_info.bicycle_model_type = MsgAdapter.ReadChar()
            MsgAdapter.ReadShort()
            if member_info.uid ~= 0 then
                table.insert(t.member_list, member_info)
            end
        end
        return t
    end

    self.info_list = {}
    local count = MsgAdapter.ReadInt()
    for i = 0, count - 1 do
        local info = {}
        info.index = MsgAdapter.ReadInt()   -- // 第X界     0 开始 
        info.team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        info.team_info = uta_team_info_func(info.index, info.team_name)
        if #info.team_info.member_list > 0 then
            table.insert(self.info_list, info)
        end
    end

    UniverseTeamArenaCtrl.Log("4963 冠军记录手册[组队] 信息", count,self)
end




-- 天下第一比武竞猜 客户端操作
CSUTAGuessReq = CSUTAGuessReq or BaseClass(BaseProtocolStruct)
function CSUTAGuessReq:__init()
    self.msg_type = 4968
end

function CSUTAGuessReq:Encode()
    local uta_send_id_ll_func = function (id_t)
        if TableIsEmpty(id_t) then
            MsgAdapter.WriteInt(0)
            MsgAdapter.WriteInt(0)
            return
        end
        MsgAdapter.WriteInt(id_t[1])
        MsgAdapter.WriteInt(id_t[2])
    end
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)      -- req_type ==== UTACfg.GuessReqType
    if type(self.p1_t) == "table" then
        uta_send_id_ll_func(self.p1_t)
    else
        MsgAdapter.WriteLL(self.p1_t)
    end
    uta_send_id_ll_func(self.p2_t)
    MsgAdapter.WriteInt(self.p3)
    UniverseTeamArenaCtrl.Log("4968 天下第一比武竞猜 客户端操作", self)
end

-- 某一个比赛的竞猜信息
SCUTAGuessMatchInfo = SCUTAGuessMatchInfo or BaseClass(BaseProtocolStruct)
function SCUTAGuessMatchInfo:__init()
    self.msg_type = 4969
end

function SCUTAGuessMatchInfo:Decode()

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

    local info = {}
    info.match_id, info.match_id_t = uta_read_id_ll_func()        -- 比赛序号

    -- 队伍下注信息
    local team_bet_info_func = function ()
        local t = {}
        t.match_id = info.match_id          --//!< 下注的比赛序号
        t.match_id_t = info.match_id_t
        t.team_id,t.team_id_t = uta_read_id_ll_func()     --//!< 下注的队伍序号
        t.plat_id = MsgAdapter.ReadInt()    --//!< 所属平台ID
        t.server_id = MsgAdapter.ReadInt()  --//!< 所属服务器ID
        t.bet_num = MsgAdapter.ReadInt()    --//!< 下注的金额
        t.guess_status = MsgAdapter.ReadInt() --竞猜状态 ==== UTACfg.GuessStatus
        t.prof_list = {}
        for i = 1, 5 do
            t.prof_list[i] = RoleData.ProfIdToProfType(MsgAdapter.ReadInt())
        end
        t.win_count = MsgAdapter.ReadInt()  -- 连胜场次
        t.win_rate = MsgAdapter.ReadInt()   -- 胜率
        t.player_bet_num = MsgAdapter.ReadInt()  -- 玩家对比赛左方的下注信息
        return t
    end

    info.l_team_bet_info = team_bet_info_func()    -- 左边队伍的下注信息
    info.r_team_bet_info = team_bet_info_func()    -- 右边队伍的下注信息

    info.l_team_bet_info.side_bet_num = info.r_team_bet_info.bet_num    -- 对方下注了多少
    info.r_team_bet_info.side_bet_num = info.l_team_bet_info.bet_num    -- 对方下注了多少
    self.info = info

    UniverseTeamArenaCtrl.Log("-- 某一个比赛的竞猜信息", self)
end

-- 玩家参与下注的列表
SCUTAGuessBetRecord = SCUTAGuessBetRecord or BaseClass(BaseProtocolStruct)
function SCUTAGuessBetRecord:__init()
    self.msg_type = 4970
end

function SCUTAGuessBetRecord:Decode()

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

    -- 队伍信息
    local team_info_func = function ()
        local t = {}
        t.team_id, t.team_id_t = uta_read_id_ll_func()
        t.team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        t.bet_num = MsgAdapter.ReadInt()        --//!< 下注的金额
        t.is_get = MsgAdapter.ReadInt()         -- 是否已领取
        t.rate = MsgAdapter.ReadInt()           -- 赔率
        t.head_id = MsgAdapter.ReadShort()      -- 头像
        t.avatar_type = MsgAdapter.ReadShort()  -- 队长模型类型
        return t
    end

    local match_info_func = function ()
        local t = {}
        t.match_id, t.match_id_t = uta_read_id_ll_func()
        t.is_main_match = MsgAdapter.ReadChar()     -- 0资格赛，1正赛

        t.fight_result = MsgAdapter.ReadChar()      -- 比赛结果 0 未赛，1，攻胜，2守胜，3再打， 4平局
        t.round_index = MsgAdapter.ReadChar()       -- 比赛轮次
        t.is_win_group = MsgAdapter.ReadChar()       -- 1 胜者组，0败者组 仅晋级赛时有效
        if t.is_main_match == 0 then
            t.match_type = UTACfg.MatchType.Out
        else
            t.match_type = t.is_win_group == 0 and UTACfg.MatchType.MainFail or UTACfg.MatchType.MainWin
        end
        t.atk_team_info = team_info_func()
        t.def_team_info = team_info_func()
        t.atk_team_info.fight_result = t.fight_result
        t.def_team_info.fight_result = t.fight_result
        t.atk_team_info._type = 1       -- 红方
        t.def_team_info._type = 2       -- 蓝方
        t.is_get = 0
        if t.fight_result == 1 then
            t.is_get = t.atk_team_info.is_get
        elseif t.fight_result == 2 then
            t.is_get = t.def_team_info.is_get
        elseif t.fight_result == 4 then
            t.is_get = t.atk_team_info.bet_num > 0 and t.atk_team_info.is_get or t.def_team_info.is_get
        end
        return t
    end

    self.is_add = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.record_list = {}
    for i = 1, self.count do
        self.record_list[i] = match_info_func()
        self.record_list[i].index = i
    end


    UniverseTeamArenaCtrl.Log("-- 玩家参与下注的列表", self)
end

-- 竞猜币累计收益下发
SCUTAGuessHistoryEarning = SCUTAGuessHistoryEarning or BaseClass(BaseProtocolStruct)
function SCUTAGuessHistoryEarning:__init()
    self.msg_type = 4971
end

function SCUTAGuessHistoryEarning:Decode()
    self.guess_profit_num = MsgAdapter.ReadInt()    -- 累计收益
    UniverseTeamArenaCtrl.Log("4971累计收益下发", self)
end

-- 有可领竞猜奖励通知
SCUTAGuessHasBetRewardNotice = SCUTAGuessHasBetRewardNotice or BaseClass(BaseProtocolStruct)
function SCUTAGuessHasBetRewardNotice:__init()
    self.msg_type = 4972
end

function SCUTAGuessHasBetRewardNotice:Decode()
    
end