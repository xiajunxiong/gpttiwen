-- >>>>>>>>>>>>>> 主要为了归纳协议数据，规范命名,方便查询 <<<<<<<<<<<<<<

-- ============ 竞猜投注信息 =============
GuessBetInfoClass = GuessBetInfoClass or BaseClass()
-- vo :::: 4969协议SCUTAGuessMatchInfo.info.team_bet_info
function GuessBetInfoClass:__init(info)
    info = info or {}
    self.info = info
    self.match_id = info.match_id or 0      --比赛id
    self.match_id_t = info.match_id_t
    self.team_id = info.team_id or 0        --队伍id
    self.team_id_t = info.team_id_t        --用于查询的
    self.win_count = info.win_count or 0    --最高连胜
    self.win_rate = info.win_rate or "0%"   --获取本届胜率统计
    self.bet_num = info.bet_num or 0        --获取玩家投注的金额
    self.player_bet_num = info.player_bet_num   --获取自己投注了多少金额
    self.guess_status = info.guess_status   --获取竞猜状态UTACfg.GuessStatus
    self.team = info.team or {}             --队伍信息UTAMatchTeamClass
end

--获取头像
function GuessBetInfoClass:GetHeadId()
    return self.team.head_id or 0
end

--角色类型
function GuessBetInfoClass:GetAvatarType()
    return self.team.avatar_type
end

--获取战队名
function GuessBetInfoClass:GetTeamName()
    return self.team.team_name or ""
end

--获取阵容信息
function GuessBetInfoClass:GetTeamKey()
    return self.team.team_id_t or {}
end

--获取全部队员的职业列表
function GuessBetInfoClass:GetProfList()
    return self.info.prof_list or {}
end

--获取服务器名
function GuessBetInfoClass:GetServerName()
    local server_id = self.info.server_id
    local plat_id = self.info.plat_id
    return UniverseTeamArenaData.Instance:GetServerName(server_id, plat_id)
end

--获取赔率 num == 另一方投注了多少
function GuessBetInfoClass:GetRate(num)
    num = num or self.info.side_bet_num or 0
    return UniverseTeamArenaData.Instance:CalculateGuessRate(num)
end


-- =============== 比赛队伍 ============
UTAMatchTeamClass = UTAMatchTeamClass or BaseClass()
function UTAMatchTeamClass:__init(info)
    info = info or {}
    self.info = info
    self.team_id = info.team_id     -- 队伍id
    self.team_id_t = info.team_id_t
    self.team_name = info.team_name -- 队伍名字
    self.plat_type = info.plat_type -- 渠道号
    self.server_id = info.server_id -- 服务器id 队长所在的服务器
    self.avatar_type = info.avatar_type
    self.head_id = info.head_id     -- 队伍头像 （就是队长的头像）
    self.cur_score = info.cur_score or 0        -- 当前积分
    self.support_rate = info.support_rate or 0  -- 支持率 
end

function UTAMatchTeamClass:Info()
    return self.info
end

function UTAMatchTeamClass:ServerName()
    return UniverseTeamArenaData.Instance:GetServerName(self.server_id, self.plat_type) or ""
end

function UTAMatchTeamClass:IsNull()
    local team_id = tonumber(self.team_id)
    return team_id == 0
end

-- ================ 比赛 ===============
UTAMatchClass = UTAMatchClass or BaseClass()
function UTAMatchClass:__init(info)
    info = info or {}
    self.info = info
    self.match_id = info.match_id           -- 比赛id
    self.match_id_t = info.match_id_t
    self.fight_result = info.fight_result or UTACfg.FightResult.Nothing   -- 战斗结果 === UTACfg.FightResult
    self.video_id = info.video_id or 0      -- 录像id   -- 可为空
    self.video_id_t = info.video_id_t or {}
    self.atk_team = info.atk_team           -- 攻击队伍信息 === UTAMatchTeamClass
    self.def_team = info.def_team           -- 防守队伍信息 === UTAMatchTeamClass
    self.show_index = info.show_index

    self.match_type = info.match_type or UTACfg.MatchType.Out      -- 比赛类型，为空就是资格淘汰赛
end

function UTAMatchClass:Info()
    return self.info
end

function UTAMatchClass:IsCanGuess()
    if self.atk_team:IsNull() then
        return false
    end
    if self.def_team:IsNull() then
        return false
    end
    return true
end