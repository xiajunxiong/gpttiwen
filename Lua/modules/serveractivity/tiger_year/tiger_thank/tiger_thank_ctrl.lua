VIEW_REQ("modules/serveractivity/tiger_year/tiger_thank/tiger_thank_view")

TigerThankCtrl = TigerThankCtrl or BaseClass(BaseCtrl)

function TigerThankCtrl:__init()
	if TigerThankCtrl.Instance ~= nil then
		Debuger.LogError("[TigerThankCtrl] attempt to create singleton twice!")
		return
	end	
	TigerThankCtrl.Instance = self
	self.data = TigerThankData.New()
	self:RegisterAllProtocols()

	Remind.Instance:Register(Mod.TigerYear.TigerThank, self.data.tiger_thank_data, function ()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_TIGER_THANK) then
			return 0
		end
		return self.data:RemindNum() 
    end)
end

function TigerThankCtrl:__delete()
	self:UnRegisterAllProtocols()
	TigerThankCtrl.Instance = nil
	Delete(self, "data")
end

function TigerThankCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAYinHuBaiSuiInfo, "OnSCRAYinHuBaiSuiInfo")
end

function TigerThankCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCRAYinHuBaiSuiInfo)
end

function TigerThankCtrl:OnInit()

end

function TigerThankCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function TigerThankCtrl:OnSCRAYinHuBaiSuiInfo(protocol)
	self.data:SetInfo(protocol)
end

-- 发送请求
-- opera_type：0 请求下发
-- opera_type：1 直购
-- opera_type：2 领取奖励 ，p1：seq
function TigerThankCtrl:Send(opera_type, param_1)
	if opera_type == 1 then
		local cfg = self.data:GetBuyConfig() or {price = 0}
		local fee = cfg.price / 10
		ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_TIGER_THANK, fee, ACTIVITY_TYPE.RAND_TIGER_THANK, opera_type, param_1)
	else
		ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_TIGER_THANK, opera_type, param_1)
	end
end


--------------------TigerThankData--------------------
TigerThankData = TigerThankData or BaseClass()

function TigerThankData:__init()
	if TigerThankData.Instance ~= nil then
		Debuger.LogError("[TigerThankData] attempt to create singleton twice!")
		return
	end
	TigerThankData.Instance = self
	self.calculate_frame_count = 0
	self.open_day = 1
    self.tiger_thank_data = SmartData.New()
    self:ResetData()
end

function TigerThankData:ResetData()
    self:ResetTigerThankData()
end

function TigerThankData:ResetTigerThankData()
    self.tiger_thank_data:Set({
        info = {
			is_buy = 0,
			receive_flag = {},
		},
    })
end

function TigerThankData:SetInfo(protocol)
    self.tiger_thank_data.info = protocol.info
end

function TigerThankData:GetInfo()
	return self.tiger_thank_data.info
end

function TigerThankData:RemindNum()
	if self.tiger_thank_data.info.is_buy > 0 then
		for k, v in pairs(self:GetRewardConfig()) do
			if self:OpenDay() >= v.activity_days and self:GetReceiveFlag(v.seq) == 0 then
				return 1
			end
		end
	end
    return 0
end

function TigerThankData:GetRewardConfig()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_TIGER_THANK, Config.yinhubaisui_auto.gift_configure, "seq")
end

function TigerThankData:GetBuyConfig()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_TIGER_THANK, Config.yinhubaisui_auto.buy_configure)[1]
end

function TigerThankData:OpenDay()
	if self.calculate_frame_count ~= Time.frameCount then
		self.calculate_frame_count = Time.frameCount
		self.open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_TIGER_THANK)
	end
	return self.open_day
end

function TigerThankData:GetReceiveFlag(seq)
	if TableIsEmpty(self.tiger_thank_data.info.receive_flag) then
		return 0
	end
	return self.tiger_thank_data.info.receive_flag[seq] and self.tiger_thank_data.info.receive_flag[seq] or 0
end