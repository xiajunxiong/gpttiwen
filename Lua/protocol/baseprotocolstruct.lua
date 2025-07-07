
-- 协议基类
BaseProtocolStruct = BaseProtocolStruct or BaseClass()
function BaseProtocolStruct:__init()
	self.msg_type = 0
end

-- 编码
function BaseProtocolStruct:Encode()
end

-- 解码
function BaseProtocolStruct:Decode()
end

function BaseProtocolStruct:Send(net_id)
	MsgAdapter.Send(net_id)
end

function BaseProtocolStruct:EncodeAndSend(net_id)
	self:Encode()
	MsgAdapter.Send(net_id)
end

-- 协议池
ProtocolPool = ProtocolPool or BaseClass()
function ProtocolPool:__init()
	ProtocolPool.Instance = self
	self.protocol_list = {}
	self.protocol_list_by_type = {}
end

function ProtocolPool:__delete()
	self.protocol_list = {}
	self.protocol_list_by_type = {}
	if ProtocolPool.Instance == self then
		ProtocolPool.Instance = nil
	end
end

function ProtocolPool:Register(protocol)
	local reg_protocol = protocol.New()
	if nil ~= self.protocol_list_by_type[reg_protocol.msg_type] then
		Debuger.LogError("协议重复注册 msg_type:" .. reg_protocol.msg_type)
		reg_protocol:DeleteMe()
		return -1
	end

	self.protocol_list[protocol] = reg_protocol
	self.protocol_list_by_type[reg_protocol.msg_type] = reg_protocol
	return reg_protocol.msg_type
end

function ProtocolPool:GetProtocol(protocol)
	return self.protocol_list[protocol]
end

function ProtocolPool:GetProtocolByType(msg_type)
	return self.protocol_list_by_type[msg_type]
end
