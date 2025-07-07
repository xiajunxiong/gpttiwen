HeroVanBattleApplyView = HeroVanBattleApplyView or DeclareView("HeroVanBattleApplyView", "hero_van_battle/hero_van_battle_apply_view", Mod.HeroVanBattle.ApplyView)
-- VIEW_DECLARE_LEVEL(HeroVanBattleTeamView,ViewLevel.L0)
function HeroVanBattleApplyView:HeroVanBattleApplyView()

end

function HeroVanBattleApplyView:OnClickClose()
    ViewMgr:CloseView(HeroVanBattleApplyView)

    if not HeroVanBattleData.Instance:TypeApplyIsCreateSuccess() then 
        HeroVanBattleCtrl.Instance:SendTeamArenaReq(2)
    end 
end

HeroVanBattleApplyPanel = HeroVanBattleApplyPanel or DeclareMono("HeroVanBattleApplyPanel", UIWFlushPanel)
function HeroVanBattleApplyPanel:HeroVanBattleApplyPanel()
    self.data = HeroVanBattleData.Instance
    self.data_cares = {
        {data = self.data.apply_view_data,func = self.FlushView},
        {data = self.data.team_pre_signup,func = self.FlushView,init_call = false},
        {data = self.data.team_pre_signup,func = self.FlushName,keys = {"team_name"},init_call = false},
        {data = self.data.other_info,func = self.FlushEnroll,init_call = false},
    }

    self.init = false
    UH.SetText(self.confirm_name, Language.HeroVanBattle.NoNameWait)
end 


function HeroVanBattleApplyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.team_name.text = ""

    self.data:SetPreTeamSignUpInfo({})
end

-- 队长在接受了4605信息前进行刷新，此时留存数据为旧
-- 队员在接受了4605信息后进行刷新，此时留存数据为新数据
function HeroVanBattleApplyPanel:FlushView()
    if not TeamData.Instance:InTeam() then return end 

    local member_list = self.data:TypeApplyGetMemberList(self.init)

    self.member_list:SetData(member_list)

    self.Input:SetActive( not self.init and self.data.apply_view_data.is_leader)
    self.confirm_obj:SetActive(self.init or not self.data.apply_view_data.is_leader)

    if self.init or not self.data.apply_view_data.is_leader then 
        UH.SetText(self.confirm_name, self.data.team_pre_signup.team_name)
    end 

    self.btn_inter.Interactable = self.data:TypeApplyMineIsDetermine()
    if not self.init then self.init = true end 

end

function HeroVanBattleApplyPanel:FlushName()
    PublicPopupCtrl.Instance:Center(Language.HeroVanBattle.ApplyLeaderComplete)
end

function HeroVanBattleApplyPanel:OnClickApply()
    local count = DataHelper.GetStringWordNum(self.team_name.text)
    if count > ROLE_NAME_COUNT then
        PublicPopupCtrl.Instance:Center(Language.HeroVanBattle.NameLonger)
        return 
    elseif count == 0 and TeamData.Instance:IsLeader() then 
        PublicPopupCtrl.Instance:Center(Language.HeroVanBattle.NameLack)
        return 
    elseif TeamData.Instance:IsLeader() and ChatFilter.Instance:IsIllegal(self.team_name.text, true) then
        PublicPopupCtrl.Instance:Center(ErrorText[66])
        return
    elseif string.find(self.team_name.text, " ") then 
        PublicPopupCtrl.Instance:Center(ErrorText[66])
        return
    end

    if not TeamData.Instance:IsLeader() and HeroVanBattleData.Instance:TypeApplyIsMasterWaiting() then 
        PublicPopupCtrl.Instance:Center(Language.HeroVanBattle.ApplyWaitLeader)
        return 
    end 

    if not self.data:TypeApplyMineIsDetermine() then 
        PublicPopupCtrl.Instance:Center(Language.HeroVanBattle.ApplyComplete)
        return 
    end 

    if TeamData.Instance:IsLeader() then 
        HeroVanBattleCtrl.Instance:SendTeamArenaSignUpReq(0,self.team_name.text)
    else 
        HeroVanBattleCtrl.Instance:SendTeamArenaReq(1)
    end 
end

function HeroVanBattleApplyPanel:FlushEnroll()
    if self.data.other_info.is_sign_up == 1 then
        ViewMgr:CloseView(HeroVanBattleApplyView)
    end 
end

VanTeamApplyMember = VanTeamApplyMember or DeclareMono("VanTeamApplyMember",UIWidgetBaseItem)
function VanTeamApplyMember:VanTeamApplyMember()
    self.hvb_data = HeroVanBattleData.Instance
end

function VanTeamApplyMember:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local flag_master = HeroVanBattleData.Instance:TypeApplyIsMasterWaiting()
    UH.SetAvatar(self.avatar, data.avatar_type,data.avatar_id,data.fly_flag )
    UH.SetText(self.name,data.name)
    self.waiting:SetActive(flag_master)
    self.member_waiting:SetActive(not flag_master and not data.determine)
    self.complete:SetActive(data.determine)

    self.is_leader:SetActive(data.is_leader)
end 