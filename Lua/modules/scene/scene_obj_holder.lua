SceneObjHolder = SceneObjHolder or BaseClass(BaseCtrl)

function SceneObjHolder:__init()
    
    self.client_objs = {} --key:local obj id,val:scene_obj
    self.client_npcs = {}
    self.client_doors = {}

    self.vo_objs = {} --key:vo(非sm_data) val:scene_obj	--主要用于添加和删除
    self.objs = {} --key:net obj id,val:scene_obj

    self.net_roles = {}
    self.net_monsters = {}
    self.net_doors = {}
    self.net_npcs = {} --除人、怪和门之外的服务端对象

    self.npc_block_list = {}
    self.repeat_timers = {}
    TimeHelper:AddRunTimer(BindTool.Bind(self.IntervalRepeat, self), 1.5)
    self.role_vo_care = VoMgr:RoleVoList():Care(BindTool.Bind(self.OnSceneRoleChange, self))
    self.monster_vo_care = VoMgr:MonsterVoList():Care(BindTool.Bind(self.OnSceneMonsterChange, self))
    self.door_vo_care = VoMgr:DoorVoList():Care(BindTool.Bind(self.OnSceneDoorChange, self))
    self.snpc_vo_care = VoMgr:SNpcVoList():Care(BindTool.Bind(self.OnSceneSNpcChange, self))
    self.leader_npc_vo_care = VoMgr:LeaderNpcFlush():Care(BindTool.Bind(self.OnSceneLeaderNpcChange, self))
    self.role_setting_change_handle = EventSys:Bind(GlobalEventKeys.RoleSettingChange, BindTool.Bind(self.RoleSettingChange, self))
end

function SceneObjHolder:__delete()
    if self.role_setting_change_handle then
        EventSys:UnBind(self.role_setting_change_handle)
    end
    if self.role_vo_care then
        VoMgr:RoleVoList():Uncare(self.role_vo_care)
        self.role_vo_care = nil
    end
    if self.monster_vo_care then
        VoMgr:MonsterVoList():Uncare(self.monster_vo_care)
        self.monster_vo_care = nil
    end
    if self.door_vo_care then
        VoMgr:DoorVoList():Uncare(self.door_vo_care)
        self.door_vo_care = nil
    end
    if self.snpc_vo_care then
        VoMgr:SNpcVoList():Uncare(self.snpc_vo_care)
        self.snpc_vo_care = nil
    end
    if self.leader_npc_vo_care then
        VoMgr:LeaderNpcFlush():Uncare(self.leader_npc_vo_care)
        self.leader_npc_vo_care = nil
    end
    self.update_timer = nil
end

function SceneObjHolder:GetObjectByObjId(obj_id)
    if MainPlayer and MainPlayer:ObjId() == obj_id then
        return MainPlayer
    end
    local re_val = self.objs[obj_id]
    if re_val ~= nil and re_val:Vo().scene_id ~= SceneData.Instance:SceneId() then --非当前场景的角色，不返回给逻辑层。逻辑层不应该用到且可能马上就会被销毁
        return nil
    end
    if re_val == nil then
        re_val = self.client_objs[obj_id]
        if re_val ~= nil and re_val:Vo().scene_id ~= SceneData.Instance:SceneId() then --非当前场景的角色，不返回给逻辑层。逻辑层不应该用到且可能马上就会被销毁
            return nil
        end
    end
    return re_val
end

--通过vo获取网络npc
function SceneObjHolder:GetNetNpc(vo)
    local vo = SmartData.GetVal(vo)
    return self.net_npcs[vo]
end

function SceneObjHolder.CalcClientObjId(scene_id, obj_type, cfg_index)
    return scene_id * 100000 + obj_type * 100000 + (cfg_index or 0)
end

function SceneObjHolder:DeleteClientObj(obj_id)
    local obj = self.client_objs[obj_id]
    if obj ~= nil then
        if obj:Type() == SceneObjType.Npc then
            self.client_npcs[obj_id] = nil
        elseif obj:Type() == SceneObjType.Door then
            self.client_doors[obj_id] = nil
        end
        obj:DeleteMe()
        self.client_objs[obj_id] = nil
    end
