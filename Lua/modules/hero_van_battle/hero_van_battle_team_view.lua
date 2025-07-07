HeroVanBattleTeamView = HeroVanBattleTeamView or DeclareView("HeroVanBattleTeamView", "hero_van_battle/hero_van_battle_team_view", Mod.HeroVanBattle.TeamView)
VIEW_DECLARE_LEVEL(HeroVanBattleTeamView,ViewLevel.L0)
function HeroVanBattleTeamView:HeroVanBattleTeamView() 
    if LoginData.Instance:BaseData().is_on_crosssever == false then 
        LoginCtrl.Instance:SendCrossStartReq()

        self.auto_delay = TimeHelper:AddDelayTimer(function()
            HeroVanBattleCtrl.Instance:SendTeamArenaReq(0)
         end,3)
    else 
        HeroVanBattleCtrl.Instance:SendTeamArenaReq(0)
    end 
end

function HeroVanBattleTeamView:CloseCallback()
    TimeHelper:CancelTimer(self.auto_delay)    
end

function HeroVanBattleTeamView:OnClickClose()
    ViewMgr:CloseView(HeroVanBattleTeamView)
end 

HeroVanBattleTeamPanel = HeroVanBattleTeamPanel or DeclareMono("HeroVanBattleTeamPanel", UIWFlushPanel)

function HeroVanBattleTeamPanel:HeroVanBattleTeamPanel()
    self.data = HeroVanBattleData.Instance

end 

function HeroVanBattleTeamPanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[200].desc})
end

HeroVanBattleTeamLeft = HeroVanBattleTeamLeft or DeclareMono("HeroVanBattleTeamLeft", UIWFlushPanel)

function HeroVanBattleTeamLeft:HeroVanBattleTeamLeft()
    self.data = HeroVanBattleData.Instance
    self.data_cares = {
        -- {data = self.data.team_view_data,func = self.FlushView},
        {data = self.data.team_simple_info,func = self.FlushView,init_call = false},
        {data = self.data.search_team_data,func = self.FlushView,init_call = false},
    }

    -- self:FlushView()
end 

function HeroVanBattleTeamLeft:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.auto_open_handle ~= nil then
        TimeHelper:CancelTimer(self.auto_open_handle)
    end

    self.data:SetSearchKeyWord()
end

-- 有延迟，还是要根据下发个更新
function HeroVanBattleTeamLeft:FlushView()
    self:FlushMemberList()
end

function HeroVanBattleTeamLeft:FlushMemberList()
    local list = self.data:TeamViewGetTeamList()

    self.TeamList:SetData(list)

    -- 精确选中第二项第一个
    -- if self.data.team_view_data.can_enroll then 
    --     HeroVanBattleCtrl.Instance:SendTeamArenaReq(5)
    -- else
    if not self.data.team_view_data.can_enroll then 
        self.TeamList:ClickChild(list[2].children[1])
        self.auto_open_handle = TimeHelper:AddDelayTimer(function () 
            self:OnClickAllTeam()
            -- if list[1].children[1].is_empty and list[2].children[1].is_empty then 
            --     self:OnClickFriends()
            -- else 
            --     self:OnClickDirectTeam(list[1].children[1].is_empty and 2 or 1)
            -- end 
        end,0.3)
    end 

    if self.data.search_team_data.key_word ~= nil then 
        self:OnClickAllTeam()
    end
    
end

function HeroVanBattleTeamLeft:OnClickItem(data)
    if data.is_empty ~= true then 
        HeroVanBattleCtrl.Instance:SendTeamArenaReq(3,data.team_unique_id)
    else 

    end 
end

function HeroVanBattleTeamLeft:OnClickAllTeam()
    for k,v in pairs(self.TeamList.cache_list[AccordionItemType.parent]) do 
        if not v.Selector.isOn and v.ItemClick and v.gameObject.activeSelf then 
            v.ItemClick:Trigger()
        end 
    end 
end

