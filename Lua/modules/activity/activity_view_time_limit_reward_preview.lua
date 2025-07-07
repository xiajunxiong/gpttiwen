
TimeLimitRewardPreview = TimeLimitRewardPreview or DeclareView("TimeLimitRewardPreview", "time_limit_reward_preview/TimeLimitRewardPreview")
VIEW_DECLARE_MASK(TimeLimitRewardPreview, ViewMask.BgBlockClose)
function TimeLimitRewardPreview:LoadCallback(param_t)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
	if param_t.act_type then
		local config = ActivityData.GetConfig(param_t.act_type) or {}
		self.Time:SetShowCallBack(function(t)
			return Format(Language.Activity.TimeLimitRewardTip,config.name_text, t.day, t.hour ,t.min)
		end)
		self.Time:StampTime(ActivityData.GetRemindTimeStamp(param_t.act_type))
	end
	self.TimeGame:SetActive(param_t.act_type ~= nil)
end

------------------------------------------
TimeLimitRewardPreviewFlush = TimeLimitRewardPreviewFlush or DeclareMono("TimeLimitRewardPreviewFlush", UIWFlushPanel)
function TimeLimitRewardPreviewFlush:TimeLimitRewardPreviewFlush()
	self.data_cares = {
		{data = ActivityData.Instance.time_limit_reward_list_index, func = self.FlushReward, keys = {"index"}},
	}
	local total_reward = ActivityData.Instance:GetCacheTimeLimitRewardList()
	self.toggles = {}
	for i=1, #total_reward do
		self.toggles[i] = {seq = i, toggle_name = total_reward[i].toggle_name}
	end
	self.toggles_flushed = false
end

function TimeLimitRewardPreviewFlush:FlushToggle()
	if not self.toggles_flushed then
		self.ToggleList:SetData(self.toggles)
		self.ToggleList:ClickItem(1)
		self.toggles_flushed = true
	end
end

function TimeLimitRewardPreviewFlush:FlushReward()
	self:FlushToggle()
	local total_reward = ActivityData.Instance:GetCacheTimeLimitRewardList()
	local reward_item_list = total_reward[ActivityData.Instance.time_limit_reward_list_index.index].reward_item_list
	self.RewardList:SetData(reward_item_list)
	-- TimeHelper:AddDelayFrameTimer(function ()
		local y = 0
		for k,v in pairs(self.RewardList.item_list) do
			y = v.Rect.sizeDelta.y + y
		end
	self.ConTantRect.sizeDelta = Vector2.New(self.ConTantRect.sizeDelta, y + 10)
	-- end,5)
	
end

function TimeLimitRewardPreviewFlush:OnDestroy()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
	UIWFlushPanel.OnDestroy(self)
	ActivityData.Instance.time_limit_reward_list_index.index = 1
end

----------侧标签页按钮------------------
PreviewToggleBtnItem = PreviewToggleBtnItem or DeclareMono("PreviewToggleBtnItem",UIWidgetBaseItem)
function PreviewToggleBtnItem:PreviewToggleBtnItem()
end

function PreviewToggleBtnItem:SetData(data)
	UH.SetText(self.Name, data.toggle_name)
	self.data = data
end

function PreviewToggleBtnItem:OnClickToggle()
	ActivityData.Instance.time_limit_reward_list_index.index = self.data.seq
end

function PreviewToggleBtnItem:Click()
	self.Toggle.isOn = true
end

-----------每一项奖励格子-----------------
RewardRowItem = RewardRowItem or DeclareMono("RewardRowItem",UIWidgetBaseItem)
function RewardRowItem:RewardRowItem()
end

function RewardRowItem:OnDestroy()
	if self.care_handle ~= nil then
		ActivityData.Instance.time_limit_reward_point:Uncare(self.care_handle)
		self.care_handle = nil
	end
	self:CleanTimer()
end

function RewardRowItem:FlushBtn()
	if self.data.func_param == nil or self.data.func_param.activity_type == nil then
		return
	end
	local act_flag = ActivityData.Instance:GetTimeLimitRewardGetFlag(self.data.func_param.activity_type)
	if act_flag.particip_award_flag == 1 then
		self.data.btn_flag = 1
	end
	self.BtnInteractor.Interactable = self.data.btn_flag == 0
	UH.SetText(self.BtnText, Language.Common.BtnState[self.data.btn_flag])
end

