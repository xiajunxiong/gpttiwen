FILE_REQ("modules/chief/chief_config")
FILE_REQ("modules/chief/chief_data")
VIEW_REQ("modules/chief/chief_info_view")
VIEW_REQ("modules/chief/chief_other_view")
VIEW_REQ("modules/chief/chief_election_match")
VIEW_REQ("modules/chief/chief_peak_battle_match")
VIEW_REQ("modules/chief/chief_rank_view")
VIEW_REQ("modules/chief/chief_primary_info_view")

ChiefCtrl = ChiefCtrl or BaseClass(BaseCtrl)
PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        ResMgr:LoadObjectOfType("material/Statue.mat",typeof(UnityEngine.Material),
            function(obj)
                ChiefCtrl.statue_mat = obj
                PreloadCtrl.UnregisterLoading()
            end
        )
    end
)

function ChiefCtrl:__init()
    if ChiefCtrl.Instance ~= nil then
        Debuger.LogError("[ChiefCtrl] attempt to create singleton twice!")
        return
    end
    ChiefCtrl.Instance = self
    ChiefElectionData.New()
    ChiefPeakBattleData.New()
    
    self.state_objs = {}
    self.handle_runners = {}
    self:RegisterAllProtocols()
end

function ChiefCtrl:__delete()
    self:UnRegisterAllProtocols()
    ChiefCtrl.Instance = nil
end

function ChiefCtrl:OnUnloadGameLogic()
    self.IsSendReferRoleEquipment = nil
    ChiefCtrl.OpenChiefElection = nil
    ChiefCtrl.OpenRespondFunc = nil
end

function ChiefCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSChiefElectionReq)
    self:RegisterProtocol(SCChiefElectionNotice, "OnChiefElectionNotice")
    self:RegisterProtocol(SCChiefElectionMatchInfo, "OnChiefElectionMatchInfo")
    self:RegisterProtocol(SCChiefElectionRoleInfo, "OnChiefElectionRoleInfo")
    self:RegisterProtocol(SCChiefElectionTopInfo, "OnChiefElectionTopInfo")
    self:RegisterProtocol(SCChiefElectionJoinRoleInfo, "OnChiefElectionJoinRoleInfo")
    self:RegisterProtocol(SCChiefElectionChampion, "OnChiefElectionChampion")
    self:RegisterProtocol(SCChiefElectionFighterFinishNotice, "OnChiefElectionFighterFinishNotice")

    self:RegisterProtocol(CSChiefPeakBattleReq)
    self:RegisterProtocol(SCChiefPeakBattleNotice, "OnChiefPeakBattleNotice")
    self:RegisterProtocol(SCChiefPeakBattleMatchInfo, "OnChiefPeakBattleMatchInfo")
    self:RegisterProtocol(SCChiefPeakBattleRoleInfo, "OnChiefPeakBattleRoleInfo")
    self:RegisterProtocol(SCChiefPeakBattleInfo, "OnChiefPeakBattleInfo")
    self:RegisterProtocol(SCChiefPeakBattleChampion, "OnChiefPeakBattleChampion")
    self:RegisterProtocol(SCChiefPeakBattleFighterFinishNotice, "OnChiefPeakBattleFighterFinishNotice")

    --预选赛
    self:RegisterProtocol(SCChiefElectionRoleSeaInfo, "OnChiefElectionSeaRoleInfo")
    self:RegisterProtocol(SCChiefElectionSeaSceneInfo, "OnChiefElectionSeaSceneInfo")
    self:RegisterProtocol(SCChiefElectionStateInfo, "OnChiefElectionStateInfo")
    self:RegisterProtocol(SCChiefElectionSeaRank, "OnChiefElectionSeaRank")
    --战场buff
    self:RegisterProtocol(SCChiefElectionBattleResult, "OnChiefElectionBattleResult")
end

