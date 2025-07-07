
-- 考虑到G6逻辑比较多变 支持通过id分类不同的剧情对象
local StoryIdToInfo = {
    -- [1] = StoryInfoShowBoss1,
    [2] = StoryInfoShowBoss2,
    [3] = StoryInfoShowBoss3,
    [104] = StoryInfoShowBoss104,
    -- [4] = StoryInfoTaskHC,
    [4] = StoryInfoTaskHCShow,
    [101] = StoryInfoTrans1002,
    -- [102] = StoryInfo1002Skill1,
    -- [103] = StoryInfo1002Skill2,

    [105] = StoryInfoXingTianChuChang,
    [106] = StoryInfoOriginBattleStart,
    [107] = StoryInfoOriginBattleSkill,
    [201] = StoryInfoPoetry,
    [108] = StoryInfoXinMoPre,
    [109] = StoryInfoWedding,
    [110] = StoryInfoShiTuChuangGong,

    [121] = StoryInfoHarmonyHeavens,
    [122] = StoryInfoHarmonyHeavens,
    [123] = StoryInfoHarmonyHeavens,
    [124] = StoryInfoHarmonyHeavens,
    [125] = StoryInfoHarmonyHeavens,
    [126] = StoryInfoHarmonyHeavens,
    [127] = StoryInfoHarmonyHeavens,
    [128] = StoryInfoHarmonyHeavens,
    [129] = StoryInfoHarmonyHeavens,
    [130] = StoryInfoHarmonyHeavens,
    [131] = StoryInfoHarmonyHeavens,
    [132] = StoryInfoHarmonyHeavens,
    [133] = StoryInfoHarmonyHeavens,
    [134] = StoryInfoHarmonyHeavens,
    [135] = StoryInfoHarmonyHeavens,
    [136] = StoryInfoHarmonyHeavens,
    [137] = StoryInfoHarmonyHeavens,
    [138] = StoryInfoHarmonyHeavens,


    [141] = StoryInfoHarmonyHeavensShow,
    [142] = StoryInfoHarmonyHeavensShow,
    [143] = StoryInfoHarmonyHeavensShow,
    [144] = StoryInfoHarmonyHeavensShow,
    [145] = StoryInfoHarmonyHeavensShow,
    [146] = StoryInfoHarmonyHeavensShow,
    [147] = StoryInfoHarmonyHeavensShow,
    [148] = StoryInfoHarmonyHeavensShow,
    [149] = StoryInfoHarmonyHeavensShow,

}

-- local StoryTypeToInfo = {
--     [1] = StoryInfo1,
-- }

StoryData = StoryData or BaseClass()

function StoryData:__init()
    if StoryData.Instance ~= nil then
        Debuger.LogError("[StoryData] attempt to create singleton twice!")
        return
    end
    StoryData.Instance = self
    
    self:StoryConfig()
    self:StoryOptionsConfig()
    self:StoryData()
    self:StoryOptionsData()
end

function StoryData:__delete()
end

function StoryData:StoryConfig()
    self.config = Config.story_auto
    self.config_story_list = self.config.story_list
    self.config_scene_patrol = self.config.scene_patrol

    Cfg.Register("StoryByTypeParam1", Cfg.RealTimeSearch2Key(self.config_story_list, "type", "param1"))
    self.soldier_cfg = {}
    for _, value in pairs(self.config_scene_patrol) do
        self.soldier_cfg[value.scene_id] = self.soldier_cfg[value.scene_id] or {}
        table.insert(self.soldier_cfg[value.scene_id], value)
    end
end

function StoryData:StoryData()
    self.story_list = {}
    for _, co in pairs(self.config_story_list) do
        -- self.story_list[co.id] = (StoryIdToInfo[co.id] or StoryTypeToInfo[co.type] or StoryInfo).New(self, co)
        self.story_list[co.id] = StoryIdToInfo[co.id] and StoryIdToInfo[co.id].New(self, co) or nil
    end

    self.cur_story = StoryConfig.StoryInfoNone

    self.story_event = SmartData.New()
    self.story_pre_plays = {}
end

function StoryData:StoryState()
    return StoryConfig.StoryInfoNone == self.cur_story and StoryConfig.State.End or self.cur_story:State(), self.cur_story
end

function StoryData:IsPlaying()
    if StoryConfig.StoryInfoNone == self.cur_story then
        return false
    else
        return self.cur_story:IsPlaying()
    end
end

function StoryData:StartStory(story_info)
    self.cur_story = story_info:StartStory()
end

function StoryData:StartStoryById(story_id, trigger_type)
    local story_info = self.story_list[story_id]
    if story_info and story_info:needTrigger(trigger_type) then
        self:StartStory(story_info)
        return story_info
    end
end

function StoryData:PlayStoryByStoryId(story_id)
    local story_info = self.story_pre_plays[story_id]
    if story_info then
        self.cur_story = story_info
        story_info:Play()
        self.story_pre_plays[story_id] = nil
        return story_info
    end
end

function StoryData:StartStoryByMgi(monster_group_id, trigger_type)
    local co = Cfg.StoryByTypeParam1(StoryConfig.StoryType.show_boss, monster_group_id, false)
    if co then
        if 106 == co.id then return BattleData.Instance:Round() == 0 end	--TEMP_CODE临时代码，106号剧情的触发条件是1001怪物组+回合数是0 
        -- if 121 == co.id then return BattleData.Instance:Round() == 0 end	--TEMP_CODE临时代码，121号剧情的触发条件是40710怪物组+回合数是0 
        if StoryConfig.SpecialStory[co.id] then return BattleData.Instance:Round() == 0 end
        return self:StartStoryById(co.id, trigger_type)
    end
end


function StoryData:StartStoryByTaskId(task_id)
    local co = Cfg.StoryByTypeParam1(StoryConfig.StoryType.task, task_id, false)
    if co then return self:StartStoryById(co.id) end
end

function StoryData:StartStoryByHHShowScene(scene_id)
    local co = Cfg.StoryByTypeParam1(StoryConfig.StoryType.hh_show, scene_id, false)
    if co then return self:StartStoryById(co.id) end
end

function StoryData:SoldierCfg(scene_id)
    scene_id = scene_id or SceneData.Instance:SceneId()
    return self.soldier_cfg[scene_id]
end

function StoryData:RecordStoryPrePlay(story_id, story_info)
    self.story_pre_plays[story_id] = story_info
end