DayAndNight = {}
local GameDayAndNight = typeof('Game.DayAndNight')
function DayAndNight.Init()
    if DayAndNight.obj == nil then
        DayAndNight.obj = UnityGameObj.Find("DayAndNight")
    end
end
--1是夜晚，0是白天
function DayAndNight.Stop(value)
    -- local BindingFlags = require 'System.Reflection.BindingFlags'
    local func = tolua.getproperty(GameDayAndNight, 'LerpT')
    if DayAndNight.obj ~= nil then
        DayAndNight.obj:SetActive(false)
    else
        DayAndNight.Init()
        if DayAndNight.obj == nil then
            BuglyReport("获取变化组件失败")
            return
        end
        DayAndNight.obj:SetActive(false)
    end
    func:Set(DayAndNight.obj:GetComponent(GameDayAndNight), value, nil)
    func:Destroy()
    func = nil
end
function DayAndNight.Restore()
    if DayAndNight.obj ~= nil then
        DayAndNight.obj:SetActive(true)
    end
end

function DayAndNight.GetCurrTime()
    local t = TimeHelper.FormatDHMS(TimeHelper:GetServerTime())
    local time = t.hour * 60 * 60 + t.min * 60 + t.s
    local offset = 1800
    local tt = math.floor(time / offset)
    if tt % 2 == 0 then
        time = (tt + 1) * offset - time
    else
        time = time - (tt * offset)
    end
    return time / offset
end