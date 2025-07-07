-- 开局 关卡 1
-- -1 时进行位置确定
-- -0.9 展示兔子(并且进行倒计时3秒)
-- -0.8 隐藏兔子
-- -0.5 开始交换位置(-0.5之后进行请求开始游戏)
-- 0 倒计时10秒且游戏进行中

--超时服务端下发1 (失败) 退出场景 关卡设置为1
--分支 1.1 失败时需要展示失败过程 后退出
--分支 选择错误 下发1 (失败) 退出场景 关卡设置为1
--分支 选择正确 0.1 进行正确位置展示设置状态
--这里有个关卡变化 模式变化
-- 0.2 进行全隐藏
-- -1
--分支当需要选择多个兔子的时候，需要cache当前选择
--新增状态0.3 进行已选择兔子显示
--分支 选择正确时假如通关 0.2
--显示正确答案后 退出场景

LookingForRabbitsData = LookingForRabbitsData or BaseClass()
function LookingForRabbitsData:__init()
    if LookingForRabbitsData.Instance ~= nil then
        Debuger.LogError('[LookingForRabbitsData] attempt to create singleton twice!')
        return
    end
    LookingForRabbitsData.Instance = self
    self.cfg_info = Config.sanxian_auto
    self.other_info = self.cfg_info.other[1]
    self.level_cfg = self.cfg_info.level
    self.change_cfg = self.cfg_info.change
    self.word_cfg = self.cfg_info.word
    self.info = SmartData.New({today_times = 0, pass_idx = 0})
    self.cache_pass_idx = 1
    self.cache_game_type = 0
    self.final_pass_idx = 0
    self.title_flag = 0
    self.show_title = false
    self.select_idxs = {}
    self.game =
        SmartData.New(
        {
            state = -1,
            --未开始
            pass_idx = 1, -- 为当前关卡
            choose_tamp = 0
            --倒计时
        }
    )
    -- 继续细分
    -- 进场景（等资源都搞好默认都是好的 -1 展示当前兔子文字（倒计时3秒）-1.5 隐藏过程 -1.2 开始交换 0倒计时10秒 （1|2 对或者错，展示兔子并爆炸隐藏其他特效 ， 3全隐藏
    -- 回到-1））
    self.change =
        SmartData.New(
        {
            start = false,
            init = false
        }
    )
    self.just_equil_max = false
    self:InitConfig()
end
function LookingForRabbitsData:InitConfig()
    self.change_group_list = {}
    for i, v in ipairs(self.change_cfg) do
        self.change_group_list[v.change_group] = self.change_group_list[v.change_group] or {}
        table.insert(self.change_group_list[v.change_group], v.change_unit)
    end
    self.bullet_list = {}
    for i, v in ipairs(self.word_cfg) do
        table.insert(self.bullet_list, v.word)
    end
end
function LookingForRabbitsData:__delete()
end
function LookingForRabbitsData:Clear()
    self:Game().state = -1 --0 游戏种， 1 失败， 2胜利
    self:Game().pass_idx = 1
    self.cache_pass_idx = 1
    self.cache_game_type = 0
    self.final_pass_idx = 0
    self.title_flag = 0
    self.show_title = false
    self.select_idxs = {}
    -- self:Game().choose_tamp = protocol.choose_tamp
end

function LookingForRabbitsData:ActInfo()
    return self.info
end
function LookingForRabbitsData:Game()
    return self.game
end
function LookingForRabbitsData:StartTime()
    return self.other_info.start_time
end

function LookingForRabbitsData:CurLevel()
    return self.game.pass_idx
end
function LookingForRabbitsData:RewardLevel()
    local cur_level = self:CurLevel()
    for i, v in ipairs(self.level_cfg) do
        if not StringIsEmpty(v.reward_show) and v.level >= cur_level then
            return v.level
        end
    end
    return nil
