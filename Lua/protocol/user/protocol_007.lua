-- 服务器返回错误信息
SCNoticeNum = SCNoticeNum or BaseClass(BaseProtocolStruct)
function SCNoticeNum:__init()
    self.msg_type = 700
    self.result = 0
end

function SCNoticeNum:Decode()
    self.result = MsgAdapter.ReadInt()
end

SCSystemMsg = SCSystemMsg or BaseClass(BaseProtocolStruct)
function SCSystemMsg:__init()
    self.msg_type = 701
end

function SCSystemMsg:Decode()
    self.send_time = MsgAdapter.ReadInt()
    MsgAdapter.ReadShort() -- self.reserve_sh
    self.min_level = MsgAdapter.ReadShort()       --等于0时代表没有限制
    self.max_level = MsgAdapter.ReadShort()         --等于0时代表没有限制
    self.msg_length = MsgAdapter.ReadShort()
    self.display_pos = MsgAdapter.ReadInt()
    self.color = MsgAdapter.ReadInt()
    self.content_type = MsgAdapter.ReadInt()
    self.spid_id = MsgAdapter.ReadStrN(64)
    self.msg = MsgAdapter.ReadStrN(self.msg_length)
end

SCForbidReason = SCForbidReason or BaseClass(BaseProtocolStruct)
function SCForbidReason:__init()
    self.msg_type = 705
end

function SCForbidReason:Decode()
    self.forbid_time_log = MsgAdapter.ReadInt()
end