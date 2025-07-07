-- 请求频道聊天
CSChannelChat = CSChannelChat or BaseClass(BaseProtocolStruct)
function CSChannelChat:__init()
    self.msg_type = 2130
end

function CSChannelChat:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteChar(self.content_type)
    MsgAdapter.WriteChar(0)
    MsgAdapter.WriteShort(self.channel_type)
    MsgAdapter.WriteInt2(self.group_id)     -- 群组id 非群组聊天填0

    for i = 1, ChatConfig.CHAT_MSG_ITEM_INFO_NUM_MAX do
        local chat_item = self.chat_items[i] or ChatItem.EmptyItem()
        MsgAdapter.WriteInt(chat_item:Type())
        MsgAdapter.WriteInt(chat_item:Param1())
        MsgAdapter.WriteInt(0)
        MsgAdapter.WriteInt(0)
        MsgAdapter.WriteLL(chat_item:Param2())
    end

    MsgAdapter.WriteInt(self.msg_length)
    MsgAdapter.WriteStrN(self.content, self.msg_length)
end

-- 请求私聊
CSSingleChat = CSSingleChat or BaseClass(BaseProtocolStruct)
function CSSingleChat:__init()
    self.msg_type = 2131
end

function CSSingleChat:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteChar(self.content_type)
    MsgAdapter.WriteChar(0)
    MsgAdapter.WriteShort(0)

    for i = 1, ChatConfig.CHAT_MSG_ITEM_INFO_NUM_MAX do
        local chat_item = self.chat_items[i] or ChatItem.EmptyItem()
        MsgAdapter.WriteInt(chat_item:Type())
        MsgAdapter.WriteInt(chat_item:Param1())
        MsgAdapter.WriteInt(0)
        MsgAdapter.WriteInt(0)
        MsgAdapter.WriteLL(chat_item:Param2())
    end

    MsgAdapter.WriteInt(self.to_uid)
    MsgAdapter.WriteInt(self.msg_length)
    MsgAdapter.WriteStrN(self.content, self.msg_length)
end

-- 下发频道聊天
SCChannelChat = SCChannelChat or BaseClass(BaseProtocolStruct)
function SCChannelChat:__init()
    self.msg_type = 2132
end

function SCChannelChat:Decode()
    local t = {}
    t.from_uid = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.avatar_type = MsgAdapter.ReadShort()
    t.prof = MsgAdapter.ReadShort()
    t.avatar_id = MsgAdapter.ReadShort()--人物头像
    t.content_type = MsgAdapter.ReadShort()
    t.avatar_timestamp = MsgAdapter.ReadLL()
    t.level = MsgAdapter.ReadShort()
    t.channel_type = MsgAdapter.ReadShort()
    t.top_level = MsgAdapter.ReadInt()
    t.reserve_sh = MsgAdapter.ReadShort()
    t.fly_flag = MsgAdapter.ReadShort()
    t.fly_flag = t.fly_flag > FlyUpStageType.High and FlyUpStageType.High or t.fly_flag
    t.msg_timestamp = MsgAdapter.ReadUInt()
    t.avatar_quality = t.fly_flag
    t.face_chatframe = MsgAdapter.ReadInt()     -- 聊天框|冒泡框
    t.face_signature = MsgAdapter.ReadInt()     -- 签名档|尾标

    t.msg_type = MsgAdapter.ReadInt()           -- ChatConfig.MsgType
    t.group_id = MsgAdapter.ReadInt2()          -- 群组id 非群组聊天填0

    t.family = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)

    t.item_list = {}
    for i = 1, ChatConfig.CHAT_MSG_ITEM_INFO_NUM_MAX do
        t.item_list[i] = {
            type = MsgAdapter.ReadInt(),
            item_id = MsgAdapter.ReadInt(),
            show = MsgAdapter.ReadInt(),
            show2 = MsgAdapter.ReadInt(),
            sign_id = string.format("%ss%s", MsgAdapter.ReadInt(), MsgAdapter.ReadInt()),
        }
    end

    t.msg_length = MsgAdapter.ReadUInt()
    t.content = MsgAdapter.ReadStrN(t.msg_length)
    self.info = t
end

-- 下发私聊
SCSingleChat = SCSingleChat or BaseClass(BaseProtocolStruct)
function SCSingleChat:__init()
    self.msg_type = 2133
end

function SCSingleChat:Decode()
    local t = {}
    t.from_uid = MsgAdapter.ReadInt()
    t.to_uid = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.avatar_type = MsgAdapter.ReadShort()
    t.prof = MsgAdapter.ReadShort()
    t.avatar_id = MsgAdapter.ReadShort()--人物头像
    t.fly_flag = MsgAdapter.ReadShort()
    t.fly_flag = t.fly_flag > FlyUpStageType.High and FlyUpStageType.High or t.fly_flag
    t.authority_type = MsgAdapter.ReadChar()
    t.content_type = MsgAdapter.ReadChar()
    t.level = MsgAdapter.ReadShort()
    t.top_level = MsgAdapter.ReadInt()
    t.avatar_timestamp = MsgAdapter.ReadLL()
    t.msg_timestamp = MsgAdapter.ReadUInt()
    t.avatar_quality = t.fly_flag

    t.face_chatframe = MsgAdapter.ReadInt()     -- 聊天框|冒泡框
    t.face_signature = MsgAdapter.ReadInt()     -- 签名档|尾标

    t.item_list = {}
    for i = 1, ChatConfig.CHAT_MSG_ITEM_INFO_NUM_MAX do
        t.item_list[i] = {
            type = MsgAdapter.ReadInt(),
            item_id = MsgAdapter.ReadInt(),
            show = MsgAdapter.ReadInt(),
            show2 = MsgAdapter.ReadInt(),
            sign_id = string.format("%ss%s", MsgAdapter.ReadInt(), MsgAdapter.ReadInt()),
        }
    end

    t.msg_length = MsgAdapter.ReadUInt()
    t.content = MsgAdapter.ReadStrN(t.msg_length)
    self.info = t
