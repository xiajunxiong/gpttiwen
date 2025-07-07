DuChuangZeiYingPatrolMonsterCh = DuChuangZeiYingPatrolMonsterCh or BaseClass(SceneObj)
function DuChuangZeiYingPatrolMonsterCh:__init()
    self.vo = nil
end
function DuChuangZeiYingPatrolMonsterCh:__delete()
    DuChuangZeiYingData.Instance.patrol_monsters_show:Uncare(self.dczy_patrol_monsters_show_sm_care)
    if self.DuChuangZeiYingEffectObj ~= nil then
        UnityGameObj.Destroy(self.DuChuangZeiYingEffectObj)
        self.DuChuangZeiYingEffectObj = nil
    end
    TimeHelper:CancelTimer(self.timer_monster_stop)
end

function DuChuangZeiYingPatrolMonsterCh:InitEnd()
    self.patrol_pos = DuChuangZeiYingData.Instance:GetPatrolMonsterPos(self.vo.id)
    self.patrol_state = DuChuangZeiYingConfig.PatrolState.ready
    local pos_birth = self.patrol_pos[0]
    self:SetScenePos(self.vo.scene_id, pos_birth.x, pos_birth.y, true)
    self:SetMoveSpeed(self.vo.move_speed or 2.5)
    self:FlushDuChuangZeiYingEffect()
    self:Patroling()

    self.dczy_patrol_monsters_show_sm_care = DuChuangZeiYingData.Instance.patrol_monsters_show:Care(BindTool.Bind(self.FlushPatrolMonsterState, self))
    self:FlushPatrolMonsterState()
end

function DuChuangZeiYingPatrolMonsterCh:FlushDuChuangZeiYingEffect()
    if nil ~= self.vo and nil == self.DuChuangZeiYingEffectObj then
        if not self.loading_dczy_effect then
            self.loading_dczy_effect = true
            ResMgr:LoadGameObject(
                "actor/scene/du_chuang_zei_ying_effect.prefab",
                function(obj)
                    if self.vo == nil then
                        self.loading_dczy_effect = false
                        return
                    end
                    self.DuChuangZeiYingEffectObj = obj:Clone(self.root_node)
                    self.DuChuangZeiYingEffectObj:ResetTransform()
                    local mono = self.DuChuangZeiYingEffectObj:GetLuaComponent("DuChuangZeiYingEffect")
                    mono.TriggerMono:SetData(self.vo)
                    self.DuChuangZeiYingEffectObj:SetActive(true)
                    self.loading_dczy_effect = false
                end
            )
        end
    end
end

function DuChuangZeiYingPatrolMonsterCh:Patroling()
    TimeHelper:CancelTimer(self.timer_monster_stop)
    if nil == self.vo then return end
    self.timer_monster_stop = TimeHelper:AddDelayTimer(function ()
        if DuChuangZeiYingConfig.PatrolState.stop == self.patrol_state then
            return
        end
        self.patrol_state = (self.patrol_state + 1) % DuChuangZeiYingConfig.PatrolState.max
        local to_pos = self.patrol_pos[self.patrol_state]
        local to_pos_c = SceneCtrl.Instance:CurScene():PosToClint(to_pos.x, to_pos.y)
        SceneCh.MoveTo(self, to_pos_c, false, BindTool.Bind(self.Patroling, self))
    end, self.vo.stop_time)
end

function DuChuangZeiYingPatrolMonsterCh:StopPatrol()
    self.patrol_state = DuChuangZeiYingConfig.PatrolState.stop
end

function DuChuangZeiYingPatrolMonsterCh:FlushPatrolMonsterState()
    if nil == self.vo then return end
    local is_show = DuChuangZeiYingData.Instance.patrol_monsters_show[self.vo.id]
    self:SetVisible(is_show)
    if not is_show then self:StopPatrol() end
end

function DuChuangZeiYingPatrolMonsterCh:FlushDrawer()
    local data = string.format(DrawerHelper.GetNpcPath(self.vo.res_id))
    self.body:SetData(data)
end
