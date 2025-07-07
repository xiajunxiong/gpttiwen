
RankView = RankView or DeclareView("RankView", SocietyConfig.ResPath .. "rank", Mod.Rank.Main)
VIEW_DECLARE_LEVEL(RankView,ViewLevel.L1)

function RankView:RankView()
	self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society

	-- GuideManager.Instance:RegisterGetGuideUi("SocietyViewTabMA", function ()
	-- 	return self.Board.Tabbar.item_list[3], function ()
	-- 		self.Board.Tabbar:ClickItem(3)
	-- 	end
	-- end)
end
 
function RankView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(),self.language.SocietyRank)
	self.Currency:DefaultSet()

	self.Board:SetTabbarVisible(true)
	local tabbar = self.Board.Tabbar
	tabbar:SetData({
			{name = self.language.SocietyRank, tab_objs = {self.TabBg2, self.TabRank}, mod = TabMod(Mod.Rank.Main), open = Mod.Rank.Main},
			{name = self.language.SocietyFirstKill, tab_objs = {self.TabBg2, self.TabFirstKill}, mod = TabMod(Mod.Rank.FirstKill), open = Mod.Rank.FirstKill},
		})
	tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Rank, tabbar, param_t.open_param))
end