WorkshopData = WorkshopData or BaseClass()
ComposeGroup = {
    Forging = 1,
    Life = 2,
    EleCrystal = 3,
    Stone = 4,
    Map = 5,
    Card = 6,
    Skill = 7,
    FabaoSP = 8,
    Key = 9,
    PetFlush = 10,
    Jewelry = 11,
    PetEgg = 12,
    MarbasRefine = 14,
    DragonBall = 16
}
ForginQua = {
    "",
    "duanzao_pinzhilv",
    "duanzao_pinzhilan",
    "duanzao_pinzhizi",
    "duanzao_pinzhicheng",
    "duanzao_pinzhihong"
}
JewelryQua = {
    "",
    "WuPinKuangPinZhiLv",--"duanzao_pinzhilv",
    "WuPinKuangPinZhiLan",--"duanzao_pinzhilan",
    "WuPinKuangPinZhiZi",--"duanzao_pinzhizi",
    "WuPinKuangPinZhiCheng",--"duanzao_pinzhicheng",
    "WuPinKuangPinZhiHong",--"duanzao_pinzhihong"
    "WuPinKuangPinZhiJin",--"duanzao_pinzhijin"
}
WorkshopData.Params = {
    MinLevel = 4,
    MaxLevel = 13,
    MinJewerlyLevel = 4,
    MaxJewerlyLevel = 13,
}
SmeltStatus = {
    Lock = 0,
    UnLock = 1,
}
WorkshopData.JewerlyProf = {
    [RoleProfessionType.QiGongShi] = 31,
    [RoleProfessionType.JianXiaKe] = 32,
    [RoleProfessionType.KuangZhanShi] = 33,
    [RoleProfessionType.TieWei] = 34,
    [RoleProfessionType.ShenSheShou] = 35,
    [RoleProfessionType.XingShuShi] = 36,
    [RoleProfessionType.YiShuShi] = 37,
    [RoleProfessionType.FuZhouShi] = 38,
}
function WorkshopData:__init()
    if WorkshopData.Instance ~= nil then
        Debuger.LogError("[WorkshopData] attempt to create singleton twice!")
        return
    end
    WorkshopData.Instance = self
    self.compose_auto = Config.compose_auto
    self.compose_type_list = self.compose_auto.type_list
    self.compose_group_list = self.compose_auto.group_list
    self.compose_list = self.compose_auto.compose_list
    self.exchange_item_group = self.compose_auto.exchange_item_group
    self.exchange_list = self.compose_auto.exchange
    self.jewel_cfg_auto = Config.jewel_cfg_auto
    self.jewel_list = self.jewel_cfg_auto.jewel_list
    self.jewel_limit_level = self.jewel_cfg_auto.limit_level
    self.jewel_group = self.jewel_cfg_auto.jewel_team
    self.jewel_set = self.jewel_cfg_auto.jewel_set
    self.jewel_recommend = self.jewel_cfg_auto.recommend_jewel
    self.ornament_cfg_auto = Config.ornament_cfg_auto
    self.ornament_forge = self.ornament_cfg_auto.ornament_forge
    self.ornament_suit = self.ornament_cfg_auto.suit
    self.jg_consume = self.compose_auto.jg_consume
    self.resolve_cfg = self.compose_auto.equip_resolve
    self.upgrade_config = Config.equip_hole_strengthen_auto
    -- self.upgrade_hole_config = self.upgrade_config.equip_hole_upgrade
    self.upgrade_combo_config = self.upgrade_config.godjewelry_strengthen
    self.upgrade_break_config = self.upgrade_config.equip_break

    self.type_show_list = {}
    self.forging_show_list = {}
    self.forging_data = SmartData.New({select_prof = 1, select_level = WorkshopData.Params.MinLevel,auto_create = false})
    self.forged_flag = SmartData.New({flag = false})
    self.crystal_data = SmartData.New({select_crystal = 9})
    self.crystal_compose_data = SmartData.New({select_type = 9})
    self.compose_data = SmartData.New({select_type = ComposeGroup.Skill})
    self.complete_item = SmartData.New({item = nil})
    self.active_info = SmartData.New()--配方激活
	self.create_info = SmartData.New()--配方制造
    self.neveal_info = SmartData.New({})
    self.quick_open_jewel_index = -1
    self.jewel_info_list = {}--宝石信息
    self.jewel_group_list = {}--宝石分组
    ----------jewerly neo-----------
    self.jewelry_data = SmartData.New({
        select_prof = 33, 
        select_level = WorkshopData.Params.MinJewerlyLevel,
        select_index = 1,
        auto_create = false  
    })
    self.jewelry_cache = {}

    -- 装备宝石分组
    -- 每次只刷新子列表
    self.equip_jewel_list = {}
    self.jewel_set_list = {}--可镶嵌信息
    self.jewel_team_list = {} --宝石分组包括快速购买信息
    self.jewelry_group_list = {}

    self.select_jewelry = SmartData.New()
    self.skill_group = 1
    self.skill_select_max = 10
    self.skill_book_data = SmartData.New({total_num = 0})
    --选择技能书列表
    self.skill_select_list = {}
    self.compose_result = nil
    self.smelt_select = SmartData.New({data = nil})
    self.smelt_select_lock = SmartData.New({str = 0})
    self.smelt_time_limit = SmartData.New({time = 0})
    self.smelt_range_cache = {}
    self.smelt_click_mark = nil
    self.red_point = SmartData.New({
        total = false,
        tab1 = false,
        tab2 = false,
        tab3 = false,
        tab4 = false
    })

    self.forging_compose_list = {}
    self.skillbook_group_list = {}
    self.jewelry_compose_list = {}

    self.workshop_jewelryinfo_result = nil
    self:init_config()

    self.activation_config = KeyList(Config.compose_auto.compose_list,"activation")
    self.active_seq_config = KeyList(Config.compose_auto.compose_list,"active_seq")
    self.compose_seq_config = KeyList(Config.compose_auto.compose_list,"seq")
    self.compose_target_config = KeyList(Config.compose_auto.compose_list,"target")

    self:InitLockCheckList()
    self:InitQueChingList()

    self.forge_mark_data = nil
    self.jewerly_mark_data = nil

    self.level_show ={
        jewerly = KeyList(Language.Workshop.JewerlyLevel,"level")
    } 
    self.level_handle = EventSys:Bind(GlobalEventKeys.RoleLevelChange,
    BindTool.Bind(self.OnRoleLevelChange, self))
    self.prof_handle = EventSys:Bind(GlobalEventKeys.RoleProfessionChange,
    BindTool.Bind(self.OnRoleProfessionChange, self))
    self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,
    BindTool.Bind(self.OnRoleNoticeComplete, self))

    self.smelt_func_open = SmartData.New({red = false,clear_red = false})
    self.smelt_red_equip_list = {}

    self.upgrade_remind_sort = SmartData.New({str = ""})
    self.blazestar_info = SmartData.New({
        select_level = 0,
        select_prof = 0,
        select_data = nil,
        operate = nil,
        obl_left = nil,
        obl_right = nil,
        quick_tick = false,
        remind_level = 0,
    })

    self.remind_block = SmartData.New({forge = false,jewerly = false,flush = 1})
    self.compose_stuff_cache = {forge = {},jewelry = {}}

    self.neveal_extra_plus_info = SmartData.New({main = nil,stuff_1 = nil,stuff_2 = nil,select_type = -1,is_show = false})
end

function WorkshopData:__delete()
    EventSys:UnBind(self.prof_handle)
	EventSys:UnBind(self.level_handle)
	EventSys:UnBind(self.complete_handle)
end

function WorkshopData:ClearData()
    self.compose_stuff_cache = {forge = {},jewelry = {}}
    self.cache_item = nil
    self.remind_block.forge = false
    self.remind_block.jewerly = false
end

function WorkshopData:OnRoleLevelChange()
    local role_level = RoleData.Instance:GetRoleLevel()
    self.forging_data.select_level = math.floor(role_level / 10) + 1 > WorkshopData.Params.MaxLevel and WorkshopData.Params.MaxLevel or math.floor(role_level / 10) + 1
    self.forging_data.select_level = self.forging_data.select_level <= WorkshopData.Params.MinLevel and WorkshopData.Params.MinLevel + 1 or self.forging_data.select_level
    self.jewelry_data.select_level = self:GetJewerlyShowLevel(role_level)
    self:BlazeSelectLevelByChange()
end

function WorkshopData:OnRoleProfessionChange()
    local profession = RoleData.Instance:GetProfession()
    self.forging_data.select_prof = profession == 0 and 1 or profession
    self.jewelry_data.select_prof = profession == 0 and WorkshopData.JewerlyProf[1] or WorkshopData.JewerlyProf[profession]
    self:BlazeSelectProfByChange()
end

function WorkshopData:OnRoleNoticeComplete()
    local role_level = RoleData.Instance:GetRoleLevel()
    local profession = RoleData.Instance:GetProfession()
    self.forging_data.select_prof = profession == 0 and 1 or profession
    self.forging_data.select_level = math.floor(role_level / 10) + 1 > WorkshopData.Params.MaxLevel and WorkshopData.Params.MaxLevel or math.floor(role_level / 10) + 1
    self.forging_data.select_level = self.forging_data.select_level <= WorkshopData.Params.MinLevel and WorkshopData.Params.MinLevel + 1 or self.forging_data.select_level
    self.jewelry_data.select_prof = profession == 0 and WorkshopData.JewerlyProf[1] or WorkshopData.JewerlyProf[profession]
    self.jewelry_data.select_level = self:GetJewerlyShowLevel(role_level)

    self:BlazeSelectLevelByChange()
    self:BlazeSelectProfByChange()
    self:BlazeSelectQuickByChange()
end

function WorkshopData:init_config()
    for _, value in ipairs(self.compose_list) do
        if value.group == ComposeGroup.Forging then
            self.forging_compose_list[value.seq] = value
        end
    end

    for _, value in ipairs(self.compose_list) do
        if value.group == ComposeGroup.Jewelry then
            self.jewelry_compose_list[value.seq] = value
        end
    end

    self.cache_compose_stuff_list = {}
    for k,v in pairs( self.forging_compose_list) do 
        for i = 1,5 do 
            if v["stuff_id_"..i] > 0 and self.cache_compose_stuff_list[v["stuff_id_"..i]] == nil then
                self.cache_compose_stuff_list[v["stuff_id_"..i]] = true
            end 
        end
    end 

    for k,v in pairs( self.jewelry_compose_list) do
        for i = 1,5 do 
            if v["stuff_id_"..i] > 0 and self.cache_compose_stuff_list[v["stuff_id_"..i]] == nil then 
                self.cache_compose_stuff_list[v["stuff_id_"..i]] = true
            end 
        end 
    end 

    for index, value in ipairs(self.exchange_item_group) do
        if self.skillbook_group_list[value.item_group] == nil then
            self.skillbook_group_list[value.item_group] = {}
        end
        table.insert(self.skillbook_group_list[value.item_group], value)
    end
    self.jg_consume_list = {}
    for index, value in ipairs(self.jg_consume) do
        self.jg_consume_list[value.equip_type] = self.jg_consume_list[value.equip_type] or {}
        self.jg_consume_list[value.equip_type][value.color] = self.jg_consume_list[value.equip_type][value.color] or {}
        self.jg_consume_list[value.equip_type][value.color][value.limit_level] = value
    end
    self.equip_resolve_list = {}
    for index, value in ipairs(self.resolve_cfg) do
        self.equip_resolve_list[value.equip_type] = self.equip_resolve_list[value.equip_type] or {}
        self.equip_resolve_list[value.equip_type][value.color] = self.equip_resolve_list[value.equip_type][value.color] or {}
        self.equip_resolve_list[value.equip_type][value.color][value.limit_level] = value
    end

    self.jewel_type_recommend = {}-- self.jewel_cfg_auto.jewel_recommend
    for k,v in pairs(self.jewel_cfg_auto.jewel_recommend) do
        if self.jewel_type_recommend[v.job] == nil then self.jewel_type_recommend[v.job] = {} end 
        self.jewel_type_recommend[v.job][v.equip_type] = v
    end 

    self.jewel_job_recommend = {}-- self.jewel_cfg_auto.jewel_recommend
    for k,v in pairs(self.jewel_cfg_auto.recommend_jewel) do
        self.jewel_job_recommend[v.job] = v
    end 

    self:InitJewelryProInfo()
    self:InitJweelryAccorCfg()
    self:InitNeoComposeAccorCfg()
    self:InitEntryCfg()
    self:InitNevealCfg()
    self:InitBlazeSatrCfg()

    self.upgrade_hole_cfgmap = {}   --key:hole value[key:prof_type value:[key:lv value:cfg]]
    for _,v in pairs(self.upgrade_config.equip_hole_upgrade) do 
        local hole_tab = self.upgrade_hole_cfgmap[v.hole_type]
        if hole_tab == nil then
            hole_tab = {}
            self.upgrade_hole_cfgmap[v.hole_type] = hole_tab
        end
        local pf = RoleData.ProfIdToProfType(v.limit_prof)
        local pf_tab = hole_tab[pf]
        if pf_tab == nil then
            pf_tab = {}
            hole_tab[pf] = pf_tab
        end
        pf_tab[v.hole_level] = v
    end
end
function WorkshopData:SkillBookData()
    return self.skill_book_data
end
function WorkshopData:GetTypeShow(group_id)
    if next(self.type_show_list) == nil then
        for k, v in pairs(self.compose_type_list) do
            if self.type_show_list[v.group_id] == nil then
                self.type_show_list[v.group_id] = {}
            end
            self.type_show_list[v.group_id][v.type_id] = v
        end
    end
    if group_id == ComposeGroup.Level then
        return Language.Workshop.LevelShow2
    end
    return self.type_show_list[group_id]
end

