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

PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        ResMgr:LoadObjectOfType(
            "material/Dodge.mat",
            typeof(UnityEngine.Material),
            function(obj)
                PreloadCtrl.dodge_mat = obj
                PreloadCtrl.UnregisterLoading()
            end
        )
    end
)

MainCh = MainCh or BaseClass(SceneCh)
MainPlayer = nil

function MainCh:__init()
    self.handle_drawer = RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.FlushRoleDrawer, self), "appearance")
    self.handle_leader = RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.FlushIsLeader, self), "title_id", "SeatHeight","facescroe_id")
    self.handle_follow = RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.FlushFollow, self), "is_in_team")
    self.handle_escort = RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.FlushFollow, self), "escort_id")
    self.handle_task = TaskData.Instance.task_list:Care(BindTool.Bind(self.FlushNPCFollow, self))
    -- self.listener = nil
    Runner.Instance:AddRunObj(self)
    self.run_audio_handle = nil
    self.cur_run_audio = nil
end

function MainCh:__delete()
    self:CleanRunAudio()
    Runner.Instance:RemoveRunObj(self)

    RoleData.Instance:GetBaseData():Uncare(self.handle_drawer)
    RoleData.Instance:GetBaseData():Uncare(self.handle_leader)
    RoleData.Instance:GetBaseData():Uncare(self.handle_follow)
    RoleData.Instance:GetBaseData():Uncare(self.handle_escort)
    TaskData.Instance.task_list:Uncare(self.handle_task)

    if MainPlayer == self then
        MainPlayer = nil
    end
    AudioMgr:AddAudioListener()
    -- if self.listener ~= nil then
    --     UnityGameObj.Destroy(self.listener)
    --     self.listener = nil
    -- end
    if self.follow_npc then
        self.follow_npc:DeleteMe()
        self.follow_npc = nil
    end
end

function MainCh:Init(vo)
    SceneCh.Init(self, vo)
    if MainPlayer == nil then
        MainPlayer = self
    end
    AudioMgr:AddAudioListener(self.root_node)
    -- self.listener = self.root_node:AddComponent(typeof(UnityEngine.AudioListener))
end

function MainCh:SetVisible(b)
    SceneCh.SetVisible(self,b)
    AudioMgr:OnListenerEnable(b)
end

function MainCh:BindVirtualCamera(virCam, set_yAxis)
    virCam.Follow = self.root_node.transform
    if set_yAxis and IS_AUDIT_VERSION and VersionTag.HasTag(VersionTag.AxisState) and GLOBAL_CONFIG:Urls().cam_axis_y ~= nil then
        local y_axis = virCam.m_YAxis
        y_axis.Value = GLOBAL_CONFIG:Urls().cam_axis_y
        virCam.m_YAxis = y_axis
        local flMgr = virCam.gameObject:GetComponent(typeof(Game.FreeLookCameraMgr))
        flMgr:SetBlock(true)
    end
    self.root_node.tag = "MainRole"
end

function MainCh:FlushRoleDrawer()
    SceneCh.FlushRoleDrawer(self)
    Runner.Instance:RunUntilTrue(
        function()
            if self.body and not self.body:IsLoading() and xray_mat then
                self.body:SetRendererMaterial(xray_mat, Game.RendererMaterialLayer.XRay:ToInt())
                return true
            end
        end
    )
    if self.cur_run_audio ~= nil and self.cur_run_audio ~= self:GetRunAudioName() then
        self:CleanRunAudio()
    end
end

function MainCh:FlushNPCFollow()
    if self.vo == nil then return end
    local main_task = TaskData.Instance:GetMainlineAccepted()
    if not main_task then
        if self.follow_npc then
            self.follow_npc:DeleteMe()
            self.follow_npc = nil
        end
        if self.cache_npc_seq then
            VoMgr:AddNpc(self.cache_npc_seq)
            self.cache_npc_seq = nil
        end
        return
    end
    local task_id = TaskData.Instance:GetMainlineAccepted().task_id
    local npc_seq = Config.tasklist_auto.task_list[task_id].follow_npc_seq
    if npc_seq == "" then
        if self.follow_npc then
            self.follow_npc:DeleteMe()
            self.follow_npc = nil
        end
        if self.cache_npc_seq then
            VoMgr:AddNpc(self.cache_npc_seq)
            self.cache_npc_seq = nil
        end
        return
    end
    local npc_dyn_cfg = Cfg.NpcBySeq(npc_seq)
    if self.follow_npc_id and self.follow_npc_id == npc_dyn_cfg.id then
        return
    end
    self.follow_npc_id = npc_dyn_cfg.id
    local npc_cfg = Cfg.NpcById(npc_dyn_cfg.id)
    if VoMgr:NpcVoList()[npc_seq] then
        VoMgr:DeleteNpc(npc_seq)
        self.cache_npc_seq = npc_seq
    end
    self.follow_npc = SceneObj.CreateObj(
        {
            obj_type = SceneObjType.Pet,
            res_id = npc_cfg.res_id,
            name = npc_cfg.name,
        })
    self.follow_npc:Vo().move_speed = self.vo.move_speed
    self.follow_npc:SetMoveSpeed(self.vo.move_speed)
    if self.follow_queue_id then
        --[[ self.follow_queue_id =  ]]ChsFollow.Instance:AddChToQueue(self.follow_queue_id, self.follow_npc)
    else
        self.follow_queue_id = ChsFollow.Instance:CreateOrAddQueue(self.follow_queue_id, {self, self.follow_npc})
    end
