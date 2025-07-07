UIChDrawer = UIChDrawer or BaseClass(UIObjDrawer)

local body_keys = {
    "main_body",
    "ride_body",
    "wing_body",
    "l_weapon_body",
    "r_weapon_body"
}

function UIChDrawer:__init()
    -- self.root_obj = root_obj
    -- self.shadow_body = self.shadow_body or BodyObj.New("actor/Shadow.prefab", self.root_obj)

    self.main_body = nil
    self.main_path = nil
    self.main_anim = nil
    self.main_res_id = nil

    self.ride_body = nil
    self.ride_anim = nil
    self.ride_res_id = 0

    self.wing_body = nil
    self.wing_res_id = nil

    self.l_weapon_body = nil
    self.l_weapon_res_id = 0
    self.r_weapon_body = nil
    self.r_weapon_res_id = 0

    self.body_keys = body_keys
    self.special_show = false

    self.anim_delay_handle = nil --有资源在加载时延迟执行播放动作的timerHandle
    self.ride_anim_delay_handle = nil --有资源在加载时延迟执行播放动作的timerHandle

    self.is_ride = false
    self.battle_state = false
end

function UIChDrawer.CopyMainRoleData()
    local main_role = UIChDrawer.New()
    main_role:SetData(RoleData.Instance:GetApearance())
    return main_role
end

function UIChDrawer.CreateByAppearance(appearance)
    local drawer = UIChDrawer.New()
    drawer:SetData(appearance)
    return drawer
end

-- auto_weapon 幻化界面要用到 当转换avatar_type时自动帮他转换成改avatar_type所拥有的武器
function UIChDrawer:SetData(appearance, auto_weapon, fly_flag)
    fly_flag = fly_flag or appearance.fly_flag
    local marbas_flag = appearance.zaohua_app_id or 0
    if appearance.battle_state ~= nil then
        self.battle_state = appearance.battle_state
    end
    self:CheckEnabled(
        "data",
        function()
            if appearance.special_appearance_type and appearance.special_appearance_type == 1 and appearance.special_appearance_param < 999 then
                -- self:SetMain(appearance.special_appearance_param, nil, true)
                local config = PartnerData.Instance:GetPartnerInfoById(appearance.special_appearance_param)
                if config then
                    self:SetMain(config.info.res_id, nil, true, fly_flag,marbas_flag)
                end
            else
                local special_show = appearance.special_show
                if special_show then
                    self:SetMain(special_show.main_res, special_show.main_color, nil, fly_flag,marbas_flag)
                    if special_show.show_weapon then
                        self:SetWeapon(special_show.weapon_res1, special_show.weapon_res2)
                    else
                        special_show.show_weapon = true
                        self:SetWeapon()
                    end
                else
                    self.r_weapon_res_id = 0
                    self.l_weapon_res_id = 0
                    self:SetMain(ResHelper.RoleResId(appearance.avatar_type, appearance.fashion_body_id, appearance.advance_time), (appearance.fashion_body_id or 0) > 0 and 0 or appearance.color,nil, fly_flag,marbas_flag)
                    if appearance.weapon_id and appearance.weapon_id > 0 then
                        local weapon = Config.equipment_auto[appearance.weapon_id]
                        self:SetWeapon(ResHelper.WeaponResId(weapon.weapon_type, appearance.fashion_weapon_id,nil,nil,appearance.advance_time,auto_weapon))
                    else
                        self:SetWeapon()
                    end
                    self.ride_res_id = 0
                    self:SetRide(self.is_ride and appearance.ride_id or 0)
                    if self.is_ride and appearance.ride_id and appearance.ride_id > 0 then
                        self.center_point_offset_cache = Vector3.zero
                    else
                        self.center_point_offset_cache = nil
                    end
                    if not self.main_body:IsLoading() then
                        self:FlushCenterPointFollow()
                    end
                end
            end
        end
    )
end
function UIChDrawer:DelWeapon()
    if self.l_weapon_body then
        self.l_weapon_body:DeleteMe()
        self.l_weapon_body = nil
    end
    if self.r_weapon_body then
        self.r_weapon_body:DeleteMe()
        self.r_weapon_body = nil
    end
