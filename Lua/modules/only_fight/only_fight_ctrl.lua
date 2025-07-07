FILE_REQ("modules/only_fight/only_fight_data")
VIEW_REQ("modules/only_fight/only_fight_rank")
VIEW_REQ("modules/only_fight/only_fight_menu")
OnlyFightCtrl = OnlyFightCtrl or BaseClass(BaseCtrl)
function OnlyFightCtrl:__init()
    if OnlyFightCtrl.Instance ~= nil then
        Debuger.LogError("[OnlyFightCtrl] attempt to create singleton twice!")
        return
    end
    OnlyFightCtrl.Instance = self
    self.data = OnlyFightData.New()
    self:RegisterProtocol(CSActivityOnlyFightOperatorReq)
    self:RegisterProtocol(SCActivityOnlyFightRankAllInfo, "ActivityOnlyFightRankAllInfo")
    self:RegisterProtocol(SCActivityOnlyFightMyInfo, "ActivityOnlyFightMyInfo")
    self:RegisterProtocol(SCActivityOnlyFightSpecialInfo, "ActivityOnlyFightSpecialInfo")
    ActivityData.Instance:CustomClickHandle(
        ActType.OnlyFight,
        function()
            if ActivityData.Instance:GetActivityStatus(ActType.OnlyFight) == ActStatusType.Standy then
                PublicPopupCtrl.Instance:Center(Language.OnlyFight.ActivityTip)
                return true
            end
            SceneCtrl.Instance:RequestEnterFb(FB_TYPE.ONLY_FIGHT)
            self.data:SetDefeatedTimes(0)
        end
    )
    --注册结算事件
    FinishData.Instance:PassRegister(
        BATTLE_MODE.BATTLE_MODE_ONLY_FIGHT,
        {
            value_func = function()
                return self.data:GetSuccReward()
            end
        }
    )
    FinishData.Instance:FailRegister(
        BATTLE_MODE.BATTLE_MODE_ONLY_FIGHT,
        {
            desc_func = function()
                return self.data:GetFailTip()
            end
        }
    )
    ActivityData.Instance:CustomStatusButtonName(ActType.OnlyFight, ActStatusType.Standy, Language.Activity.ActItem.Status[2])
    self.need_show_rank = false
end
function OnlyFightCtrl:OnInit()
    self.handle_end =
        EventSys:Bind(
        GlobalEventKeys.BattleEnd,
        function()
            if self.need_show_rank == true then
                ViewMgr:OpenView(OnlyFightRank)
                self.need_show_rank = false
            end
        end
    )
end

function OnlyFightCtrl:OnActEvent()
    local status = ActivityData.Instance:GetActivityStatus(ActType.OnlyFight)
    if ActivityData.Instance.activity_status_event.act_type == ActType.OnlyFight then
        -- OnlyFightCtrl.ActivityOnlyFightOperatorReq(0)
        self.data:SetDefeatedTimes(0)
        if status == ActStatusType.Close then
            if BattleData.Instance:BaseData().server_fighting == false then
                ViewMgr:OpenView(OnlyFightRank)
            else
                self.need_show_rank = true
            end
        end
    end
end
function OnlyFightCtrl:OnUnloadGameLogic()
    --[[ ActivityData.Instance.activity_status_event:Uncare(self.handle_act) ]]
end
function OnlyFightCtrl:ActivityOnlyFightRankAllInfo(procotol)
    -- LogError("以一敌百1229",procotol)
    self.data:SetRankAllInfo(procotol)
    if ViewMgr:IsOpen(TimeLimitRewardPreview) then
        local reward_list = self.data:GetTotalPreviewReward()
        ActivityCtrl.Instance:FlushTimeLimitReward(reward_list)
    end
end
function OnlyFightCtrl:ActivityOnlyFightSpecialInfo(procotol)
    self.data.nums_info.role_num = procotol.resi_role
end
function OnlyFightCtrl:ActivityOnlyFightMyInfo(procotol)
    -- LogError("以一敌百1230",procotol)
    local level = RoleData.Instance:GetRoleLevel()
    local cfg = ActivityData.Instance:GetActivityInfo(ActType.OnlyFight)
    self.data:SetMyRankInfo(procotol.my_info)
    if procotol.reason_type ~= 0 and level >= cfg.info.level_restrict then
        if BattleData.Instance:BaseData().server_fighting == false then
            ViewMgr:OpenView(OnlyFightRank)
        else
            self.need_show_rank = true
        end
    end
end
function OnlyFightCtrl.ActivityOnlyFightOperatorReq(op_type)
    -- LogError("以一敌百请求",op_type or 0)
    local protocol = GetProtocol(CSActivityOnlyFightOperatorReq)
    protocol.op_type = op_type or 0
    SendProtocol(protocol)
end

function OnlyFightCtrl:RecvGetReward(protocol)
    self.data:SetRewardFlags(protocol)
    if ViewMgr:IsOpen(TimeLimitRewardPreview) then
        local reward_list = self.data:GetTotalPreviewReward()
        ActivityCtrl.Instance:FlushTimeLimitReward(reward_list)
    end
end

function OnlyFightCtrl:SendGetReward(param)
    local param1 = {}
    param1.activity_type = ActType.OnlyFight
    param1.op_type = param.op_type
    param1.param1 = param.param1
    ActivityCtrl.Instance:SendActivityAdvanceFetchReq(param1)
end