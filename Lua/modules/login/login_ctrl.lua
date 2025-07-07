----Account login + Role login
FILE_REQ("modules/login/login_data")
FILE_REQ("modules/login/role_create_data")
FILE_REQ("modules/login/role_select_data")

VIEW_REQ("modules/login/welcome_view")
VIEW_REQ("modules/login/server_view")
VIEW_REQ("modules/login/login_view")
VIEW_REQ("modules/login/role_create_view")
VIEW_REQ("modules/login/role_select_view")
VIEW_REQ("modules/login/login_notice_view")
VIEW_REQ("modules/login/login_ban_dialog")
VIEW_REQ("modules/login/user_protocol_view")
VIEW_REQ("modules/login/role_create_video_view")

VIEW_REQ("modules/login/service_test")
FILE_REQ("modules/login/service_share_game")

--PC SDK 实名验证窗口：
VIEW_REQ("modules/fcm_view/sanliuling_fcm")

LoginCtrl = LoginCtrl or BaseClass(BaseCtrl)


AccountLoginResult = {
	Success 		= 0,		--登录成功
	NoThread 		= -1,
	ServerError 	= -2,		--服务器错误
	ResultExist 	= -3,
	SceneNotExist 	= -4,		--场景不存在
	NoGateway 		= -5,		--网关不存在
	NoRole			= -6,
	ThreadBusy		= -7,
	Forbid			= -8,		--账号已被封禁
	AntiWallow		= -9,
	ForbidNewRole	= -10,		--服务器人数已满
	LOGIN_LIMIT_CREATE_NEW_ROLE = -11, 	--
	LOGIN_LIMIT_LOGIN = -12,		--
	ServerConnectError = -101,	--登录服务器连接失败
}

RoleCreateResult = {
	Success 		= 0,
	NoSpace			= -1,	--角色已满，无法再次创建
	ExistName		= -2,	--名字已存在
	InvalidName		= -3,	--名字不合法
	OldServerNewUser= -4,	--服务器已达上限
	ServerError 	= -5.	--服务器发生错误
}

LoginSceneResult = {
	Success = 0,
	UserExist = -1,
	SceneNoExist = -2,
}

function LoginCtrl:__init()
	if LoginCtrl.Instance ~= nil then
		Debuger.LogError("[LoginCtrl] attempt to create singleton twice!")
		return
	end
	LoginCtrl.Instance = self
	self.data = LoginData.New()
	self.role_create_data = RoleCreateData.New()
	self.role_select_data = RoleSelectData.New()
	self.qq_big_player_is_open = false
	self.login_account_callback = nil
	self.role_create_callback = nil

	self.story_round = -1

--------Login server message----------------
	self:RegisterProtocol(CSLoginToAccount)	--登录账号
	self:RegisterProtocol(SCLoginToAccount,"RecvLoginToAccount")

	self:RegisterProtocol(CSCreateRole)		--创建角色
	self:RegisterProtocol(SCCreateRoleAck,"RecvCreateRole")

	self:RegisterProtocol(CSLoginToRole)	--登录角色
	self:RegisterProtocol(SCLoginToRole,"RecvLoginToRole")

---------Logic server message--------------
	self:RegisterProtocol(CSUserEnterGSReq)	--登录场景
	self:RegisterProtocol(SCUserEnterGSAck,"RecvLoginScene")

	-- self:RegisterProtocol(CSAllInfoReq)		--请求所有模块数据

	self:RegisterProtocol(CSRandomNameReq)
	self:RegisterProtocol(SCRandomName, "OnRandomName")

	self:RegisterProtocol(SCDisconnectNotice,"OnDisconnectNotice")
---------Cross server message--------------
	self:RegisterProtocol(SCCrossStartAck, "RecvCrossStartAck")	--收到跨服请求
	self:RegisterProtocol(SCReturnOriginalSever, "RecvReturnOriginalSever")--被通知返回原服
	self:RegisterProtocol(SCBackToOriginServer, "RecvBackToOriginServer")--（主动）被通知返回原服
	self:RegisterProtocol(CSBackToOriginServer)--返回原服
	self:RegisterProtocol(CSCrossStartReq)--进入跨服

	self:RegisterProtocol(CSRoleSubscribe)

-------Game Story----------------------------
	self:RegisterProtocol(CSStoryBattleInfo)
	self:RegisterProtocol(CSStoryRoundResult)
	self:RegisterProtocol(SCStoryNotAvaliable, "RecvStoryNotAvaliable")

	self:RegisterProtocol(SCCrossConnectInfo, "OnCrossConnectInfo")

	self:RegisterProtocol(CSRealNameRewardReq)
	self:RegisterProtocol(SCRealNameRewardInfo, "OnSCRealNameRewardInfo")

	self.login_state_care = self.data:BaseData():Care(BindTool.Bind(self.OnLoginStateChange,self),"login_state")

end

function LoginCtrl:__delete()
	self.data:BaseData():Uncare(self.login_state_care)

	self:UnRegisterProtocol(SCLoginToAccount)
	self:UnRegisterProtocol(SCCreateRoleAck)
	self:UnRegisterProtocol(SCLoginToRole)
	self:UnRegisterProtocol(SCUserEnterGSAck)
	self:UnRegisterProtocol(SCRandomName)
	self:UnRegisterProtocol(SCDisconnectNotice)
	self:UnRegisterProtocol(SCCrossStartAck)
	self:UnRegisterProtocol(SCReturnOriginalSever)
	self:UnRegisterProtocol(SCCrossConnectInfo)

	if LoginCtrl.Instance == self then
		LoginCtrl.Instance = nil
	end
end

function LoginCtrl:OnInit()
	-- 跨服使者
	BehaviorData.Instance:CustomNpcTalkBtnsSeq(574, BindTool.Bind(self.SetupBehaviourBtn, self))
	FunOpen.Instance:SelfCondition(Mod.QQBigPlayer.Main, function ()
		return self.qq_big_player_is_open
	end)
end

function LoginCtrl:PlatformAccountLogin()
	ChannelAgent:Login(function (data)
		self:SendLoginVerify(data)
		-- self.data:SetAccountKey(data.AccountId)
		-- LogError("PlatformAccountLogin callback")
		-- ViewMgr:FlushView(WelcomeView)
	end)
end


function LoginCtrl:RequestServerAccountLogin(on_login, is_cross)
	if self.data:LoginState() > 0 then		--连接没有错误正在进行时，请求连接无效
		return
	end
	self.data:SetLoginState(LoginStateType.LoginServerConnecting)
	local user_id = string.format("%s_%s",tostring(self.data:GetPlatformName()),tostring(self.data:GetLastLoginServer()))
	if BuglyAgent ~= nil and not IS_AUDIT_VERSION then
		BuglyAgent.SetUserId(user_id)
	end
	VoiceRecorder.SetUserId(user_id)
	MainProber:SetUserId(self.data:GetPlatformName(), self.data:GetLastLoginServer(),self.data:GetPhpLoginTime())
	MainProber:Step(MainProber.STEP_LOAD_SERVER_BEG, tostring(self.data:GetPlatformName()), self.data:GetLastLoginServer()) 	--连接登录服开始埋点上报
	-- LogError("RequestServerAccountLogin ",self.data:GetPlatformName())
	NetworkCtrl.Instance:ConnectLoginServer(function(error)
			if StringIsEmpty(error) then
				self.data:SetLoginState(LoginStateType.AccountLogining)
				self.login_account_callback = on_login
				local protc = GetProtocol(CSLoginToAccount)
				--php platform server data
				protc.login_time = self.data:GetPhpLoginTime()
				protc.login_str = self.data:GetPhpLoginStr()

				--user data
				protc.pname = self.data:GetPlatformName()
				protc.server = self.data:GetLastLoginServer()
				--LogError("login platform",protc.pname,"login_str",protc.login_str,"login_time",protc.login_time)
				SendProtocol(protc)
			else
				self.data:MarkLoginStateError()
				Debuger.LogError("RequestServerAccountLogin--ConnectLoginServer Error [%s]",tostring(error))
				on_login(AccountLoginResult.ServerConnectError, is_cross)
			end
		end)
end

