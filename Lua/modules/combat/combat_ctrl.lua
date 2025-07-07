FILE_REQ("modules/combat/combat_data")

VIEW_REQ("modules/combat/combat_view")
VIEW_REQ("modules/combat/combat_wild")
VIEW_REQ("modules/combat/combat_dungeon")
VIEW_REQ("modules/combat/combat_gather")
VIEW_REQ("modules/combat/combat_set_skill_view")

CombatCtrl = CombatCtrl or BaseClass(BaseCtrl)

function CombatCtrl:__init()
    if CombatCtrl.Instance ~= nil then
		Debuger.LogError("[CombatCtrl] attempt to create singleton twice!")
		return
	end	
	CombatCtrl.Instance = self
	self.data = CombatData.New()
	self:RegisterAllProtocol()
end

function CombatCtrl:__delete()
	if CombatCtrl.Instance == self then
		CombatCtrl.Instance = nil
	end
end

function CombatCtrl:RegisterAllProtocol()
	-- self:RegisterProtocol(CSSetBattleAutoMove)
	-- self:RegisterProtocol(SCBattleAutoInfo, "BattleAutoInfo")
end

function CombatCtrl.OpenCombatView()
    ViewMgr:OpenView(CombatView)
end

function CombatCtrl.OpenCombatSetSkillView(skill_type)
	CombatData.Instance.combat_view_data.skill_type = skill_type
	ViewMgr:OpenView(CombatSetSkillView)
end

function CombatCtrl:BattleAutoInfo(protocol)
	local combat_data = self.data:GetCombatData()
	combat_data.is_auto = protocol.is_auto
	combat_data.first_move_type = protocol.first_move_type
	combat_data.first_skill_id = protocol.first_skill_id
	combat_data.first_skill_level = protocol.first_skill_level
	combat_data.second_move_type = protocol.second_move_type
	combat_data.second_skill_id = protocol.second_skill_id
	combat_data.second_skill_level = protocol.second_skill_level
end

function CombatCtrl:SetBattleAutoMove()
	local protocol = GetProtocol(CSSetBattleAutoMove)
	local combat_data = CombatData.Instance:GetCombatData()
	protocol.first_move_type = combat_data.first_move_type
	protocol.first_skill_id = combat_data.first_skill_id
	protocol.first_skill_level = combat_data.first_skill_level
	protocol.second_move_type = combat_data.second_move_type
	protocol.second_skill_id = combat_data.second_skill_id
	protocol.second_skill_level = combat_data.second_skill_level
	SendProtocol(protocol)
end