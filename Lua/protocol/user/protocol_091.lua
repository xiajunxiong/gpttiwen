
-- 创建队伍
CSCreateTeam = CSCreateTeam or BaseClass(BaseProtocolStruct)
function CSCreateTeam:__init()
	self.msg_type = 9150
end

function CSCreateTeam:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteShort(self.limit_level_low)
	MsgAdapter.WriteShort(self.limit_level_high)
	MsgAdapter.WriteShort(self.team_type)
	MsgAdapter.WriteChar(self.is_cross)
	MsgAdapter.WriteChar(self.is_match)
	MsgAdapter.WriteChar(self.allow_red_join) --是否允许红名玩家加入(1是,0否)
	MsgAdapter.WriteChar(0)
	MsgAdapter.WriteShort(0)
end

-- 解散队伍
CSDismissTeam = CSDismissTeam or BaseClass(BaseProtocolStruct)
function CSDismissTeam:__init()
	self.msg_type = 9155
end

function CSDismissTeam:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

-- 获取组队列表
CSUnfullTeamListByTypeReq = CSUnfullTeamListByTypeReq or BaseClass(BaseProtocolStruct)
function CSUnfullTeamListByTypeReq:__init()
	self.msg_type = 9165
end

function CSUnfullTeamListByTypeReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.team_type)
end

-- 邀请加入队伍
CSInviteJoinReq = CSInviteJoinReq or BaseClass(BaseProtocolStruct)
function CSInviteJoinReq:__init()
	self.msg_type = 9151
end

function CSInviteJoinReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.uid)
end

-- 退队
CSExitTeamReq = CSExitTeamReq or BaseClass(BaseProtocolStruct)
function CSExitTeamReq:__init()
	self.msg_type = 9158
end

function CSExitTeamReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

-- 将成员踢出队伍
CSKickOutOfTeamReq = CSKickOutOfTeamReq or BaseClass(BaseProtocolStruct)
function CSKickOutOfTeamReq:__init()
	self.msg_type = 9156
end

function CSKickOutOfTeamReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.kick_index)
end

-- 申请入队
CSJoinTeamReq = CSJoinTeamReq or BaseClass(BaseProtocolStruct)
function CSJoinTeamReq:__init()
	self.msg_type = 9154
end

function CSJoinTeamReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.team_index)
	MsgAdapter.WriteInt(self.is_cross)
end

-- 答复入队邀请
CSInviteJoinRet = CSInviteJoinRet or BaseClass(BaseProtocolStruct)
function CSInviteJoinRet:__init()
	self.msg_type = 9152
end

function CSInviteJoinRet:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.uid)
	MsgAdapter.WriteInt(self.is_agree)
end

-- 审批入队申请
CSJoinTeamReqRet = CSJoinTeamReqRet or BaseClass(BaseProtocolStruct)
function CSJoinTeamReqRet:__init()
	self.msg_type = 9153
end

function CSJoinTeamReqRet:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.uid)
	MsgAdapter.WriteInt(self.is_agree)
end

-- 换队长
CSChangeTeamLeader = CSChangeTeamLeader or BaseClass(BaseProtocolStruct)
function CSChangeTeamLeader:__init()
	self.msg_type = 9157
end

function CSChangeTeamLeader:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.member_index)
end

-- 改变队伍加入是否是要队长通过
CSChangeMustCheck = CSChangeMustCheck or BaseClass(BaseProtocolStruct)
function CSChangeMustCheck:__init()
	self.msg_type = 9160
end

function CSChangeMustCheck:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.check_type)
end

-- 匹配请求
CSTeamMatchOp = CSTeamMatchOp or BaseClass(BaseProtocolStruct)
function CSTeamMatchOp:__init()
	self.msg_type = 9170
end

