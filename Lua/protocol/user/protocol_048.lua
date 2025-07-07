-- 元气烟火秀
SCRAYuanQiYanHuo = SCRAYuanQiYanHuo or BaseClass(BaseProtocolStruct)
function SCRAYuanQiYanHuo:__init()
    self.msg_type = 4800
end

function SCRAYuanQiYanHuo:Decode()
    local info = {}
    info.is_buy = MsgAdapter.ReadInt()
    info.is_receive_free = MsgAdapter.ReadInt()
    info.reward_flag = MsgAdapter.IntFlag(1)
    self.info = info
end

SCRASanBaiChouInfo = SCRASanBaiChouInfo or BaseClass(BaseProtocolStruct)
function SCRASanBaiChouInfo:__init()
    self.msg_type = 4805
end
function SCRASanBaiChouInfo:RASanBaiChouTypeParam()
    local t = {}
    t.times = MsgAdapter.ReadShort()
	t.select_seq = MsgAdapter.ReadShort()
    t.pool = {}
    for i = 1, MAX_RA_SANBAI_CHOU_DRAW_NUM, 1 do
        t.pool[i] = {index = i, value = MsgAdapter.ReadShort()}
    end
    return t
end
function SCRASanBaiChouInfo:Decode()
    self.draw_times = MsgAdapter.ReadInt()--总次数
    self.reward_item_seq = MsgAdapter.ReadInt()--表里的type
    self.select = {}
    for i = 1, MAX_RA_SANBAI_CHOU_TYPE_NUM do
        self.select[i] = self:RASanBaiChouTypeParam()
    end
    -- LogDG(self)
end

SCRASanBaiChouDrawInfo = SCRASanBaiChouDrawInfo or BaseClass(BaseProtocolStruct)
function SCRASanBaiChouDrawInfo:__init()
    self.msg_type = 4806
end
function SCRASanBaiChouDrawInfo:Decode()
    self.type = MsgAdapter.ReadShort() + 1
    self.select_seq = MsgAdapter.ReadShort()
    self.type_times = MsgAdapter.ReadInt()
    self.draw_times = MsgAdapter.ReadInt()												-- 总共抽奖次数
	self.pool = {}
    for i = 1, MAX_RA_SANBAI_CHOU_DRAW_NUM, 1 do
        self.pool[i] = {index = i, value = MsgAdapter.ReadShort()}
    end
    -- LogDG(self)
end

--------------- 圣器 ：这个框是圣器的地盘哦 ----------------
-- //!<4830 圣器客户端操作请求
CSHolyEquipOpReq = CSHolyEquipOpReq or BaseClass(BaseProtocolStruct)
function CSHolyEquipOpReq:__init()
    self.msg_type = 4830
end

function CSHolyEquipOpReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteInt(self.p1)
    MsgAdapter.WriteInt(self.p2)
    MsgAdapter.WriteInt(self.p3)
    MsgAdapter.WriteInt(self.p4)
    MsgAdapter.WriteInt(self.p5)
    MsgAdapter.WriteInt(self.p6)
    ShengQiCtrl.Log("//!<4830 圣器客户端操作请求",self)
end

-- //!<4831 圣器升级请求
CSHolyEquipLevelUpReq = CSHolyEquipLevelUpReq or BaseClass(BaseProtocolStruct)		
function CSHolyEquipLevelUpReq:__init()
    self.msg_type = 4831
end

