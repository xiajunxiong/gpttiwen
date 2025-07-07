--殒神之地
FILE_REQ("modules/fallen_god/fallen_god_data")
VIEW_REQ("modules/fallen_god/fallen_god_view")
VIEW_REQ("modules/fallen_god/fallen_god_open_view")
VIEW_REQ("modules/fallen_god/fallen_god_rank_view")

FallenGodCtrl = FallenGodCtrl or BaseClass(BaseCtrl)
function FallenGodCtrl:__init()
    if FallenGodCtrl.Instance ~= nil then
        Debuger.LogError("[FallenGodCtrl] attempt to create singleton twice!")
        return
    end
    FallenGodCtrl.Instance = self
    self.data = FallenGodData.New()
    self:RegisterAllProtocols()
end

function FallenGodCtrl:__delete()
    FallenGodCtrl.Instance = nil
    Delete(self, "data")
end

function FallenGodCtrl:OnInit()
    EventSys:Bind(GlobalEventKeys.RoleNoticeChange, BindTool.Bind(self.OnRoleNoticeComplete, self))
    ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Challenge.FallenGod, function()
        local act_status, next_time = self.data:CheckNextWeekTime()
        local time_str = act_status == 1 and Language.ChallengeNavi.OpenTime or Language.ChallengeNavi.ResetTime
        return self.data:GetRemainBossNum(), nil, next_time, time_str
    end)
    FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_FALLEN_GOD, function(is_win)
        local item_list = FallenGodData.Instance:GetRewardList()
        if item_list ~= nil then
            ViewMgr:OpenView(GiftOpenView, {gift_type = GIFT_OPEN_TYPE.CONGRATULATIONS, item_list = item_list})
            FallenGodData.Instance:SetRewardList(nil)
        end
	end)
    for i, v in ipairs(self.data:GetBossList()) do
        BehaviorData.Instance:CustomNpcTalkBtnsSeq(v.npc_seq, BindTool.Bind(self.SetupBehaviourBtn, self), true)
    end
    ActivityData.Instance:CustomClickHandle(ActType.FallenGod, function()
        if not self.data:IsActOpen() then
            PublicPopupCtrl.Instance:Center(Language.FallenGod.Error4)
            return true
        end
        ViewMgr:OpenViewByKey(Mod.Challenge.FallenGod)
    end)
    ActivityData.Instance:CustomButtonNameFunc(ActType.FallenGod, function()
        if self.data:IsActOpen() then
            return true, Language.Active.BtnName
        end
        return false, Language.FallenGod.OpenDay
    end)
end

function FallenGodCtrl:OnUnloadGameLogic()
    self.is_init_info = false
end

function FallenGodCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCFallenGodInfo,"OnSCFallenGodInfo")
    self:RegisterProtocol(CSFallenGodReq)
end

function FallenGodCtrl:OnSCFallenGodInfo(protocol)
    local old_sn = self.data:GetPassBossSn()
    self.data:SetInfo(protocol)
    if self.is_init_info == true then
        local new_sn = self.data:GetPassBossSn()
        if old_sn < new_sn then
            local next_cfg = self.data:GetBossCfg(new_sn + 1)
            if next_cfg ~= nil then
                PublicPopupCtrl.Instance:Center(Format(Language.FallenGod.Error2, next_cfg.name))
            end
        end
    else
        self.is_init_info = true
    end
    local remain_num = self.data:GetRemainBossNum()
    local max_num = self.data:GetMaxBossNum()
    ActivityData.Instance:SetActivityFinishInfo(ActType.FallenGod, max_num - remain_num, max_num)
end

function FallenGodCtrl:SendReq(type, param1, param2)
    local protocol = GetProtocol(CSFallenGodReq)
	protocol.type = type
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

function FallenGodCtrl:SendFight(sn)
    self:SendReq(FallenGodReqType.TYPE_1, sn)
end

function FallenGodCtrl:SendAllInfo()
    self:SendReq(FallenGodReqType.TYPE_0)
end

function FallenGodCtrl:OnRoleNoticeComplete()
    -- if not FunOpen.Instance:GetFunIsOpened(Mod.Challenge.FallenGod) then
    --     return
    -- end
    self:SendAllInfo()
end

function FallenGodCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    return self:SetupBehaviourBtn2(btn_list, obj_vo)
end
function FallenGodCtrl:SetupBehaviourBtn2(btn_list, obj_vo)
    local act_status, next_time = self.data:CheckNextWeekTime()
    if act_status == 1 then
        return
    end
    return btn_list
end
