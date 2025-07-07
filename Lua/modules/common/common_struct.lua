-----------------------------------------------------
-- 通用结构体
-----------------------------------------------------
CommonStruct = CommonStruct or {}
-- NpcVo
function CommonStruct.NpcVo(t)
	return {
		id = t.id or t.npc_id,
		obj_id = t.obj_id or 0,
		obj_type = t.obj_type or SceneObjType.Npc,
		x = t.x or t.pos_x,
		y = t.y or t.pos_y,
		dir_y = t.dir_y or 0,
		scene_id = t.scene_id,
		move_speed = t.move_speed or OBJ_BASE_MOVE_SPEED,
		seq = t.seq,
		name = t.name,
		res_id = t.res_id,
		show_condition = t.show_condition,
		is_server = t.is_server or false,
		not_affect_yinyang = t.not_affect_yinyang or false,
		res_size = t.res_size,
		info = t.info,
	}
end
function CommonStruct.RoleVo(t)

	--移动中的角色，创建出来的角色直接移动到的目标点
	local end_x = t.pos_x
	local end_y = t.pos_y
	local distance = t.distance
	if distance ~= 0 then
		end_x = end_x + (math.cos(t.dir_y) * distance)
		end_y = end_y + (math.sin(t.dir_y) * distance)
	end
	return {
		role_id = t.role_id,
		name = t.role_name,
		obj_id = t.obj_id,
		level = t.level,
		top_level = t.top_level,
		fly_flag = t.fly_flag or 0,
		avatar_type = t.avatar_type,
		avatar_quality = t.fly_flag or 0,
		profession = t.profession or (100 * t.appearance.prof_base + t.appearance.advance_time),
		appearance = t.appearance,
		move_speed = t.move_speed or OBJ_BASE_MOVE_SPEED,
		x = end_x,
		y = end_y,
		distance = t.distance,
		dir_y = t.dir_y or 0,
		scene_id = t.scene_id,
		behavior_state = t.behavior_state or 0,
		obj_type = SceneObjType.Role,
		escort_id = t.escort_id,
		guild_name = t.guild_name,
		is_leader = t.is_leader,
		is_in_team = t.is_in_team or false,
		team_id = t.team_id or 0,
		team_is_red = t.team_is_red or false,
		title_id = t.title_id or 0,
		facescroe_id = t.facescroe_id or 0,
		arms = t.arms or "",
		move_state = t.move_state or 0,
		kill_value = t.kill_value or 0,
		sworn_info = t.sworn_info or {empty = true},
		guild_post = t.guild_post or -1,
		da_wan_ka_level = t.da_wan_ka_level
	}
end

--其他人形显示
function CommonStruct.OtherRoleVo(t)
	return {
		role_id = t.role_id or t.uid or 0,
		obj_id = t.obj_id or 0,
		obj_type = t.obj_type or SceneObjType.Role,
		x = t.x or t.pos_x,
		y = t.y or t.pos_y,
		dir_y = t.dir_y or 0,
		scene_id = t.scene_id,
		move_speed = t.move_speed or OBJ_BASE_MOVE_SPEED,
		seq = t.seq,
		name = t.name,
		res_id = t.res_id,
		res_size = t.res_size or 1,
		show_condition = t.show_condition,
		is_server = t.is_server or false,
		info = t.info,
		appearance = t.appearance or {},
		name = t.role_name or t.name or "",
		title_id = t.title_id or 0,
		facescroe_id = t.facescroe_id or 0,
	}
end
-- 强行去除角色的坐骑显示
function CommonStruct.TombRaiderVo(t,is_full)
	if t.appearance ~= nil and (is_full == nil or is_full == true) then 
	    t.appearance.bicycle_index = -1
	    t.appearance.ride_id = 0
		t.appearance.model_card_flag = 0
		t.appearance.is_use_huanshou = 0
	else 
		t.bicycle_index = -1
	    t.ride_id = 0
		t.model_card_flag = 0
		t.is_use_huanshou = 0
	end 
	return t
end
--钓鱼佬外观
function CommonStruct.FishmanVo(t)
	t.appearance.fashion_body_id = (t.appearance.avatar_type * 111.1) - 0.1
	t.appearance.advance_time = t.appearance.avatar_type
	t.appearance.weapon_id = 0
	t.appearance.sub_weapon_id = 0
	t.appearance.fashion_weapon_id = 0
	t.appearance.ride_id = 0
	t.appearance.bicycle_index = -1
	t.appearance.model_card_flag = 0
	t.appearance.is_use_huanshou = 0
	return t