function CSHolyEquipLevelUpReq:Encode()
    local info = self.info

    local t = {
        stuff_list = {},
        sheng_qi_list = {},
        sheng_qi_index = self.sheng_qi_index
    }

    MsgAdapter.WriteBegin(self.msg_type)

    t.stuff_type_num,t.sheng_qi_num = ShengQiData.Instance:GetSelStuffTypeInfo(info)

   
    local index = 1
    for i,v in pairs(info) do
        if i:Type() == ItemType.ShengQi then
            table.insert(t.sheng_qi_list, i.index)
        else
            if not t.stuff_list[i.item_id] then
                t.stuff_list[i.item_id] = {
                    item_id = i.item_id,
                    num = v
                }
            else
                t.stuff_list[i.item_id].num = t.stuff_list[i.item_id].num + v
            end
        end
    end

    local new_stuff_list = {}
    for i,v in pairs(t.stuff_list) do
        table.insert(new_stuff_list,v)
    end
    t.stuff_list = new_stuff_list

    MsgAdapter.WriteInt(t.sheng_qi_index)    -- // 要升级的圣器idx
    MsgAdapter.WriteShort(#t.stuff_list)  -- 材料种类数量
    MsgAdapter.WriteShort(t.sheng_qi_num)    -- 圣器数量

    for i = 1,4 do
        local item_id = 0
        if t.stuff_list[i] then
            item_id = t.stuff_list[i].item_id or 0
        end
        MsgAdapter.WriteUShort(item_id)
    end
    for i = 1,4 do
        local num = 0
        if t.stuff_list[i] then
            num = t.stuff_list[i].num or 0
        end
        MsgAdapter.WriteUShort(num)
    end

    for i = 1,30 do
        local sheng_qi_index = t.sheng_qi_list[i] or -1
        MsgAdapter.WriteShort(sheng_qi_index)
    end

    ShengQiCtrl.Log("-- //!<4831 圣器升级请求",t)
end


-- 圣器系统道具解析 背包变化用
local decode_sheng_qi_item_func = function ()
    local vo = {}
    vo.reason = MsgAdapter.ReadShort()      --//!< 改变的原因 ShengQiCfg.SendReason
    vo.put_reason = MsgAdapter.ReadShort()  --//!< 获得物品的来源  \see PUT_REASON_TYPE
    vo.item_id = MsgAdapter.ReadUShort()
    vo.index = MsgAdapter.ReadShort()
    vo.is_market_buy_flag = MsgAdapter.ReadChar()   --//!< 是否从市场购买标识 0:否 1:从市场购买
    vo.bag_type = MsgAdapter.ReadChar()             --//!< 背包类型,1-圣器背包,2-符石背包 ShengQiCfg.BAG_TYPE
    MsgAdapter.ReadShort()
    vo.num = MsgAdapter.ReadInt()
    vo.is_bind = MsgAdapter.ReadChar()
    vo.has_param = MsgAdapter.ReadChar()
    vo.invalid_time = MsgAdapter.ReadUInt()
    vo.item_unique_id_1 = MsgAdapter.ReadUInt()
    vo.item_unique_id_2 = MsgAdapter.ReadUInt()
    vo.param = {}
    return vo
end
    
-- 圣器系统道具解析 背包下发用
local decode_sheng_qi_item_func_bag = function ()
    local vo = {}
    vo.item_id = MsgAdapter.ReadUShort()
    vo.index = MsgAdapter.ReadShort()
    vo.is_market_buy_flag = MsgAdapter.ReadChar()   --//!< 是否从市场购买标识 0:否 1:从市场购买
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    vo.num = MsgAdapter.ReadInt()
    vo.is_bind = MsgAdapter.ReadChar()
    vo.has_param = MsgAdapter.ReadChar()
    vo.param_length = MsgAdapter.ReadShort()
    vo.invalid_time = MsgAdapter.ReadUInt()
    vo.item_unique_id_1 = MsgAdapter.ReadUInt()
    vo.item_unique_id_2 = MsgAdapter.ReadUInt()
    vo.param = {}
    return vo
end

-- //!<4832 圣器背包全部信息下发
SCHolyBagListInfo = SCHolyBagListInfo or BaseClass(BaseProtocolStruct)
function SCHolyBagListInfo:__init()
    self.msg_type = 4832
end

function SCHolyBagListInfo:Decode()
    local count = MsgAdapter.ReadInt()
    self.sheng_qi_list = {}
    for i = 1, count do
        local vo = decode_sheng_qi_item_func_bag()
        vo.param = ShengQi.DecodeParam()
        local sheng_qi = ShengQi.New(vo)
        self.sheng_qi_list[sheng_qi.index] = sheng_qi
    end
    ShengQiCtrl.Log("-- //!<4832 圣器背包全部信息下发",count,self)
end

--clearbag:
--savedata:
-- //!<4833 符石背包全部信息下发
SCFuShiBagListInfo = SCFuShiBagListInfo or BaseClass(BaseProtocolStruct)
function SCFuShiBagListInfo:__init()
    self.msg_type = 4833
end

function SCFuShiBagListInfo:Decode()
    local count = MsgAdapter.ReadInt()
    self.fu_shi_list = {}
    for i = 1, count do
        local vo = decode_sheng_qi_item_func_bag()
        vo.param = FuShi.DecodeParam()
        local fu_shi = FuShi.New(vo)
        self.fu_shi_list[fu_shi.index] = fu_shi
    end
    ShengQiCtrl.Log("-- //!<4833 符石背包全部信息下发",count,self)
end


-- //!<4834 圣器/符石背包单个信息下发
SCHolyBagItemChange = SCHolyBagItemChange or BaseClass(BaseProtocolStruct)
function SCHolyBagItemChange:__init()
    self.msg_type = 4834
end

function SCHolyBagItemChange:Decode()
    local vo = decode_sheng_qi_item_func()
    local bag_type = vo.bag_type
    local item
    if vo.has_param == 0 then
        self.bag_type = bag_type
        self.item = Item.Create(vo)
        self.reason = vo.reason
        return
    end
    local param_length = MsgAdapter.ReadShort()
    if bag_type == ShengQiCfg.BagType.ShengQi then
        vo.param = ShengQi.DecodeParam()
        item = ShengQi.New(vo)
    elseif bag_type == ShengQiCfg.BagType.FuShi then
        vo.param = FuShi.DecodeParam()
        item = FuShi.New(vo)
    end
    self.bag_type = bag_type
    self.item = item
    self.reason = vo.reason
    ShengQiCtrl.Log("-- //!<4834 圣器/符石背包单个信息下发",bag_type,"||",vo.reason,vo)
end

-- //!<4835 圣器 圣器圣器/符石合成消耗列表下发
SCSCHolyEquipLevelUpConsumeInfo = SCSCHolyEquipLevelUpConsumeInfo or BaseClass(BaseProtocolStruct)
function SCSCHolyEquipLevelUpConsumeInfo:__init()
    self.msg_type = 4835
end

function SCSCHolyEquipLevelUpConsumeInfo:Decode()
    self.bag_type = MsgAdapter.ReadInt()
    local count = MsgAdapter.ReadInt()
    self.index_list = {}
    for i = 1,count do
        local index = MsgAdapter.ReadInt()
        table.insert(self.index_list, index)
    end
    ShengQiCtrl.Log("/!<4835 圣器 圣器圣器/符石合成消耗列表下发",self)
end


-- 第一次获得圣器通知
SCShengQiFirstNotice = SCShengQiFirstNotice or BaseClass(BaseProtocolStruct)
function SCShengQiFirstNotice:__init()
    self.msg_type = 4836
end

function SCShengQiFirstNotice:Decode()
    self.first_flag = MsgAdapter.ReadInt()
    --LogDG("第一次获得圣器通知",self.first_flag)
end

-- 圣器跟随信息
SCShengQiFollowInfo = SCShengQiFollowInfo or BaseClass(BaseProtocolStruct)
function SCShengQiFollowInfo:__init()
    self.msg_type = 4837
end

function SCShengQiFollowInfo:Decode()
    self.follow_index = MsgAdapter.ReadInt()
    --LogDG("圣器跟随信息",self.follow_index)
end

-----------------------------------------------------------
-----------------------------------------------------------