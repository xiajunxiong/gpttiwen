--丝绸之路，贸易之路。
FILE_REQ("modules/trade_road/trade_road_config")
FILE_REQ("modules/trade_road/trade_road_data")

VIEW_REQ("modules/trade_road/trade_road_begin_view")
VIEW_REQ("modules/trade_road/trade_road_view")
VIEW_REQ("modules/trade_road/trade_road_market_view")
VIEW_REQ("modules/trade_road/trade_road_rank_view")
VIEW_REQ("modules/trade_road/trade_road_tavern_view")
VIEW_REQ("modules/trade_road/trade_road_record_view")
VIEW_REQ("modules/trade_road/trade_road_reward_view")
VIEW_REQ("modules/trade_road/trade_road_map_view")
VIEW_REQ("modules/trade_road/trade_road_buy_view")
VIEW_REQ("modules/trade_road/trade_road_sale_view")
VIEW_REQ("modules/trade_road/trade_road_buy_bag_view")
VIEW_REQ("modules/trade_road/trade_road_buy_supply_view")
VIEW_REQ("modules/trade_road/trade_road_item")

TradeRoadCtrl = TradeRoadCtrl or BaseClass(BaseCtrl)
function TradeRoadCtrl:__init()
    if TradeRoadCtrl.Instance ~= nil then
        Debuger.LogError("[TradeRoadCtrl] attempt to create singleton twice!")
        return
    end
    TradeRoadCtrl.Instance = self
    self.data = TradeRoadData.New()
    self:RegisterAllProtocols()
end

function TradeRoadCtrl:__delete()
    if TradeRoadCtrl.Instance == self then
        TradeRoadCtrl.Instance = nil
    end
end

function TradeRoadCtrl:OnInit()
    BehaviorData.Instance:CustomNpcTalkBtnsSeq(self.data:GetAcceptNpcSeq(), BindTool.Bind(self.SetupBehaviourBtn, self))
    ActivityData.Instance:CustomClickHandle(ActType.TradeRoad, function()
        TeamData.Instance:CheckSingle(function ()
            if not LoginData.Instance:IsOnCrossSever() then
                LoginCtrl.Instance:SendCrossStartReq()
            end
            Invoke(function()
                SceneLogic.Instance:AutoToNpc(self.data:GetAcceptNpcSeq(), nil, false, true)
            end, LoginData.Instance:IsOnCrossSever() and 0.1 or 2)
        end,Language.TradeRoad.ActTeamTip)
       --[[  if TeamData.Instance:InTeam() then
            PublicPopupCtrl.Instance:Center(Language.Fight.GatherTeamTips)
            return
        end ]]
        --[[ if not LoginData.Instance:IsOnCrossSever() then
            LoginCtrl.Instance:SendCrossStartReq()
        end
        Invoke(function()
            SceneLogic.Instance:AutoToNpc(self.data:GetAcceptNpcSeq(), nil, false, true)
        end, LoginData.Instance:IsOnCrossSever() and 0.1 or 2) ]]
    end)
    local act_status = ActivityData.Instance:GetActivityInfo(ActType.TradeRoad)
    if act_status ~= nil then
        act_status:Care(BindTool.Bind1(self.OnActEvent, self), "act_type", "status")
    end
end

function TradeRoadCtrl:OnUnloadGameLogic()
    self.data.road_info:Clear()
end

function TradeRoadCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSActivitySilkRoadsReq)
    self:RegisterProtocol(SCActivitySilkRoadSimpleInfo, "OnSCActivitySilkRoadSimpleInfo")
    self:RegisterProtocol(SCActivitySilkRoadsRoleAllInfo, "OnSCActivitySilkRoadsRoleAllInfo")
    self:RegisterProtocol(SCActivitySilkRoadsAssetsInfo, "OnSCActivitySilkRoadsAssetsInfo")
    self:RegisterProtocol(SCActivitySilkRoadsWarehouseChange, "OnSCActivitySilkRoadsWarehouseChange")
    self:RegisterProtocol(SCActivitySilkRoadsAllRankInfo, "OnSCActivitySilkRoadsAllRankInfo")
    self:RegisterProtocol(SCActivitySilkRoadsAllTradingLogInfo, "OnSCActivitySilkRoadsAllTradingLogInfo")
    self:RegisterProtocol(SCActivitySilkRoadsSingleTradingLogNotice, "OnSCActivitySilkRoadsSingleTradingLogNotice")
