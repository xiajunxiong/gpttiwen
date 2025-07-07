RoleCreateScene = RoleCreateScene or BaseClass(BaseScene)

function RoleCreateScene:__init()
	self.video_com = false
end

function RoleCreateScene:__delete()
	
end

function RoleCreateScene:StartLoading()
	ViewMgr:MainViewVisible(false)
	local role_num = #LoginData.Instance.account.login_roles
	self.need_video_view = role_num <= 0
	PublicPopupCtrl.Instance:HideWait()
	if self.need_video_view then
			RoleCreateData.Instance:LockUpDirector(true)
		-- if VersionTag.HasTag(VersionTag.ShowCGSupport) then
			VideoCtrl.Instance:StartVideoById(4,function()
				MainProber:Step(MainProber.STEP_ROLECREATECG_END,MainProber.user_id)
				RoleCreateData.Instance:UnLockDirector()
				self.video_com = true
			end)
			-- ViewMgr:OpenView(RoleCreateVideoApiView)
		-- else
		-- 	ViewMgr:OpenView(RoleCreateVideoView)
		-- end
	else
		AudioMgr:PlayBg("2000")
		ViewMgr:CloseView(WelcomeView)
	end
	ViewMgr:OpenView(RoleCreateView)
	ViewMgr:OpenView(TopLayerView)


	self.scene_loaded = false

	SceneMgr:LoadScene("JSCJ_1",function(error)
		if self:Disposed() then
			return
		end
		if not StringIsEmpty(error) then
			Debuger.LogError("GameScene Load Scene failed===JSCJ_1")
		else

		-- 	local vir_cam_obj = SceneMgr.FindGameObjectWithTag("MainVirtualCamera")
		-- 	local vir_cam = vir_cam_obj:GetComponent(typeof(Cinemachine.CinemachineVirtualCamera))
		-- 	MainPlayer:BindVirtualCamera(vir_cam)

		-- 	local ter_col_obj = SceneMgr.FindGameObjectWithTag("TerrainCollider")
		-- 	self.terrain_collider = ter_col_obj:GetComponent(typeof(Game.TerrainCollider))
		-- 	self.terrain_edge_min = self.terrain_collider.EdgeMinPos
		-- 	self.terrain_edge_max = self.terrain_collider.EdgeMaxPos

		-- 	local quality_obj = SceneMgr.FindGameObjectWithTag("Quality")
		-- 	self.quality = quality_obj:GetComponent(typeof(Crystal.Quality))
		-- 	SceneData.Instance:Base().scene_name = SceneData.Instance:SceneName()

		-- 	local main_camera = UnityGameObj.FindGameObjectWithTag("MainCamera")
		-- 	local post_effect = main_camera:GetComponent(typeof(Crystal.PostEffectDepthOfField))
		-- 	post_effect.enabled = false
		end
		-- SceneCtrl.Instance:Objs():OnRoleCreateSceneLoaded()
		self:OnRoleCreateSceneLoaded()
		-- SceneCtrl.Instance:Logic():OnGameSceneLoaded(SceneMgr:LoadedSceneId())
		-- MainViewData.Instance:SetBattle(false)
		self.scene_loaded = true
		-- SE:Play(SceneEffectConfig.EffectList.RoleCreate, nil,nil,  MainPlayer.root_node)
	end)
end

function RoleCreateScene:LoadingComplete()
	if self.need_video_view then
		if not ViewMgr:IsOpen(VideoView) and not self.video_com then
			return false
		else
			if ViewMgr:IsOpen(RoleCreateView) and ViewMgr:IsOpen(TopLayerView) and self.scene_loaded then
				if self.delay_close_welcome == nil then
					self.delay_close_welcome = TimeHelper:AddDelayFrameTimer(function ()
						ViewMgr:CloseView(WelcomeView)
					end,10)
				end
				return true
			else
				return false
			end
		end
	else
		local load_com = ViewMgr:IsOpen(RoleCreateView) and ViewMgr:IsOpen(TopLayerView) and self.scene_loaded
		if load_com then
			RoleCreateData.Instance:UnLockDirector()
		end
		return load_com
	end
end

function RoleCreateScene:Dispose()
	self.director = nil
	BaseScene.Dispose(self)
	ViewMgr:CloseView(RoleCreateView)
end

function RoleCreateScene:OnRoleCreateSceneLoaded()
	local director_obj = SceneMgr.FindGameObjectWithTag("RoleCreateDirector")
	self.director = director_obj:GetLuaComponent(RoleCreateDirector:GetTypeName())
	self.director:ResetDirector()
	MainProber:Step(MainProber.STEP_ROLECREATE_SCENELOAD_END, MainProber.user_id)
end

function RoleCreateScene:RoleCreateShow(avatar_type)
	self.director:RoleCreateShow(avatar_type)
end

function RoleCreateScene:RoleCreateShowProf(prof_index)
	self.director:RoleCreateShowProf(prof_index)
end

