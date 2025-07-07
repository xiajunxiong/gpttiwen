CSCreateGuild = CSCreateGuild or BaseClass(BaseProtocolStruct)
function CSCreateGuild:__init()
    self.msg_type = 9800
end
function CSCreateGuild:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteStrN(self.name, PROTC_GUILD_NAME_LEN)
    MsgAdapter.WriteStrN(self.notice, PROTC_TEAM_NOTICE_LEN)
    MsgAdapter.WriteStrN(self.arms, 8)
end

CSQuitGuild = CSQuitGuild or BaseClass(BaseProtocolStruct)
function CSQuitGuild:__init()
    self.msg_type = 9801
end
function CSQuitGuild:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.guild_id)
end

CSApplyJoinGuildReq = CSApplyJoinGuildReq or BaseClass(BaseProtocolStruct)
function CSApplyJoinGuildReq:__init()
    self.msg_type = 9805
end
function CSApplyJoinGuildReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.guild_id)
    MsgAdapter.WriteInt(self.one_key)
end

CSApplyJoinGuildAck = CSApplyJoinGuildAck or BaseClass(BaseProtocolStruct)
function CSApplyJoinGuildAck:__init()
    self.msg_type = 9806
end
function CSApplyJoinGuildAck:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.guild_id)
    MsgAdapter.WriteInt(self.uid)
    MsgAdapter.WriteInt(self.result) -- 0为拒绝
end

local function GuildEventhandlerParam()
    local t = {}
    t.eventhandler_type = MsgAdapter.ReadInt()
    t.eventhandler_time = MsgAdapter.ReadUInt()
    t.eventhandler_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    t.info_type = {}
    t.info_type.item_id = MsgAdapter.ReadUShort()
    local param_length = MsgAdapter.ReadShort()
    if t.info_type.item_id ~= 0 and param_length > 0 then
        t.info_type.param = ItemDataParam(t.info_type.item_id, param_length)
    end
    return t
end
SCGuildBaseInfo = SCGuildBaseInfo or BaseClass(BaseProtocolStruct)
function SCGuildBaseInfo:__init()
    self.msg_type = 9803
end
function SCGuildBaseInfo:Decode()
    self.patriarch_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    self.patriarch_last_login_time = MsgAdapter.ReadUInt()
    self.patriarch_tan_he_start_time = MsgAdapter.ReadUInt()
    self.can_tan_he_patriarch = MsgAdapter.ReadShort()
    self.is_join_tan_he_patriarch = MsgAdapter.ReadShort()
    self.guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    self.guild_notice = MsgAdapter.ReadStrN(PROTC_TEAM_NOTICE_LEN)
    self.guild_id = MsgAdapter.ReadInt()
    self.level = MsgAdapter.ReadInt()
    self.exp = MsgAdapter.ReadLL()
    self.create_time = MsgAdapter.ReadUInt()
    self.join_time = MsgAdapter.ReadUInt()
    self.is_need_approval = MsgAdapter.ReadInt()
    self.guild_buff_rank = MsgAdapter.ReadInt()
--[[    local count = MsgAdapter.ReadInt()
    self.eventhandler_list = {}
    for i = 1, count do
        local temp = GuildEventhandlerParam()
        if temp.eventhandler_type > 0 then
            table.insert(self.eventhandler_list, temp)
        end
    end--]]
    self.arms = MsgAdapter.ReadStrN(8)
end

SCQuitGuild = SCQuitGuild or BaseClass(BaseProtocolStruct)
function SCQuitGuild:__init()
    self.msg_type = 9802
end
function SCQuitGuild:Decode()
    self.reason = MsgAdapter.ReadInt()
