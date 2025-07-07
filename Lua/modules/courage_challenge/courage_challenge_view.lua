CourageChallengeView = CourageChallengeView or DeclareView("CourageChallengeView", "courage_challenge/courage_challenge", Mod.Experience.GuMo)

function CourageChallengeView:CourageChallengeView()
	self.data = CourageChallengeData.Instance
	CourageChallengeCtrl.Instance:SendCourageChallengeRankListReq()
	CourageChallengeCtrl.Instance:SendCourageChallengeReq(3)
	if UnityPlayerPrefs.HasKey(PrefKeys.CourageChallengeAuto()) then
		self.AutoImg:SetActive(PrefsInt(PrefKeys.CourageChallengeAuto())==1)
	else
		self.AutoImg:SetActive(true)
		PrefsInt(PrefKeys.CourageChallengeAuto(), 1)
	end
	
end

function CourageChallengeView:OpenCallback()
	ActFormationCtrl.Instance:SendGetFormationInfo(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_COURAGE_CHALLENGE)
end

function CourageChallengeView:OnClickFormation()
	ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_COURAGE_CHALLENGE)
	self.data:SetRedPoint(false)
	self.RedPiont:SetNum(0)
end

function CourageChallengeView:OnClickHelp()
	PublicPopupCtrl.Instance:HelpTip(146)
end
function CourageChallengeView:OnClickHelp2()
	PublicPopupCtrl.Instance:HelpTip(352)
end

function CourageChallengeView:LoadCallback(param_t)
	-- LogDG(param_t)
	self.click_type = param_t.type or CourageChallengeData.PagodaType.GuMo

	--根据弑神之塔的功能开启显示标题
	local is_open, _ = FunOpen.Instance:GetFunIsOpened(Mod.Experience.ShiShen)
	self.OldTitle:SetActive(not is_open)
	self.NewTitle:SetActive(is_open)
	if not is_open then
		self.LayerList2:SetObjActive(false)
	end
	
	-- self:FlushGuMoList()
	local list,count = self:GetLayerList()
	param_t = self:GetParamt(param_t)
	local index = param_t.index or count
	self.item_index = param_t.item_index
	self.LayerList:SetDataList(list, index)
	self:FlushItemView(list[index or 1] or {})

	--[[ self.LayerList:SetCompleteCallBack(function()
		self.LayerTimer = Invoke(function()
			if index > #list-6 then
				self.ScrollRect.verticalNormalizedPosition = 0
			else
				self.LayerList:ForceJumpVertical((index-1) * 120)
			end
		end,0.1)
	end) ]]
	
	self:FlushShiShenList()

	-- self.max_height = #list * 120 - 1059 + 339
	self:SetCompetitionRank(ServeractivityConfig.CompetitionRank.Type.CourageChallengeRank)
	
	self.is_toggle_gumo = true
	local final_data = self.data:GetFinalSelectData()
	if final_data then
		if final_data.type == CourageChallengeData.PagodaType.ShiShen then
			self.ToggleShiShen:Trigger()
			self.is_toggle_gumo = false
		end
		if self.click_type == CourageChallengeData.PagodaType.ShiShen then
			self.ToggleShiShen:Trigger()
			self.is_toggle_gumo = false
		end
	else
		if self.click_type == CourageChallengeData.PagodaType.ShiShen then
			self.ToggleShiShen:Trigger()
			self.is_toggle_gumo = false
		end
	end
end
function CourageChallengeView:FlushGuMoList()
	local list,count = self:GetLayerList()
	self.LayerList:SetDataList(list)
end
function CourageChallengeView:FlushShiShenList()
	local list, count, _ = self:GetLayerList2()
	local data = self.data:GetSelectData2()
	local param_t = self:GetParamt2(data)
	local index = param_t.index
	self.item_index2 = param_t.item_index
	if index ~= nil then
		self.LayerList2:SetDataList(list, index)
	else
		self.LayerList2:SetData(list)
	end