end
function UIChDrawer:UpdateAnimator(time)
    for _, key in pairs(self.body_keys) do
        if self[key] then
            self[key].anim:UpdateAnimator(time)
        end
    end
end

function UIChDrawer:SetMainSolo(path, fly_flag,marbas_flag)
    for _, key in pairs(self.body_keys) do
        if self[key] then
            self[key]:DeleteMe()
            self[key] = nil
        end
    end
    UIObjDrawer.SetData(self, path, fly_flag,marbas_flag)
    -- if type(res_id) == "string" then
    -- else
    --     self:SetMain(res_id)
    -- end
end

function UIChDrawer:SetMain(res_id, res_color, special, fly_flay,marbas_flag)
    if not res_id then
        self.main_body:DeleteMe()
        self.main_body = nil
        return
    end
    self.main_res_id = res_id
    local path  --= DrawerHelper.GetRolePath(res_id, res_color)
    if special then
        self.special_show = true
        path = DrawerHelper.GetNpcPath(res_id)
    else
        self.special_show = false
        path = DrawerHelper.GetRolePath(res_id, res_color)
    end
    if not path then
        return
    end
    -- LogError(path)
    self.main_path = path
    if self.main_body ~= nil then
        if path == self.main_body:Path() then
            if fly_flay ~= self.cache_fly_flag or marbas_flag ~= self.cache_marbas_flag then
                -- LogError("!?@teyewh",fly_flay,marbas_flag)
                self:SetFlyEffect(fly_flay,marbas_flag)
            end
            return
        end
        self.main_body:DeleteMe()
    end

    self.main_body = BodyObj.New(path, self.root_obj, true)
    self:flushEventNode(self.main_body:GetLoadEventNode())
    self.main_anim = self.main_body:Anim()
    if self.cache_main_anim then
        self.main_anim:PlayAnim(self.cache_main_anim, self.anim_params)
        if self.weapon_anim then
            self.weapon_anim:PlayAnim(self.cache_main_anim)
        end
        self.cache_main_anim = nil
    end

    if self.wing_body ~= nil then
        self.main_body:AttachBody(ChAttachKeyName.Back, self.wing_body)
    end

    if self.ride_body ~= nil then
        self.main_body:AttachToParent(ChAttachKeyName.Mount, self.ride_body)
    end
	
	self.main_body:SetLoadedCallback(function ()
		local need_overeide_clip = DrawerHelper.GetWeaponIsNeedOverrideClip(self.r_weapon_res_id)
		if need_overeide_clip then
			self:flushWeaponBodyAnimOverride()
		end
        self:FlushCenterPointFollow()
        self:SetFlyEffect(fly_flay,marbas_flag)
	end)

    self:flushMainBodyAnimOverride()
    -- self:FlushModelCameraDataKey()
end

function UIChDrawer:SetRide(res_id, show_end_func)
    res_id = res_id or 0
    if 0 == res_id then
        if self.ride_body ~= nil then
            self.ride_body:DeleteMe()
            self.ride_body = nil
        end
        -- return
    end
    self.ride_res_id = res_id
    local path = DrawerHelper.GetRidePath(res_id)
    --[[ if not path then
        return
    end ]]
    if self.ride_body ~= nil then
        if path == self.ride_body:Path() then
            self.main_path = path
            -- self:flushShow()
            -- return
        end
        -- LogError("DelRide")
        self.ride_body:DeleteMe()
    end

    if path == nil then
        self.ride_body = nil
        self.ride_anim = nil
        self.main_body:ResetTransform()
    else
        -- LogError(path)
        self.main_path = path
        -- LogError("SetRide")
        self.ride_body = BodyObj.New(path, self.root_obj)
        self:flushEventNode(self.ride_body:GetLoadEventNode())
        self.ride_anim = self.ride_body:Anim()
        if nil ~= show_end_func then
            self.ride_body:SetLoadedCallback(function ()
                self.ride_anim:WaitAnimEnd(ChAnimType.Show, show_end_func)
            end)
        end
        if self.cache_ride_anim then
            self.ride_anim:PlayAnim(self.cache_ride_anim)
            if self.weapon_anim then
                self.weapon_anim:PlayAnim(self.cache_ride_anim)
            end
            self.cache_ride_anim = nil
        end
        self.ride_body:AttachBody(ChAttachKeyName.Mount, self.main_body)
    end
    self:flushMainBodyAnimOverride()
    -- self:flushWeaponAttach()
    self:flushShow()
