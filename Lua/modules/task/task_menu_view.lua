
TaskMenuView = TaskMenuView or DeclareMono("TaskMenuView", UIWFlushPanelGroup)

function TaskMenuView:TaskMenuView()
	self.data = TaskCtrl.Instance:Data()

	self.data_cares = {
        {data = self.data:GetTaskList(), func = self.FlushMenu},
		-- {data = self.data.cur_sel_task, func = self.FlushMenu},
	}
end

function TaskMenuView:FlushMenu()
	local mainline = self.data:GetMainlineAccepted()
	if nil == mainline then
		self.NormalObj:SetActive(true)
		return
	end
	local special = type(mainline.info.schedule) == "number"
	self.SpecialObj:SetActive(special)
	self.NormalObj:SetActive(not special)
end

TaskMenuViewSpecial = TaskMenuViewSpecial or DeclareMono("TaskMenuViewSpecial", UIWFlushPanel)

function TaskMenuViewSpecial:TaskMenuViewSpecial()
	self.data = TaskCtrl.Instance:Data()

	self.data_cares = {
        {data = self.data:GetTaskList(), func = self.FlushMenuSpecial},
		-- {data = self.data.cur_sel_task, func = self.FlushMenuSpecial},
	}
end

function TaskMenuViewSpecial:FlushMenuSpecial()
	self.mainline = self.data:GetMainlineAccepted()
	if nil == self.mainline then return end
	local info = self.mainline.info
	local special = type(info.schedule) == "number"
	if not special then return end
	local vo = self.mainline.vo
	self.TaskSection.text = info.section
	self.Progress:SetProgress(info.schedule / 100)
	self.Progress:SetText(info.schedule .. "%")
	self.TaskDesc.text = self.mainline.task_desc
	self.TaskGoal.text = self.mainline.task_goal_menu or self.mainline.task_goal
end

function TaskMenuViewSpecial:OnClickContinue()
	-- TaskCtrl.Instance:GoTask(self.mainline)
	AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(self.mainline)}, 1)
end

TaskMenuViewNormal = TaskMenuViewNormal or DeclareMono("TaskMenuViewNormal", UIWFlushPanel)

function TaskMenuViewNormal:TaskMenuViewNormal()
	self.data = TaskCtrl.Instance:Data()

	self.data_cares = {
        {data = self.data:GetTaskList(), func = self.FlushMenuNormal},
        {data = self.data.task_trace_list, func = self.FlushMenuNormal},
		{data = RoleData.Instance:GetBaseData(), func = self.FlushMenuNormal, init_call = false, keys = {"level"}},
		{data = RoleData.Instance:GetBaseData(), func = self.FlushMenuGuide, init_call = false, keys = {"level"}},
		{data = self.data.memu_effect_pos, func = self.FlushEffectCompelete},
		{data = self.data.menu_guide, func = self.FlushMenuGuide},
		{data = self.data.menu_to_task, func = self.FlushToTask, init_call = false},
		{data = self.data.task_wait_list, func = self.FlushMenuNormal, init_call = false},
		{data = GuideData.Instance.rolling_task_box, func = self.StartRollingList, init_call = false, keys = {"start"}},
	}

	GuideManager.Instance:RegisterGetGuideUi("MainTask", function ()
		return self.TaskList.index_item[1].gameObject, nil
	end)
	GuideManager.Instance:RegisterGetGuideUi("SecondTaskItem", function ()
		return self.TaskList.index_item[2].gameObject
	end)
	GuideManager.Instance:RegisterGetGuideUi("TaskBox", function ()
		return self.TaskBox
	end)
	self.task_accepted_list = {}
end
function TaskMenuViewNormal:Awake()
	UIWFlushPanel.Awake(self)
	if self.role_pos_event == nil then
		self.role_pos_event = EventSys:Bind(GlobalEventKeys.RolePosChange, BindTool.Bind(self.ResetMenuGuide, self))
	end
end

