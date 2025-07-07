-- require("D:/O/Assets/Game/Lua/config/auto/duanyangfuli_auto")
-- require("config/auto/duanyangfuli_auto")
--===========================一般界面模板===========================
ActDuanYangFuLiView = ActDuanYangFuLiView or DeclareView("ActDuanYangFuLiView", "serveractivity/duanyangfuli/act_duanyangfuli",Mod.DragonBoat.DuanYangFuLi)

function ActDuanYangFuLiView:ActDuanYangFuLiView()
    self.data = ActDuanYangFuLiData.Instance
    -- LogError("self.data",self.data)
end

-- 回调
function ActDuanYangFuLiView:LoadCallback()
    local cfg_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DUANYANGFULI, Config.duanyangfuli_auto.basis_configuration)
    self.data.section = cfg_list
    -- LogError("端阳福礼sectionsectionsectionsection",cfg_list)

    self.data.actOpenDay = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_DUANYANGFULI)
    -- local actOpenDay = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_DUANYANGFULI)
    -- LogError("actOpenDay",actOpenDay)
    -- local server_time = TimeManager.GetServerTime()
    -- local day_end_time = TimeManager.DayEnd(server_time)
    -- local refresh_time = day_end_time - server_time
    -- self.zero_point_timer = TimeManager:AddDelayTimer(ActDuanYangFuLiPanel:FlushDuanYangFuLiItemList(), refresh_time)
end

function ActDuanYangFuLiView:CloseCallback()
    -- if self.zero_point_timer ~= nil then
    --     TimeManager:CancelTimer(self.zero_point_timer)
    -- end

end

-- 关闭界面
function ActDuanYangFuLiView:OnClickClose()
    ViewMgr:CloseView(ActDuanYangFuLiView)
end

-- 点击提示
function ActDuanYangFuLiView:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[331].desc})
end

--===========================界面刷新模板===========================
ActDuanYangFuLiPanel = ActDuanYangFuLiPanel or DeclareMono("ActDuanYangFuLiPanel", UIWFlushPanel)

function ActDuanYangFuLiPanel:ActDuanYangFuLiPanel()
    self.data = ActDuanYangFuLiData.Instance
    self.data_cares = {
        {data = self.data.duanyangfuli_smart, func = self.FlushDuanYangFuLiPanel,init_call = false},
        {data = self.data.duanyangfuli_smart, func = self.FlushDuanYangFuLiItemList,init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
    self:SetActiveRemainingTimeMeter()
end

function ActDuanYangFuLiPanel:Awake()
    UIWFlushPanel.Awake(self)
    -- self.data = ActDuanYangFuLiData.Instance
    self.data.actOpenDay = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_DUANYANGFULI)
    -- LogError("actOpenDay",self.data.actOpenDay)
    local server_time = TimeManager.GetServerTime()
    local day_end_time = TimeManager.DayEnd(server_time)
    local refresh_time = day_end_time - server_time
    self.zero_point_timer = TimeHelper:AddDelayTimer(self:FlushDuanYangFuLiItemList(), refresh_time)

    self.data.first_remind = true
    -- 刷红点
    self.data:RemindNotice()

    -- 设置一键购买花费数量
    UH.SetText(self.DiscountPrice, self.data:GetDiscountPrice())
    UH.SetText(self.AllBagMoney, self.data:GetAllBagMoney())

    -- 活动剩余时间回调
    self.ActiveRemainingTimeMeter:SetCallBack(function ()
        ViewMgr:CloseView( ActDuanYangFuLiView)
    end)

    -- 设置剩余活动时间
    -- self.ActiveRemainingTimeMeter:StampTime(ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_ZHAOCAIJINBAO).next_status_switch_time, TimeType.Type_Time_3)
    -- self.ActiveRemainingTimeMeter:CreateActTime(ACTIVITY_TYPE.RAND_DUANYANGFULI,TimeType.Type_Time_3,Language.DuanYangFuLi.DuanYangFuLiTime)

    self:FlushDuanYangFuLiItemList()
    self:FlushDuanYangFuLiPanel()
end

