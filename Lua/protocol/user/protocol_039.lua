-- 灵玉基金
SCRALingYuJiJinInfo = SCRALingYuJiJinInfo or BaseClass(BaseProtocolStruct)
function SCRALingYuJiJinInfo:__init()
    self.msg_type = 3900
end

function SCRALingYuJiJinInfo:Decode()
    self.active_flags = MsgAdapter.IntFlag(0, true)
    self.fetch_flags = {}
    for i = 0, 7 do
        self.fetch_flags[i] = {}
        self.fetch_flags[i] = MsgAdapter.IntFlag(0)
    end
end

CSGuildActivePersonRank = CSGuildActivePersonRank or BaseClass(BaseProtocolStruct)
function CSGuildActivePersonRank:__init()
	self.msg_type = 3910
end

function CSGuildActivePersonRank:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

local function ReadGuildActivePersonRank()
    local t = {}
    t.name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
	t.prof = MsgAdapter.ReadInt()
	t.reserve_ch = MsgAdapter.ReadChar()
	t.post = MsgAdapter.ReadChar()
	t.level = MsgAdapter.ReadShort()
	t.week_acitve_val = MsgAdapter.ReadInt()
    return t
end

SCGuildActivePersonRank = SCGuildActivePersonRank or BaseClass(BaseProtocolStruct)
function SCGuildActivePersonRank:__init()
    self.msg_type = 3913
end

function SCGuildActivePersonRank:Decode()
   self.index = MsgAdapter.ReadInt()
   self.rank_list = {}
   for i = 1, MsgAdapter.ReadInt() do
        self.rank_list[i] = ReadGuildActivePersonRank()
    end
end

SCGuildActiveInfo = SCGuildActiveInfo or BaseClass(BaseProtocolStruct)
function SCGuildActiveInfo:__init()
    self.msg_type = 3911
end

function SCGuildActiveInfo:Decode()
    self.day_active_val = MsgAdapter.ReadInt()
    self.week_active_val = MsgAdapter.ReadInt()
end

SCGuildActiveInfoMember = SCGuildActiveInfoMember or BaseClass(BaseProtocolStruct)
function SCGuildActiveInfoMember:__init()
    self.msg_type = 3912
end

function SCGuildActiveInfoMember:Decode()
    self.flag = MsgAdapter.ReadInt()
	self.seq = MsgAdapter.ReadInt()
end

CSGuildActiveGetGift = CSGuildActiveGetGift or BaseClass(BaseProtocolStruct)
function CSGuildActiveGetGift:__init()
	self.msg_type = 3914
end

function CSGuildActiveGetGift:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.seq)
end

SCCustomInstructionInfo = SCCustomInstructionInfo or BaseClass(BaseProtocolStruct)
function SCCustomInstructionInfo:__init()
    self.msg_type = 3920
end
function SCCustomInstructionInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.list = {}
    for i = 1, self.count do
        self.list[i] = {index = i, desc = MsgAdapter.ReadStrN(64), type = 99}
    end
end
CSCustomInstructionAdd = CSCustomInstructionAdd or BaseClass(BaseProtocolStruct)
function CSCustomInstructionAdd:__init()
    self.msg_type = 3921
end
function CSCustomInstructionAdd:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteStrN(self.desc, 64)
end
CSCustomInstructionRemove = CSCustomInstructionRemove or BaseClass(BaseProtocolStruct)
function CSCustomInstructionRemove:__init()
    self.msg_type = 3922
end
function CSCustomInstructionRemove:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.index)
end
CSCustomInstructionBatchRemove = CSCustomInstructionBatchRemove or BaseClass(BaseProtocolStruct)
function CSCustomInstructionBatchRemove:__init()
    self.msg_type = 3923
end
function CSCustomInstructionBatchRemove:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.num)
    for i = 1, self.num do
        MsgAdapter.WriteShort(self.del_list[i])
    end
end

-- 神印地用户操作
CSPetGodFBReq = CSPetGodFBReq or BaseClass(BaseProtocolStruct)
function CSPetGodFBReq:__init()
    self.msg_type = 3930
end

function CSPetGodFBReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.req_type)
	MsgAdapter.WriteInt(self.param_1)
end

-- 神印地角色信息
SCPetGodFBRoleInfo = SCPetGodFBRoleInfo or BaseClass(BaseProtocolStruct)
function SCPetGodFBRoleInfo:__init()
    self.msg_type = 3931
end

function SCPetGodFBRoleInfo:Decode()
    -- self.info = {
    --     day_pass_times = MsgAdapter.ReadInt(),      -- 今日通关次数
    --     day_buy_times = MsgAdapter.ReadInt(),       -- 今日购买次数
    -- }
    local t = {}
    for i = 1, FightConfig.PET_GOD_FB_MAX_TYPE_NUM do
        t[i] = MsgAdapter.ReadInt()
    end
    self.info = t
