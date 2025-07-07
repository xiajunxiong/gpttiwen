-- 天衣耀世
TianYiYaoShiView = TianYiYaoShiView or DeclareView("TianYiYaoShiView", "serveractivity/tian_yi_yao_shi/tian_yi_yao_shi", Mod.RandActivity.TianYiYaoShi)
function TianYiYaoShiView:OnCloseClick()
    ViewMgr:CloseView(TianYiYaoShiView)
end

function TianYiYaoShiView:OnShopClick()
    ViewMgr:OpenView(TianYiYaoShiShopView)
end

function TianYiYaoShiView:OnLogClick()
    ViewMgr:OpenView(TianYiYaoShiLogView)
end

function TianYiYaoShiView:OnRateClick()
    ViewMgr:OpenView(TianYiYaoShiRateView)
end

function TianYiYaoShiView:OnTipClick()
    PublicPopupCtrl.Instance:HelpTip(343)
end


TianYiYaoShiPanel = TianYiYaoShiPanel or DeclareMono("TianYiYaoShiPanel", UIWFlushPanel)
function TianYiYaoShiPanel:TianYiYaoShiPanel()
    self.data = TianYiYaoShiData.Instance
    self.act_type = self.data.act_type
    self.show_model_index = 1
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushItem, init_call = false},
    }

    self.ui_obj_show = UIObjDrawer.New()
    self.ModelShow:SetShowData({
        ui_obj = self.ui_obj_show,
        view = self,
        seq = 1,
        show_shadow = false,
    })
    self.ui_role_show = UIChDrawer.New()
    self.RoleModelShow:SetShowData({
        ui_obj = self.ui_role_show,
        view = self,
        seq = 2,
        show_shadow = false,
    })
end

function TianYiYaoShiPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.data.remind_data.first_open_flag = true

    local base_cfg = self.data:GetBaseCfg()
    self.model_list = {}        -- 显示参考下这个：ItemInfoViewModelShow
    self.model_index_list = {}
    for i, v in pairs(base_cfg) do
        local item_id = v.reward_item.item_id
        local model_info = self.data:GetModelInfoByItemId(item_id)
        if model_info then
            table.insert(self.model_list, model_info)
            local len = #self.model_list
            table.insert(self.model_index_list, {index = len})
        end
    end
    self.ModelIndexList:SetData(self.model_index_list)
    self.ModelIndexList:SetectItem(self.show_model_index,true)

    local item_list_data = self.data:GetBaseCfg()
    for i = 1, self.ItemList:Length() do
        if item_list_data[i] then
            self.ItemList[i]:SetData(item_list_data[i])
        end
    end

    self.item_id = self.data:ItemId()
    local icon = Item.GetIconId(self.item_id)
    UH.SetIcon(self.StuffIcon, icon, ICON_TYPE.ITEM)
    UH.SetIcon(self.StuffIcon1, icon, ICON_TYPE.ITEM)
    UH.SetIcon(self.StuffIcon10, icon, ICON_TYPE.ITEM)

    self.TimeMeter:CreateActTime(self.act_type,TimeType.Type_Time_8,Language.TianYiYaoShi.Tip2,Language.TianYiYaoShi.Tip3)
    self:FlushPanel()
end

function TianYiYaoShiPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_func)
    Delete(self, "ui_obj_show")
    Delete(self, "ui_role_show")
    self.data.jump_animation = false
end

function TianYiYaoShiPanel:FlushPanel()
    self:FlushItem()
end

function TianYiYaoShiPanel:FlushItem()
    local num = Item.GetNum(self.item_id)
    UH.SetText(self.StuffNum, num)
end

