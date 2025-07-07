
SCActivityStatus = SCActivityStatus or BaseClass(BaseProtocolStruct)
function SCActivityStatus:__init()
	self.msg_type = 1200
end

function SCActivityStatus:Decode()
	local t = {}
	t.activity_open_type = MsgAdapter.ReadInt()
	t.act_type = MsgAdapter.ReadShort()
	t.status = MsgAdapter.ReadChar()
	t.is_broadcast = MsgAdapter.ReadChar()
	t.next_status_switch_time = MsgAdapter.ReadUInt()
	t.param_1 = MsgAdapter.ReadUInt()
	t.param_2 = MsgAdapter.ReadUInt()
	self.act_status_info = t
end

-- 活动对应的房间信息
SCActivityRoomStatusAck = SCActivityRoomStatusAck or BaseClass(BaseProtocolStruct)
function SCActivityRoomStatusAck:__init()
	self.msg_type = 1201
end

function SCActivityRoomStatusAck:Decode()
	self.activity_type = MsgAdapter.ReadInt()
	self.room_user_max = MsgAdapter.ReadInt()
	self.room_status_list = {}
	for i = 1, ActivityConfig.ACTIVITY_ROOM_MAX do
		self.room_status_list[i] = {
			is_open = MsgAdapter.ReadInt(),
			role_num = MsgAdapter.ReadInt(),
		}
	end
end

-- 活动房间状态请求
CSActivityRoomStatusReq = CSActivityRoomStatusReq or BaseClass(BaseProtocolStruct)
function CSActivityRoomStatusReq:__init()
	self.msg_type = 1202
end

function CSActivityRoomStatusReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.activity_type)
end

-- 进入活动
CSActivityEnterReq = CSActivityEnterReq or BaseClass(BaseProtocolStruct)
function CSActivityEnterReq:__init()
	self.msg_type = 1203
end

function CSActivityEnterReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.activity_type)
    MsgAdapter.WriteInt(self.room_index)
end

--小队竞技请求
CSActivityTeamFightMatchReq = CSActivityTeamFightMatchReq or BaseClass(BaseProtocolStruct)
function CSActivityTeamFightMatchReq:__init()
	self.msg_type = 1204
end

function CSActivityTeamFightMatchReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.request_type)--1为请求匹配，0为取消匹配，2为获取个人信息，3为获取排行榜信息4为获取战报，5为只看我的战报
	MsgAdapter.WriteInt(self.param1)
end

--小队竞技下发
SCActivityTeamFightScore = SCActivityTeamFightScore or BaseClass(BaseProtocolStruct)

function SCActivityTeamFightScore:__init()
	self.msg_type = 1206
end

function SCActivityTeamFightScore:Decode()
	local info = {}
	info.score = MsgAdapter.ReadInt()
	info.rank = MsgAdapter.ReadInt()
	info.victory_count = MsgAdapter.ReadInt()
	info.defeat_count = MsgAdapter.ReadInt()
	info.continue_victory_count = MsgAdapter.ReadInt()
	if info.continue_victory_count == 1 then
		info.continue_victory_count = 0
	end
	info.box_fetch_flag = bit:d2b(MsgAdapter.ReadInt())
	info.battle_count = MsgAdapter.ReadInt()
	self.score_data = info

	local score_value = TeamSportData.Instance:GetScoreValue()
	self.score_value = info.score
	self.change_value = score_value - info.score
	LogError("小队竞技下发",self)
end

SCActivityTeamFightNotice = SCActivityTeamFightNotice or BaseClass(BaseProtocolStruct)
function SCActivityTeamFightNotice:__init()
	self.msg_type = 1205
end

function SCActivityTeamFightNotice:Decode()
	self.is_match = MsgAdapter.ReadInt()  --0为没有匹配，1为匹配中
end

--迷宫竞速操作
CSActivityMazeRacerHandleReq = CSActivityMazeRacerHandleReq or BaseClass(BaseProtocolStruct)

function CSActivityMazeRacerHandleReq:__init()
	self.msg_type = 1207
	self.param1 = 0
end

-- op_type:
-- 	0 请求传送
-- 	1 使用符咒  p1:符咒ID
function CSActivityMazeRacerHandleReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.op_type)
    MsgAdapter.WriteShort(self.param1)
end

--迷宫竞速可用道具信息
SCActivityMazeRacerAmuletNum = SCActivityMazeRacerAmuletNum or BaseClass(BaseProtocolStruct)
function SCActivityMazeRacerAmuletNum:__init()
	self.msg_type = 1208
end

function SCActivityMazeRacerAmuletNum:Decode()
	local type_count = MsgAdapter.ReadInt()
	self.amulet_num = {}
	for i=1,type_count do
		local ptc_item_info = {}
		ptc_item_info.item_id = MsgAdapter.ReadUShort()
		ptc_item_info.item_num = MsgAdapter.ReadUShort()
		table.insert(self.amulet_num,ptc_item_info)
	end
end

local function MazeRacerRankItem()
	local rank_item = {}
	rank_item.pass_time = MsgAdapter.ReadUInt()
	rank_item.rank = MsgAdapter.ReadUShort()
	local mem_count = MsgAdapter.ReadUShort()
	rank_item.mem_list = {}
	for j=1,mem_count do
		local mem_info = {}
		mem_info.role_uid = MsgAdapter.ReadInt()
		mem_info.level = MsgAdapter.ReadUShort()
		mem_info.prof = MsgAdapter.ReadUShort()
		mem_info.avatar = MsgAdapter.ReadUShort()
		mem_info.avata_id = MsgAdapter.ReadUShort()--人物头像
		mem_info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		mem_info.guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)	
		table.insert(rank_item.mem_list,mem_info)
	end
	return rank_item
end

