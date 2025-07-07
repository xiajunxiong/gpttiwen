SCTitleAllInfo = SCTitleAllInfo or BaseClass(BaseProtocolStruct)
function SCTitleAllInfo:__init()
	self.msg_type = 1900
end

function SCTitleAllInfo:Decode()
	self.title_info = {}
	for i = 0, MsgAdapter.ReadInt() - 1 do
		local title_id = MsgAdapter.ReadShort()
		MsgAdapter.ReadShort()
		self.title_info[title_id] = MsgAdapter.ReadInt()
	end
end

SCTitleSingleInfo = SCTitleSingleInfo or BaseClass(BaseProtocolStruct)
function SCTitleSingleInfo:__init()
	self.msg_type = 1901
end

function SCTitleSingleInfo:Decode()
	MsgAdapter.ReadInt()
	MsgAdapter.ReadInt()
	self.title_id = MsgAdapter.ReadShort()
	MsgAdapter.ReadShort()
	self.stamp_time = MsgAdapter.ReadInt()
end

SCTitleOtherInfo = SCTitleOtherInfo or BaseClass(BaseProtocolStruct)
function SCTitleOtherInfo:__init()
	self.msg_type = 1902
end

function SCTitleOtherInfo:Decode()
	self.title_id = MsgAdapter.ReadInt()--wear_index改成title_id
end

CSTitleOp = CSTitleOp or BaseClass(BaseProtocolStruct)
function CSTitleOp:__init()
	self.msg_type = 1903
	self.type = 0
	self.param = 0
end
function CSTitleOp:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.type)
	MsgAdapter.WriteInt(self.param)
end

SCTitleChange = SCTitleChange or BaseClass(BaseProtocolStruct)
function SCTitleChange:__init()
	self.msg_type = 1904
end

function SCTitleChange:Decode()
	self.title_id = MsgAdapter.ReadUShort()
	self.obj_id = MsgAdapter.ReadUShort()
	self.facescroe_id = MsgAdapter.ReadInt() --颜值系统|头衔资源id
end

SCTitleNotice = SCTitleNotice or BaseClass(BaseProtocolStruct)
function SCTitleNotice:__init()
	self.msg_type = 1905
end

function SCTitleNotice:Decode()
	self.notice = MsgAdapter.ReadShort() --通知 0 获得 1 失去
	MsgAdapter.ReadShort() 
	self.title_id = MsgAdapter.ReadInt()
end

SCTrainAllInfo = SCTrainAllInfo or BaseClass(BaseProtocolStruct)
function SCTrainAllInfo:__init()
	self.msg_type = 1920
end
function SCTrainAllInfo:Decode()
	local count = MsgAdapter.ReadInt()
	local pet_train = {}
	self.trainData = {}
	pet_train.train_type = MsgAdapter.ReadShort()
	pet_train.level = MsgAdapter.ReadUShort()
	pet_train.exp = MsgAdapter.ReadInt()
	
	self.trainData[pet_train.train_type] = pet_train
	for i = 1,count do
		local data = {}
		data.train_type = MsgAdapter.ReadShort()
		data.level = MsgAdapter.ReadUShort()
		data.exp = MsgAdapter.ReadInt()
		-- table.insert(self.trainData, data)
		self.trainData[data.train_type] = data
	end
end

CSTrainReq = CSTrainReq or BaseClass(BaseProtocolStruct)
function CSTrainReq:__init()
	self.msg_type = 1921
	self.req_type = 0
	self.param1 = 0
	self.param2 = 0
end
function CSTrainReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.req_type)
	MsgAdapter.WriteInt(self.param1)
	-- MsgAdapter.WriteShort(self.req_type)
	-- MsgAdapter.WriteShort(self.param1)
	MsgAdapter.WriteInt(self.param2)
end

-- 修炼 宠物属性技能
CSRolePetTrainSkillReq = CSRolePetTrainSkillReq or BaseClass(BaseProtocolStruct)
function CSRolePetTrainSkillReq:__init()
	self.msg_type = 1922
	self.train_num = 0
end

function CSRolePetTrainSkillReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.train_num)
end

SCTrainRedDotFlagInfo = SCTrainRedDotFlagInfo or BaseClass(BaseProtocolStruct)
function SCTrainRedDotFlagInfo:__init()
	self.msg_type = 1924
end

function SCTrainRedDotFlagInfo:Decode()
	self.pet_train_red_dot_flag = MsgAdapter.ReadShort()
	local flag = MsgAdapter.ReadUShort()
	self.train_red_dot_flag = DataHelper.ReverseList(bit:d16b(flag))
