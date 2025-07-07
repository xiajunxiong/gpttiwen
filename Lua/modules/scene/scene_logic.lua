SceneLogic = SceneLogic or BaseClass()
--主城传送点
MAINCITYWORLDPOS = {
    {x = 153, y = 117},
    --↘
    {x = 221, y = 205},
    --↖
    {x = 182.4333, y = 157.8167}
    --中间
}
MAINCITYVIEWPOS = {
    {x = 50.49, y = -32.18},
    {x = -37.90, y = 13.58},
    {x = 9.54, y = -10.98}
}
function SceneLogic:__init()
    if SceneLogic.Instance ~= nil then
        Debuger.LogError('[SceneLogic] attempt to create singleton twice!')
        return
    end
    SceneLogic.Instance = self
    self.scene_data = SceneData.Instance
    self.scene_ctrl = SceneCtrl.Instance
    self.holder = SceneCtrl.Instance:Objs()
    self.fly_end_func = nil
    self.override_inter_beh = nil --重写与寻路NPC交互的行为
    self.target_scene_id = 0
    self.move_to_pos_cache = SmartData.New()
    self.move_to_pos_cache.node_list = {}
    self.move_to_pos_cache.guaji = false
    self.auto_to_npc = nil
end
function SceneLogic:__delete()
end
--内部方法外部不要调用
--使用客户端坐标
function SceneLogic:MoveToPos(pos, end_func)
    if TeamData.Instance:NeedFollow() then
        PublicPopupCtrl.Instance:Center(Language.Team.TipsMove)
        return
    end
    if pos.y == 0 then
        StarTreasureMapCtrl.Instance:Check()
    end
    -- 在附近就不走了 运镖也不传送
    --[[ local level = RoleData.Instance:GetRoleLevel()
    local limit = Config.sundries_auto.constant[1].fly_levels
    if level > limit then
        if self:CheckMainCityFly(pos, end_func) then
            return
        end
    end ]]
    if MainPlayer then
        MainPlayer:MoveTo(pos, true, end_func)
    end
end
function SceneLogic:CheckMainCityFly(pos, end_func)
    local flag = false
    if self:SceneID() ~= self:MainCityID() or EscortCtrl.Instance:IsEscorting() then
        return flag
    end
    local mainPos = Vector2.New(MainPlayer:GetPos().x, MainPlayer:GetPos().z)
    local dis = GameMath.GetDistanceByPoint(mainPos, Vector2.New(pos.x, pos.z))
    local scene_cfg = self.scene_data:GetSceneCfg(self:MainCityID())
    local eX, _ = self:EntryPoint(self:MainCityID(), scene_cfg, mainPos)
    -- 我离哪个传送点近
    local eXX, _ = self:EntryPoint(self:MainCityID(), scene_cfg, {x = pos.x, y = pos.z}) -- 目标点离哪个传送点近
    if eX ~= eXX and dis > 1000 then --如果是同一个就不传了说明我离的很近
        self.move_to_pos_cache.target_pos = {x = pos.x, y = pos.z}
        self:FlyToPos(self:MainCityID())
        self.handle_delay1 =
            TimeHelper:AddDelayTimer(
            function()
                MainPlayer:MoveTo(pos, true, end_func)
            end,
            1
        )
        flag = true
    end
    return flag
end
--直飞某位置
function SceneLogic:AutoFlyToScene(scene_id, target)
    if scene_id == 0 then
        LogError('场景id有问题 找不到对应场景', scene_id)
        return
    end
    if EscortCtrl.Instance:IsEscorting() then
        PublicPopupCtrl.Instance:Center(Language.Map.BlockTips[2])
        return
    end
    local param_t = CommonStruct.FlyParam()
    param_t.scene_id = scene_id
    param_t.pos_x = target.x
    param_t.pos_y = target.y
    SceneCtrl.Instance:SendFlyByShoe(param_t)
end
function SceneLogic:FlyToPos(scene_id, end_func)
    if scene_id == 0 then
        LogError('场景id有问题 找不到对应场景', scene_id)
        return
    end
    if EscortCtrl.Instance:IsEscorting() then
        PublicPopupCtrl.Instance:Center(Language.Map.BlockTips[2])
        return
    end
    local scene_cfg = self.scene_data:GetSceneCfg(scene_id)
    local eX, eY = self:EntryPoint(scene_id, scene_cfg)
    -- 飞到场景的入口点
    if self.scene_ctrl:CurScene().PosToServer ~= nil then
        local x, y = self.scene_ctrl:CurScene():PosToServer(Vector3.New(eX, 0, eY), scene_cfg.mapid)
        local param_t = CommonStruct.FlyParam()
        param_t.scene_id = scene_id
        param_t.pos_x = x
        param_t.pos_y = y
        SceneCtrl.Instance:SendFlyByShoe(param_t)
        self.fly_end_func = end_func
    end
