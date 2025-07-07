--=======================================================================================================================================
--需要热更配置重新缓存配置数据 需要放到InitExternalData （有自己定义缓存的）还需要在 external_lua_ctrl定义枚举
--=======================================================================================================================================
--建议使用 GetSectionList相关接口 获取配置（里面自带缓存） 最好能不自己定义缓存就不定义 如果表结构比较简单，那可以不需要定义缓存
--=======================================================================================================================================
--如果是需要配置表单独用哪个字段作为键名，那可以改下配置为 cs,index即可 可避免需要 External这个操作
--=======================================================================================================================================
--配置直接用就行，不需要解析，【配置上字段叫什么名字就用什么名字】，不需要自己重新全部去命名定义所有的配置字段名
--=======================================================================================================================================
--活动命名上尽量不要用拼音和拼音缩写，因为活动名字替换非常频繁
--=======================================================================================================================================

--初始化SmartData 放这里
function ServerActivityData:InitSmartData()
	self.open_server_data = SmartData.New()
	self.open_select_data = SmartData.New()
	self.limit_shop_data = SmartData.New()
	self.fashion_shop_data =  SmartData.New({buy_times = {}, now_select = 0})
	self.open_roller_data = SmartData.New{info = {}, index = 1}
	self.login_reward_data = SmartData.New()
	self.login_reward_close = SmartData.New()
	self.lusrate_nian_data = SmartData.New({today_times = 0})
	self.thriving_shop_data = SmartData.New({count = 0,exchange_count = {}})
	self.sale_shop_select = SmartData.New()
	self.sale_shop_count = SmartData.New()
	self.fortune_bag_data = SmartData.New({is_buy_flag = 0,is_fetch_reward = 0})
	self.heritage_treasure_data = SmartData.New({piece_list = {},raward_flag = 0,val = false})
	self.heritage_treasure_select = SmartData.New({data = {}})
	self.partner_exchange_data = SmartData.New()
	self.partnet_exchang_save = SmartData.New({val = false,change = false})
	self.pet_exchange_data = SmartData.New()
	self.discount_carnival_info = SmartData.New()
	self.discount_carnival_data = SmartData.New({info = nil})
	self.luck_roller_data = SmartData.New{info = {}, index = 1,val = false,type = 0}
	self.lucky_discount_data = SmartData.New({discount = 10,buy_count = {},is_try = false,today_buy_count = {}})
	self.llustrate_treasure_data = SmartData.New()
	self.element_cards_data = SmartData.New({flop_seq = 0 ,flop_count = 0})
	self.sign_in_continuity_data = SmartData.New({login_day = 0,reward_flag_t = {}})
	self.heaven_gift_data = SmartData.New({receive_count = 0,remind_check = false,is_refresh_box = 0})
	self.exclusive_gift_data = SmartData.New({info = {}, has_red_point = false, day_info = {}, lv_info = {}})
	self.wealth_cornucopia_info = SmartData.New({buy_day_times = {},buy_sum_times = {}})
	self.exp_purchase_data = SmartData.New({info = {}})
	self.gift_box_data = SmartData.New({day_buy_times = {}, buy_times = {}})
	self.chain_box_data = SmartData.New({can_buy_seq = bit:d2b(0), day_buy_times = {}, buy_times = {}})
	self.give_pet_draw_data = SmartData.New({give_mark = 1})
	self.bonus_data = SmartData.New({flag = {},need_list = {},nums_list = {},val = false})
	self.linkage_gift_data = SmartData.New({gift_seq = -1,fetch_flag = -1,can_receive_tamestamp = 0,partner_id = 21})
	self.adventure_gift_data = SmartData.New({gift_seq = 0,end_time = 0})
	self.qm_discount_data = SmartData.New({})
	self.spring_festival_store_data = SmartData.New({info = {}})
	self.cow_convert_data = SmartData.New({today_buy_times = {},score = 0,unlock_integral = 0,act_buy_times = {}})  -- 牛运兑换数据
	self.cow_origin_data = SmartData.New({map_info = {},convert_infos = {},question_id = nil})		-- 牛运当头数据
	self.little_year_gift_data = SmartData.New({thread_id = 0,has_get_reward = 0,val = true})
	self.cow_going_lock_data = SmartData.New({today_reward = 0,id = 0})
	self.give_secret_key_data = SmartData.New({info = {}})
	self.acc_recharge_data = SmartData.New({info = {is_get_reward = 1, now_recharge = 0, is_received_free = 1},val = false})
	self.deal_discount_data = SmartData.New({info = {},item_list = {}})
	self.pet_loot_data = SmartData.New({info = {}})
	self.pet_befall_data = SmartData.New({info = {},result = {}})
	self.pet_befall_remind_data = SmartData.New({info = {}})
	self.gift_rotation_data = SmartData.New({fetch_num = 0, fetch_flags = {}, now_get = {}, op_type = 0, hear_item = 0, val = false})
	self.spring_climb_data = SmartData.New({info = {}, get_item_flags = {}, remind_flag = true})
	self.secret_treasure_data = SmartData.New()
	self.labour_gift_data = SmartData.New({receive_count = 0, remind_check = false, is_refresh_box = 0})
	self.combine_gift_data = SmartData.New({ra_day_buy_times = {}, ra_buy_times = {}, phase = 1, open_day = 1, val = 0})
	self.cmd_buy_check_data = SmartData.New({ret_info = {}})

	self.heaven_sent_data = SmartData.New({reward_flag = {},val = false})
	self.heart_skill_data = SmartData.New({reward_flag = {},val = false})
	self.world_soul_data = SmartData.New({reward_flag = {},val = false})

	self.one_yuan_buy_data =  SmartData.New({has_recharge = 0, buy_times = 0})
	self.pet_befall_store_data = SmartData.New({act_buy_times = 0,day_buy_times = 0}) -- 神兽降临直购信息
	self.summer_experience_data = SmartData.New({open_day = 1, is_fetch_box = 0, first_flag = {}, first_flag_num = 0, is_fight_open = false, fight_boss_num = 1})
	self.summer_experience_two_data = SmartData.New({open_day = 1, is_fetch_box = 0, first_flag = {}, first_flag_num = 0, is_fight_open = false, fight_boss_num = 1})
	self.banquet_data = SmartData.New({base_info = {}, reward_info = {}})
	self.flower_lamp_data = SmartData.New({base_info = {}, game_info = {}, item_info = {}})		-- 多彩河灯
	self.lotus_data = SmartData.New({info = {}})
	self.lucky_sign_data = SmartData.New({sign_id = 0,is_lock = 0,reward_flag = 0})
	self.cool_card_data = SmartData.New{open_index=0}
	self.cool_breeze_data = SmartData.New()
	self.horcrux_tiancheng_info = SmartData.New({
		role = SmartData.New(),
		pool = SmartData.New(),
		draw = SmartData.New(),
		info = SmartData.New{
			model_index = 1,
		}
	})
	self.chaozhi_gift_info = SmartData.New({gift_num_list = {}})
	self.national_mibao_data = SmartData.New({is_change = true})
	self.national_precious_data = SmartData.New({})
	self.precious_cur_select_slot = SmartData.New({})
	--拍卖行
	self.paimaihang_notice_info = SmartData.New({pai_mai_start_timestamp = 0, pai_mai_end_timestamp = 0, pai_mai_last_end_timestamp = 0, shop_item_list = {}, is_flush = false})
	self.paimaihang_user_info = SmartData.New({shop_item_list = {}, is_flush = false})
	self.paimaihang_record_info = SmartData.New({record_count = 0, record_list = {}})
	self.paimaihang_other_info = SmartData.New({remind_info = {}, remind_val = 0, show_record = false})
	---回归豪礼
	self.regress_reward_data = SmartData.New({task_data = {} , select_data = {},select_val = true,task_val = true})
	self.regress_reward_red_pos = SmartData.New({num = 0})
	self.happy_linkup_data = SmartData.New({ra_frequency_num = 0,chanllenge_index = 0,challenge_is_succ = 0,challenge_end_timestamp = 0})
	self.sign_happy_data = SmartData.New({login_day = 1,gain = 0,flag = {}})
	--万灵共盈
	self.all_souls_full_info = SmartData.New({ra_buy_reward_timestamp = 0, task_list = {}, remind_val = 0})
	-- 山海问答
	self.mountain_sea_question_data = SmartData.New({is_can_answer = false, times = 0, question_id = 0, reward_id = 0})
	self.mountain_sea_question_answer = SmartData.New({is_true = false, true_id = 0})
	-- 山海风月
	self.mountain_sea_wind_moon_reward_data = SmartData.New({times = 0, info = {}})
	self.mountain_sea_wind_moon_state_data = SmartData.New({is_can_lottery = false})
	self.mountain_sea_wind_moon_lottery_data = SmartData.New({info = {}})
	-- 上海相逢
	self.mountain_sea_meet_data = SmartData.New({is_buy = 0,task_start_info = {},task_end_info = {}})
	-- 山海聚宝
	self.mountain_sea_minec_data = SmartData.New({})
	--山海初遇
	self.mountain_sea_first_meet_data = SmartData.New({task_list = {},reward_info = {}})

	self.week_gift_data = SmartData.New({info_list = {},val = false})
end

function ServerActivityData:InitExternalData()
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoBackReward,function() 
        self.regress_reward_data_cfg = nil
        self.regress_task_data_cfg = nil
        self.regress_reward_data.task_data = {}
        self:RegressTaskDataCfg()
        self:RegressRewardDataCfg()
    end)
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoCoolSummer,function()
        Cfg.UnRegister("CoolCardInfoById")
        Cfg.Register("CoolCardInfoById",Cfg.RealTimeSearch(Config.cool_summer_auto.sign_group,"sign_id"))
        Cfg.UnRegister("CoolCardRewardByType")
        Cfg.Register("CoolCardRewardByType",Cfg.RealTimeSearch(Config.cool_summer_auto.vitality_raward_group,"type"))
        Cfg.UnRegister("CoolBreezeCoByType")
        Cfg.Register("CoolBreezeCoByType",Cfg.RealTimeSearch(Config.cool_summer_auto.cool_breeze,"type"))
    end,true)
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoExclusiveGift,function()
        self.exclusive_gift_day_list = nil
    end,true)
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoExpPurchase,function()
        self.exp_configure = nil
    end,true)
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoMayDayActivity,function()
        self.combine_gift_cfg = nil
    end,true)
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoNationalDay,function()
        self["precious_reward_group"] = nil
    end,true)
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoPetLoot,function()
        self:GetPetLootConfig()
    end,true)
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoSealPromote,function()
        self.promote_config = nil
    end,true)
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoSpringFestivalStore,function()
        self.spring_festival_store_cfg = nil
    end,true)
end
--=======================================================================================================================================
--=======================================================================================================================================
--=======================================================================================================================================

--开服转盘
function ServerActivityData:SetTurnTableInfo(protocol)
	self.open_roller_data.info = protocol.open_roller_info
end

function ServerActivityData:SetTurnTableReward(protocol)
	self.open_roller_data.index = protocol.index
end

--开服商店
function ServerActivityData:SetRandDiscountStoreInfo(protocol)
	self.limit_shop_data:Set(protocol.limit_buy_count)
end

function ServerActivityData:GetLimitShopTimes(seq)
	return self.limit_shop_data[seq] or 0
end

function ServerActivityData:GetLimitShopRemindNum()
	for k,v in pairs(self:GetActivityOpenCfg("open_discount_store","seq")) do
		if v.need_gold == 0 then
			local count = v.buy_times == 0 and v.total_exchange or v.buy_times
			if count - ServerActivityData.Instance:GetLimitShopTimes(v.seq) > 0 then
				return 1
			end
		end
	end
    return 0
end

-- 时装商店
function ServerActivityData:SetFashionShopInfo(protocol)
	self.fashion_shop_data.buy_times = protocol.buy_times
end

function ServerActivityData:GetFashionShopConfig()
	local config = self:GetActivityOpenCfg("open_fashion_store", "seq")
	table.sort(config, DataHelper.WeightSort("seq", function(data)
		return data.total_exchange > self:GetFashionShopBuyTimes(data.seq) and 0 or 1000
	end))
	return config
end

function ServerActivityData:GetFashionShopBuyTimes(seq)
	return self.fashion_shop_data.buy_times[seq] or 0
end

-- 一元购
function ServerActivityData:SetOneYuanBuyInfo(protocol)
	self.one_yuan_buy_data.has_recharge = protocol.has_chong_zhi
	self.one_yuan_buy_data.buy_times = protocol.buy_times
	ActivityRandData.Instance:CheckRandOpenData()
end

function ServerActivityData:GetOneYuanBuyData()
	if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_ONE_YUAN_BUY) then
		return
	end
	local data = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ONE_YUAN_BUY, Config.buy_one_gold_auto.gift_configure, "type")
	local role_level = RoleData.Instance:GetRoleLevel()
	for i, v in ipairs(data) do
		if role_level >= v.level_down and role_level <= v.level_up then
			-- 只可能会有一个礼包
			return v
		end
	end
end

function ServerActivityData:ResetOneYuanBuyRemind()
	self.one_yuan_buy_data:Set({has_recharge = 0, buy_times = 0})
end

-- 一元购主界面入口是否存在
function ServerActivityData:GetOneYuanBuyIsOpen()
	if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_ONE_YUAN_BUY) then
		return false
	end
	if not FunOpen.Instance:IsFunOpen(Mod.RandActivity.OneYuanBuy) then
		return false
	end
	if self.one_yuan_buy_data.has_recharge > 0 and self.one_yuan_buy_data.buy_times == 0 then
		return true
	end
	return false
end

--评分奖励
function ServerActivityData:GetScoreData()
	return self:GetActivityOpenCfg("character_rating","seq")
end


-- ========================= 28天登陆奖励 =======================

--28天登录奖励
function ServerActivityData:GetLoginRewardList()
	local time_seq = self:GetLoginRewardTimeSeq()
	return self.login_reward_config[time_seq]
end

function ServerActivityData:GetLoginRewardData(day)
	local list_data = self:GetLoginRewardList()
	return list_data[day]
end

-- 获取登录奖励在哪个阶段
function ServerActivityData:GetLoginRewardTimeSeq()
	if self.login_reward_config then
		return self.login_reward_config.time_seq
	end
	local config = self:GetActivityOpenCfg("login_reward","seq")
	local server_open_time = TimeCtrl.Instance:ServerStartTS()
	-- 不知为何这个时间有可能获得空 
	-- 部分玩家为空就拿最新阶段配置，另一个是2020年版本的配置，舍弃也无大碍
	if server_open_time == nil then
		BuglyReport("开服时间获得空！！！！！")
		server_open_time = config[#config].time
	end
	server_open_time = server_open_time < config[1].time and config[1].time or server_open_time
	local time = 0
	local time_seq = 0
	self.login_reward_config = {}
	for i = 1,#config do
		if server_open_time >= config[i].time then
			if config[i].time ~= time then
				time_seq = time_seq + 1
			end
			time = config[i].time
			if self.login_reward_config[time_seq] == nil then
				self.login_reward_config[time_seq] = {}
			end
			table.insert(self.login_reward_config[time_seq],config[i])
		end
	end
	self.login_reward_config.time_seq = time_seq
	return self.login_reward_config.time_seq
end

function ServerActivityData:SetLoginRewardInfo(protocol)
	self.login_reward_config = nil
	self.login_reward_data.login_day = protocol.login_day
	local old_day
	if not TableIsEmpty(self.login_reward_data.reward_flag) then
		-- 对比旧数据
		for i,v in pairs(self.login_reward_data.reward_flag) do
			if v ~= protocol.reward_flag[i] then
				old_day = i
				break
			end
		end
	end
	self.login_reward_data.reward_flag = protocol.reward_flag
	if old_day then
		self:HandleLoginRewardItem(old_day)
		self:HandleLoginRewardReview(old_day)
	end
	self.login_reward_data:Notify()
end

function ServerActivityData:ResetLogonRewardData()
	self.login_reward_data.reward_flag = {}
end

-- 处理奖励预览
function ServerActivityData:HandleLoginRewardReview(day)
	day = day or self.login_reward_data.login_day
	local day_data = self:GetLoginRewardPreviewData(day)
	if day_data then
		self.login_preview_data = day_data
	end
end

-- 处理快捷使用
function ServerActivityData:HandleLoginRewardItem(day)
	day = day or self.login_reward_data.login_day
	local day_data = self:GetLoginRewardData(day)
	if day_data and day_data.use_pop == 1 then
		local item_id = day_data.reward_item[0].item_id
		local items = BagData.Instance:GetItemsByItemId(item_id)
		if items and items[1] and items[1]:IsCanUse() then
			KeyUseData.Instance:AddItem(items[1])
			KeyUseData.Instance:SetBtnName(Language.LoginReward.UseBtn)
			KeyUseData.Instance:SetOpenTime(60)
			ViewMgr:OpenView(KeyUseView)
		end
	end
end

-- 获取当前登录奖励预览界面的配置
function ServerActivityData:GetLoginRewardPreviewData(day)
	day = day or self.login_reward_data.login_day
	local list_data = self:GetLoginRewardList()
	for i = day + 1, #list_data do
		local v = list_data[i]
		if v.preview_pop == 1 then
			local flag = self:GetLoginRewardFlag(v.login_days)
			if flag == 0 then
				return v
			end
		end
	end
end

-- 0不可领，1可领，-1已领
function ServerActivityData:GetLoginRewardFlag(day)
	if not self.login_reward_data.reward_flag then
		return 0
	end
	local login_day = self:GetLoginRewardDays()
	if day > login_day then
		return 0
	end
	if self.login_reward_data.reward_flag[day] == 0 then
		return 1
	else
		return -1
	end
end 

function ServerActivityData:GetLoginRewardDays()
	return self.login_reward_data.login_day or 0
end 

function ServerActivityData:GetLoginRewardNum()
	if not self.login_reward_data.login_day or self.login_reward_data.login_day == 0 then
		return 0
	end
	local list = self:GetLoginRewardList()
	for i,v in pairs(list) do
		local login_days = v.login_days
		if login_days <= self.login_reward_data.login_day then
			local flag = self:GetLoginRewardFlag(login_days)
			if flag > 0 then
				return 1,login_days
			end
		end
	end
	return 0
end

function ServerActivityData:GetLoginRewardSpecialDays(cur_days)
	local list = self:GetLoginRewardList()
	for i = cur_days, #list do 
		if list[i] and list[i].is_special == 1 then 
			return list[i]
		end 
	end
	return nil
end

-- 活动是否结束
function ServerActivityData:GetLoginRewardDaysEnd()
	local list = self:GetLoginRewardList()
	if self:GetLoginRewardDays() >= #list then
		local reward_remind = self:GetLoginRewardNum()
		if reward_remind <= 0 then
			return true
		end
	end
	return false
end

function ServerActivityData:CheckTodayLoginReward()
	if self.login_reward_data.init then 
		if RewardData.Instance:IsRewardActivityBackAuto() then
			TimeHelper:AddDelayTimer(function()
				local mode = BattleData.Instance:BattleMode()
				if BattleData.Instance:IsServerFighting() and FinishData.Instance:GetCloseFunc(mode) == nil then
					FinishData.Instance:SetCloseFunc(mode,function()
						TimeHelper:AddDelayTimer(function()
							ViewMgr:OpenViewByKey(Mod.Activity.ActivityBack)
						end,2.5)
						FinishData.Instance:SetCloseFunc(mode,nil)
					end)
				else 
					TimeHelper:AddDelayTimer(function()
						ViewMgr:OpenViewByKey(Mod.Activity.ActivityBack)
					end,3.5)
				end 
			end ,1)
		end
		return 
	end
	self.login_reward_data.init = true
	if self:GetLoginRewardFlag(self.login_reward_data.login_day) > 0 then 
		TimeHelper:AddDelayTimer(function()
			local mode = BattleData.Instance:BattleMode()
		    if BattleData.Instance:IsServerFighting() and FinishData.Instance:GetCloseFunc(mode) == nil then
			    FinishData.Instance:SetCloseFunc(mode,function()
				    TimeHelper:AddDelayTimer(function()ViewMgr:OpenView(LoginRewardView)end,2.5)
				    FinishData.Instance:SetCloseFunc(mode,nil)
			    end)
		    else 
			    TimeHelper:AddDelayTimer(function()ViewMgr:OpenView(LoginRewardView)end,3.5)
		    end 
	    end ,1)
	end 
end

-- 记录活动图标挂件mono
function ServerActivityData:SetLoginRewardActIconMono(mono)
	self.login_reward_icon_mono = mono
end

function ServerActivityData:GetLoginRewardActIconMono()
	return self.login_reward_icon_mono
end

-- ==========================================================
-- ==========================================================




--特卖商店
function ServerActivityData:SetRASaleShopInfo(protocol)
	self.sale_shop_count:Set(protocol.sale_shop_count)
end

function ServerActivityData:GetSaleShopTimes(seq)
	return self.sale_shop_count[seq] or 0
end

function ServerActivityData:CheckSaleShopList()
	self.sale_shop_list = {}
	local config = Config.sale_shop_activity_auto.sale_shop
	for k,v in pairs(self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SALE_SHOP,config,"seq")) do
		if self.sale_shop_list[v.type] == nil then
			self.sale_shop_list[v.type] = {}
		end
		table.insert(self.sale_shop_list[v.type],v)
	end
end

function ServerActivityData:GetSaleShopPageData(type)
	local list = self.sale_shop_list[type] or {}
	table.sort(list,DataHelper.WeightSort("seq",function(data)
		local buy_times = data.limit_type == 1 and data.buy_times or data.today_times 
        if buy_times - self:GetSaleShopTimes(data.item_seq) <= 0 then
            return 1000
		end
	end))
	return list
end

function ServerActivityData:GetSaleShopRemindNum()
	return 0
end

--欣欣向荣
function ServerActivityData:SetRAHappyUpInfo(protocol)
	self.thriving_shop_data.count = protocol.count
	self.thriving_shop_data.exchange_count = protocol.exchange_count
end

function ServerActivityData:GetThrivingTimes(seq)
	return self.thriving_shop_data.exchange_count[seq] or 0
end

function ServerActivityData:GetThrivingShowList()
	return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_THRIVING_SHOP, Config.happy_up_activity_auto.happy_up)
end

function ServerActivityData:IsThrivingItemUnlock(data)
	if ActivityData.IsOpen(ACTIVITY_TYPE.RAND_THRIVING_SHOP) then
		local unlock_time = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_THRIVING_SHOP) + data.time * TIME_HOUR_SECOND_NUM
		return TimeHelper.GetServerTime() >= unlock_time, unlock_time
	end
	return false
end

function ServerActivityData:GetThrivingRemind()
	if not self:IsThrivingRemind() then
		return 0
	end
	for i, v in pairs(self:GetThrivingShowList()) do
		local limit_times = v.limit_type == 1 and v.buy_times or v.today_times
		local is_unlock, unlock_time = self:IsThrivingItemUnlock(v)
		if is_unlock and limit_times > (self:GetThrivingTimes(v.item_seq) or 0) and Item.GetNum(v.consume_item_id) >= v.consume_item_count then
			return 1
		end
	end
	return 0
end

--是否红点提示
function ServerActivityData:IsThrivingRemind()
	local act_status = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_THRIVING_SHOP)
	local save_val = UnityPlayerPrefs.GetInt(PrefKeys.ThrivingIsRemind())
	if act_status ~= nil then
		return act_status.param_1 ~= save_val
	end
	return act_status.param_1 ~= 1
end

