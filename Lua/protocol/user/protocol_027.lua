
SCCrossStartAck = SCCrossStartAck or BaseClass(BaseProtocolStruct)
function SCCrossStartAck:__init()
    self.msg_type = 2801
end
function SCCrossStartAck:Decode()
    self.geteway_hostname = MsgAdapter.ReadStrN(PROTC_HOST_NAME_LEN)
    self.gateway_port = MsgAdapter.ReadInt()
    self.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    self.new_uid = MsgAdapter.ReadInt()
    self.scene_id = MsgAdapter.ReadInt()
    self.last_scene_id = MsgAdapter.ReadInt()
    self.time = MsgAdapter.ReadUInt()
    self.key = MsgAdapter.ReadStrN(PROTC_SESSION_KEY_LEN)
end

CSCrossStartReq = CSCrossStartReq or BaseClass(BaseProtocolStruct)
function CSCrossStartReq:__init()
    self.msg_type = 2800
end
function CSCrossStartReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

CSBackToOriginServer = CSBackToOriginServer or BaseClass(BaseProtocolStruct)
function CSBackToOriginServer:__init()
    self.msg_type = 2802
end
function CSBackToOriginServer:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end
SCBackToOriginServer = SCBackToOriginServer or BaseClass(BaseProtocolStruct)
function SCBackToOriginServer:__init()
    self.msg_type = 2803
end
function SCBackToOriginServer:Decode()
    self.scene_id = MsgAdapter.ReadInt()
    self.last_scene_id = MsgAdapter.ReadInt()
    self.time = MsgAdapter.ReadUInt()
    self.session_key = MsgAdapter.ReadStrN(PROTC_SESSION_KEY_LEN)
    self.gateway_hostname = MsgAdapter.ReadStrN(PROTC_HOST_NAME_LEN)--MsgAdapter.ReadChar(64)
    self.gateway_port = MsgAdapter.ReadUShort()
    MsgAdapter.ReadShort()
end


CSGuildHonorReq = CSGuildHonorReq or BaseClass(BaseProtocolStruct)
function CSGuildHonorReq:__init()
    self.msg_type = 2750
end
-- 0 P1分配到 0 1 2 3 p2 roleid
-- 到活动场景
function CSGuildHonorReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.p1)
    MsgAdapter.WriteInt(self.p2)
end

local function RoleItemInfo()
    return {
        role_id = MsgAdapter.ReadInt(),
        role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        level = MsgAdapter.ReadInt(),
        prof = MsgAdapter.ReadShort(),
        zone_id = MsgAdapter.ReadShort()
    }
end
SCGuildHonorRoleInfo = SCGuildHonorRoleInfo or BaseClass(BaseProtocolStruct)
function SCGuildHonorRoleInfo:__init()
    self.msg_type = 2751
end
function SCGuildHonorRoleInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.role_info = {}
    for i = 1, self.count, 1 do
        local info = RoleItemInfo()
        self.role_info[info.role_id] = info
    end
    self.zone_info = {}
    for _,v in pairs(self.role_info) do
        if self.zone_info[v.zone_id] == nil then
            self.zone_info[v.zone_id] = {}
        end
        table.insert(self.zone_info[v.zone_id], v)
    end
end

SCGuildHonorRoleChange = SCGuildHonorRoleChange or BaseClass(BaseProtocolStruct)
function SCGuildHonorRoleChange:__init()
    self.msg_type = 2752
end
function SCGuildHonorRoleChange:Decode()
    self.change_reson = MsgAdapter.ReadShort()--0移动 1删除
    self.zone_id = MsgAdapter.ReadShort()
    self.role_id = MsgAdapter.ReadInt()
end


SCGuildHonorInfo = SCGuildHonorInfo or BaseClass(BaseProtocolStruct)
function SCGuildHonorInfo:__init()
    self.msg_type = 2753
end
function SCGuildHonorInfo:Decode()
    self.state = MsgAdapter.ReadInt()
    self.next_match_state = MsgAdapter.ReadUInt()
    self.result_type = MsgAdapter.ReadInt()
