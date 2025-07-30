
-- 创建角色请求
CSCreateRole = CSCreateRole or BaseClass(BaseProtocolStruct)
function CSCreateRole:__init()
	self.msg_type = 7150

	self.pname = ""
	self.role_name = ""
	self.login_time = 0
	self.login_str = ""
	self.server = 0
	self.avatar_type = 0
	self.init_appearance = 0
	self.name_str_id = 0
	self.job_id = 0
	self.plat_spid_str = ""
	self.mini_game_id = 0
	self.is_auto_create = 0
	self.audit_seq = 0
end

function CSCreateRole:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteStrN(self.pname, PROTC_PLAT_NAME_LEN)
	MsgAdapter.WriteStrN(self.role_name, PROTC_ROLE_NAME_LEN)
	MsgAdapter.WriteUInt(self.login_time)
	MsgAdapter.WriteStrN(self.login_str, PROTC_LOGIN_STR_LEN)
	MsgAdapter.WriteShort(self.server)
	MsgAdapter.WriteChar(self.avatar_type)
	MsgAdapter.WriteChar(self.color)
	MsgAdapter.WriteLL(self.name_str_id)
	MsgAdapter.WriteInt(self.job_id)
	MsgAdapter.WriteStrN(self.plat_spid_str, PLAT_SPID)
	MsgAdapter.WriteInt(self.mini_game_id)
	MsgAdapter.WriteChar(self.is_auto_create)
	MsgAdapter.WriteChar(self.audit_seq)
	MsgAdapter.WriteShort(0)
end

-- 创建角色返回
SCCreateRoleAck = SCCreateRoleAck or BaseClass(BaseProtocolStruct)
function SCCreateRoleAck:__init()
	self.msg_type = 7100
end

function SCCreateRoleAck:Decode()
	self.result = MsgAdapter.ReadInt()
	self.role_id = MsgAdapter.ReadInt()
	self.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	MsgAdapter.ReadShort()			--reserved
	self.avatar_type = MsgAdapter.ReadChar()
	self.color = MsgAdapter.ReadChar()
	self.create_role_time = MsgAdapter.ReadLL()
	LogError("SCCreateRoleAck:Decode, result:"..tostring(self.result).."_role_id:"..tostring(self.role_id).."_role_name:"..tostring(self.role_name).."_avatar_type:"..tostring(self.avatar_type).."_color:"..tostring(self.color).."_create_role_time:"..tostring(self.create_role_time))
end

-- 角色请求
CSRandomNameReq = CSRandomNameReq or BaseClass(BaseProtocolStruct)
function CSRandomNameReq:__init()
	self.msg_type = 7153
end

function CSRandomNameReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.sex)
end

SCRandomName = SCRandomName or BaseClass(BaseProtocolStruct)
function SCRandomName:__init()
	self.msg_type = 7154
end

function SCRandomName:Decode()
	local t = {}
	t.front_sex = MsgAdapter.ReadShort()
	t.front_id = MsgAdapter.ReadShort()
	t.middle_sex = MsgAdapter.ReadShort()
	t.middle_id = MsgAdapter.ReadShort()
	t.back_sex = MsgAdapter.ReadShort()
	t.back_id = MsgAdapter.ReadShort()
	t.name_str_id = MsgAdapter.ReadLL()
	self.name_info = t
end

CSStoryBattleInfo = CSStoryBattleInfo or BaseClass(BaseProtocolStruct)
function CSStoryBattleInfo:__init()
	self.msg_type = 7155
end
function CSStoryBattleInfo:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

CSStoryRoundResult = CSStoryRoundResult or BaseClass(BaseProtocolStruct)
function CSStoryRoundResult:__init()
	self.msg_type = 7156
end
function CSStoryRoundResult:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.round)--从0开始两回合
end

SCStoryNotAvaliable = SCStoryNotAvaliable or BaseClass(BaseProtocolStruct)
function SCStoryNotAvaliable:__init()
	self.msg_type = 7157
end
function SCStoryNotAvaliable:Decode()
	
end