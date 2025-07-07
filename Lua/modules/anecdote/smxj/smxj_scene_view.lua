SmxjSceneView = SmxjSceneView or DeclareView("SmxjSceneView", "anecdote/smxj_scene_view")
VIEW_DECLARE_MASK(SmxjSceneView, ViewMask.None)
VIEW_DECLARE_LEVEL(SmxjSceneView, ViewLevel.L0)

function SmxjSceneView:OnClickSociety()
	ViewMgr:OpenView(SocietyView)
end

function SmxjSceneView:OnClickChat()
	ViewMgr:OpenView(ChatView)
end

function SmxjSceneView:OnClickReturn()
	local mode = SceneData.Instance:GetMode()
	PublicPopupCtrl.Instance:AlertTip(Language.Chat.BackTips .. FightCtrl.GetWarnTips(mode),function()
		SceneCtrl.Instance:RequestLeaveFb()
	end)
end

--================================SmxjSceneTopLeft=================================
SmxjSceneTopLeft = SmxjSceneTopLeft or DeclareMono("SmxjSceneTopLeft", UIWFlushPanel)
----WifiSp===UIImageSpriteSet
----TimeTex===Text
----PosTex===Text
----LineTex===Text
----NameTex===Text
----DianLiangImg===image
----IconsObj==obj
-- 网络延迟时间类型
Delay_Type = {
    Nomal = 0.5,
    Bad = 1
}
function SmxjSceneTopLeft:SmxjSceneTopLeft()
    self.time_ctrl = TimeCtrl.Instance
    self.scene_data = SceneData.Instance
    self.map_data = MapCtrl.Instance:Data()
    self.cur_time = 0
    self.cur_wifi = 2
    self.status_flush_timer = nil
    Runner.Instance:AddRunObj(self, 1)
    self.login_data = LoginData.Instance:BaseData()
    self.data_cares = {
        {data = self.time_ctrl.delay_time, func = self.FlushWIFI},
        {data = self.scene_data:Base(), func = self.FlushMap},
        {data = self.scene_data.scene_custom_names, func = self.FlushMapName},
        {data = self.login_data, func = self.FlushMapName, keys = {"is_on_crosssever"}, init_call = false},
        {data = self.login_data, func = self.FlushMap, keys = {"is_on_crosssever"}, init_call = false},
        {data = AnecdoteData.Instance.task_cache_list, func = self.OnTaskChange, init_call = false}
    }
	self.role_pos_event = EventSys:Bind(GlobalEventKeys.RolePosChange, BindTool.Bind(self.onMainRolePosChange, self))
    self.last_update_pos_frame = 0
end

function SmxjSceneTopLeft:FlushRewardPreName()
    local state = MainViewData.Instance:GetGameState()
    if state == MainViewGameState.rush_dungeon then
        local info = RushDungeonData.Instance.layer_info
        UH.SetText(self.RewardTex, string.format( Language.RushDungeon.LevelReward, info.map_level + 1))
    else
        UH.SetText(self.RewardTex, Language.Fight.RewardDropPreview)
    end
end

function SmxjSceneTopLeft:FlushIcons()
    local state = MainViewData.Instance:GetGameState()
    self.IconsObj:SetActive(state ~= MainViewGameState.fight_crystal and state ~= MainViewGameState.rush_dungeon and state ~= MainViewGameState.du_chuang_zei_ying)
    self.RewardPreview:SetActive(state == MainViewGameState.fight_crystal or state == MainViewGameState.rush_dungeon)
end
function SmxjSceneTopLeft:onMainRolePosChange()
    if MainViewData.Instance:BaseData().is_battle == true then
        return
    end
    if MainPlayer == nil then
        return
    end
    if Time.frameCount - self.last_update_pos_frame < 10 then   --降低刷新主界面位置信息的频率，优化GC Alloc
        return
    end
    self.last_update_pos_frame = Time.frameCount
    local pos = MainPlayer:GetPos()
    local cur_scene = SceneCtrl.Instance:CurScene()
    if cur_scene ~= nil and cur_scene.PosToServer ~= nil then
        local x, y = cur_scene:PosToServer(pos)
        if DEBUG_MODE then
            UH.SetText(self.PosTex, string.format("(%d,%d)(%s,%s)", math.floor(pos.x), math.floor(pos.z), x, y))
        else
            local txt = string.format("(%d,%d)", math.floor(x), math.floor(y))
            UH.SetText(self.PosTex, txt)
        end
    end