end
local function MemberInfo()
    return {
        name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN),
        uid = MsgAdapter.ReadInt(),
        online = MsgAdapter.ReadChar(),
        post = MsgAdapter.ReadChar(),
        level = MsgAdapter.ReadShort(),
        top_level = MsgAdapter.ReadInt(),
        profession = MsgAdapter.ReadInt(),
        gongxian = MsgAdapter.ReadInt(),
        total_gongxian = MsgAdapter.ReadInt(),
        join_time = MsgAdapter.ReadLL(),
        timestamp = MsgAdapter.ReadLL(),
        appearance = Appearance(),
		seven_active = MsgAdapter.ReadInt(),
		last_change_times = MsgAdapter.ReadUInt(),
        role_guild_task_finish_times = MsgAdapter.ReadShort(),     -- 完成次数
        role_guild_task_is_mail_notice = MsgAdapter.ReadChar(),    -- 邮件通知
        role_guild_task_is_solicit = MsgAdapter.ReadChar(),        -- 是否征集
        role_guild_task_solicit_task_id = MsgAdapter.ReadInt(),    -- 征集任务id
        role_guild_task_solicit_num = MsgAdapter.ReadShort(),        -- 征集进度
        role_guild_task_solicit_fetch_num = MsgAdapter.ReadShort(),  -- 征集进度是否领取
    }
end

SCGuildMemberInfo = SCGuildMemberInfo or BaseClass(BaseProtocolStruct)
function SCGuildMemberInfo:__init()
    self.msg_type = 9804
end
function SCGuildMemberInfo:Decode()
    self.member_count = MsgAdapter.ReadInt()
    self.member_list = {}
    for i = 1, self.member_count do
        self.member_list[i] = MemberInfo()
        -- self.member_list[i].index = i
    end
end

CSGuildExpelOutReq = CSGuildExpelOutReq or BaseClass(BaseProtocolStruct)
function CSGuildExpelOutReq:__init()
    self.msg_type = 9807
end

function CSGuildExpelOutReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.guild_id)
    MsgAdapter.WriteInt(self.expel_uid)
end

CSSetGuildNoticeReq = CSSetGuildNoticeReq or BaseClass(BaseProtocolStruct)
function CSSetGuildNoticeReq:__init()
    self.msg_type = 9808
end
function CSSetGuildNoticeReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.guild_id)
    MsgAdapter.WriteStrN(self.notice, PROTC_TEAM_NOTICE_LEN)
end

CSGetGuildList = CSGetGuildList or BaseClass(BaseProtocolStruct)
function CSGetGuildList:__init()
    self.msg_type = 9809
end
function CSGetGuildList:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param)
	MsgAdapter.WriteInt(self.param1)
end
local function GuildNodeInfo()
    return {
        -- is_apply = MsgAdapter.ReadInt(), -- 1是已申请
        guild_id = MsgAdapter.ReadInt(),
        uid = MsgAdapter.ReadInt(),
        level = MsgAdapter.ReadInt(),
        member_count = MsgAdapter.ReadInt(),
        guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN),
        patriarch_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN),
        arms = MsgAdapter.ReadStrN(8),
		seven_active = MsgAdapter.ReadInt(),
        last_change_times = MsgAdapter.ReadUInt(),
        extra_member_list = MsgAdapter.ReadInt(),
    }
end
SCSendGuildList = SCSendGuildList or BaseClass(BaseProtocolStruct)
function SCSendGuildList:__init()
    self.msg_type = 9810
end
function SCSendGuildList:Decode()
    self.count = MsgAdapter.ReadInt()
    self.guild_list = {}
    for i = 1, self.count do
        self.guild_list[i] = GuildNodeInfo()
        self.guild_list[i].index = i
    end
end

SCSendGuildNotic = SCSendGuildNotic or BaseClass(BaseProtocolStruct)
function SCSendGuildNotic:__init()
    self.msg_type = 9811
end
function SCSendGuildNotic:Decode()
    self.guild_id = MsgAdapter.ReadInt()
    self.notice = MsgAdapter.ReadStrN(PROTC_TEAM_NOTICE_LEN)
end
CSGuildPetTrainingUp = CSGuildPetTrainingUp or BaseClass(BaseProtocolStruct)
function CSGuildPetTrainingUp:__init()
    self.msg_type = 9812
end
function CSGuildPetTrainingUp:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.training_type)
end

CSGuildPetTraining = CSGuildPetTraining or BaseClass(BaseProtocolStruct)
function CSGuildPetTraining:__init()
    self.msg_type = 9813
end
function CSGuildPetTraining:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.pet_index)
    MsgAdapter.WriteInt(self.places_index)
 --0-2
end

CSGetGuildTrainingInfo = CSGetGuildTrainingInfo or BaseClass(BaseProtocolStruct)
function CSGetGuildTrainingInfo:__init()
    self.msg_type = 9814
end
function CSGetGuildTrainingInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

