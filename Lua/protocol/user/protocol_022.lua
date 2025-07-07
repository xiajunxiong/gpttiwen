
CSWorldBossReq = CSWorldBossReq or BaseClass(BaseProtocolStruct)
function CSWorldBossReq:__init()
    self.msg_type = 2200
end

function CSWorldBossReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
end

CSWorldBoss2Req = CSWorldBoss2Req or BaseClass(BaseProtocolStruct)
function CSWorldBoss2Req:__init()
    self.msg_type = 3200
end

function CSWorldBoss2Req:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
end

SCWorldBossInfo = SCWorldBossInfo or BaseClass(BaseProtocolStruct)
function SCWorldBossInfo:__init()
    self.msg_type = 2201
end

function SCWorldBossInfo:Decode()
	self.boss_info = {
		boss_npc_id = MsgAdapter.ReadInt(),
		boss_max_hp = MsgAdapter.ReadInt(),
		boss_cur_hp = MsgAdapter.ReadInt(),
	}
end

SCWorldBoss2Info = SCWorldBoss2Info or BaseClass(BaseProtocolStruct)
function SCWorldBoss2Info:__init()
    self.msg_type = 3201
end

function SCWorldBoss2Info:Decode()
	self.boss_info = {
		boss_npc_id = MsgAdapter.ReadInt(),
		boss_max_hp = MsgAdapter.ReadInt(),
		boss_cur_hp = MsgAdapter.ReadInt(),
	}
end

SCWorldBossResult = SCWorldBossResult or BaseClass(BaseProtocolStruct)
function SCWorldBossResult:__init()
    self.msg_type = 2202
end

function SCWorldBossResult:Decode()
	local t = {}
	local rank_type = WorldBossConfig.RankType
	local ri = RoleData.Instance:GetRoleId()
	t[rank_type.hurt] = {}
	for i = 1, WorldBossConfig.RANK_NUM_MAX do
		local role_id = MsgAdapter.ReadInt()
		local name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		if not StringIsEmpty(name) then table.insert(t[rank_type.hurt], {role_id = role_id, name = name, is_self = ri == role_id}) end
	end
	t[rank_type.heal] = {}
	for i = 1, WorldBossConfig.RANK_NUM_MAX do
		local role_id = MsgAdapter.ReadInt()
		local name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		if not StringIsEmpty(name) then table.insert(t[rank_type.heal], {role_id = role_id, name = name, is_self = ri == role_id}) end
	end
	t[rank_type.luck] = {}
	for i = 1, WorldBossConfig.RANK_NUM_MAX do
		local role_id = MsgAdapter.ReadInt()
		local name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		if not StringIsEmpty(name) then table.insert(t[rank_type.luck], {role_id = role_id, name = name, is_self = ri == role_id}) end
	end
	self.rank_list = t
end

SCWorldBoss2Result = SCWorldBoss2Result or BaseClass(BaseProtocolStruct)
function SCWorldBoss2Result:__init()
    self.msg_type = 3202
end

function SCWorldBoss2Result:Decode()
	local t = {}
	local rank_type = WorldBossConfig.RankType
	local ri = RoleData.Instance:GetRoleId()
	t[rank_type.hurt] = {}
	for i = 1, WorldBossConfig.RANK_NUM_MAX do
		local role_id = MsgAdapter.ReadInt()
		local name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		if not StringIsEmpty(name) then table.insert(t[rank_type.hurt], {role_id = role_id, name = name, is_self = ri == role_id}) end
	end
	t[rank_type.heal] = {}
	for i = 1, WorldBossConfig.RANK_NUM_MAX do
		local role_id = MsgAdapter.ReadInt()
		local name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		if not StringIsEmpty(name) then table.insert(t[rank_type.heal], {role_id = role_id, name = name, is_self = ri == role_id}) end
	end
	t[rank_type.luck] = {}
	for i = 1, WorldBossConfig.RANK_NUM_MAX do
		local role_id = MsgAdapter.ReadInt()
		local name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		if not StringIsEmpty(name) then table.insert(t[rank_type.luck], {role_id = role_id, name = name, is_self = ri == role_id}) end
	end
	self.rank_list = t