function TaskMenuViewNormal:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	GuideManager.Instance:UnRegisterGetGuideUi("MainTask")
	GuideManager.Instance:UnRegisterGetGuideUi("TaskBox")
	GuideManager.Instance:UnRegisterGetGuideUi("SecondTaskItem")
	if self.role_pos_event then
		EventSys:UnBind(self.role_pos_event)
		self.role_pos_event = nil
	end
	TimeHelper:CancelTimer(self.timer_dft)
end

function TaskMenuViewNormal:ResetMenuGuide()
	self.data.menu_guide.pos = nil
end

function TaskMenuViewNormal:FlushMenuNormal()
	self.task_accepted_list = self.data:GetTaskListAcceptedFollowMenu()
	self.TaskList:ClearSmData()
	self.TaskList:SetDataList(self.task_accepted_list)
end

function TaskMenuViewNormal:FlushEffectCompelete()
	if self.data.memu_effect_pos.val > 0 then
		local temp_pos = self.data.memu_effect_pos.val
		TimeHelper:AddDelayTimer(function ()
			if self.TaskList.index_item[temp_pos] then
				self.TaskList.index_item[temp_pos]:CompleteEffect()
				return true
			end
		end, 0.5)
		self.data.memu_effect_pos.val = 0
	end
end

function TaskMenuViewNormal:FlushMenuGuide()
	local top = self.BaseList.sizeDelta.y
	top = top ~= 0 and top + 4 or top
	local menu_guide = self.data.menu_guide
	local value = menu_guide.val > 0 and menu_guide.arrow and nil ~= menu_guide.pos and menu_guide.is_show
	if value then
		self.GuideDesc.text = menu_guide.desc
		self.GuidePos.position = Vector3.New(self.GuidePos.position.x, menu_guide.pos.y, self.GuidePos.position.z)
	end
	self.GuidePos:SetObjActive(value)
	if self.LayoutGroup.padding.top ~= top then
		self:FlushMenuNormal()
	end
	self.LayoutGroup.padding.top = top
end

function TaskMenuViewNormal:OnListPressDown()
	self.data.menu_guide.pre_val = self.data.menu_guide.val
	self.data.menu_guide.val = 0
end

function TaskMenuViewNormal:StartRollingList()
	if GuideData.Instance.rolling_task_box.start == true then
		local pos = self.NContent.localPosition
		pos = Vector3.New(pos.x, 0, 0)
		local endY = self.NContent.sizeDelta.y
		local end_pos = Vector3.New(pos.x, endY, 0)
		local frame = 0
		local total = 36
		local half = total/2
		TimeHelper:CancelTimer(self.handle_rolling)
		self.handle_rolling = TimeHelper:AddRunFrameTimer(function()
			if frame > half then
				self.NContent.localPosition = Vector3.Lerp(end_pos, pos, (frame - half) / half)
			else
				self.NContent.localPosition = Vector3.Lerp(pos, end_pos, frame / half)
			end
			frame = frame + 1
			if frame >= total then
				GuideManager.Instance.click_btn:OnClick()
			end
		end, 1, total)
	else
		TimeHelper:CancelTimer(self.handle_rolling)
	end
end

function TaskMenuViewNormal:FlushToTask()
	local task_id = self.data.menu_to_task.to_task_id
	for index, task in ipairs(self.task_accepted_list) do
		if task.task_id == task_id then
			self.timer_dft = TimeHelper:AddDelayFrameTimer(function ()
				self.TaskList:GuideVertical(index)
			end)
			return
		end
	end
end

TaskMenuViewNormalItem = DeclareMono("TaskMenuViewNormalItem", UIWidgetBaseItem)
function TaskMenuViewNormalItem:TaskMenuViewNormalItem()
	self.task_data = TaskCtrl.Instance:Data()
	self.language = Language.Task.Menu
	self.task_type = Language.Task.TaskType
	self.task_state = Language.Task.TaskState
end

