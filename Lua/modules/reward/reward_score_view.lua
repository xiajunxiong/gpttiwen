RewardScoreView = RewardScoreView or DeclareMono("RewardScoreView", UIWFlushPanel)

function RewardScoreView:RewardScoreView()
    self.data = RewardData.Instance
    self.data_cares = {
        {data = self.data.score_data, func = self.FlushScoreView},
    }
end

function RewardScoreView:FlushScoreView()
    local list = ServerActivityData.Instance:GetScoreData()
    table.sort(list,DataHelper.WeightSort("seq",function(data)
        if RewardData.Instance:GetScoreRewardFlag(data.seq) == 1 then
            return 1000
        end
    end))
    self.List:SetData(list)
end

----------------------------RewardScoreItem----------------------------
RewardScoreItem = RewardScoreItem or DeclareMono("RewardScoreItem", UIWidgetBaseItem)

function RewardScoreItem:SetData(data)
    local has_score = RewardData.Instance:GetScoreNum()
    self.Grid:SetData(RechargeData.Instance:GetRewardItemList(data.reward_item))
    UH.SetText(self.Score,Format(Language.Reward.Score.NeedScoreTip,data.score))
    local score = has_score >= data.score and data.score or has_score
    self.Progress:SetProgress(score / data.score)
    self.Progress:SetText(Format("%s/%s",score,data.score))
    local fatch_flag = RewardData.Instance:GetScoreRewardFlag(data.seq)
    --是否可以领取
    if has_score >= data.score then
        self.Effect:SetActive(fatch_flag == 0)
        self.BtnGetInter.Interactable = fatch_flag == 0
        UH.SetText(self.BtnGet,Language.Recharge.BtnState[fatch_flag])
    end
    self.Receive:SetActive(has_score >= data.score)     
    UIWidgetBaseItem.SetData(self, data)
end

function RewardScoreItem:OnClickReceive()
    local param_t = {
        oper_type = RewardConfig.OperType.score,
        seq = self.data.seq or 0,
    }
    RewardCtrl.Instance:SendFetchRewardReq(param_t)
end