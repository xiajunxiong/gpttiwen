
SocietyView = SocietyView or DeclareView("SocietyView", SocietyConfig.ResPath .. "society",Mod.Society.Friend)
VIEW_DECLARE_LEVEL(SocietyView,ViewLevel.L1)

function SocietyView:SocietyView()
	self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society

	GuideManager.Instance:RegisterGetGuideUi("SocietyViewTabMA", function ()
		return self.Board.Tabbar.item_list[3], function ()
			self.Board.Tabbar:ClickItem(3)
		end
	end)
end
 
function SocietyView:LoadCallback(param_t)
	param_t = param_t or {}
	self.Board:SetData(self:GetType(),self.language.Society)
	self.Currency:DefaultSet()

	self.Board:SetTabbarVisible(true)
	local tabbar = self.Board.Tabbar
	if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
		tabbar:SetData({
			{name = self.language.SocietyFriend, tab_objs = {self.TabBg1, self.TabFriendGroup, self.TabChat}, mod = TabMod(Mod.Society.Friend), open = Mod.Society.Friend},
			{name = self.language.SocietyMail, tab_objs = {self.TabBg1, self.TabMailLeft, self.TabMailDetail}, mod = TabMod(Mod.Society.Mail), open = Mod.Society.Mail},
		})
	else
		tabbar:SetData({
			{name = self.language.SocietyFriend, tab_objs = {self.TabBg1, self.TabFriendGroup, self.TabChat}, mod = TabMod(Mod.Society.Friend), open = Mod.Society.Friend},
			{name = self.language.SocietyMail, tab_objs = {self.TabBg1, self.TabMailLeft, self.TabMailDetail}, mod = TabMod(Mod.Society.Mail), open = Mod.Society.Mail},
			{name = self.language.SocietyMasterApprentice, tab_objs = {self.TabBg1, self.TabMasterApprenticeGroup, self.TabDeclaration, self.TabShiTuInfo}, mod = TabMod(Mod.Society.Master), open = Mod.Society.Master},
			{name = self.language.SocietySworn, tab_objs = {self.TabSworn}, mod = TabMod(Mod.Society.Sworn), open = Mod.Society.Sworn},
			{name = self.language.SocietyMarry, tab_objs = {self.TabMarry}, mod = TabMod(Mod.Society.Marry), open = Mod.Society.Marry},
			-- {name = self.language.SocietyFirstKill, tab_objs = {self.TabBg2, self.TabFirstKill}, mod = TabMod(Mod.Society.FirstKill), open = Mod.Society.FirstKill},
			-- {name = self.language.SocietyRank, tab_objs = {self.TabBg2, self.TabRank}, mod = TabMod(Mod.Society.Rank), open = Mod.Society.Rank},
		})
	end

	tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Society, tabbar, param_t.open_param))
	SocietyCtrl.Instance:SendFriendInfo()
	SocietyCtrl.Instance:SendShiTuSeekingInfo()
	SwornCtrl.Instance:SendSeekingInfo()
end

function SocietyView:StartOpening(callback)
	BaseView.StartOpening(self, callback)
	self.ChatTabObj:SetActive(true)
end

function SocietyView:StartClosing(callback)
	self.ChatTabObj:SetActive(false)
	BaseView.StartClosing(self, callback)
end