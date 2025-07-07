
-- 本服个人榜请求
CSGetPersonRankList = CSGetPersonRankList or BaseClass(BaseProtocolStruct)
function CSGetPersonRankList:__init()
    self.msg_type = 10050
end

function CSGetPersonRankList:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.rank_type)
    MsgAdapter.WriteInt(self.special_type)--0无1好友2家族
    MsgAdapter.WriteShort(self.rank_start)
    MsgAdapter.WriteShort(self.rank_end)
end

local function ReadRankRoleInfo()
    local t = {
        uid = MsgAdapter.ReadInt(),
        name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        prof = MsgAdapter.ReadInt(),    
        avatar_type = MsgAdapter.ReadShort(),
        avatar_id = MsgAdapter.ReadShort(),
        top_level = MsgAdapter.ReadInt(),
        reserve_sh = MsgAdapter.ReadShort(),
        fly_flag = MsgAdapter.ReadShort(),
    }
    t.fly_flag = t.fly_flag > FlyUpStageType.High and FlyUpStageType.High or t.fly_flag
	t.avatar_quality = t.fly_flag
    return t
end

-- 本服个人榜返回
SCGetPersonRankListAck = SCGetPersonRankListAck or BaseClass(BaseProtocolStruct)
function SCGetPersonRankListAck:__init()
	self.msg_type = 10000
end

function SCGetPersonRankListAck:Decode()
    local t = {}
    t.rank_type = MsgAdapter.ReadInt()
    t.special_type = MsgAdapter.ReadInt()
    t.my_rank_index = MsgAdapter.ReadInt()
    t.count = MsgAdapter.ReadInt()
    t.rank_infos = {}
    for i = 1, t.count do
        local info = {}
        info.role = ReadRankRoleInfo()
        info.record_index = MsgAdapter.ReadInt()
        info.rank_value = MsgAdapter.ReadLL()
        info.flexible_int = MsgAdapter.ReadInt()
        info.flexible_ll = MsgAdapter.ReadLL()
        info.rank = i
        if t.my_rank_index == info.record_index then 
            t.rank_info_m = info 
        end
        t.rank_infos[i] = info
    end
    self.info = t

    -- LogError("魅力七夕本服排行榜下发！！！",self.info)
end

-- 本服宠物榜请求
CSGetPetRankList = CSGetPetRankList or BaseClass(BaseProtocolStruct)
function CSGetPetRankList:__init()
    self.msg_type = 10056
end

function CSGetPetRankList:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.rank_type)
    MsgAdapter.WriteInt(SocietyConfig.RANK_START)
    MsgAdapter.WriteInt(SocietyConfig.RANK_END)
end

local function ReadRankPetInfo()
    local t = {}
	t.pet_id = MsgAdapter.ReadInt()
	t.str_level = MsgAdapter.ReadUShort() -- 强化等级
	t.level = MsgAdapter.ReadUShort() -- 等级
	t.skin_id = MsgAdapter.ReadInt()
	t.pet_unique_item_id = MsgAdapter.ReadInt2()
    t.reserve_sh = MsgAdapter.ReadShort()
    t.fly_flag = MsgAdapter.ReadShort()
    t.fly_flag = t.fly_flag > FlyUpStageType.High and FlyUpStageType.High or t.fly_flag
    t.lian_yao_pet_id = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    local co = PetData.Instance:GetPetCfg(t.pet_id)
    t.icon_id = co and co.icon_id or 0
    t.qua_icon = PetData.PinZhi[co and co.quality or 1]
    if t.skin_id ~= 0 then
        local co_skin = Cfg.PetSkinAttr(t.skin_id)
        t.icon_id = co_skin and co.head_portrait or t.icon_id
    end
	return t
end

-- 本服宠物榜返回
SCGetPetRankListAck = SCGetPetRankListAck or BaseClass(BaseProtocolStruct)
function SCGetPetRankListAck:__init()
	self.msg_type = 10008
end

