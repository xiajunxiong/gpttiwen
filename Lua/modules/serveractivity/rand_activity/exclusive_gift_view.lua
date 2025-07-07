ExclusiveGiftView = ExclusiveGiftView or DeclareView("ExclusiveGiftView", "serveractivity/exclusive_gift", Mod.RandActivity.ExclusiveGift)

function ExclusiveGiftView:ExclusiveGiftView()
	self.data = ServerActivityData.Instance.exclusive_gift_data
	self.info_change_handle = self.data:Care(BindTool.Bind(self.FlushViewCallback, self), "info")
	self.ht_2 = ServerActivityData.Instance:GetCmdBuyItemCheckRet():Care(BindTool.Bind(self.SendBuy,self))
	self.now_page = 1
end

function ExclusiveGiftView:LoadCallback()
	UH.SetText(self.RmbTxt, Language.Recharge.GoldType.rmb)
	-- 如果打开的时候有红点，需要跳转到最后一页来显示
	if self.data.has_red_point then
		self.now_page = self.data.new_page or #self.data.info
		self.data.has_red_point = false

		if #self.data.lv_info > 0 then
			ServerActivityData.Instance:SetExclusiveGiftKeyValue(self.data.lv_info[#self.data.lv_info].end_time)
		end
		ServerActivityData.Instance:SetExclusiveGiftDayKeyValue()
	end
	self:UpdateViewShow()
	self.show_time = TimeHelper:AddDelayTimer(function()
		self.CellListAnim:SetTrigger("Show")
	end, 0.3)
	if #self.data.info == 1 and ServerActivityData.Instance:IsHasWeedendGift() then
        self.btn_left:SetActive(true)
    end
end

function ExclusiveGiftView:CloseCallback()
	-- 关闭界面的时候无论外面有无红点都会清除
	self.data.has_red_point = false
	self.data:Uncare(self.info_change_handle)
	ServerActivityData.Instance:GetCmdBuyItemCheckRet():Uncare(self.ht_2)
	self.Time:CloseCountDownTime()
	TimeHelper:CancelTimer(self.show_time)
end

function ExclusiveGiftView:OnCloseClick()
	ViewMgr:CloseView(ExclusiveGiftView)
end

function ExclusiveGiftView:FlushViewCallback()
	if #self.data.info == 0 then
	 	ViewMgr:CloseView(ExclusiveGiftView)
	 	return
	end
	if self.now_page > #self.data.info then
		self.now_page = #self.data.info
	end
	self:UpdateViewShow()
end

function ExclusiveGiftView:OnLeftClick()
	if #self.data.info == 1 and ServerActivityData.Instance:IsHasWeedendGift() then
        ViewMgr:OpenViewByKey(Mod.RandActivity.WeekGift)
        ViewMgr:CloseView(ExclusiveGiftView)
        return
    end
	if self.now_page > 0 then
		self.now_page = self.now_page - 1
		self.CellListAnim:SetTrigger("Show")
	end
	self:UpdateViewShow()
end

function ExclusiveGiftView:OnRightClick()
	if #self.data.info > self.now_page then
		self.now_page = self.now_page + 1
		self.CellListAnim:SetTrigger("Show")
	end
	self:UpdateViewShow()
end

function ExclusiveGiftView:JudgeLRActive()
	self.btn_left:SetActive(self.now_page ~= 1)
	self.btn_right:SetActive(self.now_page ~= #self.data.info)
end

function ExclusiveGiftView:UpdateViewShow()
	self:JudgeLRActive()
	-- 增加容错
	if not self.data.info[self.now_page] then
		ViewMgr:CloseView(ExclusiveGiftView)
		return
	end
	local info = self.data.info[self.now_page]
	local t_type = self.data.info[self.now_page].type
	if info.gift_type == ServeractivityConfig.ExclusiveGiftType.TYPE_DEFAULT then
		self.config = ServerActivityData.Instance:GetExclusiveGiftConfigure(t_type)
	else
		self.config = ServerActivityData.Instance:GetExclusiveGiftDayConfigure(info.phase, info.type)
	end
	local item_data = {}
	for i = 1, 3 do
		item_data[i] = {}
		if self.config.reward_item[i - 1] then
			item_data[i].reward_item = DataHelper.TableCopy(self.config.reward_item[i - 1])
		else
			-- 第三个是元宝
			item_data[i].reward_item = {item_id = CommonItemId.Gold, num = self.config.gold_num, is_bind = 0}
		end
		item_data[i].des = self.config["item_des"..i]
	end
	UH.SetText(self.PriceTxt, self.config.price_des)
	UH.SetText(self.MoneyTxt, self.config.buy_money / 10)
	UH.SetText(self.LimitTxt, string.format(Language.ExclusiveGift.LimitStr, self.config.limit_num - self.data.info[self.now_page].buy_times))
	-- if self.data.info[self.now_page].gift_type == ServeractivityConfig.ExclusiveGiftType.TYPE_DEFAULT then
	self.Time.gameObject:SetActive(true)
	self.Time:StampTime(self.data.info[self.now_page].end_time, TimeType.Type_Special_9)
	-- elseif self.data.info[self.now_page].gift_type == ServeractivityConfig.ExclusiveGiftType.TYPE_DAY_GIFT then
	-- 	self.Time.gameObject:SetActive(false)
	-- end

	for i=1, self.CellList:Length() do
        self.CellList[i]:SetData(item_data[i])
    end
end

function ExclusiveGiftView:OnBuyClick()
	local fee = self.config.buy_money / 10
	if TableIsEmpty(self.data.info[self.now_page]) then
		return
	end
	if self.data.info[self.now_page].gift_type == ServeractivityConfig.ExclusiveGiftType.TYPE_DEFAULT then
		local req_type = ServeractivityConfig.ExclusiveGiftReqType.TYPE_BUY_ITEM
		ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_EXCLUSIVE_GIFT_NEW,fee,ACTIVITY_TYPE.RAND_EXCLUSIVE_GIFT, req_type, self.data.info[self.now_page].type, 0, 0)
	elseif self.data.info[self.now_page].gift_type == ServeractivityConfig.ExclusiveGiftType.TYPE_DAY_GIFT then
		local req_type = ServeractivityConfig.ExclusiveGiftReqType.TYPE_BUY_DAY_ITEM
		ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_EXCLUSIVE_GIFT_DAY,fee,ACTIVITY_TYPE.RAND_EXCLUSIVE_GIFT, req_type, self.data.info[self.now_page].phase, self.data.info[self.now_page].type, 0)
	end
end

function ExclusiveGiftView:SendBuy()
	if TableIsEmpty(self.data.info[self.now_page]) then
		return
	end
	if self.data.info[self.now_page].gift_type == ServeractivityConfig.ExclusiveGiftType.TYPE_DEFAULT then
		ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_EXCLUSIVE_GIFT_NEW, self.config.buy_money)
	elseif self.data.info[self.now_page].gift_type == ServeractivityConfig.ExclusiveGiftType.TYPE_DAY_GIFT then
		ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_EXCLUSIVE_GIFT_DAY, self.config.buy_money)
	end