function ChiefCtrl:OnInit()
    ActivityData.Instance:CustomClickHandle(ActType.ChiefElection, BindTool.Bind(self.OnChiefElection, self))
    ActivityData.Instance:CustomClickHandle(ActType.ChiefPeakBattle, BindTool.Bind(self.OnPeakBattleMatch, self))

    self.handle_act = ActivityData.Instance.activity_status_event:KeysCare({"act_type", "status"},BindTool.Bind(self.OnActEvent, self))
    self.on_scene_data_inited = EventSys:Bind(GlobalEventKeys.SceneDataInited,BindTool.Bind(self.OnSceneDataInited, self))
    self.on_scene_data_disposed = EventSys:Bind(GlobalEventKeys.SceneDataDisposed,BindTool.Bind(self.OnSceneDataDisposed, self))

    FinishData.Instance:FailRegister(BATTLE_MODE.BATTLE_MODE_CHIEF_ELECTION,{
        desc_func = function()return ChiefElectionData.Instance:GetFailTip()end
    })
    FinishData.Instance:FailRegister(BATTLE_MODE.BATTLE_MODE_CHIEF_PRIMARY,{
        desc_func = function()return ChiefElectionData.Instance:GetPrimaryTip()end
    })
    FinishData.Instance:PassRegister(BATTLE_MODE.BATTLE_MODE_CHIEF_PEAK_BATTLE,{
        desc_func = function()
            return ChiefPeakBattleData.Instance:GetFailTip()
        end,
        reward_func = function()
            return ChiefPeakBattleData.Instance:GetRewardData()
        end,
        show_type = ChiefPeakBattleData.Instance:GetFhishShowType()
    })

    ActivityData.Instance:RegisterRewardGetFunc(ActType.ChiefElection, function ()
        return ChiefElectionData.Instance:GetTotalPreviewReward(true)
    end)
end

function ChiefCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(CSChiefElectionReq)
    self:UnRegisterProtocol(SCChiefElectionNotice)
    self:UnRegisterProtocol(SCChiefElectionMatchInfo)
    self:UnRegisterProtocol(SCChiefElectionRoleInfo)
    self:UnRegisterProtocol(SCChiefElectionTopInfo)
    self:UnRegisterProtocol(SCChiefElectionJoinRoleInfo)
    self:UnRegisterProtocol(SCChiefElectionChampion)
    self:UnRegisterProtocol(SCChiefElectionFighterFinishNotice)

    self:UnRegisterProtocol(CSChiefPeakBattleReq)
    self:UnRegisterProtocol(SCChiefPeakBattleNotice)
    self:UnRegisterProtocol(SCChiefPeakBattleMatchInfo)
    self:UnRegisterProtocol(SCChiefPeakBattleRoleInfo)
    self:UnRegisterProtocol(SCChiefPeakBattleInfo)
    self:UnRegisterProtocol(SCChiefPeakBattleChampion)
    self:UnRegisterProtocol(SCChiefPeakBattleFighterFinishNotice)

    self:UnRegisterProtocol(SCChiefElectionRoleSeaInfo)
    self:UnRegisterProtocol(SCChiefElectionSeaSceneInfo)
    self:UnRegisterProtocol(SCChiefElectionStateInfo)
    self:UnRegisterProtocol(SCChiefElectionSeaRank)
    self:UnRegisterProtocol(SCChiefElectionBattleResult)
end

function ChiefCtrl:SendExitTeam()
    if TeamCtrl.Instance:Data():InTeam() then
        TeamCtrl.Instance:SendExitTeam()
    end
end
--=======================首席竞选======================
CHIEFELECTION_REQ = {
    CAMP_REQ = 1,
    JOIN_REQ = 2,
    READY_REQ = 3,
    RINK_REQ = 4,
    GOTO = 5,
    LOG = 6,
}

--[[
    @LOG_TYPE = 1 : 玩家收到进入协议 p2 round_state
    @LOG_TYPE = 2 : 玩家打开了界面
    @LOG_TYPE = 3 : 玩家正在战斗 弹出了首席弹窗
    @LOG_TYPE = 4 : 玩家战斗结束 重新打开界面
    @LOG_TYPE = 5 : 玩家首席对战时间到了自动关闭界面 玩家自己没点进去
    @LOG_TYPE = 6 : 玩家点击进入
    @LOG_TYPE = 7 : 玩家自己关闭了界面
]]

