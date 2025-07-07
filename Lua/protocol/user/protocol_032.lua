
-- 世界答题下发
SCWorldQuestionNotice = SCWorldQuestionNotice or BaseClass(BaseProtocolStruct)
function SCWorldQuestionNotice:__init()
    self.msg_type = 3205
end

function SCWorldQuestionNotice:Decode()
    self.info = {
        now_answer_subject = MsgAdapter.ReadInt(),          -- 题目id
        answer_tamp = MsgAdapter.ReadLL(),                  -- 结束回答时间戳
        show_answer_tamp = MsgAdapter.ReadLL(),             -- 展示答案时间戳
        next_subject_refresh_tamp = MsgAdapter.ReadLL(),    -- 下一次题目刷新时间
        answer_end_tamp = MsgAdapter.ReadLL(),              -- 结束时间戳
        is_broadcast = MsgAdapter.ReadChar(),			    -- 是否广播
		answer_ret = MsgAdapter.ReadChar(),				    -- 是否答题正确
        level_limit = MsgAdapter.ReadShort(),               -- 等级限制 达到该等级的玩家才给显示
        role_answer_id = MsgAdapter.ReadInt(),              -- 玩家答案下标[1-4]
    }
end

SCWorldBoss2RoleInfo = SCWorldBoss2RoleInfo or BaseClass(BaseProtocolStruct)
function SCWorldBoss2RoleInfo:__init()
    self.msg_type = 3199
end
function SCWorldBoss2RoleInfo:Decode()
    self.play_times = MsgAdapter.ReadInt()
    self.shanghai = MsgAdapter.ReadInt()
    self.zhiliao = MsgAdapter.ReadInt()
end

SCWorldBoss2RankInfo = SCWorldBoss2RankInfo or BaseClass(BaseProtocolStruct)
function SCWorldBoss2RankInfo:__init()
    self.msg_type = 3198
end
function SCWorldBoss2RankInfo:Decode()
    self.type = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.hurt_rank = {}
    for i = 1, self.count, 1 do
        self.hurt_rank[i] = self:RankItemInfo(i, self.type)
    end
end
function SCWorldBoss2RankInfo:RankItemInfo(rank, type)
    return {
        rank = rank,
        type = type,
        role_id = MsgAdapter.ReadInt(),
        name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        level = MsgAdapter.ReadInt(),
        prof = MsgAdapter.ReadInt(),
        value = MsgAdapter.ReadLL()
    }
end

local function MsgMatchUserInfo()
    return{
        uid = MsgAdapter.ReadInt(),
        prof = MsgAdapter.ReadInt(),
        level = MsgAdapter.ReadShort(),
        avatar_type = MsgAdapter.ReadShort(),
        avatar_id = MsgAdapter.ReadShort(),--人物头像
        MsgAdapter.ReadShort(),
        name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    }
end
--小队竞技 开始匹配通知 business_type 1 小队竞技 2 欢乐寻宠 3 龙舟
SCStartMatch = SCStartMatch or BaseClass(BaseProtocolStruct)
function SCStartMatch:__init()
	self.msg_type = 3230
end

function SCStartMatch:Decode()
    self.business_type = MsgAdapter.ReadShort()
    self.user_list = {}
    for i=1,MsgAdapter.ReadShort() do
        self.user_list[i] = MsgMatchUserInfo()
    end
end
--小队竞技 匹配信息更新（新增队友或队友离开匹配时）
SCMatchInfoUpdate = SCMatchInfoUpdate or BaseClass(BaseProtocolStruct)
function SCMatchInfoUpdate:__init()
	self.msg_type = 3231
end

function SCMatchInfoUpdate:Decode()
    self.business_type = MsgAdapter.ReadShort()
    self.user_list = {}
    for i=1,MsgAdapter.ReadShort() do
        self.user_list[i] = MsgMatchUserInfo()
    end
end
--小队竞技 玩家停止匹配通知
SCStopMatch = SCStopMatch or BaseClass(BaseProtocolStruct)
function SCStopMatch:__init()
	self.msg_type = 3232
end

function SCStopMatch:Decode()
    self.business_type = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
end
--小队竞技 匹配到对手通知
SCMatchEnemy = SCMatchEnemy or BaseClass(BaseProtocolStruct)
function SCMatchEnemy:__init()
	self.msg_type = 3233
end

function SCMatchEnemy:Decode()
    self.business_type = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    self.left_user_num = MsgAdapter.ReadShort()
    self.right_user_num = MsgAdapter.ReadShort()
    self.left_user_list = {}
    self.right_user_list = {}
    for i=1,self.left_user_num do
        self.left_user_list[i] = MsgMatchUserInfo()
    end
    for i=1,self.right_user_num do
        self.right_user_list[i] = MsgMatchUserInfo()
    end
end

