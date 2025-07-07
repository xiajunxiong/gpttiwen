SEVEN_DAY_MAX_NUM = 7

VIEW_REQ("modules/serveractivity/openserver/seven_day_view")

SevenDayCtrl = SevenDayCtrl or BaseClass(BaseCtrl)

function SevenDayCtrl:__init()
	if SevenDayCtrl.Instance ~= nil then
		Debuger.LogError("[SevenDayCtrl] attempt to create singleton twice!")
		return
	end
	SevenDayCtrl.Instance = self
	self.data = SevenDayData.New()
	self:RegisterAllProtocols()
end

function SevenDayCtrl:__delete()
	self:UnRegisterAllProtocols()
	SevenDayCtrl.Instance = nil
end

function SevenDayCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSMysteriousTrialReq)
	self:RegisterProtocol(SCMysteriousTrialInfo,"OnRandMysteriousTrialInfo")
	self:RegisterProtocol(SCMysteriousTrialSingleInfo,"OnRandMysteriousTrialSingleInfo")
	self:RegisterProtocol(SCMysteriousRewardInfo,"OnRandMysteriousRewardInfo")
end

function SevenDayCtrl:OnInit()
	RemindCtrl.Instance:Remind():Register(Mod.RandActivity.SevenDay,self.data.goal_red_piont,function()
		return self.data:GetRemindNum()
	end)
	ActivityRandData.Instance:Register(Mod.RandActivity.SevenDay,function()
		return self.data:IsSevenDayOpen()
	end,self.data.info_data)
end

function SevenDayCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSMysteriousTrialReq)
	self:UnRegisterProtocol(SCMysteriousTrialInfo)
	self:UnRegisterProtocol(SCMysteriousTrialSingleInfo)
	self:UnRegisterProtocol(SCMysteriousRewardInfo)
	RemindCtrl.Instance:Remind():UnRegister(Mod.RandActivity.SevenDay)
end

--七天目标全部
function SevenDayCtrl:OnRandMysteriousTrialInfo(protocol)
	self.data:SetMysteriousTrialInfo(protocol)
end

--七天目标单独下发
function SevenDayCtrl:OnRandMysteriousTrialSingleInfo(protocol)
	self.data:SetMysteriousTrialSingleInfo(protocol)
end

--七天奖励单独下发
function SevenDayCtrl:OnRandMysteriousRewardInfo(protocol)
	self.data:SetMysteriousRewardInfo(protocol)
end

function SevenDayCtrl:SendAllReq(op_type,param1,param2)
	local protocol = ProtocolPool.Instance:GetProtocol(CSMysteriousTrialReq)
	protocol.op_type = op_type or 0
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

--===========================SevenDayData=====================================
SevenDayData = SevenDayData or BaseClass()

function SevenDayData:__init()
	if SevenDayData.Instance ~= nil then
		Debuger.LogError("[SevenDayData] attempt to create singleton twice!")
		return
	end
	SevenDayData.Instance = self
	self:InitSevenDayData()
end

function SevenDayData:__delete()
	SevenDayData.Instance = nil
end

function SevenDayData:InitSevenDayData()
	ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoRandActivityConfig_1,function() 
			self.config = Config.randactivityconfig_1_auto
		end,true)
	self.goal_red_piont = SmartData.New({val = false})
	self.select_data = SmartData.New{type = 1,val = false}
	self.reward_data = SmartData.New({flag = {},task_num = 0})
	self.info_data = SmartData.New({day_num = 0,type = -1,max_day = 0,val = false})
	self.goal_data = SmartData.New({info = {},val = false})
	self.reward_id_config = {}
	for k,v in pairs(self.config.mysterious_trial_reward) do
		if self.reward_id_config[v.reward_id] == nil then
			self.reward_id_config[v.reward_id] = {}
		end
		table.insert(self.reward_id_config[v.reward_id],v)
	end
	for k,v in pairs(self.reward_id_config) do
		table.sort(v,DataHelper.SortFunc([[time]],true))
	end
end

function SevenDayData:GetReardItem(reward_id)
    local stamp_time = RoleData.Instance:GetCreateTime()
	for i,v in ipairs(self.reward_id_config[reward_id] or {}) do
		if stamp_time > v.time then
			return v.reward_item[0]
		end
	end