end

-- 聊天中物品信息请求
CSChatItemInfoReq = CSChatItemInfoReq or BaseClass(BaseProtocolStruct)
function CSChatItemInfoReq:__init()
    self.msg_type = 2135
end

function CSChatItemInfoReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    for _, val in ipairs(self.sign_id) do
        MsgAdapter.WriteInt(val)
    end
end

-- 聊天物品信息
SCChatItemInfoRet = SCChatItemInfoRet  or BaseClass(BaseProtocolStruct)
function SCChatItemInfoRet:__init()
    self.msg_type = 2136
end

function SCChatItemInfoRet:Decode()
    local t = {}
    t.sign_id = string.format("%ss%s", MsgAdapter.ReadInt(), MsgAdapter.ReadInt())
    t.item_id = MsgAdapter.ReadUShort()
	t.num = MsgAdapter.ReadShort()
	t.is_bind = MsgAdapter.ReadChar() ~= 0
	t.has_param = MsgAdapter.ReadChar() ~= 0
	t.invalid_time = MsgAdapter.ReadUInt()
	if t.has_param then
		local param_length = MsgAdapter.ReadShort()
		t.param = ItemDataParam(t.item_id, param_length)
    end
    --当发送物品为宠物时会有宠物属性列表
    t.has_attrs = MsgAdapter.ReadInt()
    if t.has_attrs ~= 0 then
        t.attr_list = {}
        for i = 1, PET_ATTR_LIST_COUNT do
            t.attr_list[i - 1] = MsgAdapter.ReadInt()
        end
    end
    self.info = t
end

SCPlayerIsForbidden = SCPlayerIsForbidden or BaseClass(BaseProtocolStruct)
function SCPlayerIsForbidden:__init()
    self.msg_type = 2139
end

function SCPlayerIsForbidden:Decode()
    self.is_forbidden = MsgAdapter.ReadShort()
    self.forbid_talk_type = MsgAdapter.ReadShort()
    self.forbid_talk_end_timestamp = MsgAdapter.ReadUInt()
end
--弑神之塔请求
CSNewCourageChallengeOp = CSNewCourageChallengeOp or BaseClass(BaseProtocolStruct)
function CSNewCourageChallengeOp:__init()
    self.msg_type = 5330
end
function CSNewCourageChallengeOp:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)--0关卡列表信息 1 奖励信息 2 领取普通奖励seq 3 领取战令奖励seq 4一键
    MsgAdapter.WriteInt(self.param)
end
--弑神之塔列表信息
SCNewCourageChallengeLevelListInfo = SCNewCourageChallengeLevelListInfo or BaseClass(BaseProtocolStruct)
function SCNewCourageChallengeLevelListInfo:__init()
    self.msg_type = 5331
end
function SCNewCourageChallengeLevelListInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.list = {}
    local info
    for i = 1, self.count, 1 do
        info = self:NewLayerLevelNode()
        if self.list[info.layer_level] == nil then
            self.list[info.layer_level] = {}
        end
        self.list[info.layer_level][info.level_seq] = info
    end
end
function SCNewCourageChallengeLevelListInfo:NewLayerLevelNode()
    local t = {}
    t.layer_level = MsgAdapter.ReadShort()
    t.level_seq = MsgAdapter.ReadChar()
    t.flag_real = MsgAdapter.ReadUChar()
    t.flag = bit:d8b(t.flag_real)--[1-3]条件完成 8首杀标志
    t.flag_list, t.flag_num = CourageChallengeStruct.GetStarList(t.flag)
    t.first_kill = t.flag[1] or 0
    t.get_extra = t.flag[2] or 0
    return t
end
--弑神之塔单个信息
SCNewCourageChallengeLevelSignleInfo = SCNewCourageChallengeLevelSignleInfo or BaseClass(BaseProtocolStruct)
function SCNewCourageChallengeLevelSignleInfo:__init()
    self.msg_type = 5332
end
function SCNewCourageChallengeLevelSignleInfo:Decode()
    self.info = {}
    self.info.layer_level = MsgAdapter.ReadShort()
    self.info.level_seq = MsgAdapter.ReadChar()
    self.info.flag_real = MsgAdapter.ReadUChar()
    self.info.flag = bit:d8b(self.info.flag_real)
    self.info.flag_list, self.info.flag_num = CourageChallengeStruct.GetStarList(self.info.flag)
    self.info.first_kill = self.info.flag[1] or 0
    self.info.get_extra = self.info.flag[2] or 0
    self.level = self.info.layer_level
    self.seq = self.info.level_seq
    LogDG(self.info)
end

--弑神之塔奖励信息
SCNewCourageChallengeRewardInfo = SCNewCourageChallengeRewardInfo or BaseClass(BaseProtocolStruct)
function SCNewCourageChallengeRewardInfo:__init()
    self.msg_type = 5333
end
function SCNewCourageChallengeRewardInfo:Decode()
    self.all_info = {}
    self.all_info.token_is_buy = MsgAdapter.ReadChar() -- 0 1已购买
    MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    self.all_info.common_reward_flag = MsgAdapter.IntFlag(16)
    self.all_info.token_reward_flag = MsgAdapter.IntFlag(16)
end
-- 快捷聊天
SCChatRecordList = SCChatRecordList  or BaseClass(BaseProtocolStruct)
function SCChatRecordList:__init()
    self.msg_type = 2147
end

