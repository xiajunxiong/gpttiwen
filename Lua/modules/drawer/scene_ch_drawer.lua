SceneChDrawer = SceneChDrawer or BaseClass()
function SceneChDrawer:__init(root_obj, vo)
    self.special_mount_id = {
        [3046] = 1,
        [3047] = 1,
        [3048] = 1,
    }
    self.root_obj = root_obj
    self.vo = vo

    self.main_body = nil
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

    self.gaozi_body = nil
    self.fuzi_body = nil
    
    self.fishrod_body = nil
    self.fishrod_anim = nil

    self.gather_res_id = 0
    self.special_show = false
    if QuaCtrl:RoleShadowEnabled() then
        self:EnableShadow(true)
    end
end

function SceneChDrawer:EnableShadow(enable)
    if enable and not self.shadow_body then
        self.shadow_body = BodyObj.New("actor/Shadow.prefab", self.root_obj)
    end
    if self.shadow_body then
        self.shadow_body:SetShow(enable)
    end
end

local body_keys = {
    "main_body",
    "ride_body",
    "wing_body",
    "l_weapon_body",
    "r_weapon_body"
}
local gather_res = {
    ["wakuang"] = 110002,
    ["famu"] = 100002,
    ["yugan"] = 120002
}

function SceneChDrawer:SetRendererMaterial(material, layer)
    if self.ride_res_id and self.special_mount_id[self.ride_res_id] == 1 then
        return
    end
    for _, key in pairs(body_keys) do
        if self[key] then
            self[key]:SetRendererMaterial(material, layer)
        end
    end
end
function SceneChDrawer:RevertRendererMaterial()
    for _, key in pairs(body_keys) do
        if self[key] then
            self[key]:RevertRendererMaterial()
        end
    end
end

function SceneChDrawer:ForeachBodyObject(func)
    for _, key in pairs(body_keys) do
        if self[key] then
            func(self[key])
        end
    end
end

function SceneChDrawer:IsLoading()
    for _, key in pairs(body_keys) do
        if self[key] and self[key]:IsLoading() then
            return true
        end
    end
    return false
end

function SceneChDrawer:SetMain(res_id, color, special)
    self.main_res_id = res_id ~= nil and res_id or 0
    local path
    if special then
        self.special_show = true
        path = DrawerHelper.GetNpcPath(res_id)
    else
        self.special_show = false
        path = DrawerHelper.GetRolePath(res_id, color)
    end
    if self.main_body ~= nil then
        if path == self.main_body:Path() then
            return
        end
        self.main_body:DeleteMe()
    end
    -- 判断是否能挪移尺
    local can_hyper = TombRaiderData.Instance:InTombRaider()
    self.main_body = BodyObj.New(path, self.root_obj,nil,can_hyper)
    if self.main_anim then
        self.playing_anim = self.main_anim:CurAnim()
    end
    self.main_anim = self.main_body:Anim()

    if self.wing_body ~= nil then
        self.main_body:AttachBody(ChAttachKeyName.Back, self.wing_body)
    end

    if self.ride_body ~= nil then
        self.main_body:AttachToParent(ChAttachKeyName.Mount, self.ride_body)
        if self.ride_res_id and self.special_mount_id[self.ride_res_id] == 1 then
            local ch_obj = self.main_body:getObj()
            if ch_obj then
                ch_obj:SetParent(self.root_obj)
                ch_obj:SetLocalEulerAngles(Vector3.zero)
            end
        end
    end
	if self.playing_anim then
        self:PlayAnim(self.playing_anim)
        self.playing_anim = nil
    end
	self.main_body:SetLoadedCallback(function ()
        --LogError("vo",self.vo.appearance.weapon_id == self.r_weapon_res_id, self.r_weapon_res_id,"appearance.weapon_id", self.vo.appearance.weapon_id)
		local need_overeide_clip = DrawerHelper.GetWeaponIsNeedOverrideClip(self.r_weapon_res_id)
		if need_overeide_clip then-- and self:CheckWeaponAndMainSuitable(self.vo.appearance) then
			self:flushWeaponBodyAnimOverride()
        end
	end)

    self:flushMainBodyAnimOverride()
end

