-- ===================================== 封神战令 ===========================
GodTokenPanel = GodTokenPanel or DeclareMono("GodTokenPanel", UIWFlushPanel)
function GodTokenPanel:GodTokenPanel()
	self.data = GodTokenData.Instance
	self.data_cares = {
		{data = self.data.tab_index_data, func = self.FlushTab, init_call = false},
	}
end

function GodTokenPanel:Awake()
	UIWFlushPanel.Awake(self)
	GodTokenCtrl.Instance:ReqInfo()
	self:FlushTab()
end

function GodTokenPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	self.data:SetSelectTab(GodTokenData.SELECT_TAB.REWARD)
end

function GodTokenPanel:FlushTab()
	local tab_index = self.data:GetSelectTab()
	for i = 1, self.Tabs:Length() do
		local tab = self.Tabs[i]
		if i == GodTokenData.SELECT_TAB.WEEKLY_TASK or i == GodTokenData.SELECT_TAB.DAILY_TASK then
			tab:SetActive(tab_index == GodTokenData.SELECT_TAB.WEEKLY_TASK or tab_index == GodTokenData.SELECT_TAB.DAILY_TASK)
		else
			tab:SetActive(i == tab_index)
		end
	end
end

--========== 等级信息 ==========
GodTokenLevelPanel = GodTokenLevelPanel or DeclareMono("GodTokenLevelPanel",UIWFlushPanel)

function GodTokenLevelPanel:GodTokenLevelPanel()
	self.data = GodTokenData.Instance
	self.data_cares = {
		{data = self.data.god_token_data, func = self.FlushLevel, init_call = false,keys = {"token_level","token_exp","surplus_receive_count"}},
    }
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.BuyLevelObj, Vector3.New(70, 25))
    end
    self.last_cur_exp = self.data:GetCurExp()
    self.last_need_exp = self.data:GetNextLevelNeedExp()
    self.last_level = self.data.god_token_data.token_level

    self.exp_btn_pos = self.GetExpBtnObj:GetLocalPosition()
    self.level_btn_pos = self.BuyLevelObj:GetLocalPosition()
end

function GodTokenLevelPanel:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self:FlushLevel()
end

function GodTokenLevelPanel:FlushLevel()
	if self.data.god_token_data.token_level ~= self.last_level then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.JinJie2)
	end
	self.red_point:SetNum(self.data.god_token_data.surplus_receive_count)
	UH.SetText(self.LevelText,self.data.god_token_data.token_level)
	if self.data:GetCanMaxLevel() <= 0 then
		UH.SetText(self.BtnText,Language.GodToken.SurplusReward)
	else
		UH.SetText(self.BtnText,Language.GodToken.BuyLevel)
	end
	if self.data:GetCanMaxLevel() <= 0 then
		if self.data.god_token_data.token_level ~= self.last_level then
			self.AnimProgress:AddToProgress(1,function ()
				self.AnimProgress:SetProgress(0)
			end)
			self.last_level = self.data.god_token_data.token_level
		else
			self.AnimProgress:SetProgress(0)
		end
		self.AnimProgress:SetText(Language.GodToken.ExpFull)
		self.AnimProgress.EffectObj:SetActive(false)
	else
		local need_exp = self.data:GetNextLevelNeedExp()
		if self.last_cur_exp ~= self.data:GetCurExp() or self.last_need_exp ~= need_exp or self.data.god_token_data.token_level ~= self.last_level then
			if self.data.god_token_data.token_level ~= self.last_level then
				self.AnimProgress:AddToProgress(need_exp/need_exp,function ()
					self.AnimProgress:AddToProgress(self.data:GetCurExp()/need_exp)
				end)
			else
				self.AnimProgress:AddToProgress(self.data:GetCurExp()/need_exp)
			end
			self.last_cur_exp = self.data:GetCurExp()
			self.last_need_exp = need_exp
			self.last_level = self.data.god_token_data.token_level
		else
			self.AnimProgress:SetProgress(self.data:GetCurExp()/need_exp)
		end
		self.AnimProgress:SetText(string.format(Language.GodToken.ExpValue,self.data:GetCurExp(),need_exp))
		self.AnimProgress.EffectObj:SetActive(self.data:GetCurExp()/need_exp > 0)
	end
