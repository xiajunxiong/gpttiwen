-- 配饰道具基本信息
----------------------------AccrssoryItemBaseInfo----------------------------
AccrssoryItemBaseInfo = AccrssoryItemBaseInfo or DeclareMono("AccrssoryItemBaseInfo", UIWidgetBaseItem)
function AccrssoryItemBaseInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtDestiny, data.is_mine and Format(Language.Accrssory.DestinyAdd, data:GetDestinyAdd(true)) or "")
end

----------------------------AccrssoryItemBaseAttr----------------------------
AccrssoryItemBaseAttr = AccrssoryItemBaseAttr or DeclareMono("AccrssoryItemBaseAttr", ItemInfoViewItem)
function AccrssoryItemBaseAttr:OnEnable()
	ItemInfoViewItem.OnEnable(self)
    UH.SetText(self.Title, self.language.ItemTitles.AccrssoryItemBaseAttr)
    local base_attr_config = AccrssoryData.Instance:GetAccrssoryBaseAttr(self.item1.item_id)
	self.List:SetData(self:GetAttrData(base_attr_config))
end

function AccrssoryItemBaseAttr:GetAttrData(data)
    local add = self.item1.is_mine and self.item1:GetDestinyAdd() or 0
    local t = {}
    for k, v in pairs(data) do
        local t1 = {}
        -- 取消冒号
        t1.not_need = true
        t1.attr_name = v.attr_name
        -- 处理百分比属性加成的问题
        if DataHelper.IsPercent(v.attr_type) then
            -- 保留两位小数
            local add_num = string.format("%.2f", v.attr_value * add / 10)
            if tonumber(add_num) > 0 then
                add_num = add_num.."%"
                v.attr_percent_value = v.attr_percent_value .. Format(Language.Accrssory.DestinyAdd2, add_num)
            end
        else
            local add_num = math.floor(v.attr_percent_value * add)
            if add_num > 0 then
                v.attr_percent_value = v.attr_percent_value .. Format(Language.Accrssory.DestinyAdd2, add_num)
            end
        end
        t1.attr_value = v.attr_percent_value
        table.insert(t, t1)
    end
    return t
end

----------------------------AccrssoryItemStrengthAttr----------------------------
AccrssoryItemStrengthAttr = AccrssoryItemStrengthAttr or DeclareMono("AccrssoryItemStrengthAttr", ItemInfoViewItem)
function AccrssoryItemStrengthAttr:OnEnable()
	ItemInfoViewItem.OnEnable(self)
    UH.SetText(self.Title, self.language.ItemTitles.AccrssoryItemStrengthAttr)
    self.item = self.info.item_list[self.ShowType]
    self.List:SetData(self:GetAttrData())
end

function AccrssoryItemStrengthAttr:GetAttrData()
    local item_2 = self.info.item_list[2]
    local attr_list1 = self.item:StrengthAttr()
    local t = {}
    local attr_list2
    if self.ShowType == 1 and item_2 then
        attr_list2 = item_2:StrengthAttr()
    end
    for k, v in pairs(attr_list1) do
        -- 取消冒号
        v.not_need = true
        v.attr_value = v.attr_percent_value
        -- 如果有装备对比
        if attr_list2 and item_2:StrengthLevel() > 0 and attr_list2[k] then
            if v.attr_type == attr_list2[k].attr_type and v.attr_value ~= attr_list2[k].attr_value then
                v.show_arrow = true
                v.is_up = tonumber(v.attr_value) > tonumber(attr_list2[k].attr_value)
            end
        end
    end
    return attr_list1
end


----------------------------AccrssoryItemExtraAttr----------------------------
AccrssoryItemExtraAttr = AccrssoryItemExtraAttr or DeclareMono("AccrssoryItemExtraAttr", ItemInfoViewItem)
function AccrssoryItemExtraAttr:OnEnable()
    ItemInfoViewItem.OnEnable(self)
    self.item = self.info.item_list[self.ShowType]
    UH.SetText(self.Title, self.language.ItemTitles.AccrssoryItemExtraAttr)
	self.List:SetData(self:GetAttrData())
