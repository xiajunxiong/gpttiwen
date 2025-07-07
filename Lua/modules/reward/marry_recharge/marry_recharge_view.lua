MarryRechargeView = MarryRechargeView or DeclareMono("MarryRechargeView", UIWFlushPanel)
function MarryRechargeView:MarryRechargeView()
    self.data_cares = {
        {data = MarryRechargeData.Instance.act_info, func = self.FlushView, init_call = false}
    }
end

function MarryRechargeView:OnEnable()
    --todo
end

function MarryRechargeView:Awake()
    UIWFlushPanel.Awake(self)
    self.ActTimer:SetCallBack(BindTool.Bind1(self.OnActOver, self), nil)
    MarryRechargeData.Instance:SetNewFlag()
end

function MarryRechargeView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function MarryRechargeView:FlushView()
    local show_list = MarryRechargeData.Instance:GetRewardList()
    table.sort(show_list, DataHelper.WeightSort("seq", function(data)
        if MarryRechargeData.Instance:IsFetchedReward(data.seq) then
            return 1000
        end
    end))
    self.RewardList:SetData(show_list)

    UH.SetText(self.RechargeNum, MarryRechargeData.Instance:GetRechargeNum())

    local end_timestamp = MarryRechargeData.Instance:GetActEndTimestamp()
    self.ActTimer:StampTime(end_timestamp, TimeType.Type_Time_8, Language.MarryRecharge.Txt2, Language.Activity.NotOpenTip)
end

function MarryRechargeView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(255)
end

function MarryRechargeView:OnActOver()
    RewardData.Instance.reward_list_data:Notify()
end

MarryRechargeRewardItem = MarryRechargeRewardItem or DeclareMono("MarryRechargeRewardItem", UIWidgetBaseItem)
function MarryRechargeRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.DescText, Format(Language.MarryRecharge.RechargeText, data.acc_price))
    self.ItemList:SetData(DataHelper.FormatItemList(data.reward_item))

    local is_fetched = MarryRechargeData.Instance:IsFetchedReward(data.seq)
    local can_fetch = MarryRechargeData.Instance:GetRechargeNum() >= data.acc_price
    self.Fetched:SetActive(is_fetched)
    self.BtnGet:SetActive(not is_fetched and can_fetch)
    self.NoEnough:SetActive(not is_fetched and not can_fetch)
end

function MarryRechargeRewardItem:OnClickFetch()
    MarryRechargeCtrl.Instance:SendFetchReward(self.data.seq)
end
