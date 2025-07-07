
StoryInfoHarmonyHeavens = StoryInfoHarmonyHeavens or BaseClass(StoryInfo)

function StoryInfoHarmonyHeavens:StoryInfo()
    self.trigger_type = StoryConfig.TriggerType.BattleStartNew

    self.group_id = self.info.param1
end

function StoryInfoHarmonyHeavens:StartStory()
    self.state = StoryConfig.State.Loading
    self:storyReady()
    if self.handle_runer then
        Runner.Instance:RemoveRunObj(self.handle_runer)
        self.handle_runer = nil
    end
    self.handle_runer = Runner.Instance:RunUntilTrue(function ()
        if SceneCtrl.Instance:CurScene():GetClassType() == BattleScene and SceneCtrl.Instance:CurScene():ReadyComplete() then
            ResMgr:LoadGameObject(DrawerHelper.GetStoryPath(self.info.res_name), BindTool.Bind(self.storyLoaded, self), self.instantiate)
            return true
        end
        return false
    end)
    return self
end


function StoryInfoHarmonyHeavens:StoryReady()
    local main_camera = UnityGameObj.FindGameObjectWithTag("MainCamera")
    local post_effect = main_camera:GetComponent(typeof(Crystal.PostEffectDepthOfField))
    post_effect.enabled = false
end

function StoryInfoHarmonyHeavens:storyCheckResourcesEnd()

end

function StoryInfoHarmonyHeavens:Play()
    ViewMgr:MainViewVisible(false)
    local ch
    for i = 1, 20 do
        -- if BattleData.IsEnemyIdx(i) then
            ch = BattleCtrl.Instance.displayer:getCh(i)
            if ch then
                ch:SetIsShow(false)
            end
        -- end
    end
    StoryInfo.Play(self)
end

function StoryInfoHarmonyHeavens:StoryLoaded(obj)
    -- obj:SetParent(UnityGameObj.Find("ResidentObjectManager"))
    self.story_mono = obj:GetLuaComponent(StoryMonoItem:GetTypeName())
    self.story_mono:SetStoryInfo(self)
end

function StoryInfoHarmonyHeavens:StoryDyLoad(wait_until_get_resource, res_type, res_id)
    if res_type == "MonsterAnim" then
        wait_until_get_resource(
            function()
                local ch = BattleCtrl.Instance.displayer:getCh(13)
                local obj = ch:GetSkin0()
                return obj
            end
        )
    end
end

function StoryInfoHarmonyHeavens:StoryEvent(type, params)
    if StoryConfig.CustomEventType.Pause == type then
        local value = tonumber(params[2])
        self:Pause()
        if 1 == value then
            local ch
            for i = 1, 20 do
                if i <= 5 then
                    ch = BattleCtrl.Instance.displayer:getCh(i)
                    if ch then
                        if ch.vo.curhp > 0 then
                            local displayer = BattleCtrl.Instance.displayer
                            local fw = displayer:getIdxForward(i)
                            ch:SetPos(displayer:getIdxPos(i) - fw * 7)
                            ch:SetIsShow(true)
                            ch:SetForward(fw)
                            ch:PlayAnim(ChAnimType.Run)
                            ch:MoveToPos(displayer:getIdxPos(i), false, nil, BindTool.Bind(self.RoleMoveEnd, self), nil, 8)
                        end
                    end
                end
            end
        elseif 2 == value then
            if not ViewMgr:IsOpen(NpcDialogView) then
                -- BehaviorData.Instance.auto_behavior = 5
                NpcDialogCtrl.Instance:NpcDialogView({talk_id = 4984}, function ()
                    BehaviorData.Instance.auto_behavior = 0
                    if not MainViewData.Instance:IsBattle() then return end
                    ViewMgr:MainViewVisible(false)
                    self:Resume()
                    AudioMgr:PlayEffect(AudioType.Battle, "JiuYingDengChang")
                    local ch = BattleCtrl.Instance.displayer:getCh(13)
                    if ch ~= nil then
                        ch:SetIsShow(true)
                    else
                        LogError("创角流程异常!BOSS战斗角色没找到!!!")
                    end
                end)
            end
        elseif 3 == value then
            if not ViewMgr:IsOpen(NpcDialogView) then
                -- BehaviorData.Instance.auto_behavior = 5
                NpcDialogCtrl.Instance:NpcDialogView({talk_id = 4985}, function ()
                    BehaviorData.Instance.auto_behavior = 0
                    self:Stop()
                end)
            end
        end
    elseif StoryConfig.CustomEventType.ShowBoss == type then
        self.data.story_event:Set{id = self.info.id}
    end
end

function StoryInfoHarmonyHeavens:RoleMoveEnd()
    if not ViewMgr:IsOpen(NpcDialogView) then
        -- BehaviorData.Instance.auto_behavior = 5
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = 4983}, function ()
            BehaviorData.Instance.auto_behavior = 0
            self:Resume()
        end)
    end
end

function StoryInfoHarmonyHeavens:StoryEnd()
    -- ViewMgr:CloseView(StoryShowBossView)
    local ch
    for i = 1, 20 do
        -- if BattleData.IsEnemyIdx(i) then
            ch = BattleCtrl.Instance.displayer:getCh(i)
            if ch then
                ch:SetIsShow(true)
            end
        -- end
    end
    ViewMgr:MainViewVisible(true)
    BattleCtrl.SendPrepareFinish()
    if self.handle_runer then
        Runner.Instance:RemoveRunObj(self.handle_runer)
        self.handle_runer = nil
    end
    StoryInfo.EndStory(self)
end

-- function StoryInfoHarmonyHeavens:NeedTrigger()
--     return self:MonsterGroupIdVerify()
-- end

-- StoryInfoHarmonyHeavens1 = StoryInfoHarmonyHeavens1 or BaseClass(StoryInfoHarmonyHeavens)

-- function StoryInfoHarmonyHeavens1:StoryInfo()
--     self.trigger_type = StoryConfig.TriggerType.BattleStartNew
--     self.group_id = 40710
-- end