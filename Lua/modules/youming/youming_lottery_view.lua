-- 幽冥探宝
YouMingLotteryView = YouMingLotteryView or DeclareView("YouMingLotteryView", "youming/youming_lottery_view", Mod.YouMing.Lottery)
function YouMingLotteryView:YouMingLotteryView()
    
end

function YouMingLotteryView:OnCloseClick()
    ViewMgr:CloseView(YouMingLotteryView)
end


YouMingLotteryPanel = YouMingLotteryPanel or DeclareMono("YouMingLotteryPanel", UIWFlushPanel)
function YouMingLotteryPanel:YouMingLotteryPanel()
    self.data = YouMingData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushStuffInfo, init_call = false},
        {data = self.data.lottery_info, func = self.FlushPanel, init_call = false},
        {data = self.data.lottery_result, func = self.FlushResult, init_call = false}
    }

    self.play_queue = {}
    self.play_queue_index = 1
    self.effect_ht_list = {}
end

function YouMingLotteryPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.TabLock:SetActive(false)

    self.reward_progress_group = DataHelper.ConvertGroup(Config.dim_pet_cfg_auto.turntable_schedule_reward, "turntable_id")

    self.TabList[1].isOn = true
    self.tab_type = 0   -- 0是普通探宝，1是高级探宝
    self.is_jump = false
    self:SetAxleRotZ(0)
    self:FlushPanel()
end

function YouMingLotteryPanel:GetRewardItemList(tab_type)
    if TableIsEmpty(self.data.lottery_info) then
        return {}
    end
    return self.data.lottery_info[tab_type].reward_list or {}
end

function YouMingLotteryPanel:GetItemId()
    return self.tab_type == 0 and Config.dim_pet_cfg_auto.other[1].turntable_use or Config.dim_pet_cfg_auto.other[1].high_turntable_use
end

function YouMingLotteryPanel:SetAxleRotZ(angle)
    self.axle = self.AxleList[self.tab_type + 1]
    self.rot_z = angle
    UH.LocalEulerAngles(self.axle, Vector3.New(0,0,self.rot_z))
end

function YouMingLotteryPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.runner_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.runner_timer)
    end
    TimeHelper:CancelTimer(self.play_time_ht)
    self.is_jump = true
    self:DialEffectEndFunc()
end

function YouMingLotteryPanel:FlushPanel()
    local luck_value = self.data:GetLuckyValue(self.tab_type)
    UH.SetText(self.LuckValue, luck_value)
    
    local item_list_data = self:GetRewardItemList(self.tab_type)
    for i = 1, self.ItemList:Length() do
        local data = item_list_data[i]
        if data then
            self.ItemList[i]:SetData(Item.Create(data))
        end
    end
    local point_list_data = self.reward_progress_group[self.tab_type]
    local max_value = point_list_data[#point_list_data].schedule
    if luck_value < max_value then
        max_value = max_value + (max_value / 5)
    end
    self.ProgressBar:SetProgress(luck_value / max_value)
    for i = 1,self.ProgressItemList:Length() do
        local data = point_list_data[i]
        self.ProgressItemList[i]:SetData(Item.Create({item_id = data.item_id, num = data.item_num}))
        UH.SetText(self.ProgressNumList[i], data.schedule)
        local poing_img = luck_value >= data.schedule and "JieDian_Liang" or "JieDian_An"
        UH.SpriteName(self.ProgressPointList[i], poing_img)
        local flag = self.data:GetLotteryRewardFlag(data)
        self.GetRewardList[i]:SetActive(flag == 1)
    end

    self:FlushStuffInfo()
end

function YouMingLotteryPanel:FlushStuffInfo()
    local item_id = self:GetItemId()
    local icon_id = Item.GetIconId(item_id)
    local has_num = Item.GetNum(item_id)
    UH.SetIcon(self.Icon1, icon_id, ICON_TYPE.ITEM)
    UH.SetIcon(self.Icon2, icon_id, ICON_TYPE.ITEM)
    UH.SetText(self.Count1, DataHelper.ConsumeNum(has_num, 1, true))
    UH.SetText(self.Count2, DataHelper.ConsumeNum(has_num, 10, true))
    self.RedPointList[1]:SetNum(has_num)
    self.RedPointList[2]:SetNum(has_num >= 10 and 1 or 0)
end

function YouMingLotteryPanel:FlushResult()
    if TableIsEmpty(self.data.lottery_result) then
        return
    end
    for i,v in pairs(self.data.lottery_result) do
        table.insert(self.play_queue, v)
    end
    self.data.lottery_result:Set({})
    self:Play(self.play_queue[self.play_queue_index])
end

-- index == 1 ~ 8
function YouMingLotteryPanel:Play(index, process_time)
    if self.playing then
        return
    end

    self.play_effect_ht = self.EffectTool:Play("7167014")
    for i,v in pairs(self.effect_ht_list) do
        self.EffectTool:Stop(v)
    end

    self.target_angle_z = -45 * (index - 1)
    if self.is_jump then
        self:SetAxleRotZ(self.target_angle_z)
        self:DialEffectEndFunc()
        return
    end
    self.TabLock:SetActive(true)
    self.playing = true
    self.speed = 0
    self.min_speed = 10            -- 最小速度
    self.stop_min_speed = 8         -- 将要停止时最小速度
    self.max_speed = 40           -- 最大速度
    self.accelerated_speed = 2      -- 加速度
    self.process_time = process_time or 2           -- 加速过程时间
    self.is_stop = false

    if self.runner_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.runner_timer)
    end
    self.runner_timer = Runner.Instance:RunUntilTrue(BindTool.Bind(self.RunnerFunc,self))