-- 剧情选项记录请求
CSTaskChosedReq = CSTaskChosedReq or BaseClass(BaseProtocolStruct)
function CSTaskChosedReq:__init()
	self.msg_type = 3234
end

function CSTaskChosedReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteUShort(self.req_type)
	MsgAdapter.WriteUShort(self.param1)
	MsgAdapter.WriteUShort(self.param2)
	MsgAdapter.WriteUShort(self.param3)
end

-- 剧情选项选择记录
SCMainTaskChosedTaskList = SCMainTaskChosedTaskList or BaseClass(BaseProtocolStruct)
function SCMainTaskChosedTaskList:__init()
	self.msg_type = 3235
end

function SCMainTaskChosedTaskList:Decode()
    self.count = MsgAdapter.ReadInt()
    self.options = {}
    for i = 1, self.count do
        local index = MsgAdapter.ReadUShort()
        self.options[index] = MsgAdapter.ReadUShort()
    end
end

SCTaskChainTurnTabInfo = SCTaskChainTurnTabInfo or BaseClass(BaseProtocolStruct)
function SCTaskChainTurnTabInfo:__init()
    self.msg_type = 3236
end
function SCTaskChainTurnTabInfo:Decode()
    self.bounty_task_info = {
        reward_idx = MsgAdapter.ReadShort(),
        group_id = MsgAdapter.ReadShort(),
    }
end

-- 获取群聊列表
CSGetRoleGroupList = CSGetRoleGroupList or BaseClass(BaseProtocolStruct)
function CSGetRoleGroupList:__init()
	self.msg_type = 3267
end

function CSGetRoleGroupList:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

-- 获取群聊详细信息以及成员信息
CSGetGroupInfo = CSGetGroupInfo or BaseClass(BaseProtocolStruct)
function CSGetGroupInfo:__init()
	self.msg_type = 3268
end

function CSGetGroupInfo:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt2(self.group_id)
end

local function ReadRoleGroupInfo()
    local t = {
        group_id = MsgAdapter.ReadInt2(),
        is_cross = MsgAdapter.ReadInt(),
        group_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        master = MsgAdapter.ReadInt(),
        avatars = {}
    }
    for i = 1, SocietyConfig.CHAT_GROUP_AVATAR_NUM do
        t.avatars[i] = MsgAdapter.ReadInt()
    end
    t.member_count = MsgAdapter.ReadInt()
    return t
end

local function ReadChatGroupInfo()
    local t = {
        group_id = MsgAdapter.ReadInt2(),
        group_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        group_notice = MsgAdapter.ReadStrN(PROTC_GROUP_NOTICE_LEN),
        is_cross = MsgAdapter.ReadInt(),
        creator_uid = MsgAdapter.ReadInt(),
        member_count = MsgAdapter.ReadInt(),
        member_list = {},
    }
    t.group_key = SocietyData.GroupId2Key(t.group_id)
    t.online_count = 0
    for i = 1, t.member_count do
        local role = {
            uid = MsgAdapter.ReadInt(),
            avatar_type = MsgAdapter.ReadInt(),
            name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
            prof = MsgAdapter.ReadInt(),
            level = MsgAdapter.ReadInt(),
            member_logout_tamp = MsgAdapter.ReadUInt(),
        }
        role.is_master = t.creator_uid == role.uid
        if 0 == role.member_logout_tamp then
            t.online_count = t.online_count + 1
        end
        t.member_list[role.uid] = role
    end
    return t
end

-- 所有群组的缩略信息
SCRoleGroupInfo = SCRoleGroupInfo or BaseClass(BaseProtocolStruct)
function SCRoleGroupInfo:__init()
	self.msg_type = 3241
end

function SCRoleGroupInfo:Decode()
    self.reason = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.groups = {}
    for i = 1, self.count do
        self.groups[i] = ReadRoleGroupInfo()
    end
end

-- 单个群组的详细信息
SCGroupAllInfo = SCGroupAllInfo or BaseClass(BaseProtocolStruct)
function SCGroupAllInfo:__init()
	self.msg_type = 3242
end

function SCGroupAllInfo:Decode()
    self.info = ReadChatGroupInfo()
end

-- 发送邀请群组信息
SCRoleInviteEnterGroup = SCRoleInviteEnterGroup or BaseClass(BaseProtocolStruct)
function SCRoleInviteEnterGroup:__init()
	self.msg_type = 3243
end

function SCRoleInviteEnterGroup:Decode()
    self.info = {
        is_cross = MsgAdapter.ReadInt(),
        server_id = MsgAdapter.ReadInt(),
        invite_uid = MsgAdapter.ReadInt(),
        invite_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        group_id = MsgAdapter.ReadInt2(),
        group_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
    }
end

