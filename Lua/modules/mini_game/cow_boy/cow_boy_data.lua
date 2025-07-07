CowBoyData = CowBoyData or BaseClass()
-- 关卡指引用
CowBoyGameGuide = {
    [1] = {
        [1] = {
            show = {[1] = true, [2] = true},
            Txt = 1,
            TxtPos = {x = -285, y = -375},
            Obj1 = {x = -242, y = 25, w = 114, h = 140},
            Obj2 = {x = -61, y = 25, w = 114, h = 140},
            Target = 2,
        },
    },
}

CowBoyRewardState = {
    NotEnough = 0,
    CanGet = 1,
    Got = 2,
}

function CowBoyData:__init()
    if CowBoyData.Instance ~= nil then
        Debuger.LogError("[CowBoyData] attempt to create singleton twice!")
        return
    end
    CowBoyData.Instance = self
    self.config = Config.cowboy_auto
    self.game_type = MINIGAME_TYPE.COW_BOY
    self.page_max_level = 10
    -- 是否可以拖拽
    self.is_can_drag = false
    -- 是否拖拽中
    self.is_draging = false
    -- 主角血量
    self.person_hp = 0
    -- 主角当前所在格子
    self.person_pos = 1
    -- 终点当前所在格子
    self.end_pos = 1
    self.battle_type = CowBoyCfg.BattleType.None
    -- 记录拖拽时鼠标进入哪层
    self.enter_island_mono = nil
    self.level_max = #self.config.level_list
    self.page_max = math.floor(self.level_max / self.page_max_level)

    self.level_data = SmartData.New({now_level = 0, now_page = 1, reward_flags = {}})
    
    self:ResetData()

    self.game_smdata = SmartData.New({reset_girl_eff = false})

    -- 对话和引导细分
    self.dialog_config = self.config.talk_list--DataHelper.Subdivision(self.config.talk_list, {"seq", "talk_type"})
    -- 地图格子数据
    self.map_config = DataHelper.Subdivision(self.config.map_list, {"level_type", "grid_id"})
    -- 根据关卡id返回关卡配置
    Cfg.Register("CowBoyLevelConfig", Cfg.RealTimeSearch(self.config.level_list, "seq"))
    -- 事件类型
    Cfg.Register("CowBoyEvent", Cfg.RealTimeSearch2Key(self.config.event_list, "event_type", "match_id"))

end

function CowBoyData:__delete()
    CowBoyData.Instance = nil
end

function CowBoyData:Other()
    return self.config.other[1]
end

function CowBoyData:GetLeftListConfig()
    if not self.left_config then
        self.left_config = DataHelper.Subdivision(self.config.level_list, {"plies", "plies_seq"})
    end
    return self.left_config
end

function CowBoyData:GetLevelConfig()
    if not self.level_config then
        self.level_config = DataHelper.Subdivision(self.config.level_list, {"plies", "plies_seq"})
    end
    return self.level_config
end

-- 特殊关卡判断标志位
function CowBoyData:OtherSeq(index)
    return self.config.other[1]["seq_"..index]
end

function CowBoyData:ResetData()
    self.level_data:Set({now_level = 0, now_page = 1, reward_flags = {}})
    self.now_level_detail = {}
end

function CowBoyData:GameType()
    return self.game_type
end

function CowBoyData:PageMaxLevel()
    return self.page_max_level
end

function CowBoyData:LevelMax()
    return self.level_max
end

function CowBoyData:PageMax()
    return self.page_max
end

function CowBoyData:DialogConfig()
    local cfg = {}
    local step_id = 0
    local now_level = self.level_data.now_level + 1
    local talk_type = LoginData.Instance:LoginState() == LoginStateType.Logined and MiniGameTalkType.Normal or MiniGameTalkType.First
    for _,v in ipairs(self.dialog_config) do
        if v.seq == now_level and v.talk_type == talk_type and v.grid_id == 0 then
            table.insert(cfg, v)
            if not StringIsEmpty(v.step_id) then
                step_id = v.step_id
            end
        end
    end
    -- return self.dialog_config
    return cfg, step_id
end

function CowBoyData:GetGridDialogCfg()
    local cfg = {}
    local now_pos = self:PersonPos()
    local now_level = self.level_data.now_level + 1
    local talk_type = LoginData.Instance:LoginState() == LoginStateType.Logined and MiniGameTalkType.Normal or MiniGameTalkType.First
    for _, v in ipairs(self.dialog_config) do
        if v.seq == now_level and v.talk_type == talk_type and v.grid_id == now_pos then
            table.insert(cfg, v)
        end
    end
    return cfg
