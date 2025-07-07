ServerActivityData = ServerActivityData or BaseClass()

function ServerActivityData:__init()
    if ServerActivityData.Instance ~= nil then
        Debuger.LogError("[ServerActivityData] attempt to create singleton twice!")
        return
    end
    ServerActivityData.Instance = self
    self:InitSmartData()
    self:InitExternalData()
    self:InitOpenServerData()
    self.cfg_cache_list = {}
    self.cfg_cache_nums = {}
end
------------------开服列表----------------------
function ServerActivityData:InitOpenServerData()
    self.open_server_list = {}
    for k,v in pairs(Config.activity_main_auto.open_sever_cfg) do
        if v.is_show == 1 then
            v.mod = TabMod(v.mod_key)
            table.insert(self.open_server_list,v)
        end
    end
    table.sort(self.open_server_list,function(a,b)
        return a.seq < b.seq
    end)
end

function ServerActivityData:CheckGroupOpen()
    -- 福牛报喜不是活动，特殊处理
    if ActLuckyCowData.Instance:LuckyCowIsOpen() then
        return true
    end
    -- 寅虎纳福不是活动，特殊处理
    if ActLuckyTigerData.Instance:LuckyTigerIsOpen() then
        return true
    end
    for k,v in pairs(self.open_server_list) do
        if ActivityData.IsOpen(v.act_type) and FunOpen.Instance:GetFunIsOpened(v.mod_key)then
            return true
        end
    end
    if ViewMgr:IsOpen(ServerActivityView) then
        ViewMgr:CloseView(ServerActivityView)
    end
    return false
end

function ServerActivityData:CheckOpenServerData()
    local list = {}
    for i,v in ipairs(self.open_server_list) do
        -- 福牛报喜不是活动，特殊处理
        if v.mod_key == Mod.ServerActivity.LuckyCow and ActLuckyCowData.Instance:LuckyCowIsOpen() then
            table.insert(list,v)
        -- 寅虎纳福不是活动，特殊处理
        elseif v.mod_key == Mod.ServerActivity.LuckyTiger and ActLuckyTigerData.Instance:LuckyTigerIsOpen() then
            table.insert(list,v)
        elseif ActivityData.IsOpen(v.act_type) and FunOpen.Instance:GetFunIsOpened(v.mod_key)then
            table.insert(list,v)
        end
    end
    if #list > 1 then
        table.sort(list,function(a,b)return a.seq < b.seq end)
    end
    if #list < #self.open_server_data then
        self.open_select_data:Set({})
    end
    self.open_server_data:Set(list)
end

function ServerActivityData:GetOpenServerConfig(act_type)
    for k,v in pairs(Config.activity_main_auto.open_sever_cfg) do
        if v.act_type == act_type then
            return v
        end
    end
end

function ServerActivityData:GetOpenServerCfgByModKey(mod_key)
    for k,v in pairs(Config.activity_main_auto.open_sever_cfg) do
        if v.mod_key == mod_key then
            return v
        end
    end
end

function ServerActivityData:SetOpenSelectData(data)
    self.open_select_data:Set(data)
end

function ServerActivityData:GetOpenSelectData()
    return self.open_select_data
end

--新服
function ServerActivityData:GetOpenServerData()
    local list = {}
    for i,v in ipairs(self.open_server_data) do
        table.insert(list,v)
    end
    return list
end

--春节开启条件
function ServerActivityData:GetSpringFestivalEnd()
    for k,v in pairs(Config.activity_main_auto.spring_festival) do
        if ActivityData.IsOpen(v.act_type) then
            return true
        end
    end
    if ViewMgr:IsOpen(SpringFestivalView) then
        ViewMgr:CloseView(SpringFestivalView)
    end
    return false
end

--春节
function ServerActivityData:GetSpringFestivalList()
    local list = {}
    for k,v in pairs(Config.activity_main_auto.spring_festival) do
        v.mod = TabMod(v.mod_key)
        table.insert(list,v)
    end
    table.sort(list,function(a,b)return a.seq < b.seq end)
    return list
end

--五一
function ServerActivityData:GetLabourDayEnd()
    for k,v in pairs(Config.activity_main_auto.burning_may_day) do
        if ActivityData.IsOpen(v.act_type) then
            return true
        end
    end
    if ViewMgr:IsOpen(LabourDayView) then
        ViewMgr:CloseView(LabourDayView)
    end
    return false
end

--五一
function ServerActivityData:GetMayDayList()
    local list = {}
    for k,v in pairs(Config.activity_main_auto.burning_may_day) do
        v.mod = TabMod(v.mod_key)
        table.insert(list,v)
    end
    table.sort(list,function(a,b)return a.seq < b.seq end)
    return list
