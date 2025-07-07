
HarmonyHeavenView = HarmonyHeavenView or DeclareView("HarmonyHeavenView", HarmonyHeavenConfig.ResPath .. "harmony_heaven", Mod.HarmonyHeaven.Main)

VIEW_DECLARE_LEVEL(HarmonyHeavenView,ViewLevel.L0)

function HarmonyHeavenView:HarmonyHeavenView()
	self.data = HarmonyHeavenCtrl.Instance:Data()
end

function HarmonyHeavenView:LoadCallback()
end

function HarmonyHeavenView:OnClickClose()
    ViewMgr:CloseView(HarmonyHeavenView)
end

function HarmonyHeavenView:OnClickTips()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[self.data:ConfigOtherFbCfg()].desc)
end

function HarmonyHeavenView:OnClickShop()
	ViewMgr:OpenView(HarmonyHeavenShopView)
end

HarmonyHeavenViewF = HarmonyHeavenViewF or DeclareMono("HarmonyHeavenViewF", UIWFlushPanel)

function HarmonyHeavenViewF:HarmonyHeavenViewF()
	self.data = HarmonyHeavenCtrl.Instance:Data()
	self.team_data = TeamCtrl.Instance:Data()
	self.language = Language.HarmonyHeaven.Main

	self.data_cares = {
		{data = self.data.fb_sm.info, func = self.FlushInfo},
		{data = self.data.fb_info.role_info, func = self.FlushRoleInfo},
		{data = self.data.fb_info.world_info, func = self.FlushPassTime, init_call = false},
	}

	HarmonyHeavenCtrl.Instance:SendHongMengTianJieReqWorldPass()
	self.data:FbShowListAutoSel()
end

function HarmonyHeavenViewF:Awake()
	UIWFlushPanel.Awake(self)

	local type = self.data:GetFbType()
	self.TypeList[type].isOn = true
	self:OnToggleType(type)

	-- self.TipsBoss.text = string.format(self.language.TipsBoss, self.data:BossRewardRepeatShow("·"))
	-- self.TipsHelp.text = string.format(self.language.TipsHelp, self.data:HelpRewardRepeatShow("·"))
	-- self.TipsDiff.text = string.format(self.language.TipsDiff, 100)
	self.TipsTeam.text = string.format(self.language.TipsTeam, self.data:TeamNumShow("-"))
	self.DiffTips.text = string.format(self.language.DiffTips, self.data:ConfigOtherFirstDifficult(), self.data:ConfigOtherDownPercent(), self.data:ConfigOtherMinDifficult())
end

function HarmonyHeavenViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_dft)
end

function HarmonyHeavenViewF:OnToggleType(value)
    -- AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    self:FlushShow(value)
    -- self:FlushModelShow(value)
end

function HarmonyHeavenViewF:FlushShow(type)
    local show_list, click_item = self.data:GetFbShowList(type)
	self.ShowList:SetData(show_list)
	self.ShowList:ClickItemData(click_item)
    self.ShowList:JumpVertCondition(self.ShowList:GetByData(click_item))
end

function HarmonyHeavenViewF:FlushInfo()
	local info = self.data.fb_sm.info
	if info.init then return end

	TimeHelper:CancelTimer(self.timer_dft)
	self.timer_dft = TimeHelper:AddDelayFrameTimer(function ()
		if self.effect_handel1 ~= nil then
			self.EffectTool:Stop(self.effect_handel1)
			self.effect_handel1 = nil
		end
		self.effect_handel1 = self.EffectTool:Play(info.special_eddects)
		if 2 == info.fb_type then
			if self.effect_handel2 == nil then
				self.effect_handel2 = self.EffectTool:Play(3161112)
			end
		else
			if self.effect_handel2 ~= nil then
				self.EffectTool:Stop(self.effect_handel2)
				self.effect_handel2 = nil
			end
		end
	end)

    self.BgSetter:SetByPath(string.format(HarmonyHeavenConfig.PngPath, info.seq))
	self.NameShow.text = info.fb_name
	self.DescShow.text = info.dec
	self.DropTips.text = string.format(self.language.DropTips, info.max_drop_equip)
	self.EquipTips.text = string.format(self.language.EquipTips, info.equip_require, MarbasData.Instance:GetTotalLevel())

	local rewards, help_rewards = self.data:GetRewardsShow(info)
	local monsters = self.data:GetMonstersShow(info)
	self.RewardList:SetData(rewards)
	self.HelpRewards:SetData(help_rewards)
	self.MonsterList:SetData(monsters)

	self:FlushPassTime()
	self:FlushTips2Show()
