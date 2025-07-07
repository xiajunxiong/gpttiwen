LBFTreasure = LBFTreasure or DeclareMono("LBFTreasure", UIWFlushPanel)
function LBFTreasure:LBFTreasure()
    self.data = LBFestivalData.Instance

    self.data_cares = {
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end 

function LBFTreasure:OnClickReturn()
    self.RewardShow.gameObject:SetActive(true)
    self.TaskShow.gameObject:SetActive(false)
end

function LBFTreasure:OnGetExpClick()
    self.RewardShow.gameObject:SetActive(false)
    self.TaskShow.gameObject:SetActive(true)
end

function LBFTreasure:OnTipClick()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[383].desc})
end

function LBFTreasure:OnBuyClick()
        -- 检查是否支持万能卡
	local fee = self.data:GetGodSeniorPrice() -- 元宝数
    local card_num = fee / 10
    ServerActivityCtrl.Instance:SendCmdBuyItemCheckReq(BUY_ITEM_TYPE.TYPE_GOD_SPACE_SEASON_PASS, card_num)

	-- if ServerActivityCtrl.Instance:IsCanItemBuy(BUY_ITEM_TYPE.TYPE_GOD_SPACE_SEASON_PASS, card_num) then
    --     ServerActivityCtrl.Instance:SendBuyCmdReq(BUY_ITEM_TYPE.TYPE_GOD_SPACE_SEASON_PASS, card_num)
	-- else
    --     ServerActivityCtrl.Instance:SendCmdBuyItemCheckReq(BUY_ITEM_TYPE.TYPE_GOD_SPACE_SEASON_PASS, fee)
        
	-- 	-- ChannelAgent:RequestPay(fee, BUY_ITEM_TYPE.TYPE_GOD_SPACE_SEASON_PASS)
	-- end
end

function LBFTreasure:SendBuy()
    local fee = self.data:GetGodSeniorPrice() -- 元宝数
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_GOD_SPACE_SEASON_PASS,fee)
end

function LBFTreasure:OnBuyLevelClick()
    if self.data:CheckIsOpenBuyLevel() then 
        ViewMgr:OpenView(LBFTreasureSurplusRewardView)
    else
        ViewMgr:OpenView(LBFTreasureBuyLevelView) 
    end 
end

function LBFTreasure:OnLookRewardClick()
    ViewMgr:OpenView(LBFTreasureWelfareView)
end

function LBFTreasure:OnGetAllRewardClick()
    if not self.data:CheckTreasureWithReward() then 
        PublicPopupCtrl.Instance:Center(Language.LBFestival.Treasure.OnkeyEmpty)        
        return 
    end 

    self.data:TryToOneKeyTresureReward()
end

LBFTreasureRewardPanel = LBFTreasureRewardPanel or DeclareMono("LBFTreasureRewardPanel", UIWFlushPanel)
function LBFTreasureRewardPanel:LBFTreasureRewardPanel()
    self.data = LBFestivalData.Instance
	self.data_cares = {
		{data = self.data.season_pass_info, func = self.FlushPanel},
        {data = self.data.operate_info, func = self.FlushPanel, init_call = false},
    }
end 

function LBFTreasureRewardPanel:FlushPanel()
    local param = self.data:GetTreasureRewardParam()
    -- list
    self.RewardList:SetData(param.reward_list)
    local content_index =  self.data.wait_reward_index-- self.data.season_pass_info.level
    if content_index >= 26 then 
        self.RewardList:ForceJumpHorizontal(-3369)
    else 
        self.RewardList:JumpHorizontalIndex(content_index,-136,-2*content_index)
    end 
    -- timer
    self.InfoTime:StampTime(param.stamp_time,TimeType.Type_Time_3,Language.LBFestival.Treasure.TimeDone)
    -- obj
    self.ProLock:SetActive(not param.pro_un_lock)
    -- LBFTreasureRewardItem
    self.EndItem:SetData(param.reward_list[#param.reward_list])
    -- text
    UH.SetText(self.PriceNum,param.pro_price)
    -- obj
    self.AllGetBtnEffect:SetActive(param.can_one_key)
end

LBFTreasureLevelPanel = LBFTreasureLevelPanel or DeclareMono("LBFTreasureLevelPanel", UIWFlushPanel)
function LBFTreasureLevelPanel:LBFTreasureLevelPanel()
    self.data = LBFestivalData.Instance
    self.data_cares = {
		{data = self.data.season_pass_info, func = self.FlushPanel},--  init_call = false},
        {data = self.data.operate_info, func = self.FlushPanel, init_call = false},
    }

    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.BuyLevelObj, Vector3.New(70, 25))
    end
end 

function LBFTreasureLevelPanel:OnDestroy()
	if self.red_point ~= nil then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
	UIWFlushPanel.OnDestroy(self)
end

function LBFTreasureLevelPanel:FlushPanel()
    local param = self.data:GetTreasureLevelParam()
    -- text
    UH.SetText(self.LevelText,param.level)
    -- text
    UH.SetText(self.BtnText,param.buy_text)
    -- obj
    -- self.BuyLevelObj
    self.red_point:SetNum(param.with_surplus)
    -- progress
    self.Progress:SetProgress(param.exp_progress)
    self.Progress:SetText(param.exp_progress_str)
    -- obj
    -- self.GetExpBtnObj
    self.show_buy_level = param.show_buy_level
end