function WorkshopData:GetTypeShowCombine(param_t)
    -- LogError("GetTypeShowCombine",param_t)
    if next(self.type_show_list) == nil then
        for k, v in pairs(self.compose_type_list) do
            if self.type_show_list[v.group_id] == nil then
                self.type_show_list[v.group_id] = {}
            end
            self.type_show_list[v.group_id][v.type_id] = v
        end
    end
    -- if group_id == ComposeGroup.Level then
    --     return Language.Workshop.LevelShow2
    -- end

    -- LogError("self.compose_type_list",self.compose_type_list)
    local combine_list = {}
    for k,v in pairs(self.compose_type_list) do 
        for i,j in pairs(param_t) do 
            if v.group_id == j then 
                table.insert( combine_list, v )
            end 
        end 
    end 

    table.sort(combine_list,function(a,b)return a.group_id < b.group_id end)
    
    return combine_list
end
-- 筛选锻造显示的物品
function WorkshopData:ForgingShowList()
    -- 有默认职业和默认等级
    local type_id = self.forging_data.select_prof
    local level = self.forging_data.select_level
    local data = {}
    for k, v in pairs(self.compose_list) do
        if v.type == type_id then
            if Equip.GetEquipLevel(v.target) == level and not Equip.IsDivine(v.target) and v.is_not_show == 0 then
                table.insert(data, v)
            end
        end
    end
    -- 排序 按品质排策划说的
    table.sort(data, function(a, b)
        local itemA = Item.Create({item_id = a.target})
        local itemB = Item.Create({item_id = b.target})
        if (itemA:Color() * 10 + (10 - itemA:EquipTypeSort())) < (itemB:Color() * 10  + (10 - itemB:EquipTypeSort())) then
            return true
        else
            return false
        end
    end)
    return data
end
function WorkshopData:GetSelectById(item_id)
    local list = self:ForgingShowList()
    for key, value in pairs(list) do
        if value.target == item_id then
            return value
        end
    end
    return nil
end

function WorkshopData:CheckUpgradeRemind()
    local num = 0
    local list = BagData.Instance:GetEquipSortUpgrade()
    local ignore_list = Split(self.upgrade_remind_sort.str,"|")
    for k,v in pairs(list) do
        local cur_cfg,next_cfg = self:GetEquipUpgradeCfgByType(k)
        local equip = k <= 4 and BagData.Instance:GetEquipList()[k] or BagData.Instance:GetJewelryList()[k-5]
        if next_cfg ~= nil and equip ~= nil then 
            local flag = true
            if equip:IsStarGod() then 
                flag = equip:StrengthenLevel() >= next_cfg.need_godequip_level
            else 
                flag = equip:LimitLv() >= next_cfg.need_equip_level
            end 
            for i,j in pairs(next_cfg.strengthen_consume_item) do
                if Item.GetNum(j.item_id) < j.num then 
                    flag = false
                end 
            end 
            for i,j in pairs(ignore_list) do 
                if tonumber(j) == k then 
                    flag = false
                end 
            end 
            if flag then 
                num = num + 1
            end 
        end 
    end  
    return num
end

function WorkshopData:ClearUpgradeRemindIgnore()
    self.upgrade_remind_sort.str = "" 
end

function WorkshopData:AddUpgradeRemindIgnore(equip_type)
    local ignore_list = Split(self.upgrade_remind_sort.str,"|")
    for k,v in pairs(ignore_list) do 
        if tonumber(v) == equip_type then 
            return
         end 
    end 
    table.insert(ignore_list,equip_type)
    local str = ""
    for k,v in pairs(ignore_list) do 
        local plus = (k == 1) and v or "|"..v
        str = str .. plus
    end 
    self.upgrade_remind_sort.str = str
end

function WorkshopData:CheckIsIgnoreRemindInUpgrade(type)
    local ignore_list = Split(self.upgrade_remind_sort.str,"|")
    for k,v in pairs(ignore_list) do 
        if tonumber(v) == type then 
            return true
         end 
    end 
    return false
end

function WorkshopData:GetEquipUpgradeCfgByType(type)
    local cur_level = BagData.Instance:GetEquipSortUpgrade(type)
    local role_prof_type = RoleData.Instance:ProfType()
    local cur_cfg = self.upgrade_hole_cfgmap[type][role_prof_type][cur_level]
    local next_cfg = self.upgrade_hole_cfgmap[type][role_prof_type][cur_level + 1]    
    -- for k,v in pairs(self.upgrade_hole_config) do 
    --     if  role_prof_type == RoleData.ProfIdToProfType(v.limit_prof) 
    --         and cur_level == v.hole_level and type == v.hole_type then 
    --         cur_cfg = v
    --     end 
    --     if role_prof_type == RoleData.ProfIdToProfType(v.limit_prof)  
    --         and cur_level + 1 == v.hole_level and type == v.hole_type then 
    --         next_cfg = v
    --     end 
    -- end 
    return cur_cfg,next_cfg
end

-- 获得突破配置
function WorkshopData:GetEquipUpgradeBreakCfgByType(type)
    local cur_break = BagData.Instance:GetEquipSortStrengthen(type)
    local cur_cfg = nil
    local next_cfg = nil
    for k,v in pairs(self.upgrade_break_config) do 
        if cur_break > 0 then 
            if  RoleData.Instance:ProfType() == RoleData.ProfIdToProfType(v.limit_prof) 
                and cur_break == v.grade and type == v.hole_type then 
                cur_cfg = v
            end 
        end
        if RoleData.Instance:ProfType() == RoleData.ProfIdToProfType(v.limit_prof)  
            and cur_break + 1 == v.grade and type == v.hole_type then 
            next_cfg = v
        end 
    end 
    return cur_cfg,next_cfg
end

function WorkshopData:GetEquipUpgradeCombo()
    local result = {}
    local role_prof = RoleData.Instance:ProfType()
    for k,v in pairs(self.upgrade_combo_config) do
        if role_prof == RoleData.ProfIdToProfType(v.limit_prof) then 
            table.insert(result, v)
        end 
    end 

    local out_list = {}
    for k,v in pairs(result) do 
        local vo = {}
        vo.str = string.format(Language.Workshop.StrenSuitStart,v.hole_level_all)
        vo.hole_level_all = v.hole_level_all
        local with_out_attr = true
        for i = 1, 3 do 
            if v["attr_add_c"..i] ~= 0 then 
                vo.str = vo.str..Language.Common.AttrList[v["attr_type_c"..i]].."  +".. (DataHelper.IsPercent(v["attr_type_c"..i]) and Percent(v["attr_add_c"..i]) or v["attr_add_c"..i]).."    "
                with_out_attr = false
            end 
        end 
        if v.sp_id > 0 then 
            local cfg = Cfg.SpecialEffect(v.sp_id)
            vo.str = vo.str..(with_out_attr and "" or "    ")..Language.ItemInfo.ItemTitles.SuitSp..cfg.description
        end 
        table.insert(out_list,vo)
    end 
    return out_list
end
-- 获取当前最大共同强化等级
function WorkshopData:GetCurMaxEquipUpGradeLevel()
    local min = 2000
    for k,v in pairs(BagData.Instance:GetEquipSortUpgrade()) do 
        if v < min then 
            min = v 
        end 
    end 

    return min
end
function WorkshopData:CheckEquipUpGradeMark(level)
    -- LogError("!CheckEquipUpGradeMark?",level,self:GetCurMaxEquipUpGradeLevel())
    return level <= self:GetCurMaxEquipUpGradeLevel()
end
-- 获取输入level是否当前最大level
function WorkshopData:CheckEquipUpGradeMarkEqual(level)
    local min = self:GetCurMaxEquipUpGradeLevel()
    if level ~= nil and  level > min then return false end 

    local suit_list = self:GetEquipUpgradeCombo()
    local max = nil
    for k,v in pairs(suit_list) do 
        if max == nil or (v.hole_level_all >= max.hole_level_all and v.hole_level_all <= min) then 
            max = v
        end 
    end 

    if level ~= nil then 
        return level == max.hole_level_all
    else 
        return max
    end 
    
end
--
function WorkshopData:CheckAllEquipUpGradeMark()
    local suit_list = self:GetEquipUpgradeCombo()
    local num = 0 
    for k,v in pairs(suit_list) do 
        if self:CheckEquipUpGradeMark(v.hole_level_all) then 
            num = num + 1
        end 
    end 

    return num
end

function WorkshopData:GetForgeComposeCfg(item_id)
    return self.compose_target_config[item_id]
    --[[ for k,v in pairs(self.compose_list) do 
        if v.target == item_id then 
            return v
        end 
    end  ]]
end
function WorkshopData:CrystalShowList()
    local type_id = self.crystal_data.select_crystal
    local data = {}
    for k, v in pairs(self.compose_list) do
        if v.type == type_id then
            table.insert(data, v)
        end
    end
    return data
end
function WorkshopData:CrystalComposeShowList()
    local type_id = self.crystal_compose_data.select_type
    local data = {}
    local flag_combine = nil
    for k,v in pairs(self.compose_type_list) do 
        if v.type_id == type_id and (v.group_id == ComposeGroup.Card or v.group_id == ComposeGroup.Skill) then
            flag_combine = v.group_id
        end
    end 
    local list = flag_combine ~= nil and self.exchange_list or self.compose_list
    for k, v in pairs(list) do
        if flag_combine ~= nil then 
            if v.group == flag_combine then
                table.insert( data, v )
            end  
        else 
            if v.type == type_id then 
                table.insert( data, v )
            end 
        end 
    end 
    return data
end
function WorkshopData:SetQuickJewelIndex(item)
    local list = self:GetAllEquip()
    local jump_pos = 0
    for k,v in pairs(list) do 
        if v.index == item.index 
        and v.in_bag == item.in_bag
        and v.item_id == item.item_id then 
            jump_pos = k
        end 
    end 
    self.quick_open_jewel_index = jump_pos -- + list:Length()
end
function WorkshopData:GetQuickJewelIndex()
    return self.quick_open_jewel_index 
end
function WorkshopData:ClearQuickJewelIndex()
    self.quick_open_jewel_index = -1
end
function WorkshopData:SetInsetMark(item)
    self.quick_open_inset = item
end
function WorkshopData:GetInsetMark()
    return self.quick_open_inset 
end
function WorkshopData:ForgingData()
    return self.forging_data
end
function WorkshopData:CrystalData()
    return self.crystal_data
end
function WorkshopData:NeoComposeData()
    return self.crystal_compose_data
end
function WorkshopData:SetLifeItemQuickJump(item_id)
    self.quick_jump_life_item = item_id
end
function WorkshopData:GetLifeItemQuickJump()
    if self.quick_jump_life_item == nil then return 1 end 
    local pos_ = 1
    for k,v in pairs(self:LifeShowList()) do 
        if v.target == self.quick_jump_life_item then 
            pos_ = k
            break
        end 
    end 
    self.quick_jump_life_item = nil
    return pos_
end
function WorkshopData:LifeShowList()
    local data = {}
    for k, v in pairs(self.compose_list) do
        if v.group == ComposeGroup.Life then
            table.insert(data, v)
        end
    end
    return data
end
function WorkshopData:SetActiveInfo(list)
    self.active_info:Set(list)
    -- Debuger.LogError(Serialize(self.active_info))
end
function WorkshopData:ChangeActiveInfo(id, flag,notice)
    self.active_info[id] = flag
	self:IsShowActiveInfo(id,flag,notice)
end

function WorkshopData:IsShowActiveInfo(id,flag,notice)
	if flag then
		local seq_config = self:GetActivationSeqConfig(id)
		if seq_config.group == 11 and seq_config.activation > 0 and notice == 1 then
				table.insert(WorkshopCtrl.Instance.popup_list, seq_config.activation)
                if ViewMgr:IsClosed(RolePeiFangPopupView) then
                    ViewMgr:OpenView(RolePeiFangPopupView,{show_id = seq_config.activation})
                end
		end
	end
end

function WorkshopData:GetAcitveInfo(id)
    return (self.active_info[id] or 0) > 0
end

function WorkshopData:SetCreateInfo(list)
    self.create_info:Set(list)
    --Debuger.LogError(Serialize(self.create_info))
end

function WorkshopData:GetCreateInfo(id)
    return (self.create_info[id] or 0) > 0
end

function WorkshopData:GetWorkShopActivation(item)
    return item:ShowType() == ShowType.Formula and self:GetAcitveInfo(item:Config().param1)
end

function WorkshopData:SetForgeMark(data)
    self.forge_mark_data = data
end

function WorkshopData:GetForgeMark()
    return self.forge_mark_data
end

function WorkshopData:SetForgeRemindBlock(flag)
    self.remind_block.forge = flag
    self.remind_block.flush = self.remind_block.flush + 1
end

function WorkshopData:GetForgeRemindBlock()
    return self.remind_block.forge
end

function WorkshopData:SetJewerlyRemindBlock(flag)
    self.remind_block.jewerly = flag
    self.remind_block.flush = self.remind_block.flush + 1
end

function WorkshopData:GetJewerlyRemindBlock()
    return self.remind_block.jewerly
end

--溯源检查 放入list和mark 可以检查mark是否在list中
function WorkshopData:CheckIsMarkItOrigin(mark,list)
    if mark == nil or list == nil then return false,-1 end 
    if mark.in_bag == nil then return false,-1 end 
    for k,v in pairs(list) do 
        if mark.in_bag == v.in_bag then 
            if mark.in_bag then 
                if v.index == mark.index and v.column == mark.column then 
                    return true , k
                end 
            else 
                if v.index == mark.index then 
                    return true , k
                end 
            end 
        end 
    end  

    return false ,-1
end

--传入顺序制作拥有和需要 计算可制作最大
function WorkshopData:CalcMaxNum(have_list, need_list)
    local max_num = 9999
    for k, v in pairs(need_list) do
        local have = have_list[k]
        local need = v
        local res = math.floor(have / need)
        max_num = max_num < res and max_num or res
    end
    return max_num
end

function WorkshopData:GetAllEquip()
    local list1 = BagData.Instance:GetEquipList()
    local list2 = BagData.Instance:GetGridByType({ItemType.Weapon, ItemType.Armor})
    local new_list = {}
    table.sort(list1, function(a, b)
        if a:EquipTypeSort() < b:EquipTypeSort()then
            return true
        else
            return false
        end
    end)
    for k, v in pairs(list1) do
        table.insert(new_list, v)
    end
    table.sort(list2, function(a, b)
        if a:EquipTypeSort() < b:EquipTypeSort()then
            return true
        else
            return false
        end
    end)
    for k, v in pairs(list2) do
        table.insert(new_list, v)
    end
    return new_list