end

function SmxjSceneTopLeft:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.status_flush_timer = TimeHelper:AddRunTimer(function() 
        local lv = Nirvana.DeviceTool.GetBatteryLevel()
        self.DianLiangImg.fillAmount = Nirvana.DeviceTool.GetBatteryLevel() / 100
    end,10,-1,true)
end

function SmxjSceneTopLeft:OnDisable()
    TimeHelper:CancelTimer(self.status_flush_timer)
    self.status_flush_timer = nil
end

function SmxjSceneTopLeft:FlushMap()
    local config = self.scene_data:SceneConfig()
    if config == nil then
        return
    end
    if nil == self.guide_tips_crystal_map and self.scene_data:GetMode() == SceneModeType.FightCrystal then
        if UnityPlayerPrefs.GetInt(PrefKeys.FightCrystal()) == 0 then
            local param_t = {
                desc = Language.Guide.FightCrystal,
                pos = Vector2.New(-64, -115),
                show = 5,
                arrow = 2,
                arrow_pos = Vector2.New(-62.3,17.2),
                not_destroy = true,
            }
            self.guide_tips_crystal_map = UiGuideTip.New(self.NameTex.gameObject,param_t)
            UnityPlayerPrefs.SetInt(PrefKeys.FightCrystal(), 1)
        end
    elseif nil ~= self.guide_tips_crystal_map and self.scene_data:GetMode() ~= SceneModeType.FightCrystal then
        self.guide_tips_crystal_map:DeleteMe()
        self.guide_tips_crystal_map = nil
    end
    local custom_scene_name = SceneData.Instance:GetSceneNameCustomBySceneId(SceneData.Instance:SceneId())
    local scene_name = custom_scene_name or SceneData.Instance:Base().scene_name
    if self.login_data.is_on_crosssever then
        UH.SetText(self.NameTex, string.format("%s(%s)", scene_name, Language.Login.CrossRealm))
    else
        UH.SetText(self.NameTex, scene_name)
    end
    UH.SetIcon(self.MapIcon, MapData.MapIdToIcon(config.id), ICON_TYPE.MAP)
    self:onMainRolePosChange()
end

function SmxjSceneTopLeft:FlushMapName()
    local custom_scene_name = SceneData.Instance:GetSceneNameCustomBySceneId(SceneData.Instance:SceneId())
    local temp_name = custom_scene_name or SceneData.Instance:Base().scene_name
    if self.login_data.is_on_crosssever then
        UH.SetText(self.NameTex, string.format("%s(%s)", temp_name, Language.Login.CrossRealm))
    else
        UH.SetText(self.NameTex, temp_name)
    end
end
function SmxjSceneTopLeft:Update()
    local server_time = TimeManager:GetServerTime()
    if self.cur_time ~= tonumber(os.date("%M", server_time)) then
        UH.SetText(self.TimeTex, os.date("%H:%M", server_time))
        self.cur_time = tonumber(os.date("%M", server_time))
    end
end

function SmxjSceneTopLeft:FlushWIFI()
    local delay = self.time_ctrl:GetDelayTime()
    local index = 4
    if delay == nil then
        index = 2
    end
    if delay and delay > Delay_Type.Nomal then
        index = 3
    end
    if delay and delay > Delay_Type.Bad then
        index = 2
        PublicPopupCtrl.Instance:Center("网络不稳定")
    end
    if self.cur_wifi ~= index then
        UH.SpriteName(self.WifiSp, "XiTongXinXi_XinHao" .. index)
        self.cur_wifi = index
    end
end
function SmxjSceneTopLeft:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Runner.Instance:RemoveRunObj(self)
    TimeHelper:CancelTimer(self.timer_map_guide)
    EventSys:UnBind(self.role_pos_event)
end

function SmxjSceneTopLeft:OnTaskChange()
	local task_info = AnecdoteData.Instance:GetTaskInfoByType(AnecdoteType.StarLife)
    if task_info == nil then
        local cfg = Config.smxj_story_auto.other[1]
        SceneCtrl.Instance:DeleteNpc(cfg.npc_id, true)
		SceneCtrl.Instance:RequestLeaveFb()
    end
end

function SmxjSceneTopLeft:OnClickMapMin()
    PublicPopupCtrl.Instance:Center(Language.Map.BlockTips[1])
end
