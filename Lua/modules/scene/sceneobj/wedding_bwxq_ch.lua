WeddingBaoWeiXiuQiuCh = WeddingBaoWeiXiuQiuCh or BaseClass(SceneObj)
function WeddingBaoWeiXiuQiuCh:__init()
    self.vo = nil
end
function WeddingBaoWeiXiuQiuCh:__delete()
    MarryData.Instance.wedding_fb_event:Uncare(self.bwxq_patrol_monsters_show_sm_care1)
    MarryData.Instance.wedding_fb_bwxq_sm:Uncare(self.bwxq_patrol_monsters_show_sm_care2)
    
    TimeHelper:CancelTimer(self.timer_monster_stop)
end

function WeddingBaoWeiXiuQiuCh:InitEnd()
    self.patrol_pos = MarryData.Instance:GetBaoWeiXiuQiuMonsterPos(self.vo.id)
    self.patrol_state = MarryConfig.BWXQPatrolState.ready
    local pos_birth = self.patrol_pos[0]
    self:SetScenePos(self.vo.scene_id, pos_birth.x, pos_birth.y, true)
    self:SetMoveSpeed(self.vo.move_speed or 2.5)
    self:Patroling()
    
    self.bwxq_patrol_monsters_show_sm_care1 = MarryData.Instance.wedding_fb_event:Care(BindTool.Bind(self.FlushPatrolMonsterState, self))
    self.bwxq_patrol_monsters_show_sm_care2 = MarryData.Instance.wedding_fb_bwxq_sm:Care(BindTool.Bind(self.FlushPatrolMonsterState, self))
    self:FlushPatrolMonsterState()
end


function WeddingBaoWeiXiuQiuCh:Patroling()
    TimeHelper:CancelTimer(self.timer_monster_stop)
    if nil == self.vo then return end
    self.timer_monster_stop = TimeHelper:AddDelayTimer(function ()
        if MarryConfig.BWXQPatrolState.stop == self.patrol_state then
            return
        end
        self.patrol_state = (self.patrol_state + 1) % MarryConfig.BWXQPatrolState.max
        local to_pos = self.patrol_pos[self.patrol_state]
        local to_pos_c = SceneCtrl.Instance:CurScene():PosToClint(to_pos.x, to_pos.y)
        SceneCh.MoveTo(self, to_pos_c, false, BindTool.Bind(self.Patroling, self))
    end, self.vo.stop_time)
end

function WeddingBaoWeiXiuQiuCh:StopPatrol()
    self.patrol_state = MarryConfig.BWXQPatrolState.stop
end

function WeddingBaoWeiXiuQiuCh:FlushPatrolMonsterState()
    if nil == self.vo then return end
    local is_show = MarryData.Instance:GetBaoWeiXiuQiuMonsterShow(self.vo.id)
    self:SetVisible(is_show)
    if is_show then
        if MarryConfig.BWXQPatrolState.stop == self.patrol_state then
            self.patrol_state = MarryConfig.BWXQPatrolState.ready
            self:Patroling()
        end
    else
        self:StopPatrol()
    end
end

function WeddingBaoWeiXiuQiuCh:FlushDrawer()
    local data = string.format(DrawerHelper.GetNpcPath(self.vo.res_id))
    self.body:SetData(data)
end