end

SCWorldBossPosInfo = SCWorldBossPosInfo or BaseClass(BaseProtocolStruct)
function SCWorldBossPosInfo:__init()
    self.msg_type = 2203
end

function SCWorldBossPosInfo:Decode()
	self.pos_info = {
		id = MsgAdapter.ReadInt(),
		scene_id = MsgAdapter.ReadInt(),
		x = MsgAdapter.ReadInt(),
		y = MsgAdapter.ReadInt(),
	}
end

SCWorldBoss2PosInfo = SCWorldBoss2PosInfo or BaseClass(BaseProtocolStruct)
function SCWorldBoss2PosInfo:__init()
    self.msg_type = 3203
end

function SCWorldBoss2PosInfo:Decode()
	self.pos_info = {
		id = MsgAdapter.ReadInt(),
		scene_id = MsgAdapter.ReadInt(),
		x = MsgAdapter.ReadInt(),
		y = MsgAdapter.ReadInt(),
	}
end

-- 世界boss一些战斗数据统计
SCWorldBossBattleInfo = SCWorldBossBattleInfo or BaseClass(BaseProtocolStruct)
function SCWorldBossBattleInfo:__init()
    self.msg_type = 2204
end

function SCWorldBossBattleInfo:Decode()
	self.battle_info = {
		team_total_damage = MsgAdapter.ReadLL(),
		team_total_heal = MsgAdapter.ReadLL(),
	}
end

SCWorldBossRoleInfo = SCWorldBossRoleInfo or BaseClass(BaseProtocolStruct)
function SCWorldBossRoleInfo:__init()
    self.msg_type = 2205
end
function SCWorldBossRoleInfo:Decode()
	self.play_times = MsgAdapter.ReadInt()
    self.shanghai = MsgAdapter.ReadInt()
    self.zhiliao = MsgAdapter.ReadInt()
end

SCWorldBossRankInfo = SCWorldBossRankInfo or BaseClass(BaseProtocolStruct)
function SCWorldBossRankInfo:__init()
    self.msg_type = 2206
end
function SCWorldBossRankInfo:Decode()
    self.type = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.hurt_rank = {}
    for i = 1, self.count, 1 do
        self.hurt_rank[i] = self:RankItemInfo(i, self.type)
    end
end
function SCWorldBossRankInfo:RankItemInfo(rank, type)
    return {
        rank = rank,
        type = type,
        role_id = MsgAdapter.ReadInt(),
        name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        level = MsgAdapter.ReadInt(),
        prof = MsgAdapter.ReadInt(),
        value = MsgAdapter.ReadLL()
    }
end


-- 世界boss一些战斗数据统计
SCWorldBoss2BattleInfo = SCWorldBoss2BattleInfo or BaseClass(BaseProtocolStruct)
function SCWorldBoss2BattleInfo:__init()
    self.msg_type = 3204
end

function SCWorldBoss2BattleInfo:Decode()
	self.battle_info = {
		team_total_damage = MsgAdapter.ReadLL(),
		team_total_heal = MsgAdapter.ReadLL(),
	}
end

--[[ -- 世界boss2玩家信息
SCWorldBoss2RoleInfo = SCWorldBoss2RoleInfo or BaseClass(BaseProtocolStruct)
function SCWorldBoss2RoleInfo:__init()
    self.msg_type = 3199
end

function SCWorldBoss2RoleInfo:Decode()
	self.play_times = MsgAdapter.ReadInt()
end ]]


-- 所有运镖信息
SCEscortAllInfo = SCEscortAllInfo or BaseClass(BaseProtocolStruct)
function SCEscortAllInfo:__init()
    self.msg_type = 2220
end

function SCEscortAllInfo:Decode()
	self.escort_info = {
		today_count = MsgAdapter.ReadInt(),			
		task_seq = MsgAdapter.ReadInt(),
		npc_seq = MsgAdapter.ReadInt(),
		end_time = MsgAdapter.ReadUInt(),
	}
end

