
MsgAdapter = MsgAdapter or {}

local msg_handler = Game.MsgHandler.getInstance()
function MsgAdapter.GetSendMsgType()
	return msg_handler:GetSendMsgType()
end

function MsgAdapter.SetSendMsgType(msg_type)
	msg_handler:SetSendMsgType(msg_type)
end

-- 读
function MsgAdapter.ReadChar()
	local char = msg_handler:ReadChar()
	if char == 255 then
		char = -1
	end
	return char
end

function MsgAdapter.ReadUChar()
	return msg_handler:ReadUChar()
end

function MsgAdapter.ReadShort()
	return msg_handler:ReadShort()
end

function MsgAdapter.ReadUShort()
	return msg_handler:ReadUShort()
end

function MsgAdapter.ReadInt()
	return msg_handler:ReadInt()
end

function MsgAdapter.ReadUInt()
	return msg_handler:ReadUInt()
end

function MsgAdapter.ReadLL() 
	return msg_handler:ReadLL()
end

function MsgAdapter.ReadULL()
	return msg_handler:ReadULL()
end

function MsgAdapter.ReadFloat()
	return msg_handler:ReadFloat()
end

function MsgAdapter.ReadDouble()
	return msg_handler:ReadDouble()
end

function MsgAdapter.ReadStrN(str_len)
	return msg_handler:ReadStrN(str_len)
end

function MsgAdapter.ReadResult()
	return msg_handler:ReadResult()
end

-- 写
function MsgAdapter.WriteBegin(msg_type)
	msg_handler:WriteBegin()
	if msg_type then
		msg_handler:SetSendMsgType(msg_type)
	end
	MsgAdapter.WriteShort(0)
end

function MsgAdapter.WriteChar(value)
	msg_handler:WriteChar(value)
end

function MsgAdapter.WriteUChar(value)
	msg_handler:WriteUChar(value)
end

function MsgAdapter.WriteShort(value)
	msg_handler:WriteShort(value)
end

function MsgAdapter.WriteUShort(value)
	msg_handler:WriteUShort(value)
end

function MsgAdapter.WriteInt(value)
	msg_handler:WriteInt(value)
end

function MsgAdapter.WriteUInt(value)
	msg_handler:WriteUInt(value)
end

function MsgAdapter.WriteLL(value)
	msg_handler:WriteLL(value)
end

function MsgAdapter.WriteFloat(value)
	msg_handler:WriteFloat(value)
end

function MsgAdapter.WriteDouble(value)
	msg_handler:WriteDouble(value)
end

function MsgAdapter.WriteStrN(str, str_len)
	return msg_handler:WriteStrN(str, str_len)
end

-- 会自动写入一个int表示字符串长度
function MsgAdapter.WriteStr(str)
	return msg_handler:WriteStr(str)
end

-- 发送消息
function MsgAdapter.Send(netid)
	return msg_handler:Send(netid)
end

function MsgAdapter.WriteInt2(value)
	MsgAdapter.WriteInt(value[1] or 0)
	MsgAdapter.WriteInt(value[2] or 0)
end

function MsgAdapter.ReadInt2()
	return {MsgAdapter.ReadInt(), MsgAdapter.ReadInt()}
end

function MsgAdapter.WriteUInt2(value)
	MsgAdapter.WriteUInt(value[1] or 0)
	MsgAdapter.WriteUInt(value[2] or 0)
end

function MsgAdapter.ReadUInt2()
	return {MsgAdapter.ReadUInt(), MsgAdapter.ReadUInt()}
end

-- 1个Int(32) = 2个Short(16) = 4个Char(8) 这几个可以互相转换 !!! 可以合并为Int的32位二进制 ！！！

--读取一个Short的二进制 长度为 16 * n 位（正序） 使用 flag[seq] （seq 配置上的字段）
--默认从0开始，start 为true时 从1开始 两个Short或偶数个Short的时候 可以使用 IntFlag
-- n 默认为 1 n = 1 时可不传n
function MsgAdapter.ShortFlag(n,start)
	local t = {}
	for i=1,n or 1 do
		t[i] = MsgAdapter.ReadUShort()
	end
	if n == 0 then
		t = {MsgAdapter.ReadShort()}
	end
	return bit:SAnd(t,start)
end

--读取一个int的二进制 长度为 32 * n 位（正序） 使用 flag[seq] （seq 配置上的字段）
--默认从0开始，start 为true时 从1开始
-- n 默认为 1 n = 1 时可不传n
function MsgAdapter.IntFlag(n,start)
	local t = {}
	for i=1,n or 1 do
		t[i] = MsgAdapter.ReadUInt()
	end
	if n == 0 then
		t = {MsgAdapter.ReadInt()}
	end
	return bit:And(t,start)
end