function CSTeamMatchOp:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.team_type)
	MsgAdapter.WriteShort(self.op_type)		-- 操作类型 0:取消匹配 1:开始匹配
	MsgAdapter.WriteChar(self.is_cross)		-- 是否跨服匹配 0:否 1:是
	MsgAdapter.WriteChar(self.is_auto_create)	-- 一定时间匹配不到人是否自动创建队伍(0否1是)
end

-- 修改队伍信息
CSChangeTeamLimit = CSChangeTeamLimit or BaseClass(BaseProtocolStruct)
function CSChangeTeamLimit:__init()
	self.msg_type = 9163
end

function CSChangeTeamLimit:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteShort(self.limit_level_low)
	MsgAdapter.WriteShort(self.limit_level_high)
	MsgAdapter.WriteInt(self.team_type)
	MsgAdapter.WriteChar(self.allow_red_join) --是否允许红名玩家加入(1是,0否)
	MsgAdapter.WriteChar(0)
	MsgAdapter.WriteShort(0)
end

-- 暂离
CSTemporarilyPartReq = CSTemporarilyPartReq or BaseClass(BaseProtocolStruct)
function CSTemporarilyPartReq:__init()
	self.msg_type = 9171
end

function CSTemporarilyPartReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

local function ReadTeamInfo()
	local t = {}

	t.team_index = MsgAdapter.ReadInt()
	t.limit_level_low = MsgAdapter.ReadShort()
    t.limit_level_high = MsgAdapter.ReadShort()
    t.member_count = MsgAdapter.ReadInt()
	t.team_type = MsgAdapter.ReadShort()
	t.is_cross = MsgAdapter.ReadShort()
	t.guild_id = MsgAdapter.ReadInt()
	t.create_team_timestamp = MsgAdapter.ReadUInt()
    
    t.member_list = {}
	for i = 1, TeamConfig.TEAM_MEMBER_MAX do
		local data = {}
		data.uid = MsgAdapter.ReadInt()
		data.level = MsgAdapter.ReadInt()
		data.prof = MsgAdapter.ReadInt()
		data.avatar_type = MsgAdapter.ReadShort()
		data.avatar_id = MsgAdapter.ReadShort()--人物头像
		data.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        t.member_list[i] = data
    end

    return t
end

-- 请求队伍列表回复
SCTeamListAck = SCTeamListAck or BaseClass(BaseProtocolStruct)
function SCTeamListAck:__init()
	self.msg_type = 9104
end

function SCTeamListAck:Decode()
    self.count = MsgAdapter.ReadInt()
    self.team_list = {}
    for i = 1, self.count do
        self.team_list[i] = ReadTeamInfo()
    end
end

-- 发送队伍信息给玩家
SCTeamInfo = SCTeamInfo or BaseClass(BaseProtocolStruct)
function SCTeamInfo:__init()
	self.msg_type = 9100
end