end
function SceneLogic:EntryPoint(to_scene_id, scene_cfg, target_pos)
    --[[ if to_scene_id == self:MainCityID() then
        local target = self.move_to_pos_cache.target_pos
        target = target_pos and target_pos or target
        if target then
            local MIN = math.min
            local dis = 99999999
            local res = 3
            for index, pos in ipairs(MAINCITYWORLDPOS) do
                local dis2 = GameMath.GetDistanceByPoint(pos, target)
                dis = MIN(dis, dis2)
                res = dis == dis2 and index or res
            end
            if target_pos == nil then
                self.move_to_pos_cache.target_pos = nil
            end
            res = MAINCITYWORLDPOS[res]
            return res.x, res.y
        end
    end ]]
    if scene_cfg.entry_point == nil then
        BuglyReport("to_scene_id = ", to_scene_id)
        return 0, 0
    end
    return scene_cfg.entry_point.x, scene_cfg.entry_point.y
end
function SceneLogic:FindTargetNpc(npc_vo, server)
    local exe_func = function(npc_obj)
        if Config.npc_auto.specific_npc[npc_obj.vo.id] == nil then
            npc_obj:LookAt()
        end
        if self.override_inter_beh ~= nil then
            local tmp_beh = self.override_inter_beh
            self.override_inter_beh = nil
            if type(tmp_beh) == 'function' then
                tmp_beh(npc_obj)
            else
                BehaviorCtrl.ExeByCfgVo(tmp_beh, npc_obj.vo)
            end
        else
            npc_obj:InteractionBehavior()
        end
        self.auto_to_npc = nil
    end
    if server then
        -- 服务端对象又是id 又有的是npcid 统一使用id 没有改成id的在vo源上修改
        self.holder:ForeachSNPC(
            function(npc_obj)
                if npc_obj.vo and npc_obj.vo.x == npc_vo.x and npc_obj.vo.y == npc_vo.y then
                -- and (npc_obj.vo.id and npc_vo.id and npc_obj.vo.id == npc_vo.id) then
                    exe_func(npc_obj)
                    return true
                end
                return false
            end
        )
    else
        self.holder:ForeachCNPC(
            function(npc_obj)
                if npc_obj.vo.seq == npc_vo.seq then
                    exe_func(npc_obj)
                    return true
                end
                return false
            end
        )
    end
end
function SceneLogic:ObjectPos(vo, server)
    local targetPos = Vector3.New(vo.x, 0, vo.y)
    local cur_scene = SceneCtrl.Instance:CurScene()
    if server then
        targetPos = cur_scene:PosToClint(vo.x or 0, vo.y or 0)
    else
        if vo == nil then
            BuglyReport("vo is nil out !!!!!!! why ??????????")
            return targetPos
        elseif vo.x == nil  or vo.y == nil then
            BuglyReport("pos is nil why ???? log vo = ", vo)
            return targetPos
        end
        targetPos.y = cur_scene:GetPosHeight(vo.x, vo.y)
    end
    local dis_cfg = Config.npc_auto.distance[vo.id]
    if dis_cfg then
        local player_pos = MainPlayer:GetPos()
        local pos = GameMath.DistancePos(player_pos.x, player_pos.z, targetPos.x, targetPos.z, dis_cfg.distance)
        targetPos.x = pos.x
        targetPos.z = pos.y
    end
    return targetPos
end
function SceneLogic:ClientPos(vo, server)
    local targetPos = Vector3.New(vo.x, 0, vo.y)
    if server then
        targetPos = SceneCtrl.Instance:CurScene():PosToClint(vo.x, vo.y)
    else
        targetPos.y = SceneCtrl.Instance:CurScene():GetPosHeight(vo.x, vo.y)
    end
    return targetPos
end

