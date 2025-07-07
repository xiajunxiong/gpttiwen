
ActivityWeekView = ActivityWeekView or DeclareView("ActivityWeekView", ActivityConfig.ResPath .. "activity_week")

VIEW_DECLARE_MASK(ActivityWeekView,ViewMask.Block)

function ActivityWeekView:ActivityWeekView()
	self.data = ActivityCtrl.Instance:Data()
	self.language = Language.Activity
end

function ActivityWeekView:Awake()
	local week_up_list, week_down_list = self.data:GetActivityWeekList()
	self.UpList:SetData(week_up_list)
	self.DownList:SetData(week_down_list)
end

function ActivityWeekView:OnClickClose()
	ViewMgr:CloseView(ActivityWeekView)
end

ActivityWeekViewUpItem = DeclareMono("ActivityWeekViewUpItem", UIWidgetBaseItem)

function ActivityWeekViewUpItem:SetData(data)
	self.Name.text = data.name
end

ActivityWeekViewDownItem = DeclareMono("ActivityWeekViewDownItem", UIWidgetBaseItem)

function ActivityWeekViewDownItem:SetData(data)
	self.ItemList:SetData(data)
end

ActivityWeekViewDownListItem = DeclareMono("ActivityWeekViewDownListItem", UIWidgetBaseItem)

function ActivityWeekViewDownListItem:SetData(data)
	local value = ActivityData.Instance:GetWeekNum() == data.week_num
	local content = data.act_name or data.act_show_time
	if data.info ~= nil and RoleLevel() < data.info.level_restrict then
		content = content .. Format(Language.Activity.WeekTip,data.info.level_restrict)
	end
	UH.SetText(self.Content,ColorStr(content,value and COLORSTR.Yellow5 or COLORSTR.Yellow13))

	self.BtnTip:SetActive(data.info and data.info.content_tip ~= 0 and value)
	self.IsToday:SetActive(value)
	UIWidgetBaseItem.SetData(self, data)
end

function ActivityWeekViewDownListItem:OnClickHelp()
	local reward_func = ActivityData.Instance:GetPreviewRewardFunc(self.data.info.act_type)
	if reward_func ~= nil then
		ActivityCtrl.Instance:OpenTimeLimitReward(reward_func(),self.data.info.act_type)
	else
		PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[self.data.info.content_tip].desc)
	end
end