
SocietyViewMasterApprenticeSearch = SocietyViewMasterApprenticeSearch or DeclareView("SocietyViewMasterApprenticeSearch", SocietyConfig.ResPath .. "society_master_apprentice_search")

function SocietyViewMasterApprenticeSearch:SocietyViewMasterApprenticeSearch()
	self.data = SocietyCtrl.Instance:Data()
end

function SocietyViewMasterApprenticeSearch:OnFlush()
	-- local info = RoleData.Instance:GetBaseData()
	-- local setting = ItemInfoConfig.ProfSetting
	-- self.Name.text = info.name
	-- self.Level.text = info.level
	-- self.ProfI.SpriteName = setting[info.profession]
	-- self.ProfI.gameObject:SetActive(nil ~= setting[info.profession])
	-- if nil ~= info.role_id then self.Uid.text = "ID:" .. info.role_id end
	-- if not StringIsEmpty(self.data.ma_seeking_notice) then self.InputField.text = self.data.ma_seeking_notice end
end

function SocietyViewMasterApprenticeSearch:OnClickClose()
	ViewMgr:CloseView(SocietyViewMasterApprenticeSearch)
end

function SocietyViewMasterApprenticeSearch:OnClickSend()
	-- local str = self.InputField.text
	-- local str_num = DataHelper.GetStringWordNum(str)
	-- if StringIsEmpty(str) then
	-- 	PublicPopupCtrl.Instance:Center(Language.Society.CommonTips.DeclWordMin)
	-- 	return
	-- elseif str_num > SocietyConfig.DECL_WORD_MAX then
    --     PublicPopupCtrl.Instance:Center(string.format(Language.Society.CommonTips.DeclWordMax, SocietyConfig.DECL_WORD_MAX))
    --     return
	-- end
	-- SocietyCtrl.Instance:SendShiTuSeekingInsert(str)
	-- ViewMgr:CloseView(SocietyViewMasterApprenticeDecl)
end

function SocietyViewMasterApprenticeSearch:OnClickCancel()
	-- SocietyCtrl.Instance:SendShiTuSeekingRemove()
	-- ViewMgr:CloseView(SocietyViewMasterApprenticeDecl)
end
