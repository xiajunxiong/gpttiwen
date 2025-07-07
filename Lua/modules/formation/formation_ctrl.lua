
FILE_REQ("modules/formation/formation_config")
FILE_REQ("modules/formation/formation_info")
FILE_REQ("modules/formation/formation_data")

VIEW_REQ("modules/formation/formation_view")
VIEW_REQ("modules/formation/formation_pet_view")

FormationCtrl = FormationCtrl or BaseClass(BaseCtrl)

function FormationCtrl:__init()
	if FormationCtrl.Instance ~= nil then
		Debuger.LogError("[FormationCtrl] attempt to create singleton twice!")
		return
	end	
	FormationCtrl.Instance = self
	self.data = FormationData.New()

	self:RegisterAllProtocols()
end

function FormationCtrl:__delete()
	if FormationCtrl.Instance == self then
		FormationCtrl.Instance = nil
	end

	self:UnRegisterAllProtocols()
end

function FormationCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSFormationSimpleInfo)
	self:RegisterProtocol(CSFormationDetailInfo)
	self:RegisterProtocol(CSFormationRename)
	self:RegisterProtocol(CSFormationSave)
	self:RegisterProtocol(CSFormationDelete)

	self:RegisterProtocol(SCFormationSimpleInfo, "OnFormationSimpleInfo")
	self:RegisterProtocol(SCFormationDetailInfo, "OnFormationDetailInfo")
	self:RegisterProtocol(SCFormationRename, "OnFormationRename")
end

function FormationCtrl:UnRegisterAllProtocols()
end

function FormationCtrl:OnFormationSimpleInfo(protocol)
	self.data:SetFormationSimpleInfo(protocol)
end

function FormationCtrl:OnFormationDetailInfo(protocol)
	self.data:SetFormationDetailInfo(protocol)
end

function FormationCtrl:OnFormationRename(protocol)
	self.data:SetFormationRename(protocol)
end

function FormationCtrl:SendFormationSimpleInfo()
	local protocol = GetProtocol(CSFormationSimpleInfo)
	SendProtocol(protocol)
end

function FormationCtrl:SendFormationDetailInfo(formation_seq)
	local protocol = GetProtocol(CSFormationDetailInfo)
	protocol.formation_seq = formation_seq or 0
	SendProtocol(protocol)
end

function FormationCtrl:SendFormationRename(formation_seq, new_name)
	local protocol = GetProtocol(CSFormationRename)
	protocol.formation_seq = formation_seq or 0
	protocol.new_name = new_name or ""
	SendProtocol(protocol)
end

function FormationCtrl:SendFormationSave(is_save_new_format, formation, formation_seq)
	if formation == nil then return end
	local protocol = GetProtocol(CSFormationSave)
	local detail = formation:FormationIntro()
	protocol.is_save_new_format = is_save_new_format or 0
	protocol.formation_seq = formation_seq or formation:Seq() or 0
	protocol.role_row = detail.role_row or 0
	protocol.pet_idx = detail.pet_idx or -1
	protocol.partner_list = detail.partner_list or {}
	protocol.member_list = detail.member_list or {}
	protocol.auto_info = detail.auto_info or {}
	SendProtocol(protocol)
end

function FormationCtrl:SendFormationSaveNew(formation)
	self:SendFormationSave(1, formation)
end

function FormationCtrl:SendFormationSaveComplete(formation)
	self:SendFormationSave(0, formation, 0)
end

function FormationCtrl:SendFormationDelete(formation_seq)
	local protocol = GetProtocol(CSFormationDelete)
	protocol.formation_seq = formation_seq or 0
	SendProtocol(protocol)
end

function FormationCtrl:FlushFormation0()
	if ViewMgr:IsOpen(FormationView) then
		self.data:FlushFormation0()
	end
end