function SCTeamInfo:Decode()
    local t = {}
	t.team_index = MsgAdapter.ReadInt()
    t.limit_level_low = MsgAdapter.ReadShort()
    t.limit_level_high = MsgAdapter.ReadShort()
	t.team_type = MsgAdapter.ReadInt()
	t.check_type = MsgAdapter.ReadShort()
	t.is_cross = MsgAdapter.ReadChar()
	t.allow_red_join = MsgAdapter.ReadChar() --0不允许红名加入，1允许
	
	t.leader_index = MsgAdapter.ReadInt()
    t.member_count = MsgAdapter.ReadInt()
    t.member_list = {}
    for i = 1, t.member_count do
		local data = {}
		data.member_index = MsgAdapter.ReadInt()	-- real_index(操作用)
		data.member_index_s = MsgAdapter.ReadInt()	-- show_index(展示用)
		data.is_partner = MsgAdapter.ReadInt()		-- 是否为伙伴
		data.level = MsgAdapter.ReadInt()			-- 等级
		data.uid = MsgAdapter.ReadInt()				-- 角色uid 或者 伙伴id
		data.prof = MsgAdapter.ReadInt()			-- 职业
		data.is_follow = MsgAdapter.ReadChar()		-- 跟随
		data.is_online = MsgAdapter.ReadChar()		-- 在线
		data.pos_index = MsgAdapter.ReadShort()		-- 组队站位
		
		data.cur_hp = MsgAdapter.ReadInt()
		data.max_hp = MsgAdapter.ReadInt()
		data.cur_mp = MsgAdapter.ReadInt()
		data.max_mp = MsgAdapter.ReadInt()
		data.hp_per = data.cur_hp / data.max_hp
		data.mp_per = data.cur_mp / data.max_mp
		data.skin_id = MsgAdapter.ReadInt()

		data.pet_id = MsgAdapter.ReadInt()
		data.pet_level = MsgAdapter.ReadInt()
		data.pet_str_level = MsgAdapter.ReadInt()
		data.pet_cur_hp = MsgAdapter.ReadInt()
		data.pet_max_hp = MsgAdapter.ReadInt()
		data.pet_cur_mp = MsgAdapter.ReadInt()
		data.pet_max_mp = MsgAdapter.ReadInt()
		data.pet_skin_id = MsgAdapter.ReadInt()

		local sub_partner = {
			id = MsgAdapter.ReadInt(),
			cur_hp = MsgAdapter.ReadInt(),
			max_hp = MsgAdapter.ReadInt(),
			cur_mp = MsgAdapter.ReadInt(),
			max_mp = MsgAdapter.ReadInt(),
		}
		sub_partner.hp_per = sub_partner.cur_hp / sub_partner.max_hp
		sub_partner.mp_per = sub_partner.cur_mp / sub_partner.max_mp
		data.sub_partner = sub_partner

		data.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		data.appearance = Appearance()
        t.member_list[i] = data
    end

    self.info = t
end

-- 通知你已经没有队伍了 被踢了或者自己退了
SCOutOfTeam = SCOutOfTeam or BaseClass(BaseProtocolStruct)
function SCOutOfTeam:__init()
	self.msg_type = 9101
end

function SCOutOfTeam:Decode()
end

-- 你收到了组队邀请
SCInviteNotice = SCInviteNotice or BaseClass(BaseProtocolStruct)
function SCInviteNotice:__init()
	self.msg_type = 9102
end

function SCInviteNotice:Decode()
	self.team_index = MsgAdapter.ReadInt()
end

-- 你收到了入队申请
SCJoinReq = SCJoinReq or BaseClass(BaseProtocolStruct)
function SCJoinReq:__init()
	self.msg_type = 9103
end

function SCJoinReq:Decode()
    self.req_index = MsgAdapter.ReadInt()
    
    local t = {}
    t.uid = MsgAdapter.ReadInt()
    t.level = MsgAdapter.ReadInt()
    t.prof = MsgAdapter.ReadInt()
    t.avatar_timestamp = MsgAdapter.ReadLL()
	t.req_timestamp = MsgAdapter.ReadUInt()
	t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	t.avatar_type = MsgAdapter.ReadShort()
	t.avatar_id = MsgAdapter.ReadShort()--人物头像

    self.join_info = t
end

SCTeamMatchInfo = SCTeamMatchInfo or BaseClass(BaseProtocolStruct)
function SCTeamMatchInfo:__init()
	self.msg_type = 9169
end

function SCTeamMatchInfo:Decode()
	self.is_match = MsgAdapter.ReadInt()
end

-- 组队邀请
CSTeamInvitationReq = CSTeamInvitationReq or BaseClass(BaseProtocolStruct)
function CSTeamInvitationReq:__init()
	self.msg_type = 9172
end

function CSTeamInvitationReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.uid)
	MsgAdapter.WriteInt(self.target) --target 0 默认组队 1 结义组队 2 情缘组队
end

-- 组队邀请通知
SCTeamInvitationNotice = SCTeamInvitationNotice or BaseClass(BaseProtocolStruct)
function SCTeamInvitationNotice:__init()
	self.msg_type = 9173
