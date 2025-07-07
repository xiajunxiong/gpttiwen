VIEW_REQ("modules/period_activity_act/rabbit_race/rabbit_race_view")
FILE_REQ("modules/period_activity_act/rabbit_race/rabbit_race_data")

RabbitRaceCtrl = RabbitRaceCtrl or BaseClass(BaseCtrl)

-- gm命令 ：tuzikuaipao:0 0 跳过30秒阶段

RabbitRaceCtrl.ReqType = {
    Set = 0,        -- 放置兔子 p1,p2,p3赛道的兔子类型
    Operate = 1,    -- 操作兔子 p1 leixing ,p2 兔子id
    SetOk = 2,      -- 布局就绪
    Leave = 3,      -- 离开
}

-- 玩法阶段
RabbitRaceCtrl.Phase = {
    Set = 1,    -- 放置
    Update = 2, -- 调整
    Gameing = 3,-- 比赛进行中
    StartRun = 4,   -- 开跑
}

-- 鼓励方式
RabbitRaceCtrl.Enc = {
    Praise = 0, -- 赞扬
    Goad = 1,   -- 激将
    Food = 2,   -- 喂萝卜
}

function RabbitRaceCtrl:__init()
	if RabbitRaceCtrl.Instance ~= nil then
		Debuger.LogError("[RabbitRaceCtrl] attempt to create singleton twice!")
		return
	end
	RabbitRaceCtrl.Instance = self
	self.data = RabbitRaceData.New()
    self:RegisterAllProtocols()

    self.is_open_result = false  -- 是否打开过结算界面

    self.is_jump_set = false -- 用于跳过调整阶段第一下的设置

    SceneData.Instance.base:Care(BindTool.Bind(self.CheckCreateNPC,self), "scene_id")
end

function RabbitRaceCtrl:OnInit()
    -- 活动开启监听
    PeriodActivityData.Instance:CustomCreateHandle(Mod.PeriodActivity.RabbitRace, function()
        self:CheckCreateNPC()
        return true
    end)
    -- 活动结束监听
    PeriodActivityData.Instance:CustomDeleteHandle(Mod.PeriodActivity.RabbitRace, function()
        self:CheckCreateNPC()
        return true
    end)

    Remind.Instance:Register(Mod.PeriodActivity.RabbitRace, self.data.reward_times_data, function()
        local num = self.data:RemindNum()
        return num
    end)


    -- npc按钮
    BehaviorData.Instance:CustomNpcTalkBtnsId(self.data:GetNpcId(), BindTool.Bind(self.SetupBehaviourBtn, self))
    ActivityRandData.Instance:CustomClickModHandle(Mod.PeriodActivity.RabbitRace, BindTool.Bind1(self.OnClickActEvent, self))
    PeriodActivityData.Instance:CustomTimesHandle(Mod.PeriodActivity.RabbitRace, function()
        return self.data.reward_times_data.max_count - self.data.reward_times_data.cur_count, self.data.reward_times_data.max_count
    end)
end

function RabbitRaceCtrl:__delete()
    self:UnRegisterAllProtocols()
    RabbitRaceCtrl.Instance = nil
end

function RabbitRaceCtrl:OnUnloadGameLogic()
    
end

function RabbitRaceCtrl:CheckCreateNPC()
    local npc_id = self.data:GetNpcId()
    if self.data:IsOpen() then
        SceneCtrl.Instance:AddNpc(npc_id)
    else
        SceneCtrl.Instance:DeleteNpc(npc_id)
    end
end

function RabbitRaceCtrl:OnClickActEvent()
     -- 判断是否开启
     if not self.data:IsOpen() then
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        return
    end
    SceneLogic.Instance:AutoToNpc(self.data:GetNpcId(), nil, false, true)
end

function RabbitRaceCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSRabbitRaceReq)
    self:RegisterProtocol(SCRabbitRaceSceneInfo, "OnSCRabbitRaceSceneInfo")
    self:RegisterProtocol(SCRabbitRaceOtherInfo, "OnSCRabbitRaceOtherInfo")
    self:RegisterProtocol(SCRabbitRestInfo, "OnSCRabbitRestInfo")
    self:RegisterProtocol(SCRabbitTaceOperateResult, "OnSCRabbitTaceOperateResult")
    self:RegisterProtocol(SCRabbitRaceObjInfo, "OnSCRabbitRaceObjInfo")
    self:RegisterProtocol(SCRabbitRaceRestEnd, "OnSCRabbitRaceRestEnd")
    self:RegisterProtocol(SCRabbitRacePropInfo, "OnSCRabbitRacePropInfo")
    self:RegisterProtocol(SCRabbitRaceObjSpeedChange, "OnSCRabbitRaceObjSpeedChange")
    self:RegisterProtocol(SCRabbitRaceEnd, "OnSCRabbitRaceEnd")
