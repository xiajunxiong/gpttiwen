LoginData = LoginData or BaseClass()

LoginStateType = {
	None = 0,					--未登录
	--------登录服流程---------
	LoginServerConnecting = 1,	--连接登录服
	AccountLogining = 2,		--账号登录
	RoleCreating = 3,			--角色创建
	RoleCreateRequesting = 4,	--角色创建请求
	RoleLogining = 5,			--角色登录

	------逻辑服流程--------
	LogicServerConnecting = 6,	--连接逻辑服
	SceneLogining = 7,			--场景登录
	Logined = 8,				--已登录

	AccountConflict = 9,		--被顶号
	------跨服流程--------
	CrossServerConnecting = 10,	--连接跨服

	RoleSelecting = 11,         -- 正在选择角色
}

PLAT_ACCOUNT_TYPE_TEST = 1 		--超级玩家识别号

LoginVerifyState = {
	NOLOGIN = "no_login",
	WAITFORVERIFY = "wait_verify_result",
	LOGINCOMPILETE = "login_comp",
}
LoginData.MaxLoginTryCount = 3	--最大重连失败次数，重连超过些次数则弹框返回登录界面
LoginData.LoginTrySpace = 1	--间隔多久尝试重连一次 

function LoginData:__init()
	if LoginData.Instance ~= nil then
		Debuger.LogError("[LoginData] attempt to create singleton twice!")
		return
	end
	LoginData.Instance = self

	self.account = SmartData.New({
		plat_name = "", 		--由登录验证返回的登录唯一账号 一般以渠道返回的user_id区别是拼接了渠道号
		key = "",				--渠道返回的user_id
		spid = "",				--由登录验证返回的渠道号（涉及可能会在支付时拼接订单id用,不用母包可能会返回不同的子渠道号）
		login_str = "",			--登录验证返回的用于二次验证的key
		login_time = 0,			--登录时间由登录验证返回（如果sdk有返回会在登录验证时发送并原样转回）
		roles = {},
		access_token = "", 		--登录验证返回的access_token,有些sdk在游戏中的充值行为需要用
		login_roles = {},   --用于我的番队选择用
		account_type = 0, 		--用户类型 0普通用户 1白名单用户（超级玩家）
	})

	self.account_cache = nil
	self.view_sel_server_id = SmartData.New({server_id = -1, account_spid = "", server_list_index = 0})
	self.base_data = SmartData.New({
		login_state = LoginStateType.None,
		login_try_count = 0,
		wait_str = "",
		is_on_crosssever = false,
		real_is_on_crossserver = false,		--真正进入了跨服/原服 在收到ResvLoginScene协议后才会变化 21.11.09 临时添加 -wfc
		is_connected_cross = true,
	})

	self.user_protocol = SmartData.New({
		is_null = true,
		content = "",
		user_url = "",
		privacy_url = "",
		img_url = "",
	})
	self.role_data = nil
	self.notice_info = {}
	self.del_notice_info = {}
	-- 断线重登退回的界面
	self.back_type = RELOGIN_TYPE.WELCOME
	self.asset_version_unmatch = false

	self.is_show_notice = false

	-- 是否需要玩小游戏
	self.is_play_mini_game = false

	self.notice_view_smart = SmartData.New({flush_panel = false})
end

function LoginData:__delete()
	if LoginData.Instance == self then
		LoginData.Instance = nil
	end	
end

function LoginData:Clear()
	self.account.key = ""
	self.base_data.is_on_crosssever = false		--临时代码，断线重连暂时让玩家返回跨服
	self.base_data.real_is_on_crossserver = false
	self.base_data.is_connected_cross = true
	self:SetLoginState(LoginStateType.None)
end

function LoginData:SetServerData(protocol)
    local last_server_id = self.view_sel_server_id.server_id > 0 and self.view_sel_server_id.server_id or self:GetLastLoginServer()
    local last_account_spid = self:GetLastLoginAccountSpid()
    local server_info = self:GetServerInfoById(last_server_id, last_account_spid)
    local platform_name = self:GetPlatformName()
	
	self:SetCurrentServerData("id", protocol.role_id)
    self:SetCurrentServerData("name", protocol.name)
	self:SetCurrentServerData("level", protocol.level)
	self:SetCurrentServerData("top_level", protocol.top_level)
	
    UnityPlayerPrefs.SetInt(server_info.id .. platform_name .. "role_level", protocol.level)
    UnityPlayerPrefs.SetInt(server_info.id .. platform_name .. "avatar_type", protocol.appearance.avatar_type)
	UnityPlayerPrefs.SetInt(server_info.id .. platform_name .. "avatar_id", protocol.appearance.avatar_id)
	UnityPlayerPrefs.SetInt(server_info.id .. platform_name .. "top_level", protocol.top_level)
	UnityPlayerPrefs.SetInt("LastEnterRoleServerId",server_info.id)
