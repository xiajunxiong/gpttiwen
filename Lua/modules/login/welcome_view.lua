FILE_REQ("modules/login/login_server_list_item")

WelcomeView = WelcomeView or DeclareView("WelcomeView","welcome",0,ViewLayer.Top - 2)
VIEW_DECLARE_MASK(WelcomeView,ViewMask.None)

function WelcomeView:WelcomeView()
	self.data = LoginCtrl.Instance:Data()			--data === LoginData
	self.lang_tab = Language.Login
	self.data.view_sel_server_id.server_id = -1
end

function WelcomeView:LoadCallback()
	-- self:SetLogo()
	self:SetLoginBg()
	-- LoginCtrl.Instance:PlatformAccountLogin()
end

function WelcomeView:SetLoginBg()
	local login_bg_path, is_stream_asset = ChannelAgentConsts.LoginBgPath()
	if not is_stream_asset then
		ResMgr:LoadObject(login_bg_path,function(tex)
				if self.LoginBgRawImg then
					self.LoginBgRawImg.texture = tex
				end
			end,false)
		if is_stream_asset == false then
			self.effObj:SetActive(false)
		end
	else
		local tex = ResourceManager.LoadStreamingTexture(login_bg_path)
		self.LoginBgRawImg.texture = tex
		self.effObj:SetActive(false)
	end
end

function WelcomeView:OpenCallback()
	PreloadViewStateMachine.Instance:StopAll() --进入登录界面后删掉前面那个界面
	TimeHelper:AddDelayFrameTimer(function ()
		LoginData.Instance.can_call_login = true
	end, 5)
	--  GameStateLuaRequire.CloseView()
	if not IS_AUDIT_VERSION then
		LoginCtrl.Instance:RequestUserProtocol()
	end
	UH.SetText(self.PublishInfo,LoginData.Instance:GetPublishInfo())
end

-- 判断是否需要勾选PK
function WelcomeView:JudgeNeedPK()
	if LoginData.Instance:IsHasPKLogin() then
		if LoginData.Instance:PKToggle() == 0 then
			PublicPopupCtrl.Instance:AlertTip(Language.Login.PKTip, function()
				LoginData.Instance:PKToggle(1)
				self.PKToggle.isOn = true
			end)
			return true
		end
	end
	return false
end