function SceneChDrawer:SetShengQi(sheng_qi)
    if TableIsEmpty(sheng_qi) then
        if self.sheng_qi_body ~= nil then
            self.sheng_qi_body:DeleteMe()
            self.sheng_qi_body = nil
        end
        return
    end

    local path = sheng_qi:ResPath()
    if self.sheng_qi_body ~= nil then
        if path == self.sheng_qi_body:Path() then
            return
        end
        self.sheng_qi_body:DeleteMe()
        self.sheng_qi_body = nil
    end

    self.sheng_qi_body = BodyObj.New(path, self.root_obj)
    self.sheng_qi_body:SetLoadedCallback(function ()
        self.sheng_qi_body.obj.transform.localPosition = Vector3.New(-1.71,1.66,-0.18)
    end)
end



function SceneChDrawer:SetRide(res_id)
    self.ride_res_id = res_id ~= nil and res_id or 0
    local path = DrawerHelper.GetRidePath(res_id)
    if self.ride_body ~= nil then
        if path == self.ride_body:Path() then
            return
        end
        self.ride_body:DeleteMe()
    end
    self.vo.SeatHeight = nil
    if path == nil then
        self.ride_body = nil
        self.ride_anim = nil
        self.main_body:ResetTransform()
    else
        if self.ride_anim then
            self.playing_anim = self.ride_anim:CurAnim()
        elseif self.main_anim then
            self.playing_anim = self.main_anim:CurAnim()
        end
        self.ride_body = BodyObj.New(path, self.root_obj)
        self.ride_anim = self.ride_body:Anim()
        self.ride_body:AttachBody(ChAttachKeyName.Mount, self.main_body)
        self.wait_for_ride_load =
            Runner.Instance:RunUntilTrue(
            function()
                if self.ride_body == nil then return true end
                if not self.ride_body:IsLoading() and self.ride_body.body_com then
                    local SeatHeight = self.ride_body.body_com.SeatHeight
                    if SeatHeight then
                        self.vo.SeatHeight = SeatHeight
                    else
                        self.vo.SeatHeight = nil
                    end
                    if self.special_mount_id[res_id] == 1 then
                        local ch_obj = self.main_body:getObj()
                        if ch_obj then
                            ch_obj:SetParent(self.root_obj)
                        end
                        local ride_obj = self.ride_body:getObj()
                        if ride_obj then
                            local tbt = ride_obj:GetOrAddComponent(typeof(Game.TransformByTarget))
                            tbt.SyncPosition = false
                            tbt.SyncRotation = true
                            tbt:SetTarget(UnityGameObj.FindGameObjectWithTag("BattleEnterBefore").transform)
                        end
                    end
                    return true
                end
            end,
            true
        )
        if self.playing_anim then
            self:PlayAnim(self.playing_anim)
            self.playing_anim = nil
        end
    end
    self:flushMainBodyAnimOverride()
    -- self:flushWeaponAttach()
    self:flushWeanponShow()
end

function SceneChDrawer:SetWing(res_id)
    self.wing_res_id = res_id ~= nil and res_id or 0
    local path = DrawerHelper.GetWingPath(self.wing_res_id)
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

    self.wing_body:AttachToParent(ChAttachKeyName.Back, self.main_body)
end

