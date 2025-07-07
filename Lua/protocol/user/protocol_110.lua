
ANIMATION_SPEED = {
    NORMAL = 0,
    DOUBLE = 1,
    HALF = 2
}

local function create_node()
    return {
        grid_idx = 0,
        eve_type = BATTLE_DATA_EVENT.INVALID,
        data = {},
        childs = nil
    }
end

local function read_node()
    local re = create_node()
    re.grid_idx = MsgAdapter.ReadChar() + 1
    re.eve_type = MsgAdapter.ReadChar()
    local data_len = MsgAdapter.ReadShort()
    re.data = BattleNodeEvent.ParseData(re.eve_type, data_len)
    local son_num = MsgAdapter.ReadInt()
    if son_num > 0 then
        re.childs = {}
        for i = 1, son_num do
            re.childs[i] = read_node()
            -- re.childs[i].parent = re
        end
    end
    -- if IS_EDITOR then
        if re.eve_type == BATTLE_DATA_EVENT.PERFORM_SKILL then
            local cfg = SkillData.GetConfig(re.data.skill_id)
            if cfg then
                re.desc = string.format("%d释放%d%s技能", re.grid_idx, re.data.skill_id, cfg.skill_name)
            else
                re.desc = "获取不到配置skill_id = "..re.data.skill_id
                LogDG(re.desc)
            end
        elseif re.eve_type == BATTLE_DATA_EVENT.PERFORM_COMBO_SKILL then
            local cfg = SkillData.GetConfig(re.data.skill_id)
            if cfg then
                re.desc = string.format("%d释放%d%s连击技能", re.grid_idx, re.data.skill_id, cfg.skill_name)
            else
                re.desc = "获取不到配置combo_skill_id = "..re.data.skill_id
                LogDG(re.desc)
            end
        else
            re.desc = BATTLE_DATA_EVENT_DESC[re.eve_type]
        end
    -- end
    return re
end


CSBattleDemoStart = CSBattleDemoStart or BaseClass(BaseProtocolStruct)
function CSBattleDemoStart:__init()
    self.msg_type = 11052
end

function CSBattleDemoStart:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

CSAnimationFinish = CSAnimationFinish or BaseClass(BaseProtocolStruct)
function CSAnimationFinish:__init()
    self.msg_type = 11053
end

function CSAnimationFinish:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

---战斗操作，指令发送
CSBattleRoleOperate = CSBattleRoleOperate or BaseClass(BaseProtocolStruct)
function CSBattleRoleOperate:__init()
    self.msg_type = 11054
    self.operate_type = 0 --BATTLE_ROUND_OPERATE_TYPE
    self.param1 = 0
    self.param2 = 0
    self.param3 = 0
    self.param4 = 0
end

function CSBattleRoleOperate:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.operate_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
    MsgAdapter.WriteInt(self.param4)
    --LogDG("战斗操作协议", self)
end

---强制结束战斗
CSBattleForceFinishFight = CSBattleForceFinishFight or BaseClass(BaseProtocolStruct)
function CSBattleForceFinishFight:__init()
    self.msg_type = 11055
end

function CSBattleForceFinishFight:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

--前端通知服务器倒计时结束
CSCountDownFinish = CSCountDownFinish or BaseClass(BaseProtocolStruct)
function CSCountDownFinish:__init()
    self.msg_type = 11056
end

function CSCountDownFinish:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    -- LogDG("战斗协议11056通知服务端客户端倒计时结束")
end

---- 战斗结果通知  result_type=4 强制结束(内网有效)
SCBattleResult = SCBattleResult or BaseClass(BaseProtocolStruct)
function SCBattleResult:__init()
    self.msg_type = 11002
end

function SCBattleResult:Decode()
    local info = {}
    info.is_win = MsgAdapter.ReadChar() -- [0:负 1:胜]
    info.result_type = MsgAdapter.ReadChar() -- 结果类型
    info.battle_mode = MsgAdapter.ReadChar() -- 战斗模式
    MsgAdapter.ReadChar()
    self.battle_info = info
    BattleData.RecordData()
--    LogDG(info)
end

-- 战斗的开始由11003协议发出
-- 包含战斗模式是否观战战斗成员等信息
SCBattleInfo = SCBattleInfo or BaseClass(BaseProtocolStruct)
function SCBattleInfo:__init()
    self.msg_type = 11003
end

function SCBattleInfo:Decode()
    self.battle_mode = MsgAdapter.ReadShort()
    local character_num = MsgAdapter.ReadChar()
    self.can_modify_speed = MsgAdapter.ReadChar() --是否可以修改战报速度
    self.is_ob = MsgAdapter.ReadChar()--0非观战1击飞观战2一直观战  是你本来是在战场上， 但是被击飞了。
    self.ob_side = MsgAdapter.ReadChar()--0攻方1守方\
    self.is_pvp = MsgAdapter.ReadChar()--是否PVP 0否
    self.is_team_up = MsgAdapter.ReadChar()--是否组队 0否
    self.limit_timestamp = MsgAdapter.ReadUInt()
    self.monster_group_id = MsgAdapter.ReadInt()
    self.character_info_list = {}
    for i = 1, character_num do
        table.insert(self.character_info_list, BattleNodeEvent.parseCharacterInfo(self.character_info_list))
    end
    BuglyReport("战斗mode、怪物组id、观战类型", self.battle_mode,"|", self.monster_group_id,"|", self.is_ob, "|",RoleId())
end
SCBattleCharacterBuffList = SCBattleCharacterBuffList or BaseClass(BaseProtocolStruct)
function SCBattleCharacterBuffList:__init()
    self.msg_type = 11017
