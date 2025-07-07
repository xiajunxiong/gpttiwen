
StoryInfoXinMoPre = StoryInfoXinMoPre or BaseClass(StoryInfo)

function StoryInfoXinMoPre:StoryInfoXinMoPre()
    self.trigger_type = StoryConfig.TriggerType.None
    -- self.group_id = 1001
    self.main_ch_pos = Vector3.zero
    self.mainCanvas = nil
    self.story_mono = nil
    self.bg_handle = nil
end

function StoryInfoXinMoPre:StoryReady()
    -- ViewMgr:OpenView(StoryShowEffectView)
    -- self.main_role_info = { is_ready = false }
    -- self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
    -- self.mainCanvas:SetActive(false)

    ViewMgr:MainViewVisible(false)
    ViewMgr:OpenView(StoryXinmopreView, {story_info = self})
end

-- function StoryInfoXinMoPre:storyCheckResourcesEnd()
-- end

function StoryInfoXinMoPre:Play()
    -- self.data.story_event:Set{id = self.info.id}
    -- BattleData.Instance.story_data.hide_ui = true
    -- LogError("PLAY!!!!!!!!!!!")
    MainProber:Step(MainProber.STEP_XINMO_ANIM_START, MainProber.user_id, MainProber.server_id, MainProber.role_name, MainProber.role_id)
    StoryInfo.Play(self)
    local weak_clip_name = string.format("xinmopre_weak_idle_%d", RoleData.Instance:GetRoleAvatarType())
    ClipGeter.GetClip(weak_clip_name,function()
        -- MainPlayer:AnimParams()
        MainPlayer:PlayAnim(ChAnimType.XinMoPreWeak,{weak_clip_name})
        MainPlayer:SetWeaponShow(false)
    end)
    self.bg_handle = AudioMgr:PlayEffectHandle(AudioType.Story,"xinmopre_bg")
    MainPlayer:SetWeaponShow(false) --等待异步加载的时候武器可能漏出来的问题
    self.timer_dt_weapon = TimeHelper:AddDelayTimer(function ()
        MainPlayer:SetWeaponShow(false) --等待异步加载的时候武器可能漏出来的问题   
    end, 0.5)
end

function StoryInfoXinMoPre:StoryLoaded(obj)
    self.story_mono = obj:GetLuaComponent(StoryMonoItem:GetTypeName())
    self.story_mono:SetStoryInfo(self)
    AudioMgr:SetBgFlag(false)
end

function StoryInfoXinMoPre:StoryDyLoad(wait_until_get_resource, res_type, res_id)
    if res_type == "MainRoleAnim" then
        local prof = RoleData.Instance:GetRoleAvatarType()
        wait_until_get_resource(
            function()
                return MainPlayer.body.main_body.obj
            end,
            prof
        )
    elseif res_type == "MainRolePos" then
        wait_until_get_resource(function() 
            -- LogError("POs===",self.main_ch_pos)
            -- MainPlayer.nav_agent.enabled = false
            self.main_ch_pos = MainPlayer:GetPos():Clone()
            return MainPlayer.root_node 
        end)
    
    elseif res_type == "RoleAnim" then
        wait_until_get_resource(function()
            local re_obj = {obj = nil}
            ResMgr:LoadObjectOfType(
                "material/xinmopre_" .. tostring(RoleData.Instance:GetRoleAvatarType()) .. ".mat",
                typeof(UnityEngine.Material),
                function(mat)
                    -- local skin_obj = MainPlayer.body.main_body.renderer_com.gameObject
                    -- skin_obj:GetOrAddComponent(typeof(UnityAnim))
                    MainPlayer.body.main_body:SetRendererMaterial(mat,2)
                    -- re_obj.obj = skin_obj
                end
            )
            -- LogError("skinOBJ====",skinObj.name)
            -- return re_obj.obj

            local skin_obj = MainPlayer.body.main_body.renderer_com.gameObject
            skin_obj:GetOrAddComponent(typeof(UnityAnim))
            MainPlayer.body:SetRendererMaterial(mat,2)
            return skin_obj
        end)
        
    end
end

function StoryInfoXinMoPre:StoryEvent(type, params)
    if type == StoryConfig.CustomEventType.XinMoPre.Caption then
        local sp_pars = string.split(params[2],",")
        local show_txt = Language.Story.XinMoPre[sp_pars[1]]
        if sp_pars[1] == "Shui" then -- 前缀加上主角名
            show_txt = string.format(show_txt,RoleData.Instance:GetRoleName())
        end
        StoryXinmopreView.ShowText(show_txt,tonumber(sp_pars[2])/1000)  
    elseif type == StoryConfig.CustomEventType.XinMoPre.GotoBattle then
        self:Pause()
        local task = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.mainline)
        if task ~= nil then
            task:GoTask()
            TaskCtrl.Instance:CancelAutoMainTimeHandle()
        end
    end
end

function StoryInfoXinMoPre:StoryEnd()
    if self.bg_handle ~= nil then
        AudioMgr:StopEffectHandle(self.bg_handle)
        self.bg_handle = nil
    end
    local total_flag = RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SET_TOTAL_VOLUME) > 0
    local flag = RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SET_MUSIC_VOLUME) > 0
	AudioMgr:SetBgFlag(total_flag and flag)
    TimeHelper:CancelTimer(self.timer_dt_weapon)
    if MainPlayer then
        MainPlayer:PlayAnim(ChAnimType.Idle)
        MainPlayer.body.main_body:RemoveRendererMaterial(2)
        MainPlayer:SetPos(self.main_ch_pos) 
        MainPlayer:SetWeaponShow(true)
    end
    ViewMgr:MainViewVisible(true)
    ViewMgr:CloseView(StoryXinmopreView)
    MainProber:Step(MainProber.STEP_XINMO_ANIM_END, MainProber.user_id, MainProber.server_id, MainProber.role_name, MainProber.role_id)
    TaskCtrl.Instance:TryBeginAutoTaskImmediately()
end