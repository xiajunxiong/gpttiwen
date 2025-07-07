SuperValueEnjoyView = SuperValueEnjoyView or DeclareView("SuperValueEnjoyView", "serveractivity/yun_ze/super_value_enjoy", Mod.YunZe.SuperValueEnjoy)
VIEW_DECLARE_LEVEL(SuperValueEnjoyView, ViewLevel.L1)
VIEW_DECLARE_MASK(SuperValueEnjoyView, ViewMask.BgBlock)

function SuperValueEnjoyView:SuperValueEnjoyView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SUPER_VALUE_ENJOY, TimeType.Type_Time_8)
end

function SuperValueEnjoyView:OnCloseClick()
    ViewMgr:CloseView(SuperValueEnjoyView)
end

-- 通用3000协议发送枚举
SuperValueEnjoyReqType = {
    Info = 0,           --请求下发
    ReceiveTask = 1,    --领取任务奖励，p1：奖励seq
    BuyType1 = 2,       --购买加购资格，非直购
    BuyType2 = 3,       --购买加购资格，直购
    ReceiveTicket = 4,  --领取加购奖励
}

--------------SuperValueEnjoyPanel--------------
SuperValueEnjoyPanel = SuperValueEnjoyPanel or DeclareMono("SuperValueEnjoyPanel", UIWFlushPanel)
function SuperValueEnjoyPanel:SuperValueEnjoyPanel()
    self.Data = YunZeData.Instance
    self.data = self.Data.super_value_enjoy_data
    self.other = Config.chaozhifenxiang_auto.other[1]
    self.language = Language.SuperValueEnjoy
    self.reward_list = self.Data:GetSuperValueEnjoyRewardConfig()
    self.task_list = self.Data:GetSuperValueEnjoyTaskConfig()
    self.max_day = self.Data:GetSuperValueEnjoyMaxDay()
    self.now_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_SUPER_VALUE_ENJOY)
    self.data_cares = {
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
        {data = self.data, func = self.FlushShow, init_call = false},
    }
end

function SuperValueEnjoyPanel:Awake()
    UIWFlushPanel.Awake(self)
    if self.now_day > self.max_day then
        LogError("超值纷享活动配置错误，当前活动开启天数不可能超过表配置的最大天数", self.now_day, self.max_day)
        ViewMgr:CloseView(SuperValueEnjoyView)
        return
    end
    UH.SetText(self.TxtDesc, self.other.desc)
    UH.SetGold(self.ImgSetBuy1,BUY_CURRENCY_TYPE[self.other.price_type] or self.other.price_type)
    local icon_id = Item.GetIconId(self.other.reward_item.item_id)
    UH.SetIcon(self.ImgSetBuy2, icon_id)
    UH.SetIcon(self.ImgSetBuy3, icon_id)
    UH.SetText(self.TxtBtn1, Format(self.language.BuyType[self.other.price_type], self.other.price_type == 3 and self.other.price / 10 or self.other.price))
    UH.SetText(self.TxtBtn2, Format(self.language.Ticket, self.other.reward_item.num))
    UH.SetText(self.TxtBtn3, Format(self.language.Ticket, self.other.reward_item.num))
    for i = 1, self.List:Length() do
        self.List[i]:SetData(self.reward_list[i])
    end
    self:FlushShow()
end

function SuperValueEnjoyPanel:FlushShow()
    self.TxtBtn1:SetObjActive(not self.data.is_buy)
    self.RedPoint:SetObjActive(self.data.is_buy)
    if self.data.is_buy then
        self.TxtBtn2:SetObjActive(not self.data.is_receive_ticket)
        -- 1是每日购买，2是活动购买
        if self.other.buy_type == 1 then
            self.TxtBtn3:SetObjActive(false)
            self.ObjBtn4:SetActive(self.data.is_receive_ticket)
        else
            self.TxtBtn3:SetObjActive(self.data.is_receive_ticket and self.now_day < self.max_day)
            self.ObjBtn4:SetActive(self.data.is_receive_ticket and self.now_day == self.max_day)
        end
        self.RedPoint:SetNum(self.data.is_receive_ticket and 0 or 1)
    else
        self.TxtBtn2:SetObjActive(false)
        self.TxtBtn3:SetObjActive(false)
        self.ObjBtn4:SetActive(false)
        self.RedPoint:SetNum(0)
    end
    for i = 1, self.List:Length() do
        self.List[i]:FlushShow()
    end
    if #self.task_list == self.data.finish_num then
        UH.SetText(self.TxtTip, self.now_day < self.max_day and self.language.AllFinish[1] or self.language.AllFinish[2])
    else
        local not_finish = self.Data:GetSuperValueEnjoyNotFinishTask()
        if not TableIsEmpty(not_finish) then
            local param = self.Data:GetSuperValueEnjoyTaskParam(not_finish.type)
            param = param > not_finish.parameter and not_finish.parameter or param
            UH.SetText(self.TxtTip, Format(self.language.FinishTip[3], not_finish.describe, param, not_finish.parameter))
        end
    end
    self:FlushTask()
