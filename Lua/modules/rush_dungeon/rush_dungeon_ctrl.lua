FILE_REQ("modules/rush_dungeon/rush_dungeon_data")
VIEW_REQ("modules/rush_dungeon/rush_dungeon_left")
VIEW_REQ("modules/rush_dungeon/rush_dungeon_finish")
VIEW_REQ("modules/rush_dungeon/rush_dungeon_rank")
VIEW_REQ("modules/rush_dungeon/rush_dungeon_bless")
VIEW_REQ("modules/rush_dungeon/rush_dungeon_submit")
VIEW_REQ("modules/rush_dungeon/rush_dungeon_complete")
VIEW_REQ("modules/rush_dungeon/rush_dungeon_name")
VIEW_REQ("modules/rush_dungeon/rush_dungeon_menu")
VIEW_REQ("modules/rush_dungeon/rush_dungeon_reward")
-- 勇闯地宫
RushDungeonCtrl = RushDungeonCtrl or BaseClass(BaseCtrl)
BRAVE_GROUND_REQ_TYPE = {
    RANK_INFO = 0,
    --请求排行榜
    USE_NPC = 1,
    --使用npc param1 npc的objid
    INFO = 2,
    --信息
    ROUTE = 3,
    --操作转发
    SELECT_LEVEL = 4,--跳关
    SAO_DANG = 5,--扫荡
}
local act_seq = 4
local npc_seq = 111
local accpet_id = 40121
local beh_id = 92

function RushDungeonCtrl:__init()
    if RushDungeonCtrl.Instance ~= nil then
        Debuger.LogError("[RushDungeonCtrl] attempt to create singleton twice!")
        return
    end
    RushDungeonCtrl.Instance = self
    self.data = RushDungeonData.New()
    self:init_config()

    self:RegisterProtocol(SCBraveGroundRankInfo, "RecvBraveGroundRankInfo")
    self:RegisterProtocol(SCBraveGroundLevelRewardInfo, "RecvBraveGroundLevelRewardInfo")
    self:RegisterProtocol(SCBraveGroundRoute, "RecvBraveGroundRoute")
    self:RegisterProtocol(SCBraveGroundInfo, "RecvBraveGroundInfo")
    self:RegisterProtocol(SCBraveGroundNotify, "RecvBraveGroundNotify")
    self:RegisterProtocol(SCBraveGroundSceneInfo, "RecvBraveGroundSceneInfo")
    self:RegisterProtocol(SCBraveGroundUseItemInfo, "OnBraveGroundUseItemInfo")
    self:RegisterProtocol(SCBraveGroundTeamInfo, "OnBraveGroundTeamInfo")
    self:RegisterProtocol(SCBraveGroundRoleInfo, "OnBraveGroundRoleInfo")
    self:RegisterProtocol(SCBraveGroundCommonInfo, "OnBraveGroundCommonInfo")
    self:RegisterProtocol(CSBraveGroundReq)

    for _, v in ipairs(self.data.npc_list) do
        BehaviorData.Instance:CustomNpcTalkBtnsId(v.npc_id, BindTool.Bind(self.SetupBehaviourBtn, self))
    end
    BehaviorData.Instance:CustomNpcTalkBtnsId(accpet_id, BindTool.Bind(self.SetupBehaviourBtn2, self), true)
    --注册结算事件
    FinishData.Instance:FailRegister(
        BATTLE_MODE.BATTLE_MODE_BRAVE_GTOUND,
        {
            desc_func = function()
                return self.data:GetFailTip()
            end
        }
    )
    FinishData.Instance:PassRegister(BATTLE_MODE.BATTLE_MODE_BRAVE_GTOUND,{
        reward_func = function ()
            return self.data:RewardList()
        end,
        reward_desc_func = function ()
            return Language.RushDungeon.RewardYetGet
        end
    })
    FinishData.Instance:SetCloseFunc(
        BATTLE_MODE.BATTLE_MODE_BRAVE_GTOUND,
        function()
            if not FinishData.Instance:IsWin() then
                RushDungeonCtrl.Instance:SendBraveGroundReq(
                    {
                        req_type = BRAVE_GROUND_REQ_TYPE.INFO
                    }
                )
                -- SceneCtrl.Instance:RequestLeaveFb()
                -- ViewMgr:OpenView(RushDungeonRank)
                -- RushDungeonData.Instance:ClearData()
            end
        end
    )
    -- ActivityData.Instance:SetActivityFinishInfo(ActType.RushDungeon, 0, self.data:GetOpenLevel())
    ActivityData.Instance:CustomClickHandle(ActType.RushDungeon, BindTool.Bind(self.OpenRushDungeon, self))
    -- RemindCtrl.Instance:Remind():Register(Mod.Challenge.RushDungeon,self.data.mop_up_info,BindTool.Bind(self.FlushRewardRemind, self))