-- 外部调用
-- npc_vo可以是npc_id或者seq或者完整的vo表
-- 外部可自行传入vo vo结构需包含 【x】【y】 【scene_id】【id】等信息
-- 程序会先走到那 然后根据【id】寻找到对象 【一般用于服务端对象的寻找】
-- 寻找客户端对象是通过 【seq】 寻找服务端对象是通过【id】
-- beh: beh_cfg or beh_id or func or nil
-- beh 对象重写行为【需要寻找到对应NPC后才会执行行为】
-- server 传入坐标是否是服务端坐标 【默认不是】
-- fly 要不要直接飞行 【默认不飞】
function SceneLogic:AutoToNpc(npc_vo, beh, server, fly)
    if MainViewData.Instance:IsBattle() then
        return
    end
    if TeamData.Instance:NeedFollow() then
        PublicPopupCtrl.Instance:Center(Language.Team.TipsMove)
        return
    end
    if MarryData.Instance:IsParadeFloat() then
        PublicPopupCtrl.Instance:Center(Language.Marry.ParadeFloatTip)
        return
    end
    if server == false and fly == true and tonumber(npc_vo) and FightCtrl.IsRequestFb(function()
        self:AutoToNpc(npc_vo, beh, server, fly)end,VoMgr:NpcTargetByNpcId(npc_vo)) then
        return
    end
    self:ClearMoveCache()
    server = server or false
    fly = fly or false

    -- 检查重写行为
    self:CheckBehaivor(beh)
    -- 检查选中对象
    local cache = npc_vo
    if type(npc_vo) == 'number' then
        npc_vo = VoMgr:NpcVoBySeq(npc_vo)
        if npc_vo ~= nil then
            npc_vo.obj_type = SceneObjType.Npc
        end
    end
    if npc_vo == nil then
        npc_vo = VoMgr:NpcVoByNpcId(cache)
        npc_vo = npc_vo[1]
        if npc_vo == nil then
            LogError('未找到NPC配置:', cache)
            return
        end
        npc_vo.obj_type = SceneObjType.Npc
    end
    if npc_vo.is_server == true then
        server = true
    end
    local x = npc_vo.x
    local y = npc_vo.y
    
    self.move_to_pos_cache.cur_target = npc_vo
    self.move_to_pos_cache.target_pos = {x = x, y = y}
    --存储
    self.move_to_pos_cache.server = server
    self.move_to_pos_cache.fly = fly
    self.move_to_pos_cache.beh = self.override_inter_beh
    self.auto_to_npc = true
    local move_func = function(npc_vo, server)
        -- 指引中停止移动 指引中走这里的逻辑会有影响 例如指引去干某事或者找NPC
        --[[ if GuideManager.Instance:IsGuide() then return end ]]

        local target_pos = self:ObjectPos(npc_vo, server)
        self.move_to_pos_cache.end_func = function()
            self:FindTargetNpc(npc_vo, server)
        end
        self.handle_delay2 =
            TimeHelper:AddDelayFrameTimer(
            function()
                self:MoveToPos(target_pos, self.move_to_pos_cache.end_func)
            end,
            1
        )
    end
    -- 同场景移动
    local cur_scene = self:SceneID()
    if cur_scene == npc_vo.scene_id then
        move_func(npc_vo, server)
        return
    end
    -- 飞行移动
    if fly then
        self:FlyToPos(
            npc_vo.scene_id,
            function()
                move_func(npc_vo, server)
            end
        )
    else
        self:CrossScene(
            npc_vo.scene_id,
            function()
                move_func(npc_vo, server)
            end
        )
    end
end
-- 外部调用
-- 场景id 【scene_id】
-- target_pos 【x,y】的【table】
-- 重写行为 【寻路后直接执行行为】
-- 是否是服务端位置 【默认不是】
-- 是否飞行 【默认不飞】
function SceneLogic:AutoToPos(scene_id, target_pos, beh, server, fly)
    if MainViewData.Instance:IsBattle() then
        return
    end
    if MarryData.Instance:IsParadeFloat(scene_id) then
        PublicPopupCtrl.Instance:Center(Language.Marry.ParadeFloatTip)
        return true
    end
    if target_pos == nil and FightCtrl.IsRequestFb(function()
        self:AutoToPos(scene_id, target_pos, beh, server, fly)end,scene_id) then
        return
    end

    self:ClearMoveCache()
    server = server or false
    fly = fly or false
    -- 检查重写行为
    self:CheckBehaivor(beh)

    local cur_scene = self:SceneID()
    scene_id = scene_id == nil and cur_scene or scene_id
    if target_pos then
        self.move_to_pos_cache.target_pos = {x = target_pos.x, y = target_pos.y}
    end

    self.move_to_pos_cache.scene_id = scene_id
    self.move_to_pos_cache.server = server
    self.move_to_pos_cache.fly = fly
    self.move_to_pos_cache.beh = self.override_inter_beh
    self.auto_to_npc = false

    local end_func = function()
        self:ExeBehavior()
    end
    self.move_to_pos_cache.end_func = end_func
    local move_func = function(target_pos, end_func)
        if target_pos then
            self.handle_delay3 =
                TimeHelper:AddDelayFrameTimer(
                function()
                    self:MoveToPos(self:ClientPos(target_pos, server), end_func)
                end,
                1
            )
        else
            end_func()
        end
    end
    -- 同场景移动
    if cur_scene == scene_id then
        if self.override_inter_beh and type(self.override_inter_beh) ~= 'function' then
            if self.override_inter_beh.behavior_type == 102 then --TempCode
                end_func()
                return
            end
        end
        move_func(target_pos, end_func)
        return
    end
    -- 飞行移动
    if fly then
        self:FlyToPos(
            scene_id,
            function()
                move_func(target_pos, end_func)
            end
        )
    else
        self:CrossScene(
            scene_id,
            function()
                move_func(target_pos, end_func)
            end
        )
    end
