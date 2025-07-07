local function AttrInfo()
    local t = {
        attr_type = MsgAdapter.ReadShort(),
        attr_value = MsgAdapter.ReadShort()
    }
    return t
end

--[[ local function EquipmentParam()
    local t = {}
    t.has_random = MsgAdapter.ReadChar()
	t.is_treasure = MsgAdapter.ReadChar() --0通常 1珍品
    t.attr_num = MsgAdapter.ReadShort()
    t.attr_list = {}
    for i = 1, EQUIP_ITEM_ATTR_NUM do
        t.attr_list[i] = AttrInfo()
        t.attr_list[i].index = i
    end
    t.stone = {}
    for i = 1, EQUIP_STONE_MOUNT_MAX do
        t.stone[i] = MsgAdapter.ReadUShort()
    end
    t.creater_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.sp_id = MsgAdapter.ReadInt()
    -- t.reserve_sh = MsgAdapter.ReadShort()
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

function InfoType()
    local t = {}
    t.index = MsgAdapter.ReadShort()
    t.item_id = MsgAdapter.ReadUShort()
    t.num = MsgAdapter.ReadShort()
    t.is_bind = MsgAdapter.ReadShort()
    -- t.item_unique_id = MsgAdapter.ReadLL()
    t.item_unique_id_1 = MsgAdapter.ReadUInt()  
    t.item_unique_id_2 = MsgAdapter.ReadUInt()
    t.param = EquipmentParam()
    return t
end

--装备列表
SCEquipmentInfoAck = SCEquipmentInfoAck or BaseClass(BaseProtocolStruct)
function SCEquipmentInfoAck:__init()
    self.msg_type = 1600
end

function SCEquipmentInfoAck:Decode()
    self.element_crystal_id = MsgAdapter.ReadUShort()
    self.element_crystal_id = self.element_crystal_id == CommonItemId.Exp and 0 or self.element_crystal_id
    self.used_times = MsgAdapter.ReadShort()
    self.reserve_sh = MsgAdapter.ReadShort()
    self.count = MsgAdapter.ReadShort()
    self.equipment_list = {}
    for i = 1, self.count do
        local equip_info = InfoType()
        self.equipment_list[equip_info.index] = equip_info
    end
end
--装备变更
SCEquipmentGridChange = SCEquipmentGridChange or BaseClass(BaseProtocolStruct)
function SCEquipmentGridChange:__init()
    self.msg_type = 1601
end
function SCEquipmentGridChange:Decode()
    self.reason = MsgAdapter.ReadInt()
    self.equip_info = InfoType()
end
SCElementCrystalChangeNotice = SCElementCrystalChangeNotice or BaseClass(BaseProtocolStruct)
function SCElementCrystalChangeNotice:__init()
    self.msg_type = 1602
end
function SCElementCrystalChangeNotice:Decode()
    self.element_crystal_id = MsgAdapter.ReadUShort()
    self.element_crystal_id = self.element_crystal_id == CommonItemId.Exp and 0 or self.element_crystal_id
    self.used_times = MsgAdapter.ReadShort()
end
--装备词条信息
SCEquipmentLockInfo = SCEquipmentLockInfo or BaseClass(BaseProtocolStruct)
function SCEquipmentLockInfo:__init()
    self.msg_type = 1603
end
function SCEquipmentLockInfo:Decode()
    self.m_equipment_list = {}
    for i = 1,5 do 
        local vo = {}
        vo.is_lock = {}
        for j = 1,6 do 
            vo.is_lock[j] = MsgAdapter.ReadChar()
        end 
        MsgAdapter.ReadShort()
        table.insert( self.m_equipment_list, vo )
    end 
    self.m_jewelry_list = {}
    for i = 1,3 do 
        local vo = {}
        vo.is_lock = {}
        for j = 1,6 do 
            vo.is_lock[j] = MsgAdapter.ReadChar()
        end 
        MsgAdapter.ReadShort()
        table.insert( self.m_jewelry_list, vo )
    end 
end
--装备强化系统
SCEquipmentJewerlyUpGreadeInfo = SCEquipmentJewerlyUpGreadeInfo or BaseClass(BaseProtocolStruct)
function SCEquipmentJewerlyUpGreadeInfo:__init()
    self.msg_type = 1604
end

function SCEquipmentJewerlyUpGreadeInfo:Decode()
    self.equip_jewelry_upgreade = {}
    for i = 0,7 do 
        self.equip_jewelry_upgreade[i] = MsgAdapter.ReadInt()
    end 

    self.equip_jewelry_strengthen = {}
    for i= 0,7 do 
        self.equip_jewelry_strengthen[i] = MsgAdapter.ReadInt()
    end 
end

--神装共鸣
SCEquipmentJewerlyResonaceInfo = SCEquipmentJewerlyResonaceInfo or BaseClass(BaseProtocolStruct)
function SCEquipmentJewerlyResonaceInfo:__init()
    self.msg_type = 1605
end

function SCEquipmentJewerlyResonaceInfo:Decode()
	self.resonace_flag = MsgAdapter.ReadInt()
end

--------------造化装备
--装备操作请求
CSZaoHuaEquipmentOperaReq = CSZaoHuaEquipmentOperaReq or BaseClass(BaseProtocolStruct)
function CSZaoHuaEquipmentOperaReq:__init()
    self.msg_type = 1640
end

-- oper_type 0 请求，1穿戴装备（param1背包格子序号）2脱下装备（param1）
function CSZaoHuaEquipmentOperaReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.oper_type)
    MsgAdapter.WriteInt(self.param_1)
    MsgAdapter.WriteInt(self.param_2)
    MsgAdapter.WriteInt(self.param_3)
end

function YuanHunZhuParam()
    local t = {}
    t.has_net_value = MsgAdapter.ReadInt()
    t.item_id = MsgAdapter.ReadUShort()
    t.attr_seq = {}
    for i = 1,3 do 
        t.attr_seq[i] = MsgAdapter.ReadUShort()
    end 
    t.attrs = {}
    for i = 1,3 do 
        local vo = {}
        vo.attr_type = MsgAdapter.ReadShort()
        vo.attr_value = MsgAdapter.ReadShort()
        t.attrs[i] = vo
    end 
    return t
end 

local function ZaoHuaEquipParam(item_id,param_length)
    if param_length == 0 then return nil end 
    local t = {}
    t.has_net_value = MsgAdapter.ReadInt()
    t.ling_li_value = MsgAdapter.ReadInt()
    t.hun_li_value = MsgAdapter.ReadInt()
    t.yuan_hun_zhu = {}
    for i = 1,4 do 
        local vo = YuanHunZhuParam()
        -- vo.item_id = MsgAdapter.ReadUShort()
        -- vo.reserved_sh = MsgAdapter.ReadUShort()
        table.insert(t.yuan_hun_zhu,vo)
    end 
    return t
end

local function ZaoHuaEquipment()
    local t = {}
    t.item_id = MsgAdapter.ReadUShort()
    -- LogError("?!item_id",t.item_id)
    t.index = MsgAdapter.ReadShort()
    -- LogError("?!index",t.index)
    t.is_market_buy_flag = MsgAdapter.ReadChar()
    -- LogError("?!is_market_buy_flag",t.is_market_buy_flag)
    t.reserve_ch = MsgAdapter.ReadChar() -- 对齐用 预留值
    -- LogError("?!reserve_ch",t.reserve_ch)
    t.reserve_sh = MsgAdapter.ReadShort()
    -- LogError("?!reserve_sh",t.reserve_sh)
    t.num = MsgAdapter.ReadInt()
    -- LogError("?!num",t.num)
    t.is_bind = MsgAdapter.ReadChar()
    -- LogError("?!is_bind",t.is_bind)
    t.has_param = MsgAdapter.ReadChar()
    -- LogError("?!has_param",t.has_param)
    t.invalid_time = MsgAdapter.ReadUInt()
    -- LogError("?!invalid_time",t.invalid_time)
    t.item_unique_id_1 = MsgAdapter.ReadUInt()  
    -- LogError("?!item_unique_id_1",t.item_unique_id_1)
    t.item_unique_id_2 = MsgAdapter.ReadUInt()  
    -- LogError("?!item_unique_id_2",t.item_unique_id_2)
    if t.has_param ~= 0 then 
        t.param_length = MsgAdapter.ReadShort()
        -- LogError("?!param_length",t.param_length)
        t.param = ZaoHuaEquipParam(t.item_id,t.param_length)
        -- LogError("?!param_data",t.param_data)
    end 
    return t
end

-- 造化装备背包下发
SCZaoHuaBagListInfo = SCZaoHuaBagListInfo or BaseClass(BaseProtocolStruct)
function SCZaoHuaBagListInfo:__init()
    self.msg_type = 1641
end

function SCZaoHuaBagListInfo:Decode()
    self.reason = MsgAdapter.ReadShort() -- 0 正常下发（上线或客户端请求）1 整理
    self.item_count = MsgAdapter.ReadShort() -- 物品数量

    -- LogError("!?reason",self.reason)
    -- LogError("!?item_count",self.item_count)
    self.item_list = {}
    for i = 1,self.item_count do 
        -- LogError("?fuckew>?",i)
        local item = ZaoHuaEquipment()
        table.insert(self.item_list,item)     
    end 
end


-- 造化背包单个修改下发
SCZaoHuaBagItemChange = SCZaoHuaBagItemChange or BaseClass(BaseProtocolStruct)
function SCZaoHuaBagItemChange:__init()
    self.msg_type = 1642
end

function SCZaoHuaBagItemChange:Decode()
    self.reason = MsgAdapter.ReadShort() -- 改变的原因
    self.put_reason = MsgAdapter.ReadShort() -- 置入原因

    self.item_info = ZaoHuaEquipment()
end 

local function ZaoHuaPro()
    local t = {}    
    t.index = MsgAdapter.ReadShort() -- 序号
    t.item_id = MsgAdapter.ReadUShort() -- 物品ID
    t.num = MsgAdapter.ReadShort() -- 数量
    t.is_bind = MsgAdapter.ReadShort() -- 数量
    t.item_unique_id_1 = MsgAdapter.ReadUInt()  
    t.item_unique_id_2 = MsgAdapter.ReadUInt()  
    t.param = ZaoHuaEquipParam(t.item_id)
    return t
end

--造化装备信息
SCZaoHuaEquipmentInfoAck = SCZaoHuaEquipmentInfoAck or BaseClass(BaseProtocolStruct)
function SCZaoHuaEquipmentInfoAck:__init()
    self.msg_type = 1643
end

function SCZaoHuaEquipmentInfoAck:Decode()
    self.count = MsgAdapter.ReadInt()
    self.equipment_list = {}
    for i = 1,self.count do
        local vo = ZaoHuaPro()
        table.insert(self.equipment_list,vo)
    end 
end

--造化装备 改变通知
SCZaoHuaEquipmentGridChange = SCZaoHuaEquipmentGridChange or BaseClass(BaseProtocolStruct)
function SCZaoHuaEquipmentGridChange:__init()
    self.msg_type = 1644
end

function SCZaoHuaEquipmentGridChange:Decode()
    self.item_info = ZaoHuaPro()
end

--造化装备 打造返回
SCZaoHuaEquipmentComposeResults = SCZaoHuaEquipmentComposeResults or BaseClass(BaseProtocolStruct)
function SCZaoHuaEquipmentComposeResults:__init()
    self.msg_type = 1645
end
function SCZaoHuaEquipmentComposeResults:Decode()
    self.result = MsgAdapter.ReadChar() -- 0失败
    self.item = {}
    self.item.has_param = MsgAdapter.ReadChar() ~= 0
    self.item.item_id = MsgAdapter.ReadUShort()
    self.item.num = MsgAdapter.ReadUShort()
    if self.item.has_param then
        self.item.param_len = MsgAdapter.ReadUShort()
        self.item.param = ItemDataParam(self.item.item_id, self.item.param_len)
    end
end


SCZaoHuaAppearance = SCZaoHuaAppearance or BaseClass(BaseProtocolStruct)
function SCZaoHuaAppearance:__init()
    self.msg_type = 1646
end

function SCZaoHuaAppearance:Decode()
    self.wear_app_id = MsgAdapter.ReadInt()
    self.flag = MsgAdapter.IntFlag(3)
end 

CSZaoHuaEquipmentDiscomposeReq = CSZaoHuaEquipmentDiscomposeReq or BaseClass(BaseProtocolStruct)
function CSZaoHuaEquipmentDiscomposeReq:__init()
    self.msg_type = 1647
end

function CSZaoHuaEquipmentDiscomposeReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    for i = 1,16 do 
        MsgAdapter.WriteInt(self.bag_list[i])
    end 
end 

SCZaoHuaEquipmentCommonDataInfo = SCZaoHuaEquipmentCommonDataInfo or BaseClass(BaseProtocolStruct)
function SCZaoHuaEquipmentCommonDataInfo:__init()
    self.msg_type = 1648
end

function SCZaoHuaEquipmentCommonDataInfo:Decode()
    self.show_mo_zhuang = MsgAdapter.ReadInt()
end 

--装备操作请求
CSEquipmentOperaReq = CSEquipmentOperaReq or BaseClass(BaseProtocolStruct)
function CSEquipmentOperaReq:__init()
    self.msg_type = 1650
end
function CSEquipmentOperaReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.oper_type)
    MsgAdapter.WriteInt(self.param_1)
    MsgAdapter.WriteInt(self.param_2)
    MsgAdapter.WriteInt(self.param_3)
    MsgAdapter.WriteInt(self.param_4)
