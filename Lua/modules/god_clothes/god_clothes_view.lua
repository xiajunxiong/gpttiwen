GodClothesView = GodClothesView or DeclareView("GodClothesView", "god_clothes/god_clothes", Mod.RandActivity.GodClothes)
VIEW_DECLARE_LEVEL(GodClothesView, ViewLevel.L1)
VIEW_DECLARE_MASK(GodClothesView, ViewMask.BgBlock)
function GodClothesView:GodClothesView()
    self.Data = GodClothesData.Instance
    self.language = Language.GodClothes
    self.Time:SetShowCallBack(function (t)
        return Format(self.language.Time, t.day, t.hour, t.min)
    end)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_GOD_CLOTHES, TimeType.Type_Time_3)
end

function GodClothesView:LoadCallback(param_t)
    
end

function GodClothesView:CloseCallback()

end

function GodClothesView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[342].desc)
end

function GodClothesView:OnCloseClick()
    ViewMgr:CloseView(GodClothesView)
end


----------------------------GodClothesPanel----------------------------
GodClothesPanel = GodClothesPanel or DeclareMono("GodClothesPanel", UIWFlushPanel)
function GodClothesPanel:GodClothesPanel()
    self.Data = GodClothesData.Instance
    self.language = Language.GodClothes
    self.data = self.Data.info_data
    self.item_id = self.Data:LotteryItemId()
    self.is_showing = false
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.Data:LotteryItem(), BindTool.Bind(self.FlushAll, self))
    end
end

function GodClothesPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Data:ClearFirstRemind()
    UH.SetIcon(self.Icon, Item.GetIconId(self.item_id), ICON_TYPE.ITEM)
    self.now_model = 0
    self.model_show_data, self.max_model = self.Data:ModelShowCfg()
    self.ObjLeft:SetActive(self.max_model ~= 1)
    self.ObjRight:SetActive(self.max_model ~= 1)
    self.ShowList:SetData(self.Data:ExchangeShowData())
    self:FlushModel()
    self:FlushAll()
end

function GodClothesPanel:FlushAll()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_GOD_CLOTHES) then
        PublicPopupCtrl.Instance:Center(self.language.ActEnd)
        ViewMgr:CloseView(GodClothesView)
        return
    end
    if self.is_showing then
        return
    end
    self.ObjBlock:SetActive(false)
    UH.SetText(self.TxtNum, Item.GetNum(self.item_id))
    self.ExchangeRemind:SetNum(self.Data:AllExchangeRemind())
    self.SelectList:SetData(self.data.info.item_select)
    self.RandList:SetData(self.data.info.item_rand)
    self.OneRemind:SetNum(self.Data:LotteryRemind())
    self.ThreeRemind:SetNum(self.Data:LotteryRemind(3))
end

function GodClothesPanel:OnBuyClick()
    ViewMgr:OpenView(GodClothesBuyView)
end

function GodClothesPanel:OpenGetWay()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.item_id})})
end

function GodClothesPanel:OnSelectShowClick()
    ViewMgr:OpenView(GodClothesShowView, {m_type = GodClothesItemType.Select})
end

function GodClothesPanel:OnRandShowClick()
    ViewMgr:OpenView(GodClothesShowView, {m_type = GodClothesItemType.Rand})
end

function GodClothesPanel:OnExchangeShowClick()
    ViewMgr:OpenView(GodClothesExchangeView)
end

function GodClothesPanel:OnOpenSelectClick()
    ViewMgr:OpenView(GodClothesSelectView)
end

function GodClothesPanel:OnChangeClick(value)
    self.now_model = (self.now_model + value) % self.max_model
    self:FlushModel()
end

function GodClothesPanel:OnLotteryClick(times)
    if self.Data:IsSelect() then
        if self.Data:CanLotteryTimes() >= times then
            local has_num = Item.GetNum(self.item_id)
            if has_num >= times then
                GodClothesCtrl:SendReq(GodClothesReqType.Lottery, times)
            else
                PublicPopupCtrl.Instance:Center(self.language.LimitTip[8])
                ViewMgr:OpenView(GodClothesBuyView)
            end
        else
            PublicPopupCtrl.Instance:Center(self.language.LimitTip[7])
        end
    else
        PublicPopupCtrl.Instance:Center(self.language.LimitTip[6])
        ViewMgr:OpenView(GodClothesSelectView)
    end
end

function GodClothesPanel:FlushModel()
    if self.max_model > 1 then
        TimeHelper:CancelTimer(self.ht)
        self.ht = Invoke(function()
            self:OnChangeClick(1)
            self:FlushModel()
        end, 5)
    end
    self.ModelUi:SetData({item_id = self.model_show_data[self.now_model + 1]})
    UH.SetText(self.TxtName, self.model_show_data[self.now_model + 1]:Name())
end

function GodClothesPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ht)
    BagData.Instance:RemoveListenItems(self.item_listen)
end

--------------GodClothesBoxItem--------------
GodClothesBoxItem = GodClothesBoxItem or DeclareMono("GodClothesBoxItem", UIWidgetBaseItem)
function GodClothesBoxItem:GodClothesBoxItem()
    self.Data = GodClothesData.Instance
    self.language = Language.GodClothes
end

function GodClothesBoxItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:SetSelectAct(not self.Data:IsSelect())
    if self.Data:IsSelect() then
        self.Cell:SetObjActive(data.is_get == 1)
        self.ObjOpen:SetActive(data.is_get == 1)
        if data.is_get == 1 then
            local reward = self.Data:GetReward(data.group, data.seq)
            self.Cell:SetData(Item.Create(reward.reward_item))
            
        end
    else
        self.Cell:SetObjActive(false)
        self.ObjOpen:SetActive(data.item_type == GodClothesItemType.Select)
    end
end

function GodClothesBoxItem:SetSelectAct(value)
    if self.ObjSelect then
        self.ObjSelect:SetActive(value)
    end
end

function GodClothesBoxItem:OnDestroy()
    
end

--------------GodClothesShowCellItem--------------
GodClothesShowCellItem = GodClothesShowCellItem or DeclareMono("GodClothesShowCellItem", UIWidgetBaseItem)
function GodClothesShowCellItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(Item.Create(data.reward_item))
end