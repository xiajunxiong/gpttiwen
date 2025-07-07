-- self.cooperate_operate
-- self.cooperate_data
function GuildData:SetRoleGuildTaskInfo(protocol)
    self.cooperate_data.finish_times = protocol.finish_times
    self.cooperate_data.task_index = protocol.task_index
    self.cooperate_data.task_list = protocol.task_list


    if self.cooperate_operate.selected_data ~= nil then 
        self.cooperate_operate.selected_data = self.cooperate_data.task_list[self.cooperate_operate.selected_data.index]
    end 

    self.cooperate_data.flush = self.cooperate_data.flush + 1

    if nil ~= EscortCtrl.Instance.task_btn_handle then
        BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(EscortCtrl.Instance.task_btn_handle)
        EscortCtrl.Instance.task_btn_handle = nil
    end
    if self.handle_completed ~= nil then 
        TaskData.Instance:UnlistenTTaskCompleteById(self.handle_completed)
        local task_id = self.cooperate_data.task_list[self.cooperate_data.task_index].task_id
        local task_info_cfg = self:GetCooperateTaskInfoCfg(task_id)
        self.handle_completed = TaskData.Instance:ListenTaskCompleteById(BindTool.Bind(self.OnTaskCompletedChange, self), task_info_cfg.end_task_id)
    elseif  self.cooperate_data.task_index >-1 then 
        local task_id = self.cooperate_data.task_list[self.cooperate_data.task_index].task_id
        local task_info_cfg = self:GetCooperateTaskInfoCfg(task_id)
        if task_info_cfg == nil then  
            return 
        end 
        self.handle_completed = TaskData.Instance:ListenTaskCompleteById(BindTool.Bind(self.OnTaskCompletedChange, self), task_info_cfg.end_task_id)
    end 
end

function GuildData:SetRoleGuildTaskChange(protocol)
    if self.cooperate_data.task_list == nil then 
        self.cooperate_data.task_list = {}
    end 

    self.cooperate_data.task_list[protocol.task_index] = protocol.task_info
    self.cooperate_operate.selected_data = self.cooperate_data.task_list[protocol.task_index]

    if protocol.reason == 0 then  -- 任务开始
        self.cooperate_data.task_index = protocol.task_index
        if self.handle_completed == nil then 
            local task_info_cfg = self:GetCooperateTaskInfoCfg( protocol.task_info.task_id)
            self.handle_completed = TaskData.Instance:ListenTaskCompleteById(BindTool.Bind(self.OnTaskCompletedChange, self), task_info_cfg.end_task_id)
        end 

    elseif protocol.reason == 1 then -- 任务完成
        self.cooperate_data.task_index = -1
        self.cooperate_data.finish_times = self.cooperate_data.finish_times + 1

        if ViewMgr:IsOpen(GuildCooperateTaskView) then
            ViewMgr:OpenView(GuildCooperateTaskView)
        end 

        if nil ~= EscortCtrl.Instance.task_btn_handle then
			BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(EscortCtrl.Instance.task_btn_handle)
			EscortCtrl.Instance.task_btn_handle = nil
		end
    elseif protocol.reason == 2 then -- 任务放弃
        self.cooperate_data.task_index = -1
        
        if nil ~= EscortCtrl.Instance.task_btn_handle then
			BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(EscortCtrl.Instance.task_btn_handle)
			EscortCtrl.Instance.task_btn_handle = nil
		end
        TaskData.Instance:UnlistenTTaskCompleteById(self.handle_completed)
        self.handle_completed = nil
    elseif protocol.reason == 3 then -- 征集完成
        
    end 

    self.cooperate_data.flush = self.cooperate_data.flush + 1
    
    self.cooperate_operate.flush = self.cooperate_operate.flush + 1
end

function GuildData:OnTaskCompletedChange()
    TaskData.Instance:UnlistenTTaskCompleteById(self.handle_completed)
    if self.cooperate_data.finish_times + 1 < Config.family_daily_mission_auto.other[1].tasknum_every_day then 
        ViewMgr:OpenView(GuildCooperateTaskView)
    end 
    self.handle_completed = nil
end

function GuildData:SelectCooperateOperateItem(item)
    self.cooperate_operate.selected_data = item 
