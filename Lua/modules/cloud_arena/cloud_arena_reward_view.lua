CloudArenaRewardView = CloudArenaRewardView or DeclareView("CloudArenaRewardView","cloud_arena/cloud_arena_reward")
VIEW_DECLARE_MASK(CloudArenaRewardView, ViewMask.BgBlock)
function CloudArenaRewardView:CloudArenaRewardView()
	self.data = CloudArenaData.Instance
end

function CloudArenaRewardView:LoadCallback(open_param)
	if open_param and open_param.from_main then
		self.data.notice_smart.reward_type = 1
		self.ToggleBtns[self.data.notice_smart.reward_type].isOn = true
		self.TimeMeter:SetCallBack(function ()
			ViewMgr:CloseView(CloudArenaRewardView)
		end)
		self.TimeMeter:SetShowCallBack(function (t)
			return string.format(Language.CloudArena.SeasonFinishLeft, t.day, t.hour)
		end)
		self.TimeMeter:StampTime(self.data.mySelfInfo.season_finish_timestamp)
		self.TimeObj:SetActive(true)
		self.GotoBtnObj:SetActive(true)
	else
		self.TimeObj:SetActive(false)
		self.GotoBtnObj:SetActive(false)
	end
end

function CloudArenaRewardView:OnClickGoto()
	CloudArenaCtrl.Instance:OpenCloudArenaView()
	ViewMgr:CloseView(CloudArenaRewardView)
end

function CloudArenaRewardView:OnClickClose()
	ViewMgr:CloseView(CloudArenaRewardView)
end

function CloudArenaRewardView:OnClickToggle(param)
	self.data.notice_smart.reward_type = param
end

function CloudArenaRewardView:OnClickGetReward()
	local can_get, latest_index = self.data:GetLatestCanGet()
	if can_get then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemGet)
		CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_FETCH_RANING_REWARD,latest_index - 1)
	else
		PublicPopupCtrl.Instance:Center(Language.CloudArena.ScoreNoReach)
	end
end
------------------------------
CloudArenaRewardPanel = CloudArenaRewardPanel or DeclareMono("CloudArenaRewardPanel", UIWFlushPanel)
function CloudArenaRewardPanel:CloudArenaRewardPanel()
	self.data = CloudArenaData.Instance
	self.data_cares = {
		{data = self.data.notice_smart, func = self.FlushRewardList, keys = {"reward_type"}},
		{data = self.data.mySelfInfo, func = self.FlushMyRewardInfo, init_call = false},
		{data = self.data.notice_smart, func = self.FlushRedPoint, keys = {"reward_red"}},
	}
end

function CloudArenaRewardPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	self.data.notice_smart.reward_type = 1
	self.btn_red_point = nil
	self.ranking_red_point = nil
end

function CloudArenaRewardPanel:FlushRewardList()
	local show_list = TableCopy(self.data:GetCurRewardList())
	if self.data.notice_smart.reward_type == 0 then
		local temp_list = {}
		for k,v in pairs(show_list) do
			temp_list[25 - v.ranking] = v
		end
		show_list = temp_list
	end
	self.RewardList:SetData(show_list)
	self:FlushMyRewardInfo()
end

function CloudArenaRewardPanel:FlushMyRewardInfo()
	--段位奖励
	if self.data.notice_smart.reward_type == 0 then
		local can_get, latest_index = self.data:GetLatestCanGet()
		if latest_index ~= 0 then
			local ranking_cfg = self.data:GetRankConfig(nil, latest_index)
			self.RankingMaxObj:SetActive(false)
			self.RankingTitleObj:SetActive(false)
			
			if ranking_cfg then
				UH.SetText(self.CurScoreText, string.format(Language.CloudArena.CurScore, self.data.mySelfInfo.cur_score))
				UH.SetText(self.RankingReachTxt, string.format(Language.Common.Xstr, QualityColorStr[ranking_cfg.rank], ranking_cfg.ranking_title))
				UH.SetText(self.RankingReachRank, can_get and Language.CloudArena.CanGet or string.format(Language.CloudArena.ScoreReach, ranking_cfg.score))
			end
			self.RankingReachTitleObj:SetActive(true)
		else
			UH.SetText(self.CurScoreText,"")
			self.RankingMaxObj:SetActive(true)
			self.RankingTitleObj:SetActive(false)
			self.RankingReachTitleObj:SetActive(false)
		end
		UH.SetText(self.ConditionText, "")
		self.RewardItemList:SetObjActive(false)
		UH.SetText(self.SeasonRankText, "")
		self.NoRankingTempObj:SetActive(false)
		self.SeasonTitleObj:SetActive(false)
		self.RankTitleObj:SetActive(true)
	else
		--赛季奖励
		if self.data.mySelfInfo.cur_rank > -1 then
			self.RewardItemList:SetObjActive(true)
			self.NoRankingTempObj:SetActive(false)
			local rank_reward_cfg = self.data:GetCurRankReward()
			local show_list = {}
			if rank_reward_cfg then
				for k,v in pairs(rank_reward_cfg.ranking_reward_show) do
					table.insert(show_list, Item.New({item_id = v.item_id,num = v.num, is_bind = v.is_bind}))
				end
				self.RewardItemList:SetData(show_list)

				UH.SetText(self.SeasonRankText, string.format(Language.CloudArena.RankNum, self.data.mySelfInfo.cur_rank + 1))
			end
		else
			self.RewardItemList:SetObjActive(false)
			self.NoRankingTempObj:SetActive(true)
			UH.SetText(self.SeasonRankText, "")
		end
		UH.SetText(self.ConditionText, Language.CloudArena.MyReward)
		self.SeasonTitleObj:SetActive(true)
		self.RankTitleObj:SetActive(false)
		self.RankingMaxObj:SetActive(false)
		self.RankingTitleObj:SetActive(false)
		self.RankingReachTitleObj:SetActive(false)
		UH.SetText(self.CurScoreText, "")
	end
