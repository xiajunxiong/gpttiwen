--* C# SkillFlyerPath

local GetGameObjectByIndex = Game.SkillManager.GetGameObjectByIndex

BattleFlyerPath = BattleFlyerPath or BaseClass()

function BattleFlyerPath:__init(path_inst, skill_param, FeatureInstance)
    self.is_end = false
    self.path_inst = path_inst
    self.path_inst.UpdatePathDel = BindTool.Bind(self.UpdatePathDel, self)
    self.skill_param = skill_param
end

function BattleFlyerPath:UpdatePathDel(time)
    if self.is_end then
        return
    end
    self:UpdatePath(time)
end

--override
function BattleFlyerPath:UpdatePath(time)
    self:OnHitPoint()
    self:OnEnd()
end

function BattleFlyerPath:OnHitPoint(hit_idx)
    if self.is_end then
        return
    end
    if hit_idx == nil then
        hit_idx = 0
    end
    self.path_inst:OnHitPoint(hit_idx)
end

function BattleFlyerPath:OnEnd()
    if self.is_end then
        return
    end
    self.is_end = true
    self.path_inst:OnEnd()
end

local function quickGenCoHelperBasedOnBegin(begin_pos, target_pos, up_dir)
    local begin_to_target_dir = (target_pos - begin_pos):SetNormalize()
    local up_dir = (up_dir or BattleData.Instance:SceneData():GetCenterPosTran().up):SetNormalize()
    local cross_dir = Vector3.Cross(up_dir, begin_to_target_dir):SetNormalize()
    return Game.CoordinateHelper(-cross_dir, up_dir, begin_to_target_dir, begin_pos)
end

local function quickGenCoHelperBasedOnCenter(begin_pos, target_pos, up_dir)
    local target_to_begin_dir = (begin_pos - target_pos):SetNormalize()
    local up_dir = (up_dir or BattleData.Instance:SceneData():GetCenterPosTran().up):SetNormalize()
    local cross_dir = Vector3.Cross(up_dir, target_to_begin_dir):SetNormalize()
    return Game.CoordinateHelper(target_to_begin_dir, up_dir, cross_dir, (target_pos + begin_pos) / 2)
end


BattleFlyerPathTeamCenter = BattleFlyerPathTeamCenter or BaseClass(BattleFlyerPath)

BattleFlyerPathTeamCenter.defualt_speed = 65
BattleFlyerPathTeamCenter.defualt_distance = 100
BattleFlyerPathTeamCenter.path_time = BattleFlyerPathTeamCenter.defualt_distance / BattleFlyerPathTeamCenter.defualt_speed

function BattleFlyerPathTeamCenter:__init(path_inst, skill_param)
    -- local move_speed = BattleFlyerPathTeamCenter.defualt_speed * self.path_inst.SpeedScale
    if BattleData.Instance:SceneData() == nil then
        return
    end
    self.begin_pos = path_inst.Tran.position
    if path_inst.PathParams.Length > 0 then--延迟结束
        self.hit_path = tonumber(path_inst.PathParams[0])
        self.deviation = tonumber(path_inst.PathParams[1])
    end
    local move_dir
    if skill_param.TargetIdxs.Length > 0 then
        self.dir_tar_pos = BattleData.Instance:SceneData():GetCampTransform(skill_param.TargetIdxs[0])
        self.dir_tar_pos = self.dir_tar_pos.position
        self.dir_tar_pos.y = self.dir_tar_pos.y + (self.deviation or 1.25)
        move_dir = self.dir_tar_pos - self.begin_pos

        self.hit_time = move_dir:Magnitude() / BattleFlyerPathTeamCenter.defualt_speed
        move_dir = move_dir:Normalize()
        path_inst.Tran.forward = move_dir
    else
        move_dir = path_inst.Tran.forward
    end
    local path_vec = move_dir * BattleFlyerPathTeamCenter.defualt_distance
    self.end_target_pos = path_vec + self.begin_pos
    self.path_time = BattleFlyerPathTeamCenter.path_time / self.path_inst.SpeedScale
    if self.hit_time ~= nil then
        self.hit_time = self.hit_time / self.path_inst.SpeedScale
    end
end

function BattleFlyerPathTeamCenter:UpdatePath(time)
    local per = time / self.path_time
    if per > 1 then
        per = 1
    end
    local he = false
    if self.hit_time ~= nil and time >= self.hit_time then
        if self.dir_tar_pos ~= nil then
            self.path_inst.Tran.position = self.dir_tar_pos --保证在触发打击事件时法球的位置在被打击者的位置上
            he = true--停一帧
        end
        self:OnHitPoint()
        self.hit_time = nil
    end
    if self.hit_time == nil and self.hit_path ~= nil then--路径结束且仅显示命中路径
        he = true
    end
    if per >= 1 then
        self:OnEnd()
        self.hit_path = nil
    end
    if he == false then
        if self.begin_pos and self.end_target_pos then
            self.path_inst.Tran.position = Vector3.Lerp(self.begin_pos, self.end_target_pos, per)
        end
    end
end


BattleFlyerPathStraight = BattleFlyerPathStraight or BaseClass(BattleFlyerPath)

BattleFlyerPathStraight.defualt_speed = 65
BattleFlyerPathStraight.defualt_distance = 100
BattleFlyerPathStraight.path_time = BattleFlyerPathStraight.defualt_distance / BattleFlyerPathStraight.defualt_speed

