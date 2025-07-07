FabaoCost = FabaoCost or DeclareView("FabaoCost", "fabao_lottery/fabao_cost")

function FabaoCost:FabaoCost()
    self.data = FabaoLotteryData.Instance
end
function FabaoCost:LoadCallback()
    self.Alert:SetData(
        self:GetType(),
        Vector2.New(546, 350),
        {
            ok_func = BindTool.Bind(self.OnClickOk, self),
            cancel_func = BindTool.Bind(self.OnClickCancel, self),
            show_ok = true,
            show_cancel = true,
            v2_ok = Vector2.New(105, 42),
            v2_cancel = Vector2.New(-105, 42)
        }
    )
end
function FabaoCost:OnClickOk()
    local stove_info = self.data:StoveInfo()
    if stove_info.count > 0 then
        FabaoLotteryCtrl.DrawSpeedUp(stove_info.index - 1, stove_info.count)
    end
    ViewMgr:CloseView(FabaoCost)
end
function FabaoCost:OnClickCancel()
    ViewMgr:CloseView(FabaoCost)
end
function FabaoCost:CloseCallback()
end

FabaoCostPanel = FabaoCostPanel or DeclareMono("FabaoCostPanel", UIWFlushPanel)
function FabaoCostPanel:FabaoCostPanel()
    self.lang = Language.Fabao
    self.data = FabaoLotteryData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushCostItem},
        {data = self.data:StoveInfo(), func = self.FlushSpeedTime},
    }
    self.item = nil
end
function FabaoCostPanel:FlushSpeedTime()
    local stove_info = self.data:StoveInfo()
    local draw = self.data:GetDrawInfo(stove_info.index)
    if draw then
        local speed_time = self.data:SpeedTime()
        local show_time = draw.end_time - speed_time * stove_info.count 
        show_time = show_time < 0 and 0 or show_time
        self.Time2:StampTime(show_time)
    end
end
function FabaoCostPanel:onFlush()
    -- self.item = nil
    -- local item = self:GetCostItem()
    -- if item:Num() < 1 then
    --     PublicPopupCtrl.Instance:Center(string.format(self.lang.ZeroNum, item:Name()))
    -- end
    self:FLushTime()
    self:CalcCount()
end
function FabaoCostPanel:CalcCount(num)
    local stove_info = self.data:StoveInfo()
    local can_use = self.data:CanUseSpeed()
    if num == nil then
        UH.SetText(self.UseCount, can_use)
        stove_info.count = can_use
    else
        local temp = stove_info.count
        temp = temp + num
        if temp < 1 then
            temp = 1
            PublicPopupCtrl.Instance:Center(self.lang.CostMinHint)
        end
        if temp > can_use then
            PublicPopupCtrl.Instance:Center(self.lang.CostMaxHint)
            temp = can_use
        end
        UH.SetText(self.UseCount, temp)
        stove_info.count = temp
    end
end
function FabaoCostPanel:FLushTime()
    local stove_info = self.data:StoveInfo()
    local draw = self.data:GetDrawInfo(stove_info.index)
    if draw then
        local need_time = draw.end_time - draw.speed_up_time
        self.Time1:StampTime(need_time)
        local speed_time = self.data:SpeedTime()
        local count = math.ceil((need_time - TimeHelper.GetServerTime()) / speed_time)
        -- LogDG("算出需个数", count)
        stove_info.count = count
    end
end
function FabaoCostPanel:OnClickAdd(num)
    self:CalcCount(num)
end
function FabaoCostPanel:OnClickMax()
    local stove_info = self.data:StoveInfo()
    local can_use = self.data:CanUseSpeed()
    if stove_info.count >= can_use then
        PublicPopupCtrl.Instance:Center(self.lang.CostMaxHint)
    else
        stove_info.count = can_use
        UH.SetText(self.UseCount, can_use)
    end
end
function FabaoCostPanel:GetCostItem()
    if self.item == nil then
        local item_id = self.data:SpeedItem()
        self.item = Item.Init(item_id, Item.GetNum(item_id))
    end
    return self.item
end
function FabaoCostPanel:FlushCostItem()
    local item = self:GetCostItem()
    local num = item:Num()
    self.CostItem:SetData(item)
    UH.SetText(self.CostName, item:Name())
    UH.SetText(self.CostNum, string.format(self.lang.CostName, item:Name(), num))
    -- UH.Color(self.CostNum, num == 0 and COLORS.Red8 or COLORS.White)
end
