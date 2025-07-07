GameStateUpdateLuaBundle = GameStateUpdateLuaBundle or BaseClass(GameState)


local need_update_bundles = nil

local LUABUNDLE_OPTIMIZED = false--常量，lua bundleName是否是按web标准来mark的，目前R线是false,Web线是true
local function get_need_update_bundles()
	if need_update_bundles ~= nil then
		return need_update_bundles
	end
	need_update_bundles = {
		"protocol",
		"scenes/scene_cfg0",
		"scenes/scene_cfg1",
		"managers",
		"config",
	}
	local auto_ignore = {
		-- ["a"] = true,
	}
	local mod_ignore = {
		["x"] = true,
	}
	local ascaii_a = 97--a的ascii码是97
	for i=ascaii_a,ascaii_a + 25 do
		local ascaii_ch = string.char(i)
		if auto_ignore[ascaii_ch] ~= true then
			local auto_name = "auto_" .. ascaii_ch
			table.insert(need_update_bundles,auto_name)
			-- UnityEngine.Debug.LogError("AddCFGName===" .. auto_name)
		end
		if mod_ignore[ascaii_ch] ~= true then
			local mod_name = "modules_" .. ascaii_ch
			table.insert(need_update_bundles,mod_name)
		end
	end

	for i=1,#need_update_bundles do
		need_update_bundles[i] = string.format("lua/%s",need_update_bundles[i])
	end	
	return need_update_bundles
end

local function get_need_update_bundles_app()
	local list = Game.LuaManager.Singleton().LuaBundleList
	if list ~= nil then
		return list:ToTable()
	else
		return nil
	end
end


function GameStateUpdateLuaBundle:__init()
	-- self.cur_st = GameStateLuaRequire.SubState.None
	-- self.timer = 0


end

function GameStateUpdateLuaBundle:__delete()

end

local begin_update = false

function GameStateUpdateLuaBundle:Update()

	-- UnityEngine.Debug.LogError("FC==" .. tostring(Time.frameCount))
	-- if begin_update == false and Time.frameCount >= 300 then
	-- 	begin_update = true
	-- 	self:beginUpdate()
	-- end
	if self.wait_for_begin == true and PreloadViewStateLoad.preload_obj ~= nil then
		self.wait_for_begin = false
		self:beginUpdate()
	end
	PreloadData.Instance:SetUpdateLuabundlePer(self.completed_count/self.total_count)

end

function GameStateUpdateLuaBundle:Start()
	print("GameStateUpdateLuaBundle:Start")
	GameState.Start(self)

	if not IS_MICRO_PLAYER() then
		self:Stop()
		return
	end
	if SIMULATE_ASSETBUNDLE then
		self:Stop()
		return
	end
	if not VersionTag.HasTag(VersionTag.ManifestFileBundleInfoHash) then
		self:Stop()
		return
	end
	self.wait_for_begin = false
	PreloadData.Instance:SetState(PreloadData.State.UpdateLuaBundle)
	--update.prefab装载完成前不进行后面的下载流程，全力保证update.prefab能流畅装载
	if PreloadViewStateLoad.preload_obj ~= nil then
		self:beginUpdate()
	else
		self.completed_count = 0
		self.total_count = 1
		self.wait_for_begin = true
	end

	-- MainProber:Step(MainProber.STEP_LOAD_CONFIG_BEG)

	
	
end

-- local random_seed_num = -2147483648

-- local function SetRandomSeed()
--     random_seed_num = random_seed_num + 1
--     math.randomseed(tostring(os.time() + random_seed_num):reverse():sub(1, 7))
-- end

-- --无参数 :[0-1]的浮点
-- --只有参数n:[1-n]的整数
-- --m和n都有:[n-m]的整数
-- local function Ramdon(n, m)
--     SetRandomSeed()
--     if n == nil then
--     	return  math.random()
--     elseif m == nil then
--     	return  math.random(n)
--    	else
-- 	    return math.random(n, m)
--    	end
-- end

function GameStateUpdateLuaBundle:beginUpdate()
	local bundles = nil
	if LUABUNDLE_OPTIMIZED then
		bundles = get_need_update_bundles()
	else
		bundles = get_need_update_bundles_app()
	end
	if bundles == nil or #bundles == 0 then
		self:Stop()
		return
	end
	self.total_count = #bundles
	self.completed_count = 0
	for _,v in pairs(bundles) do
		Nirvana.AssetManager.LoadBundle(v,function(err,ab)
			self:onLuaBundleLoadComplete(err,ab,v)
		end)
	end
end

function GameStateUpdateLuaBundle:onLuaBundleLoadComplete(err,ab,bundle_name)
	-- local rnd_num = Ramdon(0,100)
	-- if rnd_num < 20 then
	-- 	err = "TestError"
	-- end
	if not StringIsEmpty(err) or ab == nil then
		UnityEngine.Debug.LogError("GameStateUpdateLuaBundle ERROR===" .. bundle_name .. ",ERR_STR=" .. err)
		Nirvana.AssetManager.LoadBundle(v,function(retry_err,retry_ab) 
			self:onLuaBundleLoadComplete(retry_err,retry_ab,bundle_name) end)
	else
		self.completed_count = self.completed_count + 1
		if self.completed_count >= self.total_count then
			self:Stop()
		end
	end
end


function GameStateUpdateLuaBundle:Stop()

	-- self.cur_st = GameStateLuaRequire.SubState.None
	-- MainProber:Step(MainProber.STEP_LOAD_CONFIG_END)
	GameState.Stop(self)
	print("GameStateUpdateLuaBundle:Stop")
end



return GameStateUpdateLuaBundle