SummerFirstView = SummerFirstView or DeclareView("SummerFirstView", "serveractivity/summer_first/summer_first_view",Mod.SummerFirst.Main)

VIEW_DECLARE_LEVEL(SummerFirstView,ViewLevel.L0)

function SummerFirstView:SummerFirstView()
    self.data = ServerActivityData.Instance
end

function SummerFirstView:LoadCallback()
    local phase = self.data:GetSummerFirstStage()
    local page_list = DataHelper.GetPageData(self.data:GetSummerFirstList(),6)
    self.PageList[phase]:SetData(page_list[phase] or {})
    for i=1,self.PageList:Length() do
        self.PageList[i]:SetObjActive(phase == i)
    end
    -- self.ContentWidth = self.Content.rect.width / (self.PageList:Length() * 2)
    -- self.MaxItemWidth = self.ContentWidth * (self.PageList:Length() + 2)       
end

function SummerFirstView:OnClickClose()
    ViewMgr:CloseView(SummerFirstView)
end

-- function SummerFirstView:OnClickDrag()
--     if self.Content.localPosition.x > -self.ContentWidth then
--         UH:SetRectPosition(self.Content,{x = -self.ContentWidth})
--     end
--     if self.Content.localPosition.x < -self.MaxItemWidth then
--         UH:SetRectPosition(self.Content,{x = -self.MaxItemWidth})
--     end
-- end

----------------------------SummerFirstPageItem----------------------------
SummerFirstPageItem = SummerFirstPageItem or DeclareMono("SummerFirstPageItem", UIWidgetBaseItem)

function SummerFirstPageItem:SummerFirstPageItem()
    local canvas = ViewMgr:GetMainCanvasRect().rect
    self.Rect.sizeDelta = Vector2.New(canvas.width,canvas.height)
end

function SummerFirstPageItem:SetData(data)
    for i=1,self.ItemList:Length() do
        if data[i] ~= nil then
            self.ItemList[i]:SetData(data[i])
        end
    end
end

----------------------------SummerFirstlItem----------------------------
SummerFirstlItem = SummerFirstlItem or DeclareMono("SummerFirstlItem", UIWidgetBaseItem)

function SummerFirstlItem:SummerFirstlItem()
    self.Time:SetCallBack(function()
        self.Time:SetTime(Language.SpringFestival.TimeTips[3])
    end)
end

function SummerFirstlItem:SetData(data)
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

function SummerFirstlItem:OnClickItem()
    if self.data.act_type == ACTIVITY_TYPE.RAND_LOTUS then
        ViewMgr:OpenView(LotusGuideView)
        return
    end
    ViewMgr:OpenView(SummerFirstTipView,{config = self.data,root_pos = UH.GetRootPosition(self.Root) + Vector2.New(398,-172)})
end

function SummerFirstlItem:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.data and self.handle then
        Remind.Instance:UnRegisterGroup(self.data.mod,self.handle)
    end
end

----------------------------SummerFirstTipView----------------------------
SummerFirstTipView = SummerFirstTipView or DeclareView("SummerFirstTipView","serveractivity/summer_first/summer_first_tip")
VIEW_DECLARE_MASK(SummerFirstTipView,ViewMask.BlockClose)
VIEW_DECLARE_LEVEL(SummerFirstTipView,ViewLevel.L1)

function SummerFirstTipView:LoadCallback(param_t)
    self.config = param_t.config or {}
    UH.SetText(self.Title,self.config.text)
    UH.SetText(self.Desc,self.config.desc)
    self.Grid:SetData(DataHelper.FormatItemList(self.config.reward_item))
    UH.LocalPos(self.Root,param_t.root_pos)
    UH.SetEdgePosition(self.Root,self.Rect)
end

function SummerFirstTipView:OnClickEnter()
    if ActivityRandData.Instance:OnClickHandle(self.config.act_type) then
        ViewMgr:CloseView(SummerFirstView)
    else
        ActivityRandData.SwitchView(self.config.act_type,true)
    end
    ViewMgr:CloseView(SummerFirstTipView)
end