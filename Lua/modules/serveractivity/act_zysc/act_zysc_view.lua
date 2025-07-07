-- @Author: Ghost
-- @Date: 2022-08-26 16:32:16
-- @Last Modified by:   Ghost
-- @Last Modified time: 2022-08-26 16:32:16
ActZyscView = ActZyscView or DeclareView('ActZyscView', 'serveractivity/act_zysc/act_zysc_view', Mod.MidAutumnFestival.Zysc)
VIEW_DECLARE_MASK(ActZyscView, ViewMask.BgBlock)
VIEW_DECLARE_LEVEL(ActZyscView, ViewLevel.L1)
-- 追月商城
function ActZyscView:ActZyscView()
    self.data = ActZyscCtrl.Instance:Data()
    self.data:InitData()
    -- 抽奖道具数量变化监听
    self.hanle = self.data.SD_ZqzyInfo:Care(BindTool.Bind(self.FlushCurr, self), "curr_num")
end

function ActZyscView:LoadCallback(param)
    self.data:ClearFirstRemind()
    self:FlushCurr()
    self.DjcTime:SetShowCallBack(function(t)
        return Format(Language.ActZysc.Time, t.day, t.hour, t.min)
    end)
    self.DjcTime:CreateActTime(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG, TimeType.Type_Time_3)
    if param.mod_key == Mod.MidAutumnFestival.Zqzy then
        self.TgZhuiYue.isOn = true
    else
        self.TgShangCheng.isOn = true
    end

    --self.CurrencySub:SetCurrency(CurrencyType.ImmortalCoin, CurrencyType.Gold)
    -- self.Currency:SetCurrency(self.data_zqzy.consume_item_id)
end

function ActZyscView:FlushCurr()
    local config = ActZyscData.Instance:GetSC_ZqzyInfo_Data()
    self.need_ItemId = config.consume_item_id
    UH.SetText(self.LbCoinNum, Item.GetNum(self.need_ItemId))
    self.ZyzyPanel:FlushDrawBtnInfo()
end

function ActZyscView:OnBtnClose()
    ViewMgr:CloseView(ActZyscView)
end

function ActZyscView:CloseCallback()
    self.data.SD_ZqzyInfo:Uncare(self.hanle)
end

-- 追月商城panel
----------------------------ActZyscPanel----------------------------
ActZyscPanel = ActZyscPanel or DeclareMono("ActZyscPanel", UIWFlushPanel)
function ActZyscPanel:ActZyscPanel()
    self.language = Language.ActZysc
    self.data = ActZyscCtrl.Instance:Data()
    self.data_cares = {
        {data = ActZyscData.Instance.SC_ZyscInfo, key = {'val'}, func = self.FlushList, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function ActZyscPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushList()
    self:OnListDragEnd()
end

function ActZyscPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.CurrencySub:SetCurrency(CurrencyType.ImmortalCoin, CurrencyType.Gold)
end

function ActZyscPanel:FlushList()
    self.ListItems:SetData(self.data.SC_ZyscInfo.data or {})
end

function ActZyscPanel:OnBuyClick(data)
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG) then
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        return
    end
    if data.buy_times <= 0 then
        -- 购买次数不足
        PublicPopupCtrl.Instance:Center(self.language.NotTimesTip)
        return
    end
    local seq = data.config.seq
    if data.config.cfg_or == 0 then
        -- 元宝购买
        if not MallCtrl.IsNotCurrency(CurrencyType.Gold, data.config.price) then
            -- 元宝不足
            return
        end
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG, ZyscReqType.Buy_0, seq)
    else
        -- 直购
        local fee = data.config.price / 10
        self.now_buy_data = data
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_ZHUI_YUE_SHANG_CHENG, fee, ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG, ZyscReqType.Buy_3, seq)
    end
end

function ActZyscPanel:SendBuy()
    if TableIsEmpty(self.now_buy_data) then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_ZHUI_YUE_SHANG_CHENG, self.now_buy_data.config.price)
    self.now_buy_data = nil
end

function ActZyscPanel:OnListDrag()
    for k, v in pairs(self.ListItems.item_list) do
        v:SetRectPos(self.RectTF.localPosition.x)
    end