end
function RushDungeonCtrl:OpenRushDungeon()
    ViewMgr:OpenView(RushDungeonMenu)
end
function RushDungeonCtrl:OnUnloadGameLogic()
    RushDungeonData.Instance:ClearData()
end
function RushDungeonCtrl:SetupBehaviourBtn2(btn_list, obj_vo)
    if obj_vo and obj_vo.id == accpet_id then
        local info = self.data.mop_up_info
        if info.level <= 0 or info.is_get == 1 then
            return
        end
        local button1 =
            NpcDialogBehBtnData.FuncCreate(
            -- 判断有没有队伍 或者人数不足
            Language.Task.TaskBtn.RushDungeon2,
            function()
                ViewMgr:OpenView(RushDungeonMenu)
                --[[ local state = ActivityData.Instance:GetActivityStatus(ActType.RushDungeon)
                if state == ActStatusType.Close then
                    PublicPopupCtrl.Instance:Center(Language.RushDungeon.OpenTip)
                    return
                end ]]
            end
        )
        -- button1:SetIconType(NpcIconType.Act)
        -- local new_list = {}
        -- table.insert(btn_list, button1)
        --[[ local state = ActivityData.Instance:GetActivityStatus(ActType.RushDungeon)
        if state == ActStatusType.Close then
            return new_list
        end ]]
    end
end
function RushDungeonCtrl:OnInit()
    self.handle_scene =
        SceneData.Instance:Base():Care(
        function()
            if self.data:IsInScene() then
                local id = SceneData.Instance:SceneId()
                local name = SceneData.Instance:SceneName()
                if id >= 1110 then
                    name = string.format(Language.RushDungeon.MapName, id - 1100)
                else
                    name = string.format(Language.RushDungeon.MapName, id - 1100 + 1)
                end
                SceneData.Instance:SetCustomSceneName(id, name)
            else
                RushDungeonData.Instance:ClearData()
            end
        end,
        "scene_id"
    )
    --[[ ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.RushDungeon.Main,function ()
            local data = self.data:RankInfo().rank_list
            local role_id = RoleData.Instance:GetRoleId()
            local cur_level = 0
            -- local cur_rank = 9999
            for _, v in ipairs(data) do
                for i = 1, v.role_info_count do
                    if v.role_info[i].role_id == role_id then
                        cur_level = math.max(cur_level, v.reached_level)
                        -- cur_rank = math.min(cur_rank, v.rank)
                    end
                end
            end
            return cur_level
            -- ChallengeNaviCtrl.SetFormatDescNum(desc_val_txt,cur_level)
        end) ]]

end
function RushDungeonCtrl:__delete()
    SceneData.Instance:Base():Uncare(self.handle_scene)
    RushDungeonCtrl.Instance = nil
end

function RushDungeonCtrl:init_config()
    --[[ ActivityData.Instance:CustomClickHandle(
        act_seq,
        function()
            RoleSettingCtrl.Instance:SetEncounterMineMonsterFlag(1)
            SceneCtrl.Instance:RequestEnterFb(FB_TYPE.RUSH_DUNGEON)
        end
    ) ]]
end
function RushDungeonCtrl:RecvBraveGroundRankInfo(protocol)
    -- LogError("勇闯地宫排行信息",protocol)
    self.data:SetRankInfo(protocol)
    local role_id = RoleData.Instance:GetRoleId()
    local cur_level = 0
    -- local cur_rank = 9999
    for _, v in ipairs(protocol.rank_list) do
        if v.role_id == role_id then
            cur_level = math.max(cur_level, v.reached_level)
        end
        --[[ for i = 1, v.role_info_count do
            if v.role_info[i].role_id == role_id then
                cur_level = math.max(cur_level, v.reached_level)
                -- cur_rank = math.min(cur_rank, v.rank)
            end
        end ]]
    end
    -- self.data:SetCurLevel(cur_level)
    -- ActivityData.Instance:SetActivityFinishInfo(ActType.RushDungeon, self.data.cur_level,  self.data:GetOpenLevel())
