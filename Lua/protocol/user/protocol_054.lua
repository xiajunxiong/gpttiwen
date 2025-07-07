-- 请求家族任务
CSRoleGuildTaskReq = CSRoleGuildTaskReq or BaseClass(BaseProtocolStruct)
function CSRoleGuildTaskReq:__init()
    self.msg_type = 5400
end

-- req 0 接受 p1 5401协议index
-- req 1 放弃 p1 5401协议index
-- req 2 请求帮助 
-- req 3 帮助 p1 role_id p2 要提交的宠物pet_index, 不提交宠物赋值-1
-- req 4 刷新
-- req 5 打怪任务的打怪 发这个请求
-- req 6 发送邮件 p1 是否一键 p2 角色uid
-- req 7 领取赠予奖励 
function CSRoleGuildTaskReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

local function RoleGuildTaskItem()
    local t = {}
    t.is_get = MsgAdapter.ReadChar()          -- 是否刷新过
    t.is_solicit = MsgAdapter.ReadChar()      -- 是否征集过
    local reserve_sh = MsgAdapter.ReadShort()
    t.task_id = MsgAdapter.ReadInt()          -- 开始的任务
    t.curr_task_id = MsgAdapter.ReadInt()     -- 当前的任务
    t.refresh_time = MsgAdapter.ReadUInt()

    return t
end

-- 家族任务信息
SCRoleGuildTaskInfo = SCRoleGuildTaskInfo or BaseClass(BaseProtocolStruct)
function SCRoleGuildTaskInfo:__init()
    self.msg_type = 5401
end

function SCRoleGuildTaskInfo:Decode()
    self.finish_times = MsgAdapter.ReadInt()  -- 完成次数
    self.task_index = MsgAdapter.ReadInt() -- 当前进行中的任务
    self.task_list = {}
    -- 固定就6个哦
    for i = 0, 5 do 
        local vo = RoleGuildTaskItem()
        vo.index = i
        self.task_list[i] = vo
    end 
end

-- 任务状态改变
-- reason 0 任务开始
-- reason 1 任务完成
-- reason 2 任务放弃
-- reason 3 任务募集
SCRoleGuildTaskChange = SCRoleGuildTaskChange or BaseClass(BaseProtocolStruct)
function SCRoleGuildTaskChange:__init()
    self.msg_type = 5402
end

function SCRoleGuildTaskChange:Decode()
    self.reason = MsgAdapter.ReadInt()
    self.task_index = MsgAdapter.ReadInt()
    local vo = RoleGuildTaskItem()
    vo.index = self.task_index
    self.task_info = vo
end

--实名奖励领取协议
CSRealNameRewardReq = CSRealNameRewardReq or BaseClass(BaseProtocolStruct)
function CSRealNameRewardReq:__init()
    self.msg_type = 5410
end

function CSRealNameRewardReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteStrN(self.spid, 4)
end

--实名奖励获得flag
SCRealNameRewardInfo = SCRealNameRewardInfo or BaseClass(BaseProtocolStruct)
function SCRealNameRewardInfo:__init()
    self.msg_type = 5411
end

function SCRealNameRewardInfo:Decode()
    self.receive_flag = MsgAdapter.ReadInt()    --0未领取，1已领取
end
--个人大玩咖等级
SCRoleInfoEtc = SCRoleInfoEtc or BaseClass(BaseProtocolStruct)
function SCRoleInfoEtc:__init()
    self.msg_type = 5412
end

function SCRoleInfoEtc:Decode()
    self.da_wan_ka_level = MsgAdapter.ReadInt()
end

CSSpid360Req = CSSpid360Req or BaseClass(BaseProtocolStruct)
function CSSpid360Req:__init()
    self.msg_type = 5415
end

function CSSpid360Req:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteStrN(self.spid, 4)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
end

SCSpid360Info = SCSpid360Info or BaseClass(BaseProtocolStruct)
function SCSpid360Info:__init()
    self.msg_type = 5416
end