function WelcomeView:OnStartBtnClick()
	if GLOBAL_CONFIG:Flags().abandoned_device then
		ViewMgr:OpenView(AbandonDeviceTip)
		return
	end
	if self:JudgeNeedPK() then
		return
	end
    if self.UserTick.activeSelf and not self.UserTickGou.activeSelf then
		PublicPopupCtrl.Instance:Center(Language.Login.LoginUserProtocolUrlTips)
		return
	end

	if self.data.view_sel_server_id.server_id > 0 then
		self.data:SaveLastLoginServer(self.data:GetAccountKey(),self.data.view_sel_server_id.server_id)
	end
	
	local servers_info = GLOBAL_CONFIG:ServerInfoOther(self.data.view_sel_server_id.server_list_index)
	local sel_server_info = self.data:GetServerInfoById(self.data:GetLastLoginServer(), self.data:GetLastLoginAccountSpid())
	local now_server_time = servers_info.server_time + (Time.realtimeSinceStartup - GLOBAL_CONFIG.client_time)
	UnityPlayerPrefs.SetInt(PrefKeys.LoginUserProtocol(), 1)
	--非超级用户才判断开服时间
	if self.data.account.account_type ~= PLAT_ACCOUNT_TYPE_TEST then
		if sel_server_info.flag == 0 then
			local dialog_info = {
				content = Language.Login.ServerOpenTips4,
				confirm = {
					func = function()
						ViewMgr:CloseView(DialogTipsView)
					end
				},
				no_cancel = true,
			}
			PublicPopupCtrl.Instance:DialogTips(dialog_info)
			return	
		end
		--判断服务器状态
		if not StringIsEmpty(sel_server_info.open_time) and not StringIsEmpty(sel_server_info.ahead_time) and now_server_time < sel_server_info.open_time - sel_server_info.ahead_time then
		-- if true then
			local t_time = os.date("*t", sel_server_info.open_time)
			-- 提示不要了，改成弹窗
			-- local tip_str =string.format(Language.Login.RushDungeonips, t_time.month, t_time.day, t_time.hour, t_time.min)
			-- tip_str = string.format(Language.Login.ServerOpenTips2, tip_str)
			-- PublicPopupCtrl.Instance:Center(tip_str)
			local dialog_info = {
				content = Format(Language.Login.ServerOpenTips3, t_time.year, t_time.month, t_time.day, t_time.hour, t_time.min),
				confirm = {
					func = function()
						ViewMgr:CloseView(DialogTipsView)
					end
				},
				no_cancel = true,
			}
			PublicPopupCtrl.Instance:DialogTips(dialog_info)
			return
		end
		-- 删档计费弹窗
		local account_data = LoginData.Instance:AccountData()
		if account_data.fc_recharge > 0 then
			local dialog_info = {
				content = Format(Language.Login.ReturnRechargeTip, account_data.fc_recharge, account_data.login_day, account_data.fill_gold),
				confirm = {
					func = function()
						self:RequestServerAccountLogin()
						ViewMgr:CloseView(DialogTipsView)
					end
				},
			}
			PublicPopupCtrl.Instance:DialogTips(dialog_info)
		else
			self:RequestServerAccountLogin()
		end
	-- end
	-- LogError("need_reverify",self.data.need_reverify)
	-- if self.data.need_reverify then
	-- 	LoginCtrl.Instance:SendReverify(self.data:GetPlatformName(), function ()
	-- 		self:RequestServerAccountLogin()
	-- 	end)
	else
		-- 删档计费弹窗
		local account_data = LoginData.Instance:AccountData()
		if account_data.fc_recharge > 0 then
			local dialog_info = {
				content = Format(Language.Login.ReturnRechargeTip, account_data.fc_recharge, account_data.login_day, account_data.fill_gold),
				confirm = {
					func = function()
						self:RequestServerAccountLogin()
						ViewMgr:CloseView(DialogTipsView)
					end
				},
			}
			PublicPopupCtrl.Instance:DialogTips(dialog_info)
		else
			self:RequestServerAccountLogin()
		end
	end
end


