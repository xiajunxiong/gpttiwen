YingChunData = YingChunData or BaseClass()

function YingChunData:__init()
    if YingChunData.Instance ~= nil then
        Debuger.LogError("[YingChunData] attempt to create singleton twice!")
        return
    end
    YingChunData.Instance = self
    self.ying_chun_data = SmartData.New({
        flush_notice=false,
        flush_info=false,
        flush_curr = 0,
    })
    self.ying_chun_info = nil
    self.ying_chun_notice = nil 
    self.isSkipAni=false
end

function YingChunData:SetYingChunInfo(protocol)
    self.ying_chun_info=protocol
    self.ying_chun_data.flush_info = not self.ying_chun_data.flush_info
end

function YingChunData:SetYingChunNotice(protocol)
    self.ying_chun_notice = protocol
    self.ying_chun_data.flush_notice = not self.ying_chun_data.flush_notice
end

--奖励概率
function YingChunData:GetProbility()
    local cfgs=self:GetShowCfg()
    local CenterReward = ""
    local AroundReward = ""
    for k,v in pairs(cfgs) do
        if v.is_award == 1 then
            CenterReward = v.show_rate
        else
            AroundReward = v.show_rate
        end
        if CenterReward ~= "" and AroundReward ~= "" then
            break;
        end
    end
    return {CenterReward=CenterReward,AroundReward=AroundReward}
end

--显示的配置
function YingChunData:GetShowCfg()
    local cfgs = Config.yingchunnafu_auto.basis_configuration
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_YING_CHUN_NA_FU, cfgs) or {}
    return cfg
end

--抽奖记录
function  YingChunData:GetRecord()
    local record={}
    if self.ying_chun_info then
        record = DataHelper.ReverseList(self.ying_chun_info.info.rewards_log)
    end
    return record
end

--抽奖道具id
function YingChunData:GetConsunmeItem()
    return Config.yingchunnafu_auto.other[1].consume_item
end

function YingChunData:FlushCurr()
    local item_id=self:GetConsunmeItem()
    self.ying_chun_data.flush_curr=Item.GetNum(item_id)
end

-- 红点 1：开活动 2：可抽奖
function YingChunData:Remind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_YING_CHUN_NA_FU) then
        return 0
    end
    local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_YING_CHUN_NA_FU)
    if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_YING_CHUN_NA_FU)) then
        return 1
    end
    return self:DrawRemind()
end

-- 抽奖红点
function YingChunData:DrawRemind()
    local item_id = self:GetConsunmeItem()
    local num_have = Item.GetNum(item_id)
    return num_have >= 1 and 1 or 0
end

-- 清除开活动查看红点
function YingChunData:ClearFirstRemind()
    local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_YING_CHUN_NA_FU)
    if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_YING_CHUN_NA_FU)) then
        PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_YING_CHUN_NA_FU), act_start)
        self.ying_chun_data:Notify()
    end
end