-- 单个群组的缩略信息
SCRoleOneGroupInfo = SCRoleOneGroupInfo or BaseClass(BaseProtocolStruct)
function SCRoleOneGroupInfo:__init()
	self.msg_type = 3244
end

function SCRoleOneGroupInfo:Decode()
    self.reason = MsgAdapter.ReadInt()
    self.info = ReadRoleGroupInfo()
end

-- 请求邀请加入群组
CSRoleInviteEnterGroup = CSRoleInviteEnterGroup or BaseClass(BaseProtocolStruct)
function CSRoleInviteEnterGroup:__init()
	self.msg_type = 3261
end

function CSRoleInviteEnterGroup:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt2(self.group_id)
	MsgAdapter.WriteInt(self.uid)
end

-- 接受邀请后加入群组(拒绝直接忽略信息)
CSEnterGroupReq = CSEnterGroupReq or BaseClass(BaseProtocolStruct)
function CSEnterGroupReq:__init()
	self.msg_type = 3262
end

function CSEnterGroupReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.is_cross)
    MsgAdapter.WriteInt(self.server_id)
    MsgAdapter.WriteInt(self.invite_uid)
	MsgAdapter.WriteInt2(self.group_id)
end

-- 创建群组
CSCreateGroupReq = CSCreateGroupReq or BaseClass(BaseProtocolStruct)
function CSCreateGroupReq:__init()
	self.msg_type = 3263
end

function CSCreateGroupReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.is_cross)
    MsgAdapter.WriteStrN(self.group_name, PROTC_ROLE_NAME_LEN)
end

-- 移除玩家出群聊
CSGroupKickMember = CSGroupKickMember or BaseClass(BaseProtocolStruct)
function CSGroupKickMember:__init()
	self.msg_type = 3264
end

function CSGroupKickMember:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt2(self.group_id)
    MsgAdapter.WriteInt(self.uid)
end

-- 解散群聊
CSGroupDissmiss = CSGroupDissmiss or BaseClass(BaseProtocolStruct)
function CSGroupDissmiss:__init()
	self.msg_type = 3265
end

function CSGroupDissmiss:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt2(self.group_id)
end

-- 离开群聊
CSGroupLeaveReq = CSGroupLeaveReq or BaseClass(BaseProtocolStruct)
function CSGroupLeaveReq:__init()
	self.msg_type = 3266
end

function CSGroupLeaveReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt2(self.group_id)
	MsgAdapter.WriteInt(self.server_id)
end

-- 修改群组宣言
CSChangeGroupNoticeReq = CSChangeGroupNoticeReq or BaseClass(BaseProtocolStruct)
function CSChangeGroupNoticeReq:__init()
	self.msg_type = 3269
end

function CSChangeGroupNoticeReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt2(self.group_id)
    MsgAdapter.WriteStrN(self.group_name, PROTC_ROLE_NAME_LEN)
    MsgAdapter.WriteStrN(self.group_notice, PROTC_GROUP_NOTICE_LEN)
end

-- 临时群组的信息下发
SCTempGroupInfo = SCTempGroupInfo or BaseClass(BaseProtocolStruct)
function SCTempGroupInfo:__init()
	self.msg_type = 3270
end

function SCTempGroupInfo:Decode()
    self.info = ReadRoleGroupInfo()
end

-- 临时群组离开通知
SCTempGroupLeaveNotice = SCTempGroupLeaveNotice or BaseClass(BaseProtocolStruct)
function SCTempGroupLeaveNotice:__init()
	self.msg_type = 3271
end

function SCTempGroupLeaveNotice:Decode()
end

-- 群组通知信息显示
SCChatGroupMsgNotice = SCChatGroupMsgNotice or BaseClass(BaseProtocolStruct)
function SCChatGroupMsgNotice:__init()
	self.msg_type = 3272
end

function SCChatGroupMsgNotice:Decode()
    self.info = {
        group_id = MsgAdapter.ReadInt2(),
        msg_length = MsgAdapter.ReadInt(),
    }
    self.info.msg_content = MsgAdapter.ReadStrN(self.info.msg_length)
end

-- 临时群组转正
CSTempChatGroupBecomeOfficialReq = CSTempChatGroupBecomeOfficialReq or BaseClass(BaseProtocolStruct)
function CSTempChatGroupBecomeOfficialReq:__init()
	self.msg_type = 3273
end

function CSTempChatGroupBecomeOfficialReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteStrN(self.group_name, PROTC_ROLE_NAME_LEN)
end

SCTempChatGroupBecomeOfficialNotice = SCTempChatGroupBecomeOfficialNotice or BaseClass(BaseProtocolStruct)
function SCTempChatGroupBecomeOfficialNotice:__init()
	self.msg_type = 3274
end

function SCTempChatGroupBecomeOfficialNotice:Decode()
    self.info = {
        group_id = MsgAdapter.ReadInt2(),
    }
end

