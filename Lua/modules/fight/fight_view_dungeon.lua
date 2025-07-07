
FightViewDungeon = FightViewDungeon or DeclareMono("FightViewDungeon", UIWFlushPanel)

function FightViewDungeon:FightViewDungeon()
	self.data = FightCtrl.Instance:Data()
	self.team_data = TeamCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data.cur_sel_types, func = self.FlushShowList},
		{data = self.data.cur_sel_dungeon, func = self.FlushDetail},
		{data = self.data.fight_crystal_sweep, func = self.FlushSweepShow, init_call = false},
		{data = self.data.fight_crystal_sweep, func = self.FlushEnergyRedPoint},
		{data = BagData.Instance.item_grid, func = self.FlushDetail, init_call = false},
		{data = RoleData.Instance:GetBaseData(), func = self.FlushDetail, init_call = false, keys = {"level"}},
	}

	GuideManager.Instance:RegisterGetGuideUi("FightViewDungeonItem", function ()
		if self.ShowList:SetupCompleted() then
			for _, item in pairs(self.ShowList.item_list) do
				if item:IsGuideItem() then
					return item, item:Click()
				end
			end
			return self.ShowList.item_list[1], self.ShowList.item_list[1]:Click()
        end
        return nil,nil
    end)


	self.flush_item_handle = nil
end

function FightViewDungeon:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	if self.flush_item_handle ~= nil then
		TimeHelper:CancelTimer(self.flush_item_handle)
		self.flush_item_handle = nil
	end
	TimeHelper:CancelTimer(self.timer_ct_next)
	TimeHelper:CancelTimer(self.timer_ct_all)
end 

function FightViewDungeon:Awake()
	UIWFlushPanel.Awake(self)
	local dungeon_type_list = self.data:GetDungeonTypeList()
	self.TypeList:SetData(dungeon_type_list)
	self.TypeList:ClickItemData(dungeon_type_list[self.data.cur_sel_types.dungeon_type])
end

function FightViewDungeon:OnEnable()
	UIWFlushPanel.OnEnable(self)
    self.data.cur_sel_tabbar.val = FightConfig.FightTabType.dungeon
end

function FightViewDungeon:FlushShowList()
	self.ToggleGroupShow:SetAllTogglesOff()
	local dungeon_list = self.data:GetDungeonListWithType()
	self.data.cur_sel_dungeon:Set(nil == self.data.cur_sel_type and dungeon_list[next(dungeon_list)] or self.data:GetItemWithType(dungeon_list, self.data.cur_sel_type))
	self.ShowList:SetData(dungeon_list)
	self.ShowList:ClickItemData(self.data.cur_sel_dungeon_jump or self.data.cur_sel_dungeon:Val())
	if self.data.cur_sel_dungeon_jump then
		local jump_pos = 1
		local sel_seq = self.data.cur_sel_dungeon_jump.scene_seq 
		for k, v in pairs(dungeon_list) do
			if sel_seq == v.scene_seq then
				jump_pos = k
				break
			end
		end
		if self.flush_item_handle ~= nil then
			TimeHelper:CancelTimer(self.flush_item_handle)
			self.flush_item_handle = nil
		end

		self.flush_item_handle = TimeHelper:AddDelayTimer(function ()
			self.flush_item_handle = nil
            self.ShowList:JumpVerticalIndex(jump_pos, 96)
        end, 1)
	end
	self.data.cur_sel_dungeon_jump = nil
end