function ServerActivityData:SetIsThrivingRemind(is_remind)
	if is_remind then
		local act_status = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_THRIVING_SHOP)
		UnityPlayerPrefs.SetInt(PrefKeys.ThrivingIsRemind(), act_status ~= nil and act_status.param_1 or 1)
	else
		UnityPlayerPrefs.SetInt(PrefKeys.ThrivingIsRemind(), 0)
	end
	self.thriving_shop_data:Notify()
end

--驱除妖兽
function ServerActivityData:GetNianOther()
	return Config.repel_the_monster_auto.other[1]
end

function ServerActivityData:GetNianConsume()
	return Config.repel_the_monster_auto.dispel_consume[1]
end

function ServerActivityData:SetTodayTimes(data)
	self.lusrate_nian_data.today_times = data.today_times
end

function ServerActivityData:GetRewardShow()
	local list1 = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_LUSTRATE_NIAN, Config.repel_the_monster_auto.reward_show)
	return self:GetSectionList(list1, "seq", TimeCtrl.Instance:GetActOpenServerDay(ACTIVITY_TYPE.RAND_LUSTRATE_NIAN), true)
end

function ServerActivityData:LustrateNianItemCheck()
	BehaviorData.Instance:CustomNpcTalkBtnsId(self:GetNianOther().npc_id, 
	BindTool.Bind(self.SetupBehaviourBtnForNian, self), true)
end

function ServerActivityData:SetupBehaviourBtnForNian(btn_list,obj_vo)
	if obj_vo and obj_vo.id == self:GetNianOther().npc_id 
	    and TeamData.Instance:InTeam() and TeamData.Instance:IsLeader()
        and ActStatusType.Open == ActivityData.Instance:GetActivityStatus(ACTIVITY_TYPE.RAND_LUSTRATE_NIAN) then 
	    TeamData.Instance:SetTeamItemCheckTarget(self:GetNianOther().item_id)
	    TeamCtrl.Instance:SendCheckMemberItem({item_id = self:GetNianOther().item_id,item_num = 0})
	end
end

--福袋
function ServerActivityData:SetFortuneBagInfo(protocol)
	self.fortune_bag_data.is_buy_flag = bit:d2b(protocol.is_buy_flag)
	self.fortune_bag_data.is_fetch_reward = protocol.is_fetch_reward
	self.fortune_bag_data.batch_end_time = protocol.batch_end_time
	RewardData.Instance:CheckFortuneBag()
end

function ServerActivityData:GetFortuneBagBatchTime()
	return self.fortune_bag_data.batch_end_time
end

function ServerActivityData:GetFortuneBagNum()
	return self.fortune_bag_data.is_fetch_reward == 0 and 1 or 0
end

function ServerActivityData:GetFetchBuyFlag(seq)
	return self.fortune_bag_data.is_buy_flag[32-seq] or 0
end

function ServerActivityData:ClearFortuneBagData()
	self.fortune_bag_data:Clear()
end

--税率优惠
function ServerActivityData:GetTaxDiscountRate()
	local config = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_TAX_DISCOUNT,Config.tax_discount_auto.tax_rate_discount)
	return config[1].reduce_tax_rate / 10 or 100
end

--天降好礼
function ServerActivityData:SetHeavenGiftInfo(protocol)
	self.heaven_gift_data.receive_count = protocol.receive_count
	self.heaven_gift_data.is_refresh_box = protocol.is_refresh_box
end

function ServerActivityData:GetHeavenGiftCount()
	return self.heaven_gift_data.receive_count
end

function ServerActivityData:IsHasHeavenGiftCount()
	return  Config.tianjianghaoli_auto.other[1].reward_frequency > self.heaven_gift_data.receive_count
end

function ServerActivityData:IsHeavenGiftCount(id)
	if Config.tianjianghaoli_auto.case_coordinate[1].treasure_chest_id == id then
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_HEAVEN_GIFT) then
			PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
			return true
		end
		if not self:IsHasHeavenGiftCount() then
			PublicPopupCtrl.Instance:Center(Language.HeavenGift.NotTimesTip)
			return true
		end
	end
end

function ServerActivityData:IsRefreshingBox()
	return self.heaven_gift_data.is_refresh_box == 1
end

function ServerActivityData:GetHeavenGiftNum()
	return (self:IsHasHeavenGiftCount() and self:IsRefreshingBox() and self.heaven_gift_data.remind_check == false) and 1 or 0
end

function ServerActivityData:ResetHeavenGiftRemind()
	self.heaven_gift_data.remind_check = false
end

function ServerActivityData:GetHeavenGiftShow()
	for _,v in pairs(Config.tianjianghaoli_auto.activity_interface) do 
		if v.is_show == 1 then
			return v.interface
		end
	end
	return 1
end

-- 伙伴兑换
function ServerActivityData:SetPartnerExchange(protocol)
	self.partner_exchange_data.ra_partner_exchange_times = protocol.ra_partner_exchange_times
end

function ServerActivityData:GetPartnerExchangeTimes(seq)
	if self.partner_exchange_data.ra_partner_exchange_times then
		return self.partner_exchange_data.ra_partner_exchange_times[seq] or 0
	end
	return 0
end

function ServerActivityData:GetPartnerExchangeCurTimes(data)
	local times = self:GetPartnerExchangeTimes(data.seq1)
	local need_times = data.buy_tepy == 1 and data.number or data.buy_seccond_day
	return (need_times - times) > 0 and (need_times - times) or 0
end

function ServerActivityData:GetPartnerExchangeConfig()
	return self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_PARTNER_EXCHANGE,Config.partner_activity_auto.partner_exchange)
end

-- 红点逻辑·伙伴兑换
function ServerActivityData:GetPartnerExchangeRemind()
	if self:GetPartnetExchangeSave() then
		return 0
	end
	for k,v in pairs(self:GetPartnerExchangeConfig()) do 
		local flag_enough = true
		if self:GetPartnerExchangeCurTimes(v) > 0 then
			for i = 1,3 do 
				if v["item"..i] ~= nil and v["item"..i].num ~= nil then
					flag_enough = flag_enough and (Item.GetNum(v["item"..i].item_id) >= v["item"..i].num)
				end 
			end
			if flag_enough then 
				return 1 
			end
		end
	end
	return 0
end

function ServerActivityData:SetPartnetExchangeSave(flag)
	self.partnet_exchang_save.val = flag or false
end

function ServerActivityData:GetPartnetExchangeSave()
	return self.partnet_exchang_save.val or false
end

-- 宠物兑换
function ServerActivityData:SetPetExchange(protocol)
	self.pet_exchange_data:Set(protocol.info)
end

function ServerActivityData:SetPetExchangeOnceInfo(protocol)
	if TableIsEmpty(self.pet_exchange_data) then
		return
	end
	local info = protocol.info
	local seq = info.seq
	if info.type == 0 then
		self.pet_exchange_data.ra_pet_exchange_times[seq] = info.p1
		self.pet_exchange_data.day_pet_exchange_times[seq] = info.p2
	else
		self.pet_exchange_data.shop_item_times[seq] = info.p1
	end
	self.pet_exchange_data:Notify()
end

function ServerActivityData:GetPetExchangeShopRemain(cfg)
	local info = self.pet_exchange_data.shop_item_times or {}
	return cfg.buy_times - (info[cfg.seq] or 0)
end

function ServerActivityData:GetPetExchange()
	return self.pet_exchange_data.ra_pet_exchange_times
end

function ServerActivityData:GetPetExchangeViewList(type_)
	local cur_show_list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_PET_EXCHANGE,Config.pet_exchange_activity_auto.pet_exchange)
	local list = {}
	for k,v in pairs(cur_show_list) do 
		if v.tab == type_ then 
			table.insert( list, v )
		end 
	end 
	return list
end

function ServerActivityData:GetPetTagList()
	local cur_show_list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_PET_EXCHANGE,Config.pet_exchange_activity_auto.pet_exchange)
	local list = {}
	for k,v in pairs(cur_show_list) do 
		local flag_with = false
		for i,j in pairs(list) do 
			if j.tab == v.tab then 
				flag_with = true
				break
			end
		end 
		if not flag_with then 
			local vo = {}
			vo.tab = v.tab
			table.insert( list, vo)
		end 
	end 	
	table.sort(list,DataSorter.KeyLowerSorter("tab"))
	return list
end

--折扣狂欢
function ServerActivityData:GetDCReward()
	return self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DISCOUNT_CARNIVAL,Config.discount_carnival_auto.reward,"seq")
end

function ServerActivityData:GetDCCard()
	local cfg = Config.discount_carnival_auto.coupon
	return Config.discount_carnival_auto.coupon
end

function ServerActivityData:SetDCInfo(list)
	self.discount_carnival_info:Set(list)
end

function ServerActivityData:GetDCInfo()
	return self.discount_carnival_info
end

function ServerActivityData:IsCanBuy(seq,buy_times)
	return (self.discount_carnival_info[seq] or 0) < buy_times, (self.discount_carnival_info[seq] or 0)
end

function ServerActivityData:SetDCWillBuy(data)
	self.discount_carnival_buy = data
end

function ServerActivityData:GetDCWillBuy()
	return self.discount_carnival_buy
end

function ServerActivityData:SetDCUse(info, isOn)
	if isOn then
		self.discount_carnival_data.info = info
	else
		if self.discount_carnival_data.info == info then
			self.discount_carnival_data.info = nil
		end
	end
end

function ServerActivityData:GetDCUse()
	return self.discount_carnival_data.info
end

function ServerActivityData:GetDiscountCarnivalShow()
	for _,v in pairs(Config.discount_carnival_auto.activity_interface) do
		if v.is_show==1 then
			return v.interface
		end
	end
	return 1
end

--折扣狂欢红点逻辑
function ServerActivityData:GetDCRemindNum()
	local list = self:GetDCReward()
	for _,v in pairs(list) do
		if v.original_price == 0 then
		    if v.buy_tepy == 1 then
		        local flag, num = self:IsCanBuy(v.seq1, v.buy_times)
		        num = v.buy_times - num
		        if num > 0 then
		            return 1
		        end
		    else
		        local flag, num = self:IsCanBuy(v.seq1, v.buy_seccond_day)
		        num = v.buy_seccond_day - num
		        if num > 0 then
		            return 1
		        end
		    end
		end
	end
	return 0
end

--幸运转盘
function ServerActivityData:SetLuckyTurntableInfo(protocol)
	self.luck_roller_data.info = protocol.info
end

function ServerActivityData:SetLuckyTurntableReward(protocol)
	self.luck_roller_data.type = protocol.type
	self.luck_roller_data.index = protocol.index
	self.luck_roller_data.val = not self.luck_roller_data.val
end

function ServerActivityData:GetLuckyTurntableInfo()
	return self.luck_roller_data.info
end

function ServerActivityData:GetLuckyTurntableProgress(seq)
	local progress = self:GetLuckyTurntableInfo().progress or {}
	return progress[seq] or 0
end

function ServerActivityData:SetLuckyTurntableSave(flag)
	self.luck_roller_save = flag or false
end

function ServerActivityData:GetLuckyTurntableSave()
	return self.luck_roller_save or false
end

function ServerActivityData:GetLuckyTurntableFlag(seq)
	local flag = self:GetLuckyTurntableInfo().flag or {}
	return flag[33-seq] or 0
end

function ServerActivityData:GetLuckRollerRedNum()
	return self.luck_roller_data.info.last_num or 0
end

--是否有封印提升
function ServerActivityData:IsCanPromote(pet_id)
	if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SEAL_PROMOTE) then
		return false
	end
	if self.promote_config == nil then
		local config = Config.seal_promote_auto.probability_promote
		self.promote_config = KeyList(self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SEAL_PROMOTE,config,"seq"),"pets_id")
	end
	return self.promote_config[pet_id] ~= nil
end

--幸运折扣
function ServerActivityData:SetLuckyDiscountInfo(protocol)
	self.lucky_discount_data.discount = protocol.discount
	self.lucky_discount_data.buy_count = protocol.buy_count
	self.lucky_discount_data.today_buy_count = protocol.today_buy_count
end

function ServerActivityData:GetLuckyDiscountNum()
	return self.lucky_discount_data.discount
end

function ServerActivityData:GetLuckyDiscountTimes(data)
	return data.limit_type == 1 and (self.lucky_discount_data.buy_count[data.seq] or 0) or (self.lucky_discount_data.today_buy_count[data.seq] or 0)
end

function ServerActivityData:IsLuckyDiscountTry()
	return self.lucky_discount_data.is_try
end

--幸运折扣红点
function ServerActivityData:GetLuckyDiscountRemaidNum()
	for k,v in pairs(Config.lucky_discount_auto.reward) do
		if v.original_price == 0 then
			if v.frequency - self:GetLuckyDiscountTimes(v) ~= 0 then
				return 1
			end
		end
	end
	return 0
end

--图鉴夺宝
function ServerActivityData:SetIllustrateTreasureInfo(protocol)
	self.llustrate_treasure_data:Set(protocol.info)
end

function ServerActivityData:GetIllustrateTreasureFlag(seq)
	if self.llustrate_treasure_data.day_fetch_flag then
		return self.llustrate_treasure_data.day_fetch_flag[32-seq] or 0
	end
	return 0
end

function ServerActivityData:GetIllustrateTreasureData(seq)
	if self.llustrate_treasure_data.task_list then
		return self.llustrate_treasure_data.task_list[seq] or {}
	end
end

function ServerActivityData:GetIllustrateTreasureDays()
	return self.llustrate_treasure_data.login_day or 0
end

function ServerActivityData:GetIllustrateTreasureTimeStamp()
	return self.llustrate_treasure_data.end_timestamp or 0
end

function ServerActivityData:GetIllustrateTreasureEnd()
	if self:GetIllustrateTreasureTimeStamp() - TimeHelper.GetServerTime() < 0 then
		return false
	end
	local day_fetch_num = self.llustrate_treasure_data.day_fetch_num or 0

	local is_task_end = self.llustrate_treasure_data.is_task_end or false

	if is_task_end == true and day_fetch_num == #Config.tujian_treasure_auto.login_reward then
		return false
	end
	return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_ILLUSTRATE_TREASURE)
end

function ServerActivityData:GetIllustrateTreasureRemind()
    local days = self:GetIllustrateTreasureDays()
	for k,v in pairs(Config.tujian_treasure_auto.login_reward) do
		if self:GetIllustrateTreasureFlag(v.seq) == 0 and days >= v.sign_day then
			return 1
		end
	end
	for k,v in pairs(Config.tujian_treasure_auto.treasure_reward) do
		local info = self:GetIllustrateTreasureData(v.seq)
		if info.is_fetch == 0 and info.count >= v.parameter3 then
			return 1
		end
	end
	return 0
end

--元素翻牌
function ServerActivityData:GetElementCardsCfg()
	return Config.element_cards_auto
end

function ServerActivityData:SetElementCardsData(protocol)
	self.element_cards_data.flop_seq = protocol.info.flop_seq
	self.element_cards_data.flop_count = protocol.info.flop_count
end

function ServerActivityData:GetElementCardsData()
	return self.element_cards_data
end

function  ServerActivityData:GetElementShowInterface()
	if not  self.elementInterface then
		local cfg=self:GetElementCardsCfg().activity_interface
		for _,v in pairs(cfg) do
			if v.is_show==1 then
				self.elementInterface = v.interface
				break
			end
		end
	end
	return self.elementInterface
end

--红点逻辑
function ServerActivityData:GetElementCardsNum()
	return self.element_cards_data.flop_count
end

--连登好礼
function ServerActivityData:GetSignInContinuityCfg()
	return Config.sign_in_continuity_auto
end

function ServerActivityData:GetSignInContinuityDayList()
	local cfg = self:GetSignInContinuityCfg()
	local day_list = self:GetSectionList(cfg.reward,"seq")
	return day_list
end

function ServerActivityData:SetSignInContinuityData(protocol)
	self.sign_in_continuity_data.login_day = protocol.info.login_day
	self.sign_in_continuity_data.reward_flag_t = protocol.info.reward_flag_t
end

function ServerActivityData:GetSignInContinuityData()
	return self.sign_in_continuity_data
end

function ServerActivityData:GetSignInContinuityRewardFlag(day,seq)
	local sign_data = self:GetSignInContinuityData()
	if sign_data.login_day >= day then
		local flag = sign_data.reward_flag_t[32 - seq]
		if flag ~= nil then
			if flag == 0 then
				return SignInContinuityView.REWARD_FLAG.CAN
			else
				return SignInContinuityView.REWARD_FLAG.GETED
			end
		else
			return SignInContinuityView.REWARD_FLAG.NOT
		end
	else
		return SignInContinuityView.REWARD_FLAG.NOT
	end
end

function ServerActivityData:GetSignInContinuityShow()
	for _,v in pairs(self:GetSignInContinuityCfg().activity_interface) do
		if v.is_show ==1 then
			return v.interface
		end
	end
	return 1
end
--连登好礼红点逻辑
function ServerActivityData:GetSignInContinuityRemindNum()
	local list = self:GetSignInContinuityDayList()
	for _,v in pairs(list) do
		local flag = self:GetSignInContinuityRewardFlag(v.day,v.seq)
		if flag == SignInContinuityView.REWARD_FLAG.CAN then
			return 1
		end
	end
	return 0
end

--传承瑰宝
function ServerActivityData:SetHeritageTreasureInfo(protocol)
	self.heritage_treasure_data.piece_list = protocol.piece_list
	self.heritage_treasure_data.raward_flag = protocol.raward_flag
end

function ServerActivityData:GetHeritageTreasurePieceList()
	return self.heritage_treasure_data.piece_list or {}
end

function ServerActivityData:GetHeritageTreasurePieceData(index)
	return self.heritage_treasure_data.piece_list[index]
end

function ServerActivityData:SetHeritageTreasureSelectData(data)
	self.heritage_treasure_select.data = data
end

function ServerActivityData:GetHeritageTreasureSelectData()
	return self.heritage_treasure_select.data
end

--传承瑰宝红点
function ServerActivityData:GetHeritageTreasureNum()
	if self.heritage_treasure_data.raward_flag == 1 then
		return 0
	end
	local num = Item.GetNum(Config.treasure_activity_auto.other[1].consume_item_id)
	for k,v in pairs(self:GetHeritageTreasurePieceList()) do
		if v.clue < 3 and num > 0 then
			return 1
		end
		if v.clue == 3 then
            return 1
        end
	end
	return self:GetHeritageTreasureGiftNum()
end

function ServerActivityData:GetHeritageTreasureGiftNum()
	if self.heritage_treasure_data.raward_flag == 1 then
		return 0
	end
	local num = 0
	for k,v in pairs(self:GetHeritageTreasurePieceList()) do
		if v.clue > 3 then
            num = num + 1
        end
	end
	return num >= RA_HERITAGE_TREASURE_TREASURE_MAX_PIECE and 1 or 0
end

-- 专属礼包
function ServerActivityData:SetExclusiveGiftInfo(protocol)
	-- 为了确保只有有效数据，这里处理一下，因为不一定5个都有数据
	local info = {}
	for i, v in pairs(protocol.info) do
		-- 这里去除掉没有激活的，还有达到购买次数的，还有已经过了结束时间的
		
		if v.is_active > 0 
			and self:GetExclusiveGiftConfigure(v.type) ~= nil 
			and	v.buy_times < self:GetExclusiveGiftConfigure(v.type).limit_num 
			and v.end_time > TimeManager.GetServerTime() then
			-- 这里还需要处理活动时间与礼包时间的冲突
			local t = DataHelper.TableCopy(v)
			local act_time =  ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_EXCLUSIVE_GIFT).next_status_switch_time
			t.end_time = act_time > t.end_time and t.end_time or act_time
			t.gift_type = ServeractivityConfig.ExclusiveGiftType.TYPE_DEFAULT
			table.insert(info, t)
		end
	end
	table.sort(info, DataSorter.KeyLowerSorter("type"))
	self.exclusive_gift_data.lv_info = info
	self:FlushExclusiveGiftAllInfo()
end

function ServerActivityData:SetExclusiveGiftDayInfo(protocol)
	local info = {}
	for i, v in ipairs(protocol.day_item) do
		if v.phase > 0 and v.end_time > TimeManager.GetServerTime() then
			local config = self:GetExclusiveGiftDayConfigure(v.phase, v.type)
			if config ~= nil and v.buy_times < config.limit_num then
				local t = DataHelper.TableCopy(v)
				local act_time = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_EXCLUSIVE_GIFT).next_status_switch_time
				t.end_time = act_time > t.end_time and t.end_time or act_time
				t.gift_type = ServeractivityConfig.ExclusiveGiftType.TYPE_DAY_GIFT
				table.insert(info, t)
			end
		end
	end
	self.exclusive_gift_data.day_info = info
	self.exclusive_gift_data.day_init = true
	self:FlushExclusiveGiftAllInfo()
end

function ServerActivityData:FlushExclusiveGiftAllInfo()
	local temp_info = {}
	local count = 0
	local max_num = 10
	for i, v in ipairs(self.exclusive_gift_data.lv_info) do
		table.insert(temp_info, v)
		count = count + 1
	end
	-- 如果最后一个礼包的结束时间不一致，证明存在新的礼包
	if #temp_info > 0 and not self:IsEqualExclusiveGiftKeyValue(temp_info[#temp_info].end_time) then
		self.exclusive_gift_data.new_page = #temp_info
		self.exclusive_gift_data.has_red_point = true
	end
	local new_list = {}
	for i, v in ipairs(self.exclusive_gift_data.day_info) do
		if count < max_num then
			table.insert(new_list, v)
			count = count + 1
		end
	end
	for i = #new_list, 1, -1 do
		table.insert(temp_info, new_list[i])
	end
	--如果和最后一次不一致，证明存在新的每日礼包
	if self.exclusive_gift_data.day_init == true then
		if #new_list > 0 and not self:IsEqualExclusiveGiftDayKeyValue() then
			self.exclusive_gift_data.new_page = #temp_info
			self.exclusive_gift_data.has_red_point = true
		end
	end
	self.exclusive_gift_data.info = temp_info

	local is_need_open = false
	-- 新增每日首次收到新的等级专属礼包和每日专属礼包需要自动打开
	for k, v in pairs(self.exclusive_gift_data.info) do
		local config = {}
		if v.gift_type == ServeractivityConfig.ExclusiveGiftType.TYPE_DEFAULT then
			config = self:GetExclusiveGiftConfigure(v.type)
		else
			config = self:GetExclusiveGiftDayConfigure(v.phase, v.type)
		end
		if config.is_popup == 1 then
			local str = DTNewTodayNotRemind.ExclusiveGift .. config.gift_id
			if not DialogTipsData.Instance:TodayIsTrue(str) then
				is_need_open = true
				DialogTipsData.Instance:SetTodayRemind(str)
			end
		end
	end
	if is_need_open then
		if not ViewMgr:IsOpen(WeekGiftView) and not ViewMgr:IsOpen(ExclusiveGiftView) then
			ViewMgr:OpenView(ExclusiveGiftView)
		end
	end
end

function ServerActivityData:GetExclusiveGiftInfo(num)
	return num and self.exclusive_gift_data.info[num] or self.exclusive_gift_data.info
end

function ServerActivityData:GetExclusiveGiftConfigure(index)
	for k, v in pairs(Config.exclusive_gift_auto.gift_configure) do
		if v.type == index then
			return v
		end
	end
end

-- 如果玩家持续在线，倒计时结束需要清除掉该数据
function ServerActivityData:ExclusiveGiftTimeCheck()
	if #self.exclusive_gift_data.info > 0 then
		-- 一秒容错
		if TimeManager.GetServerTime() > self.exclusive_gift_data.info[1].end_time - 1 then
			local new_info = {}
			for i, v in pairs(DataHelper.TableCopy(self.exclusive_gift_data.lv_info)) do
				if not (TimeManager.GetServerTime() > v.end_time - 1) then
					table.insert(new_info, v)
				end
			end
			self.exclusive_gift_data.lv_info = new_info
			
			new_info = {}
			for i, v in pairs(DataHelper.TableCopy(self.exclusive_gift_data.day_info)) do
				if not (TimeManager.GetServerTime() > v.end_time - 1) then
					table.insert(new_info, v)
				end
			end
			self.exclusive_gift_data.day_info = new_info
			self:FlushExclusiveGiftAllInfo()
		end
	end
