BattleSceneData = BattleSceneData or BaseClass()

local function create_pos_data(f2c, b2f, side)
    return {f2c = f2c, b2f = b2f, side = side}
end
local function default_pos_data_static(f2c, b2f, side)
    return {f2c = 3.6, b2f = 3.6, side = 3}
end
local function hexagon_pos_data_static()
    return {f2c = 5.049, b2f = 3.366, side = 2.9155}
end

local default_pos_data = nil
if PreloadCtrl ~= nil then
    PreloadCtrl.RegisterPreload(
        function()
            PreloadCtrl.RegisterLoading()
            ResMgr:LoadObject(
                "BattlePosCoreData.asset",
                function(obj)
                    default_pos_data = create_pos_data(obj.F2CDis, obj.B2FDis, obj.SideDis)
                    PreloadCtrl.UnregisterLoading()
                end,
                false
            )
        end
    )
end

function BattleSceneData:__init(data_holder)
    self.center_pos_tran = data_holder.CenterPos
    local core_data = data_holder.PosCoreData
    if IS_HEXAGON then
        self.pos_data = hexagon_pos_data_static()
    else
        self.pos_data = default_pos_data_static()-- core_data == nil and default_pos_data or default_pos_data_static()
        -- create_pos_data(core_data.F2CDis, core_data.B2FDis, core_data.SideDis)
    end

    self.cam_tran = data_holder.CameraTran
    self.original_pos = self.cam_tran.localPosition
    self.player_side_pos = nil
    self.enemy_side_pos = nil

    self.whole_trans = {}
    self.row_trans = {}
    self.camp_trans = {}
    self.cam_type = 0
    BattleData.Instance:BaseData().cam_type = 0
    self.forward1 = Vector3.New(1.0000001192093,0,-1.1920928955078e-07)
    self.forward2 = Vector3.New(-1.0000001192093,-0,1.1920928955078e-07)
end

function BattleSceneData:__delete()
    self.center_pos_tran = nil
    self.cam_tran = nil
end

function BattleSceneData:GetCenterPosTran()
    return self.center_pos_tran
end

function BattleSceneData:GetPos(index, offset)
    local local_p
    local is_enemy = BattleData.IsEnemyIdx(index)
    local i = index - 1
    if BattleData.Instance:IsPVP() and not BattleData.Instance:IsOffensive() then
        i = BattleData.GetSwitchedIdx(index) - 1
    elseif BattleData.Instance:OBSide() == 1 then--[[ BattleData.Instance:IsOB() > 0 and ]]
        i = BattleData.GetSwitchedIdx(index) - 1
    end
    --对余数为0 1 3 进行x的1.8偏移
    -- 0 2 4
    -- 10 11 12 13 14
    -- 15 16 17 18 19

    -- 5 6 7 8 9
    -- 0 1 2 3 4 
    if is_enemy then
        local_p = Vector3.New(((i % 5) - 2) * self.pos_data.side, 0, self.pos_data.f2c + math.floor((19 - i) / 5) * self.pos_data.b2f)
        if offset ~= nil then
            offset.z = -offset.z
            offset.x = -offset.x
        end
    else
        local_p = Vector3.New(((i % 5) - 2) * self.pos_data.side, 0, -self.pos_data.f2c - math.floor((9 - i) / 5) * self.pos_data.b2f)
    end
    if offset ~= nil then
        local_p = local_p + offset
    end
    if IS_HEXAGON then
        local p = i % 5
        if not (p == 0 or p == 2 or p == 4) then
            offset = Vector3.New(0, 0, 1.683)
            if is_enemy then
                offset.z = -offset.z
                offset.x = -offset.x
            end
        end
        if offset ~= nil then
            local_p = local_p + offset
        end
    end
    return self.center_pos_tran:TransformPoint(local_p)
end

function BattleSceneData:GetRowPosForward(index, offset)
    local pos_i
    if index <= 5 then
        pos_i = 3
    elseif index <= 10 then
        pos_i = 8
    elseif index <= 15 then
        pos_i = 13
    else
        pos_i = 18
    end
    return self:GetPos(pos_i, offset), self:GetForward(pos_i)
end
--获取队伍位置和方向
function BattleSceneData:GetCampPosForward(index, offset)
    local re_pos, re_forward
    if index <= 10 then
        re_pos = (self:GetPos(3, offset) + self:GetPos(8, offset)) / 2
        re_forward = self:GetForward(3)
    else
        re_pos = (self:GetPos(13, offset) + self:GetPos(18, offset)) / 2
        re_forward = self:GetForward(13)
    end
    return re_pos, re_forward
end

function BattleSceneData:GetWholePosForward(index)
    if index == nil then
        index = 13
    end
    return self.center_pos_tran.position, self:GetForward(index)
end

function BattleSceneData:GetRowSidePoint(index, offset)
    --	index = index - 1
    local row_idx = math.floor((index - 1) / 5) + 1
    -- if row_idx == 3 then
    --  	row_idx = 4
    -- elseif row_idx == 4 then
    -- 	row_idx = 3
    -- end
    local up_idx = row_idx * 5
    local down_idx = up_idx - 4
    if BattleData.IsEnemyIdx(index) then
        return self:GetPos(up_idx, Vector3.New(-offset, 0, 0)), self:GetPos(down_idx, Vector3.New(offset, 0, 0))
    else
        return self:GetPos(up_idx, Vector3.New(offset, 0, 0)), self:GetPos(down_idx, Vector3.New(-offset, 0, 0))
    end