--迷宫竞速排行榜信息
SCActivityMazeRacerRankLists = SCActivityMazeRacerRankLists or BaseClass(BaseProtocolStruct)
function SCActivityMazeRacerRankLists:__init()
	self.msg_type = 1209
end

function SCActivityMazeRacerRankLists:Decode()
	self.maze_id = MsgAdapter.ReadUShort()
	local list_num = MsgAdapter.ReadUShort()
	self.rank_list = {}
	for i=1,list_num do
		local rank_item = MazeRacerRankItem()
		table.insert(self.rank_list,rank_item)
	end
end

SCActivityTeamFightRank = SCActivityTeamFightRank or BaseClass(BaseProtocolStruct)
function SCActivityTeamFightRank:__init()
	self.msg_type = 1210
	self.rank_list = {}
	self.count = 0
end

function SCActivityTeamFightRank:Decode()
	self.count = MsgAdapter.ReadInt()
	self.rank_list = {}
	for i=1,self.count do
		local name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		local win_rate = MsgAdapter.ReadShort()
		local score = MsgAdapter.ReadShort()
		local level = MsgAdapter.ReadInt()
		self.rank_list[i] = {name = name,win_rate = win_rate,score = score,level = level}
	end
end

SCActivityTeamFightRecord = SCActivityTeamFightRecord or BaseClass(BaseProtocolStruct)
function SCActivityTeamFightRecord:__init()
	self.msg_type = 1211
	self.record_list = {}
	self.count = 0
end

function SCActivityTeamFightRecord:Decode()
	self.record_list = {}
	self.count = MsgAdapter.ReadInt()
	for i=1, self.count do
		local name1 = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		local name2 = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		if self.record_list[i] == nil then
			self.record_list[i] = {}
		end
		self.record_list[i][1] = name1
		self.record_list[i][2] = name2
	end
end

--迷宫竞速等待室信息
SCActivityMazeRacerWaitsInfo = SCActivityMazeRacerWaitsInfo or BaseClass(BaseProtocolStruct)
function SCActivityMazeRacerWaitsInfo:__init()
	self.msg_type = 1212
end

function SCActivityMazeRacerWaitsInfo:Decode()
	self.next_begin_timestamp = MsgAdapter.ReadUInt()
	self.cur_waiting_role_num = MsgAdapter.ReadInt()
	self.min_level = MsgAdapter.ReadUShort()
	self.max_level = MsgAdapter.ReadUShort()
end

--迷宫竞速 竞赛中副本信息
SCActivityMazeRacerCopysInfo = SCActivityMazeRacerCopysInfo or BaseClass(BaseProtocolStruct)
function SCActivityMazeRacerCopysInfo:__init()
	self.msg_type = 1213
end

function SCActivityMazeRacerCopysInfo:Decode()
	self.begin_timestamp = MsgAdapter.ReadUInt()
	-- self.my_layer = MsgAdapter.ReadUShort()
	-- self.my_rank = MsgAdapter.ReadUShort()
	self.my_layer = 0
	self.my_rank = 0
	MsgAdapter.ReadUShort()
	local list_num = MsgAdapter.ReadUShort()
	self.rank_list = {}
	-- 	list_num,self.begin_timestamp,TimeManager.GetServerTime(),
	-- 	TimeManager.GetServerTime()-self.begin_timestamp)
	for i=1,list_num do
		local rank_info = {}
		rank_info.rank = i
		rank_info.leader_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		rank_info.cur_layer = MsgAdapter.ReadInt()	--0表示已通关
		rank_info.leader_uid = MsgAdapter.ReadInt()	--队长UID
		rank_info.team_index = MsgAdapter.ReadInt() --队伍index
		table.insert(self.rank_list,rank_info)
	end
end

--迷宫竞速 通关信息
SCActivityMazeRacerPassdInfo = SCActivityMazeRacerPassdInfo or BaseClass(BaseProtocolStruct)
function SCActivityMazeRacerPassdInfo:__init()
	self.msg_type = 1214
end

function SCActivityMazeRacerPassdInfo:Decode()
	self.pass_time = MsgAdapter.ReadUInt()
	self.rank_posi = MsgAdapter.ReadUShort()
	self.total_encounter = MsgAdapter.ReadUShort()
	local award_num = MsgAdapter.ReadInt()
	self.award_list = {}
	for i=1,award_num do
		local item_info = {}
		item_info.item_id = MsgAdapter.ReadUShort()
		item_info.num = MsgAdapter.ReadUShort()
		table.insert(self.award_list, Item.Create(item_info))
	end
end

SCActivityTeamFightMatchSucc = SCActivityTeamFightMatchSucc or BaseClass(BaseProtocolStruct)
function SCActivityTeamFightMatchSucc:__init()
	self.msg_type = 1215
end

function SCActivityTeamFightMatchSucc:Decode()
end

--迷宫竞速 - 受咒信息
SCActivityMazeRacerCurseInfo = SCActivityMazeRacerCurseInfo or BaseClass(BaseProtocolStruct)
function SCActivityMazeRacerCurseInfo:__init()
	self.msg_type = 1216
end

function SCActivityMazeRacerCurseInfo:Decode()
	self.obj_id = MsgAdapter.ReadUShort()
	self.cures_type = MsgAdapter.ReadUShort()
end

--迷宫竞速 - 活动排名新增
SCActivityMazeRacerActInsert = SCActivityMazeRacerActInsert or BaseClass(BaseProtocolStruct)
function SCActivityMazeRacerActInsert:__init()
	self.msg_type = 1217
end

function SCActivityMazeRacerActInsert:Decode()
	self.maze_id = MsgAdapter.ReadUShort()
	MsgAdapter.ReadUShort()
	self.rank_info = MazeRacerRankItem()
end

