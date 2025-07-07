local timelineHandle = Game.TimelineHandle
local state = StoryConfig.State

StoryInfo = StoryInfo or BaseClass()

function StoryInfo:__init(data, co)
    self:storyInfo(data, co)
    self.is_playing = false
end

function StoryInfo:storyInfo(data, co)
    self.data = data
    self.info = co
    self.state = state.story_end
    self.instantiate = true
    self.director = nil

    self.trigger_type = StoryConfig.TriggerType.None
    self.ready_time = 0     -- 剧情准备时间

    if self.StoryInfo then self:StoryInfo() end
end

function StoryInfo:StartStory()
    self.state = state.Loading
    self:storyReady()
    if self.ready_time > 0 then
        self.timer_dt = TimeHelper:AddDelayTimer(function()
            ResMgr:LoadGameObject(DrawerHelper.GetStoryPath(self.info.res_name), BindTool.Bind(self.storyLoaded, self), self.instantiate)
        end, self.ready_time)
    else
        ResMgr:LoadGameObject(DrawerHelper.GetStoryPath(self.info.res_name), BindTool.Bind(self.storyLoaded, self), self.instantiate)
    end
    return self
end

function StoryInfo:storyReady()
    if self.StoryReady then self:StoryReady() end
end

function StoryInfo:storyLoaded(obj)
    if self.StoryLoaded then self:StoryLoaded(obj) end

    self.director = obj:GetComponent(typeof(timelineHandle))
    self.director.OnCheckResoureceEndEvent = self.director.OnCheckResoureceEndEvent + BindTool.Bind(self.storyCheckResourcesEnd, self)
    self.storyEndFunc = BindTool.Bind(self.storyEnd, self)
    self.storyEventFunc = BindTool.Bind(self.storyEvent, self)
    self.director.OnFinishEvent = self.director.OnFinishEvent + self.storyEndFunc
    self.director.OnCustomEvent = self.director.OnCustomEvent + self.storyEventFunc
    self.trans_by_target_list = {}
    
    self.director:RefreshAllResources(BindTool.Bind(self.storyDyLoad, self))
    
    self.state = state.Loaded
end

function StoryInfo:storyDyLoad(key, wait_resource, target)
    local params = string.split(key, ":")
    local res_type = params[1]
    local res_id = tonumber(params[2])

    local function wait_until_get_resource(resource_get_func, prof)
        local run_until_get = function()
            local resource = resource_get_func()
            if nil == resource then
                return false
            else
                wait_resource.Resources = resource.transform
                wait_resource.Prof = prof or 1
                if nil ~= target then
                    self:SetTarget(resource, target)
                end
                return true
            end
        end
        if run_until_get() == false then
            Runner.Instance:RunUntilTrue(run_until_get)
        end
    end
    if self.StoryDyLoad then self:StoryDyLoad(wait_until_get_resource, res_type, res_id) end
end

function StoryInfo:storyCheckResourcesEnd()
    self:Play()
end

function StoryInfo:Play()   
    if nil ~= self.director then
        self.director:Play()
        self.is_playing = true
    end
end

function StoryInfo:Pause()
    self.director:Pause()
end

function StoryInfo:Resume()
    if self.director then
        self.director:Resume()
    end
end

function StoryInfo:Stop()
    self.is_playing = false
    if self.director then
        self.director:Stop()
    end
end


function StoryInfo:storyEnd()
    self.is_playing = false
    TimeHelper:CancelTimer(self.timer_dt)
    if nil ~= self.director then
        self.director.OnFinishEvent = self.director.OnFinishEvent - self.storyEndFunc
        self.director.OnCustomEvent = self.director.OnCustomEvent - self.storyEventFunc
    end
    if self.StoryEnd then self:StoryEnd() end
    self:EndStory()
end

function StoryInfo:storyEvent(key)
    local params = string.split(key, ":")
    if #params < 1 then return end
    local type = params[1]

    if StoryConfig.CustomEventType.Stop == type then
        self:Stop()
    end
    if StoryConfig.CustomEventType.Black == type then
        PublicPopupCtrl.Instance:TopLayerBlackShow(true)
    end
    if StoryConfig.CustomEventType.NpcDialog == type and #params >= 2 then
        local nd_params = string.split(params[2],",")
        local nd_talk_id = tonumber(nd_params[1])
        local nd_keep_t = 0
        if #nd_params >= 2 then
            nd_keep_t = tonumber(nd_params[2])
            nd_keep_t = nd_keep_t / 1000
        end
        local nd_need_pause = true
        if #nd_params >= 3 then
            nd_need_pause = tonumber(nd_params[3]) ~= 0
        end
        if nd_need_pause then self:Pause() end
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = nd_talk_id}, function ()
            -- BehaviorData.Instance.auto_behavior = 0
            if nd_need_pause then self:Resume() end
        end)
    end
    if StoryConfig.CustomEventType.AudioEff == type and #params >= 2 then
        local ae_params = string.split(params[2],",")
        local au_path = ae_params[1]
        if #ae_params >= 2 and ae_params[2] == "1" then
            au_path = string.format("%s_%s",au_path,RoleData.Instance:GetRoleAvatarType())
        end
        AudioMgr:PlayEffectByPath(au_path)
    end

    if self.StoryEvent then self:StoryEvent(type, params) end
