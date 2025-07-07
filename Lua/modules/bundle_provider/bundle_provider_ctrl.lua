VIEW_REQ("modules/bundle_provider/quietly_download_view")

BundleProviderCtrl = BundleProviderCtrl or BaseClass(BaseCtrl)

local nirvanaAssetMgr = Nirvana.AssetManager

local QuietlyCacheState =
{
	Stoped = 0,
	Running = 1,
	Paused = 2,
}

local CDMType =
{
	CDM = 1,
	NoCDM = 2,
}

--bundleList的下载状态，大于等于0表示已下载的索引值
local QuietlyBundleListState = {
	WaitTrigger = -1,
	Complete = -2,
}

UnityNetReachablility = {
	NotReachable = "NotReachable",		--没网
	CarrierDataNetwork = "ReachableViaCarrierDataNetwork",	--4G
	LocalAreaNetwork = "ReachableViaLocalAreaNetwork", 	--WIFI
}


function BundleProviderCtrl.CreateUpdateProgress()
	return {
				total_percent = 0,		--总进度 0-1
				download_speed = 0,		--当前下载速度speed	KB
				total_size = 1,			--下载资源总尺寸 KB
				downloaded_size = 0,	--已下载尺寸	KB
				downloaded_files_size = 0,		--记录所有已经下载完成的文件的总尺寸
				unrecord_size_bundles = {},	--key:bundleNane,value:bool 记录哪些bundle在开始下载时就已经缓存了不记录到下载尺寸数据里
			}
end

local function createQuietyBundle()
	return
	{
	error_timer = 0,
	error_total_timer = 20,
	downFailure_timer = 0,
	downFailure_total_time = 3,
	used_cmd_index = CDMType.CDM,
	Url_list = {},
	}
end

local function createUrlData()
	return
	{
		url_list = {},
		used_url_index = 1,
	}
end

local reward_pkg_key = "Lv0_38"

local small_pkg_abph_key = "Lv0_20"

function BundleProviderCtrl:__init()
	if BundleProviderCtrl.Instance ~= nil then
		Debuger.LogError("[BundleProviderCtrl] Attemp to create a singleton twice !")
	end
	BundleProviderCtrl.Instance = self
	self.used_bundle_list_dic = {}

	self.quietly_cache_state = QuietlyCacheState.Stoped
	self.quietly_cache_pause_count = 0  --大于0表示暂停
	self.quietly_downding_cfg = nil

	local manifest = nirvanaAssetMgr.Manifest
	if manifest ~= nil then
		local used_bundle_list = manifest:GetAllAssetBundles():ToTable()
		for i,v  in ipairs(used_bundle_list) do
			self.used_bundle_list_dic[v] = true
		end
	end
	self:InitErrorSetting()

	self.quietly_bundles_cfg = ABPhasePkg
	local needRecheck = BundleProviderSimulate.RE_CHECK or 
		(UnityPlayerPrefs.GetInt(PrefKeys.RecheckPhasePkgs,1) == 1)
	if needRecheck then
		UnityPlayerPrefs.SetInt(PrefKeys.RecheckPhasePkgs,0)
	end
	if not GLOBAL_CONFIG:PkgInfo().is_small_pkg then
		self.quietly_bundles_cfg[small_pkg_abph_key] = nil
	end

	for k,v in pairs(self.quietly_bundles_cfg) do
		v.key = k
		
		-- LogError("key==",k,",count==",#v,"[1]=",v[1])
		local pref_key = PrefKeys.PhasePkgCachedI(v.key)
		if needRecheck then
			UnityPlayerPrefs.SetInt(pref_key,0)
		end
		local cachedI = UnityPlayerPrefs.GetInt(pref_key,0)
		--cacheI大于总需要下载数的话就标记这个包已经下载完成
		v.state = (cachedI < #v) and QuietlyBundleListState.WaitTrigger or QuietlyBundleListState.Complete
	end

	self.view_data = SmartData.New({
		reward_getted_flag = 0,		--0表示未领取
		reward_getable_flashed = false,	--是否可领奖的数据刷新
	})
	self:RegisterProtocol(CSFetchRecourceDownloadReward)
    self:RegisterProtocol(SCFetchRecourceDownloadReward, "RecvSCFetchRecourceDownloadReward")

	self.last_net_flag = UnityNetReachablility.LocalAreaNetwork
	self.check_4g_timer = nil
end

function BundleProviderCtrl:__delete()

end

function BundleProviderCtrl:OnUnloadGameLogic()
	self.last_net_flag = UnityNetReachablility.LocalAreaNetwork	--登出时设置网络为wifi，重登后保持会提醒4G下载
end

function BundleProviderCtrl:InitErrorSetting()
	self.record_list = createQuietyBundle()
	local url1 = GLOBAL_CONFIG:Urls().update_url
	local url2 = GLOBAL_CONFIG:Urls().update_url2

	if url1 ~= nil and url1 ~= "" then
		local UrlTable = createUrlData()
		table.insert(UrlTable.url_list,url1)
		self.record_list.Url_list[CDMType.CDM] = UrlTable
	end

	if url2 ~= nil and url2 ~= "" then
		local UrlTable = createUrlData()
		table.insert(UrlTable.url_list,url2)
		self.record_list.Url_list[CDMType.NoCDM] = UrlTable
	end

end

local function run_provider()	--是否执行偷偷下载
	return (not SIMULATE_ASSETBUNDLE and not IS_AUDIT_VERSION --and not IS_MICRO_PLAYER()
		and HAS_RESCACHE()) or BundleProviderSimulate.OPEN
end


function BundleProviderCtrl:OnInit()
	if run_provider() then
		RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.CheckToQuitlyCache,self),"level")
		self:CheckToQuitlyCache()
	end
