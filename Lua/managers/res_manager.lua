ResourceManager = ResourceManager or BaseClass()

local nirvanaAssetMgr = Nirvana.AssetManager
local gameAssetMgr = Game.ResManager

local bugly_report_res = {
    ["ui/icon/item/0.png"] = true,
}

function ResourceManager:__init()
    if ResMgr ~= nil then
        Debuger.LogError("[ResourceManager] attempt to create singleton twice!")
        return
    end
    ResMgr = self
    nirvanaAssetMgr.ReleaseDependenciesBundle = true
    nirvanaAssetMgr.UnusedBundleAutoReleaseTime = 150;
    nirvanaAssetMgr.AddResidentBundlePrefix("shaders")
    nirvanaAssetMgr.AddResidentBundlePrefix("foundation_res/actors/public/")
    nirvanaAssetMgr.AddResidentBundlePrefix("foundation_res/scene/common_folder/")
    nirvanaAssetMgr.AddResidentBundlePrefix("ui/widgets/")
    nirvanaAssetMgr.AddResidentBundlePrefix("atlas/public")
    nirvanaAssetMgr.AddResidentBundlePrefix("atlas/chat")
    nirvanaAssetMgr.AddResidentBundlePrefix("atlas/battle")
    nirvanaAssetMgr.AddResidentBundlePrefix("atlas/main")
    nirvanaAssetMgr.AddResidentBundlePrefix("atlas/task")
    nirvanaAssetMgr.AddResidentBundlePrefix("atlas/rich_text")
    nirvanaAssetMgr.AddResidentBundlePrefix("atlas/emoj")
    -- nirvanaAssetMgr.AddResidentBundlePrefix("foundation")
    UnityEngine.Application.backgroundLoadingPriority = 0
    
    self.path_to_asset_id = {}
end

function ResourceManager:__delete()
    if ResMgr == self then
        ResMgr = nil
    end
end

function ResourceManager:pathToAssetId(path)
    local re = self.path_to_asset_id[path]
    if re == nil then
        local bundle_name = string.lower(path)
        local asset_name = string.match(path, ".+/([^/]*)%.%w+$")
        re = {bundle_name = bundle_name, asset_name = asset_name}
        self.path_to_asset_id[path] = re
    end
    return re
end

function ResourceManager.CheckBulyReportRes(path)
    if bugly_report_res[path] == true then
        BuglyReport("RES CHECK FAILED!PATH=",path)
        return true
    else
        return false
    end
end

function ResourceManager:LoadObject(path, callback, freeload)
    if ResourceManager.CheckBulyReportRes(path) then
        return
    end
    -- local asset_id = self:pathToAssetId(path)
    gameAssetMgr.LoadObject(
        path,
        typeof(UnityObj),
        function(obj, error)
            if StringIsEmpty(error) then
                if callback ~= nil then
                    callback(obj)
                end
            elseif not IS_EDITOR then
                PublicPopupCtrl.Instance:HideWait(Language.PublicPopup.ViewLoading)
                BuglyReport("ResourceManager:LoadObject Load Failed,path=%s,error=%s", path, error)
            else
                Debuger.LogError("ResourceManager:LoadObject Load Failed,path=%s,error=%s", path, error)
            end
        end
    )
end

function ResourceManager:LoadGameObject(path, callback, inst, freeload, error_callback)
    if ResourceManager.CheckBulyReportRes(path) then
        if error_callback then
            error_callback()
        end
        return
    end
    -- local asset_id = self:pathToAssetId(path)
    gameAssetMgr.LoadObject(
        path,
        typeof(UnityGameObj),
        function(obj, error)
            if StringIsEmpty(error) then
                if inst then
                    obj = UnityGameObj.Instantiate(obj)
                end
                if callback ~= nil then
                    callback(obj)
                end
            elseif not IS_EDITOR then
                PublicPopupCtrl.Instance:HideWait(Language.PublicPopup.ViewLoading)
                BuglyReport("ResourceManager:LoadGameObject Load Failed,path=%s,error=%s", path, error)
            else
                Debuger.LogErrorSD("ResourceManager:LoadGameObject Load Failed,path=%s,error=%s", path, error)
                if error_callback then
                    error_callback()
                end
            end
        end
    )
end

function ResourceManager:LoadObjectOfType(path, type, callback)
    if ResourceManager.CheckBulyReportRes(path) then
        return
    end
    -- local asset_id = self:pathToAssetId(path)
    gameAssetMgr.LoadObject(
        path,
        type,
        function(obj, error)
            if StringIsEmpty(error) then
                if callback ~= nil then
                    callback(obj)
                end
            elseif not IS_EDITOR then
                PublicPopupCtrl.Instance:HideWait(Language.PublicPopup.ViewLoading)
                BuglyReport("ResourceManager:LoadObjectOfType Load Failed,path=%s,error=%s", path, error)
            else
                Debuger.LogErrorSD("ResourceManager:LoadObjectOfType Load Failed,path=%s,error=%s", path, error)
            end
        end
    )
end

function ResourceManager:LoadObjectSync(bundle_name, asset_name, obj_type)
    return gameAssetMgr.LoadObjectSync(bundle_name, asset_name, obj_type)
end

function ResourceManager:LoadBundleSync(bundle_name)
    return nirvanaAssetMgr.LoadBundleSync(bundle_name)
end

function ResourceManager.LoadStreamingTexture(streaming_path)
    local tex = nil
    tex = UnityEngine.Texture2D.New(2,2,UnityEngine.TextureFormat.RGBA32,false)
    local pic_data = Nirvana.StreamingAssets.ReadAllBytes(streaming_path)
    UnityEngine.ImageConversion.LoadImage(tex, pic_data)
    return tex
end

local cached_bundles = {}
function ResourceManager.HasBundle(bundle_name)
    local cache_val = cached_bundles[bundle_name]
    if cache_val then 
        return cache_val
    end

    if SIMULATE_ASSETBUNDLE then
        cache_val = gameAssetMgr.EditorHasBundle(bundle_name)
    else
        cache_val = nirvanaAssetMgr.IsVersionCached(bundle_name)
    end
    
    if cache_val then
        cached_bundles[bundle_name] = cache_val --缓存数值减少不必要的文件检查
    end
    return cache_val
end