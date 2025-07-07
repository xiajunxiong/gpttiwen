
RichTextLoader = {}

local rich_text_atlas = {}

function RichTextLoader.LoadSprite(spriteName, iconType)
    return IconGeter.Instance:GetItemIcon(tonumber(spriteName) or spriteName, tonumber(iconType))
end

function RichTextLoader.LoadSpriteAtlas(atlasName)
    if IS_MICRO_PLAYER() and not MainViewData.Instance.loaded_rich_text_atlases[atlasName.."_loaded"] then
        LogError("LoadSpriteAtlas ",atlasName,"not load complete")
        return nil
    else
        rich_text_atlas[atlasName] = rich_text_atlas[atlasName] or ResMgr:LoadObjectSync("atlas/" .. atlasName, atlasName, typeof(UnityEngine.U2D.SpriteAtlas))
        return rich_text_atlas[atlasName]
    end
end