end

function SuperValueEnjoyPanel:FlushTask()
    local str = "\n"
    for i, v in ipairs(self.task_list) do
        local param = self.Data:GetSuperValueEnjoyTaskParam(v.type)
        str = str .. Format(self.language.FinishTip[4], v.describe, param > v.parameter and v.parameter or param, v.parameter)
        str = str .. "\n"
    end
    UH.SetText(self.TxtTaskDesc, str)
end

function SuperValueEnjoyPanel:SendBuy()
    if self.other.price_type == 3 then
        ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_SUPER_VALUE_ENJOY, self.other.price)
    end
end

function SuperValueEnjoyPanel:OnBuyClick()
    if not self.data.is_buy then
        if self.other.price_type == 3 then
            local fee = self.other.price / 10
            ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_SUPER_VALUE_ENJOY, fee, ACTIVITY_TYPE.RAND_SUPER_VALUE_ENJOY, SuperValueEnjoyReqType.BuyType2)
        else
            if MallCtrl.IsNotCurrency(self.other.price_type, self.other.price) then
                ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SUPER_VALUE_ENJOY, SuperValueEnjoyReqType.BuyType1)
            end
        end
    elseif not self.data.is_receive_ticket then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SUPER_VALUE_ENJOY, SuperValueEnjoyReqType.ReceiveTicket)
    else
        if self.other.buy_type == 1 then
            PublicPopupCtrl.Instance:Center(self.language.Received)
        else
            PublicPopupCtrl.Instance:Center(self.now_day < self.max_day and self.language.TomorrowReceived or self.language.Received)
        end
    end
end

function SuperValueEnjoyPanel:SetAllTaskActive(value)
    self.ObjAllTask:SetActive(value)
end

function SuperValueEnjoyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

--------------SuperValueEnjoyItem--------------
SuperValueEnjoyItem = SuperValueEnjoyItem or DeclareMono("SuperValueEnjoyItem", UIWidgetBaseItem)
function SuperValueEnjoyItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(Item.Create(data.reward_item[0]))
end

function SuperValueEnjoyItem:FlushShow()
    local is_received = YunZeData.Instance:GetSuperValueEnjoyTaskFlag(self.data.seq) == 1
    local can_receive = (not is_received) and YunZeData.Instance.super_value_enjoy_data.finish_num >= self.data.task
    local finish_str = ""
    if self.data.task == 2 then
        finish_str = Language.SuperValueEnjoy.FinishTip[2]
    else
        finish_str = Format(Language.SuperValueEnjoy.FinishTip[1], DataHelper.GetDaXie(self.data.task))
    end
    if can_receive then
        UH.SetText(self.TxtDesc, Language.SuperValueEnjoy.CanReceived)
    else
        UH.SetText(self.TxtDesc, is_received and Language.SuperValueEnjoy.Received or finish_str)
    end
    self.RedPoint:SetNum((YunZeData.Instance.super_value_enjoy_data.is_buy and can_receive) and 1 or 0)
    if YunZeData.Instance.super_value_enjoy_data.is_buy then
        self.ObjReceive:SetActive(not is_received)
    else
        self.ObjReceive:SetActive(false)
    end
end

function SuperValueEnjoyItem:OnReceiveClick()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    local is_received = YunZeData.Instance:GetSuperValueEnjoyTaskFlag(self.data.seq) == 1
    local can_receive = (not is_received) and YunZeData.Instance.super_value_enjoy_data.finish_num >= self.data.task
    if YunZeData.Instance.super_value_enjoy_data.is_buy then
        if can_receive then
            ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SUPER_VALUE_ENJOY, SuperValueEnjoyReqType.ReceiveTask, self.data.seq)
        else
            PublicPopupCtrl.Instance:Center(Language.SuperValueEnjoy.FinishNotEnought)
        end
    else
        PublicPopupCtrl.Instance:Center(Language.SuperValueEnjoy.NotBuyTip)
    end
end