function SCGetPetRankListAck:Decode()
    local t = {}
    -- local role_id = RoleData.Instance:GetRoleId()
    t.last_snapshot_time = MsgAdapter.ReadUInt()
    t.rank_type = MsgAdapter.ReadInt()
    t.my_rank_index = MsgAdapter.ReadInt()
    t.count = MsgAdapter.ReadInt()
    t.rank_infos = {}
    for i = 1, t.count do
        local info = {}
        info.role = ReadRankRoleInfo()
        info.pet = ReadRankPetInfo()
        info.record_index = MsgAdapter.ReadInt()
        info.rank_value = MsgAdapter.ReadLL()
        info.flexible_int = MsgAdapter.ReadInt()
        info.flexible_ll = MsgAdapter.ReadLL()
        info.rank = i
        if t.my_rank_index == info.record_index then t.rank_info_m = info end
        info.details_func = function ()
            SocietyCtrl.Instance:SendGetRankPetDetailedInfo(info.role.uid, info.pet.pet_unique_item_id)
        end
        t.rank_infos[i] = info
    end
    self.info = t
end

local function WriteUUID(uuid)
    MsgAdapter.WriteInt(uuid.plat_type)
    MsgAdapter.WriteInt(uuid.db_index)
    MsgAdapter.WriteInt(uuid.role_id)
end

-- 跨服排行服务器列表请求
CSCrossGetPersonRankBaseInfo = CSCrossGetPersonRankBaseInfo or BaseClass(BaseProtocolStruct)
function CSCrossGetPersonRankBaseInfo:__init()
    self.msg_type = 14051
end

function CSCrossGetPersonRankBaseInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    WriteUUID(self.uuid)
    MsgAdapter.WriteInt(self.role_level)
end

-- 跨服排行服务器列表返回
SCCrossPersonRankBaseInfo = SCCrossPersonRankBaseInfo or BaseClass(BaseProtocolStruct)
function SCCrossPersonRankBaseInfo:__init()
    self.msg_type = 14003
end

function SCCrossPersonRankBaseInfo:Decode()
    local t = {}
    t.count = MsgAdapter.ReadInt()
    t.server_infos = {}
    for i = 1, t.count do
        t.server_infos[i] = CommonStruct.UniqueServerID(MsgAdapter.ReadInt(), MsgAdapter.ReadInt())
    end
    self.info = t


end

-- 全服个人榜请求
CSCrossGetPersonAllRankList = CSCrossGetPersonAllRankList or BaseClass(BaseProtocolStruct)
function CSCrossGetPersonAllRankList:__init()
    self.msg_type = 14052
end

function CSCrossGetPersonAllRankList:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.rank_type)
    MsgAdapter.WriteUInt(self.last_snapshot_time)
    MsgAdapter.WriteShort(self.rank_start)
    MsgAdapter.WriteShort(self.rank_end)
end

-- 全服个人榜返回
SCGetCrossPersonAllServerRankListAck = SCGetCrossPersonAllServerRankListAck or BaseClass(BaseProtocolStruct)
function SCGetCrossPersonAllServerRankListAck:__init()
    self.msg_type = 14004
end

function SCGetCrossPersonAllServerRankListAck:Decode()
    local t = {}
    -- local role_id = RoleData.Instance:GetRoleId()
    t.last_snapshot_time = MsgAdapter.ReadUInt()
    t.my_rank_index = MsgAdapter.ReadInt()
    t.rank_type = MsgAdapter.ReadInt()
    t.count = MsgAdapter.ReadInt()
    t.rank_infos = {}
    for i = 1, t.count do
        local info = {}
        info.role = ReadRankRoleInfo()
        info.record_index = MsgAdapter.ReadInt()
        info.rank_value = MsgAdapter.ReadLL()
        info.flexible_int = MsgAdapter.ReadInt()
        info.flexible_ll = MsgAdapter.ReadLL()
        info.rank = i
        if info.record_index == t.my_rank_index then t.rank_info_m = info end
        t.rank_infos[i] = info
    end
    self.info = t

   
end

-- 全服宠物榜请求
CSCrossGetPetAllRankList = CSCrossGetPetAllRankList or BaseClass(BaseProtocolStruct)
function CSCrossGetPetAllRankList:__init()
    self.msg_type = 14054
end

function CSCrossGetPetAllRankList:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.rank_type)
    MsgAdapter.WriteUInt(self.last_snapshot_time)
    MsgAdapter.WriteShort(SocietyConfig.RANK_START)
    MsgAdapter.WriteShort(SocietyConfig.RANK_END)
end

-- 全服宠物榜返回
SCGetCrossPetAllServerRankListAck = SCGetCrossPetAllServerRankListAck or BaseClass(BaseProtocolStruct)
function SCGetCrossPetAllServerRankListAck:__init()
	self.msg_type = 14006