-- 运镖操作
CSEscortOp = CSEscortOp or BaseClass(BaseProtocolStruct)
function CSEscortOp:__init()
    self.msg_type = 2221
end

function CSEscortOp:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

-- 运镖结果通知
SCEscortResultNotic = SCEscortResultNotic or BaseClass(BaseProtocolStruct)
function SCEscortResultNotic:__init()
    self.msg_type = 2222
end

function SCEscortResultNotic:Decode()
    self.is_win = MsgAdapter.ReadInt()
end

-- 运镖状态改变后 通知周围玩家
SCEscortStatusChangeNotice = SCEscortStatusChangeNotice or BaseClass(BaseProtocolStruct)
function SCEscortStatusChangeNotice:__init()
    self.msg_type = 2223
end

function SCEscortStatusChangeNotice:Decode()
    self.obj_id = MsgAdapter.ReadUShort()
    MsgAdapter.ReadShort()
    self.escort_id = MsgAdapter.ReadInt()
end

-- 宝图操作
CSTreasureMapReq = CSTreasureMapReq or BaseClass(BaseProtocolStruct)
function CSTreasureMapReq:__init()
	self.msg_type = 2210
end
function CSTreasureMapReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.req_type)
	MsgAdapter.WriteInt(self.param1)
	MsgAdapter.WriteShort(self.param2)
	MsgAdapter.WriteShort(self.param3)
end

SCTreasureMapInfo = SCTreasureMapInfo or BaseClass(BaseProtocolStruct)
function SCTreasureMapInfo:__init()
	self.msg_type = 2211
end

function SCTreasureMapInfo:Decode()
	self.play_times = MsgAdapter.ReadShort()
	self.get_reward_times = MsgAdapter.ReadShort()
	self.pos_x = MsgAdapter.ReadInt()
	self.pos_y = MsgAdapter.ReadInt()
	self.monster_id = MsgAdapter.ReadInt()
	self.scene_id = MsgAdapter.ReadInt()
	self.task_id = MsgAdapter.ReadInt()
	self.is_new = MsgAdapter.ReadShort()
	MsgAdapter.ReadShort()
end

SCTreasureMapItemInfo = SCTreasureMapItemInfo or BaseClass(BaseProtocolStruct)
function SCTreasureMapItemInfo:__init()
	self.msg_type = 2212
end
function SCTreasureMapItemInfo:Decode()
	self.scene_id = MsgAdapter.ReadInt()
	self.x = MsgAdapter.ReadShort()
	self.y = MsgAdapter.ReadShort()
end
SCTreasureMapItemInfoOpenResult = SCTreasureMapItemInfoOpenResult or BaseClass(BaseProtocolStruct)
function SCTreasureMapItemInfoOpenResult:__init()
	self.msg_type = 2213
end
function SCTreasureMapItemInfoOpenResult:Decode()
	self.seq = MsgAdapter.ReadInt()
	self.index = MsgAdapter.ReadInt()
	self.item_id  = MsgAdapter.ReadInt()--用于判断高低藏宝图，高级藏宝图才会遇怪，再次发送既可以遇怪
end

SCTreasureMapBatchUsedResult = SCTreasureMapBatchUsedResult or BaseClass(BaseProtocolStruct)
function SCTreasureMapBatchUsedResult:__init()
	self.msg_type = 2214
end
function SCTreasureMapBatchUsedResult:Decode()
	self.coin = MsgAdapter.ReadInt()
	self.count = MsgAdapter.ReadInt()
	self.rewards = {}
	local temp = {}
	for i = 1, self.count, 1 do
		temp = {}
		temp.item_id = MsgAdapter.ReadUShort()
		temp.is_bind = MsgAdapter.ReadShort()
		temp.num = MsgAdapter.ReadInt()
		self.rewards[i] = Item.Create(temp)
	end
end

SCMapUnlockFristNotice = SCMapUnlockFristNotice or BaseClass(BaseProtocolStruct)
function SCMapUnlockFristNotice:__init()
	self.msg_type = 2231
	self.active_id = 0
end

function SCMapUnlockFristNotice:Decode()
	self.active_id = MsgAdapter.ReadInt()
end