function SCChatRecordList:Decode()
    local t1, t2 = {}, {friends = {}, groups = {}}
    local show_type = SocietyConfig.QuickShowType
    for i = 1, ChatConfig.RECORD_NUM_MAX do
        local i1, i2 = MsgAdapter.ReadInt(), MsgAdapter.ReadInt()
        if i2 > 0 then
            local group_key = SocietyData.GroupId2Key({i1, i2})
            table.insert(t1, {type = show_type.group, val = group_key})
            t2.groups[group_key] = true
        elseif i1 > 0 then
            table.insert(t1, {type = show_type.role, val = i1})
            t2.friends[i1] = true
        elseif show_type.add == i1 then
            table.insert(t1, {type = show_type.add, val = show_type.add})
            t2.add = true
        end
    end
    self.show_list = t1
    self.show_id_list = t2
end

-- 快捷聊天设置
CSChatRecordSet = CSChatRecordSet  or BaseClass(BaseProtocolStruct)
function CSChatRecordSet:__init()
    self.msg_type = 2148
end

function CSChatRecordSet:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    local show_type = SocietyConfig.QuickShowType
    for i = 1, ChatConfig.RECORD_NUM_MAX do
        local info = self.show_list[i]
        if info then
            if show_type.role == info.type then
                MsgAdapter.WriteLL(info.val)
            elseif show_type.group == info.type then
                local t = SocietyData.GroupKey2Id(info.val)
                MsgAdapter.WriteInt(t[1])
                MsgAdapter.WriteInt(t[2])
            elseif show_type.add == info.type then
                MsgAdapter.WriteLL(show_type.add)
            end
        else
            MsgAdapter.WriteLL(0)
        end
    end
end

-- 快捷发言设置
CSChatSimpleRecordSet = CSChatSimpleRecordSet  or BaseClass(BaseProtocolStruct)
function CSChatSimpleRecordSet:__init()
    self.msg_type = 2150
end

function CSChatSimpleRecordSet:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.index)
    MsgAdapter.WriteStrN(self.record, PROTC_ROLE_NAME_LEN)
end

-- 快捷发言下发
SCChatSimpleRecordInfo = SCChatSimpleRecordInfo  or BaseClass(BaseProtocolStruct)
function SCChatSimpleRecordInfo:__init()
    self.msg_type = 2149
end

function SCChatSimpleRecordInfo:Decode()
    self.records = {}
    for i = 1, ChatConfig.SIMPLE_RECORD_NUM_MAX do
        self.records[i] = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    end
end

-- 删除被禁言玩家的聊天
SCMutePlayerDeleteChatRecord = SCMutePlayerDeleteChatRecord  or BaseClass(BaseProtocolStruct)
function SCMutePlayerDeleteChatRecord:__init()
    self.msg_type = 2151
end

function SCMutePlayerDeleteChatRecord:Decode()
    self.role_id = MsgAdapter.ReadInt()
end

SCCourageChallengePetHelperInfo = SCCourageChallengePetHelperInfo  or BaseClass(BaseProtocolStruct)
function SCCourageChallengePetHelperInfo:__init()
    self.msg_type = 2152
end

function SCCourageChallengePetHelperInfo:Decode()
    self.pet_num = MsgAdapter.ReadInt()
end

CSUserChatRecord = CSUserChatRecord or BaseClass(BaseProtocolStruct)
function CSUserChatRecord:__init()
    self.msg_type = 2153
end

function CSUserChatRecord:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end
--===========================锢魔之塔总信息下发===========================
CourageChallengeStruct = CourageChallengeStruct or {}

function CourageChallengeStruct.LayerStruct()
    local t = {}
    t.layer_level = MsgAdapter.ReadShort()
    t.level_seq = MsgAdapter.ReadChar()
    t.flag = bit:d8b(MsgAdapter.ReadUChar())
    t.flag_list,t.flag_num = CourageChallengeStruct.GetStarList(t.flag)
    t.first_kill = t.flag[1] or 0
    t.get_extra = t.flag[2] or 0
    return t
end

function CourageChallengeStruct.RewardStruct()
    local t = {}
    t.layer_level = MsgAdapter.ReadInt()
    t.flag = MsgAdapter.IntFlag(2)
    return t
end

function CourageChallengeStruct.GetStarList(flag)
    local list,num = {},0
    for i=0,2 do
        list[i+1] = flag[8-i] or 0
        num = num + list[i+1]
    end
    return list,num
end

function CourageChallengeStruct.RankinfoStruct(index)
    local t = {}
    t.rank = index
    t.role_id = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.avatar_type = MsgAdapter.ReadShort()
    t.avatar_id = MsgAdapter.ReadShort()--人物头像
    t.prof = MsgAdapter.ReadShort()--新增角色职业
    t.star_num = MsgAdapter.ReadInt()
    t.layer_level = MsgAdapter.ReadInt()
    t.rank_value = t.star_num
    return t
end

-- 勇气的挑战请求
CSCourageChallengeOp = CSCourageChallengeOp or BaseClass(BaseProtocolStruct)
function CSCourageChallengeOp:__init()
    self.msg_type = 2140
end

function CSCourageChallengeOp:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

SCCourageChallengeAllInfo = SCCourageChallengeAllInfo or BaseClass(BaseProtocolStruct)
function SCCourageChallengeAllInfo:__init()
    self.msg_type = 2141
end 

function SCCourageChallengeAllInfo:Decode()
    self.layer_list = {}
    for i=1,MsgAdapter.ReadInt() do
        local info = CourageChallengeStruct.LayerStruct()
        if self.layer_list[info.layer_level] == nil then
            self.layer_list[info.layer_level] = {}
        end
        self.layer_list[info.layer_level][info.level_seq] = info
    end
    self.reward_list = {}
    for i=1,MsgAdapter.ReadInt() do
        local reward_info = CourageChallengeStruct.RewardStruct()
        self.reward_list[reward_info.layer_level] = reward_info
    end
end

SCCourageChallengeSingleInfo = SCCourageChallengeSingleInfo or BaseClass(BaseProtocolStruct)
function SCCourageChallengeSingleInfo:__init()
    self.msg_type = 2143
end 