end

--家族任务
SCGuildWeekTaskAllInfo = SCGuildWeekTaskAllInfo or  BaseClass(BaseProtocolStruct)
function SCGuildWeekTaskAllInfo:__init()
	self.msg_type = 1930
end
function SCGuildWeekTaskAllInfo:Decode()
	self.finish_times = MsgAdapter.ReadInt()
	self.task_id = MsgAdapter.ReadInt()
	self.task_prog = MsgAdapter.ReadInt()
end
local GWTRankItem = function ()
	return {
		name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
		avatar_type = MsgAdapter.ReadShort(),
		avatar_id = MsgAdapter.ReadShort(),
		prof = MsgAdapter.ReadInt(),
		cont = MsgAdapter.ReadInt(),
	}
end
SCGuildWeekRankList = SCGuildWeekRankList or BaseClass(BaseProtocolStruct)
function SCGuildWeekRankList:__init()
	self.msg_type = 1931
end
function SCGuildWeekRankList:Decode()
	self.count =  MsgAdapter.ReadInt()
	self.rank_list = {}
	for i = 1, self.count do
		self.rank_list[i] = GWTRankItem()
		self.rank_list[i].index = i
	end
end

--修炼技能已提升信息下发
SCTrainInfoPromoted = SCTrainInfoPromoted or BaseClass(BaseProtocolStruct)
function SCTrainInfoPromoted:__init()
	self.msg_type = 1923
end
function SCTrainInfoPromoted:Decode()
	self.physical_dec_damage = MsgAdapter.ReadInt()
	self.magic_dec_damage = MsgAdapter.ReadInt()
end

SCCollectionAllInfo = SCCollectionAllInfo or BaseClass(BaseProtocolStruct)
function SCCollectionAllInfo:__init()
	self.msg_type = 1950
end
function SCCollectionAllInfo:Decode()
	self.active_info = {}
	for i = 0, 1999 do
		self.active_info[i]=MsgAdapter.ReadUChar()
	end
end

SCCollectionSingleInfo = SCCollectionSingleInfo or BaseClass(BaseProtocolStruct)
function SCCollectionSingleInfo:__init()
	self.msg_type = 1951
end

function SCCollectionSingleInfo:Decode()
	self.seq = MsgAdapter.ReadShort()
	self.is_active = MsgAdapter.ReadShort()
end

CSCollectionOp = CSCollectionOp or BaseClass(BaseProtocolStruct)
function CSCollectionOp:__init()
	self.msg_type = 1952
end

function CSCollectionOp:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.op_type)
	MsgAdapter.WriteInt(self.param1)
end

--收录系统
SCCollectionIncludedInfo = SCCollectionIncludedInfo or BaseClass(BaseProtocolStruct)
function SCCollectionIncludedInfo:__init()
	self.msg_type = 1953
end

function SCCollectionIncludedInfo:Decode()
	self.can_active = MsgAdapter.IntFlag(COLLECTION_INCLUDED_MAX)
	self.has_active = MsgAdapter.IntFlag(COLLECTION_INCLUDED_MAX)
end

SCCollectionIncludedSingle = SCCollectionIncludedSingle or BaseClass(BaseProtocolStruct)
function SCCollectionIncludedSingle:__init()
	self.msg_type = 1954
end

function SCCollectionIncludedSingle:Decode()
	self.seq = MsgAdapter.ReadUShort()
	self.can_active = MsgAdapter.ReadUChar()
	self.has_active = MsgAdapter.ReadUChar()
end

-- 珍品图鉴卡信息下发
SCCollectionAtlasCardInfo = SCCollectionAtlasCardInfo or BaseClass(BaseProtocolStruct)
function SCCollectionAtlasCardInfo:__init()
	self.msg_type = 1955
end

function SCCollectionAtlasCardInfo:Decode()
	self.info = {}
	-- 服务端从0开始记录，但表的seq是从1开始的
	for i = 0, MAX_ATLAS_CARD_NUM - 1 do
		self.info[i] = MsgAdapter.ReadUShort()
	end
end

-- 珍品图鉴卡信息单条下发
SCCollectionAtlasCardSignleInfo = SCCollectionAtlasCardSignleInfo or BaseClass(BaseProtocolStruct)
function SCCollectionAtlasCardSignleInfo:__init()
	self.msg_type = 1956
end

function SCCollectionAtlasCardSignleInfo:Decode()
	self.seq = MsgAdapter.ReadUShort()
	self.level = MsgAdapter.ReadUShort()
end

