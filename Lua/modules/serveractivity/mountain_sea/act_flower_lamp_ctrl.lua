ActFlowerLampCtrl = ActFlowerLampCtrl or BaseClass(BaseCtrl)
function ActFlowerLampCtrl:__init()
    if ActFlowerLampCtrl.Instance ~= nil then
		Debuger.LogError("[ActFlowerLampCtrl] attempt to create singleton twice!")
		return
	end
	ActFlowerLampCtrl.Instance = self
    self.data = ActFlowerLampData.New()
    --山海花灯
	self:RegisterProtocol(SCRAFlowerLampBaseInfoMS, "OnSCRAFlowerLampBaseInfoMS")
	self:RegisterProtocol(SCRAFlowerLampGameInfoMS, "OnSCRAFlowerLampGameInfoMS")
    self:RegisterProtocol(SCRAFlowerLampItemInfoMS, "OnSCRAFlowerLampItemInfoMS")
    --山海觅宝
	self:RegisterProtocol(SCRALotusInfoMS, "OnSCRALotusInfoMS")
    

    -- 山海花灯
	Remind.Instance:Register(Mod.MountainSea.FlowerLampMS, self.data.ms_flower_lamp_data, function()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_MS_FLOWER_LAMP) then
			return 0
		end
		return self.data:FlowerLampRemind()
    end)
    -- 山海觅宝
	Remind.Instance:Register(Mod.MountainSea.LotusTwo, self.data.lotus_data, function()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_LOTUS_MS) then
            return 0
        end 
		return self.data:LotusRemindNum()
	end)
end

function ActFlowerLampCtrl:__delete()
	ActFlowerLampCtrl.Instance = nil
	Delete(self, "data")
end

-- 多彩花灯
function ActFlowerLampCtrl:FlowerReq(oper_type,p1,p2,p3)
	local act = ACTIVITY_TYPE.RAND_MS_FLOWER_LAMP
	ServerActivityCtrl.Instance:SendActivityReq(act,oper_type,p1,p2,p3)
end

function ActFlowerLampCtrl:OnSCRAFlowerLampBaseInfoMS(protocol)
	self.data:SetFlowerLampBaseInfo(protocol)
end

function ActFlowerLampCtrl:OnSCRAFlowerLampGameInfoMS(protocol)
	self.data:SetFlowerLampGameInfo(protocol)
	if protocol.info.notice_type == FlowerLampData.NoticeType.GameOver then
		PublicPopupCtrl.Instance:Center(string.format(Language.FlowerLamp.GameOverTip,protocol.info.score))
	end
end

function ActFlowerLampCtrl:OnSCRAFlowerLampItemInfoMS(protocol)
	self.data:SetFlowerLampItemInfo(protocol)
	-- 当没有游戏状态时，强制结束游戏（重连情况下）
	if not self.data.ms_flower_lamp_data.game_info.notice_type then
		if self.data.flower_lamp_end_flag then
			self:FlowerReq(FlowerLampData.ReqType.ForceClose)
			self.data.ms_flower_lamp_data.item_info = {}
			self.data.flower_lamp_end_flag = nil
		end
		self.data.flower_lamp_end_flag = true
	end
end

--山海觅宝
function ActFlowerLampCtrl:OnSCRALotusInfoMS(protocol)
    self.data:SetLotusInfo(protocol)
end


ActFlowerLampData = ActFlowerLampData or BaseClass()
function ActFlowerLampData:__init()
    if ActFlowerLampData.Instance ~= nil then
		Debuger.LogError("[ActFlowerLampData] attempt to create singleton twice!")
		return
    end
    ActFlowerLampData.Instance = self
    self.ms_flower_lamp_data = SmartData.New({base_info = {}, game_info = {}, item_info = {}})	-- 山海花灯
    self.lotus_data = SmartData.New({info = {}})    -- 荷香觅宝
end

-------------------------------------------------------
--山海花灯
-------------------------------------------------------
function ActFlowerLampData:GetFlowerLampCfg()
    return Config.shanhaihuadeng_auto.basis_configuration
end

function ActFlowerLampData:GetFlowerLampItemCfg()
    return Config.shanhaihuadeng_auto.lantern_configuration
end

