MiniGameData = MiniGameData or BaseClass()
-- 对话类型
MiniGameTalkType = {
    First = 1,  --未登录时触发的
    Normal = 2, --登录后触发的
}

-- 战斗结束类型
MiniGameBattleEndType = {
    None = 0,   --没有战斗
    Win = 1,    --胜利
    Faild = 2,  --失败
}
-- 添加新小游戏后需要自行添加的地方请全局搜索括号里面的字段(*xiaoyouxi*)
function MiniGameData:__init()
    if MiniGameData.Instance ~= nil then
        Debuger.LogError("[MiniGameData] attempt to create singleton twice!")
        return
    end
    MiniGameData.Instance = self
    self.config = Config.minigame_auto
    self:ResetData()
end

function MiniGameData:__delete()
    MiniGameData.Instance = nil
end

function MiniGameData:ResetData()
    -- 随机出来的角色信息和职业信息，这个已经是已经选定好了
    self:SetRandInfo({}, {})
    -- 随机出来的职业信息，这个还没选
    self.jod_info = {}
end

function MiniGameData:BattleEndType(value)
    if value then
        self.battle_end_type = value
    else
        return self.battle_end_type or MiniGameBattleEndType.None
    end
end

function MiniGameData:GetRandInfo()
    return self.rand_avartar_info, self.rand_jod_info
end

function MiniGameData:SetRandInfo(rand_avartar_info, rand_jod_info)
    self:SetRandAvartarInfo(rand_avartar_info)
    self:SetRandJobInfo(rand_jod_info)
end

function MiniGameData:SetRandAvartarInfo(rand_avartar_info)
    self.rand_avartar_info = rand_avartar_info
end

function MiniGameData:SetRandJobInfo(rand_jod_info)
    self.rand_jod_info = rand_jod_info
end

