MainViewBattlePetSeal = MainViewBattlePetSeal or DeclareMono("MainViewBattlePetSeal", UIWFlushPanel)
---ItemList====UIWidgetList

function MainViewBattlePetSeal:MainViewBattlePetSeal()
    self.battle_data = BattleData.Instance
    self.data_cares = {
        {data = BagData.Instance:GetColumnList(ItemColumnType.Item), func = self.FlushItemList}
    }
    GuideManager.Instance:RegisterGetGuideUi("FirstSeal",function ()
        local card_id = GuideData.Instance:GetSealNeedCard()
        if card_id and Item.GetNum(card_id) > 0 then
            for k,v in pairs(self.CardList.item_list) do
                if v.data.item_id == card_id then
                    return self.CardList.item_list[k]
                end
            end
            -- return self.CardList.item_list[2]
        else
            GuideManager.Instance:ForceStop()
        end
        
    end)
    GuideManager.Instance:RegisterGetGuideUi("SilverCardSeal",function ()
        local target_card = Config.function_guide_auto.pet_skill[1].yinka_fengyinka
        for i,v in pairs(self.CardList.item_list) do
            if v:GetData().item_id == target_card then
                if Item.GetNum(v:GetData().item_id) > 0 then
                    return v,function ()
                        self:OnItemSelected(v)
                    end
                else
                    GuideManager.Instance:ForceStop()
                end
            end
        end
        return self.CardList.item_list[1]
    end)
end

YouMingSealCard = {
    [56625] = 3,
    [56626] = 4,
}

PetSealCard = {
    [26078] = 0,
    [26079] = 1,
    [26080] = 2
}

function MainViewBattlePetSeal:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushItemList()
end

function MainViewBattlePetSeal:FlushItemList()
    local card_list = {}
    for i,v in pairs(BattleData.Instance.ch_info) do
        if v.character_type == 2 and v.level == 1 then
            local monster_cfg = Cfg.MonsterById(v.character_id)
            if monster_cfg then
                if monster_cfg.catch_pet_id ~= 0 then
                    local big_type = Item.GetBigType(monster_cfg.catch_pet_id)
                    if big_type == ITEM_BIG_TYPE.PET then
                        self.pet_cfg = PetData.Instance:GetPetCfg(monster_cfg.catch_pet_id)
                    elseif big_type == ITEM_BIG_TYPE.YOUMING then
                        self.youming_cfg = YouMingData.Instance:GetYouMingCfg(monster_cfg.catch_pet_id)
                    end
                end
            else
                local big_type = Item.GetBigType(v.character_id)
                if big_type == ITEM_BIG_TYPE.PET then
                    self.pet_cfg = PetData.Instance:GetPetCfg(v.character_id)
                elseif big_type == ITEM_BIG_TYPE.YOUMING then
                    self.youming_cfg = YouMingData.Instance:GetYouMingCfg(v.character_id)
                end
                self.pet_cfg = PetData.Instance:GetPetCfg(v.character_id)
            end
            break
        end
    end
    if self.pet_cfg == nil and self.youming_cfg == nil then
        return
    end
    if self.pet_cfg then
        for k,v in pairs(PetSealCard) do
            local data = {}
            data.item_id = k
            data.pet_cfg = self.pet_cfg
            data.card_qua = v + 1
            table.insert(card_list, data)
            -- card_list[v + 1] = data
        end
    end
    if self.youming_cfg then
        for k,v in pairs(YouMingSealCard) do
            local data = {}
            data.item_id = k
            data.youming_cfg = self.youming_cfg
            table.insert(card_list, data)
            -- card_list[v + 1] = data
        end
    end
    self.CardList:SetData(card_list)
end

function MainViewBattlePetSeal:OnClickClose()
    self.battle_data:ViewInfo().opened_sub = BattleSubView.None
end

function MainViewBattlePetSeal:OnItemSelected(cell)
    if self.battle_data:ViewInfo().opened_sub == BattleSubView.Seal then
        self.battle_data:ViewInfo().opened_sub = BattleSubView.None
    end
    local cell_data = cell:GetData()
    if cell_data.pet_cfg ~= nil then
        BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.SEAL_PET, PetSealCard[cell_data.item_id])
    elseif cell_data.youming_cfg ~= nil then
        BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.SEAL_YOUMING, PetSealCard[cell_data.item_id])
    end
end

function MainViewBattlePetSeal:OnAddClick(data)
    local item_id = data.item_id
    ViewMgr:OpenView(BuyPetSealCardView, {item_id = item_id})
end


BuyPetSealCardView = BuyPetSealCardView or DeclareView("BuyPetSealCardView", "fight/buy_pet_seal_card")
function BuyPetSealCardView:BuyPetSealCardView()
    
end

