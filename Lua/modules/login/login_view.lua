LoginView = LoginView or DeclareView("LoginView","login/login",0,ViewLayer.Top)
VIEW_DECLARE_MASK(LoginView,ViewMask.None)

function LoginView:LoadCallback()
	-- self.login_callback = login_callback
	self.lang_tab = Language.DevLogin
	if GLOBAL_CONFIG:PkgInfo().root_pkg then
		ViewMgr:OpenView(RootAgentQueryView)
	end
end

function LoginView:OnLoginClick()

	 local account_str = self.account_input.text
	 local password_str = self.password_input.text

	 if StringIsEmpty(account_str) then
	 	PublicPopupCtrl.Instance:Center(self.lang_tab.EmtpyAccount)
	 	return
	 elseif StringIsEmpty(password_str) then
	 	PublicPopupCtrl.Instance:Center(self.lang_tab.EmtpyPassword)
	 	return
	 end
	 if ChannelAgent ~= nil then
		LoginData.Instance:SaveLastLoginAccount(account_str,password_str)
		local data = { AccountId = account_str, Token = password_str}
	 	ChannelAgent:LoginCallback(data)
	 end
	 
end

function LoginView:OnZhuceClick()
	 local url = GLOBAL_CONFIG:Urls().ZhuCe_url
	 Application.OpenURL(url);
end

function LoginView:OnFlush()
	local showacc = LoginData.Instance:GetLastLoginAccount()
	if not StringIsEmpty(showacc) then
		self.account_input.text = showacc
		if not GLOBAL_CONFIG:PkgInfo().root_pkg then
			self.password_input.text = LoginData.Instance:GetPassword(showacc)
		end
	end
end

--------------------------------------
RootAgentQueryView = RootAgentQueryView or DeclareView("RootAgentQueryView","login/root_agent_query_view",0,ViewLayer.Top + 1)
VIEW_DECLARE_MASK(LoginView,ViewMask.None)
function RootAgentQueryView:RootAgentQueryView()
end

function RootAgentQueryView:OnClickOk()
	ROOT_PKG_REQ_AGENT_ID = self.Input.text
	local url = self:GetSpecialQueryUrl()
	WebRequester:Request(url,function(data,error)
		self.requesting = false
		if StringIsEmpty(error) == true then
			local remote_config,suc = TryParseJson(data)
			if suc == true then
				GLOBAL_CONFIG:SetUrls(remote_config.param_list)
				GLOBAL_CONFIG:SetRemotePkgInfo(remote_config.version_info)
				GLOBAL_CONFIG:SetFlags(remote_config.param_list.switch_list)
				GLOBAL_CONFIG:SetReminderInfo(remote_config.param_list.reminder_info)
				remote_config.server_info.account_spid = StringIsEmpty(remote_config.server_info.account_spid) and GLOBAL_CONFIG:PkgInfo().agent_id or remote_config.server_info.account_spid
				GLOBAL_CONFIG:SetServerInfo(remote_config.server_info)
				GLOBAL_CONFIG:SetServerInfoOther(0, remote_config.server_info)
				GLOBAL_CONFIG.client_time = Time.realtimeSinceStartup
				for i=1,3 do
					if remote_config["server_info"..tostring(i)] ~= nil then
						GLOBAL_CONFIG:SetServerInfoOther(i,remote_config["server_info"..tostring(i)])
					else
						break
					end
				end
				GLOBAL_CONFIG:SaveConfig()
				ViewMgr:CloseView(RootAgentQueryView)
			else	--remote data error
				PreloadData.Instance:SetConnectRetryCount(PreloadData.Instance:GetConnectRetryCount() + 1)
				self.retry_wait = retry_wait_frame_count
				UnityEngine.Debug.LogError("RootPgk Init query data Json parse failed,url=" .. url)
			end
		else		--remote request failed
			PreloadData.Instance:SetConnectRetryCount(PreloadData.Instance:GetConnectRetryCount() + 1)
			self.retry_wait = retry_wait_frame_count
			UnityEngine.Debug.LogError("RootPgk Init query data require failed:" .. tostring(error) .. ",url=" .. url)
		end
	end)
end

function RootAgentQueryView:GetSpecialQueryUrl()
	local url_config = GLOBAL_CONFIG:Urls()
	local pkg_config = GLOBAL_CONFIG:PkgInfo()

	local php_url = StringIsEmpty(url_config.config_url) == false and url_config.config_url or url_config.init_url
	local version = pkg_config.is_youyan and "999" or "99999"
	local game_name = VersionTag.HasTag(VersionTag.PlatformNative) and Nirvana.PlatformNative.getProductName() or UnityApp.productName

	local uuid = GLOBAL_CONFIG:PkgInfo().device_id
	if VersionTag.HasTag(VersionTag.FileInfoJson) then
		local temp = MainProber.GetUUID()
		if not IS_IOS() then
			uuid = temp
		else
			if tonumber(string.sub(temp,1,8)) ~= nil and tonumber(string.sub(temp,1,8)) == 0 then
			else
				uuid = temp
			end
		end
	end
	local url = string.format("%s?plat=%s&version=%s&asset=%s&device=%s&isrestart=%d&pkg=%s&channelId=%s&game_name=%s&pkg_name=%s&uuid=%s",
		php_url,
		ROOT_PKG_REQ_AGENT_ID,
		version,
		pkg_config.assets_version,
		pkg_config.device_id,
		check_version and 1 or 0,
		pkg_config.pkg,
		"",
		WebRequester:UrlEncode(GameUtil.EncodeB64(game_name)),		--游戏名称
		WebRequester:UrlEncode(GameUtil.EncodeB64(UnityApp.identifier)),		--游戏包名
		uuid
	)
	return url
end

--------------------------------------------
GameAgentQuit = GameAgentQuit or DeclareView("GameAgentQuit", "gameagentquit/GameAgentQuit", 0, ViewLayer.Top)
function GameAgentQuit:GameAgentQuit()
end

function GameAgentQuit:OnClickClose()
	ViewMgr:CloseView(GameAgentQuit)
	if VersionTag.HasTag(VersionTag.GameQuitDelg) then
		if ChannelAgent.GameQuitCall then
			ChannelAgent.GameQuitCall(false)
		end
	end
end

function GameAgentQuit:OnClickConfirm()
	ViewMgr:CloseView(GameAgentQuit)
	if VersionTag.HasTag(VersionTag.GameQuitDelg) then
		if ChannelAgent.GameQuitCall then
			ChannelAgent.GameQuitCall(true)
		end
	end
end