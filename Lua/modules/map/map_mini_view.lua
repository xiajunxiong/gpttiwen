MapMiniView = MapMiniView or DeclareView("MapMiniView",MapConfig.ResPath.."map_mini")

function MapMiniView:MapMiniView()
    self.data = MapData.Instance
    self.role_pos_event = EventSys:Bind(GlobalEventKeys.RolePosChange, BindTool.Bind(self.onMainRolePosChange, self))

    -- 40571 宛村声望商店NPC
    GuideManager.Instance:RegisterGetGuideUi("MiniMapWanCunShengWang",function()
        return self.NpcList:GuideVertical(function(data) return data.id == 40571 end,2);
    end)
end

function MapMiniView:CloseCallback()
    EventSys:UnBind(self.role_pos_event)
    EventSys:Fire(GlobalEventKeys.CloseMapMini)
    TimeHelper:CancelTimer(self.show_time)

end

function MapMiniView:LoadCallback(scene_id)
    --加载场景怪物
    local show_tab = {}
    local scene_config = self.data:GetMapShowData(scene_id)
    if scene_config ~= nil then
        UH.SetText(self.Title,scene_config.name)
        UH.SetText(self.BossLevel,scene_config.monster_levels)
        local boss_list = self.data:GetMapMiniShowList(scene_config)
        self.BossList:SetData(boss_list)
        show_tab[1] = #boss_list > 0
    end
    --加载NPC列表
    local npc_list = self.data:GetMapMiniNpcList(scene_id)
    show_tab[2] = #npc_list > 0
    self.NpcList:SetData(npc_list)
    
    --加载地图
    if self.Texture:TextureIsNil() then
        self.Texture:SetByPath(self.data:GetMapMiniPath(scene_id))
    end
    --加载地图图标
    local icon_list = self.data:GetMapMiniIconList(scene_id)
    for k,v in pairs(icon_list) do
        local pos = Vector3.New(v.x,v.z,v.y)
        v.pos = MapCtrl.Instance:WorldToTransformUV(pos,self.MapTex,scene_id)
    end
    self.IconList.StepCount = #icon_list
    self.IconList:SetData(icon_list)

    self:onMainRolePosChange()
    self.RoleIcon:SetActive(SceneData.Instance:SceneId() == scene_id)

    --刷新标签
    for i=1,2 do
        self.TabList[i]:SetActive(show_tab[i] or false)
    end
    if not show_tab[1] and show_tab[2] then
        self.RoleToggle.isOn = true
    end

    MapCtrl.Instance:SendMapExplorationReq({req_type = 2})
    -- self.MapExploreObj:SetActive(MapExploreData.Instance:CanExplore(scene_id))
    local cur_prog_detail = MapExploreData.Instance:GetCurSceneProgDeatail(scene_id)
    self.Panel.scene_id = scene_id
    self.Panel:FlushExRed()
    local show_flag =  cur_prog_detail.prog_total ~= ""

    self.MapExploreObj:SetActive(show_flag)
    if show_flag then 
        UH.SetText(self.MapExploreProg,cur_prog_detail.prog_total)

        self.MapExploreDetail.gameObject:SetActive(cur_prog_detail.prog_map ~= "")
        UH.SetText(self.MapExploreDetail,cur_prog_detail.prog_map)
        self.AnecdoteEventDetail.gameObject:SetActive(cur_prog_detail.prog_event ~= "")
        UH.SetText(self.AnecdoteEventDetail,cur_prog_detail.prog_event)
        self.AnecdoteChallengeDetail.gameObject:SetActive(cur_prog_detail.prog_challenge ~= "")
        UH.SetText(self.AnecdoteChallengeDetail,cur_prog_detail.prog_challenge)
    end 
end

function MapMiniView:OnClickPlus()
    self.BtnPlus:SetActive(not self.BtnPlus.activeSelf)
end

function MapMiniView:OnClickPorgReward()
    ViewMgr:OpenView(MapExploreTotalRewardView,{scene_id = self.Panel.scene_id})
end

--加载我的位置
function MapMiniView:onMainRolePosChange()
    if MainPlayer == nil then return end
    local scene_id = MapData.Instance:GetSelectSceneId()
    local pos = MainPlayer:GetPos()
    local main_pos = MapCtrl.Instance:WorldToTransformUV(pos,self.MapTex, scene_id)
    UH.LocalPos(self.MainRolePos,main_pos)
end

--点击地图寻路
function MapMiniView:OnClickMap()
    local scene_id = MapData.Instance:GetSelectSceneId()
    local view_pos = MapCtrl.Instance:TransformUVPos(self.MapTex, scene_id)
    if ViewMgr:IsOpen(MapView) then
        MapCtrl.Instance:AutoToPosMapView(view_pos)
        return
    end
    local local_pos = MapCtrl.Instance:TransformUVToWorld(view_pos)
    local y = SceneCtrl.Instance:CurScene():GetPosHeight(local_pos.x, local_pos.z)
    if y == 0 and scene_id == SceneLogic.Instance:SceneID() then
        return
    end
    SceneLogic.Instance:AutoToPos(nil, {x = local_pos.x, y = local_pos.z})
end

--世界地图
function MapMiniView:OnClickWorld()
    if ViewMgr:IsOpen(MapView) then
        ViewMgr:CloseView(MapMiniView)
    else
        ViewMgr:CloseView(MapMiniView)
        self.show_time = TimeHelper:AddDelayTimer(function()
            ViewMgr:OpenView(MapView)
        end, 0.6)
    end
end

function MapMiniView:OnClickClose()
    ViewMgr:CloseView(MapMiniView)
end

