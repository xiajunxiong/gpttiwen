-- 提醒
RemindCtrl = RemindCtrl or BaseClass(BaseCtrl)
function RemindCtrl:__init()
    if RemindCtrl.Instance ~= nil then
        ErrorLog("[RemindCtrl] Attemp to create a singleton twice !")
    end
    RemindCtrl.Instance = self
    self.remind = Remind.New()
end

function RemindCtrl:__delete()
    RemindCtrl.Instance = nil
end

function RemindCtrl:Remind()
    return self.remind
end
function RemindCtrl:OnUnloadGameLogic()
    self.remind:Clear()
end

RemindVo = RemindVo or BaseClass()
function RemindVo:__init()
    self.modkey = 0
    self.check_func = nil
    self.num = 0
    self.is_dirty = false
    self.handle = nil
    self.origin = nil
end

RemindGroupVo = RemindGroupVo or BaseClass()
function RemindGroupVo:__init()
    self.groud_id = nil
    self.update_func = nil
    self.handle = nil
    self.nums = {}  --保存改id下的所有数量(key=check_func)
end
-- 一遍全检 其余统计
function RemindGroupVo:Num()
    local num = 0
    for key, value in pairs(self.nums) do
        num = num + value
    end
    return num
end

-- 在ctrl中进行注册和销毁
-- 减少检查函数的调用 判断源数据是否变化如果变化就通知如果没变化就不通知
Remind = Remind or BaseClass()

function Remind:__init()
    if Remind.Instance ~= nil then
        ErrorLog("[Remind] Attemp to create a singleton twice !")
    end
    Remind.Instance = self
    self.remind_vo_list = {}
    self.group_vo_list = {}
    self.remind_frame_list = {}
    -- 对已存在modlist进行一个新cache
    self.KeyToGroup = {}
    for k, v in pairs(Mod) do
        if type(v) == "table" then
            for key, value in pairs(v) do
                self.KeyToGroup[value] = v
            end
        end
    end
    self.act_group = {
        [Mod.MayDay] = "yuanqiqingdian",
    }
end
function Remind:Clear()
    --[[ for key, value in pairs(self.remind_vo_list) do
        for key2, value2 in pairs(value) do
            value2.num = 0
        end
    end ]]
    for _, value in pairs(self.group_vo_list) do
        for _, value2 in pairs(value) do
            value2.nums = {}
            -- value2.update_func()--这里看情况吧，这里是切换的时候作清除用的
            value2.num = nil
        end
    end
end
-- modkey==remindid(使用modkey做唯一标识)
-- origin==smdata(刷新源)
-- func==返回0或者1的方法
-- ...监听的key
-- 原则上一个key对应一个结果(是你监听源的意图而不是为了返回所有红点的结果)
function Remind:Register(modkey, origin, check_func, ...)
    if self.remind_vo_list[modkey] == nil then
        self.remind_vo_list[modkey] = {}
        self.remind_frame_list[modkey] = {}
    end
    local vo = RemindVo.New()
    local handle = origin:Care(BindTool.Bind(self.UpdataRemind, self, vo), ...)
    vo.modkey = modkey
    vo.origin = origin
    vo.handle = handle  --这里不是很好
    vo.check_func = check_func

    self.remind_vo_list[modkey][handle] = vo
    self.remind_frame_list[modkey][handle] = Time.frameCount
    return handle
end

-- group_id (使用modkey的表 例如：Mod.Workshop)
-- func(传入显隐红点的方法)
function Remind:RegisterGroup(group_id, func, init_call)
    if self.group_vo_list[group_id] == nil then
        self.group_vo_list[group_id] = {}
    end
    local vo = RemindGroupVo.New()
    local handle = vo
    vo.groud_id = group_id
    vo.update_func = func
    self.group_vo_list[group_id][handle] = vo
    if init_call then
        func()
    end
    return handle
end

function Remind:UnRegister(modkey, handle)
    if handle then
        local vo = self.remind_vo_list[modkey][handle]
        vo.origin:Uncare(handle)

        local group = self.KeyToGroup[modkey]
        if group and self.group_vo_list[group] then
            for _, v in pairs(self.group_vo_list[group]) do
                v.nums[vo.check_func] = nil--这里待定
            end
        end
        self.remind_vo_list[modkey][handle] = nil
        self.remind_frame_list[modkey][handle] = nil
        return
    end
    if modkey == nil then
        return
    end
    if self.remind_vo_list[modkey] == nil then
        return
    end
    for k, vo in pairs(self.remind_vo_list[modkey]) do
        vo.origin:Uncare(k)
    end
    local group = self.KeyToGroup[modkey]
    if group and self.group_vo_list[group] then
        for _, v in pairs(self.group_vo_list[group]) do
            v.nums = {}
            v.num = nil
        end
    end
    if table.nums(self.remind_vo_list[modkey]) == 0 then
        self.remind_vo_list[modkey] = nil
        self.remind_frame_list[modkey] = nil
    end
end

