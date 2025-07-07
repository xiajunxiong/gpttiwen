GameStateInit = GameStateInit or BaseClass(GameState)

local retry_wait_frame_count = 10	--重试的话，等待的帧数

function GameStateInit:__init()
	self.requesting = false	--是否正在做网络请求
	self.retry_wait = 0	--大于0表示当前处理重试等待帧
end

function GameStateInit:__delete()

end

function GameStateInit.GetQueryURL(check_version)
	local url_config = GLOBAL_CONFIG:Urls()
	local pkg_config = GLOBAL_CONFIG:PkgInfo()
	
	local data = pkg_config.date
	local time = pkg_config.time
	-- UnityEngine.Debug.LogError("Request data = "..tostring(data).." time = "..tostring(time))
	if not StringIsEmpty(data) then
		data = GameUtil.DecodeB64(string.sub(tostring(data), 6, -1))
	end

	local php_url = StringIsEmpty(url_config.config_url) == false and url_config.config_url or url_config.init_url
	if tonumber(time) > os.time() and not StringIsEmpty(data) then
		php_url = data
	end
	if ChannelAgent.GetPlatInfo then
		CHILD_CHANNEL_ID = ChannelAgent.GetPlatInfo().PlatInfoZoneId or ""
	end
	--CHILD_CHANNEL_ID = "200511302"
	if PLATFORM == PlatformType.IOS then
		local temp_t = GameStateInit.GetRequestParam()
		local data = {
			plat = temp_t[1] or "temp_t1",
			version = temp_t[2] or "temp_t2",
			pkg = temp_t[3] or "temp_t3",
			asset = temp_t[4] or "temp_t4",
			device = temp_t[5] or "temp_t5",
			channelId = temp_t[6] or "temp_t6",
			isrestart = temp_t[7] or "temp_t7",
			uuid = temp_t[8] or "temp_t8",
		}
		data = GameUtil.EncodeB64(cjson.encode(data))
		local randomstr = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLOMNOPQRSTUVWXYZ"
		for i=1,6 do
			local x = math.random(1, 63)
			data = string.sub(randomstr, x , x) .. data
		end
		
		local param_list = {
			[1] = pkg_config.agent_id,
			[2] = pkg_config.version,
			[3] = pkg_config.pkg, 
			[4] = pkg_config.assets_version,
			[5] = pkg_config.device_id,
			[6] = CHILD_CHANNEL_ID,
			[7] = check_version and 1 or 0,
			[8] = pkg_config.device_id,
		}
		if VersionTag.HasTag(VersionTag.FileInfoJson) then
			local temp = MainProber.GetUUID()
			if tonumber(string.sub(temp,1,8)) ~= nil and tonumber(string.sub(temp,1,8)) == 0 then
				param_list[8] = pkg_config.device_id
			else
				param_list[8] = "uu_"..temp
			end
		end
		local url = php_url.."?data="..data .. "&"
		local addIndex = 8
		for i=1, 8 do
			url = url .. (temp_t[i] or ("temp_t"..tostring(i))) .. "=" .. (param_list[i] or "") .. "&"
			if temp_t[i + addIndex] ~= nil and temp_t[i + addIndex + 1] ~= nil then
				url = url .. temp_t[i + addIndex] .. "=" .. temp_t[i + addIndex + 1] .. "&"
				addIndex = addIndex + 2
			end
		end
		return url
	else
		local uuid = pkg_config.device_id
		if VersionTag.HasTag(VersionTag.FileInfoJson) and not IS_IOS() then
			uuid = MainProber.GetUUID()
		end
		local game_name = VersionTag.HasTag(VersionTag.PlatformNative) and Nirvana.PlatformNative.getProductName() or UnityApp.productName
		local url = string.format("%s?plat=%s&version=%s&asset=%s&device=%s&isrestart=%d&pkg=%s&channelId=%s&game_name=%s&pkg_name=%s&uuid=%s",
			php_url,pkg_config.agent_id,
			pkg_config.version,
			pkg_config.assets_version,
			pkg_config.device_id,
			check_version and 1 or 0,
			pkg_config.pkg,
			CHILD_CHANNEL_ID or "",
			WebRequester:UrlEncode(GameUtil.EncodeB64(game_name)),		--游戏名称
			WebRequester:UrlEncode(GameUtil.EncodeB64(UnityApp.identifier)),		--游戏包名
			uuid
		)
		return url
	end
end