SCMapUnlockAllListInfo = SCMapUnlockAllListInfo or BaseClass(BaseProtocolStruct)
function SCMapUnlockAllListInfo:__init()
	self.msg_type = 2230
end

function SCMapUnlockAllListInfo:Decode()
	self.irregular_flag_list = {}
	self.new_unlock_flag_list = {}
	for i = 1, MapConfig.IRREGULAR_ID_LIST_NUM_MAX do
		self.irregular_flag_list[i] = MsgAdapter.ReadLL()
	end
	for i = 1, MapConfig.IRREGULAR_ID_LIST_NUM_MAX do
		self.new_unlock_flag_list[i] = MsgAdapter.ReadLL()
	end
end

CSMapUnlockRemoveAnim = CSMapUnlockRemoveAnim or BaseClass(BaseProtocolStruct)
function CSMapUnlockRemoveAnim:__init()
	self.msg_type = 2232
end

function CSMapUnlockRemoveAnim:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.remove_irregular_id)
end
SCMapUnlockFirstEnterFbType = SCMapUnlockFirstEnterFbType or BaseClass(BaseProtocolStruct)
function SCMapUnlockFirstEnterFbType:__init()
	self.msg_type = 2233
end
function SCMapUnlockFirstEnterFbType:Decode()
	self.count = MsgAdapter.ReadInt()
	self.fb_type_enter_flag = {}
	for i = 1, self.count do
		self.fb_type_enter_flag[i] = MsgAdapter.ReadLL()--从0开始
	end
end

--挑战副本通关数据，表示每个章节已经通关到第几关
SCChallengeFbRoleInfor = SCChallengeFbRoleInfor or BaseClass(BaseProtocolStruct)
function SCChallengeFbRoleInfor:__init()
	self.msg_type = 2240
end

function SCChallengeFbRoleInfor:Decode()
	self.highest_pass_chapter = {}
	for i=1,ChallengeData.PROTOCOL_FBID_MAX do
		self.highest_pass_chapter[i-1] = MsgAdapter.ReadChar()	--副本ID从0开始数
	end
	MsgAdapter.ReadUShort()			--reserved
	self.next_refresh_timestamp = MsgAdapter.ReadUInt()
end

--挑战副本通关信息
SCChallengeFbAwardList = SCChallengeFbAwardList or BaseClass(BaseProtocolStruct)
function SCChallengeFbAwardList:__init()
	self.msg_type = 2241
end

function SCChallengeFbAwardList:Decode()
	local list_num = MsgAdapter.ReadInt()
	self.reward_list = {}
	for i=1,list_num do
		local reward = {}
		reward.item_id = MsgAdapter.ReadUShort()
		reward.item_num = MsgAdapter.ReadUShort()
		self.reward_list[i] = reward
	end
end

----------------------------------------------------------
--七日登录 神秘试炼
----------------------------------------------------------
CSMysteriousTrialReq = CSMysteriousTrialReq or BaseClass(BaseProtocolStruct)

function CSMysteriousTrialReq:__init()
    self.msg_type = 2250
end

function CSMysteriousTrialReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

SCMysteriousTrialInfo = SCMysteriousTrialInfo or BaseClass(BaseProtocolStruct)
function SCMysteriousTrialInfo:__init()
    self.msg_type = 2251
end

function SCMysteriousTrialInfo:Decode()
    self.day_num = MsgAdapter.ReadChar()   --活动天数
    self.goal_num = MsgAdapter.ReadChar()  --当前解锁天数 目标天数
    self.task_num = MsgAdapter.ReadShort() --已完成的任务数量
	self.type = MsgAdapter.ReadShort()
	MsgAdapter.ReadShort()
    self.reward_flag = MsgAdapter.ReadInt()--奖励领取flag
	SevenDayData.Instance:CheckSevenDayData(self.type)
    self.goal_info = {}
    for i=1,self.goal_num do
        self.goal_info[i] = {}
        for j=0,SEVEN_DAY_GOAL_MAX_NUM - 1 do
            local info = {}
            info.seq = j
            info.start_time = i
			info.state = MsgAdapter.ReadUShort()
			MsgAdapter.ReadShort()
			info.progress = MsgAdapter.ReadInt()
            self.goal_info[i][j] = info
            if info.state == 1 then
                SevenDayData.Instance:CheckTaskRemin(i,j,true)
            end
        end
	end
	local t = os.date("*t",TimeHelper:GetServerTime())
	t.day = t.day + (SEVEN_DAY_MAX_NUM - self.day_num + 1)
	self.time_stamp = TimeCtrl.Instance:ChangDataStampTime(t)
