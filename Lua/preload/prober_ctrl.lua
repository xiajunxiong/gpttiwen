MainProber = { 
	error_view = nil,
	error_log = nil,
	error_alert_view = nil,

	screen_on = true,
	countly_on = true,
	log_level = 0,
	login_times = 0,
	agent_id = "",
	device_id = "",
	pkg_ver = "",
	assets_ver = "",
	session_id = 0,
	net_state = 0,
	user_id = "",
	server_id = 0,
	role_id = 0,
	role_name = "",
	role_level = 0,
	last_ping = 0,
	ping_itvl = 900,
	device_uuid = 0,
	channel_id = 0,
	step_count = {},
	role_authority = 0,
	LOG_DELIMITER = "\t",

	STEP_MAIN_LOADER_BEG 			= 100,		--游戏启动
	STEP_TASK_INIT_QUERY_BEG 		= 160,		--query申请开始
	STEP_TASK_INIT_QUERY_END 		= 180,		--query
	STEP_LOAD_CONFIG_BEG			= 200,		--加载配置开始（require）
	STEP_LOAD_CONFIG_END			= 300,		--加载配置结束
	STEP_UNKNOW_GRAHPIC_TYPE		= 305,		--发现了未预置的显卡名时上报后台
	STEP_CONNECT_GAME_SERVER		= 310,		--连接游戏服完成
	STEP_ASSET_UPDATE_BEG 			= 400,		--更新资源开始
	SETP_ASSET_UPDATE_PREPARE		= 410,		--下载完Manifest和FileInfo开始匹配检查本地资源
	SETP_ASSET_BEGIN_DL				= 420,		--匹配检查本地资源结束，正式开始下载资源
	STEP_ASSET_UPDATE_END 			= 500,		--更新资源结束
	STEP_BEGINCG_END				= 510,		--游戏启动 cg播放结束后(包括跳过)
	STEP_PHP_VERIFY_BEG				= 600,		--登陆验证开始
	STEP_PHP_VERIFY_END 			= 700,		--登陆验证结束
	STEP_LOAD_SERVER_BEG 			= 800,		--登陆服务器开始
	STEP_LOAD_SERVER_END			= 900,		--登陆服务器结束
	STEP_ROLECREATECG_END			= 910,		--创角前CG播放结束后(包括跳过)
	STEP_ROLECREATE_SCENELOAD_END   = 920,		--创角场景加载完毕
	STEP_ROLECREATE_VIEWLOAD_END	= 930,		--打开创建角色界面（UI）
	STEP_ROLECREATE_CLICK_RETURN	= 935,		--创角形象界面返回按钮
	STEP_ROLECREATE_CLICK_AVATAR 	= 940,		--创建界面点击形象
	STEP_ROLECREATE_CONFIRM_SHOW    = 945,		--确定形象面板显示
	STEP_ROLECREATE_CONFIRM_AVATAR  = 950,		--创建界面确定选择形象
	STEP_ROLECREATE_CLICK_PAUSE		= 955,		--创角职业选择/形象界面返回
	STEP_ROLECREATE_CLICK_PROF		= 960,		--点击选择职业
	STEP_CREATE_ROLE_BEG 			= 1000,		--创建角色开始
	STEP_CREATE_ROLE_END 			= 1100,		--创建角色完成
	STEP_REQUEST_LOGIN_ROLE			= 1200,		--开始角色登陆
	STEP_LOAD_GAME_SERVER_BEG		= 1300,		--角色登录完成并连接游戏服务器
	STEP_LOAD_GAME_SERVER_END		= 1350,		--连接游戏服务器完成
	STEP_ENTER_SCENE_END			= 1400,		--进服场景完成
	STEP_ENTER_REAL_SCENE_END		= 1500,		--服务器拉角色进入真正的场景
	STEP_ROLE_ENTER_GAME			= 1600,		--角色登陆
	STEP_GAME_START					= 2000,		--游戏开始

	STEP_XINMO_ANIM_START			= 2010,		--心魔剧情动画开始
	STEP_XINMO_ANIM_END				= 2011,		--心魔剧情动画结束
	STEP_XINMO_BATTLE_START			= 2012,		--心魔战斗开始
	STEP_XINMO_BATTLE_End			= 2013,		--心魔战斗结束

	STEP_HEARTBEAT					= 2100,		--心跳
	STEP_SESSION_END 				= 10000,
	STEP_CHAT 						= 20100,    --聊天

	EVENT_ON_CREATE 				= 10001,
	EVENT_ON_START 					= 10002,
	EVENT_ON_PAUSE 					= 10003,
	EVENT_ON_RESUME 				= 10004,
	EVENT_ON_STOP 					= 10005,
	EVENT_ON_DESTROY 				= 10006,
	EVENT_KEY_DOWN 					= 10007,
	EVENT_NET_STATE_CHANGE 			= 10008,
	EVENT_MEMORY_WARN 				= 10009,
	EVENT_ERROR 					= 10010,
	EVENT_CRASH_NATIVE 				= 10011,
	EVENT_CRASH_JAVA 				= 10012,
	EVENT_UPDATE_RETRY 				= 10013,
	EVENT_UPDATE_ERROR 				= 10014,
	EVENT_UPDATE_FATAL 				= 10015,
	EVENT_CONFIG_RETRY 				= 10016,
	EVENT_ASSET_RETRY 				= 10017,

	COUNT_TEST = 20001,
}

