DivineCostumeResultView = DivineCostumeResultView or DeclareView("DivineCostumeResultView", "divine_costume/divine_costume_result")
VIEW_DECLARE_MASK(DivineCostumeResultView, ViewMask.BgBlockClose)
function DivineCostumeResultView:DivineCostumeResultView()
    self.Data = DivineCostumeData.Instance
    self.language = Language.DivineCostume
end

function DivineCostumeResultView:LoadCallback(param_t)
    local config = {}
    if param_t.equip_type > 4 then
        config = Config.godequip_up_auto.godjewelry_strengthen
	else
        config = Config.godequip_up_auto.godequip_strengthen
    end
    local cfg = {}
    local color = param_t.is_active and 7 or 8
    local prof = RoleData.Instance:GetRoleProfType()
    for k, v in pairs(config) do
        if v.equip_type == param_t.equip_type and v.level == param_t.level and v.limit_prof == prof and v.color == color then
            cfg = v
            break
        end
    end
    local path = DrawerHelper.GetWeaponPath(cfg.model_id)
    self.ui_obj = UIChDrawer.New()
    self.ui_obj:SetMainSolo(path)
    self.Model:SetShowData({ui_obj = self.ui_obj, view = self})
    UH.SetText(self.TxtGrade, Format(self.language.Grade, param_t.level))
    UH.SetText(self.TxtName, Item.GetQuaName(cfg.id))
    UH.SpriteName(self.ImgTitle, param_t.is_active and "_LocShenZhuangJiHuo" or "_LocShenZhuangHuaShen")
    UH.SpriteName(self.ImgGrade, param_t.is_active and "JieDi1" or "JieDi2")
    if TableIsEmpty(cfg) then
        return
    end
    self.ObjTitle2:SetActive(not param_t.is_active)
    local left_attr = self:GetAttrList(cfg)
    local right_attr = self:GetAttrList(cfg)
    if param_t.is_active then
        -- 激活的话，右边的属性为：1阶神装属性 + 41阶化神属性 - 41阶神装属性（干掉）
        -- local cfg1, cfg41_7, cfg41_8
        -- for k, v in pairs(config) do
        --     if v.equip_type == param_t.equip_type and v.level == param_t.level and v.limit_prof == prof and v.color == 7 then
        --         cfg1 = v
        --     elseif v.equip_type == param_t.equip_type and v.level == 41 and v.limit_prof == prof and v.color == 7 then
        --         cfg41_7 = v
        --     elseif v.equip_type == param_t.equip_type and v.level == 41 and v.limit_prof == prof and v.color == 8 then
        --         cfg41_8 = v
        --     end
        --     if cfg1 and cfg41_7 and cfg41_8 then
        --         break
        --     end
        -- end
        -- local cfg1_attr = self:GetAttrList(cfg1)
        -- local cfg41_7_attr = self:GetAttrList(cfg41_7)
        -- local cfg41_8_attr = self:GetAttrList(cfg41_8)
        -- right_attr = TableCopy(cfg41_8_attr, 2)
        -- for i, v in ipairs(right_attr) do
        --     v.attr_value = cfg1_attr[i].attr_value + v.attr_value - cfg41_7_attr[i].attr_value
        --     v.attr_percent_value = DataHelper.Percent(v.attr_type, v.attr_value)
        -- end

        -- 激活时左边为0属性，右边为1阶的
        for k, v in pairs(left_attr) do
            v.attr_value = 0
            v.attr_percent_value = DataHelper.Percent(v.attr_type, v.attr_value)
        end
    else
        local cfg_l = {}
        for k, v in pairs(config) do
            if v.equip_type == param_t.equip_type and v.level == param_t.level and v.limit_prof == prof and v.color == 7 then
                cfg_l = v
                break
            end
        end
        left_attr = self:GetAttrList(cfg_l)
    end
    local list_data = {}
    for i, v in ipairs(left_attr) do
        local t = {}
        t.left = v
        t.right = right_attr[i]
        table.insert(list_data, t)
    end
    self.List:SetData(list_data)
end

function DivineCostumeResultView:GetAttrList(cfg)
    local attr = {}
    local index = 1
    while cfg["attr_add_"..index] and cfg["attr_add_"..index] > 0 do
        table.insert(attr, CommonStruct.AttrStruct(cfg["attr_type_"..index], cfg["attr_add_"..index]))
        index = index + 1
    end
    return attr
end

function DivineCostumeResultView:CloseCallback()
	Delete(self, "ui_obj")
end

---------------DivineCostumeResultItem---------------
DivineCostumeResultItem = DivineCostumeResultItem or DeclareMono("DivineCostumeResultItem", UIWidgetBaseItem)
function DivineCostumeResultItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtAttrName1, data.left.attr_name .. ": ")
    UH.SetText(self.TxtAttrValue1, data.left.attr_percent_value)
    UH.SetText(self.TxtAttrName2, data.right.attr_name .. ": ")
    UH.SetText(self.TxtAttrValue2, data.right.attr_percent_value)
end 