function BattleFlyerPathStraight:__init(path_inst, skill_param)
    -- local move_speed = BattleFlyerPathStraight.defualt_speed * self.path_inst.SpeedScale
    if BattleData.Instance:SceneData() == nil then
        return
    end
    self.begin_pos = path_inst.Tran.position
    if path_inst.PathParams.Length > 0 then--路径结束且仅显示命中路径(仅显示路径根据path_time结束)
        self.hit_path = tonumber(path_inst.PathParams[0])
        if path_inst.PathParams.Length > 1 then--路径结束且仅显示命中路径（仅显示路径且根据hit_time结束）
            self.notice_end = tonumber(path_inst.PathParams[1])
        end
    end
    local move_dir
    if skill_param.TargetIdxs.Length > 0 then
        self.dir_tar_pos = BattleData.Instance:SceneData():GetPos(skill_param.TargetIdxs[0])
        self.dir_tar_pos.y = self.dir_tar_pos.y + 1.25
        move_dir = self.dir_tar_pos - self.begin_pos

        self.hit_time = move_dir:Magnitude() / BattleFlyerPathStraight.defualt_speed
        move_dir = move_dir:Normalize()
        path_inst.Tran.forward = move_dir
    else
        move_dir = path_inst.Tran.forward
    end
    local path_vec = move_dir * BattleFlyerPathStraight.defualt_distance
    self.end_target_pos = path_vec + self.begin_pos
    self.path_time = BattleFlyerPathStraight.path_time / self.path_inst.SpeedScale
    if self.hit_time ~= nil then
        self.hit_time = self.hit_time / self.path_inst.SpeedScale
    end
end

local function distance(p1x, p1y, p2x, p2y, need_sqrt)
    local x_step = p1x - p2x
    local y_step = p1y - p2y
    local val = x_step * x_step + y_step * y_step
    if need_sqrt then
        return math.sqrt(val)
    end
    return val
end

function BattleFlyerPathStraight:UpdatePath(time)
    local per = time / self.path_time
    if per > 1 then
        per = 1
    end
    local he = false
    if self.hit_time ~= nil and time >= self.hit_time then
        if self.dir_tar_pos ~= nil then
            self.path_inst.Tran.position = self.dir_tar_pos --保证在触发打击事件时法球的位置在被打击者的位置上
            he = true--停一帧
        end
        self:OnHitPoint()
        self.hit_time = nil
    end
    if self.hit_time == nil and self.hit_path ~= nil then--路径结束且仅显示命中路径
        he = true
        if self.notice_end ~= nil then
            per = per < 1 and 1 or per--必须通知才能够结束逻辑（如果有结束特效的话就需要设置）
            self.notice_end = nil
        end
    end
    if per >= 1 then
        self:OnEnd()
        self.hit_path = nil
    end
    if he == false then
        if self.begin_pos and self.end_target_pos then
            self.path_inst.Tran.position = Vector3.Lerp(self.begin_pos, self.end_target_pos, per)
        end
    end
end
BattleFlyerBaTianHuiXuan = BattleFlyerBaTianHuiXuan or BaseClass(BattleFlyerPath)
BattleFlyerBaTianHuiXuan.defualt_speed = 65
function BattleFlyerBaTianHuiXuan:__init(path_inst, skill_param, featureInstance)
    self.featureInstance = featureInstance
    self.begin_pos = path_inst.Tran.position
    self.dir_tar_pos = BattleData.Instance:SceneData():GetRowPosForward(skill_param.TargetIdxs[0])
    self.dir_tar_pos.y = self.dir_tar_pos.y + 1.25
    local stay_offset = 1
    if path_inst.PathParams.Length > 0 then--停滞时间
        stay_offset = tonumber(path_inst.PathParams[0])
    end
    local move_dir = self.dir_tar_pos - self.begin_pos
    self.hit_time = move_dir:Magnitude() / BattleFlyerBaTianHuiXuan.defualt_speed / self.path_inst.SpeedScale
    self.path_time = self.hit_time
    self.stay_time = self.hit_time + stay_offset
    self.end_time = self.stay_time + self.path_time
end
function BattleFlyerBaTianHuiXuan:UpdatePath(time)
    local per = time / self.path_time
    if per > 1 then
        per = 1
    end
    if time < self.path_time then
        self.path_inst.Tran.position = Vector3.Lerp(self.begin_pos, self.dir_tar_pos, per)
    end
    if self.hit_time ~= nil and time >= self.hit_time then
        self:OnHitPoint()
        self.featureInstance:ThrowTrigger("huixuan_hit", self.skill_param)
        self.hit_time = nil
    end
    if self.stay_time and time >= self.stay_time then
        per = (time - self.stay_time) /self.path_time
        self.path_inst.Tran.position = Vector3.Lerp(self.dir_tar_pos, self.begin_pos, per)
    end
    if time >= self.end_time then
        self:OnEnd()
    end
end

BattleFlyerXuanZhuanFeiFu = BattleFlyerXuanZhuanFeiFu or BaseClass(BattleFlyerPath)

BattleFlyerXuanZhuanFeiFu.defualt_speed = 30
BattleFlyerXuanZhuanFeiFu.euler_y = 55

local function CreateDebugObject(name, position, foward)
    -- LogErrorHL(name, position)
    local obj = UnityEngine.GameObject(tostring(name))
    obj:SetPosition(position)
    if foward then
        obj:SetForward(foward)
    end
end

