local tianyan_mat
PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        ResMgr:LoadObjectOfType(
            "material/tianyan_001.mat",
            typeof(UnityEngine.Material),
            function(obj)
                tianyan_mat = obj
                PreloadCtrl.UnregisterLoading()
            end
        )
    end
)
--===========================SceneObj场景对象基类===========================
SceneObj = SceneObj or BaseClass()

function SceneObj.CreateObj(vo)
    local NewType = SceneConfig.NewType
    local ch = (GetView(NewType[vo.obj_type]) or SceneCh).New()
    ch:Init(vo)
    if SceneConfig.NotTouchType[vo.obj_type] then
        ch:SetTouchAble(false)
    end
    if SceneConfig.NotShadowType[vo.obj_type] then
        ch:EnableShadow(false)
    end
    ch:InitEnd()
    return ch
end

function SceneObj:__init()
    self.cache_pos_scene_id = 0
    self.cache_pos_x = 0
    self.cache_pos_y = 0
    self.effect_list = {}
end

function SceneObj:Init(vo)
    self.vo = vo
    self.root_node = GameObjPool:Get("actor/__ActorRoot.prefab")
    self.vo.root_obj = self.root_node
    MainViewData.Instance:AddObjVo(self.vo)
    self.nav_agent = self.root_node:GetComponent(typeof(UnityAI.NavMeshAgent))
    self.nav_agent_enabled = false
    self.effect_buff_player = {}

    self:AddBindListener()
    self:InitFlushDrawer()
    self:SetDirByEulerY()
    self:FlushRootObjName()
end

function SceneObj:InitFlushDrawer()
    self.body = self:CreateDrawer()
    if self.body:IsRoleType() then
        self:FlushRoleDrawer()
    else
        self:FlushDrawer()
    end
    self.vo.body = self.body
    self:ShowEffect()
end
function SceneObj:HenshinMonkey()
    if self.vo == nil then
        return
    end
    if self.vo.appearance.henshin_monkey == nil then
        self.vo.appearance.henshin_monkey = true
        self:FlushRoleDrawer()
    else
        self.vo.appearance.henshin_monkey = nil
        self:FlushRoleDrawer()
    end
end
function SceneObj:FlushRoleDrawer()
    if self.body == nil or self.vo == nil or next(self.vo.appearance) == nil then
        return
	end
    if self:InTheFishingScene() then
        return
    end
    if self:InTheWeddingScene() then
        return
    end
    local appearance = self.vo.appearance
    if appearance.special_appearance_type and appearance.special_appearance_type == 1 and appearance.special_appearance_param < 999 then
        local config = PartnerData.Instance:GetPartnerInfoById(appearance.special_appearance_param)
        if config then
            self.body:SetMain(config.info.res_id, nil, true)
        end
    elseif SceneData.Instance:GetMode() == SceneModeType.LoversMatch then
        local spe_res_id = LoversMatchData.Instance:GetRoleFormResId(self:Vo())
        if spe_res_id then
            self.body:SetMain(spe_res_id, nil, true)
        else
            self.body:SetMain(ResHelper.RoleResId(appearance.avatar_type, appearance.fashion_body_id, appearance.advance_time), appearance.fashion_body_id > 0 and 0 or appearance.color)
        end
    else
        if appearance.henshin_monkey == true then
            self.body:SetMain(nil, nil, true)
        else
            local card_model_res_id = LiveData.Instance:CardModel(appearance)
            if card_model_res_id then
                self.body:SetMain(card_model_res_id, nil, true)
            else
                self.body:SetMain(ResHelper.RoleResId(appearance.avatar_type, appearance.fashion_body_id, appearance.advance_time), appearance.fashion_body_id > 0 and 0 or appearance.color)
            end
        end
    end
    self.body:SetLoadedCallback(function()
        if self.vo == nil then
            return
        end
        if not StringIsEmpty(self.vo.res_size) then
            self.body:SetScale(self.vo.res_size)
        end
        if self:CanEnableYinYangEye() then
            self:EnableYinYangEye()
        end
        self:BodyLoadedCallback()           
    end)
    self:VisibilityJudge()
end