--信息请求 req_type 0角色信息 1 比赛信息 2报名 3准备 4排行榜信息
function ChiefCtrl:SendElectionReq(req_type, param1, param2)
    local protocol = GetProtocol(CSChiefElectionReq)
    protocol.req_type = req_type or 0
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    SendProtocol(protocol)
end

--战斗信息 是通知回合的变化的
function ChiefCtrl:OnChiefElectionNotice(protocol)
    ChiefElectionData.Instance:SetChiefElectionNotice(protocol)
    --玩家中途x回合准备阶段掉了，上线收到这个，能继续计入的
    if protocol.round_state == 0 then
        --玩家已经在首席16强的场景里
        if SceneData.Instance:GetMode() == SceneModeType.ChiefElelction then
            LogError("玩家已经在首席16强的场景里")
            return
        end
        --是否在队伍中
        self:SendExitTeam()
        --请求战斗排行信息 是所有参赛的人员的信息
        self:SendElectionReq(CHIEFELECTION_REQ.CAMP_REQ)
        --首席在战斗中会弹窗
        if MainViewData.Instance:IsBattle() then
            ChiefCtrl.OpenRespondFunc = function()
                ViewMgr:OpenView(ChiefWaitRespondView)
                ChiefCtrl.Instance:SendElectionReq(CHIEFELECTION_REQ.LOG,4)
            end
            ChiefCtrl.Instance:SendElectionReq(CHIEFELECTION_REQ.LOG,3)
            BuglyReport("玩家正在战斗 弹出了首席弹窗 role_name=",RoleData.Instance:GetRoleName())
        else
            --弹出进入界面
            ViewMgr:OpenView(ChiefWaitRespondView)
            ChiefCtrl.Instance:SendElectionReq(CHIEFELECTION_REQ.LOG,2)
        end
        --显示场景右边框图标
        MainOtherData.Instance:SetRightMenuData({key = "Chief", is_active = true}) 
    else
        ViewMgr:CloseView(ChiefElectionMatch)
    end
end

--战斗排行信息 是所有参赛的人员的信息
function ChiefCtrl:OnChiefElectionMatchInfo(protocol)
    ChiefElectionData.Instance:SetChiefElectionMatchInfo(protocol)
end

--报名状态
function ChiefCtrl:OnChiefElectionRoleInfo(protocol)
    ChiefElectionData.Instance:SetChiefElectionRoleInfo(protocol)
end

--冠军胜出的人
function ChiefCtrl:OnChiefElectionTopInfo(protocol)
    if self.IsSendReferRoleEquipment == nil then
        ChiefElectionData.Instance:SetChiefElectionTopInfo(protocol)
        self.winner_list = protocol.winner_list
        for _, info in ipairs(protocol.winner_list) do
            if info.role_id ~= 0 then
                RoleInfoCtrl.Instance:SendReferRoleEquipment(info.role_id)
            end
        end
        if protocol.winner_id ~= 0 then
            RoleInfoCtrl.Instance:SendReferRoleEquipment(protocol.winner_id)
        end
        self.IsSendReferRoleEquipment = true
    end
end

--战力排行榜
function ChiefCtrl:OnChiefElectionJoinRoleInfo(protocol)
    ChiefElectionData.Instance:SetChiefElectionJoinRoleInfo(protocol)
end

--首席诞生下发
function ChiefCtrl:OnChiefElectionChampion(protocol)
    ChiefCtrl.IsChiefnFinish = true
    ViewMgr:OpenView(ChiefFinishView)
    if ViewMgr:IsOpen(ChiefElectionMatch) then
        ViewMgr:CloseView(ChiefElectionMatch)
    end
end

--通知是否打完
function ChiefCtrl:OnChiefElectionFighterFinishNotice(protocol)
    ChiefElectionData.Instance:SetWinerInfo(protocol.role_id)
    if SceneData.Instance:GetMode() ~= SceneModeType.ChiefElelction then
        return
    end
    if RoleData.Instance:GetRoleId() ~= protocol.role_id then
        return
    end
    if protocol.is_win == 1 and ChiefCtrl.IsChiefnFinish then
        return
    end
    FinishCtrl.Instance:Finish({is_win = protocol.is_win},BATTLE_MODE.BATTLE_MODE_CHIEF_ELECTION)
