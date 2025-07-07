
MarryCarView = MarryCarView or DeclareView("MarryCarView", MarryConfig.ResPath .. "marry_car")
VIEW_DECLARE_LEVEL(MarryCarView, ViewLevel.L1)

function MarryCarView:MarryCarView()
    self.language = Language.Marry
end

function MarryCarView:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.MarryCarTitle, Vector2.New(915, 551))
end

MarryCarViewF = MarryCarViewF or DeclareMono("MarryCarViewF", UIWFlushPanel)

function MarryCarViewF:MarryCarViewF()
    self.data = MarryCtrl.Instance:Data()
    self.language = Language.Marry.MarryCar
end

function MarryCarViewF:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushShow()
end

function MarryCarViewF:FlushShow()
    local show_list = self.data:GetMarryCarShowList()
    for i = 1, self.ShowItems:Length() do
        self.ShowItems[i]:SetData(show_list[i])
    end
    self.ShowItems[1]:SetSelected(true)
    self.ShowItems[1]:OnSelect(true)
    self.TipsShow.text = self.language.TipsShow
end

function MarryCarViewF:OnClickConfirm()
    local info = self.data.marry_car_sm
    if MallCtrl.IsNotCurrency(info.money_type, info.price) then
        ViewMgr:OpenView(MarryCarPopupView)
    end
end

function MarryCarViewF:OnClickCancel()
    ViewMgr:CloseView(MarryCarView)
end

MarryCarViewShowItem = MarryCarViewShowItem or DeclareMono("MarryCarViewShowItem", UIWidgetBaseItem)
function MarryCarViewShowItem:MarryCarViewShowItem()
    self.marry_data = MarryCtrl.Instance:Data()
end

function MarryCarViewShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.PriceShow.text = data.price
    local path = DrawerHelper.GetRidePath(data.res_id)
    if not self.WeddingGame then
        self.WeddingGame = UIChDrawer.New()
        self.WeddingGame:SetMainSolo(path)
        self.WeddingShow:SetShowData({ui_obj = self.WeddingGame, view = self})
    else
        self.WeddingGame:SetMainSolo(path)
    end
end

function MarryCarViewShowItem:OnDestroy()
    Delete(self,[[WeddingGame]])
end

function MarryCarViewShowItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function MarryCarViewShowItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.marry_data.marry_car_sm:Set(self.data)
    end
end

MarryCarPopupView = MarryCarPopupView or DeclareView("MarryCarPopupView", MarryConfig.ResPath .. "marry_car_popup")

function MarryCarPopupView:MarryCarPopupView()
    self.data = MarryCtrl.Instance:Data()
    self.language = Language.Marry.MarryCar
end

function MarryCarPopupView:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.PopupTitle)

    local info = self.data.marry_car_sm
    self.ContentShow.text = string.format(self.language.BuyTips, info.type_name, info.price .. FormatEnum.CurrencyType[info.money_type])
end

function MarryCarPopupView:OnClickConfirm()
    local info = self.data.marry_car_sm
    MarryCtrl.Instance:SendWeddingFBReqXunYou(info.float_type)
    ViewMgr:CloseView(MarryCarPopupView)
    ViewMgr:CloseView(MarryCarView)
end

function MarryCarPopupView:OnClickCancel()
    ViewMgr:CloseView(MarryCarPopupView)
    ViewMgr:CloseView(MarryCarView)
end