
MusicGameData = MusicGameData or BaseClass()

function MusicGameData:__init()
    if MusicGameData.Instance ~= nil then
        Debuger.LogError("[MusicGameData] attempt to create singleton twice!")
        return
    end
    MusicGameData.Instance = self
    
    self:MusicGameConfig()
    self:MusicGameData()


    Cfg.Register("MusicGameMusicById",Cfg.RealTimeSearch(Config.music_game_auto.music, "music_id"))
    Cfg.Register("MusicGameButtonByType",Cfg.RealTimeSearch(Config.music_game_auto.button_type, "type"))
end

function MusicGameData:MusicGameConfig()
    self.score_determine_list = {}
    for k, v in ipairs(Config.music_game_auto.score_determine) do
        table.insert(self.score_determine_list, v)
    end
    table.sort(self.score_determine_list, function (a, b)
        if not a or not b then return a end
        return a.score < b.score
    end)

    self.score_add_scale_list = {}
    for k, v in ipairs(Config.music_game_auto.mentor_addition) do
        table.insert(self.score_add_scale_list, v)
    end
    table.sort(self.score_add_scale_list, function (a, b)
        if not a or not b then return a end
        return a.score > b.score
    end)
end

function MusicGameData:MusicGameData()
    self.music_game_sm = SmartData.New{
        score = 0,
        scale = 1,
        combo = 0,
        state = MusicGameConfig.GameState.Ready,
        cur_time = 0,
    }
    self.music_button_pos_list = {}
end

function MusicGameData:ResetMusicGameSM()
    self.music_game_sm:Set{
        score = 0,
        scale = 1,
        combo = 0,
        state = MusicGameConfig.GameState.Ready,
        cur_time = 0
    }
end

function MusicGameData:MusicGame(id, is_test)
    local music = Cfg.MusicGameMusicById(id)
    if music then
        self:ResetMusicGameSM()
        self.is_test = is_test
        self.cur_music = MusicGameInfo.New(music)
        return true
    else
        return false
    end
end


function MusicGameData:CurMusic()
    return self.cur_music
end

function MusicGameData:MusicButtonPosListById(id)
    if nil == self.music_button_pos_list[id] then
        local list = {}
        for k, v in ipairs(Config.music_game_auto.button_pos) do
            if id == v.music_id then
                list[v.time] = v
            end
        end
        self.music_button_pos_list[id] = list
    end
    return self.music_button_pos_list[id]
end

function MusicGameData:ClickShow(judge_time)
    local is_before = judge_time < 0
    judge_time = math.abs(judge_time)
    local score_co = self.score_determine_list[1]
    for k, v in ipairs(self.score_determine_list) do
        if judge_time < (is_before and v.before or v.after) then
            score_co = v
        end
    end
    self:JudgeScore(score_co)
    return score_co.type
end

function MusicGameData:ClickShow2(judge_time)
    local is_miss = judge_time > 0
    local score_co = self.score_determine_list[1]
    if not is_miss then
        judge_time = math.abs(judge_time)
        for k, v in ipairs(self.score_determine_list) do
            if judge_time < v.before then
                score_co = v
            end
        end
    end
    self:JudgeScore(score_co)
    return score_co.type
end

function MusicGameData:ClickShowMiss()
    local score_co = self.score_determine_list[1]
    self:JudgeScore(score_co)
    return score_co.type
end


function MusicGameData:JudgeScore(score_co)
    local is_combo = self.cur_music:IsCombo(score_co)
    self.music_game_sm.score = self.music_game_sm.score + score_co.score * (is_combo and self.cur_music:ComboScale() or 1)
    self.music_game_sm.combo = is_combo and (self.music_game_sm.combo + 1) or 0

    -- 上报分数
    if not self.is_test and score_co.score > 0 then
        self.cur_music:Report(self.music_game_sm.score, self.music_game_sm.scale)
    end
end

function MusicGameData:SetScoreAddScale(score)
    local scale = 1
    local score_show = 0
    for k, v in ipairs(self.score_add_scale_list) do
        if score >= v.score then
            score_show = v.score
            scale = 1 + v.add_percentage / 100
            break
        end
    end
    self.cur_music:AddScaleTips(score_show, self.music_game_sm.scale, scale)
    self.music_game_sm.scale = scale
end