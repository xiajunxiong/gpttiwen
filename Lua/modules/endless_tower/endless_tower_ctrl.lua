FILE_REQ("modules/endless_tower/endless_tower_data")
VIEW_REQ("modules/endless_tower/endless_tower_view")
VIEW_REQ("modules/endless_tower/endless_tower_rank")

EndlessTowerCtrl = EndlessTowerCtrl or BaseClass(BaseCtrl)

EndlessTowerType = {
    None = 0,
    GoBattle = 1,
    GoNext = 2,
    GoEnd = 3,
    GoNextBattle = 4,
}

function EndlessTowerCtrl:__init()
	if EndlessTowerCtrl.Instance ~= nil then
		Debuger.LogError("[EndlessTowerCtrl] attempt to create singleton twice!")
		return
	end
	EndlessTowerCtrl.Instance = self
	self.data = EndlessTowerData.New()
    self:RegisterAllProtocols()
end

function EndlessTowerCtrl:__delete()
    self:UnRegisterAllProtocols()
    EndlessTowerCtrl.Instance = nil
end

function EndlessTowerCtrl:OnUnloadGameLogic()
end

function EndlessTowerCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSEndlessTowerReq)
    self:RegisterProtocol(SCEndlessTowerInfo, "OnEndlessTowerInfo")
end

function EndlessTowerCtrl:OnInit()
    EventSys:Bind(GlobalEventKeys.RoleNoticeChange, BindTool.Bind(self.OnRoleNoticeComplete, self))
    ChallengeNaviCtrl.Instance:RegisterOpenReqFunc(Mod.Experience.EndlessTower,BindTool.Bind(self.OnEndlessTower, self))
    ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Experience.EndlessTower,function()
        return self.data:GetPassLevel()
    end)
    ActFormationData.Instance:RegistFunctions(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_ENDLESS_TOWER,
	{MainType = self,challenge_func = [[OnEnterEndlessTower]],pet_on_limit_func = [[GetPetNum]],assist_text_func = [[GetAssistTip]]})

    FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_ENDLESS_TOWER,function(is_win)
        self:OnEndlessTower()
        if is_win == 1 then
            self.data:OperaState(EndlessTowerType.GoNext)
            self.data.anims_data.battle_state = 0
        else
            self.data.anims_data.battle_state = 0
        end
        self.data:SetFinishRewardData()
	end)
    FinishData.Instance:PassRegister(BATTLE_MODE.BATTLE_MODE_ENDLESS_TOWER,{
        reward_func = function()
            return self.data:GetFinishRewardData()
        end,
    })
end

function EndlessTowerCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(CSEndlessTowerReq)
    self:UnRegisterProtocol(SCEndlessTowerInfo)
end

function EndlessTowerCtrl:SendEndlessTowerReq(req_type, param1, param2)
    local protocol = GetProtocol(CSEndlessTowerReq)
    protocol.req_type = req_type or 0
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    SendProtocol(protocol)
end

function EndlessTowerCtrl:OnEndlessTowerInfo(protocol)
	self.data:SetEndlessTowerInfo(protocol)
end

function EndlessTowerCtrl:OnRoleNoticeComplete()
    if not FunOpen.Instance:GetFunIsOpened(Mod.Experience.EndlessTower) then
        return
    end
    EndlessTowerCtrl.Instance:SendEndlessTowerReq()
end

--宠物助战提示
function EndlessTowerCtrl:GetAssistTip(assist_num)
    return Format(Language.CourageChallenge.EndAssistTip,assist_num,EndlessTowerData.Instance:GetPetNum())
end

--当前最大可助战宠物数量
function EndlessTowerCtrl:GetPetNum()
	return EndlessTowerData.Instance:GetPetNum()
end

function EndlessTowerCtrl:OnEnterEndlessTower()
    EndlessTowerCtrl.Instance:SendEndlessTowerReq(1)
    ViewMgr:CloseView(EndlessTowerView)
end

function EndlessTowerCtrl:CloseView(battle_mode,param_t)
    if battle_mode == BATTLE_MODE.BATTLE_MODE_ENDLESS_TOWER then
        if param_t.result_type == RESULT_TYPE.FORCED_FINISH then
            self.data.anims_data.battle_state = 0
        else
            self.data.anims_data.battle_state = 1
        end
        self:OnEndlessTower()
    else
        if EndlessTowerCtrl.CloseFunc then
            EndlessTowerCtrl.CloseFunc()
            EndlessTowerCtrl.CloseFunc = nil
        end
        ViewMgr:CloseView(EndlessTowerView)
    end
end

function EndlessTowerCtrl:OnEndlessTower()
    if TeamCtrl.Instance:Data():InTeam() then
        PublicPopupCtrl.Instance:AlertTip(Language.EndlessTower.ExitTeamTip,function()
            TeamCtrl.Instance:SendExitTeam()
            self:InEndlessTower()
        end)
        return
    end
    self:InEndlessTower()
end

function EndlessTowerCtrl:InEndlessTower()
    if self.data:GetPassLevelData() == nil then
        self.data:OperaState(EndlessTowerType.GoEnd)
    end
    if PrefsInt([[EndlessTower]] .. RoleId()) == 0 then
        VideoCtrl.Instance:StartVideoById(6,function()
            ViewMgr:OpenViewByKey(Mod.Experience.EndlessTower)
        end)
        PrefsInt([[EndlessTower]] .. RoleId(),1)
    else
        ViewMgr:OpenViewByKey(Mod.Experience.EndlessTower)
    end
end