end

function SCGetCrossPetAllServerRankListAck:Decode()
    local t = {}
    -- local role_id = RoleData.Instance:GetRoleId()
    t.last_snapshot_time = MsgAdapter.ReadUInt()
    t.my_rank_index = MsgAdapter.ReadInt()
    t.rank_type = MsgAdapter.ReadInt()
    t.count = MsgAdapter.ReadInt()
    t.rank_infos = {}
    for i = 1, t.count do
        local info = {}
        info.role = ReadRankRoleInfo()
        info.pet = ReadRankPetInfo()
        info.record_index = MsgAdapter.ReadInt()
        info.rank_value = MsgAdapter.ReadLL()
        info.flexible_int = MsgAdapter.ReadInt()
        info.flexible_ll = MsgAdapter.ReadLL()
        info.rank = i
        if info.record_index == t.my_rank_index then t.rank_info_m = info end
        info.details_func = function ()
            SocietyCtrl.Instance:SendCrossGetRankPetAllDetailedInfo(info.role.uid, info.pet.pet_unique_item_id)
        end
        t.rank_infos[i] = info
    end
    self.info = t
end

local function WriteUSID(usid)
    usid = usid or {}
    MsgAdapter.WriteInt(usid.plat_type or 0)
    MsgAdapter.WriteInt(usid.server_id or 0)
end

-- 跨服个人榜请求
CSCrossGetPersonRankList = CSCrossGetPersonRankList or BaseClass(BaseProtocolStruct)
function CSCrossGetPersonRankList:__init()
    self.msg_type = 14050
end

function CSCrossGetPersonRankList:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    WriteUSID(self.usid)
    MsgAdapter.WriteInt(self.rank_type)
    MsgAdapter.WriteUInt(self.last_snapshot_time)
    MsgAdapter.WriteShort(self.rank_start)
    MsgAdapter.WriteShort(self.rank_end)
end

-- 跨服个人榜返回
SCGetCrossPersonRankListAck = SCGetCrossPersonRankListAck or BaseClass(BaseProtocolStruct)
function SCGetCrossPersonRankListAck:__init()
    self.msg_type = 14002
end

function SCGetCrossPersonRankListAck:Decode()
    local t = {}
    t.usid = CommonStruct.UniqueServerID(MsgAdapter.ReadInt(), MsgAdapter.ReadInt())
    t.last_snapshot_time = MsgAdapter.ReadUInt()
    t.my_rank_index = MsgAdapter.ReadInt()
    t.rank_type = MsgAdapter.ReadInt()
    t.count = MsgAdapter.ReadInt()
    t.rank_infos = {}
    for i = 1, t.count do
        local info = {}
        info.role = ReadRankRoleInfo()
        info.record_index = MsgAdapter.ReadInt()
        info.rank_value = MsgAdapter.ReadLL()
        info.flexible_int = MsgAdapter.ReadInt()
        info.flexible_ll = MsgAdapter.ReadLL()
        info.rank = i
        if info.record_index == t.my_rank_index then t.rank_info_m = t.rank_info_m or info end
        t.rank_infos[i] = info
    end
    self.info = t
end

-- 跨服宠物榜请求
CSCrossGetPetRankList = CSCrossGetPetRankList or BaseClass(BaseProtocolStruct)
function CSCrossGetPetRankList:__init()
    self.msg_type = 14053
end

function CSCrossGetPetRankList:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    WriteUSID(self.usid)
    MsgAdapter.WriteInt(self.rank_type)
    MsgAdapter.WriteUInt(self.last_snapshot_time)
    MsgAdapter.WriteShort(SocietyConfig.RANK_START)
    MsgAdapter.WriteShort(SocietyConfig.RANK_END)
end

-- 跨服宠物榜返回
SCGetCrossPetRankListAck = SCGetCrossPetRankListAck or BaseClass(BaseProtocolStruct)
function SCGetCrossPetRankListAck:__init()
	self.msg_type = 14005
end