function SCSpid360Info:Decode()
    self.login_flag = MsgAdapter.IntFlag()
    self.fetch_flag = MsgAdapter.IntFlag()
end

-----------------灵宝系统-----------------{
-- 灵宝系统请求
CSLingBaoSystemReq = CSLingBaoSystemReq or BaseClass(BaseProtocolStruct)
function CSLingBaoSystemReq:__init()
	self.msg_type = 5420
end

function CSLingBaoSystemReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.req_type)
	MsgAdapter.WriteInt(self.param1)
	MsgAdapter.WriteInt(self.param2)
end

-- 灵宝系统背包所有信息下发（登陆下发）
SCLingBaoBagInfoAck = SCLingBaoBagInfoAck or BaseClass(BaseProtocolStruct)
function SCLingBaoBagInfoAck:__init()
    self.msg_type = 5421
end

function SCLingBaoBagInfoAck:Decode()
	self.item_count = MsgAdapter.ReadShort()
	self.item_list = {}
    for i = 1, self.item_count do 
        local item = LingBao.DecodeInfo()
        table.insert(self.item_list, item)     
    end
end

-- 灵宝系统背包单条信息下发
SCLingBaoBagSingleInfo = SCLingBaoBagSingleInfo or BaseClass(BaseProtocolStruct)
function SCLingBaoBagSingleInfo:__init()
    self.msg_type = 5422
end

function SCLingBaoBagSingleInfo:Decode()
    self.reason = MsgAdapter.ReadShort() -- 改变的原因
    self.put_reason = MsgAdapter.ReadShort() -- 置入原因
    self.item_info = LingBao.DecodeInfo()
end

-- 灵宝系统所有圣痕佩戴信息下发（登陆下发）
SCLingBaoSystemShengHenWearInfo = SCLingBaoSystemShengHenWearInfo or BaseClass(BaseProtocolStruct)
function SCLingBaoSystemShengHenWearInfo:__init()
    self.msg_type = 5423
end

function SCLingBaoSystemShengHenWearInfo:Decode()
    self.ware_count = MsgAdapter.ReadShort()
    self.ware_list = {}
    for i = 1, self.ware_count do
        table.insert(self.ware_list, LingBao.DecodeWareInfo())
    end
end

-- 灵宝系统圣痕佩戴单条信息下发
SCLingBaoSystemShengHenWearSingleInfo = SCLingBaoSystemShengHenWearSingleInfo or BaseClass(BaseProtocolStruct)
function SCLingBaoSystemShengHenWearSingleInfo:__init()
    self.msg_type = 5424
end

function SCLingBaoSystemShengHenWearSingleInfo:Decode()
    self.info = LingBao.DecodeWareInfo()
end

-- 灵宝系统圣痕所有信息下发
SCLingBaoSystemShengHenAllInfo = SCLingBaoSystemShengHenAllInfo or BaseClass(BaseProtocolStruct)
function SCLingBaoSystemShengHenAllInfo:__init()
    self.msg_type = 5425
end

function SCLingBaoSystemShengHenAllInfo:Decode()
    self.is_open = MsgAdapter.ReadChar()
    self.shenghen_type = MsgAdapter.ReadShort() --如果这个是-1证明发的是所有圣痕，按圣痕顺序下来
    self.count = MsgAdapter.ReadShort()
    self.shenghen_info = {}
    if self.count == 1 then
        table.insert(self.shenghen_info, LingBao.DecodeShengHenInfo(self.shenghen_type))
    else
        for i = 1, self.count do
            table.insert(self.shenghen_info, LingBao.DecodeShengHenInfo(i - 1))
        end
    end
end

-- 灵宝系统单个圣痕中所有灵宝的强化信息下发
SCLingBaoSystemShengHenPartInfo = SCLingBaoSystemShengHenPartInfo or BaseClass(BaseProtocolStruct)
function SCLingBaoSystemShengHenPartInfo:__init()
    self.msg_type = 5426
end

