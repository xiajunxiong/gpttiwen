RoleInfoType = {
    role = 1,
    group = 2,
}

RoleInfoData = RoleInfoData or BaseClass()

function RoleInfoData:__init()
    if RoleInfoData.Instance ~= nil then
        Debuger.LogError("[RoleInfoData] attempt to create singleton twice!")
        return
    end 
    RoleInfoData.Instance = self
    self.partner_skill_cfg = {}
    self.partner_upgrade_cfg = {}
    self:InitData()
    self:RoleInfoData()
end

function RoleInfoData:InitData()
    Cfg.Register("PartnerGiftCfg",Cfg.RealTimeSearch2Key(Config.partner_cfg_auto.gift_list,"gift_id","gift_level"))
    self:InitHelper(Config.partner_cfg_auto.skill_list,self.partner_skill_cfg,"partner_id")
    self:InitHelper(Config.partner_cfg_auto.partner_upgrade,self.partner_upgrade_cfg,"partner_id")
end

--数据整理
function RoleInfoData:InitHelper(base_cfg,cfg,key)
    for i=1,#base_cfg do
        local v = base_cfg[i]
        v.index = i
        if cfg[v[key]] == nil then
            cfg[v[key]] = {}
        end
        table.insert(cfg[v[key]],v)
    end
end

function RoleInfoData:RoleInfoData()
    self.role_info = {}
    self.role_equips = SmartData.New{
        equipment_list = {},
        jewelry_list = {},
        prof = 0,
        level = 1,
    }
    self.query_role_info = {}
    self.query_role_data = SmartData.New({val = false})
    self.view_equip_uid = 0
    self:ResetRoleAccrssory()
end

function RoleInfoData:ResetRoleAccrssory()
    self.role_accrssory = SmartData.New({
        info = {},
        skill = {},
    })
end

function RoleInfoData:GetRoleAccrssory()
    return self.role_accrssory
end

function RoleInfoData:SetAccrssoryData(info)
    self.role_accrssory.info = info.equipment_list
    self.role_accrssory.skill = AccrssoryData.Instance:CalculateSkill(self.role_accrssory.info)
end

function RoleInfoData:SetRoleInfo(info)
    info = info or {}
    self.role_info.info = nil ~= info.info and info.info() or {}
    if nil ~= info.funcs then
        self.role_info.funcs = info.funcs()
    end
    self.role_info.hide_base = info.hide_base
    self.role_info.pos = info.pos
end

function RoleInfoData:RoleInfo()
    return self.role_info
end

-- func_info(自定义方法所需的info)
-- uid/name ..(填了显示,不填不显示,自动排版)
function RoleInfoData.RoleInfoNew(params)
    return {
        type = RoleInfoType.role,
        func_info = params.func_info or params,
        uid = params.uid or params.role_id,
        name = params.name,
        level = params.level,
        prof = params.prof,
        prof_id = params.prof_id,
        family = params.family,
        avatar_type = params.avatar_type,
        avatar_id = params.avatar_id,
        avatar_quality = params.avatar_quality,
        top_level = params.top_level,
    }
end

function RoleInfoData.GroupInfoNew(params)
    return {
        type = RoleInfoType.group,
        func_info = params.func_info,
        group_id = params.group_id,
        group_name = params.group_name,
        member_count = params.member_count,
        group_avatars = params.group_avatars,
    }
end

function RoleInfoData:SetRoleRoundInfo(info)
    self.role_round_info = info or {}
end

function RoleInfoData:RoleRoundInfo()
    table.sort(self.role_round_info, function (a, b)
        if not a or not b then return a end
        local at = (a.obj_type and SceneObjType.Role == a.obj_type) and 0 or 1
        local bt = (b.obj_type and SceneObjType.Role == b.obj_type) and 0 or 1
        return at > bt
    end)
    return self.role_round_info
end

--设置伙伴信息
function RoleInfoData:SetPartnerData(partner_list)
    self.partner_data = {}
    for i = 1,#partner_list do
        local v = partner_list[i]
        local partner_data = self:PartnerInfoNew(v)
        if partner_data then
            table.insert(self.partner_data,partner_data)
        end
    end
end

--获取伙伴信息
function RoleInfoData:GetPartnerData()
    return self.partner_data
end

