FILE_REQ("modules/shen_yu/shen_yu_data")

VIEW_REQ("modules/shen_yu/shen_yu_map")

-- ================= 神域SLG玩法 ===================
ShenYuCtrl = ShenYuCtrl or BaseClass(BaseCtrl)

function ShenYuCtrl:__init()
	if ShenYuCtrl.Instance ~= nil then
		Debuger.LogError("[ShenYuCtrl] attempt to create singleton twice!")
		return
	end
	ShenYuCtrl.Instance = self
	self.data = ShenYuData.New()
    self:RegisterAllProtocols()
end

function ShenYuCtrl:__delete()
    self:UnRegisterAllProtocols()
    ShenYuCtrl.Instance = nil
end

function ShenYuCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end

function ShenYuCtrl:RegisterAllProtocols()
    
end