function SCLingBaoSystemShengHenPartInfo:Decode()
    MsgAdapter.ReadShort()
    self.shenghen_type = MsgAdapter.ReadShort()
    self.lingbao_str_lv = {}
    for i = 0, LingBaoConfig.LingBaoMax - 1 do
        self.lingbao_str_lv[i] = MsgAdapter.ReadShort() --强化等级
        MsgAdapter.ReadShort()
    end
end

-- 灵宝系统相关通知下发
SCLingBaoSystemRelatedNoitce = SCLingBaoSystemRelatedNoitce or BaseClass(BaseProtocolStruct)
function SCLingBaoSystemRelatedNoitce:__init()
    self.msg_type = 5427
end

function SCLingBaoSystemRelatedNoitce:Decode()
    self.notice_type = MsgAdapter.ReadShort()
    self.param1 = MsgAdapter.ReadShort()
    self.param2 = MsgAdapter.ReadInt()
    self.param3 = MsgAdapter.ReadInt()
end

-- 灵宝系统灵宝分解请求
CSLingBaoSystemDecomposeReq = CSLingBaoSystemDecomposeReq or BaseClass(BaseProtocolStruct)
function CSLingBaoSystemDecomposeReq:__init()
    self.msg_type = 5428
end

function CSLingBaoSystemDecomposeReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
    for i, v in ipairs(self.list) do
        MsgAdapter.WriteShort(v)
    end
end

-- 灵宝系统灵宝合成(升星/升阶)请求
CSLingBaoSystemComposeReq = CSLingBaoSystemComposeReq or BaseClass(BaseProtocolStruct)
function CSLingBaoSystemComposeReq:__init()
    self.msg_type = 5429
end

function CSLingBaoSystemComposeReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteShort(self.shenghen_type)--圣痕类型是有效范围则bag_index_list列表首个为部位序号，无效范围则列表都是背包序号
    for i, v in ipairs(self.index_list) do
        MsgAdapter.WriteShort(v)
    end
end
-----------------灵宝系统-----------------}


-----------------神龙系统-----------------

-- //!< 5480 神龙系统请求
CSGodDragonSystemReq = CSGodDragonSystemReq or BaseClass(BaseProtocolStruct)
function CSGodDragonSystemReq:__init()
    self.msg_type = 5480
end	

function CSGodDragonSystemReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.p1)
    MsgAdapter.WriteInt(self.p2)
    --LOG("神龙系统 请求",self)
end

--//!< 5485 神龙系统使用龙魂经验道具请求 》》 废弃了
CSGodDragonSystemUseSoulItemReq = CSGodDragonSystemUseSoulItemReq or BaseClass(BaseProtocolStruct)
function CSGodDragonSystemUseSoulItemReq:__init()
    self.msg_type = 5485
end

function CSGodDragonSystemUseSoulItemReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.bag_index)     -- 龙魂背包下标
    for i = 1, MAX_GOD_DRAGON_USE_GRID_NUM do
        local stuff_info = self.stuff_list[i]
        if stuff_info then
            MsgAdapter.WriteUShort(stuff_info.item_id)
            MsgAdapter.WriteShort(stuff_info.num)
        end
    end
    --LOG("神龙系统经验道具 请求", self)
end

--//!< 5486 神龙系统龙魂分解请求
CSGodDragonSystemSoulDecomposeReq = CSGodDragonSystemSoulDecomposeReq or BaseClass(BaseProtocolStruct)
function CSGodDragonSystemSoulDecomposeReq:__init()
    self.msg_type = 5486
end

function CSGodDragonSystemSoulDecomposeReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    local list = self.list    --//分解序号列表
    for i,v in pairs(list) do  
        MsgAdapter.WriteShort(v)
    end
    --LOG("神龙系统分解 请求",self)
end

-- 龙魂背包
SCGodDragonSoulBag = SCGodDragonSoulBag or BaseClass(BaseProtocolStruct)
function SCGodDragonSoulBag:__init()
    self.msg_type = 5481
end

