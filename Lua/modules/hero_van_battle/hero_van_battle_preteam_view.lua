HeroVanBattlePreTeamView = HeroVanBattlePreTeamView or DeclareView("HeroVanBattlePreTeamView", "hero_van_battle/hero_van_battle_preteam")
-- VIEW_DECLARE_LEVEL(HeroVanBattleTeamView,ViewLevel.L0)
function HeroVanBattlePreTeamView:HeroVanBattlePreTeamView() end
function HeroVanBattlePreTeamView:OnClickClose()
    ViewMgr:CloseView(HeroVanBattlePreTeamView)
end 

HeroVanBattlePreTeamPanel = HeroVanBattlePreTeamPanel or DeclareMono("HeroVanBattlePreTeamPanel", UIWFlushPanel)
function HeroVanBattlePreTeamPanel:HeroVanBattlePreTeamPanel()
    self.data = HeroVanBattleData.Instance

    UH.SetText(self.team_name, self.data.team_sign_up_detail_info.team_name)
    self.member_list:SetData(self.data.team_sign_up_detail_info.member_list)

    self.member_list:SetCompleteCallBack(function()
        for k,v in pairs(self.member_list.item_list) do
            v:SetMaster(k == 1)
        end 
    end)
end 