end

function GuildData:GetSelectCooperateOperateItem()
    return self.cooperate_operate.selected_data
end


-- 直接返回协议信息
function GuildData:GetCooperateTaskList()
    return self.cooperate_data.task_list  
end

function GuildData:GetCooperateTaskInfoCfg(task_id)
    for k,v in pairs(Config.family_daily_mission_auto.mission_msg) do
        if v.task_id == task_id then 
            return v
        end 
    end 
end

function GuildData:GetCooperateTaskDeatil()
    local fix_oper = {is_empty = true}
    fix_oper.flush_time = self.cooperate_data.finish_times.."/"..Config.family_daily_mission_auto.other[1].tasknum_every_day
    fix_oper.finish_done = self.cooperate_data.finish_times == Config.family_daily_mission_auto.other[1].tasknum_every_day

    local cur_oper = self:GetSelectCooperateOperateItem()
    if cur_oper == nil then 
        return fix_oper
    end 

    fix_oper.refresh_time = cur_oper.refresh_time
    local task_cfg = Config.family_daily_mission_auto.mission_list[cur_oper.task_id]

    if task_cfg == nil then 
        return fix_oper
    end 

    fix_oper.index = cur_oper.index
    fix_oper.is_empty = cur_oper == nil

    local is_accepted = self.cooperate_data.task_index >= 0 and cur_oper.index == self.cooperate_data.task_index

    local task_info_cfg = self:GetCooperateTaskInfoCfg(cur_oper.task_id)
    local task_reward_cfg = Config.family_daily_mission_auto.mission_reward[cur_oper.task_id]
    -- local task_cfg = 

    fix_oper.task_id = task_info_cfg.task_id
    fix_oper.end_task_id = task_info_cfg.end_task_id
    fix_oper.icon = task_info_cfg.mission_icon
    fix_oper.name = task_cfg.task_name
    fix_oper.desc = task_info_cfg.mission_desc-- is_accepted and task_cfg.progress_desc or task_cfg.accept_desc
    fix_oper.reward = BagData.Instance:ItemListBuild(task_info_cfg.reward_show)
    fix_oper.oper_str = Language.GuildCooperateTask.OperStr[task_info_cfg.sub_type]

    fix_oper.flush_cost = Config.family_daily_mission_auto.other[1].refresh_cost
    fix_oper.status_running = is_accepted

    fix_oper.show_help = task_info_cfg.is_help == 1 and is_accepted and not self:GetMyCooHelpDone() and not self:GetMyCooHelpComplete()
    fix_oper.show_help_inter = cur_oper.is_solicit == 0
    fix_oper.show_help_done = self:GetMyCooHelpDone() and is_accepted
    fix_oper.show_oper = is_accepted
    fix_oper.show_accept = self.cooperate_data.task_index < 0 

    fix_oper.sub_type = task_info_cfg.sub_type

    return fix_oper
end

function GuildData:GetShouldShowingTask()
    if self.cooperate_data.task_index < 0 then 
        for k,v in pairs(self.cooperate_data.task_list) do 
            if v.task_id > 0 then 
                return k
            end 
        end 
    else 
        return self.cooperate_data.task_index
    end 
    return 0
end

function GuildData:GetCooperateTaksIsRunning(task_index)
    return self.cooperate_data.task_index == task_index
end

function GuildData:GetCooperateDiffcultStr(color)
    if color == 1 then
        return "_Loc_JianDan"
    elseif color == 2 then 
        return "_Loc_PuTong"
    elseif color == 3 then 
        return "_Loc_KunNan"
    end
end

function GuildData:GetCooperateHelpList()
    local member_info = self:GetMemberInfo()
    local help_list = {}

    local role_id = RoleData.Instance:GetRoleId()
    for k,v in pairs(member_info.member_list) do 
        if v.role_guild_task_is_solicit == 1
           and v.role_guild_task_solicit_task_id > 0  then 
            local task_cfg = Config.family_daily_mission_auto.mission_list[v.role_guild_task_solicit_task_id]

            if task_cfg ~= nil and (v.role_guild_task_solicit_num == 0 and role_id ~= v.uid) 
                or (role_id == v.uid and v.role_guild_task_solicit_fetch_num == 0) then 
                local vo = TableCopy(v)
                table.insert(help_list,vo)
            end 
        end 
    end 

    return help_list