function SCGodDragonSoulBag:Decode()
    local count = MsgAdapter.ReadShort()
    self.item_list = {}
    for i = 1, count do
        local vo = {}
        vo.item_id = MsgAdapter.ReadUShort()
        vo.index = MsgAdapter.ReadShort()
        vo.is_market_buy_flag = MsgAdapter.ReadChar()
        vo.num = MsgAdapter.ReadInt()
        vo.is_bind = MsgAdapter.ReadChar()
        vo.has_param = MsgAdapter.ReadChar()
        vo.invalid_time = MsgAdapter.ReadUInt()
        vo.item_unique_id_1 = MsgAdapter.ReadUInt()
        vo.item_unique_id_2 = MsgAdapter.ReadUInt()
        if vo.has_param == 1 then
            local param_length = MsgAdapter.ReadShort()
            vo.param = FairyDragonSoul.DecodeParam()
        else
            vo.param = {}
        end
        local item = FairyDragonSoul.New(vo)
        self.item_list[vo.index] = item
    end
    --LOG("神龙系统背包 下发",self)
end

-- 龙魂单条下发
SCGodDragonSoul = SCGodDragonSoul or BaseClass(BaseProtocolStruct)
function SCGodDragonSoul:__init()
    self.msg_type = 5482
end

function SCGodDragonSoul:Decode()
    local vo = {}
    self.reason = MsgAdapter.ReadShort()       -- 改变的原因
    self.put_reason = MsgAdapter.ReadShort()   -- 获得的来源
    vo.item_id = MsgAdapter.ReadUShort()
    vo.index = MsgAdapter.ReadShort()
    vo.is_market_buy_flag = MsgAdapter.ReadChar()
    vo.num = MsgAdapter.ReadInt()
    vo.is_bind = MsgAdapter.ReadChar()
    vo.has_param = MsgAdapter.ReadChar()
    vo.invalid_time = MsgAdapter.ReadUInt()
    vo.item_unique_id_1 = MsgAdapter.ReadUInt()
    vo.item_unique_id_2 = MsgAdapter.ReadUInt()
    
    if vo.has_param == 1 then
        local param_length = MsgAdapter.ReadShort()
        vo.param = FairyDragonSoul.DecodeParam()
    else
        vo.param = {}
    end
    local item = FairyDragonSoul.New(vo)
    self.item = item
    --LOG("神龙系统单个龙魂 下发",self)
end

--//!< 5483 神龙系统相关通知下发
SCGodDragonSystemNoitce = SCGodDragonSystemNoitce or BaseClass(BaseProtocolStruct)
function SCGodDragonSystemNoitce:__init()
    self.msg_type = 5483
end

function SCGodDragonSystemNoitce:Decode()
    local t = {}
    t.notice_type = MsgAdapter.ReadShort()
    t.p1 = MsgAdapter.ReadShort()
    t.p2 = MsgAdapter.ReadInt()
    t.p3 = MsgAdapter.ReadLL()
    self.info = t
    --LOG("神龙系统通知 下发",self)
end

--//!< 5484 神龙系统基础信息
SCGodDragonSystemBaseInfo = SCGodDragonSystemBaseInfo or BaseClass(BaseProtocolStruct)
function SCGodDragonSystemBaseInfo:__init()
    self.msg_type = 5484
end

function SCGodDragonSystemBaseInfo:Decode()
    self.info = FairyDragon.DecodeBaseInfo()
    --LOG("神龙系统基础信息 下发",self)
end

--//!< 5487 神龙系统祈愿结果信息下发
SCGodDragonSystemPrayResultInfo = SCGodDragonSystemPrayResultInfo or BaseClass(BaseProtocolStruct)
function SCGodDragonSystemPrayResultInfo:__init()
    self.msg_type = 5487
end

function SCGodDragonSystemPrayResultInfo:Decode()
    local count = MsgAdapter.ReadInt()
    self.pray_list = {}
    for i = 1, count do
        table.insert(self.pray_list, MsgAdapter.ReadInt())
    end
    --LOG("神龙系统祈愿结果 下发",self)
