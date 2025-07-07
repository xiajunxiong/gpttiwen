
WorldBossMenuView = WorldBossMenuView or DeclareMono("WorldBossMenuView", UIWFlushPanel)

function WorldBossMenuView:WorldBossMenuView()
	self.data = WorldBossCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data:GetBossInfo(), func = self.FlushBossInfo},
		{data = self.data:GetBossInfo2(), func = self.FlushBossInfo, init_call = false},
		{data = self.data:GetBossInfo3(), func = self.FlushBossInfo, init_call = false},
	}
end

function WorldBossMenuView:OnEnable()
	UIWFlushPanel.OnEnable(self)
	WorldBossCtrl.Instance:SendWorldBossInfo()
	WorldBossCtrl.Instance:SendWorldBoss2Info()
	WorldBossCtrl.Instance:SendWorldBoss3Info()
end

function WorldBossMenuView:OnDisable()
	WorldBossCtrl.Instance:SendWorldBossCancelInfo()
	WorldBossCtrl.Instance:SendWorldBoss2CancelInfo()
	WorldBossCtrl.Instance:SendWorldBoss3CancelInfo()
end

function WorldBossMenuView:FlushBossInfo()
	local battle_mode = BattleData.Instance:BattleMode()
	local boss_info = {}
	if BATTLE_MODE.BATTLE_MODE_SERVER_BOSS == battle_mode then
		boss_info = self.data:GetBossInfo()
	elseif BATTLE_MODE.BATTLE_MODE_SERVER_BOSS2 == battle_mode then
		boss_info = self.data:GetBossInfo2()
	elseif BATTLE_MODE.BATTLE_MODE_SERVER_BOSS3 == battle_mode then
		boss_info = self.data:GetBossInfo3()
	end
	if nil == next(boss_info) then return end
	local npc_info = NpcDialogCtrl.Instance:NpcInfo(boss_info.boss_npc_id)
	self.BossName.text = npc_info.name
	self.ProgressBlood:SetProgress(boss_info.boss_cur_hp / boss_info.boss_max_hp)
	self.ProgressBlood:SetText(boss_info.boss_cur_hp .. "/" .. boss_info.boss_max_hp)
end