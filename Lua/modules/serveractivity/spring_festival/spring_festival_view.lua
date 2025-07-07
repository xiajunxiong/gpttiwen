SpringFestivalView = SpringFestivalView or DeclareView("SpringFestivalView", "serveractivity/spring_festival/spring_festival_view",Mod.SpringFestival.Main)

VIEW_DECLARE_LEVEL(SpringFestivalView,ViewLevel.L0)

function SpringFestivalView:LoadCallback()
    local list = ServerActivityData.Instance:GetSpringFestivalList()
    for i=1,self.ItemList:Length() do
        if list[i] ~= nil then
            self.ItemList[i]:SetData(list[i])
        end
        self.ItemList[i]:SetObjActive(list[i] ~= nil)
    end
end

function SpringFestivalView:OnClickClose()
    ViewMgr:CloseView(SpringFestivalView)
end

----------------------------SpringFestivalItem----------------------------
SpringFestivalItem = SpringFestivalItem or DeclareMono("SpringFestivalItem", UIWidgetBaseItem)

function SpringFestivalItem:SpringFestivalItem()
    self.Time:SetCallBack(function()
        self.Time:SetTime(Language.SpringFestival.TimeTips[3])
    end)
end

function SpringFestivalItem:SetData(data)
    -- UH.SetText(self.Title,string.sub(data.text,1,3))
    -- UH.SetText(self.Name,string.sub(data.text,4,-1))
    UH.SetText(self.Name,data.text)
    local is_open = ActivityData.IsOpen(data.act_type)
    local TimeTips = Language.SpringFestival.TimeTips
    local open_time,end_time = ActivityRandData.GetBaseDayOpenStampTime(data.act_type)
    if is_open then
        if self.handle == nil then
            self.handle = Remind.Instance:RegisterGroup(data.mod,function()
                self.RedPos:SetNum(Remind.Instance:GetGroupNum(data.mod))
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

function SpringFestivalItem:OnClickItem()
    local pos = Vector2.New(self.Root.localPosition.x,self.Root.localPosition.y)
    ViewMgr:OpenView(SpringFestivalTipView,{config = self.data,root_pos = pos})
end

function SpringFestivalItem:OnDestroy()
    if self.data then
	    Remind.Instance:UnRegisterGroup(self.data.mod,self.handle)
    end
end

----------------------------SpringFestivalTipView----------------------------
SpringFestivalTipView = SpringFestivalTipView or DeclareView("SpringFestivalTipView","serveractivity/spring_festival/spring_festival_tip")
VIEW_DECLARE_MASK(SpringFestivalTipView,ViewMask.BlockClose)
VIEW_DECLARE_LEVEL(SpringFestivalTipView,ViewLevel.L1)

function SpringFestivalTipView:LoadCallback(param_t)
    self.config = param_t.config or {}
    UH.SetText(self.Title,self.config.text)
    UH.SetText(self.Desc,self.config.desc)
    self.Grid:SetData(DataHelper.FormatItemList(self.config.reward_item))
    -- UH.LocalPos(self.Root,param_t.root_pos + Vector2.New(220,-235))
    UH.LocalPos(self.Root,param_t.root_pos + Vector2.New(180,-240))
    UH.SetEdgePosition(self.Root,self.Rect)
end

function SpringFestivalTipView:OnClickEnter()
    if ActivityRandData.Instance:OnClickHandle(self.config.act_type) then
        ViewMgr:CloseView(SpringFestivalView)
    else
        ActivityRandData.SwitchView(self.config.act_type,true)
    end
    ViewMgr:CloseView(SpringFestivalTipView)
end