end

function TradeRoadCtrl:OnActEvent()
    local act_state = ActivityData.Instance:GetActivityStatus(ActType.TradeRoad)
    if act_state == ActStatusType.Close then
        ViewMgr:CloseView(TradeRoadBeginView)
        ViewMgr:CloseView(TradeRoadView)
        ViewMgr:CloseView(TradeRoadMapView)
        ViewMgr:CloseView(TradeRoadBuyBagView)
        ViewMgr:CloseView(TradeRoadBuySupplyView)
        ViewMgr:CloseView(TradeRoadBuyView)
        ViewMgr:CloseView(TradeRoadSaleView)
        self.data.road_info:Clear()
    end
end

function TradeRoadCtrl:SendSilkRoadsReq(op_type, param1, param2)
    local protocol = GetProtocol(CSActivitySilkRoadsReq)
    protocol.op_type = op_type
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    SendProtocol(protocol)
end

function TradeRoadCtrl:OnSCActivitySilkRoadSimpleInfo(protocol)
    if protocol.simple_type == TradeRoadConfig.SMIPLE_TYPE.TYPE_READY then
        self.data:OnReady(protocol)
    elseif protocol.simple_type == TradeRoadConfig.SMIPLE_TYPE.TYPE_START_OUTSET then
        self.data:OnStartOutset(protocol)
        self:OpenTradeRoadView()
        self:DelayOpenNpc(1)
    elseif protocol.simple_type == TradeRoadConfig.SMIPLE_TYPE.TYPE_KEEP_GOING then
        self.data:OnKeepGoing(protocol)
        self:DelayOpenNpc()
    elseif protocol.simple_type == TradeRoadConfig.SMIPLE_TYPE.TYPE_BUY_NEWS then
        self.data:OnBuyNews(protocol)
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.BuySuccess)
    elseif protocol.simple_type == TradeRoadConfig.SMIPLE_TYPE.TYPE_EXPANSION_WAREHOUSE then
        self.data:OnBuyBag(protocol)
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.BuySuccess)
    elseif protocol.simple_type == TradeRoadConfig.SMIPLE_TYPE.TYPE_BUY_SUPPLY then
        self.data:OnBuySupply(protocol)
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.BuySuccess)
    elseif protocol.simple_type == TradeRoadConfig.SMIPLE_TYPE.TYPE_END then
        ViewMgr:CloseView(TradeRoadMapView)
        -- ViewMgr:CloseView(TradeRoadView)
        TradeRoadData.Instance:SetOverSmart()
        self.data:OnQuitGame(protocol)
        self:SendAllInfoReq()
        Invoke(BindTool.Bind1(self.SendRankListReq, self), 0.5)
    end
end

function TradeRoadCtrl:OnSCActivitySilkRoadsRoleAllInfo(protocol)
    self.data:SetAllInfo(protocol)
end

function TradeRoadCtrl:OnSCActivitySilkRoadsAssetsInfo(protocol)
    self.data:SetCurrencyInfo(protocol)
end

function TradeRoadCtrl:OnSCActivitySilkRoadsWarehouseChange(protocol)
    if protocol.notice_type == SILK_ROADS_NOTICE_TYPE.TYPE_REWARD then
        local old_data = self.data:GetBagItemByIndex(protocol.grid_index)
        local old_num = old_data ~= nil and old_data.commodity_count or 0
        TradeRoadData.Instance:AddShowItem({seq = protocol.commodity_seq, num = protocol.commodity_count - old_num})
    elseif protocol.notice_type == SILK_ROADS_NOTICE_TYPE.TYPE_BUY then
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.NoticeType[protocol.notice_type])
    elseif protocol.notice_type == SILK_ROADS_NOTICE_TYPE.TYPE_SALE then
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.NoticeType[protocol.notice_type])
    end
    self.data:OnGridChange(protocol)
