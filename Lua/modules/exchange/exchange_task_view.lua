
ExchangeTaskView = ExchangeTaskView or DeclareView("ExchangeTaskView","exchange/exchange_task", Mod.ExchangeTask.Main)
VIEW_DECLARE_MASK(ExchangeTaskView,ViewMask.Block)

function ExchangeTaskView:ExchangeTaskView()
    self.data = ExchangeMonoCtrl.Instance:Data()
end

function ExchangeTaskView:LoadCallback()
    self.ShowCell:SetData(self.data.medaltask_exchange_info.target)
    self.ETips.text = string.format(Language.ExchangeMono.Tips, self.data.medaltask_exchange_info.target:QuaName())
end

function ExchangeTaskView:OnClickClose()
    ViewMgr:CloseView(ExchangeTaskView)
end

function ExchangeTaskView:OnClickExchange()
    local is_enough = true
    for _, item in pairs(self.data.medaltask_exchange_info.sub_items) do
        is_enough = is_enough and (Item.GetNum(item.item_id) >= item.e_num)
    end
    BagCtrl.Instance:SendMedalReq{req_type = MEDAL_REQ_TYPE.EXCHANGE, param1 = self.data.medaltask_exchange_info.seq}
    if not is_enough then return end
    self:OnClickClose()
end

ExchangeTaskViewF = ExchangeTaskViewF or DeclareMono("ExchangeTaskViewF", UIWFlushPanel)

function ExchangeTaskViewF:ExchangeTaskViewF()
    self.data = ExchangeMonoCtrl.Instance:Data()
    self.bag_data = BagData.Instance

    self.data_cares = {
        {data = self.bag_data.item_grid, func = self.FlushItemList},
    }
end

function ExchangeTaskViewF:FlushItemList()
    for _, item in pairs(self.data.medaltask_exchange_info.sub_items) do
        local num = Item.GetNum(item.item_id)
        -- item.num = RichTextHelper.ColorStr(string.format("%s/%s", num, item.e_num), num >= item.e_num and COLORSTR.Green4 or COLORSTR.White)
        item.num = RichTextHelper.ColorStr(num, num < item.e_num and COLORSTR.Red10 or COLORSTR.Green3) .. "/" .. item.e_num
    end
    self.ItemList:SetData(self.data.medaltask_exchange_info.sub_items)
end