end

function SCTeamInvitationNotice:Decode()
	self.info = {
		notice_type = MsgAdapter.ReadInt(),
		is_cross = MsgAdapter.ReadShort(),
		target = MsgAdapter.ReadShort(), --target 0 默认组队 1 结义组队 2 情缘组队
		add_chivalrous_value = MsgAdapter.ReadInt(),
		team_index = MsgAdapter.ReadInt(),
		team_type = MsgAdapter.ReadInt(),
		team_member_num = MsgAdapter.ReadInt(),
		leader = {								-- 邀请者信息
			uid = MsgAdapter.ReadInt(),
			name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
			level = MsgAdapter.ReadInt(),
			prof = MsgAdapter.ReadInt(),
			avatar_type = MsgAdapter.ReadShort(),
			avatar_id = MsgAdapter.ReadShort()
		}
	}
end

-- 组队邀请回复
CSTeamInvationRet = CSTeamInvationRet or BaseClass(BaseProtocolStruct)
function CSTeamInvationRet:__init()
	self.msg_type = 9174
end

function CSTeamInvationRet:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.uid)
	MsgAdapter.WriteInt(self.is_accpet)			-- 0拒绝 1接受
	MsgAdapter.WriteInt(self.is_from_cross)		-- 9173下发 原样发回去
	MsgAdapter.WriteInt(self.team_index)		-- 9173下发 原样发回去
end

-- 招募
CSTeamRecruit = CSTeamRecruit or BaseClass(BaseProtocolStruct)
function CSTeamRecruit:__init()
	self.msg_type = 9175
end

function CSTeamRecruit:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

-- 进入副本请求转发
SCTeamEnterFbRoute = SCTeamEnterFbRoute or BaseClass(BaseProtocolStruct)
function SCTeamEnterFbRoute:__init()
	self.msg_type = 9108
end

function SCTeamEnterFbRoute:Decode()
	local t = {}
	t.enter_type = MsgAdapter.ReadInt()
	t.enter_type_param = MsgAdapter.ReadInt()
	t.agree_enter_end_time = MsgAdapter.ReadUInt()
	t.param1 = MsgAdapter.ReadInt()
	t.param2 = MsgAdapter.ReadInt()
	t.param3 = MsgAdapter.ReadInt()
	self.enter_fb_info = t
end

-- 进入副本队员确定信息
SCTeamEnterFbInfo = SCTeamEnterFbInfo or BaseClass(BaseProtocolStruct)
function SCTeamEnterFbInfo:__init()
	self.msg_type = 9109
end

function SCTeamEnterFbInfo:Decode()
	local t = {}
	t.member_index = MsgAdapter.ReadShort()
	t.is_determine = MsgAdapter.ReadShort()
	self.enter_fb_team_info = t
end

-- 回复进入副本请求
CSTeamEnterFbRet = CSTeamEnterFbRet or BaseClass(BaseProtocolStruct)
function CSTeamEnterFbRet:__init()
	self.msg_type = 9176
end

function CSTeamEnterFbRet:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.is_determine)
end

-- 队伍切换到跨服
CSTeamSwitchToCross = CSTeamSwitchToCross or BaseClass(BaseProtocolStruct)
function CSTeamSwitchToCross:__init()
	self.msg_type = 9177
end

function CSTeamSwitchToCross:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

-- 组队频道信息
SCTeamChannelInfo = SCTeamChannelInfo or BaseClass(BaseProtocolStruct)
function SCTeamChannelInfo:__init()
	self.msg_type = 9178
end

function SCTeamChannelInfo:Decode()
	self.reason = MsgAdapter.ReadShort()
	self.count = MsgAdapter.ReadShort()
	self.team_list = {}
	for i = 1, self.count do
        self.team_list[i] = ReadTeamInfo()
    end
end

-- 交换站位
CSTeamSwitchPosition = CSTeamSwitchPosition or BaseClass(BaseProtocolStruct)
function CSTeamSwitchPosition:__init()
	self.msg_type = 9179
