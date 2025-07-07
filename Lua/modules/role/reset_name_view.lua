ResetNameView = ResetNameView or DeclareView("ResetNameView","reset_name_view")

VIEW_DECLARE_MASK(ResetNameView,ViewMask.Block)

function ResetNameView:LoadCallback(param_t)
    self.count = param_t.count or ROLE_NAME_COUNT
    self.reset_func = param_t.reset_func
    UH.SetText(self.Title,param_t.title or "")
    UH.SetText(self.Tips,param_t.tips or "")
    UH.SetText(self.Placeholder,Format(Language.ResetName.PlaceholderTips,
    DataHelper.GetDaXie(self.count)))
end

function ResetNameView:OnClickRename()
    local block, notice_str = LoginData.Instance:BlockChangeName()
    if block then
        PublicPopupCtrl.Instance:Center(notice_str)
        return
    end
    
    local function callback(str)
        if self.reset_func then
            self.reset_func(str)
        end
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
        ViewMgr:CloseView(ResetNameView)
    end

    if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(self.NewName.text, 1, callback)
	else
        local name = self.NewName.text
        if name == "" then
            PublicPopupCtrl.Instance:Center(ErrorText[1])
            return
        end
        
        local str_count = DataHelper.GetStringWordNum(name)
        if str_count > self.count then
            PublicPopupCtrl.Instance:Center(Format(ErrorText[36],self.count))
            return
        end

        if ChatFilter.Instance:IsIllegal(self.NewName.text, true) then
            PublicPopupCtrl.Instance:Center(ErrorText[66])
            return
        end
        callback(name)
    end
end

function ResetNameView:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(ResetNameView)
end