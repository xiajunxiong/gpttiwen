CSRoleTopLevelReq = CSRoleTopLevelReq or BaseClass(BaseProtocolStruct)
function CSRoleTopLevelReq:__init()
    self.msg_type = 2860
end
function CSRoleTopLevelReq:Encode()
    -- 0 点击弹窗 1领取奖励 p1 level 2成就 p1 seq
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.op_type)
    MsgAdapter.WriteShort(self.param1)
end
SCRoleTopLevelAllInfo = SCRoleTopLevelAllInfo or BaseClass(BaseProtocolStruct)
function SCRoleTopLevelAllInfo:__init()
    self.msg_type = 2861
end
function SCRoleTopLevelAllInfo:Decode()
    --刷新后首次弹出 0未弹 1已弹
    self.is_refresh_first_notice_top_level_flag = MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    self.top_level = MsgAdapter.ReadInt()
    self.next_top_level_refresh_timestamp = MsgAdapter.ReadUInt()
    -- 999/32 1024/32
    self.role_top_level_reward_fetch_flag = MsgAdapter.IntFlag(32,true)
    self.top_level_achieve_flag = MsgAdapter.IntFlag(1, true)
end
SCRoleTopLevelNotice = SCRoleTopLevelNotice or BaseClass(BaseProtocolStruct)
function SCRoleTopLevelNotice:__init()
    self.msg_type = 2862
end
function SCRoleTopLevelNotice:Decode()
    -- 0 弹窗 p1 0未弹 1已弹
    -- 1 领取奖励 p1 0未领 1已领 p2 top_level 2成就已领
    self.notice_type = MsgAdapter.ReadShort()
    self.param1 = MsgAdapter.ReadShort()
    self.param2 = MsgAdapter.ReadInt()
end
SCRoleTopLevelAOINotice = SCRoleTopLevelAOINotice or BaseClass(BaseProtocolStruct)
function SCRoleTopLevelAOINotice:__init()
    self.msg_type = 2863
end
function SCRoleTopLevelAOINotice:Decode()
    self.obj_id = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    self.top_level = MsgAdapter.ReadInt()
end