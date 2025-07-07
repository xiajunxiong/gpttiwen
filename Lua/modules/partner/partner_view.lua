
FILE_REQ("modules/partner/partner_view_show_list")
FILE_REQ("modules/partner/partner_view_attr")
FILE_REQ("modules/partner/partner_view_up")
FILE_REQ("modules/partner/partner_view_model")
FILE_REQ("modules/partner/partner_view_quality")
FILE_REQ("modules/partner/partner_view_contract")
FILE_REQ("modules/partner/partner_view_strengthen")

PartnerView = PartnerView or DeclareView("PartnerView", PartnerConfig.ResPath .. "partner", Mod.Partner.PartnerAttr)
VIEW_DECLARE_LEVEL(PartnerView,ViewLevel.L1)

function PartnerView:PartnerView()
	self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner

	GuideManager.Instance:RegisterGetGuideUi("PartnerViewTabUp", function ()
		return self.Board.Tabbar.item_list[2], function ()
			self.Board.Tabbar:ClickItem(2)
		end
	end)

	GuideManager.Instance:RegisterGetGuideUi("PartnerViewTabQuality", function ()
		return self.Board.Tabbar.item_list[4], function ()
			self.Board.Tabbar:ClickItem(4)
		end
	end)

	GuideManager.Instance:RegisterGetGuideUi("PartnerViewTabLife", function ()
		return self.Board.Tabbar.item_list[6], function ()
			-- self.Board.Tabbar:ClickItem(6)
		end
	end)
end

function PartnerView:LoadCallback(param_t)
	param_t = param_t or {}
	self.Board:SetData(self:GetType(), self.language.Partner)
	self.Board:SetCompetitionRank(ServeractivityConfig.CompetitionRank.Type.Partner)
	self.Board:SetTabbarVisible(true)
	if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
		self.Board.Tabbar:SetData({
			{name = self.language.PartnerAttr, tab_objs = {self.TabShowList, self.TabAttr}, mod = TabMod(Mod.Partner.PartnerAttr), open = Mod.Partner.PartnerAttr},
		})
	else
		self.Board.Tabbar:SetData({
			{name = self.language.PartnerAttr, tab_objs = {self.TabShowList, self.TabAttr}, mod = TabMod(Mod.Partner.PartnerAttr), open = Mod.Partner.PartnerAttr},
			{name = self.language.PartnerUp, tab_objs = {self.TabShowList, self.TabUp, self.TabOffset}, mod = TabMod(Mod.Partner.PartnerUp), open = Mod.Partner.PartnerUp },
			{name = self.language.PartnerStrengthen, tab_objs = {self.TabShowList, self.TabStrengthen, self.TabOffsetS}, mod = TabMod(Mod.Partner.PartnerStrengthen), open = Mod.Partner.PartnerStrengthen},
			{name = self.language.PartnerQuality,tab_objs = {self.TabShowList,self.TabQua,self.TabOffset},mod = TabMod(Mod.Partner.PartnerQuality), open = Mod.Partner.PartnerQuality},
			{name = self.language.PartnerGift, tab_objs = {self.TabShowList, self.TabGift}, mod = TabMod(Mod.Partner.PartnerGift), open = Mod.Partner.PartnerGift},
			-- {name = self.language.PartnerLife, tab_objs = {self.TabShowList, self.TabLife, self.TabOffset}, mod = TabMod(Mod.Partner.PartnerLife), open = Mod.Partner.PartnerLife},
			{name = self.language.PartnerContract,tab_objs = {self.TabShowList,self.TabContract},mod = TabMod(Mod.Partner.PartnerContract), open = Mod.Partner.PartnerContract},
		})
	end
	self.Board.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Partner, self.Board.Tabbar, param_t.open_param))

	self.handle_currencys = self.data.cur_sel_tabbar:Care(BindTool.Bind(self.FlushCurrencyShow, self))


	PartnerCtrl.Instance:SendPatnerPetRankReqPartner()
end

function PartnerView:CloseCallback()
	self.data.cur_sel_tabbar.val = PartnerConfig.PartnerViewTabs.None
	if self.handle_currencys then
        self.data.cur_sel_tabbar:Uncare(self.handle_currencys)
        self.handle_currencys = nil
    end
end

function PartnerView:FlushCurrencyShow()
	if PartnerConfig.PartnerViewTabs.Attr == self.data.cur_sel_tabbar.val then
		self.Currency:SetCurrency(CurrencyType.CoinBind, CurrencyType.ImmortalCoin, CurrencyType.Gold, CurrencyType.PartnerLevelItem)
	elseif PartnerConfig.PartnerViewTabs.Life == self.data.cur_sel_tabbar.val then
		self.Currency:SetCurrency(CurrencyType.CoinBind, CurrencyType.ImmortalCoin, CurrencyType.Gold, CurrencyType.LuckCoin)
	else
		self.Currency:DefaultSet()
	end
end