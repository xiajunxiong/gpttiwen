
-- GM回复
SCGMCommand = SCGMCommand or BaseClass(BaseProtocolStruct)
function SCGMCommand:__init()
    self.msg_type = 2000
end

function SCGMCommand:Decode()
    self.type = MsgAdapter.ReadStrN(64)
    self.result = MsgAdapter.ReadStrN(1024)
end

-- 打卡水晶的开关
CSExpCrystalOp = CSExpCrystalOp or BaseClass(BaseProtocolStruct)
function CSExpCrystalOp:__init()
    self.msg_type = 2001
end

function CSExpCrystalOp:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

-- 打卡水晶的信息
SCExpCrystalInfo = SCExpCrystalInfo or BaseClass(BaseProtocolStruct)
function SCExpCrystalInfo:__init()
    self.msg_type = 2002
end

function SCExpCrystalInfo:Decode()
    self.is_open = MsgAdapter.ReadShort()
    self.count = MsgAdapter.ReadShort()
end

--遭遇an怪
SCEncounterMineMonsterFlag = SCEncounterMineMonsterFlag or BaseClass(BaseProtocolStruct)
function SCEncounterMineMonsterFlag:__init()
    self.msg_type = 2005
end

function SCEncounterMineMonsterFlag:Decode()
    self.flag = MsgAdapter.ReadInt()-- 0否1是
end

CSSetEncounterMineMonsterFlag = CSSetEncounterMineMonsterFlag or BaseClass(BaseProtocolStruct)
function CSSetEncounterMineMonsterFlag:__init()
    self.msg_type = 2030
end

function CSSetEncounterMineMonsterFlag:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.flag)-- 0否1是
end

--改变形象
CSChangeAppearance = CSChangeAppearance or BaseClass(BaseProtocolStruct)
function CSChangeAppearance:__init()
    self.msg_type = 2034
end

function CSChangeAppearance:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteChar(self.avartar_type)
    MsgAdapter.WriteChar(self.color)
    MsgAdapter.WriteShort(self.reserve_sh)
end

--改变头像
CSChangeHeadReq = CSChangeHeadReq or BaseClass(BaseProtocolStruct)
function CSChangeHeadReq:__init()
    self.msg_type = 2035
end

function CSChangeHeadReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.avatar_id)
    MsgAdapter.WriteShort(0)
end

--开启箱子获得的物品奖励信息
--主要用于判断是否要在场景的箱子上显示获得物品的模型
SCTreasureChestRewardList = SCTreasureChestRewardList or BaseClass(BaseProtocolStruct)

function SCTreasureChestRewardList:__init()
    self.msg_type = 2039
end

function SCTreasureChestRewardList:Decode()
    self.rewardInfo = {}
    self.obj_id = MsgAdapter.ReadUShort()
    local reward_count = MsgAdapter.ReadShort()
    self.add_coin = MsgAdapter.ReadInt()
    self.rewardInfo.rewardList = {}
    for i=1, reward_count do
        self.rewardInfo.rewardList[i] = {}
        self.rewardInfo.rewardList[i].item_id = MsgAdapter.ReadInt()
        self.rewardInfo.rewardList[i].num = MsgAdapter.ReadShort()
        self.rewardInfo.rewardList[i].is_bind = MsgAdapter.ReadShort()
    end
end

--========================公告=======================================
SCNoticeCfgUpdata = SCNoticeCfgUpdata or BaseClass(BaseProtocolStruct)
function SCNoticeCfgUpdata:__init()
    self.msg_type = 2041
end

function SCNoticeCfgUpdata:Decode()
    local info = {}
    info.id = MsgAdapter.ReadInt() -- 公告id
    info.is_show = MsgAdapter.ReadInt()   -- 是否显示
    info.title = MsgAdapter.ReadStrN(MsgAdapter.ReadInt())-- 标题
    info.subject = MsgAdapter.ReadStrN(MsgAdapter.ReadInt())-- 主题标题
    info.text = MsgAdapter.ReadStrN(MsgAdapter.ReadInt())-- 内容
    self.notice_info = info
end

-- 发送GM命令
CSGMCommand = CSGMCommand or BaseClass(BaseProtocolStruct)
function CSGMCommand:__init()
    self.msg_type = 2050

    self.type = "" -- 64
    self.command = "" -- 1024
end

function CSGMCommand:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteStrN(self.type, 64)
    MsgAdapter.WriteStrN(self.command, 1024)
end

-------------------宝宝信息请求-------------------
CSBabyReq = CSBabyReq or BaseClass(BaseProtocolStruct)

function CSBabyReq:__init()
    self.msg_type = 2061

    self.req_type = 0
    self.param1 = 0
end

function CSBabyReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
end

-------------------宝宝改名请求-------------------
CSBabyRename = CSBabyRename or BaseClass(BaseProtocolStruct)

function CSBabyRename:__init()
    self.msg_type = 2062
    self.change_name = ""
end

function CSBabyRename:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.baby_index)
    MsgAdapter.WriteStrN(self.change_name, PROTC_ROLE_NAME_LEN)
end

-------------------宝宝激活请求-------------------
CSBabyActive = CSBabyActive or BaseClass(BaseProtocolStruct)

function CSBabyActive:__init()
    self.msg_type = 2065
    self.req_type = 0
    self.baby_name = ""
end

function CSBabyActive:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteStrN(self.baby_name, PROTC_ROLE_NAME_LEN)
end

-------------------宝宝信息-------------------
SCBabyInfo = SCBabyInfo or BaseClass(BaseProtocolStruct)

function SCBabyInfo:__init()
    self.msg_type = 2060

    self.single_ji_fen = 0 --单人求子积分
    self.duet_ji_fen = 0 --双人求子积分
    self.draw_times = 0 --单人求子每天抽签次数
    self.worship_times = 0 --单人求子每天供奉次数
    self.duet_create_stage = 0 --双人求子阶段
    self.play_game_times = 0 --是否玩过小游戏
    self.create_type = 0 --当前求子类型 1 => 单人 2 => 双人
    self.baby_info = {} --宝宝信息
end

function SCBabyInfo:Decode()
    self.single_ji_fen = MsgAdapter.ReadUShort()
    self.duet_ji_fen = MsgAdapter.ReadUShort()
    self.draw_times = MsgAdapter.ReadUChar()
    self.worship_times = MsgAdapter.ReadUChar()
    self.duet_create_stage = MsgAdapter.ReadUChar()
    self.play_game_times = MsgAdapter.ReadUChar()
    self.create_type = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    for i = 1, BABY_MAX_NUM do
        self.baby_info[i] = SCBabyInfo.BabyDataStruct()
    end
end

function SCBabyInfo.BabyDataStruct()
    local data = {}
    MsgAdapter.ReadUInt()
    data.baby_id = MsgAdapter.ReadLL()
    data.level = MsgAdapter.ReadUShort()
    data.duet_create_score = MsgAdapter.ReadUShort() -- 双人生产积分
    data.is_duet = MsgAdapter.ReadUChar() -- 单人生产还是双人生产
    data.add_score_times = MsgAdapter.ReadUChar()
    data.create_stage = MsgAdapter.ReadUChar()
    MsgAdapter.ReadUChar()
    data.exp = MsgAdapter.ReadInt()
    data.father_id = MsgAdapter.ReadInt()
    data.mother_id = MsgAdapter.ReadInt()
    data.baby_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    data.father_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    data.mother_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    return data
end

-------------------询问对方是否进行-------------------
SCBabyReqRoute = SCBabyReqRoute or BaseClass(BaseProtocolStruct)