end

function ActZyscPanel:OnListDragEnd()
    TimeHelper:CancelTimer(self.ShowTimer)
    self.ShowTimer = TimeHelper:AddCountDownTT(function()
        self:OnListDrag()
    end, function()
    end, 1, 0.01)
end

function ActZyscPanel:OnHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[355].desc)
end

function ActZyscPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ShowTimer)
end

-- 追月商城item
ActZyscItem = ActZyscItem or DeclareMono('ActZyscItem', UIWidgetBaseItem)
function ActZyscItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local data_config = data.config
    local lists_reward = data.Config_SC_Reward
    if lists_reward[1] ~= nil then
        self.List1:SetData(DataHelper.FormatItemList(lists_reward[1], 1))
    end
    if lists_reward[2] ~= nil then
        self.List2:SetData(DataHelper.FormatItemList(lists_reward[2], 1))
    end
    if data_config.cfg_or == 0 then
        -- 非直购
        UH.SetText(self.LbValue, data_config.value)
        UH.SetText(self.LbPrice, data_config.price)
    else
        -- 直购
        UH.SetText(self.LbValue, data_config.value / 10)
        UH.SetText(self.LbPrice, data_config.price / 10)
    end
    local name
    if data_config.gift_name ~= 0 then
        name = data_config.gift_name
    else
        name = Item.GetName(data_config.reward_item[0].item_id)
    end
    UH.SetText(self.LbMz, name)
    UH.SetText(self.LbZs, data_config.give_item.num)
    local res_hb = HuoBi[999]
    if data_config.price_type == CurrencyType.Gold then
        res_hb = HuoBi[0]
    end
    UH.SpriteName(self.ImSpHb, res_hb)
    UH.SpriteName(self.ImSpHb2, res_hb)
    local buytime
    if data_config.limit_type == 1 then
        buytime = Format(Language.ActZysc.limit_buy, data.buy_times)
    else
        buytime = Format(Language.ActZysc.limit_buy_day, data.buy_times)
    end
    UH.SetText(self.LbBuyTime, buytime)
    if data.buy_times == 0 then
        self.BtnInteract.Interactable = false
        if self.EffectHandle ~= nil then
            UH.StopEffect(self.UiEffect, self.EffectHandle)
        end
    else
        self.BtnInteract.Interactable = true
        if self.EffectHandle == nil then
            self.EffectHandle = UH.PlayEffect(self.UiEffect, 3164044)
        end
    end
end

function ActZyscItem:SetRectPos(parentX)
    local y = self:getPostY(self.RectPosSelf.localPosition.x + parentX)
    UH.SetRectPosition(self.RectPos, {y = y})
end

function ActZyscItem:getPostY(x)
    return 0.00017 * math.pow(x, 2) + (-0.13 * x)
end

function ActZyscItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    if self.EffectHandle ~= nil then
        UH.StopEffect(self.UiEffect, self.EffectHandle)
    end
end

-- 仲秋追月
----------------------------ActZqzyPanel----------------------------
ActZqzyPanel = ActZqzyPanel or DeclareMono("ActZqzyPanel", UIWFlushPanel)
function ActZqzyPanel:ActZqzyPanel()
    self.data = ActZyscCtrl.Instance:Data()
    self.language = Language.ActZysc
    self.data.SD_ZqzyInfo.val_MsIndex = 1
    self.data_cares = {
        {data = self.data.SD_ZqzyInfo, keys = {'val_sel'}, func = self.FlushPanelInfo, init_call = false},
        {data = self.data.SD_ZqzyInfo, keys = {'val_MsIndex'}, func = self.FlushModuleShowName, init_call = true},
    }
end

