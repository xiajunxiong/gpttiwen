FabaoFiller = FabaoFiller or DeclareView("FabaoFiller", "fabao_lottery/fabao_filler")

function FabaoFiller:FabaoFiller()
    self.data = FabaoLotteryData.Instance
end
function FabaoFiller:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Fabao.FillTitle)
end
function FabaoFiller:CloseCallback()
    self.data:ClearFill()
end

FabaoFillerPanel = FabaoFillerPanel or DeclareMono("FabaoFillerPanel", UIWFlushPanel)
function FabaoFillerPanel:FabaoFillerPanel()
    self.lang = Language.Fabao
    self.data = FabaoLotteryData.Instance
    self.data_cares = {
        {data = self.data:FillList(), func = self.FlushMetaril}
    }
end
function FabaoFillerPanel:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[126].desc})
end
function FabaoFillerPanel:onFlush()
    self:FlushMetaril()
end
function FabaoFillerPanel:FlushMetaril()
    local list = self.data:MaterialInfo()
    self.List:SetData(list)
    local list2 = self.data:FillItemList()
    self.List2:SetData(list2)
    self.StartInter.Interactable = self.data:IsFullFill()
    self:FlushPercent()
    self:FlushCalcTime()
end
function FabaoFillerPanel:FlushCalcTime()
    local time = self.data:CalcNeedTime()
    local t = TimeManager.FormatDHMS(time)
    local desc = string.format("%02d:%02d:%02d", t.hour, t.min ,t.s)
    self.Time:SetTime(string.format(self.lang.CalcTime, desc))
end
function FabaoFillerPanel:FlushPercent()
    --根据选择的道具计算概率
    local fill = self.data:FillList()
    local info = self.data:UpPercent(CommonItemId.BlueStone)
    UH.SetText(self.PercentTex[1], string.format(self.lang.Rate, info.probability_up * fill[1]))
    info = self.data:UpPercent(CommonItemId.PurpleStone)
    UH.SetText(self.PercentTex[2], string.format(self.lang.Rate, info.probability_up * fill[2]))
    info = self.data:UpPercent(CommonItemId.OrangeStone)
    UH.SetText(self.PercentTex[3], string.format(self.lang.Rate, info.probability_up * fill[3]))
end
function FabaoFillerPanel:OnClickItem(cell)
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ClickGeneral1)
    if self.data:IsFullFill() then
        PublicPopupCtrl.Instance:Center(self.lang.FilterMatFull)
        return
    end
    local cell_data = cell:GetData()
    if tonumber(cell_data:Num()) <= 0 then
        PublicPopupCtrl.Instance:Center(self.lang.MatUnenough)
        return
    end
    if cell_data.item_id == CommonItemId.BlueStone then
        self.data:FillList()[1] = self.data:FillList()[1] + 1
    elseif cell_data.item_id == CommonItemId.PurpleStone then
        self.data:FillList()[2] = self.data:FillList()[2] + 1
    else
        self.data:FillList()[3] = self.data:FillList()[3] + 1
    end
    table.insert(self.data:FillItemList(), Item.Init(cell_data.item_id))
end
function FabaoFillerPanel:OnClickItem2(cell)
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ClickGeneral1)
    local cell_data = cell:GetData()
    local fill = self.data:FillItemList()
    for i, v in ipairs(fill) do
        if v == cell_data then
            table.remove(fill, i)
        end
    end
    if cell_data.item_id == CommonItemId.BlueStone then
        self.data:FillList()[1] = self.data:FillList()[1] - 1
    elseif cell_data.item_id == CommonItemId.PurpleStone then
        self.data:FillList()[2] = self.data:FillList()[2] - 1
    else
        self.data:FillList()[3] = self.data:FillList()[3] - 1
    end
end
function FabaoFillerPanel:OnClickOnekey()
    if self.data:TotalNum() < 5 then
        PublicPopupCtrl.Instance:Center(self.lang.NumTip)
        return
    end
    self:AutoUseItem(3)
    self:AutoUseItem(2)
    self:AutoUseItem(1)
end
function FabaoFillerPanel:AutoUseItem(index)
    if self.data:IsFullFill() then
        return
    end
    local temp = self.List:GetItem(index)
    local item = temp:GetData()
    local num = tonumber(item:Num())
    local surp = 5 - self.data:FillNum()
    if num > 0 and surp > 0 then
        for i = 1, num do
            if i <= surp then
                self:OnClickItem(temp)
            end
        end
    end
end
function FabaoFillerPanel:OnClickStart()
    if not self.data:IsFullFill() then
        PublicPopupCtrl.Instance:Center(self.lang.FullFillHint)
        return
    end
    local fill = self.data:FillItemList()
    --选择鼎的index 0-3
    FabaoLotteryCtrl.DrawMaterial(
        self.data:StoveInfo().index-1,
        fill[1].item_id,
        fill[2].item_id,
        fill[3].item_id,
        fill[4].item_id,
        fill[5].item_id
    )
    ViewMgr:CloseView(FabaoFiller)
end
