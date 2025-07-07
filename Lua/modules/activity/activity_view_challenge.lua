
ActivityViewChallenge = ActivityViewChallenge or DeclareMono("ActivityViewChallenge", UIWFlushPanel)

function ActivityViewChallenge:ActivityViewChallenge()
	self.data = ActivityCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data:GetActivityList(), func = self.FlushActList},
	}
	GuideManager.Instance:RegisterGetGuideUi("ArenaSelectBtn", function()
		if self.ChallengeList and self.ChallengeList:GetItem(1) then
			return self.ChallengeList:GetItem(1).BtnGo
		end
	end)
	GuideManager.Instance:RegisterGetGuideUi("LeiTaiZhanBtn", function()
		if self.ChallengeList and self.ChallengeList:GetItem(3) then
			return self.ChallengeList:GetItem(3).BtnGo
		end
	end)
	self.ChallengeList:SetCompleteCallBack(function ()
		local notice_list_data = self:GetNiticeListData()
		self.NoticeList:SetData(notice_list_data)
	end)
end

function ActivityViewChallenge:FlushActList()
	local challenge_list = self.data:GetActivityListByGroup(ActivityConfig.ActivityGroupType.challenge)
	self.ChallengeList:SetData(challenge_list)
end

-- 由于临时加的紧急需求，先写死，只是预告
function ActivityViewChallenge:GetNiticeListData()
	local list = {}
	local notice_1 = {
		name = Language.ArenaContestNotice.Title1,
		cross_type = 1,
		icon = "a8888",
		condition_tip = Language.ArenaContestNotice.Condition1,
		notice_tip = Language.ArenaContestNotice.NoticeTip,
		btn_name = Language.ArenaContestNotice.BtnName1,
		effect_id = "3161079",
		enter_func = function ()
			ViewMgr:OpenView(ContestHeroNotice)
		end
	}
	local notice_2 = {
		name = Language.ArenaContestNotice.Title2,
		cross_type = 2,
		icon = "a9999",
		condition_tip = Language.ArenaContestNotice.Condition2,
		notice_tip = Language.ArenaContestNotice.NoticeTip,
		btn_name = Language.ArenaContestNotice.BtnName1,
		effect_id = "3165097",
		enter_func = function ()
			ViewMgr:OpenView(ContestMeetingNotice)
		end
	}
	-- 已逐渐实装，不需要了
	-- table.insert(list, notice_1)
	-- table.insert(list, notice_2)
	table.insert(list, {})
	return list
end

----------------------------ChallengeItem----------------------------
ChallengeItem = ChallengeItem or DeclareMono("ChallengeItem", UIWidgetBaseItem)
function ChallengeItem:ChallengeItem()
	self.language = Language.Activity.ActItem
end

function ChallengeItem:SetData(data)
	if data.info == nil then
		self.Blank:SetActive(true)
		return
	end
	UIWidgetBaseItem.SetData(self, data)
	-- 跨服标记
	self.CrossServerFlag:SetActive(data.info ~= nil and data.info.cross_service == 1)
	self.AllCrossServerFlag:SetActive(data.info.cross_service == 2)
	local reward_list = self:GetRewardList(data.reward_list)
	local is_future = not data.group_info.is_in_future
	UH.SetIcon(self.Icon, data.info.icon_id, ICON_TYPE.ACT)
	UH.SetText(self.Times,is_future and self:GetLimitNum(data) or "")
	UH.SetText(self.TopConditionTipsTxt,self:GetTopConditionTip(data) or "")
	if data.info.act_type == ActType.CloudArena or data.info.act_type == ActType.WorldArena 
	or ActType.HeroVanBattle == data.info.act_type 
	or ActType.HeroVanBattleAgain == data.info.act_type
	or ActType.UTArena == data.info.act_type then	--穹顶和九州论剑和英雄会武和天下第二都用同样的背景图，跟幻界战场一样
		UH.SpriteName(self.ImageBG,"a1032BG")
	else
		UH.SpriteName(self.ImageBG,data.info.icon_id .. "BG")
	end
	--显示图标
	local is_finish = ActStatusType.Open == data.status and self:IsTimeComplete(data)
	local is_open = false
	if data.act_type == ActType.WorldArena then
		is_open = is_future
	elseif data.act_type == ActType.HeroVanBattle then
		is_open = true
	else
		is_open = is_future and ActStatusType.Close ~= data.status
	end
	self.is_open = is_future
	UH.Alpha(self.AlphaBG,is_future and is_finish and 0.5 or 1)
	self.Finish:SetActive(is_future and is_finish)
	self.EnterInter.Interactable = is_open
	self.Mark:SetActive(not is_open)
	--颜色改变
	local btn_state = self:GetBtnState(data,is_future)
	UH.SetText(self.Title,Format(self.language.StatuColor[is_open and 1 or 2],data.act_name))
	UH.SetText(self.Enter,is_open and btn_state or Format(self.language.StatuColor[2],btn_state))
	self.Grid:SetData(reward_list)
	--设置角标
	self:FlushLabelView(data.info.title,is_open)
	--注册红点
	if not self.is_care then
		self.is_care = true
		self.care_handles = {}
		self:careData(data.art_red_point, BindTool.Bind(self.ActRedPoint, self))
	end
	if self.red_point_act then
		self.red_point_act:SetNum(self.data.art_red_point.val and 1 or 0)
	end
	--显示特效
	if is_open and self.Effect.isActiveAndEnabled then
		-- TimeHelper:AddDelayTimer(function()
			if data.info.icon_id == "a1009" or data.act_type == ActType.HeroVanBattleAgain then
				UH.SpriteName(self.Icon,nil)
			end
			if not self.isActiveAndEnabled then
				local id = self:GetEffectId(data.act_type)
				self.Effect:Play(self:GetEffectId(data.act_type))
				self.isActiveAndEnabled = true
			end
        -- end, 0.5)
	end
	if data.act_type == ActType.Arena then
		self.NewReport:SetActive(ArenaData.Instance.has_new_report)
	end
	if data.act_type == ActType.ArenaMatch then
		self.NewReport:SetActive(ArenaMatchData.Instance:GetRecordRemindNum() > 0)
	end
	if data.act_type == ActType.CloudArena then
		if self.cloud_arena_smart_handle == nil then
			self.cloud_arena_smart_handle = CloudArenaData.Instance.mySelfInfo:KeysCare({"cur_score"}, BindTool.Bind2(self.SetData, self, self.data))
			CloudArenaCtrl.SendRealTimeInfo()
		end
	-- 	self.NewReport:SetActive(CloudArenaData.Instance.notice_smart.new_record)
	end
