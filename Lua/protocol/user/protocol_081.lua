
-- 通知目标不在线
SCSingleChatUserNotExist = SCSingleChatUserNotExist or BaseClass(BaseProtocolStruct)
function SCSingleChatUserNotExist:__init()
	self.msg_type = 8102
end

function SCSingleChatUserNotExist:Decode()
	self.to_uid = MsgAdapter.ReadInt()
end