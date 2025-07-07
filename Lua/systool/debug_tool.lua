local UnityDebug = UnityEngine.Debug

Debuger = {}

------------log打印函数组，后缀带SD表示会打印栈信息-----------
function LogD(...)
    if DEBUG_MODE then
        LogError(...)
    end
end
function BuglyReport(...)
    local log_str = ""
    for k, v in pairs({...}) do
        if type(v) == "table" then
            log_str = log_str .. Debuger.TableToString(v)
        else
            log_str = log_str .. tostring(v)
        end
        log_str = log_str .. "   "
    end
    Debuger.LogErrorSD(string.format("BuglyReportqQuickFind %s", log_str))
end
function LogDG(...)
    if DEBUG_MODE then
        local log_str = ""
        for k, v in pairs({...}) do
            if type(v) == "table" then
                log_str = log_str .. Debuger.TableToString(v)
            else
                log_str = log_str .. tostring(v)
            end
            log_str = log_str .. "   "
        end
        Debuger.LogErrorSD(string.format( "<color=#23F1AB>%s</color>", log_str))
    end
end

function LogError(...)
    local log_str = ""
    for k, v in pairs({...}) do
        if type(v) == "table" then
            log_str = log_str .. Debuger.TableToString(v)
        else
            log_str = log_str .. tostring(v)
        end
        log_str = log_str .. "   "
    end
    Debuger.LogErrorSD(log_str)
end

local function L(...)
    local log_str = ""
    for k, v in pairs({...}) do
        if type(v) == "table" then
            log_str = log_str .. Debuger.TableToString(v)
        else
            log_str = log_str .. tostring(v)
        end
        log_str = log_str .. "   "
    end
    Debuger.LogErrorSD(log_str)
end

function LogErrorHL(name, ...)
    if true then
		return
	end
    L(string.format("*******************%s*******************", name), ...)
end

--为了减少卡顿，所有战斗的log应该使用这里
function LogErrorBT(...)
    if true then
		return
    end
    L(...)
end

function Debuger.Log(str, ...)
    UnityDebug.Log(string.format(str, ...))
end

function Debuger.LogSD(str, ...)
    UnityDebug.Log(debug.traceback(string.format(str, ...)))
end

function Debuger.LogWarning(str, ...)
    UnityDebug.LogWarning(string.format(str, ...))
end

function Debuger.LogWarningSD(str, ...)
    UnityDebug.LogWarning(debug.traceback(string.format(str, ...)))
end

function Debuger.LogError(str, ...)
    UnityDebug.LogError(string.format(str, ...))
end

function Debuger.LogErrorSD(str, ...)
    if ... == nil then
        UnityDebug.LogError(debug.traceback(str))
    else
        UnityDebug.LogError(debug.traceback(string.format(str, ...)))
    end
end

--@G打印遍历Table函数
function LogTable(table,...)
    LogError(Serialize(table),...)
end

--@G打印遍历Table函数 也可做为 LogError 带标记颜色 和 时间戳
function LOG(tab,...)
    if not DEBUG_MODE then
        return
    end
    local str = type(tab) == [[table]] and Serialize(tab) or tab
    if ... == nil then
        LogError(str)
    else
        if type(tab) == [[table]] then
            local list = {...}
            LogError(ColorStr(list[1],COLORSTR.Green7),str,...)
        else
            LogError(ColorStr(str,COLORSTR.Green7),ColorStr(os.date("%X",TimeManager:GetServerTime())),...)
        end
    end
end

