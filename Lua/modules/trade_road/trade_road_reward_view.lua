TradeRoadRewardView = TradeRoadRewardView or DeclareView("TradeRoadRewardView", TradeRoadConfig.ResPath .. "trade_road_reward")
VIEW_DECLARE_MASK(TradeRoadRewardView, ViewMask.BgBlockClose)

function TradeRoadRewardView:LoadCallback()
    local show_list = TradeRoadData.Instance:GetRewardShowList()
    local money = TradeRoadData.Instance:GetRewardPrice()
    local data = show_list[1]
    for i, v in ipairs(show_list) do
        if money >= v.num_end then
            data = v
        else
            break
        end
    end
    self.levels = data.levels
    table.sort(show_list, DataHelper.SortFunc("levels", true))
    self.RewardList:SetData(show_list)
    self.RewardList:AutoSlide(data, false)
end

TradeRoadRewardItem = TradeRoadRewardItem or DeclareMono("TradeRoadRewardItem", UIWidgetBaseItem)
function TradeRoadRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local desc = Format(Language.TradeRoad.RewardDesc, DataHelper.ConverMoney(data.num_end))
    UH.SetText(self.LevelText, Format(Language.TradeRoad.RewardLevel, data.levels))
    UH.SetText(self.MoneyText, desc)

    local base_list = {}
    if data.coin > 0 then
        table.insert(base_list, {item_id = CommonItemId.Coin, num = data.coin})
    end
    table.insert(base_list, data.reward_item)
    self.ItemList:SetData(DataHelper.FormatItemList(base_list))

    UH.SpriteName(self.BG, self.RootView.levels == data.levels and "Tiao2" or "Tiao")
end