--
function HeroVanBattleTeamLeft:OnClickDirectTeam(num)
    for k,v in pairs(self.TeamList.cache_list[AccordionItemType.parent]) do 
        if k == Language.HeroVanBattle.TeamName[num] and not v.Selector.isOn and v.ItemClick and v.gameObject.activeSelf then 
            v.ItemClick:Trigger()
        elseif k ~= Language.HeroVanBattle.TeamName[num] and v.Selector.isOn and v.ItemClick and v.gameObject.activeSelf then  
            v.ItemClick:Trigger()
        end  
    end 
end

function HeroVanBattleTeamLeft:OnClickFriends()
    for k,v in pairs(self.TeamList.cache_list[AccordionItemType.parent]) do 
        if k == Language.HeroVanBattle.TeamName[3] and not v.Selector.isOn and v.ItemClick and v.gameObject.activeSelf then 
            v.ItemClick:Trigger()
        elseif k ~= Language.HeroVanBattle.TeamName[3] and v.Selector.isOn and v.ItemClick and v.gameObject.activeSelf then  
            v.ItemClick:Trigger()    
        end 
    end 
end

function HeroVanBattleTeamLeft:OnClickSearch()
    -- if self.Search.text == "" then return end
    
    self.data:SetSearchKeyWord(self.Search.text)
end

VanTeamParentItem = VanTeamParentItem or DeclareMono("VanTeamParentItem",UIWidgetBaseItem)
function VanTeamParentItem:BlazeStarParentItem()
    self.hvb_data = HeroVanBattleData.Instance
end

function VanTeamParentItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.LblTitle, data.name)
end 

function VanTeamParentItem:OnSelect(selected)
    if self.Arrow then
        self.Arrow:SetActive(not selected)
    end

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

VanTeamItem = VanTeamItem or DeclareMono("VanTeamItem",UIWidgetBaseItem)
function VanTeamItem:VanTeamItem()
    self.hvb_data = HeroVanBattleData.Instance

end

function VanTeamItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.total:SetActive(not data.is_empty)
    self.empty:SetActive(data.is_empty == true)

    self.emptyDesc:SetActive(data.is_us ~= true)
    self.usEmptyDesc:SetActive(data.is_us == true)

    if data.is_empty ~= true then 
        self:FlushInfo()
    end 
end 

function VanTeamItem:FlushInfo()
    UH.SetText(self.LblName,self.data.team_name)    
    UH.SetAvatarp(self.avatar,self.data.avatar_type)--,self.data.headshot_id,self.data.fly_flag)
    for i = 1,5 do 
        self.members[i].gameObject:SetActive(self.data.member_list[i].role_id > 0)
        if self.data.member_list[i].role_id > 0 then 
            UH.SpriteName(self.members[i], ProfSettingB[RoleData.ProfIdToProfType(self.data.member_list[i].profession)])        
        end 
    end 
end

function VanTeamItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end 


HeroVanBattleTeamRight = HeroVanBattleTeamRight or DeclareMono("HeroVanBattleTeamRight", UIWFlushPanel)

function HeroVanBattleTeamRight:HeroVanBattleTeamRight()
    self.data = HeroVanBattleData.Instance
    if self.data.team_view_data.can_enroll then 
        HeroVanBattleCtrl.Instance:SendTeamArenaReq(5)
    end 
    self.data_cares = {
        {data = self.data.team_sign_up_detail_info,func = self.FlushView,init_call = false},
        {data = self.data.team_view_mine_team,func = self.FlushMineTeam,init_call = false},
    }

    self.member_list:SetCompleteCallBack(function()
        for k,v in pairs(self.member_list.item_list) do
            v:SetMaster(k == 1)
        end 
    end)

    self.init_flush = false
end

function HeroVanBattleTeamRight:FlushView()
    self.empty:SetActive(self.data.team_sign_up_detail_info.team_unique_id == nil)
    if self.data.team_sign_up_detail_info.team_unique_id == nil then 
        return 
    end 

    -- if not self.init_flush and self.can_enroll then 
    --     return 
    -- end 

    if not self.init_flush and not self.can_enroll then 
        self.init_flush = true
    end 

    if not self.init_flush then 
        return 
    end 

    self.member_list:SetData(self.data:GetSignUpDetailInfo())
end

function HeroVanBattleTeamRight:FlushMineTeam()

    if not self.init_flush and self.can_enroll then 
        self.init_flush = true
    end 

    self.empty:SetActive(false)    
    local list = self.data:TeamViewGetMineTeamlist()
    self.member_list:SetData(list)
