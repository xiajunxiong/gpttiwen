
function SocietyData:RankData()
    self.rank_infos = {}
    self.rank_info_m = {}
    self.rank_flush_times = {}
    self.rank_info_index = {}
    for i = 1, SocietyConfig.RANK_TYPE_NUM_MAX do
        self.rank_infos[i] = {}
        self.rank_info_m[i] = {}
        self.rank_flush_times[i] = {}
        self.rank_info_index[i] = {}
    end
    self.rank_server_infos = {
        {server_id = SocietyConfig.ServerSp.all},
        {server_id = SocietyConfig.ServerSp.login},
    }
    self.rank_all_info = {}
    self.rank_cross_info = {}
    self.rank_all_data = SmartData.New({val = false})
    self.rank_cross_data = SmartData.New({val = false})
end

local function USIDTrans(rank_server_infos, uuid)
    for _, v in ipairs(rank_server_infos) do
        if uuid.plat_type == v.plat_type and uuid.server_id == v.server_id then
            return v
        end
    end
end

local function USIDTransL(rank_server_infos, server_id)
    for _, v in ipairs(rank_server_infos) do
        if nil == v.plat_type and server_id == v.server_id then
            return v
        end
    end
end

function SocietyData:SetGetPersonRankListAck(protocol)
    local info = protocol.info
    local type = SocietyConfig.RankTypeRoleToC[info.rank_type]
    local usid = USIDTransL(self.rank_server_infos, SocietyConfig.ServerSp.login)
    if nil == self.rank_infos[type] or nil == usid then
        LogError("SetGetPersonRankListAck error")
        return
    end
    self.rank_infos[type][usid] = info.rank_infos
    self.rank_info_m[type][usid] = info.rank_info_m
    self.rank_flush_times[type][usid] = 0
    self.rank_info_index[type][usid] = info.my_rank_index
    self:RankFlushList(type)
    if SocietyConfig.PersonRankType.Cap == info.rank_type then
        self.my_rank_show.val = (info.my_rank_index >= 0 and info.my_rank_index < 999) and info.my_rank_index + 1 or 0
    end
end

function SocietyData:SetnGetPetRankListAck(protocol)
    local info = protocol.info
    local type = SocietyConfig.RankTypePetToC[info.rank_type]
    local usid = USIDTransL(self.rank_server_infos, SocietyConfig.ServerSp.login)
    if nil == self.rank_infos[type] or nil == usid then
        LogError("SetnGetPetRankListAck error")
        return
    end
    self.rank_infos[type][usid] = info.rank_infos
    self.rank_info_m[type][usid] = info.rank_info_m
    self.rank_flush_times[type][usid] = info.last_snapshot_time + SocietyConfig.RANK_INTERVAL
    self.rank_info_index[type][usid] = info.my_rank_index
    self:RankFlushList(type)
end

function SocietyData:SetGetCrossPersonAllServerRankListAck(protocol)
    local info = protocol.info
    local type = SocietyConfig.RankTypeRoleToC[info.rank_type]
    local usid = USIDTransL(self.rank_server_infos, SocietyConfig.ServerSp.all)
    if nil == self.rank_infos[type] or nil == usid then
        LogError("SetGetCrossPersonAllServerRankListAck error")
        return
    end
    self.rank_infos[type][usid] = info.rank_infos
    self.rank_info_m[type][usid] = info.rank_info_m
    self.rank_flush_times[type][usid] = info.last_snapshot_time + SocietyConfig.RANK_INTERVAL
    self.rank_info_index[type][usid] = info.my_rank_index
    self:RankFlushList(type)
end

function SocietyData:SetGetCrossPetAllServerRankListAck(protocol)
    local info = protocol.info
    local type = SocietyConfig.RankTypePetToC[info.rank_type]
    local usid = USIDTransL(self.rank_server_infos, SocietyConfig.ServerSp.all)
    if nil == self.rank_infos[type] or nil == usid then
        LogError("SetGetCrossPetAllServerRankListAck error")
        return
    end
    self.rank_infos[type][usid] = info.rank_infos
    self.rank_info_m[type][usid] = info.rank_info_m
    self.rank_flush_times[type][usid] = info.last_snapshot_time + SocietyConfig.RANK_INTERVAL
    self.rank_info_index[type][usid] = info.my_rank_index
    self:RankFlushList(type)
end

