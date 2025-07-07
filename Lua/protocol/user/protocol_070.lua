
---账号登录
CSLoginToAccount = CSLoginToAccount or BaseClass(BaseProtocolStruct)
function CSLoginToAccount:__init()
	self.msg_type = 7056
	self.rand_val1 = 0		--随机数 100w-1000w
	self.login_time = 0
	self.login_str = ""
	self.pname = ""
	self.rand_val2 = 0		--随机数 100w-1000w
	--reserved
	self.server = 0
end

function CSLoginToAccount:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.rand_val1)
	MsgAdapter.WriteUInt(self.login_time)
	MsgAdapter.WriteStrN(self.login_str, PROTC_LOGIN_STR_LEN)
	MsgAdapter.WriteStrN(self.pname, PROTC_PLAT_NAME_LEN)
	MsgAdapter.WriteInt(self.rand_val2)
	MsgAdapter.WriteShort(0)
	MsgAdapter.WriteShort(self.server)
end

--账号登录回调
SCLoginToAccount = SCLoginToAccount or BaseClass(BaseProtocolStruct)
function SCLoginToAccount:__init()
	self.msg_type = 7001
end

function SCLoginToAccount:Decode()
	self.index = MsgAdapter.ReadShort()
	self.result = MsgAdapter.ReadShort()
	self.count = MsgAdapter.ReadInt()
	self.role_item = {}
	for i=1, self.count do
		local ri = {}
		ri.role_id = MsgAdapter.ReadInt()
		ri.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		ri.profession = MsgAdapter.ReadInt()
		ri.avatar_type = MsgAdapter.ReadShort()
		ri.color = MsgAdapter.ReadShort()
		ri.level = MsgAdapter.ReadShort()
		ri.avatar_id = MsgAdapter.ReadShort()--人物头像
		ri.weapon_id = MsgAdapter.ReadInt()
		ri.top_level = MsgAdapter.ReadInt()
		ri.fly_flag = MsgAdapter.ReadShort()
		ri.fly_flag = ri.fly_flag > FlyUpStageType.High and FlyUpStageType.High or ri.fly_flag
		ri.reserve_sh = MsgAdapter.ReadShort()
		ri.avatar_quality = ri.fly_flag
		ri.surface_list = {}
		for j = 0,5 do 
			ri.surface_list[j] = MsgAdapter.ReadShort()
		end 
		ri.forbid_time = MsgAdapter.ReadUInt()
		self.role_item[i] = ri
	end
end


--登录角色
CSLoginToRole = CSLoginToRole or BaseClass(BaseProtocolStruct)

function CSLoginToRole:__init()
	self.msg_type = 7057

	self.rand_val1 = 0		--随机数 100w-1000w
	self.login_time = 0
	self.login_str = ""
	self.pname = ""
	self.server = 0
	--reserved
	self.anti_wallow = 0
	self.rand_val2 = 0		--随机数 100w-1000w
	self.role_id = 0
end

function CSLoginToRole:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.rand_val1)
	MsgAdapter.WriteUInt(self.login_time)
	MsgAdapter.WriteStrN(self.login_str, PROTC_LOGIN_STR_LEN)
	MsgAdapter.WriteStrN(self.pname, PROTC_PLAT_NAME_LEN)
	MsgAdapter.WriteShort(self.server)
	MsgAdapter.WriteChar(0)
	MsgAdapter.WriteChar(self.anti_wallow)
	MsgAdapter.WriteInt(self.rand_val2)
	MsgAdapter.WriteInt(self.role_id)
end

--登录角色回调
SCLoginToRole = SCLoginToRole or BaseClass(BaseProtocolStruct)
function SCLoginToRole:__init()
	self.msg_type = 7000
end

function SCLoginToRole:Decode()
	self.role_id = MsgAdapter.ReadInt()
	self.result = MsgAdapter.ReadShort()
	MsgAdapter.ReadChar()			--reserved
	self.is_merged_server = MsgAdapter.ReadChar()
	self.scene_id = MsgAdapter.ReadInt()
	self.last_scene_id = MsgAdapter.ReadInt()
	self.key = MsgAdapter.ReadStrN(PROTC_SESSION_KEY_LEN)
	self.time = MsgAdapter.ReadUInt()
	self.gs_hostname = MsgAdapter.ReadStrN(PROTC_HOST_NAME_LEN)
	self.gs_port = MsgAdapter.ReadUShort()
	self.gs_index = MsgAdapter.ReadUShort()
	self.server_time = MsgAdapter.ReadUInt()
	self.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	self.backup_gs_hostname = MsgAdapter.ReadStrN(PROTC_HOST_NAME_LEN)
	self.backup_gs_port = MsgAdapter.ReadUShort()
	MsgAdapter.ReadShort()
	self.create_time = MsgAdapter.ReadUInt()
end

-- 登陆服心跳请求
CSLHeartBeat = CSLHeartBeat or BaseClass(BaseProtocolStruct)
function CSLHeartBeat:__init()
	self.msg_type = 7052
end

function CSLHeartBeat:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

-- 登陆服心跳返回
SCLHeartBeat = SCLHeartBeat or BaseClass(BaseProtocolStruct)
function SCLHeartBeat:__init()
	self.msg_type = 7007
end

function SCLHeartBeat:Decode()
end
