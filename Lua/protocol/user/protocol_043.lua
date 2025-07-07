SCWeedendGiftInfo = SCWeedendGiftInfo or BaseClass(BaseProtocolStruct)
function SCWeedendGiftInfo:__init()
    self.msg_type = 4335
end

function SCWeedendGiftInfo:Decode()
    self.info_list = {}
    self.count = MsgAdapter.ReadInt()
    for i=1,self.count do
        local info = {
            index = MsgAdapter.ReadInt(),
            times = MsgAdapter.ReadInt(),
            end_time = MsgAdapter.ReadUInt(),
            title_name = Language.WeekGift.Title[1]
        }
        self.info_list[i] = info
    end
end

-- 限时累充
SCRAXianShiLeiChongInfo = SCRAXianShiLeiChongInfo or BaseClass(BaseProtocolStruct)
function SCRAXianShiLeiChongInfo:__init()
    self.msg_type = 4331
end

function SCRAXianShiLeiChongInfo:Decode()
    self.open_day = MsgAdapter.ReadInt()            --活动开启时的开服天数
    self.day_recharge_num = MsgAdapter.ReadInt()    --每日累充金额
    self.can_fetch_flag = MsgAdapter.IntFlag(0)     --能够领取的标志
    self.fetched_flag = MsgAdapter.IntFlag(0)       --已领取的标志
end

-- 超值献礼
SCRAChaoZhiGiftInfo = SCRAChaoZhiGiftInfo or BaseClass(BaseProtocolStruct)
function SCRAChaoZhiGiftInfo:__init()
    self.msg_type = 4340
end

function SCRAChaoZhiGiftInfo:Decode()
    local t = {}
    t.gift_act_flag = MsgAdapter.ReadInt()
    t.count = MsgAdapter.ReadInt()
    t.gift_num_list = {}
    for i = 1, t.count do
        t.gift_num_list[i] = MsgAdapter.ReadInt()
    end
    self.info = t
end

-- 修为客户端操作请求
CSXiuWeiReq = CSXiuWeiReq or BaseClass(BaseProtocolStruct)
function CSXiuWeiReq:__init()
    self.msg_type = 4345
end

function CSXiuWeiReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.p1)
    MsgAdapter.WriteInt(self.p2)
end


-- 修为系统全部技能信息
SCXiuWeiAllInfo = SCXiuWeiAllInfo or BaseClass(BaseProtocolStruct)
function SCXiuWeiAllInfo.ReadInfo()
    local info = {}
    info.level = MsgAdapter.ReadShort()     -- 等级
    MsgAdapter.ReadShort()
    info.exp = MsgAdapter.ReadInt()         -- 经验
    return info
end

function SCXiuWeiAllInfo:__init()
    self.msg_type = 4346
end

function SCXiuWeiAllInfo:Decode()
    local info_list = {}
    for i = 0, 5 do
        info_list[i] = SCXiuWeiAllInfo.ReadInfo()
    end
    self.info_list = info_list
end


-- 修为系统单个技能信息
SCXiuWeiInfo = SCXiuWeiInfo or BaseClass(BaseProtocolStruct)
function SCXiuWeiInfo:__init()
    self.msg_type = 4347
end

function SCXiuWeiInfo:Decode()
    self.skill_type = MsgAdapter.ReadInt()
    self.skill_info = SCXiuWeiAllInfo.ReadInfo()
end

--宠物飞升任务
SCPetFeiShengTaskInfo = SCPetFeiShengTaskInfo or BaseClass(BaseProtocolStruct)
function SCPetFeiShengTaskInfo:__init()
    self.msg_type = 4300
end

function SCPetFeiShengTaskInfo:Decode()
    local t = {}
    t.pet_unique_id1 = MsgAdapter.ReadUInt()
    t.pet_unique_id2 = MsgAdapter.ReadUInt()
    t.today_is_finish = MsgAdapter.ReadInt()
    t.task_id = MsgAdapter.ReadInt()
    self.info = t
end


-- 九州秘宝
CSTreasureTaskReq = CSTreasureTaskReq or BaseClass(BaseProtocolStruct)
function CSTreasureTaskReq:__init()
    self.msg_type = 4349
end

function CSTreasureTaskReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.type)
    MsgAdapter.WriteShort(self.param)
end

SCTreasureTaskInfo = SCTreasureTaskInfo or BaseClass(BaseProtocolStruct)
function SCTreasureTaskInfo:__init()
    self.msg_type = 4350
end

function SCTreasureTaskInfo:Decode()
    local t = {}
    t.level = MsgAdapter.ReadShort()        -- 接任务时的等级
    t.task_id = MsgAdapter.ReadShort()     -- 当前任务ID
    t.route = MsgAdapter.ReadShort()        -- 路线
    t.pos_id = MsgAdapter.ReadShort()       -- 坐标点ID
    t.pool = {}
    for i = 1, 8 do
        t.pool[MsgAdapter.ReadShort()] = i    -- 奖池 8
    end
    self.info = t
end

SCXiuWeiDanYaoLimitInfo = SCXiuWeiDanYaoLimitInfo or BaseClass(BaseProtocolStruct)
function SCXiuWeiDanYaoLimitInfo:__init()
    self.msg_type = 4348
end

function SCXiuWeiDanYaoLimitInfo:Decode()
    self.use_count_qua1 = MsgAdapter.ReadInt()
    self.use_count_qua2 = MsgAdapter.ReadInt()
end

-- 宠物徽章分解
CSPetBadgeResolve = CSPetBadgeResolve or BaseClass(BaseProtocolStruct)
function CSPetBadgeResolve:__init()
    self.msg_type = 4433
end

function CSPetBadgeResolve:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    for i,v in pairs(self.item_info_list) do
        MsgAdapter.WriteShort(v.col or 0)
        MsgAdapter.WriteShort(v.index or 0)
    end
end