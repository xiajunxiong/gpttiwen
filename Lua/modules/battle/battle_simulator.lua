BattleSimulator = BattleSimulator or BaseClass()

-- local DefaultFileName = "Default"

function BattleSimulator:__init()
    if BattleSimulator.Instance ~= nil then
        Debuger.LogError("[BattleSimulator] attempt to create singleton twice!")
        return
    end
    BattleSimulator.Instance = self

    self.data = BattleData.Instance
    self.stream = BattleCtrl.Instance.stream

    self.base =
        SmartData.New(
        {
            isPlaying = false
        }
    )
    self.SimulationData = {}
end

function BattleSimulator:Base()
    return self.base
end

function BattleSimulator:IsPlaying()
    return self.base.isPlaying
end

function BattleSimulator:Start(fileName)
    if ViewMgr:IsOpen(LoginView) then
        self.LoginViewClosed = true
        ViewMgr:CloseView(LoginView)
    end
    if self.base.isPlaying then
        self.stream:Clear()
    end
    self.base.isPlaying = true
    self.fileName = fileName
    local data = self:LoadData(self.fileName)
    self.originChInfo = data.ChInfo
    BattleCtrl.Instance:OnRecvChInfo(TableCopy(self.originChInfo, 3), true)
    self.hallow_list = data.HallowList
    if self.hallow_list then
        BattleCtrl.Instance:SCBattleHallowList(TableCopy(self.hallow_list, 3), true)
    end
    self.dragon_list = data.DragonList
    if self.dragon_list then
        local p = {dragon_list = TableCopy(self.dragon_list, 3)}
        BattleCtrl.Instance:OnSCBattleGodDragonInfo(p, true)
    end

    self.rabbit_shield_list = data.RabbitList
    if self.rabbit_shield_list then
        local p = {rabbit_shield_list = TableCopy(self.rabbit_shield_list, 3)}
        BattleCtrl.Instance:OnSCBattleShieldGridInfo(p, true)
    end

    Game.BattleDataLinker.data = TableToString(data.RoundData)
    self.roundData = data.RoundData
    self.curRound = 1
    TimeHelper:AddDelayTimer(
        function()
            self:PlayRound()
        end,
        3
    )
end

-- function BattleSimulator:SkipToRound(round)
--     if self.stream.play_queue_timer then
--         TimeHelper:CancelTimer(self.stream.play_queue_timer)
--         self.stream.play_queue_timer = nil
--     end
--     BattleCtrl.Instance.displayer:RestoreAllCh()
--     if round == 1 then
--         for _, v in pairs(self.data.ch_info) do
--             v.curhp = v.maxhp
--             v.curmp = v.maxmp
--             v.buff_list:Clear()
--             v.ui_buff_list:Clear()
--             v.attr_buff_list:Clear()
--         end
--     else
--         local round_data = self.roundData[round]
--         if round_data then
--         end
--     end
--     self.curRound = round
--     TimeHelper:AddDelayTimer(
--         function()
--             self:PlayRound()
--         end,
--         1
--     )
-- end

function BattleSimulator:PlayRound()
    -- Time.timeScale = 0.5
    local cur_round_data = self.roundData[self.curRound]
    if not cur_round_data then
        LogError("无第" .. self.curRound .. "回合信息")
        self:Quit()
        return
    end
    self.data:BaseData().cur_round = self.curRound
    self.stream.behaviour_queues = {}
    BattleData.Instance:GenerateDataChInfo()

    --one queue each move data
    for i, mo_da in ipairs(cur_round_data) do
        local queue = BattleBehaviourQueue.New(i)
        BattleNodeEvent.SetBehaviourQueue(queue)
        table.insert(self.stream.behaviour_queues, queue)
        -- BattleStream.setup_move_data_pre(mo_da)
        BattleNodeEvent.detail_player = BattleDetailPlayer.New(mo_da.eve_type, mo_da.grid_idx, mo_da.data)
        BattleStream.setup_move_data_post(mo_da, true)
        BattleNodeEvent.detail_player = nil
    end
    BattleNodeEvent.SetBehaviourQueue(nil)

    local limit_num = 1
    self.stream:playQueueListSyncly(
        function()
            TimeHelper:AddDelayTimer(
                function()
                    self.curRound = self.curRound + 1
                    PublicPopupCtrl.Instance:Center("第"..self.curRound.."回合")
                    self:PlayRound()
                end,
                1
            )
        end,
        limit_num
    )
