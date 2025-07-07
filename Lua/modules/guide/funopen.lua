--------------------------------
--功能开启(针对窗口)
--------------------------------
FunOpen = FunOpen or BaseClass(BaseCtrl)
FunOpen.ConstNotice = {
    MaoYao = 15,
    XingTian = 24,
    LingLu = 31,
}
function FunOpen:__init()
    if FunOpen.Instance ~= nil then
        Debuger.LogError("[FunOpen] attempt to create singleton twice!")
        return
    end
    FunOpen.Instance = self

    self.all_config = Config.funopen_auto
    self.funopen_config = self.all_config.funopen
    self.function_notice = Config.function_notice_auto
    self.func_notice =  Config.function_notice_auto.notice

    self.func_notice_sort = KeyList(self.func_notice,"sn")
    self.opened_list = {}
    self.opened_mod_list = {}

    self:checkConfig()
    -- 监听人物等级变化
    -- 监听任务变化
    -- 初始化需检查哪些已经开启
    self.all_fun_checked = false
    self.check_handle_list = {}
    self.custom_condition = {}
    self.handle_cares = {}

    self:RegisterProtocol(SCFuncPreviewInfo, "RecvFuncPreviewInfo")
    self:RegisterProtocol(CSFuncPreviewClaimReward)

    -- 当前打开的功能预告data
    self.cur_open_func_notice_data = {}
    -- 功能预告可领取红点 灵鹿预告前的预告
    self.show_notice_red_point = SmartData.New({show = false})
end
function FunOpen:OnInit()
    self.handle_level = RoleData.Instance:GetBaseData():KeysCare({"level"}, BindTool.Bind(self.OnRoleLevelChange, self))
    self.handle_task = TaskData.Instance:GetTaskList():Care(BindTool.Bind(self.OnTaskListChange, self))
end
function FunOpen:__delete()
    RoleData.Instance:GetBaseData():Uncare(self.handle_level)
    TaskData.Instance:GetTaskList():Uncare(self.handle_task)
    for key, value in pairs(self.handle_cares) do
        value:Uncare(key)
    end
    self.handle_cares = {}
    self.all_fun_checked = false
end
function FunOpen:OnUnloadGameLogic()
    self.opened_list = {}
    self.opened_mod_list = {}
    self.all_fun_checked = false
end
function FunOpen:checkConfig()
    self.fun_info_list = {}
    for _, value in ipairs(self.funopen_config) do
        self.fun_info_list[value.client_id] = value
    end
end
-- 针对功能开启的注册(主界面)
--【危险！！！】此函数会再次被调用时会直接覆盖掉前一次的函数注册，不出于必要不要乱用！！
function FunOpen:Register(modKey, func)
    -- 如果已开启不进行监听
    if modKey == Mod.RandActivity.FirstCharge then
        LogError("first register")
    end
    
    self.check_handle_list[modKey] = func
    -- self:UpdateMainIcon()
    if RoleData.Instance:GetRoleLevel() > 0 then
        func(self:CheckOpen(modKey))
    end
end

--【危险！！！】此函数注销掉所有的模块开启监听注册，不出于必要不要乱用！！
function FunOpen:UnRegister(modKey)
    if modKey then
        self.check_handle_list[modKey] = nil
    end
end

-- params = {
--     modKey = 
--     smData = 刷新源
-- }
function FunOpen:RegisterCondition(params)
    local handle = params.smData:Care(BindTool.Bind(self.UpdateFunc, self, params.modKey))
    self.handle_cares[handle] = params.smData
end
--【注意！！！】modKey应该与注册检查开关按钮的modKey一致如果有使用Register
--在 oninit里注册或者在标签上
--func 返回true or false
function FunOpen:SelfCondition(modKey, func)
    self.custom_condition[modKey] = func
end
function FunOpen:OnRoleLevelChange()
    -- LogError("人物等级变化")
    self:UpdateMainIcon()
end
--任务列表变化
function FunOpen:OnTaskListChange()
    if RoleData.Instance:GetRoleLevel() > 0 then
        self:CheckAllFunOpen()
        self:UpdateMainIcon()
    end
