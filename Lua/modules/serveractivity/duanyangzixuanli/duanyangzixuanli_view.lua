--===========================端阳自选礼一般界面模板===========================
ActDuanYangZiXuanLiView = ActDuanYangZiXuanLiView or DeclareView("ActDuanYangZiXuanLiView", "serveractivity/duanyangzixuanli/act_duanyangzixuanli",Mod.DragonBoat.DuanYangZiXuanLi)

-- 创建单例
function ActDuanYangZiXuanLiView:ActDuanYangZiXuanLiView()
    self.data = ActDuanYangZiXuanLiData.Instance
    
end

-- 回调
function ActDuanYangZiXuanLiView:LoadCallback()

    local cfg_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI, Config.duanyangzixuanli_auto.basis_configuration)
    self.data.section = cfg_list
    -- LogError("端阳自选礼sectionsectionsectionsection",cfg_list)
    -- LogError("端阳sectionsectionsectionsection",self.data.section)
    
    self.data.actOpenDay = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI)
    -- LogError("actOpenDay",self.actOpenDay)
end

-- 关闭界面
function ActDuanYangZiXuanLiView:OnClickClose()
    ViewMgr:CloseView(ActDuanYangZiXuanLiView)
end

-- 点击提示
function ActDuanYangZiXuanLiView:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[328].desc})
end


--===========================端阳自选礼界面刷新模板===========================
ActDuanYangZiXuanLiPanel = ActDuanYangZiXuanLiPanel or DeclareMono("ActDuanYangZiXuanLiPanel", UIWFlushPanel)


function ActDuanYangZiXuanLiPanel:ActDuanYangZiXuanLiPanel()
    self.data = ActDuanYangZiXuanLiData.Instance
    
    -- 监听大item的刷新
    self.data_cares = {
        -- 监听函数，只要duanyangzixuanli_smart变了，就会回调这几个函数
        {data = self.data.duanyangzixuanli_smart, func = self.FlushDuanYangZiXuanLiPanel,init_call = false},
        {data = self.data.duanyangzixuanli_smart, func = self.FlushDuanYangZiXuanLiBigItemList,init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end


function ActDuanYangZiXuanLiPanel:Awake()
    UIWFlushPanel.Awake(self)

    self.data.first_remind = true
    -- 刷红点
    self.data:RemindNotice()

    -- 设置一键购买花费数量
    UH.SetText(self.BuyAllMoneyTxt, self.data:GetBuyAllMoneyCost())

    -- 活动剩余时间回调
    self.ActiveRemainingTimeMeter:SetCallBack(function ()
        -- ViewMgr:CloseView(ActDuanYangZiXuanLiView)
    end)

    -- 设置剩余活动时间
    -- self.ActiveRemainingTimeMeter:CreateActTime(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI,TimeType.Type_Time_3)
    local startime,endtime =ActivityRandData.GetBaseDayOpenStampTime(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI)
    self.ActiveRemainingTimeMeter:StampTime(endtime,TimeType.Type_Time_3)

    self:FlushDuanYangZiXuanLiBigItemList()
    self:FlushDuanYangZiXuanLiPanel()
end

function ActDuanYangZiXuanLiPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    -- 刷红点
    self.data:RemindNotice()
end

-- 一键购买全部
function ActDuanYangZiXuanLiPanel:OnClickBuyAll()

    -- 发送购买协议
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_DUANYANGZIXUANLI, self.data:GetBuyAllMoneyCost(), ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI, ActDuanYangZiXuanLiCtrl.REQ_TYPE.REQ_BUY)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)

end

-- 刷新界面按钮
function ActDuanYangZiXuanLiPanel:FlushDuanYangZiXuanLiPanel()

    local flag = self.data.duanyangzixuanli_smart.is_buy_reward or 0
    -- LogError("self.data.duanyangzixuanli_smart.is_buy_reward",self.data.duanyangzixuanli_smart.is_buy_reward)
    self.BuyAllBtn:SetActive(flag == 0)


