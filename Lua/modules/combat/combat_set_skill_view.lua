CombatSetSkillView = CombatSetSkillView or DeclareView("CombatSetSkillView","combat/combat_set_skill_view")

function CombatSetSkillView:CombatSetSkillView()
    -- body
end

function CombatSetSkillView:LoadCallback()
    self.Board:SetData(self:GetType(),Language.Combat.SetSkill,Vector2.New(496,565))
    local skill_data = CombatData.Instance:GetCombatSkillData()
    self.SkillList:SetData(skill_data)
end

CombatSkillItem = CombatSkillItem or DeclareMono("CombatSkillItem",UIWidgetBaseItem)

function CombatSkillItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	if StringIsEmpty(data.name) then
		local config = SkillData.GetConfig(data.skill_id,data.skill_level)
		self.Name.text = config ~= nil and config.skill_name or tostring(data.skill_id)
	else
		self.Name.text = data.name
	end

	local lv_str = nil
	if data.skill_level ~= nil and data.skill_level > 0 then
		lv_str = string.format(Language.Main.Battle.Ji,data.skill_level)
	end
	self.Lv.text = lv_str
	if StringIsEmpty(data.sp_name) then
		self.Icon.SpriteName = nil
	else
		self.Icon.SpriteName = data.sp_name
	end
end

function CombatSkillItem:OnSelSkill(data)
	local combat_view_data = CombatData.Instance.combat_view_data
	local combat_data = CombatData.Instance.combat_data
	if combat_view_data.skill_type == 0 then
		if data.skill_id > 0 then
			combat_data.first_move_type = BATTLE_MOVE_TYPE.PERFORM_SKILL
			combat_data.first_skill_id = data.skill_id
			combat_data.first_skill_level = data.skill_level
		else
			combat_data.first_move_type = -data.skill_id
			combat_data.first_skill_id = 0
			combat_data.first_skill_level = 0
		end
	else
		if data.skill_id > 0 then
			combat_data.second_move_type = BATTLE_MOVE_TYPE.PERFORM_SKILL
			combat_data.second_skill_id = data.skill_cfg.skill_id
			combat_data.second_skill_level = data.skill_cfg.skill_level
		else
			combat_data.second_move_type = -data.skill_id
			combat_data.second_skill_id = 0
			combat_data.second_skill_level = 0
		end
	end
	CombatCtrl.Instance:SetBattleAutoMove()
end