end
SCGuildHonorAddRoleInfo = SCGuildHonorAddRoleInfo or BaseClass(BaseProtocolStruct)
function SCGuildHonorAddRoleInfo:__init()
    self.msg_type = 2754
end
function SCGuildHonorAddRoleInfo:Decode()
    self.role_info = RoleItemInfo()
end

SCGuildHonorZoneBattleResultInfo = SCGuildHonorZoneBattleResultInfo or BaseClass(BaseProtocolStruct)
function SCGuildHonorZoneBattleResultInfo:__init()
    self.msg_type = 2755
end
function SCGuildHonorZoneBattleResultInfo:Decode()
    self.zone_id = MsgAdapter.ReadInt()
    self.win_guild_id = MsgAdapter.ReadInt()--0是未分胜负，-1是平手，不然就是胜利的guild_id
end
SCGuildHonorAllZoneBattleResultInfo = SCGuildHonorAllZoneBattleResultInfo or BaseClass(BaseProtocolStruct)
function SCGuildHonorAllZoneBattleResultInfo:__init()
    self.msg_type = 2756
end
function SCGuildHonorAllZoneBattleResultInfo:Decode()
    self.battle_info = {}
    self.battle_info.guild_id1 = MsgAdapter.ReadInt()
    self.battle_info.guild_name1 = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    self.battle_info.guild_id2 = MsgAdapter.ReadInt()
    self.battle_info.guild_name2 = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    self.zone_battle_result = {}
    for i=1,3 do
        self.zone_battle_result[i] = MsgAdapter.ReadInt()
        --0是未分胜负，-1是平手，不然就是胜利的guild_id
    end
end
SCGuildHonorZoneRoleNumInfo = SCGuildHonorZoneRoleNumInfo or BaseClass(BaseProtocolStruct)
function SCGuildHonorZoneRoleNumInfo:__init()
    self.msg_type = 2757
end
function SCGuildHonorZoneRoleNumInfo:Decode()
    self.zone_role_num = {}
    for i=1,3 do
        self.zone_role_num[i] = MsgAdapter.ReadInt()
    end
end

SCGuildHonorBattleProcessInfo = SCGuildHonorBattleProcessInfo or BaseClass(BaseProtocolStruct)
function SCGuildHonorBattleProcessInfo:__init()
    self.msg_type = 2759
end
function SCGuildHonorBattleProcessInfo:Decode()
    self.process = MsgAdapter.ReadInt()
    self.settlement_list_time = MsgAdapter.ReadUInt()
end

SCGuildHonorBattleScoreInfo = SCGuildHonorBattleScoreInfo or BaseClass(BaseProtocolStruct)
function SCGuildHonorBattleScoreInfo:__init()
    self.msg_type = 2760
end
function SCGuildHonorBattleScoreInfo:Decode()
    self.guild_id = MsgAdapter.ReadInt()
    self.join_guild_fight_day_flag = bit:d16b(MsgAdapter.ReadUShort())
    self.score = {}
    for i= 0, 6 do
        self.score[i] = MsgAdapter.ReadUShort()
    end
    ---MsgAdapter.ReadUShort()
    -- LogDG(self)
end
local function HonorScoreRankItemInfo(index)
    local t = {}
    t.index = index
    t.server_id = MsgAdapter.ReadInt()
    t.guild_id = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    t.banner = MsgAdapter.ReadStrN(PROTC_GUILD_BANNER)
    t.join_guild_fight_day_flag = bit:d16b(MsgAdapter.ReadUShort())
    t.score = {}
    for i = 0, 6 do
        t.score[i] = MsgAdapter.ReadUShort()
    end
    return t
end
SCGuildHonorScoreRankInfo = SCGuildHonorScoreRankInfo or BaseClass(BaseProtocolStruct)
function SCGuildHonorScoreRankInfo:__init()
    self.msg_type = 2761
end
function SCGuildHonorScoreRankInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.items = {}
    for i = 1, self.count do
        self.items[i] = HonorScoreRankItemInfo(i)
    end
