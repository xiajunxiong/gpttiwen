local channel_agent = {
	login_callback = nil
}
local pc_agent = nil
function channel_agent.ISPCSDK()
	return IS_STANDALONE() and VersionTag.HasTag(VersionTag.GameAgentPC) and tostring(Nirvana.GameAgent.GetGameAgentType()) ~= "None"
end

function channel_agent.PCAgentType()
	if channel_agent.ISPCSDK() then
		return tostring(Nirvana.GameAgent.GetGameAgentType())
	else
		return "None"
	end
end

--请求购买道具
function channel_agent:RequestPay(money,param1,param2,param3)
	local server_id = LoginData.Instance:GetLastLoginServer()
	local role_id = RoleData.Instance:GetBaseData().role_id
	local gift_id = (param1 or 0).."-"..(param2 or 0).."-"..(param3 or 0)
	if IS_IOS() then
		if param1 ~= 1 and money ~= 1 and money ~= 3 and money ~= 18 and not GLOBAL_CONFIG:PkgInfo().is_youyan then
			gift_id = "0_"..gift_id
		end
		if GLOBAL_CONFIG:PkgInfo().agent_id == "xqx" then
			gift_id = gift_id.."_0"
		end
	end
	local role_name = RoleData.Instance.base_data.name
	if GLOBAL_CONFIG:Flags().log_print then
		print("gift_id "..tostring(gift_id))
	end
	self:Mai(role_id, role_name, money, server_id, nil, gift_id or 0)
end

-- 初始化
function channel_agent:Init()
	if self.is_init then 
		return 
	end
	self.is_init = true
	PlatChannelAgent.SetLoginCallBack(function (logindata)
		self:LoginCallback(logindata)
	end)
	PlatChannelAgent.SetLogoutCallBack(function ()
		self:LogoutCallback()
	end)
	PlatChannelAgent.SetUserInfoRefreshAction(function (userInfo)
		self:GetUserInfo(userInfo)
	end)
	PlatChannelAgent.SetDoExitAction(function ()
		self:DoExitAction()
	end)
	PlatChannelAgent.SetOnMessageCallBack(function (param)
		print("Lua OnMessageCallBack agent param "..tostring(param))
		self:GetPlatformParam(param)
	end)
	PlatChannelAgent.Init()

	if channel_agent.ISPCSDK() then
		if channel_agent.PCAgentType() == "ShunWang" then
			pc_agent = require("preload/pc_agent/shunwang_agent")
		-- elseif channel_agent.PCAgentType() == "Thunder" then
		-- 	pc_agent = require("preload/pc_agent/thunder_agent")
		end
	end
end


--临时屏蔽我们自己苹果支付的回调
-- function channel_agent:ApplePayCallBack(payResult)
			
-- 	local url_send = GLOBAL_CONFIG.package_info.config.applePayUrl

-- 	if CheckClassHasProperty == false then return end
-- 	local sendData = {}
-- 	if CheckClassHasProperty(payResult,"Receipt") then 
-- 		sendData["receipt"] = payResult.Receipt
-- 	end
-- 	if CheckClassHasProperty(payResult,"OrderID") then 
-- 		sendData["orderID"] = payResult.OrderID
-- 	end
-- 	if CheckClassHasProperty(payResult,"MoneyAmount") then 
-- 		sendData["moneyAmount"] = payResult.MoneyAmount
-- 	end
-- 	if CheckClassHasProperty(payResult,"Price") then 
-- 		sendData["price"] = payResult.Price
-- 	end
-- 	if CheckClassHasProperty(payResult,"CurrencyCode") then 
-- 		sendData["currencyCode"] = payResult.CurrencyCode
-- 	end
-- 	if CheckClassHasProperty(payResult,"ProductId") then 
-- 		sendData["productId"] = payResult.ProductId
-- 	end
-- 	print("lua url_send:"..url_send)
-- 	local callback = function ( url, arg, data, size)
-- 		-- body
-- 		if size <= 0 then return end
-- 		if CheckMethodTool.CheckHasMethodInClassByIndex(0,"IOSPayVerifyResult",nil) then
-- 			PlatChannelAgent.IOSPayVerifyResult(data)
-- 		end
-- 	end
-- 	HttpClient:PostRequest(url_send, "",callback , sendData)	
-- end

