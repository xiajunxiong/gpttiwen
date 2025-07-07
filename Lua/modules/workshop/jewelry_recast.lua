JewelryRecast = JewelryRecast or DeclareView("JewelryRecast", "workshop/jewelry_recast")
function JewelryRecast:JewelryRecast()
    self.lang = Language.Workshop
end
function JewelryRecast:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Title2, Vector2.New(1357, 567))
end

JewelryRecastPanel = JewelryRecastPanel or DeclareMono("JewelryRecastPanel", UIWFlushPanel)
function JewelryRecastPanel:JewelryRecastPanel()
    self.cur_select = 1
    self.select_index = -1
    self.data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.select_data = SmartData.New()
    self.data_cares = {
        {data = self.data.select_jewelry, func = self.FlushResultShow},
        {data = self.select_data, func = self.FlushResultShow}
    }
    self.lang = Language.Workshop
    self.comm = Language.Common
end
function JewelryRecastPanel:onFlush()
    -- 判断显示什么按钮
end
-- 新灵饰的显示
function JewelryRecastPanel:FlushResultShow()
    local select_list = self.data:GetSelectJewelry()
    self.is_full = table.nums(select_list:Val()) == 3
    UH.SpriteName(self.SpTitle, self.is_full and "_Loclingshi2" or "_Loclingshi1")
    if self.is_full then
        for i, v in ipairs(select_list) do
            self:ShowJewelrySet(v, i)
        end
        self.select_data:Set({})
    end
    if not self.is_full and table.nums(self.select_data) == 0 then
        for i = 1, 3 do
            self:HideJewelrySet(i)
        end
        self:OnClickSelect()
    end
    self.BtnOk:SetActive(self.is_full)
    self.BtnCreate:SetActive(not self.is_full)
    -- 已存在需要选择的情况
    if table.nums(self.select_data) == 2 then
        -- 名字
        local data = {name = self.lang.NewJewelry}
        local item1 = self.select_data[1]
        local item2 = self.select_data[2]
        data.type_name = item1:TypeName() == item2:TypeName() and item1:TypeName() or string.format("%s/%s", item1:TypeName(), item2:TypeName())
        --属性
        if item1.param.attr_pair.attr_type == item2.param.attr_pair.attr_type then
            data.attr_type = {item1.param.attr_pair.attr_type}
        else
            if item1:LimitLv() == item2:LimitLv() then
                data.attr_type = {item1.param.attr_pair.attr_type, item2.param.attr_pair.attr_type}
            else
                data.attr_type = item1:LimitLv() < item2:LimitLv() and {item1.param.attr_pair.attr_type} or {item2.param.attr_pair.attr_type}
            end
        end
        --装备等级
        data.equip_level = (item1:EquipLevel() + item2:EquipLevel()) / 2
        if data.equip_level % 1 ~= 0 then
            data.equip_level = string.format("%d/%d", data.equip_level - 0.5, data.equip_level + 0.5)
        end
        data.limit_level = math.floor((item1:LimitLv() + item2:LimitLv()) / 2)
        if data.limit_level % 10 ~= 0 then
            data.limit_level = string.format("%d/%d", data.limit_level - 5, data.limit_level + 5)
        end
        --前缀
        local suit1 = Jewelry.GetSuitInfo(item1.param.lyrics_id)
        local suit2 = Jewelry.GetSuitInfo(item2.param.lyrics_id)
        data.suit_attr = ""
        if suit1.suit_type == suit2.suit_type then
            data.suit_name = {suit1.name}

            for i = 1, 3 do
                local str = string.format("%s+?", self.comm.AttrList[suit1["param_" .. i]])
                data.suit_attr = data.suit_attr .. string.joint(i, str, "  ")
            end
        else
            if suit1.level == suit2.level then
                data.suit_name = {suit1.name, suit2.name}
                for i = 1, 3 do
                    local str = string.format("%s/%s+?", self.comm.AttrList[suit1["param_" .. i]], self.comm.AttrList[suit2["param_" .. i]])
                    data.suit_attr = data.suit_attr .. string.joint(i, str, "  ")
                end
            else
                data.suit_name = suit1.level < suit2.level and {suit1.name} or {suit2.name}
                local suit = suit1.level < suit2.level and suit1 or suit2
                for i = 1, 3 do
                    local str = string.format("%s+?", self.comm.AttrList[suit["param_" .. i]])
                    data.suit_attr = data.suit_attr .. string.joint(i, str, "  ")
                end
            end
        end

        --颜色
        local color_list
        data.color = (item1:Color() + item2:Color()) / 2
        if data.color % 1 ~= 0 then
            color_list = {data.color - 0.5, data.color + 0.5}
            data.color = string.format("<color=#%s>%s</color>/<color=#%s>%s</color>", QualityColorStr[data.color - 0.5], Language.Common.ItemColor[data.color - 0.5], QualityColorStr[data.color + 0.5], Language.Common.ItemColor[data.color + 0.5])
        else
            color_list = {data.color}
            data.color = string.format("<color=#%s>%s</color>", QualityColorStr[data.color], Language.Common.ItemColor[data.color])
        end

        -- 灵气
        if item1.param.anima_val == INT_MAX or item2.param.anima_val == INT_MAX then
            data.anima_val = {item1.param.anima_val > item2.param.anima_val and item2.param.anima_val or item1.param.anima_val}
        else
            data.anima_val = item1.param.anima_val > item2.param.anima_val and item2.param.anima_val or item1.param.anima_val
        end
        -- 传说属性 只有橙色有传说属性
        if suit1.legend_attr > 0 and suit2.legend_attr > 0 then
            data.legend_attr = string.format(self.lang.LegendStr[1], self.lang.LegendAttr[suit1.legend_attr], self.lang.LegendAttr[suit2.legend_attr])
        else
            data.legend_attr = self.lang.LegendAttr[0]
            if suit1.legend_attr > 0 then
                data.legend_attr = string.format(self.lang.LegendStr[2], self.lang.LegendAttr[suit1.legend_attr])
            end
            if suit2.legend_attr > 0 then
                data.legend_attr = string.format(self.lang.LegendStr[2], self.lang.LegendAttr[suit2.legend_attr])
            end
        end
        self.SetList[3]:SetData(data, false)
        self.SetObjList[3]:SetActive(true)
        self.ItemList[3]:SetActive(false)
    end
