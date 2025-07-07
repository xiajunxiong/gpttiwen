SCPeriodActivityInfo = SCPeriodActivityInfo or BaseClass(BaseProtocolStruct)
function SCPeriodActivityInfo:__init()
    self.msg_type = 4700
end

function SCPeriodActivityInfo:Decode()
    self.period_type = MsgAdapter.ReadInt()
    self.begin_time = MsgAdapter.ReadUInt()
    self.end_time = MsgAdapter.ReadUInt()
    -- LogError("周期活动",self)
    -- LogDG(self.end_time - TimeHelper:GetServerTime(),"周期活动剩余的时间（小于等于0已过期不显示）!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
end

--跨服开启时间 时间戳
SCWorldStatusHiddenInfo = SCWorldStatusHiddenInfo or BaseClass(BaseProtocolStruct)
function SCWorldStatusHiddenInfo:__init()
    self.msg_type = 4750
end

function SCWorldStatusHiddenInfo:Decode()
    self.cross_open_server_time = MsgAdapter.ReadUInt() -- 跨服开启时间
    -- LogError("跨服开启时间",self)
end

-- 天宫寻宝操作请求
CSSkyTreasureHuntReq = CSSkyTreasureHuntReq or BaseClass(BaseProtocolStruct)
function CSSkyTreasureHuntReq:__init()
    self.msg_type = 4735
end

function CSSkyTreasureHuntReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
end

-- 天宫寻宝下发
SCSkyTreasureHuntInfo = SCSkyTreasureHuntInfo or BaseClass(BaseProtocolStruct)
function SCSkyTreasureHuntInfo:__init()
    self.msg_type = 4736
end

function SCSkyTreasureHuntInfo:Decode()
    self.pos = MsgAdapter.ReadInt()
    self.use_free = MsgAdapter.ReadInt()
end

--仙子去哪儿 0 玩家信息请求 1完成游戏请求
CSFairyWhereReq = CSFairyWhereReq or BaseClass(BaseProtocolStruct)
function CSFairyWhereReq:__init()
    self.msg_type = 4730
end

function CSFairyWhereReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
end

SCFairyWhereInfo = SCFairyWhereInfo or BaseClass(BaseProtocolStruct)
function SCFairyWhereInfo:__init()
    self.msg_type = 4731
end

function SCFairyWhereInfo:Decode()
    self.has_fetch = MsgAdapter.ReadChar() --是否领取过仙子去哪儿的奖励 0 未领取
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    -- LogDG("是否领取过仙子去哪儿的奖励 0 未领取",self.has_fetch)
end

SCFairyWhereSceneInfo = SCFairyWhereSceneInfo or BaseClass(BaseProtocolStruct)
function SCFairyWhereSceneInfo:__init()
    self.msg_type = 4732
end

function SCFairyWhereSceneInfo:Decode()
    local info = {}
    local scene_id = MsgAdapter.ReadInt()
    local stamp_time = MsgAdapter.ReadUInt()
    self.info = MapData.Instance:GetMapConfig(scene_id) or {}
    self.info.scene_id = scene_id
    self.info.stamp_time = stamp_time
    self.scene_id = scene_id
    -- LogError("仙子去哪儿",self)
end

-- 兔子快跑请求
CSRabbitRaceReq = CSRabbitRaceReq or BaseClass(BaseProtocolStruct)
function CSRabbitRaceReq:__init()
    self.msg_type = 4770
end

function CSRabbitRaceReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.p1)
    MsgAdapter.WriteInt(self.p2)
    MsgAdapter.WriteInt(self.p3)
    --LogError("兔子快跑请求",self)
end

-- 兔子快跑下发
SCRabbitRaceSceneInfo = SCRabbitRaceSceneInfo or BaseClass(BaseProtocolStruct)
function SCRabbitRaceSceneInfo:__init()
    self.msg_type = 4771
end

function SCRabbitRaceSceneInfo:Decode()
    local info = {}
    info.is_finish = MsgAdapter.ReadInt()
    info.phase = MsgAdapter.ReadInt()
    info.next_phase_time = MsgAdapter.ReadUInt()
    info.results = {}
    for i = 0, 2 do
        info.results[i] = MsgAdapter.ReadInt()
    end
    info.role_set_info = {}
    for i = 1,2 do
        local data = {}
        data.role_id = MsgAdapter.ReadInt()
        data.set_rabbit = {}
        data.tow_set_rabbit = {}    -- 第二次设置的
        data.is_ready = {}
        for n = 0, 2 do
            data.set_rabbit[n] = MsgAdapter.ReadInt()
        end
        for n = 0, 2 do
            data.tow_set_rabbit[n] = MsgAdapter.ReadInt()
        end
        for n = 1, 2 do
            data.is_ready[n] = MsgAdapter.ReadInt()
        end
        info.role_set_info[data.role_id] = data
    end
    self.info = info
    --LogError("兔子快跑下发", info)