end
function SCBattleCharacterBuffList:Decode()
    local buff_list_count = MsgAdapter.ReadInt()
    self.character_buff_list = {}
    for i = 1, buff_list_count do
        local info = BattleNodeEvent.parseBuffInfo()
        self.character_buff_list[info.grid_idx] = self.character_buff_list[info.grid_idx] ~= nil and self.character_buff_list[info.grid_idx] or {}
        self.character_buff_list[info.grid_idx][info.new_buff_key] = info
    end
end
local function BattleRoundResult(flag)
    if flag == 1 then--等于1存在战报
        local result = {}
        MsgAdapter.ReadUShort()
        MsgAdapter.ReadShort()
        result.round = MsgAdapter.ReadInt()
        result.root = {}
        local tr_num = MsgAdapter.ReadInt()
        local total_data_len = MsgAdapter.ReadInt()
        for i = 1, tr_num do
            result.root[i] = read_node()
        end
        if IS_EDITOR then
            if BattleData.Instance.battle_record_list[result.round] == nil then
                BattleData.Instance.battle_record_list[result.round] = {}
            end
            BattleData.Instance.battle_record_list[result.round] = result.root
            Game.BattleDataLinker.data = TableToString(BattleData.Instance.battle_record_list)
        end
        return result
    end
    return nil
end
local function RoundResult()
    local result = {}
    result.round = MsgAdapter.ReadShort()
    result.has_prepare_result = MsgAdapter.ReadChar()
    result.has_formal_result = MsgAdapter.ReadChar()
    result.prepare_result = BattleRoundResult(result.has_prepare_result)
    result.formal_result = BattleRoundResult(result.has_formal_result)
    return result
end

SCBattleReport = SCBattleReport or BaseClass(BaseProtocolStruct)
function SCBattleReport:__init()
    self.msg_type = 11018
end
function SCBattleReport:Decode()

    self.version_info = {}
    self.version_info.sc_battleinfo_version = MsgAdapter.ReadUShort()
    self.version_info.sc_battleroundresult_version = MsgAdapter.ReadUShort()
    self.version_info.sc_battle_dragon_version = MsgAdapter.ReadUShort()
    self.version_info.event_version_num = MsgAdapter.ReadUInt()
    self.version_info.event_version_list = {}
    for i = 1, self.version_info.event_version_num do
        local version = {}
        version.event_type = MsgAdapter.ReadShort()
        version.event_version = MsgAdapter.ReadUShort()
        table.insert(self.version_info.event_version_list, version)
    end

    self.battle_info = {}
    self.battle_info.msg_type = MsgAdapter.ReadUShort()
    MsgAdapter.ReadShort()
    self.battle_info.battle_mode = MsgAdapter.ReadShort()
    local character_num = MsgAdapter.ReadChar()
    self.can_modify_speed = MsgAdapter.ReadChar()
    self.battle_info.is_ob = MsgAdapter.ReadChar()--0非观战1击飞观战2一直观战  是你本来是在战场上， 但是被击飞了。
    self.battle_info.ob_side = MsgAdapter.ReadChar()--0攻方1守方\
    self.battle_info.is_pvp = MsgAdapter.ReadChar()--是否PVP 0否
    self.battle_info.is_team_up = MsgAdapter.ReadChar()--是否组队 0否
    self.battle_info.limit_timestamp = MsgAdapter.ReadUInt()
    self.battle_info.monster_group_id = MsgAdapter.ReadInt()

    

    self.battle_info.character_info_list = {}
    for i = 1, character_num do
        table.insert(self.battle_info.character_info_list, BattleNodeEvent.parseCharacterInfo())
    end
    MsgAdapter.ReadInt()--协议头
    self.count = MsgAdapter.ReadInt()
    self.hallow_list = {}
    local temp
    for i = 1, self.count do
        temp = {} 
        temp.grid_idx = MsgAdapter.ReadShort() + 1 -- 21-30
        temp.hallow_id = MsgAdapter.ReadInt()--圣器id
        temp.owner_uid = MsgAdapter.ReadInt()
        temp.owner_idx = temp.grid_idx < 26 and temp.grid_idx - 20 or temp.grid_idx - 15
        self.hallow_list[temp.grid_idx] = temp
    end
    -- 神龙信息，相当于插了个 11022 协议
    MsgAdapter.ReadInt()
    self.battle_info.dragon_list = SCBattleGodDragonInfo.GetInfoList()
    
    self.round_num = MsgAdapter.ReadInt()
    self.round_list = {}
    for i = 1, self.round_num do
        self.round_list[i] = RoundResult()
    end
end

SCBattleCharacterSpecialEffectList = SCBattleCharacterSpecialEffectList or BaseClass(BaseProtocolStruct)
function SCBattleCharacterSpecialEffectList:__init()
    self.msg_type = 11019
end
SCBattleCharacterSpecialEffectList.MsgCharacterSpecialEffect = function ()
    local t = {}
    t.spe_id = MsgAdapter.ReadShort()
    t.grid_idx = MsgAdapter.ReadShort() + 1
    return t
end
function SCBattleCharacterSpecialEffectList:Decode()
    self.spe_count = MsgAdapter.ReadInt()
    self.spe_list = {}
    for i = 1, self.spe_count do
        local temp = self.MsgCharacterSpecialEffect()
        self.spe_list[temp.grid_idx] = self.spe_list[temp.grid_idx] or {}
        table.insert(self.spe_list[temp.grid_idx], temp.spe_id)
    end
end

SCBattleCharacterHaloList = SCBattleCharacterHaloList or BaseClass(BaseProtocolStruct)
function SCBattleCharacterHaloList:__init()
    self.msg_type = 11020
end
SCBattleCharacterHaloList.MsgCharacterHalo = function ()
    local t = {}
    t.grid_idx = MsgAdapter.ReadShort() + 1
    t.halo_id = MsgAdapter.ReadShort()
    t.cover_grid_flag = {}
    for i = 1, 20 do
        t.cover_grid_flag[i] = MsgAdapter.ReadChar()
    end
    return t
