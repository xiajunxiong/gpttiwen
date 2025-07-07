
RoleSelectData = RoleSelectData or BaseClass()

function RoleSelectData:__init()
	if RoleSelectData.Instance ~= nil then
		Debuger.LogError("[RoleSelectData] attempt to create singleton twice!")
		return
	end
    RoleSelectData.Instance = self
	
	self.role_select_data = SmartData.New({
		cur_select_role_id = 0,
		cur_select_role_data = {},
    })
end

function RoleSelectData:GetSelectRoleId()
	return self.role_select_data.cur_select_role_id
end

function RoleSelectData:SetSelectRoleData(role_data)
	self.role_select_data.cur_select_role_data = role_data
end

function RoleSelectData:GetSelectRoleData()
	return self.role_select_data.cur_select_role_data
end