function SceneObj:FlushDrawer()
    local config = SceneObjData.GetDrawerCfg(self.vo)
    local path = SceneObjData.GetDrawerPath(self.vo or {},config or {})

    if config and config.res_id then
        if config.res_id == SceneConfig.EmptyResId then
            self:SetTouchAble(false)
        else
            self.vo.res_id = config.res_id
        end
        if config.is_show == 0 and self.body then
            self.body:DeleteShadow()
        end
    end
    self.body:SetData(path)
    self.body:SetLoadedCallback(function()
        if self.vo == nil then
            return
        end
        if config and config.res_size and config.res_size ~= "" then
            self.body:SetScale(config.res_size)
            self.vo.res_size = config.res_size ~= "" and config.res_size or 1
        else
            self.vo.res_size = 1
        end
        local is_anecdotes, task_info = self:IsAnecdoteObj()
        if is_anecdotes and task_info then
            if task_info.info.is_yinyang == 1 then
                self:SetVisible(GuideData.Instance.yinyang_eye_open)
            end
        end
        if config and config.res_id ~= SceneConfig.EmptyResId and self:CanEnableYinYangEye() then
            self:EnableYinYangEye()
        end
       self:BodyLoadedCallback()
    end)        
end

function SceneObj:CreateDrawer()
    if self.vo.name == Language.Common.XinMo then
        self.vo.temp_obj_type = SceneObjType.Role
        self.vo.appearance = DataHelper.TableCopy(MainPlayer:Vo().appearance)
        self.vo.appearance.fashion_body_id = 0
        self.vo.appearance.advance_time = 1
        self.vo.appearance.color = 100
        self.vo.appearance.weapon_id = 0
    end
    if SceneConfig.RoleType[self:Type()] or self.vo.temp_obj_type then
        return SceneChDrawer.New(self.root_node, self.vo)
    end
    return SceneObjDrawer.New(self.root_node, self:Type() ~= SceneObjType.Door)
end

function SceneObj:AddBindListener()
    self.pos_data_care_handle = SceneMgr:CareLoadedSceneIdChange(BindTool.Bind(self.OnLoadedSceneIdChange, self))
    if self:Type() == SceneObjType.Role then
        self.vo_sm_care = self:Vo():Care(BindTool.Bind(self.FlushRoleDrawer, self), "appearance")
        self.vo_sm_care_is_leader = self:Vo():Care(BindTool.Bind(self.FlushIsLeader, self), "is_leader", "SeatHeight")
        self:FlushIsLeader()
    elseif self:Type() == SceneObjType.Statue then
        self.vo.appearance.ride_id = 0
        self.vo.appearance.fashion_body_id = 0
        self.vo_sm_care = self:Vo():Care(BindTool.Bind(self.FlushRoleDrawer, self), "appearance")
    else
        if self:IsTaskNpc() then
            self.task_sm_care_task_img = TaskData.Instance:GetTaskList():Care(BindTool.Bind(self.FlushTaskImg, self))
            self.role_sm_care_task_img = RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.FlushTaskImg, self), "level")
            self:FlushTaskImg()
        end
        if ArenaMatchData.Instance:GetArenaPosSceneInfo(SceneData.Instance:SceneId()) and ArenaMatchConfig.ArenaMatchIds[self.vo.id] then
            self:FlushArenaMacth()
            self.arena_match_sm_care = ArenaMatchData.Instance.arena_show_list:Care(BindTool.Bind(self.FlushArenaMacth, self))
        end
    end
end

function SceneObj:ShowEffect()
    if self.vo then
        if self:Type() == SceneObjType.Npc then
            if self.vo.seq == Config.smxj_story_auto.other[1].npc_id then
                SE:Play(SceneEffectConfig.EffectList.StarLifeEffect3, self:GetPos(), function(obj, key, time)
                    UH.LocalPosG(obj, Vector3.New(0, 1.5, 0))
                    table.insert(self.effect_list, obj)
                end, self.root_node)
            end
        elseif self:Type() == SceneObjType.Pet then
            if self.body:IsLoading() then
                self.cache_show_fly_effect = true
            else
                self:SetFlyEffect()
            end
        end
    end
end

--模型点击操作 可重写
function SceneObj:InteractionBehavior()
    if self:Type() == SceneObjType.Npc then
        local list = TaskData.Instance:AcceptedTask()
        local scene_id = SceneData.Instance:SceneId()
        if list[MAIN_LINE_SP_TASK] == 1 and scene_id == MAIN_LINE_SP_MAP then
            local main = TaskData.Instance:GetMainlineAccepted()
            if main and not StringIsEmpty(main.info.tasks_dialog_beh) then
                local params = string.split(main.info.tasks_dialog_beh, "|")
                local npc_seq = tonumber(params[5]) or 0
                if npc_seq == tonumber(self.vo.seq) then
                    main:GoTask()
                    return
                end
            end
        end
        if SceneData.Instance:GetMode() == SceneModeType.SplaceAnecdote or SceneData.Instance:GetMode() == SceneModeType.SoulsAnecdote then
            SplaceAnecdoteData.Instance:SetOperateNpc(self.vo.obj_id)
        end 
    end
    if self.vo then
        BehaviorCtrl.ExeByVo(self.vo)
    end