end

-- 龙魂新的升级请求
CSGodDragonSystemSoulLevelUpSeq = CSGodDragonSystemSoulLevelUpSeq or BaseClass(BaseProtocolStruct)
function CSGodDragonSystemSoulLevelUpSeq:__init()
    self.msg_type = 5488
end

function CSGodDragonSystemSoulLevelUpSeq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.up_index)
    MsgAdapter.WriteShort(0)
    local list = self.list    --//分解序号列表
    for i,v in pairs(list) do  
        MsgAdapter.WriteShort(v)
    end
    --LOG("龙魂新的升级请求",self)
end

-----------------神龙系统-----------------



-----------------幽冥系统-----------------

--//!< 5440 幽冥系统请求
CSNetherWorldSystemReq = CSNetherWorldSystemReq or BaseClass(BaseProtocolStruct)
function CSNetherWorldSystemReq:__init()
    self.msg_type = 5440
end

function CSNetherWorldSystemReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.p1)
    MsgAdapter.WriteInt(self.p2)
    MsgAdapter.WriteInt(self.p3)
end

--//!< 5441 幽冥系统背包所有信息下发
SCNetherWorldBagInfoAck = SCNetherWorldBagInfoAck or BaseClass(BaseProtocolStruct)
function SCNetherWorldBagInfoAck:__init()
    self.msg_type = 5441
end

function SCNetherWorldBagInfoAck:Decode()
    self.bag_type = MsgAdapter.ReadShort()
    self.item_count = MsgAdapter.ReadShort()
    self.vo_list = {}
    --LogDG("背包数量", self.item_count)
    for i = 1, self.item_count do
        local vo = {}
        vo.in_bag = true
        vo.item_id = MsgAdapter.ReadUShort()
        vo.index = MsgAdapter.ReadShort()
        vo.is_market_buy_flag = MsgAdapter.ReadChar()
        vo.num = MsgAdapter.ReadInt()
        vo.is_bind = MsgAdapter.ReadChar()
        vo.has_param = MsgAdapter.ReadChar()
        vo.invalid_time = MsgAdapter.ReadUInt()
        vo.item_unique_id_1 = MsgAdapter.ReadUInt()
        vo.item_unique_id_2 = MsgAdapter.ReadUInt()
        if vo.has_param == 1 then
            local len = MsgAdapter.ReadShort()
            if self.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NW then
                vo.param = YouMing.DecodeParam(vo)  
            elseif self.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NWE then
                vo.param = YouMing.DecodeEquipParam(vo)
            elseif self.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_ALL then
                local big_type = Item.GetBigType(vo.item_id)
                if big_type == ITEM_BIG_TYPE.YouMing then
                    vo.bag_type = YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NW
                    vo.param = YouMing.DecodeParam(vo)  
                elseif big_type == ITEM_BIG_TYPE.YouMingEquip then
                    vo.bag_type = YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NWE
                    vo.param = YouMing.DecodeEquipParam(vo)
                else
                    LogError("错误 big_type = ", big_type)
                end
            else
                LogError("错误 bag_type = ", self.bag_type)
            end
        end
        self.vo_list[i] = vo
        LOG("5441 单个信息", self.vo_list[i].bag_type, self.vo_list[i].index, self.vo_list[i].item_id)
    end
    -- LOG("5441 幽冥背包所有信息", self.item_count, self)
end


--//!< 5442 幽冥系统背包单条信息下发
SCNetherWorldBagSingleInfo = SCNetherWorldBagSingleInfo or BaseClass(BaseProtocolStruct)
function SCNetherWorldBagSingleInfo:__init()
    self.msg_type = 5442
end

