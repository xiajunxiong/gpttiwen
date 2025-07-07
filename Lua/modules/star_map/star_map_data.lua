StarMapData = StarMapData or BaseClass()

StarMapData.MaxStage = 4

function StarMapData:__init()
    if StarMapData.Instance ~= nil then
        Debuger.LogError("[StarMapData] attempt to create singleton twice!")
        return
    end
    StarMapData.Instance = self
    self.cfg = Config.starmap_auto
    self:InitCfg()

    self.super_star_data = SmartData.New()  -- 超星系
    self.star_data = SmartData.New()        -- 星图
    self.star_skill_data = SmartData.New()  -- 星图技能
    self.stage_data = SmartData.New()       -- 星图阶数

    self.sel_super_node = SmartData.New()   -- 选中的超星系节点
    self.sel_node = SmartData.New()         -- 选中的星图节点
    self.sel_tab_data = SmartData.New()     -- 选中的标签
    self.is_show_detail = SmartData.New({v = true}) -- 是否展示星图详情
    self.remind_data = SmartData.New({v = false})   -- 用于红点通知

    self.is_super_show_detail = false        -- 是否展示超星详情
    self.jump_super_seq = nil
    self.sel_selector = nil                 -- 记录下选中的toggle,方便关闭掉
    self.super_selector = nil
end

function StarMapData:ClearData()
	
end

function StarMapData:CheckRemind()
    self.remind_data.v = not self.remind_data.v
end