end

function CloudArenaRewardPanel:FlushRedPoint()
	if self.data:HasRankingRewardCanGet() == 1 then
		if self.ranking_red_point == nil then
			self.ranking_red_point = UiRedPoint.New(self.ToggleBtn2, Vector3.New(22, 53, 0))
		end
		self.ranking_red_point:SetNum(1)
	elseif self.ranking_red_point ~= nil then
		self.ranking_red_point:SetNum(0)
	end
end
-----------------------------------------
CloudArenaRewardItem = CloudArenaRewardItem or DeclareMono("CloudArenaRewardItem", UIWidgetBaseItem)
function CloudArenaRewardItem:CloudArenaRewardItem()
	self.data_instance = CloudArenaData.Instance
	if self.care_handle ~= nil then
		self.data_instance.mySelfInfo:Uncare(self.care_handle)
		self.care_handle = nil
	end
end

function CloudArenaRewardItem:OnDestroy()
	if self.care_handle ~= nil then
		self.data_instance.mySelfInfo:Uncare(self.care_handle)
		self.care_handle = nil
	end
end

function CloudArenaRewardItem:FlushItem()
	if self.data ~= nil then
		self:SetData(self.data)
	end
end

function CloudArenaRewardItem:SetData(data)
	if self.care_handle == nil then
		self.care_handle = self.data_instance.mySelfInfo:KeysCare({"fetch_ranking_reward_flag"}, BindTool.Bind1(self.FlushItem, self))
	end
	--段位奖励
	if self.data_instance.notice_smart.reward_type == 0 then
		UH.AnchoredPosition(self.SplitRect, Vector2.New(-80 , 0))
		UH.AnchoredPosition(self.ScrollViewRect, Vector2.New(120, 0))
		self.ScrollViewRect.sizeDelta = Vector2.New(336, 95)
		self.ContentLayout.minWidth = 336
		UH.SpriteName(self.RankImg, nil)
		UH.SetText(self.RankText,"")
		self.CurRankSignObj:SetActive(false)
		
		UH.SetText(self.RankLevelText, string.format(Language.Common.Xstr, QualityColorStrShallow[data.rank], data.ranking_title))
		if data.score and data.ranking and data.ranking_reward and data.rank and data.ranking_title then
			UH.SetText(self.RankScore, data.score..Language.CloudArena.Score)
		else
			LogError("CloudArenaRewardItem SetData",data)
			self.data = nil
			return
		end
		UH.SpriteName(self.RankIcon, CloudArenaRankName[data.rank])

		
		local reward_flag = self.data_instance:GetRankingRewardFlag(data.ranking + 1)
		local gray = reward_flag.get_flag == 1
		local can_get = reward_flag.reach_flag == 1 and reward_flag.get_flag == 0
		local reward_list = {}
		for k,v in pairs(data.ranking_reward) do
			if gray or not can_get then
				table.insert(reward_list, {item = Item.New({item_id = v.item_id,num = v.num, is_bind = v.is_bind,is_gray = gray}), is_gray = gray, index = #reward_list + 1})
			else
				table.insert(reward_list, Item.New({item_id = v.item_id,num = v.num, is_bind = v.is_bind}))
			end
		end

		if gray or not can_get then
			table.insert(reward_list, {item = Item.New({item_id = CloudArenaData.DayRewardItem[1], num = data.ranking_reward_money, is_gray = gray}), is_gray = gray, index = #reward_list + 1})
			self.RewardItemList:SetData(reward_list)
			self.RewardItemList:SetObjActive(true)
			self.GetRewardItemList:SetObjActive(false)
		else
			table.insert(reward_list, Item.New({item_id = CloudArenaData.DayRewardItem[1], num = data.ranking_reward_money}))
			self.GetRewardItemList:SetData(reward_list)
			self.RewardItemList:SetObjActive(false)
			self.GetRewardItemList:SetObjActive(true)			
		end
	else
		--赛季奖励
		UH.AnchoredPosition(self.SplitRect, Vector2.New(-175, 0))
		UH.AnchoredPosition(self.ScrollViewRect, Vector2.New(84, 0))
		self.ScrollViewRect.sizeDelta = Vector2.New(360, 95)
		self.ContentLayout.minWidth = 360
		UH.SpriteName(self.RankIcon, nil)
		UH.SetText(self.RankLevelText,"")
		UH.SetText(self.RankScore,"")
		if data.min_rank_pos and data.max_rank_pos then
			if data.min_rank_pos == data.max_rank_pos then
				UH.SpriteName(self.RankImg, "paiming_"..data.min_rank_pos)
				local rank_str = data.min_rank_pos <= 3 and "" or data.min_rank_pos
				if data.min_rank_pos > 3 and self.data_instance.mySelfInfo.cur_rank + 1 == data.min_rank_pos then
					-- rank_str = string.format(Language.Common.Xstr, COLORSTR.Blue6, rank_str)
					self.CurRankSignObj:SetActive(true)
				else
					self.CurRankSignObj:SetActive(false)
				end
				UH.SetText(self.RankText, rank_str)
			elseif data.min_rank_pos < data.max_rank_pos then
				UH.SpriteName(self.RankImg, nil)
				local rank_str = data.min_rank_pos.."-"..data.max_rank_pos
				if self.data_instance.mySelfInfo.cur_rank + 1 >= data.min_rank_pos and self.data_instance.mySelfInfo.cur_rank + 1 <= data.max_rank_pos then
					-- rank_str = string.format(Language.Common.Xstr, COLORSTR.Blue6, rank_str)
					self.CurRankSignObj:SetActive(true)
				else
					self.CurRankSignObj:SetActive(false)
				end
				UH.SetText(self.RankText, rank_str)
			end

			local reward_list = {}
			for k,v in pairs(data.ranking_reward_show) do
				table.insert(reward_list, {item = Item.New({item_id = v.item_id,num = v.num, is_bind = v.is_bind})})
			end
			self.RewardItemList:SetData(reward_list)
		end
		self.RewardItemList:SetObjActive(true)
		self.GetRewardItemList:SetObjActive(false)
	end
	self.data = data
end

function CloudArenaRewardItem:OnClickGet()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemGet)
	CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_FETCH_RANING_REWARD, self.data.ranking)
end
----------------------------------
--带已领取遮罩item
CloudArenaRewardItemCell = CloudArenaRewardItemCell or DeclareMono("CloudArenaRewardItemCell",UIWidgetBaseItem)
function CloudArenaRewardItemCell:CloudArenaRewardItemCell()
	self.data_instance = CloudArenaData.Instance
end

function CloudArenaRewardItemCell:SetData(data)
	self.ItemCell:SetData(data.item)
	self.MaskObj:SetActive(data.is_gray)
	self.data = data
end
---------------------------------------
--带领取奖励特效item
CloudArenaRewardEffectItemCell = CloudArenaRewardEffectItemCell or DeclareMono("CloudArenaRewardEffectItemCell", UIWidgetBaseItem)
function CloudArenaRewardEffectItemCell:CloudArenaRewardEffectItemCell()
end

function CloudArenaRewardEffectItemCell:SetData(data)
	self.ItemCell:SetData(data)
end

function CloudArenaRewardEffectItemCell:ShowEffect(show)
	self.ItemEffectObj:SetActive(show)
end
-----------------------------------------
CloudArenaDayRewardView = CloudArenaDayRewardView or DeclareView("CloudArenaDayRewardView","cloud_arena/cloud_arena_day_reward")
VIEW_DECLARE_MASK(CloudArenaDayRewardView, ViewMask.BgBlock)
function CloudArenaDayRewardView:CloudArenaDayRewardView()
	self.data = CloudArenaData.Instance
end

function CloudArenaDayRewardView:OnClickGet()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemGet)
	CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_FETCH_DAY_REWARD)
	self.data.mySelfInfo.day_reward_status = 2
