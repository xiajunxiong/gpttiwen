HeroVanBattlePreShow = HeroVanBattlePreShow or DeclareView("HeroVanBattlePreShow", "hero_van_battle/hero_van_battle_preshow", Mod.HeroVanBattle.PreShow)
VIEW_DECLARE_LEVEL(HeroVanBattlePreShow,ViewLevel.L1)
function HeroVanBattlePreShow:OnClickClose()
    ViewMgr:CloseView(HeroVanBattlePreShow)
end 

HeroVanBattlePreShowPanel = HeroVanBattlePreShowPanel or DeclareMono("HeroVanBattlePreShowPanel", UIWFlushPanel)
function HeroVanBattlePreShowPanel:HeroVanBattlePreShowPanel()
    self.data = HeroVanBattleData.Instance
    -- 取npc上有没有报名按钮
    self.enrolling:SetActive( self.data:TeamViewNpcCouldShow() and not self.data:EnterSceneNpcCouldShow())
end

function HeroVanBattlePreShowPanel:OnClickMatchRules()
    ViewMgr:OpenView(HeroVanBattleView)
    ViewMgr:CloseView(HeroVanBattlePreShow)
end 