--=====================玩家充值信息=====================
SCChongzhiInfo = SCChongzhiInfo or BaseClass(BaseProtocolStruct)
function SCChongzhiInfo:__init()
	self.msg_type = 1218
end

function SCChongzhiInfo:Decode()
	self.recharge_num = MsgAdapter.ReadLL()             --历史总充值数 单位：角
	self.today_recharge_num = MsgAdapter.ReadLL()       --今日总充值数
	self.reward_fatch_flag = MsgAdapter.ReadInt()--充值返利奖励领取标志
	self.reset_level_num = MsgAdapter.ReadInt()  --每日重置时的等级
end

--=====================玩家充值请求操作=====================
CSChongzhiOperateReq = CSChongzhiOperateReq or BaseClass(BaseProtocolStruct)
function CSChongzhiOperateReq:__init()
	self.msg_type = 1219
end

function CSChongzhiOperateReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.op_type)
	MsgAdapter.WriteInt(self.param1)
	MsgAdapter.WriteInt(self.param2)
end

CSActivityGuildFightOperatorReq = CSActivityGuildFightOperatorReq or BaseClass(BaseProtocolStruct)
function CSActivityGuildFightOperatorReq:__init()
	self.msg_type = 1220
end
function CSActivityGuildFightOperatorReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.op_type)
end
local function GuildFightRankItem()
	return {
		rank = MsgAdapter.ReadInt(),--排名位置 0未上榜
		score = MsgAdapter.ReadInt(),--总积分
		win_times = MsgAdapter.ReadInt(),--胜利次数
		battle_times = MsgAdapter.ReadInt(),--战斗次数 服务端计算胜率会保留两位小数
		guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN),
	}
end

SCActivityGuildFightRankAllInfo = SCActivityGuildFightRankAllInfo or BaseClass(BaseProtocolStruct)
function SCActivityGuildFightRankAllInfo:__init()
	self.msg_type = 1221
end
function SCActivityGuildFightRankAllInfo:Decode()
	self.rank_all_info = {}
	self.rank_all_info.resi_role = MsgAdapter.ReadInt()
	self.rank_all_info.resi_guild = MsgAdapter.ReadInt()
	self.rank_all_info.list_num = MsgAdapter.ReadInt()
	self.rank_all_info.list_info = {}
	for i = 1, self.rank_all_info.list_num do
		self.rank_all_info.list_info[i] = GuildFightRankItem()
	end
end

SCActivityGuildFightMyGuildInfo = SCActivityGuildFightMyGuildInfo or BaseClass(BaseProtocolStruct)
function SCActivityGuildFightMyGuildInfo:__init()
	self.msg_type = 1222
end
function SCActivityGuildFightMyGuildInfo:Decode()
	-- 0 正常下发 1 结算并下发信息
	self.type = MsgAdapter.ReadInt()
	self.my_family_info = GuildFightRankItem()
end

SCActivityGuildFightParticipNum = SCActivityGuildFightParticipNum or BaseClass(BaseProtocolStruct)
function SCActivityGuildFightParticipNum:__init()
	self.msg_type = 1225
end
function SCActivityGuildFightParticipNum:Decode()
	self.guild_join_num = MsgAdapter.ReadInt()
end

SCActivityGuildAnswerCurQuestion = SCActivityGuildAnswerCurQuestion or BaseClass(BaseProtocolStruct)
function SCActivityGuildAnswerCurQuestion:__init()
	self.msg_type = 1223
end
function SCActivityGuildAnswerCurQuestion:Decode()
	self.cur_seq = MsgAdapter.ReadInt()
	self.question_id = MsgAdapter.ReadInt()
	self.next_time = MsgAdapter.ReadUInt()
end

CSActivityGuildAnswerAnswerQuestion = CSActivityGuildAnswerAnswerQuestion or BaseClass(BaseProtocolStruct)
function CSActivityGuildAnswerAnswerQuestion:__init()
	self.msg_type = 1224
end
function CSActivityGuildAnswerAnswerQuestion:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.answer)
end

SCActivityGuildAnswerResultInfo = SCActivityGuildAnswerResultInfo or BaseClass(BaseProtocolStruct)
function SCActivityGuildAnswerResultInfo:__init()
	self.msg_type = 1226
end
function SCActivityGuildAnswerResultInfo:Decode()
	self.answer_result = MsgAdapter.ReadInt() --0错1对
end

SCActivityGuildAnswerAwardSettle = SCActivityGuildAnswerAwardSettle or BaseClass(BaseProtocolStruct)
function SCActivityGuildAnswerAwardSettle:__init()
	self.msg_type = 1227
end
function SCActivityGuildAnswerAwardSettle:Decode()
	self.item_list = {}
	self.coin = MsgAdapter.ReadInt()
	self.exp = MsgAdapter.ReadInt()
	self.gongxian = MsgAdapter.ReadInt()
	table.insert(self.item_list, Item.Create({item_id = CommonItemId.Coin, num = self.coin}))
	table.insert(self.item_list, Item.Create({item_id = CommonItemId.Exp, num = self.exp}))
	table.insert(self.item_list, Item.Create({item_id = CommonItemId.GongXian, num = self.gongxian}))
	local item_num = MsgAdapter.ReadInt()
	for _ = 1, item_num do
		local item = {}
		item.item_id = MsgAdapter.ReadUShort()
		item.is_bind = MsgAdapter.ReadShort()
		item.num = MsgAdapter.ReadInt()
		table.insert(self.item_list, Item.Create(item))
	end
end

CSActivityOnlyFightOperatorReq = CSActivityOnlyFightOperatorReq or BaseClass(BaseProtocolStruct)
function CSActivityOnlyFightOperatorReq:__init()
	self.msg_type = 1228