end

function HarmonyHeavenViewF:FlushRoleInfo()
	local info = self.data.fb_info.role_info
	if info.init then return end
	local boss_reward_limit = self.data:ConfigOtherBossRewardLimit()
	local help_reward_limit = self.data:ConfigOtherHelpRewardLimit()
	local reward_times_show = info.rewards_times_max
	local help_times_show = info.help_times
	self.TipsBoss.text = string.format(self.language.TipsBoss2, self.data:BossRewardRepeatShow("、"), 
		RichTextHelper.ColorStr(reward_times_show, reward_times_show > 0 and COLORSTR.Green3 or COLORSTR.Red8), boss_reward_limit)
	self.TipsHelp.text = string.format(self.language.TipsHelp, self.data:HelpRewardRepeatShow("、"), 
		RichTextHelper.ColorStr(help_times_show, help_times_show > 0 and COLORSTR.Green3 or COLORSTR.Red8), help_reward_limit)

	self:FlushTips2Show()
end

function HarmonyHeavenViewF:FlushPassTime()
	local info = self.data.fb_sm.info
	if info.init then return end
	local pass_time = self.data:GetFbPassTime(info.fb_id)
	local pass_t = os.date("*t", pass_time)
	local server_t = os.date("*t", TimeHelper.GetServerTime())
	local is_pass = pass_time > 0
	self.PassObj:SetActive(is_pass)
	self.NotPassObj:SetActive(not is_pass)
	self.PassShow.text = string.format(self.language.PassShow, pass_t.year, pass_t.month, pass_t.day)

	local diff = 100
	if is_pass then
		local t1 = os.date("*t", pass_time)
		local t2 = os.date("*t", TimeHelper.GetServerTime())
		local mon1 = os.time({year = t1.year, month = t1.month, day = t1.day - (1 == t1.wday and 6 or t1.wday - 2), hour = 0, min = 0, sec = 0})
		local mon2 = os.time({year = t2.year, month = t2.month, day = t2.day - (1 == t2.wday and 6 or t2.wday - 2), hour = 0, min = 0, sec = 0})
		diff = math.max(self.data:ConfigOtherMinDifficult(), diff - (mon2 - mon1) / (7 * 24 * 60 * 60) * self.data:ConfigOtherDownPercent())
	end
	self.TipsDiff.text = string.format(self.language.TipsDiff, diff)
end

function HarmonyHeavenViewF:FlushTips2Show()
	local info = self.data.fb_sm.info
	local role_info = self.data.fb_info.role_info
	if info.init or role_info.init then return end
	self.Tips2Obj:SetActive(2 == info.fb_type and 0 == role_info.fb_first_pass_mark[info.lock_id])
end

function HarmonyHeavenViewF:OnClickMonster(monster)
	local info = self.data.fb_sm.info
	local rewards = self.data:GetMonsterRewardsShow(info, monster.index)
	self.RewardInfoShow:SetData(rewards)
	self.RewardInfoObj:SetActive(true)
end

function HarmonyHeavenViewF:OnClickMonsterBlock()
	self.RewardInfoObj:SetActive(false)
end