end

--暑期
function ServerActivityData:GetSummerFirstList()
    local list = {}
    for k,v in pairs(Config.activity_main_auto.summer_first) do
        v.mod = TabMod(v.mod_key)
        table.insert(list,v)
    end
    table.sort(list,function(a,b)return a.seq < b.seq end)
    return list
end

function ServerActivityData:GetSummerFirstStage()
    return Config.activity_main_auto.other[1].phase
end

--暑期 暑期活动分为三期 由配置 phase 控制
function ServerActivityData:GetSummerFirstEnd()
    local page_list = DataHelper.GetPageData(self:GetSummerFirstList(),6)
    for k,v in pairs(page_list[self:GetSummerFirstStage()] or {}) do
        if ActivityData.IsOpen(v.act_type) then
            return true
        end
    end
    if ViewMgr:IsOpen(SummerFirstView) then
        ViewMgr:CloseView(SummerFirstView)
    end
    return false
end

function ServerActivityData:IsSummerFirstRemind(act_type)
    if not ActivityData.IsOpen(act_type) then
        return false
    end
    local page_list = DataHelper.GetPageData(self:GetSummerFirstList(),6)
    for k,v in pairs(page_list[self:GetSummerFirstStage()] or {}) do
        if v.act_type == act_type then
            return true
        end
    end
    return false
end

--国庆
function ServerActivityData:GetNationalDayEnd()
    for k,v in pairs(Config.activity_main_auto.national_day) do
        if ActivityData.IsOpen(v.act_type) then
            return true
        end
    end
    if ViewMgr:IsOpen(NationalDayView) then
        ViewMgr:CloseView(NationalDayView)
    end
    return false
end

--国庆
function ServerActivityData:GetNationalDayList()
    local list = {}
    for k, v in pairs(Config.activity_main_auto.national_day) do
        v.mod = TabMod(v.mod_key)
        -- 去除已结束
        if self:GetNowActState(v.act_type) ~= 3 then
            table.insert(list, TableCopy(v, 2))
        end
    end
    -- 排序
    table.sort(list, function(a, b)
        return self:GetNowActState(a.act_type) * 100 + a.seq < self:GetNowActState(b.act_type) * 100 + b.seq
    end)
    for i, v in ipairs(list) do
        v.index = i
        -- 增加“新”的角标
        if ActivityData.IsOpen(v.act_type) then
            v.is_new = ActivityData.Instance:IsHasSign(v.act_type)
        else
            v.is_new = false
        end
    end
    return list
end

-- 月满中秋 活动入口显示
function ServerActivityData:GetYmzqShow()
    -- 这里写死用累充活动的时间来取配置
    local cfg = self:GetOpenConfig(ACTIVITY_TYPE.RAND_YUE_XI_LEI_CHONG, Config.activity_main_auto.moon_mid_autumn)
    for k, v in pairs(cfg) do
        if ActivityData.IsOpen(v.act_type) and FunOpen.Instance:GetFunIsOpened(v.mod_key) and v.is_hidden ~= 0 then
            return true
        end
    end
    if ViewMgr:IsOpen(YmzqView) then
        ViewMgr:CloseView(YmzqView)
    end
    return false
end

-- 月满中秋 子活动显示
function ServerActivityData:GetYmzqList()
    local list = {}
    local cfg = self:GetOpenConfig(ACTIVITY_TYPE.RAND_YUE_XI_LEI_CHONG, Config.activity_main_auto.moon_mid_autumn)
    for k, v in pairs(cfg) do
        v.mod = TabMod(v.mod_key)
        if self:GetNowActState(v.act_type) ~= 3 and FunOpen.Instance:GetFunIsOpened(v.mod_key) and v.is_hidden ~= 0 then
            table.insert(list, TableCopy(v))
        end
    end
    table.sort(list, function(a, b)
        return a.location < b.location
    end)
    return list
end

--万圣活动 入口显示
function ServerActivityData:GetHalloweenShow()
    local cfg=Config.activity_main_auto.spring_festival
    for k, v in pairs(cfg) do
        if ActivityData.IsOpen(v.act_type) and FunOpen.Instance:GetFunIsOpened(v.mod_key)  then
            return true
        end
    end
    if ViewMgr:IsOpen(ActHalloweenView) then
        ViewMgr:CloseView(ActHalloweenView)
    end
    return false
end

