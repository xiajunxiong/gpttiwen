
PlatformType = {
    Unknow = 0,
    Android = 1,
    IOS = 2,
    WebGL = 3,
    WeChatMini = 4,
    Standalone = 5,
}

UnityApp = UnityEngine.Application
UnityObj = UnityEngine.Object
UnityGameObj = UnityEngine.GameObject
UnityUI = UnityEngine.UI
UnityAnim = UnityEngine.Animator
UnityPlayerPrefs = UnityEngine.PlayerPrefs
UnityAI = UnityEngine.AI
UnityProfiler = UnityEngine.Profiling.Profiler
UnityQualitySettings = UnityEngine.QualitySettings
RectOffset = UnityEngine.RectOffset
UnityCamera = UnityEngine.Camera
TimeEvent = Game.TimeEvent
SIMULATE_ASSETBUNDLE = true--Game.RunTimeDetails.SimulateAssetBundle -- 是否是模拟Editor环境-- 是否是模拟Editor环境
IS_EDITOR = true--Game.RunTimeDetails.IsEditor --是否是模拟Editor环境

GameUtil = Game.Util

PlatChannelAgent = Nirvana.ChannelAgent
BehaviourType = Nirvana.BehaviourType

DEBUG_MODE = IS_EDITOR
IS_AUDIT_VERSION = false       --审核版本开关
IS_AUDIT_VERSION_LOCAL = false  --本地测试审核版本开关

--针对刘海屏的判断
IS_IPHONEX = UnityEngine.SystemInfo.deviceModel == "iPhone10,3" or UnityEngine.SystemInfo.deviceModel == "iPhone10,6"
    or UnityEngine.SystemInfo.deviceModel == "iPhone11,2" or UnityEngine.SystemInfo.deviceModel == "iPhone11,4"
    or UnityEngine.SystemInfo.deviceModel == "iPhone11,6" or UnityEngine.SystemInfo.deviceModel == "iPhone11,8"
    or UnityEngine.SystemInfo.deviceModel == "iPhone12,1" or UnityEngine.SystemInfo.deviceModel == "iPhone12,3"
    or UnityEngine.SystemInfo.deviceModel == "iPhone12,5" or UnityEngine.SystemInfo.deviceModel == "iPhone12,8"
    or UnityEngine.SystemInfo.deviceModel == "iPhone13,1" or UnityEngine.SystemInfo.deviceModel == "iPhone13,2"
    or UnityEngine.SystemInfo.deviceModel == "iPhone13,3" or UnityEngine.SystemInfo.deviceModel == "iPhone13,4"

--版本兼容
xpcall(function ()
    PLATFORM = Game.RunTimeDetails.Platform
end, 
function ()
    PLATFORM = PlatformType.Unknow
end)

function IS_IOS()
    return PLATFORM == PlatformType.IOS
end

function IS_WEBGL()
    if VersionTag.HasTag(VersionTag.WebGLRTDFlags) then
        return Game.RunTimeDetails.IsWebGL;
    else
        return false
    end
end
--在VersionTag被require前调用此接口会报错
function IS_MICRO_PLAYER()
    if VersionTag.HasTag(VersionTag.WebGLRTDFlags) then
        return Game.RunTimeDetails.IsMicroPlayer
    else
        return false
    end
end
--web平台不使用我们自己逻辑层的资源缓存
function HAS_RESCACHE()
    return not IS_WEBGL()
end

--PC平台包环境(非unity)
function IS_STANDALONE()
    return not IS_EDITOR and PLATFORM == PlatformType.Standalone
end
--PC平台包主界面图标倍率（1为原大小，通过控制localScale实现）
STANDALONE_UI_VECTOR3 = Vector3.New(0.8, 0.8, 0.8)

if IS_IOS() and not IS_IPHONEX then
    local str = UnityEngine.SystemInfo.deviceModel
    -- local str = "iPhone14,4"
    for i,v in string.gmatch(str,"iPhone(%d+),.*") do
        local ios_ver = tonumber(i)
        -- UnityEngine.Debug.LogError("aaa===" .. tostring(ios_ver))
        if ios_ver > 13 then
            IS_IPHONEX = true
        end
        break
        -- LogError("RET===",i,"|",v,type(i))
    end   
end

PrefKeys = {
    HeartBeatTest = "HeartBeatTest",        --int 心跳包测试缓存
    UpdateRecheck = "lua_asset_update_recheck", --int 1表示需要重启并重新检查是否有资源需要更新
    ClientRestoreFlag = "ClientRestoreFlag",    --int 1表示需要清理客户端cache数据
    RecheckPhasePkgs = "RecheckPhasePkgs",
    -- LoginNoticeNews = "LoginNoticeNews",   -- 最新公告增添时间缓存
    InternalBundleVersion = "InternalBundleVersion",    --记录当前包的版本，用于在覆盖安装包的时候清理旧资源
}