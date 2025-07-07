LevelComplementData = LevelComplementData or BaseClass()
function LevelComplementData:__init()
    if LevelComplementData.Instance ~= nil then
        Debuger.LogError("[LevelComplementData] attempt to create singleton twice!")
        return
    end
    LevelComplementData.Instance = self

    self.is_show_tips = true
    self.data_info = SmartData.New({accumulated_exp = 0, day_supplement_exp = 0, last_get_daily_exp_timestamp = 0, day_buy_exp_count = 0, level_difference = 0})
    self.my_server_average_level = 0
end

function LevelComplementData:__delete()
    LevelComplementData.Instance = nil
end

function LevelComplementData:ClearData()
    TimeHelper:CancelTimer(self.close_timer)
    self.close_timer = nil
end

function LevelComplementData:Config()
    return Config.level_complement_auto
end
function LevelComplementData:OtherConfig()
    return self:Config().other[1]
end

--是否功能开启
function LevelComplementData:IsFunOpen()
    if not FunOpen.Instance:GetFunIsOpened(Mod.LevelComplement.Main) then
        return false
    end
    if self.data_info.day_supplement_exp <= 0 then
        return false
    end
    return self:GetCurCfg() ~= nil
end

--改成展示10秒自动关闭
function LevelComplementData:IsShowTips()
    return self.is_show_tips
end
function LevelComplementData:SetIsShowTips(is_show)
    self.is_show_tips = is_show
end
function LevelComplementData:ShowTipsTime()
    return 10
end

--
function LevelComplementData:GetExpCfg(level)
    return self:Config().exp_cfg_1[level]
end

function LevelComplementData:GetCurCfg()
    return self:GetExpCfg(self.data_info.level_difference)
end

function LevelComplementData:SetInfo(protocol)
    local flush_value = false
    if self.data_info.accumulated_exp ~= nil and self.data_info.accumulated_exp ~= protocol.accumulated_exp and protocol.accumulated_exp == 0 then
        if FlyUpData.Instance:IsLevelLimit() then
            flush_value = true
        end
    end
    self.data_info.accumulated_exp = protocol.accumulated_exp --累计经验
    self.data_info.day_supplement_exp = protocol.day_supplement_exp --当前凌晨补充的经验数值
    self.data_info.last_get_daily_exp_timestamp = protocol.last_get_daily_exp_timestamp
    self.data_info.day_buy_exp_count = protocol.day_buy_exp_count --当前购买经验次数
    self.data_info.level_difference = math.max(protocol.level_difference, 0) --等级差值(会负数，转下)
    self.data_info.role_level = RoleData.Instance:GetRoleLevel()

    if not self:IsFunOpen() and ViewMgr:IsOpen(LevelComplementView) then
        ViewMgr:CloseView(LevelComplementView)
    end
    --飞升卡等级的时候如果助力经验消耗完需要刷新（隐藏）主界面的助力图标
    if flush_value then
        self:OnValueChange()
    end
end
function LevelComplementData:OnValueChange()
    local role_level = RoleData.Instance:GetRoleLevel()
    if self.data_info.role_level ~= nil then
        if self.data_info.role_level < role_level then
            self.data_info.level_difference = self.data_info.level_difference - (role_level - self.data_info.role_level)
            self.data_info.role_level = role_level
        else
            self.data_info:SetDirty("role_level")
        end
    end
end

function LevelComplementData:SetSpecialInfo(protocol)
    self.my_server_average_level = protocol.my_server_average_level
end

function LevelComplementData:GetInfo()
    return self.data_info
end

--等级差值
function LevelComplementData:GetWorldLevelMean()
    return self.my_server_average_level - self:OtherConfig().rank_difference
end

function LevelComplementData:GetSureValue(level_difference, key)
    local role_level = RoleData.Instance:GetRoleLevel()
    if level_difference > 0 then
        local value = 0
        local exp_cfg = self:GetExpCfg(level_difference)
        for i = role_level, role_level + level_difference do
            local need_exp = Config.roleexp_auto.exp_config[i].exp
            value = value + need_exp * (exp_cfg[key] / 10000)
        end
        return math.floor(value)
    end
    return 0
end

function LevelComplementData:GetNextDayAddExp()
    if self.data_info.level_difference > 0 then
        return self:GetSureValue(self.data_info.level_difference, "exp")
    end
    return 0
end

---剩余经验、经验上限、购买经验数量
function LevelComplementData:GetRemainExp()
    return math.max(self.data_info.accumulated_exp, 0)
end
function LevelComplementData:GetExpLimit()
    local exp_cfg = self:GetExpCfg(self.data_info.level_difference)
    return exp_cfg ~= nil and self.data_info.day_supplement_exp * (exp_cfg.exp_limit / 10000) or 0
end
function LevelComplementData:GetBuyExp()
    local exp_cfg = self:GetExpCfg(self.data_info.level_difference)
    return exp_cfg ~= nil and self.data_info.day_supplement_exp * (exp_cfg.buy_exp / 10000) or 0
end

--获取今日活跃倍率
function LevelComplementData:GetMultiple()
    local last_time = self.data_info.last_get_daily_exp_timestamp
    if last_time > 0 then
        local lerp_day = TimeCtrl.Instance:GetDataDayNum(TimeHelper.GetServerTime(), last_time) - 1
        if lerp_day >= 3 then
            return self:GetCurCfg().multiple_2 / 10000
        elseif lerp_day >= 2 then
            return self:GetCurCfg().multiple_1 / 10000
        end
    end
    return self:GetCurCfg().multiple / 10000
end

function LevelComplementData:GetBuyTimes()
    return self.data_info.day_buy_exp_count
end
function LevelComplementData:GetMaxBuyTimes()
    return self:OtherConfig().buy_num
end