end

function GuildData:CheckTaskIsCoo(task_id)
    if Config.family_daily_mission_auto.mission_list[task_id] ~= nil then 
        return true
    end 
    return false
end

function GuildData:CheckCooMemberIsOnly(uid)
    local member_info = self:GetMemberInfo()
    local notice_num = 0
    local is_notice = false
    for k,v in pairs(member_info.member_list) do
        if v.role_guild_task_is_mail_notice == 0 then
            notice_num = notice_num + 1  
        end

        if uid == v.uid and v.role_guild_task_is_mail_notice == 0 then
            is_notice = true
        end
    end 
    return is_notice and notice_num == 1
end

function GuildData:GetMyCooHelpDone()
    local member_info = self:GetMemberInfo()

    local role_id = RoleData.Instance:GetRoleId()
    for k,v in pairs(member_info.member_list) do 
        if v.uid == role_id then 
            local task_cfg = Config.family_daily_mission_auto.mission_list[v.role_guild_task_solicit_task_id]
            if task_cfg ~= nil then 
                return v.role_guild_task_solicit_num == task_cfg.c_param2 and v.role_guild_task_solicit_fetch_num == 0
            else
                return false
            end 
        end 
    end 

    return false
end

function GuildData:GetMyCooHelpComplete()
    local member_info = self:GetMemberInfo()

    local role_id = RoleData.Instance:GetRoleId()
    for k,v in pairs(member_info.member_list) do 
        if v.uid == role_id then 
            local task_cfg = Config.family_daily_mission_auto.mission_list[v.role_guild_task_solicit_task_id]
            if task_cfg ~= nil then 
                return v.role_guild_task_solicit_fetch_num == 1
            else
                return false
            end 
        end 
    end 

    return false
end

function GuildData:GetIsMissionItem(item_id)
    if self.cooperate_data.task_index == nil or self.cooperate_data.task_index < 0 then 
        return false
    end 

    local task_id = self.cooperate_data.task_list[self.cooperate_data.task_index].task_id
    local task_cfg = Config.family_daily_mission_auto.mission_list[task_id]

    return task_cfg.c_param1 == item_id
end

-- 检查当前任务
function GuildData:PetSealCountinue()
    if self.cooperate_data.task_index == nil or self.cooperate_data.task_index < 0 then 
        return false
    end 
    
    local task_id = self.cooperate_data.task_list[self.cooperate_data.task_index].task_id
    local task_cfg = Config.family_daily_mission_auto.mission_list[task_id]

    if task_cfg.condition ~= 45 then
        return 
    end
    
    local cache_success = false
    for k,v in pairs(PetData.Instance:GetPetList()) do 
		if v.pet_id == task_cfg.c_param1 and v:CanSubmit() then 
            cache_success = true
		end 
	end 

    local task = TaskData.Instance:GetTaskInfoByTaskId(task_id)
    if cache_success then 
        if MainViewData.Instance:IsBattle() then

            if nil ~= self.handle_pet_seal then
                MainViewData.Instance:BaseData():Uncare(self.handle_pet_seal)
                self.handle_pet_seal = nil
            end
            self.handle_pet_seal = MainViewData.Instance:BaseData():Care(function ()
                MainViewData.Instance:BaseData():Uncare(self.handle_pet_seal)
                ViewMgr:CloseView(FightAutoSealInfoView)
                SceneLogic.Instance:ClearMoveCache()
                self.handle_pet_seal = nil
                task:GoTask()
            end, "is_battle")
        else
            SceneLogic.Instance:ClearMoveCache()
            task:GoTask()
        end
    end 
end

function GuildData:CheckIsCooBattle(monster_id)
    if self.cooperate_data.task_index == nil or self.cooperate_data.task_index == -1 then 
        return false
    end 

    local typ_task_id = self.cooperate_data.task_list[self.cooperate_data.task_index].task_id
    local task_cfg = Config.family_daily_mission_auto.mission_list[typ_task_id]
    
    return task_cfg.c_param1 == monster_id 
end