end
function CSActivityOnlyFightOperatorReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.op_type)
end
local function OnlyFightRankItem()
	return {
		rank = MsgAdapter.ReadInt(),--0未上榜
		total_score = MsgAdapter.ReadInt(),
		win_times = MsgAdapter.ReadInt(),
		battle_times = MsgAdapter.ReadInt(),
		role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
	}
end
SCActivityOnlyFightRankAllInfo = SCActivityOnlyFightRankAllInfo or BaseClass(BaseProtocolStruct)
function SCActivityOnlyFightRankAllInfo:__init()
	self.msg_type = 1229
end
function SCActivityOnlyFightRankAllInfo:Decode()
	self.role_num = MsgAdapter.ReadInt()
	self.list_num = MsgAdapter.ReadInt()
	self.list_info = {}
	for i=1,self.list_num do
		self.list_info[i] = OnlyFightRankItem()
	end
end

SCActivityOnlyFightMyInfo = SCActivityOnlyFightMyInfo or BaseClass(BaseProtocolStruct)
function SCActivityOnlyFightMyInfo:__init()
	self.msg_type = 1230
end
function SCActivityOnlyFightMyInfo:Decode()
	self.reason_type = MsgAdapter.ReadInt()
	self.my_info = OnlyFightRankItem()
end

--活动报名
CSActivitySignUpReq = CSActivitySignUpReq or BaseClass(BaseProtocolStruct)
function CSActivitySignUpReq:__init()
	self.msg_type = 1231
end
function CSActivitySignUpReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.seq)
end

SCActivitySignUpInfo = SCActivitySignUpInfo or BaseClass(BaseProtocolStruct)
function SCActivitySignUpInfo:__init()
	self.msg_type = 1232
end
function SCActivitySignUpInfo:Decode()
	self.sign_up_flag = MsgAdapter.ReadInt()
end

SCRAActivityPartnerExchange = SCRAActivityPartnerExchange or BaseClass(BaseProtocolStruct)
function SCRAActivityPartnerExchange:__init()
	self.msg_type = 1233
end

function SCRAActivityPartnerExchange:Decode()
	self.ra_partner_exchange_times =  {}
	for i = 0,9 do 
		self.ra_partner_exchange_times[i] = MsgAdapter.ReadUShort()
	end 
end

-- 宠物兑换
SCRAActivityPetExchange = SCRAActivityPetExchange or BaseClass(BaseProtocolStruct)
function SCRAActivityPetExchange:__init()
	self.msg_type = 1234
end

function SCRAActivityPetExchange:Decode()
	local info = {}

	info.ra_pet_exchange_times =  {}		-- 活动已兑换次数
	for i = 0,19 do 
		info.ra_pet_exchange_times[i] = MsgAdapter.ReadUShort()		
	end 

	info.day_pet_exchange_times =  {}		-- 每日已兑换次数
	for i = 0,19 do 
		info.day_pet_exchange_times[i] = MsgAdapter.ReadUShort()		
	end

	info.shop_item_times = {}	-- 商店已购买次数
	for i = 0,9 do
		info.shop_item_times[i] = MsgAdapter.ReadInt()
	end

	self.info = info
end

-- 宠物兑换活动单条下发
SCRAActivityPetExchangeInfo = SCRAActivityPetExchangeInfo or BaseClass(BaseProtocolStruct)
function SCRAActivityPetExchangeInfo:__init()
	self.msg_type = 3192
end

function SCRAActivityPetExchangeInfo:Decode()
	local info = {}
	info.type = MsgAdapter.ReadShort()	-- 0 是兑换，  p1活动已兑换次数，p2每日已兑换次数   1 是商店购买， p1已购买次数
	info.seq = MsgAdapter.ReadShort()
	info.p1 = MsgAdapter.ReadInt()
	info.p2 = MsgAdapter.ReadInt()
	self.info = info
	--LOG("宠物兑换活动单条下发",info)
end


SCRATianMingDivinationInfo = SCRATianMingDivinationInfo or BaseClass(BaseProtocolStruct)
function SCRATianMingDivinationInfo:__init()
	self.msg_type = 1235
end

function SCRATianMingDivinationInfo:Decode()
	self.free_shou_times =  MsgAdapter.ReadInt()
	self.add_lots_list = {}
	for i = 1,DivinationData.ADD_LOT_ITEM_COUNT do 
		self.add_lots_list[i] = MsgAdapter.ReadChar()
	end 
	self.reserve_sh = MsgAdapter.ReadShort()
	self.today_use_times = MsgAdapter.ReadUInt() --今日抽了几次
	-- self.reward_history_item_count_list = {}
	-- for i = 1,DivinationData.MAX_REWARD_HISTORY_COUNT do
	-- 	local tab = {}
	-- 	tab.seq = MsgAdapter.ReadChar()
	-- 	tab.add_lot = MsgAdapter.ReadChar()
	-- 	tab.name = "七个字七个"
	-- 	self.reward_history_item_count_list[i] = tab
	-- end
	-- self.reward_history_list_cur_index = MsgAdapter.ReadShort()	--服务器给的下标已经是+1的状态了

end

SCTianMingDivinationActivityStartChouResult = SCTianMingDivinationActivityStartChouResult or BaseClass(BaseProtocolStruct)
function SCTianMingDivinationActivityStartChouResult:__init()
	self.msg_type = 1236
end

function SCTianMingDivinationActivityStartChouResult:Decode()
	self.reward_index = MsgAdapter.ReadShort() 	--获奖物品的下标
	MsgAdapter.ReadShort()
end

SCTianMingRewardPoolInfo = SCTianMingRewardPoolInfo or BaseClass(BaseProtocolStruct)
function SCTianMingRewardPoolInfo:__init()
	self.msg_type = 1237
end