end

function GodTokenLevelPanel:OnGetExpClick()
	if not self.data:GetDailyTaskIsAllAccomplish() then
		self.data:SetSelectTab(GodTokenData.SELECT_TAB.DAILY_TASK)
	else
		self.data:SetSelectTab(GodTokenData.SELECT_TAB.WEEKLY_TASK)
	end
end

function GodTokenLevelPanel:OnBuyLevelClick()
	if self.data:GetCanMaxLevel() > 0 then
		ViewMgr:OpenView(GodTokenBuyLevelView)
	else
		ViewMgr:OpenView(GodTokenSurplusRewardView)
	end
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end

function GodTokenLevelPanel:OnDestroy()
	if self.red_point ~= nil then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
	UIWFlushPanel.OnDestroy(self)
end

--========== 等级奖励 ==========
GodTokenRewardPanel = GodTokenRewardPanel or DeclareMono("GodTokenRewardPanel",UIWFlushPanel)

function GodTokenRewardPanel:GodTokenRewardPanel()
	self.data = GodTokenData.Instance
	self.data_cares = {
		{data = self.data.god_token_data, func = self.FlushPanel, init_call = false},
    }
end

function GodTokenRewardPanel:Awake()
	self.is_jump = false
	UIWFlushPanel.Awake(self)
	self:FlushPanel()
end

function GodTokenRewardPanel:FlushPanel()
	local end_time = os.date("%Y/%m/%d",self.data:GetEndTime()-1)
	local time_t = Split(end_time,"/")
	UH.SetText(self.InfoTime,string.format(Language.GodToken.EndTime,end_time))
	UH.SetText(self.TitleName,string.format(Language.GodToken.Title,DataHelper.GetWord(tonumber(time_t[2]))))
	local reward_list,end_item = self.data:GetRewardList(nil,false,false)
	self.reward_list = reward_list
	self.RewardList:SetData(reward_list)
	if not self.is_jump then
		local content_index = self.data:GetOrdinaryRewardNotIndex(reward_list)
		self.RewardList:JumpHorizontalIndex(content_index,-119)
		--self.is_jump = true
	end
	self.EndItem:SetData(end_item)
	self.EndItem:SetEndLevel()
	self.StateLock:SetActive(self.data:IsBuyToken() == false)
	self.JinjieLock:SetActive(not self.data:IsBuyToken())
	UH.SetText(self.PriceNum,self.data:GetGodTokenPrice()/10)

	self.AllGetBtnEffect:SetActive(self.data:GetReamindNum1() > 0)
end

function GodTokenRewardPanel:OnLookRewardClick()
	-- ViewMgr:OpenView(GodTokenRewardInfoView)
	self.data:SetSelectTab(GodTokenData.SELECT_TAB.CONVERT)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end

function GodTokenRewardPanel:OnBuyClick()
	local end_day = self.data:GetEndDay()
	local buy_func = function ()
		if self.data:IsBuyToken() == false then
			local param_t = {
				ok_func = function ()
					GodTokenCtrl.Instance:BuyGodToken()
				end,
				content_text = string.format(Language.GodToken.BuyGodTokenTip,self.data:GetGodTokenPrice()/10),
			}
			ViewMgr:OpenView(GodTokenTipView,param_t)
		end
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
	end
	if end_day <= 5 then
		local end_time = os.date("%Y/%m/%d",self.data:GetEndTime()-1)
		local time_t = Split(end_time,"/")
		local dialog_info = {
            content = string.format(Language.GodToken.BuyTip2, time_t[2], time_t[3]),
            confirm = {
                func = function ()
					buy_func()
					ViewMgr:CloseView(DialogTipsView)
				end
            },
            cancel = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                end
            },
            new_tnr = DTNewTodayNotRemind.GodTokenBuyTip,
        }
		PublicPopupCtrl.Instance:DialogTips(dialog_info)
	else
		buy_func()
	end
