SCSkillList = SCSkillList or BaseClass(BaseProtocolStruct)
function SCSkillList:__init()
	self.msg_type = 1300
end
function SCSkillList:Decode()
	self.skill_count = MsgAdapter.ReadInt()
	self.skill_list = {}
	for i = 1, self.skill_count do
		self.skill_list[i] = self.ReadItemInfo()
	end
	self.common_use_skill_num = MsgAdapter.ReadInt()
	self.common_use_skill_index_list = {}
	for i = 1, self.common_use_skill_num do
		self.common_use_skill_index_list[i] = MsgAdapter.ReadShort()
	end
end
function SCSkillList.ReadItemInfo()
	local SkillItemInfo = {}
	SkillItemInfo.index = MsgAdapter.ReadShort()
	SkillItemInfo.skill_id = MsgAdapter.ReadUShort()
	SkillItemInfo.skill_level = MsgAdapter.ReadShort()
	SkillItemInfo.reserve_sh = MsgAdapter.ReadShort()
	SkillItemInfo.skill_exp = MsgAdapter.ReadInt()
	return SkillItemInfo
end

SCSkillChangeNotice = SCSkillChangeNotice or BaseClass(BaseProtocolStruct)
function SCSkillChangeNotice:__init()
	self.msg_type = 1301
end
function SCSkillChangeNotice:Decode()
	self.skill = SCSkillList.ReadItemInfo()
end


--获取技能列表 op_type:0
--学习技能 op_type:1 param1:技能id （用来学习技能，不能升级）
--使用技能草 op_type:2 param1:技能id param2:技能草类型 param3:是否批量[0否 1是]
--设置常用技能 op_type:3 param1:技能序号(index) param2:[0:添加常用 1:删除常用] 
--升级技能（一键）op_type:4 p1:技能id，p2 目标等级	（旧，不要了，用5）
--升级技能（新，只能升级技能，不能学习）op_type:5 param1:技能id param2:是否勾选所有技能升级
CSSkillOprateReq = CSSkillOprateReq or BaseClass(BaseProtocolStruct)
function CSSkillOprateReq:__init()
	self.msg_type = 1350
end

function CSSkillOprateReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.operate_type)
	MsgAdapter.WriteInt(self.param1)
	MsgAdapter.WriteInt(self.param2)
	MsgAdapter.WriteInt(self.param3)
end

CSSkillPositionModify = CSSkillPositionModify or BaseClass(BaseProtocolStruct)
function CSSkillPositionModify:__init()
	self.msg_type = 1351
end

function CSSkillPositionModify:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.skill_num)
	for i = 1, self.skill_num do
		MsgAdapter.WriteShort(self.skill_idx_list[i])
	end
end

SCCommonUseSkillChangeNotice = SCCommonUseSkillChangeNotice or BaseClass(BaseProtocolStruct)
function SCCommonUseSkillChangeNotice:__init()
	self.msg_type = 1303
end

function SCCommonUseSkillChangeNotice:Decode()
	self.common_use_skill_index_list = {}
	self.common_use_skill_num = MsgAdapter.ReadInt()
	local MAX_SKILL_NUM = 32
	for i=1, self.common_use_skill_num do
		local is_use = MsgAdapter.ReadShort()
		self.common_use_skill_index_list[i] = is_use
	end
end

SCLifeSkillInfo = SCLifeSkillInfo or BaseClass(BaseProtocolStruct)
function SCLifeSkillInfo:__init()
	self.msg_type = 1302
	self.reason_type = 0
	self.count = 0
end

function SCLifeSkillInfo:Decode()
	self.life_skill_list = {}
	self.reason_type = MsgAdapter.ReadShort()
	self.count = MsgAdapter.ReadShort()
	for i=1,self.count do
		local skill_id = MsgAdapter.ReadInt()
		local skill_level = MsgAdapter.ReadShort()
		local reserve_sh = MsgAdapter.ReadShort()
		local skill_exp = MsgAdapter.ReadInt()
		table.insert(self.life_skill_list, {skill_id = skill_id, skill_level = skill_level,reserve_sh = reserve_sh,skill_exp = skill_exp})
	end
