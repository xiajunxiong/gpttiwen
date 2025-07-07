local xray_mat
local alpha_mat
PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        ResMgr:LoadObjectOfType(
            "material/XRay.mat",
            typeof(UnityEngine.Material),
            function(obj)
                xray_mat = obj
                PreloadCtrl.UnregisterLoading()
            end
        )
    end
)

PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        ResMgr:LoadObjectOfType(
            "material/Alpha_1.mat",
            typeof(UnityEngine.Material),
            function(obj)
                alpha_mat = obj
                PreloadCtrl.UnregisterLoading()
            end
        )
    end
)

SceneCh = SceneCh or BaseClass(SceneObj)
--npc,monster,role
function SceneCh:__init()
    self.vo = nil
    self.in_ride = false
    self.nav_mesh_path = nil
    --self.move_end_func

    self.move_wait = nil
    self.path_pos_list = {}
    self.touch_began_time = 0
    self.follow_queue_id = nil --跟随的queue_id
    --附属品列表主要用于box掉落的物品展示
    self.attach_obj_list = {}
    self.role_setting_change_handle = EventSys:Bind(GlobalEventKeys.RoleSettingChange, BindTool.Bind(self.RoleSettingChange, self))
    -- self.role_pos_change_handle = EventSys:Bind(GlobalEventKeys.RolePosChange, BindTool.Bind(self.MainRolePosChange, self))
    self.handle_team_list = TeamData.Instance:GetTeamInfo():Care(BindTool.Bind(self.RoleSettingChange, self))
    self.restar_auto_pos = false

    self.shield_ride = false    --是否屏蔽坐骑

    self.flush_follow_care1 = nil
    self.flush_follow_care2 = nil
end

function SceneCh:__delete()
    TeamData.Instance:GetTeamInfo():Uncare(self.handle_team_list)
    if self.delayAttachItem ~= nil then
        TimeHelper:CancelTimer(self.delayAttachItem)
        self.delayAttachItem = nil
    end
    TimeHelper:CancelTimer(self.timer_niuniu_fb)
    if #self.attach_obj_list > 0 then
        for k, v in pairs(self.attach_obj_list) do
            v:DeleteMe()
        end
        self.attach_obj_list = {}
    end
    if self.handle_pet then
        self.vo:Uncare(self.handle_pet)
        self.handle_pet = nil
    end

    if self.flush_follow_care1 then
        self.vo:Uncare(self.flush_follow_care1)
        self.flush_follow_care1 = nil
    end
    if self.flush_follow_care2 then
        self.vo:Uncare(self.flush_follow_care2)
        self.flush_follow_care2 = nil
    end
    if self.move_wait ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.move_wait)
        self.move_wait = nil
    end
    if self.follow_pet then
        self.follow_pet:DeleteMe()
        self.follow_pet = nil
    end
    if self.follow_escort_car then
        self.follow_escort_car:DeleteMe()
        self.follow_escort_car = nil
    end
    if self.LeaderObj ~= nil then
        UnityGameObj.Destroy(self.LeaderObj)
        self.LeaderObj = nil
    end
    if self.TaskImgObj ~= nil then
        UnityGameObj.Destroy(self.TaskImgObj)
        self.TaskImgObj = nil
    end
    if self.role_setting_change_handle then
        EventSys:UnBind(self.role_setting_change_handle)
    end
    if self.role_pos_change_handle then
        EventSys:UnBind(self.role_pos_change_handle)
    end
    if self.arena_match_show then
        self.arena_match_show:DeleteMe()
        self.arena_match_show = nil
    end
    if self.follow_shengqi then
        self.follow_shengqi:DeleteMe()
        self.follow_shengqi = nil
    end
