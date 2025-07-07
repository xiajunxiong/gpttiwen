SceneObjDrawer = SceneObjDrawer or BaseClass()

function SceneObjDrawer:__init(root_obj, has_shadow)
    self.root_obj = root_obj
    self.main_body = nil
    self.main_anim = nil
    self.has_shadow = has_shadow
    if self.has_shadow == true and QuaCtrl:RoleShadowEnabled() then
        self:EnableShadow(true)
    end
end

function SceneObjDrawer:SetData(path)
    if self.main_body ~= nil then
        if self.main_body.res_path == path then
            return
        end
        if self.main_body ~= nil then
            self.main_body:DeleteMe()
            self.main_body = nil
        end
    end
    self.main_body = BodyObj.New(path, self.root_obj)
    self.main_anim = self.main_body:Anim()
end

function SceneObjDrawer:EnableShadow(enable)
    if enable and not self.shadow_body then
        self.shadow_body = BodyObj.New("actor/Shadow.prefab", self.root_obj)
    end
    if self.shadow_body then
        self.shadow_body:SetShow(enable)
    end
end

function SceneObjDrawer:__delete()
    if self.shadow_body ~= nil then
        self.shadow_body:DeleteMe()
        self.shadow_body = nil
    end
    if self.main_body ~= nil then
        self.main_body:DeleteMe()
        self.main_body = nil
    end
end
function SceneObjDrawer:DeleteShadow()
    if self.shadow_body ~= nil then
        self.shadow_body:DeleteMe()
        self.shadow_body = nil
    end
end

function SceneObjDrawer:PlayAnim(anim)
    if self.main_anim ~= nil then
        self.main_anim:PlayAnim(anim)
    end
end
function SceneObjDrawer:AnimClipsEmpty(name)
    if self.main_anim ~= nil then
        return self.main_anim:AnimClipsEmpty(name)
    end
end
function SceneObjDrawer:IsInAnim(anim)
    if self.main_anim ~= nil then
        return self.main_anim:CurAnim() == anim
    end
    return false
end

function SceneObjDrawer:IsLoading()
    return self.main_body ~= nil and self.main_body:IsLoading()
end

function SceneObjDrawer:WaitAnimEnd(anim, func)
    self.main_anim:WaitAnimEnd(anim, func)
end

function SceneObjDrawer:WaitAnimEvent(anim, eve_name, func)
    self.main_anim:WaitAnimEvent(anim, eve_name, func)
end

function SceneObjDrawer:SetOnAnimHit(func)
    self.main_anim:SetOnAnimHit(func)
end

function SceneObjDrawer:SkillPlayer()
    return self.main_anim:SkillPlayer()
end

function SceneObjDrawer:SetAnimActive(bool)
    self.main_anim:SetAnimActive(bool)
end
function SceneObjDrawer:SetScale(size)
    -- if self.main_body ~= nil then
    self.main_body:SetScale(size)
    -- end
end

function SceneObjDrawer:HasAnim(anim)
    if self.main_anim == nil then
        return false
    end
    return self.main_anim:HasAnim(anim)
end

function SceneObjDrawer:SetLoadedCallback(callback)
    -- if self.main_body ~= nil then
    self.main_body:SetLoadedCallback(callback)
    -- end
end

function SceneObjDrawer:GetHeight()
    local prop = self.main_body:GetModelCameraPropertyCom()
    if prop == nil then
        return
    end
    return (prop.boundMax.y - prop.boundMin.y) * self.root_obj.transform.localScale.y
end

function SceneObjDrawer:SetRendererMaterial(material, layer)
    if self.main_body then
        self.main_body:SetRendererMaterial(material, layer)
    end
end
function SceneObjDrawer:RevertRendererMaterial()
    if self.main_body then
        self.main_body:RevertRendererMaterial()
    end
end

function SceneObjDrawer:IsRoleType()
    return false
end