end

function ChallengeItem:GetEffectId(act_type)
	if Config.daily_activity_auto.effect_show[act_type] then
		return Config.daily_activity_auto.effect_show[act_type].effect_id or 0
	end
	return 0
end

--刷新角标
function ChallengeItem:FlushLabelView(title,is_open)
	if title > 0 then
		UH.SetText(self.LabelName,Language.Activity.LabelName[title])
		UH.SpriteName(self.Label,ActivityConfig.LabelName[title])
	end
	self.Label:SetObjActive(is_open and title > 0)
end

function ChallengeItem:ActRedPoint()
	if self.data == nil then return end
    self.red_point_act = self.red_point_act or UiRedPoint.New(self.RedPoint, Vector3.zero)
	self.red_point_act:SetNum(self.data.art_red_point.val and 1 or 0)
end

function ChallengeItem:GetLimitNum(data)
	local condition_func = ActivityData.Instance:GetConditionList(data.act_type)
	return condition_func and condition_func() or ""
end

function ChallengeItem:GetTopConditionTip(data)
	local condition_func = ActivityData.Instance:GetTopConditionList(data.act_type)
	return condition_func and condition_func() or ""
end

function ChallengeItem:IsTimeComplete(data)
	return (0 < data.total_num and data.cur_num == data.total_num) or (0 == data.total_num and 0 == data.cur_num)
end

function ChallengeItem:GetBtnState(data,is_future)
	local state = is_future and (data.act_type ~= ActType.WorldArena) and data.status or ActStatusType.Open
	return (not is_future) and Format(self.language.LevelLimit, data.info.level_restrict) or (data.state_buttons[state] or self.language.Enter[state])
end

function ChallengeItem:GetRewardList(list)
	if #list > 3 then
		self.Content.pivot = Vector2.New(0,0.5)
	end
	return list
end

--点击前往
function ChallengeItem:OnClickEnter()
	if not self.is_open then
		local level = self.data.info.level_restrict
		PublicPopupCtrl.Instance:Center(string.format(ErrorText[90], level))
		return
	end
	if nil ~= self.data.click_handle then
		if self.data.click_handle(true) then
			return
		end
	end
	ViewMgr:CloseView(AthleticsView)
end

function ChallengeItem:OnDestroy()
	if self.cloud_arena_smart_handle ~= nil then
		CloudArenaData.Instance.mySelfInfo:Uncare(self.cloud_arena_smart_handle)
		self.cloud_arena_smart_handle = nil
	end
	if not self.is_care then return end
	self:uncareData()
end

----------------------------ChallengeGrid----------------------------
ChallengeGrid = ChallengeGrid or DeclareMono("ChallengeGrid", UIWidgetBaseItem)

function ChallengeGrid:SetData(data)
	self.Call:SetData(data)
end


-- 预告item
ChallengeNoticeItem = ChallengeNoticeItem or DeclareMono("ChallengeNoticeItem", UIWidgetBaseItem)
function ChallengeNoticeItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	if TableIsEmpty(data) then
		self.Blank:SetActive(true)
		return
	else
		self.Blank:SetActive(false)
	end
	local info = data
	UH.SetText(self.Title, info.name)
	UH.SetText(self.TopConditionTipsTxt, info.condition_tip)
	UH.SetText(self.NoticeTip, info.notice_tip)
	UH.SetText(self.Enter, info.btn_name)
	UH.SetIcon(self.Icon, info.icon, ICON_TYPE.ACT)
	self.CrossServerFlag:SetActive(info.cross_type == 1)
	self.AllCrossServerFlag:SetActive(info.cross_type == 2)
	if data.effect_id then
		self.Effect:Play(data.effect_id)
	end
end

function ChallengeNoticeItem:OnClickEnter()
	if self.data.enter_func then
		self.data.enter_func()
	end
end