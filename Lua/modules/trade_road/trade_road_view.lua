TradeRoadView = TradeRoadView or DeclareView("TradeRoadView", TradeRoadConfig.ResPath .. "trade_road_view", Mod.TradeRoad.Main)
VIEW_DECLARE_LEVEL(TradeRoadView, ViewLevel.Lt) --防止Npc对话框关闭界面

function TradeRoadView:LoadCallback(param_t)
    self.Tabbar:SetData(
        {
            {name = Language.TradeRoad.Tab1, tab_objs = {self.Tab1.gameObject, self.Currency}, mod = TabMod(Mod.TradeRoad.Main), open = Mod.TradeRoad.Main},
            {name = Language.TradeRoad.Tab2, tab_objs = {self.Tab2.gameObject}, mod = TabMod(Mod.TradeRoad.Rank), open = Mod.TradeRoad.Rank},
            {name = Language.TradeRoad.Tab3, tab_objs = {self.Tab3.gameObject, self.Currency}, mod = TabMod(Mod.TradeRoad.Tavern), open = Mod.TradeRoad.Tavern}
        }
    )
    self.param_t = param_t
    self.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.TradeRoad, self.Tabbar, self.param_t.open_param or 1))

    GuideManager.Instance:RegisterGetGuideUi("TradeRoadTabTavern", function()
        return self.Tabbar:GetItem(3)
    end)
end

function TradeRoadView:OpenCallback()
    if TradeRoadData.Instance:IsJoined() then
        if TradeRoadCtrl.Instance:IsTradeGuide() then
            self.Tabbar:ClickItem(1)
        end
        self.start_timer = Invoke(BindTool.Bind1(self.StartGuide, self), 0.1)
    end
end

function TradeRoadView:StartGuide()
    local function func_guide()
        if ViewMgr:IsOpen(TradeRoadView) then
            TradeRoadCtrl.Instance:GuideStart()
        end
    end
    if not ViewMgr:IsOpen(NpcDialogView) then
        func_guide()
    else
        NpcDialogData.Instance:SetCustomEndFunc(func_guide)
    end
end

function TradeRoadView:OnClickClose()
    ViewMgr:CloseView(TradeRoadView)
end

function TradeRoadView:CloseCallback()
    GuideManager.Instance:UnRegisterGetGuideUi("TradeRoadTabTavern")
    TimeHelper:CancelTimer(self.start_timer)
    self.start_timer = nil
end

function TradeRoadView:Awake()
    if self.over_handle == nil then
        self.over_handle = TradeRoadData.Instance.over_smart:Care(BindTool.Bind1(self.OnGameOver, self))
    end
end

function TradeRoadView:OnDestroy()
    if self.over_handle ~= nil then
        TradeRoadData.Instance.over_smart:Uncare(self.over_handle)
        self.over_handle = nil 
    end
end

function TradeRoadView:OnGameOver()
    self.Tabbar:ClickItem(2)
    for i, v in pairs(self.Tabbar.item_list) do
        v:SetData(v:GetData())
    end
end

-----标签页
TradeRoadTabbarBtn = TradeRoadTabbarBtn or DeclareMono("TradeRoadTabbarBtn", UIWidgetBaseItem)
function TradeRoadTabbarBtn:SetData(data)
    self.gameObject.name = "btn_" .. (data.open or 0) % 10
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name)
    UH.SetText(self.NameUnact, data.name)

    self.TabActivator.Actives:Clear()
    if data.tab_objs ~= nil then
        for _, item in pairs(data.tab_objs) do
            self.TabActivator.Actives:Add(item)
        end
    end
    self.TabActivator.Actives:Add(self.Name.gameObject)

    if data.mod then
        if self.handle == nil then
            self.handle = RemindCtrl.Instance:Remind():RegisterGroup(data.mod, function()
                self.RedPoint:SetNum(RemindCtrl.Instance:Remind():GetGroupNum(data.mod))
            end, true)
        end
    end

    local is_open = true
    if data.open == Mod.TradeRoad.Main or data.open == Mod.TradeRoad.Tavern then
        is_open = TradeRoadData.Instance:IsJoined()
    end
    self.TipBlock.enabled = not is_open
    self.BtnInter.Interactable = is_open
    UH.SpriteName(self.BtnSp, is_open and "tabbar_btn" or "tabbar_btn_lock")
