
-- 服务器时间返回
SCTimeAck = SCTimeAck or BaseClass(BaseProtocolStruct)
function SCTimeAck:__init()
	self.msg_type = 9001
	self.server_time = 0
	self.server_real_start_time = 0
	self.open_days = 0
	self.server_real_combine_time = 0
	self.server_six_start_time = 0
	self.is_server_driving_send = 0
end

function SCTimeAck:Decode()
	self.server_time = MsgAdapter.ReadUInt()
	self.server_real_start_time = MsgAdapter.ReadUInt()
	self.open_days = MsgAdapter.ReadInt()
	self.server_real_combine_time = MsgAdapter.ReadUInt()
	self.server_six_start_time = MsgAdapter.ReadUInt()
	self.is_server_driving_send = MsgAdapter.ReadChar()
	MsgAdapter.ReadChar()
	MsgAdapter.ReadShort()
end

SCDisconnectNotice = SCDisconnectNotice or BaseClass(BaseProtocolStruct)
function SCDisconnectNotice:__init()
	self.msg_type = 9003
end

function SCDisconnectNotice:Decode()
	self.reason = MsgAdapter.ReadInt()	--1:顶号
end

-- SCTimeDateInfo = SCTimeDateInfo or BaseClass(BaseProtocolStruct)
-- function SCTimeDateInfo:__init()
-- 	self.msg_type = 9004
-- end

-- function SCTimeDateInfo:Decode()
-- 	self.time = MsgAdapter.ReadUInt()
-- 	self.year = MsgAdapter.ReadUShort()
-- 	self.month = MsgAdapter.ReadUShort() + 1 --服务器序号从0开始
-- 	self.day = MsgAdapter.ReadUChar()
-- 	self.hour = MsgAdapter.ReadUChar()
-- 	self.min = MsgAdapter.ReadUChar()
-- 	self.sec = MsgAdapter.ReadUChar()
-- end

-- 请求服务器时间
CSTimeReq = CSTimeReq or BaseClass(BaseProtocolStruct)
function CSTimeReq:__init()
	self.msg_type = 9051
end

function CSTimeReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

SCCrossConnectInfo = SCCrossConnectInfo or BaseClass(BaseProtocolStruct)
function SCCrossConnectInfo:__init()
	self.msg_type = 9005
end

function SCCrossConnectInfo:Decode()
	self.is_connected_cross = MsgAdapter.ReadChar() --0未连接 1已连接
	self.is_cross = MsgAdapter.ReadChar()           --0在原服 1在跨服
	MsgAdapter.ReadShort()
end