end
-- 选择重铸物
function JewelryRecastPanel:OnClickItem(data)
    self.SelectParent:SetActive(false)
    self.ItemList[self.cur_select]:SetActive(false)
    self.SetObjList[self.cur_select]:SetActive(true)
    self.SetList[self.cur_select]:SetData(data, true)
    self.select_data[self.cur_select] = data
end
function JewelryRecastPanel:ShowJewelrySet(data, index)
    self.ItemList[index]:SetActive(false)
    self.SetObjList[index]:SetActive(true)
    self.SetList[index]:SetData(data, true)
end
function JewelryRecastPanel:HideJewelrySet(index)
    self.ItemList[index]:SetActive(true)
    self.SetObjList[index]:SetActive(false)
end
function JewelryRecastPanel:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[1].desc})
end

function JewelryRecastPanel:OnClickSetItem(index)
    self.cur_select = index
    self.SelectParent:SetActive(true)
    -- 背包里的灵饰列表
    local list = {}
    -- 排除已选择的灵饰
    if next(self.select_data) then
        local data = BagData.Instance:GetGridByType({ItemType.Jewelry})
        for k, jewelry in pairs(data) do
            for k2, v in pairs(self.select_data) do
                if jewelry ~= v and jewelry.param.creater_name ~= "" then
                    table.insert(list, jewelry)
                end
            end
        end
    else
        local tempList = BagData.Instance:GetGridByType({ItemType.Jewelry})
        for k, jewelry in pairs(tempList) do
            if jewelry.param.creater_name ~= "" then
                table.insert(list, jewelry)
            end
        end
    end
    self.JewelryList:SetData(list)
end
function JewelryRecastPanel:OnClickCreate()
    -- 格子1 格子2
    -- Debuger.LogError(self.select_data[1].index .. "|" .. self.select_data[2].index)
    if table.nums(self.select_data) == 2 then
        self.ctrl:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_JEWE_REFORGE, self.select_data[1].index, self.select_data[2].index, 0)
        return
    end
    PublicPopupCtrl.Instance:Center(ErrorText[34])
end
function JewelryRecastPanel:OnClickOk()
    if self.select_index ~= -1 then
        self.ctrl:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_GAIN_REFORGE, self.select_index, 0, 0)
        self.select_data:Set({})
        return
    end
    PublicPopupCtrl.Instance:Center(ErrorText[35])
end
function JewelryRecastPanel:OnClickSelect(data, index)
    if self.is_full then
        for i = 1, self.SelectList:Length() do
            self.SelectList[i]:SetActive(i == index)
        end
        self.select_index = index - 1
        return
    else
        for i = 1, self.SelectList:Length() do
            self.SelectList[i]:SetActive(false)
        end
    end
    self.select_index = -1