end

function SceneObj:CenterPoint()
    if self.center_point == nil then
        self.center_point = self.root_node:FindChild("CenterPoint")
    end
    return self.center_point
end

function SceneObj:clearCenterPointFollow()
    if self.center_point then
        self:CenterPoint():SetLocalPosition(Vector3.New(0,1.25,0))
    end
    if self.center_point_follow then
        self.center_point_follow:SetTarget(nil)
        self.center_point_follow.PositionOffset = Vector3.zero
    end
end

--必须在main_body load完成后刷新
function SceneObj:FlushCenterPointFollow()
    if self.center_point_follow == nil then
        self.center_point_follow = self:CenterPoint():GetComponent(typeof(Game.TransformByTarget))
    end
    if (not self.body:IsLoading()) and self.body.main_body:getObj() ~= nil then
        if self.body.main_body then
            self.center_point_follow:SetTarget(self.body.main_body:getObj().transform)
        end
        if self.body.GetHeight and self.body:GetHeight() then
            self.center_point_follow.PositionOffset = self.center_point_offset_cache == nil and Vector3.New(0, self.body:GetHeight() / 2, 0) or Vector3.zero
        end
    end
end

function SceneObj:VisibilityJudge()
    -- 保证在同屏人数之下
    if self.vo and self.vo.obj_type == SceneObjType.Role then
        if RoleSettingData.IsShield(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER) then
            if TeamData.Instance:MemberInTeam(self.vo.name) then
                self.root_node:SetActive(true)
            else
                self.root_node:SetActive(false)
            end
            if self.follow_pet then
                self.follow_pet.root_node:SetActive(false)
            end
        end
    end
end

function SceneObj:FlushRootObjName()
    if IS_EDITOR then
        self.root_node.name = self.vo.name and string.format("%s[%d]", self.vo.name, self:ObjId()) or tostring(self:ObjId())
    end
end

function SceneObj:__delete()
    local tbt = self.root_node:GetComponent(typeof(Game.TransformByTarget))
    tbt:SetTarget(nil)
    tbt.enabled = false
    if self.effect_buff_player then
        for asset_name, player in pairs(self.effect_buff_player) do
            if player then
                GameBuffManager:RemovePlayer(player, true)
            end
        end
    end
    self.effect_buff_player = {}
    TimeHelper:CancelTimer(self.free_time_anim)
    self.last_check_pos = nil
    TimeHelper:CancelTimer(self.range_invoke_timer)
    ChsFollow.Instance:OnChDelete(self)

    if self.pos_data_care_handle ~= nil then
        SceneMgr:UncareLoadedSceneIdChange(self.pos_data_care_handle)
        self.pos_data_care_handle = nil
    end
    self:clearCenterPointFollow()
    self.center_point_follow = nil
    -- if self.collider ~= nil then
    --     self.collider.enabled = true
    -- end
    for k, v in pairs(self.effect_list) do
        SE:Stop(v)
    end
    if self.root_node ~= nil then
        if self.nav_agent_enabled == true then
            self.nav_agent.enabled = false
            self.nav_agent_enabled = false
        end
        local release_func = function ()
            if self.collider ~= nil then
                self.collider.enabled = true
            end
            if self.body ~= nil then
                self.body:DeleteMe()
                self.body = nil
            end
            GameObjPool:Release(self.root_node)
            --self.root_node = nil
        end

        if GpQualityMgr:IsLowMemDevice() then
            release_func()
        else
            TimeHelper:AddDelayTimer(release_func, 0.1)
        end
    end
    if self.arena_match_sm_care then
        ArenaMatchData.Instance.arena_show_list:Uncare(self.arena_match_sm_care)
        self.arena_match_sm_care = nil
    end
    if self.task_sm_care_task_img then
        TaskData.Instance:GetTaskList():Uncare(self.task_sm_care_task_img)
        self.task_sm_care_task_img = nil
    end
    if self.role_sm_care_task_img then
        RoleData.Instance:GetBaseData():Uncare(self.role_sm_care_task_img)
        self.role_sm_care_task_img = nil
    end
    if self.task_img_handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.task_img_handle_runner)
        self.task_img_handle_runner = nil
    end    
    if self.vo ~= nil then
        if self.vo.Val then
            self.vo:Val().root_obj = nil
        else
            self.vo.root_obj = nil
        end
        if self.vo_sm_care then
            self:Vo():Uncare(self.vo_sm_care)
            self.vo_sm_care = nil
        end
        if self.vo_sm_care_is_leader then
            self:Vo():Uncare(self.vo_sm_care_is_leader)
            self.vo_sm_care_is_leader = nil
        end
        if self.vo_hide_care then 
            self:Vo():Uncare(self.vo_hide_care)
            self.vo_hide_care = nil
        end 
        self.vo.body = nil
        MainViewData.Instance:RemoveObjVo(self.vo)
    end
    self.vo = nil
    SE:Stop(self.fly_eff_handle)
    self.cache_fly_flag = nil
    self.cache_marbas_flag = nil