function RoleCreateScene:StopCreate(value)
	self.director:StopAudio()
	self.director:SetPostEffectEnable(value)
end

function RoleCreateScene:PauseCreate()
	self.director:StopAudio()
	self.director:SetPostEffectEnable(false)
	self.director:ResetDirector()
end

RoleCreateDirector = RoleCreateDirector or DeclareMono("RoleCreateDirector")

function RoleCreateDirector:RoleCreateDirector()
	self.anim_handles_cam = {}
	self.anim_handles_role = {}
	for i = 1, self.CameraAnims:Length() do
		self.anim_handles_cam[i] = self.CameraAnims[i]:ListenEvent("show", function(eve_param,anim_st_info)
			if "end" == eve_param then
				RoleCreateData.Instance:ShowRight(true)
			end
		end)
	end

	self.touch_helper = Game.SceneTouchHelper
	Runner.Instance:AddRunObj(self, 3)
	self.is_touch_begin = false
	self.avatar_type = 0
	self.prof_index = 0
	self.show_weapon_id = 0

	for i = 1, self.RoleAnims:Length() do
		self.anim_handles_role [i] = self.RoleAnims[i]:ListenEvent("IdleEnter", function (eve_param)
			local value = tonumber(eve_param)
			value = 5 == value and RoleCreateConfig.DefaultProf[self.avatar_type] or value
			RoleCreateData.Instance:ModelRotateShow(true, self.RoleAnims[i], self["ModelRotate" .. self.avatar_type][value], self.ModelOffsets[self.avatar_type])
		end)
	end
end

function RoleCreateDirector:OnDestroy()
	RoleCreateData.Instance:ResetModelRotateInfo()
    Runner.Instance:RemoveRunObj(self)

	for i = 1, self.CameraAnims:Length() do
		self.CameraAnims[i]:UnlistenEvent("show", self.anim_handles_cam[i])
	end
	for i = 1, self.RoleAnims:Length() do
		self.RoleAnims[i]:UnlistenEvent("IdleEnter", self.anim_handles_role[i])
	end
end

function RoleCreateDirector:RoleCreateShow(avatar_type)
	if self.avatar_type == avatar_type then return end
	for i = 1, self.RoleAnims:Length() do
		if i ~= avatar_type then self.RoleAnims[i]:SetObjActive(false) end
	end
	for i = 1, self.CameraAnims:Length() do
		if i ~= avatar_type then self.CameraAnims[i]:SetObjActive(false) end
	end
	for i = 1, self.RoleEffects:Length() do
		if i ~= avatar_type then self.RoleEffects[i]:SetActive(false) end
	end
	
	if not self.CameraAnims[avatar_type]:GetObjActive() then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
		self:StopAudio()
		self.handle = AudioMgr:PlayEffectHandle(AudioType.UI, "role_create" .. avatar_type)
		self.CameraAnims[avatar_type]:SetObjActive(true)
		-- self.RoleEffects[avatar_type]:SetActive(true)
		-- self:PlayRoleEffect(self.RoleEffects[avatar_type])
		self:SetPostEffectEnable(true)
	end

	self.RoleAnims[avatar_type]:SetObjActive(true)
	self.avatar_type = avatar_type
	self.prof_index = 0
	self:RoleCreateShowProf(RoleCreateConfig.DefaultProf[self.avatar_type], true)
end

function RoleCreateDirector:RoleCreateShowProf(prof_index, enter_show)
	if self.prof_index == prof_index then return end
	self.RoleEffects[self.avatar_type]:SetActive(enter_show)
	local fieldOfView = enter_show and (6 == self.avatar_type and 32 or 25) or 38
	self.Cameras[self.avatar_type].fieldOfView = fieldOfView
	local len = self.VCams[self.avatar_type].m_Lens
	len.FieldOfView = fieldOfView
	self.VCams[self.avatar_type].m_Lens = len

	local is_default_prof = RoleCreateConfig.DefaultProf[self.avatar_type] == prof_index
	self.prof_index = prof_index
	local show_weapon_id = self.avatar_type * 10 + self.prof_index
	self.RoleAnims[self.avatar_type]:SetInteger(APH("state"), (enter_show or not is_default_prof) and  prof_index or 5)

	local camera_trigger = "show" .. (enter_show and 2 or 1)
	self.CameraAnims[self.avatar_type]:SetTrigger(APH(camera_trigger))
	self.RoleAnims[self.avatar_type]:SetBool(APH("to_unloop"), true)
	RoleCreateData.Instance:ModelRotateShow(false)

	self:ShowWeaponById(self.show_weapon_id, false)
	self.show_weapon_id = show_weapon_id
	self:ShowWeaponById(self.show_weapon_id, true)

	local weapon_anim = RoleCreateConfig.WeaponShow[self.show_weapon_id]
	if weapon_anim then
		self.WeaponAnims[weapon_anim]:SetTrigger(APH("show"))
	end
end

