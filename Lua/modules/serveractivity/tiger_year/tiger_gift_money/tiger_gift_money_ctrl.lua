VIEW_REQ("modules/serveractivity/tiger_year/tiger_gift_money/tiger_gift_money_view")

TigerGiftMoneyCtrl = TigerGiftMoneyCtrl or BaseClass(BaseCtrl)

function TigerGiftMoneyCtrl:__init()
	if TigerGiftMoneyCtrl.Instance ~= nil then
		Debuger.LogError("[TigerGiftMoneyCtrl] attempt to create singleton twice!")
		return
	end	
	TigerGiftMoneyCtrl.Instance = self
	self.data = TigerGiftMoneyData.New()
	self:RegisterAllProtocols()

	Remind.Instance:Register(Mod.TigerYear.TigerGiftMoney, self.data.gift_money_remind, function ()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_TIGER_GIFT_MONEY) then
			return 0
		end
		return self.data:RemindNum() 
    end)	
end

function TigerGiftMoneyCtrl:__delete()
	self:UnRegisterAllProtocols()
	TigerGiftMoneyCtrl.Instance = nil
	Delete(self, "data")
end

function TigerGiftMoneyCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRATigerGiftMoneyInfo, "OnTigerGiftMoneyInfo")
end

function TigerGiftMoneyCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCRATigerGiftMoneyInfo)
end

function TigerGiftMoneyCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function TigerGiftMoneyCtrl:OnTigerGiftMoneyInfo(protocol)
	self.data:SetInfo(protocol)
end

-- 发送请求
-- opera_type：0 请求下发
-- opera_type：1 领取任务，p1：任务seq
-- opera_type：2 领取奖励
function TigerGiftMoneyCtrl:Send(opera_type, param_1)
	ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_TIGER_GIFT_MONEY, opera_type, param_1)
end

--------------------TigerGiftMoneyData--------------------
TigerGiftMoneyData = TigerGiftMoneyData or BaseClass()

function TigerGiftMoneyData:__init()
	if TigerGiftMoneyData.Instance ~= nil then
		Debuger.LogError("[TigerGiftMoneyData] attempt to create singleton twice!")
		return
	end
	TigerGiftMoneyData.Instance = self
    self.gift_money_data = SmartData.New({money_num = 0,is_click_receive = false})
	self.gift_money_remind = SmartData.New({is_flush = false})
	self.level_task_data = {}
	self.is_close_anim = false
    self:ResetData()
end

function TigerGiftMoneyData:ResetData()
    self.money_num = nil
end

function TigerGiftMoneyData:GetTaskDataCfg()
	return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_TIGER_GIFT_MONEY,Config.yinhuyasui_auto.basic_configure)
end

function TigerGiftMoneyData:GetRewardDataCfg()
	return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_TIGER_GIFT_MONEY,Config.yinhuyasui_auto.gift_configure)
end


function TigerGiftMoneyData:GetRewardInfo()
	local info = self:GetRewardDataCfg()[1]
	local item = {
		is_bind = 0,
		num = 0,
		item_id = info.item_id_1
	}
	info.item = item
	return info
end

function TigerGiftMoneyData:GetOpenTime()
	local t = os.date([[*t]], ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_TIGER_GIFT_MONEY))
	t.day = t.day - 1
	local cur_day = os.date([[*t]], (ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_TIGER_GIFT_MONEY) - TimeHelper:GetServerTime()))
	if cur_day.day - 1 <= 0 then
		return nil
	end
	return TimeCtrl.Instance:ChangDataStampTime(t)
end

function TigerGiftMoneyData:RemindNum()
	if self.gift_money_data.money_num > 0 then
		if self.gift_money_data.is_receive then
			return 1
		end
	end
	if self.gift_money_data.task_list then --避免协议未下发 直接进入判断
		for i,v in pairs(self.gift_money_data.task_list) do
			if v.task_status == 1 and v.reward_status == 0 then
				return 1
			end
		end
	end	
	return 0
end

function TigerGiftMoneyData:OnClickReceive()
	--self.gift_money_data:SetDirty("is_click_receive")
	self.gift_money_data:Notify()
	self.gift_money_remind:SetDirty("is_flush")
end

function TigerGiftMoneyData:GetTaskListByLevel(role_level)
	if not self.level_task_data[role_level] then
		self.level_task_data[role_level] = {}
		local level = 0
		for i,v in pairs(self:GetTaskDataCfg()) do
			if role_level >= v.level then
				level = v.level
			end
		end
		for i,v in pairs(self:GetTaskDataCfg()) do
			if level == v.level then
				table.insert(self.level_task_data[role_level],v)
			end
		end
	end	
	return self.level_task_data[role_level]
end

function TigerGiftMoneyData:SetInfo(protocol)
	if self.money_num then
		if protocol.money_num > 0 and self.money_num == 0 then
			self.is_close_anim = true
		end
	end
	self.money_num = protocol.money_num
	self.gift_money_data.money_num = protocol.money_num
	self.gift_money_data.is_receive = protocol.is_can_receive > 0 and protocol.money_num > 0
	self.gift_money_data.role_level = protocol.role_level
	self.gift_money_data.task_list = self:GetTaskListByLevel(protocol.role_level)
	for i,v in pairs(self.gift_money_data.task_list) do
		v.task_status = protocol.task_status[v.task_type]  --完成情况
		v.reward_status = protocol.task_reward_receive[v.task_type] --是否被领取
		v.times = protocol.task_list[v.task_type]
	end
	--调整排序
	table.sort(self.gift_money_data.task_list,function(a,b)
		return self:CalculateVal(a) > self:CalculateVal(b)
    end)
	self.gift_money_data:SetDirty("task_list")
	self.gift_money_remind:SetDirty("is_flush")
end

function TigerGiftMoneyData:CalculateVal(info)
	local val = (info.task_status == 1 and 100 or 0) + (info.reward_status == 1 and -200 or 0) - info.seq
	return val
end