-- 移交群主
CSHandoverChatGroupLeadership = CSHandoverChatGroupLeadership or BaseClass(BaseProtocolStruct)
function CSHandoverChatGroupLeadership:__init()
	self.msg_type = 3275
end

function CSHandoverChatGroupLeadership:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt2(self.group_id)
    MsgAdapter.WriteInt(self.new_leader_uid)
end

-- 世界boss3 操作请求
CSWorldBoss3Req = CSWorldBoss3Req or BaseClass(BaseProtocolStruct)
function CSWorldBoss3Req:__init()
	self.msg_type = 3210
end

function CSWorldBoss3Req:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
end

-- 世界boss3 信息
SCWorldBoss3Info = SCWorldBoss3Info or BaseClass(BaseProtocolStruct)
function SCWorldBoss3Info:__init()
    self.msg_type = 3211
end

function SCWorldBoss3Info:Decode()
	self.boss_info = {
		boss_npc_id = MsgAdapter.ReadInt(),
		boss_max_hp = MsgAdapter.ReadInt(),
		boss_cur_hp = MsgAdapter.ReadInt(),
	}
end

--  3212 活动结束奖励结算信息
SCWorldBoss3Result = SCWorldBoss3Result or BaseClass(BaseProtocolStruct)
function SCWorldBoss3Result:__init()
    self.msg_type = 3212
end

function SCWorldBoss3Result:Decode()
	local t = {}
	local rank_type = WorldBossConfig.RankType
	local ri = RoleData.Instance:GetRoleId()
	t[rank_type.hurt] = {}
	for i = 1, WorldBossConfig.RANK_NUM_MAX do
		local role_id = MsgAdapter.ReadInt()
		local name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		if not StringIsEmpty(name) then table.insert(t[rank_type.hurt], {role_id = role_id, name = name, is_self = ri == role_id}) end
	end
	t[rank_type.heal] = {}
	for i = 1, WorldBossConfig.RANK_NUM_MAX do
		local role_id = MsgAdapter.ReadInt()
		local name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		if not StringIsEmpty(name) then table.insert(t[rank_type.heal], {role_id = role_id, name = name, is_self = ri == role_id}) end
	end
	t[rank_type.luck] = {}
	for i = 1, WorldBossConfig.RANK_NUM_MAX do
		local role_id = MsgAdapter.ReadInt()
		local name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		if not StringIsEmpty(name) then table.insert(t[rank_type.luck], {role_id = role_id, name = name, is_self = ri == role_id}) end
	end
	self.rank_list = t
end

--  3213 世界BOSS3 位置信息
SCWorldBoss3PosInfo = SCWorldBoss3PosInfo or BaseClass(BaseProtocolStruct)
function SCWorldBoss3PosInfo:__init()
    self.msg_type = 3213
end

function SCWorldBoss3PosInfo:Decode()
	self.pos_info = {
		id = MsgAdapter.ReadInt(),
		scene_id = MsgAdapter.ReadInt(),
		x = MsgAdapter.ReadInt(),
		y = MsgAdapter.ReadInt(),
	}
end

-- 3214 世界boss 一些战斗数据统计
SCWorldBoss3BattleInfo = SCWorldBoss3BattleInfo or BaseClass(BaseProtocolStruct)
function SCWorldBoss3BattleInfo:__init()
    self.msg_type = 3214
end

function SCWorldBoss3BattleInfo:Decode()
	self.battle_info = {
		team_total_damage = MsgAdapter.ReadLL(),
		team_total_heal = MsgAdapter.ReadLL(),
	}
end

-- 3215 世界boss玩家信息
SCWorldBoss3RoleInfo = SCWorldBoss3RoleInfo or BaseClass(BaseProtocolStruct)
function SCWorldBoss3RoleInfo:__init()
    self.msg_type = 3215
end
function SCWorldBoss3RoleInfo:Decode()
    self.play_times = MsgAdapter.ReadInt()
    self.shanghai = MsgAdapter.ReadInt()
    self.zhiliao = MsgAdapter.ReadInt()
end

-- 3216 排行榜信息
SCWorldBoss3RankInfo = SCWorldBoss3RankInfo or BaseClass(BaseProtocolStruct)
function SCWorldBoss3RankInfo:__init()
    self.msg_type = 3216
end
function SCWorldBoss3RankInfo:Decode()
    self.type = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.hurt_rank = {}
    for i = 1, self.count, 1 do
        self.hurt_rank[i] = self:RankItemInfo(i, self.type)
    end
end
function SCWorldBoss3RankInfo:RankItemInfo(rank, type)
    return {
        rank = rank,
        type = type,
        role_id = MsgAdapter.ReadInt(),
        name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        level = MsgAdapter.ReadInt(),
        prof = MsgAdapter.ReadInt(),
        value = MsgAdapter.ReadLL()
    }
end