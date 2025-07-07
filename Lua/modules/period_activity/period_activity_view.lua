PeriodActivityView = PeriodActivityView or DeclareView("PeriodActivityView", "period_activity/period_activity_view",Mod.PeriodActivity.Main)

VIEW_DECLARE_LEVEL(PeriodActivityView,ViewLevel.L0)

function PeriodActivityView:PeriodActivityView()
    UH.SetListVisible(self.Panel,PeriodActivityData.Instance:GetPeriodType())
    if PeriodActivityCtrl.Instance.IsFirstEnter == nil then
        PeriodActivityCtrl.Instance.IsFirstEnter = true
        ActivityRandData.Instance:CheckRandOpenData()
    end
end

function PeriodActivityView:OnClickClose()
    ViewMgr:CloseView(PeriodActivityView)
end

function PeriodActivityView:OnClickShop()
    ViewMgr:OpenView(LoversShopView)
end

function PeriodActivityView:OnClickShop3()
    ViewMgr:OpenView(WenCangBookStoreView)
end

function PeriodActivityView:OnClickHelp(help_id)
    PublicPopupCtrl.Instance:HelpTip(help_id)
end

--===========================PeriodActivityPanel===========================
PeriodActivityPanel = PeriodActivityPanel or DeclareMono("PeriodActivityPanel", UIWFlushPanel)

function PeriodActivityPanel:PeriodActivityPanel()
    self.data = PeriodActivityData.Instance
end

function PeriodActivityPanel:Awake()
    UIWFlushPanel.Awake(self)
    local item_list = self.data:GetItemList()
    for i=1,self.ItemList:Length() do
        if item_list[i] then
            self.ItemList[i]:SetData(item_list[i])
        end
    end
    local stamp_time,is_show = self.data:GetActivityStampTime()
    local time_tip = Language.PeriodActivity.ActTime[is_show and 1 or 2]
    self.Time:StampTime(stamp_time,TimeType.Type_Special_4,time_tip)
end

----------------------------PeriodActivityItem----------------------------
PeriodActivityItem = PeriodActivityItem or DeclareMono("PeriodActivityItem", UIWidgetBaseItem)

function PeriodActivityItem:SetData(data)
    local _,is_open = PeriodActivityData.Instance:GetActivityStampTime()
    is_open = data.show_type == 7 and true or is_open
    UH.SetText(self.Name,Language.PeriodActivity.ItemListName[data.show_type])
    UH.SpriteName(self.ImgName,PeriodActivityConfig.ImgName .. data.show_type)
    UH.SpriteName(self.ImgBG,PeriodActivityConfig.ImgBGName .. (data.mod == Mod.PeriodActivity.SkyTreasureHunt and 1 or 2))
    UH.SetText(self.Time,self:GetTime(data.start_time,data.end_time))
    UH.SetInter(self.Inter,is_open)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushOpenView(data,is_open)
end

function PeriodActivityItem:FlushOpenView(data,is_open)
    if is_open == true then
        if not IsEmpty(data.mod) and PeriodActivityData.Instance:IsOpen(data) then
            self:FlushRemindView(data.mod)
        else
            self.RedPoint:SetNum(0)
        end
        self:FlushTimeView(data)
    else
        if PeriodActivityData.Instance:GetDataDayNum() > data.open_days then
            UH.SetText(self.Time,Language.PeriodActivity.ActEndTip)
        end
        self.RedPoint:SetNum(0)
    end
end

function PeriodActivityItem:FlushTimeView(data)
    if self:IsAllTime(data) then
        UH.SetText(self.Time,Language.PeriodActivity.AllTimeTip)
    end
    if data.end_time1 ~= 0 then
        UH.SetText(self.Time,self.Time.text .. "\n" .. self:GetTime(data.begin_time1,data.end_time1))
        UH.SpriteName(self.ImgTime,PeriodActivityConfig.ImgTime)
        UH.SpriteName(self.ImgBG,PeriodActivityConfig.ImgTimeBG)
    end
    local curr_time = ActivityData.Instance:GetCurrTime()
    self.Label:SetActive(not (IsRange(curr_time,data.start_time,data.end_time) or IsRange(curr_time,data.begin_time1,data.end_time1)))
end

function PeriodActivityItem:IsAllTime(data)
    if data.start_time == 0 and data.end_time == 2400 then
        return true
    end
    if data.begin_time1 == 0 and data.end_time1 == 2400 then
        return true
    end
end

function PeriodActivityItem:FlushRemindView(mod)
    if self.handle == nil then
        self.handle = RegisterRemind(mod,function(num)
            self.RedPoint:SetNum(num)
        end, true)
    end
end

function PeriodActivityItem:OnDestroy()
    if self.data == nil then return end
    UnRegisterRemind(self.data.mod,self.handle)
end

function PeriodActivityItem:GetTime(start_time,end_time)
    local s_time = ActivityData.Instance:GetTimeData(start_time)
    local e_time = ActivityData.Instance:GetTimeData(end_time)
    return Format((Language.PeriodActivity.TimeFormat),s_time.hour,s_time.min,e_time.hour,e_time.min)
end

function PeriodActivityItem:OnClickItem()
    if not self.Inter.Interactable then
        PublicPopupCtrl.Instance:Error(Language.PeriodActivity.EndTip)
        return
    end
    local period_type =  PeriodActivityData.Instance:GetPeriodType()
    if period_type <= 2 then
        ViewMgr:OpenView(PeriodActivityTipView,{config = self.data,is_open = PeriodActivityData.Instance:IsOpen(self.data)})
    else
        ViewMgr:OpenView(PeriodActivityInfoView,{config = self.data,is_open = PeriodActivityData.Instance:IsOpen(self.data)})
    end