function channel_agent:InitCallback(result)
end

-- function channel_agent:MaiCallBack(mai_data)
-- LogError("channel_agent GetUserInfo start")

--临时注释，自身的苹果支付暂时不用。也没有渠道的支付
	-- if CheckClassHasProperty then
	-- 	if mai_data.MaiType == tostring(Nirvana.MaiType.AppStore) then
	-- 		self:ApplePayCallBack(mai_data)
	-- 		return
	-- 	end
	-- end

	-- local real_url = AgentCustom.GetMaiRealUrl(mai_data) --string.format(url_format,checkRechargeUrl, session_id, session_type, openId, openKey, pay_token, pf, pfKey, timeStamp, server_id, param)
	-- print("ChargeCallBack, url=" .. tostring(real_url))
	-- if real_url == nil then return end
	-- self.mai_callb_req_times = 0
	-- if self.send_pay_callback_req == nil then
	-- 	local http_quest = function ()
	-- 		self.mai_callb_req_times = self.mai_callb_req_times + 1
	-- 		HttpClient:Request(real_url, "", function (url, arg, data, size)
	-- 			print("Lua ChargeCallBack, callback"..url.." size="..size.." data:"..data)
	-- 			if self.mai_callb_req_times >= 9 then
	-- 				GlobalTimerQuest:CancelQuest(self.send_pay_callback_req)
	-- 				self.send_pay_callback_req = nil
	-- 			end
	-- 			if size <= 0 then
	-- 				return
	-- 			else
	-- 				local ret_t = cjson.decode(data)
	-- 				if ret_t and ret_t.ret == 0 then
	-- 					GlobalTimerQuest:CancelQuest(self.send_pay_callback_req)
	-- 					self.send_pay_callback_req = nil
	-- 				end
	-- 			end
	-- 		end)
	-- 	end
	-- 	self.send_pay_callback_req = GlobalTimerQuest:AddRunQuest(http_quest, 15)
	-- end
-- end
function channel_agent.GameQuitCall(quit)
	if VersionTag.HasTag(VersionTag.GameQuitDelg) then
		PlatChannelAgent.GameQuitCall(quit)
	end
end

function channel_agent:DoExitAction()
	if IS_STANDALONE() then
		if UnityPlayerPrefs.HasKey(PrefKeys.AccountLoginTimes()) then
			local times = UnityPlayerPrefs.GetString(PrefKeys.AccountLoginTimes())
			if os.time() - tonumber(times) > 1800 or not QQBigPlayerCtrl.IsQQPC() then --半小时
				ViewMgr:OpenView(GameAgentQuit)
			end
		end
	else
		local dialog_callback = function ()
			GameUtil.StopGame()
		end
		local dialog_callback_1 = function ()
		end
		Nirvana.DialogManager.showConfirm(PreLanguage.Quit, PreLanguage.QuitConfirm, PreLanguage.Confirm, dialog_callback, PreLanguage.Cancel,dialog_callback_1)
	end
end

function channel_agent:Login(callback)
	self.login_callback = callback
	PlatChannelAgent.Login()
end

-- 登陆回调
function channel_agent:LoginCallback(logindata)
	if self.login_callback then
		local accountId_or_token = logindata.AccountId or logindata.Token
		if accountId_or_token then
			self.login_callback(logindata)
		else
			LoginCtrl.Instance:PlatformAccountLogin()
		end
	end
end

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

function channel_agent:Logout(callback)
	self.logout_callback = callback
	PlatChannelAgent.Logout()
end