function SocietyData:SetGetCrossPersonRankListAck(protocol)
    local info = protocol.info
    local not_cross = 0 == info.usid.plat_type and 0 == info.usid.server_id
    local type = SocietyConfig.RankTypeRoleToC[info.rank_type]
    local usid = not_cross and USIDTransL(self.rank_server_infos, SocietyConfig.ServerSp.login) or USIDTrans(self.rank_server_infos, info.usid)
    if nil == self.rank_infos[type] or nil == usid then
        LogError("SetGetCrossPersonRankListAck error")
        return
    end
    if not_cross then
        self.rank_info_m[type][usid] = info.rank_info_m
        self.rank_info_index[type][usid] = info.my_rank_index
    end
    self.rank_infos[type][usid] = info.rank_infos
    self.rank_flush_times[type][usid] = info.last_snapshot_time + SocietyConfig.RANK_INTERVAL
    self:RankFlushList(type)
    if not_cross and SocietyConfig.PersonRankType.Cap == info.rank_type then
        self.my_rank_show.val = (info.my_rank_index >= 0 and info.my_rank_index < 999) and info.my_rank_index + 1 or 0
    end
end

function SocietyData:SetGetCrossPetRankListAck(protocol)
    local info = protocol.info
    local not_cross = 0 == info.usid.plat_type and 0 == info.usid.server_id
    local type = SocietyConfig.RankTypePetToC[info.rank_type]
    local usid = not_cross and USIDTransL(self.rank_server_infos, SocietyConfig.ServerSp.login) or USIDTrans(self.rank_server_infos, info.usid)
    if nil == self.rank_infos[type] or nil == usid then
        LogError("SetGetCrossPetRankListAck error")
        return
    end
    if not_cross then
        self.rank_info_m[type][usid] = info.rank_info_m
        self.rank_info_index[type][usid] = info.my_rank_index
    end
    self.rank_infos[type][usid] = info.rank_infos
    self.rank_flush_times[type][usid] = info.last_snapshot_time + SocietyConfig.RANK_INTERVAL
    self:RankFlushList(type)
end

function SocietyData:SetCrossPersonRankBaseInfo(protocol)
    local info = protocol.info
    local login_server = LoginData.Instance:GetLastLoginServer()
    local server_login = USIDTransL(self.rank_server_infos, SocietyConfig.ServerSp.login)
    local server_all = USIDTransL(self.rank_server_infos, SocietyConfig.ServerSp.all)
    self.rank_server_infos = {
        server_all,
        server_login,
    }
    for i = 1, info.count do
        local server_info = info.server_infos[i]
        if login_server ~= server_info.server_id then
            table.insert(self.rank_server_infos, server_info)
        else
            self.rank_server_infos[2].sever_sp = server_info
        end
    end
    table.sort(self.rank_server_infos, function (a, b)
        if not a or not b then return a end
        local an = nil == a.plat_type and 0 or 1
        local bn = nil == b.plat_type and 0 or 1
        if an == bn then
            return a.server_id < b.server_id
        end
        return an < bn
    end)
end

function SocietyData:ResetRankSM()
    self.rank_sm:Set{
        type = 1,
        rank_type = 1,
        title_type = 1,
        server = USIDTransL(self.rank_server_infos, SocietyConfig.ServerSp.login), 
     }
end

function SocietyData:RankFlushList(type)
    if type == self.rank_sm.type then
        self.rank_flush.list = not self.rank_flush.list
    end
end

function SocietyData:RankTypList()
    if nil == self.rank_type_list then
        self.rank_type_list = {}
        for i = 1, SocietyConfig.RANK_TYPE_NUM_MAX do
            self.rank_type_list[i] = {
                type = i,
                type_name = Language.Society.Rank.RankNames[i],
                title_type = SocietyConfig.RankTitleType[i],
            }
        end
    end
    return self.rank_type_list 
end

function SocietyData:RankServerList()
    return self.rank_server_infos
end

function SocietyData:GetRankInfos()
    return self.rank_infos[self.rank_sm.type][self.rank_sm.server] or {}
end

