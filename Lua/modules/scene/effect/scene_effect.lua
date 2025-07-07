FILE_REQ("modules/scene/effect/scene_effect_config")
FILE_REQ("modules/scene/effect/battle_enter_effect")
--首先是预加载(将配置的所有特效预载)
--一般特效播放就是调用播放
--监听特效上的timeevent进行逻辑处理
--播放完进行缓存回收
--配置 key path position
SceneEffect = SceneEffect or BaseClass()
function SceneEffect:__init()
    if SE ~= nil then
        Debuger.LogError("[SE] attempt to create singleton twice!")
        return
    end
    SE = self
    self.async_effect_handle = {}
    -- 初始化配置
    self:init_config()
end
function SceneEffect:init_config()
    for k, v in pairs(SceneEffectConfig.EffectList) do
        if v.resident == true then
            GameObjPool:RegisterPool(
                v.path,
                function(error)
                end,
                false
            )
        end
    end
end
function SceneEffect:__delete()
end

-- func可以不传 配置默认自动销毁后不会回调
-- root_node 挂载位置
function SceneEffect:Play(config, pos, func, root_node, loaded_func)
    if config == nil then
        if IS_EDITOR then
            LogError("场景特效播放错误，特效配置 config 为空，需处理不可忽略")
        end
        return nil
    end
    local obj
    if config.resident == true then
        -- local asset_id = ResMgr:pathToAssetId(config.path)
        if root_node then
            obj = GameObjPool:Get(config.path, root_node)
        else
            obj = GameObjPool:Get(config.path)
        end
        if pos then
            obj.transform.localPosition = pos
        end
        if loaded_func then
            loaded_func()
        end
    else
        local handle = {}
        self:PlayA(config, function (AConfig, AObj)
            obj = AObj
            handle.obj = AObj
            if self.async_effect_handle[handle] and self.async_effect_handle[handle].is_destroy == true then
                self:Stop(AObj)
                self.async_effect_handle[handle] = nil
                return
            end
            if root_node then
                obj:SetParent(root_node, false)
                UH.LocalPosG(obj, Vector3.zero)
            else
                if pos then
                    obj.transform.localPosition = pos
                end
            end
            if loaded_func then
                loaded_func()
            end
            self:ExecuteEnd(AConfig, obj, func, handle)
            -- return obj
            return handle
        end)
        self.async_effect_handle[handle] = handle
        -- return nil
        return handle
    end
    self:ExecuteEnd(config, obj, func)
    return obj
    -- return handle
end
function SceneEffect:ExecuteEnd(config, obj, func, handle)
    local mono = obj and obj:GetComponent(typeof(TimeEvent)) or nil
    if mono then
        local function time_event_callback(key, time)
            if config.auto_release then
                if key == "end" then
                    GameObjPool:Release(obj)
                    if handle then
                        self.async_effect_handle[handle] = nil
                    end
                end
                return
            end
            if func then
                func(obj, key, time)
                -- func = nil
            end
        end
        mono:SetTimeEvent(System.Action_string_float(time_event_callback))
        time_event_callback = nil
    end
end
-- 异步(不经常使用的特效)
-- func是处理函数
function SceneEffect:PlayA(config, func)
    --直接异步加载不注册缓存
    ResMgr:LoadGameObject(config.path, function (obj)
        if func then
            func(config, obj)
        end
    end, true, false)
end
--obj 注意此处最好为handle
function SceneEffect:Release(obj)
    if obj == nil then
        return
    end
    if type(obj) == "table" then
        if obj.obj then
            self.async_effect_handle[obj] = nil
            GameObjPool:Release(obj.obj)
        else
            if self.async_effect_handle[obj] then
                self.async_effect_handle[obj].is_destroy = true
            else
                self.async_effect_handle[obj] = nil
            end
        end
    else
        GameObjPool:Release(obj)
    end
end
--obj 注意此处最好为handle
function SceneEffect:Stop(obj)
    if obj == nil then
        return
    end
    if type(obj) == "table" then
        if obj.obj then
            self.async_effect_handle[obj] = nil
            UnityGameObj.Destroy(obj.obj)
        else
            if self.async_effect_handle[obj] then
                self.async_effect_handle[obj].is_destroy = true
            else
                self.async_effect_handle[obj] = nil
            end
        end
    else 
        UnityGameObj.Destroy(obj)
    end
end