function TianYiYaoShiPanel:FlushModel()
    if TableIsEmpty(self.model_list) then
        return
    end
    local info = self.model_list[self.show_model_index]
    self.model_info = info
    if #self.model_list > 1 then
        TimeHelper:CancelTimer(self.time_func)
        self.time_func = Invoke(BindTool.Bind(self.UpdateModelIndex,self),5)
    end

    self.ModelShow:SetObjActive(info.show_type == 1)
    self.RoleModelShow:SetObjActive(info.show_type == 2)
    if info.show_type == 1 then
        self.ui_obj_show:SetData(info.data)
    elseif info.show_type == 2 then
        self.ui_role_show:SetData(info.data)
    end
    UH.SetText(self.ModelName, info.name)
end

function TianYiYaoShiPanel:UpdateModelIndex()
    local index = self.show_model_index + 1
    if index > #self.model_index_list then
        index = 1
    end
    self.show_model_index = index
    self.ModelIndexList:SetectItem(self.show_model_index,true)
end

function TianYiYaoShiPanel:OnModelPageClick(data)
    self.show_model_index = data.index
    self:FlushModel()
end

function TianYiYaoShiPanel:OnNextPageClick(num)
    local index = self.show_model_index + num
    if index > #self.model_index_list then
        index = 1
    end
    if index < 1 then
        index = #self.model_index_list
    end
    self.ModelIndexList:SetectItem(index,true)
end

function TianYiYaoShiPanel:OnPlayClick(num)
    num = num or 1
    local item_num = Item.GetNum(self.data:ItemId())
    if item_num < num then
        -- PublicPopupCtrl.Instance:Error(Language.TianYiYaoShi.Tip1)
        -- MainOtherCtrl.Instance:GetWayViewById(self.data:ItemId())
        PublicPopupCtrl.Instance:AlertTip(Language.TianYiYaoShi.Tip10, function ()
            ViewMgr:OpenView(TianYiYaoShiShopView)
        end)
        return
    end
    if num == 10 then
        ServerActivityCtrl.Instance:SendActivityReq(self.act_type,TianYiYaoShiCtrl.OpType.PlayTen)
    else
        ServerActivityCtrl.Instance:SendActivityReq(self.act_type,TianYiYaoShiCtrl.OpType.PlayOne)
    end
end

function TianYiYaoShiPanel:OnAnimationToggleClick(value)
    self.data.jump_animation = value
end

function TianYiYaoShiPanel:OnStuffClick()
    local item = Item.Create({item_id = self.item_id})
    local data_list = {item}
    local info = {
        item_list = data_list,
    }
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function TianYiYaoShiPanel:OnLookModelClick()
    if not self.model_info then
        return
    end
    local item = Item.Create({item_id = self.model_info.item_id})
    local data_list = {item}
    local info = {
        item_list = data_list,
    }
	ItemInfoCtrl.Instance:ItemInfoView(info)
end


TianYiYaoShiModelPageItem = TianYiYaoShiModelPageItem or DeclareMono("TianYiYaoShiModelPageItem", UIWidgetBaseItem)
function TianYiYaoShiModelPageItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
end

TianYiYaoShiPanelItem = TianYiYaoShiPanelItem or DeclareMono("TianYiYaoShiPanelItem", UIWidgetBaseItem)
function TianYiYaoShiPanelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetObjActive(data.seq > 1)
    self.Png:SetObjActive(data.seq <= 1)

    local item = Item.Create(data.reward_item)
    if data.seq > 1 then
        self.ItemCell:SetData(item)
    else
        local model_info = TianYiYaoShiData.Instance:GetModelInfoByItemId(item.item_id)
        if model_info and model_info.png then
            self.Png:SetByPath(model_info.png)
        end
    end
    UH.SetText(self.ItemName, item:Name())
end

function TianYiYaoShiPanelItem:OnPngClick()
    local item = Item.Create(self.data.reward_item)
    local data_list = {item}
    local info = {
        item_list = data_list,
    }
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

-- 商城
TianYiYaoShiShopView = TianYiYaoShiShopView or DeclareView("TianYiYaoShiShopView", "serveractivity/tian_yi_yao_shi/tian_yi_yao_shi_shop")
function TianYiYaoShiShopView:TianYiYaoShiShopView()
    self.Curr:DefaultSet()