end

function SceneLogic:ExeBehavior()
    if self.override_inter_beh ~= nil then
        if type(self.override_inter_beh) == 'function' then
            local tmp_beh = self.override_inter_beh
            self.override_inter_beh = nil
            tmp_beh()
            self.auto_to_npc = nil
            return
        end
        local tmp_beh = self.override_inter_beh
        self.override_inter_beh = nil
        BehaviorCtrl.ExeByCfgVo(tmp_beh)
        if tmp_beh.behavior_type == 101 then --keyuse导致挖宝表现问题
            self.auto_to_npc = nil
        end
    end
end

function SceneLogic:CheckBehaivor(beh)
    self.override_inter_beh = nil
    if beh == nil or type(beh) == 'table' or type(beh) == 'function' then
        self.override_inter_beh = beh
    else
        self.override_inter_beh = BehaviorCtrl.GetCfg(beh)
    end
end

function SceneLogic:CrossScene(scene_id, end_func)
    local scene_cfg = self.scene_data:SceneConfig()
    if scene_cfg == nil then
        return
    end
    self.move_to_pos_cache.move_end_func = end_func

    -- self.node_list = {}
    -- self.checked_list = {}
    
    self.path_list = {}
    self.target_scene_id = scene_id

    if self.target_scene_id == scene_cfg.id then
        return
    end
    self:FindPath2()

    -- self:FindPath(scene_cfg.id)

    if table.nums(self.path_list) == 0 then--self.node_list
        if SceneData.Instance:GetMode() == SceneModeType.ArenaMatch then
            self:AutoToPos(self.target_scene_id, nil, end_func, nil, true)
        elseif SceneData.Instance:GetMode() ~= SceneModeType.None then
            PublicPopupCtrl.Instance:Center(ErrorText[106])
        end
        return
    end
    local idx
    local max = 9999
    for i, list in pairs(self.path_list) do
        local num = table.nums(list)
        if max > num then
            idx = i
            max = num
        end
    end


    -- local cur_node = nil
    -- for _, v in pairs(self.node_list) do
    --     if v.to_scene_id == self.target_scene_id then
    --         cur_node = v
    --         break
    --     end
    -- end
    -- if cur_node == nil then
    --     return
    -- end
    -- local path_list = {}
    -- path_list[1] = cur_node
    -- local loop_count = 0
    -- if nil ~= cur_node then
    --     while loop_count < 200 do
    --         for k, v in pairs(self.node_list) do
    --             if v == cur_node.prve_node then
    --                 table.insert(path_list, 1, v)
    --                 cur_node = v
    --                 if v.scene_id == scene_cfg.id then
    --                     break
    --                 end
    --             end
    --         end
    --         loop_count = loop_count + 1
    --     end
    -- end

    self.move_to_pos_cache.node_list = self.path_list[idx]
    local t = table.remove(self.move_to_pos_cache.node_list, 1)

    self.move_to_pos_cache.last_move_pos = t
    self.move_to_pos_cache.last_scene_id = scene_cfg.id

    self:MoveToPos(
        self:ClientPos(t),
        function()
            self.auto_to_npc = nil
        end
    )
end
function SceneLogic:FindPath2()
    local scene_cfg = self.scene_data:SceneConfig()
    if scene_cfg == nil then
        return
    end
    local path = {}
    self.checked_list2 = {}
    self.seaching_list = {}
    for index, value in ipairs(scene_cfg.doors) do
        if nil ~= value.to_scene_id then
            local node = {}
            node.x = value.x
            node.y = value.y
            node.to_scene_id = value.to_scene_id
            node.scene_id = scene_cfg.id
            node.prve_node = nil
            table.insert(path, node)
            self.seaching_list[index] = {node}
            self.checked_list2[index] = {}
        end
    end
    for index, node in ipairs(path) do
        self:SearchPath(node.to_scene_id, node, index)
    end
    local target = {}
    for index, list in ipairs(self.seaching_list) do
        for _, node in pairs(list) do
            if node.to_scene_id == self.target_scene_id then
                target[index] = node
                break
            end
        end
    end
    local path_list = {}
    for index, cur_node in pairs(target) do
        path_list[index] = path_list[index] or {}
        path_list[index][1] = cur_node
        local loop = 0
        if nil ~= cur_node then
            while loop < 200 do
                for _, v in pairs(self.seaching_list[index]) do
                    if v == cur_node.prve_node then
                        table.insert(path_list[index], 1, v)
                        cur_node = v
                        if v.scene_id == scene_cfg.id then -- 返回到起始场景结束
                            break
                        end
                    end
                end
                loop = loop + 1
            end
        end
    end
    self.path_list = path_list
