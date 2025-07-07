
function PetOtherInfo()
    local t = {}
    t.fight_index = MsgAdapter.ReadShort() -- 出战宠物index
    t.follow_index = MsgAdapter.ReadShort() -- 跟随宠物index
    t.standby_index1 = MsgAdapter.ReadShort() -- 待机宠物index
    t.standby_index2 = MsgAdapter.ReadShort() -- 待机宠物index
    t.standby_index3 = MsgAdapter.ReadShort() -- 待机宠物index
    t.extend_grid_num = MsgAdapter.ReadShort() -- 宠物列表扩展次数
    t.extend_times = MsgAdapter.ReadShort()
    t.standby_index4 = MsgAdapter.ReadShort()   -- 待机宠物index
    t.standby_index5 = MsgAdapter.ReadShort()   -- 待机宠物index
    
    t.standby_list = {}
    for i = 1, PetData.Instance:GetMaxPetOn() do
        t.standby_list[i] = t["standby_index" .. i] or -1
    end
    return t
end
CSPetOp = CSPetOp or BaseClass(BaseProtocolStruct)
function CSPetOp:__init()
    self.msg_type = 1700
end
function CSPetOp:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteInt(self.p1)
    MsgAdapter.WriteInt(self.p2)
    MsgAdapter.WriteInt(self.p3)
    MsgAdapter.WriteInt(self.p4)
    MsgAdapter.WriteInt(self.p5)
end

CSPetRename = CSPetRename or BaseClass(BaseProtocolStruct)
function CSPetRename:__init()
    self.msg_type = 1701
end
function CSPetRename:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.index)
    MsgAdapter.WriteStrN(self.name, PROTC_ROLE_NAME_LEN)
end

CSPetAssignAttributePoints = CSPetAssignAttributePoints or BaseClass(BaseProtocolStruct)
function CSPetAssignAttributePoints:__init()
    self.msg_type = 1702
end
function CSPetAssignAttributePoints:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.index)
    MsgAdapter.WriteInt(self.con)
    MsgAdapter.WriteInt(self.str)
    MsgAdapter.WriteInt(self.def)
    MsgAdapter.WriteInt(self.agi)
    MsgAdapter.WriteInt(self.magic)
end

SCSendPetAllInfo = SCSendPetAllInfo or BaseClass(BaseProtocolStruct)
function SCSendPetAllInfo:__init()
    self.msg_type = 1703
end
function SCSendPetAllInfo:Decode()
    self.reason = MsgAdapter.ReadInt()
    self.pet_count = MsgAdapter.ReadInt()
    self.pet_storage_count = MsgAdapter.ReadInt()
    self.pet_list = {}
    for i = 1, self.pet_count do
        local info = PortocolCommon.PetProParam()
        info.rank = MsgAdapter.ReadInt()
        local pet = Pet.New(info)
        if pet.config ~= nil then
            self.pet_list[info.index] = Pet.New(info)
        end
        -- self.pet_list[info.index] = Pet.New(info)
    end
    self.storage_list = {}
    for i = 1, self.pet_storage_count do
        local info = PortocolCommon.PetProParam()
        info.rank = MsgAdapter.ReadInt()
        local pet = Pet.New(info)
        if pet.config ~= nil then
            self.storage_list[info.index] = Pet.New(info)
        end
        -- self.storage_list[info.index] = Pet.New(info)
    end
end

SCSendPetSingleInfo = SCSendPetSingleInfo or BaseClass(BaseProtocolStruct)
function SCSendPetSingleInfo:__init()
    self.msg_type = 1704
end
function SCSendPetSingleInfo:Decode()
    self.notie_type = MsgAdapter.ReadInt()
    self.add_exp = MsgAdapter.ReadInt()
    local info = PortocolCommon.PetProParam()
    info.rank = MsgAdapter.ReadInt()
    self.pet_info = Pet.New(info)
    -- if self.notice_type == PetNotieType.PET_NOTIE_TYPE_LIAN_YAO then
    self.temp_pet = Pet.New(TableCopy(info))
    -- end
end

SCSendPetOtherInfo = SCSendPetOtherInfo or BaseClass(BaseProtocolStruct)
function SCSendPetOtherInfo:__init()
    self.msg_type = 1705
end
function SCSendPetOtherInfo:Decode()
    self.pet_other_info = PetOtherInfo()
end

SCSendPetDeleteInfo = SCSendPetDeleteInfo or BaseClass(BaseProtocolStruct)
function SCSendPetDeleteInfo:__init()
    self.msg_type = 1706
end
function SCSendPetDeleteInfo:Decode()
    self.pet_index = MsgAdapter.ReadInt()
end

SCSendPetAttrListInfo = SCSendPetAttrListInfo or BaseClass(BaseProtocolStruct)
function SCSendPetAttrListInfo:__init()
    self.msg_type = 1707
end
function SCSendPetAttrListInfo:Decode()
    self.attr_list = {}
    self.index = MsgAdapter.ReadInt()
    self.capaility = MsgAdapter.ReadInt()
    for i = 1, PET_ATTR_LIST_COUNT do
        self.attr_list[i - 1] = MsgAdapter.ReadInt()
    end
end
SCSendPetBreakthrougtsRet = SCSendPetBreakthrougtsRet or BaseClass(BaseProtocolStruct)
function SCSendPetBreakthrougtsRet:__init()
    self.msg_type = 1708
end
function SCSendPetBreakthrougtsRet:Decode()
    self.index = MsgAdapter.ReadInt()
    self.is_succ = MsgAdapter.ReadInt() --0失败 1成功
    self.old_str_level = MsgAdapter.ReadInt()
    self.is_lian_yao = MsgAdapter.ReadInt()
end

SCSendPetGetRet = SCSendPetGetRet or BaseClass(BaseProtocolStruct)
function SCSendPetGetRet:__init()
    self.msg_type = 1709
end
function SCSendPetGetRet:Decode()
    self.index = MsgAdapter.ReadInt()
    self.reason = MsgAdapter.ReadInt()
end

-- 伙伴系统操作
CSPartnerOperate = CSPartnerOperate or BaseClass(BaseProtocolStruct)
function CSPartnerOperate:__init()
    self.msg_type = 1710
end

function CSPartnerOperate:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.oper_type)
    MsgAdapter.WriteUShort(self.partner)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
end