end



-- 刷新大item列表
function ActDuanYangZiXuanLiPanel:FlushDuanYangZiXuanLiBigItemList()
    -- 获得大item列表
    local duanyangzixuanli_bigitem_list = self.data:GetDuanYangZiXuanLiBigItemList()

    if #duanyangzixuanli_bigitem_list <= 4 then
        self.ContentRectTransform.pivot = Vector2.New(0.3,1)
    end

    -- 设置panel的大item列表
    if duanyangzixuanli_bigitem_list then
        self.DuanYangZiXuanLiBigItemList:SetData(duanyangzixuanli_bigitem_list)
    end
end

-- 发送购买
function ActDuanYangZiXuanLiPanel:SendBuy()
    -- if self.cfg == nil then
    --     return
    -- end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_DUANYANGZIXUANLI, self.data:GetBuyAllMoneyCost()*10)
end




-- --===========================界面大Item模板===========================
ActDuanYangZiXuanLiBigItem = ActDuanYangZiXuanLiBigItem or DeclareMono("ActDuanYangZiXuanLiBigItem", UIWidgetBaseItem)

function ActDuanYangZiXuanLiBigItem:ActDuanYangZiXuanLiBigItem()
    self.data_instance = ActDuanYangZiXuanLiData.Instance

    -- 能接受奖励计时器结束时的回调
    self.CanReceiveTimeMeter:SetCallBack(function ()
        -- ActDuanYangZiXuanLiCtrl.Instance:SendDuanYangZiXuanLiReq(ActDuanYangZiXuanLiCtrl.REQ_TYPE.REQ_INFO)
        -- 刷新大Item的全部按钮变黄色，可领取和计时器关闭
        -- self:FlushChaseButtonYellow()
        self.HouKeLingQuText:SetActive(false)
        self.CanReceiveTimeMeterText:SetActive(false)
        self.CanReceiveTimeMeterOver = true;
    end)


end




-- -- 取消监听
-- function ActDuanYangZiXuanLiBigItem:OnRelease()
--     -- 基类取消监听
--     UIWidgetBaseItem.OnRelease(self)
--     -- 如果句柄不是空就取消监听
--     if self.smart_care_handle ~= nil then
--         self.data_instance.duanyangzixuanli_smart:Uncare(self.smart_care_handle)
--     end
-- end

-- 设置大item的数据
function ActDuanYangZiXuanLiBigItem:SetData(data)
    -- 获得要用到的大item的数据
    self.activity_day_name = data.activity_day_name
    self.activity_day = data.activity_day
    self.reward_group = data.reward_group
    -- self.gift_icon = data.gift_icon
    -- self.gift_desc = data.gift_desc

    self.isReceive = data.isReceive
    -- LogError("isReceive",self.isReceive)

    -- 基类设置item数据
    UIWidgetBaseItem.SetData(self, data)

    -- 设置天数文本的text
    UH.SetText(self.Day, self.activity_day_name)

    -- 设置活动结束计时器的时间
    -- local endtime = ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI)
    local startime,endtime =ActivityRandData.GetBaseDayOpenStampTime(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI)

    -- 没领取显示后可领取
    self.HouKeLingQuText:SetActive(not self.isReceive)

    -- 没领取就显示倒计时，领取了说明倒计时结束了，倒计时结束会回调让倒计时不显示
    self.CanReceiveTimeMeterText:SetActive(not self.isReceive)
    self.CanReceiveTimeMeter:StampTime(endtime-86400 * (self.data_instance:GetSelfSectionLength()+1-self.activity_day), TimeType.Type_Time_7)
    self.CanReceiveTimeMeterOver = TimeManager.GetServerTime() > endtime-86400 * (self.data_instance:GetSelfSectionLength()+1-self.activity_day)
    -- LogError(self.activity_day_name,"CanReceiveTimeMeterOver",self.CanReceiveTimeMeterOver)
    -- LogError(self.activity_day_name,"self.isReceive",self.isReceive)
    
    -- 领取按钮假如要是黄色 要当前没领取礼包而且计时器已经计时完了，不然就不能是黄色的按钮
    local flag = (not self.isReceive) and (self.CanReceiveTimeMeterOver == true)
    -- LogError(self.activity_day_name,"flag",flag)
    self.ChaseButtonYellow = flag
    -- LogError("self.ChaseButtonYellow",self.ChaseButtonYellow)

    -- self:FlushDuanYangZiXuanLiItemList(self.reward_group,self.gift_icon,self.gift_desc,self.ChaseButtonYellow)
    -- 刷新小item
    self:FlushDuanYangZiXuanLiItemList(self.reward_group,self.ChaseButtonYellow)

