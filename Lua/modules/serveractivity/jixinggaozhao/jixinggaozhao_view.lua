ActJiXingGaoZhaoView = ActJiXingGaoZhaoView or DeclareView("ActJiXingGaoZhaoView", "serveractivity/jixinggaozhao/act_jixinggaozhao", Mod.RandActivity.JiXingGaoZhao)

function ActJiXingGaoZhaoView:ActJiXingGaoZhaoView()
    self.data = ActJiXingGaoZhaoData.Instance
end

function ActJiXingGaoZhaoView:LoadCallback()
    self.SkipToggle.isOn = self.data.jixing_smart.skip_rolling
    -- BagData.Instance:SetHideCurrencyCenterFlag(true)
    ActJiXingGaoZhaoCtrl.Instance:SendJiXingGaoZhaoReq(ActJiXingGaoZhaoCtrl.REQ_TYPE.REQ_INFO)

    self.LeftTime:SetCallBack(function ()
        ViewMgr:CloseView(ActJiXingGaoZhaoView)
    end)
    self.LeftTime:CreateActTime(ACTIVITY_TYPE.RAND_JIXINGGAOZHAO, TimeType.Type_Time_3)
end

function ActJiXingGaoZhaoView:CloseCallback()
    -- BagData.Instance:SetHideCurrencyCenterFlag(false)
    self.data.draw_result_item = nil
end

function ActJiXingGaoZhaoView:OnClickBuyStoreBtn()
    ViewMgr:OpenView(ActJiXingGaoZhaoStoreView)
end

function ActJiXingGaoZhaoView:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[182].desc})
end

function ActJiXingGaoZhaoView:OnClickRate()
    ViewMgr:OpenView(ActJiXingGaoZhaoRateView)
    -- ActJiXingGaoZhaoData.Instance:SetDrawResult({draw_result = 7})
end

function ActJiXingGaoZhaoView:OnClickClose()
    ViewMgr:CloseView(ActJiXingGaoZhaoView)
end

----------------------------ActJiXingGaoZhaoPanel----------------------------
ActJiXingGaoZhaoPanel = ActJiXingGaoZhaoPanel or DeclareMono("ActJiXingGaoZhaoPanel", UIWFlushPanel)
function ActJiXingGaoZhaoPanel:ActJiXingGaoZhaoPanel()
    self.data = ActJiXingGaoZhaoData.Instance
    self.data_cares = {
        {data = self.data.jixing_smart, func = self.FlushBoxes, keys = {"flush"}},
        {data = self.data.jixing_smart, func = self.ShowRolling, keys = {"draw_result"}, init_call = false},
        {data = self.data.jixing_smart, func = self.FlushPoolItem, keys = {"flush_pool_item"}},
        {data = self.data.jixing_smart, func = self.FlushRecordList, keys = {"flush_record"}},
        {data = BagData.Instance.item_grid[ItemColumnType.Material], func = self.FlushStarInfo, init_call = false},
    }
end

function ActJiXingGaoZhaoPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushStarInfo()
    self:FlushRollingList()
end
-- 刷新转盘奖励
function ActJiXingGaoZhaoPanel:FlushRollingList()
    local item_list = self.data:GetRollingList()
    for i = 1, self.ItemList:Length() do
        self.ItemList[i]:SetData(item_list[i])
    end
    
    for i = 1, 8 do
        self.SelectToggles[i].isOn = false
    end
end

--奖池物品（转盘右下角）
function ActJiXingGaoZhaoPanel:FlushPoolItem()
    self.PoolItem:SetData(Item.Init(self.data.pool_item_id))
    UH.SetText(self.PoolItemNum, self.data.pool_item_num)
end

function ActJiXingGaoZhaoPanel:FlushStarInfo()
    UH.SetText(self.StarCountTxt, self.data:GetStarCount())
end

--宝箱及进度条状态
function ActJiXingGaoZhaoPanel:FlushBoxes()
    -- LogError("flushBoxes")
    local boxes_info = self.data:GetBoxInfoList()
    for i = 1, self.Boxes:Length() do
        if boxes_info[i] then
            self.Boxes[i]:SetData(boxes_info[i])
        end
    end
    self.RewardProgress:SetProgress(self.data:GetDrawProgress())
    UH.SetText(self.DrawTimes, string.format(Language.JiXingGaoZhao.DrawTimes, self.data.draw_times))
end
-- 消息记录
function ActJiXingGaoZhaoPanel:FlushRecordList()
    self.InfoList:SetData(self.data:GetRecordList())
