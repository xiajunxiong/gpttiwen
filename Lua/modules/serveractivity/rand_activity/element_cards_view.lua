ElementCardsView = ElementCardsView or DeclareView("ElementCardsView","serveractivity/element_cards_view",Mod.Splendid.ElementCards)
VIEW_DECLARE_LEVEL(ElementCardsView, ViewLevel.L1)
--Title=========Text

VIEW_DECLARE_MASK(ElementCardsView,ViewMask.BgBlock)

--请求活动协议类型
ElementCardsView.REQ_OP_RTPE = {
	INFO = 0, 		--信息下发
	CTRL = 1,  		--翻牌
}

--翻牌特效
ElementCardsView.Card_Effext={
	before="7165498",
	after="7165499"
}

function ElementCardsView:ElementCardsView()
	
end

function ElementCardsView:LoadCallback(param_t)
	self.Title.text = ActivityRandData.GetName(ACTIVITY_TYPE.RAND_ELEMENT_CARDS)
	self.panels[ServerActivityData.Instance:GetElementShowInterface()]:SetActive(true)
end

function ElementCardsView:CloseCallback()
	-- body
end

ElementCardsPanel = ElementCardsPanel or DeclareMono("ElementCardsPanel",UIWFlushPanel)
function ElementCardsPanel:ElementCardsPanel()
	self.data = ServerActivityData.Instance
	self.cfg = self.data:GetElementCardsCfg()
	self.data_cares = {
		{data = self.data.element_cards_data,func = self.FlushPanel,keys = {"flop_count"}, init_call = false},
    }
    self.is_ctrl_cards_ing = false    --是否在翻牌中
    self.is_ctrl_cards_ed = false     --是否翻完了
    self.is_click_btn_ing = false     --是否正在点击按钮
    self.element_cards_str = Language.ElementCards[self.data:GetElementShowInterface()]
end

function ElementCardsPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Time:CloseCountDownTime()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_ELEMENT_CARDS, TimeType.Type_Special_4)
    self:SetRewardShow()
    self:SetCardsText()
    TimeHelper:CancelTimer(self.playEffTimer)
    self.playEffTimer = TimeHelper:AddDelayTimer(function()
        self:FlushPanel()
    end, 0.5)
end


function ElementCardsPanel:FlushPanel(param_t)
    local act_data = self.data:GetElementCardsData()
    if act_data.flop_seq > 0 then
        self:CtrlCards(act_data.flop_seq)
    end
    UH.SetText(self.RemainNum, self.data:GetElementCardsNum() .. "/1")
    self:FlushEffect()
end

function ElementCardsPanel:FlushEffect()
    if ServerActivityData.Instance:GetElementShowInterface() == 2 then
		if not self.effect_handle then
			self.effect_handle = {}
		end
		local act_data = self.data:GetElementCardsData()
        if self.is_ctrl_cards_ing or act_data.flop_seq == 0 then
            -- 背面特效
			for i = 1, 4, 1 do
				UH.StopEffect(self.EffectTools[i],self.effect_handle[i])
				if i~=act_data.flop_seq then
					self.effect_handle[i]=UH.PlayEffect(self.EffectTools[i],ElementCardsView.Card_Effext.before)
				end
			end
        else
            -- 正面特效
			self.effect_handle[act_data.flop_seq]=UH.PlayEffect(self.EffectTools[ act_data.flop_seq], ElementCardsView.Card_Effext.after)
        end
    end
end


function ElementCardsPanel:OnDestroy()
	TimeHelper:CancelTimer(self.ShowTimer)
	TimeHelper:CancelTimer(self.playEffTimer)
	if  self.effect_handle  then
		for i = 1, 4, 1 do
				UH.StopEffect(self.EffectTools[i],self.effect_handle[i])
		end
	end
	UIWFlushPanel.OnDestroy(self)
end

function ElementCardsPanel:SetRewardShow()
	local reward_item_t = {}
	for i,v in pairs(self.cfg.other[1].reward_item) do
		local item = Item.Create(v)
		table.insert(reward_item_t,item)
	end
	self.RewardList:SetData(reward_item_t)
end

function ElementCardsPanel:SetCardsText()
	local len = self.CardsText:Length()
	for i=1,len do
		local v = self.cfg.reward[i]
        if v then
    		UH.SetText(self.CardsText[i],self.element_cards_str[v.brand])
        end
	end
end

--翻牌
function ElementCardsPanel:CtrlCards(index)
	if self.is_ctrl_cards_ing == true then
		return
	end
	self.is_ctrl_cards_ing = true
	if self.CardsAnim[index] then
        self.CardsAnim[index]:Play()
    end
    self.ShowTimer = TimeHelper:AddDelayTimer(function()
        self.is_ctrl_cards_ing = false
        self.is_ctrl_cards_ed = true
		self:FlushEffect()
    end, 1)
end

--复原
function ElementCardsPanel:CardsRecover()
	local act_data = self.data:GetElementCardsData()
	self.CardsAnim[act_data.flop_seq]:Play("element_cards_recover")
end

--获取翻牌元素信息
function ElementCardsPanel:GetElementInfo(target_index)
	local len = #self.element_cards_str
	local ramdon_index = AmuletData.Instance:GetRamdonList(1,len)
	local element_info = {elements = {},target_index = 1}
	local target = self.element_cards_str[target_index]
	for i=1,len do
		element_info.elements[i] = self.element_cards_str[ramdon_index[i]]
		if element_info.elements[i] == target then
		    element_info.target_index = i
		end
	end
	return element_info
end

function ElementCardsPanel:OnCloseClick()
	ViewMgr:CloseView(ElementCardsView)
end

function ElementCardsPanel:OnTipClick()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[78].desc})
end

function ElementCardsPanel:OnExecuteActClick()
	if self.is_ctrl_cards_ing == true then
		return
	end
	if self.is_click_btn_ing == true then
		return
	end
	if self.is_ctrl_cards_ed then
		if self.data:GetElementCardsNum() > 0 then
			self:CardsRecover()
		end
		self.is_ctrl_cards_ed = false
		self.is_click_btn_ing = true
		self.ShowTimer = TimeHelper:AddDelayTimer(function()
			self.is_click_btn_ing = false
	        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ELEMENT_CARDS,ElementCardsView.REQ_OP_RTPE.CTRL)
	    end, 1)
		return
	end
	ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ELEMENT_CARDS,ElementCardsView.REQ_OP_RTPE.CTRL)
end