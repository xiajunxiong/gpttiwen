VIEW_REQ("modules/serveractivity/luck_num/luck_num_view")

-- 幸运数字万圣节活动
LuckNumCtrl = LuckNumCtrl or BaseClass(BaseCtrl)

LuckNumCtrl.ReqType= {
    Info = 0,
    GetLuckNum = 1,     -- 抽取幸运数字 p1抽取索引
    GetReward = 2,      -- 领取奖励
}

function LuckNumCtrl:__init()
	if LuckNumCtrl.Instance ~= nil then
		Debuger.LogError("[LuckNumCtrl] attempt to create singleton twice!")
		return
	end
	LuckNumCtrl.Instance = self
	self.data = LuckNumData.New()
    self:RegisterAllProtocols()
    Remind.Instance:Register(Mod.Act_Halloween.LuckNum,self.data.act_info, function ()
        return self.data:Remind() 
    end)
end

function LuckNumCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end


function LuckNumCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRALuckNumInfo, "OnSCRALuckNumInfo")
    self:RegisterProtocol(SCRALuckNumChange, "OnSCRALuckNumChange")
end

function LuckNumCtrl:OnSCRALuckNumInfo(protocol)
    self.data:SetActInfo(protocol)
end

function LuckNumCtrl:OnSCRALuckNumChange(protocol)
    local info = protocol.info
    if info.is_valid == 1 then
        self.data:SetLuckNum(info.luck_num)
    else
        self.data:SetLuckNum(-1)
    end
end


LuckNumData = LuckNumData or BaseClass()

function LuckNumData:__init()
    if LuckNumData.Instance ~= nil then
        Debuger.LogError("[LuckNumData] attempt to create singleton twice!")
        return
    end
    LuckNumData.Instance = self
    self.act_info = SmartData.New()
    self.act_type = ACTIVITY_TYPE.RAND_LUCK_NUM
    self.cfg = Config.xingyunshuzi_auto
end

function LuckNumData:ClearData()
    self.act_info:Set({})
end

function LuckNumData:SetActInfo(protocol)
    local info = protocol.info
    self.act_info.reward_flag = info.reward_flag
    self.act_info.task_list = info.task_list
end

function LuckNumData:SetLuckNum(num)
    self.act_info.luck_num = num
end
function LuckNumData:GetLuckNum()
    return self.act_info.luck_num or -1
end


-- 获取任务信息
function LuckNumData:GetTaskInfo(seq)
    if TableIsEmpty(self.act_info.task_list) then
        return {}
    end
    return self.act_info.task_list[seq]
end

-- 1 获取，2 已获取，3立即前往
function LuckNumData:GetTaskBtnState(seq)
    local info = self:GetTaskInfo(seq)
    if TableIsEmpty(info) then
        return 3
    end
    if info.luck_num >= 0 then
        return 2
    end
    if info.is_finish == 1 then
        return 1
    else
        return 3
    end
end

--- 获取奖励预览配置
function LuckNumData:GetRewardPrevewCfg()
   local cfg = ServerActivityData.Instance:GetActOpenDayCfg(self.act_type,self.cfg.show_reward,"seq")
   return cfg
end

-- 是否中奖了
function LuckNumData:IsLuck()
    local t_num = self:GetLuckNum()
    if t_num < 0 then
        return false
    end
    for i = 0, 2 do
        local task_info = self:GetTaskInfo(i)
        if task_info.luck_num and task_info.luck_num == t_num then
            return true
        end
    end
    return false
end

-- 是否领奖了
function LuckNumData:IsGetedReward()
    if TableIsEmpty(self.act_info) then
        return false
    end
    return self.act_info.reward_flag == 1
end

-- 奖励状态 1 可以领，0不能领，-1 已领取
function LuckNumData:GetRewaredState()
    local t_num = self:GetLuckNum()
    if t_num < 0 then
        return 0
    end
    if self:IsGetedReward() then
        return -1
    end
    return 1
end


-- 活动红点
function LuckNumData:Remind()
    if not ActivityData.IsOpen(self.act_type) then
		return 0
    end
    if TableIsEmpty(self.act_info) then
        return 0
    end
    if self:IsGetedReward() then
        return 0
    end
    for i = 0, 2 do
        local btn_state = self:GetTaskBtnState(i)
        if btn_state == 1 then
            return 1
        end
    end
    local reward_state = self:GetRewaredState()
    if reward_state == 1 then
        return 1
    end
    return 0
end