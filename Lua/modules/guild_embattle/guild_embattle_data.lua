GuildEmbattleData = GuildEmbattleData or BaseClass()
function GuildEmbattleData:__init()
    if GuildEmbattleData.Instance ~= nil then
        Debuger.LogError('[GuildEmbattleData] attempt to create singleton twice!')
        return
    end
    GuildEmbattleData.Instance = self
    self.base =
        SmartData.New(
        {
            champion_guild_id = 0,
            guess_item = {},
            curr_round = 1,
            --第几轮
            round_state = 0,
            --0是准备1是开始
            next_round_state_time = 0,
            --回合结束时间
            is_finish = 0,
            fighter_num = 0
        }
    )
    self.battle_process = SmartData.New({process = 0})
    self.score_rank_info = SmartData.New({list = {}})
    -- self.last_info = SmartData.New({num = 0, list = {}})
    self.view_data = SmartData.New({select = 1, stage = 1, flush = false, round = 1})
    self.battle_data = SmartData.New({index = 0, area = 1, data = nil})
    self.my_score_info = nil
    self.fighter_info = nil
    self.fighter_num = 0

    self.champion_guild_id = 0
    self.guess_item = {}
    self.preview_reward_list = {}

    ----------------
    self.match_info = {}
    self.last_match_info = SmartData.New()
    self:initConfig()
end
function GuildEmbattleData:initConfig()
    local value
    for i = 0, #Config.family_honor_auto.other[1].reward_show do
        value = Config.family_honor_auto.other[1].reward_show[i]
        table.insert(self.preview_reward_list, Item.Create(value))
    end
    self:CacheRankReward()
end
function GuildEmbattleData:ResetMatchList()
    self.match_list = {}
    --存储战况列表
    -- local list = {8, 4, 2, 1}
    for i = 1, 4 do --16-8 8-4 4-2 2-1
        self.match_list[i] = {}
        --[[ for j = 1, list[i] do
            self.match_list[i][j] = {}
        end ]]
    end
end
function GuildEmbattleData:ResetMatchRecordList()
    self.last_match_list = {}
    -- local list = {8, 4, 2, 1}
    for i = 1, 4 do --16-8 8-4 4-2 2-1
        self.last_match_list[i] = {}
        --[[ for j = 1, list[i] do
            self.last_match_list[i][j] = {}
        end ]]
    end
end
function GuildEmbattleData:__delete()
end
function GuildEmbattleData:Clear()
    -- self.match_info = {}
end
function GuildEmbattleData:ViewData()
    return self.view_data
end
function GuildEmbattleData:SetBattleProcess(info)
    self.battle_process.process = info.process
end
function GuildEmbattleData:BattleProcess()
    return self.battle_process
end
function GuildEmbattleData:SetScoreRankInfo(info)
    self.score_rank_info.list = info.items
end
function GuildEmbattleData:ScoreRankInfo()
    return self.score_rank_info
end
function GuildEmbattleData:ServerName(server_id)
    local info = LoginData.Instance:GetServerInfoById(server_id, LoginData.Instance:GetLastLoginAccount())
    if info ~= nil then
        return info.name
    end
    return ''
end
function GuildEmbattleData:SetMyBattleScoreInfo(info)
    self.my_score_info = info
end

function GuildEmbattleData:GetScoreRankInfo(guild_id)
    for k, v in pairs(self.score_rank_info.list) do
        if guild_id > 0 and v.guild_id == guild_id then
            return v
        end
    end
end

function GuildEmbattleData:MyGuildScoreInfo()
    local guild_id = GuildData.Instance:GetGuildID()
    for _, value in ipairs(self.score_rank_info.list) do
        if guild_id > 0 and value.guild_id == guild_id then
            return value
        end
    end
    if self.my_score_info then
        return self.my_score_info
    end
    return nil
end
function GuildEmbattleData:Base()
    return self.base
end
function GuildEmbattleData:SetBase(info)
    --当前回合
    --0是准备1是开始
    --回合结束时间
    self.base.curr_round = info.curr_round
    self.base.round_state = info.round_state
    self.base.next_round_state_time = info.next_round_state_time
    self.base.is_finish = info.is_finish

    self.base.fighter_num = info.fighter_num
    self.base.start_first_round = info.start_first_round
end

function GuildEmbattleData:Round()
    return self.base.curr_round
end
function GuildEmbattleData:State()
    return self.base.round_state
end

function GuildEmbattleData:FighterInfo()
    return self.fighter_info
end
function GuildEmbattleData:SetMatchInfo(info)
    self.match_info = info
end

-- 新增
-- 1是否有参赛资格
-- 2当前状态是否满足进入比赛现场
function GuildEmbattleData:EntryQualify()
    local entry = false
    for _, value in ipairs(self.match_info) do
        entry = value:EntryQualify()
        if entry == true then
            break
        end
    end
    return entry
end
function GuildEmbattleData:EntryScene()
    --当前状态会回合准备中即可进场
end
function GuildEmbattleData:MatchList()
    local list = {}
    -- round = 1
    local temp
    for index, value in ipairs(self.match_info) do
        temp = value:MatchList()
        if self:GuildNum(temp) > 0 then
            AndList(list, temp)
        end
    end
    --这个list是当前回合的所有对战信息
    return list