function SCNetherWorldBagSingleInfo:Decode()
    self.reason = MsgAdapter.ReadShort()        -- 改变的原因
    self.put_reason = MsgAdapter.ReadShort()    -- 获得物品来源\see PUT_REASON_TYPE

    local vo = {}
    vo.in_bag = true
    vo.item_id = MsgAdapter.ReadUShort()
    vo.index = MsgAdapter.ReadShort()
    vo.is_market_buy_flag = MsgAdapter.ReadChar()
    vo.bag_type = MsgAdapter.ReadChar()
    vo.num = MsgAdapter.ReadInt()
    vo.is_bind = MsgAdapter.ReadChar()
    vo.has_param = MsgAdapter.ReadChar()
    vo.invalid_time = MsgAdapter.ReadUInt()
    vo.item_unique_id_1 = MsgAdapter.ReadInt()
    vo.item_unique_id_2 = MsgAdapter.ReadInt()
    if vo.has_param == 1 then
        local len = MsgAdapter.ReadShort()
        if vo.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NW then
            vo.param = YouMing.DecodeParam(vo)  
        elseif vo.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NWE then
            vo.param = YouMing.DecodeEquipParam(vo)
        elseif vo.bag_type == YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_ALL then
            local big_type = Item.GetBigType(vo.item_id)
            if big_type == ITEM_BIG_TYPE.YouMing then
                vo.bag_type = YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NW
                vo.param = YouMing.DecodeParam(vo)  
            elseif big_type == ITEM_BIG_TYPE.YouMingEquip then
                vo.bag_type = YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NWE
                vo.param = YouMing.DecodeEquipParam(vo)
            else
                LogError("错误 big_type = ", big_type)
            end
        else
            LogError("错误 bag_type = ", self.bag_type)
        end
    end
    self.vo = vo
    --LOG("5442 幽冥系统背包单条信息下发", self)
end

--//!< 5443 幽冥系统基础信息下发
SCNetherWorldSystemBaseInfo = SCNetherWorldSystemBaseInfo or BaseClass(BaseProtocolStruct)
function SCNetherWorldSystemBaseInfo:__init()
    self.msg_type = 5443
end

function SCNetherWorldSystemBaseInfo:Decode()
    -- 普通寻宝和高级寻宝信息
    self.lottery_info = {}
    for i = 0, 1 do
        local info = {}
        info.lucky_value = MsgAdapter.ReadInt()     -- 探宝幸运值
        info.reward_flag = MsgAdapter.IntFlag(1,true)     -- 幸运奖励领取标记
        info.reward_list = {}   -- 转盘上显示的奖励信息
        for j = 1, 12 do
            local reward_info = {}
            reward_info.item_id = MsgAdapter.ReadUShort()
            reward_info.num = MsgAdapter.ReadShort()
            reward_info.version = MsgAdapter.ReadShort()
            reward_info.is_bind = MsgAdapter.ReadChar()
            MsgAdapter.ReadChar()
            table.insert(info.reward_list, reward_info)
        end
        self.lottery_info[i] = info
    end
    --LOG("5443 幽冥系统基础信息下发", self)
end

--//!< 5444 幽冥系统相关通知下发
SCNetherWorldSystemNotice = SCNetherWorldSystemNotice or BaseClass(BaseProtocolStruct)
function SCNetherWorldSystemNotice:__init()
    self.msg_type = 5444
end

function SCNetherWorldSystemNotice:Decode()
    self.notice_type = MsgAdapter.ReadShort()
    self.p1 = MsgAdapter.ReadShort()
    self.p2 = MsgAdapter.ReadInt()
    self.p3 = MsgAdapter.ReadInt()
    self.p4A = MsgAdapter.ReadUInt()
    self.p4B = MsgAdapter.ReadUInt()
    self.p4 = int64.new(self.p4A, self.p4B)--userdata
    self.p4 = int64.tostring(self.p4)
    --LOG("5444 幽冥系统相关通知下发", self)
end

-- //!< 5445 幽冥系统幽冥单个孔位格子穿戴信息
SCNetherWorldSystemWearSingleInfo = SCNetherWorldSystemWearSingleInfo or BaseClass(BaseProtocolStruct)
function SCNetherWorldSystemWearSingleInfo:__init()
    self.msg_type = 5445
