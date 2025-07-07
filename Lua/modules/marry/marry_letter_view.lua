MarryLetterView = MarryLetterView or DeclareView("MarryLetterView", "marry/marry_letter")

function MarryLetterView:MarryLetterView()
    self.data = MarryData.Instance
    self.Currency:DefaultSet()
end

function MarryLetterView:OnClickClose()
    ViewMgr:CloseView(MarryLetterView)
end

function MarryLetterView:LoadCallback(param_t)
    for i=1,self.Wedding:Length() do
        self.Wedding[i]:SetData(i)
    end
    UH.SetText(self.Desc,Language.GetHelpDesc(240))
    self.info = param_t.info
end

function MarryLetterView:OnClickEnter()
    if not self.select_type then
        PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[3])
        return
    end
    --判断亲密度
    local limit_intimacy = Config.get_married_auto.other[1].marry_min_intimacy
    if SocietyData.Instance:GetFriendIntimacyVal(self.info.uid) < limit_intimacy then
        PublicPopupCtrl.Instance:Center(Format(Language.Love.LoveTntimacyTip,limit_intimacy))
        return
    end	
    local config = Config.get_married_auto.marry_base[self.select_type]
    if config and MallCtrl.IsNotCurrency(config.money_type,config.price) then
        MarryCtrl.Instance:SendMarryReq(MarryConfig.REQUEST_TYPE.INVITE,self.info.uid,config.marry_type)
        -- PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[24])
        ViewMgr:CloseView(MarryLetterView)
    end
end

function MarryLetterView:OnClickToggle(type)
    self.select_type = type
end