function WelcomeView:RequestServerAccountLogin()
	self.data:SetWaitStr(Language.Login.Logining)
	if self.show_wait_delay ~= nil then
		TimeHelper:CancelTimer(self.show_wait_delay)
		self.show_wait_delay = nil
	end
	self.show_wait_delay = TimeHelper:AddDelayTimer(function ()
		PublicPopupCtrl.Instance:ShowWait(self.data:WaitStr())
	end,2)
	-- LogError("登录状态：",self.data:LoginState())
	LoginCtrl.Instance:RequestServerAccountLogin(function(error_code)
			if self.show_wait_delay ~= nil then
				TimeHelper:CancelTimer(self.show_wait_delay)
				self.show_wait_delay = nil
			end
			PublicPopupCtrl.Instance:HideWait()
			if error_code >= AccountLoginResult.Success then
				if IS_AUDIT_VERSION and GLOBAL_CONFIG:Flags().use_auto_create then--not GLOBAL_CONFIG:PkgInfo().is_youyan then
					local role_data = LoginData.Instance:GetLoginRoles()[1]
					RoleSelectData.Instance:SetSelectRoleData(role_data)
					LoginData.Instance:SetLastLoginIndex(role_data.index - 1)
					LoginCtrl.Instance:RequestRoleLogin(role_data.id)
					LoginData.Instance:SetLastRoleData()
				else
					ViewMgr:OpenView(RoleSelectView)
				end
			elseif error_code == AccountLoginResult.NoRole then
				if not IS_AUDIT_VERSION then
					local flag = LoginCtrl.Instance:RoleCreateWithLimited()
					if not flag then
						-- ViewMgr:OpenView(StoryOptionsView)
						-- ViewMgr:OpenView(RoleCreateVideoView)
						TimeHelper:AddDelayTimer(function()
							-- 这里需要判断是否进入小游戏
							if not LoginData.Instance:IsPlayMiniGame() then
								if IS_STANDALONE() then
									LoginCtrl.Instance:SpecialRequestAutoCreateRole(function ()
										LoginCtrl.Instance:AutoCreateCallback()
									end)
								else
									SceneCtrl.Instance:GotoScene(RoleCreateScene)
								end
							else
								MiniGameCtrl.Instance:GoToMiniGame()
							end
						end,0.2)
					else
						return
					end
				else
					if LoginData.Instance:IsPlayMiniGame() then
						TimeHelper:AddDelayTimer(function()	MiniGameCtrl.Instance:GoToMiniGame() end,0.2)
					elseif GLOBAL_CONFIG:PkgInfo().is_youyan and not GLOBAL_CONFIG:Flags().use_auto_create then
						SceneCtrl.Instance:GotoScene(RoleCreateScene)
					end
				end
			elseif error_code ~= AccountLoginResult.LOGIN_LIMIT_CREATE_NEW_ROLE and error_code ~= AccountLoginResult.LOGIN_LIMIT_LOGIN then
				PublicPopupCtrl.Instance:Center(string.format(Language.Login.FailedHint,tostring(error_code)))
			end
			--连接登录服务器结束埋点
			MainProber:Step(MainProber.STEP_LOAD_SERVER_END, MainProber.user_id, self.data:GetLastLoginServer(), error_code)
			if ChannelAgent.BehaviourTrigger then
				ChannelAgent.BehaviourTrigger(BehaviourType.SelectServer)
			end
		end)


	if self.try_timer == nil then 
		TimeHelper:CancelTimer(self.try_timer)
		self.try_timer = nil
	end 

	self.try_timer = TimeHelper:AddDelayTimer(function()
		-- 5秒钟以后还在试图登录登录服务器
		self.try_timer = nil
		if LoginData.Instance:LoginState() == LoginStateType.LoginServerConnecting then 
			LoginCtrl.GMDisconnect()
		end 
	end, 5)
	
	if ViewMgr:IsOpen(LoginNoticeView) then 
		ViewMgr:CloseView(LoginNoticeView)
	end 
end

function WelcomeView:OnClickBackToAccount()
	if LoginCtrl.Instance.login_verify_state == LoginVerifyState.LOGINCOMPILETE then
		ChannelAgent:Logout()
	end
end

function WelcomeView:OnClickBtnToLogin()
	if not StringIsEmpty(GLOBAL_CONFIG:Urls().login_kefu_tips) then
		PublicPopupCtrl.Instance:AlertTip(GLOBAL_CONFIG:Urls().login_kefu_tips)
		return
	end
	LoginCtrl.Instance:PlatformAccountLogin()
end

function WelcomeView:OnClickLoginNotice()
	if LoginData.Instance:GetNoticeData().is_null or LoginData.Instance:GetNoticeData() == nil
	    or LoginData.Instance:GetNoticeData().content == nil 
	    or #LoginData.Instance:GetNoticeData().content == 0 then return end 
	ViewMgr:OpenView(LoginNoticeView)
end 

function WelcomeView:OnClickDeleteAccountNotice()
	if LoginCtrl.Instance:RequestDeleteAccountNotice() then
		ViewMgr:OpenView(LoginNoticeView, {delete_account = true})
	end
end

function WelcomeView:OnClickService()
	ViewMgr:OpenView(ServiceView)
end

function WelcomeView:OnClickUserProtocol()
	if LoginData.Instance:GetUserProtocol().is_null 
	  then return end 
	ViewMgr:OpenView(UserProtocolView)
end

function WelcomeView:OnClickAgeNotice()
	ViewMgr:OpenView(AgeNoticeView)
end