end

SCPetFollowStatus = SCPetFollowStatus or BaseClass(BaseProtocolStruct)
function SCPetFollowStatus:__init()
    self.msg_type = 1690
end
function SCPetFollowStatus:Decode()
    self.obj_id = MsgAdapter.ReadInt()
    self.pet_id = MsgAdapter.ReadInt()
    self.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    self.skin_id = MsgAdapter.ReadInt()
    self.fly_flag = MsgAdapter.ReadInt()
    self.fly_flag = self.fly_flag > FlyUpStageType.High and FlyUpStageType.High or self.fly_flag
end

-- 抓捕宠物得到什么物品
SCSealPetGetItemInfo = SCSealPetGetItemInfo or BaseClass(BaseProtocolStruct)
function SCSealPetGetItemInfo:__init()
    self.msg_type = 1691
end

function SCSealPetGetItemInfo:Decode()
    self.pet_id = MsgAdapter.ReadInt()
    self.item_id = MsgAdapter.ReadInt()
    self.num = MsgAdapter.ReadShort()
    self.is_get_pet = MsgAdapter.ReadShort()--0碎片1宠物
end

SCPetIdRecordList = SCPetIdRecordList or BaseClass(BaseProtocolStruct)
function SCPetIdRecordList:__init()
    self.msg_type = 1692