end

function UIChDrawer:SetWing(res_id)
    if not res_id then
        self.wing_body:DeleteMe()
        self.wing_body = nil
        return
    end
    self.wing_res_id = res_id
    local path = DrawerHelper.GetWingPath(self.wing_res_id)
    if not path then
        return
    end
    if self.wing_body ~= nil then
        if path == self.wing_body:Path() then
            return
        end
        self.wing_body:DeleteMe()
    end
    if path == nil then
        self.wing_body = nil
        return
    end
    self.wing_body = BodyObj.New(path, self.root_obj)
    self:flushEventNode(self.wing_body:GetLoadEventNode())

    self.wing_body:AttachToParent(ChAttachKeyName.Back, self.main_body)
end

function UIChDrawer:SetWeapon(res_id_r, res_id_l)
    local need_flush_attach = false
    if not res_id_l or res_id_l == 0 then
        if self.l_weapon_body then
            self.l_weapon_body:DeleteMe()
            self.l_weapon_body = nil
        end
        self.l_weapon_res_id = 0
    else
        self.l_weapon_res_id = res_id_l
        local l_path = DrawerHelper.GetWeaponPath(self.l_weapon_res_id)
        if l_path then
            if self.l_weapon_body and self.l_weapon_body:Path() ~= l_path then
                self.l_weapon_body:DeleteMe()
                self.l_weapon_body = nil
            end
            if not self.l_weapon_body then
                self.l_weapon_body = BodyObj.New(l_path, self.root_obj)
                self:flushEventNode(self.l_weapon_body:GetLoadEventNode())
            end
            local type_l, par_key = DrawerHelper.GetWeaponTypeAndKey(self.l_weapon_res_id)
            self.l_weapon_body:AttachToParent(par_key, self.main_body)
            need_flush_attach = true
        end
    end
    if not res_id_r or res_id_r == 0 then
        if self.r_weapon_body then
            self.r_weapon_body:DeleteMe()
            self.r_weapon_body = nil
        end
        self.r_weapon_res_id = 0
    else
        self.r_weapon_res_id = res_id_r
        local r_path = DrawerHelper.GetWeaponPath(self.r_weapon_res_id)
        if r_path then
            if self.r_weapon_body and self.r_weapon_body:Path() ~= r_path then
                self.r_weapon_body:DeleteMe()
                self.r_weapon_body = nil
            end
            if not self.r_weapon_body then
                self.r_weapon_body = BodyObj.New(r_path, self.root_obj)
                self:flushEventNode(self.r_weapon_body:GetLoadEventNode())
				self.r_weapon_body:SetLoadedCallback(function ()
					local need_overeide_clip = DrawerHelper.GetWeaponIsNeedOverrideClip(self.r_weapon_res_id)
					if need_overeide_clip then
						self:flushWeaponBodyAnimOverride()
					end
				end)
            end
            local type_r, par_key = DrawerHelper.GetWeaponTypeAndKey(self.r_weapon_res_id)
            self.r_weapon_body:AttachToParent(par_key, self.main_body)
            need_flush_attach = true
        end
    end
    if need_flush_attach then
        -- self:flushWeaponAttach()
        self:flushShow()
    end
    self:flushMainBodyAnimOverride()
end

function UIChDrawer:__delete()
    if self.anim_delay_handle ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.anim_delay_handle)
        self.anim_delay_handle = nil
    end
    if self.ride_anim_delay_handle ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.ride_anim_delay_handle)
        self.ride_anim_delay_handle = nil
    end
    if self.main_body ~= nil then
        self.main_body:DeleteMe()
        self.main_body = nil
        self.main_anim = nil
    end

    if self.ride_body ~= nil then
        self.ride_body:DeleteMe()
        self.ride_body = nil
        self.ride_anim = nil
    end

    if self.wing_body ~= nil then
        self.wing_body:DeleteMe()
        self.wing_body = nil
    end

    if self.l_weapon_body ~= nil then
        self.l_weapon_body:DeleteMe()
        self.l_weapon_body = nil
    end

    if self.r_weapon_body ~= nil then
        self.r_weapon_body:DeleteMe()
        self.r_weapon_body = nil
    end
