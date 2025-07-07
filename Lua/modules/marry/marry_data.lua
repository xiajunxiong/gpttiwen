MarryData = MarryData or BaseClass()

function MarryData:__init()
    if MarryData.Instance ~= nil then
        Debuger.LogError("[MarryData] attempt to create singleton twice!")
        return
    end
    MarryData.Instance = self
    self.info_data = SmartData.New({
        role_info = {},
        create_time = 0,
        mate_info = {},
        self_info = {},
        leave_end_time = 0,
        has_wedding = MarryConfig.HasWeddingType.none,
        val = false
    })
    self.config = Config.get_married_auto
    self.other_config = Config.get_married_auto.other[1]
    self.skill_flag_list = bit:d2b(0)
    self:MarryDataWeddingFB()
end

function MarryData:ClearData()
    self.info_data:Set({
        role_info = {},
        create_time = 0,
        mate_info = {},
        self_info = {},
        leave_end_time = 0,
        has_wedding = MarryConfig.HasWeddingType.none,
    })
    self.parade_float_info = nil
end

--结婚信息下发
function MarryData:SetMarryInfo(protocol)
    self.info_data.has_wedding = protocol.has_wedding
    self.info_data.create_time = protocol.create_time
    self.info_data.leave_end_time = protocol.leave_end_time
    for k,v in pairs(protocol.role_info) do
        if RoleData.Instance:GetRoleId() == v.role_id then
            self.info_data.self_info = v
            self.skill_flag_list = TableCopy(v.skill_flag)
        else
            self.info_data.mate_info = v
        end
    end
    self.info_data.role_info = protocol.role_info
    self.marry_id = protocol.marry_id
end

function MarryData:SetMarryQueryRet(protocol)
	self.query_info = protocol.query_info
end

function MarryData:GetMarryQueryInfo()
	return self.query_info
end

--获取结婚纪念日配置
function MarryData:GetAssessConfig(seq)
    return self.config.marry_day[seq]
end

--结婚纪念日对应天数配置
function MarryData:GetAssessDayConfig(marry_time)
    local config = DataHelper.FormatList(self.config.marry_day)
    local list = DataHelper.ReverseList(config)
    for i,v in ipairs(list) do
        if marry_time >= v.marry_time then
            return list[i-1] or v
        end
    end
    if marry_time >= list[#list].marry_time then
        return list[#list]
    end
    return list[#list]
end

--设置情缘技能开关列表
function MarryData:SetSkillFlagList(seq,flag)
    self.skill_flag_list[32 - (seq or 1)] = flag
end

function MarryData:GetSkillFlagList()
    return bit:b2d(self.skill_flag_list)
end

--获取情缘NPC
function MarryData:GetNpcId()
    return self.config.other[1].marry_npc
end

--获取当前亲密度 对方的uid
function MarryData:GetIntimacyNum(uid)
    uid = uid or self:GetHusbandOrWifeUid() or 0
    return SocietyData.Instance:GetFriendIntimacyVal(uid) or 0
end

--获取情缘需要等级
function MarryData:GetMarryLevel()
    local config = FunOpen.Instance:GetOpenData(Mod.Society.Marry)
    return config and config.level or 0
end

--获取双方基础流程信息
function MarryData:GetRoleBaseInfo()
    return self.info_data.role_info
end

--自己的结婚信息 + 玩家信息
function MarryData:GetSelfInfo()
    local role_id = self.info_data.self_info.role_id or 0
    return self.info_data.self_info,RoleData.Instance:GetRoleData()
end

--对方的结婚信息 + 玩家信息
function MarryData:GetMateInfo()
    local role_id = self.info_data.mate_info.role_id or 0
    return self.info_data.mate_info,SocietyData.Instance:GetFriendInfoByUid(role_id)
end

--获取双方角色信息2010协议
function MarryData:GetRoleInfo()
    local list = {}
    for i,v in ipairs(self:GetRoleBaseInfo()) do
        list[i] = RoleInfoData.Instance:GetQueryRoleInfo(v.role_id) or {}
    end
    return list
end

--获取结婚的时间
function MarryData:GetCreateTime()
    return self.info_data.create_time
end

--获取结婚天数
function MarryData:GetDayNum()
    return TimeCtrl.Instance:GetDataDayNum(TimeHelper:GetServerTime(),self:GetCreateTime())
end

--获取伴侣id 0=没有结婚
function MarryData:GetHusbandOrWifeUid()
    return self.info_data.mate_info.role_id or 0
end

--获取伴侣名字
function MarryData:GetHusbandOrWifeName()
    local _,role_info = self:GetMateInfo()
    return role_info and role_info.name or ""
end

--获取冷静期时间戳
function MarryData:GetCoolingOffTime()
    local leave_end_time = self:GetSelfInfo().leave_end_time or 0
    return leave_end_time ~= 0 and leave_end_time or self.info_data.leave_end_time
end

--是否举办过婚宴
function MarryData:HasWeddingType(type)
    return self.info_data.has_wedding == type
end

--情缘红点
function MarryData:GetRemindNum()
    local base_info = self:GetSelfInfo()
    local has_gift_time = (base_info.gift_type_end_time or 0) - TimeHelper:GetServerTime()
    if has_gift_time <= 0 then
        return 0
    end
    if base_info.gift_type and base_info.gift_type ~= 0 then
        local flag = base_info.fetch_gift_flag
        if flag[1] == 0 or flag[2] == 0 then
            return 1
        end
    end
    return 0
end

--获取玩家结婚前缀
function MarryData:GetMarryPreName(info)
    local role_sex = RoleData.Instance:GetRoleSex(info.avatar_type)
    return Language.Marry.MarryName[role_sex] or ""
end

--是否结婚
function MarryData:IsHasMarry()
    return self:GetHusbandOrWifeUid() ~= 0
end

function MarryData:SetWeddingLookLoveInfo(protocol)
    self.look_love_info = protocol
end

function MarryData:GetWeddingLookLoveInfo()
    return self.look_love_info
end

function MarryData:SetParadeFloatInfo(info)
    self.parade_float_info = info
end

--是否处于巡游中
function MarryData:IsParadeFloat(scene_id)
    if SceneLogic.Instance:MainCityID() == SceneLogic.Instance:SceneID() then
        for k,v in pairs(self.parade_float_info or {}) do
            if v.uid == RoleData.Instance:GetRoleId() then
                if TeamCtrl.Instance:Data():InTeam() then
                    TeamCtrl.Instance:SendExitTeam()
                end
                if scene_id == SceneLogic.Instance:MainCityID()then
                    return false
                end
                return true
            end
        end
    end
    return false
end