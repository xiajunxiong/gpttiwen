MarryIntroduceView = MarryIntroduceView or DeclareView("MarryIntroduceView", "marry/marry_introduce")

function MarryIntroduceView:MarryIntroduceView()
    self.Board:SetData(self:GetType(), Language.Marry.TitleNameList[4], Vector2.New(996, 630),true)
end

MarryRelieveView = MarryRelieveView or DeclareView("MarryRelieveView", "marry/marry_relieve")

function MarryRelieveView:MarryRelieveView()
    self.Board:SetData(self:GetType(), Language.Marry.TitleNameList[5], Vector2.New(915, 551))
end

function MarryRelieveView:LoadCallback(param_t)
    UH.SetText(self.Desc,Language.GetHelpDesc(241))
    self.param_t = param_t
end

function MarryRelieveView:OnClickComfirm()
    MarryCtrl.Instance:OnMarriageRelieve(self.param_t.index)
    ViewMgr:CloseView(MarryRelieveView)
end

MarryRelieveSelectView = MarryRelieveSelectView or DeclareView("MarryRelieveSelectView", "marry/marry_relieve_select")

function MarryRelieveSelectView:MarryRelieveSelectView()
    self.Board:SetData(self:GetType(), Language.Marry.TitleNameList[6], Vector2.New(442, 470))
end

function MarryRelieveSelectView:LoadCallback(param_t)
    self.List:SetData(Language.Marry.RelieveList)
end

function MarryRelieveSelectView:OnClickItem(data)
    ViewMgr:OpenView(MarryRelieveView,{index = data.index})
    ViewMgr:CloseView(MarryRelieveSelectView)
end
----------------------------MarryRelieveSelectItem----------------------------
MarryRelieveSelectItem = MarryRelieveSelectItem or DeclareMono("MarryRelieveSelectItem", UIWidgetBaseItem)

function MarryRelieveSelectItem:SetData(data)
    UH.SetText(self.Title,data.title)
    UH.SetText(self.Desc,Language.GetHelpDesc(242 + data.index))
    UIWidgetBaseItem.SetData(self, data)
end

--=========================MarryRelieveComfirmView=========================
MarryRelieveComfirmView = MarryRelieveComfirmView or DeclareView("MarryRelieveComfirmView", "marry/marry_relieve_comfirm")

function MarryRelieveComfirmView:MarryRelieveComfirmView()
    self.Board:SetData(self:GetType(), Language.Marry.PopupViewTitle)
    self.data = MarryData.Instance
end

function MarryRelieveComfirmView:LoadCallback(param_t)
    UH.SetText(self.Desc,Language.GetHelpDesc(245))
    UH.SetText(self.Content,Format(Language.Marry.ErrorText[9],self.data:GetHusbandOrWifeName()))
    UH.SetText(self.CancelName,Language.Marry.BtnName[9])
    UH.SetText(self.ConfirmName,Language.Marry.BtnName[10])
    self.param_t = param_t
end

function MarryRelieveComfirmView:OnClickCancel()
    ViewMgr:CloseView(MarryRelieveComfirmView)
end

function MarryRelieveComfirmView:OnClickConfirm()
    ViewMgr:CloseView(MarryRelieveComfirmView)
    if self.param_t.confirm_func then
        self.param_t.confirm_func()
    end
end

--=========================MarryQuitView=========================
MarryQuitView = MarryQuitView or DeclareView("MarryQuitView", "marry/marry_quit")

function MarryQuitView:MarryQuitView()
    self.Alert:SetData(self:GetType(), Vector2.New(466, 317))
    self.Time:SetShowCallBack(function(t)return Format(Language.Marry.MarryQuitTip, t.hour, t.min, t.s)end)
    self.Time:SetCallBack(function()ViewMgr:CloseView(SwornQuitView)end)
end

function MarryQuitView:LoadCallback(param_t)
    self.Time:StampTime(param_t.time_stamp)
end

function MarryQuitView:OnClick()
    MarryCtrl.Instance:SendMarryReq(MarryConfig.REQUEST_TYPE.CANCEL_FORCE_DIVORCE)
    ViewMgr:CloseView(MarryQuitView)
end