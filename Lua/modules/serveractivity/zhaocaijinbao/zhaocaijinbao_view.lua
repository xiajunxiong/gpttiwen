ActZhaoCaiJinBaoView = ActZhaoCaiJinBaoView or DeclareView("ActZhaoCaiJinBaoView", "serveractivity/zhaocaijinbao/act_zhaocaijinbao", Mod.RandActivity.ZhaoCaiJinBao)

function ActZhaoCaiJinBaoView:ActZhaoCaiJinBaoView()
    self.data = ActZhaoCaiJinBaoData.Instance
end

function ActZhaoCaiJinBaoView:LoadCallback()
    self.SkipToggle.isOn = self.data.zhaocai_smart.skip_rolling
    local show_item_list = self.data:GetShowList()
    if #show_item_list > 1 then
        self.auto_change_runner = TimeHelper:AddRunTimer(function ()
            if self.data.zhaocai_smart.show_index + 1 > #show_item_list then
                self.data.zhaocai_smart.show_index = 1
            else
                self.data.zhaocai_smart.show_index = self.data.zhaocai_smart.show_index + 1
            end
        end,5,nil,false)
    end
    BagData.Instance:SetHideCurrencyCenterFlag(true)
end

function ActZhaoCaiJinBaoView:CloseCallback()
    if self.auto_change_runner ~= nil then
        TimeHelper:CancelTimer(self.auto_change_runner)
    end
    BagData.Instance:SetHideCurrencyCenterFlag(false)
    self.data.draw_result_item = nil
end

function ActZhaoCaiJinBaoView:OnClickBuyStoreBtn()
    ViewMgr:OpenView(ActZhaoCaiJinBaoStoreView)
end

function ActZhaoCaiJinBaoView:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[181].desc})
end

function ActZhaoCaiJinBaoView:OnClickPreview()
    ViewMgr:OpenView(ActZhaoCaiJinBaoPreview)
    -- ActZhaoCaiJinBaoData.Instance:SetDrawResult({draw_result = 7})
end

function ActZhaoCaiJinBaoView:OnClickRecord()
    ActZhaoCaiJinBaoCtrl.Instance:SendZhaoCaiJinBaoReq(ActZhaoCaiJinBaoCtrl.REQ_TYPE.REQ_GET_RECORD)
    ActZhaoCaiJinBaoCtrl.Instance.open_record_view = true
end

function ActZhaoCaiJinBaoView:OnClickClose()
    ViewMgr:CloseView(ActZhaoCaiJinBaoView)
end

function ActZhaoCaiJinBaoView:OnClickArrow(param)
    if self.auto_change_runner ~= nil then
        TimeHelper:CancelTimer(self.auto_change_runner)
    end
    local show_item_list = self.data:GetShowList()
    if self.data.zhaocai_smart.show_index + param > #show_item_list then
        self.data.zhaocai_smart.show_index = 1
    elseif self.data.zhaocai_smart.show_index + param <= 0 then
        self.data.zhaocai_smart.show_index = #show_item_list
    else
        self.data.zhaocai_smart.show_index = self.data.zhaocai_smart.show_index + param
    end
    self.auto_change_runner = TimeHelper:AddRunTimer(function ()
        if self.data.zhaocai_smart.show_index + 1 > #show_item_list then
            self.data.zhaocai_smart.show_index = 1
        else
            self.data.zhaocai_smart.show_index = self.data.zhaocai_smart.show_index + 1
        end
    end,5,nil,false)
end

----------------------------ActZhaoCaiJinBaoPanel----------------------------
ActZhaoCaiJinBaoPanel = ActZhaoCaiJinBaoPanel or DeclareMono("ActZhaoCaiJinBaoPanel", UIWFlushPanel)
function ActZhaoCaiJinBaoPanel:ActZhaoCaiJinBaoPanel()
    self.data = ActZhaoCaiJinBaoData.Instance
    self.data_cares = {
        {data = self.data.zhaocai_smart, func = self.FlushPoolList, keys = {"flush"}},
        {data = self.data.zhaocai_smart, func = self.FlushModelUI, keys = {"show_index"}},
        {data = self.data.zhaocai_smart, func = self.FlushRewardFlags, keys = {"reward_flag"}},
        {data = self.data.zhaocai_smart, func = self.ShowRolling, keys = {"draw_result"}, init_call = false},
        {data = BagData.Instance.item_grid[ItemColumnType.Material], func = self.FlushStarInfo, init_call = false},
    }