end

function CloudArenaDayRewardView:OnClickClose()
    ViewMgr:CloseView(CloudArenaDayRewardView)
end
--------------------------
CloudArenaDayRewardPanel = CloudArenaDayRewardPanel or DeclareMono("CloudArenaDayRewardPanel",UIWFlushPanel)
function CloudArenaDayRewardPanel:CloudArenaDayRewardPanel()
	self.data =CloudArenaData.Instance
	self.data_cares = {
		{data = self.data.mySelfInfo, func = self.FlushRewardInfo, keys = {"day_reward_status"}}
	}
	self.red_point = nil
end

function CloudArenaDayRewardPanel:FlushRewardInfo()
	self.RankList:SetData(DataHelper.ReverseList(Config.cloud_battlefield_auto.ranking_cfg))
	local rank_cfg = self.data:GetRankConfig()
	if self.data.mySelfInfo.day_reward_status == 1 then
		rank_cfg = self.data:GetRankConfig(self.data.mySelfInfo.last_day_score)
	end

	if self.data.mySelfInfo.day_reward_status == 1 then
		local rank_cfg = self.data:GetRankConfig(self.data.mySelfInfo.last_day_score)
		if rank_cfg then
			UH.SetText(self.OldRankingText, string.format(Language.Common.Xstr, QualityColorStr[rank_cfg.rank], rank_cfg.ranking_title))
			local reward_list = {}
			for i=1, 2 do
				table.insert(reward_list, Item.Init(CloudArenaData.DayRewardItem[i], rank_cfg["day_reward_"..i]))
			end
			self.OldRankList:SetData(reward_list)
			self.TodayReward:SetActive(true)
			if self.red_point == nil then
				self.red_point = UiRedPoint.New(self.GetRewardBtn, Vector3.New(76,23,0))
			end
			self.red_point:SetNum(1)
			self.NextDayReward:SetActive(false)
		end
	else
		local cur_rank_cfg = self.data:GetRankConfig()
		local reward_list = {}
		for i=1, 2 do
			table.insert(reward_list, Item.Init(CloudArenaData.DayRewardItem[i], cur_rank_cfg["day_reward_"..i]))
		end
		self.CurRewardList:SetData(reward_list)
		UH.SetText(self.RankingText, string.format(Language.Common.Xstr, QualityColorStr[cur_rank_cfg.rank], cur_rank_cfg.ranking_title))
		self.TodayReward:SetActive(false)
		self.NextDayReward:SetActive(true)
	end