end
function GuildEmbattleData:GuildNum(list)
    local count = 0
    for index, value in pairs(list) do
        if type(value) == 'table' then
            count = count + table.nums(value)
        end
    end
    return count
end
function GuildEmbattleData:FinalMatchList()
    local list = {}
    -- round = 1
    for index, value in ipairs(self.match_info) do
        if self:GuildNum(value:FinalMatchList()) > 0 then
            AndList(list, value:FinalMatchList())
        end
    end
    --这个list是当前回合的所有对战信息
    return list
end
function GuildEmbattleData:FinalMatchList2()
    local list = {}
    -- round = 1
    for index, value in ipairs(self.last_match_info) do
        if self:GuildNum(value:FinalMatchList()) > 0 then
            AndList(list, value:FinalMatchList())
        end
    end
    --这个list是当前回合的所有对战信息
    return list
end
--到时候还得分组存
--start随人数变化 >8=1 <8>4=2 <4>2=3
function GuildEmbattleData:FlushMatchData()
    if self.fighter_info == nil then
        LogError('没收到数据')
        return
    end

    self:ResetMatchList()
    --第一轮 2个家族一组存储
    local index
    for i, value in ipairs(self.fighter_info) do
        index = math.ceil(i / 2)
        self.match_list[1][index] = self.match_list[1][index] or {}
        self.match_list[1][index].index = index
        table.insert(self.match_list[1][index], value)
    end
    --上一轮 出线家族存储
    local temp
    for round = 2, 4 do
        for i, v in ipairs(self.match_list[round - 1]) do
            index = math.ceil(i / 2)
            temp = nil
            if v[1] and v[2] and v[1].match_round > v[2].match_round then
                temp = v[1]
            elseif v[1] and v[2] and v[1].match_round < v[2].match_round then
                temp = v[2]
            elseif v[2] == nil then
                temp = v[1]
            --v1必然存在
            end
            self.match_list[round][index] = self.match_list[round][index] or {}
            self.match_list[round][index].index = index
            table.insert(self.match_list[round][index], temp)
        end
    end
    --季军赛问题
    local max_round = self:MaxRound()
    if self.match_list[max_round - 1] then
        for _, v in ipairs(self.match_list[max_round - 1]) do
            temp = nil
            if v[1] and v[1].match_round_2 > 0 then
                temp = v[1]
            elseif v[2] and v[2].match_round_2 > 0 then
                temp = v[2]
            end
            self.match_list[max_round][2] = self.match_list[max_round][2] or {}
            self.match_list[max_round][2].index = 2
            table.insert(self.match_list[max_round][2], temp)
        end
    end
end
function GuildEmbattleData:OpponetInfo(guide_id)
    local temp = nil
    for index, value in ipairs(self.match_info) do
        temp = value:OpponetInfo(guide_id)
        if temp then
            return temp
        end
    end
    return temp
    --[[ local guild_id = GuildData.Instance:GetGuildID()
    local list = self.match_list[self:Round()]
    if list == nil then
        return
    end
    for index, value in ipairs(list) do
        for k, v in ipairs(value) do
            if v.guild_id == guild_id then
                return value
            end
        end
    end
    return nil ]]
end
function GuildEmbattleData:OpponetInfo2(guild_id)
    --这里比较麻烦需要找打我方家族位置，然后判断是1还是
    --如果不需要那就返回就完事了
    local list = self.match_list[self:Round()]
    if list == nil then
        return
    end
    for index, value in ipairs(list) do
        for k, v in ipairs(value) do
            if v.guild_id == guild_id then
                return value
            end
        end
    end
    return nil
end
function GuildEmbattleData:GuildName(name)
    return string.format('<color=#23F1AB>%s</color>', name)
end
function GuildEmbattleData:SetMathcRecord(protocol)
    -- self.last_info.num = protocol.fighter_num
    -- self.last_info.list = protocol.fighter_info
    -- self:FormatMatchRecordData()

    self.last_match_info:Set(protocol.match_info_record)
end
function GuildEmbattleData:LastMatchInfo()
    return self.last_match_info
