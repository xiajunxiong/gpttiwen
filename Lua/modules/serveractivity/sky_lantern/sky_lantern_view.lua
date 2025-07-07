SkyLanternView = SkyLanternView or DeclareView("SkyLanternView", "serveractivity/sky_lantern/sky_lantern", Mod.MayDay.SkyLantern)
function SkyLanternView:LoadCallback()
    -- ViewMgr:CloseView(MayDayView)
end

function SkyLanternView:CloseCallback()
    BagData.Instance:SetHideCurrencyCenterFlag(false)
end

SkyLanternPanel = SkyLanternPanel or DeclareMono("SkyLanternPanel", UIWFlushPanel)
function SkyLanternPanel:SkyLanternPanel()
    self.data_cares = {
        {data = SkyLanternData.Instance.act_info, func = self.FlushView, init_call = false}
    }
    self.open_time = TimeHelper.GetServerTime() + 1
end

function SkyLanternPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.BtnAdd:SetActive(IS_EDITOR)
    UH.SetText(self.Text3, Format(Language.SkyLantern.Text3Desc, SkyLanternData.Instance:OtherConfig().reward_increase))

    local fly_time1, fly_time2 = SkyLanternData.Instance:GetFlyTime()
    UH.SetText(self.TimeDesc1, Format(Language.SkyLantern.TimeStr, self:TimeStr(fly_time1), self:TimeStr(fly_time2)))
    self:ShowActTime()

    local reward_cfg = SkyLanternData.Instance:GetRewardList()
    if reward_cfg ~= nil then
        self.ItemShowList:SetData(DataHelper.FormatItemList(reward_cfg.reward_item))
    end
    self.WaitTimer:SetCallBack(BindTool.Bind1(self.WaitTimeOver, self))
    self.WaitTimer:SetShowCallBack(BindTool.Bind1(self.WaitTimeShow, self))

    ActivityData.Instance:SetHasSign(ACTIVITY_TYPE.RAND_SKY_LANTERN)
    SkyLanternData.Instance.act_info:Notify()
    SkyLanternData.Instance:CheckRemind()
    BagData.Instance:SetHideCurrencyCenterFlag(true)
end

function SkyLanternPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.play_timer)
    self.play_timer = nil
    TimeHelper:CancelTimer(self.delay_effect)
    self.delay_effect = nil
end

function SkyLanternPanel:FlushView()
    if self.play_timer ~= nil then
        return
    end
    self.TaskList:SetData(SkyLanternData.Instance:GetTaskList())

    local consume_num = SkyLanternData.Instance:GetConcuseNum()
    local have_num = SkyLanternData.Instance:GetItemNum()
    UH.SetText(self.ItemNum, Format("%d/%d", have_num, consume_num))
    self:SetClubEffect(have_num >= consume_num)

    UH.SpriteName(self.DengImg, "AnDeng")
    if SkyLanternData.Instance:IsFlyTime() then
        UH.SpriteName(self.BtnPlayImg, "AnNiuLiang")
        self.WaitTimer:SetObjActive(false)
        self.WaitTimer:StampTime(SkyLanternData.Instance:GetOverTime())
    else
        UH.SpriteName(self.BtnPlayImg, "AnNiuAn")
        self.WaitTimer:SetObjActive(true)
        local next_time = SkyLanternData.Instance:GetNextStartTime()
        local is_over = next_time >= ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_SKY_LANTERN)
        if is_over then
            self.WaitTimer:SetTime("")
            UH.SetText(self.WaitTimeText, Language.SkyLantern.DianLiangStr2)
            UH.LocalPosG(self.WaitTimeText.gameObject, Vector3.New(0, 0, 0))
        else
            UH.SetText(self.WaitTimeText, Language.SkyLantern.DianLiangStr1)
            self.WaitTimer:StampTime(next_time)
        end
    end
end

function SkyLanternPanel:ShowActTime()
    local open_time = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_SKY_LANTERN)
    if open_time > 0 then
        local open_t = os.date("*t", open_time)
        local end_t = os.date("*t", ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_SKY_LANTERN) - 1)
        UH.SetText(self.TimeDesc2, Format(Language.SkyLantern.ActTime, open_t.month, open_t.day, end_t.month, end_t.day))
    else
        UH.SetText(self.TimeDesc2, Format(Language.SkyLantern.ActTime, 1, 1, 1, 1))
    end
