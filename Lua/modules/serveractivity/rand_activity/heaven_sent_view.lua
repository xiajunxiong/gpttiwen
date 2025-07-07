HeavenSentView = HeavenSentView or DeclareView("HeavenSentView","serveractivity/heaven_sent_view",Mod.RandActivity.HeavenSent)

VIEW_DECLARE_LEVEL(HeavenSentView, ViewLevel.L1)

function HeavenSentView:HeavenSentView()
    self.page = 1
    self.data = ServerActivityData.Instance
    self.care_handle = self.data.heaven_sent_data:Care(BindTool.Bind(self.FlusnItemView,self))
end

function HeavenSentView:CloseCallback()
	TimeHelper:CancelTimer(self.ShowTimer)
	self.data.heaven_sent_data:Uncare(self.care_handle)
	self.care_handle = nil
end

function HeavenSentView:LoadCallback()
    self.toggle_list = self.data:GetHeavenSentList()
    if #self.toggle_list > 1 then
        self.ToggleList:SetDataList(self.toggle_list,self:GetIndex() or 1)
    else
        self.ToggleList:SetData(self.toggle_list)
        self:OnClickItem(self.toggle_list[1])
    end
    self.ToggleList:SetObjActive(#self.toggle_list > 1)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_HEAVEN_SENT,TimeType.Type_Special_4)

    local type = Config.lingpai_divine_auto.other[1].type
    UH.SpriteName(self.ImgSet,"_LocJiSong" .. type)
end

function HeavenSentView:GetIndex()
    for i,v in ipairs(self.toggle_list) do
        if self.data:GetHeavenSentFlag(v.seq) == 0 then
            return i
        end
    end
end

function HeavenSentView:FlusnItemView()
    local data = self.toggle_list[self.page]
    if data == nil then 
        return
    end
    local target_num,Tips = data.acc_price,Language.HeavenSent.Tips
    local recharge_num = self.data:GetHeavenSentRechargeNum()
    local post,list = DataHelper.GetValueLength(target_num)

    for i=1,self.NumList:Length() do
        if list[i] ~= nil then
            UH.SpriteName(self.NumList[i],list[i])
        end
        self.NumList[i]:SetObjActive(i<=post)
    end

    local item_list = DataHelper.FormatItemList(data.reward_item,1)
    for i=1,self.ItemList:Length() do
        self.ItemList[i]:SetData(item_list[i])
    end

    UH.SetText(self.Desc,Format(Tips[1],recharge_num))
    self.Progress:SetProgress(Valve(recharge_num,target_num) / target_num)
    self.Progress:SetText(Format("%s/%s",recharge_num,target_num))

    local flag = self.data:GetHeavenSentFlag(data.seq)
    local can_reward = flag == 0 and recharge_num >= target_num

    self.Call:SetData(Item.New(data.gift_item))
    self.Effect:SetActive(can_reward)
    self.BtnInter.Interactable = flag == 0

    UH.SetText(self.BtnName,Language.HeavenSent.BtnName[flag == 0 and (recharge_num >= target_num and 2 or 1) or 3])
    UH.SetText(self.Tips,Format(Tips[can_reward and 3 or 2],target_num - recharge_num))

    if flag == 1 then
        UH.SetText(self.Desc,"")
        UH.SetText(self.Tips,Format(Tips[1],recharge_num))
    end
    self.select_data = data
end

function HeavenSentView:FlushBtnView()
    self.BtnLeft:SetActive(self.page > 1)
    self.BtnRight:SetActive(self.page < #self.toggle_list)
    if self.effect_handle then
        self.EffectTool:Stop(self.effect_handle)
    end
    self.effect_handle = self.EffectTool:Play(6165003)
end

function HeavenSentView:OnClickItem(data)
    self.page = data.page or 1
    self:FlusnItemView()
    self:FlushBtnView()
    self.PanelAnim:Stop()
    self.PanelAnim:Play()
    self.BoxEffect:SetActive(false)
    TimeHelper:CancelTimer(self.ShowTimer)
    self.ShowTimer = Invoke(function()
        self.BoxEffect:SetActive(true)
    end)
end

function HeavenSentView:OnClickPage(num)
    self.page = self.page + num
    self.ToggleList:SetDataList(self.toggle_list,self.page)
end

function HeavenSentView:OnClickRecharge()
    if self.Effect.activeSelf then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_HEAVEN_SENT,1,self.select_data.seq)
    else
        ViewMgr:OpenViewByKey(Mod.Store.Recharge)
    end
end

function HeavenSentView:OnClickClose()
    ViewMgr:CloseView(HeavenSentView)
end

----------------------------HeavenSentItem----------------------------
HeavenSentItem = HeavenSentItem or DeclareMono("HeavenSentItem", UIWidgetBaseItem)

function HeavenSentItem:SetData(data)
    data.page = data.seq + 1
    UIWidgetBaseItem.SetData(self, data)
end

function HeavenSentItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end