end

function TradeRoadCtrl:SendAllInfoReq()
    self:SendSilkRoadsReq(TradeRoadConfig.REQ_OP_TYPE.TYPE_INFO)
end

-----排行榜请求|下发
function TradeRoadCtrl:SendRankListReq()
    self:SendSilkRoadsReq(TradeRoadConfig.REQ_OP_TYPE.TYPE_RANK_ALL_INFO)
end
function TradeRoadCtrl:OnSCActivitySilkRoadsAllRankInfo(protocol)
    self.data:SetRankInfo(protocol)
end

-----日志信息请求|下发
function TradeRoadCtrl:SendRecordListReq()
    self:SendSilkRoadsReq(TradeRoadConfig.REQ_OP_TYPE.TYPE_ALL_TRADING_INFO)
end
function TradeRoadCtrl:OnSCActivitySilkRoadsAllTradingLogInfo(protocol)
    self.data:SetRecordList(protocol)
end
function TradeRoadCtrl:OnSCActivitySilkRoadsSingleTradingLogNotice(protocol)
    self.data:AddRecordInfo(protocol)
end

function TradeRoadCtrl:DelayOpenNpc(delay_time)
    Invoke(BindTool.Bind1(self.ShowNpcDialog, self), delay_time or 0.5)
end

function TradeRoadCtrl:ShowNpcDialog()
    local city_cfg = self.data:GetCurCityCfg()
    --#1169707 LuaException
    if city_cfg == nil then
        return
    end
    local param_t = {talk_list = {}}
    local up_items = self.data:GetTargetCityRecordList(city_cfg.seq, {[SILK_ROADS_LOG_TYPE.TYPE_DOUBLE_UP] = true})
    if #up_items == 1 then
        --当只有一个大涨物品的时候，显示“欢迎来到XXX，我们城市里的XXX物品，价格已经涨了XX%了”
        local info = up_items[1]
        local item_cfg = self.data:GetItemCfg(info.commodity_seq)
        local rate_val = self.data:GetItemPriceRate(info.target_city_map_seq, info.commodity_seq)
        local talk_text = Format(Language.TradeRoad.WelcomeTalk1, city_cfg.city_name, item_cfg.freight_name, rate_val)
        table.insert(param_t.talk_list, {talker = city_cfg.npc_id, talk_text = talk_text})
    else
        --有多个大涨或没有大涨的时候，都显示“到目前为止，少侠的玉币数量一共有XXX，请再接再厉。”
        local total_price = self.data:GetRewardPrice()
        table.insert(param_t.talk_list, {talker = city_cfg.npc_id, talk_text = Format(Language.TradeRoad.WelcomeTalk2, total_price)})
    end
    NpcDialogCtrl.Instance:NpcDialogView(param_t)
    
    --奖励弹窗
    local has_reward = TradeRoadData.Instance:HasReward()
    if has_reward then
        local function func_delay()
            if not ViewMgr:IsOpen(TradeRoadItemShowView) and TradeRoadData.Instance:HasReward() then
                ViewMgr:OpenView(TradeRoadItemShowView)
            end
        end
        Invoke(func_delay, 0.3)
    end
end

