HeroVanBattleNeoRankView = HeroVanBattleNeoRankView or DeclareView("HeroVanBattleNeoRankView", "hero_van_battle/hero_van_battle_neo_rank")
-- VIEW_DECLARE_LEVEL(HeroVanBattleTeamView,ViewLevel.L0)
function HeroVanBattleNeoRankView:HeroVanBattleNeoRankView()

end

function HeroVanBattleNeoRankView:OnClickClose()
    HeroVanBattleCtrl.Instance:SendTeamTeamArenaSceneReq(1)
    ViewMgr:CloseView(HeroVanBattleNeoRankView)
end 


HeroVanBattleNeoRankPanel = HeroVanBattleNeoRankPanel or DeclareMono("HeroVanBattleNeoRankPanel", UIWFlushPanel)

function HeroVanBattleNeoRankPanel:HeroVanBattleNeoRankPanel()
    self.data = HeroVanBattleData.Instance
    self.data_cares = {
        {data = self.data.neo_rank_data, func = self.FlushShow,keys = {"flush"}},
    }

end 

function HeroVanBattleNeoRankPanel:OnClickShowMember(data)
    HeroVanBattleCtrl.Instance:SendTeamArenaReq(3,data.team_unique_id,1)
end 

function HeroVanBattleNeoRankPanel:FlushShow()
    if #self.rank_list.item_list == 0 then 
        self.rank_list:SetData(self.data.neo_rank_data.rank_list)
    else 
        for k,v in pairs(self.rank_list.item_list) do 
            v:SetData(self.data.neo_rank_data.rank_list[k])
        end 
    end 

    UH.SetText(self.my_team_name, Language.HeroVanBattle.RankMyTeam..self.data.battle_scene_info.my_info.team_name)
    UH.SetText(self.my_rank, Language.HeroVanBattle.RankMyTeamRank..self.data.battle_scene_info.rank_posi)
    UH.SetText(self.my_point, Language.HeroVanBattle.RankMyCurPoint..self.data.battle_scene_info.my_info.total_score)
    local rate = self.data.battle_scene_info.my_info.fight_times == 0 and 0 or math.floor((self.data.battle_scene_info.my_info.victory_times / self.data.battle_scene_info.my_info.fight_times)*100)
    UH.SetText(self.my_rate, Language.HeroVanBattle.RankMyTeamRate..rate.."%")
    self.my_reward:SetData(self.data:GetRankReward(self.data.battle_scene_info.rank_posi))
end

VanNeoRankTeamCell = VanNeoRankTeamCell or DeclareMono("VanNeoRankTeamCell",UIWidgetBaseItem)
function VanNeoRankTeamCell:VanNeoRankTeamCell()
    self.hvb_data = HeroVanBattleData.Instance
end

function VanNeoRankTeamCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.rank.gameObject:SetActive(self.data.rank > 3)
    self.rank_icon.gameObject:SetActive(self.data.rank <= 3)
    UH.SetText(self.rank, data.rank)
    UH.SpriteName(self.rank_icon, "hvb_PaiMing"..data.rank.."Xiao")
    UH.SetText(self.team_name,data.team_name)
    UH.SetText(self.team_point,data.total_score)
    local rate = data.fight_times == 0 and 0 or math.floor((data.victory_times / data.fight_times)*100)
    UH.SetText(self.team_rate,rate.."%".."("..data.victory_times.."/"..data.fight_times..")")

    self.rewards:SetData(BagData.Instance:ItemListBuild(self.hvb_data:GetRankReward(data.rank)))
end 