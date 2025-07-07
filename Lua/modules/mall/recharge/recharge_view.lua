RechargeView = RechargeView or DeclareMono("RechargeView", UIWFlushPanel)

function RechargeView:RechargeView()
    self.data = RechargeData.Instance
    self.data_cares = {
        {data = self.data.recharge_data, func = self.FlushRechargeList, init_call = false}
    }
end

function RechargeView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if IS_STANDALONE() then
        self.ScrollRect.scrollSensitivity = 10
        self.PCNoticeObj:SetActive(QQBigPlayerCtrl.IsQQPC() == true)
    end
    self:FlushRechargeList()
end

function RechargeView:FlushRechargeList()
    self.List:SetData(self.data:GetRechargeList())
    for k,item in pairs(self.List.item_list) do
        item:FlushFirstView()
    end
end

----------------------------RechargeBoardItem----------------------------
RechargeBoardItem = RechargeBoardItem or DeclareMono("RechargeBoardItem", UIWidgetBaseItem)

function RechargeBoardItem:SetData(data)
    self.List:SetData(data)
end

----------------------------RechargeItem----------------------------
RechargeItem = RechargeItem or DeclareMono("RechargeItem", UIWidgetBaseItem)

function RechargeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local flush_effect = true
    if IS_AUDIT_VERSION then
        if not GLOBAL_CONFIG:PkgInfo().is_youyan then
            UH.SpriteName(self.Gold)
            UH.SpriteName(self.GoldAudit,[[gold_icon_]] .. Valve(data.index,6))
            self.AuditBg:SetActive(true)
            flush_effect = false
        else
            self.AuditBg:SetActive(false)
            local img_path, stream_has = ChannelAgentConsts.GetUIStremingAsset("Recharge"..Valve(data.index,6))
            if stream_has then
                self.GoldAuditRawObj:SetActive(true)
                ChannelAgentConsts.SetRawImg(self.GoldAuditRaw,img_path)
                UH.SpriteName(self.Gold)
                flush_effect = false
            else
                UH.SpriteName(self.Gold,[[gold_icon_]] .. Valve(data.index,6))
            end

            local bg_img_path, bg_stream_has = ChannelAgentConsts.GetUIStremingAsset("RechargeBg")
            if bg_stream_has then
                self.AuditBgRawObj:SetActive(true)
                ChannelAgentConsts.SetRawImg(self.AuditBgRaw,bg_img_path)
            end
        end
    else
        UH.SpriteName(self.Gold,[[gold_icon_]] .. Valve(data.index,6))
        UH.SpriteName(self.GoldAudit)
        self.AuditBg:SetActive(false)
    end
    local money_num = RechargeData.Instance:MoneyToGold(data.money)
    
    UH.SetText(self.CNum,data.money)
    UH.SetText(self.Num,money_num)
    self:FlushFirstView()
    if flush_effect then
        self.EffectTimer = Invoke(function()
            self:FlushEffectView()
        end,0.5)
    end
end

function RechargeItem:FlushFirstView()
    local extra_reward = RechargeData.Instance:GetExtraReward(self.data.index)
    UH.SetText(self.FirstNum,extra_reward)
    self.First:SetActive(extra_reward ~= 0)
end

function RechargeItem:FlushEffectView()
    if self.Effect.isActiveAndEnabled then
        local index = Valve(self.data.index,6)
        if self.effect_handler then
            self.Effect:Stop(self.effect_handler)
        end
        self.effect_handler = self.Effect:Play(tostring(10010238 + index))
    end
end

function RechargeItem:OnClickRechargeItem()
    -- 检查是否支持万能卡
    local fee = self.data.money
    if ServerActivityCtrl.Instance:IsCanItemBuy(BUY_ITEM_TYPE.TYPE_RECHARGE, fee) then
        local gold_num = RechargeData.Instance:MoneyToGold(self.data.money)
        ServerActivityCtrl.Instance:SendBuyCmdReq(BUY_ITEM_TYPE.TYPE_RECHARGE, fee, gold_num)
        return
    end

    if IS_AUDIT_VERSION then
        RoleCtrl.Instance:SendAuditBehaviourSubmitReq(0)
    end
    RechargeCtrl.Instance:Recharge(self.data.money)
end

function RechargeItem:OnDestroy()
    TimeHelper:CancelTimer(self.EffectTimer)
end

--首充提醒监听
--================================MainViewRechargeView================================
MainViewRechargeView = MainViewRechargeView or DeclareMono("MainViewRechargeView", UIWFlushPanel)

function MainViewRechargeView:MainViewRechargeView()
    self.data_cares = {
        {data = TaskData.Instance.task_id_list,func = self.OnFlush,keys = {"finish_list"}}
    }
end

function MainViewRechargeView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_delay)
end

function MainViewRechargeView:OnFlush()
    local is_open = FunOpen.Instance:GetFunIsOpened(Mod.RechargeTip.Main)
    local pre_value = UnityPlayerPrefs.GetInt(PrefKeys.RechargeTip())
    if pre_value == 0 and is_open then
        self:SetActive(true)
        TimeHelper:CancelTimer(self.handle_delay)
        self.handle_delay = TimeHelper:AddDelayTimer(function ()
            self:SetActive(false)
        end, 15)
        UnityPlayerPrefs.SetInt(PrefKeys.RechargeTip(), 1)
    end
end

function MainViewRechargeView:OnClick()
    TimeHelper:CancelTimer(self.handle_delay)
    self:SetActive(false)
    ViewMgr:OpenViewByKey(Mod.RandActivity.FirstCharge)
end


function MainViewRechargeView:SetActive(is_active)
    self.Tip:SetActive(is_active)
    self.Txt:SetActive(not is_active)
end
--首充动画逻辑
--================================MainViewRechargePanel================================
MainViewRechargePanel = MainViewRechargePanel or DeclareMono("MainViewRechargePanel", UIWFlushPanel)

function MainViewRechargePanel:MainViewRechargePanel()
    self.IconAnimator:ListenEvent("play_first_charge",BindTool.Bind(self.OnAnima,self))
end

function MainViewRechargePanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    TimeHelper:CancelTimer(self.handle_time3)
    TimeHelper:CancelTimer(self.handle_time30)
    self.play_count = 0
    self:Delay3()
end

function MainViewRechargePanel:OnAnima()
    self.play_count = self.play_count + 1
    if self.play_count == 4 then
        self.play_count = 0
        self.IconAnimator:SetBool("is_play", false)
    end
end

function MainViewRechargePanel:Delay3()
    TimeHelper:CancelTimer(self.handle_time3)
    self.handle_time3 = TimeHelper:AddDelayTimer(function ()
        self.IconAnimator:SetBool("is_play", true)
        self:Delay30()
    end, 3)
end

function MainViewRechargePanel:Delay30()
    TimeHelper:CancelTimer(self.handle_time30)
    self.handle_time30 = TimeHelper:AddDelayTimer(function ()
        self:Delay3()
    end, 30)
end

function MainViewRechargePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_time3)
    TimeHelper:CancelTimer(self.handle_time30)
end