function FightViewDungeon:FlushDetail()
	local cur_sel_dungeon = self.data.cur_sel_dungeon
	if nil == cur_sel_dungeon:Val() then return end
	local have_num = BagData.Instance:GetNumByItemId(cur_sel_dungeon.consume.item_id)
	local show_sweep = 1 == self.data.cur_sel_dungeon.scene_type_1
	self.Title.text = cur_sel_dungeon._scene.name
	self.Content.text = cur_sel_dungeon.fb_explain_text .. (show_sweep and Language.Fight.Sweep.tips or "")
	self.Monsters:SetData(cur_sel_dungeon.monsters)
	-- self.Rewards:SetData(cur_sel_dungeon.rewards)
	self.Consume:SetData(cur_sel_dungeon.consume)
	-- self.TodayTimes.gameObject:SetActive(nil ~= cur_sel_dungeon.use_time)
	-- self.TodayTimes.text = nil ~= cur_sel_dungeon.use_time and string.format(Language.Fight.TodayTimes, 1 - cur_sel_dungeon.use_time) or ""
	self.HaveNum.text = RichTextHelper.ColorStr(have_num, have_num > 0 and COLORSTR.Green7 or COLORSTR.Red8)
	self.BgMapSp:SetByPath(string.format(FightConfig.PngPath, cur_sel_dungeon.bg_map))
	
	local exp_num = self.data:GetFightCrystalExpNum()
	-- self.ExpNum.text = cur_sel_dungeon.exp_num
	self.ExpNum.text = exp_num
	self.ItemSNum.text = cur_sel_dungeon.item_s1_num
	self.ExpNum:SetObjActive(exp_num > 0)

	self:FlushSweepShow()

	self.DownNObj:SetActive(show_sweep)
	self.DownSObj:SetActive(not show_sweep)
	self.Rewards1Obj:SetActive(show_sweep)
	self.Rewards2Obj:SetActive(not show_sweep)
	if show_sweep then
		self.Rewards1:SetData(cur_sel_dungeon.rewards)
		local use_stone = have_num > 0
		self.SweepConsumeDesc.text = string.format(Language.Fight.Sweep.consume, use_stone and cur_sel_dungeon.consume:Name() or Language.Fight.Sweep.energy)
		self.SweepConsumeValue.text = use_stone and 1 or self.data:GetCrystalFbSweepUsePower()
		if use_stone then
			UH.SetIcon(self.SweepConsumeIcon, cur_sel_dungeon.consume:IconId())
		else
			UH.SpriteName(self.SweepConsumeIcon, FightConfig.TiLiZhi)
		end
		self.BtnSweepInter.Interactable = self.data:IsFightCrystalPassFb(cur_sel_dungeon.scene_seq)
	else
		self.Rewards2:SetData(cur_sel_dungeon.rewards)
	end
end

function FightViewDungeon:FlushSweepShow()
	local show_sweep = 1 == self.data.cur_sel_dungeon.scene_type_1
	if show_sweep then
		local strength_num,max_power = self.data:GetFightStrengthNum()
		self.SweepDesc.text = Format(Language.Fight.Sweep.desc, strength_num, max_power)
	end
	self.SweepObj:SetActive(show_sweep)
	self.SweepDetail:SetActive(show_sweep)
	self:FlushSweepDetail()
end

function FightViewDungeon:FlushEnergyRedPoint()
	self.energy_max_red_point = self.energy_max_red_point or UiRedPoint.New(self.SweepObj, Vector3.New(-58,18,0))
	self.energy_max_red_point:SetNum(self.data:IsFightCrystalEnergyMaxRedPoint() and 1 or 0)
end

function FightViewDungeon:FlushSweepDetail()
	local info = self.data.fight_crystal_sweep
	local time_desc = Language.Fight.Sweep.time 
	TimeHelper:CancelTimer(self.timer_ct_next)
	TimeHelper:CancelTimer(self.timer_ct_all)
	if not self.SweepDetail.activeSelf then return end
	if 0 == info.next_energy_reply_tamp then
		self.SweepNextTime.text = string.format(time_desc, 0, 0, 0)
		self.SweepAllTime.text = string.format(time_desc, 0, 0, 0)
		return
	end
	self.timer_ct_next = TimeHelper:AddCountDownCT(function ()
		local next_time = info.next_energy_reply_tamp - TimeHelper.GetServerTime()
		local ft = TimeHelper.FormatDHMS(next_time)
		self.SweepNextTime.text = string.format(time_desc, ft.hour, ft.min, ft.s)
	end, function ()
		self.SweepNextTime.text = string.format(time_desc, 0, 0, 0)
	end, info.next_energy_reply_tamp)
	self.timer_ct_all = TimeHelper:AddCountDownCT(function ()
		local all_time = info.total_energy_reply_tamp - TimeHelper.GetServerTime()
		local ft = TimeHelper.FormatDHMS(all_time)
		self.SweepAllTime.text = string.format(time_desc, ft.hour, ft.min, ft.s)
	end, function ()
		self.SweepAllTime.text = string.format(time_desc, 0, 0, 0)
	end, info.total_energy_reply_tamp)
end