-- param_t:{item_id, num, cancel_func}
function BuyPetSealCardView:LoadCallback(param_t)
    param_t = param_t or {}
    if param_t.item_id then
        self.Panel:SetItemId(param_t.item_id, param_t.num, param_t.is_auto_buy)
        self.Panel:FlushPanel()
    end
    self.cancel_func = param_t.cancel_func
end

function BuyPetSealCardView:CloseCallback()
    
end

function BuyPetSealCardView:OnCancelClick()
    if self.cancel_func then
        self.cancel_func()
    end
    ViewMgr:CloseView(BuyPetSealCardView)
end

MainViewBattlePetBuyCard = MainViewBattlePetBuyCard or DeclareMono("MainViewBattlePetBuyCard", UIWFlushPanel)
function MainViewBattlePetBuyCard:MainViewBattlePetBuyCard()
    self.data_cares = {
        {data = BagData.Instance:GetColumnList(ItemColumnType.Item), func = self.FlushPanel}
    }
    self.buy_num = 1
end

function MainViewBattlePetBuyCard:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushPanel()
end

function MainViewBattlePetBuyCard:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ht)
end

function MainViewBattlePetBuyCard:SetItemId(item_id, buy_num, is_auto)
    self.item_id = item_id
    self.buy_num = buy_num or 1
    self.is_auto = is_auto
    if self.is_auto then
        TimeHelper:CancelTimer(self.ht)
        self.ht = Invoke(function ()
            self:OnOkClick()
        end,2)
    end
end

function MainViewBattlePetBuyCard:FlushPanel()
    if not self.item_id then
        return
    end
    self.ItemCell:SetData(Item.Create({item_id = self.item_id}))
    local item_num = Item.GetNum(self.item_id)
    local item_name = Item.GetName(self.item_id)
    UH.SetText(self.CardCount, string.format(Language.PetSeal.NowCardCount, item_name, item_num))
    self:FlushBuyInfo()
end

function MainViewBattlePetBuyCard:FlushBuyInfo()
    local info = ShopData.GetFastBuyData(self.item_id)
    self.info = info
    if TableIsEmpty(info) then
        return
    end
    local item_price = self.buy_num * info.price
    UH.SetText(self.BuyNum, tostring(self.buy_num))
    UH.SetText(self.BuyTip, string.format(Language.PetSeal.BuyTip, item_price, self.buy_num, Item.GetName(self.item_id)))
end

function MainViewBattlePetBuyCard:OnPutClick(p_int)
    self.buy_num = self.buy_num + p_int
    self.buy_num = self.buy_num < 1 and 1 or self.buy_num
    self:FlushBuyInfo()
end

function MainViewBattlePetBuyCard:Close()
    ViewMgr:CloseView(BuyPetSealCardView)
end

function MainViewBattlePetBuyCard:Open()
    self:SetObjActive(true)
end

function MainViewBattlePetBuyCard:OnCancelClick()
    self:Close()
end

function MainViewBattlePetBuyCard:OnOkClick()
    local price = self.buy_num * self.info.price
    if MallCtrl.IsNotCurrency(CurrencyType.CoinBind, price)then
        RoleCtrl.FastPurchase(self.item_id, self.buy_num)
        self:Close()
    elseif self.is_auto then
        BehaviorData.Instance.auto_behavior = 0
    end
end



MainViewBattlePetSealItem = MainViewBattlePetSealItem or DeclareMono("MainViewBattlePetSealItem", UIWidgetBaseItem)
function MainViewBattlePetSealItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local card_num = Item.GetNum(data.item_id)
    if data.pet_cfg then
        local pet_qua = data.pet_cfg.quality
        local card_qua = data.card_qua
        local qua_flag = card_qua >= pet_qua
        self.MaskObj:SetActive(card_num <= 0 or not qua_flag)
        self.SealBtnObj:SetActive(card_num > 0 and qua_flag)
        self.AddBtnObj:SetActive(qua_flag)
        if qua_flag then
            if card_num > 0 then
                self.ItemCell:SetData(Item.Create({item_id = data.item_id, num = card_num}))
            else
                self.ItemCell:SetData(Item.Create({item_id = data.item_id}))
                UH.SetText(self.Tip, Language.PetSeal.Tip1)
            end
        else
            self.ItemCell:SetData(Item.Create({item_id = data.item_id}))
            UH.SetText(self.Tip, Language.PetSeal.Tip2)
        end
    elseif data.youming_cfg then
        if card_num > 0 then
            self.ItemCell:SetData(Item.Create({item_id = data.item_id, num = card_num}))
        else
            self.ItemCell:SetData(Item.Create({item_id = data.item_id}))
            UH.SetText(self.Tip, Language.PetSeal.Tip1)
        end
    end
end
