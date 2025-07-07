--复刻->云泽宝箱 ACTIVITY_TYPE.RAND_YUN_ZE_CUSTOMIZED_BOX
VIEW_REQ("modules/serveractivity/yun_ze_customized_box/yun_ze_customized_box_data")
VIEW_REQ("modules/serveractivity/yun_ze_customized_box/yun_ze_customized_box_view")

YunZeCustomizedBoxCtrl = YunZeCustomizedBoxCtrl or BaseClass(BaseCtrl)
function YunZeCustomizedBoxCtrl:__init()
	if YunZeCustomizedBoxCtrl.Instance ~= nil then
		Debuger.LogError("[YunZeCustomizedBoxCtrl] attempt to create singleton twice!")
		return
	end
	YunZeCustomizedBoxCtrl.Instance = self
	self.data = YunZeCustomizedBoxData.New()
	
	self:RegisterAllProtocols()
end

function YunZeCustomizedBoxCtrl:__delete()
	if YunZeCustomizedBoxCtrl.Instance == self then
		YunZeCustomizedBoxCtrl.Instance = nil
	end
end

function YunZeCustomizedBoxCtrl:OnInit()
	Remind.Instance:Register(Mod.YunZe.YunZeCustomizedBox, self.data.buy_count_change, function()
		return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_YUN_ZE_CUSTOMIZED_BOX) and self.data:GetRemindNum() or 0
	end)
end

function YunZeCustomizedBoxCtrl:RegisterAllProtocols()
	self:RegisterProtocol(SCRAYunZeBaoXiangInfo, "OnCustomizedTreasureInfo")
end

function YunZeCustomizedBoxCtrl:OnCustomizedTreasureInfo(protocol)
	self.data:OnProtocolData(protocol)
	-- ServerActivityData.Instance:SetBonusNum(ACTIVITY_TYPE.RAND_YUN_ZE_CUSTOMIZED_BOX, protocol.flag_num, #self.data:GetTreasureList())
end