end

function MainCh:Type()
    return SceneObjType.MainRole
end
function MainCh:CreateDrawer()
    return SceneChDrawer.New(self.root_node, self.vo,nil,true)
end

function MainCh:FlushDrawer()
end
function MainCh:GetDrawerData()
end

function MainCh:SetDirY(dir_y)
    self.root_node.transform.rotation = Quaternion.AngleAxis(dir_y, Vector3.up)
end

function MainCh:TestSetMain(res_id)
    -- local main_path = res_id ~= nil and string.format("actor/role/%s.prefab",res_id) or nil
    self.body:SetMain(res_id)
end

function MainCh:TestSetWing(res_id)
    -- local wing_path = res_id ~= nil and string.format("actor/wing/%s.prefab",res_id) or nil
    self.body:SetWing(res_id)
end

function MainCh:TestSetRide(res_id)
    -- local ride_path = res_id ~= nil and string.format("actor/ride/%s.prefab",res_id) or nil
    self.body:SetRide(res_id)
end

function MainCh:TestSetWeapon(r_weapon_id, l_weapon_id)
    self.body:SetWeapon(r_weapon_id, l_weapon_id)
end

function MainCh:TestCancel()
    self.body:SetRide(nil)
    self.body:SetWeapon(nil)
    self.body:SetShengQi(nil)
end

function MainCh:SetShengQi(sheng_qi)
    self.body:SetShengQi(sheng_qi)
end

function MainCh:GetDistance(pos)
    return GameMath.GetDistance(pos.x, pos.z, self:GetPos().x, self:GetPos().z, false)
end

function MainCh:FlushRoleStatus(flag)
    -- if DEBUG_MODE then 
    --     LogDG("?看看",self.vo.role_name,"他居然",(self.vo.move_state == 0 and "正常了" or "隐身了"))
    -- end
    if flag == 0 then 
        self:HideRoleShow(HIDETYPE.ShowBack)
    elseif flag == 1 then 
        self:HideRoleShow(HIDETYPE.HideSelf)
    end 
end

function MainCh:HideRoleShow(type)
    
    if self:Type() ~= SceneObjType.MainRole then 
        return 
    end 
    if not TombRaiderData.Instance:InTombRaider() then 
        return 
    end 
    if type == HIDETYPE.ShowBack then 
        self.body:SetMain(SceneObj.EmptyResId, nil, true)
        self.body:SetWeapon(0, 0)

        self:SetVisible(true)
        self:FlushRoleDrawer()
        -- self:EnableShadow(true)
    elseif type == HIDETYPE.HideSelf then 
        self.body:SetRendererMaterial(alpha_mat, 0)
    elseif type == HIDETYPE.HideEnemy then 
        -- self.body:SetMain(SceneObj.EmptyResId, nil, true)
        -- self.body:SetWeapon(0, 0)
        -- self:EnableShadow(false)
        self:SetVisible(false) -- 本方法即可完成整个角色的不可视，其余暂时注释
    end 
end

function MainCh:PlayGhost()
    local com = self.body:GhostCom()
    if com ~= nil then
        com:Play()
    end
end

function MainCh:StopGhost()
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