end

function SceneObj:CanEnableYinYangEye()
    if GuideData.Instance.yinyang_eye_open == false or 
    GuideData.Instance.yinyang_eye_effect.id == 0 or 
    GuideData.Instance.yinyang_eye_effect.id2 == 0 then
        return false
    end
    if self.vo and self.vo.id and (self.vo.id == 59053 or self.vo.id == 59046) then
        return false
    end
    if self.vo and self.vo.not_affect_yinyang == true then
        return false
    end
    return true
end

function SceneObj:BodyLoadedCallback()
    if self.cache_show_fly_effect then
        self:SetFlyEffect()
        self.cache_show_fly_effect = false
    end
    self:FlushCenterPointFollow()
    if self.vo.obj_type == SceneObjType.Npc
    or self.vo.obj_type == SceneObjType.Monster then
        local config = NpcDialogData.GetNpcConfig(self.vo)
        self.vo.npc_type = config.npc_type
        self:CheckHillSea()
        self:CheckHarmonyHeaven()
        self:CheckSpecials()
    end
    if FishData.Instance:InFishMap() and self.vo.obj_type == SceneObjType.Role then
        self.body:SetFishRod("yugan", self.vo.appearance.avatar_type)
    end
    if self.vo.obj_type == SceneObjType.Role or self.vo.obj_type == SceneObjType.MainRole then
        self:FreeTimeAnim()
    end
    if self.vo.obj_type == SceneObjType.Role and TombRaiderData.Instance:InTombRaider() then
        if self.wait_flush_role_status then 
            self:FlushRoleStatus()
        end 
    end
    if self.vo.obj_type == SceneObjType.ShengQi then
        local tbt = self.root_node:GetComponent(typeof(Game.TransformByTarget))
        tbt.SyncPosition = false
        tbt.SyncRotation = true
        tbt:SetTarget(UnityGameObj.FindGameObjectWithTag("BattleEnterBefore").transform)
        tbt.enabled = true
        self.body.main_body:getObj():SetLocalPosition(Vector3.New(0.5,0.5,1))--(-0.5,2.5,-0.5))
        self.body.main_body:getObj():SetEulerAngles(Vector3.New(0,-45,0))
    end
    if self.vo.obj_type == SceneObjType.PalaceStatus then
        local height, _ = SceneCtrl.Instance:CurScene():GetPosHeight(self.vo.x, self.vo.y)
		height = height + 0.8
        local pos = Vector3.New(self.vo.x, height, self.vo.y)
		self:ResetPos(pos)
        -- local v3 = self.root_node:GetLocalPosition()
        -- v3.y = v3.y + 0.8
        -- self.root_node:SetLocalPosition(v3)
    end
end

function SceneObj:CheckHillSea()
    if self.vo.npc_type == 1100 then
        local name = FightFieldBossData.Instance:GetHillSeaName(self.vo.id)
        if name and name ~= "" then
            local grade_type = FightFieldBossData.Instance:GetGetHillSeaType(self.vo.id)
            local effect_handle = SE:Play(SceneEffectConfig.EffectList["HillSea"..grade_type],Vector3.zero,nil,self.root_node)
            table.insert(self.effect_list, effect_handle)
        end
    elseif self.vo.npc_type == 1200 then --北斗七星
        local start_main_cfg = Cfg.BigDipperStarManByNpcSeq(self.vo.id, false)
        if start_main_cfg ~= nil then
            local star_main_type = start_main_cfg.type
            local grade_type = star_main_type == 1 and "3" or "2"
            local effect_handle = SE:Play(SceneEffectConfig.EffectList["HillSea"..grade_type],Vector3.zero,nil,self.root_node)
            table.insert(self.effect_list, effect_handle)
        end
        if Cfg.FallenGodCfgByNpcSeq(self.vo.seq, false) ~= nil then
            local grade_type = 4
            local effect_handle = SE:Play(SceneEffectConfig.EffectList["HillSea"..grade_type],Vector3.zero,nil,self.root_node)
            table.insert(self.effect_list, effect_handle)
        end
    end
