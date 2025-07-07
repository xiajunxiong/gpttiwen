
-- 请求阵容简易信息（打开界面时请求）
CSFormationSimpleInfo = CSFormationSimpleInfo or BaseClass(BaseProtocolStruct)
function CSFormationSimpleInfo:__init()
    self.msg_type = 2950
end

function CSFormationSimpleInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

-- 阵容简易信息返回
SCFormationSimpleInfo = SCFormationSimpleInfo or BaseClass(BaseProtocolStruct)
function SCFormationSimpleInfo:__init()
    self.msg_type = 2951
end

function SCFormationSimpleInfo:Decode()
    local info = {
        valid_formation_num = MsgAdapter.ReadShort(),
        active_formation_num = MsgAdapter.ReadShort(),
        formation_list = {},
    }
    for i = 1, info.active_formation_num do
        info.formation_list[i] = {
            seq = MsgAdapter.ReadShort(),
            reserve_sh = MsgAdapter.ReadShort(),
            name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        }
    end
    self.info = info
end

-- 请求获取阵容详细信息
CSFormationDetailInfo = CSFormationDetailInfo or BaseClass(BaseProtocolStruct)
function CSFormationDetailInfo:__init()
    self.msg_type = 2952
end

function CSFormationDetailInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.formation_seq)
end

-- 阵容详细信息返回
SCFormationDetailInfo = SCFormationDetailInfo or BaseClass(BaseProtocolStruct)
function SCFormationDetailInfo:__init()
    self.msg_type = 2953
end

function SCFormationDetailInfo:Decode()
    local info = {
        reason = MsgAdapter.ReadChar(),
        is_valid = MsgAdapter.ReadChar(),
        role_row = MsgAdapter.ReadChar(),
        formation_seq = MsgAdapter.ReadChar(),
        auto_info = {
            first_move_type = MsgAdapter.ReadShort(),
            first_skill_id = MsgAdapter.ReadUShort(),
            first_skill_level = MsgAdapter.ReadShort(),
            second_move_type = MsgAdapter.ReadShort(),
            second_skill_id = MsgAdapter.ReadUShort(),
            second_skill_level = MsgAdapter.ReadShort(),
        },
        pet_idx = MsgAdapter.ReadShort(),
        partner_num = MsgAdapter.ReadShort(),
        partner_list = {},
    }
    for i = 1, info.partner_num do
        info.partner_list[i] = {
            partner_id = MsgAdapter.ReadInt(),
            partner_pos = MsgAdapter.ReadShort() + 1,
        }
        MsgAdapter.ReadShort()
    end
    self.info = info
end

-- 请求获取阵容详细信息
CSFormationRename = CSFormationRename or BaseClass(BaseProtocolStruct)
function CSFormationRename:__init()
    self.msg_type = 2954
end

function CSFormationRename:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.formation_seq)
    MsgAdapter.WriteStrN(self.new_name, PROTC_ROLE_NAME_LEN)
end

-- 阵容改名回包
SCFormationRename = SCFormationRename or BaseClass(BaseProtocolStruct)
function SCFormationRename:__init()
    self.msg_type = 2955
end

function SCFormationRename:Decode()
    self.info = {
        formation_seq = MsgAdapter.ReadInt(),
        new_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
    }
end

-- 保存阵容改动
CSFormationSave = CSFormationSave or BaseClass(BaseProtocolStruct)
function CSFormationSave:__init()
    self.msg_type = 2956
end

function CSFormationSave:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.is_save_new_format)    -- 0:完成布阵 1:保存为新阵容
    MsgAdapter.WriteShort(self.formation_seq)
    MsgAdapter.WriteShort(self.role_row)            -- 主角站位 0:后排 1:前排
    MsgAdapter.WriteShort(self.pet_idx)

    local auto_info = self.auto_info
    MsgAdapter.WriteShort(auto_info.first_move_type or 0)
    MsgAdapter.WriteUShort(auto_info.first_skill_id or 0)
    MsgAdapter.WriteShort(auto_info.first_skill_level or 0)
    MsgAdapter.WriteShort(auto_info.second_move_type or 0)
    MsgAdapter.WriteUShort(auto_info.second_skill_id or 0)
    MsgAdapter.WriteShort(auto_info.second_skill_level or 0)

    MsgAdapter.WriteShort(#self.partner_list)
    for i = 1, 4 do
        local partner = self.partner_list[i]
        if partner then
            MsgAdapter.WriteInt(partner.partner_id)
            MsgAdapter.WriteShort(partner.partner_pos - 1)
            MsgAdapter.WriteShort(0)
        else
            MsgAdapter.WriteInt(0)
            MsgAdapter.WriteShort(0)
            MsgAdapter.WriteShort(0)
        end
    end
    MsgAdapter.WriteInt(#self.member_list)
    for i = 1, #self.member_list do
        local member = self.member_list[i]
        MsgAdapter.WriteInt(member.uid)
        MsgAdapter.WriteShort(member.member_pos - 1)
        MsgAdapter.WriteShort(0)
    end
end

-- 删除阵容（清空某个阵容，注意当前阵容不可以删除）
CSFormationDelete = CSFormationDelete or BaseClass(BaseProtocolStruct)
function CSFormationDelete:__init()
    self.msg_type = 2957
end

function CSFormationDelete:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.formation_seq)
end

-- 玉虚境操作请求 0:挑战 1:扫荡 2:领取宝箱 3:购买关卡次数 param1 : seq
CSMaterialReq = CSMaterialReq or BaseClass(BaseProtocolStruct)
function CSMaterialReq:__init()
    self.msg_type = 2910
end

function CSMaterialReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.op_type)
    MsgAdapter.WriteShort(self.param1)
