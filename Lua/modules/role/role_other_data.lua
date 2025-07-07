--其他的数据管理 
function RoleData:InitOtherData()
    self.promote_red_piont = SmartData.New({val = false})
    self.profession_view_data = SmartData.New({select_profession = 0})
    self.change_prof_times = SmartData.New({num = 0})
    self.role_total_score = SmartData.New({flush_score = false})
    self.old_total_score = 0

    self.world_level_updata = SmartData.New({})
    self.world_level_data = {
        open_day=0,
        today_zero_timestamp=0
    }

    self.break_info = SmartData.New({
        seq = 1,
        now_exp = 0,
        breathing_count = 5,
        is_max_level = false,
        is_max_exp = false,
        state = -1,
        use_id = 1,
        use_flag = false,
        cumulation_exp = 0,
        cumulation_time = 0,
        hour = 0,
        min = 0,
        cumulation_coin = 0,
        cumulation_item = {},
    })
    self.break_rate_by_hour = 3600 / Config.break_cfg_auto.other[1].player_exp_s
end

function RoleData:ClearOtherData()
    self.profession_view_data:Set({select_profession = 0})
    self.change_prof_times = SmartData.New({num = 0})
    SkillData.Instance:Clear()
    self.old_total_score = 0
    self.break_info:Set({
        seq = 1,
        now_exp = 0,
        breathing_count = 5,
        is_max_level = false,
        is_max_exp = false,
        state = -1,
        use_id = 1,
        use_flag = false,
        cumulation_exp = 0,
        cumulation_time = 0,
        hour = 0,
        min = 0,
        cumulation_coin = 0,
        cumulation_item = {},
    })
end

--进阶红点 声望 任务 评分
function RoleData:ChangePromoteRedPiont()
    self.promote_red_piont.val = not self.promote_red_piont.val
    BianQiangData.Instance:FlushListRemind(BianQiangData.ListType.ZhiYeJinJie)
end

function RoleData:ChangeSikllRedPiont()
    SkillData.Instance.change_data.change = not SkillData.Instance.change_data.change
end

function RoleData:ChangeRoleTotalScore()
    self.role_total_score.flush_score = not self.role_total_score.flush_score
end

function RoleData:GetProfessionViewData()
    return self.profession_view_data
end

function RoleData:SetProfessionExtraInfo(protocol)
    self.change_prof_times.num = protocol.change_prof_times
end

function RoleData:SetBreakInfo(protocol)
    self.break_info.seq = protocol.cur_seq
    self.break_info.now_exp = protocol.cur_break_exp
    self.break_info.cumulation_exp = protocol.cumulation_user_exp_count
    self.break_info.cumulation_time = protocol.user_cumulation_pratice_time
    self.break_info.cumulation_coin = protocol.cumulation_user_coin_count
    local now_config = Config.break_cfg_auto.break_config[self.break_info.seq] or {}
    -- 如果表配了-1相当于可以无限吐纳
    if (now_config.tuna_num or 0) == -1 then
        self.break_info.breathing_count = -1
    else
        -- 计算出剩余吐纳次数
        local have_break_count = (now_config.tuna_num or 0) - protocol.tuna_num
        have_break_count = have_break_count > 0 and have_break_count or 0
        self.break_info.breathing_count = have_break_count > 0 and have_break_count or 0
    end
    self.break_info.is_max_level = protocol.cur_seq >= Config.break_cfg_auto.other[1].break_max_num
    self.break_info.is_max_exp = protocol.cur_break_exp >= (now_config.break_exp or 0)

    self.break_info.hour = math.floor(self.break_info.cumulation_time / 3600)
	self.break_info.min = math.floor((self.break_info.cumulation_time / 60) % 60)
	self.break_info.hour = self.break_info.hour > (now_config.time_max_hour or 0) and (now_config.time_max_hour or 0) or self.break_info.hour
    self.break_info.min = self.break_info.hour == (now_config.time_max_hour or 0) and 0 or self.break_info.min

    -- 奖励找回中的境界红点
    RewardData.Instance:ActivityBackRedPoint()
end

function RoleData:SetBreakCumulationItem(protocol)
    self.break_info.cumulation_item = protocol.cumulation_item
end

function RoleData:GetBreakRateByHour()
    return self.break_rate_by_hour
end

-- 境界红点
function RoleData:BreakRemind()
    return self.break_info.hour >= Config.break_cfg_auto.seq_time[1].end_time or FightData.Instance:GetFightSoulRemind() == 1
end

-- 单纯的境界领取奖励红点
function RoleData:SingleBreakRemind()
    return self.break_info.hour >= Config.break_cfg_auto.seq_time[1].end_time and 1 or 0
end

-- 境界时间描述
function RoleData:BreakTimeDesc()
    local now_config = Config.break_cfg_auto.break_config[self.break_info.seq] or {}
    return Format(Language.Role.Break.MaxTime, self.break_info.hour, self.break_info.min, now_config.time_max_hour or 0)
end

