--GM行为操作放到这个类
BehaviorGM = BehaviorGM or {}

--@G加载自定义函数字符串
function BehaviorGM.LoadCall(key,params)
    load(Format([[return function(param) %s end]],key))()(string.find(key,[[true]]) == nil and Split(params," ") or {params})
end

--一键活动开启 param1 1 开启 0 关闭 param2 活动类型 0 全部
function BehaviorGM.ActivityOneKeyOpen(param1,param2)
    local function Send(act_type,param)
        if param == 1 then
            if ActivityData.IsOpen(act_type)then
                GMCmdCtrl.Instance:SendGMCommand("activitynextstate",act_type)
            end
        else
            if not ActivityData.IsOpen(act_type)then
                GMCmdCtrl.Instance:SendGMCommand("activitynextstate",act_type)
            end
        end
    end
    if tonumber(param2) == 0 then
        for k,v in pairs(ACTIVITY_TYPE) do
            Send(v,tonumber(param1))
        end
    else
        local config = ActivityRandData.Instance.all_config_list[tonumber(param2)]
        for k,v in pairs(config or {}) do
            Send(v.act_type,tonumber(param1))
        end
    end
end

--服务端坐标转客户端
function BehaviorGM.PosToClient(x, y)
    local pos,need_samp=SceneCtrl.Instance:CurScene():PosToClint(x,y)
    if pos.y == 0 or need_samp == true then
        LogError("坐标不可走")
    end
    LogError(pos,"(y=0时不可走),samp=",need_samp,"samp=true时不可走")
end

--@G变量名 >> 驼峰命名法 驼峰命名 >> 变量名 首字母大写
function BehaviorGM.NameHump(name)
    local str = [[]]
    local split = string.split(name,[[_]])
    if string.find(name,[[ ]]) ~= nil then
        split = string.split(name,[[ ]])
    end
    if split and table.nums(split) > 1 then
        for i,v in ipairs(split) do
            local a = string.sub(v,1,1)
            local b = string.sub(v,2,-1)
            str = str .. string.upper(a) .. b
        end
    else
        for i=1,string.len(name)do
            local a = string.sub(name,i,i)
            if i > 1 and string.byte(a) < string.byte([[a]]) then
                str = str .. "_"
            end
            str = str .. string.lower(a)
        end
    end
    UnityEngine.GUIUtility.systemCopyBuffer = str
    LogError(str)
end

function BehaviorGM.OpenActView(param1, param2)
    local act_type = tonumber(param1)
    if act_type == 0 then
        return
    end
    local function func_open()
        if ActivityRandData.Instance:OnClickHandle(act_type) then
            return
        end
        ActivityRandData.SwitchView(act_type, true)
    end
    if ActivityData.IsOpen(act_type) then
        func_open()
    elseif tonumber(param2) == 1 then
        GMCmdCtrl.Instance:SendGMCommand("activitynextstate", act_type)
        TimeHelper:AddDelayTimer(func_open, 1)
    else
        LogError(act_type .. "活动未开启，无法打开界面")
    end
end

function BehaviorGM.SuperSkillBook()
    for i = 24327, 24360 do
        GMCmdCtrl.Instance:SendGMCommand("additem", i .. " " .. 10 .. " 0")
    end
end

local test_agentid = {
    ["dev"] = true,
    ["ml1"] = true,
    ["ka1"] = true,
    ["iev"] = true,
}
function BehaviorGM.ClientSpeedRun(speed)
    --没有sdk且不是我们的测试渠道号，则不应用速度设置
    if PlatChannelAgent.GetAgentId() ~= "dev" and 
        test_agentid[GLOBAL_CONFIG:PkgInfo().agent_id] ~= true then
        return
    end
    GMCmdCtrl.Instance:SendGMCommand("nocheckmove","1")
    TimeHelper:SetTimeScale(tonumber(speed))
    Time.timeScale = tonumber(speed)
end

--测试代码接口
function BehaviorGM.TestFunction(param1, param2, param3)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_HALLOWEEN_PRAY, HalloweenPrayCtrl.ReqType.Info)
end