function LoginCtrl:RoleCreateWithLimited()
	local limited_info = GLOBAL_CONFIG:GetCreateLimitedInfo()
	local server_info = GLOBAL_CONFIG:ServerInfo()
	local cur_select_server_id = self.data.view_sel_server_id.server_id > 0 and self.data.view_sel_server_id.server_id or self.data:GetLastLoginServer()

	if limited_info.regist_limit == nil and limited_info.regist_limit == 0 then 	--客户端屏蔽创建角色的逻辑
		return false
	end 

	local cur_server_info = nil 
	for k,v in pairs(server_info.server_list) do 
		if v.id == cur_select_server_id then 
			cur_server_info = v
		end 
	end 

	if cur_server_info == nil then cur_server_info = server_info.server_list[1] end 

	local is_allowed = false 
	local server_time = server_info.server_time
	if limited_info.regist_allow_server ~= nil then 
		for k,v in pairs(limited_info.regist_allow_server) do
			if tonumber(v) == cur_server_info.id then
				is_allowed = true
				break
			end
		end
	end 

	if not is_allowed and limited_info.regist_limit ~= nil and server_time - cur_server_info.open_time >= limited_info.regist_limit 
	    and limited_info.regist_serverid ~= nil and cur_server_info.id ~= nil and cur_server_info.id <= limited_info.regist_serverid then
			
		-- SysMsgCtrl.Instance:ErrorRemind(Language.Login.ClientCreateRoleForbid, true)
		-- GameNet.Instance:DisconnectLoginServer()
		-- self:RemoveConnectingEffect()
		return true 
	end
	return false
end

function LoginCtrl:RecvLoginToAccount(protocol)
	-- self.data:SetAccountRoles(protocol.role_item)
	--LogError("RecvLoginToAccount",protocol)
	self.data:SetLoginRoles(protocol.role_item)
	self.data:SetLastLoginIndex(protocol.index)
	if self.login_account_callback ~= nil then
		if protocol.result >= AccountLoginResult.Success then
			LoginData.Instance:SetIsPlayMiniGame(false)
		elseif protocol.result == AccountLoginResult.NoRole then
			-- LogError("IS_AUDIT_VERSION and GLOBAL_CONFIG:Flags().use_auto_create",tostring(IS_AUDIT_VERSION and GLOBAL_CONFIG:Flags().use_auto_create))
			if IS_AUDIT_VERSION and GLOBAL_CONFIG:Flags().use_auto_create then-- not GLOBAL_CONFIG:PkgInfo().is_youyan then
				self.data:SetLoginState(LoginStateType.RoleCreating)
				self:RequestAutoCreateRole(function ()
					self:AutoCreateCallback()
				end)
			else
				local flag = self:RoleCreateWithLimited()
				if not flag then 
					self.data:SetLoginState(LoginStateType.RoleCreating)
				else 
					PublicPopupCtrl.Instance:Center(Language.Login.ServerLimited)
					PublicPopupCtrl.Instance:HideWait()
					self:unloadGameLogicBeforeRelogin()
					LoginData.Instance:Clear()
	
					self.data:SetAccountKey(LoginData.Instance:GetLastLoginAccount())
					-- return
				end
			end
		-- elseif protocol.result == AccountLoginResult.Forbid then
		-- 	self.data:SetForbidTime(protocol.forbid_time)
		-- 	ViewMgr:OpenView(LoginBanDialog)
		else
			if protocol.result == AccountLoginResult.LOGIN_LIMIT_CREATE_NEW_ROLE or protocol.result == AccountLoginResult.LOGIN_LIMIT_LOGIN then
				PublicPopupCtrl.Instance:Center(Language.Login.ServerBusyCannotCreate)
			end
			self.data:MarkLoginStateError(0)
		end
		if self.login_account_callback == nil then
			LogError("没有回调登录结果：",protocol.result)
		end
		local ck_bac = self.login_account_callback
		self.login_account_callback = nil
		ck_bac(protocol.result)
	end
	-- if protocol.result == AccountLoginResult.Success then
	-- 	EventSys:Fire(GlobalEventKeys.LoginSuccess)
	-- end
end

--正式服的自动创角（角色形象和颜色纯随机，职业根据随机出来的形象中可选的职业再纯随机）
function LoginCtrl:SpecialRequestAutoCreateRole(on_create)
	if self.data:LoginState() ~= LoginStateType.RoleCreating then
		return
	end
	self.special_auto_create = true
	self.data:SetLoginState(LoginStateType.RoleCreateRequesting)
	self.role_create_callback = on_create
	local user_id = self.data:GetPlatformName()
	local role_name = GameUtil.EncodeB64(self.role_create_data.role_name.name)
	MainProber:Step(MainProber.STEP_CREATE_ROLE_BEG, user_id, self.data:GetLastLoginServer(), role_name) --角色创建申请埋点
	local protc = GetProtocol(CSCreateRole)
	protc.pname = self.data:GetPlatformName()
	protc.role_name = "autots"
	protc.login_time = self.data:GetPhpLoginTime()
	protc.login_str = self.data:GetPhpLoginStr()
	protc.server = self.data:GetLastLoginServer()
	protc.avatar_type = math.random(1,6) --[1,6]
	protc.color = math.random(0,3) 		--[0,4)
	if IS_MICRO_PLAYER() then
		protc.color = 0
	end
	protc.name_str_id = 0
	protc.job_id = RoleCreateData.Instance:GetAvatarCanRandomChooseProf(protc.avatar_type)	--[1,9]
	protc.plat_spid_str = self.data:GetAccountSpid()
	protc.mini_game_id = self.data:GetMiniGame()
	protc.is_auto_create = 1
	protc.audit_seq = 0
	SendProtocol(protc)
end

function LoginCtrl:RequestAutoCreateRole(on_create)
	if self.data:LoginState() ~= LoginStateType.RoleCreating then
		return
	end
	self.special_auto_create = false
	self.data:SetLoginState(LoginStateType.RoleCreateRequesting)
	self.role_create_callback = on_create
	local user_id = self.data:GetPlatformName()
	local role_name = GameUtil.EncodeB64(self.role_create_data.role_name.name)
	MainProber:Step(MainProber.STEP_CREATE_ROLE_BEG, user_id, self.data:GetLastLoginServer(), role_name) --角色创建申请埋点
	local protc = GetProtocol(CSCreateRole)
	protc.pname = self.data:GetPlatformName()
	protc.role_name = "autots"
	protc.login_time = self.data:GetPhpLoginTime()
	protc.login_str = self.data:GetPhpLoginStr()
	protc.server = self.data:GetLastLoginServer()
	protc.avatar_type = StringIsEmpty(GLOBAL_CONFIG:Urls().audit_avatar_type) and math.random(1,6) or GLOBAL_CONFIG:Urls().audit_avatar_type		--[1,6]
	protc.color = StringIsEmpty(GLOBAL_CONFIG:Urls().audit_color_index) and math.random(0,3) or GLOBAL_CONFIG:Urls().audit_color_index		--[0,4)
	if IS_MICRO_PLAYER() then
		protc.color = 0
	end
	--protc.init_appearance = self.role_create_data.cur_sel_color.role_id * 10000 + self.role_create_data.cur_sel_color.color_id	--avatar_type * 10000 + [0,4)
	protc.name_str_id = 0
	protc.job_id = StringIsEmpty(GLOBAL_CONFIG:Urls().audit_prof_id) and RoleCreateData.Instance:GetAvatarCanRandomChooseProf(protc.avatar_type) or GLOBAL_CONFIG:Urls().audit_prof_id			--[1,9]
	protc.plat_spid_str = self.data:GetAccountSpid()
	protc.mini_game_id = self.data:GetMiniGame()
	protc.is_auto_create = 1
	protc.audit_seq = StringIsEmpty(GLOBAL_CONFIG:Urls().audit_seq) and 0 or GLOBAL_CONFIG:Urls().audit_seq
	SendProtocol(protc)
end


function LoginCtrl:RequestCreateRole(on_create)
	if self.data:LoginState() ~= LoginStateType.RoleCreating then
		return
	end
	self.special_auto_create = false
	self.data:SetLoginState(LoginStateType.RoleCreateRequesting)
	self.role_create_callback = on_create
	local user_id = self.data:GetPlatformName()
	local role_name = GameUtil.EncodeB64(self.role_create_data.role_name.name)
	MainProber:Step(MainProber.STEP_CREATE_ROLE_BEG, user_id, self.data:GetLastLoginServer(), role_name) --角色创建申请埋点
	local protc = GetProtocol(CSCreateRole)
	protc.pname = self.data:GetPlatformName()
	protc.role_name = self.role_create_data.role_name.name or 0
	protc.login_time = self.data:GetPhpLoginTime()
	protc.login_str = self.data:GetPhpLoginStr()
	protc.server = self.data:GetLastLoginServer()
	protc.avatar_type = self.role_create_data.create_view_data.appear_id		--[1,6]
	protc.color = self.role_create_data.create_view_data.color_index --[0,4)
	--protc.init_appearance = self.role_create_data.cur_sel_color.role_id * 10000 + self.role_create_data.cur_sel_color.color_id	--avatar_type * 10000 + [0,4)
	protc.name_str_id = self.role_create_data.name_str_id or 0
	protc.job_id = self.role_create_data.create_view_data.prof_id or 0
	protc.plat_spid_str = self.data:GetAccountSpid()
	protc.mini_game_id = self.data:GetMiniGame()
	protc.is_auto_create = 0
	SendProtocol(protc)
