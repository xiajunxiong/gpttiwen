HeroVanBattleGloryView = HeroVanBattleGloryView or DeclareView("HeroVanBattleGloryView", "hero_van_battle/hero_van_battle_glory_view",Mod.HeroVanBattle.GloryView)
-- VIEW_DECLARE_LEVEL(HeroVanBattleTeamView,ViewLevel.L0)
function HeroVanBattleGloryView:HeroVanBattleGloryView()
    if LoginData.Instance:BaseData().is_on_crosssever == false then 
        LoginCtrl.Instance:SendCrossStartReq()

        self.auto_delay = TimeHelper:AddDelayTimer(function()
            HeroVanBattleCtrl.Instance:SendTeamTeamArenaRankReq()
         end,3)
    else 
        HeroVanBattleCtrl.Instance:SendTeamTeamArenaRankReq()
    end 
end

function HeroVanBattleGloryView:OnClickClose()
    ViewMgr:CloseView(HeroVanBattleGloryView)
end 


HeroVanBattleGloryPanel = HeroVanBattleGloryPanel or DeclareMono("HeroVanBattleGloryPanel", UIWFlushPanel)

function HeroVanBattleGloryPanel:HeroVanBattleGloryPanel()
    self.data = HeroVanBattleData.Instance

    self.data_cares = {
        {data = self.data.finally_rank_data, func = self.FlushShow,keys = {"flush"},init_call = false},
    }
end 

function HeroVanBattleGloryPanel:OnClickShowMember(data)
    HeroVanBattleCtrl.Instance:SendTeamArenaReq(4,data.team_unique_id)
end 

function HeroVanBattleGloryPanel:FlushShow()
    self.rank_list:SetData(self.data.finally_rank_data.rank_list)

    UH.SetText(self.my_team_name, Language.HeroVanBattle.RankMyTeam..self.data.finally_rank_data.my_rank_info.team_name)
    UH.SetText(self.my_rank, Language.HeroVanBattle.RankMyTeamRank..self.data.finally_rank_data.my_rank_index)
    UH.SetText(self.my_point, Language.HeroVanBattle.RankMyCurPoint..self.data.finally_rank_data.my_rank_info.total_score)
    local rate = self.data.finally_rank_data.my_rank_info.fight_times > 0 and math.floor((self.data.finally_rank_data.my_rank_info.victory_times / self.data.finally_rank_data.my_rank_info.fight_times)*100) or 0
    UH.SetText(self.my_rate, Language.HeroVanBattle.RankMyTeamRate..rate.."%")
    self.my_reward:SetData(self.data:GetRankReward(self.data.finally_rank_data.my_rank_index))

    self.my_is_success:SetActive(self.data.finally_rank_data.my_rank_index <= 10 and self.data.finally_rank_data.my_rank_index >0)
end 

VanGloryRankTeamCell = VanGloryRankTeamCell or DeclareMono("VanGloryRankTeamCell",UIWidgetBaseItem)
function VanGloryRankTeamCell:VanGloryRankTeamCell()
    self.hvb_data = HeroVanBattleData.Instance
end 

function VanGloryRankTeamCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.rank, data.rank)

    self.rank_1:SetActive(data.rank == 1)
    self.rank_2:SetActive(data.rank == 2)
    self.rank_3:SetActive(data.rank == 3)
    self.rank_n:SetActive(data.rank > 3)

    UH.SetText(self.team_name,data.team_name)
    UH.SetText(self.point,data.total_score)
    local rate = data.fight_times > 0 and math.floor((data.victory_times / data.fight_times)*100) or 0
    UH.SetText(self.rate,rate.."%".."("..data.victory_times.."/"..data.fight_times..")")

    self.rewards:SetData(BagData.Instance:ItemListBuild(self.hvb_data:GetRankReward(data.rank)))

    self.is_success:SetActive(data.rank <= 10)
end 

VanGloryTeamRewardItem = VanGloryTeamRewardItem or DeclareMono("VanGloryTeamRewardItem",UIWidgetBaseItem)
function VanGloryTeamRewardItem:VanGloryTeamRewardItem()
    self.hvb_data = HeroVanBattleData.Instance
end

function VanGloryTeamRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.call:SetData(Item.Create(data))
end 
