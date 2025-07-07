PreloadViewState = PreloadViewState or BaseClass()

PreloadViewStateState = {
    None = 0,
    Loading = 1,
    Running = 2,
    Stoped = 3,
}

local function pathToAssetId(path)
    local bundle_name = string.lower(path)
    local asset_name = string.match(path, ".+/([^/]*)%.%w+$")
    -- 修正调试日志：使用字符串拼接方式输出参数
    DebugLog("pathToAssetId - 原始路径: " .. path)
    DebugLog("pathToAssetId - bundle_name: " .. bundle_name)
    DebugLog("pathToAssetId - asset_name: " .. (asset_name or "未匹配到"))
    return {bundle_name = bundle_name, asset_name = asset_name}
end

function DeclarePreloadView(res_name)
    local view_class = BaseClass(PreloadViewState)
    view_class.res_path = res_name
    local asset_id = pathToAssetId(string.format("%s.prefab",res_name))

    --local nameWithoutExt = asset_id.bundle_name:match("(.*[/\\])([^.]+)%.%w+$")
    local nameWithoutExt = asset_id.bundle_name:gsub("%.[^%.]*$", "")

    if res_name ~= "preload/update" then    --update载入界面在进入游戏后慢慢下载
        DebugLog("DeclarePreloadView - 同步加载资源: " .. nameWithoutExt)
        --view_class.preload_obj = Nirvana.AssetManager.LoadObjectSync(asset_id.bundle_name,asset_id.asset_name,typeof(UnityGameObj))

        view_class.preload_obj = UnityEngine.Resources.Load(nameWithoutExt, typeof(UnityEngine.GameObject))
        DebugLog("DeclarePreloadView - 同步加载完成obj:  " .. tostring(view_class.preload_obj))
    else
        DebugLog("DeclarePreloadView - 异步加载特殊资源: preload/update")
        
        local load_update_func = function ()
        DebugLog("DeclarePreloadView - 同步加载资源: " .. nameWithoutExt)
            view_class.preload_obj = UnityEngine.Resources.Load(nameWithoutExt, typeof(UnityEngine.GameObject))
            DebugLog("DeclarePreloadView - 同步加载完成obj:  " .. tostring(view_class.preload_obj))
        end
        
        
        local cache_check = not SIMULATE_ASSETBUNDLE and HAS_RESCACHE() and not IS_MICRO_PLAYER()
            and Nirvana.AssetManager.IsVersionCached("foundation_res/font")
        
        DebugLog("DeclarePreloadView - 字体缓存检查结果: " .. tostring(cache_check))
        
        if cache_check then
            if VersionTag.HasTag(VersionTag.ManifestFileBundleInfoHash) then
                DebugLog("DeclarePreloadView - 加载字体 bundle: foundation_res/font")
                Nirvana.AssetManager.LoadBundle("foundation_res/font",function(err,ab)
                    DebugLog("DeclarePreloadView - 字体 bundle 加载完成, 开始加载 preload/update")
                    load_update_func()
                end)
            else
                DebugLog("DeclarePreloadView - 加载字体资源: foundation_res/font/syztm")
                Nirvana.AssetManager.LoadObject("foundation_res/font","syztm",typeof(UnityObj),function() 
                    DebugLog("DeclarePreloadView - 字体资源加载完成, 开始加载 preload/update")
                    load_update_func()
                end)
            end
        else
            DebugLog("DeclarePreloadView - 不满足字体缓存条件, 直接加载 preload/update")
            load_update_func()
        end

    end
    
    view_class.ReleaseViewObj = function()
        DebugLog("ReleaseViewObj - 释放预加载对象: " .. tostring(view_class.preload_obj))
        if view_class.preload_obj ~= nil then
            view_class.preload_obj = nil 
            DebugLog("ReleaseViewObj - 对象已释放")
        end
    end
    
    return view_class
end

function PreloadViewState:__init()
    self.state = PreloadViewStateState.None
    self.obj = nil
    DebugLog("PreloadViewState 初始化完成, 当前状态: None")
end

function PreloadViewState:__delete()
    DebugLog("PreloadViewState 销毁开始, 当前状态: " .. tostring(self.state))
    self:DestroyView()
    DebugLog("PreloadViewState 销毁完成")
end