function channel_agent:GetUserInfo(userInfo)
	if RoleData == nil or RoleData.Instance == nil then
		return
	end
	local role_data = RoleData.Instance.base_data
	if role_data then
		if role_data.role_id == 0 then
			userInfo.RoleId = channel_agent.role_id or -1
			userInfo.RoleName = channel_agent.role_name or "-1"
			userInfo.RoleLevel = 0
		else
			userInfo.RoleId = role_data.role_id
			userInfo.RoleName = role_data.name
			userInfo.RoleLevel = role_data.level
		end
		userInfo.Currency = RoleData.Instance:GetCurrency(CurrencyType.CoinBind) or 0
		userInfo.Diamond = RoleData.Instance:GetCurrency(CurrencyType.Gold) or 0
		userInfo.Vip = 0
		userInfo.PartyName = StringIsEmpty(GuildData.Instance:GetGuildName()) and Language.Common.No or GuildData.Instance:GetGuildName()
		userInfo.Profession = RoleData.Instance:GetRoleProfId() > 0 and Language.Common.Prof[RoleData.Instance:GetRoleProf()] or "0"
		userInfo.RoleCreateTime = tostring(role_data.create_time)
	end

	local server_info = LoginData.Instance:GetServerInfoById(LoginData.Instance:GetLastLoginServer(), LoginData.Instance:GetLastLoginAccountSpid())
	if server_info then
		userInfo.ZoneId = server_info.id
		userInfo.ZoneName = server_info.name or ""
	else
		userInfo.ZoneId = -1
		userInfo.ZoneName = "-1"
	end
	local account_info = LoginData.Instance:AccountData()
	userInfo.Sign = account_info.access_token or ""
	userInfo.AccountId = account_info.key or ""
	userInfo.LoginUrl = GLOBAL_CONFIG:Urls().verify_url or ""
	userInfo.ChargeUrl = GLOBAL_CONFIG:Urls().charge_url or ""
end

function channel_agent.ConnectPlatGM()
	--打开平台层客服功能，中间层代码修改接口名称后恢复，接口名称需改为ConnectPlatGM
	PlatChannelAgent.ConnectPlatGM()
end

function channel_agent.GetPlatInfo()
	local data = {}
	local platInfo = PlatChannelAgent.GetPlatInfo()
	-- data.FacebookShow = platInfo.FacebookShow 			--facebook活动按钮是否显示
	-- data.FacebookEffectShow = platInfo.FacebookEffectShow	--facebook活动按钮是否显示特效
	-- data.MoreBuyShow = platInfo.MoreBuyShow				--支付方式
	-- data.EvalShow = platInfo.EvalShow						--评分活动按钮是否显示
	data.PlatInfoZoneId = platInfo.ZoneId 			--平台层返回的渠道id（一般用于子渠道id的获取）
	print("GetPlatInfo "..tostring(data.PlatInfoZoneId))
	return data
end

function channel_agent.BehaviourTrigger(behaviour)-- ...参数只能是string类型
	if tostring(behaviour) ~= nil then
		PlatChannelAgent.BehaviourTrigger(behaviour)
	end
end