end

SCRabbitRaceOtherInfo = SCRabbitRaceOtherInfo or BaseClass(BaseProtocolStruct)
function SCRabbitRaceOtherInfo:__init()
    self.msg_type = 4772
end

function SCRabbitRaceOtherInfo:Decode()
    self.reward_times = MsgAdapter.ReadInt()
    --LogError("兔子快跑奖励信息下发", self)
end

-- 兔子休息信息
SCRabbitRestInfo = SCRabbitRestInfo or BaseClass(BaseProtocolStruct)
function SCRabbitRestInfo:__init()
    self.msg_type = 4773
end

function SCRabbitRestInfo:Decode()
    local info = {}
    info.track_type = MsgAdapter.ReadInt()
    info.obj_id = MsgAdapter.ReadInt()
    info.role_id = MsgAdapter.ReadInt()
    self.info = info
    --LogError("兔子休息信息", info.obj_id)
end

-- 兔子鼓励结果
SCRabbitTaceOperateResult = SCRabbitTaceOperateResult or BaseClass(BaseProtocolStruct)
function SCRabbitTaceOperateResult:__init()
    self.msg_type = 4774
end

function SCRabbitTaceOperateResult:Decode()
    local info = {}
    info.en_type = MsgAdapter.ReadInt()     -- 对兔子的鼓励类型
    info.is_succ = MsgAdapter.ReadInt()     -- 是否成功
    info.succ_times = MsgAdapter.ReadInt()   -- 成功次数
    info.fail_times = MsgAdapter.ReadInt()   -- 失败次数
    info.obj_id = MsgAdapter.ReadInt()
    self.info = info
    --LogError("兔子鼓励结果", self)
end

-- 兔子休息结束
SCRabbitRaceRestEnd = SCRabbitRaceRestEnd or BaseClass(BaseProtocolStruct)
function SCRabbitRaceRestEnd:__init()
    self.msg_type = 4775
end

function SCRabbitRaceRestEnd:Decode()
    local info = {}
    info.track_type = MsgAdapter.ReadInt()
    info.obj_id = MsgAdapter.ReadInt()
    info.role_id = MsgAdapter.ReadInt()
    self.info = info
    --LogError("兔子休息结束!!!!!!", self)
end

-- 兔子胡萝卜次数
SCRabbitRacePropInfo = SCRabbitRacePropInfo or BaseClass(BaseProtocolStruct)
function SCRabbitRacePropInfo:__init()
    self.msg_type = 4776
end

function SCRabbitRacePropInfo:Decode()
    self.use_prop_times = MsgAdapter.ReadInt()
    --LogError("兔子胡萝卜次数",self.use_prop_times)
end

-- 兔子结束通知 
SCRabbitRaceEnd = SCRabbitRaceEnd or BaseClass(BaseProtocolStruct)
function SCRabbitRaceEnd:__init()
    self.msg_type = 4777
end

function SCRabbitRaceEnd:Decode()
    self.win_role_id = MsgAdapter.ReadInt()
    --LogError("兔子快跑结束", self.win_role_id)
end


-- 对象速度变化
SCRabbitRaceObjSpeedChange = SCRabbitRaceObjSpeedChange or BaseClass(BaseProtocolStruct)
function SCRabbitRaceObjSpeedChange:__init()
    self.msg_type = 1155
end

function SCRabbitRaceObjSpeedChange:Decode()
    self.obj_id = MsgAdapter.ReadInt()
    self.move_speed = MsgAdapter.ReadInt()
    --LogError("兔子速度变化",self)
end

SCRAJinYiTianXiang = SCRAJinYiTianXiang or BaseClass(BaseProtocolStruct)
function SCRAJinYiTianXiang:__init()
    self.msg_type = 4790
end

function SCRAJinYiTianXiang:Decode()
    self.draw_times = MsgAdapter.ReadInt()
    self.get_reward_flag = MsgAdapter.IntFlag()
    self.has_draw_value = MsgAdapter.ReadInt()
    --LOG("4790:::::",self)
end