function SCTianMingRewardPoolInfo:Decode()
	-- self.reward_index = MsgAdapter.ReadShort() + 1 	--获奖物品的下标
	-- MsgAdapter.ReadShort()
	local pool_num = MsgAdapter.ReadInt()
	self.pool_info = {}
	for i = 1,pool_num do
		local tab = {}
		tab.uid = MsgAdapter.ReadInt()
		tab.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		tab.rate_item_id = MsgAdapter.ReadInt()
		tab.item_nun = MsgAdapter.ReadInt()
		self.pool_info[i] = tab
	end

end

--新服竞技历史排名奖励信息
SCSportsHistroyInfo=SCSportsHistroyInfo or BaseClass(BaseProtocolStruct)
function SCSportsHistroyInfo:__init()
	self.msg_type=1239
end
function SCSportsHistroyInfo:Decode()
	self.ra_sports_histroy_top=MsgAdapter.ReadShort()   ---\\-1是未上榜，排名
	self.ra_sports_histroy_reward=bit:d2b(MsgAdapter.ReadUShort())  --//二进制，是否领奖
end

SCActivityMazeRacerTransportPos = SCActivityMazeRacerTransportPos or BaseClass(BaseProtocolStruct)
function SCActivityMazeRacerTransportPos:__init()
	self.msg_type = 1240
end
function SCActivityMazeRacerTransportPos:Decode()
	self.time = MsgAdapter.ReadUInt()
	self.x = MsgAdapter.ReadInt()
	self.y = MsgAdapter.ReadInt()
	self.start_time = MsgAdapter.ReadUInt()
	self.layer = MsgAdapter.ReadInt()
end

--活动入场信息下发
SCActivityEnterInfo = SCActivityEnterInfo or BaseClass(BaseProtocolStruct)
function SCActivityEnterInfo:__init()
	self.msg_type = 1241
end

function SCActivityEnterInfo:Decode()
	local info = {}
	info.act_type = MsgAdapter.ReadShort()
	info.is_enter_flag = MsgAdapter.ReadShort()
	info.enter_end_timestamp = MsgAdapter.ReadUInt()
	self.enter_info = info
end
SCActivityGuildFightSpecialInfo = SCActivityGuildFightSpecialInfo or BaseClass(BaseProtocolStruct)   -- 1242 家族大乱斗特殊信息下发
function SCActivityGuildFightSpecialInfo:__init()
	self.msg_type = 1242
end
function SCActivityGuildFightSpecialInfo:Decode()
	self.resi_role = MsgAdapter.ReadInt()
	self.resi_guild = MsgAdapter.ReadInt()
end
SCActivityOnlyFightSpecialInfo = SCActivityOnlyFightSpecialInfo or BaseClass(BaseProtocolStruct)   -- 1243  以一敌百特殊信息下
function SCActivityOnlyFightSpecialInfo:__init()
	self.msg_type = 1243
end
function SCActivityOnlyFightSpecialInfo:Decode()
	self.resi_role = MsgAdapter.ReadInt()
	-- self.resi_family = MsgAdapter.ReadInt()
end

-- 随机活动特殊货币增加
SCRAMoneyAddInfo = SCRAMoneyAddInfo or BaseClass(BaseProtocolStruct)
function SCRAMoneyAddInfo:__init()
	self.msg_type = 1244
end

function SCRAMoneyAddInfo:Decode()
	self.rand_activity = MsgAdapter.ReadInt()
	self.add_money = MsgAdapter.ReadInt()
end

-- 赠寻宝钥匙活动
SCRAGiveSecretKeyInfo = SCRAGiveSecretKeyInfo or BaseClass(BaseProtocolStruct)
function SCRAGiveSecretKeyInfo:__init()
	self.msg_type = 1245
end
function SCRAGiveSecretKeyInfo:Decode()
	self.info = {}
	self.info.reward_flag = bit:d2b(MsgAdapter.ReadInt())
	self.info.first_pass_flag = MsgAdapter.IntFlag(5)
	-- for i = 0, 4 do
	-- 	self.info.first_pass_flag[i] = MsgAdapter.ReadUInt()
	-- end
end

-- 累计充值
SCRACumulativeChargeInfo = SCRACumulativeChargeInfo or BaseClass(BaseProtocolStruct)
function SCRACumulativeChargeInfo:__init()
	self.msg_type = 1246
end

function SCRACumulativeChargeInfo:Decode()
	self.info = {}
	self.info.is_get_reward = MsgAdapter.ReadInt()
	self.info.now_recharge = MsgAdapter.ReadLL()
	self.info.is_received_free = MsgAdapter.ReadInt()
end

--限时活动奖励领取请求
CSActivityAdvanceFetchReq = CSActivityAdvanceFetchReq or BaseClass(BaseProtocolStruct)
function CSActivityAdvanceFetchReq:__init()
	self.msg_type = 1247
end

function CSActivityAdvanceFetchReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteShort(self.activity_type)
	MsgAdapter.WriteShort(self.op_type)
	MsgAdapter.WriteInt(self.param1)
end

SCActivityAdvanceFetchInfo = SCActivityAdvanceFetchInfo or BaseClass(BaseProtocolStruct)
function SCActivityAdvanceFetchInfo:__init()
	self.msg_type = 1248
end

function SCActivityAdvanceFetchInfo:Decode()
	self.activity_type = MsgAdapter.ReadShort()
	self.level = MsgAdapter.ReadShort()
	MsgAdapter.ReadShort()
	self.particip_award_flag = MsgAdapter.ReadChar()
	local flag = MsgAdapter.ReadChar()
	self.survival_award_flag = bit:d8b(flag)
end

--热更活动配置重读通知（空协议）
SCActivityConfigReloadNotice = SCActivityConfigReloadNotice or BaseClass(BaseProtocolStruct)
function SCActivityConfigReloadNotice:__init()
	self.msg_type = 1250
end

