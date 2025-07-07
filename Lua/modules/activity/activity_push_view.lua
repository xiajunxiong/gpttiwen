
ActivityPushView = ActivityPushView or DeclareView("ActivityPushView", ActivityConfig.ResPath .. "activity_push")

VIEW_DECLARE_MASK(ActivityPushView,ViewMask.Block)

function ActivityPushView:ActivityPushView()
	self.data = ActivityCtrl.Instance:Data()
	self.language = Language.Activity
end

function ActivityPushView:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.ActivityPush, Vector2.New(1008, 531))
	local push_up_list, push_down_list = self.data:GetActivityPushList()
	local push_list = {}
	for i,v in ipairs(push_down_list) do
		if v.info.is_push_show == 1 then
			table.insert(push_list,v)
		end
	end
	self.UpList:SetData(push_up_list)
	self.DownList:SetData(push_list)
	self.Tips.text = self.language.PushTips
end

ActivityPushViewUpItem = DeclareMono("ActivityPushViewUpItem", UIWidgetBaseItem)

function ActivityPushViewUpItem:SetData(data)
	self.Name.text = data.name
end

ActivityPushViewDownItem = DeclareMono("ActivityPushViewDownItem", UIWidgetBaseItem)

function ActivityPushViewDownItem:ActivityPushViewDownItem()
	self.language = Language.Activity.PushDown
end

function ActivityPushViewDownItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local push_info = data.push_info
	local open_days, open_times = "", ""
	for i = 1, #push_info.open_days do
		local day = tonumber(push_info.open_days[i])
		open_days = open_days .. (7 == day and self.language.days7 or string.format(self.language.days, DataHelper.GetDaXie(day))) .. (#push_info.open_days == i and "" or ",")
	end
	for i = 1, #push_info.begin_times do
		open_times = open_times .. string.format(self.language.times, push_info.begin_times[i], push_info.end_times[i]) .. (#push_info.begin_times == i and "" or "\n")
	end
	self.Name.text = data.act_name
	self.Period.text = push_info.is_everyday and self.language.everyday or open_days
	self.Time.text = open_times
	self.BtnRemind.isOn = data:IsRemind()
end

function ActivityPushViewDownItem:OnClickRemind()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
	local flag = self.BtnRemind.isOn
	self.data:SetIsRemind(flag)
	self.RemindT.localPosition = flag and Vector2.New(20, 0) or Vector2.New(-20, 0)
end