function TaskMenuViewNormalItem:ExpendSize(data, axis)
	self.TaskDesc.text = data:TaskGoal(true)
    local extra_height1 = (TaskConfig.TaskType.challenge == data.task_type and 107) or (TaskConfig.TaskType.team_challenges == data.task_type and 77) or 0
	local extra_height2 = data:TaskMenuConditionPause() and -20 or 0
    return self.TaskDesc.preferredHeight + (data:TaskMenuCondition() and 91 or 50) + extra_height1 + extra_height2
end

function TaskMenuViewNormalItem:OnDestroy()
	TimeHelper:CancelTimer(self.timer_ct28)
	TimeHelper:CancelTimer(self.timer_ing_eff)
	TimeHelper:CancelTimer(self.StampTimer)
	if self.finger_eff_handle ~= nil then
		self.EffectTool:Stop(self.finger_eff_handle)
		self.finger_eff_handle = nil
	end
	if self.highlight_eff_handle ~= nil then
		self.EffectTool:Stop(self.highlight_eff_handle)
		self.highlight_eff_handle = nil
	end
	self:uncareData()
end

function TaskMenuViewNormalItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	self:uncareData()
	self.care_handles = {}
	self:careData(self.task_data.menu_guide, BindTool.Bind(self.FlushArrow, self))
	self:careData(self.task_data.task_ing_eff, BindTool.Bind(self.ShowIngEffect, self))
	if data.task_type == TaskConfig.TaskType.mainline then
		if data.mainline_need_click and data.info.manual_status == 1 then
			if self.highlight_eff_handle == nil then
				self.highlight_eff_handle = self.EffectTool:Play(3164053)
			end
		elseif self.highlight_eff_handle ~= nil then
			self.EffectTool:Stop(self.highlight_eff_handle)
			self.highlight_eff_handle = nil
		end
		if data.info.finger_effect == 1 then
			if self.finger_eff_handle == nil then
				self.finger_eff_handle = self.EffectTool:Play(3164054)
			end
		elseif self.finger_eff_handle ~= nil then
			self.EffectTool:Stop(self.finger_eff_handle)
			self.finger_eff_handle = nil
		end
	else
		if self.finger_eff_handle ~= nil then
			self.EffectTool:Stop(self.finger_eff_handle)
			self.finger_eff_handle = nil
		end

		if self.highlight_eff_handle ~= nil then
			self.EffectTool:Stop(self.highlight_eff_handle)
			self.highlight_eff_handle = nil
		end
	end
	local task_goal = data:TaskGoal(true)
	self.XinObj:SetActive(data:IsNew())

	self.TaskTypeSp.SpriteName = data.skin_type and TaskConfig.TaskMenuSkin[data.skin_type] or TaskConfig.TaskMenuTypeSp[data.task_type]

	self.TaskName.text = data:MenuTitle()--"["..data.task_id.."]"..data:MenuTitle()
	self.TaskDesc.text = task_goal
	self.TaskState.text = data:TaskStateLabel()
	local show_c, condition = data:TaskMenuCondition()
	self.TaskCondition:SetObjActive(show_c)
	self.TaskCondition.text = condition
	-- self.ItemPos.sizeDelta = Vector2.New(264, show_c and 90 or 62)
	self.show_r = TaskConfig.TaskType.mainline == self.data.task_type and (show_c or data:TaskMenuConditionPause()) and nil ~= data.reward_show
	self.RewardCell:SetObjActive(self.show_r)
	self.RewardListObj:SetActive(TaskConfig.TaskType.challenge == data.task_type or TaskConfig.TaskType.team_challenges == data.task_type)
	if self.show_r then
		self.RewardCell:SetData(data.reward_show)
	end
	self.RewardList:SetData(data.reward_list or {})
	-- self.BtnUpgradePreviewObj:SetActive(TaskConfig.TaskType.upgrade == data.task_type)
	self.BtnLiveProp:SetActive(TaskConfig.TaskType.live_prop == data.task_type and data.show_attri_btn == true)
	self.Btn721Obj:SetActive(721 == data.task_id)
	self.ExpObj:SetActive(data:ShowExpObj())
	self.BtnBranchTaskObj:SetActive(TaskConfig.TaskType.branch_serial == data.task_type or TaskConfig.TaskType.branch_marbas == data.task_type or
     TaskConfig.TaskType.branch_relic == data.task_type or TaskConfig.TaskType.team_challenges == data.task_type)
    if TaskConfig.TaskType.branch_serial == data.task_type or TaskConfig.TaskType.branch_marbas == data.task_type or TaskConfig.TaskType.branch_relic == data.task_type then
        UH.SetText(self.BranchTaskText, Language.Task.Menu.BranchTask.zj)
        local co = Config.tasklist_auto.chain_go_on[data.task_id]
        local role_level = RoleData.Instance:GetRoleLevel()
        self.red_point_branch_task = self.red_point_branch_task or UiRedPoint.New(self.BtnBranchTaskObj, Vector2.New(25, 25))
        self.red_point_branch_task:SetNum((co and 1 == co.turn and data.min_level <= role_level) and 1 or 0)
    elseif TaskConfig.TaskType.team_challenges == data.task_type then
        UH.SetText(self.BranchTaskText, Language.Task.Menu.BranchTask.hs)
        if self.red_point_branch_task ~= nil then
            self.red_point_branch_task:SetNum(0)
        end
    end
	if TaskConfig.TaskType.mainline == self.data.task_type then
		-- or TaskConfig.TaskType.branch == self.data.task_type 
		-- or TaskConfig.TaskType.branch_serial == self.data.task_type 
		-- or TaskConfig.TaskType.reco == self.data.task_type 
		-- or TaskConfig.TaskType.bounty == self.data.task_type 
		-- or TaskConfig.TaskType.upgrade == self.data.task_type then
		-- TimeHelper:AddRunFrameTimer(function ()
		-- 	self:Guide()
		-- end)
		TimeHelper:AddDelayTimer(function()
			self:Guide()
		end,3)
	end
	TimeHelper:CancelTimer(self.timer_ct28)
	TimeHelper:CancelTimer(self.StampTimer)
	if data:TaskMenuConditionPause() then
		local time_24 = TimeCtrl.Instance:ChangDataTime24(TimeHelper.GetServerTime())
		local task_goal28 = string.gsub(task_goal, "time_next_day", "%%s")
		self.timer_ct28 = TimeHelper:AddCountDownCT(function ()
			local ft = TimeHelper.FormatDHMS(time_24 - TimeHelper.GetServerTime())
			self.TaskDesc.text = string.format(task_goal28, string.format(self.language.goal28, ft.hour, ft.min))
		end, function ()
			self.TaskDesc.text = string.format(task_goal28, string.format(self.language.goal28, 0, 0))
		end, time_24)
	elseif data.IsLevelEnough and not data:IsLevelEnough() and data.task_id == TaskData.Instance:GetOtherCfg().special_dec_task_id then
		UH.SetText(self.TaskDesc, task_goal)
	else
		local stamp_time = data:TaskStampTime()
		if stamp_time and stamp_time ~= 0 then
			self:FlushTaskStampTime(stamp_time,task_goal,data)
		else
			self.TaskDesc.text = task_goal
		end
	end

	self.RecommendScore:SetObjActive(TaskConfig.TaskType.challenge == data.task_type)
	if TaskConfig.TaskType.challenge == data.task_type then
		local recommend_score = ChallengeTaskData.Instance:GetWordTaskRecommendScore(data.task_id)
		self.RecommendScore.text = string.format(self.language.RecommendScore, RichTextHelper.ColorStr(RoleData.Instance:GetSumScore(), RoleData.Instance:GetSumScore() < recommend_score and COLORSTR.Red8 or COLORSTR.Green4), recommend_score)
	end
	self.RedPoint:SetNum(0)
	if TaskConfig.TaskType.peak_road == data.task_type then
		self.PeakObj:SetActive(true)
		self:careData(PeakRoadData.Instance:PeakData(), BindTool.Bind(self.FlushRedPoint, self))
	else
		self.PeakObj:SetActive(false)
	end

	if TaskConfig.TaskType.task_chain == data.task_type and data.reward_task_chain then
		self.TaskChainReward:SetData(data.reward_task_chain)
		self.TaskChainReward:SetObjActive(true)
	else
		self.TaskChainReward:SetObjActive(false)
	end