function SCBabyReqRoute:__init()
    self.msg_type = 2063
    self.message_type = 0 --消息类型 BABY_REQUEST_TYPE
end

function SCBabyReqRoute:Decode()
    self.message_type = MsgAdapter.ReadInt()
end

-------------------开始小游戏-------------------
SCBabyStartPlay = SCBabyStartPlay or BaseClass(BaseProtocolStruct)

function SCBabyStartPlay:__init()
    self.msg_type = 2064
    self.start_time = 0
end

function SCBabyStartPlay:Decode()
    self.start_time = MsgAdapter.ReadUInt()
end

-- 好友请求
CSFriendReq = CSFriendReq or BaseClass(BaseProtocolStruct)
function CSFriendReq:__init()
    self.msg_type = 2070
end

function CSFriendReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

local function ReadFriendInfo()
    local t = {}

    t.uid = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.family = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.love = MsgAdapter.ReadInt()
    t.sex = MsgAdapter.ReadChar()
    t.is_online = MsgAdapter.ReadChar()
    t.prof = MsgAdapter.ReadShort()
    t.level = MsgAdapter.ReadShort()
    t.vip_level = MsgAdapter.ReadChar()
    t.group_type = MsgAdapter.ReadChar()
    t.avatar_timestamp = MsgAdapter.ReadLL()
    t.avatar_type = MsgAdapter.ReadShort()
    t.avatar_id = MsgAdapter.ReadShort()--人物头像
    t.top_level = MsgAdapter.ReadInt()
    t.last_online_timestamp = MsgAdapter.ReadUInt()

    return t
end

local function ReadBlackInfo()
    local t = {}

    t.uid = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.family = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.avatar_type = MsgAdapter.ReadShort()
    t.is_online = MsgAdapter.ReadChar()
    t.vip_level = MsgAdapter.ReadChar()
    t.level = MsgAdapter.ReadShort()
    t.prof = MsgAdapter.ReadShort()
    t.avatar_id = MsgAdapter.ReadShort()--人物头像
    MsgAdapter.ReadShort()
    t.top_level = MsgAdapter.ReadInt()
    t.avatar_timestamp = MsgAdapter.ReadLL()
    t.last_online_timestamp = MsgAdapter.ReadUInt()

    return t
end

SCFriendInfo = SCFriendInfo or BaseClass(BaseProtocolStruct)
function SCFriendInfo:__init()
    self.msg_type = 2071
end

function SCFriendInfo:Decode()
    self.group_names = {}
    for i = 1, SocietyConfig.FRIEND_GROUP_CUSTOM_MAX do
        self.group_names[i] = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    end
    self.count = MsgAdapter.ReadInt()
    self.friend_list = {}
    for i = 1, self.count do
        self.friend_list[i] = ReadFriendInfo()
    end
end

SCFriendListChange = SCFriendListChange or BaseClass(BaseProtocolStruct)
function SCFriendListChange:__init()
    self.msg_type = 2072
end

function SCFriendListChange:Decode()
    self.change_type = MsgAdapter.ReadInt()
    self.friend_info = ReadFriendInfo()
end

SCAddFriendRoute = SCAddFriendRoute or BaseClass(BaseProtocolStruct)
function SCAddFriendRoute:__init()
    self.msg_type = 2073
end

function SCAddFriendRoute:Decode()
    local t = {}
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.uid = MsgAdapter.ReadInt()
    t.avatar_type = MsgAdapter.ReadShort()
    t.prof = MsgAdapter.ReadShort()
    t.level = MsgAdapter.ReadInt()
    t.avatar_id = MsgAdapter.ReadInt()--人物头像
    t.top_level = MsgAdapter.ReadInt()
    t.avatar_timestamp = MsgAdapter.ReadLL()
    self.friend_info = t
end

SCChangeBlack = SCChangeBlack or BaseClass(BaseProtocolStruct)
function SCChangeBlack:__init()
    self.msg_type = 2075
end

function SCChangeBlack:Decode()
    self.change_type = MsgAdapter.ReadInt()
    self.black_info = ReadBlackInfo()
end

SCBlackInfo = SCBlackInfo or BaseClass(BaseProtocolStruct)
function SCBlackInfo:__init()
    self.msg_type = 2076
end

function SCBlackInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.black_list = {}
    for i = 1, self.count do
        self.black_list[i] = ReadBlackInfo()
    end
end

CSCreateGroup = CSCreateGroup or BaseClass(BaseProtocolStruct)
function CSCreateGroup:__init()
    self.msg_type = 2077
end

function CSCreateGroup:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.group_type)
    MsgAdapter.WriteStrN(self.group_name, PROTC_ROLE_NAME_LEN)
    for _, uid in pairs(self.uid_list) do
        MsgAdapter.WriteInt(uid)
    end
end

CSMoveGroup = CSMoveGroup or BaseClass(BaseProtocolStruct)
function CSMoveGroup:__init()
    self.msg_type = 2078
end

function CSMoveGroup:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.group_type)
    for _, uid in pairs(self.uid_list) do
        MsgAdapter.WriteInt(uid)
    end
end

-- CSRenameGroup = CSRenameGroup or BaseClass(BaseProtocolStruct)
-- function CSRenameGroup:__init()
-- 	self.msg_type = 2079
-- end

-- function CSRenameGroup:Encode()
-- 	MsgAdapter.WriteBegin(self.msg_type)
-- 	MsgAdapter.WriteInt(self.group_type)
-- 	MsgAdapter.WriteStrN(self.group_name, PROTC_ROLE_NAME_LEN)
-- end

SCFriendGroupInfo = SCFriendGroupInfo or BaseClass(BaseProtocolStruct)
function SCFriendGroupInfo:__init()
    self.msg_type = 2080
end

function SCFriendGroupInfo:Decode()
    self.group_names = {}
    for i = 1, SocietyConfig.FRIEND_GROUP_CUSTOM_MAX do
        self.group_names[i] = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    end
end

SCRecentlyTeamInfo = SCRecentlyTeamInfo or BaseClass(BaseProtocolStruct)
function SCRecentlyTeamInfo:__init()
    self.msg_type = 2081
end

function SCRecentlyTeamInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.friend_list = {}
    for i = 1, self.count do
        --[[ local t = {}
        t.uid = MsgAdapter.ReadInt()
        t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        t.family = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        t.avatar_type = MsgAdapter.ReadShort()
        t.is_online = MsgAdapter.ReadChar()
        t.vip_level = MsgAdapter.ReadChar()
        t.level = MsgAdapter.ReadShort()
        t.prof = MsgAdapter.ReadShort()
        t.avatar_id = MsgAdapter.ReadShort()--人物头像
        MsgAdapter.ReadShort()
        t.avatar_timestamp = MsgAdapter.ReadLL()
        t.last_online_timestamp = MsgAdapter.ReadUInt() ]]
        self.friend_list[i] = ReadBlackInfo()
    end
end

SCOneRecentlyTeamInfo = SCOneRecentlyTeamInfo or BaseClass(BaseProtocolStruct)
function SCOneRecentlyTeamInfo:__init()
    self.msg_type = 2082
end

