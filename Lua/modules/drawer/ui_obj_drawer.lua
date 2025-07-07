local root_path = "actor/__UIObjRoot.prefab"
PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        GameObjPool:RegisterPool(
            root_path,
            function(error)
                if not StringIsEmpty(error) then
                -- Debuger.LogError("__UIObjRoot pool register error:%s", error)
                end
                PreloadCtrl.UnregisterLoading()
            end,
            false
        )
    end
)

local body_keys = {
    "main_body"
}

UIObjDrawer = UIObjDrawer or BaseClass()

function UIObjDrawer.CreateByPath(path)
    local re_obj = UIObjDrawer.New()
    re_obj:SetData(path)
    return re_obj
end

function UIObjDrawer:__init(root_obj)
    self.root_obj = root_obj
    -- self.model_camera_data = self.root_obj:GetComponent(typeof(Game.ModelCameraData))
    self.main_body = nil
    self.main_path = nil
    self.main_anim = nil
    self.load_event_node =
        EventNode.New(
        function(loaded)
            self.load_event_node:Set(loaded)
        end
    )

    self.body_keys = body_keys
    self.enabled = false
    self.on_enabled = {}
end

function UIObjDrawer:__delete()
    if self.anim_delay_handle ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.anim_delay_handle)
        self.anim_delay_handle = nil
    end

    if self.load_event_node then
        self.load_event_node:DeleteMe()
        self.load_event_node = nil
    end
    SE:Stop(self.fly_eff_handle)
    self:clearCenterPointFollow()
    if self.root_obj then
        GameObjPool:Release(self.root_obj)
        self.root_obj = nil
    end

    self.anim_params = nil

    -- if self.shadow_body ~= nil then
    --     self.shadow_body:DeleteMe()
    --     self.shadow_body = nil
    -- end
    if self.main_body ~= nil then
        self.main_body:DeleteMe()
        self.main_body = nil
    end
    
    
    --[[ if self.render_effect_event_node then
        self.render_effect_event_node:DeleteMe()
        self.render_effect_event_node = nil
    end ]]
    -- self.model_camera_data.ChangeResAction = nil
end

function UIObjDrawer:Enable()
    self.enabled = true
    if not self.root_obj then
        self.root_obj = GameObjPool:Get(root_path)
    end
    for _, func in pairs(self.on_enabled) do
        func()
    end
    self.on_enabled = {}
end

function UIObjDrawer:SampleMainAnim(anim_name, time)
    if self.main_body then
        self.main_body:SampleAnim(anim_name, time)
    end
end

function UIObjDrawer:CenterPoint()
    if self.center_point == nil then
        self.center_point = self.root_obj:FindChild("CenterPoint")
    end
    return self.center_point
end

function UIObjDrawer:clearCenterPointFollow()
    if self.center_point and not self.center_point:Equals(nil) then
        self:CenterPoint():SetLocalPosition(Vector3.New(0,1.25,0))
    end
    if self.center_point_follow then
        self.center_point_follow:SetTarget(nil)
        self.center_point_follow.PositionOffset = Vector3.zero
    end
end


--必须在main_body load完成后刷新
function UIObjDrawer:FlushCenterPointFollow()
    if self.center_point_follow == nil then
        self.center_point_follow = self:CenterPoint():GetComponent(typeof(Game.TransformByTarget))
    end
    self:CenterPoint():SetActive(true)
    if self.center_point_follow and self.main_body:getObj() ~= nil then
        self.center_point_follow:SetTarget(self.main_body:getObj().transform)
    end
    if self.GetHeight and self:GetHeight() then
        if self.center_point_follow then
            self.center_point_follow.PositionOffset = self.center_point_offset_cache == nil and Vector3.New(0, self:GetHeight() / 2, 0) or Vector3.zero
        end
    end
end

function UIObjDrawer:SampleAnim(anim_name, time)
    for _, key in pairs(self.body_keys) do
        if self[key] then
            self[key]:SampleAnim(anim_name, time)
        -- self.render_effect_event_node:AddChild(self[key]:SetRenderEffect(data))
        end
    end
end

