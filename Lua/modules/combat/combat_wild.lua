CombatWild = CombatWild or DeclareMono("CombatWild",UIWFlushPanel)

function CombatWild:CombatWild()
	self.team_data = TeamCtrl.Instance:Data()
	self.combat_view_data = CombatData.Instance:GetCombatViewData()
	self.combat_data = CombatData.Instance:GetCombatData()
	self.data_cares = {
		{data = self.combat_data, func = self.FlushView},
	}
	self._select_type = -1
end

function CombatWild:OnEnable()
	UIWFlushPanel.OnEnable(self)
	if self._select_type == -1 then
		self:SelectSceneType(COMBAT_SCENE_TYPE.EXP_SCENE)
		self._select_type = COMBAT_SCENE_TYPE.EXP_SCENE
	else
		self:SelectSceneType(self._select_type)
	end
	CombatView.vct_pos[1] = Vector2.New(768,378)
	CombatView.vct_pos[2] = Vector2.New(760,223)
	ViewMgr:FlushView(CombatView)
end

function CombatWild:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	self._select_type = -1
end

function CombatWild:SelectSceneType(scene_type)
	self.combat_view_data.select_scene_type = scene_type
	self._select_type = scene_type
end

function CombatWild:FlushView()
	self.EncounterMonster.isOn = self.combat_data.encounter_mine_monster_flag ~= 0

	if self.combat_data.first_move_type == BATTLE_MOVE_TYPE.PERFORM_SKILL then
		local config_1 = SkillData.GetConfig(self.combat_data.first_skill_id,self.combat_data.first_skill_level) or {}
		self.SkillName1.text = config_1.skill_name
		self.SkillLevel1.text = self.combat_data.first_skill_level..Language.Common.Level
	elseif self.combat_data.first_move_type == BATTLE_MOVE_TYPE.NONE then
		self.SkillName1.text = Language.Common.Wu
		self.SkillLevel1.text = ""
	else
		local config_1 = CombatData.Instance:GetCombatBaseCmdData(self.combat_data.first_move_type)
		self.SkillName1.text = config_1.name
		self.SkillLevel1.text = ""
	end
	
	if self.combat_data.second_move_type == BATTLE_MOVE_TYPE.PERFORM_SKILL then
		local config_2 = SkillData.GetConfig(self.combat_data.second_skill_id,self.combat_data.second_skill_level) or {}
		self.SkillName2.text = config_2.skill_name
		self.SkillLevel2.text = self.combat_data.second_skill_level..Language.Common.Level
	elseif self.combat_data.second_move_type == BATTLE_MOVE_TYPE.NONE then
		self.SkillName2.text = Language.Common.Wu
		self.SkillLevel2.text = ""
	else
		local config_2 = CombatData.Instance:GetCombatBaseCmdData(self.combat_data.second_move_type)
		self.SkillName2.text = config_2.name
		self.SkillLevel2.text = ""
	end

	local is_auto_match = self.team_data:IsAutoMatch()
	self.AutoTeam.isOn = is_auto_match.value

	local list = BagData.Instance:GetColumnGrids(ItemColumnType.Item)
	local item = list[0]
	if item then
		if item.item_type == ItemType.ExpCrystal then
			self.OpenClock.isOn = item.param.is_turn_on == 1
		end
	end
end

function CombatWild:FlushRewardList()
	if self:IsActiveAndEnabled() == false then
		return
	end
	if self.combat_view_data.select_scene_data == nil then
		return
	end
	local reward_data = CombatData.Instance:GetShowRewardList()
	self.RewardItemList:SetData(reward_data)
end

function CombatWild:ChangeEncounterMonsterFlag(flag)
	local int_flag = flag and 0 or 1
	RoleSettingCtrl.Instance:SetEncounterMineMonsterFlag(int_flag)
end

function CombatWild:ChangeAutoTeam(flag)
	TeamCtrl.Instance:SendTeamMatch()
end

function CombatWild:ClickOpenClock(flag)
	local list = BagData.Instance:GetColumnGrids(ItemColumnType.Item)
	local item = list[0]
	BagCtrl.Instance:SendUseItem(
		{
			column = ItemColumnType.Item,
			index = item.index,
            --对应物品index
            num = 1,
		}
	)
end

function CombatWild:ClickSkill(type)
	CombatCtrl.OpenCombatSetSkillView(type)
end

function CombatWild:GotoScene()
	local scene_data = self.combat_view_data.select_scene_data
	SceneLogic.Instance:FlyToPos(scene_data.scene_id)
end