end

function SceneObjHolder:DeleteNetObj(vo)
    if SmartData.Check(vo) then
        vo = vo:Val()
    end
    local obj = self.vo_objs[vo]
    if obj == nil then
        return
    end
    local obj_id = vo.obj_id
    local id_get_obj = self.objs[obj_id]
    if id_get_obj ~= nil and SmartData.GetVal(id_get_obj:Vo()) == vo then
        self.objs[obj_id] = nil
    end

    if obj:Type() == SceneObjType.Role then
        self.net_roles[vo] = nil
    elseif obj:Type() == SceneObjType.Monster then
        self.net_monsters[vo] = nil
    elseif obj:Type() == SceneObjType.Door then
        self.net_doors[vo] = nil
    -- elseif obj:Type() == SceneObjType.Box or obj:Type() == SceneObjType.Npc then
    elseif SceneConfig.NetNpcsType[obj:Type()] then
        self.net_npcs[vo] = nil
    end
    obj:DeleteMe()
end

function SceneObjHolder:CreateClientObj(obj_type, vo)
    local obj_id = vo.obj_id or 0
    if self.client_objs[obj_id] ~= nil then
        -- LogDG("SceneObjHolder:CreateClientObj ERROR,client_obj is EXIST.ID=%d", obj_id)
        return self.client_objs[obj_id]
    end
    vo.obj_type = obj_type
    local obj = SceneObj.CreateObj(vo)
    if obj_type == SceneObjType.Npc then
        self.client_npcs[obj_id] = obj
    elseif obj_type == SceneObjType.Door then
        self.client_doors[obj_id] = obj
    end
    if obj ~= nil then
        self.client_objs[obj_id] = obj
    end
    return obj
end

function SceneObjHolder:CreateNetObj(obj_type, vo)
    local key_vo = SmartData.GetVal(vo)
    if self.vo_objs[key_vo] ~= nil then
        Debuger.LogErrorSD("SceneObjHolder:CreateNetObj ERROR,objs is EXIST.vo=" .. tostring(key_vo))
        return
    end
    local obj_id = vo.obj_id or 0

    vo.obj_type = obj_type
    local obj = SceneObj.CreateObj(vo)
    if obj == nil then
        return
    end
    if obj_type == SceneObjType.Role then
        self.net_roles[key_vo] = obj
    elseif obj_type == SceneObjType.Monster then
        self.net_monsters[key_vo] = obj
    elseif obj_type == SceneObjType.Door then
        self.net_doors[key_vo] = obj
    elseif SceneConfig.NetNpcsType[obj_type] then
        self.net_npcs[key_vo] = obj
    end
    self.objs[obj_id] = obj
    self.vo_objs[key_vo] = obj
    return obj
end
function SceneObjHolder:RoleSettingChange(key)
    if key == ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER
    or key == ROLE_SETTING_TYPE.GUILD_ANSWER_SHOW_ROLE_NUM then
        self:CheckForVisbleRoleSetting()
    end
end
function SceneObjHolder:CheckForVisbleRoleSetting()
    if not MainPlayer then
        return
    end
    --在显示其他人的前提下进行
    if RoleSettingData.IsShield(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER) then
        return
    end
    local x = MainPlayer:GetPos().x
    local y = MainPlayer:GetPos().y
    local guild_name = RoleData.Instance:GetGuildName()

    local sorted_roles = {}
    self:ForeachRole(
        function(obj)
            table.insert(sorted_roles, obj)
        end
    )
    table.sort(
        sorted_roles,
        function(a, b)
            local score_a = a.guild_name == guild_name and 1000000 or 0
            local pos_a = a:GetPos()
            score_a = score_a - (pos_a.x - x) * (pos_a.x - x) - (pos_a.y - y) * (pos_a.y - y)
            local score_b = b.guild_name == guild_name and 1000000 or 0
            local pos_b = a:GetPos()
            score_b = score_b - (pos_b.x - x) * (pos_b.x - x) - (pos_b.y - y) * (pos_b.y - y)
            return score_a > score_b
        end
    )
    
    local num = 0
    local limit = RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS)
    if GuildData.Instance:IsInAnswerScene() then
        limit = RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.GUILD_ANSWER_SHOW_ROLE_NUM)
    end
    for i, obj in ipairs(sorted_roles) do
        if TombRaiderData.Instance:InTombRaider() then 
            break
        end 
        if not TeamData.Instance:MemberInTeam(obj.vo.name) then
            num = num + 1
            obj:SetVisible(num <= limit)
        else
            obj:SetVisible(true)
        end
    end
