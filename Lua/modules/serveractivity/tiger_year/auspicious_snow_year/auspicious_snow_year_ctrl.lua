VIEW_REQ("modules/serveractivity/tiger_year/auspicious_snow_year/auspicious_snow_year_view")

AuspiciousSnowYearCtrl = AuspiciousSnowYearCtrl or BaseClass(BaseCtrl)

function AuspiciousSnowYearCtrl:__init()
	if AuspiciousSnowYearCtrl.Instance ~= nil then
		Debuger.LogError("[AuspiciousSnowYearCtrl] attempt to create singleton twice!")
		return
	end	
	AuspiciousSnowYearCtrl.Instance = self
	self.data = AuspiciousSnowYearData.New()
	self:RegisterAllProtocols()

	Remind.Instance:Register(Mod.TigerYear.AuspiciousSnow, self.data.auspicious_snow_data, function ()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_AUSPICIOUS_SNOW_YEAR) then
			return 0
		end
		return self.data:RemindNum() 
    end)	
end

function AuspiciousSnowYearCtrl:__delete()
	self:UnRegisterAllProtocols()
	AuspiciousSnowYearCtrl.Instance = nil
	Delete(self, "data")
end

function AuspiciousSnowYearCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRATigerAuspiciousSnowInfo, "OnTigerAuspiciousSnowInfo")
end

function AuspiciousSnowYearCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCRATigerGiftMoneyInfo)
end

function AuspiciousSnowYearCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function AuspiciousSnowYearCtrl:OnTigerAuspiciousSnowInfo(protocol)
	self.data:SetInfo(protocol)
end

-- 发送请求
-- opera_type：0 请求下发
-- opera_type：1 雪球奖励
-- opera_type：2 集字奖励
function AuspiciousSnowYearCtrl:Send(opera_type)
	ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_AUSPICIOUS_SNOW_YEAR, opera_type)
end

--------------------AuspiciousSnowYearData--------------------
AuspiciousSnowYearData = AuspiciousSnowYearData or BaseClass()

function AuspiciousSnowYearData:__init()
	if AuspiciousSnowYearData.Instance ~= nil then
		Debuger.LogError("[AuspiciousSnowYearData] attempt to create singleton twice!")
		return
	end
	AuspiciousSnowYearData.Instance = self
    self.auspicious_snow_data = SmartData.New({times = 0,is_receive = false})	
    self:ResetData()
	self:FlushShowList()
end

function AuspiciousSnowYearData:ResetData()
	
end

function AuspiciousSnowYearData:GetShowItemCfg()
	local show_item = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_AUSPICIOUS_SNOW_YEAR,Config.ruixuefengnian_auto.recharge_configure)[1].reward_item
	return show_item
end

function AuspiciousSnowYearData:GetRewardShowDataCfg()
	local data = Config.ruixuefengnian_auto.other[1]
	if not self.show_list then
		self.show_list = {}
		table.insert(self.show_list,{item_id = data.consume_item_id_1})
		table.insert(self.show_list,{item_id = data.consume_item_id_2})
		table.insert(self.show_list,{item_id = data.consume_item_id_3})
		table.insert(self.show_list,{item_id = data.consume_item_id_4})
	end
	return self.show_list
end

function AuspiciousSnowYearData:GetRewardData()
	return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_AUSPICIOUS_SNOW_YEAR,Config.ruixuefengnian_auto.gift_configure)
end

function AuspiciousSnowYearData:GetLuckDrawData()
	return self:GetRewardData()[self.auspicious_snow_data.draw_result + 1]
end

function AuspiciousSnowYearData:RemindNum()
	if Item.GetNum(self:GetOtherData().lucky_draw_item_id) > 0 then
		return 1
	end
	if self.auspicious_snow_data.is_receive then
		return 1
	end
	return self.auspicious_snow_data.times
end

function AuspiciousSnowYearData:SetInfo(protocol)
	self.auspicious_snow_data.draw_result = -1
	self.auspicious_snow_data.times = self:GetOtherData().collect_times - protocol.times
	self.auspicious_snow_data.draw_result = protocol.draw_result
end
function AuspiciousSnowYearData:FlushShowList()
	self.auspicious_snow_data.is_receive = true
	for i,v in pairs(self:GetRewardShowDataCfg()) do
		v.num = Item.GetNum(v.item_id)
		v.is_has = v.num > 0 and true or false
		if v.is_has == false then
			self.auspicious_snow_data.is_receive = false
		end
	end
	self.auspicious_snow_data:SetDirty("is_receive")
end

function AuspiciousSnowYearData:GetOtherData()
	return Config.ruixuefengnian_auto.other[1]
end

function AuspiciousSnowYearData:IsSnowGiftCount(id)
	if Config.ruixuefengnian_auto.scene_list[1].npc_id == id then		
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_AUSPICIOUS_SNOW_YEAR) then
			PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
			return true
		end
		if self.auspicious_snow_data.times <= 0 then
			PublicPopupCtrl.Instance:Center(Language.AuspiciousSnowYear.GoToTip)
			return true
		end
	end
end