function SCOneRecentlyTeamInfo:Decode()
    self.del_role_id = MsgAdapter.ReadInt()
    --[[ local t = {}
    t.uid = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.family = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.avatar_type = MsgAdapter.ReadShort()
    t.is_online = MsgAdapter.ReadChar()
    t.vip_level = MsgAdapter.ReadChar()
    t.level = MsgAdapter.ReadShort()
    t.prof = MsgAdapter.ReadShort()
    t.avatar_id = MsgAdapter.ReadShort()--人物头像
    MsgAdapter.ReadShort()
    t.avatar_timestamp = MsgAdapter.ReadLL()
    t.last_online_timestamp = MsgAdapter.ReadUInt() ]]
    self.friend_info = ReadBlackInfo()
end

SCRecentlyChatAdd = SCRecentlyChatAdd or BaseClass(BaseProtocolStruct)
function SCRecentlyChatAdd:__init()
    self.msg_type = 2083
end

function SCRecentlyChatAdd:Decode()
    self.del_role_id = MsgAdapter.ReadInt()
    --[[ local t = {}
    t.uid = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.family = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.avatar_type = MsgAdapter.ReadShort()
    t.is_online = MsgAdapter.ReadChar()
    t.vip_level = MsgAdapter.ReadChar()
    t.level = MsgAdapter.ReadShort()
    t.prof = MsgAdapter.ReadShort()
    t.avatar_id = MsgAdapter.ReadShort()--人物头像
    MsgAdapter.ReadShort()
    t.avatar_timestamp = MsgAdapter.ReadLL()
    t.last_online_timestamp = MsgAdapter.ReadUInt() ]]
    self.friend_info = ReadBlackInfo()
end

-- 请求推荐好友列表
CSFriendRecommendReq = CSFriendRecommendReq or BaseClass(BaseProtocolStruct)
function CSFriendRecommendReq:__init()
    self.msg_type = 2090
end

function CSFriendRecommendReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

-- 发送推荐好友列表
SCFriendRecommendList = SCFriendRecommendList or BaseClass(BaseProtocolStruct)
function SCFriendRecommendList:__init()
    self.msg_type = 2091
end

function SCFriendRecommendList:Decode()
    self.friend_info_list = {}
    for i = 1, SocietyConfig.FRIEND_ADD_NUM_MAX do
        local t = {}
        t.uid = MsgAdapter.ReadInt()
        t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        t.level = MsgAdapter.ReadInt()
        t.top_level = MsgAdapter.ReadInt()
        t.avatar = MsgAdapter.ReadChar()
        t.prof = MsgAdapter.ReadChar()
        t.avatar_type = MsgAdapter.ReadShort()
        t.avatar_id = MsgAdapter.ReadShort()--人物头像
        MsgAdapter.ReadShort()
        t.avatar_timestamp = MsgAdapter.ReadLL()
        self.friend_info_list[i] = t
    end
end

-- 添加所有好友请求
CSFriendRecommendAddOnListReq = CSFriendRecommendAddOnListReq or BaseClass(BaseProtocolStruct)
function CSFriendRecommendAddOnListReq:__init()
    self.msg_type = 2092
end

function CSFriendRecommendAddOnListReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.is_add_all) -- 1是添加所有; 0是添加一个,下面add_index起作用
    MsgAdapter.WriteShort(self.add_index)
end

-- 下发一个推荐好友
SCFriendRecommendOne = SCFriendRecommendOne or BaseClass(BaseProtocolStruct)
function SCFriendRecommendOne:__init()
    self.msg_type = 2093
end

function SCFriendRecommendOne:Decode()
    local t = {}
    t.uid = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.level = MsgAdapter.ReadInt()
    t.avatar = MsgAdapter.ReadChar()
    t.prof = MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    t.avatar_timestamp = MsgAdapter.ReadLL()
    self.friend_info = t
end

-- 推荐好友列表换一批
CSRefreshFriendRecommend = CSRefreshFriendRecommend or BaseClass(BaseProtocolStruct)
function CSRefreshFriendRecommend:__init()
    self.msg_type = 2094
end

function CSRefreshFriendRecommend:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

CSFuzzySearchRoleByName = CSFuzzySearchRoleByName or BaseClass(BaseProtocolStruct)
function CSFuzzySearchRoleByName:__init()
    self.msg_type = 2095
end

function CSFuzzySearchRoleByName:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteStrN(self.name, PROTC_ROLE_NAME_LEN)
end

SCFuzzySearchRolesInfo = SCFuzzySearchRolesInfo or BaseClass(BaseProtocolStruct)
function SCFuzzySearchRolesInfo:__init()
    self.msg_type = 2096
end

function SCFuzzySearchRolesInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.friend_info_list = {}
    for i = 1, self.count do
        local t = {}
        t.uid = MsgAdapter.ReadInt()
        t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        t.avatar_type = MsgAdapter.ReadShort()
        t.prof = MsgAdapter.ReadShort()
        t.avatar_id = MsgAdapter.ReadInt()--人物头像
        t.level = MsgAdapter.ReadInt()
        t.top_level = MsgAdapter.ReadInt()
        t.avatar_timestamp = MsgAdapter.ReadLL()
        self.friend_info_list[i] = t
    end
end

-- 拜师/收徒请求
CSShiTuMakeRelationReq = CSShiTuMakeRelationReq or BaseClass(BaseProtocolStruct)
function CSShiTuMakeRelationReq:__init()
    self.msg_type = 2110
end

function CSShiTuMakeRelationReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.type)
    MsgAdapter.WriteInt(self.uid)
end

-- 回复拜师或收徒
CSShiTuMakeRelationAck = CSShiTuMakeRelationAck or BaseClass(BaseProtocolStruct)
function CSShiTuMakeRelationAck:__init()
    self.msg_type = 2111
end

function CSShiTuMakeRelationAck:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUInt(self.type)
    MsgAdapter.WriteInt(self.uid)
    MsgAdapter.WriteInt(self.is_agree)
end

-- 毕业或出师请求
CSShiTuBreakRelationReq = CSShiTuBreakRelationReq or BaseClass(BaseProtocolStruct)
function CSShiTuBreakRelationReq:__init()
    self.msg_type = 2112
end

function CSShiTuBreakRelationReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.type)-- 0解除关系 1 强制解除关系 2冷静期取消解除关系
    MsgAdapter.WriteInt(self.relation_type)--师或徒
    MsgAdapter.WriteInt(self.uid)
    -- MsgAdapter.WriteShort(self.is_force)  --少于30天可以申请强制解除
    MsgAdapter.WriteInt(self.is_consume_gold) --强制解除是否使用元宝
    -- LogDG("发送师徒操作协议", self)
end

-- 拜师或收徒 请求转发
SCShiTuMakeRelationRoute = SCShiTuMakeRelationRoute or BaseClass(BaseProtocolStruct)
function SCShiTuMakeRelationRoute:__init()
    self.msg_type = 2113
end

function SCShiTuMakeRelationRoute:Decode()
    self.type = MsgAdapter.ReadInt()
    local t = {}
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.uid = MsgAdapter.ReadInt()
    t.level = MsgAdapter.ReadInt()
    t.sex = MsgAdapter.ReadShort()
    t.prof = MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    t.avatar_type = MsgAdapter.ReadShort()
    t.avatar_id = MsgAdapter.ReadShort()--人物头像
    t.type = self.type
    self.role_info = t
end