end

--七天目标单独下发
SCMysteriousTrialSingleInfo = SCMysteriousTrialSingleInfo or BaseClass(BaseProtocolStruct)
function SCMysteriousTrialSingleInfo:__init()
    self.msg_type = 2252
end

function SCMysteriousTrialSingleInfo:Decode()
    local info = {}
    info.start_time = MsgAdapter.ReadChar()
    info.seq = MsgAdapter.ReadChar()
    info.state = MsgAdapter.ReadShort()
	info.progress = MsgAdapter.ReadInt()
	self.task_num = MsgAdapter.ReadShort()
	MsgAdapter.ReadShort()
    self.info = info
end

--七天奖励单独下发
SCMysteriousRewardInfo = SCMysteriousRewardInfo or BaseClass(BaseProtocolStruct)
function SCMysteriousRewardInfo:__init()
    self.msg_type = 2253
end

function SCMysteriousRewardInfo:Decode()
    self.reward_flag = MsgAdapter.ReadInt()
end

--远洋贸易请求
CSTransactionTaskReq = CSTransactionTaskReq or BaseClass(BaseProtocolStruct)

function CSTransactionTaskReq:__init()
    self.msg_type = 2255
end

function CSTransactionTaskReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteInt(self.index)
    MsgAdapter.WriteInt(self.num)
end

--远洋贸易下发
SCTransactionTaskInfo = SCTransactionTaskInfo or BaseClass(BaseProtocolStruct)
function SCTransactionTaskInfo:__init()
    self.msg_type = 2256
end

function SCTransactionTaskInfo:Decode()
	self.task_id = MsgAdapter.ReadInt()
	self.task_status = MsgAdapter.ReadShort()--0 进行中 1已完成
	self.version_reset = MsgAdapter.ReadShort()
	self.level = MsgAdapter.ReadShort()
	self.type = MsgAdapter.ReadShort()
	self.seq_list = {}
	for i=1,9 do
		self.seq_list[i] = {index = i,seq = MsgAdapter.ReadShort()}
	end
	MsgAdapter.ReadShort()
	self.reward_flag = MsgAdapter.ReadInt()
end

--活跃系统请求
CSActiveSystemReq = CSActiveSystemReq or BaseClass(BaseProtocolStruct)

function CSActiveSystemReq:__init()
    self.msg_type = 2257
end

function CSActiveSystemReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteInt(self.index)
end

--活跃系统下发
SCActiveSystemInfo = SCActiveSystemInfo or BaseClass(BaseProtocolStruct)
function SCActiveSystemInfo:__init()
    self.msg_type = 2258
end

function SCActiveSystemInfo:Decode()
	self.level = MsgAdapter.ReadInt()
	self.active_num = MsgAdapter.ReadInt()
	self.reward_flag = MsgAdapter.ReadInt() --宝箱领取
	self.progress_list = {}
	self.count = MsgAdapter.ReadInt() - 1
	for i=0,self.count do
		self.progress_list[i] = MsgAdapter.ReadInt()
	end
	self.flag_list = {}
	for i=0,self.count do
		self.flag_list[i] = MsgAdapter.ReadInt() --各个类型的额外奖励领取情况 （0:不可领取  1:可领取但未领取  2:已领取)
	end
end

--活跃系统下发
SCActiveSystemSingleInfo = SCActiveSystemSingleInfo or BaseClass(BaseProtocolStruct)
function SCActiveSystemSingleInfo:__init()
    self.msg_type = 2264
end

