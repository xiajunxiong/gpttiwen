VitalityFlopExchange = VitalityFlopExchange or DeclareView("VitalityFlopExchange", "vitality_flop/vitality_flop_exchange")

VitalityFlopExchangePanel = VitalityFlopExchangePanel or DeclareMono("VitalityFlopExchangePanel", UIWFlushPanel)
function VitalityFlopExchangePanel:VitalityFlopExchangePanel()
    self.data = VitalityFlopData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.ItemChange, init_call = false},
        {data = self.data:Info(), func = self.FlushGouShow}
    }
end
function VitalityFlopExchangePanel:FlushGouShow()
    self.Gou:SetActive(self.data:Info().exchange)
end
function VitalityFlopExchangePanel:ItemChange()
    self:FlushExchange()
end

function VitalityFlopExchangePanel:onFlush()
    self:FlushExchange()
end

function VitalityFlopExchangePanel:FlushExchange()
    self.ExchangeList:SetData(self.data:ExchangeList())
end
function VitalityFlopExchangePanel:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(VitalityFlopExchange)
end
function VitalityFlopExchangePanel:OnClickCloseTip()
    self.data:Info().exchange = not self.data:Info().exchange
end
VitalityFlopExchangeItem = VitalityFlopExchangeItem or DeclareMono("VitalityFlopExchangeItem", UIWidgetBaseItem)
function VitalityFlopExchangeItem:VitalityFlopExchangeItem()
    self.flop_data = VitalityFlopData.Instance
end
function VitalityFlopExchangeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item_info
    local item
    for i = 1, self.CellList:Length() do
        item_info = nil
        item_info = data["item"..i]
        if item_info and next(item_info) then
            item = Item.Create(DataHelper.TableCopy(item_info))
            item.num = string.format("%s/%s", Item.GetNum(item.item_id), item.num)
            self.CellList[i]:SetData(item)
            self.Items[i]:SetActive(true)
        else
            self.Items[i]:SetActive(false)
        end
    end
    self.Cell:SetData(Item.Create(data.reward_item))
    self.RedPoint:SetNum(self.flop_data:CalcCount(data) and 1 or 0)
end
function VitalityFlopExchangeItem:OnClickExchange()
    VitalityFlopCtrl.Exchange(self.data.seq)
end