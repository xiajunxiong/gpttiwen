PeriodActivityStoreData = PeriodActivityStoreData or BaseClass()

function PeriodActivityStoreData:__init()
    if PeriodActivityStoreData.Instance ~= nil then
        Debuger.LogError("[PeriodActivityStoreData] attempt to create singleton twice!")
        return
    end
    PeriodActivityStoreData.Instance = self
    self.wen_cang_shop_data = SmartData.New()
end

function PeriodActivityStoreData:ClearData()
    self.wen_cang_shop_data:Set({})
end


-- 获取文昌书局商品List
function PeriodActivityStoreData:GetWenCangShopList()
    return Config.study_market_auto.store_config
end

function PeriodActivityStoreData:GetWenCangItemId()
    return Config.study_market_auto.other[1].play_coin
end