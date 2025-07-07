GameScene = GameScene or BaseClass(BaseScene)
local Input = UnityEngine.Input
local touch_helper = Game.SceneTouchHelper
function GameScene:__init(scene_id)
	self.scene_id = scene_id
	self.scene_loaded = false
	self.terrain_collider = nil
	self.terrain_edge_min = Vector3.zero
	self.terrain_edge_max = Vector3.zero

	self.quality = nil
	Runner.Instance:AddRunObj(self, 3)

	self.player_last_pos = Vector3.zero
	self.is_in_door = false
	self.is_touch_begin = false
	self.touch_began_time = 0
	self.is_touch_moving = false

	self.free_time = Time.realtimeSinceStartup
end

function GameScene:__delete()
	Runner.Instance:RemoveRunObj(self)
end

function GameScene:StartLoading()
	ViewMgr:MainViewVisible(true)
	ViewMgr:OpenView(TopLayerView)

	--[[ if LoginData.Instance:IsNewChara() then
		CutChapterCtrl.Instance:SetCheckPos(1)
		LoginData.Instance:ClearIsNewChara()
	end ]]

	self.scene_loaded = false

	local scene_cfg = SceneData.Instance:SceneConfig()
	if scene_cfg == nil then	--断线重连问题已被修复，不会频繁跳场景，应该不会还有这个问题，暂时屏蔽看后台还会不会有问题
		-- PublicPopupCtrl.Instance:Center("场景配置异常,使用场景id为"..SceneData.Instance:SceneId())
		self.scene_loaded = true
		return
	end
	local audio = Config.audio_auto.scene[scene_cfg.id]
	AudioMgr:PlayBg(audio == nil and 2001 or audio.audio_id)
	-- LogDG("场景类型",scene_cfg.id, scene_cfg.scene_type)
	SceneData.Instance:SetMode(scene_cfg.scene_type, scene_cfg.id)

	local map_cfg = Config_scenelist[scene_cfg.mapid]
	local sn = map_cfg.map_res
	SceneMgr:LoadScene(sn,function(error)
			if self:Disposed() then
				return
			end
			if not StringIsEmpty(error) then
				Debuger.LogError("GameScene Load Scene failed===%s,ERROR=%s",sn,error)
			else

				local ter_col_obj = SceneMgr.FindGameObjectWithTag("TerrainCollider")
				self.terrain_collider = ter_col_obj:GetComponent(typeof(Game.TerrainCollider))
				self.terrain_edge_min = self.terrain_collider.EdgeMinPos
				self.terrain_edge_max = self.terrain_collider.EdgeMaxPos

				if MainPlayer == nil then
					return
				end
				self:ChangeCarpet(scene_cfg.id)
				self:ChangeCarpet2(scene_cfg.id)
				self:ChangeCarpet3(scene_cfg.id)

				self:ResetCamera()

				local quality_obj = SceneMgr.FindGameObjectWithTag("Quality")
				self.quality = quality_obj:GetComponent(typeof(Crystal.Quality))
				SceneData.Instance:Base().scene_name = SceneData.Instance:SceneName()
				GpQualityMgr:SetShadowDirection(self.quality.ShadowDirection)
				GpQualityMgr:SetShadowStrength(self.quality.Strength)

				-- self:SetQualitySceneBlock(QuaCtrl:GetQuality())

				local main_camera = UnityGameObj.FindGameObjectWithTag("MainCamera")
				local post_effect = main_camera:GetComponent(typeof(Crystal.PostEffectDepthOfField))
				post_effect.enabled = false
			end
			SceneCtrl.Instance:Objs():OnGameSceneLoaded()
			SceneCtrl.Instance:Logic():OnGameSceneLoaded(SceneMgr:LoadedSceneId(), scene_cfg.scene_type)
			MainViewData.Instance:SetBattle(false)
			BattleData.Instance.story_data.is_start = false
			self.scene_loaded = true
			SceneData.Instance:OnModeSceneLoaded()
			YinYangCtrl.Instance:OnModeSceneLoaded()
			-- 切场景都会去检查
			MSLanternCtrl.Instance:CheckCreateNPC()
			ImperialExamCtrl.Instance:CheckCreateNPC()
			SE:Play(SceneEffectConfig.EffectList.RoleCreate, nil,nil,  MainPlayer.root_node)
		end, self.scene_id)