function SCGetCrossPetRankListAck:Decode()
    local t = {}
    local role_id = RoleData.Instance:GetRoleId()
    t.usid = CommonStruct.UniqueServerID(MsgAdapter.ReadInt(), MsgAdapter.ReadInt())
    t.last_snapshot_time = MsgAdapter.ReadUInt()
    t.my_rank_index = MsgAdapter.ReadInt()
    t.rank_type = MsgAdapter.ReadInt()
    t.count = MsgAdapter.ReadInt()
    t.rank_infos = {}
    for i = 1, t.count do
        local info = {}
        info.role = ReadRankRoleInfo()
        info.pet = ReadRankPetInfo()
        info.record_index = MsgAdapter.ReadInt()
        info.rank_value = MsgAdapter.ReadLL()
        info.flexible_int = MsgAdapter.ReadInt()
        info.flexible_ll = MsgAdapter.ReadLL()
        info.rank = i
        if info.record_index == t.my_rank_index then t.rank_info_m = t.rank_info_m or info end
        info.details_func = function ()
            if 0 == t.usid.plat_type and 0 == t.usid.server_id then
                SocietyCtrl.Instance:SendCrossGetRankPetDetailedInfo(info.role.uid, info.pet.pet_unique_item_id, {})
            else
                SocietyCtrl.Instance:SendCrossGetRankPetDetailedInfo(info.role.uid, info.pet.pet_unique_item_id)
            end
        end
        t.rank_infos[i] = info
    end
    self.info = t
end

-- 请求宠物排行榜某一个宠物信息
CSGetRankPetDetailedInfo = CSGetRankPetDetailedInfo or BaseClass(BaseProtocolStruct)
function CSGetRankPetDetailedInfo:__init()
    self.msg_type = 10059
end

function CSGetRankPetDetailedInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.uid)
    MsgAdapter.WriteInt(self.rank_type)
    MsgAdapter.WriteInt2(self.pet_unique_item_id)
end

-- 请求跨服宠物排行榜某一个宠物信息
CSCrossGetRankPetDetailedInfo = CSCrossGetRankPetDetailedInfo or BaseClass(BaseProtocolStruct)
function CSCrossGetRankPetDetailedInfo:__init()
    self.msg_type = 14055
end

function CSCrossGetRankPetDetailedInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.uid)
    WriteUSID(self.usid)
    MsgAdapter.WriteInt(self.rank_type)
    MsgAdapter.WriteInt2(self.pet_unique_item_id)
end

-- 请求跨服宠物总榜某一个宠物信息
CSCrossGetRankPetAllDetailedInfo = CSCrossGetRankPetAllDetailedInfo or BaseClass(BaseProtocolStruct)
function CSCrossGetRankPetAllDetailedInfo:__init()
    self.msg_type = 14056
end

function CSCrossGetRankPetAllDetailedInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.uid)
    MsgAdapter.WriteInt(self.rank_type)
    MsgAdapter.WriteInt2(self.pet_unique_item_id)
end

-- 排行榜宠物详细信息下发
SCRankPetDetailedInfo = SCRankPetDetailedInfo or BaseClass(BaseProtocolStruct)
function SCRankPetDetailedInfo:__init()
    self.msg_type = 10009
end

function SCRankPetDetailedInfo:Decode()
    local buff_level = MsgAdapter.ReadInt()
    local pet_info = PortocolCommon.PetProParam()
    pet_info.index = -1
    local pet = Pet.New(pet_info)
    pet:SetComLevel(buff_level or 1)
    local attr_list = {}
    for i = 1, PET_ATTR_LIST_COUNT do
        attr_list[i - 1] = MsgAdapter.ReadInt()
    end
    pet:SetAttrList(attr_list)

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
    pet.stamp_blueprint = PetStampBlueprint.New(info, -1)
    self.pet = pet
end

--===========================锢魔之塔排行榜下发===========================
CSCourageChallengeRankListReq = CSCourageChallengeRankListReq or BaseClass(BaseProtocolStruct)
function CSCourageChallengeRankListReq:__init()
    self.msg_type = 10060
end

function CSCourageChallengeRankListReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.rank_type)
    MsgAdapter.WriteShort(self.count)
end

SCCourageChallengeRankListInfo = SCCourageChallengeRankListInfo or BaseClass(BaseProtocolStruct)
function SCCourageChallengeRankListInfo:__init()
    self.msg_type = 10061
end 

function SCCourageChallengeRankListInfo:Decode()
    self.rank_type = MsgAdapter.ReadShort()
    self.rank_list = {}
    for i=1,MsgAdapter.ReadShort() do
        self.rank_list[i] = CourageChallengeStruct.RankinfoStruct(i)
    end
end

--请求排行榜刷新时间
CSGetRankRefreshTime = CSGetRankRefreshTime or BaseClass(BaseProtocolStruct)
function CSGetRankRefreshTime:__init()
    self.msg_type = 10057
