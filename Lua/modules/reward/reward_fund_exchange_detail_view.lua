RewardFundExchangeDetailView = RewardFundExchangeDetailView or DeclareView("RewardFundExchangeDetailView", RewardConfig.ResPath .. "fund_exchange_detail")
function RewardFundExchangeDetailView:RewardFundExchangeDetailView()

end 

function RewardFundExchangeDetailView:RewardFundExchangeDetailView(param_t)
    self.Board:SetData(self:GetType(), Language.Reward.FundExchangeDetailTitle, Vector2.New(535, 563))
end

RewardFundExchangeDetailPanel = RewardFundExchangeDetailPanel or DeclareMono("RewardFundExchangeDetailPanel", UIWFlushPanel)
function RewardFundExchangeDetailPanel:RewardFundExchangeDetailPanel()
    self.data = RewardData.Instance

    self.click_list:SetCompleteCallBack(function()
        self.click_list:GetItem(1).ItemClick:Trigger()
    end)    
end 

function RewardFundExchangeDetailPanel:onFlush() 
    self.click_list:SetData(self.data:GetFundExchangeDetailClickList())
end 

function RewardFundExchangeDetailPanel:OnClickColumn(data)
    self.list_plus:SetData(self.data:GetFundExchangeDetailPageList(data.page))
end

RewardFundExchangeDetailCell = RewardFundExchangeDetailCell or DeclareMono("RewardFundExchangeDetailCell",UIWidgetBaseItem)
function RewardFundExchangeDetailCell:RewardFundExchangeDetailCell() end
function RewardFundExchangeDetailCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.name,Item.GetQuaName(data.item_id))
    UH.SetText(self.price,data.price)
    self.bg1:SetActive(data.index %2 == 0)
    self.bg2:SetActive(data.index %2 == 1)
end 

RewardFundExchangeDetailClick = RewardFundExchangeDetailClick or DeclareMono("RewardFundExchangeDetailClick",UIWidgetBaseItem)
function RewardFundExchangeDetailClick:RewardFundExchangeDetailClick() end
function RewardFundExchangeDetailClick:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.text,data.page_name)
end 