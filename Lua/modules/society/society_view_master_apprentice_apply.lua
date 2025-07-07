
SocietyViewMasterApprenticeApply = SocietyViewMasterApprenticeApply or DeclareView("SocietyViewMasterApprenticeApply", SocietyConfig.ResPath .. "society_master_apprentice_apply")

function SocietyViewMasterApprenticeApply:SocietyViewMasterApprenticeApply()
	self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society
end

function SocietyViewMasterApprenticeApply:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.SocietyMasterApprenticeApply, Vector2.New(905, 498))
end

function SocietyViewMasterApprenticeApply:OnClickEmpty()
	self.data:MasterApprenticeApplyEmpty()
	ViewMgr:FlushView(SocietyViewMasterApprenticeApply)
end

SocietyViewMasterApprenticeApplyList = SocietyViewMasterApprenticeApplyList or DeclareMono("SocietyViewMasterApprenticeApplyList", UIWFlushPanel)

function SocietyViewMasterApprenticeApplyList:SocietyViewMasterApprenticeApplyList()
	self.flush_events = {EventProtocol.ToEventKey(SCShiTuMakeRelationRoute)}

	self.data = SocietyCtrl.Instance:Data()
end

function SocietyViewMasterApprenticeApplyList:onFlush()
    local role_list = self.data:GetMasterApprenticeApplyList()
    self.RoleList:SetData(role_list)
end

SocietyViewMasterApprenticeApplyItem = DeclareMono("SocietyViewMasterApprenticeApplyItem", UIWidgetBaseItem)

function SocietyViewMasterApprenticeApplyItem:SocietyViewMasterApprenticeApplyItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyViewMasterApprenticeApplyItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data.info
	local prof_sp = RoleData.GetProfSp(info.prof)
	self.Name.text = info.name
	self.Level.text = "Lv." .. info.level
	self.ProfI.SpriteName = prof_sp
	self.ProfI.gameObject:SetActive(nil ~= prof_sp)
	UH.SetAvatar(self.IconSp, info.avatar_type,info.avatar_id,info.avatar_quality)
	self.Reco1Obj:SetActive(SocietyConfig.ShiTuSeekingType.seek_master == info.type)
	self.Reco2Obj:SetActive(SocietyConfig.ShiTuSeekingType.seek_apprentice == info.type)
end

function SocietyViewMasterApprenticeApplyItem:OnClickAgree()
	local info = self.data.info
	SocietyCtrl.Instance:SendShiTuMakeRelationAck(info.uid ,true, info.type)
end

function SocietyViewMasterApprenticeApplyItem:OnClickRefuse()
	local info = self.data.info
	SocietyCtrl.Instance:SendShiTuMakeRelationAck(info.uid ,false, info.type)
end