end
--[[ function GuildEmbattleData:FormatMatchRecordData()
    if self.last_info.num == 0 then
        return
    end
    self:ResetMatchRecordList()
    local index
    for i, value in ipairs(self.last_info.list) do
        index = math.ceil(i / 2)
        self.last_match_list[1][index] = self.last_match_list[1][index] or {}
        self.last_match_list[1][index].index = index
        table.insert(self.last_match_list[1][index], value)
    end
    local temp
    for round = 2, 4 do
        for i, v in ipairs(self.last_match_list[round - 1]) do
            index = math.ceil(i / 2)
            temp = nil
            if v[1] and v[2] and v[1].match_round > v[2].match_round then
                temp = v[1]
            elseif v[1] and v[2] and v[1].match_round < v[2].match_round then
                temp = v[2]
            elseif v[2] == nil then
                temp = v[1]
            end
            self.last_match_list[round][index] = self.last_match_list[round][index] or {}
            self.last_match_list[round][index].index = index
            table.insert(self.last_match_list[round][index], temp)
        end
    end
    local max_round = 5 - self:CalcRound(self.last_info.num, 1)
    if self.last_match_list[max_round - 1] then
        for _, v in ipairs(self.last_match_list[max_round - 1]) do
            temp = nil
            if v[1] and v[1].match_round_2 > 0 then
                temp = v[1]
            elseif v[2] and v[2].match_round_2 > 0 then
                temp = v[2]
            end
            self.last_match_list[max_round][2] = self.last_match_list[max_round][2] or {}
            self.last_match_list[max_round][2].index = 2
            table.insert(self.last_match_list[max_round][2], temp)
        end
    end
end ]]
--[[ function GuildEmbattleData:LastInfo()
    return self.last_info
end ]]
function GuildEmbattleData:LastMatchList()
    return self.last_match_list
end
function GuildEmbattleData:SetGuessInfo(protocol)
    self.base.champion_guild_id = protocol.champion_guild_id
    self.base.guess_item = protocol.guess_item
end
function GuildEmbattleData:GuessChampion()
    return self.base.champion_guild_id
end
function GuildEmbattleData:IsChampion(guild_id)
    return self.base.champion_guild_id == guild_id
end
function GuildEmbattleData:IsWinner(round, guild_id)
    if self.base.guess_item[round] then
        return self.base.guess_item[round][guild_id] or false
    end
    return false
end
function GuildEmbattleData:BattleData()
    return self.battle_data
end
--观看战斗的数据
function GuildEmbattleData:WatchBattle(data)
    if data[1] then
        local is_can_watch = false
        local battle_id = 0
        self.battle_data.area = 0
        for i, v in ipairs(data[1].battle_result) do
            if v == 0 and data[1].battle_id[i] ~= 0 then
                is_can_watch = true
                battle_id = data[1].battle_id[i]
                self.battle_data.area = i
                self.battle_data.data = data
            end
        end
        if is_can_watch then
            if not LoginData.Instance:IsOnCrossSever() then
                LoginCtrl.Instance:SendCrossStartReq()
                Runner.Instance:RunUntilTrue(
                    function()
                        if LoginData.Instance:IsOnCrossSever() then
                            BattleCtrl.Instance:SendWatchBattleReq(battle_id, 1)
                            return true
                        end
                    end
                )
                return
            end
            BattleCtrl.Instance:SendWatchBattleReq(battle_id, 1)
        else
            PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.BattleEnd2)
        end
    else
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.NoWatchBattle)
    end
end
function GuildEmbattleData:IsCanWatch(area, data)
    --[[ local list = self:FighterInfo()
    if list == nil then
        return false
    end ]]
    if data == nil then
        return false
    end
    local info = data[1]
    if info == nil then
        return false
    end
    if info.battle_result and info.battle_result[area] == 0 and info.battle_id and info.battle_id[area] ~= 0 then
        return true, info.battle_id[area]
    end
    return false
end
function GuildEmbattleData:FormatColor(desc, type)
    return string.format(Language.GuildEmbattle.GuessBtnColorStr[type], desc)
end
--当前比赛组别类型显示，根据家族参赛家族数量决定
-- 1 显示4组比晒
-- 2 显示2组比晒
-- 3 不显示组
function GuildEmbattleData:GroupType(fighter_num)
    fighter_num = fighter_num or self.base.fighter_num
    local group_type = 3
    if fighter_num > 8 then
        group_type = 1
    elseif fighter_num > 4 then
        group_type = 2
    elseif fighter_num >= 2 then
        group_type = 3
    end
    return group_type
end
--根据参赛家族数量 不知道是啥了 计算组别最后目标显示的
function GuildEmbattleData:GroupTargetType(fighter_num)
    fighter_num = fighter_num or self.base.fighter_num
    local target_type = 1
    if fighter_num > 8 then
        target_type = 1
    elseif fighter_num > 4 then
        target_type = 2
    elseif fighter_num > 2 then
        target_type = 3
    else
        target_type = 4
    end
    return target_type
end
--根据参赛家族数量得到需要进行的比赛轮数
function GuildEmbattleData:MatchNum(fighter_num)
    fighter_num = fighter_num or self.base.fighter_num
    local num = 4
    if fighter_num > 8 then
        num = 4
    elseif fighter_num > 4 then
        num = 3
    elseif fighter_num > 2 then
        num = 2
    else
        num = 1
    end
    return num
end
function GuildEmbattleData:InitRound(fighter_num)
    fighter_num = fighter_num or self.fighter_num
    local round = 1
    if fighter_num > 8 then
        round = 1
    elseif fighter_num > 4 then
        round = 2
    elseif fighter_num > 2 then
        round = 3
    elseif fighter_num == 2 then
        round = 4
    end
    return round
end
-- 1 2 3 4
-- 16-8 8-4  4-2 2-1
-- need_select_view_round
function GuildEmbattleData:CalcRound(num, curr_round)
    num = num or self.fighter_num
    curr_round = curr_round or self:Round()
    local round = self:InitRound(num)
    round = round + ((curr_round - 1) * 1)
    return round
