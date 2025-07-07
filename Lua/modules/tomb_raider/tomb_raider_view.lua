TombRaiderView = TombRaiderView or DeclareView("TombRaiderView", "tomb_raider/tomb_raider_view", Mod.TombRaider.Main)
VIEW_DECLARE_LEVEL(TombRaiderView,ViewLevel.L0)
function TombRaiderView:TombRaiderView()
    self:SetTimerCloseView()
end

function TombRaiderView:SetTimerCloseView()
	self.timer_close_view = TimeHelper:AddRunTimer(function ()
        if not TeamData.Instance:InTeam() then 
            TimeHelper:CancelTimer(self.timer_close_view)
            return 
        end 
   		if Config.treasure_contend_auto.other[1].team_target ~= TeamData.Instance:GetRealGoalData().target_id then
            PublicPopupCtrl.Instance:Center(ErrorText[180])
			ViewMgr:CloseView(TombRaiderView)
		end
	end, 1)	
end

function TombRaiderView:CloseCallback()
	TimeHelper:CancelTimer(self.timer_close_view)
end

--===========================TombRaiderPanel===========================
TombRaiderPanel = TombRaiderPanel or DeclareMono("TombRaiderPanel", UIWFlushPanel)

function TombRaiderPanel:TombRaiderPanel()
    self.data = TombRaiderData.Instance

    self.data_cares = {
        {data = TeamSportData.Instance.match_data,func = self.FlushMatch},
        {data = self.data.achievement_info,func = self.FlushRewardInfo,init_call = false},
        {data = self.data.achievement_info,func = self.FlushSelfInfo,init_call = false},
        {data = TeamData.Instance:GetTeamInfo(),func = self.FlushPlayerInfo,init_call = false},
    }

    self:FlushPanel()

    TombRaiderCtrl.Instance:SendMatchOperate(2)
    TombRaiderCtrl.Instance:SendMatchOperate(4)

    local role_id = RoleData.Instance:GetRoleId()
    if UnityPlayerPrefs.GetInt(role_id.."tomb_raider_opened") == 0 then
        -- self:OnClickGuide()
        ViewMgr:OpenView(TombRaiderSynopsisView)
        UnityPlayerPrefs.SetInt(role_id.."tomb_raider_opened",1)
    end 
    self:InitRewardInfo()

    if YinYangData.Instance.view_data.opened then 
        YinYangCtrl.CSYinYangEyeOpReq(0)
    end 
end

function TombRaiderPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
    if self.match_timer ~=nil then 
        TimeHelper:CancelTimer(self.match_timer)
    end 
    TimeHelper:CancelTimer(self.auto_delay)    
    
    self.data.tomb_matching = false
end

function TombRaiderPanel:FlushPanel()
    self:FlushBattleInfo()
    self:FlushPlayerInfo()
    -- self:FlushRewardInfo()
    self:FlushSelfInfo()
end

function TombRaiderPanel:InitRewardInfo()
    for i = 1,3 do 
        local vo = {}
        vo.is_have = true
        vo.is_get = false
        vo.is_empty = true
        self.BoxSorts[i]:SetData(vo)
    end 
end

function TombRaiderPanel:FlushMatch()
    -- LogError("?Matching?",TeamSportData.Instance.match_data.is_match,TeamSportData.Instance.match_data.is_complete)
    self.Matching:SetActive(TeamSportData.Instance.match_data.is_match == 1 and TeamSportData.Instance.match_data.is_complete == 0)
    self.BtnMatch:SetActive(TeamSportData.Instance.match_data.is_match == 0 and TeamSportData.Instance.match_data.is_complete == 0)
    self.BtnCancelMatch:SetActive(TeamSportData.Instance.match_data.is_match == 1 and TeamSportData.Instance.match_data.is_complete == 0)
    self.MatchComplete:SetActive(TeamSportData.Instance.match_data.is_complete == 1 and TeamSportData.Instance.match_data.is_match == 0)

    if TeamSportData.Instance.match_data.is_match == 1 then
        self.matchtiming:CreateRunTime(0)
    else
        self.matchtiming:CloseCountDownTime()
    end 

    if TeamSportData.Instance.match_data.is_complete == 1 and TeamSportData.Instance.match_data.is_match == 0 then 
        self.enter_anim:SetTrigger(APH("is_show"))
        self.EffectTool:Play(5165100)
    end 

    self.is_match = TeamSportData.Instance.match_data.is_match == 1 and 0 or 1 
    self.data.tomb_matching = TeamSportData.Instance.match_data.is_match == 1