end
local function GuildHonorBattleMatchInfoItem(idx)
    local t = {}
    t.idx = idx --用于获取原始数据
    t.server_id = MsgAdapter.ReadInt()
    t.guild_id = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    t.banner = MsgAdapter.ReadStrN(PROTC_GUILD_BANNER)
    t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)--族长名字
    t.rank = MsgAdapter.ReadInt()--家族大乱斗排名

    t.match_round = MsgAdapter.ReadInt()--到达的回合，和curr_round不同
    t.match_round_2 = MsgAdapter.ReadInt()--开始季军赛的家族==1，胜者+1
    t.is_finish = MsgAdapter.ReadChar()--结束
    t.is_lose = MsgAdapter.ReadChar()--失败
    t.last_match_rank = MsgAdapter.ReadUShort() --上次比赛排名

    t.champion_guess_num = MsgAdapter.ReadInt()
    t.guess_num = {}
    for i = 1, GUILD_HONOR_BATTLE_MAX_ROUND do
        t.guess_num[i] = MsgAdapter.ReadInt()--每轮支持的人数
    end
    t.battle_id = {}
    for i = 1, GUILD_HONOR_BATTLE_MAX_ZONE do
        t.battle_id[i] = MsgAdapter.ReadInt()--每个区域的观战id
    end
    t.battle_result = {}
    for i = 1, GUILD_HONOR_BATTLE_MAX_ZONE do
        t.battle_result[i] = MsgAdapter.ReadInt()--每个区域的战斗结果，用来显示某个区域是否可观战
    end
    -- MsgAdapter.ReadUShort()
    return t
end
--赛况信息
SCGuildHonorBattleMatchInfo = SCGuildHonorBattleMatchInfo or BaseClass(BaseProtocolStruct)
function SCGuildHonorBattleMatchInfo:__init()
    self.msg_type = 2762
end
function SCGuildHonorBattleMatchInfo:Decode()
    --修改
    self.match_info_count = MsgAdapter.ReadInt()
    self.match_info = {}
    --非比赛组不需要显示

    LogDG("协议打印==================================================")
    LogDG("正式比赛组数", self.match_info_count)
    for i = 1, self.match_info_count do
        self.match_info[i] = GuildHonorBattleMatchInfo.New()
        self.match_info[i]:DecodeData()
    end
    --[[ for index, value in ipairs(self.match_info) do
        value:DebugMsg()
    end ]]
    LogDG("结束打印==================================================")
    --[[ self.curr_round = MsgAdapter.ReadInt()--当前回合
    self.round_state = MsgAdapter.ReadInt()--0准备1战斗2结束 0回合准备状态 1是开始
    self.next_round_state_time = MsgAdapter.ReadUInt()--回合结束时间
    self.is_finish = MsgAdapter.ReadInt()--是否完成
    self.fighter_num = MsgAdapter.ReadInt()
    self.fighter_info = {}
    for i = 1, self.fighter_num do
        self.fighter_info[i] = GuildHonorBattleMatchInfoItem(i)
    end ]]
end
--竞猜信息
SCGuildHonorBattleGuessInfo = SCGuildHonorBattleGuessInfo or BaseClass(BaseProtocolStruct)
function SCGuildHonorBattleGuessInfo:__init()
    self.msg_type = 2763
end
function SCGuildHonorBattleGuessInfo:Decode()
    
    self.champion_guild_id = MsgAdapter.ReadInt()
    self.guess_item_count = MsgAdapter.ReadInt()
    self.guess_item = {}
    local guild_id,round,win_rank
    for _ = 1, self.guess_item_count do
        win_rank = MsgAdapter.ReadShort()
        round = MsgAdapter.ReadShort()
        guild_id = MsgAdapter.ReadInt()
        -- round = MsgAdapter.ReadInt()
        self.guess_item[round] = self.guess_item[round] or {}
        self.guess_item[round][win_rank] = self.guess_item[round][win_rank] or {}
        self.guess_item[round][win_rank][guild_id] = true
        -- self.guess_type = MsgAdapter.ReadShort()--0胜利1冠军
    end
    -- LogDG("竞猜信息", self)
end

SCGuildHonorBattleMatchRecord = SCGuildHonorBattleMatchRecord or BaseClass(BaseProtocolStruct)
function SCGuildHonorBattleMatchRecord:__init()
    self.msg_type = 2764
