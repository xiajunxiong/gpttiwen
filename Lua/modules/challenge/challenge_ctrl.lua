FILE_REQ("modules/challenge/challenge_data")
FILE_REQ("modules/challenge/challenge_task_data")
VIEW_REQ("modules/challenge/challenge_view")
VIEW_REQ("modules/challenge/challenge_task_view")
VIEW_REQ("modules/challenge/challenge_fuben_win")
VIEW_REQ("modules/challenge/challenge_popup_view")

ChallengeCtrl = ChallengeCtrl or BaseClass(BaseCtrl)

function ChallengeCtrl:__init()
    if ChallengeCtrl.Instance ~= nil then
        Debuger.LogError("[ChallengeCtrl] attempt to create singleton twice!")
        return
    end
    ChallengeCtrl.Instance = self
    self.data = ChallengeData.New()
    self.task_data = ChallengeTaskData.New()

    self:RegisterProtocol(SCTaskCompleteTaskList, "onSCTaskCompleteTaskList")

    self:RegisterProtocol(SCChallengeFbRoleInfor, "OnSCChallengeFbRoleInfor")
    self:RegisterProtocol(SCChallengeFbAwardList, "OnSCChallengeFbAwardList")

    ActivityData.Instance:RegisterCondition(ActType.ChallengeFb, function()
        return self.data:GetConditionTip()
    end)
    ActivityData.Instance:RegisterCondition(ActType.ChallengeTask, function()
        return self.task_data:GetConditionTip()
    end)
    ActivityData.Instance:CustomClickHandle(ActType.ChallengeFb, function()
        ViewMgr:OpenView(ChallengeView)
    end)
    ActivityData.Instance:CustomClickHandle(ActType.ChallengeTask, function()
        ViewMgr:OpenView(ChallengeTaskView)
    end)

    FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_TEAM_CHALLENGE_TASK, function(is_win)
        if is_win then
            if ChallengeTaskData.Instance.team_succ_reward ~= nil then
                local show_list = {}
                for k, v in pairs(ChallengeTaskData.Instance.team_succ_reward) do
                    local item_data = Item.Create(v)
                    if v.item_id == CurrencyType.Chivalrous or v.item_id == HuoBi[14] then
                        PublicPopupCtrl.Instance:CenterI({item_id = v.item_id, num = v.num, color_str = item_data:ColorStr()})
                    else
                        table.insert(show_list, item_data)
                    end
                end
                if #show_list ~= 0 then
                    AllSoulsStorybookCtrl.Instance:OpenRewardView(show_list)
                end
                ChallengeTaskData.Instance.team_succ_reward = nil
            end
        end
    end)
end

function ChallengeCtrl:OnInit()
    ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Experience.Task, function(d)
        local num, total = self.task_data:GetPassTask()
        return (total - num)
    end)
end

function ChallengeCtrl:__delete()
    self:UnRegisterProtocol(SCTaskCompleteTaskList)
    self:UnRegisterProtocol(SCChallengeFbRoleInfor)
    self:UnRegisterProtocol(SCChallengeFbAwardList)
    if ChallengeCtrl.Instance == self then
        ChallengeCtrl.Instance = nil
    end
end

function ChallengeCtrl:onSCTaskCompleteTaskList(protocol)
    self.data.complete_task_list = protocol.complete_task_list
    TaskData.Instance:SetCompleteList(protocol.task_complete_list)
    ChallengeData.Instance:SetFinalTask()
    ConspiracyData.Instance:CheckDaiyInfo()
    EventSys:Fire(GlobalEventKeys.TaskCompleteDataNotice)
end

function ChallengeCtrl:OnSCChallengeFbRoleInfor(protocol)
    for fb_id, chapter_id in pairs(protocol.highest_pass_chapter) do
        self.data:SetFbPassInfo(fb_id, chapter_id)
    end
    self.data:SetFreshTS(protocol.next_refresh_timestamp)
end

function ChallengeCtrl:OnSCChallengeFbAwardList(protocol)
    self.data:SetFbReward(protocol.reward_list)
    ViewMgr:OpenView(ChallengeFubenWin)
end