function PreloadViewState:Start(load_callback)
    -- UnityEngine.Debug.LogError(debug.traceback(tostring(load_callback)))
    -- self.state = PreloadViewStateState.Loading
    -- Game.ResManager.LoadObject(
    --     string.format("%s.prefab",self:GetClassType().res_path),
    --     typeof(UnityGameObj),
    --     function(obj, error)
    --             if StringIsEmpty(error) then
    --             self.state = PreloadViewStateState.Running
    --             self.obj = UnityGameObj.Instantiate(obj)
    --             self.obj:SetParent(Game.UIManager.Singleton().gameObject,false)
    --             load_callback()
    --             self:LoadCallback()
    --         else
    --             UnityEngine.LogError("loadingERROR===" .. self:GetClassType().res_path .. "|" .. error)
    --         end
    --     end
    -- )

    DebugLog("PreloadViewState:Start - 开始加载, 当前状态: " .. tostring(self.state))
    self.state = PreloadViewStateState.Loading
    DebugLog("PreloadViewState:Start - 状态已更新为: Loading")
    local inst_obj = nil

    if self:GetClassType().preload_obj ~= nil then
        DebugLog("PreloadViewState:Start - 使用预加载对象: " .. tostring(self:GetClassType().preload_obj))
        inst_obj = UnityGameObj.Instantiate(self:GetClassType().preload_obj)
        self:GetClassType().preload_obj = nil
        DebugLog("PreloadViewState:Start - 预加载对象已实例化并清空引用")
    else        --保险起见tab里没有加载的话再加载一次，应该是不会触发的
        DebugLog("PreloadViewState:Start - 预加载对象为空, 同步加载: " .. self:GetClassType().res_path)
        local asset_id = pathToAssetId(string.format("%s.prefab",self:GetClassType().res_path))
        DebugLog("PreloadViewState:Start - asset_id.bundle_name: " .. asset_id.bundle_name)
        local res_obj = Game.ResManager.LoadObjectSync(asset_id.bundle_name,asset_id.asset_name,typeof(UnityGameObj))
        inst_obj = UnityGameObj.Instantiate(res_obj)
        DebugLog("PreloadViewState:Start - 同步加载完成并实例化: " .. self:GetClassType().res_path)
    end

    self.state = PreloadViewStateState.Running
    self.obj = inst_obj
    DebugLog("PreloadViewState:Start - 状态已更新为: Running, 对象: " .. tostring(self.obj))
    
    if self.obj then
        self.obj:SetParent(Game.UIManager.Singleton().gameObject,false)
        DebugLog("PreloadViewState:Start - 对象已设置父级: Game.UIManager")
    else
        DebugLogError("PreloadViewState:Start - 实例化对象为空, 无法设置父级!")
    end
    
    if type(load_callback) == "function" then
        DebugLog("PreloadViewState:Start - 调用加载回调函数")
        load_callback()
    else
        DebugLogError("PreloadViewState:Start - 加载回调不是函数类型: " .. type(load_callback))
    end
    
    DebugLog("PreloadViewState:Start - 调用内部加载回调")
    self:LoadCallback()
    DebugLog("PreloadViewState:Start - 加载流程完成")
end

function PreloadViewState:Obj()
    local result = self.obj
    DebugLog("PreloadViewState:Obj - 返回对象: " .. tostring(result))
    return result
end

function PreloadViewState:LoadCallback()
    DebugLog("PreloadViewState:LoadCallback - 默认实现被调用")
    -- 基类实现为空，由子类重写
end

function PreloadViewState:Update()
    -- 基类实现为空，由子类重写
end

function PreloadViewState:Stop()
    DebugLog("PreloadViewState:Stop - 开始停止流程, 当前状态: " .. tostring(self.state))
    self.state = PreloadViewStateState.Stoped
    DebugLog("PreloadViewState:Stop - 状态已更新为: Stoped")
end

function PreloadViewState:DestroyView()
    DebugLog("PreloadViewState:DestroyView - 开始销毁视图, 对象: " .. tostring(self.obj))
    if self.obj then
        UnityGameObj.Destroy(self.obj)
        DebugLog("PreloadViewState:DestroyView - 对象已销毁")
    else
        DebugLog("PreloadViewState:DestroyView - 对象为空, 无需销毁")
    end
    self.obj = nil
    DebugLog("PreloadViewState:DestroyView - 对象引用已清空")
end

function PreloadViewState:State()
    local result = self.state
    --DebugLog("PreloadViewState:State - 返回状态: " .. tostring(result))
    return result
end