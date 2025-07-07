BoxMapExplore = BoxMapExplore or BaseClass(SceneObj)
function BoxMapExplore:__init()
    self.vo = nil
    self.patrol_list = {}
end 
function BoxMapExplore:__delete()
    self:CloseBox()
end 

function BoxMapExplore:InitEnd()
    -- --进行初始位置设置
    local explore_cfg = MapExploreData.Instance:GetBoxMapExploreCfg(self.vo.explore_seq,self.vo.scene_id)
    self:SetScenePos(self.vo.scene_id, explore_cfg.x,  explore_cfg.y, true)
    -- local bir_pos = string.split(self.vo.birth_xyz, "|")
    -- self:SetScenePos(self.vo.scene_id, bir_pos[1], bir_pos[2], true)
    -- self:SetMoveSpeed(self.vo.move_speed or 2.5)

    -- self:StartPatrol()
end 

function BoxMapExplore:CloseBox()
    self.body:PlayAnim(ChAnimType.Idle)
end

-- function BoxMapExplore:FlushDrawer()
--     local data = string.format(DrawerHelper.GetNpcPath(self.vo.res_id))
--     self.body:SetData(data)
-- end

function BoxMapExplore:OpenBox()
    self.body:PlayAnim(ChAnimType.BattleRun)
end