LBFTreasureTaskPanel = LBFTreasureTaskPanel or DeclareMono("LBFTreasureTaskPanel", UIWFlushPanel)
function LBFTreasureTaskPanel:LBFTreasureTaskPanel()
    self.data = LBFestivalData.Instance
    self.data_cares = {
		{data = self.data.season_pass_info, func = self.FlushPanel},--  init_call = false},
        {data = self.data.operate_info, func = self.FlushPanel, init_call = false},
    }
    self.data:SetTreasureTaskType(1)
end 

function LBFTreasureTaskPanel:FlushPanel()
    local param  = self.data:GetTreasureTaskParam()
    -- list 
    self.List:SetData(param.task_list)
    -- Timer
    self.Time:StampTime(param.stamp_time,TimeType.Type_Time_3)

end

function LBFTreasureTaskPanel:OnToggleTaskTypeClick(task_type)
	self.data:SetTreasureTaskType(task_type)
end

function LBFTreasureTaskPanel:OnDrag()
    
end

function LBFTreasureTaskPanel:OnEndDrag()
    
end

LBFTreasureTaskItem = LBFTreasureTaskItem or DeclareMono("LBFTreasureTaskItem",UIWidgetBaseItem)
function LBFTreasureTaskItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)

    UH.SetText(self.TaskDescText,data.desc..data.progress_str)
    UH.SetText(self.ExpText,data.open_exp)

    self.NotAccomplishBgObj:SetActive(data.accomplish)
	self.AccomplishBgObj:SetActive(not data.accomplish)
    self.Lock:SetActive(data.show_lock)

end 

function LBFTreasureTaskItem:OnGotoClick()
	if self.data.open_panel ~= nil and self.data.open_panel ~= "" and self.data.open_panel ~= 0 then
		ViewMgr:OpenViewByKey(self.data.open_panel)
		ViewMgr:CloseView(LBFestivalView)
    else 
        PublicPopupCtrl.Instance:Center(Language.LBFestival.Treasure.EmptyPanel)
	end
end

LBFTreasureRewardItem = LBFTreasureRewardItem or DeclareMono("LBFTreasureRewardItem",UIWidgetBaseItem)
function LBFTreasureRewardItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)

    UH.SetText(self.LevelText,string.format(data.open_grade..Language.GodToken.Level))
	self.Block1:SetActive(data.nomal_level_flag) -- GodTokenData.Instance.god_token_data.token_level < data.open_grade)
	self.Block2:SetActive(data.pro_levek_flag) --GodTokenData.Instance.god_token_data.token_level < data.open_grade or not GodTokenData.Instance:IsBuyToken()) 
    self.Lock:SetActive(data.pro_lock_flag)
	-- local ordinary_reward_data = {}
	-- ordinary_reward_data.reward_item = data.ordinary_item
	-- ordinary_reward_data.state = GodTokenData.Instance:GetOrdinaryRewardState(data.seq)
	-- ordinary_reward_data.seq = data.seq
	self.OrdinaryRewardItem:SetData(data.nomal_reward) --ordinary_reward_data)
	-- local senior_reward_item_list = {}
	-- local senior_state = GodTokenData.Instance:GetSeniorRewardState(data.seq)
	-- for i=0,#data.senior_item do
	-- 	senior_reward_item_list[i + 1] = {}
	-- 	local v = data.senior_item[i]
	-- 	senior_reward_item_list[i + 1].reward_item = v
	-- 	senior_reward_item_list[i + 1].state = senior_state
	-- 	senior_reward_item_list[i + 1].seq = data.seq
	-- end
	-- if data.senior_gold_num and data.senior_gold_num > 0 then
	-- 	local reward = {item_id = CommonItemId.Gold,num = data.senior_gold_num,is_bind = 0}
	-- 	table.insert(senior_reward_item_list,{reward_item = reward,state = senior_state,seq = data.seq})
	-- end
	self.SeniorRewardList:SetData(data.pro_reward)-- senior_reward_item_list)
end 

LBFTreasureRewardChildItem = LBFTreasureRewardChildItem or DeclareMono("LBFTreasureRewardChildItem",UIWidgetBaseItem)
function LBFTreasureRewardChildItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
    self.Cell:SetData(data.reward_item)
    self.EffectObj:SetActive(data.state == GodTokenData.REWARD_STATE.CAN)
    self.BlockObj:SetActive(data.state == GodTokenData.REWARD_STATE.NOT)
    self.GetFlagObj:SetActive(data.state == GodTokenData.REWARD_STATE.GETED)

    --注册红点
	if not self.is_care then
        self.is_care = true
        self:careData(LBFestivalData.Instance.season_pass_info, BindTool.Bind(self.ActRedPoint, self))
    else 
        self:ActRedPoint()
	end
end

function LBFTreasureRewardChildItem:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
	-- if not self.is_care then return end
    self:uncareData()
    if self.red_point then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
end
function LBFTreasureRewardChildItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
	-- if not self.is_care then return end
    self:uncareData()
    if self.red_point then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
end

function LBFTreasureRewardChildItem:ActRedPoint()
    if self.RedPos == nil then return end 
    self.red_point = self.red_point or UiRedPoint.New(self.RedPos, Vector3.New(0,0,0))
    if self.red_point ~= nil and ViewMgr:IsOpen(LBFestivalView) then 
	    self.red_point:SetNum(self.data.state == GodTokenData.REWARD_STATE.CAN and 1 or 0)
    end
end

function LBFTreasureRewardChildItem:OnGetClick()
	-- GodTokenCtrl.Instance:GetReawrd(self.data.seq)
    LBFestivalCtrl.Instance:ReqTreasureReward(self.data.level,self.data.type)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.OpenGift)
end