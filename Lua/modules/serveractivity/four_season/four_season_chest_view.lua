FourSeasonChestView = FourSeasonChestView or DeclareView("FourSeasonChestView", "serveractivity/four_season/four_season_chest", Mod.PlanActivity.FourSeasonChest)
VIEW_DECLARE_LEVEL(FourSeasonChestView, ViewLevel.L1)
VIEW_DECLARE_MASK(FourSeasonChestView, ViewMask.BgBlock)
function FourSeasonChestView:FourSeasonChestView()
    self.Time:SetShowCallBack(function(t)
        return Format(Language.FourSeasonChest.Time, t.day, t.hour ,t.min)
    end)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_FOUR_SEASON_CHEST, TimeType.Type_Time_3)
end

function FourSeasonChestView:OnCloseClick()
    ViewMgr:CloseView(FourSeasonChestView)
end

----------------------------FourSeasonChestPanel----------------------------
FourSeasonChestPanel = FourSeasonChestPanel or DeclareMono("FourSeasonChestPanel", UIWFlushPanel)
function FourSeasonChestPanel:FourSeasonChestPanel()
    self.Data = FourSeasonData.Instance
    self.language = Language.FourSeasonChest
    self.data = self.Data.chest_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function FourSeasonChestPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Data:ResetChestSelectFlag()
    self:FlushAll()

    UH.SetText(self.Name,ActivityRandData.GetName(ACTIVITY_TYPE.RAND_FOUR_SEASON_CHEST))
end

function FourSeasonChestPanel:FlushAll()
    self.base_data = self.Data:ChestBaseData()
    local index = 1
    for k, v in pairs(self.base_data) do
        v.my_index = index
        index = index + 1
    end
    local remind_num = self.Data:ChestRemind()
    self.ObjFree:SetActive(remind_num > 0)
    self.Remind:SetNum(remind_num)
    self.List:SetData(self.base_data)
end

function FourSeasonChestPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function FourSeasonChestPanel:OnFreeClick()
    if not self:JudgeCommon() then
        return
    end
    if self.Data:ChestRemind() > 0 then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_FOUR_SEASON_CHEST, FourSeasonChestReqType.Free)
    end
end

function FourSeasonChestPanel:OnBuyClick(data, select_data)
    if not self:JudgeCommon() then
        return
    end
    if not self.Data:ChestIsCanBuy(data) then
        PublicPopupCtrl.Instance:Center(self.language.NotTimesTip)
        return
    end
    local fee = data.buy_money / 10
    local buy_bit = bit:d2b(0)
    local select_num = 0
    for i, v in pairs(select_data) do
        if v > 0 then
            buy_bit[32 - i] = 1
            select_num = select_num + 1
        end
    end
    if select_num < data.reward_num then
        PublicPopupCtrl.Instance:Center(self.language.SelectTip)
        return
    end
    local param2 = bit:b2d(buy_bit)
    self.now_buy_data = data
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_FOUR_SEASON_CHEST, fee, ACTIVITY_TYPE.RAND_FOUR_SEASON_CHEST, FourSeasonChestReqType.Buy, data.seq, param2)
end

function FourSeasonChestPanel:JudgeCommon()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_FOUR_SEASON_CHEST) then
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        return false
    end
    return true
end

function FourSeasonChestPanel:SendBuy()
    if TableIsEmpty(self.now_buy_data) then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_FOUR_SEASON_CHEST, self.now_buy_data.buy_money)
    self.now_buy_data = nil
end

--------------FourSeasonChestItem--------------
FourSeasonChestItem = FourSeasonChestItem or DeclareMono("FourSeasonChestItem", UIWidgetBaseItem)
function FourSeasonChestItem:FourSeasonChestItem()
    self.Data = FourSeasonData.Instance
    self.language = Language.FourSeasonChest
end

function FourSeasonChestItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ObjRoot.name = "FourSeasonChestItem" .. data.my_index

    UH.SpriteName(self.ImgBg, "SiJiBaoXiang" .. data.gift_icon)
    UH.SpriteName(self.ImgDesc, "TuiJianDi" .. data.gift_icon)
    UH.SetText(self.TxtTitle, data.gift_des)
    UH.SetText(self.TxtPrice, data.buy_money / 10)
    self.can_times, self.total_times = self.Data:ChestLimitTimes(data)
    UH.SetText(self.TxtLimit, Format(self.language.LimitType[data.limit_type], self.can_times, self.total_times))
    UH.SetText(self.TxtCanSelect, Format(self.language.CanSelect, data.reward_num))
    if self.can_times <= 0 then
        self.Data:ResetChestSelectFlag(data.item_id)
    end
    local item_list = self.Data:ChestRewardData(data.item_id)
    self.List:SetData(item_list)
    self.IHandler.Interactable = self.can_times > 0
    self:FlushDesc(data.gift_icon)
    self:FlushSelectStr()
end

function FourSeasonChestItem:FlushDesc(index)
    for i = 1, self.TxtDescs:Length() do
        UH.SetText(self.TxtDescs[i], self.data.gift_name)
        self.TxtDescs[i]:SetObjActive(index == i)
    end
end

function FourSeasonChestItem:OnSelectClick(value, mono)
    local mono_data = mono:GetData()
    local select_num = 0
    self.Data:ChestSelectFlag(mono_data.item_id, mono_data.type, value and 1 or 0)
    for k, v in pairs(self.Data:ChestSelectFlag(mono_data.item_id)) do
        if v > 0 then
            select_num = select_num + 1
        end
    end
    if select_num > self.data.reward_num then
        value = false
    end
    self:FlushSelectStr()
    mono:FlushSelect(value)
end

function FourSeasonChestItem:FlushSelectStr()
    local select_num = 0
    for k, v in pairs(self.Data:ChestSelectFlag(self.data.item_id)) do
        if v > 0 then
            select_num = select_num + 1
        end
    end
    local select_num_str = select_num >= self.data.reward_num and select_num or ColorStr(select_num, COLORSTR.Red8)
    UH.SetText(self.TxtSelect, Format(self.language.Selected, select_num_str, self.data.reward_num))
end

function FourSeasonChestItem:GetSelectData()
    return self.Data:ChestSelectFlag(self.data.item_id)
end

--------------FourSeasonChestSelectItem--------------
FourSeasonChestSelectItem = FourSeasonChestSelectItem or DeclareMono("FourSeasonChestSelectItem", UIWidgetBaseItem)
function FourSeasonChestSelectItem:FourSeasonChestSelectItem()
    self.Data = FourSeasonData.Instance
end

function FourSeasonChestSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(Item.Create(data.reward_item))
    self:FlushSelect(self.Data:ChestSelectFlag(data.item_id, data.type) == 1)
end

function FourSeasonChestSelectItem:FlushSelect(value)
    self.Toggle.isOn = value
end