RoleData = RoleData or BaseClass()

function RoleData:__init()
    if RoleData.Instance ~= nil then
        Debuger.LogError("[RoleData] attempt to create singleton twice!")
        return
    end
    RoleData.Instance = self
    self.base_data = SmartData.New(CommonStruct.RoleBaseParam())
    self.battle_attri = SmartData.New() --战斗属性
    self.currencys = SmartData.New()    --货币属性
	self:InitOtherData()
end

function RoleData:ClearData()
	self.base_data:Set(CommonStruct.RoleBaseParam())
	self.battle_attri:Set({})	--战斗属性
	self.currencys:Set({})	    --货币属性
    self:ClearOtherData()
end

function RoleData:__delete()
    RoleData.Instance = nil
end

function RoleData:SetRoleAttrList(attr_list)
    for k,v in pairs(attr_list) do
        self.battle_attri[k] = v
    end
end

function RoleData:Set(key_list,data)
    for _,key in pairs(key_list) do
        self.base_data[key] = data[key]
    end
end

function RoleData:SetApearance(apearance)
    self.base_data.appearance = CommonStruct.AppearanceParam(apearance)
end

--获取玩家自身信息
function RoleData:GetRoleData()
    return {
        uid = self:GetRoleId(),
        name = self:GetRoleName(),
        role_name = self:GetRoleName(),
        prof = self:GetRoleProf(),
        role_id = self:GetRoleId(),
        level = self:GetRoleLevel(),
        avatar_id = self:GetRoleAvatarId(),
        avatar_type = self:GetRoleAvatarType(),
        avatar_quality = self:GetRoleAvatarQuality(),
        avatar_level = self:GetRoleLevel(),
        top_level = self:GetTopLevel(),
        appearance = self:GetApearance()
    }
end

function RoleData:GetApearance()
    return self.base_data.appearance
end

function RoleData:GetBaseData()
    return self.base_data
end

function RoleData:AllBattleAttri()
    return self.battle_attri
end

function RoleData:GetBattleAttri(attri_type)
    local reval = self.battle_attri[attri_type]
    return reval ~= nil and reval or 0
end

function RoleData:GetCurrencyData()
    return self.currencys
end

function RoleData:GetCurrency(cur_type)
    local reval = self.currencys[cur_type]
    return reval ~= nil and reval or (Item.GetNum(HuoBi[cur_type]) or 0)
end

function RoleData:SetCurrency(cur_type, value, block_hint)
    if self.currencys[cur_type] and block_hint ~= true then
        local num = value - self.currencys[cur_type]
        if num > 0 then
            if BagData.Instance:IsMarkedCenterVirtual() then PublicPopupCtrl.Instance:CenterV {type = cur_type, num = num} end 
            ChatCtrl.Instance:ChannelChatCustom{item_get = {item = Item.Init(HuoBi[cur_type], num), num = num}}
        elseif num < 0 then
            ChatCtrl.Instance:ChannelChatCustom{item_lost = {item = Item.Init(HuoBi[cur_type], num), num = -num}}
        end
    end
    self.currencys[cur_type] = tonumber(value)
    if cur_type == CurrencyType.GuildCotribution then
        GuildData.Instance.guild_shop_first_item.flush = not GuildData.Instance.guild_shop_first_item.flush
    end
end

function RoleData.GetNextLevelExp(level)
    local re_val = Config.roleexp_auto.exp_config[level]
    local top_level = RoleData.Instance:GetTopLevel()
    if top_level > 0 then
        if top_level > 400 then
            return Config.roleexp_auto.top_level[400].up_exp
        end
        return Config.roleexp_auto.top_level[top_level].up_exp
    end
    return re_val ~= nil and re_val.exp or 0
end

function RoleData:GetRoleName()
    return self.base_data.name
end

function RoleData:GetGuildName()
    return self.base_data.guild_name
end

function RoleLevel()
    return RoleData.Instance:GetRoleLevel()
end

function RoleData:GetRoleLevel()
    return self.base_data.level
end
function RoleData:GetTopLevel()
    return self.base_data.top_level
end
--当前角色是几转(晋阶等级)
function RoleData:ProfStage()
    return RoleData.ProfIdToProfStage(self.base_data.profession_id)
end

--获得当前是几转，目前按数据规律可以直接取余，如果之后数据不遵循些规律则改成找表 career_advancements_auto
--职业id 转换成 晋阶等级
function RoleData.ProfIdToProfStage(prof_id)
    return prof_id > 100 and prof_id % 100 or 0
end