end

function CourageChallengeView:OnToggleGuMo()
	self.is_toggle_gumo = true
	local list,count = self:GetLayerList()
	local data = self.data:GetSelectData()
	local param_t = self:GetParamt(data)
	local index = param_t.index or count
	self.item_index = param_t.item_index
	self:FlushItemView(list[index or 1] or {})
	self.Tips:SetActive(true)
end
function CourageChallengeView:OnToggleShiShen()
	self.is_toggle_gumo = false
	local list, count, first = self:GetLayerList2()
	local data = self.data:GetSelectData2()
	local param_t = self:GetParamt2(data)
	local index = param_t.index
	self.item_index2 = param_t.item_index
	if index == nil then--第一条单独立出来了
		self.LayerList2:SetData(list)
		self.NewFirstItem:Click()
		self:FlushItemView(first or {})
	else
		self.LayerList2:SetDataList(list, index)
		self:FlushItemView(list[index or 1] or {})
	end
	-- self.Tips:SetActive(false)
end

function CourageChallengeView:SetCompetitionRank(rank_type)
    CompetitionRanklCtrl.Instance:SendCompetitionRankType(rank_type)
    local is_open = CompetitionRanklData.Instance:GetIsOpenEnd(rank_type)
    self.BtnComp:SetActive(is_open)
    UH.SetText(self.CompDesc, Language.CompetitionRank.RankName[rank_type])
end
function CourageChallengeView:CompetitionRank()
    --7165070
    ViewMgr:OpenViewByKey(Mod.RandActivity.CompetitionRank)
end

function CourageChallengeView:FlushItemView(data)
	local item_list
	if data.type == CourageChallengeData.PagodaType.GuMo then
		item_list = self.data:GetLayerBossList(data.layer_level)
		-- self:OnClickBossItem(nil,item_list[self.item_index or 1] or {})
		self.BossList:SetDataList(item_list, self.item_index)
	else
		item_list = self.data:GetLayerBossList2(data.layer_level)
		self.BossList:SetDataList(item_list, self.item_index2)
		-- self:OnClickBossItem(nil,item_list[self.item_index2 or 1] or {})
	end
end