end

function LoginData:BaseData()
	return self.base_data
end

function LoginData:SetLoginState(login_state)
	self.base_data.login_state = login_state
end

function LoginData:MarkLoginStateError(error_code)
	if error_code ~= nil then
		self.base_data.login_state = error_code
		return
	end
	if self.base_data.login_state > 0 then
		self.base_data.login_state = -self.base_data.login_state
	end
end

--负数表示对应状态进度异常
function LoginData:LoginState()
	return self.base_data.login_state
end

function LoginData:GetLoginStateError()
	if self.base_data.login_state < 0 then
		return -self.base_data.login_state
	else
		return 0
	end
end

function LoginData:LoginTryCount()
	return self.base_data.login_try_count
end

function LoginData:LoginTryCountGrow()
	self.base_data.login_try_count = self.base_data.login_try_count + 1
	return self.base_data.login_try_count
end

function LoginData:LoginTryCountReset()
	self.base_data.login_try_count = 0
end

function LoginData:WaitStr()
	return self.base_data.wait_str
end

function LoginData:SetWaitStr(str)
	self.base_data.wait_str = str
end

--------account data----------------
function LoginData:GetAccountKey()
	return self.account.key
end

function LoginData:SetAccountKey(key)
	self.account.key = key
end

function LoginData:AccountData()
	return self.account
end

-- 清除封测充值金额
function LoginData:ResetFCRecharge()
	self.account.fc_recharge = 0
end

-- 返回需要玩的小游戏
function LoginData:GetMiniGame()
	return tonumber(self.account.mini_game)
end

-- 是否需要刚开始就玩小游戏
function LoginData:IsPlayMiniGame()
	return tonumber(self.account.mini_game) > 0 and self.is_play_mini_game
end

-- 设置是否需要刚开始就玩小游戏
function LoginData:SetIsPlayMiniGame(value)
	self.is_play_mini_game = value
end

function LoginData:GetAccountSpid()
	return StringIsEmpty(self.account.spid) and GLOBAL_CONFIG:PkgInfo().agent_id or self.account.spid
end

function LoginData:GetPlatformName()
	-- LogError(self.view_sel_server_id.account_spid)
	if not StringIsEmpty(self.view_sel_server_id.account_spid) then
		self.account.plat_name = string.format("%s_%s",self.view_sel_server_id.account_spid,self:GetAccountKey())
		return self.account.plat_name
	else
		return self.account.plat_name ~= "" and self.account.plat_name or string.format("%s_%s",self:GetAccountSpid(),self:GetAccountKey())
	end
	-- return string.format("%s_%s",self:GetAccountSpid(),self:GetAccountKey())
end

function LoginData:GetPhpLoginStr()
	return self.account.login_str
end

function LoginData:GetPhpLoginTime()
	return self.account.login_time
end

local function NewRoleItem(role_data)
	return {
		id = role_data.role_id,
		name = role_data.role_name,
		level = role_data.level,
		color = role_data.color,
		avatar_type = role_data.avatar_type,
		avatar_id = role_data.avatar_id,
		profession = role_data.profession,
		weapon_id = role_data.weapon_id,
		surface_list = role_data.surface_list,
		forbid_time = role_data.forbid_time,
		top_level = role_data.top_level,
		fly_flag = role_data.fly_flag,
		avatar_quality = role_data.fly_flag,
	}
end

function LoginData:GetViewSelServer() 
	return self.view_sel_server_id
end 

function LoginData:GetLastLoginRole()
	local login_roles = self:GetLoginRoles()
	local re = login_roles[self:GetLastLoginIndex() + 1]
	return re
end

--上一次登录的角色序号（选择角色界面）
function LoginData:SetLastLoginIndex(index)
	self.last_login_index = index