function BattleFlyerXuanZhuanFeiFu:__init(path_inst, skill_param)
    local target_idxs = skill_param.TargetIdxs:ToTable()
    local tar_idx_sam = target_idxs[1]
    local side_pos_up, side_pos_down = BattleData.Instance:SceneData():GetRowSidePoint(tar_idx_sam, 2)
    local launch_pos = path_inst.Tran.position
    side_pos_up.y = launch_pos.y
    side_pos_down.y = launch_pos.y
    local straight_dir = side_pos_down - side_pos_up
    local straight_mag = straight_dir:Magnitude()

    local shoot_dir, return_dir
    if BattleData.IsEnemyIdx(skill_param.PlayerIdx) then
        shoot_dir = BattleData.Instance:SceneData():GetForward(skill_param.PlayerIdx, Vector3.New(0, -BattleFlyerXuanZhuanFeiFu.euler_y, 0))
        return_dir = -BattleData.Instance:SceneData():GetForward(skill_param.PlayerIdx, Vector3.New(0, BattleFlyerXuanZhuanFeiFu.euler_y, 0))
    else
        shoot_dir = BattleData.Instance:SceneData():GetForward(skill_param.PlayerIdx, Vector3.New(0, BattleFlyerXuanZhuanFeiFu.euler_y, 0))
        return_dir = -BattleData.Instance:SceneData():GetForward(skill_param.PlayerIdx, Vector3.New(0, -BattleFlyerXuanZhuanFeiFu.euler_y, 0))
    end

    self.straight_in_pos = side_pos_up
    self.straight_out_pos = side_pos_down

    self.shoot_curve = Game.CurveParabola(launch_pos, shoot_dir, self.straight_in_pos, straight_dir)
    self.shoot_time = self.shoot_curve:TotalTime(BattleFlyerXuanZhuanFeiFu.defualt_speed)

    self.return_curve = Game.CurveParabola(self.straight_out_pos, straight_dir, launch_pos, return_dir)
    self.return_time = self.return_curve:TotalTime(BattleFlyerXuanZhuanFeiFu.defualt_speed)

    self.straight_time = straight_mag / BattleFlyerXuanZhuanFeiFu.defualt_speed

    self.shoot_time = self.shoot_time / self.path_inst.SpeedScale
    self.return_time = self.return_time / self.path_inst.SpeedScale
    self.straight_time = self.straight_time / self.path_inst.SpeedScale

    -- CreateDebugObject("StraightIn",self.straight_in_pos,straight_dir)
    -- CreateDebugObject("StraightOut",self.straight_out_pos,straight_dir)

    -- CreateDebugObject("Shoot",launch_pos,shoot_dir)
    -- CreateDebugObject("Return",launch_pos,return_dir)

    self.hit_event_pers = {}
    ---key:idx,straight per
    for _, idx in pairs(target_idxs) do
        local idx_pos = BattleData.Instance:SceneData():GetPos(idx)
        idx_pos.y = launch_pos.y
        local hit_per = (idx_pos - self.straight_in_pos):Magnitude() / straight_mag
        self.hit_event_pers[idx] = hit_per
    end
end

function BattleFlyerXuanZhuanFeiFu:UpdatePath(time)
    if time < self.shoot_time then --发射曲线
        local per = time / self.shoot_time
        local p = self.shoot_curve:GetPos(per)
        self.path_inst.Tran.position = p
    elseif time < self.shoot_time + self.straight_time then --直线
        local per = (time - self.shoot_time) / self.straight_time
        local p = Vector3.Lerp(self.straight_in_pos, self.straight_out_pos, per)
        self.path_inst.Tran.position = p
        for hit_idx, hit_per in pairs(self.hit_event_pers) do
            if per >= hit_per then
                self.hit_event_pers[hit_idx] = nil
                self:OnHitPoint(hit_idx)
                break
            end
        end
    elseif time < self.shoot_time + self.straight_time + self.return_time then --回归曲线
        local per = (time - self.shoot_time - self.straight_time) / self.return_time
        local p = self.return_curve:GetPos(per)
        self.path_inst.Tran.position = p
    else --结束
        for hit_idx, _ in pairs(self.hit_event_pers) do
            self:OnHitPoint(hit_idx)
        end
        self.hit_event_pers = {}
        self:OnEnd()
    end
end

BattleFlyerPureParabola = BattleFlyerPureParabola or BaseClass(BattleFlyerPath)

BattleFlyerPureParabola.defualt_speed = 10
function BattleFlyerPureParabola:__init(path_inst, skill_param, featureInstance)
    -- LogError("BattleFlyerPureParabola __init!!!!!")
    self.target_idxs = skill_param.TargetIdxs:ToTable()
    self.featureInstance = featureInstance
    -- for k, v in pairs(self.target_idxs) do
    --     LogError(k, v)
    -- end
    if skill_param.TargetIdxs.Length > 0 then
        -- self.a = diff_coef * 2
        -- self.b = self.a * self.x0 * self.x0
        -- self.CoordinateHelper = quickGenerateCoordinateHelper(self.begin_pos, self.target_pos, )
        -- LogError(self.hit_time)
        local begin_pos = path_inst.Tran.position
        local tar_pos = BattleData.Instance:SceneData():GetPos(self.target_idxs[1])
        -- local cross_pos = tar_pos:Clone()
        -- cross_pos.y = cross_pos.y + 1
        -- -- dir_tar_pos.y = dir_tar_pos.y + 1.25
        -- self.curve = Game.CurveParabola(begin_pos, tar_pos, (tar_pos + begin_pos) / 2 + BattleData.Instance:SceneData():GetCenterPosTran().up:SetNormalize(), 1)
        -- -- CreateDebugObject("begin_pos", begin_pos)
        -- -- CreateDebugObject("tar_pos", tar_pos)
        -- -- LogError("cross_pos", cross_pos)
        -- self.hit_time = self.curve:TotalTime(BattleFlyerPureParabola.defualt_speed * self.path_inst.SpeedScale)

        local center_tran = BattleData.Instance:SceneData():GetCenterPosTran()
        -- local diff_coef = tonumber(path_inst.PathParams[0])
        if path_inst.PathParams.Length > 0 then
            self.b = tonumber(path_inst.PathParams[0])
        else
            self.b = 5
        end
        local plane_angle = 0
        if path_inst.PathParams.Length > 1 then
            plane_angle = tonumber(path_inst.PathParams[1])
        end
        if path_inst.PathParams.Length > 2 and tonumber(path_inst.PathParams[2]) ~= 0 then
            self.acc = tonumber(path_inst.PathParams[2]) * BattleFlyerPureParabola.defualt_speed
        end
        local plane_radian = plane_angle * math.pi / 180
        local plane_up_dir = center_tran.up:SetNormalize() + center_tran.right:SetNormalize() * math.tan(plane_radian)

        self.CoordinateHelper = quickGenCoHelperBasedOnCenter(begin_pos, tar_pos, plane_up_dir)

        if self.acc then
            local v = self.path_inst.SpeedScale * BattleFlyerPureParabola.defualt_speed
            local d = Vector3.Distance(begin_pos, tar_pos)
            self.hit_time = (math.sqrt(v * v + 2 * self.acc * d) - v) / self.acc
            self.d = d
            self.v = v
        else
            self.hit_time = Vector3.Distance(begin_pos, tar_pos) / self.path_inst.SpeedScale / BattleFlyerPureParabola.defualt_speed
        end
        self.x0 = Vector3.Distance(self.CoordinateHelper:WorldToLocal(begin_pos), Vector3.zero)
        self.a = self.b / self.x0 / self.x0
    end
