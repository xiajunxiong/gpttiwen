VIEW_REQ("modules/serveractivity/yun_ze/enjoy_redouble/enjoy_redouble_view")

EnjoyRedoubleCtrl = EnjoyRedoubleCtrl or BaseClass(BaseCtrl)
function EnjoyRedoubleCtrl:__init()
    if EnjoyRedoubleCtrl.Instance ~= nil then
		Debuger.LogError("[EnjoyRedoubleCtrl] attempt to create singleton twice!")
		return
	end
	EnjoyRedoubleCtrl.Instance = self
    self.data = EnjoyRedoubleData.New()

    Remind.Instance:Register(Mod.YunZe.EnjoyRedouble, self.data.flush_box_data, function()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE) then
			return 0
		end
		return self.data:GetRemind()
	end)
    
    self:RegisterProtocol(SCRAEnjoyRedoubleBase, "OnSCRAEnjoyRedoubleInfo")
    self:RegisterProtocol(SCRAEnjoyRedoubleTakeParam, "OnSCRAEnjoyRedoubleTakeParam")
    self:RegisterProtocol(SCRAEnjoyRedoubleTaskParamChange, "OnSCRAEnjoyRedoubleTaskParamChange")
end

function EnjoyRedoubleCtrl:__delete()
	EnjoyRedoubleCtrl.Instance = nil
	Delete(self, "data")
end

function EnjoyRedoubleCtrl:OnUnloadGameLogic()
    EnjoyRedoubleData.Instance.first_click = true
end

function EnjoyRedoubleCtrl:OnSCRAEnjoyRedoubleInfo(protocol)
	self.data:SetRewardInfo(protocol)
end

function EnjoyRedoubleCtrl:OnSCRAEnjoyRedoubleTakeParam(protocol)
	self.data:SetTaskData(protocol)
end

function EnjoyRedoubleCtrl:OnSCRAEnjoyRedoubleTaskParamChange(protocol)
	self.data:SetTaskChangeData(protocol)
end

EnjoyRedoubleConfig = {
    ItemPos = {
        [1] = {
            [1] = Vector3.New(0,100,0),
        },
        [2] = {
            [1] = Vector3.New(-99,95,0),
            [2] = Vector3.New(66,94,0),
        },
        [3] = {
            [1] = Vector3.New(-167,66,0),
            [2] = Vector3.New(0,100,0),
            [3] = Vector3.New(167,66,0),
        },
        [4] = {
            [1] = Vector3.New(-229,-19,0),
            [2] = Vector3.New(-99,95,0),
            [3] = Vector3.New(66,94,0),
            [4] = Vector3.New(208,-19,0),
        },
    }
}

--------------------EnjoyRedoubleData--------------------
EnjoyRedoubleData = EnjoyRedoubleData or BaseClass()
function EnjoyRedoubleData:__init()
    if EnjoyRedoubleData.Instance ~= nil then
		Debuger.LogError("[EnjoyRedoubleData] attempt to create singleton twice!")
		return
    end
    EnjoyRedoubleData.Instance = self
    self.flush_box_data = SmartData.New({})
    self.flush_task_data = SmartData.New({})
    EnjoyRedoubleData.Instance.first_click = true
end
-- 红点
function EnjoyRedoubleData:GetRemind()
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE)
    if self.today_free_flag == 0 then
        return 1
    end
    for i,v in ipairs(self:GetRewardInfo()) do
        if v.times then
            if v.times >= v.task and v.is_fetch == false and v.is_activation and v.seq <= open_day then
                return 1
            end
        end
    end
    return 0
end

function EnjoyRedoubleData:GetRewardInfo()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE, Config.fenxiangjiabei_auto.reward_buy, "seq")
end

function EnjoyRedoubleData:GetTaskData()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE, Config.fenxiangjiabei_auto.task)
end

function EnjoyRedoubleData:GetPhaseProgress(index)
    local data = self:GetRewardInfo()[index]
    if self.task_finish_num >= data.task then
        return 1
    end
    local last_task = 0
    if index > 1 then
        last_task = self:GetRewardInfo()[index - 1].task
    end
    if self.task_finish_num > last_task then
        return (self.task_finish_num - last_task) / (data.task - last_task)
    end
    return 0
end

function EnjoyRedoubleData:GetEveryDayReward(day)
    if not self.task_data then 
        self.task_data = {} 
        for i,v in ipairs(Config.fenxiangjiabei_auto.task) do
            if not self.task_data[v.section] then
                self.task_data[v.section] = {}
            end
            v.times = 0
            table.insert(self.task_data[v.section],v)
        end
    end
    if self.task_data[day] == nil then
        if IS_EDITOR then
            LogError("RAND_ENJOY_REDOUBLE 纷享加倍当前活动第",day,"天，缺少第",day,"天的配置")
        else
            BuglyReport("RAND_ENJOY_REDOUBLE 纷享加倍当前活动第",day,"天，缺少第",day,"天的配置")
        end
    end
    return self.task_data[day]
end
function EnjoyRedoubleData:SetCurShowBoxData(seq)
    self.cur_box_seq = seq
    self.flush_box_data:Notify()
end

function EnjoyRedoubleData:SetTaskData(protocol)
    local cur_tsak_list = self:GetEveryDayReward(TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE))
    if cur_tsak_list == nil then  --没有对应天数配置
        return  
    end
    for i,v in ipairs(cur_tsak_list) do
        v.times = protocol.task_list[v.type] > v.parameter and v.parameter or protocol.task_list[v.type]
    end
    self.flush_task_data:Notify()
end

function EnjoyRedoubleData:SetTaskChangeData(protocol)
    local cur_tsak_list = self:GetEveryDayReward(TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_ENJOY_REDOUBLE))
    for i,v in ipairs(cur_tsak_list or {}) do
        if v.type == protocol.task_type then
            v.times = protocol.task_times
        end
    end
    self.flush_task_data:Notify()
end

function EnjoyRedoubleData:SetRewardInfo(protocol)
    local activation_flag = bit:d2b(protocol.activation_flag) --激活标记  第一个位置空出，标记是否全部激活
    local fetch_flag = bit:d2b(protocol.fetch_flag) --领取标记 第一个位置空出
    self.today_free_flag = protocol.today_free_flag
    self.is_all_activation = protocol.activation_flag < 0
    self.task_finish_num = protocol.task_finish_num
    for i,v in ipairs(self:GetRewardInfo()) do
        v.times = self.task_finish_num > v.task and v.task or self.task_finish_num
        v.is_activation = (self.is_all_activation and 1 or activation_flag[32 - i]) == 1
        v.is_fetch = fetch_flag[32 - i] == 1
    end
    self.flush_box_data:Notify()
end