end

function LoginData:GetLastLoginIndex()
	return self.last_login_index or 0
end

function LoginData:SetLoginRoles(role_list)
	self.account.login_roles = {}
	local get_forbid_text_flag = false
	for k,v in pairs(role_list) do
		self.account.login_roles[k] = NewRoleItem(v)
		if self.account.login_roles[k].forbid_time > 0 then
			get_forbid_text_flag = true
		end
	end
	if get_forbid_text_flag then
		LoginCtrl.Instance:RequestForbidTips()
	end
end

function LoginData:AddLoginRoles(role_data)
	self.account.login_roles[#self.account.login_roles+1] = NewRoleItem(role_data)
end

function LoginData:GetLoginRoleById(role_id)
	for k,v in pairs(self.account.login_roles) do
		if v.id == role_id then
			return v
		end
	end
end

function LoginData:GetLoginRoles()
	local result_list = {}
	for i = 1, 5 do
		local vo = {}
		if self.account.login_roles[i] ~= nil then 
			vo = self.account.login_roles[i]
			vo.index = i
		else 
			vo.index = i
			vo.id = -1
			vo.name = 0
			vo.level = 0
			vo.avatar_type = 0
			vo.avatar_id = 0
			vo.color = 0
			vo.profession = 0
		end 
		table.insert( result_list, vo )
	end 
	return result_list
end 

function LoginData:GetForbidRoleInfo()
	local role_data = RoleSelectData.Instance:GetSelectRoleData()
	return string.format(Language.Login.ForbidRoleInfoFormat, role_data.name, role_data.id, self:ServerName())
end

function LoginData:GetNewCreateRoles()
	return self.account.login_roles[#self.account.login_roles]
end

function LoginData:SetForbidTipInfo(remote_config)
	self.role_forbid_use_php = true
	self.role_forbid_tip = Language.Login.DearPlayer .. remote_config.data[1].content
end

------------Account local data cache----------------
local file_path = UnityApp.persistentDataPath .. "/UserData.txt"


local function create_account_cache()
	return {
		last_login_server = -1,
		last_account_spid = "",
		password = nil,
	}
end

function LoginData:getAccountCache()
	if self.account_cache == nil then
		if FileTool.Exists(file_path) then
			self.account_cache = TryParseJson(FileTool.ReadAllText(file_path)) or { __last_account=nil }
		else
			self.account_cache = { __last_account=nil }
		end
	end
	return self.account_cache	
end

function LoginData:getAccountCacheData(account_key)
	local cache = self:getAccountCache()
	local data = cache[account_key]
	if data == nil then
		data = create_account_cache()
		cache[account_key] = data
	end
	return data
end

function LoginData:saveAccountCache()
	if self.account_cache ~= nil and not IS_AUDIT_VERSION then
		FileTool.WriteAllText(file_path,cjson.encode(self.account_cache))
	end
end

function LoginData:GetLastLoginAccount()
	return self:getAccountCache().__last_account	
end

function LoginData:GetLastLoginServer(account_key)
	if account_key == nil then
		account_key = self:GetAccountKey()
	end
	local data = self:getAccountCacheData(account_key)
	if data.last_login_server < 0 then
		return GLOBAL_CONFIG:ServerInfo().last_server, false
	else
		return data.last_login_server, true
	end
end

function LoginData:GetLastLoginAccountSpid(account_key)
	if account_key == nil then
		account_key = self:GetAccountKey()
	end
	local data = self:getAccountCacheData(account_key)
	if not StringIsEmpty(data.last_account_spid) then
		return data.last_account_spid
	elseif not StringIsEmpty(self.view_sel_server_id.account_spid) then
		return self.view_sel_server_id.account_spid
	else
		return GLOBAL_CONFIG:ServerInfo().account_spid
	end
end

function LoginData:GetPassword(account_key)
	local data = self:getAccountCacheData(account_key)
	return data.password
end

function LoginData:SaveLastLoginAccount(account_key, password)
    if account_key == nil or StringIsEmpty(account_key) then
        return
    end
    local data = self:getAccountCacheData(account_key)
    self.account_cache.__last_account = account_key
    data.password = password
    self:saveAccountCache()
end

function LoginData:SaveLastLoginServer(account_key,server_id)
	local data = self:getAccountCacheData(account_key)
	data.last_login_server = server_id
	data.last_account_spid = self.view_sel_server_id.account_spid
	self:saveAccountCache()
end

------------Server data----------------
function LoginData:GetServerList()
	return GLOBAL_CONFIG:ServerInfo().server_list
end

function LoginData:GetServerData()
	local recommend_server = {}
	local mine_server = nil
	local all_server = {}
	local account_cache = LoginData.Instance:getAccountCache()
	local server = LoginData.Instance:GetLastLoginServer()
	local server_info = GLOBAL_CONFIG:ServerInfoOther(self.view_sel_server_id.server_list_index)

	local server_offset = server_info.server_offset or 0
    if server_offset >= 1500 then
        server_offset = 0
    end
	local plat_account_type = self.account.account_type
    if IS_EDITOR then
        plat_account_type = PLAT_ACCOUNT_TYPE_TEST
    end
    local recommend_id_list = nil
    if not StringIsEmpty(server_info.recommand_gs_list) and next(server_info.recommand_gs_list) then
    	recommend_id_list = {}
    	for k,v in pairs(server_info.recommand_gs_list) do
    		recommend_id_list[v] = 1
    	end
    end

	-- LogError("GetServerData",self.view_sel_server_id.server_list_index, server_list)
	for i,v in pairs(server_info.server_list) do
		if PLAT_ACCOUNT_TYPE_TEST == plat_account_type or 4 ~= v.flag and v.id > server_offset then -- 非测试号屏蔽测试服
			if recommend_id_list ~= nil then
				if recommend_id_list[v.id] == 1 then
					table.insert(recommend_server, v)
				end
			else
				table.insert(recommend_server, v)
			end
			table.insert( all_server, v)
			local server_key = account_cache.__last_account .. v.id
			local role_id_key = server_key .. "id"
			local role_id_value = UnityPlayerPrefs.GetInt(role_id_key)
			if role_id_value > 0 then
				v.role_id = role_id_value
				local role_name_key = server_key .. "name"
				v.role_name = UnityPlayerPrefs.GetString(role_name_key)
				local role_name_key = server_key .. "level"
				v.level = UnityPlayerPrefs.GetInt(role_name_key)
				local role_name_key = server_key .. "top_level"
				v.top_level = UnityPlayerPrefs.GetInt(role_name_key)
				if mine_server == nil then
					mine_server = {}
					mine_server.type_name = Language.Login.MineServer
				end
				table.insert( mine_server, v)
			end
		end
	end
	local data_list = {}
	data_list[#data_list + 1] = mine_server
	if recommend_id_list == nil then
		table.sort( recommend_server, DataSorter.KeyUpperSorter("open_time"))
		for i=1,#recommend_server do
			if #recommend_server > 1 then
				table.remove(recommend_server)
			else
				break
			end
		end
	end

	recommend_server.type_name = Language.Login.RecommendServer
	data_list[#data_list + 1] = recommend_server

	table.sort( all_server, DataSorter.KeyLowerSorter("id"))
	local area_index = 1
	local start_id = 0
	local offset = server_info.server_offset or 0
	local temp_list = {}
	for i,v in ipairs(all_server) do
		if temp_list[area_index] == nil then
			temp_list[area_index] = {}
			start_id = v.id - offset
		end

		table.insert(temp_list[area_index], v)

		if #temp_list[area_index] >= 10 or i == #all_server then
			if IS_EDITOR then
				temp_list[area_index].type_name = string.format(Language.Login.AreaServerEditor,start_id,v.id - offset)
			else
				temp_list[area_index].type_name = string.format(Language.Login.AreaServer,start_id,v.id - offset)
			end
			area_index = area_index + 1
		end
	end
	for i = #temp_list, 1, -1 do
		table.insert(data_list, temp_list[i])
	end

	return data_list
end

function LoginData:GetServerInfoById(server_id, account_spid)
	local server_list_cfg = GLOBAL_CONFIG:ServerInfoOther()
	for i = 0, 3 do
		local server_info = server_list_cfg[i]
		if server_info then
			if StringIsEmpty(server_info.account_spid) or server_info.account_spid == account_spid then
				for k,v in pairs(server_info.server_list) do
					if v.id == server_id then
						return v
					end
				end
			end
		end
	end
	local server_list = GLOBAL_CONFIG:ServerInfo().server_list
	for k,v in pairs(server_list) do
		if v.id == server_id then
			return v
		end
	end
	return nil
end

function LoginData:SetCurrentServerData(data_key,value)
	local account_cache = LoginData.Instance:getAccountCache()
	local server = LoginData.Instance:GetLastLoginServer()
	local key = account_cache.__last_account .. server .. data_key
	if type(value) == "string" then
		UnityPlayerPrefs.SetString(key,value)
	elseif type(value) == "number" then
		UnityPlayerPrefs.SetInt(key,value)
	end
end
	
function LoginData:ServerName()
	local info = self:GetServerInfoById(LoginData.Instance:GetLastLoginServer(), LoginData.Instance:GetLastLoginAccount())
	if info then
		return info.name or ""
	else
		return ""
	end
end

function LoginData:SaveLoginRoleData(data)
	self.role_data = TableCopy(data)
end

function LoginData:RoleData()
	return self.role_data
end

function LoginData:NewCharaCreateComplete()
	self.is_new_role = true
end

function LoginData:IsNewChara()
	return self.is_new_role or false
end

function LoginData:ClearIsNewChara()
	self.is_new_role = nil
end

function LoginData:GetNoticeData()
	return self.notice_info
end 

function LoginData:GetDeleteAccountNoticeData()
	return self.del_notice_info
end

function LoginData:GetUserProtocol()
	return self.user_protocol
end 

function LoginData:GetPublishInfo()
	local urls = GLOBAL_CONFIG:Urls()
	if urls.publish_info == nil then return "" end 
	local company_info = GLOBAL_CONFIG:Urls().publish_info.company or ""
	local game_num = GLOBAL_CONFIG:Urls().publish_info.game_num or ""
	local str = company_info.." "..game_num
	return str
end

-- 判断是否显示pk的勾选
function LoginData:IsHasPKLogin()
	local flags = GLOBAL_CONFIG:Flags()
	return flags.is_pk == true
end

-- 设置或者拿到当前的pk toggle，value传0或1
function LoginData:PKToggle(value)
	if value then
		PrefsInt(PrefKeys.LoginPKToggle(), value)
	else
		return PrefsInt(PrefKeys.LoginPKToggle())
	end
end

function LoginData:SetLoginUserProtocol(remote_config)
	if StringIsEmpty(remote_config.data) then 
		self.user_protocol.is_null = true
		return 
	end
	self.user_protocol.is_null = false
	self.user_protocol.content = remote_config.data[1].content
	self.user_protocol.user_agree_url = remote_config.data[1].user_agree_url
	self.user_protocol.privacy_url = remote_config.data[1].privacy_url
	self.user_protocol.img_url = remote_config.data[1].img_url
end

function LoginData:SetDeleteAccountNotice(remote_config)
	self.del_notice_info = {}
	if StringIsEmpty(remote_config.data) then
		self.notice_view_smart.is_null = true
		return
	end
	self.notice_view_smart.is_null = false
	self.del_notice_info.content = {}

	local vo = {}
	vo.title = Language.Login.DeleteAccountNotice
	vo.text = remote_config.data[1].content
	table.insert(self.del_notice_info.content , vo)
	
	-- self.del_notice_info = remote_config.data[1].content
	
	self.notice_view_smart:SetDirty("flush_panel")
end

function LoginData:SetLoginNoticeInfo(remote_config)
	self.notice_info = {}
	if StringIsEmpty(remote_config.data) then 
		self.notice_info.is_null = true
		return 
	end 
	self.notice_info.is_null = false
	self.notice_info.content = {}
	for k,v in ipairs(remote_config.data) do
		local vo = {}
		vo.title = v.title
		vo.text = v.content
		vo.beg_time = v.beg_time
		vo.end_time = v.end_time
		vo.add_time = v.add_time
		table.insert( self.notice_info.content , vo )
	end 

	-- if not ViewMgr:IsOpen(LoginView) then 
	-- 	return 
	-- end 
	self:CheckShowLoginNotice()
end

function LoginData:CheckShowLoginNotice()
	-- 小游戏的时候不能弹公告
	if LoginStateType.Logined == LoginData.Instance:LoginState() or IS_AUDIT_VERSION or self.notice_info.content == nil or self:IsPlayMiniGame() then return end
	
	if UnityPlayerPrefs.GetInt(PrefKeys.LoginNoticeNews(self:GetLastLoginAccount())) == 0 or
	UnityPlayerPrefs.GetInt(PrefKeys.LoginNoticeNews(self:GetLastLoginAccount())) ~= tonumber(self.notice_info.content[1].add_time) then 
	
	ViewMgr:OpenView(LoginNoticeView)
	UnityPlayerPrefs.SetInt(PrefKeys.LoginNoticeNews(self:GetLastLoginAccount()),tonumber(self.notice_info.content[1].add_time))
    end 
end

function LoginData:MarkAssetVersionUnmatch()
	self.asset_version_unmatch = true
end

function LoginData:AssetVersionUnmatch()
	return self.asset_version_unmatch
end

function LoginData:SetForbidTime(_time)
	self.login_forbid_time = _time
end

function LoginData:GetForbidTime(_time)
	return self.login_forbid_time
end

--是否在跨服
function LoginData:IsOnCrossSever()
	return self:BaseData().is_on_crosssever
end

--跨服是否连通
function LoginData:IsConnectedCross()
	return self:BaseData().is_connected_cross
end

function LoginData:SetLastRoleData(param_t)
	self.last_login_role_data = param_t
end

function LoginData:GetLastRoleData()
	return self.last_login_role_data
end

function LoginData:SetLoginBackType(type_)
	-- LogError("SetLoginBackType?",type_ == 0 and "welcome" or "select")
	self.back_type = type_
end

function LoginData:GetLoginBackType()
	return self.back_type
end

function LoginData:BlockChangeName()
	if not StringIsEmpty(GLOBAL_CONFIG:Urls().block_change_name) then
		local time_t = string.split(GLOBAL_CONFIG:Urls().block_change_name,"_")
		if #time_t == 2 then
			local block_start = tonumber(time_t[1])
			local block_end = tonumber(time_t[2])
			if TimeCtrl.Instance:GetServerTime() >= block_start and TimeCtrl.Instance:GetServerTime() < block_end then
				local reopen_time_t = TimeManager.FormatUnixTime2Date(block_end + 1)
				local notice_str = string.format(Language.Login.BlockChangeNameNotice,reopen_time_t.month,reopen_time_t.day,tonumber(reopen_time_t.hour),tonumber(reopen_time_t.minute))
				return true, notice_str
			else
				return false
			end
		else
			return false
		end
	else
		return false
	end
end

function LoginData:BlockBattleStrategy()
	if not StringIsEmpty(GLOBAL_CONFIG:Urls().block_battle_strategy) then
		local time_t = string.split(GLOBAL_CONFIG:Urls().block_battle_strategy,"_")
		if #time_t == 2 then
			local block_start = tonumber(time_t[1])
			local block_end = tonumber(time_t[2])
			if TimeCtrl.Instance:GetServerTime() >= block_start and TimeCtrl.Instance:GetServerTime() < block_end then
				local reopen_time_t = TimeManager.FormatUnixTime2Date(block_end + 1)
				local notice_str = string.format(Language.Login.BlockChangeNameNotice,reopen_time_t.month,reopen_time_t.day,tonumber(reopen_time_t.hour),tonumber(reopen_time_t.minute))
				return true, notice_str
			else
				return false
			end
		else
			return false
		end
	else
		return false
	end
end

function LoginData:SetPCSdkExtraInfo(extra_info)
	self.pc_sdk_extra_info = extra_info
end

function LoginData:PCRealNameSignNeedOpen()
	if ChannelAgent.ISPCSDK and ChannelAgent.ISPCSDK() then
		if ChannelAgent.PCAgentType then
			local agent_type = ChannelAgent.PCAgentType()
			if agent_type == "_360PC" then
				return self.pc_sdk_extra_info == 0 or self.pc_sdk_extra_info == "0"
			end
		end
	end
	return false
end

function LoginData:GetRealNameReward()
	local agent_cfg = GLOBAL_CONFIG:AgentAdaptInfoById(GLOBAL_CONFIG:PkgInfo().agent_id)
	local item_list = {}
	if not TableIsEmpty(agent_cfg) then
		for k,v in pairs(agent_cfg.real_name_reward) do
			table.insert(item_list, Item.Init(v.item_id, v.num, v.is_bind))
		end
	end
	return item_list
end