SpiritQuickBuyView = SpiritQuickBuyView or DeclareView("SpiritQuickBuyView", "equip_view/spirit_quick_buy")
VIEW_DECLARE_LEVEL(SpiritQuickBuyView, ViewLevel.Lt)
VIEW_DECLARE_MASK(SpiritQuickBuyView, ViewMask.BgBlock)

function SpiritQuickBuyView:SpiritQuickBuyView()
    self.Data = EquipData.Instance
    self.language = Language.Equip.SpiritQuick
end

function SpiritQuickBuyView:LoadCallback(param_t)
    self.view_param_t = param_t
    self.now_group_level_config = self.Data:GetSpiritLevelData(param_t.record_need.item_id)
    -- 现在不用快速购买，改用灵玉市场购买铸灵
    -- self.quick_buy_shop_data = ShopData.GetFastBuyData(self.now_group_level_config[0].spirit_stone_id)
    self.money_type = CurrencyType.ImmortalCoin
    self.immortal_coin = MarketData.Instance:GetItemPrice(self.now_group_level_config[0].spirit_stone_id)
    self.need_buy_consume = 0
    self.Cell:SetData(param_t.item)
    self:ShowText(param_t.record_need)
    self.confirm_fun = param_t.confirm_fun
end

function SpiritQuickBuyView:ShowText(record_need)
    local str = self.language[1]
    for i = 0, record_need.level do
        if record_need.item[i] and record_need.item[i] > 0 then
            local item_data1 = Item.Create({item_id = self.now_group_level_config[i].spirit_stone_id})
            str = str .. Format(self.language[2], record_need.item[i], item_data1:HasLevelQuaName())
        end
    end
    if record_need.need_buy_num > 0 then
        self.need_buy_consume = record_need.need_buy_num * self.immortal_coin
        local item_data2 = Item.Create({item_id = self.now_group_level_config[0].spirit_stone_id})
        str = str .. Format(self.language[3], self.need_buy_consume, Language.Common.HuoBi[self.money_type], record_need.need_buy_num, item_data2:HasLevelQuaName())
    end
    local item_data3 = Item.Create({item_id = record_need.next_item_id})
    str = str .. Format(self.language[4], item_data3:HasLevelQuaName())
    str = str .. self.language[5]
    UH.SetText(self.TxtCenter, str)
end

function SpiritQuickBuyView:OnCloseClick()
    ViewMgr:CloseView(SpiritQuickBuyView)
end

function SpiritQuickBuyView:OnCancelClick()
    ViewMgr:CloseView(SpiritQuickBuyView)
end

function SpiritQuickBuyView:OnConfirmClick()
    if self.immortal_coin > 0 then
        if RoleData.Instance:GetCurrency(self.money_type) >= self.need_buy_consume then
            self.confirm_fun()
        else
            local param_t = {
                config = StoreData.Instance:GetItemConfig(CommonItemId.ImmortalCoinId,5),
                comfirm_func = function(data)
                    ShoppingCtrl.Instance:SendStoreBuyReq(data)
                    ViewMgr:CloseView(MoneyExchangeView)
                end
            }
            param_t.money_type = CurrencyType.ImmortalCoin
            ViewMgr:OpenView(MoneyExchangeView,param_t)
        end
    else
        if MallCtrl.IsNotCurrency(self.money_type, self.need_buy_consume) then
            if self.confirm_fun ~= nil and type(self.confirm_fun) == "function" then
                self.confirm_fun()
            end
        end
    end
    ViewMgr:CloseView(SpiritQuickBuyView)
end