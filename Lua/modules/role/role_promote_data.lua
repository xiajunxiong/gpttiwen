
RolePromoteData = RolePromoteData or BaseClass()

function RolePromoteData:__init()
	if RolePromoteData.Instance ~= nil then
		return
	end
	RolePromoteData.Instance = self
	self:InitPromoteHelpData()
	self.info_data = SmartData.New({sub_level = 0,sub_fight_finish = 0})
	self.prof_config = TabGroup(Config.career_advancements_auto.career_advancements,[[job_id]])
end

function RolePromoteData:InitPromoteHelpData()
	self.promote_help_list = {}
	for k,v in pairs(Config.career_advancements_auto.strategy_desc) do
		if self.promote_help_list[v.type] == nil then
			self.promote_help_list[v.type] = {}
		end
		self.promote_help_list[v.type][v.seq] = v
	end
	self.promote_help_data = SmartData.New({type = 1})
end

function RolePromoteData:SetAdvanceCareerSubInfo(protocol)
	self.info_data.sub_level = protocol.sub_level
	self.info_data.sub_fight_finish = protocol.sub_fight_finish
end

function RolePromoteData:GetSubFightFinish()
	return self.info_data.sub_fight_finish
end

--获取当前职业进阶的等级
function RolePromoteData:GetProfGrade()
	return self.info_data.sub_level
end

--获取当前职业的阶段id
function RolePromoteData:GetProfId()
	return RoleData.Instance:GetRoleProfId()
end

--获取对应职业配置
function RolePromoteData.GetProfConfig(prof_id,order)
	return Cfg.CareerAdvancementsGrade(prof_id,order or 0,false)
end

--获取对应职业名字
function RolePromoteData.GetProfName(prof_id,order)
	local config = Cfg.CareerAdvancementsGrade(prof_id,order or 0)
	return config and config.job_name or Language.Common.ProfNone
end

--获取当前职业进阶最大等级
function RolePromoteData:GetMaxGrade(prof_id)
	local config = self.prof_config[prof_id] or {}
	return config[#config].order or 0
end

--获取当前职业进阶等级名字
function RolePromoteData.GetGradeName(order)
    return Format(Language.Role.PromoteOrderTip,DataHelper.GetDaXie(order + 1))
end

--获取角色职业对应晋阶等级配置
function RolePromoteData.GetRoleProfConfig()
	return Cfg.CareerAdvancementsGrade(RoleData.Instance:GetRoleProfId(),RolePromoteData.Instance:GetProfGrade())
end

--获取角色职业下一阶对应晋阶等级配置，没有的话返回nil
function RolePromoteData.GetRoleProfNextConfig()
	local prof_id = RoleData.Instance:GetRoleProfId()
	if RolePromoteData.Instance:GetMaxGrade(prof_id) ~= RolePromoteData.Instance:GetProfGrade() then
		return Cfg.CareerAdvancementsGrade(RoleData.Instance:GetRoleProfId(), RolePromoteData.Instance:GetProfGrade() + 1)
	else
		return Cfg.CareerAdvancementsGrade(RoleData.Instance:GetRoleProfId() + 1, 0)
	end
end

--获取角色职业上一阶对应晋阶等级配置，没有的话返回nil
function RolePromoteData.GetRoleProfLastConfig()
	local prof_id = RoleData.Instance:GetRoleProfId()
	if 0 ~= RolePromoteData.Instance:GetProfGrade() then
		return Cfg.CareerAdvancementsGrade(RoleData.Instance:GetRoleProfId(), RolePromoteData.Instance:GetProfGrade() - 1)
	end
end

--获取下一阶段职业配置（大阶段）
function RolePromoteData.GetNextProfConfig(prof_id,order)
	return RolePromoteData.GetProfConfig(prof_id,order + 1) or RolePromoteData.GetProfConfig(prof_id + 1) 
end

--获取上一个阶段职业配置（大阶段）
function RolePromoteData.GetLastProfConfig(prof_id)
	return RolePromoteData.GetProfConfig(prof_id - 1,RolePromoteData.Instance:GetMaxGrade(prof_id - 1)) or {}
end

--获取上一个阶段职业配置（小阶段）
function RolePromoteData.GetLastSmallProfConfig()
	local prof_id = RoleData.Instance:GetRoleProfId()
	local prof_grade = RolePromoteData.Instance:GetProfGrade()
	local max_grade = RolePromoteData.Instance:GetMaxGrade(prof_id)
	if prof_grade == 0 then
		prof_id = prof_id - 1
		prof_grade = RolePromoteData.Instance:GetMaxGrade(prof_id)
	else
		prof_grade = prof_grade - 1
	end

	return RolePromoteData.GetProfConfig(prof_id, prof_grade) or {}
end

--获取晋升攻略
function RolePromoteData:GetPromoteHelpData()
	local type = self.promote_help_data.type
	return self.promote_help_list[type] or {}
end

--获取晋升职业限制技能等级
function RolePromoteData:GetPromoteSkillLimit(skill_id)
	local config = Cfg.CareerSkillList(RoleData.Instance:GetRoleProf(),skill_id)
	return config and config.skill_level_limit or 0
end

--获取晋升任务
function RolePromoteData:GetPromoteTaskList(cfg)
	local task_config = TaskData.Instance:GetCurPromoteTaskData()
	local pre_task_config = TaskData.Instance:GetAdvancedTaskConfig(cfg.pre_task)
	local task_list = {
        [1] = {
			task_desc = Format(Language.Role.PromoteCurrLevelTip,cfg.need_level),
			target = RoleData.Instance:GetRoleLevel() < cfg.need_level,
			event = function()
				if RoleData.Instance:GetRoleLevel() < 32 then
					ViewMgr:OpenViewByKey(Mod.MainTask.Task)
				else
					ViewMgr:OpenViewByKey(Mod.Activity.Daily)
				end
			end},
		[2] = {
			task_desc = Language.Role.PromoteScoreTip .. cfg.pre_score .. (RoleData.Instance:GetRoleScore() < cfg.pre_score and Format(Language.Role.PromoteCurrScoreTip,RoleData.Instance:GetRoleScore()) or ""),
			target = RoleData.Instance:GetRoleScore() < cfg.pre_score,
			event = function()
				ViewMgr:OpenViewByKey(Mod.BianQiang.Main)
			end
		},	
    }
	if task_config and pre_task_config then
		local list = {task_desc = pre_task_config.task_name,target = not TaskData.Instance:GetTaskIsCompleted(cfg.pre_task),event = function()
			if RoleData.Instance:GetRoleLevel() < pre_task_config.min_level then
				PublicPopupCtrl.Instance:Center(Format(Language.Role.PromoteTaskLevel,pre_task_config.min_level))
				return
			end
			if not TaskData.Instance:GetTaskIsAccepted(task_config.task_id) then
				TaskCtrl.Instance:SendTaskAccept(task_config.task_id)
			else
				local task_info = TaskData.Instance:GetTaskInfoByTaskId(task_config.task_id)
				if task_info then
					AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task_info)}, 1)
				end
			end
			ViewMgr:CloseView(RolePromoteView)
		end}
		table.insert(task_list,list)
	else
		local list = {
			promote_task = Language.Role.PromoteTaskDescTip,
			task_desc = cfg.desc,
			target = self:GetSubFightFinish() == 0,
			event = function ()
				if cfg.npc ~= "" then
					SceneLogic.Instance:AutoToNpcs(cfg.npc)
					ViewMgr:CloseView(RolePromoteView)
					return
				end
				if cfg.open_panel ~= "" then
					if FightConfig.IS_SINGLE_MOD[cfg.open_panel] then
						FightCtrl.Instance:IsSingleFight(function()
							ViewMgr:OpenViewByKey(cfg.open_panel)
							ViewMgr:CloseView(RolePromoteView)
						end)
						return
					end

					local is_open, _ = FunOpen.Instance:GetFunIsOpened(cfg.open_panel)
					
					if not is_open then
						GuideManager.Instance:ForceStop()
					end
					ViewMgr:OpenViewByKey(cfg.open_panel)
					ViewMgr:CloseView(RolePromoteView)
					return
				end
				-- RoleCtrl.Instance:SendAdvanceCareerFightReq()
				-- RoleCtrl.Instance:SendAdvanceCareerFightReq()
			end
		}
		table.insert(task_list,list)
	end
	return task_list