end
function SceneCh:FlushRoleDrawer()
    SceneObj.FlushRoleDrawer(self)
    if self.body == nil or self.vo == nil or next(self.vo.appearance) == nil then
        return
	end
    if self:InTheFishingScene() then
        return
    end
    if self:InTheWeddingScene() then
        return
    end
    --武器坐骑装载 appearance.model_card_flag 变身卡标记，有则不刷新武器
    local appearance = self.vo.appearance
    local card_model_res_id = LiveData.Instance:CardModel(appearance)
    local lovers_match_res_id = LoversMatchData.Instance:GetRoleFormResId(self:Vo())
    if appearance.weapon_id and appearance.weapon_id > 0 and not card_model_res_id and lovers_match_res_id == nil
     and not appearance.henshin_monkey --[[ and appearance.special_appearance_type ~= 1 ]] then
        local weapon = Config.equipment_auto[appearance.weapon_id]
        local sub_weapon = Config.equipment_auto[appearance.sub_weapon_id]
        local weapon_type = weapon and weapon.weapon_type or nil
        local sub_weapon_type = sub_weapon and sub_weapon.weapon_type or nil
        self.body:SetWeapon(ResHelper.WeaponResId(weapon_type, appearance.fashion_weapon_id, sub_weapon_type,nil,appearance.advance_time))
    else
        self.body:SetWeapon(0, 0)
    end
    if (appearance.ride_id or appearance.bicycle_index) and not self.shield_ride and LiveData.Instance:CardModel(appearance) == nil and lovers_match_res_id == nil then
        local ride_id
        if appearance.ride_id > 0 then
            ride_id = appearance.ride_id
        elseif appearance.bicycle_index >= 0 then -- 判断是否乘坐了灵骑
            local bicycle = BicycleData.Instance:GetBicycleByIndex(appearance.bicycle_index)
            ride_id = bicycle:ModelId(appearance.bicycle_model_type)
        end
        if ride_id then
            self.body:SetRide(ride_id)
            self.center_point_offset_cache = Vector3.zero
            self:FlushCenterPointFollow()
            self.in_ride = true
        else
            self.body:SetRide(0)
            self.center_point_offset_cache = nil
            self:FlushCenterPointFollow()
        end
    else
        self.in_ride = false
        self.body:SetRide(0)
        self.center_point_offset_cache = nil
        self:FlushCenterPointFollow()
    end
    self:SetFlyEffect(appearance)
end

function SceneCh:InTheFishingScene()
    if FishData.Instance:InFishMap() and self.vo.obj_type == SceneObjType.MainRole then
        return true
    end
    return false
end

function SceneCh:InTheWeddingScene()
    if SCENE_TYPE.WEDDING_FB == SceneData.Instance:GetMode() and self.vo.obj_type == SceneObjType.MainRole then
        return true
    end
    return false
end

function SceneCh:StopMove(flag)
    if self.move_wait == nil then
        return
    end
    Runner.Instance:RemoveRunUntilTrue(self.move_wait)
    self.move_wait = nil
    if self.vo.obj_type == SceneObjType.MainRole and not flag then
        self:SyncPos()
    end
    self.body:PlayAnim(ChAnimType.Idle)
    self.nav_agent:ResetPath()
end
function SceneCh:SyncPos()
    SceneCtrl.Instance:MainChRequestMove(self:GetPos(), self:GetPos())
end
function SceneCh:RoleSettingChange()
    self:VisibilityJudge()
    self:FlushIsLeader()
end
function SceneCh:MainRolePosChange()
    if self.vo and self.vo.obj_type == SceneObjType.Npc then
        local player_pos = MainPlayer:GetPos()
        if GameMath.GetDistance(player_pos.x, player_pos.z, self:GetPos().x, self:GetPos().z, false) < 5 then
            self:LookAt()
        end
    end
end