end

function LoginCtrl:RecvCreateRole(protocol)
	if protocol.result >= RoleCreateResult.Success then
		-- self.data:AddAccountRole(protocol)
		self.data:AddLoginRoles(protocol)
		MainProber:SetRoleInfo(protocol.role_id, protocol.role_name)
		ChannelAgent.role_id = protocol.role_id
		ChannelAgent.role_name = protocol.role_name
		local role_base_data = RoleData.Instance:GetBaseData()
		role_base_data.create_time = protocol.create_role_time
		role_base_data.role_id = protocol.role_id
		role_base_data.name = protocol.role_name
		role_base_data.level = 1
		ChannelAgent.BehaviourTrigger(BehaviourType.CreateRole)
	else
		-- self.data:MarkLoginStateError()
		self.data:SetLoginState(LoginStateType.RoleCreating)
	end
	if self.role_create_callback ~= nil then
		local ck_bac = self.role_create_callback
		self.role_create_callback = nil
		ck_bac(protocol.result)
	end	
end

function LoginCtrl:AutoCreateCallback()
	--LogError("AutoCreateCallback")
	RoleCreateData.Instance:ResetModelRotateInfo()
	local user_id = LoginData.Instance:GetPlatformName()
	local role_name = GameUtil.EncodeB64(LoginData.Instance:GetNewCreateRoles().name)
	--角色创建申请结果埋点
	MainProber:Step(MainProber.STEP_CREATE_ROLE_END, user_id, LoginData.Instance:GetLastLoginServer(), role_name, LoginData.Instance:GetNewCreateRoles().id) 
	-- PublicPopupCtrl.Instance:Center(string.format("RecvCreateRole SUC:%s",tostring(err_code)))
	LoginCtrl.Instance:RequestRoleLogin(LoginData.Instance:GetNewCreateRoles().id)
	-- 创角成功后清除封测充值金额
	LoginData.Instance:ResetFCRecharge()
	LoginData.Instance:NewCharaCreateComplete()
end


function LoginCtrl:RequestRoleLogin(role_id)
	if self.data:LoginState() ~= LoginStateType.RoleCreateRequesting and 
		self.data:LoginState() ~= LoginStateType.AccountLogining and 
		self.data:LoginState() ~= LoginStateType.Logined and 
		self.data:LoginState() ~= LoginStateType.RoleSelecting and
		self.data:LoginState() ~= LoginStateType.RoleCreating then
		return
	end
	self.data:SetLoginState(LoginStateType.RoleLogining)
	if role_id == nil then
		local defualt_role = self.data:GetLastLoginRole()
		role_id = defualt_role ~= nil and defualt_role.id or 0
	end
	if role_id <= 0 then
		Debuger.LogErrorSD("Role ID Below 0!!!role_id===%d",role_id)
		self.data:MarkLoginStateError()
		return
	end
	local user_id = string.format("%s_%s_%s",tostring(self.data:GetPlatformName()),tostring(self.data:GetLastLoginServer()),tostring(role_id))
	if BuglyAgent ~= nil and not IS_AUDIT_VERSION then
		BuglyAgent.SetUserId(user_id)
	end
	VoiceRecorder.SetUserId(user_id)
	MainProber:SetUserId(self.data:GetPlatformName(), self.data:GetLastLoginServer(), self.data:GetPhpLoginTime())
	local roledata = self.data:GetLoginRoleById(role_id)
	local role_name = ""
	if roledata then
		role_name = GameUtil.EncodeB64(roledata.name)
	end
	MainProber:Step(MainProber.STEP_REQUEST_LOGIN_ROLE, self.data:GetPlatformName(), self.data:GetLastLoginServer(), role_name, role_id) --登录角色申请埋点
	local protc = GetProtocol(CSLoginToRole)
	protc.login_time = self.data:GetPhpLoginTime()
	protc.login_str = self.data:GetPhpLoginStr()
	protc.pname = self.data:GetPlatformName()
	protc.server = self.data:GetLastLoginServer()
	protc.role_id = role_id
	SendProtocol(protc)
end

function LoginCtrl:RecvLoginToRole(protocol)
	-- TimeHelper:AddDelayTimer(function ()
	if protocol.result < 0 then
		self.data:MarkLoginStateError()
		MainProber:SetRoleInfo(0,"")
		Debuger.LogErrorSD("LoginCtrl:RecvLoginToRole ERR_CODE=%s",tostring(protocol.result))
	else
		local roledata = self.data:GetLoginRoleById(protocol.role_id)
		local role_name = GameUtil.EncodeB64(roledata.name)
		MainProber:Step(MainProber.STEP_LOAD_GAME_SERVER_BEG, MainProber.user_id, self.data:GetLastLoginServer(), role_name, protocol.role_id, protocol.result)
		
		self.data:SetLoginState(LoginStateType.LogicServerConnecting)
		local role_base_data = RoleData.Instance:GetBaseData()
		role_base_data.role_id = protocol.role_id
		role_base_data.scene_id = protocol.scene_id
		role_base_data.last_scene_id = protocol.last_scene_id
		role_base_data.session_key = protocol.key
		role_base_data.role_name = protocol.role_name
		role_base_data.name = protocol.role_name
		role_base_data.time = protocol.time
		role_base_data.create_time = protocol.create_time

		ChannelAgent.role_id = protocol.role_id
		ChannelAgent.role_name = roledata.name
		RoleCtrl.Instance.role_login_trigger = true
		RechargeCtrl.Instance.wait_info = true
		-- ChannelAgent.BehaviourTrigger(BehaviourType.RoleLogin)
		MainProber:SetRoleInfo(protocol.role_id, protocol.role_name)
		-- Debuger.LogError("host===%s,port=%s",tostring(protocol.gs_hostname),tostring(protocol.gs_port))
		NetworkCtrl.Instance:SetLogicServerHost(protocol.gs_hostname,protocol.gs_port)
		NetworkCtrl.Instance:ConnectLogicServer(function(err)
				if StringIsEmpty(err) == true then
					MainProber:Step(MainProber.STEP_LOAD_GAME_SERVER_END, MainProber.user_id, self.data:GetLastLoginServer(), role_name, protocol.role_id)
					NetworkCtrl.Instance:DisconnectLoginServer()
					self:RequestLoginScene()
					self.data:SaveLoginRoleData(RoleData.Instance:GetBaseData())
					self.data:BaseData().is_on_crosssever = false
				else
					if protocol.gs_hostname ~= protocol.backup_gs_hostname or
					protocol.gs_port ~= protocol.backup_gs_port then
						local backup = DataHelper.TableCopy(protocol)
						backup.gs_hostname = protocol.backup_gs_hostname
						backup.gs_port = protocol.backup_gs_port
						self:RecvLoginToRole(backup)
						return
					end
					self.data:MarkLoginStateError()
					Debuger.LogErrorSD("LoginCtrl:RecvLoginToRole:ConnectLogicServer ERROR---Ret=%s",err)
				end
			end)
	end
-- end,1)
end

function LoginCtrl:RequestCrossLogin()
	self.data:SetLoginState(LoginStateType.CrossServerConnecting)
	NetworkCtrl.Instance:ConnectCrossServer(function(err)
		if StringIsEmpty(err) == true then
			NetworkCtrl.Instance:DisconnectLogicServer()
			VoMgr:Clear()
			self:RequestLoginScene()
			self.data:BaseData().is_on_crosssever = true
		else
			self.data:MarkLoginStateError()
			Debuger.LogErrorSD("LoginCtrl:RecvCrossToRole:ConnectCrossServer ERROR---Ret=%s",err)
		end
	end)
end

