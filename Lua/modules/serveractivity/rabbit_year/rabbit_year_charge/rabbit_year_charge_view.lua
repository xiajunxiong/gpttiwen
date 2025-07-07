RabbitYearChargeView = RabbitYearChargeView or DeclareView("RabbitYearChargeView", "serveractivity/rabbit_year/rabbit_year_charge", Mod.RabbitYear.HongYunLeiChong)
VIEW_DECLARE_LEVEL(RabbitYearChargeView, ViewLevel.Lt)
VIEW_DECLARE_MASK(RabbitYearChargeView, ViewMask.BgBlock)
function RabbitYearChargeView:RabbitYearChargeView()
end

function RabbitYearChargeView:LoadCallback(param_t)
    ActHalloweenData.Instance:ClearLeiChongFirstRemind()
end

function RabbitYearChargeView:OnCloseClick()
    ViewMgr:CloseView(RabbitYearChargeView)
end


--Panel 
RabbitYearChargePanel = RabbitYearChargePanel or DeclareMono("RabbitYearChargePanel", UIWFlushPanel)

function RabbitYearChargePanel:RabbitYearChargePanel()
    self.language=Language.Halloween_Lei_Chong
    self.data = ActHalloweenData.Instance
    self.data_cares = {
        {data = self.data.lei_chong_data, func = self.OnFlush, init_call = false }
    }
    self:OnFlush()
end

function RabbitYearChargePanel:OnFlush()
    self.List:SetData(self.data:GetLeiChongCfg())
    UH.SetText(self.ChargeNum,self.language.ChargeMoney..ColorStr(self.data.lei_chong_data.chong_zhi_value..self.language.yuan,COLORSTR.Yellow17))
end

function RabbitYearChargePanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_WAN_SHENG_LEI_CHONG, TimeType.Type_Special_9, Language.RabbitYearShop.ActTime)
end

--Item
RabbitYearChargeItem = RabbitYearChargeItem or DeclareMono("RabbitYearChargeItem", UIWidgetBaseItem)

function RabbitYearChargeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local num = data.acc_recharge_down / 10
    local charge_num = ActHalloweenData.Instance.lei_chong_data.chong_zhi_value
    local fecth_flag = ActHalloweenData.Instance.lei_chong_data.fetch_flag[data.seq]
    UH.SetText(self.TxtNum, num .. Language.Halloween_Lei_Chong.yuan)
    local rewards = {}
    local is_ke_ling_qu = charge_num >= num and fecth_flag == 0
    if data.gold > 0 then
        table.insert(rewards,{item=Item.Init(HuoBi[0],data.gold,1),is_ke_ling_qu=is_ke_ling_qu})
    end
    for k, v in pairs(data.reward_item) do
        table.insert(rewards, {item=Item.Create(v),is_ke_ling_qu=is_ke_ling_qu})
    end
    self.List:SetData(rewards)
    local value = charge_num / num
    -- self.Pro:SetProgress(value)
    -- self.Pro:SetText(charge_num .. "/" .. num)
    -- self.Block:SetActive(is_ke_ling_qu)
    -- self.ObjFetched:SetActive(fecth_flag == 1)
    self.BtnGet:SetActive(is_ke_ling_qu)
    self.BtnCharge:SetActive(fecth_flag == 0 and charge_num < num )
    self.Got:SetActive(fecth_flag == 1)
    self.Red:SetNum(1)
end

function RabbitYearChargeItem:OnClickGoCharge()
    ViewMgr:OpenViewByKey(Mod.Store.Recharge)
end

function RabbitYearChargeItem:OnClickGet()
    if not BagData.Instance:TryFindEmptyForList(ItemColumnType.Item, self.data.reward_item) then
        PublicPopupCtrl.Instance:Center(Language.Activity.BagFullTip)
        return
    end
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_WAN_SHENG_LEI_CHONG, WAN_SHENG_LEI_CHONG_REQ_TYPE.FETCH, self.data.seq)
end

RabbitYearChargeItemRewardItem = RabbitYearChargeItemRewardItem or DeclareMono("RabbitYearChargeItemRewardItem", UIWidgetBaseItem)
function RabbitYearChargeItemRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local call = Item.Create({item_id = data.item.item_id})
    self.Cell:SetData(call)
    UH.SetText(self.Num, data.item.num)
    -- if data.is_ke_ling_qu then
    --     if self.EffectHandle == nil then
    --         self.EffectHandle = UH.PlayEffect(self.UiEffect, 3164016)
    --     end
    -- else
    --     if self.EffectHandle ~= nil then
    --         UH.StopEffect(self.UiEffect, self.EffectHandle)
    --     end
    -- end
end

function RabbitYearChargeItemRewardItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    -- if self.EffectHandle ~= nil then
    --     UH.StopEffect(self.UiEffect, self.EffectHandle)
    -- end
end


