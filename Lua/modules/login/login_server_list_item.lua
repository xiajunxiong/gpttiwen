LoginServerListItem = DeclareMono("LoginServerListItem",UIWidgetBaseItem)
---ServerName---------Text
---ServerState--------Image

-- function LoginServerListItem:OnSelect()
-- 	-- Debuger.LogError("Item---OnSelect!!!!")
	
-- end


function LoginServerListItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	-- LogError("image name",SERVER_STATE[data.flag])
	UH.SpriteName(self.ServerState, SERVER_STATE[data.flag])
	if IS_EDITOR then
		self.ServerName.text = string.format(Language.Login.ServerNameEditor, data.id, data.name)
	else
		self.ServerName.text = string.format(Language.Login.ServerName,data.name)--,data.id,data.name)
	end
	local platform_name = LoginCtrl.Instance.data:GetPlatformName()
	-- LogError("LoginServerListItem platform_name",platform_name)
	local level = UnityPlayerPrefs.GetInt(data.id.. platform_name .. "role_level")
	local avatar = UnityPlayerPrefs.GetInt(data.id.. platform_name .. "avatar_type")
	local avatar_id = UnityPlayerPrefs.GetInt(data.id.. platform_name .. "avatar_id")
	local avatar_quality = UnityPlayerPrefs.GetInt(data.id.. platform_name .. "avatar_quality")
	local top_level = UnityPlayerPrefs.GetInt(data.id.. platform_name .. "top_level")
	if level == 0 or avatar == 0 then
		self.avatarOj:SetActive(false)
	else
		self.avatarOj:SetActive(true)
		if top_level and top_level ~= 0 then
			self.level.text = string.format(Language.PeakRoad.Level2,top_level)
		else
			self.level.text = string.format("Lv.%s",level)
		end
		self.avatar:SetAvatar(avatar,avatar_id,avatar_quality)
	end
	self.timer:SetActive(GLOBAL_CONFIG:ServerInfo().server_time < data.open_time)
	self.NewTag:SetActive(data.flag == 1)
	if GLOBAL_CONFIG:ServerInfo().server_time < data.open_time then
		UH.SetText(self.OpenTimeNeo, os.date(Language.UiTimeMeter.TimeStempStr, data.open_time)..Language.Login.Open)
	else
		UH.SetText(self.OpenTimeNeo,"")
	end
	if LoginData.Instance:GetLastLoginServer() == data.id  
		and avatar == RoleData.Instance:GetRoleAvatarType() then 
			self.level.text = string.format("Lv.%s",RoleData.Instance:GetRoleLevel())
	end
end

function LoginServerListItem:SelectEventData()
	return self:GetData()
end

LoginServerTypeItem = DeclareMono("LoginServerTypeItem",UIWidgetBaseItem)

function LoginServerTypeItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	self.TypeName.text = data.type_name 
	self.TypeName2.text = data.type_name
end

function LoginServerTypeItem:SelectEventData()
	return self:GetData()
end

function LoginServerTypeItem:Click()
	self.ClickEvent:Trigger()
end