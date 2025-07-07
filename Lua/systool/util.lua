string.split = function(s, p)
    local rt = {}
    string.gsub(s, '[^'..p..']+', function(w) table.insert(rt, w) end )
    return rt
end

string.splitzh = function(s)
    local len = string.len(s)
    local rt = {}
    for i = 1, len / 3 do
        table.insert(rt, string.sub(s, 1 + (i - 1) * 3, i * 3))
    end
    return rt
end

string.joint = function(i, s, m)
    local n = ""
    if i == 1 then
        n = n .. s
    else
        n = m .. n .. s
    end
    return n
end

--上面那个连接不要再使用了
--t是从1开始的连续table(限定字符串和数字)
string.concat = function(t, m)
    return table.concat(t, m)
end

table.nums = function(t)
    if t == nil then
        return 0
    end
    local i = 0
    for k, v in pairs(t) do
        i = i + 1
    end
    return i
end

table.rearrange = function(t)
    local n = {}
    local i = 0
    for _, v in pairs(t) do
        n[i + 1] = v
        i = i + 1
    end
    return n
end

table.rearrange2 = function(t)
    local n = {}
    for _, v in pairs(t) do
        table.insert(n, v)
    end
    return n
end

-- 奇偶取偶
math.roundtoint = function(n)
    local n1 = math.floor(n)
    local n2 = math.ceil(n)
    if n1 % 2 == 0 then
        return n1
    end
    if n2 % 2 == 0 then
        return n2
    end
    return n1
end

--@G缓存列表
function CacheList(self, key, param, func)
    if self[key] == nil then
        self[key] = {}
    end
    if self[key][param] then
        return self[key][param]
    else
        self[key][param] = func()
        return self[key][param]
    end
end

--@G返回一个以key为键值的表 仅转换
function KeyList(param, key)
    local list = {}
    for _, v in pairs(param) do
        list[v[key]] = v
    end
    return list
end

--@Glist连接多个表param
function AndList(list, ...)
    local param = {...}
    for _, v in ipairs(param) do
        local num = #list
        for i = 1, #v do
            list[num + i] = v[i]
        end
    end
    return list
end

--@G配置 表分组
function TabGroup(config, ...)
    local list,keys = {},{...}
    for k, v in pairs(config) do
        local tab = list
        for i, key in ipairs(keys) do
            if tab[v[key]] == nil then
                tab[v[key]] = {}
            end
            if i == #keys then
                if #keys == 1 then
                    table.insert(tab[v[key]],v)
                else
                    tab[v[key]] = v
                end
            else
                tab = tab[v[key]]
            end
        end
    end
    return list
end

--@G转换百分值
function Percent(value, retain, nosign)
    if math.floor(value) < value then
        return Format("%." .. (retain or 1) .. "f" .. (nosign and "" or "%%"), value / 10)
    else
        return Format("%s" .. (nosign and "" or "%%"), value / 10)
    end
end

--@G是否是一个空值 数字 == 0 字符串 == “” 表 == {}
function IsEmpty(value)
    if value == nil then
        return true
    elseif type(value) == [[table]] then
        return next(value) == nil or table.nums(value) == 0
    end
    return value == "" or value == 0
end

--@G拷贝一个表[慎用！！！]
function TableCopy(t, n)
    return DataHelper.TableCopy(t, n)
end

--拷贝一个配置表对象，元表也会一并被拷贝[慎用！！！！！]
function CfgTableCopy(t)
    local ct = TableCopy(t)
    local ct_meta = getmetatable(t)
    if ct_meta then
        setmetatable(ct,ct_meta)
    end
    return ct
end

--@G优化string.format接口
function Format(fmt, ...)
    return string.format(fmt, ...)
end

--@G拆分
function Split(content, txt)
    return tonumber(content) and {content} or string.split(tostring(content), tostring(txt))
end

--@G颜色
function ColorStr(str, color)
    return string.format("<color=#%s>%s</color>", color, str)
end

--@G过值阀门
function Valve(value, param)
    return value > 0 and (value >= param and param or value) or 0
