ArticulatelyView = ArticulatelyView or DeclareView("ArticulatelyView",MayDayConfig.ResPath .. "articulately_view", Mod.MayDay.Articulately)

function ArticulatelyView:LoadCallback()
	local begin_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(ACTIVITY_TYPE.RAND_ARTICULATELY)
    begin_time = begin_time or ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_ARTICULATELY)
    end_time = end_time or ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_ARTICULATELY)
    local begin_month,end_month = os.date("%m",begin_time),os.date("%m",end_time)
    local open_day,end_day = os.date("%d",begin_time),os.date("%d",end_time-1)
    begin_month = tonumber(begin_month)
    end_month = tonumber(end_month)
    open_day = tonumber(open_day)
    end_day = tonumber(end_day)
    UH.SetText(self.Time,Format(Language.Articulately.TimeTip,begin_month,open_day,end_month,end_day))
end

function ArticulatelyView:OnClickGet()
    ViewMgr:OpenViewByKey(Config.miaobishenghua_auto.other[1].open_panel)
    ViewMgr:CloseView(ArticulatelyView)
end

function ArticulatelyView:OnClickClose()
    ViewMgr:CloseView(ArticulatelyView)
end

--===========================ArticulatelyPanel===========================
ArticulatelyPanel = ArticulatelyPanel or DeclareMono("ArticulatelyPanel", UIWFlushPanel)

function ArticulatelyPanel:ArticulatelyPanel()
    self.data = ArticulatelyData.Instance
    self.data_cares = {
        {data = self.data.game_data, func = self.OnFlush,keys = {"state"},init_call = true},
        {data = self.data.game_data, func = self.OnFlushCallView,keys = {"val"},init_call = false},
    }
end

function ArticulatelyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ShowTimer1)
    TimeHelper:CancelTimer(self.ShowTimer2)
    UH.StopEffect(self.Effect,self.effect_handle)
    self.data:SetRainState(ArticulatelyData.ArticulatelyState.None)
    ArticulatelyData.IsDrag = false
    ArticulatelyData.IsError = false
    self.data:ChearData()
end

function ArticulatelyPanel:OnFlush()
    local state = self.data:GetRainState()
    if state == ArticulatelyData.ArticulatelyState.None then
        self:FlushItemView()
        self:FlushItemList()
        self:FlushConsumeView()
    end
    self.StartPanel:SetActive(state == ArticulatelyData.ArticulatelyState.None)
    self.GamePanel:SetActive(state == ArticulatelyData.ArticulatelyState.Open)
    self.Tips:SetActive(state == ArticulatelyData.ArticulatelyState.None)
end

function ArticulatelyPanel:OnFlushCallView()
    local state = self.data:GetRainState()
    if state == ArticulatelyData.ArticulatelyState.Open then
        local item = self.ItemList[self.ramdon_index]
        local item_list = self.data:GetItemList()
        if self.data:IsHasSort() then
            item:OnFillInItem(item_list)
            if #item_list == ArticulatelyData.ARTICULATELY_MAX_STEP_NUM then
                self.Effect:SetObjActive(true)
                self:FlushEffectView(self.ramdon_index)
                self.ShowTimer1 = Invoke(function()
                    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ARTICULATELY,1)
                    self.ItemList[self.ramdon_index]:SetObjActive(false)
                    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.EnterGame)
                end,0.5)
                self.ShowTimer2 = Invoke(function()
                    self.Effect:SetObjActive(false)
                    self.data:SetRainState(ArticulatelyData.ArticulatelyState.None)
                    ArticulatelyData.IsDrag = false
                    ArticulatelyData.IsError = false
                    if self.data:IsNextDay() then
                        ViewMgr:CloseView(ArticulatelyView)
                    end
                    self.data:ChearData()
                end,4)
            end
        else
            PublicPopupCtrl.Instance:Error(Language.Articulately.ErrorTip)
            ArticulatelyData.IsError = true
            self.data:ChearData()
            item:OnResetItem()
        end
    end
end

function ArticulatelyPanel:FlushEffectView(index)
    UH.StopEffect(self.Effect,self.effect_handle)
    self.effect_handle = UH.PlayEffect(self.Effect,tostring(6161084 + index))
end

function ArticulatelyPanel:FlushItemView()
    self.Grid:SetData(self.data:GetRewardList())
    local item_id = Config.miaobishenghua_auto.other[1].item_id 
    UH.SetText(self.Num,Format(Language.Articulately.NumTip,Item.GetNum(item_id)))
    self.Call:SetData(Item.Create({item_id = item_id}))
end

function ArticulatelyPanel:FlushItemList()
    local index = PrefsInt([[Articulately]]..RoleId())
    index = index + 1
    if index > ArticulatelyData.ARTICULATELY_MAX_TIMES then
        index = 1
    end
    for i=1,self.ItemList:Length() do
        if i == index then
            self.ItemList[i]:SetData({index = i})
        else
            self.ItemList[i]:OnResetItem()
        end
        self.ItemList[i]:SetObjActive(i == index)
    end
    PrefsInt([[Articulately]]..RoleId(),index)
    self.ramdon_index = index
end

function ArticulatelyPanel:FlushConsumeView()
    local RichText = RichTextHelper.SpriteStrAtlas("MaoBi", 50, 50, nil,"articulately")
    UH.SetText(self.Consume,Format(Language.Articulately.ConsumeTip,RichText,1))
end

function ArticulatelyPanel:OnClickEnter()
    if Item.GetNum(Config.miaobishenghua_auto.other[1].item_id) <= 0 then
        PublicPopupCtrl.Instance:Error(Language.Articulately.ItemTip)
        return
    end
    self.data:SetRainState(ArticulatelyData.ArticulatelyState.Open)
end

--===========================ArticulatelyItem===========================
ArticulatelyItem = ArticulatelyItem or DeclareMono("ArticulatelyItem", UIWidgetBaseItem)

function ArticulatelyItem:SetData(data)
    for i=1,self.ItemList:Length() do
        self.ItemList[i]:SetData({index = i})
    end
end

function ArticulatelyItem:OnFillInItem(item_list)
    for i=1,self.ItemList:Length() do
        if item_list[i] then
            self.ItemList[i]:OnFillInItem()
        end
    end
end

function ArticulatelyItem:OnResetItem()
    for i=1,self.ItemList:Length() do
        self.ItemList[i]:OnResetItem()
    end
end

--===========================ArticulatelyCell===========================
ArticulatelyCell = ArticulatelyCell or DeclareMono("ArticulatelyCell", UIWidgetBaseItem)

function ArticulatelyCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
end

function ArticulatelyCell:OnDragBegin()
    self.IsDragBegin = true
    ArticulatelyData.IsDrag = true
end

function ArticulatelyCell:OnDragEnd()
    ArticulatelyData.IsDrag = false
    ArticulatelyData.IsError = false
end

function ArticulatelyCell:OnDrag()
    if ArticulatelyData.IsError then
        return
    end
    ArticulatelyData.Instance:AddItem(self.data)
end

function ArticulatelyCell:OnClickItem()
    ArticulatelyData.Instance:AddItem(self.data)
end

function ArticulatelyCell:OnFillInItem()
    UH.Alpha(self.Image,1)
end

function ArticulatelyCell:OnResetItem()
    self.IsDragBegin = false
    UH.Alpha(self.Image,0)
end

function ArticulatelyCell:OnEnter()
    if ArticulatelyData.IsError then
        return
    end
    if ArticulatelyData.IsDragBegin then
        return
    end
    if ArticulatelyData.IsDrag then
        ArticulatelyData.Instance:AddItem(self.data)
    end
end