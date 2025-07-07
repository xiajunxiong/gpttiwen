SwornEditDeclView = SwornEditDeclView or DeclareView("SwornEditDeclView", SwornConfig.ResPath .. "sworn_edit_decl")
function SwornEditDeclView:LoadCallback(param_t)
	self.decl_type = param_t.decl_type --1个人，2团伙
end

function SwornEditDeclView:OnFlush()
	local info = RoleData.Instance:GetBaseData()
	UH.SetText(self.Name, info.name)

	local prof_sp = RoleData.GetProfSp(info.profession)
	UH.SpriteName(self.ProfI, prof_sp)
	self.ProfI.gameObject:SetActive(nil ~= prof_sp)

	local avatar_data = {
		avatar_type = RoleData.Instance:GetRoleAvatarType(),
		avatar_id = RoleData.Instance:GetRoleAvatarId(),
		avatar_quality = RoleData.Instance:GetRoleAvatarQuality(),
		level = info.level,
		-- top_level = info.top_level
	}
	self.SwornAvatar:SetData(avatar_data)

	if nil ~= info.role_id then
		UH.SetText(self.Uid, "ID:" .. info.role_id)
	end
	local show_str = ""
	if self:IsSelf() then
		show_str = SwornData.Instance:GetSelfDecl()
	else
		show_str = SwornData.Instance:GetTeamDecl()
	end
	if not StringIsEmpty(show_str) then
		UH.SetText(self.InputField, show_str)
	end
end

function SwornEditDeclView:OnClickClose()
	ViewMgr:CloseView(SwornEditDeclView)
end

function SwornEditDeclView:OnClickSend()
	local block, notice_str = LoginData.Instance:BlockChangeName()
    if block then
        PublicPopupCtrl.Instance:Center(notice_str)
        return
    end

	local function callback(str)
		local str_num = DataHelper.GetStringWordNum(str)
		if StringIsEmpty(str) then
			PublicPopupCtrl.Instance:Center(Language.Society.CommonTips.DeclWordMin)
			return
		elseif str_num > SwornConfig.DECL_WORD_MAX then
			PublicPopupCtrl.Instance:Center(string.format(Language.Society.CommonTips.DeclWordMax, SwornConfig.DECL_WORD_MAX))
			return
		end
		SwornCtrl.Instance:SendSeekingInsert(self:IsSelf() and 0 or 1, 0, str)
		ViewMgr:CloseView(SwornEditDeclView)
		PublicPopupCtrl.Instance:Center(Language.Sworn.SwornDoSucc)
	end

	if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(self.InputField.text, 5, callback)
	else
		callback(ChatFilter.Instance:Filter(self.InputField.text))
	end
end

function SwornEditDeclView:OnClickDelete()
	SwornCtrl.Instance:SendSeekingInsert(self:IsSelf() and 0 or 1, 1, "")
	ViewMgr:CloseView(SwornEditDeclView)
	PublicPopupCtrl.Instance:Center(Language.Sworn.SwornDoSucc)
end

function SwornEditDeclView:IsSelf()
	return self.decl_type == 1
end
