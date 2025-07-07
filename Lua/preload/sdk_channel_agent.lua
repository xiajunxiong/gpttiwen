local channel_agent = {
	login_callback = nil,
	name = "",
	role_id = 0,
}

--SDK登录操作  逻辑需要改一下

function channel_agent:Login(callback)
	self.login_callback = callback
	--走SDK接口登录
	PlatChannelAgent.Login()
end

--SDK注销登录操作
function channel_agent:Logout(callback)
	self.logout_callback = callback
	--走SDK接口注销登录操作
	PlatChannelAgent.Logout()
end

--SDK登录回调
function channel_agent:LoginCallback(logindata)

    --SDK登录回调操作判断上传的账号和KEY是否存在  后续网站二次验证
    if self.login_callback then
		local accountId_or_token = logindata.AccountId or logindata.Token
		if accountId_or_token then
			self.login_callback(logindata)
		else
			LoginCtrl.Instance:PlatformAccountLogin()
		end
	end
	
	
end


--支付走这边mai
function channel_agent:Mai(role_id, role_name, money, server_id, callback, gift_type)
  
    -- 渠道
    local spid = InstallPackageInfo:AgentId()
	-- 渠道账号
	local uid = LoginData.Instance:GetAccountKey()
    -- 等级
    local RoleLevel = RoleData.Instance:GetRoleLevel()
	   --  角色ID  角色名字 金额  区服ID  回调参数  一级 物品的唯一性ID
	   
	    --生成订单
		   --          区服ID_角色ID_渠道_渠道账号_时间戳_礼包ID
    local order_id = tostring(server_id .. "_"..role_id.."_"..spid.."_".. uid .. "_" ..os.time().."_"..gift_type)
	
	
	-- 
	local mai_request = Nirvana.MaiRequest.New()
	mai_request.OrderId = order_id
	mai_request.CurrencyAmount = money * 10
	mai_request.ProductId = ""
	mai_request.Count = 1
	mai_request.MoneyAmount = money
	mai_request.CurrencyName = Language.Common.Gold
	mai_request.ProductName = string.format("%d %s",mai_request.CurrencyAmount,mai_request.CurrencyName)
	mai_request.ProductDescription = ""
	PlatChannelAgent.Mai(mai_request)
end


--SDK登录注销回调操作
function channel_agent:LogoutCallback()
	if ViewMgr:IsOpen(WelcomeView) then
		--Accountkey设置为空时会通过smartData自动调用登录
		if StringIsEmpty(LoginData.Instance:GetAccountKey()) then
			LoginData.Instance.account:SetDirty("key")
		else
			LoginCtrl.Instance:Data():SetAccountKey("")
		end
		LoginCtrl.Instance.login_verify_state = LoginVerifyState.NOLOGIN
		-- LoginCtrl.Instance:PlatformAccountLogin()
	elseif self.logout_callback ~= nil then
			local cb = self.logout_callback
			self.logout_callback = nil
			cb()
	else
		LoginCtrl.Instance:unloadGameLogicBeforeRelogin()
		LoginData.Instance:Clear()
		SceneCtrl.Instance:GotoScene(LoginScene)
		-- RebootGame()
	end
end

--请求购买道具  没用
function channel_agent:RequestPay(money, param1,param2,param3)
	local pkg_config = GLOBAL_CONFIG:PkgInfo()
	local platform = LoginData.Instance:GetPlatformName()
	local server_id = LoginData.Instance:GetLastLoginServer()
	local role_id = RoleData.Instance:GetBaseData().role_id
	local gift_id = (param1 or 0).."-"..(param2 or 0).."-"..(param3 or 0)
	local role_name = RoleData.Instance.base_data.name
	self:Mai(role_id, role_name, money, server_id, nil, gift_id or 0)
	if IS_EDITOR then
		GMCmdCtrl.Instance:SendGMCommand("buyitem",string.format("%s %s %s %s",money * 10,param1 or 0,param2 or 0,param3 or 0))
	end
end

-- 初始化  
     --初始化SDK操作
function channel_agent:Init()
         --判断初始化操作
       if self.is_init then 
            return
			
	     end
	  self.is_init  = true
	 --SDK初始化登录回调操作
	PlatChannelAgent.SetLoginCallBack(function (logindata)
		self:LoginCallback(logindata)
	end)
	--SDK初始化登录注销回调
	PlatChannelAgent.SetLogoutCallBack(function ()
		self:LogoutCallback()
	end)
	PlatChannelAgent.SetDoExitAction(function ()
		self:DoExitAction()
	end)
	PlatChannelAgent.SetUserInfoRefreshAction(function (userInfo)
		self:GetUserInfo(userInfo)
	end)
	PlatChannelAgent.SetOnMessageCallBack(function (param)
		self:GetPlatformParam(param)
	end)
	--游戏初始化 用这个调用SDK的初始化操作
	PlatChannelAgent.Init()
end

