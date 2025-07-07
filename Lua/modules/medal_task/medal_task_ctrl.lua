FILE_REQ("modules/medal_task/medal_task_data")
VIEW_REQ("modules/medal_task/medal_task_view")
VIEW_REQ("modules/medal_task/medal_task_help")
VIEW_REQ("modules/medal_task/medal_task_start")
MedalTaskCtrl = MedalTaskCtrl or BaseClass(BaseCtrl)

function MedalTaskCtrl:__init()
    if MedalTaskCtrl.Instance ~= nil then
        Debuger.LogError("[MedalTaskCtrl] attempt to create singleton twice!")
        return
    end
    MedalTaskCtrl.Instance = self

    self.data = MedalTaskData.New()

    self:RegisterAllProtocols()
    -- self.handle_completed = TaskData.Instance:CareCompletedTaskChange(BindTool.Bind(self.OnTaskCompletedChange, self))
end 

function MedalTaskCtrl:__delete()
    TaskData.Instance:UnCareAcceptedTaskChange(self.handle_completed)
end

function MedalTaskCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCMedalFbKillBoss, "OnMedalFbKillBoss")
    self:RegisterProtocol(CSMedalTaskPlayVideo)
end

function MedalTaskCtrl:OnMedalFbKillBoss(protocol)
    self.data:SetMedalFbBossKilled(protocol)
end
function MedalTaskCtrl:SendMedalTaskPlayVideo(id)
    local protocol = GetProtocol(CSMedalTaskPlayVideo)
    protocol.video_id = id
    SendProtocol(protocol)
end
function MedalTaskCtrl:OnTaskCompletedChange(params)
    if next(params.finish_list) then
        for key, value in pairs(params.finish_list) do
            local info = self.data:GetSpReward(nil, key + 1)--注意
            if info ~= nil then
                local item = Item.Create(info.item_list[0])
                ViewMgr:OpenView(TreasureMapShow, {item = item, close_func = function ()
                    ConspiracyData.Instance:SetOpenTask(key + 1)
                    -- ViewMgr:OpenView(ConspiracyModrons)
                end})
            end
        end
    end
end