end

----------------------------PeriodActivityTipView----------------------------
PeriodActivityTipView = PeriodActivityTipView or DeclareView("PeriodActivityTipView", "period_activity/period_activity_tip")

function PeriodActivityTipView:LoadCallback(param_t)
    local data = param_t.config
    local config = PeriodActivityConfig

    self.Texture:SetByPath(Format(config.TipPath,data.show_type))
    self.Grid:SetData(DataHelper.FormatItemList(data.reward_item))
    self.BtnList:SetActive(data.period_type == 2)
    self.BtnHelp:SetActive(data.show_type == 1 and PeriodActivityData.Instance:GetHelpHandle(data.mod) ~= nil)
    
    UH.SetInter(self.BtnInter[data.period_type],param_t.is_open)
    UH.SpriteName(self.Title,config.TitleKey .. data.show_type)
    UH.SetText(self.Desc,data.desc)

    local Position = self.Position[data.show_type]
    if Position.transform then
        Position:SetActive(true)
        UH.LocalPos(self.BGPosition,Position.transform.localPosition)
    else
        Position:SetData(data)
        Position:SetObjActive(true)
        local transform = Position.gameObject.transform
        UH.LocalPos(self.BGPosition,transform.localPosition)
    end
    local times_func = PeriodActivityData.Instance:GetTimesHandle(data.mod)
    if times_func ~= nil then
        UH.SetText(self.Times,Format(Language.PeriodActivity.TimesTip,times_func()))
    else
        UH.SetText(self.Times,Language.PeriodActivity.TimesTitle)
    end
    self.data = param_t.config
    if param_t.is_open then
        self:FlushRemindView(data.mod)
    else
        self.RedPoint[self.data.period_type]:SetNum(0)
    end
end

function PeriodActivityTipView:FlushRemindView(mod)
    if self.handle == nil then
        self.handle = RegisterRemind(mod,function(num)
            self.RedPoint[self.data.period_type]:SetNum(num)
        end, true)
    end
end

function PeriodActivityTipView:CloseCallback()
    if self.data ~= nil then
        UnRegisterRemind(self.data.mod,self.handle)
    end
end

function PeriodActivityTipView:OnClickEnter()
    if IsEmpty(self.data.mod) then
        return
    end
    if FightCtrl.IsActFB() then
		return
	end
    if not PeriodActivityData.Instance:IsOpen(self.data.mod) then
        PublicPopupCtrl.Instance:Error(Language.PeriodActivity.EnterError1)
        return
    end
    if ActivityRandData.Instance:OnClickHandle(self.data.mod) then
        ViewMgr:CloseView(PeriodActivityView)
    else
        ViewMgr:OpenViewByKey(self.data.mod)
    end
    ViewMgr:CloseView(PeriodActivityTipView)
end

function PeriodActivityTipView:OnClickClose()
    ViewMgr:CloseView(PeriodActivityTipView)
end

function PeriodActivityTipView:OnClickRank()
    ViewMgr:CloseView(PeriodActivityTipView)
    ViewMgr:OpenView(LoversMatchRankView)
end

function PeriodActivityTipView:OnClickHelp()
    Call(PeriodActivityData.Instance:GetHelpHandle(self.data.mod))
end
--===========================PeriodActivityTipItem===========================
PeriodActivityTipItem2 = PeriodActivityTipItem2 or DeclareMono("PeriodActivityTipItem2", UIWidgetBaseItem)

function PeriodActivityTipItem2:SetData(data)
    FairyWhereCtrl.Instance:SendFairyWhereReq(2)
end

PeriodActivityTipItem5 = PeriodActivityTipItem5 or DeclareMono("PeriodActivityTipItem5", UIWidgetBaseItem)

function PeriodActivityTipItem5:SetData(data)
    self.TitleList[1]:SetActive(data.mod == Mod.PeriodActivity.SanXianGuiDong)
    self.TitleList[2]:SetActive(data.mod == Mod.PeriodActivity.LuckyRing)
end

PeriodActivityTipItem6 = PeriodActivityTipItem6 or DeclareMono("PeriodActivityTipItem6", UIWidgetBaseItem)

function PeriodActivityTipItem6:SetData(data)
    self.Grid:SetData(DataHelper.FormatItemList(data.reward_item))
    local times_func = PeriodActivityData.Instance:GetTimesHandle(data.mod)
    if times_func ~= nil then
        UH.SetText(self.Times,Format(Language.PeriodActivity.TimesTip,times_func()))
    else
        UH.SetText(self.Times,Language.PeriodActivity.TimesTitle)
    end
    UH.SetText(self.Desc,data.desc)
end



PeriodActivityItem2 = PeriodActivityItem2 or DeclareMono("PeriodActivityItem2", PeriodActivityItem)
function PeriodActivityItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local _,is_open = PeriodActivityData.Instance:GetActivityStampTime()
    is_open = data.show_type == 7 and true or is_open
    UH.SetText(self.Name, data.plays_name)
    UH.SetText(self.Time,self:GetTime(data.start_time,data.end_time))
    UH.SetInter(self.Inter,is_open)
    self:FlushOpenView(data,is_open)
end