end

function ActZhaoCaiJinBaoPanel:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetText(self.FreshCostTxt, self.data:GetRefreshCost())
    self:FlushStarInfo()
    self.NextFreshTimeMeter:SetCallBack(function ()
        ActZhaoCaiJinBaoCtrl.Instance:SendZhaoCaiJinBaoReq(ActZhaoCaiJinBaoCtrl.REQ_TYPE.REQ_INFO)
    end)
    self.LeftTimeMeter:SetCallBack(function ()
        ViewMgr:CloseView(ActZhaoCaiJinBaoView)
    end)
    
end

function ActZhaoCaiJinBaoPanel:FlushRewardFlags()
    local flag = self.data:GetRewardFlag()
    if flag > 0 then
        if self.reward_red == nil then
            self.reward_red = UiRedPoint.New(self.BuyBtnRedParendObj, Vector3.New(60,16,0))
        end
        self.reward_red:SetNum(1)
    elseif self.reward_red ~= nil then
        self.reward_red:SetNum(0)
    end
end

function ActZhaoCaiJinBaoPanel:FlushModelUI()
    local show_item_list = self.data:GetShowList()
    if show_item_list[self.data.zhaocai_smart.show_index] ~= nil then
        self.ModelShow:SetData({item_config = show_item_list[self.data.zhaocai_smart.show_index]})
    end
end

function ActZhaoCaiJinBaoPanel:FlushPoolList()
    local item_list = self.data:GetPoolList()
    for i = 1, self.ItemList:Length() do
        self.ItemList[i]:SetData(item_list[i])
    end
    self.NextFreshTimeMeter:StampTime(self.data.next_refresh_timestamp, TimeType.Type_Time_1)
    self.LeftTimeMeter:StampTime(ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_ZHAOCAIJINBAO).next_status_switch_time, TimeType.Type_Time_3)
    for i = 1, 10 do
        self.SelectToggles[i].isOn = false
    end
end

function ActZhaoCaiJinBaoPanel:FlushStarInfo()
    local star_count = self.data:GetStarCount()
    UH.SetText(self.StarCountTxt, star_count)
    UH.SetText(self.DrawBtnText, star_count >= 10 and Language.ZhaoCaiJinBao.DrawTen or Language.ZhaoCaiJinBao.DrawOnce)
end

function ActZhaoCaiJinBaoPanel:OnDestroy()
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

function ActZhaoCaiJinBaoPanel:ShowRolling()
    self.show_rolling = true
    if self.data.zhaocai_smart.skip_rolling then
        self.SelectToggles[self.data.zhaocai_smart.draw_result].isOn = true
        if self.data.draw_result_item ~= nil then
            self.delay_show_result = TimeHelper:AddDelayTimer(function ()
                TimeHelper:AddDelayTimer(function ()
                    self.show_rolling = false
                end, 0.3)
                ActRewardTipData.Instance:OpenActRewardTipViewByData(ServeractivityConfig.ActRewerdTip.RewardType.Success,{self.data.draw_result_item})
            end, 0.3)
        end
    else
        local slow_index = self.data.zhaocai_smart.draw_result > 5 and self.data.zhaocai_smart.draw_result - 5 or self.data.zhaocai_smart.draw_result + 5
        self.roll_index = 1
        local update_func = function ()
            self.SelectToggles[self.roll_index].isOn = true
            self.roll_index = self.roll_index + 1 > MAX_RA_ZHAOCAIJINBAO_REFRESH_NUM and 1 or self.roll_index + 1
        end
        local complete_func = function()
            self.rolling_slow_runner = TimeHelper:AddCountDownTT(update_func,
            function ()
                self.SelectToggles[self.data.zhaocai_smart.draw_result].isOn = true
                if self.data.draw_result_item ~= nil then
                    -- self.delay_show_result = TimeHelper:AddDelayTimer(function ()
                        self.show_rolling = false
                        ActRewardTipData.Instance:OpenActRewardTipViewByData(ServeractivityConfig.ActRewerdTip.RewardType.Success,{self.data.draw_result_item})
                    -- end, 0.3)
                end
            end, 6 * 0.4, 0.4, false)
        end
        self.rolling_runner = TimeHelper:AddCountDownTT(update_func, complete_func, 0.1 * (19 + slow_index), 0.1)
    end
