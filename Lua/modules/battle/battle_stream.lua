FILE_REQ("modules/battle/battle_node_event")
FILE_REQ("modules/battle/battle_behaviour_queue")

BattleStream = BattleStream or BaseClass()

function BattleStream:__init(data, displayer)
    self.data = data
    self.displayer = displayer
    self.round_phase_care = nil
    self.behaviour_queues = {}
    BattleBehaviour.displayer = self.displayer
end

function BattleStream:IsPlaying()
    return self.is_playing
end

function BattleStream:Prepare(on_complete,real_create_end)
    LogDG("创建场景对象")
    -- TeamCtrl.Instance:OnSalaryBattleStateInfo({seq = 3,salary = 80})       --本地测试代码

    local base_data = self.data:BaseData()
    self.round_phase_care = base_data:Care(BindTool.Bind(self.OnRoundPhaseChange, self), "round_phase")
    local ch_Info = self.data:ChInfo()
    local wait_count = self.data:ChInfoCount()
    local function create_end()
        wait_count = wait_count - 1
        if wait_count <= 0 then
            on_complete()
        end
    end
    for k, v in pairs(ch_Info) do
        self:CreateCh(k, v, create_end, real_create_end)
    end
    --创建一个系统角色来提供播放系统技能
    if ch_Info[3] then
        local sys_vo = DataHelper.TableCopy(ch_Info[3])
        sys_vo.grid_idx = 0
        self:CreateCh(0, sys_vo, nil)
    end
    -- Test
    local list = self.data:GetSQList()
    --[[ local temp
    for i = 1, 10 do
        temp = {}
        temp.grid_idx = 20 + i
        temp.hallow_id = i - 1--圣器id
        temp.owner_uid = i
        temp.owner_idx = temp.grid_idx < 26 and temp.grid_idx - 20 or temp.grid_idx - 15
        list[i] = temp
    end ]]
    -- local sv_list = {1,2,3,4,5,6,7,8,9,10}
    for i, v in pairs(list) do
        self:CreateSV(v.grid_idx, v)
    end


    local dragon_list = self.data:GetDragonList()
    for i, v in pairs(dragon_list) do
        self:CreateDragon(v.grid_idx, v)
    end

    for i = RABBIT_SHIELD_GRID_BEGIN_INDEX + 1, RABBIT_SHIELD_GRID_BEGIN_INDEX + RABBIT_SHILED_GRID_MAX do
        if self.data:GetRabbitShield(i) then
            self:CreateRabbitShield(i)
        end
    end
end
--圣器
function BattleStream:CreateSV(idx, info)
    --LogDG("BattleStream创建圣器")
    local sv = BattleSV.New(info)
    self.displayer.svs[idx] = sv
    sv:Load()
end

--神龙
function BattleStream:CreateDragon(idx, info)
    --LOG("创建神龙 idx == ", idx, info)
    local dgn = BattleDragon.New(info)
    self.displayer.dgns[idx] = dgn
    dgn:Load()
end

-- 偃月兔护盾 idx == 41 - 50
function BattleStream:CreateRabbitShield(idx)
    local ch = BattleRabbitShield.New({grid_idx = idx})
    self.displayer.rabbit_shields[idx] = ch
    ch:Load()
    return ch
end

function BattleStream:CreateCh(idx, info, on_complete,real_create_end)
    local ch = BattleCh.New(info)
    self.displayer.chs[idx] = ch
    ch:Load(
        function()
            if ch.vo.curhp <= 0 then
                ch:WaitAnimEnd(
                    ChAnimType.Death,
                    function()
                        ch:ClearWaitDieCheck()
                        ch.created = true
                    end
                )
                ch:PlayAnim(ChAnimType.Death)
            else
                ch.created = true
            end
            if on_complete then
                on_complete(idx)
            end
        end,real_create_end
    )
