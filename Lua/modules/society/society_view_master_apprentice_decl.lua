
SocietyViewMasterApprenticeDecl = SocietyViewMasterApprenticeDecl or DeclareView("SocietyViewMasterApprenticeDecl", SocietyConfig.ResPath .. "society_master_apprentice_decl")

function SocietyViewMasterApprenticeDecl:SocietyViewMasterApprenticeDecl()
	self.data = SocietyCtrl.Instance:Data()
end

function SocietyViewMasterApprenticeDecl:OnFlush()
	local info = RoleData.Instance:GetBaseData()
	local prof_sp = RoleData.GetProfSp(info.profession)
	self.Name.text = info.name
	self.Level.text = info.level
	self.ProfI.SpriteName = prof_sp
	self.ProfI.gameObject:SetActive(nil ~= prof_sp)
    UH.SetAvatar(self.IconSp, RoleData.Instance:GetRoleAvatarType(),RoleData.Instance:GetRoleAvatarId(),RoleData.Instance:GetRoleAvatarQuality())
	if nil ~= info.role_id then self.Uid.text = "ID:" .. info.role_id end
	if not StringIsEmpty(self.data.ma_seeking_notice) then self.InputField.text = self.data.ma_seeking_notice end
end

function SocietyViewMasterApprenticeDecl:OnClickClose()
	ViewMgr:CloseView(SocietyViewMasterApprenticeDecl)
end

function SocietyViewMasterApprenticeDecl:OnClickSend()
	if LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:Center(StringIsEmpty(self.data.ma_seeking_notice) and Language.Society.MasterApprentice.cross_tips_error1 or Language.Society.MasterApprentice.cross_tips_error2)
        return
    end

	local function callback(str)
		local str_num = DataHelper.GetStringWordNum(str)
		if StringIsEmpty(str) then
			PublicPopupCtrl.Instance:Center(Language.Society.CommonTips.DeclWordMin)
			return
		elseif str_num > SocietyConfig.DECL_WORD_MAX then
			PublicPopupCtrl.Instance:Center(string.format(Language.Society.CommonTips.DeclWordMax, SocietyConfig.DECL_WORD_MAX))
			return
		end
		SocietyCtrl.Instance:SendShiTuSeekingInsert(str, 1 - SocietyViewDeclarationDetail.seeking_type)
		ViewMgr:CloseView(SocietyViewMasterApprenticeDecl)
	end

	if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(self.InputField.text, 2, callback)
	else
		callback(ChatFilter.Instance:Filter(self.InputField.text))
	end
end

function SocietyViewMasterApprenticeDecl:OnClickDelete()
	SocietyCtrl.Instance:SendShiTuSeekingRemove(1 - SocietyViewDeclarationDetail.seeking_type)
	ViewMgr:CloseView(SocietyViewMasterApprenticeDecl)
end
