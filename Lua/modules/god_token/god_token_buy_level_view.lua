GodTokenBuyLevelView = GodTokenBuyLevelView or DeclareView("GodTokenBuyLevelView", "god_token/god_token_buy_Level")

VIEW_DECLARE_MASK(GodTokenBuyLevelView,ViewMask.BgBlock)

function GodTokenBuyLevelView:GodTokenBuyLevelView()
	--self.Board:SetData(self:GetType(),Language.GodToken.BuyLevel, Vector2.New(888, 644))
end

function GodTokenBuyLevelView:LoadCallback()
	UH.SetText(self.Title,Language.GodToken.BuyLevel)
end

function GodTokenBuyLevelView:CloseCallback()
	-- body
end

function GodTokenBuyLevelView:OnCloseClick()
	ViewMgr:CloseView(GodTokenBuyLevelView)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

GodTokenBuyLevelPanel = GodTokenBuyLevelPanel or DeclareMono("GodTokenBuyLevelPanel",UIWFlushPanel)
function GodTokenBuyLevelPanel:GodTokenBuyLevelPanel()
	self.data = GodTokenData.Instance
end

function GodTokenBuyLevelPanel:Awake()
	UIWFlushPanel.Awake(self)
	self.can_max_level = self.data:GetCanMaxLevel()
	self.buy_level_num = self.can_max_level < 10 and self.can_max_level or 10 		--默认升10级
	self:FlushReward()
end

function GodTokenBuyLevelPanel:FlushReward()
	self.can_max_level = self.data:GetCanMaxLevel()
	local level = self.data.god_token_data.token_level
	local up_level = level+self.buy_level_num
	local reward_data = self.data:GetSectionRewardList(level+1,up_level,true)
	self.List:SetData(reward_data)
	UH.SetText(self.LevelTip1,string.format(up_level))
	UH.SetText(self.LevelTip2,string.format(Language.GodToken.MaxLevelTip,self.can_max_level))
	self.exp_count = math.floor(self.data:GetLevelNeedExp(level,up_level)/10)
	self.price = self.exp_count * self.data:GetExpPrice()
	UH.SetText(self.PriceNum,self.price)
	local has_gold_num = RoleData.Instance:GetCurrency(CurrencyType.Gold)
	if has_gold_num >= self.price then
		has_gold_num = has_gold_num >= 100000 and DataHelper.ConverMoney(has_gold_num) or has_gold_num
		UH.SetText(self.HasNum,has_gold_num.."")
	else
		UH.SetText(self.HasNum,ColorStr(has_gold_num,COLORSTR.Red20))
	end
	UH.SetText(self.Tip3,string.format(Language.GodToken.Tip3,self.data:GetExpPrice()))
	UH.SetText(self.BuyNum,self.buy_level_num .. "")
end

function GodTokenBuyLevelPanel:OnBuyClick()
	if not MallCtrl.IsNotCurrency(CurrencyType.Gold,self.price) then
        return
    end
	GodTokenCtrl.Instance:SendReq(RA_GOD_TOKEN_OP_TYPE.RA_GOD_TOKEN_OP_TYPE_BUY_EXP,self.exp_count)
	ViewMgr:CloseView(GodTokenBuyLevelView)
end

function GodTokenBuyLevelPanel:OnLevelDowm(step_num)
	TimeHelper:CancelTimer(self.ht)
    self.ht = TimeHelper:AddRunTimer(BindTool.Bind(self.LevelHandle,self,step_num),0.2)
end

function GodTokenBuyLevelPanel:OnLevelUp()
	TimeHelper:CancelTimer(self.ht)
	self:FlushReward()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function GodTokenBuyLevelPanel:OnDestroy()
	TimeHelper:CancelTimer(self.ht)
	UIWFlushPanel.OnDestroy(self)
end

function GodTokenBuyLevelPanel:LevelHandle(num)
	self.buy_level_num = self.buy_level_num + num
	if self.buy_level_num < 1 then
		TimeHelper:CancelTimer(self.ht)
		PublicPopupCtrl.Instance:Center(Language.GodToken.ReduceLevelTip)
		self.buy_level_num = 1
	elseif self.buy_level_num > self.can_max_level then
		TimeHelper:CancelTimer(self.ht)
		PublicPopupCtrl.Instance:Center(Language.GodToken.AddLevelTip)
		self.buy_level_num = self.can_max_level
	end
	UH.SetText(self.BuyNum,self.buy_level_num .. "")
end