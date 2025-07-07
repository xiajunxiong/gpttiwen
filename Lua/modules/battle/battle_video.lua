BattleVideo = BattleVideo or BaseClass()
function BattleVideo:__init()
    if BattleVideo.Instance ~= nil then
        Debuger.LogError("[BattleVideo] attempt to create singleton twice!")
        return
    end
    BattleVideo.Instance = self
    self.ctrl = BattleCtrl.Instance
    self.data = BattleData.Instance
    self.stream = self.ctrl.stream
    self.cur_round = 1
    self.round_num = 0
    self.prepare_round = false

    self.video_cfg = Config.videotape_auto
    self.videotape_list = {}
    for index, value in ipairs(self.video_cfg.videotape_list) do
        local battle_mode = value.battle_mode
        self.videotape_list[battle_mode] = self.videotape_list[battle_mode] or {}
        table.insert(self.videotape_list[battle_mode], value)
    end
    self.battle_report_simple_info = SmartData.New()

    --新录像方案 统一用monster_group_id存
    self.video_config = TabGroup(self.video_cfg.videotape_list,[[battle_mode]],[[monster_group_id]])
end

function BattleVideo:OnUnloadGameLogic()
    self.cur_round = 1
    self.round_num = 0
    self.prepare_round = false
end
--这个不要用了
function BattleVideo:VideoCfg(mode, index)
    if self.videotape_list[mode] and self.videotape_list[mode][index] then
        return self.videotape_list[mode][index]
    end
    return nil
end

function BattleVideo:SimpleInfo()
    return self.battle_report_simple_info
end

function BattleVideo:GetSimpleInfo(group_id, first_kill)
    if self.battle_report_simple_info[group_id] then
        return self.battle_report_simple_info[group_id][first_kill or 0] or 0
    end
    return 0
end

function BattleVideo:OnBattleReportSimpleInfo(procotol)
    local monster_group_id = procotol.monster_group_id
    self.battle_report_simple_info[monster_group_id] = self.battle_report_simple_info[monster_group_id] or {}
    self.battle_report_simple_info[monster_group_id][procotol.is_server_first_kill] = procotol.report_file_id
    self:SimpleInfo():Notify()
end

function BattleVideo:OnBattleReportStopWatching(procotol)
    self:ReportQuit()--直接退出
end

function BattleVideo:OnBattleReport(procotol)
    --播放录像
    self.data:BaseData().video_playing = true
    self.ctrl:OnRecvChInfo(procotol.battle_info)
    self.data:SetSQList(procotol.hallow_list)
    self.round_list = procotol.round_list
    self.round_num = procotol.round_num
    for i = 1, self.round_num do
        local round = self.round_list[i]
        --判断有没有预备战报
        if round.has_prepare_result == 1 then
            self.data:SetRoundData(round.prepare_result.round, round.prepare_result.root)
            self.prepare_round = true
        end
        if self.prepare_round == false and round.has_formal_result == 1 then
            self.data:SetRoundData(round.formal_result.round, round.formal_result.root)
        end
    end
    --判断场景
    self.handle_runer =
        Runner.Instance:RunUntilTrue(
        function()
            if SceneCtrl.Instance:CurScene():GetClassType() == BattleScene and SceneCtrl.Instance:CurScene():ReadyComplete()then
                self.handle_delay1 = TimeHelper:AddDelayTimer(BindTool.Bind(self.PlayRound, self), 1)
                -- self:PlayRound()
                return true
            end
            return false
        end
    )
end

function BattleVideo:PlayRound()
    self.data:BaseData().cur_round = self.cur_round
    local cur_round_data = self.data:GetRoundData()
    if next(cur_round_data) == nil then
        LogError("录像播放结束")
        self:ReportQuit()
        return
    end
    self.stream.behaviour_queues = {}
    BattleData.Instance:GenerateDataChInfo()
    --one queue each move data
    for i, mo_da in ipairs(cur_round_data) do
        local queue = BattleBehaviourQueue.New(i)
        BattleNodeEvent.SetBehaviourQueue(queue)
        table.insert(self.stream.behaviour_queues, queue)
        BattleNodeEvent.detail_player = BattleDetailPlayer.New(mo_da.eve_type, mo_da.grid_idx, mo_da.data)
        BattleStream.setup_move_data_post(mo_da, true)
        BattleNodeEvent.detail_player = nil
    end
    BattleNodeEvent.SetBehaviourQueue(nil)
    local limit_num = 3
    self.stream:playQueueListSyncly(
        function()
            --判断如果是预阶段就从1开始
            self.handle_delay2 =
                TimeHelper:AddDelayTimer(
                function()
                    if self.prepare_round then
                        self.prepare_round = false
                        self.cur_round = 1
                        for i = 1, self.round_num do
                            local round = self.round_list[i]
                            if round.has_formal_result == 1 then
                                self.data:SetRoundData(round.formal_result.round, round.formal_result.root)
                            end
                        end
                        self:PlayRound()
                        return
                    end
                    self.cur_round = self.cur_round + 1
                    self:PlayRound()
                end,
                1
            )
        end,
        limit_num
    )
end

function BattleVideo:ReportQuit()
    Runner.Instance:RemoveRunUntilTrue(self.handle_runer)
    TimeHelper:CancelTimer(self.handle_delay1)
    TimeHelper:CancelTimer(self.handle_delay2)
    self.data:BaseData().server_fighting = false
    self.data:BaseData().video_playing = true
    self.cur_round = 1
    Time.timeScale = TimeHelper:TimeScale()
    if BattleData.Instance:InEmbattleWatching() then
        SceneCtrl.Instance:GotoScene(BattleScene)
    else
        SceneCtrl.Instance:GotoScene(GameScene)
    end
    self.stream:Clear()
    local cache = ViewMgr:ViewCache()
    if cache then
        ViewMgr:OpenView(cache)
    end
end

--当前录像配置
function BattleVideo:PlayVideoCfg(cfg)
    if cfg then
        self.play_video_cfg = cfg
    else
        return self.play_video_cfg
    end
end

--获取当前录像配置
function BattleVideo:GetVideoConfig(battle_mode,monster_group_id)
    if self.video_config[battle_mode]and self.video_config[battle_mode][monster_group_id]then
        return self.video_config[battle_mode][monster_group_id]
    end
end