end

function AccrssoryItemExtraAttr:GetAttrData()
    local item_2 = self.info.item_list[2]
    local attr_list1, max_num1 = self.item:GetAttrList()
    local t = {}
    local attr_list2, max_num2
    if self.ShowType == 1 and item_2 then
        attr_list2, max_num2 = item_2:GetAttrList()
    end
    for i = 1, max_num1 do
        local t1 = {}
        -- 取消冒号
        t1.not_need = true
        if attr_list1[i].attr_value > 0 then
            local attr_struct = CommonStruct.AttrStruct(attr_list1[i].attr_type, attr_list1[i].attr_value)
            t1.attr_name = attr_struct.attr_name
            t1.attr_value = attr_struct.attr_percent_value
            -- 如果有装备对比
            if attr_list2 then
                if attr_list1[i].attr_type == attr_list2[i].attr_type and attr_list1[i].attr_value ~= attr_list2[i].attr_value then
                    t1.show_arrow = true
                    t1.is_up = attr_list1[i].attr_value > attr_list2[i].attr_value
                end
            end
        else
            t1.attr_name = Language.Accrssory.CanInherit
        end
        table.insert(t, t1)
    end
    return t
end

----------------------------AccrssoryItemSuit----------------------------
AccrssoryItemSuit = AccrssoryItemSuit or DeclareMono("AccrssoryItemSuit", ItemInfoViewItem)
function AccrssoryItemSuit:OnEnable()
	ItemInfoViewItem.OnEnable(self)
    UH.SetText(self.Title, self.language.ItemTitles.AccrssoryItemSuit)
    local suit_config = AccrssoryData.Instance:GetSuitColorPartConfig()[self.item1:SuitType()][self.item1:Color()]
    self.List:SetData(self:GetSuitData(suit_config))
    self:SetSkillTxt()
end

function AccrssoryItemSuit:GetSuitData(suit_config)
    local t = {}
    local equip_info = self.item1.is_mine and AccrssoryData.Instance.accrssory_equip_data.info or RoleInfoData.Instance:GetRoleAccrssory().info
    for i = 1, ACCRSSORY_MAX do
        local t1 = {}
        local name = self.item1:SuitName() .. Language.Accrssory.EquipType[i]
        if not TableIsEmpty(equip_info[i]) then
            if equip_info[i].item_id == suit_config[i] then
                t1.txt = Format(Language.Accrssory.EquipOn, name)
            else
                t1.txt = Format(Language.Accrssory.EquipOff, name)
            end
        else
            t1.txt = Format(Language.Accrssory.EquipOff, name)
        end
        table.insert(t, t1)
    end
    return t
end

function AccrssoryItemSuit:SetSkillTxt()
    local skill1_use, skill2_use = false, false
    local skill_info = AccrssoryData.Instance:GetSkillConfig()[self.item1:SuitType()][self.item1:Color()]
    local now_skill_info = self.item1.is_mine and AccrssoryData.Instance.accrssory_equip_data.skill or RoleInfoData.Instance:GetRoleAccrssory().skill
    for k, v in pairs(now_skill_info) do
        if not skill1_use and v.skill_id == skill_info.skill_1 and v.skill_level == skill_info.skill_level then
            skill1_use = true
        end
        if not skill2_use and v.skill_id == skill_info.skill_2 and v.skill_level == skill_info.skill_level then
            skill2_use = true
        end
    end
    local skill1_config = Cfg.AccrssorySkillDesc(skill_info.skill_1, skill_info.skill_level)
    local skill2_config = Cfg.AccrssorySkillDesc(skill_info.skill_2, skill_info.skill_level)
    UH.SetText(self.TxtSkill1, Format(skill1_use and Language.Accrssory.SkillOn or Language.Accrssory.SkillOff, skill1_config.skill_name, skill1_config.skill_level))
    UH.SetText(self.TxtSkill2, Format(skill2_use and Language.Accrssory.SkillOn or Language.Accrssory.SkillOff, skill2_config.skill_name, skill2_config.skill_level))
end