end

function SceneObjHolder:OnSceneRoleChange(old)
    local role_vo_list = VoMgr:RoleVoList()
    -- if old then
    for k, v in pairs(old) do
        if v ~= SmartData.NotiNil and (role_vo_list[k] == nil or role_vo_list[k]:Val() ~= v) then --可能是vo内的字段发生了改变，这种通知不用把角色删除
            self:DeleteNetObj(v)
        end
    end
    -- end

    local sc_cfg = SceneData.Instance:SceneConfig()
    if sc_cfg == nil then
        return
    end

    local scene_id = sc_cfg.id
    for _, v in pairs(role_vo_list) do
        if v.scene_id == scene_id then
            if nil == self.net_roles[SmartData.GetVal(v)] then
                local obj = self:CreateNetObj(SceneObjType.Role, v)
                if obj then
                    obj:SetScenePos(scene_id, v.x, v.y, true)
                    obj:SetDirByEulerY(v.dir_y)
                    obj:SetMoveSpeed(v.move_speed)
                end
            end
        else
            self:DeleteNetObj(v)
        end
    end
    self:CheckForVisbleRoleSetting()
    --[[ if GuildData.Instance:IsInAnswerScene2() then
        self:SetTouchAbleAboutAnswer()
    end ]]
    if OnlyFightData.Instance:IsInOnlyFight(true) then
        self:ShieldingPet(true)
    end
end
function SceneObjHolder:OnSceneMonsterChange(old)
    local monster_vo_list = VoMgr:MonsterVoList()
    -- if old then
    for k, v in pairs(old) do
        if v ~= SmartData.NotiNil and (monster_vo_list[k] == nil or monster_vo_list[k] ~= v) then --可能是vo内的字段发生了改变，这种通知不用把角色删除
            self:DeleteNetObj(v)
        end
    end
    -- end

    local sc_cfg = SceneData.Instance:SceneConfig()
    if sc_cfg == nil then
        return
    end
    local scene_id = sc_cfg.id
    for k, v in pairs(monster_vo_list) do
        if v.scene_id == scene_id then
            if nil == self.net_monsters[v] then
                local obj = self:CreateNetObj(SceneObjType.Monster, v)
                if obj then
                    obj:SetScenePos(scene_id, v.x, v.y, true)
                    obj:SetDirByEulerY(v.dir_y)
                    obj:SetMoveSpeed(v.move_speed)
                end
            end
        else
            self:DeleteNetObj(v)
        end
    end
end

function SceneObjHolder:OnSceneDoorChange(old)
    local door_vo_list = VoMgr:DoorVoList()
    -- if old then
    for k, v in pairs(old) do
        if v ~= SmartData.NotiNil and (door_vo_list[k] == nil or door_vo_list[k] ~= v) then --可能是vo内的字段发生了改变，这种通知不用把角色删除
            self:DeleteNetObj(v)
        end
    end
    -- end
    local sc_cfg = SceneData.Instance:SceneConfig()
    if sc_cfg == nil then
        return
    end
    local scene_id = sc_cfg.id
    for k, v in pairs(door_vo_list) do
        if v.scene_id == scene_id then
            if nil == self.net_doors[v] then
                local obj = self:CreateNetObj(SceneObjType.Door, v)
                if obj then
                    obj:SetScenePos(scene_id, v.x, v.y, true)
                    obj:SetDirByEulerY(v.dir_y)
                    obj:SetMoveSpeed(v.move_speed)
                    obj.enter_handle = BindTool.Bind(SceneCtrl.Instance.SendSpecialTransportReq, SceneCtrl.Instance, v.obj_id)
                end
            end
        else
            self:DeleteNetObj(v)
        end
    end