function ActZqzyPanel:Awake()
    UIWFlushPanel.Awake(self)
    local data_zqzy = self.data:GetSC_ZqzyInfo_Data()
    local data_zqzy_Big = self.data.SC_ZqzyInfo.data_Reward_Big
    UH.SetText(self.LbChaoJi, data_zqzy.show_awards_rate)
    UH.SetText(self.LbDaJiang, data_zqzy.show_common_rate)
    UH.SetText(self.LbRewardBig, Item.GetQuaName(data_zqzy_Big.reward_item.item_id))
    self:FlushPanelInfo()
    self:FlushDrawInfo()
    self:FlushDrawBtnInfo()
    -- self.BtnEffectL:Play(3164044)
    -- self.BtnEffectR:Play(3164044)
    self.EffectTool:Play(3164044, self.BtnEffectL)
    self.EffectTool:Play(3164044, self.BtnEffectR)
    -- self.BtnEffectL:SetActive(true)
    -- self.BtnEffectR:SetActive(true)
end

function ActZqzyPanel:FlushPanelInfo()
    local data_zqzy = self.data.SC_ZqzyInfo
    self.val_sel = self.data.SD_ZqzyInfo.val_sel
    if self.val_sel then
        local data_reward = data_zqzy.data_Reward_Sl_Seq
        for k, v in pairs(data_reward) do
            local item = self.ListItem[k + 1]
            if item ~= nil then
                item:SetData(v)
            end
        end
    end
end

function ActZqzyPanel:FlushModuleShowName()
    local index = self.data.SD_ZqzyInfo.val_MsIndex
    local data = self.data.data_ModuleShow[index]
    UH.SetText(self.LbModuleShow, Item.GetName(data.item_id))
end

function ActZqzyPanel:FlushDrawBtnInfo()
    self.Num_Draw_More = 5 -- 默认多抽为5次 最大为10次
    local config = self.data:GetSC_ZqzyInfo_Data()
    local num_have = Item.GetNum(config.consume_item_id)
    local num_need = config.consume_num
    local Num_Draw_More = self.Num_Draw_More
    if num_have > num_need * 5 then
        Num_Draw_More = math.floor(num_have / num_need)
        Num_Draw_More = Num_Draw_More > 10 and 10 or Num_Draw_More
        self.Num_Draw_More = Num_Draw_More
    end
    self.need_ItemId = config.consume_item_id
    self.num_need_one = num_need
    self.num_need_more = Num_Draw_More * num_need
    UH.SetText(self.LbMore, string.format(self.language.ChouJiang, Num_Draw_More))
    UH.SetText(self.LbMoreCost, self.num_need_more)
    UH.SetText(self.LbOne, string.format(self.language.ChouJiang, 1))
    UH.SetText(self.LbOneCost, num_need)
    UH.SetToggle(self.ToggleSkip, ActZyscCtrl.Instance.ToggleSkip)
    local data_zqzy_Big = self.data.SC_ZqzyInfo.data_Reward_Big
    local num = num_have >= num_need and 1 or 0
    self.RedPos:SetNum(num)
end

function ActZqzyPanel:FlushDrawInfo()
    local config = self.data:GetSC_ZqzyInfo_Data()
    local data_zqzy_Big = self.data.SC_ZqzyInfo.data_Reward_Big
    local Item_Id_Big = data_zqzy_Big.reward_item.item_id
    UH.SetText(self.LbRewardBig, Item.GetQuaName(Item_Id_Big))
    local path = IllusionData.Instance:GetShowPngPathByItemId(Item_Id_Big)
    self.Item_RewardBig = Item.Create(data_zqzy_Big.reward_item)
    if path then
        self.ImgRewBig:SetActive(true)
        self.SetterBg:SetActive(true)
        self.Setter:SetByPath(path)
    else
        if Item_Id_Big == ActZyscCtrl.Instance.ID_QiongQi then
            self.SetterBg:SetActive(true)
            self.ImgQiongQi:SetActive(true)
        else
            UH.SetIcon(self.CellBig, self.Item_RewardBig:IconId())
            -- self.CellBig:SetData(self.Item_RewardBig)
            self.CellBig:SetObjActive(true)
        end
    end
    self.SListModuleShow:SetCompleteCallBack(BindTool.Bind(self.ItemModuleShow, self))
    self.SListModuleShow:SetData(self.data.data_ModuleShow)
    self:StartChangePageTimer()
    self.ScPage.OnPageChanged:AddListener(BindTool.Bind(self.OnChangeModuleShow, self))
end