end

-- 神印地副本场景信息
SCPetGodFBSceneInfo = SCPetGodFBSceneInfo or BaseClass(BaseProtocolStruct)
function SCPetGodFBSceneInfo:__init()
    self.msg_type = 3932
end

function SCPetGodFBSceneInfo:Decode()
    self.info = {
        fb_seq = MsgAdapter.ReadInt(),          -- 副本seq
        is_finish = MsgAdapter.ReadInt(),       -- 副本完成标记
        boss_npc_id = MsgAdapter.ReadInt(),     -- boss
    }
end

-- 周赏
SCWeekActiveDegreeInfo = SCWeekActiveDegreeInfo or BaseClass(BaseProtocolStruct)
function SCWeekActiveDegreeInfo:__init()
    self.msg_type = 3925
end

function SCWeekActiveDegreeInfo:Decode()
    self.reward_flag = MsgAdapter.IntFlag(2,true)  --活跃奖励领取标记[奖励index] 
    self.finish_flag = MsgAdapter.IntFlag(2)       --每日任务达成标记[任务index]
    self.integral_num = MsgAdapter.ReadInt()
end

-- 百鬼夜行
SCGhostNightInfo = SCGhostNightInfo or BaseClass(BaseProtocolStruct)
function SCGhostNightInfo:__init()
    self.msg_type = 3941
end

function SCGhostNightInfo:Decode()
    self.surplus_num = MsgAdapter.ReadShort() -- 剩余boss数量
    self.count = MsgAdapter.ReadShort()
    self.npc_info = {}
    for i=1,self.count do
        self.npc_info[i] = {
            npc_id = MsgAdapter.ReadInt(),
            scene_id = MsgAdapter.ReadInt()
        }
    end
end

--百鬼夜行翻牌
local function GhostNightPoker()
    local t = {}
    t.index = MsgAdapter.ReadInt()
    t.role_uid = MsgAdapter.ReadInt()
    t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    ItemDrawInfo(t)
    return t
end

SCGhostNightPokerInfo = SCGhostNightPokerInfo or BaseClass(BaseProtocolStruct)
function SCGhostNightPokerInfo:__init()
    self.msg_type = 3942
end

function SCGhostNightPokerInfo:Decode()
    local info = {}
    info.draw_group_id = MsgAdapter.ReadInt()
    info.timeout = MsgAdapter.ReadUInt()
    info.mypoker = {}
    for i = 1, SIDE_MAX_ROLE_NUM do
        info.mypoker[i] = GhostNightPoker()
    end
    self.info = info
end

-- 北斗七星请求
CSBigDipperReq = CSBigDipperReq or BaseClass(BaseProtocolStruct)
function CSBigDipperReq:__init()
    self.msg_type = 3950
end

function CSBigDipperReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.type)
	MsgAdapter.WriteInt(self.param1)
	MsgAdapter.WriteInt(self.param2)
end

SCBigDipperInfo = SCBigDipperInfo or BaseClass(BaseProtocolStruct)
function SCBigDipperInfo:__init()
    self.msg_type = 3951
end

function SCBigDipperInfo:Decode()
    local info = {}
	local monster_num = MsgAdapter.ReadInt()
    for i = 1, monster_num do
        info[i] = MsgAdapter.ReadInt()
    end
	if monster_num < 8 then
		--配置未刷新临时数据
		local temp = {}
		for i = 0, 7 do
			table.insert(temp,i)
		end
		self.boss_list = temp
	else
		self.boss_list = info
	end
end

MAX_BIG_DIPPER_MONSTER_NUM = 8


local function MyBigDipperInfo()
    local t = {}
    t.first_pass_flag = tonumber(MsgAdapter.ReadChar()) --是否首通
    t.first_reward_flag = tonumber(MsgAdapter.ReadChar()) --首通是否领取
	t.pass_flag = tonumber(MsgAdapter.ReadChar()) --阶段通关二进制
	t.pass_reward_flag = tonumber(MsgAdapter.ReadChar()) --阶段通关奖励领取
	t.pass_round_num = MsgAdapter.ReadInt() --通关回合数
    return t
end

SCBigDipperMyInfo = SCBigDipperMyInfo or BaseClass(BaseProtocolStruct)
function SCBigDipperMyInfo:__init()
    self.msg_type = 3952
end

function SCBigDipperMyInfo:Decode()
    local info = {}
    for i = 1, MAX_BIG_DIPPER_MONSTER_NUM do
        info[i] = MyBigDipperInfo()
    end
	
	local help_list = {}
	for i = 1, MAX_BIG_DIPPER_MONSTER_NUM do
        help_list[i] = tonumber(MsgAdapter.ReadChar())
    end
    self.my_info = info
	self.help_info = help_list
