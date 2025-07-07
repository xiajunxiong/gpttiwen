-- 符石重铸
FuShiResetView = FuShiResetView or DeclareView("FuShiResetView", "sheng_qi/fu_shi_reset_view")
function FuShiResetView:FuShiResetView()
    self.Board:SetData(self:GetType(),Language.ShengQi.Title3,Vector2.New(493, 680))
end


FuShiResetPanel = FuShiResetPanel or DeclareMono("FuShiResetPanel", UIWFlushPanel)
function FuShiResetPanel:FuShiResetPanel()
    self.data = FuShiData.Instance
    self.data_cares = {
        {data = self.data.fu_shi_list, func = self.FlushPanel, init_call = false},
        {data = self.data.reset_fushi, func = self.FlushPanel, init_call = false},
    }
end

function FuShiResetPanel:Awake()
    UIWFlushPanel.Awake(self)
    FuShiResetPanel.stuff_data = nil
    self:FlushPanel()
end

function FuShiResetPanel:FlushPanel()
    local fushi = self.data:GetResetFushi()
    if TableIsEmpty(fushi) then
        return
    end
    self.FuShiCell:SetData(fushi)
    FuShiResetPanel.stuff_data = FuShiResetPanel.stuff_data or Item.Create({item_id = 0})
    self.StuffCell:SetData(FuShiResetPanel.stuff_data)
    self.RemoveBtn:SetActive(FuShiResetPanel.stuff_data.item_id > 0)
    self.need_coin = self.data:GetResetNeedCoin(fushi)
    UH.SetText(self.CoinText, self.need_coin)

    local list_data = self.data:GetResetStuffList(fushi)
    if #list_data < 15 then
        for i = #list_data + 1, 15 do
            table.insert(list_data,Item.Create({item_id = 0}))
        end
    end
    self.List:SetData(list_data)
    
    local attri_list_data = fushi:AttriShowList()
    self.AttriList:SetData(attri_list_data)

    self.Inter.Interactable = FuShiResetPanel.stuff_data.item_id ~= 0
end

function FuShiResetPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetResetFushi({})
    FuShiResetPanel.stuff_data = nil
    if self.move_effect then
        self.move_effect:Destroy()
    end
end

function FuShiResetPanel:OnRemoveClick()
    self.stuff_data = nil
    self:FlushPanel()
end

function FuShiResetPanel:OnItemClick(mono)
    local data = mono:GetData()
    if data.item_id > 0 then
        FuShiResetPanel.stuff_data = data
    end
    self.effect_start_pos = UH.GetRootPosition(mono.gameObject)
    self:FlushPanel()
end

function FuShiResetPanel:OnResetClick()
    if self.effect_ing then
        return
    end
    if not FuShiResetPanel.stuff_data or FuShiResetPanel.stuff_data.item_id == 0 then
        return
    end
    if MallCtrl.IsNotCurrency(CurrencyType.CoinBind, self.need_coin) then
        if not self.move_effect then
            self.move_effect = ShengQiMoveEffect.New()
            self.effect_end_pos = UH.GetRootPosition(self.FuShiCell.gameObject)
        end

        local pos_list = {}
        pos_list[1] = self.effect_start_pos
        pos_list[2] = self.effect_end_pos

        UH.LocalPosG(self.TuoWeiEffect, self.effect_start_pos)
        self.TuoWeiEffect:SetActive(true)
        self.effect_ing = true
        self.move_effect:Play(pos_list, self.TuoWeiEffect, function ()
            self.TuoWeiEffect:SetActive(false)
            self.EffectTool:Play("3165168")
            local fushi = self.data:GetResetFushi()
            ShengQiCtrl.Instance:ResetFuShi(fushi,FuShiResetPanel.stuff_data)
            FuShiResetPanel.stuff_data = nil
            self.effect_ing = false
        end, 10)
    end
end

FuShiResetItem = FuShiResetItem or DeclareMono("FuShiResetItem", UIWidgetBaseItem)
function FuShiResetItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    self.SelImg:SetActive(data == FuShiResetPanel.stuff_data)
end


FuShiGetShow = FuShiGetShow or DeclareView("FuShiGetShow","sheng_qi/fu_shi_get_show")
VIEW_DECLARE_MASK(FuShiGetShow,ViewMask.BgBlockClose)
function FuShiGetShow:LoadCallback(param)
    local fushi = param.item
    self.Cell:SetData(fushi)
    local attri_list_data = fushi:AttriShowList()
    self.List:SetData(attri_list_data)
    UH.SetText(self.Name, fushi:QuaName())
end