end
function GuildEmbattleData:InitStage(fighter_num)
    fighter_num = fighter_num or self.base.fighter_num
    local stage = 1
    if fighter_num > 8 then
        stage = 1
    elseif fighter_num > 4 then
        stage = 2
    elseif fighter_num > 2 then
        stage = 3
    elseif fighter_num <= 2 then
        stage = 4
    end
    return stage
end
--正在进行的阶段
-- 1 2 3 4
-- 16-8 8-4 4-2 2-1
function GuildEmbattleData:Stage(fighter_num, curr_round)
    fighter_num = fighter_num or self.base.fighter_num
    curr_round = curr_round or self.base.curr_round
    local stage = self:InitStage(fighter_num)
    stage = stage + ((curr_round - 1) * 1)
    if stage == 0 then
        LogDG('数据有问题，当前round = ', curr_round, '计算得阶段stage = ', stage)
        stage = 1
    end
    return stage
end
function GuildEmbattleData:FinalRound(fighter_num)
    return 5 - self:InitStage(fighter_num)
end
function GuildEmbattleData:FinalStage()
    local stage = self:Stage(nil, 1)
    return 5 - stage
end
--最后一轮比晒回合
function GuildEmbattleData:MaxRound()
    local init = self:CalcRound(nil, 1)
    return 5 - init
end
-- show_round 展示其实不完全对的展示 计算出展示的round
function GuildEmbattleData:ShowRound()
    local round = self:ViewData().round
    local init_round = self:InitRound()
    round = round - (init_round - 1)
    return round
end
--根据选择的阶段，推出是显示第几轮的信息
function GuildEmbattleData:RoundByStage()
    local stage = self:ViewData().stage
    local round = stage - self:InitStage() + 1
    return round
end
function GuildEmbattleData:GetGuildName(guild_id)
    for _, value in ipairs(self.match_info) do
        for _, value2 in ipairs(value.fighter_info) do
            if value2.guide_id == guild_id then
                return value2.name
            end
        end
    end
    return ''
end
function GuildEmbattleData:IsOpen()
    local status = ActivityData.Instance:GetActivityStatus(ActType.GuildEmbattle)
    if status ~= ActStatusType.Close then
        return true
    end
    return false
end
function GuildEmbattleData:IsReady()
    local process = self.battle_process.process
    return process == GUILD_HONOR_BATTLE_PROCESS.GUILD_FIGHT or process == GUILD_HONOR_BATTLE_PROCESS.READY
end
function GuildEmbattleData:Result(data)
    local state = self:State()
    local round = self:RoundByStage()
    if self:Round() == round then
        if state == GUILD_HONOR_STATE.GUESSING then
            --无输赢
            return 0
        elseif state == GUILD_HONOR_STATE.FIGHTING then
            if data[2] == nil or (data[2] and data[2].guild_id <= 0) then
                --1轮空
                return 1
            end
            if data[1].is_lose == 0 and data[2].is_lose == 1 then
                --1胜利
                return 1
            end
            if data[1].is_lose == 1 and data[2].is_lose == 0 then
                --2胜利
                return 2
            end
            if data[1].battle_result then
                local is_end = true
                local pingju = true
                local count1 = 0
                local count2 = 0
                for i, v in ipairs(data[1].battle_result) do
                    if v == data[1].guild_id then
                        count1 = count1 + 1
                    elseif v == data[2].guild_id then
                        count2 = count2 + 1
                    elseif v == 0 then
                        is_end = false
                    end
                    if v ~= -1 then
                        pingju = false
                    end
                end
                if pingju then
                    if data[1].rank < data[2].rank then
                        return 1
                    else
                        return 2
                    end
                end
                if is_end then
                    if count1 > count2 then
                        return 1
                    elseif count1 < count2 then
                        return 2
                    end
                end
            end
            return 0
        end
    else
        if data[2] == nil or (data[2] and data[2].guild_id <= 0) then
            --1轮空
            return 1
        end
        if data[1].match_round > round then
            return 1
        end
        if data[2].match_round > round then
            return 2
        end
        return 0
    end
end
function GuildEmbattleData:GetPreviewReward()
    return self.preview_reward_list
end

-------------获取最后胜利排行列表
function GuildEmbattleData:GetRank()
    -- LogDG("================获取排行榜====================")
    local temp = {}
    local temp_obj
    for key, value in ipairs(self.match_info) do
        temp_obj = value:WinRankChampion()
        if temp_obj then
            table.insert(temp, temp_obj)
        end
    end
    table.sort(temp, DataSorter.KeyLowerSorter("win_rank"))
    -- LogDG("================结束排行榜====================")
    return temp
end
function GuildEmbattleData:GetLastRank()
    -- LogDG("================获取排行榜====================")
    local temp = {}
    local temp_obj
    for key, value in ipairs(self.last_match_info) do
        temp_obj = value:WinRankChampion()
        if temp_obj then
            table.insert(temp, temp_obj)
        end
    end
    table.sort(temp, DataSorter.KeyLowerSorter("win_rank"))
    -- LogDG("================结束排行榜====================")
    return temp