end
function SceneLogic:SearchPath(scene_id, prve_node, index)
    local scene_cfg = self.scene_data:GetSceneCfg(scene_id)
    if scene_cfg == nil then
        return
    end
    local searching = self.seaching_list[index]
    local checked = self.checked_list2[index]
    checked[scene_id] = 1

    for _, door in ipairs(scene_cfg.doors) do
        if nil ~= door.to_scene_id and prve_node.scene_id ~= door.to_scene_id then
            local node = {}
            node.x = door.x
            node.y = door.y
            node.to_scene_id = door.to_scene_id
            node.scene_id = scene_id
            node.prve_node = prve_node --前一个
            table.insert(searching, node)
            if door.to_scene_id == self.target_scene_id then
                return
            else
                if checked[door.to_scene_id] ~= 1 then
                    self:SearchPath(door.to_scene_id, node, index)
                end
            end
        end
    end
end
function SceneLogic:FindPath(scene_id, node)
    local scene_cfg = self.scene_data:GetSceneCfg(scene_id)
    if scene_cfg == nil then
        return
    end
    -- 当前场景
    self.checked_list[scene_id] = 1
    if node == nil then
        for _, v in ipairs(scene_cfg.doors) do
            if v.to_scene_id ~= nil then
                local cur_node = {}
                cur_node.x = v.x
                cur_node.y = v.y
                cur_node.to_scene_id = v.to_scene_id
                cur_node.scene_id = scene_id
                cur_node.prve_node = node --前一个
                self.node_list[#self.node_list + 1] = cur_node
                if v.to_scene_id == self.target_scene_id then
                    return
                end
            end
        end
    end
    for _, v in ipairs(scene_cfg.doors) do
        if v.to_scene_id ~= nil then
            local cur_node = {}
            cur_node.x = v.x
            cur_node.y = v.y
            cur_node.to_scene_id = v.to_scene_id
            cur_node.scene_id = scene_id
            cur_node.prve_node = node --前一个
            self.node_list[#self.node_list + 1] = cur_node
            if v.to_scene_id == self.target_scene_id then
                --找到目的地信息
                return
            else
                if self.checked_list[v.to_scene_id] ~= 1 then
                    self:FindPath(v.to_scene_id, cur_node)
                end
            end
        end
    end
end
function SceneLogic:ClickToObj(obj, is_server)
    self.move_to_pos_cache.cur_target = obj.vo
    SE:Play(SceneEffectConfig.EffectList.SelectEffect, obj:GetPos())
    local end_func = function()
        if obj.vo and obj.vo.id and Config.npc_auto.specific_npc[obj.vo.id] == nil then
            obj:LookAt()
        end
        if not self:IsMultObjs(obj) then
            obj:InteractionBehavior()
        end
    end
    --判断位置
    local target_pos
    local obj_type = obj:Type()
    if SceneObjType.WeddingBaoWeiXiuQiuMonster == obj_type or SceneObjType.RabbitRace == obj_type then
        target_pos = obj:GetPos()
        if obj.OnClickObjEvent then
            obj:OnClickObjEvent()
        end
    elseif SceneObjType.UTAStatue == obj_type then
        target_pos = obj:GetTargetPos()
    else
        target_pos = self:ObjectPos(obj.vo, is_server)
    end
    local player_pos = MainPlayer:GetPos()
    if GameMath.GetDistance(player_pos.x, player_pos.z, target_pos.x, target_pos.z, false) < 1.69 then
        end_func()
        return
    end
    self:MoveToPos(target_pos, end_func)
end

function SceneLogic:IsMultObjs(obj)
    local vo_list = {}
    local objs_holder = SceneCtrl.Instance:Objs()
    local client_objs = objs_holder.client_objs
    local net_npcs = objs_holder.net_npcs
    local net_monsters = objs_holder.net_monsters
    local pos_t = obj:GetPos()
    objs_holder:ForeachRole(
        function(role_obj)
            local role_point = role_obj:GetPos()
            if GameMath.GetDistance(pos_t.x, pos_t.z, role_point.x, role_point.z, false) < 4 then
                table.insert(vo_list, DataHelper.TableCopy(role_obj.vo))
            end
        end
    )
    for _, client_obj in pairs(client_objs) do
        if client_obj:Type() == SceneObjType.Npc and client_obj.vo.res_id ~= 800 and client_obj.vo.res_id ~= 802 then
            local npc_point = client_obj:GetPos()
            if GameMath.GetDistance(pos_t.x, pos_t.z, npc_point.x, npc_point.z, false) < 4 then
                table.insert(vo_list, DataHelper.TableCopy(client_obj.vo))
            end
        end
    end
    for _, npc_obj in pairs(net_npcs) do
        -- if npc_obj:Type() == SceneObjType.Npc then
        local npc_point = npc_obj:GetPos()
        if GameMath.GetDistance(pos_t.x, pos_t.z, npc_point.x, npc_point.z, false) < 7 then
            table.insert(vo_list, DataHelper.TableCopy(npc_obj.vo))
        end
        -- end
    end
    for _, monster_obj in pairs(net_monsters) do
        local monster_point = monster_obj:GetPos()
        if GameMath.GetDistance(pos_t.x, pos_t.z, monster_point.x, monster_point.z, false) < 4 then
            table.insert(vo_list, DataHelper.TableCopy(monster_obj.vo))
        end
    end

    -- 筛选vo
    for i = #vo_list, 1, -1 do
        local vo = vo_list[i]
        local obj_type = vo.obj_type
        if obj_type == SceneObjType.RabbitRace then
            table.remove(vo_list, i)
        end
    end

    if #vo_list > 1 then
        SocietyCtrl.Instance:ClickToRoleVoList(vo_list)
        return true
    else
        return false
    end
end

function SceneLogic:ClickToRole(obj)
    --[[ if GuildData.Instance:IsOpenGuildMelee() then
        if Time.time - obj.vo.time > 5 then
            BattleCtrl.ChallengeOthersReq(BATTLE_MODE.BATTLE_MODE_GUILD_FIGHT, obj.vo.role_id)
        else
            PublicPopupCtrl.Instance:Center(ErrorText[139])
        end
        return
    end ]]
    --[[ if SceneData.Instance:GetMode() == SCENE_TYPE.ONLY_FIGHT then
        --判断一下是否开启了
        local status = ActivityData.Instance:GetActivityStatus(ActType.OnlyFight)
        if status == ActStatusType.Open then
            if Time.time - obj.vo.time > 5 then
            BattleCtrl.ChallengeOthersReq(BATTLE_MODE.BATTLE_MODE_ONLY_FIGHT, obj.vo.role_id)
            else
                PublicPopupCtrl.Instance:Center(ErrorText[139])
            end
        else
            PublicPopupCtrl.Instance:Center(ErrorText[129])
        end
        return
    end ]]
    if not self:IsMultObjs(obj) then
        SocietyCtrl.Instance:ClickToRoleVo(obj.vo)
    end
end

function SceneLogic:OnGameSceneLoaded(scene_id, scene_type)
    local cache_scene_id = self.move_to_pos_cache.scene_id or 0
    if GuildData.Instance.need_switch_scene then
        SceneLogic.Instance:AutoToNpc(GuildCtrl.Instance.data:GetGuildTaskNpcId(), nil, false, true)
        GuildData.Instance.need_switch_scene = false
    end
    if cache_scene_id == scene_id and FightConfig.IS_NEED_CONTINUE[scene_type] then
        self:Continue()
    end
    if self.is_auto_map == true and cache_scene_id == scene_id and scene_type == SCENE_TYPE.COMMON_SCENE then
        self:Continue()
    end
    if EscortCtrl.Instance:IsEscorting() and table.nums(self.move_to_pos_cache.node_list) == 0 then
        self:Continue()
        return
    end
    -- 场景加载完成 这里要判断是不是战斗->场景
    if table.nums(self.move_to_pos_cache.node_list) > 0 then
        local cache_func = function()
            local t
            if self.move_to_pos_cache.last_scene_id == scene_id then
                t = self.move_to_pos_cache.last_move_pos
            else
                t = table.remove(self.move_to_pos_cache.node_list, 1)
            end
            self.move_to_pos_cache.last_move_pos = t
            self.move_to_pos_cache.last_scene_id = scene_id
            if t then --在延迟中被清空了
                MainPlayer:MoveTo(self:ClientPos(t), true)
            end
        end
        self.handle_delay4 = TimeHelper:AddDelayTimer(cache_func, 1)
    elseif table.nums(self.move_to_pos_cache.node_list) == 0 then
        local t = self.move_to_pos_cache.last_move_pos
        if t and self.move_to_pos_cache.last_scene_id == scene_id and t.scene_id == scene_id then
            MainPlayer:MoveTo(self:ClientPos(t), true)
            return
        end
        if self.move_to_pos_cache.move_end_func then
            self.handle_delay5 = TimeHelper:AddDelayTimer(self.move_to_pos_cache.move_end_func, 1)
            self.move_to_pos_cache.move_end_func = nil
        end
    end
    if self.move_to_pos_cache.guaji == true then
        self:GoOnPatrol()
    end
    if scene_type == SCENE_TYPE.FANTASY_FB then
        FightCtrl.Instance:FightFantasyAuto({check_win = true})
    end
    if self.fly_end_func then
        self.handle_delay6 = TimeHelper:AddDelayTimer(self.fly_end_func, self:SceneID() == self:MainCityID() and 2 or 1)
        self.fly_end_func = nil
    end
end

--继续(修改为内部方法)
function SceneLogic:Continue()
    self.handle_delay7 =
        TimeHelper:AddDelayTimer(
        function()
            if self.auto_to_npc == true then
                local target = self.move_to_pos_cache.cur_target
                local server = self.move_to_pos_cache.server
                local fly = self.move_to_pos_cache.fly
                local beh = self.move_to_pos_cache.beh
                self:AutoToNpc(target, beh, server, fly)
            elseif self.auto_to_npc == false then
                local scene_id = self.move_to_pos_cache.scene_id
                local target_pos = self.move_to_pos_cache.target_pos
                local server = self.move_to_pos_cache.server
                local fly = self.move_to_pos_cache.fly
                local beh = self.move_to_pos_cache.beh
                self:AutoToPos(scene_id, target_pos, beh, server, fly)
            end
        end,
        0.5
    )
end

function SceneLogic:ClearMoveCache(cache_guaji)
    TimeHelper:CancelTimer(self.handle_delay1)
    TimeHelper:CancelTimer(self.handle_delay2)
    TimeHelper:CancelTimer(self.handle_delay3)
    TimeHelper:CancelTimer(self.handle_delay4)
    TimeHelper:CancelTimer(self.handle_delay5)
    TimeHelper:CancelTimer(self.handle_delay6)
    TimeHelper:CancelTimer(self.handle_delay7)

    if self.repeat_call ~= nil then
        Runner.Instance:CancelRepeat(self.repeat_call)
        self.repeat_call = nil
    end
    if self.move_to_pos_cache.guaji then
        self.move_to_pos_cache.guaji = false
        RoleSettingCtrl.Instance:SendRolePatrol(false)
        if cache_guaji then
            UnityPlayerPrefs.SetInt(PrefKeys.GoPatrol(), math.floor(Time.time))
        end
    end
    
    self.move_to_pos_cache.node_list = {}
    self.move_to_pos_cache.move_end_func = nil
    self.move_to_pos_cache.last_scene_id = 0
    self.move_to_pos_cache.last_move_pos = nil
    self.move_to_pos_cache.beh = nil
    --self.move_to_pos_cache.cur_target = nil
    self.auto_to_npc = nil

    self.is_auto_map = false
    if PublicPopupCtrl.Instance then
        PublicPopupCtrl.Instance.stop = false
    end
    if ChatCtrl.Instance then
        ChatCtrl.Instance.stop = false
    end
    if MainOtherCtrl.Instance then
        MainOtherCtrl.Instance.stop = false
    end

    --[[ if MainPlayer then
        MainPlayer:StopMove()
    end ]]
    --RoleSettingCtrl.Instance:SetEncounterMineMonsterFlag(0)
end

-- 简单巡逻
function SceneLogic:GoOnPatrol()
    RoleSettingCtrl.Instance:SendRolePatrol(true)
    if self.patroling == true then
        self.patroling = false
    end
    if self.repeat_call then
        return
    end
    -- Runner.Instance:CancelRepeat(self.repeat_call)
    -- self.repeat_call = Runner.Instance:RepeatCall(BindTool.Bind(self.EasyPatrol, self), 0.5)
    self.move_to_pos_cache.guaji = true
    local pos_list = SceneData.Instance:GetXunLuPos()
    if pos_list == nil then
        self.repeat_call = Runner.Instance:RepeatCall(BindTool.Bind(self.EasyPatrol, self), 0.5)
    else
        self.repeat_call = Runner.Instance:RepeatCall(BindTool.Bind(self.Patrol, self), 0.5)
    end
end

local check_time = os.time()
function SceneLogic:Patrol()
    if BattleData.Instance:BaseData().server_fighting == true then
        return
    end
    if MainPlayer and MainPlayer:IsInAnim(ChAnimType.Idle) then
        if os.time() - check_time > 5 then
            self.patroling = false
        end
    else
        check_time = os.time()
    end
	if MainPlayer and MainPlayer.restar_auto_pos then
		self.patroling = false
		MainPlayer.restar_auto_pos = false
	end
    if self.patroling == true then
        return
    end
    check_time = os.time()
    self:XunLu()
end
function SceneLogic:XunLu()
    self.patroling = true
    local pos_list = SceneData.Instance:GetXunLuPos()
    if pos_list == nil then return end
    local max = table.nums(pos_list)
    local p1 = MainPlayer:GetPos()
    local p2 = Vector3.New(0, 0, 0)
    local point = self:RamdonPoint(max, self.old_point, pos_list)
    self.old_point = point
    p2.x = point.x
    p2.y = SceneCtrl.Instance:CurScene():GetPosHeight(point.x, point.y)
    p2.z = point.y
    MainPlayer:MoveTo(
        p2,
        true,
        function()
            self.patroling = false
        end
    )
end
function SceneLogic:RamdonPoint(max, old, list)
    local point = list[GameMath.Ramdon(max)]
    if old and old == point then
        return self:RamdonPoint(max, old, list)
    end
    return point
end
function SceneLogic:EasyPatrol()
    if MainViewData.Instance:BaseData().is_battle == true then
        return
    end
    if MainPlayer == nil then
        return
    end
    local main_point = MainPlayer:GetPos()
    local new_point = Vector3.New(0, 0, 0)
    new_point.x = main_point.x + GameMath.Ramdon(-5, 5)
    new_point.y = main_point.y
    new_point.z = main_point.z + GameMath.Ramdon(-5, 5)
    MainPlayer:MoveTo(new_point, true)
end

function SceneLogic:CurTarget()
    return self.move_to_pos_cache.cur_target
end

function SceneLogic:MoveCache()
    return self.move_to_pos_cache
end

function SceneLogic:MainCityID()
    return 108
end

function SceneLogic:SceneID()
    return self.scene_data:SceneId()
end

--跳到NPC需要切场景的
function SceneLogic:AutoToNpcs(npc_id,beh)
    local npc_config = VoMgr:NpcVoByNpcId(npc_id)
    if npc_config then
        local scene_id = npc_config[1] and npc_config[1].scene_id or 0
        if scene_id == SceneData.Instance:SceneId() or scene_id == 0 then
            self:AutoToNpc(npc_id,beh)
        else
            self:AutoToPos(scene_id,nil,function()
                self:AutoToNpc(npc_id,beh)
            end,nil,true)
        end
    end
end

function SceneLogic:Beh()
    return self.override_inter_beh
end

function SceneLogic:CancelBeh()
    self.override_inter_beh = nil
end

--强制设置当前场景摄像机根节点 root_node ObjectGame
function SceneLogic:ForceCamera(root_node)
    local scene_cfg = SceneData.Instance:SceneConfig()
	local vir_cam_obj = SceneMgr.FindGameObjectWithTag("MainVirtualCamera")
	local vir_cam = vir_cam_obj:GetComponent(typeof(Cinemachine.CinemachineVirtualCamera))
    local show_virtual = MPNEEDVIRTUALCAMERA[scene_cfg.id]
	if show_virtual then
		vir_cam.Priority = 8
	else
        vir_cam.Follow = root_node.transform
		vir_cam.Priority = 1
	end
	local vir_cam_obj2 = SceneMgr:GetFreeLookCamera(SceneManager.FreeLookCamType.FreeLookCamera)
	local vir_cam2 = vir_cam_obj2:GetComponent(typeof(Cinemachine.CinemachineFreeLook))
    vir_cam2.Follow = root_node.transform

	local vir_cam_obj3 = SceneMgr:GetFreeLookCamera(SceneManager.FreeLookCamType.FreeLookCamera2)
	local vir_cam3 = vir_cam_obj3:GetComponent(typeof(Cinemachine.CinemachineFreeLook))
    vir_cam3.Follow = root_node.transform
	vir_cam3.Priority = MPNEEDCHANGE[scene_cfg.id] and 6 or 4

	local vir_cam_obj4 = SceneMgr:GetFreeLookCamera(SceneManager.FreeLookCamType.FishSceneCamera)
	local vir_cam4 = vir_cam_obj4:GetComponent(typeof(Cinemachine.CinemachineFreeLook))
    vir_cam4.Follow = root_node.transform
end