end

function StoryInfo:EndStory()
    self.state = state.End
    for _, target in pairs(self.trans_by_target_list or {}) do
        target.enabled = false
        target:SetTarget(nil)
    end

    if nil ~= self.director and not self.director:Equals(nil) then
        UnityGameObj.Destroy(self.director.gameObject)
        self.director = nil
    end
    self.data.story_event:Set{}
end

function StoryInfo:State()
    return self.state
end

function StoryInfo:TriggerType()
    return self.trigger_type
end

function StoryInfo:needTrigger(trigger_type)
    if (StoryConfig.TriggerType.BattleStart == self.trigger_type or StoryConfig.TriggerType.BattleStartNew == self.trigger_type) and BattleData.Instance:Round() > 0 then return false end   -- 战斗中断线重进 round>0会导致boss可能已经死亡而卡死所以不播剧情
    local value = trigger_type == self.trigger_type or nil == trigger_type or StoryConfig.TriggerType.None == self.trigger_type or StoryConfig.TriggerType.BattleStartNew == self.trigger_type
    if self.NeedTrigger then
        local val_cdt = self:NeedTrigger()
        if not val_cdt then self:ConditionErrorTip() end
        value = value and val_cdt
    end
    return value
end

function StoryInfo:SetTarget(resource, target)
    local trans_by_target = resource:GetComponent(typeof(Game.TransformByTarget))
    if nil == target or nil == trans_by_target then return end
    trans_by_target.enabled = true
    trans_by_target:SetTarget(target.transform)
    table.insert(self.trans_by_target_list, trans_by_target)
end

function StoryInfo:BossPos()
    local ch = BattleCtrl.Instance.displayer:getCh(13)
    if nil == ch then
        return false
    else
        return true, ch:GetRoot().transform
    end
end

function StoryInfo:ConditionErrorTip()
    PublicPopupCtrl.Instance:Center(string.format(Language.Story.ConditionError, self.info.name) .. (Language.Story.ErrorTips[self.info.id] or ""))
end

function StoryInfo.SceneIdVerify(scene_id)
    return scene_id == SceneData.Instance:SceneId()
end

function StoryInfo:MonsterGroupIdVerify()
    return MainViewData.Instance:IsBattle() and self.info.param1 == BattleData.Instance:BaseData().monster_group_id
end

function StoryInfo:SetCustomFunc(func)
    self.custom_func = func
end

function StoryInfo:CallCustomFunc()
    if nil ~= self.custom_func then
        Call(self.custom_func)
        self.custom_func = nil
    end
end

function StoryInfo:IsPlaying()
    return self.is_playing
    -- return self.director and (1 == self.director.state) or false
end

function StoryInfo:SetPostEffectEnable(value)
    if nil == self.post_effect then
        self:GetPostEffect()
    end
    local state = self.post_effect.enabled
    self.post_effect.enabled = value
    return state
end

StoryMonoItem = StoryMonoItem or DeclareMono("StoryMonoItem")

function StoryMonoItem:SetStoryInfo(story_info)
    self.story_info = story_info
end

function StoryMonoItem:OnDestroy()
    if self.story_info then
        self.story_info:storyEnd()
        self.story_info = nil
    end
end


FILE_REQ("modules/story/story_infos/story_info_showboss2")
FILE_REQ("modules/story/story_infos/story_info_showboss3")
FILE_REQ("modules/story/story_infos/story_info_showboss104")

FILE_REQ("modules/story/story_infos/story_info_trans1002")
FILE_REQ("modules/story/story_infos/story_info_1002skill1")
FILE_REQ("modules/story/story_infos/story_info_1002skill2")

FILE_REQ("modules/story/story_infos/story_info_task_hc")
FILE_REQ("modules/story/story_infos/story_info_task_hc_show")

FILE_REQ("modules/story/story_infos/story_info_xingtian_chuchang")

FILE_REQ("modules/story/story_infos/story_info_origin_start")
FILE_REQ("modules/story/story_infos/story_info_origin_skill")
FILE_REQ("modules/story/story_infos/story_info_xinmopre")

FILE_REQ("modules/story/story_infos/story_info_wedding")
FILE_REQ("modules/story/story_infos/story_info_shitu_chuangong")

VIEW_REQ("modules/story/story_views/story_showboss_view")
VIEW_REQ("modules/story/story_views/story_options_view")
VIEW_REQ("modules/story/story_views/story_showeffect_view")
VIEW_REQ("modules/story/story_views/story_xinmopre_view")
VIEW_REQ("modules/story/story_views/story_marry_bless_view")

FILE_REQ("modules/story/story_infos/story_info_harmony_heavens")
FILE_REQ("modules/story/story_infos/story_info_harmony_heavens_show")

FILE_REQ("modules/story/story_infos/story_poetry")