end

function ServerActivityData:GetExclusiveGiftKeyValue()
	local key = PrefKeys.ActivityIsRemind(RoleData.Instance:GetRoleId(), ACTIVITY_TYPE.RAND_EXCLUSIVE_GIFT)
	return UnityPlayerPrefs.GetString(key)
end

function ServerActivityData:SetExclusiveGiftKeyValue(value)
	local key = PrefKeys.ActivityIsRemind(RoleData.Instance:GetRoleId(), ACTIVITY_TYPE.RAND_EXCLUSIVE_GIFT)
	UnityPlayerPrefs.SetString(key, tostring(value))
end

function ServerActivityData:IsEqualExclusiveGiftKeyValue(value)
	return self:GetExclusiveGiftKeyValue() == tostring(value)
end

function ServerActivityData:GetExclusiveGiftDayConfigure(phase, index)
	if self.exclusive_gift_day_list == nil then
		self.exclusive_gift_day_list = {}
		for k, v in pairs(Config.exclusive_gift_auto.gift_day) do
			self.exclusive_gift_day_list[v.phase] = self.exclusive_gift_day_list[v.phase] or {}
			self.exclusive_gift_day_list[v.phase][v.type] = v
		end
	end
	if self.exclusive_gift_day_list[phase] ~= nil then
		return self.exclusive_gift_day_list[phase][index]
	end
end

function ServerActivityData:GetExclusiveGiftDayKeyValue()
	local key = PrefKeys.ActivityIsRemind(RoleData.Instance:GetRoleId(), ACTIVITY_TYPE.RAND_EXCLUSIVE_GIFT) .. "_Day"
	return UnityPlayerPrefs.GetString(key)
end
function ServerActivityData:SetExclusiveGiftDayKeyValue()
	if #self.exclusive_gift_data.day_info > 0 then
		local value = 0
		for i, v in pairs(self.exclusive_gift_data.day_info) do
			value = math.max(v.end_time, value)
		end
		local key = PrefKeys.ActivityIsRemind(RoleData.Instance:GetRoleId(), ACTIVITY_TYPE.RAND_EXCLUSIVE_GIFT) .. "_Day"
		UnityPlayerPrefs.SetString(key, tostring(value))
	end
end
function ServerActivityData:IsEqualExclusiveGiftDayKeyValue()
	local value = 0
	if #self.exclusive_gift_data.day_info > 0 then
		for i, v in pairs(self.exclusive_gift_data.day_info) do
			value = math.max(v.end_time, value)
		end
	end
	return self:GetExclusiveGiftDayKeyValue() == tostring(value)
end

function ServerActivityData:ResetExclusiveGiftRemind()
	self.exclusive_gift_data:Set({info = {}, has_red_point = false, day_info = {}, lv_info = {}})
end

--新修改
function ServerActivityData:IsExclusiveGiftDayType()
	local info = self:GetExclusiveGiftInfo()
    if info ~= nil and info[1] ~= nil then
        return info[1].gift_type == ServeractivityConfig.ExclusiveGiftType.TYPE_DAY_GIFT
    end
	return false
end

function ServerActivityData:IsHasExclusiveGift()
	return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_EXCLUSIVE_GIFT) and #self:GetExclusiveGiftInfo() > 0
end

function ServerActivityData:GetGiftInfoList()
	local list = {}
	if self:IsHasExclusiveGift() then
		for k,v in pairs(self:GetExclusiveGiftInfo()) do
			table.insert(list,v)
		end
	end
	if self:IsHasWeedendGift() then
		for k,v in pairs(self:GetWeedendGiftInfo()) do
			table.insert(list,v)
		end
	end
	if #list > 1 then
		table.sort(list,DataHelper.SortFunc([[end_time]]))
	end
	return list[1],#list
end

-- 财神聚宝
function ServerActivityData:GetWealthCornucopiaCfg()
	return Config.wealth_cornucopia_auto.cornucopia_configure
end

function ServerActivityData:GetWealthCornucopiaPercentage()
	return Config.wealth_cornucopia_auto.other[1].percentage or 0
end

function ServerActivityData:SetWealthCornucopiaInfo(protocol)
	self.wealth_cornucopia_info.buy_day_times = protocol.buy_day_times
	self.wealth_cornucopia_info.buy_sum_times = protocol.buy_sum_times
end

-- 获取财神聚宝符合的item 这里不走GetActivityOpenCfg接口，需额外加工
function ServerActivityData:GetWealthCornucopiaListByDayNum(day_num)
	day_num = day_num or TimeCtrl.Instance:GetActOpenServerDay(ACTIVITY_TYPE.RAND_WEALTH_CORNUCOPIA)
	self.wealth_cornucopia_list = {}
	local cfg = self:GetWealthCornucopiaCfg()
	for i,v in pairs(cfg) do
		if day_num >= v.section_start and (day_num <= v.section_end or v.section_end == 0) then
			self.wealth_cornucopia_list[v.seq + 1] = v
			self.wealth_cornucopia_list[v.seq + 1].is_can_buy = self:GetWealthCornucopiaItemIsBuy(v.seq)
	    end
	end
	-- 是否可购 > seq   排序下标要从1开始，坑
    local sort_func = function (a,b)
    	if a.is_can_buy == b.is_can_buy then
    		-- if a.gold_num == b.gold_num then
    		-- 	return a.type < b.type
    		-- else
    		-- 	return a.gold_num < b.gold_num
    		-- end
    		return a.seq < b.seq
    	else
    		return a.is_can_buy > b.is_can_buy
    	end
    end
    table.sort(self.wealth_cornucopia_list,sort_func)
    return self.wealth_cornucopia_list
end

-- 获取财神聚宝的item是否可以购买 1 = true
function ServerActivityData:GetWealthCornucopiaItemIsBuy(seq)
	seq = seq + 1
	local data = self.wealth_cornucopia_list[seq]
	if data.limit_type == 1 then
		local num = self.wealth_cornucopia_info.buy_sum_times[data.seq] or 0
		return num < data.buy_times and 1 or 0
	else
		local num = self.wealth_cornucopia_info.buy_day_times[data.seq] or 0
		return num < data.today_times and 1 or 0
	end
end

----------限时宝盒
function ServerActivityData:SetGiftBoxInfo(protocol)
	self.gift_box_data.day_buy_times = protocol.day_buy_times
	self.gift_box_data.buy_times = protocol.buy_times
	self.gift_box_data.has_free_reward = protocol.has_free_reward
end

function ServerActivityData:GetGiftBoxDayBuyTimes(seq)
	return self.gift_box_data.day_buy_times[seq] or 0
end

function ServerActivityData:GetGiftBoxBuyTimes(seq)
	return self.gift_box_data.buy_times[seq] or 0
end

function ServerActivityData:IsGiftBoxBuyHaveTimes(cfg)
	if cfg ~= nil then
		if cfg.limit_type == 1 then
			local buy_times = self:GetGiftBoxBuyTimes(cfg.seq)
			return buy_times < cfg.buy_times
		elseif cfg.limit_type == 2 then
			local buy_times = self:GetGiftBoxDayBuyTimes(cfg.seq)
			return buy_times < cfg.today_times
		end
	end
	return false
end

function ServerActivityData:GetGiftBoxItemList()
	local act_cfg = Config.gift_box_auto.gift_configure
	return self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_GIFT_BOX, act_cfg, "seq")
end

function ServerActivityData:GetGiftBoxShowList()
	local show_list = self:GetGiftBoxItemList()
	table.sort(show_list, DataHelper.WeightSort("seq", function(data)
		if not self:IsGiftBoxBuyHaveTimes(data) then
			return 100
		end
	end))
	--新增逻辑，front首位推荐
    local front, front_index = 0, 0
    for i, v in ipairs(show_list) do
        if v.front ~= 0 and self:IsGiftBoxBuyHaveTimes(v) and (front == 0 or front > v.front) then
            front = v.front
            front_index = i
        end
    end
    if front ~= 0 then
        table.insert(show_list, 1, table.remove(show_list, front_index))
    end
	return show_list
end

function ServerActivityData:GetGiftBoxRemindNum()
    return self.gift_box_data.has_free_reward == 0 and 1 or 0
end

function ServerActivityData:IsGiftBoxFreeGiftFetched()
    return self.gift_box_data.has_free_reward == 1
end

function ServerActivityData:GetGiftBoxFreeReward()
	local other_cfg = Config.gift_box_auto.other[1]
    return other_cfg.free_reward_item
end

function ServerActivityData:GetGiftBoxShowStyle()
	for i, v in ipairs(Config.gift_box_auto.activity_interface) do
		if v.is_show == 1 then
			return v.interface
		end
	end
	return 0
end

----------限时宝袋
function ServerActivityData:SetChainBoxInfo(protocol)
	self.chain_box_data.can_buy_seq = protocol.can_buy_seq
	self.chain_box_data.day_buy_times = protocol.day_buy_times
	self.chain_box_data.buy_times = protocol.buy_times
	self.chain_box_data.has_free_reward = protocol.has_free_reward
end
--档位是否开启
function ServerActivityData:GetChainBoxBuyIsOpen(seq)
	if seq == 0 then
		return true
	end
	return self.chain_box_data.can_buy_seq[32 - seq + 1] == 1
end

function ServerActivityData:GetChainBoxDayBuyTimes(seq)
	return self.chain_box_data.day_buy_times[seq] or 0
end
function ServerActivityData:GetChainBoxBuyTimes(seq)
	return self.chain_box_data.buy_times[seq] or 0
end

--判断是否还有购买次数
function ServerActivityData:IsChainBoxBuyHaveTimes(cfg)
	if cfg ~= nil then
		if cfg.limit_type == 1 then
			local buy_times = self:GetChainBoxBuyTimes(cfg.seq)
			return buy_times < cfg.buy_times
		elseif cfg.limit_type == 2 then
			local buy_times = self:GetChainBoxDayBuyTimes(cfg.seq)
			return buy_times < cfg.today_times
		end
	end
	return false
end

function ServerActivityData:GetChainBoxItemList()
	local act_cfg = Config.chain_box_auto.gift_configure
	return self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_CHAIN_BOX, act_cfg, "seq")
end

function ServerActivityData:GetChainBoxShowList()
	local show_list = self:GetChainBoxItemList()
	table.sort(show_list, DataHelper.WeightSort("seq", function(data)
		if not self:IsChainBoxBuyHaveTimes(data) then
			return 100
		end
	end))
	return show_list
end

function ServerActivityData:GetChainBoxRemindNum()
    return self.chain_box_data.has_free_reward == 0 and 1 or 0
end

function ServerActivityData:IsChainBoxFreeGiftFetched()
    return self.chain_box_data.has_free_reward == 1
end

function ServerActivityData:GetChainBoxFreeReward()
	local other_cfg = Config.chain_box_auto.other[1]
    return other_cfg.free_reward_item
end

function ServerActivityData:GetChainBoxShowStyle()
	for i, v in ipairs(Config.chain_box_auto.activity_interface) do
		if v.is_show == 1 then
			return v.interface
		end
	end
	return 0
end

-- 经验飙升
function ServerActivityData:SetExpPurchaseInfo(protocol)
	local info = {}
	info.buy_times = protocol.buy_times
	info.day_buy_times = protocol.day_buy_times
	self.exp_purchase_data.info = info
end

-- 获取当前等级对应的经验飙升数据
function ServerActivityData:GetExpPurchaseConfigure()
	if self.exp_configure == nil then
		self.exp_configure = DataHelper.TableCopy(Config.exp_purchase_auto.exp_configure)
		table.sort(self.exp_configure, DataSorter.KeyUpperSorter("grade"))
		self.now_grade = 0
	end
	local grade = RoleData.Instance:GetRoleLevel()
	if self.now_grade ~= grade then
		self.now_grade = grade
		for i, v in ipairs(self.exp_configure) do
			if grade >= v.grade then
				self.now_exp_configure = v
				break
			end
		end
	end
	return self.now_exp_configure
end

-- 获取当前经验飙升的限购类型，1是总限购，2是每日限购
function ServerActivityData:GetExpPurchaseLimitType()
	return Config.exp_purchase_auto.other[1].limit_type
end

-- 根据限购类型获取总限购次数
function ServerActivityData:GetExpPurchaseLimitTimes()
	-- 1是总限购，2是每日限购
	if self:GetExpPurchaseLimitType() == 1 then
		return Config.exp_purchase_auto.other[1].buy_times
	else
		return Config.exp_purchase_auto.other[1].today_times
	end
end

-- 根据限购类型获取剩余限购次数
function ServerActivityData:GetExpPurchaseHasLimitTimes()
	local now_count = self:GetExpPurchaseLimitTimes()
	if self:GetExpPurchaseLimitType() == 1 then
		now_count = now_count - (self.exp_purchase_data.info.buy_times or 0)
	else
		now_count = now_count - (self.exp_purchase_data.info.day_buy_times or 0)
	end
	return now_count
end

-- 是否达到购买限制
function ServerActivityData:GetExpPurchaseIsReachLimit()
	return self:GetExpPurchaseHasLimitTimes() < 1
end

-- 是否满级
function ServerActivityData:GetExpPurchaseIsReachLevelMax()
	return RoleData.Instance:GetRoleLevel() >= ROLE_MAX_LEVEL
end

-- 判断该等级道具是否足够
function ServerActivityData:GetExpPurchaseIsReachItem()
	local configure = self:GetExpPurchaseConfigure()
	return Item.GetNum(configure.item_id) >= configure.item_num
end

-- 判断是否有红点
function ServerActivityData:GetExpPurchaseRemind()
	if not self:GetExpPurchaseIsReachLevelMax() and not self:GetExpPurchaseIsReachLimit() then
		if self:GetExpPurchaseIsReachItem() then
			return 1
		end	
	end
	return 0
end

-- 赠30连抽
function ServerActivityData:SetGivePetDrawInfo(protocol)
	self.give_pet_draw_data.give_mark = protocol.give_mark
end

function ServerActivityData:GetActPetSearchEnd()
	local config = Config.pet_search_auto.other[1]
	local role_level = RoleData.Instance:GetRoleLevel()
	return role_level >= config.open and role_level < config.close
end

function ServerActivityData:GetActPetSearchNum()
	return self.give_pet_draw_data.give_mark == 0 and 1 or 0
end

----------------------------------------------------------
--额外奖励
----------------------------------------------------------
function ServerActivityData:SetRandActivityBonusInfo(protocol)
	self.bonus_data.flag = protocol.bonus_flag
	self.bonus_data.val = not self.bonus_data.val
end

function ServerActivityData:SetBonusNum(act_type,curr_num,need_num)
	self.bonus_data.nums_list[act_type] = curr_num or 0
	self.bonus_data.need_list[act_type] = need_num or 1
	self.bonus_data.val = not self.bonus_data.val
end

function ServerActivityData:GetBonusInfo(act_type,type)
	local info = self:GetBonusConfig(act_type,type)
	if info == nil then 
		return 
	end
	info.curr_num = self.bonus_data.nums_list[act_type] or 0
	if type ~= nil then
		local nums_list = self.bonus_data.nums_list[act_type] or {}
		info.curr_num = nums_list[type] or 0
	end
	info.need_num = self.bonus_data.need_list[act_type] or 1
	info.flag = self.bonus_data.flag[info.seq] or 0
	return info
end

function ServerActivityData:GetBonusConfig(act_type,type)
	local config = {}
	for k,v in pairs(Config.extra_bonus_auto.bonus_cfg) do
		if v.activity_id == act_type and v.type == (type or 0) then
			table.insert(config,v)
		end
	end
	if act_type == 2073 then
		return self:GetActOpenDayCfg(act_type,config)[1]
	else
		return self:GetSectionList(config)[1]
	end
end

--获取额外奖励是否有红点
function ServerActivityData:GetBonusRedNum(act_type,type)
	if not ActivityData.IsOpen(act_type) then
		return 0
	end
	local info = self:GetBonusInfo(act_type,type)
	if info.type_item == 1 and info.flag == 0 and info.curr_num >= info.need_num then
		return 1
	end
	return 0
end

----------------------------------------------------------
--联动送礼
----------------------------------------------------------
function ServerActivityData:SetRALinkageGiftInfo(protocol)
	self.linkage_gift_data.gift_seq = protocol.gift_seq
    self.linkage_gift_data.fetch_flag = protocol.fetch_flag
    self.linkage_gift_data.can_receive_tamestamp = protocol.can_receive_tamestamp
end

function ServerActivityData:GetLinkageGiftEnd()
	return self.linkage_gift_data.fetch_flag ~= 2
end

function ServerActivityData:GetLinkageGiftNum()
	return self.linkage_gift_data.fetch_flag == 1 and 1 or 0
end

function ServerActivityData:GetLinkageGiftRewardCfg(seq)
    for _,v in pairs(Config.linkage_gift_auto.linkage_reward) do
        if v.seq == seq then
            return v
        end
    end
    return nil
end

----------------------------------------------------------
--奇遇礼包
----------------------------------------------------------
function ServerActivityData:SetRaAdventureGiftInfo(protocol)
	self.adventure_gift_data.gift_seq = protocol.gift_seq
    self.adventure_gift_data.end_time =protocol.end_time
end

function ServerActivityData:GetAdventureGiftData(type)
	for k,v in pairs(Config.adventure_gift_activity_auto.adventure_gift) do
		if v.type == type then
			return v
		end
	end
	return nil
end

function ServerActivityData:GetAdventureGiftEnd()
	local end_time = self.adventure_gift_data.end_time
	return end_time > 0 and (end_time - TimeCtrl.Instance:GetServerTime()) > 0
end

----------------------------------------------------------
--全民团购
----------------------------------------------------------
function ServerActivityData:SetRAGroupDiscountInfo(protocol)
	self.qm_discount_data:Set(protocol.commodity_info)
end

function ServerActivityData:GetGroupDiscountInfo(type_id)
    return self.qm_discount_data[type_id] or {}
end

function ServerActivityData:GetGroupDiscountCfg(seq)
	local list = {}
	for k,v in pairs(Config.qm_discount_auto.discount_form) do
		if v.seq == seq then
            table.insert(list,v)
        end
	end
	table.sort(list,function(a,b) 
		return a.discount_number < b.discount_number
	end)
    return list
end

--红点逻辑
function ServerActivityData:GetGroupDiscountNum()
	local config = Config.qm_discount_auto.group_buying
	for k,v in pairs(self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_QM_DISCOUNT,config)) do
		if v.original_price == 0 then
            local info = self:GetGroupDiscountInfo(v.type)
            local count = v.buy_tepy == 1 and v.buy_seccond or v.buy_seccond_day
            if count - (info.buy_count or 0) > 0 then
                return 1
            end
        end
	end
    return 0
end

---------------------------
-- 牛运当头
---------------------------
function ServerActivityData:GetCowOriginConfig()
	return Config.monopoly_gift_auto
end

function ServerActivityData:GetCowOriginData()
	return self.cow_origin_data
end

function ServerActivityData:SetCowOriginMapData(protocol)
	if protocol.info.now_map_index > self:GetCowOriginItemMaxCount() then
		protocol.info.now_map_index = self:GetCowOriginItemMaxCount()
	end
	self.cow_origin_data.map_info = protocol.info
end

function ServerActivityData:SetCowOriginConvertData(protocol)
	self.cow_origin_data.convert_infos = protocol.convert_infos
end

function ServerActivityData:SetCowOriginAnswerData(protocol)
	self.cow_origin_data.question_id = protocol.question_id
end

function ServerActivityData:GetCowOriginMapData()
	return self.cow_origin_data.map_info
end

function ServerActivityData:GetCowOriginConvertData()
	return self.cow_origin_data.convert_infos
end
function ServerActivityData:GetCowOriginAnswerData()
	return self.cow_origin_data.question_id
end

function ServerActivityData:GetCowOriginConvertList()
	return self:GetCowOriginConfig().num_exchange
end

function ServerActivityData:GetCowOriginMapId()
	return self.cow_origin_data.map_info.now_map_id or 1
end

function ServerActivityData:GetCowOriginItemMaxCount()
	return 23
end

function ServerActivityData:GetCowOriginMapCellList(map_id)
	map_id = map_id or self:GetCowOriginMapId()
	local act_all_cfg = self:GetCowOriginConfig().activity_configure
	local cfg = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_COW_ORIGIN,act_all_cfg,"seq")
	local list = {}
	for i = 1, #cfg do
		if cfg[i].layer_number == map_id then
			table.insert(list,cfg[i])
		end
	end
	--list[#list].layer_number_type = 6
	return list
end

function ServerActivityData:CowOriginConvertRemindNum()
	for i,v in pairs(self:GetCowOriginConvertList()) do
		local today_times = self.cow_origin_data.convert_infos[v.item_seq] or 0
		local limit_times = v.today_times
		if today_times < limit_times then
			local is_can = true
			for _,v2 in pairs(v.consume_item) do
				if Item.GetNum(v2.item_id) < v2.num then
					is_can = false
				end
			end
			if is_can == true then
				return 1
			end
		end
	end
	return 0
end

function ServerActivityData:CowOriginRemindNum()
	if self:GetCowOriginUseCount() > 0 then
		return 1
	end
	return self:CowOriginConvertRemindNum()
end

-- 这也是神奇，改为道具消耗了
function ServerActivityData:GetCowOriginUseCount()
	local item_id = self:GetCowOriginConfig().other[1].cost_item
	return Item.GetNum(item_id) or 0
end

-- 这个格子是否玩过了
function ServerActivityData:IsPlayed(data)
	local map_data = ServerActivityData.Instance:GetCowOriginMapData()
	local now_map_index = map_data.now_map_index
	local is_played = data.seq < now_map_index - 1 or (data.seq == now_map_index - 1 and map_data.has_play_special_plan == 1)
	return is_played
end

function ServerActivityData:GetCowOriginTodayRemind()
	local act_status = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_COW_ORIGIN)
	if act_status ~= nil and UnityPlayerPrefs.GetInt(PrefKeys.ActOpenRemindSp(act_status.act_type)) ~= act_status.param_2 then
		return 1
	end
	return 0
end
function ServerActivityData:SaveCowOriginTodayRemind()
	local act_status = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_COW_ORIGIN)
	if act_status ~= nil then
		UnityPlayerPrefs.SetInt(PrefKeys.ActOpenRemindSp(act_status.act_type), act_status.param_2)
	end
end

----------------------------
-- 牛运兑换
----------------------------
function ServerActivityData:GetCowConvertConfig()
	return Config.cow_exchange_auto
end

function ServerActivityData:GetCowConvertList()
	local cfg = self:GetCowConvertConfig().gift_configure
	local list_data = self:GetSectionList(cfg,"seq")
	for i,v in pairs(list_data) do
		v.buy_flag = self:GetCowConvertBuyFlag(v)
	end
	local sort_func = function (a,b)
		if a.buy_flag == b.buy_flag then
    		return a.seq < b.seq
    	else
    		return a.buy_flag > b.buy_flag
    	end
	end
	table.sort(list_data,sort_func)
	return list_data
end

function ServerActivityData:SetCowConvertData(protocol)
	self.cow_convert_data.unlock_integral = protocol.unlock_integral
	self.cow_convert_data.score = protocol.score
	self.cow_convert_data.today_buy_times = protocol.today_buy_times
	self.cow_convert_data.act_buy_times = protocol.act_buy_times
end