end
function FunOpen:CheckAllFunOpen()
    if self.all_fun_checked then
        return
    end
    local is_open
    local level = RoleData.Instance:GetRoleLevel()
    for _, v in pairs(self.funopen_config) do
        is_open = false
        if not is_open and v.level == 1 and TaskData.Instance:GetTaskIsCompleted(v.task) then
            is_open = true
        elseif not is_open and level >= v.level and TaskData.Instance:GetTaskIsCompleted(v.task) then
            is_open = true
        end
        local self_func = self.custom_condition[v.client_id]
        if is_open then
            if self_func and self_func() then
                self.opened_list[v.client_id] = 1   --标记已开启
            else
                if self_func == nil then
                    self.opened_list[v.client_id] = 1   --标记已开启
                end
            end
        end
    end
    self.all_fun_checked = true
end
function FunOpen:UpdateMainIcon()
    -- local cache = {}
    for modKey, func in pairs(self.check_handle_list) do
        local is_opened = self:CheckOpen(modKey)
        func(is_opened)
        --功能开启飘图标交由外部实现
        --[[ if is_opened then
            table.insert(cache, modKey)
        end ]]
    end
    -- 因切换账号问题去掉
    --[[ for _, modKey in ipairs(cache) do
        self.check_handle_list[modKey] = nil
    end ]]
end
function FunOpen:UpdateFunc(modKey)
    local func = self.check_handle_list[modKey]
    if func then
        local is_opened = self:CheckOpen(modKey)
        func(is_opened)
    end
end
--【注意！！！注册时如果时mod表那么其内部有一个解锁功能也就解锁，注册时是单个modkey即以单个为标准】
--内部
function FunOpen:CheckOpen(modKey)
    local is_opened = false
    if type(modKey) == "table" then
        if self.opened_mod_list[modKey] == 1 then
            return true
        end
        local func = self:GetFunIsOpenBySelfCondition(modKey)
        if func ~= nil then
            return func()
        end
        local value_list = {}
        for _,v in pairs(modKey) do
            table.insert(value_list, v)
        end
        table.sort(value_list,function (a,b)
            return a < b
        end)

        for _, value in pairs(value_list) do
            is_opened = self:GetFunIsOpened(value)
            if is_opened == true then
                break
            end
        end
        self.opened_mod_list[modKey] = is_opened and 1 or 0
    else
        is_opened = self:GetFunIsOpened(modKey)
    end
    --判断
    return is_opened
end

function FunOpen:GetFunIsOpened(modKey)
    -- 添加审核版本控制一些功能开启
    if IS_AUDIT_VERSION then
        if not GLOBAL_CONFIG:PkgInfo().is_youyan then
            if ModAuditClose[modKey] then
                return false
            else
                for k, _ in pairs(ModAuditClose) do
                    if type(k) == "table" then
                        if k[modKey] then
                            return false
                        else
                            for _,j in pairs(k) do
                                if j == modKey then
                                    return false
                                end
                            end
                        end
                    end
                end
            end
        else
            local function block_fun()
                if ModBlockRecharge[modKey] then
                    return false
                else
                    for k, _ in pairs(ModBlockRecharge) do
                        if type(k) == "table" then
                            if k[modKey] then
                                return false
                            else
                                for _,j in pairs(k) do
                                    if j == modKey then
                                        return false
                                    end
                                end
                            end
                        end
                    end
                end
            end

            if not GLOBAL_CONFIG:Flags().show_month_and_fortunebag then
                if block_fun() ~= nil then
                    return block_fun()
                end
            elseif modKey ~= Mod.RewardOpen and modKey ~= Mod.RewardOpen.Main and modKey ~= Mod.Reward.FortuneBag and modKey ~= Mod.Reward.MonthCard then
                if block_fun() ~= nil then
                    return block_fun()
                end
            end
        end
    end
    local func = self:GetFunIsOpenBySelfCondition(modKey)
    if func ~= nil then
        return func()
    end
    if self.opened_list[modKey] == 1 then
        return true
    end
    local info = self.fun_info_list[modKey]
    if info then
        local level = RoleData.Instance:GetRoleLevel()
        return self:CheckLevel(info, level)
    end
    --无配置
    if DEBUG_MODE then
        LogError("模块",modKey,"没有功能开启配置麻烦策划配置一下")
    end
    return true, ""
