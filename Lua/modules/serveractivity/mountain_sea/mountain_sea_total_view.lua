MountainSeaTotalView = MountainSeaTotalView or DeclareView("MountainSeaTotalView", "serveractivity/mountain_sea/mountain_sea_total_view", Mod.MountainSea.Main)
VIEW_DECLARE_LEVEL(MountainSeaTotalView, ViewLevel.L0)

function MountainSeaTotalView:MountainSeaTotalView()
    self:SetTimerCloseView()
end

function MountainSeaTotalView:SetTimerCloseView()
	local begin_time,end_time = PlanActivityData.Instance:GetActMSBeginAndEndAndDayTime()
	self.timer_close_view = TimeHelper:AddRunTimer(function ()
   		if TimeHelper.GetServerTime() > end_time then
				PublicPopupCtrl.Instance:Error(Language.PlanActivity.TipsText)
				ViewMgr:CloseView(MountainSeaTotalView)
			end
	end, 1)	
end

function MountainSeaTotalView:CloseCallback()
	TimeHelper:CancelTimer(self.timer_close_view)
end

function MountainSeaTotalView:OnClickBack()
    ViewMgr:CloseView(MountainSeaTotalView)
end

MountainSeaTotalPanel = MountainSeaTotalPanel or DeclareMono("MountainSeaTotalPanel", UIWFlushPanel)
function MountainSeaTotalPanel:MountainSeaTotalPanel()
    -- 活动的具体data走call，其余的方法应整合到call中
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = ActivityData.Instance:GetActivityEvent(), func = self.OnFlushShowView},
        {data = BagData.Instance.item_grid, func = self.OnFlushShowView, init_call = false},
    }

end

function MountainSeaTotalPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)    
    TimeHelper:CancelTimer(self.handle_red_show)
    self.handle_red_show = nil

    Remind.Instance:UnRegisterGroup(Mod.RandActivity,self.handle_Rand)
    Remind.Instance:UnRegisterGroup(Mod.MountainSea,self.handle_mountain)
end

function MountainSeaTotalPanel:OnFlushShowView()
    local cur_act = ServerActivityData.Instance:GetMountainSeaTotalView()
    for i = 1, self.act_parts:Length() do
        self.act_parts[i].gameObject:SetActive((cur_act[i] ~= nil) and cur_act[i].is_empty == nil)
        if(cur_act[i] ~= nil) and cur_act[i].is_empty == nil then 
            self.act_parts[i]:SetData(cur_act[i])
        end 
    end 

    TimeHelper:CancelTimer(self.handle_red_show)
    self.handle_red_show = nil
    
    self.handle_red_show =TimeHelper:AddRunTimer(function()
        for i = 1, self.act_parts:Length() do
            if(cur_act[i] ~= nil) and cur_act[i].is_empty == nil then 
                self.act_parts[i]:RedFlush()
            end 
        end 
    end , 3)

    if self.handle_mountain == nil then 
        self.handle_mountain =  Remind.Instance:RegisterGroup(Mod.MountainSea,function()
            local cur_act = ServerActivityData.Instance:GetMountainSeaTotalView()
            for i = 1, self.act_parts:Length() do
                if(cur_act[i] ~= nil) and cur_act[i].is_empty == nil then 
                    self.act_parts[i]:RedFlush()
                end 
            end 
    
        end,true)
    end 

    if self.handle_Rand == nil then 
        self.handle_Rand = Remind.Instance:RegisterGroup(Mod.RandActivity,function()
            local cur_act = ServerActivityData.Instance:GetMountainSeaTotalView()
            for i = 1, self.act_parts:Length() do
                if(cur_act[i] ~= nil) and cur_act[i].is_empty == nil then 
                    self.act_parts[i]:RedFlush()
                end 
            end 
        end,true)
    end
end


MountainSeaTotalActCell = MountainSeaTotalActCell or DeclareMono("MountainSeaTotalActCell", UIWFlushPanel)
function MountainSeaTotalActCell:MountainSeaTotalActCell() end

function MountainSeaTotalActCell:OnDestroy()
    UIWFlushPanel.OnDestroy(self)   
    TimeHelper:CancelTimer(self.handle_roll_show)
    self.handle_roll_show = nil
end

