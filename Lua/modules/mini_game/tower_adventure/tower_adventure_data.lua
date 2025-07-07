TowerAdventureData = TowerAdventureData or BaseClass()

TowerAdventureGameType = {
    NearMonster = 1,  --近战怪物
    Teammate = 2,     --队友
    FarMonster = 3,   --远程怪物
}

-- 1是自己的，2是敌方的，塔从下到上的图片名字，目前只有123，如:Hong1
TowerAdventureTowerType = {
    [1] = "Hong",
    [2] = "Huang",
}

-- 1是自己的，2是敌方的
TowerAdventureTowerWho = {
    My = 1,
    Monster = 2,
}

-- 房子图片的编号
TowerAdventureTowerPos = {
    Bottom = 1,
    Center = 2,
    Top = 3,
}

-- 三座塔的x值
TowerAdventureTowerX = {
    [1] = -350,
    [2] = 350,
    [3] = 1050,
}

-- 关卡指引用
TowerAdventureTowerGameGuide = {
    [1] = {
        [1] = {
            show = {[1] = true, [2] = true},
            Txt = 1,
            TxtPos = {x = -350, y = 0},
            Obj1 = {x = -370, y = -252, w = 100, h = 150},
            Obj2 = {x = 350, y = -242, w = 394, h = 170},
            Target = 1,
            Effect = true,
        },
        [2] = {
            show = {[1] = true, [2] = true},
            Txt = 2,
            TxtPos = {x = 350, y = 0},
            Obj1 = {x = 230, y = -252, w = 100, h = 150},
            Obj2 = {x = 350, y = -242, w = 394, h = 170},
            Target = 1,
            Effect = false,
        },
    },
}

function TowerAdventureData:__init()
    if TowerAdventureData.Instance ~= nil then
        Debuger.LogError("[TowerAdventureData] attempt to create singleton twice!")
        return
    end
    TowerAdventureData.Instance = self
    self.config = Config.tower_adventure_auto
    self.mini_config = Config.minigame_auto
    self.game_type = MINIGAME_TYPE.TOWER_ADVENTURE
    self.page_max_level = 16
    self.page_max_reward = 4
    -- 每次向左移动700
    self.move_distance = 700
    -- 移动速度
    self.speed = 30
    -- 是否可以拖拽
    self.is_can_drag = false
    -- 是否拖拽中
    self.is_draging = false
    -- 主角在哪层
    self.person_layer_mono = nil
    -- 主角血量
    self.person_hp = 0
    -- 记录拖拽时鼠标进入哪层
    self.enter_layer_mono = nil
    -- 记录主角被放到哪个person位置的Rect
    self.enter_layer_person = nil
    -- 主角目前在哪座塔，三座塔循环显示
    self.in_turn_tower = 1
    self.level_max = #self.config.tower_cfg
    self.page_max = math.floor(self.level_max / self.page_max_level)

    self.level_data = SmartData.New({now_level = 0, now_page = 1, reward_flags = {}, remind = false})
    
    self:ResetData()

    -- 对话和引导细分
    self.dialog_config = DataHelper.Subdivision(self.config.story_talk, {"seq", "talk_type"})
end

function TowerAdventureData:__delete()
    TowerAdventureData.Instance = nil
end

function TowerAdventureData:Other()
    return self.config.other[1]
end

-- 特殊关卡判断标志位
function TowerAdventureData:OtherSeq(index)
    return self.config.other[1]["seq_"..index]
end

function TowerAdventureData:ResetData()
    self.level_data:Set({now_level = 0, now_page = 1, reward_flags = {}, remind = false})
    self.now_level_detail = {}
end

function TowerAdventureData:GameType()
    return self.game_type
end

function TowerAdventureData:PageMaxLevel()
    return self.page_max_level
end

function TowerAdventureData:LevelMax()
    return self.level_max
end

function TowerAdventureData:PageMax()
    return self.page_max
end

function TowerAdventureData:Speed()
    return self.speed
end

function TowerAdventureData:DialogConfig()
    return self.dialog_config
end

-- 是否用于展示
function TowerAdventureData:GetPersonHp(is_show)
    return is_show and self:DealHpNum(self.person_hp) or self.person_hp
end

function TowerAdventureData:SetPersonHp(value)
    self.person_hp = value
end

