MainViewStrategyPanel = MainViewStrategyPanel or DeclareMono("MainViewStrategyPanel", UIWFlushPanel)
local strategy_list = {
    {type = 1, desc = "集火"},
    {type = 2, desc = "治疗"},
    {type = 3, desc = "复活"},
    {type = 4, desc = "控制"},
    {type = 5, desc = "破防"},
    {type = 6, desc = "打蓝"},
    {type = 7, desc = "防御"},
    {type = 8, desc = "净化"},
}
function MainViewStrategyPanel:MainViewStrategyPanel()
    self.data = BattleData.Instance
    self.data_cares = {
        {data = self.data:CustomStrategy(), func = self.FlushList},
    }
end
function MainViewStrategyPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushList()
    GuideManager.Instance:RegisterGetGuideUi("BtnJiHuo", function ()
        if self.gameObject.activeSelf and self.List:SetupCompleted() then
            return self.List:GetItem(1)
        end
        return nil,nil
    end)
end
function MainViewStrategyPanel:FlushList()
    local data = DataHelper.TableCopy(strategy_list)
    local list = self.data:CustomStrategy()
    if list:Length() > 0 then
        for index, value in ipairs(list) do
            table.insert(data, value)
        end
    end
    self.List:SetData(data)
end
function MainViewStrategyPanel:OnClickClear(type)
    self.ParentObj:SetActive(false)
    if type == 1 then
        BattleCtrl.Instance:ClearStrategy(-1, type)
        BattleData.Instance:ViewInfo().clear_all = true
    else
        BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.STRATEGY, -1)
        BattleData.Instance:ViewInfo().select_strategy = true
    end
end

function MainViewStrategyPanel:OnClickAdd()
    local list = self.data:CustomStrategy()
    if list:Length() == 8 then
        PublicPopupCtrl.Instance:Center("无法继续添加")
    else
        ViewMgr:OpenView(BattleStrategyAdd)
    end
end