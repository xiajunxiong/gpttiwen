
MoZuZaiNaLiView = MoZuZaiNaLiView or DeclareView("MoZuZaiNaLiView","serveractivity/mozuzainali", Mod.YunZe.MoZuZaiNaLi)
VIEW_DECLARE_LEVEL(MoZuZaiNaLiView, ViewLevel.L1)
function MoZuZaiNaLiView:MoZuZaiNaLiView()
end

function MoZuZaiNaLiView:OnClickClose()
    ViewMgr:CloseView(MoZuZaiNaLiView)
end

MoZuZaiNaLiViewF = MoZuZaiNaLiViewF or DeclareMono("MoZuZaiNaLiViewF", UIWFlushPanel)

function MoZuZaiNaLiViewF:MoZuZaiNaLiViewF()
    self.data = MoZuZaiNaLiData.Instance
    self.act_data = ActivityData.Instance
    self.language = Language.MoZuZaiNaLi

    self.data_cares = {
        {data = self.act_data.activity_status_event, func = self.FlushState},
        {data = self.data.info_sm, func = self.FlushInfo, keys = {"day_times"}},
    }
end

function MoZuZaiNaLiViewF:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushRemainTime()
    self:FlushShow()

    MoZuZaiNaLiData.Instance.login_flag = true
    MoZuZaiNaLiData.Instance.info_sm:Notify()
end

function MoZuZaiNaLiViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_ct)
end

function MoZuZaiNaLiViewF:FlushState()
    if self.act_data.activity_status_event.act_type ~= ACTIVITY_TYPE.RAND_MOZUZAINALI then return end
    local activity_status = self.act_data:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_MOZUZAINALI)
    local status = activity_status and activity_status.status or ActStatusType.Close
    if ActStatusType.Open == status then
        self:FlushRemainTime()
    else
        ViewMgr:CloseView(MoZuZaiNaLiView)
    end
end

function MoZuZaiNaLiViewF:FlushRemainTime()
    TimeHelper:CancelTimer(self.timer_ct)
    local activity_status = self.act_data:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_MOZUZAINALI)
    local end_time = activity_status and activity_status.param_2 or 0
    if end_time - TimeHelper.GetServerTime() <= 0 then
        self.RemainTime.text = string.format(self.language.RemainTime, 0, 0)
        return
    end
    self.timer_ct = TimeHelper:AddCountDownCT(function ()
        local last_time = end_time - TimeHelper.GetServerTime()
        local ft = TimeHelper.FormatDHMS(last_time)
        self.RemainTime.text = string.format(self.language.RemainTime, ft.day, ft.hour)
    end, function ()
        self.RemainTime.text = string.format(self.language.RemainTime, 0, 0)
    end, end_time, 1)
end

function MoZuZaiNaLiViewF:FlushShow()
    self.ShowDesc2.text = self.language.ShowDesc2
    local rewards = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MOZUZAINALI, self.data.config_reward)
    self.Rewards:SetData(rewards[1] and DataHelper.FormatItemList(rewards[1].reward_item) or {})
end

function MoZuZaiNaLiViewF:FlushInfo()
    local day_times = self.data.info_sm.day_times
    local total_times = self.data:BasicsFrequency()
    local not_finish = day_times < total_times
    self.DayTimes.text = string.format(self.language.DayTimes, day_times, total_times)
    self.NotFinishObj:SetActive(not_finish)
    self.FinishObj:SetActive(not not_finish)
    if not not_finish then
        local activity_status = self.act_data:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_MOZUZAINALI)
        local end_time = activity_status and activity_status.param_2 or 0
        local last_time = end_time - TimeHelper.GetServerTime()
        local ft = TimeHelper.FormatDHMS(last_time)
        local last_day = ft.day
        self.FinishShow.text = last_day > 0 and self.language.FinishShow.finish_today or self.language.FinishShow.finish_all
    end
end

function MoZuZaiNaLiViewF:OnClickGo()
    -- if LoginData.Instance:IsOnCrossSever() then
    --     PublicPopupCtrl.Instance:Center(self.language.OnCrossTips)
    --     return
    -- end
    if LoginData.Instance:IsOnCrossSever() then
        local param_t = {
        content = Language.Common.CrossTips,
        confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                    LoginCtrl:SendBackToOriginServer()
                end
            },
        }
        PublicPopupCtrl.Instance:DialogTips(param_t)
        return
    end
    SceneLogic.Instance:AutoToPos(self.data:BasicsSceneId(),nil,nil,nil,true)
    ViewMgr:CloseView(MoZuZaiNaLiView)
    PublicPopupCtrl.Instance:Center(self.language.GoTips)
    if ViewMgr:IsOpen(YunZeView) then
        ViewMgr:CloseView(YunZeView)
    end
end