-- 伙伴属性容器	- 格式
local function ReadPartnerInfo()
    local t = {}

    --列表
    t.partner = MsgAdapter.ReadUShort() -- 伙伴 ID
    t.is_unlocked = MsgAdapter.ReadChar() -- 解锁状态: 0 - 锁定; 1 - 解锁
    t.fight_state = MsgAdapter.ReadChar() -- 战斗状态: 0 - 待机; 1 - 出战
    --进阶
    t.up_level = MsgAdapter.ReadUChar() -- 进阶等级
    t.attr_stage = MsgAdapter.ReadChar() -- 属性已经突破的阶段, [0, 8],进阶成功后,此数据会被重置为0

    t.skill_list = {}
    for i = 1, PartnerConfig.PARTNER_SKILL_EN_MAX do
        t.skill_list[i] = MsgAdapter.ReadShort()
    end

    --情缘
    t.fetter_cur = {} -- 当前框伙伴情缘
    for i = 1, PartnerConfig.PARTNER_FETTER_MAX do
        t.fetter_cur[i] = MsgAdapter.ReadUChar()
    end
    t.fetter_wash = {} -- 洗练框伙伴情缘
    for i = 1, PartnerConfig.PARTNER_FETTER_MAX do
        t.fetter_wash[i] = MsgAdapter.ReadUChar()
    end

    --命格
    t.pes = {}
    for i = 1, PartnerConfig.PARTNER_PE_MAX do
        local data = {attr_list = {}}
        data.item_id = MsgAdapter.ReadUShort()
        data.unlock_level = MsgAdapter.ReadUShort()
        -- data.attr_type = MsgAdapter.ReadInt()
        -- data.attr_value = MsgAdapter.ReadInt()
        for i = 1, PartnerConfig.PARTNER_EQUIP_VOC_NUM do
            data.attr_list[i] = {
                attr_type = MsgAdapter.ReadInt(),
                attr_value = MsgAdapter.ReadInt(),
            }
        end
        t.pes[i] = data
    end

    --属性
    t.cap = MsgAdapter.ReadInt()
    t.quality_segment = MsgAdapter.ReadChar()
    -- t.is_arena_match = MsgAdapter.ReadChar() -- 是否守擂中(暂时保留字)
    t.task_finish = MsgAdapter.ReadChar() -- 是否完成进阶所需任务
    t.quality = MsgAdapter.ReadChar() -- resver_ch
    t.intensify_level = MsgAdapter.ReadChar() + 1   --强化等级
    -- t.count = MsgAdapter.ReadChar() -- 改变的属性数量
    
    t.attr_list = {}
    -- for i = 1, t.count do
    --     local type = MsgAdapter.ReadInt() -- 属性类型
    --     local value = MsgAdapter.ReadInt() -- 属性数值
    --     t.attr_list[type] = value
    -- end

    for i = 0, 49 do
        t.attr_list[i] = MsgAdapter.ReadInt() -- 属性数值
    end
    
    t.crystal = MsgAdapter.ReadInt() -- 装备元素水晶id
    t.used_times = MsgAdapter.ReadInt() -- 装备元素水晶剩余使用次数

    t.skin_id = MsgAdapter.ReadInt()    -- 穿戴的皮肤id 未穿戴发0
    t.level = MsgAdapter.ReadInt()    -- 等级

    t.biography_open_flag = MsgAdapter.ReadChar()
    t.biography_fetch_flag = MsgAdapter.ReadChar()
    t.soar_count = MsgAdapter.ReadShort()   -- 伙伴飞升次数

    t.contracts = {}
    for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
        t.contracts[i] = MsgAdapter.ReadUShort()
    end
    MsgAdapter.ReadShort()

    t.contract_attrs = {}   --  改伙伴用于给其契约目标增加的属性
    for i = 0, PartnerConfig.PARTNER_CONTRACT_ATTR_MAX - 1 do
        t.contract_attrs[i] = {
            attr_type = PartnerConfig.ContractI2Type[i],
            attr_value = MsgAdapter.ReadInt(),
        }
    end

    t.soar_weapon_level = MsgAdapter.ReadShort()    -- 伙伴飞升武器阶数
    t.rank = MsgAdapter.ReadShort() + 1             -- 本服排名 -1未上榜 0开始
    
    return t
end

-- 所有伙伴信息
SCPartnerAllInfo = SCPartnerAllInfo or BaseClass(BaseProtocolStruct)
function SCPartnerAllInfo:__init()
    self.msg_type = 1711
end

function SCPartnerAllInfo:Decode()
    self.count = MsgAdapter.ReadInt() -- 伙伴列表长度[1,30] - 当前配置表拥有的伙伴数量
    self.partner_list = {}
    for i = 1, self.count do
        local info = ReadPartnerInfo()
        self.partner_list[info.partner] = info
    end
end

-- 单个伙伴信息
SCPartnerSingleInfo = SCPartnerSingleInfo or BaseClass(BaseProtocolStruct)
function SCPartnerSingleInfo:__init()
    self.msg_type = 2022
end

function SCPartnerSingleInfo:Decode()
    self.partner_info = ReadPartnerInfo()
end

-- 改变伙伴出战状态协议回复
SCChangeFightStateReq = SCChangeFightStateReq or BaseClass(BaseProtocolStruct)
function SCChangeFightStateReq:__init()
    self.msg_type = 1712
end

function SCChangeFightStateReq:Decode()
    self.partner = MsgAdapter.ReadUShort() -- 对应的伙伴
    self.is_fight = MsgAdapter.ReadChar() -- 出战状态: 0 - 待机; 1 - 出战
    self.is_show = MsgAdapter.ReadChar() -- 解锁界面: 1 - 展示
    -- self.is_arena_match = MsgAdapter.ReadInt() -- 是否守擂中
end

-- 伙伴属性列表
SCPartnerAttrListInfo = SCPartnerAttrListInfo or BaseClass(BaseProtocolStruct)
function SCPartnerAttrListInfo:__init()
    self.msg_type = 1713
end

function SCPartnerAttrListInfo:Decode()
    self.partner = MsgAdapter.ReadUShort() -- 对应的伙伴
    self.count = MsgAdapter.ReadChar() -- 改变的属性数量
    self.notice_type = MsgAdapter.ReadChar()
    self.cap = MsgAdapter.ReadInt()
    self.attr_list = {}
    for i = 1, self.count do
        local type = MsgAdapter.ReadInt() -- 属性类型
        local value = MsgAdapter.ReadInt() -- 属性数值
        self.attr_list[type] = value
    end
end

-- 羁绊界面信息回复
SCPartnerFetter = SCPartnerFetter or BaseClass(BaseProtocolStruct)
function SCPartnerFetter:__init()
    self.msg_type = 1714
end

function SCPartnerFetter:Decode()
    self.partner = MsgAdapter.ReadUShort() -- 对应的伙伴
    MsgAdapter.ReadShort() -- resver_sh

    self.fetter_cur = {} -- 当前框羁绊
    for i = 1, PartnerConfig.PARTNER_FETTER_MAX do
        self.fetter_cur[i] = MsgAdapter.ReadUChar()
    end

    self.fetter_wash = {} -- 洗练框羁绊
    for i = 1, PartnerConfig.PARTNER_FETTER_MAX do
        self.fetter_wash[i] = MsgAdapter.ReadUChar()
    end
