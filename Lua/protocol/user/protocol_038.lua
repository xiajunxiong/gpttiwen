SCRAHeavenSentInfo = SCRAHeavenSentInfo or BaseClass(BaseProtocolStruct)
function SCRAHeavenSentInfo:__init()
    self.msg_type = 3810
end

function SCRAHeavenSentInfo:Decode()
    self.recharge_num = MsgAdapter.ReadInt()
    self.reward_flag = MsgAdapter.IntFlag()
end
--心法无量
SCRAHeartSkillInfo = SCRAHeartSkillInfo or BaseClass(BaseProtocolStruct)
function SCRAHeartSkillInfo:__init()
    self.msg_type = 3811
end

function SCRAHeartSkillInfo:Decode()
    self.recharge_num = MsgAdapter.ReadInt()
    self.reward_flag = MsgAdapter.IntFlag()
end

--赋世魂器
SCRAWorldSoulInfo = SCRAWorldSoulInfo or BaseClass(BaseProtocolStruct)
function SCRAWorldSoulInfo:__init()
    self.msg_type = 3812
end

function SCRAWorldSoulInfo:Decode()
    self.recharge_num = MsgAdapter.ReadInt()
    self.reward_flag = MsgAdapter.IntFlag()
end

--时装商店
SCRAFashionShopInfo = SCRAFashionShopInfo or BaseClass(BaseProtocolStruct)
function SCRAFashionShopInfo:__init()
    self.msg_type = 3820
end

function SCRAFashionShopInfo:Decode()
    self.buy_times = {}
    for i = 0, 63 do
        self.buy_times[i] = MsgAdapter.ReadInt()
    end
end

SET_AUTO_ADD_POINT_FLAG = {
    SET_AUTO_ADD_POINT_FLAG_ROLE = 0,	--index plan_index p0~p4属性点
    SET_AUTO_ADD_POINT_FLAG_PET = 1,	--p0 pet_index ,p1 flag
}

--设置自动加点协议
CSSetAutoAddPointFlag = CSSetAutoAddPointFlag or BaseClass(BaseProtocolStruct)
function CSSetAutoAddPointFlag:__init()
	self.msg_type = 3880
end

function CSSetAutoAddPointFlag:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.req_type)
	MsgAdapter.WriteShort(self.index)
	MsgAdapter.WriteUShort(self.param0 or 0)
	MsgAdapter.WriteUShort(self.param1 or 0)
	MsgAdapter.WriteUShort(self.param2 or 0)
	MsgAdapter.WriteUShort(self.param3 or 0)
	MsgAdapter.WriteUShort(self.param4 or 0)
end

-- 一元购
SCRAOneYuanGouInfo = SCRAOneYuanGouInfo or BaseClass(BaseProtocolStruct)
function SCRAOneYuanGouInfo:__init()
    self.msg_type = 3890
end

function SCRAOneYuanGouInfo:Decode()
    self.has_chong_zhi = MsgAdapter.ReadInt()
    self.buy_times = MsgAdapter.ReadInt()
end

--铸灵升级（废弃），穿戴，脱下请求
CSZhuLingReq = CSZhuLingReq or BaseClass(BaseProtocolStruct)
function CSZhuLingReq:__init()
    self.msg_type = 3850
end

function CSZhuLingReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteShort(self.param1)
    MsgAdapter.WriteShort(self.param2)
    MsgAdapter.WriteShort(self.param3)
    MsgAdapter.WriteShort(self.param4)
    MsgAdapter.WriteShort(self.param5)
    MsgAdapter.WriteShort(self.param6)
    MsgAdapter.WriteShort(self.param7)
    MsgAdapter.WriteShort(self.param8)
end

--铸灵升级，穿戴，脱下后下发（废弃）
SCZhuLingChangeInfo = SCZhuLingChangeInfo or BaseClass(BaseProtocolStruct)
function SCZhuLingChangeInfo:__init()
    self.msg_type = 3851
end

function SCZhuLingChangeInfo:Decode()
    self.req_type = MsgAdapter.ReadShort()
    self.item_src = MsgAdapter.ReadUShort()
    self.column = MsgAdapter.ReadUShort()
    self.index = MsgAdapter.ReadUShort()
    self.slot_index = MsgAdapter.ReadUShort()
    self.item_id = MsgAdapter.ReadUShort()
    self.level = MsgAdapter.ReadUShort()
    self.consume_item_src = MsgAdapter.ReadUShort()
    self.consume_column = MsgAdapter.ReadUShort()
    self.consume_index = MsgAdapter.ReadUShort()
    self.consume_slot_index = MsgAdapter.ReadUShort()
end

--铸灵分解完下发
SCZhuLingDecomposeInfo = SCZhuLingDecomposeInfo or BaseClass(BaseProtocolStruct)
function SCZhuLingDecomposeInfo:__init()
    self.msg_type = 3852
end

function SCZhuLingDecomposeInfo:Decode()
    self.fen_chen_num = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.item_id_list = {}
    for i = 1, self.count do
        self.item_id_list[i] = {}
        self.item_id_list[i].item_id = MsgAdapter.ReadInt()
        self.item_id_list[i].num = MsgAdapter.ReadInt()
    end
end

--铸灵分解请求
CSZhuLingDecomposeInfo = CSZhuLingDecomposeInfo or BaseClass(BaseProtocolStruct)
function CSZhuLingDecomposeInfo:__init()
    self.msg_type = 3853
end

function CSZhuLingDecomposeInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    for k, v in pairs(self.item_list) do
        MsgAdapter.WriteShort(v.column)
        MsgAdapter.WriteShort(v.index)
        MsgAdapter.WriteInt(v.num)
    end
end

-- 铸灵升级请求
CSZhuLingUpLevelReq = CSZhuLingUpLevelReq or BaseClass(BaseProtocolStruct)
function CSZhuLingUpLevelReq:__init()
    self.msg_type = 3854
end

function CSZhuLingUpLevelReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.item_src)
    MsgAdapter.WriteShort(self.index)
    MsgAdapter.WriteShort(self.column)
    MsgAdapter.WriteShort(self.slot_index)
    MsgAdapter.WriteInt(self.need_buy_num)
    for i = 0, self.now_level do
        MsgAdapter.WriteUShort(self.item_list[i] or 0)
    end
end

-- 替换装备时的返回
SCZhuLingPutOnSuccNotice = SCZhuLingPutOnSuccNotice or BaseClass(BaseProtocolStruct)
function SCZhuLingPutOnSuccNotice:__init()
    self.msg_type = 3855
end

function SCZhuLingPutOnSuccNotice:Decode()
    self.is_exchange = MsgAdapter.ReadUChar()
    MsgAdapter.ReadUChar()
    MsgAdapter.ReadUChar()
    MsgAdapter.ReadUChar()
end