-- 自动切页
function ActZqzyPanel:OnAddPage()
    if not self.AutoChange then
        self.AutoChange = true
        return
    end
    local index = self.data.SD_ZqzyInfo.val_MsIndex
    if index > #self.data.data_ModuleShow - 1 then
        index = 0
    end
    self.ScPage:ScrollToPage(index)
    self:OnChangeModuleShow(index)
    self.AutoChange = true
end

-- 切页回调
function ActZqzyPanel:OnChangeModuleShow(index)
    self.AutoChange = false
    self.data.SD_ZqzyInfo.val_MsIndex = index + 1
    self:ItemModuleShow()
end

-- 通知模型item加载模型
function ActZqzyPanel:ItemModuleShow()
    self.SListModuleShow.item_list[self.data.SD_ZqzyInfo.val_MsIndex]:ShowModule()
end

function ActZqzyPanel:StartChangePageTimer()
    if self.time_func == nil then
        self.AutoChange = true
        self.time_func = TimeHelper:AddRunTimer(function()
            self:OnAddPage()
        end, 5, nil, false)
    end
end

function ActZqzyPanel:CloseChangePageTimer()
    if self.time_func ~= nil then
        TimeHelper:CancelTimer(self.time_func)
        self.time_func = nil
    end
end

function ActZqzyPanel:OnBtnGaiLv()
    if not self.GpGaiLv.activeSelf then
        self.GpGaiLv:SetActive(true)
    else
        self.GpGaiLv:SetActive(false)
    end
end

function ActZqzyPanel:OnGp_Reward()
    if self.val_sel then -- 以选择奖励
        return
    end
    ViewMgr:OpenView(ActZqzyXzView)
end

-- 抽一次
function ActZqzyPanel:OnDrawOne()
    if not self:JuageDraw(self.num_need_one) then
        return
    end
    ActZyscCtrl.Instance:REQ_DrawReward(1)
end
-- 抽多次
function ActZqzyPanel:OnDrawMore()
    if not self:JuageDraw(self.num_need_more) then
        return
    end
    ActZyscCtrl.Instance:REQ_DrawReward(self.Num_Draw_More)
end

-- 能否抽奖
function ActZqzyPanel:JuageDraw(num)
    if not self.val_sel then
        PublicPopupCtrl.Instance:Center(Language.ActZysc.sel_tip)
        ViewMgr:OpenView(ActZqzyXzView)
        return false
    end
    if Item.GetNum(self.need_ItemId) < num then
        PublicPopupCtrl.Instance:Center(Language.ActZysc.noNeedTip)
        self:OnCoinShow()
        return false
    end
    return true
end

function ActZqzyPanel:OnCoinShow()
    CellClicks.BagGridNoButton({
        GetData = function()
            if not self.Item_Coin then
                self.Item_Coin = Item.Create({item_id = self.data:GetSC_ZqzyInfo_Data().consume_item_id})
            end
            return self.Item_Coin
        end,
    })
end

-- 模型展示详情
function ActZqzyPanel:OnCheckModuleShow()
    CellClicks.BagGridNoButton({
        GetData = function()
            return self.data.data_ModuleShow[self.data.SD_ZqzyInfo.val_MsIndex]
        end,
    })
end

-- 大奖详情
function ActZqzyPanel:OnCheckRewardBigShow()
    CellClicks.BagGridNoButton({
        GetData = function()
            if not self.Item_RewardBig then
                self.Item_RewardBig = Item.Create(self.data.SC_ZqzyInfo.data_Reward_Big.reward_item)
            end
            return self.Item_RewardBig
        end,
    })
end

function ActZqzyPanel:OnToogleSkip(param)
    ActZyscCtrl.Instance.ToggleSkip = param
end

function ActZqzyPanel:OnJiLuClick()
    ViewMgr:OpenView(ZyscCjjlView)
end

function ActZqzyPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:StartChangePageTimer()
end

function ActZqzyPanel:OnDisable()
    UIWFlushPanel.OnDisable(self)
    self:CloseChangePageTimer()
end

function ActZqzyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.Item_RewardBig = nil
    self.Item_Coin = nil
    self.language = nil
    self.Item_RewardBig = nil
    self.SListModuleShow:SetCompleteCallBack(nil)
    self:CloseChangePageTimer()