end

function SceneObj:CheckSpecials()
    local npc_types = self.vo.npc_types or NpcHelper.NpcTypesById(self.vo.id)
    if npc_types.boss and FightFieldBossData.Instance:IsSceneWild(self.vo.id) then
        local effect_handle = SE:Play(SceneEffectConfig.EffectList.FightFieldBoss,Vector3.zero,nil,self.root_node)
        table.insert(self.effect_list, effect_handle)
    end
end

function SceneObj:CheckHarmonyHeaven()
    if SCENE_TYPE.HARMONY_HEAVEN == SceneData.Instance:GetMode() then
        local scene_info = HarmonyHeavenData.Instance.fb_info.scene_info
        if scene_info.init then return end
        local stage = scene_info.stage
        if stage < 2 then
            local effect_handle = SE:Play(SceneEffectConfig.EffectList.HillSea3,Vector3.zero,nil,self.root_node)
            table.insert(self.effect_list, effect_handle)
        elseif 2 == stage then
            local effect_handle = SE:Play(SceneEffectConfig.EffectList.HillSea4,Vector3.zero,nil,self.root_node)
            table.insert(self.effect_list, effect_handle)
        end
    end
end


function SceneObj:InTheFishingScene()
    if FishData.Instance:InFishMap() and self.vo.obj_type == SceneObjType.MainRole then
        return true
    end
    return false
end

function SceneObj:InTheWeddingScene()
    if SCENE_TYPE.WEDDING_FB == SceneData.Instance:GetMode() and self.vo.obj_type == SceneObjType.MainRole and MarryData.Instance:GetWeddingIsHost() then
        return true
    end
    return false
end

--设置指定场景的位置,只有角色处理当前设置场景时才会被设置，否则指令会被缓存到切换到此场景
function SceneObj:SetScenePos(scene_id, x, y, is_server_pos, z)
    if x == nil or y == nil then
        return
    end
    if scene_id == SceneMgr:LoadedSceneId() then
        self.cache_pos_scene_id = 0
        local cl_p
        local sam_set = false
        if is_server_pos then
            cl_p,sam_set = SceneCtrl.Instance:CurScene():PosToClint(x, y)
        else
            cl_p = Vector3.New(x, 0, y)
            cl_p.y,sam_set = SceneCtrl.Instance:CurScene():GetPosHeight(x, y)
        end
        if z then
            cl_p.y = z
        end
        if sam_set == true then
            if DEBUG_MODE then
                LogError("客户端放置位置，位置坐标有问题, 对象是 === ", self.vo.name, x, y)
            end
        end
        self:SetPos(cl_p,sam_set)
    else
        self.cache_pos_scene_id = is_server_pos and scene_id or -scene_id
        self.cache_pos_x = x
        self.cache_pos_y = y
    end
end

--场景变化
function SceneObj:OnLoadedSceneIdChange()
    self:ApplyCacheScenePos()
    if self:Type() == SceneObjType.MainRole then
        if LoginData.Instance:PCRealNameSignNeedOpen() then
            if ChannelAgent.OpenPCRealNameSignView then
                ChannelAgent.OpenPCRealNameSignView()
            end
        end
        if LoginData.Instance:IsNewChara() then
            LoginData.Instance:ClearIsNewChara()
            if LoginData.Instance:GetMiniGame() > 0 then
                local mini_other
                if TowerAdventureCtrl.Instance:IsNeedFirstStart() then
                    -- 塔楼
                    mini_other = TowerAdventureData.Instance:Other()
                elseif CowBoyCtrl.Instance:IsNeedFirstStart() then
                    --牛郎织女
                    mini_other = CowBoyData.Instance:Other()
                end
                if mini_other then
                    local end_func = function()
                        if not StringIsEmpty(tostring(mini_other.step_id)) then
                            GuideManager.Instance:Start(mini_other.step_id)
                        end
                    end
                    MiniGameCtrl.Instance:OpenDialogView(mini_other.npc_id_1, mini_other.txt_1, end_func)
                end
            else
                local task_info = TaskData.Instance:GetTaskInfoByTaskId(10)
                if task_info then
                    TimeHelper:AddDelayTimer(function ()
                        AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task_info)}, 1)
                    end,1)
                end
            end
        end
    end
