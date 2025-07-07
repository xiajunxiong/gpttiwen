
FightMapView = FightMapView or DeclareView("FightMapView", FightConfig.ResPath .. "fight_map")
VIEW_DECLARE_LEVEL(FightMapView,ViewLevel.L1)
VIEW_DECLARE_MASK(FightMapView, ViewMask.BgBlock)

function FightMapView:FightMapView()
	self.role_pos_event = EventSys:Bind(GlobalEventKeys.RolePosChange, BindTool.Bind(self.OnMainRolePosChange, self))
end

function FightMapView:CloseCallback()
    EventSys:UnBind(self.role_pos_event)
end

function FightMapView:LoadCallback()
	self:OnMainRolePosChange()
end

function FightMapView:OnMainRolePosChange()
	if MainPlayer == nil then return end
    local pos = MainPlayer:GetPos()
    local main_pos = MapCtrl.Instance:WorldToTransformUV(pos, self.MapPos)
	self.MainRolePos.localPosition = main_pos
end

function FightMapView:OnClickClose()
	ViewMgr:CloseView(FightMapView)
end

function FightMapView:OnClickMap()
    local scene_id = SceneData.Instance:SceneId()
    local view_pos = MapCtrl.Instance:TransformUVPos(self.MapPos, scene_id)
    local local_pos = MapCtrl.Instance:TransformUVToWorld(view_pos)
    local y = SceneCtrl.Instance:CurScene():GetPosHeight(local_pos.x, local_pos.z)
    if y == 0 and scene_id == SceneLogic.Instance:SceneID() then
        return
	end
	if SceneData.Instance:GetMode() == SceneModeType.FightCrystal then
		ViewMgr:CloseView(FightMapView)
	end
    SceneLogic.Instance:AutoToPos(nil, {x = local_pos.x, y = local_pos.z},function ()
		FightData.Instance:AutoToNpc()
	end,false,false)
end


FightMapViewF = FightMapViewF or DeclareMono("FightMapViewF", UIWFlushPanel)
function FightMapViewF:FightMapViewF()
	self.data = MapData.Instance
	self.fight_data = FightData.Instance
	self.arena_match_data = ArenaMatchData.Instance
	self.base_data = MainViewData.Instance:BaseData()
	self.society_data = SocietyCtrl.Instance:Data()

	self.data_cares = {
		{data = self.base_data, func = self.FlushBattle, keys = {"is_battle"}, init_call = false},
		{data = self.fight_data:GetFightCrystalInfo(), func = self.FlushMap, init_call = false},
		{data = self.society_data.shitu_fb_sm, func = self.FlushMap, init_call = false},
	}

	GuideManager.Instance:RegisterGetGuideUi("ButtonNextFB", 
	function()
		local list_data = self.ItemList:GetData()
		local scene_mode = SceneData.Instance:GetMode()
		if scene_mode == SceneModeType.FightCrystal then
			for k,v in pairs(list_data) do 
				if v.obj_type == 10 then  -- 水晶副本的传送门为10
					return self.ItemList.item_list[k],function() self.MainView:OnClickMap() end 
				end 
		    end 
	    end 
	end)
	self.scene_care_handle = SceneMgr:CareLoadedSceneIdChange(BindTool.Bind(self.OnLoadedSceneIdChange,self))
end

function FightMapViewF:Awake()
	UIWFlushPanel.Awake(self)
	self:FlushMap()
end

function FightMapViewF:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	GuideManager.Instance:UnRegisterGetGuideUi("ButtonNextFB")
	if nil ~= self.scene_care_handle then
        SceneMgr:UncareLoadedSceneIdChange(self.scene_care_handle)
        self.scene_care_handle = nil
    end
end

function FightMapViewF:OnLoadedSceneIdChange()
    self:FlushMap()
end

function FightMapViewF:CheckScene()
    local value = false
    if 0 == SceneMgr:LoadedSceneId() then value = true end
    if MainViewData.Instance:IsBattle() then value = true end
    return value