end
function BattleStream:Clear()
    BattleSkillPool.Instance:Clear()
    BattleLuaFunction.update_until_true = {}
    BattleLuaFunction.fixedupdate_until_true = {}
    if self.round_phase_care ~= nil then
        local base_data = self.data:BaseData()
        base_data:Uncare(self.round_phase_care)
        self.round_phase_care = nil
    end
    self.displayer:Clear()
    self.data:Clear()
    if self.play_queue_timer then
        TimeHelper:CancelTimer(self.play_queue_timer)
        self.play_queue_timer = nil
    end
    --清理干净
    for _, value in pairs(self.behaviour_queues) do
        for _, beh in ipairs(value) do
            if beh and beh:IsDone() == false then
                beh:ForceDone()
            end
        end
    end
	self.cur_queue = nil
    self.behaviour_queues = {}
    TeamData.Instance:PopBattleSalaryDisplay()      --每次战斗结束都清空侠义值展示数据
end

function BattleStream:OnRoundPhaseChange()
    local base_data = self.data:BaseData()
    if base_data.round_phase ~= BATTLE_ROUND_PHASE.Client then
        return
    end
    --start play cur round
    local cur_round_data = self.data:GetRoundData()

    self.behaviour_queues = {}
    BattleData.Instance:GenerateDataChInfo()
    --one queue each move data
    for i, mo_da in ipairs(cur_round_data) do
        local queue = BattleBehaviourQueue.New(i)
        BattleNodeEvent.SetBehaviourQueue(queue)
        table.insert(self.behaviour_queues, queue)
        BattleNodeEvent.detail_player = BattleDetailPlayer.New(mo_da.eve_type, mo_da.grid_idx, mo_da.data)
        BattleStream.setup_move_data_post(mo_da, true)
        BattleNodeEvent.detail_player = nil
    end
    BattleNodeEvent.SetBehaviourQueue(nil)
    self.is_playing = true
    local limit_num = 3
    limit_num = BattleHelper.LIMIT_PLAY_QUEUE[base_data.monster_group_id] or limit_num
    self:playQueueListSyncly(
        BindTool.Bind(self.OnQueuePlayComplete, self),
        limit_num
    )
end
function BattleStream:OnQueuePlayComplete()
    local base_data = self.data:BaseData()
    if BattleHelper.BATTLE_ORIGIN_JUQING(base_data.monster_group_id) and StoryData.Instance:IsPlaying() then  --临时代码，播放初始战斗剧情的时候防止战斗提前结束
        Runner.Instance:RunUntilTrue(function()
            if not StoryData.Instance:IsPlaying() then
                self.is_playing = false
                BattleCtrl.Instance:RoundPlayFinish()
                return true
            else
                return false
            end
        end)
    else
        self.is_playing = false
        BattleCtrl.Instance:RoundPlayFinish()
    end
end
local LockMgr = BaseClass()

function LockMgr:__init()
    self.list = {}
end

function LockMgr:Lock(param)
    self.list[param] = true
end

function LockMgr:Unlock(param)
    self.list[param] = false
end

function LockMgr:IsLocked(param)
    if type(param) == "table" then
        for index, _ in pairs(param) do
            if self.list[index] then
                return true
            end
        end
    else
        if self.list[param] then
            return true
        end
    end
    return false
end