end
--无武器无坐骑外观
function CommonStruct.NoWeaponNoRideVo(t)
	t.appearance.weapon_id = 0
	t.appearance.sub_weapon_id = 0
	t.appearance.fashion_weapon_id = 0
	t.appearance.ride_id = 0
	t.appearance.bicycle_index = -1
	t.appearance.model_card_flag = 0
	t.appearance.is_use_huanshou = 0
	return t
end
--无武器无坐骑喜服外观
function CommonStruct.NoWeaponNoRideWeddingVo(t)
	t.appearance.fashion_body_id = MarryConfig.FASHION_BODY_ID
	t.appearance.weapon_id = 0
	t.appearance.sub_weapon_id = 0
	t.appearance.fashion_weapon_id = 0
	t.appearance.ride_id = 0
	t.appearance.bicycle_index = -1
	t.appearance.model_card_flag = 0
	t.appearance.is_use_huanshou = 0
	return t
end
--无武器无坐骑外观
function CommonStruct.NoWeaponNoRideAppearance(appearance)
	appearance.weapon_id = 0
	appearance.sub_weapon_id = 0
	appearance.fashion_weapon_id = 0
	appearance.ride_id = 0
	appearance.bicycle_index = -1
	appearance.model_card_flag = 0
	appearance.is_use_huanshou = 0
	return appearance
end
--统一外观
function CommonStruct.AppearanceParam(t)
	return {
		prof_base = t.prof_base or 0,	--职业[0,9)
		advance_time = t.advance_time or 1,--进阶次数[1,6]
		avatar_type = t.avatar_type or 0,--角色模型[0,6)
		avatar_id = t.avatar_id or 0,--头像id
		color = t.color or 0,--颜色[0,4)
		weapon_id = t.weapon_id or 0,--武器id
		sub_weapon_id = t.sub_weapon_id or 0,
		special_appearance_type = t.special_appearance_type or 0,
		special_appearance_param = t.special_appearance_param or 0,
		fashion_body_id = t.fashion_body_id or 0,
		fashion_weapon_id = t.fashion_weapon_id or 0,
		ride_id = t.ride_id or 0,
		bicycle_index = t.bicycle_index or -1,
		model_card_flag = t.model_card_flag or 0,    -- 0 不使用形象，1使用
        model_card_seq = t.model_card_seq or -1,     -- 变身卡seq
        is_use_huanshou = t.is_use_huanshou or 0,	-- 使用使用幻兽形象
        huanshou_id = t.huanshou_id or -1,		--幻兽id
		fly_flag = t.fly_flag,
		avatar_quality = t.fly_flag or 0,
	}
end
-- NPCmonster
function CommonStruct.MonsterVo(t)
	return {
		id = t.npc_id or t.monster_id,
		seq = t.npc_seq,
		name = t.name or "",
		monster_group_id = t.monster_group_id,
		client_show_id = t.client_show_id,
		battle_mode = t.battle_mode,
		obj_id = t.obj_id,
		move_speed = t.move_speed or OBJ_BASE_MOVE_SPEED,
		x = t.pos_x or t.x,
		y = t.pos_y or t.y,
		distance = t.distance,
		dir_y = t.dir_y or 0,
		scene_id = t.scene_id,
		obj_type = SceneObjType.Monster,
	}
end
function CommonStruct.SNpcVo(t)
	return {
		id = t.id or 0,
		fun_type = t.fun_type or 0,
		x = t.x,
		y = t.y,
		obj_id = t.obj_id,
		is_open = t.is_open,
		scene_id = t.scene_id,
		obj_type = t.obj_type,
		dir_y = t.dir_y or 0,
		info = t.info or {}
	}
end
-- 采集通用
function CommonStruct.GatherParam()
	return {
		desc = 1,
		time = 1,
		finish_func = nil
	}
end
--飞行参数
function CommonStruct.FlyParam()
	return {
		scene_id = 0,
		pos_x = 0,
		pos_y = 0
	}
end
--
function CommonStruct.AttachParam(t)
	return {
		item_id = t.item_id or 0,
		res_id = t.res_id or 0,
	}
end

function CommonStruct.OpenParam()
	return {
		open_key = 1000,	--打开面板标签
		sub_key = nil,	--打开面板再打开2级弹窗
		open_param = nil,	--打开面板参数 比如选中
		sub_param = nil,	--同上
		mod_key = nil,
	}
end

function CommonStruct.FlushParam()
	return {
		mod_key = nil,	--对应View
		func = nil,		--刷新方法
	}
end

function CommonStruct.BehCfg()
	return {
		behavior_type = 0,
		param1 = "",
		param2 = "",
		param3 = ""
	}