local function GuildTraningPetParam()
    return {
        pet_index = MsgAdapter.ReadInt(),
        begin_time = MsgAdapter.ReadUInt()
    }
end
SCSendGuildTrainingInfo = SCSendGuildTrainingInfo or BaseClass(BaseProtocolStruct)
function SCSendGuildTrainingInfo:__init()
    self.msg_type = 9815
end
function SCSendGuildTrainingInfo:Decode()
    self.guild_training_level = {}
    for i = 1, GUILD_PET_TRAINING_TYPE_COUNT do
        if i > 1 then
            self.guild_training_level[i - 1] = {level = MsgAdapter.ReadInt()}
            self.guild_training_level[i - 1].index = i - 1
        else
            MsgAdapter.ReadInt()
        end
    end
    self.traning_pet_index = {}
    for i = 1, GUILD_PET_TRAINING_PET_MAX_COUNT do
        self.traning_pet_index[i] = GuildTraningPetParam()
        self.traning_pet_index[i].index = i
    end
end
local function GuildApplyUserInfo()
    return {
        uid = MsgAdapter.ReadInt(),
        avatar_timestamp = MsgAdapter.ReadLL(),
        level = MsgAdapter.ReadInt(),
        top_level = MsgAdapter.ReadInt(),
        profession = MsgAdapter.ReadInt(),
        name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        time = MsgAdapter.ReadLL(),
        avatar_type = MsgAdapter.ReadShort(),
        reserve_sh = MsgAdapter.ReadShort(),
        MsgAdapter.ReadInt()
    }
end

SCSendGuildApplyList = SCSendGuildApplyList or BaseClass(BaseProtocolStruct)
function SCSendGuildApplyList:__init()
    self.msg_type = 9816
end
function SCSendGuildApplyList:Decode()
    self.count = MsgAdapter.ReadInt()
    self.apply_list = {}
    for i = 1, self.count do
        self.apply_list[i] = GuildApplyUserInfo()
    end
end

CSGuildExploreOp = CSGuildExploreOp or BaseClass(BaseProtocolStruct)
function CSGuildExploreOp:__init()
    self.msg_type = 9817
end
function CSGuildExploreOp:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteInt(self.param)
end
SCSendGuildExploreInfo = SCSendGuildExploreInfo or BaseClass(BaseProtocolStruct)
function SCSendGuildExploreInfo:__init()
    self.msg_type = 9818
end
function SCSendGuildExploreInfo:Decode()
    self.guild_explore_flag = MsgAdapter.ReadShort()
    self.guild_explore_seq = MsgAdapter.ReadShort()
    self.guild_explore_open_time = MsgAdapter.ReadUInt()
end

SCCreateRet = SCCreateRet or BaseClass(BaseProtocolStruct)
function SCCreateRet:__init()
    self.msg_type = 9819
end
function SCCreateRet:Decode()
    self.param = MsgAdapter.ReadInt()
end
CSGuildAbdicationOp = CSGuildAbdicationOp or BaseClass(BaseProtocolStruct)
function CSGuildAbdicationOp:__init()
   self.msg_type = 9820
end
function CSGuildAbdicationOp:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.guild_id)
    MsgAdapter.WriteInt(self.uid)
end

CSGuildAppointmentOp = CSGuildAppointmentOp or BaseClass(BaseProtocolStruct)
function CSGuildAppointmentOp:__init()
    self.msg_type = 9822
end
function CSGuildAppointmentOp:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.guild_id)
    MsgAdapter.WriteInt(self.uid)
    MsgAdapter.WriteInt(self.post)
end

CSGuildShopBuy = CSGuildShopBuy or BaseClass(BaseProtocolStruct)
function CSGuildShopBuy:__init()
    self.msg_type = 9823
end
function CSGuildShopBuy:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.guild_id)
    MsgAdapter.WriteInt(self.shop_seq)
    MsgAdapter.WriteInt(self.count)
end
local function ShopNode()
    return {
        seq = MsgAdapter.ReadShort(),
        buy_count = MsgAdapter.ReadShort()
    }
end
SCGuildShopInfo = SCGuildShopInfo or BaseClass(BaseProtocolStruct)
function SCGuildShopInfo:__init()
    self.msg_type = 9824
end
function SCGuildShopInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.shop_buy_list = {}
    for i = 1, self.count do
        local temp = ShopNode()
        self.shop_buy_list[temp.seq] = temp
    end
