BattleStrategy = BattleStrategy or DeclareView("BattleStrategy","battle/battle_strategy")
VIEW_DECLARE_MASK(BattleStrategy, ViewMask.Block)
function BattleStrategy:BattleStrategy()
    
end
function BattleStrategy:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Main.Battle.StrategyTitle, Vector2.New(378,537))
end
function BattleStrategy:CloseCallback()
    BattleData.Instance.strategy_data.del = false
end
BattleStrategyPanel = BattleStrategyPanel or DeclareMono("BattleStrategyPanel", UIWFlushPanel)
function BattleStrategyPanel:BattleStrategyPanel()
    self.data = BattleData.Instance
    self.data_cares = {
        {data = self.data:CustomStrategy(), func = self.FlushList},
        -- {data = self.data.strategy_data, func = self.BtnAddShow, keys = {"del"}},
        {data = self.data.strategy_data, func = self.AddBtnShow, keys = {"add_show"}},
        {data = self.data.strategy_data, func = self.FlushList, keys = {"del"}},
    }
end
function BattleStrategyPanel:onFlush()
    
end
function BattleStrategyPanel:AddBtnShow()
    self.BtnAdd:SetActive(self.data.strategy_data.add_show)
    self.BtnAdd.transform:SetAsLastSibling()
end
--[[ function BattleStrategyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_delay)
end ]]

function BattleStrategyPanel:FlushList()
    if table.nums(self.data.strategy_data.del_list) > 0 then
        self.data.strategy_data.del_list = {}
        return
    end
    self.data.strategy_data.del_list = {}
    local data = self.data:CustomStrategy()
    local del = self.data.strategy_data.del
    self.List:SetCompleteCallBack(BindTool.Bind(self.BtnAddShow, self))
    self.List:SetData(data)
    self.BtnDel:SetActive(not del)
    self.BtnCancel:SetActive(del)
    self.BtnSave:SetActive(del)
end
function BattleStrategyPanel:BtnAddShow()
    local data = self.data:CustomStrategy()
    local del = self.data.strategy_data.del
    if del == false and data:Length() < 8 then
        self.BtnAdd.transform:SetAsLastSibling()
        self.BtnAdd:SetActive(true)
        BattleData.Instance.strategy_data.add_show = true
    else
        self.BtnAdd:SetActive(false)
        BattleData.Instance.strategy_data.add_show = false
    end
end
function BattleStrategyPanel:OnClickDel()
    if BattleData.Instance:IsServerFighting() then
        PublicPopupCtrl.Instance:Center(Language.Main.Battle.StrategyError)
        return
    end
    self.data.strategy_data.del = true
    -- self:FlushList()
end
function BattleStrategyPanel:OnClickCancel()
    self.data.strategy_data.del = false
    self:FlushList()
end
function BattleStrategyPanel:OnClickSave()
    self.data.strategy_data.del = false
    --获取要删除的列表然后删除
    local list = BattleData.Instance.strategy_data.del_list
    local data = self.data:CustomStrategy()
    --[[ if data:Length() - table.nums(list) < 8 then
        BattleData.Instance.strategy_data.add_show = true
    end ]]
    -- LogDG(table.nums(list), list)
    BattleCtrl.CSCustomInstructionBatchRemove(table.nums(list), list)
    -- self:FlushList()
end
ZhiLingItem = ZhiLingItem or DeclareMono("ZhiLingItem", UIWidgetBaseItem)
function ZhiLingItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    -- UH.SpriteName(self.ImgSp, "AnNiu3")
    UH.SetText(self.Desc, data.desc)
    if self.NoDel then
        if data.type == 99 then
            UH.Color(self.Desc, COLORS.Yellow7)
        else
            UH.Color(self.Desc, COLORS.Yellow1)
        end
    end
    --[[ if self.handle_care then
        BattleData.Instance.strategy_data:Uncare(self.handle_care)
    end
    self.handle_care = BattleData.Instance.strategy_data:Care(BindTool.Bind(self.Show, self), "del") ]]
    self:Show()
end
function ZhiLingItem:Show()
    local del = BattleData.Instance.strategy_data.del
    if not self.NoDel then
        self.BtnDel:SetActive(del)
    end
end
function ZhiLingItem:OnClickDel()
    --把自己隐藏掉
    self.gameObject:SetActive(false)
    table.insert(BattleData.Instance.strategy_data.del_list, self.data.index - 1)
    -- BattleCtrl.CSCustomInstructionRemove(self.data.index)
end
function ZhiLingItem:OnClickAdd()
    local data = BattleData.Instance:CustomStrategy()
    if data:Length() >= 7 then
        BattleData.Instance.strategy_data.add_show = false
    end
    ViewMgr:OpenView(BattleStrategyAdd)
end
function ZhiLingItem:OnClickStrategy()
    self.ParentMono.ParentObj:SetActive(false)
    if self.data.type == 99 then
        BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.STRATEGY, self.data.type, self.data.index)
    else
        BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.STRATEGY, self.data.type)
    end
    BattleData.Instance:ViewInfo().select_strategy = true
end
function ZhiLingItem:OnDestroy()
    --[[ if self.handle_care then
        BattleData.Instance.strategy_data:Uncare(self.handle_care)
    end ]]
end
function ZhiLingItem:OnClickRevise()
    local del = BattleData.Instance.strategy_data.del
    if del then
        return
    end
    if BattleData.Instance:IsServerFighting() then
        PublicPopupCtrl.Instance:Center(Language.Main.Battle.StrategyError)
        return
    end
    ViewMgr:OpenView(BattleStrategyAdd, self.data)
end
BattleStrategyAdd = BattleStrategyAdd  or DeclareView("BattleStrategyAdd","battle/battle_strategy_add")
VIEW_DECLARE_MASK(BattleStrategyAdd, ViewMask.Block)
function BattleStrategyAdd:BattleStrategyAdd()
    
end
function BattleStrategyAdd:LoadCallback(data)
    if next(data) == nil then
        UH.SetText(self.Title, Language.Main.Battle.BattleStrategyTitle[1])
        self.type = 0
    else
        self.data = data
        self.type = 1
        UH.SetText(self.Title, Language.Main.Battle.BattleStrategyTitle[2])
        self.Input.text = data.desc
    end
end
function BattleStrategyAdd:OnClickCancel()
    local data = BattleData.Instance:CustomStrategy()
    if data:Length() < 8 then
        BattleData.Instance.strategy_data.add_show = true
    end
    ViewMgr:CloseView(BattleStrategyAdd)
end
function BattleStrategyAdd:OnClickOK()
    if ChatFilter.Instance:IsIllegal(self.Input.text) then
        PublicPopupCtrl.Instance:Center(ErrorText[66])
        return
    end
    if StringIsEmpty(self.Input.text) then
        PublicPopupCtrl.Instance:Center(ErrorText[184])
        return
    end
    if DataHelper.GetStringWordNum(self.Input.text) > 4 then
        PublicPopupCtrl.Instance:Center(ErrorText[185])
        return
    end
    if self.type == 0 then
        BattleCtrl.CSCustomInstructionAdd(self.Input.text)
    else
        BattleCtrl.CSCustomInstructionRemove(self.data.index)
        BattleCtrl.CSCustomInstructionAdd(self.Input.text)
    end
    ViewMgr:CloseView(BattleStrategyAdd)
end