function ActFlowerLampData:GetFlowerLampItemData(type)
    local cfg = self:GetFlowerLampItemCfg()
    return cfg[type + 1]
end

function ActFlowerLampData:SetFlowerLampBaseInfo(protocol)
    self.ms_flower_lamp_data.base_info = protocol.info
end

function ActFlowerLampData:SetFlowerLampGameInfo(protocol)
    self.ms_flower_lamp_data.game_info = protocol.info
end

function ActFlowerLampData:SetFlowerLampItemInfo(protocol)
    local last_lamp_list = self.ms_flower_lamp_data.item_info.lamp_list or {}
    local lamp_id_list = {}
    for i, v in pairs(protocol.info.lamp_list) do
        local n = (last_lamp_list[i] or 0) + 1
        for j = n, v do
            table.insert(lamp_id_list, i)
        end
    end
    self.ms_flower_lamp_data.item_info = protocol.info
    self.ms_flower_lamp_data.item_info.lamp_id_list = lamp_id_list
end

function ActFlowerLampData:FlowerLampRemainCount()
    local max_count = Config.shanhaihuadeng_auto.other[1].day_times
    return max_count - (self.ms_flower_lamp_data.base_info.day_join_count or 0)
end

function ActFlowerLampData:GetFlowerLampTaskData()
    local cfg = self:GetFlowerLampCfg()
    local task_data = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MS_FLOWER_LAMP, cfg, "task_id")
    local day_task_list = {}
    local act_task_list = {}
    for i, v in pairs(task_data) do
        if v.task_type == 1 then
            table.insert(day_task_list, v)
        else
            table.insert(act_task_list, v)
        end
    end
    local sort_func = function(a, b)
        local a_flag = self:GetFlowerLampTaskFlag(a)
        local b_flag = self:GetFlowerLampTaskFlag(b)
        if a_flag == b_flag then
            return a.task_id < b.task_id
        end
        return a_flag > b_flag
    end
    table.sort(day_task_list, sort_func)
    table.sort(act_task_list, sort_func)
    return task_data, day_task_list, act_task_list
end

-- 1 可领取，0不可领取 -1 已领取
function ActFlowerLampData:GetFlowerLampTaskFlag(task_data)
    local task_id = task_data.task_id
    local flag = self.ms_flower_lamp_data.base_info.task_flags[task_id]
    local progress_num = self:GetFlowerLampTaskProgress(task_data)
    if flag == 1 then
        return -1, progress_num
    else
        if progress_num >= task_data.param_1 then
            return 1, progress_num
        else
            return 0, progress_num
        end
    end
end

function ActFlowerLampData:GetFlowerLampTaskProgress(task_data)
    local task_param = task_data.task_param
    local task_type = task_data.task_type
    if task_param == 1 then
        return self.ms_flower_lamp_data.base_info.day_max_score or 0
    elseif task_param == 2 then
        return self.ms_flower_lamp_data.base_info.sum_score or 0
    elseif task_param == 3 then
        -- 每日任务
        if task_type == 1 then
            return self.ms_flower_lamp_data.base_info.day_join_count or 0
        else
            return self.ms_flower_lamp_data.base_info.act_join_count or 0
        end
    end
end

function ActFlowerLampData:FlowerLampTaskRemind(task_list)
    task_list = task_list or self:GetFlowerLampTaskData()
    for i, v in pairs(task_list) do
        local flag = self:GetFlowerLampTaskFlag(v)
        if flag > 0 then
            return 1
        end
    end
    return 0
end

-- 多彩花灯红点,是否已点击查看，是否有次数，是否有奖励领
function ActFlowerLampData:FlowerLampRemind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_MS_FLOWER_LAMP) then
        return 0
    end
    if not self.ms_flower_lamp_data.base_info.day_join_count then
        return 0
    end
    if self:FlowerLampTaskRemind() > 0 then
        return 1
    end
    if self:FlowerLampRemainCount() > 0 then -- and not self.flower_lamp_look_flag then
        return 1
    end
    return 0
end


-------------------------------------------------------
--山海觅宝
-------------------------------------------------------
function ActFlowerLampData:GetLotusCfg()
    local cfg = Config.shanhaimibao_auto.basis_configuration
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_LOTUS_MS, cfg, "type")
end