-- 随机两个角色
function MiniGameData:CreateRandAvartarInfo()
    if TableIsEmpty(self.rand_avartar_info) then
        local avartar_info = NeoCharaChangeData.Instance:GetProfCharaList(1)
        local rand = GameMath.Ramdon(#avartar_info)
        --牛郎织女固定用角男形象by工作台id 61335
        if LoginData.Instance:GetMiniGame() == MINIGAME_TYPE.COW_BOY then
            rand = 6
        end
        self.rand_avartar_info = avartar_info[rand]
    end
    return self.rand_avartar_info
end

-- 随机两个角色中的职业
function MiniGameData:CreateRandJobInfo()
    if TableIsEmpty(self.rand_avartar_info) then
        self:CreateRandAvartarInfo()
    end
    if TableIsEmpty(self.jod_info) then
        self.jod_info = {}
        local m_job_info = NeoCharaChangeData.Instance:GetProfCharaProfList(1, self.rand_avartar_info.avartar_type)
        local get_rand_job = function(index)
            local rand = GameMath.Ramdon(#m_job_info)
            self.jod_info[index] = m_job_info[rand]
            table.remove(m_job_info, rand)
        end
        get_rand_job(1)
        get_rand_job(2)
    end
    return self.jod_info
end

function MiniGameData:JudgeTip(not_need_tip)
    not_need_tip = not_need_tip == true
    if LoginData.Instance:GetMiniGame() > 0 then
        return true
    else
        if not not_need_tip then
            LogError("没有该小游戏："..LoginData.Instance:GetMiniGame())
        end
        return false
    end
end

-- 获取对应小游戏模块id
function MiniGameData:GetMiniGameMod(mini_game_type)
    mini_game_type = mini_game_type or LoginData.Instance:GetMiniGame()
    if mini_game_type > 0 then
        if MINIGAME_MOD[mini_game_type] then
            return MINIGAME_MOD[mini_game_type]
        else
            LogError("没有该小游戏对应模块："..mini_game_type)
        end
    else
        LogError("没有该小游戏："..mini_game_type)
    end
    return 0
end

-- 这里需要判断没有的时候和有的时候，根据是否需要来处理
function MiniGameData:CheckChallengeNaviData()
    local mini_game = LoginData.Instance:GetMiniGame()
    local is_need = mini_game > 0
    local mod = is_need and self:GetMiniGameMod() or 0
    -- 首先移除小游戏模块的全部挑战信息
    for k,v in pairs(MINIGAME_MOD) do
        MINIGAME_OPEN_MOD[v] = 0 --先重置
    end
    -- 然后有小游戏的话再添加
    if is_need then
        MINIGAME_OPEN_MOD[mod] = 1
        --打开界面
        ChallengeNaviCtrl.Instance:RegisterOpenReqFunc(mod, function()
            ViewMgr:CloseView(NpcDialogView)-- 打开时需要停止自动任务
            if MainPlayer then
                MainPlayer:StopMove()
            end
            ViewMgr:OpenViewByKey(mod)
        end)
        -- 添加完需要手动注册一遍显示方法
        if mini_game == MINIGAME_TYPE.TOWER_ADVENTURE then
            TowerAdventureCtrl.Instance:RegisterChallengeNaviShow()
        elseif mini_game == MINIGAME_TYPE.COW_BOY then
            CowBoyCtrl.Instance:RegisterChallengeNaviShow()
        end
    end
end

-- 通用的奖励和阶段细分，没有奖励的小游戏不走这
function MiniGameData:GetLeftListConfig(mini_game_type)
    if not self.left_config then
        self.left_config = {}
    end
    mini_game_type = mini_game_type or LoginData.Instance:GetMiniGame()
    if self:JudgeTip() then
        if not self.left_config[mini_game_type] then
            self.left_config[mini_game_type] = {}
            local group_2 = DataHelper.GetSearch(self.config.minigame_award, "minigame_ID", mini_game_type).group_2
            local t = {}
            for k, v in pairs(self.config.award_cfg_2) do
                if v.minigame_group == group_2 then
                    table.insert(t, v)
                end
            end
            self.left_config[mini_game_type] = DataHelper.Subdivision(t, {"plies", "award_num"})
        end
    end
    return self.left_config[mini_game_type] or {}
end

-- 通用的关卡细分，没有奖励的小游戏不走这
function MiniGameData:GetLevelConfig(mini_game_type)
    if not self.level_config then
        self.level_config = {}
    end
    mini_game_type = mini_game_type or LoginData.Instance:GetMiniGame()
    if self:JudgeTip() then
        self.level_config[mini_game_type] = {}
        local index_config = DataHelper.GetSearch(self.config.minigame_award, "minigame_ID", mini_game_type)
        local group_1 = index_config.group_1
        local l_config = Config[index_config.config_name.."_auto"][index_config.level_name]
        self.level_config[mini_game_type] = DataHelper.Subdivision(l_config, {"plies", "plies_seq"})
        for k, v in pairs(self.config.award_cfg_1) do
            self.level_config[mini_game_type][v.plies][v.plies_seq].item_list = v.item_list
        end
    end
    return self.level_config[mini_game_type]
end

-- 获取当前小游戏对话表的细分，需要根据不同的小游戏自己添加
function MiniGameData:DialogConfig(mini_game_type)
    mini_game_type = mini_game_type or LoginData.Instance:GetMiniGame()
    if self:JudgeTip() then
        if mini_game_type == MINIGAME_TYPE.TOWER_ADVENTURE then
            return TowerAdventureData.Instance:DialogConfig()
        elseif mini_game_type == MINIGAME_TYPE.COW_BOY then
            return CowBoyData.Instance:DialogConfig()
        end
    end
end

-- 处理该关卡的数据细分，需要根据不同的小游戏自己添加
function MiniGameData:SetLevelDetail(cfg)
    if self:JudgeTip() then
        local mini_game_type = LoginData.Instance:GetMiniGame()
        if mini_game_type == MINIGAME_TYPE.TOWER_ADVENTURE then
            TowerAdventureData.Instance:SetLevelDetail(cfg)
        elseif mini_game_type == MINIGAME_TYPE.COW_BOY then
            CowBoyData.Instance:SetLevelDetail(cfg)
        end
    end
end

-- 处理战斗结束
function MiniGameData:DealBattleEnd(is_win)
    self:BattleEndType(is_win and MiniGameBattleEndType.Win or MiniGameBattleEndType.Faild)
    local mini_game_type = LoginData.Instance:GetMiniGame()
    if mini_game_type == MINIGAME_TYPE.TOWER_ADVENTURE then
        
    elseif mini_game_type == MINIGAME_TYPE.COW_BOY then
        if TableIsEmpty(CowBoyData.Instance:NowUseLevelData()) then
            self:BattleEndType(MiniGameBattleEndType.None)
        else
            ViewMgr:OpenView(CowBoyGameView)
        end
    end
end