GotoSchoolLogic = GotoSchoolLogic or BaseClass()

function GotoSchoolLogic:__init()
    if GotoSchoolLogic.Instance ~= nil then
        Debuger.LogError("[GotoSchoolLogic] attempt to create singleton twice!")
        return
    end
    GotoSchoolLogic.Instance = self
    self.data = GotoSchoolData.Instance
    self.obj_id = CUSTOM_OBJID.GOTO_SCHOOL
    self.fail_obj_id = self.obj_id + 1
    self.red_cfg = Config.go_to_school_auto.range[1]
    self.green_cfg = Config.go_to_school_auto.range[2]
    self.check_time = 0.1   -- 隔多长时间检测一次
    self.end_talk = Config.go_to_school_auto.other[1].end_talk
    self.fail_reward = Config.go_to_school_auto.reward[1].reward_show
    self.succ_reward = Config.go_to_school_auto.reward[2].reward_show
end

function GotoSchoolLogic:SetChildCfg(cfg)
    self.child_cfg = cfg
end

function GotoSchoolLogic:Reset()
    self.green_time = 0     -- 离开了绿圈多久
    self.red_time = 0       -- 进入了红圈多久
    if not self.child_cfg then
        self.child_cfg = self.data:GetRandChildCfg()    -- 小孩配置
    end
    LogDG(self.child_cfg)

    local route_seq_list = Split(self.child_cfg.route_seq, "|")
    local index = GameMath.Ramdon(1, #route_seq_list)
    self.route_seq = tonumber(route_seq_list[index])    -- 路线
    self.route_position = 1 -- 目标路线点
    self.is_fail = false    -- 是否护送失败
    self.is_succ = false    -- 是否护送成功
    self.fail_cfg = nil     -- 失败的配置
    self.end_route_cfg = self.data:GetEndRouteCfg(self.route_seq)   -- 终点配置
end

-- 前期准备
function GotoSchoolLogic:Ready()
    -- 重置数据
    self:Reset()
    -- 创建小孩
    local npc_id = self.child_cfg.NPC_id
    local co = NpcHelper.NpcCo(npc_id)
    local vo = CommonStruct.NpcVo(co)
    vo.move_speed = self.child_cfg.speed
    local route_cfg = self.data:GetRouteCfg(self.route_seq, self.route_position)
    vo.obj_type = SceneObjType.SchoolBoy
    vo.npc_id = npc_id
    vo.scene_id = self.data:SceneId()
    vo.obj_id = self.obj_id
    vo.x = route_cfg.pos_x
    vo.y = route_cfg.pos_y
    vo.alway_show = true
    vo.is_server = false
    vo.name = self.child_cfg.name
    VoMgr:AddGCObj(vo)

    -- 创建终点特效 （暂无）

    -- 切换场景的特效
    --SceneData.Instance.change_scene_effect.id = 30000000

    -- 5秒后开始游戏
    self.time_ht_2 = Invoke(function ()
        self:Start()
    end,5)
    GotoSchoolData.Instance:SetTip(GotoSchoolCtrl.Tip.Time)
    ViewMgr:OpenView(GotoSchoolGameView)
    PublicPopupCtrl.Instance:Center("请保持在绿色区域范围内护送，进入红色区域<color=#" ..COLORSTR.Yellow6 .. ">3</color>秒后会被孩子发现")
end

-- 开始游戏
function GotoSchoolLogic:Start()
    -- 创建范围特效
    local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(self.obj_id)
    if obj then
        self.effect_handle = SE:Play(SceneEffectConfig.EffectList.SchoolRangeGreen, nil, nil, obj.root_node)
        Runner.Instance:RunUntilTrue(function (  )
            if not self.effect_handle then
                return true
            end
            if self.effect_handle.obj ~= nil then
                self.effect_handle.obj.transform.localScale = Vector3.New(9,1,9)
                return true
            end
        end)

        self.effect_handle_red = SE:Play(SceneEffectConfig.EffectList.SchoolRangeRed, nil, nil, obj.root_node)
        Runner.Instance:RunUntilTrue(function (  )
            if not self.effect_handle_red then
                return true
            end
            if self.effect_handle_red.obj ~= nil then
                self.effect_handle_red.obj.transform.localScale = Vector3.New(9 / 4,1,9 / 4)
                self.effect_handle_red.obj.transform.localPosition = Vector3.New(0,0.1,0)
                return true
            end
        end)
    end

    self:NextRoute()
    self.time_ht = TimeHelper:AddRunTimer(function()
		self:CheckUpdate()
    end, self.check_time)
end

function GotoSchoolLogic:CheckUpdate()
    local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(self.obj_id)
    if not obj then
        return
    end
    if self.is_fail then
        return
    end
    local dis = MainPlayer:GetDistance(obj:GetPos())
    if dis > self.green_cfg.red_circle then
        -- 绿圈外
        if self.green_time == 0 then
            PublicPopupCtrl.Instance:Center("您和孩子的距离过远，请返回绿色区域范围内")
        end
        self.green_time = self.green_time + self.check_time
        self.red_time = 0
    end
    if dis <= self.green_cfg.red_circle and dis > self.red_cfg.red_circle then
        -- 绿圈内
        self.green_time = 0
        self.red_time = 0
    end
    if dis >= 0 and dis <= self.red_cfg.red_circle then
        -- 红圈内
        if self.red_time == 0 then
            PublicPopupCtrl.Instance:Center("即将被孩子发现，请返回绿色区域范围内）")
        end
        self.red_time = self.red_time + self.check_time
    end

    if self.green_time >= self.green_cfg.into_red_circle then
        self.fail_cfg = self.green_cfg
        self:GameFail()
    elseif self.red_time >= self.red_cfg.into_red_circle then
        self.fail_cfg = self.red_cfg
        self:GameFail()
    end
end

-- 战斗结束
function GotoSchoolLogic:BattleEnd(is_win)
    LOG("战斗结束",is_win)
    if self.fail_cfg then
        local talk_id
        if is_win then
            self.is_fight_succ = true
            talk_id = self.fail_cfg.succ_talk
        else
            self.is_fight_succ = false
            talk_id = self.fail_cfg.fail_talk
        end

        NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id},function ()
            SceneCtrl.Instance:RequestLeaveFb()
        end)
    end