function RewardRowItem:SetData(data)
	self.data = data
	if data.func_param ~= nil then
		if data.btn_flag == nil or data.click_func == nil then
			local act_flag = ActivityData.Instance:GetTimeLimitRewardGetFlag(data.func_param.activity_type)
			if act_flag ~= nil then
				if data.func_param.op_type == ActivityConfig.TIME_LIMIT_REWARD_GET_OP.JOIN_AWARD then
					data.btn_flag = data.btn_flag and data.btn_flag or act_flag.particip_award_flag
				end
			end
		end
		self.BtnInteractor.Interactable = data.btn_flag == 0
		UH.SetText(self.BtnText, Language.Common.BtnState[data.btn_flag])
		self.BtnObj:SetActive(true)
		if self.care_handle == nil then
			if data.func_param.activity_type then
				self.care_handle = ActivityData.Instance.time_limit_reward_point:KeysCare({"val"},BindTool.Bind1(self.FlushBtn, self))
			end
		elseif data.func_param == nil or data.func_param.activity_type == nil then
			ActivityData.Instance.time_limit_reward_point:Uncare(self.care_handle)
			self.care_handle = nil
		end
	else
		self.BtnObj:SetActive(false)
	end
	self:CleanTimer()
	-- self.ParentFitter.enabled = false
	if data.tips_act_type and ActivityData.Instance:GetActTipsSeq(data.tips_act_type) ~= nil then
		self.ParentFitter.enabled = false
		self:HideAll()
		UH.SetText(self.TipsText, Config.language_cfg_auto.textdesc[ActivityData.Instance:GetActTipsSeq(data.tips_act_type)].desc)
		self.TipsText.enabled = true
		self.delay_resize = TimeHelper:AddDelayFrameTimer(function ()
			self.Rect.sizeDelta = Vector2.New(508, self.TipsRect.sizeDelta.y)
			self.ParentFitter.enabled = true
		end,0)
		return
	else
		self.TipsText.enabled = false
	end
	local title_type = data.title_type and data.title_type or 1
	for i=1,2 do
		self["TitleObj"..i]:SetActive(title_type == i)
		UH.SetText(self["TitleText"..i], data.title_name)
	end
	if StringIsEmpty(data.tip_str) and data.click_func == nil then
		self.ItemListRect.sizeDelta = Vector2.New(465, 100)
	else
		self.ItemListRect.sizeDelta = Vector2.New(254, 100)
	end
	UH.SetText(self.TipText, data.tip_str)
	-- UH.SetText(self.TitleText, data.title_name)
	if data.item_list then
		self.ItemList:SetObjActive(true)
		self.ItemList:SetData(data.item_list)
		self.ChildList:SetObjActive(false)
		self.Rect.sizeDelta = Vector2.New(508, 150)
	elseif data.child_list then
		self.ChildList:SetObjActive(true)
		self.ChildList:SetData(data.child_list)
		self.ItemList:SetObjActive(false)
		self.Rect.sizeDelta = Vector2.New(508, 208 + 150 * (#data.child_list - 1))
	end
end

function RewardRowItem:CleanTimer()
	if self.delay_resize then
		TimeHelper:CancelTimer(self.delay_resize)
		self.delay_resize = nil
	end
end

function RewardRowItem:HideAll()
	for i=1,2 do
		self["TitleObj"..i]:SetActive(false)
	end
	self.ItemList:SetObjActive(false)
	self.ChildList:SetObjActive(false)
	UH.SetText(self.TipText,"")
end

function RewardRowItem:OnClickGet()
	if self.data.click_func ~= nil then
		self.data.click_func(self.data.func_param)
	else
		if self.data.btn_flag == 2 and self.data.func_param.activity_tip then
			PublicPopupCtrl.Instance:Center(self.data.func_param.activity_tip)
			return
		end
		ActivityCtrl.Instance:SendActivityAdvanceFetchReq(self.data.func_param)
	end
end
-----------奖励子项
RewardRowChildItem = RewardRowChildItem or DeclareMono("RewardRowChildItem", UIWidgetBaseItem)
function RewardRowChildItem:RewardRowChildItem()
end

function RewardRowChildItem:OnDestroy()
	if self.care_handle ~= nil then
		ActivityData.Instance.time_limit_reward_point:Uncare(self.care_handle)
		self.care_handle = nil
	end
end

function RewardRowChildItem:SetData(data)
	self.BtnObj:SetActive(data.tip_str == nil and data.func_param ~= nil)
	self.TipText.gameObject:SetActive(data.tip_str ~= nil)
	UH.SetText(self.TipText, data.tip_str)
	UH.SetText(self.ChildTitle, data.child_title)
	self.ItemList:SetData(data.item_list)
	self.BtnInteractor.Interactable = data.btn_flag == 0
	UH.SetText(self.BtnText, Language.Common.BtnState[data.btn_flag])
	if StringIsEmpty(data.tip_str) and data.func_param == nil then
		self.ItemListRect.sizeDelta = Vector2.New(465, 100)
	else
		self.ItemListRect.sizeDelta = Vector2.New(340, 100)
	end

	self.data = data
end

function RewardRowChildItem:OnClickGet()
	if self.data.click_func ~= nil then
		self.data.click_func(self.data.func_param)
	else
		ActivityCtrl.Instance:SendActivityAdvanceFetchReq(self.data.func_param)
	end
end