end

function CSTeamSwitchPosition:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteShort(self.pos1)
	MsgAdapter.WriteShort(self.pos2)
end

-- 成员外观变化通知
SCTeamMemberAppearanceChange = SCTeamMemberAppearanceChange or BaseClass(BaseProtocolStruct)
function SCTeamMemberAppearanceChange:__init()
	self.msg_type = 9180
end

function SCTeamMemberAppearanceChange:Decode()
	self.member_index = MsgAdapter.ReadInt()
	self.appearance = Appearance()
end

-- 队伍审批列表删除一个玩家的申请
SCTeamRemoveApply = SCTeamRemoveApply or BaseClass(BaseProtocolStruct)
function SCTeamRemoveApply:__init()
	self.msg_type = 9181
end

function SCTeamRemoveApply:Decode()
	self.remove_uid = MsgAdapter.ReadInt()
end

-- 队长改变通知附近玩家
SCTeamLeaderChangeNoticeArea = SCTeamLeaderChangeNoticeArea or BaseClass(BaseProtocolStruct)
function SCTeamLeaderChangeNoticeArea:__init()
	self.msg_type = 9111
end

function SCTeamLeaderChangeNoticeArea:Decode()
	self.obj_id = MsgAdapter.ReadUShort()
	self.is_leader = MsgAdapter.ReadShort()
end

-- 同步成员血量魔量
SCUpdateMemberHPMP = SCUpdateMemberHPMP or BaseClass(BaseProtocolStruct)
function SCUpdateMemberHPMP:__init()
	self.msg_type = 9186
end

function SCUpdateMemberHPMP:Decode()
	self.role_info = {
		uid = MsgAdapter.ReadInt(),
		is_partner = MsgAdapter.ReadInt(),
		cur_hp = MsgAdapter.ReadInt(),
		max_hp = MsgAdapter.ReadInt(),
		cur_mp = MsgAdapter.ReadInt(),
		max_mp = MsgAdapter.ReadInt(),
		skin_id = MsgAdapter.ReadInt(),
		level =  MsgAdapter.ReadInt(),
	}
	self.role_info.hp_per = self.role_info.cur_hp / self.role_info.max_hp
	self.role_info.mp_per = self.role_info.cur_mp / self.role_info.max_mp
end

-- 队员出战宠物信息
SCUpdateMemberPetHPMP = SCUpdateMemberPetHPMP or BaseClass(BaseProtocolStruct)
function SCUpdateMemberPetHPMP:__init()
	self.msg_type = 9189
end

function SCUpdateMemberPetHPMP:Decode()
	self.pet_info = {
		uid = MsgAdapter.ReadInt(),
		pet_id = MsgAdapter.ReadInt(),
		pet_level = MsgAdapter.ReadInt(),
		pet_str_level = MsgAdapter.ReadInt(),
		pet_cur_hp = MsgAdapter.ReadInt(),
		pet_max_hp = MsgAdapter.ReadInt(),
		pet_cur_mp = MsgAdapter.ReadInt(),
		pet_max_mp = MsgAdapter.ReadInt(),
		pet_skin_id = MsgAdapter.ReadInt(),
	}
end

-- 队员帮助加血加蓝通知
SCTeamMemberAddHpMpNotice = SCTeamMemberAddHpMpNotice or BaseClass(BaseProtocolStruct)
function SCTeamMemberAddHpMpNotice:__init()
	self.msg_type = 9190
end

function SCTeamMemberAddHpMpNotice:Decode()
	self.info = {
		from_team_member_index = MsgAdapter.ReadShort(),
		is_to_use_pet = MsgAdapter.ReadShort(),
		partner_id = MsgAdapter.ReadInt(),
		item_id = MsgAdapter.ReadShort(),
		use_gold_recovery = MsgAdapter.ReadShort(),
	}
end

