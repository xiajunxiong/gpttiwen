GiftRotationView = GiftRotationView or DeclareView("GiftRotationView", "serveractivity/gift_rotation", Mod.Splendid.GiftRotation)
VIEW_DECLARE_LEVEL(GiftRotationView, ViewLevel.L1)
VIEW_DECLARE_MASK(GiftRotationView, ViewMask.BgBlock)

function GiftRotationView:LoadCallback()
	self.Time:CreateActTime(ACTIVITY_TYPE.RAND_GIFT_ROTATION, TimeType.Type_Special_4)
	self.Time:SetCallBack(function()
		PublicPopupCtrl.Instance:Center(Language.GiftRotation.ActivityCloseTip)
		ViewMgr:CloseView(GiftRotationView)
	end)
end

function GiftRotationView:CloseCallback()
    self.Time:CloseCountDownTime()
    ViewMgr:CloseView(ShowItemView)
end

function GiftRotationView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[107].desc)
end


---------------------------------------------
--------------GiftRotationPanel--------------
---------------------------------------------
GiftRotationPanel = GiftRotationPanel or DeclareMono("GiftRotationPanel", UIWFlushPanel)
function GiftRotationPanel:GiftRotationPanel()
    self.config = Config.gift_rotation_auto
    self.other = self.config.other[1]
    self.data = ServerActivityData.Instance.gift_rotation_data
    self.item_data = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_GIFT_ROTATION, self.config.gift, "seq")
    self.data_cares = {
        {data = self.data, func = self.ReceivedGift, keys = {"now_get"}, init_call = false},
        {data = self.data, func = self.FlushDownShow, keys = {"fetch_flags"}, init_call = false},
    }
    for i = 1, self.List:Length() do
        if self.item_data[i] then
            self.List[i]:SetData(self.item_data[i])
        end
    end
    -- 是否在播放旋转动画中
    self.is_rotating = false
    -- 用于转动显示的id
    self.can_rotate_id = {}
    -- 用于记录当前转到哪
    self.item_index = 0
    -- 是否可以点击
    self.is_can_click = false
    -- 将收到外观道具id置为0
    self.data.hear_item = 0

    TimeHelper:CancelTimer(self.time_can_click)
    self.time_can_click = TimeHelper:AddDelayTimer(function()
        self.is_can_click = true
    end, 1.5)
    self:FlushDownShow()
end

function GiftRotationPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_handle)
    TimeHelper:CancelTimer(self.time_handle2)
    TimeHelper:CancelTimer(self.time_can_click)
    if self.item_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
        self.item_timer = nil
    end
    self:ClearShield()
end

function GiftRotationPanel:OnCloseClick()
    if not self.is_rotating then
        ViewMgr:CloseView(GiftRotationView)
    end
end

-- 点击领取
function GiftRotationPanel:OnReceiveClick()
    if not self.is_can_click then
        return
    end
    if not self.is_rotating and self.data.fetch_num > 0 then
        self.is_rotating = true
        self.item_index = 0
        self.can_rotate_id = self:GetNotReceiveList()
        SocietyData.Instance.is_mail_shield_tip = true
        AppearanceCtrl.Instance.is_shield_appear_show = true
        RoleCtrl.Instance.is_shield_title_show = true
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_GIFT_ROTATION, 1)
    end
end

-- 收到了领取道具的seq1的回调，收到才去执行转动
function GiftRotationPanel:ReceivedGift()
    self:FlushDownShow()
    if self.data.now_get.now_get_seq1 then
        self:PlayDialEffect(self.data.now_get.now_get_seq1 + 1)
    else
        self.is_rotating = false
    end
end

-- 展示礼包道具
function GiftRotationPanel:ShowItem(mono)
    if not self.is_can_click then
        return
    end
    local tab = {
        pos = UH.GetRootPosition(mono.ObjShowPos),
        item_list = DataHelper.FormatItemList(mono.data.reward),
    }
    ViewMgr:OpenView(ShowItemView, tab)
end

-- 刷新下方可领取天数和按钮和红点
function GiftRotationPanel:FlushDownShow()
    -- 如果礼包都领取完了就不显示按钮和可领取天数
    local is_show = false
    for i = 1, self.List:Length() do
        if not ServerActivityData.Instance:GetGiftRotationIsReceive(i - 1) then
            is_show = true
            break
        end
    end
    -- 如果礼包没领取完，可领取次数为0，但是已经是活动最后一天了，也不显示
    if is_show then
        local activity_info = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_GIFT_ROTATION)
        if activity_info ~= nil and activity_info.status ~= ActStatusType.Close then
            local total_time = activity_info.next_status_switch_time - TimeHelper.GetServerTime()
            if total_time < 86400 and self.data.fetch_num == 0 then
                is_show = false
            end
        end
    end
    self.TxtLimit:SetObjActive(is_show)
    self.ObjReceive:SetActive(is_show)
    UH.SetText(self.TxtLimit, self.data.fetch_num)
    UH.SetText(self.TxtReceive, self.data.fetch_num > 0 and Language.GiftRotation.GiftRotationClick or Language.GiftRotation.GiftRotationTomorrow)
    self.BtnHandler.Interactable = self.data.fetch_num > 0
    self.RedPoint:SetNum(self.data.fetch_num > 0 and 1 or 0)
end

