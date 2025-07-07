
FILE_REQ("modules/role_info/role_info_config")
FILE_REQ("modules/role_info/role_info_data")

VIEW_REQ("modules/role_info/role_info_view")
VIEW_REQ("modules/role_info/role_info_round_view")
VIEW_REQ("modules/role_info/role_info_details_view")
VIEW_REQ("modules/role_info/role_info_equip_view")
VIEW_REQ("modules/role_info/role_info_details_pet")
VIEW_REQ("modules/role_info/role_info_details_partner")
VIEW_REQ("modules/role_info/role_info_accrssory_view")
VIEW_REQ("modules/role_info/role_info_details_bicycle")
VIEW_REQ("modules/role_info/role_info_details_sheng_qi")

RoleInfoCtrl = RoleInfoCtrl or BaseClass(BaseCtrl)

function RoleInfoCtrl:__init()
	if RoleInfoCtrl.Instance ~= nil then
		Debuger.LogError("[RoleInfoCtrl] attempt to create singleton twice!")
		return
	end

	RoleInfoCtrl.Instance = self
	self.data = RoleInfoData.New()
	
	self:RegisterAllProtocols()
end

function RoleInfoCtrl:__delete()
	if RoleInfoCtrl.Instance == self then
		RoleInfoCtrl.Instance = nil
	end

	self:UnRegisterAllProtocols()
end

function RoleInfoCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSReferRoleEquipment)

	self:RegisterProtocol(SCRoleEquipmentInfor, "OnRoleEquipmentInfor")
	self:RegisterProtocol(SCQueryRoleFollowUpInfo,"OnQueryRoleFollowUpInfo")
end

function RoleInfoCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCRoleEquipmentInfor)
	self:UnRegisterProtocol(SCQueryRoleFollowUpInfo)
end

function RoleInfoCtrl:OnRoleEquipmentInfor(protocol)
	self.data:SetRoleEquipmentInfor(protocol)
end

function RoleInfoCtrl:OnQueryRoleFollowUpInfo(protocol)
	self.data:SetRoleEquipmentRank(protocol)
	if self.data.role_equips.role_id == self.data.view_equip_uid then
		self.data.view_equip_uid = 0
		ViewMgr:OpenView(RoleInfoDetailsView)
	end
end

--查询玩家信息 获取玩法信息RoleInfoCtrl.Instance.data:GetQueryRoleInfo(role_id)
function RoleInfoCtrl:SendReferRoleEquipment(role_id)
	local protocol = GetProtocol(CSReferRoleEquipment)
	protocol.role_id = role_id or 0
	protocol.query = 0
	SendProtocol(protocol)
end
-- info = {
-- 	info = function()
-- 		return RoleInfoNew
-- 	end,
-- 	funcs = function()
-- 		return {"func1", "func2"}
-- 	end,
-- 	hide_base = bool,		默认为nil
-- 	pos = Vector3			pos不传默认自适应
-- }
-- 注意:即使hide_base,info还是要的,按钮交互时要用到

function RoleInfoCtrl:RoleInfoView(info)
	self.data:SetRoleInfo(info)
	ViewMgr:OpenView(RoleInfoView)
end

function RoleInfoCtrl:RoleInfoRoundView(info)
	self.data:SetRoleRoundInfo(info)
	ViewMgr:OpenView(RoleInfoRoundView)
end