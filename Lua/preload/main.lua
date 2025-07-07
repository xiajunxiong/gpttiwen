-- local breakSocketHandle,debugXpCall = require("LuaDebugjit")("localhost",7003)
-- local timer = Timer.New(function() breakSocketHandle() end, 1, -1, false)
-- timer:Start()

cjson = require "cjson"
require 'tolua.reflection'
require("preload/csharp_class_define")
tolua.loadassembly('Assembly-CSharp')
ScreenResolution = require("preload/screen_resolution")

G_IsRebooting = false

local main_data={
    state_machine = nil,
}
--UI底板大小1425*750

-- 打印带时间戳的调试信息
function DebugLog(msg)
    local timeStr = os.date("%H:%M:%S")
    UnityEngine.Debug.Log(string.format("[%s] [调试] %s", timeStr, msg))
end

-- 打印带时间戳的错误信息
function DebugError(msg)
    local timeStr = os.date("%H:%M:%S")
    UnityEngine.Debug.LogError(string.format("[%s] [错误] %s", timeStr, msg))
end

-- 修复客户端，重启应用后会删除所有cache文件夹里的文件
function MainCleanClientPreOperation(on_restore)
    DebugLog("显示客户端重置确认对话框")
    Nirvana.DialogManager.showConfirm(PreLanguage.ClientRestore,PreLanguage.ClientRestoreMsg,PreLanguage.Confirm,
        function ()
            DebugLog("用户确认重置客户端")
            if on_restore ~= nil then
                on_restore()
            end
            UnityPlayerPrefs.SetInt(PrefKeys.ClientRestoreFlag,1);
            GameUtil.StopGame()
        end,
        PreLanguage.Cancel, function () 
            DebugLog("用户取消重置客户端")
        end)
end

--delete all cache and restart
function MainCleanClientExeOperation()
    DebugLog("开始执行客户端清理操作")
    local pkg_version = UnityEngine.Application.version
    local pref_version = UnityPlayerPrefs.GetString(PrefKeys.InternalBundleVersion,"")
    DebugLog(string.format("包版本: %s, 偏好设置版本: %s", pkg_version, pref_version))
    
    if pref_version == "" or pref_version == nil then
        pref_version = pkg_version
        UnityPlayerPrefs.SetString(PrefKeys.InternalBundleVersion,pref_version)
        DebugLog(string.format("设置新的偏好版本: %s", pkg_version))
    end
    
    if pkg_version == pref_version and UnityPlayerPrefs.GetInt(PrefKeys.ClientRestoreFlag,0) == 0 then 
        DebugLog("版本匹配且无重置标志，跳过清理操作")
        return false 
    end

    --delete cache file
    DebugLog("开始删除缓存文件...")
    local files = System.IO.Directory.GetFiles(UnityEngine.Application.persistentDataPath)
    if files ~= nil then
        local deletedCount = 0
        for i = 0,files.Length - 1 do
            local fi = files[i]
            if not string.match(fi,"^.+output_log.txt$") then
                System.IO.File.Delete(fi)
                deletedCount = deletedCount + 1
            end        
        end
        DebugLog(string.format("已删除 %d 个文件", deletedCount))
    end
    
    if not IS_STANDALONE() then
        DebugLog("开始删除缓存目录...")
        local dirs = System.IO.Directory.GetDirectories(UnityEngine.Application.persistentDataPath)
        if dirs ~= nil then
            local deletedDirCount = 0
            for i = 0,dirs.Length - 1 do
                local di = dirs[i]
                if not string.match(di,"^.+ExternalLua$") then
                    System.IO.Directory.Delete(di,true)
                    deletedDirCount = deletedDirCount + 1
                end
            end
            DebugLog(string.format("已删除 %d 个目录", deletedDirCount))
        end
    end
    
    DebugLog("清除所有偏好设置")
    UnityPlayerPrefs.DeleteAll()
    UnityPlayerPrefs.SetInt(PrefKeys.ClientRestoreFlag,0)
    UnityPlayerPrefs.SetString(PrefKeys.InternalBundleVersion,pkg_version)
    DebugLog(string.format("设置客户端重置标志为0，内部版本号为: %s", pkg_version))
    
    DebugLog("重启游戏")
    RebootGame()
    return true
end

