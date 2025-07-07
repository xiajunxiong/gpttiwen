SpringClimbView = SpringClimbView or DeclareView("SpringClimbView", "serveractivity/labour_day/spring_climb", Mod.LabourDay.SpringClimb)
VIEW_DECLARE_LEVEL(SpringClimbView, ViewLevel.L1)
VIEW_DECLARE_MASK(SpringClimbView, ViewMask.BgBlock)

function SpringClimbView:SpringClimbView()
    self.Currency:SetCurrency(CurrencyType.JadePendant)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SPRING_CLIMB, TimeType.Type_Special_4)
    ServerActivityData.Instance:SetSpringClimbPref()
end

function SpringClimbView:OnCloseClick()
    ViewMgr:CloseView(SpringClimbView)
end

function SpringClimbView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[109].desc)
end


--------------SpringClimbPanel--------------
SpringClimbPanel = SpringClimbPanel or DeclareMono("SpringClimbPanel", UIWFlushPanel)
function SpringClimbPanel:SpringClimbPanel()
    self.other = Config.may_day_activity_auto.other[1]
    self.data = ServerActivityData.Instance.spring_climb_data
    self.is_first = true
    self.item_data = ServerActivityData.Instance:GetSpringClimbData()

    for i = 1, self.List:Length() do
        if self.item_data[i] then
            self.List[i]:SetData(self.item_data[i])
        end
    end
    TimeHelper:AddDelayFrameTimer(function()
        UH.LocalPosG(self.ObjPerson, self:GetLayerPersonPos())
    end)

    self.data_cares = {
        {data = self.data, func = self.FlushShow, key = {"info"}, init_call = true},
    }
end

function SpringClimbPanel:FlushShow()
    UH.SetText(self.TxtExplore, Format(Language.SpringClimb.ExploreTime, self.data.info.explore_times))
    if not self.is_first then
        -- 基础层变更提示
        if self.other.explore_1 == self.data.info.explore_total_times then
            local str = Format(Language.SpringClimb.DownLayerChange, self.data.info.explore_total_times, DataHelper.GetDaXie(self.other.initial + 1))
            PublicPopupCtrl.Instance:Center(str)
        end
        -- 达到第七层并且道具未领取完提示
        if self.data.info.layer == 6 then
            if not self:JudgeIsAllGet() then
                PublicPopupCtrl.Instance:Center(Language.SpringClimb.ToTopTip)
            end
        end
        Runner.Instance:RemoveRunUntilTrue(self.time_handler)
        -- 设置目标移动位置
        self.time_handler = UH.MoveToTarget(self.ObjPerson, self:GetLayerPersonPos(), 20)
    end
    self.is_first = false
end

-- 获得当前层的小人位置
function SpringClimbPanel:GetLayerPersonPos()
    if not self.List[self.data.info.layer + 1] then
        LogError("数据异常，层数不能为"..self.data.info.layer + 1)
        return Vector3.zero
    end
    return UH.GetRootPosition(self.List[self.data.info.layer + 1].ObjPerson)
end

function SpringClimbPanel:OnExploreClick()
    if Item.GetNum(self.other.item_id_1) < self.other.item_num_1 then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.JadePendant]})})
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SPRING_CLIMB, 1)
end

-- 判断第七层的道具是否全部领取
function SpringClimbPanel:JudgeIsAllGet()
    for i = 1, #self.item_data[7] do
        if self.data.get_item_flags[i] == 0 then
            return false
        end
    end
    return true
end

function SpringClimbPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Runner.Instance:RemoveRunUntilTrue(self.time_handler)
end

--------------SpringClimbList--------------
SpringClimbList = SpringClimbList or DeclareMono("SpringClimbList", UIWidgetBaseItem)
function SpringClimbList:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.List:SetData(self:GetItemListData())
end

function SpringClimbList:GetItemListData()
    local tab = {}
    for i, v in ipairs(self.data) do
        if v.reward_item.item_id then
            local item = {}
            item.seq = v.seq
            item.layer = v.layer
            item.reward = v.reward_item
            table.insert(tab, item)
        end
    end
    return tab
end


--------------SpringClimbItem--------------
SpringClimbItem = SpringClimbItem or DeclareMono("SpringClimbItem", UIWidgetBaseItem)
function SpringClimbItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    -- 第七层的道具只能领取一次
    if data.layer == 6 then
        self.is_care = true
        self:careData(ServerActivityData.Instance.spring_climb_data, BindTool.Bind(self.FlushItemShow, self), "get_item_flags")
    else
        self.Cell:SetData(Item.New(data.reward))
    end
end

function SpringClimbItem:FlushItemShow()
    local item_data = Item.New(self.data.reward)
    if ServerActivityData.Instance.spring_climb_data.get_item_flags[self.data.seq + 1] then
        item_data.is_grey = ServerActivityData.Instance.spring_climb_data.get_item_flags[self.data.seq + 1] == 1
        self.ObjReceived:SetActive(ServerActivityData.Instance.spring_climb_data.get_item_flags[self.data.seq + 1] == 1)
    end
    self.Cell:SetData(item_data)
end

function SpringClimbItem:OnDestroy()
    if self.is_care then
        self:uncareData()
    end
end