function SCActiveSystemSingleInfo:Decode()
	self.level = MsgAdapter.ReadInt()
	self.active_num = MsgAdapter.ReadInt()
	self.reward_flag = MsgAdapter.ReadInt()
	self.type = MsgAdapter.ReadInt()
	self.progress = MsgAdapter.ReadInt()
	self.flag = MsgAdapter.ReadInt()
end

--领取活跃额外奖励
CSActiveSystemExtraRewarrdFetch = CSActiveSystemExtraRewarrdFetch or BaseClass(BaseProtocolStruct)
function CSActiveSystemExtraRewarrdFetch:__init()
    self.msg_type = 2265
end

function CSActiveSystemExtraRewarrdFetch:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.type)
end

SCSpecialActiveInfo = SCSpecialActiveInfo or BaseClass(BaseProtocolStruct)
function SCSpecialActiveInfo:__init()
	self.msg_type = 2266
end

function SCSpecialActiveInfo:Decode()
	self.special_active_reward_flag = MsgAdapter.IntFlag()
end

--任务一键完成
CSOnekeyFinish = CSOnekeyFinish or BaseClass(BaseProtocolStruct)
function CSOnekeyFinish:__init()
    self.msg_type = 2259
end

function CSOnekeyFinish:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
end

CSPokerReq = CSPokerReq or BaseClass(BaseProtocolStruct)
function CSPokerReq:__init()
	self.msg_type = 2260
end

function CSPokerReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.oper_type) --POKER_REQ_OPER_TYPE
	MsgAdapter.WriteInt(self.role_uid)
	MsgAdapter.WriteInt(self.index)	--[1~5]
	MsgAdapter.WriteInt(self.reason) --POKER_REQ_REASON_TYPE
end

local function ReadHeartSkillInfo(index)
    local t = {}
	t.index = index or 0
	t.seq = MsgAdapter.ReadShort()
	t.level = MsgAdapter.ReadShort()
	if index == nil then
		MsgAdapter.ReadInt()
	end
    return t
end

--心法请求
CSHeartSkillReq = CSHeartSkillReq or BaseClass(BaseProtocolStruct)
function CSHeartSkillReq:__init()
    self.msg_type = 2270
end

function CSHeartSkillReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.op_type)
	MsgAdapter.WriteInt(self.param1)
	MsgAdapter.WriteInt(self.param2)
	MsgAdapter.WriteInt(self.param3)
end

--心法合成请求
CSHeartSkillDisplaceReq = CSHeartSkillDisplaceReq or BaseClass(BaseProtocolStruct)
function CSHeartSkillDisplaceReq:__init()
    self.msg_type = 2274
end

function CSHeartSkillDisplaceReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.count)
	for i=1,self.count do
		local info = self.info_list[i]
		MsgAdapter.WriteShort(info.column_type)
		MsgAdapter.WriteShort(info.index)
	end
end


SCHeartSkillAllInfo = SCHeartSkillAllInfo or BaseClass(BaseProtocolStruct)
function SCHeartSkillAllInfo:__init()
    self.msg_type = 2271
end

function SCHeartSkillAllInfo:Decode()
	self.unlock_flag = bit:d8b(MsgAdapter.ReadUChar()) --解锁标识
	self.equip_skill_list = {}
	for i=0,5 do
		self.equip_skill_list[i] = MsgAdapter.ReadShort() --此时装备的技能列表	下列列表的index
	end
	local initiat_skill_count = MsgAdapter.ReadShort()
	local passive_skill_count = MsgAdapter.ReadShort()
	self.initiat_skill_list = {}
	for i=0,initiat_skill_count-1 do
		local info = ReadHeartSkillInfo(i)
		self.initiat_skill_list[info.seq] = info
	end
	self.passive_skill_list = {}
	for i=0,passive_skill_count-1 do
		local info = ReadHeartSkillInfo(i)
		self.passive_skill_list[info.seq] = info
	end
end

SCHeartSkillSingleInfo = SCHeartSkillSingleInfo or BaseClass(BaseProtocolStruct)
function SCHeartSkillSingleInfo:__init()
    self.msg_type = 2272
end

