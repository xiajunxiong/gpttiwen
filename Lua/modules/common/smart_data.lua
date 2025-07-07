local notifier = nil

SmartDataNotifier = SmartDataNotifier or BaseClass()

local function printlog(key, ...)
    if key == "num" then
        Debuger.LogErrorSD(...)
    end
end

function SmartDataNotifier:__init()
    notifier = self
    self.notify_funcs = {} --key:func value:change keys/old value
    Runner.Instance:AddRunObj(self, 15)
end

function SmartDataNotifier:__delete()
    self.notify_funcs = nil
    Runner.Instance:RemoveRunObj(self)
    notifier = nil
end

local notifying_func = nil

function SmartDataNotifier:Update()
    local n_f = self.notify_funcs
    self.notify_funcs = {}
    for func, keys in pairs(n_f) do
        -- notifying_func = func
        if keys == true then
            func()
        else
            func(keys)
        end
    end
    -- notifying_func = nil
end

--each func notify once each frame
function SmartDataNotifier:Notify(func, key, old_val)
    -- if notifying_func == func then
    -- Debuger.LogErrorSD("SmartDataNotifier WARNING!Set smart data in Flush func is not Surpport!!!key=%s,old_view===%s",
    -- 	tostring(key),tostring(old_val))
    -- end
    if old_val == nil then
        old_val = SmartData.NotiNil
    end
    local wait_noti_keys = self.notify_funcs[func]
    if wait_noti_keys ~= true then
        if wait_noti_keys == nil then
            if key == nil then
                wait_noti_keys = true
            else
                wait_noti_keys = {[key] = old_val}
            end
            self.notify_funcs[func] = wait_noti_keys
        else
            if key == nil then
                wait_noti_keys = true
            elseif wait_noti_keys[key] == nil then
                wait_noti_keys[key] = old_val
            end
        end
    end
end

function SmartDataNotifier:UnNotify(func)
    self.notify_funcs[func] = nil
end

---override pairs
local rawpairs = pairs

function pairs(tbl)
    if tbl == nil then
        return rawpairs(RO_EMTPY_TAB)
    end
    if not tbl._smart_data then
        return rawpairs(tbl)
    else
        return rawpairs(tbl._val)
    end
end

local rawipairs = ipairs

function ipairs(tbl)
    if tbl == nil then
        return rawipairs(RO_EMTPY_TAB)
    end
    if not tbl._smart_data then
        return rawipairs(tbl)
    else
        return rawipairs(tbl._val)
    end
end

local rawnext = next
function next(tbl, index)
    if tbl == nil then
        return rawnext(RO_EMTPY_TAB, index)
    end
    if not tbl._smart_data then
        return rawnext(tbl, index)
    else
        return rawnext(tbl._val, index)
    end
end

SmartData = SmartData or {}
SmartData.NotiNil = "__sm_nil__"

local _AK = "_AK"

function SmartData:Val()
    return self._val
end

function SmartData:Set(val)
    if self._val ~= val then
        if self._val ~= nil then
            for k, v in pairs(self._val) do
                if type(v) == "table" and v._smart_data then
                    v._parents[self] = nil
                end
            end
        end
        self._val = val
        if self._val ~= nil then
            for k, v in pairs(self._val) do
                if type(v) == "table" and v._smart_data then
                    v._parents = v._parents or {}
                    v._parents[self] = k
                end
            end
        end
        self:onValueChange(nil, nil)
    end
end

function SmartData:addListener(key, func, handle)
    local key_lists = self._listeners[key]
    if key_lists == nil then
        key_lists = {}
        self._listeners[key] = key_lists
    end
    key_lists[handle] = func
end

--func (val,key,tab)
--... keys
function SmartData:Care(func, ...)
    local arg = {...}
    return self:KeysCare(arg, func)
end

function SmartData:KeysCare(keys, func)
    local handle = {}
    if keys == nil or #keys == 0 then
        handle[_AK] = true
        self:addListener(_AK, func, handle)
    else
        for _, v in pairs(keys) do
            handle[v] = true
            self:addListener(v, func, handle)
        end
    end
    return handle
end

function SmartData:Uncare(handle)
    for k, _ in pairs(handle or {}) do
        local key_lists = self._listeners[k]
        if key_lists ~= nil then
            local noti_func = key_lists[handle]
            if noti_func ~= nil then
                notifier:UnNotify(noti_func)
                key_lists[handle] = nil
            end
        end
    end
end

function SmartData:Length()
    local len = 0
    for _, _ in pairs(self._val) do
        len = len + 1
    end
    return len
end