end

function BattleFlyerPureParabola:UpdatePath(time)
    if time < self.hit_time then
        if self.acc then
            local x = self.x0 - (self.acc * time * time / 2 + self.v * time)
            local y = self.b - self.a * x * x
            local time_pre = time - 0.01
            local x_pre = self.x0 - (self.acc * time_pre * time_pre / 2 + self.v * time_pre)
            local y_pre = self.b - self.a * x_pre * x_pre

            self.path_inst.Tran.forward = self.CoordinateHelper:LocalToWorld(Vector3.New(x - x_pre, y - y_pre, 0), false)
            self.path_inst.Tran.position = self.CoordinateHelper:LocalToWorld(Vector3.New(x, y, 0))
        else
            local per = time / self.hit_time
            local x = self.x0 * (1 - per) - self.x0 * per
            local y = self.b - self.a * x * x

            local per_pre = (time - 0.01) / self.hit_time
            local x_pre = self.x0 * (1 - per_pre) - self.x0 * per_pre
            local y_pre = self.b - self.a * x_pre * x_pre

            self.path_inst.Tran.forward = self.CoordinateHelper:LocalToWorld(Vector3.New(x - x_pre, y - y_pre, 0), false)
            self.path_inst.Tran.position = self.CoordinateHelper:LocalToWorld(Vector3.New(x, y, 0))
        end
    else
        for _, idx in pairs(self.target_idxs) do
            self:OnHitPoint(idx)
            self.featureInstance:ThrowTrigger("parabola_hit", self.skill_param)
        end
        self:OnEnd()
    end
end

BattleFlyerLuoYingLianFu = BattleFlyerLuoYingLianFu or BaseClass(BattleFlyerPath)

BattleFlyerLuoYingLianFu.defualt_speed = 10
function BattleFlyerLuoYingLianFu:__init(path_inst, skill_param)
    -- LogError("BattleFlyerLuoYingLianFu __init!!!!!")
    self.target_idxs = skill_param.TargetIdxs:ToTable()
    if skill_param.TargetIdxs.Length > 0 then
        -- self.hit_time = math.pi * self.radius / self.speed
        -- self.end_time = 2 * self.hit_time
        -- CreateDebugObject("begin_pos", begin_pos)
        -- CreateDebugObject("tar_pos", target_pos)
        -- CreateDebugObject("center_pos", center_pos)
        local begin_pos = path_inst.Tran.position
        local target_pos = BattleData.Instance:SceneData():GetPos(self.target_idxs[1])
        self.target_pos = target_pos
        -- local center_pos = (begin_pos + target_pos) / 2
        local center_pos = target_pos

        local begin_to_target_dir = (target_pos - begin_pos):SetNormalize()
        local up_dir = BattleData.Instance:SceneData():GetCenterPosTran().up:SetNormalize()
        local cross_dir = Vector3.Cross(up_dir, begin_to_target_dir):SetNormalize()

        local clockwise = path_inst.PathParams[2] == "1"
        if clockwise then
            self.CoordinateHelper = Game.CoordinateHelper(-cross_dir, up_dir, -begin_to_target_dir, center_pos)
        else
            self.CoordinateHelper = Game.CoordinateHelper(cross_dir, up_dir, -begin_to_target_dir, center_pos)
        end

        self.radius = Vector3.Distance(begin_pos, center_pos)
        self.speed = BattleFlyerLuoYingLianFu.defualt_speed * self.path_inst.SpeedScale
        -- self.max_radian = 45/360*2*math.pi
        self.acceleration = tonumber(path_inst.PathParams[0])
        self.max_angle = tonumber(path_inst.PathParams[1])
        self.path1 = self.max_angle / 360 * 2 * math.pi * self.radius
        self.time1 = self.path1 / self.speed
        self.path2 = self.radius
        self.time2 = math.sqrt(2 * self.path2 / self.acceleration)
        self.inflection_pos = Vector3.New(self.radius * math.sin(self.max_angle / 360 * 2 * math.pi), 1, self.radius * math.cos(self.max_angle / 360 * 2 * math.pi))
    else
        LogErrorBT("No Target!!!")
    end
end

