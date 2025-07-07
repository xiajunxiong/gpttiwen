PlanActivityView = PlanActivityView or DeclareView("PlanActivityView", "serveractivity/plan_activity/plan_activity_view", Mod.PlanActivity.Main)

VIEW_DECLARE_LEVEL(PlanActivityView, ViewLevel.L0)

function PlanActivityView:PlanActivityView()
    self.handle = ActivityData.Instance:GetActivityEvent():Care(BindTool.Bind(self.OnFlushShowView, self))
	self.bag_handle = BagData.Instance:Care(BindTool.Bind(self.OnFlushShowView, self))
	self.btn_go = {}
	self.part2_item = {}
	self.part2_item_index =  1
	self:SetTimerCloseView()
    self:OnFlushShowView()
end

function PlanActivityView:SetTimerCloseView()
	local begin_time,end_time = PlanActivityData.Instance:GetActBeginAndEndAndDayTime()
	self.timer_close_view = TimeHelper:AddRunTimer(function ()
			if TimeHelper.GetServerTime() > end_time then
				PublicPopupCtrl.Instance:Error(Language.PlanActivity.TipsText)
				ViewMgr:CloseView(PlanActivityView)
			end
	end, 1)	
end

function PlanActivityView:CloseCallback()
	TimeHelper:CancelTimer(self.add_timer)
	TimeHelper:CancelTimer(self.timer_close_view)
	TimeHelper:CancelTimer(self.timer_rt)
	TimeHelper:CancelTimer(self.timer_fresh)
    ActivityData.Instance:GetActivityEvent():Uncare(self.handle)
	BagData.Instance:UnCare(self.bag_handle)
end

function PlanActivityView:OnFlushShowView()
	for k,v in pairs(Config.activity_main_auto.btn_list) do
		if v.mod_key == Mod.PlanActivity.Main then
			UH.SetText(self.titletext,v.text)
			break
		end
	end
    local list = PlanActivityData.Instance:GetRandPlanList()
	self:FlushPartOne(list)
	self:FlushPartTwo(list)
	self:FlushPartThree(list)
	self:FlushPartFour(list)
	self:FlushPartFive(list)
	self:FlushPartSix(list)
end

function PlanActivityView:GetOpenAndEndTime(act_type)
	local open_time = 0
	local end_time = 0
	local act_info = ActivityData.Instance:GetActivityStatusInfo(act_type)
	if act_info ~= nil and act_info.status == ActStatusType.Open then
		open_time = act_info.param_1
		end_time = act_info.param_2 - 1
		if ACTIVITY_TYPE.RAND_HELP_OTHERS == act_type then --桃源一三事开启时间特殊处理
			local config = Config.wanlingyinxiang_auto.rand_pool
			for k,v in pairs(config) do
				if v.act_type == act_type then
					local open_day = PlanActivityData.Instance:GetPlanActivityOpenDay()
					if open_day >= v.section_start and open_day <= v.section_end then
						local p_open,p_end = PlanActivityData.Instance:GetActBeginAndEndAndDayTime()
						open_time = p_open + (v.section_start - 1)*24*60*60
						end_time = open_time + (v.section_end - v.section_start)*24*60*60
					end 
				end
			end
		end
	end	
	if open_time == 0 then
		open_time,end_time = PlanActivityData.Instance:GetPlanDayOpenStampTime(act_type)
	end
	open_time = TimeManager.FormatUnixTime2Date(open_time)
	end_time = TimeManager.FormatUnixTime2Date(end_time)
	return open_time,end_time
end

function PlanActivityView:GetActInfoByIndex(list,index)
	--local begin_time,end_time = PlanActivityData.Instance:GetActBeginAndEndAndDayTime()
	--local day_time = math.ceil((TimeHelper.GetServerTime() - begin_time) / TIME_DAY_SECOND_NUM)
	local activity_info = nil
	local plan_info = nil
	local config = Config.wanlingyinxiang_auto.rand_pool
	for k,v in pairs(config) do
		if v.plan_type == index and v.parameter == 1 then
			for k1,v1 in pairs(list) do
				if v.act_type == v1.act_type  then
					if ACTIVITY_TYPE.RAND_HELP_OTHERS == v.act_type then --桃源一三事开启时间特殊处理
						local open_day = PlanActivityData.Instance:GetPlanActivityOpenDay()
						if open_day >= v.section_start and open_day <= v.section_end then
							activity_info = v1
							plan_info = v
							break
						end
					else
						activity_info = v1
						plan_info = v
						break
					end						
				end
			end
			if activity_info then
				break
			end
		end
	end
	return activity_info,plan_info
