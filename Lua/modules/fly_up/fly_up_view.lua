FlyUpView = FlyUpView or DeclareView("FlyUpView", "fly_up/fly_up_view",Mod.FlyUp.Main)

VIEW_DECLARE_LEVEL(FlyUpView,ViewLevel.L0)

function FlyUpView:FlyUpView()
    self.TopTitle:SetActive(FlyUpData.Instance:GetNextStage(true) == FlyUpStageType.High)
    UH.SetText(self.Desc, Config.language_cfg_auto.textdesc[188 + FlyUpData.Instance:GetNextStage(true)].desc)
end

function FlyUpView:OnClickClose()
    ViewMgr:CloseView(FlyUpView)
end

----------------------------FlyUpPanel----------------------------
FlyUpPanel = FlyUpPanel or DeclareMono("FlyUpPanel", UIWFlushPanel)

function FlyUpPanel:FlyUpPanel()
    self.data = FlyUpData.Instance
    self.data_cares = {
        {data = self.data.add_data, func = self.OnIndex,  init_call = false},
        {data = self.data.info_data, func = self.OnFlush,  init_call = true},
        {data = BagData.Instance.item_grid,func = self.FlushItemView,init_call = true},
    }
    self.cur_turn = 1
end

function FlyUpPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Toggle:SetActive(PrefsInt([[FlyUpAnimToggle]]) == 1)
    --抛物线1
    self.BGEffectCell:SetData({x_offset = 2,speed_time = 2,complete_func = function()
        self.BGEffect:SetActive(true)
    end})
    --抛物线2
    self.EffectCell:SetData({y_offset = 10,speed_time = 2,complete_func = function()
        self:OnFlyComplete()
    end})
end

function FlyUpPanel:OnFlush()
    self:FlushItemList()
    self:FlushGetReach()
    self:FlushDownView()
end

function FlyUpPanel:FlushItemList()
    local stage = self.data:GetNextStage(true)
    for i=1,self:GetItemList():Length() do
        self:GetItemList()[i]:SetData({index=i-1,stage=stage})
    end
end

function FlyUpPanel:FlushGetReach()
    local stage = self.data:GetNextStage(true)
    for i=1,self:GetReachItem():Length() do
        self:GetReachItem()[i]:SetData({index=i-1,stage=stage})
    end
end

function FlyUpPanel:FlushDownView()
    local stage = self.data:GetNextStage(true)
    local BtnName = Language.FlyUp.BtnUpName
    local is_finish = self.data:IsElementFinish(stage)
    UH.SetText(self.BtnName,BtnName[is_finish and 1 or 2])
    self.Down:SetActive(not is_finish)
end

--刷新消耗品
function FlyUpPanel:FlushItemView()
    local item_id,max_num = self:GetConsumeNum()
    local is_has_num = Item.GetNum(item_id) >= max_num
    self.Call:SetData(Item.Create({item_id = item_id}))
    self.Call:SetDataByItemIdWithNeed(item_id,max_num)
    self.RedPoint:SetNum(GetActive(self.Down) and is_has_num and 1 or 0)
    self.ConsumeNumID,self.IsHasConsumeNum = item_id,is_has_num
end

function FlyUpPanel:OnIndex()
    self.index = self.data:GetIndex()
    local stage = self.data:GetNextStage(true)
    self:GetItemList()[self.cur_turn]:SetActive(false)
    self:GetItemList()[self.index]:SetActive(true)
    self:GetItemList()[self.index]:PlayEffect()
    if self.data:IsElementFinish(stage) then
        self:GetItemList()[self.index]:SetActive(false)
    end
    self.cur_turn = self.index
    self:OnPlayEffect()
    self:FlushItemList()
end

function FlyUpPanel:OnClickToggle()
    if not self.InterList.Interactable then
        return
    end
    local is_active = GetActive(self.Toggle)
    self.Toggle:SetActive(not is_active)
    PrefsInt([[FlyUpAnimToggle]],is_active and 0 or 1)
end

function FlyUpPanel:OnClickEnter()
    if GetActive(self.Down) then
        if not self.IsHasConsumeNum then
            local item = Item.Create({item_id = self.ConsumeNumID})
            MainOtherCtrl.Instance:GetWayView({item = item})
            PublicPopupCtrl.Instance:Center(ErrorText[24])
            return
        end
        self:OnPlayAnimation()
    else
        if not self.data:IsFinish() then
            ViewMgr:OpenView(FlyUpPopupView)
        end
        ViewMgr:CloseView(FlyUpView)
    end