end

SCGuildPetTrainingExp = SCGuildPetTrainingExp or BaseClass(BaseProtocolStruct)
function SCGuildPetTrainingExp:__init()
    self.msg_type = 9825
end
function SCGuildPetTrainingExp:Decode()
    self.pet_index = MsgAdapter.ReadInt()
    self.time = MsgAdapter.ReadInt()
    self.exp = MsgAdapter.ReadInt()
end

CSGuildRenameReq = CSGuildRenameReq or BaseClass(BaseProtocolStruct)
function CSGuildRenameReq:__init()
    self.msg_type = 9826
end

function CSGuildRenameReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteStrN(self.new_name, PROTC_GUILD_NAME_LEN)
end

-- CSGuildBuildReq = CSGuildBuildReq or BaseClass(BaseProtocolStruct)
-- function CSGuildBuildReq:__init()
--     self.msg_type = 9826
-- end
-- function CSGuildBuildReq:Encode()
--     MsgAdapter.WriteBegin(self.msg_type)
--     MsgAdapter.WriteInt(self.guild_id)
--     MsgAdapter.WriteInt(self.build_seq)
-- end

CSGuildBuildDonateReq = CSGuildBuildDonateReq or BaseClass(BaseProtocolStruct)
function CSGuildBuildDonateReq:__init()
    self.msg_type = 9827
end
function CSGuildBuildDonateReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.guild_id)
    MsgAdapter.WriteInt(self.item_id)
    MsgAdapter.WriteInt(self.num)
    MsgAdapter.WriteInt(self.build_seq)
end
SCGuildBuildInfo = SCGuildBuildInfo or BaseClass(BaseProtocolStruct)
function SCGuildBuildInfo:__init()
    self.msg_type = 9828
end
function SCGuildBuildInfo:Decode()
    self.build_flag = MsgAdapter.ReadInt()
    self.build_value = {}
    for i = 1, GUILD_BUILD_COUNT do
        self.build_value[i - 1] = MsgAdapter.ReadInt()
    end
    self.build_donate_count = {}
    for i = 1, GUILD_GUILD_DONATE_COUNT_MAX do
        self.build_donate_count[i - 1] = MsgAdapter.ReadUShort()
    end
end

SCGuildBossInfo = SCGuildBossInfo or BaseClass(BaseProtocolStruct)
function SCGuildBossInfo:__init()
    self.msg_type = 9831
end
function SCGuildBossInfo:Decode()
    self.guild_boss_chapter_id = MsgAdapter.ReadInt()--今天章节
    --self.guild_boss_seq = MsgAdapter.ReadInt()--boss序号
    --self.guild_boss_play_times = MsgAdapter.ReadInt()--挑战次数
	--self.remain_hp_num = MsgAdapter.ReadLL() -- 剩余血量
    --self.max_hp_num = MsgAdapter.ReadLL()--最大血量
    --self.hp_refresh_time = MsgAdapter.ReadUInt()--血量刷新时间
	self.boss_participate_reward_fetch_flag = MsgAdapter.ReadInt()--参与奖励领取标记（二进制）
	--self.boss_kill_reward_fetch_flag = MsgAdapter.ReadInt()--击杀奖励领取标记（二进制）
end

CSGuildBossReq = CSGuildBossReq or BaseClass(BaseProtocolStruct)
function CSGuildBossReq:__init()
    self.msg_type = 9832
end
function CSGuildBossReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.p1)
end

CSGuildLevelUpReq = CSGuildLevelUpReq or BaseClass(BaseProtocolStruct)
function CSGuildLevelUpReq:__init()
    self.msg_type = 9833
end
function CSGuildLevelUpReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.guild_id)
end

SCPlayerApplyList = SCPlayerApplyList or BaseClass(BaseProtocolStruct)
function SCPlayerApplyList:__init()
    self.msg_type = 9834
end
function SCPlayerApplyList:Decode()
    self.apply_count = MsgAdapter.ReadInt()
    self.guild_id_list = {}
    for i = 1, self.apply_count do
        self.guild_id_list[MsgAdapter.ReadInt()] = i
    end
end

