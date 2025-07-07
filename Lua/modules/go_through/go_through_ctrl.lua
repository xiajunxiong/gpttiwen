FILE_REQ("modules/go_through/go_through_data")
VIEW_REQ("modules/go_through/go_through_view")
VIEW_REQ("modules/go_through/go_through_menu_view")

GoThroughCtrl = GoThroughCtrl or BaseClass(BaseCtrl)

function GoThroughCtrl:__init()
	if GoThroughCtrl.Instance ~= nil then
		Debuger.LogError("[GoThroughCtrl] attempt to create singleton twice!")
		return
	end
	GoThroughCtrl.Instance = self
	self.data = GoThroughData.New()
    self:RegisterAllProtocols()
end

function GoThroughCtrl:__delete()
    self:UnRegisterAllProtocols()
    GoThroughCtrl.Instance = nil
end

function GoThroughCtrl:OnUnloadGameLogic()
    if self.fight_target ~= nil then
        SceneCtrl.Instance:DeleteNpc(self.fight_target)
        self.fight_target = nil
    end
    self:RemoveBehaviourBtn()
end

function GoThroughCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSGoThroughReq)
    self:RegisterProtocol(SCGoThroughInfo, "OnGoThroughInfo")
    self:RegisterProtocol(SCGoThroughSceneInfo, "OnGoThroughSceneInfo")
end

function GoThroughCtrl:OnInit()
    EventSys:Bind(GlobalEventKeys.RoleNoticeChange, BindTool.Bind(self.SendGoThroughReq, self))
    EventSys:Bind(GlobalEventKeys.RoleLevelChange, BindTool.Bind(self.CheckBehaviourBtn, self))
    ChallengeNaviCtrl.Instance:RegisterOpenReqFunc(Mod.Challenge.GoThrough,function() 
		ViewMgr:OpenViewByKey(Mod.Challenge.GoThrough)
	end)
    ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Challenge.GoThrough,function()
        return self.data:GetPassLevel()
    end)
    FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_GO_THROUGH,function(is_win)
        if is_win == 1 then
            local reward_list = self.data:GetRewardData()
            if #reward_list > 0 then
                ViewMgr:OpenView(GiftOpenView,{
                    item_list = DataHelper.FormatItemList(reward_list),gift_type = GIFT_OPEN_TYPE.CONGRATULATIONS,
                    close_func = function()
                        if TeamCtrl.Instance:Data():IsLeader() then
                            SceneCtrl.Instance:RequestLeaveFb()
                        end
                    end,
                })
            end
        end
        self:SendGoThroughReq()
        self.data:SetRewardData()
	end) 
end

function GoThroughCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(CSGoThroughReq)
    self:UnRegisterProtocol(SCGoThroughInfo)
    self:UnRegisterProtocol(SCGoThroughSceneInfo)
end

function GoThroughCtrl:SendGoThroughReq(req_type, param_1)
    local protocol = GetProtocol(CSGoThroughReq)
    protocol.req_type = req_type or 0
    protocol.param_1 = param_1 or 0
    SendProtocol(protocol)
end

function GoThroughCtrl:OnGoThroughInfo(protocol)
	self.data:SetGoThroughInfo(protocol)
    self:OnCreateNpc()
    self:CheckBehaviourBtn()
end

function GoThroughCtrl:OnGoThroughSceneInfo(protocol)
	self.data:SetGoThroughSceneInfo(protocol)
end