SCAnswerAllInfo = SCAnswerAllInfo or BaseClass(BaseProtocolStruct)
function SCAnswerAllInfo:__init()
	self.msg_type = 1960
end
function SCAnswerAllInfo:Decode()
	self.question_id = MsgAdapter.ReadInt()
	self.today_buy_exp_count = MsgAdapter.ReadChar()
	self.yestoday_buy_exp_count = MsgAdapter.ReadChar()
	self.answer_num = MsgAdapter.ReadUShort()
end


--op_type 0:获取信息
--op_type 1:回答题目 pram1-答案id[1,4]
--op_type 2:购买经验 param1-0=购买今天的经验,1=购买昨天的经验
CSAnswerOp = CSAnswerOp or BaseClass(BaseProtocolStruct)
function CSAnswerOp:__init()
	self.msg_type = 1961
	self.op_type = 0
	self.param1 = 0
end

function CSAnswerOp:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.op_type)
	MsgAdapter.WriteInt(self.param1)
end

-- 答题总奖励数
SCAnswerOverInfo = SCAnswerOverInfo or BaseClass(BaseProtocolStruct)
function SCAnswerOverInfo:__init()
	self.msg_type = 1962
end

function SCAnswerOverInfo:Decode()
	self.add_coin = MsgAdapter.ReadLL()
	self.add_exp = MsgAdapter.ReadLL()
end

-- 在线奖励信息
SCOnlineRewardInfo = SCOnlineRewardInfo or BaseClass(BaseProtocolStruct)
function SCOnlineRewardInfo:__init()
	self.msg_type = 1970
end

function SCOnlineRewardInfo:Decode()
	self.today_online_time = MsgAdapter.ReadUInt()
	self.reward_fetch_flag = MsgAdapter.ReadInt()
end

-- 签到奖励信息
SCSignRewardInfo = SCSignRewardInfo or BaseClass(BaseProtocolStruct)
function SCSignRewardInfo:__init()
	self.msg_type = 1971
end

function SCSignRewardInfo:Decode()
	self.sign_count = MsgAdapter.ReadInt()
	self.reward_fetch_flag = MsgAdapter.ReadInt()
	self.month_zero_open_day = MsgAdapter.ReadInt()
end

-- 等级奖励信息
SCLevelRewardInfo = SCLevelRewardInfo or BaseClass(BaseProtocolStruct)
function SCLevelRewardInfo:__init()
	self.msg_type = 1972
end

function SCLevelRewardInfo:Decode()
	self.reward_fetch_flag = MsgAdapter.ReadInt()
end

-- 领取奖励
CSFetchRewardReq = CSFetchRewardReq or BaseClass(BaseProtocolStruct)
function CSFetchRewardReq:__init()
	self.msg_type = 1973
end

function CSFetchRewardReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.oper_type)
	MsgAdapter.WriteInt(self.seq)
end

-- 找回奖励
CSFindRewardReq = CSFindRewardReq or BaseClass(BaseProtocolStruct)
function CSFindRewardReq:__init()
	self.msg_type = 1974
end

function CSFindRewardReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.type)
	MsgAdapter.WriteInt(self.consume_type)
	MsgAdapter.WriteInt(self.count)
end

-- 找回奖励信息
SCFindRewardInfo = SCFindRewardInfo or BaseClass(BaseProtocolStruct)
function SCFindRewardInfo:__init()
	self.msg_type = 1975
end

function SCFindRewardInfo:Decode()
	self.count_list = {}
	self.today_find_count = {}
	self.find_reward_flag = MsgAdapter.ReadUInt()
	for i = 0, RewardConfig.DAY_ACTIVITY_TYPE_MAX - 1 do
		self.count_list[i] = MsgAdapter.ReadInt()		-- 能找回的次数
	end
	for i = 0, RewardConfig.DAY_ACTIVITY_TYPE_MAX - 1 do
		self.today_find_count[i] = MsgAdapter.ReadInt()	-- 已找回的次数
	end
end

-- 幻化全部信息
SCSurfaceAllInfo = SCSurfaceAllInfo or BaseClass(BaseProtocolStruct)
function SCSurfaceAllInfo:__init()
	self.msg_type = 1980
end

function SCSurfaceAllInfo:Decode()
	self.wear_surface = {}
	for i = 0, 5 do
		self.wear_surface[i] = MsgAdapter.ReadShort()
	end

	self.surface_list = {}
	self.surface_count = MsgAdapter.ReadInt()
	for i = 1, self.surface_count do
		local data = {}
		data.id = MsgAdapter.ReadInt()
		data.level = MsgAdapter.ReadInt()
		data.end_time = MsgAdapter.ReadInt()
		table.insert(self.surface_list ,data)
	end