end




----------增量包下载奖励----------------
SCFetchRecourceDownloadReward  = SCFetchRecourceDownloadReward  or BaseClass(BaseProtocolStruct)
function SCFetchRecourceDownloadReward :__init()
    self.msg_type = 3956
end

function SCFetchRecourceDownloadReward:Decode()
    self.flag = MsgAdapter.ReadInt()    --0：未领取,非零：已领取
end

CSFetchRecourceDownloadReward  = CSFetchRecourceDownloadReward  or BaseClass(BaseProtocolStruct)
function CSFetchRecourceDownloadReward :__init()
    self.msg_type = 3955
end

function CSFetchRecourceDownloadReward:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end
--------增量包下载奖励结束-----------------

-- 防诈骗奖励
CSFangFraudRewardReq = CSFangFraudRewardReq or BaseClass(BaseProtocolStruct)
function CSFangFraudRewardReq:__init()
    self.msg_type = 3957
end

function CSFangFraudRewardReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(#self.answer_list)
    for i = 1, RewardConfig.ANTI_FRAUD_NUM_MAX do
        MsgAdapter.WriteShort(self.answer_list[i] or 0)
    end
end

SCFangFraudRewardInfo = SCFangFraudRewardInfo or BaseClass(BaseProtocolStruct)
function SCFangFraudRewardInfo:__init()
    self.msg_type = 3958
end

function SCFangFraudRewardInfo:Decode()
    self.info = {
        flag = MsgAdapter.ReadInt()    --0：未领取,非零：已领取
    }
end


--属性丹操作请求
CSRoleAttrPelletReq = CSRoleAttrPelletReq or BaseClass(BaseProtocolStruct)
function CSRoleAttrPelletReq:__init()
    self.msg_type = 3960
end

function CSRoleAttrPelletReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.op_type)
	MsgAdapter.WriteShort(self.param1)
	MsgAdapter.WriteInt(self.param2)
	MsgAdapter.WriteInt(self.param3)
end

MAX_ATTR_PELLET_TYPE_NUM = 30
PARTNER_NUM_MAX = 40

--属性丹所有信息下发
SCRoleAttrPelletAllInfo = SCRoleAttrPelletAllInfo or BaseClass(BaseProtocolStruct)
function SCRoleAttrPelletAllInfo:__init()
    self.msg_type = 3961
end

function SCRoleAttrPelletAllInfo:Decode()
	local pellet_type_num = MsgAdapter.ReadInt()
	self.role_use_attr_list = {}
	for i = 1, pellet_type_num do
        local count = MsgAdapter.ReadShort() --使用次数
		self.role_use_attr_list[i] = count
    end
	self.partner_use_attr_list = {}
	local partner_num = MsgAdapter.ReadInt()
	for i = 1, partner_num do
		self.partner_use_attr_list[i] = {}
		for k = 1, pellet_type_num do
			local count = MsgAdapter.ReadShort() --使用次数
			self.partner_use_attr_list[i][k] = count
		end
    end
end


--属性丹单个信息下发
SCRoleAttrPelletSingleInfo = SCRoleAttrPelletSingleInfo or BaseClass(BaseProtocolStruct)
function SCRoleAttrPelletSingleInfo:__init()
    self.msg_type = 3962
end

function SCRoleAttrPelletSingleInfo:Decode()
	self.role_attr_pellet_type = MsgAdapter.ReadShort()
	self.use_count = MsgAdapter.ReadShort()
	self.attr_pellet_type = MsgAdapter.ReadInt()
	self.param3 = MsgAdapter.ReadInt()
end


-- 活跃转盘操作请求
CSActiveTurntableReq = CSActiveTurntableReq or BaseClass(BaseProtocolStruct)
function CSActiveTurntableReq:__init()
    self.msg_type = 3965
end

function CSActiveTurntableReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(0)
end

-- 活跃转盘任务信息下发
SCActiveTurntableInfo = SCActiveTurntableInfo or BaseClass(BaseProtocolStruct)
function SCActiveTurntableInfo:__init()
    self.msg_type = 3966
end

function SCActiveTurntableInfo:Decode()
	self.can_roller_times = MsgAdapter.ReadInt()
	self.day_task_flag = MsgAdapter.IntFlag(2)
	self.week_task_flag = MsgAdapter.IntFlag(2)
end

-- 活跃转盘抽奖返回
SCActiveTurntableRewardInfo = SCActiveTurntableRewardInfo or BaseClass(BaseProtocolStruct)
function SCActiveTurntableRewardInfo:__init()
    self.msg_type = 3967
end

