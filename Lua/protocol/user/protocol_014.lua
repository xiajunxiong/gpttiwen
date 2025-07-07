
--返回角色信息
SCRoleInfoAck = SCRoleInfoAck or BaseClass(BaseProtocolStruct)
function SCRoleInfoAck:__init()
	self.msg_type = 1400
end

function SCRoleInfoAck:Decode()
	self.role_id = MsgAdapter.ReadInt()
	self.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	self.level = MsgAdapter.ReadShort()
	self.profession_id = MsgAdapter.ReadShort()
	self.top_level = MsgAdapter.ReadInt()
	self.title_id = MsgAdapter.ReadInt()
	self.facescroe_id = MsgAdapter.ReadInt() --颜值系统|头衔资源id
	self.cur_exp = MsgAdapter.ReadLL()
	self.guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)	
	self.move_speed = MAIN_PLAY_BASE_MOVE_SPEED * (100 + MsgAdapter.ReadInt()) / 100
	self.appearance = Appearance()
	self.create_time = MsgAdapter.ReadUInt()
	self.profession = math.modf(self.profession_id / 100)
	self.role_name = self.name
	self.kill_value = MsgAdapter.ReadInt()	--罪恶值
	self.m_authority_type = MsgAdapter.ReadChar()
	MsgAdapter.ReadChar()
	MsgAdapter.ReadShort()
	self.plat_type = MsgAdapter.ReadInt()
	self.role_uuid = int64.new(self.role_id, self.plat_type)
end

-- obj移速改变通知
SCRoleSpeedChangeNotice = SCRoleSpeedChangeNotice or BaseClass(BaseProtocolStruct)
function SCRoleSpeedChangeNotice:__init()
	self.msg_type = 1408
end

function SCRoleSpeedChangeNotice:Decode()
	self.obj_id = MsgAdapter.ReadInt()
	self.move_speed = MsgAdapter.ReadInt()
end

--货币数据变化统一网络包
SCMoney = SCMoney or BaseClass(BaseProtocolStruct)

function SCMoney:__init()
	self.msg_type = 1401
end

function SCMoney:Decode()
	--MsgAdapter.ReadShort()
	self.is_no_show = MsgAdapter.ReadShort()
	self.count = MsgAdapter.ReadShort()
	self.money_list = {}
	for i=1,self.count do
		MsgAdapter.ReadShort()
		table.insert(self.money_list,{msg_money_type=MsgAdapter.ReadShort(),money_num=MsgAdapter.ReadLL()})
	end
end

--attr_type=====common_enum:BattleAttriType
SCRoleAttrList = SCRoleAttrList or BaseClass(BaseProtocolStruct)

function SCRoleAttrList:__init()
	self.msg_type = 1402
end

function SCRoleAttrList:Decode()
	self.attr_count = MsgAdapter.ReadShort()
	self.notify_reason = MsgAdapter.ReadShort()
	self.capability = MsgAdapter.ReadInt()
	self.attr_list = {}
    for i=1,self.attr_count do
        self.attr_list[MsgAdapter.ReadInt()] = MsgAdapter.ReadInt()
    end
end

-- 登录时请求一系列消息
CSAllInfoReq = CSAllInfoReq or BaseClass(BaseProtocolStruct)
function CSAllInfoReq:__init()
	self.msg_type = 1450
end

function CSAllInfoReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

SCRoleExpChange = SCRoleExpChange or BaseClass(BaseProtocolStruct)

function SCRoleExpChange:__init()
	self.msg_type = 1404
end

function SCRoleExpChange:Decode()
	self.change_exp = MsgAdapter.ReadLL()
	self.cur_exp = MsgAdapter.ReadLL()
	self.change_reason = MsgAdapter.ReadShort() --变化原因
	MsgAdapter.ReadShort() 	--reserved	
end


SCRoleLevelChange = SCRoleLevelChange or BaseClass(BaseProtocolStruct)

function SCRoleLevelChange:__init()
	self.msg_type = 1405
end

function SCRoleLevelChange:Decode()
	self.change_type = MsgAdapter.ReadInt()-- 0 普通等级 1巅峰等级
	self.level = MsgAdapter.ReadInt()
	self.cur_exp = MsgAdapter.ReadLL()
end

SCRoleStatusChangeNotice = SCRoleStatusChangeNotice or BaseClass(BaseProtocolStruct)
function SCRoleStatusChangeNotice:__init()
	self.msg_type = 1409
end
function SCRoleStatusChangeNotice:Decode()
	self.obj_id = MsgAdapter.ReadUShort()
	self.behavior_state = MsgAdapter.ReadShort()
	self.cur_move_status = MsgAdapter.ReadShort() -- 0 正常 1 潜行 2 施法 3 引导 4 闪现
	MsgAdapter.ReadShort()
end