end

--==========================首席巅峰战==========================
CHIEFPEAKBATTLE_REQ = {
    CAMP_REQ = 1,
    BETS_REQ = 2,
    READY_REQ = 3,
    DECLARA_REQ = 4
}
--角色信息 比赛信息 下注 p1 seq p2 figher_index 准备
function ChiefCtrl:SendPeakBattleReq(req_type, param1, param2, declara)
    local protocol = GetProtocol(CSChiefPeakBattleReq)
    protocol.req_type = req_type or 0
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    protocol.declara = declara or ""
    SendProtocol(protocol)
end

--战斗信息 是通知回合的变化的
function ChiefCtrl:OnChiefPeakBattleNotice(protocol)
    if not FunOpen.Instance:GetFunIsOpened(Mod.Activity.TimeLimit) then
        return
    end
    if self:IsNoEnoughLevel(ActType.ChiefPeakBattle) then
        return
    end
    if ActivityData.IsOpen(ActType.ChiefPeakBattle) then
        return
    end
    ChiefPeakBattleData.Instance:SetChiefPeakBattleNotice(protocol)
    if protocol.round_state == 0 then
        self:SendExitTeam()
        self:SendPeakBattleReq(CHIEFPEAKBATTLE_REQ.CAMP_REQ)
        if not ChiefCtrl.OpenChiefPeakBattle then
            ChiefCtrl.OpenChiefPeakBattle = true
            self:OpenRespondView()
            MainOtherData.Instance:SetRightMenuData({key = "Chief", is_active = true})
        end
    else
        if ViewMgr:IsOpen(ChiefPeakBattleMatch) then
            ViewMgr:CloseView(ChiefPeakBattleMatch)
        end
    end
end

--战斗排行信息 是所有参赛的人员的信息
function ChiefCtrl:OnChiefPeakBattleMatchInfo(protocol)
    ChiefPeakBattleData.Instance:SetChiefPeakBattleMatchInfo(protocol)
end

function ChiefCtrl:OnChiefPeakBattleRoleInfo(protocol)
    ChiefPeakBattleData.Instance:SetChiefPeakBattleRoleInfo(protocol)
end

--角色冠军下发
function ChiefCtrl:OnChiefPeakBattleInfo(protocol)
    ChiefPeakBattleData.Instance:SetChiefPeakBattleInfo(protocol)
    RoleInfoCtrl.Instance:SendReferRoleEquipment(protocol.winner_id or 0)
end

function ChiefCtrl:OnChiefPeakBattleChampion(protocol)
    ChiefCtrl.IsChiefnFinish = true
    ViewMgr:OpenView(ChiefFinishView)
    if ViewMgr:IsOpen(ChiefPeakBattleMatch) then
        ViewMgr:CloseView(ChiefPeakBattleMatch)
    end
end

--通知是否打完
function ChiefCtrl:OnChiefPeakBattleFighterFinishNotice(protocol)
    ChiefPeakBattleData.Instance:SetWinerInfo(protocol.role_id, protocol.is_win)
end

--===================================首席优化==================================
--首席巅峰战打开
function ChiefCtrl:OnPeakBattleMatch()
    if self:GetStatus(ActType.ChiefPeakBattle) == ActStatusType.Open then
        ViewMgr:OpenView(ChiefPeakBattleMatch)
    else
        PublicPopupCtrl.Instance:Center(Language.ChiefPeakBattle.ErrorLog[7])
        return true
    end
end

