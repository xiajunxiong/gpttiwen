local channel_agent = {
	login_callback = nil,
	name = "",
	role_id = 0,
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

function channel_agent:Login(callback)
	self.login_callback = callback
	if channel_agent.ISPCSDK() then
		self:PCSDKLogin()
	else
		ViewMgr:OpenView(LoginView)
	end
end

function channel_agent:PCSDKLogin()
	local pc_login_data = {}
	pc_login_data.AccountId = Nirvana.GameAgent.GetUserId()
	pc_login_data.Token = Nirvana.GameAgent.GetToken()
	local extra_info = Nirvana.GameAgent.GetExtraInfo()
	pc_login_data.DynamicParams = {[0] = extra_info, Length = 1}
	channel_agent.PCFileLog("accountId",pc_login_data.AccountId,"Token",pc_login_data.Token,"ExtraInfo",pc_login_data.DynamicParams[0])
	LoginData.Instance:SetPCSdkExtraInfo(extra_info)
	-- if channel_agent.PCAgentType() == "Flash2144" then
	-- 	pc_login_data.TimeStamp = Nirvana.GameAgent.GetLoginTime()
	-- end
	self:LoginCallback(pc_login_data)
end

function channel_agent:Logout(callback)
	if ViewMgr:IsOpen(WelcomeView) then
		LoginCtrl.Instance:Data():SetAccountKey("")
		LoginCtrl.Instance.login_verify_state = LoginVerifyState.NOLOGIN
		ViewMgr:OpenView(LoginView)
	else
		callback()
	end
end

function channel_agent:LoginCallback(login_data)
	ViewMgr:CloseView(LoginView)

	if self.login_callback ~= nil then
		local cb = self.login_callback
		self.login_callback = nil
		cb(login_data)
	end
end

function channel_agent:Mai(role_id, role_name, money, server_id, callback, gift_type)
    if LoginData.Instance:BaseData().is_on_crosssever then --跨服不允许买东西
          PublicPopupCtrl.Instance:AlertTip("跨服内仅支持代币券商品，万灵币礼包请返回原服进行操作",function()end)
     return false
   else	  
    local url = GLOBAL_CONFIG:Urls().pay_event_url
	local account = LoginData.Instance:GetAccountKey()
    url = url .. "?type=%s&spid=%s&role_id=%s&server_id=%s&money=%s&gift=%s&time=%s&sign=%s"	
    local sign = GameUtil.StrMd5( "bnb" .. account .. MainProber.role_id .. os.time() .. "wanwuguiyuan" )	
    url = string.format(url, "bnb", LoginData.Instance:AccountData().spid, MainProber.role_id, MainProber.server_id, money,  gift_type, os.time(), sign)
	
	local cxurl = GLOBAL_CONFIG:Urls().pay_cx_url .. "?account="..account.."&server_id="..MainProber.server_id.."&role_id="..MainProber.role_id.."&spid="..LoginData.Instance:AccountData().spid
		print("dev 日志11111111 "..cxurl)
	
	
	WebRequester:Request( cxurl,function(data, error)
	   local ret,status=TryParseJson(data)
	     if ret.ret==0 then
	   
	     PublicPopupCtrl.Instance:AlertTip(ret.data,function()
	      WebRequester:Request(
            url,
            function(data, error)
			    local ret_t,status=TryParseJson(data)
                if ret_t.ret==0 then
				    ChatCtrl.Instance:ChannelChatCustom{content=ret_t.data}
                    PublicPopupCtrl.CenterRich(ret_t.data)
                end
				if ret_t.ret==1 then
				   ChatCtrl.Instance:ChannelChatCustom{content=ret_t.data}
                   PublicPopupCtrl.CenterRich(ret_t.data)
                end
				if ret_t.ret==2 then
				   ChatCtrl.Instance:ChannelChatCustom{content=ret_t.data}
                   PublicPopupCtrl.CenterRich(ret_t.data)
                end
				if ret_t.ret==3 then
				     PublicPopupCtrl.Instance:AlertTip(ret_t.msg,function()
                      OpenURL(ret_t.data);
					end)
                end
           end)
	    end)
		
		end
		
		if ret.ret==2 then
		         PublicPopupCtrl.Instance:AlertTip(ret.msg,function()
                      OpenURL(ret.data);
				 end)
		
		end
		
	 end)  
	 end
end

function OpenURL(url)
    local openURL = UnityEngine.Application.OpenURL(url)
    return openURL
end

function channel_agent:LogoutCallback()
end

--请求购买道具
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
function channel_agent:Init()
	PlatChannelAgent.SetDoExitAction(function ()
		self:DoExitAction()
	end)
	PlatChannelAgent.SetUserInfoRefreshAction(function (userInfo)
		self:GetUserInfo(userInfo)
	end)
	PlatChannelAgent.SetOnMessageCallBack(function (param)
		self:GetPlatformParam(param)
	end)
	PlatChannelAgent.Init()
	if channel_agent.ISPCSDK() then
		if channel_agent.PCAgentType() == "_360PC" then
			pc_agent = require("preload/pc_agent/three_six_zero_agent")
		elseif channel_agent.PCAgentType() == "QQ" then
			pc_agent = require("preload/pc_agent/qq_agent")
		end
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
			channel_agent:BehaviourTrigger(BehaviourType.RoleLogout)
			GameUtil.StopGame()
		end
		local dialog_callback_1 = function ()
		end
		Nirvana.DialogManager.showConfirm(PreLanguage.Quit, PreLanguage.QuitConfirm, PreLanguage.Confirm, dialog_callback, PreLanguage.Cancel,dialog_callback_1)
	end
end

function channel_agent.GameQuitCall(quit)
	if VersionTag.HasTag(VersionTag.GameQuitDelg) then
		PlatChannelAgent.GameQuitCall(quit)
	end
end


function channel_agent.ConnectPlatGM()
	print("点击了平台客服按钮")
	--打开平台层客服功能，中间层代码修改接口名称后恢复，接口名称需改为ConnectPlatGM
	-- PlatChannelAgent.ConnectPlatGM()
end

-- function channel_agent.GetPlatInfo()
-- 	local data = {}
-- 	local platInfo = PlatChannelAgent.GetPlatInfo()
-- 	data.FacebookShow = platInfo.FacebookShow 			--facebook活动按钮是否显示
-- 	data.FacebookEffectShow = platInfo.FacebookEffectShow	--facebook活动按钮是否显示特效
-- 	data.MoreBuyShow = platInfo.MoreBuyShow				--支付方式
-- 	data.EvalShow = platInfo.EvalShow						--评分活动按钮是否显示
-- 	data.PlatInfoZoneId = platInfo.ZoneId 			--平台层返回的渠道id（一般用于子渠道id的获取）
-- 	print("GetPlatInfo "..tostring(data.PlatInfoZoneId))
-- 	return data
-- end

function channel_agent.BehaviourTrigger(behaviour)-- ...参数只能是string类型
	if pc_agent and pc_agent.PCPlatBehaviourTrigger then
		pc_agent.PCPlatBehaviourTrigger(behaviour)
	end
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
	if param == "recordPermission" then
		RecordPermission = true
	else
		PlatChannelAgent.FacebookActive(param)
	end
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

function channel_agent.PCFileLog(...)
	if VersionTag.HasTag(VersionTag.GameAgentPC) then
		local log_str = ""
		for k, v in pairs({...}) do
			if type(v) == "table" then
				if debuger then
					log_str = log_str .. Debuger.TableToString(v)
				else
					log_str = log_str .. "k:"..k.." value is table;"
				end
			else
				log_str = log_str .. tostring(v)
			end
			log_str = log_str .. "   "
		end
		-- print("PCFileLog:"..log_str)
		Nirvana.GameAgent.FileLog(debug.traceback(log_str))
	end
end

function channel_agent.SetPCLogEnable(enable)-- true/false
	if VersionTag.HasTag(VersionTag.GameAgentPC) then
		Nirvana.GameAgent.SetFileLogEnable(enable)
	end
end

function channel_agent:OpenPCRealNameSignView()
	if pc_agent.OpenPCRealNameSignView then
		pc_agent:OpenPCRealNameSignView()
	end
end

return channel_agent