function SCCourageChallengeSingleInfo:Decode()
    self.layer_info = CourageChallengeStruct.LayerStruct()
    self.level = self.layer_info.layer_level
    self.seq = self.layer_info.level_seq
end

SCCourageChallengeScoreInfo = SCCourageChallengeScoreInfo or BaseClass(BaseProtocolStruct)
function SCCourageChallengeScoreInfo:__init()
    self.msg_type = 2144
end 

function SCCourageChallengeScoreInfo:Decode()
    self.reward_info = CourageChallengeStruct.RewardStruct()
    self.layer_level = self.reward_info.layer_level
end

--======================================================

SCMonsterWaveInfo = SCMonsterWaveInfo or BaseClass(BaseProtocolStruct)
function SCMonsterWaveInfo:__init()
    self.msg_type = 2145
end
function SCMonsterWaveInfo:Decode()
    self.play_times = MsgAdapter.ReadInt()
    self.today_pass_wave = {}
    self.today_pass_wave[0] = MsgAdapter.ReadInt()
    self.today_pass_wave[1] = MsgAdapter.ReadInt()
    self.reward_flag = {}
    self.reward_flag[0] = MsgAdapter.IntFlag()
    self.reward_flag[1] = MsgAdapter.IntFlag()
end

SCMonsterWaveFinishInfo = SCMonsterWaveFinishInfo or BaseClass(BaseProtocolStruct)

function SCMonsterWaveFinishInfo:__init()
    self.msg_type = 2146
end
function SCMonsterWaveFinishInfo:Decode()
    self.role_level = MsgAdapter.ReadInt()
    self.diff_type = MsgAdapter.ReadShort()
    self.pass_wave = MsgAdapter.ReadShort()
end

--成就所有信息
SCAchievementAllInfo = SCAchievementAllInfo or BaseClass(BaseProtocolStruct)

function SCAchievementAllInfo:__init()
    self.msg_type = 2150
end

function SCAchievementAllInfo:Decode()
    self.achieve_progress = {}
    for i = 0, ACHIEVEMENT_TYPE_NUM - 1 do
        self.achieve_progress[i] = MsgAdapter.ReadInt()
    end
    self.achieve_flag = MsgAdapter.IntFlag(32) --1024位 二进制
end

CSMedalReq = CSMedalReq or BaseClass(BaseProtocolStruct)
function CSMedalReq:__init()
    self.msg_type = 2160
end
function CSMedalReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
    MsgAdapter.WriteInt(self.param4)
    MsgAdapter.WriteInt(self.param5)
end

SCMedalInfo = SCMedalInfo or BaseClass(BaseProtocolStruct)
function SCMedalInfo:__init()
    self.msg_type = 2161
end
function SCMedalInfo:Decode()
    -- self.medal_item_id = MsgAdapter.ReadInt()
    self.medal_list = {}
    self.count = MsgAdapter.ReadInt()
    for i = 1,self.count do 
        local t = {}
        t.grid_index = MsgAdapter.ReadInt()
        t.item_id = MsgAdapter.ReadUShort() -- 道具ID
        t.num = MsgAdapter.ReadShort()
        t.is_bind = MsgAdapter.ReadChar()
        t.has_param = MsgAdapter.ReadChar() ~= 0
        t.invalid_time = MsgAdapter.ReadUInt()
        if t.has_param then
            local param_length = MsgAdapter.ReadShort()
            t.param = ItemDataParam(t.item_id,param_length)
        end
        table.insert( self.medal_list, t )
    end 
end

SCMiYiInfo = SCMiYiInfo or BaseClass(BaseProtocolStruct)
function SCMiYiInfo:__init()
    self.msg_type = 3830--2170
end
function SCMiYiInfo:Decode()
    self.pass_info = {}
    for i = 1, MI_YI_MAX_LEVEL do
        self.pass_info[i] = {}
        for j = 1, MI_YI_MAX_MONSTER_NUM do
            self.pass_info[i][j] = MsgAdapter.ReadChar()
        end
    end
    self.fetch_flag = {}
    for i = 1, MI_YI_MAX_LEVEL do
        self.fetch_flag[i] = bit:d2b(MsgAdapter.ReadUShort())
    end
    --每周首杀奖励领取标志  
end

DAILY_GUILD_TASK_STATUS = {
    ACCEPTABLE = 0,
    ACCEPTED = 1,
    FINISHED = 2,
}
function ItemDrawInfo(t)
    local item = {}
    item.item_id = MsgAdapter.ReadUShort()
    MsgAdapter.ReadShort()
    item.num = MsgAdapter.ReadInt()
	item.is_bind = MsgAdapter.ReadShort()
    item.has_param = MsgAdapter.ReadShort()
    item.invalid_time = MsgAdapter.ReadUInt()
    item.gold_price = MsgAdapter.ReadInt()
    if item.has_param > 0 then
        local param_length = MsgAdapter.ReadInt()
        item.param = ItemDataParam(item.item_id, param_length)
    end
	item.item_unique_id_1 = MsgAdapter.ReadUInt()  
    item.item_unique_id_2 = MsgAdapter.ReadUInt()
    t.item_info = item
end

MiYiPokerStruct = {}
function MiYiPokerStruct.MiYiPoker()
    local t = {}
    t.index = MsgAdapter.ReadInt() --牌index[0, 4]
    t.role_uid = MsgAdapter.ReadInt()
    MsgAdapter.ReadInt()
    t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.fixed_reward = {} --固定奖励
    t.fixed_reward.reward_id = MsgAdapter.ReadInt()
    t.fixed_reward.num = MsgAdapter.ReadInt()
    t.fixed_reward.is_bind = MsgAdapter.ReadInt()
    ItemDrawInfo(t)
    return t
end
SCMiYiPoker = SCMiYiPoker or BaseClass(BaseProtocolStruct)
function SCMiYiPoker:__init()
    self.msg_type = 3831--2171