end

function PlanActivityView:FlushTimeAndName(list,index)
	local activity_info,plan_info = self:GetActInfoByIndex(list,index)
	if activity_info then
		UH.SetText(self.namelist[index],activity_info.text)
		table.insert(self.btn_go,index,activity_info)
		local open_time,end_time = self:GetOpenAndEndTime(activity_info.act_type)
		if open_time and end_time then
			local str = string.format(Language.PlanActivity.ActivityTime,tostring(open_time.month),tostring(open_time.day),tostring(end_time.month),tostring(end_time.day))
			UH.SetText(self.timelist[index],str)
		end	
--[[		if self.newlist[index] then
			if ActivityData.Instance:IsHasSign(activity_info.act_type) then
				self.newlist[index]:SetActive(true)
			else
				self.newlist[index]:SetActive(false)
			end
		end--]]	
		if self.newlist[index] then
			self.newlist[index]:SetActive(false)
		end
		if ActivityData.Instance:IsHasSign(activity_info.act_type) then
			self.redlist[index]:SetNum(1)
		else
			if Remind.Instance:GetRemindNum(activity_info.mod_key) > 0 then
				self.redlist[index]:SetNum(1)
			else
				self.redlist[index]:SetNum(0)
			end
		end
	end
end

function PlanActivityView:GetPlanRewardItems(plan_info)
	local reward_item 
	if plan_info.reward_item_multi == 1 then
		local config = Config.wanlingyinxiang_auto.rand_reward_item
		local open_day = TimeCtrl.Instance:GetActOpenServerDay(plan_info.act_type)
		if open_day < 1 then
			open_day = 1
		end
		for k,v in pairs(config) do
			if v.act_type == plan_info.act_type then
				if v.section_end ~= 0 then
					if (v.section_start <= open_day or open_day <= 0) and v.section_end >= open_day then
						reward_item = v.reward_item
						break
					end
				else
					if open_day >= v.section_start then
						reward_item = v.reward_item
						break
					end
				end
			end
		end
		if reward_item == nil then
			reward_item = {}
		end
	else
		reward_item = plan_info.reward_item
	end
	return reward_item
end

function PlanActivityView:FlushRewardItems(plan_info,index)
	local reward_item = self:GetPlanRewardItems(plan_info)
	self.rewardlist[index]:SetData(reward_item) 
end

function PlanActivityView:FlushPartOne(list)
	local activity_info,plan_info = self:GetActInfoByIndex(list,1)
	local has_next = false
	if plan_info then
		UH.SetText(self.decact1,plan_info.describe)
		self.NextObj:SetActive(false)
		self:FlushRewardItems(plan_info,1)
		local next_info = nil
		local config = Config.wanlingyinxiang_auto.rand_pool
		for k,v in pairs(config) do
			if v.seq  == (plan_info.seq + 1) and v.plan_type == 1 and v.parameter == 1 then
				next_info = v
				has_next = true
				break
			end
		end		
		if next_info then
			local name = ""
			for k,v in pairs(PlanActivityData.Instance.plan_config) do
				if next_info.act_type == v.act_type then
					name = v.text
					break
				end
			end
			UH.SetText(self.nameact1next,name)
			local open_time,end_time = self:GetOpenAndEndTime(next_info.act_type)
			if open_time and end_time then
				local str = string.format(Language.PlanActivity.ActivityTime,tostring(open_time.month),tostring(open_time.day),tostring(end_time.month),tostring(end_time.day))
				UH.SetText(self.timeact1next,str)
			end
			self.NextObj:SetActive(true)
		end
	end
	if activity_info then
		self:FlushTimeAndName(list,1)
	end		
end