function channel_agent:DoExitAction()

		local dialog_callback = function ()
			channel_agent:BehaviourTrigger(BehaviourType.RoleLogout)
			GameUtil.StopGame()
		end
		local dialog_callback_1 = function ()
		end
		Nirvana.DialogManager.showConfirm(PreLanguage.Quit, PreLanguage.QuitConfirm, PreLanguage.Confirm, dialog_callback, PreLanguage.Cancel,dialog_callback_1)

end
function channel_agent.ConnectPlatGM()
	print("点击了平台客服按钮")
end


function channel_agent.BehaviourTrigger(behaviour)
end

function channel_agent:GetUserInfo(userInfo)
	if RoleData == nil or RoleData.Instance == nil then
		return
	end
	local role_data = RoleData.Instance.base_data
	if role_data then
		userInfo.RoleId = role_data.role_id == 0 and channel_agent.role_id or role_data.role_id
		userInfo.RoleName = role_data.name == "" and channel_agent.role_name or role_data.name
		userInfo.RoleLevel = role_data.level
		userInfo.Currency = RoleData.Instance:GetCurrency(CurrencyType.CoinBind) or 0
		userInfo.Diamond = RoleData.Instance:GetCurrency(CurrencyType.Gold) or 0
		userInfo.Vip = 0
		userInfo.PartyName = StringIsEmpty(GuildData.Instance:GetGuildName()) and Language.Common.No or GuildData.Instance:GetGuildName()
		userInfo.Profession = RolePromoteData.GetProfName(RoleData.Instance:GetRoleProf())
		userInfo.RoleCreateTime = tostring(role_data.create_time)
	end
	local server_info = LoginData.Instance:GetServerInfoById(LoginData.Instance:GetLastLoginServer(), LoginData.Instance:GetLastLoginAccountSpid())
	if server_info then
		userInfo.ZoneId = server_info.id
		userInfo.ZoneName = server_info.name or ""
	end
	local account_info = LoginData.Instance:AccountData()
	userInfo.Sign = account_info.access_token
	userInfo.AccountId = account_info.key
	userInfo.LoginUrl = GLOBAL_CONFIG:Urls().verify_url or ""
	userInfo.ChargeUrl = GLOBAL_CONFIG:Urls().charge_url or ""
end

function channel_agent:Split(split_string, splitter)
	local split_result = {}
	local search_pos_begin = 1
	while true do
		local find_pos_begin, find_pos_end = string.find(split_string, splitter, search_pos_begin)
		if not find_pos_begin then
			break
		end
		split_result[#split_result + 1] = string.sub(split_string, search_pos_begin, find_pos_begin - 1)
		search_pos_begin = find_pos_end + 1
	end
	if search_pos_begin <= string.len(split_string) then
		split_result[#split_result + 1] = string.sub(split_string, search_pos_begin)
	end
	return split_result
end

-- 传参数到sdk层
function channel_agent.ToPlatWithParam(param)
	--	PlatChannelAgent.FacebookActive(param)

end

function channel_agent:GetPlatformParam(param)
	if param ~= "" and param ~= nil then
		if param == "onResume" then
			if NetworkCtrl.Instance then
				-- NetworkCtrl.Instance:HeartBeatPause(false)
				NetworkCtrl.Instance:ClearHeartBeatCache()
				NetworkCtrl.Instance:OnHeartBeatCheck()
			end
		elseif param == "onPause" then
			-- NetworkCtrl.Instance:HeartBeatPause(true)
		else
			local param_t = self:Split(param,"_")
			if param_t[1] == "insetTop" then
				if param_t[2] and param_t[2] ~= "0" then
					IS_IPHONEX = true
				end
			elseif param_t[1] == "recordPermission" then
				RecordPermission = param_t[2] == "1" --是否由语音权限的全局变量
			elseif param_t[1] == "channelId" then
				CHILD_CHANNEL_ID = param_t[2]
			end
		end
		if GLOBAL_CONFIG:Flags().log_print then
			print("dev GetPlatformParam "..tostring(param))
		end
	end
end

function channel_agent:reportMaiEvent(money)
    local url = GLOBAL_CONFIG:Urls().pay_event_url
    if nil == url or "" == url then
        return
    end
    if IS_AUDIT_VERSION then --审核服不上报充值
        return
    end
    url = url .. "?type=%s&spid=%s&user_id=%s&role_id=%s&server_id=%s&data=%s&time=%s&sign=%s"
    local sign =
        GameUtil.StrMd5(
        2 .. LoginData.Instance:AccountData().spid .. LoginData.Instance:GetPlatformName() .. MainProber.role_id .. MainProber.server_id .. os.time() .. "c8cae60e3fd8586258c1ea0c3ee00950"
    )
    url = string.format(url, 2, LoginData.Instance:AccountData().spid, LoginData.Instance:GetPlatformName(), MainProber.role_id, MainProber.server_id, money, os.time(), sign)
    if GLOBAL_CONFIG:Flags().log_print then
        print("EventReport, url=" .. url)
    end
    WebRequester:Request(
        url,
        function(data, error)
            if StringIsEmpty(error) then
                if GLOBAL_CONFIG:Flags().log_print then
                    print("reportMaiEvent " .. tostring(data))
                end
            end
        end
    )
end

return channel_agent