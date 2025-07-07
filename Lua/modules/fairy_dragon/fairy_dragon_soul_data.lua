FairyDragonSoulData = FairyDragonSoulData or BaseClass()
-- 龙魂数据处理
function FairyDragonSoulData:__init()
    if FairyDragonSoulData.Instance ~= nil then
        Debuger.LogError("[FairyDragonSoulData] attempt to create singleton twice!")
        return
    end
    FairyDragonSoulData.Instance = self
    self.dragon = FairyDragon.Instance
    self:InitCfg()
  
    self.soul_item_list = SmartData.New()       -- 龙魂道具列表

    self.soul_bag_sel = nil    -- 背包选择的龙魂

    self.soul_resolve_sel_list = {}

    self.soul_up_item = nil     -- 需要升级的龙魂

    self.sel_stuff_list = SmartData.New()    -- 选择的经验材料

    self.sel_bag_type = FairyDragonCfg.SOUL_TYPE.All

    self.sel_stuff_soul = SmartData.New()   -- 选择的龙魂材料
end

function FairyDragonSoulData:ClearData()

end

function FairyDragonSoulData:InitCfg()
    self.cfg = Config.god_dragon_auto
    self.other_cfg = Config.god_dragon_auto.other[1]

    -- 龙魂配置
    Cfg.Register("FairyDragonSoulCfg",Cfg.RealTimeSearch(self.cfg.dragon_soul,"dragon_soul_id"))
    -- 龙魂属性组配置
    Cfg.Register("FairyDragonSoulAttriGroupCfg",Cfg.RealTimeSearch3Key(self.cfg.primary_group, "primary_group", "prof", "soul_level"))

    --龙魂属性组
    local primary_group_cfg = DataHelper.ConvertGroup(self.cfg.primary_group, "primary_group")
    self.base_group_attri_cfg = {}
    for i,v in pairs(primary_group_cfg) do
        self.base_group_attri_cfg[i] = DataHelper.ConvertGroup(v, "prof")
    end

    -- 孔位配置
    self.hole_cfg = TableCopy(self.cfg.dragon_soul_unlock)
    table.sort(self.hole_cfg, function (a,b)
        return a.client_order < b.client_order
    end)
end

function FairyDragonSoulData:SetItemList(item_list)
    self.soul_item_list:Set(item_list)
end
function FairyDragonSoulData:GetItemList(soul_type, not_use)
    if not soul_type then
        return self.soul_item_list
    else
        local list = {}
        for i,v in pairs(self.soul_item_list) do
            if v.item_id ~= 0 then
                if soul_type == FairyDragonCfg.SOUL_TYPE.All or v:SoulType() == soul_type then
                    if not not_use then
                        table.insert(list, v)
                    else
                        if not v:IsInUse() then
                            table.insert(list, v)
                        end
                    end
                end
            end
        end
        return list
    end
end

function FairyDragonSoulData:ItemListCount()
    local count = 0
    for i,v in pairs(self.soul_item_list) do
        if v.item_id ~= 0 then
            count = count + 1
        end
    end
    return count
end

function FairyDragonSoulData:GetBagItemList(soul_type)
    soul_type = soul_type or FairyDragonCfg.SOUL_TYPE.All
    local list = self:GetItemList(soul_type)
    table.sort(list, function (a,b)
        local a_num = a:Color()
        local b_num = b:Color()
        if a:IsInUse() then
            a_num = a_num + 10000
        end
        if b:IsInUse() then
            b_num = b_num + 10000
        end
        return a_num > b_num
    end)
    return list
end

function FairyDragonSoulData:GetResolveItemList()
    local list = self:GetItemList(FairyDragonCfg.SOUL_TYPE.All, true)
    table.sort(list, function (a,b)
        local a_num = a:Color()
        local b_num = b:Color()
        if a:IsInUse() then
            a_num = a_num + 10000
        end
        if b:IsInUse() then
            b_num = b_num + 10000
        end
        return a_num > b_num
    end)
    return list
end

