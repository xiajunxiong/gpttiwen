
FILE_REQ("modules/sword_zither/sword_zither_config")
FILE_REQ("modules/sword_zither/sword_zither_data")
VIEW_REQ("modules/sword_zither/sword_zither_view")

SwordZitherCtrl = SwordZitherCtrl or BaseClass(BaseCtrl)

function SwordZitherCtrl:__init()
	if SwordZitherCtrl.Instance ~= nil then
		Debuger.LogError("[SwordZitherCtrl] attempt to create singleton twice!")
		return
	end	
	SwordZitherCtrl.Instance = self
	self.data = SwordZitherData.New()
	self:RegisterAllProtocols()
end

function SwordZitherCtrl:__delete()
	if SwordZitherCtrl.Instance == self then
		SwordZitherCtrl.Instance = nil
	end
	self:UnRegisterAllProtocols()
end

function SwordZitherCtrl:RegisterAllProtocols()
	self:RegisterProtocol(SCRAJianDanQinXinInfo, "OnRAJianDanQinXinInfo")
end

function SwordZitherCtrl:UnRegisterAllProtocols()
end

function SwordZitherCtrl:OnRAJianDanQinXinInfo(protocol)
	self.data:SetRAJianDanQinXinInfo(protocol)
	ServerActivityData.Instance:SetBonusNum(ACTIVITY_TYPE.RAND_SWORD_ZITHER,self.data:GetPassLayerNum(protocol.flag),1)
end