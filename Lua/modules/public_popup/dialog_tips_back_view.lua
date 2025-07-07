DialogTipsBackView = DialogTipsBackView or DeclareView("DialogTipsBackView", "public_popup/dialog_tips_back",nil)

function DialogTipsBackView:DialogTipsBackView()
end

function DialogTipsBackView:LoadCallback(param_t)
    self.param_t = param_t
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end

function DialogTipsBackView:CloseCallback()
end

function DialogTipsBackView:OnFlush()
    local param_t = self.param_t
    param_t.cost_type = param_t.cost_type == nil and 0 or param_t.cost_type
    --默认铜币
    self.Content.text = string.format(Language.PublicPopup.BackContent, 
        COLORSTR.Yellow6,param_t.cost, 
        RichTextHelper.SpriteStrAtlas(FindHuoBi[param_t.cost_type], 40, 40), 
        COLORSTR.Yellow6,param_t.name)
    self.RewardList:SetData(param_t.rewards or {})

    self.ConfirmName.text = Language.PublicPopup.ReplyType.confirm
    self.CancelName.text = Language.PublicPopup.ReplyType.cancel
end

function DialogTipsBackView:OnClickCancel()
    ViewMgr:CloseView(DialogTipsBackView)
end

function DialogTipsBackView:OnClickConfirm()
    Call(self.param_t.confirm_func)
    ViewMgr:CloseView(DialogTipsBackView)
    ViewMgr:CloseView(NpcDialogView)
end

ActDialogTipsBackView = ActDialogTipsBackView or DeclareView("ActDialogTipsBackView", "public_popup/act_dialog_tips_back")

function ActDialogTipsBackView:LoadCallback(param_t)
    self.param_t = param_t
    UH.SetText(self.Content,param_t.content)
    UH.SetText(self.CancelName,param_t.cancel_name)
    UH.SetText(self.ConfirmName,param_t.confirm_name)
    self.RewardList:SetData(param_t.reward_list)
end

function ActDialogTipsBackView:OnClickCancel()
    ViewMgr:CloseView(ActDialogTipsBackView)
    self.param_t.cancel_func()
end

function ActDialogTipsBackView:OnClickConfirm()
    ViewMgr:CloseView(ActDialogTipsBackView)
    self.param_t.confirm_func()
end

function ActDialogTipsBackView:OnClickClose()
    ViewMgr:CloseView(ActDialogTipsBackView)
end