end

-- 伙伴基础阶级信息
SCPartnerBaseGradeInfo = SCPartnerBaseGradeInfo or BaseClass(BaseProtocolStruct)
function SCPartnerBaseGradeInfo:__init()
    self.msg_type = 1715
end

function SCPartnerBaseGradeInfo:Decode()
    self.partner = MsgAdapter.ReadUShort() -- 伙伴
    self.up_level = MsgAdapter.ReadChar() -- 等级
    self.attr_stage = MsgAdapter.ReadChar() -- 突破阶段 - [0, 7]

    self.up_ret = MsgAdapter.ReadChar() -- 进阶标记 1 - 进阶成功
    self.task_finish = MsgAdapter.ReadChar() -- 是否完成进阶所需任务
    MsgAdapter.ReadShort() -- resver_sh
end

-- 保存所选的伙伴技能
CSPartnerToSaveSkillSelection = CSPartnerToSaveSkillSelection or BaseClass(BaseProtocolStruct)
function CSPartnerToSaveSkillSelection:__init()
    self.msg_type = 1716
end

function CSPartnerToSaveSkillSelection:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUShort(self.partner) -- 伙伴ID
    MsgAdapter.WriteShort(self.index - 1) -- 技能 0 - 2
    MsgAdapter.WriteShort(self.skill_id) -- 技能ID
    MsgAdapter.WriteShort(0)
end

-- 伙伴技能启用/禁用
CSPartnerSkillActivate = CSPartnerSkillActivate or BaseClass(BaseProtocolStruct)
function CSPartnerSkillActivate:__init()
    self.msg_type = 2049
end

function CSPartnerSkillActivate:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.partner) -- 伙伴ID
    MsgAdapter.WriteShort(self.is_active) -- 0禁用 1启用
    MsgAdapter.WriteShort(self.index - 1) -- 技能 0 - 2
end

-- 伙伴当前技能通知
SCPartnerSkillReq = SCPartnerSkillReq or BaseClass(BaseProtocolStruct)
function SCPartnerSkillReq:__init()
    self.msg_type = 1717
end

function SCPartnerSkillReq:Decode()
    self.partner = MsgAdapter.ReadUShort() -- 伙伴ID

    self.skill_list = {}
    for i = 1, PartnerConfig.PARTNER_SKILL_EN_MAX do
        self.skill_list[i] = MsgAdapter.ReadShort()
    end
end

-- 出战伙伴顺序信息
SCSynFightPartnerOrder = SCSynFightPartnerOrder or BaseClass(BaseProtocolStruct)
function SCSynFightPartnerOrder:__init()
    self.msg_type = 1718
end

function SCSynFightPartnerOrder:Decode()
    self.fight_num = MsgAdapter.ReadInt()
    self.fight_list = {}
    for i = 1, self.fight_num do
        self.fight_list[i] = MsgAdapter.ReadInt()
    end
end

CSChangeSyncFightOrder = CSChangeSyncFightOrder or BaseClass(BaseProtocolStruct)
function CSChangeSyncFightOrder:__init()
    self.msg_type = 1719
end

function CSChangeSyncFightOrder:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUShort(self.insert_partner) -- 将要插入的伙伴ID
    MsgAdapter.WriteUShort(self.target_partner) -- 将要变位的伙伴ID
end

CSPartnerEqOperatorReq = CSPartnerEqOperatorReq or BaseClass(BaseProtocolStruct)
function CSPartnerEqOperatorReq:__init()
    self.msg_type = 1770
end

function CSPartnerEqOperatorReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUShort(self.op_type) -- 操作类型
    MsgAdapter.WriteUShort(self.partner) -- 伙伴ID
    MsgAdapter.WriteInt(self.param1) -- 参数1 格子ID
end

-- 单个伙伴装备槽信息
SCPartnerEqSingleSlots = SCPartnerEqSingleSlots or BaseClass(BaseProtocolStruct)
function SCPartnerEqSingleSlots:__init()
    self.msg_type = 1771
end

function SCPartnerEqSingleSlots:Decode()
    self.partner = MsgAdapter.ReadUShort()
    self.equip_slot = MsgAdapter.ReadUShort()
    local t = {}
    t.item_id = MsgAdapter.ReadUShort()
    t.unlock_level = MsgAdapter.ReadUShort()
    -- t.attr_type = MsgAdapter.ReadInt()
    -- t.attr_value = MsgAdapter.ReadInt()
    t.attr_list = {}
    for i = 1, PartnerConfig.PARTNER_EQUIP_VOC_NUM do
        t.attr_list[i] = {
            attr_type = MsgAdapter.ReadInt(),
            attr_value = MsgAdapter.ReadInt(),
        }
    end
    self.pe = t
end

-- 伙伴升品下发
SCPartnerQualityUp = SCPartnerQualityUp or BaseClass(BaseProtocolStruct)
function SCPartnerQualityUp:__init()
    self.msg_type = 1772
end 

function SCPartnerQualityUp:Decode()
    self.partner_id = MsgAdapter.ReadInt()     -- 伙伴id
    self.up_type = MsgAdapter.ReadInt()        -- 升品类型
    self.quality = MsgAdapter.ReadInt()        -- 品质
    self.attr_list = {}                        -- 属性值
    for i = 0 ,PartnerConfig.PARTNER_ATTR_MAX - 1 do 
        self.attr_list[i] = MsgAdapter.ReadInt()
    end 
    self.notice_type = MsgAdapter.ReadChar()    -- 下发类型 0、初始化下发；1、升品下发；2、激活伙伴；3、寻宝激活
    self.quality_segment = MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    
end 

-- 伙伴升品请求
CSPartnerQualityUpReq = CSPartnerQualityUpReq or BaseClass(BaseProtocolStruct)
function CSPartnerQualityUpReq:__init()
    self.msg_type = 1773
end

function CSPartnerQualityUpReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.use_type)    -- 升品主动道具类型
    MsgAdapter.WriteInt(self.up_type)     -- 升品类型
    MsgAdapter.WriteInt(self.partner_id)  -- 伙伴id
end

-- 伙伴皮肤激活总信息下发
SCPartnerSkinActive = SCPartnerSkinActive or BaseClass(BaseProtocolStruct)
function SCPartnerSkinActive:__init()
    self.msg_type = 1777
end 

function SCPartnerSkinActive:Decode()
    self.count = MsgAdapter.ReadInt()
    local skin_list = {}
    for i = 1, self.count do
        local partner_id = MsgAdapter.ReadShort()
        local skin_active_status = MsgAdapter.ReadShort()
        skin_list[partner_id] = skin_active_status
    end
    self.skin_list = skin_list
end

-- 伙伴皮肤单个信息下发
SCPartnerSkinSingleOper = SCPartnerSkinSingleOper or BaseClass(BaseProtocolStruct)
function SCPartnerSkinSingleOper:__init()
    self.msg_type = 1778
