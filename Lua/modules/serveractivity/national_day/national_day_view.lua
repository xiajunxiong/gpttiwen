NationalDayView = NationalDayView or DeclareView("NationalDayView", "serveractivity/national_day/national_day_view", Mod.NationalDay.Main)

VIEW_DECLARE_LEVEL(NationalDayView, ViewLevel.L0)

function NationalDayView:NationalDayView()
    self.handle = ActivityData.Instance:GetActivityEvent():Care(BindTool.Bind(self.OnFlushShowView, self))
    self.List:SetCompleteCallBack(function()
        if self.IsComplete == nil then
            self.ListAnim:Play()
            self.IsComplete = false
        end
    end)
    self:OnFlushShowView()
end

function NationalDayView:OnFlushShowView()
    local list = ServerActivityData.Instance:GetNationalDayList()
    self.ScrollList.enabled = #list > 4
    self.List:SetData(list)
end

function NationalDayView:OnClickClose()
    ViewMgr:CloseView(NationalDayView)
end

function NationalDayView:CloseCallback()
    ActivityData.Instance:GetActivityEvent():Uncare(self.handle)
end

----------------------------NationalDayItem----------------------------
NationalDayItem = NationalDayItem or DeclareMono("NationalDayItem", UIWidgetBaseItem)
function NationalDayItem:NationalDayItem()
    self.Time:SetCallBack(function()
        self.Time:SetTime(Language.LabourDay.TimeTips[3])
    end)
end

function NationalDayItem:SetData(data)
    self.Time:CloseCountDownTime()
    UH.RenameGame(self, [[Item]] .. data.index)
    UH.SetText(self.TxtName, data.text)
    UH.SetText(self.TxtDescrition, data.act_tag or "")
    self:FlushColorView(data.index % 6 == 0 and 6 or data.index % 6)
    local is_open = ActivityData.IsOpen(data.act_type)
    local open_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(data.act_type)
    if is_open then
        self:FlushRemindView(data.mod)
        self.Time:CreateActTime(data.act_type, TimeType.Type_Special_4, Language.NationalDay.TimeTips[1])
        UH.SetText(self.TxtBtnGo, Language.NationalDay.Go)
    else
        self.RedPos:SetNum(0)
        if (open_time or 0) >= TimeManager:GetServerTime() then
            self.Time:StampTime(open_time, TimeType.Type_Special_4, Language.NationalDay.TimeTips[2])
            UH.SetText(self.TxtBtnGo, Language.NationalDay.Prepare)
        else
            self.Time:SetTime(Language.NationalDay.TimeTips[3])
            UH.SetText(self.TxtBtnGo, Language.NationalDay.End)
            self:FlushColorView(0)
        end
    end
    self.Inter.Interactable = is_open
    self.ObjDown:SetActive(is_open)
    self.TxtBtnGo.color = is_open and COLORS.Yellow7 or COLORS.White
    self.ObjNew:SetActive(data.is_new)
    UIWidgetBaseItem.SetData(self, data)
end

function NationalDayItem:FlushColorView(index)
    self.TxtName.color = ServeractivityConfig.NationalDayColor[index]
    self.TxtTime.color = ServeractivityConfig.NationalDayColor[index]
    UH.SpriteName(self.ImgSetBg, [[Back_]] .. index)
    UH.SpriteName(self.ImgSetBtnGo, [[Btn_]] .. index)
end

function NationalDayItem:FlushRemindView(mod)
    if self.handle == nil then
        self.handle = Remind.Instance:RegisterGroup(mod, function()
            self.RedPos:SetNum(Remind.Instance:GetGroupNum(mod))
        end, true)
    end
end

function NationalDayItem:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Remind.Instance:UnRegisterGroup(self.data.mod, self.handle)
end

function NationalDayItem:OnClickItem()
    if self.data.is_new then
        ActivityData.Instance:SetHasSign(self.data.act_type)
        self.data.is_new = false
        self.ObjNew:SetActive(false)
    end
    if ActivityRandData.Instance:OnClickHandle(self.data.act_type) then
        
    else
        ActivityRandData.SwitchView(self.data.act_type, true)
    end
end