-- 投票开始通知
SCTeamVoteStart = SCTeamVoteStart or BaseClass(BaseProtocolStruct)
function SCTeamVoteStart:__init()
	self.msg_type = 9183
end

function SCTeamVoteStart:Decode()
	self.vote_end_timestamp = MsgAdapter.ReadUInt()
	self.campaigner_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
end

-- 发起投票
CSTeamLaunchLeaderVote = CSTeamLaunchLeaderVote or BaseClass(BaseProtocolStruct)
function CSTeamLaunchLeaderVote:__init()
	self.msg_type = 9184
end

function CSTeamLaunchLeaderVote:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

-- 投票
CSTeamVote = CSTeamVote or BaseClass(BaseProtocolStruct)
function CSTeamVote:__init()
	self.msg_type = 9185
end

function CSTeamVote:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.is_yes)
end

-- 队长可视NPC操作
CSTeamLeaderNpcReq = CSTeamLeaderNpcReq or BaseClass(BaseProtocolStruct)
function CSTeamLeaderNpcReq:__init()
	self.msg_type = 9187
end

function CSTeamLeaderNpcReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.oper_type)
	MsgAdapter.WriteInt(#self.npc_list)
	for i = 1, #self.npc_list do
		MsgAdapter.WriteInt(self.npc_list[i])
	end
end

-- 队长可视NPC下发
SCTeamLeaderNpcInfo = SCTeamLeaderNpcInfo or BaseClass(BaseProtocolStruct)
function SCTeamLeaderNpcInfo:__init()
	self.msg_type = 9188
end

function SCTeamLeaderNpcInfo:Decode()
	self.oper_type = MsgAdapter.ReadInt()
	local npc_count = MsgAdapter.ReadInt()
	self.npc_list = {}
	for i = 1, npc_count do
		self.npc_list[i] = MsgAdapter.ReadInt()
	end
end

-- 队伍简易信息更变通知
SCTeamSimpleInfoChangeNotice = SCTeamSimpleInfoChangeNotice or BaseClass(BaseProtocolStruct)
function SCTeamSimpleInfoChangeNotice:__init()
	self.msg_type = 9191
end

function SCTeamSimpleInfoChangeNotice:Decode()
	self.info = {
		limit_level_low = MsgAdapter.ReadShort(),
		limit_level_high = MsgAdapter.ReadShort(),
		team_type = MsgAdapter.ReadInt(),
		check_type = MsgAdapter.ReadShort(),
		is_cross = MsgAdapter.ReadChar(),
		allow_red_join = MsgAdapter.ReadChar(),
	}
end

-- 队伍简易信息更变通知
SCTeamMemberBaseInfoChangeNotice = SCTeamMemberBaseInfoChangeNotice or BaseClass(BaseProtocolStruct)
function SCTeamMemberBaseInfoChangeNotice:__init()
	self.msg_type = 9192
end

function SCTeamMemberBaseInfoChangeNotice:Decode()
	self.member_info = {
		level = MsgAdapter.ReadInt(),
		uid = MsgAdapter.ReadInt(),
		prof = MsgAdapter.ReadInt(),
		name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
		appearance = Appearance(),
	}
end

-- 队友不符合条件通知
SCTeamMemberIncompatibleNotice = SCTeamMemberIncompatibleNotice or BaseClass(BaseProtocolStruct)
function SCTeamMemberIncompatibleNotice:__init()
	self.msg_type = 9196
end

function SCTeamMemberIncompatibleNotice:Decode()
	self.notice_type = MsgAdapter.ReadInt()
	self.team_member_index = MsgAdapter.ReadInt()
	self.param1 = MsgAdapter.ReadInt()
	self.param2 = MsgAdapter.ReadInt()
end

-- 召回请求
CSTeamLeaderSummonMember = CSTeamLeaderSummonMember or BaseClass(BaseProtocolStruct)
function CSTeamLeaderSummonMember:__init()
	self.msg_type = 9193
end

function CSTeamLeaderSummonMember:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.summon_role_id)
end

