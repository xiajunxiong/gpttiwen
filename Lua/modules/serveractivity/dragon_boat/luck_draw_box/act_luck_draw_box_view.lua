-- 端午活动，幸运盲盒
ActLuckDrawBoxView = ActLuckDrawBoxView or DeclareView("ActLuckDrawBoxView", "serveractivity/dragon_boat/luck_draw_box/act_luck_draw_box_view",Mod.RandActivity.LuckBox)
function ActLuckDrawBoxView:OnCloseClick()
    ViewMgr:CloseView(ActLuckDrawBoxView)
end


ActLuckDrawBoxPanel = ActLuckDrawBoxPanel or DeclareMono("ActLuckDrawBoxPanel", UIWFlushPanel)
function ActLuckDrawBoxPanel:ActLuckDrawBoxPanel()
    self.data = ActLuckDrawBoxData.Instance
    self.data_cares = {
        {data = self.data.act_info, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushStuff, init_call = false},
        {data = self.data.remind_data, func = self.FlushRewardRemind, init_call = true}
    }
end

function ActLuckDrawBoxPanel:Awake()
    UIWFlushPanel.Awake(self)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.LUCK_DRAW_BOX,ActLuckDrawBoxCtrl.ReqType.Info)
    self.list_data = {}
    for y = 1,3 do
        for x = 1,4 do
            table.insert(self.list_data, {x = x, y = y})
        end
    end
    self.data.first_remind = true
    self.data:RemindNotice()
    local s_time = ActivityData.GetStartStampTime(ACTIVITY_TYPE.LUCK_DRAW_BOX)
    local e_time = ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.LUCK_DRAW_BOX)

    local s_time_info = TimeManager.FormatUnixTime2Date(s_time)
    local e_time_info = TimeManager.FormatUnixTime2Date(e_time)
    local s_m = ColorStr(s_time_info.month, COLORSTR.Yellow16)
    local s_d = ColorStr(s_time_info.day, COLORSTR.Yellow16)
    local e_m = ColorStr(e_time_info.month, COLORSTR.Yellow16)
    local e_d = ColorStr(e_time_info.day, COLORSTR.Yellow16)
    UH.SetText(self.TimeText, string.format(Language.LuckBox.Time,s_m,s_d,e_m,e_d))
    ActLuckDrawBoxData.Instance.is_playing = false
    self:FlushPanel()
end

function ActLuckDrawBoxPanel:FlushRewardRemind()
    local remind_num = self.data:RewardRemind()
    self.RedPoint:SetNum(remind_num)
end

function ActLuckDrawBoxPanel:FlushPanel()
    local info = self.data.act_info
    self.List:SetData(self.list_data)
    self:FlushStuff()
    self:FlushRewardRemind()
end

function ActLuckDrawBoxPanel:FlushStuff()
    local item_id,need_num = self.data:GetStuffItem()
    local num = Item.GetNum(item_id)
    UH.SetText(self.StuffValue, num)
    UH.SetIcon(self.Icon, Item.GetIconId(item_id))
    self.OpenRedPoint:SetNum(0)
    if num >= need_num then
        self.OpenRedPoint:SetNum(1)
        self.RandEffect:SetActive(true)
    else
        self.RandEffect:SetActive(false)
    end
end

