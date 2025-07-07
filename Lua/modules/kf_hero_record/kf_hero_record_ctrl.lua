VIEW_REQ("modules/kf_hero_record/kf_hero_record_view")
-- 跨服英雄录
KFHeroRecordCtrl = KFHeroRecordCtrl or BaseClass(BaseCtrl)

function KFHeroRecordCtrl:__init()
    if KFHeroRecordCtrl.Instance ~= nil then
        Debuger.LogError("[KFHeroRecordCtrl] attempt to create singleton twice!")
        return
    end
    KFHeroRecordCtrl.Instance = self
    self.data = KFHeroRecordData.New()
end

function KFHeroRecordCtrl:OnInit()
    self:RegisterRemind()
    self:RegisterAllProtocol()
end

function KFHeroRecordCtrl:RegisterRemind()
    local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.KFHeroRecord.Main,
        self.data.info_data,
        remind_func
    )
    ActivityRandData.Instance:Register(Mod.KFHeroRecord.Main, function()
		return self.data:IsOpen()
	end, self.data.info_data)
end

function KFHeroRecordCtrl:RegisterAllProtocol()
    self:RegisterProtocol(CSKuaFuYingXiongLuReq)
    self:RegisterProtocol(SCKuaFuYingXiongLuActivityInfo, "OnSCKuaFuYingXiongLuActivityInfo")
    self:RegisterProtocol(SCKuaFuYingXiongLuRoleAllInfo, "OnSCKuaFuYingXiongLuRoleAllInfo")
    self:RegisterProtocol(SCKuaFuYingXiongLuRoleSingleRankInfo, "OnSCKuaFuYingXiongLuRoleSingleRankInfo")
    self:RegisterProtocol(SCKuaFuYingXiongLuRoleSingleTaskInfo, "OnSCKuaFuYingXiongLuRoleSingleTaskInfo")
    self:RegisterProtocol(SCKuaFuYingXiongLuRoleRankValue, "OnSCKuaFuYingXiongLuRoleRankValue")
end

function KFHeroRecordCtrl:__delete()
    if KFHeroRecordCtrl.Instance == self then
        KFHeroRecordCtrl.Instance = nil
    end
end

function KFHeroRecordCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function KFHeroRecordCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end

-- 发送请求
function KFHeroRecordCtrl:SendReq(req_type, param1, param2)
    local protocol = ProtocolPool.Instance:GetProtocol(CSKuaFuYingXiongLuReq)
    protocol.req_type = req_type
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

function KFHeroRecordCtrl:SendRankInfo()
    for k, v in pairs(KFHeroRecordRank) do
        SocietyCtrl.Instance:SendCrossGetPersonAllRankList(k)
    end
end