local NetStateEnum = {
        NotReachable = 0,
        ReachableViaCarrierDataNetwork = 1,
        ReachableViaLocalAreaNetwork = 2
}
function MainProber:Start()
	local package_info = GLOBAL_CONFIG:PkgInfo()
	self.agent_id = package_info.agent_id
	self.device_id = package_info.device_id --PlatformAdapter.GetPhoneUniqueId()
	self.device_uuid = StringIsEmpty(MainProber.GetUUID()) and 0 or MainProber.GetUUID()
	self.pkg_ver = package_info.version --TempCode GLOBAL_CONFIG.package_info.version
	self.assets_ver = package_info.assets_version
	self.net_state = NetStateEnum[tostring(UnityEngine.Application.internetReachability)]
	-- print("MainProber "..tostring(self.device_uuid))
	if ChannelAgent.GetPlatInfo then
		self.channel_id = ChannelAgent.GetPlatInfo().PlatInfoZoneId or 0
	end
	--print("self.pkg_ver is "..tostring(self.pkg_ver).." assets_ver is "..tostring(self.assets_ver))
	self.session_id = tostring(math.random(1000, 10000000))
end

function MainProber:Update()
	if Time.realtimeSinceStartup - self.last_ping > self.ping_itvl then
		self.last_ping = Time.realtimeSinceStartup
		if nil ~= self.step_count[2000] then
			MainProber:Step(20000, MainProber.user_id, MainProber.server_id, MainProber.role_name, MainProber.role_id, MainProber.role_level, PLATFORM)
		end
	end
end

function MainProber:SetUserId(user_id, server_id, login_times)
	self.user_id = user_id
	self.server_id = server_id
	self.login_times = login_times or 0
end

function MainProber:SetRoleInfo(role_id, role_name)
	self.role_id = role_id
	self.role_name = GameUtil.EncodeB64(role_name)
end

function MainProber:SetRoleName(role_name)
	self.role_name = GameUtil.EncodeB64(role_name)
end

function MainProber:SetRolelevel(role_level)
	self.role_level = role_level
end

function MainProber:Stop()
	self.screen_on = false
	self.countly_on = true

	self.step_count = {}
end

function MainProber:Step(st, ...)
	-- UnityEngine.Debug.LogError("Step "..tostring(st).." "..tostring(GLOBAL_CONFIG:Flags().countly_report))
	if self.countly_on and nil ~= st and GLOBAL_CONFIG:Flags().countly_report and not GLOBAL_CONFIG:PkgInfo().single then
		self.step_count[st] = 1

		local url = GLOBAL_CONFIG:Urls().report_url2
		if nil == url or "" == url then
			url = GLOBAL_CONFIG:Urls().report_url
		--else
		--	print("url is "..url)
		end
		if nil == url then
		--	print("url")
			return
		end

		local log = ""
		for i=1, select("#", ...) do
			log = log .. self.LOG_DELIMITER .. tostring(select(i, ...))
		end
		local now = os.time()
								-- 1  2    3   4   5   6   7   8   9   10  11 12			1 					2 					3
		local arg = string.format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s%s", tostring(st), tostring(self.agent_id), tostring(self.device_id), 
			--			4 							5 			   				6 				  			7 			  				8 		 		9
			tostring(self.pkg_ver), tostring(self.assets_ver), tostring(self.session_id), tostring(self.login_times), tostring(self.net_state), tostring(now), 
			-- 				10 						11 			  12
			tostring(self.device_uuid),tostring(self.channel_id), log)
		
		local sign = GameUtil.StrMd5(tostring(st) .. tostring(self.agent_id) .. tostring(self.session_id) .. tostring(now) .. "a0167e746e90736a1dfa16a0f65a6fa7")
		local b64_str = GameUtil.EncodeB64(arg)
		local send_url = string.format("%s?data=%s&sign=%s", tostring(url), b64_str, tostring(sign))
		WebRequester:Request(send_url,function(data,error)
			if error ~= nil and error ~= "" then
				print(error)
			end
		end)
		-- HttpClient:Request(string.format("%s?data=%s&sign=%s", tostring(url), b64_str, tostring(sign)), "")
		if DEBUG_MODE or GLOBAL_CONFIG:Flags().log_print then
			print("MainProber:Step -> st is "..st ..--[[" url_str is "..url_str ]]" url is->" .. send_url)
		end
	end
end

--用于记录上次登录的角色区服等信息
--在用户更换设备时能准确获取账号上一次登录的区服
function MainProber:SendRoleReport()
	-- LogError("SendRoleReport",LoginData.Instance:GetAccountSpid(), self.server_id, LoginData.Instance:GetAccountKey(), self.role_id, self.role_name)
	local url_head = GLOBAL_CONFIG:Urls().role_report_url
	if url_head == nil then
		-- LogError("return 1")
		return
	end
	local send_url = string.format("%s?spid=%s&server_id=%s&user_id=%s&role_id=%s&role_name=%s&",
		url_head,
		LoginData.Instance:GetAccountSpid(),
		self.server_id,
		LoginData.Instance:GetAccountKey(),
		self.role_id,
		self.role_name)
	WebRequester:Request(send_url, function(data,error)
		if error ~= nil and error ~= "" then
			print(error)
		-- else
		-- 	local ret_t, succ = TryParseJson(data)
		-- 	if succ then
		-- 		LogError("data",data)
		-- 	end
		end
	end)

end


local uniqueId
function MainProber.GetUUID()
	if uniqueId == nil then
		uniqueId = Nirvana.DeviceTool.GetUUID()
		if string.find(uniqueId,"OAID") then
			uniqueId = string.sub(uniqueId,6,-1)
		end
	end
	return uniqueId
end