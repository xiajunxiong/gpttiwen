ActiveDailyView = ActiveDailyView or DeclareMono("ActiveDailyView", UIWFlushPanel)

function ActiveDailyView:ActiveDailyView()
    self.data = ActiveData.Instance
    self.data_cares = {
        {data = ActivityData.Instance.active_data, func = self.FlushActiveView},
        {data = ActivityData.Instance.active_data, func = self.FlushListView},
    }
    self:CheckGetGuideUi()
end

function ActiveDailyView:FlushListView()
    self.List:SetData(self.data:GetDailyList())
    self:CheckAnim()
    self:RightDownFlush()
end

function ActiveDailyView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.ShowTimer)
    if GuideData.Instance:TaskBoxGuide() then
        local task_info = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.mainline)
        if task_info then
            local show_c, condition = task_info:TaskMenuCondition()
            if show_c and task_info:AutoCfg().min_level == 33 then
                GuideManager.Instance:Start(ConstGuideId.TaskBoxGuide)
                GuideData.Instance:StopTaskBoxGuide()
            end
        end
    end
end

function ActiveDailyView:FlushActiveView()
    local active_num = ActivityData.Instance:GetActiveIntegralNum()
    local reward_list,list = ActivityData.Instance:GetActiveRewardData()
    self.Progress:SetProgress(self:GetProgressNum(active_num,list))
    self:FlushEffect(active_num >= list[#list])
    self.Progress:SetText(active_num)
    self:FlushRewardList(reward_list)
end

function ActiveDailyView:FlushEffect(show_effect)
    if self.effect_handle ~= nil then
        self.EffectTool:Stop(self.effect_handle)
    end
    if self.ShowTimer and self.EffectTool.isActiveAndEnabled then
        self.effect_handle = self.EffectTool:Play(2161018)
    end
    self.Effect:SetActive(show_effect)
end

function ActiveDailyView:FlushRewardList(list)
    for i=1,self.RewardList:Length() do
        self.RewardList[i]:SetData(list[i])
    end
end

function ActiveDailyView:GetProgressNum(num,list)
    local amount_num = (DataHelper.GetFillAmount(num,list) - 0.075)
    return amount_num < 0 and 0 or amount_num
end

function ActiveDailyView:CheckGetGuideUi()
	GuideManager.Instance:RegisterGetGuideUi("GuildTaskBtn", function()
		return self:GetGuideUi(ActType.BountyTask)
	end)
	GuideManager.Instance:RegisterGetGuideUi("ResearchTaskBtn", function()
		return self:GetGuideUi(ActType.ResearchTask)
	end)
	GuideManager.Instance:RegisterGetGuideUi("TreasureMapBtn", function()
		return self:GetGuideUi(ActType.TreasureMap)
	end)
	GuideManager.Instance:RegisterGetGuideUi("WSTQuestenBtn", function()
		return self:GetGuideUi(ActType.WSTQuesten)
	end)
    GuideManager.Instance:RegisterGetGuideUi("WLStorybookBtn", function()
		return self:GetGuideUi(ActType.AllSoulsFB)
	end)
    GuideManager.Instance:RegisterGetGuideUi("TreasureTaskBtn", function()
		return self:GetGuideUi(ActType.TreasureTask)
	end)

    GuideManager.Instance:RegisterGetGuideUi("TwiceDailyBtn", function()
        local act_key = GuideManager.Instance:GuideView().step_cfg.step_param_2
        if act_key == "" then act_key = ActType.TreasureTask end 
        self.List:JumpVertCondition(function(data) return data.act_type == act_key end,2)
        self.FixArea.sizeDelta = Vector2.New(906, 269)
		return self.FixArea.gameObject
	end)

    GuideManager.Instance:RegisterGetGuideUi("FourDailyBtn", function()
        local act_key = GuideManager.Instance:GuideView().step_cfg.step_param_2
        if act_key == "" then act_key = ActType.TreasureTask end 
        self.List:JumpVertCondition(function(data) return data.act_type == act_key end,2)
        self.FixArea.sizeDelta = Vector2.New(906, 547)
		return self.FixArea.gameObject
	end)

    GuideManager.Instance:RegisterGetGuideUi("ActiveDailyRightArea", function ()
        return self.WholeView, function ()
            
        end
    end)
	-- GuideManager.Instance1RegisterGetGuideUi("DreamNotesBtn", function()
	-- 	return self:GetGuideUi(ActType.DreamNotes)
    -- end)
end

function ActiveDailyView:GetGuideUi(act_type)
	return self.List:GuideVertical(function(data) return data.act_type == act_type end,2)
end

function ActiveDailyView:CheckAnim()
    if self.ShowTimer == nil and ActiveData.IsListAnim == nil then
        self.ShowTimer = Invoke(function()
            self.ListAnim:Play()
            ActiveData.IsListAnim = true
        end,0.1)
    end
end

function ActiveDailyView:RightDownFlush()
    self.BtnRoller:SetActive(self.data:IsShowRoller())
    self.ObjAllFinish:SetActive(self.data:IsShowAllFinish())
end

function ActiveDailyView:OnClickRoller()
    ViewMgr:OpenViewByKey(Mod.ServerActivity.LuckRoller)
end

function ActiveDailyView:OnAllFinishClick()
    ViewMgr:OpenView(ActiveDailyAllFinishView)
end

----------------------------ActiveDailyItem----------------------------
ActiveDailyItem = ActiveDailyItem or DeclareMono("ActiveDailyItem", UIWidgetBaseItem)

function ActiveDailyItem:SetData(data)
    local act_type = data.act_type
    UH.SetText(self.Name,data.desc)
    UH.RenameGame(self,Format("Item_%s",data.index))
    self.Grid:SetData(ActiveData.Instance:GetItemReardData(data))
    self.act_info = ActivityData.Instance:GetActivityInfo(act_type)
    local is_open = self:FlushItemView(ActivityData.GetConfig(act_type),data) 
    -- 特殊类型需要使用特殊类型的进度检查和领取情况检查
    if data.is_special == 1 then 
        UH.SetText(self.Times,ActiveData.Instance:GetDailyCondition(act_type))
        local reward_flag = ActivityData.Instance:GetSpecialIsGetReward(data.type)
        self.BtnEffect:SetActive(self.act_info:IsComplete() and not reward_flag)
        
        self.Complete:SetActive(is_open and (self.act_info and self.act_info:IsComplete()) and reward_flag)
        self.Mark:SetActive(is_open and (self.act_info and self.act_info:IsComplete()) and reward_flag)
    elseif data.need_count == 0 then
        UH.SetText(self.Times,ActiveData.Instance:GetDailyCondition(act_type))
        self.Complete:SetActive(is_open and (self.act_info and self.act_info:IsComplete()))
        self.Mark:SetActive(is_open and (self.act_info and self.act_info:IsComplete()))
        self.BtnEffect:SetActive(false)
    else
        local FlagType = ActiveConfig.FlagType
        UH.SetText(self.Times,self:GetProgressTimes(data))
        local flag = ActivityData.Instance:GetActiveFlag(data.type)
        --LogError("type==",data.type,"flag===",flag,data)
        self.BtnEffect:SetActive(is_open and flag == FlagType.CanReward)
        self.Complete:SetActive(is_open and flag == FlagType.Rewarded and data.need_goto == 0)
        self.Mark:SetActive(is_open and flag == FlagType.Rewarded and data.need_goto == 0)
    end
    self.BtnInter.Interactable = is_open

    self.Time:CloseCountDownTime()
    self.BaseItem:SetActive(data.act_type == ActType.Hint)
    if data.act_type == ActType.Hint then
        self.Time:StampTime(AnecdoteHintData.Instance:EndTimeStamp(),
        TimeType.Type_Special_4,Language.Anecdote.Hint.TimeCounts)
    end
    self.RedPiont:SetNum(AnecdoteHintData.IsHasRemind(data) and 1 or 0)
    UIWidgetBaseItem.SetData(self,data)
end

function ActiveDailyItem:FlushItemView(config,data)
    UH.SetIcon(self.Icon,config.icon_id,ICON_TYPE.ACT)
    UH.SpriteName(self.Label,ActivityConfig.LabelName[config.title])
    UH.SetText(self.LabelName,Language.Activity.LabelName[config.title])
    self.LabelList:SetData(ActiveData.Instance:GetLabelList(config.act_type))
    local is_enough,limit_level = ActivityData.Instance:IsNoEnoughLevel(config.act_type,true)
    local is_open_btn,open_btn_name = self.act_info:CustomButtonNameFunc()
    local is_open_day = RoleData.Instance:CreatedDay() >= data.open_day
    UH.SetText(self.BtnName,is_enough and limit_level or Language.Active.BtnName)
    if not is_enough and not is_open_day then
        UH.SetText(self.BtnName,Format(Language.Activity.OpenDayTime[1],DataHelper.GetDaXie(data.open_day)))
        if data.open_day - RoleData.Instance:CreatedDay() == 1 then
            UH.SetText(self.BtnName,Language.Activity.OpenDayTime[2])
        end
    end
    if not is_enough and open_btn_name ~= nil then
        UH.SetText(self.BtnName,open_btn_name)
        return not is_enough and is_open_btn
    end
    return not is_enough and is_open_day
end

function ActiveDailyItem:GetProgressTimes(data)
    local OtherTimes = Language.Active.OtherTimes[data.act_type]
    local num = ActivityData.Instance:GetProgressNum(data.type)
    return Format(OtherTimes or Language.Active.Progress,num,ActiveData.Instance:GetDailyNeedCount(data))
end

function ActiveDailyItem:OnClickGo()
    if FightCtrl.IsActFB() then
        return
    end
    if not self.BtnInter.Interactable and ActivityData.Instance:IsNoEnoughLevel(self.data.act_type) then
        return
    end
    if not self.BtnInter.Interactable then
        if self.data.act_type == ActType.Prestige or self.data.act_type == ActType.ResearchTask then
            PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTips)
            return
        end
    end
    GuideData.Instance:StopTaskBoxGuide()
    if self.act_info and self.act_info.click_handle then
        if self.act_info.click_handle(ActiveDailyView) then
            return
        end
    end
    ViewMgr:CloseView(ActiveView)
    ActivityData.Instance:SetOpenRemind(self.data.act_type)
end

function ActiveDailyItem:OnClickItem()
    ItemInfoCtrl.Instance:ActivityInfoView{info = self.act_info}
end

function ActiveDailyItem:OnClickReward()
    if self.data.is_special == 0 then 
        ActiveCtrl.Instance:SendExtraRewarrdFetch(self.data.type)
    else 
        ActivityCtrl.Instance:SendActiveSystemReq(3,self.data.type)
    end 
    -- 
end

function ActiveDailyItem:GetActType()
    return self.data.act_type
end

----------------------------ActivityViewDailyRewardItem----------------------------
ActivityViewDailyRewardItem = ActivityViewDailyRewardItem or DeclareMono("ActivityViewDailyRewardItem", UIWidgetBaseItem)

function ActivityViewDailyRewardItem:SetData(data)
    if IsEmpty(data) then
        BuglyReport(ActivityData.Instance:GetActiveLevel() .. "级的活跃奖励没有配置")
        return
    end
	data.item = self:GetItemData(data)
	self.Call:SetData(data.item)
	UH.SetText(self.Num,data.integral_active)
	
	local not_reward = ActivityData.Instance:GetActiveRewardFlag(data.seq) == 0
    local can_reward = ActivityData.Instance:GetActiveIntegralNum() >= data.integral_active
    self.Mark:SetActive(not not_reward)
	self.Effect:SetActive(not_reward and can_reward)
	self.Label:SetActive(ActivityData.Instance:IsActiveActReward(data.seq)and not_reward)
	UIWidgetBaseItem.SetData(self, data)	
end

function ActivityViewDailyRewardItem:OnClickItem()
	local list,is_show_activity = ActivityData.Instance:GetActiveRewardList(self.data.reward_item,self.data.seq)
	if is_show_activity == true or #list > 1 then
		ViewMgr:OpenView(SilkRoadPreview,{reward_list = list})
	else
		CellClicks.BagGridNoButton({GetData = function()
			return self.data.item
		end})
	end
end

function ActivityViewDailyRewardItem:OnClickReward()
	ActivityCtrl.Instance:SendActiveSystemReq(1,self.data.seq - 1)
end

function ActivityViewDailyRewardItem:GetItemData(data)
	if data == nil then return Item.New() end
	for k,v in pairs(data.reward_id or {}) do
		return Item.New(v)
	end
	if data.exp > 0 then
		return Item.New({item_id = CommonItemId.Exp,num = data.exp})
	end
    if data.coin > 0 then
		return Item.New({item_id = CommonItemId.Coin,num = data.coin})
    end
	if data.gold > 0 then
		return Item.New({item_id = CommonItemId.Gold,num = data.gold})
	end
end

--=========================ActivityViewActTypeItem================================
ActivityViewActTypeItem = DeclareMono("ActivityViewActTypeItem", UIWidgetBaseItem)
function ActivityViewActTypeItem:ActivityViewActTypeItem()
	self.config = ActivityConfig.ActivityActType
end

function ActivityViewActTypeItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.TypeName.text = data.label_name
	self.TypeName.color = self.config.label[data.label_color]
	self.TypeNameOutline.effectColor = self.config.outline[data.label_color]
	self.BgSp.SpriteName = self.config.bg_sp[data.label_color]
	local len = string.len(data.label_name) --解决闪动问题
	self.BGRict.sizeDelta = Vector2.New(len > 6 and 80 or 60,30)
end