end
-----------------------------------
CloudArenaDayRewardItem = CloudArenaDayRewardItem or DeclareMono("CloudArenaDayRewardItem", UIWidgetBaseItem)
function CloudArenaDayRewardItem:CloudArenaDayRewardItem()
	self.data_instance = CloudArenaData.Instance
end

function CloudArenaDayRewardItem:SetData(data)
	UH.SpriteName(self.RankIcon, CloudArenaRankName[data.rank])
	UH.SetText(self.RankText, string.format(Language.Common.Xstr, QualityColorStrShallow[data.rank], data.ranking_title))
	UH.SetText(self.RankScore, data.score..Language.CloudArena.Score)
	local reward_list = {}
	for i=1, 2 do
		table.insert(reward_list, Item.Init(CloudArenaData.DayRewardItem[i], data["day_reward_"..i]))
	end
	self.RewardList:SetData(reward_list)
	if self.data_instance.cur_rank_cfg == nil then
		self.Selector:SetActive(false)
	else
		self.Selector:SetActive(data.rank == self.data_instance.cur_rank_cfg.rank and data.level == self.data_instance.cur_rank_cfg.level)
	end
end
---------------------------------------
CloudArenaMainIconSub = CloudArenaMainIconSub or DeclareMono("CloudArenaMainIconSub", UIWFlushPanel)
function CloudArenaMainIconSub:CloudArenaMainIconSub()
	self.data_instance = CloudArenaData.Instance
	self.data_cares = {
		{data = self.data_instance.mySelfInfo, func = self.FlushIcon, init_call = false}
	}
end

function CloudArenaMainIconSub:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self:FlushIcon()
end

function CloudArenaMainIconSub:FlushIcon()
	UH.SetText(self.Name,Language.CloudArena.CloudArena)
	self.TimeMater:SetCallBack(function ()
			self.data_instance.main_view_btn_check:SetDirty("val")
		end)
	self.TimeMater:SetShowCallBack(function (t)
		if t.day == 0 then
			if t.hour == 0 then
				return string.format(Language.CloudArena.SeasonFinishLeftMin, t.min)
			else
				return string.format(Language.CloudArena.SeasonFinishLeftLastDay, t.hour)
			end
		else
			return string.format(Language.CloudArena.SeasonFinishLeftDay, t.day)
		end
	end)
	self.TimeMater:StampTime(self.data_instance.mySelfInfo.season_finish_timestamp, TimeType.Type_Time_6)
end

function CloudArenaMainIconSub:OnClickIcon()
	ViewMgr:OpenView(CloudArenaRewardView,{from_main = true})
end