function MainCh:HyperMove(distance,func)
    local move_dir = self.root_node:GetEulerAngles().y * (math.pi / 180)
    local end_x = self:GetPos().x + (math.sin(move_dir) * distance)
    local end_z = self:GetPos().z + (math.cos(move_dir) * distance)

    local pos_x = 0
    local pos_z = 0
    local flag = true
    for i = 1,5 do 
        pos_x = self:GetPos().x + (math.sin(move_dir) * ((distance/5) * i))
        pos_z = self:GetPos().z + (math.cos(move_dir) * ((distance/5) * i))
        local flag_pos = self.nav_agent:CalculatePath(Vector3.New(pos_x, SceneCtrl.Instance:CurScene():GetPosHeight(pos_x, pos_z),pos_z), self.nav_mesh_path)
        flag = flag_pos and flag
    end 

    if not flag then 
        return 
    end 

    self:PlayGhost()
    -- self.hyper_end_timer = TimeHelper:AddDelayFrameTimer(function ()
    --     MainPlayer:MoveTo(Vector3.New(end_x, SceneCtrl.Instance:CurScene():GetPosHeight(end_x, end_z),end_z),true)
    -- end,20)

    local timer = 1
    self.hyper_timer = TimeHelper:AddRunFrameTimer(
        function()
            local pos_x = self:GetPos().x + (math.sin(move_dir) * (distance/5))
            local pos_z = self:GetPos().z + (math.cos(move_dir) * (distance/5))
            self:SetScenePos(SceneData.Instance:SceneId(),pos_x,pos_z,false)
            if timer == 5 then 
                self:StopGhost()
                if func ~= nil then func() end 
            else 
                timer = timer + 1
            end 
        end, 4, 20)
end 

function MainCh:Update()
    self:CheckRunAudio()
end

local ride_audio_map = {
    [0] = "",
    [1] = "zuo_qi_ben_pao",
    [2] = "zuo_qi_fei_xing",
}

function MainCh:GetRunAudioName()
    local run_audio = "jue_se_ben_pao"
    local appearance = self.vo.appearance
    if appearance ~= nil and (appearance.ride_id or appearance.bicycle_index) and not self.shield_ride then
        if appearance.ride_id > 0 then      --幻化坐骑
            local sur_info = Cfg.SurfaceInfoByResId(appearance.ride_id,false)
            if sur_info ~= nil then
                run_audio = ride_audio_map[sur_info.run_audio]
            end
        elseif appearance.bicycle_index >= 0 then -- 灵骑
            local bicycle = BicycleData.Instance:GetBicycleByIndex(appearance.bicycle_index)
            local ride_res_id = bicycle:ModelId(appearance.bicycle_model_type)
            local bic_info = Cfg.BicycleBaseInfoByModelId(ride_res_id)
            if bic_info ~= nil then
                run_audio = ride_audio_map[bic_info.run_audio or 0]
            end
        end
    end
    return run_audio 
end

function MainCh:CleanRunAudio()
    if self.run_audio_handle ~= nil then
        if type(self.run_audio_handle) ~= "table" then
            AudioMgr:StopEffectHandle(self.run_audio_handle)
        end
        self.cur_run_audio = nil
        self.run_audio_handle = nil            
    end
end

function MainCh:CheckRunAudio()
    if self.body == nil then return end
    local is_in_run = self.body:IsInAnim(ChAnimType.Run)
    if is_in_run then
        if self.run_audio_handle == nil then
            --不同坐骑读不同的配置
            local run_audio = self:GetRunAudioName()
            self.cur_run_audio = run_audio
            if StringIsEmpty(run_audio) then
                self.run_audio_handle = {}
            else
                self.run_audio_handle = AudioMgr:PlayEffectHandle(AudioType.Scene,run_audio)
            end
        end
    else
        self:CleanRunAudio()
    end
end

--服务端强制移动位置
function MainCh:ForceMove(protocol)
    local end_x = protocol.pos_x
    local end_y = protocol.pos_y
    local distance = protocol.distance
    if distance ~= 0 then
        end_x = end_x + (math.cos(protocol.dir_y) * distance)
        end_y = end_y + (math.sin(protocol.dir_y) * distance)
    end
    if SceneCtrl.Instance:CurScene():LoadingComplete() and SceneCtrl.Instance:CurScene():GetClassType() == GameScene and distance ~= 0 then
        local pos = SceneCtrl.Instance:CurScene():PosToClint(end_x, end_y)
        self:MoveTo(pos, false)
    end
    self:SetVoPos(end_x, end_y)	--vo数据随时都可以进行设置，与前端场景状态无关
end

--变成婚宴佬
function MainCh:BecomeWeddingman(is_host)
    local appearance = self.vo.appearance
    if is_host then
        self.body:SetMain(ResHelper.RoleResId(appearance.avatar_type, MarryConfig.FASHION_BODY_ID, appearance.advance_time))
    end
    self.body:SetWeapon(0, 0)
    self.in_ride = false
    self.body:SetRide(0)
end

--变成钓鱼佬
function MainCh:BecomeFisherman()
    local appearance = self.vo.appearance
    self.body:SetMain(ResHelper.Fisherman(appearance.avatar_type), appearance.color)
    self.body:SetWeapon(0, 0)
    self.in_ride = false
    self.body:SetRide(0)
    self.body:SetFishRod("yugan", appearance.avatar_type)
end
