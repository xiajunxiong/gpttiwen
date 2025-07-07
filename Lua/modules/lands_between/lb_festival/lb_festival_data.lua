
LB_FESTIVALl_OPER_TYPE = {
    INFO_REQ = 0,                 -- 请求信息
    FETCH_LEVEL_REWARD = 1,       -- 领取等级奖励 p1:等级  p2:0为普通奖励 1为高级奖励
    BUY_EXP = 2,                  -- 购买经验 p1:购买数量
    FETCH_SURPLUS = 3,            -- 领取溢出奖励
}

LBFestivalData = LBFestivalData or BaseClass()

function LBFestivalData:__init()
    if LBFestivalData.Instance ~= nil then
        Debuger.LogError("[LBFestivalData] attempt to create singleton twice!")
        return
    end
    LBFestivalData.Instance = self

    self:InitConfig()

    self.season_pass_info = SmartData.New({wait_reward_index = 0,flush = 0})-- 5528 同步sm -- 神域宝藏信息下发
    self.fuli_info = SmartData.New({flush = 0})-- 5527 同步sm -- 福利信息下发

    self.operate_info = SmartData.New({flush = 0}) -- 
    self.operate_buy_info = SmartData.New({flush = 0}) -- 
    self.total_remind = SmartData.New({flush = 0}) -- 

    LandsBetweenData.Instance:ExtraRemindMark({
        mod_key = Mod.LBFestival.main,
        target = self.total_remind,
        func = BindTool.Bind(self.GetLBFestivalRemind, self),
    })
end

function LBFestivalData:InitConfig()
    self.treasure_config = Config.shenyukuanghuan_auto

    -- 神域狂欢·宝藏·奖励列表
    self.config_t_r = self.treasure_config.grade_reward
    self.config_t_r_surplus = Config.shenyukuanghuan_auto.surplus_reward[1]
end

function LBFestivalData:SetFuliInfo(protocol)
    self.fuli_info.fuli_flag = protocol.fuli_flag
    -- self.fuli_info.fuli_buy_num = protocol.fuli_buy_num or 0 
end

function LBFestivalData:SetFuliBuyTimeInfo(protocol)
    -- self.fuli_info.fuli_flag = protocol.fuli_flag
    self.fuli_info.fuli_buy_num = protocol.total_buy_num
end

function LBFestivalData:SetSeasonPassInfo(protocol)
    self.season_pass_info.is_open = protocol.is_open
    self.season_pass_info.is_active_senior = protocol.is_active_senior
    self.season_pass_info.level = protocol.level
    self.season_pass_info.exp = protocol.exp

    self.season_pass_info.fetch_common_flag = protocol.fetch_common_flag
    self.season_pass_info.fetch_senior_flag = protocol.fetch_senior_flag

    self.season_pass_info.daily_tasks = protocol.daily_tasks
    self.season_pass_info.daily_task_progress = protocol.daily_task_progress
    
    self.season_pass_info.season_tasks = protocol.season_tasks
    self.season_pass_info.season_task_progress = protocol.season_task_progress

    self.season_pass_info.surplus_exp = protocol.surplus_exp
    self.season_pass_info.surplus_receive_count = protocol.surplus_receive_count

    self.season_pass_info.flush = self.season_pass_info.flush + 1
    self.total_remind.flush = self.total_remind.flush + 1
end

function LBFestivalData:GetLBFestivalRemind()
    local treasure_num = self:GetTreasureRemind()
    local total_num = treasure_num
    return total_num
end