end

function FightMapViewF:FlushBattle()
	if self.base_data.is_battle then ViewMgr:CloseView(FightMapView) end
end

function FightMapViewF:FlushMap()
	if self:CheckScene() then return end

	local scene_id = SceneData.Instance:SceneId()

	local bg_sp, type_sp = FightConfig.MapNameBgSp.normal
	local scene_mode = SceneData.Instance:GetMode()
	if scene_mode == SceneModeType.FightCrystal then
		local fight_crystal_info = self.fight_data:GetFightCrystalInfo()
		local cur_seq_info = self.fight_data:GetFightCrystalConfigBySeq(fight_crystal_info.level_seq)
		if cur_seq_info.scene_type_1 > 1 then
			bg_sp = FightConfig.MapNameBgSp.special
			type_sp = FightConfig.MapNameTypeSp[cur_seq_info.scene_type_1]
		end
	end
	self.NameBgSp.SpriteName = bg_sp
	self.NameTypeSp.SpriteName = type_sp

	self.SceneName.text = SceneData.Instance:GetSceneNameCustomBySceneId(scene_id) or SceneData.Instance:Base().scene_name
	self.MapTexture:SetByPath(self.data:GetMapMiniPath(scene_id))

	self:FlushItems(scene_id)
end

function FightMapViewF:FlushItems(scene_id)
	local objs , icons = {}, {}
	local scene_mode = SceneData.Instance:GetMode()
	if scene_mode == SceneModeType.FightCrystal then
		objs = self.fight_data:GetCrystalFbObjInfo()
		local fight_crystal_info = self.fight_data:GetFightCrystalInfo()
		for key, val in pairs(objs) do
			objs[key].pos = SceneCtrl.Instance:CurScene():PosToClint(val.pos_x, val.pos_y)
			objs[key].scale = 1.2
			objs[key].name_i = 10 == val.obj_type and self.fight_data:CheckCrystalSceneName("", fight_crystal_info.layer, fight_crystal_info.level_seq) or ""
		end
		local cur_seq_info = self.fight_data:GetFightCrystalConfigBySeq(fight_crystal_info.level_seq)
		icons = 3 == cur_seq_info.scene_type_1 and FightConfig.CrystalIconsC2 or FightConfig.CrystalIconsC
	elseif scene_mode == SceneModeType.ArenaMatch then
		local co = SceneData.Instance:GetSceneCfg(scene_id) or {}
		objs = co and DataHelper.TableCopy(co.npcs) or {}
		for key, val in pairs(objs) do
			local seq = val.seq
			local cfg = ArenaMatchData.Instance:GetArenacfgByNpcSeq(seq)
			local pos = cfg.position or 0
			local icon = "am_" .. pos
			objs[key].pos = Vector3.New(val.x, val.z, val.y)
			objs[key].icon = icon
		end
		icons = FightConfig.ArenaMatchIcons
		for key, val in pairs(icons) do
			icons[key].scale = 0.7
		end
	elseif scene_mode == SceneModeType.ShiTuFB then
		objs = SocietyData.Instance:GetShiTuFBMonsters()
		for key, val in pairs(objs) do
			objs[key].pos = SceneCtrl.Instance:CurScene():PosToClint(val.pos_x, val.pos_y)
			objs[key].icon = FightConfig.CrystalIconsS[1]
		end
		icons = FightConfig.ShiTuFBIcons
	elseif scene_mode == SceneModeType.HarmonyHeaven then
		objs = HarmonyHeavenData.Instance:GetFightMapObjs()
		for key, val in pairs(objs) do
			objs[key].pos = SceneCtrl.Instance:CurScene():PosToClint(val.pos_x, val.pos_y)
			objs[key].icon = FightConfig.CrystalIconsS[3]
		end
		icons = FightConfig.HarmonyHeavenIcons
	elseif scene_mode == SceneModeType.AdvancedFB then
		objs = self.fight_data:GetAdvanceFbObjInfo()
		for key, val in pairs(objs) do
			objs[key].pos = SceneCtrl.Instance:CurScene():PosToClint(val.pos_x, val.pos_y)
			objs[key].icon = FightConfig.AdvancedIcons[val.obj_type][1]
		end
		icons = FightConfig.AdvancedIcons
	elseif scene_mode == SceneModeType.FightFantasy then 
		objs = self.fight_data:GetFantasyFbObjInfo()
		local fight_fantasy_info = self.fight_data:GetFightFantasyInfo()
		for key, val in pairs(objs) do
			objs[key].pos = SceneCtrl.Instance:CurScene():PosToClint(val.pos_x, val.pos_y)
			objs[key].scale = 1.2
			objs[key].name_i = 10 == val.obj_type and self.fight_data:CheckFantasySceneName("", fight_fantasy_info.layer, fight_fantasy_info.level_seq) or ""
		end
		icons = FightConfig.FantasyIconsC
	elseif scene_mode == SceneModeType.MedalFB then 
		objs = MedalTaskData.Instance:GetMedalTaskFbObjInfo(scene_id)
		for key, val in pairs(objs) do
			objs[key].pos = SceneCtrl.Instance:CurScene():PosToClint(val.pos_x, val.pos_y)
			objs[key].icon = FightConfig.CrystalIconsS[val.obj_type]
		end  
		icons = FightConfig.AdvancedIcons
	elseif scene_mode == SceneModeType.Conspiracy then
		local info = ConspiracyData.Instance:SceneInfo(SceneData.Instance:SceneId())
		objs = ConspiracyData.Instance:BossListByIndex(info.index)
		for key, val in pairs(objs) do
			objs[key].pos = SceneCtrl.Instance:CurScene():PosToClint(val.x, val.y)
			objs[key].icon = FightConfig.CrystalIconsS[3]
		end
		icons = FightConfig.ConspiracyIcons
	elseif scene_mode == SceneModeType.SplaceAnecdote or scene_mode == SCENE_TYPE.SOULS_ANECDOTE then 
		objs = self.fight_data:GetSplaceAnecObjsInfo()
		local fight_anec_info = self.fight_data:GetSplaceAnecInfo()
		for key, val in pairs(objs) do
			objs[key].pos = SceneCtrl.Instance:CurScene():PosToClint(val.pos_x, val.pos_y)
			objs[key].scale = 1.2
			objs[key].name_i = 10 ==  val.obj_type and self.fight_data:CheckCrystalSceneName("", fight_anec_info.layer, fight_anec_info.level_seq) or ""
			objs[key].icon = FightConfig.SplaceAnecdoteIconsS[val.obj_type]
		end
		icons = FightConfig.SplaceAnecdoteIconsC
	end
	self.ItemList:SetData(objs)
	self.IconList:SetData(icons)
end

FightMapViewItem = DeclareMono("FightMapViewItem", UIWidgetBaseItem)

function FightMapViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.ItemPos.localPosition = MapCtrl.Instance:WorldToTransformUV(data.pos, self.MapPos)
	self.TypeSp.SpriteName = data.icon
	self.TypePos.localScale = Vector3.New(data.scale or 1, data.scale or 1, 1)
 	self.Name.text = data.name_i or ""
end

function FightMapViewItem:OnClickItem()
	if self.data == nil then return end
	SceneLogic.Instance:AutoToPos(nil, {x = self.data.pos.x, y = self.data.pos.z},function ()
		FightData.Instance:AutoToNpc()
	end, false, false)
end

FightMapViewItemI = DeclareMono("FightMapViewItemI", UIWidgetBaseItem)

function FightMapViewItemI:FightMapViewItemI()
	self.language = Language.Fight.FightMap
end

function FightMapViewItemI:SetData(data)
	self.IconSp.SpriteName = data[1]
	self.Name.text = self.language.IconNames[data[1]]
	self.IconPos.localScale = Vector3.New(data.scale or 1, data.scale or 1, 1)
end