function BattleFlyerLuoYingLianFu:UpdatePath(time)
    if time < self.time1 then
        local radian = time * self.speed / self.radius
        local pos = Vector3.New(self.radius * math.sin(radian), 1, self.radius * math.cos(radian))
        self.path_inst.Tran.position = self.CoordinateHelper:LocalToWorld(pos)
    else
        local per = 0.5 * self.acceleration * (time - self.time1) * (time - self.time1) / self.path2
        local pos = Vector3.Lerp(self.inflection_pos, Vector3.zero, per)
        self.path_inst.Tran.position = self.CoordinateHelper:LocalToWorld(pos)
        if time >= self.time1 + self.time2 then
            self:OnHitPoint()
            self:OnEnd()
        end
    end
end

BattleFlyerFengRen = BattleFlyerFengRen or BaseClass(BattleFlyerPath)

BattleFlyerFengRen.defualt_speed = 10
function BattleFlyerFengRen:__init(path_inst, skill_param, featureInstance)
    self.target_idxs = skill_param.TargetIdxs:ToTable()
    self.featureInstance = featureInstance
    self.skill_param = skill_param
    if skill_param.TargetIdxs.Length > 0 then
        -- self.end_dir = self.CoordinateHelper:LocalToWorld(Vector3.New(math.random(-1000, 1000) / 1000, 0, math.random(-1000, 1000) / 1000)):SetNormalize()
        -- LogError(self.end_dir)
        -- LogError(self.hit_time)
        -- LogError(self.end_time - self.hit_time)
        -- LogError(self.end_time - self.hit_time)
        -- local guide_pos3_local = Vector3.Lerp(target_pos_local, end_pos_local, 1/3)
        -- guide_pos3_local.x = guide_pos3_local.x + math.random(-5, 5)
        -- local guide_pos4_local = Vector3.Lerp(target_pos_local, end_pos_local, 2/3)
        -- guide_pos4_local.x = guide_pos4_local.x + math.random(-5, 5)
        -- self.guide_pos3 = self.CoordinateHelper:LocalToWorld(guide_pos3_local)
        -- self.guide_pos4 = self.CoordinateHelper:LocalToWorld(guide_pos4_local)
        -- CreateDebugObject("guide_pos3", self.guide_pos3)
        -- CreateDebugObject("guide_pos4", self.guide_pos4)
        -- local ran1 = math.random(-90, 90)
        -- local ran2 = math.random(-90, -60)
        -- LogError(ran2)
        -- plr_dir = BattleData.Instance:SceneData():GetForward(skill_param.PlayerIdx, Vector3.New(0, ran1, 0))
        -- tar_dir = BattleData.Instance:SceneData():GetForward(skill_param.PlayerIdx, Vector3.New(0, ran1 + ran2, 0))
        -- LogError(plr_dir - tar_dir)
        -- self.curve = Game.CurveParabola(plr_pos, plr_dir, tar_pos, tar_dir)
        -- self.time = self.curve:TotalTime(self.speed)
        self.speed = BattleFlyerFengRen.defualt_speed * self.path_inst.SpeedScale
        self.hit_time = 10 / self.speed
        self.end_time = self.hit_time + 10 / self.speed

        -- self.begin_pos = BattleData.Instance:SceneData():GetPos(skill_param.PlayerIdx)
        self.begin_pos = BattleData.Instance:SceneData():GetCenterPosTran().position
        self.target_pos = BattleData.Instance:SceneData():GetPos(self.target_idxs[1])
        -- self.sup_pos = Vector3.New(self.begin_pos.x + math.random(-10, 10), self.begin_pos.y + math.random(-10, 10), (self.begin_pos.z + self.tar_pos.z) / 2)

        local begin_to_target_dir = (self.target_pos - self.begin_pos):SetNormalize()
        local up_dir = BattleData.Instance:SceneData():GetCenterPosTran().up:SetNormalize()
        local cross_dir = Vector3.Cross(up_dir, begin_to_target_dir):SetNormalize()
        self.CoordinateHelper = Game.CoordinateHelper(-cross_dir, up_dir, begin_to_target_dir, self.begin_pos)

        -- self.guide_pos = Vector3.New(math.random(-10, 10), math.random(-10, 10), 0)
        self.guide_pos1 = self.CoordinateHelper:LocalToWorld(Vector3.New(math.random(-10, 10), 0, math.random(-5, 5)))
        self.guide_pos2 = self.CoordinateHelper:LocalToWorld(Vector3.New(math.random(-10, 10), 0, math.random(-5, 5)))
        -- LogError(math.random(-1000, 1000) / 1000)
        -- LogError(math.random(-1000, 1000) / 1000)
        -- LogError(math.random(-1000, 1000) / 1000)
        -- local end_pos_local = Vector3.New(math.random(-10, 10), 0, math.random(10, 15))
        -- local target_pos_local = self.CoordinateHelper:LocalToWorld(self.target_pos)
        -- self.end_pos = self.CoordinateHelper:LocalToWorld(end_pos_local)
        self.end_pos = self.guide_pos2 + (self.target_pos - self.guide_pos2):SetNormalize() * 20
    else
        LogErrorBT("No Target!!!")
    end
end

local function bezier2(p0, p1, p2, t)
    local p0p1 = p0 * (1 - t) + p1 * t
    local p1p2 = p1 * (1 - t) + p2 * t
    return p0p1 * (1 - t) + p1p2 * t
end

local function bezier3(p0, p1, p2, p3, t)
    return p0 * math.pow((1 - t), 3) + p1 * (3 * t * math.pow((1 - t), 2)) + p2 * (3 * (1 - t) * math.pow(t, 2)) + p3 * math.pow(t, 3)
end