end

-- function UIChDrawer:playAnimInternal(anim)
--     if self.main_anim ~= nil then
--         self.main_anim:PlayAnim(anim)
--     end
--     if self.ride_anim ~= nil then
--         self.ride_anim:PlayAnim(anim)
--     end
-- end


function UIChDrawer:PlayAnim(anim)
    if self.anim_delay_handle ~= nil then 
        Runner.Instance:RemoveRunUntilTrue(self.anim_delay_handle)
        self.anim_delay_handle = nil
    end
    if self.ride_anim_delay_handle ~= nil then 
        Runner.Instance:RemoveRunUntilTrue(self.ride_anim_delay_handle)
        self.ride_anim_delay_handle = nil
    end
    if self.main_anim ~= nil then
        if self.main_body:IsLoading() or self.main_anim:IsOverriderLoading() then
            self.anim_delay_handle = Runner.Instance:RunUntilTrue(
                function()
                    if not self.main_body:IsLoading() and not self.main_anim:IsOverriderLoading() then
                        self.anim_delay_handle = nil
                        self.main_anim:PlayAnim(anim, self.anim_params)
                        if self.weapon_anim then
                            self.weapon_anim:PlayAnim(anim)
                        end
                        return true
                    end
                    return false
                end,
            false)
        else
            self.main_anim:PlayAnim(anim, self.anim_params)
            if self.weapon_anim then
                self.weapon_anim:PlayAnim(anim)
            end
        end
    else
        self.cache_main_anim = anim
    end
    if self.ride_anim ~= nil then
        if self.ride_body:IsLoading() then
            self.ride_anim_delay_handle = Runner.Instance:RunUntilTrue(
                function()
                    if self.ride_body ~= nil and not self.ride_body:IsLoading() then
                        self.ride_anim_delay_handle = nil
                        self.ride_anim:PlayAnim(anim)
                        if self.weapon_anim then
                            self.weapon_anim:PlayAnim(anim)
                        end
                        return true
                    end
                    return false
                end,
            false)
        else
            self.ride_anim:PlayAnim(anim)
            if self.weapon_anim then
                self.weapon_anim:PlayAnim(anim)
            end
        end
    else
        self.cache_ride_anim = anim
    end
end

function UIChDrawer:flushMainBodyAnimOverride()
    if self.special_show == true then
        return
    end
    if not DrawerHelper.IsMainResMatchWeapon(self.main_res_id,self.r_weapon_res_id) then
        return
    end
    local override_path
    if self.battle_state then
        override_path = DrawerHelper.GetBattleWeaponOverridePath(self.main_res_id, self.l_weapon_res_id, self.r_weapon_res_id)
    elseif self.ride_res_id > 0 then
        override_path = DrawerHelper.GetRideOverridePath(self.main_res_id, self.ride_res_id, 0, 0)
    else --武器动画override
        override_path = DrawerHelper.GetSceneWeaponOverridePath(self.main_res_id, self.l_weapon_res_id, self.r_weapon_res_id)
    end
    local ao = self.main_anim:SetAnimOverride(override_path,need_overeide_clip)
    if ao then
        self:flushEventNode(ao)
    end
	local need_overeide_clip = DrawerHelper.GetWeaponIsNeedOverrideClip(self.r_weapon_res_id)
	if need_overeide_clip then
		self:flushWeaponBodyAnimOverride()
	end	
end

