GuildArms = GuildArms or DeclareView("GuildArms", "guild/guild_arms")
function GuildArms:GuildArms()
    self.lang = Language.Guild
    self.data = GuildData.Instance
end

function GuildArms:LoadCallback()
    self.Alert:SetData(self:GetType(), Vector2.New(458, 367))
end
function GuildArms:OnFlush()
    local info = self.data:GetGuildInfo()
    local cost = self.data.guild_other.change_badge_coin
    UH.SetText(self.CostTex, cost)
    UH.SpriteName(self.CostIcon, HuoBi[self.data.guild_other.badge_money_type])
    self.Arms.text = info.arms
end
function GuildArms:OnClickChange()
    local block, notice_str = LoginData.Instance:BlockChangeName()
    if block then
        PublicPopupCtrl.Instance:Center(notice_str)
        return
    end

    local function callback(str)
        GuildCtrl.GuildSetBanner(str)
        GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_BASE_INFO})
        ViewMgr:CloseView(GuildArms)
    end

    if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(self.Arms.text, 1, callback)
	else
        if ChatFilter.Instance:IsIllegal(self.Arms.text) then
            PublicPopupCtrl.Instance:Center(ErrorText[66])
            return
        end
        if StringIsEmpty(self.Arms.text) then
            PublicPopupCtrl.Instance:Center(ErrorText[149])
            return
        end
        if self.Arms.text == " " then
            PublicPopupCtrl.Instance:Center(ErrorText[190])
            return
        end
		callback(self.Arms.text)
	end
end