end

ActZqzyItem = ActZqzyItem or DeclareMono("ActZqzyItem", UIWidgetBaseItem)
function ActZqzyItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local config_reward = data.config.reward_item
    local item = Item.Create(config_reward)
    item.IsGem = false
    UH.SpriteName(self.SpaBg, 'YiXuanZeDi')
    UH.SetText(self.LbName, Item.GetQuaName(config_reward.item_id))

    self.Cell:SetData(item)
    self.LbSel:SetObjActive(false)
    self.BgName:SetActive(true)
    self.Cell:SetObjActive(true)
end

-- 仲秋追月选择奖励界面
ActZqzyXzView = ActZqzyXzView or DeclareView("ActZqzyXzView", "serveractivity/act_zysc/act_zqzy_xzview")
VIEW_DECLARE_MASK(ActZqzyXzView, ViewMask.BgBlock)
VIEW_DECLARE_LEVEL(ActZqzyXzView, ViewLevel.L2)
function ActZqzyXzView:ActZqzyXzView()
    ActZyscCtrl.Instance:Init_RewardSeq()
end

function ActZqzyXzView:LoadCallback()
end

function ActZqzyXzView:OnBtnClose()
    ViewMgr:CloseView(ActZqzyXzView)
end

function ActZqzyXzView:CloseCallback()
    ActZyscCtrl.Instance:Clean_RewardSeq()
end

function ActZqzyXzView:OnConfirm()
    local check = ActZyscCtrl.Instance:REQ_SaveRewardSeq();
    if check then
        self:OnBtnClose()
    end
end

-- 仲秋追月选择奖励界面
ActZqzyXzPanel = ActZqzyXzPanel or DeclareMono("ActZqzyXzPanel", UIWFlushPanel)
function ActZqzyXzPanel:ActZqzyXzPanel()
    self.data = ActZyscCtrl.Instance:Data()
    self.data_cares = {{data = self.data.SD_ZqzyInfo, keys = {'data_Reward_Sl_length'}, func = self.Fl_SelNum, init_call = false}}
end

function ActZqzyXzPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.ListReward:SetData(self.data:GetSC_ZqzyReward_Data())
    self:Fl_SelNum()
end

function ActZqzyXzPanel:Fl_SelNum()
    local length = self.data.SD_ZqzyInfo.data_Reward_Sl_length
    UH.SetText(self.LbSelNum, string.format(Language.ActZysc.tip1, length, RA_ZhuiYueShangCheng.ZYSC_SELECT_NUM))
end

-- 中秋追月选择页面Item
ActZqzyXzItem = ActZqzyXzItem or DeclareMono("ActZqzyXzItem", UIWidgetBaseItem)
function ActZqzyXzItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item)
    -- 屏蔽推荐标签
    item.IsGem = false
    self.Cell:SetData(item)
end

function ActZqzyXzItem:OnToggle()
    local check = ActZyscCtrl.Instance:Set_RewardSeq(self.data.seq, self.TgSelect.isOn)
    self.TgSelect.isOn = check
end

function ActZqzyXzItem:OnPressDown()
    self.TimeHandle = TimeHelper:AddRunTimer(function()
        CellClicks.BagGridNoButton(self.Cell)
        TimeHelper:CancelTimer(self.TimeHandle)
    end, 0.2, 1, false)
end

function ActZqzyXzItem:OnPressUp()
    if self.TimeHandle ~= nil then
        TimeHelper:CancelTimer(self.TimeHandle)
        self.TimeHandle = nil
    end
end

ActZqzyMShowItem = ActZqzyMShowItem or DeclareMono("ActZqzyMShowItem", UIWidgetBaseItem)
function ActZqzyMShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

end

function ActZqzyMShowItem:ShowModule()
    local data = self.data
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New() --DrawerHelper.CreatByItem(data.vo)
        self.ModuleShow:SetShowData({ui_obj = self.ui_obj, view = self, show_shadow = false})
        self.ui_obj:PlayAnim(ChAnimType.Show)
    end

    self.ui_obj:SetData(DrawerHelper.GetDataByItem(data.vo))
end

function ActZqzyMShowItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
end

