HeroVanBattleView = HeroVanBattleView or DeclareView("HeroVanBattleView", "hero_van_battle/hero_van_battle_view", Mod.HeroVanBattle.Main)
VIEW_DECLARE_LEVEL(HeroVanBattleView,ViewLevel.L0)
function HeroVanBattleView:HeroVanBattleView()
    if LoginData.Instance:BaseData().is_on_crosssever == false then 
        LoginCtrl.Instance:SendCrossStartReq()

        self.auto_delay = TimeHelper:AddDelayTimer(function()
            HeroVanBattleCtrl.Instance:SendTeamArenaReq(0)
         end,3)
    else 
        HeroVanBattleCtrl.Instance:SendTeamArenaReq(0)        
    end 
end

function HeroVanBattleView:CloseCallback()
    TimeHelper:CancelTimer(self.auto_delay)    
end


function HeroVanBattleView:OnClickClose()
    ViewMgr:CloseView(HeroVanBattleView)
end 

--===========================HeroVanBattlePanel===========================
HeroVanBattlePanel = HeroVanBattlePanel or DeclareMono("HeroVanBattlePanel", UIWFlushPanel)

function HeroVanBattlePanel:HeroVanBattlePanel()
    self.data = HeroVanBattleData.Instance

    self.act_timer:StampTime(self.data.act_info.end_sign_up_timestamp,TimeType.Type_Special_9,Language.HeroVanBattle.enrollTime)

    -- 取npc上有没有报名按钮
    self.enrolling:SetActive( self.data:TeamViewNpcCouldShow() and not self.data:EnterSceneNpcCouldShow())

    if self.data.is_pre_click then 
        self:OnClickMatchRules()
        self.data.is_pre_click = false
    end 

    self.enroll_inter.Interactable = self:IsStartSignUp()
    self.team_inter.Interactable = self:IsStartSignUp()
end 

function HeroVanBattlePanel:IsStartSignUp()
    local server_time = TimeManager:GetServerTime()

    return self.data.act_info.start_sign_up_timestamp < server_time
end

function HeroVanBattlePanel:OnClickMatchRules()
    if self.extral_panel.activeSelf == false then
        UH.SpriteName(self.sp_match, "AnNiu-XuanZhong")
        self.extral_panel:SetActive(not self.extral_panel.activeSelf)
    elseif self.extral_panel.activeSelf == true and self.match_part.activeSelf == true then
        UH.SpriteName(self.sp_match, "AnNiu")
        self.extral_panel:SetActive(not self.extral_panel.activeSelf)
        return 
    end 

    self.match_part:SetActive(true)
    self.reward_part:SetActive(false)


    UH.SpriteName(self.sp_reward, "AnNiu")
    UH.SpriteName(self.sp_match, "AnNiu-XuanZhong")    

    UH.SetText(self.enroll_text,ActivityData.IsOpen(ActType.HeroVanBattle) 
        and Language.HeroVanBattle.MainEnrollTextEnter or Language.HeroVanBattle.MainEnrollText)
end 

function HeroVanBattlePanel:OnClickRewardRules()
    if self.extral_panel.activeSelf == false then
        UH.SpriteName(self.sp_reward, "AnNiu-XuanZhong")    
        self.extral_panel:SetActive(not self.extral_panel.activeSelf)
    elseif self.extral_panel.activeSelf == true and self.reward_part.activeSelf == true then
        UH.SpriteName(self.sp_reward, "AnNiu")   
        self.extral_panel:SetActive(not self.extral_panel.activeSelf)
        return 
    end 

    self.match_part:SetActive(false)
    self.reward_part:SetActive(true)

    UH.SpriteName(self.sp_match, "AnNiu")
    UH.SpriteName(self.sp_reward, "AnNiu-XuanZhong")    
end 

function HeroVanBattlePanel:OnClickTeamShow()
    if not self:IsStartSignUp() then 
        PublicPopupCtrl.Instance:Center(Language.HeroVanBattle.BeforeSignUpTime)
        return 
    end 

    -- 测试流程
    self.data:TypeTeamViewShowTeam()

    ViewMgr:CloseView(HeroVanBattleView)
end 

function HeroVanBattlePanel:OnClickEnroll()
    if not self:IsStartSignUp() then 
        PublicPopupCtrl.Instance:Center(Language.HeroVanBattle.BeforeSignUpTime)
        return 
    end 
    
    -- 测试流程
    -- self.data:TypeTeamViewEnroll()
    SceneLogic.Instance:AutoToNpcs(Config.hero_competition_auto.other[1].npc_id)

    ViewMgr:CloseView(HeroVanBattleView)
end 

function HeroVanBattlePanel:OnClickCloseExtral()
    self.extral_panel:SetActive(false)
    UH.SpriteName(self.sp_match, "AnNiu")
    UH.SpriteName(self.sp_reward, "AnNiu")    
end

--===========================HeroVanBattleViewMatch========================
HeroVanBattlePanelMatch = HeroVanBattlePanelMatch or DeclareMono("HeroVanBattlePanelMatch", UIWFlushPanel)
function HeroVanBattlePanelMatch:HeroVanBattlePanelMatch()
    self.data = HeroVanBattleData.Instance

    UH.SetText(self.Enlist, Config.language_cfg_auto.textdesc[208].desc)

    local enlist_str = Config.language_cfg_auto.textdesc[270].desc
    local str_list = Split(enlist_str,"|")
    local show_list = {}
    for k,v in pairs(str_list) do 
        table.insert(show_list,v)
    end 

    self.EnlisList:SetData(show_list)

    local match_str = Config.language_cfg_auto.textdesc[201].desc
    local matchstr_list = Split(match_str,"|")
    local match_list = {}
    for k,v in pairs(matchstr_list) do 
        table.insert(match_list,v)
    end 

    self.MacthList:SetData(match_list)
    -- self.reward_list:SetData(BagData.Instance:ItemListBuild(self.data:GetCurJoinReward()))
end 

HeroVanBattleMatchTipsCell = HeroVanBattleMatchTipsCell or DeclareMono("HeroVanBattleMatchTipsCell", UIWidgetBaseItem)
function HeroVanBattleMatchTipsCell:SetData(data)
    UH.SetText(self.text, data)
    UIWidgetBaseItem.SetData(self, data)
end 

--===========================HeroVanBattleViewReward========================
HeroVanBattlePanelReward = HeroVanBattlePanelReward or DeclareMono("HeroVanBattlePanelReward", UIWFlushPanel)

function HeroVanBattlePanelReward:HeroVanBattlePanelReward()
    self.data = HeroVanBattleData.Instance

    UH.SetText(self.rank_rule,Config.language_cfg_auto.textdesc[209].desc)
    self.join_reward_list:SetData(BagData.Instance:ItemListBuild(self.data:GetCurJoinReward()))
    self.reward_list:SetData(self.data:GetCurRankReward())

    local reward_tips = Split(Config.language_cfg_auto.textdesc[202].desc,"|")
    for i = 1,3 do 
        UH.SetText(self["tips"..i],reward_tips[i])
    end 
end 


HeroVanBattleRewardCell = HeroVanBattleRewardCell or DeclareMono("HeroVanBattleRewardCell", UIWidgetBaseItem)
function HeroVanBattleRewardCell:SetData(data)
    UH.SetText(self.text, data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.rank_num, string.format( Language.CompetitionRank.RankChat,data.rank_min))
    self.item_list:SetData(BagData.Instance:ItemListBuild(data.reward_show))
end 
