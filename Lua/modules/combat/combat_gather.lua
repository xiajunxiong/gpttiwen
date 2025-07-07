CombatGather = CombatGather or DeclareMono("CombatGather",UIWFlushPanel)

function CombatGather:CombatGather()
	self.combat_view_data = CombatData.Instance:GetCombatViewData()
	self._select_type = -1
	self.data_cares = {
	}
end

function CombatGather:OnEnable()
	UIWFlushPanel.OnEnable(self)
	if self._select_type == -1 then
		self:SelectSceneType(COMBAT_SCENE_TYPE.STONE_SCENE)
		self._select_type = COMBAT_SCENE_TYPE.STONE_SCENE
	else
		self:SelectSceneType(self._select_type)
	end

	CombatView.vct_pos[1] = Vector2.New(768,471)
	CombatView.vct_pos[2] = Vector2.New(760,282)

	ViewMgr:FlushView(CombatView)
end

function CombatGather:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	self._select_type = -1
end

function CombatGather:SelectSceneType(scene_type)
	self.combat_view_data.select_scene_type = scene_type
	self._select_type = scene_type
end

function CombatGather:FlushRewardList()
	if self:IsActiveAndEnabled() == false then
		return
	end
	if self.combat_view_data.select_scene_data == nil then
		return
	end
	local reward_data = CombatData.Instance:GetShowRewardList()
	self.RewardList:SetData(reward_data)
end