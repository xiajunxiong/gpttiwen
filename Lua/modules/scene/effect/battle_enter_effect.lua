BattleEnterEffect = BattleEnterEffect or DeclareMono("BattleEnterEffect", UIWFlushPanel)
function BattleEnterEffect:OnEnable()
    if MainPlayer then
        MainPlayer:BindVirtualCamera(self.Camera)
        AudioMgr:PlayEffect(AudioType.Battle, "enter_battle")
    end
end
function BattleEnterEffect:OnDisable()
end

RoleFirstEnter = RoleFirstEnter or DeclareMono("RoleFirstEnter", UIWFlushPanel)
function RoleFirstEnter:Awake()
    -- LogError("Awake")
end
function RoleFirstEnter:OnEnable()
    if LoginData.Instance:PCRealNameSignNeedOpen() then
        if ChannelAgent.OpenPCRealNameSignView then
            ChannelAgent.OpenPCRealNameSignView()
        end
    end

    if LoginData.Instance:IsNewChara() then
        -- if ViewMgr:IsOpen(LoadingView) then
        --    self.handle_runner = Runner.Instance:RunUntilTrue(function ()
        --         if MainPlayer then
        --             MainPlayer:SetVisible(false)
        --         end
        --         if not ViewMgr:IsOpen(LoadingView) then
                    
        --             self:PlayNiXingLa()
        --             return true
        --         end
        --     end)
        -- else
        --     self:PlayNiXingLa()
        -- end
    else
        self.gameObject:SetActive(false)
    end
end
function RoleFirstEnter:PlayNiXingLa()
    -- 睁眼动画后，如果需要进入小游戏，并且已经到第x关了，就要开始播放波纹特效
    -- 先干掉了
    -- if TowerAdventureCtrl.Instance:IsNeedFirstStart() then
    --     SocietyData.Instance:PlayGiftEffect(100, 1)
    -- end
    
    -- ViewMgr:CloseView(StoryShowEffectView)
    PublicPopupCtrl.Instance:TopLayerBlackShow(false)
    LoginData.Instance:ClearIsNewChara()
    StoryData.Instance.is_playing_story = true
   --[[  local mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
    self.handle_delay1 =
        TimeHelper:AddDelayTimer(function ()
            if mainCanvas then
                mainCanvas:SetActive(false)
            end
        end,1)
 ]]
    
    --[[ local obj1 = UnityGameObj.Find("ResidentObjectManager")
    if obj1 then
        obj1:SetActive(false)
    end ]]
    local obj2 = UnityGameObj.Find("UICamera")
    if obj2 then
        obj2:SetActive(false)
    end
    --AudioMgr:SetBgVolume(0.01)
    AudioMgr:SetBgFlag(false)
    local sex = RoleData.Instance:GetRoleSex()
    AudioMgr:PlayEffect(AudioType.UI, sex == 1 and "qichuangnan" or "qichuangnv")
    --[[ self.handle_delay1 = TimeHelper:AddDelayTimer(function ()
        SceneCtrl.Instance:Objs():Shielding(true)
    end,2.5) ]]
    SceneCtrl.Instance:Objs():Shielding(true)
    self.handle_repeat = TimeHelper:AddCountDownTT(function ()
        SceneCtrl.Instance:Objs():Shielding(true)
    end, function ()
        
    end, 7, 0.5)
    --[[ local main_camera = UnityGameObj.FindGameObjectWithTag("MainCamera") ]]
    --[[ self.handle_runner2 = Runner.Instance:RunUntilTrue(function ()
        if ViewMgr:IsOpen(MainView) then
            ViewMgr:MainViewVisible(false)

            if main_camera then
                main_camera = main_camera:GetComponent(typeof(UnityCamera))
                main_camera.enabled = false
            end
            return true
        end
    end) ]]
    -- self.handle_delay =
    --     TimeHelper:AddDelayTimer(
    --     function()
    --         SceneCtrl.Instance:Objs():Shielding(false)
    --         AudioMgr:SetBgVolume(1)
            
    --         --[[ if main_camera then
    --             main_camera.enabled = true
    --         end ]]
    --         --[[ if mainCanvas then
    --             mainCanvas:SetActive(true)
    --         end ]]
    --         --[[ if obj1 then
    --             obj1:SetActive(true)
    --         end ]]
    --         if obj2 then
    --             obj2:SetActive(true)
    --         end
    --         StoryData.Instance.is_playing_story = false
    --         -- ViewMgr:MainViewVisible(true)
    --         self.gameObject:SetActive(false)
    --         local task_info = TaskData.Instance:GetTaskInfoByTaskId(10)
    --         if task_info then
    --             AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task_info)}, 1)
    --         end
    --     end,
    --     9.583
    -- )

    if self.handle_runner2 then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner2)
        self.handle_runner2 = nil
    end
    self.handle_runner2 = Runner.Instance:RunUntilTrue(function ()
        if self.Anim360:get_Item("juqing_cam.FBX").normalizedTime >= 1 then
            -- AudioMgr:SetBgVolume(1)
            local flag = RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SET_MUSIC_VOLUME) > 0
	        AudioMgr:SetBgFlag(flag)
            if obj2 then
                obj2:SetActive(true)
            end
            StoryData.Instance.is_playing_story = false
            self.gameObject:SetActive(false)
            SceneCtrl.Instance:Objs():Shielding(false)
            
            -- 睁眼动画后，如果需要进入小游戏，并且已经到第x关了，就要开指引
            -- 每个小游戏独立判断
            if LoginData.Instance:GetMiniGame() > 0 then
                local mini_other
                if TowerAdventureCtrl.Instance:IsNeedFirstStart() then
                    -- 塔楼
                    mini_other = TowerAdventureData.Instance:Other()
                elseif CowBoyCtrl.Instance:IsNeedFirstStart() then
                    --牛郎织女
                    mini_other = CowBoyData.Instance:Other()
                end
                if mini_other then
                    local end_func = function()
                        if not StringIsEmpty(tostring(mini_other.step_id)) then
                            GuideManager.Instance:Start(mini_other.step_id)
                        end
                    end
                    MiniGameCtrl.Instance:OpenDialogView(mini_other.npc_id_1, mini_other.txt_1, end_func)
                end
            else
                local task_info = TaskData.Instance:GetTaskInfoByTaskId(10)
                if task_info then
                    AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task_info)}, 1)
                end
            end
            return true
        end
        return false
    end)
end

function RoleFirstEnter:OnDisable()
    --无用
    StoryData.Instance.is_playing_story = false
    TimeHelper:CancelTimer(self.handle_delay)
    -- TimeHelper:CancelTimer(self.handle_delay1)
    TimeHelper:CancelTimer(self.handle_repeat)
    SceneCtrl.Instance:Objs():Shielding(false)
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    if self.handle_runner2 then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner2)
        self.handle_runner2 = nil
    end
    --[[ if self.handle_runner2 then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner2)
        self.handle_runner2 = nil
    end ]]
    -- LogError("OnDisable")
end
function RoleFirstEnter:OnDestroy()
    StoryData.Instance.is_playing_story = false
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay1)
    SceneCtrl.Instance:Objs():Shielding(false)
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    if self.handle_runner2 then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner2)
        self.handle_runner2 = nil
    end
    --[[ if self.handle_runner2 then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner2)
        self.handle_runner2 = nil
    end ]]
    -- LogError("OnDestroy")
end