function FightViewDungeon:OnClickTeam()
	-- ViewMgr:OpenView(TeamView)
	-- if not self.team_data:InTeam() then
	-- 	TeamCtrl.Instance:SendCreateTeam()
	-- end
	ViewMgr:CloseView(FightView)
	local target = self.team_data:GetTeamGoalWithId(self.data.cur_sel_dungeon.team_target)
	if LoginData.Instance:IsOnCrossSever() and TeamConfig.CrossType.not_cross == target.is_cross then
		PublicPopupCtrl.Instance:Center(Language.Team.MatchCrossError)
		return
	end
	if RoleData.Instance:GetRoleLevel() < target.low_level then
		PublicPopupCtrl.Instance:Center(Language.Team.MatchLevelError)
		return
	end
	if self.team_data:InTeam() then
		TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
		TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
		ViewMgr:OpenView(TeamView)
	else
		self.team_data.cur_platform_target_id = self.data.cur_sel_dungeon.team_target
		TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
		PublicPopupCtrl.Instance:Center(Language.Team.MatchTips)
	end
end

function FightViewDungeon:OnClickFight()
	if RoleData.Instance:GetRoleLevel() < self.data.cur_sel_dungeon.level_min then
		PublicPopupCtrl.Instance:Center(ErrorInfo[100008])
		return
	end
	if nil ~= self.data.cur_sel_dungeon.click_handle then
		self.data.cur_sel_dungeon.click_handle()
	end
end

function FightViewDungeon:OnClickSweep()
	local value, tips = self.data:IsCanFightCrystalFbSweep()
	if not value then
		PublicPopupCtrl.Instance:Center(tips)
		return
	end
	-- ViewMgr:OpenView(FightCrystalSweepView)
	FightCtrl.Instance:SendCrystalFbFastPassInfo(self.data.cur_sel_dungeon.scene_seq, 1)
end

function FightViewDungeon:OnClickSweepDetail()
	-- self.SweepDetail:SetActive(not self.SweepDetail.activeSelf)
	-- self:FlushSweepDetail()
	ViewMgr:OpenView(FightCrystalSweepItemView)
end

function FightViewDungeon:OnClickTips()
	PublicPopupCtrl.Instance:OpenTipsPopup{tips_str = Config.language_cfg_auto.textdesc[86].desc}
end

function FightViewDungeon:OnClickSG()
	ItemInfoCtrl.Instance:ItemInfoView{item_list = {Item.Init(CommonItemId.SkyGlass)}}
end

FightViewDungeonTypeItem = DeclareMono("FightViewDungeonTypeItem", UIWidgetBaseItem)

function FightViewDungeonTypeItem:FightViewDungeonTypeItem()
	self.fight_data = FightCtrl.Instance:Data()
end

function FightViewDungeonTypeItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	self:uncareData()
	self.care_handles = {}
	-- self:careData(self.fight_data.dungeon_red_points, BindTool.Bind(self.TypeItemRedPoint, self))
	
	self.TypeName.text = data.name
end

function FightViewDungeonTypeItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function FightViewDungeonTypeItem:OnClickItem()
	self.fight_data.cur_sel_types.dungeon_type = self.data.type
end

function FightViewDungeonTypeItem:OnDestroy()
    self:uncareData()
end

function FightViewDungeonTypeItem:TypeItemRedPoint()
	self.red_point_item = self.red_point_item or UiRedPoint.New(self.ItemObj, Vector3.New(50,20,0))
    self.red_point_item:SetNum(self.fight_data:TypeItemRedPoint(self.data.type) and 1 or 0)
end

FightViewDungeonShowItem = DeclareMono("FightViewDungeonShowItem", UIWidgetBaseItem)

function FightViewDungeonShowItem:FightViewDungeonShowItem()
	self.fight_data = FightCtrl.Instance:Data()
end

function FightViewDungeonShowItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	if data.is_show and data.is_show == 0 then
		self.gameObject:SetActive(false)
	end
	self:uncareData()
	self.care_handles = {}
	self:careData(self.fight_data.dungeon_red_points, BindTool.Bind(self.ShowItemRedPoint, self))
	self:careData(BagData.Instance.item_grid, BindTool.Bind(self.FlushFightTips, self))
	self:careData(self.fight_data.fight_crystal_sweep, BindTool.Bind(self.FlushFightTips, self))

	self.Name.text = data._scene.name
	self.Level.text = string.format(Language.Fight.Level, data.level_min, data.level_max)
	self.CurObj:SetActive(data.value_cur)
	self.RecoObj:SetActive(data.value_reco)
	UH.SetIcon(self.MapIcon, data.map_id, ICON_TYPE.MAP)
	local is_lock = RoleData.Instance:GetRoleLevel() < data.level_min
	self.MaskObj:SetActive(is_lock)
	self.DropDouble:SetActive(not is_lock and FightData.IsDropDouble(data.scene_seq))