function ActLuckDrawBoxPanel:OnRandOpenClick()
    if ActLuckDrawBoxData.Instance.is_playing == true then
        return
    end
    local list = self:GetNotOpenList()
    if TableIsEmpty(list) then
        PublicPopupCtrl.Instance:Center(Language.LuckBox.Tip1)
        return
    end
    local item_id,need_num = self.data:GetStuffItem()
    local num = Item.GetNum(item_id)
    if num >= need_num then
        local index = GameMath.Ramdon(1, #list)
        local v = list[index]
        --LogError("幸运盲盒请求开盒子",ACTIVITY_TYPE.LUCK_DRAW_BOX,ActLuckDrawBoxCtrl.ReqType.Open,v.x,v.y)
        -- ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.LUCK_DRAW_BOX,ActLuckDrawBoxCtrl.ReqType.Open,v.x,v.y)
        -- ActLuckDrawBoxData.Instance.is_playing = true
        -- Invoke(function ()
        --     ActLuckDrawBoxData.Instance.is_playing = false
        -- end,2)
        index = self.List:GetSelectedIndex(v)
        local mono = self.List:GetItem(index)
        if mono then
            mono:OnOpenClick()
        end
    else
        PublicPopupCtrl.Instance:Center(Language.LuckBox.Tip2)
        --MainOtherCtrl.Instance:GetWayViewById(item_id)
        ViewMgr:OpenView(ActLuckDrawStore)
    end
end

function ActLuckDrawBoxPanel:GetNotOpenList()
    local list = {}
    for i,v in pairs(self.list_data) do
        if self.data:IsOpen(v.x, v.y) < 0 then
            table.insert(list,v)
        end
    end
    return list
end

function ActLuckDrawBoxPanel:OnBuyClick()
    ViewMgr:OpenView(ActLuckDrawStore)
end

function ActLuckDrawBoxPanel:OnPreviewClick()
    ViewMgr:OpenView(ActLuckDrawPreview)
end

function ActLuckDrawBoxPanel:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[325].desc)
end

ActLuckDrawBoxItem = ActLuckDrawBoxItem or DeclareMono("ActLuckDrawBoxItem", UIWidgetBaseItem)
function ActLuckDrawBoxItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local flag = ActLuckDrawBoxData.Instance:IsOpen(data.x,data.y)
    self.is_open = flag >= 0
    if flag >= 0 then
        -- flag == seq
        local reward_cfg = ActLuckDrawBoxData.Instance:GetRewardCfg(flag)
        self.ItemCell:SetData(Item.Create(reward_cfg.reward_item))
    end
    self.On:SetActive(self.is_open == true)
    self.Off:SetActive(self.is_open ~= true)
end

function ActLuckDrawBoxItem:OnOpenClick()
    if ActLuckDrawBoxData.Instance.is_playing == true then
        return
    end
    if not self.is_open then
        local item_id,need_num = ActLuckDrawBoxData.Instance:GetStuffItem()
        local num = Item.GetNum(item_id)
        if num < need_num then
            PublicPopupCtrl.Instance:Center(Language.LuckBox.Tip2)
            --MainOtherCtrl.Instance:GetWayViewById(item_id)
            ViewMgr:OpenView(ActLuckDrawStore)
            return
        end
        --LogError("幸运盲盒请求开盒子",ACTIVITY_TYPE.LUCK_DRAW_BOX,ActLuckDrawBoxCtrl.ReqType.Open,self.data.x,self.data.y)
        self.Off:SetActive(false)
        self.EffectTool:Play("6161116")
        ActLuckDrawBoxData.Instance.is_playing = true
        TimeHelper:CancelTimer(self.time_ht)
        self.time_ht = Invoke(function ()
            ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.LUCK_DRAW_BOX,ActLuckDrawBoxCtrl.ReqType.Open,self.data.x,self.data.y)
            ActLuckDrawBoxData.Instance.is_playing = false
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.BoxOpen)
        end,1.5)
    end
end

function ActLuckDrawBoxItem:OnDestroy()
    TimeHelper:CancelTimer(self.time_ht)
end


-- ============= 预览 =============
ActLuckDrawPreview = ActLuckDrawPreview or DeclareView("ActLuckDrawPreview", "serveractivity/dragon_boat/luck_draw_box/act_luck_draw_preview")
function ActLuckDrawPreview:OnCloseClick()
    ViewMgr:CloseView(ActLuckDrawPreview)
end

function ActLuckDrawPreview:LoadCallback()
    local list_data = ActLuckDrawBoxData.Instance:GetPreviewListData()
    self.List:SetData(list_data)