function WelcomeView:OnClickSkip()
	self.Gou:SetActive(not self.Gou.activeSelf)
end

function WelcomeView:OnClickSelServer()
	ViewMgr:OpenView(ServerView)
end

function WelcomeView:CloseCallback()
    if self.try_timer ~= nil then
		TimeHelper:CancelTimer(self.try_timer)
		self.try_timer = nil
	end
	if self.show_wait_delay ~= nil then
		TimeHelper:CancelTimer(self.show_wait_delay)
		self.show_wait_delay = nil
	end
end
------------------------------------------------------
WelcomeLogo = WelcomeLogo or DeclareMono("WelcomeLogo", UIWFlushPanel)
function WelcomeLogo:WelcomeLogo()
	self.is_loaded = false
end

function WelcomeLogo:Awake()
	local logo_path, is_stream_asset = ChannelAgentConsts.LogoPath()
	if GLOBAL_CONFIG:PkgInfo().agent_id == "ml6" then
		self.LogoBgObj:SetActive(false)
		self.LoginLogoRawImg.enabled = false
		self.is_loaded = true
	elseif not is_stream_asset then
		ResMgr:LoadObject(logo_path,function(tex)
			self.is_loaded = true
			self.LoginLogoRawImg.texture = tex
			self.LoginLogoRawImg:SetNativeSize()
		end,false)
		if is_stream_asset == false then
			self.LogoBgObj:SetActive(false)
		end
	else
		local tex = ResourceManager.LoadStreamingTexture(logo_path)
		self.LoginLogoRawImg.texture = tex
		self.LoginLogoRawImg:SetNativeSize()
		self.LogoBgObj:SetActive(false)
		self.is_loaded = true
	end
	UIWFlushPanel.Awake(self)
end

function WelcomeLogo:IsLoaded()
	return self.is_loaded
end

----------------------功能按钮显示和隐藏--------------
---BtnEnter====GameObject
---BtnServer====GameObject
WelcomeViewBtnsShow = WelcomeViewBtnsShow or  DeclareMono("WelcomeViewBtnsShow",UIWFlushPanel)

function WelcomeViewBtnsShow:WelcomeViewBtnsShow()
    self.data_cares = {
        {data = LoginData.Instance:AccountData(), func = self.OnAccountKeyChange, keys = {"key"}},
		{data = LoginData.Instance:GetUserProtocol(), func = self.OnUserProtocolChange, keys = {"is_null"}},
    }
	self.link_mark = {}
	-- 添加审核版本控制显示公告
	if IS_AUDIT_VERSION then
		self.BtnLoginNotice:SetActive(false)
	end
end

function WelcomeViewBtnsShow:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	if self.pc_auto_enter_delay_handle ~= nil then
		TimeHelper:CancelTimer(self.pc_auto_enter_delay_handle)
		self.pc_auto_enter_delay_handle = nil
	end
end