end
function WorkshopData:GetStoneGroup(jewel_type)
    if next(self.jewel_group_list) == nil then
        for k, v in pairs(self.jewel_list) do
            if self.jewel_group_list[v.jewel_type] == nil then
                self.jewel_group_list[v.jewel_type] = {}
            end
            table.insert(self.jewel_group_list[v.jewel_type], v)
        end
    end
    return self.jewel_group_list[jewel_type]
end

function WorkshopData:GetStoneInfo(jewel_id)
    if next(self.jewel_info_list) == nil then
        for k, v in pairs(self.jewel_list) do
            self.jewel_info_list[v.jewel_id] = v
        end
    end
    return self.jewel_info_list[jewel_id]
end

function WorkshopData:GetStoneAccorCfg(equip_type)
    local stoneinbag = BagData.Instance:GetGridByType({ItemType.Gem})
    local stonebytype = self:StoneByType()
    local create_parent = function(info)
        return {id = info.jewel_type, name = info.type_name, children = nil}
    end
    if self.equip_jewel_list[equip_type] == nil then
        self.equip_jewel_list[equip_type] = {}
        local jewel_set = self:GetEquipSet(equip_type)
        for k, v in pairs(self.jewel_group) do
            for k2, v2 in pairs(jewel_set.jewel_list) do
                if tonumber(v2) == v.jewel_type then
                    self.equip_jewel_list[equip_type][v.jewel_type] = create_parent(v)
                end
            end
        end
    end
    for k, v in pairs(self.equip_jewel_list[equip_type]) do
        v.children = stonebytype[v.id] or {}
    end
    return self.equip_jewel_list[equip_type]
end

function WorkshopData:InitJweelryAccorCfg() 
    self.jweelry_accor_cfg = {}
    local create_parent = function(info)
        return {id = info.level, name = info.level..Language.Role.ji..Language.Workshop.Tab5, children = {}}
    end
    for key, value in ipairs(self.ornament_forge) do
        if self.jweelry_accor_cfg[value.level] == nil then
            self.jweelry_accor_cfg[value.level] = create_parent(value)
        end
        table.insert(self.jweelry_accor_cfg[value.level].children, value)
    end
end 

function WorkshopData:InitNeoComposeAccorCfg()
    self.neo_compose_accor_cfg = {}
    local param_t = {
        -- [1] = ComposeGroup.Life,
        [2] = ComposeGroup.EleCrystal,
        -- [2] = ComposeGroup.Card,
        -- [3] = ComposeGroup.Key,
        [4] = ComposeGroup.PetFlush,
        -- [3] = ComposeGroup.Skill,
        [5] = ComposeGroup.MarbasRefine,
        [6] = ComposeGroup.Map,
        [7] = ComposeGroup.DragonBall,
    }

    local compose_list = {}
    local data = self:GetTypeShowCombine(param_t)
    local flag_in_skill = false
    local flag_in_card = false
    local flag_in_key = false
    local flag_in_petflush = false
    local flag_in_life = false
    local flag_in_marbasrefine = false
    local flag_in_map = false
    local flag_in_dragonball = false
    for k,v in pairs(data) do 
        if v.group_id == 7 and not flag_in_skill then
            local temp = {
                type_id = v.type_id,
                name = Config.compose_auto.group_list[v.group_id].group_name,
                group_id = v.group_id,
                children = {}
            }
            table.insert( self.neo_compose_accor_cfg, temp )
            flag_in_skill = true
        elseif v.group_id ==6 and not flag_in_card then
            local temp = {
                type_id = v.type_id,
                name = Config.compose_auto.group_list[v.group_id].group_name,
                group_id = v.group_id,
                children = {}
            }
            table.insert( self.neo_compose_accor_cfg, temp )
            flag_in_card = true
        elseif v.group_id ==3 then
            local temp = {}
            temp.group_id =  v.group_id
            temp.type_id = v.type_id
            temp.name = v.type_name
            temp.children = {}
            table.insert( self.neo_compose_accor_cfg, temp )
        elseif v.group_id ==9 and not flag_in_key then
            local temp = {
                type_id = v.type_id,
                name = Config.compose_auto.group_list[v.group_id].group_name,
                group_id = v.group_id,
                children = {}
            }
            table.insert( self.neo_compose_accor_cfg, temp )
            flag_in_key = true 
        elseif v.group_id ==10 and not flag_in_petflush then
            local temp = {
                type_id = v.type_id,
                name = Config.compose_auto.group_list[v.group_id].group_name,
                group_id = v.group_id,
                children = {}
            }
            table.insert( self.neo_compose_accor_cfg, temp )
            flag_in_petflush = true 
        elseif v.group_id == 2 and not flag_in_life then 
            local temp = {
                type_id = v.type_id,
                name = Config.compose_auto.group_list[v.group_id].group_name,
                group_id = v.group_id,
                children = {}
            }
            table.insert( self.neo_compose_accor_cfg, temp )
            flag_in_life = true 
        elseif v.group_id == 14 and not flag_in_marbasrefine then 
            local temp = {
                type_id = v.type_id,
                name = Config.compose_auto.group_list[v.group_id].group_name,
                group_id = v.group_id,
                children = {}
            }
            table.insert( self.neo_compose_accor_cfg, temp )
            flag_in_marbasrefine = true 
        elseif v.group_id == 5 and not flag_in_map then 
            local temp = {
                type_id = v.type_id,
                name = Config.compose_auto.group_list[v.group_id].group_name,
                group_id = v.group_id,
                children = {}
            }
            table.insert( self.neo_compose_accor_cfg, temp )
            flag_in_map = true 
        elseif v.group_id == 16 and not flag_in_dragonball then 
            local temp = {
                type_id = v.type_id,
                name = Config.compose_auto.group_list[v.group_id].group_name,
                group_id = v.group_id,
                children = {}
            }
            table.insert( self.neo_compose_accor_cfg, temp )
            flag_in_dragonball = true 
        end 
    end 

    -- local type_id = self.crystal_compose_data.select_type
    -- local data = {}
    -- local flag_combine = nil
    -- for k,v in pairs(self.compose_type_list) do 
    --     if v.type_id == type_id and (v.group_id == ComposeGroup.Card or v.group_id == ComposeGroup.Skill) then
    --         flag_combine = v.group_id
    --     end
    -- end 
    -- local list = flag_combine ~= nil and self.exchange_list or self.compose_list
    -- for k, v in pairs(list) do
    --     if flag_combine ~= nil then 
    --         if v.group == flag_combine then
    --             table.insert( data, v )
    --         end  
    --     else 
    --         if v.type == type_id then 
    --             table.insert( data, v )
    --         end 
    --     end 
    -- end 

    for k,v in pairs(self.neo_compose_accor_cfg) do 
        -- local flag_ = v.group_id == ComposeGroup.Card or v.group_id == ComposeGroup.Skill 
        --     or v.group_id == ComposeGroup.PetFlush or v.group_id == ComposeGroup.Key 
        -- local list = flag_ and self.exchange_list or self.compose_list
        -- for i,j in pairs(list) do
        --     if flag_ and j.group == v.group_id then 
        --         table.insert(v.children,j)
        --     elseif not flag_ and j.type == v.type_id then 
        --         table.insert(v.children,j)
        --     end 
        -- end 
        for i,j in pairs(self.exchange_list) do 
            if j.group == v.group_id then 
                table.insert(v.children,j)
            end 
        end 
        for i,j in pairs(self.compose_list) do 
            if j.group == v.group_id and j.is_not_show == 0 then 
                table.insert(v.children,j)
            end 
        end 
    end 
    -- for key, value in ipairs(self.ornament_forge) do
    --     if self.jweelry_accor_cfg[value.level] == nil then
    --         self.jweelry_accor_cfg[value.level] = create_parent(value)
    --     end
    --     table.insert(self.jweelry_accor_cfg[value.level].children, value)
    -- end
    -- LogError("neo_compose_accor_cfg",self.neo_compose_accor_cfg)
end 

-- 灵饰分组
function WorkshopData:GetJweelryAccorCfg(is_sort)
    if not is_sort then return self.jweelry_accor_cfg end 

    local list = {}
    for k,v in pairs(self.jweelry_accor_cfg) do 
        table.insert( list, v )
    end 
    table.sort(list, DataSorter.KeyLowerSorter("id"))

    -- Debuger.LogError(Serialize(list))
    return list
end

function WorkshopData:GetNeoComposeAccorCfg(is_sort)
    local fix_list = {}

    for k,v in pairs(self.neo_compose_accor_cfg) do 
        if v.group_id == ComposeGroup.DragonBall then 

            if FairyDragonData.Instance:IsOpen() then 
                table.insert(fix_list,v)    
            end 
        else
            table.insert(fix_list,v)
        end 
    end     

    return fix_list
end 

--合成
function WorkshopData:GetComposeData()
    return self.compose_data
