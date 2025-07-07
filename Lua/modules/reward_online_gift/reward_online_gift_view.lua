RewardOnlineGiftView = RewardOnlineGiftView or DeclareView("RewardOnlineGiftView", "reward_online_gift/reward_online_gift_view",Mod.RewardOnlineGift.Main)
function RewardOnlineGiftView:RewardOnlineGiftView()

end

function RewardOnlineGiftView:LoadCallback(param_t)

end

function RewardOnlineGiftView:OnClickClose()
    ViewMgr:CloseView(RewardOnlineGiftView)
end

RewardOnlineGiftPanel= RewardOnlineGiftPanel or DeclareMono("RewardOnlineGiftPanel", UIWFlushPanel)
function RewardOnlineGiftPanel:RewardOnlineGiftPanel()
    self.data = RewardOnlineGiftData.Instance
    self.ctrl = RewardOnlineGiftCtrl.Instance
    self.data_cares = {
        {data = self.data.base_info, func = self.FlushRewardView},
    }    
end 

function RewardOnlineGiftPanel:FlushRewardView()
    local cell_list = self.data:GetCellList()
    self.RewardList:SetData(cell_list)
end

function RewardOnlineGiftPanel:OnClickReward(data)
    self.ctrl:SendReqForReward(data.cfg)
end

RewardOnlineGiftCell = RewardOnlineGiftCell or DeclareMono("RewardOnlineGiftCell", UIWidgetBaseItem)
function RewardOnlineGiftCell:RewardOnlineGiftCell()
    self.Running:SetCallBack(function()self:TimerComplete() end)
end
function RewardOnlineGiftCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self:FlushShow()
    local item = Item.Create(data.cfg.online_reward[0])
    self.Call:SetData(item)
    UH.SetText(self.Name, item:Name())
    local t_f = TimeHelper.FormatDHMS(data.cfg.time)

    local time_str = (t_f.hour == 0 and "" or t_f.hour..Language.RewardOnlineGift.Time.H) .. 
    (t_f.min == 0 and "" or t_f.min..Language.RewardOnlineGift.Time.M) ..
    (t_f.s == 0 and "" or t_f.s..Language.RewardOnlineGift.Time.S)

    -- UH.SetText(self.Wait,time_str..Language.RewardOnlineGift.WaitDesc)
    UH.SetText(self.Wait,data.cfg.time_des)--time_str..Language.RewardOnlineGift.WaitDesc)
end

function RewardOnlineGiftCell:TimerComplete()
    if RewardOnlineGiftData.Instance:GetIsLastRunDoneTimer(self.data.cfg) then 
        self.Panel:FlushRewardView()
    end 
end

function RewardOnlineGiftCell:FlushShow()
    local param = RewardOnlineGiftData.Instance:CheckStatus(self.data.cfg)
    if  param.play_time  > 0 then
        self.Running:TotalTime(param.play_time,TimeType.Type_Special_11,Language.RewardOnlineGift.WaitDescS)
    end 

    -- 正在进行时 
    self.RunningCan.alpha = (param.is_running) and 1 or 0
    -- 非现在进行时 + 未完成
    self.Wait.gameObject:SetActive(not param.is_running and not param.now_complete)
    -- 已完成 + 还没领取 
    self.CanGet:SetActive(param.now_complete and not param.item_catch)
    self.ClickBlock:SetActive(param.now_complete and not param.item_catch)
    self.EffPos:SetActive(param.now_complete and not param.item_catch)
    -- 拿了 
    self.Mask:SetActive(param.item_catch)
    self.Got:SetActive(param.item_catch)
end

function RewardOnlineGiftCell:OnRelease()
end

RewardOnlineGiftSub = RewardOnlineGiftSub or DeclareMono("RewardOnlineGiftSub",UIWFlushPanel)
function RewardOnlineGiftSub:RewardOnlineGiftSub()
    self.data_cares = {
		{data = RoleData.Instance:GetBaseData(), func = self.FlushPanel, keys = {"level"}},
		{data = RewardOnlineGiftData.Instance.check_info, func = self.FlushPanel, init_call = false},
    }
end

function RewardOnlineGiftSub:FlushPanel()
    if RewardOnlineGiftData.Instance:GetRunningGiftTimer() > 0 then 
        self.Status:TotalTime(RewardOnlineGiftData.Instance:GetRunningGiftTimer())
        self.RedPoint:SetActive(false)
    else
        self.Status:CloseCountDownTime()
        UH.SetText(self.Status.LblTime,Language.RewardOnlineGift.WithFor)
        self.RedPoint:SetActive(true)
    end 

    if RoleData.Instance:CreatedDay() > 1 then 
        UH.LocalPos(self.Rect,Vector3.New(0,-80,0))
    else 
        UH.LocalPos(self.Rect,Vector3.New(0,0,0))
    end 
end

function RewardOnlineGiftSub:OnClick()
    ViewMgr:OpenView(RewardOnlineGiftView)
end