end



-- function ActDuanYangZiXuanLiBigItem:FlushDuanYangZiXuanLiItemList(reward_group,gift_icon,gift_desc,ChaseButtonYellow)
function ActDuanYangZiXuanLiBigItem:FlushDuanYangZiXuanLiItemList(reward_group,ChaseButtonYellow)

    -- local duanyangzixuanli_item_list = self.data_instance:GetDuanYangZiXuanLiItemList(reward_group,gift_icon,gift_desc,ChaseButtonYellow,self)
    local duanyangzixuanli_item_list = self.data_instance:GetDuanYangZiXuanLiItemList(reward_group,ChaseButtonYellow,self)
    -- LogError("duanyangzixuanli_item_list",duanyangzixuanli_item_list)


    if duanyangzixuanli_item_list then
        self.DuanYangZiXuanLiItemList:SetData(duanyangzixuanli_item_list)
    end
end

function ActDuanYangZiXuanLiBigItem:FlushChaseButtonGray()
    -- LogError("其他按钮也变灰色")
    for k, v in pairs(self.DuanYangZiXuanLiItemList.item_list) do
        v.ChaseButtonInteractorHandler.Interactable = false
    end


end


function ActDuanYangZiXuanLiBigItem:FlushChaseButtonYellow()
    -- LogError("其他按钮也变黄色")
    for k, v in pairs(self.DuanYangZiXuanLiItemList.item_list) do
        v.ChaseButtonInteractorHandler.Interactable = true
    end


end
-- function ActDuanYangZiXuanLiBigItem:OnSelect(selected)
--     UIWidgetBaseItem.OnSelect(self,selected)
-- end

-- function ActDuanYangZiXuanLiBigItem:Click()
--     self.Toggle:Trigger()
-- end
-- --===========================界面Item模板===========================
ActDuanYangZiXuanLiItem = ActDuanYangZiXuanLiItem or DeclareMono("ActDuanYangZiXuanLiItem", UIWidgetBaseItem)

function ActDuanYangZiXuanLiItem:ActDuanYangZiXuanLiItem()
    self.data_instance = ActDuanYangZiXuanLiData.Instance
end

-- -- 取消监听
-- function ActDuanYangZiXuanLiBigItem:OnRelease()
--     -- 基类取消监听
--     UIWidgetBaseItem.OnRelease(self)
--     -- 如果句柄不是空就取消监听
--     if self.smart_care_handle ~= nil then
--         self.data_instance.duanyangzixuanli_smart:Uncare(self.smart_care_handle)
--     end
-- end

function ActDuanYangZiXuanLiItem:ShowItemRedPoint()
    if self.data_instance.duanyangzixuanli_smart.is_buy_reward == 1 and 
    self.data_instance.duanyangzixuanli_smart.fetch_reward_flag[self.BigItem.activity_day]  == 0 and
    self.ChaseButtonInteractorHandler.Interactable == true
    then        
        return 1
    else
        return 0
    end
end