function HarmonyHeavenViewF:OnClickFight()
	local info = self.data.fb_sm.info
	if info.init then return end

	if not self.data.fb_info.role_info.init and 2 == info.fb_type and 0 == self.data.fb_info.role_info.fb_first_pass_mark[info.lock_id] then
		PublicPopupCtrl.Instance:Center(self.language.LockTips2)
		return
	end
	-- AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	if MarbasData.Instance:GetTotalLevel() < info.equip_require then
		PublicPopupCtrl.Instance:Center(self.language.FightEquipTips)
		return
	end

    if not TeamData.Instance:InTeam() then
        PublicPopupCtrl.Instance:Center(string.format(Language.Team.NeedNumAndTeam, self.data:TeamNumMin()))
        TeamData.Instance.cur_platform_target_id = info.target_id
        TeamCtrl.Instance:SendTeamMatch(info.target_id, TeamConfig.CrossType.is_cross, TeamConfig.MatchType.start)
        MainViewData.Instance:ToggleTTTeam()
        return
    end
    if TeamData.Instance:MemberNumRole() < self.data:TeamNumMin() then
		PublicPopupCtrl.Instance:Center(string.format(Language.Team.NeedNumAndTeam, self.data:TeamNumMin()))
        self:GotoMatch(info)
        return
    end
    --组队且满足人数
	--[[ if not LoginData.Instance:IsOnCrossSever() then
		if self.care_handle ~= nil then
			LoginData.Instance:BaseData():Uncare(self.care_handle)
		end
		self.care_handle = LoginData.Instance:BaseData():KeysCare({"real_is_on_crossserver"},function ()
			if self.care_handle ~= nil then
				LoginData.Instance:BaseData():Uncare(self.care_handle)
			end
			if LoginData.Instance:IsOnCrossSever() then
				SceneCtrl.Instance:RequestEnterFb(FB_TYPE.HARMONY_HEAVEN, info.fb_type, info.seq)
			end
		end)
		LoginCtrl:SendCrossStartReq()
    else ]]
		SceneCtrl.Instance:RequestEnterFb(FB_TYPE.HARMONY_HEAVEN, info.fb_type, info.seq)
    --[[ end ]]
	-- SceneCtrl.Instance:RequestEnterFb(FB_TYPE.HARMONY_HEAVEN, info.fb_type, info.seq)
    -- ViewMgr:CloseView(HarmonyHeavenView)
end

function HarmonyHeavenViewF:OnClickTeam()
	-- AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	local info = self.data.fb_sm.info
	if info.init then return end

	local target = self.team_data:GetTeamGoalWithId(info.target_id)
	if LoginData.Instance:IsOnCrossSever() and TeamConfig.CrossType.not_cross == target.is_cross then
		PublicPopupCtrl.Instance:Center(Language.Team.MatchCrossError)
		return
	end
	if RoleData.Instance:GetRoleLevel() < target.low_level then
		PublicPopupCtrl.Instance:Center(Language.Team.MatchLevelError)
		return
	end

    if TeamData.Instance:InTeam() then
        if TeamData.Instance:IsLeader() then
            self:GotoMatch(info)
        else
            ViewMgr:OpenView(TeamView)
        end
    else
        TeamData.Instance.cur_platform_target_id = info.target_id
        TeamData.Instance.auto_sel_target = true
        ViewMgr:OpenView(TeamPlatformView)
    end
end

function HarmonyHeavenViewF:GotoMatch(info)
    local target = TeamData.Instance:GetTeamGoalWithId(info.target_id)
    TeamCtrl.Instance:SendTeamMatch(TeamData.Instance.cur_platform_target_id, TeamConfig.CrossType.is_cross, TeamConfig.MatchType.cancel)
    TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
    TeamData.Instance.cur_platform_target_id = info.target_id
    TeamCtrl.Instance:SendTeamMatch(info.target_id, TeamConfig.CrossType.is_cross, TeamConfig.MatchType.start)
    ViewMgr:OpenView(TeamView)
end

function HarmonyHeavenViewF:OnClickRecord()
	local info = self.data.fb_sm.info
	if info.init then return end
	HarmonyHeavenCtrl.Instance:SendHongMengTianJieReqPassTeam(info.fb_id)
	-- ViewMgr:OpenView(HarmonyHeavenRecordView)
end

function HarmonyHeavenViewF:OnClickDiffTips()
	self.DiffTipsObj:SetActive(true)
end

function HarmonyHeavenViewF:OnClickDiffTips()
	self.DiffTipsObj:SetActive(true)
end

function HarmonyHeavenViewF:OnBlockDiffTips()
	self.DiffTipsObj:SetActive(false)
end

function HarmonyHeavenViewF:OnClickHelpTips()
	self.HelpTipsObj:SetActive(true)
end

function HarmonyHeavenViewF:OnBlockHelpTips()
	self.HelpTipsObj:SetActive(false)
end


HarmonyHeavenViewItem = HarmonyHeavenViewItem or DeclareMono("HarmonyHeavenViewItem", UIWidgetBaseItem)