end

-- 玉虚境场景信息
SCMaterialSceneInfo = SCMaterialSceneInfo or BaseClass(BaseProtocolStruct)
function SCMaterialSceneInfo:__init()
    self.msg_type = 2911
end

function SCMaterialSceneInfo:Decode()
    self.chapter = MsgAdapter.ReadChar()    --章节ID
    self.level = MsgAdapter.ReadChar()     --关卡seq
    self.kill_boss_flag = MsgAdapter.ReadChar()--0:未击杀  1：已击杀
    self.obj_list = {}
    self.talk_npc_id = 0
    for i=1,MsgAdapter.ReadChar() do
        local vo = {}
        vo.id = MsgAdapter.ReadInt()
        vo.scene_id = SceneData.Instance:SceneId()
        vo.is_need_talk = MsgAdapter.ReadChar()
        vo.obj_type = MsgAdapter.ReadChar()
        vo.obj_id = MsgAdapter.ReadUShort()
        vo.x = MsgAdapter.ReadShort()
        vo.y = MsgAdapter.ReadShort()
        self.obj_list[vo.id] = vo
        if self.talk_npc_id == 0 and vo.is_need_talk == 1 then
            self.talk_npc_id = vo.id
        end
    end
end

local function MaterialLevelStruct(is_short)
    local t = {}
    t.chapter = MsgAdapter.ReadChar()    --章节ID
    t.level = MsgAdapter.ReadChar()      --关卡seq
    local is_first_kill = MaterialData.Instance:IsFirstKill(t)
    local flag = bit:d8b(MsgAdapter.ReadUChar())
    t.flag_list,t.flag_num = CourageChallengeStruct.GetStarList(flag)
    t.first_kill = flag[1] or 0
    MsgAdapter.ReadUChar()                 --成就条件完成标识二进制
    t.remain_times = MsgAdapter.ReadChar() --玩家今日通过次数
    t.buy_times = MsgAdapter.ReadChar()    --玩家今日购买次数
    if is_short then
        MsgAdapter.ReadShort()
    end
    t.is_first_kill = is_first_kill == false and t.first_kill == 1
    return t
end

local function MaterialChapterStruct()
    local t = {}
    t.chapter = MsgAdapter.ReadShort()
    t.integral_count = MsgAdapter.ReadShort()
    return t
end

-- 玉虚境角色所有信息
SCMaterialAllInfo = SCMaterialAllInfo or BaseClass(BaseProtocolStruct)
function SCMaterialAllInfo:__init()
    self.msg_type = 2912
end

function SCMaterialAllInfo:Decode()
    self.chapter_info = {}    --关卡数据
    self.intergral_info = {}  --章节总积分
    for i=1,MsgAdapter.ReadShort() do
        local info = MaterialLevelStruct()
        if self.chapter_info[info.chapter] == nil then
            self.chapter_info[info.chapter] = {}
        end
        self.chapter_info[info.chapter][info.level] = info
    end
    for i=1,MsgAdapter.ReadShort() do
        local info = MaterialChapterStruct()
        self.intergral_info[info.chapter] = info.integral_count
    end
end

-- 玉虚境角色单个信息
SCMaterialSingleInfo = SCMaterialSingleInfo or BaseClass(BaseProtocolStruct)
function SCMaterialSingleInfo:__init()
    self.msg_type = 2913
end

function SCMaterialSingleInfo:Decode()
    MsgAdapter.ReadShort()
    self.integral_count = MsgAdapter.ReadShort()--当前章节总积分
    self.chapter_info = MaterialLevelStruct(true)
    self.chapter = self.chapter_info.chapter
    self.level = self.chapter_info.level
    self.is_first_kill = self.chapter_info.is_first_kill
end

-- 玉虚境其它信息下发
SCMaterialOtherInfo = SCMaterialOtherInfo or BaseClass(BaseProtocolStruct)
function SCMaterialOtherInfo:__init()
    self.msg_type = 2914
end

function SCMaterialOtherInfo:Decode()
    self.chapter = MsgAdapter.ReadChar()--玩家当前到达的章节
    self.level = MsgAdapter.ReadChar() --玩家当前到达对应章节的关卡
    self.day_pass_time = MsgAdapter.ReadShort()--今天通关总次数
    self.box_flag = MsgAdapter.IntFlag()       --宝箱领取标识
end