function FairyDragonSoulData:ItemChange(protocol)
    local item = protocol.item
    local reason = protocol.reason
    local put_reason = protocol.put_reason
    local old_item = self:GetItem(item.index)

    self:UpdateItemList(item)
    if item.item_id == 0 then
        return
    end

    if reason == 1 then
        if not BlockCenter[put_reason] then
            PublicPopupCtrl.Instance:CenterI({item_id = item.item_id, num = item.num,
                color_str = item:ColorStr(),item = item,icon_id = item:IconId()})
            ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = num}}
        end
    elseif reason == 0 then
        if old_item then
            if item:Level() > old_item:Level() then
                local old_attri = old_item:AllAttriList()
                local new_attri = item:AllAttriList()
                local attri_list = FairyDragonData.Instance:FormatAttriListCpmpare(old_attri, new_attri)

                local old_skill = old_item:Skill()
                local new_skill = item:Skill()
                local param_t = {
                    last_level = old_item:Level(),
                    cur_level = item:Level(),
                    level_text = Language.FairyDragon.Level,
                    attri_list = attri_list,
                    is_show_skill = new_skill.skill_id ~= old_skill.skill_id,
                    --skill_item = new_skill,
                    is_show_skill = false,
                    item = item,
                }
                if new_skill then
                    param_t.cur_skill_desc = new_skill:Desc()
                    if old_skill then
                        param_t.last_skill_desc = old_skill:Desc()
                    end
                end
                ViewMgr:OpenView(FairyDragonUpTip,param_t)
            end
        end
    end
end


-- 插入/变化单个龙魂
function FairyDragonSoulData:UpdateItemList(soul_item)
    if soul_item.item_id == 0 then
        self:RemoveItem(soul_item.index)
        return
    end
    self.soul_item_list[soul_item.index] = soul_item
end

-- 移除龙魂
function FairyDragonSoulData:RemoveItem(bag_index)
    self.soul_item_list[bag_index] = nil
end

function FairyDragonSoulData:GetItem(bag_index)
    return self.soul_item_list[bag_index]
end

function FairyDragonSoulData:GetHoleCfg()
    return self.hole_cfg
end

-- 获取孔位上的龙魂
function FairyDragonSoulData:GetHoleSoulByType(soul_type)
    local info = FairyDragon.Instance:GetSoulInfo(soul_type)
    if TableIsEmpty(info) then
        return
    end
    if info.index >= 0 then
        return self:GetItem(info.index)
    end
end

-- 获取解锁配置
function FairyDragonSoulData:GetUnlockCfg(hole_index)
    local cfg = self:GetHoleCfg()
    for i,v in pairs(cfg) do
        if v.dragon_soul_hole == hole_index then
            return v
        end
    end
end

-- 获取龙魂基础配置
function FairyDragonSoulData:GetBaseCfg(item_id)
    local cfg = Cfg.FairyDragonSoulCfg(item_id)
    return cfg
end

-- 获取龙魂基础属性配置
function FairyDragonSoulData:GetBaseAttriCfg(group_id, level)
    local prof = RoleData.Instance:ProfType()
    local cfg = Cfg.FairyDragonSoulAttriGroupCfg(group_id, prof, level)
    return cfg
end

-- 获取龙魂基础属性
function FairyDragonSoulData:GetBaseAttri(group_id, level)
    local cfg = self:GetBaseAttriCfg(group_id, level)
    if not cfg then
        return {}
    end
    local attri_list = {}
    for i = 1, cfg.affix_num do
        local attri = {}
        attri.attri_type = cfg["attri_type_" .. i]
        attri.attri_value = cfg["attri_num_" .. i] or 0
        table.insert(attri_list, attri)
    end
    return attri_list
end

-- 获取龙魂基础属性+预览下级属性
function FairyDragonSoulData:GetBaseAttriAndNext(group_id, level, next_level)
    next_level = next_level or level
    local cfg = self:GetBaseAttriCfg(group_id, level)
    local next_cfg = self:GetBaseAttriCfg(group_id, next_level)
    local attri_list = {}
    for i = 1, cfg.affix_num do
        local attri = {}
        attri.attri_type = next_cfg["attri_type_" .. i]
        attri.cur_attri_value = cfg["attri_num_" .. i] or 0
        attri.next_attri_value = next_cfg["attri_num_" .. i] or 0
        table.insert(attri_list, attri)
    end
    return attri_list
end

-- 获取龙魂孔解锁提示
function FairyDragonSoulData:GetUnlockTip(cfg, is_not_n)
    local qua = cfg.dragon_soul_quality
    local qua_str = Language.FairyDragon.ColorLevel[qua] or ""
    qua_str = ColorStr(qua_str, QualityColorStr[qua])
    local level_str = ColorStr(cfg.dragon_soul_level .. Language.FairyDragon.Level, COLORSTR.Red8)
    if is_not_n then
        return string.format(Language.FairyDragon.Text11, qua_str, level_str)
    end
    return string.format(Language.FairyDragon.Text12, qua_str, level_str)
