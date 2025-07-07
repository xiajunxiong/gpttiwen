-- ===================== 神印置换 ===================
PetStampChangeView = PetStampChangeView or DeclareView("PetStampChangeView", "pet/pet_stamp/pet_stamp_change")

function PetStampChangeView:PetStampChangeView()

end

function PetStampChangeView:LoadCallback(param_t)
    param_t = param_t or {}
    if param_t.item then
        PetData.Instance:SetChangeBook(1, param_t.item)
    end
end

function PetStampChangeView:OnCloseClick()
    ViewMgr:CloseView(PetStampChangeView)
end

PetStampChangePanel = PetStampChangePanel or DeclareMono("PetStampChangePanel", PetBookChangePanel)

function PetStampChangePanel:SetResultItem()
    self.result_item_id = Config.pet_god_print_auto.other[1].exchange_get_item_id
    self.sel_qua = 5
    local item_1 = PetData.Instance:GetChangeBook(1)
    if item_1 and item_1:Color() > 5 then
        self.sel_qua = item_1:Color()
        self.result_item_id = Config.pet_god_print_auto.other[1].exchange_get_item_id_1
    end
end

function PetStampChangePanel:OnChangeBookClick()
    if not self.data.pet_book_change_data.info[1] or not self.data.pet_book_change_data.info[2] then
        PublicPopupCtrl.Instance:Error(Language.PetBookChange.Error5)
        return
    end
    local p1 = self.data.pet_book_change_data.info[1]:Index()
    local p2 = self.data.pet_book_change_data.info[2]:Index()
    if not p1 or not p2 or p1 < 1 or p2 < 1 then
        PublicPopupCtrl.Instance:Error(Language.PetBookChange.Error3)
        return
    end
    PetCtrl.SendPetOp(PET_OP_TYPE.PetStampChange, p1, p2)
end

PetStampChangeItem = PetStampChangeItem or DeclareMono("PetStampChangeItem", PetBookChangeItem)

function PetStampChangeItem:SetName()
    local item = self.data.item or Item.Create({item_id = 0})
    local item_name
    if self.PosIndex == nil then
        item_name = ColorStr(item:Name(), item:ColorStr(true))
    elseif item.item_id > 0 then
        item_name = item:QuaName(true)
    else
        item_name = Language.PetBookChange.Tip4
    end
    UH.SetText(self.ItemName, item_name)
end

function PetStampChangeItem:OnAddClick()
    if self.PosIndex == nil then
        return
    end
    ViewMgr:OpenView(PetStampChangeSelView, {pos_index = self.PosIndex, qua = self.data.qua})
end

function PetStampChangeItem:OnRemoveClick()
    if self.PosIndex == nil then
        return
    end
    PetData.Instance:SetChangeBook(self.PosIndex, nil)
end



-- 神印置换选择界面
PetStampChangeSelView = PetStampChangeSelView or DeclareView("PetStampChangeSelView", "pet/pet_stamp/pet_stamp_change_sel")
function PetStampChangeSelView:PetStampChangeSelView()
    self.BG:SetData(self:GetType(),Language.PetBookChange.Title2, Vector2.New(911, 530))
    self.sel_list = {}
end

function PetStampChangeSelView:LoadCallback(param_t)
    param_t = param_t or {}
    self.sel_qua = param_t.qua or 5
    self.list_data = param_t.list_data or self:GetListData()
    self.pos_index = param_t.pos_index or 1

    local put_item = PetData.Instance:GetChangeBook(self.pos_index)
    self.is_replace = put_item ~= nil

    local btn_text = self.is_replace and Language.PetBookChange.Btn2 or Language.PetBookChange.Btn1
    UH.SetText(self.BtnText, btn_text)
    self:FlushPanel()
end

function PetStampChangeSelView:CloseCallback()
    
end

function PetStampChangeSelView:GetListData()
    local list_data = {}
    local pet_stamps = PetStampData.Instance:GetPetStampList()
    for _,v in pairs(pet_stamps) do
        if v:QuaLevel() == self.sel_qua and (not v.info.pet_index or v.info.pet_index < 0) then
            local is_ok = true
            for i = 1, 2 do
                local put_item = PetData.Instance:GetChangeBook(i)
                if put_item then
                    if put_item == v or (put_item:Index() == v:Index()) then
                        is_ok = false
                    end
                end
            end
            if is_ok then
                table.insert(list_data, v)
            end
        end
    end
    return list_data
end

function PetStampChangeSelView:FlushPanel()
    self.List:SetData(self.list_data)
    self.EmptyObj:SetActive(TableIsEmpty(self.list_data))
end

function PetStampChangeSelView:OnSelClick(value, data)
    if value then
        self.sel_list[data] = true
    else
        if self.sel_list[data] then
            self.sel_list[data] = nil
        end
    end
end

function PetStampChangeSelView:OnOkClick()
    ViewMgr:CloseView(PetStampChangeSelView)
    for i,v in pairs(self.sel_list)do
        if v then
            PetData.Instance:SetChangeBook(self.pos_index, i)
            return
        end
    end
    if not self.is_replace then
        PublicPopupCtrl.Instance:Center(Language.PetBookChange.Error4)
    end
end


PetStampChangeSelItem = PetStampChangeSelItem or DeclareMono("PetStampChangeSelItem", UIWidgetBaseItem)
function PetStampChangeSelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    UH.SetText(self.Name, data:QuaName())
    local attri_list = data:AttriListInfo()
    self.AttriList:SetData(attri_list)
end


PetStampChangeSelItemAttri = PetStampChangeSelItemAttri or DeclareMono("PetStampChangeSelItemAttri", UIWidgetBaseItem)
function PetStampChangeSelItemAttri:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local attri_info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
    if data.attri_value == 0 then
        UH.SetText(self.Text, "")
        return
    end
    local name = attri_info.attr_name
    local value = attri_info.attr_percent_value
    local text = string.format("%s+%s", name, value)
    if data.is_main == 1 then
        text = ColorStr(text, COLORSTR.Yellow1)
    end
    UH.SetText(self.Text, text)
end