function ServerActivityData:GetCowConvertData()
	return self.cow_convert_data
end

function ServerActivityData:GetCowConvertBuyCount(seq,limit_type)
	limit_type = limit_type or 1
	if limit_type == 1 then
		return self.cow_convert_data.act_buy_times[seq] or 0
	else
		return self.cow_convert_data.today_buy_times[seq] or 0
	end
end

function ServerActivityData:GetCowConvertBuyFlag(data)
	local cow_convert_data = self:GetCowConvertData()
	local buy_count = self:GetCowConvertBuyCount(data.seq,data.limit_type)
	local limit_count = data.limit_type == 1 and data.buy_times or data.today_times
	-- 解锁条件判断
	if cow_convert_data.unlock_integral < data.unlock_integral then
		return 0
	end
	-- 够买限制判断
	if limit_count <= buy_count then
		return -1
	end
	return 1
end

----------------------------------------------------------
--小年送礼
----------------------------------------------------------
function ServerActivityData:SetRALittleYearGiftInfo(protocol)
	self.little_year_gift_data.thread_id = protocol.thread_id
	self.little_year_gift_data.has_get_reward = protocol.has_get_reward
end

function ServerActivityData:GetLittleYearGiftThreadId()
	return self.little_year_gift_data.thread_id
end

function ServerActivityData:IsLittleYearGiftReward()
	return self.little_year_gift_data.has_get_reward == 1
end

function ServerActivityData:GetLittleYearGiftThread(id)
	for k,v in pairs(Config.little_year_gift_auto.thread_bank) do
		if v.thread_id == id then
			return v
		end
	end
end

function ServerActivityData:GetLittleYearGiftRewardData(id)
	local config = Config.little_year_gift_auto.gift_configure
	for k,v in pairs(self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_LITTLE_YEAR_GIFT,config)) do
		if v.thread_id == id then
			return DataHelper.FormatItemList(v.reward_item)
		end
	end
end

function ServerActivityData:GetLittleYearGiftNum()
	if self:IsLittleYearGiftReward() then
		return 0
	end
	return self.little_year_gift_data.val and 1 or 0
end

-------------------------------------------------------
--牛行大运
-------------------------------------------------------
function ServerActivityData:SetRACowGoingLuckInfo(protocol)
	-- 第二天
	if self.cow_going_lock_data.today_reward ~= protocol.today_reward 
	   and self.cow_going_lock_data.twice and protocol.today_reward == 0 then 
		self.cow_going_lock_data.twice = false
	end 

	self.cow_going_lock_data.today_reward = protocol.today_reward
	self.cow_going_lock_data.id = protocol.id
end

function ServerActivityData:SetCowGoingLuckTwice()
	self.cow_going_lock_data.twice = true
end 

function ServerActivityData:GetCowGoingLuckRemind()
	if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_COW_GOING_LUCK) then
		return 0
	end
	return self.cow_going_lock_data.today_reward == 0 and 1 or 0
end

-- 年货节
function ServerActivityData:SetSpringFestivalStoreInfo(protocol)
	self.spring_festival_store_data.info = protocol.info
end

function ServerActivityData:GetSpringFestivalStoreMaxPhase()
	return Config.spring_festival_store_auto.other[1].phase
end

-- 根据阶段返回对应数据，并且处理购买完的放最后(阶段，是否是上期)
function ServerActivityData:GetSpringFestivalStoreDataByPrase(phase, is_last)
	if phase < 1 then
		return {}
	end
	if not self.spring_festival_store_cfg then
		self.spring_festival_store_cfg = {}
	end
	-- 这里根据开服第几天开的活动来决定用哪些数据
	local server_act_open_day = TimeCtrl.Instance:GetActOpenServerDay(ACTIVITY_TYPE.RAND_SPRING_STORE)
	if not self.spring_festival_store_cfg[server_act_open_day] then
		self.spring_festival_store_cfg[server_act_open_day] = {}
		for k, v in pairs(Config.spring_festival_store_auto.gift_configure) do
			if v.start_time <= server_act_open_day and v.end_time >= server_act_open_day then
				table.insert(self.spring_festival_store_cfg[server_act_open_day], v)
			elseif v.start_time <= server_act_open_day and v.end_time == 0 then
				table.insert(self.spring_festival_store_cfg[server_act_open_day], v)
			end
		end
		table.sort(self.spring_festival_store_cfg[server_act_open_day], DataSorter.KeyLowerSorter("type"))
	end
	-- 根据这张表再获取阶段数据
	local tab = {}
	for i, v in ipairs(self.spring_festival_store_cfg[server_act_open_day]) do
		if v.phase == phase then
			-- 这里的times直接就是剩余购买次数，1是活动限购，2是每日限购
			if v.limit_type == 1 then
				v.times = is_last and v.buy_times or v.buy_times - self.spring_festival_store_data.info.activity_buy_times[v.seq + 1]
			elseif v.limit_type == 2 then
				v.times = is_last and v.today_times or v.today_times - self.spring_festival_store_data.info.today_buy_times[v.seq + 1]
			end
			v.is_last = is_last
			table.insert(tab, v)
		end
	end
	-- 是上期的就不用是否买完排序了
	if not is_last then
		table.sort(tab, DataHelper.WeightSort("type", function(data)
			return data.times == 0 and 1000 or 0
		end))
	end
	return tab
end

------------------------ 赠送寻宝次数 ------------
function ServerActivityData:GetGiveSecretKeyCfg()
	return Config.give_secret_key_auto
end

function ServerActivityData:SetGiveSecretKeyData(protocol)
	self.give_secret_key_data.info = protocol.info
end

function ServerActivityData:GetGiveSecretKeyData(protocol)
	return self.give_secret_key_data
end

function ServerActivityData:GetGiveSecretKeyAllRewardNum()
	return self:GetGiveSecretKeyCfg().other[1].num_target
end

-- 0 = 不可领取，1 = 可领取，-1 = 领取过了
function ServerActivityData:GetGiveSecretKeyRewardFlag(data)
	if self.give_secret_key_data.info.reward_flag == nil then
		return 0
	end
	if self.give_secret_key_data.info.reward_flag[32 - data.seq] == 1 then
		return -1
	end
	if data.type == 1 then
		if RoleData.Instance:GetRoleLevel() >= data.parameter then
			return 1
		else
			return 0
		end
	elseif data.type == 2 then
		if self:GetGiveSeretKeyIsFightCrystalPassFb(data.parameter) then
			return 1
		else
			return 0
		end
	end
	return 0
end

-- 奖励是否全部领完了
function ServerActivityData:GetGiveSeretKeyIsOpen()
	local list_data = self:GetGiveSecretKeyCfg().target
	local flag
	for i,v in pairs(list_data) do
		flag = self:GetGiveSecretKeyRewardFlag(v)
		if flag == 1 or flag == 0 then
			return true
		end 
	end
	return false
end

function ServerActivityData:GetGiveSeretKeyRemindNum()
	local list_data = self:GetGiveSecretKeyCfg().target
	for i,v in pairs(list_data) do
		if self:GetGiveSecretKeyRewardFlag(v) == 1 then
			return 1
		end 
	end
	return 0
end
-- 赠寻宝钥匙活动是否通过副本
function ServerActivityData:GetGiveSeretKeyIsFightCrystalPassFb(scene_seq)
	-- local index = math.floor(scene_seq / 32)
	-- local seq = scene_seq % 32
	-- local flag = self.give_secret_key_data.info.first_pass_flag[index]
	-- return (bit:uint_and(flag, (bit:_lshift(1, seq)))) > 0
	return self.give_secret_key_data.info.first_pass_flag[scene_seq] > 0
end

-- 记录活动图标挂件mono
function ServerActivityData:SetGiveSecretKeyActIconMono(mono)
	self.give_secret_key_data.icon_mono = mono
end

function ServerActivityData:GetGiveSecretKeyActIconMono()
	return self.give_secret_key_data.icon_mono
end

-- ----------
-- 神兽降临
-- ----------
function ServerActivityData:GetPetBefallCfg()
	return Config.pet_befall_auto
end

function ServerActivityData:SetPetBefallInfo(protocol)
	self.pet_befall_data.info = protocol.info
end

function ServerActivityData:SetPetBefallResult(protocol)
	self.pet_befall_data.result = protocol.info
end

function ServerActivityData:GetPetBefallData()
	return self.pet_befall_data
end

function ServerActivityData:GetPetBefallGiftData()
	return self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_PET_BEFALL,self:GetPetBefallCfg().gift_configure,"seq")
end

function ServerActivityData:GetPetBefallShowData()
	local show_data = {}
	local cfg = self:GetPetBefallGiftData()
	for i = 1,#cfg do
		if cfg[i].is_show == 1 then
			local item_id = cfg[i].reward_item.item_id
			if Item.GetType(item_id) == ItemType.Gift then
				local item_list,config = BagData.Instance:GiftItemList(item_id)
				for _,value in pairs(item_list or {}) do
					if value:ShowType() == ShowType.Pet then
						table.insert(show_data,{pet_id = value.item_id})
					end
				end
			else
				table.insert(show_data,{pet_id = item_id})
			end
		end
	end
	return show_data
end

function ServerActivityData:SetPetBefallMarqueeMsg(marquee_msg)
	self.pet_befall_marquee_msg = marquee_msg
end

function ServerActivityData:GetPetBefallMarqueeMsg()
	return self.pet_befall_marquee_msg
end

--  
function ServerActivityData:RecordPetBefallRemind(role_id)
	self.pet_befall_remind_data.info[role_id] = 1
end
-- 红点逻辑
function ServerActivityData:GetPetBefallRemind()
	if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_PET_BEFALL) then
		return 0
	end
	local cfg = self:GetPetBefallCfg().jackpot_configure[1]
	local stuff_id = cfg.item_id
	-- 每次登录有道具只提示一次
	if Item.GetNum(stuff_id) >= cfg.item_num then
		if self.pet_befall_remind_data.info[RoleData.Instance:GetRoleId()] == nil then
			return 1
		else
			return 0
		end
	else
		return 0
	end
end

function ServerActivityData:GetPetBefallStuffSource()
	local open_day = TimeCtrl.Instance:ActOpenDay(ACTIVITY_TYPE.RAND_PET_BEFALL)
	local act_type
	local cfg = self:GetPetBefallCfg()
	for i,v in pairs(cfg.view_name) do
		if open_day >= v.section_start and open_day <= v.section_end then
			act_type = v.act_type
			break
		end
	end
	if act_type == nil or not ActivityData.IsOpen(act_type) then
		return
	end
	local cfg = ActivityRandData.Instance:GetConfig(act_type)
	return cfg.mod_key
end

-- 神兽降临直购配置
function ServerActivityData:GetPetBefallStoreCfg()
	local act_cfg = self:GetPetBefallCfg()
	local cfg = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_PET_BEFALL,act_cfg.pet_befall_configure,"type")
	return cfg[1]
end

-- 神兽降临奖池道具
function ServerActivityData:GetPetBefallPoolItemId()
	local act_cfg = self:GetPetBefallCfg()
	local cfg = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_PET_BEFALL,act_cfg.jackpot_configure)
	return cfg[1].jackpot_item_id or 27680
end

------------------------累计充值------------------------
function ServerActivityData:SetAccRechargeData(protocol)
	self.acc_recharge_data.info = protocol.info
end

function ServerActivityData:GetAccRechargeConfig()
	return self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ACC_RECHARGE, Config.acc_recharge_auto.gift_configure)[1]
end

function ServerActivityData:GetAccRechargeRemind()
	if not self:GetAccRechargeIsAcc() then
		return self.acc_recharge_data.info.is_received_free < 1 and 1 or 0
	end
	if self.acc_recharge_data.info.is_received_free < 1 then
		return 1
	else
		return self.acc_recharge_data.info.now_recharge >= self:GetAccRechargeConfig().acc_price and self.acc_recharge_data.info.is_get_reward < 1 and 1 or 0
	end
end

function ServerActivityData:SetAccRechargeNeedOpenValue(value)
	if ActivityRandData.GetRemind(ACTIVITY_TYPE.RAND_ACC_RECHARGE) == 1 and value == 0 then
		ActivityRandData.Instance:CheckRandOpenData()
	end
	self.acc_recharge_data.val = not self.acc_recharge_data.val
	ActivityRandData.SetRemind(ACTIVITY_TYPE.RAND_ACC_RECHARGE,value)
end

-- 是否需要关闭主界面的入口
function ServerActivityData:GetAccRechargeIsOpen()
	if ActivityRandData.GetRemind(ACTIVITY_TYPE.RAND_ACC_RECHARGE) == 1 then
		return false
	end
	return self.acc_recharge_data.info.is_get_reward < 1
end

-- 判断是否是累计充值
function ServerActivityData:GetAccRechargeIsAcc()
	return self:GetAccRechargeConfig().acc_price ~= 0
end

------------------------宠物夺宝------------------------
function ServerActivityData:SetPetLootInfo(protocol)
	self.pet_loot_data:Set(protocol.info)
	self:GetPetLootConfig()
end

function ServerActivityData:GetPetLootConfig()
	if self.pet_loot_data.final_flag == nil then
		return
	end
	if self.pet_loot_config == nil then
		self.pet_loot_config = {}
	end
	-- 将协议和表的数据整合到一起
	self.pet_loot_config.task_finish_times = self.pet_loot_data.task_finish_times
	for i, v in ipairs(Config.pet_loot_auto.show_configure) do
		v.final_flag = self.pet_loot_data.final_flag[i]
	end
	self.pet_loot_config.list_info = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_PET_LOOT, Config.pet_loot_auto.gift_configure, "type")
	for i, v in ipairs(self.pet_loot_config.list_info) do
		v.task_info = self.pet_loot_data.task_info[v.seq + 1]
		v.sort_id = v.task_info.has_get_reward_times >= v.reward_times and v.type + 1000 or v.type
	end
	-- 排序id，领取完的要滞后
	table.sort(self.pet_loot_config.list_info, DataSorter.KeyLowerSorter("sort_id"))
	return self.pet_loot_config
end

-- 宠物夺宝红点
function ServerActivityData:GetPetLootRemind()
	if not self.pet_loot_data.task_finish_times then
		return 0
	end
	if self:GetPetLootShowRemind() == 1 then
		return 1
	else
		for i, v in ipairs(self.pet_loot_config.list_info) do
			if v.task_info.can_get_reward_times > 0 then
				return 1
			end
		end
	end
	return 0
end

-- 宠物夺宝阶段奖励红点
function ServerActivityData:GetPetLootShowRemind()
	if not self.pet_loot_data.task_finish_times then
		return 0
	end
	for i, v in ipairs(Config.pet_loot_auto.show_configure) do
		if self.pet_loot_data.task_finish_times >= v.task_num then
			if v.final_flag == 0 then
				return 1
			elseif i == #Config.pet_loot_auto.show_configure then
				return 0
			end
		else
			return 0
		end
	end
end

-- 宠物夺宝返回当前阶段展示奖励表和是否已领取最后一个阶段奖励
function ServerActivityData:GetPetLootShowInfo()
	if not self.pet_loot_data.task_finish_times then
		return 0
	end
	for i, v in ipairs(Config.pet_loot_auto.show_configure) do
		if v.final_flag == 0 then
			return v, false
		elseif i == #Config.pet_loot_auto.show_configure then
			return v, true
		end
	end
end

------------------------交易行折扣------------------------
function ServerActivityData:SetTradeMarketReturnGoldInfo(protocol)
	self.deal_discount_data.info = protocol.deal_discount_info
end

function ServerActivityData:SetTradeMarketReturnHotItemInfo(protocol)
	self.deal_discount_data.item_list = protocol.item_list
end

function ServerActivityData:GetDealDiscountInfo()
	return self.deal_discount_data.info
end

function ServerActivityData:GetDealDiscountItemList()
	return self.deal_discount_data.item_list
end

--交易行折扣
function ServerActivityData:GetDealDiscountRate()
	local config = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DEAL_DISCOUNT,Config.trade_discount_auto.deal_discount)
	return config[1].reduce_discount or 0
end

--交易行折扣 
function ServerActivityData:GetDealDiscountNum()
	local remain_gold = self:GetDealDiscountInfo().remain_calaimable_gold or 0
	return remain_gold > 0 and 1 or 0
end

-- 好礼转动下发
function ServerActivityData:SetGiftRotationInfo(protocol)
	self.gift_rotation_data.fetch_num = protocol.fetch_num
	self.gift_rotation_data.fetch_flags = protocol.fetch_flags
	self.gift_rotation_data.val = not self.gift_rotation_data.val
	-- 强制提醒刷新主界面红点
	self.gift_rotation_data:Notify()
end

-- 好礼转动抽取到的礼盒下发
function ServerActivityData:SetGiftRotationGetInfo(protocol)
	self.gift_rotation_data.now_get = protocol.info
end

-- 好礼转动对应礼包seq1是否有领取
function ServerActivityData:GetGiftRotationIsReceive(seq1)
	if self.gift_rotation_data.fetch_flags[seq1 + 1] then
		return self.gift_rotation_data.fetch_flags[seq1 + 1]  == 1
	end
	return false
end

-- 好礼转动红点
function ServerActivityData:GiftRotationRemind()
	return self.gift_rotation_data.fetch_num > 0 and 1 or 0
end

-- 好礼转动颜值道具延迟下发播报
function ServerActivityData:SetHearsayInfo(protocol)
	self.gift_rotation_data.op_type = protocol.op_type
	self.gift_rotation_data.hear_item = protocol.param1
end

-- 春日登峰
function ServerActivityData:SetSpringClimbInfo(protocol)
	self.spring_climb_data.info = protocol.info
	self.spring_climb_data.get_item_flags = protocol.get_item_flags
end

function ServerActivityData:GetSpringClimbData()
	local data = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SPRING_CLIMB, Config.may_day_activity_auto.spring_climb, "seq")
	table.sort(data, DataSorter.KeyLowerSorter("type"))
	local spring_config = {}
	for i, v in ipairs(data) do
		if not spring_config[v.layer + 1] then
			spring_config[v.layer + 1] = {}
		end
		table.insert(spring_config[v.layer + 1], v)
	end
	return spring_config
end

function ServerActivityData:GetSpringClimbRemind()
	local role_id = RoleData.Instance:GetRoleId()
	local key = PrefKeys.ActivityIsRemind(role_id, ACTIVITY_TYPE.RAND_SPRING_CLIMB)
	local act_info = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_SPRING_CLIMB)
	local other = Config.may_day_activity_auto.other[1]
	-- 判断是否活动期间第一次登陆
	local is_act_first = false
	if UnityPlayerPrefs.GetString(key) ~= tostring(act_info.param_1) then
		is_act_first = true
	end
	return (is_act_first or Item.GetNum(other.item_id_1) >= other.item_num_1) and 1 or 0
end

function ServerActivityData:SetSpringClimbPref()
	local role_id = RoleData.Instance:GetRoleId()
	local key = PrefKeys.ActivityIsRemind(role_id, ACTIVITY_TYPE.RAND_SPRING_CLIMB)
	local act_info = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_SPRING_CLIMB)
	UnityPlayerPrefs.SetString(key, tostring(act_info.param_1))
	self.spring_climb_data.remind_flag = not self.spring_climb_data.remind_flag
end

-- 特惠秘宝
function ServerActivityData:SetSecretTreasureInfo(protocol)
	self.secret_treasure_data.phase = protocol.phase
	self.secret_treasure_data.is_open_today_flag =  protocol.is_open_today_flag
	self.secret_treasure_data.today_buy_times = protocol.today_buy_times
	self.secret_treasure_data.activity_buy_times = protocol.activity_buy_times
end 

function ServerActivityData:GetSecrectTreasureCfg()
	local result = {}
	result.cur_list = {}
	result.last_list = {}
	local function copyConfig(cfg)
		local vo = {}
		for k,v in pairs(cfg) do 
			vo[k] = v
		end 
		return vo
	end 
	for k,v in pairs(self:GetSecretTreasureConfig()) do 
		if v.phase == self:GetSecretTreasurePhase() then 
			local vo = copyConfig(v)
			vo.is_buyed = self:GetSecrectTreasureBuyTime(vo.limit_type == 2 and vo.seq + 1 or vo.type + 1,vo.limit_type) 
			>= ( vo.limit_type == 1 and vo.buy_times or vo.today_times) and 1 or 0 
			table.insert(result.cur_list,vo)
		end 
		if self:GetSecretTreasurePhase() - 1 == v.phase then 
			local vo = copyConfig(v)
			vo.is_buyed = self:GetSecrectTreasureBuyTime(vo.limit_type == 2 and vo.seq + 1 or vo.type + 1,vo.limit_type) 
			>= ( vo.limit_type == 1 and vo.buy_times or vo.today_times) and 1 or 0 
			table.insert(result.last_list,vo)
		end 
	end 
	table.sort(result.cur_list, function(a, b)
		local itemA = a.is_buyed *100 + a.type
		local itemB = b.is_buyed *100 + b.type
        if itemB > itemA then
            return true
        else
            return false
        end
	end)
	if self.secrect_treasure_cfg == nil then 
		self.secrect_treasure_cfg = KeyList(self:GetSecretTreasureConfig(),"type")
	end 
	for k,v in pairs(result.cur_list) do v.panel_index = k end 
	for k,v in pairs(result.last_list) do v.panel_index = k end 
	return result
end 

function ServerActivityData:GetSecrectTreasureBuyTime(seq,limit)
	if limit == 1 then
		return self.secret_treasure_data.activity_buy_times[seq]
	else 
		return self.secret_treasure_data.today_buy_times[seq]
	end 
end

function ServerActivityData:GetLastPhaseBuy(type)
	local cfg = self.secrect_treasure_cfg[type]
	local buyed_time = self.secret_treasure_data.activity_buy_times[type + 1]
	local all_time = cfg.limit_type == 1 and cfg.buy_times or cfg.today_times * (cfg.section_end - cfg.section_start)
	return buyed_time,all_time
end

function ServerActivityData:GetSecrectWithNextPhase()
	for k,v in pairs(self:GetSecretTreasureConfig()) do 
		if v.phase == self:GetSecretTreasurePhase() + 1 then 
			return true
		end
	end
	return false
end

-------------------------------------------------------
--特惠秘宝
-------------------------------------------------------
function ServerActivityData:GetSecretTreasurePhase()
	return self.secret_treasure_data.phase
end

function ServerActivityData:GetSecretTreasureConfig()
	return Config.may_day_activity_auto.special_offer_treasure
end

function ServerActivityData:GetSecretTreasureStampTime()
	return ActivityRandData.GetNextStampTime(ACTIVITY_TYPE.RAND_SECRET_TREASURE, self:GetSecretTreasureConfig(), self:GetSecretTreasurePhase())
end

-------------------------------------------------------
--劳动好礼
-------------------------------------------------------
function ServerActivityData:SetLabourGiftInfo(protocol)
	self.labour_gift_data.receive_count = protocol.receive_count
	self.labour_gift_data.is_refresh_box = protocol.is_refresh_box
end

function ServerActivityData:GetLabourGiftCount()
	return self.labour_gift_data.receive_count
end

function ServerActivityData:IsHasLabourGiftCount()
	return  Config.may_day_activity_auto.other[1].reward_frequency > self.labour_gift_data.receive_count
end

function ServerActivityData:IsLabourGiftCount(id)
	local is_labour_gift_id = false
	for k, v in pairs(Config.may_day_activity_auto.case_coordinate) do
		if v.treasure_chest_id == id then
			is_labour_gift_id = true
			break
		end
	end
	if is_labour_gift_id then
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_LABOUR_GIFT) then
			PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
			return true
		end
		if not self:IsHasLabourGiftCount() then
			PublicPopupCtrl.Instance:Center(Language.LabourGift.NotTimesTip)
			return true
		end
	end
