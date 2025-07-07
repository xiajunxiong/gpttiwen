RoleSettingData = RoleSettingData or BaseClass()
RoleSettingData.VOLUME_ZERO = 101
function RoleSettingData:TotalVol(old, new)
    local eff_new_on = new > 0
    local eff_old_on = old > 0
    if eff_new_on ~= eff_old_on then --开关
        if eff_new_on == false then --关掉所有音乐
            AudioMgr:SetEffectFlag(false)
        else
            local eff_vol = RoleSettingData.formatToVolVal(self:GetData(ROLE_SETTING_TYPE.SET_SOUND_VOLUME), new)
            if eff_vol > 0 and self:GetData(ROLE_SETTING_TYPE.SET_SOUND_VOLUME) > 0 then
                AudioMgr:SetEffectFlag(true)
                AudioMgr:SetEffectVolume(eff_vol)
            end
        end
    else --只设置音量
        local eff_vol =  RoleSettingData.formatToVolVal(self:GetData(ROLE_SETTING_TYPE.SET_SOUND_VOLUME), new)
        if eff_vol > 0 then
            AudioMgr:SetEffectVolume(eff_vol)
        end
    end
    local bg_new_on = new >= 0
    local bg_old_on = old >= 0
    if bg_new_on ~= bg_old_on then --开关
        if bg_new_on == false then --关掉所有音乐
            AudioMgr:SetBgFlag(false)
        else
            local bg_vol = RoleSettingData.formatToVolVal(self:GetData(ROLE_SETTING_TYPE.SET_MUSIC_VOLUME), new)
            if bg_vol >= 0 and self:GetData(ROLE_SETTING_TYPE.SET_MUSIC_VOLUME) > 0 then
                AudioMgr:SetBgFlag(true)
                AudioMgr:SetBgVolume(bg_vol)
            end
        end
    else --只设置音量
        local bg_vol = RoleSettingData.formatToVolVal(self:GetData(ROLE_SETTING_TYPE.SET_MUSIC_VOLUME), new)
        if bg_vol >= 0 then
            AudioMgr:SetBgVolume(bg_vol)
        end
    end
end

function RoleSettingData:BgmVol(old, new)
    local tot_vol = self:GetData(ROLE_SETTING_TYPE.SET_TOTAL_VOLUME)
    if tot_vol < 0 then
        return
    end
    local new_on = new >= 0
    local old_on = old >= 0
    if new_on ~= old_on then
        if new_on == false then
            AudioMgr:SetBgFlag(false)
        else
            AudioMgr:SetBgFlag(true)
            AudioMgr:SetBgVolume(RoleSettingData.formatToVolVal(tot_vol, new))
        end
    else
        AudioMgr:SetBgVolume(RoleSettingData.formatToVolVal(tot_vol, new))
    end
end

function RoleSettingData:SoundVol(old, new)
    local tot_vol = self:GetData(ROLE_SETTING_TYPE.SET_TOTAL_VOLUME)
    if tot_vol <= 0 then
        return
    end
    local new_on = new > 0
    local old_on = old > 0
    if new_on ~= old_on then
        if new_on == false then
            AudioMgr:SetEffectFlag(false)
        else
            AudioMgr:SetEffectFlag(true)
            AudioMgr:SetEffectVolume(RoleSettingData.formatToVolVal(tot_vol, new))
        end
    else
        AudioMgr:SetEffectVolume(RoleSettingData.formatToVolVal(tot_vol, new))
    end
end

function RoleSettingData.formatToVolVal(val1,val2)
    val1 = math.abs(val1)
    val1 = val1 == RoleSettingData.VOLUME_ZERO and 0 or val1
    local set_val = 0
    if val2 ~= nil then
        val2 = math.abs(val2)
        val2 = val2 == RoleSettingData.VOLUME_ZERO and 0 or val2
        set_val = math.min(val1, val2) 
    else
        set_val = val1
    end
    return set_val/100
end

function RoleSettingData:PicQua(old, new)
    QuaCtrl:SetQuality(new)
end

function RoleSettingData:RoleNum(old, new)
    SceneCtrl.Instance:Objs():CheckForVisbleRoleSetting()
end

local low_qua_graphic_type = {  --这个列表里放低画质的GPU型号
    "Mali%-",          --Mali-T*|Mali-*
    "PowerVR"           --PowerVR *|PowerVR SGX*
}