function CourageChallengeView:GetLayerList()
	local list = self.data:GetLayerList()
	list[1].gray_func = function(gray)
		self.TowerGray.enabled = not gray
	end
	list[#list].show_func = function()
		self.BaseItem:SetParent(self.Parent)
	end
	return list,#list
end
function CourageChallengeView:GetLayerList2()
	local list, item = self.data:GetLayerList2()
	list[1].gray_func = function(gray)
		self.TowerGray2.enabled = not gray
	end
	list[#list].show_func = function()
		self.BaseItem2:SetParent(self.Parent2)
	end
	self.NewFirstItem:SetData(item)
	return list, #list, item
end

function CourageChallengeView:GetParamt(param_t)
	if param_t.index ~= nil then
		return param_t
	end
	local data = self.data:GetSelectData()
	if data.layer_level ~= nil then
		return {index = self.data:GetSelectIndex(data.layer_level),item_index = data.level_seq + 1}
	end
	local recommend_data = self.data:GetRecommendData()
	if recommend_data == nil then --打到顶层
		return {}
	end
	return {index = self.data:GetSelectIndex(recommend_data.layer_level),item_index = recommend_data.level_seq + 1}
end
function CourageChallengeView:GetParamt2(param_t)
	if param_t.index ~= nil then
		return param_t
	end
	local data = self.data:GetSelectData2()
	if data.layer_level ~= nil then
		return {index = self.data:GetSelectIndex2(data.layer_level),item_index = data.level_seq + 1}
	end
	local recommend_data = self.data:GetRecommendData2()
	if recommend_data == nil then --打到顶层
		return {}
	end
	return {index = self.data:GetSelectIndex2(recommend_data.layer_level),item_index = recommend_data.level_seq + 1}
end

function CourageChallengeView:OnClickItem(cell,data)
	local item_list
	local index
	if data.type == CourageChallengeData.PagodaType.GuMo then
		item_list = self.data:GetLayerBossList(data.layer_level)
		index = self.item_index
	else
		item_list = self.data:GetLayerBossList2(data.layer_level)
		index = self.item_index2
	end
	self.BossList:SetDataList(item_list,index or 1)
	if index ~= nil then
		self.ShowTimer = TimeHelper:AddDelayTimer(function()
			if index ~= nil and index > 5 then
				self.BossList:JumpVerticalIndex(index, 126)
			end
			if data.type == CourageChallengeData.PagodaType.GuMo then
				self.item_index = nil
			else
				self.item_index2 = nil
			end
		end, 0.5)
	end
	if data.type == CourageChallengeData.PagodaType.GuMo then
		self:FlushRedNumView(data.layer_level)
	else
		self:FlushRedNumView2()
	end
end

function CourageChallengeView:OnClickBossItem(cell,data)
	local config = CourageChallengeData.Instance:GetMonsterConfig(data.monster_group)
	data.monster_name = data.monster_name or config.name or ""
	data.item = data.item or Monster.New({item_id = config.monster_id or 0})
	if data.type == CourageChallengeData.PagodaType.GuMo then
		UH.SetText(self.Layer,Format(Language.CourageChallenge.LayerItem,data.layer_level))
		UH.SetText(self.StarNum,self.data:GetLayerNum(data.layer_level))
		UH.SetText(self.Desc, Language.CourageChallenge.GuMoGet)
	else
		UH.SetText(self.Layer,"")
		UH.SetText(self.StarNum, self.data:GetTotalStar2()) --self.data:GetLayerNum2(data.layer_level))
		UH.SetText(self.Desc, Language.CourageChallenge.ShiShenGet)
	end
	UH.SetText(self.Name,data.monster_name)
	UH.SetText(self.Level,data.level .. Language.Common.Level)
	self.Element:SetData(data.item:EleAttr(), false, data.item:IsChaos())
	local info
	if data.type == CourageChallengeData.PagodaType.GuMo then
		info = self.data:GetLayerBossInfo(data.layer_level,data.level_seq) or {}
	else
		info = self.data:GetLayerBossInfo2(data.layer_level,data.level_seq) or {}
	end
	self.Grid:SetData(self.data:GetRewardList(data,info))
	local index = ((info.get_extra or 0) == 0 and (RoleData.Instance:GetRoleLevel() > self.data:GetExtraRewardLevel() + data.level)) and 2 or 1
	UH.SetText(self.ExtraRewardTip,Format(Language.CourageChallenge.ExtraRewardTip[index],self.data:GetExtraRewardLevel()))

	self.Condition:SetData({
		flag = info.flag_list or {},
		star_num = info.flag_num or 0,
		desc = self.data:GetConditionList(data)
	})
	if data.type == CourageChallengeData.PagodaType.GuMo then
		self.data:SetSelectData(data)
	else
		self.data:SetSelectData2(data)
	end
	self.data:FinalSelectData(data)
	self:SetVideoView(data)
	-- self.BtnAuto:SetActive(self.data:GetFirstKill(data.layer_level,data.level_seq) == 0)
end

function CourageChallengeView:OnClickEnter()
	TeamData.Instance:CheckSingle(function ()
		if ActFormationData.Instance:GetHasEmptyAssist(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_COURAGE_CHALLENGE) then
			PublicPopupCtrl.Instance:DialogTips({
				content = Language.CourageChallenge.EmptyAssistTip,
				cancel = {
					name = Language.CourageChallenge.SettingTip,
					func = function()
						PublicPopupCtrl.Instance:CloseDialogTips()
						self:OnClickFormation()
					end
				},
				confirm = {
					name = Language.CourageChallenge.FinishBtnName[1],
					func = function()
						PublicPopupCtrl.Instance:CloseDialogTips()
						CourageChallengeCtrl.Instance:OnChallenge()
					end
				}
			})
			return
		end
		CourageChallengeCtrl.Instance:OnChallenge()	
	end)
end
function CourageChallengeView:OnClickAchi()
	ViewMgr:OpenView(CourageChallengeAchievement)
end

function CourageChallengeView:OnClickReward()
	ViewMgr:OpenView(CourageChallengeRewardView)
end

function CourageChallengeView:OnClickClose()
	ViewMgr:CloseView(CourageChallengeView)
end

function CourageChallengeView:OnClickRank()
	ViewMgr:OpenView(CourageChallengeRankView,{type = CourageChallengeData.PagodaType.GuMo})
end
function CourageChallengeView:OnClickRank2()
	ViewMgr:OpenView(CourageChallengeRankView,{type = CourageChallengeData.PagodaType.ShiShen})
end
function CourageChallengeView:OnFlush()
	self:FlushRedNumView(self.data:GetSelectData().layer_level)
	self:FlushRedNumView2()
end

function CourageChallengeView:FlushRedNumView(layer_level)
	if layer_level == nil then return end
	if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.BtnReward, Vector3.New(40,30,0))
	end
	self.red_point:SetNum(self.data:GetlayerRewardNum(layer_level))

	for k,item in pairs(self.LayerList.item_list or {}) do
		if item ~= nil and item:GetLayerLevel() == layer_level then
			item:FlushRedPiontNum()
		end
	end
	self.RedPiont:SetNum(self.data.red_point_data.val and 1 or 0)
	self.RedPoint1:SetNum(self.data.red_point_data.val and 1 or 0)