end

function ServerActivityData:IsLabourGiftRefreshingBox()
	return self.labour_gift_data.is_refresh_box == 1
end

function ServerActivityData:GetLabourGiftNum()
	return (self:IsHasLabourGiftCount() and self:IsLabourGiftRefreshingBox() and self.labour_gift_data.remind_check == false) and 1 or 0
end

function ServerActivityData:ResetLabourGiftRemind()
	self.labour_gift_data.remind_check = false
end

-------------------------------------------------------
--组合贺礼
-------------------------------------------------------
function ServerActivityData:OnCombineGiftOpen()
	self:FlushCombineGiftOpenDay()
	self:FlushCombineGiftPhase()
end
function ServerActivityData:OnDayChangeCallCombineGift()
	self:FlushCombineGiftOpenDay()
	local cur_phase = self.combine_gift_data.phase
	self:FlushCombineGiftPhase()
end

function ServerActivityData:SetCombineGiftInfo(protocol)
	self.combine_gift_data.ra_day_buy_times = protocol.ra_day_buy_times
	self.combine_gift_data.ra_buy_times = protocol.ra_buy_times
end

function ServerActivityData:GetCombineGiftInfo()
	return self.combine_gift_data
end

function ServerActivityData:FlushCombineGiftOpenDay()
	local activity_status = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_COMBINE_GIFT)
	local open_day = 1
	if activity_status ~= nil then
		local open_time = TimeCtrl.Instance:ChangDataTime(activity_status.param_1)
		local server_time = TimeCtrl.Instance:ChangDataTime(TimeCtrl.Instance:GetServerTime())
		open_day = math.floor((server_time - open_time) / TIME_DAY_SECOND_NUM) + 1
	end
	self.combine_gift_data.open_day = open_day
end
function ServerActivityData:GetCombineGiftOpenDay()
	return self.combine_gift_data.open_day
end

function ServerActivityData:FlushCombineGiftPhase()
	local open_day = self:GetCombineGiftOpenDay()
	local phase = 1
	for i, v in pairs(self:GetCombineGiftCfg()) do
		if v[1].section_start <= open_day and open_day <= v[1].section_end then
			phase = v[1].phase
			break
		end
	end
	self.combine_gift_data.phase = phase
end
function ServerActivityData:GetCombineGiftPhase()
	return self.combine_gift_data.phase
end

function ServerActivityData:GetCombineGiftCfg()
	if self.combine_gift_cfg == nil then
		self.combine_gift_cfg = {}
		for i, v in pairs(Config.may_day_activity_auto.combination_configure) do
			if self.combine_gift_cfg[v.phase] == nil then
				self.combine_gift_cfg[v.phase] = {}
			end
			table.insert(self.combine_gift_cfg[v.phase], v)
		end
	end
	return self.combine_gift_cfg
end

function ServerActivityData:GetCombineGiftOff(phase)
	for i, v in pairs(Config.may_day_activity_auto.combination_off_configure) do
		if v.phase == phase then
			return v
		end
	end
end

function ServerActivityData:GetCombineGiftMaxPhase()
	local config = Config.may_day_activity_auto.combination_configure
	return config[#config].phase
end

function ServerActivityData:GetCombineGiftShowList()
	local show_list = self:GetCombineGiftShowListByPrase(self.combine_gift_data.phase)
	table.sort(show_list, DataHelper.WeightSort("seq", function(data)
		return self:GetCombineGiftRemainBuyTimes(data) < 1 and 1000 or 0
	end))
	return show_list
end

function ServerActivityData:GetCombineGiftRemainBuyTimes(data)
	if data.limit_type == 1 then
		return data.buy_times - (self.combine_gift_data.ra_buy_times[data.type] or 0)
	elseif data.limit_type == 2 then
		return data.today_times - (self.combine_gift_data.ra_day_buy_times[data.type] or 0)
	end
	return 0
end

function ServerActivityData:GetCombineGiftShowListByPrase(phase)
	local show_list = self:GetCombineGiftCfg()[phase] or {}
	table.sort(show_list, DataSorter.KeyLowerSorter("type"))
	return show_list
end

function ServerActivityData:SetCombineGiftRemind(num)
	local key = PrefKeys.ActivityIsRemind(RoleData.Instance:GetRoleId(), ACTIVITY_TYPE.RAND_COMBINE_GIFT)
	local open_day = TimeCtrl.Instance:GetCurOpenServerDay()
	UnityPlayerPrefs.SetInt(key, open_day)
	self.combine_gift_data.val = 0 --换号得重置下。
	self.combine_gift_data.val = open_day
end

function ServerActivityData:GetCombineGiftRemind()
	local key = PrefKeys.ActivityIsRemind(RoleData.Instance:GetRoleId(), ACTIVITY_TYPE.RAND_COMBINE_GIFT)
	local key_val = UnityPlayerPrefs.GetInt(key)
	local open_day = TimeCtrl.Instance:GetCurOpenServerDay()
	return key_val == open_day and 0 or 1
end

function ServerActivityData:SetCmdBuyItemCheckRet(protocol)
	self.cmd_buy_check_data.ret_info = {}
	self.cmd_buy_check_data.ret_info.buy_type = protocol.buy_type
	self.cmd_buy_check_data.ret_info.unique_id_1 = protocol.unique_id_1
	self.cmd_buy_check_data.ret_info.unique_id_2 = protocol.unique_id_2
	self.cmd_buy_check_data.ret_info.cfg_ver = protocol.cfg_ver
end

function ServerActivityData:GetCmdBuyItemCheckRet()
	return self.cmd_buy_check_data
end

-------------------------------------------------------
--令牌天赐
-------------------------------------------------------
function ServerActivityData:SetRAHeavenSentInfo(protocol)
	self.heaven_sent_data.recharge_num = protocol.recharge_num
	self.heaven_sent_data.reward_flag = protocol.reward_flag
	self.heaven_sent_data.val = not self.heaven_sent_data.val
end

function ServerActivityData:GetHeavenSentFlag(seq)
	return self.heaven_sent_data.reward_flag[seq] or 0
end

function ServerActivityData:GetHeavenSentRechargeNum()
	return self.heaven_sent_data.recharge_num or 0
end

function ServerActivityData:GetHeavenSentList()
	return self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_HEAVEN_SENT,Config.lingpai_divine_auto.gift_configure,"seq")
end

function ServerActivityData:GetHeavenSentRedNum()
    local recharge_num = self:GetHeavenSentRechargeNum()
	for k,v in pairs(self:GetHeavenSentList()) do
		if recharge_num >= v.acc_price and self:GetHeavenSentFlag(v.seq) == 0 then
			return 1
		end
	end
	return 0
end

function ServerActivityData:GetHeavenSentEnd()
	for k,v in pairs(self:GetHeavenSentList()) do
		if self:GetHeavenSentFlag(v.seq) == 0 then
			return true
		end
	end
	return false
end

-------------------------------------------------------
--心法无量
-------------------------------------------------------
function ServerActivityData:SetRAHeartSkillInfo(protocol)
	self.heart_skill_data.recharge_num = protocol.recharge_num
	self.heart_skill_data.reward_flag = protocol.reward_flag
	self.heart_skill_data.val = not self.heart_skill_data.val
end

function ServerActivityData:GetHeartSkillFlag(seq)
	return self.heart_skill_data.reward_flag[seq] or 0
end

function ServerActivityData:GetHeartSkillRechargeNum()
	return self.heart_skill_data.recharge_num or 0
end

function ServerActivityData:GetHeartSkillList()
	return self:GetSectionList(Config.xinfawuliang_auto.gift_configure,"seq")
end

function ServerActivityData:GetHeartSkillRedNum()
    local recharge_num = self:GetHeartSkillRechargeNum()
	for k,v in pairs(self:GetHeartSkillList()) do
		if recharge_num >= v.acc_price and self:GetHeartSkillFlag(v.seq) == 0 then
			return 1
		end
	end
	return 0
end

function ServerActivityData:GetHeartSkillEnd()
	for k,v in pairs(self:GetHeartSkillList()) do
		if self:GetHeartSkillFlag(v.seq) == 0 then
			return true
		end
	end
	return false
end

-------------------------------------------------------
--初夏历练
-------------------------------------------------------
function ServerActivityData:SetSummerExperienceInfo(protocol)
	self.summer_experience_data.open_day = protocol.open_day
	self.summer_experience_data.is_fetch_box = protocol.is_fetch_all_pass_flag
	self.summer_experience_data.first_flag = protocol.first_flag

	self.summer_experience_data.first_flag_num = 0
	for i, v in ipairs(protocol.first_flag) do
		if v == 1 then
			self.summer_experience_data.first_flag_num = self.summer_experience_data.first_flag_num + 1
		else
			break
		end
	end
end

function ServerActivityData:GetSummerExperienceConfig()
	if not self.summer_experience_config then
		self.summer_experience_config = {}
	end
	local day = self.summer_experience_data.open_day
	if not self.summer_experience_config[day] then
		self.summer_experience_config[day] = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SUMMER_EXPERIENCE, Config.burning_summer_auto.summer_lilian, "type")
		local open_time = TimeCtrl.Instance:ChangDataTime(ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_SUMMER_EXPERIENCE).param_1)
		for k, v in pairs(self.summer_experience_config[day]) do
			v.open_time = nil
			if v.activity_time > self.summer_experience_data.open_day then
				v.open_time = TimeCtrl.Instance:ChangDataTimeDetail(open_time + 86400 * (v.activity_time - 1))
			end
		end
	end
	return self.summer_experience_config[day]
end

function ServerActivityData:GetSummerExperienceRemind()
	local is_all_win = self.summer_experience_data.first_flag_num == #self:GetSummerExperienceConfig()
	return (is_all_win and self.summer_experience_data.is_fetch_box == 0) and 1 or 0
end

-------------------------------------------------------
--多彩花灯
-------------------------------------------------------
function ServerActivityData:GetFlowerLampCfg()
    return Config.burning_summer_auto.colorful_lantern
end

function ServerActivityData:GetFlowerLampItemCfg()
    return Config.burning_summer_auto.lantern_configuration
end

function ServerActivityData:GetFlowerLampItemData(type)
    local cfg = self:GetFlowerLampItemCfg()
    return cfg[type + 1]
end

function ServerActivityData:SetFlowerLampBaseInfo(protocol)
    self.flower_lamp_data.base_info = protocol.info
end

function ServerActivityData:SetFlowerLampGameInfo(protocol)
    self.flower_lamp_data.game_info = protocol.info
end

function ServerActivityData:SetFlowerLampItemInfo(protocol)
    local last_lamp_list = self.flower_lamp_data.item_info.lamp_list or {}
    local lamp_id_list = {}
    for i, v in pairs(protocol.info.lamp_list) do
        local n = (last_lamp_list[i] or 0) + 1
        for j = n, v do
            table.insert(lamp_id_list, i)
        end
    end
    self.flower_lamp_data.item_info = protocol.info
    self.flower_lamp_data.item_info.lamp_id_list = lamp_id_list
end

function ServerActivityData:FlowerLampRemainCount()
    local max_count = Config.burning_summer_auto.other[1].day_times
    return max_count - (self.flower_lamp_data.base_info.day_join_count or 0)
end

function ServerActivityData:GetFlowerLampTaskData()
    local cfg = self:GetFlowerLampCfg()
    local task_data = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_FLOWER_LAMP, cfg, "task_id")
    local day_task_list = {}
    local act_task_list = {}
    for i, v in pairs(task_data) do
        if v.task_type == 1 then
            table.insert(day_task_list, v)
        else
            table.insert(act_task_list, v)
        end
    end
    local sort_func = function(a, b)
        local a_flag = self:GetFlowerLampTaskFlag(a)
        local b_flag = self:GetFlowerLampTaskFlag(b)
        if a_flag == b_flag then
            return a.task_id < b.task_id
        end
        return a_flag > b_flag
    end
    table.sort(day_task_list, sort_func)
    table.sort(act_task_list, sort_func)
    return task_data, day_task_list, act_task_list
end

-- 1 可领取，0不可领取 -1 已领取
function ServerActivityData:GetFlowerLampTaskFlag(task_data)
    local task_id = task_data.task_id
    local flag = self.flower_lamp_data.base_info.task_flags[task_id]
    local progress_num = self:GetFlowerLampTaskProgress(task_data)
    if flag == 1 then
        return -1, progress_num
    else
        if progress_num >= task_data.param_1 then
            return 1, progress_num
        else
            return 0, progress_num
        end
    end
end

function ServerActivityData:GetFlowerLampTaskProgress(task_data)
    local task_param = task_data.task_param
    local task_type = task_data.task_type
    if task_param == 1 then
        return self.flower_lamp_data.base_info.day_max_score or 0
    elseif task_param == 2 then
        return self.flower_lamp_data.base_info.sum_score or 0
    elseif task_param == 3 then
        -- 每日任务
        if task_type == 1 then
            return self.flower_lamp_data.base_info.day_join_count or 0
        else
            return self.flower_lamp_data.base_info.act_join_count or 0
        end
    end
end

function ServerActivityData:FlowerLampTaskRemind(task_list)
    task_list = task_list or self:GetFlowerLampTaskData()
    for i, v in pairs(task_list) do
        local flag = self:GetFlowerLampTaskFlag(v)
        if flag > 0 then
            return 1
        end
    end
    return 0
end

-- 多彩花灯红点,是否已点击查看，是否有次数，是否有奖励领
function ServerActivityData:FlowerLampRemind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_FLOWER_LAMP) then
        return 0
    end
    if not self.flower_lamp_data.base_info.day_join_count then
        return 0
    end
    if self:FlowerLampTaskRemind() > 0 then
        return 1
    end
    if self:FlowerLampRemainCount() > 0 and not self.flower_lamp_look_flag then
        return 1
    end
    return 0
end

-------------------------------------------------------
--琼楼玉宴
-------------------------------------------------------
function ServerActivityData:GetBanquetCfg()
    return Config.burning_summer_auto.qionglou_banquet
end

-- 当前宴会的奖励
function ServerActivityData:GetBanquetRewardCfg(seq)
    local cfg = self:GetBanquetCfg()
    local datas = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_BANQUET, cfg, "seq")
    if seq then
        for i, v in pairs(datas) do
            if v.seq == seq then
                return v
            end
        end
    else
        local progress_num = self.banquet_data.base_info.progress_num
        for i, v in pairs(datas) do
            if progress_num >= v.prep_down and (progress_num <= v.prep_up or v.prep_up == 0) then
                return v
            end
        end
    end
end

function ServerActivityData:SetBanquetBaseInfo(protocol)
    self.banquet_data.base_info = protocol.info
end

function ServerActivityData:SetBanquetRewardInfo(protocol)
    self.banquet_data.reward_info = protocol.info
end

function ServerActivityData:GetBanquetRewardListByGroupId(group_id)
    local list = {}
    for i, v in pairs(Config.burning_summer_auto.banquet_group) do
        if v.reward_group == group_id then
            for j, v2 in pairs(v.reward_item) do
                table.insert(list, v2)
            end
        end
    end
    return list
end

function ServerActivityData:GetBanquetStuffRewardGroupId()
    return Config.burning_summer_auto.other[1].reward_group_2
end

function ServerActivityData:GetBanquetStuffListData()
    if self.banquet_stuff_list_data == nil then
        local cfg = Config.burning_summer_auto.other[1]
        self.banquet_stuff_list_data = {}
        for i = 1, 3 do
            local info = {}
            info.item_id = cfg["item_id_" .. i]
            info.index = i
            info.need_num = cfg.prep_num
            self.banquet_stuff_list_data[i] = info
        end
    end
    return self.banquet_stuff_list_data
end

function ServerActivityData:GetBanquetStuffItemList()
    local cfg = Config.burning_summer_auto.other[1]
    local banquet_stuff_item_list = {}
    for i = 1, 3 do
        local item_id = cfg["item_id_" .. i]
        local num = Item.GetNum(item_id)
        local data = {}
        local item = Item.Create({item_id = item_id, num = DataHelper.ConsumeNum(num, Config.burning_summer_auto.other[1].prep_num, true)})
        data.put_index = i
        data.item = item
        table.insert(banquet_stuff_item_list, data)
    end
    return banquet_stuff_item_list
end

-- 1 可领取，0未达成，-1已领取
function ServerActivityData:GetBanquetBoxFlag(data)
    if self.banquet_data.reward_info.box_reward_flags[data.seq] == 1 then
        return -1
    end
    local progress_num = self.banquet_data.base_info.progress_num
    if progress_num >= data.prep_down and data.sp_reward_group > 0 then
        return 1
    else
        return 0
    end
end

function ServerActivityData:GetBanquetRemind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_BANQUET) then
        return 0
    end
    -- 是否查看过
    if self.banquet_look_flag then
        return 0
    end
    -- 是否宴会开启
    if self.banquet_data.base_info.status == BanquetCfg.Status.Banquet then
        -- 是否筹备成功
        local cfg = self:GetBanquetRewardCfg()
        if cfg.seq > 0 then
            return 1
        end
    end
    -- 是否首次登陆
    local server_open_day = TimeCtrl.Instance:GetCurOpenServerDay()
    local report_day = ActivityRandData.GetRemind(ACTIVITY_TYPE.RAND_BANQUET)
    if not report_day or report_day < server_open_day then
        return 1
    end
    return 0
end

-------------------------------------------------------
--荷香觅宝
-------------------------------------------------------
function ServerActivityData:GetLotusCfg()
    local cfg = Config.cool_summer_auto.lotus_mibao
    return self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_LOTUS, cfg, "type")
end

function ServerActivityData:SetLotusInfo(protocol)
    self.lotus_data.info = protocol.info
end

function ServerActivityData:GetLotusActTimeDesc(act_type)
    act_type = act_type or ACTIVITY_TYPE.RAND_LOTUS
    if not ActivityData.IsOpen(act_type) then
        return Language.Activity.NotOpenTip
    end
    local act_info = ActivityData.Instance:GetActivityStatusInfo(act_type)
    local open_time = act_info.param_1
    local end_time = act_info.param_2 - 1
    local str1 = os.date("%m.%d", open_time)
    local str2 = os.date("%m.%d", end_time)
    return string.format("%s ~ %s", str1, str2)
end

function ServerActivityData:GetLotusGridListData()
    local cfg = self:GetLotusCfg()
    local list_data = {}
    local active_stuff_list = {} -- 激活材料
    for i, v in pairs(cfg) do
        local index = v.type + 1
        if v.line_num < 5 and index % 5 ~= 0 then
            table.insert(list_data, v)
            if not active_stuff_list[v.activation_item_id] then
                active_stuff_list[v.activation_item_id] = {item_id = v.activation_item_id, type = v.type}
            end
        end
    end
	local new_active_stuff_list = {}
	for k,v in pairs(active_stuff_list) do
		table.insert(new_active_stuff_list,v)
	end
	table.sort(new_active_stuff_list, function (a, b)
		return a.type < b.type
	end)
	table.sort(list_data, function (a, b)
		return a.type < b.type
	end)
	return list_data, new_active_stuff_list
end

function ServerActivityData:GetLotusColListData()
    local cfg = self:GetLotusCfg()
    local list_data = {}
    for i, v in pairs(cfg) do
        local index = v.type + 1
        if v.line_num == 5 and index % 5 ~= 0 then
            table.insert(list_data, v)
        end
    end
    return list_data
end

function ServerActivityData:GetLotusRowListData()
    local cfg = self:GetLotusCfg()
    local list_data = {}
    for i, v in pairs(cfg) do
        local index = v.type + 1
        if v.line_num < 5 and index % 5 == 0 then
            table.insert(list_data, v)
        end
    end
    return list_data
end

-- 是否已点亮
function ServerActivityData:IsLotusGridLighten(data)
    local index = (data.line_num - 1) * 4 + data.seq + 1
    if self.lotus_data.info.grid_list_active_flag[index] == 1 then
        return true
    else
        return false
    end
end

-- 是否全部点亮
function ServerActivityData:IsLotusAllLighten()
    local num = 0
    for i, v in pairs(self.lotus_data.info.grid_list_active_flag) do
        if v == 1 then
            num = num + 1
            if num >= 16 then
                return true
            end
        end
    end
    return false
end

-- 是否可提交碎片
function ServerActivityData:IsLotusGridPut(data)
    local item_num = Item.GetNum(data.activation_item_id)
    return item_num >= data.activation_item_num
end

-- 是否可领最后一个奖励 1 可，0不可，-1 已领
function ServerActivityData:IsLotusEndRewardFlag()
    if self.lotus_data.info.end_reward_flag == 1 then
        return -1
    end
    if not self:IsLotusAllLighten() then
        return 0
    else
        return 1
    end

end

function ServerActivityData:LotusRemindNum()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_LOTUS) then
        return 0
    end
    if not self.lotus_data.info or not next(self.lotus_data.info) then
        return 0
    end
    -- 每日首次登陆
    local server_open_day = TimeCtrl.Instance:GetCurOpenServerDay()
    local report_day = ActivityRandData.GetRemind(ACTIVITY_TYPE.RAND_LOTUS)
    if not report_day or report_day < server_open_day then
        return 1
    end
    -- 是否可提交碎片
    local grid_list_data = self:GetLotusGridListData()
    for i, v in pairs(grid_list_data) do
        if not self:IsLotusGridLighten(v) then
            if self:IsLotusGridPut(v) then
                return 1
            end
        end
    end
    -- 是否可领宝箱
    if self:IsLotusEndRewardFlag() > 0 then
        return 1
    end
    return 0
end

-------------------------------------------------------
--好运花签
-------------------------------------------------------
function ServerActivityData:SetRALuckySignInfo(protocol)
    self.lucky_sign_data.sign_id = protocol.sign_id
    self.lucky_sign_data.is_lock = protocol.is_lock
    self.lucky_sign_data.reward_type = protocol.reward_type
    self.lucky_sign_data.reward_flag = protocol.reward_flag
end

function ServerActivityData:GetLuckySignId()
    return self.lucky_sign_data.sign_id
end

function ServerActivityData:GetLuckySignLock()
    return self.lucky_sign_data.is_lock
end

function ServerActivityData:GetLuckySignRewatdList()
    local type = self.lucky_sign_data.reward_type or 0
    local config = Config.burning_summer_auto.huaqian_group[type]
    return DataHelper.FormatItemList(config.reward_item)
end

function ServerActivityData:GetLuckySignConfig()
    return Config.burning_summer_auto.sign_group_id[self.lucky_sign_data.sign_id]
end

function ServerActivityData:GetLuckySignNum()
    return self.lucky_sign_data.reward_flag == 0 and 1 or 0
end

-------------------------------------------------------
--元气翻牌
-------------------------------------------------------
function ServerActivityData:SetRAYuanQiTurningInfo(protocol)
    local info = protocol.info
    if info.open_index > 0 then
        info.result = Cfg.CoolCardInfoById(info.info_id)
        local co = Cfg.CoolCardRewardByType(info.reward_id)
        if co then
            local rewards = {}
            for i = 0, #co.reward_item do
                local item_data = co.reward_item[i]
                table.insert(rewards, Item.Init(item_data.item_id, item_data.num, item_data.is_bind))
            end
            info.rewards = rewards
        end
    end
    self.cool_card_data:Set(info)
end

-------------------------------------------------------
--凉风送爽
-------------------------------------------------------
function ServerActivityData:SetRACoolBreezeInfo(protocol)
    local info = protocol.info
    self.cool_breeze_data:Set(info)
end