-- 依次播放队列列表，当前队列播完才会播放下一个
function BattleStream:playQueueListSyncly(on_complete, limit_num)
    --LogDG("开始循环播放队列")
    local memo = {}
    for _, queue in ipairs(self.behaviour_queues) do
        local actor_list = queue:GetActorList()
        queue.depend_list = {}
        for actor, _ in pairs(actor_list) do
            if memo[actor] then
                for depend_queue, _ in pairs(memo[actor]) do
                    if queue.depend_list[depend_queue.index] == nil then
                        queue.depend_list[depend_queue.index] = true
                        for deep_index, _ in pairs(depend_queue.depend_list) do
                            queue.depend_list[deep_index] = true
                        end
                    end
                end
                memo[actor][queue] = true
            else
                memo[actor] = {[queue] = true}
            end
        end
    end

    if self.play_queue_timer then
        TimeHelper:CancelTimer(self.play_queue_timer)
        self.play_queue_timer = nil
    end
    local lock_mgr = LockMgr.New()
    local playing_indexes = {}
    local played_indexes = {}
    local end_index = table.nums(self.behaviour_queues)
    -- local all_done
    local playing_num
    local played_num
    self.run_func = function()
        if BattleHelper.BATTLE_IGNORE_SEQUENCE[BattleData.Instance:BattleMode()] and
            self.cur_queue ~= nil and self.cur_queue:IsPlaying() then
            return
        end
        local all_done = true
        for index, queue in ipairs(self.behaviour_queues) do
            if queue:IsPlayed() then
                lock_mgr:Unlock(queue.index)
                playing_indexes[index] = nil
                played_indexes[index] = true
            else
                all_done = false
                playing_num = table.nums(playing_indexes)
                if playing_num >= limit_num then
                    break
                end
                played_num = table.nums(played_indexes)
                if index == end_index and played_num < (end_index - 1) then
                    break
                end

                if not queue:IsPlaying() and not lock_mgr:IsLocked(queue.depend_list) then
                    self.cur_queue = queue
                    lock_mgr:Lock(queue.index)
                    queue:Play()
                    playing_indexes[index] = true
                    if queue:IsPlayed() then
                        self.run_func()
                    end
                    break
                end
            end
        end

        if all_done then
            if on_complete ~= nil then
                on_complete()
            end
            if self.play_queue_timer then
                TimeHelper:CancelTimer(self.play_queue_timer)
                self.play_queue_timer = nil
            end
        end
    end
    local interval_time = BattleHelper.BATTLE_IGNORE_SEQUENCE[BattleData.Instance:BattleMode()] and 0.05 or 1
    self.play_queue_timer = TimeHelper:AddRunTimer(self.run_func, interval_time, nil, true)
end
function BattleStream:Interrupt()
    if self.play_queue_timer then
        TimeHelper:CancelTimer(self.play_queue_timer)
        self.play_queue_timer = nil
    end
    if self.cur_queue then
        self.cur_queue:ForceDone()
    end
    self:Clear()
    -- BattleCtrl.CSBattleFastForwardReq()
end
function BattleStream.setup_move_data_pre(data)
    local grid_idx = data.grid_idx
    local eve_type = data.eve_type
    local eve_data = data.data
    BattleNodeEvent.SetDataBegin(eve_type, grid_idx, eve_data)
    local child_data = data.childs
    if child_data ~= nil then
        for _, v in pairs(child_data) do
            BattleStream.setup_move_data_pre(v)
        end
    end
end

function BattleStream.setup_move_data_post(data, parent, last)
    local grid_idx = data.grid_idx
    local eve_type = data.eve_type
    local eve_data = data.data
    local child_data = data.childs

    --[[ local detail_player = BattleNodeEvent.detail_player
    if not parent then
        detail_player:PushChild(eve_type, grid_idx, eve_data)
    end ]]
    BattleNodeEvent.SetDataBegin(eve_type, grid_idx, eve_data, last)
     --3
    if child_data ~= nil then
        for _, v in ipairs(child_data) do
            BattleNodeEvent.SetDataAfterBegin(v.eve_type, v.grid_idx, v.data)
         --5
        end
    end
    if parent then
        BattleNodeEvent.SetDataAfterBegin(eve_type, grid_idx, eve_data, last)
     --5
    end
    BattleNodeEvent.SetDataPerform(eve_type, grid_idx, eve_data, child_data, last)
     --2
    if child_data ~= nil then
        for i, v in ipairs(child_data) do
            BattleStream.setup_move_data_post(v, false, i == 1 and data or child_data[i - 1])
        end
    end
    BattleNodeEvent.SetDataEnd(eve_type, grid_idx, eve_data, last)
     --4

    if parent then
        BattleStream.execute_after_end(data)
    end
    --[[ if parent then
        detail_player:AllSet()
    end ]]
end

function BattleStream.execute_after_end(data)
    local grid_idx = data.grid_idx
    local eve_type = data.eve_type
    local eve_data = data.data
    local child_data = data.childs
    BattleNodeEvent.SetDataAfterEnd(eve_type, grid_idx, eve_data)
    if child_data ~= nil then
        for _, v in pairs(child_data) do
            BattleStream.execute_after_end(v)
        end
    end
end
