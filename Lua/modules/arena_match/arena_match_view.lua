
FILE_REQ("modules/arena_match/arena_match_view_arena")
FILE_REQ("modules/arena_match/arena_match_view_team")
FILE_REQ("modules/arena_match/arena_match_view_reward")

ArenaMatchView = ArenaMatchView or DeclareView("ArenaMatchView", ArenaMatchConfig.ResPath .. "arena_match", Mod.ArenaMatch.Arena)
VIEW_DECLARE_LEVEL(ArenaMatchView,ViewLevel.L0)

function ArenaMatchView:ArenaMatchView()
	self.data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch
end

function ArenaMatchView:LoadCallback(param_t)
	self.Board:SetData(self:GetType())

	self.Board:SetTabbarVisible(true)
	local tabbar = self.Board.Tabbar
	tabbar:SetData({
		{name = self.language.ArenaMatchArena, tab_objs = {self.TabArena}},
		{name = self.language.ArenaMatchTeam, tab_objs = {self.TabTeam}},
		{name = self.language.ArenaMatchReward, tab_objs = {self.TabReward},open = Mod.ArenaMatch.Reward, is_self_img = true},
	})
	tabbar:ClickItem(param_t.open_param or 1)

	if param_t.is_open_reward then
		self.time_ht = Invoke(function ()
			ViewMgr:OpenView(ArenaMatchRewardPreviewView,{show_enter_btn = true})
		end,1)
	end
end

function ArenaMatchView:CloseCallback()
	TimeHelper:CancelTimer(self.time_ht)
	self.data.cur_sel_preview.val = -1
end

ArenaMatchMainIconSub = ArenaMatchMainIconSub or DeclareMono("ArenaMatchMainIconSub", UIWFlushPanel)
function ArenaMatchMainIconSub:ArenaMatchMainIconSub()
	
end

function ArenaMatchMainIconSub:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self:FlushIcon()
end

function ArenaMatchMainIconSub:FlushIcon()
	UH.SetText(self.Name,Language.ArenaMatch.Title)
	local time = ArenaMatchData.Instance.EndTime.time or 0
	self.TimeMater:SetShowCallBack(function (t)
        local t_str = ""
		if t.day > 0 then
			t_str = string.format(Language.ArenaMatch.TimeShow1,t.day)
		elseif t.hour > 0 then
			t_str = string.format(Language.ArenaMatch.TimeShow2,t.hour)
		else
			t_str = string.format(Language.ArenaMatch.TimeShow3,t.min)
		end
        return t_str
	end)
	self.TimeMater:TotalTime(time, TimeType.Type_Time_6)
end

function ArenaMatchMainIconSub:OnClickIcon()
	ViewMgr:OpenView(ArenaMatchView, {open_param = 3})
	ArenaMatchData.Instance.cur_sel_preview.val = 0
end