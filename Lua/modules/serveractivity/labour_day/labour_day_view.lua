LabourDayView = LabourDayView or DeclareView("LabourDayView", "serveractivity/labour_day/labour_day_view",Mod.LabourDay.Main)

VIEW_DECLARE_LEVEL(LabourDayView,ViewLevel.L0)

function LabourDayView:LoadCallback()
    self:FlushListView(ServerActivityData.Instance:GetMayDayList())
end

function LabourDayView:FlushListView(list)
    self.List:SetData(list)
    self.ScrollList.enabled = #list > 6
    self.List:SetCompleteCallBack(function()self.ListAnim:Play()end)
end

function LabourDayView:OnClickClose()
    ViewMgr:CloseView(LabourDayView)
end

----------------------------LabourDayItem----------------------------
LabourDayItem = LabourDayItem or DeclareMono("LabourDayItem", UIWidgetBaseItem)

function LabourDayItem:LabourDayItem()
    self.Time:SetCallBack(function()
        self.Time:SetTime(Language.LabourDay.TimeTips[3])
    end)
end

function LabourDayItem:SetData(data)
    UH.SetText(self.Name,data.text)
    self.gameObject.name = "Item" .. data.seq
    UH.LocalPos(self.Root,Vector2.New(0,ServeractivityConfig.LabourDayPos[data.seq % 7] or 0))

    local is_open = ActivityData.IsOpen(data.act_type)
    local open_time,end_time = ActivityRandData.GetBaseDayOpenStampTime(data.act_type)
    if is_open then
        self:FlushRemindView(data.mod)
        self.Time:CreateActTime(data.act_type,TimeType.Type_Special_4,Language.LabourDay.TimeTips[1])
        -- self.Time:StampTime(end_time,TimeType.Type_Special_4,Language.LabourDay.TimeTips[1])
    else
        self.RedPos:SetNum(0)
        if (open_time or 0) >= TimeManager:GetServerTime() then
            self.Time:StampTime(open_time,TimeType.Type_Special_4,Language.LabourDay.TimeTips[2])
        else
            self.Time:SetTime(Language.LabourDay.TimeTips[3])
        end
    end

    self.Inter.Interactable = is_open
    self.Effect:SetActive(is_open)
    UIWidgetBaseItem.SetData(self, data)
end

function LabourDayItem:FlushRemindView(mod)
    if self.handle == nil then
        self.handle = Remind.Instance:RegisterGroup(mod,function()
            self.RedPos:SetNum(Remind.Instance:GetGroupNum(mod))
        end, true)
    end
end

function LabourDayItem:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Remind.Instance:UnRegisterGroup(self.data.mod,self.handle)
end

function LabourDayItem:OnClickItem()
    ViewMgr:OpenView(LabourDayTipView,{config = self.data,root_pos = UH.GetRootPosition(self.Root) + Vector2.New(240,-10)})
end

LabourDayTipView = LabourDayTipView or DeclareView("LabourDayTipView", "serveractivity/labour_day/labour_day_tip")
VIEW_DECLARE_MASK(LabourDayTipView,ViewMask.None)
function LabourDayTipView:LoadCallback(param_t)
    self.config = param_t.config or {}
    UH.SetText(self.Title,self.config.text)
    UH.SetText(self.Desc,self.config.desc)
    local list = DataHelper.FormatItemList(self.config.reward_item)
    self.Grid:SetData(list)
    if #list > 3 then
		self.Content.pivot = Vector2.New(0,0.5)
    end
    UH.LocalPos(self.Root,param_t.root_pos)
    UH.SetEdgePosition(self.Root,self.Rect)
end

function LabourDayTipView:OnClickEnter()
    if ActivityRandData.Instance:OnClickHandle(self.config.act_type) then
        ViewMgr:CloseView(LabourDayView)
    else
        ActivityRandData.SwitchView(self.config.act_type,true)
    end
    ViewMgr:CloseView(LabourDayTipView)
end

function LabourDayTipView:OnClickClose()
    ViewMgr:CloseView(LabourDayTipView)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end