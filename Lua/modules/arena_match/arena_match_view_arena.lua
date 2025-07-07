
ArenaMatchViewArena = ArenaMatchViewArena or DeclareMono("ArenaMatchViewArena", UIWFlushPanel)

function ArenaMatchViewArena:ArenaMatchViewArena()
	self.data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch

	self.data_cares = {
		{data = self.data.role_info_list, func = self.FlushArenaInfo},
		{data = self.data.achievement_data, func = self.FlushAchievementRemind, init_call = true},
	}
	-- 设置是否打开过此界面，之前每打开过会显示红点
	local notice_data = self.data.platform_notice_data
	if not notice_data.is_first_times or (notice_data.is_first_times and notice_data.is_first_times == 1) then
		ArenaMatchCtrl.Instance:SendPlatformBattleReq({oper_type = ArenaMatchConfig.OperType.set_notice,p1 = 1})
		ArenaMatchCtrl.Instance:SendPlatformBattleReq({oper_type = ArenaMatchConfig.OperType.last_rank_info})
		self.ResetFlag:SetActive(true)
	else
		self.ResetFlag:SetActive(false)
	end
	
	GuideManager.Instance:RegisterGetGuideUi("XiFangQiSuBtn", function ()
        return self.Arenas[2]
    end)
end

function ArenaMatchViewArena:Awake()
	UIWFlushPanel.Awake(self)
	self.Time:SetShowCallBack(function (t)
        local t_str = ""
		if t.day > 0 then
			t_str = string.format(self.language.Time1,t.day, t.hour,t.min)
		else
			t_str = string.format(self.language.Time2,t.hour,t.min)
		end
        return t_str
	end)
	self.Time:TotalTime(self.data:GetDayEndTime(),TimeType.Type_Special_4, self.language.TimeTip)
end
function ArenaMatchViewArena:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self.Currency:SetCurrency(CurrencyType.Arena)
end

function ArenaMatchViewArena:FlushArenaInfo()
	self.ReportRemindObj:SetActive(self.data:GetRecordRemindNum() > 0)
	local list_data = self.data.config_pos
	for i = 1, self.Arenas:Length() do
		if list_data[i] then
			self.Arenas[i]:SetData(list_data[i])
		end
	end
end

function ArenaMatchViewArena:OnClickArena(seq)
	-- local arena = self.data.arena_pos_list_s[seq]
	-- PublicPopupCtrl.Instance:DialogTips{
	-- 	content = string.format(Language.ArenaMatch.GoTips, arena.desc),
	-- 	confirm = {
	-- 		func = function()
	-- 			SceneCtrl.Instance:RequestEnterFb(FB_TYPE.ARENA_MATCH, arena.seq)
	-- 			ViewMgr:CloseView(ArenaMatchView)
	-- 			ViewMgr:CloseView(DialogTipsView)
	-- 		end
	-- 	}
	-- }
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.LoginStartClick)
	SceneCtrl.Instance:RequestEnterFb(FB_TYPE.ARENA_MATCH, seq)
	ViewMgr:CloseView(ArenaMatchView)
end

function ArenaMatchViewArena:OnClickPreview()
	ViewMgr:OpenView(ArenaMatchRewardPreviewView)
end

function ArenaMatchViewArena:OnClickTips()
	local tips = Config.language_cfg_auto.textdesc[169].desc or ""
	PublicPopupCtrl.Instance:HelpTip(tips)
end

function ArenaMatchViewArena:OnClickReport()
	ViewMgr:OpenView(ArenaMatchFightReportView)
	self.ReportRemindObj:SetActive(false)
end

function ArenaMatchViewArena:OnClickStore()
	--ShoppingCtrl.Instance:OpenStoreView(3)
	StoreData.Instance:SetJumpIndex({conver_type = 3})
    ViewMgr:OpenViewByKey(Mod.Store.Main)
end

function ArenaMatchViewArena:OnClickAchieve()
	ViewMgr:OpenView(ArenaMatchAchieveView)
end

function ArenaMatchViewArena:FlushAchievementRemind()
    self.AchieveRedPoint:SetNum(self.data:GetAchievementRemind())
end

function ArenaMatchViewArena:OnOpenDetailClick(pos)
	ViewMgr:OpenView(ArenaMatchDetailView, {pos = pos})
end

function ArenaMatchViewArena:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	self.Time:CloseCountDownTime()
end

--  ^_^  ^_^  ^_^  ArenaMatchViewArenaItem == 废弃   ^_^  ^_^  ^_^
ArenaMatchViewArenaItem = ArenaMatchViewArenaItem or DeclareMono("ArenaMatchViewArenaItem", UIWidgetBaseItem)
-- data:擂台方位配置
function ArenaMatchViewArenaItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	UH.SetText(self.Name, data.desc)
	local show_info = ArenaMatchConfig.ShowInfo[data.seq]
	UH.SpriteName(self.Img, show_info.img)
	--UH.SpriteName(self.Qua, show_info.qua)
	self.gameObject.name = show_info.obj_name

	local list = ArenaMatchData.Instance:GetArenaHoldNumList()
	local num = list[data.seq]
	local desc = num and string.format(Language.ArenaMatch.ArenaInfoDescNum, num) or Language.ArenaMatch.ArenaInfoDescZero
	UH.SetText(self.Desc, desc)
	local show_reward_list = ArenaMatchData.Instance:GetShowRewardList(data.position)
	if self.Rewards then
		self.Rewards:SetData(show_reward_list)
	end
end

function ArenaMatchViewArenaItem:OnClick()
	ViewMgr:OpenView(ArenaMatchDetailView, self.data)
end

function ArenaMatchViewArenaItem:OnRewardClick()
	ArenaMatchData.Instance.cur_sel_preview.val = self.data.position
	ViewMgr:SelectTabbar(Mod.ArenaMatch.Reward)
end



ArenaMatchViewArenaMatchItem = ArenaMatchViewArenaMatchItem or DeclareMono("ArenaMatchViewArenaMatchItem", UIWidgetBaseItem)
function ArenaMatchViewArenaMatchItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local list = ArenaMatchData.Instance:GetArenaHoldNumList()
	local num = list[data.seq]
	local desc = num and string.format(Language.ArenaMatch.ArenaInfoDescNum, num) or Language.ArenaMatch.ArenaInfoDescZero
	UH.SetText(self.Desc, desc)
	local show_reward_list = ArenaMatchData.Instance:GetShowRewardList(data.position)
	if self.Rewards then
		self.Rewards:SetData(show_reward_list)
	end
end

function ArenaMatchViewArenaMatchItem:OnClick()
	ViewMgr:OpenView(ArenaMatchDetailView, self.data)
end

function ArenaMatchViewArenaMatchItem:OnRewardClick()
	ArenaMatchData.Instance.cur_sel_preview.val = self.data.position
	ViewMgr:SelectTabbar(Mod.ArenaMatch.Reward)
end