local graphic_type_qua_cfg = {
    ["^Adreno %(TM%) (%d+)$"] = {540,650},  --Adreno (TM) *
    ["^Mali%-G5(%d+)$"] = {2,7},            --Mali-G5*
    ["^Mali%-G7(%d+)$"] = {2,7},            --Mali-G7*
    ["^PowerVR GE(%d+)$"] = {8320,8360},    --PowerVR GE*   目前最高看到了8322，默认全部为中
    ["^PowerVR GM(%d+)$"] = {9446,9500},    --PowerVR GM*   目前只看到9446一款，默认全部为中
    ["^PowerVR GT(%d+)$"] = {7400,9500},    --PowerVR GT*   目前只发7400用于安卓，默认全部为中
}


local function GetDefaultQuality()
    local systemInfo = UnityEngine.SystemInfo
    local device_model = systemInfo.deviceModel
    local system_memory_size = systemInfo.systemMemorySize
    local graphic_type = systemInfo.graphicsDeviceName
    -- graphic_type = "PowerVR GT9300"
    local proc_type = systemInfo.processorType
    -- proc_type = nil--"ARMv7 VFPv3 NEON"
    if graphic_type == nil or type(graphic_type) ~= "string" then
        graphic_type = ""
    end
    if proc_type == nil or type(proc_type) ~= "string" then
        proc_type = ""
    end
    print(string.format("SystemInfo\ndeviceModel:%s\nsupportsImageEffects:%s\nsupportedRenderTargetCount:%s\nsystemMemorySize:%s\ngraphicsMemorySize:%s\ngraphicType:%s\nprocType:%s",
        tostring(device_model),tostring(systemInfo.supportsImageEffects),tostring(systemInfo.supportedRenderTargetCount),
		tostring(system_memory_size),tostring(systemInfo.graphicsMemorySize),tostring(graphic_type),tostring(proc_type)))
    if PLATFORM == PlatformType.IOS then        --ios设备
        if device_model == "iPhone7,1" 
            or device_model == "iPhone7,2"
            or device_model == "iPhone8,1"
            or device_model == "iPhone8,2"
            or device_model == "iPhone8,4" 
            or system_memory_size <= 2500 then --iPhone6 6p 6s 6sp SE 2G内存以下设备 默认中画质
            return QUALITY_LEVEL.MEDIUM
        else
            return QUALITY_LEVEL.HIGH
        end
    else
        if IS_MICRO_PLAYER() then
            return QUALITY_LEVEL.MEDIUM
        end
        for graph_prefix,compare_vers in pairs(graphic_type_qua_cfg) do
            local cur_ver_str = string.match(graphic_type, graph_prefix)
            -- LogError("stringFind",graphic_type,graph_prefix,"|",cur_ver_str,"|",cur_ver_str ~= nil and type(cur_ver_str))
            if cur_ver_str ~= nil then
                -- LogError("cur_ver_str=",cur_ver_str)
                local cur_ver = tonumber(cur_ver_str)
                if cur_ver ~= nil then
                    if cur_ver >= compare_vers[2] then
                        return QUALITY_LEVEL.HIGH
                    elseif cur_ver >= compare_vers[1] then
                        return QUALITY_LEVEL.MEDIUM
                    else
                        return QUALITY_LEVEL.LOW
                    end
                end
            end
        end

        for _,low_qua_prefix in pairs(low_qua_graphic_type) do      --筛选低画质GPU型号
            if string.find(graphic_type, low_qua_prefix) == 1 then
                -- LogError("~~~~~~~~~~~~~~~~~~")
                return QUALITY_LEVEL.LOW
            end 
        end

        if string.match(proc_type,"^.*Intel.*$") or string.match(proc_type,"^.*x86.*$") then        --PC上或者模拟器上直接返回中画质
            return QUALITY_LEVEL.MEDIUM
        end

        Debuger.LogError("Report Unknow GraphicType=%s,DeviceModel=%s",tostring(graphic_type),tostring(device_model))
        --此处需要上报设备和GPU型号
        -- MainProber:Step(305,device_model,graphic_type)
        MainProber:Step(MainProber.STEP_UNKNOW_GRAHPIC_TYPE,device_model,graphic_type)
        return QUALITY_LEVEL.LOW

        -- if system_memory_size > 7000 then --and graphics_mem > 3000 then		--内存大于4G且显存大于2G
        --     return QUALITY_LEVEL.MEDIUM
        -- else
        --     return QUALITY_LEVEL.LOW
        -- end



    end
end

local settingExe = {
    [ROLE_SETTING_TYPE.SET_TOTAL_VOLUME] = RoleSettingData.TotalVol,
    [ROLE_SETTING_TYPE.SET_MUSIC_VOLUME] = RoleSettingData.BgmVol,
    [ROLE_SETTING_TYPE.SET_SOUND_VOLUME] = RoleSettingData.SoundVol,
    [ROLE_SETTING_TYPE.SET_PICTUER_QUALITY] = RoleSettingData.PicQua,
    [ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS] = RoleSettingData.RoleNum
}