end
function FunOpen:GetFunOpenLevel(modKey)
    local info = self.fun_info_list[modKey]
    return info and info.level or 1
end
function FunOpen:CheckLevel(info, level)
    if level < info.level then
        return false, string.format(ErrorText[90], info.level), info.level
    end
    if info.task ~= 0 then
        return TaskData.Instance:GetTaskIsCompleted(info.task),"任务未完成"
    end
    --无配置
    return true
end
-- 自定义条件
function FunOpen:GetFunIsOpenBySelfCondition(modKey)
    return self.custom_condition[modKey]
end
--open_param : mod_key与1000求余得出来的index数字/ Mod.ParseKey转换出来的param_t
function FunOpen:IWantSelect(modTab, tabbar, open_param)
    local index = 1

    if open_param then
        if type(open_param) == "table" then --Mod.ParseKey转换出来的param_t
            if open_param.mod_key ~= nil then
                for i, value in ipairs(tabbar:GetData() or {}) do
                    if open_param.mod_key == value.open then
                        return i
                    end
                end
            end
            return 1
        else
            index = self:RealIndex(tabbar, open_param)
            return index
        end
    end

    local is_opened = false
    local temp = {}
    for _, value in pairs(modTab) do
        is_opened = self:GetFunIsOpened(value)
        table.insert(temp, {key = value, open = is_opened})
    end
    table.sort(temp, DataSorter.KeyLowerSorter("key"))
    for _, value in ipairs(temp) do
        if value.open == true then
            index = value.key % 1000
            break
        end
    end
    --找到了已开启的key
    index = self:RealIndex(tabbar, index)
    return index
end
function FunOpen:RealIndex(tabbar, index)
    if tabbar then
        for i, value in ipairs(tabbar:GetData() or {}) do
            if value.open then
                local key = value.open % 1000
                if index == key then
                    index = i
                    break
                end
            end
        end
    end
    return index
end

--判断是不是下一个开启的标签
function FunOpen:NextTabbar(modKey)
    for _, value in pairs(Mod) do
        if "table" == type(value) then
            for _, value2 in pairs(value) do
                if modKey == value2 then
                    --找出value中的下一个开启
                    return self:CheckNextOpen(value)
                end
            end
        end
    end
end
function FunOpen:FindMod(modKey)
    for _, value in pairs(Mod) do
        if "table" == type(value) then
            for _, v in pairs(value) do
                if modKey == v then
                    return value
                end
            end
        end
    end
end

function FunOpen:CheckNextOpen(group)
    local temp
    local is_opened
    for key, value in pairs(group) do
        is_opened = self:GetFunIsOpened(value)
        if is_opened == false then
            local info = self.fun_info_list[value]
            if temp == nil then
                temp = info
            end
            if info.level < temp.level then
                temp = info
            end
        end
    end
    return temp
end
function FunOpen:RecvFuncPreviewInfo(protocol)
    GuideData.Instance:SetNoticeInfo(protocol.notice_id_list)
end
-- 默认请求
function FunOpen:SendFuncPreviewClaimReward(notice_id,req_type)
    local protocol = GetProtocol(CSFuncPreviewClaimReward)
    protocol.req_type = req_type == nil and 1 or req_type
    protocol.notice_id = notice_id
    SendProtocol(protocol)
end

