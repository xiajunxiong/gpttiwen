HalloweenPrayView = HalloweenPrayView or DeclareView("HalloweenPrayView", "serveractivity/halloween_pray/halloween_pray_view", Mod.Act_Halloween.Pray)
VIEW_DECLARE_LEVEL(HalloweenPrayView, ViewLevel.L1)
VIEW_DECLARE_MASK(HalloweenPrayView, ViewMask.BgBlock)

function HalloweenPrayView:HalloweenPrayView()
end

function HalloweenPrayView:LoadCallback(param_t)
    AppearanceCtrl.Instance.is_shield_appear_show = true
    HalloweenPrayData.Instance:ClearFirstRemind();
    self.Panels[ HalloweenPrayData.Instance:GetShowStyle()]:SetActive(true)
end

--Panel
HalloweenPrayPanel = HalloweenPrayPanel or DeclareMono("HalloweenPrayPanel", UIWFlushPanel)

function HalloweenPrayPanel:HalloweenPrayPanel()
    self.language=Language.Halloween_Pray
    self.data = HalloweenPrayData.Instance
    self.data_cares = {
        {data = self.data.pray_data, func = self.FlushCurrNum, init_call = false,keys = {"curr_num"}},
        {data = self.data.draw_result_data, func = self.ShowDrawResult,keys = {"reward_list"}, init_call = false},
    }
    self:InitShow()
    self.is_drawing=false--是否正在抽奖
end

function HalloweenPrayPanel:InitShow()
    local pary_cfg = self.data:GetCfg()
    for k, v in pairs(pary_cfg.big_reward) do
        if self.Icons[k] then
            local path =IllusionData.GetLiHuiByItemId(v.reward_item.item_id)
            if path then
                self.Icons[k]:SetByPath(path)
            end
        end
    end
    for k, v in pairs(pary_cfg.common_reward) do
        if self.Cells[k] then
            local item=Item.Create(v.reward_item)
            item.IsGem = false
            self.Cells[k]:SetData(item)
        end
    end
    self:FlushCurrNum()
    self.ToggleSkipAni.isOn = self.data.pray_data.isSkipAni
    self.Time:SetShowCallBack(function(t)
        return Format(self.language.time, t.day, t.hour, t.min)
    end)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_HALLOWEEN_PRAY, TimeType.Type_Time_3)
    self.model_show_index=1
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj, view = self })
        self.ModelIndexList:SetData(pary_cfg.show_reward)
        self.ModelIndexList:SetectItem(self.model_show_index, true)
    end
end

function HalloweenPrayPanel:ShowModel()
    self:CloseChangePageTimer()
    self:StartChangePageTimer()
    local index = self.model_show_index
    local data = self.data:GetCfg().show_reward[index]
    UH.SetText(self.TextModelName, Item.GetName(data.item_id))
    local data = self.data:GetCfg().show_reward[self.model_show_index]
    self.ui_obj:SetData(DrawerHelper.GetDataByItem(data.vo))
    self.BtnLeft:SetActive(index ~= 1)
    self.BtnRight:SetActive(index ~= #self.data:GetCfg().show_reward)
    -- self.ui_obj:PlayAnim(ChAnimType.Show)
end

function HalloweenPrayPanel:FlushCurrNum()
    UH.SetText(self.TextCurrNum, self.data.pray_data.curr_num)
end

function HalloweenPrayPanel:StartChangePageTimer()
    if self.time_func == nil then
        self.time_func = TimeHelper:AddRunTimer(function()
            self:OnAddPage()
        end, 5, nil, false)
    end
end

-- 自动切页
function HalloweenPrayPanel:OnAddPage()
    local index = self.model_show_index
    if index == #self.data:GetCfg().show_reward  then
        index = 0
    end
    self.model_show_index=index+1
    self.ModelIndexList:SetectItem(self.model_show_index, true)
end

function HalloweenPrayPanel:CloseChangePageTimer()
    if self.time_func ~= nil then
        TimeHelper:CancelTimer(self.time_func)
        self.time_func = nil
    end
end

function HalloweenPrayPanel:OnNextModelClick(data)
    self.model_show_index = self.ModelIndexList:GetSelectedIndex(data)
    self:ShowModel()
end

function HalloweenPrayPanel:OnLeftClick()
    local model_show_index = self.model_show_index - 1 
    self.model_show_index=model_show_index>0 and model_show_index or 1
    self.ModelIndexList:SetectItem(self.model_show_index, true)
end


function HalloweenPrayPanel:OnRightClick()
    local model_show_index = self.model_show_index + 1 
    self.model_show_index=model_show_index<=#self.data:GetCfg().show_reward and model_show_index or #self.data:GetCfg().show_reward
    self.ModelIndexList:SetectItem(self.model_show_index, true)
end

function HalloweenPrayPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function HalloweenPrayPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self:CloseChangePageTimer()
    TimeHelper:CancelTimer(self.ShowTimer)
    if self.open_view_timer ~= nil then
        TimeHelper:CancelTimer(self.open_view_timer)
        self.open_view_timer = nil
    end
end


function HalloweenPrayPanel:OnClickSkip(toggle)
    self.data.pray_data.isSkipAni=toggle
end

function HalloweenPrayPanel:OnClickGetCurr()
    if ViewMgr:IsOpen(HalloweenShopView) then
        ViewMgr:CloseView(HalloweenShopView)
    end
    self.open_view_timer=TimeHelper:AddDelayFrameTimer(function()
        ViewMgr:OpenView(HalloweenShopView)
    end, 1)
end

function HalloweenPrayPanel:onClickDrawOne()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_HALLOWEEN_PRAY) then
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        return 
    end
    if self.is_drawing then
        return
    end
    if self.data.pray_data.curr_num >= 1 then
        self.is_drawing = true
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_HALLOWEEN_PRAY, HalloweenPrayCtrl.ReqType.Pray)
        PublicPopupCtrl.Instance:SetShield()
        AchieveCtrl.Instance:DeLayCourseRemind()
    else
        self:ShowCurInfo()
        PublicPopupCtrl.Instance:Center(Language.Bag.NotEnoughItem)
    end