--没写在这里的默认值都是1
local defaultSetVal = {
    [ROLE_SETTING_TYPE.SET_TOTAL_VOLUME] = 100,
    [ROLE_SETTING_TYPE.SET_MUSIC_VOLUME] = 100,
    [ROLE_SETTING_TYPE.SET_SOUND_VOLUME] = 100,
    -- [ROLE_SETTING_TYPE.SET_PICTUER_QUALITY] = GetDefaultQuality(),
    -- [ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS] = 30,
    [ROLE_SETTING_TYPE.SET_SHOW_REFUSE_TEAM_REQUEST] = 0,
    [ROLE_SETTING_TYPE.SET_SHOW_REFUSE_STRANGER_MSG] = 0,
}

local function getDefaultSetVal(setting_type)
    if setting_type == ROLE_SETTING_TYPE.SET_PICTUER_QUALITY and defaultSetVal[setting_type] == nil then
        defaultSetVal[setting_type] = GetDefaultQuality()
        return defaultSetVal[setting_type]
    elseif setting_type == ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS and defaultSetVal[setting_type] == nil then
        local def_qua = defaultSetVal[ROLE_SETTING_TYPE.SET_PICTUER_QUALITY]
        if def_qua == nil then
            def_qua = GetDefaultQuality()
            defaultSetVal[ROLE_SETTING_TYPE.SET_PICTUER_QUALITY] = def_qua
        end
        if (def_qua == QUALITY_LEVEL.LOW) or 
            (IS_IOS() and def_qua ~= QUALITY_LEVEL.HIGH) then        --默认人数显示低配5，高中配30,IOS只有高配是30
            defaultSetVal[setting_type] = 5
        else
            defaultSetVal[setting_type] = 30
        end
        return defaultSetVal[setting_type]
    else
        return defaultSetVal[setting_type] or 1
    end
end

local delayApplyType = {
    [ROLE_SETTING_TYPE.SET_PICTUER_QUALITY] = true
    -- [ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS] = true,
}

function RoleSettingData:__init()
    -- local def_qua = GetDefaultQuality()
    -- LogError("GetDefaultQuality===",def_qua)
    -- if RoleSettingData.Instance ~= nil then
    --     Debuger.LogError("[RoleSettingData] attempt to create singleton twice!")
    --     return
    -- end
    RoleSettingData.Instance = self

    self.role_setting_data_server = {}
    self.role_setting_data = SmartData.New()

    for i = 1, ROLE_SETTING_TYPE.ROLE_SETTING_TYPE_MAX do
        local pref_key = PrefKeys.SettingVal(i)
        local def_val = UnityPlayerPrefs.GetInt(pref_key, -999999)
        if def_val == -999999 then --没取到存档的值的话，取逻辑里的默认值并写入存档
            def_val = getDefaultSetVal(i)
            UnityPlayerPrefs.SetInt(pref_key, def_val)
        end
        self.role_setting_data[i] = def_val
    end
    for i = 1, ROLE_SETTING_TYPE.ROLE_SETTING_TYPE_MAX do
        local exe_func = settingExe[i]
        if exe_func ~= nil then
            -- if i == ROLE_SETTING_TYPE.SET_PICTUER_QUALITY then 
            --     exe_func(self,1, 1)                
            -- else
                -- LogError("???", i,defaultSetVal[i],self:GetData(i)) 
                exe_func(self, defaultSetVal[i], self:GetData(i))
            -- end 
        end
    end
    self.delay_apply = {}

    self:InitChangeHeadData()

    self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.CheckHeadTipsFlag, self))
	self.change_handle = EventSys:Bind(GlobalEventKeys.RoleProfessionChange, BindTool.Bind(self.CheckHeadTipsFlag, self))

    self.shield_fight_monster = false      -- 是否屏蔽遇到怪物请求
    self.joy_is_on = SmartData.New({val = 0})
    self.power_is_on = SmartData.New({val = 0})
end

function RoleSettingData:__delete()
    if RoleSettingData.Instance == self then
        RoleSettingData.Instance = nil
    end
end

function RoleSettingData:ClearData()
    self.head_red_point.val = false
end

function RoleSettingData:SetData(key, value)
    local old = self:GetData(key)
    if old ~= value then
        UnityPlayerPrefs.SetInt(PrefKeys.SettingVal(key), value)
        self.role_setting_data[key] = value
        self.joy_is_on.val = key == ROLE_SETTING_TYPE.SET_INFO_IS_JOY and value or self.joy_is_on.val
        self.power_is_on.val = key == ROLE_SETTING_TYPE.SET_INFO_IS_POWER and value or self.power_is_on.val
        EventSys:Fire(GlobalEventKeys.RoleSettingChange, key, value)
        if key == ROLE_SETTING_TYPE.SET_PICTUER_QUALITY then
            if value == QUALITY_LEVEL.LOW then
                RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS, 5)
            elseif value == QUALITY_LEVEL.MEDIUM then
                RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS, 10)
            else
                RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS, 30)
            end
        end
        --RoleSettingView
        if ViewMgr:IsOpen(RoleView) and delayApplyType[key] then
            self.delay_apply[key] = function()
                local exe_func = settingExe[key]
                if exe_func ~= nil then
                    exe_func(self, old, value)
                end
            end
        else
            local exe_func = settingExe[key]
            if exe_func ~= nil then
                exe_func(self, old, value)
            end
        end
    end
