RewardRebateView = RewardRebateView or DeclareMono("RewardRebateView", UIWFlushPanel)

function RewardRebateView:RewardRebateView()
    self.data = RechargeData.Instance
    self.data_cares = {
        {data = self.data.reward_data, func = self.FlushRewardView},
        {data = RoleData.Instance.currencys, func = self.FlushRewardView},
    }
end

function RewardRebateView:FlushRewardView()
    local list = self.data:GetFirstData()
    local max_num = list[#list].gold
    local num = self.data:GetTodayRechargeNum()
    self.Progress:SetProgress(num / max_num)
    self.Progress:SetText(Format(Language.Recharge.LimitTip,num,max_num))
    UH.SetText(self.Num,Format(Language.Recharge.GoldTip,num))
    table.sort(list,function(a,b)
		a.weight = self.data:GetRewardFatchFlag(a.seq)==1 and a.seq + 100 or a.seq
		b.weight = self.data:GetRewardFatchFlag(b.seq)==1 and b.seq + 100 or b.seq
		return a.weight < b.weight
    end)
    self.RebateList:SetData(list)
end

function RewardRebateView:OnClickRecharge()
    ViewMgr:CloseView(RewardView)
	ViewMgr:OpenViewByKey(Mod.Store.Recharge)
end

----------------------------RebateItem----------------------------
RebateItem = RebateItem or DeclareMono("RebateItem", UIWidgetBaseItem)

function RebateItem:SetData(data)
    UH.SetText(self.Num,data.gold)
    self.Grid:SetData(RechargeData.Instance:GetRewardItemList(data.reward_item))
    UIWidgetBaseItem.SetData(self, data)
    self:FlushRewardView()
end

--领取状态
function RebateItem:FlushRewardView()
    if self.data == nil then return end
    local recharge_num = RechargeData.Instance:GetTodayRechargeNum()
    local fatch_flag = RechargeData.Instance:GetRewardFatchFlag(self.data.seq)
    --是否可以领取
    if recharge_num >= self.data.gold then
        self.Effect:SetActive(fatch_flag == 0)
        self.BtnGetInter.Interactable = fatch_flag == 0
        UH.SetText(self.Text,Language.Recharge.BtnState[fatch_flag])
    end
    self.Receive:SetActive(recharge_num >= self.data.gold)
end

function RebateItem:OnClickReceive()
    RechargeCtrl.Instance:SendRechargeReq(1,self.data.seq)
end