end


function HalloweenPrayPanel:onClickDrawTen()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_HALLOWEEN_PRAY) then
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        return
    end
    if self.is_drawing then
        return
    end
    if self.data.pray_data.curr_num >= 10 then
        self.is_drawing = true
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_HALLOWEEN_PRAY, HalloweenPrayCtrl.ReqType.Pray10)
        PublicPopupCtrl.Instance:SetShield()
        AchieveCtrl.Instance:DeLayCourseRemind()
    else
        self:ShowCurInfo()
        PublicPopupCtrl.Instance:Center(Language.Bag.NotEnoughItem)
    end
end


function HalloweenPrayPanel:OnCliskModelShow()
    CellClicks.BagGridNoButton({
    GetData = function()
        return self.data:GetCfg().show_reward[self.model_show_index]
    end,
})
end

function HalloweenPrayPanel:OnClickGaiLv()
    ViewMgr:OpenView(HalloweenPrayGaiLv)
end

function HalloweenPrayPanel:OnClickRecord()
    ViewMgr:OpenView(HalloweenPrayRecord)
end

function HalloweenPrayPanel:ShowDrawResult()
    if self.data.draw_result_data.reward_list and #self.data.draw_result_data.reward_list > 0 then
        if self.data.pray_data.isSkipAni == true then
            ViewMgr:OpenView(HalloweenPrayReward, {item_list = self.data:GetRewardShowList()})
            self.is_drawing = false
        else
            -- self.Anim:Play()
            self.DrawEffect:SetActive(true)
            TimeHelper:CancelTimer(self.ShowTimer)
            self.ShowTimer = TimeHelper:AddDelayTimer(function()
                ViewMgr:OpenView(HalloweenPrayReward, {item_list = self.data:GetRewardShowList()})
                self.DrawEffect:SetActive(false)
                self.is_drawing = false
            end, 2.5)
        end
    end
end

function HalloweenPrayPanel:OnCloseClick(data)
    if not self.is_drawing then
        AppearanceCtrl.Instance.is_shield_appear_show = false
        ViewMgr:CloseView(HalloweenPrayView)
    end
end

function HalloweenPrayPanel:ShowDetail(index)
    CellClicks.BagGridNoButton({
    GetData = function()
        return self.data:GetCfg().show_reward[index]
    end,
})
end

function HalloweenPrayPanel:ShowCurInfo()
    ItemInfoCtrl.Instance:ItemInfoView({item_list = {Item.Init(self.data:ItemId(), 1, 0)}})
end

--Item
HalloweenPrayModelItem = HalloweenPrayModelItem or DeclareMono("HalloweenPrayModelItem", UIWidgetBaseItem)

function HalloweenPrayModelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
end