end

function SevenDayData:CheckSevenDayData(type)
	if self.info_data.type ~= type and ViewMgr:IsOpen(SevenDayView) then
		ViewMgr:CloseView(SevenDayView)
	end
	self.goal_list = {}
	self.goal_btn_list = {}
	self.task_remin_list = {}
	self.info_data.type = type or -1
	for k,v in pairs(self.config.mysterious_trial) do
		if v.type == self.info_data.type then
			if self.goal_list[v.start_time] == nil then
				self.goal_list[v.start_time] = {}
				self.task_remin_list[v.start_time] = {}
				self.goal_btn_list[v.start_time] = {day = v.start_time}
				if v.start_time > self.info_data.max_day then
					self.info_data.max_day = v.start_time
				end
			end
			self.goal_list[v.start_time][v.seq] = v
		end
	end
	self.reward_list = {}
	for i,v in ipairs(self.config.cumulative_trial_reward) do
		if v.type == self.info_data.type then
			table.insert(self.reward_list,v)
		end
	end
    self.end_reward_data = self.reward_list[#self.reward_list]
    self.reward_list[#self.reward_list]=nil
end

function SevenDayData:SetSelectType(type)
	self.select_data.type = type
	self.select_data.val = not self.select_data.val
end

function SevenDayData:GetSelectType()
	return self.select_data.type or 1
end

function SevenDayData:GetGoalBtnData()
	return self.goal_btn_list
end

function SevenDayData:GetRewardList()
	return self.reward_list,self.end_reward_data
end

function SevenDayData:GetGoalData()
	local list = {}
	for k,v in pairs(self.goal_list[self.select_data.type]) do
		v.task_info = self:GetTaskInfo(v.start_time,v.seq)
		v.weight = v.seq
		if v.task_info.state == 2 then
			v.weight = 100 + v.seq
		end
		table.insert(list,v)
	end
	table.sort(list,function(a,b)return a.weight < b.weight end)
	return list
end

--服务端下发
function SevenDayData:SetMysteriousTrialInfo(protocol)
	self.info_data.day_num = protocol.day_num
	self.info_data.time_stamp = protocol.time_stamp
	self.goal_data.info = protocol.goal_info
	self.reward_data.flag = bit:d2b(protocol.reward_flag)
	self.reward_data.task_num = protocol.task_num
	self.info_data.val = not self.info_data.val
end

--七天目标单独下发
function SevenDayData:SetMysteriousTrialSingleInfo(protocol)
	local info = protocol.info or {}
	self.reward_data.task_num = protocol.task_num or 0
	if self.goal_data.info[info.start_time] == nil then
		return
	end
	self.goal_data.info[info.start_time][info.seq] = info
	if self.reward_data.task_num ~= protocol.task_num then
		self.reward_data.task_num = protocol.task_num
	end
	self.goal_data.val = not self.goal_data.val
	self:CheckTaskRemin(info.start_time,info.seq,info.state == 1)
	self:CheckRedPoint()
end

--七天奖励单独下发
function SevenDayData:SetMysteriousRewardInfo(protocol)
	self.reward_data.flag = bit:d2b(protocol.reward_flag)
	self:CheckRedPoint()
end

--获取目标领取状态
function SevenDayData:GetTaskInfo(start_time,seq)
	if self.goal_data.info[start_time] then
		return self.goal_data.info[start_time][seq] or {progress = 0,state = 0}
	end
	return {progress = 0,state = 0}
end

--任务领取状态
function SevenDayData:GetRewardFlag(seq)
	if seq == nil then
		return 0
	end
	return self.reward_data.flag[32 - seq] or 0
end

--获取当前任务完成进度
function SevenDayData:GetTaskNum()
	return self.reward_data.task_num or 0,self.end_reward_data and self.end_reward_data.request_score or 35
end

function SevenDayData:GetDayNum()
	return self.info_data.day_num or 0
end

function SevenDayData:GetTimeStamp()
	return self.info_data.time_stamp
end

--红点逻辑
function SevenDayData:GetRemindNum()
	if not FunOpen.Instance:GetFunIsOpened(Mod.RandActivity.SevenDay) then
		return 0
	end
	if not self:IsSevenDayOpen() then
		return 0
	end
	if self.reward_data.task_num and self.reward_data.task_num > 0 then
		for k,v in pairs(self.reward_list or {}) do
			if v.request_score and self.reward_data.task_num >= v.request_score then
				if self:GetRewardFlag(v.seq) == 0 then
					return 1
				end
			end
		end
		if self.end_reward_data then
			if self.end_reward_data.request_score and self.reward_data.task_num >= self.end_reward_data.request_score then
				if self:GetRewardFlag(self.end_reward_data.seq) == 0 then
					return 1
				end
			end
		end
	end
	for _,task_list in pairs(self.task_remin_list or {}) do
		for _,flag in pairs(task_list or {}) do
			if flag ~= nil and flag == true then
				return 1
			end
		end
	end
	return 0
end

function SevenDayData:CheckTaskRemin(start_time,seq,flag)
	if self.task_remin_list[start_time] == nil then
		self.task_remin_list[start_time] = {}
	end
	self.task_remin_list[start_time][seq] = flag == true and true or nil
end

function SevenDayData:GetTaskGroupRemin(start_time)
	for _,flag in pairs(self.task_remin_list[start_time] or {}) do
		if flag then
			return 1
		end
	end
	return 0
end

function SevenDayData:GetFinishNum(start_time)
	local num = 0
	for k,v in pairs(self.goal_data.info[start_time] or {}) do
		if v.state == 2 then
			num = num + 1
		end
	end
	return num,self.goal_data.info[start_time] and #self.goal_data.info[start_time] + 1 or SEVEN_DAY_GOAL_MAX_NUM
end

function SevenDayData:ClearData()
	self.reward_data.flag = {}
	self.reward_data.task_num = 0
	self.goal_data.info = {}
	self.info_data.day_num = 0
	self.info_data.type = -1
	self.task_remin_list = {}
end

function SevenDayData:CheckRedPoint()
	self.goal_red_piont.val = not self.goal_red_piont.val
end

--获取坐骑|时装显示配置
function SevenDayData:GetMountShowCfgByStuffId(stuff_id)
    local cfg = Config.surface_cfg_auto.surface_list
    for _, v in pairs(cfg) do
        if v.active_stuff_id == stuff_id then
            return v
        end
    end
end

-- 获取宠物皮肤|伙伴皮肤的路径
function SevenDayData:GetSkinPath(stuff_id)
    for i, v in pairs(Config.skin_auto.partner_skin) do
        if v.active_stuff_id and v.active_stuff_id == stuff_id then
            return DrawerHelper.GetPartnerPath(v.res_id, v.partner_skin_id), v
        end
    end
    for i, v in pairs(Config.skin_auto.pet_skin) do
        if v.active_stuff_id and v.active_stuff_id == stuff_id then
            return DrawerHelper.GetPetPath(v.res_id, v.pet_skin_id), v
        end
    end
end

function SevenDayData:GetPartnerPath(stuff_id)
	for i, v in pairs(Config.partner_cfg_auto.partner_info) do
        if v.activation_item_norex and v.activation_item_norex == stuff_id then
            return DrawerHelper.GetNpcPath(v.res_id), v
        end
    end
end

function SevenDayData:GetGiftConfig(start_time)
	for k,v in pairs(Config.randactivityconfig_1_auto.trial_buy) do
		if v.type == self:GetDayType() and v.start_time == start_time then
			return v
		end
	end
end

function SevenDayData:IsFnishReward()
	if self.end_reward_data then
		return self:GetRewardFlag(self.end_reward_data.seq) == 1
	end
	return false
end

function SevenDayData:GetDayType()
	return self.info_data.type
end

function SevenDayData:GetMaxDayNum()
	return self.info_data.max_day
end

function SevenDayData:IsSevenDayOpen()
	return FunOpen.Instance:GetFunIsOpened(Mod.RandActivity.SevenDay) and self:GetDayType() ~= -1 and self:GetDayType() <= 1
end