--活动监听下发
function ChiefCtrl:OnActEvent()
    if not FunOpen.Instance:GetFunIsOpened(Mod.Activity.TimeLimit) then
        return
    end
    if ActivityData.Instance:GetActivityEventType() == ActType.ChiefElection then
        if self:IsNoEnoughLevel(ActType.ChiefElection) then
            return
        end
        if self:GetStatus(ActType.ChiefElection) == ActStatusType.Close then
            ChiefElectionData.Instance:CancelData()
            if ViewMgr:IsOpen(ChiefElectionMatch) then
                ViewMgr:CloseView(ChiefElectionMatch)
            end
        elseif self:GetStatus(ActType.ChiefElection) == ActStatusType.Open then
            self.IsSendReferRoleEquipment = nil
        end
    end
    if ActivityData.Instance:GetActivityEventType() == ActType.ChiefPeakBattle then
        if self:IsNoEnoughLevel(ActType.ChiefPeakBattle) then
            return
        end
        if self:GetStatus(ActType.ChiefPeakBattle) == ActStatusType.Close then
            ChiefPeakBattleData.Instance:CancelData()
            if ViewMgr:IsOpen(ChiefPeakBattleMatch) then
                ViewMgr:CloseView(ChiefPeakBattleMatch)
            end
        end
    end
end

function ChiefCtrl:GetData()
    return self:IsElection() and ChiefElectionData.Instance or ChiefPeakBattleData.Instance
end

function ChiefCtrl:IsElection()
    return self:GetStatus(ActType.ChiefElection) == ActStatusType.Open
end

--获取活动状态
function ChiefCtrl:GetStatus(act_type)
    return ActivityData.Instance:GetActivityStatus(act_type)
end

function ChiefCtrl:Cinematics()
    PublicPopupCtrl.Instance:BattleCinematics(BattleCinematics.Type.Normal,0.5,nil,function()
        PublicPopupCtrl.Instance:CloseBattleCinematics()
    end)
end

function ChiefCtrl:Enter()
    if not FunOpen.Instance:GetFunIsOpened(Mod.Activity.TimeLimit) then
        return
    end
    if self:IsElection() then
        if self:IsNoEnoughLevel(ActType.ChiefElection) then
            return
        end
        self:SendElectionReq(CHIEFELECTION_REQ.READY_REQ)
    else
        if self:IsNoEnoughLevel(ActType.ChiefPeakBattle) then
            return
        end
        self:SendPeakBattleReq(CHIEFPEAKBATTLE_REQ.READY_REQ)
    end
end

--首席竞选订阅事件
function ChiefCtrl:ChiefEctionSubscribe()
    if not ChiefCtrl.EctionSubscribe then
        local prof = RoleData.Instance:GetRoleProf()
        if prof ~= 0 then
            LoginCtrl.Instance:RoleSubscribe(prof)
            ChiefCtrl.EctionSubscribe = true
        end
    end
end

--首席巅峰战订阅事件
function ChiefCtrl:ChiefPeakBattleSubscribe()
    if not ChiefCtrl.PeakBattleSubscribe then
        LoginCtrl.Instance:RoleSubscribe(RoleSubscribeTopicType.ChiefPeakBattle)
        ChiefCtrl.PeakBattleSubscribe = true
    end
end

--是否满足开启条件
function ChiefCtrl:IsCanOpenEction()
    local cfg = Config.daily_activity_auto.daily[ActType.ChiefElection]
    return RoleData.Instance:GetRoleLevel() >= cfg.level_restrict
end

--开启首席弹窗
function ChiefCtrl:OpenRespondView()
    if MainViewData.Instance:IsBattle() then
        ChiefCtrl.OpenRespondFunc = function()
            ViewMgr:OpenView(ChiefWaitRespondView)
        end
        BuglyReport("玩家正在战斗 弹出了首席弹窗 role_name=",RoleData.Instance:GetRoleName(),BattleData.Instance:BaseData())
        return
    end
    ViewMgr:OpenView(ChiefWaitRespondView)
end

