
ActivityInfoView = ActivityInfoView or DeclareView("ActivityInfoView","item_info/activity_info")

VIEW_DECLARE_MASK(ActivityInfoView,ViewMask.BlockClose)

function ActivityInfoView:ActivityInfoView()
	self.data = ItemInfoCtrl.Instance:Data()
	self.info = self.data:ActivityInfo()
	self.language = Language.ItemInfo.ActivityInfoDesc
end

function ActivityInfoView:LoadCallback()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end

function ActivityInfoView:OnFlush()
	if nil ~= self.info.pos then self.InfoPosition.position = self.info.pos end
	self.InfoPosition.gameObject:SetActive(true)

	local act = self.info.info
	local act_info = act.info
	self.Name.text = act.act_name
	self.RecoSite.text = string.format(self.language.reco_site, act_info.recommend_text)
	self.LevelLimit.text = string.format(self.language.level_limit, act_info.level_restrict)
	self.NumLimit.text = string.format(self.language.num_limit, act_info.number_min == act_info.number_max and act_info.number_min or (act_info.number_min .. "-" .. act_info.number_max))
	self.ActTime.text = string.format(self.language.act_time, act_info.time_text)
	self.LevelLimitT.text = self.language.level_limit_t
	self.NumLimitT.text = self.language.num_limit_t
	self.ActTimeT.text = self.language.act_time_t
	self.Content.text = act_info.content_text
	self.RewardList:SetData(act.reward_list or {})
	UH.SetIcon(self.IconSp, act_info.icon_id, ICON_TYPE.ACT)
end