function ActDuanYangZiXuanLiItem:SetData(data)

    self.gift_icon = data.gift_icon
    self.gift_desc = data.gift_desc

    -- 获得顺序，用来展示弹窗的道具列表
    self.seq = data.seq 
    self.item_list = data.reward_item
    self.ChaseButtonInteractorHandler.Interactable = data.ChaseButtonYellow

    -- 设置小item的文本和图片
    UH.SpriteName(self.WuPinKuang, self.data_instance.WuPinKuang[self.gift_icon])
    UH.SpriteName(self.GiftIcon, self.data_instance.GiftIcon[self.gift_icon])
    UH.SetText(self.ItemNameText, self.gift_desc)

    -- 获得当前小item的大item
    self.BigItem = data.BigItem

    -- 设置红点
    self.ItemRedPoint:SetNum(self:ShowItemRedPoint())

    -- 如果大item已经领取
    if self.data_instance.duanyangzixuanli_smart.fetch_reward_flag[self.BigItem.activity_day]  ~= 0  then
        -- 拿到当前领取的顺序
        local seq = self.data_instance.duanyangzixuanli_smart.select_reward_seq[self.BigItem.activity_day]
        if self.seq == seq then
            self:FlushChaseButtonAndYiLingQuImage()
        end
    end

end

function ActDuanYangZiXuanLiItem:OnClickChase()

    -- 未购买时点击领取按钮提示：您还未购买礼包哦。未购买时所有的领取按钮都可以用这个提示
    if self.data_instance.duanyangzixuanli_smart.is_buy_reward == 0 then
        PublicPopupCtrl.Instance:Center(Language.DuanYangZiXuanLi.NotBuyBag)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    -- 购买后未到领取时间点击领取按钮则提示：未到领取时间   
    elseif self.data_instance.duanyangzixuanli_smart.is_buy_reward == 1 and self.BigItem.CanReceiveTimeMeterOver == false then
        PublicPopupCtrl.Instance:Center(Language.DuanYangZiXuanLi.NotReady)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    -- 购买后已领取礼包后，再点击其余礼包按钮，则提示：您已领取其他礼包
    elseif  self.data_instance.duanyangzixuanli_smart.is_buy_reward == 1 and self.ChaseButtonInteractorHandler.Interactable == false then
        PublicPopupCtrl.Instance:Center(Language.DuanYangZiXuanLi.HaveGet)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    -- 按钮黄色且已经购买
    elseif self.ChaseButtonInteractorHandler.Interactable == true and self.data_instance.duanyangzixuanli_smart.is_buy_reward == 1 then
        -- 发送领取，领取成功了会变成fetch_reward_flag = 1 
        ActDuanYangZiXuanLiCtrl.Instance:SendDuanYangZiXuanLiReq(ActDuanYangZiXuanLiCtrl.REQ_TYPE.REQ_GET_REWARD,self.BigItem.activity_day, self.seq)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    end

end

-- 刷新按钮和已领取的图片
function ActDuanYangZiXuanLiItem:FlushChaseButtonAndYiLingQuImage()
    local flag = self.data_instance.duanyangzixuanli_smart.fetch_reward_flag or 0
    self.ChaseButton:SetActive(flag == 0);
    self.YiLingQuImage:SetActive(flag ~= 0);
end


-- 点击出弹窗
function ActDuanYangZiXuanLiItem:OnClickGiftIcon()
    local t = {}
    t.item_list = DataHelper.FormatItemList(self.item_list)
    ViewMgr:OpenView(DuanYangZiXuanLiShowItemView, t)
end


----------------------------DuanYangZiXuanLiShowItemView----------------------------
DuanYangZiXuanLiShowItemView = DuanYangZiXuanLiShowItemView or DeclareView("DuanYangZiXuanLiShowItemView", "serveractivity/duanyangzixuanli/duanyangzixuanli_item_view")
VIEW_DECLARE_MASK(DuanYangZiXuanLiShowItemView, ViewMask.BlockClose)
function DuanYangZiXuanLiShowItemView:LoadCallback(param_t)
    self.ShowList:SetData(param_t.item_list)
end