end

function YouMingLotteryPanel:RunnerFunc()
     -- 减速处理
     if self.start_time and Time.time - self.start_time >= self.process_time and not self.is_stop and self.accelerated_speed > 0 then
        self.accelerated_speed = -self.accelerated_speed
    end
    if not self.is_stop then
        self.speed = self.speed + self.accelerated_speed
        if self.accelerated_speed < 0 then
            -- 减到最小速度后，开始锁定目标吧
            if self.speed <= self.min_speed then
                self.speed = self.min_speed
                self.is_stop = true
            end
        else
            if self.speed >= self.max_speed then
                self.speed = self.max_speed
                if not self.start_time then
                    self.start_time = Time.time
                end
            else
                self.speed = self.speed
            end
        end
    end

    local rot_z = self.rot_z - (self.speed * Time.deltaTime * 30)
    if self.is_stop == true then
        if self.speed > self.stop_min_speed then
            self.speed = self.speed - 0.1
        end
        if rot_z <= self.target_angle_z - 360 then
            rot_z = self.target_angle_z
            self.start_time = nil
            self.TabLock:SetActive(false)
            self:DialEffectEndFunc()
            self:SetAxleRotZ(rot_z)
            return true
        end
        self:SetAxleRotZ(rot_z)
    else
        rot_z = rot_z <= -360 and rot_z + 360 or rot_z
        self:SetAxleRotZ(rot_z)
    end
end

function YouMingLotteryPanel:DialEffectEndFunc()
    self.playing = false
    if self.play_effect_ht then
        self.EffectTool:Stop(self.play_effect_ht)
        self.play_effect_ht = nil
    end

    if self.is_jump then
        for i = self.play_queue_index, #self.play_queue do
            local index = self.play_queue[i]
            self:EndShow(index)
        end
        self.play_queue = {}
        self.play_queue_index = 1
    else
        local index = self.play_queue[self.play_queue_index]
        self:EndShow(index)
        self.play_queue_index = self.play_queue_index + 1
        if self.play_queue_index <= #self.play_queue then
            self.playing = true
            TimeHelper:CancelTimer(self.play_time_ht)
            self.play_time_ht = Invoke(function ()
                self.playing = false
                self:Play(self.play_queue[self.play_queue_index],0.5)
            end, 0.5)
        else
            self.play_queue = {}
            self.play_queue_index = 1
        end
    end
end

function YouMingLotteryPanel:EndShow(index)
    local item_list_data = self:GetRewardItemList(self.tab_type)
    local data = item_list_data[index]
    if data then
        local item = Item.Create(data)
        PublicPopupCtrl.Instance:CenterI(item)
    end
    local effect_id = index % 2 == 0 and 7167015 or 7167016
    local effect_ht = self.EffectTool:Play(effect_id, self.ResultEffectList[index].gameObject)
    table.insert(self.effect_ht_list, effect_ht)
end

function YouMingLotteryPanel:OnBtn1Click()
    if self.playing then
        return
    end
    local item_id = self:GetItemId()
    local has_num = Item.GetNum(item_id)
    if has_num < 1 then
        MainOtherCtrl.Instance:GetWayViewById(item_id)
        return
    end
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.HUNT_TREASURE, self.tab_type, 0)
end

function YouMingLotteryPanel:OnBtn2Click()
    if self.playing then
        return
    end
    local item_id = self:GetItemId()
    local has_num = Item.GetNum(item_id)
    if has_num < 10 then
        MainOtherCtrl.Instance:GetWayViewById(item_id)
        return
    end
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.HUNT_TREASURE, self.tab_type, 1)
end


function YouMingLotteryPanel:OnTabSelectClick(tab_type)
    self.tab_type = tab_type
    self:SetAxleRotZ(0)
    self:FlushPanel()
end

function YouMingLotteryPanel:OnJumpSelectClick(value)
    self.is_jump = value
end

function YouMingLotteryPanel:OnGetProgressPointClick(index)
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.FETCH_LUCKY_REWARD, self.tab_type, index - 1)
end