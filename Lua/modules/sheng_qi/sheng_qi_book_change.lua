-- ===================== 圣器技能书置换 ===================
ShengQiBookChangeView = ShengQiBookChangeView or DeclareView("ShengQiBookChangeView", "sheng_qi/sheng_qi_book_change")

function ShengQiBookChangeView:ShengQiBookChangeView()

end

function ShengQiBookChangeView:LoadCallback(param_t)
    param_t = param_t or {}
    if param_t.item then
        local item = Item.Create({item_id = param_t.item.item_id, index = param_t.item.index, num = 1, is_bind = param_t.item:Bind()})
        PetData.Instance:SetChangeBook(1, item)
    end
end

function ShengQiBookChangeView:OnCloseClick()
    ViewMgr:CloseView(ShengQiBookChangeView)
end


ShengQiBookChangPanel = ShengQiBookChangPanel or DeclareMono("ShengQiBookChangPanel", PetBookChangePanel)

function ShengQiBookChangPanel:SetResultItem()
    self.result_item_id = Config.shengqi_auto.other[1].baoxiang1.item_id
    self.sel_qua = 5
    local item_1 = PetData.Instance:GetChangeBook(1)
    if item_1 and item_1:Color() > 5 then
        self.sel_qua = item_1:Color()
        self.result_item_id = Config.shengqi_auto.other[1].baoxiang2.item_id
        UH.SetText(self.Tip1, Language.PetBookChange.Tip7)
        UH.SpriteName(self.TitleImg, "_LocChaoShuZhiHuan")
    else
        UH.SetText(self.Tip1, Language.PetBookChange.Tip6)
        UH.SpriteName(self.TitleImg, "_LocGaoShuZhiHuan")
    end
end

function ShengQiBookChangPanel:OnChangeBookClick()
    if not self.data.pet_book_change_data.info[1] or not self.data.pet_book_change_data.info[2] then
        PublicPopupCtrl.Instance:Error(Language.PetBookChange.Error6)
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
    ShengQiCtrl.Instance:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_SKILL_CHANGE, p1, p2)
end



ShengQiChangeItem = ShengQiChangeItem or DeclareMono("ShengQiChangeItem", PetBookChangeItem)

function ShengQiChangeItem:SetName()
    local item = self.data.item or Item.Create({item_id = 0})
    local item_name
    if self.PosIndex == nil then
        item_name = ColorStr(item:Name(), item:ColorStr(true))
    elseif item.item_id > 0 then
        item_name = item:QuaName(true)
    else
        item_name = Language.PetBookChange.Tip5
    end
    UH.SetText(self.ItemName, item_name)
end

function ShengQiChangeItem:OnAddClick()
    if self.PosIndex == nil then
        return
    end
    ViewMgr:OpenView(ShengQiChangeSelView, {pos_index = self.PosIndex, qua = self.data.qua})
end

function ShengQiChangeItem:OnRemoveClick()
    if self.PosIndex == nil then
        return
    end
    PetData.Instance:SetChangeBook(self.PosIndex, nil)
end



-- 置换选择界面
ShengQiChangeSelView = ShengQiChangeSelView or DeclareView("ShengQiChangeSelView", "sheng_qi/sheng_qi_change_sel")
function ShengQiChangeSelView:ShengQiChangeSelView()
    self.BG:SetData(self:GetType(),Language.PetBookChange.Title, Vector2.New(660, 520))
    self.sel_list = {}
end

function ShengQiChangeSelView:LoadCallback(param_t)
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

function ShengQiChangeSelView:CloseCallback()
    
end

function ShengQiChangeSelView:GetListData()
    local list_data = {}
    local col_grids = BagData.Instance.item_grid[ItemColumnType.Item]
    for _,v in pairs(col_grids) do
        if v:Type() == ItemType.ShengQiSkillBook and v:Color() == self.sel_qua then
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

function ShengQiChangeSelView:FlushPanel()
    self.List:SetData(self.list_data)
    self.EmptyObj:SetActive(TableIsEmpty(self.list_data))
end

function ShengQiChangeSelView:OnSelClick(value, data)
    if value then
        self.sel_list[data] = true
    else
        if self.sel_list[data] then
            self.sel_list[data] = nil
        end
    end
end

function ShengQiChangeSelView:OnOkClick()
    ViewMgr:CloseView(ShengQiChangeSelView)
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