function ServerActivityData:SetRACoolBreezeSingleInfo(protocol)
    if 1 == protocol.task_group_type then
        if self.cool_breeze_data.fixe_task_list then
            self.cool_breeze_data.fixe_task_list[protocol.index] = protocol.info
        end
    elseif 2 == protocol.task_group_type then
        if self.cool_breeze_data.random_task_list then
            self.cool_breeze_data.random_task_list[protocol.index] = protocol.info
        end
    end
    self.cool_breeze_data.flush_show = not self.cool_breeze_data.flush_show
end

function ServerActivityData:GetCoolBreezeShowList()
    local show_list = {}
    for i = 1, 3 do
        local info = self.cool_breeze_data.fixe_task_list[i]
        if info.type > 0 then
            table.insert(show_list, info)
        end
    end
    for i = 1, 2 do
        local info = self.cool_breeze_data.random_task_list[i]
        if info.type > 0 then
            table.insert(show_list, info)
        end
    end
    table.insert(show_list, {type = 0, task_refresh_timestamp_min = self.cool_breeze_data.task_refresh_timestamp_min})
    if #show_list < 3 then
        for i = 1, 3 - #show_list do
            table.insert(show_list, {type = -1})
        end
    end
    return show_list
end
-------------------------------------------------------
--觅宝坊店
-------------------------------------------------------
function ServerActivityData:SetSCRAMiBaoFangDianInfo(protocol)
    self.national_mibao_data.is_open_day_flag = protocol.is_open_day_flag
    self.national_mibao_data.is_need_recal_phase_flag = protocol.is_need_recal_phase_flag
	self.national_mibao_data.open_day_cfg = protocol.open_day_cfg
    self.national_mibao_data.activity_buy_times = protocol.activity_buy_times
    self.national_mibao_data.today_buy_times = protocol.today_buy_times
end

function ServerActivityData:SetSCRAMiBaoFangDianSingleInfo(protocol)
    self.national_mibao_data.is_change = not self.national_mibao_data.is_change
    self.national_mibao_data.today_buy_times[protocol.seq] = protocol.today_buy_times
    self.national_mibao_data.activity_buy_times[protocol.index] = protocol.activity_buy_times
end

--觅宝红点
function ServerActivityData:GetMiBaoRemindNum()
    return self:GetMiBaoOpenDayFlag() == 0 and 1 or 0
end

function ServerActivityData:GetMiBaoOpenDayFlag()
	return self.national_mibao_data.is_open_day_flag or 0
end

--获取觅宝坊店配置
function ServerActivityData:GetMiBaoStoreConfig()
	--return self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_NATIONAL_MIBAO_STORE,Config.national_day_auto.mibaofangdian)
	--获得当前数据
	self.national_mibao_data.info_list = {}
	local server_day = self.national_mibao_data.open_day_cfg
	for i,v in ipairs(Config.national_day_auto.mibaofangdian) do
		if v.section_start <= server_day and v.section_end >= server_day then
			table.insert(self.national_mibao_data.info_list, v)
		elseif v.section_start <= server_day and v.section_end == 0 then
			table.insert(self.national_mibao_data.info_list, v)
		end
	end
	return self.national_mibao_data.info_list
end

--觅宝坊店下一期时间戳
function ServerActivityData:GetMiBaoStoreStampTime(phase)
	local index = phase or 1
	return ActivityRandData.GetNextStampTime(ACTIVITY_TYPE.RAND_NATIONAL_MIBAO_STORE,self:GetMiBaoStoreConfig()[index])
end

function ServerActivityData:GetMiBaoStoreTimes(data)
	local info = data.limit_type == 1 and self.national_mibao_data.activity_buy_times or self.national_mibao_data.today_buy_times
	--return data.limit_type == 1 and (info[data.index] or 0) or (info[data.seq] or 0)
	return info[data.seq] or 0
end

--获取觅宝剩余次数
function ServerActivityData:GetMiBaoStoreHasTimes(data)
	local times = data.limit_type == 1 and data.buy_times or data.today_times
	return times - self:GetMiBaoStoreTimes(data)
end

--觅宝坊店根据活动开启时间返回阶段
function ServerActivityData:GetMiBaoStorePhase()
	return self:GetMiBaoStoreList()[1].phase or 1
end

--获取觅宝列表
function ServerActivityData:GetMiBaoStoreList()
	local list = self:GetSectionList(self:GetMiBaoStoreConfig(),nil,TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_NATIONAL_MIBAO_STORE),true)
	table.sort(list,DataHelper.WeightSort([[type]],function(data)
		if self:GetMiBaoStoreHasTimes(data) <= 0 then
			return 100
		end
	end))
	return list
end

-- 根据阶段返回对应数据
function ServerActivityData:GetMiBaoStoreDataByPhase(phase)
	local list = {}
	for k,v in pairs(self:GetMiBaoStoreConfig()) do
		if v.phase == phase then
			table.insert(list,v)
		end
	end
	return list
end

-------------------------------------------------------
--珍品行铺
-------------------------------------------------------
function ServerActivityData:SetSCRAPreciousDianInfo(protocol)
    self.national_precious_data.activity_buy_times = protocol.activity_buy_times
    self.national_precious_data.today_buy_times = protocol.today_buy_times
    self.national_precious_data.is_change = not self.national_precious_data.is_change
end
function ServerActivityData:SetOnSCRAZhenPinShangPuOpenDay(protocol)
    self.national_precious_data.open_day_cfg = protocol.day
	self.national_precious_data.info_list = nil
	self:GetPreciousStoreConfig()
end
function ServerActivityData:GetPreciousStorePhase()
    return self:GetPreciousStoreList()[1].phase or 1
end

function ServerActivityData:GetPreciousStoreConfig()
	--return self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_NATIONAL_PRECIOUS_STORE,Config.national_day_auto.zhenpinhangpu)
	if not self.national_precious_data.info_list then
		self.national_precious_data.info_list = {}
		local server_day = self.national_precious_data.open_day_cfg
	    for i,v in ipairs(Config.national_day_auto.zhenpinhangpu) do
			if v.section_start <= server_day and v.section_end >= server_day then
				table.insert(self.national_precious_data.info_list, v)
			elseif v.section_start <= server_day and v.section_end == 0 then
				table.insert(self.national_precious_data.info_list, v)
			end
		end
	end
    return self.national_precious_data.info_list
end

function ServerActivityData:GetPerciousRemindNum()
    return self:PreciousStoreLookFlag() and 0 or 1
end

function ServerActivityData:PreciousStoreLookFlag(flag)
	if flag then
		self.national_precious_data.national_precious_look_flag = true
		self.national_precious_data.is_change = not self.national_precious_data.is_change
	else
		return self.national_precious_data.national_precious_look_flag
	end
end

function ServerActivityData:GetPreciousStoreTimes(data)
	local info = data.limit_type == 1 and self.national_precious_data.activity_buy_times or self.national_precious_data.today_buy_times
	--return data.limit_type == 1 and (info[data.index] or 0) or (info[data.seq] or 0)
	for i = 0,#info do
		if info[i].type == data.type then
			return info[i].buy_times
		end
	end
	return 0
end

--珍品下一期时间戳
function ServerActivityData:GetPreciousStoreStampTime()
	return ActivityRandData.GetNextStampTime(ACTIVITY_TYPE.RAND_PRECIOUS_STORE,self:GetPreciousStoreList()[1])
end

--获取珍品剩余次数
function ServerActivityData:GetPreciousStoreHasTimes(data)
	local times = data.limit_type == 1 and data.buy_times or data.today_times
	return times - self:GetPreciousStoreTimes(data)
end

function ServerActivityData:GetPreciousStoreList()
	local list = self:GetSectionList(self:GetPreciousStoreConfig(),nil,TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_PRECIOUS_STORE),true)
	table.sort(list,DataHelper.WeightSort([[type]],function(data)
		if self:GetPreciousStoreHasTimes(data) <= 0 then
			return 100
		end
	end))
	return list 
end

function ServerActivityData:GetPreciousStoreRewardList(reward_group)
	return CacheList(self,"precious_reward_group",reward_group,function()
		local list = {}
		for i,v in ipairs(Config.national_day_auto.reward_group) do
			if v.item_id == reward_group then
				table.insert(list,v.reward_item)
			end
		end
		return list
	end)
end

-------------------------------------------------------
--拍卖行
-------------------------------------------------------
function ServerActivityData:GetPaiMaiHangOtherCfg()
	return Config.pai_mai_hang_auto.other[1]
end

function ServerActivityData:GetPaiMaiHangShowList()
	local base_list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_PAI_MAI_HANG, Config.pai_mai_hang_auto.gift_configure)
	local open_day = 1
	if ActivityData.IsOpen(ACTIVITY_TYPE.RAND_PAI_MAI_HANG) then
		open_day = TimeCtrl.Instance:ActOpenDay(ACTIVITY_TYPE.RAND_PAI_MAI_HANG)
	end
	local show_list = {}
	for i, v in pairs(base_list) do
		if v.activity_day == open_day then
			table.insert(show_list, v)
		end
	end
	table.sort(show_list, DataSorter.KeyLowerSorter("seq"))
	return show_list
end

function ServerActivityData:GetPaiMaiHangCfg(seq)
	for i, v in pairs(self:GetPaiMaiHangShowList()) do
		if v.seq == seq then
			return v
		end
	end
end

function ServerActivityData:GetPaiMaiHangRecordCfg(record_time, activity_day, seq)
	local timestamp1 = TimeCtrl.Instance:ChangDataTime(TimeCtrl.Instance:ServerStartTS())
	local timestamp2 = TimeCtrl.Instance:ChangDataTime(record_time)
	local open_day = math.floor((timestamp2 - timestamp1) / TIME_DAY_SECOND_NUM) + 1
	local base_list = self:GetSectionList(Config.pai_mai_hang_auto.gift_configure, nil, open_day)
	for i, v in pairs(base_list) do
		if v.activity_day == activity_day and v.seq == seq then
			return v
		end
	end
	return nil
end

function ServerActivityData:CheckPaiMaiHangRemind()
end

function ServerActivityData:GetPaiMaiHangBeginTime()
	return self.paimaihang_notice_info.pai_mai_start_timestamp
end

function ServerActivityData:GetPaiMaiHangMinAdd()
	return self:GetPaiMaiHangOtherCfg().pre_price
end

function ServerActivityData:SetPaiMaiHangNoticeInfo(protocol)
	self.paimaihang_notice_info.pai_mai_start_timestamp = protocol.pai_mai_start_timestamp
	self.paimaihang_notice_info.pai_mai_end_timestamp = protocol.pai_mai_end_timestamp
	self.paimaihang_notice_info.pai_mai_last_end_timestamp = protocol.pai_mai_last_end_timestamp
	self.paimaihang_notice_info.shop_item_list = protocol.shop_item_list
	self:CheckPaiMaiHangRemind()
end
function ServerActivityData:SetPaiMaiHangSingleNoticeInfo(protocol)
	self.paimaihang_notice_info.shop_item_list[protocol.seq] = protocol.shop_item_info
	self.paimaihang_notice_info.is_flush = not self.paimaihang_notice_info.is_flush
end

function ServerActivityData:SetPaiMaiHangUserInfo(protocol)
	self.paimaihang_user_info.shop_item_list = protocol.shop_item_list
end

function ServerActivityData:SetPaiMaiHangUserSingleInfo(protocol)
	self.paimaihang_user_info.shop_item_list[protocol.seq] = protocol.shop_item_info
	self.paimaihang_user_info.is_flush = not self.paimaihang_user_info.is_flush
end

function ServerActivityData:SetPaiMaiHangUserRecordInfo(protocol)
	self.paimaihang_record_info.record_count = protocol.record_count
	self.paimaihang_record_info.record_list = protocol.record_list
end

function ServerActivityData:GetPaiMaiHangNoticeItemInfo(seq)
	if seq ~= nil then
		return self.paimaihang_notice_info.shop_item_list[seq]
	end
	return self.paimaihang_notice_info.shop_item_list
end

function ServerActivityData:GetPaiMaiHangUserItemInfo(seq)
	if seq ~= nil then
		return self.paimaihang_user_info.shop_item_list[seq]
	end
	return self.paimaihang_user_info.shop_item_list
end

function ServerActivityData:GetPaiMaiHangRecordList()
	return self.paimaihang_record_info.record_list
end

function ServerActivityData:IsPaiMaiHangRecordCur(seq, record_timestamp)
	if self.paimaihang_notice_info.pai_mai_start_timestamp < record_timestamp and record_timestamp < self.paimaihang_notice_info.pai_mai_last_end_timestamp then
		local notice_info = self:GetPaiMaiHangNoticeItemInfo(seq)
		if notice_info ~= nil and notice_info.is_big_end_flag == 0 then
			return true
		end
	end
	return false
end

function ServerActivityData:ResetPaiMaiHangRemind()
	self.paimaihang_other_info:Set({remind_val = 0, remind_info = {}, show_record = false})
	TimeHelper:CancelTimer(self.paimaihang_remind_timer)
end

function ServerActivityData:CheckPaiMaiHangRemind()
	--拍卖开启红点
	if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_PAI_MAI_HANG) then
		TimeHelper:CancelTimer(self.paimaihang_remind_timer)
		self.paimaihang_other_info.remind_val = 0
		return
	end
	local act_status = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_PAI_MAI_HANG)
	if act_status ~= nil then
		local save_val = UnityPlayerPrefs.GetInt(PrefKeys.PaiMaiHangRemind())
		if save_val ~= act_status.param_2 then
			self.paimaihang_other_info.remind_val = 1
		end
	end
	local begin_time = self.paimaihang_notice_info.pai_mai_start_timestamp
	local function func_update() end
	local function func_finish()
		if ActivityData.IsOpen(ACTIVITY_TYPE.RAND_PAI_MAI_HANG) then
			self.paimaihang_other_info.remind_info = {timestamp = TimeHelper.GetServerTime()}
			self.paimaihang_other_info.remind_val = 1
			self.paimaihang_notice_info:Notify()
		end
	end
	TimeHelper:CancelTimer(self.paimaihang_remind_timer)
	if TimeHelper.GetServerTime() <= begin_time then
		self.paimaihang_remind_timer = TimeHelper:AddCountDownCT(func_update, func_finish, begin_time + 1, 3600, false)
	end
end

function ServerActivityData:SavePaiMaiHangTodayRemind()
	local act_status = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_PAI_MAI_HANG)
	if act_status ~= nil then
		UnityPlayerPrefs.SetInt(PrefKeys.PaiMaiHangRemind(), act_status.param_2)
	end
end

function ServerActivityData:OnAddPaiMaiHangNotice(param_t)
	--竞价被超过红点
	if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_PAI_MAI_HANG) or ViewMgr:IsOpen(PaiMaiHangView) then
		self.paimaihang_other_info.remind_val = 0
		return
	end
	local begin_time = self.paimaihang_notice_info.pai_mai_start_timestamp
	local end_time = self.paimaihang_notice_info.pai_mai_last_end_timestamp
	local info = {timestamp = tonumber(param_t[2]), seq = tonumber(param_t[1])}
	if begin_time < info.timestamp and info.timestamp < end_time then
		if self.paimaihang_other_info.remind_info.timestamp == nil then
			self.paimaihang_other_info.remind_info = info
			self.paimaihang_other_info.remind_val = 1
			self.paimaihang_other_info.show_record = true
		elseif self.paimaihang_other_info.remind_info.timestamp < info.timestamp then
			self.paimaihang_other_info.remind_info = info
			self.paimaihang_other_info.remind_val = 1
			self.paimaihang_other_info.show_record = true
		end
	end
end

-------------------------------------------------------
--赋世魂器
-------------------------------------------------------
function ServerActivityData:SetRAWorldSoulInfo(protocol)
	self.world_soul_data.recharge_num = protocol.recharge_num
	self.world_soul_data.reward_flag = protocol.reward_flag
	self.world_soul_data.val = not self.world_soul_data.val
end

function ServerActivityData:GetWorldSoulFlag(seq)
	return self.world_soul_data.reward_flag[seq] or 0
end

function ServerActivityData:GetWorldSoulRechargeNum()
	return self.world_soul_data.recharge_num or 0
end

function ServerActivityData:GetWorldSoulList()
	return self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_WORLD_SOUL,Config.assign_horcrux_auto.gift_configure,"seq")
end

function ServerActivityData:GetWorldSoulRedNum()
	local recharge_num = self:GetWorldSoulRechargeNum()
	for k,v in pairs(self:GetWorldSoulList()) do
		if recharge_num >= v.acc_price and self:GetWorldSoulFlag(v.seq) == 0 then
			return 1
		end
	end
	return 0
end

function ServerActivityData:GetWorldSoulEnd()
	for k,v in pairs(self:GetWorldSoulList()) do
		if self:GetWorldSoulFlag(v.seq) == 0 then
			return true
		end
	end
	return false
end
-------------------------------------------
----------回归豪礼
-------------------------------------------
function ServerActivityData:GetRegressDay()
	if self.regress_reward_data.activity_time then
	    local day_tb = TimeHelper.FormatDHMS(TimeHelper.GetServerTime() - self.regress_reward_data.activity_time)
	    return day_tb.day + 1 --得到回归天数
	end
	return 0
end

function ServerActivityData:GetRegressRewardEnd()
	local day = self:GetRegressDay()
	if self.regress_reward_data.activity_time == 0 or day > 7  then  ---0不可以参与 时间段可以参与
		return false

	end
	return true
end

function ServerActivityData:AddRegressRewardRedNum(num)
	self.regress_reward_red_pos:Notify()
end

function ServerActivityData:GetRegressRewardReminByDay(day)
	local cur_open_day = self:GetRegressDay()
	if day > cur_open_day then return 0 end 
	if self.regress_reward_data.task_data[day] then
	    for i,v in ipairs(self.regress_reward_data.task_data[day]) do
		   if v.state == 2 then return 1 end
	    end
	end
	if self.regress_reward_data.select_data[day] == 1 then
		return 1
	end
	return 0
end

function ServerActivityData:RegressRewardRedNum()
    local cur_open_day = self:GetRegressDay()
    local num = 0
    for i, v in ipairs(self.regress_reward_data.task_list) do
        local cur_open_day = self:GetRegressDay()
        if i > cur_open_day then
            return 0
        end
        self:GetRegressTaskDataByDay(i)
        num = self:GetRegressRewardReminByDay(i)
        if num == 1 then
            break
        end
    end
    return num
end

function ServerActivityData:SetBackRewardInfo(protocol)
	self.regress_reward_data.activity_time = protocol.activity_time
	self.regress_reward_data.task_list = protocol.task_list
	self.regress_reward_data.select_data = protocol.day_flag
	self.regress_reward_red_pos:Notify()
	self.regress_reward_data:Notify()
	self:RegressTaskDataCfg()
    self:RegressRewardDataCfg()
	if self.regress_reward_data.day == nil then  --第一次全部遍历
		for  i,v in ipairs(protocol.day_flag) do
			self:GetRegressTaskDataByDay(i)
		end
	end
end

function ServerActivityData:SetRegressRewardSelectType(day)
	self.regress_reward_data.day = day
	self.regress_reward_data.select_val = not self.regress_reward_data.select_val
	self:GetRegressTaskDataByDay(day)
end

--登录天数奖励
function ServerActivityData:RegressRewardDataCfg()
	local cur_server_open_day = TimeCtrl.Instance:GetCurOpenServerDay()
	if not self.regress_reward_data_cfg then
		self.regress_reward_data_cfg = {}
	    for i,v in ipairs(Config.back_reward_auto.gift_configure) do
		     if v.section_start <= cur_server_open_day and v.section_end >= cur_server_open_day then
			      table.insert(self.regress_reward_data_cfg,v)
	         elseif v.section_start <= cur_server_open_day and v.section_end == 0 then
			      table.insert(self.regress_reward_data_cfg,v)
		     end
	    end
    end
	table.sort(self.regress_reward_data_cfg, DataSorter.KeyLowerSorter("activity_days"))
end

function ServerActivityData:GetRegressTaskDataByDay(day)
	local cur_open_day = self:GetRegressDay()
	if not self.regress_reward_data.task_data[day] then
	      self.regress_reward_data.task_data[day] = TableCopy(self.regress_task_data_cfg[day] or {})
	end
	for i,v in ipairs(self.regress_reward_data.task_data[day]) do
		if day <= cur_open_day then
		   v.state = self.regress_reward_data.task_list[v.task_id].flag + 1
		else
			v.state = 1 ---超过天数显示未达成
		end
		v.times = self.regress_reward_data.task_list[v.task_id].param > v.param_1 and v.param_1 or self.regress_reward_data.task_list[v.task_id].param
	end
    --调整排序
	table.sort(self.regress_reward_data.task_data[day],function(a,b)
		if a.times == a.param_1 or b.times == b.param_1 then 
		   if a.state == 2 or b.state == 2 then
			    if a.state == 1 or b.state == 1 then
					return a.times > b.times
				end
				return a.state < b.state 
		   else
		      if a.times == 0 and b.times == 0 then
			      return a.type < b.type 
		      end
	       end
		   return a.times < b.times
		end
		return a.type < b.type 
    end)
	return self.regress_reward_data.task_data[day]
end

--依据天数任务配置
function ServerActivityData:RegressTaskDataCfg()
	if not self.regress_task_data_cfg then
        self.regress_task_data_cfg = {}
	    for i,v in ipairs(Config.back_reward_auto.task_configure) do
			if self.regress_task_data_cfg[v.activity_days] == nil then
				self.regress_task_data_cfg[v.activity_days] = {}
			end
			table.insert(self.regress_task_data_cfg[v.activity_days],v)
			self.regress_reward_data.max_day = v.activity_days
		end
	end
end

-- 连连乐翻天
function ServerActivityData:SetHappyLookInfo(protocol)
	self.happy_linkup_data.ra_frequency_num = protocol.ra_frequency_num
	self.happy_linkup_data.chanllenge_index = protocol.chanllenge_index
	self.happy_linkup_data.challenge_is_succ = protocol.challenge_is_succ
	self.happy_linkup_data.challenge_end_timestamp = protocol.challenge_end_timestamp

	HappyLinkupData.Instance:FlushGameStatus()
end
function ServerActivityData:GetHappyLookInfo()
	return self.happy_linkup_data
end
function ServerActivityData:HappyLinkUpRedNum()
	return ( HappyLinkupData.Instance:GetCurTimer() > 0 ) and 1 or 0
end

--登陆连好礼配置
function ServerActivityData:GetSignHappyConfig()
	return self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SIGN_HAPPY,Config.sign_happy_auto.reward)
end
-- 登陆连好礼配置
function ServerActivityData:GetSignHappyUpReward()
    if not self.sign_happy_data.up_list then
        self.sign_happy_data.up_list = {}
        local index = 0
        for i, v in ipairs(self:GetSignHappyConfig()) do
            if v.reward_item1 ~= nil then
                for j = 0, #v.reward_item1 + 1 do
                    if v.reward_item1[j] then
                        index = index + 1
                        v.reward_item1[j].index = index
                        table.insert(self.sign_happy_data.up_list, v.reward_item1[j])
                    end
                end
            end
        end
    end
    local is_show = self:GetSignHappyGainSate()
    for i, v in ipairs(self:GetSignHappyConfig()) do -- 3显示左半实虚线 2实线 1虚线 4橙虚线 5显示下一天领取情况 2领取l  4未领取 5未领取右半实虚线
        if v.reward_item1 ~= nil then
            for j = 0, #v.reward_item1 + 1 do
                if v.reward_item1[j] then
                    v.reward_item1[j].gain = false
                    if self.sign_happy_data.flag[33 - i] == 1 then -- 领取了
                        v.reward_item1[j].state = 2
                        v.reward_item1[j].gain = true
                        if self.sign_happy_data.flag[33 - i + 1] == 0 and v.seq1 < self.sign_happy_data.login_day then -- 前未领取领取
                            v.reward_item1[j].state = 5
                        end
                    else
                        v.reward_item1[j].state = v.seq1 + 1 <= self.sign_happy_data.login_day and 4 or 1
                        if self.sign_happy_data.flag[33 - i + 1] == 1 and v.seq1 < self.sign_happy_data.login_day then -- 前领取领取
                            v.reward_item1[j].state = 8 ---8-->>3
                        end
                    end
                    if v.seq1 == self.sign_happy_data.login_day then
                        v.reward_item1[j].state = self.sign_happy_data.flag[33 - i + 1] == 1 and 7 or 6
                    end
                end
            end
        end
        v.reward_flag = v.seq1 < self.sign_happy_data.login_day and self.sign_happy_data.flag[33 - i] or 2
        if v.seq1 == self.sign_happy_data.login_day then -- 0未领取  1领取 2未达到 3下一天展示
            v.reward_flag = 3
        end
    end
    self.sign_happy_data.gain = is_show
    return self.sign_happy_data.up_list