local StatueSceneInfo = {
    [208] = {x = 79.3, y = 167.3, z = 4.4, dir_y = -75, prof = 1},
    [210] = {x = 79.3, y = 167.3, z = 4.4, dir_y = -75, prof = 2},
    [211] = {x = 79.3, y = 167.3, z = 4.4, dir_y = -75, prof = 5},
    [212] = {x = 79.3, y = 167.3, z = 4.4, dir_y = -75, prof = 4},
    [213] = {x = 79.3, y = 167.3, z = 4.4, dir_y = -75, prof = 6},
    [206] = {x = 79.3, y = 167.3, z = 4.4, dir_y = -75, prof = 3},
    [209] = {x = 79.3, y = 167.3, z = 4.4, dir_y = -75, prof = 8},
    [207] = {x = 79.3, y = 167.3, z = 4.4, dir_y = -75, prof = 7}
}

function ChiefCtrl:OnSceneDataInited(scene_id)
    --[[ if self.wait_for_statue_loaded then
        Runner.Instance:RemoveRunUntilTrue(self.wait_for_statue_loaded)
        self.wait_for_statue_loaded = nil
    end
    if self.wait_for_query then
        Runner.Instance:RemoveRunUntilTrue(self.wait_for_query)
        self.wait_for_query = nil
    end
    if self.wait_for_winner_list then
        Runner.Instance:RemoveRunUntilTrue(self.wait_for_winner_list)
        self.wait_for_winner_list = nil
    end ]]
    for _, value in ipairs(self.handle_runners) do
        Runner.Instance:RemoveRunUntilTrue(value)
    end
    self.handle_runners = {}
    for _, value in ipairs(self.state_objs) do
        value:DeleteMe()
    end
    self.state_objs = {}
    --[[ if self.state_obj then
        self.state_obj:DeleteMe()
        self.state_obj = nil
    end ]]

    --[[ local scene_info = StatueSceneInfo[scene_id]
    if not scene_info then
        return
    end ]]
    -- self.wait_for_winner_list =
    if scene_id ~= SceneLogic.Instance:MainCityID() then
        return
    end
    self.handle_runners[#self.handle_runners+1] = Runner.Instance:RunUntilTrue(
        function()
            if self.winner_list then
                for _, info in pairs(self.winner_list) do
                    if info.role_id ~= 0 then
                        self.handle_runners[#self.handle_runners+1] = Runner.Instance:RunUntilTrue(
                            function()
                                local role_info = RoleInfoCtrl.Instance.data:GetQueryRoleInfo(info.role_id)
                                if role_info then
                                    local role_appearance = TableCopy(role_info.appearance)
                                    local pos_data = ChiefElectionData.Instance.pos_list[info.prof]
                                    role_appearance.ride_id = 0
                                    role_appearance.bicycle_index = -1
                                    role_appearance.model_card_flag = 0
                                    role_appearance.is_use_huanshou = 0
                                    self.state_objs[#self.state_objs+1] =
                                    SceneObj.CreateObj(
                                        SmartData.New(
                                            {
                                                obj_id = CUSTOM_OBJID.CHIEF_STATUE,
                                                dir_y = pos_data.dir_y,
                                                scene_id = scene_id,
                                                title_id = string.format("<color=#%s>%s</color>", COLORSTR.Yellow1, Language.ChiefElection.ProfTitle .. Language.Common.Prof[info.prof]),
                                                name = role_info.role_name,
                                                obj_type = SceneObjType.Statue,
                                                appearance = role_appearance,
                                                ignor_follow = true,
                                                res_size = pos_data.mod_size,
                                            }
                                        )
                                    )
                                    local state_obj = self.state_objs[#self.state_objs]
                                    -- state_obj:SetScenePos(scene_id, 228.06, 179.92, false, 65.985)
                                    state_obj:ResetPos(Vector3.New(pos_data.x, pos_data.y, pos_data.z))
                                    self.handle_runners[#self.handle_runners+1] =
                                        Runner.Instance:RunUntilTrue(
                                        function()
                                            local body = state_obj.body
                                            if not body:IsLoading() then
                                                body.main_body:SetScale(pos_data.mod_size)
                                                body.main_anim:SetAnimActive(true)
                                                body.main_body:SampleAnim("d_idle", 0.1)
                                                body.main_anim:SetAnimActive(false)
                                                body:ForeachBodyObject(
                                                    function(bo)
                                                        bo:RemoveRendererMaterial(Game.RendererMaterialLayer.Base:ToInt())
                                                        bo:SetRendererMaterial(ChiefCtrl.statue_mat, 1, {"_MainTex"})
                                                    end
                                                )
                                                return true
                                            end
                                        end,
                                        true
                                    )
                                    return true
                                end
                            return true
                        end)
                    end
                end
            end
        return true
        end)
end

function ChiefCtrl:OnSceneDataDisposed(scene_id)
    for _, value in ipairs(self.handle_runners) do
        Runner.Instance:RemoveRunUntilTrue(value)
    end
    self.handle_runners = {}
    for _, value in ipairs(self.state_objs) do
        value:DeleteMe()
    end
    self.state_objs = {}
    --[[ if self.wait_for_statue_loaded then
        Runner.Instance:RemoveRunUntilTrue(self.wait_for_statue_loaded)
        self.wait_for_statue_loaded = nil
    end
    if self.wait_for_query then
        Runner.Instance:RemoveRunUntilTrue(self.wait_for_query)
        self.wait_for_query = nil
    end
    if self.wait_for_winner_list then
        Runner.Instance:RemoveRunUntilTrue(self.wait_for_winner_list)
        self.wait_for_winner_list = nil
    end
    if self.state_obj then
        self.state_obj:DeleteMe()
        self.state_obj = nil
    end ]]
end

function ChiefCtrl:IsNoEnoughLevel(act_type)
	local act_cfg = ActivityData.GetConfig(act_type)
	return RoleData.Instance:GetRoleLevel() < act_cfg.level_restrict
end

--===================================新修改==================================
--首席竞选打开
function ChiefCtrl:OnChiefElection()
    if self:GetStatus(ActType.ChiefElection) == ActStatusType.Open then
        if ChiefElectionData.Instance:GetMatchState() == 3 then
            ViewMgr:OpenView(ChiefElectionMatch)
            return
        end
        if TeamCtrl.Instance:Data():InTeam() then
            PublicPopupCtrl.Instance:AlertTip(Language.ChiefElection.ErrorLog[12],function()
                TeamCtrl.Instance:SendExitTeam()
                self:SendElectionReq(CHIEFELECTION_REQ.GOTO)
            end)
            return 
        end
        self:SendElectionReq(CHIEFELECTION_REQ.GOTO)
    else
        PublicPopupCtrl.Instance:Center(Language.ChiefElection.ErrorLog[11])
        return true
    end
end

--混战角色信息
function ChiefCtrl:OnChiefElectionSeaRoleInfo(protocol)
    ChiefElectionData.Instance:SetChiefElectionSeaRoleInfo(protocol)
end

--混战场景人数信息
function ChiefCtrl:OnChiefElectionSeaSceneInfo(protocol)
    ChiefElectionData.Instance:SetChiefElectionSeaSceneInfo(protocol)
end

--混战活动状态
function ChiefCtrl:OnChiefElectionStateInfo(protocol)
    if self:IsNoEnoughLevel(ActType.ChiefElection) then
        return
    end
    if RoleData.Instance:GetRoleProfType() ~= protocol.prof_match then
        return
    end
    if SceneData.Instance:GetMode() ~= SceneModeType.ChiefPrimary then
        if protocol.match_state == 0 then
            local act_info = ActivityData.Instance:GetActivityInfo(ActType.ChiefElection)
            PublicPopupCtrl.Instance:ActTips(act_info,protocol.next_match_state_time)
        end
    end
    ChiefElectionData.Instance:SetChiefElectionStateInfo(protocol)
end

--混战排行榜下发
function ChiefCtrl:OnChiefElectionSeaRank(protocol)
    ChiefElectionData.Instance:SetChiefElectionSeaRank(protocol)
end

function ChiefCtrl:IsEndChiefPrimary(mode)
    if mode == SceneModeType.ChiefPrimary then
        return ChiefElectionData.Instance:GetMatchState() == 0
    end
    return false
end

function ChiefCtrl:OnChiefElectionBattleResult(protocol)
    ChiefElectionData.Instance:SetChiefElectionBattleResult(protocol)
end