end

function TaskMenuViewNormalItem:FlushTaskStampTime(stamp_time,task_goal,data)
	self.StampTimer = TimeHelper:AddCountDownCT(function ()
		local total_time = stamp_time - TimeHelper.GetServerTime()
		local t = TimeHelper.FormatDHMS(total_time)
		UH.SetText(self.TaskDesc,data:GetTimeDesc(t, task_goal))
	end, function ()
		UH.SetText(self.TaskDesc,data:GetTimeDesc({}, task_goal, true))
		data:TaskTimeEnd()
	end, stamp_time)
end

function TaskMenuViewNormalItem:FlushRedPoint()
	if TaskConfig.TaskType.peak_road == self.data.task_type then
		self.RedPoint:SetNum(PeakRoadData.Instance:RewardCanGet())
	end
end
function TaskMenuViewNormalItem:FlushArrow()
	if self.show_r then
		self.RewardCell:SetObjActive(self.show_r and self.task_data.menu_guide.arrow)
	end
	-- self.BtnUpgradePreviewObj:SetActive(self.task_data.menu_guide.arrow and TaskConfig.TaskType.upgrade == self.data.task_type)
end

function TaskMenuViewNormalItem:Guide()
	if 0 == self.task_data.menu_guide.val then
		self.task_data.menu_guide.pos = nil
		return
	end
	local task = self.task_data.task_list[self.task_data.menu_guide.val]
	if nil == task then
		self.task_data.menu_guide.pos = nil
		return
	end
	-- if TaskConfig.TaskType.bounty == task.task_type then
	-- 	if TaskConfig.TaskType.bounty == self.data.task_type then
	-- 		self.task_data.menu_guide.desc = Language.Task.GuideDesc.Other[self.data.task_type] or Language.Task.GuideDesc.Other.default
	-- 		self.task_data.menu_guide.pos = self.ItemPos.position
	-- 	end
	-- 	return
	-- end
	if task:TaskMenuConditionPause() then
		if TaskConfig.TaskType.branch_serial == self.data.task_type then
			self.task_data.menu_guide.desc = Language.Task.GuideDesc.Other[self.data.task_type] or Language.Task.GuideDesc.Other.default
			self.task_data.menu_guide.pos = self.ItemPos.position
		end
	elseif task:IsLevelEnough() then
		if TaskConfig.TaskType.mainline == self.data.task_type and not self.data:TaskMenuConditionPause() then
			self.task_data.menu_guide.desc =  Language.Task.GuideDesc.Main
			self.task_data.menu_guide.pos = self.ItemPos.position
		end
	else
		if TaskConfig.TaskType.branch == self.data.task_type
			or TaskConfig.TaskType.reco == self.data.task_type 
			or TaskConfig.TaskType.bounty == self.data.task_type 
			or TaskConfig.TaskType.upgrade == self.data.task_type then
			self.task_data.menu_guide.desc = Language.Task.GuideDesc.Other[self.data.task_type] or Language.Task.GuideDesc.Other.default
			self.task_data.menu_guide.pos = self.ItemPos.position
		end
	end
