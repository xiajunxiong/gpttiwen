LunJianData = LunJianData or BaseClass()
LunJianData.SCHEDULE_COUNT = 19
LunJianData.Status = {
    Invalid = 0,
    Signup = 1,    --报名阶段
    Schedule = 2,   --分组阶段
    Battle = 3,     --正赛阶段  6号-24号
    Calculate = 4,  --结算阶段
    Rest = 5,       --休赛阶段
}

LunJianData.RewardState = {
    CantGet = 0,
    GetAble = 1,
    Getted = 2,
}

LunJianData.BATTLE_BEGIN_DAY = 6    --从每月几号开始正赛阶段
LunJianData.BATTLE_HOUR = 18        --每天几点开始
function LunJianData:__init()
    if LunJianData.Instance ~= nil then
        Debuger.LogError("[LunJianData] attempt to create singleton twice!")
        return
    end
    LunJianData.Instance = self

    self.view_data = SmartData.New({
        rank_seled_group = 1,
        report_seled_group = 1,
    })
    self.season_info = SmartData.New({
        signup_num = 0,
        cur_status = LunJianData.Status.Invalid, --LunJianData.Status
        next_status_ts = 0,
        index = 0,  ---赛季号
        group_num = 9,
        battle_ready_ts = 0,    --战斗准备阶段结束时间戳，与ServerTime比较算出差值
    })
    self.my_info = SmartData.New({
        has_signup = false,
        group_idx = 0,  --[1,总分组数]
        schedule_list = nil, --赛程信息
        score = 0,
        win_num = 0,
        rank = 20,
        win_box_reward_flag = 0,    --胜场宝箱领取标识
        today_battle_end = false,   --今天是否已经战斗结束
        has_fetch_signup_reward = false,--报名奖励领取标识
        daily_reward_fetch_flag = 0, --每日奖励领取标记（0：不可领取 1：可领取 2：已领取） LunJianData.RewardState
        flush_act = false
    })
    local showup_info = {
        plat_type = 0,
        uid = 0,
        level = 0,
        profession = 0,
        avatar_type = 0,
        headshot_id = 0,
        name = "",
        is_show_up = false
    }
    self.my_show_up_info = SmartData.New(showup_info)
    self.other_show_up_info = SmartData.New(showup_info)
    --key:groupId,value:{ts=上次获取数据时间，rank=SCWorldArenaGroupRankList[user_list ITEM]}
    self.rank_data = SmartData.New()
    Cfg.Register("LunJianGroupName",Cfg.RealTimeSearch(Config.worldarena_auto.team_name,"sn"))
    Cfg.Register("LunJianWinNumBox",Cfg.RealTimeSearch(Config.worldarena_auto.victory_times_reward,"sn"))

    self.groups_cache = nil --自己的分组在最前面
end

function LunJianData:Clear()
    self.season_info.cur_status = LunJianData.Status.Invalid
    self.season_info.index = 0
    self.season_info.battle_ready_ts = 0
    self.my_info.schedule_list = nil
    self.my_info.has_signup = false
    self.my_info.has_fetch_signup_reward = false
    self.my_info.today_battle_end = false
    self.groups_cache = nil
    self.rank_data:Set({})
    -- ActivityCtrl.Instance:OnActivityStatus(
    --         {act_status_info = {act_type = ActType.WorldArena,status = ActStatusType.Close,
    --             next_status_switch_time = 0}})
end

function LunJianData:__delete()
    LunJianData.Instance = nil
end

function LunJianData:CurSeason()
    return self.season_info.index
end

function LunJianData:SeasonStatus()
    return self.season_info.cur_status
end

function LunJianData:SignupNum()
    return self.season_info.signup_num
end

function LunJianData:HasSignuped()
    return self.my_info.has_signup
end

function LunJianData:MyScore()
    return self.my_info.score
end

function LunJianData:MyRank()
    return self.my_info.rank
end

