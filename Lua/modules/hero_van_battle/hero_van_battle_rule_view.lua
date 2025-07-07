HeroVanBattleRuleView = HeroVanBattleRuleView or DeclareView("HeroVanBattleRuleView", "hero_van_battle/hero_van_battle_rule_view",Mod.HeroVanBattle.Rule)
-- VIEW_DECLARE_LEVEL(HeroVanBattleTeamView,ViewLevel.L0)
function HeroVanBattleRuleView:HeroVanBattleRuleView()
    -- self.btn_close:SetActive(not TeamData.Instance:IsLeader())

    if not HeroVanBattleData.Instance:MemberTeamCheck() then 
        if LoginData.Instance:IsOnCrossSever() then 
            PublicPopupCtrl.Instance:Center(Language.HeroVanBattle.TeamMemberError)
        end
        ViewMgr:CloseView(HeroVanBattleRuleView)
    else 
        if TeamData.Instance:IsLeader() then 
		    TombRaiderCtrl.Instance:SendLeaderEjectWindows({windows_type = Mod.HeroVanBattle.Rule})
		end 
    end 
end

function HeroVanBattleRuleView:OpenCallback()

end 

function HeroVanBattleRuleView:CloseCallback()

end 

function HeroVanBattleRuleView:OnClickClose()
    ViewMgr:CloseView(HeroVanBattleRuleView)
end

function HeroVanBattleRuleView:OnClickApply()
    HeroVanBattleData.Instance:TypeTeamViewEnroll()
    ViewMgr:CloseView(HeroVanBattleRuleView)
end