function StarMapData:InitCfg()
    -- 角色阶组配置
    self.role_stage_group_cfg = DataHelper.ConvertGroup(self.cfg.role_star, "grade")
    self.role_info_group_cfg = DataHelper.ConvertGroup(self.cfg.role_star_condition, "grade")
    -- 宠物阶组配置
    self.pet_stage_group_cfg = DataHelper.ConvertGroup(self.cfg.pet_star, "grade")
    self.pet_info_group_cfg = DataHelper.ConvertGroup(self.cfg.pet_star_condition, "grade")
    -- 伙伴阶组配置
    self.partner_stage_group_cfg = DataHelper.ConvertGroup(self.cfg.partner_star, "grade")
    self.partner_info_group_cfg = DataHelper.ConvertGroup(self.cfg.partner_star_condition, "grade")

    self.role_map_group_cfg_list = {}
    self.pet_map_group_cfg_list = {}
    self.partner_map_group_cfg_list = {}
    for i = 0, StarMapData.MaxStage do
        local role_cfg = self.role_stage_group_cfg[i]
        local pet_cfg = self.pet_stage_group_cfg[i]
        local partner_cfg = self.partner_stage_group_cfg[i]
        -- local info_role_cfg = self.role_info_group_cfg[i]
        -- local info_pet_cfg = self.pet_info_group_cfg[i]
        -- local info_partner_cfg = self.partner_info_group_cfg[i]
        self.role_map_group_cfg_list[i] = DataHelper.ConvertGroup(role_cfg, "star_id")
        self.pet_map_group_cfg_list[i] = DataHelper.ConvertGroup(pet_cfg, "star_id")
        self.partner_map_group_cfg_list[i] = DataHelper.ConvertGroup(partner_cfg, "star_id")

        -- 角色星图
        Cfg.Register("StarMapRoleCfg" .. i,Cfg.RealTimeSearch2Key(role_cfg, "star_id", "star_level"))
        -- 宠物星图
        Cfg.Register("StarMapPetCfg" .. i,Cfg.RealTimeSearch2Key(pet_cfg, "star_id", "star_level"))
        -- 伙伴星图
        Cfg.Register("StarMapPartnerCfg" .. i,Cfg.RealTimeSearch2Key(partner_cfg, "star_id", "star_level"))
    end

    -- self.role_map_group_cfg = DataHelper.ConvertGroup(self.cfg.role_star, "star_id")
    -- self.pet_map_group_cfg = DataHelper.ConvertGroup(self.cfg.pet_star, "star_id")
    -- self.partner_map_group_cfg = DataHelper.ConvertGroup(self.cfg.partner_star, "star_id")
    
    self.super_group_cfg = DataHelper.ConvertGroup(self.cfg.superstar, "star_id")
    self.skill_hole_group_cfg = DataHelper.ConvertGroup(self.cfg.hole_open, "starmap_tpye")

    Cfg.Register("StarMapRoleInfoCfg",Cfg.RealTimeSearch2Key(self.cfg.role_star_condition,"star_type","grade"))
    Cfg.Register("StarMapPetInfoCfg",Cfg.RealTimeSearch2Key(self.cfg.pet_star_condition,"star_type", "grade"))
    Cfg.Register("StarMapPartnerInfoCfg",Cfg.RealTimeSearch2Key(self.cfg.partner_star_condition,"star_type", "grade"))

    -- 超星系配置
    Cfg.Register("StarMapSuperCfg",Cfg.RealTimeSearch2Key(self.cfg.superstar, "star_id", "star_level"))
    -- 超星系信息
    Cfg.Register("StarMapSuperInfoCfg",Cfg.RealTimeSearch(self.cfg.superstar_condition,"star_type"))

    -- -- 角色星图
    -- Cfg.Register("StarMapRoleCfg",Cfg.RealTimeSearch2Key(self.cfg.role_star, "star_id", "star_level"))
    -- Cfg.Register("StarMapRoleInfoCfg",Cfg.RealTimeSearch(self.cfg.role_star_condition,"star_type"))
    -- -- 宠物星图
    -- Cfg.Register("StarMapPetCfg",Cfg.RealTimeSearch2Key(self.cfg.pet_star, "star_id", "star_level"))
    -- Cfg.Register("StarMapPetInfoCfg",Cfg.RealTimeSearch(self.cfg.pet_star_condition,"star_type"))
    -- -- 伙伴星图
    -- Cfg.Register("StarMapPartnerCfg",Cfg.RealTimeSearch2Key(self.cfg.partner_star, "star_id", "star_level"))
    -- Cfg.Register("StarMapPartnerInfoCfg",Cfg.RealTimeSearch(self.cfg.partner_star_condition,"star_type"))

    -- 技能配置
    Cfg.Register("StarMapSkillCfg",Cfg.RealTimeSearch2Key(self.cfg.skill_dec, "skill_id", "skill_level"))
    -- 技能孔配置
    Cfg.Register("StarMapSkillHoleCfg",Cfg.RealTimeSearch2Key(self.cfg.hole_open, "starmap_tpye", "skill_hole"))

    self.map_type_cfg = {
        [Mod.StarMap.Role] = StarMapCtrl.MapType.ROLE,
        [Mod.StarMap.Pet] = StarMapCtrl.MapType.PET,
        [Mod.StarMap.Partner] = StarMapCtrl.MapType.PARTNER,
    }
end

function StarMapData:SetAllData(protocol)
    self.super_star_data:Set(protocol.supper_star_list)
    self.star_data:Set(protocol.star_list)
    self.star_skill_data:Set(protocol.star_skill)
    self.stage_data:Set(protocol.stage_list)
end

function StarMapData:InstallSkillNotice(protocol)
    local super_seq = protocol.super_seq
    local level = self:GetSuperStarLevel(super_seq)
    level = level <= 0 and 1 or level
    local cfg = self:GetSuperLevelCfg(super_seq, level)
    local skill_id = cfg.param1
    local skill_level = cfg.param2

    local notice_type = protocol.notice_type or 0
    if notice_type == 0 then
        SkillData.Instance:PushNewSkillPopup({skill_id = skill_id, 
        is_star_map = true, skill_level = skill_level})
        ViewMgr:OpenView(NewSkillPopupView)
    elseif notice_type == 1 then
        SkillData.Instance:PushNewSkillPopup({skill_id = skill_id, 
        is_star_map = true, skill_level = skill_level, title_text = Language.StarMap.Text23})
        ViewMgr:OpenView(NewSkillPopupView)
    end
end

function StarMapData:GetMapType()
    local tab_data = self:GetSelTab()
    if TableIsEmpty(tab_data) then
        return
    end
    local mod = tab_data.open
    return self.map_type_cfg[mod]
