GameStateUpdate = GameStateUpdate or BaseClass(GameState)
local nirvanaAssetMgr = Nirvana.AssetManager

local State =
{
	None = 0,
	Loading = 1,
	Failed = 2,
	Complete = 3,
}

-- AssetErrorCode = {
-- 	None = 0,
-- 	NetFailure = 1,						--木有开网络
-- 	DownFailure = 2,					--某个资源的问题
-- }

local CDMType =
{
	CDM = 1,
	NoCDM = 2,
}

-- local InstallAssetBundles = IS_IOS and require("preload/install_bundle_list_ios") or require("preload/install_bundle_list")
local InstallAssetBundles = GLOBAL_CONFIG:PkgInfo().is_small_pkg and require("preload/install_bundle_list_s") or require("preload/install_bundle_list")
local agent_id_abpath = string.format("agent/%s/",tostring(GLOBAL_CONFIG:PkgInfo().agent_id))
agent_id_abpath = agent_id_abpath .. "%s"
InstallAssetBundles[string.format(agent_id_abpath,"login_logo.png")] = true
InstallAssetBundles[string.format(agent_id_abpath,"loading.png")] = true
InstallAssetBundles[string.format(agent_id_abpath,"loading1.png")] = true
InstallAssetBundles[string.format(agent_id_abpath,"loading2.png")] = true
InstallAssetBundles[string.format(agent_id_abpath,"loading3.png")] = true
InstallAssetBundles[string.format(agent_id_abpath,"loading4.png")] = true
InstallAssetBundles[string.format(agent_id_abpath,"loading5.png")] = true
InstallAssetBundles[string.format(agent_id_abpath,"loading6.png")] = true
InstallAssetBundles[string.format(agent_id_abpath,"login_bg.png")] = true
InstallAssetBundles[string.format(agent_id_abpath,"update_bg0.png")] = true
-- table.insert(InstallAssetBundles,)
local RestartAssetBundles = {
	["lua/tolua"] = true,			--tolua 核心代码
	["lua/preload"] = true,			--preload lua
	["levels/game"] = true,		--game场景
	["preload/update.prefab"] = true,			--preload 资源
	["preload/health.prefab"] = true,			--preload 资源
	["shaders"] = true,				--shader
}


--临时设置所有bundle都需要更新
local function NeedUpdate(bundle_name)
	if FULL_RES_PLAYER == true then
	-- if true then
		return true
	end
	if InstallAssetBundles[bundle_name] then
		return true
	end
	if string.match(bundle_name,"^lua/.+$") then
		return true
	end
	-- for i,v in ipairs(InstallAssetBundles) do
	-- 	if string.match(bundle_name,v) then
	-- 		return true
	-- 	end
	-- end
	return false
end

local function NeedRestart(bundle_name)
	return RestartAssetBundles[bundle_name] == true
	-- for i,v in ipairs(RestartAssetBundles) do
	-- 	if string.match(bundle_name,v) then
	-- 		return true
	-- 	end
	-- end
	-- return false
end



function GameStateUpdate:__init()

	--KB
	-- self.download_speed = 0
	-- self.download_bytes = 0
	-- self.content_bytes = 0
	-- self.total_percent = 0		--  0-1

	self.current_download = 0

	-- self.downloaded_size = 0
	-- self.total_size = 0
	self.downloaded_files_size = 0		--记录所有已经下载完成的文件的总尺寸

	self.remote_manifest_state = State.None
	self.remote_file_info_state = State.None
	self.prepared = false

	self.open_download_count = false 	 --是否开启切换计时
	self.error_timer	= 0				 --错误计时器
	self.error_total_timer = 5		     --错误持续时间总长
	self.used_cmd_list = CDMType.CDM   --当前使用的CDM目录
	self.URLList = {}  					 --资源URL总数
	self.downFailure_Time = 0            --下载失败计数
	self.downFailure_TotalTime = 3	 	 --下载失败总次数
	self.update_bundles_index = 1		 --记录当前下载失败的bundle索引
end

