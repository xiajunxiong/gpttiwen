-- =========== 天下第一比武大会：武会宝阁 ==========

UTAGuessStore = UTAGuessStore or DeclareView("UTAGuessStore", "universe_team_arena/uta_guessing_store",Mod.UniverTeamArena.Store)
function UTAGuessStore:LoadCallback()
    UH.SetText(self.Title, Language.UTA.Title2)
end

function UTAGuessStore:OnCloseClick()
    ViewMgr:CloseView(UTAGuessStore)
end

UTAGuessStorePanel = UTAGuessStorePanel or DeclareMono("UTAGuessStorePanel", UIWFlushPanel)
function UTAGuessStorePanel:UTAGuessStorePanel()
    self.page_type = 12
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushRightInfo, init_call = false},
    }
end

function UTAGuessStorePanel:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetIcon(self.HasCurrIcon, Item.GetIconId(HuoBi[CurrencyType.GuessCoin]))
    UH.SetIcon(self.NeedCurrIcon, Item.GetIconId(HuoBi[CurrencyType.GuessCoin]))
    self:FlushPanel()
end

function UTAGuessStorePanel:FlushPanel()
    self:FlushList()
end

function UTAGuessStorePanel:FlushList()
    local list_data = StoreData.Instance:GetPageData(self.page_type)
    self.List:SetData(list_data)
    if self.sel_data == nil then
        self.List:SetectItem(1, true)
    end
end

function UTAGuessStorePanel:FlushRightInfo()
    if not self.sel_data then
        return
    end
    local item_id = self.sel_data.item_id
    UH.SetText(self.ItemName, Item.GetName(item_id))
    local desc = Item.GetDesc(item_id)
    if StringIsEmpty(desc) and BagData.IsHasGift(item_id) then
        local item_num = 1
        local config = BagData.Instance:GetGiftItemList(item_id)
        local temp = self:GetGiftTitle(config)
        for k,v in pairs(config.item_list) do
            temp = Format("%s\n%s x%s",temp,Item.GetQuaName(v.item_id),v.num)
        end
        desc = temp
    end

    UH.SetText(self.ItemDesc, desc)
    local curr_num = RoleData.Instance:GetCurrency(CurrencyType.GuessCoin) or 0
    UH.SetText(self.BuyCount, self.buy_count)
    local need_curr = self.buy_count * self.sel_data.price
    if need_curr > curr_num then
        UH.SetText(self.HasCurr, ColorStr(curr_num, COLORSTR.Red8))
    else
        UH.SetText(self.HasCurr, curr_num)
    end
    UH.SetText(self.NeedCurr,need_curr)

    local limit_str = Language.UTAPanel.Store.Text1
    if self.sel_data.limit_type > 0 then
        local limit_times = StoreData.Instance:GetHasTimes(self.sel_data)
        local str =  Language.UTA.StoreLimitType[self.sel_data.limit_type] or ""
        limit_str = limit_str .. limit_times .. str
    else
        limit_str = limit_str .. Language.UTAPanel.Store.Text2
    end
    UH.SetText(self.LimitText, limit_str)
end

function UTAGuessStorePanel:GetGiftTitle(config)
	local ItemGiftDesc = Language.ItemInfo.ItemGiftDesc
	if config.select_num and config.select_num ~= 0 then
		return Format(ItemGiftDesc.select_num,config.select_num)
	elseif config.rand_num and config.rand_num ~= 0 then
		return Format(ItemGiftDesc.rand_num,config.rand_num)
	end
	return ItemGiftDesc.item_num
end

function UTAGuessStorePanel:OnSelItem(data)
    self.buy_count = 1
    self.sel_data = data
    self:FlushRightInfo()
end

function UTAGuessStorePanel:OnAddClick(num)
    if not num or not self.sel_data then
        return
    end
    local count = self.buy_count + num
    if count == 0 then
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Store.Tip1)
        return
    end
    if self.sel_data.limit_type > 0 then
        local limit_times = StoreData.Instance:GetHasTimes(self.sel_data)
        if count > limit_times then
            PublicPopupCtrl.Instance:Center(Language.UTAPanel.Store.Tip2)
            return
        end
    end
    self.buy_count = count
    self:FlushRightInfo()
end

function UTAGuessStorePanel:OnBuyClick()
    if self.buy_count == 0 or not self.sel_data then
        return
    end
    -- 等级判断
    if self.sel_data.require_type == 1 and RoleData.Instance:GetRoleLevel() < self.sel_data.require_value then
        PublicPopupCtrl.Instance:Center(Format(Language.Shop.RequireLevelTip, self.sel_data.require_value))
        return
    end

    local need_curr = self.buy_count * self.sel_data.price
    local curr_num = RoleData.Instance:GetCurrency(CurrencyType.GuessCoin) or 0
    if curr_num >= need_curr then
        local seq = self.sel_data.seq
        local item_id = self.sel_data.item_id
        local conver_type = self.sel_data.conver_type
        --LogError("武会宝阁购买请求",conver_type,seq,item_id,self.buy_count)
        ShoppingCtrl.Instance:SendShopBuyReq(conver_type,seq,item_id,self.buy_count)
    else
        UniverseTeamArenaCtrl.Instance:ShowGuessItem()
        PublicPopupCtrl.Instance:Error(Language.UTAPanel.Tip1)
    end
end

UTAGuessStoreItem = UTAGuessStoreItem or DeclareMono("UTAGuessStoreItem", UIWidgetBaseItem)
function UTAGuessStoreItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data.item_id, num = data.num})
    self.ItemCell:SetData(item)
    UH.SetText(self.ItemName, item:Name())
    UH.SetText(self.Price, data.price)
    self.Flag:SetActive(data.limit_type > 0)
    UH.SetIcon(self.Icon, Item.GetIconId(HuoBi[CurrencyType.GuessCoin]))
end