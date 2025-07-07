-- FILE_REQ("modules/achieve/achieve_data")
FILE_REQ("modules/achieve/achieve_new_data")

VIEW_REQ("modules/achieve/achieve_popup_view")
VIEW_REQ("modules/achieve/achieve_new_view")
VIEW_REQ("modules/achieve/achieve_new_detail_popul")
VIEW_REQ("modules/achieve/achieve_new_cenghao")
VIEW_REQ("modules/achieve/achieve_new_road")
VIEW_REQ("modules/achieve/achieve_course")

AchieveCtrl = AchieveCtrl or BaseClass(BaseCtrl)

AchieveCtrl.OP_TYPE = {
    TASK_REWARD = 0,    -- 领取任务奖励 p1:seq
    BATTLE_BOOS = 1,    -- 打守星boos
    BOSS_REWARD = 2,    -- 领取boos奖励
    COURSE_REWARD = 3,  -- 领取历程等级奖励 p1:level
    SERVER_COURSE_INFO = 4, -- 请求历程全服完成情况
}

AchieveCtrl.COURSE_NOTICE_TYPE = {
    FINISH_FLAG = 0,    -- 通知历程达成标记， p1:seq, p2 - 0完成  1未完成
    LEVEL_REWARD = 1,   -- 通知等级奖励领取标记， p1:level, p2 - 0未领取  1已领取
}

function AchieveCtrl:__init()
    if AchieveCtrl.Instance ~= nil then
        Debuger.LogError("[AchieveCtrl] attempt to create singleton twice!")
        return
    end
    AchieveCtrl.Instance = self
    -- self.achieve_data_instance = AchieveData.New()
    self.new_data = AchieveNewData.New()
    self.achieve_popup_data = {}
    self.popup_list = {}
    self.popup_seq = -1
    self.popup_list_count = 0
    self.open_count = 0
    self.lock_list = {}
    self.is_delay_course_remind = false      -- 是否延迟成就历程提醒
    --注册网络事件
    self:RegisterProtocol(SCCOGAllInformations, "RecvAchievementAllInfo")

    -- self:RegisterProtocol(SCAchievementFetchInfo,"RecvAchievementFetchInfo")
    self:RegisterProtocol(SCCOGSingleTaskProgs, "RecvSingleTaskProgs")
    self:RegisterProtocol(SCCOGRewardFetchFlag, "RecvRewardFetchFlag")
    self:RegisterProtocol(SCCOGStarAwardStatus, "RecvStarAwardStatus")

    -- self:RegisterProtocol(SCSendAchievementHonorRollInfo,"RecvAchievementHonorRollInfo")
    -- self:RegisterProtocol(CSGetAchievementHonorRollInfo)
    self:RegisterProtocol(SCCOGSingleSeqNotify, "RecvSCCOGSingleSeqNotify")

    -- 历程信息协议
    self:RegisterProtocol(SCAchieveCourseAllInfo,"OnSCAchieveCourseAllInfo")
    self:RegisterProtocol(SCAchieveCourseInfo,"OnSCAchieveCourseInfo")
    self:RegisterProtocol(SCAchieveCourseNotice,"OnSCAchieveCourseNotice")
    self:RegisterProtocol(SCAchieveCourseServerAllInfo,"OnSCAchieveCourseServerAllInfo")
    self:RegisterProtocol(SCAchieveCourseServerInfo,"OnSCAchieveCourseServerInfo")

    self:RegisterProtocol(CSOGOperatorRequest)

    FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_ACHIEVE, function(is_win)
        if is_win == 1 then ViewMgr:OpenView(AchieveNewView,{is_win = true})end
    end)
end

function AchieveCtrl:OnInit()
    RemindCtrl.Instance:Remind():Register(Mod.Achieve.Target, self.new_data.base, BindTool.Bind(AchieveNewData.CacuRemindCount, self.new_data, nil, true))
    RemindCtrl.Instance:Remind():Register(Mod.Achieve.Target, self.new_data.base, BindTool.Bind(AchieveNewData.CacuRemindCount, self.new_data, nil, nil, true))
    RemindCtrl.Instance:Remind():Register(
        Mod.Achieve.Course,
        self.new_data.course_base_data,
        BindTool.Bind(self.new_data.CourseRemind, self.new_data)
    )
end
function AchieveCtrl:__delete()
    --注销网络事件
    self:UnRegisterProtocol(SCCOGStarAwardStatus)
    self:UnRegisterProtocol(SCCOGSingleTaskProgs)
    self:UnRegisterProtocol(SCCOGRewardFetchFlag)
    self:UnRegisterProtocol(SCAchievementAllInfo)
    self:UnRegisterProtocol(SCAchievementFetchInfo)
    -- self:UnRegisterProtocol(SCAchievementProgressInfo)
    -- self:UnRegisterProtocol(SCSendAchievementHonorRollInfo)
    if AchieveCtrl.Instance == self then
        AchieveCtrl.Instance = nil
    end
