SceneManager = SceneManager or BaseClass()

local gameSceneMgrClass = Game.SceneManager
local gameSceneMgr = gameSceneMgrClass.Singleton()
local release_unusedassets_time = 150
local release_unusedassets_without_load = 600

local clear_scene_count = 8
if UnityEngine.SystemInfo.systemMemorySize <= 2500 then
	release_unusedassets_time = 75
	release_unusedassets_without_load = 300
	clear_scene_count = 6
end

SceneManager.DefaultResamplePrecision = 1
SceneManager.MaxResamplePer = 5
function SceneManager:__init()
	if SceneMgr ~= nil then
		Debuger.LogError("[SceneManager] attempt to create singleton twice!")
		return
	end
	SceneMgr = self


	self.data = SmartData.New({
		loaded_scene_id = 0,
		})

	self.loading_scene_name = nil
	self.cur_scene_name = nil
	self.loading_com_handle = nil
	self.last_release = 0
	gameSceneMgrClass.OnLevelLoaded = BindTool.Bind(self.OnLevelLoaded, self)
	gameSceneMgrClass.OnLevelUnloaded = BindTool.Bind(self.OnLevelUnloaded, self)
	self.loaded_levels = {}
	self.quality_level = nil
	Game.TerrainCollider.ResamplePrecision = SceneManager.DefaultResamplePrecision
	self.main_camera = nil

	Runner.Instance:AddRunObj(self)
	self.uncleared_scene = 0
	self.free_cam_objs = nil --key:SceneManager.FreeLookCamType,value:GameObject
end


function SceneManager:__delete()
	self.main_camera = nil
	Runner.Instance:RemoveRunObj(self)
	if SceneMgr == self then
		SceneMgr = nil
	end
end

function SceneManager:MainCamera()
	if self.main_camera == nil then
		local cam_obj = UnityGameObj.FindGameObjectWithTag("MainCamera")
		if cam_obj ~= nil then
			self.main_camera = cam_obj:GetComponent(typeof(UnityEngine.Camera))
		end
	end
	return self.main_camera
end

function SceneManager:OnReboot()
	gameSceneMgrClass.OnLevelLoaded = nil
	gameSceneMgrClass.OnLevelUnloaded = nil
end

local function sceneNameToBundleName(scene_name)

	return string.format("levels/%s", string.lower(scene_name));
end

function SceneManager:ResetReleaseData()
	self.last_release = Time.realtimeSinceStartup
	self.uncleared_scene = 0
end

function SceneManager:LoadScene(scene_name,load_com,scene_id)
	self.uncleared_scene = self.uncleared_scene + 1
	if Time.realtimeSinceStartup - self.last_release >= release_unusedassets_time or self.uncleared_scene >= clear_scene_count then
		-- LogError("UnloadUnusedAssets==",self.last_release,Time.realtimeSinceStartup)
		self:ResetReleaseData()
		UnityEngine.Resources.UnloadUnusedAssets();
	end
	if scene_id == nil then
		scene_id = 0
	end
	--是否已经加载了scene_name
	-- Debuger.LogErrorSD("self.loading_scene_name===%s,self.cur_scene_name===%s,scene_name===%s"
	-- 	,tostring(self.loading_scene_name),tostring(self.cur_scene_name),tostring(scene_name))
	if self.loading_scene_name == nil and self.cur_scene_name == scene_name then
		self.data.loaded_scene_id = scene_id
		load_com(nil)
		return
	end
	self.data.loaded_scene_id = 0
	self.loading_scene_name = scene_name
	self.cur_scene_name = scene_name

	local scene_data = gameSceneMgr:GetSceneResData(scene_name)
	local need_update_bundles = {}
	local main_bundle_name = sceneNameToBundleName(scene_data.main)
	local need_update = false
	-- LogError("CHECK===" , main_bundle_name)
	if not BundleProviderCtrl.IsBundleCached(main_bundle_name) then
		need_update_bundles[main_bundle_name] = BundleProviderCtrl.CreateUpdateProgress()
		need_update = true
	end
	for i=0,scene_data.important.Length-1 do
		local bn = sceneNameToBundleName(scene_data.important[i])
		-- LogError("CHECK===" , bn)
		if not BundleProviderCtrl.IsBundleCached(bn) then
			need_update_bundles[bn] = BundleProviderCtrl.CreateUpdateProgress()
			need_update = true
		end
	end

	self.loading_com_handle = load_com
	if need_update then
		self:updateSceneRes(need_update_bundles,function()
			if self.loading_scene_name == scene_name and self.loading_com_handle == load_com then
				self:loadSceneInternal(scene_name,scene_id,load_com)
			end
		end)
	else
		self:loadSceneInternal(scene_name,scene_id,load_com)
	end

	-- gameSceneMgr:LoadScene(scene_name,function(err)
	-- 		if self.loading_scene_name == scene_name then
	-- 			self.loading_scene_name = nil
	-- 			self.data.loaded_scene_id = scene_id
	-- 		end
	-- 		load_com(err)
	-- 	end
	-- 	)
