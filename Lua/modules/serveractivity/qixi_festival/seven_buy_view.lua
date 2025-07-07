SevenBuyView = SevenBuyView or DeclareView("SevenBuyView", "serveractivity/qixi_festival/seven_buy", Mod.QixiFestival.SevenBuy)
VIEW_DECLARE_LEVEL(SevenBuyView, ViewLevel.L1)
VIEW_DECLARE_MASK(SevenBuyView, ViewMask.BgBlock)
function SevenBuyView:SevenBuyView()
    self.Time:SetShowCallBack(function(t)
        return Format(Language.SevenBuy.Time, t.day, t.hour ,t.min)
    end)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SEVEN_BUY, TimeType.Type_Time_3)
end

function SevenBuyView:OnCloseClick()
    ViewMgr:CloseView(SevenBuyView)
end

function SevenBuyView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[341].desc)
end

----------------------------SevenBuyPanel----------------------------
SevenBuyPanel = SevenBuyPanel or DeclareMono("SevenBuyPanel", UIWFlushPanel)
function SevenBuyPanel:SevenBuyPanel()
    self.Data = SevenBuyData.Instance
    self.language = Language.SevenBuy
    self.data = self.Data.info_data
    self.now_page = 1
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function SevenBuyPanel:Awake()
    UIWFlushPanel.Awake(self)
    if self.data.info.is_open_flag == 0 then
        SevenBuyCtrl.Instance:SendReq(SevenBuyReqType.FirstOpen)
    end
    self.phase_data = self.Data:PhaseCfg()
    self.PhaseList:SetData(self.phase_data)
    self:FlushAll()
end

function SevenBuyPanel:FlushAll()
    self.now_buy_data = nil
    self.base_data, self.max_page = self.Data:BaseCfgData()
    local phase_total_count = self.phase_data[1].total_buy_times
    self.Progress:SetProgress(self.data.info.activity_buy_count / phase_total_count)
    UH.SetText(self.TxtBuyTimes, self.data.info.activity_buy_count)
    self:OnPageClick(0)
    for k, v in pairs(self.PhaseList.item_list) do
        v:FlushShow()
    end
end

function SevenBuyPanel:OnPageClick(value)
    self.now_page = self.now_page + value
    if self.now_page > self.max_page then
        self.now_page = self.max_page
    end
    if self.now_page < 1 then
        self.now_page = 1
    end
    self.ObjLeft:SetActive(self.now_page ~= 1)
    self.ObjRight:SetActive(self.now_page ~= self.max_page)
    self.List:SetData(self.base_data[self.now_page])
end

function SevenBuyPanel:OnBuyClick(data)
    if not TableIsEmpty(self.now_buy_data) then
        return
    end
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_SEVEN_BUY)
    if open_day < data.activity_days then
        return
    end
    if self.Data:IsBuyAll(data) then
        PublicPopupCtrl.Instance:Center(self.language.NotTimesTip)
        return
    end
    self.now_buy_data = data
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_SEVEN_BUY)
    if open_day >= data.activity_days then
        local fee = data.price / 10
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_SEVEN_BUY, fee, ACTIVITY_TYPE.RAND_SEVEN_BUY, SevenBuyReqType.Buy, data.seq)
    end
end

function SevenBuyPanel:SendBuy()
    if TableIsEmpty(self.now_buy_data) and ViewMgr:IsOpen(SevenBuyRewardView) then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_SEVEN_BUY, self.now_buy_data.price)
    self.now_buy_data = nil
end

function SevenBuyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

--------------SevenBuyItem--------------
SevenBuyItem = SevenBuyItem or DeclareMono("SevenBuyItem", UIWidgetBaseItem)
function SevenBuyItem:SevenBuyItem()
    self.Data = SevenBuyData.Instance
    self.language = Language.SevenBuy
end

function SevenBuyItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtDay, Format(self.language.Day, DataHelper.GetDaXie(data.activity_days)))

    local activity_status = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_SEVEN_BUY)
    local open_stamp = TimeCtrl.Instance:ChangDataTime(activity_status.param_1, data.seq)
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_SEVEN_BUY)
    
    self.Time:CloseCountDownTime()
    if open_day >= data.activity_days then
        -- 证明开了
        UH.SetText(self.TxtName, data.gift_name)
        local show_now_times = data.limit_times - self.Data:LimitTimes(data)
        local color = show_now_times > 0 and COLORSTR.Green9 or COLORSTR.Red7
        UH.SetText(self.TxtLimit, Format(self.language.LimitType[data.buy_limit], color, show_now_times, data.limit_times))
        UH.SetText(self.TxtPrice, data.price / 10)
        self.ObjBlock:SetActive(false)
        self.ObjEffect:SetActive(show_now_times > 0)
        self.IHandler.Interactable = show_now_times > 0
    else
        -- 没开需要倒计时
        self.Time:SetCallBack(function()
            self.Data.info_data:Notify()
        end)
        self.Time:StampTime(open_stamp, TimeType.Type_Time_3, self.language.Unlock)
        UH.SetText(self.TxtName, self.language.NotOpenName)
        UH.SetText(self.TxtLimit, Format(self.language.LimitType[data.buy_limit], COLORSTR.Red7, "?", "?"))
        UH.SetText(self.TxtPrice, "???")
        self.ObjEffect:SetActive(false)
        self.ObjBlock:SetActive(true)
        self.IHandler.Interactable = true
    end
    self.ObjBuyAll:SetActive(self.Data:IsBuyAll(data))
end

function SevenBuyItem:OnBoxClick()
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_SEVEN_BUY)
    if open_day >= self.data.activity_days then
        ViewMgr:OpenView(SevenBuyRewardView, {activity_days = self.data.activity_days, data = self.data})
    end
end

--------------SevenBuyDownItem--------------
SevenBuyDownItem = SevenBuyDownItem or DeclareMono("SevenBuyDownItem", UIWidgetBaseItem)
function SevenBuyDownItem:SevenBuyDownItem()
    self.Data = SevenBuyData.Instance
    self.language = Language.SevenBuy
end

function SevenBuyDownItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item)
    UH.SpriteName(self.ImgFrame, "YuanPinZhiKuang" .. item:Color())
    UH.SetIcon(self.Icon, item:IconId())
    UH.LocalPosG(self.ObjNode, Vector3.New(953 / data.total_buy_times * data.buy_times, 0, 0))
    if self.data.buy_times == self.data.total_buy_times then
        UH.LocalScaleG(self.ObjNode, Vector3.New(1, 1, 1))
        UH.LocalPosG(self.ObjRemind, Vector3.New(45, 45, 0))
        UH.LocalPosG(self.ObjTimes, Vector3.New(0, -60, 0))
    else
        UH.LocalScaleG(self.ObjNode, Vector3.New(0.8, 0.8, 1))
        UH.LocalPosG(self.ObjRemind, Vector3.New(38, 38, 0))
        UH.LocalPosG(self.ObjTimes, Vector3.New(0, -51, 0))
    end
    self:FlushShow()
end

function SevenBuyDownItem:FlushShow()
    self.Remind:SetNum(self.Data:IsCanGetPhaseReward(self.data) and 1 or 0)
    self.ObjEffect:SetActive(self.Data:IsCanGetPhaseReward(self.data))
    UH.SetText(self.TxtTimes, Format(self.language.BuyTimes, self.data.buy_times))
    self.ObjReceived:SetActive(self.Data:PhaseRewardFlag(self.data.seq) == 1)
end

function SevenBuyDownItem:OnReceiveClick()
    if self.Data:IsCanGetPhaseReward(self.data) then
        SevenBuyCtrl.Instance:SendReq(SevenBuyReqType.PhaseReward, self.data.seq)
    else
        local info = {
            item_list = {Item.Create(self.data.reward_item)}
        }
        ItemInfoCtrl.Instance:ItemInfoView(info)
    end
