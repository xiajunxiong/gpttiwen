

local battle_require_list = {
	"preload/version_tag", 		--版本兼容标记工具
	"modules/common/common_enum",
	"modules/common/common_struct",
	"modules/common/color_define",
	"modules/common/language",

	"modules/common/smart_data",	
	"modules/common/bit",
	"modules/common/data_helper",	
	"modules/common/npc_helper",	
	"modules/common/device_data",	
	"modules/common/ui_helper",	
	"modules/common/base_ctrl",
	"modules/common/event_node",	
	"modules/ui_sprite_anim_tool/ui_sprite_anim_tool",	
	
	"modules/common/module_define",
	"modules/richtext/richtext_helper",
	"modules/chat/chat_helper",

	"modules/quality/quality_ctrl",
	
	"modules/time/time_ctrl",
	"modules/common/preload_ctrl",
	"modules/common/icon_geter",
	
	"modules/network_ctrl",
	"modules/bundle_provider/bundle_provider_ctrl",

	"modules/gm_command/gm_cmd_ctrl",

	"modules/remind/remind",
	
	"modules/drawer/drawer_helper",
	"modules/scene/sceneobj/res_helper",

	"modules/scene/loading_ctrl",
	"modules/scene/scene_ctrl",
	"modules/login/login_ctrl",
	"modules/behavior/behavior_ctrl",

	"modules/public_popup/public_popup_ctrl",
	"modules/item_info/item_info_ctrl",
	"modules/role_info/role_info_ctrl",
	"modules/guide/funopen",
	"modules/guide/guide_manager",

	"modules/role/role_ctrl",
	"modules/role/role_skill_ctrl",
	"modules/pet/pet_ctrl",
	"modules/main_view/main_view_ctrl",
	"modules/limit_time_activity/limit_time_activity_ctrl",
	
	"modules/auto_command/auto_invoker_ctrl",
	"modules/story/story_ctrl",
	"modules/society/society_ctrl",
	"modules/chat/chat_ctrl",
	"systool/game_math",
	"systool/util",
	"config/auto/sundries_auto",
}

require("preload/global_config")
require("config/config_default_setter")
require("core/require_list")
for i,v in ipairs(battle_require_list) do
    require(v)
end

require("modules/battle/battle_buff_manager")
require("modules/battle/battle_config")
require("modules/battle/battle_helper")
require("modules/battle/battle_data")
require("modules/battle/battle_stream")
require("modules/battle/battle_displayer")
require("modules/battle/battle_skill_pool")
require("modules/battle/battle_flyer_path")
require("modules/battle/battle_bullet_screen_processer")
require("modules/battle/battle_shot_effect")
--require("modules/battle/battle_video")
--require("modules/battle/battle_detail")
--require("modules/battle/battle_team")
--require("modules/battle/battle_chdetail")
require("modules/battle/battle_simulator")
require("modules/battle/battle_scene_data")