function WelcomeViewBtnsShow:OnAccountKeyChange()
	local acc_key = LoginCtrl.Instance:Data():GetAccountKey()
	self.BtnDelAccount:SetActive(not StringIsEmpty(GLOBAL_CONFIG:Urls().del_account_url))
	if StringIsEmpty(acc_key) == true then
		self.BtnEnter:SetActive(false)
		self.BtnServer:SetActive(false)
		self.BtnBackToAccount:SetActive(false)
		self.BtnLoginNotice:SetActive(false)
		self.BtnService:SetActive(false)
		self.BtnUserProtocol:SetActive(false)
		self.BtnToLogin:SetActive(true)
		Runner.Instance:RunUntilTrue(function ()
			if LoginData.Instance.can_call_login then
				LoginCtrl.Instance:PlatformAccountLogin()
				return true
			else
				return false
			end
		end)
		-- LoginCtrl.Instance:PlatformAccountLogin()

		self.UserTick:SetActive(false)
		self.ObjPKToggle:SetActive(false)
	else
		self.BtnToLogin:SetActive(false)
		-- 如果是刚开始就要玩小游戏，直接帮忙进最新服，不显示开始游戏和选服
		if LoginData.Instance:IsPlayMiniGame() then
			self.BtnEnter:SetActive(false)
			self.BtnServer:SetActive(false)
			local last_server, _ = LoginData.Instance:GetLastLoginServer()
			-- 这里自动选最新的服务器，就是推荐服
			LoginData.Instance.view_sel_server_id.server_id = last_server
			self.WelComeMono:OnStartBtnClick()
		else
			self.BtnEnter:SetActive(true)
			self.BtnServer:SetActive(true)
			if not self.is_show_notice and not IS_AUDIT_VERSION then 
				LoginCtrl.Instance:RequestLoginNotice()
				-- LoginCtrl.Instance:RequestUserProtocol()
			self.is_show_notice = true
			else 
				LoginData.Instance:CheckShowLoginNotice()
			end
			if GLOBAL_CONFIG:Flags().abandoned_device then
				ViewMgr:OpenView(AbandonDeviceTip)
			end
			if IS_STANDALONE() then
				if self.pc_auto_enter_delay_handle ~= nil then
					TimeHelper:CancelTimer(self.pc_auto_enter_delay_handle)
				end
				self.pc_auto_enter_delay_handle = TimeHelper:AddDelayTimer(function ()
					self.WelComeMono:OnStartBtnClick()
				end,20)
			end
		end
		self.BtnBackToAccount:SetActive(IS_EDITOR or not IS_STANDALONE())
		-- 添加审核版本控制显示公告
		if not IS_AUDIT_VERSION then
			self.BtnLoginNotice:SetActive(true)
		end
		--self.BtnService:SetActive(ServiceData.Instance:IsOpenService() and not IS_AUDIT_VERSION)

		local data = LoginData.Instance:GetUserProtocol()

		self.BtnUserProtocol:SetActive(not data.is_null and not StringIsEmpty(data.content) and not IS_AUDIT_VERSION)

		local user_flag = false--not data.is_null and (not StringIsEmpty(data.user_agree_url) or not StringIsEmpty(data.privacy_url))
		if QQBigPlayerCtrl.IsQQPC() then
			user_flag = not data.is_null and (not StringIsEmpty(data.user_agree_url) or not StringIsEmpty(data.privacy_url))
		end
		self.UserTick:SetActive(user_flag)
		local flag = UnityPlayerPrefs.HasKey(PrefKeys.LoginUserProtocol())
		self.UserTickGou:SetActive(flag)
		self:FlsuhPKToggle()
	end
end


function WelcomeViewBtnsShow:FlsuhPKToggle()
	local is_has_pk = LoginData.Instance:IsHasPKLogin()
	self.ObjPKToggle:SetActive(is_has_pk)
	self.BtnEnterPos:SetLocalPosition(is_has_pk and Vector3.New(0,-260,0) or Vector3.New(0,-280,0))
	if is_has_pk then
		self.PKToggle.isOn = LoginData.Instance:PKToggle() == 1
	end
end

function WelcomeViewBtnsShow:OnPKClick(value)
	LoginData.Instance:PKToggle(value and 1 or 0)
end

function WelcomeViewBtnsShow:OnUserProtocolChange()
	local data = LoginData.Instance:GetUserProtocol()
	-- self.UserTick:SetActive(not data.is_null)
	if data.is_null then return end 

	local user_text = RichTextHelper.ParseAllFunc(data.user_agree_url, true ,function(paramt) self.link_mark[paramt.mark] = paramt.address end)
	local privacy_text = RichTextHelper.ParseAllFunc(data.privacy_url, true ,function(paramt) self.link_mark[paramt.mark] = paramt.address end)
	UH.SetText(self.UserTickText,Format(Language.Login.LoginUserProtocolUrl,user_text,privacy_text))

	RichTextHelper.Bind(self.UserTickText, BindTool.Bind(self.OnClickLink, self))
end