--我的胜场
function LunJianData:MyWinNum()
    return self.my_info.win_num
end

function LunJianData:MyGroupId()
    return self.my_info.group_idx
end

function LunJianData:AllGroupId()
    if self.groups_cache == nil then
        local my_group = self:MyGroupId() or 0
        self.groups_cache = {}
        if my_group > 0 then
            table.insert(self.groups_cache,my_group)    --我的分组放在第一个，其它的按ID排
        end
        for i=1,self.season_info.group_num do
            if i ~= my_group then
                table.insert(self.groups_cache,i)
            end
        end
    end
    return self.groups_cache 
end

function LunJianData:ViewData()
    return self.view_data
end

function LunJianData:SetRankSeledGroup(group_id)
    self.view_data.rank_seled_group = group_id
end

function LunJianData:RankSeledGroup()
    return self.view_data.rank_seled_group
end

function LunJianData:SetReportSeledGroup(group_id)
    self.view_data.report_seled_group = group_id
end

function LunJianData:ReportSeledGroup()
    return self.view_data.report_seled_group
end

function LunJianData:ScheduleList()
    return self.my_info.schedule_list
end

function LunJianData:HasScheduleList()
    return self.my_info.schedule_list ~= nil and #self.my_info.schedule_list ~= 0
end

function LunJianData.GroupIdToName(group_id)
    local co = Cfg.LunJianGroupName(group_id)
    if co ~= nil then
        return string.format(Language.LunJian.GroupNameFormat,co.team_name)
    else
        return string.format(Language.LunJian.GroupNameFormat,"未知[".. tostring(group_id) .."]")
    end
end

function LunJianData.OtherCfg()
    return Config.worldarena_auto.other[1]
end


function LunJianData:OnSCWorldArenaInfo(protc)
    self.season_info.signup_num = protc.already_signup_num
    local old_status = self.season_info.cur_status
    self.season_info.cur_status = protc.cur_status
    self.season_info.next_status_ts = protc.next_status_timestampe
    if self.season_info.index ~= protc.season_index then
        self.season_info.index = protc.season_index
        self.my_info.schedule_list = nil
    end
    if self.season_info.group_num ~= protc.total_group_num then
        self.groups_cache = nil
        self.season_info.group_num = protc.total_group_num
    end
    local actInfo = ActivityData.Instance:GetActivityInfo(ActType.WorldArena)
    if actInfo then
        actInfo:ActRedPoint()
    end    
    if old_status == LunJianData.Status.Signup or protc.cur_status == LunJianData.Status.Signup then
        ActivityRandData.Instance:CheckRandOpenData()
    end
    self:CheckActTips()
end

function LunJianData:NextStatusTS()
    return self.season_info.next_status_ts
end

function LunJianData:OnSCWorldArenaStartEnterSceneNotice(protc)
    self.season_info.battle_ready_ts = protc.enterscene_end_timestamp
    self.my_info.today_battle_end = protc.has_finish_this_round ~= 0    --今天是赛程是否已经打过了
    if self.my_info.today_battle_end == false and 
        self.season_info.battle_ready_ts > TimeHelper:GetServerTime() then
        local act_info = ActivityData.Instance:GetActivityInfo(ActType.WorldArena)
        PublicPopupCtrl.Instance:ActTips(act_info,self.season_info.battle_ready_ts)
        -- ActivityCtrl.Instance:OnActivityStatus(
        --     {act_status_info = {act_type = ActType.WorldArena,status = ActStatusType.Open,
        --         next_status_switch_time = self.season_info.battle_ready_ts}})
        ActFormationCtrl.Instance:SendGetFormationInfo(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_WORLD_ARENA)        
    else
        -- ActivityCtrl.Instance:OnActivityStatus(
        --     {act_status_info = {act_type = ActType.WorldArena,status = ActStatusType.Close,
        --         next_status_switch_time = 0}})
    end
end