end
function SceneObjHolder:OnSceneSNpcChange(old)
    local snpc_vo_list = VoMgr:SNpcVoList()
    -- if old then
    for k, v in pairs(old) do
        if v ~= SmartData.NotiNil and (snpc_vo_list[k] == nil or snpc_vo_list[k] ~= v) then --可能是vo内的字段发生了改变，这种通知不用把角色删除
            self:DeleteNetObj(v)
        end
    end
    -- end
    local sc_cfg = SceneData.Instance:SceneConfig()
    if sc_cfg == nil then
        return
    end
    local scene_id = sc_cfg.id
    for _, v in pairs(snpc_vo_list) do
        if v.scene_id == scene_id then
            if nil == self.net_npcs[v] then
                local obj = self:CreateNetObj(v.obj_type, v)
                if obj then
                    obj:SetScenePos(scene_id, v.x, v.y, true)
                end
            end
        else
            self:DeleteNetObj(v)
        end
    end
end

function SceneObjHolder:OnSceneLeaderNpcChange(old)
    local sc_cfg = SceneData.Instance:SceneConfig()
    if sc_cfg == nil or MainViewData.Instance:BaseData().is_battle == true then
        return
    end
    local npc_list = VoMgr:NpcVoList()
    local scene_id = sc_cfg.id
    local npc_list_leader = VoMgr:NpcVoListLeader()
    for k, v in pairs(npc_list_leader) do
        local obj_id = SceneObjHolder.CalcClientObjId(scene_id, SceneObjType.Npc, k)
        if self:CheckNpcCanCreate(v, scene_id) and nil == npc_list[k] then
            if MainPlayer and self:IsInitView(MainPlayer:GetPos(), v) then
                if nil == self.client_objs[obj_id] then
                    v.obj_id = obj_id
                    local npc = self:CreateClientObj(SceneObjType.Npc, v)
                    npc:SetDirByEulerY(v.dir_y)
                    npc:SetScenePos(scene_id, v.x, v.y, false)
                end
            else
                self:DeleteClientObj(obj_id)
            end
        elseif nil == npc_list[k] then
            self:DeleteClientObj(obj_id)
        end
    end
end