function SCHeartSkillSingleInfo:Decode()
	local count = MsgAdapter.ReadInt()
	self.type = MsgAdapter.ReadShort()
	local index = MsgAdapter.ReadShort()
	self.info = ReadHeartSkillInfo()
	self.info.index = index
end

SCHeartSkillOtherInfo = SCHeartSkillOtherInfo or BaseClass(BaseProtocolStruct)
function SCHeartSkillOtherInfo:__init()
    self.msg_type = 2273
end

function SCHeartSkillOtherInfo:Decode()
	MsgAdapter.ReadChar()
	self.unlock_flag = bit:d8b(MsgAdapter.ReadUChar()) --解锁标识
	self.equip_skill_list = {}
	MsgAdapter.ReadShort()
	for i=0,5 do
		self.equip_skill_list[i] = MsgAdapter.ReadShort()
	end
end

--月卡周卡请求
CSMonthCardReq = CSMonthCardReq or BaseClass(BaseProtocolStruct)
function CSMonthCardReq:__init()
	self.msg_type = 2280
end

function CSMonthCardReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.req_type)
	MsgAdapter.WriteInt(self.param1)
	MsgAdapter.WriteInt(self.param2)
end

--月卡周卡信息下发
SCMonthCardInfo = SCMonthCardInfo or BaseClass(BaseProtocolStruct)
function SCMonthCardInfo:__init()
	self.msg_type = 2281
end

function SCMonthCardInfo:Decode()
	self.is_fetch_card_day_reward_flag = MsgAdapter.ReadUChar() --月卡/周卡每日奖励领取标识 二进制(card_type)
	MsgAdapter.ReadChar()
	MsgAdapter.ReadShort()
	self.week_card_end_timestamp = MsgAdapter.ReadUInt() --周卡结束时间戳
	self.month_card_end_timestamp = MsgAdapter.ReadUInt() --月卡结束时间戳
	self.card_shop_buy_times = {} --商城购买记录
	local MAX_MONTH_CARD_SHOP_ITEM_NUM = 30
	for i = 0, MAX_MONTH_CARD_SHOP_ITEM_NUM - 1 do
		self.card_shop_buy_times[i] = MsgAdapter.ReadInt()
	end
end

--声望任务数据下发
SCPrestigeTaskInfo = SCPrestigeTaskInfo or BaseClass(BaseProtocolStruct)
function SCPrestigeTaskInfo:__init()
	self.msg_type = 2290
end

function SCPrestigeTaskInfo:Decode()
	self.village_id = MsgAdapter.ReadShort()		--今天选择的村庄id
	self.is_accept_task = MsgAdapter.ReadShort()	--是否接取任务(是否开始)
	self.today_finish_task_num = MsgAdapter.ReadShort() --今日已完成的任务数
	self.today_task_num = MsgAdapter.ReadShort()	--今天的任务数
	self.today_task_list = {}	--今日任务列表
	for i = 1, self.today_task_num do
		local data = {}
		data.task_id = MsgAdapter.ReadUShort()
		data.star = MsgAdapter.ReadShort()
		self.today_task_list[i] = data
	end
end

--选择声望任务村庄
CSPrestigeTaskSelectVillage = CSPrestigeTaskSelectVillage or BaseClass(BaseProtocolStruct)
function CSPrestigeTaskSelectVillage:__init()
	self.msg_type = 2291
end

function CSPrestigeTaskSelectVillage:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.village_id)
end

--选择声望任务村庄
SCPrestigeTaskFinalNotice = SCPrestigeTaskFinalNotice or BaseClass(BaseProtocolStruct)
function SCPrestigeTaskFinalNotice:__init()
	self.msg_type = 2292
end

function SCPrestigeTaskFinalNotice:Decode()
	self.village_id = MsgAdapter.ReadInt()
	self.total_add_prestige = MsgAdapter.ReadInt() --总获得声望
end

--声望任务请求
CSPrestigeTaskReq = CSPrestigeTaskReq or BaseClass(BaseProtocolStruct)
function CSPrestigeTaskReq:__init()
	self.msg_type = 2293
end

function CSPrestigeTaskReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteShort(self.req_type)
	MsgAdapter.WriteShort(self.param)
end