end

function SceneObj:FlushRoleStatus(stop_flush)
    
end

function SceneObj:ApplyCacheScenePos()
    if self.cache_pos_scene_id ~= 0 and math.abs(self.cache_pos_scene_id) == SceneMgr:LoadedSceneId() then
        local is_server_pos = self.cache_pos_scene_id > 0
        self.cache_pos_scene_id = 0
        local cl_p
        local need_samp = false
        if is_server_pos then
            cl_p,need_samp = SceneCtrl.Instance:CurScene():PosToClint(self.cache_pos_x, self.cache_pos_y)
        else
            cl_p = Vector3.New(self.cache_pos_x, 0, self.cache_pos_y)
            cl_p.y,need_samp = SceneCtrl.Instance:CurScene():GetPosHeight(self.cache_pos_x, self.cache_pos_y)
        end
        local flag, pos = self:SetPos(cl_p,need_samp)
        if flag == false and self:Type() == SceneObjType.MainRole and self.nav_agent.isOnNavMesh == false then
            TimeHelper:CancelTimer(self.handle_check)
            self.handle_check = TimeHelper:AddRunTimer(function ()
                if self.nav_agent and self.nav_agent.isOnNavMesh == false and self.nav_agent.enabled == true then
                    self:SetPos(pos)
                    TimeHelper:CancelTimer(self.handle_check)
                end
            end, 1, 5)
        end
    end
end

function SceneObj:IsTaskNpc()
    if self.vo == nil then return end
    if Config.npc_auto.task_log[self.vo.id] ~= nil then
        return false
    end
    local is_task_npc, npc_co = nil ~= self.vo
    if is_task_npc then 
        npc_co = NpcHelper.NpcCo(self.vo.id, false)
    end
    is_task_npc = is_task_npc and (npc_co ~= nil and npc_co.is_show == 1 and npc_co.icon_id ~= "ShangDianTuBiao")
    return is_task_npc and self:Type() == SceneObjType.Npc
end

function SceneObj:SetFlyEffect(appearance)
    appearance = appearance or self.vo

    local enable_flag_fly = appearance.fly_flag and appearance.fly_flag > 0
    local enable_flag_marbas = appearance.zaohua_app_id ~= nil

    local flag_fly_change = enable_flag_fly and (appearance.fly_flag ~=  self.cache_fly_flag or self.fly_eff_handle == nil)
    local flag_marbas_change = enable_flag_marbas and (appearance.zaohua_app_id ~=  self.cache_marbas_flag or self.fly_eff_handle == nil)

    if flag_fly_change or flag_marbas_change then
        if self.body.GetHeight and self.body:GetHeight() then            
            SE:Stop(self.fly_eff_handle)
            -- 赋值
            self.cache_marbas_flag = appearance.zaohua_app_id or 0
            local show_flag = self.cache_marbas_flag > 0 
            self.cache_fly_flag = show_flag and 0 or appearance.fly_flag

            local sp_path =  show_flag and MarbasData.Instance:GetVigorousEffId(self.cahce_marbas_flag) 
                or SceneEffectConfig.EffectList["ChFlyUpEffect"..appearance.fly_flag]

            local function fly_eff(obj)
                obj:SetLocalScale(Vector3.New(self.body:GetHeight() / 2,self.body:GetHeight() / 2,self.body:GetHeight() / 2))
            end

            if sp_path ~= nil then 
                self.fly_eff_handle = self:EffectInCenter(sp_path, 
                    show_flag and "cache_marbas_flag" or "cache_fly_flag",
                    show_flag and nil or fly_eff,
                    self.fly_eff_handle)
            end 
        end
    elseif not enable_flag_fly and not enable_flag_marbas then
        SE:Stop(self.fly_eff_handle)
        self.fly_eff_handle = nil
        self.cache_fly_flag = 0
        self.cache_marbas_flag = 0
    end
end

-- 与eff_handle绑定的简化代码(请勿用作他用)
function SceneObj:EffectInCenter(sp_path,check_key,extral_func,lock_key)
    return SE:Play(sp_path, nil,function (obj)
        if self[check_key] and self[check_key] > 0 and obj and self.body and self.body:GetHeight() then
            if extral_func ~= nil then 
                extral_func(obj)
            end  
        else
            SE:Stop(lock_key)
        end
    end, self:CenterPoint())
end