function SceneCh:MoveTo(pos, server_sync, end_func)
    -- LogError("SceneCh:MoveTo",pos)
    -- if self.vo.name == "士贞玉昆" then
    --     Debuger.LogErrorSD("士贞玉昆=MoveTo===="..tostring(pos))
    -- end
    --战斗中，停止接收移动命令
    if BattleData.Instance:IsServerFighting() then
        return
    end
    if self.nav_mesh_path == nil then
        self.nav_mesh_path = UnityAI.NavMeshPath()
    end
    if self.move_wait ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.move_wait)
        self.move_wait = nil
    end
    if self.nav_agent.isOnNavMesh == false then
        -- LogError("NAV AGENT 没准备好")
        --[[ if DEBUG_MODE then
            LogError("NAV AGENT 没准备好",self.vo.name)
        end ]]
        return 1
    end
    if self.nav_agent:CalculatePath(pos, self.nav_mesh_path) == false then
        if DEBUG_MODE then
            LogError("没有路径，可能坐标不同步", pos, self.vo.name)
        end
        -- self.body:PlayAnim(ChAnimType.Idle)
		self.restar_auto_pos = true
        return
    end
    local path_poses
    self.path_pos_list = {}
    if server_sync then
        path_poses = self.nav_mesh_path.corners:ToTable()
        table.remove(path_poses, 1)
    else
        path_poses = {[1] = pos}
    end
    --[[ table.remove(path_poses,#path_poses)
    table.insert(path_poses, pos)
    LogError(path_poses) ]]
    -- 对路径点进行修改
    -- 改变路径中每个点的位置，为了不贴边缘走，这样很怪。
    -- local new_path_poses = path_poses
    --[[ if self:Type() == SceneObjType.MainRole then
        for i = 1, #path_poses - 1, 1 do
            local flag, hit = UnityAI.NavMesh.FindClosestEdge(path_poses[i], nil, -1)
            if flag and hit.distance < 0.5 then
                path_poses[i] = hit.position + hit.normal * 0.5
            end
        end
    end ]]
    if end_func and MainPlayer then
        local num = table.nums(path_poses)
        if num <= 0 then
            return
        end
        local end_point
        if num >= 2 then
            end_point = Vector3.MoveTowards(path_poses[num], path_poses[num - 1], 1.69)
        elseif num == 1 then
            end_point = Vector3.MoveTowards(path_poses[num], MainPlayer:GetPos(), 1)
            if Vector3.Distance2(MainPlayer:GetPos(), end_point) < 0.6 then
                if self.body and not self.body:AnimClipsEmpty(ChAnimType.Run2) then
                    self.body:PlayAnim(ChAnimType.Run2)
                elseif self.body then
                    self.body:PlayAnim(ChAnimType.Idle)
                end
                self:SetForward(end_point - MainPlayer:GetPos())
                end_func()
                return
            end
        end
        end_point.y = SceneCtrl.Instance:CurScene():GetPosHeight(end_point.x, end_point.z)
        path_poses[num] = end_point
    end
    for _, v in ipairs(path_poses) do
        table.insert(self.path_pos_list, v)
    end

    local i_holder = {index = 1}
    local move_ready = {}
    self.body:PlayAnim(ChAnimType.Run)
    move_ready.move_func = function(index_holder)
        if index_holder.index > #path_poses then
            -- 是否刹车，刹车后站立
            if self.body and not self.body:AnimClipsEmpty(ChAnimType.Run2) then
                self.body:PlayAnim(ChAnimType.Run2)
            elseif self.body then
                self.body:PlayAnim(ChAnimType.Idle)
            end
            if end_func ~= nil then
                end_func()--这里问题很多
            end
            if self.OnMovedEvent then
                self:OnMovedEvent()
            end
            return
        end
        local ne_p = path_poses[index_holder.index]
        -- Debuger.LogError("ne_p===%s,poses_count===%d,cur_idx===%d",tostring(ne_p),#path_poses,index_holder.index)
        if server_sync then --send move protocal
            if SceneCtrl.Instance:CurScene():IsTouchMoving() then
                if Time.realtimeSinceStartup - self.touch_began_time > 0.5 then
                    self.touch_began_time = Time.realtimeSinceStartup
                    SceneCtrl.Instance:MainChRequestMove(self:GetPos(), ne_p)
                end
            else
                SceneCtrl.Instance:MainChRequestMove(self:GetPos(), ne_p, true)
            end
        end
        local suc = self.nav_agent:SetDestination(ne_p)
        --[[ if suc == false then
            suc = self.nav_agent:SetDestination(GameScene.SamplePos(ne_p))
        end ]]
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

function SceneCh:PlayAnim(anim_name,anim_param, is_ride)
    if self.body and self.in_ride == false then
        self.body:PlayAnim(anim_name,anim_param, is_ride)
    end
end
function SceneCh:IsInAnim(anim_name)
    if self.body then
        return self.body:IsInAnim(anim_name)
    end
    return false
end

function SceneCh:GetMoveEndRealPos()
    return self.path_pos_list[#self.path_pos_list]
end

function SceneCh:GetPathPosList()
    return self.path_pos_list
end

function SceneCh:FlushIsLeader()
    if nil ~= self.vo and self.vo.is_leader == 1
    and self.root_node.activeSelf == true then
        if nil == self.LeaderObj then
            if not self.loading_lingpai then
                self.loading_lingpai = true
                ResMgr:LoadGameObject(
                    "actor/scene/lingpai.prefab",
                    function(obj)
                        if self.vo == nil then
                            return
                        end
                        self.LeaderObj = obj:Clone(self.root_node:GetParent())
                        local tbt = self.LeaderObj:GetComponent(typeof(Game.TransformByTarget))
                        local y = self.vo.title_id > 0 and 0.5 or 0
                        y = self.vo.SeatHeight and y + (self.vo.SeatHeight - 35) / 10 or y
                        tbt:SetTarget(self.root_node:FindChild("TopPoint").transform)
                        tbt.PositionOffset = Vector3.New(0, y, 0)
                        if self.vo.is_leader == 1 and self.vo.obj_type == SceneObjType.MainRole then--主角队长
                            self.LeaderObj:SetActive(true)
                        elseif self.vo.is_leader == 1 and TeamData.Instance:MemberInTeam(self.vo.name) then--我的队伍里的队长
                            self.LeaderObj:SetActive(true)
                        elseif self.vo.is_leader == 1 and not RoleSettingData.IsShield(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER) and self.root_node.activeSelf == true then--没被关掉的队长
                            self.LeaderObj:SetActive(true)
                        else
                            self.LeaderObj:SetActive(false)
                        end
                        self.loading_lingpai = false
                    end
                )
            end
        else
            local tbt = self.LeaderObj:GetComponent(typeof(Game.TransformByTarget))
            local y = self.vo.title_id > 0 and 0.5 or 0
            y = self.vo.SeatHeight and y + (self.vo.SeatHeight - 35) / 10 or y
            tbt.PositionOffset = Vector3.New(0, y, 0)
            self.LeaderObj:SetActive(true)
        end
    elseif nil ~= self.LeaderObj then
        self.LeaderObj:SetActive(false)
    end
end

function SceneCh:FlushTaskImg()
    local show_task_img = self:IsTaskNpc() and TaskCtrl.Instance:GetMainViewFuncImgByNpcId(self.vo.id, {})
    if show_task_img then
        if nil == self.TaskImgObj then
            if not self.loading_task_img then
                self.loading_task_img = true
                ResMgr:LoadGameObject(
                    "actor/ui/renwujuanzhou.prefab",
                    function(obj)
                        if self.vo == nil then
                            return
                        end
                        self.TaskImgObj = obj:Clone(self.root_node)
                        local tbt = self.TaskImgObj:GetComponent(typeof(Game.TransformByTarget))
                        tbt:SetTarget(self.root_node.transform)
                        if self.task_img_handle_runner then
                            Runner.Instance:RemoveRunUntilTrue(self.task_img_handle_runner)
                        end
                        self.task_img_handle_runner =
                            Runner.Instance:RunUntilTrue(function ()
                                if self.vo.body and self.vo.body.GetHeight and self.vo.body:GetHeight() then
                                    if tbt then
                                        local offset_y = 0
                                        local y = self.vo.body:GetHeight()
                                        y = y * (self.vo.res_size or 1)
                                        y = y + 0.4
                                        y = y < 2 and 2 or y
                                        y = y > 4.4 and 4.4 or y
                                        if self.vo.id and 43026 == self.vo.id then y = 6 end        -- 共工人高马大
                                        local npc_co = NpcHelper.NpcCo(self.vo.id, false) or {}
                                        offset_y = offset_y + y + (StringIsEmpty(npc_co.tit_name) and 0.5 or 1)
                                        tbt.PositionOffset = Vector3.New(0, offset_y, 0)
                                    end
                                    return true
                                end
                                return false
                            end)
                        self.TaskImgObj:SetActive(true)
                        self.loading_task_img = false
                    end
                )
            end
        else
            -- local tbt = self.TaskImgObj:GetComponent(typeof(Game.TransformByTarget))
            -- local y = self.vo.title_id > 0 and 0.5 or 0
            -- y = self.vo.SeatHeight and y + (self.vo.SeatHeight - 35) / 10 or y
            -- tbt.PositionOffset = Vector3.New(0, y, 0)
            self.TaskImgObj:SetActive(true)
        end
    elseif nil ~= self.TaskImgObj then
        self.TaskImgObj:SetActive(false)
    end
end

function SceneCh:FlushArenaMacth()
    if self.arena_match_show then
        self.arena_match_show:DeleteMe()
        self.arena_match_show = nil
    end
    if nil ~= self.vo then
        local amd = ArenaMatchData.Instance
        local arena_show_list = amd.arena_show_list
        local arena = amd:GetArenaListByNpcSeq(self.vo.seq)
        local info = arena_show_list[arena and arena.seq or -1]
        if nil == info then return end
        local res_id = info:ArenaMatchShow()
        self.arena_match_show = SceneObj.CreateObj({SceneObjType.Npc, res_id = res_id, nav_agent_block = true})
        self.arena_match_show:SetDirByEulerY(self.vo.dir_y)
        self.arena_match_show:SetScenePos(SceneData.Instance:SceneId(), self.vo.x, self.vo.y, false, SceneCtrl.Instance:CurScene():GetPosHeight(self.vo.x, self.vo.y) + ArenaMatchConfig.ArenaShowOffsetY[info:ArenaPosition()])
        self.arena_match_show:EnableShadow(true)
    end
end

-- instantiation end
function SceneCh:InitEnd()
    if not SceneConfig.RoleType[self:Type()] then
        self:PlayAnim(ChAnimType.Idle)
    end
    if self:Type() == SceneObjType.Role then
        --监听跟随宠物变化
        if not self.vo.ignor_follow then
            self.flush_follow_care1 = self.vo:Care(BindTool.Bind(self.FlushFollow, self), "is_in_team")
            self.flush_follow_care2 = self.vo:Care(BindTool.Bind(self.FlushFollow, self), "escort_id")
            self:FlushFollow()
        end
        --监听的时候是赋值后
    end
    if self:Type() == SceneObjType.Role or self:Type() == SceneObjType.MainRole then
        if not self.vo.ignor_follow then
            SceneCtrl.Instance.follow_data:FollowInit(self)
        end
    end
    if self:Type() == SceneObjType.Npc and DuChuangZeiYingData.Instance:IsNiuNiuInFB(self.vo) then
        TimeHelper:CancelTimer(self.timer_niuniu_fb)
        self.timer_niuniu_fb = TimeHelper:AddDelayTimer(function ()
            self.body:PlayAnim(ChAnimType.KunBang)
        end, 1)
    end
    if self:Type() == SceneObjType.Role and TombRaiderData.Instance:InTombRaider() then
        self.vo_hide_care = self:Vo():Care(BindTool.Bind(self.FlushRoleStatus, self), "move_state") 
        self:FlushRoleStatus()
    end
end
function SceneCh:FlushFollow()
    if self.vo == nil then
        return
    end
    if self.follow_pet then
        if self.follow_queue_id ~= nil then
            ChsFollow.Instance:OnChDelete(self.follow_pet)
        end
        self.follow_pet:DeleteMe()
        self.follow_pet = nil
    end
    if self.follow_escort_car then
        if self.follow_queue_id ~= nil then
            ChsFollow.Instance:OnChDelete(self.follow_escort_car)
        end
        self.follow_escort_car:DeleteMe()
        self.follow_escort_car = nil
    end
    if self.vo.is_in_team or (self:Type() == SceneObjType.MainRole and TeamData.Instance:InTeam()) then
        self.follow_queue_id = nil
        return
    end

    local info = self.vo.follow_pet
    local ch_queue = {}
    if self.follow_queue_id == nil then
        table.insert(ch_queue, self)
    end
    -- if not self.follow_npc and nil ~= info and 0 ~= info.pet_id then
    if nil ~= info and 0 ~= info.pet_id then
        local pet = Pet.New(info)
        self.follow_pet = SceneObj.CreateObj(
            {
                obj_type = SceneObjType.Pet,
                res_id = pet:ResId(),
                name = pet:Name(),
                pet_qua = pet:Quality(),
                fly_flag = pet:FeiShengTimes(),
                rank = pet:Rank()
            })
        self.follow_pet:Vo().move_speed = self.vo.move_speed
        self.follow_pet:SetMoveSpeed(self.vo.move_speed)
        table.insert(ch_queue, self.follow_pet)
        self:VisibilityJudge()
        if self.root_node.activeSelf == false then
            self.follow_pet.root_node:SetActive(false)
        end
        if OnlyFightData.Instance:IsInOnlyFight(true) then
            self:ShieldingPet(true)
        end
    end
    local escort_info = EscortCtrl.Instance:Data():GetEscortConfigBySeq(self.vo.escort_id)
    if self.vo and self.vo.escort_id and self.vo.escort_id > 0 and nil ~= escort_info then
        self.follow_escort_car = SceneObj.CreateObj({obj_type = SceneObjType.EscortCar, res_id = escort_info.res_id, name = escort_info.name})
        self.follow_escort_car:Vo().move_speed = self.vo.move_speed
        self.follow_escort_car:SetMoveSpeed(self.vo.move_speed)
        table.insert(ch_queue, self.follow_escort_car)
    end
    
    if self.follow_queue_id == nil then
        self.follow_queue_id = ChsFollow.Instance:CreateOrAddQueue(self.follow_queue_id, ch_queue)
    else
        for i,v in ipairs(ch_queue) do
            ChsFollow.Instance:AddChToQueue(self.follow_queue_id, v)
        end
    end
end

function SceneCh:AddAttachItem(vo)
    self.delayAttachItem =
        TimeHelper:AddDelayTimer(
        function()
            local attach_obj = AttachObj.New(self, vo)
            attach_obj:Init(vo)
            table.insert(self.attach_obj_list, attach_obj)
        end,
        1
    )
end

--是否屏蔽坐骑 true=屏蔽
function SceneCh:ShieldRide(hide)
    if self.body == nil or self.vo == nil or next(self.vo.appearance) == nil then
        return
	end
    self.shield_ride = hide or false
    local appearance = self.vo.appearance
    if appearance.ride_id > 0 and not self.shield_ride then
        self.body:SetRide(appearance.ride_id)
        self.in_ride = true
    else
        self.in_ride = false
        self.body:SetRide(0)
    end
end

--处理跟随逻辑
--[[ function SceneCh:Follow()
    if self:IsOtherType() then
        return
    end
    if self.follow_pet and self:IsInAnim(ChAnimType.Run) then
        local petPos = self.follow_pet:GetPos()
        local pos = self:GetPos()
        local distance = GameMath.GetDistance(pos.x, pos.z, petPos.x, petPos.z, true)
        local v2 = GameMath.DistancePos(petPos.x, petPos.z, pos.x, pos.z, 2)
        if distance > 10 then
            self.follow_pet:SetPos(pos)
        elseif distance > 4 then
            local y = SceneCtrl.Instance:CurScene():GetPosHeight(v2.x, v2.y)
            self.follow_pet:MoveTo(Vector3.New(v2.x, y, v2.y), false)
        else
            if distance <= 1 then
                self.follow_pet:StopMove()
            end
        end
    end
    if self.follow_pet then
        local petPos = self.follow_pet:GetPos()
        local pos = self:GetPos()
        if GameMath.GetDistance(pos.x, pos.z, petPos.x, petPos.z, true) > 10 then
            local obj_for = Quaternion.AngleAxis(self.vo.dir_y or 45, Vector3.up) * Vector3.forward
            self.follow_pet:SetPos(self:GetPos() - obj_for * 2)
            self.follow_pet:StopMove()
        end
    end
end

function SceneCh:Update()
    self:Follow()
end
function SceneCh:IsOtherType()
    return self:Type() ~= SceneObjType.Role and self:Type() ~= SceneObjType.MainRole
end ]]


function SceneCh:PlayGhost()
    if self.body == nil then return end
    local com = self.body:GhostCom()
    if com ~= nil then
        com:Play()
    end
end

function SceneCh:StopGhost()
    if self.body == nil then return end
    local com = self.body:GhostCom()
    if com ~= nil then
        xpcall(
            function()
                com:Stop()
            end,
            function(err)
                Debuger.LogError("MainCh:StopGhost ERROR:" .. tostring(err))
            end
        )
    end
end


function SceneCh:HyperMove(x,y)
    local cur_pos = self:GetPos()
    local pos = SceneCtrl.Instance:CurScene():PosToClint(x, y)
    local real_distance = GameMath.GetDistance(cur_pos.x, cur_pos.z, pos.x, pos.z, true)  
    local move_dir = self.root_node:GetEulerAngles().y * (math.pi / 180)
    self:PlayGhost()

    self.is_hyping = true
    local timer = 1
    self.hyper_timer = TimeHelper:AddRunFrameTimer(
        function()
            local pos_x = self:GetPos().x + (math.sin(move_dir) * (real_distance/5))
            local pos_z = self:GetPos().z + (math.cos(move_dir) * (real_distance/5))

            self:SetScenePos(SceneData.Instance:SceneId(),pos_x,pos_z,false)
            if timer == 5 then 
                self:SetScenePos(SceneData.Instance:SceneId(),pos.x,pos.z,false)
                self:StopGhost()
                self.is_hyping = false

                if self.hype_delay_func ~= nil then 
                    self.hype_delay_func()
                    self.hype_delay_func = nil
                end 
            else 
                timer = timer + 1
            end 
        end, 4, 20)
end

function SceneCh:SetHyperDelayFun(func)
    self.hype_delay_func = func
end 

function SceneCh:FlushRoleStatus()
    if self.vo == nil then return end 
    if self.vo.move_state == 0 then 
        self:HideRoleShow(HIDETYPE.ShowBack)
    elseif self.vo.move_state == 1 then 
        local type = TombRaiderData.Instance:CheckIsMyTeam(self.vo.role_id) and HIDETYPE.HideSelf or HIDETYPE.HideEnemy
        self:HideRoleShow(type)
    end 
end

-- 简单的人物透明，“隐形”，恢复
function SceneCh:HideRoleShow(type,special)
    if self:Type() ~= SceneObjType.Role then 
        return 
    end 
    if special == nil and not TombRaiderData.Instance:InTombRaider() then 
        return 
    end 
    if self.body == nil then 
        self.wait_flush_role_status = true
        return 
    end 
    if type == HIDETYPE.ShowBack then 
        if self.is_hyping == true then 
            self:SetHyperDelayFun(function()
                self:HideRoleShow(HIDETYPE.ShowBack)
            end)
            return 
        end 
        self.body:SetMain(SceneObj.EmptyResId, nil, true)
        self.body:SetWeapon(0, 0)
        -- self:SetTouchAble(true)
        -- self:EnableShadow(true)
        -- self:HideFlyEffect(false)

        self:SetVisible(true)
        self:FlushRoleDrawer()
        if MainViewData.Instance:SceneObjVos()[self.vo] == nil then 
            MainViewData.Instance:AddObjVo(self.vo)
        end 
        -- if self.vo.is_leader == 1 and nil ~= self.LeaderObj then self.LeaderObj:SetActive(true) end 
    elseif type == HIDETYPE.HideSelf then 
        self.body:SetRendererMaterial(alpha_mat, 0)
    elseif type == HIDETYPE.HideEnemy then 
        -- self.body:SetMain(SceneObj.EmptyResId, nil, true)
        -- self.body:SetWeapon(0, 0)
        -- self:SetTouchAble(false)
        -- self:EnableShadow(false)
        -- self:HideFlyEffect(true)
        MainViewData.Instance:RemoveObjVo(self.vo)
        self:SetVisible(false) -- 本方法即可完成整个角色的不可视，其余暂时注释
        -- if nil ~= self.LeaderObj then self.LeaderObj:SetActive(false) end 
    end
    self.wait_flush_role_status = false
end

function SceneCh:SetWeaponShow(is_show)
    self.body:SetWeaponShow(is_show)
end