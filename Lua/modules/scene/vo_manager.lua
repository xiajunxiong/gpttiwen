VoManager = VoManager or BaseClass()
function VoManager:__init()
    if VoMgr ~= nil then
        Debuger.LogError("[VoMgr] attempt to create singleton twice!")
        return
    end
    VoMgr = self
    self.npc_dynamic_cfg = Config.npc_dynamic_auto.npc_dynamic_cfg
    self.npc_cache_id = {}
    self:init_config()

    -- 现在是服务端发送的对象都用obj_id存起来了
    --net
    self.role_vo_list = SmartData.New()
    self.monster_vo_list = SmartData.New()
    self.door_vo_list = SmartData.New()
    self.snpc_vo_list = SmartData.New()
    self.snpc_pos_list = SmartData.New()
    -- temp
    self.snpc_vo_list2 = {}
    -- npc
    self.npc_vo_list = {}
    --组队时需要看到相同的NPC
    self.npc_vo_list_leader = {}
    self.leader_npc_flush = SmartData.New({val = false})
    -- block屏蔽
    self.block_npc_list = {}
    --由客户端创建的对象、服务端只给出是否创建的对象(通用创建)
    self.generic_creation_list = {}
end
function VoManager:__delete()
    VoMgr = nil
end
function VoManager:init_config()
    for _, v in pairs(self.npc_dynamic_cfg) do
        if self.npc_cache_id[v.id] == nil then
            self.npc_cache_id[v.id] = {}
        end
        table.insert(self.npc_cache_id[v.id], v)
    end
end
function VoManager:Clear()
    for k, _ in pairs(self.role_vo_list) do
        self:DelRole(k)
    end
    -- self.role_vo_list:Set({})
    for k, _ in pairs(self.monster_vo_list) do
        self:DelMonster(k)
    end
    -- self.monster_vo_list:Set({})
    for k, _ in pairs(self.door_vo_list) do
        self:DelDoor(k)
    end
    -- self.door_vo_list:Set({})
    for k, _ in pairs(self.door_vo_list) do
        self:DelSNpc(k)
    end
    for k, _ in pairs(self.snpc_vo_list) do
        self:DelSNpc(k)
    end
    -- self.snpc_vo_list:Set({})
    self.block_npc_list = {}
end
function VoManager:initScene(scene_id)
    --[[ if MainViewData.Instance:BaseData().is_battle == true then
		return
    end ]]
    local sc_cfg = SceneData.Instance:GetSceneCfg(scene_id)
    for _, value in ipairs(sc_cfg.npcs) do
        if value.show_condition == 0 then
            if self.npc_vo_list[value.seq] == nil then
                self.npc_vo_list[value.seq] = value
                self.npc_vo_list[value.seq].count = 1
                self.npc_vo_list[value.seq].scene_id = sc_cfg.id
            end
        end
    end
end
function VoManager:disposeScene(scene_id)
    --[[ if MainViewData.Instance:BaseData().is_battle == true then
		return
    end ]]
    if scene_id > 0 then
        local sc_cfg = SceneData.Instance:GetSceneCfg(scene_id)
        if sc_cfg.npcs then
            for _, vo in ipairs(sc_cfg.npcs) do
                if vo.show_condition == 0 then
                    self.npc_vo_list[vo.seq] = nil
                end
            end
        end
        -- 删除服务端没删除的角色
        for _, vo in pairs(self.role_vo_list) do
            if vo.scene_id == scene_id then
                self.role_vo_list[vo.obj_id] = nil
            end
        end
        for _, vo in pairs(self.monster_vo_list) do
            if vo.scene_id == scene_id then
                self.monster_vo_list[vo.obj_id] = nil
            end
        end
        for _, vo in pairs(self.snpc_vo_list) do
            if vo.scene_id == scene_id then
                self.snpc_vo_list[vo.obj_id] = nil
            end
        end
        for _, vo in pairs(self.snpc_pos_list) do
            self.snpc_pos_list[vo.obj_id] = nil
        end
        for _, vo in pairs(self.door_vo_list) do
            self.door_vo_list[vo.obj_id] = nil
        end
    end
    MazeRunData.Instance:ClearRuneCurseInfo()   --切场景时清理迷宫竞速里的buff数据
end
function VoManager:NpcVoBySeq(seq)
    if self.npc_dynamic_cfg[seq] == nil then
        -- Debuger.LogErrorSD("seq 【%s】 is nil check then npc dynamic config", seq)
            if self.snpc_vo_list2[seq] then
                return self.snpc_vo_list2[seq]
            end
        return
    end
    return self.npc_dynamic_cfg[seq]
end