end
--打完BOSS收到 endlevel是当前通过的level
function RushDungeonCtrl:RecvBraveGroundLevelRewardInfo(protocol)
    -- LogError("这个什么玩意还有用吗",protocol)
    PublicPopupCtrl.Instance:CloseReward2()
    --这里不要用了艹
    -- self.data:SetLayerInfo(protocol)

    -- local name = string.format(Language.RushDungeon.MapName, self.data:LayerInfo().map_level + 1)
    -- SceneData.Instance:SetCustomSceneName(SceneData.Instance:SceneId(), name)
    --[[ ViewMgr:OpenView(RushDungeonName) ]]
    --如果是大于100层就弹结束界面
    --[[ if protocol.end_level > 100 then
        ViewMgr:OpenView(RushDungeonComplete)
    else
        local layer = self.data:LayerInfo()
        local items = self.data:RewardList()
        TimeHelper:AddDelayFrameTimer(
            function()
                PublicPopupCtrl.Instance:Reward2(items, string.format(Language.RushDungeon.Finish, layer.end_level))
            end,
            5
        )
    end ]]
    ViewMgr:CloseView(RushDungeonSubmit)
    RushDungeonCtrl.Instance:SendBraveGroundReq()
end
function RushDungeonCtrl:RecvBraveGroundRoute(protocol)
    -- 打开捐献界面 发送使用NPC
    self.data:CacheNpcId(protocol.param1)
    self.data:CacheObjId(protocol.param2)
    ViewMgr:OpenView(RushDungeonSubmit)
end
function RushDungeonCtrl:SendBraveGroundReq(param_t)
    param_t = param_t or {}
    local protocol = GetProtocol(CSBraveGroundReq)
    protocol.req_type = param_t.req_type or 0
    protocol.param1 = param_t.param1 or 0
    protocol.param2 = param_t.param2 or 0
    SendProtocol(protocol)
end

--检查是否是地宫NPC并修改行为
function RushDungeonCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    if obj_vo == nil then
        return
    end
    local cfg = self.data:CheckNpcInDungeon(obj_vo.id)
    -- 1恢复 2 跳层 3 选关
    if cfg then
        if self.data:MinLevel() == 0 then
            return
        end
        self.data:CacheNpcVo(obj_vo)
        self.data:CacheNpcId(obj_vo.id)
        if cfg.npc_type == 1 then
            table.insert(
                btn_list,
                NpcDialogBehBtnData.BehCfgCreate(NpcBehaviorBtn[3], BehaviorFactory.OpenView(RushDungeonBless), obj_vo)
            )
        elseif cfg.npc_type == 2 then
            table.insert(
                btn_list,
                NpcDialogBehBtnData.FuncCreate(
                    NpcBehaviorBtn[4],
                    function()
                        local param_t = {
                            req_type = BRAVE_GROUND_REQ_TYPE.ROUTE,
                            param1 = obj_vo.id,
                            param2 = obj_vo.obj_id
                        }
                        self:SendBraveGroundReq(param_t)
                    end
                )
            )
        else
            table.insert(
                btn_list,
                NpcDialogBehBtnData.FuncCreate(
                    NpcBehaviorBtn[4],
                    function()
                        --弹小界面
                        local param_t = {
                            req_type = BRAVE_GROUND_REQ_TYPE.SELECT_LEVEL,
                        }
                        local level = self.data:MinLevel() + 1
                        -- level = level > self.data:GetOpenLevel() and self.data:GetOpenLevel() or level
                        local info = {
                            content = string.format(Language.RushDungeon.FlyTip, 
                                level > Config.drop_hundred_floor_auto.other[1].max_layer and Config.drop_hundred_floor_auto.other[1].max_layer or level),
                            confirm = {
                                func = function ()
                                    self:SendBraveGroundReq(param_t)
                                    PublicPopupCtrl.Instance:CloseDialogTips()
                                end
                            },
                        }
                        PublicPopupCtrl.Instance:DialogTips(info)
                    end
                )
            )
        end
    end