end

function StarMapData:GetMapCfg(map_type, stage)
    return self:GetInfoGroupCfg(map_type, stage)
end

function StarMapData:GetSkillCfg(skill_id, level)
    level = level or 1
    return Cfg.StarMapSkillCfg(skill_id, level)
end

function StarMapData:GetStuffDataByCfg(cfg)
    local list_data = TableCopy(cfg.cost_item or {})
    if cfg.cost_coin and cfg.cost_coin > 0 then
        local data = {item_id = CommonItemId.Coin, num = cfg.cost_coin}
        table.insert(list_data, data)
    end
    return list_data
end

-- 获取超星位重置返回的材料  -->>小星图不可重置了
function StarMapData:GetMapResetBackStuff(map_type)
    local cfg_list = self:GetMapCfg(map_type)

    local list = {}
    local coin_reset_back = self.cfg.other[1].reset_back / 1000
    local add_func = function (item_id, num)
        if not list[item_id] then
            list[item_id] = 0
        end
        list[item_id] = list[item_id] + num
    end
    local coin_num = 0
    for i,v in pairs(cfg_list) do
        local star_id = v.star_type
        local level = self:GetStarLevel(map_type,star_id)
        if level > 0 then
            for j = 1, level do
                local cfg = self:GetLevelCfg(map_type, star_id, j)
                local item_list = cfg.cost_item
                for _,v in pairs(item_list) do
                    add_func(v.item_id, v.num)
                end
                coin_num = coin_num + cfg.cost_coin * coin_reset_back
            end
        end
    end
    local item_list = {}
    coin_num = math.floor(coin_num)
    if coin_num > 0 then
        local coin_item = Item.Create({item_id = CommonItemId.Coin, num = coin_num})
        table.insert(item_list, coin_item)
    end
    for i,v in pairs(list) do
        local item = Item.Create({item_id = i, num = v})
        table.insert(item_list, item)
    end
    return item_list
end

function StarMapData:IsCanReset(map_type)
    local cfg_list = self:GetMapCfg(map_type)
    local v = cfg_list[1]
    local star_id = v.star_type
    local level = self:GetStarLevel(map_type,star_id)
    return level > 0
end

function StarMapData:GetStage(map_type)
    local cur_stage = self.stage_data[map_type] or 0
    if cur_stage > StarMapData.MaxStage then
        return StarMapData.MaxStage
    end
    return cur_stage
end

function StarMapData:IsUnlockStage(map_type, stage)
    local cur_stage = self:GetStage(map_type)
    return stage <= cur_stage
end

-- ==================== 超级星图专区 ========================

function StarMapData:GetSuperGroupCfg(id)
    local group_cfg = self.super_group_cfg[id]
    return group_cfg or {}
end

function StarMapData:GetSuperLevelCfg(id, level)
    return Cfg.StarMapSuperCfg(id, level)
end

function StarMapData:GetSuperInfoCfg(id)
    return Cfg.StarMapSuperInfoCfg(id)
end

function StarMapData:GetSuperFirstCfg(id)
    local group_cfg = self:GetSuperGroupCfg(id)
    if TableIsEmpty(group_cfg) then
        LogDG("星图超星系获取配置错误 id == ", id)
        return {}
    end
    return group_cfg[1]
end

function StarMapData:GetSuperStarLevel(id)
    return self.super_star_data[id] or 0
end

function StarMapData:GetSuperMaxLevel(id)
    local group_cfg = self:GetSuperGroupCfg(id)
    return #group_cfg
end

function StarMapData:IsCanActiveSuper(id)
    if id == 0 then
        return true
    end
    local cfg = self:GetSuperInfoCfg(id)
    local adjoin_list = {}
    local t = Split(cfg.adjoin, "|")
    for i,v in pairs(t) do
        local seq = tonumber(v)
        local level = self:GetSuperStarLevel(seq)
        if level >= cfg.condition then
            return true
        end
    end
    return false
end

function StarMapData:IsActiveLineSuper(data)
    for i, v in pairs(data) do
        local level = self:GetSuperStarLevel(v)
        if level < 1 then
            return false
        end
    end
    return true