end
function GuildEmbattleData:GetRankReward(rank)
    return self.rank_reward_list[rank] or {}
end
function GuildEmbattleData:CacheRankReward()
    self.rank_reward_list = {}
    local info = Config.family_honor_auto.rank_reward
    local temp
    for i,v in ipairs(info) do
        temp = {}
        for _,j in pairs(v.reward) do
            table.insert(temp, Item.Init(j.item_id, j.num, j.is_bind))
        end
        if v.title_item > 0 then
            table.insert(temp, Item.Init(v.title_item, 1))
        end
        --[[ if v.gongxian > 0 then
            table.insert(temp, Item.Init(CommonItemId.GongXian, v.gongxian))
        end ]]
        if v.winner_coin > 0 then
            table.insert(temp, Item.Init(CommonItemId.Coin, v.winner_coin, 1))
        end
        --[[ if v.family_coin > 0 then
            table.insert(temp, Item.Init(CommonItemId.GuildMoney, v.family_coin))
        end ]]
        table.insert(self.rank_reward_list, temp)
    end
end






GuildHonorBattleFighterInfo = GuildHonorBattleFighterInfo or BaseClass()
function GuildHonorBattleFighterInfo:__init(match_info)
    self.match_info = match_info --套了一层
end
function GuildHonorBattleFighterInfo:__delete()
end
function GuildHonorBattleFighterInfo:DebugMsg(index)
    LogDG('=========================================', index)
    LogDG('家族index', self.number)
    LogDG('家族名字', self.name)
    LogDG('族长名字', self.role_name)
    LogDG('争夺目标', self.match_info.win_rank)
    LogDG('到达回合', self.match_round, '当前回合', self.match_info.curr_round)
    LogDG('finish = ', self.is_finish, 'lose = ', self.is_lose)
    LogDG('支持人数', self.guess_num)
    LogDG('战斗ids ', self.battle_id)
end
function GuildHonorBattleFighterInfo:ResultType()
    local round = self.match_info:Round()
    if round == 0 then
        LogDG('显示有问题')
        return 0
    end
    if self.match_info.fighter_num == 1 then
        return 1
    end
    --[[ if self.match_info.curr_round and self.match_round > self.match_info.curr_round then
        return 1
    end ]]
    if self.match_round > round then
        return 1
    else
        if self.is_finish == 1 and self.is_lose == 0 then
            return 1
        elseif self.is_finish == 1 and self.is_lose == 1 then
            return 2
        end
    end
    local round_state = self.match_info.round_state
    if round_state == GUILD_HONOR_STATE.GUESSING then
        return 0
    elseif round_state == GUILD_HONOR_STATE.FIGHTING then
        if self.battle_result then
            local is_end = true
            local pingju = true
            local count1 = 0
            for i, v in ipairs(self.battle_result) do
                if v == self.guild_id then
                    count1 = count1 + 1
                elseif v == 0 then
                    is_end = false
                end
                if v ~= -1 then
                    pingju = false
                end
            end
            if pingju then
                return 0
            end
            if is_end then
                if count1 > 1 then
                    return 1
                else
                    return 2
                end
            end
        end
        return 0
    end
end
function GuildHonorBattleFighterInfo:GuessNum()
    local round = self.match_info:Round()
    if round == 0 then
        return 0
    end
    return self.guess_num[round]
end
function GuildHonorBattleFighterInfo:CurrRound()
    return self.match_info.curr_round
end
function GuildHonorBattleFighterInfo:WinRank()
    return self.match_info.win_rank
end
function GuildHonorBattleFighterInfo:IsWin()
    local base = GuildEmbattleData.Instance:Base()
    local win_rank = self:WinRank()
    local curr_round = self:CurrRound()
    if base.guess_item[curr_round] and base.guess_item[curr_round][win_rank] then
        return base.guess_item[curr_round][win_rank][self.guild_id] or false
    end
    return false
end
function GuildHonorBattleFighterInfo:IsChampion()
    local base = GuildEmbattleData.Instance:Base()
    return base.champion_guild_id == self.guild_id
end
function GuildHonorBattleFighterInfo:DecodeRecord(index, info)
    self.index = index
    self.number = info.number
    self.server_id = info.server_id
    self.guild_id = info.guild_id
    self.name = info.name
    self.banner = info.banner
    self.role_name = info.role_name
    --族长名字
    -- self.rank = MsgAdapter.ReadInt()
    --家族大乱斗排名
    self.guild_fight_rank = info.guild_fight_rank

    self.match_round = info.match_round
    --到达的回合，和curr_round不同
    self.champion_guess_num = info.champion_guess_num
    self.guess_num = {}
    for i = 1, GUILD_HONOR_BATTLE_MAX_ROUND do
        self.guess_num[i] = info.guess_num[i]
        --每轮支持的人数
    end
