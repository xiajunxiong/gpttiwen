CloudArenaAchievementView = CloudArenaAchievementView or DeclareView("CloudArenaAchievementView","cloud_arena/cloud_arena_achievement")
VIEW_DECLARE_MASK(CloudArenaAchievementView, ViewMask.BgBlock)
function CloudArenaAchievementView:CloudArenaAchievementView()
	self.data = CloudArenaData.Instance
end

function CloudArenaAchievementView:OnClickClose()
	ViewMgr:CloseView(CloudArenaAchievementView)
end

function CloudArenaAchievementView:LoadCallback()
		self.TimeMeter:SetCallBack(function ()
			ViewMgr:CloseView(CloudArenaAchievementView)
		end)
		self.TimeMeter:StampTime(self.data.mySelfInfo.season_finish_timestamp,TimeType.Type_Time_6)
end

------------------------------
CloudArenaAchievementPanel = CloudArenaAchievementPanel or DeclareMono("CloudArenaAchievementPanel", UIWFlushPanel)
function CloudArenaAchievementPanel:CloudArenaAchievementPanel()
	self.data = CloudArenaData.Instance
	self.data_cares = {
		{data = self.data.notice_smart, func = self.FlushAchievementList, keys = {"achievement_update"}}
	}
end

function CloudArenaAchievementPanel:FlushAchievementList()
	local list = table.rearrange(self.data:GetAchievementInfo())
	local function sort_func(a,b)
		if a.reward_status == 1 then
			if b.reward_status == 1 then
				return a.achieve_type < b.achieve_type
			else
				return true
			end
		elseif a.reward_status == 0 then
			if b.reward_status == 0 then
				if a.config and a.config.progress_way == 2 then
					if b.config ~= nil then
						return b.config.progress_way == 2 and a.achieve_type < b.achieve_type
					else
						return true
					end
				elseif b.config and b.config.progress_way ~= 2 then
					return a.progress_num / a.config.ach_param1 > b.progress_num / b.config.ach_param1
				else
					return true
				end
			else
				return b.reward_status ~= 1
			end
		else
			if b.reward_status ~= 2 then
				return false
			else
				return a.achieve_type < b.achieve_type
			end
		end
	end
	table.sort(list, sort_func)
	self.AchievementList:SetData(list)
end

-----------------------------------------
CloudArenaAchievementItem = CloudArenaAchievementItem or DeclareMono("CloudArenaAchievementItem", UIWidgetBaseItem)
function CloudArenaAchievementItem:CloudArenaAchievementItem()
	self.data_instance = CloudArenaData.Instance
end

function CloudArenaAchievementItem:SetData(data)
	local achieve_config = self.data_instance:GetAchievementConfig(data.achieve_type, data.cur_level)
	-- LogError("achieve_config",achieve_config)
	if achieve_config == nil then
		LogError("achieve_type",data.achieve_type,"cur_level",data.cur_level," config == nil")
	end
	if achieve_config ~= nil then
		UH.SetText(self.AchieveText, achieve_config.ach_name)
		UH.SetText(self.AchieveDesc, achieve_config.ach_dsc)
		
		if achieve_config.progress_way ~= 2 then
			self.Progress:SetText(string.format(Language.Common.Slash, data.reward_status == 0 and data.progress_num or achieve_config.ach_param1, achieve_config.ach_param1))
			self.Progress:SetProgress(data.reward_status ~= 0 and 1 or data.progress_num / achieve_config.ach_param1)
		else
			self.Progress:SetText(string.format(Language.Common.Slash, data.reward_status ~= 0 and 1 or 0, 1))
			self.Progress:SetProgress(data.reward_status ~= 0 and 1 or 0)
		end
		self.Item:SetData(Item.Init(CommonItemId.CloudArena, achieve_config.money_num))
		self.Item:ShowEffect(data.reward_status == 1)
		UH.SetText(self.BtnText, Language.CloudArena.AchieveRewardStatue[data.reward_status])
		-- self.Interactor.Interactable = data.reward_status == 1
		-- self.Button.enabled = data.reward_status == 1
	end
	self.data = data
end

function CloudArenaAchievementItem:OnClickGet()
	if self.data.reward_status == 1 then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemGet)
		CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_FETCH_ACHIEVE, self.data.achieve_type)
	else
		PublicPopupCtrl.Instance:Center(Language.CloudArena.AchieveTip[self.data.reward_status])
	end