end
function TaskMenuViewNormalItem:OnPressItem()
	if self.data.task_type == TaskConfig.TaskType.mainline then
		ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.MainTask)
	end
end
function TaskMenuViewNormalItem:OnClickItem()
	if nil == self.data then return end
	local scene_mode = SceneData.Instance:GetMode()
	if SceneModeType.None ~= scene_mode and SceneModeType.FightFly2 ~= scene_mode then
        PublicPopupCtrl.Instance:Center(Language.Chat.BackActTips)
        return
    end
	self.data:IsNew(false)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	if TaskConfig.TaskType.silk_road == self.data.task_type then
		ViewMgr:OpenView(SilkRoadView)
		return
	elseif SceneData.Instance.change_scene_effect.id ~= 0 then
		return
	end
	AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(self.data, true)}, 1)
end

function TaskMenuViewNormalItem:OnClickUpgradePreview()
	ViewMgr:OpenView(RolePromoteView)
end

function TaskMenuViewNormalItem:OnClickBranchTask()
	if TaskConfig.TaskType.branch_serial == self.data.task_type then
		ViewMgr:OpenView(BranchSerialTaskView)
	elseif TaskConfig.TaskType.branch_marbas == self.data.task_type then
		ViewMgr:OpenView(BranchMarbasTaskView)
    elseif TaskConfig.TaskType.team_challenges == self.data.task_type then
        ViewMgr:OpenView(ChallengeTaskView, {open_param = 3})
    else
		ViewMgr:OpenView(BranchRelicTaskView)
	end
