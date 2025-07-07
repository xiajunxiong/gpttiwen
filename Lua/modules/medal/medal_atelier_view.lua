MedalAtelierView  = MedalAtelierView or DeclareView("MedalAtelierView", "medal/medal_atelier",Mod.Medal.Build)
VIEW_DECLARE_MASK(MedalAtelierView,ViewMask.BgBlockClose)
function MedalAtelierView:MedalAtelierView()
    self.Board:SetData(self:GetType(), Language.Medal.AtelierView, Vector2.New(970, 592))
    
end

function MedalAtelierView:LoadCallback(param_t)
end

function MedalAtelierView:CloseCallback()
    PublicPopupCtrl.Instance:RemoveInterceptRollingFunc(self.ht)
    MedalData.Instance:SetSpMedalMarqueeMsg(nil)
end