end
function GameScene:ResetCamera()
	local scene_cfg = SceneData.Instance:SceneConfig()
	local vir_cam_obj = SceneMgr.FindGameObjectWithTag("MainVirtualCamera")
	local vir_cam = vir_cam_obj:GetComponent(typeof(Cinemachine.CinemachineVirtualCamera))
	self:SetMainVirtualCamera(scene_cfg.id,vir_cam_obj,vir_cam)

	local vir_cam_obj2 = SceneMgr:GetFreeLookCamera(SceneManager.FreeLookCamType.FreeLookCamera)
	local vir_cam2 = vir_cam_obj2:GetComponent(typeof(Cinemachine.CinemachineFreeLook))
	MainPlayer:BindVirtualCamera(vir_cam2, true)

	local vir_cam_obj3 = SceneMgr:GetFreeLookCamera(SceneManager.FreeLookCamType.FreeLookCamera2)
	local vir_cam3 = vir_cam_obj3:GetComponent(typeof(Cinemachine.CinemachineFreeLook))
	MainPlayer:BindVirtualCamera(vir_cam3)
	vir_cam3.Priority = MPNEEDCHANGE[scene_cfg.id] and 6 or 4

	local vir_cam_obj4 = SceneMgr:GetFreeLookCamera(SceneManager.FreeLookCamType.FishSceneCamera)
	local vir_cam4 = vir_cam_obj4:GetComponent(typeof(Cinemachine.CinemachineFreeLook))
	MainPlayer:BindVirtualCamera(vir_cam4)
end

--特殊逻辑换地毯
function GameScene:ChangeCarpet(id)
	if MPSCENEID[id] == nil then return end
	local ditan = UnityGameObj.Find("MP_1_ditan_02")
	local taizi = UnityGameObj.Find("MP_1_taizi_01")
	local baozuo = UnityGameObj.Find("MP_1_baozuo")
	if ditan and taizi and baozuo then
		local renderer = ditan:GetComponent(typeof(UnityEngine.Renderer))
		local renderer2 = taizi:GetComponent(typeof(UnityEngine.Renderer))
		local renderer3 = baozuo:GetComponent(typeof(UnityEngine.Renderer))
		if renderer and renderer2 and renderer3 then
			local material = renderer.sharedMaterial
			local material2 = renderer2.sharedMaterial
			local material3 = renderer3.sharedMaterial
			ResMgr:LoadObject(string.format("ui/textures/menpai/%s.jpg",id),function (tex)
				material.mainTexture = tex
			end)
			ResMgr:LoadObject(string.format("ui/textures/menpai/%s_taizi.jpg",id),function (tex)
				material2.mainTexture = tex
			end)
			ResMgr:LoadObject(string.format("ui/textures/menpai/%s_baozuo.jpg",id),function (tex)
				material3.mainTexture = tex
			end)
		end
	end
end
function GameScene:ChangeCarpet3(id)
	if SLCJRES[id] == nil then return end
	local ditan = UnityGameObj.Find("SLCJ_1_ditan01")
	local yuanpan = UnityGameObj.Find("XWM_2_pingtai02a")
	local zhanqi = UnityGameObj.Find("SLCJ_1_zhanqi01")
	if ditan and yuanpan and zhanqi then
		local renderer = ditan:GetComponent(typeof(UnityEngine.Renderer))
		local renderer2 = yuanpan:GetComponent(typeof(UnityEngine.Renderer))
		local renderer3 = zhanqi:GetComponent(typeof(UnityEngine.Renderer))
		if renderer and renderer2 and renderer3 then
			local material = renderer.sharedMaterial
			local material2 = renderer2.sharedMaterial
			local material3 = renderer3.sharedMaterial
			ResMgr:LoadObject(string.format("ui/textures/slcj/%s.jpg",SLCJRES[id][1]),function (tex)
				material.mainTexture = tex
			end)
			ResMgr:LoadObject(string.format("ui/textures/slcj/%s.jpg",SLCJRES[id][2]),function (tex)
				material2.mainTexture = tex
			end)
			ResMgr:LoadObject(string.format("ui/textures/slcj/%s.tga",SLCJRES[id][3]),function (tex)
				material3.mainTexture = tex
			end)
		end
	end
end
function GameScene:ChangeCarpet2(id)
	if LEITAIZHAN[id] == nil then return end
	local taizi = UnityGameObj.Find("LTCJ_2_taizi02") or UnityGameObj.Find("xd_2_yuanpan1 (8)")
	if taizi then
		local renderer = taizi:GetComponent(typeof(UnityEngine.Renderer))
		if renderer then
			local material = renderer.sharedMaterial
			ResMgr:LoadObject(string.format("ui/textures/leitaizhan/%s.jpg",LEITAIZHAN[id]),function (tex)
				material.mainTexture = tex
			end)
		end
	end
