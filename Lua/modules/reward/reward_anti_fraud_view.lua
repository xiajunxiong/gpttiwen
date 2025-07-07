RewardAntiFraudView = RewardAntiFraudView or DeclareMono("RewardAntiFraudView", UIWFlushPanel)

function RewardAntiFraudView:RewardAntiFraudView()
	self.data = RewardCtrl.Instance:Data()
	self.language = Language.Reward.AntiFraud
	self.data_cares = {
		{data = self.data.anti_fraud_info, func = self.FlushInfo},
	}
	RichTextHelper.Bind(self.TipsShow, BindTool.Bind(RichTextItem.OnClickLink, self))
	self.data.anti_fraud_red_point.val = false
end

function RewardAntiFraudView:Awake()
	UIWFlushPanel.Awake(self)

	self:FlushRewardInfo()
	self:FlushTipsShow()
end

function RewardAntiFraudView:FlushRewardInfo()
	self.RewardList:SetData(self.data:GetRewardAntiFraudList())
end

function RewardAntiFraudView:FlushTipsShow()
	if not IS_STANDALONE() then
		self.TipsShow.text = self.language.TipsShow1 .. RichTextHelper.LinkStr(self.language.TipsShow2, "_ul", COLORSTR.Green2, "1", "antiFraud")
	else
		UH.SetText(self.TipsShow, self.language.TipsShow3)
	end
end

function RewardAntiFraudView:FlushInfo()
	local flag = self.data.anti_fraud_info.flag
	self.BtnGetInter.Interactable = 0 == flag
	self.BtnGet.text = 0 == flag and self.language.Get or self.language.Geted
	if 1 == flag then
		local answers = Config.sundries_auto.question_configuration
		for i = 1, self.Togs:Length() do
			self.Togs[i].isOn = 1 == answers[i].is_correct
			self.Togs[i].interactable = false
		end
	end
end

function RewardAntiFraudView:OnClickGet()
	local answer_list = {}
	for i = 1, self.Togs:Length() do
		if self.Togs[i].isOn then
			table.insert(answer_list, i)
		end
	end
	if 0 == #answer_list then
		PublicPopupCtrl.Instance:Center(self.language.AnswerTips)
		return
	end
	RewardCtrl.Instance:SendFangFraudRewardReq(answer_list)
end

function RewardAntiFraudView:OnTogPic(index)
end