end

function ActJiXingGaoZhaoPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.rolling_runner ~= nil then
        TimeHelper:CancelTimer(self.rolling_runner)
    end
    if self.delay_show_result ~= nil then
        TimeHelper:CancelTimer(self.delay_show_result)
    end
    if self.rolling_slow_runner ~= nil then
        TimeHelper:CancelTimer(self.rolling_slow_runner)
    end
    self.show_rolling = false
end

function ActJiXingGaoZhaoPanel:ShowRolling()
    self.show_rolling = true
    if self.data.jixing_smart.skip_rolling then
        self.SelectToggles[self.data.jixing_smart.draw_result].isOn = true
        if self.data.draw_result_item ~= nil then
            self.delay_show_result = TimeHelper:AddDelayTimer(function ()
                self.show_rolling = false
                ActRewardTipData.Instance:OpenActRewardTipViewByData(ServeractivityConfig.ActRewerdTip.RewardType.Success,{self.data.draw_result_item})
            end, 0.3)
        end
    else
        local slow_index = self.data.jixing_smart.draw_result > 4 and self.data.jixing_smart.draw_result - 4 or self.data.jixing_smart.draw_result + 4
        -- LogError("slow_index",slow_index)
        self.roll_index = 1
        local update_func = function ()
            self.SelectToggles[self.roll_index].isOn = true
            self.roll_index = self.roll_index + 1 > 8 and 1 or self.roll_index + 1
        end
        local complete_func = function()
            self.rolling_slow_runner = TimeHelper:AddCountDownTT(update_func,
            function ()
                self.SelectToggles[self.data.jixing_smart.draw_result].isOn = true
                if self.data.draw_result_item ~= nil then
                    -- self.delay_show_result = TimeHelper:AddDelayTimer(function ()
                        self.show_rolling = false
                        ActRewardTipData.Instance:OpenActRewardTipViewByData(ServeractivityConfig.ActRewerdTip.RewardType.Success,{self.data.draw_result_item})
                    -- end, 0.3)
                end
            end, 5 * 0.4, 0.4, false)
        end
        self.rolling_runner = TimeHelper:AddCountDownTT(update_func, complete_func, 0.1 * (15 + slow_index), 0.1)
    end
end

function ActJiXingGaoZhaoPanel:OnClickDraw()
    if self.show_rolling then
        PublicPopupCtrl.Instance:Center(Language.JiXingGaoZhao.ShowRolling)
        return
    elseif self.data:GetStarCount() <= 0 then
        ViewMgr:OpenView(ActJiXingGaoZhaoStoreView)
        return
    end
    for i = 1, 8 do
        self.SelectToggles[i].isOn = false
    end
    self.data.draw_result_item = nil
    self.show_rolling = true
    ActJiXingGaoZhaoCtrl.Instance:SendJiXingGaoZhaoReq(ActJiXingGaoZhaoCtrl.REQ_TYPE.REQ_DRAW)
    --test code
    -- if IS_EDITOR then
    --     ActJiXingGaoZhaoCtrl.Instance:OnSCRAJiXingGaoZhaoDrawResult({seq = 1, num = 20})
    -- end
    -- test end
end

function ActJiXingGaoZhaoPanel:OnClickSkipRolling(toggle)
    self.data.jixing_smart.skip_rolling = toggle
end
--------转盘item（包含NumItem和ItemCell）----------------------------
ActJiXingGaoZhaoPoolItem = ActJiXingGaoZhaoPoolItem or DeclareMono("ActJiXingGaoZhaoPoolItem", UIWidgetBaseItem)
function ActJiXingGaoZhaoPoolItem:ActJiXingGaoZhaoPoolItem()
    self.data_instance = ActJiXingGaoZhaoData.Instance
end

function ActJiXingGaoZhaoPoolItem:SetData(data)
    self.NumItem:SetObjActive(data.reward_type == 2)
    self.ItemCell:SetObjActive(data.reward_type ~= 2)
    if data.reward_type == 2 then
        self.NumItem:SetData(data.reward_per)
    else
        self.ItemCell:SetData(data.reward_type == 0 and Item.Create(data.reward_item) or Item.Init(HuoBi[CurrencyType.Gold], data.gold_num))
    end
end

-------转盘折扣数值显示item-------------------------
ActJiXingRewardNumItem = ActJiXingRewardNumItem or DeclareMono("ActJiXingRewardNumItem", UIWidgetBaseItem)
function ActJiXingRewardNumItem:ActJiXingRewardNumItem()
end