end

function SkyLanternPanel:TimeStr(time_t)
    local hour = time_t.hour > 12 and time_t.hour - 12 or time_t.hour
    if time_t.min ~= 0 then
        return Format(Language.SkyLantern.TimeT[2], hour, time_t.min)
    end
    return Format(Language.SkyLantern.TimeT[1], hour)
end

function SkyLanternPanel:WaitTimeShow(time_t)
    return Format(Language.SkyLantern.WaitTimeStr, time_t.hour, time_t.min, time_t.s)
end
function SkyLanternPanel:WaitTimeOver()
    self:FlushView()
end

function SkyLanternPanel:OnClickPlay()
    local consume_num = SkyLanternData.Instance:GetConcuseNum()
    local have_num = SkyLanternData.Instance:GetItemNum()
    if consume_num > have_num or not SkyLanternData.Instance:IsFlyTime() then
        SkyLanternCtrl.Instance:SendPlay()
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        return
    end
    AudioMgr:PlayEffect(AudioType.Scene, "enter_game")
    self.ClubAnim:Play("diandeng")
    UH.SetInter(self.BtnPlayInter, false)
    UH.SetInter(self.BtnCloseInter, false)
    SkyLanternData.Instance:ClearNeedDelayItems()
    SkyLanternCtrl.Instance:SendPlay()
    -- UH.SpriteName(self.DengImg, "LiangDeng")
    local function func_show()
        self.play_timer = nil
        -- UH.SpriteName(self.DengImg, "AnDeng")
        self.DengEffect:SetActive(false)
        UH.SetInter(self.BtnPlayInter, true)
        UH.SetInter(self.BtnCloseInter, true)
        self:FlushView()
    end
    local function func_show_reward()
        SkyLanternCtrl.Instance:ShowGetItems()
    end
    Invoke(func_show_reward, 3)
    self.play_timer = Invoke(func_show, 4)
end

function SkyLanternPanel:OnClickShow()
    self.ItemShow:SetActive(true)
end
function SkyLanternPanel:OnClickShowBlock()
    self.ItemShow:SetActive(false)
end

function SkyLanternPanel:OnClickAdd(param)
    SkyLanternCtrl.Instance:GMAddHuoZhong(param)
end

function SkyLanternPanel:SetClubEffect(is_active)
    if is_active then
        if self.open_time < TimeHelper.GetServerTime() then
            TimeHelper:CancelTimer(self.delay_effect)
            self.delay_effect = nil
            self.ClubEffect:SetActive(true)
        elseif self.delay_effect == nil then
            local function func_delay()
                self.delay_effect = nil
                self.ClubEffect:SetActive(true)
            end
            self.delay_effect = Invoke(func_delay, 0.5)
        end
    else
        TimeHelper:CancelTimer(self.delay_effect)
        self.delay_effect = nil
        self.ClubEffect:SetActive(false)
    end
end

function SkyLanternPanel:OnClickClose()
    self.ClubEffect:SetActive(false)
    ViewMgr:CloseView(SkyLanternView)
end


SkyLanternTaskItem = SkyLanternTaskItem or DeclareMono("SkyLanternTaskItem", UIWidgetBaseItem)
function SkyLanternTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local is_over = SkyLanternData.Instance:IsTaskOver(self.data.seq1)
    self.BtnGo:SetActive(not is_over)
    self.ImgOver:SetActive(is_over)

    local prog_str = ""
    if is_over then
        prog_str = Format("(%d/%d)", data.parameter, data.parameter)
    else
        local prog = SkyLanternData.Instance:GetTaskProg(self.data.seq1)
        prog_str = Format("(%s/%s)", ColorStr(prog, COLORSTR.Red8), data.parameter)
    end
    UH.SetText(self.Desc, data.describe .. ColorStr(prog_str, COLORSTR.Yellow15))
end

function SkyLanternTaskItem:OnClickGo()
    ViewMgr:OpenViewByKey(self.data.open_panel)
    ViewMgr:CloseView(MayDayView)
    ViewMgr:CloseView(SkyLanternView)
end