end

function TombRaiderPanel:FlushBattleInfo()

end 

function TombRaiderPanel:FlushSelfInfo()
    local rank_info = self.data:GetRankInfo()
    local my_rank = ( rank_info.my_pos ~= nil and rank_info.my_pos > 0) and rank_info.rank_list[rank_info.my_pos] or {index = 0}
    UH.SetText(self.PointShow, Language.TombRaider.ViewPointShow..(my_rank.rank_value or 0))
    UH.SetText(self.RecordShow, string.format(Language.TombRaider.ViewRewardShow,
        self.data.achievement_info.param[0] or 0,self.data.achievement_info.param[3] or 0,self.data.achievement_info.param[2] or 0))
end

function TombRaiderPanel:FlushPlayerInfo()
    local team_info =  TeamData.Instance:GetTeamInfo()
    local member_list = team_info.member_list 
    local member_num = 0
    -- LogError("!?@FlushPlayerInfo",team_info)
    if TeamData.Instance:InTeam() then 
        local team_info =  TeamData.Instance:GetTeamInfo()
        -- local index = 2
        for k,v in pairs(member_list) do 
            if self.PlayerSorts[v.index] == nil then break end 

            if v.is_self then 
                self.PlayerSorts[v.index]:SetData(v)
            elseif v.info and (v.info.is_partner == 1 or (v.info.is_follow == 0 and v.info.is_leader == 0)) then 
                if self.PlayerSorts[v.index] ~= nil then self.PlayerSorts[v.index]:SetData({}) end 
            else 
                if self.PlayerSorts[v.index] then 
                    self.PlayerSorts[v.index]:SetData(v)
                    -- index = index + 1
                end 
            end 
            member_num = member_num + (v.info ~= nil and 1 or 0)
        end
    else
        local vo = {
            is_single = true
        }
        self.PlayerSorts[1]:SetData(vo)
        for i = 2,3 do 
            self.PlayerSorts[i]:SetData({})
        end 
    end 

    for i = 4,5 do 
        if TeamData.Instance:InTeam() and member_list[i].info ~= nil and member_list[i].info.is_partner == 0 then 
            PublicPopupCtrl.Instance:Center(Language.TombRaider.MemberTooMore)
            break
        end 
    end 
end

function TombRaiderPanel:FlushRewardInfo()
    local data = self.data.achievement_info
    local config = self.data:GetBoxRewardInfo()
    for i = 1,3 do 
        local vo = {}
        vo.is_have = data.param[config[i].param_type] >= config[i].param
        vo.is_get = data.reward_status[i] == 1

        vo.cur_pro = data.param[i == 2 and config[1].param_type or config[i].param_type]
        vo.total = config[i].param
        vo.seq = i
        vo.is_empty = false
        self.BoxSorts[i]:SetData(vo)
    end 
end