end

function BattleSceneData:GetForward(index, euler_off)
    local re_val
    if euler_off == nil or euler_off == Vector3.zero then
        if BattleData.IsEnemyIdx(index) then
            -- LogError(-self.center_pos_tran.forward)
            return self.forward2
        else
            -- LogError(self.center_pos_tran.forward)
            return self.forward1
        end
    else
        local rot_bac = self.center_pos_tran.rotation
        if BattleData.IsEnemyIdx(index) then
            self.center_pos_tran.forward = -self.center_pos_tran.forward
            self.center_pos_tran:Rotate(euler_off)
            re_val = self.center_pos_tran.forward
        else
            self.center_pos_tran:Rotate(euler_off)
            re_val = self.center_pos_tran.forward
        end
        self.center_pos_tran.rotation = rot_bac
    end
    -- LogError(re_val)
    return re_val
end

if BattleSceneData.WholeTrans == nil then
    BattleSceneData.WholeTrans = {
        [1] = UnityEngine.GameObject("BattleWhole1"),
        [2] = UnityEngine.GameObject("BattleWhole2")
    }
--if Game.ResidentObjectManager ~= nil then
end

if BattleSceneData.CampTrans == nil then
    BattleSceneData.CampTrans = {
        [1] = UnityEngine.GameObject("BattleCamp1"),
        [2] = UnityEngine.GameObject("BattleCamp2")
    }
end

if BattleSceneData.RowTrans == nil then
    BattleSceneData.RowTrans = {
        [1] = UnityEngine.GameObject("BattleRow1"),
        [2] = UnityEngine.GameObject("BattleRow2"),
        [3] = UnityEngine.GameObject("BattleRow3"),
        [4] = UnityEngine.GameObject("BattleRow4")
    }
end
--被击队伍中心
function BattleSceneData:GetCampTransform(index)
    local tran_index = BattleData.IsEnemyIdx(index) and 2 or 1
    local re = self.camp_trans[tran_index]
    if re == nil then
        re = BattleSceneData.CampTrans[tran_index].transform
        local pos, forward = self:GetCampPosForward(index)
        re.position = pos
        re.forward = forward
        self.camp_trans[tran_index] = re
    end
    return re
end

function BattleSceneData:GetWholeTransform(index)
    local tran_index = BattleData.IsEnemyIdx(index) and 2 or 1
    local re = self.whole_trans[tran_index]
    if re == nil then
        re = BattleSceneData.WholeTrans[tran_index].transform
        local pos, forward = self:GetWholePosForward(index)
        re.position = pos
        re.forward = forward
        self.whole_trans[tran_index] = re
    end
    return re
end

function BattleSceneData:GetRowTransform(index)
    local tran_index = math.floor((index - 1) / 5) + 1
    local re = self.row_trans[tran_index]
    if re == nil then
        re = BattleSceneData.RowTrans[tran_index].transform
        local pos, forward = self:GetRowPosForward(index)
        re.position = pos
        re.forward = forward
        self.row_trans[tran_index] = re
    end
    return re
end

function BattleSceneData:playerSidePos()
    if self.player_side_pos == nil then
        local ray_p = Vector3.New(0, 0, -self.pos_data.f2c - self.pos_data.b2f / 2)
        ray_p = self.center_pos_tran:TransformPoint(ray_p)
        local ray_dir = -self.cam_tran.forward
        self.player_side_pos = ray_p + ray_dir * (25 / ray_dir:Magnitude())
    end
    return self.player_side_pos
end

function BattleSceneData:enemySidePos()
    if self.enemy_side_pos == nil then
        local ray_p = Vector3.New(0, 0, self.pos_data.f2c + self.pos_data.b2f / 2)
        ray_p = self.center_pos_tran:TransformPoint(ray_p)
        local ray_dir = -self.cam_tran.forward
        self.enemy_side_pos = ray_p + ray_dir * (30 / ray_dir:Magnitude())
    end
    return self.enemy_side_pos
end

--0:original pos 1:playerside 2:enemy side
function BattleSceneData:CameraSwitch(cam_type)
    SceneCtrl.Instance:CurScene():RecoverCamera()
    SceneCtrl.Instance:CurScene():LockCamera()
    self.cam_type = cam_type
    BattleData.Instance:BaseData().cam_type = cam_type
    if cam_type == 1 then
        self.cam_tran.position = self:playerSidePos()
    elseif cam_type == 2 then
        self.cam_tran.position = self:enemySidePos()
    else
        SceneCtrl.Instance:CurScene():RecallCamera()
        -- self.cam_tran.localPosition = self.original_pos
    end
end

function BattleSceneData:SelCameraChange()
    local cur_pos = self.cam_tran.position
    if cur_pos == self:playerSidePos() then
        self.cam_tran.position = self:enemySidePos()
    elseif cur_pos == self:enemySidePos() then
        self.cam_tran.position = self:playerSidePos()
    end
end

function BattleSceneData:CamType()
    return self.cam_type
end