-- 获取当前活动状态（1：已开启，2：准备开启，3：已结束）
function ServerActivityData:GetNowActState(act_type)
    local open_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(act_type)
    return ActivityData.IsOpen(act_type) and 1 or ((open_time or 0) >= TimeManager:GetServerTime() and 2 or 3)
end

--随机活动当前表
function ServerActivityData:GetCurrentRandActivityConfig()
    return Config.randactivityconfig_1_auto
end

--获取随机列表其他配置
function ServerActivityData:GetOtherConfig(key)
    local config = self:GetCurrentRandActivityConfig()
    return key and config.other[1][key] or config.other[1]
end

-- 获取活动的配置验证
function ServerActivityData:GetCfgVer(activity_type)
    return (Config.buy_item_validation_auto.basis_configure[activity_type] or {}).cfg_ver or 0
end

--@G根据section_start字段查找满足条件范围的天数配置 is_section配置不是用section_start 和 section_end
function ServerActivityData:GetSectionList(act_cfg,sort_key,server_start,is_section)
    if act_cfg == nil then
        return {} --配置找不到的时候
    end
    if act_cfg[#act_cfg].section_start == nil and is_section == nil then
        return act_cfg or {}--没有天数的时候
    end
    local server_start = server_start or TimeCtrl.Instance:GetCurOpenServerDay()
    server_start = server_start <= 0 and 1 or server_start
    if self.cfg_cache_list[act_cfg] and self.cfg_cache_nums[act_cfg] then
        if self.cfg_cache_nums[act_cfg] == server_start then
            return self.cfg_cache_list[act_cfg]
        end
    end
    local info_list = {}
    for k,v in pairs(act_cfg or {}) do
        local section_num = is_section == true and v.start_time or v.section_start
        local section_end = is_section == true and v.end_time or v.section_end
        if section_num <= server_start and section_end >= server_start then
            table.insert(info_list, v)
        elseif section_num <= server_start and section_end == 0 then
            table.insert(info_list, v)
        end
    end
    if next(info_list) == nil then
        for k,v in pairs(act_cfg or {}) do
            table.insert(info_list,v)
        end
    end
    if sort_key ~= nil then
        table.sort(info_list, function(a, b) return a[sort_key] < b[sort_key] end)
    end
    --缓存一个配置列表
    self.cfg_cache_nums[act_cfg] = server_start
    self.cfg_cache_list[act_cfg] = info_list
    return info_list or {}
end

--旧 从随机活动表里获取【开服活动】开启配置 其他开服活动传other_cfg配置 根据新开服第几天（活动期间配置刷新）【新服活动】
--默认情况下 GetSectionList里面就是根据 TimeCtrl.Instance:GetCurOpenServerDay() 新服天数(新开服务器) 筛选 可以直接用 ServerActivityData.Instance:GetSectionList(config)
function ServerActivityData:GetActivityOpenCfg(key,sort_key,other_cfg)
    local config = other_cfg or self:GetCurrentRandActivityConfig()[key]
    return self:GetSectionList(config,sort_key)
end

--根据活动开启时的开服天数（活动存续期间这个天数是不变的）（根据9001服务器时间戳）获取配置 （活动期间配置不刷新）
--根据【不同区服开启的服务器时间戳】获取对应活动开启时的开服天数的配置
function ServerActivityData:GetActOpenDayCfg(act_type,config,sort_key)
    return self:GetSectionList(config,sort_key,TimeCtrl.Instance:GetActOpenServerDay(act_type))
end
    
--特殊 根据活动开启时的开服天数（活动存续期间这个天数是不变的）获取配置·六点开服 如上
--根据【不同区服开启的服务器时间戳】获取对应活动开启时的开服天数的配置
function ServerActivityData:GetActOpenDayCfgSixStart(act_type,config,sort_key)
    return self:GetSectionList(config,sort_key,TimeCtrl.Instance:GetActOpenServerDaySixStart(act_type))
end

--获取配置 获取活动开启第几天（根据1200协议时间戳）配置（活动期间配置不刷新）
--统一的活动开启固定时间获取配置 比如国庆节开启的活动
function ServerActivityData:GetOpenConfig(act_type,config,sort_key)
    return self:GetSectionList(config,sort_key,TimeCtrl.Instance:GetActOpenDay(act_type))
end

--根据活动开启天数（根据9001服务器时间戳）获取配置 获取活动开启第几天（根据1200协议时间戳）配置 存在 start_time 和 end_time 字段时 如上
function ServerActivityData:GetOpenConfigStartTime(act_type,config,sort_key)
    return self:GetSectionList(self:GetActOpenDayCfg(act_type,config),sort_key,TimeCtrl.Instance:GetActOpenDay(act_type),true)
end