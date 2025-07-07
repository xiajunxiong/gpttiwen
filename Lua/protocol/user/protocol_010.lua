-- 进入场景请求
CSUserEnterGSReq = CSUserEnterGSReq or BaseClass(BaseProtocolStruct)
function CSUserEnterGSReq:__init()
	self.msg_type = 1050

	self.scene_id = 0
	self.scene_key = 0
	self.last_scene_id = 0
	self.role_id = 0
	self.role_name = ""
	self.time = 0
	self.is_login = 0
	--reserved
	self.server = 0
	self.key = ""
	self.pname = ""
	self.is_micro_pc = 0
	self.plat_spid = ""
end

function CSUserEnterGSReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.scene_id)
	MsgAdapter.WriteInt(self.scene_key)
	MsgAdapter.WriteInt(self.last_scene_id)
	MsgAdapter.WriteInt(self.role_id)
	MsgAdapter.WriteStrN(self.role_name, PROTC_ROLE_NAME_LEN)
	MsgAdapter.WriteInt(self.time)
	MsgAdapter.WriteChar(self.is_login)
	MsgAdapter.WriteChar(0)			--reserved
	MsgAdapter.WriteShort(self.server)
	MsgAdapter.WriteStrN(self.key, PROTC_SESSION_KEY_LEN)
	MsgAdapter.WriteStrN(self.pname, PROTC_PLAT_NAME_LEN)
	MsgAdapter.WriteInt(self.is_micro_pc)
	MsgAdapter.WriteStrN(self.plat_spid, PLAT_SPID)
end


-- 进入场景返回
SCUserEnterGSAck = SCUserEnterGSAck or BaseClass(BaseProtocolStruct)
function SCUserEnterGSAck:__init()
	self.msg_type = 1000
end

function SCUserEnterGSAck:Decode()
	self.result = MsgAdapter.ReadInt()
	self.is_on_crosssever = MsgAdapter.ReadInt()
end


-- 心跳包
CSHeartbeatReq = CSHeartbeatReq or BaseClass(BaseProtocolStruct)
function CSHeartbeatReq:__init()
	self.msg_type = 1053
end

function CSHeartbeatReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

-- 人物改名
CSRoleResetName = CSRoleResetName or BaseClass(BaseProtocolStruct)
function CSRoleResetName:__init()
	self.msg_type = 1054
	self.new_name = ""
end

function CSRoleResetName:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteStrN(self.new_name,PROTC_ROLE_NAME_LEN)
end

-- 心跳回包
SCHeartbeatResp = SCHeartbeatResp or BaseClass(BaseProtocolStruct)
function SCHeartbeatResp:__init()
	self.msg_type = 1003
end

function SCHeartbeatResp:Decode()

end

-- 某个角色改名
SCRoleResetName = SCRoleResetName or BaseClass(BaseProtocolStruct)
function SCRoleResetName:__init()
	self.msg_type = 1004
	self.obj_id = 0			-- 角色id
	self.new_name = ""		-- 角色新名字
end

function SCRoleResetName:Decode()
	self.obj_id = MsgAdapter.ReadUShort()
	MsgAdapter.ReadShort()
	self.new_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	self.name = self.new_name
	self.role_name = self.new_name
end

-- 订阅/取消订阅系统信息
CSRoleSubscribe = CSRoleSubscribe or BaseClass(BaseProtocolStruct)
function CSRoleSubscribe:__init()
	self.msg_type = 1057
end

function CSRoleSubscribe:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteShort(self.is_unsubscribe)		-- 0订阅 1取消订阅
	MsgAdapter.WriteShort(self.topic)				-- 订阅的主题
end

--客服系统，通知有多少条新的客服消息
SCNoticeClientNewMessageFromCustomService = SCNoticeClientNewMessageFromCustomService or BaseClass(BaseProtocolStruct)
function SCNoticeClientNewMessageFromCustomService:__init()
	self.msg_type = 1005
end

function SCNoticeClientNewMessageFromCustomService:Decode()
	self.msg_count = MsgAdapter.ReadInt()
end