end
function JewelryRecastPanel:OnClickClose()
    self.SelectParent:SetActive(false)
end

JewelryRecastItem = JewelryRecastItem or DeclareMono("JewelryRecastItem", UIWidgetBaseItem)
function JewelryRecastItem:JewelryRecastItem()
    self.lang = Language.Workshop
    self.comm = Language.Common
end
function JewelryRecastItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    UH.SetText(self.Name, data:NameId())
    UH.Color(self.Name, QualityColor[data:Color()])
    UH.SetText(self.Attr, string.format("%s:%d", self.comm.AttrList[data.param.attr_pair.attr_type], data.param.attr_pair.attr_value))
    --string.format(self.lang.CLevel, data:LimitLv()))
    UH.SetText(self.Level, string.format("Lv.%d", data:EquipLevel()))
    UH.SetText(self.MType, string.format(self.lang.MType, data:TypeName()))
    local param = data.param
    UH.SetText(self.Value, param.anima_val == INT_MAX and string.format(self.lang.Anima, self.lang.WuXian) or string.format(self.lang.Anima, param.anima_val))
end

JewelryRecastShow = JewelryRecastShow or DeclareMono("JewelryRecastShow")
function JewelryRecastShow:JewelryRecastShow()
    self.lang = Language.Workshop
    self.comm = Language.Common
end
function JewelryRecastShow:SetData(data, flag)
    self.data = data
    self.cell:SetActive(not flag)
    self.cell2:SetActive(flag)
    if flag then
        self.SubCell:SetData(data)
        UH.Color(self.Name, QualityColor[data:Color()])
        UH.SetText(self.Name, data:NameId())
        UH.SetText(self.CLevel, string.format(self.lang.CLevel, data:LimitLv()))
        UH.SetText(self.Level, string.format("Lv.%d", data:EquipLevel()))
        UH.SetText(self.MType, string.format(self.lang.MType, data:TypeName()))
        local param = data.param
        UH.SetText(self.BaseAttr, string.format("%s    %d", self.comm.AttrList[param.attr_pair.attr_type], param.attr_pair.attr_value))
        -- 获取灵饰的套装属性
        local info = data:SuitInfo()
        local suit = ""
        if info then
            for i = 1, 3 do
                local str = string.format("%s+%d", self.comm.AttrList[info["param_" .. i]], info["attr_" .. i])
                suit = suit .. string.joint(i, str, "  ")
            end
        end
        UH.SetText(self.SuitAttr, suit)
        if info.legend_attr == 0 then
            UH.SetText(self.LegendAttr, self.lang.LegendAttr[info.legend_attr])
        else
            UH.SetText(self.LegendAttr, string.format(self.lang.Legend, self.lang.LegendAttr[info.legend_attr], "+" .. info.legend_percent .. "%"))
        end
        UH.SetText(self.AnimaValue, param.anima_val == INT_MAX and string.format(self.lang.Anima, self.lang.WuXian) or string.format(self.lang.Anima, param.anima_val))
        if self.LblColor then
            UH.SetText(self.LblColor, "")
        end
    else
        local suit = ""
        for k, v in pairs(data.suit_name) do
            suit = suit .. string.joint(k, v, "/")
        end
        UH.SetText(self.Level, string.format("Lv.%s", data.equip_level))
        UH.SetText(self.CLevel, string.format(self.lang.CLevel, data.limit_level))
        UH.SetText(self.Name, string.format("%s·%s", suit, data.name))
        UH.SetText(self.MType, string.format(self.lang.MType, data.type_name))
        if type(data.anima_val) == "table" then
            if data.anima_val[1] == INT_MAX then
                UH.SetText(self.AnimaValue, string.format(self.lang.Anima, self.lang.WuXian))
            else
                UH.SetText(self.AnimaValue, string.format(self.lang.Anima, string.format(self.lang.WuXian2, data.anima_val[1])))
            end
        else
            UH.SetText(self.AnimaValue, string.format(self.lang.Anima, data.anima_val))
        end
        local attr = ""
        for k, v in pairs(data.attr_type) do
            local str = string.format("%s    %s", self.comm.AttrList[v], "?")
            attr = attr .. string.joint(k, str, "/")
        end
        UH.SetText(self.BaseAttr, attr)
        if self.LblColor then
            UH.SetText(self.LblColor, string.format(self.lang.ColorStr, data.color))
        end
        UH.SetText(self.LegendAttr, data.legend_attr)
        UH.SetText(self.SuitAttr, data.suit_attr)
    end
end
function JewelryRecastShow:GetData()
    return self.data
end