end

function StarMapData:SetSelSuperNode(data)
    self.sel_super_node:Set(data)
end

function StarMapData:GetSelSuperNode()
    return self.sel_super_node
end

function StarMapData:GetSuperAttriListByCfg(cfg)
    local list = {}
    if cfg.star_type ~= 0 then
        return list
    end
    local attri = {}
    attri.attri_type = cfg.param1
    attri.attri_value = cfg.param2
    attri.target = cfg.target
    table.insert(list, attri)
    return list
end

-- 1可升级，0材料不足，-1已满级, -2 不可激活, -3 铜币不足
function StarMapData:GetSuperNodeUpFlag(star_id)
    local level = self:GetSuperStarLevel(star_id)
    local max_level = self:GetSuperMaxLevel(star_id)
    if level >= max_level then
        return -1
    end
    if not self:IsCanActiveSuper(star_id) then
        return -2
    end
    
    local cfg = self:GetSuperLevelCfg(star_id, level + 1)
    local has_coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    if has_coin < cfg.cost_coin then
        return -3
    end
    for i,v in pairs(cfg.cost_item) do
        local has_num = Item.GetNum(v.item_id)
        if has_num < v.num then
            return 0, v.item_id
        end
    end
    return 1
end

-- 超星系获得的属性和技能列表
function StarMapData:GetSuperMapAttriAndSkillList(t_type)
    t_type = t_type or StarMapCtrl.MapType.ROLE
    local cfg_list = self:GetMapCfg(StarMapCtrl.MapType.Super)
    local list = {}
    local skill_list = {}
    for i,v in pairs(cfg_list) do
        local star_id = v.star_type
        local level = self:GetSuperStarLevel(star_id)
        if level > 0 then
            local cfg = self:GetSuperLevelCfg(star_id, level)
            if cfg.target == t_type then
                if cfg.star_type == 1 then
                    local skill_id = cfg.param1
                    local skill_level = cfg.param2
                    local skill_cfg = self:GetSkillCfg(skill_id, skill_level)
                    table.insert(skill_list, skill_cfg)
                else
                    local attri_type = cfg.param1
                    local attri_value = cfg.param2
                    if not list[attri_type] then
                        list[attri_type] = 0
                    end
                    list[attri_type] = list[attri_type] + attri_value
                end
            end
        end
    end
    local attri_list = {}
    for i,v in pairs(list) do
        local attri = {}
        attri.attri_type = i
        attri.attri_value = v
        table.insert(attri_list, attri)
    end
    table.sort(attri_list, function (a, b)
        return a.attri_type < b.attri_type
    end)
    return attri_list, skill_list
end

function StarMapData:GetSuperSkillCfg(id)
    local level = StarMapData.Instance:GetSuperStarLevel(id)
    if level == 0 then
        return
    end
    local cfg = StarMapData.Instance:GetSuperLevelCfg(id, level)
    if cfg.star_type == 1 then
        local skill_id = cfg.param1
        local skill_level = cfg.param2
        local skill_cfg = StarMapData.Instance:GetSkillCfg(skill_id, skill_level)
        return skill_cfg
    end
end

function StarMapData:GetSuperSkillListData(map_type)
    local list = {}
    local group_cfg = self.super_group_cfg
    for i,v in pairs(group_cfg) do
        local first_cfg = v[1]
        if first_cfg.star_type == 1 and first_cfg.target == map_type then
            local level = self:GetSuperStarLevel(i)
            if level > 0 then
                local cfg = v[level]
                table.insert(list, cfg)
            else
                table.insert(list, first_cfg)
            end
        end
    end
    table.sort(list, function (a,b)
        local a_num = self:IsUsedSkillBySuperSeq(a.star_id) and 10000 or 0
        local b_num = self:IsUsedSkillBySuperSeq(b.star_id) and 10000 or 0
        local a_active = self:GetSuperStarLevel(a.star_id) > 0 and 1 or 0
        local b_active = self:GetSuperStarLevel(b.star_id) > 0 and 1 or 0
        a_num = a_num + a_active * 1000
        b_num = b_num + b_active * 1000
        if a_num == b_num then
            a_num = self:GetSuperStarLevel(a.star_id) * 10
            b_num = self:GetSuperStarLevel(b.star_id) * 10
            if a_num == b_num then
                return a.param1 < b.param1
            end
            return a_num > b_num
        end
        return a_num > b_num
    end)
    return list
