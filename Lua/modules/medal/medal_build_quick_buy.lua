--------------------------------------------
-- 从唤灵快速购买直接修订，只支持元宝对商场购买
MedalBuildQuickBuy = MedalBuildQuickBuy or DeclareView("MedalBuildQuickBuy","medal/medal_buy_quick_buy")
VIEW_DECLARE_MASK(MedalBuildQuickBuy,ViewMask.BgBlock)
function MedalBuildQuickBuy:MedalBuildQuickBuy()
    self.buy_num = 1
    self.is_empty = false
    self.param = nil
end

function MedalBuildQuickBuy:LoadCallback(param)
    if param and param.item_id then
        self.param = param
        --InfoText
            UH.SetText(self.InfoText, string.format(Language.TreasureHunt.desc, QualityColorStr[Item.GetColor(param.item_id)],Item.GetName(param.item_id)))
        --Item
        local item = Item.Init(param.item_id)
        self.ItemCell:SetData(item)
        --消耗类型
        local k,conifg = MallData.Instance:GetShopItemConfig(param.item_id)
        self.store_cfg = conifg
        UH.SpriteName(self.CurrencyImg, self.store_cfg.price_type == 0 and HuoBi[CurrencyType.Gold] or HuoBi[CurrencyType.Blessing])
        --限购数量
        UH.SetText(self.BuyLimitText,"")
        --购买数量
        self.buy_num = param.draw_times ~=nil and param.draw_times or self.buy_num
        self:setCostText()
        --按钮文字
        UH.SetText(self.ConfirmText, self.store_cfg.price_type == 0 and Language.TreasureHunt.BuyUse or Language.TreasureHunt.ExchangeUse)
    else
        self.is_empty = true
        UH.SetText(self.InfoText,"")
        UH.SetText(self.BuyLimitText,"")
        UH.SetText(self.CostNumText,"")
        UH.SetText(self.ConfirmText,Language.Common.Confirm)
        UH.SetText(self.BuyNumText,1)
        self.ItemCell:SetData()
    end
end

function MedalBuildQuickBuy:setCostText()
    UH.SetText(self.BuyNumText, self.buy_num)
    UH.SetText(self.CostNumText, self.buy_num * self.store_cfg.price)
end

function MedalBuildQuickBuy:OnClickAdd()
    self.buy_num = self:PriceLimit(self.buy_num + 1) and self.buy_num +1 or self.buy_num
    self:setCostText()
end

function MedalBuildQuickBuy:PriceLimit(num)
    local cost = num * self.store_cfg.price
    local have = RoleData.Instance:GetCurrency(self.store_cfg.price_type == 0 and CurrencyType.Gold or CurrencyType.Blessing)
    return have >= cost
end

function MedalBuildQuickBuy:OnClickSub()
    self.buy_num = self.buy_num - 1 > 0 and self.buy_num - 1 or 1
    self:setCostText()
end

function MedalBuildQuickBuy:OnClickCancel()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    ViewMgr:CloseView(MedalBuildQuickBuy)
end

function MedalBuildQuickBuy:OnClickBuy()
    if not self.is_empty then
        if self.store_cfg.price_type == 0 then
            if RoleData.Instance:GetCurrency(CurrencyType.Gold) < self.buy_num * self.store_cfg.price then
                local info = {
                    content = Language.Mall.NotCurrency[3],
                    close = true,
                    cancel = {
                        name = Language.Mall.NotCurrency[4],
                        func = function()
                            TradeData.Instance:SetJumpItemData(Item.Init())
                            ViewMgr:OpenViewByKey(Mod.Mall.Trade)
                            PublicPopupCtrl.Instance:CloseDialogTips()
                        end
                    },
                    confirm = {
                        name = Language.Mall.NotCurrency[5],
                        func = function()
                            ViewMgr:OpenViewByKey(Mod.Store.Recharge)
                            PublicPopupCtrl.Instance:CloseDialogTips()
                        end
                    },
                }
                if GLOBAL_CONFIG:Flags().block_recharge then
                    info.no_confirm = true
                end
                PublicPopupCtrl.Instance:DialogTips(info)
                return
            end
        else
            if RoleData.Instance:GetCurrency(CurrencyType.Blessing) < self.buy_num * self.store_cfg.price then
                MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = HuoBi[CurrencyType.Blessing]})})
                return
            end
        end

        ShoppingCtrl:SendShopBuyReq(self.store_cfg.conver_type,self.store_cfg.seq,self.store_cfg.item_id, self.buy_num)
        ViewMgr:CloseView(MedalBuildQuickBuy)
    end
end