TigerGiftMoneyView = TigerGiftMoneyView or DeclareView("TigerGiftMoneyView", "serveractivity/tiger_year/tiger_gift_money", Mod.TigerYear.TigerGiftMoney)
VIEW_DECLARE_LEVEL(TigerGiftMoneyView, ViewLevel.L0)
function TigerGiftMoneyView:TigerGiftMoneyView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_TIGER_GIFT_MONEY, TimeType.Type_Time_8,Language.TigerGiftMoney.ActTime)
end

function TigerGiftMoneyView:OnClickClose()
    ViewMgr:CloseView(TigerGiftMoneyView)
end

--------------------TigerGiftMoneyPanel--------------------
TigerGiftMoneyPanel = TigerGiftMoneyPanel or DeclareMono("TigerGiftMoneyPanel", UIWFlushPanel)
function TigerGiftMoneyPanel:TigerGiftMoneyPanel()
    self.Data = TigerGiftMoneyData.Instance
    self.data = self.Data.gift_money_data
    self.data_cares = {
        {data = self.data, func = self.FlushShow,init_call = false},
        {data = self.data, func = self.FlushEffect,keys = {"is_click_receive"},init_call = false},
        {data = self.data, func = self.FlushList,keys = {"task_list"},init_call = true},
    }   
    self.is_click = false
end

function TigerGiftMoneyPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Data.is_close_anim = false
    self:FlushShow()
end

function TigerGiftMoneyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self:FlushEffect()
end
function TigerGiftMoneyPanel:FlushList()
    self.TaskList:SetData(self.data.task_list)
end

function TigerGiftMoneyPanel:FlushShow()
    local reward_add_info = self.Data:GetRewardInfo()
    self.BtnInter.Interactable = self.data.is_receive
    self.BtnEffect:SetActive(self.data.is_receive)
    UH.SetText(self.MoneyTxt,self.data.money_num)
    UH.SetText(self.TaskAddTxt,string.format(Language.TigerGiftMoney.TaskAdd,Item.GetName(reward_add_info.item_id_1),reward_add_info.add_num))
    self.RewardItem:SetData(Item.New(reward_add_info.item))
    if self.Data:GetOpenTime() then
        self.Time:StampTime(self.Data:GetOpenTime(), TimeType.Type_Special_4,Language.TigerGiftMoney.OpenTime)        
    else
        self.TimeObj:SetActive(false)
    end   
    if self.Data.is_close_anim then
        self.HongBaoAnim:SetTrigger(APH("Close"))  
    end
    local str = Language.TigerGiftMoney.BtnStatus[1]
    if self.data.money_num == 0 then
        str = Language.TigerGiftMoney.BtnStatus[2]
        if not self.hong_bao_cancel then
            self.hong_bao_cancel = TimeHelper:AddDelayTimer(BindTool.Bind(self.PlayEffect,self),0.4)
        end
    else
        self:FlushEffect()
    end
    UH.SetText(self.BtnTxt,str)
    self.is_click = false
    self:FlushHongBaoShow()
end

function TigerGiftMoneyPanel:FlushEffect()
    if self.hong_bao_effect then
        TimeHelper:CancelTimer(self.hong_bao_effect)
        self.hong_bao_effect = nil
    end
    if self.hong_bao_cancel then
        TimeHelper:CancelTimer(self.hong_bao_cancel)
        self.hong_bao_cancel = nil
    end
    self:StopEffect()
end

function TigerGiftMoneyPanel:FlushHongBaoShow()
    local state = self.data.money_num == 0 and self.data.is_receive == false
    if self.Data:GetOpenTime() then
        state = false
    end
    self.HongBaoClose:SetActive(not state)
    self.HongBaoOpen:SetActive(state)
end

function TigerGiftMoneyPanel:PlayOpenEffect()
    self.effect1 = self.EffectTool:Play(6161034)
    self.hong_bao_cancel = TimeHelper:AddDelayTimer(BindTool.Bind(self.PlayEffect,self),0.4)
end

function TigerGiftMoneyPanel:PlayEffect()
    if not self.effect2 then
        self.effect2 = self.EffectTool:Play(6161035)
    end
end

function TigerGiftMoneyPanel:StopEffect()
    if self.effect1 then
        self.EffectTool:Stop(self.effect1)
        self.effect1 = nil
    end
    if self.effect2 then
        self.EffectTool:Stop(self.effect2)
        self.effect2 = nil
    end
end

function TigerGiftMoneyPanel:OnReceiveClick()
    if not self.is_click then
        self.is_click = true
        self:FlushEffect()
        TigerGiftMoneyCtrl.Instance:Send(2)
        self.HongBaoAnim:SetTrigger(APH("Open"))
        self.HongBaoClose:SetActive(true)
        self.HongBaoOpen:SetActive(true)
        self.hong_bao_effect = TimeHelper:AddDelayTimer(BindTool.Bind(self.PlayOpenEffect,self),0.6)
    end
end

-------------------TigerGiftMoneyItem-------------------
TigerGiftMoneyItem = TigerGiftMoneyItem or DeclareMono("TigerGiftMoneyItem", UIWidgetBaseItem)

function TigerGiftMoneyItem:SetData(data) 
    UH.SetText(self.DescTxt,string.format(Language.TigerGiftMoney.TaskDesc,data.task_desc,data.times,data.parameter_1))
    UH.SetText(self.BtnTxt,Language.TigerGiftMoney.BtnStatus[data.task_status + data.reward_status])
    self.BtnInter.Interactable = data.reward_status == 0
    self.BtnObj:SetActive(data.task_status == 1 and true or data.jump_mod_key > 0)
    if self.effect1 then
        self.EffectTool:Stop(self.effect1)
        self.EffectTool:Stop(self.effect2)
        self.effect1 = nil
        self.effect2 = nil
    end
    if data.task_status == 1 and data.reward_status == 0 then
        self.effect1 = self.EffectTool:Play(6161031)
        self.effect2 = self.EffectTool:Play(6161032)
    end

    self.List:SetData(DataHelper.FormatItemList(data.reward_item))
    UIWidgetBaseItem.SetData(self,data)
end

function TigerGiftMoneyItem:OnItemClick()
    if self.data.task_status == 0 then
        if ViewMgr:IsOpen(TigerYearTotalView) then
            ViewMgr:CloseView(TigerYearTotalView)
        end     
        ViewMgr:OpenViewByKey(self.data.jump_mod_key)
        ViewMgr:CloseView(TigerGiftMoneyView)
        TigerYearTotalCtrl.Instance:AutoCloseTotal(Mod.TigerYear.TigerGiftMoney)
    else
        TigerGiftMoneyData.Instance:OnClickReceive()
        TigerGiftMoneyCtrl.Instance:Send(1,self.data.seq)
    end
end