end

function StarMapData:IsUsedSkillBySuperSeq(seq)
    for i,v in pairs(self.star_skill_data) do
        for j, v2 in pairs(v) do
            if v2 == seq then
                return true
            end
        end
    end
    return false
end

function StarMapData:SetJumpSuper(seq)
    self.jump_super_seq = seq
end
function StarMapData:GetJumpSuper()
    return self.jump_super_seq
end

-- ============================================


-- ========================= 星图 ========================
-- 当前选中的标签页
function StarMapData:GetSelTab()
    return self.sel_tab_data:Val()
end
function StarMapData:SetSelTab(tab_data)
    self.sel_tab_data:Set(tab_data or {})
end

function StarMapData:GetSelNode()
    return self.sel_node:Val()
end
function StarMapData:SetSelNode(node_data)
    self.sel_node:Set(node_data or {})
end

-- 星图放大的区域，返回技能孔
function StarMapData:GetMaxMapHole()
    local node = self:GetSelNode()
    if TableIsEmpty(node) then -- or node.star_type == 0 then
        return -1
    else
        return node.skill_hole
    end
end

function StarMapData:GetGroupCfg(map_type, id, stage)
    stage = stage or 0
    if map_type == StarMapCtrl.MapType.ROLE then
        local group_cfg = self.role_map_group_cfg_list[stage]
        return group_cfg[id] or {}
    elseif map_type == StarMapCtrl.MapType.PET then
        local group_cfg = self.pet_map_group_cfg_list[stage]
        return group_cfg[id] or {}
    elseif map_type == StarMapCtrl.MapType.PARTNER then
        local group_cfg = self.partner_map_group_cfg_list[stage]
        return group_cfg[id] or {}
    elseif map_type == StarMapCtrl.MapType.Super then
        return self.super_group_cfg[id] or {}
    end
    return {}
end

function StarMapData:GetInfoGroupCfg(map_type, stage)
    stage = stage or 0
    if map_type == StarMapCtrl.MapType.Super then
        return self.cfg.superstar_condition
    end
    if map_type == StarMapCtrl.MapType.ROLE then
        return self.role_info_group_cfg[stage]
    elseif map_type == StarMapCtrl.MapType.PET then
        return self.pet_info_group_cfg[stage]
    elseif map_type == StarMapCtrl.MapType.PARTNER then
        return self.partner_info_group_cfg[stage]
    end
end

function StarMapData:GetLevelCfg(map_type,id, level, stage)
    stage = stage or 0
    if map_type == StarMapCtrl.MapType.Super then
        return Cfg.StarMapSuperCfg(id, level)
    end
    local cfg_name
    if map_type == StarMapCtrl.MapType.ROLE then
        cfg_name = "StarMapRoleCfg" .. stage
    elseif map_type == StarMapCtrl.MapType.PET then
        cfg_name = "StarMapPetCfg" .. stage
    elseif map_type == StarMapCtrl.MapType.PARTNER then
        cfg_name = "StarMapPartnerCfg" .. stage
    end
    if not Cfg[cfg_name] then
        LogDG("星图获取等级配置报错", cfg_name, stage)
    end
    return Cfg[cfg_name](id, level)
end

function StarMapData:GetInfoCfg(map_type, id, stage)
    stage = stage or 0
    if map_type == StarMapCtrl.MapType.Super then
        return Cfg.StarMapSuperInfoCfg(id)
    end
    if map_type == StarMapCtrl.MapType.ROLE then
        return Cfg.StarMapRoleInfoCfg(id, stage)
    elseif map_type == StarMapCtrl.MapType.PET then
        return Cfg.StarMapPetInfoCfg(id, stage)
    elseif map_type == StarMapCtrl.MapType.PARTNER then
        return Cfg.StarMapPartnerInfoCfg(id, stage)
    end
