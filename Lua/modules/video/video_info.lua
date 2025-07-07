
VideoInfo = VideoInfo or BaseClass()
function VideoInfo:__init(co, data)
    self.data = data
    self:VideoInfo(co)
end

function VideoInfo:VideoInfo(co)
    self.info = co
end

function VideoInfo:Play()
    if VersionTag.HasTag(VersionTag.ShowCGSupport) then
        self.data.is_playing = true
        ViewMgr:OpenView(VideoView)
    else
        self:EndVideo()
    end
end

function VideoInfo:EndVideo()
    ViewMgr:CloseView(VideoView)
    self.is_playing = false
    if self.end_func then
        Call(self.end_func)
        self.end_func = nil
    end
    self:CheckAfterVideoEnd()
end

function VideoInfo:ResPath()
    return DrawerHelper.GetVideoPath(self.info.res_name)
end

function VideoInfo:CanSkip()
    return self.info.can_skip
end

function VideoInfo:SetEndFunc(end_func)
    self.end_func = end_func
end

function VideoInfo:GetBulletData()
    if self.info.bullet_screen_group > 0 then
        if self.cfg_bullet == nil then
            self.cfg_bullet = VideoData.Instance:GetBulletInfoByGroupId(self.info.bullet_screen_group)
        end
        local function add_to_t(t, key, v)
            if t[key] ~= nil then
                key = key + 1
                add_to_t(t, key, v)
            else
                t[key] = v
            end
        end
        local custom_str_seqs = {}
        local custom_str_nums = 0
        local prefabKey = PrefKeys.StoryCustomBullet()..self.info.id
        for i = 1, 30 do
            local str = UnityPlayerPrefs.GetString(prefabKey..i,"")
            if not StringIsEmpty(str) then
                local rev_str = string.reverse(str)                 --翻转字符串
                local split_index = string.find(rev_str,"_")        --取翻转后的第一个
                local seq = string.reverse(string.sub(rev_str, 1, split_index - 1))
                rev_str = string.sub(rev_str, split_index + 1, -1)
                add_to_t(custom_str_seqs, tonumber(seq), string.reverse(rev_str))
                custom_str_nums = custom_str_nums + 1
            else
                break
            end
        end
        custom_str_nums = custom_str_nums >= 30 and 0 or custom_str_nums
        return self.cfg_bullet, custom_str_seqs, custom_str_nums
    end
end

function VideoInfo:CheckAfterVideoEnd()
    if self.info.type == VideoConfig.TriggerType.FinishTask then
        local task = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.mainline)
        -- if task ~= nil then
        --     if UnityPlayerPrefs.GetInt(PrefKeys.VideoGuide()) == 1 then
        --         task:GoTask()
        --     else
        --         UnityPlayerPrefs.SetInt(PrefKeys.VideoGuide(), 1)
        --     end
        -- end
        if task ~= nil and task:TaskMenuConditionPause() then
            task:GoTask()
        end
    end
end