function LoginCtrl:RecvCrossStartAck(protocol)
	self.data:SaveLoginRoleData(RoleData.Instance:GetBaseData())
	-- self.data:SetLoginState(LoginStateType.CrossServerConnecting)
	self.data:SetWaitStr(Language.Login.Crossing)

	local role_base_data = RoleData.Instance:GetBaseData()
	role_base_data.role_id = protocol.new_uid
	role_base_data.scene_id = protocol.scene_id
	role_base_data.last_scene_id = protocol.last_scene_id
	role_base_data.session_key = protocol.key
	role_base_data.role_name = protocol.role_name
	role_base_data.time = protocol.time
	NetworkCtrl.Instance:SetCrossServerHost(protocol.geteway_hostname, protocol.gateway_port)
	self:RequestCrossLogin()
end
function LoginCtrl:RecvReturnOriginalSever()
	self:RequestBackLogicServer()
end
function LoginCtrl:RecvBackToOriginServer(protocol)
	NetworkCtrl.Instance:SetLogicServerHost(protocol.gateway_hostname, protocol.gateway_port)
	NetworkCtrl.Instance:DisconnectLogicServer()
	self:RequestBackLogicServer(protocol)
end
-- 跨服返回原服
function LoginCtrl:RequestBackLogicServer(extra)
	if self.data:BaseData().is_on_crosssever == false then
		PublicPopupCtrl.Instance:Center(Language.Login.NoOnCross)
		return
	end
	self.data:SetWaitStr(Language.Login.BaceOriginSever)
	self.data:SetLoginState(LoginStateType.LogicServerConnecting)
	NetworkCtrl.Instance:ConnectLogicServer(function(err)
		if StringIsEmpty(err) == true then
			NetworkCtrl.Instance:DisconnectLoginServer()
			NetworkCtrl.Instance:DisconnectCrossServer()
			PublicPopupCtrl.Instance:ShowWait(self.data:WaitStr())
			VoMgr:Clear()
			-- 使用原数据进行登录场景即可返回
			self:RequestLoginScene(true,extra)
			self.data:BaseData().is_on_crosssever = false
		else
			self.data:MarkLoginStateError()
			Debuger.LogError("LoginCtrl:RecvLoginToRole:ConnectLogicServer ERROR---Ret=%s",err)
		end
	end)
end

function LoginCtrl:RequestLoginScene(is_cross,extra)
	if self.data:LoginState() ~= LoginStateType.LogicServerConnecting and self.data:LoginState() ~= LoginStateType.CrossServerConnecting then
		return
	end
	self.data:SetLoginState(LoginStateType.SceneLogining)
	local role_base_data = RoleData.Instance:GetBaseData()
	if is_cross == true then
		role_base_data = self.data:RoleData()
	end
	extra = extra or {}

	local protc = GetProtocol(CSUserEnterGSReq)
	protc.is_login = 1
	protc.role_id = role_base_data.role_id
	protc.scene_id = extra.scene_id or role_base_data.scene_id
	protc.last_scene_id = extra.last_scene_id or role_base_data.last_scene_id
	protc.key = extra.session_key or role_base_data.session_key
	protc.role_name = role_base_data.role_name
	protc.time = extra.time or role_base_data.time
	protc.server = LoginData.Instance:GetLastLoginServer()
	protc.pname = self.data:GetPlatformName()
	protc.plat_spid = self.data:GetAccountSpid()
	SendProtocol(protc)	
end

function LoginCtrl:RecvLoginScene(protocol)
	if protocol.result >= LoginSceneResult.Success then
		self.data:SetLoginState(LoginStateType.Logined)
		self.data:BaseData().is_on_crosssever = (protocol.is_on_crosssever == 1)
		if self.data:BaseData().real_is_on_crossserver == (protocol.is_on_crosssever == 1) then
			self.data:BaseData():SetDirty("real_is_on_crossserver")
		else
			self.data:BaseData().real_is_on_crossserver = (protocol.is_on_crosssever == 1)
		end
		-- self:RequestAllInfo()
		-- MainCh.Create(RoleData.Instance:GetBaseData())
		if MainPlayer == nil then
			local vo = RoleData.Instance:GetBaseData()
			vo.obj_type = SceneObjType.MainRole
			SceneObj.CreateObj(vo)
		end
		TimeCtrl.Instance:ClearDelayTime()
		self:LoginEventReport()
		TimeHelper:AddDelayTimer(function ()
			MainViewData.Instance:FlushRbButton()
		end, 1)
	else
		self.data:MarkLoginStateError()
		Debuger.LogError("LoginCtrl:RecvLoginScene ERROR---Ret=%s",tostring(protocol.result))
	end
	MainProber:Step(MainProber.STEP_ENTER_SCENE_END, MainProber.user_id, MainProber.server_id, MainProber.role_name, MainProber.role_id)
end

function LoginCtrl:RequestAllInfo()
	-- local protc = GetProtocol(CSAllInfoReq)	
	-- SendProtocol(protc)
end

function LoginCtrl:SendRandomNameReq()
	if GetLocalType() == LocalType.VNMAND or GetLocalType() == LocalType.VNMIOS then
		return
	end
	local protocol = GetProtocol(CSRandomNameReq)
	protocol.sex = self.role_create_data.create_view_data.sex or 0
	SendProtocol(protocol)
end

function LoginCtrl:OnRandomName(protocol)
	if self.role_create_data:SetRandomName(protocol.name_info) then
		self:SendRandomNameReq()
	end
end

function LoginCtrl:OnDisconnectNotice(protocol)
	if protocol.reason == 1 then
		self.data:SetLoginState(LoginStateType.AccountConflict)
	end
end

function LoginCtrl:unloadGameLogicBeforeRelogin()
	NetworkCtrl.Instance:DisconnectLoginServer()
	NetworkCtrl.Instance:DisconnectLogicServer()
	NetworkCtrl.Instance:DisconnectCrossServer()
	self.data:SetLoginState(LoginStateType.None)
	if MainPlayer ~= nil then
		MainPlayer:DeleteMe()
	end
	VoMgr:Clear()		--reset scene net obj data
	SceneData.Instance:SetMode(0)--reset scene mode
	TeamData.Instance:IsAutoMatch().value = false
	if BattleData.Instance:BaseData().server_fighting == true then		--reset battle state
		BattleCtrl.Instance:GetStream():Clear()
		BattleData.Instance:BaseData().server_fighting = false
	end	
	ViewMgr:OnUnloadGameLogic()
	GuideManager.Instance:OnUnloadGameLogic()
	SmartData.NotifyEnable = false
	ModulesManager.Instance:OnUnloadGameLogic()
	SmartData.NotifyEnable = true
	LoginData.Instance:ClearIsNewChara()
end


function LoginCtrl:OnAccidentDisconnect()
	if self.data:LoginState() == LoginStateType.AccountConflict then
		self:HintToReturnLogin(Language.Login.AccountConflict)
	elseif self.data:LoginState() == LoginStateType.RoleLogining then
		self:HintToReturnLogin(Language.Login.NetFAILReloginHint)
	elseif self.data:LoginState() == LoginStateType.RoleCreateRequesting then
		self:HintToReturnLogin(Language.Login.NetFAILReloginHint)
	elseif self.data:LoginState() == LoginStateType.RoleCreating then
		self:HintToReturnLogin(Language.Login.NetFAILReloginHint)
	else
		LogError("因不明原因断线！",self.data:LoginState())
	    self.data:SetWaitStr(Language.Login.Reconnecting)
		self:ReloginQuietly()
	end
end

--静默重连
function LoginCtrl:ReloginQuietly()
	self:unloadGameLogicBeforeRelogin()
	PublicPopupCtrl.Instance:ShowWait(self.data:WaitStr())
	local re_login_func = function () 
		self:RequestServerAccountLogin(function(ret)
			if ret >= AccountLoginResult.Success then
				-- if self.data:BaseData().is_on_crosssever == true then
				-- 	self:RequestCrossLogin()
				-- else
					if LoginData.Instance:GetLoginBackType() == RELOGIN_TYPE.ROLE_SELECT then 
						SceneCtrl.Instance:GotoScene(LoginScene)
					else 
						self:RequestRoleLogin()
					end 
					-- self:RequestRoleLogin()
					-- LoginData.Instance:Clear() 
				-- end
			elseif ret == AccountLoginResult.NoRole then
				-- ViewMgr:OpenView(StoryOptionsView)
				-- 这里需要判断是否进入小游戏
				if not LoginData.Instance:IsPlayMiniGame() then
					SceneCtrl.Instance:GotoScene(RoleCreateScene)
				else
					MiniGameCtrl.Instance:GoToMiniGame()
				end
			end
		end)
	end
	if false then		--每次断线 都做一下版本检测，如果本地资源 版本与后台不一，则重启游戏进行版本检测
		local url = GameStateInit.GetQueryURL(true)
		WebRequester:Request(url,function(data,error)
			if StringIsEmpty(error)  and data ~= GLOBAL_CONFIG:PkgInfo().assets_version then
				LoginData.Instance:MarkAssetVersionUnmatch()
				self:HintToReturnLogin(Language.Login.NewAssetVersionRelogin)
			else
				re_login_func()
			end
		end)
	else
		re_login_func()
	end
