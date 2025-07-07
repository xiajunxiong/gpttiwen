PetExp = PetExp or DeclareView("PetExp", "pet/pet_exp")
VIEW_DECLARE_MASK(PetExp, ViewMask.None)

function PetExp:PetExp()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance
    self.select_data = nil
    self.handle = BagData.Instance:Care(BindTool.Bind(self.OnFlush, self))
    self.handle_pet = PetData.Instance:GetPetSelect():Care(BindTool.Bind(self.OnFlush, self))
end

function PetExp:LoadCallback()
    self.cur_select_item = nil
    -- self.Board:SetData(self:GetType(), self.lang.ExpTitle, Vector2.New(670, 456))
end

function PetExp:OnFlush()
    self.pet_index = self.pet_data:GetSelectPetIndex()
    local data = TableCopy(self.pet_data:GetPetExpItemList())
    table.sort(data, function (a,b)
        return a.add_exp < b.add_exp
    end )
    self.ExpList:SetData(data)
    if self.select_data then
        self.ExpList:ClickItemData(self.select_data)
    else
        self:autoSelectNext()
        -- self.ExpList:ClickItem(1)
    end
end

function PetExp:OnClickClose()
    ViewMgr:CloseView(PetExp)
end

function PetExp:OnClickItem(data)
    self.cur_select_item = data.item_id
    self.select_data = data
    self.add_exp = data.add_exp
end

function PetExp:OnClickUseOne()
    -- 如果数量不足就 弹窗
    if Item.GetNum(self.cur_select_item) == 0 then
        self:OpenBuyConfirm()
        return
    end
    local role_level = RoleData.Instance:GetRoleLevel()
    local pet = PetData.Instance:GetCurSelect()
    if pet == nil then
        --理论上来讲，能打开这个界面的时候，这个pet一定不会是空的，应该是某些特殊原因造成这里的pet为空，直接return不做反馈提示
        return
    end
    if pet:Level() == role_level then
        PublicPopupCtrl.Instance:Center(ErrorText[117])
        return
    end
    local num = Item.GetNum(self.cur_select_item)
    PetCtrl.SendPetOp(PET_OP_TYPE.AddExp, self.pet_index, self.cur_select_item, 1)
    if not pet:IsNeedBreach() then
        self.ExpPool:Spwan(self.add_exp * 1)
    end
    if num <= 1 then
        self:autoSelectNext()
    end
end

function PetExp:OnClickBuy()
    if self.cur_select_item <= 26393 then
        MallCtrl.Instance:Jump(26392, nil, Mod.Store.Main)
    else
        MallCtrl.Instance:Jump(self.cur_select_item, nil, Mod.Store.Main)
    end
end

function PetExp:OnAutoUpClick()
    local exp_item_list = self.pet_data:GetPetExpItemList()
    local is_has_exp = false
    for i,v in pairs(exp_item_list) do
        local num = Item.GetNum(v.item_id)
        if num > 0 then
            is_has_exp = true
            break
        end
    end
    if not is_has_exp then
        PublicPopupCtrl.Instance:Error(Language.Pet.ExpStuffNotTip)
        local item_id = exp_item_list[#exp_item_list].item_id
        local item = Item.Create({item_id = item_id})
        MainOtherCtrl.Instance:GetWayView({item = item})
    else
        PetCtrl.SendPetOp(PET_OP_TYPE.PetAutoUpLevel, self.pet_index)
    end
end

function PetExp:OnClickUseTen()
    local role_level = RoleData.Instance:GetRoleLevel()
    local pet = PetData.Instance:GetCurSelect()
    if pet == nil then
        return
    end
    if pet:Level() == role_level then
        PublicPopupCtrl.Instance:Center(ErrorText[117])
        return
    end
    -- 数量不足10个时不再弹窗而是直接使用剩余所有
    local num = Item.GetNum(self.cur_select_item)
    if num > 0 then
        PetCtrl.SendPetOp(PET_OP_TYPE.AddExp, self.pet_index, self.cur_select_item, num > 10 and 10 or num)
        if not pet:IsNeedBreach() then
            self.ExpPool:Spwan(self.add_exp * (num > 10 and 10 or num))
        end
        if num <= 10 then
            self:autoSelectNext()
        end
    else
        -- 如果数量不足1就弹提示
        self:OpenBuyConfirm()
    end
end

function PetExp:OpenBuyConfirm()
    PublicPopupCtrl.Instance:DialogTips(
        {
            content = Language.Pet.ExpNotEnough,
            confirm = {
                func = function()
                    --MallCtrl.Instance:Jump(26392, nil, Mod.Store.Main)
                    --MallData.Instance:SetJumpItemNum(1)
                    MarketData.Instance:SetJumpIndex(PetExpItemMarketData[self.cur_select_item] or 22061)
                    ViewMgr:OpenViewByKey(Mod.Mall.Market)
                    ViewMgr:CloseView(DialogTipsView)
                end
            }
        }
    )
end

function PetExp:CloseCallback()
    BagData.Instance:UnCare(self.handle)
    PetData.Instance:GetPetSelect():Uncare(self.handle_pet)
end

function PetExp:autoSelectNext()
    local data_list = TableCopy(self.pet_data:GetPetExpItemList())
    table.sort(data_list, function (a,b)
        return a.add_exp < b.add_exp
    end)
    local select_next = false
    for i = 1, #data_list do
        data_list[i].num = Item.GetNum(data_list[i].item_id)
        if select_next and data_list[i].num >= 1 then
            self.ExpList:ClickItem(i)
            select_next = false
            break
        end
        if self.cur_select_item == data_list[i].item_id then
            select_next = true
        end
    end
    if select_next or self.cur_select_item == nil then
        for i = 1, #data_list do
            if data_list[i].num >= 1 then
                self.ExpList:ClickItem(i)
                break
            elseif i == #data_list then
                self.ExpList:ClickItem(1)
            end
        end
    end
end
------------------------------------
PetExpItem = DeclareMono("PetExpItem", UIWidgetBaseItem)
-- LblName = Text
-- ExpCell = UIWItemCell
-- LblExp = Text
-- ToggleItem = LuaUIEventClick
function PetExpItem:SetData(data)
    UH.SetText(self.LblName, Item.Create({item_id = data.item_id}):QuaName(true))
    self.ExpCell:SetDataByItemIdWithZeroNum(data.item_id)
    UH.SetText(self.LblExp, data.add_exp)
    UIWidgetBaseItem.SetData(self, data)
end

function PetExpItem:Click()
    if self.ToggleItem then
        self.ToggleItem:Trigger()
    end
end

ExpPoolItem = ExpPoolItem or DeclareMono("ExpPoolItem", UIWObjPoolItem)
function ExpPoolItem:setData(data)
    UH.SetText(self.NumTex, string.format("+%s", data))
    self.handle_delay = TimeHelper:AddDelayTimer(
        function()
            self:RecycleSelf()
        end,
        0.5
    )
end
function ExpPoolItem:onRecycle()
    TimeHelper:CancelTimer(self.handle_delay)
end
function ExpPoolItem:OnDestroy()
    TimeHelper:CancelTimer(self.handle_delay)
end