end
function CourageChallengeView:FlushRedNumView2()
	local flag = self.data:AchiCanGet()
	self.RedPoint2:SetNum(flag and 1 or 0)
	self.AchiRed:SetNum(flag and 1 or 0)
end

function CourageChallengeView:CloseCallback()
	if self.IsGoToPlayVideo == nil then
		CourageChallengeData.Instance:Initialization()
	end
	TimeHelper:CancelTimer(self.ShowTimer)
	TimeHelper:CancelTimer(self.LayerTimer)
	if self.handle_care then
        BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        self.handle_care = nil
    end
end

--===============================录像相关===============================
function CourageChallengeView:SetVideoView(data)
	local video_config
	if data.type == CourageChallengeData.PagodaType.GuMo then
		video_config = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_COURAGE_CHALLENGE, data.monster_group)
	else
		video_config = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_NEW_COURAGE_CHALLENGE, data.monster_group)
	end
	if video_config then
		if self.handle_care then
            BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        end
        self.handle_care = BattleVideo.Instance:SimpleInfo():Care(BindTool.Bind(self.FlushGrayView, self, video_config))
        self:FlushGrayView(video_config)
        BattleCtrl.BattleReportSimpleInfo(video_config.monster_group_id, 0)
	else
		self.ViewGray.enabled = false
	end
end

function CourageChallengeView:FlushGrayView(video_config)
	self.ViewGray.enabled = BattleVideo.Instance:GetSimpleInfo(video_config.monster_group_id, 0) == 0
end

function CourageChallengeView:OnClickPlayVideo()
	local data
	if self.is_toggle_gumo then
		data = self.data:GetSelectData()
	else	
		data = self.data:GetSelectData2()
	end
	local video_config
	if data.type == CourageChallengeData.PagodaType.GuMo then
		video_config = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_COURAGE_CHALLENGE, data.monster_group)
	else
		video_config = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_NEW_COURAGE_CHALLENGE, data.monster_group)
	end
	-- local video_config = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_COURAGE_CHALLENGE, data.monster_group)
	if video_config ~= nil then
		BattleVideo.Instance:PlayVideoCfg(video_config)
		BattleCtrl.BattleReportDetailInfo(video_config.monster_group_id)
		self.IsGoToPlayVideo = true
	end
