PetStampItemBaseInfo = PetStampItemBaseInfo or DeclareMono("PetStampItemBaseInfo", UIWidgetBaseItem)
function PetStampItemBaseInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushInfo()
end

function PetStampItemBaseInfo:FlushInfo()
    local data = self.data
    -- 不显示等级显示品质了
    UH.SetText(self.Level, string.format(Language.PetStamp.Tip6, data:QuaLevel()))
    local pet_index = self.data:PetIndex()
    --local use_flag = pet_index ~= nil and Language.PetStamp.ItemTip1 or ""
    UH.SetText(self.UseFlag, "")

    local index = data:Index()
    local stamp = PetStampData.Instance:GetPetStampByIndex(index)
    -- 在背包才显示锁
    if stamp and stamp.item_id > 0 and ViewMgr:IsOpen(PetView) then
        UH.SpriteName(self.LockImg, data:IsLock() and "SuoZhu" or "SuoDaKai")
    else
        UH.SpriteName(self.LockImg, nil)
    end
end

function PetStampItemBaseInfo:OnLockClick()
    local pet_index = self.data:PetIndex()
    local p1 = self.data:Index()
    if self.p2 == nil then
        self.p2 = self.data:IsLock() and 0 or 1
    end
    PetCtrl.SendPetOp(PET_OP_TYPE.PetStampLock, p1, self.p2)
    UH.SpriteName(self.LockImg, self.p2 == 1 and "SuoZhu" or "SuoDaKai")
    self.p2 = self.p2 == 1 and 0 or 1
end

function PetStampItemBaseInfo:OnClickPressDown()
    ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end

PetStampItemInfo = PetStampItemInfo or DeclareMono("PetStampItemInfo", ItemInfoViewItem)

function PetStampItemInfo:OnEnable()
    ItemInfoViewItem.OnEnable(self)
    UH.SetText(self.Title, Language.PetWeapon.ItemAttriTitle)
    self.item = self.info.item_list[self.ShowType]
    self:FlushPanel()
end

function PetStampItemInfo:FlushPanel()
    local attri_list = TableCopy(self.item:AttriListInfo(), 1) or {}
    if self.item:Swallowed() then
        table.insert(attri_list, self.item:SwallowAttr())
    end
    --LogError(self.item:Name(), self.item.info)
    local item_2 = self.info.item_list[2]
    local has_attri = next(attri_list) ~= nil
    self.NotAttriTip:SetObjActive(not has_attri)
    self.AttriList:SetObjActive(has_attri)
    if not has_attri then
        UH.SetText(self.NotAttriTip, Language.PetStamp.ItemTip2)
    else
         -- 是右边的，则比较一下左边的
        if self.ShowType == 1 and item_2 then
            local new_attri_list = TableCopy(attri_list, 1)
            local item_2_attri_list = TableCopy(item_2:AttriListInfo(), 1)
            if item_2:Swallowed() then
                table.insert(attri_list, item_2:SwallowAttr())
            end
            for i,v in pairs(new_attri_list) do
                local v2 = item_2_attri_list[i]
                if v2 and v.attri_type == v2.attri_type and v.attri_value ~= v2.attri_value then
                    v.show_arrow = true
                    v.is_up = v.attri_value > v2.attri_value
                end
            end
            self.AttriList:SetData(new_attri_list)
        else
            self.AttriList:SetData(attri_list)
        end
    end

    local suit_desc = self.item:SuitDesc()

    UH.SetText(self.EffectDesc, suit_desc)
end

PetStampItemInfoAttri = PetStampItemInfoAttri or DeclareMono("PetStampItemInfoAttri", UIWidgetBaseItem)
function PetStampItemInfoAttri:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Arrow:SetObjActive(data.show_arrow ~= nil)
	if data.show_arrow then 
	    UH.SpriteName(self.Arrow, data.is_up and "up_s" or "down_s")		
    end
    local color = data.is_main ~= nil and COLORSTR.Red12 or COLORSTR.Yellow7
    local name
    --TitleCustomColor 是不同的界面上神印属性 名称 需要用的颜色，类型是string
    local name_color = self.TitleCustomColor ~= nil and COLORSTR[self.TitleCustomColor] or color
    --ValueCustomColor 是不同的界面上神印属性 值 需要用的颜色，类型是string
    local value_color = self.ValueCustomColor ~= nil and COLORSTR[self.ValueCustomColor] or color
    if data.is_main == nil and data.attri_index == nil and self.SpecialColor then
        name_color = COLORSTR[self.SpecialColor]
        value_color = COLORSTR[self.SpecialColor]
    end
    local attri_value_str = ""
    if not data.up_add_attri then
        name = Language.Common.AttrList[data.attri_type] or ""
        if data.is_main then
            name = ColorStr(string.format(Language.PetStamp.Attri1,name), name_color)
        else
            name = ColorStr(string.format(Language.PetStamp.Attri2,name), name_color)
        end

        if DataHelper.IsPercent(data.attri_type) then
            attri_value_str = string.format("+%s%%",data.attri_value/10)
            attri_value_str = ColorStr(attri_value_str, value_color)
        else
            attri_value_str = string.format("+%s",data.attri_value)
            attri_value_str = ColorStr(attri_value_str, value_color)
        end

        if data.is_main then
            local level_color = self.ValueCustomColor ~= nil and COLORSTR[self.ValueCustomColor] or COLORSTR.Red12
            attri_value_str = string.format(Language.PetStamp.Attri4, attri_value_str, level_color, data.is_main)
        end
    else
        name = string.format(Language.PetStamp.Attri3, data.up_add_attri)
        name = ColorStr(name, COLORSTR.Green3)
        name = ColorStr(string.format(Language.PetStamp.Attri2,name), color)
    end

    UH.SetText(self.AttriValue,attri_value_str)
    UH.SetText(self.AttriName,name)
end