function SceneObjHolder:IntervalRepeat()
    local sc_cfg = SceneData.Instance:SceneConfig()
    if sc_cfg == nil or MainViewData.Instance:BaseData().is_battle == true then
        return
    end
    local scene_id = sc_cfg.id
    local npc_list = VoMgr:NpcVoList()
    local obj_id = 0
    for k, v in pairs(npc_list) do
        obj_id = SceneObjHolder.CalcClientObjId(scene_id, SceneObjType.Npc, k)
        if self:CheckNpcCanCreate(v, scene_id) then
            if MainPlayer and self:IsInitView(MainPlayer:GetPos(), v) then
                if nil == self.client_objs[obj_id] then
                    v.obj_id = obj_id
                    local npc = self:CreateClientObj(SceneObjType.Npc, v)
                    npc:SetDirByEulerY(v.dir_y)
                    npc:SetScenePos(scene_id, v.x, v.y, false)
                    self.repeat_timers[obj_id] = {timer = 0, is_first = false}
                end
            else
                self:DeleteClientObj(obj_id)
                self.repeat_timers[obj_id] = nil
            end
        else
            self:DeleteClientObj(obj_id)
            self.repeat_timers[obj_id] = nil
        end
    end
    local gc_list = VoMgr:GCObjList()
    for obj_id, v in pairs(gc_list) do
        if self:CheckNpcCanCreate(v, scene_id, true) then
            if MainPlayer and self:IsInitView(MainPlayer:GetPos(), v) then
                if nil == self.client_objs[obj_id] then
                    -- v.obj_id = obj_id
                    local obj = self:CreateClientObj(v.obj_type, v)
                    if obj then
                        obj:SetDirByEulerY(v.dir_y)
                        obj:SetScenePos(scene_id, v.x, v.y, v.is_server)
                    else
                        LogError("obj_id",obj_id,v)
                    end
                end
            else
                self:DeleteClientObj(obj_id)
            end
        else
            self:DeleteClientObj(obj_id)
        end
    end
    -- 计时器，每过15s~25s播放一次在场的npc的show动画
    -- for k,v in pairs(self.repeat_timers)do 
    --     self.repeat_timers[k] = self.repeat_timers[k] + 1
    -- end 
    -- local mark_pos = 0
    local index_pos = 0
    for k,v in pairs(self.repeat_timers) do 
        v.timer = v.timer + 1
        local rand_ = v.is_first and GameMath.Ramdon(25,40) or GameMath.Ramdon(1,4) + index_pos

        if self.client_objs[k] then 
            if v.timer > rand_ then 
                -- local timer = v.timer
                v.timer = 0

                if (self.client_objs[k].IsInAnim and not self.client_objs[k]:IsInAnim(ChAnimType.Show) and 
                    not self.client_objs[k]:IsInAnim(ChAnimType.Show2) and not v.complete) or 
                    not v.is_first then
                        -- LogError("enter!",k,self.client_objs[k].vo.name,timer,rand_)
                        local play_anim
                        if self.client_objs[k]:HasAnim(ChAnimType.Show2) then
                            play_anim = ChAnimType.Show2
                        else
                            play_anim = ChAnimType.Show
                        end
                        v.complete = self.client_objs[k]:PlayAnim(play_anim)
                end 

                v.is_first = true
                break
            else
                if self.client_objs[k].IsInAnim and not self.client_objs[k]:IsInAnim(ChAnimType.Show) and 
                    not self.client_objs[k]:IsInAnim(ChAnimType.Show2) then
                    v.complete = false
                else 
                    v.timer = 0
                end 
            end 
        else 
            self.repeat_timers[k] = nil
        end 

        index_pos = index_pos + 1
    end 
end

function SceneObjHolder:IsInitView(pos1, pos2)
    if pos2.alway_show == true then--忽略是否在视角内，总是显示
        return true
    end
    if pos2.is_server == true then--是否是服务端坐标
        local pos = SceneCtrl.Instance:CurScene():PosToClint(pos2.x, pos2.y)
        local dis_x = pos1.x - pos.x
        local dis_y = pos1.z - pos.z
        return (dis_x * dis_x + dis_y * dis_y) < 400
    end
    local dis_x = pos1.x - pos2.x
    local dis_y = pos1.z - pos2.y
    return (dis_x * dis_x + dis_y * dis_y) < 400
end

function SceneObjHolder:OnGameSceneLoaded()
    local sc_cfg = SceneData.Instance:SceneConfig()
    if sc_cfg == nil or SceneCtrl.Instance:CurScene():GetClassType() ~= GameScene --[[ or MainViewData.Instance:BaseData().is_battle == true ]] then
        return
    end
    local scene_id = SceneData.Instance:SceneId()
    for k, v in pairs(sc_cfg.doors) do
        local obj_id = SceneObjHolder.CalcClientObjId(scene_id, SceneObjType.Door, k)
        if nil == self.client_objs[obj_id] then
            local door_vo = {}
            door_vo.obj_id = obj_id
            door_vo.door_id = v.id
            door_vo.to_scene_id = v.to_scene_id
            door_vo.name = v.name
            door_vo.type = v.type
            local door = self:CreateClientObj(SceneObjType.Door, door_vo)
            door.enter_handle = BindTool.Bind(SceneCtrl.Instance.EnterDoorReq, SceneCtrl.Instance, door_vo.door_id)
            door:SetScenePos(scene_id, v.x, v.y, false, v.z)
        end
    end
    --巡逻每个场景都得检查我也没办法就这样
    StoryCtrl.Instance:CreateSoldier()
    -- AnimalRaceCtrl.Instance:CreateAnimal()
    CatchFoxData.Instance:CreateCatchFox()

    MapExploreData.Instance:CreateBoxMapExplore()
    --这里走奇闻自己的场景逻辑
    AnecdoteLogic.Instance:OnGameSceneLoaded(scene_id)
    --地图探索功能
    MapExploreData.Instance:OnGameSceneLoaded(scene_id)
    --阴阳眼检查
    YinYangData.Instance:OnGameSceneLoaded()
    --摇杆功能
    JoyStickData.Instance:SceneChange()
    --寅虎探春
    TigerSpringTravelCtrl.Instance:OnGameSceneLoaded(scene_id)