end
function SCGuildHonorBattleMatchRecord:Decode()
    --[[ self.match_info_record_count = MsgAdapter.ReadInt()
    self.match_info_record = {}
    for i = 1, self.match_info_record_count do
        self.match_info_record[i] = GuildHonorBattleMatchInfo.New()
        self.match_info_record[i]:DecodeData(true)
    end ]]
    local temp_list = {}
    local temp
    self.fighter_num = MsgAdapter.ReadInt()
    -- LogDG("有效参与家族数", self.fighter_num)
    local list = {}
    for i = 1, self.fighter_num, 1 do
        temp = self:FighterItemInfo(i)
        -- LogDG("原数据显示", temp)
        list[temp.win_rank] = list[temp.win_rank] or {}
        list[temp.win_rank].fighter_info = list[temp.win_rank].fighter_info or {}
        list[temp.win_rank].win_rank = temp.win_rank
        list[temp.win_rank].start_first_round = temp.start_first_round
        table.insert(list[temp.win_rank].fighter_info, temp)
    end
    --[[ for index, value in pairs(list) do
        LogDG(value.win_rank)
    end ]]
    self.match_info_record = {}
    -- LogDG("信息重组出现，有",table.nums(list),"组数据")
    local count = table.nums(list)
    for key, value in pairs(list) do
        temp = GuildHonorBattleMatchInfo.New()
        temp:DecodeRecord(value)
        table.insert(self.match_info_record, temp)
    end
    --[[ for i = 1, count, 1 do
        self.match_info_record[i] = GuildHonorBattleMatchInfo.New()
        self.match_info_record[i]:DecodeRecord(list[i])
    end ]]
    -- LogDG("收到2764=========================")

    --[[ self.fighter_num = MsgAdapter.ReadInt()
    self.fighter_info = {}
    for i = 1, self.fighter_num do
        self.fighter_info[i] = FighterItemInfo(i)
    end
    table.sort(self.fighter_info, DataSorter.KeyLowerSorter("number"))
    local new_table = {}
    for index, value in ipairs(self.fighter_info) do
        if value.guild_id > 0 then
            table.insert( new_table, value )
        end
    end
    self.fighter_info = new_table
    self.fighter_num = table.nums(new_table)
    if self.fighter_info[1] and self.fighter_info[1].guild_id == 0 then
        --表示无赛果
        self.fighter_num = 0
    end ]]
    -- LogDG(self)
end

function SCGuildHonorBattleMatchRecord:FighterItemInfo(idx)
    local t = {}
    t.start_first_round = MsgAdapter.ReadShort()
    t.win_rank = MsgAdapter.ReadShort()
    t.number = MsgAdapter.ReadInt()
    t.server_id = MsgAdapter.ReadInt()
    t.guild_id = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    t.banner = MsgAdapter.ReadStrN(PROTC_GUILD_BANNER)
    t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)--族长名字
    t.guild_fight_rank = MsgAdapter.ReadInt()--家族大乱斗排名
    t.match_round = MsgAdapter.ReadInt()--到达的回合，和curr_round不同
    -- t.match_round_2 = MsgAdapter.ReadInt()
    t.champion_guess_num = MsgAdapter.ReadInt()
    t.guess_num = {}
    -- t.ranking = idx
    for i = 1, GUILD_HONOR_BATTLE_MAX_ROUND do
        t.guess_num[i] = MsgAdapter.ReadInt()--每轮支持的人数
    end
    if t.number == 0 then
        t.number = 17
    end
    --[[ if t.rank == 0 then
        t.rank = 17
    end ]]
    return t
end

SCGuildHonorBattleTopInfo = SCGuildHonorBattleTopInfo or BaseClass(BaseProtocolStruct)
function SCGuildHonorBattleTopInfo:__init()
    self.msg_type = 2765
end
function SCGuildHonorBattleTopInfo:Decode()
    self.guild_top_info = {}
    for i = 1, 3 do
        self.guild_top_info[i] = self.guild_top_info[i] or {}
        self.guild_top_info[i].guild_id = MsgAdapter.ReadInt()
    end
    for i = 1, 3 do
        self.guild_top_info[i] = self.guild_top_info[i] or {}
        self.guild_top_info[i].guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    end