end
function LookingForRabbitsData:ChangeList()
    local cur_level = self:CurLevel()
    local level_cfg = self:LevelCfg()
    if level_cfg then
        local list = {}
        local group = self.change_group_list[level_cfg.change_group]
        for i = 1, level_cfg.change_time do
            table.insert(list, group[GameMath.Ramdon(1, #group)])
        end
        -- LogError('关卡变化')
        -- LogDG(list)
        return list
    else
        LogDG('关卡配置获取失败, 当前关卡 = ', cur_level)
    end
end
function LookingForRabbitsData:LevelCfg(cur_level)
    cur_level = cur_level or self:CurLevel()
    local level_cfg = self.level_cfg[cur_level]
    return level_cfg
end
function LookingForRabbitsData:NotifyStart()
    self.change.start = not self.change.start
end
function LookingForRabbitsData:RightIdxs(need, max)
    local list = {}
    for i = 1, need do
        table.insert(list, self:RamdonNum(max, list))
    end
    return list
end
function LookingForRabbitsData:RamdonNum(max, list)
    local num = GameMath.Ramdon(1, max)
    local same = false
    for i, v in ipairs(list) do
        if v == num then
            same = true
        end
    end
    if not same then
        return num
    else
        return self:RamdonNum(max, list)
    end
end
function LookingForRabbitsData:FinalArray(list, list2)
    local a, b
    for i, v in ipairs(list2) do
        if #v > 2 then
            for k = 1, (#v / 2) do
                a = tonumber(v[k * 2 - 1])
                b = tonumber(v[k * 2])
                list[a], list[b] = self:ChangePos(list[a], list[b])
            end
        else
            a = tonumber(v[1])
            b = tonumber(v[2])
            list[a], list[b] = self:ChangePos(list[a], list[b])
        end
    end
    return list
end
function LookingForRabbitsData:ChangePos(a, b)
    a = tonumber(a)
    b = tonumber(b)
    a = a + b
    b = a - b
    a = a - b
    return a, b
end
function LookingForRabbitsData:NeedRabbit(unit_num)
    local num = 0
    for i, v in ipairs(self.level_cfg) do
        if v.unit_num == unit_num then
            num = math.max(num, v.rabbit_num)
        end
    end
    return num
end
function LookingForRabbitsData:SetPosObj(objs)
    self.pos_objs = objs
end
function LookingForRabbitsData:GetPosObj()
    return self.pos_objs
end
function LookingForRabbitsData:NotifyInit()
    self.change.init = not self.change.init
end

function LookingForRabbitsData:SetRightIdxs(idxs)
    self.right_idxs = idxs
end
function LookingForRabbitsData:GetRightIdxs()
    return self.right_idxs
end
function LookingForRabbitsData:IsChangeMode(pass_idx)
    local old = self:LevelCfg()
    local new = self:LevelCfg(pass_idx)
    return new.unit_num ~= old.unit_num
end
function LookingForRabbitsData:SelectIdxs()
    return self.select_idxs
end
function LookingForRabbitsData:SetSelect(index)
    table.insert(self.select_idxs, index)
end
function LookingForRabbitsData:ClearSelect()
    self.select_idxs = {}
end
function LookingForRabbitsData:GetPassReward()
    if self:IsOutCount() then
        return {}
    end
    local reward_list = {}
    local pass_level = self.final_pass_idx
    for i, v in ipairs(self.level_cfg) do
        if pass_level == v.level then
            for k, value in pairs(v.final_reward) do
                table.insert(reward_list, value)
            end
        end
    end
    --[[ if pass_level <= 0 then
        for k, value in pairs(self.other_info.reward_min) do
            table.insert(reward_list, value)
        end
    end ]]
    if self.show_title == true then
        table.insert(reward_list, {item_id = self.other_info.title_item_id})
    end
    return DataHelper.FormatItemList(reward_list)
end
function LookingForRabbitsData:ShowReward()
    if self:IsOutCount() then
        return {}
    end
    local pass_level = self.final_pass_idx
    local show_level = 0
    for i, v in ipairs(self.level_cfg) do
        if pass_level >= v.level and not StringIsEmpty(v.reward_show) then
            show_level = math.max(v.level, show_level)
        end
    end
    if show_level > 0 then
        return {Item.Create({item_id = self.level_cfg[show_level].item_id})}
    end
    return {}
end
function LookingForRabbitsData:PassRewardDesc()
    if self:IsOutCount() then
        return ""
    end
    local pass_level = self.final_pass_idx
    local desc = Language.LFRabbits.NoItem
    for i, v in ipairs(self.level_cfg) do
        if pass_level >= v.level and not StringIsEmpty(v.reward_show) then
            desc = v.reward_show
        end
    end
    return desc
end
function LookingForRabbitsData:IsInGame()
    return SceneData.Instance:GetMode() == SCENE_TYPE.SAN_XIAN_GUI_DONG
end
function LookingForRabbitsData:IsOutCount()
    return self.info.today_times >= self.other_info.reward_time and not self.just_equil_max
end
function LookingForRabbitsData:RamdonBullet(state)
    local level_info = self:LevelCfg()
    local info = level_info.start_talk
    local list
    local idx
    if state == -0.9 then
        info = level_info.start_talk
        list = string.split(info, '|')
        idx = tonumber(list[GameMath.Ramdon(1, #list)])
    elseif state == 0.1 or state == 0.3 then
        info = level_info.succ_talk
        list = string.split(info, '|')
        idx = tonumber(list[GameMath.Ramdon(1, #list)])
    elseif state == 1.1 then
        info = level_info.fail_talk
        list = string.split(info, '|')
        idx = tonumber(list[GameMath.Ramdon(1, #list)])
    end
    return self.bullet_list[idx]
end
function LookingForRabbitsData:IsPassFinal()
    return self.final_pass_idx == self.other_info.max_level
end
function LookingForRabbitsData:IsActiveTitle()
    local item_id = self.other_info.title_item_id
    local config = Item.GetConfig(item_id)
    local flag = AppearanceData.Instance:GetActiveFlag(config.param1)
    return flag == 1
end