function TombRaiderPanel:OnClickBack()    
    if TeamSportData.Instance.match_data.is_match == 1 then 
        local info = {
            content = Language.TombRaider.CancelMatchByClose,
            cancel = {
                name = nil,
                func = nil
            },
            confirm = {
                name = nil,
                func = function ()
                    -- 强制结束
                    TombRaiderCtrl.Instance:SendMatchOperate(self.is_match)
                    ViewMgr:CloseView(TombRaiderView)
                    ViewMgr:CloseView(DialogTipsView)

                    if TeamData.Instance:InTeam() and not TeamData.Instance:IsLeader() then 
                        TeamCtrl.Instance:SendExitTeam()
                    end 
                end
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)

    elseif TeamData.Instance:InTeam() and not TeamData.Instance:IsLeader() then 

        local info = {
            content = Language.Team.TipsIsLeaveTeam,
            cancel = {
                name = nil,
                func = nil
            },
            confirm = {
                name = nil,
                func = function ()
                    ViewMgr:CloseView(TombRaiderView)
                    ViewMgr:CloseView(DialogTipsView)

                    TeamCtrl.Instance:SendExitTeam()
                end
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
    else 
        ViewMgr:CloseView(TombRaiderView)
    end 
end

function TombRaiderPanel:OnClickSociety()
	ViewMgr:OpenView(SocietyView)
end

function TombRaiderPanel:OnClickRank() 
    ViewMgr:OpenView(TombRaiderRankView)
end 

function TombRaiderPanel:OnClickReward() 
    ViewMgr:OpenView(TombRaiderRewardView)
end 

function TombRaiderPanel:OnClickGuide() 
    ViewMgr:OpenView(TombRaiderSynopsisView)
end

function TombRaiderPanel:EffectComplete()
    ViewMgr:CloseView(TombRaiderView)
    if self.data:InTombRaider() then 
        ViewMgr:CloseAllViewInBattle()
        ViewMgr:OpenView(TombRaiderBattleView)
    end 
end 

function TombRaiderPanel:AllViewClose()
    ViewMgr:CloseView(TombRaiderRewardView)
    ViewMgr:CloseView(TombRaiderSynopsisView)
    ViewMgr:CloseView(TombRaiderRankView)
    ViewMgr:CloseView(SocietyView)
    ViewMgr:CloseView(TeamInviteView)
    ViewMgr:CloseView(SilkRoadPreview)
end

function TombRaiderPanel:OnClickMatch() 
    -- if TeamData.Instance:InTeam() and not TeamData.Instance:IsLeader() and TeamSportData.Instance.match_data.is_match == 0 then  
        -- 身位队员不能主动匹配
        -- PublicPopupCtrl.Instance:Center(Language.TombRaider.NeedLeader)
        -- return
    -- end

    TombRaiderCtrl.Instance:SendMatchOperate(self.is_match)
    if LoginData.Instance:BaseData().is_on_crosssever == false then 
        self.auto_delay = TimeHelper:AddDelayTimer(function()
           TombRaiderCtrl.Instance:SendMatchOperate(1)
        end,3)
    end 
    -- self.is_match = self.is_match == 1 and 0 or 1
    -- self.data.tomb_matching = self.is_match ~= 1

    if TeamData.Instance:InTeam() and TeamData.Instance:IsLeader() then 
        TombRaiderCtrl.Instance:SendLeaderEjectWindows({windows_type = Mod.TombRaider.Main})
    end 
end

function TombRaiderPanel:OnClickMember(data)
    if not TeamData.Instance:InTeam() or not TeamData.Instance:IsLeader() then 
        return 
    end 
    TombRaiderCtrl.Instance:OnRoleInfo(data)
end

function TombRaiderPanel:OnClickInvite() 
    if self.is_match == 0 then 
        PublicPopupCtrl.Instance:Center(Language.TombRaider.InvniteStop)
        return 
    end 

    if not TeamData.Instance:InTeam() then 
        TeamData.Instance:SetCurSelTargetByAct(ActType.TombRaider)
        TeamCtrl.Instance:SendCreateTeam()

        self.invite_delay = TimeHelper:AddDelayFrameTimer(function()
            ViewMgr:OpenView(TeamInviteView)
         end,5)
    else 
        ViewMgr:OpenView(TeamInviteView)
    end 
end

function TombRaiderPanel:OnClickBox(data) 
    if data.is_have == true and data.is_get == false then
        TombRaiderCtrl.Instance:SendMatchOperate(3,data.seq)
    else
        local cfg = self.data:GetBoxRewardInfo()[data.seq]
        local reward_list = {}
        for k,v in pairs(cfg.item_list) do 
            table.insert(reward_list,BagData.Instance:ItemBuild(v))
        end 
        ViewMgr:OpenView(SilkRoadPreview,{reward_list = reward_list,pos = Vector3.New(410,-160,0),
        desc = string.format(Language.TombRaider.progress,data.cur_pro,data.total)})
    end
end

TombRaiderMenber = TombRaiderMenber or DeclareMono("TombRaiderMenber", UIWidgetBaseItem)
function TombRaiderMenber:WorkShopBtnItem()
    -- body
end

function TombRaiderMenber:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.data.info == nil and not self.data.is_single then
        UH.SetText(self.player_name, Language.TombRaider.WithoutPlayer)
        self.isleader:SetActive(false)
        self.player_icon.gameObject:SetActive(false)
        return 
    end 

    self.player_icon.gameObject:SetActive(true)

    if not self.data.is_single then 
        -- LogError("?@noy single",self.data.is_leader)
        UH.SetAvatar(self.player_icon, self.data.info.appearance.avatar_type,self.data.info.appearance.avatar_id,0)--self.data.info.appearance.avatar_quality)
        UH.SetAvatar(self.player_sp_icon, self.data.info.appearance.avatar_type,self.data.info.appearance.avatar_id,self.data.info.appearance.avatar_quality)
        UH.SetText(self.player_name, self.data.info.name)
        self.isleader:SetActive(self.data.is_leader)
    else
        local data = RoleData.Instance:GetBaseData()
        UH.SetAvatar(self.player_icon, data.appearance.avatar_type,data.appearance.avatar_id,0)-- data.appearance.avatar_quality)
        UH.SetAvatar(self.player_sp_icon, data.appearance.avatar_type,data.appearance.avatar_id,data.appearance.avatar_quality)
        UH.SetText(self.player_name, data.name)
        self.isleader:SetActive(data.is_leader == 1)
    end 

    -- self.player_icon
    -- self.player_name
    -- self.isleader
end

TombRaiderBoxShow = TombRaiderBoxShow or DeclareMono("TombRaiderBoxShow", UIWidgetBaseItem)
function TombRaiderBoxShow:WorkShopBtnItem()
    -- body
end

function TombRaiderBoxShow:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.is_not_have:SetActive(not data.is_have)
    self.is_got:SetActive(data.is_get)
    -- self.is_first:SetActive(not data.is_get)
    -- self.box:SetActive(not data.is_get)

    self.red_point = self.red_point or UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
	self.red_point:SetNum((data.is_empty== false and data.is_have == true and data.is_get == false) and 1 or 0)    

    self.inter.Interactable = true
end

TombChatRecordList = TombChatRecordList or DeclareMono("TombChatRecordList", UIWFlushPanel)

function TombChatRecordList:TombChatRecordList()
	self.data = ChatCtrl.Instance:Data()

    self.data_cares = {
		{data = self.data:GetChatChannelList(), func = self.FlushRecordList},
	}

    self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,BindTool.Bind(self.RoleNoticeComplete, self))
end 

function TombChatRecordList:RoleNoticeComplete()
    self:FlushRecordList()
end

function TombChatRecordList:FlushRecordList()
    if self.RecordList == nil or not ViewMgr:IsOpen(self.view:GetType()) then return end 

	local chat_list = self.data:GetMainViewChatList()
    self.RecordList:SetDataList(chat_list)
    self.RecordList:JumpToDown()
end

function TombChatRecordList:OnClickArrow()
    local state = self.ChatAnim:GetBool("state")
    self.ChatAnim:SetBool("state", not state)
end