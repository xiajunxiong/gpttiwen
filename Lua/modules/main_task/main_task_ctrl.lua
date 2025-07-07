FILE_REQ("modules/main_task/main_task_data")
VIEW_REQ("modules/main_task/main_task_main")
VIEW_REQ("modules/main_task/main_task_view")
MainTaskCtrl = MainTaskCtrl or BaseClass(BaseCtrl)

function MainTaskCtrl:__init()
    if MainTaskCtrl.Instance ~= nil then
        Debuger.LogError("[MainTaskCtrl] attempt to create singleton twice!")
        return
    end
    MainTaskCtrl.Instance = self
    self.data = MainTaskData.New()

    RemindCtrl.Instance:Remind():Register(
        Mod.MainTask.Main,
        MainTaskData.Instance.chapter_info,
        BindTool.Bind(self.FlushMainTaskRemind, self)
    )

    self:RegisterAllProtocols()
end 

function MainTaskCtrl:__delete()
    if MainTaskCtrl.Instance == self then
        MainTaskCtrl.Instance = nil
    end

    self:UnRegisterAllProtocols()
end 

function MainTaskCtrl:OnUnloadGameLogic()
    -- self.data:Clear()
end

function MainTaskCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSBossChapterReq)

    self:RegisterProtocol(SCBossChapterInfo, "OnBossChapterInfo")


    self.handle_level = RoleData.Instance:GetBaseData():KeysCare({"level"}, BindTool.Bind(self.OnRoleLevelChange, self))
	-- FunOpen.Instance:Register(Mod.MainTask.EliteLock, BindTool.Bind(self.FlushMainTaskFunOpen, self))
end 

function MainTaskCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(CSBossChapterReq)
    self:UnRegisterProtocol(SCBossChapterInfo)
end

function MainTaskCtrl:OnBossChapterInfo(protocol)
    self.data:SetBossChapterInfo(protocol)

   --[[  local flag,chapter_num = self.data:CheckMainTaskElite()
    if flag then 
        
    elseif not flag and chapter_num ~= nil then 
        ViewMgr:OpenView(MainTaskView)
        self.data:SetQuickLocal(nil,chapter_num)
        self.data:SetQuickLoclMode(2)
    end 
 ]]
    self.data:ClearBossChapterReason()
end 

function MainTaskCtrl:SendBossChapterReq(param_t) 
    local protocol = GetProtocol(CSBossChapterReq)
    protocol.req_type = param_t.req_type or 0
    protocol.param = param_t.param or 0
    SendProtocol(protocol)
end 

function MainTaskCtrl:FlushMainTaskRemind()
    -- local num = self.data:CheckWithChapterReward()
    return 0
end


function MainTaskCtrl:OnRoleLevelChange()
    local level = RoleData.Instance:GetRoleLevel()
    if not FunOpen.Instance:GetFunIsOpened(Mod.MainTask.EliteLock) then 
        return 
    end 
    for k,v in pairs(self.data:GetMonsterEliteUnlock()) do 
        if v == level then 
            local flag_num = self.data:GetEliteUnLockComplete()
            if flag_num ~= k then 
                TimeHelper:AddDelayTimer(function () 
                    self.data:SetIsUnLock(true)
                    ViewMgr:OpenView(MainTaskView)
                    self.data:SetQuickLocal(nil,k)
                    self.data:SetEliteUnLockSave(k)
                end,5)
                self.data:ReflushEliteUnLockComplete(k)
            end 
            break
        end 
    end 
end