SceneData = SceneData or BaseClass()

function SceneData:__init()
	if SceneData.Instance ~= nil then
		Debuger.LogError("[SceneData] attempt to create singleton twice!")
		return
	end
	SceneData.Instance = self

	self.scene_id = 0
	self.last_scene_id = 0
	self.map_id = 0
	self.scene_custom_names = SmartData.New({})

	self.scene_config = nil

	self.base = SmartData.New({
		scene_name = "",
		scene_id = 0,
		scene_mode = SceneModeType.None,--场景类型
		})
	self.role_obj_id = 0
	self.change_scene_effect = SmartData.New({id = 0})
	self.mode_tab = nil
	self.role_obj_list = {}
	self.map_xunlu_auto = Config.map_xunlu_auto
	self.xunlu_group_list = {}
	self.xunlu_pos_list = {}
	self:init_config()
end
function SceneData:__delete()
	if SceneData.Instance == self then
		SceneData.Instance = nil
	end
end
function SceneData:init_config()
	for _, value in ipairs(self.map_xunlu_auto.scene_to_xy) do
		self.xunlu_group_list[value.scene_id] = value.xy_group
	end
	for _, value in ipairs(self.map_xunlu_auto.xy_groups) do
		if self.xunlu_pos_list[value.group_id] == nil then
			self.xunlu_pos_list[value.group_id] = {}
		end
		table.insert(self.xunlu_pos_list[value.group_id], {x = value.x_xy, y = value.y_xy})
	end
	self.scene_effect_config = Config.world_map_auto.scene_effect
end

function SceneData:SceneId()
	return self.scene_id
end
function SceneData:LastSceneId()
	return self.last_scene_id
end
function SceneData:MapId()
	return self.map_id
end
function SceneData:SetSceneId(si)
	if self.scene_id ~= si then
		VoMgr:disposeScene(self.scene_id)
		EventSys:Fire(GlobalEventKeys.SceneDataDisposed, self.scene_id, si)
		-- if si ~= self.last_scene_id then
			self.last_scene_id = self.scene_id
		-- end
		self.scene_id = si		
		self.base.scene_id = self.scene_id
		if self.scene_id > 0 then
			local status,co = pcall(require,"config/scenes/scene_" .. self.scene_id)
			self.scene_config = status == true and Config.scenes[self.scene_id] or nil
			if not status then
				if DEBUG_MODE then
					LogError("Scene config not exist:" .. self.scene_id)
				end
			end
			self.base.scene_name = self.scene_config.name
			self.map_id = self.scene_config.mapid
			VoMgr:initScene(self.scene_id)
			EventSys:Fire(GlobalEventKeys.SceneDataInited, self.scene_id)
		else
			self.scene_config = nil
			self.base.scene_name = ""
			self.map_id = 0
		end
	end
end
function SceneData:SceneName()
	return self.base.scene_name
end
function SceneData:SceneConfig()
	return self.scene_config
end

function SceneData:Base()
	return self.base
end

function SceneData:SetRoleObjId(obj_id)
	self.role_obj_id = obj_id
end

function SceneData:GetRoleObjId()
	return self.role_obj_id
end

function SceneData:GetSceneCfg(scene_id)
	scene_id = scene_id == nil and self.scene_id or scene_id
	local status,co = pcall(require,"config/scenes/scene_" .. scene_id)
	if not status then
		LogError("Scene config not exist:" .. scene_id)
	end
	return status == true and Config.scenes[scene_id] or {}
end

function SceneData:GetEntrance(scene_id, to_scene_id)
	local list = Config.entrance_auto.entrance_list
	local x, y = nil, nil
	for k,v in pairs(list) do
		if v.scene_id == scene_id and v.to_scene_id == to_scene_id then
			x ,y = v.to_pos_x ,v.to_pos_z
		end
	end
	return x, y
end

function SceneData:SetCustomSceneName(scene_id, scene_name)
	self.scene_custom_names[scene_id] = scene_name
end

function SceneData:GetSceneNameCustomBySceneId(scene_id)
	return self.scene_custom_names[scene_id]
end

