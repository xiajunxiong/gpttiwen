--奇闻场景逻辑注册类
AnecdoteLogic = AnecdoteLogic or BaseClass()

--加载完某个场景 就触发
local load_logic_tab = {
    [AnecdoteType.StarLife] = StarLifeScene
}

--需要走到某个范围 就触发
local range_logic_tab = {
    [AnecdoteType.Vision] = VisionScene,
    [AnecdoteType.FishTear] = FishTearScene,
    [AnecdoteType.DuanShiFei] = DuanShiFeiScene,
    [AnecdoteType.PuzzleStorehouse] = PuzzleStorehouseScene,
}

function AnecdoteLogic:__init()
    if AnecdoteLogic.Instance ~= nil then
        Debuger.LogError("[AnecdoteLogic] attempt to create singleton twice!")
        return
    end
    AnecdoteLogic.Instance = self
    AnecdoteData.Instance:Care(BindTool.Bind(self.OnTaskChange,self))
    self.register_list = {}
end

function AnecdoteLogic:InitSceneLogicData()
    for k,v in pairs(load_logic_tab) do
        if v.Init then
            v.Init(v,self:GetSceneConfig(k))
        end
        self:LoadRegister(k,BindTool.Bind(v.OnSceneLogic,v))
    end
    for k,v in pairs(range_logic_tab) do
        if v.Init then
            v.Init(v,self:GetSceneConfig(k))
        end
        self:RangeRegister(k,BindTool.Bind(v.OnSceneLogic,v))
    end
end

function AnecdoteLogic:InitSceneLogic(type)
    for k,v in pairs(load_logic_tab) do
        if k == type and v.InitSceneLogic then
            v.InitSceneLogic(v)
        end
    end
    for k,v in pairs(range_logic_tab) do
        if k == type and v.InitSceneLogic then
            v.InitSceneLogic(v)
        end
    end
    self.cur_anecdote_type = type
end

function AnecdoteLogic:OnLeaveScene(type)
    for k,v in pairs(load_logic_tab) do
        if k == type and v.OnLeaveScene then
            v.OnLeaveScene(v)
        end
    end
    for k,v in pairs(range_logic_tab) do
        if k == type and v.OnLeaveScene then
            v.OnLeaveScene(v)
        end
    end
    self.cur_anecdote_type = nil
end

function AnecdoteLogic:CheckTaskChange(tab)
    for k,v in pairs(tab or {}) do
        if not self:IsHasTask({type=k}) then
            local config = self:GetSceneConfig(k)
            if config ~= nil then
                self:RemoveRegister(config.scene_id)
            end
            if v.OnTaskFinish then
                v.OnTaskFinish(v)
            end
        else
            if v.OnTaskChange then
                v.OnTaskChange(v)
            end
        end
    end
end

function AnecdoteLogic:OnTaskChange()
    self:CheckTaskChange(load_logic_tab)
    self:CheckTaskChange(range_logic_tab)
end

function AnecdoteLogic:ClearLastSceneLogic(scene_id)
    TimeHelper:CancelTimer(self.load_timer)
    TimeHelper:CancelTimer(self.range_timer)
    self.range_invoke_timer = self.range_invoke_timer or {}
    if scene_id then
        TimeHelper:CancelTimer(self.range_invoke_timer[scene_id])
        self.range_invoke_timer[scene_id] = nil
    else
        for k,v in pairs(self.range_invoke_timer) do
            TimeHelper:CancelTimer(self.range_invoke_timer[k])
            self.range_invoke_timer[k] = nil
        end
    end
    self:OnLeaveScene(self.cur_anecdote_type)
end

function AnecdoteLogic:RemoveRegister(scene_id)
    self:ClearLastSceneLogic(scene_id)
    self.register_list[scene_id] = nil
end

--注册场景逻辑 加载完某个场景 就触发
function AnecdoteLogic:LoadRegister(type,func)
    local config = self:GetSceneConfig(type)
    if config == nil then
        return
    end
    if self.register_list[config.scene_id] then
        return
    end
    self.register_list[config.scene_id] = {beh_cfg = config,load_func = func}
end

--注册场景逻辑 需要走到某个范围 就触发
function AnecdoteLogic:RangeRegister(type,func)
    local config = self:GetSceneConfig(type)
    if config == nil then
        return
    end
    if self.register_list[config.scene_id] then
        return
    end
    self.register_list[config.scene_id] = {beh_cfg = config,range_func = func}
end

--进入场景
function AnecdoteLogic:OnGameSceneLoaded(scene_id)
    self:ClearLastSceneLogic()
    local param_t = self.register_list[scene_id]
    if param_t and self:IsHasTask(param_t.beh_cfg) then
        self:InitSceneLogic(param_t.beh_cfg.type)
        self:CheckSceneLogic(param_t)
    end    
end

--开启监听条件
function AnecdoteLogic:CheckSceneLogic(param_t)
    local delay_time = param_t.beh_cfg.delay_time or 1
    if param_t.load_func then
        self.load_timer = Invoke(function()
            param_t.load_func(param_t.beh_cfg)
        end,delay_time)
    end
    if param_t.range_func then
        self.range_timer = Invoke(function()
            self:CheckRange(param_t.beh_cfg,param_t.range_func)
        end,delay_time)
    end
end

--开启范围检查
function AnecdoteLogic:CheckRange(beh_cfg,func)
    local interval = 0
    local show_range = false
    local is_in_leave = true
    TimeHelper:CancelTimer(self.range_invoke_timer[beh_cfg.scene_id])
    local function GetPos(pos)
        local posl = Split(pos,"|")
        return {x = tonumber(posl[1]),z = tonumber(posl[2])}
    end
    local trigger_pos = GetPos(beh_cfg.trigger_pos)
    local function Check(value)
        show_range = value or not show_range
        interval = show_range and beh_cfg.loop or -beh_cfg.loop
        if show_range then 
            if MainPlayer and MainPlayer:GetDistance(trigger_pos) < beh_cfg.distance then
                is_in_leave = false
            else
                is_in_leave = true
                show_range = false
            end
        end
        if beh_cfg.loop == 0 then
            Call(func,is_in_leave,beh_cfg)
        else
            Call(func,show_range,is_in_leave,beh_cfg)
        end
    end
    self.range_invoke_timer[beh_cfg.scene_id] = TimeHelper:AddRunTimer(function()
        if 0 == interval then
            Check(false)
        end
        interval = interval > 0 and interval - 1 or interval + 1
        if not show_range and is_in_leave then
            Check(true)
        end
    end, beh_cfg.invoke or 1)
end

--是否有奇闻任务
function AnecdoteLogic:IsHasTask(beh_cfg)
    return AnecdoteData.Instance:GetTaskInfoByType(beh_cfg.type or 0) ~= nil
end

--得到一些场景对象board_list
function AnecdoteLogic.GetBoardList(npc_list)
    local board_list = {}
    for i,v in ipairs(Split(npc_list,"|")) do
        if tonumber(v) == 0 then
            local role_id = RoleData.Instance:GetRoleId()
            local board = MainViewData.Instance:GetBoard("role_id",role_id)
            if board~= nil then
                table.insert(board_list,board)
            end
        else
            local board = MainViewData.Instance:GetNpcBoard(tonumber(v))
            if board ~= nil then
                table.insert(board_list,board)
            end
        end
    end
    return board_list
end

function AnecdoteLogic:GetSceneConfig(type)
	return Config.anecdotes_tasklist_auto.scene_config[type]
end