end
function SCMiYiPoker:Decode()
    self.draw_group_id = MsgAdapter.ReadInt()
    self.timeout = MsgAdapter.ReadUInt()
    self.mypoker = {}
    for i = 1, SIDE_MAX_ROLE_NUM do
        self.mypoker[i] = MiYiPokerStruct.MiYiPoker()
    end
end

SCMiYiPokerInfo = SCMiYiPokerInfo or BaseClass(BaseProtocolStruct)
function SCMiYiPokerInfo:__init()
    self.msg_type = 3833--2173
end

function SCMiYiPokerInfo:Decode()
    self.weekly_pass_count = MsgAdapter.ReadInt() --本周通关次数
    self.day_pass_count = MsgAdapter.ReadInt()
end
SCMiYiSceneInfo = SCMiYiSceneInfo or BaseClass(BaseProtocolStruct)
function SCMiYiSceneInfo:__init()
    self.msg_type = 3834--2174
end
function SCMiYiSceneInfo:Decode()
    self.kill_flag = bit:d2b(MsgAdapter.ReadUInt())
end
SCMiYiPokerKillPassInfo = SCMiYiPokerKillPassInfo or BaseClass(BaseProtocolStruct)
function SCMiYiPokerKillPassInfo:__init()
    self.msg_type = 3835--2175
end
function SCMiYiPokerKillPassInfo:Decode()
    self.is_first_kill = MsgAdapter.ReadUChar()
    self.is_week_kill = MsgAdapter.ReadUChar()
    self.day_pass_count = MsgAdapter.ReadUShort()
end
CSMiYiReq = CSMiYiReq or BaseClass(BaseProtocolStruct)
function CSMiYiReq:__init()
    self.msg_type = 3836
end
function CSMiYiReq:Encode()
    -- 0 领取周首杀奖励 p1 scene_id p2 monster_npc_id
    -- 1 扫荡 p1 scene_id
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

SCGuildTaskAllInfo = SCGuildTaskAllInfo or BaseClass(BaseProtocolStruct)
function SCGuildTaskAllInfo:__init()
    self.msg_type = 2180
end
function SCGuildTaskAllInfo:Decode()
    local GUILD_TASK_MAX_NUM = 8
    self.task_list = {}
    local statuses = {}
    local ids = {}
    local progresses = {}
    for i = 1, GUILD_TASK_MAX_NUM do
        statuses[i] = MsgAdapter.ReadChar()
    end
    for i = 1, GUILD_TASK_MAX_NUM do
        ids[i] = MsgAdapter.ReadInt()
    end
    for i = 1, GUILD_TASK_MAX_NUM do
        progresses[i] = MsgAdapter.ReadInt()
    end
    for i = 1, GuildData.Instance:GetGuildTaskNum() do
        -- 子表预装填，避免lua table的rehash
        table.insert(self.task_list, {status = statuses[i], id = ids[i], progress = progresses[i]})
    end
end

SCGuildTaskSingleInfo = SCGuildTaskSingleInfo or BaseClass(BaseProtocolStruct)
function SCGuildTaskSingleInfo:__init()
    self.msg_type = 2181
end
function SCGuildTaskSingleInfo:Decode()
    self.status = MsgAdapter.ReadInt()
    self.id = MsgAdapter.ReadInt()
    self.progress = MsgAdapter.ReadInt()
    self.is_new = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
end

CSGuildTaskReq = CSGuildTaskReq or BaseClass(BaseProtocolStruct)
function CSGuildTaskReq:__init()
    self.msg_type = 2182
end
function CSGuildTaskReq:Encode()
    -- 0接任务/提交已完成的任务 1快捷完成
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.index)
end
local function ResearchTaskInfo()
    return {
        task_id = MsgAdapter.ReadUShort(),
        progress = MsgAdapter.ReadUShort(),
        monster_id = MsgAdapter.ReadInt(),
        monster_group_id = MsgAdapter.ReadInt(),
        scene_id = MsgAdapter.ReadInt(),
        pos_x = MsgAdapter.ReadInt(),
        pos_y = MsgAdapter.ReadInt(),
    }
end

CSResearchTaskRequest = CSResearchTaskRequest or BaseClass(BaseProtocolStruct)
function CSResearchTaskRequest:__init()
    self.msg_type = 2190
end
function CSResearchTaskRequest:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
end

SCResearchTaskInfo = SCResearchTaskInfo or BaseClass(BaseProtocolStruct)
function SCResearchTaskInfo:__init()
    self.msg_type = 2191
end
function SCResearchTaskInfo:Decode()
    self.today_finish_times = MsgAdapter.ReadInt()
    MsgAdapter.ReadShort()
    self.is_new = MsgAdapter.ReadShort()
    self.task_info = ResearchTaskInfo()
end
CSResearchTaskAuto = CSResearchTaskAuto or BaseClass(BaseProtocolStruct)
function CSResearchTaskAuto:__init()
    self.msg_type = 2192
end
function CSResearchTaskAuto:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

-- 全服首杀 信息请求
CSServerFirstKillReq = CSServerFirstKillReq or BaseClass(BaseProtocolStruct)
function CSServerFirstKillReq:__init()
    self.msg_type = 2127
end

function CSServerFirstKillReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.type)          -- 玩法类型
    MsgAdapter.WriteInt(self.level_group)   -- 等级组
end

-- 全服首杀 信息下发
SCServerFirstKillInfo = SCServerFirstKillInfo or BaseClass(BaseProtocolStruct)
function SCServerFirstKillInfo:__init()
    self.msg_type = 2128
end

function SCServerFirstKillInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.info_list = {}
    for i = 1, self.count do
        local t = {
            member_count = MsgAdapter.ReadShort(),
            type = MsgAdapter.ReadShort(),
            level_group = MsgAdapter.ReadShort(),
            seq = MsgAdapter.ReadShort(),
            round_num = MsgAdapter.ReadShort(),
            boss_id = MsgAdapter.ReadInt(),
            kill_timestamp = MsgAdapter.ReadLL(),
            member_name = {},
            killers = "",
        }
        for j = 1, t.member_count do
            table.insert(t.member_name, MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN))
            t.killers = t.killers .. (j < t.member_count and (t.member_name[j] .. " ") or t.member_name[j])
        end
        table.insert(self.info_list, t)
    end
end


-- 领取赏金任务转盘奖励
CSBountyTaskTurnTableRewardFetch = CSBountyTaskTurnTableRewardFetch or BaseClass(BaseProtocolStruct)
function CSBountyTaskTurnTableRewardFetch:__init()
    self.msg_type = 2183
end

function CSBountyTaskTurnTableRewardFetch:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

-- 赏金任务转盘奖励信息
SCBountyTaskTurnTableRewardInfo = SCBountyTaskTurnTableRewardInfo or BaseClass(BaseProtocolStruct)
function SCBountyTaskTurnTableRewardInfo:__init()
    self.msg_type = 2184
end

function SCBountyTaskTurnTableRewardInfo:Decode()
    self.bounty_task_info = {
        reward_idx = MsgAdapter.ReadShort(),
        group_id = MsgAdapter.ReadShort(),
    }
end

-- 赏金任务进度信息
SCBountyTaskInfo = SCBountyTaskInfo or BaseClass(BaseProtocolStruct)
function SCBountyTaskInfo:__init()
    self.msg_type = 2185
end

function SCBountyTaskInfo:Decode()
    self.bounty_task_progress = {
        cur_task_idx = MsgAdapter.ReadShort(),              -- 当前正在做的任务 -1无效
        today_task_num = MsgAdapter.ReadShort(),            -- 今天总共要做的任务数
        has_finish_first_task = MsgAdapter.ReadShort(),     -- 是否完成了第一个任务
        reserve_sh = MsgAdapter.ReadShort(),
    }
end

-- 勋章任务进度信息
SCMedalFbKillBoss = SCMedalFbKillBoss or BaseClass(BaseProtocolStruct)
function SCMedalFbKillBoss:__init()
    self.msg_type = 2163
end 

function SCMedalFbKillBoss:Decode()
    self.scene_id = MsgAdapter.ReadShort()
end

CSMedalTaskPlayVideo = CSMedalTaskPlayVideo or BaseClass(BaseProtocolStruct)
function CSMedalTaskPlayVideo:__init()
    self.msg_type = 2165
end
function CSMedalTaskPlayVideo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.video_id)
end

SCLottreyInfo = SCLottreyInfo or BaseClass(BaseProtocolStruct)
function SCLottreyInfo:__init()
    self.msg_type = 2166
end

function SCLottreyInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.reward_list = {}
    for i = 1,self.count do
        local vo = {}
        vo.bay_type = MsgAdapter.ReadInt()
        vo.bay_index = MsgAdapter.ReadInt()
        table.insert( self.reward_list, vo )
    end 
end

SCMedalTakeOff = SCMedalTakeOff or BaseClass(BaseProtocolStruct)
function SCMedalTakeOff:__init()
    self.msg_type = 2167
end

function SCMedalTakeOff:Decode()
    self.out_column = MsgAdapter.ReadInt()
    self.out_index = MsgAdapter.ReadInt()
end

SCMedalUpgradeNotice = SCMedalUpgradeNotice or BaseClass(BaseProtocolStruct)
function SCMedalUpgradeNotice:__init()
    self.msg_type = 2168
end

function SCMedalUpgradeNotice:Decode()
    self.is_succ = MsgAdapter.ReadShort()
    self.is_wear = MsgAdapter.ReadShort()
    self.column = MsgAdapter.ReadInt()
    self.index = MsgAdapter.ReadInt()
end

--自动抓宠设置
MAX_AUTO_CATCH_NUM = 5
CSSetAutoCatchPet = CSSetAutoCatchPet or BaseClass(BaseProtocolStruct)
function CSSetAutoCatchPet:__init()
    self.msg_type = 2175
    self.setting_list = {}
end

function CSSetAutoCatchPet:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.scene_id)                -- 目标场景id
    MsgAdapter.WriteShort(self.max_seal_card_common)  -- 最大普卡使用量
    MsgAdapter.WriteShort(self.max_seal_card_silver)  -- 最大银卡使用量
    MsgAdapter.WriteShort(self.max_seal_card_gold)    -- 最大金卡使用量
    MsgAdapter.WriteShort(self.auto_catch_info_num)   -- 自动抓宠信息数
    for i=1, self.auto_catch_info_num do
        if self.setting_list[i] == nil then
            MsgAdapter.WriteInt(0)    --宠物(怪物)ID
            MsgAdapter.WriteInt(0) --最大抓宠数
        else
            MsgAdapter.WriteInt(self.setting_list[i].monster_id)    --宠物(怪物)ID
            MsgAdapter.WriteInt(self.setting_list[i].max_catch_num) --最大抓宠数
        end
    end
end

SCAutoCatchPetInfo = SCAutoCatchPetInfo or BaseClass(BaseProtocolStruct)
function SCAutoCatchPetInfo:__init()
    self.msg_type = 2176
end

