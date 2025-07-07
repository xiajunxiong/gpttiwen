--------------------YunZeData--------------------
YunZeData = YunZeData or BaseClass()
function YunZeData:__init()
    if YunZeData.Instance ~= nil then
		Debuger.LogError("[YunZeData] attempt to create singleton twice!")
		return
    end
    YunZeData.Instance = self
    self.cfg = Config.yunzemengce_auto
    self.plan_cfg = {}
    -- 云泽画境
    self.say_territory_data = SmartData.New({times = 0,is_remind = true})
    -- 超值纷享
    self.super_value_enjoy_data = SmartData.New({task_fetch_flag = {}, is_buy = false, is_receive_ticket = false, task_param = {}, finish_num = 0})
    -- 分享同花礼
    self.same_flower_data = SmartData.New({result = -1, info = {}, remind = false})
    -- 云泽好礼
    self.nice_gift_data = SmartData.New({info = {buy_count = 0, prop_free_num = 0}})    -- 已购买数量，银锤子数量
    -- 精灵大抓捕
    self.elves_catch_data = SmartData.New({is_catch_sp = false, catch_num = {}})
    -- 云泽祈愿
	self.yz_wish_data = SmartData.New({next_refresh = 0, is_can_lottery = 0, is_lock_Reward = {}})

    -- 红点刷新
    self.remind_data = SmartData.New({})
end

-- 红点通知
function YunZeData:RemindNotify()
    self.remind_data:Set({})
end

-- 云泽梦册活动是否开启
function YunZeData:IsOpen()
    if not ActivityRandData.Instance:GetBtnListIsOpen(Mod.YunZe.Main) then
        return false
    end
    for k,v in pairs(self.cfg.rand_pool) do
		if ActivityData.IsOpen(v.act_type) then
			return true
		end
	end
	if ViewMgr:IsOpen(YunZeView) then
		ViewMgr:CloseView(YunZeView)
	end
	return false
end

function YunZeData:GetActListData(plan_type)
    if TableIsEmpty(self.plan_cfg) then
        self.plan_cfg = DataHelper.ConvertGroup(self.cfg.rand_pool, "plan_type")
    end
    return self.plan_cfg[plan_type] or {}
end

-- 返回当前活动，以及下期活动 (后者可为空)
function YunZeData:GetPlanActInfo(plan_type)
    local act_list = self:GetActListData(plan_type)
    local open_act = {}
    local not_open_act = {}
    for i,v in ipairs(act_list) do
        if ActivityData.IsOpen(v.act_type) then
            table.insert(open_act, v)
        elseif not TableIsEmpty(open_act) then
            table.insert(not_open_act, v)
        end
    end
    return open_act, not_open_act
end

-- 获取活动时间描述
function YunZeData:GetTimeDesc(act_type)
    local open_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(act_type)
    end_time = end_time - 1
    local begin_month,end_month = os.date("%m",open_time),os.date("%m",end_time)
    local open_day,end_day = os.date("%d",open_time),os.date("%d",end_time)
    return Format(Language.YunZe.TimeDesc,begin_month,open_day,end_month,end_day)
end

-- 获取活动预览奖励
function YunZeData:GetRewardPreview(act_type)
    local reward_cfg = self.cfg.rand_reward_item
    local list = {}
    for i,v in ipairs(reward_cfg) do
        if v.act_type == act_type then
            table.insert(list, v)
        end
    end
    if TableIsEmpty(list) then
        return {}
    end
    local reward = ServerActivityData.Instance:GetActOpenDayCfg(act_type,list)
    if TableIsEmpty(reward) then
        return {}
    else
        return reward[1].reward_item
    end
end

-- boos活动
function YunZeData:GetBoosActCfg()
    local cfg_list = self:GetActListData(6)
    return cfg_list[1] or {}
end

-- 云泽画境
function YunZeData:GetSayTerritoryData()
	return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_YUN_ZE_TERRITORY, Config.yunzehuajing_auto.reward, "section_day")
end
function YunZeData:GetSayTerritoryPicture()
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_YUN_ZE_TERRITORY)
	return self:GetSayTerritoryData()[open_day].picture
end

function YunZeData:SetSayTerritoryInfo(protocol)
	self.say_territory_data.explore_list = protocol.explore_list
	self.say_territory_data.times = protocol.times
end
function YunZeData:GetSayTerritoryRemind()
    if Config.yunzehuajing_auto.basics[1].frequency - self.say_territory_data.times > 0 and self.say_territory_data.is_remind then
        return 1
    end
    return 0
end