end

-- 是否用于展示
function CowBoyData:GetPersonHp(is_show)
    return self.person_hp-- is_show and self:DealHpNum(self.person_hp) or self.person_hp
end

-- 主角所在格子
function CowBoyData:PersonPos(value)
    if value then
        self.person_pos = value
    else
        return self.person_pos
    end
end

-- 终点所在格子
function CowBoyData:EndPos(value)
    if value then
        self.end_pos = value
    else
        return self.end_pos
    end
end

function CowBoyData:SetPersonHp(value)
    self.person_hp = value
end

function CowBoyData:AddPersonHp(value)
    self.person_hp = self.person_hp + value
end

function CowBoyData:SetLevelData(protocol)
    self.level_data.now_level = protocol.finish_stage_num
    self.level_data.reward_flags = protocol.phase_reward_flag
    if self.reward_groud == nil then
        self.reward_groud = MiniGameData.Instance:GetLeftListConfig(MINIGAME_TYPE.COW_BOY)
    end
end

function CowBoyData:GetRewardItem()
    local page_reward_cfg = self.reward_groud[self.level_data.now_page]
    if page_reward_cfg and page_reward_cfg[1] then
        local reward_item_cfg = page_reward_cfg[1].reward_item
        return Item.Init(reward_item_cfg.item_id, reward_item_cfg.num, reward_item_cfg.is_bind)
    end
end

function CowBoyData:GetRewardState()
    if self.level_data.now_page * self.page_max_level > self.level_data.now_level then
        return CowBoyRewardState.NotEnough
    elseif self.level_data.reward_flags[self.level_data.now_page] == 1 then
        return CowBoyRewardState.Got
    else
        return CowBoyRewardState.CanGet
    end
end

function CowBoyData:RewardShow()
    return RoleId() ~= 0
end

-- 未登录的时候手动设置当前等级
function CowBoyData:SetLevelDataNowLevel(level)
    if level > self.level_max then
        level = self.level_max
    end
    self.level_data.now_level = level
end

function CowBoyData:GetLevelDataNowLevel()
    return self.level_data.now_level
end

-- 判断当前是否已经打完所有关卡
function CowBoyData:NowIsMaxLevel()
    return self.level_data.now_level >= self:LevelMax()
end

-- 根据类型获取地图数据
function CowBoyData:GetMapCfg(map_type)
    return self.map_config[map_type]
end

-- 处理该关卡的数据细分，相当于重置数据
function CowBoyData:SetLevelDetail(cfg)
    self.now_use_level_data = cfg
    self.now_level_detail = {}
    self.person_hp = cfg.base_power
    self.person_pos = cfg.start_point
    self.end_pos = cfg.end_point
    self.map_width = cfg.map_width or 2228
    local map_cfg = self:GetMapCfg(cfg.level_type)
    for i, v in ipairs(map_cfg) do
        self.now_level_detail[i] = self:T_GetIsland(v)
    end
    -- 首先要把主角位置和周围格子遮挡剔除
    self:LevelDetailById(self.person_pos).is_mask = false
    local can_pos = self:LevelDetailById(self.person_pos).can_pos
    for k, v in pairs(can_pos) do
        self:LevelDetailById(v).is_mask = false
    end
end

function CowBoyData:GetMapWidth()
    return self.map_width or 2228
end

-- 一个格子信息初始化处理
function CowBoyData:T_GetIsland(cfg)
    local t = {}
    t.id = cfg.grid_id
    t.map_type = cfg.level_type
    t.cell_type = cfg.event_type
    t.event_param = cfg.event_param
    t.cell_info = Cfg.CowBoyEvent(cfg.event_type, cfg.event_param, false) or {}
    t.hp = cfg.power
    t.pos = Vector3.New(cfg.pos_x, cfg.pos_y, 0)
    -- 是否碎裂，走过了的不能再走
    t.is_break = false
    -- 是否遮挡
    t.is_mask = true
    t.grid_res = cfg.grid_res
    t.is_show = cfg.is_show
    -- 相邻的位置
    t.can_pos = {}
    local t1 = Split(cfg.nearby, "|")
    for i, v in ipairs(t1) do
        table.insert(t.can_pos, tonumber(v))
    end
    return t
end

