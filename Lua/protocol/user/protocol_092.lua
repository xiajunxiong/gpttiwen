--队伍是否红名，通知附近的人
SCTeamRedNameNotifyArea = SCTeamRedNameNotifyArea or BaseClass(BaseProtocolStruct)
function SCTeamRedNameNotifyArea:__init()
    self.msg_type = 9203
end

function SCTeamRedNameNotifyArea:Decode()
    self.is_red = MsgAdapter.ReadInt()
    self.team_id = MsgAdapter.ReadInt()
end

-- 队长请求弹出界面
CSTeamLeaderEjectWindowsReq = CSTeamLeaderEjectWindowsReq or BaseClass(BaseProtocolStruct)
function CSTeamLeaderEjectWindowsReq:__init()
    self.msg_type = 9204
end

function CSTeamLeaderEjectWindowsReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.windows_type) 
end

-- 广播队长弹出界面
SCTeamLeaderEjectWindowsInfo = SCTeamLeaderEjectWindowsInfo or BaseClass(BaseProtocolStruct)
function SCTeamLeaderEjectWindowsInfo:__init()
    self.msg_type = 9205
end

function SCTeamLeaderEjectWindowsInfo:Decode()
    self.windows_type = MsgAdapter.ReadInt()
end

SCTeamRoleRecruitInfo = SCTeamRoleRecruitInfo or BaseClass(BaseProtocolStruct)
function SCTeamRoleRecruitInfo:__init()
    self.msg_type = 9206
end
function SCTeamRoleRecruitInfo:Decode()
    self.info = {}
    self.info.team_id = MsgAdapter.ReadInt()
    self.info.team_type = MsgAdapter.ReadInt()
    self.info.leader_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    self.info.level_low = MsgAdapter.ReadShort()
    self.info.level_high = MsgAdapter.ReadShort()
    self.info.cur_member_num = MsgAdapter.ReadShort()
    self.info.max_member_num = MsgAdapter.ReadShort()
    LogDG(self)
end

CSTeamRoleRecruitAnswer = CSTeamRoleRecruitAnswer or BaseClass(BaseProtocolStruct)
function CSTeamRoleRecruitAnswer:__init()
    self.msg_type = 9207
end
function CSTeamRoleRecruitAnswer:Encode()
    -- enum ANSWER_TYPE
    -- {
    --   ACCEPT = 0,        // 接受
    --   REFUSE_MANUALLY = 1,  // 手动拒绝
    --   AUTO_REFUSE = 2,    // 自动拒绝
    -- };
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.answer_type)
end