end

--提示并返回登录界面
function LoginCtrl:HintToReturnLogin(hint_con,has_cancel,view)
	PublicPopupCtrl.Instance:HideWait()
	local tip_data = {
		content = hint_con,
		confirm = {
			func = function()
			    if view then
			    	ViewMgr:CloseView(view)
			    end
				if LoginData.Instance:AssetVersionUnmatch() then
					RebootGame()
				else
					ChannelAgent:Logout(function ()
						self:unloadGameLogicBeforeRelogin()
						LoginData.Instance:Clear()
						SceneCtrl.Instance:GotoScene(LoginScene)
					end)
					PublicPopupCtrl.Instance:CloseDialogTips()
				end
			end
		},
	}

	tip_data.no_cancel = not (has_cancel == true)
	if self.data:LoginState() == LoginStateType.RoleCreating then
		RoleCreateData.Instance:LockUpDirector(true)
	end
	-- 小游戏相关界面打开了才执行
	local is_need_delay = false
	if MiniGameCtrl.Instance:IsOpenMiniGameView(true) then
		-- 这里需要判断npc对话界面是否打开，需要先关掉
		if ViewMgr:IsOpen(NpcDialogView) then
			ViewMgr:CloseView(NpcDialogView)
			is_need_delay = true
		end
	end
	-- 关闭小游戏相关界面后需要延迟一帧再打开对话框，否则会打不开
	if is_need_delay then
		TimeHelper:AddRunFrameTimer(function()
			PublicPopupCtrl.Instance:DialogTips(tip_data)
		end, 1, 1, false)
	else
		PublicPopupCtrl.Instance:DialogTips(tip_data)
	end
	if false then	--每次断线 都做一下版本检测，如果本地资源 版本与后台不一，则重启游戏进行版本检测
		local url = GameStateInit.GetQueryURL(true)
		WebRequester:Request(url,function(data,error) 
			if not StringIsEmpty(error) then
				return
			end
			if data ~= GLOBAL_CONFIG:PkgInfo().assets_version then
				LoginData.Instance:MarkAssetVersionUnmatch()
			end
		end)
	end
end


function LoginCtrl:OnLoginStateChange()
	PowerSavingView.ExitSavingPowerMode()
	local state_error = self.data:GetLoginStateError()
	local login_state = self.data:LoginState()
	-- Debuger.LogError("OnLoginStateChange======%d",state_error)
	-- Debuger.LogError("OnLoginStateChange======%d",login_state)
	if state_error ~= 0 then
		if self.data:LoginTryCountGrow() > LoginData.MaxLoginTryCount then
			local con = string.format("%s[ERROR=%d]",Language.Login.ReloginHint,state_error)
			self.data:LoginTryCountReset()
			self:HintToReturnLogin(con)
		else     --尝试重连N次
			TimeHelper:AddDelayTimer(function ()
				-- Debuger.LogError("Reconnect!!!!!!!=====%d",self.data:LoginTryCount())
			self:ReloginQuietly()
			end,LoginData.LoginTrySpace)
		end
	else
		local login_state = self.data:LoginState()
		if login_state == LoginStateType.RoleLogining or 
			login_state == LoginStateType.LogicServerConnecting or login_state == LoginStateType.SceneLogining
			or login_state == LoginStateType.CrossServerConnecting then
			PublicPopupCtrl.Instance:ShowWait(self.data:WaitStr())
		else
			self.data:LoginTryCountReset()
			-- PublicPopupCtrl.Instance:HideWait()
		end
	end
end

function LoginCtrl:SendBackToOriginServer()
	local protocol = GetProtocol(CSBackToOriginServer)
	SendProtocol(protocol)
end

function LoginCtrl:SendCrossStartReq()
	local protocol = GetProtocol(CSCrossStartReq)
	SendProtocol(protocol)
end

function LoginCtrl:SendRoleSubscribe(is_unsubscribe, topic)
	local protocol = GetProtocol(CSRoleSubscribe)
	protocol.is_unsubscribe = is_unsubscribe
	protocol.topic = topic
	SendProtocol(protocol)
end

function LoginCtrl:RoleSubscribe(topic)
	self:SendRoleSubscribe(0, topic)
end

function LoginCtrl:RoleSubscribeCancel(topic)
	self:SendRoleSubscribe(1, topic)
end

function LoginCtrl:ClickDirectorAvatarType(avatar_type)
	local data = self.role_create_data:GetCharaDataByAvatarType(avatar_type)
	if nil == data then return end
	self.role_create_data:SetDirectorCharaData(data)
end

function LoginCtrl:ClickAvatarType(avatar_type)
	local data = self.role_create_data:GetCharaDataByAvatarType(avatar_type)
	if nil == data then return end
	self.role_create_data:SetRoleCharaData(data)
	local cur_scene = SceneCtrl.Instance:CurScene()
	if cur_scene.RoleCreateShow then
		cur_scene:RoleCreateShow(data.role_id)
	end
end

function LoginCtrl:ClickAvatarTypeProf(prof_index)
	local cur_scene = SceneCtrl.Instance:CurScene()
	if cur_scene.RoleCreateShowProf then
		cur_scene:RoleCreateShowProf(prof_index)
	end
end

function LoginCtrl:SendStoryBattleInfo()
	local protocol = GetProtocol(CSStoryBattleInfo)
	SendProtocol(protocol)
end
function LoginCtrl:SendStoryRoundResult()
	local protocol = GetProtocol(CSStoryRoundResult)
	self.story_round = self.story_round + 1
	protocol.round = self.story_round
	SendProtocol(protocol)
end
--无法进行剧情战斗
function LoginCtrl:RecvStoryNotAvaliable(protocol)
	self.story_round = -1
	BattleCtrl.Instance:GetStream():Clear()
    BattleData.Instance:Clear()
	SceneCtrl.Instance:GotoScene(RoleCreateScene)
end

function LoginCtrl:RequestForbidTips()
	if not StringIsEmpty(GLOBAL_CONFIG:Urls().role_freeze_url) then
		local url = GLOBAL_CONFIG:Urls().role_freeze_url.."?spid=".. self.data:AccountData().spid
		WebRequester:Request(url,function(data,error)
			if StringIsEmpty(error) == true then
				local remote_config,suc = TryParseJson(data)
				if suc == true and remote_config.data ~= cjson.null then
					self.data:SetForbidTipInfo(remote_config)
				else	--remote data error 
					self.data.role_forbid_use_php = false
				end
			else	--remote request failed
				self.data.role_forbid_use_php = false
			end
		end)
	else
		self.data.role_forbid_use_php = false
	end
end

-- 请求登录公告
function LoginCtrl:RequestLoginNotice()
    local url = (GLOBAL_CONFIG:Urls().update_notice_query_url or "http://cls.tt06.bluegames.cn/api/c2s/fetch_notice.php").."?spid="..self.data:AccountData().spid
    WebRequester:Request(url,function(data,error)
		if StringIsEmpty(error) == true then
			local remote_config,suc = TryParseJson(data)
LogError("LoginCtrl:RequestLoginNotice suc=",tostring(suc)," data=",tostring(data))
			if suc == true and remote_config.data ~= cjson.null then
				self.data:SetLoginNoticeInfo(remote_config)
            else	--remote data error 
			end
		else	--remote request failed
		end
	end)
end

-- 请求隐私政策
function LoginCtrl:RequestUserProtocol()
	-- local pkg_config = GLOBAL_CONFIG:PkgInfo()
	local url = (GLOBAL_CONFIG:Urls().user_protocol_url or "http://cls.tt06.bluegames.cn/api/c2s/fetch_privacy_notice.php").."?spid=".. (GLOBAL_CONFIG:Urls().spid or "")

	WebRequester:Request(url,function(data,error)
		if StringIsEmpty(error) == true then
			local remote_config,suc = TryParseJson(data)
			if suc == true and remote_config.data ~= cjson.null then
				self.data:SetLoginUserProtocol(remote_config)
            else	--remote data error 
			end
		else	--remote request failed
		end
	end)