--主入口函数。从这里开始lua逻辑
function MainStart()
    DebugLog("开始执行主入口函数 MainStart")
    
    --init bugly
    DebugLog("设置后台加载优先级为4")
    UnityEngine.Application.backgroundLoadingPriority = 4
    
    if BuglyAgent ~= nil then
        DebugLog("初始化Bugly错误收集")
        BuglyAgent.ConfigDebugMode(false);
        -- if PLATFORM == PlatformType.Android then
        --     DebugLog("初始化Android平台Bugly，AppID: eeb07d7961")
        --     BuglyAgent.InitWithAppId("eeb07d7961")
        -- elseif PLATFORM == PlatformType.IOS then
        --     DebugLog("初始化iOS平台Bugly，AppID: c3a07271b0")
        --     BuglyAgent.InitWithAppId("c3a07271b0")
        -- end
        BuglyAgent.EnableExceptionHandler()
    end
    
    if IS_EDITOR or not IS_IOS() then
        print("logic start")
    end
    
    DebugLog("加载文件工具模块")
    require("preload/file_tool")
    
    DebugLog("检查是否需要执行客户端清理操作")
    if MainCleanClientExeOperation() == true then
        DebugLog("客户端清理操作已执行，退出MainStart")
        return
    end
    
    DebugLog("加载版本兼容标记工具")
    require("preload/version_tag")
    
    DebugLog("加载基础类")
    require("preload/baseclass")
    
    DebugLog("加载游戏状态机")
    require("preload/game_state_machine")
    
    DebugLog("加载预加载视图状态机")
    require("preload/preload_view_state_machine")
    
    DebugLog("加载Lua MonoBehaviour")
    require("preload/lua_mono_behaviour")
    
    DebugLog("加载预加载数据")
    require("preload/preload_data")
    
    DebugLog("加载预加载语言")
    require("preload/preload_language")
    
    DebugLog("加载网络请求模块")
    WebRequester =  require("preload/web_request")
    
    DebugLog("加载编辑器设置")
    require("preload/editor_settings")
    
    DebugLog("加载安装包信息")
    require("preload/install_package_info")
    
    DebugLog("加载渠道代理常量")
    require("preload/channel_agent_const")
    
    DebugLog("加载全局配置")
    require("preload/global_config")
    
    DebugLog("从SDK获取渠道ID")
    local plat_agent_id = PlatChannelAgent.GetAgentId()
    DebugLog(string.format("获取到的渠道ID: %s", plat_agent_id))
    
    DebugLog(string.format("加载渠道代理模块: %s_channel_agent", plat_agent_id))
    ChannelAgent = require("preload/"..plat_agent_id.."_channel_agent")
    
    DebugLog("加载探测器控制器")
    require("preload/prober_ctrl")
    
    DebugLog("注册Update函数到UpdateBeat")
    UpdateBeat:Add(Update)
    
    DebugLog("初始化渠道代理")
    ChannelAgent:Init()
    
    DebugLog("启动主探测器")
    MainProber:Start()
    
    local dev_mod = string.gsub(tostring(UnityEngine.SystemInfo.deviceModel)," ","_")
    if not IS_IOS() then
        DebugLog(string.format("记录主加载器开始，设备型号: %s", dev_mod))
        MainProber:Step(MainProber.STEP_MAIN_LOADER_BEG, "", "",dev_mod)
    end
    
    DebugLog("创建预加载数据实例")
    PreloadData.New()
    
    DebugLog("创建游戏状态机实例并运行")
    main_data.state_machine = GameStateMachine.New()
    main_data.state_machine:Run()
    
    DebugLog("创建预加载视图状态机实例并运行")
    local view_st_machine = PreloadViewStateMachine.New()
    view_st_machine:Run()
    
    DebugLog("主入口函数 MainStart 执行完成")
end

function Update()
    if WebRequester then
        WebRequester:Update()
    end

    if main_data.state_machine ~= nil then
        main_data.state_machine:Update()
    end
    
    if PreloadViewStateMachine.Instance ~= nil then
        PreloadViewStateMachine.Instance:Update()
    end

    if nil ~= MainProber then
        MainProber:Update()
    end
end

function MainStop()
    DebugLog("执行MainStop函数，移除Update回调")
    UpdateBeat:Remove(Update)
    
    if nil ~= MainProber then
        DebugLog("停止主探测器")
        MainProber:Stop()
        MainProber = nil
    end
end

function RebootGame()
    DebugLog("执行RebootGame函数，准备重启游戏")
    G_IsRebooting = true
    
    if ModulesManager ~= nil and ModulesManager.Instance ~= nil then
        DebugLog("通知模块管理器处理重启")
        ModulesManager.Instance:OnReboot()
    end
    
    if SceneMgr ~= nil then
        DebugLog("通知场景管理器处理重启")
        SceneMgr:OnReboot()
    end
    
    DebugLog("调用GameUtil.Reboot()重启游戏")
    GameUtil.Reboot()
end

function __G__TRACKBACK__(msg)
    local log = debug.traceback() .. "\n[LUA-ERROR] " .. msg
    DebugError(log)
    return msg
end

function StringIsEmpty(str)
    return str == "" or str == nil
end

function TryParseJson(json_str,error_log)
    local status, json_tab = xpcall(function() 
            return cjson.decode(json_str)
        end,
        function (err)
                if not StringIsEmpty(error_log) then
                    err = err .. "\n" .. error_log
                end
                __G__TRACKBACK__(err)
        end)
    return json_tab,status
end

DebugLog("开始执行Lua主脚本")
local status, msg = xpcall(MainStart, __G__TRACKBACK__)

if not status then
    DebugError(string.format("主入口函数执行失败: %s", msg))
end

LocalType = {                                       --本地化标记
    CN = "cn",                                      --国区
    TWIOS = "twios",                                --台区ios
    TWAND = "twand",                                --台区Android
    VNMIOS = "vnmios",                              --越南版ios
    VNMAND = "vnmand",                              --越南版Android
    KOR = "KOR"                                     --韩国版
}

function GetLocalType()
    local tempType = LocalType.CN
    DebugLog(string.format("获取本地化类型: %s", tempType))
    return tempType
end