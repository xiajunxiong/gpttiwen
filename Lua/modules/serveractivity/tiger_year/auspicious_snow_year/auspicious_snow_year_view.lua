AuspiciousSnowYearView = AuspiciousSnowYearView or DeclareView("AuspiciousSnowYearView", "serveractivity/tiger_year/auspicious_snow_year", Mod.TigerYear.AuspiciousSnow)
VIEW_DECLARE_LEVEL(AuspiciousSnowYearView, ViewLevel.L0)
function AuspiciousSnowYearView:AuspiciousSnowYearView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_AUSPICIOUS_SNOW_YEAR, TimeType.Type_Time_8,Language.AuspiciousSnowYear.ActTime)
end

function AuspiciousSnowYearView:OnClickClose()
    AuspiciousSnowYearData.Instance:FlushShowList()
    BagData.Instance:SetHideCurrencyCenterFlag(false)
    ViewMgr:CloseView(AuspiciousSnowYearView)
end

function AuspiciousSnowYearView:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[223].desc})
end

--------------------AuspiciousSnowYearPanel--------------------
AuspiciousSnowYearPanel = AuspiciousSnowYearPanel or DeclareMono("AuspiciousSnowYearPanel", UIWFlushPanel)
function AuspiciousSnowYearPanel:AuspiciousSnowYearPanel()
    self.Data = AuspiciousSnowYearData.Instance
    self.data = self.Data.auspicious_snow_data
    self.data_cares = {
        {data = self.data, func = self.FlushShow,init_call = false},
        {data = self.data, func = self.PlayLuckDraw,init_call = false,keys ={"draw_result"}},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Material), func = self.FlushShowList, init_call = true},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushShow, init_call = true},
    }   
    -- 是否在播放旋转动画中
    self.is_rotating = false
    -- 用于记录当前转到哪
    self.item_index = 0
    -- 是否可以点击
    self.is_can_click = false
end

function AuspiciousSnowYearPanel:Awake()
    UIWFlushPanel.Awake(self)
    for i,v in pairs(self.Data:GetRewardData()) do
        self.List[i]:SetData(v)
    end
    --self:FlushShowList()
    --self:FlushShow()
    BagData.Instance:SetHideCurrencyCenterFlag(true)
end

function AuspiciousSnowYearPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.time_handle then
        TimeHelper:CancelTimer(self.time_handle)
        self.time_handle = nil
    end
    if self.time_handle2 then
        TimeHelper:CancelTimer(self.time_handle2)
        self.time_handle2 = nil
    end
    if self.item_timer then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
        self.item_timer = nil
    end
end

function AuspiciousSnowYearPanel:FlushShowList()
    if self.is_rotating and not self.is_received then 
        return
    end
    self.Data:FlushShowList()
    if self.is_received then  --领取飘字
        self.is_received = false
        for k, v in pairs(self.Data:GetShowItemCfg()) do
            local item = Item.Create(v)
            PublicPopupCtrl.Instance:CenterI({item_id = item.item_id, num = item.num, color_str = item:ColorStr()})
        end
    end
end

function AuspiciousSnowYearPanel:FlushShow()
    self.ShowItem:SetData(Item.New(self.Data:GetShowItemCfg()[0]))
    self.ShowList:SetData(self.Data.show_list)
    local num = Item.GetNum(self.Data:GetOtherData().lucky_draw_item_id)
    UH.SetText(self.DrawTimes,string.format(Language.AuspiciousSnowYear.DrawTimes,num))
    self.BtnDrawInter.Interactable = num > 0
    self.ReceiveInter.Interactable = self.data.is_receive
    self.DrawEffect:SetActive(num > 0)
    self.ReceEffect:SetActive(self.data.is_receive)
    self.RedPos:SetNum(self.data.times)
end

function AuspiciousSnowYearPanel:PlayLuckDraw()
    if self.data.draw_result >= 0 then
        self:PlayDialEffect(self.data.draw_result + 1)
    end
end

-- 转盘动效，index转到哪个item上
function AuspiciousSnowYearPanel:PlayDialEffect(index)
    self.speed = 0
    self.start_speed = 0            -- 开始速度
    self.min_speed = 0.08           -- 最小速度
    self.max_speed = 0.2              -- 最大速度
    self.accelerated_speed = 0.02   -- 加速度
    self.process_time = 3           -- 过程时间
    self.start_time = Time.time
    self.is_stop = false
    self.ramdon_right = 10
    self.ramdon_left = 1
    self.confirm_num = 4 --几步确认
    self.confirm_index = 0 
    if self.item_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
    end
    self.item_timer = Runner.Instance:RunUntilTrue(BindTool.Bind(self.DialEffect,self,index))
end

-- 转动中
function AuspiciousSnowYearPanel:DialEffect(index)
    -- 减速处理
    if Time.time - self.start_time >= self.process_time and not self.is_stop and self.accelerated_speed > 0 then
        self.accelerated_speed = -self.accelerated_speed
    end
    if not self.is_stop then
        self.speed = self.speed + self.accelerated_speed
        if self.accelerated_speed < 0 then
            -- 减到最小速度后，开始锁定目标吧
            if self.speed <= self.min_speed then
                self.speed = self.min_speed
                self.is_stop = true
                local mid = self.ramdon_right/2
                self.ramdon_right = index > mid and self.ramdon_right or mid
                self.ramdon_left =  index > mid and mid or self.ramdon_left
            end
        else
            self.speed = self.speed > self.max_speed and self.max_speed or self.speed
        end
    end
    self.start_speed = self.start_speed + self.speed
    if self.start_speed >= 1 then
        -- 转盘停止
        if self.is_stop == true and self.confirm_index > self.confirm_num then
            self.item_index = index
            self:DialEffectEndFunc()
            return true
        end
        self.last_index = self.item_index
        while self.item_index == self.last_index do
            self.item_index = GameMath.Ramdon(self.ramdon_left,self.ramdon_right)
        end
        self.start_speed = 0
        if self.is_stop then
            self.confirm_index = self.confirm_index + 1
        end
        self.List[self.item_index]:SetEffectActive(true)
    end