function SCAutoCatchPetInfo:Decode()
    self.info = {}
    self.info.scene_id = MsgAdapter.ReadInt()
    self.info.max_seal_card_common = MsgAdapter.ReadShort()    --最大普卡使用量
    self.info.max_seal_card_silver = MsgAdapter.ReadShort()    --最大银卡使用量
    self.info.max_seal_card_gold = MsgAdapter.ReadShort()      --最大金卡使用量
    self.info.cur_seal_card_common = MsgAdapter.ReadShort()    --当前普卡使用量
    self.info.cur_seal_card_silver = MsgAdapter.ReadShort()    --当前银卡使用量
    self.info.cur_seal_card_gold = MsgAdapter.ReadShort()      --当前金卡使用量
    self.info.auto_catch_info_num = MsgAdapter.ReadInt()       --自动抓宠信息数
    self.info.info_list = {}
    for i=1,self.info.auto_catch_info_num do
        self.info.info_list[i] = {}
        self.info.info_list[i].monster_id = MsgAdapter.ReadInt()            --宠物(怪物)ID
        self.info.info_list[i].max_catch_num = MsgAdapter.ReadShort()       --最大抓宠数
        self.info.info_list[i].cur_catch_num = MsgAdapter.ReadShort()       --当前抓宠数
    end
    self.info.card_info_list = {}
    self.info.card_info_list[1] = {max_seal_num = self.info.max_seal_card_common , cur_seal_num = self.info.cur_seal_card_common}
    self.info.card_info_list[2] = {max_seal_num = self.info.max_seal_card_silver , cur_seal_num = self.info.cur_seal_card_silver}
    self.info.card_info_list[3] = {max_seal_num = self.info.max_seal_card_gold , cur_seal_num = self.info.cur_seal_card_gold}
end


SCRoleGiftGatherDreamList = SCRoleGiftGatherDreamList or BaseClass(BaseProtocolStruct)
function SCRoleGiftGatherDreamList:__init()
    self.msg_type = 2177
end

function SCRoleGiftGatherDreamList:Decode()
	MsgAdapter.WriteBegin(self.msg_type)
	self.today_is_publish_gather_dream_flag = MsgAdapter.ReadShort()
	local count = MsgAdapter.ReadShort()
    self.gift_uid_list = {}
    for i=1,count do
        self.gift_uid_list[i] = {}
        self.gift_uid_list[i].gift_uid = MsgAdapter.ReadInt()   
    end
end

SCRoleCanPublishGatherDreamList = SCRoleCanPublishGatherDreamList or BaseClass(BaseProtocolStruct)
function SCRoleCanPublishGatherDreamList:__init()
    self.msg_type = 2178
end

function SCRoleCanPublishGatherDreamList:Decode()
	 MsgAdapter.WriteBegin(self.msg_type)
	self.publish_is_send_flag = MsgAdapter.ReadShort()
	local count = MsgAdapter.ReadShort()
    self.pet_id_list = {}
    for i=1,count do
        self.pet_id_list[i] = {}
        self.pet_id_list[i].pet_id = MsgAdapter.ReadInt()   
    end
end

SCRoleGatherDreamList = SCRoleGatherDreamList or BaseClass(BaseProtocolStruct)
function SCRoleGatherDreamList:__init()
    self.msg_type = 2179
end

function SCRoleGatherDreamList:Decode()
	MsgAdapter.WriteBegin(self.msg_type)
	self.info_type = MsgAdapter.ReadShort()
	local count = MsgAdapter.ReadShort()
	if self.info_type == 0 then
		self.get_list = {}	
		for i=1, count do
			local temp = {}
			temp.role_id = MsgAdapter.ReadInt()
			temp.post = MsgAdapter.ReadInt()
			temp.from_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
			temp.avatar_id = MsgAdapter.ReadShort()
			temp.avatar_type = MsgAdapter.ReadShort()
			temp.gather_dream_type = MsgAdapter.ReadShort()
			temp.item_id = MsgAdapter.ReadUShort()
			temp.timestamp = MsgAdapter.ReadUInt()
			table.insert(self.get_list, temp)
		end
	else
		self.send_list = {}	
		for i=1, count do
			local temp = {}
			temp.role_id = MsgAdapter.ReadInt()
			temp.post = MsgAdapter.ReadInt()
			temp.from_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
			temp.avatar_id = MsgAdapter.ReadShort()
			temp.avatar_type = MsgAdapter.ReadShort()
			temp.gather_dream_type = MsgAdapter.ReadShort()
			temp.item_id = MsgAdapter.ReadUShort()
			temp.timestamp = MsgAdapter.ReadUInt()
			table.insert(self.send_list, temp)
		end	
	end
end

-- 灵骑系统请求
CSBicycleReq = CSBicycleReq or BaseClass(BaseProtocolStruct)
function CSBicycleReq:__init()
    self.msg_type = 2100
end

function CSBicycleReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)       -- op_type : BicycleCfg.OP_TYPE
    MsgAdapter.WriteInt(self.param_1)
    MsgAdapter.WriteInt(self.param_2)
    MsgAdapter.WriteInt(self.param_3)
    --LogError("灵骑系统请求", self)
end

-- 灵骑全部信息下发
SCBicycleAllInfo = SCBicycleAllInfo or BaseClass(BaseProtocolStruct)
function SCBicycleAllInfo:__init()
    self.msg_type = 2101
end

function SCBicycleAllInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.bicycle_list = {}
    for i = 1, self.count do
        local bicycle = Bicycle.DecodeToInfo()
        self.bicycle_list[bicycle.id] = bicycle
    end
end

-- 灵骑单个信息下发
SCBicycleOneInfo = SCBicycleOneInfo or BaseClass(BaseProtocolStruct)
function SCBicycleOneInfo:__init()
    self.msg_type = 2102
end

function SCBicycleOneInfo:Decode()
    self.index = MsgAdapter.ReadInt()
    self.bicycle = Bicycle.DecodeToInfo()
end

-- 灵骑属性下发
SCBicycleAttri = SCBicycleAttri or BaseClass(BaseProtocolStruct)
function SCBicycleAttri:__init()
    self.msg_type = 2103
end

function SCBicycleAttri:Decode()
    self.info = {}
    self.info.bicycle_id = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    self.info.attri_list = {}
    for i = 1,BicycleCfg.MAX_ATTR_NUM do
        self.info.attri_list[i] = {}
        self.info.attri_list[i].attri_type = MsgAdapter.ReadShort()
        self.info.attri_list[i].value = MsgAdapter.ReadShort()
    end
