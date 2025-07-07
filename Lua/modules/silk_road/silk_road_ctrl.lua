VIEW_REQ("modules/silk_road/silk_road_view")

SilkRoadCtrl = SilkRoadCtrl or BaseClass(BaseCtrl)

function SilkRoadCtrl:__init()
	if SilkRoadCtrl.Instance ~= nil then
		Debuger.LogError("[SilkRoadCtrl] attempt to create singleton twice!")
		return
	end
	SilkRoadCtrl.Instance = self
	self.data = SilkRoadData.New()
	self:RegisterAllProtocols()
end

function SilkRoadCtrl:__delete()
	SilkRoadCtrl.Instance = nil
	EventSys:UnBind(self.complete_handle)
	self:UnRegisterAllProtocols()
	Delete(self,"data")
end

function SilkRoadCtrl:OnUnloadGameLogic()
	self.data:ClearData()
	self.data.version_reset = nil
	SilkRoadCtrl.IsOnekeyFinish = nil
	self.bounty_submission_info = nil
end

function SilkRoadCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSTransactionTaskReq)
	self:RegisterProtocol(SCTransactionTaskInfo,"OnTransactionTaskInfo")
	ActivityData.Instance:CustomClickHandle(ActType.SilkRoad,function()
		ViewMgr:OpenViewByKey(Mod.SilkRoad.Main)
		if FunOpen.Instance:GetFunIsOpened(Mod.SilkRoad.Main) then
			TaskData.Instance:StopTaskState(TaskConfig.TaskType.silk_road)
		end
	end)
	self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.OnRoleNoticeComplete, self))
end

function SilkRoadCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSTransactionTaskReq)
    self:UnRegisterProtocol(SCTransactionTaskInfo)
end

function SilkRoadCtrl:SendTransactionTaskReq(op_type,index,num)
	local protocol = GetProtocol(CSTransactionTaskReq)
	protocol.op_type = op_type or 0
	protocol.index = index or 0
	protocol.num = num or 0
	SendProtocol(protocol) 
end

function SilkRoadCtrl:OnTransactionTaskInfo(protocol)
	self.data:SetTransactionTaskInfo(protocol)
	local info = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.silk_road)
	if info then
		info:AutoTask()
	end
end

function SilkRoadCtrl:OnRoleNoticeComplete()
	local task_id = Config.transaction_task_auto.other[1].task_id
	if RoleData.Instance:GetRoleLevel() >= Config.transaction_task_auto.task_list[task_id].min_level then
		ActivityData.Instance:SetActivityFinishInfo(ActType.SilkRoad,SilkRoadData.Instance:GetRandom(),SilkRoadData.Instance:GetRandom())
		self:SendTransactionTaskReq()
	end
end

SilkRoadData = SilkRoadData or BaseClass()

function SilkRoadData:__init()
	if SilkRoadData.Instance ~= nil then
		Debuger.LogError("[SilkRoadData] attempt to create singleton twice!")
		return
	end
	SilkRoadData.Instance = self
	self.other = Config.silk_roads_auto.other[1]
	self:Initialization()
	self:InitRewardGroupData()
end

function SilkRoadData:ClearData()
	self.single_data:Clear()
	self.single_list = nil
	self.gather_list = nil
	self.npc_shop_list = nil
	self.order_list = nil
	self.seq_list = nil
	self.type = nil
	self.level = nil
	self.select_data = {}
end

function SilkRoadData:FindBackCoin()
	return self.other.coin
end

function SilkRoadData:FindBackItem()
	return self.other.reward_item
end

function SilkRoadData:Initialization()
	self.select_item = SmartData.New({val = false})
end

function SilkRoadData:SetSelectData(data)
	self.select_data = data
end

function SilkRoadData:GetSelectData()
	return self.select_data or {}
end

function SilkRoadData:GetRandom()
	return Config.transaction_task_auto.other[1].random or 0
end

function SilkRoadData:GetLevel()
	return self.level or RoleData.Instance:GetRoleLevel()
end