end

-- 幻化单个信息
SCSurfaceSingleInfo = SCSurfaceSingleInfo or BaseClass(BaseProtocolStruct)
function SCSurfaceSingleInfo:__init()
	self.msg_type = 1982
end

function SCSurfaceSingleInfo:Decode()
	self.reason = MsgAdapter.ReadInt()	-- 0默认不提示 1激活时装道具提示 2到期提示
	self.surface_info = {
		id = MsgAdapter.ReadInt(),
		level = MsgAdapter.ReadInt(),
		end_time = MsgAdapter.ReadInt(),
	}
end

-- 幻化穿戴信息
SCSurfaceWearInfo = SCSurfaceWearInfo or BaseClass(BaseProtocolStruct)
function SCSurfaceWearInfo:__init()
	self.msg_type = 1983
end

function SCSurfaceWearInfo:Decode()
	self.wear_surface = {}
	for i = 0, 5 do
		self.wear_surface[i] = MsgAdapter.ReadShort()
	end
end

-- 幻化请求
CSSurfaceOp = CSSurfaceOp or BaseClass(BaseProtocolStruct)
function CSSurfaceOp:__init()
	self.msg_type = 1981
end

function CSSurfaceOp:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.oper_type)
	MsgAdapter.WriteInt(self.param1)
	MsgAdapter.WriteInt(self.param2)
end

-- 背包物品分解
CSBagDecompose = CSBagDecompose or BaseClass(BaseProtocolStruct)
function CSBagDecompose:__init()
	self.msg_type = 1984
end

function CSBagDecompose:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteShort(self.column)
	MsgAdapter.WriteShort(self.index)
	MsgAdapter.WriteInt(self.num)
end

-- 时装升级请求
CSFashionCollectionRequest = CSFashionCollectionRequest or BaseClass(BaseProtocolStruct)
function CSFashionCollectionRequest:__init()
	self.msg_type = 1985
end

function CSFashionCollectionRequest:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.oper_type)
	MsgAdapter.WriteInt(self.param1)
end

-- 时装升级全部信息下发
CSFashionCollectionInfo = CSFashionCollectionInfo or BaseClass(BaseProtocolStruct)
function CSFashionCollectionInfo:__init()
	self.msg_type = 1986
end

function CSFashionCollectionInfo:Decode()
	local info = {}
	info.sp_level = MsgAdapter.ReadInt()
	info.levels = {}
	for i = 0, SURFACE_MAX_NUM - 1 do
		info.levels[i] = MsgAdapter.ReadUChar()
	end
	self.info = info
end

--==========================评分奖励==========================
SCScoreRewardInfo = SCScoreRewardInfo or BaseClass(BaseProtocolStruct)
function SCScoreRewardInfo:__init()
	self.msg_type = 1976
end

function SCScoreRewardInfo:Decode()
	self.score = MsgAdapter.ReadInt()
    self.flag = MsgAdapter.ReadInt()
end

--==========================月度投资==========================
SCMonthInvestInfo = SCMonthInvestInfo or BaseClass(BaseProtocolStruct)
function SCMonthInvestInfo:__init()
	self.msg_type = 1977
end

function SCMonthInvestInfo:Decode()
	local info = {}
	info.is_buy = MsgAdapter.ReadShort()
	info.last_num = MsgAdapter.ReadShort()
	info.day = MsgAdapter.ReadInt()
	self.flag = MsgAdapter.ReadInt()
	self.info = info
end

CSRewardFindNotifyCancelReq = CSRewardFindNotifyCancelReq or BaseClass(BaseProtocolStruct)
function CSRewardFindNotifyCancelReq:__init()
	self.msg_type = 1978
end

function CSRewardFindNotifyCancelReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

SCRewardFindNotify = SCRewardFindNotify or BaseClass(BaseProtocolStruct)
function SCRewardFindNotify:__init()
	self.msg_type = 1979
end

function SCRewardFindNotify:Decode()
	self.notify = MsgAdapter.ReadInt()
end

--奖励界面-在线礼包信息下发
SCDayRewardOnlineGiftInfo = SCDayRewardOnlineGiftInfo or BaseClass(BaseProtocolStruct)
function SCDayRewardOnlineGiftInfo:__init()
	self.msg_type = 1990
end

function SCDayRewardOnlineGiftInfo:Decode()
	self.online_gift_join_begin_timestamp = MsgAdapter.ReadUInt()
	self.online_gift_times = MsgAdapter.ReadUInt()
	self.online_gift_flag = MsgAdapter.IntFlag()   
end