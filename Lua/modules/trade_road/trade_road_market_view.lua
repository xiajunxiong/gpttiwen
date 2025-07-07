TradeRoadMarketPanel = TradeRoadMarketPanel or DeclareMono("TradeRoadMarketPanel", UIWFlushPanel)
function TradeRoadMarketPanel:TradeRoadMarketPanel()
    self.data_cares = {
        {data = TradeRoadData.Instance.road_info, func = self.FlushView}
    }
    GuideManager.Instance:RegisterGetGuideUi("TradeRoadMarketShop", function()
        return self.Left, function() end
    end)
    GuideManager.Instance:RegisterGetGuideUi("TradeRoadMarketBag", function()
        return self.Right, function() end
    end)
end

function TradeRoadMarketPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    GuideManager.Instance:UnRegisterGetGuideUi("TradeRoadMarketShop")
    GuideManager.Instance:UnRegisterGetGuideUi("TradeRoadMarketBag")
end

function TradeRoadMarketPanel:FlushView()
    if not TradeRoadData.Instance:IsJoined() then
        return
    end
    local city_cfg = TradeRoadData.Instance:GetCurCityCfg()
    UH.SetText(self.CityTitle, city_cfg.city_name)

    self.ShopList:SetData(TradeRoadData.Instance:GetShopList())

    local bag_list = TradeRoadData.Instance:GetBagItemList()
    self.BagEmpty:SetActive(#bag_list < 1)
    self.BagList:SetData(bag_list)
    
    local init_price = TradeRoadData.Instance:GetInitPrice()
    UH.SetText(self.Money1Text, DataHelper.ConverMoney(init_price))
    local total_price = TradeRoadData.Instance:GetRewardPrice()
    UH.SetText(self.Money2Text, DataHelper.ConverMoney(total_price))



    local index, next_seq = TradeRoadData.Instance:GetNextCitySeq()
    self.SupplyNode:SetActive(next_seq ~= nil)
    if next_seq ~= nil then
        local city_cfg = TradeRoadData.Instance:GetCityCfg(next_seq)
        UH.SetText(self.BtnGoText, Format(Language.TradeRoad.GoNextCity, city_cfg.city_name))
        
        local expend_val = TradeRoadData.Instance:GetSupplyCost(TradeRoadData.Instance:GetSupplyExpend())
        UH.SetText(self.SupplyText, expend_val)
    else
        UH.SetText(self.BtnGoText, Language.TradeRoad.GoNextOver)
    end
end

function TradeRoadMarketPanel:OnClickMap()
    -- ViewMgr:OpenView(TradeRoadMapView)
    ViewMgr:CloseView(TradeRoadView)
end

function TradeRoadMarketPanel:OnClickRecord()
    ViewMgr:OpenView(TradeRoadRecordView)
end

function TradeRoadMarketPanel:OnClickReward()
    ViewMgr:OpenView(TradeRoadRewardView)
end

--点击出发
function TradeRoadMarketPanel:OnClickNext()
    --弹窗1：最后一个城市->取消/弹窗结算
    if TradeRoadData.Instance:IsCurLastCity() then
        local info = {
            content = Language.TradeRoad.TradingLast,
            confirm = {
                func = function()
                    TradeRoadCtrl.Instance:SendFinshReq()
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end
    --弹窗2：贸易次数上限->取消/弹窗结算
    if TradeRoadData.Instance:GetTradingNum() >= TradeRoadData.Instance:GetTradingMaxNum() then
        local info = {
            content = Language.TradeRoad.TradingEmpty,
            confirm = {
                func = function()
                    TradeRoadCtrl.Instance:SendFinshReq()
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end
    --弹窗3：体力不足->购买体力/弹窗结算
    local expend_val = TradeRoadData.Instance:GetSupplyCost(TradeRoadData.Instance:GetSupplyExpend())
    if expend_val > TradeRoadData.Instance:CurSupply() then
        local info = {
            content = Language.TradeRoad.SupplyEmpty,
            cancel = {
                name = Language.TradeRoad.BuySupply,
                func = function()
                    PublicPopupCtrl.Instance:CloseDialogTips()
                    if TradeRoadData.Instance:CurBuySupplyTimes() >= TradeRoadData.Instance:MaxBuySupplyTimes() then
                        return
                    end
                    ViewMgr:OpenView(TradeRoadBuySupplyView)
                end
            },
            confirm = {
                func = function()
                    TradeRoadCtrl.Instance:SendFinshReq()
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end,
                wait_time = 5,
            },
            close = true
        }
        info.no_cancel = TradeRoadData.Instance:CurBuySupplyTimes() >= TradeRoadData.Instance:MaxBuySupplyTimes()
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end
    
    --弹窗4：当前城市未打探消息->取消/前往
    --前往下一个城镇时，增加一个弹窗，勾选之后，（得是本局游戏不提示。。。）
    if TradeRoadData.Instance:IsCanBuyMessage() and TradeRoadData.Instance:IsTavernRemind() and TradeRoadData.Instance:GetRemainNews() > 0 then
        local info = {
            content = Language.TradeRoad.GoToTavernTips,
            confirm = {
                func = function()
                    local not_reminds = PublicPopupCtrl.Instance.dialog_tips_data.today_not_reminds[DTTodayNotRemind.TradeRoadMarketNext]
                    PublicPopupCtrl.Instance.dialog_tips_data.today_not_reminds[DTTodayNotRemind.TradeRoadMarketNext] = false
                    TradeRoadData.Instance:SetTavernRemind(not not_reminds)
                    PublicPopupCtrl.Instance:CloseDialogTips()
                    self:GoToNext()
                end,
            },
            tnr = DTTodayNotRemind.TradeRoadMarketNext,
            trn_str = Language.TradeRoad.NoRemind
        }
        PublicPopupCtrl.Instance:DialogTips(info)
    else
        self:GoToNext()
    end
end

function TradeRoadMarketPanel:GoToNext()
    local expend_val = TradeRoadData.Instance:GetSupplyCost(TradeRoadData.Instance:GetSupplyExpend())
    PublicPopupCtrl.Instance:Center(Format(Language.TradeRoad.BeginTips1, expend_val, TradeRoadData.Instance:CurSupply() - expend_val))

    local index, next_seq = TradeRoadData.Instance:GetNextCitySeq()
    local city_cfg = TradeRoadData.Instance:GetCityCfg(next_seq)
    if city_cfg ~= nil then
        PublicPopupCtrl.Instance:Center(Format(Language.TradeRoad.BeginTips2, city_cfg.city_name))
    end
    TradeRoadData.Instance:SetToNextCity()
    ViewMgr:CloseView(TradeRoadView)
end

--商店物品
TradeRoadMarketShopItem = TradeRoadMarketShopItem or DeclareMono("TradeRoadMarketShopItem", UIWidgetBaseItem)
function TradeRoadMarketShopItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.item_cfg = TradeRoadData.Instance:GetItemCfg(data)

    self.ItemCell:SetData({seq = self.item_cfg.seq})
    UH.SetText(self.NameText, self.item_cfg.freight_name)
    UH.Color(self.NameText, QualityColorShallow[self.item_cfg.pinzhi])

    local item_info = TradeRoadData.Instance:GetCurCityItemInfo(data)
    UH.SetText(self.PriceText, item_info.price)

    local last_price = TradeRoadData.Instance:GetBeforeCityItemPrice(TradeRoadData.Instance:CurCitySeq(), self.item_cfg.seq)
    if last_price == -1 then
        last_price = self.item_cfg.freight_market_price
    end
    local lerp_val = item_info.price - last_price

    self.ImgState.gameObject:SetActive(lerp_val ~= 0)
    UH.SetText(self.RateText, math.abs(lerp_val))
    if lerp_val > 0 then --涨
        UH.Color(self.RateText, COLORS.Red20)
        UH.SpriteName(self.ImgState, "UpBg")
        UH.SpriteName(self.ImgRate, "UpArrow")
        UH.SetText(self.StateText, Language.TradeRoad.PriceState[2])
    elseif lerp_val < 0 then
        UH.Color(self.RateText, COLORS.Green9)
        UH.SpriteName(self.ImgState, "DownBg")
        UH.SpriteName(self.ImgRate, "DownArrow")
        UH.SetText(self.StateText, Language.TradeRoad.PriceState[1])
    else
        UH.Color(self.RateText, COLORS.Yellow24)
    end
end

function TradeRoadMarketShopItem:OnClickItem()
    if TradeRoadData.Instance:IsHasBagEmpty() then
        local item_info = TradeRoadData.Instance:GetCurCityItemInfo(self.data)
        if item_info ~= nil then
            ViewMgr:OpenView(TradeRoadBuyView, {seq = self.data})
        end
    else
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.BagNoEmpty)
        if TradeRoadData.Instance:CurBuyBagTimes() >= TradeRoadData.Instance:MaxBuyBagTimes() then
            return
        end
        ViewMgr:OpenView(TradeRoadBuyBagView)
    end
end

--背包物品
TradeRoadMarketBagItem = TradeRoadMarketBagItem or DeclareMono("TradeRoadMarketBagItem", UIWidgetBaseItem)
function TradeRoadMarketBagItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.item_cfg = TradeRoadData.Instance:GetItemCfg(data.commodity_seq)

    self.ItemCell:SetData({seq = self.item_cfg.seq})
    UH.SetText(self.NameText, self.item_cfg.freight_name)
    UH.Color(self.NameText, QualityColorShallow[self.item_cfg.pinzhi])

    UH.SetText(self.PriceText, data.commodity_cost)
    UH.SetText(self.NumText, data.commodity_count)

    local item_info = TradeRoadData.Instance:GetCurCityItemInfo(data.commodity_seq)
    self.ImgState.gameObject:SetActive(item_info ~= nil)

    local sale_price = item_info ~= nil and item_info.price or self.item_cfg.freight_market_price
    local lerp_val = sale_price - data.commodity_cost
    if lerp_val > 0 then --涨
        UH.SpriteName(self.ImgState, "UpBg")
        UH.SetText(self.StateText, Language.TradeRoad.PriceState[2])
    elseif lerp_val < 0 then
        UH.SpriteName(self.ImgState, "DownBg")
        UH.SetText(self.StateText, Language.TradeRoad.PriceState[1])
    end
    self.ImgState.gameObject:SetActive(lerp_val ~= 0)
end

function TradeRoadMarketBagItem:OnClickItem()
    ViewMgr:OpenView(TradeRoadSaleView, self.data)
end