local function PlanDetail(index)
    local t = {}
    t.plan_name = MsgAdapter.ReadStrN(ATTRIBUTE_PLAN_NAME_LENGTH)
    t.add_point_num = MsgAdapter.ReadShort()
    t.add_point_list = {}
    local num = 0
    for i=0,ROLE_ATTRIBUTE_PLAN_MAX_NUM - 1 do
        local info = {name = Language.Role.AddPointTypeName[i],type = i}
        info.value = MsgAdapter.ReadShort()
        t.add_point_list[i] = info
        num = num + info.value
    end
    t.all_point_num = t.add_point_num + num
    if t.plan_name == "" then
        t.plan_name = Language.Role.DefaultPlanName .. index or 1
    end
    t.custom_add_point_list = {}
    for i = 0, ROLE_ATTRIBUTE_PLAN_MAX_NUM - 1 do
    	t.custom_add_point_list[i] = MsgAdapter.ReadShort()
    end
    MsgAdapter.ReadShort()
    return t
end

-- 人物加点方案信息
SCRoleAttributePlanInfo = SCRoleAttributePlanInfo or BaseClass(BaseProtocolStruct)

function SCRoleAttributePlanInfo:__init()
	self.msg_type = 1403
end

function SCRoleAttributePlanInfo:Decode()
	self.plan_num = MsgAdapter.ReadShort()
	self.cur_plan_index = MsgAdapter.ReadShort()
	self.plan_list = {}
	for i=0,MAX_ATTR_PLAN_NUM - 1 do
		self.plan_list[i] = PlanDetail(i+1)
	end
	self.reset_plan_times = MsgAdapter.ReadShort()
	self.plan_info = self.plan_list[self.cur_plan_index]
	self.tui_jian_auto_add = MsgAdapter.ReadShort() 	--首套加点方案的推荐方案是否生效中 非0生效、0是未生效
end

-- 创建加点方案
CSAttributePlanCreate = CSAttributePlanCreate or BaseClass(BaseProtocolStruct)

function CSAttributePlanCreate:__init()
	self.msg_type = 1451
end

function CSAttributePlanCreate:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteStrN(self.plan_name,ATTRIBUTE_PLAN_NAME_LENGTH)
end

-- 修改加点方案名称
CSAttributePlanChangeName = CSAttributePlanChangeName or BaseClass(BaseProtocolStruct)

function CSAttributePlanChangeName:__init()
	self.msg_type = 1452
end

function CSAttributePlanChangeName:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.plan_index)
	MsgAdapter.WriteStrN(self.plan_name,ATTRIBUTE_PLAN_NAME_LENGTH)
end

-- 切换加点方案
CSAttributePlanSwitchPlan = CSAttributePlanSwitchPlan or BaseClass(BaseProtocolStruct)

function CSAttributePlanSwitchPlan:__init()
	self.msg_type = 1453
end

function CSAttributePlanSwitchPlan:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.switch_to_index)
end

-- 修改加点方案属性
CSAttributePlanAddPoint = CSAttributePlanAddPoint or BaseClass(BaseProtocolStruct)

function CSAttributePlanAddPoint:__init()
	self.msg_type = 1454
end

function CSAttributePlanAddPoint:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteShort(self.plan_index)
	for i=0,ROLE_ATTRIBUTE_PLAN_MAX_NUM - 1 do
        MsgAdapter.WriteShort(self.add_point_list[i] or 0)
    end
end

-- 洗点加点方案
CSAttributePlanClearPoint = CSAttributePlanClearPoint or BaseClass(BaseProtocolStruct)

function CSAttributePlanClearPoint:__init()
	self.msg_type = 1455
end

function CSAttributePlanClearPoint:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.plan_index)
end

-- 人物职业变更通知
SCRoleProfessionChangeNotice = SCRoleProfessionChangeNotice or BaseClass(BaseProtocolStruct)

function SCRoleProfessionChangeNotice:__init()
	self.msg_type = 1406
	self.profession_id = 0
end

function SCRoleProfessionChangeNotice:Decode()
	self.profession_id = MsgAdapter.ReadInt()
	self.profession = math.modf(self.profession_id / 100)
end

-- 人物晋升职业
CSRoleCareerAdvanceReq = CSRoleCareerAdvanceReq or BaseClass(BaseProtocolStruct)

function CSRoleCareerAdvanceReq:__init()
	self.msg_type = 1456
end

function CSRoleCareerAdvanceReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.profession_id) -- 晋升到某个职业[小阶段时没用]
	MsgAdapter.WriteInt(self.to_color)      -- 晋升后改变颜色 [0,4)[小阶段时没用]
end

--人物设置
CSRoleSystemSetReq = CSRoleSystemSetReq or BaseClass(BaseProtocolStruct)

function CSRoleSystemSetReq:__init()
	self.msg_type = 1460
	self.count = 0
	self.setting_info = nil