function SceneChDrawer:SetWeapon(res_id_r, res_id_l)
    local need_flush_attach = false
    self.l_weapon_res_id = res_id_l ~= nil and res_id_l or 0
    local l_path = DrawerHelper.GetWeaponPath(self.l_weapon_res_id)
    if (self.l_weapon_body == nil and l_path ~= nil) or (self.l_weapon_body ~= nil and self.l_weapon_body:Path() ~= l_path) then
        if self.l_weapon_body ~= nil then
            self.l_weapon_body:DeleteMe()
            self.l_weapon_body = nil
        end
        if l_path ~= nil and self.special_show == false then
            self.l_weapon_body = BodyObj.New(l_path, self.root_obj)
            local type_l, par_key = DrawerHelper.GetWeaponTypeAndKey(self.l_weapon_res_id)
            self.l_weapon_body:AttachToParent(par_key, self.main_body)
            need_flush_attach = true
        end
    else
        if self.l_weapon_body and l_path then
            local type_l, par_key = DrawerHelper.GetWeaponTypeAndKey(self.l_weapon_res_id)
            self.l_weapon_body:AttachToParent(par_key, self.main_body)
        end
    end

    self.r_weapon_res_id = res_id_r ~= nil and res_id_r or 0
    local r_path = DrawerHelper.GetWeaponPath(self.r_weapon_res_id)
    if (self.r_weapon_body == nil and r_path ~= nil) or (self.r_weapon_body ~= nil and self.r_weapon_body:Path() ~= r_path) then
        if self.r_weapon_body ~= nil then
            self.r_weapon_body:DeleteMe()
            self.r_weapon_body = nil
        end
        if r_path ~= nil and self.special_show == false then
            self.r_weapon_body = BodyObj.New(r_path, self.root_obj)
            local type_r, par_key = DrawerHelper.GetWeaponTypeAndKey(self.r_weapon_res_id)
            self.r_weapon_body:AttachToParent(par_key, self.main_body)
            need_flush_attach = true
			self.r_weapon_body:SetLoadedCallback(function ()
				local need_overeide_clip = DrawerHelper.GetWeaponIsNeedOverrideClip(self.r_weapon_res_id)
				if need_overeide_clip then
					self:flushWeaponBodyAnimOverride()
				end
			end)
        end
    else
        if self.r_weapon_body and r_path then
            local type_r, par_key = DrawerHelper.GetWeaponTypeAndKey(self.r_weapon_res_id)
            self.r_weapon_body:AttachToParent(par_key, self.main_body)
        end
    end
		
    if need_flush_attach then
        -- self:flushWeaponAttach()
        self:flushWeanponShow()
    end
    self:flushMainBodyAnimOverride()
end

function SceneChDrawer:SetGather(anim)
    local need_flush_attach = false
    if anim == ChAnimType.WaKuang then
        self.gather_res_id = self:SetGaoZi(anim)
        need_flush_attach = true
    elseif anim == ChAnimType.FaMu then
        self.gather_res_id = self:SetFuZi(anim)
        need_flush_attach = true
    elseif anim == ChAnimType.Gather then
        self.gather_res_id = 1
        need_flush_attach = true
    elseif anim == ChAnimType.FishIdle then
        self.gather_res_id = self:SetFishRod(anim)
        need_flush_attach = true
    end
    if need_flush_attach then
        self:flushWeanponShow()
    end
end
function SceneChDrawer:SetGaoZi(anim)
    local res_id = gather_res[anim]
    if self.gaozi_body then
        self.gaozi_body:SetShow(true)
    else
        local path = DrawerHelper.GetWeaponPath(res_id)
        self.gaozi_body = BodyObj.New(path, self.root_obj)
    end
    local type_l, par_key = DrawerHelper.GetWeaponTypeAndKey(res_id)
    self.gaozi_body:AttachToParent(par_key, self.main_body)
    return res_id
end
function SceneChDrawer:SetFuZi(anim)
    local res_id = gather_res[anim]
    if self.fuzi_body then
        self.fuzi_body:SetShow(true)
    else
        local path = DrawerHelper.GetWeaponPath(res_id)
        self.fuzi_body = BodyObj.New(path, self.root_obj)
    end
    local type_l, par_key = DrawerHelper.GetWeaponTypeAndKey(res_id)
    self.fuzi_body:AttachToParent(par_key, self.main_body)
    return res_id
end
function SceneChDrawer:SetFishRod(anim, avatar_type)
    local res_id = gather_res[anim]
    if self.fishrod_body then
        if avatar_type then
            self:flushFishrodBodyAnimOverride(avatar_type)
        else
            self.fishrod_body:SetShow(true)
        end
    else
        local path = DrawerHelper.GetWeaponPath(res_id)
        self.fishrod_body = BodyObj.New(path, self.root_obj)
        self.fishrod_body:SetLoadedCallback(function ()
            local need_overeide_clip = DrawerHelper.GetWeaponIsNeedOverrideClip(res_id)
            if need_overeide_clip then
                if avatar_type then
                    self:flushFishrodBodyAnimOverride(avatar_type)
                end
            end
            self.fishrod_body:SetShow(false)
        end)
    end
    local type_l, par_key = DrawerHelper.GetWeaponTypeAndKey(res_id)
    self.fishrod_body:AttachToParent(par_key, self.main_body)
    return res_id
end