function ActJiXingRewardNumItem:SetData(data)
    self.data = data
    UH.SetText(self.SpText, string.format(Language.Common.Percent, data))
end
-------宝箱item----------------------------
ActJiXingGaoZhaoRewardBox = ActJiXingGaoZhaoRewardBox or DeclareMono("ActJiXingGaoZhaoRewardBox", UIWidgetBaseItem)
function ActJiXingGaoZhaoRewardBox:ActJiXingGaoZhaoRewardBox()
end

function ActJiXingGaoZhaoRewardBox:OnEnable()
    -- LogError("onenable play_effect",self.enabled_play_effect)
    if self.enabled_play_effect then
        self.get_eff_handle = self.EffectTool:Play(6161021)
    end
end

function ActJiXingGaoZhaoRewardBox:SetData(data)
    self.data = data
    self.HasGet:SetActive(data.flag == 1)
    -- LogError("setdata canget ",data.can_get)
    UH.SetText(self.DrawTimesText, string.format(Language.JiXingGaoZhao.BoxDrawTimes, data.lucky_draw_num))
    if data.can_get and data.flag == 0 then
        if self.EffectTool.isActiveAndEnabled and self.get_eff_handle == nil then
            self.get_eff_handle = self.EffectTool:Play(6161021)
        else
            self.enabled_play_effect = true
        end
    elseif self.get_eff_handle ~= nil then
        self.EffectTool:Stop(self.get_eff_handle)
    else
        self.enabled_play_effect = false
    end
end

function ActJiXingGaoZhaoRewardBox:OnClickBox()
    if self.data.flag == 1 then
        PublicPopupCtrl.Instance:Center(Language.JiXingGaoZhao.AlreadyGet)
        return
    elseif not self.data.can_get then
        ViewMgr:OpenView(ShowItemView,{item_list = self.data.item_list,pos = Vector3.New(-95 + self.data.seq * 175, -49,0),pivot = 0})
        return
    end
    ActJiXingGaoZhaoCtrl.Instance:SendJiXingGaoZhaoReq(ActJiXingGaoZhaoCtrl.REQ_TYPE.REQ_GET_DRAW_REWARD, self.data.seq)
end
-------------------------------------------
ActJiXingGaoZhaoInfoItem = ActJiXingGaoZhaoInfoItem or DeclareMono("ActJiXingGaoZhaoInfoItem", UIWidgetBaseItem)
function ActJiXingGaoZhaoInfoItem:ActJiXingGaoZhaoInfoItem()
    self.data_instance = ActJiXingGaoZhaoData.Instance
    self.reward_type_func = {
        [1] = function ()
            if self.data then
                return Item.GetName(self.data.item_id)
            end
        end,
        [2] = function ()
            if self.data then
                return self.data.num or ""
            else
                return IS_EDITOR and "self.data is nil" or ""
            end
        end}
end

function ActJiXingGaoZhaoInfoItem:SetData(data)
    self.data = data
    local format_type = data.reward_type <= 1 and 1 or data.reward_type
    UH.SetText(self.InfoText, string.format(Language.JiXingGaoZhao.RecordInfoFormat[format_type], data.role_name, format_type == 1 and data.num or "", self.reward_type_func[format_type]()))
end
--------吉星商城----------------------------
ActJiXingGaoZhaoStoreView = ActJiXingGaoZhaoStoreView or DeclareView("ActJiXingGaoZhaoStoreView", "serveractivity/jixinggaozhao/act_jixinggaozhao_store")
function ActJiXingGaoZhaoStoreView:ActJiXingGaoZhaoStoreView()
    self.data = ActJiXingGaoZhaoData.Instance
    self.care_handle = ServerActivityData.Instance:GetCmdBuyItemCheckRet():Care(function ()
        self:SendBuy()
    end)
    self.jixing_care_handle = self.data.jixing_smart:KeysCare({"storeFlush"},function ()
        self.StoreItemList:SetData(ActJiXingGaoZhaoData.Instance:GetStoreInfoList())
    end)
end

function ActJiXingGaoZhaoStoreView:LoadCallback()
    self.StoreItemList:SetData(ActJiXingGaoZhaoData.Instance:GetStoreInfoList())
end

function ActJiXingGaoZhaoStoreView:CloseCallback()
    ServerActivityData.Instance:GetCmdBuyItemCheckRet():Uncare(self.care_handle)
    self.data.jixing_smart:Uncare(self.jixing_care_handle)
