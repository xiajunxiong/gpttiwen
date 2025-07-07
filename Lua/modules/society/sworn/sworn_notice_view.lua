SwornNoticeView = SwornNoticeView or DeclareView("SwornNoticeView", SwornConfig.ResPath .. "sworn_notice")
function SwornNoticeView:SwornNoticeView()
end

function SwornNoticeView:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    self.Input.text = SwornData.Instance:GetSwornDecl()
end

function SwornNoticeView:OnClickCancel()
    ViewMgr:CloseView(SwornNoticeView)
end

function SwornNoticeView:OnClickOk()
    local block, notice_str = LoginData.Instance:BlockChangeName()
    if block then
        PublicPopupCtrl.Instance:Center(notice_str)
        return
    end
    
    local function callback(str)
        SwornCtrl.Instance:SendChangeSwornDeclReq(str)
        ViewMgr:CloseView(SwornNoticeView)
        PublicPopupCtrl.Instance:Center(Language.Sworn.SwornDoSucc)
    end

    if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(self.Input.text, 5, callback)
    else
        local str = self.Input.text
        if ChatFilter.Instance:IsIllegal(str) then
            PublicPopupCtrl.Instance:Center(Language.Chat.ErrorCode.IllegalContent)
        elseif DataHelper.GetStringWordNum(str) > SwornConfig.SWORN_DECL_WORD_MAX then
            PublicPopupCtrl.Instance:Center(string.format(Language.Sworn.DeclWordMax, SwornConfig.SWORN_DECL_WORD_MAX))
            return
        else
            callback(str)
        end
    end
end