end
function GameScene:LoadingComplete()
	local complete = (ViewMgr:IsOpen(MainView) and ViewMgr:IsOpen(TopLayerView) and self.scene_loaded)
	return complete
end

function GameScene:Dispose()
	BaseScene.Dispose(self)
	SceneCtrl.Instance:Objs():ClearScene()
	Runner.Instance:RemoveRunObj(self)
end

function GameScene:PosToClint(x,y)
	local need_samp = false
	local re_p = Vector3.New()
	if self.terrain_collider and x ~= nil and y ~= nil then
		re_p.x = x / 5 + self.terrain_edge_min.x
		re_p.z = y / 5 + self.terrain_edge_min.z
		re_p.y,need_samp = self:GetPosHeight(re_p.x,re_p.z)
	end
	return re_p,need_samp
end

function GameScene:PosToServer(pos, mapid)
	--位置偏移,放大,四舍五入
	local scene_cfg = SceneData.Instance:SceneConfig()
	if scene_cfg and mapid and scene_cfg.mapid ~= mapid then
		local map_cfg = Config_scenelist[mapid]
		local x = (pos.x - map_cfg.edge_min_x) * 5
		local y = (pos.z - map_cfg.edge_min_z) * 5
		local re_x = math.roundtoint(x)
		local re_y = math.roundtoint(y)--math.floor(() * 5 + 0.5)
		return re_x,re_y
	end
	local x = (pos.x - self.terrain_edge_min.x) * 5
	local y = (pos.z - self.terrain_edge_min.z) * 5
    local re_x = math.roundtoint(x)
    local re_y = math.roundtoint(y)
	return re_x,re_y
end

function GameScene:GetPosHeight(x,z,y)
	if self.terrain_collider then
		local ret = self.terrain_collider:GetHeight(x,z)
		if ret == 0 then
			for i=2,SceneManager.MaxResamplePer do
				Game.TerrainCollider.ResamplePrecision = i * SceneManager.DefaultResamplePrecision
				ret = self.terrain_collider:GetHeight(x,z)
				if ret ~= 0 then
					break
				end
			end
			Game.TerrainCollider.ResamplePrecision = SceneManager.DefaultResamplePrecision
			return ret,(ret~=0)
		else
			return ret,false
		end
	else
		return 0,false
	end
end
function GameScene:GetHeight(x, z)
	if self.terrain_collider then
		local ret = self.terrain_collider:GetHeight(x,z)
		return ret
	end
	return 0
end

function GameScene.SamplePos(pos)
	return Game.TerrainCollider.SamplePos(pos)
end

function GameScene:Update()
	if MainPlayer ~= nil then
		local player_p = MainPlayer:GetPos()
		if not player_p:Equals(self.player_last_pos) then
			self.player_last_pos:Set(player_p.x,player_p.y,player_p.z)
			self:onMainPlayerPosChange(player_p)
			EventSys:Fire(GlobalEventKeys.RolePosChange)
		end
	end
	self:updateTouch()
end

function GameScene:updateTouch()
	local touch_up = UnityTouchState.IsTouchUp()
	local touch_down = UnityTouchState.IsTouchDown()
	local touch_ui = UnityTouchState.IsTouchOnUI()

	if RoleSettingData.Instance:GetServerData(ROLE_SETTING_TYPE.SET_AUTO_RESEARCH_TASK) ~= 0 then
		self:ResearchTaskAuto()
	end
	self:ResearchDailyDegree()
	self:ResearchGuildLetter()
	GmData.Instance:CameraFollow()
	if self.is_touch_begin then
		if touch_up then
			GmData.Instance:CheckAndOpen()
			if not self:GmWatch() then
				EventSys:Fire(GlobalEventKeys.TOUCH_ENDED)
			end
			self.is_touch_begin = false
			self.is_touch_moving = false
			self.touch_began_time = Time.realtimeSinceStartup
			self.first_touch_point = nil
		else
			if Input.touchCount >= 2 then
				self.first_touch_point = nil
			end
			if self.first_touch_point then
				local dis = Vector3.Distance(self.first_touch_point, Input.mousePosition)
				if dis > 1 then
					GmData.Instance:updateTouchRotate()
				end
			end
			GmData.Instance:updateTouchTranslate()
		end
		if not self:GmWatch() and self.is_touch_begin and Time.realtimeSinceStartup - self.touch_began_time > 0.3 then--添加长按判断
			EventSys:Fire(GlobalEventKeys.TOUCH_MOVED)
			self.is_touch_moving = true
		end
	else
		if not touch_down then
			return
		end
		if touch_ui then
			self.free_time = Time.realtimeSinceStartup
			--转换坐标然后播放特效
			PublicPopupData.Instance:ClickData().position = Input.mousePosition	
			ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.CommonUI)
			return
		end
		if touch_down then
			self.free_time = Time.realtimeSinceStartup
			if self:ModuleTouchBlock() then
				return
			end
			ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.Scene)
			self.is_touch_begin = true
			self.is_touch_moving = false
			self.touch_began_time = Time.realtimeSinceStartup
			EventSys:Fire(GlobalEventKeys.TOUCH_BEGAN)
			self.first_touch_point = Input.mousePosition
			return
		end
	end