local function ReadShiTuInfo()
    local t = {}

    t.family = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.uid = MsgAdapter.ReadInt()
    t.level = MsgAdapter.ReadInt()
    t.sex = MsgAdapter.ReadShort()
    MsgAdapter.ReadChar()
    t.is_online = MsgAdapter.ReadChar()
    t.prof = MsgAdapter.ReadInt()
    t.avatar_type = MsgAdapter.ReadShort()
    t.avatar_id = MsgAdapter.ReadShort()--人物头像
    t.shitu_time = MsgAdapter.ReadUInt()
    t.login_time = MsgAdapter.ReadUInt()            -- 上次登录时间

    t.apprentice_old_cap = MsgAdapter.ReadInt()     -- 拜师时 徒弟的战力
    t.apprentice_cap = MsgAdapter.ReadInt()         -- 当前徒弟战力
    t.apprentice_cap_value = math.max(t.apprentice_cap - t.apprentice_old_cap, 0)
    t.mile_reward_flag = MsgAdapter.ReadInt()       -- 里程碑奖励标志[序号]
    t.relation_break_time_type = MsgAdapter.ReadUInt() --1是强制解除 0是正常解除，对方不在线或拒绝
    t.relation_break_time = MsgAdapter.ReadUInt()   -- 解除关系倒计时
    -- LogDG("师徒列表信息", t.name, t)
    return t
end

-- 师徒信息
SCShiTuRelationInfo = SCShiTuRelationInfo or BaseClass(BaseProtocolStruct)
function SCShiTuRelationInfo:__init()
    self.msg_type = 2114
end

function SCShiTuRelationInfo:Decode()
    self.master_info_list = {}
    self.apprentice_info_list = {}
    for i = 1, SocietyConfig.MASTER_COUNT_MAX do
        self.master_info_list[i] = ReadShiTuInfo()
    end
    self.apprentice_count = MsgAdapter.ReadInt()
    for i = 1, self.apprentice_count do
        self.apprentice_info_list[i] = ReadShiTuInfo()
    end
end

-- 毕业或出师 请求转发
SCShiTuBreakRelationRoute = SCShiTuBreakRelationRoute or BaseClass(BaseProtocolStruct)
function SCShiTuBreakRelationRoute:__init()
    self.msg_type = 2115
end

function SCShiTuBreakRelationRoute:Decode()
    self.type = MsgAdapter.ReadUInt()
    local t = {}
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.uid = MsgAdapter.ReadInt()
    t.level = MsgAdapter.ReadInt()
    t.sex = MsgAdapter.ReadShort()
    t.prof = MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    t.avatar_timestamp = MsgAdapter.ReadLL()
    self.role_info = t
end

-- 回复毕业或出师
CSShiTuBreakRelationAck = CSShiTuBreakRelationAck or BaseClass(BaseProtocolStruct)
function CSShiTuBreakRelationAck:__init()
    self.msg_type = 2116
end

function CSShiTuBreakRelationAck:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUInt(self.type)
    MsgAdapter.WriteInt(self.uid)
    MsgAdapter.WriteInt(self.is_agree)
end

-- 请求师徒信息
CSShiTuReq = CSShiTuReq or BaseClass(BaseProtocolStruct)
function CSShiTuReq:__init()
    self.msg_type = 2117
end

function CSShiTuReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
end

-- 师徒副本场景怪物信息
SCShiTuFbSceneInfo = SCShiTuFbSceneInfo or BaseClass(BaseProtocolStruct)
function SCShiTuFbSceneInfo:__init()
    self.msg_type = 2118
end

function SCShiTuFbSceneInfo:Decode()
    local t = {
        end_time = MsgAdapter.ReadUInt(),
        count = MsgAdapter.ReadInt(),
        monsters = {}
    }
    for i = 1, t.count do
        t.monsters[i] = {
            group = MsgAdapter.ReadShort(),
            index = MsgAdapter.ReadShort(),
            is_kill = MsgAdapter.ReadShort(),
            obj_id = MsgAdapter.ReadShort(),
            pos_x = MsgAdapter.ReadInt(),
            pos_y = MsgAdapter.ReadInt(),
        }
    end
    self.info = t
end

-- 师徒副本其他信息
SCShiTuFbCommonInfo = SCShiTuFbCommonInfo or BaseClass(BaseProtocolStruct)
function SCShiTuFbCommonInfo:__init()
    self.msg_type = 2119
end

function SCShiTuFbCommonInfo:Decode()
    self.info = {
        is_get_shi_tu_reward = MsgAdapter.ReadInt(),        -- 是否已经获取过师徒副本奖励

        day_mile_cap_fetch_num = MsgAdapter.ReadInt(),          -- 今日战力里程 领取次数
        day_mile_pass_day_fetch_num = MsgAdapter.ReadInt(),     -- 今日拜师天数里程 领取次数

        week_chuan_gong_flag = MsgAdapter.ReadInt(),        -- 本周传功奖励标记
        week_chuan_gong_exp_flag = MsgAdapter.ReadInt(),    -- 本周传功经验标记
        week_chuan_gong_score_max = MsgAdapter.ReadInt(),   -- 本周传功最高得分
        mile_reward_flag = MsgAdapter.ReadLL(),
    }
end

-- 师徒副本组队邀请
SCShiTuFbInvitationNotice = SCShiTuFbInvitationNotice or BaseClass(BaseProtocolStruct)
function SCShiTuFbInvitationNotice:__init()
    self.msg_type = 2120
end

function SCShiTuFbInvitationNotice:Decode()
    self.info = {
        name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        team_index = MsgAdapter.ReadInt(),
        uid = MsgAdapter.ReadInt(),
        is_cross = MsgAdapter.ReadInt(),
    }
end

-- 结算通知
SCShiTuFbFinishNotice = SCShiTuFbFinishNotice or BaseClass(BaseProtocolStruct)
function SCShiTuFbFinishNotice:__init()
    self.msg_type = 2121
end

function SCShiTuFbFinishNotice:Decode()
    self.finish_group = MsgAdapter.ReadInt()
end

-- 战斗通知
SCShiTuFbBattleResult = SCShiTuFbBattleResult or BaseClass(BaseProtocolStruct)
function SCShiTuFbBattleResult:__init()
    self.msg_type = 2122
end

function SCShiTuFbBattleResult:Decode()
    self.info = {
        result = MsgAdapter.ReadInt(),
        group = MsgAdapter.ReadInt(),
        index = MsgAdapter.ReadInt(),
    }
end

CSShiTuSeekingReq = CSShiTuSeekingReq or BaseClass(BaseProtocolStruct)
function CSShiTuSeekingReq:__init()
    self.msg_type = 2125
end

function CSShiTuSeekingReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.seeking_type)
    for i = 1, SocietyConfig.PREFERENCE_NUM do
        MsgAdapter.WriteChar(self.maq_info[i] or 0)
    end
    MsgAdapter.WriteStrN(self.seeking_notice, SHITU_SEEKING_NOTICE)
end

SCShiTuSeekingInfo = SCShiTuSeekingInfo or BaseClass(BaseProtocolStruct)
function SCShiTuSeekingInfo:__init()
    self.msg_type = 2126
end

function SCShiTuSeekingInfo:Decode()
    self.seeking_notice = MsgAdapter.ReadStrN(SHITU_SEEKING_NOTICE) -- 自己的宣言
    self.seeking_type = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.role_info_list = {}
    for i = 1, self.count do
        local t = {}
        t.family = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        t.uid = MsgAdapter.ReadInt()
        MsgAdapter.ReadChar()
        t.sex = MsgAdapter.ReadChar()
        t.prof = MsgAdapter.ReadChar()
        t.is_online = MsgAdapter.ReadChar()
        t.level = MsgAdapter.ReadShort()
        t.avatar_id = MsgAdapter.ReadShort()--人物头像
        t.timestamp = MsgAdapter.ReadUInt()
        t.avatar_type = MsgAdapter.ReadInt()
        t.seeking_notice = MsgAdapter.ReadStrN(SHITU_SEEKING_NOTICE)
        t.seeking_type = self.seeking_type
        t.prof_b = RoleData.ProfIdToProfType(t.prof)
        self.role_info_list[i] = t
    end
