EnjoyRedoubleView = EnjoyRedoubleView or DeclareView("EnjoyRedoubleView","serveractivity/yun_ze/enjoy_redouble_view",Mod.YunZe.EnjoyRedouble)

function EnjoyRedoubleView:OnClickClose()
    ViewMgr:CloseView(EnjoyRedoubleView)
end
EnjoyRedoublePanel = EnjoyRedoublePanel or DeclareMono("EnjoyRedoublePanel", UIWFlushPanel)
function EnjoyRedoublePanel:EnjoyRedoublePanel()
    self.data = EnjoyRedoubleData.Instance
    self.data_cares = {
        {data = self.data.flush_task_data,func = self.FlushTask,init_call = true},
        {data = self.data.flush_box_data,func = self.FlushGiftBox,init_call = true},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(),func = self.SendBuy, init_call = false},
    }
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE,TimeType.Type_Time_8,Language.EnjoyRedouble.ActTime)
    self.data:SetCurShowBoxData(TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE))
end
function EnjoyRedoublePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end
function EnjoyRedoublePanel:FlushTask()
    self.TaskList:SetData(self.data:GetEveryDayReward(TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE)))
    for i = 1,self.ProgressList:Length() do
        UH.FillAmount(self.ProgressList[i],self.data:GetPhaseProgress(i))
    end
end
function EnjoyRedoublePanel:FlushGiftBox()
    self.BtnList:SetDataList(self.data:GetRewardInfo())
    self:FlushPanel()
end
function EnjoyRedoublePanel:PlayEffect()
    if not self.effect_free then
        self.effect_free = self.EffectTool:Play(6161017)
    end
end
function EnjoyRedoublePanel:FlushPanel()
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE)
    UH.SetText(self.TabTxt,string.format(Language.EnjoyRedouble.TabTxt,DataHelper.GetDaXie(self.data.cur_box_seq)))
    UH.SetText(self.TaskTxt,string.format(Language.EnjoyRedouble.TaskTxt,DataHelper.GetDaXie(open_day)))
    UH.SetText(self.BuyDesc,string.format(Language.EnjoyRedouble.BuyDesc,Config.fenxiangjiabei_auto.other[1].desc))
    self.RedPos:SetNum(self.data.today_free_flag > 0 and 0 or 1)
    if self.data.today_free_flag < 1 then
        TimeHelper:AddDelayTimer(BindTool.Bind(self.PlayEffect,self),0.1)
    else
        if self.effect_free then
            self.EffectTool:Stop(self.effect_free)
            self.effect_free = nil
        end
    end 
    self.GiftObj:SetActive(self.data.today_free_flag > 0)
    local reward_data = self.data:GetRewardInfo()[self.data.cur_box_seq]
    for i = 1,self.RewardList:Length() do
        if reward_data.reward_item[i - 1] then
            self.RewardList[i]:SetObjActive(true)
            self.RewardList[i]:SetData(reward_data.reward_item[i - 1])
            UH.LocalPos(self.ItemPosLst[i],EnjoyRedoubleConfig.ItemPos[#reward_data.reward_item + 1][i])
        else
            self.RewardList[i]:SetObjActive(false)
        end
    end
    self.is_activation = reward_data.is_activation
    local state = self.data.task_finish_num >= reward_data.task and reward_data.seq <= open_day
    self.Receive:SetActive(state and reward_data.is_fetch == false)
    self.Lock:SetActive(not state)
    self.BtnTxtObj:SetActive(reward_data.is_fetch or not state)
    local show_text = string.format(Language.EnjoyRedouble.ActOpen,DataHelper.GetDaXie(self.data.cur_box_seq))
    UH.LocalPos(self.BtnTxtPos,Vector3.New(0,-11,0))
    if state and reward_data.is_fetch then
        show_text = Language.EnjoyRedouble.Receive
        UH.LocalPos(self.BtnTxtPos,Vector3.New(0,0,0))
    end
    if not state and reward_data.seq <= TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE) then
        show_text = Language.EnjoyRedouble.UnReceive
    end
    UH.SetText(self.BtnTxt,show_text)
    self:FlushBtnBuy()
end
function EnjoyRedoublePanel:FlushBtnBuy()
    local other_data = Config.fenxiangjiabei_auto.other[1]
    UH.SetText(self.BuyTxt,string.format(Language.EnjoyRedouble.BuyType[other_data.price_type],other_data.price/10))
    if self.is_activation then
        UH.SetText(self.BuyTxt,Language.EnjoyRedouble.BuyType[4])
    end
end

function EnjoyRedoublePanel:OnClickFreeBtn()
    if self.data.today_free_flag > 0 then
        PublicPopupCtrl.Instance:Center(Language.EnjoyRedouble.FreeGiftTips)
        return
    end
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE,0)
end
function EnjoyRedoublePanel:OnClickBtnGift()
    if not self.is_activation then
        --PublicPopupCtrl.Instance:Error(Language.EnjoyRedouble.ActActivation)
        --提示确认框
        PublicPopupCtrl.Instance:DialogTips{
            content = string.format(Language.EnjoyRedouble.BuyTip,Config.fenxiangjiabei_auto.other[1].price/10,Config.fenxiangjiabei_auto.other[1].desc),
            confirm = {
                name = Language.MirageTerritory.Confirm,
                func = function()
                    self:OnBuyClick()
                    ViewMgr:CloseView(DialogTipsView)
                end
            },
        }
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE,2,self.data.cur_box_seq)
end

