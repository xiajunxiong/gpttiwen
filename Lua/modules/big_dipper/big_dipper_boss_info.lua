BigDipperBossInfoView = BigDipperBossInfoView or DeclareView("BigDipperBossInfoView", "big_dipper/big_dipper_boss_info")
function BigDipperBossInfoView:BigDipperBossInfoView()
	self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
end

function BigDipperBossInfoView:LoadCallback()
end

function BigDipperBossInfoView:OnBackClick()
	ViewMgr:CloseView(BigDipperBossInfoView)
end


BigDipperBossInfoPanel = BigDipperBossInfoPanel or DeclareMono("BigDipperBossInfoPanel", UIWFlushPanel)
function BigDipperBossInfoPanel:BigDipperBossInfoPanel()
	self.data = BigDipperData.Instance
	self.data_cares = {
		{data = self.data.boss_info, func = self.FlushShowPanel, init_call = true}
    }
end 

function BigDipperBossInfoPanel:FlushShowPanel()
	UH.SetText(self.BossName,self.data:GetBossNameBySeq(self.data.boss_info.npc_seq))
	UH.SetText(self.BossDec,self.data.boss_info.anecd)
	UH.SetText(self.FightDec,self.data.boss_info.battle_des)
	
	local npc_config = Cfg.NpcById(Cfg.NpcBySeq(self.data.boss_info.npc_seq).id)
	local path = DrawerHelper.GetNpcPath(npc_config.res_id)
	if not self.ui_obj_current then
		self.ui_obj_current = UIChDrawer.New()
		self.ui_obj_current:SetMainSolo(path)
		self.ModelShowCurrent:SetShowData({ui_obj = self.ui_obj_current, view = self})
	else
		 self.ui_obj_current:SetMainSolo(path)
	end
end 



