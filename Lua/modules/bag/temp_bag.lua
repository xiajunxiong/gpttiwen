TempBag = TempBag or DeclareView("TempBag", "bag/temp_bag")
function TempBag:TempBag()
    -- body
end
function TempBag:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Bag.TempBag, Vector2.New(540, 446))
end
TempBagPanel = TempBagPanel or DeclareMono("TempBagPanel", UIWFlushPanel)
function TempBagPanel:TempBagPanel()
    self.bag_data = BagData.Instance
    self.data_cares = {
        {data = self.bag_data:GetColumnGrids(ItemColumnType.Temporary), func = self.onFlush}
    }
end
function TempBagPanel:onFlush()
    local data = self.bag_data:GetColumnGrids(ItemColumnType.Temporary)
    -- LogError(table.nums(data))
    self.TempList:SetData(data)
end

function TempBagPanel:OnClickOneKey()
    local param_t = {
        oper_type = BAG_OPERA_TYPE.OPERA_TYPE_WITHDRAW_ALL,
        param_1 = ItemColumnType.Temporary
    }
    BagCtrl.Instance:SendKnapsackOperaReq(param_t)
end