local function GuildBossRankItem()
    return {
        role_id = MsgAdapter.ReadInt(),
        role_level = MsgAdapter.ReadInt(),
        top_level = MsgAdapter.ReadInt(),
        avatar_type = MsgAdapter.ReadInt(),
		avatar_id = MsgAdapter.ReadInt(),
        prof_base = MsgAdapter.ReadInt(), --职业 [0,9)
        role_name =MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        total_damage = MsgAdapter.ReadLL(),
    }
end
SCGuildBossRankInfo = SCGuildBossRankInfo or BaseClass(BaseProtocolStruct)
function SCGuildBossRankInfo:__init()
    self.msg_type = 9836
end
function SCGuildBossRankInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.rank_list = {}
    for i = 1, self.count, 1 do
        self.rank_list[i] = GuildBossRankItem()
        self.rank_list[i].index = i
    end
end

-----------------------------家族邀请---------------------------------------
CSGuildInvitationReq = CSGuildInvitationReq or BaseClass(BaseProtocolStruct)
function CSGuildInvitationReq:__init()
    self.msg_type = 9829
end

function CSGuildInvitationReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.uid)
end

SCGuildInvitationNotice = SCGuildInvitationNotice or BaseClass(BaseProtocolStruct)
function SCGuildInvitationNotice:__init()
    self.msg_type = 9830
end

function SCGuildInvitationNotice:Decode()
    self.invitation_info = {}
    self.invitation_info.user_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    self.invitation_info.guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
end

CSInvitationReply = CSInvitationReply or BaseClass(BaseProtocolStruct)
function CSInvitationReply:__init()
    self.msg_type = 9835
end

function CSInvitationReply:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.result)
end

--[[SCGuildEventInfo = SCGuildEventInfo or BaseClass(BaseProtocolStruct)
function SCGuildEventInfo:__init()
    self.msg_type = 9839
end
function SCGuildEventInfo:Decode()
    self.eventhandler_list = {}
    local count = MsgAdapter.ReadInt()
    for i = 1, count do
        local temp = GuildEventhandlerParam()
        if temp.eventhandler_type > 0 then
            table.insert(self.eventhandler_list, temp)
        end
    end
end--]]

--工会是否需要批准加入信息
SCGuildNeedApproveInfo = SCGuildNeedApproveInfo or BaseClass(BaseProtocolStruct)
function SCGuildNeedApproveInfo:__init()
    self.msg_type = 9837
end

function SCGuildNeedApproveInfo:Decode()
    self.is_need_approval = MsgAdapter.ReadInt()
end

--工会是否需要批准加入信息请求
CSGuildNeedApproveReq = CSGuildNeedApproveReq or BaseClass(BaseProtocolStruct)
function CSGuildNeedApproveReq:__init()
    self.msg_type = 9838
end

function CSGuildNeedApproveReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.is_need_approval)
end

CSGuildSetBanner = CSGuildSetBanner or BaseClass(BaseProtocolStruct)
function CSGuildSetBanner:__init()
    self.msg_type = 9840
end
function CSGuildSetBanner:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteStrN(self.arms, 8)
end

SCGuildBossChapterGuildRankInfo = SCGuildBossChapterGuildRankInfo or BaseClass(BaseProtocolStruct)
function SCGuildBossChapterGuildRankInfo:__init()
    self.msg_type = 9841
end
function SCGuildBossChapterGuildRankInfo:Decode()
    local count = MsgAdapter.ReadInt()
    self.rank_list = {}
    for i=1, count do
        local temp = {}
        temp.boss_index = MsgAdapter.ReadShort()
        temp.kill_num = MsgAdapter.ReadShort()
        temp.guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
        temp.guild_banner = MsgAdapter.ReadStrN(PROTC_GUILD_BANNER)
        temp.rank = i
        table.insert(self.rank_list, temp)
    end
end

SCGuildGatherDreamInfoList = SCGuildGatherDreamInfoList or BaseClass(BaseProtocolStruct)
function SCGuildGatherDreamInfoList:__init()
    self.msg_type = 9842