end


function FairyDragonSoulData:SetBagSel(data)
    self.soul_bag_sel = data
end
function FairyDragonSoulData:GetBagSel()
    return self.soul_bag_sel
end

function FairyDragonSoulData:SetResolveSel(index, value)
    if not value then
        value = nil
    else
        value = true
    end
    self.soul_resolve_sel_list[index] = value
end

function FairyDragonSoulData:GetResolveSel(index)
    return self.soul_resolve_sel_list[index]
end

function FairyDragonSoulData:ClearResolveSel()
    self.soul_resolve_sel_list = {}
end

-- =================== 升级相关 ====================

function FairyDragonSoulData:SetUpSoul(item)
    self.soul_up_item = item
end
function FairyDragonSoulData:GetUpSoul()
    if TableIsEmpty(self.soul_up_item) then
        return
    end
    local index = self.soul_up_item.index
    local item = self:GetItem(index)
    return item
end


-- 升级材料配置
function FairyDragonSoulData:StuffCfg()
    if not self.stuff_cfg then
        self.stuff_cfg = {}
        for i,v in pairs(self.cfg.item_type) do
            if v.item_type == 1 then
                self.stuff_cfg[v.item_id] = v.exp_num
            end
        end
    end
    return self.stuff_cfg
end

--添加升级材料 减去就 -1   结果 1 添加成功，-1经验满了，0 数量不足, -2其它
function FairyDragonSoulData:AddExpStuff(item_id, num, soul_item)
    num = num or 0
    local cur_num = self.sel_stuff_list[item_id] or 0
    if num > 0 then
        soul_item = soul_item or self:GetUpSoul()
        if TableIsEmpty(soul_item) then
            return -2
        end
        local has_num = Item.GetNum(item_id)
        if has_num <= cur_num then
            return 0
        end
        local max_need_exp = soul_item:FullExp()
        local stuff_exp = self:GetSelStuffExp()
        if stuff_exp >= max_need_exp then
            return -1
        end
        self.sel_stuff_list[item_id] = cur_num + num
        return 1
    elseif num == 0 then
        self.sel_stuff_list[item_id] = nil
        return 1
    elseif cur_num > 0 then
        self.sel_stuff_list[item_id] = cur_num + num
        return 1
    end
    return -2
end

function FairyDragonSoulData:GetSelExpStuffNum(item_id)
    return self.sel_stuff_list[item_id] or 0
end

function FairyDragonSoulData:GetSelStuff()
    return self.sel_stuff_list
end

-- 清空
function FairyDragonSoulData:ClearSelStuff()
    self.sel_stuff_list:Set({})
end

-- 获取单个材料值多少经验
function FairyDragonSoulData:GetItemExp(item_id)
    local cfg = self:StuffCfg()
    return cfg[item_id] or 0
end

-- 选中的材料值多少经验
function FairyDragonSoulData:GetSelStuffExp()
    local exp = 0
    for i,v in pairs(self.sel_stuff_list) do
        local item_exp = self:GetItemExp(i)
        exp = exp + item_exp * v
    end
    return exp
end


-- 预览等级
function FairyDragonSoulData:GetPreviewLevel(soul_item, add_exp)
    local prof = RoleData.Instance:ProfType()
    local exp = soul_item:Exp() + add_exp
    local group_id = soul_item:GetGroupId()
    local group_cfg = self.base_group_attri_cfg[group_id][prof]
    local level = soul_item:Level()
    for i = level + 1, #group_cfg - 1 do
        local cfg = group_cfg[i]
        if exp < cfg.exe_up then
            return cfg.soul_level
        else
            exp = exp - cfg.exe_up
        end
    end
    return group_cfg[#group_cfg].soul_level
end


function FairyDragonSoulData:SetBagSelType(soul_type)
    self.sel_bag_type = soul_type
end
function FairyDragonSoulData:GetBagSelType()
    return self.sel_bag_type or FairyDragonCfg.SOUL_TYPE.All
end



function FairyDragonSoulData:AddSoulStuff(item)
    if self.sel_stuff_soul[item] then
        self.sel_stuff_soul[item] = nil
    else
        self.sel_stuff_soul[item] = 1
    end
end

function FairyDragonSoulData:ClearSoulStuff()
    self.sel_stuff_soul:Set({})
end