--服务端通知客户端需要检查版本，版本号不一致的话弹出重启窗
SCNoticeClientRestart = SCNoticeClientRestart or BaseClass(BaseProtocolStruct)
function SCNoticeClientRestart:__init()
	self.msg_type = 1264
end

function SCNoticeClientRestart:Decode()
	self.version_msg = MsgAdapter.ReadStrN(64)
end

--直购效验（接收）
SCCmdBuyItemCheckRet = SCCmdBuyItemCheckRet or BaseClass(BaseProtocolStruct)
function SCCmdBuyItemCheckRet:__init()
	self.msg_type = 1251
end

function SCCmdBuyItemCheckRet:Decode()
	--服务端会限制1分钟等待充值完成，上个订单如果没取消1分钟内不会下发第二次该协议
	--如果unique_id 发的是0 就别充值
	self.buy_type = MsgAdapter.ReadUInt()
	self.unique_id_1 = MsgAdapter.ReadUInt()
	self.unique_id_2 = MsgAdapter.ReadUInt()
	self.cfg_ver = MsgAdapter.ReadInt()
	if DEBUG_MODE then
		LOG("直购效验（接收）", self)
	end
end

--直购效验，适用于没有活动id的活动（发送）
CSCmdBuyItemCheckReq = CSCmdBuyItemCheckReq or BaseClass(BaseProtocolStruct)
function CSCmdBuyItemCheckReq:__init()
	self.msg_type = 1252
end

function CSCmdBuyItemCheckReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.buy_type)
	MsgAdapter.WriteInt(self.param1)
	MsgAdapter.WriteInt(self.param2)
	MsgAdapter.WriteInt(self.cfg_ver)
end

--迷宫竞速角色信息
SCActivityMazeRoleInfo = SCActivityMazeRoleInfo or BaseClass(BaseProtocolStruct)
function SCActivityMazeRoleInfo:__init()
	self.msg_type = 1253
end

function SCActivityMazeRoleInfo:Decode()
	self.pass_count = MsgAdapter.ReadInt() 		-- 迷宫竞速通关奖励次数
end

-- 夏日迷宫整个迷宫信息下发
SCMazeSummerMapInfo = SCMazeSummerMapInfo or BaseClass(BaseProtocolStruct)
function SCMazeSummerMapInfo:__init()
	self.msg_type = 1254
end

function SCMazeSummerMapInfo:Decode()
	self.role_x = MsgAdapter.ReadInt()
	self.role_y = MsgAdapter.ReadInt()
	self.map_infos = {}
	-- 服务端发的是[3, 10]的数组
	for i = 0, 2 do
		for j = 0, 9 do
			self.map_infos[j * 3 + i] = SummerMazeSingleMapInfo()
		end
	end
end

-- 夏日迷宫单个迷宫格子信息下发
SCMazeSummerSingleMapInfo = SCMazeSummerSingleMapInfo or BaseClass(BaseProtocolStruct)
function SCMazeSummerSingleMapInfo:__init()
	self.msg_type = 1255
end

function SCMazeSummerSingleMapInfo:Decode()
	self.change_x = MsgAdapter.ReadInt()
	self.change_y = MsgAdapter.ReadInt()
	self.map_info = SummerMazeSingleMapInfo()
end

-- 夏日迷宫商店下发
SCMazeSummerShopInfo = SCMazeSummerShopInfo or BaseClass(BaseProtocolStruct)
function SCMazeSummerShopInfo:__init()
	self.msg_type = 1256
end

function SCMazeSummerShopInfo:Decode()
	self.shop_num = MsgAdapter.ReadInt()
	self.shop_info = {}
	for i = 1, self.shop_num do
		self.shop_info[i] = {}
		self.shop_info[i].shop_id = MsgAdapter.ReadChar()
		self.shop_info[i].seq = MsgAdapter.ReadChar()
		self.shop_info[i].buy_times = MsgAdapter.ReadShort()
	end
end

-- 夏日迷宫答题下发
SCMazeSummerQuestionInfo = SCMazeSummerQuestionInfo or BaseClass(BaseProtocolStruct)
function SCMazeSummerQuestionInfo:__init()
	self.msg_type = 1257
end

function SCMazeSummerQuestionInfo:Decode()
	self.question_id = MsgAdapter.ReadInt()
end

-- 夏日迷宫任务下发
SCMazeSummerTaskInfo = SCMazeSummerTaskInfo or BaseClass(BaseProtocolStruct)
function SCMazeSummerTaskInfo:__init()
	self.msg_type = 1258
end

function SCMazeSummerTaskInfo:Decode()
	self.type_info = {}
	-- 反正0不要的
	for i = 0, 9 do
		self.type_info[i] = MsgAdapter.ReadInt()
	end
	self.task_info = {}
	for i = 1, 10 do
		self.task_info[i] = {}
		self.task_info[i].is_get_reward = MsgAdapter.ReadChar()
		MsgAdapter.ReadChar()
		self.task_info[i].task_id = MsgAdapter.ReadUShort()
	end
end

-- 夏日迷宫进行状态下发
SCMazeSummerStatusInfo = SCMazeSummerStatusInfo or BaseClass(BaseProtocolStruct)
function SCMazeSummerStatusInfo:__init()
	self.msg_type = 1259
end

function SCMazeSummerStatusInfo:Decode()
	self.is_playing = MsgAdapter.ReadInt()
	self.play_times = MsgAdapter.ReadInt()
end

-- 夏日迷宫玩法结算下发
SCMazeSummerPlayFinishInfo = SCMazeSummerPlayFinishInfo or BaseClass(BaseProtocolStruct)
function SCMazeSummerPlayFinishInfo:__init()
	self.msg_type = 1260
end

function SCMazeSummerPlayFinishInfo:Decode()
	self.is_pass = MsgAdapter.ReadInt()
end