end

ActLuckDrawPreviewItem = ActLuckDrawPreviewItem or DeclareMono("ActLuckDrawPreviewItem", UIWidgetBaseItem)
function ActLuckDrawPreviewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item)
    self.Cell:SetData(item)
    UH.SetText(self.Name, item:QuaName(true))
    UH.SetText(self.Tip, string.format(Language.LuckBox.Text1,data.show_rate * 100))
end


-- ============= 商城 ===========
ActLuckDrawStore = ActLuckDrawStore or DeclareView("ActLuckDrawStore", "serveractivity/dragon_boat/luck_draw_box/act_luck_draw_store_view")
function ActLuckDrawStore:LoadCallback()
    self.Currency:DefaultSet()
end
function ActLuckDrawStore:OnCloseClick()
    ViewMgr:CloseView(ActLuckDrawStore)
end


ActLuckDrawStoreBuyPanel = ActLuckDrawStoreBuyPanel or DeclareMono("ActLuckDrawStoreBuyPanel", UIWFlushPanel)
function ActLuckDrawStoreBuyPanel:ActLuckDrawStoreBuyPanel()
    self.data = ActLuckDrawBoxData.Instance
    self.data_cares = {
        {data = self.data.act_info, func = self.FlushPanel, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function ActLuckDrawStoreBuyPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function ActLuckDrawStoreBuyPanel:FlushPanel()
    local list_data = self.data:GetShopList()
    table.sort(list_data, function (a,b)
        local buy_num = ActLuckDrawBoxData.Instance:GetBuyCount(a)
        buy_num = a.limit_times - buy_num
        local a_flag = buy_num > 0 and 1 or 0

        buy_num = ActLuckDrawBoxData.Instance:GetBuyCount(b)
        buy_num = b.limit_times - buy_num
        local b_flag = buy_num > 0 and 1 or 0

        if b_flag == a_flag then
            return a.seq < b.seq
        end
        return a_flag > b_flag
    end)
    self.List:SetData(list_data)
    if #list_data > 3 then
        self.ListObj:SetLocalPosition(Vector3.New(-3.3,-20,0))
    else
        self.ListObj:SetLocalPosition(Vector3.New(37,-20,0))
    end
end

function ActLuckDrawStoreBuyPanel:OnBuyClick(data)
    local buy_num = ActLuckDrawBoxData.Instance:GetBuyCount(data)
    local can_buy_count = data.limit_times - buy_num
    if can_buy_count > 0 then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        self.cfg = data
        if data.cfg_or == 1 then
            --LogError("盲盒请求购买",ACTIVITY_TYPE.LUCK_DRAW_BOX,ActLuckDrawBoxCtrl.ReqType.Buy,data.seq)
            local fee = data.price / 10
            ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_LUCK_BOX,fee,ACTIVITY_TYPE.LUCK_DRAW_BOX,ActLuckDrawBoxCtrl.ReqType.Buy,data.seq)
        else
            if MallCtrl.IsNotCurrency(data.price_type, data.price) then
                ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.LUCK_DRAW_BOX,ActLuckDrawBoxCtrl.ReqType.Buy,data.seq)
            end
        end
    else
        PublicPopupCtrl.Instance:Error(Language.LuckBox.Tip3)
    end
end

function ActLuckDrawStoreBuyPanel:SendBuy()
    if self.cfg == nil then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_LUCK_BOX, self.cfg.price)
end