end

function CSGetRankRefreshTime:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

SCGetRankRefreshTime = SCGetRankRefreshTime or BaseClass(BaseProtocolStruct) 
function SCGetRankRefreshTime:__init()
    self.msg_type = 10058
end

function SCGetRankRefreshTime:Decode()
    self.time = MsgAdapter.ReadUInt()
end

local function ReadChivalrousRank(index)
    local t = {}
    t.rank = index
    t.role_id = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.avatar_type = MsgAdapter.ReadShort()
    t.avatar_id = MsgAdapter.ReadShort()--人物头像
    t.prof = MsgAdapter.ReadInt()
    t.week_chivalrous = MsgAdapter.ReadInt()
    return t
end

-- 侠义值排行榜请求
CSChivalrousRankListReq = CSChivalrousRankListReq or BaseClass(BaseProtocolStruct)
function CSChivalrousRankListReq:__init()
    self.msg_type = 10062
end

function CSChivalrousRankListReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.rank_type)
end

-- 侠义值排行榜下发
SCChivalrousRankList = SCChivalrousRankList or BaseClass(BaseProtocolStruct) 
function SCChivalrousRankList:__init()
    self.msg_type = 10063
end

function SCChivalrousRankList:Decode()
    self.rank_type = MsgAdapter.ReadShort()
    self.rank_list = {}
    for i = 1, MsgAdapter.ReadShort() do
        self.rank_list[i] = ReadChivalrousRank(i)
    end
end

local function WriteServerID(target_server_id)
    MsgAdapter.WriteInt(target_server_id.plat_type)
    MsgAdapter.WriteInt(target_server_id.server_id)
end

--请求家族活跃排行榜
CSCrossGuildActiveGuildRankInfo = CSCrossGuildActiveGuildRankInfo or BaseClass(BaseProtocolStruct) 
function CSCrossGuildActiveGuildRankInfo:__init()
    self.msg_type = 14070
end

function CSCrossGuildActiveGuildRankInfo:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.is_all)
	MsgAdapter.WriteInt(self.plat_type)
    MsgAdapter.WriteInt(self.server_id)
	MsgAdapter.WriteInt(self.rank_type)
	MsgAdapter.WriteUInt(self.last_snapshot_time)
	MsgAdapter.WriteInt(self.guild_id)
	MsgAdapter.WriteShort(self.index_start)
	MsgAdapter.WriteShort(self.index_end)
end

local function ReadGuildActiveRank()
    local t = {}
    t.server_id = CommonStruct.UniqueServerID(MsgAdapter.ReadInt(), MsgAdapter.ReadInt())
    t.guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
	t.guild_banner = MsgAdapter.ReadStrN(PROTC_GUILD_BANNER)
	t.rank_val = MsgAdapter.ReadLL()
	t.rank_index = MsgAdapter.ReadInt()
    return t
end

-- 家族活跃排行榜下发{单服}
SCCrossGuildRankListAck = SCCrossGuildRankListAck or BaseClass(BaseProtocolStruct) 
function SCCrossGuildRankListAck:__init()
    self.msg_type = 14071
end

function SCCrossGuildRankListAck:Decode()
    self.unique_server_id = CommonStruct.UniqueServerID(MsgAdapter.ReadInt(), MsgAdapter.ReadInt())
	self.index = MsgAdapter.ReadInt()
    self.last_snapshot_time = MsgAdapter.ReadUInt()
	self.rank_type = MsgAdapter.ReadInt()
	self.rank_list = {}
    for i = 1, MsgAdapter.ReadInt() do
        self.rank_list[i] = ReadGuildActiveRank()
    end
end

-- 家族活跃排行榜下发{总榜}
SCCrossGuildRankListAllServer = SCCrossGuildRankListAllServer or BaseClass(BaseProtocolStruct) 
function SCCrossGuildRankListAllServer:__init()
    self.msg_type = 14072
end

function SCCrossGuildRankListAllServer:Decode()
	self.index = MsgAdapter.ReadInt()
    self.last_snapshot_time = MsgAdapter.ReadUInt()
	self.rank_type = MsgAdapter.ReadInt()
	self.rank_list = {}
    for i = 1, MsgAdapter.ReadInt() do
        self.rank_list[i] = ReadGuildActiveRank()
    end
end