end

function GodTokenRewardPanel:OnTipClick()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[81].desc})
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end

function GodTokenRewardPanel:OnDestroy()
	if self.check_handle then
		Runner.Instance:CancelRepeat(self.check_handle)
		self.check_handle = nil
	end
	UIWFlushPanel.OnDestroy(self)
end

function GodTokenRewardPanel:OnConvertClick()
	self.data:SetSelectTab(GodTokenData.SELECT_TAB.CONVERT)
end

function GodTokenRewardPanel:OnGetAllRewardClick()
	for i,v in pairs(self.reward_list or {}) do
		local state = self.data:GetOrdinaryRewardState(v.seq)
		local senior_state = self.data:GetSeniorRewardState(v.seq)
		if state == GodTokenData.REWARD_STATE.CAN or senior_state == GodTokenData.REWARD_STATE.CAN then
			GodTokenCtrl.Instance:GetReawrd(v.seq)
		end
	end
end

GodTokenRewardItem = GodTokenRewardItem or DeclareMono("GodTokenRewardItem",UIWidgetBaseItem)

function GodTokenRewardItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	UH.SetText(self.LevelText,string.format(data.open_grade..Language.GodToken.Level))
	self.Block1:SetActive(GodTokenData.Instance.god_token_data.token_level < data.open_grade)
	self.Block2:SetActive(GodTokenData.Instance.god_token_data.token_level < data.open_grade or not GodTokenData.Instance:IsBuyToken()) 
	local ordinary_reward_data = {}
	ordinary_reward_data.reward_item = data.ordinary_item
	ordinary_reward_data.state = GodTokenData.Instance:GetOrdinaryRewardState(data.seq)
	ordinary_reward_data.seq = data.seq
	self.OrdinaryRewardItem:SetData(ordinary_reward_data)
	local senior_reward_item_list = {}
	local senior_state = GodTokenData.Instance:GetSeniorRewardState(data.seq)
	for i=0,#data.senior_item do
		senior_reward_item_list[i + 1] = {}
		local v = data.senior_item[i]
		senior_reward_item_list[i + 1].reward_item = v
		senior_reward_item_list[i + 1].state = senior_state
		senior_reward_item_list[i + 1].seq = data.seq
	end
	if data.senior_gold_num and data.senior_gold_num > 0 then
		local reward = {item_id = CommonItemId.Gold,num = data.senior_gold_num,is_bind = 0}
		table.insert(senior_reward_item_list,{reward_item = reward,state = senior_state,seq = data.seq})
	end
	self.SeniorRewardList:SetData(senior_reward_item_list)
end

function GodTokenRewardItem:SetEndLevel()
	UH.SetText(self.LevelText,string.format(self.data.open_grade..Language.GodToken.EndLevel))
end

GodTokenRewardChildItem = GodTokenRewardChildItem or DeclareMono("GodTokenRewardChildItem",UIWidgetBaseItem)

function GodTokenRewardChildItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
    self.Cell:SetData(Item.Create(data.reward_item))
    self.EffectObj:SetActive(data.state == GodTokenData.REWARD_STATE.CAN)
    self.BlockObj:SetActive(data.state == GodTokenData.REWARD_STATE.NOT)
    self.GetFlagObj:SetActive(data.state == GodTokenData.REWARD_STATE.GETED)
end

function GodTokenRewardChildItem:OnGetClick()
	GodTokenCtrl.Instance:GetReawrd(self.data.seq)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.OpenGift)
end

