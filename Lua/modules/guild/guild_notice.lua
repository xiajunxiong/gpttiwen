GuildNotice = GuildNotice or DeclareView("GuildNotice", "guild/guild_notice")

function GuildNotice:GuildNotice()
    self.data = GuildData.Instance
end

function GuildNotice:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    self.Input.text = self.data:GetGuildInfo().guild_notice
end

function GuildNotice:OnClickCancel()
    ViewMgr:CloseView(GuildNotice)
end

function GuildNotice:OnClickOk()
    local block, notice_str = LoginData.Instance:BlockChangeName()
    if block then
        PublicPopupCtrl.Instance:Center(notice_str)
        return
    end

    local function callback(str)
        local param_t = {
            guild_id = self.data:GetGuildID(),
            notice = str
        }
        GuildCtrl.SetGuildNoticeReq(param_t)
        ViewMgr:CloseView(GuildNotice)
    end

    if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(self.Input.text, 5, callback)
	else
        if ChatFilter.Instance:IsIllegal(self.Input.text) then
            PublicPopupCtrl.Instance:Center(Language.Chat.ErrorCode.IllegalContent)
        else
		    callback(self.Input.text)
        end
	end
end