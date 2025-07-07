	GLOBAL_CONFIG = {

	urls = nil,

	flags = nil,

	server_info = nil,

	pkg_info = nil,

	remote_pkg_info = {
		assets_version = "",
		manifest_md5 = nil,
		file_info_md5 = nil,
		file_json_md5 = nil,
	},

	reminder_info = {
		up_version = nil,
		down_version = nil,
		version_tip = nil,
		version_url = nil,
	},

	client_time = 0,                    -- 更新数据时的客户端时间(query返回是记录)
}

local localConfig = require("preload/local_config")

function GLOBAL_CONFIG:GetUrlHead()
	return "192.168.0.3:81//"
end

local function constInitUrl()
	local url_heal = GLOBAL_CONFIG:GetUrlHead()
	return string.format("http://%s/api/query/api/type/%s", url_heal, InstallPackageInfo:AgentId())

end

------------Get&Create Config Function--------------

local function createUrlConfig()
	local new_cfg = {
		init_url = nil,		--配置获取地址
		config_url = nil,		--配置获取地址(备用)
		report_url = "",		--埋点事件上报
		update_url = nil,		--资源更新地址
		update_url2 = nil,		--备用资源更新地址
		verify_url = nil,		--登录验证URL
		upload_url = nil,		--资源上传地址
		gift_fetch_url = nil,	--兑换码地址
		charge_url = nil,		--创建订单url 个别sdk需要先发送url申请获取订单信息需要传入sdk层
		popup_notice_url = nil,	--游戏内获取公告
		act_query_url = nil,	--不重启更新外部代码下载地址
	}
	local localCfg = localConfig:Get()
	new_cfg.init_url = constInitUrl()
	new_cfg.config_url = localCfg.config_url
	new_cfg.report_url = localCfg.report_url
	return new_cfg
end


function GLOBAL_CONFIG:Urls()
	if self.urls == nil then
		self.urls = createUrlConfig()
	end
	return self.urls
end

function GLOBAL_CONFIG:Flags()
	if self.flags == nil then
		self.flags = {
			audit_version = false,	--是否是审核版本
			update_assets = false,	--是否更新资源
			countly_report = true,  --埋点上报控制开关（query返回false后不再有埋点上报）
			voice = false,			--语音功能接口开关
		}
	end
	return self.flags
end

function GLOBAL_CONFIG:ServerInfoOther(index)
	if index then
		return self.server_info_others[index]
	else
		return self.server_info_others
	end
end

function GLOBAL_CONFIG:ServerInfo()
	if self.server_info == nil then
		self.server_info = {
			last_server = 0,
			server_time = 0,
			server_list = {
				[1] = {
					id = 0,
					name = "Invalid Server",
					ip = "",
					port = 0,
					flag = 0,
					avatar = "",
					role_name = "",
					role_level = 0,
					open_time = 0,
					ahead_time = 0,
					report_port = 0,--战报请求端口
				}
			}
		}
	end
	return self.server_info
end

function GLOBAL_CONFIG:PkgInfo()
	if self.pkg_info == nil then
		self.pkg_info = {
			-- identifier = UnityEngine.Application.identifier,
			agent_id = InstallPackageInfo.AgentId(),
			version = InstallPackageInfo.Version(),
			assets_version = "",
			device_id = UnityEngine.SystemInfo.deviceUniqueIdentifier,
			pkg = InstallPackageInfo.Pkg(),
			date = InstallPackageInfo.Date(),
			time = InstallPackageInfo.Time(),
			is_youyan = InstallPackageInfo.IsYouYan() == 1,
			is_small_pkg = InstallPackageInfo.IsSmallPkg() == 1,
			mini_game_type = InstallPackageInfo.MiniGameType(),
			root_pkg = InstallPackageInfo.RootPkg() == 1,
			single = InstallPackageInfo.IsSingle()
		}
	end
	if not SIMULATE_ASSETBUNDLE then
		if IS_MICRO_PLAYER() then
			local wpi = Game.WebGLPkgInfo.Singleton().Info
			if wpi ~= nil then
				self.pkg_info.assets_version = wpi.assetVersion
			end
		else
			self.pkg_info.assets_version = Nirvana.AssetManager.LoadVersion()
		end
	end
	if IS_EDITOR and EditorSettings:OfficialLogin() then
		self.pkg_info.version = self.pkg_info.is_youyan and "999" or "99999"
		--self.pkg_info.pkg = "1.0.0"
	end
	return self.pkg_info
end

function GLOBAL_CONFIG:RemotePkgInfo()
	return self.remote_pkg_info
end

function GLOBAL_CONFIG:ReminderInfo()
	return self.reminder_info or {}
end