end
function GameScene:GmWatch()
	return GmData.Instance.view_data.free_move == true
end
--检查自动三界
function GameScene:ResearchTaskAuto()
	local surplus = ResearchTaskData.Instance:Time()
	local level = Config.daily_activity_auto.daily[ActType.ResearchTask].level_restrict
	local role_level = RoleData.Instance:GetRoleLevel()
	local start_time = Config.robot_auto.other[1].start_time
	local is_in_team = TeamData.Instance:InTeam()
	local mode = SceneData.Instance:GetMode()
	if is_in_team == false and mode == SceneModeType.None and role_level > level and surplus ~= 0 and TaskData.Instance:GetTaskInfoByTaskId(CUSTOM_OBJID.RESEARCH_TASK) == nil
	and Time.realtimeSinceStartup - self.free_time > start_time then
		-- ResearchTaskCtrl.SendResearchTaskAuto()
		ViewMgr:OpenView(AutoResearchTip)
		self.free_time = Time.realtimeSinceStartup
	end
end

function GameScene:ResearchDailyDegree()
	local start_time = Config.activedegree_auto.other[1].start_time or 600
	if Time.realtimeSinceStartup - self.free_time > start_time then
		if SceneData.Instance:GetMode() == SCENE_TYPE.COMMON_SCENE
			or SceneData.Instance:GetMode() == SCENE_TYPE.GUAJI then

			if not TeamData.Instance:InTeam() or not TeamData.Instance:IsFollow() then

				ActivityCtrl.Instance:CheckAndOpenNextNotice()
			end
		end
		self.free_time = Time.realtimeSinceStartup
	end
end

--检查家族主动邀请
function GameScene:ResearchGuildLetter()
	if GuildData.Instance:IsGuildLetterFlag() then
		return
	end
	if TeamData.Instance:InTeam() or SceneData.Instance:GetMode() ~= SceneModeType.None or self:FreeTime() < GuildData.Instance:GuildLetterFreeTime() then
		return
	end
	if GuideManager.Instance:IsGuide() then
		return
	end
	if MainPlayer == nil or not MainPlayer:IsInAnim(ChAnimType.Idle) then
		return
	end
	if AutoInvokerCtrl.Instance:AutoState() ~= AutoCommandState.None then
		return
	end
	if FunOpen.Instance:GetFunIsOpened(Mod.Guild.GuildInfo) then
		GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.REQ_GUILD_RECOMMEND})
		GuildData.Instance:SetGuildLetterFlag()
		-- self.free_time = Time.realtimeSinceStartup
	end
end

function GameScene:FreeTime()
	return Time.realtimeSinceStartup - self.free_time
end