function UIObjDrawer:SetRenderEffect(data)
    -- if self.render_effect_event_node then
    --     self.render_effect_event_node:DeleteMe()
    -- end
    -- self.render_effect_event_node = EventNode.New(function (loaded)
    --     self.render_effect_event_node:Set(loaded)
    -- end)
    --待办 要在所有都加载完才执行这一步
    for _, key in pairs(self.body_keys) do
        if self[key] then
            self[key]:SetRenderEffect(data)
        -- self.render_effect_event_node:AddChild(self[key]:SetRenderEffect(data))
        end
    end
    -- return self.render_effect_event_node
end

function UIObjDrawer:flushEventNode(event_node)
    if self.load_event_node then
        self.load_event_node:AddChild(event_node)
    end
   --[[  if self.render_effect_event_node then
        self.render_effect_event_node:AddChild(event_node)
    end ]]
end

function UIObjDrawer:UpdateAnimator(time)
    if self.main_body then
        self.main_body.anim:UpdateAnimator(time)
    end
end

function UIObjDrawer:GetLoadEventNode()
    return self.load_event_node
end

function UIObjDrawer:GetRoot()
    return self.root_obj
end

function UIObjDrawer:SetData(path, fly_flag,marbas_flag)
    self.main_path = path
    self:CheckEnabled(
        "data",
        function()
            if self.main_body ~= nil then
                if path == self.main_body:Path() then
                    --形象没有变化但是形象的飞升等级变化了
                    if fly_flag ~= self.cache_fly_flag or marbas_flag ~= self.cache_marbas_flag then
                        self:SetFlyEffect(fly_flag,marbas_flag)
                    end
                    return
                end
                self.main_body:DeleteMe()
            end
            self.main_body = BodyObj.New(path, self.root_obj)
            self.main_body:SetLoadedCallback(function ()
                self:FlushCenterPointFollow()
                self:SetFlyEffect(fly_flag,marbas_flag)
            end)
            self:flushEventNode(self.main_body:GetLoadEventNode())
            self.main_anim = self.main_body:Anim()
            if self.cache_main_anim then
                self:CheckAndPlayAnim(self.cache_main_anim)
                self.cache_main_anim = nil
            end
            
            -- self:FlushModelCameraDataKey()
        end
    )
end

function UIObjDrawer:SetFlyEffect(fly_flag,marbas_flag)
    -- LogError("!?@",fly_flag,marbas_flag)
    local enable_flag_fly = fly_flag and type(fly_flag) == "number" and fly_flag > 0
    local enable_flag_marbas = marbas_flag and type(marbas_flag) == "number" and marbas_flag > 0

    local flag_fly_change = enable_flag_fly and (fly_flag ~=  self.cache_fly_flag or self.fly_eff_handle == nil)
    local flag_marbas_change = enable_flag_fly and enable_flag_marbas and (marbas_flag ~=  self.cache_marbas_flag or self.fly_eff_handle == nil)

    if flag_fly_change or flag_marbas_change then
        if  self.GetHeight and self:GetHeight() then 
            SE:Stop(self.fly_eff_handle)
            -- 赋值
            self.cache_marbas_flag = marbas_flag or 0
            local show_flag = self.cache_marbas_flag > 0
            self.cache_fly_flag = show_flag and 0 or fly_flag

            local sp_path = show_flag and MarbasData.Instance:GetVigorousEffId(self.cache_marbas_flag) 
                or SceneEffectConfig.EffectList["ChFlyUpEffect"..self.cache_fly_flag]

            local function fly_eff(obj)
                obj:SetLocalScale(Vector3.New(self.body:GetHeight() / 2,self.body:GetHeight() / 2,self.body:GetHeight() / 2))
            end

            if sp_path ~= nil then 
                self.fly_eff_handle = self:EffectInCenter(sp_path, 
                    show_flag and "cache_marbas_flag" or "cache_fly_flag",
                    show_flag and nil or fly_eff,
                    self.fly_eff_handle)
            end 
        end 
    elseif not enable_flag_fly and not enable_flag_marbas then 
        SE:Stop(self.fly_eff_handle)
        self.cache_fly_flag = 0
        self.cache_marbas_flag = 0
    end
end

