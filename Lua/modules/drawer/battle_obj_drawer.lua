BattleObjDrawer = BattleObjDrawer or BaseClass(nil, "BattleObjDrawer")

function BattleObjDrawer:__init(root_obj, has_shadow)
    self.root_obj = root_obj
    self.main_body = nil
    self.main_anim = nil
    self.has_shadow = has_shadow
    if self.has_shadow == true and QuaCtrl:RoleShadowEnabled() then
        self:EnableShadow(true)
    end
    -- self.is_loading = os.time()
end

function BattleObjDrawer:SetData(path, sync, callback)
    --[[ if sync == true then ]]
        self.main_body = BodyObj.New(path, self.root_obj, nil, true)
        self.main_anim = self.main_body:Anim()
        if callback then
            self:SetLoadedCallback(callback)
        end
    --[[ else
        TimeHelper:AddDelayTimer(function ()
            self.main_body = BodyObj.New(path, self.root_obj)
            self.main_anim = self.main_body:Anim()
        end,10)
    end ]]
end

function BattleObjDrawer:EnableShadow(enable)
    if enable and not self.shadow_body then
        self.shadow_body = BodyObj.New("actor/Shadow.prefab", self.root_obj)
    end
    if self.shadow_body then
        self.shadow_body:SetShow(enable)
    end
end

function BattleObjDrawer:EnableMain(enable)
    if self.main_body then
        self.main_body:SetShow(enable)
    end
end

function BattleObjDrawer:__delete()
    if self.shadow_body ~= nil then
        self.shadow_body:DeleteMe()
        self.shadow_body = nil
    end
    if self.main_body ~= nil then
        self.main_body:DeleteMe()
        self.main_body = nil
    end
end
function BattleObjDrawer:DeleteShadow()
    if self.shadow_body ~= nil then
        self.shadow_body:DeleteMe()
        self.shadow_body = nil
    end
end

function BattleObjDrawer:PlayAnim(anim)
    if self.main_anim ~= nil then
        self.main_anim:PlayAnim(anim)
    end
end
function BattleObjDrawer:AnimClipsEmpty(name)
    if self.main_anim ~= nil then
        return self.main_anim:AnimClipsEmpty(name)
    end
end
function BattleObjDrawer:IsInAnim(anim)
    if self.main_anim ~= nil then
        return self.main_anim:CurAnim() == anim
    end
    return false
end
function BattleObjDrawer:IsLoading()
    return self.main_body == nil or (self.main_body ~= nil and self.main_body:IsLoading())
end

function BattleObjDrawer:WaitAnimEnd(anim, func)
    self.main_anim:WaitAnimEnd(anim, func)
end

function BattleObjDrawer:WaitAnimEvent(anim, eve_name, func)
    self.main_anim:WaitAnimEvent(anim, eve_name, func)
end

function BattleObjDrawer:SetOnAnimHit(func)
    self.main_anim:SetOnAnimHit(func)
end

function BattleObjDrawer:GetWaitAnimEventHandle()
    return self.main_anim:GetWaitAnimEventHandle()
end
function BattleObjDrawer:SetWaitAnimEventHandle(handles)
    if self.main_anim and handles then
        self.main_anim:SetWaitAnimEventHandle(handles)
    end
end

function BattleObjDrawer:SkillPlayer()
    return self.main_anim:SkillPlayer()
end

function BattleObjDrawer:SetAnimActive(bool)
    self.main_anim:SetAnimActive(bool)
end
function BattleObjDrawer:SetScale(size)
    -- if self.main_body ~= nil then
    self.main_body:SetScale(size)
    -- end
end
function BattleObjDrawer:SetLoadedCallback(callback)
    -- if self.main_body ~= nil then
    self.main_body:SetLoadedCallback(callback)
    -- end
end

function BattleObjDrawer:GetMainBodyHeight()
    return self:GetHeight()
end

function BattleObjDrawer:GetHeight()
    local prop = self.main_body:GetModelCameraPropertyCom()
    if prop == nil then
        return
    end
    return (prop.boundMax.y - prop.boundMin.y) * self.root_obj.transform.localScale.y
end
function BattleObjDrawer:MeshGhostInit(interval, duration)
    self.main_body:MeshGhostInit(interval, duration)
end
function BattleObjDrawer:GhostCom()
    return self.main_body:GetMeshGhostCom()
end
function BattleObjDrawer:SetParent(par_obj)
    self.main_body:SetParent(par_obj)
end

function BattleObjDrawer:ResetTransform()
    self.main_body:ResetTransform()
end

function BattleObjDrawer:FindChildByName(name)
    if self.main_body == nil then
        return nil
    end
    return self.main_body:FindChildByName(name)
end