end
SCGuildHonorRoleBattleState = SCGuildHonorRoleBattleState or BaseClass(BaseProtocolStruct)
function SCGuildHonorRoleBattleState:__init()
    self.msg_type = 2766
end
function SCGuildHonorRoleBattleState:Decode()
    self.state = MsgAdapter.ReadInt()
    -- LogDG("战斗状态", self.state)
end
CSGuildHonorReq2 = CSGuildHonorReq2 or BaseClass(BaseProtocolStruct)
function CSGuildHonorReq2:__init()
    self.msg_type = 2758
end
function CSGuildHonorReq2:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.p1)
    MsgAdapter.WriteInt(self.p2)
    -- LogDG(self)
end

CSPlatformBattleReq = CSPlatformBattleReq or BaseClass(BaseProtocolStruct)
function CSPlatformBattleReq:__init()
    self.msg_type = 2780
end

function CSPlatformBattleReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.oper_type)
    MsgAdapter.WriteShort(self.p1)
    MsgAdapter.WriteShort(self.p2)
    for _, info in pairs(self.pet_list) do
        local pos = ArenaMatchInfo.PosConvert(self.pos_c2s_config, info.vo.pos)
        local pet_index = info.pet.index
        MsgAdapter.WriteShort(pet_index)
        MsgAdapter.WriteShort(pos)
        MsgAdapter.WriteUChar(bit:b8d(info.pet_exclusive_skill_disable_flag))
        MsgAdapter.WriteUChar(bit:b8d(info.lian_yao_exclusive_skill_disable_flag))
        MsgAdapter.WriteShort(0)
    end
    for i = 1, ArenaMatchConfig.PET_NUM_MAX - self.pet_num do
        MsgAdapter.WriteShort(-1)
        MsgAdapter.WriteShort(-1)
        MsgAdapter.WriteUChar(0)
        MsgAdapter.WriteUChar(0)
        MsgAdapter.WriteShort(0)
    end

    for _, info in pairs(self.partner_list) do
        local pos = ArenaMatchInfo.PosConvert(self.pos_c2s_config, info.vo.pos)
        local partner_id = info.partner.partner_id
        MsgAdapter.WriteShort(partner_id)
        MsgAdapter.WriteShort(pos)
        MsgAdapter.WriteInt(0)
    end

    for i = 1, ArenaMatchConfig.PARTNER_NUM_MAX - self.partner_num do
        MsgAdapter.WriteShort(0)
        MsgAdapter.WriteShort(-1)
        MsgAdapter.WriteInt(0)
    end
end

-- 玩家自己守擂信息
SCPlatformBattleRoleInfo = SCPlatformBattleRoleInfo or BaseClass(BaseProtocolStruct)
function SCPlatformBattleRoleInfo:__init()
    self.msg_type = 2781
end

function SCPlatformBattleRoleInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.role_info_list = {}
    self.arena_match_index_list = {}
    self.arena_match_id_list = {}
    for i = 1, self.count do
        local t = {}
        t.platform_number = MsgAdapter.ReadShort()
        t.rank = MsgAdapter.ReadShort()
        t.pet_list = {}
        t.partner_list = {}
        for i = 1, ArenaMatchConfig.PET_NUM_MAX do
            local index = PetData.Instance:GetPetIndexByUniqueId(MsgAdapter.ReadUInt(), MsgAdapter.ReadUInt())
            t.pet_list[i] = {
                index = index
            }
            if index > -1 then
                self.arena_match_index_list[index] = true
            end
        end
        for i = 1, ArenaMatchConfig.PET_NUM_MAX do
            t.pet_list[i].pos = MsgAdapter.ReadUChar()
        end
        MsgAdapter.ReadUChar()
        MsgAdapter.ReadUChar()
        MsgAdapter.ReadUChar()
        for i = 1, ArenaMatchConfig.PARTNER_NUM_MAX do
            local id = MsgAdapter.ReadShort()
            t.partner_list[i] = {
                id = id,
                pos = MsgAdapter.ReadShort(),
            }
            MsgAdapter.ReadUInt()
            if id > 0 then
                self.arena_match_id_list[id] = true
            end
        end
        self.role_info_list[i] = t
    end