function GameStateUpdate:Start()
	GameState.Start(self)
-- 	UnityEngine.Debug.LogErrorFormat("SAB=={0},BU={1},US={2},REC={3},remo_Ass={4},pkg_Ass={5}",
-- 	SIMULATE_ASSETBUNDLE,Game.LuaManager.BlockUpdate,
-- 	GLOBAL_CONFIG:Flags().update_assets,UnityPlayerPrefs.GetInt(PrefKeys.UpdateRecheck,1),
-- 	GLOBAL_CONFIG:RemotePkgInfo().assets_version,
-- 	GLOBAL_CONFIG:PkgInfo().assets_version
-- )
	-- print("IsSmallPkg~~~~~~~~~~~~~~ "..tostring(GLOBAL_CONFIG:PkgInfo().is_small_pkg))
	if GLOBAL_CONFIG:PkgInfo().single then
		self:Stop()
		return
	end
	MainProber:Step(MainProber.STEP_ASSET_UPDATE_BEG)
	self:InitUrl()
	self:SwitchUrl()
	nirvanaAssetMgr.DefualtUrlVariant = GLOBAL_CONFIG:RemotePkgInfo().assets_version
	if false then
		self:Stop()
		return
	end

	if SIMULATE_ASSETBUNDLE or Game.LuaManager.BlockUpdate == true then
		self:Stop()
		return
	end

	if GLOBAL_CONFIG:Flags().update_assets == false then	--后台说不要更新
		self:Stop()
		return
	end

	if IS_MICRO_PLAYER() then		--webGL平台不需要检查更新
		self:Stop()
		return
	end

	if UnityPlayerPrefs.GetInt(PrefKeys.UpdateRecheck,1) ~= 1 and	--默认要检查一次资源
		GLOBAL_CONFIG:PkgInfo().assets_version == GLOBAL_CONFIG:RemotePkgInfo().assets_version then--版本号相同不需要检测更新
		self:Stop()
		return
	end

	
	PreloadData.Instance:SetState(PreloadData.State.UpdateCheck)
	--审核版本的特殊更新资源列表，暂时注释掉
	-- if IS_AUDIT_VERSION == true then
	-- 	InstallAssetBundles = {
	-- 	"^lua/.*",
	-- 	"^levels/game.*",
	-- 	"^ui/ui_widgets/.*",
	-- 	"^ui/localization.txt$",}
	-- end
	-- UnityEngine.Debug.LogError("Start:LoadRemoteManifest")
	self:LoadRemoteManifest()
	self:LoadRemoteBundleFileInfo()
	
end

local function createUrlTable()
	return
	{
		url_list = {},
		used_url_index = 1,
	}
end

function GameStateUpdate:InitUrl()
	local url1 = GLOBAL_CONFIG:Urls().update_url
	local url2 = GLOBAL_CONFIG:Urls().update_url2

	if url1 ~= nil and url1 ~= "" then
		local UrlTable = createUrlTable()
		table.insert(UrlTable.url_list,url1)
		self.URLList[CDMType.CDM] = UrlTable
	end

	if url2 ~= nil and url2 ~= "" then
		local UrlTable = createUrlTable()
		table.insert(UrlTable.url_list,url2)
		self.URLList[CDMType.NoCDM] = UrlTable
	end
end