function EnjoyRedoublePanel:OnBuyClick()
    local other_data = Config.fenxiangjiabei_auto.other[1]
    local fee = Config.fenxiangjiabei_auto.other[1].price/10
    if other_data.buy_type == 1 then --1每日购买 2活动购买
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_ENJOY_REDOUBLE, fee, ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE,1,self.data.cur_box_seq)
        self.data.cur_price = other_data.price
    else
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_ENJOY_REDOUBLE, fee, ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE,1,0)--一次性购买
        self.data.cur_price = other_data.price
    end
end

function EnjoyRedoublePanel:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_ENJOY_REDOUBLE,self.data.cur_price)
end

----------------------------EnjoyRedoubleTaskItem----------------------------
EnjoyRedoubleTaskItem = EnjoyRedoubleTaskItem or DeclareMono("EnjoyRedoubleTaskItem", UIWidgetBaseItem)
function EnjoyRedoubleTaskItem:SetData(data)
    local state = (data.times >= data.parameter) and 2 or 1 --2已完成
    local show_str = string.format(Language.EnjoyRedouble.Times[1],data.describe,data.times > data.parameter and data.parameter or data.times,data.parameter)
    if #show_str > 33 then
        local num = 21
        show_str = "<size="..num..">"..show_str.."</size>"
    end
    UH.SetText(self.Desc,show_str)
    for i = 1,self.StateObjs:Length() do
        self.StateObjs[i]:SetActive(i == state)
    end
    UIWidgetBaseItem.SetData(self,data)
end

----------------------------EnjoyRedoubleGiftItem----------------------------
EnjoyRedoubleGiftItem = EnjoyRedoubleGiftItem or DeclareMono("EnjoyRedoubleGiftItem", UIWidgetBaseItem)
function EnjoyRedoubleGiftItem:SetData(data)
    data.item = Item.New(data)
    UH.SetIcon(self.IconSp, Item.GetIconId(data.item_id))
    UH.SetText(self.Num,string.format(Language.EnjoyRedouble.GiftNum,data.num))
    UIWidgetBaseItem.SetData(self,data)
end
function EnjoyRedoubleGiftItem:OnClickItem()
    CellClicks.BagGridNoButton({GetData = function()
        return self.data.item
    end})
end

----------------------------EnjoyRedoubleGiftItem----------------------------
EnjoyRedoubleBtnItem = EnjoyRedoubleBtnItem or DeclareMono("EnjoyRedoubleBtnItem", UIWidgetBaseItem)
function EnjoyRedoubleBtnItem:SetData(data)
    local task_num = EnjoyRedoubleData.Instance.task_finish_num
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE)
    UH.SetText(self.Times,string.format(Language.EnjoyRedouble.TaskTimes,task_num > data.task and data.task or task_num,data.task))
    local is_finish = data.is_fetch and data.is_activation
    local state = task_num >= data.task and 1 or 2 --1已达成
    --self.RedPos:SetNum((not is_finish and state == 1) and 1 or 0)
    if task_num >= data.task and data.is_fetch == false and data.seq <= open_day then
        --TimeHelper:AddDelayTimer(BindTool.Bind(self.PlayEffect,self),0.1)
        self:PlayEffect()
    else
        if self.effect_free then
            self.EffectTool:Stop(self.effect_free)
            self.effect_free = nil
        end
    end 
    self.BoxObj:SetActive(not (task_num >= data.task) or data.is_fetch or data.seq > open_day)
    for i = 1,self.StateObjs:Length() do
        self.StateObjs[i]:SetActive(i == state)
    end
    UIWidgetBaseItem.SetData(self,data)
end
function EnjoyRedoubleBtnItem:PlayEffect()
    if not self.effect_free then
        self.effect_free = self.EffectTool:Play(6161018)
    end
end
function EnjoyRedoubleBtnItem:OnClickItem()
    EnjoyRedoubleData.Instance:SetCurShowBoxData(self.data.seq)
    local reward_data = EnjoyRedoubleData.Instance:GetRewardInfo()[self.data.seq]
    if EnjoyRedoubleData.Instance.first_click and not reward_data.is_activation then
        --PublicPopupCtrl.Instance:Error(Language.EnjoyRedouble.ActActivation)
        --提示确认框
        PublicPopupCtrl.Instance:DialogTips{
            content = string.format(Language.EnjoyRedouble.BuyTip,Config.fenxiangjiabei_auto.other[1].price/10,Config.fenxiangjiabei_auto.other[1].desc),
            confirm = {
                name = Language.MirageTerritory.Confirm,
                func = function()
                    self:OnBuyClick()
                    ViewMgr:CloseView(DialogTipsView)
                end
            },
        }
        EnjoyRedoubleData.Instance.first_click = false
        return
    end
end

function EnjoyRedoubleBtnItem:OnBuyClick()
    local other_data = Config.fenxiangjiabei_auto.other[1]
    local fee = Config.fenxiangjiabei_auto.other[1].price/10
    if other_data.buy_type == 1 then --1每日购买 2活动购买
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_ENJOY_REDOUBLE, fee, ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE,1, EnjoyRedoubleData.Instance.cur_box_seq)
        EnjoyRedoubleData.Instance.cur_price = other_data.price
    else
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_ENJOY_REDOUBLE, fee, ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE,1,0)--一次性购买
        EnjoyRedoubleData.Instance.cur_price = other_data.price
    end
end