end

function SceneManager:Update()
	if Time.realtimeSinceStartup - self.last_release >= release_unusedassets_without_load then
		self:ResetReleaseData()
		UnityEngine.Resources.UnloadUnusedAssets();
	end
end

function SceneManager:updateSceneRes(bundle_list,on_complete)
	local total_count = 0
	for bn,prog in pairs(bundle_list) do
		BundleProviderCtrl.Instance:UpdateBundle(bn,function() end,prog)
		total_count = total_count + 1
	end
	local desc_str = Language.Common.SceneDownloadLoadingDesc
	local frame_count = {count = 0}
	LoadingCtrl.Instance:ManualLoading(function ()
		local total_per = 0
		local total_size = 0
		local downloaded_size = 0
		for _,prog in pairs(bundle_list) do
			total_per = total_per + (prog.downloaded_size / prog.total_size)
			total_size = total_size + prog.total_size
			downloaded_size = downloaded_size + prog.downloaded_size
		end
		local per = total_per/total_count
		if per >= 1 then
			if frame_count.count < 10 then		--下载完成之后延迟几帧防止文件还没有被正常写入
				per = 0.99
				frame_count.count = frame_count.count + 1
				-- LogError("FraeCount===",frame_count.count)
			else
				on_complete()
			end
		end
		return per,string.format(desc_str,downloaded_size/1024,total_size/1024)
	end)
	-- local curT = Time.time
	-- LoadingCtrl.Instance:AutoPercent(function()
	-- 	if Time.time - curT >= 5 then
	-- 		on_complete()
	-- 		return true
	-- 	else
	-- 		return false
	-- 	end
	-- end,"TEST")
end


function SceneManager:loadSceneInternal(sn,scene_id,on_com)
	UnityEngine.Application.backgroundLoadingPriority = 4
	gameSceneMgr:LoadScene(sn,function(err)
		UnityEngine.Application.backgroundLoadingPriority = 0
		if self.loading_scene_name == sn and self.loading_com_handle == on_com then
			self.loading_scene_name = nil
			self.data.loaded_scene_id = scene_id
			self.loading_com_handle = nil
			on_com(err)
		end
	end
	)
end


function SceneManager.FindGameObjectWithTag(tag)
	return gameSceneMgrClass.FindGameObjectWithTag(tag)
end

function SceneManager:LoadedSceneId()
	return self.data.loaded_scene_id
end

function SceneManager:CareLoadedSceneIdChange(func)
	return self.data:Care(func,"loaded_scene_id")
end

function SceneManager:UncareLoadedSceneIdChange(handle)
	self.data:Uncare(handle)
end

local function applyLevelQuality(level,quality)
	if quality == QUALITY_LEVEL.LOW then
		gameSceneMgrClass.SetLevelTagRendererEnabled(level, QUALITY_SCENE_BLOCK_TAG.MIDDLE, false)
		gameSceneMgrClass.SetLevelTagRendererEnabled(level, QUALITY_SCENE_BLOCK_TAG.HIGH, false)
	elseif quality == QUALITY_LEVEL.MEDIUM then
		gameSceneMgrClass.SetLevelTagRendererEnabled(level, QUALITY_SCENE_BLOCK_TAG.MIDDLE, true)
		gameSceneMgrClass.SetLevelTagRendererEnabled(level, QUALITY_SCENE_BLOCK_TAG.HIGH, false)
	else
		gameSceneMgrClass.SetLevelTagRendererEnabled(level, QUALITY_SCENE_BLOCK_TAG.MIDDLE, true)
		gameSceneMgrClass.SetLevelTagRendererEnabled(level, QUALITY_SCENE_BLOCK_TAG.HIGH, true)
	end
end

function SceneManager:OnLevelLoaded(level)
	
	table.insert(self.loaded_levels,level)
	applyLevelQuality(level,self.quality_level)
end

function SceneManager:OnLevelUnloaded(level)
	for i,v in ipairs(self.loaded_levels) do
		if v.handle == level.handle then
			table.remove(self.loaded_levels,i)
			return
		end
	end
end

function SceneManager:SetQualityLevel(quality)
	if self.quality_level == quality then
		return
	end
	self.quality_level = quality
	for _,v in pairs(self.loaded_levels) do
		applyLevelQuality(v,self.quality_level)
	end
end

SceneManager.FreeLookCamType = {
	FreeLookCamera = "FreeLookCamera",
	FreeLookCamera2 = "FreeLookCamera2",
	FishSceneCamera = "FishSceneCamera",
}


function SceneManager:GetFreeLookCamera(type)
	if self.free_cam_objs == nil then
		self.free_cam_objs = {}
		local cam_objs = UnityGameObj.FindGameObjectsWithTag("FreeLookCamera")
		local obj_len = cam_objs.Length
		for i=0,obj_len - 1 do
			local cam_obj = cam_objs[i]
			self.free_cam_objs[cam_obj.name] = cam_obj
		end
	end
	local cam_obj = self.free_cam_objs[type]
	return cam_obj
end