end

function SCPetIdRecordList:Decode()
    self.count = MsgAdapter.ReadInt()
    self.has_get_list = {}
    for i=1,self.count do
        local id = MsgAdapter.ReadUShort()
        self.has_get_list[id] = 1
    end
end

CSAbandonmentPetByList = CSAbandonmentPetByList or BaseClass(BaseProtocolStruct)
function CSAbandonmentPetByList:__init()
    self.msg_type = 1693
end

function CSAbandonmentPetByList:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    for i=1, #self.pets_index do
        MsgAdapter.WriteUChar(self.pets_index[i])
    end
end

SCMovePetGridNotice = SCMovePetGridNotice or BaseClass(BaseProtocolStruct)
function SCMovePetGridNotice:__init()
    self.msg_type = 1694
end
function SCMovePetGridNotice:Decode()
    self.type = MsgAdapter.ReadInt()--0是失败，1是移动到仓库 2 是移动到背包
    self.pet_index = MsgAdapter.ReadInt()
    self.pet_storage_index = MsgAdapter.ReadInt()
end

SCPetSkinInfo = SCPetSkinInfo or BaseClass(BaseProtocolStruct)
function SCPetSkinInfo:__init()
    self.msg_type = 1695
end

function SCPetSkinInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.pet_skin_list = {}
    for i=1, self.count do
        self.pet_skin_list[MsgAdapter.ReadInt()] = 1
    end