end

--------------SevenBuyRewardView--------------
SevenBuyRewardView = SevenBuyRewardView or DeclareView("SevenBuyRewardView", "serveractivity/qixi_festival/seven_buy_reward")
VIEW_DECLARE_MASK(SevenBuyRewardView, ViewMask.BgBlock)
function SevenBuyRewardView:SevenBuyRewardView()
    self.Data = SevenBuyData.Instance
    self.language = Language.SevenBuy
    self.now_page = 1
    self.ht = self.Data.info_data:Care(BindTool.Bind(self.FlushBtn, self))
    self.ht2 = ServerActivityData.Instance:GetCmdBuyItemCheckRet():Care(BindTool.Bind(self.SendBuy, self))
end

function SevenBuyRewardView:LoadCallback(param_t)
    self.data = param_t.data
    UH.SetText(self.TxtDesc, self.Data:Desc(self.data.seq))
    self.base_data, self.max_page = self.Data:RewardShowCfgData(param_t.activity_days)
    self:OnPageClick(0)
    self:FlushBtn()
end

function SevenBuyRewardView:CloseCallback()
    self.Data.info_data:Uncare(self.ht)
    ServerActivityData.Instance:GetCmdBuyItemCheckRet():Uncare(self.ht2)
end

function SevenBuyRewardView:OnPageClick(value)
    self.now_page = self.now_page + value
    if self.now_page > self.max_page then
        self.now_page = self.max_page
    end
    if self.now_page < 1 then
        self.now_page = 1
    end
    self.ObjLeft:SetActive(self.now_page ~= 1)
    self.ObjRight:SetActive(self.now_page ~= self.max_page)
    self.List:SetData(self.base_data[self.now_page])
end

function SevenBuyRewardView:OnCloseClick()
    ViewMgr:CloseView(SevenBuyRewardView)
end

function SevenBuyRewardView:FlushBtn()
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_SEVEN_BUY)
    if open_day >= self.data.activity_days then
        -- 证明开了
        local show_now_times = self.data.limit_times - self.Data:LimitTimes(self.data)
        self.ObjEffect:SetActive(show_now_times > 0)
        self.IHandler.Interactable = show_now_times > 0
        UH.SetText(self.TxtPrice, self.data.price / 10)
    else
        self.ObjEffect:SetActive(false)
        self.IHandler.Interactable = true
        UH.SetText(self.TxtPrice, "???")
    end
end

function SevenBuyRewardView:OnBuyClick(data)
    if not TableIsEmpty(self.now_buy_data) then
        return
    end
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_SEVEN_BUY)
    if open_day < self.data.activity_days then
        return
    end
    if self.Data:IsBuyAll(self.data) then
        PublicPopupCtrl.Instance:Center(self.language.NotTimesTip)
        return
    end
    self.now_buy_data = self.data
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_SEVEN_BUY)
    if open_day >= self.data.activity_days then
        local fee = self.data.price / 10
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_SEVEN_BUY, fee, ACTIVITY_TYPE.RAND_SEVEN_BUY, SevenBuyReqType.Buy, self.data.seq)
    end
end

function SevenBuyRewardView:SendBuy()
    if TableIsEmpty(self.now_buy_data) then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_SEVEN_BUY, self.now_buy_data.price)
    self.now_buy_data = nil
end

--------------SevenBuyRewardItem--------------
SevenBuyRewardItem = SevenBuyRewardItem or DeclareMono("SevenBuyRewardItem", UIWidgetBaseItem)
function SevenBuyRewardItem:SevenBuyRewardItem()
    self.Data = SevenBuyData.Instance
    self.language = Language.SevenBuy
end

function SevenBuyRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(Item.Create(data.show_item))
    UH.SetText(self.TxtRate, Format(self.language.Rate, data.show_rate))
end