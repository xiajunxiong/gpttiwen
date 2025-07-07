
StoryInfoXingTianChuChang = StoryInfoXingTianChuChang or BaseClass(StoryInfo)

function StoryInfoXingTianChuChang:StoryInfo()
    self.trigger_type = StoryConfig.TriggerType.BattleStart
    self.ready_time = 1
    self.group_id = 1014
end

function StoryInfoXingTianChuChang:StoryReady()
    -- BattleData.Instance:BaseData().pause = true
    -- ViewMgr:MainViewVisible(false)
    -- ViewMgr:OpenView(StoryShowBossView)
end
function StoryInfoXingTianChuChang:storyCheckResourcesEnd()
    BattleData.Instance:BaseData().sel_tar_type = BATTLE_MOVE_TYPE.NONE
    BattleData.Instance:ViewInfo().opened_sub = BattleSubView.None
    ViewMgr:MainViewVisible(false)
    if self.handle_runer then
        Runner.Instance:RemoveRunObj(self.handle_runer)
        self.handle_runer = nil
    end
    self.handle_runer = Runner.Instance:RunUntilTrue(
        function()
            if
                (SceneCtrl.Instance:CurScene():GetClassType() == BattleScene and
                    SceneCtrl.Instance:CurScene():ReadyComplete())
            then
                if BattleSimulator.Instance:IsPlaying() then
                    self:StoryEnd()
                    return true
                end
                self:Play()
                return true
            end
            return false
        end)
end
function StoryInfoXingTianChuChang:Play()
    local ch
    for i = 1, 20 do
        if BattleData.IsEnemyIdx(i) then
            ch = BattleCtrl.Instance.displayer:getCh(i)
            if ch then
                ch:SetIsShow(false)
            end
        end
    end
    ch = BattleCtrl.Instance.displayer:getCh(13)
    if ch then
        SE:Play(SceneEffectConfig.EffectList.XTInitEffect, nil,nil,ch:GetRoot())
        ch:SetIsShow(true)
        ch:PlayAnim(ChAnimType.BianShen)
    end
    ViewMgr:OpenView(StoryShowBossView)
    AudioMgr:PlayEffect(AudioType.Battle, "XingTianChuChang")
    StoryInfo.Play(self)
end

function StoryInfoXingTianChuChang:StoryLoaded(obj)
    --[[ self.story_mono = obj:GetLuaComponent(StoryMonoItem:GetTypeName())
    local boss_live, boss_pos = self:BossPos()
    if boss_live then self.story_mono.BossPos:SetTarget(boss_pos) end ]]
end

function StoryInfoXingTianChuChang:StoryDyLoad(wait_until_get_resource, res_type, res_id)
    -- if res_type == "MonsterPos" then
    --     wait_until_get_resource(
    --         function()
    --             -- return BattleCtrl.Instance.displayer:getCh(13):GetRoot()
    --             return MainPlayer.root_node
    --         end
    --     )
    -- end
end

function StoryInfoXingTianChuChang:StoryEvent(type, params)
    if StoryConfig.CustomEventType.ShowBoss == type then
        self.data.story_event:Set{id = self.info.id}
    end
end

function StoryInfoXingTianChuChang:StoryEnd()
    ViewMgr:CloseView(StoryShowBossView)
    TimeHelper:AddDelayFrameTimer(function ()
        local ch
        for i = 1, 20 do
            if BattleData.IsEnemyIdx(i) then
                ch = BattleCtrl.Instance.displayer:getCh(i)
                if ch then
                    ch:SetIsShow(true)
                end
            end
        end
    end,3)
    ViewMgr:MainViewVisible(true)
    BattleCtrl.SendPrepareFinish()
    if self.handle_runer then
        Runner.Instance:RemoveRunObj(self.handle_runer)
        self.handle_runer = nil
    end
    StoryInfo.EndStory(self)
end

function StoryInfoXingTianChuChang:NeedTrigger()
    return self:MonsterGroupIdVerify()
end