end

function ActZhaoCaiJinBaoPanel:OnClickDraw()
    if self.show_rolling then
        PublicPopupCtrl.Instance:Center(Language.ZhaoCaiJinBao.ShowRolling)
        return
    end
    local star_count = self.data:GetStarCount()
    if star_count <= 0 then
        PublicPopupCtrl.Instance:Center(Language.ZhaoCaiJinBao.NotEnoughItem)
        ViewMgr:OpenView(ActZhaoCaiJinBaoStoreView)
        return
    elseif star_count >= 10 then
        ViewMgr:OpenView(ActZhaoCaiJinBaoConfirmDialog)
        return
    end
    self.data.draw_result_item = nil
    ActZhaoCaiJinBaoCtrl.Instance:SendZhaoCaiJinBaoReq(ActZhaoCaiJinBaoCtrl.REQ_TYPE.REQ_DRAW, 1)
end


function ActZhaoCaiJinBaoPanel:OnClickFreshBtn()
    if self.show_rolling then
        PublicPopupCtrl.Instance:Center(Language.ZhaoCaiJinBao.ShowRollingCannotFresh)
        return
    elseif RoleData.Instance:GetCurrency(CurrencyType.Gold) < self.data:GetRefreshCost() then
        local info = {
            content = Language.Mall.NotCurrency[3],
            close = true,
            cancel = {
                name = Language.Mall.NotCurrency[4],
                func = function()
                    TradeData.Instance:SetJumpItemData(Item.Init())
                    ViewMgr:OpenViewByKey(Mod.Mall.Trade)
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
            },
            confirm = {
                name = Language.Mall.NotCurrency[5],
                func = function()
                    ViewMgr:OpenViewByKey(Mod.Store.Recharge)
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
            },
        }
        BehaviorData.Instance.auto_behavior = 0
        if GLOBAL_CONFIG:Flags().block_recharge then
            info.no_confirm = true
        end
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end
    ActZhaoCaiJinBaoCtrl.Instance:SendZhaoCaiJinBaoReq(ActZhaoCaiJinBaoCtrl.REQ_TYPE.REQ_REFRESH)
end

function ActZhaoCaiJinBaoPanel:OnClickSkipRolling(toggle)
    self.data.zhaocai_smart.skip_rolling = toggle
end

------进宝商城界面view-----------------------
ActZhaoCaiJinBaoStoreView = ActZhaoCaiJinBaoStoreView or DeclareView("ActZhaoCaiJinBaoStoreView", "serveractivity/zhaocaijinbao/act_zhaocaijinbao_store")
function ActZhaoCaiJinBaoStoreView:ActZhaoCaiJinBaoStoreView()
    self.data = ActZhaoCaiJinBaoData.Instance
    self.reward_red_care_handle = self.data.zhaocai_smart:KeysCare({"reward_flag"}, function ()
        self:FlushRewardFlags()
    end)
end

function ActZhaoCaiJinBaoStoreView:LoadCallback()
    self.Currency:DefaultSet()
    self:FlushRewardFlags()
end

function ActZhaoCaiJinBaoStoreView:FlushRewardFlags()
    local flag = self.data:GetRewardFlag()
    if flag > 0 then
        if self.reward_red == nil then
            self.reward_red = UiRedPoint.New(self.StoreRewardToggleObj, Vector3.New(73,18,0))
        end
        self.reward_red:SetNum(1)
    elseif self.reward_red ~= nil then
        self.reward_red:SetNum(0)
    end
end

function ActZhaoCaiJinBaoStoreView:CloseCallback()
    self.data.direct_cache_cfg = nil
    self.data.zhaocai_smart:Uncare(self.reward_red_care_handle)
