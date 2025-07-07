WorkshopStoneSell = WorkshopStoneSell or DeclareView("WorkshopStoneSell", "workshop/workshop_sellstone")

function WorkshopStoneSell:WorkshopStoneSell()

end 

function WorkshopStoneSell:LoadCallback(param_t)
end

function WorkshopStoneSell:CloseCallback()

end

function WorkshopStoneSell:OnClose()
	ViewMgr:CloseView(WorkshopStoneSell)
end

WorkshopStoneSellPanel = WorkshopStoneSellPanel or DeclareMono("WorkshopStoneSellPanel", UIWFlushPanel)
function WorkshopStoneSellPanel:WorkshopStoneSellPanel()
    self.data = WorkshopData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushStoneList},
    }

end 

function WorkshopStoneSellPanel:FlushStoneList()
    local list = BagData.Instance:GetGridByType({ItemType.Gem})
    self.StoneList:SetData(list)

    self.FollowTip:SetActive(#list == 0)
    self:FlushBottom()
end

function WorkshopStoneSellPanel:FlushBottom()
    local _data = self.StoneList:GetSelectGroupData()

    local total_coin = 0
    for k,v in pairs(_data) do 
        local _coin = v:Num() * v:Price()
        total_coin = total_coin + _coin
    end 

    self.TotalCoin.text = total_coin
end

function WorkshopStoneSellPanel:OnClickSell()
    local _data = self.StoneList:GetSelectGroupData()

    for k,v in pairs(_data) do 
        BagCtrl.Instance:SendKnapsackOperaReq{
            oper_type = ItemInfoConfig.BagOperType.sell, 
            param_1 = v:GridColumn(), 
            param_2 = v:GridIndex(), 
            param_3 = v:Num()
        }
    end 
end

WorkshopStoneSellCell = DeclareMono("WorkshopStoneSellCell", UIWidgetBaseItem)
function WorkshopStoneSellCell:RoleCreateViewColorItem()

end

function WorkshopStoneSellCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local _item = Item.New(data)
    self.Name.text = _item:QuaName()
    self.Item:SetData(_item)
end

function WorkshopStoneSellCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end