end

function LoginCtrl:RequestDeleteAccountNotice()
	if StringIsEmpty(GLOBAL_CONFIG:Urls().del_account_url) then
		return false
	end
	local url = GLOBAL_CONFIG:Urls().del_account_url.."?spid="..(GLOBAL_CONFIG:Urls().spid or "")
	WebRequester:Request(url, function(data, error)
		if StringIsEmpty(error) == true then
			local remote_config, suc = TryParseJson(data)
			if suc == true and remote_config.data ~= cjson.null then
				self.data:SetDeleteAccountNotice(remote_config)
			end
		end
	end)
	return true
end


--GM命令工具中模拟断线逻辑
function LoginCtrl.GMDisconnect()
	LoginCtrl.Instance:unloadGameLogicBeforeRelogin();
	LoginCtrl.Instance:HintToReturnLogin(Language.Login.NetFAILReloginHint)
	-- NetworkCtrl.Instance:TestDisconnect()
end

function LoginCtrl.GMDisconnect2()
	-- LoginCtrl.Instance:unloadGameLogicBeforeRelogin();
	-- LoginCtrl.Instance:HintToReturnLogin(Language.Login.NetFAILReloginHint)
	NetworkCtrl.Instance:TestDisconnect()
end

function LoginCtrl:LoginEventReport(event)
	local url = GLOBAL_CONFIG:Urls().event_url
	if nil == url or "" == url then
		-- LogError("LoginEventReport return 1 ",GLOBAL_CONFIG:PkgInfo().device_id)
		return
	end
	url = url .. "?type=%s&spid=%s&user_id=%s&role_id=%s&role_name=%s&server_id=%s&time=%s&device_id=%s&sign=%s&uuid=%s"

	local role_base_data = RoleData.Instance:GetBaseData()
	local time_stamp = os.time()
	local spid = GLOBAL_CONFIG:PkgInfo().agent_id
	local user_id =  string.gsub(self.data:GetPlatformName(), GLOBAL_CONFIG:PkgInfo().agent_id .. "_", "")
	local device_id = GLOBAL_CONFIG:PkgInfo().device_id
	local uuid = device_id
	if VersionTag.HasTag(VersionTag.FileInfoJson) then
		local temp = MainProber.GetUUID()
		if not IS_IOS() then
			uuid = temp
		else
			if tonumber(string.sub(temp,1,8)) ~= nil and tonumber(string.sub(temp,1,8)) == 0 then
				uuid = "uu_" .. uuid
			else
				uuid = temp
			end
		end
	end
	local sign = GameUtil.StrMd5(spid .. self.data:GetLastLoginServer() .. user_id .. role_base_data.role_id .. time_stamp .. device_id .. "hdISla9sjXphPqEoE8lZcg==")
	url = string.format(url,
						1,--event
						spid,
						user_id,
						role_base_data.role_id,
						"",
						self.data:GetLastLoginServer(),
						time_stamp,
						device_id,
						sign,
						uuid)
	-- print("EventReport, url=" .. url)
	-- HttpClient:Request(url, "")
	if GLOBAL_CONFIG:Flags().log_print then
		print("EventReport url "..tostring(url))
	end
	WebRequester:Request(url,function(data,error)
		-- if StringIsEmpty(error) == true then
		-- 	local data_t,suc = TryParseJson(data)
		-- 	if suc == true then
		-- 		LogError("data_t ",data_t)
		-- 	end
		-- end
	end)
end

function LoginCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    if nil == obj_vo then
        return
    end
	if obj_vo.seq == 574 then
		local is_cross = LoginData.Instance:IsOnCrossSever()
		local button = NpcDialogBehBtnData.FuncCreate(
			Language.Login.CrossEnvoy[is_cross and "out_cross" or "in_cross"],
			function ()
				if is_cross then
					LoginCtrl.Instance:SendBackToOriginServer()
				else
					LoginCtrl.Instance:SendCrossStartReq()
				end
			end,
			true
		)
		table.insert(btn_list, button)
    end
end

function LoginCtrl:OnReboot()
	VoiceRecorder.Unload()
end

function LoginCtrl:SendRootPkgLoginVerify(login_data, times)
	LogError("login_data:",login_data)	
	local url_head = string.format("http://%s/get_login_kf_pack.php",GLOBAL_CONFIG:GetUrlHead())
	-- LogError("login_data",login_data)
	local param = login_data.Token or ""
	local userName = login_data.AccountId or ""
	local url_format = "%s?spid=%s&userId=%s&param=%s&token=1"



	local real_url = string.format(url_format, url_head, ROOT_PKG_REQ_AGENT_ID or "", userName, param)
	-- LogError("real_url",real_url)
	WebRequester:Request(real_url,function(data,error)
		if StringIsEmpty(error) then
			self:RecvLoginVerify(data, trytime, user_id)
		else
			MainProber:Step(MainProber.STEP_PHP_VERIFY_END, user_id, error)
			LogError("request loginVerify failed:",error)
		end
	end)
	self.sended_verify = true
	self.login_verify_state = LoginVerifyState.WAITFORVERIFY
	--固定循环发送3秒一次
	if self.verify_runner == nil then
		self.verify_runner = TimeHelper:AddRunTimer(function ()
			self.sended_verify = false
			if not self.returned_verify then
				self:SendRootPkgLoginVerify(login_data)
			else
				TimeHelper:CancelTimer(self.verify_runner)
			end
		end, 3, nil, false)
	end
end

function LoginCtrl:SendLoginVerify(login_data, times)
	if GLOBAL_CONFIG:PkgInfo().root_pkg then
		self:SendRootPkgLoginVerify(login_data, times)
		return
	end
	local trytime = times or 1
	-- LoginData.Instance:SaveLastLoginAccount(login_data.AccountId, login_data.Token)
	local url_head = GLOBAL_CONFIG:Urls().verify_url
	if trytime > 2 then
		url_head = GLOBAL_CONFIG:Urls().verify_url2
	end

	if IS_EDITOR or (PlatChannelAgent.GetAgentId() == "dev" and not GLOBAL_CONFIG:PkgInfo().is_youyan) then
		url_head = GLOBAL_CONFIG:Urls().verify_url
	end
	
	local timestamp = login_data.TimeStamp or os.time()
	-- local timestamp = 1627311117--login_data.TimeStamp or os.time()
	-- LogError("timestamp",timestamp)
	local token = login_data.Token or ""
	local userName = login_data.UserName or ""
	local param = ""
	LogError("login_data.DynamicParams: ", login_data.DynamicParams)
	if login_data.DynamicParams ~= nil and login_data.DynamicParams.Length > 0 then
		param = login_data.DynamicParams[0]
	end
	local token2  = ""
	if login_data.CustomArray ~= nil then
		local iter = login_data.CustomArray:GetEnumerator()
		while iter:MoveNext() do
			local k = tostring(iter.Current.Key)
			local v = tostring(iter.Current.Value)
			if k == "Token2" then
				token2 = v
			elseif k == "CurrencyType" then
				RechargeData.Instance.currency_type = tonumber(v)
			end
		end
	end
	self.last_check_time = os.time()
	local pkg_config = GLOBAL_CONFIG:PkgInfo()
	local send_account = login_data.AccountId

	-- if not EditorSettings:OfficialLogin() then
	-- 	send_account = "dev_" .. send_account
	-- end
	local url_format ="%s?spid=%s&userId=%s&timestamp=%s&sign=%s&device=%s&param=%s&token2=%s&check_time=%s&pkg=%s&asset=%s&username=%s"
	LogError("url_head:",url_head)	
	LogError("spid:",GLOBAL_CONFIG:PkgInfo().agent_id)
	LogError("userId:",send_account)
	LogError("timestamp:",timestamp)
	LogError("sign:",token)
	LogError("device:",GLOBAL_CONFIG:PkgInfo().device_id)
	LogError("param:",param)
	LogError("token2:",token2)
	LogError("最后一次发送登录验证的时间戳:",self.last_check_time)
	LogError("包版本号，例1.0.0:",pkg_config.version)
	LogError("客户端资源版本号:",pkg_config.assets_version)
	LogError("userName:",userName)




	local real_url = string.format(url_format,
			url_head,
			GLOBAL_CONFIG:PkgInfo().agent_id, 				--spid
			send_account,		 							--userId
			timestamp, 										--timestamp
			token, 											--sign
			GLOBAL_CONFIG:PkgInfo().device_id,				--device
			param,											--param
			token2, 										--token2
			self.last_check_time,							--最后一次发送登录验证的时间戳
			pkg_config.version,								--包版本号，例1.0.0
			pkg_config.assets_version,						--客户端资源版本号
			userName
			)

	--登录验证url混淆
	if not IS_EDITOR and IS_AUDIT_VERSION then
		local param_str = self:GetVerifyParam({[1] = GLOBAL_CONFIG:PkgInfo().agent_id, [2] = send_account, [3] = timestamp, [4] = token, [5] = GLOBAL_CONFIG:PkgInfo().device_id, 
				[6] = param, [7] = token2, [8] = self.last_check_time,[9] = userName}) --传入param_list
		real_url = string.format("%s?%s", url_head, param_str)
	end
	self.returned_verify = false
	local user_id = tostring(self.data:GetAccountSpid().."_"..login_data.AccountId)
	MainProber:Step(MainProber.STEP_PHP_VERIFY_BEG, user_id, url_head)
	if GLOBAL_CONFIG:Flags().log_print then
		print("login_verify_url "..tostring(real_url))
		if ChannelAgent.PCFileLog and IS_STANDALONE() then
			ChannelAgent.PCFileLog("login_verify:"..real_url)
		end
	end
	--发送
	WebRequester:Request(real_url,function(data,error)
 	   	LogError("Request, Send, real_url: " , real_url)
	    LogError("Request, Send, data: " , data)
		if StringIsEmpty(error) then
				self:RecvLoginVerify(data, trytime, user_id)
			else
				MainProber:Step(MainProber.STEP_PHP_VERIFY_END, user_id, error)
				LogError("request loginVerify failed:",error)
			end
		end)
	self.sended_verify = true
	self.login_verify_state = LoginVerifyState.WAITFORVERIFY
	--固定循环发送3秒一次
	if self.verify_runner == nil then
		self.verify_runner = TimeHelper:AddRunTimer(function ()
			self.sended_verify = false
			if not self.returned_verify then
				self:SendLoginVerify(login_data)
			else
				TimeHelper:CancelTimer(self.verify_runner)
			end
		end, 3, nil, false)
	end
