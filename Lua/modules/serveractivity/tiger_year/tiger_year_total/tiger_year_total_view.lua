TigerYearTotalView = TigerYearTotalView or DeclareView("TigerYearTotalView", "serveractivity/tiger_year/tiger_year_total_view",Mod.TigerYear.Main)
VIEW_DECLARE_LEVEL(TigerYearTotalView,ViewLevel.L0)

function TigerYearTotalView:TigerYearTotalView()
    self:SetTimerCloseView()
end

function TigerYearTotalView:SetTimerCloseView()
	self.timer_close_view = TimeHelper:AddRunTimer(function ()
   		if TigerYearTotalCtrl.Instance:GetTigerActIsOpen() then
				PublicPopupCtrl.Instance:Error(Language.PlanActivity.TipsText)
				ViewMgr:CloseView(TigerYearTotalView)
			end
	end, 1)	
end

function TigerYearTotalView:LoadCallback()
    local list = TigerYearTotalCtrl.Instance:CacheCurActList()
    for i=1,self.ItemList:Length() do
        if list[i] ~= nil then
            self.ItemList[i]:SetData(list[i])
        end
        self.ItemList[i]:SetObjActive(list[i] ~= nil)
    end
end

function TigerYearTotalView:CloseCallback()
	TimeHelper:CancelTimer(self.timer_close_view)
end

function TigerYearTotalView:OnClickClose()
    ViewMgr:CloseView(TigerYearTotalView)
end

----------------------------SimpleNewYearTItem----------------------------
SimpleNewYearTItem = SimpleNewYearTItem or DeclareMono("SimpleNewYearTItem", UIWidgetBaseItem)
function SimpleNewYearTItem:SimpleNewYearTItem()
    self.Time:SetCallBack(function()
        self.Time:SetTime(Language.TigerYearTotal.TimeTips[3])
    end)
end
-- data{text(名字),act_type(活动类型),mod(模块组)}
function SimpleNewYearTItem:SetData(data)
    UH.SetText(self.Title,string.sub(data.text,1,3))
    UH.SetText(self.Name,string.sub(data.text,4,-1))
    -- self.imgset

    local is_open = ActivityData.IsOpen(data.act_type)
    local TimeTips = Language.TigerYearTotal.TimeTips
    local open_time,end_time = ActivityRandData.GetBaseDayOpenStampTime(data.act_type)
   
    self.mod = TabMod(data.mod_key)
    if is_open then
        if self.handle == nil then
            self.handle = Remind.Instance:RegisterGroup(self.mod,function()
                self.RedPos:SetNum(Remind.Instance:GetGroupNum(self.mod))
            end, true)
        end
        self.Time:CreateActTime(data.act_type,TimeType.Type_Special_4,TimeTips[1])
    else
        self.RedPos:SetNum(0)
        if open_time and open_time >= TimeManager:GetServerTime() then
            self.Time:StampTime(open_time,TimeType.Type_Special_4,TimeTips[2])
        else
            self.Time:SetTime(TimeTips[3])
        end
    end

    self.Inter.Interactable = is_open
    UIWidgetBaseItem.SetData(self, data)
end

function SimpleNewYearTItem:OnClickItem()
    if ActivityRandData.Instance:OnClickHandle(self.data.act_type) then
        -- ViewMgr:CloseView(TigerYearTotalView)
    else
        ActivityRandData.SwitchView(self.data.act_type,true)
    end
    -- ViewMgr:CloseView(TigerYearTotalView)
end

function SimpleNewYearTItem:OnDestroy()
    if self.data then
	    Remind.Instance:UnRegisterGroup(self.mod,self.handle)
    end
end

--虎年活动提醒监听
--================================MainViewTigerYearView================================
MainViewTigerYearView = MainViewTigerYearView or DeclareMono("MainViewTigerYearView", UIWFlushPanel)

function MainViewTigerYearView:MainViewTigerYearView()
    self.data_cares = {
        {data = ActivityData.Instance:GetActivityEvent(),func = self.OnFlush,keys = {"act_type", "status"}},
        {data = RoleData.Instance:GetBaseData(), func = self.OnFlush, init_call = false, keys = {"level"}},
        {data = ActivityData.Instance.acts_tips_data,func = self.FlushTipsView,init_call = false},
    }

    self.Timer:SetShowCallBack(function (t)
        local t_str = ""

        if t.day ~= nil and tonumber(t.day) > 0 then 
            t_str = string.format(Language.UiTimeMeter.TimeStr3, t.day, t.hour ,t.min)
        elseif (t.day == nil or tonumber(t.day) == 0) and tonumber(t.hour) > 0 then 
            t_str = string.format(Language.UiTimeMeter.Special4, t.hour, t.min)
        elseif (t.day == nil or tonumber(t.day) == 0) and (t.hour == nil or tonumber(t.hour) == 0) and tonumber(t.min) > 0 then 
            t_str = string.format(Language.UiTimeMeter.Specia2, t.min, t.s)
        end 
        return t_str
    end)
end

function MainViewTigerYearView:OnFlush()
    self:FlushTipsView()
end

function MainViewTigerYearView:FlushTipsView()
    self.Tips:SetActive(TigerYearTotalCtrl.Instance:GetTigerYearTip())
	local begin_time,end_time = TigerYearTotalCtrl.Instance:GetActBeginAndEndAndDayTime()
	self.Timer:StampTime(end_time,TimeType.Type_Time_3, "")
end