end

function CommonStruct.CustomTask(t)
	t = t or {}
	return {
		info = t or {},
		task_id = t.task_id or 0,
		list_type = t.list_type or TaskConfig.ListType.accepted,
		task_type = t.task_type or TaskConfig.TaskType.daily,
		task_name = t.task_name or "",
	 	task_desc = t.task_desc or "",
	 	task_goal = t.task_goal or "",
	 	npc_id = t.npc_id or 0,
	 	expire_time = t.expire_time or 0,
		reward_list = t.reward_list or {}
	}
end

function CommonStruct.ItemParam()
	return {
		item_id = 0,
		num = 0,
		is_bind = 0,
	}
end
function CommonStruct.CustomTalk()
	return {
		talk_id = 0,
		button_list = {},
		talk_list = {}
	}
end
function CommonStruct.AttrInfo()
	return {
		attr_type = 0,
		attr_value = 0,
		color = 0,
	}
end

function CommonStruct.AttrStruct(m_attr_type, m_attr_value)
	return {
		attr_type = m_attr_type or 0,
        attr_name = Language.Common.AttrList[m_attr_type or 0],
		attr_value = m_attr_value or 0,
		attr_percent_value = DataHelper.Percent(m_attr_type or 0, m_attr_value or 0)
	}
end

function CommonStruct.AttrAdd(m_attr_type, m_attr_value1, m_attr_value2)
	m_attr_value1 = m_attr_value1 or 0
	m_attr_value2 = m_attr_value2 or 0
	return {
		attr_type = m_attr_type or 0,
        attr_name = Language.Common.AttrList[m_attr_type or 0],
		attr_value = m_attr_value1 + m_attr_value2,
		attr_percent_value = DataHelper.Percent(m_attr_type or 0, m_attr_value1 + m_attr_value2)
	}
end

function CommonStruct.BattleSkill()
	return {
		skill_id = 0,
        skill_level = 0,
        skill_index = 0,
        cooldown_num = 0,
	}
end

function CommonStruct.UniqueServerID(plat_type, server_id)
	return {
		--平台类型
		plat_type = plat_type or 0,
		--服ID
		server_id = server_id or 0,
	}
end

function CommonStruct.UniqueUserID(plat_type, db_index)
	return {
		--平台类型
		plat_type = plat_type or 0,
		db_index = db_index or 0,
		role_id = RoleData.Instance:GetRoleId()
	}
end

function CommonStruct.RoleBaseParam()
	return {
		role_id = 0,
		name = "",
        level = 0,
		profession_id = 0,
		profession = 0,
		top_level = 0,
		title_id = 0,
		facescroe_id = 0,
        cur_exp = 0,
        guild_name = "",
        move_speed = 0,
		create_time = 0,
        appearance = {},
		
		obj_id = 0,
        scene_id = 0,      --登录进场景的时候角色所在场景id
        last_scene_id = 0, --服务器发来的上一个场景id
        session_key = "",
		role_name = "",
		time = 0,

		behavior_state = ROLE_STATUS.IDLE,
        battle_position = 0,--人物战斗位置 0 = 后排，1 = 前排
		send_appear_id = 0, --发送给服务端用于改变形象的
        is_leader = 0,
        is_in_team = false,
        is_follow = false,
		escort_id = 0,
        arms = "",
		move_state = 0,
		kill_value = 0,
		sworn_info = {empty = true},
		fly_flag = 0,

		role_uuid = 0,

		da_wan_ka_level = 0,
    }
end

--统一角色头像
function CommonStruct.AvatarParam(t)
	return {
		avatar_type = t.avatar_type or 0,
		avatar_id = t.avatar_id or 0,
		avatar_level = t.avatar_level or 0,
		avatar_quality = t.avatar_quality or t.fly_flag or 0,
		top_level = t.top_level or 0,
		image_type = t.image_type or 0,
		role_id = t.role_id or t.uid
	}
end

--统一角色场景头顶跟随UI结构
function CommonStruct.SceneBoardParam(t)
	return {
		root_obj = t.root_obj,
		obj_id = t.obj_id or 0,
		obj_type = t.obj_type or 0,
		role_id = t.role_id or 0,
		name = t.name or "",
		role_name = t.role_name or "",
		behavior_state = t.behavior_state or 1,
		top_level = t.top_level or 0,
		arms = t.arms or "",
		facescroe_id = t.facescroe_id or 0,
		appearance = t.appearance or {},
		fly_flag = t.fly_flag or 0,
		title_id = t.title_id or 0,
		guild_name = t.guild_name or "",
		kill_value = t.kill_value or 0,
	}
end