end 

function SCPartnerSkinSingleOper:Decode()
    self.info = {
        type = MsgAdapter.ReadInt(),
        partner_id = MsgAdapter.ReadInt(),
        skin_id = MsgAdapter.ReadInt(),
    }
end

-- 伙伴皮肤请求
CSPartnerSkinReq = CSPartnerSkinReq or BaseClass(BaseProtocolStruct)
function CSPartnerSkinReq:__init()
    self.msg_type = 1779
end

function CSPartnerSkinReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.oper_type)
    MsgAdapter.WriteInt(self.partner_id)
    MsgAdapter.WriteInt(self.skin_id)
end

-- 下发伙伴飞升相关通知
SCPartnerSoarNotice = SCPartnerSoarNotice or BaseClass(BaseProtocolStruct)
function SCPartnerSoarNotice:__init()
    self.msg_type = 2028
end 

function SCPartnerSoarNotice:Decode()
    self.notice_type = MsgAdapter.ReadInt()
    self.partner = MsgAdapter.ReadInt()
    self.soar_count = MsgAdapter.ReadShort()
    self.soar_weapon_level = MsgAdapter.ReadShort()
end

-- 单个任务操作请求
CSTaskInfoSingleHandle = CSTaskInfoSingleHandle or BaseClass(BaseProtocolStruct)
function CSTaskInfoSingleHandle:__init()
    self.msg_type = 1720
end

function CSTaskInfoSingleHandle:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUShort(self.oper_type) -- 操作类型
    MsgAdapter.WriteUShort(self.task_id) -- 任务ID
end

SCTaskCompleteTaskList = SCTaskCompleteTaskList or BaseClass(BaseProtocolStruct)
function SCTaskCompleteTaskList:__init()
    self.msg_type = 1725
end

function SCTaskCompleteTaskList:Decode()
    self.list_count = MsgAdapter.ReadInt()
    self.complete_task_list = {}
    self.task_complete_list = {}
    for i = 1, self.list_count do
        self.complete_task_list[i] = MsgAdapter.ReadUShort()
        self.task_complete_list[self.complete_task_list[i]] = 1
    end
end

-- 可接和已接的信息总表
SCTaskDetailedInfoList = SCTaskDetailedInfoList or BaseClass(BaseProtocolStruct)
function SCTaskDetailedInfoList:__init()
    self.msg_type = 1721
end

function SCTaskDetailedInfoList:Decode()
    self.task_count = MsgAdapter.ReadInt() -- 任务数量
    self.task_list = {}
    self.task_id_accepted_list = {}
    for i = 1, self.task_count do
        local data = {}
        data.task_id = MsgAdapter.ReadUShort() -- 任务ID
        data.state = MsgAdapter.ReadChar() -- 状态类型 0 - 进行中 1 - 可提交
        data.progress = MsgAdapter.ReadUChar() -- 任务进度
        data.expire_time = MsgAdapter.ReadUInt() -- 任务到期时间
        data.list_type = MsgAdapter.ReadInt() -- 任务列表类型 1可接 0已接受
        self.task_list[i] = data
        if data.list_type == 0 then
            self.task_id_accepted_list[data.task_id] = 1
        end
        if Config.feisheng_tasklist_auto.task_list[data.task_id] then
            -- LogError("可接和已接的信息总表",data)
        end
    end
end

-- 从总表中移除任务信息
SCTaskDeleteNotifyList = SCTaskDeleteNotifyList or BaseClass(BaseProtocolStruct)
function SCTaskDeleteNotifyList:__init()
    self.msg_type = 1722
end

function SCTaskDeleteNotifyList:Decode()
    self.task_count = MsgAdapter.ReadInt() -- 任务数量
    self.task_list = {}
    for i = 1, self.task_count do
        local data = {}
        data.reason = MsgAdapter.ReadShort() -- 移除原因 0 其他原因 1 完成
        data.task_id = MsgAdapter.ReadUShort()
        self.task_list[i] = data
    end
end

-- 与任务指定的NPC交谈
CSTalkWithAppointedNPC = CSTalkWithAppointedNPC or BaseClass(BaseProtocolStruct)
function CSTalkWithAppointedNPC:__init()
    self.msg_type = 1723
end

function CSTalkWithAppointedNPC:Encode()
    MsgAdapter.WriteUShort(self.task_id) -- 任务ID
    MsgAdapter.WriteUShort(self.npc_seq) -- NPC_SEQ
end

-- 提交道具(列表)
CSGiveNPCTaskNeedItems = CSGiveNPCTaskNeedItems or BaseClass(BaseProtocolStruct)
function CSGiveNPCTaskNeedItems:__init()
    self.msg_type = 1724
end

function CSGiveNPCTaskNeedItems:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUShort(self.task_id) -- 任务ID
    MsgAdapter.WriteUShort(#self.commit_list) -- 提交的道具种数
    for i = 1, #self.commit_list do
        local item_data = self.commit_list[i]
        MsgAdapter.WriteUShort(item_data.item_id or 0) -- 道具ID
        MsgAdapter.WriteUShort(item_data.sub_num or 0) -- 提交的道具数量
    end
end

-- 提交货币
CSTaskCommitMoneyReq = CSTaskCommitMoneyReq or BaseClass(BaseProtocolStruct)
function CSTaskCommitMoneyReq:__init(  )
    self.msg_type = 2029
end

function CSTaskCommitMoneyReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.task_id)
end

-- 任务能否被放弃
SCTaskCanAbandonedList = SCTaskCanAbandonedList or BaseClass(BaseProtocolStruct)
function SCTaskCanAbandonedList:__init()
    self.msg_type = 1726
end

function SCTaskCanAbandonedList:Decode()
    self.task_list = {}
    self.list_num = MsgAdapter.ReadInt()
    for i = 1, self.list_num do
        local data = {
            task_id = MsgAdapter.ReadUShort(),
            task_state = MsgAdapter.ReadUShort() -- 0 - 不能; 1 - 可以
        }
        self.task_list[data.task_id] = data
    end
end

-- 单个任务当前状态
SCTaskSingleStatusInfo = SCTaskSingleStatusInfo or BaseClass(BaseProtocolStruct)
function SCTaskSingleStatusInfo:__init()
    self.msg_type = 1727
end

function SCTaskSingleStatusInfo:Decode()
    self.task_info = {
        task_id = MsgAdapter.ReadUShort(),
        task_state = MsgAdapter.ReadUShort() -- 0 - 未进行; 1 - 已完成; 2 - 进行中
    }
end

CSWorkshopOperateRequire = CSWorkshopOperateRequire or BaseClass(BaseProtocolStruct)
function CSWorkshopOperateRequire:__init()
    self.msg_type = 1730