function HarmonyHeavenViewItem:HarmonyHeavenViewItem()
	self.hh_data = HarmonyHeavenCtrl.Instance:Data()
	self.language = Language.HarmonyHeaven.Main
end

function HarmonyHeavenViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local is_lock1 = MarbasData.Instance:GetTotalLevel() < data.equip_require
	local is_lock2 = true
	if not is_lock1 then
		is_lock2 = 0 == data.is_open or (2 == data.fb_type and 0 == self.hh_data.fb_info.role_info.fb_first_pass_mark[data.lock_id])
	end
	self.NameShow1.text = data.fb_name
	self.NameShow2.text = data.fb_name
	self.NameShow3.text = data.fb_name
	self.EquipShow.text = string.format(self.language.EquipRequire, is_lock1 and COLORSTR.Red8 or COLORSTR.Green3, data.equip_require)
	self.SuoObj:SetActive(is_lock2)
	self.BgNLObj:SetActive(is_lock2)
	self.TogObj:SetActive(not is_lock)
	self.BlockObj:SetActive(0 == data.is_open)
end

function HarmonyHeavenViewItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function HarmonyHeavenViewItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected then
		if 2 == self.data.fb_type and 0 == self.hh_data.fb_info.role_info.fb_first_pass_mark[self.data.lock_id] then
			PublicPopupCtrl.Instance:Center(self.language.LockTips2)
		end
        self.hh_data.fb_sm.info:Set(self.data)
    end
end

function HarmonyHeavenViewItem:OnClickBlock()
    PublicPopupCtrl.Instance:Center(self.language.NotOpen)
end


HarmonyHeavenViewMonsterItem = DeclareMono("HarmonyHeavenViewMonsterItem", UIWidgetBaseItem)

function HarmonyHeavenViewMonsterItem:HarmonyHeavenViewMonsterItem()
	self.hh_data = HarmonyHeavenCtrl.Instance:Data()
	self.language = Language.HarmonyHeaven.Main
end

function HarmonyHeavenViewMonsterItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetIcon(self.IconSp, data.icon_id, ICON_TYPE.ITEM)
	local info = self.hh_data.fb_info.role_info
	if info.init then return end
	local boss_reward_limit = self.hh_data:ConfigOtherBossRewardLimit()
	local reward_times_show = info.reward_times[data.index]
	self.TimesShow.text = string.format(self.language.TimesShow, RichTextHelper.ColorStr(reward_times_show, reward_times_show > 0 and COLORSTR.Green3 or COLORSTR.Red8), boss_reward_limit)
end

HarmonyHeavenViewRewardItem = DeclareMono("HarmonyHeavenViewRewardItem", UIWidgetBaseItem)

function HarmonyHeavenViewRewardItem:HarmonyHeavenViewRewardItem()
	self.hh_data = HarmonyHeavenCtrl.Instance:Data()
end

function HarmonyHeavenViewRewardItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	-- local prof_type = RoleData.Instance:ProfType()
	-- prof_type = prof_type > 0 and prof_type or 1
	-- if data.old_id then
	-- 	local co = Cfg.RewardJobBagByIdType(data.old_id, prof_type)
	-- 	data.item_id = co and co.new_id or data.item_id
	-- 	-- item.show_key = "workshop_equip"
	-- end
    self.Cell:SetData(data)
	self.FirstShow:SetActive(data.is_hh_first)
	self.GetObj:SetActive(data.is_hh_first and 1 == (self.hh_data.fb_info.role_info.fb_first_pass_mark[data.is_hh_id] or 0))

	if self.effect_handle ~= nil then
        self.EffectTool:Stop(self.effect_handle)
		self.effect_handle = nil
    end
	self.effect_handle = self.EffectTool:Play(3161114)
end

HarmonyHeavenViewRewardItem2 = DeclareMono("HarmonyHeavenViewRewardItem2", UIWidgetBaseItem)

function HarmonyHeavenViewRewardItem2:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local prof_type = RoleData.Instance:ProfType()
	prof_type = prof_type > 0 and prof_type or 1
	if data.old_id then
		local co = Cfg.RewardJobBagByIdType(data.old_id, prof_type)
		data.item_id = co and co.new_id or data.item_id
		-- item.show_key = "workshop_equip"
	end
    self.Cell:SetData(Item.Create(data))
end