function SocietyData:GetRankInfoM()
    local show_m, info_m = nil == self.rank_sm.server.plat_type and (self.rank_sm.type <= 3 or RoleData.Instance:ProfType() == (self.rank_sm.type - 3))
    if show_m then
        info_m = self.rank_info_m[self.rank_sm.type][self.rank_sm.server]
        if nil == info_m then
            show_m = 3 ~= self.rank_sm.type
            local pet
            for _, pet_info in pairs(PetData.Instance.pet_list) do
                show_m = true
                pet = (nil == pet or pet:GetScore() < pet_info:GetScore()) and pet_info or pet
            end
            local t
            if pet then
                t = {
                    pet_id = pet:ID(),
                    str_level = pet:StrLevel(),
                    level = pet:Level(),
                    pet_unique_item_id = 0,
                    name = pet:Name(),
                    qua_icon = pet:QuaIcon(),
                    icon_id = pet:IconId(),
                    fly_flag = pet:FeiShengTimes(),
                    lian_yao_pet_id = pet:YaoPetId(),
                }
            end
            info_m = {
                role = {
                    name = RoleData.Instance:GetRoleName(),
                    prof = RoleData.Instance:GetRoleProfId(),
                    avatar_type = RoleData.Instance:GetRoleAvatarType(),
                    avatar_id = RoleData.Instance:GetRoleAvatarId(),
                    avatar_quality = RoleData.Instance:GetRoleAvatarQuality(),
                    top_level = RoleData.Instance:GetTopLevel(),
                    fly_flag = FlyUpData.Instance:GetStage(),
                },
                pet = t,
                rank = 0,
            }
            info_m.rank_value = SocietyData.GetRankValue(self.rank_sm.type, pet)
            local index = self.rank_info_index[self.rank_sm.type][self.rank_sm.server]
            if index ~= nil and index ~= -1 then
                info_m.rank = index + 1
            end
        end
    end
    return show_m, info_m
end

function SocietyData.GetRankValue(type, pet)
    if 1 == type then       --综合排行
        return RoleData.Instance:GetSumScore() + PetData.GetStandByScore()
    elseif 2 == type then   --等级排行
        return RoleData.Instance:GetRoleLevel()
    elseif 3 == type then --宠物排行
        return pet and pet:GetScore() or 0
    else
        return RoleData.Instance:GetRoleScore()
    end 
end

--综合评分  人物+出战伙伴+出战宠物
function SocietyData:GetRoleTotalScore()
    local rank_value = 0
    rank_value = rank_value + RoleData.Instance:GetRoleScore()
    local partner_list = PartnerCtrl.Instance:Data():GetPartnerAllInfo()
    for _, info in pairs(partner_list) do
        local vo = info.vo
        if PartnerConfig.LockState.unlock == vo.is_unlocked and PartnerConfig.FightState.fight == vo.fight_state then
            rank_value = rank_value + vo.cap
        end
    end
    local pet_info = PetData.Instance:GetFightPetInfo()
    if pet_info ~= nil then
        rank_value = rank_value + pet_info:GetScore()
    end
    return rank_value
end

--获取玩家在排行榜的数据 外部下发的 rank_list
function SocietyData.GetRoleRankInfo(rank_list)
    local rank,info = DataHelper.SimpleCondIndex(rank_list,function(data)
        return RoleData.Instance:GetRoleId() == (data.role_id or data.uid or data.role.uid)
    end)
    return {
        rank = info and info.rank or rank or 0,
        rank_value = info and info.rank_value or 0,
        role = {
            uid = RoleData.Instance:GetRoleId(),
            prof = RoleData.Instance:GetRoleProfId(),
            name = info and info.name or RoleData.Instance:GetRoleName(),
            avatar_type = RoleData.Instance:GetRoleAvatarType(),
            avatar_id = RoleData.Instance:GetRoleAvatarId(),
            avatar_quality = RoleData.Instance:GetRoleAvatarQuality(),
            level = RoleData.Instance:GetRoleLevel(),
            top_level = RoleData.Instance:GetTopLevel(),
        }
    }
end

function SocietyData:SetRankAllInfo(protocol)
    self.rank_all_info[protocol.info.rank_type] = protocol.info
    self.rank_all_data.val = not self.rank_all_data.val
end

function SocietyData:SetRankCrossInfo(protocol)
    self.rank_cross_info[protocol.info.rank_type] = protocol.info
    self.rank_cross_data.val = not self.rank_cross_data.val
end

-- 在这里可以自己去定制所需的排行榜
function SocietyData:CrossPersonAllServerRankListOtherDeal(protocol)
    if KFHeroRecordRank[protocol.info.rank_type] then
        -- 跨服英雄录
        KFHeroRecordData.Instance:SetRankData(protocol)
    end
end

--外部获取排行榜数据 本服（在跨服）个人排行榜 + 玩家排行信息 （本服排行榜在跨服是在跨服中的排行榜，而不是跨服排行榜）
function SocietyData:GetRankInfo(rank_type)
    local rank_info = self.rank_all_info[rank_type] or {}
    return rank_info.rank_infos or {},rank_info.rank_info_m or {}
end

--外部获取排行榜数据 全服（是跨服）个人排行榜 + 玩家排行信息 （跨服排行榜不是在跨服的排行榜，而是全服排行榜）
function SocietyData:GetRankCrossInfo(rank_type)
    local rank_info = self.rank_cross_info[rank_type] or {}
	return rank_info.rank_infos or {},rank_info.rank_info_m or {}
end