end

-- 护送失败
function GotoSchoolLogic:GameFail()
    LOG("护送失败",self.fail_cfg)
    TimeHelper:CancelTimer(self.time_ht)
    self.is_fail = true
    local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(self.obj_id)
    if obj then
        obj:StopMove()
        MainPlayer:StopMove()
    end
    if self.fail_cfg then
        --生成一个npc，然后直接进入对话，对话完毕进入战斗,战斗结束后再进入对应的结算对话，发放奖励
        local npc_id = self.fail_cfg.trigger_npc
        local co = NpcHelper.NpcCo(npc_id)
        local vo = CommonStruct.NpcVo(co)
        vo.move_speed = 0
        local pos = MainPlayer:GetPos() + MainPlayer.root_node.transform.forward * 1
        vo.obj_type = SceneObjType.SchoolBoy
        vo.npc_id = npc_id
        vo.scene_id = self.data:SceneId()
        vo.obj_id = self.fail_obj_id
        vo.x = pos.x
        vo.y = pos.z
        vo.alway_show = true
        vo.is_server = false
        vo.look_main = true
        vo.is_can_click = true
        VoMgr:AddGCObj(vo)

        local talk_id = self.fail_cfg.trigger_talk
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id},function ()
            -- 对话结束进入战斗
            GotoSchoolCtrl.Instance:SendReq(GotoSchoolCtrl.ReqType.Fight, self.fail_cfg.red_circle_monster_group)
        end)
    end
end

-- 护送成功
-- 3、孩子到达终点（白色圆圈）后消失，弹出对话，任务进度自动完成无缝退出场景，自动寻路活动NPC，
-- 对话完后弹出文字（任务完成）并自动获得奖励。获得奖励弹窗且飘字。
function GotoSchoolLogic:GameSucc()
    LOG("护送成功",self.is_fail)
    if self.is_fail then
        return
    end
    self.is_succ = true
    -- 对话结束退出副本
    NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.end_talk},function ()
        LogDG("退出副本")
        SceneCtrl.Instance:RequestLeaveFb()
        --SceneData.Instance.change_scene_effect.id = 30000000
    end)
    
end