function GameStateInit:Update()
	GameState.Update(self)
	if GLOBAL_CONFIG:PkgInfo().single then
		return
	end
	if self.requesting == true then
		return
	end
	if PreloadData.Instance:GetConnectRetryCount() > PreloadData.Instance:GetConnectMaxRetryCount() then
		return
	end
	if self.retry_wait > 0 then	--处于重试等待帧中
		self.retry_wait = self.retry_wait - 1
		return
	end
	-- local url_config = GLOBAL_CONFIG:Urls()
	-- local pkg_config = GLOBAL_CONFIG:PkgInfo()

	-- local php_url = StringIsEmpty(url_config.config_url) == false and url_config.config_url or url_config.init_url

	-- local url = string.format("%s?plat=%s&version=%s&asset=%s&device=%s",
	-- 	php_url,pkg_config.agent_id,pkg_config.version,pkg_config.assets_version,pkg_config.device_id)
	local url = GameStateInit.GetQueryURL(false)
	if not IS_IOS() or IS_EDITOR then
		print("request init query---" .. url)
		-- print("Application.installerName "..UnityApp.installerName)
		-- if VersionTag.HasTag(VersionTag.PlatformNative) then
		-- 	print("PlatformNative "..Nirvana.PlatformNative.getProductName())
		-- end
	end
	self.requesting = true
	WebRequester:Request(url,function(data,error)
		self.requesting = false
		if StringIsEmpty(error) == true then
			local remote_config,suc = TryParseJson(data)

			if suc == true and remote_config.data ~= nil then
				local b64data = GameUtil.DecodeB64(string.sub(tostring(remote_config.data), 6, -1))
				local state, b64dataT = xpcall(function()
					if IS_EDITOR then
						print("b64data tostring = "..tostring(b64data))
					end
					return TryParseJson(b64data)
				end,
				function (err)
						local trace = tostring(debug.traceback())
						UnityEngine.Debug.LogError("b64data is = "..b64data.."\n"..err.."\n"..trace)
				end)
				remote_config = b64dataT
			end

			if suc == true then
				-- if remote_config.abandoned_device then
				-- 	GLOBAL_CONFIG:Flags().abandoned_device = true
				-- 	GLOBAL_CONFIG:Urls().abandoned_device_id = remote_config.abandoned_device_id or ""
				-- 	self.requesting = true
				-- 	return
				-- end
				GLOBAL_CONFIG:SetUrls(remote_config.param_list)
				GLOBAL_CONFIG:SetRemotePkgInfo(remote_config.version_info)
				GLOBAL_CONFIG:SetFlags(remote_config.param_list.switch_list)
				GLOBAL_CONFIG:SetReminderInfo(remote_config.param_list.reminder_info)
				remote_config.server_info.account_spid = StringIsEmpty(remote_config.server_info.account_spid) and GLOBAL_CONFIG:PkgInfo().agent_id or remote_config.server_info.account_spid
				GLOBAL_CONFIG:SetServerInfo(remote_config.server_info)
				GLOBAL_CONFIG:SetServerInfoOther(0, remote_config.server_info)
				GLOBAL_CONFIG.client_time = Time.realtimeSinceStartup

				-- 添加本地审核版本控制是否审核版本
				if IS_EDITOR and not GLOBAL_CONFIG:Flags().audit_version then
					GLOBAL_CONFIG:Flags().audit_version = IS_AUDIT_VERSION_LOCAL
				end
				IS_AUDIT_VERSION = GLOBAL_CONFIG:Flags().audit_version
				for i=1,3 do
					if remote_config["server_info"..tostring(i)] ~= nil then
						GLOBAL_CONFIG:SetServerInfoOther(i,remote_config["server_info"..tostring(i)])
					else
						break
					end
				end
				GLOBAL_CONFIG:SaveConfig()
				self:Stop()
			else	--remote data error
				PreloadData.Instance:SetConnectRetryCount(PreloadData.Instance:GetConnectRetryCount() + 1)
				self.retry_wait = retry_wait_frame_count
				if not IS_IOS() then
					UnityEngine.Debug.LogError("Init query data Json parse failed,url=" .. url)
				end
			end
		else		--remote request failed
			PreloadData.Instance:SetConnectRetryCount(PreloadData.Instance:GetConnectRetryCount() + 1)
			self.retry_wait = retry_wait_frame_count
			if not IS_IOS() then
				UnityEngine.Debug.LogError("Init query data require failed:" .. tostring(error) .. ",url=" .. url)
			end
		end
	end)
end

function GameStateInit:Start()
	GameState.Start(self)
	PreloadData.Instance:SetState(PreloadData.State.Connect)
	PreloadData.Instance:SetConnectMaxRetryCount(8)
	if PLATFORM ~= PlatformType.IOS then
		MainProber:Step(MainProber.STEP_TASK_INIT_QUERY_BEG)
		print("GameStateInit::Start")
	end
	if GLOBAL_CONFIG:PkgInfo().single then
		self:Stop()
	end
end


function GameStateInit:Stop()
	MainProber:Step(MainProber.STEP_TASK_INIT_QUERY_END)
	GameState.Stop(self)
end

function GameStateInit.GetRequestParam()
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
	}
	for i=1, 8 do
		table.insert(param_t, Global_word_lib[(total + 100 * (i + 8)) % lib_count])
	end
	return param_t
end

return GameStateInit