function Debuger.TableToString(tab, depth ,convented_tabs)
    if tab == nil then
        return "[nil]"
    end
    if convented_tabs ~= nil then
        if convented_tabs[tab] == true then
            return string.format("[ShowedTable:%s]",tostring(tab))
        end
    else
        convented_tabs = {}
    end
    convented_tabs[tab] = true
    if depth == nil then
        depth = 0
    end
    local depth_str = ""
    for i = 1, depth do
        depth_str = depth_str .. "  "
    end
    local tab_str = depth_str .. tostring(tab) .. "{\n"
    for k, v in pairs(tab) do
        if k ~= "_class_type" then
            if type(v) ~= "function" then
                if type(v) ~= "table" then
                    tab_str = string.format("%s%s[%s] = [%s]\n", tab_str, depth_str, tostring(k), tostring(v))
                else
                    local tt_str
                    if v["chs"] ~= nil then
                        tt_str = tostring(v)
                    else
                        tt_str = Debuger.TableToString(v, depth + 1,convented_tabs)
                    end
                    tab_str = string.format("%s%s[%s] = \n%s\n", tab_str, depth_str, tostring(k), tt_str)
                end
            end
        end
    end
    tab_str = tab_str .. depth_str .. "}"
    return tab_str
end

function TableToString(tab,level,convented_tabs)
	if type(tab) ~= "table" then
		return
	end
	if convented_tabs ~= nil then--重复嵌套问题
        if convented_tabs[tab] == true then
            return "[0],"
        end
    else
        convented_tabs = {}
    end
	convented_tabs[tab] = true

	if level == nil then
		level = 0
	end
	local level_str = ""
	for i = 1, level do
		level_str = level_str .. "\t"
	end
	local res = level_str .. "{\n"
	for key, value in pairs(tab) do
		if type(value) ~= "table" then
			res = string.format("%s%s[%s] = [%s],\n", res, level_str, tostring(key), tostring(value))
		else
			local tt_str = TableToString(value, level + 1, convented_tabs)
			res = string.format("%s%s[%s] = \n%s\n", res, level_str, tostring(key), tt_str)
		end
	end
	res = res .. level_str .."},"
	return res
end

--隐藏打印显示
local serialize_hide_tab = {
    ["_class_type"] = true,
    ["GetClassType"] = true,
    ["DeleteMe"] = true,
    ["__delete"] = true,
    ["__init"] = true,
    ["super"] = true,
    ["New"] = true
}

function Serialize(tab, level, strStart, strEnd)
	if tab == nil then 
		return "table is nil " 
	end
	if type(tab) == "string" then
		return tostring(tab)
	end
	if type(tab) == "table" and not next(tab)then
		return "{},"
	end
	local rep = string.rep
	strStart = strStart or "\""
	strEnd = strEnd or "\""
	local tab_str = level == nil and tostring(tab) .. " ".. os.date("%X",TimeManager:GetServerTime()) or ""
	level = level or 0
	local str = tab_str .. "\n" .. rep("\t", level)  .. "{\n"
	for k,v in pairs(tab) do
		if serialize_hide_tab[k] == nil then
			local kIsStr = 0
			if type(k) == "string" then
				kIsStr = 1
			end
			if type(v) == "table" then
				str = str .. rep("\t", level + 1) .. "[" .. rep(strStart, kIsStr) .. k .. rep(strEnd, kIsStr) .. "]" .. " = " .. Serialize(v, level + 1) .. "\n"
			else
				local vIsStr = 0
				if type(v) == "string" then
					vIsStr = 1
				end
				if type(v) == "function" then
					vIsStr = 1
					v = ColorStr(tostring(v),COLORSTR.Green10)
				end
				if type(v) == "boolean" then
					vIsStr = 1
					v = tostring(v)
				end
				if type(v) == "userdata" then
					vIsStr = 1
					v = "userdata"
				end
				str = str .. rep("\t", level + 1) .. "[" .. rep(strStart, kIsStr) .. k .. rep(strEnd, kIsStr) .. "]" .. " = " .. rep(strStart, vIsStr) .. v .. rep(strEnd, vIsStr) .. ",\n"
			end
		else
			if k == "_class_type" then
				str = str .. rep("\t", level + 1).. "[\"ClassName\"]" .. " = " .. ColorStr(GetClassName(tab),COLORSTR.Red10) .. "\n"
			end
		end
	end
	str = str .. rep("\t", level) .. "},"
	return str
end