end

function CSRoleSystemSetReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.count)
	
	for k,v in pairs(self.setting_info) do
		MsgAdapter.WriteShort(k)
		MsgAdapter.WriteShort(v)
	end
end

SCRoleSystemSetInfo = SCRoleSystemSetInfo or BaseClass(BaseProtocolStruct)

function SCRoleSystemSetInfo:__init()
	self.msg_type = 1461
	self.setting_info = {}
end

function SCRoleSystemSetInfo:Decode()
	for i=1,ROLE_SETTING_TYPE.ROLE_SETTING_TYPE_MAX do
		self.setting_info[i] = MsgAdapter.ReadInt()
	end
end

--人物转职
CSRoleCareerChangeReq = CSRoleCareerChangeReq or BaseClass(BaseProtocolStruct)

function CSRoleCareerChangeReq:__init()
	self.msg_type = 1457
	self.avartar_type = 0
	self.color = 0
	self.reserve_sh = 0
	self.to_profession = 0
end

function CSRoleCareerChangeReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteChar(self.avartar_type)
	MsgAdapter.WriteChar(self.color)
	MsgAdapter.WriteShort(self.reserve_sh)
	MsgAdapter.WriteInt(self.to_profession)
end


SCProfessionExtraInfo = SCProfessionExtraInfo or BaseClass(BaseProtocolStruct)

function SCProfessionExtraInfo:__init()
	self.msg_type = 1407
	self.change_prof_times = 1
end

function SCProfessionExtraInfo:Decode()
	self.change_prof_times = MsgAdapter.ReadInt()
end


CSProfessionExtraInfoReq = CSProfessionExtraInfoReq or BaseClass(BaseProtocolStruct)

function CSProfessionExtraInfoReq:__init()
	self.msg_type = 1458
end

function CSProfessionExtraInfoReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end


SCReturnOriginalSever = SCReturnOriginalSever or BaseClass(BaseProtocolStruct)
function SCReturnOriginalSever:__init()
	self.msg_type = 14001
end
function SCReturnOriginalSever:Decode()
end

-- 客户端自定义设置-群组请求
CSRoleSystemSetChatGroupReq = CSRoleSystemSetChatGroupReq or BaseClass(BaseProtocolStruct)
function CSRoleSystemSetChatGroupReq:__init()
	self.msg_type = 1462
end

function CSRoleSystemSetChatGroupReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt2(self.group_id)
	MsgAdapter.WriteInt(self.set_value)		-- 0:免打扰 1:打扰
end

-- 客户端自定义设置-群组信息
SCRoleSystemSetChatGroupInfo = SCRoleSystemSetChatGroupInfo or BaseClass(BaseProtocolStruct)
function SCRoleSystemSetChatGroupInfo:__init()
	self.msg_type = 1463
end

function SCRoleSystemSetChatGroupInfo:Decode()
	self.count = MsgAdapter.ReadInt()
    self.groups = {}
	for i = 1, self.count do
        self.groups[i] = {
			group_id = MsgAdapter.ReadInt2(),
			set_value = MsgAdapter.ReadInt(),
		}
    end
end

-- 角色自动对话信息下发
SCRoleAutoTalkInfo = SCRoleAutoTalkInfo or BaseClass(BaseProtocolStruct)
function SCRoleAutoTalkInfo:__init()
	self.msg_type = 1410
end

function SCRoleAutoTalkInfo:Decode()
	self.notice_type = MsgAdapter.ReadInt()		-- 0 初始化，刚登陆的时候，不需要给玩家发对话 1 切换对话id，给玩家发送对话
	self.talk_groups_id = MsgAdapter.ReadInt()
	self.talk_id = MsgAdapter.ReadInt()
end
--角色权限信息下发
SCRoleAuthorityInfo = SCRoleAuthorityInfo or BaseClass(BaseProtocolStruct)
function SCRoleAuthorityInfo:__init()
	self.msg_type = 1411
end

function SCRoleAuthorityInfo:Decode()
	self.m_authority_type = MsgAdapter.ReadChar()
	MsgAdapter.ReadChar()
	MsgAdapter.ReadShort()
end

--角色历史评分变化下发
SCRoleHistoryMaxCapabilityChange = SCRoleHistoryMaxCapabilityChange or BaseClass(BaseProtocolStruct)
function SCRoleHistoryMaxCapabilityChange:__init()
	self.msg_type = 1412
end

function SCRoleHistoryMaxCapabilityChange:Decode()
	self.max_role_capability = MsgAdapter.ReadInt()
end
--角色职业颜色变更请求
CSRoleChangeColorReq = CSRoleChangeColorReq or BaseClass(BaseProtocolStruct)
function CSRoleChangeColorReq:__init()
	self.msg_type = 1459
end

function CSRoleChangeColorReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.to_color)		-- 目标颜色
end