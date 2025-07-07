
RewardDailySignInView = RewardDailySignInView or DeclareMono("RewardDailySignInView", UIWFlushPanel)

function RewardDailySignInView:RewardDailySignInView()
	self.data = RewardCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data.daily_sign_change,func = self.onFlush}
	}
end

function RewardDailySignInView:onFlush()
	self.RewardGrid:SetData(self.data:GetRewardDailySignInList())
end

--------------------------------RewardOnlineItem---------------------------------
RewardDailySignInItem = DeclareMono("RewardDailySignInItem", UIWidgetBaseItem)

function RewardDailySignInItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local count, flag = RewardData.Instance:DailySignInCountFlag()
    local is_get = 1 == flag
    local value = data.day < count or ((data.day == count) and is_get)
    self.Cell:SetData(data.reward)
    self.Mask:SetActive(value)
    self.Gou:SetActive(value)
    self.BtnGet:SetActive(data.day == count and not is_get)
    self.Day.text = data.day
end

function RewardDailySignInItem:OnClickGet()
    if not BagData.Instance:TryFindEmptyInBag(self.data.reward.item_id) then
        PublicPopupCtrl.Instance:Center(ErrorText[99])
        return
    end
	RewardCtrl.Instance:SendDailySignInRewardReq(self.data.day)
end