function GoThroughCtrl:SetupBehaviourBtn(btn_list,obj_vo)
	if obj_vo and obj_vo.id == self.data:GetNpcId() then
        table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(Language.GoThrough.NpcBtnName[1],
		function()
            if TeamCtrl.Instance:Data():InTeam() then
                if not TeamCtrl.Instance:Data():IsLeader() then
                    PublicPopupCtrl.Instance:Center(ErrorText[57])
                    return
                end
                if TeamData.Instance:MemberNumRole() < self.data:GetTargetNum() then
                    PublicPopupCtrl.Instance:AlertTip(Language.GoThrough.EnterTeamTip,function()
                        self:OnTeamMatch()
                    end)
                    return
                end
                local pass_level = self.data:GetPassLevel()
                if TeamData.Instance:GetRealGoalData().target_id ~= self.data:GetTeamTarget(pass_level) then
                    local target = TeamData.Instance:GetTeamGoalWithId(self.data:GetTeamTarget(pass_level))
                    TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
					TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
                    PublicPopupCtrl.Instance:Center(ErrorText[180])
                    ViewMgr:OpenView(TeamView)
                    return
                end
            end
            if not TeamCtrl.Instance:Data():InTeam() then
                PublicPopupCtrl.Instance:AlertTip(Language.GoThrough.EnterTeamTip,function()
                    self:OnTeamMatch()
                end)
                return
            end
            SceneCtrl.Instance:RequestEnterFb(FB_TYPE.GO_THROUGH,self.data:GetPassLevel())
		end))
        table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(Language.GoThrough.NpcBtnName[2],
		function()
            if TeamCtrl.Instance:Data():InTeam() then
                if not TeamCtrl.Instance:Data():IsLeader() then
                    PublicPopupCtrl.Instance:Center(ErrorText[57])
                    return
                end
            end
            self:OnTeamMatch()
		end))
    end
end

function GoThroughCtrl:OnTeamMatch()
    local pass_level = self.data:GetPassLevel()
    local target_id = self.data:GetTeamTarget(pass_level)
    local target = TeamData.Instance:GetTeamGoalWithId(target_id)
    TeamCtrl.Instance:TeamDetectByTargetId(target.target_id,function()
        if TeamCtrl.Instance:Data():InTeam() then
            if not TeamCtrl.Instance:Data():IsLeader() then
                PublicPopupCtrl.Instance:Center(ErrorText[57])
                return
            end
        end
        SceneCtrl.Instance:RequestEnterFb(FB_TYPE.GO_THROUGH,self.data:GetPassLevel())
    end, self.data:GetTargetNum(), true)
end

function GoThroughCtrl:SetupBehaviourList(talk_list,obj_vo)
    if obj_vo and obj_vo.id == self.data:GetNpcId() then
        local data = self.data:GetPassLevelData()
        if data ~= nil then
            table.insert(talk_list,{
                talker = self.data:GetNpcId(), 
                talk_text = Format(Language.GoThrough.NpcDialogTip,data.reorder,data.name),
            })
        end
    end
end

function GoThroughCtrl:OnCreateNpc()
    if self.fight_target ~= nil then
        SceneCtrl.Instance:DeleteNpc(self.fight_target)
        self.fight_target = nil
    end
    local data = self.data:GetPassLevelData()
    if data ~= nil then
        if data.fight_target ~= 0 and self.data:GetProgress() == 0 then
            SceneCtrl.Instance:AddNpc(data.fight_target)
            self.fight_target = data.fight_target
        end
    end
end

function GoThroughCtrl:CheckBehaviourBtn()
    if not FunOpen.Instance:IsFunOpen(Mod.Challenge.GoThrough) then
        return
    end
    if self.data:GetPassLevelData() == nil then
        self:RemoveBehaviourBtn()
    else
        if self.BehaviourBtn == nil then
            self.BehaviourBtn = BehaviorData.Instance:CustomNpcTalkBtnsId(self.data:GetNpcId(), BindTool.Bind(self.SetupBehaviourBtn, self))
        end
        if self.BehaviourList == nil then
            self.BehaviourList = BehaviorData.Instance:CustomNpcTalkListId(self.data:GetNpcId(), BindTool.Bind(self.SetupBehaviourList, self))
        end
    end
end

function GoThroughCtrl:RemoveBehaviourBtn()
    if self.BehaviourBtn then
        BehaviorData.Instance:RemoveCustomNpcTalkBtnsId(self.BehaviourBtn)
        self.BehaviourBtn = nil
    end
    if self.BehaviourList then
        BehaviorData.Instance:RemoveCustomNpcTalkListId(self.BehaviourList)
        self.BehaviourList = nil
    end
end