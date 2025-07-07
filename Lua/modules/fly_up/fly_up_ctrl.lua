FILE_REQ("modules/fly_up/fly_up_data")
VIEW_REQ("modules/fly_up/fly_up_view")
VIEW_REQ("modules/fly_up/fly_up_succ_view")
VIEW_REQ("modules/fly_up/fly_up_popup_view")
VIEW_REQ("modules/fly_up/fly_up_role_view")
VIEW_REQ("modules/fly_up/fly_up_menu_view")

FlyUpCtrl = FlyUpCtrl or BaseClass(BaseCtrl)

function FlyUpCtrl:__init()
	if FlyUpCtrl.Instance ~= nil then
		Debuger.LogError("[FlyUpCtrl] attempt to create singleton twice!")
		return
	end
	FlyUpCtrl.Instance = self
	self.data = FlyUpData.New()
    self:RegisterAllProtocols()
end

function FlyUpCtrl:__delete()
    self:UnRegisterAllProtocols()
    FlyUpCtrl.Instance = nil
end

function FlyUpCtrl:OnUnloadGameLogic()
    self.data:ClearData()
    self:RemoveSetupBehaviourBtn()
end

function FlyUpCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSFlyUpSystemReq)
    self:RegisterProtocol(SCFlyUpSystemInfo, "OnFlyUpSystemInfo")
    self:RegisterProtocol(SCFlyUpSystemAdvInfo, "OnFlyUpSystemAdvInfo")
    self:RegisterProtocol(SCFlyUpSkillSigleInfo, "OnFlyUpSkillSigleInfo")
    local CheckRoleRemind = function()
        return self.data:GetRoleFlyUpNum()
    end
    RemindCtrl.Instance:Remind():Register(Mod.Role.TabUniqueSkill,BagData.Instance.item_grid,CheckRoleRemind)
	RemindCtrl.Instance:Remind():Register(Mod.Role.TabUniqueSkill,RoleData.Instance.base_data,CheckRoleRemind,"level")
	RemindCtrl.Instance:Remind():Register(Mod.Role.TabUniqueSkill,self.data.red_piont_data,CheckRoleRemind)
	RemindCtrl.Instance:Remind():Register(Mod.FlyUp.Main,BagData.Instance.item_grid,function()
        return self.data:GetRedPiontNum()
    end)
end

function FlyUpCtrl:OnInit()
    EventSys:Bind(GlobalEventKeys.RoleNoticeChange, BindTool.Bind(self.OnRoleNoticeChange, self))
    ActivityData.Instance:CustomClickHandle(ActType.FlyUpDaily,function()
        SceneLogic.Instance:AutoToNpcs(self.data:GetNpcId())
	end)
end

function FlyUpCtrl:OnRoleNoticeChange()
    if not FunOpen.Instance:IsFunOpen(Mod.FlyUp.Main) then
        return
    end
    FlyUpCtrl.Instance:SendFlyUpReq()
end

function FlyUpCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(CSFlyUpSystemReq)
    self:UnRegisterProtocol(SCFlyUpSystemInfo)
    self:UnRegisterProtocol(SCFlyUpSystemAdvInfo)
    self:UnRegisterProtocol(SCFlyUpSkillSigleInfo)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Role.TabUniqueSkill)
	RemindCtrl.Instance:Remind():UnRegister(Mod.FlyUp.Main)
end

--req_type 0请求下发 1进阶 2完成飞升 开启试炼3 升级飞升技能4 接取飞升任务5 晋阶保存6 一键升级技能7
function FlyUpCtrl:SendFlyUpReq(req_type,param1,param2)
    local protocol = GetProtocol(CSFlyUpSystemReq)
    protocol.req_type = req_type or 0
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    SendProtocol(protocol)
end

function FlyUpCtrl:OnFlyUpSystemAdvInfo(protocol)
	self.data:SetFlyUpSystemAdvInfo(protocol)
end

function FlyUpCtrl:OnFlyUpSkillSigleInfo(protocol)
	self.data:SetFlyUpSkillSigleInfo(protocol)
end

function FlyUpCtrl:OnFlyUpSystemInfo(protocol)
    if self.data.info_data.stage and self.data.info_data.stage ~= protocol.stage then
        ViewMgr:OpenView(FlyUpSuccView,{stage_type = protocol.stage})
    end
    if self.data.info_data.stage == 0 and protocol.stage == 1 then
        TitleData.close_popup_func = function()
            SkillData.Instance:PushNewSkillPopup({skill_id = self.data:GetSkillId(),skill_level = 1})
            ViewMgr:OpenView(NewSkillPopupView)
        end
    end
    if self.data.info_data.stage and self.data.info_data.task_index ~= protocol.task_index then
        if protocol.task_index >= FLYUP_MAX_TASK_NUM and protocol.is_one_click_finish ~= 1 then
            self.ShowTimer = Invoke(function()
                ViewMgr:OpenView(GetRewardView,{
                    is_not_show = true, 
                    desc = Format(Language.FlyUp.RewardTip,FLYUP_MAX_TASK_NUM),
                    item_list = self.data:GetTaskRewardData()
                })
            end, 2)
        end
    end
	self.data:SetFlyUpSystemInfo(protocol)
    if self.data:IsCompletedTask() then
        self:InitSetupBehaviourBtn()
    else
        self:RemoveSetupBehaviourBtn()
    end
end

function FlyUpCtrl:InitSetupBehaviourBtn()
    if self.behaviour_handle == nil then
        self.behaviour_handle = BehaviorData.Instance:CustomNpcTalkBtnsSeq(self.data:GetNpcId(), BindTool.Bind(self.SetupBehaviourBtn, self))
    end
end

function FlyUpCtrl:RemoveSetupBehaviourBtn()
	if self.behaviour_handle then
		BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(self.behaviour_handle)
		self.behaviour_handle = nil
	end
end

function FlyUpCtrl:SetupBehaviourBtn(btn_list,obj_vo)
    if obj_vo and obj_vo.id == self.data:GetNpcId() then
		local detail_info = NpcDialogData.CreateDetailInfo1({tips = Language.FlyUp.NpcBtnTips,
            rewards = self.data:GetTaskRewardData()
        })
		local button = NpcDialogBehBtnData.FuncCreate(Language.FlyUp.NpcBtnName,function()
            if not self.data:IsCompletedTask() then
                PublicPopupCtrl.Instance:Center(Language.Activity.ActItem.DoPreTask)
                return
            end
            local cur_num, max_num = FlyUpData.Instance:GetCurrTaskNum()
            if cur_num >= max_num then
                PublicPopupCtrl.Instance:Center(Language.FlyUp.TaskFinishTip)
                return
            end
            local task_info = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.fly_up)
            if task_info ~= nil then
                PublicPopupCtrl.Instance:Center(Language.FlyUp.TaskTip)
                task_info:GoTask()
                return
            end
            TaskData.Instance:StopTaskState(TaskConfig.TaskType.fly_up)
            FlyUpCtrl.Instance:SendFlyUpReq(5)
		end,true,detail_info)
		button:SetIconType(NpcIconType.Act)
        table.insert(btn_list,button)
	end
end