end
function SCBattleCharacterHaloList:Decode()
    self.halo_count = MsgAdapter.ReadInt()
    self.halo_list = {}
    for i = 1, self.halo_count do
        local temp = self.MsgCharacterHalo()
        self.halo_list[temp.grid_idx] = self.halo_list[temp.grid_idx] or {}
        table.insert(self.halo_list[temp.grid_idx], temp)
    end
end

SCBattleRoleOBSwitch = SCBattleRoleOBSwitch or BaseClass(BaseProtocolStruct)
function SCBattleRoleOBSwitch:__init()
    self.msg_type = 11012
end

function SCBattleRoleOBSwitch:Decode()
    self.is_ob = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    BuglyReport("观战变化", self.is_ob,"|", RoleId())
end

SCBattleRoundPhase = SCBattleRoundPhase or BaseClass(BaseProtocolStruct)
function SCBattleRoundPhase:__init()
    self.msg_type = 11005
    self.is_second_move_avaliable = {}
    for i = 1, BATTLE_MAX_CH_NUM do
        self.is_second_move_avaliable[i] = false
    end
end

function SCBattleRoundPhase:Decode()
    self.round = MsgAdapter.ReadShort()
    self.round_phase = MsgAdapter.ReadShort() --回合阶段 [1:等待玩家输入指令阶段(倒计时3秒或30秒取决于玩家是不是自动的) 2:服务器计算阶段 3:播放动画阶段]
    self.next_round_phase_time = MsgAdapter.ReadUInt()
    if self.round_phase == BATTLE_ROUND_PHASE.Command then
        for i = 1, BATTLE_MAX_CH_NUM do
            self.is_second_move_avaliable[i] = MsgAdapter.ReadChar()
        end
    end
    -- LogDG("战斗协议11005", self)
end

SCBattleRoleInfo = SCBattleRoleInfo or BaseClass(BaseProtocolStruct)
function SCBattleRoleInfo:__init()
    self.msg_type = 11006
    self.role_skill_list = {}
    self.pet_list = {}
end