end

function TaskMenuViewNormalItem:OnClickLiveProp()
	ViewMgr:OpenView(LiveAttriPreview)
end

function TaskMenuViewNormalItem:OnClick721()
	VideoCtrl.Instance:StartVideoById(1)
end

function TaskMenuViewNormalItem:CompleteEffect()
	self.FinishSize.localScale = Vector3.New(1, self.ItemPos.sizeDelta.y / 90, 1)
	if self.EffectTool.isActiveAndEnabled then
		self.EffectTool:Play(10010280)
		self.EffectTool:Play(10010303)
	end
end

function TaskMenuViewNormalItem:ShowIngEffect()
	local is_show = self.task_data.task_ing_eff.val == self.data.task_id
	TimeHelper:CancelTimer(self.timer_ing_eff)
	if is_show then
		self.timer_ing_eff = TimeHelper:AddRunFrameTimer(function ()
			self.IngEffect.localScale = Vector3.New(1, self.ItemPos.sizeDelta.y / 90, 1)
		end)
	end
	self.IngEffect:SetObjActive(is_show)
end

TaskMenuViewNormalItemReward = TaskMenuViewNormalItemReward or DeclareMono("TaskMenuViewNormalItemReward")

function TaskMenuViewNormalItemReward:OnEnable()
	self.ListContent.localPosition = Vector2.New(0, 0)
end

AdventureTaskMenuView = AdventureTaskMenuView or DeclareMono("AdventureTaskMenuView", UIWFlushPanel)
--InviteFightObj=========GameObject
--InviteFightTime========UIWTimeMeter
--InviteFightTimeObj=====GameObject

function AdventureTaskMenuView:AdventureTaskMenuView()
	self.data = TaskCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data.menu_adventure, func = self.FlushMenuAdventure, init_call = false},
		{data = LoginData.Instance:BaseData(),func = self.FlushMenuAdventure, keys = {"login_state"}},		--切换账号时没有刷新
		{data = InviteFightData.Instance:Base(), func = self.FlushInviteFight, init_call = true},
		{data = LiveData.Instance.prop_use_all_info, func = self.FlushLiveState, init_call = true},
	}
end

function AdventureTaskMenuView:FlushMenuAdventure()
	if LoginData.Instance:LoginState() ~= LoginStateType.Logined then
		self.AdventureObj:SetActive(false)
		TimeHelper:CancelTimer(self.timer_ct)
		return
	end
	local task = self.data.menu_adventure:Val()
	local expire_time = task.expire_time
	local value = nil ~= next(task)
	self.AdventureObj:SetActive(value)
	if not value then return end
	TimeHelper:CancelTimer(self.timer_ct)
	self.timer_ct = TimeHelper:AddCountDownCT(function ()
		local last_time = expire_time - TimeHelper.GetServerTime()
		local ft = TimeHelper.FormatDHMS(last_time)
		self.Time.text = string.format("%02d:%02d", ft.min, ft.s)
	end, function ()
		self.Time.text = "00:00"
	end, expire_time, 1)
end

function AdventureTaskMenuView:FlushLiveState()
	local is_proping = LiveData.Instance:IsProping()
	self.LiveState:SetActive(is_proping)