end
function SCNetherWorldSystemWearSingleInfo:Decode()
    self.bag_index = MsgAdapter.ReadShort()
    self.equip_grid_type = MsgAdapter.ReadShort() + 1
    local info = {}
    info.in_bag = false
    info.index = self.equip_grid_type
    info.item_id = MsgAdapter.ReadInt()
    info.enqiue_id_1 = MsgAdapter.ReadUInt()
    info.enqiue_id_2 = MsgAdapter.ReadUInt()
    info.param = {}
    info.param.attr_type_sort = MsgAdapter.ReadShort()    -- 道具归属类型
    MsgAdapter.ReadShort()
    info.param.level = MsgAdapter.ReadUShort()
    info.param.str_level = MsgAdapter.ReadUShort()        -- 强化等级
    info.param.cur_exp = MsgAdapter.ReadLL()              -- 当前经验值
    info.param.attr_entry_list = {}   -- 副词条列表
    for j = 1, YouMingCfg.MAX_NETHER_WORLD_EQUIP_ATTR_AFFIX_NUM do
        local t = {}
        t.attri_type = MsgAdapter.ReadShort()
        t.attri_value = MsgAdapter.ReadShort()
        info.param.attr_entry_list[j] = t
    end
    self.equip_info = info
    --LOG("孔位穿戴变化", self.equip_info)
end

--//!< 5446 幽冥系统幽冥改造结果下发
SCNetherWorldSystemReformResultInfo	= SCNetherWorldSystemReformResultInfo or BaseClass(BaseProtocolStruct)
function SCNetherWorldSystemReformResultInfo:__init()
    self.msg_type = 5446
end
function SCNetherWorldSystemReformResultInfo:Decode()
    self.bag_index = MsgAdapter.ReadShort()
    self.reform_index = MsgAdapter.ReadShort();			-- //改造下标[0, MAX_NETHER_WORLD_REFORM_GROWTH_NUM)
    self.reform_growth_list = {}
    for i = 0, YouMingCfg.ADD_POINT_TYPE_MAX - 1 do
        self.reform_growth_list[i] = MsgAdapter.ReadUShort()
    end
    -- unsigned short reform_growth_list[ADD_POINT_TYPE_MAX];	
    --LOG("幽冥系统改造结果下发", self)
end

--//!< 5447 幽冥批量合成操作
CSNetherWorldBatchComposeReq = CSNetherWorldBatchComposeReq or BaseClass(BaseProtocolStruct)
function CSNetherWorldBatchComposeReq:__init()
    self.msg_type = 5447
end

function CSNetherWorldBatchComposeReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    for i,v in pairs(self.list or {}) do
        MsgAdapter.WriteUShort(v)
    end
    --LOG("5447 幽冥批量合成操作", self)
end

--//!< 5448 幽冥批量合成 返回
SCNetherWorldBatchComposeRet = SCNetherWorldBatchComposeRet or BaseClass(BaseProtocolStruct)
function SCNetherWorldBatchComposeRet:__init()
    self.msg_type = 5448
end

function SCNetherWorldBatchComposeRet:Decode()
    local count = MsgAdapter.ReadInt()
    self.bag_index_list = {}
    for i = 1, count do
        table.insert(self.bag_index_list, MsgAdapter.ReadShort())
    end
    --LOG("5448 幽冥批量合成 返回", self)
end

--//!< 5449 幽冥批量分解操作
CSNetherWorldBatchDecomposeReq = CSNetherWorldBatchDecomposeReq or BaseClass(BaseProtocolStruct)
function CSNetherWorldBatchDecomposeReq:__init()
    self.msg_type = 5449
end

function CSNetherWorldBatchDecomposeReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    for i,v in pairs(self.list or {}) do
        MsgAdapter.WriteShort(v)
    end
    --LOG("5449 幽冥批量分解操作", self)
end


--//!< 5450 幽冥系统幽冥属性信息下发
SCNetherWorldAttrListInfo = SCNetherWorldAttrListInfo or BaseClass(BaseProtocolStruct)
function SCNetherWorldAttrListInfo:__init()
    self.msg_type = 5450