end
function BundleProviderCtrl:CheckToQuitlyCache1()
end
function BundleProviderCtrl:CheckToQuitlyCache()
	if self.quietly_cache_state ~= QuietlyCacheState.Stoped then
		return
	end
	local download_bundle_cfg = nil
	--判断应该下载哪个bundle
	for k,v in pairs(self.quietly_bundles_cfg) do
		if v.state ~= QuietlyBundleListState.Complete and 
			(download_bundle_cfg == nil or v.priority < download_bundle_cfg.priority) then
			if k == "Lv38_70" then
				if RoleData.Instance:GetRoleLevel() >= 38 then
					download_bundle_cfg = v
				end
			elseif k == "Lv70_120" then
				if RoleData.Instance:GetRoleLevel() >= 68 then
					download_bundle_cfg = v
				end
			elseif RoleData.Instance:GetRoleLevel() >= 30 then
					download_bundle_cfg = v
			end
		end
	end
	--没有包要下载
	if download_bundle_cfg == nil then
		return
	end
	self.quietly_cache_state = QuietlyCacheState.Running
	self:Begin4GCheck()
	download_bundle_cfg.state = UnityPlayerPrefs.GetInt(PrefKeys.PhasePkgCachedI(download_bundle_cfg.key),0)
	-- LogError("Start Download Pkgs",download_bundle_cfg.key,download_bundle_cfg.state)
	self.quietly_downding_cfg = download_bundle_cfg
	if self.quietly_downding_cfg.prog == nil then
		self.quietly_downding_cfg.prog = BundleProviderCtrl.CreateUpdateProgress()
	end
	MainViewData.Instance:QuietlyDownloadStateChange()
	self:updateBundles(download_bundle_cfg,download_bundle_cfg.state + 1,true,
		function() 
			self.quietly_downding_cfg = nil
			MainViewData.Instance:QuietlyDownloadStateChange()
			-- LogError("Complete Download Pkgs",download_bundle_cfg.key)
			self.quietly_cache_state = QuietlyCacheState.Stoped
			self:End4GCheck();
			download_bundle_cfg.state = QuietlyBundleListState.Complete
			if download_bundle_cfg.key == reward_pkg_key then
				self:MarkRewardGettableFlash()
			end
			UnityPlayerPrefs.SetInt(PrefKeys.PhasePkgCachedI(download_bundle_cfg.key),#download_bundle_cfg) --已完成存档
			self:CheckToQuitlyCache()
		end,download_bundle_cfg.prog,true)
end

--检测4G网络的计时器
function BundleProviderCtrl:Begin4GCheck()
	if self.check_4g_timer ~= nil then
		return
	end
	self.check_4g_timer = 
		TimeHelper:AddRunTimer(function()	
				--没登录的时候不提示
			if LoginData.Instance:LoginState() ~= LoginStateType.Logined then
				return
			end
			if self:IsStoped() then		--如果不在下载则不提示
				return
			end
			local net_flag = self:InternetReachability()
			if net_flag == self.last_net_flag then
				return
			end
			if self:IsRunning() and net_flag == UnityNetReachablility.CarrierDataNetwork then
				local param_t = {
					content = Language.QuietlyDownload.Tips4GCheck ,
					confirm = {
						name = Language.QuietlyDownload.Tips4GCheckComform,
						func =  function()
							ViewMgr:CloseView(DialogTipsView)
						end,
					},
					cancel = {
						name = Language.QuietlyDownload.Tips4GCheckCancel,
						func = function()
							ViewMgr:CloseView(DialogTipsView)
							self:pauseQuietlyCache();
						end
					},
					tnr = DTTodayNotRemind.DownloadNetChek
					}
				PublicPopupCtrl.Instance:DialogTips(param_t)
			elseif self:IsPaused() and net_flag == UnityNetReachablility.LocalAreaNetwork then	--切回wifi的时候再偷偷打开
				self:resumeQuietlyCache();
			end
			self.last_net_flag  = net_flag
		end,5,-1,true)
end

function BundleProviderCtrl:End4GCheck()
	if self.check_4g_timer == nil then
		return
	end
	TimeHelper:CancelTimer(self.check_4g_timer)
	self.check_4g_timer = nil
end

function BundleProviderCtrl:QuitelyDownloadingProg()
	return self.quietly_downding_cfg and self.quietly_downding_cfg.prog or nil
end

local cached_bundles = {}
function BundleProviderCtrl.IsBundleCached(bundle_name)
	local cache_val = cached_bundles[bundle_name]
	if cache_val then
		return cache_val
	end
 	local bundle_info = nirvanaAssetMgr.GetBundleInfo(bundle_name)
	cache_val = bundle_info.IsCached
	if cache_val then
		cached_bundles[bundle_name] = cache_val
	end
	return cache_val
end

function BundleProviderCtrl:UpdateBundle(bundle_name,on_complete,pro_holder)

 	local bundle_info = nirvanaAssetMgr.GetBundleInfo(bundle_name)

 	if bundle_info.IsCached then
 		if pro_holder then pro_holder.total_percent = 1 end
 		on_complete()
 	else
 		local need_update_bundles = {}
 		local un_cached_bundles_array = bundle_info.UnCachedBundles
 		local downloading_bundles_array = bundle_info.DownloadingBundles
 		if downloading_bundles_array ~= nil then
	 		for i = 0,downloading_bundles_array.Length - 1 do
				table.insert(need_update_bundles,downloading_bundles_array[i])
			end
		end
		if un_cached_bundles_array ~= nil then
	 		for i = 0,un_cached_bundles_array.Length - 1 do
				table.insert(need_update_bundles,un_cached_bundles_array[i])
			end
		end
		local need_pause_quitely = self.quietly_cache_state == QuietlyCacheState.Running
		if need_pause_quitely then
			self:pauseQuietlyCache()
		end
		self:updateBundles(need_update_bundles,1,false,
			function()
				if need_pause_quitely then
					self:resumeQuietlyCache()
				end
				on_complete()
			end,
			pro_holder,true)
 	end

 	-- return pro_holder
end


function BundleProviderCtrl:pauseQuietlyCache()
	-- self.quietly_cache_pause_count = self.quietly_cache_pause_count + 1
	if --self.quietly_cache_pause_count > 0 and 
		self.quietly_cache_state == QuietlyCacheState.Running then
		self.quietly_cache_state = QuietlyCacheState.Paused
	end
end

function BundleProviderCtrl:resumeQuietlyCache()
	-- self.quietly_cache_pause_count = self.quietly_cache_pause_count - 1
	if --self.quietly_cache_pause_count <= 0 and 
		self.quietly_cache_state == QuietlyCacheState.Paused then
		self.quietly_cache_state = QuietlyCacheState.Running
	end
end



function BundleProviderCtrl:needUpdate(bundle_name)
	if BundleProviderSimulate.OPEN then
		return not BundleProviderSimulate.IsVersionCached(bundle_name)
	else		
		local bundle_used = (self.used_bundle_list_dic[bundle_name] ~= nil)
		if not bundle_used then
			return false
		end
		return not ResourceManager.HasBundle(bundle_name)
	end
end


function BundleProviderCtrl.getBundleSize(bundle_name)
	if BundleProviderSimulate.OPEN then
		return BundleProviderSimulate.GetBundleSize(bundle_name)
	else
		return nirvanaAssetMgr.GetBundleSize(bundle_name)
	end
end

local update_err_hash_invalid = "Bundle hash is invalid"

function BundleProviderCtrl:updateBundles(bundle_names,index,quietly,complete_callback,progress_recorder,init_prog)
	if index > #bundle_names then
		complete_callback()
		return
	end

		if progress_recorder ~= nil and init_prog then 	--计算总尺寸
			progress_recorder.total_size = 0
			for i = index,#bundle_names do
				local v = bundle_names[i]
				if self:needUpdate(v) then
					local b_size = BundleProviderCtrl.getBundleSize(v) / 1024
					progress_recorder.total_size = progress_recorder.total_size + b_size
					if i < index then
						progress_recorder.downloaded_size = progress_recorder.downloaded_size + b_size
						progress_recorder.downloaded_files_size = progress_recorder.downloaded_files_size + b_size
						if progress_recorder.total_size > 0 then
							progress_recorder.total_percent = progress_recorder.downloaded_size / progress_recorder.total_size
						else
							progress_recorder.total_percent = (index-1) / #bundle_names
						end
					end
				else
					progress_recorder.unrecord_size_bundles[v] = true
				end
			end
			if progress_recorder.total_size == 0 then	--所有资源都已经缓存，直接完成
				complete_callback()
				return
			end
		end

	local bundle = bundle_names[index]
	local on_update_com = 
		function(err)
			local next_index = index
			if StringIsEmpty(err) or err == update_err_hash_invalid then   --hash not find in manifest,not need update
				if progress_recorder ~= nil then
					progress_recorder.downloaded_files_size = progress_recorder.downloaded_size
				end
				while true do
					next_index = next_index + 1
					local next_update_bundle_name = bundle_names[next_index]
					if next_update_bundle_name == nil or progress_recorder.unrecord_size_bundles[next_update_bundle_name] ~= true then
						break
					end
				end
				-- if quietly and (next_index - self.record_i) >= START_I_WRITE_STEP then
				-- 	UnityEngine.PlayerPrefs.SetInt(PrefKeys.QulityBundleStartI,next_index)
				-- 	self.record_i = next_index
				-- end
				if quietly and (index%10 == 0) then
					UnityPlayerPrefs.SetInt(PrefKeys.PhasePkgCachedI(bundle_names.key),index)--记录当前Index
				end
				if self.record_list.used_cmd_index == CDMType.NoCDM then
					self:SwitchUrl()
					self:ClearErrorOperation()
				end
			else
				if quietly then
					self:FailureOperation()
				end
			end

			local exe_update =
				function () 
					TimeHelper:AddDelayFrameTimer( 
						function ()
							--BeginSample("on_update_com")
							self:updateBundles(bundle_names,next_index,quietly,complete_callback,progress_recorder)
							--EndSample()
						end,
					0)
				end
			if quietly == true and self.quietly_cache_state ~= QuietlyCacheState.Running then
				if self.quietly_cache_state == QuietlyCacheState.Paused then
					Runner.Instance:RunUntilTrue(
						function ()
							if self.quietly_cache_state == QuietlyCacheState.Running then
								exe_update()
							end
							return self.quietly_cache_state ~= QuietlyCacheState.Paused
						end
						)
				end
			else
				exe_update()
			end
		end

		-- LogError("try Update====",bundle)
	if self:needUpdate(bundle) then
		-- LogError("@@@@@@try Update====",bundle)
		local update_func = function (progress,speed,byes,length)
			if progress_recorder ~= nil and progress_recorder.unrecord_size_bundles[bundle] ~= true then
				progress_recorder.downloaded_size = progress_recorder.downloaded_files_size + byes/1024
				if progress_recorder.total_size > 0 then
					progress_recorder.total_percent = progress_recorder.downloaded_size / progress_recorder.total_size
				else
					progress_recorder.total_percent = ((index-1) + progress) / #bundle_names
				end
				progress_recorder.download_speed = speed / 1024
			end
		end
		if BundleProviderSimulate.OPEN then
			BundleProviderSimulate.UpdateBundle(bundle,update_func,on_update_com)
		else
			nirvanaAssetMgr.UpdateBundle(bundle,update_func,on_update_com)
		end
	else
		if progress_recorder ~= nil and progress_recorder.unrecord_size_bundles[bundle] ~= true then
			progress_recorder.downloaded_size = progress_recorder.downloaded_files_size + BundleProviderCtrl.getBundleSize(bundle) / 1024
		end
		on_update_com()
	end
end

function BundleProviderCtrl:FailureOperation()
	
	self.record_list.error_timer = self.record_list.error_timer + Time.deltaTime
	if self.record_list.error_timer >= self.record_list.error_total_timer then
		self.record_list.error_timer = 0
		self.record_list.downFailure_timer = self.record_list.downFailure_timer + 1
	end
	if self.record_list.downFailure_timer >= self.record_list.downFailure_total_time then
		self:SwitchUrl()
		self:ClearErrorOperation()
	end

end

function BundleProviderCtrl:SwitchUrl()

	self.record_list.used_cmd_index = self.record_list.used_cmd_index + 1 > #self.record_list.Url_list and CDMType.CDM or CDMType.NoCDM
	local Url = self.record_list.Url_list[self.record_list.used_cmd_index]
	local url_list = Url.url_list
	Url.used_url_index = Url.used_url_index + 1 > #url_list and 1 or Url.used_url_index + 1
	local downloadUrl = url_list[Url.used_url_index]
	nirvanaAssetMgr.DownloadingURL = downloadUrl
end

function BundleProviderCtrl:ClearErrorOperation()
	self.record_list.error_timer = 0
	self.record_list.downFailure_timer = 0
end

function BundleProviderCtrl:IsRunning()
	return self.quietly_cache_state == QuietlyCacheState.Running
end

function BundleProviderCtrl:IsPaused()
	return self.quietly_cache_state == QuietlyCacheState.Paused
end

function BundleProviderCtrl:IsStoped()
	return self.quietly_cache_state == QuietlyCacheState.Stoped
end


-----------增量下载奖励--------------
function BundleProviderCtrl:RecvSCFetchRecourceDownloadReward(protocol)
	-- LogError("BundleProviderCtrl:RecvSCFetchRecourceDownloadReward====",protocol)
	self.view_data.reward_getted_flag = protocol.flag
	MainViewData.Instance:QuietlyDownloadStateChange()
end

function BundleProviderCtrl:ReqFetchResourceDownloadReward()
	local protocol = GetProtocol(CSFetchRecourceDownloadReward)
    SendProtocol(protocol)
end

function BundleProviderCtrl:ViewData()
	return self.view_data
end


function BundleProviderCtrl:HasRewardGetted()
	-- if run_provider() then
		return self.view_data.reward_getted_flag ~= 0
	-- else
	-- 	return false
	-- end
end

function BundleProviderCtrl:MarkRewardGettableFlash()
	MainViewData.Instance:QuietlyDownloadStateChange()
	self.view_data.reward_getable_flashed = not self.view_data.reward_getable_flashed
end

function BundleProviderCtrl:RewardGettable()
	if run_provider() then
		--0-38级的包下载完就可以领取奖励
		return self.quietly_bundles_cfg[reward_pkg_key].state == QuietlyBundleListState.Complete
	else
		--没有打开偷偷下载功能是，任何时候都能领
		return true
	end
end

--2 WIFI,1 4G,0 没网
function BundleProviderCtrl:InternetReachability()
	if BundleProviderSimulate.OPEN then
		return BundleProviderSimulate.InternetReachability()
	else
		return tostring(UnityApp.internetReachability)--UnityEngine.NetworkReachability.ReachableViaLocalAreaNetwork
	end
end


------------------下载模拟逻辑-----------------------

BundleProviderSimulate = BundleProviderSimulate or {
	OPEN = true,
	bundle_cache_flag = {},
	bundle_size = {},
}
if IS_EDITOR then
	BundleProviderSimulate.OPEN = false		--想要模拟下载改这里
else
	BundleProviderSimulate.OPEN = false
end

if BundleProviderSimulate.OPEN then
	BundleProviderSimulate.RE_CHECK = true	--想要每次进游戏都重新下载改这里(false的话就是每次都接着上次下载进度继续)
else
	BundleProviderSimulate.RE_CHECK = false
end

function BundleProviderSimulate.downloadSpeedScale()		--想要下的更快点改这里
	return 80000
end

--是否是wifi
-- UnityNetReachablility = {
-- 	NotReachable = 0,		--没网
-- 	CarrierDataNetwork = 1,	--4G
-- 	LocalAreaNetwork = 2, 	--WIFI
-- }
function BundleProviderSimulate.InternetReachability()
	local va = UnityNetReachablility.CarrierDataNetwork
	return va
end

function BundleProviderSimulate.IsVersionCached(bundle_name)
	if BundleProviderSimulate.bundle_cache_flag[bundle_name] == nil then
		local rnd_num = GameMath.Ramdon(100)
		BundleProviderSimulate.bundle_cache_flag[bundle_name] = (rnd_num > 100)
	end
	return BundleProviderSimulate.bundle_cache_flag[bundle_name]
end

function BundleProviderSimulate.GetBundleSize(bundle_name)
	if BundleProviderSimulate.bundle_size[bundle_name] == nil then
		BundleProviderSimulate.bundle_size[bundle_name] = GameMath.Ramdon(1024,1024*1024*10)
	end
	return BundleProviderSimulate.bundle_size[bundle_name]
end


function BundleProviderSimulate.downloadSpeed()
	return GameMath.Ramdon(1024*200, 1024 * 50)
end

function BundleProviderSimulate.UpdateBundle(bundle,update_func,com_func)
	local tot_size = BundleProviderSimulate.GetBundleSize(bundle)
	local cur_size = 0
	--(progress,speed,byes,length)
	Runner.Instance:RunUntilTrue(function(progress,speed,bytes,length) 
		local d_speed = BundleProviderSimulate.downloadSpeed()
		cur_size = cur_size + d_speed * Time.deltaTime * BundleProviderSimulate.downloadSpeedScale()
		if cur_size >= tot_size then
			cur_size = tot_size
		end
		local prog = cur_size / tot_size
		update_func(prog,d_speed,cur_size,tot_size)
		if prog >= 1 then
			BundleProviderSimulate.bundle_cache_flag[bundle] = true
			com_func()
			return true
		else
			return false
		end
	end)
end