-- 超值纷享
function YunZeData:SetSuperValueEnjoyInfo(protocol)
	self.super_value_enjoy_data.task_fetch_flag = protocol.task_fetch_flag
	self.super_value_enjoy_data.is_buy = protocol.buy_flag[1] == 1
	self.super_value_enjoy_data.is_receive_ticket = protocol.buy_flag[2] == 1
    self.super_value_enjoy_data.task_param = protocol.task_param
    -- 计算今日已完成任务数量
    self.super_value_enjoy_data.finish_num = 0
    for k, v in pairs(self:GetSuperValueEnjoyTaskConfig()) do
        if self:GetSuperValueEnjoyTaskParam(v.type) >= v.parameter then
            self.super_value_enjoy_data.finish_num = self.super_value_enjoy_data.finish_num + 1
        end
    end
end

-- 是否已领取
function YunZeData:GetSuperValueEnjoyTaskFlag(seq)
    return self.super_value_enjoy_data.task_fetch_flag[seq + 1] or 0
end

-- 任务进度
function YunZeData:GetSuperValueEnjoyTaskParam(m_type)
    return self.super_value_enjoy_data.task_param[m_type + 1] or 0
end

function YunZeData:GetSuperValueEnjoyRemind()
    if self.super_value_enjoy_data.is_buy then
        if not self.super_value_enjoy_data.is_receive_ticket then
            return 1
        else
            for k, v in pairs(self:GetSuperValueEnjoyRewardConfig()) do
                local is_received = self:GetSuperValueEnjoyTaskFlag(v.seq) == 1
                local can_receive = (not is_received) and self.super_value_enjoy_data.finish_num >= v.task
                if can_receive then
                    return 1
                end
            end
            return 0
        end
    end
    return 0
end

-- 获取超值纷享最大开几天
function YunZeData:GetSuperValueEnjoyMaxDay()
    local day = 0
    for k, v in pairs(Config.chaozhifenxiang_auto.task) do
        if day < v.section then
            day = v.section
        end
    end
    return day
end

-- 获取第一个没有完成的任务
function YunZeData:GetSuperValueEnjoyNotFinishTask()
    for i, v in ipairs(self:GetSuperValueEnjoyTaskConfig()) do
        if self:GetSuperValueEnjoyTaskParam(v.type) < v.parameter then
            return v
        end
    end
end

function YunZeData:GetSuperValueEnjoyRewardConfig()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SUPER_VALUE_ENJOY, Config.chaozhifenxiang_auto.reward, "type")
end

function YunZeData:GetSuperValueEnjoyTaskConfig()
    local day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_SUPER_VALUE_ENJOY)
    local t = {}
    for k, v in pairs(Config.chaozhifenxiang_auto.task) do
        if v.section == day then
            t[v.seq1 + 1] = v
        end
    end
    return t
end


-- 分享同花礼
function YunZeData:GetSameFlowerRewardCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SAME_FLOWER,Config.fenxiangtonghuali_auto.reward, "type")
    return cfg
end

function YunZeData:GetSameFlowerRewardList(reward_type)
    local cfg = self:GetSameFlowerRewardCfg()
    if not cfg then
        return {}
    end
    local list = {}
    for i,v in pairs(cfg) do
        if reward_type == v.item_pool then
            table.insert(list, v)
        end
    end
    return list
end

function YunZeData:GetSameFlowerRewardLock(seq)
    if not self.same_flower_data.info.lock_rewards then
        return false
    end
    return self.same_flower_data.info.lock_rewards[seq] == 1
end

function YunZeData:GetSameFlowerResult()
    return self.same_flower_data.result or -1
end

function YunZeData:SetSameFlowerResult(result)
    self.same_flower_data.result = result
end

-- 分享同花礼红点
function YunZeData:GetSameFlowerRemind()
    local act_type = ACTIVITY_TYPE.RAND_SAME_FLOWER
    if not ActivityData.IsOpen(act_type) then
		return 0
    end

    local key = PrefKeys.ActivityIsRemind(RoleData.Instance:GetRoleId(), act_type)
    local save_value = UnityPlayerPrefs.GetString(key)
    local t0 = TimeManager.FormatUnixTime2Date(TimeCtrl.Instance:GetServerTime())
    local value = Format(Language.SameFlower.RemindValue, ActivityData.Instance:GetActivityStatusInfo(act_type).param_1, t0.year, t0.month, t0.day)
    if save_value ~= value then
        return 1
    else
        return 0
    end
end

-- 云泽二三是红点
function YunZeData:GetErSanSiRemind()
    local act_type = ACTIVITY_TYPE.RAND_YUN_ZE_ER_SAN_SHI
    if not ActivityData.IsOpen(act_type) then
		return 0
    end
    if not YunZeErSanShiData.Instance.login_flag or YunZeErSanShiData.Instance.info_sm.info.is_turntable_reward == 1 then
        return 1
    end
    return 0
end