end

-- 一个擂台 所有玩家信息
SCPlatformBattleRankInfo = SCPlatformBattleRankInfo or BaseClass(BaseProtocolStruct)
function SCPlatformBattleRankInfo:__init()
    self.msg_type = 2782
end

function SCPlatformBattleRankInfo:Decode()
    self.platform_number = MsgAdapter.ReadShort()
    self.count = MsgAdapter.ReadShort()
    self.rank_info_list = {}
    for i = 1, self.count do
        local t = {}
        t.rank = i
        t.uid = MsgAdapter.ReadInt()
        t.level = MsgAdapter.ReadInt()
        t.top_level = MsgAdapter.ReadInt()
        t.avatar_type = MsgAdapter.ReadShort()
        t.avatar_id = MsgAdapter.ReadShort()--人物头像
        t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        t.pet_list = {}
        t.partner_list = {}
        for i = 1, ArenaMatchConfig.PET_NUM_MAX do
            t.pet_list[i] = {
                pos = MsgAdapter.ReadInt(),
                id = MsgAdapter.ReadInt(),
                cap = MsgAdapter.ReadInt(),
                level = MsgAdapter.ReadInt(),
                str_level = MsgAdapter.ReadInt(),
                skin_id = MsgAdapter.ReadInt(),
                name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
            }
        end
        for i = 1, ArenaMatchConfig.PARTNER_NUM_MAX do
            t.partner_list[i] = {
                pos = MsgAdapter.ReadInt(),
                id = MsgAdapter.ReadInt(),
                cap = MsgAdapter.ReadInt(),
                skin_id = MsgAdapter.ReadInt(),
                level = MsgAdapter.ReadInt(),
                intensify_level = MsgAdapter.ReadInt() + 1,
            }
        end
        self.rank_info_list[i] = t
    end
end

CSPlatformBattleRankFirstInfoReq = CSPlatformBattleRankFirstInfoReq or BaseClass(BaseProtocolStruct)
function CSPlatformBattleRankFirstInfoReq:__init()
    self.msg_type = 2783
end

function CSPlatformBattleRankFirstInfoReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    for _, arena in pairs(self.arena_list) do
        MsgAdapter.WriteChar(arena.seq)
    end
end

-- 一个擂台第一名玩家信息
SCPlatformBattleRankFirstInfo = SCPlatformBattleRankFirstInfo or BaseClass(BaseProtocolStruct)
function SCPlatformBattleRankFirstInfo:__init()
    self.msg_type = 2784
end

function SCPlatformBattleRankFirstInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.arena_show_list = {}
    for i = 1, self.count do
        local t = {}
        t.platform_number = MsgAdapter.ReadInt()
        t.rank = 1
        t.uid = MsgAdapter.ReadInt()
        t.level = MsgAdapter.ReadInt()
        t.top_level = MsgAdapter.ReadInt()
        t.avatar_type = MsgAdapter.ReadShort()
        t.avatar_id = MsgAdapter.ReadShort()--人物头像
        t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        t.pet_list = {}
        t.partner_list = {}
        for i = 1, ArenaMatchConfig.PET_NUM_MAX do
            t.pet_list[i] = {
                pos = MsgAdapter.ReadInt(),
                id = MsgAdapter.ReadInt(),
                cap = MsgAdapter.ReadInt(),
                level = MsgAdapter.ReadInt(),
                str_level = MsgAdapter.ReadInt(),
                skin_id = MsgAdapter.ReadInt(),
                name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
            }
        end
        for i = 1, ArenaMatchConfig.PARTNER_NUM_MAX do
            t.partner_list[i] = {
                pos = MsgAdapter.ReadInt(),
                id = MsgAdapter.ReadInt(),
                cap = MsgAdapter.ReadInt(),
                skin_id = MsgAdapter.ReadInt(),
                level = MsgAdapter.ReadInt(),
                intensify_level = MsgAdapter.ReadInt() + 1,
            }
        end
        self.arena_show_list[t.platform_number] = t
    end
end