function SceneObj:EnableYinYangEye()
    if not self:CanEnableYinYangEye() then
        return
    end
    if self:Type() == SceneObjType.Npc or self:Type() == SceneObjType.Role or self:Type() == SceneObjType.MainRole then
        local task_info = AnecdoteData.Instance:GetAnecdote(self.vo)
        if self.body and self.body.SetRendererMaterial and not task_info then
            self.body:SetRendererMaterial(tianyan_mat, 1)
        end
        local npc_bubble = AnecdoteData.Instance:GetTaskBubble(self.vo)
        if task_info and task_info.info.is_yinyang == 1 then
            self:SetVisible(true)
            if self.body and self.body.SetRendererMaterial then
                local info = YinYangData.Instance:AnecdoteInfo(self.vo.id)
                self.body:SetRendererMaterial((info and info.NPC_type == 0) and PreloadCtrl.shan or PreloadCtrl.e, 1)
            end
        end
        if npc_bubble and not task_info then
            if npc_bubble.is_show_iden == 1 then
                self:SetVisible(false)
            end
        elseif npc_bubble and task_info and task_info.info.is_yinyang ~= 1 then
            if npc_bubble.is_show_iden == 1 then
                self:SetVisible(false)
            end
        end
    elseif self:Type() == SceneObjType.MapExploreBox then
        self:SetVisible(false)
    end
end

function SceneObj:RevertYinYangEye()
    if self:Type() == SceneObjType.Npc or self:Type() == SceneObjType.Role or self:Type() == SceneObjType.MainRole then
        if self.body and self.body.RevertRendererMaterial then
            self.body:RevertRendererMaterial()
        end
        local npc_bubble = AnecdoteData.Instance:GetTaskBubble(self.vo)
        local task_info = AnecdoteData.Instance:GetAnecdote(self.vo)
        if task_info and task_info.info.is_yinyang == 1 then
            self:SetVisible(false)
        end
        if npc_bubble and not task_info then
            if npc_bubble.is_show_iden == 1 then
                self:SetVisible(true)
            end
        elseif npc_bubble and task_info and task_info.info.is_yinyang ~= 1 then
            if npc_bubble.is_show_iden == 1 then
                self:SetVisible(true)
            end
        end
    elseif self:Type() == SceneObjType.MapExploreBox then
        self:SetVisible(true)
    end
end

function SceneObj:IsAnecdoteObj()
    local task_info = AnecdoteData.Instance:GetAnecdote(self.vo)
    if task_info then
        return true, task_info
    end
    return false
end
--=======================外部接口===============================
function SceneObj:Type()
    return self.vo ~= nil and self.vo.obj_type or SceneObjType.None
end

function SceneObj:Vo()
    return self.vo
end

function SceneObj:ObjId()
    return self.vo and self.vo.obj_id or 0
end

function SceneObj:EnableShadow(enable)
    if CatchButterfliesData.Instance:NpcID() == self.vo.id then
        self.body:EnableShadow(false)
    else
        self.body:EnableShadow(enable)
    end
end

function SceneObj:SetTouchAble(able, v3)
    if self.collider == nil then
        self.collider = self.root_node:GetComponent(typeof(UnityEngine.Collider))
    end
    if self.collider ~= nil then
        self.collider.enabled = able
    end
    if v3 and self.collider ~= nil then
        self.collider.size = v3
    end
end

function SceneObj:SetDirByEulerY(dir_y)
    if self.root_node ~= nil then
        local an = Vector3.New(0, dir_y or self.vo.dir_y or 0, 0)
        self.root_node:SetEulerAngles(an)
    end
end

function SceneObj:GetLocalEulerAngles()
    return self.root_node:GetLocalEulerAngles()
end

function SceneObj:SetMoveSpeed(move_speed)
    self.nav_agent.speed = move_speed
end

function SceneObj:ShieldingPet(hide)
    if self.follow_pet then
        self.follow_pet:SetVisible(not hide)
    end
end

function SceneObj:SetForward(forward)
    forward.y = 0
    self.root_node:SetForward(forward)
end

function SceneObj:GetForward()
    return self.root_node:GetForward()
end

function SceneObj:SetPos(pos,sample_set)
    if self.nav_agent then
        if sample_set == true then
            pos = GameScene.SamplePos(pos)
        end
        self.last_check_pos = pos
        local result = self.nav_agent:Warp(pos)
        if self.nav_agent_enabled == false and not (self.vo and self.vo.nav_agent_block) then
            self.nav_agent.enabled = true
            self.nav_agent_enabled = true
        end
        return result, pos
    end
    return true
end
function SceneObj:SetAngles(v3)
    self.root_node:SetEulerAngles(v3)
