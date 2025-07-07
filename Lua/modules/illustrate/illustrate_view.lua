IllustrateView = IllustrateView or DeclareView("IllustrateView", "illustrate/illustrate",Mod.Illustrate.Main)

VIEW_DECLARE_LEVEL(IllustrateView,ViewLevel.L1)

function IllustrateView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(),Language.Illustrate.Main)
	self.Board:SetTabbarVisible(true)
	self.Board.Tabbar:SetData({
		{name = Language.Illustrate.Title,tab_objs = {self.Tab1},mod=TabMod(Mod.Illustrate.Main),open=Mod.Illustrate.Main},
		{name = Language.Illustrate.Collect,tab_objs = {self.Tab2},mod=TabMod(Mod.Illustrate.Collect),open=Mod.Illustrate.Collect},
		{name = Language.Illustrate.Rare.Title,tab_objs = {self.Tab3},mod=TabMod(Mod.Illustrate.Rare),open=Mod.Illustrate.Rare},
	})
	self.Board.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Illustrate, self.Board.Tabbar, param_t.open_param))
    self.Board:SetEffectActive(UIWBoard1.Effect.LBLight,not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_ILLUSTRATE_TREASURE))
	self.Currency:DefaultSet()

	UH.SetText(self.EnterName,RichTextHelper.LinkStr(ColorStr(Language.Illustrate.GoTo,COLORSTR.Green2),"_ul",COLORSTR.Green2))
	self.Treasure:SetActive(ServerActivityData.Instance:GetIllustrateTreasureEnd())
end

function IllustrateView:CloseCallback()
	IllustrateData.Instance:Initialization()
end

function IllustrateView:OnClickTreasure()
    ViewMgr:OpenView(IllustrateTreasureView)
end