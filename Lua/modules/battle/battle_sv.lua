-- 圣器
local root_path = 'actor/__ActorRoot.prefab'
local move_speed = 15 * 3.5
local step_dis = move_speed * 0.025
BattleSV = BattleSV or BaseClass()
function BattleSV:__init(data)
    self.vo = data
    self.root_node = GameObjectPool:Get(root_path)
end
function BattleSV:__delete()
    self:StatusRecovery()
    self.loaded_call_back = nil
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    if self.body_show then
        self.body_show:DeleteMe()
        self.body_show = nil
    end
    GameObjectPool:Release(self.root_node)
    self.root_node = nil
end
function BattleSV:CenterPoint()
    if self.center_point == nil then
        self.center_point = self.root_node:FindChild("CenterPoint")
    end
    return self.center_point
end
function BattleSV:GetRoot()
    return self.root_node
end

function BattleSV:Load(call_back)
    self.body_show = BattleObjDrawer.New(self.root_node, true)
    local config = Config.shengqi_auto.shengqi_type[self.vo.hallow_id + 1]
    local body_path = 'actor/weapon/%s.prefab'
    body_path = string.format(body_path, config.res_id)
    self.loaded_call_back = call_back
    self.body_show:SetData(body_path, nil, BindTool.Bind(self.BodyLoadedCallback, self))
end

function BattleSV:BodyLoadedCallback()
    self.handle_runner = Runner.Instance:RunUntilTrue(function()
        if SceneCtrl.Instance:CurScene():LoadingComplete() then
            self:Prepare()
            return true
        end
    end)
    if self.loaded_call_back then
        self.loaded_call_back()
        self.loaded_call_back = nil
    end
end
function BattleSV:Prepare()
    -- 获取携带者的forward
    -- 携带者的pos
    -- 负forward 1单位的pos y + 1单位的pos
    local pos = self:getIdxPos(self.vo.owner_idx, Vector3.New(0, 1, -1))
    local fw = self:getIdxForward(self.vo.owner_idx)
    self:SetPos(pos)
    self:SetForward(fw)
end
function BattleSV:SetPos(pos)
    if self.root_node then
        self.root_node:SetPosition(pos)
    end
end
function BattleSV:SetForward(forward)
    forward.y = 0
    self.root_node:SetForward(forward)
end
function BattleSV:GetPos()
    return self.root_node:GetPosition()
end
function BattleSV:getIdxForward(idx)
    local scene_data = BattleData.Instance:SceneData()
    if scene_data == nil then
        return Vector3.zero
    end
    return scene_data:GetForward(idx)
end
function BattleSV:getIdxPos(idx, offset)
    local scene_data = BattleData.Instance:SceneData()
    if scene_data == nil then
        return Vector3.zero
    end
    return scene_data:GetPos(idx, offset)
end
function BattleSV:EnableNCA()
    
end

function BattleSV:SkillPlayer()
    return self.body_show:SkillPlayer()
end

function BattleSV:MoveToPos(pos, run_anim, end_forward, move_end, dodge, custom_move_speed)
    if self.root_node == nil then
        BuglyReport("ShengQi Is Nil Code MoveToPos")
        return
    end
    local p1 = self.root_node:GetPosition()
    local p2 = pos
    local p3 = p2 - p1
    local tot_dis = p3:Magnitude()
    if tot_dis <= step_dis then -- frame pos .set pos and forward
        self:SetPos(pos)
        if end_forward ~= nil then
            self:SetForward(end_forward)
        end
        if move_end ~= nil then
            move_end()
        end
        return
    end
    if self.move_wait ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.move_wait)
    end 
    local dis = 0
    local forward = Vector3.Normalize(p3)
    local ms = forward * move_speed
    self.move_wait = Runner.Instance:RunUntilTrue(function()
        if self.root_node == nil then
            return true
        end
        if self.is_diauped == true then
            if move_end ~= nil then
                move_end()
            end
            return true
        end
        dis = Time.deltaTime * move_speed
        tot_dis = tot_dis - dis
        if tot_dis < 0 then
            self:SetPos(pos)
            if end_forward ~= nil then
                self:SetForward(end_forward)
            end
            if move_end ~= nil then
                move_end()
            end
            return true
        else
            local offset = ms * Time.deltaTime
            self:SetPos(self:GetPos() + offset)
            return false
        end
    end)
end