function ActDuanYangFuLiPanel:SetActiveRemainingTimeMeter()
    self.ActiveRemainingTimeMeter:CloseCountDownTime()
    self.ActiveRemainingTimeMeter:SetShowCallBack(function (t)
        local t_str = ""
		if t.day > 0 then
			t_str = string.format(Language.DuanYangFuLi.Time1,t.day, t.hour,t.min)
		else
			t_str = string.format(Language.DuanYangFuLi.Time2,t.hour,t.min)
		end
        return t_str
	end)

    local activityStartTime,activityEndTime = ActivityRandData.GetBaseDayOpenStampTime(ACTIVITY_TYPE.RAND_DUANYANGFULI)
	-- self.ActiveRemainingTimeMeter:CreateActTime(ACTIVITY_TYPE.RAND_DUANYANGFULI,TimeType.Type_Time_3)
    self.ActiveRemainingTimeMeter:StampTime(activityEndTime,TimeType.Type_Time_3)
end


function ActDuanYangFuLiPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    -- 刷红点
    self.data:RemindNotice()
    if self.zero_point_timer ~= nil then
        TimeHelper:CancelTimer(self.zero_point_timer)
    end
end


-- 刷新界面按钮
function ActDuanYangFuLiPanel:FlushDuanYangFuLiPanel()
    local isBuyFlag = self.data:IsBuyEvenOnlyOne()
    self.BuyAllButton:SetActive(isBuyFlag == false)
    self.RightInfo:SetActive(isBuyFlag == false)
end

-- 刷新界面按钮
function ActDuanYangFuLiPanel:FlushDuanYangFuLiItemList()
    self.data.actOpenDay = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_DUANYANGFULI)
    -- LogError("actOpenDay",self.data.actOpenDay)

    -- self.data = ActDuanYangFuLiData.Instance
    -- 获得大item列表
    local duanyangfuli_item_list = self.data:GetDuanYangFuLiItemList()

    -- 设置panel的大item列表
    if duanyangfuli_item_list then
        self.DuanYangFuLiItemList:SetData(duanyangfuli_item_list)
    end
end


function ActDuanYangFuLiPanel:OnClickBuyAll()
    self.data.currentMoney = self.data:GetDiscountPrice()
    -- LogError("self.data.currentMoney",self.data.currentMoney)
    -- 发送购买协议
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_DUANYANGFULI, self.data.currentMoney, ACTIVITY_TYPE.RAND_DUANYANGFULI, ActDuanYangFuLiCtrl.REQ_TYPE.REQ_BUY,0,1)
    -- ActDuanYangFuLiCtrl.Instance:SendDuanYangFuLiReq(ActDuanYangFuLiCtrl.REQ_TYPE.REQ_BUY,0,1)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)

end

-- 发送购买
function ActDuanYangFuLiPanel:SendBuy()
    -- if self.cfg == nil then
    --     return
    -- end
    -- PublicPopupCtrl.Instance:AlertTip(Language.DuanYangFuLi.CanNotDiscount,function ()
        if  self.data.currentMoney ~= nil then
            -- LogError("self.data.currentMoney",self.data.currentMoney)
            ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE. TYPE_DUANYANGFULI,self.data.currentMoney*10)
        end
    -- end)

end
-- function ActDuanYangFuLiPanel:OnEnable()
--     UIWFlushPanel.OnEnable(self)
-- end

-- function ActDuanYangFuLiPanel:OnDisable()
--     UIWFlushPanel.OnDisable(self)
-- end

-- function ActDuanYangFuLiPanel:OnFlush()
-- end

--===========================界面Item模板===========================
ActDuanYangFuLiItem = ActDuanYangFuLiItem or DeclareMono("ActDuanYangFuLiItem", UIWidgetBaseItem)

function ActDuanYangFuLiItem:ActDuanYangFuLiItem()
    self.data_instance = ActDuanYangFuLiData.Instance
end