end

SCShiTuPreferenceInfo = SCShiTuPreferenceInfo or BaseClass(BaseProtocolStruct)
function SCShiTuPreferenceInfo:__init()
    self.msg_type = 2124
end

function SCShiTuPreferenceInfo:Decode()
    self.maq_info = {}
    for i = 1, SocietyConfig.PREFERENCE_NUM do
        self.maq_info[i] = MsgAdapter.ReadChar()
    end
end

-- 角色前排功能
CSSetBattlePosition = CSSetBattlePosition or BaseClass(BaseProtocolStruct)
function CSSetBattlePosition:__init()
    self.msg_type = 2031
end

function CSSetBattlePosition:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.position) -- 主角站位 0 = 后排 ，1 = 前排
end

SCSetBattlePosition = SCSetBattlePosition or BaseClass(BaseProtocolStruct)
function SCSetBattlePosition:__init()
    self.msg_type = 2006
end

function SCSetBattlePosition:Decode()
    self.battle_position = MsgAdapter.ReadInt()
end

-- 本地设置主角自动战斗的行为
CSSetBattleAutoMove = CSSetBattleAutoMove or BaseClass(BaseProtocolStruct)
function CSSetBattleAutoMove:__init()
    self.msg_type = 2033
end

function CSSetBattleAutoMove:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.first_move_type) -- 行为类型
    MsgAdapter.WriteShort(self.first_skill_id) -- 技能ID
    MsgAdapter.WriteShort(self.first_skill_level) -- 技能等级
    MsgAdapter.WriteShort(self.second_move_type) -- 行为类型
    MsgAdapter.WriteShort(self.second_skill_id) -- 技能ID
    MsgAdapter.WriteShort(self.second_skill_level) -- 技能等级
end

SCBattleAutoInfo = SCBattleAutoInfo or BaseClass(BaseProtocolStruct)
function SCBattleAutoInfo:__init()
    self.msg_type = 2007
end

function SCBattleAutoInfo:Decode()
    self.is_auto = MsgAdapter.ReadInt() -- 当前是否自动
    self.first_move_type = MsgAdapter.ReadShort()
    self.first_skill_id = MsgAdapter.ReadShort()
    self.first_skill_level = MsgAdapter.ReadShort()
    self.second_move_type = MsgAdapter.ReadShort()
    self.second_skill_id = MsgAdapter.ReadShort()
    self.second_skill_level = MsgAdapter.ReadShort()
end

-- 许愿池 许愿
CSTreviFountainOp = CSTreviFountainOp or BaseClass(BaseProtocolStruct)
function CSTreviFountainOp:__init()
    self.msg_type = 2003
end

function CSTreviFountainOp:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

-- 许愿池信息
SCTreviFountainInfo = SCTreviFountainInfo or BaseClass(BaseProtocolStruct)
function SCTreviFountainInfo:__init()
    self.msg_type = 2004
end

function SCTreviFountainInfo:Decode()
    self.trevi_fountain_list = {}
    self.use_count = MsgAdapter.ReadInt()
    for i=1,self.use_count do
        local info = {}
        info.buff_id = MsgAdapter.ReadUInt()
        info.end_time = MsgAdapter.ReadUInt()
        self.trevi_fountain_list[info.end_time] = info
    end
    self.info = {use_count = self.use_count}
end

-- 任务追踪状态
CSTaskTraceStatus = CSTaskTraceStatus or BaseClass(BaseProtocolStruct)
function CSTaskTraceStatus:__init()
    self.msg_type = 2008
end

function CSTaskTraceStatus:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(#self.list)
    for i = 1, #self.list do
		MsgAdapter.WriteUInt(self.list[i])
    end
end

SCTaskTraceStatus = SCTaskTraceStatus or BaseClass(BaseProtocolStruct)
function SCTaskTraceStatus:__init()
    self.msg_type = 2009
end

function SCTaskTraceStatus:Decode()
	self.count = MsgAdapter.ReadInt()
	self.list = {}
    for i = 1, self.count do
		self.list[i] = MsgAdapter.ReadUInt()
    end
end

-- 宝箱通知 通知附近的玩家 宝箱被打开
SCNoticeNearbyOnOpenTreasureChest  = SCNoticeNearbyOnOpenTreasureChest or BaseClass(BaseProtocolStruct)
function SCNoticeNearbyOnOpenTreasureChest:__init()
    self.msg_type = 2040
end

function SCNoticeNearbyOnOpenTreasureChest:Decode()
	self.obj_id = MsgAdapter.ReadUShort()
	MsgAdapter.ReadShort()
end

-- --送花收到某某人的协议(屏蔽原先功能)
-- SCGiveFlowerNotice = SCGiveFlowerNotice or BaseClass(BaseProtocolStruct)
-- function SCGiveFlowerNotice:__init()
--     self.msg_type = 2097
-- end

-- function SCGiveFlowerNotice:Decode()
--     self.giver_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
--     self.seq = MsgAdapter.ReadInt()
-- end

--送花收到某某人的协议
SCGiveFlowerNotice = SCGiveFlowerNotice or BaseClass(BaseProtocolStruct)
function SCGiveFlowerNotice:__init()
    self.msg_type = 2097
end

function SCGiveFlowerNotice:Decode()
    self.info = {
        giver_id = MsgAdapter.ReadInt(),                            --赠送人ID
        giver_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),      --赠与人的名字
        receiver_id = MsgAdapter.ReadInt(),                         --接收人ID
        receiver_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),   --接收人的名字
        give_type = MsgAdapter.ReadInt(),                           --送花的类型(只代表那些有特效的)
    }
end

-- 查询玩家装备信息
CSReferRoleEquipment = CSReferRoleEquipment or BaseClass(BaseProtocolStruct)
function CSReferRoleEquipment:__init()
    self.msg_type = 2010
end

function CSReferRoleEquipment:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.role_id)
    MsgAdapter.WriteInt(self.query) -----1 下发2025协议  0下发2011协议
end

