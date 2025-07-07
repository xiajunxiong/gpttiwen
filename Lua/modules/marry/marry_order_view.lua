
MarryOrderView = MarryOrderView or DeclareView("MarryOrderView", MarryConfig.ResPath .. "marry_order")
VIEW_DECLARE_LEVEL(MarryOrderView, ViewLevel.L1)

function MarryOrderView:LoadCallback()
end

function MarryOrderView:OnClickClose()
    ViewMgr:CloseView(MarryOrderView)
end

MarryOrderViewF = MarryOrderViewF or DeclareMono("MarryOrderViewF", UIWFlushPanel)

function MarryOrderViewF:MarryOrderViewF()
    self.data = MarryCtrl.Instance:Data()
    self.language = Language.Marry.MarryOrder

    self.data_cares = {
		{data = self.data.marry_order_info, func = self.FlushShow, init_call = false},
    }

    MarryCtrl.Instance:SendWeddingFBReqRegistList()
end

function MarryOrderViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushInfo()
end

function MarryOrderViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    MarryCtrl.Instance:SendWeddingFBReqSubRegistList()
end

function MarryOrderViewF:FlushInfo()
    local type_list = self.data:GetMarryOrderTypeList()
    local show_list = self.data:GetMarryOrderShowList()
    local reward_list = self.data:GetMarryOrderRewardList()
    local wedding_heat_reward_str_show = self.data:GetWeddingFBHeatRewardStrShow("、")
    local invite_num = self.data:ConfigOtherInviteNumber()

    self.TypeList:SetDataList(type_list)
    self.ShowList:SetDataList(show_list)
    self.RewardList:SetData(reward_list)
    self.TipsShow3.text = string.format(self.language.TipsShow3, wedding_heat_reward_str_show)
    self.NumShow.text = string.format(self.language.NumShow, invite_num)

    local role_info = self.data:GetRoleInfo()
    for i = 1 , self.ShowNames:Length() do
        UH.SetText(self.ShowNames[i], role_info[i] and role_info[i].role_name or "")
    end
end

function MarryOrderViewF:FlushShow()
    local info = self.data.marry_order_info
    self.BtnOrder.text = (info.init or 0 == info.regist_time) and self.language.BtnOrderShowConfirm or self.language.BtnOrderShowCancel
end

function MarryOrderViewF:OnClickOrder()
    local type = self.data.marry_order_sm.type
    local info = self.data.marry_order_sm.info
    if self.data:GetMarryOrderOutDate(info.open_time) then
        PublicPopupCtrl.Instance:Center(self.language.OutDateTips)
    else
        ViewMgr:OpenView(MarryOrderPopupView)
    end
end

function MarryOrderViewF:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[237].desc})
end


MarryOrderViewTypeItem = MarryOrderViewTypeItem or DeclareMono("MarryOrderViewTypeItem", UIWidgetBaseItem)
function MarryOrderViewTypeItem:MarryOrderViewTypeItem()
    self.marry_data = MarryCtrl.Instance:Data()
    self.language = Language.Marry.MarryOrder
end

function MarryOrderViewTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.TypeShow.text = string.format(self.language.TypeShow, data.month, data.day)
end

function MarryOrderViewTypeItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function MarryOrderViewTypeItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.marry_data.marry_order_sm.type:Set(self.data)
    end
end

MarryOrderViewShowItem = MarryOrderViewShowItem or DeclareMono("MarryOrderViewShowItem", UIWidgetBaseItem)
function MarryOrderViewShowItem:MarryOrderViewShowItem()
    self.marry_data = MarryCtrl.Instance:Data()
    self.language = Language.Marry.MarryOrder
end

function MarryOrderViewShowItem:OnDestroy()
    self:uncareData()
end

function MarryOrderViewShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self:uncareData()
    self.care_handles = {}
    self:careData(self.marry_data.marry_order_sm.type, BindTool.Bind(self.FlushShow, self))
    self:careData(self.marry_data.marry_order_info, BindTool.Bind(self.FlushShow, self))
end

function MarryOrderViewShowItem:FlushShow()
    local is_order = self.marry_data:GetMarryOrderIsRegist(self.data.open_time)
    local is_out_date = self.marry_data:GetMarryOrderOutDate(self.data.open_time)
    local color_show = (is_order or is_out_date) and COLORSTR.Green14 or COLORSTR.Red14
    self.TimeShow.text = RichTextHelper.ColorStr(string.format(self.language.TimeShow, self.data.open_time_show, self.data.end_time_show), color_show)
    self.StateShow.text = RichTextHelper.ColorStr(self.language.StateShow[is_order and 1 or (is_out_date and 3 or 2)], color_show)
    -- self.BlockOD:SetActive(is_out_date)
end

function MarryOrderViewShowItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function MarryOrderViewShowItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.marry_data.marry_order_sm.info:Set(self.data)
    end
end

function MarryOrderViewShowItem:OnClickBlockOD()
    PublicPopupCtrl.Instance:Center(self.language.OutDateTips)
end


MarryOrderPopupView = MarryOrderPopupView or DeclareView("MarryOrderPopupView", MarryConfig.ResPath .. "marry_order_popup")

function MarryOrderPopupView:MarryOrderPopupView()
    self.data = MarryCtrl.Instance:Data()
    self.language = Language.Marry.MarryOrder
end

function MarryOrderPopupView:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.PopupTitle)

    local info = self.data.marry_order_info
    local content_show = ""
    if info.init or 0 == info.regist_time then
        local type = self.data.marry_order_sm.type
        local info = self.data.marry_order_sm.info
        local ft = os.date("*t", type.start + info.open_time)
        local time_show = string.format(self.language.TimeFormat, ft.year, ft.month, ft.day, ft.hour, ft.min)
        content_show = string.format(self.language.PopupConfirm, time_show)
    else
        local ft = os.date("*t", info.regist_time)
        local time_show = string.format(self.language.TimeFormat, ft.year, ft.month, ft.day, ft.hour, ft.min)
        content_show = string.format(self.language.PopupCancel, time_show)
    end
    self.ContentShow.text = content_show
end

function MarryOrderPopupView:OnClickConfirm()
    local info = self.data.marry_order_info
    if info.init or 0 == info.regist_time then
        local type = self.data.marry_order_sm.type
        local info = self.data.marry_order_sm.info
        MarryCtrl.Instance:SendWeddingFBReqRegist(type.start + info.open_time)
    else
        MarryCtrl.Instance:SendWeddingFBReqRegistOff()
    end
    ViewMgr:CloseView(MarryOrderPopupView)
end