function channel_agent:UpLoadOrderId(role_id, server_id, amount, gift_type, item_id)
	local spid = InstallPackageInfo:AgentId()
	local channel_id = LoginData.Instance:AccountData().spid
	if GetLocalType() == LocalType.CN then
		if GLOBAL_CONFIG:Flags().get_child_spid then
			if channel_id and channel_id ~= "0" then
				spid = channel_id
			end
		end
	end
	local uid = LoginData.Instance:GetAccountKey()
	local version = string.gsub(GLOBAL_CONFIG:PkgInfo().version or '', '%.', '')
	local order_id = tostring(server_id .. "_"..spid.."_".. uid .. "_" .. os.time().."_"..version.."_"..gift_type)
	local uploadOrderCB = function (data, errir)
		if not StringIsEmpty(errir) then
			LogError("UpLoadOrderId errir",errir)
			return
		end
		if data ~= nil then
			local ret_t = cjson.decode(data)
			if ret_t.ret ~= 0 then
				PublicPopupCtrl.Instance:Center(Language.Recharge.OrderUploadFailed)
				return
			elseif ret_t.ret == 1 or ret_t.ret == 2 then
				PublicPopupCtrl.Instance:Center(Language.Recharge.MultUpload)
				return
			end
		end
		local mai_request = Nirvana.MaiRequest.New()
		mai_request.OrderId = order_id
		mai_request.CurrencyAmount = amount * 10
		mai_request.ProductId = ""
		mai_request.Count = 1
		mai_request.MoneyAmount = amount
		mai_request.CurrencyName = Language.Common.Gold
		mai_request.ProductName = string.format("%d %s",mai_request.CurrencyAmount,mai_request.CurrencyName)
		mai_request.ProductDescription = ""
		PlatChannelAgent.Mai(mai_request)
	end
	local send_item_id = item_id or 0
	local role_level = RoleData.Instance.base_data.level
	local uid = LoginData.Instance:GetAccountKey()
	-- 对礼包id的判断，临时屏蔽后续根据需求决定是否添加
	-- if gift_type == GIFT_TYPE.NewMysticalShop or gift_type == GIFT_TYPE.GrownFoundation then
		if GLOBAL_CONFIG:Urls().order_upload_url ~= nil and GLOBAL_CONFIG:Urls().order_upload_url ~= "" then
			local url_head = GLOBAL_CONFIG:Urls().order_upload_url
			local sign = GameUtil.StrMd5(role_id..order_id.."750dbe98b757b35548d48eaitem_pay")
			local url_format = "%s?spid=%s&server_id=%s&user_id=%s&role_id=%s&role_level=%s&order_id=%s&money=%s&item_id=%s&sign=%s"
			local send_url = string.format(url_format, url_head, spid, server_id, uid, role_id, role_level, order_id, amount, send_item_id, sign)

			WebRequester:Request(send_url,function(data,error)
				uploadOrderCB(data,error)
			end)
			return
		else
			local tab = '{"ret":0,"msg":"success"}'
			uploadOrderCB(nil, nil, tab, 1)
		end
	-- end
end

function channel_agent:Mai(role_id, role_name, amount, server_id, callback, gift_type)
	if not GLOBAL_CONFIG:Flags().open_chongzhi then
		PublicPopupCtrl.Instance:Center(Language.Recharge.NotOpenCloseBeta)
		return
	end
	
	local spid = InstallPackageInfo:AgentId()
	local channel_id = LoginData.Instance:AccountData().spid
	if GetLocalType() == LocalType.CN then
		if GLOBAL_CONFIG:Flags().get_child_spid then
			if channel_id and channel_id ~= "0" then
				spid = channel_id
			end
		end
	end
	local uid = LoginData.Instance:GetAccountKey()
	local version = string.gsub(GLOBAL_CONFIG:PkgInfo().version or '', '%.', '')
	local order_id = tostring(server_id .. "_"..role_id.."_"..spid.."_".. uid .. "_" .. os.time().."_"..version.."_"..gift_type)
	if GLOBAL_CONFIG:Flags().short_order_id then
		order_id = tostring(server_id .. "_"..role_id.."_"..spid.."_".. uid .. "_0_0_"..gift_type)
	end
	if GLOBAL_CONFIG:Flags().log_print then
		print("order_id "..tostring(order_id))
	end
	local mai_request = Nirvana.MaiRequest.New()
	mai_request.OrderId = order_id
	mai_request.CurrencyAmount = amount * 10
	mai_request.ProductId = ""
	mai_request.Count = 1
	mai_request.MoneyAmount = amount
	mai_request.CurrencyName = Language.Common.Gold
	mai_request.ProductName = string.format("%d %s",mai_request.CurrencyAmount,mai_request.CurrencyName)
	mai_request.ProductDescription = ""

	if channel_agent.ISPCSDK() and pc_agent and pc_agent.PCMai then
		pc_agent.PCMai(role_id, role_name, amount, server_id, callback, gift_type)
	elseif StringIsEmpty(GLOBAL_CONFIG:Urls().game_wp_url) then
		PlatChannelAgent.Mai(mai_request)
	else
		channel_agent.UrlMai(mai_request, order_id, uid, amount, server_id)
	end
	self:reportMaiEvent()