end

function HeroVanBattleTeamRight:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end 

VanTeamMember = VanTeamMember or DeclareMono("VanTeamMember",UIWidgetBaseItem)
function VanTeamMember:VanTeamMember()
    self.hvb_data = HeroVanBattleData.Instance
end

function VanTeamMember:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.name,data.role_name)
    UH.SetText(self.level,data.level)
    UH.SpriteName(self.prof,ProfSettingB[RoleData.ProfIdToProfType(data.profession)],data.fly_flag)
    UH.SetAvatarp(self.avatar,data.avatar_type,data.headshot_id,data.fly_flag)
    if data.rank_list == nil then return end
    
    UH.SetText(self.rank_prof,data.rank_list[1] > -1 and string.format(Language.HeroVanBattle.RankText,data.rank_list[1]+1) or Language.HeroVanBattle.RankLess)
    UH.SetText(self.rank_level,data.rank_list[2] > -1 and string.format(Language.HeroVanBattle.RankText,data.rank_list[2]+1) or Language.HeroVanBattle.RankLess)
    UH.SetText(self.rank_score,data.rank_list[3] > -1 and string.format(Language.HeroVanBattle.RankText,data.rank_list[3]+1) or Language.HeroVanBattle.RankLess)

    UH.SetText(self.rank_name,Language.Common.Prof[RoleData.ProfIdToProfType(self.data.profession)]..Language.HeroVanBattle.PorfRank)
    
end

function VanTeamMember:SetMaster(flag)
    self.is_master:SetActive(flag)
end

HeroVanBattleTeamBottom = HeroVanBattleTeamBottom or DeclareMono("HeroVanBattleTeamBottom", UIWFlushPanel)

function HeroVanBattleTeamBottom:HeroVanBattleTeamBottom()
    self.data = HeroVanBattleData.Instance
    self.data_cares = {
        {data = self.data.team_view_data,func = self.FlushView},
        {data = self.data.team_simple_info,func = self.FlushView},
    }
end 

function HeroVanBattleTeamBottom:FlushView()
    local flag_signup = self.data:TypeApplyIsCreateSuccess()
    local flag_enter = self.data.team_view_data.can_enroll
    self.enroll:SetActive(flag_enter and not flag_signup)
    self.teamshow:SetActive(not flag_enter and not flag_signup)
    self.enrollDone:SetActive(flag_signup)

    self.enroll_time1:StampTime(self.data.act_info.end_sign_up_timestamp,TimeType.Type_Special_9,Language.HeroVanBattle.enrollTime)
    self.enroll_time2:StampTime(self.data.act_info.end_sign_up_timestamp,TimeType.Type_Special_9,Language.HeroVanBattle.enrollTime)
    self.enroll_time3:StampTime(self.data.act_info.end_sign_up_timestamp,TimeType.Type_Special_9,Language.HeroVanBattle.enrollTime)

    local my_team = self.data:GetMyTeamInfo()
    if my_team ~= nil then 
        UH.SetText(self.enroll_text3,string.format(Language.HeroVanBattle.TeamViewDone,my_team.team_name))
    end 
    --self.enroll_text3
end

function HeroVanBattleTeamBottom:OnClickGoToEnroll()
    -- LogError("@?OnClickGoToEnroll")
    SceneLogic.Instance:AutoToNpcs(Config.hero_competition_auto.other[1].npc_id)
    ViewMgr:CloseView(HeroVanBattleTeamView)
end

function HeroVanBattleTeamBottom:OnClickGoEnroll()
    if not self.data:MemberTeamCheck() then 
        if LoginData.Instance:IsOnCrossSever() then 
            PublicPopupCtrl.Instance:Center(Language.HeroVanBattle.TeamMemberError)
        end 
        return 
    end 

    -- LogError("@?OnClickGoEnroll")
    if TeamData.Instance:IsLeader() then 
        self.data:TypeApplyViewLeader()

        TombRaiderCtrl.Instance:SendLeaderEjectWindows({windows_type = Mod.HeroVanBattle.ApplyView})
    end 
    
end