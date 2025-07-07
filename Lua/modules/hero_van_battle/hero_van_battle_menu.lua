HeroVanBattleMenu = HeroVanBattleMenu or DeclareMono("HeroVanBattleMenu", UIWFlushPanel)

function HeroVanBattleMenu:HeroVanBattleMenu()
    self.data = SceneData.Instance
    self.hero_data = HeroVanBattleData.Instance
    self.data_cares = {
        {data = ActivityData.Instance.enter_data, func = self.FlushShow,keys = {"flush"}},
    }

    PublicPopupCtrl.Instance:OpenWarningView(nil,{
        desc = Config.language_cfg_auto.textdesc[272].desc,
        warning = Config.language_cfg_auto.textdesc[273].desc
    })
end 

function HeroVanBattleMenu:FlushShow()
    self.op_gaming:SetActive(not self.hero_data:EnterSceneNpcCouldShow())
    self.op_ready:SetActive(self.hero_data:EnterSceneNpcCouldShow())

end

HeroVanBattleGaming = HeroVanBattleGaming or DeclareMono("HeroVanBattleGaming", UIWFlushPanel)
function HeroVanBattleGaming:HeroVanBattleGaming()
    self.hero_data = HeroVanBattleData.Instance
    self.data_cares = {
        {data = self.hero_data.battle_scene_info, func = self.FlushShow,keys = {"flush"}},
    }
end 

function HeroVanBattleGaming:FlushShow()
    if  self.hero_data.battle_scene_info.my_info ~= nil then 
        UH.SetText(self.point, self.hero_data.battle_scene_info.my_info.total_score)
        local rank = self.hero_data.battle_scene_info.rank_posi
        UH.SetText(self.rank, rank == -1 and Language.HeroVanBattle.RankLess or rank)
    end 

    UH.SetText(self.team_num, self.hero_data.battle_scene_info.resi_team_num)

    self.time_limit:CreateActTime(ActType.HeroVanBattle,TimeType.Type_Special_9)
end

function HeroVanBattleGaming:OnClickNeoRank()
    HeroVanBattleCtrl.Instance:SendTeamTeamArenaSceneReq(0)

    ViewMgr:OpenView(HeroVanBattleNeoRankView)
end

HeroVanBattleReady = HeroVanBattleReady or DeclareMono("HeroVanBattleReady", UIWFlushPanel)
function HeroVanBattleReady:HeroVanBattleReady()
    self.hero_data = HeroVanBattleData.Instance
    self.data_cares = {
        {data = self.hero_data.battle_scene_info, func = self.FlushShow,keys = {"flush"}},
    }

    self:FlushShow()
end 

function HeroVanBattleReady:FlushShow()
    local enter_info = ActivityData.Instance:GetActivityEnterInfo(ActType.HeroVanBattle)

    self.time_start:StampTime(enter_info.enter_end_timestamp)
    UH.SetText(self.team_num, self.hero_data.battle_scene_info.resi_team_num)
end 

function HeroVanBattleReady:OnClickRule()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[274].desc})
end


MainViewHeroVanBattleView = MainViewHeroVanBattleView or DeclareMono("MainViewHeroVanBattleView", UIWFlushPanel)
function MainViewHeroVanBattleView:MainViewHeroVanBattleView()
    self.hero_data = HeroVanBattleData.Instance
    self.data_cares = {
        {data = self.hero_data.act_info, func = self.FlushIcon},
    }
end

function MainViewHeroVanBattleView:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self:FlushIcon()
end

function MainViewHeroVanBattleView:FlushIcon()
	UH.SetText(self.Name,Language.HeroVanBattle.Title)
	local time = HeroVanBattleData.Instance:EndRankMainViewShowTime() or 0
    local flag,stamp_time = HeroVanBattleData.Instance:IsSignTiming()

    self.Timer:CloseCountDownTime()
	self.Timer:StampTime(flag and stamp_time or time,TimeType.Type_Special_9)
    if not flag then 
        self.Timer:SetShowCallBack(function (t)	
            return Language.HeroVanBattle.EndRank
        end)
    else 
        self.Timer:SetShowCallBack(function (t)	
            local t_str = nil
            if tonumber(t.day) > 0 then
                t_str = string.format(Language.UiTimeMeter.Special3, t.day, t.hour)
            elseif tonumber(t.hour) > 0 then
                t_str = string.format(Language.UiTimeMeter.Special4, t.hour, t.min)
            else
                t_str = string.format(Language.UiTimeMeter.Specia2, t.min, t.s)
            end
            return t_str
        end)
    end 
end

function MainViewHeroVanBattleView:OnClickIcon()
	ViewMgr:OpenView(HeroVanBattleGloryView)
end

HeroVanBattleGloryActivePanel = HeroVanBattleGloryActivePanel or DeclareMono("HeroVanBattleGloryActivePanel", UIWFlushPanel)
function HeroVanBattleGloryActivePanel:HeroVanBattleGloryActivePanel()
    self.hero_data = HeroVanBattleData.Instance
    self.data_cares = {
        {data = self.hero_data.act_info, func = self.FlushIcon},
        {data = self.hero_data.finally_rank_data, func = self.FlushIcon},
    }
    UH.SetText(self.LabelGlory,Language.HeroVanBattle.EndRank)
    self.GloryInter.Interactable = false
end

function HeroVanBattleGloryActivePanel:FlushIcon()
    local flag,stamp_time = self.hero_data:IsSignTiming()
    self.GloryInter.Interactable = not flag and self.hero_data.finally_rank_data.rank_list ~= nil
    if not flag then 
        UH.SetText(self.LabelGlory,Language.HeroVanBattle.EndRank)
    end 
end 

function HeroVanBattleGloryActivePanel:OnClickGlory()
    if self.hero_data.finally_rank_data.rank_list == nil then 
        PublicPopupCtrl.Instance:Center(Language.HeroVanBattle.NoAnyGloryRankInfo)
        return 
    end 

    ViewMgr:OpenView(HeroVanBattleGloryView)
end