end

function StarMapData:GetFirstCfg(map_type,id, stage)
    stage = stage or 0
    local group_cfg = self:GetGroupCfg(map_type,id, stage)
    if TableIsEmpty(group_cfg) then
        LogDG("星图超星系获取配置错误 type and id == ", map_type,id)
        return {}
    end
    return group_cfg[1]
end

function StarMapData:GetSkillHoleListCfg(map_type)
    return self.skill_hole_group_cfg[map_type] or {}
end

function StarMapData:GetHoleSkillInfo(map_type, hole_index)
    if TableIsEmpty(self.star_skill_data) or TableIsEmpty(self.star_skill_data[map_type]) then
        return -1
    end
    local super_seq = self.star_skill_data[map_type][hole_index] or 0
    return super_seq
end

function StarMapData:GetStarLevel(map_type,id, stage)
    stage = stage or 0
    if TableIsEmpty(self.star_data) then
        return 0
    end
    if map_type == StarMapCtrl.MapType.Super then
        return self:GetSuperStarLevel(id)
    end
    if map_type == nil or id == nil then
        LogDG(string.format("异常map_type == %s, id == %s", map_type, id))
        return 0
    end
    return self.star_data[map_type][stage][id] or 0
end

function StarMapData:GetMaxLevel(map_type,id, stage)
    stage = stage or 0
    local group_cfg = self:GetGroupCfg(map_type,id,stage)
    return #group_cfg
end

function StarMapData:IsCanActive(map_type,id, stage)
    stage = stage or 0
    if id == 0 then
        return true
    end
    local cfg = self:GetInfoCfg(map_type,id,stage)
    local adjoin_list = {}
    local t = Split(cfg.adjoin, "|")
    for i,v in pairs(t) do
        local seq = tonumber(v)
        local level = self:GetStarLevel(map_type,seq,stage)
        if level >= cfg.condition then
            return true
        end
    end
    return false
end

function StarMapData:GetAttriListByCfg(map_type,cfg)
    local list = {}
    if map_type == StarMapCtrl.MapType.ROLE then
        local attri = {}
        local prof_type = RoleData.Instance:ProfType()
        attri.attri_type = cfg["att_type" .. prof_type]
        attri.attri_value = cfg["att" .. prof_type]
        attri.level = cfg.star_level or 0
        table.insert(list, attri)
    else
        local attri = {}
        attri.attri_type = cfg.att_type
        attri.attri_value = cfg.att
        attri.level = cfg.star_level or 0
        table.insert(list, attri)
    end
    return list
end

-- 1可升级，0材料不足，-1已满级, -2 不可激活, -3 铜币不足
function StarMapData:GetNodeUpFlag(map_type, star_id, stage)
    stage = stage or 0
    local level = self:GetStarLevel(map_type, star_id, stage)
    local max_level = self:GetMaxLevel(map_type, star_id, stage)
    if level >= max_level then
        return -1
    end
    if not self:IsCanActive(map_type, star_id, stage) then
        return -2
    end
    
    local cfg = self:GetLevelCfg(map_type, star_id, level + 1, stage)
    local has_coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    if has_coin < cfg.cost_coin then
        return -3
    end
    for i,v in pairs(cfg.cost_item) do
        local has_num = Item.GetNum(v.item_id)
        if has_num < v.num then
            return 0, v.item_id
        end
    end
    return 1
end