function SmartData:Insert(value, pos)
    if value._smart_data then
        value._parents = value._parents or {}
        value._parents[self] = pos or (#self._val + 1)
    end
    if pos then
        table.insert(self._val, pos, value)
    else
        table.insert(self._val, value)
    end
    self:onValueChange(nil, nil)
end

function SmartData:Remove(pos)
    if pos then
        table.remove(self._val, pos)
    else
        table.remove(self._val)
    end
    self:onValueChange(nil, nil)
end

function SmartData:Clear()
    self._val = {}
end

function SmartData:Notify()
    self:onValueChange(nil, nil)
end

SmartData.NotifyEnable = true

-- --设置只要赋值就通知的键值类型
-- function SmartData:NotifyWhatever(key)
-- 	self._notify_whatever[key] = true
-- end

function SmartData:onValueChange(key, old_val)
    if SmartData.NotifyEnable then
        if key ~= nil then
            local key_lists = self._listeners[key]
            if key_lists ~= nil then
                for _, v in pairs(key_lists) do
                    notifier:Notify(v, key, old_val)
                end
            end
            key_lists = self._listeners[_AK]
            if key_lists ~= nil then
                for _, v in pairs(key_lists) do
                    notifier:Notify(v, key, old_val)
                end
            end
        else --key is nil,all listener notify
            for _, key_lists in pairs(self._listeners) do
                for _, v in pairs(key_lists) do
                    notifier:Notify(v, key, old_val)
                end
            end
        end
    end

    --nofity parent tab
    if self._parent_noti and self._parents ~= nil then
        for p, k in pairs(self._parents) do
            -- Debuger.LogErrorSD("Key change!!,par===%s,key===%s,fc=%s",tostring(p),tostring(k),Time.frameCount)
            p:onValueChange(k, self._val)
        end
    end
end

function SmartData:TraceAll()
    self._trace_all = true
end

function SmartData:UntraceAll()
    self._trace_all = false
end

function SmartData:Trace(key)
    self._trace_list[key] = true
end

function SmartData:Untrace(key)
    self._trace_list[key] = nil
end

function SmartData:SetDirty(key)
    self:onValueChange(key)
end

local own_keys = {
    _parent_noti = true,
    _parents = true,
    _val = true,
    _trace_list = true,
    _trace_all = true
    -- _notify_whatever = true
}

SmartData.__newindex = function(tab, key, value)
    -- Debuger.LogErrorSD("key====%s,kt===%s,value===%s,vt===%s",
    -- 	tostring(key),type(key),tostring(value),type(value))
    if own_keys[key] then
        rawset(tab, key, value)
        return
    end

    local old_val = tab._val[key]
    if tab._trace_all or tab._trace_list[key] then
        -- LogError(key, "产生变化", old_val, "=>", value)
    end

    if old_val ~= value then --set
        tab._val[key] = value
        --change notify
        -- Debuger.LogErrorSD("Key change!!,key===%s,val===%s,fc=%s",tostring(key),tostring(value),Time.frameCount)
        if type(old_val) == "table" and old_val._smart_data then --remove from parent list
            old_val._parents[tab] = nil
        end
        if type(value) == "table" and value._smart_data then --set parent
            value._parents = value._parents or {}
            value._parents[tab] = key
        end
        tab:onValueChange(key, old_val)
    -- elseif tab._notify_whatever[key] then
    -- 	tab:onValueChange(key,old_val)
    end
end

SmartData.__index = function(tab, key)
    if own_keys[key] then
        return rawget(tab, key)
    end
    local re_val = SmartData[key]
    return re_val ~= nil and re_val or tab._val[key]
end

-- SmartData.__len = function(tab)
-- Debuger.LogErrorSD("!!!!!!!!!!!")
-- 	return #tab._val
-- end

function SmartData.New(value, nofity_parent)
    -- return value or {}
    local re_val = {}
    re_val._smart_data = true
    if nofity_parent ~= nil then
        re_val._parent_noti = nofity_parent
    else
        re_val._parent_noti = true
    end
    if value == nil then
        value = {}
    else
        for k, v in pairs(value) do
            if type(v) == "table" and v._smart_data then
                v._parents = v._parents or {}
                v._parents[re_val] = k
            end
        end
    end
    re_val._val = value
    re_val._listeners = {}
    re_val._trace_list = {}
    setmetatable(re_val, SmartData)
    return re_val
end

function SmartData.Check(tab)
    return tab._smart_data == true
end

function SmartData.GetVal(tab)
    if SmartData.Check(tab) then
        return tab:Val()
    else
        return tab
    end
end