end
function GuildHonorBattleFighterInfo:DecodeData(index, record)
    self.index = index
    self.number = MsgAdapter.ReadInt()
    self.server_id = MsgAdapter.ReadInt()
    self.guild_id = MsgAdapter.ReadInt()
    self.name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    self.banner = MsgAdapter.ReadStrN(PROTC_GUILD_BANNER)
    self.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    --族长名字
    self.rank = MsgAdapter.ReadInt()
    --家族大乱斗排名
    self.guild_fight_rank = self.rank

    self.match_round = MsgAdapter.ReadInt()
    --到达的回合，和curr_round不同
    if not record then
        self.is_finish = MsgAdapter.ReadChar()
        self.is_lose = MsgAdapter.ReadChar()
        --match_round回合是否完成战斗
        --match_round是不是输了
        self.last_match_rank = MsgAdapter.ReadUShort()
        --上次比赛排名
    end
    self.champion_guess_num = MsgAdapter.ReadInt()
    self.guess_num = {}
    for i = 1, GUILD_HONOR_BATTLE_MAX_ROUND do
        self.guess_num[i] = MsgAdapter.ReadInt()
        --每轮支持的人数
    end
    if not record then
        self.battle_id = {}
        for i = 1, GUILD_HONOR_BATTLE_MAX_ZONE do
            self.battle_id[i] = MsgAdapter.ReadInt()
            --每个区域的观战id
        end
        self.battle_result = {}
        for i = 1, GUILD_HONOR_BATTLE_MAX_ZONE do
            self.battle_result[i] = MsgAdapter.ReadInt()
            --每个区域的战斗结果，用来显示某个区域是否可观战
        end
    end
end
function GuildHonorBattleFighterInfo:HadJoin(curr_round)
    local guild_id = GuildData.Instance:GetGuildID()
    --match_round 小于当前说明已经止步
    if self.match_round < curr_round then
        return false
    end
    if self.guild_id == guild_id and self.is_finish == 0 and self.is_lose == 0 then
        --这里面还要判断是否有对手如果没有对手不需要进场的
        return true
    end
    return false
end
function GuildHonorBattleFighterInfo:Guessing()
end

GuildHonorBattleMatchInfo = GuildHonorBattleMatchInfo or BaseClass()
function GuildHonorBattleMatchInfo:__init()
    self.data = GuildEmbattleData.Instance
end
function GuildHonorBattleMatchInfo:__delete()
end
function GuildHonorBattleMatchInfo:DebugMsg()
    LogDG('___________________________________', self.win_rank)
    LogDG('争夺目标', self.win_rank, '出现回合', self.start_first_round)
    LogDG('当前回合', self.curr_round)
    LogDG('回合状态', self.round_state, 'finish = ', self.is_finish)
    LogDG('比赛家族数', self.fighter_num)
end

function GuildHonorBattleMatchInfo:DecodeRecord(info)
    self.fighter_num = table.nums(info.fighter_info)
    self.fighter_info = info.fighter_info
    self.curr_round = self.data:FinalRound(self.fighter_num)
    self.win_rank = info.win_rank
    self.start_first_round = info.start_first_round
    if self.win_rank ~= 1 then
        self.start_first_round = info.start_first_round + 1
    end
    table.sort(self.fighter_info, DataSorter.KeyLowerSorter("number"))
    
    local temp = {}
    for i, value in ipairs(self.fighter_info) do
        temp[i] = GuildHonorBattleFighterInfo.New(self)
        temp[i]:DecodeRecord(i, value)
        -- LogDG("出现",self.start_first_round, "家族", temp[i].name, "到达回合",temp[i].match_round,"当前回合", self.curr_round,"序列", temp[i].number,"争夺",self.win_rank)
    end
    for i = 1, 16 do
        self.fighter_info[i] = GuildHonorBattleFighterInfo.New(self)
        self.fighter_info[i].guild_id = 0
        self.fighter_info[i].match_round = 0
        self.fighter_info[i].number = i
    end
    for i = 1, self.fighter_num do
        self.fighter_info[temp[i].number] = temp[i]
    end
    -- LogError("===========curr_round=", self.curr_round,"===================win_rank=",self.win_rank,"=================fighter_num=", self.fighter_num)
    self:Grouping()
end

function GuildHonorBattleMatchInfo:BecomeRecord()
    self.curr_round = self.data:FinalRound(self.fighter_num)
    self.round_state = nil
    self.next_round_state_time = nil
    self.is_finish = nil
    self:Grouping()
