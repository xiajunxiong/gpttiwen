ActPartnetExchangeView = ActPartnetExchangeView or DeclareView("ActPartnetExchangeView","serveractivity/act_partner_exchange",Mod.Splendid.PartnerExchange)
VIEW_DECLARE_LEVEL(ActPartnetExchangeView, ViewLevel.L1)
function ActPartnetExchangeView:ActPartnetExchangeView()
    self.data = ServerActivityData.Instance
end 

function ActPartnetExchangeView:LoadCallback()
    self.List:SetData(self.data:GetPartnerExchangeConfig())
    self.Board:SetData(self:GetType(),ActivityRandData.GetName(ACTIVITY_TYPE.RAND_PARTNER_EXCHANGE))
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_PARTNER_EXCHANGE,TimeType.Type_Special_4)
    self.BtnCheck:SetActive(self.data:GetPartnetExchangeSave())
end

function ActPartnetExchangeView:OnClickCheck()
    self.BtnCheck:SetActive(not self.BtnCheck.activeSelf)
    self.data:SetPartnetExchangeSave(self.BtnCheck.activeSelf)
end

ActPartnerExchangeOperateCell = ActPartnerExchangeOperateCell or DeclareMono("ActPartnerExchangeOperateCell", UIWidgetBaseItem)

function ActPartnerExchangeOperateCell:SetData(data)
    self.stuff_list = {}
    for i=1,self.CellList:Length() do
        local item_data = data["item"..i]
        if item_data.item_id then
            self.stuff_list[i] = item_data
            local item = Item.Create(self.stuff_list[i])
            self.CellList[i]:SetData(item)
        end
        self.CellList[i]:SetObjActive(item_data.item_id ~= nil)
    end
    local item = Item.Create(data.item)
    self.TargetCell:SetData(item)
    UH.SetText(self.TargetName,item:Name())
    UIWidgetBaseItem.SetData(self,data)
    self:uncareData()
    self:careData(BagData.Instance.item_grid, BindTool.Bind(self.FlushBagChange, self))
    self:careData(ServerActivityData.Instance.partnet_exchang_save, BindTool.Bind(self.FlushRedNum, self))
end

function ActPartnerExchangeOperateCell:OnRelease()
    self:uncareData()
end

function ActPartnerExchangeOperateCell:FlushBagChange()
    self:FlushExchangeInfo()
    self:FlushRedNum()
end

function ActPartnerExchangeOperateCell:FlushExchangeInfo()
    local exchange_str = self.data.buy_tepy == 1 and Language.PartnerExchange.ExchangeTipsWithNum or Language.PartnerExchange.ExchangeDailyWithNum
    self.cur_times = ServerActivityData.Instance:GetPartnerExchangeCurTimes(self.data)
    UH.SetText(self.TargetNeed,Format(exchange_str,self.cur_times))
end

function ActPartnerExchangeOperateCell:FlushRedNum()
    local flag_enough = true
    for i=1,self.CellList:Length() do
        if self.stuff_list[i] then
            local num = self.stuff_list[i].num
            local item_id = self.stuff_list[i].item_id
            if Item.GetNum(item_id) < num or self.cur_times <= 0 then
                flag_enough = false
            end
            CellFlushs.SetNum(self.CellList[i],DataHelper.ConsumeNum(Item.GetNum(item_id), num, true))
        end 
    end
    if not ServerActivityData.Instance:GetPartnetExchangeSave() then
        self.RedPiont:SetActive(flag_enough)
    else
        self.RedPiont:SetActive(false)
    end
    self.EnterInter.Interactable = flag_enough
end

function ActPartnerExchangeOperateCell:OnExChangeClick()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_PARTNER_EXCHANGE,1,self.data.seq1)
end