end

function RabbitRaceCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(CSRabbitRaceReq)
    self:UnRegisterProtocol(SCRabbitRaceSceneInfo)
    self:UnRegisterProtocol(SCRabbitRaceOtherInfo)
    self:UnRegisterProtocol(SCRabbitRestInfo)
    self:UnRegisterProtocol(SCRabbitTaceOperateResult)
end

function RabbitRaceCtrl:SendRabbitRaceReq(req_type,p1,p2,p3)
	local protocol = GetProtocol(CSRabbitRaceReq)
    protocol.req_type = req_type
    protocol.p1 = p1 or 0
    protocol.p2 = p2 or 0
    protocol.p3 = p3 or 0
    SendProtocol(protocol)
end

function RabbitRaceCtrl:SaveRabbitInfo()
    local sel_data = self.data.select_rabbit_data
    self:SendRabbitRaceReq(RabbitRaceCtrl.ReqType.Set, sel_data[0] or 0, sel_data[1] or 0, sel_data[2] or 0)
end

function RabbitRaceCtrl:OnSCRabbitRaceSceneInfo(protocol)
    local info = protocol.info
    self.data.base_data:Set(info)
    local phase = info.phase
    if phase == RabbitRaceCtrl.Phase.Set then
        self.is_jump_set = true
        ViewMgr:OpenView(RabbitRaceReadyView)
    elseif phase == RabbitRaceCtrl.Phase.Update then
        if not self.is_jump_set then
            ViewMgr:OpenView(RabbitRaceReadyView)
            local role_id = RoleData.Instance:GetRoleId()
            local set_info = info.role_set_info[role_id] or {}
            for i,v in pairs(set_info.tow_set_rabbit) do
                if v > 0 and v < 4 then
                    self.data:SetSelectRibbit(i,v)
                else
                    self.data:SetSelectRibbit(i,nil)
                end
            end
        else
            self.is_jump_set = false
        end
    elseif phase == RabbitRaceCtrl.Phase.Gameing then
        local role_id = RoleData.Instance:GetRoleId()
        local set_info = info.role_set_info[role_id] or {}
        for i,v in pairs(set_info.tow_set_rabbit) do
            if v > 0 and v < 4 then
                self.data:SetSelectRibbit(i,v)
            else
                self.data:SetSelectRibbit(i,nil)
            end
        end
    end

    -- 检查是否结束
    self:CheckOver(info.results)
end

-- 检查结束
function RabbitRaceCtrl:CheckOver(results)
    results = results or self.data.base_data.results
    local count = 0
    for i,v in pairs(results) do
        if v > 0 and self.data.track_move_info[i] then
            count = count + 1
            if self.data.win_list[i] == nil then
                self.data.win_list[i] = v
                self:DesEvent(i,v)
            end
        end
    end
    if self.data.win_role_id ~= nil or (count >= 3 and not self.is_open_result) then
        Invoke(function ()
            if not RabbitRaceData.Instance.is_main then
                TeamCtrl.Instance:SendTemporarilyPartReq()
            end
            if not TableIsEmpty(self.data.base_data) then
                ViewMgr:OpenView(RabbitRaceOverView)
            end
        end,2)
        self.is_open_result = true
        return true
    else
        return false
    end
end

-- 到达终点事件
function RabbitRaceCtrl:DesEvent(track,role_id)
    self:PlaySceneDesEffect(track,role_id)
end

function RabbitRaceCtrl:OnSCRabbitRaceOtherInfo(protocol)
    self.data.reward_times_data.cur_count = protocol.reward_times
end

-- 休息
function RabbitRaceCtrl:OnSCRabbitRestInfo(protocol)
    if self.data.win_role_id ~= nil then
        return
    end

    local info = protocol.info
    local obj_id = info.obj_id
    local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(info.obj_id)
    if obj then
        obj.vo.is_rest = true   -- 防止终点停止
    end
    if info.role_id == RoleData.Instance:GetRoleId() then
        local bubble = self.data:GetRestBubble()
        if obj then
            local npc_board = MainViewData.Instance:GetBoard([[obj_id]],obj_id)
            if npc_board then
                npc_board:SetBubble(true, bubble)
            end
            self.data:AddObjData(info.obj_id, 0)

            local rabbit_id = self.data:GetTrackRibbit(info.track_type)
            local rabbit_cfg = self.data:GetRabbitCfg(rabbit_id)
            local track_name = ColorStr(Language.RabbitRun.WayName[info.track_type], COLORSTR.Yellow6)
            PublicPopupCtrl.Instance:Center(string.format(Language.RabbitRun.LazyFloat,ColorStr(rabbit_cfg.name,COLORSTR.Yellow6),track_name))
        else
            self.data.obj_bubble_data[obj_id] = bubble
        end
    end
