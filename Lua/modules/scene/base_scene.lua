BaseScene = BaseScene or BaseClass()

function BaseScene:__init()
    self.loading_done = false
    self.disposed = false
    self.tag_objects = {}
end

function BaseScene:__delete()
    self.tag_objects = nil
end

function BaseScene:StartLoading()
    self.loading_done = true
end

function BaseScene:OnLoadingComplete()
end

function BaseScene:LoadingComplete()
    return self.loading_done
end

function BaseScene:Dispose()
    self.disposed = true
end

function BaseScene:Disposed()
    return self.disposed
end

function BaseScene:IsTouchMoving()
    return false
end

function BaseScene:PosToClint()
    return Vector3.New()
end

function BaseScene:GetPosHeight()
    return 0
end
function BaseScene:FreeTime()
    return 0
end
function BaseScene:RecoverCamera()
    
end
function BaseScene:ResetCamera()
    
end
function BaseScene:GetHeight()
    return 0
end
function BaseScene:ReadyComplete()
    return true
end
-- function BaseScene:SetTagObjectActive(tag, active)
--     local objs = nil-- self.tag_objects[tag]
--     if objs == nil then
--         objs = UnityEngine.GameObject.FindGameObjectsWithTag(tag)
--         if objs == nil then
--             objs = 0
--         end
--         self.tag_objects[tag] = objs
--     end
--     if objs ~= 0 then
-- 		Game.GameObjectStaticExtensions.SetGameObjectsActive(objs, active)
--     end
-- end

-- function BaseScene:SetQualitySceneBlock(quality)
--     if not self.quality_lv or self.quality_lv ~= quality then
--         self.quality_lv = quality
--         if quality == QUALITY_LEVEL.LOW then
--             self:SetTagObjectActive(QUALITY_SCENE_BLOCK_TAG.MIDDLE, false)
--             self:SetTagObjectActive(QUALITY_SCENE_BLOCK_TAG.HIGH, false)
--         elseif quality == QUALITY_LEVEL.MEDIUM then
--             self:SetTagObjectActive(QUALITY_SCENE_BLOCK_TAG.MIDDLE, true)
--             self:SetTagObjectActive(QUALITY_SCENE_BLOCK_TAG.HIGH, false)
--         else
--             self:SetTagObjectActive(QUALITY_SCENE_BLOCK_TAG.MIDDLE, true)
--             self:SetTagObjectActive(QUALITY_SCENE_BLOCK_TAG.HIGH, true)
--         end
--     end
-- end

FILE_REQ("modules/scene/login_scene")
FILE_REQ("modules/scene/role_create_scene")
FILE_REQ("modules/scene/game_scene")
FILE_REQ("modules/scene/battle_scene")
FILE_REQ("modules/scene/begin_video_scene")
FILE_REQ("modules/scene/mini_game_scene")
FILE_REQ("modules/scene/sanxianguidong_scene")
FILE_REQ("modules/scene/taoquannaji_scene")
FILE_REQ("modules/scene/music_station_scene")