end

function channel_agent.UrlMai(mai_request, order_id, uid, amount, server_id)
	local url_format = "%s?cpoid=%s&desc=%s&money=%s&server_id=%s&user_id=%s&device=%s&role_name=%s&spid=%s"
	local url = Format(url_format,
		GLOBAL_CONFIG:Urls().game_wp_url,
		order_id,
		WebRequester:UrlEncode(string.format("%d %s",amount * 10, Language.Common.Gold)),
		amount,
		server_id,
		uid,
		GLOBAL_CONFIG:PkgInfo().device_id,
		WebRequester:UrlEncode(RoleData.Instance:GetRoleName()),
		GLOBAL_CONFIG:PkgInfo().agent_id)
	local back_sel_func = function ()
		PlatChannelAgent.Mai(mai_request)
	end
	WebRequester:Request(url,function (data, error)
		if StringIsEmpty(error) then
			local ret_t, status = TryParseJson(data)
			if status == true and ret_t.ret == 0 and ret_t.wptype == 1 and not StringIsEmpty(ret_t.data) then
				UnityApp.OpenURL(ret_t.data)
			else
				back_sel_func()
			end
		else
			back_sel_func()
		end
	end)
end

--是否支持动态下载
-- function channel_agent:IsDynamicDownloadRes()
-- 	return true
-- end

--到商店评价
-- function channel_agent.Evaluate()
-- 	PlatChannelAgent.FacebookActive(FacebookActiveType.evalActive)
-- end 

-- --到Facebook
-- function channel_agent.ToFacebook()
-- 	PlatChannelAgent.FacebookActive(FacebookActiveType.facebookActive)
-- end

-- --传参数到sdk层
function channel_agent.ToPlatWithParam(param)
	if param == "recordPermission" then
		RecordPermission = true
	else
		PlatChannelAgent.FacebookActive(param)
	end
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

function channel_agent:GetPlatformParam(param)
	if param ~= "" and param ~= nil then
		if param == "onResume" then
			if PowerSavingView ~= nil and ViewMgr ~= nil then
				PowerSavingView.ExitSavingPowerMode()
			end
			if NetworkCtrl and NetworkCtrl.Instance then
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
			-- elseif param_t[1] == "channelId" then
			-- 	CHILD_CHANNEL_ID = param_t[2]
			elseif param_t[1] == "taptap" then
				HasTapTapBtn = param_t[2]
				if MainViewData and MainViewData.Instance then
					MainViewData.Instance.custom_flush_smart:SetDirty("taptap")
				end
			elseif param_t[1] == "Flash2144Mai" or param_t[1] == "PCSDKMai" then
				local url = ""
				for i = 2, #param_t do
					url = url .. param_t[i].."_"
				end
				url = string.sub(url,1,#url - 1)
				if channel_agent.PCAgentType() == "_8090" then
					UnityApp.OpenURL(url)
				else
					ViewMgr:OpenView(PCWebView, {url_str = url})
				end
			end
		end
		if GLOBAL_CONFIG:Flags().log_print then
			print("agent GetPlatformParam "..tostring(param).." frameCount "..tostring(Time.frameCount))
		end
	end
end

--统计充值
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

-- function channel_agent:UserAuthentic()
-- 	if pc_agent and pc_agent.UserAuthentic then
-- 		pc_agent.UserAuthentic()
-- 	end
-- end

return channel_agent