function TowerAdventureData:AddPersonHp(value)
    self.person_hp = self.person_hp + value
end

function TowerAdventureData:SetLevelData(protocol)
    self.level_data.now_level = protocol.finish_stage_num
    self.level_data.reward_flags = protocol.phase_reward_flag
    self.level_data:SetDirty("remind")
end

-- 未登录的时候手动设置当前等级
function TowerAdventureData:SetLevelDataNowLevel(level)
    if level > self.level_max then
        level = self.level_max
    end
    self.level_data.now_level = level
end

-- 根据1-seq返回是否有领取标志位
function TowerAdventureData:RewardFlags(seq)
    return self.level_data.reward_flags[seq] or 0
end

-- 根据page和award_num返回是否有领取标志位
function TowerAdventureData:RewardFlagsByPageNum(page, num)
    local seq = (page - 1) * self.page_max_reward + num
    return self:RewardFlags(seq)
end

-- 根据页数和页数对应的关卡，返回根据总关卡计数的关卡
function TowerAdventureData:GetLevelByPageAndLevel(page, level)
    return (page - 1) * self.page_max_level + level
end

-- 根据总关卡计数的关卡，返回根据页数和页数对应的关卡
function TowerAdventureData:GetPageAndLevelByLevel(level)
    return math.floor((level - 1) / self.page_max_level) + 1, ((level - 1) % self.page_max_level) + 1
end

-- 塔楼冒险总红点
function TowerAdventureData:AllRemind()
    if not MiniGameData.Instance:JudgeTip(true) then
        return 0
    end
    if LoginData.Instance:GetMiniGame() ~= MINIGAME_TYPE.TOWER_ADVENTURE then
        return 0
    end
    for i = 1, self.page_max do
        if self:LeftRemind(i) == 1 then
            return 1
        end
    end
    return 0
end

-- 左侧按钮红点
function TowerAdventureData:LeftRemind(index)
    if not MiniGameData.Instance:JudgeTip(true) then
        return 0
    end
    for i = 1, self.page_max_reward do
        if self:ButtomRemind(index, i) == 1 then
            return 1
        end
    end
    return 0
end

-- 下方进度条红点
function TowerAdventureData:ButtomRemind(page, num)
    if not MiniGameData.Instance:JudgeTip(true) then
        return 0
    end
    local l_config = MiniGameData.Instance:GetLeftListConfig()
    local left_config = l_config[page][num]
    local need_level = self:GetLevelByPageAndLevel(page, left_config.show_seq)
    if self.level_data.now_level >= need_level then
        return self:RewardFlagsByPageNum(page, num) == 0 and 1 or 0
    end
    return 0
end


-- 处理该关卡的数据细分
function TowerAdventureData:SetLevelDetail(cfg)
    self.now_use_level_data = cfg
    self.now_level_detail = {}
    self.person_hp = cfg.hp
    -- 创建刚开始自己的塔
    self.now_level_detail[1] = self:T_GetTower(1, 1)
    table.insert(self.now_level_detail[1].layer_list, self:T_GetLayer(1, 1, TowerAdventureTowerWho.My))
    local index = 1
    while cfg["tower_"..index] and cfg["tower_"..index] ~= "" do
        local t1 = Split(cfg["tower_"..index], ",")
        for k, v in pairs(t1) do
            -- p1代表在第几层，p2是TowerAdventureGameType枚举，p3是模型id，p4是血量
            local t2 = Split(v, ":")
            for i = 1, #t2 do
                t2[i] = tonumber(t2[i])
            end
            if TableIsEmpty(self.now_level_detail[index + 1]) then
                self.now_level_detail[index + 1] = self:T_GetTower(index + 1, 2)
            end
            -- 一层最多可以放两只怪
            if TableIsEmpty(self.now_level_detail[index + 1].layer_list[t2[1]]) then
                self.now_level_detail[index + 1].layer_list[t2[1]] = self:T_GetLayer(t2[1], index + 1, TowerAdventureTowerWho.Monster, t2[3], t2[2], t2[4])
            else
                -- 表示已经有一只怪了，这里需要交换一下第一只怪和第二只怪的位置
                local t_monster = self.now_level_detail[index + 1].layer_list[t2[1]].monster_1
                self.now_level_detail[index + 1].layer_list[t2[1]].monster_1 = self:T_GetMonster(t2[3], t2[2], t2[4])
                self.now_level_detail[index + 1].layer_list[t2[1]].monster_2 = t_monster
            end
        end
        index = index + 1
    end