--转换成伙伴信息
function RoleInfoData:PartnerInfoNew(data)
    local info = {}
    local partner_cfg = Cfg.PartnerById(data.partner_id)
    if partner_cfg == nil then
        LogError("异常Log需检查！查询伙伴信息为空，partner_id == ",data.partner_id)
        return nil
    end
    info.icon_id = partner_cfg.icon_id
    info.name = partner_cfg.name
    info.job = partner_cfg.job
    info.lv = data.level
    info.cap = data.capability
    info.cap_lv = self:GetPartnerScoreLv(info.job,info.lv,info.cap)
    info.skin_id = data.skin_id
    info.res_id = partner_cfg.res_id
    info.quality = data.quality
    info.quality_segment = data.quality_segment
    info.upgrade_lv = data.upgrade_lv
    info.soar_count = data.soar_count
    info.intensify_level = data.intensify_level
    info.rank = data.rank
    local skill_list = self.partner_skill_cfg[data.partner_id]
    local upgrade_info = self:GetUpgradeInfo(data.partner_id,data.upgrade_lv)
    local quality_cfg = self:GetQualityInfo(data.partner_id,data.quality,data.quality_segment)
    info.skill_desc_list = {}
    for _,v in pairs(skill_list) do
        local skill_lv = quality_cfg["skill_level_"..v.skill_index]
        if skill_lv > 0 then
            local desc = Cfg.SkillDescPartnerAndRole(v.skill_id,skill_lv)
            table.insert(info.skill_desc_list,desc)
        end
    end
    return info
end

--获取伙伴进阶信息
function RoleInfoData:GetUpgradeInfo(partner_id,lv)
    local upgrade_data = self.partner_upgrade_cfg[partner_id]
    for _,v in pairs(upgrade_data) do
        if v.level == lv then
            return v
        end
    end
end

--获取伙伴品质信息
function RoleInfoData:GetQualityInfo(partner_id,qua,seg)
    return PartnerData.Instance:GetPartnerQualityCfg(partner_id,PartnerConfig.ChangeServerQua[qua],seg)
end

--获取伙伴评分
function RoleInfoData:GetPartnerScoreLv(job,lv,cap)
    local co = Cfg.PartnerScoreInfoByJobLevel(job,lv)
    if co.score_s <= cap then
        return "scoreLevel3"
    elseif co.score_a <= cap then
        return "scoreLevel2"
    elseif co.score_b <= cap then
        return "scoreLevel1"
    else
        return "scoreLevel0"
    end
end

--获取宠物列表
function RoleInfoData:GetPetList(is_sort)
    is_sort = is_sort or true
    if is_sort then
        local pet_list = self.role_equips.pet_list
        local sort_func = function (a,b)
            if a.pet_state == b.pet_state then
                return a.index < b.index
            else
                return a.pet_state < b.pet_state
            end
        end
        table.sort(pet_list,sort_func)
        return pet_list
    else
        return self.role_equips.pet_list
    end
end

--解析人物装备信息
function RoleInfoData:GetRoleEquipmentList(equipment_info_list)
    local equipment_list = {}
	for k,v in pairs(equipment_info_list) do
        equipment_list[k] = Equip.New(v)
        equipment_list[k].in_bag = false
    end
    return equipment_list
end

--解析人物灵饰信息
function RoleInfoData:GetRoleJewelryList(jewelry_info_list)
    local jewelry_list = {}
    for k,v in pairs(jewelry_info_list) do
        if 0 ~= v.item_id then
            jewelry_list[k] = Jewelry.New(v)
            jewelry_list[k].in_bag = false
        end
	end
    return jewelry_list
end

--===========================查询玩家装备信息下发===========================
function RoleInfoData:SetRoleEquipmentInfor(protocol)
    self.role_equips.role_id = protocol.query_role_info.role_id
    self.role_equips.medal_list = MedalData.GetRoleMedalData(protocol.medal_list)
    self.role_equips.jewelry_list = self:GetRoleJewelryList(protocol.jewelry_list)
    self.role_equips.equipment_list = self:GetRoleEquipmentList(protocol.equipment_list)

    self.role_equips.pet_list = protocol.pet_list
    self.role_equips.partner_list = protocol.partner_list
    self.role_equips.face_equip_list = protocol.face_equip_list
    self.role_equips.bicycle_list = protocol.bicycle_list
    self.role_equips.sheng_qi_list = protocol.sheng_qi_list

    self:SetAccrssoryData(protocol.accrssory_info)--配饰信息
    self:SetPartnerData(protocol.partner_list)
    self:SetQueryRoleInfo(protocol)--人物基本信息在query_role_info用 GetQueryRoleInfo 获取
end
--===========================人物查询后续信息追加下发===========================
function RoleInfoData:SetRoleEquipmentRank(protocol)
    self.role_equips.comp_rank = protocol.comp_rank
    self.role_equips.prof_rank = protocol.prof_rank
end

--设置当前需要打开的人物信息界面uid
function RoleInfoData:SetViewEquipUid(uid)
    self.view_equip_uid = uid
end

function RoleInfoData:GetRoleEquips()
    return self.role_equips
end
--[[ query_role_info = {
    uid        
    role_id        
    name 
    role_name 
    guild_name
    avatar_type
    prof
    level 
    top_level 
    capability 
    appearance
}]]
--保存需要查找的玩家 SendReferRoleEquipment 请求
function RoleInfoData:SetQueryRoleInfo(protocol)
    local info = protocol.query_role_info
    self.query_role_info[info.role_id] = info
    self.query_role_data.val = not self.query_role_data.val
end

--获取查询的玩家信息
function RoleInfoData:GetQueryRoleInfo(role_id)
    return self.query_role_info[role_id]
end