end

function ServerActivityData:GetSignHappyGainSate()
	local num = 0
	for i,v in ipairs(self.sign_happy_data.flag) do
        if 33 - i <= self.sign_happy_data.login_day and v == 1 then
              num = num + 1
		end
	end
	if self.sign_happy_data.login_day >= #self:GetSignHappyConfig() and num >= 5 then
		return 2
	end
	if num == self.sign_happy_data.login_day then
		return 0
	end
	return 1
end
function  ServerActivityData:GetSignHappyRedNum() --登陆连红点
	if self:GetSignHappyGainSate() == 2 then
		return 0
	end
	return self:GetSignHappyGainSate()
end

function ServerActivityData:SetSignHappyInfo(protocol)
	self.sign_happy_data.login_day = protocol.login_day
	self.sign_happy_data.flag = bit:d2b(protocol.flag)
	self.sign_happy_data.gain = 0
	self:GetSignHappyUpReward()
end

-------------------------------------------
----------万灵共盈
-------------------------------------------
function ServerActivityData:GetAllSoulsFullCfg()
	local show_list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_WAN_LING_GONG_YING, Config.wanlinggongying_auto.basis_configuration, "task_id")
	local act_list = Config.wanlinggongying_auto.activity_configuration
	for i, v in pairs(show_list) do
		v.act_type = act_list[v.task_id + 1] ~= nil and act_list[v.task_id + 1].activity_type or 0
	end
	return show_list
end
function ServerActivityData:GetAllSoulsFullOtherCfg()
	return Config.wanlinggongying_auto.other[1]
end

function ServerActivityData:AllSoulsFullFilter()
	local act_list = self:GetAllSoulsFullCfg()
	local open_list = {}
	local standy_list = {}
	local over_list = {}
	for i, v in pairs(act_list) do
		local open_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(v.act_type)
		local today_time = TimeCtrl.Instance:ChangDataTime(TimeCtrl.Instance:GetServerTime())
		if open_time ~= nil then
			if open_time <= today_time and today_time < end_time then
				if self:GetNowActState(v.act_type) == 1 then
					table.insert(open_list, v)
				end
			elseif open_time > today_time then
				v.open_time = open_time
				table.insert(standy_list, v)
			else
				table.insert(over_list, v)
			end
		end
	end
	table.sort(standy_list, DataHelper.SortFunc("open_time"))
	return open_list, standy_list, over_list
end

function ServerActivityData:SetAllSoulsFullInfo(protocol)
	self.all_souls_full_info.ra_buy_reward_timestamp = protocol.ra_buy_reward_timestamp
	self.all_souls_full_info.task_list = protocol.task_list
	self:CheckAllSoulsFullRed()
end

function ServerActivityData:GetAllSoulsFullTaskInfo(task_id)
	return self.all_souls_full_info.task_list[task_id]
end
function ServerActivityData:GetAllSoulsFullIsActive()
	return self.all_souls_full_info.ra_buy_reward_timestamp > 0
end

function ServerActivityData:CheckAllSoulsFullRed()
	--活动开启红点|进行中活动完成任务红点，已结束活动参与红点
	if self:GetAllSoulsFullTodayRemind() == 1 then
		self.all_souls_full_info.remind_val = 1
		return
	end
	if not self:GetAllSoulsFullIsActive() then
		self.all_souls_full_info.remind_val = 0
	else
		local open_list, standy_list, over_list = self:AllSoulsFullFilter()
		for i, v in pairs(open_list) do
			local task_info = self:GetAllSoulsFullTaskInfo(v.task_id)
			if task_info ~= nil and task_info.task_flag == 1 then
				self.all_souls_full_info.remind_val = 1
				return
			end
		end
		for i, v in pairs(over_list) do
			local task_info = self:GetAllSoulsFullTaskInfo(v.task_id)
			if task_info ~= nil and task_info.task_flag ~= 2 then --and task_info.param1 > 0 then
				self.all_souls_full_info.remind_val = 1
				return
			end
		end
		self.all_souls_full_info.remind_val = 0
	end
end

function ServerActivityData:GetAllSoulsFullTodayRemind()
	local act_status = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_WAN_LING_GONG_YING)
	if act_status ~= nil and UnityPlayerPrefs.GetInt(PrefKeys.ActOpenRemindSp(act_status.act_type)) ~= act_status.param_2 then
		return 1
	end
	return 0
end
function ServerActivityData:SaveAllSoulsFullTodayRemind()
	local act_status = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_WAN_LING_GONG_YING)
	if act_status ~= nil then
		UnityPlayerPrefs.SetInt(PrefKeys.ActOpenRemindSp(act_status.act_type), act_status.param_2)
	end
end

function ServerActivityData:CheckAllSoulsFullData()
	local act_type = ActivityData.Instance:GetActivityEventType()
	if act_type == nil or not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_WAN_LING_GONG_YING) 
	or act_type == ACTIVITY_TYPE.RAND_WAN_LING_GONG_YING or Config.wanlinggongying_auto == nil then
		return
	end
	--万灵共盈 相关活动开启关闭，需要检测下奖励是否可以领取
	for i, v in pairs(Config.wanlinggongying_auto.activity_configuration) do
		if act_type == v.activity_type then
			self:CheckAllSoulsFullRed()
			self.all_souls_full_info:Notify()
			self:CheckAllSoulsFullNewSign()
			break
		end
	end
end

--里面有新活动开启的时候外面万灵印象那里需要有新字提示
function ServerActivityData:CheckAllSoulsFullNewSign()
	for i, v in pairs(Config.wanlinggongying_auto.activity_configuration) do
		if ActivityData.Instance:IsHasSign(v.activity_type) then
			ActivityData.Instance:ResetHasSign(ACTIVITY_TYPE.RAND_WAN_LING_GONG_YING)
			return
		end
	end
end
function ServerActivityData:ClearAllSoulsFullNewSign()
	for i, v in pairs(Config.wanlinggongying_auto.activity_configuration) do
		ActivityData.Instance:SetHasSign(v.activity_type)
	end
end

-------------------------------------------
----------桃园一三事(义胆助人)
-------------------------------------------
function ServerActivityData:GetHelpOthersCurTaskPrefix()
    -- local show_cfg = self:GetOpenConfig(ACTIVITY_TYPE.RAND_HELP_OTHERS, Config.taoyuanyisanshi_auto.task_prefix)
    local show_cfg = self:GetOpenConfigStartTime(ACTIVITY_TYPE.RAND_HELP_OTHERS, Config.taoyuanyisanshi_auto.task_prefix)
	return show_cfg ~= nil and show_cfg[1] or nil
end

function ServerActivityData:GetHelpOthersNextTime()
	--按照分段去显示活动时间，开3天，空3天，再开3天
	local act_type = ACTIVITY_TYPE.RAND_HELP_OTHERS
	local activity_info = ActivityData.Instance:GetActivityStatusInfo(act_type)
	if activity_info == nil or activity_info.status == ActStatusType.Close then
		return nil
	end
	local show_cfg = self:GetHelpOthersCurTaskPrefix()
	if show_cfg == nil then
		return nil
	end
	local start_timestamp = activity_info.param_1
	local end_timestamp = TimeCtrl:ChangDataTime(start_timestamp, show_cfg.end_time)
	return end_timestamp
end

-------------------------------------------
----------山海缘起
-------------------------------------------
function ServerActivityData:GetShanHaiCurTaskPrefix()
    local show_cfg = self:GetOpenConfig(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_TASK, Config.shanhai_origin_auto.task_prefix)
	return show_cfg ~= nil and show_cfg[1] or nil
end

function ServerActivityData:GetShanHaiNextTime()
	--按照分段去显示活动时间，开3天，空3天，再开3天
	local act_type = ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_TASK
	local activity_info = ActivityData.Instance:GetActivityStatusInfo(act_type)
	if activity_info == nil or activity_info.status == ActStatusType.Close then
		return nil
	end
	local show_cfg = self:GetShanHaiCurTaskPrefix()
	if show_cfg == nil then
		return nil
	end
	local start_timestamp = activity_info.param_1
	local end_timestamp = TimeCtrl:ChangDataTime(start_timestamp, show_cfg.section_end)
	return end_timestamp
end

function ServerActivityData:ReStartMountainSeaRecallNeo()
	local role_id = RoleData.Instance:GetRoleId()
	UnityPlayerPrefs.SetString(role_id.."mountain_total","")
end

function ServerActivityData:SetMountainSeaRecallNeo(act_type)
	if self.mountain_sea_recall_neo == nil then self.mountain_sea_recall_neo = {} end 
	for k ,v in pairs(self.mountain_sea_recall_neo) do 
		if v == act_type then 
			return 
		end 
	end 
	table.insert(self.mountain_sea_recall_neo,act_type)

	local str = ""
	for k,v in pairs(self.mountain_sea_recall_neo) do 
		str = v.."|"..str
	end 

	local role_id = RoleData.Instance:GetRoleId()
	UnityPlayerPrefs.SetString(role_id.."mountain_total",str)
end

function ServerActivityData:GetMountainSeaRecallNeo(act_type)
	if self.mountain_sea_recall_neo == nil then 
		local role_id = RoleData.Instance:GetRoleId()
		local str_list = UnityPlayerPrefs.GetString(role_id.."mountain_total","")
		local list = Split(str_list,"|")
		self.mountain_sea_recall_neo = {}
		for k,v in pairs(list) do 
			table.insert(self.mountain_sea_recall_neo,tonumber(v))
		end 
	end 

	for k ,v in pairs(self.mountain_sea_recall_neo) do 
		if v == act_type then 
			return true
		end 
	end 
	return false
end 

-- 山海回想
function ServerActivityData:GetMountainSeaTotalView()
	local cfg = self:GetOpenConfig(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_MINEC,Config.recall_shanhai_summary_auto.recall_shanhai,"seq")

	local cur_list = PlanActivityData.Instance:GetRandMountainSeaList()

	local sort_list = {}
	for k,v in pairs(cfg) do 
		for pos,value in pairs(cur_list) do 
			local operate = v.plan_type == 1 and v.plan_type or v.plan_type + 1 
			if value.act_type == v.act_type and v.parameter == 1 and sort_list[operate] == nil then
				sort_list[operate] = {}
				sort_list[operate].cfg = v
				sort_list[operate].operate = operate
			end 
		end 
	end 

	for k,v in pairs(Config.recall_shanhai_summary_auto.recall_shanhai) do 
		if 1 == v.plan_type then 
			if sort_list[1] ~= nil and v.section_start == sort_list[1].cfg.section_end + 1 then 
				sort_list[2] = {}
				sort_list[2].cfg = v
				sort_list[2].operate = 2
			end 
		end 
	end 
	if sort_list[2] == nil then sort_list[2] = {operate = 2,is_empty = true} end 

	local commit_list = {}
	for k,v in pairs(sort_list) do
		local vo = {}
		vo.is_empty = v.is_empty
		if v.is_empty == nil then 
			for i,j in pairs(Config.activity_main_auto.recall_summary) do 
		    	if j.act_type == v.cfg.act_type then
			    	vo.name = j.text
					vo.mod_key = j.mod_key
			    end 
		    end 
			vo.describe = v.cfg.describe
			vo.act_type = v.cfg.act_type
			vo.plan_type = v.cfg.plan_type

			vo.rewards = {}
			if v.cfg.parameter == 1 then
				if v.cfg.reward_item_multi == 0 then 
					vo.rewards = v.cfg.plan_type == 6 and {{item_id = v.cfg.item_id}} or v.cfg.reward_item
				else
					local config_list = self:GetActOpenDayCfg(v.cfg.act_type,Config.recall_shanhai_summary_auto.recall_shanhai_show)
					for i,j in pairs(config_list) do-- (Config.recall_shanhai_summary_auto.recall_shanhai_show) do 
						if v.cfg.act_type == j.act_type then--and v.cfg.section_start <= j.section_start and v.cfg.section_end >= j.section_end then 
							vo.rewards = j.reward_item
						end 
					end 
				end 
			end 
			for i,j in pairs(Config.recall_shanhai_summary_auto.recall_shanhai_jump) do 
				if j.act_type == v.cfg.act_type then 
					vo.tick = j.item_id
					vo.tick_limit = j.limit_type
					local config = ActivityRandData.Instance:GetConfig(info.activity)
					vo.jump_mod = config.mod_key
				end 
			end 
			commit_list[v.operate] = vo
		end
	end 

	return commit_list
end

-- 提取工具
function ServerActivityData:GetOpenAndEndTime(act_type)
	local open_time = 0
	local end_time = 0
	local act_info = ActivityData.Instance:GetActivityStatusInfo(act_type)
	if act_info ~= nil and act_info.status == ActStatusType.Open then
		open_time = act_info.param_1
		end_time = act_info.param_2
	end
	if open_time == 0 then
		open_time,end_time = PlanActivityData.Instance:GetPlanDayOpenStampTime(act_type)
	end

	open_time = TimeManager.FormatUnixTime2Date(open_time)
	end_time = TimeManager.FormatUnixTime2Date(end_time)
	return open_time,end_time
end

function ServerActivityData:GetMountainSeaTip()
	for k, v in pairs(ActivityRandData.Instance.activity_rand_list or {}) do
		local config = Config.recall_shanhai_summary_auto.recall_shanhai
		for k1,v1 in pairs(config) do
			if v1.act_type == v.act_type then
				if v1.parameter == 1 then
					if ActivityData.Instance:IsHasSign(v.act_type) then
						return true
					end
				end
			end
		end
	end
	return false
end

-- 返回指定山海配置
function ServerActivityData:GetMountainSeaActCfg(type)
	local config = Config.recall_shanhai_summary_auto.recall_shanhai
	local days =  TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_MINEC)
	
	for k,v in pairs(config) do 
		if v.act_type == type and days >= v.section_start and days <= v.section_end then 
			return v
		end 
	end 
	return nil
end

-- 山海聚宝
function ServerActivityData:SetMountainSeaMineCollectInfo(protocol)
	-- local vo = {
	-- 	total_draw_times = protocol.total_draw_times,
	-- 	extra_reward_flag = protocol.extra_reward_flag,
	-- }
	self.mountain_sea_minec_data.info = {}
	for i = 1,3 do 
		local vo = {
		    total_draw_times = protocol.phase_info[i].total_draw_times,
		    extra_reward_flag = protocol.phase_info[i].extra_reward_flag,
		}
		table.insert(self.mountain_sea_minec_data.info,vo)
	end 
	-- self.mountain_sea_minec_data.info = protocol.phase_info
end

function ServerActivityData:SetMountainSeaMineCollectDrawRet(protocol)
	local vo ={
		show_type = protocol.show_type
	}
	self.mountain_sea_minec_data.draw_ret = vo
end

function ServerActivityData:GetMountainSeaMineCollectCfg()
	local list = self:GetSectionList(Config.shanhaijubao_auto.basis_configuration,"type")
	local act_info = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_MINEC)
	local act_time = TimeHelper.GetServerTime() - act_info.param_1
	local show_group_list = {}
	local group_list = {}
	for k,v in pairs(list) do 
		if group_list[v.phase] == nil then group_list[v.phase] = {} end 
		if show_group_list[v.phase] == nil then show_group_list[v.phase] = {} end 
		group_list[v.phase][v.seq+1] = v
		if v.is_show == 1 then 
			table.insert(show_group_list[v.phase],v)
		end 
	end 

	local group_btn_list = {}
	local next_phase = 0
	for i = 1,#group_list do 
		local vo = {}
		local timer = TimeHelper.FormatDHMS(act_time)
		local start_timer = TimeHelper.FormatDHMS(act_info.param_1)
		vo.inter = group_list[i][1].activity_time <= timer.day+1
		if vo.inter == false and next_phase == 0 then next_phase = i end 
		vo.phase_time = TimeCtrl.Instance:ChangDataTime(act_info.param_1,group_list[i][1].activity_time)
		vo.name = Language.MountainSeaMineCollect.PhaseName[i]
		vo.phase = i
		vo.show_next = next_phase

		table.insert(group_btn_list,vo)
	end 

	local phase_list = {}
	for k,v in pairs(Config.shanhaijubao_auto.phase_reward) do 
		if phase_list[v.phase] == nil then phase_list[v.phase] = {} end 
		table.insert(phase_list[v.phase],v)
	end 

	local extra_info = {}
	for i = 1,3 do 
		local extra_list = bit:d2b(self.mountain_sea_minec_data.info[i].extra_reward_flag)
		local cur_reward_index = 0 
		local is_all_in = false
		for i = 1, #phase_list[i]  do 
			if extra_list[33-i] == 0 then 
				cur_reward_index = i 
				break
			elseif i == #phase_list[i] and extra_list[33-i] == 1 then 
				cur_reward_index = i 
				is_all_in = true
			end 
		end 
		local vo = {}
		vo.phase_reward = phase_list[i][cur_reward_index]
		vo.cur_index = cur_reward_index
		vo.is_all_in = is_all_in
		table.insert(extra_info,vo)
	end 

	return {
		group_list = group_list,
		show_group_list = show_group_list,
		act_time = act_time,
		group_btn_list = group_btn_list,
		extra_info = extra_info,
	}
end

function ServerActivityData:GetMinecTotalShowItem()
	local master = self:GetMountainSeaMineCollectCfg()

	local cur_phase = 0
    for i = 1,3 do 
        if master.group_btn_list[i].inter == true then 
            cur_phase = i
        end 
    end

	if master.group_list[cur_phase + 1] == nil then 
		return false,nil
	end 
	for k,v in pairs(master.group_list[cur_phase + 1]) do 
		if v.is_show_1 == 1 then 
			return true, v
		end 
	end 

	return false,nil
end

function ServerActivityData:SetMinecAnimMark(flag)
	BagData.Instance:SetHideCurrencyCenterFlag(flag)
	-- self.mountain_sea_minec_anim_mark = flag
end

function ServerActivityData:GetMinecAnimMark()
	return self.mountain_sea_minec_anim_mark or false
end

function ServerActivityData:MountainSeaMineCollectRemind() 
	return Item.GetNum(Config.shanhaijubao_auto.other[1].consume_item) >= Config.shanhaijubao_auto.other[1].consume_num and 1 or 0
end 

-- 山海问答
function ServerActivityData:ResetMountainSeaQuestion()
	self.mountain_sea_question_data:Set({is_can_answer = false, times = 0, question_id = 0, reward_id = 0})
end

function ServerActivityData:SetMountainSeaQuestionData(protocol)
	local total_times = Config.time_limit_answer_auto.other[1].ans_times
	total_times = total_times - protocol.times
	self.mountain_sea_question_data.is_can_answer = protocol.is_can_answer == 1
	self.mountain_sea_question_data.times = total_times > 0 and total_times or 0
	self.mountain_sea_question_data.question_id = protocol.question_id
	self.mountain_sea_question_data.reward_id = protocol.reward_seq
	self.mountain_sea_question_data:Notify()
end

function ServerActivityData:SetMountainSeaQuestionStateData(protocol)
	self.mountain_sea_question_data.is_can_answer = protocol.is_can_answer == 1
end

function ServerActivityData:MountainSeaQuestionRemind()
	return (self.mountain_sea_question_data.is_can_answer and self.mountain_sea_question_data.times > 0) and 1 or 0
end

-- 山海风月
function ServerActivityData:ResetMountainSeaWindMoon()
	self.mountain_sea_wind_moon_reward_data:Set({times = 0, info = {}})
	self.mountain_sea_wind_moon_state_data:Set({is_can_lottery = false})
	self.mountain_sea_wind_moon_lottery_data:Set({info = {}})
end

function ServerActivityData:SetMountainSeaWindMoonRewardData(protocol)
	local total_times = Config.integral_point_choujiang_auto.other[1].day_times
	total_times = total_times - protocol.times
	self.mountain_sea_wind_moon_reward_data.times = total_times > 0 and total_times or 0
	self.mountain_sea_wind_moon_reward_data.info = protocol.reward_info
end

function ServerActivityData:SetMountainSeaWindMoonStateData(protocol)
	self.mountain_sea_wind_moon_state_data.is_can_lottery = protocol.is_can_lottery == 1
	self.mountain_sea_wind_moon_state_data:Notify()
end

function ServerActivityData:SetMountainSeaWindMoonLotteryData(protocol)
	self.mountain_sea_wind_moon_lottery_data.info = protocol.lottery_info
end

function ServerActivityData:MountainSeaWindMoonRewardRemind()
	-- 如果有可领取的礼包，就有红点
	for k, v in pairs(self.mountain_sea_wind_moon_reward_data.info) do
		if v > 0 then
			return 1
		end
	end
	return 0
end

function ServerActivityData:MountainSeaWindMoonBtnRemind()
	return (self.mountain_sea_wind_moon_state_data.is_can_lottery and self.mountain_sea_wind_moon_reward_data.times > 0) and 1 or 0
end

function ServerActivityData:MountainSeaWindMoonRemind()
	if self:MountainSeaWindMoonRewardRemind() == 1 or self:MountainSeaWindMoonBtnRemind() == 1 then
		return 1
	end
	return 0
end

-- 获取对应花色奖励可领取数量
function ServerActivityData:GetMountainSeaWindMoonCanGetNum(color_num)
	return self.mountain_sea_wind_moon_reward_data.info[color_num] or 0
end

-- 魂器天成
function ServerActivityData:SetRAHorcruxTianChengRoleInfo(protocol)
	self.horcrux_tiancheng_info.role:Set(protocol.info)
end

function ServerActivityData:SetRAHorcruxTianChengPoolInfo(protocol)
	self.horcrux_tiancheng_info.pool:Set(protocol.info)
end

function ServerActivityData:SetRAHorcruxTianChengDrawResult(protocol)
	self.horcrux_tiancheng_info.draw:Set(protocol.info)
end

function ServerActivityData:GetHorcruxTianchengRewardList()
	local list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_HORCRUX_TIANCHENG, Config.horcrux_tiancheng_auto.gift_configure)
	return list
end

function ServerActivityData:GetHorcruxTianchengShopList()
	local list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_HORCRUX_TIANCHENG, Config.horcrux_tiancheng_auto.shop_configure)
	return list
end

function ServerActivityData:GetHorcruxTianchengConsumeRewardList()
	local list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_HORCRUX_TIANCHENG, Config.horcrux_tiancheng_auto.consume_configuration)
	table.sort(list, function (a, b)
		if not a or not b then return a end
		local ag = self:GetHorcruxTianchengShopConsumeRewardIsGet(a.seq) and 1 or 0
		local bg = self:GetHorcruxTianchengShopConsumeRewardIsGet(b.seq) and 1 or 0
		if ag == bg then
			return a.seq < b.seq
		end
        return ag < bg
	end)
	return list
end

function ServerActivityData:GetHorcruxTianchengExchangeList()
	local list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_HORCRUX_TIANCHENG, Config.horcrux_tiancheng_auto.exchange_configure)
	return list