function FunOpen:CheckNextNotice()
    local level = RoleData.Instance:GetRoleLevel()
    local length = 0
    local ling_qi_notice
    for k,v in pairs(self.func_notice_sort) do 
        length = length + 1 
        if v.notice_id == FunOpen.ConstNotice.LingLu then
            ling_qi_notice = v
        end
    end
    -- 新增一个逻辑灵骑之前的如果没有可以领取的就显示灵骑
    self.show_notice_red_point.show = false
    local flag
    for i = 0, length -1 do
        local temp = self.func_notice_sort[i] or {}
        flag = GuideData.Instance:NoticeInfo(temp.notice_id)
        local t_flag = GuideData.Instance:NoticeGuideDone(temp.notice_id)
        if ( t_flag or flag <= FunPreNoticeEnum.UnLock) and level >= temp.start_level and temp.not_use == 0 then
            -- 有任务限制的任务完成也不显示
            if temp.task and temp.task > 0 then
                if not TaskData.Instance:GetTaskIsCompleted(temp.task) then
                    return temp
                end
            elseif temp.notice_id == FunOpen.ConstNotice.XingTian then--刑天预告24 第2天是否领取
                if ServerActivityData.Instance:GetLoginRewardFlag(2) >= 0 then
                    return temp
                end
            elseif temp.notice_id == FunOpen.ConstNotice.LingLu then
                if not  BicycleData.Instance:BicycleIsActiveById(1) then
                    return temp
                end
            else
                if ling_qi_notice and temp.sn < ling_qi_notice.sn and not BicycleData.Instance:BicycleIsActiveById(1) then
                    if flag == FunPreNoticeEnum.UnLock then
                        self.show_notice_red_point.show = true
                    end
                    return ling_qi_notice
                end
                return temp
            end
        end
    end
end

-- return 1:功能预告列表，2:预告类型：0默认，1是宠物
function FunOpen:GetNoticeList()
    local list = {}
    local temp = self:CheckNextNotice()
    local list_type = 0
    if temp ~= nil then
        table.insert(list,temp)
        --当检查到当前的展示是宠物模型时
        if temp.res_id and temp.res_id > 0 then
            list_type = 1
            return list, list_type
        end
    end
    if not ActivityData.Instance:IsNoEnoughLevel(ActType.Battlefield,true) then
        if ActivityData.IsOpen(ActType.Battlefield) then
            table.insert(list,1,{act_type = ActType.Battlefield})
        else
            table.insert(list,{act_type = ActType.Battlefield})
        end
    end
    if not ActivityData.Instance:IsNoEnoughLevel(ActType.GuildEmbattle, true) then
        if ActivityData.IsOpen(ActType.GuildEmbattle) then
            table.insert(list, 1, {act_type = ActType.GuildEmbattle})
        else
            table.insert(list, {act_type = ActType.GuildEmbattle})
        end
    end
    -- if AnecdoteHintData.Instance:IsOpening() then
    --     table.insert(list, 1, {modkey = Mod.Anecdote.Hint,name = Language.Anecdote.Hint.Title,icon = AnecdoteHintData.ActIconName})
    -- end
    if FunOpen.Instance:IsFunOpen(Mod.LunJian.Main) and LunJianData.Instance:CurSeason() > 0 then
        local insert_pos =  (LunJianData.Instance:SeasonStatus() ~= LunJianData.Status.Rest or LunJianData.Instance:SeasonStatus() ~= LunJianData.Invalid )and 1 or (#list + 1)
        table.insert(list, insert_pos, {modkey = Mod.LunJian.Signup,name = Language.LunJian.Title,icon = "a10044"})
    end

    -- if RewardOnlineGiftData.Instance:ShouldShowGift() then 
    --     local insert_pos = (#list + 1)
    --     table.insert(list, insert_pos, {modkey = Mod.RewardOnlineGift.Main,name = Language.RewardOnlineGift.Title,icon = "a10044"})
    -- end 

    return list,list_type
end

--@G根据modKey获取功能开启配置
function FunOpen:GetOpenData(modKey)
    return self.fun_info_list[modKey]
end

--@G功能是否开启 外部接口 这个不会弹出log
function FunOpen:IsFunOpen(modkey)
    if self:GetOpenData(modkey) then
        return self:GetFunIsOpened(modkey)
    end
    return true
end

--获取其他表
function FunOpen.GetConfog()
    return Config.funopen_auto.other[1]
end