--职业id(prof_id)转换为职业枚举(RoleProfessionType[0-8])
function RoleData.ProfIdToProfType(prof_id)
    prof_id = prof_id or 0
    if prof_id < 100 then
        return prof_id
    end
    return math.modf(prof_id / 100)
end

--玩家的职业枚举(RoleProfessionType[0-8])
function RoleData:ProfType()
    return RoleData.ProfIdToProfType(self:GetRoleProfId())
end

--获取职业icon名
--prof RoleProfessionType[0-8]
--is_small 小图标
function RoleData.GetProfSp(prof, is_small)
    local prof_type = RoleData.ProfIdToProfType(prof)
    local setting = is_small and ProfSettingS or ProfSettingB
    return setting[prof_type]
end

--获取职业icon名
--prof_id 大于100的职业id 101~
--is_small 小图标
function RoleData.GetProfSpByProfId(prof_id, is_small)
    local prof_type = RoleData.ProfIdToProfType(prof_id)
    return RoleData.GetProfSp(prof_type, is_small)
end

--获取主角职业id
function RoleData:GetRoleProfId()
    return self.base_data.profession_id or 0
end

function RoleData:GetProfession()
    return self.base_data.profession or 0
end

--获取角色类型
function RoleData:GetRoleAvatarType()
    return self:GetApearance().avatar_type
end

--获取头像id
function RoleData:GetRoleAvatarId()
    return self:GetApearance().avatar_id
end

--获取角色头像品质
function RoleData:GetRoleAvatarQuality()
    return self:GetApearance().fly_flag
end

--获取玩家性别 sex 0 女 1 男
function RoleData:GetRoleSex(avatar_type)
    avatar_type = avatar_type or self:GetRoleAvatarType()
    if avatar_type == nil then
        return 0
    end
    return avatar_type > 3 and 1 or 0
end

--获取人物职业枚举
function RoleData:GetRoleProf(prof_id)
    prof_id = prof_id or self:GetRoleProfId()
    return math.floor(prof_id / 100)
end

--获取职业类别 201 => 200
function RoleData:GetRoleProfType(prof_id)
    prof_id = prof_id or self:GetRoleProfId()
    return math.floor(prof_id / 100) * 100
end

function RoleId()
    return RoleData.Instance:GetRoleId()
end

--获取玩家id
function RoleData:GetRoleId()
    return self.base_data.role_id or 0
end

function RoleData:SetIsLeader(value)
    self.base_data.is_leader = value and 1 or 0
end

function RoleData:GetCreateTime()
    return self.base_data.create_time
end

--角色评分接口
function RoleData:GetRoleScore()
    local score = self.base_data.capability or 0
    if self.old_total_score == 0 then
        self.old_total_score = self.base_data.capability
    end
    return score,self:GetScoreLevel(score,self:ProfType(),self:GetRoleLevel())
end

--评分等级
function RoleData:GetScoreLevel(score, prof, level)
    local config = Cfg.RoleScoreLevel(prof, level)
    if config ~= nil then
        for i = #RoleScoreKey,1, -1 do
            if score >= config[RoleScoreKey[i]] then
                return i
            end
        end
    end
    return 0
end

-- 获取玩家总评分
function RoleData:GetSumScore()
    return self:GetRoleScore() + PetData.GetScore() + PartnerData.GetScore()
end

function RoleData:GetRoleTotalScore()
    return self:GetRoleScore() + PetData.GetScore() + PetData.GetStandByScore() + PartnerData.GetScore()
end

function RoleData:TotalScore()
    local score = self:GetRoleScore()
    local old_score = 0
    if self.old_total_score and self.old_total_score ~= 0 then
        old_score = self.old_total_score
    end
    self.old_total_score = score
    return score, old_score
end

--角色创建到现在是第几天(从创建时间达到24小时的次数 如20点创建的号到24点依然算第一天，到次日20点才算第二天)
function RoleData:CreatedDay()
    return TimeHelper.FormatDHMS(TimeHelper.GetServerTime() - self:GetCreateTime()).day + 1
end

--与上面函数相反，只按是否过零点算天数 如20点创建的号到24点直接算第二天
function RoleData:CreateDayEx()
    local t0 = TimeManager.FormatUnixTime2Date(TimeHelper.GetServerTime())
    t0.hour = 0
	t0.minute = 0
    t0.second = 0
   	local s_t = os.time(t0)
    local t1 = TimeManager.FormatUnixTime2Date(self:GetCreateTime())
    t1.hour = 0
    t1.minute = 0
    t1.second = 0
    local c_t = os.time(t1)     
    return (s_t - c_t) / 3600 / 24 + 1
end

function RoleData:UUID()
    return self.base_data.role_uuid
end