-----npc自定义按钮
function TradeRoadCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    return self:SetupBehaviourBtn2(btn_list, obj_vo)
end
function TradeRoadCtrl:SetupBehaviourBtn2(btn_list, obj_vo)
    if nil == obj_vo or not LoginData.Instance:IsOnCrossSever() then
        return
    end
    if obj_vo.seq == self.data:GetAcceptNpcSeq() and FunOpen.Instance:GetFunIsOpened(Mod.TradeRoad.Main) then
        local function func_rank()
            if TeamData.Instance:InTeam() then
                PublicPopupCtrl.Instance:Center(Language.Fight.GatherTeamTips)
                return
            end
            self:OpenTradeRoadView(Mod.TradeRoad.Rank)
        end
        if ActivityData.IsOpen(ActType.TradeRoad) then
            table.insert(btn_list, 1, NpcDialogBehBtnData.FuncCreate(Language.TradeRoad.NpcBtn2, func_rank, true, nil))
        end
        local function func_begin()
            if TeamData.Instance:InTeam() then
                PublicPopupCtrl.Instance:Center(Language.Fight.GatherTeamTips)
                return
            end
            if TradeRoadData.Instance:IsJoined() then
                self:OpenTradeRoadView()
            elseif self.data:JoinTimes() < self.data:CanJoinTimes() then
                if TradeRoadData.Instance:CurBuffId() == 0 then
                    TradeRoadCtrl.Instance:SendReady()
                end
                ViewMgr:OpenView(TradeRoadBeginView)
            else
                PublicPopupCtrl.Instance:Center(Language.TradeRoad.ActTimesError1)
            end
        end
        if ActivityData.IsOpen(ActType.TradeRoad) then
            table.insert(btn_list, 1, NpcDialogBehBtnData.FuncCreate(Language.TradeRoad.NpcBtn1, func_begin, true, nil))
        end
    end
end

--准备，开始出发，继续出发
function TradeRoadCtrl:SendReady()
    self:SendSilkRoadsReq(TradeRoadConfig.REQ_OP_TYPE.TYPE_READY)
end
function TradeRoadCtrl:SendStartOutset()
    self:SendSilkRoadsReq(TradeRoadConfig.REQ_OP_TYPE.TYPE_START_OUTSET)
end
function TradeRoadCtrl:SendGoNextCityReq()
    self:SendSilkRoadsReq(TradeRoadConfig.REQ_OP_TYPE.TYPE_KEEP_GOING)
end

--购买货物，出售货物
function TradeRoadCtrl:SendBuyItemReq(seq, count)
    self:SendSilkRoadsReq(TradeRoadConfig.REQ_OP_TYPE.TYPE_BUY_COMMODITY, seq, count)
end
function TradeRoadCtrl:SendSaleItemReq(seq, count)
    self:SendSilkRoadsReq(TradeRoadConfig.REQ_OP_TYPE.TYPE_SALE_COMMODITY, seq, count)
end

--购买背包，购买体力
function TradeRoadCtrl:SendBuyBagReq(count)
    self:SendSilkRoadsReq(TradeRoadConfig.REQ_OP_TYPE.TYPE_EXPANSION_WAREHOUSE, count)
end
function TradeRoadCtrl:SendBuySupplyReq(count)
    self:SendSilkRoadsReq(TradeRoadConfig.REQ_OP_TYPE.TYPE_BUY_SUPPLY, count)
end

function TradeRoadCtrl:SendFinshReq()
    self:SendSilkRoadsReq(TradeRoadConfig.REQ_OP_TYPE.TYPE_END)
end

function TradeRoadCtrl:SendBuyMessage()
    self:SendSilkRoadsReq(TradeRoadConfig.REQ_OP_TYPE.TYPE_BUY_NEWS)
end

function TradeRoadCtrl.IsEnough(need_money)
    if need_money > TradeRoadData.Instance:CurMoney() then
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.MoneyNoEnough)
        return false
    end
    return true
end

--开始指引，is_force是否强制
function TradeRoadCtrl:GuideStart(is_force)
    is_force = is_force or false
    if UnityPlayerPrefs.GetInt("TradeRoadGuide" .. RoleData.Instance:GetRoleId()) == 0 then
        UnityPlayerPrefs.SetInt("TradeRoadGuide" .. RoleData.Instance:GetRoleId(), 1)
    elseif not is_force then
        return
    end
    GuideManager.Instance:Start(103)
end
function TradeRoadCtrl:IsTradeGuide()
    return UnityPlayerPrefs.GetInt("TradeRoadGuide" .. RoleData.Instance:GetRoleId()) == 0
end

--打开丝绸之路界面
function TradeRoadCtrl:OpenTradeRoadView(mod_key)
    if TradeRoadData.Instance:IsJoined() then
        ViewMgr:OpenView(TradeRoadMapView)
    end
    Invoke(function()
        ViewMgr:OpenViewByKey(mod_key or Mod.TradeRoad.Main)
    end, 0.2)
end