function SceneChDrawer:__delete()
    if self.wait_for_ride_load then
        Runner.Instance:RemoveRunUntilTrue(self.wait_for_ride_load)
        self.wait_for_ride_load = nil
    end

    if self.shadow_body then
        self.shadow_body:DeleteMe()
        self.shadow_body = nil
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

    if self.gaozi_body ~= nil then
        self.gaozi_body:DeleteMe()
        self.gaozi_body = nil
    end
    if self.fuzi_body ~= nil then
        self.fuzi_body:DeleteMe()
        self.fuzi_body = nil
    end
    if self.fishrod_body ~= nil then
        self.fishrod_body:DeleteMe()
        self.fishrod_body = nil
        self.fishrod_anim = nil
    end
end

function SceneChDrawer:PlayAnim(anim,anim_param, is_ride)
    if anim ~= ChAnimType.WaKuang and anim ~= ChAnimType.FaMu and anim ~= ChAnimType.Gather
    and not FishAnim[anim] then
        self.gather_res_id = 0
        self:flushWeanponShow(true)
    else
        self:SetGather(anim)
    end
    if self.main_anim ~= nil and not is_ride then
        self.main_anim:PlayAnim(anim,anim_param)
    end
    if self.ride_anim ~= nil and self.special_mount_id[self.ride_res_id] ~= 1 then
        self.ride_anim:PlayAnim(anim)
    end
    if FishAnim[anim] and self.fishrod_anim ~= nil then
        self.fishrod_anim:PlayAnim(anim)
    end
end

function SceneChDrawer:AnimClipsEmpty(name)
    if self.main_anim ~= nil then
        return self.main_anim:AnimClipsEmpty(name)
    end
end
function SceneChDrawer:IsInAnim(anim)
    if self.main_anim ~= nil then
        return self.main_anim:CurAnim() == anim
    end
    return false
end

function SceneChDrawer:flushMainBodyAnimOverride()
    if self.special_show == true then return end
    if self.main_res_id == nil then return end
    if not DrawerHelper.IsMainResMatchWeapon(self.main_res_id,self.r_weapon_res_id) then
        return
    end
    local override_path = nil
    if self.ride_res_id > 0 then
        override_path = DrawerHelper.GetRideOverridePath(self.main_res_id, self.ride_res_id, self.l_weapon_res_id, self.r_weapon_res_id)
    else --武器动画override
        override_path = DrawerHelper.GetSceneWeaponOverridePath(self.main_res_id, self.l_weapon_res_id, self.r_weapon_res_id)
    end
    self.main_anim:SetAnimOverride(override_path)
	
	local need_overeide_clip = DrawerHelper.GetWeaponIsNeedOverrideClip(self.r_weapon_res_id)
	if need_overeide_clip then-- and self:CheckWeaponAndMainSuitable(self.vo.appearance) then
		self:flushWeaponBodyAnimOverride()
	end
end

function SceneChDrawer:flushWeaponBodyAnimOverride()
	if self.special_show == true then return end
    if not DrawerHelper.IsMainResMatchWeapon(self.main_res_id,self.r_weapon_res_id) then
        return
    end
	if self.r_weapon_body ~= nil and self.r_weapon_res_id > 0 then
		local override_path
		if self.ride_res_id > 0 then
			override_path = DrawerHelper.GetRideSceneWeaponAnimationClipOverridePath(self.r_weapon_res_id, self.main_res_id, self.ride_res_id)
		else 
			override_path = DrawerHelper.GetSceneWeaponAnimationClipOverridePath(self.r_weapon_res_id,self.main_res_id)
		end
		local weapon_anim = self.r_weapon_body:Anim()
		weapon_anim:SetAnimOverride(override_path)
		weapon_anim:SetAnimSynchronizer(self.main_anim.animator)
	end
end

function SceneChDrawer:flushFishrodBodyAnimOverride(avatar_type)
	if self.special_show == true then return end
    if self.fishrod_body then
        local override_path = string.format("actor/wanim/%d/role%d.asset", gather_res.yugan, avatar_type)
        self.fishrod_anim = self.fishrod_body:Anim()
        self.fishrod_anim:SetAnimOverride(override_path)
        self.fishrod_anim:SetAnimSynchronizer(self.main_anim.animator)
    end
end