end
function CSWorkshopOperateRequire:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.op_type)
    MsgAdapter.WriteShort(self.param1)
    MsgAdapter.WriteShort(self.param2)
    MsgAdapter.WriteShort(self.param3)
    MsgAdapter.WriteShort(self.param4)
    MsgAdapter.WriteShort(self.param5)
    MsgAdapter.WriteShort(self.param6)
    MsgAdapter.WriteShort(self.reserve)
end

SCWorkshopAllInfoRespond = SCWorkshopAllInfoRespond or BaseClass(BaseProtocolStruct)
function SCWorkshopAllInfoRespond:__init()
    self.msg_type = 1731
end
--BitMap
function SCWorkshopAllInfoRespond:Decode()
    self.voc_refresh_times = MsgAdapter.ReadInt()
    self.sp_active_flag = MsgAdapter.IntFlag(160)
	self.sp_created_flag = MsgAdapter.IntFlag(160)
end

SCSingleRecipeActiveFlag = SCSingleRecipeActiveFlag or BaseClass(BaseProtocolStruct)
function SCSingleRecipeActiveFlag:__init()
    self.msg_type = 1732
end
function SCSingleRecipeActiveFlag:Decode()
    self.active_id = MsgAdapter.ReadUShort() -- 激活标识号
    self.is_active = MsgAdapter.ReadChar() -- 是否激活： 0 - 未激活； 1 - 激活
	self.is_notice = MsgAdapter.ReadChar() -- 是否弹窗： 0 - 不弹； 1 - 弹窗提示
end
local function AttrInfo()
    local t = {
        attr_type = MsgAdapter.ReadShort(),
        attr_value = MsgAdapter.ReadShort()
    }
    return t
end

local function JewelryParam()
    local t = {}
    t.lyrics_id = MsgAdapter.ReadShort() --词牌名
    MsgAdapter.ReadShort()
    t.anima_val = MsgAdapter.ReadInt() --灵气值
    t.attr_pair = AttrInfo()
    t.attr_pair2 = AttrInfo()
    t.lege_attr = AttrInfo()
    t.creater_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    return t
end
--[[ local function NewJewelryParam()
	local t = {}
	t.has_random = MsgAdapter.ReadChar()
	t.is_treasure = MsgAdapter.ReadChar()
	t.attr_num = MsgAdapter.ReadShort()
	t.attr_list = {}
	for i = 1, EQUIP_ITEM_ATTR_NUM do
		t.attr_list[i] = AttrInfo()
		t.attr_list[i].index = i
	end
	t.creater_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	t.inc_num = MsgAdapter.ReadShort()
	t.attr_extra_color = {}
	for i = 1, EQUIP_ITEM_ATTR_NUM do
		t.attr_extra_color[i] = MsgAdapter.ReadChar()
	end
	t.attr_extra_inc = {}
	for i = 1, EQUIP_ITEM_ATTR_NUM do
		t.attr_extra_inc[i] = AttrInfo()
		t.attr_extra_inc[i].index = i
		t.attr_extra_inc[i].color = t.attr_extra_color[i]
    end
    return t
end ]]
function JewelryPro() -- 灵饰 信息
    local t = {}
    t.jewelry_type = MsgAdapter.ReadShort() -- 灵饰 类型
    t.item_id = MsgAdapter.ReadUShort() -- 道具ID
    -- t.item_unique_id = MsgAdapter.ReadLL() -- 唯一id  
    t.item_unique_id_1 = MsgAdapter.ReadUInt()  
    t.item_unique_id_2 = MsgAdapter.ReadUInt()
    t.param = NewJewelryParam()
    t.is_bind = MsgAdapter.ReadInt()
     --JewelryParam() -- 净值数据
    t.index = t.jewelry_type
    return t
end
local function JewelryReforgeCon()
    local t = {}
    t.jewelry_id = MsgAdapter.ReadUShort()
    MsgAdapter.ReadShort()
    self.param = JewelryParam()
end

SCJewelryReforgeOutcomes = SCJewelryReforgeOutcomes or BaseClass(BaseProtocolStruct)
function SCJewelryReforgeOutcomes:__init()
    self.msg_type = 1733
end
function SCJewelryReforgeOutcomes:Decode()
    self.reforge_list = {}
    for i = 1, JEWELTY_REFORGE_LIST_MAX do
        local temp = JewelryPro()
        if temp.item_id == 0 then
            self.reforge_list[i] = nil
        else
            self.reforge_list[i] = Jewelry.New(temp)
        end
    end
end

local GemUpgradedata = function()
    local function own_func(list_num)
        local list = {}
        for i = 1, list_num do
            list[i] = {
                item_id = MsgAdapter.ReadUShort(),
                num = MsgAdapter.ReadShort()
            }
        end
        return list
    end
    local t = {
        target_gem = MsgAdapter.ReadUShort(),
        fin_gem_id = MsgAdapter.ReadUShort(),
        fin_need_num = MsgAdapter.ReadInt(),
        money_type = MsgAdapter.ReadInt(),
        consume_coins = MsgAdapter.ReadInt(),
        list_num = MsgAdapter.ReadInt()
    }
    t.own_list = own_func(t.list_num)
    return t
end
SCGemOneKeyUpgradesNeeds = SCGemOneKeyUpgradesNeeds or BaseClass(BaseProtocolStruct)
function SCGemOneKeyUpgradesNeeds:__init()
    self.msg_type = 1734
end
function SCGemOneKeyUpgradesNeeds:Decode()
    self.mate_data = GemUpgradedata()
end

local function GridPair(info) --ItemPairCon
    MsgAdapter.WriteUShort(info.item_id)
    MsgAdapter.WriteShort(info.num)
end
local function ItemPairCon()
    return {
        item_id = MsgAdapter.ReadUShort(),
        num = MsgAdapter.ReadShort()
    }
end
CSConvertPetSkillBookSeq = CSConvertPetSkillBookSeq or BaseClass(BaseProtocolStruct)
function CSConvertPetSkillBookSeq:__init()
    self.msg_type = 1735
end
function CSConvertPetSkillBookSeq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUShort(self.exc_seq)
    MsgAdapter.WriteUShort(self.list_num)
    for k, v in pairs(self.item_list) do
        GridPair(v)
    end
end
SCWorkshopComposeResults = SCWorkshopComposeResults or BaseClass(BaseProtocolStruct)
function SCWorkshopComposeResults:__init()
    self.msg_type = 1736
end
function SCWorkshopComposeResults:Decode()
    self.result = MsgAdapter.ReadChar() -- 0失败
    self.item = {}
    self.item.has_param = MsgAdapter.ReadChar() ~= 0
    -- self.item_info = ItemPairCon()
    self.item.item_id = MsgAdapter.ReadUShort()
    self.item.num = MsgAdapter.ReadUShort()
    if self.item.has_param then
        self.item.param_len = MsgAdapter.ReadUShort()
        self.item.param = ItemDataParam(self.item.item_id, self.item.param_len)
    end
