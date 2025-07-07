
MapData = MapData or BaseClass()

function MapData:__init()
	if MapData.Instance ~= nil then
		Debuger.LogError("[MapData] attempt to create singleton twice!")
		return
	end	
	MapData.Instance = self

	self:MapConfig()
	self:MapData()
end

function MapData:MapConfig()
	self.config = Config.world_map_auto
	self.config_island_list = self.config.island_list
	self.config_towns_list = self.config.towns_list
	self.config_room_list = self.config.room_list
	self.config_irregular_region_xy = self.config.irregular_region_xy
	self.config_word_name_show = self.config.word_name_show
	self.config_word_type_show = self.config.word_type_show
	self.config_scene_unlock = self.config.scene_unlock
	self.config_minimap = Config.minimap_auto
	self.config_camera_list = self.config_minimap.camera_list
end

function MapData:MapData()
	self:MapDataLocal()
	self:MapDataWorld()
	self:MapDataList()
	self:MapDataMini()
	self.view_rect = SmartData.New{width = 0, height = 0}
	self.view_rect_ready = false

	self.irregular_flag_list = {}
	self.new_unlock_flag_list = {}
	self.irregular_state = SmartData.New{
		flag = false
	}
	self.map_guide = SmartData.New{
		state = MapConfig.GuideState.hide
	}
	self.mini_camera_cfg = {}
	for _,v in ipairs(self.config_camera_list) do
		self.mini_camera_cfg[v.name] = v
	end
	self.first_enter_fbtype = {}
end

function MapData:SetMapUnlockAllListInfo(protocol)
	self.irregular_flag_list = protocol.irregular_flag_list
	self.new_unlock_flag_list = protocol.new_unlock_flag_list
	for _, v in pairs(self.local_l_list) do
		self.local_l_list[v.id].lock_state, self.local_l_list[v.id].anim_state = self:GetIrregularFlagState(v.id)
	end
	for _, v in pairs(self.local_t_info.list) do
		self.local_t_info.list[v.id].lock_state, self.local_t_info.list[v.id].anim_state = self.local_l_list[v.irregular_id].lock_state, self.local_l_list[v.irregular_id].anim_state
	end
	self.irregular_state.flag = not self.irregular_state.flag
end

function MapData:GetIrregularFlagState(irregular_id)
	local index, val = math.ceil((irregular_id + 1) / 64), irregular_id % 64
	local flag1 = self.irregular_flag_list[index] or 0
	local flag2 = self.new_unlock_flag_list[index] or 0
	return 1 == bit:_rshift(flag1, val) % 2, 1 == bit:_rshift(flag2, val) % 2
end

function MapData.MapIdToIcon(scene_id)
	return Config.world_map_auto.map_png[scene_id] and Config.world_map_auto.map_png[scene_id].map_png or ""
end
--获取加载地图路径
function MapData:GetMapMiniPath(scene_id)
	local map_res = MapData.MapRes(scene_id)
    return Format(MapConfig.Mini.png_path, MPMAPRES[scene_id] and MPMAPRES[scene_id] or map_res)
end

--获取地图资源名称(不传就是当前)
function MapData.MapRes(scene_id)
	local scene_cfg = SceneData.Instance:GetSceneCfg(scene_id)
	local map_cfg = Config_scenelist[scene_cfg.mapid]
	if map_cfg then
		return map_cfg.map_res
	end
	LogError("没有找到地图配置")
	return nil
end

--获取室外的名字
function MapData.GetOutdoorName(scene_id)
	local scene_cfg = SceneData.Instance:GetSceneCfg(scene_id)
	local doors_list = scene_cfg.doors or {}
	if next(doors_list) and table.nums(doors_list) == 1 then
		if doors_list[1].type == 2 then
			return doors_list[1].name
		end
	end
end

function MapData:GetMiniCameraCfg(scene_id)
	local map_res = MapData.MapRes(scene_id)
	if MPNEEDCHANGE[scene_id] then
		return {
			x = 73,
			y = 27,
			z = 155,
			r_x = 48,
			r_y = 45,
			r_z = 0,
			name = "MP_1",
			size = 25
		}
	end
	return self.mini_camera_cfg[map_res]