end

function LoginCtrl:RecvLoginVerify(data, try_times, sended_user_id)
	self.returned_verify = true
	-- print("RecvLoginVerify "..data)
	--收到验证结果时如果状态是等待php或已经登陆那么return
	if self.login_verify_state ~= LoginVerifyState.WAITFORVERIFY and self.login_verify_state ~= LoginVerifyState.LOGINCOMPILETE then
		LogError("LoginVerifyByPhp, callback return 1.2 ")
		return
	end

	self.is_login_verifying = false
	self.returned_verify = true
	local ret_t, suc = TryParseJson(data)
	if suc ~= true then
		self:OpenVerifyFailAlert(string.format(Language.Login.LoginVerifyJsonillegal, "1"))
		MainProber:Step(MainProber.STEP_PHP_VERIFY_END, sended_user_id, "return_not_json")
		return
	end
	if type(ret_t) ~= "table" or not next(ret_t) then
		if try_times == nil then
			try_times = 1
		end
		if try_times < 4 then
			self:SendLoginVerify(login_data, try_times + 1)
			return
		end
		--失败回调
		LoginCtrl.Instance.login_verify_state = LoginVerifyState.NOLOGIN
		LogError("LoginVerifyByPhp, callback return 2")
		self:OpenVerifyFailAlert(string.format(Language.Login.LoginVerifyFailed,"2"))
		MainProber:Step(MainProber.STEP_PHP_VERIFY_END, sended_user_id, "timeout")
		return
	end

	if ret_t.data ~= nil then
		local b64data = GameUtil.DecodeB64(string.sub(tostring(ret_t.data), 6, -1))

		local b64dataT, succ  = TryParseJson(b64data)
		if succ ~= true then
			self:OpenVerifyFailAlert(string.format(Language.Login.LoginVerifyJsonillegal, "2"))
			MainProber:Step(MainProber.STEP_PHP_VERIFY_END, sended_user_id, "return_data_not_json")
			return
		end
		ret_t = b64dataT
	end

	if nil == ret_t or nil == ret_t.user or ret_t.user.account == nil then
		--失败回调
		LoginCtrl.Instance.login_verify_state = LoginVerifyState.NOLOGIN
		LogError("服务器返回数据:" ..data) -- 添加这行日志
		LogError("LoginVerifyByPhp, callback return 3 ret:"..tostring(ret_t.ret).."_msg:"..tostring(ret_t.msg))
		self:OpenVerifyFailAlert(string.format(Language.Login.LoginVerifyFailed, "3"))
		if ret_t ~= nil then
			MainProber:Step(MainProber.STEP_PHP_VERIFY_END, sended_user_id, "ret:"..tostring(ret_t.ret).."_msg:"..tostring(ret_t.msg))
					LogError("LoginVerifyByPhp, 1")
		else
			MainProber:Step(MainProber.STEP_PHP_VERIFY_END, sended_user_id, "ret_is_nil")
					LogError("LoginVerifyByPhp, 2")
		end
		return
	end

	local verify_accout = string.sub(ret_t.user.account, 5, -1)
	--若收到的验证信息时间戳和最后一次发送的是否相同
	if ret_t.user.check_time and ret_t.user.check_time ~= self.last_check_time then
		LogError("LoginVerifyByPhp, callback return 4")
		return 
	end
	--若收到的验证结果是该账号已经登陆则return
	if self.login_verify_state == LoginVerifyState.LOGINCOMPILETE and verify_accout == self.data.account.key then
		LogError("LoginVerifyByPhp, callback return 5")
		return
	end



	self.sended_verify = false

	if ret_t.server_info then
		GLOBAL_CONFIG:SetServerInfo(ret_t.server_info)
	end

	self.data.account.spid = ret_t.user.spid
	self.data.account.login_str = ret_t.user.login_sign
	self.data.account.login_time = ret_t.user.login_time
	self.data.account.plat_name = ret_t.user.account
	self.data.account.access_token = ret_t.user.access_token
	self.data.account.account_type = ret_t.user.account_type
	-- 封测累计充值金额
	self.data.account.fc_recharge = ret_t.user.fc_recharge or 0
	-- 封测返回倍率
	self.data.account.rate = ret_t.user.rate or 0
	-- 封测累计登陆了几天
	self.data.account.login_day = ret_t.user.login_day or 0
	-- 返还元宝数
	self.data.account.fill_gold = ret_t.user.fill_gold or 0
	-- 小游戏
	self.data.account.mini_game = ret_t.user.mini_game or 0

	--纯内网
	if not EditorSettings:OfficialLogin() then
		self.data.account.key = verify_accout
		--LoginData.Instance:SaveLastLoginAccount(verify_accout, ret_t.user.access_token)

	--unity登外网
	elseif IS_EDITOR or (PlatChannelAgent.GetAgentId() == "dev" and not GLOBAL_CONFIG:PkgInfo().is_youyan) then
		self.data.account.key = verify_accout
		--LoginData.Instance:SaveLastLoginAccount(ret_t.user.uid, ret_t.user.access_token)

	--纯外网（游戏包内）
	else
		self.data.account.key = ret_t.user.uid
		--LoginData.Instance:SaveLastLoginAccount(verify_accout, ret_t.user.access_token)
	end

	-- 重置是否需要玩小游戏标志
	self.data:SetIsPlayMiniGame(false)

	--检查上次登录服务器是用的本地缓存还是推荐服
	local server, use_cache_server = self.data:GetLastLoginServer()
	-- LogError("loginverify role_data",ret_t.role_data == cjson.null, not StringIsEmpty(ret_t.role_data))
	if not use_cache_server and ret_t.role_data ~= cjson.null and not StringIsEmpty(ret_t.role_data) then
		
		local last_server_id = 0
		local last_login_time = 0
		for k,v in pairs(ret_t.role_data) do
			if tonumber(v.server_id) < 2000 and (last_login_time == 0 or v.last_login_time > last_login_time) then
				last_login_time = v.last_login_time
				last_server_id = tonumber(v.server_id)
			end
		end
		if last_server_id ~= server then
			self.data:SaveLastLoginServer(self.data:GetAccountKey(), last_server_id)
		end
	else
		-- 证明该账号在任何服务器上没有角色，unity上的只会用本地的缓存去判断，所以unity上的判断不准确
		self:JudgePlayMiniGame()
	end
	

	MainProber:Step(MainProber.STEP_PHP_VERIFY_END, ret_t.user.account, true)
	--成功回调
	self.login_verify_state = LoginVerifyState.LOGINCOMPILETE
	--QQPC包获取大玩咖信息
	self:SendGetQQPCPlayerInfo()

	if IS_STANDALONE() then
		UnityPlayerPrefs.SetString(PrefKeys.AccountLoginTimes(), tostring(os.time()))
	end
