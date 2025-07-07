GodTokenRewardInfoView = GodTokenRewardInfoView or DeclareView("GodTokenRewardInfoView", "god_token/god_token_reward_info")

VIEW_DECLARE_MASK(GodTokenRewardInfoView,ViewMask.BgBlock)

function GodTokenRewardInfoView:GodTokenRewardInfoView()
	--self.Board:SetData(self:GetType(),Language.GodToken.RewardInfoTitle, Vector2.New(888, 644))
end

function GodTokenRewardInfoView:LoadCallback()
	self.Title.text = Language.GodToken.RewardInfoTitle
end

function GodTokenRewardInfoView:CloseCallback()
	-- body
end

function GodTokenRewardInfoView:OnCloseClick()
	ViewMgr:CloseView(GodTokenRewardInfoView)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

GodTokenRewardInfoPanel = GodTokenRewardInfoPanel or DeclareMono("GodTokenRewardInfoPanel",UIWFlushPanel)
function GodTokenRewardInfoPanel:GodTokenRewardInfoPanel()
	self.data = GodTokenData.Instance
	self.data_cares = {
		{data = self.data.god_token_data, func = self.FlushBtnText, init_call = false},
    }
end

function GodTokenRewardInfoPanel:Awake()
	UIWFlushPanel.Awake(self)
	local ordinary_reward_data = self.data:GetRewardShowList(1,true)
	local senior_reward_data = self.data:GetRewardShowList(2,true)
	self.OrdinaryRewardList:SetData(ordinary_reward_data)
	self.SeniorRewardList:SetData(senior_reward_data)
	self:FlushBtnText()
	-- if self.data:IsBuyToken() then
	-- 	-- if self.data:GetCanMaxLevel() <= 0 then
	-- 	-- 	self.UpBtn:SetActive(false)
	-- 	-- end
	-- end
end

function GodTokenRewardInfoPanel:FlushBtnText()
	self.UpBtnInter.Interactable = not self.data:IsBuyToken()
	UH.SetText(self.BtnText,Language.GodToken.UpGodToken .. RechargeData.Instance:GetLocalSign() .. self.data:GetGodTokenPrice()/10)
	-- if self.data:IsBuyToken() == false then
	-- 	UH.SetText(self.BtnText,Language.GodToken.UpGodToken .. "￥" .. self.data:GetGodTokenPrice()/10)
	-- else
	-- 	UH.SetText(self.BtnText,Language.GodToken.BuyLevel)
	-- end
end

function GodTokenRewardInfoPanel:OnUPClick()
	if self.data:IsBuyToken() == false then
	 	local param_t = {
			ok_func = function ()
				GodTokenCtrl.Instance:BuyGodToken()
			end,
			content_text = string.format(Language.GodToken.BuyGodTokenTip,self.data:GetGodTokenPrice()/10),
		}
		ViewMgr:OpenView(GodTokenTipView,param_t)
	-- else
	-- 	if self.data:GetCanMaxLevel() > 0 then
	-- 		ViewMgr:OpenView(GodTokenBuyLevelView)
	-- 		ViewMgr:CloseView(GodTokenRewardInfoView)
	-- 	else
	-- 		PublicPopupCtrl.Instance:Center(Language.GodToken.BuyLevelLimitTip)
	-- 	end
    end
end
