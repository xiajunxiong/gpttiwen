PetBagdeSelect = PetBagdeSelect or DeclareView("PetBagdeSelect", "pet/pet_bagde/pet_bagde_select")
function PetBagdeSelect:OnCloseClick()
    ViewMgr:CloseView(PetBagdeSelect)
end

function PetBagdeSelect:CloseCallback()
    PetBagdeData.Instance:SetSelectData(nil)
end

function PetBagdeSelect:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[276].desc)
end



PetBagdeSelectPanel = PetBagdeSelectPanel or DeclareMono("PetBagdeSelectPanel", UIWFlushPanel)
function PetBagdeSelectPanel:PetBagdeSelectPanel()
    self.data = PetBagdeData.Instance
    local qua_list = self:GetQuaList()
    self.sel_qua_data = qua_list[1]     -- 选中的品质
    self.sel_item_data = nil            -- 选中的徽章
    self.pet_bagde_item = nil           -- 当前宠物的徽章
    
    self.data_cares = {
        {data = PetData.Instance.pet_select, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushPanel, init_call = false},
    }
end

function PetBagdeSelectPanel:Awake()
    UIWFlushPanel.Awake(self)
    -- if PetData.Instance.bagde_remind_flag then
    --     PetData.Instance.bagde_remind_flag = false
    --     PetData.Instance.pet_list:Notify()
    -- end
    self:FlushPanel()
end

function PetBagdeSelectPanel:FlushPanel()
    self:FlushPetBagde()
    self:FlushList()
    self:FlushRightInfo()
end

function PetBagdeSelectPanel:FlushPetBagde()
    self.pet = PetData.Instance:GetCurSelect()
    if self.pet then
        local item = self.pet:BagdeItem()
        self.pet_bagde_item = item
    end
end

function PetBagdeSelectPanel:FlushRightInfo()
    self.ItemInfos[1]:SetObjActive(self.sel_item_data ~= nil or self.pet_bagde_item ~= nil)
    self.ItemInfos[2]:SetObjActive(self.sel_item_data ~= nil and self.pet_bagde_item ~= nil)
    if self.sel_item_data == nil and self.pet_bagde_item == nil then
        return
    elseif self.pet_bagde_item == nil and self.sel_item_data ~= nil then
        self.ItemInfos[1]:SetData(self.sel_item_data)
    elseif self.sel_item_data ~= nil and self.pet_bagde_item ~= nil then
        self.ItemInfos[1]:SetData(self.pet_bagde_item)
        self.ItemInfos[2]:SetData(self.sel_item_data)
    elseif self.pet_bagde_item then
        self.ItemInfos[1]:SetData(self.pet_bagde_item)
    end
    
    local item = self.pet_bagde_item ~= nil and self.pet_bagde_item or self.sel_item_data
    UH.SetText(self.ItemName, item:QuaName(true))
    UH.SetIcon(self.ItemIcon, item:IconId())

    self.BtnObj:SetActive(self.sel_item_data ~= nil)
    local btn_text = Language.PetBagde.SelectBtn1
    local pet_index = item:PetIndex()
    if not pet_index then
        btn_text = Language.PetBagde.SelectBtn2
    end
    UH.SetText(self.BtnText, btn_text)
end

function PetBagdeSelectPanel:FlushList()
    local qua = self.sel_qua_data.qua_level
    local bagde_list = self.data:GetBagdeList(qua, true)
    UH.SetText(self.ItemQuaText, self.sel_qua_data.name)
    self.NoBagde:SetActive(TableIsEmpty(bagde_list))
    self.BagdeList:SetObjActive(not TableIsEmpty(bagde_list))
    if not TableIsEmpty(bagde_list) then
        local list = self:FormatBaglist(bagde_list, 4)
        if #list >= 1 then
            self.sel_item_data = list[1][1]
            self.data:SetSelectData(self.sel_item_data)
            self:FlushRightInfo()
        end
        self.BagdeList:SetData(list)
    end
end

function PetBagdeSelectPanel:FormatBaglist(bagde_list, count)
    count = count or 4
    local list = {}
    local index = 1
    local data
    for _,v in pairs(bagde_list) do
        if index == 1 then
            data = {}
        end
        table.insert(data, v)
        index = index + 1
        if index > count then
            table.insert(list, data)
            index = 1
        end
    end
    if index > 1 then
        table.insert(list, data)
    end
    return list
end

function PetBagdeSelectPanel:GetQuaList()
    if not self.qua_list then
        self.qua_list = {{qua_level = 0, name = Language.PetBagde.Txt3}}
        local cfg = self.data.cfg.pet_badges
        for i,v in pairs(cfg) do
            local name = ColorStr(v.name, QualityColorStr[v.quality])
            table.insert(self.qua_list, {qua_level = v.quality, name = name})
        end
    end
    return self.qua_list
end

function PetBagdeSelectPanel:OnItemQuaClick()
    local qua_list = self:GetQuaList()
    self.ItemQuaList:SetData(qua_list)
end

