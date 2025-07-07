BoxTreasureTask = BoxTreasureTask or BaseClass(SceneObj)
function BoxTreasureTask:__init()
    self.vo = nil
end 
function BoxTreasureTask:__delete()
    self:CloseBox()
    TreasureTaskData.Instance.info_sm:Uncare(self.treasure_task_box_show_sm_care)
end 

function BoxTreasureTask:InitEnd()
    -- --进行初始位置设置
    local co = Cfg.TreasureTaskRewardCoordinateBySeq(self.vo.treasure_task_seq)
    self:SetScenePos(co.scene, co.x,  co.y, true)
    self:SetVisible(false)
    self.treasure_task_box_show_sm_care = TreasureTaskData.Instance.info_sm:Care(BindTool.Bind(self.FlushTreasureTaskShow, self))
    self:FlushTreasureTaskShow(true)
end

function BoxTreasureTask:FlushTreasureTaskShow(is_init)
    if nil == self.vo then return end
    local is_show = TreasureTaskData.Instance:IsBoxShow(self.vo.treasure_task_seq)
    if is_init or is_show then
        -- if is_show then
            self:CloseBox()
        -- end
        self:SetVisible(is_show)
    end
end

function BoxTreasureTask:CloseBox()
    self.body:PlayAnim(ChAnimType.Idle)
end

function BoxTreasureTask:OpenBox()
    self.body:PlayAnim(ChAnimType.BattleRun)
    self.delete_timer_handle = TimeHelper:AddDelayTimer(function ()
        TreasureTaskData.Instance:DelBoxEffByObjId(self.vo.obj_id)
        VoMgr:DelGCObj(self.vo.obj_id, true)
    end,1)
end