end

function ActZhaoCaiJinBaoStoreView:OnClickClose()
    ViewMgr:CloseView(ActZhaoCaiJinBaoStoreView)
end

--------进宝商城商品界面----------------------------------------------
ActZhaoCaiJinBaoStoreItemPanel = ActZhaoCaiJinBaoStoreItemPanel or DeclareMono("ActZhaoCaiJinBaoStoreItemPanel", UIWFlushPanel)
function ActZhaoCaiJinBaoStoreItemPanel:ActZhaoCaiJinBaoStoreItemPanel()
    self.data = ActZhaoCaiJinBaoData.Instance
    self.data_cares = {
        {data = self.data.zhaocai_smart, func = self.FlushStoreList, keys = {"storeFlush"}},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function ActZhaoCaiJinBaoStoreItemPanel:FlushStoreList()
    local store_sort_list = self.data:GetStoreInfoList()
    table.sort(store_sort_list,function (a,b)
        if a.buy_max == b.buy_max then
            return a.seq < b.seq
        else
            return not a.buy_max
        end
    end)
    -- LogError("store_sort_list",store_sort_list)
    if store_sort_list then
        self.StoreItemList:SetData(store_sort_list)
    end
end

function ActZhaoCaiJinBaoStoreItemPanel:SendBuy()
    if self.data.direct_cache_cfg then
        ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_ZHAOCAIJINBAO, self.data.direct_cache_cfg.buy_money)
        self.data.direct_cache_cfg = nil
        -- ViewMgr:CloseView(ActZhaoCaiJinBaoStoreView)
    end
end

-----进宝商城商品item---------
ActZhaoCaiJinBaoStoreItem = ActZhaoCaiJinBaoStoreItem or DeclareMono("ActZhaoCaiJinBaoStoreItem", UIWidgetBaseItem)
function ActZhaoCaiJinBaoStoreItem:ActZhaoCaiJinBaoStoreItem()
    self.data_instance = ActZhaoCaiJinBaoData.Instance
    self.buy_money_rate = {[-1] = 0.1,[0] = 1,[16] = 1}
end

function ActZhaoCaiJinBaoStoreItem:SetData(data)
    self.item = Item.Create(data.reward_item[0])
    self.Item:SetData(self.item)
    UH.SetText(self.BuyLimitNumText, string.format(Language.ZhaoCaiJinBao.LimitType[data.limit_type], data.buy_max and COLORSTR.Red8 or COLORSTR.Green2, data.buy_times - data.buy_count, data.buy_times))
    UH.SetText(self.ItemName, self.item:Name())
    self.BtnObj:SetActive(not data.buy_max)
    if not data.buy_max then
        UH.SetText(self.BuyBtnText, data.buy_money * self.buy_money_rate[data.price_type])
        UH.SpriteName(self.CurrencyImg,HuoBi[data.price_type >= 0 and data.price_type or 999])
    end
    self.data = data
end

function ActZhaoCaiJinBaoStoreItem:OnClickBuy()
    if self.data.price_type == -1 then
        self.data_instance.direct_cache_cfg = self.data
        local fee = self.data.buy_money * self.buy_money_rate[self.data.price_type]
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_ZHAOCAIJINBAO, fee, ACTIVITY_TYPE.RAND_ZHAOCAIJINBAO, ActZhaoCaiJinBaoCtrl.REQ_TYPE.REQ_BUY_ITEM_SHOP, self.data.seq)
    else
        ActZhaoCaiJinBaoCtrl.Instance:SendZhaoCaiJinBaoReq(ActZhaoCaiJinBaoCtrl.REQ_TYPE.REQ_BUY_ITEM_DIRECT, self.data.seq)
    end
end
------进宝商城奖励界面--------------------------------------
ActZhaoCaiJinBaoStoreRewardPanel = ActZhaoCaiJinBaoStoreRewardPanel or DeclareMono("ActZhaoCaiJinBaoStoreRewardPanel", UIWFlushPanel)
function ActZhaoCaiJinBaoStoreRewardPanel:ActZhaoCaiJinBaoStoreRewardPanel()
    self.data = ActZhaoCaiJinBaoData.Instance
    self.data_cares = {
        {data = self.data.zhaocai_smart, func = self.FlushRewardList, keys = {"storeFlush"}}
    }