end

SCPetBreachAttri = SCPetBreachAttri or BaseClass(BaseProtocolStruct)
function SCPetBreachAttri:__init()
    self.msg_type = 1696
end

-- 宠物突破属性预览
function SCPetBreachAttri:Decode()
    self.info = {}
    self.info.index = MsgAdapter.ReadInt()
    self.info.attr_list = {}
    for i = 1, PET_ATTR_LIST_COUNT do
        self.info.attr_list[i - 1] = MsgAdapter.ReadInt()
    end
end

-- 魂器升级
CSPetWeaponIntensify = CSPetWeaponIntensify or BaseClass(BaseProtocolStruct)
function CSPetWeaponIntensify:__init()
    self.msg_type = 1697
end

function CSPetWeaponIntensify:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    self.cost_num = #self.stuff_info
    MsgAdapter.WriteInt(self.is_put_on)
    MsgAdapter.WriteShort(self.index)
    MsgAdapter.WriteShort(self.pet_soul_index)
    MsgAdapter.WriteInt(self.cost_num)
    for i=1, 30 do
        if not self.stuff_info[i] then
            MsgAdapter.WriteShort(-1)
        else
            MsgAdapter.WriteShort(self.stuff_info[i].bag_colum)
        end
    end
    for i=1, 30 do
        if not self.stuff_info[i] then
            MsgAdapter.WriteShort(-1)
        else
            MsgAdapter.WriteShort(self.stuff_info[i].bag_idx)
        end
    end
    for i=1, 30 do
        if not self.stuff_info[i] then
            MsgAdapter.WriteShort(-1)
        else
            MsgAdapter.WriteShort(self.stuff_info[i].bag_num)
        end
    end
    --LogError("魂器升级请求: ", self)
