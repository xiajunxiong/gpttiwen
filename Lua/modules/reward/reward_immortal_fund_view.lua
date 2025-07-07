RewardImmortalFundView = RewardImmortalFundView or DeclareMono("RewardImmortalFundView", UIWFlushPanel)

function RewardImmortalFundView:RewardImmortalFundView()
    self.Data = RewardData.Instance
    self.data = self.Data.immortal_fund_data
    self.other = Config.immortal_coin_fund_auto.other[1]
    self.data_cares = {
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
        {data = RoleData.Instance.base_data, func = self.FlushList, keys = {"level"}, init_call = false},
        {data = self.Data.immortal_fund_data, func = self.FlushList, init_call = false},
    }
    self:FlushList()
end

function RewardImmortalFundView:Awake()
    UIWFlushPanel.Awake(self)
    self.Data:SetImmortalFundPref()
end

function RewardImmortalFundView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.Data:SetImmortalFundPref()
end

function RewardImmortalFundView:FlushList()
    local phase = self.data.phase
    if self.Data:GetImmortalFundIsFecthAll() == 1 then
        phase = phase + 1
    end
    phase = (phase == self.other.phase) and (self.other.phase - 1) or phase
    self.config = self.Data:GetImmortalFundCoinfig(phase)
    self.phase_config = self.Data:GetImmortalFundPhaseCoinfig(phase)
    self.ItemList:SetData(self.config)
    -- 最后一个阶段就算领完了也不显示购买按钮了
    if self.data.phase < (self.other.phase - 1) then
        self.ObjBtnBuy:SetActive(self.Data:GetImmortalFundIsFecthAll() == 1)
    else
        self.ObjBtnBuy:SetActive(false)
    end
    UH.SetText(self.TxtPrice, self.phase_config.buy_money / 10)
    UH.SetText(self.TxtCumulation, self.phase_config.acc_total)
    UH.SetText(self.TxtMagnification, Format(Language.Reward.ImmortalFund.Magnification, self.phase_config.reward_multiple))
    UH.SetText(self.TxtGive, self.phase_config.reward_buy)
    self.SpriteText.text = Format(Language.Reward.ImmortalFund.Magnification, self.phase_config.reward_multiple)
end

function RewardImmortalFundView:OnBuyClick()
    if self.data.phase < (self.other.phase - 1) then
        local fee = self.phase_config.buy_money / 10
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_IMMORTAL_FUND_BUY,fee,ACTIVITY_TYPE.RAND_IMMORTAL_FUND, 1, self.data.phase + 1, 0, 0)
    end
end

function RewardImmortalFundView:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_IMMORTAL_FUND_BUY, self.phase_config.buy_money)
end


----------------------------ImmortalFundItem----------------------------
ImmortalFundItem = ImmortalFundItem or DeclareMono("ImmortalFundItem", UIWidgetBaseItem)
function ImmortalFundItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.is_buy = RewardData.Instance.immortal_fund_data.phase == data.phase
    self.Cell:SetData(Item.Create({item_id = CommonItemId.ImmortalCoin, is_bind = 1, num = data.reward_immortal_coin}))
    local now_level = RoleData.Instance:GetRoleLevel()
    self.is_level_enough = now_level >= data.reward_level
    self.Progress:SetProgress(now_level / data.reward_level)
    self.Progress:SetText(Format(Language.Common.Slash, self.is_level_enough and data.reward_level or now_level, data.reward_level))
    UH.SetText(self.TxtLevel, data.reward_level)
    self.ObjEffect:SetActive(self.is_buy and self.is_level_enough and RewardData.Instance:GetImmortalFundIsFecth(self.data.seq) == 0)
    self.ObjReceived:SetActive(self.is_buy and RewardData.Instance:GetImmortalFundIsFecth(self.data.seq) == 1)
end

function ImmortalFundItem:OnReceiveClick()
    if self.is_buy then
        if RewardData.Instance:GetImmortalFundIsFecth(self.data.seq) == 0 then
            if self.is_level_enough then
                ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_IMMORTAL_FUND, 2, self.data.type, 0, 0)
            else
                PublicPopupCtrl.Instance:Center(Language.Reward.ImmortalFund.NotLevelTip)
            end
        else
            PublicPopupCtrl.Instance:Center(Language.Reward.ImmortalFund.ReceivedTip)
        end
    else
        PublicPopupCtrl.Instance:Center(Language.Reward.ImmortalFund.NotBuyTip)
    end
end