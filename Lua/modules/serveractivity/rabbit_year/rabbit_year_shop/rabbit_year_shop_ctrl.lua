VIEW_REQ("modules/serveractivity/rabbit_year/rabbit_year_shop/rabbit_year_shop_view")

RabbitYearShopCtrl = RabbitYearShopCtrl or BaseClass(BaseCtrl)

RabbitYearShopCtrl.ReqType = {
    Info = 0,
    Buy1 = 1,   -- 直购购买
    Buy2 = 2,   -- 普通购买
}

function RabbitYearShopCtrl:__init()
	if RabbitYearShopCtrl.Instance ~= nil then
		Debuger.LogError("[RabbitYearShopCtrl] attempt to create singleton twice!")
		return
	end
	RabbitYearShopCtrl.Instance = self
	self.data = RabbitYearShopData.New()
    self:RegisterAllProtocols()
end

function RabbitYearShopCtrl:__delete()
    RabbitYearShopCtrl.Instance = nil
end

function RabbitYearShopCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end

function RabbitYearShopCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCFuYunShangChengInfo, "OnSCFuYunShangChengInfo")
end

function RabbitYearShopCtrl:OnSCFuYunShangChengInfo(protocol)
    self.data.act_info:Set(protocol.info)
end


RabbitYearShopData = RabbitYearShopData or BaseClass()

function RabbitYearShopData:__init()
    if RabbitYearShopData.Instance ~= nil then
        Debuger.LogError("[RabbitYearShopData] attempt to create singleton twice!")
        return
    end
    RabbitYearShopData.Instance = self
    self.act_info = SmartData.New()
    self.act_type = ACTIVITY_TYPE.RAND_FU_YUN_SHANG_CHENG
end

function RabbitYearShopData:ClearData()
    self.act_info:Clear()
end

function RabbitYearShopData:GetShopItemList()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(self.act_type,Config.fuyunshangcheng_auto.basis_configuration,"seq")
    local open_day = TimeCtrl.Instance:GetActOpenDay(self.act_type)
    local list = {}
    for i,v in pairs(cfg) do
        local end_day = v.end_time == 0 and 10000 or v.end_time
        if open_day >= v.start_time and open_day <= end_day then
            table.insert(list, v)
        end
    end
    return list
end

-- 获取剩余购买次数
function RabbitYearShopData:GetRemainBuyTiems(data)
    if TableIsEmpty(self.act_info) then
        return 0
    end
    local buy_times = 0
    if data.limit_type == 2 then
        buy_times = self.act_info.day_buy_times[data.seq]
        return data.day_times - buy_times
    else
        buy_times = self.act_info.act_buy_times[data.seq]
        return data.limit_times - buy_times
    end
end