-- 相当于离开场景
function GotoSchoolLogic:Destroy()
    TimeHelper:CancelTimer(self.time_ht)
    TimeHelper:CancelTimer(self.time_ht_2)
    
    SE:Stop(self.effect_handle)
    SE:Stop(self.effect_handle_red)
    VoMgr:DelGCObj(self.obj_id,true)
    VoMgr:DelGCObj(self.fail_obj_id,true)
    self.effect_handle = nil

    ViewMgr:CloseView(GotoSchoolGameView)

    if self.is_succ then
        self.is_succ = false
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.end_talk},function ()
            GotoSchoolData.Instance:SetTip(GotoSchoolCtrl.Tip.TaskFinish)
            ViewMgr:OpenView(GotoSchoolGameView)

            Invoke(function ()
                ViewMgr:CloseView(GotoSchoolGameView)
                if self.data:GetRemainRewardCount() > 0 then
                    GotoSchoolCtrl.Instance:SendReq(GotoSchoolCtrl.ReqType.Succ)
                    local reward_list = DataHelper.FormatItemList(self.succ_reward)
                    ViewMgr:OpenView(GiftOpenView,{item_list = reward_list,gift_type = GIFT_OPEN_TYPE.SPECIAL}) 
                end
            end,2)
        end)
    end

    if self.is_fight_succ then
        self.is_fight_succ = false
        GotoSchoolData.Instance:SetTip(GotoSchoolCtrl.Tip.TaskFinish)
        ViewMgr:OpenView(GotoSchoolGameView)
        Invoke(function ()
            ViewMgr:CloseView(GotoSchoolGameView)
            if self.data:GetRemainRewardCount() > 0 then
                GotoSchoolCtrl.Instance:SendReq(GotoSchoolCtrl.ReqType.FightReward)
                local reward_list = DataHelper.FormatItemList(self.fail_reward)
                ViewMgr:OpenView(GiftOpenView,{item_list = reward_list,gift_type = GIFT_OPEN_TYPE.SPECIAL}) 
            end
        end,2)
    end
end


-- 前往下一个目的地
function GotoSchoolLogic:NextRoute()
    if self.route_position == self.end_route_cfg.position then
        --说明走到了终点
        self:GameSucc()
        return
    end
    self.route_position = self.route_position + 1
    local cfg = self.data:GetRouteCfg(self.route_seq, self.route_position)
    LOG("路线点：",cfg)
    local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(self.obj_id)
    if obj then
        local scene = SceneCtrl.Instance:CurScene()
        local y = scene:GetPosHeight(cfg.pos_x, cfg.pos_y)
        local pos = Vector3.New(cfg.pos_x, y ,cfg.pos_y)
        obj:MoveTo(pos, false, BindTool.Bind(self.OnMoveTarget,self))
    end
end

-- 到达一个点
function GotoSchoolLogic:OnMoveTarget()
    if self.is_fail then
        return
    end
    -- 播放气泡框
    local npc_board = MainViewData.Instance:GetBoard([[obj_id]],self.obj_id)
    if npc_board then
        local cfg = self.data:GetRouteCfg(self.route_seq, self.route_position)
        local bubble = NpcDialogData.Instance:RandBubbleById(cfg.bubble_id)
        npc_board:SetBubble(true, bubble)
        Invoke(function ()
            if npc_board then
                npc_board:SetBubble(false)
            end 
        end,cfg.bubble_time)
    end
    self:NextRoute()
end

GotoSchoolObjCh = GotoSchoolObjCh or BaseClass(SceneCh)

function GotoSchoolObjCh:__init()
    self.vo = nil
end

function GotoSchoolObjCh:InitEnd()
    if self.vo.move_speed and self.vo.move_speed > 0 then
        self:SetMoveSpeed(self.vo.move_speed)
    end
    if self.vo.look_main then
        TimeHelper:AddRunFrameTimer(function ()
            self:LookAt()
        end,1,1)
    end
end

function SceneObj:LookAt(obj)
    if obj == nil then
        obj = MainPlayer
    end
    local tar_obj = obj.root_node
    local forward = tar_obj:GetPosition() - self.root_node:GetPosition()
    self:SetForward(forward)
end

--模型点击操作
function GotoSchoolObjCh:InteractionBehavior()
    if self.vo.is_can_click then
        SceneCh.InteractionBehavior(self)
    end
end