function PlanActivityView:FlushPartTwo(list)
	local activity_info,plan_info = self:GetActInfoByIndex(list,2)
	if plan_info then
		-- BuglyReport(plan_info)
		if self.IsComplete == nil then
			self.IsComplete = true
			local reward_item = self:GetPlanRewardItems(plan_info)
			local reward = {}
			local seq = 0
			-- BuglyReport(reward_item)
			if reward_item and table.nums(reward_item) > 0 then
				for k,v in pairs(reward_item) do
					local item = {}
					item.item_id = v.item_id
					item.num = v.num
					item.is_bind = v.is_bind
					item.index = 0
					seq = seq + 1 
					item.seq = seq
					item.count = table.nums(reward_item)
					table.insert(reward,seq,item)
				end
			end
			self.rewardlist[2]:SetData(reward) 
			self.rewardlist[2]:SetCompleteCallBack(function()	
				self.ScrollRect.enabled = false
				self.Content.enabled = false
				self:ShowRewardMove(reward)
			end)
		else
			local seq = 0
			self.part2_item = {}
			local reward_item = self:GetPlanRewardItems(plan_info)
			if reward_item and table.nums(reward_item) > 0 then
				for k,v in pairs(reward_item) do
					local item = {}
					item.item_id = v.item_id
					item.num = v.num
					item.is_bind = v.is_bind
					seq = seq + 1 
					item.seq = seq
					table.insert(self.part2_item,seq,item)
				end
			end
		end
	end
	if activity_info then
		self:FlushTimeAndName(list,2)
	end
end

function PlanActivityView:ShowRewardMove(reward)
	self.move_record = {}
	local index = 0
	for k,v in pairs(reward) do
		index = index + 1
		table.insert(self.move_record,index,-1)
	end
	self.add_timer = TimeHelper:AddDelayTimer(function ()
		if self.ContentPos and self.ContentPos.anchoredPosition then
			self.ContentPos.anchoredPosition = Vector2.New(self.ContentPos.sizeDelta.x, 0)
			if self.timer_rt == nil then
				self.timer_rt = TimeHelper:AddRunTimer(BindTool.Bind(self.Rolling, self,reward), 0)
			end
		end
	end, 0.2)
end

function PlanActivityView:Rolling(reward)
	if self.ContentPos and self.ContentPos.anchoredPosition then
		local x =self.ContentPos.anchoredPosition.x - 60 * Time.deltaTime
		local have_move = math.floor((-195) - x)
		local move_count = math.floor(have_move/110)
		local reward_count = table.nums(reward) or 0
		local yu_index = math.fmod(move_count,reward_count)
		local zheng_index = math.modf(move_count,reward_count)
		if move_count > 0 then
			local add_index = yu_index
			if add_index == 0 then
				add_index = reward_count
			end
			for k,v in pairs(reward) do
				if v.seq == add_index then
					if self.move_record[add_index] ~= zheng_index then
						self.move_record[add_index] = zheng_index
						v.index = v.index + 1
						if table.nums(self.part2_item) > 0 then
							for k1,v1 in pairs(self.part2_item) do
								if v1.seq == add_index then
									v.item_id = v1.item_id
									v.num = v1.num
								end
							end
						end
						self.rewardlist[2]:SetItemDataByKey(k,v)
						break
					end
				end
			end	
			--self.rewardlist[2]:SetData(reward) 
		end
		self.ContentPos.anchoredPosition = Vector2.New(x,0)
	end
end

function PlanActivityView:FlushPartThree(list)
	local activity_info,plan_info = self:GetActInfoByIndex(list,3)
	if plan_info then
		self:FlushRewardItems(plan_info,3)
	end
	if activity_info then
		self:FlushTimeAndName(list,3)
	end
end

function PlanActivityView:FlushPartFour(list)
	local activity_info,plan_info = self:GetActInfoByIndex(list,4)
	if plan_info then
		self:FlushRewardItems(plan_info,4)
	end
	if activity_info then
		self:FlushTimeAndName(list,4)
	end
end