end

-- 小游戏判断
function LoginCtrl:JudgePlayMiniGame()
	-- 再多加一层判断
	local server_list = self.data:GetServerData()
	if not TableIsEmpty(server_list) and not TableIsEmpty(server_list[1]) then
		-- 这里有可能没有推荐服
		if TableIsEmpty(server_list[1][1]) then
			if self.data:GetMiniGame() > 0 then
				-- 该账号很干净，需要进入小游戏
				self.data:SetIsPlayMiniGame(true)
			end
		elseif not TableIsEmpty(server_list[1][1]) and not server_list[1][1].role_id then
			if self.data:GetMiniGame() > 0 then
				-- 该账号很干净，需要进入小游戏
				self.data:SetIsPlayMiniGame(true)
			end
		end
	end
end

function LoginCtrl:OpenVerifyFailAlert(str)	
	local info = {
        content = str,
        confirm = {
        	name = Language.Login.ReLogin,
            func = function()
				LoginCtrl.Instance:Data():SetAccountKey("")
				self:PlatformAccountLogin()
				-- ViewMgr:OpenView(LoginView)
                ViewMgr:CloseView(DialogTipsView)
            end
        },
        no_cancel = true,
        close = false
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

function LoginCtrl:GetVerifyParam(param_list)
	local temp_t = LoginCtrl.GetRequestParam()
	local data = {
		spid = temp_t[1] or "temp_t1",
		userId = temp_t[2] or "temp_t2",
		timestamp = temp_t[3] or "temp_t3",
		sign = temp_t[4] or "temp_t4",
		device = temp_t[5] or "temp_t5",
		param = temp_t[6] or "temp_t6",
		token2 = temp_t[7] or "temp_t7",
		check_time = temp_t[8] or "temp_t8",
		username = temp_t[9] or "temp_t9",
	}
	data = GameUtil.EncodeB64(cjson.encode(data))
	local randomstr = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLOMNOPQRSTUVWXYZ"
	for i=1,6 do
		local x = math.random(1, 63)
		data = string.sub(randomstr, x , x) .. data
	end

	local param_str = ""
	local addIndex = 9
	for i=1, 9 do
		param_str = param_str .. (temp_t[i] or "temp_t"..tostring(i) ).. "=" .. (param_list[i] or "") .. "&"
		if temp_t[i + addIndex] ~= nil and temp_t[i + addIndex + 1] ~= nil then
			param_str = param_str .. temp_t[i + addIndex] .. "=" .. temp_t[i + addIndex + 1] .. "&"
			addIndex = addIndex + 2
		end
	end
	param_str = "data=" .. data .. "&" .. param_str
	return param_str
end

function LoginCtrl.GetRequestParam()
	if Global_word_lib == nil then
		require("preload/word_library")
	end
	local a,b,c = string.byte(GLOBAL_CONFIG:PkgInfo().agent_id, 1, 3)
	a = a <= 57 and a - 46 or a
	a = a >= 97 and a - 86 or a
	b = b <= 57 and b - 46 or b
	b = b >= 97 and b - 86 or b
	c = c <= 57 and c - 46 or c
	c = c >= 97 and c - 86 or c
	local lib_count = #Global_word_lib
	local total = a * 36 * 36 + b * 36 + c
	-- UnityEngine.Debug.LogError("ASCII "..tostring(a).." "..tostring(b).." "..tostring(c).." Global_word_lib count = "..tostring(#Global_word_lib).." total = "..tostring(total).." str = "..tostring(str))
	local param_t = {
		[1] = Global_word_lib[(total + 100) % lib_count],
		[2] = Global_word_lib[(total + 200) % lib_count],
		[3] = Global_word_lib[(total + 300) % lib_count],
		[4] = Global_word_lib[(total + 400) % lib_count],
		[5] = Global_word_lib[(total + 500) % lib_count],
		[6] = Global_word_lib[(total + 600) % lib_count],
		[7] = Global_word_lib[(total + 700) % lib_count],
		[8] = Global_word_lib[(total + 800) % lib_count],
		[9] = Global_word_lib[(total + 900) % lib_count],
	}
	for i=1, 9 do
		table.insert(param_t, Global_word_lib[(total + 100 * (i + 9)) % lib_count])
	end
	return param_t
end

--跨服状态下发
function LoginCtrl:OnCrossConnectInfo(protocol)
	self.data:BaseData().is_connected_cross = protocol.is_connected_cross == 1
	self.data:BaseData().is_cross = protocol.is_cross == 1
end

function LoginCtrl:SendRealNameRewardReq()
	local protocol = GetProtocol(CSRealNameRewardReq)
	protocol.spid = GLOBAL_CONFIG:PkgInfo().agent_id
	SendProtocol(protocol)
end

function LoginCtrl:OnSCRealNameRewardInfo(protocol)
	self.data.real_name_reward_flag = protocol.receive_flag
end

--QQPC平台处理内容
function LoginCtrl:SendGetQQPCPlayerInfo()
	self.qq_big_player_is_open = false
	if ChannelAgent.ISPCSDK and ChannelAgent.ISPCSDK() then
        if ChannelAgent.PCAgentType and ChannelAgent.PCAgentType() == "QQ" then
			--登录验证后第一次发送忽略qq_big_player_is_open，后续游戏内定时发送获取则需要判断qq_big_player_is_open
			self:GetQQPCPlayerLevel(true)
			--QQPC保持OpenKey有效期
			self:ReqQQPCOpenKey()
		end
	end
end

--QQ平台大玩咖信息
--ignore是否忽略qq_big_player_is_open
function LoginCtrl:GetQQPCPlayerLevel(ignore)
	if self.qq_big_player_is_open == false and not ignore then
		return
	end 
    if StringIsEmpty(GLOBAL_CONFIG:Urls().app_key) or not VersionTag.HasTag(VersionTag.HMACSHA1) then
        return
    end
    
	local url_head = "openapi.minigame.qq.com/v3/user/get_dawanka_info?"
	local open_id = LoginData.Instance:GetAccountKey()
	local open_key = Nirvana.GameAgent.GetToken()
	local sig_str = "GET&"..WebRequester:UrlEncode("v3/user/get_dawanka_info").."&"
	sig_str = sig_str .. WebRequester:UrlEncode(Format("appid=1112173951&openid=%s&openkey=%s&pf=qqgame&version=2",
		open_id,
		open_key))
	local sig = GameUtil.HMACSHA1(sig_str, GLOBAL_CONFIG:Urls().app_key .. "&")
	local param = Format("appid=1112173951&openid=%s&openkey=%s&pf=qqgame&version=2",
		open_id,
		open_key).."&sig="..WebRequester:UrlEncode(sig)
	WebRequester:Request(url_head..param,function(data, error)
		if StringIsEmpty(error) then
			local ret, status = TryParseJson(data)
			if status then
				if ret.ret == 0 then
					self.qq_big_player_is_open = true
					QQBigPlayerData.Instance:SetQQBigPlayerInfo(ret, not ignore)
				end
				LogError("GetQQPCPlayerLevel ret",ret)
			else
				LogError("get qq_big_player info failed url=\n",url_head..param)
			end
		end
	end)    
end
--QQ平台保持OpenKey有效期
function LoginCtrl:ReqQQPCOpenKey()
	if self.openkey_runner ~= nil then
		TimeHelper.CancelTimer(self.openkey_runner)
		return
	end
	self.openkey_runner = TimeHelper:AddRunTimer(function ()
		local url = GLOBAL_CONFIG:Urls().is_login_url or "http://"..GLOBAL_CONFIG:GetUrlHead().."/api/is_login.php"
		local openid = LoginData.Instance:GetAccountKey() or ""
		local openkey =  Nirvana.GameAgent.GetToken() or ""
		local openkey_url = string.format("%s?openid=%s&openkey=%s&pf=%s", url, openid, openkey, "qqgame")
		LogError("is_login_url = "..openkey_url)
		WebRequester:Request(openkey_url,function (data, error)
			if StringIsEmpty(error) then
				local ret, status = TryParseJson(data)
				if status then
					LogError("is_login_url ret",ret)
				end
			end
		end)
	end,3600, nil, false)
	--每小时发送一次，init_call false
end