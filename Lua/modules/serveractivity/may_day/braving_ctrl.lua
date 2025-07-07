VIEW_REQ("modules/serveractivity/may_day/braving_view")
-- 乘风破浪
BravingCtrl = BravingCtrl or BaseClass(BaseCtrl)

function BravingCtrl:__init()
    if BravingCtrl.Instance ~= nil then
        Debuger.LogError("[BravingCtrl] attempt to create singleton twice!")
        return
    end
    BravingCtrl.Instance = self
    self.data = BravingData.New()
end

function BravingCtrl:OnInit()
    self:RegisterRemind()
    self:RegisterAllProtocol()
end

function BravingCtrl:RegisterRemind()
    -- 红点
    PeriodActivityCtrl.Instance:RegisterRemind(Mod.MayDay.Braving, self.data.info_data, function()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_BRAVING) then
            return 0
        end
        -- 打开过就没红点了
        if PeriodActivityData.Instance:GetRemindData(Mod.MayDay.Braving) then
            return 0
        end
        -- 没打开过就有红点
        return 1
        -- return self:FlushRemind()
    end)
end

function BravingCtrl:RegisterAllProtocol()
    self:RegisterProtocol(SCRAChengFengpoLangOpenInfo, "OnSCRAChengFengpoLangOpenInfo")
    self:RegisterProtocol(SCRAChengFengpoLangInfo, "OnSCRAChengFengpoLangInfo")
end

function BravingCtrl:__delete()
    if BravingCtrl.Instance == self then
        BravingCtrl.Instance = nil
    end
end

function BravingCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function BravingCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end

-- 发送请求
function BravingCtrl:SendReq(req_type, param1)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_BRAVING, req_type, param1)
end

function BravingCtrl:OnSCRAChengFengpoLangOpenInfo(protocol)
    self.data:SetOpenInfo(protocol)
    local times, _ = self.data:Times()
    local now_stamp = TimeHelper.GetServerTime()
    local end_stamp = ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_BRAVING)
    -- 已经不会再刷新了，就明日再来
    if (end_stamp - now_stamp <= (3600 - self.data:Other().time * 60))then
        MayDayData.Instance:MainBtnState(ACTIVITY_TYPE.RAND_BRAVING, MayDayConfig.MainBtnState.NextDay)
    else
        MayDayData.Instance:MainBtnState(ACTIVITY_TYPE.RAND_BRAVING, times > 0 and MayDayConfig.MainBtnState.Go or MayDayConfig.MainBtnState.NextDay)
    end
end

function BravingCtrl:OnSCRAChengFengpoLangInfo(protocol)
    self.data:SetInfo(protocol)
    local times, _ = self.data:Times()
    local now_stamp = TimeHelper.GetServerTime()
    local end_stamp = ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_BRAVING)
    -- 已经不会再刷新了，就明日再来
    if (end_stamp - now_stamp <= (3600 - self.data:Other().time * 60))then
        MayDayData.Instance:MainBtnState(ACTIVITY_TYPE.RAND_BRAVING, MayDayConfig.MainBtnState.NextDay)
    else
        MayDayData.Instance:MainBtnState(ACTIVITY_TYPE.RAND_BRAVING, times > 0 and MayDayConfig.MainBtnState.Go or MayDayConfig.MainBtnState.NextDay)
    end
end


-----------------------BravingData-----------------------
BravingData = BravingData or BaseClass()

-- 操作请求类型
BravingReqType = {
    Info = 0,       --请求下发
    GetQuestion = 1,--获取题目（废弃）
    Answer = 2,     --回答，p1：1~4
}

function BravingData:__init()
    if BravingData.Instance ~= nil then
        Debuger.LogError("[BravingData] attempt to create singleton twice!")
        return
    end
    BravingData.Instance = self
    self.config = Config.chengfengpolang_auto
    self.other = self.config.other[1]
    self.info_data = SmartData.New()
    self:ResetData()

    Cfg.Register("BravingQuestion", Cfg.RealTimeSearch(self.config.task, "problem_id"))
end

function BravingData:__delete()
    BravingData.Instance = nil
end

function BravingData:ResetData()
    local info = {}
    info.answer_times = 0
    info.question_id = 0    -- 大于0证明有题目
    self.info_data.info = info
    self.info_data.is_open = 0

    self.now_answer = 0    -- 大于0证明回答了
end

function BravingData:NowAnswer(value)
    if value then
        self.now_answer = value
    else
        return self.now_answer
    end
end

function BravingData:IsOpen()
    return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_BRAVING) and self.info_data.is_open == 1
end

function BravingData:Other()
    return self.other
end

-- 返回剩余次数和总次数
function BravingData:Times()
    local num = self.other.frequency - self.info_data.info.answer_times
    num = num >= 0 and num or 0
    return num, self.other.frequency
end

function BravingData:Remind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_BRAVING) then
        return 0
    end
    -- 今日红点
    if not DialogTipsData.Instance:TodayIsTrue(DTNewTodayNotRemind.Braving) then
        return 1
    end
    if self:IsOpen() then
        local times, _ = self:Times()
        return times
    end
    return 0
end

function BravingData:SetTodayRemind()
    if not DialogTipsData.Instance:TodayIsTrue(DTNewTodayNotRemind.Braving) then
        DialogTipsData.Instance:SetTodayRemind(DTNewTodayNotRemind.Braving)
        self.info_data:Notify()
    end
end

-- 获得当前题目数据
function BravingData:QuestionCfg()
    return Cfg.BravingQuestion(self.info_data.info.question_id, false)
end

function BravingData:ShowReward()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_BRAVING, self.config.reward_look)[1]
end

-- 获得当前题目数据，用于SetData
function BravingData:QuestionData()
    local t = {}
    local question = self:QuestionCfg()
    local index = 1
    while question["answer" .. index] do
        local t2 = {}
        t2.index = index
        t2.info = question
        table.insert(t, t2)
        index = index + 1
    end
    return t
end

function BravingData:SetOpenInfo(protocol)
    self.info_data.is_open = protocol.is_open
end

function BravingData:SetInfo(protocol)
    self.info_data.info = protocol.info
end