function ActDuanYangFuLiItem:SetData(data)
    -- UIWidgetBaseItem.SetData(self, data)

    -- 设置是多少元的礼包
    self.gift_name = data.gift_name
    -- LogError("self.gift_name",self.gift_name)
    UH.SetText(self.BagBuyNumText, self.gift_name)

    -- 设置是多少天
    self.activity_times = data.activity_times
    -- LogError(" self.activity_times",  self.activity_times)
    self.receive_days = data.receive_days
    -- LogError("self.receive_days", self.receive_days)
    -- local activityStartTime = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_DUANYANGFULI)
    local activityStartTime,activityEndTime = ActivityRandData.GetBaseDayOpenStampTime(ACTIVITY_TYPE.RAND_DUANYANGFULI)
    -- LogError("activityStartTime",activityStartTime)
    local startTime = activityStartTime + (self.activity_times - 1)*86400
    local endTime = activityStartTime + (self.activity_times - 1 + self.receive_days  - 1)*86400

    local startMonth =  os.date("%m",startTime)
    startMonth = tonumber(startMonth)
    -- LogError("startMonth", startMonth)
    local startDay =  os.date("%d",startTime)
    startDay = tonumber(startDay)
    -- LogError("startDay", startDay)

    local endMonth =  os.date("%m",endTime)
    endMonth = tonumber(endMonth)
    -- LogError("endMonth", endMonth)
    local endDay =  os.date("%d",endTime)
    endDay = tonumber(endDay)
    -- LogError("endDay", endDay)

    UH.SetText(self.Date, startMonth.."月"..startDay.."日~"..endMonth.."月"..endDay.."日")

    -- 获得当前顺序
    self.seq = data.seq

    -- 获得服务端下发的东西
    self.is_buy = data.is_buy
    self.fetch_reward_times = data.fetch_reward_times

    -- 说明第一次刷，还没给LastFetchRewardTimes[self.seq]赋值
    if self.data_instance.LastFetchRewardTimes[self.seq] == nil then
        self.data_instance.LastFetchRewardTimes[self.seq] = 0
        -- self.data_instance.LastFetchRewardTimes[self.seq] = self.fetch_reward_times
    end

    -- 获得奖励的reward_item
    self.reward_item = data.reward_item

    -- 设置钱的数值
    self.price = data.price
    UH.SetText(self.MoneyText, self.price/10)

    -- 如果没买才显示钱的按钮
    self.MoneyButton:SetActive(not self.is_buy and self.data_instance.actOpenDay >= self.activity_times)
    -- 买了的情况下  
    if self.is_buy == true then
        UH.SetText(self.GetBtnText, Language.DuanYangFuLi.GetText)
        if self.data_instance.actOpenDay < self.activity_times then
            self.GetButton:SetActive(true)
            self.GetRedPoint:SetNum(0)
            self.HaveGet:SetActive(false)
            self.GetButtonInteractorHandler.Interactable = false    
        else
            -- 且 已领取次数小于最大领取次数 且 活动开启天数-礼包活动时间+1>礼包已领取次数，可以领取 才显示领取按钮
            self.GetButton:SetActive(self.fetch_reward_times < self.receive_days and self.data_instance.actOpenDay - self.activity_times + 1 > self.fetch_reward_times )
            -- 如果领取按钮显示的话说明还没领第一次
            if self.GetButton.activeSelf == true then
                self.data_instance.IsGetFirst[self.seq] = false
                self.GetRedPoint:SetNum(1)
                self.GetButtonInteractorHandler.Interactable = true
            end
            self.HaveGet:SetActive(not (self.fetch_reward_times < self.receive_days and self.data_instance.actOpenDay - self.activity_times + 1 > self.fetch_reward_times ))
            -- 如果领显示已领取按钮了 而且第一次还没领 说明是第一次领 才弹窗
            if self.HaveGet.activeSelf == true and self.data_instance.IsGetFirst[self.seq] == false then
                -- 判断当前领取次数和上一次领取次数的差值
                local fetch_reward_times_change = self.fetch_reward_times - self.data_instance.LastFetchRewardTimes[self.seq]
                local all_item_list = {}
                -- LogError("fetch_reward_times_change",fetch_reward_times_change)
                if fetch_reward_times_change ~= 0  then
                    -- 如果只领取了一天的，就把一天的奖励传到表中
                    if fetch_reward_times_change == 1 then
                        all_item_list = self.reward_item
                    -- 如果只领取了两天或以上的的
                    elseif fetch_reward_times_change >= 2 then
                        -- 一次性领了几天奖励数量就成几
                        for i = 1, fetch_reward_times_change do
                            for key, value in pairs(self.reward_item) do
                                table.insert( all_item_list,value)
                            end
                        end
                    end
                end
                ViewMgr:OpenView(DuanYangFuLiRewardView, {item_list = all_item_list})
                -- 弹窗完 说一下第一次领完了
                self.data_instance.IsGetFirst[self.seq] = true
            end
        end
    elseif self.data_instance.actOpenDay < self.activity_times then
        self.GetButton:SetActive(true)
        UH.SetText(self.GetBtnText, Language.DuanYangFuLi.NotOpen)
        self.GetButtonInteractorHandler.Interactable = false
    end



    
    self:FlushRewardItemList(self.reward_item)


    -- 变化记录领取数组的列表
    self.data_instance.LastFetchRewardTimes[self.seq] = self.fetch_reward_times