function BattleFlyerFengRen:UpdatePath(time)
    if not self.is_hit then
        if time <= self.hit_time then
            -- CreateDebugObject(time, bezier3(self.begin_pos, self.guide_pos1, self.guide_pos2, self.target_pos, per))
            local per = time / self.hit_time
            self.path_inst.Tran.position = bezier3(self.begin_pos, self.guide_pos1, self.guide_pos2, self.target_pos, per)
        else
            self.is_hit = true
            self:OnHitPoint()
            self.featureInstance:ThrowTrigger("fengren_hit", self.skill_param)
        end
    else
        if time <= self.end_time then
            -- self.path_inst.Tran:Translate(self.path_inst.Tran.forward / 50 * self.speed)
            -- self.path_inst.Tran:Translate(self.end_dir / 50 * self.speed)
            local per = (time - self.hit_time) / (self.end_time - self.hit_time)
            -- self.path_inst.Tran.position = bezier3(self.target_pos, self.guide_pos3, self.guide_pos4, self.end_pos, per)
            -- CreateDebugObject(time, bezier3(self.begin_pos, self.guide_pos1, self.guide_pos2, self.target_pos, per))
            self.path_inst.Tran.position = Vector3.Lerp(self.target_pos, self.end_pos, per)
        else
            self:OnEnd()
        end
    end
end

BattleFlyerSealCard = BattleFlyerSealCard or BaseClass(BattleFlyerPath)

BattleFlyerSealCard.defualt_speed = 10
function BattleFlyerSealCard:__init(path_inst, skill_param)
    self.target_idxs = skill_param.TargetIdxs:ToTable()
    if skill_param.TargetIdxs.Length > 0 then
        -- self.guide_pos1 = self.CoordinateHelper:LocalToWorld(Vector3.New(math.random(-10, 10), 0, math.random(-5, 5)))
        -- self.guide_pos2 = self.CoordinateHelper:LocalToWorld(Vector3.New(math.random(-10, 10), 0, math.random(-5, 5)))
        -- self.end_pos = self.guide_pos2 + (self.target_pos - self.guide_pos2):SetNormalize() * 80
        -- self.speed = BattleFlyerFengRen.defualt_speed * self.path_inst.SpeedScale
        -- self.hit_time = 10 / self.speed
        -- self.end_time = self.hit_time + 30 / self.speed

        self.begin_pos = BattleData.Instance:SceneData():GetCenterPosTran().position
        self.target_pos = BattleData.Instance:SceneData():GetPos(self.target_idxs[1])

        self.CoordinateHelper = quickGenCoHelperBasedOnBegin(self.begin_pos, self.target_pos)
    else
        LogErrorBT("No Target!!!")
    end
end

function BattleFlyerSealCard:UpdatePath(time)
    self.path_inst.Tran:Rotate(BattleData.Instance:SceneData():GetCenterPosTran().up:SetNormalize(), time)
    -- if not self.is_hit then
    --     if time <= self.hit_time then
    --         -- CreateDebugObject(time, bezier3(self.begin_pos, self.guide_pos1, self.guide_pos2, self.target_pos, per))
    --         local per = time / self.hit_time
    --         self.path_inst.Tran.position = bezier3(self.begin_pos, self.guide_pos1, self.guide_pos2, self.target_pos, per)
    --     else
    --         self.is_hit = true
    --         self:OnHitPoint()
    --     end
    -- else
    --     if time <= self.end_time then
    --         local per = (time - self.hit_time) / (self.end_time - self.hit_time)
    --         -- self.path_inst.Tran.position = bezier3(self.target_pos, self.guide_pos3, self.guide_pos4, self.end_pos, per)
    --         -- CreateDebugObject(time, bezier3(self.begin_pos, self.guide_pos1, self.guide_pos2, self.target_pos, per))
    --         self.path_inst.Tran.position = Vector3.Lerp(self.target_pos, self.end_pos, per)
    --     else
    --         self:OnEnd()
    --     end
    -- end
end

BattleFlyerShanDianLian = BattleFlyerShanDianLian or BaseClass(BattleFlyerPath)
BattleFlyerShanDianLian.defualt_speed = 60
BattleFlyerShanDianLian.defualt_distance = 100
BattleFlyerShanDianLian.path_time = BattleFlyerShanDianLian.defualt_distance / BattleFlyerShanDianLian.defualt_speed
function BattleFlyerShanDianLian:__init(path_inst, skill_param)
    self.begin_pos = path_inst.Tran.position
    local move_dir
    if skill_param.TargetIdxs.Length > 0 then
        self.dir_tar_pos = BattleData.Instance:SceneData():GetPos(skill_param.TargetIdxs[0])
        self.dir_tar_pos.y = self.dir_tar_pos.y + 1.25
        move_dir = self.dir_tar_pos - self.begin_pos

        self.hit_time = move_dir:Magnitude() / BattleFlyerShanDianLian.defualt_speed
        move_dir = move_dir:Normalize()
        path_inst.Tran.forward = move_dir
    else
        move_dir = path_inst.Tran.forward
    end
    local path_vec = move_dir * BattleFlyerShanDianLian.defualt_distance
    self.end_target_pos = path_vec + self.begin_pos
    self.path_time = BattleFlyerShanDianLian.path_time / self.path_inst.SpeedScale
    if self.hit_time ~= nil then
        self.hit_time = self.hit_time / self.path_inst.SpeedScale
    end
    --hit_time要小于path_time才有足够事件显示淡出
    --LogError(self.hit_time,self.path_time)