end
--独闯贼营巡逻怪 保卫绣球熊孩子 九州秘宝宝箱 喜糖宝箱 狐妖 场景探索 阴阳眼

function SceneObjHolder:ClearScene()
    for _, v in pairs(self.client_objs) do
        v:DeleteMe()
    end
    self.client_npcs = {}
    self.client_doors = {}
    self.client_objs = {}
    MapExploreData.Instance:ClearCacheBoxMapExplore()
    TreasureTaskData.Instance:ClearCacheBoxTreasureTask()
    BountyTaskData.Instance:ClearCacheBountyTask()
end
function SceneObjHolder:Clear()
    for _, v in pairs(self.client_objs) do
        v:DeleteMe()
    end
    self.client_npcs = {}
    self.client_doors = {}
    self.client_objs = {}
    MapExploreData.Instance:ClearCacheBoxMapExplore()
    TreasureTaskData.Instance:ClearCacheBoxTreasureTask()
    BountyTaskData.Instance:ClearCacheBountyTask()
    for _, v in pairs(self.objs) do
        v:DeleteMe()
    end
    self.net_roles = {}
    self.net_monsters = {}
    self.net_doors = {}
    self.net_npcs = {} --除人、怪和门之外的服务端对象
    self.objs = {}
end

--用于点击触发
function SceneObjHolder:ForeachAllObj(func)
    for _, obj in pairs(self.client_objs) do
        local ret = func(obj)
        if ret == true then
            return
        end
    end
    for _, obj in pairs(self.objs) do
        local ret = func(obj)
        if ret == true then
            return
        end
    end
end

function SceneObjHolder:ForeachRole(func)
    for _, obj in pairs(self.net_roles) do
        local ret = func(obj)
        if ret == true then
            return
        end
    end
end

function SceneObjHolder:ForeachDoor(func)
    for _, obj in pairs(self.client_doors) do
        local ret = func(obj)
        if ret == true then
            return
        end
    end
    for _, obj in pairs(self.net_doors) do
        local ret = func(obj)
        if ret == true then
            return
        end
    end
end
-- 查找客户端NPC
function SceneObjHolder:ForeachCNPC(func)
    if self.search_wait then
        Runner.Instance:RemoveRunUntilTrue(self.search_wait)
    end
    self.search_wait =
        Runner.Instance:RunUntilTrue(
        function()
            for _, obj in pairs(self.client_npcs) do
                if func(obj) then
                    return true
                end
            end
            return false
        end
    )
end
-- 查找服务端NPC
function SceneObjHolder:ForeachSNPC(func)
    for _, obj in pairs(self.net_monsters) do
        if func(obj) then
            return
        end
    end
    for _, obj in pairs(self.net_npcs) do
        if func(obj) then
            return
        end
    end
end

--检查NPC是否需要生成
function SceneObjHolder:CheckNpcCanCreate(vo, scene_id, gc)
    local block_list = VoMgr:BlockNpcList()
    for k, _ in pairs(block_list) do
        if vo.seq and vo.seq == k then
            return false
        end
    end
    if vo.count and vo.count <= 0 then
        return false
    end
    if vo.scene_id and vo.scene_id ~= scene_id then
        return false
    end
    return true