-- 境界当前累计经验
function RoleData:GetBreakExp()
    return self.break_info.cumulation_exp--cumulation_coin--cumulation_exp
end

-- 领取境界当前累计经验
function RoleData:ReceiveBreakExp()
    RoleCtrl.Instance:SendOfflineBreak(2)
end

-- 当前境界累计道具，是否加上经验和铜币
function RoleData:GetBreakNowItem(add_exp, add_coin)
    local t = {}
    -- if add_exp and self.break_info.cumulation_exp > 0 then
    --     table.insert(t, {item_id = CommonItemId.Exp, num = self.break_info.cumulation_exp, is_bind = 0})
    -- end
    -- if add_coin and self.break_info.cumulation_coin > 0 then
    --     table.insert(t, {item_id = CommonItemId.Coin, num = self.break_info.cumulation_coin, is_bind = 0})
    -- end
    for k, v in pairs(self.break_info.cumulation_item) do
        table.insert(t, v)
    end
    return t
end

-- 获取主界面境界图标下方描述
function RoleData:GetIconTxt()
    local now_seq_cfg = Config.break_cfg_auto.break_config[self.break_info.seq] or {}
    local num =  (now_seq_cfg.player_exp_time_s or 0) * self.break_rate_by_hour--(now_seq_cfg.player_coin_time_s or 0) * self.break_rate_by_hour
    return Format(Language.Role.Break.IconStr, num >= 10000 and DataHelper.ConverMoney(num) or num)--Format(Language.Role.Break.IconCoinStr, num >= 10000 and DataHelper.ConverMoney(num) or num)
end

-- 保存需要延时播放的道具
function RoleData:SaveNeedDelayItem(item)
    if not self.need_delay_item then
        self.need_delay_item = {}
    end
    table.insert(self.need_delay_item, {item = item})
end

-- 释放需要延时播放的道具
function RoleData:ReleaseNeedDelayItem()
    if not self.need_delay_item then
        return
    end
    for k, v in pairs(self.need_delay_item) do
        PublicPopupCtrl.Instance:CenterI({item_id = v.item.item_id, num = v.item.num, color_str = v.item:ColorStr()})
        ChatCtrl.Instance:ChannelChatCustom {item_get = {item = v.item, num = v.item.num}}
    end
    self.need_delay_item = {}
end

function RoleData:GetBreakCanGetItem()
    if not self.break_can_get_config then
        self.break_can_get_config = {}
        local t = {}
        for k, v in pairs(Config.break_cfg_auto.item_group) do
            -- 这边强行写死1，策划说每个奖励组的道具必定是一样的
            if v.reward_group == 1 then
                table.insert(t, v)
            end
        end
        table.sort(t, DataSorter.KeyLowerSorter("seq"))
        for i, v in ipairs(t) do
            table.insert(self.break_can_get_config, TableCopy(v.reward))
        end
    end
    for k, v in pairs(self.break_can_get_config) do
        v.num = 0
        v.is_grey = true
    end
    return self.break_can_get_config
end

function RoleData:GetProfessionDesc(profession, change)
    if change then
        profession = RoleData.ProfIdToProfType(profession)
    end
    return Config.career_advancements_auto.job_desc[profession]
end

function RoleData:GetChangeProfData()
    local data_list = {}
    local profession = self.base_data.profession
    for i = 1, RoleProfessionType.ProfessionTypeMax - 1 do
        if profession ~= i then
            local data = {}
            data.name = Language.Common.Prof[i]
            data.profession = i
            table.insert(data_list, data)
        end
    end
    return data_list
end

-- 获取世界等级
function RoleData:GetWorldLevel()
    local open_day = self.world_level_data.open_day
    local world_level = 0
    for i,v in pairs(Config.roleexp_auto.world_level) do
        if open_day >= v.day then 
            world_level = v.world_level
        end
    end
    return world_level
end

-- 获取世界等级加成 百分比
function RoleData:GetWorldLevelAddExp()
    if FlyUpData.Instance:IsLevelLimit() then
        return 100
    end

    local role_level = self:GetRoleLevel()
    local addition = 100
    if role_level < Config.roleexp_auto.other[1].world_level_effect then
        return addition
    end
    
    -- 原服开服天数必须大于配表
    local yuanfu_day = TimeCtrl.Instance:GetCurOpenServerDay()
    if Config.roleexp_auto.other[1].world_level_open > yuanfu_day then
        return addition
    end
    local world_level = self:GetWorldLevel()
    local level_sub = world_level - role_level
    -- 世界等级经验加成受角色等级区间影响最大上限
    local max_add_limit
    for k, v in pairs(Config.roleexp_auto.world_level_divide) do
        if role_level >= v.min_level and role_level <= v.max_level then
            max_add_limit = v.level_addition_limit
            break
        end
    end
    for i, v in pairs(Config.roleexp_auto.world_level_addition) do
        if level_sub >= v.min_level and level_sub <= v.max_level then
            if max_add_limit and v.level_addition > max_add_limit then
                addition = max_add_limit
            else
                addition = v.level_addition
            end
        end
    end
    return addition
end