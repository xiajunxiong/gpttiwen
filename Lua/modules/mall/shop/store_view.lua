StoreView = StoreView or DeclareView("StoreView", "mall/store/StoreView",Mod.Store.Main)

VIEW_DECLARE_LEVEL(StoreView,ViewLevel.L0)

function StoreView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(), Language.Store.Title)
    self.Board:SetTabbarVisible(true)
    if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
        self.Board.TabbarAudit:SetData(
            {
                {name = Language.Store.Tab3, tab_objs = {self.Tab3},open = Mod.Store.Recharge},
            }
        )
    elseif GLOBAL_CONFIG:Flags().block_recharge then
        self.Board.Tabbar:SetData(
            {
                {name = Language.Store.Tab1, tab_objs = {self.Tab1},open = Mod.Store.Main,mod = TabMod(Mod.Store.Main)},
                {name = Language.Store.Tab2, tab_objs = {self.Tab2},open = Mod.Store.Gold},
            }
        )
    else
        local tabbar_t = {
            {name = Language.Store.Tab1, tab_objs = {self.Tab1},open = Mod.Store.Main,mod = TabMod(Mod.Store.Main)},
            {name = Language.Store.Tab2, tab_objs = {self.Tab2},open = Mod.Store.Gold},
            {name = Language.Store.Tab3, tab_objs = {self.Tab3},open = Mod.Store.Recharge},
        }
        if FunOpen.Instance:IsFunOpen(Mod.Store.Diamond) then
            table.insert(tabbar_t,{name = Language.Store.Tab4, tab_objs = {self.Tab4, self.DiamondTip},open = Mod.Store.Diamond})
        end
        self.Board.Tabbar:SetData(tabbar_t)
    end
    if param_t.open_param and param_t.open_param > 10 then
        local open_param = math.floor(param_t.open_param / 10) % 10
        StoreData.Instance:SetJumpIndex({conver_type = param_t.open_param % 10})
        if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
            self.Board.TabbarAudit:ClickItem(FunOpen.Instance:IWantSelect(Mod.Store, self.Board.TabbarAudit, open_param))
        else
            self.Board.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Store, self.Board.Tabbar, open_param))
        end
        
    else
        if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
            self.Board.TabbarAudit:ClickItem(FunOpen.Instance:IWantSelect(Mod.Store, self.Board.TabbarAudit, param_t.open_param))
        else
            self.Board.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Store, self.Board.Tabbar, param_t.open_param))
        end
    end
    if FunOpen.Instance:IsFunOpen(Mod.Store.Diamond) then
        self.Currency:SetCurrency(CurrencyType.CoinBind,CurrencyType.ImmortalCoin,CurrencyType.Gold, CurrencyType.Diamond)
    else
        self.Currency:DefaultSet()
    end
end

function StoreView:OnClickDiamondTip()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[419].desc)
end

function StoreView:CloseCallback()
    StoreData.Instance:Initialization()
    if StoreData.Instance.IsClickRemind == nil then
        StoreData.Instance.IsClickRemind = true
        StoreData.Instance:CheckRedPiont()
    end
    TaskData.shop_auto_close = nil
    if ViewMgr:IsOpen(ArenaView) then
        ArenaData.Instance.shop_smart:SetDirty("is_change")
    end
end