end

--获取晋升红点
function RolePromoteData:GetPromoteNum()
    local prof_id = RoleData.Instance:GetRoleProfId()
    local prof_grade = RolePromoteData.Instance:GetProfGrade()
    local config = RolePromoteData.GetProfConfig(prof_id,prof_grade)
	if config ~= nil then
		for k,v in pairs(self:GetPromoteTaskList(config)) do
			if v.target then
				return 0
			end
		end
		-- 需要计算铜币是否足够
		return RoleData.Instance:GetCurrency(CurrencyType.CoinBind) >= config.need_bind_coin and 1 or 0
	end
	return 0
end

function RolePromoteData:IsGetPromoteTask(task_id)
	local prof_id = RoleData.Instance:GetRoleProfId()
    local prof_grade = RolePromoteData.Instance:GetProfGrade()
    local config = RolePromoteData.GetProfConfig(prof_id,prof_grade)
	if config ~= nil then
		local task_list = self:GetPromoteTaskList(config)
		if task_list[task_id] and task_list[task_id].target then
			return true
		end
	end
	return false
end

function RolePromoteData:SendTaskDemand()
	local cur_config = RolePromoteData.GetRoleProfConfig()
	local task_config = TaskData.Instance:GetCurPromoteTaskData()
	if cur_config ~= nil then
		TaskCtrl.Instance:SendTaskDemand(cur_config.pre_task)
	end
	if task_config ~= nil then
		TaskCtrl.Instance:SendTaskDemand(task_config.task_id)
	end
end

function RolePromoteData:GetAddList(cur_config,next_config)
    local list = {}
    table.insert(list,{name = Language.Role.PromoteOrderAddTip[1],old_value = cur_config.bonus_show,new_value = next_config.bonus_show})
    table.insert(list,{name = Language.Role.PromoteOrderAddTip[2],old_value = cur_config.bonus_show,new_value = next_config.bonus_show})
    return list
end

function RolePromoteData:GetMaxAddList(cur_config)
    local list = {}
    table.insert(list,{name = Language.Role.PromoteOrderAddTip[1],old_value = cur_config.bonus_show})
    table.insert(list,{name = Language.Role.PromoteOrderAddTip[2],old_value = cur_config.bonus_show})
    return list
end