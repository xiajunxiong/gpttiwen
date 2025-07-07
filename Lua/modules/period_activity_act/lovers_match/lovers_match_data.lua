LoversMatchData = LoversMatchData or BaseClass()
function LoversMatchData:__init()
    if LoversMatchData.Instance ~= nil then
        Debuger.LogError("[LoversMatchData] attempt to create singleton twice!")
        return
    end
    LoversMatchData.Instance = self

    self.act_info = SmartData.New({day_count = 0})
    self.fb_info = SmartData.New({succ_match = 0, integral = 0, begin_game_time = 0, begin_count_time = false, end_game_time = 0, is_init = false, reward_level = 0})
    self.rank_info = SmartData.New({})
end

function LoversMatchData:__delete()
    LoversMatchData.Instance = nil
end

function LoversMatchData:ClearData()
    self.fb_info.is_init = false
end

--活动是否开启(是否当天有该活动)
function LoversMatchData:IsActOpen()
    return PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.LoversMatch)
end
--活动是否开启时间段内
function LoversMatchData:IsActOpenTime()
    return PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.LoversMatch)
end

--剩余次数
function LoversMatchData:GetRemainTimes()
    return self:OtherConfig().reward_times - self.act_info.day_count
end

function LoversMatchData:Config()
    return Config.lovers_match_auto
end
function LoversMatchData:OtherConfig()
    return self:Config().other[1]
end

function LoversMatchData:GetNpcSeq()
    return self:OtherConfig().npc_seq
end
function LoversMatchData:GetCenterPos()
    return {x = self:OtherConfig().pos_x or 20, z = self:OtherConfig().pos_z or 16}
end
function LoversMatchData:GetCenterDis()
    return self:OtherConfig().radius_pow or 25
end

-----基础信息
function LoversMatchData:SetDayTimes(protocol)
    self.act_info.day_count = protocol.day_count
    self.act_info:Notify()
end

-----副本信息
function LoversMatchData:SetFBStateInfo(protocol)
    self.fb_info.begin_game_time = protocol.begin_game_time
    self.fb_info.end_game_time = protocol.end_game_time
end

function LoversMatchData:SetFBInfo(protocol)
    if self.fb_info.is_init and self.fb_info.reward_level < protocol.reward_level then
        PublicPopupCtrl.Instance:Center(Language.LoversMatch["RewardLevel" .. protocol.reward_level])
    end
    self.fb_info.succ_match = protocol.succ_match
    self.fb_info.integral = protocol.integral
    self.fb_info.reward_level = protocol.reward_level
    -- self:CheckTask()
end
function LoversMatchData:CheckTask()
    local time_cfg, index = self:GetNextTaskCfg()
    if time_cfg ~= nil then
        TimeHelper:CancelTimer(self.task_timer)
        local spend_time = TimeHelper.GetServerTime() - self.fb_info.begin_game_time
        self.task_timer = Invoke(BindTool.Bind1(self.OnTimerOver, self), time_cfg.play_time - spend_time + 1)
    else
        TimeHelper:CancelTimer(self.task_timer)
    end
end
function LoversMatchData:OnTimerOver()
    -- PublicPopupCtrl.Instance:Center(Language.LoversMatch["RewardLevel" .. self.fb_info.reward_level])
    self.fb_info:Notify()
    self:CheckTask()
end

function LoversMatchData:GetNextTaskCfg()
    local spend_time = TimeHelper.GetServerTime() - self.fb_info.begin_game_time
    local succ_match = self.fb_info.succ_match
    local index = 0
    for i, v in ipairs(self:Config().reward_level) do
        index = index + 1
        if spend_time < v.play_time and succ_match < v.match_couple then
            return v, index
        end
    end
end

-----获取幻化形象
function LoversMatchData:GetNpcList()
    return self:Config().play_config
end

function LoversMatchData:SetRoleForms(protocol)
    self.role_form = protocol.role_form
end
function LoversMatchData:GetRoleFormResId(vo)
    if self.role_form ~= nil then
        local npc_id = self.role_form[vo.role_id]
        if npc_id and npc_id > 0 then
            return NpcHelper.NpcCo(npc_id).res_id
        end
    end
    return nil
end

-----排行榜
function LoversMatchData:SetRankInfo(protocol)
    self.rank_info.count = protocol.count
    self.rank_info.rank_list = protocol.rank_list
    self.rank_info:Notify()
end

function LoversMatchData:GetRankList()
    local show_list = {}
    local show_rank = 0
    for i, v in ipairs(self.rank_info.rank_list or {}) do
        local data = {name = v.name, score = v.jifen, pass_time = v.pass_time, uid = v.uid}
        local last_data = show_list[#show_list]
        if last_data == nil or last_data.score ~= data.score or last_data.pass_time ~= data.pass_time then
            show_rank = show_rank + 1
        end
        data.show_rank = show_rank
        table.insert(show_list, data)
    end
    return show_list
end

function LoversMatchData:GetRankReward(rank)
    for i, v in pairs(self:Config().rank_reward) do
        if v.rank_range_min <= rank and v.rank_range_max >= rank then
            local show_list = {}
            for i = 0, #v.item_list do
                table.insert(show_list, v.item_list[i])
            end
            if v.title_id ~= 0 then
                table.insert(show_list, {item_id = self:OtherConfig().title_id, num = 1, is_bind = 1})
            end
            return show_list
        end
    end
end

-----获取结果
function LoversMatchData:SetResult(protocol)
    self.result_info = {is_finish = protocol.is_finish}
    self.result_info.use_time = TimeHelper.GetServerTime() - self.fb_info.begin_game_time
    self.result_info.reward_level = self.fb_info.reward_level
    self.result_info.has_reward = self.result_info.is_finish == 1 and self:GetRemainTimes() > 0
end
function LoversMatchData:GetResult()
    return self.result_info
end

function LoversMatchData:GetReward(value)
    return self:OtherConfig()["reward_" .. value]
end

-----红点
function LoversMatchData:GetRemindNum()
    return self:GetRemainTimes() > 0 and 1 or 0
end

function LoversMatchData:CanMove()
    -- if ViewMgr:IsOpen(LoversMatchMaskView) then
    --     return false 
    -- end
    return true
end

function LoversMatchData:GetPointTitleCfg(score)
    local max_num = #self:Config().point_title
    for i = max_num, 1, -1 do
        if score >= self:Config().point_title[i].points then
            return self:Config().point_title[i]
        end
    end
end

function LoversMatchData:IsShowTips()
    return PrefsInt(PrefKeys.LoversMatchTips()) ~= 1
end

function LoversMatchData:SetNoShowTips()
    PrefsInt(PrefKeys.LoversMatchTips(), 1)
end