function SCActiveTurntableRewardInfo:Decode()
	self.seq = MsgAdapter.ReadInt()
    self.can_roller_times = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.item_list = {}
    for i = 1, self.count do
        local tab = {}
        tab.item_id = MsgAdapter.ReadUShort()
        tab.is_bind = MsgAdapter.ReadShort()
        tab.num = MsgAdapter.ReadInt()
        table.insert(self.item_list, tab)
    end
end

--渡劫
CSGoThroughReq = CSGoThroughReq or BaseClass(BaseProtocolStruct)
function CSGoThroughReq:__init()
	self.msg_type = 3935
end

function CSGoThroughReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param_1)
end

SCGoThroughInfo = SCGoThroughInfo or BaseClass(BaseProtocolStruct)
function SCGoThroughInfo:__init()
    self.msg_type = 3936
end

function SCGoThroughInfo:Decode()
    self.max_pass_level = MsgAdapter.ReadInt()
    self.pass_level = self.max_pass_level + 1
    self.progress = MsgAdapter.ReadInt()
    self.assist_times = MsgAdapter.ReadInt()
end

SCGoThroughSceneInfo = SCGoThroughSceneInfo or BaseClass(BaseProtocolStruct)
function SCGoThroughSceneInfo:__init()
    self.msg_type = 3937
end

function SCGoThroughSceneInfo:Decode()
    self.level = MsgAdapter.ReadInt()
    self.is_finish = MsgAdapter.ReadInt()
end

CSCapacityTargetReq = CSCapacityTargetReq or BaseClass(BaseProtocolStruct)
function CSCapacityTargetReq:__init()
	self.msg_type = 3975
end

function CSCapacityTargetReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)      --0:开始邀战|1:领取奖励
end



SCCapacityTargetInfo = SCCapacityTargetInfo or BaseClass(BaseProtocolStruct)
function SCCapacityTargetInfo:__init()
    self.msg_type = 3976
end

function SCCapacityTargetInfo:Decode()
    self.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    self.advance_times = MsgAdapter.ReadInt()
    self.avatar_type = MsgAdapter.ReadInt()
    self.avatar_id = MsgAdapter.ReadInt()
    self.color = MsgAdapter.ReadInt()
    self.prof = MsgAdapter.ReadInt()
    self.weapon_id = MsgAdapter.ReadInt()
    self.sub_weapon_id = MsgAdapter.ReadInt()

    self.capacity_target_end_time = MsgAdapter.ReadUInt()
    self.capacity_target_can_start = MsgAdapter.ReadChar()
    self.capacity_target_is_finish = MsgAdapter.ReadChar()
    self.capacity_target_is_fetch_reward = MsgAdapter.ReadShort()
    self.capacity_target_value = MsgAdapter.ReadInt()
    self.capacity_target_robot_value = MsgAdapter.ReadInt()
    self.capacity_target_begin_time = MsgAdapter.ReadUInt()

    self.capacity_target_can_start = self.capacity_target_can_start ~= 0
    self.capacity_target_is_finish = self.capacity_target_is_finish ~= 0
    self.capacity_target_is_fetch_reward = self.capacity_target_is_fetch_reward ~= 0
   
end

--殒神之地我的通关信息
MAX_FALLEN_GOD_MONSTER_NUM = 9  --协议最大值
SCFallenGodInfo = SCFallenGodInfo or BaseClass(BaseProtocolStruct)
function SCFallenGodInfo:__init()
    self.msg_type = 3953
end

function SCFallenGodInfo:Decode()
    self.monster_list = {}
    for i = 0, MAX_FALLEN_GOD_MONSTER_NUM - 1 do
        self.monster_list[i] = MyBigDipperInfo() --boss信息
    end
    self.help_list = {}
    for i = 0, MAX_FALLEN_GOD_MONSTER_NUM - 1 do
        self.help_list[i] = MsgAdapter.ReadChar() --助战次数
    end
    --LogError("monster_list -> ", self.monster_list)
    --LogError("help_list -> ", self.help_list)
end

CSFallenGodReq = CSFallenGodReq or BaseClass(BaseProtocolStruct)
function CSFallenGodReq:__init()
    self.msg_type = 3954
end

function CSFallenGodReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

-- 评价有礼领取
CSFetchPingJiaRewardReq = CSFetchPingJiaRewardReq or BaseClass(BaseProtocolStruct)
function CSFetchPingJiaRewardReq:__init()
    self.msg_type = 3968
end

function CSFetchPingJiaRewardReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

-- 评价有礼是否领取
SCFetchPingJiaRewardInfo = SCFetchPingJiaRewardInfo or BaseClass(BaseProtocolStruct)
function SCFetchPingJiaRewardInfo:__init()
    self.msg_type = 3969
end

function SCFetchPingJiaRewardInfo:Decode()
    self.flag = MsgAdapter.ReadInt()
end