ActLuckDrawStoreBuyItem = ActLuckDrawStoreBuyItem or DeclareMono("ActLuckDrawStoreBuyItem", UIWidgetBaseItem)
function ActLuckDrawStoreBuyItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item[0])
    self.Cell:SetData(item)
    UH.SetText(self.Name, item:QuaName(true))
    local limit_text = data.limit_type == 1 and Language.LuckBox.Text2 or Language.LuckBox.Text3
    local buy_num = ActLuckDrawBoxData.Instance:GetBuyCount(data)
    buy_num = data.limit_times - buy_num
    self.can_buy_count = buy_num
    local limit_num = data.limit_times
    local tip = string.format("%s：%s/%s", limit_text,buy_num, limit_num)
    UH.SetText(self.Tip1, tip)
    UH.SetText(self.Tip2, tip)
    self.price = data.price
    local fee = data.value
    if data.cfg_or == 1 then
        self.price = data.price / 10
        fee = data.value / 10
    end
    UH.SetText(self.Price, Language.LuckBox.Text4 .. fee)
    UH.SetText(self.BuyText1, self.price)
    UH.SetText(self.BuyText2, self.price)
    self.Inter.Interactable = buy_num > 0
    self.Effect:SetActive(buy_num > 0)
    UH.SetActGold(self.PriceType,data.price_type,data.cfg_or)
end

-- function ActLuckDrawStoreBuyItem:OnBuyClick()
--     if self.can_buy_count > 0 then
--         --LogError("盲盒请求购买",ACTIVITY_TYPE.LUCK_DRAW_BOX,ActLuckDrawBoxCtrl.ReqType.Buy,self.data.seq)
--         ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.LUCK_DRAW_BOX,ActLuckDrawBoxCtrl.ReqType.Buy,self.data.seq)
--     else
--         PublicPopupCtrl.Instance:Center(Language.LuckBox.Tip3)
--     end
-- end



-- 充值奖励
ActLuckDrawStoreChongZhiPanel = ActLuckDrawStoreChongZhiPanel or DeclareMono("ActLuckDrawStoreChongZhiPanel", UIWFlushPanel)
function ActLuckDrawStoreChongZhiPanel:ActLuckDrawStoreChongZhiPanel()
    self.data = ActLuckDrawBoxData.Instance
    self.data_cares = {
        {data = self.data.act_info, func = self.FlushPanel, init_call = false}
    }
end

function ActLuckDrawStoreChongZhiPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function ActLuckDrawStoreChongZhiPanel:FlushPanel()
    local list_data = self.data:GetChongZhiList()
    self.List:SetData(list_data)
end

ActLuckDrawStoreChongZhiItem = ActLuckDrawStoreChongZhiItem or DeclareMono("ActLuckDrawStoreChongZhiItem", UIWidgetBaseItem)
function ActLuckDrawStoreChongZhiItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item[0])
    self.Cell:SetData(item)
    UH.SetText(self.Num, data.add_recharge)
    local flag = ActLuckDrawBoxData.Instance:ChongZhiFlag(data)
    self.GetFlag:SetActive(flag == -1)
    self.Inter.Interactable = flag == 1
    self.BuyBtnObj:SetActive(flag ~= -1)
    self.Effect:SetActive(flag == 1)
end

function ActLuckDrawStoreChongZhiItem:OnGetClick()
    local flag = ActLuckDrawBoxData.Instance:ChongZhiFlag(self.data)
    if flag == 1 then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        --LogError("盲盒请求领奖",ACTIVITY_TYPE.LUCK_DRAW_BOX,ActLuckDrawBoxCtrl.ReqType.GetReward,self.data.seq)
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.LUCK_DRAW_BOX,ActLuckDrawBoxCtrl.ReqType.GetReward,self.data.seq)
    else
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    end
end



ActLuckDrawRemind = ActLuckDrawRemind or DeclareMono("ActLuckDrawRemind", UIWFlushPanel)
function ActLuckDrawRemind:ActLuckDrawRemind()
    self.data = ActLuckDrawBoxData.Instance
    self.data_cares = {
        {data = self.data.act_info, func = self.FlushPanel, init_call = true}
    }
end

function ActLuckDrawRemind:FlushPanel()
    local remind_num = self.data:RewardRemind()
    self.RedPoint:SetNum(remind_num)
end