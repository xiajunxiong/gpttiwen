
VideoData = VideoData or BaseClass()

function VideoData:__init()
    if VideoData.Instance ~= nil then
        Debuger.LogError("[VideoData] attempt to create singleton twice!")
        return
    end
    VideoData.Instance = self
    self.video_smart = SmartData.New({fade_in_end = false})
    self:VideoConfig()
    self:VideoData()
    self:initBulletConfig()
end

function VideoData:__delete()
end

function VideoData:VideoConfig()
    self.config = Config.story_auto.animation_story or {}
end

function VideoData:VideoData()
    self.video_list = {}
    for _, co in pairs(self.config) do
        self.video_list[co.id] = VideoInfo.New(co, self)
    end

    self.is_playing = false
end

function VideoData:initBulletConfig()
    self.bullet_cfg_list = {}
    for k, v in pairs(Config.story_auto.bullet_screen) do
        self.bullet_cfg_list[v.bullet_screen_group] = self.bullet_cfg_list[v.bullet_screen_group] or {}
        self.bullet_cfg_list[v.bullet_screen_group][v.seq] = v.bullet_screen
    end
end

function VideoData:StartVideoById(id, end_func)
    local video = self.video_list[id]
    if video then
        video:SetEndFunc(end_func)
        self.cur_video = video
        video:Play()
    end
end

function VideoData:VideoCfgByTrigger(trigger_type, param1, param2, param3)
    for _, video in pairs(self.config) do
        if video.type == trigger_type then
            if video.param1 == param1 then
                return video
            end
        end
    end
    return nil
end

function VideoData:GetBulletInfoByGroupId(group_id)
    return self.bullet_cfg_list[group_id]
end