function PetBagdeSelectPanel:OnQuaClick(data)
    self.sel_qua_data = data
    self.QuaToggle.isOn = false
    self:FlushPanel()
end

function PetBagdeSelectPanel:OnGridItemClick(data)
    self.sel_item_data = data
    self.data:SetSelectData(data)
    self:FlushRightInfo()
end

function PetBagdeSelectPanel:OnBtnClick()
    local item = self.sel_item_data
    if item and self.pet then
        local pet_index = item:PetIndex()
        if pet_index then
            -- 替换
            --LogError("替换：",PET_OP_TYPE.PetBagdeExChange, self.pet.index, pet_index)
            PetCtrl.SendPetOp(PET_OP_TYPE.PetBagdeExChange, self.pet.index, pet_index)
        else
            -- 穿戴
            --LogError("穿戴：",PET_OP_TYPE.PetBagdePutOn, self.pet.index, item.column_type, item.index)
            PetCtrl.SendPetOp(PET_OP_TYPE.PetBagdePutOn, self.pet.index, item.column_type, item.index)
        end
        ViewMgr:CloseView(PetBagdeSelect)
        self.sel_item_data = nil
    end
end

PetBagdeSelectQuaItem = PetBagdeSelectQuaItem or DeclareMono("PetBagdeSelectQuaItem", UIWidgetBaseItem)
function PetBagdeSelectQuaItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.BtnText, data.name)
end

PetBagdeSelectRowListItem = PetBagdeSelectRowListItem or DeclareMono("PetBagdeSelectRowListItem", UIWidgetBaseItem)
function PetBagdeSelectRowListItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.List:SetData(data)
end

function PetBagdeSelectRowListItem:Select(index)
    index = index or 1
    self.List:SetectItem(index, true)
    return self.data[index]
end

PetBagdeSelectGridItem = PetBagdeSelectGridItem or DeclareMono("PetBagdeSelectGridItem", UIWidgetBaseItem)
function PetBagdeSelectGridItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.ItemName, data:QuaName(true))
    UH.SetIcon(self.Icon, data:IconId())
    local pet_index = data:PetIndex()
    self.UsedObj:SetActive(pet_index ~= nil and pet_index >= 0)
    local attri_list = data:AttriList()
    self.AttriList:SetData(attri_list)
    local qua = data:QuaLevel()
    UH.SpriteName(self.Qua, "YuanPinZhiKuangXiao" .. qua)
    local skill_id = data:SkillId() or 0
    self.SkillFlag:SetActive(skill_id > 0)
    self:SetSelected(data == PetBagdeData.Instance:GetSelectData())
end

PetBagdeSelectAttriItem = PetBagdeSelectAttriItem or DeclareMono("PetBagdeSelectAttriItem", UIWidgetBaseItem)
function PetBagdeSelectAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local attri_info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
    local text = ""
    local attri_name = ""
    local attri_value = ""
    if self.IsAnd then
        text = attri_info.attr_name .. ColorStr(string.format("+%s", attri_info.attr_percent_value), COLORSTR.Green2)
    else
        attri_name = string.format("%s", attri_info.attr_name)
        attri_value = string.format("+%s", attri_info.attr_percent_value)
    end
    if self.Text then
        UH.SetText(self.Text, text)
    end
    UH.SetText(self.AttriName, attri_name)
    UH.SetText(self.AttriValue, attri_value)
end


PetBagdeSelectItemInfo = PetBagdeSelectItemInfo or DeclareMono("PetBagdeSelectItemInfo", UIWidgetBaseItem)
function PetBagdeSelectItemInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local color = self.Index == 1 and COLORSTR.Yellow9 or COLORSTR.Purple5
    local pet_index = data:PetIndex()
    local cur_pet_index = PetData.Instance:GetSelectPetIndex()

    -- 当前穿戴 | 选中替换 | 选中穿戴
    local title = ""
    local pet_name = ""
    if pet_index then
        local pet = PetData.Instance:GetPetInfo(pet_index)
        if pet then
            pet_name = ColorStr(string.format("  (%s)", pet:Name()), COLORSTR.Red2)
        end
    end
    if self.Index == 1 then
        if pet_index then
            if pet_index == cur_pet_index then
                title = Language.PetBagde.SelectTitle1
            else
                title = Language.PetBagde.SelectTitle2
            end
        else
            title = Language.PetBagde.SelectTitle3
        end
        title = ColorStr(title, color)
        title = title .. pet_name
    elseif self.Index == 2 then
        title = Language.PetBagde.SelectTitle2
        title = ColorStr(title, color)
        title = title .. pet_name
    end
    UH.SetText(self.Title, title)
    local attri_list = data:AttriList()
    self.AttriList:SetData(attri_list)
    local desc = data:SkillDesc()
    if desc then
        desc = ColorStr(desc,color)
    end
    UH.SetText(self.SkillDesc, desc)

    local h = 85 + self.SkillDesc.preferredHeight
    self.RectTrans:SetSizeWithCurrentAnchors(1,h)
    self.TextRectTrans:SetSizeWithCurrentAnchors(1,self.SkillDesc.preferredHeight)
end