function SilkRoadData:GetRewardGroupData(group_id,seq)
	if self.reward_group_data[group_id] then
		return self.reward_group_data[group_id][seq]
	end
end

function SilkRoadData:GetStatus()
	return self.single_data.status or 0
end

function SilkRoadData:SetTransactionTaskInfo(protocol)
	if self.version_reset ~= protocol.version_reset or self:IsDifferent(protocol.seq_list) then
		self:ClearData()
		self.version_reset = protocol.version_reset
		ViewMgr:CloseView(SilkRoadView)
	end
	self.level = protocol.level
	self.type = protocol.type
	self.seq_list = TableCopy(protocol.seq_list)
	self.single_data.status = protocol.task_status
	self.single_data.flag = bit:d2b(protocol.reward_flag)
	self.single_change.val = not self.single_change.val
	ActivityData.Instance:SetActivityFinishInfo(ActType.SilkRoad,self:GetCompleteNum(),self:GetRandom())
	if protocol.task_status == 1 then
		ActivityData.Instance:SetActivityFinishInfo(ActType.SilkRoad,self:GetRandom(),self:GetRandom())
		if RoleData.Instance:CreatedDay() <= 1 then
			TaskData.Instance:StopTaskState(TaskConfig.TaskType.dragon, CUSTOM_OBJID.RESEARCH_TASK)
		end
	end
	self:GetSilkRoadSingleList()
end

function SilkRoadData:GetSingleFlag(index)
	if self.single_data.flag then
		return self.single_data.flag[33 - index] or 0
	end
	return 0
end

function SilkRoadData:IsDifferent(seq_list)
	if self.seq_list == nil then
		return false
	end
	local num = 0
	for i,v in ipairs(seq_list or {}) do
		local info = self.seq_list[i]
		if info.seq ~= v.seq then
			num = num + 1
		end
	end
	return num > 0
end

function SilkRoadData:GetRewardList()
	local config = DataHelper.GetSearch(self.task_config.all_reward,[[level]],RoleLevel())
	if config then
		local list = DataHelper.FormatItemList(config.item_list)
		table.insert(list,Item.New({item_id = CommonItemId.Exp,num = config.exp}))
		return list
	end
	return {}
end

function SilkRoadData:GetCompleteNum()
	local num = 0
	for i=1,self:GetRandom() do
		num = num + self:GetSingleFlag(i)
	end
	return num
end

function SilkRoadData:GetCanSelectIndex()
	local data = self:GetSelectData()
	if data.index and self:GetSingleFlag(data.index) == 0 then
		return data.index
	end
	for i=1,self:GetRandom() do
		if self:GetSingleFlag(i) == 0 then
			return i
		end
	end
	return 0
end

function SilkRoadData:InitRewardGroupData()
	self.task_config = Config.transaction_task_auto
	self.single_data = SmartData.New{flag={},progress={},status=0}
	self.single_change = SmartData.New({val = false})
	self.reward_group_data = {}
	for k,v in pairs(self.task_config.reward_group) do
		if self.reward_group_data[v.group_id] == nil then
			self.reward_group_data[v.group_id] = {}
		end
		self.reward_group_data[v.group_id][v.seq] = v
	end
end

function SilkRoadData:GetRewardGroupId(seq,type)
	for k,v in pairs(self.task_config.level) do
		if v.seq == seq and v.type == type then
			return v.reward_group
		end
	end
end

--Check道具格子数据 single_list
function SilkRoadData:CheckSilkRoadSingleList()
	self.single_list = {}
	for k,v in pairs(self.seq_list or {}) do
		local group_id = self:GetRewardGroupId(v.index,self.type)
		local config = self:GetRewardGroupData(group_id,v.seq)
		if config then
			config.index = v.index
			table.insert(self.single_list,config)
		end
	end
	table.sort(self.single_list,function(a,b)return a.index < b.index end)
	self:CheckTradeData()
end

--获取单个道具格子数据
function SilkRoadData:GetSilkRoadSingleData(item_id)
	for k,v in pairs(self.single_list or {}) do
		if v.need_item.item_id == item_id then
			return v
		end
	end