end

function ActJiXingGaoZhaoStoreView:OnClickClose()
    ViewMgr:CloseView(ActJiXingGaoZhaoStoreView)
end

function ActJiXingGaoZhaoStoreView:SendBuy()
    if self.data.direct_cache_cfg then
        ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_JIXINGGAOZHAO, self.data.direct_cache_cfg.buy_money)
        self.data.direct_cache_cfg = nil
    end
end

---------吉星商城item------------------------------
ActJiXingGaoZhaoStoreItem = ActJiXingGaoZhaoStoreItem or DeclareMono("ActJiXingGaoZhaoStoreItem", UIWidgetBaseItem)
function ActJiXingGaoZhaoStoreItem:ActJiXingGaoZhaoStoreItem()
    self.data_instance = ActJiXingGaoZhaoData.Instance
    self.buy_money_rate = {[0] = 1,[3] = 0.1,[16] = 1}
end

function ActJiXingGaoZhaoStoreItem:SetData(data)
    self.data = data
    UH.SpriteName(self.PrictType, self.data_instance.price_type_id[data.price_type])
    UH.SetText(self.BuyBtnText, data.buy_money * self.buy_money_rate[data.price_type])
    UH.SetText(self.Name, data.gift_name)
    UH.SetText(self.BuyLimitText, string.format(Language.JiXingGaoZhao.LimitType[data.limit_type], data.buy_max and COLORSTR.Red8 or COLORSTR.Green2, data.buy_times - data.buy_count <= 0 and 0 or data.buy_times - data.buy_count, data.buy_times))
    self.ItemList:SetData(data.item_list)
    self.BtnInteractor.Interactable = not data.buy_max
end

function ActJiXingGaoZhaoStoreItem:OnClickBuy()
    if self.data.buy_max then
        PublicPopupCtrl.Instance:Center(Language.JiXingGaoZhao.BuyMax)
        return
    end
    if self.data.price_type == 3 then
        self.data_instance.direct_cache_cfg = self.data
        local fee = self.data.buy_money * self.buy_money_rate[self.data.price_type]
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_JIXINGGAOZHAO,fee, ACTIVITY_TYPE.RAND_JIXINGGAOZHAO, ActJiXingGaoZhaoCtrl.REQ_TYPE.REQ_BUY_ITEM_SHOP, self.data.seq)
    else
        ActJiXingGaoZhaoCtrl.Instance:SendJiXingGaoZhaoReq(ActJiXingGaoZhaoCtrl.REQ_TYPE.REQ_BUY_ITEM_DIRECT, self.data.seq)
    end
end

--------概率界面-----------------------
ActJiXingGaoZhaoRateView = ActJiXingGaoZhaoRateView or DeclareView("ActJiXingGaoZhaoRateView","serveractivity/jixinggaozhao/act_jixinggaozhao_rate")
VIEW_DECLARE_MASK(ActJiXingGaoZhaoRateView,ViewMask.BgBlock)
function ActJiXingGaoZhaoRateView:ActJiXingGaoZhaoRateView()
    self.data = ActJiXingGaoZhaoData.Instance
end

function ActJiXingGaoZhaoRateView:LoadCallback()
    self.Board:SetData(ActJiXingGaoZhaoRateView, Language.JiXingGaoZhao.RateTitle,Vector2.New(546, 410))
    self.ItemList:SetData(self.data:GetRollingList())
end
--------概率item（包含NumItem和ItemCell）-------------------
ActJiXingGaoZhaoRateViewItem = ActJiXingGaoZhaoRateViewItem or DeclareMono("ActJiXingGaoZhaoRateViewItem",UIWidgetBaseItem)
function ActJiXingGaoZhaoRateViewItem:ActJiXingGaoZhaoRateViewItem()
end

function ActJiXingGaoZhaoRateViewItem:SetData(data)
    self.data = data
    self.NumItem:SetObjActive(data.reward_type == 2)
    self.ItemCell:SetObjActive(data.reward_type ~= 2)
    if data.reward_type == 2 then
        self.NumItem:SetData(data.reward_per)
    else
        self.ItemCell:SetData(data.reward_type == 0 and Item.Create(data.reward_item) or Item.Init(HuoBi[CurrencyType.Gold], data.gold_num))
    end
    UH.SetText(self.RateText, string.format(Language.Common.Percent, string.format("%0.2f", data.show_rate * 100)))
end