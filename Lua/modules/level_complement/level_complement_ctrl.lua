FILE_REQ("modules/level_complement/level_complement_data")
VIEW_REQ("modules/level_complement/level_complement_view")

LevelComplementCtrl = LevelComplementCtrl or BaseClass(BaseCtrl)
function LevelComplementCtrl:__init()
    if LevelComplementCtrl.Instance ~= nil then
        Debuger.LogError("[LevelComplementCtrl] attempt to create singleton twice!")
        return
    end
    LevelComplementCtrl.Instance = self
    self.data = LevelComplementData.New()
    self:RegisterAllProtocols()
end

function LevelComplementCtrl:__delete()
    LevelComplementCtrl.Instance = nil
    EventSys:UnBind(self.care_role_complete)
end

function LevelComplementCtrl:OnInit()
    self.care_role_complete = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RoleNoticeComplete, self))
    FunOpen.Instance:Register(Mod.LevelComplement.Main, BindTool.Bind(self.OnLevelComplementOpen, self))
    FunOpen.Instance:SelfCondition(Mod.LevelComplement.Main,function ()
        if FlyUpData.Instance:IsLevelLimit() then
            return self.data:GetRemainExp() > 0
        else
            return RoleLevel() >= FunOpen.Instance:GetFunOpenLevel(Mod.LevelComplement.Main)
        end
    end)
end

function LevelComplementCtrl:OnUnloadGameLogic()
    TimeHelper:CancelTimer(self.send_timer)
    self.send_timer = nil
    TimeHelper:CancelTimer(self.send_timer2)
    self.send_timer2 = nil
    self.data:ClearData()
end

function LevelComplementCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRoleNewLevelComplementInfo, "OnSCLevelComplementInfo")
    self:RegisterProtocol(SCRoleNewLevelComplementSpecialInfo, "OnSCLevelComplementSpecialInfo")
    self:RegisterProtocol(CSRoleNewLevelComplementReq)
end

function LevelComplementCtrl:OnSCLevelComplementInfo(protocol)
    self.data:SetInfo(protocol)
end
function LevelComplementCtrl:OnSCLevelComplementSpecialInfo(protocol)
    self.data:SetSpecialInfo(protocol)
end

function LevelComplementCtrl:SendReq(req_type)
    local protocol = GetProtocol(CSRoleNewLevelComplementReq)
    protocol.req_type = req_type
    SendProtocol(protocol)
end

function LevelComplementCtrl:SendInfo()
    self:SendReq(LEVEL_COMPLEMENT_TYPE.TYPE_0)
end
function LevelComplementCtrl:SendBuy()
    self:SendReq(LEVEL_COMPLEMENT_TYPE.TYPE_1)
end

function LevelComplementCtrl:RoleNoticeComplete()
    self.send_timer2 = Invoke(function()
        self:CheckShowGuide()
    end, 3)
end

function LevelComplementCtrl:OnLevelComplementOpen(is_open)
    if is_open then
        -- TimeHelper:CancelTimer(self.send_timer)
        -- self.send_timer = Invoke(BindTool.Bind1(self.SendInfo, self), 1)
        self.data:OnValueChange()
    end
end

--等级助力首次指引
function LevelComplementCtrl:CheckShowGuide(is_force)
    if not self.data:IsFunOpen() then
        return
    end
    if ViewMgr:IsOpen(ActiveGuide) or GuideManager.Instance:IsGuide() then
        return
    end
    local save_key = self:SaveKey()
    if PrefsInt(save_key) == 0 or is_force == true then
        PrefsInt(save_key, 1)
        GuideManager.Instance:Start(116)
    end
end
function LevelComplementCtrl:SaveKey()
    return "LevelComplementGuide" .. RoleId()
end
