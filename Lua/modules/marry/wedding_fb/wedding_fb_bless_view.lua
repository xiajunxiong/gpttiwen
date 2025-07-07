
WeddingFBBlessView = WeddingFBBlessView or DeclareView("WeddingFBBlessView", MarryConfig.ResPath .. "wedding_fb_bless")
VIEW_DECLARE_LEVEL(WeddingFBBlessView, ViewLevel.L1)

function WeddingFBBlessView:WeddingFBBlessView()
    self.language = Language.Marry
end

function WeddingFBBlessView:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.WeddingFBBlessTitle, Vector2.New(820, 590), true)
end

WeddingFBBlessViewF = WeddingFBBlessViewF or DeclareMono("WeddingFBBlessViewF", UIWFlushPanel)

function WeddingFBBlessViewF:WeddingFBBlessViewF()
    self.data = MarryCtrl.Instance:Data()
    self.language = Language.Marry.WeddingFBBless

    self.BlockToggle.isOn = 1 == UnityPlayerPrefs.GetInt(PrefKeys.WeddingFBBlessEffect())
end

function WeddingFBBlessViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushShow()
end

function WeddingFBBlessViewF:FlushShow()
    local show_list = self.data:GetWeddingFBBlessShowList()
    self.ShowList:SetDataList(show_list)
end

function WeddingFBBlessViewF:OnClickSend()
    local info = self.data.wedding_bless_sm
    if MallCtrl.IsNotCurrency(info.money_type, info.price) then
        ViewMgr:OpenView(MarryPopupView, {
            title_name = self.language.TipsTitle,
            content = string.format(self.language.TipsShow, info.price .. FormatEnum.CurrencyType[info.money_type], info.item:QuaName()),
            confirm_func = function ()
                MarryCtrl.Instance:SendWeddingFBReqBless(info.shop_item_index)
            end
        })
    end
end

function WeddingFBBlessViewF:OnTogBlock()
    UnityPlayerPrefs.SetInt(PrefKeys.WeddingFBBlessEffect(), self.BlockToggle.isOn and 1 or 0)
end

WeddingFBBlessViewItem = WeddingFBBlessViewItem or DeclareMono("WeddingFBBlessViewItem", UIWidgetBaseItem)
function WeddingFBBlessViewItem:WeddingFBBlessViewItem()
    self.marry_data = MarryCtrl.Instance:Data()
end

function WeddingFBBlessViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data.item)
    self.Price.text = data.price
    self.IconSp.SpriteName = MarryConfig.BlessMoneyType[data.money_type]
end

function WeddingFBBlessViewItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function WeddingFBBlessViewItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.marry_data.wedding_bless_sm:Set(self.data)
    end
end