end

--@G是否在某个值范围
function IsRange(value, min, max)
    return value >= min and value <= max
end

--@G得到负数
function Negative(value)
    return value < 0 and -1 or 1
end

--@G获取界面类
function GetView(view_name)
    return _G[view_name]
end

--@G获取界面类
function GetMod(mod)
    if not StringIsEmpty(mod) then
        local data = string.split(mod,".")
        return _G[data[1]][data[2]]
    end
    return nil
end
--@G获取界面类
function GetModOpen(mod)
    if not StringIsEmpty(mod) then
        local data = string.split(mod,".")
        if #data >= 3 then
            return _G[data[1]][data[2]][data[3]]
        else
            return _G[data[1]][data[2]]
        end
    end
    return nil
end

--@G获取一个gameObct激活状态
function GetActive(obj)
    return obj.activeSelf
end

--@G本地化存取 int
function PrefsInt(key, value)
    if value then
        UnityPlayerPrefs.SetInt(key, value)
    else
        return UnityPlayerPrefs.GetInt(key)
    end
end

--@G本地化存取 string
function PrefsString(key, value)
    if value then
        UnityPlayerPrefs.SetString(key, value)
    else
        return UnityPlayerPrefs.GetString(key)
    end
end

--@G延迟函数 delay_time 秒 销毁计时器用TimeHelper:CancelTimer(handle) OnDestroy()
function Invoke(func, delay_time)
    return TimeHelper:AddDelayTimer(func, delay_time or 1)
end

--@G刷新函数 销毁计时器用TimeHelper:CancelTimer(handle)
function UpdateTime(update_func, complete_func, total_time, interval, init_call)
	return TimeHelper:AddCountDownTT(update_func, complete_func or function()end, total_time or 1, interval or 1, init_call)
end

--@G创建系统计时器 销毁用 Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
function RunUntilTrue(func, init_call)
    return Runner.Instance:RunUntilTrue(func, init_call)
end

--@G注册红点监听 func(num) num = Remind.Instance:GetGroupNum()
function RegisterRemind(group_id, func, init_call)
    return Remind.Instance:RegisterGroup(tonumber(group_id) and TabMod(group_id) or group_id,function()
        func(Remind.Instance:GetGroupNum(tonumber(group_id) and TabMod(group_id) or group_id))
    end, init_call)
end

--@G销毁红点监听
function UnRegisterRemind(group_id, handle)
    Remind.Instance:UnRegisterGroup(tonumber(group_id) and TabMod(group_id) or group_id, handle)
end

function FinalList(list)
    return list[#list]
end

function TableIsEmpty(t)
    return t == nil or next(t) == nil
end

-- 贝塞尔曲线
-- num == 0 返回起点位置 num == 1返回终点位置
function FormatCurve(pos_list, num)
    local len = #pos_list
    if num >= 1 then
        return pos_list[len]
    end
    if len == 1 or num <= 0 then
        return pos_list[1]
    end
    local new_pos_list = {}
    for i = 1, len - 1 do
        local next_pos = pos_list[i + 1]
        local cur_pos = pos_list[i]
        local x = cur_pos.x + (next_pos.x - cur_pos.x) * num
        local y = cur_pos.y + (next_pos.y - cur_pos.y) * num
        local z = 0
        if cur_pos.z then
            z = cur_pos.z + (next_pos.z - cur_pos.z) * num
        end
        table.insert(new_pos_list, Vector3.New(x,y,z))
    end
    return FormatCurve(new_pos_list, num)
end

-- 数字转成表
function NumToTable(num)
    local t = {}
    while num ~= nil do
        local value = num % 10
        table.insert(t, 1,value)
        num = math.floor(num / 10)
        if num == 0 then
            num = nil
        end
    end
    return t
end

--只读型空tab，不可写
RO_EMTPY_TAB = {}
setmetatable(RO_EMTPY_TAB,{
    __newindex = function  (tab, key, value)
        LogError("ReadonlyTab 不能写入任何值！",tab,key,value)
    end
})