end
-- 家族答题逻辑
function SceneObjHolder:SetTouchAbleAboutAnswer()
    -- 如果是组队并且是跟随那就屏蔽队员和主角
    -- 否则是主角可碰撞
    -- able == false 检查答题碰撞逻辑
    local inScene = GuildData.Instance:IsInAnswerScene()
    local inTeam = TeamData.Instance:InTeam()
    local leader = TeamData.Instance:GetTeamInfo().leader
    local follow = TeamData.Instance:IsFollow()
    --[[ local v1 = Vector3.New(0.5, 2.5, 0.5)
    local v2 = Vector3.New(1.5, 2.5, 1.5) ]]
    if inScene then
        if inTeam and follow then
            local uid = leader.info.uid
            local role_id = RoleData.Instance:GetRoleId()
            BuglyReport("家族答题检查队长id = ",uid,"当前角色id = ",role_id)
            --[[ for _, obj in pairs(self.net_roles) do
                if obj.vo.role_id ~= uid then
                    obj:SetTouchAble(false)
                else
                    --只有队长可以
                    obj:SetTouchAble(true, v1)
                end
            end ]]
            --[[ if MainPlayer then
                MainPlayer:SetTouchAble(role_id == uid, v1)
            end ]]
        else
            --[[ for _, obj in pairs(self.net_roles) do
                obj:SetTouchAble(false)
            end
            if MainPlayer then
                MainPlayer:SetTouchAble(true, v1)
            end ]]
        end
    else
        -- 全员可碰或者不可碰
        --[[ for _, obj in pairs(self.net_roles) do
            obj:SetTouchAble(true, v2)
        end
        if MainPlayer then
            MainPlayer:SetTouchAble(false)
        end ]]
    end
end

function SceneObjHolder:SetCharaHide(uid,type)
    for _, obj in pairs(self.net_roles) do
        obj:HideRoleShow(type)
    end
end

function SceneObjHolder:GetCharaByUid(uid)
    for _, obj in pairs(self.net_roles) do
        if uid == obj.vo.role_id  then 
            return obj
        end 
    end
end

function SceneObjHolder:GetCharaVoByUid(uid)
    for _, vo in pairs(VoMgr:RoleVoList()) do
        if uid == vo.role_id  then 
            return vo
        end 
    end
end


function SceneObjHolder:Shielding(hide)
    for k, v in pairs(self.objs) do
        v:SetVisible(not hide)
    end
    for k, v in pairs(self.client_objs) do
        v:SetVisible(not hide)
    end
    if MainPlayer then
        MainPlayer:SetVisible(not hide)
    end
end

function SceneObjHolder:EnableAllObjShadow(enable)
    for k, v in pairs(self.objs) do
        v:EnableShadow(enable)
    end
    for k, v in pairs(self.client_objs) do
        v:EnableShadow(enable)
    end
    if MainPlayer then
        MainPlayer:EnableShadow(enable)
    end
end

function SceneObjHolder:ShieldingPet(hide)
    for _, obj in pairs(self.net_roles) do
        obj:ShieldingPet(hide)
    end
    if MainPlayer then
        MainPlayer:ShieldingPet(hide)
    end
end

function SceneObjHolder:ShieldingWithoutTypes(hide, obj_types)
    for k, obj in pairs(self.objs) do
        if not obj_types[obj:Type()] then
            obj:SetVisible(not hide)
        end
    end
    for k, obj in pairs(self.client_objs) do
        if not obj_types[obj:Type()] then
            obj:SetVisible(not hide)
        end
    end
    if MainPlayer then
        MainPlayer:SetVisible(not hide)
    end
end
function SceneObjHolder:EnableYinYangEye()
    for key, obj in pairs(self.client_objs) do
        obj:EnableYinYangEye()
    end
    for key, obj in pairs(self.net_roles) do
        obj:EnableYinYangEye()
    end
    if MainPlayer then
        MainPlayer:EnableYinYangEye()
    end
end
function SceneObjHolder:RevertYinYangEye()
    for key, obj in pairs(self.client_objs) do
        obj:RevertYinYangEye()
    end
    for key, obj in pairs(self.net_roles) do
        obj:RevertYinYangEye()
    end
    if MainPlayer then
        MainPlayer:RevertYinYangEye()
    end
end