
MayDayView = MayDayView or DeclareView("MayDayView", MayDayConfig.ResPath .. "may_day", Mod.MayDay.Main)
VIEW_DECLARE_LEVEL(MayDayView, ViewLevel.L0)

function MayDayView:LoadCallback()
    self.Currency:SetCurrency(CurrencyType.MayDayKey)
end

function MayDayView:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(MayDayView)
end

MayDayViewF = MayDayViewF or DeclareMono("MayDayViewF", UIWFlushPanel)

function MayDayViewF:MayDayViewF()
    self.data = MayDayCtrl.Instance:Data()
    self.act_data = ActivityData.Instance

    self.data_cares = {
        {data = self.act_data.activity_status_event, func = self.FlushShow},
        {data = self.data.main_info, func = self.FlushDayRewardShow},
        {data = self.data.main_sm, func = self.FlushShow, init_call = false},
    }

    self.ShowList:SetCompleteCallBack(function()
        if self.IsComplete == nil then
            self.ListAnim:Play()
            self.timer_dt = TimeHelper:AddDelayTimer(function ()
                self.ShowAnim = true
                self:FlushScrollRect()
            end, 1)
            self.IsComplete = false
        end
    end)
    self.ListMove = true
    self.ShowAnim = false
end

function MayDayViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushArrowRemind()
end

function MayDayViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_dt)

    if self.red_point ~= nil then
        self.red_point:DeleteMe()
        self.red_point = nil
    end

    if self.red_point_arrow ~= nil then
        self.red_point_arrow:DeleteMe()
        self.red_point_arrow = nil
    end
end

function MayDayViewF:FlushArrowRemind()
    self.red_point_arrow = self.red_point_arrow or UiRedPoint.New(self.MoreObj, Vector3.New(25,25,0))
    self.red_point_arrow:SetNum(DialogTipsData.Instance:TodayIsTrue(DTNewTodayNotRemind.MayDay) and 0 or 1)
end

function MayDayViewF:FlushShow()
    local show_list, show_item = self.data:GetMayDayShowList()
    self.ShowList:SetData(show_list)
    self.ShowItem:SetData(show_item)
    self.ListMove = #show_list > 4
    self:FlushScrollRect()
end

function MayDayViewF:FlushScrollRect()
    local can_move = self.ListMove and self.ShowAnim
    self.ScrollRect.enabled = can_move
    self.MoreObj:SetActive(can_move)
end

function MayDayViewF:FlushDayRewardShow()
    local is_fetch_day_reward = self.data.main_info.is_fetch_day_reward
    
    if 0 == is_fetch_day_reward then
        local reward_item = self.data:MainDayRewardItem()
        self.DayRewardName.text = reward_item:Name()
        self.DayRewardCell:SetData(reward_item)
    end
    self.red_point = self.red_point or UiRedPoint.New(self.DayRewardObj, Vector3.New(40,57,0))
    self.red_point:SetNum(0 == is_fetch_day_reward and 1 or 0)
    
    self.DayRewardObj:SetActive(0 == is_fetch_day_reward)
end


function MayDayViewF:OnListPressDown()
    if self.ListMove and self.ShowAnim then
        self.ListAnim:Play("list_press")
    end
end

function MayDayViewF:OnListPressUp()
    if self.ListMove and self.ShowAnim then
        self.ListAnim:Play("list_up")
    end
end

function MayDayViewF:OnClickArrow()
    local pos = self.ContentPos.localPosition
    local size = self.ContentPos.sizeDelta
    self.ContentPos.localPosition = Vector2.New(pos.x - math.min(566, size.x + pos.x), pos.y)

    DialogTipsData.Instance:SetTodayRemind(DTNewTodayNotRemind.MayDay)
    self:FlushArrowRemind()
end

MayDayViewItem = MayDayViewItem or DeclareMono("MayDayViewItem", UIWidgetBaseItem)
function MayDayViewItem:MayDayViewItem()
    self.md_data = MayDayCtrl.Instance:Data()