end

-----------段位提升及赛季初始段位界面--------------------
CloudArenaReachRankingView = CloudArenaReachRankingView or DeclareView("CloudArenaReachRankingView", "cloud_arena/cloud_arena_reach")
VIEW_DECLARE_MASK(CloudArenaReachRankingView, ViewMask.BgBlockClose)
function CloudArenaReachRankingView:CloudArenaReachRankingView()
	self.data = CloudArenaData.Instance
end

function CloudArenaReachRankingView:LoadCallback(param)
	self.MoveAnim:ListenEvent("MoveState", function (event_key)
		if event_key == "moved" then
			UH.SpriteName(self.OldRankingImg, CloudArenaRankName[self.data.dan_change_data.new_dan])
			self.EffectTool:Play(2166013)
			self.EffectTool:Stop(self.old_dan_handle)
			self.EffectTool:Play(CloudArenaData.RankingEffId[self.data.dan_change_data.new_dan], self.RaningImgIbjArray[self.view_type])
			AudioMgr:PlayEffect(AudioType.UI,"duanweijinsheng")
		end
	end)
	--testCode
	-- param.view_type = 2
	-- self.data.dan_change_data = {}
	-- self.data.dan_change_data.new_dan = 3
	-- self.data.dan_change_data.old_dan = 2
	-- self.data.mySelfInfo.season_idx = 1
	-- self.data.season_initial_score = 2100
	--endTest

	self.view_type = param.view_type
	for i=1, self.ViewTypeObj:Length() do
		self.ViewTypeObj[i]:SetActive(param.view_type == i)
	end
	--界面类型  1：段位变化 2：赛季初始段位
	if param.view_type == 1 then
		UH.SetText(self.RankingText, Language.CloudArena.RankingTitle[self.data.dan_change_data.new_dan])
		UH.SpriteName(self.NewRankingImg, CloudArenaRankName[self.data.dan_change_data.new_dan])
		UH.SpriteName(self.OldRankingImg, CloudArenaRankName[self.data.dan_change_data.old_dan])
		self.old_dan_handle = self.EffectTool:Play(CloudArenaData.RankingEffId[self.data.dan_change_data.old_dan], self.RaningImgIbjArray[self.view_type])
		self.MoveAnim:SetTrigger(APH("Move"))
	else
		self.data.notice_smart.open_initial_view = false
		UH.SetText(self.SeasonNumText, self.data:GetSeasonIdx())
		UH.AnchoredPosition(self.RankingTitleRact, Vector3.New(self.data:GetSeasonIdx() < 10 and -238 or -261, 221))
		local rank_config = self.data:GetRankConfig(self.data.season_initial_score < 1000 and 1000 or self.data.season_initial_score)
		if rank_config then
			self.EffectTool:Play(CloudArenaData.RankingEffId[rank_config.rank], self.RaningImgIbjArray[self.view_type])
			UH.SpriteName(self.RankingStartImg, CloudArenaRankName[rank_config.rank])
		end
		self.EffectTool:Play(2166011)
		AudioMgr:PlayEffect(AudioType.UI,"chushijinguang")
	end
end

function CloudArenaReachRankingView:OnEffectEnd(param)
	if self.view_type == 1 then
		self.NewRankObj:SetActive(false)
		self.TitleTextObj:SetActive(true)
	else
		local rank_config = self.data:GetRankConfig(self.data.season_initial_score)
		if rank_config then
			self.star_index = 1
			self.delay_show_star = TimeHelper:AddDelayTimer(function ()
				self.show_star_runner = TimeHelper:AddRunTimer(function ()
					if self.star_index <= rank_config.level then
						self.EffectTool:Play(2166012, self.StarEffPar)
						AudioMgr:PlayEffect(AudioType.UI,"chushixingxing")
						self.StarObj[self.star_index]:SetActive(true)
					else
						UH.SetText(self.InitRankingText, rank_config.ranking_title)
					end
					self.star_index = self.star_index + 1
				end, 0.2, rank_config.level + 1, false)
			end,0.5)
		end
	end
end

function CloudArenaReachRankingView:CloseCallback()
	if self.delay_show_star ~= nil then
		TimeHelper:CancelTimer(self.delay_show_star)
		self.delay_show_star = nil
	end
	if self.show_star_runner ~= nil then
		TimeHelper:CancelTimer(self.show_star_runner)
		self.show_star_runner = nil
	end
end