function RoleCreateDirector:ShowWeaponById(show_weapon_id, value)
	if show_weapon_id > 0 then
		if RoleCreateConfig.Weapon2[show_weapon_id] then
			self["Weapon"..show_weapon_id.."1"]:SetActive(value)
			self["Weapon"..show_weapon_id.."2"]:SetActive(value)
		else
			self["Weapon"..show_weapon_id]:SetActive(value)
		end
	end
end

function RoleCreateDirector:PlayRoleEffect(effect)
	-- local particles = effect:GetComponentsInChildren(typeof(UnityEngine.ParticleSystem))
	-- if particles.Length > 0 then
	-- 	for i = 0, particles.Length - 1 do
	-- 		particles[i]:Simulate(0,false,true)
	-- 		particles[i]:Play()
    --     end
    -- end
	-- local anims = effect:GetComponentsInChildren(typeof(UnityEngine.Animation))
	-- if anims.Length > 0 then
	-- 	for i = 0, anims.Length - 1 do
	-- 		local anim = anims[i] 
	-- 		if anim.clip ~= nil then
	-- 			anim:Play(anim.clip.name)
	-- 		-- 	local animSt = anim[anim.clip.name]
	-- 		-- 	if animSt ~= nil then
	-- 		-- 		animSt.time = 0
	-- 		-- 	end
	-- 		end
    --     end
    -- end
end

function RoleCreateDirector:Update()
	if self.is_touch_begin then
		if self.touch_helper.IsTouchUp() then
			self.is_touch_begin = false
			local hit, pos, obj = self.touch_helper.Raycast(nil, nil)
			if not hit then return end
			if obj.layer == GameObjLayer.ColliderRole then
				local operate = obj:GetLuaComponent(RoleCreateDirectorItem:GetTypeName())
				if operate and operate.ClickItem then operate:ClickItem() end 
			end
		end
	else
		if not self.touch_helper.IsTouching() then
			return
		end
		if self.touch_helper.IsTouching() then
			self.is_touch_begin = true
		end
	end
end

function RoleCreateDirector:StopAudio()
	AudioMgr:StopEffectHandle(self.handle)
end

function RoleCreateDirector:ResetDirector()
	RoleCreateData.Instance:ResetModelRotateInfo()
	local avatar_type = RoleCreateData.Instance.create_view_data.appear_id
	if avatar_type and avatar_type > 0 then self.RoleAnims[avatar_type]:SetObjActive(false) end
	for i = 1, self.RoleAnims:Length() do
		-- if 2 ~= i then
			self.RoleAnims[i]:SetInteger(APH("state"), 0)
			-- self.RoleAnims[i]:SetObjActive(true)
		-- end
	end
	for i = 1, self.CameraAnims:Length() do
		self.CameraAnims[i]:SetObjActive(false)
	end
	for i = 1, self.RoleEffects:Length() do
		self.RoleEffects[i]:SetActive(false)
	end
	for i = 1, self.RoleItems:Length() do
		self.RoleItems[i]:ResetDirectorDirectorItem(i)
	end
	RoleCreateData.Instance:ShowRight(false)
	TimeHelper:AddDelayFrameTimer(function ()
		for i = 1, self.RoleAnims:Length() do
			-- if 2 ~= i then
				self.RoleAnims[i]:SetObjActive(true)
			-- end
		end
		-- RoleCreateData.Instance:ResetAppearId()
		-- RoleCreateData.Instance.cur_sel_role:Set{}
	end)

	self:ShowWeaponById(self.show_weapon_id, false)
	self.show_weapon_id = 6 * 10 + 2
	self:ShowWeaponById(self.show_weapon_id, true)
	self.avatar_type = 0
end

function RoleCreateDirector:SetPostEffectEnable(value)
    if nil == self.post_effect then
        local main_camera = UnityGameObj.FindGameObjectWithTag("MainCamera")
		self.post_effect = main_camera:GetComponent(typeof(Crystal.PostEffectDepthOfField))
		self.post_effect_s = {
			focusDistance = self.post_effect.focusDistance,
			blendDistance = self.post_effect.blendDistance
		}
    end
	self.post_effect.enabled = value
	if not value then
		self.post_effect.focusDistance = self.post_effect_s.focusDistance
		self.post_effect.blendDistance = self.post_effect_s.blendDistance
	else
		self.post_effect.focusDistance = 10
		self.post_effect.blendDistance = 10
	end
end


RoleCreateDirectorItem = RoleCreateDirectorItem or DeclareMono("RoleCreateDirectorItem")

function RoleCreateDirectorItem:ClickItem()
	-- LogError("ClickItem",RoleCreateData.Instance:GetIsLockDirector() )
	if RoleCreateData.Instance:GetIsLockDirector() then return end 
	
	if RoleCreateData.Instance.create_director_data.appear_id == self.avatar_type 
		or RoleCreateData.Instance:ShowRight() then return end
	
	LoginCtrl.Instance:ClickDirectorAvatarType(self.avatar_type)
end

function RoleCreateDirectorItem:ResetDirectorDirectorItem(avatar_type)
	self.avatar_type = avatar_type
end