end


SCSkillExpDeltaChangeNotify = SCSkillExpDeltaChangeNotify or BaseClass(BaseProtocolStruct)
function SCSkillExpDeltaChangeNotify:__init()
	self.msg_type = 1304
end

function SCSkillExpDeltaChangeNotify:Decode()
	self.skill_id = MsgAdapter.ReadInt()
	self.exp_change = MsgAdapter.ReadInt()
end

SCNoticeClientReloadSensitiveWords = SCNoticeClientReloadSensitiveWords or BaseClass(BaseProtocolStruct)
function SCNoticeClientReloadSensitiveWords:__init()
	self.msg_type = 1320
end

function SCNoticeClientReloadSensitiveWords:Decode()
end

-- 生活技能请求
CSLiveSkillReq = CSLiveSkillReq or BaseClass(BaseProtocolStruct)
function CSLiveSkillReq:__init()
	self.msg_type = 1310
end

function CSLiveSkillReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.op_type)
	MsgAdapter.WriteInt(self.p1)
	MsgAdapter.WriteInt(self.p2)
	MsgAdapter.WriteInt(self.p3)
	MsgAdapter.WriteInt(self.p4)
	--LogError("生活技能请求", self)
end

-- 生活技能全部信息
SCLiveSkillAllInfo = SCLiveSkillAllInfo or BaseClass(BaseProtocolStruct)
function SCLiveSkillAllInfo:__init()
	self.msg_type = 1311
end

function SCLiveSkillAllInfo:Decode()
	local count = MsgAdapter.ReadInt()
	self.info_list = {} 	-- key : 技能id, value : 技能等级
	for i = 0, count - 1 do
		local skill_info = {}
		skill_info.level = MsgAdapter.ReadShort()
		MsgAdapter.ReadShort()
		skill_info.trigger_double_count = MsgAdapter.ReadInt()
		self.info_list[i] = skill_info
	end
	--LogError("生活技能全部全部全部信息", self)
end

-- 生活技能单个信息
SCLiveSkillInfo = SCLiveSkillInfo or BaseClass(BaseProtocolStruct)
function SCLiveSkillInfo:__init()
	self.msg_type = 1312
end

function SCLiveSkillInfo:Decode()
	self.skill_type = MsgAdapter.ReadInt()
	local skill_info = {}
	skill_info.level = MsgAdapter.ReadShort()
	MsgAdapter.ReadShort()
	skill_info.trigger_double_count = MsgAdapter.ReadInt()
	self.skill_info = skill_info
	--LogError("生活技能单个信息", self)
end

-- 生活食物类型信息下发
SCLivePropsAllInfo = SCLivePropsAllInfo or BaseClass(BaseProtocolStruct)
function SCLivePropsAllInfo:__init()
	self.msg_type = 1313
end

function SCLivePropsAllInfo:Decode()
	local count = MsgAdapter.ReadInt()
	self.info_list = {}
	for i = 0, count - 1 do
		local info = {}
		info.seq = MsgAdapter.ReadShort()
		info.end_timestamp = MsgAdapter.ReadUInt()
		self.info_list[i] = info
	end
	local card_count = MsgAdapter.ReadInt()
	self.card_list = {}
	for i = 1, card_count do
		local info = {}
		info.seq = MsgAdapter.ReadShort()
		info.point_num = MsgAdapter.ReadUInt()		-- 变身卡幻化点
		self.card_list[info.seq] = info
	end
	--LogError("生活食物类型全部信息1313：：",card_count,self)
end

-- 生活食物类型单个信息下发
SCLivePropsInfo = SCLivePropsInfo or BaseClass(BaseProtocolStruct)
function SCLivePropsInfo:__init()
	self.msg_type = 1314
end

function SCLivePropsInfo:Decode()
	self.is_noitce = MsgAdapter.ReadInt()
	self.prop_type = MsgAdapter.ReadInt()
	self.param1 = MsgAdapter.ReadInt()
	local info = {}
	info.seq = MsgAdapter.ReadShort()
	MsgAdapter.ReadShort()
	info.end_timestamp = MsgAdapter.ReadUInt()
	self.prop_info = info
	--LogError("生活食物类型信息1314：：",self.is_noitce)