end
function BattleFlyerShanDianLian:UpdatePath(time)
    local per = time / self.path_time
    if per > 1 then
        per = 1
    end
    local he = false
    if self.hit_time ~= nil and time >= self.hit_time then
        if self.dir_tar_pos ~= nil then
            self.path_inst.Tran.position = self.dir_tar_pos --保证在触发打击事件时法球的位置在被打击者的位置上
            he = true
        end
        self:OnHitPoint()
        self.hit_time = nil
    end
    if per >= 1 then
        self:OnEnd()
    end
    if he == false then
        self.path_inst.Tran.position = Vector3.Lerp(self.begin_pos, self.end_target_pos, per)
    end
end

BattleFlyerXianLingZhuFu = BattleFlyerXianLingZhuFu or BaseClass(BattleFlyerPath)

BattleFlyerXianLingZhuFu.defualt_speed = 10
local xianlingzhufu_hit_count = 0
function BattleFlyerXianLingZhuFu:__init(path_inst, skill_param, featureInstance)
    self.path_inst = path_inst
    self.featureInstance = featureInstance
    self.total_hit_num = self.featureInstance.TotalFlyerCount
    --[[ if self.total_hit_num == 1 then
        xianlingzhufu_hit_count = 0
    end ]]
    if skill_param.TargetIdxs.Length > 0 then
        -- LogError(self.hit_time)
        local begin_pos = path_inst.Tran.position
        local tar_pos = BattleData.Instance:SceneData():GetPos(skill_param.PlayerIdx)
        local cross_pos = tar_pos:Clone()
        cross_pos.y = cross_pos.y + 1
        -- dir_tar_pos.y = dir_tar_pos.y + 1.25
        local height_scale = path_inst.PathParams.Length > 0 and tonumber(path_inst.PathParams[0]) or 1
        self.curve = Game.CurveParabola(begin_pos, tar_pos, (tar_pos + begin_pos) / 2 + BattleData.Instance:SceneData():GetCenterPosTran().up:SetNormalize(), height_scale)
        -- CreateDebugObject("begin_pos", begin_pos)
        -- CreateDebugObject("tar_pos", tar_pos)
        -- LogError("cross_pos", cross_pos)

        if path_inst.PathParams.Length > 1 then
            self.acc = tonumber(path_inst.PathParams[1])
        end
            
        if self.acc ~= nil and self.acc > 0 then
            self.v0 = BattleFlyerXianLingZhuFu.defualt_speed * self.path_inst.SpeedScale 
            self.s = Vector3.Distance(begin_pos, tar_pos)
            self.hit_time = (math.sqrt(self.v0 * self.v0 + 2 * self.acc * self.s) - self.v0) / self.acc
        else
            self.hit_time = self.curve:TotalTime(BattleFlyerXianLingZhuFu.defualt_speed * self.path_inst.SpeedScale)
        end
        

        -- self.v0 = (1/s) * v0
    else
        -- LogError("No Target!!!")
    end
end

function BattleFlyerXianLingZhuFu:UpdatePath(time)
    -- local time = time-- * self.path_inst.SpeedScale
    if self.acc == nil or self.acc <= 0 then
        time = time * self.path_inst.SpeedScale
    end
    -- LogError("BattleFlyerPureParabola UpdatePath!!!!!")
    if time < self.hit_time then
        local per = 0
        if self.acc == nil or self.acc <= 0 then
            per = time / self.hit_time
        else
            local cs = self.v0 * time + self.acc * time * time / 2
            per = cs / self.s
        end
        local p = self.curve:GetPos(per)
        self.path_inst.Tran.position = p
        local p_pre = self.curve:GetPos(per - 0.01)
        self.path_inst.Tran.forward = p - p_pre
    else
        --[[ xianlingzhufu_hit_count = xianlingzhufu_hit_count + 1
        if self.total_hit_num and xianlingzhufu_hit_count == self.total_hit_num then
            xianlingzhufu_hit_count = 0
        end ]]
        self.featureInstance:ThrowTrigger(BattleSkillTriggerKey.xianlingzhufu_hit, self.skill_param)
        self:OnEnd()
    end
end

BattleFlyerFollow = BattleFlyerFollow or BaseClass(BattleFlyerPath)

BattleFlyerFollow.defualt_speed = 10

function BattleFlyerFollow:__init(path_inst, skill_param, featureInstance)
    self.follow_target = GetGameObjectByIndex(skill_param.PlayerIdx)
end

function BattleFlyerFollow:UpdatePath(time)
    if self.follow_target then
        self.path_inst.Tran.position = self.follow_target.transform.position
        self.path_inst.Tran.forward = self.follow_target.transform.forward
    end
end

BattleFlyerRanDomParabola = BattleFlyerRanDomParabola or BaseClass(BattleFlyerPath)
BattleFlyerRanDomParabola.defualt_speed = 10

local rand_angle_index = 1
function BattleFlyerRanDomParabola:__init(path_inst, skill_param)
    self.target_idxs = skill_param.TargetIdxs:ToTable()
    if skill_param.TargetIdxs.Length > 0 then
        local begin_pos = path_inst.Tran.position
        local tar_pos = BattleData.Instance:SceneData():GetPos(self.target_idxs[1])
        local center_tran = BattleData.Instance:SceneData():GetCenterPosTran()

        self.b = GameMath.Ramdon(path_inst.PathParams[0], path_inst.PathParams[1])

        if path_inst.PathParams.Length > 2 and tonumber(path_inst.PathParams[2]) ~= 0 then
            self.acc = tonumber(path_inst.PathParams[2]) * BattleFlyerRanDomParabola.defualt_speed
        end

       --[[  local plane_angle = rand_angle_index == 1 and 90 or -90
        rand_angle_index = rand_angle_index == 1 and 0 or 1 ]]

        local plane_angle = GameMath.Ramdon(-90, 90)
        local plane_radian = plane_angle * math.pi / 180
        local plane_up_dir = center_tran.up:SetNormalize() + center_tran.right:SetNormalize() * math.tan(plane_radian)

        self.CoordinateHelper = quickGenCoHelperBasedOnCenter(begin_pos, tar_pos, plane_up_dir)

        if self.acc then
            local v = self.path_inst.SpeedScale * BattleFlyerRanDomParabola.defualt_speed
            local d = Vector3.Distance(begin_pos, tar_pos)
            self.hit_time = (math.sqrt(v * v + 2 * self.acc * d) - v) / self.acc
            self.d = d
            self.v = v
        else
            self.hit_time = Vector3.Distance(begin_pos, tar_pos) / self.path_inst.SpeedScale / BattleFlyerRanDomParabola.defualt_speed
        end
        self.x0 = Vector3.Distance(self.CoordinateHelper:WorldToLocal(begin_pos), Vector3.zero)
        self.a = self.b / self.x0 / self.x0
    end