end

CSChangeEquipVocReq = CSChangeEquipVocReq or BaseClass(BaseProtocolStruct)
function CSChangeEquipVocReq:__init()
    self.msg_type = 1737
end
function CSChangeEquipVocReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.change_voc_type) --类型 0装备 1 灵饰 2淬炼装备 3淬炼灵饰
    MsgAdapter.WriteInt(self.role_type)
    --获取词条的装备
    MsgAdapter.WriteInt(self.replacer_posi)
     --是否装备 1背包0装备上
    MsgAdapter.WriteInt(self.replacer_grid)
     --格子位置
    MsgAdapter.WriteInt(self.replacer_voc_index)
     --替换位置 0-5
    --摘取词条装备
    MsgAdapter.WriteInt(self.bereplacer_posi)
    MsgAdapter.WriteInt(self.bereplacer_grid)
    MsgAdapter.WriteInt(self.bereplacer_voc_index)
end

SCWorkShopJewelryInfo = SCWorkShopJewelryInfo or BaseClass(BaseProtocolStruct)
function SCWorkShopJewelryInfo:__init()
    self.msg_type = 1738
end
function SCWorkShopJewelryInfo:Decode()
    self.jewelry_times = {}
    for i = 0, INTJEWELRY_SUCC_MAX_GROUP - 1 do
        self.jewelry_times[i] = MsgAdapter.ReadInt()
    end
end

CSMtPetEquipmentReq = CSMtPetEquipmentReq or BaseClass(BaseProtocolStruct)
function CSMtPetEquipmentReq:__init()
    self.msg_type = 1739
end
function CSMtPetEquipmentReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
end

CSJewelryHandleReq = CSJewelryHandleReq or BaseClass(BaseProtocolStruct)
function CSJewelryHandleReq:__init()
    self.msg_type = 1740
end
function CSJewelryHandleReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.op_type)
    MsgAdapter.WriteShort(self.param) --0无 1 背包给子 2 灵饰装备格子
end

SCJewelryAllInform = SCJewelryAllInform or BaseClass(BaseProtocolStruct)
function SCJewelryAllInform:__init()
    self.msg_type = 1741
end
function SCJewelryAllInform:Decode()
    self.jewelry_count = MsgAdapter.ReadInt()
    self.jewelry_list = {}
    for i = 1, self.jewelry_count do
        local info = JewelryPro()
        self.jewelry_list[info.jewelry_type] = info
    end
end
SCJewelryInfoAlter = SCJewelryInfoAlter or BaseClass(BaseProtocolStruct)
function SCJewelryInfoAlter:__init()
    self.msg_type = 1742
end
function SCJewelryInfoAlter:Decode()
    self.reason = MsgAdapter.ReadInt()
    self.jewelry = JewelryPro()
end

--op_type=0 请求战斗 param1=怪物组
CSNPCHandleRequest = CSNPCHandleRequest or BaseClass(BaseProtocolStruct)
function CSNPCHandleRequest:__init()
    self.msg_type = 1760
    self.op_type = 0
    self.obj_id = 0
    self.npc_seq = 0
    self.param1 = 0
    self.param2 = 0
end

function CSNPCHandleRequest:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.op_type) --0:请求战斗,1:购买物品,2:请求采集 3:采集完毕 4:对话NPC(只有特殊商人NPC 互动NPC才可以使用)
    MsgAdapter.WriteShort(self.obj_id)
    MsgAdapter.WriteInt(self.npc_seq)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end
-- 开始采集
SCNPCStartToGather = SCNPCStartToGather or BaseClass(BaseProtocolStruct)
function SCNPCStartToGather:__init()
    self.msg_type = 1761
end
function SCNPCStartToGather:Decode()
    self.need_time = MsgAdapter.ReadUInt()
end
--取消采集行为
SCNPCStopGathering = SCNPCStopGathering or BaseClass(BaseProtocolStruct)
function SCNPCStopGathering:__init()
    self.msg_type = 1762
end
function SCNPCStopGathering:Decode()
    self.role_obj_id = MsgAdapter.ReadInt()
    self.gather_seq = MsgAdapter.ReadInt()
    self.reasons = MsgAdapter.ReadInt()
end
-- 采集广播(通知周围玩家)
SCNPCGathersNotify = SCNPCGathersNotify or BaseClass()
function SCNPCGathersNotify:__init()
    self.msg_type = 1763
end
function SCNPCGathersNotify:Decode()
    self.role_obj_id = MsgAdapter.ReadInt()
    self.gather_seq = MsgAdapter.ReadInt()
    self.npc_id = MsgAdapter.ReadInt()
end

SCNPCInfoNotify = SCNPCInfoNotify or BaseClass()
function SCNPCInfoNotify:__init()
    self.msg_type = 1765
end
function SCNPCInfoNotify:Decode()
    self.obj_id = MsgAdapter.ReadInt()
    self.param1 = MsgAdapter.ReadInt()
    self.param2 = MsgAdapter.ReadInt()
    self.param3 = MsgAdapter.ReadInt()
end

TASK_CHAIN_TASK_TYPE = {
    -- 交谈 npc_id
    TALK = 1,
    -- 交货 npc_id|item_id|num|drop_scene_id|drop_scene|drop_rate|has_drop
    ITEM = 2,
    -- 交手 monster_id|scene_id|x*10000+y|monster_group_id|jump_cost_type|jump_cost_num
    FIGHT = 3,
    -- 交宠 npc_id|pet_id|pet_item
    PET = 4,
    -- 等待 npc_id|item_id|time_limit|pay_npc_id|reputaion_city|reputation_num
    WAIT = 5,
}

TASK_CHAIN_PARAM_COUNT = 10

SCTaskChainInfo = SCTaskChainInfo or BaseClass(BaseProtocolStruct)
function SCTaskChainInfo:__init()
    self.msg_type = 1729
end

function SCTaskChainInfo:Decode()
    local info = {}
    -- 完成次数
    info.pass_count = MsgAdapter.ReadInt()
    -- 当前任务类型
    info.task_type = MsgAdapter.ReadInt()
    -- 任务参数
    info.task_param = {}
    for i = 1, TASK_CHAIN_PARAM_COUNT do
        info.task_param[i] = MsgAdapter.ReadInt()
    end
    info.can_skip = MsgAdapter.ReadInt()
    info.price_type = MsgAdapter.ReadInt()
    info.price = MsgAdapter.ReadInt()
    info.task_count = MsgAdapter.ReadInt()--阶段
    info.is_new = MsgAdapter.ReadShort()
    info.level_times_count = MsgAdapter.ReadShort()--玩家当前所能做的总环数
    info.task_limit_timestamp = MsgAdapter.ReadLL()--特殊时间限制戳
    info.task_pass_count = MsgAdapter.ReadShort()--本阶段任务进度
    info.cur_task_max_count = MsgAdapter.ReadShort()--本阶段最大环数
    info.accept_task_level = MsgAdapter.ReadInt()--每次接取任务的等级
    self.info = info
