ActDragonBoatMainView = ActDragonBoatMainView or DeclareView("ActDragonBoatMainView","serveractivity/dragon_boat/act_dragon_boad_main_view", Mod.DragonBoat.Main)
VIEW_DECLARE_LEVEL(ActDragonBoatMainView, ViewLevel.L0)
function ActDragonBoatMainView:OnCloseClick()
    ViewMgr:CloseView(ActDragonBoatMainView)
end


ActDragonBoatMainPanel = ActDragonBoatMainPanel or DeclareMono("ActDragonBoatMainPanel", UIWFlushPanel)
function ActDragonBoatMainPanel:ActDragonBoatMainPanel()
    self.data = ActDragonBoatData.Instance
    self.data_cares = {
        {data = ActivityData.Instance:GetActivityEvent(), func = self.FlushPanel, init_call = false},
        {data = self.data.remind_data, func = self.FlushPanel, init_call = false},
    }
end

function ActDragonBoatMainPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function ActDragonBoatMainPanel:FlushPanel()
    for i = 1, self.ActList:Length() do
        local mono = self.ActList[i]
        local data = self.data:GetOpenAct(i - 1)
        mono:SetObjActive(data ~= nil)
        if data then
            mono:SetData(data)
        end
    end
end





ActDragonBoatMainActItem = ActDragonBoatMainActItem or DeclareMono("ActDragonBoatMainActItem", UIWidgetBaseItem)
function ActDragonBoatMainActItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if TableIsEmpty(data) then
        return
    end
    local act_type = data.act_type
    local act_mod = data.mod_key or 0
    local act_name = data.text--ActivityRandData.GetName(act_type)
    UH.SetText(self.Name, act_name or "")
    local is_open = ActivityData.IsOpen(act_type)
    self:FlushRedNum(is_open)

    local open_day = ActDragonBoatData.Instance:OpenDay()
    self.ReadyFlag:SetActive(not is_open and open_day < data.start_time)
    self.EndFlag:SetActive(not is_open and open_day >= data.start_time)
    self.Interactor.Interactable = is_open or (open_day < data.start_time)
    self.Effect:SetActive(is_open == true or (open_day < data.start_time))
end

function ActDragonBoatMainActItem:FlushRedNum(is_open)
    self:UnRegisterGroup()
    self.RedPoint:SetNum(0)
    if is_open and self.handle == nil then
        local mod_t = TabMod(self.data.mod_key)
        self.handle = Remind.Instance:RegisterGroup(mod_t, function()
            self.RedPoint:SetNum(Remind.Instance:GetGroupNum(mod_t) > 0 and 1 or 0)
        end, true)
    end
end

function ActDragonBoatMainActItem:OnClick()
    if not self.data.mod_key then
        return
    end

    local open_day = ActDragonBoatData.Instance:OpenDay()
    local is_open = ActivityData.IsOpen(self.data.act_type)
    if is_open then
        ViewMgr:OpenViewByKey(self.data.mod_key)
    elseif open_day > self.data.end_time then
        PublicPopupCtrl.Instance:Error(Language.Activity.NotOpenEndTip)
    else
        PublicPopupCtrl.Instance:Error(Language.Activity.NotOpenTip)
    end
    
end

function ActDragonBoatMainActItem:OnDestroy()
    self:UnRegisterGroup()
end

function ActDragonBoatMainActItem:UnRegisterGroup()
    local mod_t = TabMod(self.data.mod_key)
    Remind.Instance:UnRegisterGroup(mod_t, self.handle)
    self.handle = nil
end