end
function SCNetherWorldAttrListInfo:Decode()
    self.attri_list = {}
    self.attr_list = {}
    --self.index = MsgAdapter.ReadInt()
    self.index = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    self.capaility = MsgAdapter.ReadInt()--修改为特殊评分
    for i = 1, PET_ATTR_LIST_COUNT do
        local value = MsgAdapter.ReadInt()
        self.attr_list[i - 1] = value
        if value > 0 and (i < 45 or i > 49) then
            table.insert(self.attri_list, {attri_type = i, attri_value = value})
        end
    end
    --LOG("5450 幽冥系统幽冥属性信息下发", self)
end
-- //!< 5451 幽冥系统冥装添加经验请求
CSNetherWorldEquipAddExpReq = CSNetherWorldEquipAddExpReq or BaseClass(BaseProtocolStruct)
function CSNetherWorldEquipAddExpReq:__init()
    self.msg_type = 5451
end
function CSNetherWorldEquipAddExpReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.bag_index)
    MsgAdapter.WriteShort(self.equip_grid_type)
    for i, v in ipairs(self.equip_bag_index_list) do--不超过200个
        MsgAdapter.WriteShort(v)
    end
end

-- //!< 5452 幽冥系统探宝抽取下发
SCNetherWorldTreasureList = SCNetherWorldTreasureList or BaseClass(BaseProtocolStruct)
function SCNetherWorldTreasureList:__init()
    self.msg_type = 5452
end

function SCNetherWorldTreasureList:Decode()
    local count = MsgAdapter.ReadShort()
    self.result_list = {}
    for i = 1, count do
        self.result_list[i] = MsgAdapter.ReadShort() + 1
    end
    --LOG("5452 幽冥系统探宝抽取下发", self)
end

--//!< 5454 幽冥系统附身列表信息
SCNetherWorldPossessList = SCNetherWorldPossessList or BaseClass(BaseProtocolStruct)
function SCNetherWorldPossessList:__init()
    self.msg_type = 5454
end

function SCNetherWorldPossessList:Decode()
    local count = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    self.list = {}
    for i = 1, count do
        local t = {}
        t.bag_index = MsgAdapter.ReadShort()
        MsgAdapter.ReadShort()
        t.pet_unique_id1 = MsgAdapter.ReadUInt()
        t.pet_unique_id2 = MsgAdapter.ReadUInt()
        table.insert(self.list, t)
    end
    --LOG("幽冥系统附身列表信息", self)
end

-- //!< 5453 幽冥系统领悟技能请求
CSNetherWorldGraspSkillReq = CSNetherWorldGraspSkillReq or BaseClass(BaseProtocolStruct)
function CSNetherWorldGraspSkillReq:__init()
    self.msg_type = 5453
end
function CSNetherWorldGraspSkillReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.bag_index)
    MsgAdapter.WriteShort(self.reform_index)
    MsgAdapter.WriteShort(self.skill_index)
    MsgAdapter.WriteShort(self.is_high_grasp)
    MsgAdapter.WriteInt(self.skill_book_item_id)
end

CSNetherWorldSystemAwakenLevelUpReq = CSNetherWorldSystemAwakenLevelUpReq or BaseClass(BaseProtocolStruct)
function CSNetherWorldSystemAwakenLevelUpReq:__init()
    self.msg_type = 5455
end
function CSNetherWorldSystemAwakenLevelUpReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(0)
    MsgAdapter.WriteShort(self.bag_index)
    for i, v in ipairs(self.stuff_list) do
        MsgAdapter.WriteChar(v)
    end
    --LOG("发送觉醒", self)
end

-- 通知检查探宝信息
SCNetherWorldCheckTreasure = SCNetherWorldCheckTreasure or BaseClass(BaseProtocolStruct)
function SCNetherWorldCheckTreasure:__init()
    self.msg_type = 5456
end

function SCNetherWorldCheckTreasure:Decode()

end