function SCBattleRoleInfo:Decode()
    local role_skill_num = MsgAdapter.ReadShort()
    self.role_skill_list = {}
    local sk_info = {}
    for i = 1, role_skill_num do
        sk_info = {}
        sk_info.skill_id = MsgAdapter.ReadShort()
        sk_info.skill_level = MsgAdapter.ReadShort()
        sk_info.skill_index = MsgAdapter.ReadShort()
        sk_info.cooldown_num = MsgAdapter.ReadShort()
        sk_info.remain_times = MsgAdapter.ReadShort()
        self.role_skill_list[i] = sk_info
    end
    local pet_num = MsgAdapter.ReadShort()
    self.pet_list = {}
    for i = 1, pet_num do
        local pet_info = {}
        self.pet_list[i] = pet_info
        pet_info.pet_id = MsgAdapter.ReadInt()              
        pet_info.pet_level = MsgAdapter.ReadShort()        
        pet_info.pet_str_level = MsgAdapter.ReadShort()
        pet_info.skin_id = MsgAdapter.ReadInt()
        pet_info.maxhp = MsgAdapter.ReadInt()
        pet_info.maxmp = MsgAdapter.ReadInt()
        pet_info.curhp = MsgAdapter.ReadInt()
        pet_info.curmp = MsgAdapter.ReadInt()
        pet_info.bag_idx = i - 1
        ---ele
        pet_info.element = {}
        
        pet_info.element[40] = MsgAdapter.ReadShort()       -- 与幽冥共用
        pet_info.element[41] = MsgAdapter.ReadShort()       -- 与幽冥共用
        pet_info.element[42] = MsgAdapter.ReadShort()       -- 与幽冥共用
        pet_info.element[43] = MsgAdapter.ReadShort()       -- 与幽冥共用
        
        pet_info.status = MsgAdapter.ReadShort() --状态 [0:出战 1:待机]
        local p_skill_n = MsgAdapter.ReadShort()
        pet_info.hp_shield = MsgAdapter.ReadInt()           -- 剩余护盾值 当前是幽冥时，为幽冥护盾
        pet_info.fly_flag = MsgAdapter.ReadInt()
        pet_info.fly_flag = pet_info.fly_flag > FlyUpStageType.High and FlyUpStageType.High or pet_info.fly_flag
        pet_info.lianyao_pet_id = MsgAdapter.ReadInt()
        pet_info.is_youming = MsgAdapter.ReadChar()     -- 是否是幽冥
        pet_info.is_has_youming = MsgAdapter.ReadChar() -- 是否有幽冥信息
        pet_info.skill_list = {}
        for j = 1, p_skill_n do
            local sk_info = {}
            sk_info.skill_id = MsgAdapter.ReadShort()
            sk_info.skill_level = MsgAdapter.ReadShort()
            sk_info.skill_index = MsgAdapter.ReadShort()
            sk_info.cooldown_num = MsgAdapter.ReadShort()
            sk_info.remain_times = MsgAdapter.ReadShort()
            pet_info.skill_list[j] = sk_info
        end
        if pet_info.is_has_youming ~= 0 then
            local t = {}
            pet_info.youming_info = t
            t.youming_id = MsgAdapter.ReadInt()
            t.skill_info = {}
            t.skill_info.skill_id = MsgAdapter.ReadShort()
            t.skill_info.skill_level = MsgAdapter.ReadShort()
            t.skill_info.skill_index = MsgAdapter.ReadShort()   -- 技能序号
            t.skill_info.cooldown_num = MsgAdapter.ReadShort()  -- 技能cd
            t.skill_info.remain_cast_times = MsgAdapter.ReadShort() -- 剩余释放次数，-1为不限制
        end
    end
    self.is_auto = MsgAdapter.ReadChar() --[0:否 1:是]
    self.role_side = MsgAdapter.ReadChar() --[0:攻方 1:守方]
    self.first_move_type = MsgAdapter.ReadShort() --第一个行为的类型
    self.first_skill_idx = MsgAdapter.ReadShort() --第一个行为的技能序号 (行为是技能时有效）
    self.first_skill_level = MsgAdapter.ReadShort() --第一个行为的技能等级 (行为是技能时有效）
    self.second_move_type = MsgAdapter.ReadShort() --第二个行为的类型
    self.second_skill_idx = MsgAdapter.ReadShort() --第二个行为的技能序号 (行为是技能时有效）
    self.second_skill_level = MsgAdapter.ReadShort() --第二个行为的技能等级 (行为是技能时有效）
    self.animation_speed = MsgAdapter.ReadShort()

    self.fabao_cd = MsgAdapter.ReadShort()
    self.fabao_remain_times = MsgAdapter.ReadShort()
    local fabao_num = MsgAdapter.ReadShort()
    self.fabao_list = {}
    for i = 1, fabao_num do
        local fabao_info = {}
        fabao_info.index = i - 1
        fabao_info.fabao_id = MsgAdapter.ReadShort()
        fabao_info.fabao_level = MsgAdapter.ReadShort()
        fabao_info.fabao_skill_type = MsgAdapter.ReadShort()
        fabao_info.fabao_jie = MsgAdapter.ReadShort()
        self.fabao_list[i] = fabao_info
    end
    self.used_mp_item_times = MsgAdapter.ReadShort()
    self.max_mp_item_times = MsgAdapter.ReadShort()
    --假药数据
    self.fake_potion_num = MsgAdapter.ReadInt()
    self.fake_potion_list = {}
    local temp
    for i = 1, self.fake_potion_num do
        temp = {}
        temp.item_id = MsgAdapter.ReadUShort()
        temp.num = MsgAdapter.ReadShort()
        self.fake_potion_list[i] = Item.Create(temp)
    end
    self.guild_buff_list = {}
    for i = 1, 2 do
        self.guild_buff_list[i] = MsgAdapter.ReadShort()
    end
    self.heart_skill = {}
    self.heart_skill_is_vaild = MsgAdapter.ReadChar()
    self.heart_skill_count = MsgAdapter.ReadChar()
    for i = 1, self.heart_skill_count do
        sk_info = {}
        sk_info.is_heart_skill = true
        sk_info.skill_id = MsgAdapter.ReadShort()
        sk_info.skill_level = MsgAdapter.ReadShort()
        sk_info.remain_times = MsgAdapter.ReadShort()
        sk_info.is_vaild = self.heart_skill_is_vaild
        self.heart_skill[i] = sk_info
    end
    self.hp_shield = MsgAdapter.ReadInt()
    self.soaring_skill_id = MsgAdapter.ReadUShort()     --飞升技能id
    self.soaring_skill_lv = MsgAdapter.ReadShort()      --飞升技能等级
    self.live_card_skill_id = MsgAdapter.ReadInt()      --生活额外真假的技能id
    self.live_card_seq = MsgAdapter.ReadInt()           --生活道具索引
    -- 幻兽技能信息
    self.huanshou_skill_info = {
        skill_id = MsgAdapter.ReadUShort(),
        skill_level = MsgAdapter.ReadShort(),
        skill_cd = MsgAdapter.ReadShort(),
    }
end

SCBattleRoundResult = SCBattleRoundResult or BaseClass(BaseProtocolStruct)
function SCBattleRoundResult:__init()
    self.msg_type = 11004
    self.root = {}
end

function SCBattleRoundResult:Decode()
    self.round = MsgAdapter.ReadInt()
    self.root = {}
    local tr_num = MsgAdapter.ReadInt()
    local total_data_len = MsgAdapter.ReadInt()
    for i = 1, tr_num do
        self.root[i] = read_node()
    end
    ------------------------------------------------------
    if IS_EDITOR then
        if BattleData.Instance.battle_record_list == nil then
            BattleData.Instance.battle_record_list = {}
        end
        local round = table.nums(BattleData.Instance.battle_record_list) + 1
        BattleData.Instance.battle_record_list[round] = self.root
        Game.BattleDataLinker.data = TableToString(BattleData.Instance.battle_record_list)
    end
    --LOG("11004协议", self.round,tr_num,self.root)
end

--设置自动行为的回包
SCBattleSetAutoResp = SCBattleSetAutoResp or BaseClass(BaseProtocolStruct)
function SCBattleSetAutoResp:__init()
    self.msg_type = 11007
end

function SCBattleSetAutoResp:Decode()
    self.is_auto = MsgAdapter.ReadInt() --[0:否 1:是]
end

--强制结束战斗的返回或通知
SCBattleForceFinishFight = SCBattleForceFinishFight or BaseClass(BaseProtocolStruct)
function SCBattleForceFinishFight:__init()
    self.msg_type = 11008
end

function SCBattleForceFinishFight:Decode()
    LogDG("强制结束战斗的返回或通知")
end
AUTOMOVE_REASON_TYPE = 
    {
      REASON_TYPE_NORMAL = 0,
      REASON_TYPE_EMPTY_MP_RESET_MOVE = 1,    -- 空蓝重置自动行为
      REASON_TYPE_AI_SKILL = 2, --智能施法
    }

--设置自动行为的回包
SCBattleSetAutoMoveResp = SCBattleSetAutoMoveResp or BaseClass(BaseProtocolStruct)
function SCBattleSetAutoMoveResp:__init()
    self.msg_type = 11009
end

function SCBattleSetAutoMoveResp:Decode()
    self.first_move_type = MsgAdapter.ReadShort() --第一个行为的类型 (BATTLE_MOVE_TYPE)
    self.first_skill_idx = MsgAdapter.ReadShort() --第一个行为的技能序号 (行为是技能时有效）
    self.first_skill_level = MsgAdapter.ReadShort() --第一个行为的技能等级 (行为是技能时有效）
    self.second_move_type = MsgAdapter.ReadShort() --第二个行为的类型
    self.second_skill_idx = MsgAdapter.ReadShort() --第二个行为的技能序号 (行为是技能时有效）
    self.second_skill_level = MsgAdapter.ReadShort() --第二个行为的技能等级 (行为是技能时有效）
    self.notify_reason = MsgAdapter.ReadInt() --下发原因
    LogDG("自动行为回包", self)
end

SCBattleDemonCaveWave = SCBattleDemonCaveWave or BaseClass(BaseProtocolStruct)
function SCBattleDemonCaveWave:__init()
    self.msg_type = 11010
end
function SCBattleDemonCaveWave:Decode()
    self.cur_wave = MsgAdapter.ReadInt()
end

-- 战斗开始通知
SCBattleStart = SCBattleStart or BaseClass(BaseProtocolStruct)
function SCBattleStart:__init()
    self.msg_type = 11001
end

function SCBattleStart:Decode()
    self.is_boss_fight = MsgAdapter.ReadShort()   -- 1是boss战 0不是boss战
    self.is_mine_monster = MsgAdapter.ReadShort() -- 是否是暗怪
    self.boss_group_id = MsgAdapter.ReadInt() --怪物组id
    self.can_fast_forward = MsgAdapter.ReadShort()
    -- self.pvp_buff_id = 
    -- MsgAdapter.ReadShort()
    -- self.pvp_buff_level = 
    MsgAdapter.ReadShort()
    self.max_round = MsgAdapter.ReadShort()
    self.pvp_buff_num = MsgAdapter.ReadShort()
    self.pvp_buff_list = {}
    for i = 1, self.pvp_buff_num do
        self.pvp_buff_list[i] = {}
        self.pvp_buff_list[i].buff_id = MsgAdapter.ReadShort()
        self.pvp_buff_list[i].buff_level = MsgAdapter.ReadShort()
    end
    -- MsgAdapter.ReadShort()
    if IS_EDITOR then
        BattleData.Instance.battle_record_list = {}
    end
end

CSBattleModeFightReq = CSBattleModeFightReq or BaseClass(BaseProtocolStruct)
function CSBattleModeFightReq:__init()
    self.msg_type = 11058
end
function CSBattleModeFightReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.battle_mode)
    MsgAdapter.WriteShort(self.reserve_sh)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
end

CSChallengeOthersReq = CSChallengeOthersReq or BaseClass(BaseProtocolStruct)
function CSChallengeOthersReq:__init()
    self.msg_type = 11059
end
function CSChallengeOthersReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.battle_mode)
    MsgAdapter.WriteInt(self.role_id)
end

CSBattleQieCuoWithOthers = CSBattleQieCuoWithOthers or BaseClass(BaseProtocolStruct)
function CSBattleQieCuoWithOthers:__init()
    self.msg_type = 11060
end
function CSBattleQieCuoWithOthers:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.role_id)
end

SCOthersReqQieCuoWithYou = SCOthersReqQieCuoWithYou or BaseClass(BaseProtocolStruct)
function SCOthersReqQieCuoWithYou:__init()
    self.msg_type = 11061
end
function SCOthersReqQieCuoWithYou:Decode()
    self.role_id = MsgAdapter.ReadInt()
    self.sponsor_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
end

CSBattleQieCuoMessageRep = CSBattleQieCuoMessageRep or BaseClass(BaseProtocolStruct)
function CSBattleQieCuoMessageRep:__init()
    self.msg_type = 11062
end
function CSBattleQieCuoMessageRep:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.reply_role_id)
    MsgAdapter.WriteInt(self.rep_result)
end

-- 开始观战
CSWatchBattleReq = CSWatchBattleReq or BaseClass(BaseProtocolStruct)
function CSWatchBattleReq:__init()
    self.msg_type = 11200
end
function CSWatchBattleReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.watch_side)--观看0是左边 1是右边
    MsgAdapter.WriteShort(self.id_type)--0 uid 1 battle_id 家族荣誉战需要在跨服请求
    MsgAdapter.WriteInt(self.uid)
end
CSRoleUndo = CSRoleUndo or BaseClass(BaseProtocolStruct)
function CSRoleUndo:__init()
    self.msg_type = 11203
end
function CSRoleUndo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

-- 战术指挥
BATTLE_STRATEGY = {
    OTHER = 0,
    FOUCE_FIRE = 1, -- 集火
    HEAL = 2, -- 治疗
    REALIVE = 3, -- 复活
    CONTROL = 4, -- 控制
    BREAK_DEFENCE = 5, -- 破防
    BURN_MANA = 6, -- 消蓝
    MAX = 7
}

CSBattleSetStrategy = CSBattleSetStrategy or BaseClass(BaseProtocolStruct)
function CSBattleSetStrategy:__init()
    self.msg_type = 11071
end

function CSBattleSetStrategy:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.strategy_type)
    MsgAdapter.WriteShort(self.target_grid_id)
end

CSBattleClearStrategy = CSBattleClearStrategy or BaseClass(BaseProtocolStruct)
function CSBattleClearStrategy:__init()
    self.msg_type = 11072
end

function CSBattleClearStrategy:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.target_grid_id)
    MsgAdapter.WriteShort(self.is_clear_all)
end

SCBattleStrategyInfo = SCBattleStrategyInfo or BaseClass(BaseProtocolStruct)
function SCBattleStrategyInfo:__init()
    self.msg_type = 11073
end

function SCBattleStrategyInfo:Decode()
    local count = MsgAdapter.ReadShort()
    self.reason = MsgAdapter.ReadShort()-- 0normal 1add 2remove
    self.info_list = {}
    for i = 1, count do
        self.info_list[i] = {}
        self.info_list[i].strategy_type = MsgAdapter.ReadShort()
        self.info_list[i].target_grid_idx = MsgAdapter.ReadShort() + 1 -- [0,20)
        self.info_list[i].name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        self.info_list[i].istr = ""
        if self.info_list[i].strategy_type == 0 then
            self.info_list[i].istr = MsgAdapter.ReadStrN(64)
        end
    end
end

SCBattleRoleFinishInputNotice = SCBattleRoleFinishInputNotice or BaseClass(BaseProtocolStruct)
function SCBattleRoleFinishInputNotice:__init()
    self.msg_type = 11075
end

function SCBattleRoleFinishInputNotice:Decode()
    self.role_id = MsgAdapter.ReadInt()
    self.grid_idx = MsgAdapter.ReadShort() + 1
    MsgAdapter.ReadShort()
end

SCBattleRoleStartInput = SCBattleRoleStartInput or BaseClass(BaseProtocolStruct)
function SCBattleRoleStartInput:__init()
    self.msg_type = 11076
end

function SCBattleRoleStartInput:Decode()
    self.is_first_input_finish = MsgAdapter.ReadShort()
    self.is_second_input_finish = MsgAdapter.ReadShort()
end
--离开观战
CSBattleFlyAwayRoleLeaveBattle = CSBattleFlyAwayRoleLeaveBattle or BaseClass(BaseProtocolStruct)
function CSBattleFlyAwayRoleLeaveBattle:__init()
    self.msg_type = 11077
end

function CSBattleFlyAwayRoleLeaveBattle:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.is_leave_team)
end

