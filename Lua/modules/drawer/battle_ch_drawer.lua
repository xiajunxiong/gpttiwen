BattleChDrawer = BattleChDrawer or BaseClass(nil, "BattleChDrawer")

local body_keys = {
    "main_body",
    -- "ride_body",
    "wing_body",
    "l_weapon_body",
    "r_weapon_body"
}

function BattleChDrawer:__init(root_obj)
    self.root_obj = root_obj
    -- self.shadow_body = BodyObj.New("actor/Shadow.prefab", root_obj)

    self.main_body = nil
    self.main_anim = nil
    self.main_res_id = nil

    self.wing_body = nil

    self.l_weapon_body = nil
    self.l_weapon_res_id = 0
    self.r_weapon_body = nil
    self.r_weapon_res_id = 0
    self.special_show = false

    if QuaCtrl:RoleShadowEnabled() then
        self:EnableShadow(true)
    end
end

function BattleChDrawer:EnableShadow(enable)
    if enable and not self.shadow_body then
        self.shadow_body = BodyObj.New("actor/Shadow.prefab", self.root_obj)
    end
    if self.shadow_body then
        self.shadow_body:SetShow(enable)
    end
end
function BattleChDrawer:EnableMain(enable)
    if self.main_body then
        self.main_body:SetShow(enable)
    end
end

function BattleChDrawer:SetData(appearance)
    --[[ TimeHelper:AddDelayTimer(function ()
        if appearance.special_appearance_type and appearance.special_appearance_type == 1 and appearance.special_appearance_param < 999 then
            local config = PartnerData.Instance:GetPartnerInfoById(appearance.special_appearance_param)
            if config then
                self:SetMain(config.info.res_id, nil, true)
            end
        else
            self:SetMain(ResHelper.RoleResId(appearance.avatar_type, nil, appearance.advance_time), appearance.color)
            local weapon = Config.equipment_auto[appearance.weapon_id]
            local sub_weapon = Config.equipment_auto[appearance.sub_weapon_id]
            local weapon_type = weapon and weapon.weapon_type or nil
            local sub_weapon_type = sub_weapon and sub_weapon.weapon_type or nil
            self:SetWeapon(ResHelper.WeaponResId(weapon_type, nil, sub_weapon_type, nil, appearance.advance_time))
        end
    end, 14) ]]
    if appearance.special_appearance_type and appearance.special_appearance_type == 1 and appearance.special_appearance_param < 999 then
        local config = PartnerData.Instance:GetPartnerInfoById(appearance.special_appearance_param)
        if config then
            self:SetMain(config.info.res_id, nil, true)
        end
    else
		self.r_weapon_res_id = 0
		self.l_weapon_res_id = 0
        local special_show = appearance.special_show
        if special_show then
            self:SetMain(special_show.main_res, special_show.main_color)
            if special_show.show_weapon then
                self:SetWeapon(special_show.weapon_res1, special_show.weapon_res2)
            else
                special_show.show_weapon = true
                self:SetWeapon()
            end
        else
            if appearance.henshin_monkey == true then
                self:SetMain(appearance.henshin_monkey_res, nil, true)
                self:SetWeapon()
            else
                local card_model_res_id = LiveData.Instance:CardModel(appearance)
                if card_model_res_id then
                    self:SetMain(card_model_res_id, nil, true)
                else
                    self:SetMain(ResHelper.RoleResId(appearance.avatar_type, appearance.fashion_body_id, appearance.advance_time), (appearance.fashion_body_id or 0) > 0 and 0 or appearance.color)
                    local weapon = Config.equipment_auto[appearance.weapon_id]
                    local sub_weapon = Config.equipment_auto[appearance.sub_weapon_id]
                    local weapon_type = weapon and weapon.weapon_type or nil
                    local sub_weapon_type = sub_weapon and sub_weapon.weapon_type or nil
                    local fashion_weapon_id = appearance.fashion_weapon_id or 0
                    self:SetWeapon(ResHelper.WeaponResId(weapon_type, fashion_weapon_id, sub_weapon_type, nil, appearance.advance_time))
                end
            end
        end
    end
end

function BattleChDrawer:SetMain(res_id, color, special)
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

    self.main_body = BodyObj.New(path, self.root_obj, nil, true)
    self.main_anim = self.main_body:Anim()

    if self.wing_body ~= nil then
        self.main_body:AttachBody(ChAttachKeyName.Back, self.wing_body)
    end
	
	self.main_body:SetLoadedCallback(function ()
		local need_overeide_clip = DrawerHelper.GetWeaponIsNeedOverrideClip(self.r_weapon_res_id)
		if need_overeide_clip then
			self:flushWeaponBodyAnimOverride()
		end
	end)


    self:flushMainBodyAnimOverride()
end

function BattleChDrawer:SetWing(res_id)
    local path = DrawerHelper.GetWingPath(res_id)
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