function LunJianData:OnSCWorldArenaUserSignupInfo(protc)
    self.my_info.has_signup = protc.has_siginup ~= 0
    local actInfo = ActivityData.Instance:GetActivityInfo(ActType.WorldArena)
    if actInfo then
        actInfo:ActRedPoint()
    end
    self:CheckActTips()
end

function LunJianData:OnSCWorldArenaUserScheduleInfo(protc)
    local sch_list = {}
    self.my_info.group_idx = protc.group_idx
    for _,v in pairs(protc.schedule_list) do
        sch_list[v.round_idx] = v
    end
    self.my_info.schedule_list = sch_list
end

function LunJianData:OnSCWorldArenaUserBaseInfo(protc)
    self.my_info.has_signup = protc.has_siginup ~= 0
    self.my_info.rank = protc.cur_rank
    if self.my_info.group_idx ~= protc.group_idx then
        self.groups_cache = nil
        self.my_info.group_idx = protc.group_idx 
    end
    self.my_info.win_num = protc.win_num
    self.my_info.score = protc.score
    self.my_info.win_box_reward_flag = protc.win_box_reward_flag
    self.my_info.schedule_list = nil
    self.my_info.has_fetch_signup_reward = protc.has_fetch_signup_reward ~= 0
    self.my_info.daily_reward_fetch_flag = protc.daily_reward_fetch_flag
    local actInfo = ActivityData.Instance:GetActivityInfo(ActType.WorldArena)
    if actInfo then
        actInfo:ActRedPoint()
    end
    self:CheckActTips()
end

function LunJianData:OnSCWorldArenaGroupRankList(protc)
    local data = {ts = TimeHelper.GetServerTime(),rank = protc.user_list,top3_info = protc.top3_info_list}
    self.rank_data[protc.group_idx] = data
    if protc.group_idx == self:MyGroupId() then -- 刷新我的排名
        for _,v in ipairs(data.rank) do
            if v.uid == RoleData.Instance:GetRoleId() then
                self.my_info.rank = v.rank
                self.my_info.rank = v.total_num <= 0 and -1 or v.rank
            end
        end
    end
end

function LunJianData:OnSCWorldArenaShowUpInfo(protc)
    -- LogError(RoleData.Instance:GetRoleId(),protc.attacker_info.uid,protc.defender_info.uid)
    if RoleData.Instance:GetRoleId() == protc.attacker_info.uid then
        self.my_show_up_info:Set(protc.attacker_info)
        self.other_show_up_info:Set(protc.defender_info)
    else
        self.my_show_up_info:Set(protc.defender_info)
        self.other_show_up_info:Set(protc.attacker_info)
    end
end

function LunJianData:MyShowupInfo()
    return self.my_show_up_info
end

function LunJianData:OtherShowupInfo()
    return self.other_show_up_info
end

--给外部的排名数据，如果距离上次获取时间超过一个阈值则返回空数据
function LunJianData:GetGroupRankList(group_idx)
    local data = self.rank_data[group_idx]
    if data == nil then
        return nil
    end
    local space = TimeHelper.GetServerTime() - data.ts
    if space >= 120 then
        return nil
    else
        return data.rank,data.top3_info
    end
end

function LunJianData:HasGroupRankList(group_idx)
    return self:GetGroupRankList(group_idx) ~= nil
end

function LunJianData:RankData()
    return self.rank_data
end

--比赛日的18:00-18:10为比赛时间
function LunJianData:IsBattleingTime()
    if self:SeasonStatus() ~= LunJianData.Status.Battle then
        return false
    end
    if self.my_info.today_battle_end == true then
        return false
    end
    return self.season_info.battle_ready_ts >= TimeHelper:GetServerTime()
    -- local hour = os.date("%H")
    -- if hour ~= 18 then
    --     return false
    -- end
    -- local minute = os.data("%M")
    -- if minute > 10 then
    --     return false
    -- end
    -- return true
