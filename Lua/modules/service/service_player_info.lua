ServicePlayerInfo = ServicePlayerInfo or DeclareMono("ServicePlayerInfo",UIWFlushPanel)
function ServicePlayerInfo:ServicePlayerInfo()
	self.data = ServicePlayerInfoData.Instance
end
function ServicePlayerInfo:Awake()
	UIWFlushPanel.Awake(self)
end

function ServicePlayerInfo:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self:FlushPanel()
end

function ServicePlayerInfo:FlushPanel()
	self:FlushPlayerInfo()
	self:FlushGameInfo()
end

function ServicePlayerInfo:FlushPlayerInfo(server_info)
	local server_info = server_info or self.data:GetCurServerInfo()
	if server_info == nil then
		UH.SetText(self.NameText,Language.Service.NotLogin)
		UH.SetText(self.LevelText,Language.Service.NotLogin)
		UH.SetText(self.IDText,Language.Service.NotLogin)
		UH.SetText(self.ServerText,Language.Service.NotLogin)
		return
	end
	self.role_name = server_info.role_name
	self.level = server_info.level
	self.role_id = server_info.role_id
	if IS_EDITOR then
		self.server_name = string.format(Language.Login.ServerNameEditor, server_info.id,server_info.name)
	else
		self.server_name = string.format(Language.Login.ServerName,server_info.name)--,server_info.id,server_info.name)
	end
	UH.SetText(self.NameText,server_info.role_name)
	UH.SetText(self.LevelText,server_info.level .. "")
	UH.SetText(self.IDText,server_info.role_id .. "")
	UH.SetText(self.ServerText,self.server_name)
	local platform_name = LoginCtrl.Instance.data:GetPlatformName()
	local avatar_id = UnityPlayerPrefs.GetInt(server_info.id.. platform_name .. "avatar_id")
	local avatar = UnityPlayerPrefs.GetInt(server_info.id.. platform_name .. "avatar_type")
	local avatar_quality = UnityPlayerPrefs.GetInt(server_info.id.. platform_name .. "avatar_quality")
	self.Avater:SetAvatar(avatar,avatar_id,avatar_quality)
end

function ServicePlayerInfo:FlushGameInfo()
	local url_data = ServiceData.Instance:GetUrlData()
	-- LogError("info url_data",url_data)
	for i=1,3 do
		if url_data[i] == nil then
			UH.SetText(self.ValueTextArray[i],"")
			self.TitleTextArray[i]:SetObjActive(false)
		else
			UH.SetText(self.ValueTextArray[i], url_data[i].value)
			self.TitleTextArray[i]:SetObjActive(true)
			UH.SetText(self.TitleTextArray[i], Language.Service.ServiceInfoTypeTitle[url_data[i].index])
		end
	end
	-- UH.SetText(self.WeChatText,url_data.wx_id .. "")
	-- UH.SetText(self.QQText,url_data.qq .. "")
	-- UH.SetText(self.GameNetText,url_data.gamenet)
	UH.SetText(self.Desc,url_data.gs_content)
	local we_chat_img = self.data:GetWeChatImg()
	if we_chat_img then
		self.WeChatRaw.texture = we_chat_img
	end
end

function ServicePlayerInfo:OnChangeServerClick()
	ViewMgr:OpenView(ServerView)
	-- self.ServerSelectObj:SetActive(true)
	-- self.ServerList:SetData(self.data:GetLoginedAllServer())
end

function ServicePlayerInfo:OnChangeUserClick()
	LoginCtrl.Instance:HintToReturnLogin(Language.RoleSetting.ChangeId, true , ServiceView)
    EventSys:Fire(GlobalEventKeys.TOUCH_BEGAN)
end

function ServicePlayerInfo:OnCopyInfoClick()
	if self.role_name ~= nil and self.role_id ~= nil then
		local str = string.format(Language.Service.CopyRoleInfo,self.server_name,self.role_name,self.role_id,self.level)
		self:Copy(str)
	else
		PublicPopupCtrl.Instance:Center(Language.Service.CopyRoleTip1)
	end