end

function ActZhaoCaiJinBaoStoreRewardPanel:FlushRewardList()
    local reward_sort_list = self.data:GetRewardInfoList()
    table.sort(reward_sort_list,function (a,b)
        if a.get_max == b.get_max then
            return a.seq < b.seq
        else
            return not a.get_max
        end
    end)
    if reward_sort_list then
        self.RewardList:SetData(reward_sort_list)
    end
    
end
------进宝商城奖励item----------
ActZhaoCaiJinBaoRewardItem = ActZhaoCaiJinBaoRewardItem or DeclareMono("ActZhaoCaiJinBaoRewardItem", UIWidgetBaseItem)
function ActZhaoCaiJinBaoRewardItem:ActZhaoCaiJinBaoRewardItem()
    self.data_instance = ActZhaoCaiJinBaoData.Instance
end

function ActZhaoCaiJinBaoRewardItem:SetData(data)
    self.data = data
    self.SingleChargeObj:SetActive(data.top_up_type == 0)
    self.LeiJiObj:SetActive(data.top_up_type == 1)
    self.TodayGetTextObj:SetActive(data.top_up_type == 0)
    if data.top_up_type == 0 then
        UH.SetText(self.TodayCanGetText, string.format(Language.ZhaoCaiJinBao.RewardGet[data.get_type], data.get_max and COLORSTR.Red8 or COLORSTR.Green2, data.get_times - data.has_get_reward, data.get_times))
    end
    UH.SetText(self.CostText, data.top_up_num..Language.Common.Gold)
    local btn_str = Language.ZhaoCaiJinBao.CanGet
    if data.get_max then
        btn_str = Language.ZhaoCaiJinBao.GetMax
    elseif data.can_get_reward <= 0 then
        btn_str = Language.ZhaoCaiJinBao.CannotGet
    end
    UH.SetText(self.BuyBtnText, btn_str)
    self.RewardItemList:SetData(data.reward_list)
    self.Interactor.Interactable = self.data.can_get_reward > 0 and not self.data.get_max
    if self.data.can_get_reward > 0 and not self.data.get_max then
        if self.red_point == nil then
            self.red_point = UiRedPoint.New(self.BtnObj, Vector3.New(78, 24, 0))
        end
        self.red_point:SetNum(1)
    elseif self.red_point ~= nil then
        self.red_point:SetNum(0)
    end
    if not self.data.get_max and self.data.has_get_reward < self.data.can_get_reward then
        if self.btn_effect_handle == nil then
            self.btn_effect_handle = self.EffectTool:Play(2165048)
        end
    elseif self.btn_effect_handle ~= nil then
        self.EffectTool:Stop(self.btn_effect_handle)
        self.btn_effect_handle = nil
    end
end

function ActZhaoCaiJinBaoRewardItem:OnClickGetReward()
    if self.data.get_max then
        PublicPopupCtrl.Instance:Center(Language.ZhaoCaiJinBao.GetAllReward)
    elseif self.data.can_get_reward > 0 then
        ActZhaoCaiJinBaoCtrl.Instance:SendZhaoCaiJinBaoReq(ActZhaoCaiJinBaoCtrl.REQ_TYPE.REQ_GET_CHONGZHI_REWARD, self.data.seq)
    else
        PublicPopupCtrl.Instance:Center(Language.ZhaoCaiJinBao.CannotGetReward)
    end
end

--------预览界面-----------------------
ActZhaoCaiJinBaoPreview = ActZhaoCaiJinBaoPreview or DeclareView("ActZhaoCaiJinBaoPreview","serveractivity/zhaocaijinbao/act_zhaocaijinbao_preview")
VIEW_DECLARE_MASK(ActZhaoCaiJinBaoPreview,ViewMask.BgBlock)
function ActZhaoCaiJinBaoPreview:ActZhaoCaiJinBaoPreview()
    self.data = ActZhaoCaiJinBaoData.Instance
end

