
FILE_REQ("modules/video/video_config")
FILE_REQ("modules/video/video_info")
FILE_REQ("modules/video/video_data")

VIEW_REQ("modules/video/video_view")

VideoCtrl = VideoCtrl or BaseClass(BaseCtrl)

function VideoCtrl:__init()
    if VideoCtrl.Instance ~= nil then
        Debuger.LogError("[VideoCtrl] attempt to create singleton twice!")
        return
    end
    VideoCtrl.Instance = self
    
    self.data = VideoData.New()

    self:RegisterAllProtocols()
end

function VideoCtrl:RegisterAllProtocols()
end

function VideoCtrl:OnInit()
    -- self.handle_accepted = TaskData.Instance:CareAcceptedTaskChange(BindTool.Bind(self.OnTaskAcceptedChange, self))
    self.handle_completed = TaskData.Instance:CareCompletedTaskChange(BindTool.Bind(self.OnTaskCompletedChange, self))
end

function VideoCtrl:OnUnloadGameLogic()
   
end

function VideoCtrl:StartVideoById(id, end_func)
    if IS_MICRO_PLAYER() then      --TEMP_CODE 暂时关掉视频功能，需要兼容
        if end_func then
            Call(end_func)
        end
    else
        self.data:StartVideoById(id, end_func)
    end
end

function VideoCtrl:OnTaskAcceptedChange(old)
    -- if BattleData.Instance:BaseData().server_fighting == true then
    --     return
    -- end
    local list = TaskData.Instance:AcceptedTask()
    for task_id, _ in pairs(list) do
        local video = self.data:VideoCfgByTrigger(VideoConfig.TriggerType.AcceptTask, task_id)
        if video then
            self:StartVideoById(video.id)
            break
        end
    end
end
function VideoCtrl:OnTaskCompletedChange(old)
    -- if BattleData.Instance:BaseData().server_fighting == true then
    --     return
    -- end
    local list = TaskData.Instance:FinishTask()
    for task_id, _ in pairs(list) do
        local video = self.data:VideoCfgByTrigger(VideoConfig.TriggerType.FinishTask, task_id)
        if video then
            self:StartVideoById(video.id)
            break
        end
    end
end