end
--出副本会下发
function RushDungeonCtrl:RecvBraveGroundInfo(protocol)
    -- LogError("周通关层数",protocol)
    self.data:WeekPass(protocol.reached_level)
    self.data:SetCurLevel(protocol.reached_level)
    ActivityData.Instance:SetActivityFinishInfo(ActType.RushDungeon, self.data.cur_level, self.data:GetOpenLevel())
    -- self.data.play_times = protocol.play_times
    -- ActivityData.Instance:SetActivityFinishInfo(ActType.RushDungeon, protocol.play_times, 1)
end

function RushDungeonCtrl:RecvBraveGroundNotify(protocol)
    -- ViewMgr:OpenView(RushDungeonRank)
    -- LogError("protocol")
end

function RushDungeonCtrl:RecvBraveGroundSceneInfo(protocol)
    -- LogError("场景和杀BOSS信息", protocol)
    -- 下发如果带killboss那就显示奖励
    self.data:LayerInfo().map_level = protocol.cur_level--  - 1
    self.data:LayerInfo().end_level = protocol.cur_level-- protocol.cur_level - 1
    self.data.reward_list = {}
    -- LogError("周通过层", self.data:WeekPass())
    if protocol.is_kill_boss == 1 and protocol.cur_level > self.data:WeekPass() then--周通改成扫荡层
        self.data:SetRewardList(protocol.cur_level)
        self.data:WeekPass(protocol.cur_level)
    elseif protocol.is_kill_boss == 0 then
        ViewMgr:OpenView(RushDungeonName)
    end
    local old_buff = self.data:SceneInfo()
    if old_buff then
        MainViewData.Instance:RemoveBuff(old_buff)
        self.data:SceneInfo(nil)
    end
    if protocol.buff_id > 0 then
        local buff = {}
        buff.source = 0
        buff.type = BuffType.RUSH_DUNGEON
        buff.real_buff_id = protocol.buff_id
        buff.scene_buff = SceneData.Instance:GetMode()
        buff.buff_cfg = self.data:BuffCfg(buff.real_buff_id)
        local info = MainViewData.Instance:AddBuff(buff)
        self.data:SceneInfo(info)
    end
end

function RushDungeonCtrl:OnBraveGroundUseItemInfo(protocol)
    self.data:SetBraveGroundUseItemInfo(protocol)
end
-- 9108搭配使用
function RushDungeonCtrl:OnBraveGroundTeamInfo(protocol)
    -- LogError("队伍信息",protocol)
    self.data:SetTeamInfo(protocol)
end
function RushDungeonCtrl:OnBraveGroundRoleInfo(protocol)
    -- LogError("扫荡奖励",protocol)
    --sao_dang_reward_level
    --is_retch_sao_dang_reward
    self.data:SetHistoryLevel(protocol.max_reached_level)
    --根据当前等级和历史层级判断 扫荡配置
    --[[ local level = RoleData.Instance:GetRoleLevel()
    local temp = 1
    local temp_index = 1
    for index, value in ipairs(self.data.mop_up_list) do
        if level >= value.role_level and protocol.sao_dang_reward_level >= value.level then
            temp = value.level
            temp_index = index
        end
    end
    if temp == 1 then
        temp = self.data.mop_up_list[1].level
    end
    self.data:SetMopUpReward(protocol.is_retch_sao_dang_reward)
    self.data:SetMopUpShowLevel(temp)
    self.data:SetMopUpShowIndex(temp_index)
    self.data:SetMopUpLevel(protocol.sao_dang_reward_level)
    self.data:SetMopUpIndex(temp_index)
    if protocol.is_retch_sao_dang_reward == 1 then
        self.data:SetCurLevel(protocol.sao_dang_reward_level)
        ActivityData.Instance:SetActivityFinishInfo(ActType.RushDungeon, self.data.cur_level, self.data:GetOpenLevel())
    end ]]
end
function RushDungeonCtrl:FlushRewardRemind()
    return self.data:GetRewardNum()
end
function RushDungeonCtrl:OnBraveGroundCommonInfo(protocol)
    --地宫重置时世界等级
    self.data:SetWorldLevel(protocol.world_level)
    -- LogDG("地宫当前重置世界等级", protocol.world_level)
end