end

--获取道具格子数据
function SilkRoadData:GetSilkRoadSingleList()
	if self.single_list == nil then
		self:CheckSilkRoadSingleList()
	end
	return self.single_list or {}
end

--是否有远洋贸易任务
function SilkRoadData:IsHasSilkRoadTask()
	if SilkRoadCtrl.IsOnekeyFinish then
		return false
	end
	local task_id = Config.transaction_task_auto.other[1].task_id
	if not TaskData.Instance:GetTaskIsAccepted(task_id) or TaskData.Instance:GetTaskIsCompleted(task_id)then
		return false
	end
	return true
end

--Check订单数据
function SilkRoadData:CheckTradeData()
	self.gather_list = {}
	self.npc_shop_list = {}
	for k,v in pairs(self.single_list) do
		if v.open_panel == Mod.Fight.Gather then
			self.gather_list[v.need_item.item_id] = v
			self:CheckOrderList(v)
		elseif v.open_panel == Mod.Reputation.Main then
			self.npc_shop_list[v.need_item.item_id] = v
		end
	end
end

--Check远洋贸易采集订单
function SilkRoadData:CheckOrderList(data)
	if self.order_list == nil then
		self.order_list = {}
	end
	for k,v in pairs(Split(data.get_way,"|")) do
		local co = BagData.Instance:GetWayCfg(v)
		if co.open_panel == Mod.Fight.Gather then
			local config = FightData.Instance:GetStoneSceneDataBySeq(co.params)
			if config then
				if self.order_list[config.scene_id] == nil then
					self.order_list[config.scene_id] = {}
				end
				table.insert(self.order_list[config.scene_id],data)
			end
		end
	end
end

--获取远洋贸易采集订单
function SilkRoadData:GetOrderList(scene_id)
	if not self:IsHasSilkRoadTask() or self.order_list == nil then
		return
	end
	for k,v in pairs(self.order_list[scene_id] or {}) do
		if self:GetSingleFlag(v.index) == 0 then
			return self.order_list[scene_id]
		end
	end
end

--是否远洋贸易采集道具世界提醒
function SilkRoadData:IsGatherSubmission(item_id,num)
	if not self:IsHasSilkRoadTask() or self.gather_list == nil then
		return false
	end
	if self.gather_list[item_id] then
		local info = self.gather_list[item_id]
		return self:GetSingleFlag(info.index) == 0 and (Item.GetNum(item_id) + num) >= info.need_item.num
	end
	return false
end

--是否远洋贸易采集道具商店标识提醒
function SilkRoadData:IsShopSubmission(item_id)
	if not self:IsHasSilkRoadTask() or self.npc_shop_list == nil then
		return false
	end
	if self.npc_shop_list[item_id] then
		local info = self.npc_shop_list[item_id]
		return self:GetSingleFlag(info.index) == 0 and Item.GetNum(item_id) < info.need_item.num
	end
	return false
end

--找回奖励
function SilkRoadData:GetFindRewardData()
	local list = {}
	local level = self:GetLevel()
	for k,v in pairs(self.task_config.reward_find) do
		if level >= v.min_level and level <= v.max_level then
			local config = self.task_config.reward_find_group[v.group_id] or {}
			list = DataHelper.FormatItemList(config.reward or {})
			table.insert(list,Item.New({item_id = CommonItemId.Exp,num = v.exp}))
			return list,v.find_type or 0
		end
	end
end

--门派任务道具商店标识提醒
function SilkRoadData:CheckBountySubmission(submission_info)
	self.bounty_submission_info = submission_info
end

--是否门派任务道具商店标识提醒
function SilkRoadData:IsBountySubmission(item_id)
	if ActivityData.Instance:GetActiveFlag(8) ~= 0 then
		return false
	end
	if self.bounty_submission_info == nil then
		return false
	end
	if self.bounty_submission_info.item_id == item_id then
		return Item.GetNum(self.bounty_submission_info.item_id) < self.bounty_submission_info.num
	end
	return false
end