function PlanActivityView:FlushPartFive(list)
	local activity_info,plan_info = self:GetActInfoByIndex(list,5)
	if plan_info then
		self:FlushRewardItems(plan_info,5)
	end
	if activity_info then
		self:FlushTimeAndName(list,5)
	end
end

function PlanActivityView:FlushPartSix(list)
	local activity_info,plan_info = self:GetActInfoByIndex(list,6)
	if plan_info then
		UH.SetText(self.qipaotext,plan_info.describe)
	end
	if activity_info then
		self.QiPao:SetActive(false)
		self:FlushTimeAndName(list,6)
		local show_list = ServerActivityData.Instance:GetThrivingShowList()
		local consume_item_id = 0
		for k,v in pairs(show_list) do
			if consume_item_id == 0 then
				consume_item_id = v.consume_item_id
			end
			local is_unlock, timestamp = ServerActivityData.Instance:IsThrivingItemUnlock(v)	
			if not is_unlock then
				local time_have = timestamp - TimeHelper.GetServerTime()
				if time_have <= (v.time1 * TIME_HOUR_SECOND_NUM) then
					self.QiPao:SetActive(true)
					local name = Item.GetQuaName(v.exchange_item.item_id, true)
					local str = string.format(Language.PlanActivity.QIPaoText,name)
					UH.SetText(self.qipaotext,str)
					break
				end
			end
		end
		UH.SetIcon(self.icon,Item.GetIconId(consume_item_id),ICON_TYPE.ITEM)
		UH.SetText(self.iconnum,Item.GetNum(consume_item_id))
	end
end


function PlanActivityView:OnClickGo(param)
	if self.btn_go[param] then
		ViewMgr:OpenViewByKey(self.btn_go[param].mod_key)
		ActivityData.Instance:SetHasSign(self.btn_go[param].act_type)
		TimeHelper:CancelTimer(self.timer_fresh)
		self.timer_fresh = TimeHelper:AddDelayTimer(function()
			self:OnFlushShowView()
		end, 1)
	end
end

function PlanActivityView:OnClickArrow(param)
--[[	local index = self.part2_item_index + param
	if index > 0 and index <= table.nums(self.part2_item) then
		local reward = {}
		for k,v in pairs(self.part2_item) do
			if k == index - 1 then
				table.insert(reward,k,v)
			end
		end
		self.rewardlist[2]:SetData(reward) 
		self.part2_item_index = index
	end--]]
end

function PlanActivityView:OnClickClose()
    ViewMgr:CloseView(PlanActivityView)
end


PlanActivityItem = PlanActivityItem or DeclareMono("PlanActivityItem", UIWidgetBaseItem)
function PlanActivityItem:PlanActivityItem()

end

function PlanActivityItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local item = Item.Create({item_id = data.item_id, num = data.num})
	self.Cell:SetData(item)
	if data.seq and data.index >0 then
		local add_move = (data.count - 1) * 110 * data.index + 110*data.index
		local x = 40 +(data.seq -1)*110 + add_move
		self.ItemRect.anchoredPosition = Vector2.New(x,-40)
	end
end

--���ڻ���Ѽ���
--================================MainViewPlanActivityView================================
MainViewPlanActivityView = MainViewPlanActivityView or DeclareMono("MainViewPlanActivityView", UIWFlushPanel)

function MainViewPlanActivityView:MainViewPlanActivityView()
    self.data_cares = {
        {data = ActivityData.Instance:GetActivityEvent(),func = self.OnFlush,keys = {"act_type", "status"}},
        {data = RoleData.Instance:GetBaseData(), func = self.OnFlush, init_call = false, keys = {"level"}},
        {data = ActivityData.Instance.acts_tips_data,func = self.FlushTipsView,init_call = false},
    }
end

function MainViewPlanActivityView:OnFlush()
    self:FlushTipsView()
end

function MainViewPlanActivityView:FlushTipsView()
    PlanActivityData.Instance:GetRandPlanList()
    self.Tips:SetActive(PlanActivityData.Instance:GetPlanActivityTip())
	local begin_time,end_time = PlanActivityData.Instance:GetActBeginAndEndAndDayTime()
	self.Timer:StampTime(end_time,TimeType.Type_Time_3, "")
end