end

-- function BattleSimulator:PlayNextRound()
--     self.curRound = self.curRound + 1
--     self:PlayRound()
-- end

-- function BattleSimulator:PlayPreviousRound()
--     self.curRound = self.curRound - 1
--     self:PlayRound()
-- end

-- function BattleSimulator:Pause()
--     self.cacheTimeScale = Time.timeScale
--     Time.timeScale = 0
-- end

-- function BattleSimulator:Resume()
--     Time.timeScale = self.cacheTimeScale
-- end

function BattleSimulator:Quit()
    self.base.isPlaying = false
    -- local battle_mode = self.data:BaseData().battle_mode
    -- self:CancelCountdownTimer()
    Time.timeScale = TimeHelper:TimeScale()
    if self.LoginViewClosed then
        self.LoginViewClosed = false
        SceneCtrl.Instance:GotoScene(LoginScene)
        TimeHelper:AddDelayTimer(
            function()
                ViewMgr:OpenView(LoginView)
            end,
            1
        )
    else
        SceneCtrl.Instance:GotoScene(GameScene)
    end
    self.data:BaseData().server_fighting = false
    -- PetCtrl.Instance:PopDelayPetShow()
    self.stream:Clear()
    self.roundData = {}
    self.curRound = 0

    -- if self.fileName then
    --     package.loaded["BattleSimulatorData." .. self.fileName] = nil
    --     self.fileName = nil
    -- end
    --战斗结果弹出结算界面
    -- self:RecvResult(protocol.battle_info, battle_mode)
end

local function TableToString(tab, level, convented_tabs)
    if type(tab) ~= "table" then
        return
    end

    if convented_tabs ~= nil then --重复嵌套问题
        if convented_tabs[tab] == true then
            return
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
        local key_str
        if type(key) == "number" then
            key_str = string.format("[%s]", key)
        else
            key_str = tostring(key)
        end

        local value_str
        if type(value) == "table" then
            local tab_str = TableToString(value, level + 1, convented_tabs)
            if tab_str == nil then
                tab_str = "{" .. tostring(value) .. "}"
            end
            value_str = "\n" .. tab_str
        elseif type(value) == "string" then
            value_str = string.format('"%s"', value)
        elseif type(value) ~= "userdata" then
            value_str = tostring(value)
        end
        
        if type(value) ~= "userdata" then
            res = string.format("%s%s%s = %s,\n", res, level_str, key_str, value_str)
        end
    end

    res = res .. level_str .. "}"
    return res
end

local function getPath(fileName)
    return string.format("BattleSimulatorData/%s.lua", fileName)
end
local function persistentDataPath(fileName)
    return string.format("%s/%s.lua", UnityApp.persistentDataPath, fileName)
end
local function fightdataPath()
    local data = LoginData.Instance:AccountData()
    local path = string.format("%sfightdata/%s/%s_%s_%s_%s_%s.lua",GLOBAL_CONFIG:Urls().upload_url,RoleData.Instance:GetRoleId(),
    tostring(os.date("%Y:%m:%d_%X")),data.spid,data.key,GLOBAL_CONFIG.server_info.last_server,RoleData.Instance:GetRoleId())
    return path
end
function BattleSimulator:SaveData(fileName)
    if next(self.SimulationData) then
        FileTool.WriteAllText(getPath(fileName), "BattleSimulatorData = " .. TableToString(self.SimulationData))
        self.SimulationData = {}
    end
end
function BattleSimulator:UnLoadData()
    if BattleCtrl.Instance:DevOrMl1() or BattleData.Instance:BattleMode() ~= BATTLE_MODE.BATTLE_MODE_BIG_DIPPER then
        if next(self.SimulationData) then
            WebRequester:Upload(fightdataPath(),"BattleSimulatorData = " .. TableToString(self.SimulationData))
            -- self.SimulationData = {}
        end
    end
end
function BattleSimulator:LoadData(fileName)
    Game.LuaManager.Singleton():DoFile(getPath(fileName))
    return BattleSimulatorData
    -- return TryParseJson(FileTool.ReadAllText(getPath(fileName)))
    -- return FILE_REQ(string.format("BattleSimulatorData/%s", fileName))
end