end

TASK_CHAIN_OP_TYPE = {
    ACCEPT = 0,			    -- 接取
    GIVEUP = 1,			    -- 放弃
    ITEM = 2,		        -- 交货	column|index
    TALK = 3,               -- 交谈
    SKIP = 4,               -- 跳过（只有战斗可以）
    PET = 5,                -- 上交宠物 pet_index
    FETCH_REWARD = 6,       -- 领取转盘奖励
    WAIT_ITEM = 7,          -- 领取代送道具
}

CSTaskChainOp = CSTaskChainOp or BaseClass(BaseProtocolStruct)
function CSTaskChainOp:__init()
    self.msg_type = 1728
end

function CSTaskChainOp:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

CSOGOperatorRequest = CSOGOperatorRequest or BaseClass(BaseProtocolStruct)

function CSOGOperatorRequest:__init()
    self.msg_type = 1780
    self.param = 0
end
--领取奖励 op_type:0 param seq
--打守星BOSS op_type:1 param \
--领取BOSS奖励 op_type:2 param \
function CSOGOperatorRequest:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.op_type) --请求 0:领取奖励
    MsgAdapter.WriteShort(self.param) --参数 seq
end

SCCOGAllInformations = SCCOGAllInformations or BaseClass(BaseProtocolStruct)
function SCCOGAllInformations:__init()
    self.msg_type = 1781
end

--GROWTH_TASK_NUM_PER_STAR 9
function SCCOGAllInformations:Decode()
    local t = {}
    self.cur_prog = MsgAdapter.ReadShort()
    self.cur_star = MsgAdapter.ReadShort()
    self.cur_reward_prog = MsgAdapter.ReadUShort() --stage
    self.cur_reward_star = MsgAdapter.ReadUShort() --star

    self.task_prog_list = {}
    for i = 1, AchieveNewData.StarAchieveCount do
        self.task_prog_list[i] = MsgAdapter.ReadUInt()
    end
    --9个bool，为true表示已经领了奖励
    local flag = {}
    for i=1,2 do
        local arge = bit:d8b(MsgAdapter.ReadUChar())
        for n = 1, 8 do
			flag[n + 8 * (i - 1)] = arge[9 - n]
		end
    end
    self.award_fetch_flag = flag
end

--单个进度通知
SCCOGSingleTaskProgs = SCCOGSingleTaskProgs or BaseClass(BaseProtocolStruct)
function SCCOGSingleTaskProgs:__init()
    self.msg_type = 1782
end

function SCCOGSingleTaskProgs:Decode()
    self.seq = MsgAdapter.ReadInt() --[0,8]
    self.prog_num = MsgAdapter.ReadUInt()
end

--奖励领取通知
SCCOGRewardFetchFlag = SCCOGRewardFetchFlag or BaseClass(BaseProtocolStruct)
function SCCOGRewardFetchFlag:__init()
    self.msg_type = 1783
end

function SCCOGRewardFetchFlag:Decode()
    self.seq = MsgAdapter.ReadUShort() --[0,8]
    self.flag = MsgAdapter.ReadUShort() ~= 0
end

--当前星级奖励阶段
SCCOGStarAwardStatus = SCCOGStarAwardStatus or BaseClass(BaseProtocolStruct)
function SCCOGStarAwardStatus:__init()
    self.msg_type = 1784
end

function SCCOGStarAwardStatus:Decode()
    self.cur_reward_prog = MsgAdapter.ReadUShort() --stage
    self.cur_reward_star = MsgAdapter.ReadUShort() --star [1,5]
end

--当前星级奖励阶段
SCCOGSingleSeqNotify = SCCOGSingleSeqNotify or BaseClass(BaseProtocolStruct)
function SCCOGSingleSeqNotify:__init()
    self.msg_type = 1785
end

function SCCOGSingleSeqNotify:Decode()
    self.notify_seq = MsgAdapter.ReadInt() --当前阶段任务游标[0,8]
end

--===========================颜值系统=============================
--颜值系统 操作请求
CSFaceScoreRequest = CSFaceScoreRequest or BaseClass(BaseProtocolStruct)

function CSFaceScoreRequest:__init()
    self.msg_type = 1790
end

function CSFaceScoreRequest:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

--颜值系统 所有信息
SCFaceScoreAllInfo = SCFaceScoreAllInfo or BaseClass(BaseProtocolStruct)
function SCFaceScoreAllInfo:__init()
    self.msg_type = 1791
end

function SCFaceScoreAllInfo:Decode()
    self.face_level = MsgAdapter.ReadInt()
    self.cur_exp = MsgAdapter.ReadInt()
    self.equip_list = {}
    for i = 1, FACE_ADDITION_TYPE_MAX do
        local res_seq = MsgAdapter.ReadInt()
        local config = Config.face_cfg_auto.resource[res_seq]
        if config then
            self.equip_list[config.res_type] = config
        end
    end
    self.active_flag = MsgAdapter.IntFlag(FACE_ADDITION_FLAG_MAX) --1024位 二进制
end

--颜值系统 单个格子信息 装备信息下发
SCFaceListGridInfo = SCFaceListGridInfo or BaseClass(BaseProtocolStruct)
function SCFaceListGridInfo:__init()
    self.msg_type = 1792
end

function SCFaceListGridInfo:Decode()
    self.res_type = MsgAdapter.ReadInt()
    self.res_seq = MsgAdapter.ReadInt()
end

--颜值系统 单个资源信息
SCFaceResStateInfo = SCFaceResStateInfo or BaseClass(BaseProtocolStruct)
function SCFaceResStateInfo:__init()
    self.msg_type = 1793
end

function SCFaceResStateInfo:Decode()
    self.res_seq = MsgAdapter.ReadInt()
    self.status = MsgAdapter.ReadInt()
end

--颜值系统 等级经验
SCFaceLevelAndExps = SCFaceLevelAndExps or BaseClass(BaseProtocolStruct)
function SCFaceLevelAndExps:__init()
    self.msg_type = 1794
end

function SCFaceLevelAndExps:Decode()
    self.face_level = MsgAdapter.ReadInt()
    self.cur_exp = MsgAdapter.ReadInt()
end

--颜值系统 单个道具时间戳下发
SCFaceTimeSingleInfo = SCFaceTimeSingleInfo or BaseClass(BaseProtocolStruct)
function SCFaceTimeSingleInfo:__init()
    self.msg_type = 1795
end

function SCFaceTimeSingleInfo:Decode()
    self.seq = MsgAdapter.ReadInt()
    self.stamp_time = MsgAdapter.ReadInt()