end

function BattleFlyerRanDomParabola:UpdatePath(time)
    if time < self.hit_time then
        if self.acc then
            local x = self.x0 - (self.acc * time * time / 2 + self.v * time)
            local y = self.b - self.a * x * x
            local time_pre = time - 0.01
            local x_pre = self.x0 - (self.acc * time_pre * time_pre / 2 + self.v * time_pre)
            local y_pre = self.b - self.a * x_pre * x_pre

            self.path_inst.Tran.forward = self.CoordinateHelper:LocalToWorld(Vector3.New(x - x_pre, y - y_pre, 0), false)
            self.path_inst.Tran.position = self.CoordinateHelper:LocalToWorld(Vector3.New(x, y, 0))
        else
            local per = time / self.hit_time
            local x = self.x0 * (1 - per) - self.x0 * per
            local y = self.b - self.a * x * x

            local per_pre = (time - 0.01) / self.hit_time
            local x_pre = self.x0 * (1 - per_pre) - self.x0 * per_pre
            local y_pre = self.b - self.a * x_pre * x_pre

            self.path_inst.Tran.forward = self.CoordinateHelper:LocalToWorld(Vector3.New(x - x_pre, y - y_pre, 0), false)
            self.path_inst.Tran.position = self.CoordinateHelper:LocalToWorld(Vector3.New(x, y, 0))
        end
    else
        for _, idx in pairs(self.target_idxs) do
            self:OnHitPoint(idx)
        end
        self:OnEnd()
    end
end

BattleFlyerRandomBezier = BattleFlyerRandomBezier or BaseClass(BattleFlyerPath)

BattleFlyerRandomBezier.defualt_speed = 10
function BattleFlyerRandomBezier:__init(path_inst, skill_param, featureInstance)
    self.target_idxs = skill_param.TargetIdxs:ToTable()
    self.featureInstance = featureInstance
    self.skill_param = skill_param
    if skill_param.TargetIdxs.Length > 0 then
        self.speed = BattleFlyerRandomBezier.defualt_speed * self.path_inst.SpeedScale
        self.hit_time = 10 / self.speed
        self.end_time = self.hit_time + 10 / self.speed

        self.begin_pos = path_inst.Tran.position--BattleData.Instance:SceneData():GetPos(skill_param.PlayerIdx)
        -- self.begin_pos = BattleData.Instance:SceneData():GetCenterPosTran().position
        self.target_pos = BattleData.Instance:SceneData():GetPos(self.target_idxs[1])

        local begin_to_target_dir = (self.target_pos - self.begin_pos):SetNormalize()
        local up_dir = BattleData.Instance:SceneData():GetCenterPosTran().up:SetNormalize()
        local cross_dir = Vector3.Cross(up_dir, begin_to_target_dir):SetNormalize()
        self.CoordinateHelper = Game.CoordinateHelper(-cross_dir, up_dir, begin_to_target_dir, self.begin_pos)

        self.guide_pos1 = self.CoordinateHelper:LocalToWorld(Vector3.New(math.random(-10, 10), 0, math.random(-5, 5)))
        self.guide_pos2 = self.CoordinateHelper:LocalToWorld(Vector3.New(math.random(-10, 10), 0, math.random(-5, 5)))

        self.end_pos = self.target_pos
        -- self.end_pos = self.guide_pos2 + (self.target_pos - self.guide_pos2):SetNormalize() * 20
    else
        LogErrorBT("No Target!!!")
    end
end

function BattleFlyerRandomBezier:UpdatePath(time)
    if not self.is_hit then
        if time <= self.hit_time then
            -- CreateDebugObject(time, bezier3(self.begin_pos, self.guide_pos1, self.guide_pos2, self.target_pos, per))
            local per = time / self.hit_time
            self.path_inst.Tran.position = bezier3(self.begin_pos, self.guide_pos1, self.guide_pos2, self.target_pos, per)
        else
            self.is_hit = true
            self:OnHitPoint()
            self.featureInstance:ThrowTrigger("fengren_hit", self.skill_param)
        end
    else
        if time <= self.end_time then
            local per = (time - self.hit_time) / (self.end_time - self.hit_time)
            -- CreateDebugObject(time, bezier3(self.begin_pos, self.guide_pos1, self.guide_pos2, self.target_pos, per))
            self.path_inst.Tran.position = Vector3.Lerp(self.target_pos, self.end_pos, per)
        else
            self:OnEnd()
        end
    end
end

--* path_inst:SkillFlyerPath    skill_param:SkillPlayParam
function BattleFlyerPath.Run(path_inst, skill_param, featureInstance)
    _G[path_inst.PathTypeKey].New(path_inst, skill_param, featureInstance)
end