end

-- 休息结束
function RabbitRaceCtrl:OnSCRabbitRaceRestEnd(protocol)
    local info = protocol.info
    local obj_id = info.obj_id
    local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(info.obj_id)
    if obj and info.role_id == RoleData.Instance:GetRoleId() then
        local npc_board = MainViewData.Instance:GetBoard([[obj_id]],obj_id)
        local bubble = self.data:GetRestEndBubble()
        if npc_board and obj.vo.is_rest == true then
            npc_board:SetBubble(true, bubble)
            Invoke(function ()
                npc_board:SetBubble(false)
            end,2)
        end
        self.data:AddObjData(info.obj_id, 1)
        obj.vo.is_rest = false
        self.data.oper_record = {}
        RabbitRaceData.Instance.oper_record = nil
    end
    self.data.obj_bubble_data[obj_id] = nil
end

-- 鼓励结果
function RabbitRaceCtrl:OnSCRabbitTaceOperateResult(protocol)
    local info = protocol.info
    local obj_id = info.obj_id
    local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(info.obj_id)
    if obj then
        local npc_board = MainViewData.Instance:GetBoard([[obj_id]],obj_id)
        local bubble = self.data:GetEncBubble(info.en_type, info.is_succ)
        if npc_board then
            npc_board:SetBubble(true, bubble)
        end
        if info.is_succ == 1 then
            self.data:AddObjData(info.obj_id, 1)
            obj.vo.is_rest = false
            Invoke(function ()
                if npc_board then
                    npc_board:SetBubble(false)
                end 
            end,2)
        end
        RabbitRaceData.Instance.oper_record = nil
    end
end

function RabbitRaceCtrl:OnSCRabbitRaceObjInfo(protocol)
    local info = protocol.info
    local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(info.obj_id)
    local client_pos = SceneCtrl.Instance:CurScene():PosToClint(info.pos_x, info.pos_y)
    if obj then
        --LogError("？？？？？？？？？",info.role_id, info.obj_id)
		--obj:MoveTo(client_pos, false)
    else
        local npc_id = self.data:GetRabbitNpcId(info.role_id,info.rabbit_id)
        local co = NpcHelper.NpcCo(npc_id)
        local vo = CommonStruct.NpcVo(co)

        if info.distance > 0 then
            vo.real_x = info.pos_x + (math.cos(info.dir_y) * info.distance)
            vo.real_y = info.pos_y + (math.sin(info.dir_y) * info.distance)
        end

        local delay_time = TimeCtrl.Instance:GetServerTime() - info.server_timestamp

        vo.x = info.pos_x
        vo.y = info.pos_y
        
        vo.distance = info.distance
        vo.move_speed = info.move_speed
        vo.scene_id = info.scene_id
        vo.dir_y = 90--info.dir_y
        vo.obj_id = info.obj_id
        vo.alway_show = false
        vo.is_server = true
        vo.obj_type = SceneObjType.RabbitRace
        vo.npc_id = npc_id
        vo.track = self.data:GetRabbitOnTrack(info.rabbit_id)
        local side_info = RabbitRaceData.Instance:GetSideInfo()
        local color
        if self.data.is_main then
            if info.role_id == RoleData.Instance:GetRoleId() then
                color = COLORSTR.Green8
            else
                color = COLORSTR.Red12
            end
        else
            if info.role_id == RoleData.Instance:GetRoleId() then
                color = COLORSTR.Red12
            else
                color = COLORSTR.Green8
            end
        end
        vo.name = ColorStr(vo.name, color)
        if self.data:IsSelfRabbitNpc(npc_id) then
            vo.name = ColorStr(RoleData.Instance:GetRoleName(), COLORSTR.White) .. "\n" .. vo.name
        else
            vo.name = ColorStr(side_info.name, COLORSTR.Red8) .. "\n" .. vo.name
        end
        if DEBUG_MODE then
            --vo.name = vo.name .. ColorStr(string.format("赛道%s的%s",vo.track,info.obj_id),COLORSTR.Red8)
        end

        -- 获取兔子终点位置
        vo.des_pos = RabbitRaceData.Instance:GetTrackDes(vo.track, info.role_id)
        --LogError("终点位置", vo.des_pos)

        VoMgr:AddSNpc(vo)
        --VoMgr:AddGCObj(vo)
        self.data:AddObjData(info.obj_id, 1)
        --LogError("创建",vo.obj_id, npc_id)
    end
end

-- 胡萝卜剩余次数
function RabbitRaceCtrl:OnSCRabbitRacePropInfo(protocol)
    self.data.prop_info.use_times = protocol.use_prop_times