end

function CourageChallengeView:OnClickDrag()
	if self.ContentRect.localPosition.y >= -1059 then
		-- self.max_height then
		local pos = self.ContentRect.localPosition
		pos.y = -1059--self.max_height
		self.ContentRect.localPosition = pos
	end
end
function CourageChallengeView:OnClickDrag2()
	if self.ContentRect2.localPosition.y >= -816 then
		-- self.max_height then
		local pos = self.ContentRect2.localPosition
		pos.y = -816--self.max_height
		self.ContentRect2.localPosition = pos
	end
end

function CourageChallengeView:OnToggleAuto()
	local is_active = not GetActive(self.AutoImg)
	if is_active then
		PublicPopupCtrl.Instance:Center(Language.CourageChallenge.AutoFightTips)
	end
	self.AutoImg:SetActive(is_active)
	PrefsInt(PrefKeys.CourageChallengeAuto(),is_active and 1 or 0)
end

--===============================CourageChallengeItem===============================
CourageChallengeItem = DeclareMono("CourageChallengeItem", UIWidgetBaseItem)

function CourageChallengeItem:SetData(data)
	UH.SetText(self.Layer,Format(Language.CourageChallenge.LayerItem,data.layer_level))
	if data.type == CourageChallengeData.PagodaType.GuMo then
		local cur_num,max_num = CourageChallengeData.Instance:GetLayerNum(data.layer_level)
		UH.SetText(self.StarNum,Format("<color=#"..COLORSTR.Green10..">%s</color>/%s",cur_num,max_num))
		self.Effect:SetActive(cur_num >= max_num)
		self.Inter.Interactable = self:IsLastFirstKill(data.layer_level) and data.level <= RoleData.Instance:GetRoleLevel()
	else
		local cur_num,max_num = CourageChallengeData.Instance:GetLayerNum2(data.layer_level)
		UH.SetText(self.StarNum,Format("<color=#"..COLORSTR.Green10..">%s</color>/%s",cur_num,max_num))
		cur_num = cur_num or 0
		max_num = max_num or 0
		self.Effect:SetActive(cur_num >= max_num)
		self.Inter.Interactable = self:IsLastFirstKill2(data.layer_level) and data.level <= RoleData.Instance:GetRoleLevel()
		if self.Star then
			self.Star:SetActive(self.Inter.Interactable and cur_num < max_num)
		end
	end

	self.Block:SetActive(not self.Inter.Interactable)
	
	if data.show_func ~= nil then
		data.show_func()
	end
	if data.gray_func ~= nil then
		data.gray_func(self.Inter.Interactable)
	end
	UIWidgetBaseItem.SetData(self, data)
	self:FlushRedPiontNum()
end

function CourageChallengeItem:IsLastFirstKill(layer_level)
	self.last_config = CourageChallengeData.Instance:GetLastPassData(layer_level)
	if self.last_config ~= nil then
		return CourageChallengeData.Instance:GetFirstKill(self.last_config.layer_level,self.last_config.level_seq) == 1
	end
	return true
end
function CourageChallengeItem:IsLastFirstKill2(layer_level)
	self.last_config = CourageChallengeData.Instance:GetLastPassData2(layer_level)
	if self.last_config ~= nil then
		return CourageChallengeData.Instance:GetFirstKill2(self.last_config.layer_level,self.last_config.level_seq) == 1
	end
	return true
end

function CourageChallengeItem:OnClickItem()
	if self.data.level > RoleData.Instance:GetRoleLevel() then
		PublicPopupCtrl.Instance:Center(string.format(Language.CourageChallenge.LevelFail,self.data.level))
		return 
	end

	if self.last_config ~= nil then
		local monster_config = CourageChallengeData.Instance:GetMonsterConfig(self.last_config.monster_group) or {}
		PublicPopupCtrl.Instance:Center(Format(Language.CourageChallenge.LockTips[1],self.last_config.level,monster_config.name or {}))
		return
	end
	PublicPopupCtrl.Instance:Center(Format(Language.CourageChallenge.LimitLevelTips,self.data.level))