function Remind:UnRegisterGroup(group_id, handle)
    if handle and self.group_vo_list[group_id] then
        self.group_vo_list[group_id][handle] = nil
        return
    end
    if group_id == nil then
        return
    end
    if self.group_vo_list[group_id] == nil then
        return
    end
    if table.nums(self.group_vo_list[group_id]) == 0 then
        self.group_vo_list[group_id] = nil
    end
end

--根据handle销毁红点监听 group_id的数据容易变化 导致销毁错误的监听
function Remind:UnRegisterHandle(handle)
    if handle ~= nil then
        self:UnRegisterGroup(handle.group_id,handle)
    end
end

-- 单个检查方法减少多次调用
function Remind:UpdataRemind(vo)
    --通知group做变化
    local frameCount = self.remind_frame_list[vo.modkey][vo.handle]
    if Time.frameCount - frameCount > 5 then--减少重复调用
        self.remind_frame_list[vo.modkey][vo.handle] = Time.frameCount -- 这里记录时间
        TimeHelper:AddDelayFrameTimer(BindTool.Bind(self.CheckRemind, self, vo), 10)--延迟执行(协议接收时间问题)
    end
end
-- 这里有一个很恶心的问题就是 group的注册会比较晚
function Remind:CheckRemind(vo)
    local num = 0
    if self:IsFunOpen(vo.modkey) then
        num = vo.check_func()
    end
    vo.num = num
    local group = self.KeyToGroup[vo.modkey]
    if group and self.group_vo_list[group] then
        for _, v in pairs(self.group_vo_list[group]) do
            --这里忽略key的问题
            if v.nums[vo.check_func] ~= num then
                v.nums[vo.check_func] = num
                v.update_func()
            end
        end
    end
    local tabgroup = TabMod(vo.modkey)
    local group_vo_list = self.group_vo_list[tabgroup]
    if tabgroup and group_vo_list then
        for _, group_vo in pairs(group_vo_list) do
            if group_vo.nums[vo.check_func] == nil or group_vo.nums[vo.check_func] ~= num then
                group_vo.nums[vo.check_func] = num
                group_vo.update_func()
            end
        end
    end
end

-- 获取group的红点总数 需要进行一次统一检查
function Remind:GetGroupNum(group_id)
    local num = 0
    if group_id == nil then
        return num
    end
    if self.group_vo_list[group_id] == nil then
        return num
    end
    local vo = next(self.group_vo_list[group_id])
    if vo == nil then return num end
    if self.act_group ~= nil and self.act_group[group_id] ~= nil then
        for _, v in pairs(Config.activity_main_auto[self.act_group[group_id]]) do
            if self.remind_vo_list[v.mod_key] then
                num = num + self:GetRemindTotalNum(v.mod_key, vo)
            end
        end
        return num
    end
    if vo.num == nil then
        for _, v in pairs(group_id) do
            if self.remind_vo_list[v] then
                num = num + self:GetRemindTotalNum(v, vo)
            end
        end
        vo.num = num
        --这里要将handle存起来
        return vo.num
    end
    return vo:Num()
end

-- 获取单个key的总数
function Remind:GetRemindNum(modkey)
    local num = 0
    if modkey == nil then
        return num
    end
    if self.remind_vo_list[modkey] then
        for _, vo in pairs(self.remind_vo_list[modkey]) do
            num = num + vo.num
        end
    end
    return num
end
-- 对检查方法进行一遍调用
function Remind:GetRemindTotalNum(modkey, group_vo)
    local num = 0
    if modkey == nil then
        return num
    end
    if self.remind_vo_list[modkey] then
        local temp_func = nil
        for _, vo in pairs(self.remind_vo_list[modkey]) do
            if temp_func == nil and vo.check_func ~= nil then
                if self:IsFunOpen(vo.modkey) then
                    group_vo.nums[vo.check_func] = vo.check_func() or 0
                else
                    group_vo.nums[vo.check_func] = 0
                end
                num = num + group_vo.nums[vo.check_func]
                temp_func = vo.check_func
            end
            if temp_func ~= vo.check_func and vo.check_func ~= nil then
                if self:IsFunOpen(vo.modkey) then
                    group_vo.nums[vo.check_func] = vo.check_func() or 0
                else
                    group_vo.nums[vo.check_func] = 0
                end
                num = num + group_vo.nums[vo.check_func]
                temp_func = vo.check_func
            end
        end
    end
    return num
end

function Remind:IsFunOpen(modkey)
    if FunOpen.Instance:GetOpenData(modkey) then
        return FunOpen.Instance:GetFunIsOpened(modkey)
    end
    return true
end
--使用该方法对提醒组注册红点显示的函数调用一次
--例如刷新主界面红点的显示
function Remind:DoRemind(group_id)
    local group_vo_list = self.group_vo_list[group_id]
    if group_vo_list == nil then
        return
    end
    for _, group_vo in pairs(group_vo_list) do
        group_vo.num = nil
        if group_vo.update_func then
            group_vo.update_func()
        end
    end
end