-- 夏日迷宫位置改变下发
SCMazeSummerMyPosInfo = SCMazeSummerMyPosInfo or BaseClass(BaseProtocolStruct)
function SCMazeSummerMyPosInfo:__init()
	self.msg_type = 1261
end

function SCMazeSummerMyPosInfo:Decode()
	self.role_x = MsgAdapter.ReadInt()
	self.role_y = MsgAdapter.ReadInt()
end


--琼楼玉宴信息
SCBanquetInfo = SCBanquetInfo or BaseClass(BaseProtocolStruct)
function SCBanquetInfo:__init()
	self.msg_type = 1262
end

function SCBanquetInfo:Decode()
	self.info = {}
	self.info.progress_num = MsgAdapter.ReadInt()		-- 筹备进度
	self.info.status = MsgAdapter.ReadInt()				-- 当前状态
	self.info.next_status_time = MsgAdapter.ReadUInt()	-- 状态结束时间点
	self.info.banquet_type = MsgAdapter.ReadInt() 		-- 宴会类型
end

--琼楼玉宴奖励信息
SCBanquetRewardInfo = SCBanquetRewardInfo or BaseClass(BaseProtocolStruct)
function SCBanquetRewardInfo:__init()
	self.msg_type = 1263
end

function SCBanquetRewardInfo:Decode()
	self.info = {}
	self.info.box_reward_flags = MsgAdapter.IntFlag()    -- 领取宝箱二进制
	self.info.eat_food_count = MsgAdapter.ReadInt()	     -- 吃席次数
	self.info.next_eat_tamp = MsgAdapter.ReadInt()	     -- 下次吃席时间（0也是可以吃席
end

-- 夏日单个迷宫格子信息
local function SummerMazeSingleMapInfo()
	local map_info = {}
	map_info.map_type = MsgAdapter.ReadShort()
	map_info.map_lock = MsgAdapter.ReadShort()
	map_info.map_status = MsgAdapter.ReadShort()
	map_info.param1 = MsgAdapter.ReadUShort()
	map_info.param2 = MsgAdapter.ReadUShort()
	map_info.param3 = MsgAdapter.ReadUShort()
	return map_info
end

--限时活动 丝绸之路
CSActivitySilkRoadsReq = CSActivitySilkRoadsReq or BaseClass(BaseProtocolStruct)
function CSActivitySilkRoadsReq:__init()
	self.msg_type = 1270
end

function CSActivitySilkRoadsReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.op_type)
	MsgAdapter.WriteInt(self.param1)
	MsgAdapter.WriteInt(self.param2)
	-- LogError("CS请求->   op_type->" ..  self.op_type .. "   p1->" .. self.param1 .. "   p2->" .. self.param2 )
end

--丝绸之路简易信息下发
SCActivitySilkRoadSimpleInfo = SCActivitySilkRoadSimpleInfo or BaseClass(BaseProtocolStruct)
function SCActivitySilkRoadSimpleInfo:__init()
	self.msg_type = 1271
end

function SCActivitySilkRoadSimpleInfo:Decode()
	self.simple_type = MsgAdapter.ReadInt()
	self.param1 = MsgAdapter.ReadInt()
	self.param2 = MsgAdapter.ReadInt()
	-- LogError("SC简易信息下发->", self)
end

SCActivitySilkRoadsRoleAllInfo = SCActivitySilkRoadsRoleAllInfo or BaseClass(BaseProtocolStruct)
function SCActivitySilkRoadsRoleAllInfo:__init()
	self.msg_type = 1272
end

function SCActivitySilkRoadsRoleAllInfo:Decode()
	--用序列化动态写入不需要字节对齐
	self.cumulation_coin_count = MsgAdapter.ReadInt()		--本次活动累计的货币数量（计入时刻：完成次数）
	self.cumulation_trading_count = MsgAdapter.ReadInt()	--本次活动累计的贸易次数
	self.finish_num = MsgAdapter.ReadInt()					--本次活动完成次数

	self.line_group_id = MsgAdapter.ReadShort()				--玩家本次选择的路线图id
	self.buy_warehouse_count = MsgAdapter.ReadShort()		--仓库扩充此时
	self.buy_supply_count = MsgAdapter.ReadShort()			--补给购买次数
	self.have_coin_count = MsgAdapter.ReadInt()				--当前拥有货币
	self.have_supply_count = MsgAdapter.ReadInt()			--当前拥有补给

	self.init_skill = MsgAdapter.ReadShort()				--初始技能
	self.cur_city_index = MsgAdapter.ReadShort()			--当前所在的城镇(index为下列数组)
	self.trading_num = MsgAdapter.ReadShort()				--贸易次数

	self.cur_max_grid_count = MsgAdapter.ReadShort()		--当前所能使用的数量
	self.cur_has_commodity_count = MsgAdapter.ReadShort()	--当前已有的货物数量
	self.news_pool_count = MsgAdapter.ReadInt()				--当前剩余消息条数

	local grid_count = MsgAdapter.ReadShort()
	self.grid_list = {}
	for i = 1, grid_count do
		local data = {}
		data.grid_index = MsgAdapter.ReadShort()
		data.commodity_seq = MsgAdapter.ReadShort()
		data.commodity_count = MsgAdapter.ReadShort()
		local commodity_cost = MsgAdapter.ReadFloat()		--货物成本
		data.commodity_cost = math.floor(commodity_cost + 0.5)	--取整货物成本
		self.grid_list[data.grid_index] = data
	end
	
	--角色贸易日志
	self.trading_log_count = MsgAdapter.ReadShort()		--角色贸易日志数量
	self.trading_log_list = {}
	for i = 1, self.trading_log_count do
		local data = {}
		data.log_type = MsgAdapter.ReadShort()		--类型
		data.freight_id = MsgAdapter.ReadShort()
		data.from_city_map_seq = MsgAdapter.ReadShort() --消息来源城镇地图序号
		data.target_city_map_seq = MsgAdapter.ReadShort()	--目标城镇地图序号
		data.commodity_seq = MsgAdapter.ReadShort()	--货物序号
		data.price = MsgAdapter.ReadInt()			--价格
		self.trading_log_list[i] = data
	end

	--角色城镇中的信息
	self.city_count = MsgAdapter.ReadShort()			--该路线城镇数量
	self.city_list = {}
	self.item_price_list = {}
	for i = 0, self.city_count - 1 do
		local data = {}
		data.city_map_seq = MsgAdapter.ReadShort()		--城镇对应地图序号
		data.buy_news_count = MsgAdapter.ReadShort()	--购买消息次数
		data.commodity_count = MsgAdapter.ReadShort()	--特产数量
		data.commodity_list = {}
		self.item_price_list[data.city_map_seq] = {}
		for k = 0, data.commodity_count - 1 do
			local item = {}
			item.commodity_seq = MsgAdapter.ReadShort()	--货物序号
			item.price = MsgAdapter.ReadUShort()		--货物价格
			data.commodity_list[k] = item
			self.item_price_list[data.city_map_seq][item.commodity_seq] = item
		end
		self.city_list[i] = data
	end
	-- LogError("SC总信息->", Serialize(self))