end

function AdventureTaskMenuView:OnEnable()
	self:FlushMenuAdventure()
	self:FlushLiveState()
end

function AdventureTaskMenuView:OnDisable()
	TimeHelper:CancelTimer(self.timer_ct)
end

function AdventureTaskMenuView:OnClickAdventure()
	self.data.cur_sel_accpeted = self.data.menu_adventure:Val()
	ViewMgr:OpenViewByKey(Mod.MainTask.Task)
end

function AdventureTaskMenuView:FlushInviteFight()
	local cur_state = InviteFightData.Instance:CurState()
	if cur_state == InviteFightData.State.Start or 
		cur_state == InviteFightData.State.End then
		self.InviteFightObj:SetActive(false)
		return
	end
	self.InviteFightObj:SetActive(true)
	if cur_state == InviteFightData.State.CanBegin then
		self.InviteFightTimeObj:SetActive(false)
	else
		self.InviteFightTimeObj:SetActive(true)
		-- if cur_state == InviteFightData.State.Fighting then
		self.InviteFightTime:StampTime(InviteFightData.Instance:EndTime(),
			TimeType.Type_Special_0,nil,Language.InviteFight.CountDownComplete)
		
		-- end
	end
end

function AdventureTaskMenuView:OnClickInviteFight()
	if InviteFightData.Instance:CurState() == InviteFightData.State.CanBegin then
		InviteFightData.Instance:MarkWaitData()
		InviteFightCtrl.Instance:BeginFightReq()
	else
		ViewMgr:OpenView(InviteFightView)
	end
end

function AdventureTaskMenuView:OnLivePropClick()
	ViewMgr:OpenView(LiveAttriPreview)
end

TaskMenuViewNormalItemRewardTaskChain = DeclareMono("TaskMenuViewNormalItemRewardTaskChain", UIWidgetBaseItem)

function TaskMenuViewNormalItemRewardTaskChain:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	UH.SetIcon(self.IconSp, data.IconId and data:IconId() or 0)
	self.QuaSp.SpriteName = TaskConfig.TASK_CHAIN_COLOR_FRAME_SPRITE[data:Color()]
end

function TaskMenuViewNormalItemRewardTaskChain:OnClickCell()
	local data = self.data
	if data == nil then return end
	local info = {
		item_list = {data},
		pos = Vector3.New(0, 140, 0)
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

TaskMenuViewMarryMail = TaskMenuViewMarryMail or DeclareMono("TaskMenuViewMarryMail", UIWFlushPanel)

function TaskMenuViewMarryMail:TaskMenuViewMarryMail()
	self.data = MarryCtrl.Instance:Data()

	self.data_cares = {
		{data = SceneData.Instance:Base(), func = self.FlushBtnMail, keys = {"scene_mode"}},
        {data = self.data.wedding_status_info, func = self.FlushBtnMail, init_call = false},
		{data = RoleData.Instance.base_data, func = self.FlushBtnMail, keys = {"level"}, init_call = false},
	}
end

function TaskMenuViewMarryMail:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.timer_dt)
end

function TaskMenuViewMarryMail:FlushBtnMail()
	TimeHelper:CancelTimer(self.timer_dt)
	local is_show = false
	local cur_mode = SceneData.Instance:GetMode()
	if SceneModeType.None == cur_mode then
		local info = self.data.wedding_status_info
		if not info.init then
			is_show = TimeHelper.GetServerTime() < info.finish_time and RoleData.Instance:GetRoleLevel() >= self.data:ConfigOtherPartakeLevelLimit()
			if is_show then
				self.timer_dt = TimeHelper:AddDelayTimer(function ()
					self.data.wedding_status_info.init = true
				end, info.finish_time - TimeHelper.GetServerTime())
			end
		end
	end
	self.BtnMailObj:SetActive(is_show)
end

function TaskMenuViewMarryMail:OnClickMail()
	MarryCtrl.Instance:SendWeddingFBReqWeddingShowInfo(self.data.wedding_status_info.wedding_id)
end