end

-- 灵骑御宠信息
SCBicyclePetInfo = SCBicyclePetInfo or BaseClass(BaseProtocolStruct)
function SCBicyclePetInfo:__init()
    self.msg_type = 2104
end

function SCBicyclePetInfo:Decode()
    self.info = {}
    self.info.index = MsgAdapter.ReadShort()
    self.info.pet_list = {}
    for i = 0,BicycleCfg.MAX_PET_NUM - 1 do
        self.info.pet_list[i] = MsgAdapter.ReadChar()
    end
end

-- 灵骑操作通知
SCBicycleNotice = SCBicycleNotice or BaseClass(BaseProtocolStruct)
function SCBicycleNotice:__init()
    self.msg_type = 2105
end

function SCBicycleNotice:Decode()
    self.info = {}
    self.info.notice_type = MsgAdapter.ReadShort() -- notice_type : BicycleCfg.NOTICE_TYPE
    self.info.index = MsgAdapter.ReadShort()
    self.info.param_1 = MsgAdapter.ReadShort()
    self.info.param_2 = MsgAdapter.ReadShort()
    self.info.param_3 = MsgAdapter.ReadShort()
    self.info.param_4 = MsgAdapter.ReadShort()
    --LOG("灵骑操作通知", self)
end

-- 灵骑技能洗练结果下发
SCBicycleRefineSkillInfo = SCBicycleRefineSkillInfo or BaseClass(BaseProtocolStruct)
function SCBicycleRefineSkillInfo:__init()
    self.msg_type = 2106
end

function SCBicycleRefineSkillInfo:Decode()
    self.info = {}
    self.info.index = MsgAdapter.ReadChar()
    self.info.is_refine_flag = MsgAdapter.ReadChar()  -- 1 表示有洗练内容
    self.info.new_skill_list = {}                     -- 洗练技能
    self.info.skill_list = {}
    for i = 0,BicycleCfg.MAX_BASE_SKILL_CUR_NUM - 1 do
        local skill_id = MsgAdapter.ReadUShort()
        self.info.new_skill_list[i] = skill_id
    end
    for i = 0,BicycleCfg.MAX_BASE_SKILL_CUR_NUM - 1 do
        local base_skill_info = BicycleSkill.DecodeToInfo()
        base_skill_info.bicycle_id = self.info.index + 1
        base_skill_info.skill_type = BicycleCfg.SKILL_TYPE.BASE
        base_skill_info.index = i
        self.info.skill_list[i] = base_skill_info
    end
end

-- 灵骑其它信息下发
SCBicycleOtherInfo = SCBicycleOtherInfo or BaseClass(BaseProtocolStruct)
function SCBicycleOtherInfo:__init()
    self.msg_type = 2107
end

function SCBicycleOtherInfo:Decode()
    self.is_guide_flag = MsgAdapter.ReadChar()      -- 是否完成指引
    self.ride_bicycle_index = MsgAdapter.ReadChar()       -- 玩家此时乘坐的灵骑index，-1未乘坐
    MsgAdapter.ReadShort()
end

-- 灵骑局部信息变化
SCBicycleChangeInfo = SCBicycleChangeInfo or BaseClass(BaseProtocolStruct)
function SCBicycleChangeInfo:__init()
    self.msg_type = 2108
end

function SCBicycleChangeInfo:Decode()
    self.info = {}
    self.info.change_type = MsgAdapter.ReadShort()  -- @ change_type : BicycleCfg.CHANGE_TYPE
    self.info.index = MsgAdapter.ReadShort()
    self.info.param_1 = MsgAdapter.ReadInt()
end

-- 灵骑技能相关请求
CSBicycleSkillReq = CSBicycleSkillReq or BaseClass(BaseProtocolStruct)
function CSBicycleSkillReq:__init()
    self.msg_type = 2109
end

function CSBicycleSkillReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)       -- op_type : BicycleCfg.OP_TYPE
    MsgAdapter.WriteInt(self.param_1)
    MsgAdapter.WriteInt(self.param_2)
    MsgAdapter.WriteInt(self.param_3)
    MsgAdapter.WriteInt(self.param_4)
    --LogError("灵骑技能请求", self)
end



SCMedalSpecialExchangeNotice = SCMedalSpecialExchangeNotice or BaseClass(BaseProtocolStruct)
function SCMedalSpecialExchangeNotice:__init()
    self.msg_type = 2169
end

function SCMedalSpecialExchangeNotice:Decode()
    self.info = {}
    self.info.is_wear = MsgAdapter.ReadShort()
    self.info.column = MsgAdapter.ReadShort()
    self.info.index = MsgAdapter.ReadInt()
end

CSMedalOneKeyGradeReq = CSMedalOneKeyGradeReq or BaseClass(BaseProtocolStruct)
function CSMedalOneKeyGradeReq:__init()
    self.msg_type = 2170
end

function CSMedalOneKeyGradeReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.count)
    MsgAdapter.WriteShort(self.column)
    for i = 1,10 do
        MsgAdapter.WriteUInt(self.index_flag[i] or 0)
    end 
end

CSMedalUpGradeSpecialReq = CSMedalUpGradeSpecialReq or BaseClass(BaseProtocolStruct)
function CSMedalUpGradeSpecialReq:__init()
    self.msg_type = 2171
end

function CSMedalUpGradeSpecialReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.is_wear_grid)    -- 是否穿戴 1/0
    MsgAdapter.WriteInt(self.column)          -- 穿戴不填（背包页
    MsgAdapter.WriteInt(self.index)
    --消耗·天赐令牌
    MsgAdapter.WriteInt(self.consume_column_spe)
    MsgAdapter.WriteInt(self.consume_index_spe)
    --消耗·普通令牌
    MsgAdapter.WriteInt(self.consume_column)
    MsgAdapter.WriteInt(self.consume_index)
end