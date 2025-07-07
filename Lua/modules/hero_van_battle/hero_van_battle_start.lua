HeroVanBattleStart = HeroVanBattleStart or DeclareView("HeroVanBattleStart", "hero_van_battle/hero_van_battle_start")
-- VIEW_DECLARE_LEVEL(HeroVanBattleTeamView,ViewLevel.L0)
function HeroVanBattleStart:HeroVanBattleStart() end
function HeroVanBattleStart:OnClickClose()
    ViewMgr:CloseView(HeroVanBattleStart)
    BattleCtrl.SendPrepareFinish()
end 

HeroVanBattleStartPanel = HeroVanBattleStartPanel or DeclareMono("HeroVanBattleStartPanel", UIWFlushPanel)

function HeroVanBattleStartPanel:HeroVanBattleStartPanel()
    self.data = HeroVanBattleData.Instance

    local num = 6
    -- self.time.text = num
    -- self.data.start_view_data 
    self.delay_timer = TimeHelper:AddCountDownTT(function ()
        if self.time ~= nil then 
            -- LogError("?!timer?!",math.floor(self.data.start_view_data.start_fight_timestamp - TimeManager:GetServerTime()))
            -- local num = math.floor(self.data.start_view_data.start_fight_timestamp - TimeManager:GetServerTime()+1)
            num = num - 1
            if num > 5 then num = 5
            elseif num < 1 then 
                num = 1 
            end 
            self.time.text = num
        end 
    end,
    function () 
        self.time.text = 1
        ViewMgr:CloseView(HeroVanBattleStart)
        BattleCtrl.SendPrepareFinish()
        -- self.delay_end_timer = TimeHelper:AddDelayTimer(function()
            
        -- end, 3)

    end,5,1)--self.data.start_view_data.start_fight_timestamp,1)

    local attack_team = self.data.start_view_data.fight_info.attack_team
    UH.SetText(self.left_rank,attack_team.rank_posi == -1 and Language.HeroVanBattle.RankLess or attack_team.rank_posi+1)
    UH.SetText(self.left_name,attack_team.team_name..Language.HeroVanBattle.TeamNameEnd)
    for i = 1,5 do 
        self.left_team[i].gameObject:SetActive(i <= attack_team.member_count)
        if i <= attack_team.member_count then 
            self.left_team[i]:SetData(attack_team.member_list[i])
        end 
    end 

    local defend_team = self.data.start_view_data.fight_info.defend_team
    UH.SetText(self.right_rank,defend_team.rank_posi == -1 and Language.HeroVanBattle.RankLess or defend_team.rank_posi+1)
    UH.SetText(self.right_name,defend_team.team_name..Language.HeroVanBattle.TeamNameEnd)
    for i = 1,5 do 
        self.right_team[i].gameObject:SetActive(i <= defend_team.member_count)
        if i <= defend_team.member_count then 
            self.right_team[i]:SetData(defend_team.member_list[i])
        end 
    end 
end 

function HeroVanBattleStartPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.delay_timer)
    TimeHelper:CancelTimer(self.delay_end_timer)
end

HeroVanBattleStartMember = HeroVanBattleStartMember or DeclareMono("HeroVanBattleStartMember",UIWidgetBaseItem)
function HeroVanBattleStartMember:HeroVanBattleStartMember()
    self.hvb_data = HeroVanBattleData.Instance
end

function HeroVanBattleStartMember:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    
    UH.SetText(self.Name,self.data.role_name)
    UH.SetAvatarp(self.Head, self.data.avatar_type ,self.data.headshot_id,self.data.fly_flag)
    UH.SetText(self.infos,self.data.level..Language.Common.Level.."·"..Language.Common.Prof[RoleData.ProfIdToProfType(self.data.profession)])
end