local function petQueryInfo()
    local t = {}
    t.index = MsgAdapter.ReadInt()
    t.pet_id = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.str_level = MsgAdapter.ReadUShort() -- 强化等级
    t.level = MsgAdapter.ReadUShort() -- 等级
    t.exp = MsgAdapter.ReadLL() -- 经验值
    t.hp = MsgAdapter.ReadInt()
    t.mp = MsgAdapter.ReadInt()
    t.comprehead_gift_value = MsgAdapter.ReadInt()
    t.skill_list = {} -- 技能列表 PET_SKILL_MAX_COUNT=10 技能等级*10000 + 技能id
    -- for i = 1, PET_SKILL_MAX_COUNT do
    --     t.skill_list[i] = MsgAdapter.ReadInt()
    -- end
    --改版后skill_list只有一个主动技能故写死读一个
    t.skill_list[1] = MsgAdapter.ReadInt()
    t.gift_list = {} -- 当前的天赋列表 PET_GIFT_MAX_COUNT=8
    for i = 1, PET_GIFT_MAX_COUNT do
        local gift_data = {}
        --天赋技能id
        gift_data.gift_id = MsgAdapter.ReadUShort()
        --天赋来源（0：洗炼出来的自带的，1：开领悟格子送的）
        gift_data.gift_source = MsgAdapter.ReadUChar()
        MsgAdapter.ReadUChar()
        -- if gift_data.gift_id > 0 then
            t.gift_list[i] = gift_data
        -- end
     --保留字段，空读
    end
    -- t.new_gift_list = {} -- 刷新后的天赋列表 未保存的
    -- for i = 1, PET_GIFT_MAX_COUNT do
    --     t.new_gift_list[i] = MsgAdapter.ReadUShort()
    -- end
    -- t.today_refresh_gift_count = MsgAdapter.ReadUShort() --今日刷新天赋次数
    t.lock = MsgAdapter.ReadUChar() --0为默认没有上锁  1 为上锁了
    -- 资质
    t.con_growth = MsgAdapter.ReadUChar()
    t.str_growth = MsgAdapter.ReadUChar()
    t.def_growth = MsgAdapter.ReadUChar()
    t.agi_growth = MsgAdapter.ReadUChar()
    t.magic_growth = MsgAdapter.ReadUChar()
    -- 已分配属性点
    t.con = MsgAdapter.ReadUShort()
    t.str = MsgAdapter.ReadUShort()
    t.def = MsgAdapter.ReadUShort()
    t.agi = MsgAdapter.ReadUShort()
    t.magic = MsgAdapter.ReadUShort()
    -- 洗炼后天赋和资质
    -- t.new_gift_list = {}
    -- for i = 1, PET_GIFT_MAX_COUNT do
    --     t.new_gift_list[i] = MsgAdapter.ReadInt()
    -- end

    -- t.new_con_growth = MsgAdapter.ReadUChar()
    -- t.new_str_growth = MsgAdapter.ReadUChar()
    -- t.new_def_growth = MsgAdapter.ReadUChar()
    -- t.new_agi_growth = MsgAdapter.ReadUChar()
    -- t.new_magic_growth = MsgAdapter.ReadUChar()
    t.is_bind = MsgAdapter.ReadUShort()

    t.is_fight = MsgAdapter.ReadUShort()

    return t
end

function MedalPro()
    local medal_info = {}
    medal_info.item_id = MsgAdapter.ReadUShort()
    medal_info.index = MsgAdapter.ReadShort()
    medal_info.num = MsgAdapter.ReadShort()
    medal_info.is_bind = MsgAdapter.ReadShort()
    medal_info.medal_unique_id_1 = MsgAdapter.ReadUInt()
    medal_info.medal_unique_id_2 = MsgAdapter.ReadUInt()
    medal_info.param = MedalParam()
    return medal_info
end

-- 查询玩家装备信息下发
MAX_QUERY_ATTR_NUM = 7
SCRoleEquipmentInfor = SCRoleEquipmentInfor or BaseClass(BaseProtocolStruct)
function SCRoleEquipmentInfor:__init()
    self.msg_type = 2011
end

function SCRoleEquipmentInfor:Decode()
    local info = {}
    info.role_id = MsgAdapter.ReadInt()   
    info.uid =  info.role_id                     
    info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)  --玩家名字
    info.name = info.role_name
    info.guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)--家族名字
    info.avatar_type = MsgAdapter.ReadUInt()                   --角色类型
    info.prof = MsgAdapter.ReadUInt()                          --职业
    info.level = MsgAdapter.ReadUInt()                         --人物等级
    info.top_level = MsgAdapter.ReadInt()                      --巅峰等级
    info.capability = MsgAdapter.ReadInt()                     --玩家战力
    info.battlefield_score = MsgAdapter.ReadInt()              --幻界战场积分
    local pet_count = MsgAdapter.ReadShort()                    
    local partner_count = MsgAdapter.ReadShort()
    local medal_count = MsgAdapter.ReadShort()
    info.is_finish_top = MsgAdapter.ReadChar()                 --是否完成定级赛（幻界战场）
    MsgAdapter.ReadChar()
    info.kill_value = MsgAdapter.ReadInt()                     --杀戮值
    local accrssory_count = MsgAdapter.ReadInt()                --配饰数量
    local bicycle_count = MsgAdapter.ReadShort()                --灵骑数量
    local sheng_qi_count = MsgAdapter.ReadShort()               --圣器数量
    info.achieve_info = {}
    info.achieve_info.level = MsgAdapter.ReadInt()             --当前历程等级
    info.achieve_info.num = MsgAdapter.ReadInt()               --当前历程点
    info.face_score_level = MsgAdapter.ReadInt()               --颜值等级
    info.attribute_list = {}
    for i=0,MAX_QUERY_ATTR_NUM - 1 do
        info.attribute_list[i] = MsgAdapter.ReadInt()
    end
    info.marry_info = {}
    info.marry_info.role_id = MsgAdapter.ReadInt()                      --情缘角色id
    info.marry_info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)--情缘角色名字
    info.swon_info = PortocolCommon.SwornName() --结义组名字
    info.swon_info.role_info = {info.name}
    for i=1,SWORN_PREFIX_ROLE_MAX_NUM do
        local swon_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        if swon_name ~= "" then
            table.insert(info.swon_info.role_info,swon_name)--结义组成员名字
        end
    end
    self.appearance = Appearance() -- 外观信息
    info.appearance = self.appearance
    --装备信息
    self.equipment_list = {}
    for i = 1, EQUIP_LIST_MAX do
        local equip_info = InfoType()
        self.equipment_list[equip_info.index] = equip_info
    end
    LogDG("装备信息", self.equipment_list)
    --灵饰信息
    self.jewelry_list = {}
    for i = 1, JEWELTY_LIST_MAX do
        local info = JewelryPro()
        self.jewelry_list[info.jewelry_type] = info
    end
    LogDG("灵饰信息", self.jewelry_list)
    --颜值列表
    self.face_equip_list = {}
    for i = 1,FACE_ADDITION_TYPE_MAX do
        table.insert(self.face_equip_list,MsgAdapter.ReadInt())
    end
    --宠物信息
    self.pet_list = {}
    for i=1,pet_count do
        local pet = PortocolCommon.PetProParam()
        pet.rank = MsgAdapter.ReadInt()
        -- 宠物属性
        local attr_list = {}
        local is_ok = false
        for i=0,MAX_QUERY_ATTR_NUM - 1 do
            local value = MsgAdapter.ReadInt()
            attr_list[QUERY_ATTR_TYPE[i]] = value
            if value > 0 then
                is_ok = true
            end
        end
        if is_ok then
            pet.attr_list = attr_list
        end
        -- 宠物神印
        local info = {stamp_obj_list = {}}
        for j = 1, PetStampCfg.StampMaxPos do
            MsgAdapter.ReadChar()
            MsgAdapter.ReadChar()
            MsgAdapter.ReadShort()
            local stamp_info = PetStamp.DecodeInfo()
            if stamp_info.item_id > 0 then
                local stamp = PetStamp.New(stamp_info)
                info.stamp_obj_list[j] = stamp
            end
        end
        pet.stamp_blueprint_info = info
        table.insert(self.pet_list,pet)
    end
    --伙伴信息
    self.partner_list = {}
    for i=1,partner_count do
        local partner_info = {}
        partner_info.partner_id = MsgAdapter.ReadUShort()
        partner_info.upgrade_lv = MsgAdapter.ReadUChar()
        partner_info.quality = MsgAdapter.ReadUChar()
        partner_info.capability = MsgAdapter.ReadInt()
        partner_info.skin_id = MsgAdapter.ReadShort()
        partner_info.quality_segment =  MsgAdapter.ReadChar()
        partner_info.intensify_level = MsgAdapter.ReadChar() + 1
        partner_info.level = MsgAdapter.ReadShort()
        partner_info.soar_count = MsgAdapter.ReadShort()        --伙伴飞升次数
        partner_info.soar_weapon_level = MsgAdapter.ReadShort()   --伙伴飞升武器阶数
        partner_info.rank = MsgAdapter.ReadShort() + 1
        self.partner_list[i] = partner_info
    end
    --勋章信息
    self.medal_list = {}
    for i=1,medal_count do
        self.medal_list[i] = MedalPro()
    end
    self.query_role_info = info
    -- 配饰信息
    self.accrssory_info = {}
    self.accrssory_info.equipment_list = {}
    for i = 1, accrssory_count do
        AccrssoryData.Instance:SetEquipDataInfo(Accrssory.DecodeInfo(), self.accrssory_info.equipment_list, false, false)
    end
    --灵骑列表
    self.bicycle_list = {}
    for i = 1, bicycle_count do
        self.bicycle_list[i] = Bicycle.RoleDecodeToInfo()
    end

    self.sheng_qi_list = {}
    for i = 1, sheng_qi_count do
        local vo = {}
        vo.item_id = MsgAdapter.ReadUShort()
        MsgAdapter.ReadShort()
        vo.param = ShengQi.DecodeParam()
        local sheng_qi = ShengQi.New(vo)
        table.insert(self.sheng_qi_list,sheng_qi)
    end
    ShengQiData.Instance:BagSort(self.sheng_qi_list)