--可拓展位置，被模块影响无法移动，屏蔽点击事件
function GameScene:ModuleTouchBlock()
	if FightData.Instance:SoulStoryPlaying() then
		return true
	end
	if LookingForRabbitsData.Instance:IsInGame() then
		return true
	end
	if MainViewData.Instance:BaseData().is_battle == true then
		return true
	end
	if MarryData.Instance:IsParadeFloat() then
        -- PublicPopupCtrl.Instance:Center(Language.Marry.ParadeFloatTip)
        return true
    end
	if TeamData.Instance:NeedFollow() then
		PublicPopupCtrl.Instance:Center(Language.Team.TipsMove)
		return true 
	end
	if MazeRunData.Instance:IsMoveFreeze() then
		PublicPopupCtrl.Instance:Center(Language.MazeRun.CantMove)
		return true
	end
	if TombRaiderData.Instance:IsLimitedMove() then
		PublicPopupCtrl.Instance:Center(TombRaiderData.Instance:GetFreezeReason())
		return true
	end
	if LiveData.Instance:IsRefine() and ViewMgr:IsOpen(GatherView) then
		local info = GatherData.Instance:GetGatherInfo()
		local config = GatherData.Instance:GetGatherConfig(info.desc)
		PublicPopupCtrl.Instance:Center(string.format(Language.Live.RefineTip,config.text))
		return true
	end
	if GatherData.IsGathering() or ViewMgr:IsOpen(GatherView) then
		local info = GatherData.Instance:GetGatherInfo()
		local tips = info.desc == FightConfig.FightTabType.irony and Language.MainOther.IronyNotTip or Language.MainOther.GatherNotTip
		PublicPopupCtrl.Instance:Center(tips)
		return true
	end
	if MainViewCtrl.Instance:IsInGameState(MainViewGameState.animal_race) then
        return true
	end
	local mode = SceneData.Instance:GetMode()
	if mode == SceneModeType.StarLife then
		return true
	elseif mode == SceneModeType.LoversMatch then
		local can_move = LoversMatchData.Instance:CanMove()
		if not can_move then
			PublicPopupCtrl.Instance:Center(Language.LoversMatch.FBError4)
		end
		return not can_move
	end
	if StoryData.Instance.is_playing_story == true then
		return true
	end
	return false
end

function GameScene:onMainPlayerPosChange(player_pos)
	--local player_pos = MainPlayer:GetPos()
	if SceneLogic.Instance:MoveCache().guaji == true then
		return 
	end
	--[[ if player_pos.x == 0 and player_pos.z == 0 then
		SceneCtrl.Instance:EscapeFromCurrentPosition()
	end ]]
	local in_door = nil
	SceneCtrl.Instance:Objs():ForeachDoor(function(door)
			local door_pos = door:GetPos()
			if GameMath.GetDistance(player_pos.x, player_pos.z, door_pos.x, door_pos.z, false) < 3 then--1.69 then
				in_door = door
				if self.is_in_door == false then
					self.is_in_door = true
					self.is_touch_begin = false
					MainPlayer:StopMove()
					door.enter_handle()
				end
				return true
			else
				return false
			end
		end)
	if in_door == nil then
		self.is_in_door = false
	end
end

function GameScene:SceneId()
	return self.scene_id
end
function GameScene:IsTouchMoving()
	return self.is_touch_moving
end
-- 强制终止指令输入
function GameScene:StopTouchMoving()
	self.is_touch_begin = false
end

function GameScene:CameraPosCheck()
	-- statements
end

function GameScene:SetMainVirtualCamera(scene_id,vir_cam_obj,vir_cam)
	self.vir_cam_obj = vir_cam_obj
	local show_virtual = MPNEEDVIRTUALCAMERA[scene_id]
	if show_virtual then
		vir_cam.Priority = 10
		vir_cam.Follow = nil
		if scene_id == 326 then
			MainPlayer:BindVirtualCamera(vir_cam)
		end
	else
		MainPlayer:BindVirtualCamera(vir_cam)
		vir_cam.Priority = 1
	end
end

function GameScene:MainVirtualCameraAnimation(clipName,func)
	if self.camera_anim_handle then
		Runner.Instance:RemoveRunUntilTrue(self.camera_anim_handle)
		self.camera_anim_handle = nil
	end
	local virCamAnim = self.vir_cam_obj:GetOrAddComponent(typeof(UnityEngine.Animation))	
	local animObj = ClipGeter.GetClip(clipName)
	virCamAnim.playAutomatically = false
	virCamAnim.clip = animObj
	virCamAnim:AddClip(animObj, animObj.name)
	virCamAnim:Play()
	self.camera_anim_handle = Runner.Instance:RunUntilTrue(function ()
		if virCamAnim.isPlaying == false then
			if func then
				func()
			end
			return true
		end
		return false
	end)
end

function GameScene:ForceStopMainVirtualCameraAnimation()
    if self.camera_anim_handle then
        Runner.Instance:RemoveRunUntilTrue(self.camera_anim_handle)
        self.camera_anim_handle = nil
    end
    local vir_cam = self.vir_cam_obj
    if vir_cam then
        local virCamAnim = vir_cam:GetOrAddComponent(typeof(UnityEngine.Animation))
        virCamAnim:Stop()
    end
end


function GameScene:ReadyComplete()
    return ViewMgr:IsOpen(MainView) and self.scene_loaded
end