--战斗开始时如果能获得侠义值则后端发包提示
SCSalaryBattleStateInfo = SCSalaryBattleStateInfo or BaseClass(BaseProtocolStruct)
function SCSalaryBattleStateInfo:__init()
    self.msg_type = 2920
end

function SCSalaryBattleStateInfo:Decode()
    self.seq = MsgAdapter.ReadInt()--侠义值类型seq
    self.salary = MsgAdapter.ReadInt() --本场战斗能获得多少侠义值
end

CSSetPetHelperFormation = CSSetPetHelperFormation or BaseClass(BaseProtocolStruct)
function CSSetPetHelperFormation:__init()
    self.msg_type = 2970
end

function CSSetPetHelperFormation:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.system_type)
    for i=1, 10 do
        MsgAdapter.WriteInt(self.grid_list[i].id)
        MsgAdapter.WriteShort(self.grid_list[i].type < 0 and 0 or self.grid_list[i].type)
        MsgAdapter.WriteUChar(bit:b8d(self.grid_list[i].pet_exclusive_skill_disable_flag))
        MsgAdapter.WriteUChar(bit:b8d(self.grid_list[i].lian_yao_exclusive_skill_disable_flag))
        -- MsgAdapter.WriteUChar(0)
        -- MsgAdapter.WriteUChar(0)
        MsgAdapter.WriteUInt(self.grid_list[i].unique_id_1)
        MsgAdapter.WriteUInt(self.grid_list[i].unique_id_2)
        --LogError("2970 i",i,self.grid_list[i])
    end
    for i=1, 5 do
        MsgAdapter.WriteInt(self.standby_list[i].pet_id or 0)
        MsgAdapter.WriteUInt(self.standby_list[i].unique_id_1 or 0)
        MsgAdapter.WriteUInt(self.standby_list[i].unique_id_2 or 0)
        MsgAdapter.WriteUChar(0)
        MsgAdapter.WriteUChar(0)
        MsgAdapter.WriteShort(0)
        -- MsgAdapter.WriteUChar(self.standby_list[i].pet_exclusive_skill_disable_flag)
        -- MsgAdapter.WriteUChar(self.standby_list[i].lian_yao_exclusive_skill_disable_flag)
    end
    for i = 1,2 do
        MsgAdapter.WriteChar(self.battle_auto_skill_info[i].move_type)
        MsgAdapter.WriteChar(self.battle_auto_skill_info[i].skill_level or 1)
        MsgAdapter.WriteUShort(self.battle_auto_skill_info[i].skill_id)
    end

    -- LogError("2970 ",self)
end

SCPetHelperFormationInfo = SCPetHelperFormationInfo or BaseClass(BaseProtocolStruct)
function SCPetHelperFormationInfo:__init()
    self.msg_type = 2971
end

function SCPetHelperFormationInfo:Decode()
    self.system_type = MsgAdapter.ReadInt()
    self.grid_list = {}
    for i=1,10 do
        self.grid_list[i] = {}
        self.grid_list[i].id = MsgAdapter.ReadInt()         --主角uid/pet_id/partner_id
        self.grid_list[i].type = MsgAdapter.ReadShort()     --0为空 1主角 2宠物 3伙伴
        self.grid_list[i].pet_exclusive_skill_disable_flag = bit:d8b(MsgAdapter.ReadUChar())
        self.grid_list[i].lian_yao_exclusive_skill_disable_flag = bit:d8b(MsgAdapter.ReadUChar())
        self.grid_list[i].unique_id_1 = MsgAdapter.ReadUInt()   --宠物时为unique_id
        self.grid_list[i].unique_id_2 = MsgAdapter.ReadUInt()
    end
    self.standby_list = {}
    for i = 1, 5 do
        self.standby_list[i] = {}
        self.standby_list[i].pet_id = MsgAdapter.ReadInt()      --待机宠物id 值0这没有
        self.standby_list[i].unique_id_1 = MsgAdapter.ReadUInt()    --宠物唯一id
        self.standby_list[i].unique_id_2 = MsgAdapter.ReadUInt()
        MsgAdapter.ReadShort()
        MsgAdapter.ReadShort()
        -- self.standby_list[i].pet_exclusive_skill_disable_flag = bit:d8b(MsgAdapter.ReadUChar())
        -- self.standby_list[i].lian_yao_exclusive_skill_disable_flag = bit:d8b(MsgAdapter.ReadUChar())
    end
    self.skill_list = {}
    for i = 1, 2 do
        self.skill_list[i] = {}
        self.skill_list[i].move_type = MsgAdapter.ReadChar()
        self.skill_list[i].skill_level = MsgAdapter.ReadChar()
        self.skill_list[i].skill_id = MsgAdapter.ReadUShort()
    end
    -- LogError("2971",self)
end

CSPetHelperFormation = CSPetHelperFormation or BaseClass(BaseProtocolStruct)
function CSPetHelperFormation:__init()
    self.msg_type = 2972
end

function CSPetHelperFormation:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.system_type)
    --LogError("2972 ",self.system_type)
end