end

-- 神印背包
SCPetStampBagInfo = SCPetStampBagInfo or BaseClass(BaseProtocolStruct)
function SCPetStampBagInfo:__init()
    self.msg_type = 1661
end

function SCPetStampBagInfo:Decode()
    self.info = {}
    self.info.pray_pos_index = MsgAdapter.ReadInt() -- 祈神的位置
    self.info.exp = MsgAdapter.ReadLL()     -- 存有经验 -- 放这里？？？
    self.info.today_free_cost = MsgAdapter.ReadInt()
    self.info.blue_print_list = {}
    for i = 0, PetStampCfg.BlueprintMaxCapacity - 1 do
        local blue_print_info = PetStampBlueprint.DecodeInfo()
        self.info.blue_print_list[i] = PetStampBlueprint.New(blue_print_info, i)
    end
    self.info.bag_count = MsgAdapter.ReadInt()
    self.info.pet_stamp_list = {}
    for i = 1, self.info.bag_count do
        local bag_index = MsgAdapter.ReadInt()
        local pet_stamp_info = PetStamp.DecodeInfo()
        pet_stamp_info.bag_index = bag_index
        self.info.pet_stamp_list[bag_index] = PetStamp.New(pet_stamp_info, bag_index)
    end
end

-- 神印单个信息
SCPetStampInfo = SCPetStampInfo or BaseClass(BaseProtocolStruct)
function SCPetStampInfo:__init()
    self.msg_type = 1660
end

function SCPetStampInfo:Decode()
    self.info = {}
    self.info.put_reason = MsgAdapter.ReadInt()--通知来源
    self.info.index = MsgAdapter.ReadInt()
    self.exp = MsgAdapter.ReadLL()
    local info = PetStamp.DecodeInfo()
    self.info.pet_stamp = PetStamp.New(info, self.info.index)
end

-- 神印经验信息
SCPetStampExpInfo = SCPetStampExpInfo or BaseClass(BaseProtocolStruct)
function SCPetStampExpInfo:__init()
    self.msg_type = 1662
end

function SCPetStampExpInfo:Decode()
    self.info = {}
    self.info.exp = MsgAdapter.ReadLL()
end

-- 神印方案单个下发
SCPetStanpBluePrintInfo = SCPetStanpBluePrintInfo or BaseClass(BaseProtocolStruct)
function SCPetStanpBluePrintInfo:__init()
    self.msg_type = 1663
end

function SCPetStanpBluePrintInfo:Decode()
    self.info = {}
    self.info.index = MsgAdapter.ReadInt()
    local blueprint_info = PetStampBlueprint.DecodeInfo()
    self.info.blueprint = PetStampBlueprint.New(blueprint_info,self.info.index)
end

-- 设置神印方案
CSPetStampSetBluePrint = CSPetStampSetBluePrint or BaseClass(BaseProtocolStruct)
function CSPetStampSetBluePrint:__init()
    self.msg_type = 1664
end