-- 获取没有领取的id
function GiftRotationPanel:GetNotReceiveList()
    local tab = {}
    for i, v in ipairs(self.item_data) do
        if not ServerActivityData.Instance:GetGiftRotationIsReceive(v.seq1) then
            tab[#tab + 1] = i
        end
    end
    return tab
end

-- 转盘动效，index转到哪个item上
function GiftRotationPanel:PlayDialEffect(index)
    if not next(self.can_rotate_id) then
        self.is_rotating = false
        return
    end
    if #self.can_rotate_id == 1 then
        self.item_index = 1
        self:DialEffectEndFunc()
        return
    end
    self.speed = 0
    self.start_speed = 0            -- 开始速度
    self.min_speed = 0.08            -- 最小速度
    self.max_speed = 0.2              -- 最大速度
    self.accelerated_speed = 0.02   -- 加速度
    self.process_time = 3           -- 过程时间
    self.start_time = Time.time
    self.is_stop = false
    if self.item_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
    end
    self.item_timer = Runner.Instance:RunUntilTrue(BindTool.Bind(self.DialEffect,self,index))
end

-- 转动中
function GiftRotationPanel:DialEffect(index)
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
            end
        else
            self.speed = self.speed > self.max_speed and self.max_speed or self.speed
        end
    end
    self.start_speed = self.start_speed + self.speed
    if self.start_speed >= 1 then
        self.item_index =  self.item_index % #self.can_rotate_id + 1
        self.List[self.can_rotate_id[self.item_index]]:SetSelected(true)
        self.start_speed = 0
        -- 转盘停止
        if self.is_stop == true and self.can_rotate_id[self.item_index]== index then
            self:DialEffectEndFunc()
            return true
        end
    end
end

-- 转动结束
function GiftRotationPanel:DialEffectEndFunc()
    local item = self.List[self.can_rotate_id[self.item_index]]
    local data = item:GetData()
    
    TimeHelper:CancelTimer(self.time_handle2)
    -- 这里延迟0.5秒停下
    self.time_handle2 = TimeHelper:AddDelayTimer(function ()
        item:SetSelected(false)
        item:SetEffectActive(true)
    end, 0.5)
    
    TimeHelper:CancelTimer(self.time_handle)
    -- 这里延迟1.5秒展示
    self.time_handle = TimeHelper:AddDelayTimer(function ()
        self.is_rotating = false
        item:UpdateShow()
        MainOtherCtrl.Instance:OpenGiftView(DataHelper.FormatItemList(data.reward))
        self:ClearShield()
        -- 颜值物品激活信息提示
        if self.data.hear_item ~= 0 then
            local str = Language.Common.ActiveStr[self.data.op_type + 1] or Language.Common.ActiveStr[1]
            PublicPopupCtrl.Instance:Center(Format(str, Item.GetQuaName(self.data.hear_item)))
        end
        self.data.hear_item = 0
    end, 1.5)
end

-- 取消拦截：邮件提示，外观激活窗，头衔激活窗
function GiftRotationPanel:ClearShield()
    SocietyData.Instance.is_mail_shield_tip = false
    if SocietyData.Instance.mail_shield_tip_list and next(SocietyData.Instance.mail_shield_tip_list) then
        SocietyData.Instance:ReleaseMailShieldTip()
    end
    AppearanceCtrl.Instance:ReleaseShieldParam()
    RoleCtrl.Instance:ReleaseShieldTitle()
end


--------------------------------------------
--------------GiftRotationItem--------------
--------------------------------------------
GiftRotationItem = GiftRotationItem or DeclareMono("GiftRotationItem", UIWidgetBaseItem)
function GiftRotationItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:UpdateShow()
end

function GiftRotationItem:UpdateShow()
    UH.SetText(self.TxtName, self.data.gift_des)
    UH.SpriteName(self.BoxImageShow, "box_" .. self.data.gift_icon)
    UH.SpriteName(self.BoxImageHide, "box_" .. self.data.gift_icon)
    self.BoxImageShow:SetObjActive(not ServerActivityData.Instance:GetGiftRotationIsReceive(self.data.seq1))
    self.BoxImageHide:SetObjActive(ServerActivityData.Instance:GetGiftRotationIsReceive(self.data.seq1))
end

function GiftRotationItem:SetEffectActive(value)
    self.ObjEffect:SetActive(value)
    if value then
        TimeHelper:CancelTimer(self.effect_time)
        self.effect_time = TimeHelper:AddDelayTimer(function()
            TimeHelper:CancelTimer(self.effect_time)
            self.ObjEffect:SetActive(false)
        end, 3)
    end
end

function GiftRotationItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    TimeHelper:CancelTimer(self.effect_time)
end


----------------------------------------------------------------------------
--------------------------------ShowItemView--------------------------------
----------------------------------------------------------------------------
ShowItemView = ShowItemView or DeclareView("ShowItemView", "serveractivity/show_item_view")

VIEW_DECLARE_MASK(ShowItemView, ViewMask.BlockClose)
-- param_t.pivot == 0 pivot
function ShowItemView:LoadCallback(param_t)
    UH.LocalPosG(self.ObjList, param_t.pos or Vector3.zero)
    self.ShowList:SetData(param_t.item_list)
    if param_t.pivot then
        self.RectTrans.pivot = Vector2.New(param_t.pivot,0.5)
    end
end