end

function TowerAdventureData:T_GetTower(index, tower_type)
    local t = {}
    t.index = index
    t.tower_type = tower_type
    t.layer_list = {}
    return t
end

function TowerAdventureData:T_GetLayer(index, tower_index, tower_type, res_id_1, monster_type_1, hp_1, res_id_2, monster_type_2, hp_2)
    local t = {}
    -- 数组的index，这个index会根据层数改变
    t.index = index
    -- 刚开始显示的index，这个index不会变
    t.first_index = index
    t.tower_index = tower_index
    t.tower_type = tower_type
    t.is_lock = t.index > 3
    t.monster_1 = self:T_GetMonster(res_id_1, monster_type_1, hp_1)
    t.monster_2 = self:T_GetMonster(res_id_2, monster_type_2, hp_2)
    return t
end

function TowerAdventureData:T_GetMonster(res_id, monster_type, hp)
    local t = {}
    if res_id then
        t.res_id = res_id
        t.monster_type = monster_type
        t.hp = hp
    end
    return t
end

function TowerAdventureData:AddLayer(now_tower)
    local layer = self:T_GetLayer(#now_tower.layer_list + 1, now_tower.tower_index, now_tower.tower_type)
    table.insert(now_tower.layer_list, layer)
end

function TowerAdventureData:RemoveLayer(now_tower, remove_index, memory_index)
    local new_index = 1
    table.remove(now_tower.layer_list, remove_index)
    for k, v in pairs(now_tower.layer_list) do
        v.index = k
        if v.first_index == memory_index then
            new_index = k
        end
    end
    return new_index
end

-- 获得该关卡的数据细分（是否重置）
function TowerAdventureData:GetLevelDetail(is_reset)
    if is_reset then
        self:SetLevelDetail(self.now_use_level_data)
    end
    return self.now_level_detail
end

function TowerAdventureData:NowUseLevelData()
    return self.now_use_level_data
end

-- 当前说挑战中的关卡
function TowerAdventureData:NowFightSeq()
    return self.now_use_level_data.seq or 1
end

-- 将超过999的血量处理成多少k来显示
function TowerAdventureData:DealHpNum(value)
    if value > 999 then
        -- 保留一位小数
        value = Format("%.1f", value / 1000)
        return Format(Language.TowerAdventure.HpHum, value)
    end
    return value
end

-- 根据当前打到哪来显示哪页
function TowerAdventureData:CalculateNowPage()
    self.level_data.LeftItemnow_page = math.floor(self.level_data.now_level / self.page_max_level) + 1
    self.level_data.now_page = self.level_data.now_page > self.page_max and self.page_max or self.level_data.now_page
end

function TowerAdventureData:GetIsCanDrag()
    return self.is_can_drag
end

function TowerAdventureData:SetIsCanDrag(value)
    self.is_can_drag = value
end

function TowerAdventureData:GetIsDraging()
    return self.is_draging
end

function TowerAdventureData:SetIsDraging(value)
    self.is_draging = value
end

function TowerAdventureData:GetPersonLayerMono()
    return self.person_layer_mono
end

function TowerAdventureData:SetPersonLayerMono(mono)
    self.person_layer_mono = mono
end

function TowerAdventureData:GetEnterLayerMono()
    return self.enter_layer_mono
end

function TowerAdventureData:SetEnterLayerMono(mono)
    self.enter_layer_mono = mono
end

function TowerAdventureData:GetEnterLayerPerson()
    return self.enter_layer_person
end

function TowerAdventureData:SetEnterLayerPerson(rect)
    self.enter_layer_person = rect
end

function TowerAdventureData:GetInTurnTower()
    return self.in_turn_tower
end

function TowerAdventureData:GetNextInTurnTower(value)
    value = value or 1
    local va = (self.in_turn_tower + value) % 3
    return va == 0 and 3 or va
end

function TowerAdventureData:SetInTurnTower(is_next, value)
    if is_next then
        self.in_turn_tower = self.in_turn_tower + 1
    else
        self.in_turn_tower = value
    end
    local va = self.in_turn_tower % 3
    self.in_turn_tower = va == 0 and 3 or va
end