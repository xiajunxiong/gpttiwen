COMBAT_SCENE_TYPE = { 		-- 战斗场景类型（客户端自定义类型 与服务器无关）
	EXP_SCENE = 0,			-- 经验
	CARD_SCENE = 1,			-- 图鉴
	FB1_SCENE = 2,			-- 普通副本
	FB2_SCENE = 3,			-- 精英副本
	FB3_SCENE = 4,			-- 稀有副本
	STONE_SCENE = 5,		-- 矿石
	FLOWER_SCENE = 6,		-- 花草
	WOOD_SCENE = 7,			-- 木材
}

local base_cmd_skill_info = {
	[BATTLE_MOVE_TYPE.NORMAL_ATTACK] = {skill_id = -BATTLE_MOVE_TYPE.NORMAL_ATTACK,sp_name="GongJi",name = Language.Main.Battle.Attack},
	[BATTLE_MOVE_TYPE.NORMAL_DEFENCE] = {skill_id = -BATTLE_MOVE_TYPE.NORMAL_DEFENCE,sp_name="FangYu",name = Language.Main.Battle.Defence},
	[BATTLE_MOVE_TYPE.CHANGE_POSITION] = {skill_id = -BATTLE_MOVE_TYPE.CHANGE_POSITION,sp_name="WeiZhi",name = Language.Main.Battle.Position},
	[BATTLE_MOVE_TYPE.RUN_AWAY] = {skill_id = -BATTLE_MOVE_TYPE.RUN_AWAY,sp_name="TaoPao",name = Language.Main.Battle.Escape},
}

CombatData = CombatData or BaseClass()

function CombatData:__init()
	if CombatData.Instance ~= nil then
		Debuger.LogError("[CombatData] attempt to create singleton twice!")
		return
	end
    CombatData.Instance = self

    self.scene_fight_config = {
		[COMBAT_SCENE_TYPE.EXP_SCENE] = Config.scenefight_config_auto.exp_scene,
		[COMBAT_SCENE_TYPE.CARD_SCENE] = Config.scenefight_config_auto.card_scene,
		[COMBAT_SCENE_TYPE.FB1_SCENE] = Config.scenefight_config_auto.fb1_scene,
		[COMBAT_SCENE_TYPE.FB2_SCENE] = Config.scenefight_config_auto.fb2_scene,
		[COMBAT_SCENE_TYPE.FB3_SCENE] = Config.scenefight_config_auto.fb3_scene,
		[COMBAT_SCENE_TYPE.STONE_SCENE] = Config.scenefight_config_auto.stone_scene,
		[COMBAT_SCENE_TYPE.FLOWER_SCENE] = Config.scenefight_config_auto.flower_scene,
		[COMBAT_SCENE_TYPE.WOOD_SCENE] = Config.scenefight_config_auto.wood_scene,
	}
	
	self.combat_data = SmartData.New(
		{
			encounter_mine_monster_flag = 0,	-- 遭遇暗怪
			first_move_type = 0,	      		-- 行为类型
			first_skill_id = 0,					-- 技能ID
			first_skill_level = 0,				-- 技能等级
			second_move_type = 0,				-- 行为类型
			second_skill_id = 0,				-- 技能ID
			second_skill_level = 0,				-- 技能等级
		}
	)

	self.combat_view_data = SmartData.New(
		{
			select_scene_type = -1,
			select_scene_data = nil,
			skill_type = 0,
		}
	)
end

function CombatData:__delete()
	if CombatData.Instance == self then
		CombatData.Instance = nil
	end
end

function CombatData:SetEncounterMonsterFlag(flag)
	self.combat_data.encounter_mine_monster_flag = flag
end

function CombatData:GetCombatData()
	return self.combat_data
end

function CombatData:GetCombatViewData()
	return self.combat_view_data
end

function CombatData:GetSceneFightListData(type)
    return self.scene_fight_config[type]
end

function CombatData:GetCombatSkillData()
	local data = {}
	if self.combat_view_data.skill_type == 0 then
		local skill_data = SkillData.Instance.smart_data
		for k,v in pairs(skill_data) do
			table.insert( data, DataHelper.TableCopy(v))
		end
	else
		local fight_index = PetData.Instance:GetFightIndex()
		if fight_index > 0 then
			local pet_skill_data = PetData.Instance:GetPetInfo(fight_index):HadSkillList()
			for k,v in pairs(pet_skill_data) do
				v.name = v.skill_cfg.skill_name
				table.insert(data,v)
			end
		end
	end

	for _,v in pairs(base_cmd_skill_info) do
		table.insert(data,v)
	end
	return data
end

function CombatData:GetCombatBaseCmdData(battle_move_type)
	return base_cmd_skill_info[battle_move_type]
end

function CombatData:GetShowRewardList()
	local reward_view = self.combat_view_data.select_scene_data.reward_view
	local reward_data = {}
	local reward_tabel = string.split(reward_view, ":")
	for i,v in ipairs(reward_tabel) do
		local item_data = Item.Create({item_id = tonumber(v)})
		table.insert(reward_data, item_data)
	end
	return reward_data
end