end
function SCGuildGatherDreamInfoList:Decode()
	MsgAdapter.WriteBegin(self.msg_type)
    local count = MsgAdapter.ReadInt()
    self.dream_list = {}
    for i=1, count do
        local temp = {}
        temp.from_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        temp.role_id = MsgAdapter.ReadInt()
        temp.item_id = MsgAdapter.ReadUShort()
        temp.avatar_id = MsgAdapter.ReadShort()
		temp.gather_num = MsgAdapter.ReadChar()
		temp.fetch_num = MsgAdapter.ReadChar()
		temp.need_max_num = MsgAdapter.ReadChar()
		temp.help_num = MsgAdapter.ReadChar()
		temp.avatar_type = MsgAdapter.ReadShort()
		temp.gather_dream_type = MsgAdapter.ReadShort()
        temp.publish_timestamp = MsgAdapter.ReadUInt()
        table.insert(self.dream_list, temp)
    end
end

SCGuildGatherDreamSignleInfo = SCGuildGatherDreamSignleInfo or BaseClass(BaseProtocolStruct)
function SCGuildGatherDreamSignleInfo:__init()
    self.msg_type = 9843
end
function SCGuildGatherDreamSignleInfo:Decode()
	MsgAdapter.WriteBegin(self.msg_type)
	self.dreamsignleinfo = {}
	self.dreamsignleinfo.from_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	self.dreamsignleinfo.role_id = MsgAdapter.ReadInt()
	self.dreamsignleinfo.item_id = MsgAdapter.ReadUShort()
	self.dreamsignleinfo.avatar_id = MsgAdapter.ReadShort()
	self.dreamsignleinfo.gather_num = MsgAdapter.ReadChar()
	self.dreamsignleinfo.fetch_num = MsgAdapter.ReadChar()
	self.dreamsignleinfo.need_max_num = MsgAdapter.ReadChar()
	self.dreamsignleinfo.help_num = MsgAdapter.ReadChar()
	self.dreamsignleinfo.avatar_type = MsgAdapter.ReadShort()
	self.dreamsignleinfo.gather_dream_type = MsgAdapter.ReadShort()
	self.dreamsignleinfo.publish_timestamp = MsgAdapter.ReadUInt()
end

SCGuildQuitInfo = SCGuildQuitInfo or BaseClass(BaseProtocolStruct)
function SCGuildQuitInfo:__init()
    self.msg_type = 9844    ---退出家族
end
function SCGuildQuitInfo:Decode()
    self.quit_count =  MsgAdapter.ReadInt()
    self.next_join_time = MsgAdapter.ReadUInt()
end

SCGuildRecommend = SCGuildRecommend or BaseClass(BaseProtocolStruct)
function SCGuildRecommend:__init()
    self.msg_type = 9845
end

function SCGuildRecommend:Decode()
    self.guild_id = MsgAdapter.ReadInt()
    self.guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    self.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    -- LogError("guild -> " .. self.guild_id .. "   name -> " .. self.guild_name .."   role_name -> " .. self.role_name)
end

--名人堂操作请求
CSGuildGallOfFameReq = CSGuildGallOfFameReq or BaseClass(BaseProtocolStruct)
function CSGuildGallOfFameReq:__init()
    self.msg_type = 9846
end

function CSGuildGallOfFameReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.rank_type)
end

--名人堂排行榜下发
SCGuildHallOfFameInfo = SCGuildHallOfFameInfo or BaseClass(BaseProtocolStruct)
function SCGuildHallOfFameInfo:__init()
    self.msg_type = 9847
end

function SCGuildHallOfFameInfo:Decode()
    self.info = {}
    self.info.rank_type = MsgAdapter.ReadInt()
    self.info.appearance = Appearance()
    self.info.rank_count = MsgAdapter.ReadInt()
    local rank_list = {}
    for i = 1, self.info.rank_count do
        rank_list[i] = {}
        rank_list[i].index = i
        rank_list[i].value = MsgAdapter.ReadInt()
        rank_list[i].role_id = MsgAdapter.ReadInt()
        rank_list[i].name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        rank_list[i].prof = MsgAdapter.ReadInt()
    end
    self.info.rank_list = rank_list
end

--名人堂祈福下发
SCGuildQiFuInfo = SCGuildQiFuInfo or BaseClass(BaseProtocolStruct)
function SCGuildQiFuInfo:__init()
    self.msg_type = 9848
end

function SCGuildQiFuInfo:Decode()
    self.info = {}
    self.info.is_open = MsgAdapter.ReadInt()
    self.info.bless_value = MsgAdapter.ReadInt()
    self.info.today_bless_times = MsgAdapter.ReadInt()
    self.info.is_get_bless_attr = MsgAdapter.ReadInt()