function ActZhaoCaiJinBaoPreview:LoadCallback()
    self.Board:SetData(ActZhaoCaiJinBaoPreview, Language.ZhaoCaiJinBao.PreviewTitle,Vector2.New(546, 410))
    self.ItemList:SetData(self.data:GetSectionPoolCfgList())
end
---------预览界面item------------------
ActZhaoCaiJinBaoPreviewItem = ActZhaoCaiJinBaoPreviewItem or DeclareMono("ActZhaoCaiJinBaoPreviewItem",UIWidgetBaseItem)
function ActZhaoCaiJinBaoPreviewItem:ActZhaoCaiJinBaoPreviewItem()
end

function ActZhaoCaiJinBaoPreviewItem:SetData(data)
    self.data = data
    self.ItemCell:SetData(data.item)
    UH.SetText(self.RateText, data.rate)
end
-----抽奖记录界面-----------------------------------
ActZhaoCaiJinBaoRecordView = ActZhaoCaiJinBaoRecordView or DeclareView("ActZhaoCaiJinBaoRecordView","serveractivity/zhaocaijinbao/act_zhaocaijinbao_record")
function ActZhaoCaiJinBaoRecordView:ActZhaoCaiJinBaoRecordView()
    self.data = ActZhaoCaiJinBaoData.Instance
end

function ActZhaoCaiJinBaoRecordView:LoadCallback()
    local record_list = self.data:GetRecordList()
    self.EmptyObj:SetActive(#record_list <= 0)
    self.RecordList:SetData(record_list)
end

function ActZhaoCaiJinBaoRecordView:OnClickClose()
    ViewMgr:CloseView(ActZhaoCaiJinBaoRecordView)
end
------抽奖记录信息item-------------------------------
ActZhaoCaiJinBaoRecordItem = ActZhaoCaiJinBaoRecordItem or DeclareMono("ActZhaoCaiJinBaoRecordItem", UIWidgetBaseItem)
function ActZhaoCaiJinBaoRecordItem:ActZhaoCaiJinBaoRecordItem()
end

function ActZhaoCaiJinBaoRecordItem:SetData(data)
    self.data = data
    local t = TimeManager.FormatUnixTime2Date(data.times)
    if data.item_cfg then
        local str_color = QualityColorStrShallow[data.item_cfg.color]
        UH.SetText(self.RecordText, string.format(Language.ZhaoCaiJinBao.RecordFormat, t.month, t.day, t.hour, t.minute, t.second, str_color, data.item_cfg.name, data.num))
    elseif IS_EDITOR then
        LogError("招财进宝抽奖记录界面 未找到物品配置 item_id:",data.item_id)
    end
end
--------十连确认框
ActZhaoCaiJinBaoConfirmDialog = ActZhaoCaiJinBaoConfirmDialog or DeclareView("ActZhaoCaiJinBaoConfirmDialog", "serveractivity/zhaocaijinbao/act_zhaocaijinbao_mult_confirm")
VIEW_DECLARE_MASK(ActZhaoCaiJinBaoConfirmDialog, ViewMask.BgBlockClose)
function ActZhaoCaiJinBaoConfirmDialog:ActZhaoCaiJinBaoConfirmDialog()
    self.data = ActZhaoCaiJinBaoData.Instance
end

function ActZhaoCaiJinBaoConfirmDialog:LoadCallback()
    local star_count, star_cfg = ActZhaoCaiJinBaoData.Instance:GetStarCount()
    if star_cfg then
        UH.SetText(self.Text, string.format(Language.ZhaoCaiJinBao.MultConfirmFormat, QualityColorStrShallow[star_cfg.color], star_cfg.name, star_count))
    end
end

function ActZhaoCaiJinBaoConfirmDialog:OnClickDraw(times)
    self.data.draw_result_item = nil
    self.data.draw_result_item_list = nil
    ActZhaoCaiJinBaoCtrl.Instance:SendZhaoCaiJinBaoReq(ActZhaoCaiJinBaoCtrl.REQ_TYPE.REQ_DRAW, times)
    ViewMgr:CloseView(ActZhaoCaiJinBaoConfirmDialog)
end