end

function CourageChallengeItem:FlushRedPiontNum()
	if self.red_point == nil then
		self.red_point = UiRedPoint.New(self.RedPos, Vector3.zero) 
	end
	if self.data.type == CourageChallengeData.PagodaType.GuMo then
		self.red_point:SetNum(CourageChallengeData.Instance:GetlayerRewardNum(self.data.layer_level))
	else
		self.red_point:SetNum(0)
	end
end

function CourageChallengeItem:GetLayerLevel()
	return self.data.layer_level or 0
end

function CourageChallengeItem:Click()
	if self.ToggleClick then
		self.ToggleClick:Trigger()
	end
end

--===============================CourageChallengeBossItem===============================
CourageChallengeBossItem = DeclareMono("CourageChallengeBossItem", UIWidgetBaseItem)

function CourageChallengeBossItem:SetData(data)
	local config = CourageChallengeData.Instance:GetMonsterConfig(data.monster_group)
	data.monster_name = config.name or ""
	data.item = Monster.New({item_id = config.monster_id or 0})
	self.Call:SetData(data.item)
	UH.SetText(self.Name,data.monster_name)
	UH.SetText(self.Level,data.level .. Language.Common.Level)
	self.Element:SetData(data.item:EleAttr(), false, data.item:IsChaos())
	local info
	if data.type == CourageChallengeData.PagodaType.GuMo then
		info = CourageChallengeData.Instance:GetLayerBossInfo(data.layer_level,data.level_seq) or {}
		self.ItemInter.Interactable = (info.first_kill == 1 or CourageChallengeData.Instance:IsLastPass(data.seq)) and data.level <= RoleData.Instance:GetRoleLevel()
	else
		info = CourageChallengeData.Instance:GetLayerBossInfo2(data.layer_level,data.level_seq) or {}
		self.ItemInter.Interactable = (info.first_kill == 1 or CourageChallengeData.Instance:IsLastPass2(data.seq)) and data.level <= RoleData.Instance:GetRoleLevel()
	end
	local flag_list = info.flag_list or {}

	for i=1,self.StarList:Length() do
		local flag = flag_list[i] or 0
        self.StarList[i]:SetActive(flag == 1)
    end
	
	self.Block:SetActive(not self.ItemInter.Interactable)
    self.Call:MakeGrey(not self.ItemInter.Interactable)

    UIWidgetBaseItem.SetData(self, data)
end

function CourageChallengeBossItem:Click()
	if self.ToggleClick then
		self.ToggleClick:Trigger()
	end
end

function CourageChallengeBossItem:OnClickBlock()
	if self.data.level > RoleData.Instance:GetRoleLevel() then 
		PublicPopupCtrl.Instance:Center(string.format(Language.CourageChallenge.LevelFail,self.data.level))
		return 
	end 
	PublicPopupCtrl.Instance:Center(Language.CourageChallenge.LockTips[2])
end

--===============================CourageChallengeCondition===============================
CourageChallengeCondition = DeclareMono("CourageChallengeCondition", UIWidgetBaseItem)

function CourageChallengeCondition:SetData(data)
	for i=1,self.StarList:Length() do
		local flag = data.flag[i] or 0
		self.StarList[i]:SetActive(flag == 1)
		self.GrayList[i].enabled = flag == 0
		if self.ScoreList then
			self.ScoreList[i]:SetActive(i == data.star_num)
		end
		UH.SetText(self.DescList[i],data.desc[i])
	end
	if self.NotScoreTip then
		self.NotScoreTip:SetActive(data.star_num == 0)
	end
	if self.Effect and data.star_num ~= 0 then
		self.Effect:Play((10010838 - data.star_num))
	end