CSPlatformBattlePlaformSwitch = CSPlatformBattlePlaformSwitch or BaseClass(BaseProtocolStruct)
function CSPlatformBattlePlaformSwitch:__init()
    self.msg_type = 2785
end

function CSPlatformBattlePlaformSwitch:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.platform_seq)
end 

--擂台战报
SCPlatformBattleRecord = SCPlatformBattleRecord or BaseClass(BaseProtocolStruct)
function SCPlatformBattleRecord:__init()
    self.msg_type = 2786
end

function SCPlatformBattleRecord:Decode()
    local record_number = MsgAdapter.ReadInt()
    self.record_infos = {}
    for i = 1,record_number do
        self.record_infos[i] = self:DeRecord()
    end
end

function SCPlatformBattleRecord:DeRecord()
    local record = {}
    record.role_info1 = {}
    record.role_info2 = {}
    local role_info
    for i=1,2 do
        role_info = record["role_info" .. i]
        role_info.role_id = MsgAdapter.ReadInt()
        role_info.role_level = MsgAdapter.ReadInt()
        role_info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        role_info.avatar_type = MsgAdapter.ReadShort()
        role_info.prof = math.modf(MsgAdapter.ReadShort() / 100)
        role_info.avatar_id = MsgAdapter.ReadUShort()
        MsgAdapter.ReadShort()
    end

    record.time = MsgAdapter.ReadUInt()
    record.is_win = MsgAdapter.ReadShort()
    record.platform_number = MsgAdapter.ReadShort()
    record.curr_rank = MsgAdapter.ReadShort()
    record.last_rank = MsgAdapter.ReadShort()
    return record
end

--擂台战报红点协议
SCPlatformBattleRecordNotice = SCPlatformBattleRecordNotice or BaseClass(BaseProtocolStruct)
function SCPlatformBattleRecordNotice:__init()
    self.msg_type = 2787
end

function SCPlatformBattleRecordNotice:Decode()
    self.info = {}
    self.info.is_first_times = MsgAdapter.ReadShort()            -- 非0表示本周没玩过
    self.info.has_lose_record = bit:d16b(MsgAdapter.ReadShort()) -- 每次排名降低都需要红点提示
end

-- 擂台成就奖励请求 req_type == ArenaMatchConfig.OperType2
CSPlatformBattleAchieve = CSPlatformBattleAchieve or BaseClass(BaseProtocolStruct)
function CSPlatformBattleAchieve:__init()
    self.msg_type = 2788
end

function CSPlatformBattleAchieve:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.oper_type)
    MsgAdapter.WriteInt(self.p1)
    MsgAdapter.WriteInt(self.p2)
end

-- 擂台成就信息
SCPlatformBattleAchieveInfo = SCPlatformBattleAchieveInfo or BaseClass(BaseProtocolStruct)
function SCPlatformBattleAchieveInfo:__init()
    self.msg_type = 2789
end

function SCPlatformBattleAchieveInfo:Decode()
    --self.next_reset_time = MsgAdapter.ReadUInt()
    self.type_count_info = {}
    for i = 0,16 do
        local info = {}
        info.count = MsgAdapter.ReadUShort()
        info.flags = bit:d16b(MsgAdapter.ReadUShort())
        self.type_count_info[i] = info
    end
end

-- 擂台结算通知
SCPlatformBattleEndNotice = SCPlatformBattleEndNotice or BaseClass(BaseProtocolStruct)
function SCPlatformBattleEndNotice:__init()
    self.msg_type = 2790
end

function SCPlatformBattleEndNotice:Decode()
    
end

-- 擂台上次排名信息
SCPlatformBattleLastRankInfo = SCPlatformBattleLastRankInfo or BaseClass(BaseProtocolStruct)
function SCPlatformBattleLastRankInfo:__init()
    self.msg_type = 2791
end

function SCPlatformBattleLastRankInfo:Decode()
    self.info_list = {}
    for i = 1, 2 do
        local info = {}
        info.platform_num = MsgAdapter.ReadShort()
        info.rank = MsgAdapter.ReadShort()
        if info.platform_num > 0 and info.rank > 0 then
            table.insert(self.info_list, info)
        end
    end
end