end

--获取地图显示配置
function MapData:GetMapShowData(scene_id)
	return CacheList(self,"word_show_list",scene_id,function()
		for k,v in pairs(self.config_word_type_show) do
			if v.scene_id == scene_id then
				return v
			end
		end
	end)
end

--是否显示小地图
function MapData:IsHasMiniMap(scene_id)
	local config = self:GetMapShowData(scene_id)
	return config and config.is_show_minimap == 1 or false
end

--获取怪物列表
function MapData:GetMapShowList(content)
	return tonumber(content) and {content} or string.split(content, "|")
end

--获取地图图标显示列表
function MapData:GetMapMiniIconList(scene_id)
	return CacheList(self,"map_mini_icon_list",scene_id,function()
		local list = {}
		local map_cfg = SceneData.Instance:GetSceneCfg(scene_id) or {}
		for k,v in pairs(map_cfg.npcs) do
			local data = self:CavernList(v)
			if data ~= nil then
				table.insert(list,data)
			end
		end
		for k,v in pairs(map_cfg.doors) do
			table.insert(list,v)
		end
		return list
	end)
end

--获取场景配置
function MapData:GetMapConfig(scene_id,key)
	local cfg = SceneData.Instance:GetSceneCfg(scene_id)
	return key and cfg[key] or cfg
end

function MapData:GetMapMinLevel(restrict_in)
	if self.config_scene_unlock[restrict_in] ~= nil then
		return self.config_scene_unlock[restrict_in].pram1
	else
		LogError("world_map_auto scene_unlock does not has seq:",restrict_in)
		return 0
	end
end

--根据名字查找是否是洞穴
function MapData:IsCavern(name)
	return CacheList(self,"map_mini_cavern",name,function()
		for k,v in pairs(self.config_word_type_show) do
			if v.png_type == 2 and v.name == name then
				return true
			end
		end
	end)
end

--小地图判断
function MapData:IsCanToMini(scene_id)
	if SceneData.Instance:GetMode() ~= SceneModeType.None then
		if SceneData.Instance:GetMode() == SceneModeType.FightGather then
			return true
		end
		return false
	end
	if not self:IsHasMiniMap(scene_id) then
		return false
	end
	if not UnityGameObj.FindGameObjectWithTag("MinimapCamera") then
		LogError("当前场景没有MinimapCamera " .. scene_id)
		return false
	end
	return true
end

function MapData:SetSelectSceneId(scene_id)
	self.select_scene_id = scene_id
end

function MapData:GetSelectSceneId()
	return self.select_scene_id
end

function MapData:CavernList(data)
	if data.show_condition == nil then
		return
	end
	if data.show_condition == 0 and self:IsCavern(data.name)then
		data.type = 3
		return data
	end
end
--首次进入记录
function MapData:SetFirstEnterInfo(list)
	local index = 1
	for _, value in pairs(SCENE_TYPE) do
		index = index + math.floor(value / 64)
		self.first_enter_fbtype[value] = bit:d64b(list[index])[64-value]
	end
end

function MapData:GetFirstEnterInfo(fb_type)
	return self.first_enter_fbtype[fb_type]
end

-- 1 已进入 0 未进入
function MapData:SetSingleFirstEnterInfo(fb_type, value)
	self.first_enter_fbtype[fb_type] = value
end

--获取地图静态NPC列表
function MapData:GetMapMiniNpcList(scene_id)
	local list = {}
    for i,v in ipairs(self:GetMapConfig(scene_id,"npcs")) do
        if v.name ~= "" then
            if v.show_condition == 0 and self:IsCavern(v.name) == nil then
                if v.landmark then
                    if v.landmark == 0 then
                        table.insert(list,v)
                    end
                else
                    table.insert(list,v)
                end
            end
        end
	end
	return list
end

--小地图怪物列表详细
function MapData:GetMapMiniShowList(data)
	local list = {}
    for i,v in ipairs(Language.Map.ShowTitle) do
        if data[v.key] and tostring(data[v.key])~= "0" then
			table.insert(list,{key = v.key,title = v.name,show_list = self:GetMapShowList(data[v.key])})
        end
    end
    return list
end