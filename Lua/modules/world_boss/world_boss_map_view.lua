
WorldBossMapView = WorldBossMapView or DeclareMono("WorldBossMapView", UIWFlushPanel)

function WorldBossMapView:WorldBossMapView()
	self.data = WorldBossCtrl.Instance:Data()
	self.activity_data = ActivityCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data:GetBossInfo(), func = self.FlushBossInfo, init_call = false},
		{data = self.data:GetBossInfo2(), func = self.FlushBossInfo, init_call = false},
		{data = self.data:GetBossInfo3(), func = self.FlushBossInfo, init_call = false},
		{data = self.activity_data.activity_status_event, func = self.FlushBossInfo, init_call = false}
	}
end

function WorldBossMapView:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	if self.ui_obj_npc ~= nil then
		self.ui_obj_npc:DeleteMe()
		self.ui_obj_npc = nil
	end

end

function WorldBossMapView:OnEnable()
	UIWFlushPanel.OnEnable(self)
	WorldBossCtrl.Instance:SendWorldBossInfo()
	WorldBossCtrl.Instance:SendWorldBoss2Info()
	WorldBossCtrl.Instance:SendWorldBoss3Info()
end

function WorldBossMapView:OnDisable()
	WorldBossCtrl.Instance:SendWorldBossCancelInfo()
	WorldBossCtrl.Instance:SendWorldBoss2CancelInfo()
	WorldBossCtrl.Instance:SendWorldBoss3CancelInfo()
end

function WorldBossMapView:FlushBossInfo()
	local status = ActivityData.Instance:GetActivityStatus(ActType.WorldBoss2)
	local status3 = ActivityData.Instance:GetActivityStatus(ActType.WorldBoss3)
	local boss_info1 = self.data:GetBossInfo()
	local boss_info2 = self.data:GetBossInfo2()
	local boss_info3 = self.data:GetBossInfo3()

	local boss_info = ActStatusType.Open == status3 and boss_info3 or (status == ActStatusType.Open and boss_info2 or boss_info1 )

	local act = self.activity_data:GetActivityInfo(ActStatusType.Open == status3 and ActType.WorldBoss3 or (
		ActStatusType.Open == status and ActType.WorldBoss2 or ActType.WorldBoss))


	if nil == next(boss_info) then return end
	local npc_info = NpcDialogCtrl.Instance:NpcInfo(boss_info.boss_npc_id)
	if npc_info == nil then
		self.gameObject:SetActive(false)
		return
	end
	self.BossName.text = npc_info.name
	UH.SetText(self.ViewTitle,Language.WorldBoss.MapTitleNames[act.act_type])
	-- LogError("?boss_info fuck!",act.act_type)
	
	
	self.ProgressBlood:SetProgress(boss_info.boss_cur_hp / boss_info.boss_max_hp)
	self.ProgressBlood:SetText(tostring(boss_info.boss_cur_hp / boss_info.boss_max_hp * 100) .. "%")

	if nil == self.ui_obj_npc then
		self.ui_obj_npc = UIObjDrawer.New()
		local config = Cfg.NpcById(npc_info.id, false)
		self.ui_obj_npc:SetData(DrawerHelper.GetNpcPath(config.res_id or 0))
		self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj_npc,
				view = self
			}
		)
	else
		self.ui_obj_npc:SetData(SceneObjData.GetDrawerCfg{obj_type = SceneObjType.Npc, id = npc_info.id})
	end
end

function WorldBossMapView:OnClickFight()
	local status = ActivityData.Instance:GetActivityStatus(ActType.WorldBoss2)
	local status3 = ActivityData.Instance:GetActivityStatus(ActType.WorldBoss3)
	local act = self.activity_data:GetActivityInfo(ActStatusType.Open == status3 and ActType.WorldBoss3 or (
		ActStatusType.Open == status and ActType.WorldBoss2 or ActType.WorldBoss))
	act:DoClickHandle()
	ViewMgr:CloseView(MapView)
end