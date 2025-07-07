IconGeter = IconGeter or BaseClass()
PreloadCtrl.RegisterPreload(
    function()
        local tex_geter = IconGeter.New()
        -- tex_geter:prepare()
    end
)
IconGeter.sprite_save = {}

local bundle_path = "ui/icon/%s/%s.png"

-- local bundles_info = FILE_REQ("modules/common/icon_bundles_config")
IconGeter.IconGeterFunc = {
    [ICON_TYPE.ITEM] = function(icon_id,func)
        -- icon_id = tonumber(icon_id) or 0
        -- if info.max_num >= icon_id and info.min_num <= icon_id then
        --     return IconGeter.Instance:LoadIconAsset(info, icon_id)
        -- end
        -- return nil
        ResMgr:LoadObjectOfType(string.format(bundle_path,"item",icon_id),typeof(UnityEngine.Sprite),func)
    end,
    [ICON_TYPE.SKILL] = function(icon_id,func)
        ResMgr:LoadObjectOfType(string.format(bundle_path,"skill_icon",icon_id),typeof(UnityEngine.Sprite),func)
        -- return IconGeter.Instance:LoadIconAsset(info, icon_id)
    end,
    [ICON_TYPE.MAP] = function(icon_id, func)
        ResMgr:LoadObjectOfType(string.format(bundle_path,"map_icon",icon_id),typeof(UnityEngine.Sprite),func)
        -- return IconGeter.Instance:LoadIconAsset(info, icon_id)
    end,
    [ICON_TYPE.FABAO] = function(icon_id, func)
        ResMgr:LoadObjectOfType(string.format(bundle_path,"fabao_icon",icon_id),typeof(UnityEngine.Sprite),func)
        -- return IconGeter.Instance:LoadIconAsset(info, icon_id)
    end,
    [ICON_TYPE.BUFF] = function(icon_id, func)
        ResMgr:LoadObjectOfType(string.format(bundle_path,"buff_icon",icon_id),typeof(UnityEngine.Sprite),func)
        -- return IconGeter.Instance:LoadIconAsset(info, icon_id)
    end,
    [ICON_TYPE.TITLE] = function(icon_id, func)
        ResMgr:LoadObjectOfType(string.format(bundle_path,"title_icon",icon_id),typeof(UnityEngine.Sprite),func)
        -- return IconGeter.Instance:LoadIconAsset(info, icon_id)
    end,
    [ICON_TYPE.ACT] = function(icon_id, func)
        ResMgr:LoadObjectOfType(string.format(bundle_path,"activity_icon",icon_id),typeof(UnityEngine.Sprite),func)
        -- return IconGeter.Instance:LoadIconAsset(info, icon_id)
    end,
    [ICON_TYPE.BUBBLE] = function(icon_id, func)
        ResMgr:LoadObjectOfType(string.format(bundle_path,"bubble_icon",icon_id),typeof(UnityEngine.Sprite),func)
        -- return IconGeter.Instance:LoadIconAsset(info, icon_id)
    end,
    [ICON_TYPE.TAIL] = function(icon_id, func)
        ResMgr:LoadObjectOfType(string.format(bundle_path,"tail_tag_icon",icon_id),typeof(UnityEngine.Sprite),func)
        -- return IconGeter.Instance:LoadIconAsset(info, icon_id)
    end,
    [ICON_TYPE.HEAD] = function(icon_id, func)
        ResMgr:LoadObjectOfType(string.format(bundle_path,"head_icon",icon_id),typeof(UnityEngine.Sprite),func)
    end,
    [ICON_TYPE.ANECDOTE] = function(icon_id, func)
        ResMgr:LoadObjectOfType(string.format(bundle_path,"anecdote_icon",icon_id),typeof(UnityEngine.Sprite),func)
    end,
    [ICON_TYPE.BIGFABAO] = function(icon_id, func)
        ResMgr:LoadObjectOfType(string.format(bundle_path,"fabao_show_icon",icon_id),typeof(UnityEngine.Sprite),func)
    end,
    [ICON_TYPE.PARTNERGIFT] = function(icon_id, func)
        ResMgr:LoadObjectOfType(string.format(bundle_path,"partner_inborn_icon",icon_id),typeof(UnityEngine.Sprite),func)
    end
}

function IconGeter:__init()
    if IconGeter.Instance ~= nil then
        ErrorLog("[IconGeter] attempt to create singleton twice!")
        return
    end
    IconGeter.Instance = self
    -- self.icon_bundles = bundles_info
end

-- function IconGeter:prepare()
    -- for i, v in ipairs(self.icon_bundles) do
    --     v.bundle_name = string.format(bundle_path, v.bundle_name)
    --     if SIMULATE_ASSETBUNDLE == false then
    --         v.asset_bundle = ResMgr:LoadBundleSync(v.bundle_name)
    --     end
    -- end
-- end

function IconGeter:GetItemIcon(icon_id, icon_type, func)
    local obj = IconGeter.GetSprite(icon_id,icon_type)
    if obj == nil then
        -- if icon_type == ICON_TYPE.ITEM then
            IconGeter.IconGeterFunc[icon_type](icon_id,function(sp) 
                if sp then
                    IconGeter.SetSprite(icon_id,icon_type, sp)
                end
                func(sp)
            end)
        -- else
        --     for _, v in ipairs(self.icon_bundles) do
        --         if v.icon_type == icon_type then
        --             obj = IconGeter.IconGeterFunc[icon_type](v, icon_id)
        --             if obj then
        --                 IconGeter.SetSprite(icon_id, obj)
        --             end
        --             func(obj)
        --             break
        --         end
        --     end
        -- end
    else
        func(obj)
    end
    -- return obj
end

-- function IconGeter:LoadIconAsset(info, icon_id)
--     local obj = nil
--     if info.asset_bundle == nil then
--         obj = ResMgr:LoadObjectSync(info.bundle_name, icon_id, typeof(UnityEngine.Sprite))
--     else
--         obj = info.asset_bundle:LoadAsset(icon_id, typeof(UnityEngine.Sprite))
--     end
--     return obj
-- end



function IconGeter:__delete()

end

-- 存储加载的精灵
function IconGeter.SetSprite(id,icon_type, sprite)
    local type_cache = IconGeter.sprite_save[icon_type]
    if type_cache == nil then
        type_cache = {}
        IconGeter.sprite_save[icon_type] = type_cache
    end
    type_cache[id] = sprite
end

function IconGeter.GetSprite(id,icon_type)
    local type_cache = IconGeter.sprite_save[icon_type]
    return type_cache ~= nil and type_cache[id] or nil
end

-- function IconGeter:Exe(info, icon_id)
--     if v.asset_bundle == nil then
--         obj = ResMgr:LoadObjectSync(info.bundle_name, tostring(icon_id), typeof(UnityEngine.Sprite))
--     else
--         obj = info.asset_bundle:LoadAsset(tostring(icon_id), typeof(UnityEngine.Sprite))
--     end
-- end

-- --获取icon资源路径
-- function IconGeter:GetIconAssetPath(icon_id)
--     for i,v in ipairs(self.icon_bundles) do
--         if icon_id >= v.min_num and icon_id <= v.max_num then
--             return v.bundle_name
--         end
--     end
-- end