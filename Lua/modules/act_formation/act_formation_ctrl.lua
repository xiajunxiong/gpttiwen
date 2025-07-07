FILE_REQ("modules/act_formation/act_formation_data")
VIEW_REQ("modules/act_formation/act_formation_view")
VIEW_REQ("modules/act_formation/act_formation_fight_skill_view")

ActFormationCtrl = ActFormationCtrl or BaseClass(BaseCtrl)

function ActFormationCtrl:__init()
    if ActFormationCtrl.Instance ~= nil then
        Debuger.LogError("[ActFormationCtrl] attempt to create singleton twice!")
        return
    end
    ActFormationCtrl.Instance = self
    self.data = ActFormationData.New()
    self:Register()
end

function ActFormationCtrl:Register()
    self:RegisterProtocol(CSSetPetHelperFormation)
    self:RegisterProtocol(CSPetHelperFormation)
    self:RegisterProtocol(SCPetHelperFormationInfo, "OnSCPetHelperFormationInfo")
end

function ActFormationCtrl:OnUnloadGameLogic()
    self.data:ClearCacheFormationInfo()
end

--布阵信息下发并判断是否为自动挑战
function ActFormationCtrl:OnSCPetHelperFormationInfo(protocol)
    self.data:SetActFormationInfo(protocol)
    if self.data.formation_smart.auto_save_type == protocol.system_type then
        local challenge_func, main_type = self.data:GetChallengeFunc()
        if challenge_func ~= nil then
            self.data.formation_smart.auto_save_type = -1
            challenge_func(main_type)
        else
            LogError(Language.ActFormation.SystemType[self.data.cur_system_type].."挑战接口未注册")
        end
    elseif self.data.wait_for_formation_info == protocol.system_type then
        self:OpenView(protocol.system_type, self.cache_param)
    end
end

--保存布阵信息
function ActFormationCtrl:SendSetFormationInfo()
    local formation_info = ActFormationData.Instance:GetFormationInfo()-- self.data:GetFormationInfo()
    local protocol = GetProtocol(CSSetPetHelperFormation)
    protocol.grid_list = formation_info.grid_list
    protocol.standby_list = formation_info.standby_list
    protocol.system_type = formation_info.system_type
    protocol.battle_auto_skill_info = formation_info.skill_list
    SendProtocol(protocol)
end

--发送获取申请
function ActFormationCtrl:SendGetFormationInfo(system_type)
    local protocol = GetProtocol(CSPetHelperFormation)
    protocol.system_type = system_type
    SendProtocol(protocol)
end

function ActFormationCtrl:OpenView(system_type, param)
    --成功设置了数据后才打开界面
    self.cache_param = param
    if self.data:SetCurFormationInfo(system_type) then
        self.data.wait_for_formation_info = -1
        ViewMgr:OpenView(ActFormationView, param)
        self.cache_param = nil
    end
end