end

----------------------------ExclusiveGiftItem----------------------------
ExclusiveGiftItem = ExclusiveGiftItem or DeclareMono("ExclusiveGiftItem", UIWidgetBaseItem)

function ExclusiveGiftItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local item_data = Item.New(data.reward_item)
	self.Cell:SetData(item_data)
	UH.SetText(self.NameTxt, Item.GetQuaName(data.reward_item.item_id, true))
	UH.SetText(self.DescriptionTxt, data.des)
end


--================================ExclusiveGiftMenuPanel================================
ExclusiveGiftMenuPanel = ExclusiveGiftMenuPanel or DeclareMono("ExclusiveGiftMenuPanel", UIWFlushPanel)

function ExclusiveGiftMenuPanel:ExclusiveGiftMenuPanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.week_gift_data, func = self.OnFlush, init_call = false},
        {data = self.data.exclusive_gift_data, func = self.OnFlush, init_call = false,keys = {"info"}},
        {data = SceneData.Instance:Base(), func = self.OnChange, init_call = true, keys = {"scene_mode"}},
        {data = self.data.exclusive_gift_data, func = self.OnRemind, init_call = false, keys = {"has_red_point"}},
        {data = ActivityData.Instance.activity_status_event, func = self.OnFlush,init_call = false, keys = {"act_type", "status"}},
    }
end

function ExclusiveGiftMenuPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ShowTimer)
end

function ExclusiveGiftMenuPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:OnFlush()
    self:OnChange()
end

function ExclusiveGiftMenuPanel:OnChange()
    TimeHelper:CancelTimer(self.ShowTimer)
    self.Anim:SetBool([[state]],SceneData.Instance:GetMode() == SceneModeType.None)
end

function ExclusiveGiftMenuPanel:OnRemind()
	if self.IsExclusiveGift then
		self.RedPoint:SetNum(self.data.exclusive_gift_data.has_red_point and 1 or 0)
	else
		self.RedPoint:SetNum(0)
	end
end

function ExclusiveGiftMenuPanel:OnFlush()
    if IS_AUDIT_VERSION then
        self.Panel:SetActive(false)
        return
    end
	local info,max_num = self.data:GetGiftInfoList()
    if info then
        self.Panel:SetActive(true)
        self:FlushItemView(info,max_num)
    else
        TimeHelper:CancelTimer(self.ShowTimer)
        self.Panel:SetActive(false)
        self:FlushDelteView()
    end
end

function ExclusiveGiftMenuPanel:FlushItemView(info,max_num)
    self.Time:CloseCountDownTime()
    self.Time:StampTime(info.end_time, TimeType.Type_Special_9)
    local title_name = info.title_name or Language.WeekGift.Title[0]
    UH.SetText(self.Name[1],title_name)
    UH.SetText(self.Name[2],title_name)
    UH.SetText(self.Num, max_num)
	self.IsExclusiveGift = info.title_name == nil
	self.Time:SetCallBack(function()
        if self.IsExclusiveGift then
            self.data:ExclusiveGiftTimeCheck()
        end
		self:OnFlush()
    end)
    self:OnRemind()
end

function ExclusiveGiftMenuPanel:FlushDelteView()
    self.data.exclusive_gift_data.has_red_point = false
    self.data:SetExclusiveGiftKeyValue("")
end

function ExclusiveGiftMenuPanel:OnClickGift()
    if self.Anim:GetBool([[state]]) or self.data:IsExclusiveGiftDayType() then
        if SceneData.Instance:GetMode() ~= SceneModeType.None then
            self.Anim:SetBool([[state]], false)
        end
        self:OnOpenGiftView()
    else
        self.Anim:SetBool([[state]], true)
        TimeHelper:CancelTimer(self.ShowTimer)
        self.ShowTimer = Invoke(function()
            self.Anim:SetBool([[state]], false)
        end,30)
    end
end

function ExclusiveGiftMenuPanel:OnOpenGiftView()
	if self.IsExclusiveGift then
		ViewMgr:OpenViewByKey(Mod.RandActivity.ExclusiveGift)
	else
		ViewMgr:OpenViewByKey(Mod.RandActivity.WeekGift)
	end
end