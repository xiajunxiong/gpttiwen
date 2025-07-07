LBFTreasureBuyLevelView = LBFTreasureBuyLevelView or DeclareView("LBFTreasureBuyLevelView", "lb_festival/lb_f_treasure_buy_level")
function LBFTreasureBuyLevelView:LBFTreasureBuyLevelView()

end

function LBFTreasureBuyLevelView:LoadCallback(param_t)

end 

function LBFTreasureBuyLevelView:OnClickClose()
    ViewMgr:CloseView(LBFTreasureBuyLevelView)
end

LBFTreasureBuyLevelPanel = LBFTreasureBuyLevelPanel or DeclareMono("LBFTreasureBuyLevelPanel", UIWFlushPanel)
function LBFTreasureBuyLevelPanel:LBFTreasureBuyLevelPanel()
    self.data = LBFestivalData.Instance
    self.data_cares = {
		{data = self.data.season_pass_info, func = self.FlushPanel, init_call = false},
        {data = self.data.operate_buy_info, func = self.FlushPanel, init_call = false},
    }
    self.buy_level_num = 0
    self.can_max_level = self.data:GetMaxBuyLevel()
end 

function LBFTreasureBuyLevelPanel:Awake()
	UIWFlushPanel.Awake(self)
    self:LevelHandle(10) -- 默认提升10级
end

function LBFTreasureBuyLevelPanel:OnDestroy()
	TimeHelper:CancelTimer(self.ht)
	UIWFlushPanel.OnDestroy(self)
    self.data:SetTreasureBuyLevelNum(0)
end


function LBFTreasureBuyLevelPanel:FlushPanel()
    local param = self.data:GetBuyLevelParam()
    self.List:SetData(param.reward_list)
    -- 本界面预览等级
    UH.SetText(self.LevelTip1,param.ready_level)
    -- 本界面最大等级
    UH.SetText(self.LevelTip2,param.max_level_str)
    UH.SetText(self.PriceNum,param.price_num)
    UH.SetText(self.HasNum,param.with_currency)
    -- 元宝-经验值转换
    UH.SetText(self.tip3,param.level_str)

    self.can_max_level = param.max_level
    self.exp_count = param.exp_count
    self.price = param.price_num

    -- LogError("?SD ",self.exp_count)
    if self.mark then 
        ViewMgr:CloseView(LBFTreasureBuyLevelView)
    end 
end

function LBFTreasureBuyLevelPanel:OnBuyClick()
	if not MallCtrl.IsNotCurrency(CurrencyType.Gold,self.price) then
        return
    end
	-- GodTokenCtrl.Instance:SendReq(RA_GOD_TOKEN_OP_TYPE.RA_GOD_TOKEN_OP_TYPE_BUY_EXP,self.exp_count)
    LBFestivalCtrl.Instance:ReqTreasureBuyLevel(self.exp_count)
	ViewMgr:CloseView(LBFTreasureBuyLevelPanel)
    self.mark = true
end

function LBFTreasureBuyLevelPanel:OnLevelDowm(step_num)
	TimeHelper:CancelTimer(self.ht)
    self.ht = TimeHelper:AddRunTimer(BindTool.Bind(self.LevelHandle,self,step_num),0.2)
end

function LBFTreasureBuyLevelPanel:OnLevelUp()
	TimeHelper:CancelTimer(self.ht)
	self:FlushPanel()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function LBFTreasureBuyLevelPanel:LevelHandle(num)
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
    
    self.data:SetTreasureBuyLevelNum(self.buy_level_num)
end