--1:左,2:上,3:右,4:下
local function rnd_vp_dir(edge, cam)
    local negative_x, negative_y
    if edge == 1 then
        negative_x = false
    elseif edge == 2 then
        negative_y = true
    elseif edge == 3 then
        negative_x = true
    elseif edge == 4 then
        negative_y = false
    end
    local rnd_an = 120
    local start_angle = -rnd_an / 2
    start_angle = start_angle + (edge - 1) * 90
    local end_angle = start_angle + rnd_an

    local vp_angle = GameMath.FloatRamdon(start_angle, end_angle)
    local tan_angle = math.tan(math.pi * vp_angle / 180)
    local vp_dir = Vector3.New(1, tan_angle, 0)
    if negative_x ~= nil then
        if (negative_x == true and vp_dir.x > 0) or (negative_x == false and vp_dir.x < 0) then
            vp_dir.x = -vp_dir.x
            vp_dir.y = -vp_dir.y
        end
    end
    if negative_y ~= nil then
        if (negative_y == true and vp_dir.y > 0) or (negative_y == false and vp_dir.y < 0) then
            vp_dir.y = -vp_dir.y
            vp_dir.x = -vp_dir.x
        end
    end
    -- Debuger.LogError("vp_dir=====[%s]",tostring(vp_dir))
    local world_vp_dir = cam.transform:TransformDirection(vp_dir)
    local re = world_vp_dir:ProjectOnPlane(Vector3.up):Normalize()
    return re
end

local diaup_speed = 30
local rotate_speed = 1440

function BattleSV:Diaup(on_com)
    if self.vo == nil then
        return
    end
    self.is_diauped = true
    local reach_edge_count = 0
    local cam = SceneMgr:MainCamera()
    local total_reach_edge = GameMath.Ramdon(2)
    --男左女右
    local diaup_dir = rnd_vp_dir(BattleData.IsEnemyIdx(self.vo.grid_idx) and 3 or 1, cam)
    AudioMgr:PlayEffect(AudioType.Battle,"JiNengJiFei")
    Runner.Instance:RunUntilTrue(
        function()
            if self.root_node == nil then
                return true
            end
            self.root_node:Rotate(Vector3.up, rotate_speed * Time.deltaTime)
            local offset = diaup_dir * diaup_speed * Time.deltaTime
            local target_pos = self:GetPos() + offset
            local vp_pos =
                cam:WorldToViewportPoint(Vector3.New(target_pos.x, target_pos.y + self:Height() / 2, target_pos.z))
            if reach_edge_count >= total_reach_edge then
                self:SetPos(target_pos)
                --judge diaup end
                if (vp_pos.x < -0.3 or vp_pos.x > 1.3 or vp_pos.y < -0.3 or vp_pos.y > 1.3) and not self.is_in_shihua then
                    if on_com ~= nil then
                        self.is_diauped = false
                        on_com()
                    end
                    return true
                end
            else
                local edge
                if vp_pos.x < 0.05 then
                    edge = 1
                elseif vp_pos.x > 0.95 then
                    edge = 3
                elseif vp_pos.y < 0.05 then
                    edge = 4
                elseif vp_pos.y > 0.95 then
                    edge = 2
                end
                if edge ~= nil then --change diaupdir
                    diaup_dir = rnd_vp_dir(edge, cam)
                    reach_edge_count = reach_edge_count + 1
                else
                    self:SetPos(target_pos)
                end
            end
            return false
        end
    )
end
function BattleSV:SetAnimActive()
    
end
function BattleSV:PlayAnim()
    return true
end
function BattleSV:Height()
    return 2.5
end
function BattleSV:StatusActive()
    SE:Stop(self.handle_unactive)
    self.handle_unactive = nil
    if self.handle_active == nil then
        self.handle_active = SE:Play(SceneEffectConfig.EffectList.HallowActive, nil, nil, self.root_node)
    end
end
function BattleSV:StatusUnactive()
    SE:Stop(self.handle_active)
    self.handle_active = nil
    if self.handle_unactive == nil then
        self.handle_unactive = SE:Play(SceneEffectConfig.EffectList.HallowUnactive, nil, nil, self.root_node)
    end
end
function BattleSV:StatusRecovery()
    SE:Stop(self.handle_active)
    SE:Stop(self.handle_unactive)
    self.handle_active = nil
    self.handle_unactive = nil
end