-----------------Set&Save Configs Function-----------
function GLOBAL_CONFIG:SetUrls(config)
	self.urls = config
	self.urls.init_url = constInitUrl()
	local localCfg = localConfig:Get()
	localCfg.config_url = self.urls.config_url
	localCfg.report_url = self.urls.report_url

	--TEMP_CODE
	-- local wpi = Game.WebGLPkgInfo.Singleton().Info
	-- if wpi then
	-- 	self.urls.update_url = wpi.assetUrl
	-- 	self.urls.update_url2 = nil
	-- 	UnityEngine.Debug.LogError("update_url===" .. self.urls.update_url)
	-- end	

	return urls
end

function GLOBAL_CONFIG:SetFlags(config)
	self.flags = config
	return self.flags
end

function GLOBAL_CONFIG:SetReminderInfo(config)
	self.reminder_info = config or self.reminder_info
	return self.reminder_info
end

function GLOBAL_CONFIG:SetServerInfo(config)
	self.server_info = config
	-- self.server_info.account_spid = "ml6"
	return self.server_info
end

function GLOBAL_CONFIG:SetServerInfoOther(index,config)
	self.server_info_others = self.server_info_others or {}
	config.index = index
	self.server_info_others[index] = config
end

function GLOBAL_CONFIG:SetRemotePkgInfo(config)
	self.remote_pkg_info.assets_version = config.assets_info.version
	self.remote_pkg_info.manifest_md5 = config.assets_info.manifest_md5
	self.remote_pkg_info.file_info_md5 = config.assets_info.file_info_md5
	self.remote_pkg_info.file_json_md5 = config.assets_info.file_json_md5
	return self.remote_pkg_info
end

function GLOBAL_CONFIG:SaveConfig()
	local localCfg = localConfig:Get()
	localConfig:Save()
end

function GLOBAL_CONFIG:AgentAdaptInfoById(agent_id)
	if nil == self.agent_adapt_list then
		self.agent_adapt_list = {}
		for _, v in pairs(Config.agent_adapt_auto.agent_adapt) do
			self.agent_adapt_list[v.spid] = v
		end
	end
	if nil == self.agent_adapt_list[agent_id] then
		Debuger.LogWarning("not found agent_id:" .. agent_id .. " in agent_adapt_auto!")
	end
	return self.agent_adapt_list[agent_id] or {}
end

function GLOBAL_CONFIG:ChatfilterAutoInfoById(agent_id)
	if nil == self.agent_chatfilter_list then
		self.agent_chatfilter_list = {}
		for _, v in pairs(Config.chatfilter_auto.title_1) do
			self.agent_chatfilter_list[v.seq] = v
		end
	end
	if nil == self.agent_chatfilter_list[agent_id] then
		Debuger.LogWarning("not found agent_id:" .. agent_id .. " in chatfilter_auto/title_1!")
	end
	return self.agent_chatfilter_list[agent_id] or {}
end

function GLOBAL_CONFIG:GameName()
	local agent_adapt_info = self:AgentAdaptInfoById(self.pkg_info.agent_id)
	return agent_adapt_info.game_name or UnityEngine.Application.productName
end

function GLOBAL_CONFIG:TalkLevel()
	local agent_adapt_info = self:AgentAdaptInfoById(self.pkg_info.agent_id)
	return agent_adapt_info.talk_level or TalkLevel.Default
end

function GLOBAL_CONFIG:TalkLevelSiLiao()
	local agent_adapt_info = self:AgentAdaptInfoById(self.pkg_info.agent_id)
	return agent_adapt_info.talk_level_siliao or TalkLevel.Default
end

function GLOBAL_CONFIG:HornGrade()
	local agent_adapt_info = self:AgentAdaptInfoById(self.pkg_info.agent_id)
	return agent_adapt_info.horn_grade or TalkLevel.Default
end

function GLOBAL_CONFIG:HornGradeCross()
	local agent_adapt_info = self:AgentAdaptInfoById(self.pkg_info.agent_id)
	return agent_adapt_info.horn_grade_cross or TalkLevel.Default
end

function GLOBAL_CONFIG:CanChangeMode()
	local agent_adapt_info = self:AgentAdaptInfoById(self.pkg_info.agent_id)
	if agent_adapt_info and agent_adapt_info.is_change_battle_mode then
		return agent_adapt_info.is_change_battle_mode == 1
	end
	return false
end

function GLOBAL_CONFIG:ChatFilterTable()
	local agent_chatfilter_info = self:ChatfilterAutoInfoById(self.pkg_info.agent_id)
	return agent_chatfilter_info.table
end

function GLOBAL_CONFIG:GetCreateLimitedInfo()
	local info = {}
	info.regist_limit = tonumber(self.urls.regist_limit)
	info.regist_serverid = tonumber(self.urls.regist_serverid)
	info.regist_allow_server = self.urls.regist_allow_server
	return info
end

function GLOBAL_CONFIG:FlagsVoice()
	if nil == self.flags.voice then
		return true
	else
		return self.flags.voice
	end
end

-- 专服id
function GLOBAL_CONFIG:BaseSpId()
	local server_info = self:ServerInfo()
	return server_info.merger_spid
end