end

function TianYiYaoShiShopView:OnCloseClick()
    ViewMgr:CloseView(TianYiYaoShiShopView)
end

TianYiYaoShiShopPanel = TianYiYaoShiShopPanel or DeclareMono("TianYiYaoShiShopPanel", UIWFlushPanel)
function TianYiYaoShiShopPanel:TianYiYaoShiShopPanel()
    self.data = TianYiYaoShiData.Instance
    self.act_type = self.data.act_type
    self.data_cares = {
        {data = self.data.info_data, func = self.FlushPanel, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function TianYiYaoShiShopPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function TianYiYaoShiShopPanel:FlushPanel()
    local list_data = self.data:GetShopCfg()
    self.List:SetObjActive(#list_data > 3)
    self.List2:SetObjActive(#list_data <= 3)
    if #list_data > 3 then
        self.List:SetData(list_data)
    else
        self.List2:SetData(list_data)
    end
end

function TianYiYaoShiShopPanel:OnBuyClick(data)
    if not self.data:IsCanBuy(data) then
        return
    end
    self.buy_data = data
    if data.cfg_or == 1 then
        local fee = data.price / 10
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_TIAN_YI_YAO_SHI,fee,self.act_type,TianYiYaoShiCtrl.OpType.Buy, data.seq)
    else
        if MallCtrl.IsNotCurrency(data.price_type, data.price) then
            ServerActivityCtrl.Instance:SendActivityReq(self.act_type,TianYiYaoShiCtrl.OpType.Buy,data.seq)
        end
    end
end

function TianYiYaoShiShopPanel:SendBuy()
    if self.buy_data == nil then
        return
    end
    --LogError("刷新了sendbuy",BUY_ITEM_TYPE.TYPE_TIAN_YI_YAO_SHI, self.buy_data.price)
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_TIAN_YI_YAO_SHI, self.buy_data.price)
end


TianYiYaoShiShopItem = TianYiYaoShiShopItem or DeclareMono("TianYiYaoShiShopItem", UIWidgetBaseItem)
function TianYiYaoShiShopItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item)
    self.Cell:SetData(item)
    UH.SetText(self.Name, item:QuaName(true))
    local limit_text = data.limit_type == 1 and Language.TianYiYaoShi.Tip4 or Language.TianYiYaoShi.Tip5
    local buy_num = TianYiYaoShiData.Instance:GetShopBuyTimes(data.seq)
    buy_num = data.buy_times - buy_num
    UH.SetText(self.LimitTip, string.format(limit_text, buy_num))
    self.Inter.Interactable = buy_num > 0
    self.Effect:SetActive(buy_num > 0)
    if data.cfg_or == 1 then
        UH.SetText(self.Price, data.price / 10)
        local value = data.value / 10
        UH.SetText(self.Value, Language.TianYiYaoShi.Tip6 .. value)
    else
        local value = DataHelper.ConverMoney(data.price)
        UH.SetText(self.Price, value)
        UH.SetText(self.Value, Language.TianYiYaoShi.Tip6 .. data.value)
    end
    UH.SetActGold(self.Icon, data.price_type, data.cfg_or)
end


-- 抽奖记录
TianYiYaoShiLogView = TianYiYaoShiLogView or DeclareView("TianYiYaoShiLogView", "serveractivity/tian_yi_yao_shi/tian_yi_yao_shi_log")
function TianYiYaoShiLogView:TianYiYaoShiLogView()
    self.data = TianYiYaoShiData.Instance
end

function TianYiYaoShiLogView:LoadCallback(param_t)
    local list_data = self.data:GetLogList()
    self.List:SetData(list_data)
    self.Empty:SetActive(TableIsEmpty(list_data))
end

function TianYiYaoShiLogView:OnCloseClick()
    ViewMgr:CloseView(TianYiYaoShiLogView)
end


TianYiYaoShiLogItem = TianYiYaoShiLogItem or DeclareMono("TianYiYaoShiLogItem", UIWidgetBaseItem)
function TianYiYaoShiLogItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local time_info = TimeManager.FormatUnixTime2Date(data.time)
    UH.SetText(self.Time, string.format(Language.TianYiYaoShi.Tip7, time_info.month, time_info.day, time_info.hour, time_info.minute, time_info.second))
    local item_name = Item.GetQuaName(data.item_id, true)
    UH.SetText(self.Desc, string.format(Language.TianYiYaoShi.Tip8, item_name, data.num))
end



-- 概率展示
TianYiYaoShiRateView = TianYiYaoShiRateView or DeclareView("TianYiYaoShiRateView", "serveractivity/tian_yi_yao_shi/tian_yi_yao_shi_rate")
function TianYiYaoShiRateView:TianYiYaoShiRateView()
    self.data = TianYiYaoShiData.Instance
end

function TianYiYaoShiRateView:LoadCallback()
    local cfg = self.data:GetBaseCfg()
    local list_data = {}
    for i,v in pairs(cfg) do
        if not list_data[v.group_type] then
            list_data[v.group_type] = {group_type = v.group_type, list = {}}
        end
        table.insert(list_data[v.group_type].list, v)
    end
    self.List:SetData(list_data)
end

function TianYiYaoShiRateView:OnCloseClick()
    ViewMgr:CloseView(TianYiYaoShiRateView)
end

TianYiYaoShiGroupItem = TianYiYaoShiGroupItem or DeclareMono("TianYiYaoShiGroupItem", UIWidgetBaseItem)
function TianYiYaoShiGroupItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Title, Language.TianYiYaoShi.GroupType[data.group_type])
    self.List:SetData(data.list)