-- 魔族在哪里红点
function YunZeData:GetMoZuRemind()
    local act_type = ACTIVITY_TYPE.RAND_MOZUZAINALI
    if not ActivityData.IsOpen(act_type) then
		return 0
    end
    if not MoZuZaiNaLiData.Instance.login_flag and MoZuZaiNaLiData.Instance.info_sm.day_times < MoZuZaiNaLiData.Instance:BasicsFrequency() then
        return 1
    end
    return 0
end

-- 精灵大抓捕
function YunZeData:SetElvesCatchInfo(protocol)
	self.elves_catch_data.is_catch_sp = protocol.is_catch_sp
	self.elves_catch_data.catch_num = protocol.catch_num
end

function YunZeData:GetElvesCatchRewardConfig()
	return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ELVES_CATCH, Config.jinglingdazhuabu_auto.reward, "reward_group")
end

function YunZeData:GetElvesSpId()
    -- 这里第一次计算一遍特殊精灵的id
    if not self.elves_catch_sp_elves_id then
        for k, v in pairs(Config.jinglingdazhuabu_auto.basics) do
            if v.reward_group_added > 0 then
                self.elves_catch_sp_elves_id = v.monster
                break
            end
        end
    end
    return self.elves_catch_sp_elves_id
end

function YunZeData:GetElvesCatchGetData()
	local t = {}
	local reward_config = self:GetElvesCatchRewardConfig()
	local get_reward = function(i, v, is_sp)
		local elves_config = Config.jinglingdazhuabu_auto.basics[i]
		if not TableIsEmpty(elves_config) and v > 0 then
			local t1 = {}
			t1.reward_id = elves_config.reward_group
			t1.contour = elves_config.contour
			t1.num = v
			t1.is_sp = is_sp
			t1.reward = is_sp and reward_config[elves_config.reward_group_added].reward_item or reward_config[elves_config.reward_group].reward_item
			t1.is_show_line = true
			return t1
		end
	end
	for i, v in ipairs(self.elves_catch_data.catch_num) do
		local t2 = get_reward(i, v, false)
		if not TableIsEmpty(t2) then
			local has = false
			for k, v in pairs(t) do
				if v.reward_id == t2.reward_id then
					v.num = v.num + t2.num
					has = true
					break
				end
			end
			if not has then
				table.insert(t, t2)
			end
		end
	end
	if self.elves_catch_data.catch_num[self:GetElvesSpId()] > 0 then
		local t2 = get_reward(self:GetElvesSpId(), self.elves_catch_data.catch_num[self:GetElvesSpId()], true)
		if not TableIsEmpty(t2) then
			table.insert(t, t2)
		end
	end
	if not TableIsEmpty(t) then
		t[1].is_show_line = false
    end
	return t
end

function YunZeData:GetElvesCatchRemind()
    local has_item = Item.GetNum(Config.jinglingdazhuabu_auto.other[1].cost_item)
    return has_item >= Config.jinglingdazhuabu_auto.other[1].quantity_item and 1 or 0
end

-- 云泽祈愿
function YunZeData:SetYZWishInfo(protocol)
	self.yz_wish_data.next_refresh = protocol.next_refresh
	self.yz_wish_data.is_can_lottery = protocol.is_can_lottery
	self.yz_wish_data.is_lock_Reward = protocol.is_lock_Reward
end

function YunZeData:GetYZWishRewardConfig()
	return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_YZ_WISH, Config.yunzeqiyuan_auto.reward, "seq")
end

function YunZeData:GetYZWishRemind()
    return self.yz_wish_data.is_can_lottery
end

function YunZeData:GetYZWishLock(seq)
    return self.yz_wish_data.is_lock_Reward[seq] or 0
end

function YunZeData:IsYZWishCanLottery()
    if self:GetYZWishRemind() == 0 then
        return false
    end
    return self:GetYZWishLockedNum() == Config.yunzeqiyuan_auto.other[1].reward_choice
end

function YunZeData:GetYZWishLockedNum()
    local num = 0
    for i = 0, 4 do
        if self:GetYZWishLock(i) > 0 then
            num = num + 1
        end
    end
    return num
end

function YunZeData:YZWishIsCanLocked()
    return self:GetYZWishLockedNum() < Config.yunzeqiyuan_auto.other[1].reward_choice
end


-- ================= 云泽好礼
function YunZeData:NiceGiftCfg()
    return Config.yunzehaoli_auto
end

function YunZeData:GetNiceRewardList()
    local cfg = self:NiceGiftCfg()
    local list_data = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_YZ_NICE_GIFT,cfg.reward,"seq")
    return list_data or {}
end
-- 云泽好礼红点
function YunZeData:GetNiceGiftRemind()
    if self.nice_gift_data.info.prop_free_num > 0 then
        return 1
    end
    local item_id = self:GetNiceGiftGoldItemId()
    local num = Item.GetNum(item_id)
    if num > 0 then
        return 1
    end
    return 0
end

function YunZeData:GetNiceGiftGoldItemId()
    return self:NiceGiftCfg().other[1].prop_added
end