function SceneChDrawer:flushWeanponShow(hide_gather)
    local show
    if --[[ self.ride_res_id <= 0 and ]] self.gather_res_id <= 0 then
        show = true
    else
        show = false
    end

    -- if self.r_weapon_body ~= nil then
    --     self.r_weapon_body:SetShow(show)
    -- end

    -- if self.l_weapon_body ~= nil then
    --     self.l_weapon_body:SetShow(show)
    -- end
    self:SetWeaponShow(show)
    if hide_gather == true then
        if self.gaozi_body then
            self.gaozi_body:SetShow(false)
        end
        if self.fuzi_body then
            self.fuzi_body:SetShow(false)
        end
        if self.fishrod_body then
            self.fishrod_body:SetShow(false)
        end
    end
end

function SceneChDrawer:SetWeaponShow(is_show)
    if self.r_weapon_body ~= nil then
        self.r_weapon_body:SetShow(is_show)
    end
    if self.l_weapon_body ~= nil then
        self.l_weapon_body:SetShow(is_show)
    end
end

function SceneChDrawer:SetMainBodyShow(is_show)
    if self.main_body ~= nil then
        self.main_body:SetShow(is_show)
    end
end
function SceneChDrawer:SetShadowBodyShow(is_show)
    if self.shadow_body ~= nil then
        self.shadow_body:SetShow(is_show)
    end
end
function SceneChDrawer:SetScale(size)
    -- if self.main_body ~= nil then
    self.main_body:SetScale(size)
    -- end
end

function SceneChDrawer:SetLoadedCallback(callback)
    -- if self.main_body ~= nil then
    self.main_body:SetLoadedCallback(callback)
    -- end
end
--主要是对比SetMain时appearance中的weapon_id和之前的
-- function SceneChDrawer:CheckWeaponAndMainSuitable(appearance)
--     if self.r_weapon_res_id > 0 or self.l_weapon_res_id > 0 then
--         local weapon = Config.equipment_auto[appearance.weapon_id]
--         local sub_weapon = Config.equipment_auto[appearance.sub_weapon_id]
--         local weapon_type = weapon and weapon.weapon_type or nil
--         local sub_weapon_type = sub_weapon and sub_weapon.weapon_type or nil
--         local new_r_weapon_res_id, new_l_weapon_res_id = ResHelper.WeaponResId(weapon_type, appearance.fashion_weapon_id, sub_weapon_type,nil,appearance.advance_time)
--         if self.r_weapon_res_id > 0 and self.r_weapon_res_id ~= new_r_weapon_res_id then
--             return false
--         elseif self.l_weapon_res_id > 0 and self.l_weapon_res_id ~= new_l_weapon_res_id then
--            return false
--         end
--     end
--     return true
-- end

------------------废弃，根据不同的坐骑把武器挂载到不同的角色骨骼点上------------

function SceneChDrawer:flushWeaponAttach()
    local l_key = nil
    local r_key = nil
    if self.ride_res_id <= 0 then
        l_key = ChAttachKeyName.LeftHand
        r_key = ChAttachKeyName.RightHand
    else
        local ride_type = math.floor(self.ride_res_id / 1000)
        if self.l_weapon_res_id > 0 then
            local weapon_type_l = math.floor(self.l_weapon_res_id / 10000)
            l_key = SceneChDrawer.getWeaponAttachKey(weapon_type_l, ride_type, true)
        end
        if self.r_weapon_res_id > 0 then
            local weapon_type_r = math.floor(self.r_weapon_res_id / 10000)
            r_key = SceneChDrawer.getWeaponAttachKey(weapon_type_r, ride_type, false)
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

function SceneChDrawer.getWeaponAttachKey(weapon_type, ride_type, is_l)
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

function SceneChDrawer:GetHeight()
    local prop = self.main_body:GetModelCameraPropertyCom()
    if prop == nil then
        return
    end
    return (prop.boundMax.y - prop.boundMin.y) * self.root_obj.transform.localScale.y
end

function SceneChDrawer:GhostCom()
    return self.main_body:GetMeshGhostCom()
end

function SceneChDrawer:RideId()
    return self.ride_res_id
end

function SceneChDrawer:HasAnim(anim)
    if self.main_anim == nil then
        return false
    end
    return self.main_anim:HasAnim(anim)
end

function SceneChDrawer:IsRoleType()
    return true
end