end

-- 家族内政建设单条信息下发
SCGuildConstructInfo = SCGuildConstructInfo or BaseClass(BaseProtocolStruct)
function SCGuildConstructInfo:__init()
    self.msg_type = 9849
end

function SCGuildConstructInfo:Decode()
    self.info = GuildBuildingData.Instance:Decode()
end

-- 家族内政建设操作请求
CSGuildConstructReq = CSGuildConstructReq or BaseClass(BaseProtocolStruct)
function CSGuildConstructReq:__init()
    self.msg_type = 9850
end

function CSGuildConstructReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.build_type)
    MsgAdapter.WriteShort(self.req_type)
end

-- 家族内政建设建筑完成后下发
SCGuildBuildingFinishInfo = SCGuildBuildingFinishInfo or BaseClass(BaseProtocolStruct)
function SCGuildBuildingFinishInfo:__init()
    self.msg_type = 9851
end

function SCGuildBuildingFinishInfo:Decode()
    self.info = {}
    self.info.build_type = MsgAdapter.ReadShort()
    self.info.build_level = MsgAdapter.ReadShort()
end

-- 家族内政建设成员使用家族建材后下发
SCGuildUseBuildMaterialInfo = SCGuildUseBuildMaterialInfo or BaseClass(BaseProtocolStruct)
function SCGuildUseBuildMaterialInfo:__init()
    self.msg_type = 9852
end

function SCGuildUseBuildMaterialInfo:Decode()
    self.info = {}
    self.info.op_type = MsgAdapter.ReadInt()
    self.info.role_id = MsgAdapter.ReadInt()
    self.info.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    self.info.item_id = MsgAdapter.ReadInt()
    self.info.num = MsgAdapter.ReadInt()
    self.info.build_type = MsgAdapter.ReadInt()
end

-- 家族内政建设所有信息下发
SCGuildConstructAllInfo = SCGuildConstructAllInfo or BaseClass(BaseProtocolStruct)
function SCGuildConstructAllInfo:__init()
    self.msg_type = 9853
end

function SCGuildConstructAllInfo:Decode()
    self.info = {}
    for i = 1, GUILD_BUILDING_MAX do
        local t = GuildBuildingData.Instance:Decode()
        self.info[t.build_type] = t
    end
    self.now_building_type = MsgAdapter.ReadInt()
    self.finish_time = MsgAdapter.ReadUInt()
    self.building_count = MsgAdapter.ReadInt()
end

-- 家族内政建加速信息下发
SCGuildSpeedUpInfo = SCGuildSpeedUpInfo or BaseClass(BaseProtocolStruct)
function SCGuildSpeedUpInfo:__init()
    self.msg_type = 9854
end

function SCGuildSpeedUpInfo:Decode()
    self.info = {}
    self.info.today_up_count = MsgAdapter.ReadShort()
    self.info.is_show_speed = MsgAdapter.ReadShort()
    self.info.is_helped_count = MsgAdapter.ReadInt()
end

-- 家族内政建建造完成时间戳
SCGuildConstructTimeInfo = SCGuildConstructTimeInfo or BaseClass(BaseProtocolStruct)
function SCGuildConstructTimeInfo:__init()
    self.msg_type = 9855
end

function SCGuildConstructTimeInfo:Decode()
    self.now_building_type = MsgAdapter.ReadInt()
    self.finish_time = MsgAdapter.ReadUInt()
    self.building_count = MsgAdapter.ReadInt()
end

-- 家族加速开启人信息
SCGuildConstructOpenSUPInfo = SCGuildConstructOpenSUPInfo or BaseClass(BaseProtocolStruct)
function SCGuildConstructOpenSUPInfo:__init()
    self.msg_type = 9856
end

function SCGuildConstructOpenSUPInfo:Decode()
    self.info = {}
    self.info.open_post = MsgAdapter.ReadInt()
    self.info.open_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
end

-- 家族单个成员更新
SCGuildMemberInfoChange = SCGuildMemberInfoChange or BaseClass(BaseProtocolStruct)
function SCGuildMemberInfoChange:__init()
    self.msg_type = 9857
end

function SCGuildMemberInfoChange:Decode()
    self.member_info = MemberInfo()
end