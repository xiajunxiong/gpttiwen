--===========================兔子快跑===========================
RabbitRaceData = RabbitRaceData or BaseClass()

function RabbitRaceData:__init()
    if RabbitRaceData.Instance ~= nil then
        Debuger.LogError("[RabbitRaceData] attempt to create singleton twice!")
        return
    end
    RabbitRaceData.Instance = self
    self.cfg = Config.rabbit_run_auto
    self.other = self.cfg.other[1]

    self.select_rabbit_data = SmartData.New()

    self.base_data = SmartData.New()

    self.reward_times_data = SmartData.New({
        cur_count = 0,
        max_count = self.other.reward_times,
    })    -- 奖励领取次数

    self.prop_info = SmartData.New({
        use_times = 0,  -- 胡萝卜使用次数
    })

    self.rabbit_npc = {
        [1] = 73708,
        [2] = 73709,
        [3] = 73710,
        [4] = 73705,
        [5] = 73706,
        [6] = 73707,
    }

    self.obj_list = {
        -- [obj_id] = 0|1是否再跑
    }

    -- 赛道胜利数据
    self.win_list = {

    }

    -- 存入冒泡数据
    self.obj_bubble_data = {}

    self.is_main = false     -- 是不是队长

    self.is_ok = false      -- 是否就绪

    self.side_info = nil    -- 对手信息

    self.reward_list = {}   -- 获得到的奖励列表

    self.track_move_info = {}   -- 赛道移动信息

    self.win_role_id = nil      -- 胜利的玩家

    self.oper_obj_id = nil      -- 当前操作的兔子对象id

    self.oper_record = nil       -- 当前操作记录，赞扬鼓励那些
end

function RabbitRaceData:ClearData()
    self.base_data:Set({})
    self.side_info = nil
    self.prop_info.use_times = 0
    RabbitRaceCtrl.Instance.is_open_result = false
    self.track_move_info = {}
    self.select_rabbit_data:Set({})
    self.win_role_id = nil
    self.obj_bubble_data = {}
    self:ClearObj()
    self.oper_record = nil
end

function RabbitRaceData:GetNpcId()
    return self.cfg.other[1].npc_id
end

function RabbitRaceData:SceneId()
    return self.cfg.other[1].scene_id
end

function RabbitRaceData:TeamType()
    return self.cfg.other[1].target_id
end

function RabbitRaceData:GetRabbitNpcId(role_id,rabbit_id)
    local self_role_id = RoleData.Instance:GetRoleId()
    if self_role_id == role_id then
        if self.is_main then
            return self.rabbit_npc[rabbit_id]
        else
            return self.rabbit_npc[rabbit_id + 3]
        end
    else
        if self.is_main then
            return self.rabbit_npc[rabbit_id + 3]
        else
            return self.rabbit_npc[rabbit_id]
        end
    end
end

-- 获取兔子配置
function RabbitRaceData:GetRabbitCfg(rabbit_id)
    for i,v in pairs(Config.rabbit_run_auto.rabbit) do
        if v.rabbit == rabbit_id then
            return v
        end
    end
end

-- 获取终点位置
function RabbitRaceData:GetTrackDes(track_id, role_id)
    local cfg = self:GetTrackCfg(track_id)
    local s_x
    local s_y
    local self_role_id = RoleData.Instance:GetRoleId()
    if self_role_id == role_id then
        if self.is_main then
            s_x = cfg.end_x0
            s_y = cfg.end_y0
        else
            s_x = cfg.end_x1
            s_y = cfg.end_y1
        end
    else
        if self.is_main then
            s_x = cfg.end_x1
            s_y = cfg.end_y1
        else
            s_x = cfg.end_x0
            s_y = cfg.end_y0
        end
    end
    local client_pos = SceneCtrl.Instance:CurScene():PosToClint(s_x, s_y)
    return client_pos
end

-- 获取赛道配置
function RabbitRaceData:GetTrackCfg(track_id)
    for i,v in pairs(self.cfg.track) do
        if v.track == track_id then
            return v
        end
    end
end

-- 清空选择的数据
function RabbitRaceData:ClearSelectData()
    self.select_rabbit_data:Set({})
end

-- 是否每个赛道都选了兔子
function RabbitRaceData:IsFullSelectData()
    for i = 0, 2 do
        if not self.select_rabbit_data[i] then
            return false
        end
    end
    return true
end

function RabbitRaceData:SetSelectRibbit(track_type, rabbit_type, is_send)
    self.select_rabbit_data[track_type] = rabbit_type
    if is_send == true then
        RabbitRaceCtrl.Instance:SaveRabbitInfo()
    end
end

-- 获取赛道上的兔子id
function RabbitRaceData:GetSelectRibbit(track_type)
    return self.select_rabbit_data[track_type]
end

-- 获取兔子在哪个赛道上
function RabbitRaceData:GetSelectTrack(rabbit_type)
    for i,v in pairs(self.select_rabbit_data or {}) do
        if v == rabbit_type then
            return i
        end
    end
end

-- 获取对手信息 -- 在活动玩法进行中时原则上不能为空
function RabbitRaceData:GetSideInfo()
    if not TeamData.Instance:InTeam() then
        return
    end
    local my_info = TeamData.Instance:GetTeamInfo()
    if TableIsEmpty(my_info) then
        return self.side_info
    end

    local self_id = RoleData.Instance:GetRoleId()
    for i,v in pairs(my_info.member_list) do
        if v.info and v.info.uid ~= self_id then
            self.side_info = v.info
            return v.info
        end
    end
    return self.side_info
