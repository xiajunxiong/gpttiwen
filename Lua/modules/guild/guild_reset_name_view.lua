GuildResetNameView = GuildResetNameView or DeclareView("GuildResetNameView","guild/guild_reset_name_view")
VIEW_DECLARE_MASK(GuildResetNameView,ViewMask.Block)
----NewName=========Text

function GuildResetNameView:GuildResetNameView()
    
end

function GuildResetNameView:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    self.name_count = ROLE_NAME_COUNT -- 允许名字的长度
    self.Title.text = " " 
    self.Tips.text = Language.ResetName.Tips3
    self.Placeholder.text = string.format(Language.ResetName.PlaceholderTips,DataHelper.GetDaXie(ROLE_NAME_COUNT))
end

function GuildResetNameView:ResetFun()
    local param_t = {new_name = self.NewName.text}
    GuildCtrl.SendGuildReNameReq(param_t)
    -- PublicPopupCtrl.Instance:Center(ErrorText[113])
end

function GuildResetNameView:Close()
    GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_BASE_INFO})
    ViewMgr:CloseView(GuildResetNameView)
end

function GuildResetNameView:Rename()
    local block, notice_str = LoginData.Instance:BlockChangeName()
    if block then
        PublicPopupCtrl.Instance:Center(notice_str)
        return
    end

    local function callback(str)
        local param_t = {new_name = str}
        GuildCtrl.SendGuildReNameReq(param_t)
        self:Close()
    end

    if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(self.NewName.text, 1, callback)
	else
        local name = self.NewName.text
        local str_count = DataHelper.GetStringWordNum(name)
        if str_count > self.name_count then
            PublicPopupCtrl.Instance:Center(string.format(ErrorText[36],self.name_count))
            return
        end
        if StringIsEmpty(name) then
            PublicPopupCtrl.Instance:Center(ErrorText[50])
            return
        end

        if ChatFilter.Instance:IsIllegal(name, true) then
            PublicPopupCtrl.Instance:Center(ErrorText[66])
            return
        end
        callback(name)
    end    
end