end

function ServicePlayerInfo:OnCopyWeChatClick()
	local url_data = ServiceData.Instance:GetUrlData()
	-- self:Copy(url_data.wx_id)
	self:Copy(url_data[1].value)
end

function ServicePlayerInfo:OnCopyGameNetClick()
	local url_data = ServiceData.Instance:GetUrlData()
	-- self:Copy(url_data.gamenet)
	self:Copy(url_data[2].value)
end

function ServicePlayerInfo:OnCopyQQClick()
	local url_data = ServiceData.Instance:GetUrlData()
	-- self:Copy(url_data.qq)
	self:Copy(url_data[3].value)
end

function ServicePlayerInfo:Copy(str)
	UnityEngine.GUIUtility.systemCopyBuffer = str
    PublicPopupCtrl.Instance:Center(Language.Role.CopyIdSuccess)
end

function ServicePlayerInfo:OnSelectServerClick(data)
	self.data:SetCurServerInfo(data)
	--self:FlushPlayerInfo(data)
	self:OnSelectServer(data)
	self.ServerSelectObj:SetActive(false)
	ViewMgr:CloseView(ServiceView)
end

function ServicePlayerInfo:OnCloseServerSelect()
	self.ServerSelectObj:SetActive(false)
end

function ServicePlayerInfo:OnSelectServer(selected_data)
	local old_server = LoginData.Instance.view_sel_server_id.server_id > 0 
			and LoginData.Instance.view_sel_server_id.server_id or LoginData.Instance:GetLastLoginServer()
	LoginData.Instance.view_sel_server_id.server_id = selected_data.id
	ViewMgr:FlushView(WelcomeView, WelcomeView.FK.Server)
	ViewMgr:CloseView(ServerView)
	-- LogError("OnSelectServer ",LoginData.Instance.view_sel_server_id.account_spid)
	if SceneCtrl.Instance:CurScene():GetClassType() ~= LoginScene 
		and old_server ~= LoginData.Instance.view_sel_server_id.server_id then

		LoadingCtrl.Instance:AutoPercent(function()
				local judge_scene = SceneCtrl.Instance:CurScene() ~= nil and SceneCtrl.Instance:CurScene() or SceneCtrl.Instance:NextScene()
				return judge_scene and judge_scene:LoadingComplete()
			end)
		-- ViewMgr:OpenView(LoadingView)
		Runner.Instance:RunUntilTrue(function ()
			if ViewMgr:IsOpen(LoadingView) then
				LoginData.Instance:SaveLastLoginServer(LoginData.Instance:GetAccountKey(),LoginData.Instance.view_sel_server_id.server_id)
				LoginCtrl.Instance:ReloginQuietly()
				LoginData.Instance:SetLoginBackType(RELOGIN_TYPE.ROLE_SELECT)
				return true
			else
				return false
			end
		end)
	end
end

ServerInfoItem = ServerInfoItem or DeclareMono("ServerInfoItem",UIWidgetBaseItem)

function ServerInfoItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	if StringIsEmpty(data.level) then
		self:SetObjActive(false)
		return
	end
	local platform_name = LoginCtrl.Instance.data:GetPlatformName()
	local avatar_id = UnityPlayerPrefs.GetInt(data.id.. platform_name .. "avatar_id")
	local avatar = UnityPlayerPrefs.GetInt(data.id.. platform_name .. "avatar_type")
	self.Avater:SetAvatar(avatar,avatar_id)
	UH.SetText(self.RoleName,data.role_name)
	UH.SetText(self.LevelText,data.level .. "")
	if IS_EDITOR then
		UH.SetText(self.ServerName,string.format(Language.Login.ServerNameEditor, data.id, data.name))
	else
		UH.SetText(self.ServerName,string.format(Language.Login.ServerName, data.name))--,data.id,data.name))
	end
end