end

function FlyUpPanel:OnPlayAnimation()
    if GetActive(self.Toggle) then
        FlyUpCtrl.Instance:SendFlyUpReq(1)
        FlyUpCtrl.Instance:SendFlyUpReq(6)
    else
        self.BGEffectCell:Play()
        self.InterList.Interactable = false
        TimeHelper:CancelTimer(self.ShowTimer)
        self.ShowTimer = Invoke(function()
            FlyUpCtrl.Instance:SendFlyUpReq(1)
            FlyUpCtrl.Instance:SendFlyUpReq(6)
            self.BGEffect:SetActive(false)
        end,2)
    end
end

function FlyUpPanel:OnPlayEffect()
    self.InterList.Interactable = false
    local obj = self:GetItemList()[self.index].gameObject
    local target = self:GetReachItem()[self.index].gameObject
    local start_pos = UH.GetRootPosition(obj,self.Root.localPosition)
    local target_pos = UH.GetRootPosition(target,self.Root.localPosition)
    self.CellEffect:SetActive(false)
    UH.LocalPosG(self.CellEffect,target_pos)
    self.EffectCell:Play(start_pos,target_pos)
end

function FlyUpPanel:OnFlyComplete()
    self.CellEffect:SetActive(true)
    self.InterList.Interactable = true
    self:FlushGetReach()
    self:FlushDownView()
end

function FlyUpPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ShowTimer)
end

function FlyUpPanel:GetItemList()
    return self["ItemList"..self.data:GetNextStage(true)]
end

function FlyUpPanel:GetReachItem()
    return self["ReachItem"..self.data:GetNextStage(true)]
end

function FlyUpPanel:GetConsumeNum()
    local stage,config = self.data:GetNextStage(true),self.data:GetOtherConfig()
    return config["shilian_cost" .. stage],config["cost" .. stage]
end

----------------------------FlyUpElementCall----------------------------
FlyUpElementCall = FlyUpElementCall or DeclareMono("FlyUpElementCall", UIWidgetBaseItem)

function FlyUpElementCall:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushItemView()
end

function FlyUpElementCall:FlushItemView()
    local num = FlyUpData.Instance:GetElementNum(self.data.index,self.data.stage)
    local max_num = FlyUpData.Instance:GetMaxElementNum(self.data.stage)
    UH.SetText(self.Num,Format("%s/%s",num,max_num))
end

function FlyUpElementCall:SetActive(active)
    if active == true then
        UH.StopEffect(self.Effect,self.effect_handle1)
        self.effect_handle1 = UH.PlayEffect(self.Effect,[[3165021]])
    end
    self.Selected:SetActive(active)
end

function FlyUpElementCall:PlayEffect()
    UH.StopEffect(self.Effect,self.effect_handle2)
    self.effect_handle2 = UH.PlayEffect(self.Effect,[[3165022]])
end

function FlyUpElementCall:OnDestroy()
    UH.StopEffect(self.Effect,self.effect_handle1)
    UH.StopEffect(self.Effect,self.effect_handle2)
end

----------------------------FlyUpReachItem----------------------------
FlyUpReachItem = FlyUpReachItem or DeclareMono("FlyUpReachItem", UIWidgetBaseItem)

function FlyUpReachItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushItemView()
end

function FlyUpReachItem:FlushItemView()
    local Progress = Language.FlyUp.Progress[self.data.stage]
    local max_num = FlyUpData.Instance:GetMaxElementNum(self.data.stage)
    local num = FlyUpData.Instance:GetElementNum(self.data.index,self.data.stage)
    self:FlushAttrView(FlyUpData.Instance:GetAttrItemData(self.data.stage,self.data.index,num))
    local COLOR = num == 0 and COLORSTR.Red8 or COLORSTR.Green7
    UH.SetText(self.Progress,Format(Progress,COLOR,num,max_num))
end

function FlyUpReachItem:FlushAttrView(data)
    if not IsEmpty(data) then
        local attr_value = DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value) or data.attr_value
        local Attribute = Language.FlyUp.Attribute[data.attr_value == 0 and 2 or 1]
        UH.SetText(self.Attribute,Format(Attribute,data.attr_name,attr_value))
    else
        UH.SetText(self.Attribute,Language.FlyUp.Attribute[2])
    end
end