end

function FightViewDungeonShowItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function FightViewDungeonShowItem:OnClickItem()
	self.fight_data:FlushDungeonRedPonits(self.data.scene_seq)
	self.fight_data.cur_sel_dungeon:Set(self.data)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
end

function FightViewDungeonShowItem:OnDestroy()
    self:uncareData()
end

function FightViewDungeonShowItem:ShowItemRedPoint()
    self.red_point_item = self.red_point_item or UiRedPoint.New(self.ItemObj, Vector3.New(160,40,0))
    self.red_point_item:SetNum(self.fight_data.dungeon_red_points[self.data.scene_seq] and 1 or 0)
end

function FightViewDungeonShowItem:FlushFightTips()
	if nil == self.data then return end
	local fight_state = self.fight_data:GetCrystalFbFightState(self.data.scene_seq, self.data.scene_type_1, self.data.expend_item)
	if FightConfig.FightTipsState.none ~= fight_state then
		self.FightTipsSp.SpriteName = FightConfig.FightTipsSp[fight_state]
		self.FightTips.text = Language.Fight.FightTipsShow[fight_state]
	end
	self.FightTipsSp:SetObjActive(FightConfig.FightTipsState.none ~= fight_state)
end

function FightViewDungeonShowItem:IsGuideItem()
	if nil == self.data then return end
	local fb_id = GuideData.Instance:GetSaoDangFuBenId()
	return fb_id == self.data.scene_seq
end

FightViewDungeonRewardItem1 = DeclareMono("FightViewDungeonRewardItem1", UIWidgetBaseItem)

function FightViewDungeonRewardItem1:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self.PageGrid:SetData(data)
end

FightViewDungeonRewardItem2 = DeclareMono("FightViewDungeonRewardItem2", UIWidgetBaseItem)

function FightViewDungeonRewardItem2:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local prof_type = RoleData.Instance:ProfType()
	prof_type = prof_type > 0 and prof_type or 1
	if data.old_id then
		local co = Cfg.RewardJobBagByIdType(data.old_id, prof_type)
		data.item_id = co and co.new_id or data.item_id
		-- item.show_key = "workshop_equip"
	end
	local item = Item.Create(data)
	item.block_pre_range = true
    self.Cell:SetData(item)
end

--秘境体力
---------------------------------------FightSweepStrengthPanel-------------------------------------
FightSweepStrengthPanel = FightSweepStrengthPanel or DeclareMono("FightSweepStrengthPanel", UIWFlushPanel)

function FightSweepStrengthPanel:FightSweepStrengthPanel()
    self.data = FightCtrl.Instance:Data()
    self.data_cares = {
        {data = self.data.fight_crystal_sweep, func = self.FlushSweepShow},
		{data = self.data.fight_fb_energy_show, func = self.FlushShow},
    }
end

function FightSweepStrengthPanel:FlushShow()
	local is_show = self.data.fight_fb_energy_show.is_show
	self.Energy:SetActive(is_show)
	if not is_show then
		self.BtnParent:SetActive(is_show)
	end
end

function FightSweepStrengthPanel:FlushSweepShow()
	local strength_num,max_power = self.data:GetFightStrengthNum()
	UH.SetText(self.StrengthNum,Format("%s/%s",strength_num,max_power))
	-- self.red_point = self.red_point or UiRedPoint.New(self.RedPointParent, Vector3.New(25,20,0))
	-- self.red_point:SetNum((self.data.fight_crystal_sweep.role_enter_energy > 30 and RoleData.Instance:GetRoleLevel() > 33) and 1 or 0)
end

function FightSweepStrengthPanel:OnClickItem()
	local buy_times = self.data.fight_crystal_sweep.buy_times
	local buy_times_max = self.data:GetSweepBuyTimes()
	if BagData.Instance:GetNumByItemId(CommonItemId.FightCrystalEnergyItem) > 0 then
		ViewMgr:OpenView(FightCrystalSweepItemView)
	elseif buy_times_max - buy_times > 0 then
		ViewMgr:OpenView(FightCrystalSweepItemView, {show_gold = true})
	else
		PublicPopupCtrl.Instance:Center(Language.Fight.Sweep.num_not_enough_today)
	end
end

function FightSweepStrengthPanel:OnClickStrength()
	AudioMgr:PlayEffect(AudioType.UI,self.BtnParent.activeSelf and CommonAudio.MenuClose or CommonAudio.MenuOpen)
	self.BtnParent:SetActive(not self.BtnParent.activeSelf)
end