end

-- 获取比赛阶段
function RabbitRaceData:GetPhase()
    return self.base_data.phase or RabbitRaceCtrl.Phase.Set
end

-- 获取赛道上的兔子id
function RabbitRaceData:GetTrackRibbit(track_type, role_id)
    role_id = role_id or RoleData.Instance:GetRoleId()
    if TableIsEmpty(self.base_data.role_set_info) then
        return
    end
    local info = self.base_data.role_set_info[role_id]
    if info == nil then
        for i,v in pairs(self.base_data.role_set_info) do
            if i ~= RoleData.Instance:GetRoleId() then
                info = v
                break
            end
        end
    end
    if info == nil then
        return
    end
    if self:GetPhase() == RabbitRaceCtrl.Phase.Gameing or self:GetPhase() == RabbitRaceCtrl.Phase.StartRun then
        return info.tow_set_rabbit[track_type]
    else
        return info.set_rabbit[track_type]
    end
    
end

-- 获取比赛阶段，兔子在哪条赛道上
function RabbitRaceData:GetRabbitOnTrack(rabbit_id)
    if TableIsEmpty(self.base_data.role_set_info) then
        return
    end
    for _,v in pairs(self.base_data.role_set_info) do
        for i,v2 in pairs(v.tow_set_rabbit) do
            if rabbit_id == v2 then
                return i
            end
        end
    end
end

-- 赛道胜利信息
function RabbitRaceData:IsWin(track_type, role_id)
    local role_id = role_id or RoleData.Instance:GetRoleId()
    if TableIsEmpty(self.base_data.results) then
        return 0
    end
    if self.base_data.results[track_type] == role_id then
        return 1
    else
        return 0
    end
end

-- 胡萝卜数量
function RabbitRaceData:ItemCount()
    return self.cfg.other[1].carrot_num - self.prop_info.use_times or 0
end

-- 活动是否开启
function RabbitRaceData:IsOpen()
    return PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.RabbitRace)
end

-- 通过npc获取兔子id
function RabbitRaceData:GetRabbitByNpcId(npc_id)
    for i,v in pairs(self.rabbit_npc) do
        if npc_id == v then
            return i
        end
    end
end

-- 获取休息气泡
function RabbitRaceData:GetRestBubble()
    local bubble_id = self.cfg.other[1].rest_talk
    return NpcDialogData.Instance:RandBubbleById(bubble_id)
end

-- 获取休息结束气泡
function RabbitRaceData:GetRestEndBubble()
    local bubble_id = self.cfg.other[1].rest_end_talk
    return NpcDialogData.Instance:GetBubbleData(bubble_id,1)
end

-- 获取鼓励气泡
function RabbitRaceData:GetEncBubble(enc_type, is_succ)
    for i,v in pairs(self.cfg.encourage) do
        if v.encourage == enc_type then
            local bubble_id
            if is_succ == 1 then
                bubble_id = v.succ_talk
            else
                bubble_id = v.fail_talk
            end
            return NpcDialogData.Instance:RandBubbleById(bubble_id)
        end
    end
end


-- 添加副本场景对象  is_run == 0 | 1
function RabbitRaceData:AddObjData(obj_id, is_run)
    self.obj_list[obj_id] = is_run
end

function RabbitRaceData:ClearObj()
    for i,v in pairs(self.obj_list or {}) do
        VoMgr:DelSNpc(i, true) 
    end
    self.obj_list = {}
    self.win_list = {}
end

-- 是否是兔子快跑创建的obj_id
function RabbitRaceData:IsRabbit(obj_id)
    return self.obj_list[obj_id] ~= nil
end


-- 重置是不是队长
function RabbitRaceData:ResetMain()
    if not TeamData.Instance:InTeam() then
        self.is_main = false
        return
    end
    local my_info = TeamData.Instance:GetTeamInfo()
    if TableIsEmpty(my_info) then
        self.is_main = false
        return
    end
    local self_id = RoleData.Instance:GetRoleId()
    for i,v in pairs(my_info.member_list) do
        if v.is_leader and v.info.uid == self_id then
            self.is_main = true
            return
        end
    end
    self.is_main = false
end


-- 是否是自己的兔子的npc
function RabbitRaceData:IsSelfRabbitNpc(npc_id)
    for i,v in pairs(self.rabbit_npc) do
        if v == npc_id then
            if self.is_main == true then
                return i <= 3
            else
                return i > 3
            end
        end
    end
    return false
end

-- 赛道的胜利者
function RabbitRaceData:GetTrackWinRoleId(track)
    local results = self.base_data.results or {}
    return results[track]
end


-- 手指移动结束事件
function RabbitRaceData:OnTouchEndEvent()
    if not self.oper_obj_id or self.obj_list[self.oper_obj_id] == 0 then
        return
    end
    if ViewMgr:IsOpen(RabbitRaceOperView) then
        ViewMgr:CloseView(RabbitRaceOperView)
    end
end

-- 红点逻辑
function RabbitRaceData:RemindNum()
    if not self:IsOpen() then
        return 0
    end
    if self.reward_times_data.cur_count < self.reward_times_data.max_count then
        return 1
    else
        return 0
    end
end