SCBattleEndForAudience = SCBattleEndForAudience or BaseClass(BaseProtocolStruct)
function SCBattleEndForAudience:__init()
    self.msg_type = 11078
end

function SCBattleEndForAudience:Decode()
end

CSBattlePause = CSBattlePause or BaseClass(BaseProtocolStruct)
function CSBattlePause:__init()
    self.msg_type = 11079
end
function CSBattlePause:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.is_pause)
    --0取消暂停，1暂停
end

SCBattlePause = SCBattlePause or BaseClass(BaseProtocolStruct)
function SCBattlePause:__init()
    self.msg_type = 11080
end
function SCBattlePause:Decode()
    self.is_succ = MsgAdapter.ReadInt()
    --0失败1成功
end
CSBattlePrepareFinish = CSBattlePrepareFinish or BaseClass(BaseProtocolStruct)
function CSBattlePrepareFinish:__init()
    self.msg_type = 11081
end
function CSBattlePrepareFinish:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end
CSBattleGiveup = CSBattleGiveup or BaseClass(BaseProtocolStruct)
function CSBattleGiveup:__init()
    self.msg_type = 11082
end
function CSBattleGiveup:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end
--请求基本信息
CSBattleReportSimpleInfo = CSBattleReportSimpleInfo or BaseClass(BaseProtocolStruct)
function CSBattleReportSimpleInfo:__init()
    self.msg_type = 11083