end
-- 获取显示列表
function WorkshopData:ComposeShowList()
    -- local group_id = ComposeGroup.Stone
    local type_id = self.compose_data.select_type
    local data = {}
    --[[ for k, v in pairs(self.compose_list) do
        if v.group == group_id and v.type == type_id then
            table.insert(data, v)
        end`
    end ]]
    for index, value in ipairs(self.exchange_list) do
        if value.group == type_id then
            table.insert(data, value)
        end
    end
    return data
end

function WorkshopData:FabaoSPInfo()
    local type_id = ComposeGroup.FabaoSP
    local data = {}
    for _, value in ipairs(self.exchange_list) do
        if value.group == type_id then
            table.insert(data, value)
        end
    end
    return data
end
function WorkshopData:StoneShowList(equip_type)
    --可镶嵌宝石类型列表
    local jewel_set = self:GetEquipSet(equip_type)
    --背包内宝石
    if jewel_set == nil then return end 
    local stonebytype = self:StoneByType()
    local temp = {}
    for _, value in ipairs(jewel_set.jewel_list) do
        if stonebytype[tonumber(value)] then
            for _, v in ipairs(stonebytype[tonumber(value)]) do
                temp[v.item_id] = v
            end
        end
        local item_id = self.jewel_group[tonumber(value) + 1].fast_buy_item
        if temp[item_id] == nil then
            temp[item_id] = Item.Create({item_id = item_id})
        end
    end
    --如果没有一级宝石要添加一级宝石 提供快速购买
    return temp
end
function WorkshopData:GetStoneInBag(equip_type)
    if equip_type == -1 then return 0 end 
    --可镶嵌宝石类型列表
    local jewel_set = self:GetEquipSet(equip_type)
    --背包内宝石
    local stonebytype = self:StoneByType()
    local temp = {}
    for _, value in ipairs(jewel_set.jewel_list) do
        if stonebytype[tonumber(value)] then
            for _, v in ipairs(stonebytype[tonumber(value)]) do
                temp[v.item_id] = v
            end
        end 
    end 

    local equip_data = BagData.Instance:GetEquipByType(equip_type)
    local result = {}
    for k,v in pairs(temp) do 
        local flag,limit_desc = self:GetStoneLimit(equip_data,v)
        if flag then 
            table.insert( result, v )
        end 
    end 
    return #result >0
end
function WorkshopData:StoneTeamInfo()
end
-- 背包中的宝石列表
function WorkshopData:StoneByType()
    local stoneinbag = BagData.Instance:GetGridByType({ItemType.Gem})
    local stonebytype = {}
    for _, v in pairs(stoneinbag) do
        local info = self:GetStoneInfo(v.item_id)
        if info ~= nil then --TempCode
            local jewel_type = info.jewel_type
            stonebytype[jewel_type] = stonebytype[jewel_type] or {}
            table.insert(stonebytype[jewel_type], v)
        end
    end
    --这里找到有没有宝石而已
    return stonebytype
end
-- 获取对应装备的可以镶嵌类型的表
function WorkshopData:GetEquipSet(equip_type)
    if next(self.jewel_set_list) == nil then
        for k, v in pairs(self.jewel_set) do
            v.jewel_list = string.split(v.jewel_type, "|")
            self.jewel_set_list[v.equip_type] = v
        end
    end
    return self.jewel_set_list[equip_type]
end

function WorkshopData:SetCurSelectEquipInset(data)
    self.cur_select_equip_insetr = data
end

function WorkshopData:GetCurSelectEquipInset()
    return self.cur_select_equip_insetr
end

function WorkshopData:JewelryShowList()
    return self.ornament_forge
end

function WorkshopData:GetJewelryShowCfg(ornament_type,level) 
    for k,v in pairs(self.ornament_forge) do 
        if v.level == level and v.ornament_type == ornament_type then 
            return v
        end 
    end 
    return nil
end 

function WorkshopData:GetJewelrySuitInfo(suit_id)
    return self.ornament_suit[suit_id]
end

function WorkshopData:SetSelectJewelry(data, index)
    if index then
        self.select_jewelry[index] = jewelry
    else
        self.select_jewelry:Set(data)
    end
end
function WorkshopData:GetSelectJewelry()
    return self.select_jewelry
end
function WorkshopData:GetStoneLimitLv()
    return self.jewel_limit_level
end

-- 封装镶嵌孔位解锁
function WorkshopData:GetStoneSortLimit(equip_data,index)
    if equip_data == nil then return false,nil end 
    local limit = equip_data:IsStarGod() and self.jewel_limit_level[index].godlimit_level or self.jewel_limit_level[index].limit_level
    local level = equip_data:IsStarGod() and equip_data:StrengthenLevel() or equip_data:Level()
    if limit == nil or level == nil then
        return false
    end
    return level >= limit ,limit
end

-- 封装宝石的可穿着解锁
function WorkshopData:GetStoneLimit(equip_data,stone_data)
    if equip_data == nil or stone_data == nil then return false end 

    local info = self:GetStoneInfo(stone_data.item_id)

    local limit = equip_data:IsStarGod() and info.godequip_level or info.level_limit
    local level = equip_data:IsStarGod() and equip_data:StrengthenLevel() or equip_data:Level()

    local desc = string.format(equip_data:IsStarGod() and Language.Workshop.JewelGodLimitLV or Language.Workshop.JewelLimitLV,limit)

    return level >= limit,desc
end

-- 封装宝石的可升级
function WorkshopData:GetStoneLimitLevelUp(equip_data,stone_data)
    if equip_data == nil or stone_data == nil then return false end 

    local info = self:GetStoneInfo(stone_data.item_id)
    local group =self:GetStoneGroup(info.jewel_type)

    local next_info = nil 
    for k,v in pairs(group) do 
        if v.level == info.level + 1 then 
            next_info= v
        end 
    end 

    if next_info == nil then return true end 

    local limit = equip_data:IsStarGod() and next_info.godequip_level or next_info.level_limit
    local level = equip_data:IsStarGod() and equip_data:StrengthenLevel() or equip_data:Level()

    local desc = string.format(equip_data:IsStarGod() and Language.Workshop.JewelGodLimitLV or Language.Workshop.JewelLimitLV,limit)

    return level >= limit,desc
end

function WorkshopData:InitEntryCfg()
    self.entry_rand_group = {}
    local cfg_origin = Config.entry_cfg_auto.rand_group
    for k,v in pairs(cfg_origin) do 
        if self.entry_rand_group[v.att_group] == nil then self.entry_rand_group[v.att_group] ={} end 
        if self.entry_rand_group[v.att_group][v.att_type] == nil then self.entry_rand_group[v.att_group][v.att_type] = {} end 

        table.insert( self.entry_rand_group[v.att_group][v.att_type] , v )
    end 
end

function WorkshopData:ActiveSuitInfo()
    local list = BagData.Instance:GetJewelryList()
    local is_active = true
    local result = nil
    if table.nums(list) == 3 then
        local info1 = Jewelry.GetSuitInfo(list[0].param.lyrics_id)
        local quality = 999
        for k, v in pairs(list) do
            if v.param.lyrics_id == 0 then
                is_active = false
                break
            end
            local info = Jewelry.GetSuitInfo(v.param.lyrics_id)
            if info1.suit_type == info.suit_type then
                result = info.quality < quality and info or info1
                quality = info.quality < quality and info.quality or quality
            else
                is_active = false
            end
        end
        local same_quality = true
        local level = 999
        if is_active then
            for k, v in pairs(list) do
                local info = Jewelry.GetSuitInfo(v.param.lyrics_id)
                if info1.quality == info.quality then
                    else
                    same_quality = false
                    result = info.level < level and info or info1
                    level = info.level < level and info.level or level
                end
            end
        end
    else
        is_active = false
    end
    return is_active, result
end

function WorkshopData:SuperMapInfo()
    local group_id = ComposeGroup.Map
    local data = {}
    for _, v in pairs(self.compose_list) do
        if v.group == group_id then
            table.insert(data, v)
        end
    end
    return data
end

function WorkshopData:SetMateData(data)
    self.mate_data = data
end
function WorkshopData:MateData()
    return self.mate_data
end

function WorkshopData:SetSkillGroup(group, max_num)
    self.skill_group = group
    self.skill_select_max = max_num
end
function WorkshopData:SkillShowList()
    local data = {}
    for _, v in ipairs(self.exchange_item_group) do
        if v.item_group == self.skill_group then
            table.insert( data, v )
        end
    end
    table.sort(data,function (a,b)
        local tempA = Item.Create({item_id = a.item_id}):Num()
        local tempB = Item.Create({item_id = b.item_id}):Num()
        return tempA > tempB
    end)
    return data
end
function WorkshopData:SkillSelectMax()
    return self.skill_select_max
end
function WorkshopData:SkillSelectList()
    return self.skill_select_list
end
function WorkshopData:ClearSkillSelect()
    self.skill_select_list = {}
    self.skill_book_data.total_num = 0
end

function WorkshopData:SetComposeResult(protocol)
    self.compose_result = protocol
end
function WorkshopData:ComposeResult()
    return self.compose_result
end
function WorkshopData:SetJewelryQuaInfo(protocol)
    self.workshop_jewelryinfo_result = protocol.jewelry_times
end
function WorkshopData:GetJewelryQuaInfo()
    return self.workshop_jewelryinfo_result or {}
end

function WorkshopData:GetJewelryQuaInfoTime(quailty_id)
    local operate_list = self:GetJewelryQuaInfo()
    if #operate_list == 0 then return 0 end 

    return operate_list[quailty_id] or 0
end

-- 道具检查
-- 合并，如果该道具为合成道具，解除block 否则跳过
function WorkshopData:CheckIsComppseStuff(item_id)
    -- if self.compose_stuff_cache.forge[item_id] ~= nil then 
    --     self:SetForgeRemindBlock(false)
    --     return 
    -- end 
    -- if self.compose_stuff_cache.jewelry[item_id] ~= nil then 
    --     self:SetJewerlyRemindBlock(false)
    --     return 
    -- end 
    if self.cache_compose_stuff_list[item_id] == nil then
        return 
    end 

    local role_level = RoleData.Instance:GetRoleLevel()
    local role_prof = RoleData.Instance:ProfType()
    for k,v in pairs(self.forging_compose_list) do 
        if Item.GetLimitLv(v.target) >= math.floor(role_level/10) * 10 and role_prof == RoleData.ProfIdToProfType(Equip.GetLimitProf(v.target)) then 
            local flag_success = false
            for i = 1,5 do 
                if v["stuff_id_"..i] == item_id then 
                    flag_success = true
                    -- self:SetForgeRemindBlock(false)
                    -- self.compose_stuf_fcache.forge[item_id] = true
                    -- self.cache_item = item_id
                    break
                end  
            end 

            -- 要配方里有 且 这个配方能造出来才能红
            if flag_success then 
                local flag_red = self:CheckCanCompos(v) > 0 
                if flag_red then 
                    self:SetForgeRemindBlock(false)
                    -- self.compose_stuf_fcache.forge[item_id] = true
                    self.cache_item = item_id
                    break
                end  
            end 
        end 
    end 

    for k,v in pairs(self.jewelry_compose_list) do 
        local role_level = RoleData.Instance:GetRoleLevel()
        local role_prof = RoleData.Instance:ProfType()
        if Item.GetLimitLv(v.target) >= math.floor(role_level/10) * 10 and role_prof == RoleData.ProfIdToProfType(Equip.GetLimitProf(v.target)) then 
            local flag_success = false
            for i = 1,5 do 
                if v["stuff_id_"..i] == item_id then 
                    flag_success = true
                    -- self:SetJewerlyRemindBlock(false)
                    -- self.compose_stuff_cache.jewelry[item_id] = true
                    -- self.cache_item = item_id 
                    break
                end  
            end 

            -- 要配方里有 且 这个配方能造出来才能红
            if flag_success then 
                local flag_red = self:CheckCanJewelry(v) > 0 
                if flag_red then 
                    self:SetJewerlyRemindBlock(false)
                    -- self.compose_stuf_fcache.jewelry[item_id] = true
                    self.cache_item = item_id
                    break
                end  
            end 
        end 
    end 
end

---------------红点----------
function WorkshopData:CheckWorkForging()
    if self:GetForgeRemindBlock() then 
        -- 阻挡行为发生后删除红点
        return 0
    end 
    -- 所有合成物在解锁的情况下是否可以合成
    
    local num = 0
    for k, v in pairs(self.forging_compose_list) do
        num = num + self:CheckCanCompos(v)
    end
    return num > 0 and 1 or 0
end

-- 是否可合成
function WorkshopData:CheckCanCompos(info)
    --是否解锁
    -- if info.active_seq > 0 then
    --     local flag = self:GetAcitveInfo(info.active_seq)
    --     if flag == false then
    --         return 0
    --     end
    -- end
    local role_level = RoleData.Instance:GetRoleLevel()
    if Item.GetLimitLv(info.target) < math.floor(role_level/10) * 10 then 
        return 0
    end 

    local role_prof = RoleData.Instance:ProfType()
    if role_prof ~= RoleData.ProfIdToProfType(Equip.GetLimitProf(info.target)) then 
        return 0
    end 

    local num = 1
    for i = 1, 5 do
        if info["stuff_id_"..i] > 0 then
            if Item.GetNum(info["stuff_id_"..i]) < info["stuff_num_"..i] then
                return 0
            end
        end
    end
    return num
end

function WorkshopData:CheckWorkCompose()
    for k,v in pairs(self.neo_compose_accor_cfg) do 
        if v.group_id == ComposeGroup.Map then 
            for i,j in pairs(v.children) do 
                if self:CheckCanNeoCompose(j) then 
                    return 1
                end 
            end 
        end 
    end 
    return 0
end
function WorkshopData:CheckSkillNum(info)
    local num = 0
    for i,v in ipairs(self.skillbook_group_list[info.param_1]) do
        num = num + Item.Create({item_id = v.item_id}):Num()
        if num >= info.num then
            return num
        end
    end
    num = 0
    return num
end

function WorkshopData:CheckWorkJewelry()
    if self:GetJewerlyRemindBlock() then 
        -- 阻挡行为发生后删除红点
        return 0
    end 

    -- 钱和材料
    local num = 0
    for k,v in pairs(self.jewelry_compose_list) do
        num = num + self:CheckCanJewelry(v)
    end 
    return num > 0 and 1 or 0
end

function WorkshopData:CheckCanJewelry(info)
    --是否解锁
    -- if info.active_seq > 0 then
    --     local flag = self:GetAcitveInfo(info.active_seq)
    --     if flag == false then
    --         return 0
    --     end
    -- end
    local role_level = RoleData.Instance:GetRoleLevel()
    if Item.GetLimitLv(info.target) < math.floor(role_level/10) * 10 then 
        return 0
    end 

    local role_prof = RoleData.Instance:ProfType()
    if role_prof ~= RoleData.ProfIdToProfType(Equip.GetLimitProf(info.target)) then 
        return 0
    end 

    local num = 1
    for i = 1, 5 do
        if info["stuff_id_"..i] > 0 then
            if Item.GetNum(info["stuff_id_"..i]) < info["stuff_num_"..i] then
                return 0
            end
        end
    end
    return num
end

function WorkshopData:CheckCanParentJewelry(info)
    if info == nil then return 0 end 
    for k,v in pairs(info.children) do 
        local num = self:CheckCanJewelry(v)
        if num > 0 then 
            return num
        end 
    end 
    return 0
end 

-------宝石红点------------------
function WorkshopData:CheckWorkInset()
    local list = BagData.Instance:GetEquipList()
    local list2 = BagData.Instance:GetJewelryList()
    -- 当前装备的宝石是否可以升级
    local stone_list = {}
    for key, value in pairs(list) do
        local stones = value.param.stone
        for i = 1, EQUIP_STONE_COUNT_MAX do
            if stones[i] > 0 then
                stone_list[stones[i]] = value
            else
                -- local flag_lock = RoleData.Instance:GetRoleLevel() >= self.jewel_limit_level[i].limit_level
                local flag_lock,desc = self:GetStoneSortLimit(value,i)
                if self:GetStoneInBag(value:EquipType()) and flag_lock then
                    return 1
                end
            end
        end
    end
    for key, value in pairs(list2) do
        local stones = value.param.stone
        for i = 1, EQUIP_STONE_COUNT_MAX do
            if stones[i] > 0 then
                stone_list[stones[i]] = value
            else
                -- local flag_lock = RoleData.Instance:GetRoleLevel() >= self.jewel_limit_level[i].limit_level
                local flag_lock,desc = self:GetStoneSortLimit(value,i)
                if self:GetStoneInBag(value:EquipType()) and flag_lock then
                    return 1
                end
            end
        end
    end
    local num = 0
    for key, value in pairs(stone_list) do
        local a, b = self:GemUplevel(key,value)
        num = num + a
        if num > 0 then
            return 1
        end
    end
    return num
end
function WorkshopData:GemUplevel(item_id,equip_data)
    local info = self:GemList(item_id)
    if info == nil then
        return 0
    end
    local gem_need_num = info.stuff_num_1 - 1--(自己也包含)
    local cost_list = {}
    gem_need_num = self:CalcGemNum(item_id, gem_need_num, cost_list,info.type)
    -- LogError("GemUplevel",info,target_info)
    -- local level = RoleData.Instance:GetRoleLevel()
    -- local target_info = self:GetStoneInfo(info.target)
    local flag,limit_desc = self:GetStoneLimitLevelUp(equip_data,{item_id = item_id})
    return (gem_need_num > 0 or not flag ) and 0 or 1, cost_list
end
function WorkshopData:CalcGemNum(item_id, gem_need_num, cost_list, stype)
    --宝石升级规则
    --获取背包中当前物品的数量
    --获取升级所需数量
    --重复这个操作
    local item = Item.Create({item_id = item_id})
    local own_num = item:Num()
    local cost_num = math.min(gem_need_num, own_num)
    cost_list[item_id] = cost_num
    gem_need_num = gem_need_num - cost_num
    if gem_need_num > 0 then
        local info = self:GemList(item_id - 1)--再下一级
        if info == nil or info.type ~= stype then
            -- LogError(gem_need_num) --需要购买的数量
            return gem_need_num
        end
        gem_need_num = gem_need_num * info.stuff_num_1
        return self:CalcGemNum(item_id - 1, gem_need_num, cost_list, stype)
    else
        return gem_need_num
    end
end

function WorkshopData:GemList(item_id)
    if self.gem_list then
        return self.gem_list[item_id]
    end
    self.gem_list = {}
    local group_id = ComposeGroup.Stone
    for key, value in pairs(self.compose_list) do
        if value.group == group_id then
            -- self.gem_list[value.stuff_id_1] = self.gem_list[value.stuff_id_1] or {}
            self.gem_list[value.stuff_id_1] = value
        end
    end
    return self.gem_list[item_id]
end

function WorkshopData:GetQualityRandomGroup(quality_id)
    local list = {}
    for k,v in pairs( self.ornament_cfg_auto.quality_group) do 
        if quality_id == v.quality_id then 
            table.insert( list, v)
        end 
    end 
    return list
end
---熔炼属性
function WorkshopData:SmeltAttrQua( equip_level, attr_info)
    local qualist = Cfg.SmeltAttrQua2(equip_level, attr_info.attr_type)
    if qualist then
        for index, value in ipairs(qualist) do
            if attr_info.attr_value <= value.max and attr_info.attr_value >= value.min then
                attr_info.color = value.quality
                break
            end
        end
    else
        LogError("不应该出现属性类型",attr_info.attr_type)
    end
end

--灵饰
function WorkshopData:SmeltAttrQuaJ( equip_level, attr_info)
    local qualist = Cfg.SmeltAttrQuaJ2(equip_level, attr_info.attr_type)
    if qualist then
        for index, value in ipairs(qualist) do
            if attr_info.attr_value <= value.max and attr_info.attr_value >= value.min then
                attr_info.color = value.quality
                attr_info.is_max = value.quality == 3 and attr_info.attr_value == value.max
                break
            end
        end
    else
        LogError("不应该出现属性类型",attr_info.attr_type)
    end
end
----

function WorkshopData:GetSmeltAttrIsMax(equip_level,attr_info,is_equip)
    -- self.smelt_range_cache
end

function WorkshopData:GetSmeltAttrRange(_item_id,attr_type)
    local equip = Item.Init(_item_id)
    local item_co = equip:Config()
    local equip_level = item_co ~= nil and item_co.equip_level or 1

    -- LogError("???",_item_id,item_co.entry_group,attr_type,equip_level)
    if item_co.entry_group == nil then 
        -- LogError("entry_group",item_co.entry_group,"without config please check item",equip:Name(),_item_id)
        return 
    end 
    local rand_list = self.entry_rand_group[item_co.entry_group][attr_type] -- Cfg.SmeltAttrRandGroup(item_co.entry_group, attr_type)
    if rand_list == nil  then return -1,-1 end 

    -- LogError("rand_list",rand_list)
    local min_qua = -1
    local max_qua = -1
    for k,v in pairs(rand_list) do 
        if min_qua == -1 then 
            min_qua = v.quality
        elseif min_qua > v.quality then 
            min_qua = v.quality
        end 
        if max_qua == -1 then 
            max_qua = v.quality 
        elseif max_qua < v.quality then 
            max_qua = v.quality
        end 
    end 

    -- LogError("min_qua",min_qua,"max_qua",max_qua)
    local min_value = -1
    local max_value = -1
    local qualist = equip:IsEquip() and Cfg.SmeltAttrQua2(equip_level, attr_type) or Cfg.SmeltAttrQuaJ2(equip_level, attr_type)
    -- LogError("qualist",qualist)
    for k,v in pairs(qualist or {}) do 
        if v.quality == min_qua then 
            min_value = v.min
        end 

        if v.quality == max_qua then 
            -- LogError("max!",v)
            max_value = v.max
        end 
    end 
    -- LogError("min",min_value,"max",max_value)

    return min_value,max_value
end

function WorkshopData:GetSmeltAttrAccRange(item_id)
    local equip = Item.New({item_id = item_id})
    local item_co = equip:Config()
    local equip_level = item_co.equip_level
    local equip_qua = item_co.color

    if item_co.entry_group == nil then 
        -- LogError("entry_group",item_co.entry_group,"without config please check item",equip:Name(),_item_id)
        return 
    end 

    local acc_list = {}

    for i,j in pairs(Language.Common.AttrList) do
        local rand_list = self.entry_rand_group[item_co.entry_group][i] -- Cfg.SmeltAttrRandGroup(item_co.entry_group, attr_type)
        if rand_list ~= nil then 
            local min_qua = -1
            local max_qua = -1
            for k,v in pairs(rand_list) do 
                if min_qua == -1 then 
                    min_qua = v.quality
                elseif min_qua > v.quality then 
                    min_qua = v.quality
                end 
                if max_qua == -1 then 
                    max_qua = v.quality 
                elseif max_qua < v.quality then 
                    max_qua = v.quality
                end 
            end 

            local qualist = equip:IsEquip() and Cfg.SmeltAttrQua2(equip_level, i) or Cfg.SmeltAttrQuaJ2(equip_level, i)
            local min_correct = {}
            for k,v in pairs(qualist) do
                if v.quality == min_qua then 
                    min_correct[v.att_type]=v
                end 
            end 
            for k,v in pairs(qualist) do 
                if v.quality > 1 then 
                    if v.quality ~= 6 and (v.quality < 7 or (v.quality >=7 and equip_qua >= v.quality)) then 
                        if acc_list[v.quality] == nil then 
                            acc_list[v.quality] = {}
                            acc_list[v.quality].quality = v.quality
                            acc_list[v.quality].name = string.format(Language.Workshop.SmeltPreAttr,
                            QualityColorStr[v.quality],Language.Workshop.SmeltPreColorStr[v.quality])
                            acc_list[v.quality].children = {}
                        end 
                        local vo ={
                            level = v.level,
                            quality = v.quality,
                            att_type = v.att_type,
                            min = min_correct[v.att_type].min,
                            max = v.max,
                        }
                        table.insert(acc_list[v.quality].children,vo)
                    end
                end 
            end 
        end 
    end 

    return acc_list
end

-- 对具体的词条获取上下限配置
-- 注入的应该是词条信息
function WorkshopData:GetSmeltAttrRangeConfig(data,equip)
    if data == nil then return nil end 
    local list = self:GetSmeltAttrAccRange(equip == nil and self:GetSmeltSelect().item_id or equip.item_id)
    local qua_cfg = list[data.info.color]
    if qua_cfg == nil then 
        return nil 
    end 
    for k,v in pairs(qua_cfg.children) do 
        if v.att_type == data.info.attr_type then 
            return v
        end 
    end 
    return nil
end

function WorkshopData:SetSmeltSelect(data)
    self.smelt_select.data = data
end

function WorkshopData:GetSmeltSelect()
    return self.smelt_select.data
end

function WorkshopData:GetSmeltSelectWithFresh()
    local _, attr = self:GetSmeltSelect():RefreshIncAttr()
    local flag = false
    -- LogError("attr???",attr)
    for k,v in pairs(attr) do 
        if v.attr_value > 0 and v.lock == SmeltStatus.UnLock then 
            flag = true
            break
        end 
    end 
    
    return flag 
end

-- 获取属性选中熔炼对象的信息
function WorkshopData:GetSmeltSelectAttr(data)
    local operate_cell = data == nil and self:GetSmeltSelect() or data
    local _, attr = operate_cell:IncAttr()
    local equip_level = operate_cell:EquipLevel()
    local list = {}
    local limit_lock = self:GetLockLimited(operate_cell)
    for k,v in pairs(attr)do 
        local vo = {}
        vo.index = v.index
        vo.info = v
        vo.is_limited = limit_lock < v.index and v.lock == SmeltStatus.Lock
        local min,max = self:GetSmeltAttrRange(operate_cell.item_id,vo.info.attr_type)
        vo.min = min
        vo.max = max
        if vo.is_limited then 
 
            local list_ = self:GetLockInfo(operate_cell:Color())
            local level_list = self:GetLockInfoByLevel(equip_level)
            local level_ = list_[v.index] ~= nil and list_[v.index].role_level or 0 -- (list_[v.index].level - 1 ) * 10 or 0
            local role_limit = list_[v.index] ~= nil and list_[v.index].role_level or 0
            vo.limited_level = level_ ..Language.Common.Level
            vo.role_limited_level = role_limit
            vo.is_role_limited = RoleData.Instance:GetRoleLevel() < role_limit
            
            local special = self.god_equip_lock_list[v.index]
            local res = Language.Workshop.ShowTypeDesc[special.quality]
            vo.limited_text = string.format(res, special.godequip_level)
            if Equip.IsDivine(operate_cell.item_id) then 
                vo.role_limited_level = special.role_level
                vo.is_role_limited = RoleData.Instance:GetRoleLevel() < special.role_level
            end 

            -- 此处显示为固定减少五级，方便配表
            vo.limit_text = string.format(Language.Workshop.SmeltLockAttrQuaUnlockBoth,(level_),vo.limited_text)
            vo.role_limit_text = string.format(Language.Workshop.SmeltLickArrrRoleLevelUnLock,vo.role_limited_level..Language.Common.Level)        
        end 
        table.insert( list, vo )
    end 
    return list
end

function WorkshopData:GetSmeltOpenLevel(data)

    local level_list = self:GetLockInfo(data:Color())
    local level = level_list[1] ~= nil and (level_list[1].level - 1 ) * 10 or 0
    local role_level = level_list[1] ~= nil and level_list[1].role_level or 0

    return level,role_level
end

function WorkshopData:GetBeforeComplete(index)
    if index == 1 then return true end 
    local list = self:GetSmeltSelectAttr()
    return list[index - 1].info.lock == 1
end

function WorkshopData:GetSmeltSelectRefreshAttr()
    local _, attr_origin = self:GetSmeltSelect():IncAttr()
    local _, attr = self:GetSmeltSelect():RefreshIncAttr()
    table.sort(attr_origin, DataSorter.KeyLowerSorter("index"))
    table.sort(attr, DataSorter.KeyLowerSorter("index"))

    local list = {}
    for i= 1,6 do 
        local vo = {}
        vo.index = i
        vo.info = attr[i]
        vo.info_origin = attr_origin[i]
        local min,max = self:GetSmeltAttrRange(self:GetSmeltSelect().item_id,vo.info.attr_type)
        vo.min = min
        vo.max = max
        table.insert( list,vo )
    end 

    return list
end

-- 只判断被选中的equip
-- ture 需要激活 false 不需要
function WorkshopData:GetSmeltSelectNeedUnLock()

    local _, attr_origin = self:GetSmeltSelect():IncAttr()
    local limit_lock = self:GetLockLimited(self:GetSmeltSelect())

    local lock_flag = true
    for k,v in pairs(attr_origin) do 
        if v.lock == SmeltStatus.UnLock then 
            lock_flag = false
            break
        end 
    end 

    -- 孔位全锁 且 没有已解锁的孔位
    local flag_1 = limit_lock == 0 and lock_flag

    lock_flag = true
    for k,v in pairs(attr_origin) do 
        if v.lock == SmeltStatus.UnLock and limit_lock >= v.index then 
            lock_flag = false
            break
        end 
    end 

    -- 孔位非全锁但全为空
    local flag_2 = limit_lock > 0 and lock_flag

    return flag_1 or flag_2
end

-- 扩展方案，可以判断任意equip
-- ture 需要激活 false 不需要
function WorkshopData:GetEquipNeedUnLock(equip)
    if equip == nil then return false end 

    local _, attr_origin = equip:IncAttr()
    local limit_lock = self:GetLockLimited(equip)

    local lock_flag = true
    for k,v in pairs(attr_origin) do 
        if v.lock == SmeltStatus.UnLock then 
            lock_flag = false
            break
        end 
    end 

    -- 孔位全锁 且 没有已解锁的孔位
    local flag_1 = limit_lock == 0 and lock_flag

    lock_flag = true
    for k,v in pairs(attr_origin) do 
        if Equip.IsDivine(equip.item_id) then 
            if v.lock == SmeltStatus.Lock and limit_lock >= v.index then 
                lock_flag = true
                break
            elseif v.lock == SmeltStatus.UnLock and v.attr_value ~= 0 then 
                lock_flag = false
            end 
        else 
            if v.lock == SmeltStatus.UnLock and limit_lock >= v.index then 
                lock_flag = false
                break
            end 
        end 
    end 

    -- 孔位非全锁但全为空
    local flag_2 = limit_lock > 0 and lock_flag

    return flag_1 or flag_2 
end

-- true 装备孔全空 false 不是
function WorkshopData:GetEquipIsEmpty(equip)
    if equip == nil then return false end 

    local _, attr_origin = equip:IncAttr()
    local limit_lock = self:GetLockLimited(equip)

    local lock_flag = true
    for k,v in pairs(attr_origin) do 
        local flag = v.attr_value == 0
        if limit_lock >= v.index then 
            lock_flag = flag and lock_flag
        end 
    end 

    -- 对比已放开的孔位对比是否全为空
    return lock_flag
end

-- true 装备孔全锁 false 不是
function WorkshopData:GetEquipIsAllLock(equip)
    if equip == nil then return false end 

    local _, attr_origin = equip:IncAttr()
    local limit_lock = self:GetLockLimited(equip)

    local lock_flag = true
    for k,v in pairs(attr_origin) do 
        if v.lock == SmeltStatus.UnLock then 
            lock_flag = false
            break
        end 
    end 

    -- 解锁位为0且没有已解锁的孔位
    -- LogError("lock?!",limit_lock,lock_flag)
    return limit_lock == 0 and lock_flag
end

function WorkshopData:MarkSmeltClickParam(param)
    self.smelt_click_mark = param
end

-- 暂时废弃
function WorkshopData:GetSmeltSelectRefreshNeedSave()
    local _, attr = self:GetSmeltSelect():RefreshIncAttr()
    local limit_lock = self:GetLockLimited(self:GetSmeltSelect())
    local lock_flag = false
    for k,v in pairs(attr) do 
        if v.attr_value > 0 and limit_lock > v.index then 
            lock_flag = true
            break
        end 
    end 

    return lock_flag
end


-- 是否还能锁
function WorkshopData:GetCanSmeltVocLock()
    -- 兼容老装备，以实际开孔数判断
    local attr_list = self:GetSmeltSelectAttr()
    local real_unlock = 0 
    for k,v in pairs(attr_list) do 
        if not v.is_limited then 
            real_unlock = real_unlock + 1
        end 
    end 

    -- 检查开启配置
    local limit_lock = self:GetLockLimited(self:GetSmeltSelect())

    local lock_list = bit:d2b(self.smelt_select_lock.str)
    local lock_num = 0
    for i = 1,6 do 
        lock_num = lock_list[ 33 - i ] == 1 and lock_num + 1 or lock_num 
    end 

    return (real_unlock - 1) > lock_num
end

function WorkshopData:SetSmeltVocLockPos(pos,is_lock)
    local lock_list = bit:d2b(self.smelt_select_lock.str)

    lock_list[ 33 - pos ] = is_lock and 1 or 0

    local str_ = bit:b2d(lock_list)
    self.smelt_select_lock.str = str_ 
end

function WorkshopData:ClearSmeltVocLockPos()
    self.smelt_select_lock.str = 0
end

function WorkshopData:GetSmeltVocLockPos()
    local lock_list = bit:d2b(self.smelt_select_lock.str)

    -- 以等级配置判断
    local limit_lock = self:GetLockLimited(self:GetSmeltSelect())

    -- 兼容老装备，以实际开孔数判断
    local attr_list = self:GetSmeltSelectAttr()
    local real_unlock = 0 
    for k,v in pairs(attr_list) do 
        if not v.is_limited then 
            real_unlock = real_unlock + 1
        end 
    end 

    local list = {}
    for i = 1,6 do
        local vo = (real_unlock >= i and lock_list[ 33 - i ] == 1) and 1 or 0
        table.insert( list, vo)
    end 

    return list
end

function WorkshopData:CheckSmeltAttr()
    --判断熔炼是否开启
    if not FunOpen.Instance:GetFunIsOpened(Mod.Equip.Smelt) then
        return 0
    end
    --1件当前 若干件背包
    local equip_list = BagData.Instance:GetEquipList()
    local num = 0
    for key, value in pairs(equip_list) do
        num = num + self:CheckSmeltEquipAttr(value)
    end
    local jewelry_list = BagData.Instance:GetJewelryList()
    for key, value in pairs(jewelry_list) do
        num = num + self:CheckSmeltJewelryAttr(value)
    end
    return num
end

function WorkshopData:CheckNeoSmeltAttr()
    --判断熔炼是否开启
    if not FunOpen.Instance:GetFunIsOpened(Mod.Equip.Smelt) then
        return 0
    end
    local num = 0

    if self.smelt_func_open.red then 
        -- LogError("!~~~~~~~~~~~~~~~~~~~~~~~~enter!")
        num = num + 1
    end 

    local data_list = self:GetSmeltEquipdList()
    for k,v in pairs(data_list) do 
        local flag_coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind) >= Config.entry_cfg_auto.lock_price[1].coin
        local flag_can = not self:GetEquipIsAllLock(v)
        local flag_complete = self:GetEquipNeedUnLock(v)
        local flag_item_unique = not self:CheckMarkSmeltRedList(v)


        -- if Equip.IsDivine(v.item_id) then 
        --     LogError("??",v:QuaName(),flag_coin,flag_can,flag_complete,flag_item_unique)
        -- end 
        if flag_can and flag_coin and flag_complete and flag_item_unique then 
            num = num + 1
        end 
    end 

    return num
end

--判断当前装备是否持有熔炼属性
function WorkshopData:CheckEquipWithSmeltDone(equip)
    local _,inc_attr = equip:IncAttr()
    for k,v in pairs(inc_attr) do 
        if v.attr_value > 0 then 
            return true
        end 
    end 
    return false
end

function WorkshopData:SetIsFunRed(flag)
    -- LogError("flag~~~~~~~~~~~~~~~~~~~",flag)
    self.smelt_func_open.red = flag
end

function WorkshopData:ClearSmeltRed()
    self.smelt_func_open.clear_red = not self.smelt_func_open.clear_red

    for k,v in pairs(self:GetSmeltEquipdList()) do 
        self:MarkSmeltRedList(v)
    end 
end

function WorkshopData:SetCoinSmeltTime(protocol)
    self.smelt_time_limit.time = protocol.voc_refresh_times
end

function WorkshopData:IsCoinSmeltLimit()
    return Config.entry_cfg_auto.other[1].coin_smelt_times > self.smelt_time_limit.time
end

function WorkshopData:MarkSmeltRedList(_data)
    if Equip.IsDivine(_data.item_id) and _data.param.strengthen_level and _data.param.strengthen_level < 5 then 
        return 
    end 
    local vo = {}
    vo.item_id = _data.item_id
    vo.item_unique_id_1 =  _data.item_unique_id_1
    vo.item_unique_id_2 =  _data.item_unique_id_2
    table.insert( self.smelt_red_equip_list,vo)
end

function WorkshopData:CheckMarkSmeltRedList(_data)
    for k,v in pairs(self.smelt_red_equip_list) do 
        if v.item_unique_id_1 == _data.item_unique_id_1 and v.item_unique_id_2 ==  _data.item_unique_id_2 then 
            if Equip.IsDivine(_data.item_id) then 
                return not self:GetEquipNeedUnLock(_data)
            else
                return true
            end 
        end 
    end 
    return false
end

function WorkshopData:CheckSmeltEquipAttr(data)
    local equip = data
    local list = BagData.Instance:GetGridByEquipType(equip:EquipType(), equip:LimitLv())
    -- 背包里的
    local num = 0
    for index, value in ipairs(list) do
        num = num + self:CheckSmeltEquipAttrNum(equip, value)
    end
    return num
end

function WorkshopData:CheckSmeltJewelryAttr(data)
    local equip = data
    local list = BagData.Instance:GetGridByJewelryType(equip:JewelryType(), equip:LimitLv())
    -- 背包里的
    local num = 0
    for index, value in ipairs(list) do
        num = num + self:CheckSmeltEquipAttrNum(equip, value)
    end
    return num
end

function WorkshopData:CheckEquipVocIsLock(equip,voc_index)
    local equip_data = equip or self.smelt_select.cur_equip_data
    local cur_data = BagData.Instance.equipment_voc_list

    local flag_is_equip = equip_data:IsEquip()
    local lock_flag = flag_is_equip and cur_data.equipment_list[equip_data.index+1].is_lock[voc_index] 
    or cur_data.jewelry_list[equip_data.index+1].is_lock[voc_index]

    return lock_flag == 0
end

function WorkshopData:CheckSmeltEquipAttrNum(equip, r_equip)
    --左右有相同判断大小显示
    --左边有空显示
    --左边最小和右边最大比较
    local _, attr1 = equip:IncAttr()
    local _, attr2 = r_equip:IncAttr()
    local prof_type = RoleData.Instance:GetRoleProfType()
    local logic_a = false
    local exclude = {}
    local valueB = 0
    local priorityB = 0
     --用红点的逻辑吧
    for index, value in ipairs(attr1) do
        for index2, value2 in ipairs(attr2) do
            if value.attr_value > 0 and value.attr_type == value2.attr_type 
                and value2.attr_value > 0 and value.lock == 1 and value2.lock == 1
                and self:CheckEquipVocIsLock(equip,index) then
                if value2.attr_value > value.attr_value then
                    -- print("类型相同且我比你大"..value.attr_type)
                    return 1
                else
                    --排除计算的类型
                    exclude[value.attr_type] = 1
                end
            end
        end
    end
    if logic_a == false then
        --找出空和最大
        local logic_b = false
        local priority = 0
        for index, value in ipairs(attr2) do
            if value.attr_value ~= 0 and exclude[value.attr_type] ~= 1 and value.lock == 1 
                and self:CheckEquipVocIsLock(equip,index) then
                local cfg = Cfg.SmeltAttrPri(prof_type, value.attr_type)
                local temp = cfg.priority * value.attr_value
                priority = math.max(temp, priority)
                if priority == temp then
                    valueB = value.attr_value
                    priorityB = priority
                end
            end
        end
        if valueB == 0 then
            return 0
        end
        for index, value in ipairs(attr1) do
            if value.attr_value == 0 and value.lock == 1 
                and self:CheckEquipVocIsLock(equip,index) then
                -- print("你有空属性")
                logic_b = true
                return 1
            end
        end
        -- print("权重最大"..priorityB)
        if logic_b == false then
            priority = 9999999
            for index, value in ipairs(attr1) do
                if value.attr_value ~= 0 and exclude[value.attr_type] ~= 1 and value.lock == 1 
                    and self:CheckEquipVocIsLock(equip,index) then
                    local cfg = Cfg.SmeltAttrPri(prof_type, value.attr_type)
                    if cfg.priority ~= 0 then
                        local temp = cfg.priority * value.attr_value
                        priority = math.min(temp, priority)
                        if priority == temp then--越小越被推荐替换
                            if priorityB >= priority then -- and valueB > value.attr_value then
                                -- print("你的权重比我小"..value.attr_type)
                                return 1
                            end
                        end
                    end
                end
            end
        end
    end
    return 0

    --[[ for index, value in ipairs(attr2) do
        for index2, value2 in ipairs(attr1) do
            if value.attr_value > 0 and value.attr_type == value2.attr_type then
                --比较评分大小
                local cfg = Cfg.SmeltAttrPri(prof_type, value.attr_type)
                if (value.attr_value * cfg.priority) > (value2.attr_value * cfg.priority) then--右边的评分比左边好
                    return 1
                end
            end
        end
    end
    for index, value in ipairs(attr1) do
        if value.attr_value == 0 then
            return 1
        end
    end
    local minA = 9999999
    for index, value in ipairs(attr1) do
        if value.attr_value ~= 0 then
            local cfg = Cfg.SmeltAttrPri(prof_type, value.attr_type)
            if cfg.priority ~= 0 then
                local temp = cfg.priority * value.attr_value
                minA = math.min(temp, minA)
            end
        end
    end
    local maxB = 0
    for index, value in ipairs(attr2) do
        if value.attr_value ~= 0 then
            local cfg = Cfg.SmeltAttrPri(prof_type, value.attr_type)
            local temp = cfg.priority * value.attr_value
            maxB = math.max(temp, maxB)
        end
    end
    if maxB > minA then
        return 1
    end
    return 0 ]]
end

function WorkshopData:GetSmeltEquipdList()
    local data = BagData.Instance:GetEquipList()
    local data2 = BagData.Instance:GetJewelryList()

    local new_list = {}
    for k,v in pairs(data) do
        table.insert( new_list, v )
    end 

    for k,v in pairs(data2) do
        table.insert( new_list, v )
    end 

    return new_list
end

function WorkshopData:GetSmeltIsEquipd(_data)
    local data_list = self:GetSmeltEquipdList()

    for k,v in pairs(data_list) do 
        if v.item_unique_id_1 == _data.item_unique_id_1 and v.item_unique_id_2 == _data.item_unique_id_2 then 
            return true
        end 
    end 

    return false
end

function WorkshopData:GetSmeltEquipList()
    local data = BagData.Instance:GetEquipList()
    local data2 = BagData.Instance:GetJewelryList()
    local data3 = BagData.Instance:GetGridByType({ItemType.Weapon, ItemType.Armor,ItemType.Jewelry})
    local new_list = {}

    for k, v in pairs(data) do
        table.insert(new_list, v)
    end

    for k, v in pairs(data2) do
        table.insert(new_list, v)
    end

    table.sort(new_list, function(a, b)
        if a:EquipTypeSort() > b:EquipTypeSort() then
            return true
        else
            return false
        end
    end)


    table.sort(data3, self.CountEquipSort())

    for k, v in pairs(data3) do
        table.insert(new_list, v)
    end

    return new_list
end

-- 检查该熔炼的项是否显示红点
function WorkshopData:GetSmeltBtnRed(data)
    local flag_coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind) >= Config.entry_cfg_auto.lock_price[1].coin
    local flag_can = not self:GetEquipIsAllLock(data == nil and self:GetSmeltSelect() or data)
    local flag_complete = self:GetEquipNeedUnLock(data == nil and self:GetSmeltSelect() or data)
    local flag_item_unique = not self:CheckMarkSmeltRedList(data == nil and self:GetSmeltSelect() or data)
    local flag_item_equiped = self:GetSmeltIsEquipd(data == nil and self:GetSmeltSelect() or data)

    return  flag_coin and flag_can and flag_complete and flag_item_unique and flag_item_equiped
end

function WorkshopData.CountEquipSort()
	return function(a, b)
        local itemA = a:Color() * 100000 + a:EquipTypeSort() + a:EquipLevel() * 100 + a:GetRankScore() * 10
        local itemB = b:Color() * 100000 + b:EquipTypeSort() + b:EquipLevel() * 100 + b:GetRankScore() * 10
        itemA = AnotherData.Instance:CheckEquipmentUniqueId(a,a:IsEquip()) and itemA+ 100000 or itemA
        itemB = AnotherData.Instance:CheckEquipmentUniqueId(b,b:IsEquip()) and itemB+ 100000 or itemB
        if itemA > itemB then
            return true
        else
            return false
        end
	end
end

function WorkshopData:GetSmeltPlusEquipList()
    local equip = self:GetSmeltSelect()
    local data = BagData.Instance:GetGridByType({equip:Type()})
    local list = {}
    for k,v in pairs(data) do 
        local flag_select = true
        if equip.IsEquip and equip:IsEquip() and equip:EquipType() == v:EquipType() and equip:Color() >= v:Color() 
            and (equip:LimitLv() >= v:LimitLv() or Equip.IsDivine(equip.item_id))
        --    and AnotherData.Instance:CheckEquipmentUniqueId(v, v:IsEquip()) 
           and self:GetLockLimited(v) > 0 and not self:GetEquipNeedUnLock(v) 
           and not self:GetEquipIsEmpty(v) then
            if equip.in_bag == true and v.in_bag == true and v.index ~= equip.index then
                table.insert( list, v )
            elseif equip.in_bag == false and v.in_bag == true then 
                table.insert( list, v )
            end 
        elseif equip.IsJewelry and equip:IsJewelry() and equip:JewelryType() == v:JewelryType() and equip:Color() >= v:Color() 
            and (equip:LimitLv() >= v:LimitLv() or Equip.IsDivine(equip.item_id)) 
            -- and AnotherData.Instance:CheckEquipmentUniqueId(v, v:IsEquip()) 
            and self:GetLockLimited(v) > 0 and not self:GetEquipNeedUnLock(v) 
            and not self:GetEquipIsEmpty(v) then
            if equip.in_bag == true and v.in_bag == true and v.index ~= equip.index then
                table.insert( list, v )
            elseif equip.in_bag == false and v.in_bag == true then 
                table.insert( list, v )
            end 
        end 
    end 
    return list
end

function WorkshopData:CatchSmeltAttrScore(list)
    local score = 0
    local prof_type = RoleData.Instance:ProfType()
    
    for k,v in pairs(list) do 
        if v.info.lock == 1 then 
            score = score + BagData.Instance:GetRoleScoreByAttr(v.info.attr_type, v.info.attr_value)
        elseif v.info_origin then 
            score = score + BagData.Instance:GetRoleScoreByAttr(v.info_origin.attr_type, v.info_origin.attr_value)
        end 
    end 
    -- LogError("score ",score)
    local base_data = self:GetSmeltSelect()

    -- local only_base = 0
    for k,v in pairs(base_data.param.attr_list) do 
        score = score + BagData.Instance:GetRoleScoreByAttr(v.attr_type, v.attr_value)
        -- only_base =  BagData.Instance:GetRoleScoreByAttr(v.attr_type, v.attr_value)
    end 
    -- local only_stone = 0
    if base_data.param.stone then 
        for i = 1,2 do 
            local info = self:GetStoneInfo(base_data.param.stone[i])
            if info ~= nil then 
                for j = 1,2 do 
                    score = score + BagData.Instance:GetRoleScoreByAttr(info["type_"..j], info["value_"..j])
                    -- only_stone =  BagData.Instance:GetRoleScoreByAttr(info["type_"..j], info["value_"..j])
                end 
            end  
        end 
    end 
    -- LogError("only_base ",only_base,"only_stone",only_stone)
    return math.floor(score)
end

function WorkshopData:SetLockJewelryMatrial(item_id)
    self.lock_jewelry_matrial = item_id
end

function WorkshopData:ClearLockJewelry() 
    self.lock_jewelry_matrial = nil
end

function WorkshopData:CheckIsLock(_equip,attr_list)
    
    local result = {}
    local lock_cfg = Cfg.SmeltAttrCellMax(_equip:EquipLevel())
    local lock_num = lock_cfg ~= nil and EQUIP_ITEM_ATTR_NUM - lock_cfg.max_num or 0

    for k,v in ipairs(attr_list) do 
        local vo = {}
        vo.color = v.color
        vo.attr_value = v.attr_value
        vo.index = v.index
        vo.attr_type = v.attr_type

        table.insert( result, vo )
    end 

    for i = #result ,1, -1 do 
        result[i].is_lock = lock_num > 0
        lock_num = lock_num > 0 and lock_num -1 or 0
    end 

    return result
end 

-- 用于客户端显示锁闭情况
function WorkshopData:InitLockCheckList()
    local config = Config.entry_cfg_auto.max_num
    self.equip_lock_list = {}
    self.equip_lock_level_list = {}
    local unlock_pos = {}
    local lock_pos = {}

    for i = 1, #config do
        if self.equip_lock_list[config[i].quality] == nil then 
            self.equip_lock_list[config[i].quality] = {}
        end 

        for j = 1,config[i].max_num do  
            if self.equip_lock_list[config[i].quality][j] == nil then 
                self.equip_lock_list[config[i].quality][j] = config[i] 
            elseif config[i].max_num ~= 6 and
                self.equip_lock_list[config[i].quality][j].max_num == config[i].max_num then 
                self.equip_lock_list[config[i].quality][j] = config[i] 
            end  
        end 
    end 

    for k,v in pairs(config) do 
        if self.equip_lock_level_list[v.level] == nil then 
            self.equip_lock_level_list[v.level] = {}
        end 
        table.insert( self.equip_lock_level_list[v.level], v)
    end 

    self.god_equip_lock_list = Config.entry_cfg_auto.godequip_num
    -- for i = 1, #_config do 
    --     if _unlock_pos < _config[i].max_num then 
    --         local _var = _config[i].max_num - _unlock_pos
    --         _lock_pos = _lock_pos - _var
    --         _unlock_pos = _config[i].max_num

    --         for j = 1 , _var do 
    --             table.insert( self.equip_lock_list, _config[i])
    --         end 

    --         if _unlock_pos == 6 then 
    --             break 
    --         end 
    --     end 
    -- end 
end

function WorkshopData:InitQueChingList()
    self.que_ching_comsume_list = KeyList(Config.entry_cfg_auto.quenching_consume,"equip_num_quality")
    self.que_ching_add_list = {}
    for k,v in pairs(Config.entry_cfg_auto.equip_num_add) do 
        if self.que_ching_add_list[v.equip_num_quality] == nil then 
            self.que_ching_add_list[v.equip_num_quality] = {}
        end 

        self.que_ching_add_list[v.equip_num_quality][v.att_type] = v
    end 
end

function WorkshopData:GetLockInfo(qua)
    return self.equip_lock_list[qua]
end

-- 横向获取该等级下的全部品质
function WorkshopData:GetLockInfoByLevel(equip_level)
    -- local result_list = {}
    -- for k,v in pairs(Config.entry_cfg_auto.max_num) do 
    --     if v.level == equip_level then 
    --         table.insert(result_list,v)
    --     end 
    -- end 
    -- return result_list
    return self.equip_lock_level_list[equip_level]
end

function WorkshopData:GetLockLimited(equip)-- (equip_level,quality)
    if equip == nil then return -1 end 

    local equip_level = equip.EquipLevel ~= nil and equip:EquipLevel() or Equip.GetEquipLevel(equip.item_id)
    local quality = equip:Color()
    local role_level = RoleData.Instance:GetRoleLevel()
    
    -- 神装判断
    if quality >= ItemColor.Star then 
        local godequip_level = equip.param and equip.param.strengthen_level or 0
        local max_num = 0
        for k,v in pairs(self.god_equip_lock_list) do 
            if (quality > v.quality or (godequip_level >= v.godequip_level and quality == v.quality)) and role_level >= v.role_level then 
                max_num = v.max_num
            end 
        end 
        return max_num
    end 

    -- 装备判断
    
    if self.equip_lock_list[quality] == nil then return -1 end 
    -- 判断是否是最大的 直接跳过循环（21727需求要求判断玩家等级，故废弃
    -- if equip_level > self.equip_lock_list[quality][#self.equip_lock_list[quality]].level then 
    --     return self.equip_lock_list[quality][#self.equip_lock_list[quality]].max_num
    -- end 

    local max_num = 0
    for k,v in pairs(self.equip_lock_list[quality]) do 
        if equip_level >= v.level and role_level >= v.role_level then 
            max_num = v.max_num
        end 
    end 

    return max_num
end

function WorkshopData:GetJewelryMaxSmeltNum(target_info)
    local max_num = 0
    for i = 1,#self.equip_lock_list do
        if ((target_info.level >= 10 and target_info.level or 10  )/ 10) + 1 >= self.equip_lock_list[i].level then 
            max_num = max_num + 1
        end 
    end 
    return max_num
end

function WorkshopData:CheckIsAllLock(_equip)
    local lock_cfg = Cfg.SmeltAttrCellMax(_equip:EquipLevel())
    return lock_cfg.max_num == 0
end

function WorkshopData:MarkDrawOffEquip(equip)
    if Equip.IsDivine(equip.item_id) then return end 
    if self.draw_off_equip ~= nil then return end 

    local compare = equip:Compare()
    if compare == nil or Equip.IsDivine(compare.item_id) then return end 

    local flag_color = equip:Color() >= compare:Color()  -- 品质更高
    local flag_level = (equip:LimitLv() >= compare:LimitLv()) -- 等级更高
    local flag_limit =  self:GetLockLimited(equip) >= self:GetLockLimited(compare) -- 孔位足够
    local flag_unlock = self:GetEquipNeedUnLock(equip) and not self:GetEquipIsEmpty(compare) -- 换装的未激活且被换装并非空装

    if not (flag_color and flag_level and flag_limit and flag_unlock ) then 
        self.draw_off_equip = nil
        return 
    end 

    local vo_equip = {
        index = equip:EquipType(),
        item_unique_id_1 = (equip.item_unique_id_1 and equip.item_unique_id_1 > 0) and equip.item_unique_id_1 or equip.vo.item_unique_id_1,
        item_unique_id_2 = (equip.item_unique_id_2 and equip.item_unique_id_2 > 0) and equip.item_unique_id_2 or equip.vo.item_unique_id_2
    }

    local vo_down = {
        index = compare:EquipType(),
        item_unique_id_1 = (compare.item_unique_id_1 and compare.item_unique_id_1 > 0) and compare.item_unique_id_1 or compare.vo.item_unique_id_1,
        item_unique_id_2 = (compare.item_unique_id_2 and compare.item_unique_id_2 > 0) and compare.item_unique_id_2 or compare.vo.item_unique_id_2
    }

	local flag_equip,list_e = EquipSetData.Instance:GetRoleSuitPlusReady(equip)
    local flag_down,list_d = EquipSetData.Instance:GetRoleSuitPlus(compare)

    self.draw_off_equip = {
        equip = vo_equip,
        draw_off = vo_down,
        need_wait = flag_equip
    }
end

function WorkshopData:UnMarkDrawOffEquip(equip)
    if self.draw_off_equip == nil then 
        return 
    end

    if self.draw_off_equip.need_wait then 
        self.draw_off_equip.need_wait = false
        self.draw_off_equip.cache = equip
        return 
    end 

    local operate = self.draw_off_equip.cache ~= nil and self.draw_off_equip.cache:IsEquip() or (equip ~= nil and equip:IsEquip() or nil)
    local equiped = operate and ( self.draw_off_equip.equip ~= nil and BagData.Instance:GetEquipList(self.draw_off_equip.equip.index) or (equip ~= nil and equip:Compare() or nil))
        or (self.draw_off_equip.equip ~= nil and BagData.Instance:GetJewelryList(self.draw_off_equip.equip.index - 5) or (equip ~= nil and equip:Compare() or nil))
    local equip_inherit = BagData.Instance:SearchEquipByUniqueId(self.draw_off_equip.draw_off) or (self.draw_off_equip.cache ~= nil and self.draw_off_equip.cache or equip)

    if equip_inherit == nil or equiped == nil then
        self.draw_off_equip = nil
        return 
    end 

    local function send_msg()
        local limited_lock = self:GetLockLimited(equiped)
        for i = 1,limited_lock do 
            WorkshopCtrl.Instance:SendOperateReq(
                WORLSHOP_OP.WORKSHOP_OP_AVTIVE_EQUIP,
                equiped:IsEquip() and 0 or 1 ,
                equiped.in_bag == true and 1 or 0,
                equiped.index,
                i - 1,
                AnotherData.Instance:CheckEquipmentUniqueId(equiped,equiped:IsEquip()) and 0 or 1
            )
        end

        local param_t = {}
        param_t.change_voc_type = equiped:IsEquip() and 0 or 1
        param_t.role_type = AnotherData.Instance:CheckEquipmentUniqueId(equiped, equiped:IsEquip()) and 0 or 1
        param_t.replacer_posi = equiped.in_bag and 1 or 0
        param_t.replacer_grid = equiped.index
        param_t.replacer_voc_index = 0

        param_t.bereplacer_posi = equip_inherit.in_bag and 1 or 0
        param_t.bereplacer_grid = equip_inherit.index
        param_t.bereplacer_voc_index = 0
    
        WorkshopCtrl.Instance:ChangeEquipVocReq(param_t)
    end 

    local param_t = {
        content =
            Language.Workshop.DrawEquipToSmerlt,
        confirm = {
            func =  function()
                send_msg()
                ViewMgr:CloseView(DialogTipsView)
                AudioMgr:PlayEffect(AudioType.UI, "workshop_forging")
            end 
        }
    }

    PublicPopupCtrl.Instance:DialogTips(param_t)

    self.draw_off_equip = nil
end

function WorkshopData:GetLockJewelryRecipe()
    if self.lock_jewelry_matrial == nil then return nil end 

    local mark_pos = {}
    local data = self:GetJweelryAccorCfg(true)
    local flag_success = false
    for k,v in ipairs(data) do
        for i,j in pairs(v.children) do 
            for m = 1, 5 do 
                if j["stuff_id_"..m] == self.lock_jewelry_matrial then 
                    mark_pos.parent = k
                    mark_pos.child = i
                    flag_success = true 
                    break 
                end 
            end 
            if flag_success then break end 
        end 
        if flag_success then break end 
    end 
    return mark_pos
end

function WorkshopData:InitJewelryProInfo()
    local origin_cfg = Config.ornament_item_auto

    self.ornament_group_list = {}
    for k,v in pairs(Config.ornament_item_auto) do 
        if self.ornament_group_list[v.ornament_type] == nil then 
            self.ornament_group_list[v.ornament_type] = {}
        end 

        if self.ornament_group_list[v.ornament_type][v.limit_level] == nil then 
            self.ornament_group_list[v.ornament_type][v.limit_level] = {}
        end 

        table.insert( self.ornament_group_list[v.ornament_type][v.limit_level], v)
    end 
end

function WorkshopData:GetJewelryPreInfo(_target)
    if _target== nil then return end 
    local info = {}

    local info_cfg =  self.ornament_group_list[_target.ornament_type][_target.level]
    local quality_group = self:GetQualityRandomGroup(_target.item_random)

    for k,v in pairs(info_cfg or {}) do 
        local flag_with = false
        for i,j in pairs(quality_group) do 
            if j.color == v.color then
                flag_with = true
            end
        end 

        if flag_with then
            if info[v.attr_type] == nil then 
                info[v.attr_type] = {}
                info[v.attr_type].attr_max = v.attr_max
                info[v.attr_type].attr_min = v.attr_min
            else
                if v.attr_max > info[v.attr_type].attr_max then info[v.attr_type].attr_max = v.attr_max end 
                if v.attr_min < info[v.attr_type].attr_min then info[v.attr_type].attr_min = v.attr_min end
            end 

            if info[v.attr_type_2] == nil and v.attr_type_2 ~= 0 then 
                info[v.attr_type_2] = {}
                info[v.attr_type_2].attr_max = v.attr_max_2
                info[v.attr_type_2].attr_min = v.attr_min_2
            elseif v.attr_type_2 ~= 0  then 
                if v.attr_max_2 > info[v.attr_type_2].attr_max then info[v.attr_type_2].attr_max = v.attr_max_2 end 
                if v.attr_min_2 < info[v.attr_type_2].attr_min then info[v.attr_type_2].attr_min = v.attr_min_2 end
            end 
        end 
    end 

    return info
end

--是否关闭工坊
--不能让他自己允许关闭自己
function WorkshopData.IsCloseView(open_panel)
    return open_panel == Mod.Store.Main or open_panel == Mod.Mall.Trade or open_panel == Mod.Mall.Shop
        or open_panel == Mod.Workshop.Forging or open_panel == Mod.Workshop.Jewelry --or open_panel == Mod.Workshop.Inser or open_panel == Mod.Workshop.Smelt
        or open_panel == Mod.Workshop.NeoCompose or open_panel == Mod.Workshop.NeoCompose
end

function WorkshopData:QuickComposeType(show_type,target_id)
    if show_type == nil then return end 

    local param_t = {
        [1] = ComposeGroup.EleCrystal,
        [2] = ComposeGroup.Card,
        -- [3] = ComposeGroup.Key,
        [4] = ComposeGroup.PetFlush,
        [5] = ComposeGroup.MarbasRefine,
        -- [3] = ComposeGroup.Skill,
    }
    local data_list = self:GetTypeShowCombine(param_t)
    local temp = {}
    for k,v in pairs(data_list) do 
        if v.group_id == show_type then
            temp = {
                type_id = v.type_id,
                type_name = Config.compose_auto.group_list[v.group_id].group_name,
                group_id = v.group_id,
                target_id = target_id,
            }
            break
        end 
    end 

    self.crystal_compose_data.select_type = temp.type_id
    self.crystal_compose_data.temp_name = temp.type_name
    self.crystal_compose_data.temp_item_id = temp.target_id
end 

function WorkshopData:QuickComposeItem(target_id)
    local acc_list = self:GetNeoComposeAccorCfg()

    for k,v in pairs(acc_list) do 
        for i,j in pairs(v.children) do 
            if j.target == target_id then 
                self.crystal_compose_data.select_type = v.type_id
                self.crystal_compose_data.temp_name = Config.compose_auto.group_list[v.group_id].group_name
                self.crystal_compose_data.temp_item_id = target_id
                break
            end 
        end 
    end 
end

function WorkshopData:ClearQuickComposeType()
    self.crystal_compose_data.select_type = 9 --初始化
    self.crystal_compose_data.temp_name = nil
    self.crystal_compose_data.temp_item_id = nil
end 

function WorkshopData:GetLockNeoCompose()
    if self.crystal_compose_data.temp_name == nil then return nil end 

    local mark_pos = {}
    local data = self:GetNeoComposeAccorCfg(true)
    local flag_success = false
    for k,v in ipairs(data) do
        for i,j in pairs(v.children) do 
            if v.type_id == self.crystal_compose_data.select_type then 
                mark_pos.parent = k
                if self.crystal_compose_data.temp_item_id == nil 
                   or self.crystal_compose_data.temp_item_id == j.target then 
                    mark_pos.child = i
                    flag_success = true 
                break
                end 
            end 
            if flag_success then break end 
        end 
        if flag_success then break end 
    end 
    return mark_pos
end 

function WorkshopData:CheckCanNeoCompose(data)
    local flag = data.stuff_id == nil 
    local is_can = true
    if flag then 
        for i = 1,6 do 
            local item_id = data["stuff_id_"..i] or 0
            local need_num = data["stuff_num_"..i] or 0
    
            if item_id > 0 then 
                local item = Item.New()
                item.item_id = item_id
                if item:Num() < need_num then 
                    is_can = false 
                    break
                end 
            end 
        end 
        is_can = data.type > 0 and is_can
    else 
        local item = Item.New()
        item.item_id = data.stuff_id
        if item:Num() < data.num then 
            is_can = false
        end 
        is_can = data.type > 0 and is_can
    end 

    return is_can
end 

--根据配方id获取配方配置
function WorkshopData:GetActivationConfig(item_id)
    return self.activation_config[item_id]
end
function WorkshopData:GetActivationSeqConfig(seq)
    return self.active_seq_config[seq]
end
function WorkshopData:GetProfRecommend()
    local prof_type = RoleData.Instance:ProfType()
    local info = self.jewel_recommend[1]
    for k,v in pairs(self.jewel_recommend) do 
        if v.job == prof_type then 
            info = v
        end 
    end 

    local str_ = ""
    local jewels = Split(info.recommend_jewel,"|")
    for k,v in pairs(jewels) do 
        for i,j in pairs(self.jewel_group) do 
            if tonumber(v) == j.jewel_type then 
                str_ = str_ .. string.format( Language.Common.Xstr,COLORSTR.Yellow1,j.type_name)
                if k ~= #jewels then 
                    str_ = str_ .."、"
                end 
            end 
        end 
    end 

    local str_total = string.format( Language.Workshop.StoneRecommend,str_ )
    return str_total
end 

-- 获取职业该部位该宝石是否推荐
function WorkshopData:GetProfRecommendEquipType(equip_type,item_id)
    local prof_type = RoleData.Instance:ProfType()
    local cfg = self.jewel_type_recommend[prof_type][equip_type]
    if cfg == nil then return false end 
    local jewels = Split(cfg.recommend_jewel,"|")
    local stone_info = self:GetStoneInfo(item_id)
    -- LogError("!?",stone_info.jewel_type,jewels)
    for k,v in pairs(jewels) do 
        if stone_info.jewel_type == tonumber(v) then 
            return true
        end 
    end 
    return false
end

-- 获取职业该宝石是否推荐
function WorkshopData:GetProfRecommendAllPart(item_id)
    if Item.GetType(item_id) ~= ItemType.Gem then return false end 
    local prof_type = RoleData.Instance:ProfType()
    local cfg = self.jewel_job_recommend[prof_type]
    local stone_info = self:GetStoneInfo(item_id)

    local jewels = Split(cfg.recommend_jewel,"|")
    -- LogError("!?",prof_type,stone_info.jewel_type,cfg)
    for k,v in pairs(jewels) do 
        if stone_info.jewel_type == tonumber(v) then 
            return true
        end 
    end
    return false
end

-- 获取职业该宝石是否不推荐
function WorkshopData:GetProfNotRecommendAllPart(item_id)
    if Item.GetType(item_id) ~= ItemType.Gem then return false end 
    local prof_type = RoleData.Instance:ProfType()
    local cfg = self.jewel_job_recommend[prof_type]
    local stone_info = self:GetStoneInfo(item_id)
    local jewels = Split(cfg.not_recommend_jewel,"|")
    -- LogError("?flahs!",prof_type,cfg,stone_info.jewel_type)
    for k,v in pairs(jewels) do 
        if stone_info.jewel_type == tonumber(v) then 
            return true
        end 
    end 
    return false
end

-------------------------------------------------------
-------------------Neo Jewerly-------------------------
function WorkshopData:JewerlyData()
    return self.jewelry_data
end

function WorkshopData:GetJewerlyShowLevel(level)
    local change_level = math.floor( level /10 ) + 1
    local result = change_level <= WorkshopData.Params.MinJewerlyLevel and WorkshopData.Params.MinJewerlyLevel + 1 or change_level
    return result
end

function WorkshopData:GetJewerlyLevelShowList()
    return Language.Workshop.JewerlyLevel
end

function WorkshopData:GetJewerlyTypeShow()
    local show = self:GetTypeShow(ComposeGroup.Jewelry)
    local type_id = self.jewelry_data.select_prof
    local level = self.jewelry_data.select_level
    return show[type_id].type_name,self.level_show.jewerly[level].name
end

function WorkshopData:GetJweelryShowList()
    local type_id = self.jewelry_data.select_prof
    local level = self.jewelry_data.select_level
    local data = {}
    for k, v in pairs(self.compose_list) do
        if v.type == type_id then
            if Equip.GetEquipLevel(v.target) == level and not Equip.IsDivine(v.target) and v.is_not_show == 0 then
                table.insert(data, v)
            end
        end
    end
    -- 排序 按品质排策划说的
    table.sort(data, function(a, b)
        local itemA = Item.Create({item_id = a.target})
        local itemB = Item.Create({item_id = b.target})
        if (itemA:Color() * 10 + itemA:JewelryType()) < (itemB:Color() * 10  + itemB:JewelryType())then
            return true
        else
            return false
        end
    end)

    return data
end 

function WorkshopData:GetJewerlyMark()
    return self.jewerly_mark_data
end

function WorkshopData:SetJewerlyMark(data)
    self.jewerly_mark_data = data
end
function WorkshopData:GetUltiEquipList()
    local list1 = BagData.Instance:GetEquipList()
    local list2 = BagData.Instance:GetGridByType({ItemType.Weapon, ItemType.Armor, ItemType.Jewelry})
    local list3 = BagData.Instance:GetJewelryList()
    local new_list = {}
    for index, value in pairs(list1) do
        if value.HasScore and value:HasScore() and value.IsElder and not value:IsElder() then
            table.insert(new_list, value)
        end
    end
    for index, value in pairs(list3) do
        if value.HasScore and value:HasScore() and value.IsElder and not value:IsElder() then
            table.insert(new_list, value)
        end
    end
    for index, value in ipairs(list2) do
        if value.HasScore and value:HasScore() and value.IsElder and not value:IsElder() then
            table.insert(new_list, value)
        end
    end
    return new_list
end

function WorkshopData:GetUltiConsume(equip)
    local co = equip:Config()
    return self.jg_consume_list[co.equip_type][co.color][co.limit_level]
end
function WorkshopData:GetEquipResolve(equip)
    local co = equip:Config()
    return self.equip_resolve_list[co.equip_type][co.color][co.limit_level]
end
function WorkshopData:GetEquipResolveList(list)
    local res = {}  
    local cfg
    local is_repet
    local immortal_num = 0
    for _, equip in ipairs(list) do
        cfg = self:GetForgeComposeCfg(equip.item_id)
        for i = 1, 5 do
            is_repet = false
            for key, value in pairs(res) do
                if value.item_id == cfg["stuff_id_"..i] then
                    is_repet = true
                end
            end
            if not is_repet and cfg and cfg["stuff_id_"..i] > 0 and cfg["resolve_return_"..i] > 0 then
                table.insert( res, Item.Init(cfg["stuff_id_"..i]))
            end
        end
        --[[ for index, vo in pairs(cfg.resolve_get_item) do
            local is_repet = false
            for _, item in ipairs(res) do
                if item.item_id == vo.item_id then
                    is_repet = true
                    item.num = item.num + vo.num
                end
            end
            if is_repet == false then
                table.insert(res, Item.Create(vo))
            end
        end ]]
        local blaze_cfg = equip:IsBlazeStar() and self:GetBlazeCfg(equip.item_id,equip:BlazeStar()) or nil
        -- LogDG("!?re@",cfg.resolve_return_lingyu,math.floor((equip:IsBlazeStar() and (blaze_cfg.return_times/1000) or 1) * cfg.resolve_return_lingyu),cfg,blaze_cfg)
        immortal_num = immortal_num + math.floor((equip:IsBlazeStar() and (blaze_cfg.return_times/1000) or 1) * cfg.resolve_return_lingyu)
    end
    -- 新增返还灵玉
    if immortal_num > 0 then
        table.insert(res, Item.Init(CommonItemId.ImmortalCoin, immortal_num))
    end
    return res
end

--根据合成表配置获取材料id列表
function WorkshopData:GetStuffList(config)
    local list = {}
    for i=1,5 do
        local item_id = config["stuff_id_"..i]
        if item_id ~= 0 then
            table.insert(list,{item_id = item_id,num = config["stuff_num_"..i]})
        end
    end
    return list
end

function WorkshopData.GetScoreTip(item)
    if item and (item:IsEquip() or item:IsJewelry()) and item.HasScore and item.CanResolve2 then
        local value = 0
		local hast_score = false -- not item:HasScore()
        local cant_resolve = not item:CanResolve2()
		if hast_score and cant_resolve then
			value = 1
		elseif hast_score then
			value = 2
		elseif cant_resolve then
			value = 3
        end
        return Language.ItemInfo.EquipTips[value]
    end
end
function WorkshopData:SetSelectUltiData(item)
    self.ulti_select_data = item
end
function WorkshopData:GetSelectUltiData()
    return self.ulti_select_data
end

--通过注入的装备信息解析成Ring信息
--data base,utli{attr_type,cur,max}
function WorkshopData:UntilFixRingShow(data)
    local list = data:UltimateAttrList()
    local sec_list = data:AttrSection()
    local base = {}
    local utli = {}
    for k,v in pairs(list) do
        local base_vo = {}
        local ulti_vo = {}
        base_vo.attr_type = v.attr_type
        base_vo.cur = v.attr_value
        for i,j in pairs(sec_list) do
            if j.attr_type == v.attr_type then 
                base_vo.max = j.max
                if j.with_stren == true and v.attr_value ~= 0 then 
                    ulti_vo.attr_type = v.attr_type
                    ulti_vo.cur = v.ultimate_value
                    ulti_vo.max = j.stren_max
                    table.insert( utli,ulti_vo)
                end 
            end 
        end 

        if v.attr_value ~= 0 and base_vo.max ~= nil then 
            table.insert( base,base_vo)
        end 
    end 

    return {base = base,utli = utli}
end

function WorkshopData:CanUlti(item)
    -- 本来说好的 强化精工属性和普通属性对应，然后能配表配成那样，还说有可能只有一个属性
    -- 也就是说 我得用普通属性类型 从0-5强化属性中判断是否有一致的类型，得到普通属性中有index
    -- 可能还要max_value > 0 只有大于0的值才是强化属性
    -- 最后才拿到index去ultimate_attr_list 找到对应的属性值（我™
    if item.item_id <= 0 then
        return false
    end
    if not item.HasScore or not item:HasScore() or item:Color() < ItemColor.Purple or item:Color() > ItemColor.Red then
        return false
    end
    local item_cfg = item:Config()
    local attr_num = item.param.attr_num 
    local attr_list = item.param.attr_list
    local ulti_list = item.param.ultimate_attr_list
    -- LogError(attr_list, ulti_list)
    local cur, max
    local ulti_need = false
    for i = 1, attr_num do
        cur = 0
        max = 0
        for j = 0 , 5 do
            if attr_list[i].attr_type == item_cfg["added_attr_type" .. j] and item_cfg["max_value" .. j] > 0 then
                max = item_cfg["max_value"..j]
                cur = ulti_list[attr_list[i].index]
            end
        end
        if max > 0 and cur ~= max then
            ulti_need = true
        end
    end
    local attr_need = false
    local attr_seclist = item:AttrSection()
    local cur, max
    local attr_type
    for i = 1, attr_num do
        cur = attr_list[i].attr_value
        max = 0
        for _, v in ipairs(attr_seclist) do
            if attr_list[i].attr_type == v.attr_type then
                max = v.max
            end
        end
        if max > 0 and cur ~= max then
            attr_need = true
        end
    end

    return attr_need
end