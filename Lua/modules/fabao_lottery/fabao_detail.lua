FabaoDetail = FabaoDetail or DeclareView("FabaoDetail", "fabao_lottery/fabao_detail")
function FabaoDetail:FabaoDetail()
end
function FabaoDetail:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Fabao.LotteryDetail)
end
FabaoDetailPanel = FabaoDetailPanel or DeclareMono("FabaoDetailPanel", UIWFlushPanel)
function FabaoDetailPanel:FabaoDetailPanel()
    self.lang = Language.Fabao
    self.data = FabaoLotteryData.Instance
    self.data_cares = {
        {data = self.data:DrawInfo(), func = self.FlushMetaril}
    }
end
function FabaoDetailPanel:onFlush()
    -- body
end
function FabaoDetailPanel:FlushMetaril()
    self.List:SetData(self.data:DrawInfo())
end
FabaoDetailCastingItem = FabaoDetailCastingItem or DeclareMono("FabaoDetailCastingItem", UIWidgetBaseItem)
function FabaoDetailCastingItem:FabaoDetailCastingItem()
    self.fabao_data = FabaoLotteryData.Instance
end
function FabaoDetailCastingItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_lock, lock_layer = self.fabao_data:IsLock(data.index)
    self.Lock:SetActive(is_lock)
    if is_lock then
        self.Normal:SetActive(false)
        self.Casting:SetActive(false)
        UH.SetText(self.LockTip, string.format(Language.Fabao.LockTip, lock_layer))
    else
        if data.material_num > 0 then
            local temp = {}
            for i, v in ipairs(data.material_item_id) do
                table.insert(temp, Item.Init(v))
            end
            self.List:SetData(temp)
            if self.fabao_data:IsFinish(data.index) then
                UH.SetText(self.BtnDesc, Language.Fabao.OpenStove)
            else
                UH.SetText(self.BtnDesc, Language.Fabao.AddSpeed)
            end
            self.Time:CloseCountDownTime()
            self.Time:StampTime(
                data.end_time - data.speed_up_time,
                nil,
                Language.Fabao.TimeDesc,
                Language.Fabao.FinishTime
            )
            self.Casting:SetActive(true)
            self.Normal:SetActive(false)
        else
            self.Casting:SetActive(false)
            self.Normal:SetActive(true)
        end
    end
end
function FabaoDetailCastingItem:OnClickAddSpeed()
    -- if self.fabao_data:IsFinish(self.data.index) then
    --     ViewMgr:CloseView(FabaoDetail)
    --     FabaoLotteryData.Instance:SetIndex(self.data.index)
    --     FabaoLotteryData.Instance:EffectInfo().play = not FabaoLotteryData.Instance:EffectInfo().play
    --     -- FabaoLotteryCtrl.Draw(self.data.index - 1)
    -- else
    --     self.fabao_data:SetIndex(self.data.index)
    --     ViewMgr:OpenView(FabaoCost)
    -- end

    self.fabao_data:TriggerVisualStoveClick(self.data.index)
    if self.fabao_data:IsFinish(self.data.index) then
        ViewMgr:CloseView(FabaoDetail)
    end
end