--========== 每日任务列表 =========
GodTokenTaskPanel = GodTokenTaskPanel or DeclareMono("GodTokenTaskPanel",UIWFlushPanel)
function GodTokenTaskPanel:GodTokenTaskPanel()
	self.data = GodTokenData.Instance
	self.data_cares = {
		{data = self.data.god_token_data, func = self.FlushPanel, init_call = false},
	}
	self.task_type = 1
end

function GodTokenTaskPanel:Awake()
    UIWFlushPanel.Awake(self)
   self:FlushPanel()
end

function GodTokenTaskPanel:FlushPanel()
	local task_list = self:GetTaskList()
	self.List:SetData(task_list)
	self.List:SetCompleteCallBack(BindTool.Bind(self.OnListCallback,self))
	UH.SetText(self.Level,self.data.god_token_data.token_level)
	local exp,sum_exp = self.data:GetTaskExpInfo(self.task_type)
	UH.SetText(self.Exp,string.format(Language.GodToken.TaskExp1,exp, sum_exp))

	self.Time:CloseCountDownTime()
	self.Time:TotalTime(self:GetTime(),TimeType.Type_Special_4)
	self.Time:SetCallBack(function ()
		self.Time:CloseCountDownTime()
		self.Time:TotalTime(self:GetTime(),TimeType.Type_Special_4)
	end)
end

function GodTokenTaskPanel:GetTaskList()
	if self.task_type == 1 then
		return self.data:GetDailyTaskList()
	else
		return self.data:GetWeeklyTaskList()
	end
end

function GodTokenTaskPanel:GetTime()
	if self.task_type == 1 then
		return self.data:GetTaskUpdateTime()
	else
		return self.data:GetWeeklyTaskUpdateTime()
	end
end

function GodTokenTaskPanel:OnDestroy()
	self.Time:CloseCountDownTime()
	TimeHelper:CancelTimer(self.time_handle)
	UIWFlushPanel.OnDestroy(self)
end

function GodTokenTaskPanel:OnListCallback()
	for i,v in pairs(self.List.item_list) do
		v:SetRedPointRange(self.Layout,self.Layout.gameObject,i)
	end
end

function GodTokenTaskPanel:OnDrag()
	self:OnCheckItem()
end

function GodTokenTaskPanel:OnEndDrag()
	self.default_step = 60
	TimeHelper:CancelTimer(self.time_handle)
	self.time_handle = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.OnCheckItem,self),1,self.default_step)
end

function GodTokenTaskPanel:OnCheckItem()
	if self.time_handle and self.default_step then
		self.default_step = self.default_step - 1
		if self.default_step == 0 then
			TimeHelper:CancelTimer(self.time_handle)
			self.time_handle = nil
			self.default_step = 60
		end
	end
	for i,v in pairs(self.List.item_list) do
		v:ChaceRedPointPos()
	end
end

function GodTokenTaskPanel:OnToggleTaskTypeClick(task_type)
	self.task_type = task_type
	self:FlushPanel()
end

GodTokenTaskItem = GodTokenTaskItem or DeclareMono("GodTokenTaskItem",UIWidgetBaseItem)
function GodTokenTaskItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	local succ_count
	if data.task_tab == GodTokenData.SELECT_TAB.WEEKLY_TASK then
		succ_count = GodTokenData.Instance:GetWeeklyTaskAccomplishCount(data.type).count
	else
		succ_count = GodTokenData.Instance:GetDailyTaskAccomplishCount(data.type).count
	end
	if succ_count < data.parameter then
		local num = succ_count < 0 and data.parameter or succ_count
		if succ_count >= 0 then
			--local succ_count_str = ColorStr(num .. "",COLORSTR.Green11)
			UH.SetText(self.TaskDescText,data.describe..string.format(" (%s/%s)",num,data.parameter))
		else
			local succ_count_str = ColorStr(string.format(" (%s/%s)",num,data.parameter),COLORSTR.Green11)
			UH.SetText(self.TaskDescText,data.describe..succ_count_str)
		end
	else
		succ_count = data.parameter
		local succ_count_str = ColorStr(string.format(" (%s/%s)",succ_count,data.parameter),COLORSTR.Green11)
		UH.SetText(self.TaskDescText,string.format("%s%s",data.describe,succ_count_str))
	end
	self.red_num = succ_count == data.parameter and 1 or 0
	self.RedPoint:SetNum(self.red_num)
	UH.SetText(self.ExpText,data.open_exp .. "")
	self.NotAccomplishBgObj:SetActive(succ_count <= data.parameter and succ_count >= 0)
	self.AccomplishBgObj:SetActive(succ_count < 0)