end

function SceneObj:SetVoPos(x, y)
    if self.vo ~= nil then
        if self.vo.Val then
            self.vo:Val().x = x
            self.vo:Val().y = y
        else
            self.vo.x = x
            self.vo.y = y
        end
    end
end

--强制设置位置
function SceneObj:ResetPos(pos)
    if self.nav_agent then
        self.nav_agent.enabled = false
    end
    self.root_node:SetPosition(pos)
end

function SceneObj:GetPos()
    self.get_pos_frame = self.get_pos_frame or 0
    if Time.frameCount ~= self.get_pos_frame then
        local x,y,z = self.root_node:GetPositionEx(nil,nil,nil)
        self.frame_pos = self.frame_pos or Vector3.zero
        self.frame_pos:Set(x, y, z)
    end
    return self.frame_pos
end

function SceneObj:LookAt(obj)
    if obj == nil then
        obj = MainPlayer
    end
    if type(self.vo.res_id) == "number" and self.vo and (self.vo.obj_type == SceneObjType.Npc or self.vo.obj_type == SceneObjType.Monster) and self.vo.res_id and self.vo.res_id >= 200 and obj ~= nil then
        local tar_obj = obj.root_node
        local forward = tar_obj:GetPosition() - self.root_node:GetPosition()
        self:SetForward(forward)
    end
end

function SceneObj:IsVisible()
    if self.root_node then
       return self.root_node.activeSelf
    end
    return true
end

function SceneObj:SetVisible(is_visible)
    if self.vo == nil then return end
    if self.root_node then
        self.root_node:SetActive(is_visible)
        if self.follow_pet then
            if TeamData.Instance:MemberInTeam(self.vo.name) then
                self.follow_pet.root_node:SetActive(false)
            else
                self.follow_pet.root_node:SetActive(is_visible)
            end
        end
        if self:Type() == SceneObjType.MainRole or self:Type() == SceneObjType.Role then
            self:FlushIsLeader()
        end
    end
end

function SceneObj:HasAnim(anim)
    if self.body == nil then
        return false
    end
    return self.body:HasAnim(anim)
end
function SceneObj:PlayAnim(anim, param, is_ride)
    if self.body == nil then
        return false
    end
    return self.body:PlayAnim(anim, param, is_ride)
end

function SceneObj:FreeTimeAnim()
    --只有龙坐骑需要play show2这个动画
    TimeHelper:CancelTimer(self.free_time_anim)
    local sur_info = Cfg.SurfaceInfoByResId(self.vo.appearance.ride_id,false)
    if sur_info and sur_info.action > 0 then
        self.free_time_anim = TimeHelper:AddDelayTimer(function ()
            -- if self.vo and self.vo.appearance and self.vo.appearance.ride_id == 1041 then
                if self.last_check_pos == nil or self.last_check_pos == self:GetPos() then
                    SceneObj.PlayAnim(self, sur_info.action_2, nil, true)
                end
                self.last_check_pos = self:GetPos()
                self:FreeTimeAnim()
            -- end
        end, sur_info.action)
    end
end

function SceneObj:StopMove()
    self.body:PlayAnim(ChAnimType.Idle)
    self.nav_agent:ResetPath()
end

--普通死亡消失表现
function SceneObj:Disappear(on_com)
    self:PlayAnim(ChAnimType.Death)
    self:PlayBuffAssetStart("DieEff",function (player)
        if on_com then
            on_com()
        end
    end, nil, true)
end
function SceneObj:PlayBuffAssetStart(asset_name, on_start_finish, on_remove, dont_remove)
    GameBuffManager:AddPlayer(asset_name, self.root_node, function(player)
        player.OnRemoveAction = function()
            if on_remove then
                on_remove()
            end
            -- self:OnBuffRemove(player, asset_name)
        end
        self.effect_buff_player[asset_name] = player
        self:PlayBuffAsset(player, BuffStageKey.Start, function()
            if on_start_finish then
                on_start_finish(player)
            end
            if not dont_remove then
                GameBuffManager:RemovePlayer(player)
                if self.effect_buff_player then
                    self.effect_buff_player[asset_name] = nil
                end
            end
        end)
    end)

end

function SceneObj:PlayBuffAsset(player, stage_key, on_finished)
    if player and player:HasStage(stage_key) then
        player:Play(
            stage_key,
            function()
                if on_finished then
                    on_finished()
                end
            end
        )
        return true
    else
        if on_finished then
            on_finished()
        end
        return false
    end
end