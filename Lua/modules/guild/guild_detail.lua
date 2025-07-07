GuildDetail = GuildDetail or DeclareView("GuildDetail", "guild/guild_detail")
VIEW_DECLARE_MASK(GuildDetail,ViewMask.Block)
function GuildDetail:GuildDetail()
    self.lang = Language.Guild
end
function GuildDetail:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Guild.DetailTitle,Vector2.New(376, 420))
end
function GuildDetail:OnFlush()
    local info = GuildData.Instance:GetGuildInfo()
    UH.SetText(self.LblId, string.format(self.lang.BaseInfo[2], info.guild_id))
    UH.SetText(self.LblName, string.format(self.lang.BaseInfo[3], info.patriarch_name))
    UH.SetText(self.LblCreate, string.format(self.lang.BaseInfo[5], os.date("%Y-%m-%d", info.create_time)))
    UH.SetText(self.LblJoin, string.format(self.lang.BaseInfo[6], os.date("%Y-%m-%d", info.join_time)))
    self.btn_flag = 0
    local btn_str = self.lang.Accuse
    if GuildData.Instance:CanAccuseGuildLeader() then
    	if GuildData.Instance.guild_info.is_join_tan_he_patriarch == 1 then
    		self.btn_flag = -2
    		btn_str = self.lang.Accused
    	end
    else
    	self.btn_flag = -1
    end
    self.Interactor.Interactable = self.btn_flag == 0
    UH.SetText(self.BtnText, btn_str)
end

function GuildDetail:OnClickAccuse()
	if self.btn_flag == -1 then
		PublicPopupCtrl.Instance:Center(self.lang.AccuseTips)
		return
	elseif self.btn_flag == -2 then
		PublicPopupCtrl.Instance:Center(self.lang.Accused)
		return
	end
	GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.TAN_HE_ZU_ZHANG})
	ViewMgr:CloseView(GuildDetail)
end