end

CSFishGroundReq = CSFishGroundReq or BaseClass(BaseProtocolStruct)
function CSFishGroundReq:__init()
	self.msg_type = 1315
end
function CSFishGroundReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.op_type)
	MsgAdapter.WriteInt(self.param1)
	-- LogDG(self)
end

SCFishingNotify = SCFishingNotify or BaseClass(BaseProtocolStruct)
function SCFishingNotify:__init()
	self.msg_type = 1316
end
function SCFishingNotify:Decode()
	self.role_obj_id = MsgAdapter.ReadInt()
	-- LogDG(self)
end
SCStopFishingNotify = SCStopFishingNotify or BaseClass(BaseProtocolStruct)
function SCStopFishingNotify:__init()
	self.msg_type = 1317
end
function SCStopFishingNotify:Decode()
	self.role_obj_id = MsgAdapter.ReadInt()
	self.stop_reason = MsgAdapter.ReadInt()
	self.param1 = MsgAdapter.ReadInt()
	self.param2 = MsgAdapter.ReadInt()
	-- LogDG(self)
end

-- 变身卡单条信息下发
SCLiveCardInfo = SCLiveCardInfo or BaseClass(BaseProtocolStruct)
function SCLiveCardInfo:__init()
	self.msg_type = 1318
end

function SCLiveCardInfo:Decode()
	self.seq = MsgAdapter.ReadInt()
	self.point_num = MsgAdapter.ReadInt()
end

--食谱信息
SCLifeSkillRecipeInfo =SCLifeSkillRecipeInfo  or BaseClass(BaseProtocolStruct)
function SCLifeSkillRecipeInfo:__init()
	self.msg_type = 1319
end

function SCLifeSkillRecipeInfo:Decode()
    local flag1 = MsgAdapter.ReadUInt()
    local flag2 = MsgAdapter.ReadUInt()
    self.active_recipe_flag = DataHelper.ReverseList(bit:ll2b(flag2, flag1)) -- 激活的食谱信息
end

-- 等待任务列表
SCTaskWaitingList = SCTaskWaitingList or BaseClass(BaseProtocolStruct)
function SCTaskWaitingList:__init()
	self.msg_type = 1352
end

function SCTaskWaitingList:Decode()
	self.task_count = MsgAdapter.ReadInt() -- 任务数量
	self.task_wait_list = {}
	for i = 1, self.task_count do
        local task_id = MsgAdapter.ReadUShort()
		MsgAdapter.ReadShort()
		local wait_timestamp = MsgAdapter.ReadUInt()
        self.task_wait_list[task_id] = wait_timestamp
    end
end
local SpecialTaskInfo = function()
	return MsgAdapter.IntFlag(4)
end
CSSpecialAnecdoteReq = CSSpecialAnecdoteReq or BaseClass(BaseProtocolStruct)
function CSSpecialAnecdoteReq:__init()
	self.msg_type = 1354
end
function CSSpecialAnecdoteReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.task_type)
	MsgAdapter.WriteInt(self.task_seq)
end

SCSpecialAnecdoteList = SCSpecialAnecdoteList or BaseClass(BaseProtocolStruct)
function SCSpecialAnecdoteList:__init()
	self.msg_type = 1355
end
function SCSpecialAnecdoteList:Decode()
	self.count = 100--100个类型当前只有到1个
	self.task_list = {}
	for i = 0, self.count - 1 do
		self.task_list[i] = SpecialTaskInfo()
	end
end
SCSpecialAnecdoteSingleList = SCSpecialAnecdoteSingleList or BaseClass(BaseProtocolStruct)
function SCSpecialAnecdoteSingleList:__init()
	self.msg_type = 1356
end
function SCSpecialAnecdoteSingleList:Decode()
	self.task_type = MsgAdapter.ReadInt()
	self.task_info = SpecialTaskInfo()
end