end


function ActDuanYangFuLiItem:FlushRewardItemList(reward_item)
    local reward_item_list = self.data_instance:GetRewardItemList(reward_item)

    self.RewardItemList:SetData(reward_item_list)
end


function ActDuanYangFuLiItem:OnClickBuy()
    self.data_instance.currentMoney = self.price/10
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    if self.data_instance:IsBuyEvenOnlyOne() == false then
        PublicPopupCtrl.Instance:AlertTip(Language.DuanYangFuLi.CanNotDiscount,function ()
            ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_DUANYANGFULI, self.data_instance.currentMoney, ACTIVITY_TYPE.RAND_DUANYANGFULI, ActDuanYangFuLiCtrl.REQ_TYPE.REQ_BUY,self.seq,0)
            -- ActDuanYangFuLiCtrl.Instance:SendDuanYangFuLiReq(ActDuanYangFuLiCtrl.REQ_TYPE.REQ_BUY, self.seq,0)
        end)
    else
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_DUANYANGFULI, self.data_instance.currentMoney, ACTIVITY_TYPE.RAND_DUANYANGFULI, ActDuanYangFuLiCtrl.REQ_TYPE.REQ_BUY,self.seq,0)
    -- ActDuanYangFuLiCtrl.Instance:SendDuanYangFuLiReq(ActDuanYangFuLiCtrl.REQ_TYPE.REQ_BUY, self.seq,0)
        -- AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    end
end


function ActDuanYangFuLiItem:OnClickGet()
    if self.data_instance.actOpenDay < self.activity_times then
        PublicPopupCtrl.Instance:Center(self.is_buy and Language.DuanYangFuLi.NotReady or Language.DuanYangFuLi.NotOpenBuy)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        return
    end
    ActDuanYangFuLiCtrl.Instance:SendDuanYangFuLiReq(ActDuanYangFuLiCtrl.REQ_TYPE.REQ_GET_REWARD, self.seq)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end


DuanYangFuLiRewardView = DuanYangFuLiRewardView or DeclareView("DuanYangFuLiRewardView", "serveractivity/duanyangfuli/duanyangfuli_reward_view")
VIEW_DECLARE_MASK(DuanYangFuLiRewardView, ViewMask.BgBlockClose)
function DuanYangFuLiRewardView:LoadCallback(param_t) 
    local fix_item_list = BagData.Instance:ItemListBuild(param_t.item_list or {})
    -- LogError("fix_item_list",fix_item_list)
    self.panel.list:SetData(fix_item_list)


    if #fix_item_list >= 6 then
        self.ContentRectTransform.pivot = Vector2.New(0,0.5)
    end

    -- self.handle_close = TimeHelper:AddDelayTimer(function ()
    --     ViewMgr:CloseView(DuanYangFuLiRewardView)
    -- end, 5)

    self.handle_wait = TimeHelper:AddDelayTimer(function ()
        for k,v in pairs(fix_item_list) do 

            PublicPopupCtrl.Instance:CenterI({item_id = v.item_id, num = v.num,
            color_str = v:ColorStr(),item = v,icon_id = v:IconId()})
            ChatCtrl.Instance:ChannelChatCustom{item_get = {item = v, num = v.num}}
        end 
    end, 1)
end

function DuanYangFuLiRewardView:CloseCallback()
    -- TimeHelper:CancelTimer(self.handle_close)
    TimeHelper:CancelTimer(self.handle_wait)
end

--===========================DuanYangFuLiRewardPanel===========================
DuanYangFuLiRewardPanel = DuanYangFuLiRewardPanel or DeclareMono("DuanYangFuLiRewardPanel", UIWFlushPanel)
function DuanYangFuLiRewardPanel:DuanYangFuLiRewardPanel() end 

DuanYangFuLiRewardCell = DuanYangFuLiRewardCell or DeclareMono("DuanYangFuLiRewardCell",UIWidgetBaseItem)
function DuanYangFuLiRewardCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data) 

    self.cell:SetData(data)
    UH.SetText(self.name ,data:QuaName(true))
end 