end
function GodTokenTaskItem:OnGotoClick()
	if self.data.open_panel ~= nil and self.data.open_panel ~= "" and self.data.open_panel ~= 0 then
		ViewMgr:OpenViewByKey(self.data.open_panel)
		ViewMgr:CloseView(RewardView)
	end
end

function GodTokenTaskItem:OnGetClick()
	if self.data.task_tab == GodTokenData.SELECT_TAB.DAILY_TASK then
		GodTokenCtrl.Instance:SendReq(RA_GOD_TOKEN_OP_TYPE.RA_GOD_TOKEN_OP_TYPE_FETCH_DAILY_TASK,self.data.get_index)
	else
		GodTokenCtrl.Instance:SendReq(RA_GOD_TOKEN_OP_TYPE.RA_GOD_TOKEN_OP_TYPE_FEICH_WEEKLY_TASK,self.data.get_index)
	end
end

function GodTokenTaskItem:OnDestroy()
	self.content = nil
    UIWidgetBaseItem.OnDestroy(self)
end

-- 由于红点和特效的层级冲突，红点添加了UI Order Canvas组件，会影响mask2d
function GodTokenTaskItem:ChaceRedPointPos()
	if self.content == nil or self.red_point_max_y == nil then
		return
	end
	local content_pos = self.content:GetLocalPosition()
	--LogError("content_pos_Y",content_pos.y)
	if content_pos.y >= self.red_point_max_y or content_pos.y <= self.red_point_min_y then
		if self.red_num ~= 0 then
			self.RedPoint:SetNum(0)
		end
	else
		if self.red_num ~= self.RedPoint.num then
			self.RedPoint:SetNum(self.red_num)
		end
	end
end

-- 设置本item红点可显示范围 -- 这里写死了，任务的列数 = 2 、 scrollRect的高写死，改动ui的时候注意下
-- layout === GridLayoutGroup
-- content === GameObj
function GodTokenTaskItem:SetRedPointRange(layout,content,index)
	self.content = content
	local scroll_h = 552 					   -- scrollRect的高
	local line_num = math.floor((index + 1)/2) -- 任务的列数 = 2
	local item_h = layout.cellSize.y
	local spacing_y = layout.spacing.y
	local top = layout.padding.top
	local bottom = layout.padding.bottom
	local red_h = 30 / 2  --红点露出来的高，写死
	local show_content_h = (scroll_h - top - bottom) -- 显示内容的高
	local max_line_num = math.floor(show_content_h / (item_h + spacing_y)) + 1  -- 最大显示的行数
	local end_item_remain_h = item_h - ((max_line_num * (item_h + spacing_y) - spacing_y) - show_content_h) -- 最后一行的item露出来的高
	self.red_point_max_y = (line_num * (item_h + spacing_y)) - item_h - spacing_y + red_h
	self.red_point_min_y = (line_num - max_line_num) * (item_h + spacing_y) - end_item_remain_h
	--LogError("y::",self.red_point_max_y,self.red_point_min_y,end_item_remain_h,max_line_num,line_num)
end