function ActFlowerLampData:SetLotusInfo(protocol)
    self.lotus_data.info = protocol.info
end

function ActFlowerLampData:GetLotusActTimeDesc(act_type)
    act_type = act_type or ACTIVITY_TYPE.RAND_LOTUS_MS
    if not ActivityData.IsOpen(act_type) then
        return Language.Activity.NotOpenTip
    end
    local act_info = ActivityData.Instance:GetActivityStatusInfo(act_type)
    local open_time = act_info.param_1
    local end_time = act_info.param_2 - 1
    local str1 = os.date("%m.%d", open_time)
    local str2 = os.date("%m.%d", end_time)
    return string.format("%s ~ %s", str1, str2)
end

function ActFlowerLampData:GetLotusGridListData()
    local cfg = self:GetLotusCfg()
    local list_data = {}
    local active_stuff_list = {} -- 激活材料
    for i, v in pairs(cfg) do
        local index = v.type + 1
        if v.line_num < 5 and index % 5 ~= 0 then
            table.insert(list_data, v)
            if not active_stuff_list[v.activation_item_id] then
                active_stuff_list[v.activation_item_id] = {item_id = v.activation_item_id, type = v.type}
            end
        end
    end
	local new_active_stuff_list = {}
	for k,v in pairs(active_stuff_list) do
		table.insert(new_active_stuff_list,v)
	end
	table.sort(new_active_stuff_list, function (a, b)
		return a.type < b.type
	end)
	table.sort(list_data, function (a, b)
		return a.type < b.type
	end)
	return list_data, new_active_stuff_list
end

function ActFlowerLampData:GetLotusColListData()
    local cfg = self:GetLotusCfg()
    local list_data = {}
    for i, v in pairs(cfg) do
        local index = v.type + 1
        if v.line_num == 5 and index % 5 ~= 0 then
            table.insert(list_data, v)
        end
    end
    return list_data
end

function ActFlowerLampData:GetLotusRowListData()
    local cfg = self:GetLotusCfg()
    local list_data = {}
    for i, v in pairs(cfg) do
        local index = v.type + 1
        if v.line_num < 5 and index % 5 == 0 then
            table.insert(list_data, v)
        end
    end
    return list_data
end

-- 是否已点亮
function ActFlowerLampData:IsLotusGridLighten(data)
    local index = (data.line_num - 1) * 4 + data.seq + 1
    if self.lotus_data.info.grid_list_active_flag[index] == 1 then
        return true
    else
        return false
    end
end

-- 是否全部点亮
function ActFlowerLampData:IsLotusAllLighten()
    local num = 0
    for i, v in pairs(self.lotus_data.info.grid_list_active_flag) do
        if v == 1 then
            num = num + 1
            if num >= 16 then
                return true
            end
        end
    end
    return false
end

-- 是否可提交碎片
function ActFlowerLampData:IsLotusGridPut(data)
    local item_num = Item.GetNum(data.activation_item_id)
    return item_num >= data.activation_item_num
end

-- 是否可领最后一个奖励 1 可，0不可，-1 已领
function ActFlowerLampData:IsLotusEndRewardFlag()
    if self.lotus_data.info.end_reward_flag == 1 then
        return -1
    end
    if not self:IsLotusAllLighten() then
        return 0
    else
        return 1
    end

end

function ActFlowerLampData:LotusRemindNum()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_LOTUS_MS) then
        return 0
    end
    if not self.lotus_data.info or not next(self.lotus_data.info) then
        return 0
    end
    -- 每日首次登陆
    local server_open_day = TimeCtrl.Instance:GetCurOpenServerDay()
    local report_day = ActivityRandData.GetRemind(ACTIVITY_TYPE.RAND_LOTUS_MS)
    if not report_day or report_day < server_open_day then
        return 1
    end
    -- 是否可提交碎片
    local grid_list_data = self:GetLotusGridListData()
    for i, v in pairs(grid_list_data) do
        if not self:IsLotusGridLighten(v) then
            if self:IsLotusGridPut(v) then
                return 1
            end
        end
    end
    -- 是否可领宝箱
    if self:IsLotusEndRewardFlag() > 0 then
        return 1
    end
    return 0
end
