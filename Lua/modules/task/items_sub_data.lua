ItemsSubData = ItemsSubData or BaseClass()

function ItemsSubData:__init()
    if ItemsSubData.Instance ~= nil then
        Debuger.LogError("[ItemsSubData] attempt to create singleton twice!")
        return
    end
    ItemsSubData.Instance = self

    self:ItemsSubData()
    self.later_talk = 0
end

function ItemsSubData:ItemsSubData()
    self.sub_items = SmartData.New()
    self.sub_currency = SmartData.New()
end

function ItemsSubData:SetSubInfo(items, sub_func)
    self.sub_items:Set(items)
    self.sub_func = sub_func
end

function ItemsSubData:SetSubCurrencyInfo(currency, sub_func)
    self.sub_currency:Set(currency)
    self.sub_currency_func = sub_func
end