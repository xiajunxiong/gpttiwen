
MusicStationData = MusicStationData or BaseClass()

function MusicStationData:__init()
    if MusicStationData.Instance ~= nil then
        Debuger.LogError("[MusicStationData] attempt to create singleton twice!")
        return
    end
    MusicStationData.Instance = self
    
    self:MusicStationConfig()
    self:MusicStationData()


    Cfg.Register("MusicStationMusicById",Cfg.RealTimeSearch(Config.audio_game_auto.music, "music_id"))
    Cfg.Register("MusicStationButtonByType",Cfg.RealTimeSearch(Config.audio_game_auto.button_type, "type"))

    self.editor = SmartData.New{total_time = 6300}

end

function MusicStationData:MusicStationConfig()
    self.score_determine_list = {}
    for k, v in ipairs(Config.audio_game_auto.score_determine) do
        table.insert(self.score_determine_list, v)
    end
    table.sort(self.score_determine_list, function (a, b)
        if not a or not b then return a end
        return a.score < b.score
    end)

    self.link_click_cache = {}
end

function MusicStationData:MusicStationData()
    self.music_station_sm = SmartData.New{
        score = 0,
        scale = 1,
        combo = 0,
        state = MusicStationConfig.GameState.Ready,
        cur_time = 0,
        life = 0,
    }
    self.music_button_pos_list = {}

    self.chapter_info = SmartData.New{
        chapter = 0,
    }

    self.reset_mark = false
end

function MusicStationData:ResetMark()
    self.reset_mark = true
end

function MusicStationData:ResetMusicStationSM()
    self.music_station_sm:Set{
        score = 0,
        scale = 1,
        combo = 0,
        state = MusicStationConfig.GameState.Ready,
        cur_time = 0,
        life = Config.audio_game_auto.other[1].hp,
    }

    self.reset_mark = false
end

function MusicStationData:FlushLife()
    -- 扣血
    self.music_station_sm.life = self.music_station_sm.life - Config.audio_game_auto.other[1].an_error
end

function MusicStationData:MusicStation(start)
    if self.music_list == nil then 
        self.music_list = {}
        for i = 1,#Config.audio_game_auto.music do 
            self.music_list[i] = MusicStationInfo.New(Config.audio_game_auto.music[i])
        end 
    end 

    -- self:ResetMusicStationSM()
    if self.music_list[start] ~= nil then 
        self.cur_music = self.music_list[start]
        self.chapter_info.chapter = start
        return true
    else 
        return false
    end     
end

function MusicStationData:FlushChapter(is_success) 
    if is_success then 
        self.chapter_info.chapter = self.chapter_info.chapter + 1
        if self.chapter_info.chapter > #self.music_list then 
            self.chapter_info.chapter = 1
        end 
    end 

    self:ResetMusicStationSM()
    self.cur_music = self.music_list[self.chapter_info.chapter]
end 

function MusicStationData:NeedJump()
    local flag =  self.reset_mark 
    self.reset_mark = false
    return not flag and self.music_station_sm.life > 0
end

function MusicStationData:CurMusic()
    return self.cur_music
end

function MusicStationData:MusicButtonPosListById(id)
    if nil == self.music_button_pos_list[id] then
        local list = {}
        for k, v in ipairs(Config.audio_game_auto.button_pos) do
            if id == v.music_id then
                if list[v.time] == nil then list[v.time] = {} end 
                table.insert(list[v.time],v)
            end
        end
        self.music_button_pos_list[id] = list
    end
    return self.music_button_pos_list[id]
end

function MusicStationData:ClickShow(judge_time)
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

function MusicStationData:ClickShow2(judge_time)
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

function MusicStationData:ClickShow3(judge_time,data)
    -- 
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
    self:CheckLinkComplete(data,score_co)
end

function MusicStationData:CheckLinkComplete(data,score_co)
    if data.time == nil or data.link == nil or data.seq == nil then return end 
    -- 赋值操作
    local is_pass = score_co.type > 0

    self.link_click_cache[data.time][data.link][data.seq].score_co = score_co
    self.link_click_cache[data.time][data.link][data.seq].is_pass = is_pass
    
    local is_complete = true
    -- 完成赋值操作，检查同time同link的button是否都完成了
    for k,v in pairs(self.link_click_cache[data.time][data.link]) do 
        -- 失败了给所有的button广播，发失败特效
        if not is_pass then 
            v.mono:JudgeShow(0)
        -- 没失败就统计完成数
        else 
            if not v.is_pass then 
                is_complete = false
            end 
        end 
    end 

    if not is_pass then 
        self:JudgeScore(score_co)
        return 
    end 

    if is_complete then 
        for k,v in pairs(self.link_click_cache[data.time][data.link]) do 
            v.mono:JudgeShow(v.score_co.type)
        end 
        self:JudgeScore(score_co)
    end 
end

function MusicStationData:ClickShowMiss()
    local score_co = self.score_determine_list[1]
    self:JudgeScore(score_co)
    return score_co.type
end


function MusicStationData:JudgeScore(score_co)
    local is_combo = self.cur_music:IsCombo(score_co)
    self.music_station_sm.score = self.music_station_sm.score + score_co.score * (is_combo and self.cur_music:ComboScale() or 1)
    self.music_station_sm.combo = is_combo and (self.music_station_sm.combo + 1) or 0

    if score_co.type == 0 then
        self:FlushLife()
    end 
end

function MusicStationData:SaveProgress()
    UnityPlayerPrefs.SetInt("MusicStataion_progress",self.chapter_info.chapter)
end
function MusicStationData:LoadProgress()
    local pos = UnityPlayerPrefs.GetInt("MusicStataion_progress")
    return pos == 0 and 1 or pos
end

function MusicStationData:MarkLink(pos,mono)
    -- self.link_click_cache = 
    if pos.type ~= 6 then return end 
    if self.link_click_cache[pos.time] == nil then
        self.link_click_cache[pos.time] = {}
    end 

    if self.link_click_cache[pos.time][pos.link] == nil then
        self.link_click_cache[pos.time][pos.link] = {}
    end 

    local vo = {}
    vo.link = pos.link
    vo.time = pos.time
    vo.mono = mono

    vo.is_pass = false
    vo.score_co = nil

    self.link_click_cache[pos.time][pos.link][pos.seq]=vo
end

function MusicStationData:ClearMarkLink(data)
    if data.time == nil or data.link == nil or data.seq == nil then return end 

    self.link_click_cache[data.time][data.link] = nil
end