end

function ServerActivityData:GetHorcruxTianchengShopConsumeRewardIsGet(seq)
	local flag = self.horcrux_tiancheng_info.role.consume_fetch_mark or 0
	return 1 == bit:_rshift(flag, seq) % 2
end

function ServerActivityData:GetHorcruxTianchengShopLimit(seq)
	return self.horcrux_tiancheng_info.role.shop_list[seq] or 0
end

function ServerActivityData:GetHorcruxTianchengExchangeLimit(item_seq)
	return self.horcrux_tiancheng_info.role.exchange_list[item_seq] or 0
end

function ServerActivityData:GetHorcruxTianchengCostNum()
	return Config.horcrux_tiancheng_auto.other[1].item_num
end

function ServerActivityData:GetHorcruxTianchengIsOpen()
	return Config.horcrux_tiancheng_auto.other[1].is_open
end

function ServerActivityData:SetHorcruxTianchengModelIndex(model_index)
	self.horcrux_tiancheng_info.info.model_index = model_index
end

function ServerActivityData:GetHorcruxTianchengModelIndex()
	return self.horcrux_tiancheng_info.info.model_index
end

function ServerActivityData:GetHorcruxTianchengModelList()
	local function is_type_item(item_id, item_type)
		return Item.GetType(item_id) == item_type
	end
	local show_list = {}
	local list = self:GetHorcruxTianchengRewardList()
	for k, v in ipairs(list) do
		if 1 == v.is_show then
			local reward_item = v.reward_item
			local item_id = reward_item.item_id
			if is_type_item(item_id, ItemType.Gift) then
				local item_list,config = BagData.Instance:GiftItemList(item_id)
				for _, value in pairs(item_list or {}) do
					if is_type_item(value.item_id, ItemType.Gift) then
						local item_list2,config2 = BagData.Instance:GiftItemList(value.item_id)
						for _, value2 in pairs(item_list2 or {}) do
							if is_type_item(value2.item_id, ItemType.PetWeapon) then
								local item_data = Item.Init(value2.item_id)
								item_data.hqtc_index = #show_list + 1
								table.insert(show_list, item_data)
							end
						end
					elseif is_type_item(value.item_id, ItemType.PetWeapon) then
						local item_data = Item.Init(value.item_id)
						item_data.hqtc_index = #show_list + 1
						table.insert(show_list, item_data)
					end
				end
			elseif is_type_item(item_id, ItemType.PetWeapon) then
				local item_data = Item.Init(reward_item.item_id)
				item_data.hqtc_index = #show_list + 1
				table.insert(show_list, item_data)
			end
		end
	end
	return show_list
end

function ServerActivityData:HorcruxTianchengRemind()
	local list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_HORCRUX_TIANCHENG, Config.horcrux_tiancheng_auto.consume_configuration)
	local info = self.horcrux_tiancheng_info.role
	local consume_gold = info.consume_gold or 0
	for k, v in pairs(list) do
		local is_get = self:GetHorcruxTianchengShopConsumeRewardIsGet(v.seq)
		if not is_get and consume_gold >= v.consume_num then
			return 1
		end
	end
	return 0
end

-- 山海相逢 --
function ServerActivityData:MountainSeaMeetRemind()
	-- 如果有可领取的礼包，就有红点
	if self.mountain_sea_meet_data.is_buy == 0 then
		return 0
	end
	for k, v in pairs(self.mountain_sea_meet_data.task_start_info) do
		if v.state == 1 then
			return 1
		end
	end
	for k, v in pairs(self.mountain_sea_meet_data.task_end_info) do
		if v.state == 1 then
			return 1
		end
	end
	local has_num = Item.GetNum(Config.shanhaixiangfeng_auto.other[1].consume_item)
	if has_num > 0 and self:GetMountainSeaMeetIsAllOpen() == false and self.mountain_sea_meet_data.is_buy == 1 then
		return 1
	end
	return 0
end

function ServerActivityData:GetMountainSeaMeetIsAllOpen()
	for i,v in ipairs(self.mountain_sea_meet_data.reward_group) do
		if v.is_show == 0 then
			return false
		end
	end
	return true
end

function ServerActivityData:SetMountainSeaMeetTaskInfo(protocol)
	self:SetMountainSeaMeetRewardInfo()
	self.mountain_sea_meet_data.show_tip = nil
	local poker_list = bit:d2b(protocol.poker_flag)
	local reward_list = bit:d2b(protocol.fetch_flag)
	local task_list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_MEET, Config.shanhaixiangfeng_auto.basis_configuration,"task_id")
	self.mountain_sea_meet_data.is_buy = protocol.is_buy
	self.mountain_sea_meet_data.task_start_info = {}
	self.mountain_sea_meet_data.task_end_info = {}
    for i,v in ipairs(task_list) do
		v.times = protocol.task_list[v.task_type + 1].count > v.param_1 and v.param_1 or protocol.task_list[v.task_type + 1].count
		v.state = reward_list[33 - i] == 1 and 2 or  reward_list[33 - i]
		if v.times == v.param_1 then
			v.state = reward_list[33 - i] == 0 and 1 or 2
		end
		local state = self:GetMountainSeaMeetTaskIsStart(self:GetMountainSeaMeetActData(v.task_type))
		if state == 1 then
			v.is_open = true
			table.insert(self.mountain_sea_meet_data.task_start_info,v)
		elseif state == 0 then
			v.state = reward_list[33 - i] == 0 and 1 or 2
			v.is_open = false
			table.insert(self.mountain_sea_meet_data.task_end_info,v)
		end
	end
	self:SetMountainSeaMeetTaskSort(self.mountain_sea_meet_data.task_start_info)
	self:SetMountainSeaMeetTaskSort(self.mountain_sea_meet_data.task_end_info)
    for i,v in ipairs(protocol.poker_list) do
        v.is_show = poker_list[33 - i]
		v.item = self:GetMountainSeaMeetRewardItem(v.type)
    end
	self.mountain_sea_meet_data.reward_group = protocol.poker_list
	self.mountain_sea_meet_data:Notify()
end
function ServerActivityData:GetMountainSeaMeetTaskIsStart(data)
	local open_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(data.activity_type)
	local is_open = ActivityData.IsOpen(data.activity_type)
	if is_open then
		return 1
	else
		if (open_time or 0) >= TimeManager:GetServerTime() then
			if not self.mountain_sea_meet_data.show_tip then
				self.mountain_sea_meet_data.show_tip = data.activity_name
				self.mountain_sea_meet_data.open_time = open_time
			end
			return 2	
		end
	end
	return 0
end

function ServerActivityData:GetMountainSeaMeetRewardItem(type)
	for i,v in ipairs(self.mountain_sea_meet_data.reward_group_cfg) do
		if type == v.type then
			return v
		end
	end
	return 0
end

function ServerActivityData:SetMountainSeaMeetTaskSort(list)
	--调整排序
	table.sort(list,function(a,b)
		local num1 = a.state == 1 and 10 or 0
		local num2 = b.state == 1 and 10 or 0
		return a.state - num1 < b.state - num2
	end)
end

function ServerActivityData:SetMountainSeaMeetRewardInfo()
	self.mountain_sea_meet_data.reward_data = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_MEET, Config.shanhaixiangfeng_auto.draw_configuration)
	if not self.mountain_sea_meet_data.reward_group_cfg then
		self.mountain_sea_meet_data.reward_group_cfg = {}
		for i,v in ipairs(Config.shanhaixiangfeng_auto.reward_group) do
			if self.mountain_sea_meet_data.reward_data[1].reward_group == v.reward_group then
				table.insert(self.mountain_sea_meet_data.reward_group_cfg,v)
			end
		end
	end
end
function ServerActivityData:GetMountainSeaMeetActData(task_type)
	for i,v in ipairs(Config.shanhaixiangfeng_auto.activity_configuration) do
		if v.type == task_type then 
			return v
		end
	end
end
function ServerActivityData:GetMountainSeaMeetViewModKey(type)
	local mod_key = 0
	for i,v in ipairs(Config.shanhaixiangfeng_auto.activity_configuration) do
		if v.type == type then 
			mod_key = v.activity_mod_key
		end
	end
	return mod_key
end
--山海初遇
function ServerActivityData:SetMountainSeaFirstMeetInfo(protocol)
	local list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_FIRST_MEET,Config.shanhaichuyu_auto.basis_configuration)
	self.mountain_sea_first_meet_data.is_buy = protocol.extra_flag
	local reward_list = bit:d2b(protocol.fetch_flag) --奖励标记[活动天数index]
	local extra_list = bit:d2b(protocol.fetch_extra_flag) --额外奖励标记[活动天数index]
	local finish_list = bit:d2b(protocol.day_task_finish_flag) --任务完成标记(只在领取和跨天时校验)
    for i,v in ipairs(list) do
		v.is_extra = extra_list[33 - i]
		v.state = reward_list[33 - i]
		v.is_finish = finish_list[33 - i]
	end
	self.mountain_sea_first_meet_data.reward_info = list
	self.mountain_sea_first_meet_data:Notify()
end

function ServerActivityData:SetMountainSeaFirstMeetTaskSort(list)
	--调整排序
	table.sort(list,function(a,b)
		local num1 = (a.state == 1 or a.times == a.param_1) and 10 or 0
		local num2 = (b.state == 1 or b.times == b.param_1) and 10 or 0
		return a.state - num1 < b.state - num2
	end)
end

function ServerActivityData:MountainSeaFirstMeetRemind()
	-- 有可领取的礼包，就有红点
	local cur_open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_FIRST_MEET)
	if self.mountain_sea_first_meet_data.is_buy == 0 then
		return 0
	end
	for k, v in pairs(self.mountain_sea_first_meet_data.reward_info) do
		if v.state == 0 and v.is_finish == 1 then
			return 1
		end
		--if v.state == 1 and v.is_finish == 1 and v.is_extra == 0 and v.activity_days == cur_open_day then
		--	return 1
		--end
		if v.state == 0 and v.is_finish == 0 and v.activity_days < cur_open_day and self.mountain_sea_first_meet_data.is_buy == 1 then
			return 1
		end
	end
	return 0
end

function ServerActivityData:GetMountainSeaFirstMeetTaskGroup(day)
	local cur_open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_FIRST_MEET)
	local list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_FIRST_MEET,Config.shanhaichuyu_auto.basis_configuration)
	for i,v in ipairs(list) do
		if cur_open_day + day == v.activity_days then
			return v.task_group
		end
	end
	return -1
end

function ServerActivityData:SetMountainSeaFirstMeetTask(protocol)
	local cur_open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_FIRST_MEET)
	self.mountain_sea_first_meet_data.task_list = {}
    if not self.mountain_sea_first_meet_data.task_cfg then
		self.mountain_sea_first_meet_data.task_cfg = {}
		for i,v in ipairs(Config.shanhaichuyu_auto.task_list) do
			table.insert(self.mountain_sea_first_meet_data.task_cfg,v)
		end
	end
	for i,v in ipairs(self.mountain_sea_first_meet_data.task_cfg) do
		v.times = protocol.task_list[v.task_type + 1].times
		v.state = self:GetMountainSeaFirstMeetTaskIsStart(self:GetMountainSeaFirstMeetActData(v.task_type).activity_type)
		v.state = (v.state > 0 and self:GetMountainSeaFirstMeetActDay(v.task_group) > cur_open_day) and 2 or v.state 
		v.times = v.state == 1 and v.times or 0
	end
	for i,v in ipairs(self.mountain_sea_first_meet_data.task_cfg) do
		if v.task_group == self:GetMountainSeaFirstMeetTaskGroup(0) or v.task_group == self:GetMountainSeaFirstMeetTaskGroup(1) then
			table.insert(self.mountain_sea_first_meet_data.task_list,v)
		end
	end
	self:SetMountainSeaFirstMeetTaskSort(self.mountain_sea_first_meet_data.task_list)
	self.mountain_sea_first_meet_data:Notify()
end
function ServerActivityData:SetMountainSeaFirstMeetFulsh()
	local cur_open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_FIRST_MEET)
	for i,v in ipairs(self.mountain_sea_first_meet_data.task_list) do
		v.state = self:GetMountainSeaFirstMeetTaskIsStart(self:GetMountainSeaFirstMeetActData(v.task_type).activity_type)
		v.state = (v.state > 0 and self:GetMountainSeaFirstMeetActDay(v.task_group) > cur_open_day) and 2 or v.state 
	end
	self:SetMountainSeaFirstMeetTaskSort(self.mountain_sea_first_meet_data.task_list)
end

function ServerActivityData:GetMountainSeaFirstMeetActDay(task_group)
	for i,v in ipairs(Config.shanhaichuyu_auto.basis_configuration) do
		if v.task_group == task_group then 
			return v.activity_days
		end
	end
end

function ServerActivityData:GetMountainSeaFirstMeetActData(type)
	for i,v in ipairs(Config.shanhaichuyu_auto.activity_configuration) do
		if v.type == type then 
			return v
		end
	end
end

function ServerActivityData:GetMountainSeaFirstMeetTaskIsStart(act_type)
	local open_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(act_type)
	local is_open = ActivityData.IsOpen(act_type)
	if is_open then
		return 1
	else
		if (open_time or 0) >= TimeManager:GetServerTime() then
			return 2	
		end
	end
	return 0
end

function ServerActivityData:SetMountainSeaFirstMeetInfoChange(protocol)
	local cur_open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_FIRST_MEET)
	for i,v in ipairs(self.mountain_sea_first_meet_data.task_list) do
		if v.task_type == protocol.task_type then
            v.times = protocol.times
			v.state = (v.state > 0 and self:GetMountainSeaFirstMeetActDay(v.task_group) > cur_open_day) and 2 or v.state 
		    v.times = v.state == 1 and v.times or 0
		end
	end
	self:SetMountainSeaFirstMeetTaskSort(self.mountain_sea_first_meet_data.task_list)
    self.mountain_sea_first_meet_data:Notify()
end

function ServerActivityData:GetMountainSeaFirstMeetTime(day)
	local t = os.date([[*t]], ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_FIRST_MEET))
	t.day = t.day + day - 1
	return TimeCtrl.Instance:ChangDataStampTime(t)
end

-------------------------------------------------------
--山海争锋
-------------------------------------------------------
function ServerActivityData:SetSummerExperienceTwoInfo(protocol)
	self.summer_experience_two_data.open_day = protocol.open_day
	self.summer_experience_two_data.is_fetch_box = protocol.is_fetch_all_pass_flag
	self.summer_experience_two_data.first_flag = protocol.first_flag

	self.summer_experience_two_data.first_flag_num = 0
	for i, v in ipairs(protocol.first_flag) do
		if v == 1 then
			self.summer_experience_two_data.first_flag_num = self.summer_experience_two_data.first_flag_num + 1
		else
			break
		end
	end
end

function ServerActivityData:GetSummerExperienceTwoConfig()
	if not self.summer_experience_two_config then
		self.summer_experience_two_config = {}
	end
	local day = self.summer_experience_two_data.open_day
	if not self.summer_experience_two_config[day] then
		self.summer_experience_two_config[day] = self:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SUMMER_EXPERIENCE_TWO, Config.shanhaizhengfeng_auto.basis_configuration, "type")
		local open_time = TimeCtrl.Instance:ChangDataTime(ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_SUMMER_EXPERIENCE_TWO).param_1)
		for k, v in pairs(self.summer_experience_two_config[day]) do
			v.open_time = nil
			if v.activity_time > self.summer_experience_two_data.open_day then
				v.open_time = TimeCtrl.Instance:ChangDataTimeDetail(open_time + 86400 * (v.activity_time - 1))
			end
		end
	end
	return self.summer_experience_two_config[day]
end

function ServerActivityData:GetSummerExperienceTwoRemind()
	local is_all_win = self.summer_experience_two_data.first_flag_num == #self:GetSummerExperienceTwoConfig()
	return (is_all_win and self.summer_experience_two_data.is_fetch_box == 0) and 1 or 0
end

-------------------------------------------
----------再战云泽
-------------------------------------------
function ServerActivityData:SetRACloudSwampResult(protocol)
	self.cloud_swamp_info = protocol.boss_info
end

function ServerActivityData:GetCloudSwampTimes(seq)
	if self.cloud_swamp_info then
		return self.cloud_swamp_info[seq] or 0
	end
	return 0
end

function ServerActivityData:SetCloudSwampFinishData(data)
	self.cloud_swamp_finish_data = data
end

function ServerActivityData:SetCloudSwampSelectIndex(index)
	if self.IsCloudSwampIndex == nil then
		self.IsCloudSwampIndex = true
	else
		self.cloud_swamp_select_index = index
	end
end

function ServerActivityData:GetCloudSwampFinishData()
	return DataHelper.FormatItemList(self.cloud_swamp_finish_data or {})
end

function ServerActivityData:GetCloudSwampRewardData(reward_id)
	local config = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_CLOUD_SWAMP,Config.zaizhanyunze_auto.reward)
    local reward_config = DataHelper.GetSearch(config or {},[[item_id]],reward_id)
    return reward_config and DataHelper.FormatItemList(reward_config.reward_item) or {}
end

function ServerActivityData:GetCloudSwampRewardRateData(reward_id)
	local list = {}
	local config = Config.zaizhanyunze_auto.reward_probability
	for k,v in pairs(ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_CLOUD_SWAMP,config)) do
		if v.item_id == reward_id then
			for _,item in pairs(v.reward_item) do
				table.insert(list,item)
			end
		end
	end
	return DataHelper.FormatItemList(list) or {}
end

function ServerActivityData:GetCloudSwampRewardList(config,times)
	local data_list = {}
    for i=1,3 do
        if config["number"..i] ~= 0 then
            table.insert(data_list,{reward_id = config["number_reward"..i],boss_times = config["number"..i],times = times})
        end
    end
    for k,v in pairs(data_list) do
        v.is_reward = times >= data_list[#data_list].boss_times
    end
    local list = DataHelper.ReverseList(data_list)
    for i,v in ipairs(list) do
        if times >= v.boss_times then
            return list[i-1] or v
        end
    end
    if times >= list[#list].boss_times then
        return list[#list]
    end
    return list[#list]
end

function ServerActivityData:GetCloudSwampIndex()
	if self.IsCloudSwampIndex == nil then
		local config = Config.zaizhanyunze_auto.basics
		local section_num = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_CLOUD_SWAMP)
		for i,v in ipairs(DataHelper.ReverseList(config)) do
			if section_num >= v.section_start and self:GetCloudSwampTimes(v.seq) == 0 then
				return v.seq + 1
			end
		end
	end
	return self.cloud_swamp_select_index
end

-------------------------------------------
----------周末限好礼
-------------------------------------------
function ServerActivityData:SetWeedendGiftInfo(protocol)
	self.week_gift_data.info_list = protocol.info_list
	self.week_gift_data.val = not self.week_gift_data.val

	local is_need_open = false
	-- 新增每日首次收到新的周末限好礼需要自动打开
	for k, v in pairs(self:GetWeedendGiftInfo()) do
		if v.is_popup == 1 then
			local str = DTNewTodayNotRemind.WeekGift .. v.gift_id
			if not DialogTipsData.Instance:TodayIsTrue(str) then
				is_need_open = true
				DialogTipsData.Instance:SetTodayRemind(str)
			end
		end
	end
	if is_need_open then
		if not ViewMgr:IsOpen(ExclusiveGiftView) and not ViewMgr:IsOpen(WeekGiftView) then
			ViewMgr:OpenView(WeekGiftView, {show_last = true})
		end
	end
end

function ServerActivityData:GetWeedendGiftInfo()
	local list = {}
	for i,v in ipairs(self.week_gift_data.info_list) do
		local times = (self:GetWeedendGiftData(v.index) or {}).limit_num or 0
		if v.end_time - TimeHelper.GetServerTime() > 0 and v.times < times then
			table.insert(list,v)
		end
	end
	return list
end

function ServerActivityData:GetWeedendGiftData(type)
	return Config.exclusive_gift_auto.reward_group[type]
end

function ServerActivityData:IsHasWeedendGift()
	return ActivityData.IsOpen(ACTIVITY_TYPE.RANK_WEEK_GIFT) and #self:GetWeedendGiftInfo() > 0
end

-------------------------------------------
----------超值献礼
-------------------------------------------
function ServerActivityData:SetRAChaoZhiGiftInfo(protocol)
	self.chaozhi_gift_info:Set(protocol.info)
end

function ServerActivityData:GetChaoZhiGiftShowList()
	local list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RANK_CHAOZHI_GIFT, Config.chaozhixianli_auto.gift_configure)
	return TableCopy(list)
end

function ServerActivityData:GetChaoZhiGiftPrice()
	local price = 0
	local list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RANK_CHAOZHI_GIFT, Config.chaozhixianli_auto.buy_configure)
	if list and list[1] then
		price = list[1].price
	end
	return price
end

function ServerActivityData:GetChaoZhiGiftOtherBuyTimes()
	return Config.chaozhixianli_auto.other[1].buy_times
end

function ServerActivityData:ChaoZhiGiftRemind()
	-- 有可领取的礼包，就有红点
	local open_day = TimeCtrl.Instance:ActOpenDay(ACTIVITY_TYPE.RANK_CHAOZHI_GIFT)
	local info = self.chaozhi_gift_info
	if 0 == info.gift_act_flag then
		return 0
	end
	local list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RANK_CHAOZHI_GIFT, Config.chaozhixianli_auto.gift_configure)
	for k, v in pairs(list) do
		local is_get = (info.gift_num_list[v.active_times] or 1) > 0
		if (not is_get) and (open_day >= v.active_times) then
			return 1
		end
	end
	return 0
end

function ServerActivityData:GetChaoZhiGiftEnd()
	local activity_status = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RANK_CHAOZHI_GIFT)
    local status = activity_status and activity_status.status or ActStatusType.Close
    if ActStatusType.Close == status then
		return false
    end
	local list = self:GetActOpenDayCfg(ACTIVITY_TYPE.RANK_CHAOZHI_GIFT, Config.chaozhixianli_auto.gift_configure)
	local info = self.chaozhi_gift_info
	if 0 == info.gift_act_flag then
		local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RANK_CHAOZHI_GIFT)
		return open_day <= self:GetChaoZhiGiftOtherBuyTimes()
	else
		local all_get = true
		for k, v in pairs(list) do
			local is_get = (info.gift_num_list[v.active_times] or 1) > 0
			if not is_get then
				all_get = false
			end
		end
		return not all_get
	end
end

-- 兔年新春活动是否开启
function ServerActivityData:IsOpenRabbitYear()
	if not ActivityRandData.Instance:GetBtnListIsOpen(Mod.RabbitYear.Main) then
        return false
    end
	local level = RoleData.Instance:GetRoleLevel()
	local data = FunOpen.Instance:GetOpenData(Mod.RabbitYear.Main)
	if data and level < data.level then
		return false
	end
	local act_list = Config.activity_main_auto.tiger_spring_festival
	for i,v in pairs(act_list) do
		if ActivityData.IsOpen(v.act_type) then
			return true
		end
	end
	if ViewMgr:IsOpen(RabbitYearView) then
		ViewMgr:CloseView(RabbitYearView)
	end
	return false
end

-- 兔年新春活动结束时间
function ServerActivityData:GetRabbitYearEndTime()
	local end_time = 0
	local act_list = Config.activity_main_auto.tiger_spring_festival
	for i,v in pairs(act_list) do
		local _, v_end_time = ActivityRandData.GetBaseDayOpenStampTime(v.act_type)
		if not v_end_time then
			LogError("活动时间配置没有配，运营配下。活动类型=== ", v.act_type)
		end
		if v_end_time and v_end_time > end_time then
			end_time = v_end_time
		end
	end
	return end_time
end