end
function GuildHonorBattleMatchInfo:DecodeData(record)
    self.start_first_round = MsgAdapter.ReadInt() --这组开始时，胜利组回合
    --开始的阶段
    self.win_rank = MsgAdapter.ReadInt()
    if self.win_rank ~= 1 then
        self.start_first_round = self.start_first_round + 1
    end
    --胜利排名
    if not record then
        self.curr_round = MsgAdapter.ReadInt()
        --当前回合
        self.round_state = MsgAdapter.ReadInt()
        --0是准备1是开始
        self.next_round_state_time = MsgAdapter.ReadUInt()
        --回合结束时间
        self.is_finish = MsgAdapter.ReadInt()
    end
    self.fighter_num = MsgAdapter.ReadInt()
    self.fighter_info = {}
    local temp = {}
    local temp_list = {}
    LogDG('参赛名单============="争夺目标', self.win_rank, "=======================", '有效家族数', self.fighter_num)
    for i = 1, 16 do
        table.insert(temp_list, i)
    end
    for i = 1, 16 do --这里固定读16个
        temp[i] = GuildHonorBattleFighterInfo.New(self)
        temp[i]:DecodeData(i, record)
        if temp[i].number ~= 0 then
            temp_list[temp[i].number] = nil
        end
    end
    temp_list = table.rearrange(temp_list)
    for i, v in ipairs(temp) do
        if v.number == 0 then
            v.number = table.remove(temp_list)
        end
        self.fighter_info[v.number] = v
        LogDG(i, "家族", v.name, "到达回合",v.match_round,"当前回合", self.curr_round,"序列", v.number)
    end

    -- self.fighter_info[0] = nil
    LogDG('参赛名单结束=================================')
    --[[ for i = 1, 16 do
        if temp[i].number > 0 then
            self.fighter_info[temp[i].number] = temp[i]
        else
            self.fighter_info[i] = temp[i]
            self.fighter_info[i].guild_id = 0
            self.fighter_info[i].match_round = 0
            self.fighter_info[i].number = i
        end
    end ]]
    LogDG("整理比赛名单==========参赛数=",self.fighter_num,"====整理后占位数=",table.nums(self.fighter_info))
    temp = nil
    self:Grouping()
end
--进行分组
function GuildHonorBattleMatchInfo:Grouping()
    self.match_list = {}
    --存储战况列表
    for i = 1, 4 do
        self.match_list[i] = {}
    end
    --第一轮 2个家族一组存储
    local group
    --[[ if self.fighter_num == 2 then
        for index, value in ipairs(self.fighter_info) do
            group = index
            self.match_list[1][group] = self.match_list[1][group] or {}
            table.insert(self.match_list[1][group], value)
        end
    else ]]
        for index, value in ipairs(self.fighter_info) do
            group = math.ceil(value.number / 2)
            self.match_list[1][group] = self.match_list[1][group] or {}
            table.insert(self.match_list[1][group], value)
        end
    --[[ end ]]
    --上一轮 出线家族存储
    local temp
    for round = 2, 4 do
        for i, v in ipairs(self.match_list[round - 1]) do
            group = math.ceil(i / 2)
            temp = nil
            if v[1] and v[2] and v[1].match_round > v[2].match_round then
                temp = v[1]
            elseif v[1] and v[2] and v[1].match_round < v[2].match_round then
                temp = v[2]
            elseif v[1] and v[2] == nil and v[1].match_round >= self.curr_round then
                temp = v[1]
            end
            -- if temp then
                self.match_list[round][group] = self.match_list[round][group] or {}            
                table.insert(self.match_list[round][group], temp)
            -- end
        end
    end
    -- LogDG("争夺目标", self.win_rank)
end
--当前对战我方家族的对手信息
function GuildHonorBattleMatchInfo:OpponetInfo(guild_id)
    guild_id = guild_id or GuildData.Instance:GetGuildID()
    local match_list = self.match_list[self.curr_round]
    if match_list then
        for _, group in ipairs(match_list) do
            if group then
                for _, fighter in ipairs(group) do
                    if fighter.guild_id == guild_id then
                        return group
                    end
                end
            end
        end
    end
end
--找出match_round最大的那个家族就是排名win_rank的家族
function GuildHonorBattleMatchInfo:WinRankChampion()
    local match_round = -1
    local result = 0
    for index, value in ipairs(self.fighter_info) do
        if value.match_round > match_round then
            result = index
            match_round = value.match_round
        end
    end
    if self.fighter_num == 1 then
        self.fighter_info[1].win_rank = self.win_rank
        return self.fighter_info[1]
    end
    self.fighter_info[result].win_rank = self.win_rank
    -- LogDG(self.win_rank, self.fighter_info[result].name, match_round)
    return self.fighter_info[result]
end
--根据当前选择阶段返回对战表
function GuildHonorBattleMatchInfo:MatchList()
    --非对战组不应该显示，即已经确定好名词的组别不需要显示到对战列表里
    local round = self:Round()
    -- LogDG(self.win_rank,"对战表显示回合", round)
    if self.match_list[round] == nil then
        return {}
    end
    local list = {}
    for i, v in ipairs(self.match_list[round]) do
        if v[1] and v[1].guild_id > 0 and v[2] then
            table.insert(list, v)
        end
    end
    return list--self.match_list[round] or {}
    --每个组的当前回合都他妈是不一样的
end
--各组的起始阶段
function GuildHonorBattleMatchInfo:InitStage()
    --这里的又有问题
    return self:WinnerStage() + self.start_first_round - 1
    --[[ local stage = 1
    if self.fighter_num > 8 then
        stage = 1
    elseif self.fighter_num > 4 then
        stage = 2
    elseif self.fighter_num > 2 then
        stage = 3
    elseif self.fighter_num > 0 then
        stage = 4
    end
    return stage ]]