function VoManager:NpcVoByNpcId(npc_id)
    if self.npc_cache_id[npc_id] == nil or next(self.npc_cache_id[npc_id]) == nil then
        -- Debuger.LogError("no npc vo with this npc_id 【%s】 in npc dynamic config", npc_id)
        return {}
    end
    return self.npc_cache_id[npc_id]
end

function VoManager:NpcTargetByNpcId(npc_id)
    local npc_config = VoMgr:NpcVoByNpcId(npc_id)
    if npc_config ~= nil and npc_config[1] then
        return npc_config[1].scene_id or 0
    end
end

----------角色添加--------------
function VoManager:AddRole(vo)
    self.role_vo_list[vo.obj_id] = SmartData.New(vo,false)
end
function VoManager:DelRole(obj_id)
    if self.role_vo_list[obj_id] then
       self.role_vo_list[obj_id] = nil
    end
end

function VoManager:HasRole(obj_id)
    return self.role_vo_list[obj_id] ~= nil
end

function VoManager:ChangeRoleStatus(obj_id, status)
    local vo = self.role_vo_list[obj_id]
    if vo then
        --[[ if vo.behavior_state == ROLE_STATUS.FIGHT and status ~=ROLE_STATUS.FIGHT then
            vo.time = Time.time
        end ]]
        vo.behavior_state = status
    end
end

function VoManager:ChangeRoleMoveStatus(obj_id,status)
    local vo = self.role_vo_list[obj_id]
    if vo then
        vo.move_state = status
    end
end

function VoManager:ChangeRoleIsLeader(obj_id, is_leader)
    local vo = self.role_vo_list[obj_id]
    if vo then
        vo.is_leader = is_leader
    end
end

function VoManager:ChangeRoleEscortId(obj_id, escort_id)
    local vo = self.role_vo_list[obj_id]
    if vo then
        vo.escort_id = escort_id
    end
end

function VoManager:ChangeRoleKillValue(obj_id, kill_value)
    local vo = self.role_vo_list[obj_id]
    if vo then
        vo.kill_value = kill_value
    end
end

function VoManager:ChangeRoleDaWanKaLevel(obj_id, level)
    local vo = self.role_vo_list[obj_id]
    if vo then
        vo.da_wan_ka_level = level
    end
end

---------明怪添加----------------
function VoManager:AddMonster(vo)
    vo.is_server = true
    if self.monster_vo_list[vo.obj_id] == nil then
        self.monster_vo_list[vo.obj_id] = vo
    end
end
function VoManager:DelMonster(obj_id)
    if self.monster_vo_list[obj_id] then
        self.monster_vo_list[obj_id] = nil
    end
end
---------门添加-----------------
function VoManager:AddDoor(vo)
    vo.is_server = true
    if self.door_vo_list[vo.obj_id] == nil then
        self.door_vo_list[vo.obj_id] = vo
    end
end

function VoManager:DelDoor(obj_id)
    if self.door_vo_list[obj_id] then
        self.door_vo_list[obj_id] = nil
    end
end
---------服务端对象（怪和门除外）--------------
function VoManager:AddSNpc(vo)
    vo.is_server = true
    if self.snpc_vo_list[vo.obj_id] == nil then
        self.snpc_vo_list[vo.obj_id] = vo
        self.snpc_vo_list2[vo.seq or -1] = vo
    end
end

function VoManager:DelSNpc(obj_id)
    if self.snpc_vo_list[obj_id] then
        self.snpc_vo_list[obj_id] = nil
    end
end

function VoManager:OpenBox(obj_id)
    if self.snpc_vo_list[obj_id] then
        self.snpc_vo_list[obj_id].is_open = 1
    end
end

---------服务端当前场景所有对象位置信息--------------
function VoManager:AddSNpcPos(vo)
    vo.is_server = true
    if self.snpc_pos_list[vo.obj_id] == nil then
        self.snpc_pos_list[vo.obj_id] = vo
    end
end

function VoManager:DelSNpcPos(obj_id)
    if self.snpc_pos_list[obj_id] then
        self.snpc_pos_list[obj_id] = nil
    end
end
--------通用创建的对象----------------
function VoManager:AddGCObj(vo)
    if self.generic_creation_list[vo.obj_id] == nil then
        self.generic_creation_list[vo.obj_id] = vo
    end
end
function VoManager:DelGCObj(obj_id, delete_now)
    if self.generic_creation_list[obj_id] then
        self.generic_creation_list[obj_id] = nil
    end
    if delete_now == true then
        SceneCtrl.Instance:Objs():DeleteClientObj(obj_id)
    end
end
---------屏蔽客户端静态对象----------------
function VoManager:BlockNpc(npc_seq)
    self.block_npc_list[npc_seq] = 1
end
function VoManager:UnBlockNpc(npc_seq)
    self.block_npc_list[npc_seq] = nil
