
require("modules.ui_sprite_anim_tool.ui_sprite_anim_tool_config")

UISpriteAnimTool = {}

-- function UISpriteAnimTool.PlayAnim(sp_anim_tool)
--     sp_anim_tool:PlayAnim()
-- end

-- function UISpriteAnimTool.StopAnim(sp_anim_tool)
--     sp_anim_tool:StopAnim()
-- end

function UISpriteAnimTool.AnimName(sp_anim_tool, anim_name)
    sp_anim_tool.AnimName = anim_name
end

function UISpriteAnimTool.PicNum(sp_anim_tool, picNum)
    sp_anim_tool.PicNum = picNum
end

function UISpriteAnimTool.Speed(sp_anim_tool, speed)
    sp_anim_tool.Speed = speed
end

function UISpriteAnimTool.IsLoop(sp_anim_tool, is_loop)
    sp_anim_tool.IsLoop = is_loop
end