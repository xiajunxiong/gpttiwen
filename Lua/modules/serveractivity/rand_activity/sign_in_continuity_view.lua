SignInContinuityView = SignInContinuityView or DeclareView("SignInContinuityView","serveractivity/sign_in_continuity",Mod.Splendid.SignInContinuity)
--Title===========Text
VIEW_DECLARE_MASK(SignInContinuityView,ViewMask.BgBlock)
VIEW_DECLARE_LEVEL(SignInContinuityView, ViewLevel.L1)

--请求活动协议类型
SignInContinuityView.REQ_OP_RTPE = {
	INFO = 0, 		--信息下发
	CTRL = 1,  		--翻牌
}
SignInContinuityView.REWARD_FLAG = {
	NOT = 0,     -- 不可领取
	CAN = 1,     -- 可以领取
	GETED = 2,  -- 已经领取    
}

function SignInContinuityView:LoadCallback(param_t)
	self.Title.text = ActivityRandData.GetName(ACTIVITY_TYPE.RAND_SIGN_IN_CONTINUITY)
	self.Panels[ ServerActivityData.Instance:GetSignInContinuityShow()]:SetActive(true)
end

function SignInContinuityView:CloseCallback()
	-- body
end

SignInContinuityPanel = SignInContinuityPanel or DeclareMono("SignInContinuityPanel",UIWFlushPanel)
function SignInContinuityPanel:SignInContinuityPanel()
	self.data = ServerActivityData.Instance
	self.data_cares = {
		{data = self.data.sign_in_continuity_data,func = self.FlushPanel}
    }
end

function SignInContinuityPanel:Awake()
	UIWFlushPanel.Awake(self)
	self.Time:CloseCountDownTime()
	self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SIGN_IN_CONTINUITY,TimeType.Type_Special_4)
end

function SignInContinuityPanel:FlushPanel()
	self.List:SetData(self.data:GetSignInContinuityDayList())
end

function SignInContinuityPanel:OnCloseClick()
	ViewMgr:CloseView(SignInContinuityView)
end

function SignInContinuityPanel:OnTipClick()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[79].desc})
end

SignInContinuityItem = SignInContinuityItem or DeclareMono("SignInContinuityItem",UIWidgetBaseItem)
function SignInContinuityItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.DayText, string.format(Language.SignInContinuity.LoginDay, data.day))
    UH.SetText(self.DayText2, string.format(Language.SignInContinuity.LoginDay, data.day))
    local flag = ServerActivityData.Instance:GetSignInContinuityRewardFlag(data.day, data.seq)
    local list = {}
    for k, v in pairs(data.reward_item) do
        local sub_list = {}
        sub_list.item_data = v
        sub_list.flag = flag
        table.insert(list, sub_list)
    end
    local pivot = self.Content.pivot
    if #list > 2 then
        pivot = Vector2.New(0.5, 0)
    else
        pivot = Vector2.New(0.5, 0.5)
    end
    self.Content.pivot = pivot
    self.RewardList:SetData(list)
    self.GetBtnObj:SetActive(flag == SignInContinuityView.REWARD_FLAG.CAN)
    self.GetedFlagObj:SetActive(flag == SignInContinuityView.REWARD_FLAG.GETED)
    self.NotCanFlagObj:SetActive(flag == SignInContinuityView.REWARD_FLAG.NOT)
    self.DayText.gameObject:SetActive(flag ~= SignInContinuityView.REWARD_FLAG.GETED)
    self.DayText2.gameObject:SetActive(flag == SignInContinuityView.REWARD_FLAG.GETED)
end


function SignInContinuityItem:OnGetClick()
	ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SIGN_IN_CONTINUITY,SignInContinuityView.REQ_OP_RTPE.CTRL,self.data.seq)
end

SignInContinuitySubItem = SignInContinuitySubItem or DeclareMono("SignInContinuitySubItem", UIWidgetBaseItem)
function SignInContinuitySubItem:SetData(data)
	local item_data = Item.New(data.item_data)
	self.ItemCell:SetData(item_data)
	self.Shade:SetActive(data.flag == SignInContinuityView.REWARD_FLAG.GETED)
end