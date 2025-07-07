PetBookChangeView = PetBookChangeView or DeclareView("PetBookChangeView", "pet/pet_book_change_view")

function PetBookChangeView:PetBookChangeView()

end

function PetBookChangeView:LoadCallback(param_t)
    param_t = param_t or {}
    if param_t.item then
        local item = Item.Create({item_id = param_t.item.item_id, index = param_t.item.index, num = 1, is_bind = param_t.item:Bind()})
        PetData.Instance:SetChangeBook(1, item)
    end
end

function PetBookChangeView:OnCloseClick()
    ViewMgr:CloseView(PetBookChangeView)
end

PetBookChangePanel = PetBookChangePanel or DeclareMono("PetBookChangePanel", UIWFlushPanel)
function PetBookChangePanel:PetBookChangePanel()
    self.data = PetData.Instance
    self.data_cares = {
        {data = self.data.pet_book_change_data, func = self.FlushPanel, init_call = false},
    }
end

function PetBookChangePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:SetResultItem()
    self:FlushPanel()
end

function PetBookChangePanel:SetResultItem()
    self.result_item_id = 25566
end

function PetBookChangePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:ClearChangeBookData()
end

function PetBookChangePanel:FlushPanel()
    for i = 1, 2 do
        local data = {}
        data.item = self.data:GetChangeBook(i)
        -- 限定的品质
        if self.sel_qua then
            data.qua = self.sel_qua
        end
        self.Items[i]:SetData(data)
    end
    local result_data = {}
    result_data.item = Item.Create({item_id = self.result_item_id})
    self.ResultItem:SetData(result_data)
    local item_qua_name = ColorStr(string.format("(%s)", result_data.item:Name()), result_data.item:ColorStr())
    UH.SetText(self.ChangeTip, item_qua_name)
end

function PetBookChangePanel:OnChangeBookClick()
    if not self.data.pet_book_change_data.info[1] or not self.data.pet_book_change_data.info[2] then
        PublicPopupCtrl.Instance:Error(Language.PetBookChange.Error1)
        return
    end
    local p1 = self.data.pet_book_change_data.info[1].item_id
    local p2 = self.data.pet_book_change_data.info[2].item_id
    if not p1 or not p2 or p1 < 1 or p2 < 1 then
        PublicPopupCtrl.Instance:Error(Language.PetBookChange.Error3)
        return
    end
    local cfg_1 = Item.GetConfig(p1)
    local cfg_2 = Item.GetConfig(p2)
    if not cfg_1 or not cfg_2 then
        PublicPopupCtrl.Instance:Error(Language.PetBookChange.Error3)
        return
    end
    PetCtrl.SendPetOp(PET_OP_TYPE.PetBookChange, p1, p2)
end

PetBookChangeItem = PetBookChangeItem or DeclareMono("PetBookChangeItem", UIWidgetBaseItem)
-- data : {item,qua?}
-- mono : PosIndex
function PetBookChangeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = data.item or Item.Create({item_id = 0})
    self.ItemCell:SetData(item)
    self.AddImgObj:SetActive(item.item_id == 0 and self.PosIndex ~= nil)
    self.RemoveBtnObj:SetActive(item.item_id > 0 and self.PosIndex ~= nil)
    self:SetName()
end

function PetBookChangeItem:SetName()
    local item = self.data.item or Item.Create({item_id = 0})
    local item_name
    if self.PosIndex == nil then
        item_name = ColorStr(Language.PetBookChange.Tip1, item:ColorStr(true))
    elseif item.item_id > 0 then
        item_name = item:QuaName(true)
    else
        item_name = Language.PetBookChange.Tip2
    end
    UH.SetText(self.ItemName, item_name)
end

function PetBookChangeItem:OnAddClick()
    if self.PosIndex == nil then
        return
    end
    ViewMgr:OpenView(PetBookChangeSelView, {pos_index = self.PosIndex})
end

function PetBookChangeItem:OnRemoveClick()
    if self.PosIndex == nil then
        return
    end
    PetData.Instance:SetChangeBook(self.PosIndex, nil)
end

-- 置换选择界面
PetBookChangeSelView = PetBookChangeSelView or DeclareView("PetBookChangeSelView", "pet/pet_book_change_sel")
function PetBookChangeSelView:PetBookChangeSelView()
    self.BG:SetData(self:GetType(),Language.PetBookChange.Title, Vector2.New(660, 520))
    self.sel_list = {}
end

function PetBookChangeSelView:LoadCallback(param_t)
    param_t = param_t or {}
    self.list_data = param_t.list_data or self:GetListData()
    self.pos_index = param_t.pos_index or 1

    local put_item = PetData.Instance:GetChangeBook(self.pos_index)
    self.is_replace = put_item ~= nil

    local btn_text = self.is_replace and Language.PetBookChange.Btn2 or Language.PetBookChange.Btn1
    UH.SetText(self.BtnText, btn_text)
    self:FlushPanel()
end

function PetBookChangeSelView:CloseCallback()
    
end

function PetBookChangeSelView:GetListData()
    local list_data = {}
    local col_grids = BagData.Instance.item_grid[ItemColumnType.Item]
    for _,v in pairs(col_grids) do
        if v:Type() == ItemType.Skill and v:Color() >= 5 then
            local vo = {item_id = v.item_id, num = v:Num(), index = v:GridIndex(), is_bind = v:Bind()}
            for i = 1, 2 do
                local put_item = PetData.Instance:GetChangeBook(i)
                if put_item then
                    if put_item.index == vo.index then
                        vo.num = vo.num - 1
                    end
                end
            end
            if vo.num > 0 then
                local item = Item.Create(vo)
                table.insert(list_data, item)
            end
        end
    end
    return list_data
end

function PetBookChangeSelView:FlushPanel()
    self.List:SetData(self.list_data)
    self.EmptyObj:SetActive(TableIsEmpty(self.list_data))
end

function PetBookChangeSelView:OnSelClick(value, data)
    if value then
        self.sel_list[data] = true
    else
        if self.sel_list[data] then
            self.sel_list[data] = nil
        end
    end
end

function PetBookChangeSelView:OnOkClick()
    ViewMgr:CloseView(PetBookChangeSelView)
    for i,v in pairs(self.sel_list)do
        if v then
            local item = Item.Create({item_id = i.item_id, index = i.index, num = 1, is_bind = i:Bind()})
            PetData.Instance:SetChangeBook(self.pos_index, item)
            return
        end
    end
    if not self.is_replace then
        PublicPopupCtrl.Instance:Center(Language.PetBookChange.Error2)
    end
end

PetBookChangeSelItem = PetBookChangeSelItem or DeclareMono("PetBookChangeSelItem", UIWidgetBaseItem)
function PetBookChangeSelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    UH.SetText(self.Name, data:QuaName())
end