function WelcomeViewBtnsShow:OnClickLink(dict)
	if nil == dict then return end
    local t = RichTextHelper.DictionarySSToTable(dict, true)
    local key = t[1]
    if "weblink" == key then
		if self.link_mark[t[2]] ~= nil then 
            UnityApp.OpenURL(self.link_mark[t[2]])
        end 
    end 
end

function WelcomeViewBtnsShow:OnClickUserGou()
	self.UserTickGou:SetActive(not self.UserTickGou.activeSelf)
end

----------------------当前选择的服务器显示--------------
-----ServerName====UIText
-----ServerState===Sprite
WelcomeViewServer = WelcomeViewServer or  DeclareMono("WelcomeViewServer",UIWFlushPanel)

function WelcomeViewServer:WelcomeViewServer()
    self.data_cares = {
        {data = LoginData.Instance:AccountData(), func = self.onFlush, keys = {"key"}},
		{data = LoginData.Instance:GetViewSelServer(), func = self.onFlush, keys = {"server_id"}},
    }
end

function WelcomeViewServer:onFlush()
	local data = LoginCtrl.Instance:Data()
	local last_server_id = data.view_sel_server_id.server_id > 0 and data.view_sel_server_id.server_id or data:GetLastLoginServer()
	local last_account_spid = StringIsEmpty(data.view_sel_server_id.account_spid) and data:GetLastLoginAccountSpid() or data.view_sel_server_id.account_spid
	-- LogError("WelcomeView last_account_spid",last_account_spid, StringIsEmpty(data.view_sel_server_id.account_spid))
	if StringIsEmpty(last_account_spid) or GLOBAL_CONFIG:ServerInfoOther()[1] == nil then
		last_account_spid = GLOBAL_CONFIG:ServerInfo().account_spid
	end
	data.view_sel_server_id.account_spid = last_account_spid
	for k,v in pairs(GLOBAL_CONFIG:ServerInfoOther()) do
		if not StringIsEmpty(v.account_spid) and v.account_spid == last_account_spid then
			data.view_sel_server_id.server_list_index = v.index
			break
		end
	end

	local server_info = data:GetServerInfoById(last_server_id, last_account_spid) or {}
	if server_info == nil or (server_info.flag == 4 and data.account.account_type == 0) then
		server_info = data:GetServerInfoById(GLOBAL_CONFIG:ServerInfo().last_server, GLOBAL_CONFIG:ServerInfo().account_spid)
		data.view_sel_server_id.server_id = server_info.id
		data.view_sel_server_id.account_spid = GLOBAL_CONFIG:ServerInfo().account_spid
		data.view_sel_server_id.server_list_index = 0
	end

	self.NewTag:SetActive(server_info.flag == 1)
	UH.SpriteName(self.ServerState, SERVER_STATE[server_info.flag])
	if server_info ~= nil then
		if IS_EDITOR then
			self.ServerName.text = string.format(Language.Login.ServerNameEditor, server_info.id or 1, server_info.name or "1服")
		else
			self.ServerName.text = string.format(Language.Login.ServerName,server_info.name)--,server_info.id,server_info.name)
		end	
	end
end


WelcomeView.FK = {
					-- Buttons = "onFlushButtons",
					Server = WelcomeViewServer:GetTypeName(),
					-- ServerList = WelcomeViewServerList:GetTypeName(),
				}
-----------------------------
AbandonDeviceTip = AbandonDeviceTip or DeclareView("AbandonDeviceTip","login/dialog_abandon_tips",0,ViewLayer.Top - 1)
VIEW_DECLARE_MASK(WelcomeView,ViewMask.BgBlock)
function AbandonDeviceTip:AbandonDeviceTip()
end

function AbandonDeviceTip:LoadCallback()
	UH.SetText(self.AbandonText, string.format(Language.Login.AbandonedDevice, GLOBAL_CONFIG:Urls().abandoned_device_id))
end

function AbandonDeviceTip:OnClickConfirm()
	ViewMgr:CloseView(AbandonDeviceTip)
end