end
function CSBattleReportSimpleInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.monster_group_id)
    MsgAdapter.WriteInt(0)
    MsgAdapter.WriteInt(0)
    MsgAdapter.WriteInt(0)
    MsgAdapter.WriteInt(self.is_server_first_kill)
end
--返回基本信息
SCBattleReportSimpleInfo = SCBattleReportSimpleInfo or BaseClass(BaseProtocolStruct)
function SCBattleReportSimpleInfo:__init()
    self.msg_type = 11084
end
function SCBattleReportSimpleInfo:Decode()
    self.monster_group_id = MsgAdapter.ReadInt()
    self.param1 = MsgAdapter.ReadInt()
    self.param2 = MsgAdapter.ReadInt()
    self.param3 = MsgAdapter.ReadInt()
    self.report_file_id = MsgAdapter.ReadLL()
    self.is_server_first_kill = MsgAdapter.ReadInt()
end

CSBattleReportDetailInfo = CSBattleReportDetailInfo or BaseClass(BaseProtocolStruct)
function CSBattleReportDetailInfo:__init()
    self.msg_type = 11085
end
function CSBattleReportDetailInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.monster_group_id)
    MsgAdapter.WriteInt(self.is_server_first_kill)
end
--暂停观看战报（队长可请求or单人时可请求）
CSBattleReportStopWatching = CSBattleReportStopWatching or BaseClass(BaseProtocolStruct)
function CSBattleReportStopWatching:__init()
    self.msg_type = 11086
end
function CSBattleReportStopWatching:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end
--11087 暂停观看战报返回 （队长和队员都会收到）
SCBattleReportStopWatching = SCBattleReportStopWatching or BaseClass(BaseProtocolStruct)
function SCBattleReportStopWatching:__init()
    self.msg_type = 11087
end
function SCBattleReportStopWatching:Decode()
    --全员退出录像
end

local AutoInfo = function ()
    return {
        role_id = MsgAdapter.ReadInt(),
        is_auto = MsgAdapter.ReadShort(),
        reserve_sh = MsgAdapter.ReadShort(),
    }
end
SCBattleAutoInfoList = SCBattleAutoInfoList or BaseClass(BaseProtocolStruct)
function SCBattleAutoInfoList:__init()
    self.msg_type = 11088
end
function SCBattleAutoInfoList:Decode()
    self.num = MsgAdapter.ReadInt()
    self.list = {}
    local temp
    for i = 1, self.num do
        temp = AutoInfo()
        self.list[temp.role_id] = temp.is_auto
    end
end
SCBattleAutoInfoChangeNotice = SCBattleAutoInfoChangeNotice or BaseClass(BaseProtocolStruct)
function SCBattleAutoInfoChangeNotice:__init()
    self.msg_type = 11089
end
function SCBattleAutoInfoChangeNotice:Decode()
    self.role_id = MsgAdapter.ReadInt()
    self.is_auto = MsgAdapter.ReadInt()--0手动 1自动
end