end
function VoManager:NpcVoList()
    return self.npc_vo_list
end
function VoManager:NpcVoListLeader()
    return self.npc_vo_list_leader
end
function VoManager:RoleVoList()
    return self.role_vo_list
end
function VoManager:MonsterVoList()
    return self.monster_vo_list
end
function VoManager:DoorVoList()
    return self.door_vo_list
end
function VoManager:SNpcVoList()
    return self.snpc_vo_list
end
function VoManager:BlockNpcList()
    return self.block_npc_list
end
function VoManager:SnpcPosList()
    return self.snpc_pos_list
end
function VoManager:LeaderNpcFlush()
    return self.leader_npc_flush
end
function VoManager:GCObjList()
    return self.generic_creation_list
end
-- 客户端NPC和客户端动态NPC 
-- is_not_notice 是否不通知其他队员 默认通知其他队员
function VoManager:AddNpc(vo,is_not_notice)
    local result = vo
	if type(vo) == "number" then
	    result = self:NpcVoBySeq(vo)
    end
    local function doAdd(vo)
        if type(vo) ~= "table" then
            LogDG(vo)
            return
        end
        if self.npc_vo_list[vo.seq] == nil then
            self.npc_vo_list[vo.seq] = vo
            self.npc_vo_list[vo.seq].count = 1
        else
            self.npc_vo_list[vo.seq].count = self.npc_vo_list[vo.seq].count + 1
        end
        self.npc_vo_list[vo.seq].is_not_notice = is_not_notice
        if is_not_notice == nil then
            TeamCtrl.Instance:SendTeamLeaderNpcReq(TeamConfig.TeamLeaderNpcOperType.add, {vo})
        end
    end
    if result == nil then
        result = self:NpcVoByNpcId(vo)
        for _, v in pairs(result) do
            doAdd(v)
        end
        return
    end
    vo = result
    doAdd(vo)
end

function VoManager:IsHasNpc(id)
    return self.npc_vo_list[id] ~= nil
end

-- is_not_notice 是否不通知其他队员 默认通知其他队员
function VoManager:DeleteNpc(vo,is_not_notice)
    local result = vo
    if type(vo) == "number" then
	    result = self:NpcVoBySeq(vo)
    end
    local function doDelete(vo)
        if type(vo) ~= "table" then
            LogDG(vo)
            return
        end
        if self.npc_vo_list[vo.seq] and self.npc_vo_list[vo.seq].show_condition == 1 then
            self.npc_vo_list[vo.seq].count = self.npc_vo_list[vo.seq].count - 1
            if self.npc_vo_list[vo.seq].count >= 0 then 
                if is_not_notice == nil and RoleData.Instance:GetRoleLevel() > 0 then
                    TeamCtrl.Instance:SendTeamLeaderNpcReq(TeamConfig.TeamLeaderNpcOperType.delete, {vo}) 
                end
            end
            if self.npc_vo_list[vo.seq].count <= 0 then
                self.npc_vo_list[vo.seq].count = 0
            end
        end
    end
    if result == nil then
        result = self:NpcVoByNpcId(vo)
        for _, v in pairs(result) do
            doDelete(v)
        end
        return
    end
    vo = result
    doDelete(vo)
end

-- 下线时需要清掉该角色添加的客户端动态npc
function VoManager:ClearNpc()
    for seq, vo in pairs(self.npc_vo_list) do
        vo.count = 1
        self:DeleteNpc(seq)
    end
end

function VoManager:AddNpcLeader(seq)
    local vo = self:NpcVoBySeq(seq)
    if vo == nil then
        return
    end
    if self.npc_vo_list_leader[vo.seq] == nil then
        self.npc_vo_list_leader[vo.seq] = TableCopy(vo)
        self.npc_vo_list_leader[vo.seq].count = 1
    else
        self.npc_vo_list_leader[vo.seq].count = self.npc_vo_list_leader[vo.seq].count + 1
    end
    self.leader_npc_flush.val = not self.leader_npc_flush.val
end

function VoManager:DeleteNpcLeader(seq)
    local vo = self:NpcVoBySeq(seq)
    if self.npc_vo_list_leader[vo.seq] and self.npc_vo_list_leader[vo.seq].show_condition == 1 then
        self.npc_vo_list_leader[vo.seq].count = self.npc_vo_list_leader[vo.seq].count > 0 and self.npc_vo_list_leader[vo.seq].count - 1 or 0
        self.leader_npc_flush.val = not self.leader_npc_flush.val
    end
end

function VoManager:ClearNpcLeader()
    for seq, vo in pairs(self.npc_vo_list_leader) do
        self:DeleteNpcLeader(seq)
    end
end