end

function MayDayViewItem:OnDestroy()
    self:UnRegisterGroup()

    if self.red_point ~= nil then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
end

function MayDayViewItem:UnRegisterGroup()
    Remind.Instance:UnRegisterGroup(self.data.mod, self.handle)
    self.handle = nil
end

function MayDayViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    
    self.ItemObj.name = "Item" .. data.index
    self.NameShow.text = data.text
    self.IconSp.SpriteName = data.picture
    self.RecoObj:SetActive(1 == data.show)
    
    local is_open = ActivityData.IsOpen(data.act_type)
    local open_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(data.act_type)

    local state = is_open and 1 or ((open_time or 0) >= TimeManager:GetServerTime() and 2 or 3)
    self.OpenObj:SetActive(1 == state)
    self.StandyObj:SetActive(2 == state)

    self.OpenTime:CloseCountDownTime()
    self.StandyTime:CloseCountDownTime()
    if is_open then
        self.OpenTime:CreateActTime(data.act_type, TimeType.Type_Special_9, Language.MayDay.TimeTips[1])
        self.BtnGoShow.text = Language.MayDay.BtnGoShow[data.main_btn_state]
    else
        if (open_time or 0) >= TimeManager:GetServerTime() then
            self.StandyTime:StampTime(open_time, TimeType.Type_Special_9, Language.MayDay.TimeTips[2])
        else
            self.StandyTime:SetTime(Language.MayDay.TimeTips[3])
        end
    end

    local rewards_data = self.md_data:GetRewardPreview(data.act_type)
    local rewards_item = DataHelper.FormatItemList(rewards_data)
    self.RewardList:SetData(rewards_item)

    self:FlushRemind(is_open)
end

function MayDayViewItem:FlushRemind(is_open)
    self:UnRegisterGroup()
    if is_open and self.handle == nil then
        self.handle = Remind.Instance:RegisterGroup(self.data.mod, function()
            self.red_point = self.red_point or UiRedPoint.New(self.BtnGoObj, Vector3.New(90,25,0))
            self.red_point:SetNum(is_open and (Remind.Instance:GetGroupNum(self.data.mod) > 0) and 1 or 0)
        end, true)
    end
end

function MayDayViewItem:OnClickGo()
    ActivityRandData.SwitchView(self.data.act_type, true)
    if ActivityData.IsOpen(self.data.act_type) and FunOpen.Instance:IsFunOpen(self.data.mod_key) then
        PeriodActivityData.Instance:SetRemindData(self.data.mod_key)
    end
end

MayDayViewItemMain = MayDayViewItemMain or DeclareMono("MayDayViewItemMain", UIWidgetBaseItem)
function MayDayViewItemMain:MayDayViewItemMain()
    self.md_data = MayDayCtrl.Instance:Data()
end

function MayDayViewItemMain:OnDestroy()
    Remind.Instance:UnRegisterGroup(self.data.mod, self.handle)

    if self.red_point ~= nil then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
end

function MayDayViewItemMain:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    
    self.NameShow.text = data.text

    local is_open = ActivityData.IsOpen(data.act_type)
    if is_open and self.handle == nil then
        self.handle = Remind.Instance:RegisterGroup(self.data.mod, function()
            self.red_point = self.red_point or UiRedPoint.New(self.BtnGoObj, Vector3.New(25,25,0))
            self.red_point:SetNum((Remind.Instance:GetGroupNum(self.data.mod) > 0) and 1 or 0)
        end, true)
    end
end

function MayDayViewItemMain:OnClickGo()
    ActivityRandData.SwitchView(self.data.act_type, true)
end


--=========================MayDayBoard=========================
MayDayBoard = MayDayBoard or DeclareMono("MayDayBoard")

function MayDayBoard:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end

function MayDayBoard:SetData(view_type, title_name)
	self.view_type = view_type
    UH.SpriteName(self.Title,title_name)
end