SoldierCh = SoldierCh or BaseClass(SceneObj)
function SoldierCh:__init()
    self.vo = nil
    self.patrol_list = {}
end
function SoldierCh:__delete()
    if self.move_wait ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.move_wait)
        self.move_wait = nil
    end
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay2)
end
function SoldierCh:InitEnd()
    --进行初始位置设置
    local bir_pos = string.split(self.vo.birth_xyz, "|")
    local end_pos = string.split(self.vo.destination_xyz, "|")
    self:SetScenePos(self.vo.scene_id, bir_pos[1], bir_pos[2], false)
    self:SetMoveSpeed(self.vo.move_speed or 2.5)

    local pos1 = Vector3.New(bir_pos[1], 0, bir_pos[2])
    local pos2 = Vector3.New(end_pos[1], 0, end_pos[2])
    pos1.y = SceneCtrl.Instance:CurScene():GetPosHeight(pos1.x, pos1.z)
    pos2.y = SceneCtrl.Instance:CurScene():GetPosHeight(pos2.x, pos2.z)
    self.patrol_list[1] = {}
    self.patrol_list[2] = {}
    table.insert(self.patrol_list[1], pos1)--开始
    -- table.insert(self.patrol_list[2], pos2)--结束
    for i = 1, 10 do
        if type(self.vo["xyz" .. i]) == "string" then
            local pos = string.split(self.vo["xyz" .. i],"|")
            pos = Vector3.New(pos[1], 0, pos[2])
            pos.y = SceneCtrl.Instance:CurScene():GetPosHeight(pos.x, pos.z)
            table.insert(self.patrol_list[1], pos)
        end
    end
    table.insert(self.patrol_list[1], pos2)

    for i, v in ipairs(self.patrol_list[1]) do
        table.insert(self.patrol_list[2], 1, v)
    end
    TimeHelper:CancelTimer(self.handle_delay)
    self.handle_delay = TimeHelper:AddDelayTimer(function ()
        self:Patrol(self.patrol_list[1], 1)
    end,2)
end
function SoldierCh:PatrolEnd(flag)
    self:Patrol(self.patrol_list[flag == 1 and 2 or 1], flag == 1 and 2 or 1)
end
function SoldierCh:FlushDrawer()
    local data = string.format(DrawerHelper.GetNpcPath(self.vo.res_id))
    self.body:SetData(data)
end

function SoldierCh:Patrol(path_poses, flag)
    --直接写巡逻逻辑
    if self.nav_mesh_path == nil then
        self.nav_mesh_path = UnityAI.NavMeshPath()
    end
    if self.move_wait ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.move_wait)
        self.move_wait = nil
    end

    local i_holder = {index = 1}
    local move_ready = {}
    if self.body then
        self.body:PlayAnim(ChAnimType.Run)
    end
    move_ready.move_func = function(index_holder)
        if index_holder.index > #path_poses then
            if self.body then
                if not self.body:AnimClipsEmpty(ChAnimType.Run2) then
                    self.body:PlayAnim(ChAnimType.Run2)
                else
                    self.body:PlayAnim(ChAnimType.Idle)
                end
            else
                return
            end
            TimeHelper:CancelTimer(self.handle_delay2)
            self.handle_delay2 = TimeHelper:AddDelayTimer(function ()
                self:PatrolEnd(flag)
            end,2)
            return
        end
        local ne_p = path_poses[index_holder.index]
        local suc = self.nav_agent:SetDestination(ne_p)
        if suc == false then
            index_holder.index = index_holder.index + 1
            move_ready.move_func(index_holder)
        end
    end

    move_ready.move_func(i_holder)

    self.move_wait =
        Runner.Instance:RunUntilTrue(
        function()
            if i_holder.index > #path_poses then
                return true
            end
            local target_pos = path_poses[i_holder.index]
            local self_p = self:GetPos()
            local dis_x = target_pos.x - self_p.x
            local dis_z = target_pos.z - self_p.z
            local sqrMagnitude = dis_x * dis_x + dis_z * dis_z
            local get_target = sqrMagnitude < 0.5
            if get_target == true and i_holder.index == #path_poses then
                get_target = not self.nav_agent.hasPath
            end
            if get_target then
                i_holder.index = i_holder.index + 1
                move_ready.move_func(i_holder)
            end
            return false
        end
    )
end