function GuildData:GetCooBattleTeamLimit()
    if self.cooperate_data.task_index == nil or self.cooperate_data.task_index < 0 then 
        return false
    end 

    local task_id = self.cooperate_data.task_list[self.cooperate_data.task_index].task_id
    local task_cfg = Config.family_daily_mission_auto.mission_list[task_id]
    local task_info = self:GetCooperateTaskInfoCfg(task_id)

    return task_info.sub_type
end

function GuildData:GetCooBattleTypeLimit()
    if self.cooperate_data.task_index == nil or self.cooperate_data.task_index < 0 then 
        return false
    end 

    local task_id = self.cooperate_data.task_list[self.cooperate_data.task_index].task_id
    local task_cfg = Config.family_daily_mission_auto.mission_list[task_id]

    return task_cfg.condition == 44
end

function GuildData:GetTypeHelpStr()
    local base_str = Language.GuildCooperateTask.TryHelpStr
    local task_id = self.cooperate_data.task_list[self.cooperate_data.task_index].task_id
    local task_cfg = Config.family_daily_mission_auto.mission_list[task_id]

    local param = {}
    local item_mark = "{guild_cooperate_task;"..task_cfg.c_param1.."}"
    param.str = string.format(base_str,task_cfg.task_name,item_mark)
    param.item = nil 

    return param
end

function GuildData:ShowHelpItem(item_id)
    ItemInfoCtrl.Instance:ItemInfoView{item_list = {Item.Init(item_id)}}
end

function GuildData:GetCooperateRemindNum()

    return ((self:GetMyCooHelpDone() and not self:GetMyCooHelpComplete()) or not self:GetTodayEntered()) and 1 or 0 
end

function GuildData:GetTodayEntered()
    local t0 = TimeManager.FormatUnixTime2Date(os.time())
    local str = t0.year .. "_" .. t0.month .. "_" .. t0.day

    local reday = UnityPlayerPrefs.GetString(PrefKeys.TodayGuildCooRemind())
    return reday == str
end

function GuildData:SetTodayEntered()
    local t0 = TimeManager.FormatUnixTime2Date(os.time())
    local str = t0.year .. "_" .. t0.month .. "_" .. t0.day
    UnityPlayerPrefs.SetString(PrefKeys.TodayGuildCooRemind(), str)
    self.cooperate_data.flush = self.cooperate_data.flush + 1
end

-- function GuildData:CooPetSealInit()
--     local task_id = self.cooperate_data.task_list[self.cooperate_data.task_index].task_id
--     local task_cfg = Config.family_daily_mission_auto.mission_list[task_id]

--     if self.behaviour_handle == nil then
--         self.behaviour_handle = BehaviorData.Instance:CustomNpcTalkBtnsSeq(task_cfg.commit_npc, 
--         BindTool.Bind(self.CooPetSealSetupBehaviourBtn, self))
--     end
-- end

-- function GuildData:CooPetSealComplete()
--     if self.behaviour_handle then
-- 		BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(self.behaviour_handle)
-- 		self.behaviour_handle = nil
-- 	end
-- end

-- function GuildData:CooPetSealSetupBehaviourBtn(btn_list,obj_vo)
--     local task_id = self.cooperate_data.task_list[self.cooperate_data.task_index].task_id
--     local task_cfg = Config.family_daily_mission_auto.mission_list[task_id]
--     local button = NpcDialogBehBtnData.FuncCreate(Language.GuildCooperateTask.BtnFetchPet,function()

--         local beh_par = Split(task_cfg.tasks_dialog_beh,"|")

--         local check_can = false
--         local index = 0
-- 	    for k,v in pairs(PetData.Instance:GetPetList()) do 
-- 		    if v.pet_id == tonumber(beh_par[2]) and v.status ~= 0 then 
-- 			    check_can = true
--                 index = v.index
-- 		    end 
-- 	    end 
        
--         TaskCtrl.Instance:SendGiveNPCTaskNeedItems {
-- 			task_id = task_id,
-- 			commit_list = {{item_id = index,sub_num = 1}}
-- 		}        
--     end)
--     if obj_vo and obj_vo.id == task_cfg.commit_npc and task_cfg.conition == 45 then
--         table.insert(btn_list,button)
--     end 
-- end