end

function TradeRoadTabbarBtn:OnClickTip()
    if TradeRoadData.Instance:JoinTimes() >= TradeRoadData.Instance:CanJoinTimes() then
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.NotOpen2)
    else
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.NotOpen)
    end
end

function TradeRoadTabbarBtn:Click()
    self.ClickEvent:Trigger()
end

function TradeRoadTabbarBtn:OnDestroy()
    Remind.Instance:UnRegisterGroup(self.data.mod, self.handle)
    UIWidgetBaseItem.OnDestroy(self)
end

-----货币条
TradeRoadCurrencyShow = TradeRoadCurrencyShow or DeclareMono("TradeRoadCurrencyShow", UIWFlushPanel)
function TradeRoadCurrencyShow:TradeRoadCurrencyShow()
    self.data_care1 = nil
    self.last_info = {}

    GuideManager.Instance:RegisterGetGuideUi("TradeRoadMoney", function()
        return self.Obj[1], function() end
    end)
    GuideManager.Instance:RegisterGetGuideUi("TradeRoadBag", function()
        return self.Obj[2], function() end
    end)
    GuideManager.Instance:RegisterGetGuideUi("TradeRoadSupply", function()
        return self.Obj[3], function() end
    end)
end

function TradeRoadCurrencyShow:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    GuideManager.Instance:UnRegisterGetGuideUi("TradeRoadMoney")
end

function TradeRoadCurrencyShow:FlushView()
    if self.last_info[1] ~= TradeRoadData.Instance:CurMoney() then
        self.EffectTool:Play(3165018, self.Obj[1])
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.GetDiamond)
    end
    self.last_info[1] = TradeRoadData.Instance:CurMoney()
    UH.SetText(self.Value[1], self.last_info[1])

    local max_size = TradeRoadData.Instance:CurBagMaxSize()
    local use_size = TradeRoadData.Instance:CurBagUseSize()
    UH.SetText(self.Value[2], use_size .. "/" .. max_size)

    self.last_info[3] = TradeRoadData.Instance:CurSupply()
    UH.SetText(self.Value[3], self.last_info[3])
end

function TradeRoadCurrencyShow:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.data_care1 = TradeRoadData.Instance.road_info:Care(BindTool.Bind1(self.FlushView, self))
    self.last_info[1] = TradeRoadData.Instance:CurMoney()
    self.last_info[2] = TradeRoadData.Instance:CurBagMaxSize()
    self.last_info[3] = TradeRoadData.Instance:CurSupply()
    self:FlushView()
end

function TradeRoadCurrencyShow:OnDisable()
    if self.data_care1 ~= nil then
        TradeRoadData.Instance.road_info:Uncare(self.data_care1)
        self.data_care1 = nil
    end
end

function TradeRoadCurrencyShow:OnClickAdd(index)
    if index == 1 then
    elseif index == 2 then --仓库
        if TradeRoadData.Instance:CurBuyBagTimes() >= TradeRoadData.Instance:MaxBuyBagTimes() then
            PublicPopupCtrl.Instance:Center(Language.TradeRoad.BuyBagError1)
            return
        end
        ViewMgr:OpenView(TradeRoadBuyBagView)
    elseif index == 3 then --补给
        if TradeRoadData.Instance:CurBuySupplyTimes() >= TradeRoadData.Instance:MaxBuySupplyTimes() then
            PublicPopupCtrl.Instance:Center(Language.TradeRoad.BuySupplyError1)
            return
        end
        ViewMgr:OpenView(TradeRoadBuySupplyView)
    end
end