function UIChDrawer:flushWeaponBodyAnimOverride()
	if self.special_show == true then return end
    if not DrawerHelper.IsMainResMatchWeapon(self.main_res_id,self.r_weapon_res_id) then
        return
    end
	if self.r_weapon_body ~= nil and self.r_weapon_res_id > 0 then
	    local override_path
        if self.battle_state then
            override_path = DrawerHelper.GetBattleWeaponAnimationClipOverridePath(self.r_weapon_res_id,self.main_res_id)
		elseif self.ride_res_id > 0 then
			override_path = DrawerHelper.GetRideSceneWeaponAnimationClipOverridePath(self.r_weapon_res_id, self.main_res_id)
		else 
			override_path = DrawerHelper.GetSceneWeaponAnimationClipOverridePath(self.r_weapon_res_id,self.main_res_id)
		end
		local weapon_anim = self.r_weapon_body:Anim()
		weapon_anim:SetAnimOverride(override_path)
        weapon_anim:SetAnimSynchronizer(self.main_anim.animator)
        self.weapon_anim = weapon_anim
    else
        self.weapon_anim = nil
	end
end

function UIChDrawer:flushShow()
    local is_show = self.ride_res_id <= 0
    if self.main_body ~= nil then
        self.main_body:SetShow(self.is_ride or is_show)
    end

    if self.ride_body ~= nil then
        self.ride_body:SetShow(not is_show)
    end

    if self.r_weapon_body ~= nil then
        self.r_weapon_body:SetShow(is_show)
    end

    if self.l_weapon_body ~= nil then
        self.l_weapon_body:SetShow(is_show)
    end
    self:FlushFlyEffectShow(self.is_ride)
end

------------------废弃，根据不同的坐骑把武器挂载到不同的角色骨骼点上------------

function UIChDrawer:flushWeaponAttach()
    local l_key = nil
    local r_key = nil
    if self.ride_res_id <= 0 then
        l_key = ChAttachKeyName.LeftHand
        r_key = ChAttachKeyName.RightHand
    else
        local ride_type = math.floor(self.ride_res_id / 1000)
        if self.l_weapon_res_id > 0 then
            local weapon_type_l = math.floor(self.l_weapon_res_id / 10000)
            l_key = UIChDrawer.getWeaponAttachKey(weapon_type_l, ride_type, true)
        end
        if self.r_weapon_res_id > 0 then
            local weapon_type_r = math.floor(self.r_weapon_res_id / 10000)
            r_key = UIChDrawer.getWeaponAttachKey(weapon_type_r, ride_type, false)
        end
    end

    if self.l_weapon_body ~= nil then
        self.l_weapon_body:AttachToParent(l_key, self.main_body)
    end

    if self.r_weapon_body ~= nil then
        self.r_weapon_body:AttachToParent(r_key, self.main_body)
    end
end

local weapon_ride_attach_cfg = {
    [WEAPON_TYPE.SWORD] = {
        [RIDE_TYPE.TYPE3] = {r = ChAttachKeyName.Back}
    },
    [WEAPON_TYPE.AXE] = {
        [RIDE_TYPE.TYPE3] = {r = ChAttachKeyName.Back}
    },
    [WEAPON_TYPE.GLOVES] = {
        [RIDE_TYPE.TYPE1] = {l = ChAttachKeyName.LeftWaist, r = ChAttachKeyName.RightWaist},
        [RIDE_TYPE.TYPE3] = {l = ChAttachKeyName.Back, r = ChAttachKeyName.Back}
    }
}

function UIChDrawer.getWeaponAttachKey(weapon_type, ride_type, is_l)
    local att_key = nil
    local weapon_cfg = weapon_ride_attach_cfg[weapon_type]
    if weapon_cfg ~= nil then
        local w_ride_cfg = weapon_cfg[ride_type]
        if w_ride_cfg ~= nil then
            att_key = is_l == true and w_ride_cfg.l or w_ride_cfg.r
        end
    elseif ride_type == RIDE_TYPE.TYPE5 then
        att_key = ChAttachKeyName.Back
    end
    if att_key == nil then
        att_key = is_l == true and ChAttachKeyName.LeftHand or ChAttachKeyName.RightHand
    end
    return att_key
end
function UIChDrawer:flushWeapon(is_show)
    if self.r_weapon_body ~= nil then
        self.r_weapon_body:SetShow(is_show)
    end

    if self.l_weapon_body ~= nil then
        self.l_weapon_body:SetShow(is_show)
    end
end
-- function UIChDrawer:SetModelCameraDataChangeResAction()
--     self.model_camera_data.ChangeResAction = function(folder, res_id)
--         self:SetMain(res_id)
--     end
-- end