-- 与eff_handle绑定的简化代码(请勿用作他用)
function UIObjDrawer:EffectInCenter(sp_path,check_key,extral_func,lock_key)
    return SE:Play(sp_path, nil,function (obj)
        if self[check_key] and self[check_key] > 0 and obj and self.body and self.body:GetHeight() then
            if extral_func ~= nil then 
                extral_func(obj)
            end  
        else
            SE:Stop(lock_key)
        end
    end, self:CenterPoint())
end


function UIObjDrawer:FlushFlyEffectShow(is_ride)
    self:CenterPoint():SetActive(is_ride)
end

--如果需要对各个部件也进行check，加入相应的key即可
function UIObjDrawer:CheckEnabled(key, func)
    if self.enabled then
        func()
    else
        self.on_enabled[key] = func
    end
end

-- function UIObjDrawer:GetModelCameraDataKey()
--     -- LogError("GetModelCameraDataKey", self.main_path)
--     return self.main_path
-- end

-- function UIObjDrawer:FlushModelCameraDataKey()
--     self.model_camera_data:ChangeKey(self:GetModelCameraDataKey())
-- end

-- function UIObjDrawer:GetModelCameraDataOffset()
--     return self.model_camera_data:GetOffset(self:GetModelCameraDataKey())
-- end

-- function UIObjDrawer:SetModelCameraDataOnOffsetChanged(func)
--     self.model_camera_data.OnOffsetChanged = func
-- end

-- function UIObjDrawer:SetModelCameraDataChangeResAction()
--     self.model_camera_data.ChangeResAction = function(folder, res_id)
--         self:SetData(string.format("actor/%s/%d.prefab", folder, res_id))
--     end
-- end

function UIObjDrawer:AnimParams(params)
    -- LogError(params)
    if params then
        self.anim_params = params
    end
    return self.anim_params
end

function UIObjDrawer:PlayAnim(anim)
    if self.main_anim ~= nil then
        self:CheckAndPlayAnim(anim)
    else
        self.cache_main_anim = anim
    end
end
function UIObjDrawer:CheckAndPlayAnim(anim)
    if self.anim_delay_handle ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.anim_delay_handle)
        self.anim_delay_handle = nil
    end
    if self.main_body:IsLoading() or self.main_anim:IsOverriderLoading() then
        self.anim_delay_handle = Runner.Instance:RunUntilTrue(
            function()
                if not self.main_body:IsLoading() and not self.main_anim:IsOverriderLoading() then
                    self.anim_delay_handle = nil
                    self.main_anim:PlayAnim(anim, self.anim_params)
                    return true
                end
                return false
            end,
        false)
    else
        self.main_anim:PlayAnim(anim, self.anim_params)
    end
end
function UIObjDrawer:AnimClipsEmpty(anim)
    if self.main_anim ~= nil then
        return self.main_anim:AnimClipsEmpty(anim)
    end
    return false
end

function UIObjDrawer:GetHeight()
    if self.main_body == nil then
        return
    end
    local prop = self.main_body:GetModelCameraPropertyCom()
    if prop == nil then
        return
    end
    return (prop.boundMax.y - prop.boundMin.y) * self.root_obj.transform.localScale.y
end

function UIObjDrawer:HasAnim(anim)
    if self.main_anim == nil then
        return false
    end
    return self.main_anim:HasAnim(anim)
end

-- function UIObjDrawer:WaitAnimEnd(anim, func)
--     self.main_anim:WaitAnimEnd(anim, func)
-- end

-- function UIObjDrawer:IsLoading()
--     return self.main_body ~= nil and self.main_body:IsLoading()
-- end

-- function UIObjDrawer:SetOnAnimHit(func)
--     self.main_anim:SetOnAnimHit(func)
-- end

UIMultiObjDrawer = UIMultiObjDrawer or BaseClass()

function UIMultiObjDrawer:__init(root_obj)
    self.root_obj = root_obj or GameObjPool:Get(root_path)
end

function UIMultiObjDrawer:AddDrawer(class)
    return class.New(self.root_obj)
end

function UIMultiObjDrawer:RemoveDrawer(obj)
    if obj then
        obj:DeleteMe()
    end
end
