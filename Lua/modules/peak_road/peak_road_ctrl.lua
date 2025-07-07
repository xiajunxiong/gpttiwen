FILE_REQ("modules/peak_road/peak_road_data")
VIEW_REQ("modules/peak_road/peak_road_reward")
VIEW_REQ("modules/peak_road/peak_level_show")
VIEW_REQ("modules/peak_road/peak_road_start")
VIEW_REQ("modules/peak_road/peak_achieve_view")
PeakRoadCtrl = PeakRoadCtrl or BaseClass(BaseCtrl)
function PeakRoadCtrl:__init()
    if PeakRoadCtrl.Instance ~= nil then
        Debuger.LogError("[PeakRoadCtrl] attempt to create singleton twice!")
        return
    end
    PeakRoadCtrl.Instance = self
    self.data = PeakRoadData.New()
    self:RegisterAllProtocols()
    self.is_show_level = true
end

function PeakRoadCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSRoleTopLevelReq)
    self:RegisterProtocol(SCRoleTopLevelAllInfo,"SCRoleTopLevelAllInfo")
    self:RegisterProtocol(SCRoleTopLevelNotice,"SCRoleTopLevelNotice")
    self:RegisterProtocol(SCRoleTopLevelAOINotice,"SCRoleTopLevelAOINotice")
end

function PeakRoadCtrl:__delete()
    
end
function PeakRoadCtrl:OnInit()
    self.handle_care = RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.OnRoleLevelChange, self),"level")
    self.handle_login = LoginData.Instance:BaseData():KeysCare({"login_state"}, BindTool.Bind(self.OnLoginEvent, self))
    self.handle_peak_level = self.data:PeakData():Care(BindTool.Bind(self.OnRoleLevelChange, self),"level")
end
function PeakRoadCtrl:OnUnloadGameLogic()
	self.data:Clear()
    self.is_show_level = true
end
function PeakRoadCtrl:OnLoginEvent()
    if LoginData.Instance:LoginState() == LoginStateType.Logined then
        Runner.Instance:RunUntilTrue(function ()
           if SceneCtrl.Instance:CurScene():GetClassType() == GameScene and not ViewMgr:IsOpen(LoadingView)
           and SceneCtrl.Instance:CurScene():LoadingComplete() then
                if self.data:IsPeakAndOpen() and self.is_show_level then
                    ViewMgr:OpenView(PeakLevelShow)
                    self.is_show_level = false
                end
               return true
           elseif SceneCtrl.Instance:CurScene():GetClassType() == BattleScene then
                return true
           end
        end)
    end
end
function PeakRoadCtrl:OnRoleLevelChange()
    self:CheckStartOpen()
    self:CheckTaskOpen()
end
function PeakRoadCtrl:CheckStartOpen()
    if self.data:IsPeakButNoOpen() then
        ViewMgr:OpenView(PeakRoadStart)
    end
end
function PeakRoadCtrl:CheckTaskOpen()
    if not self.data:IsPeakAndOpen() then
        TaskData.Instance:TaskInfoDeleteByTaskId(CUSTOM_OBJID.PEAK_TASK_ID)
        return
    end
    --监听等级并显示巅峰之路任务
    local task = TaskPeakRoad.New()
    task = task:TaskInfo()
    task.task_type = TaskConfig.TaskType.peak_road
    local vo = CommonStruct.CustomTask()
    vo.task_id = CUSTOM_OBJID.PEAK_TASK_ID
    vo.task_type = TaskConfig.TaskType.peak_road
    vo.task_name = Language.PeakRoad.Title
    vo.task_goal = string.format(Language.PeakRoad.TaskDesc, self.data:PeakData().level + 1)
    task.block_view = true
    -- task.dont_show = true
    task:TaskVo(vo)
    TaskData.Instance:TaskInfoUpdateByTaskId(CUSTOM_OBJID.PEAK_TASK_ID, SmartData.New(task))
end
function PeakRoadCtrl:SCRoleTopLevelAllInfo(protocol)
    RoleData.Instance:Set({"top_level"}, protocol)
    self.data:PeakData().level = protocol.top_level
    self.data:PeakData().timestamp = protocol.next_top_level_refresh_timestamp
    self.data:PeakData().reward_fetch_flag = protocol.role_top_level_reward_fetch_flag
    self.data:PeakData().achieve_flag = protocol.top_level_achieve_flag
    self.data:PeakData().is_open = protocol.is_refresh_first_notice_top_level_flag == 1
    self:CheckStartOpen()
    self:CheckTaskOpen()
end
function PeakRoadCtrl:SCRoleTopLevelNotice(protocol)
    if protocol.notice_type == 0 then
    elseif protocol.notice_type == 1 then
        self.data:PeakData().reward_fetch_flag[protocol.param2] = protocol.param1
        self.data:PeakData().reward_refresh = not self.data:PeakData().reward_refresh
    elseif protocol.notice_type == 2 then
        self.data:PeakData().achieve_flag[protocol.param2 + 1] = protocol.param1
        self.data:PeakData().achieve_refresh = not self.data:PeakData().achieve_refresh
    end
end
function PeakRoadCtrl.CSRoleTopLevelReq(op_type, param1)
    local protocol = GetProtocol(CSRoleTopLevelReq)
    protocol.op_type = op_type or 0
    protocol.param1 = param1 or 0
    SendProtocol(protocol)
end

function PeakRoadCtrl:SCRoleTopLevelAOINotice(protocol)
    local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(protocol.obj_id)
    if obj then
        SE:Play(SceneEffectConfig.EffectList.PeakLevelUp, nil, nil, obj.root_node)
    end
end