MapMiniPanel = MapMiniPanel or DeclareMono("MapMiniPanel", UIWFlushPanel)
function MapMiniPanel:MapMiniPanel()
    self.data = MapExploreData.Instance
    self.data_cares = {
        {data = self.data.explore_reward_data, func = self.FlushExRed},
    }
end 

function MapMiniPanel:FlushExRed()
    self.ex_red = self.ex_red or UiRedPoint.New(self.ExploreProgRed, Vector3.New(0,0,0))
    self.ex_red:SetNum(self.data:GetMapExploreRed(self.scene_id) and 1 or 0)
end

----------------------------MapMiniItem----------------------------
MapMiniItem = MapMiniItem or DeclareMono("MapMiniItem", UIWidgetBaseItem)

function MapMiniItem:SetData(data)
    UH.SetText(self.Title,data.title)
    local list = {}
    for i,v in ipairs(data.show_list) do
        local id = tonumber(v)
        if data.key == "monster_id"then
            table.insert(list,Monster.New({item_id = id,is_quality = true}))
        elseif data.key == "is_pet" then
            table.insert(list,Pet.New({item_id = id}))
        else
            table.insert(list,Item.New({item_id = id}))
        end
    end
    self.Grid:SetData(list)
end

----------------------------MapNpcItem----------------------------
MapNpcItem = MapNpcItem or DeclareMono("MapNpcItem", UIWidgetBaseItem)

function MapNpcItem:SetData(data)
    local config = NpcHelper.NpcCo(data.id)
    UH.SetText(self.Name,data.name)
    UH.SetText(self.Type,config.tit_name)
    UH.SetIcon(self.Head, config.npc_icon)
    UIWidgetBaseItem.SetData(self, data)
end

function MapNpcItem:OnClickItem()
    if ViewMgr:IsOpen(MapView) then
        MapCtrl.Instance:AutoToPosMapView(self.data)
        return
    end
	SceneLogic.Instance:AutoToNpc(self.data.seq)
    ViewMgr:CloseView(MapMiniView)
end

----------------------------MapMiniIconItem----------------------------
MapMiniIconItem = MapMiniIconItem or DeclareMono("MapMiniIconItem", UIWidgetBaseItem)

function MapMiniIconItem:SetData(data)
    if data.pos then
        UH.LocalPos(self.Item,data.pos)
    end
    --NPC名字
    if data.type == nil then
        if self.NpcName then
            UH.SetText(self.NpcName,data.name)
        end
    else
        if self.MapName then
            local type = MapConfig.IconTypeChagnge[data.type]
            UH.SetText(self.MapName,Format(MapConfig.IconColors[type],data.name))
            self:ShowIconView(type)
        end
    end
    if self.NpcName then
        self.NpcName:SetObjActive(data.type == nil)
    end
    UIWidgetBaseItem.SetData(self, data)
end

function MapMiniIconItem:ShowIconView(type)
    for i=1,3 do
        if self.IconList[i] then
            self.IconList[i]:SetActive(i == type + 1)
        end
    end
end

function MapMiniIconItem:OnClickItem()
    if self.data == nil then return end
    if ViewMgr:IsOpen(MapView) then
        MapCtrl.Instance:AutoToPosMapView(self.data)
        return 
    end
    if self.data.to_scene_id then
        SceneLogic.Instance:AutoToPos(nil, self.data)
    else
	    SceneLogic.Instance:AutoToNpc(self.data.id)
    end
    ViewMgr:CloseView(MapMiniView)
end

----------------------------MapInfoView----------------------------
MapInfoView = MapInfoView or DeclareView("MapInfoView", MapConfig.ResPath.."map_info")

VIEW_DECLARE_MASK(MapInfoView,ViewMask.BlockClose)

function MapInfoView:LoadCallback(param_t)
    self.param_t = param_t
    self:FlushShowView(param_t.item)
    self.List:SetData(param_t.funcs or {})
    self.Functions:SetActive(param_t.funcs ~= nil)
    UH.LocalPos(self.Root,param_t.root_pos + Vector2.New(120,-160))
    UH.SetEdgePosition(self.Root,self.Rect)
end

function MapInfoView:FlushShowView(item)
    if item == nil then return end
    self.Call:SetData(item)
    self.Element:SetData(item:EleAttr())
    UH.SetText(self.Name,item:Name())
    local scene_id = item.info.scene_id or MapData.Instance:GetSelectSceneId()
    local cfg = SceneData.Instance:GetSceneCfg(scene_id) or {}
    local scene_name = cfg.name
    if item.monster_id then
        UH.SetText(self.Desc,Format(Language.Map.BossDesc[1],item:Name(),scene_name))
    else
        UH.SetText(self.Desc,Format(Language.Map.BossDesc[2],scene_name,item:Name()))
    end
    if item.config.race == -1 then
        UH.SetText(self.Type,"")
    else
        UH.SetText(self.Type,Format("(%s)",Language.Pet.RaceClass[item.config.race]))
    end
end

----------------------------MapInfoItem----------------------------
MapInfoItem = MapInfoItem or DeclareMono("MapInfoItem", UIWidgetBaseItem)

function MapInfoItem:SetData(data)
    UH.SetText(self.Name,data.name)
    UIWidgetBaseItem.SetData(self, data)
end

function MapInfoItem:OnClickItem()
    if self.data.enter_click then
        self.data.enter_click()
    else
        InteractionPatrol.Exe({param1 = MapData.Instance:GetSelectSceneId()})
        ViewMgr:CloseView(MapMiniView)
        ViewMgr:CloseView(MapView)
    end
    ViewMgr:CloseView(MapInfoView)
end