end

function AchieveCtrl:OnUnloadGameLogic()
    --RemindCtrl.Instance:Remind():UnRegister(Mod.Achieve.Main)
    self.new_data:Clear()
end

-- function AchieveCtrl:PopupView()
--     AchieveCtrl.Instance.popup_list_count = #AchieveCtrl.Instance.popup_list
--     AchieveCtrl.Instance.open_count = 0
--     -- Debuger.LogError("init total_count:"..AchieveCtrl.Instance.popup_list_count)
--     -- Debuger.LogError("init open_count:"..AchieveCtrl.Instance.open_count)
--     local index = 1
--     while (AchieveCtrl.Instance.open_count < AchieveCtrl.Instance.popup_list_count) do
--         ViewMgr:OpenView(AchievementPopupView)
--         coroutine.wait(1)
--         -- Debuger.LogError("open_count:"..AchieveCtrl.Instance.open_count)
--         -- Debuger.LogError("total_count...."..AchieveCtrl.Instance.popup_list_count)
--     end
--     coroutine.stop()
-- end

--读取成就所有信息
function AchieveCtrl:RecvAchievementAllInfo(protocol)
    -- LogError("RecvAchievementAllInfo",protocol)
    self.new_data:SetAllInformations(protocol)
end

function AchieveCtrl:RecvSingleTaskProgs(protocol)
    -- LogError("RecvSingleTaskProgs",protocol)
    local prog_data = self.new_data:GetTaskProgData(protocol.seq)
    if prog_data ~= nil then
        prog_data.prog = protocol.prog_num
        self.new_data.base.flush_logic = not self.new_data.base.flush_logic
    end
end

function AchieveCtrl:RecvRewardFetchFlag(protocol)
    -- LogError("RecvRewardFetchFlag",protocol)
    local prog_data = self.new_data:GetTaskProgData(protocol.seq)
    if prog_data ~= nil then
        prog_data.award_flag = protocol.flag
        self.new_data:MarkChanllageBtnFlush()
        --更新boss可挑战状态
        -- local old_st = self.new_data:GetBossState()
        -- if old_st <= 0 then
        --     self.new_data:SetBossState(self.new_data:CheckIsCanChallenge() and 0 or -1)
        -- end
        self.new_data.base.flush_logic = not self.new_data.base.flush_logic
    end
end

function AchieveCtrl:RecvStarAwardStatus(protocol)
    -- LogError("RecvStarAwardStatus",protocol)
    self.new_data.base.reward_stage = protocol.cur_reward_prog
    self.new_data.base.reward_star = protocol.cur_reward_star
    self.new_data.base.flush_logic = not self.new_data.base.flush_logic
end

function AchieveCtrl:RecvSCCOGSingleSeqNotify(protocol)
    -- Debuger.LogError("recv achieve...")
    -- LogError("RecvSCCOGSingleSeqNotify",protocol)
    -- self.new_data:SetAchieveFlagBySeq(protocol.achievement_seq)
    -- self.achieve_data_instance:SetFetchBySeq(protocol)

    -- if protocol.achievement_seq then

    local seq = protocol.notify_seq
    local ach_data = AchieveNewData.Instance
    local cfg = ach_data:GetAchieveCfg(ach_data:GetCurStage(), ach_data:GetCurStar(), seq)

    if cfg.cond_type == 4 then
        BattleCtrl.Instance:DelayListen(
            BATTLE_DELAY_REASON.SEAL_PET,
            function()
                table.insert(self.popup_list, seq)
                if ViewMgr:IsClosed(AchievementPopupView) then
                    ViewMgr:OpenView(AchievementPopupView)
                end
            end
        )
    else
        table.insert(self.popup_list, seq)
        if ViewMgr:IsClosed(AchievementPopupView) then
            ViewMgr:OpenView(AchievementPopupView)
        end
    end

    -- self.popup_list_count = #self.popup_list
    -- Debuger.LogError("insert  count"..self.popup_list_count)
    --self.popup_seq = protocol.achievement_seq
    -- if #self.popup_list == 1 then
    --     coroutine.start(self.PopupView)
    -- end
    -- end
end

-- function AchieveCtrl:RecvAchievementHonorRollInfo(protocol)
--     self.achieve_data_instance.recv_data.recv_fb_type = protocol.fb_type
--     self.achieve_data_instance.recv_data.recv_first_data = protocol.first_data
--     self.achieve_data_instance.recv_data.recv_roll_data = protocol.roll_data
-- end

-- function AchieveCtrl:SendAchievementHonorRollInfo(fb_type)
--     local protocol = GetProtocol(CSGetAchievementHonorRollInfo)
--     protocol.fb_type = fb_type
--     SendProtocol(protocol)
-- end