end

TianYiYaoShiRateItem = TianYiYaoShiRateItem or DeclareMono("TianYiYaoShiRateItem", UIWidgetBaseItem)
function TianYiYaoShiRateItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item)
    self.ItemCell:SetData(item)
    UH.SetText(self.ItemName, item:QuaName(true))
    local rate = data.show_rate
    UH.SetText(self.RateText, string.format(Language.TianYiYaoShi.Tip9, rate))
end



-- 抽奖结果
TianYiYaoShiResultView = TianYiYaoShiResultView or DeclareView("TianYiYaoShiResultView", "serveractivity/tian_yi_yao_shi/tian_yi_yao_shi_result")
function TianYiYaoShiResultView:TianYiYaoShiResultView()
    self.data = TianYiYaoShiData.Instance
end

function TianYiYaoShiResultView:LoadCallback(param_t)
    local time = not self.data.jump_animation and 3 or 1
    self.Effect:SetActive(time == 3)
    if not self.data.jump_animation then
        self.Animation:Play("tian_yi_yao_shi_result_801_O")
    end
    local list_data = param_t.list or {}
    list_data = DataHelper.FormatItemList(list_data)
    if #list_data > 1 then
        self.List:SetData(list_data)
    elseif #list_data == 1 then
        self.Item:SetObjActive(true)
        self.Item:SetData(list_data[1])
    end
    self.time_ht = Invoke(function ()
        self.Close:SetActive(true)
        PublicPopupCtrl.Instance:ShowCustom(list_data)
    end,time)
end

function TianYiYaoShiResultView:OnCloseClick()
    ViewMgr:CloseView(TianYiYaoShiResultView)
end

function TianYiYaoShiResultView:OnCloseCallback()
    TimeHelper:CancelTimer(self.time_ht)
end


TianYiYaoShiResultViewItem = TianYiYaoShiResultViewItem or DeclareMono("TianYiYaoShiResultViewItem", UIWidgetBaseItem)
function TianYiYaoShiResultViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = data --Item.Create(data)
    self.Cell:SetData(item)
    UH.SetText(self.Name, item:QuaName())
    self.Effect:SetActive(TianYiYaoShiData.Instance:IsHearsay(item.item_id))
end