FoxCh = FoxCh or BaseClass(SceneObj)
function FoxCh:__init()
    self.vo = nil
    self.patrol_list = {}
end 
function FoxCh:__delete()
    if self.move_wait ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.move_wait)
        self.move_wait = nil
    end
    TimeHelper:CancelTimer(self.handle_delay)
end 
function FoxCh:InitEnd()
    --进行初始位置设置
    local bir_pos = string.split(self.vo.birth_xyz, "|")
    self:SetScenePos(self.vo.scene_id, bir_pos[1], bir_pos[2], true)
    self:SetMoveSpeed(self.vo.move_speed or 2.5)

    self:StartPatrol()
end 

function FoxCh:StartPatrol()
    local bir_pos = string.split(self.vo.birth_xyz, "|")
    local pos1 = self:QuickPos(bir_pos[1],bir_pos[2]) 

    self.patrol_list = {}
    table.insert(self.patrol_list, pos1) 
    for i = 1, 10 do
        if type(self.vo["xyz" .. i]) == "string" then
            local pos = string.split(self.vo["xyz" .. i],"|")
            pos = self:QuickPos(pos[1],pos[2])
            table.insert(self.patrol_list, pos)
        end
    end

    self.timer = 1
    self:Patrol(self.patrol_list)
end

function FoxCh:FlushDrawer()
    local data = string.format(DrawerHelper.GetNpcPath(self.vo.res_id))
    self.body:SetData(data)
end

function FoxCh:ChangeDrawer( )
    local pos_ = self:GetPos()
    self.vo.x = pos_.x
    self.vo.y = pos_.z
end

function FoxCh:Patrol(path_poses)
    --直接写巡逻逻辑
    TimeHelper:CancelTimer(self.handle_delay)
    if nil == self.vo then return end

    self.handle_delay = TimeHelper:AddDelayTimer(function ()
        if CatchFoxData.Instance:IsCatched() then return end

        SceneCh.MoveTo(self, path_poses[self.timer], false, BindTool.Bind(self.Patrol, self,path_poses))

        self.timer = self.timer == #path_poses and 1 or self.timer + 1
    end,self.vo.stop_time)
end

function FoxCh:QuickPos(x,y)
    local q_p = Vector3.New(x, 0, y)
    local q_p_c = SceneCtrl.Instance:CurScene():PosToClint(q_p.x, q_p.z)
    return q_p_c
end