function SceneData:SetMode(scene_mode, scene_id)
	local judge, client_sp_mode = self:JudgeClientSpecialSceneMode(scene_id)
	if self.base.scene_mode == scene_mode and not judge then
		
		return
	end
	local old_mode_tab = SceneConfigToMode[self.base.scene_mode]
	local old_scene_mode = self.base.scene_mode
	
	if judge then
		scene_mode = client_sp_mode
	end
	local new_mode_tab = SceneConfigToMode[scene_mode]
	self.base.scene_mode = scene_mode
	if old_mode_tab ~= new_mode_tab then
		if old_mode_tab ~= nil and old_scene_mode ~= SceneModeType.None then
			-- TeamCtrl.Instance:ChangeTeamLimitToDefault()			-- 离开任何玩法都把组队目标改为组队挂机
			TaskData.Instance:TaskLevelSceneGuide(old_scene_mode)	-- 由于副本任务服务端判断是一进场景就完成，所以需要出场景判读自动任务
			old_mode_tab.OnEnd()
		end
		if new_mode_tab ~= nil then
			new_mode_tab.OnBegin()
		end	
	end
end

function SceneData:OnModeSceneLoaded()
	local mode_tab = SceneConfigToMode[self.base.scene_mode]
	if mode_tab ~= nil and mode_tab.OnSceneLoaded ~= nil then
		mode_tab.OnSceneLoaded()
	end
end

function SceneData:GetMode()
	return self.base.scene_mode
end
function SceneData:GetXunLuPos()
	local scene_id = self:SceneId()
	local group_id = self.xunlu_group_list[scene_id]
	if group_id == nil then
		LogError("当前场景"..scene_id.."没有配置巡逻组")
		return
	end
	local pos_list = self.xunlu_pos_list[group_id]
	if pos_list == nil then
		LogError("当前巡逻组"..group_id.."没有配置坐标点")
		return
	end
	return pos_list
end

function SceneData:GetNeedChangeSceneEffect(old_scene_id)
	if self.scene_effect_config[old_scene_id] == nil and self.scene_effect_config[self:SceneId()] == nil then
		return false
	elseif self.scene_effect_config[old_scene_id] ~= nil and self.scene_effect_config[old_scene_id].leave_scene_effect ~= 0 then
		self.change_scene_effect.id = self.scene_effect_config[old_scene_id].leave_scene_effect
		return true
	elseif self.scene_effect_config[self:SceneId()] ~= nil and self.scene_effect_config[self:SceneId()].enter_scene_effect ~= 0 then
		self.change_scene_effect.id = self.scene_effect_config[self:SceneId()].enter_scene_effect
		return true
	end
end

---------检查传送门需要的命名（默认场景名）------
function SceneData:DoorName(name, protocol)
	local cfg = self:GetSceneCfg(protocol.scene_id)
	name = nil ~= cfg and cfg.name or Language.Fight.Exit
	if cfg.scene_type == SCENE_TYPE.ALL_SOULS_FB then
		name = Language.Fight.Exit
	end
	name = RushDungeonData.Instance:DoorName(name)
	name = MazeRunData.Instance:DoorName(name)
	name = FightData.Instance:CheckCrystalSceneName(name, protocol.param1, protocol.param2)
	name = MedalTaskData.Instance:CheckMedalSceneName(name,protocol.param1)
	name = FightData.Instance:CheckFantasySceneNameS(name,protocol.param1, protocol.param2)
	name = FightData.Instance:CheckAdvancedFBSceneName(name,protocol.scene_id,protocol.param1)
	name = name or Language.Fight.Exit
	return name
end

function SceneData.CalculatePath(start_pos, end_pos)
	local nav_mesh_path = UnityAI.NavMeshPath()
	if not UnityAI.NavMesh.CalculatePath(start_pos, end_pos, -1, nav_mesh_path) then
		-- LogError("CalculatePath return 1", start_pos, end_pos)
		return nil
	end
	local path_poses = nav_mesh_path.corners:ToTable()
	local way_point = {}
	for i,v in ipairs(path_poses) do
		local s_pos_x, s_pos_y = SceneCtrl.Instance:CurScene():PosToServer(path_poses[i])
		way_point[i] = {pos_x = s_pos_x, pos_y = s_pos_y}
	end
	-- LogError("way_point ",way_point)
	return way_point
end

function SceneData:SetChangeSceneNotice(info)
	self.change_scene_notice = info
end

function SceneData:GetChangeSceneNotice()
	return self.change_scene_notice
end

function SceneData:JudgeClientSpecialSceneMode(scene_id)
	if YouMingData.Instance:IsInSealScene(scene_id) then
		return true, SceneModeType.YouMingSeal
	end
end