function MountainSeaTotalActCell:SetData(data)
    self.data = data

    UH.SetText(self.act_name,self.data.name)-- .." 类型"..data.plan_type)

    local open_time,end_time = ServerActivityData.Instance:GetOpenAndEndTime(data.act_type)
    
    if open_time and end_time then

        local end_day = end_time.day
        local end_mon = end_time.month
        local start_day = open_time.day
        local start_mon = open_time.month

        if data.act_type == ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_TASK then 
            local cfg = ServerActivityData:GetMountainSeaActCfg(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_TASK)
            local pass_time = cfg.section_end - cfg.section_start 

            local act_info = ActivityData.Instance:GetActivityStatusInfo(data.act_type)
            if cfg.section_start == 4 then 
                local end_temp = act_info.param_1 + pass_time * TIME_DAY_SECOND_NUM
                local fix_end_time = TimeManager.FormatUnixTime2Date(end_temp)
                end_day = fix_end_time.day
                end_mon = fix_end_time.month
                -- end_time.day = open_time.day + pass_time + 1
            else
                -- open_time.day = end_time.day - (pass_time + 1)
                local open_temp = act_info.param_2 - ((pass_time+1) * TIME_DAY_SECOND_NUM)
                local fix_star_time = TimeManager.FormatUnixTime2Date(open_temp)
                start_day = fix_star_time.day
                start_mon = fix_star_time.month

                if end_time.hour ~= nil and end_time.hour == 0 then 
                    local act_info = ActivityData.Instance:GetActivityStatusInfo(data.act_type)
                    local fix_temp = act_info.param_2 - 1
                    local fix_end_time = TimeManager.FormatUnixTime2Date(fix_temp)
                    end_day = fix_end_time.day
                    end_mon = fix_end_time.month
                end 
            end 
        end
        
        if data.act_type ~= ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_TASK then 
            if end_time.hour ~= nil and end_time.hour == 0 then 
                local act_info = ActivityData.Instance:GetActivityStatusInfo(data.act_type)
                local fix_temp = act_info.param_2 - 1
                local fix_end_time = TimeManager.FormatUnixTime2Date(fix_temp)
                end_day = fix_end_time.day
                end_mon = fix_end_time.month
            end 
        end 

        local str = string.format(Language.PlanActivity.ActivityTime,tostring(start_mon),
            tostring(start_day),tostring(end_mon),tostring(end_day))
        UH.SetText(self.act_timer,Language.PlanActivity.ActivityTimeTol..str)
    end	

    if self.act_desc ~= nil then 
        UH.SetText(self.act_desc,data.describe)
    end 

    -- self.act_reward_cell 
    if self.act_reward_cell  ~= nil then             
        self.act_reward_items = {}
        for k,v in pairs(data.rewards or {}) do 
            local vo = BagData.Instance:ItemBuild(v)
            table.insert(self.act_reward_items,vo)
        end 
        self.act_reward_index = 1
        self.act_reward_cell:SetData(self.act_reward_items[self.act_reward_index])
        
        if #self.act_reward_items > 1 then  
            if self.handle_roll_show ~= nil then 
               TimeHelper:CancelTimer(self.handle_roll_show)
                self.handle_roll_show = nil
           end 
            self.handle_roll_show =TimeHelper:AddRunTimer(function()
                self.act_reward_index = self.act_reward_index + 1
                if self.act_reward_index > #self.act_reward_items then 
                    self.act_reward_index = 1
                end 
                if self.act_reward_cell ~= nil and self.act_reward_cell.SetData ~= nil then
                    self.act_reward_cell:SetData(self.act_reward_items[self.act_reward_index])
                end 
            end,5)
        end

        if self.data.plan_type == 6 then  
            if self.data.act_type == ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_MINEC then 
                local is_show_item,cfg = ServerActivityData.Instance:GetMinecTotalShowItem()

                if self.act_reward_cell_t ~= nil then 
                    self.act_reward_cell_t:SetActive(is_show_item)
                end 
                if is_show_item then 
                    self.act_reward_cell:SetData(BagData.Instance:ItemBuild(cfg.reward_item))
                    UH.SetText(self.act_desc,cfg.desc)
                else 
                    UH.SetText(self.act_desc,"")
                end 
            end 
        end 
    end 

    if self.act_rewards ~= nil then
        local rewards = {}
        for k,v in pairs(data.rewards or {}) do 
            local vo = BagData.Instance:ItemBuild(v)
            table.insert(rewards,vo)
        end 
        self.act_rewards:SetData(rewards) 
    end 

    if self.neo_tips ~= nil then 
        self.neo_tips:SetActive(not ServerActivityData.Instance:GetMountainSeaRecallNeo(self.data.act_type)) 

        self.neo_red_point = self.neo_red_point or UiRedPoint.New(self.neo_tips, Vector3.New(0,0,0))
        self.neo_red_point:SetNum(1)
    end 

    self:RedFlush()
end

function MountainSeaTotalActCell:RedFlush()
    if self.red_pos ~= nil then 
        self.red_point = self.red_point or UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
        local num = Remind.Instance:GetRemindNum(self.data.mod_key)
        self.red_point:SetNum(num)
    end 
end

function MountainSeaTotalActCell:OnClickNext(plus)
    self.act_reward_index = plus > 0 and (self.act_reward_index + 1) or (self.act_reward_index - 1 )
    self.act_reward_index = self.act_reward_index > #self.act_reward_items and 1  or (self.act_reward_index < 1 and #self.act_reward_items or self.act_reward_index)
    self.act_reward_cell:SetData(self.act_reward_items[self.act_reward_index])
end 

function MountainSeaTotalActCell:OnClickJoin()
    if self.data.tick ~= nil then  
        local num = Item.GetNum(self.data.tick)
        if num == 0 then 
            if self.data.tick_limit == 1 then 
                MainOtherCtrl.Instance:GetWayView({item = Item.Init(self.data.tick)})
            elseif self.data.tick_limit == 2 then 
                ViewMgr:OpenViewByKey(self.data.jump_mod)
            end 
            return 
        end 
    end 
    ViewMgr:OpenViewByKey(self.data.mod_key)

    ServerActivityData.Instance:SetMountainSeaRecallNeo(self.data.act_type)
    if self.neo_tips ~= nil then self.neo_tips:SetActive(false) end 
end