--请求打升星boss
function AchieveCtrl:ReqeustBossChanllage()
    self:RequestOperator(1)
end

--请求领取晋升奖励
function AchieveCtrl:ReqeustBossAward()
    self:RequestOperator(2)
end

--请求领取成就奖励
function AchieveCtrl:ReqeustRecvAward(pindex_seq)
    -- Debuger.LogError("ReqeustRecvAward==%s",tostring(pindex))
    self:RequestOperator(0, pindex_seq)
end

function AchieveCtrl:RequestOperator(op_type, param)
    local protocol = GetProtocol(CSOGOperatorRequest)
    protocol.op_type = op_type
    if param ~= nil then
        protocol.param = param
    end
    SendProtocol(protocol)
end


--  ===================== 历程信息 ======================
-- 成长之路 历程所有信息下发
function AchieveCtrl:OnSCAchieveCourseAllInfo(protocol)
    --LogError("成长之路 历程所有信息下发 111:", protocol.info)
    local info = protocol.info
    self.new_data.course_base_data.level = info.level
    self.new_data.course_base_data.fraction = info.fraction
    self.new_data.course_base_data.finish_flag_list = info.finish_flag_list
    self.new_data.course_base_data.level_reward_flag = info.level_reward_flag
    self.new_data.course_base_data.progress_list = info.progress_list
end

-- 成长之路 历程单个信息
function AchieveCtrl:OnSCAchieveCourseInfo(protocol)
    --LogError("成长之路 历程单个信息 2222:", protocol.info)
    local info = protocol.info
    if self.new_data.course_base_data.progress_list[info.seq] then
        self.new_data.course_base_data.progress_list[info.seq] = info.progress
    end
end

-- 成长之路 历程信息通知下发
function AchieveCtrl:OnSCAchieveCourseNotice(protocol)
    --LogError("成长之路 历程信息通知下发 3333:", protocol.info)
    local info = protocol.info
    local notice_type = info.notice_type
    self.new_data.course_base_data.level = info.level
    self.new_data.course_base_data.fraction = info.fraction
    if notice_type == AchieveCtrl.COURSE_NOTICE_TYPE.FINISH_FLAG then
        local seq = info.p1
        if self.new_data.course_base_data.finish_flag_list[seq] then
            self.new_data.course_base_data.finish_flag_list[seq] = info.p2
            local cfg = self.new_data.cfg.course[seq]
            if cfg then
                local view_param_t = {name = cfg.name, num = cfg.course_reward}
                if not ViewMgr:IsOpen(AchieveCourseFinishRemind) then
                    -- 法宝天机鼎延迟出现成就完成提醒
                    if cfg.cond_type == 50 and ViewMgr:IsOpen(FabaoLottery) then
                        self.fabao_finish_remind = view_param_t
                    elseif self.is_delay_course_remind then
                        self.delay_course_remind_content = view_param_t
                    else
                        ViewMgr:OpenView(AchieveCourseFinishRemind, view_param_t)
                    end
                end
            else
                LogError("成就达成提醒异常 协议信息：", info)
            end
        end
    elseif notice_type == AchieveCtrl.COURSE_NOTICE_TYPE.LEVEL_REWARD then
        local level = info.p1
        if self.new_data.course_base_data.level_reward_flag[level] then
            self.new_data.course_base_data.level_reward_flag[level] = info.p2
            self.new_data.course_base_data:SetDirty("level_reward_flag")
        end
    end
end

-- 成长之路 历程完成信息全部下发
function AchieveCtrl:OnSCAchieveCourseServerAllInfo(protocol)
    --LogError("成长之路 历程完成信息全部下发 444:", protocol.info)
    local info = protocol.info
    self.new_data.course_base_data.server_role_count = info.server_role_count
    self.new_data.course_base_data.role_finish_count_list = info.role_finish_count_list
end

-- 成长之路 历程完成信息单个下发
function AchieveCtrl:OnSCAchieveCourseServerInfo(protocol)
    --LogError("成长之路 历程完成信息单个下发 555:", protocol.info)
    local info = protocol.info
    self.new_data.course_base_data.server_role_count = info.server_role_count
    if self.new_data.course_base_data.role_finish_count_list[info.seq] then
        self.new_data.course_base_data.role_finish_count_list[info.seq] = info.finish_count
    end
end

-- 延迟历程达成提醒
function AchieveCtrl:DeLayCourseRemind()
    self.is_delay_course_remind = true
end

function AchieveCtrl:CloseDelayCourseRemind()
    self.is_delay_course_remind = false
    if self.delay_course_remind_content then
        ViewMgr:OpenView(AchieveCourseFinishRemind, self.delay_course_remind_content)
        self.delay_course_remind_content = nil
    end
end