-- 获得该关卡的数据细分（是否重置）
function CowBoyData:GetLevelDetail(is_reset)
    if is_reset then
        self:SetLevelDetail(self.now_use_level_data)
    end
    return self.now_level_detail
end

-- 根据id获取当前格子信息
function CowBoyData:LevelDetailById(id)
    return self.now_level_detail[id]
end

-- 是否碎裂
function CowBoyData:LevelDetailIsBreak(id, value)
    local now_detail = self:LevelDetailById(id)
    if now_detail then
        if value ~= nil then
            now_detail.is_break = value
            now_detail:Notify()
        else
            return now_detail.is_break
        end
    end
end

-- 是否遮挡
function CowBoyData:LevelDetailIsMask(id, value)
    local now_detail = self:LevelDetailById(id)
    if now_detail then
        if value ~= nil then
            now_detail.is_mask = value
            now_detail:Notify()
        else
            return now_detail.is_mask
        end
    end
end

-- 当前未处理的关卡数据
function CowBoyData:NowUseLevelData()
    return self.now_use_level_data
end

function CowBoyData:ResetNowUseLevelData()
    self.now_use_level_data = nil
end

-- 当前挑战中的关卡
function CowBoyData:NowFightSeq()
    return self.now_use_level_data.seq or 1
end

-- 当前是否有什么，（宠物伙伴技能）
function CowBoyData:NowHasType(m_type)
    for k, v in pairs(self:GetLevelDetail()) do
        if v.cell_type == m_type then
            if v.is_break or v.id == self:PersonPos() then
                return true
            end
        end
    end
    return false
end

-- 用于获取（宠物伙伴技能）所在关卡的数据
function CowBoyData:GetLevelDetailByEvent(m_type)
    for k, v in pairs(self:GetLevelDetail()) do
        if v.cell_type == m_type then
            return v
        end
    end
end

-- 将超过999的血量处理成多少k来显示
function CowBoyData:DealHpNum(value)
    if value > 999 then
        -- 保留一位小数
        value = Format("%.1f", value / 1000)
        return Format(Language.CowBoy.HpHum, value)
    end
    return value
end

-- 根据当前打到哪来显示哪页
function CowBoyData:CalculateNowPage()
    self.level_data.now_page = math.floor(self.level_data.now_level / self.page_max_level) + 1
    self.level_data.now_page = self.level_data.now_page > self.page_max and self.page_max or self.level_data.now_page
end

function CowBoyData:BattleType(value)
    if value then
        self.battle_type = value
    else
        return self.battle_type
    end
end

function CowBoyData:GetIsCanDrag()
    return self.is_can_drag
end

function CowBoyData:SetIsCanDrag(value)
    self.is_can_drag = value
end

function CowBoyData:GetIsDraging()
    return self.is_draging
end

function CowBoyData:SetIsDraging(value)
    self.is_draging = value
end

function CowBoyData:GetEnterIslandMono()
    return self.enter_island_mono
end

function CowBoyData:SetEnterIslandMono(mono)
    self.enter_island_mono = mono
end

function CowBoyData:SetBattleData(data)
    self.battle_data = data
end

function CowBoyData:GetBattleData()
    return self.battle_data
end

function CowBoyData:GetCanReachFinal()
    self.break_id = {}
    for k, v in pairs(self.now_level_detail) do
        if v.is_break then
            self.break_id[v.id] = true
        end
    end
    --checked_id中值 -1:不可走 0:可到终点 1:已查询
    self.checked_id = {[self:PersonPos()] = 1}
    for _, v in pairs(self.now_level_detail[self:PersonPos()].can_pos) do
        if v == self.end_pos then
            return true
        elseif not self.break_id[v] then
            local result, id = self:CalculateCanReachFinal(self.now_level_detail[v])
            if result == 0 then
                return true
            end
            self.checked_id[id] = result
        end
    end
    return false
end

function CowBoyData:CalculateCanReachFinal(id_cfg)
    if not self.break_id[id_cfg.id] then
        self.checked_id[id_cfg.id] = 1
        for _, v in pairs(id_cfg.can_pos) do
            if v == self.end_pos then
                return 0, id_cfg.id
            elseif not self.break_id[v] and self.checked_id[v] ~= 1 and self.checked_id[v] ~= -1 then
                local result, id = self:CalculateCanReachFinal(self.now_level_detail[v])
                if result == 0 then
                    return result, id
                end
                self.checked_id[id] = result
            end
        end
        return -1, id_cfg.id
    else
        return -1, id_cfg.id
    end
end