end

-- 对象速度变化
function RabbitRaceCtrl:OnSCRabbitRaceObjSpeedChange(protocol)
    local obj_id = protocol.obj_id
    local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(obj_id)
    if obj then
        obj:SetMoveSpeed(protocol.move_speed * 0.2)
    end
end

-- 游戏结束
function RabbitRaceCtrl:OnSCRabbitRaceEnd(protocol)
    self.data.win_role_id = protocol.win_role_id
    self:CheckOver()
end


function RabbitRaceCtrl:SetupBehaviourBtn(btn_list,obj_vo)
    if not obj_vo then
        return
    end
    local npc_id = obj_vo.id
	if npc_id == self.data:GetNpcId() then

        -- 首先判断活动是否开启 ** 
        if not self.data:IsOpen() then
            return
        end
        
        -- 玩法入口
        local button = NpcDialogBehBtnData.FuncCreate(
            Language.RabbitRun.Btn1,function()
				local target_id = self.data:TeamType()
				-- 是否是组队，且要两个人
                if not TeamData.Instance:InTeam() then
                    PublicPopupCtrl.Instance:AlertTip(Language.RabbitRun.Tip2, function ()
                        local co = TeamData.Instance.team_goal_list[target_id]
                        TeamData.Instance.cur_sel_target = co
                        TeamCtrl.Instance:SendCreateTeam()
                        TeamCtrl.Instance:SendTeamRecruit()
                    end)    
                    return
                end
                local my_info = TeamData.Instance:GetTeamInfo()

                local now_target = my_info.info.team_type
                -- 队伍类型不匹配
                if now_target ~= target_id then
                    local co = TeamData.Instance.team_goal_list[target_id]
                    TeamCtrl.Instance:SendChangeTeamLimit(co.low_level, co.high_level, target_id)
                end

                local member_count = 0
                local ren_ji_index_list = {}
                -- 剔除伙伴和机器人
                for i,v in pairs(my_info.member_list) do
                    if v.info and v:IsPartner() then
                        PartnerCtrl.Instance:SendPartnerFight(v.info.uid)
                    elseif v.info and v.info.uid < 100000 then
                        table.insert(ren_ji_index_list,v.info.member_index)
                    elseif v.info then
                        member_count = member_count + 1
                    end
                end
                table.sort(ren_ji_index_list,function (a,b)
                    return a > b
                end)
                for i,v in pairs(ren_ji_index_list) do
                    TeamCtrl.Instance:SendKickOutOfTeamReq(v)
                end

                if member_count ~= 2 then
                    if member_count < 2 then
                        PublicPopupCtrl.Instance:AlertTip(Language.RabbitRun.Tip2, function ()
                            TeamCtrl.Instance:SendTeamRecruit()
                        end)
                    else
                        PublicPopupCtrl.Instance:Error(Language.RabbitRun.Tip4)
                    end
                    return
                end
                -- 玩之前清下数据
                RabbitRaceData.Instance:ClearData()
                SceneCtrl.Instance:RequestEnterFb(FB_TYPE.RABBIT_RACE)
            end)
		button:SetIconType(NpcIconType.Act)
        table.insert(btn_list, button)
        
        -- 玩法说明
        local button = NpcDialogBehBtnData.FuncCreate(
            Language.RabbitRun.Btn2,function()
				ViewMgr:OpenView(RabbitRaceDescView)
            end)
        table.insert(btn_list, button)
    end

end


-- 播放场景终点特效
function RabbitRaceCtrl:PlaySceneDesEffect(track,role_id)
    local scene_id = RoleData.Instance.base_data.scene_id
    if scene_id ~= self.data:SceneId() then
        LogError("播放特效失败",track,role_id,scene_id)
        return
    end
    local cfg = self.data:GetTrackCfg(track)
    local x
    local z

    if role_id == RoleData.Instance:GetRoleId() then
        if self.data.is_main then
            x = cfg.fireworks_x0
            z = cfg.fireworks_y0
        else
            x = cfg.fireworks_x1
            z = cfg.fireworks_y1
        end
    else
        if self.data.is_main then
            x = cfg.fireworks_x1
            z = cfg.fireworks_y1
        else
            x = cfg.fireworks_x0
            z = cfg.fireworks_y0
        end
    end
    local scene = SceneCtrl.Instance:CurScene()
    local y = scene:GetPosHeight(x,z)
    local pos = Vector3.New(x,y,z)
    SE:Play(SceneEffectConfig.EffectList.RabbitRaceDes, pos)
    --LogError("播放了特效",track,role_id)
end

function RabbitRaceCtrl.EnterFB()
    SceneCtrl.Instance:RequestEnterFb(FB_TYPE.RABBIT_RACE)
end