-- 开始特殊主角配置战斗
CSBattleStartNpcDemoFight = CSBattleStartNpcDemoFight or BaseClass(BaseProtocolStruct)
function CSBattleStartNpcDemoFight:__init()
    self.msg_type = 11090
end
function CSBattleStartNpcDemoFight:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.npc_demo_seq)
    MsgAdapter.WriteInt(self.is_initial_fight)
end

CSBattleChat = CSBattleChat or BaseClass(BaseProtocolStruct)
function CSBattleChat:__init()
    -- self.msg_type = 11201
    self.msg_type = 2137
end
function CSBattleChat:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.comment_type)
    MsgAdapter.WriteUInt(self.msg_length)
    MsgAdapter.WriteStrN(self.content, self.msg_length)
end

SCBattleChat = SCBattleChat or BaseClass(BaseProtocolStruct)
function SCBattleChat:__init()
    -- self.msg_type = 11202
    self.msg_type = 2138
end
function SCBattleChat:Decode()
    self.comment_type = MsgAdapter.ReadInt()
    self.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    self.from_uid = MsgAdapter.ReadInt()
    self.msg_length = MsgAdapter.ReadUInt()
    self.content = MsgAdapter.ReadStrN(self.msg_length)
end

SCBattleRoleInBattleNotice = SCBattleRoleInBattleNotice or BaseClass(BaseProtocolStruct)
function SCBattleRoleInBattleNotice:__init()
    self.msg_type = 11091
end
function SCBattleRoleInBattleNotice:Decode()
    self.battle_server_idx = MsgAdapter.ReadInt()
    self.battle_id = MsgAdapter.ReadInt()
end

CSRoleRejoinBattleScene = CSRoleRejoinBattleScene or BaseClass(BaseProtocolStruct)
function CSRoleRejoinBattleScene:__init()
    self.msg_type = 11092
end
function CSRoleRejoinBattleScene:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.battle_server_idx)
    MsgAdapter.WriteInt(self.battle_id)
end

--玩家战斗统计数据
SCBattleStatisticResult = SCBattleStatisticResult or BaseClass(BaseProtocolStruct)
function SCBattleStatisticResult:__init()
    self.msg_type = 11093
end

function SCBattleStatisticResult:Decode()
    --玩家 (自己放第一位)
    self.role_num = MsgAdapter.ReadInt()
    self.role_stat_list = {}
    local role_index = -1
    for i = 1, self.role_num do
        local role_stat = {}
        role_stat.avatar_type = MsgAdapter.ReadShort()
        role_stat.avatar_id = MsgAdapter.ReadShort()
        role_stat.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        local reserve_sh = MsgAdapter.ReadShort()
        role_stat.special_appearance_type = MsgAdapter.ReadShort()  --特殊形象类型
        role_stat.special_appearance_param = MsgAdapter.ReadInt()   --特殊形象参数
        --总伤害，总承伤，总治疗
        role_stat.total_damage_to_other = MsgAdapter.ReadLL()
        role_stat.total_suffer_injure = MsgAdapter.ReadLL()
        role_stat.total_heal_num = MsgAdapter.ReadLL()
        role_stat.fly_flag = MsgAdapter.ReadInt()
        role_stat.fly_flag = role_stat.fly_flag > FlyUpStageType.High and FlyUpStageType.High or role_stat.fly_flag
        if role_stat.role_name == RoleData.Instance:GetRoleName() then
            role_index = i - 1
            table.insert(self.role_stat_list, 1, role_stat)
        else
            table.insert(self.role_stat_list, role_stat)
        end
    end
    --伙伴
    self.partner_num = MsgAdapter.ReadInt()
    self.partner_stat_list = {}
    for i = 1, self.partner_num do
        local partner_stat = {}
        partner_stat.partner_id = MsgAdapter.ReadInt()
        partner_stat.skin_id = MsgAdapter.ReadInt()
        partner_stat.total_damage_to_other = MsgAdapter.ReadLL()
        partner_stat.total_suffer_injure = MsgAdapter.ReadLL()
        partner_stat.total_heal_num = MsgAdapter.ReadLL()
        partner_stat.fly_flag = MsgAdapter.ReadInt()
        partner_stat.fly_flag = partner_stat.fly_flag > FlyUpStageType.High and FlyUpStageType.High or partner_stat.fly_flag
        table.insert(self.partner_stat_list, partner_stat)
    end
    -- 宠物 (自己的宠物放第一位)
    self.pet_num = MsgAdapter.ReadInt()
    self.pet_stat_list = {}
    local pet_index = 1
    for i = 1, self.pet_num do
        local pet_stat = {}
        pet_stat.pet_id = MsgAdapter.ReadInt()
        pet_stat.skin_id = MsgAdapter.ReadInt()
        pet_stat.level = MsgAdapter.ReadShort()
        pet_stat.str_level = MsgAdapter.ReadChar()
        local role_stat_list_index = MsgAdapter.ReadChar() --宠物对应玩家的下标
        pet_stat.index = role_stat_list_index
        pet_stat.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        pet_stat.total_damage_to_other = MsgAdapter.ReadLL()
        pet_stat.total_suffer_injure = MsgAdapter.ReadLL()
        pet_stat.total_heal_num = MsgAdapter.ReadLL()
        pet_stat.fei_sheng_times = MsgAdapter.ReadInt()
        if role_index == role_stat_list_index then
            table.insert(self.pet_stat_list, pet_index, pet_stat)
            pet_index = pet_index + 1
        else
            table.insert(self.pet_stat_list, pet_stat)
        end
    end
end

-- 首席竞选战场buff显示
SCChiefElectionBattleResult = SCChiefElectionBattleResult or BaseClass(BaseProtocolStruct)
function SCChiefElectionBattleResult:__init()
    self.msg_type = 11094
end