end


function LunJianData:WinRewardGetState(idx)
    local getted = bit:_hasflag(self.my_info.win_box_reward_flag,idx + 1)
    if getted then
        return LunJianData.RewardState.Getted
    end
    local cfg = Cfg.LunJianWinNumBox(idx)
    if cfg == nil then
        return LunJianData.RewardState.CantGet
    end
    if self:MyWinNum() >= cfg.victory_times then
        return LunJianData.RewardState.GetAble
    else
        return LunJianData.RewardState.CantGet
    end
end

function LunJianData.WinRewardCfg()
    return Config.worldarena_auto.victory_times_reward
end

function LunJianData:SeasonInfo()
    return self.season_info
end

function LunJianData:MyInfo()
    return self.my_info
end

function LunJianData:HasReportData()
    return self:CurSeason() > 0 and
        self:SeasonStatus() == LunJianData.Status.Rest and 
        self.season_info.group_num > 0 
end

function LunJianData:HasFetchSignupReward()
    -- return true
    return self.my_info.has_fetch_signup_reward
end

function LunJianData.SignupRewardCfg()
    return LunJianData.OtherCfg().report_reward
end

function LunJianData:RemindFunc()
    local status = self:SeasonStatus()
    if status ~= LunJianData.Status.Battle and status ~= LunJianData.Status.Calculate then
        if status == LunJianData.Status.Signup and self:HasSignuped() == false then --可报名的话显示红点
            return 1
        end
        return 0
    end
    if self:DailyRewardFetchFlag() == LunJianData.RewardState.GetAble then
        return 1
    end
    local win_reward_data = LunJianData.WinRewardCfg()
    for i=1,#win_reward_data do
        local cfg = Cfg.LunJianWinNumBox(i)
        if self:MyWinNum() < cfg.victory_times then
            return 0
        else
            local getted = bit:_hasflag(self.my_info.win_box_reward_flag,i + 1)
            if getted == false then
                return 1
            end
        end
    end
    return 0
end

function LunJianData:WinRewardGettedCount()
    local win_reward_data = LunJianData.WinRewardCfg()
    local re = 0
    for i=1,#win_reward_data do
        local cfg = Cfg.LunJianWinNumBox(i)
        if self:MyWinNum() < cfg.victory_times then
            return re
        end
        if bit:_hasflag(self.my_info.win_box_reward_flag,i + 1) then
            re = re + 1
        end
    end
    return re
end

function LunJianData:DailyRewardFetchFlag()
    return self.my_info.daily_reward_fetch_flag
end

--海报提醒开启条件
function LunJianData:GetPosterOpen()
    if not FunOpen.Instance:IsFunOpen(Mod.LunJian.Main) then
        return false
    end
    local Status = LunJianData.Status
    local cur_status = self:SeasonStatus()
    if cur_status == Status.Signup or cur_status == Status.Schedule or cur_status == Status.Battle then
        if not LunJianData.Instance:IsBattleingTime() then
            if not (cur_status ~= Status.Signup and self:HasSignuped() == false) then
                local now_pref = os.date("%Y_%m_%d")
                if PrefsString(PrefKeys.LunJianHinted()) ~= now_pref then
                    PrefsString(PrefKeys.LunJianHinted(),now_pref)
                    return true
                end
            end
        end
    end
    return false
end

--左上角通知下面的倒计时显示
function LunJianData:CheckActTips()
    local cur_status = self:SeasonStatus()
    if cur_status == LunJianData.Status.Signup and self:HasSignuped() == false then --可报名的话显示红点
        local act_info = ActivityData.Instance:GetActivityInfo(ActType.WorldArena)
        PublicPopupCtrl.Instance:ActTips(act_info, self:NextStatusTS())
    else
        ActivityRemindData.Instance:DeleteRemindItem(ActType.WorldArena)
        self.my_info.flush_act = not self.my_info.flush_act
    end
end