end

--颜值系统 道具时间戳下发
SCFaceTimeAllInfo = SCFaceTimeAllInfo or BaseClass(BaseProtocolStruct)
function SCFaceTimeAllInfo:__init()
    self.msg_type = 1796
end

function SCFaceTimeAllInfo:Decode()
    self.stamp_list = {}
    self.count = MsgAdapter.ReadInt()
    for i=1, self.count do
        local seq = MsgAdapter.ReadInt()
        local stamp_time = MsgAdapter.ReadInt()
        self.stamp_list[seq] = stamp_time
    end
end

-- NPC交互返回
SCNPCCommitBack = SCNPCCommitBack or BaseClass(BaseProtocolStruct)
function SCNPCCommitBack:__init()
    self.msg_type = 1766
end

function SCNPCCommitBack:Decode()
    self.obj_id = MsgAdapter.ReadInt()
    self.talk_id = MsgAdapter.ReadInt()
    self.option_list = {}
    for i = 1, 2 do
        self.option_list[i] = {
            button_id = MsgAdapter.ReadInt(),
            cost_coin = MsgAdapter.ReadInt(),
            cost_item = MsgAdapter.ReadInt(),
            cost_num = MsgAdapter.ReadInt()
        }
    end
end

-- NPC商店返回
SCNPCShopInfo = SCNPCShopInfo or BaseClass(BaseProtocolStruct)
function SCNPCShopInfo:__init()
    self.msg_type = 1767
end

function SCNPCShopInfo:Decode()
    self.obj_id = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.item_list = {}
    for i = 1, self.count do
        self.item_list[i] = {
            item_id = MsgAdapter.ReadInt(),
            num = MsgAdapter.ReadInt(),
            is_bind = MsgAdapter.ReadShort()
        }
        self.item_list[i].shop_info = {
            money = MsgAdapter.ReadShort(),
            price = MsgAdapter.ReadInt(),
            discount = MsgAdapter.ReadInt(),
            buy_num = MsgAdapter.ReadInt(),
            max_num = MsgAdapter.ReadInt(),
            obj_id = self.obj_id,
            buy_index = i - 1
        }
    end
end

-- 向NPC购买请求
CSNPCShowBuyReq = CSNPCShowBuyReq or BaseClass(BaseProtocolStruct)
function CSNPCShowBuyReq:__init()
    self.msg_type = 1768
end

function CSNPCShowBuyReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.obj_id)
    MsgAdapter.WriteInt(self.buy_index)
    MsgAdapter.WriteInt(self.buy_num)
end

-- 伙伴元素水晶请求
CSPartnerElementCrystalOper = CSPartnerElementCrystalOper or BaseClass(BaseProtocolStruct)
function CSPartnerElementCrystalOper:__init()
    self.msg_type = 1776
end

function CSPartnerElementCrystalOper:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.oper_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
    MsgAdapter.WriteInt(self.param4)
end

-- 伙伴元素水晶装备下发
SCPartnerUseElementCrystal = SCPartnerUseElementCrystal or BaseClass(BaseProtocolStruct)
function SCPartnerUseElementCrystal:__init()
    self.msg_type = 1775
end

function SCPartnerUseElementCrystal:Decode()
    self.count = MsgAdapter.ReadInt()
    self.info_list = {}
    for i = 1, self.count do
        self.info_list[i] = {
            partner = MsgAdapter.ReadInt(),
            crystal = MsgAdapter.ReadInt(),
            used_times = MsgAdapter.ReadInt(),
        }
    end
end


-- 成长之路 历程所有信息下发
SCAchieveCourseAllInfo = SCAchieveCourseAllInfo or BaseClass(BaseProtocolStruct)
function SCAchieveCourseAllInfo:__init()
    self.msg_type = 1786
end

function SCAchieveCourseAllInfo:Decode()
    self.info = {}
    self.info.level = MsgAdapter.ReadInt()      -- 历程等级
    self.info.fraction = MsgAdapter.ReadInt()   -- 历程点
    
    self.info.finish_flag_list = MsgAdapter.IntFlag(1504 / 32)     -- 历程条目完成记录
    self.info.level_reward_flag = MsgAdapter.IntFlag(2, true)            -- 历程等级奖励领取记录
    local count = MsgAdapter.ReadInt()
    --LogError("self.info.level_reward_flag", self.info.finish_flag_list)
    self.info.progress_list = {}                -- 历程条目进度
    for i = 0, count - 1 do
        self.info.progress_list[i] = MsgAdapter.ReadUInt()
    end
    --LogError("进度列表：",self.info.progress_list)
end

-- 成长之路 历程单个信息
SCAchieveCourseInfo = SCAchieveCourseInfo or BaseClass(BaseProtocolStruct)
function SCAchieveCourseInfo:__init()
    self.msg_type = 1787
end

function SCAchieveCourseInfo:Decode()
    self.info = {}
    self.info.seq = MsgAdapter.ReadInt()
    self.info.progress = MsgAdapter.ReadInt()
end

-- 成长之路 历程信息通知下发
SCAchieveCourseNotice = SCAchieveCourseNotice or BaseClass(BaseProtocolStruct)
function SCAchieveCourseNotice:__init()
    self.msg_type = 1788
end

function SCAchieveCourseNotice:Decode()
    self.info = {}
    self.info.notice_type = MsgAdapter.ReadInt()
    self.info.level = MsgAdapter.ReadInt()      -- 历程等级
    self.info.fraction = MsgAdapter.ReadInt()   -- 历程点
    self.info.p1 = MsgAdapter.ReadInt()
    self.info.p2 = MsgAdapter.ReadInt()
end

-- 成长之路 历程完成信息全部下发
SCAchieveCourseServerAllInfo = SCAchieveCourseServerAllInfo or BaseClass(BaseProtocolStruct)
function SCAchieveCourseServerAllInfo:__init()
    self.msg_type = 1789
end

function SCAchieveCourseServerAllInfo:Decode()
    self.info = {}
    self.info.server_role_count = MsgAdapter.ReadInt()      -- 该服玩家人数
    self.info.role_finish_count_list = {}                        -- 每一条完成人数
    local count = MsgAdapter.ReadInt()
    for i = 0, count - 1 do
        self.info.role_finish_count_list[i] = MsgAdapter.ReadInt()
    end
end

-- 成长之路 历程完成信息单个下发
SCAchieveCourseServerInfo = SCAchieveCourseServerInfo or BaseClass(BaseProtocolStruct)
function SCAchieveCourseServerInfo:__init()
    self.msg_type = 2027
end

function SCAchieveCourseServerInfo:Decode()
    self.info = {}
    self.info.server_role_count = MsgAdapter.ReadInt()      -- 该服玩家人数
    self.info.seq = MsgAdapter.ReadInt()
    self.info.finish_count = MsgAdapter.ReadInt()
end