end

-- 人物查询后续信息追加下发
SCQueryRoleFollowUpInfo = SCQueryRoleFollowUpInfo or BaseClass(BaseProtocolStruct)
function SCQueryRoleFollowUpInfo:__init()
    self.msg_type = 14060
end

function SCQueryRoleFollowUpInfo:Decode()
    self.comp_rank = MsgAdapter.ReadInt() -- 全服综合排名，跨服没开时下发本服，-1表示没上榜
    self.prof_rank = MsgAdapter.ReadInt() -- 全服职业排名
end

SCReplyRoleIsOnline = SCReplyRoleIsOnline or BaseClass(BaseProtocolStruct)
function SCReplyRoleIsOnline:__init()
    self.msg_type = 2085
end
function SCReplyRoleIsOnline:Decode()
    self.role_id = MsgAdapter.ReadInt()
    self.online = MsgAdapter.ReadInt()
end

-- 请求玩家工资信息
CSRoleSalaryInfo = CSRoleSalaryInfo or BaseClass(BaseProtocolStruct)
function CSRoleSalaryInfo:__init()
    self.msg_type = 2012
end

function CSRoleSalaryInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.salary_type)
end

-- 玩家工资信息返回
SCRoleSalaryInfo = SCRoleSalaryInfo or BaseClass(BaseProtocolStruct)
function SCRoleSalaryInfo:__init()
    self.msg_type = 2013
end

function SCRoleSalaryInfo:Decode()
    self.salary_type = MsgAdapter.ReadChar()
    self.reason_type = MsgAdapter.ReadChar()
    self.salary_size = MsgAdapter.ReadShort()
    self.total_value = MsgAdapter.ReadLL()
    self.salary_list = {}
    for i = 1, self.salary_size do
        self.salary_list[i] = {
            seq = MsgAdapter.ReadShort(),
            times = MsgAdapter.ReadShort(),
            total_salary = MsgAdapter.ReadInt(),
        }
    end
end

-- 玩家工资信息返回
SCRoleSalaryDay = SCRoleSalaryDay or BaseClass(BaseProtocolStruct)
function SCRoleSalaryDay:__init()
    self.msg_type = 2045
end

function SCRoleSalaryDay:Decode()
    self.salary_type = MsgAdapter.ReadInt()
    self.day_value = MsgAdapter.ReadInt()
end

-- 喇叭喊话
CSSpeaker = CSSpeaker or BaseClass(BaseProtocolStruct)
function CSSpeaker:__init()
    self.msg_type = 2014
end

function CSSpeaker:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteChar(self.speaker_type)
	MsgAdapter.WriteChar(0)
	MsgAdapter.WriteShort(0)
	MsgAdapter.WriteUInt(self.msg_length)
	MsgAdapter.WriteStrN(self.content, self.msg_length)
end

SCSpeaker = SCSpeaker or BaseClass(BaseProtocolStruct)
function SCSpeaker:__init()
    self.msg_type = 2015
end

function SCSpeaker:Decode()
    local t = {}
    t.from_uid = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.plat_name = MsgAdapter.ReadStrN(PROTC_PLAT_NAME_LEN)

    t.server_id = MsgAdapter.ReadInt()
    t.speaker_type = MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()

    t.msg_length = MsgAdapter.ReadUInt()
    t.content = MsgAdapter.ReadStrN(t.msg_length)
    self.info = t
end

SCTaskChainMonsterInfo = SCTaskChainMonsterInfo or BaseClass(BaseProtocolStruct)
function SCTaskChainMonsterInfo:__init()
    self.msg_type = 2016
end

function SCTaskChainMonsterInfo:Decode()
    self.monster_vo = {
        monster_id = MsgAdapter.ReadInt(),
        monster_group_id = MsgAdapter.ReadInt(),
        scene_id = MsgAdapter.ReadInt(),
        pos_x = MsgAdapter.ReadInt(),
        pos_y = MsgAdapter.ReadInt(),
    }
end

CSRolePatrol = CSRolePatrol or BaseClass(BaseProtocolStruct)
function CSRolePatrol:__init()
    self.msg_type = 2017
end

function CSRolePatrol:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.is_start_or_stop)      -- 0停止巡逻 1开始巡逻
end

--==================================充值相关============================================
CSRechargeConfigInfoReq = CSRechargeConfigInfoReq or BaseClass(BaseProtocolStruct)
function CSRechargeConfigInfoReq:__init()
    self.msg_type = 2018
end

function CSRechargeConfigInfoReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.currency_type)--金额类型（0 ：人民币 1：新台币 2 ：美金）
    MsgAdapter.WriteStrN(self.spid,4)      --根据客户端显示的spid 来做
end

SCRechargeConfigInfo = SCRechargeConfigInfo or BaseClass(BaseProtocolStruct)
function SCRechargeConfigInfo:__init()
	self.msg_type = 2042	
end

function SCRechargeConfigInfo:Decode()
    self.recharge_list = {} 
	self.currency_type = MsgAdapter.ReadShort()       --金额类型（0 ：人民币 1：新台币 2 ：美金）
	for i = 1,MsgAdapter.ReadShort() do
        local info = {}
        info.index = i                                --客户端序号
		info.seq = MsgAdapter.ReadChar()              --序号seq
		info.extra_reward_type = MsgAdapter.ReadChar()--额外奖励类型（0：不奖励 1：绑元 2：至尊会员 3 ：铜币 4：元宝）
		MsgAdapter.ReadShort()
		info.add_gold = MsgAdapter.ReadInt()          --加多少元宝
		info.extra_reward = MsgAdapter.ReadInt()      --额外送的奖励数量
		local money = MsgAdapter.ReadInt()            --100倍的金额 需要除以100
		info.money = money / 100                      --处理服务器将金额乘以100
        info.description_first_recharge = MsgAdapter.ReadStrN(128)--首次充值描述
        self.recharge_list[i] = info
    end
end

