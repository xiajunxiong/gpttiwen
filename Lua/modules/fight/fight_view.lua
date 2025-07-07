
FILE_REQ("modules/fight/fight_view_field")
FILE_REQ("modules/fight/fight_view_exp")
FILE_REQ("modules/fight/fight_view_field_exp")
FILE_REQ("modules/fight/fight_view_dungeon")
FILE_REQ("modules/fight/fight_view_gather")
FILE_REQ("modules/fight/fight_view_pet_island")
FILE_REQ("modules/fight/fight_view_pet_island_special_item")

FightView = FightView or DeclareView("FightView", FightConfig.ResPath .. "fight", Mod.Fight.Field)
VIEW_DECLARE_LEVEL(FightView,ViewLevel.L1)

function FightView:FightView()
	self.data = FightCtrl.Instance:Data()
	self.language = Language.Fight

	GuideManager.Instance:RegisterGetGuideUi("FightViewTabDungeon", function ()
		return self.Board.Tabbar.item_list[2], function ()
			self.Board.Tabbar:ClickItem(2)
		end
	end)
end

function FightView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(), self.language.Fight)
	self.Board:SetEffectActive(UIWBoard1.Effect.LBLight, false)
	self.Board:SetTabbarVisible(true)

	local tabbar = self.Board.Tabbar
	tabbar:SetData({
			-- {name = self.language.FightDungeon, tab_objs = {self.TabDungeon}, mod = TabMod(Mod.Fight.Dungeon), open = Mod.Fight.Dungeon },
			{name = self.language.FightField, tab_objs = {self.TabFieldExp, self.TabField}, mod = TabMod(Mod.Fight.Field), open = Mod.Fight.Field},
			--{name = self.language.FightGather, tab_objs = {self.TabGather}, mod = TabMod(Mod.Fight.Gather), open = Mod.Fight.Gather},
			-- {name = self.language.FightExp, tab_objs = {self.TabFieldExp, self.TabExp}, mod = TabMod(Mod.Fight.Exp) ,open = Mod.Fight.Exp},

			})
	tabbar:ClickItem(param_t.open_param and FunOpen.Instance:IWantSelect(Mod.Fight, tabbar, param_t.open_param) or (FunOpen.Instance:GetFunIsOpened(Mod.Fight.Dungeon) and 1 or 2))
	self.Currency:DefaultSet()
end

function FightView:CloseCallback()
	self.data.energy_effect.val = not self.data.energy_effect.val
end