end

-- 转动结束
function AuspiciousSnowYearPanel:DialEffectEndFunc()
    local item = self.List[self.item_index]
    local data = item:GetData()
    item:SetEffectActive(false)
    TimeHelper:CancelTimer(self.time_handle2)
    -- 这里延迟0.5秒停下
    self.time_handle2 = TimeHelper:AddDelayTimer(function ()
        item:SetEffectActive(false)
    end, 0.5)
    
    TimeHelper:CancelTimer(self.time_handle)
    -- 这里延迟0.8秒展示
    self.time_handle = TimeHelper:AddDelayTimer(function ()
        self.is_rotating = false
        --展示结果
        ActRewardTipData.Instance:OpenActRewardTipViewByData(ServeractivityConfig.ActRewerdTip.RewardType.Success,{self.Data:GetLuckDrawData().reward_item})
        --刷新显示
        self:FlushShowList()
    end,0.8)
end

function AuspiciousSnowYearPanel:OnClickReceive()
    AuspiciousSnowYearCtrl.Instance:Send(2)
    self.is_received = true
end

function AuspiciousSnowYearPanel:OnClickLuckDraw()
    if self.is_rotating then 
        PublicPopupCtrl.Instance:Center(Language.AuspiciousSnowYear.LuckDrawTip)
        return
    end
    if Item.GetNum(self.Data:GetOtherData().lucky_draw_item_id) <= 0 then
        PublicPopupCtrl.Instance:Center(Language.AuspiciousSnowYear.ItemNotEnought, 1, 1)
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.Data:GetOtherData().lucky_draw_item_id})})
        return
    end    
    self.is_rotating = true
    AuspiciousSnowYearCtrl.Instance:Send(1)
end

function AuspiciousSnowYearPanel:OnClickGetSnowBall()
    ViewMgr:OpenView(AuspiciousSnowYearTipView)
end

function AuspiciousSnowYearPanel:OnClickCloseTip()
    self.GoSceneTip:SetActive(false)
end
-------------------AuspiciousSnowYearItem-------------------
AuspiciousSnowYearItem = AuspiciousSnowYearItem or DeclareMono("AuspiciousSnowYearItem", UIWidgetBaseItem)

function AuspiciousSnowYearItem:SetData(data)    
    UIWidgetBaseItem.SetData(self,data)
    UH.SetIcon(self.IconSp,Item.GetIconId(data.reward_item.item_id))
    UH.SetText(self.Num,data.reward_item.num)
end
function AuspiciousSnowYearItem:SetEffectActive(value)
    if self.SelObj then
        self.SelObj:SetActive(true)
    end   
    self.effect_time = TimeHelper:AddDelayTimer(function()
        TimeHelper:CancelTimer(self.effect_time)
        if self.SelObj then
            self.SelObj:SetActive(false)
        end         
    end,0.5)
end

function AuspiciousSnowYearItem:OnClickItem()
    CellClicks.BagGridNoButton({GetData = function()
        return Item.New(self.data.reward_item)
    end})
end

function AuspiciousSnowYearItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    self.SelObj = nil
    if self.effect_time then
        TimeHelper:CancelTimer(self.effect_time)
        self.effect_time = nil
    end
end

AuspiciousSnowRewardItem = AuspiciousSnowRewardItem or DeclareMono("AuspiciousSnowRewardItem", UIWidgetBaseItem)

function AuspiciousSnowRewardItem:SetData(data)
    data.item = Item.New(data)
    self.Cell:SetData(data.item)
    self.Cell:MakeGrey(not data.is_has)
    UIWidgetBaseItem.SetData(self, data)
end

AuspiciousSnowYearTipView = AuspiciousSnowYearTipView or DeclareView("AuspiciousSnowYearTipView", "serveractivity/tiger_year/auspicious_snow_year_tip")
function AuspiciousSnowYearTipView:AuspiciousSnowYearTipView()
    UH.SetText(self.Times,string.format(Language.AuspiciousSnowYear.Times,AuspiciousSnowYearData.Instance.auspicious_snow_data.times))
end
function AuspiciousSnowYearTipView:OnClickClose()
    ViewMgr:CloseView(AuspiciousSnowYearTipView)
end

function AuspiciousSnowYearTipView:OnClickGoMap()
    if AuspiciousSnowYearData.Instance.auspicious_snow_data.times <= 0 then
        PublicPopupCtrl.Instance:Center(Language.AuspiciousSnowYear.GoToTip)
        return
    end
    ViewMgr:OpenView(MapView,{need_click = false})
    ViewMgr:CloseView(AuspiciousSnowYearTipView)
    ViewMgr:CloseView(AuspiciousSnowYearView)
    ViewMgr:CloseView(TigerYearTotalView)

    TigerYearTotalCtrl.Instance:AutoCloseTotal(Mod.TigerYear.AuspiciousSnow)
end