end

function RoleSettingData:GetData(key)
    return self.role_setting_data[key]
end

function RoleSettingData:GetServerData(key)
    return self.role_setting_data_server[key] or 0
end

function RoleSettingData:DelayApply()
    for _, func in pairs(self.delay_apply) do
        func()
    end
    self.delay_apply = {}
end

--服务端下发设置数据
function RoleSettingData:SetRoleSettingInfo(data)
    if data[ROLE_SETTING_TYPE.SET_PICTUER_QUALITY] ~= -1 then
        self.role_setting_data_server = data
        for i = 1, ROLE_SETTING_TYPE.ROLE_SETTING_TYPE_MAX do
            if i == ROLE_SETTING_TYPE.SET_INFO_IS_JOY and IS_STANDALONE() then
                self:SetData(i, 0)
            else
                self:SetData(i, self.role_setting_data_server[i])
            end
        end
    else
        self.role_setting_data_server = {}
        RoleSettingCtrl.Instance:SaveRoleSettingInfo()
    end
    LogDG("当前模式 = ", data[ROLE_SETTING_TYPE.SYSTEM_SET_INFO_BATTLE_CONTROL_MODE])
end

--是否屏蔽 0不显示1显示
function RoleSettingData.IsShield(key)
    return RoleSettingData.Instance:GetData(key) == 0
end
function RoleSettingData.IsNewMode()
    if GLOBAL_CONFIG:CanChangeMode() then
        return RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SYSTEM_SET_INFO_BATTLE_CONTROL_MODE) == SYSTEM_SET_BATTLE_CONTROL_MODE.SYSTEM_SET_BATTLE_CONTROL_MODE_SLIDE
    end
    return false
end
ChangeHeadType = {
    Advancement = 1,
    Surface = 2,
}

--人物头像列表
function RoleSettingData:InitChangeHeadData()
    self.head_config = {}
    for k,v in pairs(Config.image_change_auto.head_job) do
        if self.head_config[v.avartar_type] == nil then
            self.head_config[v.avartar_type] = {}
        end
        if self.head_config[v.avartar_type][v.type] == nil then
            self.head_config[v.avartar_type][v.type] = {}
        end
        table.insert(self.head_config[v.avartar_type][v.type],v)
    end
    self.head_red_point = SmartData.New({val = false})
end

function RoleSettingData:CheckHeadTipsFlag()
    self.head_tips_list = {{},{}}
    self.head_list = self.head_config[RoleData.Instance:GetRoleAvatarType()] or {}
    for _,list in pairs(self.head_list) do
        table.sort(list,function(a,b)return a.avatar_id < b.avatar_id end)
    end
    for _,list in pairs(self.head_list) do
        for k,v in pairs(list) do
            local key = v.type == 1 and v.advancement_level or v.surface_id
            if UnityPlayerPrefs.GetInt(PrefKeys.HeadTips(v.type,key)) == 1 then
                self:SetHeadTipsFlag(v.type,key,true)
                self:SetHeadRedPoint(true)
            end
        end
    end
end

--获取人物头像列表
function RoleSettingData:GetChangeHeadData(type)
    return self.head_list[type] or {}
end

function RoleSettingData:GetHeadRedPointNum()
    return self.head_red_point.val and 1 or 0
end

function RoleSettingData:SetHeadRedPoint(val)
    self.head_red_point.val = val
end

function RoleSettingData:SetHeadTipsFlag(type,key,flag)
    if flag and flag == true then
        self.head_red_point.val = true
    end
    self.head_tips_list[type][key] = flag
    local pref_key = PrefKeys.HeadTips(type,key)
    UnityPlayerPrefs.SetInt(pref_key,flag and 1 or 0)
end

function RoleSettingData:GetHeadTipsFlag(type,key)
    return self.head_tips_list[type][key]
end

function RoleSettingData:IsShowJoyStick()
    return self:GetData(ROLE_SETTING_TYPE.SET_INFO_IS_JOY) > 0
end


function RoleSettingData:IsShowPowerStick()
    return self:GetData(ROLE_SETTING_TYPE.SET_INFO_IS_POWER) == 0
end