end

--===============================CourageChallengeCall===============================
CourageChallengeCall = DeclareMono("CourageChallengeCall", UIWidgetBaseItem)

function CourageChallengeCall:SetData(data)
	self.Call:SetData(Item.New(data))
	self.Finish:SetActive(data.is_grey)
	self.LabelGray.enabled = data.is_grey
	self.Label:SetActive(data.is_extra_reward == true)
	self.Overdue:SetActive(data.has_overdue == true)
end

--===============================CourageChallengeSub===============================
CourageChallengeSub = CourageChallengeSub or DeclareMono("CourageChallengeSub", UIWFlushPanel)

function CourageChallengeSub:CourageChallengeSub()
	self.data = CourageChallengeData.Instance
    self.data_cares = {
		{data = RoleData.Instance:GetBaseData(), func = self.FlushShowTip, init_call = true,keys = {"level"}},
		{data = MainViewData.Instance.rb_toggle, func = self.SetShow, keys = {"isOn"}}
    }
end

function CourageChallengeSub:FlushShowTip()
	self.extra_data = self.data:GetRecommendData() or {}
	self.Sub:SetActive(self.data:IsShowExtraTip(self.extra_data) and MainViewData.Instance.rb_toggle.isOn)
end

function CourageChallengeSub:SetShow()
	self.Sub:SetActive(MainViewData.Instance.rb_toggle.isOn and self.data:IsShowExtraTip(self.extra_data))
end

function CourageChallengeSub:Awake()
	UIWFlushPanel.Awake(self)
    UH.SetText(self.CickText,RichTextHelper.LinkStr(RichTextHelper.ColorStr(Language.CourageChallenge.GoToTip,COLORSTR.Green3),"_ul",COLORSTR.Green3))
end
function CourageChallengeSub:OnPressDown()
	ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.RbButton)
end

function CourageChallengeSub:OnClickEnter()
	if SceneData.Instance:SceneId() == 155 then
        SceneLogic.Instance:AutoToNpc(CourageChallengeData.NPC)
    else
        SceneLogic.Instance:AutoToNpcs(CourageChallengeData.NPC)
	end
end

function CourageChallengeSub:OnClickClose()
	self.Sub:SetActive(false)
	self.data:SetCourageChallengeTip(self.extra_data.level,1)
end

---------------------------------------CourageChallengeRankPanel-------------------------------------
CourageChallengeRankPanel = CourageChallengeRankPanel or DeclareMono("CourageChallengeRankPanel", UIWFlushPanel)

function CourageChallengeRankPanel:CourageChallengeRankPanel()
    self.data = CourageChallengeData.Instance
    self.data_cares = {
        {data = self.data.rank_data, func = self.FlushRankView}
    }
end

function CourageChallengeRankPanel:FlushRankView()
    local rank = DataHelper.SimpleCondIndex(self.data:GetRankList(0),function(info)
        return info.role_id == RoleData.Instance:GetRoleId()
    end)
    UH.SetText(self.Num,rank or "")
    -- self.Normal:SetActive(rank == nil)
end

CourageChallengeTipView = CourageChallengeTipView or DeclareView("CourageChallengeTipView", "courage_challenge/courage_challenge_tip")

function CourageChallengeTipView:LoadCallback(param_t)
	self.ShowTimer = Invoke(function()
		ViewMgr:CloseView(CourageChallengeTipView)
	end,3)
	UH.SetText(self.Num,Format(Language.CourageChallenge.PetHelpTip,DataHelper.GetDaXie(param_t.num)))
end

function CourageChallengeTipView:CloseCallback()
	TimeHelper:CancelTimer(self.ShowTimer)
	CourageChallengeData.Instance:SetRedPoint(true)
	ViewMgr:FlushView(CourageChallengeView)
end