--========== 兑换物品 ==========
GodTokenConventPanel = GodTokenConventPanel or DeclareMono("GodTokenConventPanel",UIWFlushPanel)
function GodTokenConventPanel:GodTokenConventPanel()
	self.data = GodTokenData.Instance
	self.data_cares = {
		{data = self.data.god_token_data, func = self.FlushPanel, init_call = false},
		{data = BagData.Instance.item_grid, func = self.FlushCurr, init_call = false},
    }
	self.select_index = 1
end

function GodTokenConventPanel:Awake()
    UIWFlushPanel.Awake(self)
	self:FlushPanel()
	self:FlushCurr()
end

function GodTokenConventPanel:FlushPanel()
	self:FlushList()
end

function GodTokenConventPanel:FlushCurr()
	local item_num = Item.GetNum(27495)
	UH.SetText(self.CurrNum, item_num)
end

function GodTokenConventPanel:FlushList()
	self.List:SetDataList(self.data:GetConventList(),self.select_index)
end

GodTokenConventItem = GodTokenConventItem or DeclareMono("GodTokenConventItem",UIWidgetBaseItem)

function GodTokenConventItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	self.Cell:SetData(Item.Create(data.item))
	UH.SetText(self.ItemName,Item.GetName(data.item.item_id))
	UH.SetText(self.Price, data.item_use.num)
	self.PetLookBtnObj:SetActive(Item.GetShowType(data.item.item_id) == ShowType.Pet)
	self:SetState()
end

function GodTokenConventItem:SetState()
	local state = GodTokenData.Instance:GetConvertItemState(self.data)
	self.data.con_state = state
	self.LimitBuyText:SetObjActive(state == 1)
	if state == -1 then
		--UH.SpriteName(self.BgImg,"WeiKaiQiDi")
		UH.SetText(self.ShowTip,string.format(Language.GodToken.ShowTip1,self.data.open_token))
	elseif state == 0 then
		--UH.SpriteName(self.BgImg,"WeiKaiQiDi")
		UH.SetText(self.ShowTip,string.format(Language.GodToken.ShowTip2,self.data.end_token))
	else
		--UH.SpriteName(self.BgImg,"WeiDianZhongDi")
		local convert_count = GodTokenData.Instance:GetItemConvertCount(self.data.seq)
		if self.data.buy_reset > 0 then
			UH.SetText(self.LimitBuyText,string.format(Language.GodToken.ActivityConvertLimit,convert_count,self.data.buy_reset))
		else
			UH.SetText(self.LimitBuyText,string.format(Language.GodToken.AailyConvertLimit,convert_count,self.data.buy_everyday))
		end
	end
end

function GodTokenConventItem:OnPetItemClick()
	if Item.GetShowType(self.data.item.item_id) == ShowType.Pet then
		local pet = Pet.New({pet_id = self.data.item.item_id})
    	ViewMgr:OpenView(PetDetailView,pet)
	end
end

function GodTokenConventItem:OnConvertClick()
	local con_data = self.data
	con_data.con_state = GodTokenData.Instance:GetConvertItemState(con_data)
	if con_data.con_state == -1 then
		PublicPopupCtrl.Instance:Error(string.format(Language.GodToken.ConvertTip1,con_data.open_token))
		return
	elseif con_data.con_state == 0 then
		PublicPopupCtrl.Instance:Error(string.format(Language.GodToken.ConvertTip2,con_data.end_token + 1))
		return
	end
	if GodTokenData.Instance:GetItemConvertCount(con_data.seq) >= con_data.buy_reset then
		PublicPopupCtrl.Instance:Error(Language.GodToken.ContentTip)
		return
	end
	if Item.GetNum(27495) >= con_data.item_use.num then
		GodTokenCtrl.Instance:SendReq(RA_GOD_TOKEN_OP_TYPE.RA_GOD_TOKEN_OP_TYPE_EXCHANGE,con_data.seq)
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	else
		PublicPopupCtrl.Instance:Error(Language.GodToken.ContentTip2)
	end
end