-- 队员通知:队长需要召回你
SCTeamLeaderSummonMemberNotice = SCTeamLeaderSummonMemberNotice or BaseClass(BaseProtocolStruct)
function SCTeamLeaderSummonMemberNotice:__init()
	self.msg_type = 9194
end

function SCTeamLeaderSummonMemberNotice:Decode()
end

-- 队员回复召回
CSTeamMemberApplyLeaderSummon = CSTeamMemberApplyLeaderSummon or BaseClass(BaseProtocolStruct)
function CSTeamMemberApplyLeaderSummon:__init()
	self.msg_type = 9195
end

function CSTeamMemberApplyLeaderSummon:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.is_return)
end

-- 队长请求检查队员特有道具信息
CSLeaderCheckMemberItem = CSLeaderCheckMemberItem or BaseClass(BaseProtocolStruct)
function CSLeaderCheckMemberItem:__init()
	self.msg_type = 9199
end

function CSLeaderCheckMemberItem:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.item_id)
	MsgAdapter.WriteInt(self.item_num)
end

SCLeaderCheckMemberItemRet = SCLeaderCheckMemberItemRet or BaseClass(BaseProtocolStruct)
function SCLeaderCheckMemberItemRet:__init()
	self.msg_type = 9200
end

function SCLeaderCheckMemberItemRet:Decode()
	self.check_num = MsgAdapter.ReadInt()
	self.member_item_check = {}
	for i = 1,5 do--self.check_num do
		local vo  = {}
		vo.member_idx = MsgAdapter.ReadInt()
		vo.item_num = MsgAdapter.ReadInt()

		table.insert( self.member_item_check , vo)
	end 
end

-- 侠义值队伍列表返回
SCChivalrousTeamList = SCChivalrousTeamList or BaseClass(BaseProtocolStruct)
function SCChivalrousTeamList:__init()
	self.msg_type = 9112
end

function SCChivalrousTeamList:Decode()
	self.count = MsgAdapter.ReadInt()
    self.team_list = {}
    for i = 1, self.count do
		local add_chivalrous_value = MsgAdapter.ReadInt()	-- 侠义值
        self.team_list[i] = ReadTeamInfo()
		self.team_list[i].add_chivalrous_value = add_chivalrous_value
    end
end

-- 请求侠义值队伍列表
CSChivalrousTeamListReq = CSChivalrousTeamListReq or BaseClass(BaseProtocolStruct)
function CSChivalrousTeamListReq:__init()
	self.msg_type = 9201
end

function CSChivalrousTeamListReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

-- 请求侠义值队伍列表
CSReqJoinTeamInfo = CSReqJoinTeamInfo or BaseClass(BaseProtocolStruct)
function CSReqJoinTeamInfo:__init()
	self.msg_type = 9202
end

function CSReqJoinTeamInfo:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.is_from_cross)
	MsgAdapter.WriteInt(self.team_index)
	MsgAdapter.WriteInt(self.uid)
end

-- 历史侠义值下发
SCRoleHistoryChivalrous = SCRoleHistoryChivalrous or BaseClass(BaseProtocolStruct)
function SCRoleHistoryChivalrous:__init()
	self.msg_type = 2066
end

function SCRoleHistoryChivalrous:Decode()
	self.info = {
		history_chivalrous = MsgAdapter.ReadInt(),		-- 历史累计获得的侠义值
		chivalrous_reward_flag = MsgAdapter.ReadInt(),	-- 已领取奖励标记(二进制)
	}
end

-- 历史侠义值奖励领取请求
CSRoleHistoryChivalrousRewardFetch = CSRoleHistoryChivalrousRewardFetch or BaseClass(BaseProtocolStruct)
function CSRoleHistoryChivalrousRewardFetch:__init()
	self.msg_type = 2067
end

function CSRoleHistoryChivalrousRewardFetch:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.fetch_seq)
end