function GameStateUpdate:LoadRemoteManifest()
	print("LoadRemoteManifest~~~~~~~~~")
	self.remote_manifest_state = State.Loading
	local com_func = function (err)
		if err ~= nil and err ~= "" then
			self.remote_manifest_state = State.Failed
		else    --md5 verify
			local manifest_file_path = UnityEngine.Application.persistentDataPath .. "/AssetCache/AssetBundle"
			if VersionTag.HasTag(VersionTag.AssetCachePath) then
				manifest_file_path = nirvanaAssetMgr.AssetCachePath() .. "/AssetBundle"
			end
			if FileTool.Exists(manifest_file_path) == false then
				DebugLog("LoadRemoteManifest5")
				self.remote_manifest_state = State.Failed

			elseif GLOBAL_CONFIG:RemotePkgInfo().manifest_md5 ~= nil and
				GLOBAL_CONFIG:RemotePkgInfo().manifest_md5 ~= ""  and
				string.upper(GameUtil.FileMd5(manifest_file_path)) ~= GLOBAL_CONFIG:RemotePkgInfo().manifest_md5 then
				print("manifest md5 compare failed====" .. string.upper(GameUtil.FileMd5(manifest_file_path)) .. ",remote md5=" .. tostring(GLOBAL_CONFIG:RemotePkgInfo().manifest_md5))
				FileTool.Delete(manifest_file_path)
				self.remote_manifest_state = State.Failed
			else
				print("manifest md5====" .. string.upper(GameUtil.FileMd5(manifest_file_path)) .. ",remote md5=" .. tostring(GLOBAL_CONFIG:RemotePkgInfo().manifest_md5))
				self.remote_manifest_state = State.Complete
			end
		end
	end
	if VersionTag.HasTag(VersionTag.ManifestFileBundleInfoHash) then
		nirvanaAssetMgr.LoadRemoteManifest(
			"AssetBundle",
			com_func,nil)
	else
		nirvanaAssetMgr.LoadRemoteManifest(
			"AssetBundle",
			com_func)
	end
end

function GameStateUpdate:LoadRemoteBundleFileInfo()
	print("LoadRemoteBundleFileInfo~~~~~~~~~")

	self.remote_file_info_state = State.Loading
	local com_func = function (err)
		if err ~= nil and err ~= "" then
			print("LoadRemoteBundleFile failed 1 ".. err)
			self.remote_file_info_state = State.Failed
		else
			local file_name = VersionTag.HasTag(VersionTag.FileInfoJson) and "file_json.txt" or "file_info.txt"
			local file_info_path = UnityEngine.Application.persistentDataPath .. "/AssetCache/"..file_name
			if VersionTag.HasTag(VersionTag.AssetCachePath) then
				file_info_path = nirvanaAssetMgr.AssetCachePath() .. "/" .. file_name
			end
			local file_md5 = VersionTag.HasTag(VersionTag.FileInfoJson) and GLOBAL_CONFIG:RemotePkgInfo().file_json_md5 or GLOBAL_CONFIG:RemotePkgInfo().file_info_md5
			if FileTool.Exists(file_info_path) == false then
				print("LoadRemoteBundleFile failed 2")
				self.remote_file_info_state = State.Failed

			elseif file_md5 ~= nil and file_md5 ~= "" and string.upper(GameUtil.FileMd5(file_info_path)) ~= file_md5 then

				print("file_info md5 compare failed====" .. string.upper(GameUtil.FileMd5(file_info_path)) .. ",remote md5=" .. tostring(file_md5))
				FileTool.Delete(file_info_path)
				self.remote_file_info_state = State.Failed
			else
				print("file_info md5====" .. string.upper(GameUtil.FileMd5(file_info_path)) .. ",remote md5=" .. tostring(file_md5))
				self.remote_file_info_state = State.Complete
			end
		end
	end
	if VersionTag.HasTag(VersionTag.ManifestFileBundleInfoHash) then
		nirvanaAssetMgr.LoadBundleFileInfo(com_func,nil)
	else
		nirvanaAssetMgr.LoadBundleFileInfo(com_func)
	end
end