-- 收到这个协议需要请求一遍充值列表2042下发
SCRechargeConfigReloadNotice = SCRechargeConfigReloadNotice or BaseClass(BaseProtocolStruct)
function SCRechargeConfigReloadNotice:__init()
	self.msg_type = 2043
end

function SCRechargeConfigReloadNotice:Decode()
end

CSPartnerSwitchFightPos = CSPartnerSwitchFightPos or BaseClass(BaseProtocolStruct)
function CSPartnerSwitchFightPos:__init()
    self.msg_type = 2044
end

function CSPartnerSwitchFightPos:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.partner_id1)
    MsgAdapter.WriteShort(self.partner_id2)
end

CSRoleTaskPathFinding = CSRoleTaskPathFinding or BaseClass(BaseProtocolStruct)
function CSRoleTaskPathFinding:__init()
    self.msg_type = 2019
end

function CSRoleTaskPathFinding:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.task_id)
    MsgAdapter.WriteShort(self.is_stop)
    MsgAdapter.WriteShort(0)
end

-- 巡逻任务
SCPatrolTaskInfo = SCPatrolTaskInfo or BaseClass(BaseProtocolStruct)
function SCPatrolTaskInfo:__init()
	self.msg_type = 2020
end

function SCPatrolTaskInfo:Decode()
    self.info = {
        today_flag = MsgAdapter.ReadChar(),     --是否领取过巡逻任务
        today_reward = MsgAdapter.ReadChar(),   --是否获取过巡逻任务奖励
        task_progress = MsgAdapter.ReadShort(), --任务进度
        task_status = MsgAdapter.ReadShort(),   --领取状态 0-不可领取 1-可领取 2-已领取
    }
end

-- 28天累计登陆下发
SCLoginSystemInfo = SCLoginSystemInfo or BaseClass(BaseProtocolStruct)
function SCLoginSystemInfo:__init()
    self.msg_type = 2046
end

function SCLoginSystemInfo:Decode()
    self.login_day =  MsgAdapter.ReadInt()
    self.reward_flag = MsgAdapter.IntFlag(1, true)
    --LogError("28天累计登陆下发", self)
end

-- 28天请求
CSLoginSystemReq = CSLoginSystemReq or BaseClass(BaseProtocolStruct)
function CSLoginSystemReq:__init()
    self.msg_type = 2047
end

function CSLoginSystemReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.day)
end

SPECIAL_NOTICE_TYPE = {
    team_salary = 0,     -- 队长工资
	guild_gather_dream_gift = 1 --家族集愿奖励
}

-- 特殊信息系统栏通知
SCSpecialNotice = SCSpecialNotice or BaseClass(BaseProtocolStruct)
function SCSpecialNotice:__init()
    self.msg_type = 2048
end

function SCSpecialNotice:Decode()
    self.op_type =  MsgAdapter.ReadInt()
    self.param1 = MsgAdapter.ReadInt()
	self.param2 = MsgAdapter.ReadInt()
	self.param3 = MsgAdapter.ReadInt()
	self.param4 = MsgAdapter.ReadInt()
	self.param5 = MsgAdapter.ReadInt()
end

--属性来源列表下发
SCAttrListReason = SCAttrListReason or BaseClass(BaseProtocolStruct)
function SCAttrListReason:__init()
    self.msg_type = 2036
end

function SCAttrListReason:Decode()
    self.reason_type = MsgAdapter.ReadShort()
    self.param1 = MsgAdapter.ReadShort()
    self.reason_list = {} --属性来源列表
    for i = 0, AttrReasonConfig.MAX_REASON_TYPE_LIST_NUM - 1 do
        self.reason_list[i] = {}
        for j = 0, BattleAttriType.ATTRMAX - 1 do
            self.reason_list[i][j] = MsgAdapter.ReadInt()
        end
    end
end

--请求来源属性列表下发
CSAttrListReasonReq = CSAttrListReasonReq or BaseClass(BaseProtocolStruct)
function CSAttrListReasonReq:__init()
    self.msg_type = 2037
end

function CSAttrListReasonReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.reason_type) --ATTRLIST_REASON_TYPE
    MsgAdapter.WriteShort(self.param1) --角色默认0，宠物pet_index,伙伴partner_id
end

--传闻提示信息下发，用于好礼转动活动中颜值物品的延时播报
CSHearsayInfo = CSHearsayInfo or BaseClass(BaseProtocolStruct)
function CSHearsayInfo:__init()
    self.msg_type = 2059
end

function CSHearsayInfo:Decode()
    self.op_type = MsgAdapter.ReadInt() --0是激活成功，1是激活失败
    self.param1 = MsgAdapter.ReadInt()  --颜值道具id
end

-- 下发伙伴天赋信息
SCPartnerGiftTreeListInfo = SCPartnerGiftTreeListInfo or BaseClass(BaseProtocolStruct)
function SCPartnerGiftTreeListInfo:__init()
    self.msg_type = 2023
end

function SCPartnerGiftTreeListInfo:Decode()
    self.partner_gift_list = {}
    self.partner_num = MsgAdapter.ReadInt()
    for i = 1, self.partner_num do 
        local vo = {}
        vo.partner_id =  MsgAdapter.ReadInt()
        vo.gift_list = {}
        for j = 1,PartnerConfig.MAX_PARTNER_GIFT_TREE_SKILL_NUM do
            vo.gift_list[j] = MsgAdapter.ReadInt()
        end 
        self.partner_gift_list[i] = vo
    end 
end


-- 下发单个伙伴天赋信息
SCPartnerSingleGiftTreeInfo = SCPartnerSingleGiftTreeInfo or BaseClass(BaseProtocolStruct)
function SCPartnerSingleGiftTreeInfo:__init()
    self.msg_type = 2024
end

function SCPartnerSingleGiftTreeInfo:Decode()
    self.partner_id = MsgAdapter.ReadInt()
    self.gift_list = {}
    for j = 1,PartnerConfig.MAX_PARTNER_GIFT_TREE_SKILL_NUM do
        self.gift_list[j] = MsgAdapter.ReadInt()
    end 
end

-- 下发新服比拼人物外观
SCQueryRoleSimpleInfo = SCQueryRoleSimpleInfo or BaseClass(BaseProtocolStruct)
function SCQueryRoleSimpleInfo:__init()
    self.msg_type = 2025
end

function SCQueryRoleSimpleInfo:Decode()
    local info = {}
    info.role_id = MsgAdapter.ReadInt()
    info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    info.level = MsgAdapter.ReadUInt()
    info.top_level = MsgAdapter.ReadInt()
    info.capability = MsgAdapter.ReadInt()
    info.appearance = Appearance() -- 外观信息
    self.info = info
end

--客户端保存的数据
SCClientCustomInfo = SCClientCustomInfo or BaseClass(BaseProtocolStruct)
function SCClientCustomInfo:__init()
    self.msg_type = 2098
end

function SCClientCustomInfo:Decode()
    self.param1 = MsgAdapter.ReadInt()
    self.param2 = MsgAdapter.ReadInt()
end

--客户端请求保存数据到服务端 
CSClientCustomSeq = CSClientCustomSeq or BaseClass(BaseProtocolStruct)
function CSClientCustomSeq:__init()
    self.msg_type = 2099
end

function CSClientCustomSeq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

-- 任务小游戏
CSTaskGameReq = CSTaskGameReq or BaseClass(BaseProtocolStruct)
function CSTaskGameReq:__init(  )
    self.msg_type = 2026
end

function CSTaskGameReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.game_type)
    MsgAdapter.WriteInt(self.p1)
    MsgAdapter.WriteInt(self.p2)
end