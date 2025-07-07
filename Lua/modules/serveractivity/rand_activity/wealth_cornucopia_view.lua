WealthCornucopiaView = WealthCornucopiaView or DeclareView("WealthCornucopiaView","serveractivity/wealth_cornucopia_view",Mod.Splendid.WealthCornucopia)
VIEW_DECLARE_LEVEL(WealthCornucopiaView, ViewLevel.L1)
function WealthCornucopiaView:WealthCornucopiaView()
	self.data = ServerActivityData.Instance
    self.flush_ht = self.data.wealth_cornucopia_info:Care(BindTool.Bind(self.FlushPanel,self))
    self.buy_ht = ServerActivityData.Instance:GetCmdBuyItemCheckRet():Care(BindTool.Bind(self.SendBuy,self))
    self.now_click_config = nil
end

function WealthCornucopiaView:LoadCallback(param_t)
	UH.SetText(self.Title,ActivityRandData.GetName(ACTIVITY_TYPE.RAND_WEALTH_CORNUCOPIA))
    self:SetScaleNumImgs()
	self:FlushPanel()
    self.delay_list_anim = TimeHelper:AddDelayFrameTimer(function ()
        self.ListAnim:Play()
    end,15)
    self.Timer:CreateActTime(ACTIVITY_TYPE.RAND_WEALTH_CORNUCOPIA, TimeType.Type_Time_3, Language.WealthCornucopia.Txt2)
end

function WealthCornucopiaView:FlushPanel()
    local list_data = self.data:GetWealthCornucopiaListByDayNum()
    self.List:SetData(list_data or {})
end

function WealthCornucopiaView:SetScaleNumImgs(num)
    num = num or math.floor(self.data:GetWealthCornucopiaPercentage())
    local nums = {[1] = 0}
    local img_len = self.ScaleImgs:Length()
    for i = 1, img_len do
        if num == 0 then
            break
        end
        nums[i] = num % 10
        num = (num - nums[i]) / 10
    end
    local is_per = false
    for i = 1 , img_len do
        local va = nums[#nums - i + 1]
        if va then
            local img_name = string.format("%s",va)
            UH.SpriteName(self.ScaleImgs[i],img_name)
        elseif is_per == false then
            UH.SpriteName(self.ScaleImgs[i],"%")
            is_per = true
        else
            UH.SpriteName(self.ScaleImgs[i],"")
        end
    end
end

function WealthCornucopiaView:CloseCallback()
    if self.delay_list_anim ~= nil then
        TimeHelper:CancelTimer(self.delay_list_anim)
        self.delay_list_anim = nil
    end
    self.data.wealth_cornucopia_info:Uncare(self.flush_ht)
    ServerActivityData.Instance:GetCmdBuyItemCheckRet():Uncare(self.buy_ht)
end

function WealthCornucopiaView:OnCloseClick()
	ViewMgr:CloseView(WealthCornucopiaView)
end

function WealthCornucopiaView:OnBuyClick(data)
    if data.is_can_buy == 1 then
        self.now_click_config = data
        local fee = self.now_click_config.buy_money / 10
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_WEALTH_CORNUCOPIA_NEW,fee,ACTIVITY_TYPE.RAND_WEALTH_CORNUCOPIA, 1, self.now_click_config.seq, 0, 0)
    end
end

function WealthCornucopiaView:SendBuy()
    if self.now_click_config then
        ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_WEALTH_CORNUCOPIA_NEW, self.now_click_config.buy_money)
        self.now_click_config = nil
        ViewMgr:CloseView(WealthCornucopiaView)
    end
end

WealthCornucopiaItem = WealthCornucopiaItem or DeclareMono("WealthCornucopiaItem",UIWidgetBaseItem)

function WealthCornucopiaItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
    self.RootObj.name = "ItemParent"..data.seq + 1
    local limit_type_str
    local limit_num = 0
    local buy_num = 0
    if data.limit_type == 1 then  -- 1 是每日限购
    	limit_num = data.buy_times
    	buy_num = ServerActivityData.Instance.wealth_cornucopia_info.buy_sum_times[data.seq] or limit_num
        limit_type_str = Language.WealthCornucopia.Limit1
    else
    	limit_num = data.today_times
    	buy_num = ServerActivityData.Instance.wealth_cornucopia_info.buy_day_times[data.seq] or limit_num
        limit_type_str = Language.WealthCornucopia.Limit2
    end
    local limit_count_str = ColorStr(string.format("%s/%s",(limit_num - buy_num),limit_num),COLORSTR.Green2)
    if buy_num >= limit_num then
        limit_count_str = ColorStr(string.format("%s/%s",(limit_num - buy_num),limit_num),COLORSTR.Red7)
    end
    UH.SetText(self.LimitCount,limit_type_str .. limit_count_str)
    UH.SetText(self.SumCount,string.format("%s",(data.gold_num + data.gold_give)))
    UH.SetText(self.FeeNum,string.format("%s",data.buy_money / 10))
    UH.SetText(self.GiveNum,ServerActivityData.Instance:GetWealthCornucopiaPercentage() .. "%")
    UH.SetText(self.GiveNum2,ServerActivityData.Instance:GetWealthCornucopiaPercentage() .. "%")
    self.Interactor.Interactable = data.is_can_buy == 1
    self.FlagObj:SetActive(data.is_can_buy ~= 1)
    UH.SpriteName(self.Icon,string.format("cai_shen_%s",data.gold_icon))
    self:EffectHandle()
    self:SetIconPos()
end

function WealthCornucopiaItem:EffectHandle()
	if self.effect_ht then
		self.EffectTool:Stop(self.effect_ht)
		self.effect_ht = nil
	end
	if self.data.is_can_buy == 1 then
    	if self.data.gold_icon >= 4 and self.data.gold_icon <= 6 then
    		self.EffectObj:SetActive(true)
    		self.effect_ht = self.EffectTool:Play(100108135)
    	elseif self.data.gold_icon >= 7 and self.data.gold_icon <= 8 then
    		self.EffectObj:SetActive(true)
    		self.effect_ht = self.EffectTool:Play(100108136)
    	else
    		self.EffectObj:SetActive(false)
    	end
    else
    	if self.effect_ht then
    		self.EffectTool:Stop(self.effect_ht)
    		self.effect_ht = nil
    	end
    end
end

function WealthCornucopiaItem:SetIconPos()
    local pos = Vector3.New(0,33.5,0)
    if self.data.gold_icon >= 7 and self.data.gold_icon <= 8 then
        pos = Vector3.New(0,29.5,0)
    elseif self.data.gold_icon >= 1 and self.data.gold_icon <= 3 then
        pos = Vector3.New(0,38,0)
    end
    self.Icon.gameObject:SetLocalPosition(pos)
end