function KFHeroRecordCtrl:OnSCKuaFuYingXiongLuActivityInfo(protocol)
    self:MyLogError(ColorStr("跨服英雄录活动信息下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetActInfo(protocol)
    self:SendReq(KFHeroRecordReqType.Check)
end

function KFHeroRecordCtrl:OnSCKuaFuYingXiongLuRoleAllInfo(protocol)
    self:MyLogError(ColorStr("跨服英雄录角色所有信息下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetRoleInfo(protocol)
end

-- 单个榜单结束时，如果玩家有未领取就会发这个
function KFHeroRecordCtrl:OnSCKuaFuYingXiongLuRoleSingleRankInfo(protocol)
    self:MyLogError(ColorStr("跨服英雄录角色单个榜单信息下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetRankInfo(protocol)
end

-- 平时任务状态改变
function KFHeroRecordCtrl:OnSCKuaFuYingXiongLuRoleSingleTaskInfo(protocol)
    self:MyLogError(ColorStr("跨服英雄录角色单个任务信息下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetTaskInfo(protocol)
end

function KFHeroRecordCtrl:OnSCKuaFuYingXiongLuRoleRankValue(protocol)
    self:MyLogError(ColorStr("跨服英雄录角色榜单数值列表信息下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetValueInfo(protocol)
end

function KFHeroRecordCtrl:MyLogError(...)
    -- LogError(...)
end

-----------------------------KFHeroRecordData-----------------------------
KFHeroRecordData = KFHeroRecordData or BaseClass()

-- 操作请求类型
KFHeroRecordReqType = {
    Info = 0,       --请求所有信息
    ActInfo = 1,    --请求活动信息
    RoleInfo = 2,   --请求角色信息
    GetReward = 3,  --请求领取任务奖励，p1：rank_type，p2：seq
    GetValue = 4,   --请求玩家自身榜单数值
    Check = 5,      --服务端需求，收到5311的时候请求一次
}

-- 所需的跨服排行榜类型
KFHeroRecordRank = {
    [9] = 4,    --综合评分
    [32] = 0,   --角色评分
    [33] = 1,   --伙伴评分
    [34] = 2,   --装备评分
    [35] = 3,   --宠物评分
}

KFHeroRecordRankToC = {
    [0] = 32,   --角色评分
    [1] = 33,   --伙伴评分
    [2] = 34,   --装备评分
    [3] = 35,   --宠物评分
    [4] = 9,    --综合评分
}

function KFHeroRecordData:__init()
    if KFHeroRecordData.Instance ~= nil then
        Debuger.LogError("[KFHeroRecordData] attempt to create singleton twice!")
        return
    end
    KFHeroRecordData.Instance = self
    self.config = Config.kuafuyingxionglu_auto
    self.info_data = SmartData.New()
    self.rank_data = SmartData.New()
    self.value_data = SmartData.New()

    -- 榜单总数
    self.rank_max = 5
    self:ResetData()
end

function KFHeroRecordData:__delete()
    KFHeroRecordData.Instance = nil
end

function KFHeroRecordData:ResetData()
    local act_info = {}
    act_info.cross_open_day = 1  --跨服开服天数
    act_info.open_timestamp = 0 --开启时间戳
    act_info.end_timestamp = 0  --结束时间戳
    local state_end_times = {}
    for i = 0, self.rank_max - 1 do
        state_end_times[i] = 0
    end
    act_info.state_end_times = state_end_times  --排行榜结束时间戳
    self.info_data.act_info = act_info

    local role_info = {}
    for i = 0, self.rank_max - 1 do
        local rank_info = {}
        rank_info.rank_type = i
        rank_info.count = 0
        rank_info.task_info = {}
        role_info[i] = rank_info
    end
    self.info_data.role_info = role_info
    self.rank_data:Clear()

    local value_info = {}
    for k, v in pairs(KFHeroRecordRank) do
        value_info[v] = 0
    end
    self.value_data.info = value_info
end

function KFHeroRecordData.DecodeRank(rank_type)
    local rank_info = {}
    rank_info.rank_type = rank_type
    rank_info.count = MsgAdapter.ReadInt()
    local task_info = {}
    for i = 0, rank_info.count - 1 do
        local t = {}
        t.seq = i
        t.is_get = MsgAdapter.ReadChar()
        t.param = MsgAdapter.ReadInt()
        task_info[i] = t
    end
    rank_info.task_info = task_info
    return rank_info
end

function KFHeroRecordData:StateEndTimes(rank_type)
    return self.info_data.act_info.state_end_times[rank_type]
end

function KFHeroRecordData:Remind()
    if not self:IsOpen() then
        return 0
    end
    -- 活动红点
    local act_start = self.info_data.act_info.open_timestamp
    if tostring(act_start) ~= PrefsString(PrefKeys.KFHeroRecordFirst()) then
		-- 活动首次红点，换设备重新提示
		return 1
	end
    -- 任务奖励红点
    for k, v in pairs(self:TaskConfig()) do
        if self:RankOpenState(v.rank_type) == 1 then
            if self:TaskIsCanGet(v) then
                return 1
            end
        end
    end
    return 0
end

-- 是否已领
function KFHeroRecordData:TaskIsGet(rank_type, seq)
    local task_info = self:GetTaskInfo(rank_type, seq)
    return task_info.is_get == 1
end

-- 是否可领
function KFHeroRecordData:TaskIsCanGet(cfg)
    if not self:TaskIsGet(cfg.rank_type, cfg.seq) then
        local task_info = self:GetTaskInfo(cfg.rank_type, cfg.seq)
        return task_info.param >= cfg.parameter2
    end
    return false
end

function KFHeroRecordData:ClearFirstRemind()
    local act_start = self.info_data.act_info.open_timestamp
	if tostring(act_start) ~= PrefsString(PrefKeys.KFHeroRecordFirst()) then
        PrefsString(PrefKeys.KFHeroRecordFirst(), act_start)
        self.info_data:Notify()
	end
end

function KFHeroRecordData:IsOpenTime()
    local now_time = TimeManager.GetServerTime()
    return now_time > self.info_data.act_info.open_timestamp and now_time < self.info_data.act_info.end_timestamp
end

-- 返回该排行榜的开启状态：-1未开启，0即将开启，1已开启
function KFHeroRecordData:RankOpenState(rank_type)
    if not self:IsOpenTime() then
        return -1
    end
    local open_day = self:OpenDay()
    local cfg = self:TimeConfig(rank_type)
    local end_day = cfg.start_time + cfg.continuou_times
    if open_day >= cfg.start_time and open_day < end_day then
        return 1
    elseif open_day < cfg.start_time then
        return 0
    else
        return -1
    end
end

function KFHeroRecordData:IsOpen()
    if not FunOpen.Instance:IsFunOpen(Mod.KFHeroRecord.Main) then
        return false
    end
    return self:IsOpenTime()
end

-- 当前开到第几天，没开是0
function KFHeroRecordData:OpenDay()
    local open_day = 0
    if self:IsOpenTime() then
        open_day = TimeCtrl.Instance:GetDataDayNum(TimeManager.GetServerTime(), self.info_data.act_info.open_timestamp)
    end
    return open_day
end

function KFHeroRecordData:SetActInfo(protocol)
    self.info_data.act_info = protocol.info
    self.info_data:Notify()
end

function KFHeroRecordData:SetRoleInfo(protocol)
    self.info_data.role_info = protocol.role_info
    self.info_data:Notify()
end

function KFHeroRecordData:SetRankInfo(protocol)
    self.info_data.role_info[protocol.rank_type] = protocol.rank_info
    self.info_data:Notify()
end

function KFHeroRecordData:SetTaskInfo(protocol)
    self.info_data.role_info[protocol.rank_type].task_info[protocol.seq] = protocol.info
    self.info_data:Notify()
end

function KFHeroRecordData:SetValueInfo(protocol)
    self.value_data.info = protocol.value_info
end

function KFHeroRecordData:GetValueInfo(rank_type)
    return self.value_data.info[rank_type]
end

function KFHeroRecordData:GetTaskInfo(rank_type, seq)
    return self.info_data.role_info[rank_type].task_info[seq] or {is_get = 0, param = 0}
end

function KFHeroRecordData:BaseConfig()
    if not self.base_cfg then
        self.base_cfg = {}
    end
    local open_day = self.info_data.act_info.cross_open_day
    if not self.base_cfg[open_day] then
        local list = {}
        for k, v in pairs(self.config.basis_configuration) do
            if v.section_start <= open_day and v.section_end >= open_day then
                table.insert(list, v)
            elseif v.section_start <= open_day and v.section_end == 0 then
                table.insert(list, v)
            end
        end
        table.sort(list, DataSorter.KeyLowerSorter("sort"))
        self.base_cfg[open_day] = list
    end
    return self.base_cfg[open_day]
end

function KFHeroRecordData:BaseData(rank_type)
    local list = {}
    local my = {}
    local rank_max = 0
    for k, v in pairs(self:BaseConfig()) do
        if v.rank_type == rank_type then
            rank_max = rank_max + 1
        end
    end
    for k, v in pairs(self:BaseConfig()) do
        if v.rank_type == rank_type then
            local now_rank_data = self.rank_data[KFHeroRecordRankToC[rank_type]]
            if not TableIsEmpty(now_rank_data) and now_rank_data.rank_infos then
                if not TableIsEmpty(now_rank_data.rank_infos) and now_rank_data.rank_infos[v.seq + 1] then
                    local t = {}
                    t.rank_info = now_rank_data.rank_infos[v.seq + 1]
                    t.reward_info = v
                    list[v.seq + 1] = t
                end
                if TableIsEmpty(my) then
                    if not TableIsEmpty(now_rank_data.rank_info_m) then
                        if now_rank_data.rank_info_m.rank == (v.seq + 1) then
                            my.rank_info = TableCopy(now_rank_data.rank_info_m, 2)
                            my.reward_info = v
                            my.is_my = true
                            if my.rank_info.rank > 10 then
                                my.rank_info.no_rank = true
                            end
                        end
                    end
                end
            end
        end
    end
    if TableIsEmpty(my) then
        local my_rank_info = {}
        my_rank_info.rank = rank_max + 1
        my_rank_info.no_rank = true
        my_rank_info.is_my = true
        my_rank_info.rank_value = self:GetValueInfo(rank_type)

        my_rank_info.role = RoleData.Instance:GetRoleData()

        my.rank_info = my_rank_info

        my.reward_info = {}
        my.reward_info.rank_type = rank_type
        my.reward_info.reward1_item = {}
    end
    return list, my
end

function KFHeroRecordData:TaskConfig()
    if not self.task_cfg then
        self.task_cfg = {}
    end
    local open_day = self.info_data.act_info.cross_open_day
    if not self.task_cfg[open_day] then
        local list = {}
        for k, v in pairs(self.config.task_configuration) do
            if v.section_start <= open_day and v.section_end >= open_day then
                table.insert(list, v)
            elseif v.section_start <= open_day and v.section_end == 0 then
                table.insert(list, v)
            end
        end
        table.sort(list, DataSorter.KeyLowerSorter("sort"))
        self.task_cfg[open_day] = list
    end
    return self.task_cfg[open_day]
end

function KFHeroRecordData:TaskData(rank_type)
    local list = {}
    for k, v in pairs(self:TaskConfig()) do
        if v.rank_type == rank_type then
            table.insert(list, v)
        end
    end
    table.sort(list, DataSorter.KeyLowerSorter("sort"))
    return list
end

function KFHeroRecordData:TimeConfig(rank_type)
    if rank_type then
        for k, v in pairs(self.config.time_configuration) do
            if v.rank_type == rank_type then
                return v
            end
        end
    else
        -- 这里排序返回，开启，即将开启，已结束
        local list = TableCopy(self.config.time_configuration, 2)
        table.sort(list, DataHelper.WeightSort("rank_type", function(data)
            local state = self:RankOpenState(data.rank_type)
            if state == 1 then
                return -100
            elseif state == 0 then
                return 0
            else
                return 100
            end
        end))
        for k, v in pairs(list) do
            v.my_index = k
        end
        return list
    end
end

function KFHeroRecordData:SetRankData(protocol)
    if self:IsOpenTime() then
        self.rank_data[protocol.info.rank_type] = protocol.info
        self.rank_data:Notify()
    end
end