function BattleChDrawer:SetWeapon(res_id_r, res_id_l)
    local need_flush_attach = false
    self.l_weapon_res_id = res_id_l ~= nil and res_id_l or 0
    local l_path = DrawerHelper.GetWeaponPath(self.l_weapon_res_id)
    if (self.l_weapon_body == nil and l_path ~= nil) or (self.l_weapon_body ~= nil and self.l_weapon_body:Path() ~= l_path) then
        if self.l_weapon_body ~= nil then
            self.l_weapon_body:DeleteMe()
            self.l_weapon_body = nil
        end
        if l_path ~= nil then
            self.l_weapon_body = BodyObj.New(l_path, self.root_obj)
            local type_l, par_key = DrawerHelper.GetWeaponTypeAndKey(self.l_weapon_res_id)
            self.l_weapon_body:AttachToParent(par_key, self.main_body)
            need_flush_attach = true
        end
    end

    self.r_weapon_res_id = res_id_r ~= nil and res_id_r or 0
    local r_path = DrawerHelper.GetWeaponPath(self.r_weapon_res_id)
    if (self.r_weapon_body == nil and r_path ~= nil) or (self.r_weapon_body ~= nil and self.r_weapon_body:Path() ~= r_path) then
        if self.r_weapon_body ~= nil then
            self.r_weapon_body:DeleteMe()
            self.r_weapon_body = nil
        end
        if r_path ~= nil then
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
    end

    self:flushMainBodyAnimOverride()
end

function BattleChDrawer:__delete()
    if self.shadow_body ~= nil then
        self.shadow_body:DeleteMe()
        self.shadow_body = nil
    end

    if self.main_body ~= nil then
        self.main_body:DeleteMe()
        self.main_body = nil
        self.main_anim = nil
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

function BattleChDrawer:PlayAnim(anim)
    if self.main_anim ~= nil then
        self.main_anim:PlayAnim(anim)
    end
end
function BattleChDrawer:IsInAnim(anim)
    if self.main_anim ~= nil then
        return self.main_anim:CurAnim() == anim
    end
    return false
end

function BattleChDrawer:flushMainBodyAnimOverride()
    if self.special_show == true then return end
    --武器动画override
    local override_path = DrawerHelper.GetBattleWeaponOverridePath(self.main_res_id, self.l_weapon_res_id, self.r_weapon_res_id)
    self.main_anim:SetAnimOverride(override_path)
	
	local need_overeide_clip = DrawerHelper.GetWeaponIsNeedOverrideClip(self.r_weapon_res_id)
	if need_overeide_clip then
		self:flushWeaponBodyAnimOverride()
	end
end

function BattleChDrawer:flushWeaponBodyAnimOverride()
    if self.special_show == true then return end
	if  self.r_weapon_body ~= nil and self.r_weapon_res_id > 0 then
		local override_path = DrawerHelper.GetBattleWeaponAnimationClipOverridePath(self.r_weapon_res_id,self.main_res_id)
		local weapon_anim = self.r_weapon_body:Anim()
		weapon_anim:SetAnimOverride(override_path)
		weapon_anim:SetAnimSynchronizer(self.main_anim.animator)
	end
end

function BattleChDrawer:IsLoading()
    return  self.main_body == nil or (self.main_body ~= nil and self.main_body:IsLoading())
end

function BattleChDrawer:WaitAnimEnd(anim, func)
    self.main_anim:WaitAnimEnd(anim, func)
end

function BattleChDrawer:WaitAnimEvent(anim,eve_name,func)
    self.main_anim:WaitAnimEvent(anim,eve_name,func)
end

function BattleChDrawer:SetAnimActive(bool)
    self.main_anim:SetAnimActive(bool)
end

function BattleChDrawer:SetOnAnimHit(func)
    self.main_anim:SetOnAnimHit(func)
end

function BattleChDrawer:GetWaitAnimEventHandle()
    return self.main_anim:GetWaitAnimEventHandle()
end
function BattleChDrawer:SetWaitAnimEventHandle(handles)
    if self.main_anim and handles then
        self.main_anim:SetWaitAnimEventHandle(handles)
    end
end

function BattleChDrawer:SkillPlayer()
    return self.main_anim:SkillPlayer()
end

function BattleChDrawer:GetMainBodyHeight()
    local prop = self.main_body:GetModelCameraPropertyCom()
    if prop == nil then
        return
    end
    return (prop.boundMax.y - prop.boundMin.y) * self.root_obj.transform.localScale.y
end

function BattleChDrawer:GetHeight()
    local max_y
    local min_y
    for _, key in pairs(self.body_keys) do
        if self[key] then
            local prop = self[key]:GetModelCameraPropertyCom()
            if prop then
                if not max_y or prop.boundMax.y > max_y then
                    max_y = prop.boundMax.y
                end
                if not min_y or prop.boundMin.y < min_y then
                    min_y = prop.boundMin.y
                end
            end
        end
    end
    return (max_y - min_y) * self.root_obj.transform.localScale.y
end
function BattleChDrawer:SetLoadedCallback(callback)
    -- if self.main_body ~= nil then
    self.main_body:SetLoadedCallback(callback)
    -- end
end
function BattleChDrawer:MeshGhostInit(interval, duration)
    self.main_body:MeshGhostInit(interval, duration)
end
function BattleChDrawer:GhostCom()
    return self.main_body:GetMeshGhostCom()
end

function BattleChDrawer:FindChildByName(name)
    if self.main_body == nil then
        return nil
    end
    return self.main_body:FindChildByName(name)
end