function SCChiefElectionBattleResult:Decode()
    self.buff_info = {}
    self.buff_info.buff_id = MsgAdapter.ReadShort()
    self.buff_info.buff_value = MsgAdapter.ReadShort()
end
CSBattleSetCustomInstruction = CSBattleSetCustomInstruction or BaseClass(BaseProtocolStruct)
function CSBattleSetCustomInstruction:__init()
    self.msg_type = 11096
end
function CSBattleSetCustomInstruction:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.index)
    MsgAdapter.WriteShort(self.grid_idx)
end

CSBattleSkipReq = CSBattleSkipReq or BaseClass(BaseProtocolStruct)
function CSBattleSkipReq:__init()
    self.msg_type = 11098
end

function CSBattleSkipReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

CSBattleFastForwardReq = CSBattleFastForwardReq or BaseClass(BaseProtocolStruct)
function CSBattleFastForwardReq:__init()
    self.msg_type = 11099
end
function CSBattleFastForwardReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end
SCBattleFastForwardResp = SCBattleFastForwardResp or BaseClass(BaseProtocolStruct)
function SCBattleFastForwardResp:__init()
    self.msg_type = 11100
end
function SCBattleFastForwardResp:Decode()
    self.result = MsgAdapter.ReadInt()--0失败1成功
   -- LogDG("速战返回结果 = ", self.result == 1)
end

CSBattleAnimationSpeedModify = CSBattleAnimationSpeedModify or BaseClass(BaseProtocolStruct)
function CSBattleAnimationSpeedModify:__init()
    self.msg_type = 11204
end
function CSBattleAnimationSpeedModify:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.speed_type)
end
SCBattleAnimationSpeedModify = SCBattleAnimationSpeedModify or BaseClass(BaseProtocolStruct)
function SCBattleAnimationSpeedModify:__init()
    self.msg_type = 11205
end
function SCBattleAnimationSpeedModify:Decode()
    self.speed_type = MsgAdapter.ReadInt()
end

SCWorldLevelInfo = SCWorldLevelInfo or BaseClass(BaseProtocolStruct)
function SCWorldLevelInfo:__init()
    self.msg_type = 1126
end
function SCWorldLevelInfo:Decode()
    self.open_day = MsgAdapter.ReadInt()    -- 跨服的开启天数
    self.today_zero_timestamp = MsgAdapter.ReadUInt()
end

SCBattleHallowList = SCBattleHallowList or BaseClass(BaseProtocolStruct)
function SCBattleHallowList:__init()
    self.msg_type = 11021
end
function SCBattleHallowList:Decode()
    self.count = MsgAdapter.ReadInt()
    self.hallow_list = {}
    local temp
    for i = 1, self.count do
        temp = {} 
        temp.grid_idx = MsgAdapter.ReadShort() + 1 -- 21-30
        temp.hallow_id = MsgAdapter.ReadInt()--圣器id
        temp.owner_uid = MsgAdapter.ReadInt()
        temp.owner_idx = temp.grid_idx < 26 and temp.grid_idx - 20 or temp.grid_idx - 15
        self.hallow_list[temp.grid_idx] = temp
    end
    -- LogDG(self)
end

SCBattleGuildBossTotalDamage = SCBattleGuildBossTotalDamage or BaseClass(BaseProtocolStruct)  -- 11101 速战后下发对家族BOSS总伤害
function SCBattleGuildBossTotalDamage:__init()
    self.msg_type = 11101
end
function SCBattleGuildBossTotalDamage:Decode()
    self.total_damage = MsgAdapter.ReadLL()
    -- LogDG(self)
end

CSBattleSurrender = CSBattleSurrender  or BaseClass(BaseProtocolStruct)
function CSBattleSurrender:__init()
    self.msg_type = 11102
end
function CSBattleSurrender:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

SCBattleMoveBoardcast = SCBattleMoveBoardcast or BaseClass(BaseProtocolStruct)
function SCBattleMoveBoardcast:__init()
    self.msg_type = 11206
end
function SCBattleMoveBoardcast:Decode()
    self.info = {}
    self.info.grid_idx = MsgAdapter.ReadShort() + 1
    self.info.target_grid_idx = MsgAdapter.ReadShort() + 1
    self.info.move_type = MsgAdapter.ReadShort()--行为类行
    self.info.param1 = MsgAdapter.ReadShort()--当前行为为技能时为技能id，当前为法宝时，法宝序号 [0, fabaomax)
    self.info.param2 = MsgAdapter.ReadShort()--技能等级
    self.info.param3 = MsgAdapter.ReadShort()
end



-- 神龙战斗信息
SCBattleGodDragonInfo = SCBattleGodDragonInfo or BaseClass(BaseProtocolStruct)
function SCBattleGodDragonInfo:__init()
    self.msg_type = 11022
end

function SCBattleGodDragonInfo:Decode()
    self.dragon_list = SCBattleGodDragonInfo.GetInfoList()
end

function SCBattleGodDragonInfo.GetInfoList()
    local info_list = {}
    for i = 31, 32 do
        local t = BattleDragon.Decode()
        t.grid_idx = i
        if t.level > 0 then
            info_list[i] = t
        end
    end
    return info_list
end


-- 战斗护盾格子信息
SCBattleShieldGridInfo = SCBattleShieldGridInfo or BaseClass(BaseProtocolStruct)
function SCBattleShieldGridInfo:__init()
    self.msg_type = 11023
end

function SCBattleShieldGridInfo:Decode()
    self.rabbit_shield_list = {}
    for i = RABBIT_SHIELD_GRID_BEGIN_INDEX + 1, RABBIT_SHIELD_GRID_BEGIN_INDEX + RABBIT_SHILED_GRID_MAX do
        local has_shiled = MsgAdapter.ReadChar()
        if has_shiled == 1 then
            self.rabbit_shield_list[i] = true
        end
    end
    MsgAdapter.ReadShort()
end