function CSPetStampSetBluePrint:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.index)
    MsgAdapter.WriteStrN(self.name,PROTC_ROLE_NAME_LEN)
    MsgAdapter.WriteInt(self.pet_index)
    for i = 1, PetStampCfg.StampMaxPos do
        MsgAdapter.WriteInt(self.bag_index_list[i])
    end
end

-- 神印日志列表
SCPetStampPrayLogInfo = SCPetStampPrayLogInfo or BaseClass(BaseProtocolStruct)
function SCPetStampPrayLogInfo:__init()
   self.msg_type = 1665 
end

function SCPetStampPrayLogInfo:Decode()
    self.log_list = {}
    local log_num = MsgAdapter.ReadInt()
    for i = 0, log_num - 1 do
        self.log_list[i] = {}
        self.log_list[i].time = MsgAdapter.ReadUInt()
        self.log_list[i].item_id = MsgAdapter.ReadInt()
        self.log_list[i].index = i
    end
end

-- 神印日志详细信息
SCPetStampPrayLogContent = SCPetStampPrayLogContent or BaseClass(BaseProtocolStruct)
function SCPetStampPrayLogContent:__init()
    self.msg_type = 1666
end

function SCPetStampPrayLogContent:Decode()
    self.info = {}
    self.info.time = MsgAdapter.ReadUInt()
    local info = PetStamp.DecodeInfo()
    self.info.pet_stamp = PetStamp.New(info)
end

-- 祈神奖励下发
SCPetStampPrayRewardInfo = SCPetStampPrayRewardInfo or BaseClass(BaseProtocolStruct)
function SCPetStampPrayRewardInfo:__init()
    self.msg_type = 1667
end

function SCPetStampPrayRewardInfo:Decode()
    self.info = {}
    self.info.today_free_cost = MsgAdapter.ReadInt()
    self.use_gold = MsgAdapter.ReadInt()
    self.info.consume_coin = MsgAdapter.ReadInt()
    self.info.reward_num = MsgAdapter.ReadInt()
    self.info.end_seq = MsgAdapter.ReadInt() + 1
    self.info.pet_stamp_index_list = {}
    self.info.result_seq_list = {}
    self.info.item_info_list = {}
    for i = 1,self.info.reward_num do
        self.info.pet_stamp_index_list[i] = MsgAdapter.ReadInt()
        self.info.result_seq_list[i] = MsgAdapter.ReadInt() + 1
        local info = {}
        info.group = MsgAdapter.ReadInt()
        info.seq = MsgAdapter.ReadInt()
        self.info.item_info_list[i] = info
    end
    if #self.info.result_seq_list >= 1 then
        table.remove(self.info.result_seq_list, 1)
        table.insert(self.info.result_seq_list, self.info.end_seq)
    end
end

-- 神印升级消耗
CSPetStampIntensifyReq = CSPetStampIntensifyReq or BaseClass(BaseProtocolStruct)
function CSPetStampIntensifyReq:__init()
    self.msg_type = 1668
end

function CSPetStampIntensifyReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.stamp_index)
    MsgAdapter.WriteInt(self.num)
    for i = 1, PetStampCfg.IntensifyStuffMaxCount do
        MsgAdapter.WriteShort(self.index_list[i])
    end
end

-- 宠物祝福列表信息下发  -- 只是进入游戏下发一次
SCPetPrayListInfo = SCPetPrayListInfo or BaseClass(BaseProtocolStruct)
function SCPetPrayListInfo:__init()
    self.msg_type = 1698
end

function SCPetPrayListInfo:Decode()
    self.pray_list = {}
    for i = 0, 99 do
        self.pray_list[i] = MsgAdapter.ReadShort()   -- i: 宠物下标  value:被祝福的主宠 没有就是-1
    end
end

-- 单个宠物祝福信息下发 -- 变化下发
SCPetPrayInfo = SCPetPrayInfo or BaseClass(BaseProtocolStruct)
function SCPetPrayInfo:__init()
    self.msg_type = 1699
end

function SCPetPrayInfo:Decode()
    self.info = {}
    self.info.pet_index = MsgAdapter.ReadShort()        -- 宠物下标
    self.info.main_pet_index = MsgAdapter.ReadShort()   -- 被祝福的主宠 没有就是-1
end