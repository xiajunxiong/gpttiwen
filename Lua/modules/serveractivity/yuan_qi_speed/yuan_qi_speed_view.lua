YuanQiSpeedView = YuanQiSpeedView or DeclareView("YuanQiSpeedView", "serveractivity/yuan_qi_speed/yuan_qi_speed_view", Mod.MayDay.YuanQiSpeed)
function YuanQiSpeedView:LoadCallback()
    self.MainPanel:OpenCall()
end

YuanQiSpeedPanel = YuanQiSpeedPanel or DeclareMono("YuanQiSpeedPanel", UIWFlushPanel)
function YuanQiSpeedPanel:YuanQiSpeedPanel()
    self.data = YuanQiSpeedData.Instance
    self.data_cares = {
        {data = YuanQiSpeedData.Instance.act_info, func = self.FlushView, init_call = false},
        {data = self.data.select_info, func = self.OnSelectChange, init_call = false}
    }
end

function YuanQiSpeedPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:ShowActTime()

    local pet_list = {[1] = 1, [2] = 2, [3] = 0}
    for i = 1, 3 do
        self.RabbitList[i]:SetData({seq = pet_list[i]})
        self.RabbitList[i]:OnClickRabbit(false)
    end
    self.ItemCanGetRedNum:SetNum(1)
end

function YuanQiSpeedPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.open_timer)
    self.open_timer = nil
end

function YuanQiSpeedPanel:OpenCall()
    self.RabbitList[2]:ToggleSelect()
    local function func_delay()
        for i = 1, 3 do
            self.SelectEffects[i]:SetActive(true)
            self.RabbitEffectList[i]:SetActive(true)
        end
    end
    self.open_timer = Invoke(func_delay, 0.5)
end

function YuanQiSpeedPanel:OnClickClose()
    for i = 1, 3 do
        self.SelectEffects[i]:SetActive(false)
        self.RabbitEffectList[i]:SetActive(false)
    end
    ViewMgr:CloseView(YuanQiSpeedView)
end

function YuanQiSpeedPanel:FlushView()
    local seq = self.data.select_info.seq
    if seq == -1 then
        return
    end
    local join_times = self.data:GetJoinTimes()
    local max_times = self.data:GetMaxTimes()
    UH.SetText(self.BtnPKTimes, Format(Language.YuanQiSpeed.RemainTimes, max_times - join_times, max_times))
    self.BtnPKRedPoint:SetNum(max_times - join_times)

    local yet_get = self.data:IsPassFetched(seq)
    local can_get = self.data:IsPassFlag(seq)
    self.ItemYetGet:SetActive(yet_get)
    self.PassReward:SetActive(not yet_get)
    self.ItemCanGet:SetActive(can_get)
    self.ItemCanGetEffect:SetActive(can_get)
    for i = 1, 3 do
        self.RabbitList[i]:FlushRedNum()
    end
end

function YuanQiSpeedPanel:ShowActTime()
    local open_time = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_YUAN_QI_SPEED)
    if open_time > 0 then
        local open_t = os.date("*t", open_time)
        local end_t = os.date("*t", ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_YUAN_QI_SPEED) - 1)
        UH.SetText(self.ActTime, Format(Language.AllSoulCast.ActTime, open_t.month, open_t.day, end_t.month, end_t.day))
    else
        UH.SetText(self.ActTime, Format(Language.AllSoulCast.ActTime, 1, 1, 1, 1))
    end
end

function YuanQiSpeedPanel:OnSelectChange()
    local seq = self.data.select_info.seq
    if seq == -1 then
        return
    end
    local pk_cfg = self.data:GetShowCfg(seq)
    self.WinReward:SetData(DataHelper.FormatItemList(pk_cfg.reward1_item))
    self.FailReward:SetData(DataHelper.FormatItemList(pk_cfg.reward2_item))
    self.ItemCell:SetData(Item.Create(pk_cfg.reward3_item))
    self:FlushView()

    local rabbit_cfg = self.data:GetRabbitCfg(seq)
    UH.SetText(self.BtnPKText, Format(Language.YuanQiSpeed.PKBtnName, rabbit_cfg.name))
end

function YuanQiSpeedPanel:OnClickPK()
    if self.data.select_info.seq == -1 then
        return
    end
    if self.data:GetJoinTimes() >= self.data:GetMaxTimes() then
        PublicPopupCtrl.Instance:Center(Language.YuanQiSpeed.PKError1)
        return
    end
    if TeamCtrl.Instance:Data():InTeam() then
        PublicPopupCtrl.Instance:AlertTip(Language.YuanQiSpeed.PopupTeamTip, function()
            TeamCtrl.Instance:SendExitTeam()
            ViewMgr:CloseView(YuanQiSpeedView)
            ViewMgr:OpenView(YuanQiSpeedGameView)
            ViewMgr:CloseView(MayDayView)
        end)
        return
    end
    ViewMgr:CloseView(YuanQiSpeedView)
    ViewMgr:OpenView(YuanQiSpeedGameView)
    ViewMgr:CloseView(MayDayView)
end

function YuanQiSpeedPanel:OnClickFetchFrist()
    if self.data.select_info.seq == -1 then
        return
    end
    YuanQiSpeedCtrl.Instance:SendFirstReward(self.data.select_info.seq)
end

YuanQiSpeedRabbitItem = YuanQiSpeedRabbitItem or DeclareMono("YuanQiSpeedRabbitItem", UIWidgetBaseItem)
function YuanQiSpeedRabbitItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local rabbit_cfg = YuanQiSpeedData.Instance:GetRabbitCfg(self.data.seq)
    UH.SetText(self.Name, rabbit_cfg.name)
    UH.SetText(self.BubbleText, rabbit_cfg.desc)
    -- UH.SetText(self.Icon, "Rabbit" .. (self.data.seq + 1))
    self:FlushRedNum()
end

function YuanQiSpeedRabbitItem:ToggleSelect()
    self.Toggle:Trigger()
end

function YuanQiSpeedRabbitItem:OnClickRabbit(is_on)
    if is_on then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
        YuanQiSpeedData.Instance:SetSelect(self.data.seq)

        UH.LocalScaleG(self.BG, Vector3.New(1, 1, 1))
        UH.LocalScaleG(self.Bottom, Vector3.New(1, 1, 1))
        self.Name.gameObject:SetLocalPosition(Vector3.New(0, -154, 0))
        self.RedNum.gameObject:SetLocalPosition(Vector3.New(126, 38, 0))
    else
        UH.LocalScaleG(self.BG, Vector3.New(0.86, 0.86, 1))
        UH.LocalScaleG(self.Bottom, Vector3.New(0.86, 0.86, 1))
        self.Name.gameObject:SetLocalPosition(Vector3.New(0, -132, 0))
        self.RedNum.gameObject:SetLocalPosition(Vector3.New(105, 35, 0))
    end
end

function YuanQiSpeedRabbitItem:FlushRedNum()
    local yet_get = YuanQiSpeedData.Instance:IsPassFetched(self.data.seq)
    local can_get = YuanQiSpeedData.Instance:IsPassFlag(self.data.seq)
    local value = (can_get and not yet_get) and 1 or 0
    self.RedNum:SetNum(value)
end