function GameStateUpdate:OnPrepared()
	MainProber:Step(MainProber.SETP_ASSET_UPDATE_PREPARE)
	if self.require_update_asset == false then
		self:onUpdateComplete()
		return
	end

	local manifest = nirvanaAssetMgr.Manifest
	if manifest == nil then
		self:onUpdateComplete()
		return
	end

	self.need_restart = false
	self.need_recheck = false
	self.need_resetup_lua = false
	local update_bundles = {}
	local bundles = nil
	bundles = manifest:GetAllAssetBundles():ToTable()

	-- UnityPlayerPrefs.SetInt(PrefKeys.UpdateRecheck,0)

	for i,v in ipairs(bundles) do
		if NeedUpdate(v) then
			if not nirvanaAssetMgr.IsVersionCached(v) then
				if self.need_restart == false then
					if NeedRestart(v) then
						self.need_restart = true
						if v == "lua/preload" then
							self.need_recheck = true
							UnityPlayerPrefs.SetInt(PrefKeys.UpdateRecheck,1)
						end
					elseif self.need_resetup_lua == false and string.match(v,"^lua/.*") then		--有lua更新，更新完成之后需要手动重载一下lua列表
						self.need_resetup_lua = true
					end
				end
				table.insert(update_bundles,v)
			end
		end
	end
	if #update_bundles > 0 then
		local total_size = 0
		for i,v in ipairs(update_bundles) do
			total_size = total_size + nirvanaAssetMgr.GetBundleSize(v) / 1024
		end
		PreloadData.Instance:SetUpdateTotalBytes(total_size)
		self.update_bundles = update_bundles
		MainProber:Step(MainProber.SETP_ASSET_BEGIN_DL,"BundleCount_" .. tostring(#update_bundles))
		self:updateBundles(update_bundles,1)
		PreloadData.Instance:SetState(PreloadData.State.Update)
	else
		MainProber:Step(MainProber.SETP_ASSET_BEGIN_DL,"BundleCount_0")
		self:updateBundles(update_bundles,1)
	end
end

function GameStateUpdate:updateBundles(update_bundles,index)
	if index > #update_bundles then

		local version = nirvanaAssetMgr.CalculateVersion()
		print("Save version: " .. GLOBAL_CONFIG:RemotePkgInfo().assets_version .. "---" .. version)
		nirvanaAssetMgr.SaveVersion(version)
		UnityPlayerPrefs.SetInt(PrefKeys.RecheckPhasePkgs,1)	--标记阶段资源包需要重新check下载
		local version_changed = false
		if GLOBAL_CONFIG:PkgInfo().assets_version ~= version then
			version_changed = true
			GLOBAL_CONFIG:PkgInfo().assets_version = version
		end
		if self.need_restart then
			if not self.need_recheck then
				UnityPlayerPrefs.SetInt(PrefKeys.UpdateRecheck,0)
			end
			-- UnityEngine.Debug.LogError("UpdateComplete And ReBOOT!!!")
			RebootGame()
		else --更新完成但不需要重启游戏，只需清理一下资源即可继续状态机
			if not self.need_recheck then
				UnityPlayerPrefs.SetInt(PrefKeys.UpdateRecheck,0)
			end
			if version_changed then		--优化开启速度，更新的版本号与本地版本号一致的话不需要清理资源
				if VersionTag.HasTag(VersionTag.AssmgrCleanWithoutFI) then
					nirvanaAssetMgr.ReleaseAllBundlesWithoutFI()	--新接口，BundleFileInfo已在LoadRemote阶段装载，不需要重新清理装载
				else
					nirvanaAssetMgr.ReleaseAllBundles()
					nirvanaAssetMgr.LoadLocalBundleFileInfo()	--此接口性能较差，在mi9上需要花1-1.5秒
				end
			end
			if self.need_resetup_lua then
				-- UnityEngine.Debug.LogError("UpdateComplete Dont NEED ReBOOT!!! RESET BUNDLE MAP")
				Game.LuaManager.Singleton():SetupBundleMap();
			end
			
			-- UnityEngine.Debug.LogError("UpdateComplete Dont NEED ReBOOT!!!")
			self:onUpdateComplete()
		end
		return
	end

	if PreloadData.Instance:GetUpdateError() == true then return end

	local on_update_com = function(err)
		local next_index = index
		if err == nil then
			self.downloaded_files_size = PreloadData.Instance:GetUpdatedBytes()
			next_index = next_index + 1
			self:ClearErrorCode()
			if self.used_cmd_list == CDMType.NoCDM then
				self:SwitchUrl()
			end
		else
		self.open_download_count = true
		end
		self.update_bundles_index = next_index
		self:updateBundles(update_bundles,next_index)
	end

	local bundle = update_bundles[index]
	if not nirvanaAssetMgr.IsVersionCached(bundle) then	--再判断一遍,防止这个文件已经被之前的调用所cache了防止重复下载
		-- print("UpdateBundle: " .. tostring(bundle))
		self.current_download = bundle
		nirvanaAssetMgr.UpdateBundle(bundle,
			function (progress,speed,byes,length)
				PreloadData.Instance:SetUpdatedBytes(self.downloaded_files_size + byes / 1024)
				PreloadData.Instance:SetUpdateSpeed(speed / 1024)
				-- self.download_speed	= speed / 1024
				-- self.download_bytes = byes / 1024
				-- self.content_bytes = length / 1024
			end,
			on_update_com
			)
	else
		on_update_com(nil)
	end

end

function GameStateUpdate:DownLoadFailure()
	self:SwitchUrl()
	self.downFailure_Time = self.downFailure_Time + 1
	if self.downFailure_Time >= self.downFailure_TotalTime then
		PreloadData.Instance:SetUpdateError(true)
		self.downFailure_Time = 0
	end
end

function GameStateUpdate:SwitchUrl()
	self.used_cmd_list = self.used_cmd_list + 1 > #self.URLList and CDMType.CDM or CDMType.NoCDM
	local Url = self.URLList[self.used_cmd_list]
	local url_list = Url.url_list
	Url.used_url_index = Url.used_url_index + 1 > #url_list and 1 or Url.used_url_index + 1
	local downloadUrl = url_list[Url.used_url_index]
	nirvanaAssetMgr.DownloadingURL = downloadUrl
end

function GameStateUpdate:Update()

	if PreloadData.Instance:GetUpdateError() == false then
		if UnityEngine.NetworkReachability ~= nil and UnityEngine.Application.internetReachability == UnityEngine.NetworkReachability.NotReachable then
			PreloadData.Instance:SetUpdateError(true)
			return
		elseif self.open_download_count then
			self.error_timer = self.error_timer + Time.deltaTime
			if self.error_timer >= self.error_total_timer then
				self.error_timer = 0
				self:DownLoadFailure()
			end
		end
		if self.update_asset_event ~= nil then
			self.update_asset_event()
			self.update_asset_event = nil
		end
	else	--异常了
		return
	end

	if self.prepared == true then
		return
	end

	if self.remote_manifest_state == State.Failed then
		self:LoadRemoteManifest()
		self.open_download_count = true
	end

	if self.remote_file_info_state == State.Failed then
		self:LoadRemoteBundleFileInfo()
		self.open_download_count = true
	end

	if self.remote_manifest_state == State.Complete and
		self.remote_file_info_state == State.Complete then
			self:ClearErrorCode()
			if PreloadViewStateMachine.Instance:CurrentStateType() ~= PreloadViewStateHealth or 
				PreloadData.Instance:GetProgSpeed() == PreloadData.ProgSpeed.Fast then	--OnPrepared比较花时间，防止在健康界面渐变时界面卡住
				self.prepared = true
				self:ClearErrorCode()
				self:OnPrepared()
			end
	end

end

function GameStateUpdate:ClearErrorCode()
	PreloadData.Instance:SetUpdateError(false)
	self.open_download_count = false
	self.error_timer = 0
	self.downFailure_Time = 0
end

function GameStateUpdate:Stop()
	MainProber:Step(MainProber.STEP_ASSET_UPDATE_END)
	GameState.Stop(self)
	nirvanaAssetMgr.IgnoreHashCheck = false;
end

function GameStateUpdate:onUpdateComplete()
	self:ClearErrorCode()
	self.update_bundles = nil
	self.update_bundles_index = 1
	self:Stop()
end

function GameStateUpdate:RetryDownload()
	PreloadData.Instance:SetUpdateError(false)
	self.error_timer = 0
	self.downFailure_Time = 0
	if self.prepared and self.update_bundles then
		self.update_asset_event = function ()
			self:updateBundles(self.update_bundles,self.update_bundles_index)
		end
	end
end

return GameStateUpdate