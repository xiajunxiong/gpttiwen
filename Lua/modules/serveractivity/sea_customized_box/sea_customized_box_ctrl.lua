--复刻->山海宝箱 ACTIVITY_TYPE.RAND_SEA_CUSTOMIZED_BOX
VIEW_REQ("modules/serveractivity/sea_customized_box/sea_customized_box_data")
VIEW_REQ("modules/serveractivity/sea_customized_box/sea_customized_box_view")

SeaCustomizedBoxCtrl = SeaCustomizedBoxCtrl or BaseClass(BaseCtrl)
function SeaCustomizedBoxCtrl:__init()
	if SeaCustomizedBoxCtrl.Instance ~= nil then
		Debuger.LogError("[SeaCustomizedBoxCtrl] attempt to create singleton twice!")
		return
	end
	SeaCustomizedBoxCtrl.Instance = self
	self.data = SeaCustomizedBoxData.New()
	
	self:RegisterAllProtocols()
end

function SeaCustomizedBoxCtrl:__delete()
	if SeaCustomizedBoxCtrl.Instance == self then
		SeaCustomizedBoxCtrl.Instance = nil
	end
end

function SeaCustomizedBoxCtrl:OnInit()
	Remind.Instance:Register(Mod.MountainSea.SeaCustomizedBox, self.data.buy_count_change, function()
    	return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SEA_CUSTOMIZED_BOX) and self.data:GetRemindNum() or 0
	end)
end

function SeaCustomizedBoxCtrl:RegisterAllProtocols()
	self:RegisterProtocol(SCRAShanHaiBaoXiangInfo, "OnCustomizedTreasureInfo")
end

function SeaCustomizedBoxCtrl:OnCustomizedTreasureInfo(protocol)
	self.data:OnProtocolData(protocol)
	-- ServerActivityData.Instance:SetBonusNum(ACTIVITY_TYPE.RAND_SEA_CUSTOMIZED_BOX, protocol.flag_num, #self.data:GetTreasureList())
end