end
--胜利组的初始阶段
function GuildHonorBattleMatchInfo:WinnerStage()
    local stage = 1
    if self.data.base.fighter_num > 8 then
        stage = 1
    elseif self.data.base.fighter_num > 4 then
        stage = 2
    elseif self.data.base.fighter_num > 2 then
        stage = 3
    elseif self.data.base.fighter_num > 0 then
        stage = 4
    end
    return stage
end
--能进行几轮
function GuildHonorBattleMatchInfo:MaxRound()
    return 5 - self:InitStage()
end
--根据选择计算的显示的round，round=0不显示
function GuildHonorBattleMatchInfo:Round(select_stage)
    local round
    if self.win_rank == 1 then
        round = self:WinnerRound(select_stage)
    else
        round = self:LoserRound(select_stage)
    end
    return round
end
function GuildHonorBattleMatchInfo:WinnerRound(select_stage)
    select_stage = select_stage or self.data:ViewData().stage--这个变量
    local init_stage = self:WinnerStage()
    local round = 0
    if select_stage >= init_stage then
        round = select_stage - init_stage + 1
    end
    if self.win_rank == 1 then
        LogDG("胜者组需要显示第",round,"回合的数据，总共有",self:MaxRound(),"回合有效数据")
    end
    return round
end
function GuildHonorBattleMatchInfo:LoserRound(select_stage)
    local win_round = self:WinnerRound(select_stage)
    if win_round < self.start_first_round then--说明该组未出现
        return 0
    end
    local round = win_round - (self.start_first_round - self.data:Base().start_first_round)
    local max_round = self:MaxRound()
    if round > max_round then--说明该组已经结束对战
        return 0
    end
    LogDG("败者组",self.win_rank,"需要显示第",round,"回合的数据,总共有",max_round,"回合有效数据")
    return round
end
function GuildHonorBattleMatchInfo:FinalMatchList()
    local round = self:Round(4)
    --将无效对战组制空
    if self.match_list[round] == nil then
        return {}
    end
    local list = {}
    for i, v in ipairs(self.match_list[round]) do
        if v[1] and v[1].guild_id > 0 and v[2] then
            table.insert(list, v)
        end
    end
    return list
    -- return self.match_list[round] or {}
end
function GuildHonorBattleMatchInfo:IsWinGroup()
    return self.win_rank == 1
end
function GuildHonorBattleMatchInfo:HadJoin()
    local guild_id = GuildData.Instance:GetGuildID()
    if guild_id == 0 then
        return false
    end
    local had_join = false
    for index, value in ipairs(self.fighter_info) do
        had_join = value:HadJoin(self.curr_round)
        if had_join == true then
            break
        end
    end
    return had_join
end
--自己家族是否有对战且准备战斗
function GuildHonorBattleMatchInfo:WillBattle()
    if self.round_state == 0 and self:HadJoin() then
        return true
    end
    return false
end
--参赛资格
function GuildHonorBattleMatchInfo:EntryQualify()
    local guild_id = GuildData.Instance:GetGuildID()
    if guild_id == 0 then
        return false
    end
    local entry = false
    for index, value in ipairs(self.fighter_info) do
        entry = (value.guild_id == guild_id)
        if entry == true then
            break
        end
    end
    return entry
end


















GuildHonorBattleMatchRecordInfo = GuildHonorBattleMatchRecordInfo or BaseClass()
function GuildHonorBattleMatchRecordInfo:__init()
end
function GuildHonorBattleMatchRecordInfo:DecodeData()
    self.start_first_round = MsgAdapter.ReadInt()
    self.win_rank = MsgAdapter.ReadInt()
    self.fighter_num = MsgAdapter.ReadInt()
    self.fighter_info = {}
    for i = 1, self.fighter_num do
        self.fighter_info[i] = self:FighterItemInfo()
    end
end
function GuildHonorBattleMatchRecordInfo:FighterItemInfo()
    local t = {}
    t.number = MsgAdapter.ReadInt()
    t.server_id = MsgAdapter.ReadInt()
    t.guild_id = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    t.banner = MsgAdapter.ReadStrN(PROTC_GUILD_BANNER)
    t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    --族长名字
    --t.rank = MsgAdapter.ReadInt()--家族大乱斗排名
    t.guild_fight_rank = MsgAdapter.ReadInt()
    --家族大乱斗排名
    t.match_round = MsgAdapter.ReadInt()
    --到达的回合，和curr_round不同
    -- t.match_round_2 = MsgAdapter.ReadInt()
    t.champion_guess_num = MsgAdapter.ReadInt()
    t.guess_num = {}
    -- t.ranking = idx
    for i = 1, GUILD_HONOR_BATTLE_MAX_ROUND do
        t.guess_num[i] = MsgAdapter.ReadInt()
        --每轮支持的人数
    end
    -- if t.rank == 0 then
    --     t.rank = 17
    -- end
    return t
end
--找出match_round最大的那个家族就是排名win_rank的家族
function GuildHonorBattleMatchRecordInfo:WinRankChampion()
    local match_round = -1
    local result = 0
    for index, value in ipairs(self.fighter_info) do
        if value.match_round > match_round then
            result = index
            match_round = value.match_round
        end
    end
    -- LogDG(self.fighter_info[result])
    return self.fighter_info[result]
end
