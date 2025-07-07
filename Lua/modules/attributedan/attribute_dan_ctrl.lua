FILE_REQ("modules/attributedan/attribute_dan_data")
VIEW_REQ("modules/attributedan/attribute_dan_view")
VIEW_REQ("modules/attributedan/attribute_zonglan_view")

AttributeDanCtrl = AttributeDanCtrl or BaseClass(BaseCtrl)

function AttributeDanCtrl:__init()
	if AttributeDanCtrl.Instance ~= nil then
		Debuger.LogError("[AttributeDanCtrl] attempt to create singleton twice!")
		return
	end	
	AttributeDanCtrl.Instance = self
	self.data = AttributeDanData.New()
	
	self:RegisterProtocol(SCRoleAttrPelletAllInfo, "RecvRoleAttrPelletAllInfo")
    self:RegisterProtocol(SCRoleAttrPelletSingleInfo, "RecvRoleAttrPelletSingleInfo")
    self:RegisterProtocol(CSRoleAttrPelletReq) 
	
	-- 角色属性页签红点
	local remind_func = BindTool.Bind(self.AllRemind, self)
	RemindCtrl.Instance:Remind():Register(Mod.Role.RoleAttr, 
		AttributeDanData.Instance.attribute_red_piont, remind_func)
	RemindCtrl.Instance:Remind():Register(Mod.Role.RoleAttr, 
		IllusionData.Instance.red_points, remind_func)
end

function AttributeDanCtrl:__delete()
	self.data = nil
	AttributeDanCtrl.Instance = nil
end

-- 角色属性页签红点
function AttributeDanCtrl:AllRemind()
	if self:GetRedAttibutePiontNum() > 0 then
		-- 属性丹红点
		return 1
	elseif IllusionData.Instance:GetRemind() > 0 then
		-- 霓裳红点
		return 1
	end
	return 0
end

function AttributeDanCtrl:GetRedAttibutePiontNum()
	return self.data.attribute_red_piont.count
end

function AttributeDanCtrl:RecvRoleAttrPelletAllInfo(protocol)
	self.data.role_use_attr_list:Set(protocol.role_use_attr_list)
	self.data.partner_use_attr_list:Set(protocol.partner_use_attr_list)
	self.data:RecvRoleAttrPelletAllInfo(protocol)
end

function AttributeDanCtrl:RecvRoleAttrPelletSingleInfo(protocol)
	self.data:ChangeAttributeUseList(protocol)
end

function AttributeDanCtrl:SendRoleAttrPelletReq(m_type,param1,param2,param3)
	local protocol = GetProtocol(CSRoleAttrPelletReq)
	protocol.op_type = m_type
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	protocol.param3 = param3 or 0
	SendProtocol(protocol)
end








