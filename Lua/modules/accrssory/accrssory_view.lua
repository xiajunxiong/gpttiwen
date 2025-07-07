AccrssoryView = AccrssoryView or DeclareView("AccrssoryView", "accrssory/accrssory_view", Mod.Accrssory)
VIEW_DECLARE_LEVEL(AccrssoryView, ViewLevel.L1)
function AccrssoryView:AccrssoryView()
    self.language = Language.Accrssory
end

function AccrssoryView:LoadCallback(param_t)
    self.Tabbar:SetData(
        {
            {name = self.language.TabTitle[1], tab_objs = {self.TabList[1], self.ObjAccrssoryInfoPanel}, open = Mod.Accrssory.Main, mod = TabMod(Mod.Accrssory.Main)},
            {name = self.language.TabTitle[5], tab_objs = {self.TabList[5], self.ObjAccrssoryInfoPanel}, open = Mod.Accrssory.Strength, mod = TabMod(Mod.Accrssory.Strength)},
            {name = self.language.TabTitle[2], tab_objs = {self.TabList[2], self.ObjAccrssoryInfoPanel}, open = Mod.Accrssory.Inherit, mod = TabMod(Mod.Accrssory.Inherit)},
            {name = self.language.TabTitle[3], tab_objs = {self.TabList[3]}, open = Mod.Accrssory.Destiny, mod = TabMod(Mod.Accrssory.Destiny)},
            {name = self.language.TabTitle[4], tab_objs = {self.TabList[4]}, open = Mod.Accrssory.Illustration, mod = TabMod(Mod.Accrssory.Illustration)},
        }
    )
    self.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Accrssory, self.Tabbar, param_t))
    -- self.Currency:DefaultSet()
end

function AccrssoryView:OnHelpClick()
    if AccrssoryData.Instance:IsCanClickFly() then
        PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[211].desc)
    else
        PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[166].desc)
    end
end

function AccrssoryView:CloseCallback()
    
end

function AccrssoryView:OnCloseClick()
    ViewMgr:CloseView(AccrssoryView)
end