-- 星图获得的属性和技能列表
function StarMapData:GetMapAttriAndSkillList(map_type)
    local list = {}
    for stage = 0, StarMapData.MaxStage do
        local cfg_list = self:GetMapCfg(map_type, stage)
        for i,v in pairs(cfg_list) do
            local star_id = v.star_type
            local level = self:GetStarLevel(map_type,star_id, stage)
            if level > 0 then
                local cfg = self:GetLevelCfg(map_type,star_id, level, stage)
                local attri_list = self:GetAttriListByCfg(map_type, cfg)
                for _, attri in pairs(attri_list) do
                    if not list[attri.attri_type] then
                        list[attri.attri_type] = 0
                    end
                    list[attri.attri_type] = list[attri.attri_type] + attri.attri_value
                end
            end
        end
    end
    
    local attri_list = {}
    for i,v in pairs(list) do
        local attri = {}
        attri.attri_type = i
        attri.attri_value = v
        table.insert(attri_list, attri)
    end
    table.sort(attri_list, function (a, b)
        return a.attri_type < b.attri_type
    end)
    local skill_list = {}
    local data_list = self.star_skill_data[map_type] or {}
    if not TableIsEmpty(data_list) then
        for i,v in pairs(data_list) do
            if v > 0 then
                local super_level = self:GetSuperStarLevel(v)
                local cfg = self:GetSuperLevelCfg(v, super_level)
                if cfg.star_type == 1 then
                    local skill_id = cfg.param1
                    local skill_level = cfg.param2
                    local skill_cfg = self:GetSkillCfg(skill_id, skill_level)
                    table.insert(skill_list, skill_cfg)
                else
                    LogDG("服务端下发的技能信息有误", v)
                end
            end
        end
    end
    return attri_list, skill_list
end

function StarMapData:SetShowDetail(v)
    v = v or false
    self.is_show_detail.v = v
end
function StarMapData:GetShowDetail()
    return self.is_show_detail.v or false
end



-- [[[[[[[[[[[[[[ 红点逻辑区 ]]]]]]]]]]]]]]

function StarMapData:RemindItemList()
    if not self.remind_item_list then
        self.remind_item_list = {}
        local item_id = self.cfg.other[1].superstar_id
        table.insert(self.remind_item_list, item_id)
        local cfg = self.cfg.role_star[1]
        for i,v in pairs(cfg.cost_item) do
            table.insert(self.remind_item_list, v.item_id)
        end
        cfg = self.cfg.pet_star[1]
        for i,v in pairs(cfg.cost_item) do
            table.insert(self.remind_item_list, v.item_id)
        end
        cfg = self.cfg.partner_star[1]
        for i,v in pairs(cfg.cost_item) do
            table.insert(self.remind_item_list, v.item_id)
        end
        cfg = self.cfg.superstar[1]
        for i,v in pairs(cfg.cost_item) do
            table.insert(self.remind_item_list, v.item_id)
        end
    end
    return self.remind_item_list
end

function StarMapData:GetMapRemind(map_type, stage)
    local cfg = self:GetInfoGroupCfg(map_type, stage)
    local is_unlock = self:IsUnlockStage(map_type, stage)
    if not is_unlock then
        return 0
    end
    for i,v in pairs(cfg) do
        local id = v.star_type
        local flag = self:GetNodeUpFlag(map_type, id , stage)
        if flag > 0 then
            return 1
        end
    end
    return 0
end

-- 角色星图红点
function StarMapData:RemindRole()
    for i = 0, StarMapData.MaxStage do
        local flag = self:GetMapRemind(StarMapCtrl.MapType.ROLE, i)
        if flag > 0 then
            return 1
        end
    end
    return 0
end

-- 宠物星图红点
function StarMapData:RemindPet()
    for i = 0, StarMapData.MaxStage do
        local flag = self:GetMapRemind(StarMapCtrl.MapType.PET, i)
        if flag > 0 then
            return 1
        end
    end
    return 0
end

-- 伙伴星图红点
function StarMapData:RemindPartner()
    for i = 0, StarMapData.MaxStage do
        local flag = self:GetMapRemind(StarMapCtrl.MapType.PARTNER, i)
        if flag > 0 then
            return 1
        end
    end
    return 0
end

-- 超星系星图红点
function StarMapData:RemindSuper()
    local cfg = self.cfg.superstar_condition
    for i,v in pairs(cfg) do
        local id = v.star_type
        local flag = self:GetSuperNodeUpFlag(id)
        if flag > 0 then
            return 1
        end
    end
    return 0
end




-- ================= 星图节点实例 ============

StarMapNode = StarMapNode or BaseClass()
function StarMapNode.Create(vo)
    local map_type = vo.map_type
    local star_id = vo.star_type
    local stage = vo.grade
    return StarMapNode.New(map_type, star_id, stage)
end