end

--丝绸之路资产下发
SCActivitySilkRoadsAssetsInfo = SCActivitySilkRoadsAssetsInfo or BaseClass(BaseProtocolStruct)
function SCActivitySilkRoadsAssetsInfo:__init()
	self.msg_type = 1273
end

function SCActivitySilkRoadsAssetsInfo:Decode()
	self.have_coin_count = MsgAdapter.ReadInt()		--当前拥有货币
	self.have_supply_count = MsgAdapter.ReadInt()	--当前拥有补给
	self.cur_max_grid_count = MsgAdapter.ReadShort()--当前使用的数量(已开通的格子数)
	MsgAdapter.ReadShort()
	-- LogError("SC资产下发->", self)
end

--丝绸之路仓库变化
SCActivitySilkRoadsWarehouseChange = SCActivitySilkRoadsWarehouseChange or BaseClass(BaseProtocolStruct)
function SCActivitySilkRoadsWarehouseChange:__init()
	self.msg_type = 1274
end

function SCActivitySilkRoadsWarehouseChange:Decode()
	self.grid_index = MsgAdapter.ReadShort()	--仓库哪个格子
	self.notice_type = MsgAdapter.ReadShort()	--物品来源
	self.commodity_seq = MsgAdapter.ReadShort()	--货物序号
	self.commodity_count = MsgAdapter.ReadShort()	--货物数量
	local commodity_cost = MsgAdapter.ReadFloat()	--货物成本
	self.commodity_cost = math.floor(commodity_cost + 0.5)	--取整货物成本
	-- LogError("SC仓库变化->", self)
end

--丝绸之路排行榜信息下发
SCActivitySilkRoadsAllRankInfo = SCActivitySilkRoadsAllRankInfo or BaseClass(BaseProtocolStruct)
function SCActivitySilkRoadsAllRankInfo:__init()
	self.msg_type = 1275
end

function SCActivitySilkRoadsAllRankInfo:Decode()
	self.rank_count = MsgAdapter.ReadInt()
	self.rank_list = {}
	for i = 1, self.rank_count do
		local info = {}
		info.index = i
		info.role_id = MsgAdapter.ReadInt()
		info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		info.avatar_type = MsgAdapter.ReadShort()	--虚拟人物类型
		info.avatar_id = MsgAdapter.ReadShort()	--头像ID
		info.cumulation_coin_count = MsgAdapter.ReadInt()	--本次活动累积的货币数量
		info.cumulation_trading_count = MsgAdapter.ReadInt()--本次活动累积的贸易次数
		table.insert(self.rank_list, info)
	end
	-- LogError("SC排行榜信息->", self)
end

local function ActivitySilkRoadsRoleTradingLogData()
	local t = {}
	t.log_type = MsgAdapter.ReadShort()		--类型
	t.freight_id = MsgAdapter.ReadShort()
	t.from_city_map_seq = MsgAdapter.ReadShort() --消息来源城镇地图序号
	t.target_city_map_seq = MsgAdapter.ReadShort()	--目标城镇地图序号
	t.commodity_seq = MsgAdapter.ReadShort()	--货物序号
	MsgAdapter.ReadShort()
	t.price = MsgAdapter.ReadInt()			--价格
	return t
end

--日志信息
SCActivitySilkRoadsAllTradingLogInfo = SCActivitySilkRoadsAllTradingLogInfo or BaseClass(BaseProtocolStruct)
function SCActivitySilkRoadsAllTradingLogInfo:__init()
	self.msg_type = 1276
end

function SCActivitySilkRoadsAllTradingLogInfo:Decode()
	MsgAdapter.ReadShort()
	self.trading_log_count = MsgAdapter.ReadShort()
	self.trading_log_list = {}
	for i = 1, self.trading_log_count do
		self.trading_log_list[i] = ActivitySilkRoadsRoleTradingLogData()
	end
	-- LogError("SC日志信息->", self)
end

--单个日志
SCActivitySilkRoadsSingleTradingLogNotice = SCActivitySilkRoadsSingleTradingLogNotice or BaseClass(BaseProtocolStruct)
function SCActivitySilkRoadsSingleTradingLogNotice:__init()
	self.msg_type = 1277
end

function SCActivitySilkRoadsSingleTradingLogNotice:Decode()
	self.trading_log_index = MsgAdapter.ReadInt()
	self.trading_log_info = ActivitySilkRoadsRoleTradingLogData()
	-- LogError("SC新增日志->", self)
end