function StarMapNode:__init(map_type, id, stage)
    self.map_type = map_type
    self.id = id
    self.stage = stage or 0
end

function StarMapNode:BaseCfg()
    return StarMapData.Instance:GetInfoCfg(self.map_type, self.id, self.stage)
end

function StarMapNode:FirstCfg()
    return StarMapData.Instance:GetFirstCfg(self.map_type, self.id, self.stage)
end

function StarMapNode:LevelCfg()
    local level = self:Level()
    if level == 0 then
        return self:FirstCfg()
    end
    return StarMapData.Instance:GetLevelCfg(self.map_type, self.id, level, self.stage)
end

function StarMapNode:LevelDesc(show_type)
    show_type = show_type or 1
    local level = self:Level()
    if show_type == 1 then
        if level == 0 then
            return Language.StarMap.NotActive
        else
            return "Lv." .. level
        end
    end
end

function StarMapNode:NextLevelDesc()
    if self:IsMax() then
        return ""
    end
    local level = self:Level() + 1
    return "Lv." .. level
end

function StarMapNode:LevelAndMaxDesc(show_type)
    show_type = show_type or 1
    local level = self:Level()
    local max_level = self:MaxLevel()
    if show_type == 1 then
        return string.format(Language.StarMap.Level1, level, max_level)
    else
        return string.format("%s/%s", level, max_level)
    end
end

function StarMapNode:NextLevelCfg()
    if self:IsMax() then
        return self:LevelCfg()
    else
        local level = self:Level()
        return StarMapData.Instance:GetLevelCfg(self.map_type, self.id, level + 1, self.stage)
    end
end

function StarMapNode:AttriList()
    local cfg = self:LevelCfg()
    local list = StarMapData.Instance:GetAttriListByCfg(self.map_type,cfg)
    return list
end

function StarMapNode:NextAttriList()
    local cfg = self:NextLevelCfg()
    local list = StarMapData.Instance:GetAttriListByCfg(self.map_type,cfg)
    return list
end

function StarMapNode:ShowAddAttriList()
    local cur_attri_list = self:AttriList()
    local next_attri_list = self:NextAttriList()
    local list = {}
    for i,v in pairs(cur_attri_list) do
        local attri = TableCopy(v)
        attri.next_value = 0
        for _,v2 in pairs(next_attri_list) do
            if attri.attri_type == v2.attri_type then
                attri.next_value = v2.attri_value
                break
            end
        end
        table.insert(list, attri)
    end
    return list
end

function StarMapNode:AttriBG()
    local is_active = self:IsActive()
    local img = is_active and "ShuxingdiJihuo" or "XingWeiDi"
    return img
end

function StarMapNode:AttriIcon()
    local attri_list = self:AttriList()
    local attri = attri_list[1] or {}
    if not attri.attri_type then
        LogDG("星图属性图标异常", attri_list)
    end
    return "Attri" .. (attri.attri_type or 0)
end

function StarMapNode:Name()
    local cfg = self:FirstCfg()
    return cfg.star_name
end

function StarMapNode:Level()
    return StarMapData.Instance:GetStarLevel(self.map_type, self.id, self.stage)
end

function StarMapNode:MaxLevel()
    return StarMapData.Instance:GetMaxLevel(self.map_type, self.id, self.stage)
end

function StarMapNode:IsMax()
    local level = self:Level()
    local max_level = self:MaxLevel()
    return level >= max_level
end

function StarMapNode:IsActive()
    local level = self:Level()
    return level > 0
end

function StarMapNode:IsCanActive()
    return StarMapData.Instance:IsCanActive(self.map_type, self.id, self.stage)
end

function StarMapNode:StuffList()
    if self:IsMax() then
        return {}
    end
    local cfg = self:NextLevelCfg()
    return StarMapData.Instance:GetStuffDataByCfg(cfg)
end

function StarMapNode:GetCtrlFlag()
    return StarMapData.Instance:GetNodeUpFlag(self.map_type, self.id, self.stage)
end

function StarMapNode:Condition()
    local cfg = self:BaseCfg()
    local condition = cfg.condition
    return condition
end