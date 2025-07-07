YouMingSealView = YouMingSealView or DeclareView("YouMingSealView", "youming/youming_seal_view",Mod.YouMing.Seal,ViewLayer.Normal - 2)
VIEW_DECLARE_MASK(YouMingSealView, ViewMask.None)
function YouMingSealView:YouMingSealView()
    self.data = YouMingData.Instance
    self.show_select = false
end

function YouMingSealView:LoadCallback()
    self.run_until_handle = Runner.Instance:RunUntilTrue(function()
        if ViewMgr:IsOpen(MainView) then
            MainViewData.Instance:ForceHideTask(true)
            YinYangData.Instance:SetYinYangBtnShow(false)
            if not GuideData.Instance.yinyang_eye_open then
                GuideManager.Instance:EnableYinYangEye()
            end
            return true
        end
    end, true)

    self:SetSceneNameTitle()
    self.SelectList:SetData(Config.ghost_hunt_auto.hunt_sence)
    self.SelectPanelObj:SetActive(false)
end

function YouMingSealView:OnClickTitle()
    self.show_select = not self.show_select
    self.SelectPanelObj:SetActive(self.show_select)
end

function YouMingSealView:OnClickLeave()
    YouMingCtrl.Instance:LeaveSealScene()
end

function YouMingSealView:SetSceneNameTitle()
    UH.SetText(self.TitleName, self.data:GetSealSceneShowName())
end

function YouMingSealView:CloseCallback()
    self.data.seal_smart.auto_seal = false
    if self.run_until_handle ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_until_handle)
        self.run_until_handle = nil
    end
    MainViewData.Instance:ForceHideTask(false)
    YinYangData.Instance:SetYinYangBtnShow(true)
    if GuideData.Instance.yinyang_eye_open then
        GuideManager.Instance:RevertYinYangEye()
    end
end

function YouMingSealView:OnClickBuyCard()
    ViewMgr:OpenView(YouMingBuySealCardView)
end

function YouMingSealView:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[416].desc})
end

function YouMingSealView:OnClickCardInfo()
    local item = Item.Init(Config.ghost_hunt_auto.hunt_item_sell[2].reward.item_id, 1,1)
    MainOtherCtrl.Instance:GetWayView({item = item})
end

--------SealView---------------------------
YouMingSealPanel = YouMingSealPanel or DeclareMono("YouMingSealPanel", UIWFlushPanel)
function YouMingSealPanel:YouMingSealPanel()
    self.data_instance = YouMingData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid[ItemColumnType.Item],func = self.FlushCardCount, init_call = false},
        {data = self.data_instance.seal_smart, func = self.FlushCardCount, keys = {"flush_card_count"}},
        {data = self.data_instance.seal_smart, func = self.SetSealBtnText, keys = {"auto_seal"}},
        {data = self.data_instance.seal_smart, func = self.SetSceneNameTitle, keys = {"flush_scene_title"}},
        {data = BattleData.Instance:BaseData(), func = self.FlushPanelShow, keys = {"server_fighting"}, init_call = false},
    }
end
function YouMingSealPanel:FlushPanelShow()
    if BattleData.Instance:IsServerFighting() then
        for i = 1, self.AllObjs:Length() do
            self.AllObjs[i]:SetActive(false)
        end
    else
        for i = 1, self.AllObjs:Length() do
            self.AllObjs[i]:SetActive(true)
        end
        self:CheckOpenTips(nil,true)
    end
end

function YouMingSealPanel:FlushCardCount()
    -- local items = Config.ghost_hunt_auto.hunt_item_sell
    -- local di_count = BagData.Instance:GetNumByItemId(items[1].reward.item_id)
    -- di_count = di_count > 0 and di_count or Format(Language.Common.Xstr, COLORSTR.Red8, di_count)
    -- UH.SetText(self.DiCardCount, Format(Language.YouMing.SealCardCountFormat, di_count))

    -- local gao_count = BagData.Instance:GetNumByItemId(items[2].reward.item_id)
    -- gao_count = gao_count > 0 and gao_count or Format(Language.Common.Xstr, COLORSTR.Red8, gao_count)
    -- UH.SetText(self.GaoCardCount, Format(Language.YouMing.SealCardCountFormat, gao_count))
    local gao_count = 0
    for key, value in pairs(Config.ghost_hunt_auto.hunt_item_sell) do
        if value.is_need_chong_zhi_price == 1 then
            gao_count = BagData.Instance:GetNumByItemId(value.reward.item_id)
            local count_str = gao_count > 0 and gao_count or Format(Language.Common.Xstr, COLORSTR.Red8, gao_count)
            UH.SetText(self.GaoCardCount, Format(Language.YouMing.SealCardCountFormat, count_str))
            break
        end
    end
end

function YouMingSealPanel:SetSceneNameTitle()
    UH.SetText(self.TitleName, self.data_instance:GetSealSceneShowName())
    self.SelectPanelObj:SetActive(false)
end

function YouMingSealPanel:CheckOpenTips(gao_count, need_delay)
    if gao_count == nil then
        for key, value in pairs(Config.ghost_hunt_auto.hunt_item_sell) do
            if value.is_need_chong_zhi_price == 1 then
                gao_count = BagData.Instance:GetNumByItemId(value.reward.item_id)
                break
            end
        end
    end
    if gao_count <= 0 then
        local tip_func = function()
            local t = {
                content = Language.YouMing.SealNotEnoughCard,
                cancel = {
                    func = function()
                        PublicPopupCtrl.Instance:CloseDialogTips()
                        YouMingCtrl.Instance:SendNetherWorldSealReq(2, 0)
                    end
                },
                confirm = {
                    func = function()
                        PublicPopupCtrl.Instance:CloseDialogTips()
                        YouMingCtrl.Instance:SendNetherWorldSealReq(2, 1)
                    end
                },
                tnr = DTTodayNotRemind.YouMingSealCard
            }
            PublicPopupCtrl.Instance:DialogTips(t)
        end
        if need_delay then
            self.delay_tip_handle = TimeHelper:AddDelayTimer(function()
                tip_func()
            end,1)
        else
            tip_func()
        end
        
        return true
    end
    return false
end


function YouMingSealPanel:OnClickStartSeal()
    if not DialogTipsData.Instance.today_not_reminds[DTTodayNotRemind.YouMingSealCard] and not self.data_instance.seal_smart.auto_seal and self:CheckOpenTips() then
        return
    end
    self.data_instance.seal_smart.auto_seal = not self.data_instance.seal_smart.auto_seal
    if self.data_instance.seal_smart.auto_seal then
        YouMingCtrl.Instance:SendNetherWorldSealReq(2, 1)
    else
        YouMingCtrl.Instance:SendNetherWorldSealReq(2, 0)
    end
    self:SetSealBtnText()
end

function YouMingSealPanel:SetSealBtnText()
    UH.SetText(self.SealBtnText, self.data_instance.seal_smart.auto_seal and Language.YouMing.StopSeal or Language.YouMing.StartSeal)
end

-------SealPanel--------------------------------
YouMingSealRightNameItem = YouMingSealRightNameItem or DeclareMono("YouMingSealRightNameItem", UIWidgetBaseItem)

function YouMingSealRightNameItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local name_str = YouMingData.Instance:GetSealSceneShowName(data.sence_id)
    UH.SetText(self.NameText, name_str)
end

function YouMingSealRightNameItem:OnClickName()
    --等级不足不给跳场景
    if RoleLevel() < self.data.need_level then
        PublicPopupCtrl.Instance:Center(Language.YouMing.SealRoleLevelNotEnough)
        return

    --同场景也不跳
    elseif SceneData.Instance:SceneId() == self.data.sence_id then
        PublicPopupCtrl.Instance:Center(Language.YouMing.SealSceneAllreadyIn)
        return
    end
    -- ViewMgr:CloseView(YouMingSealView)
    YouMingCtrl.Instance:GoToSealScene(self.data.sence_id, function()
        YouMingCtrl.Instance:SendNetherWorldSealReq(2,0)
    end)
end

----------SealCardView-------------------------------
YouMingBuySealCardView = YouMingBuySealCardView or DeclareView("YouMingBuySealCardView", "youming/youming_buy_seal_card")
VIEW_DECLARE_MASK(YouMingBuySealCardView, ViewMask.BgBlock)
function YouMingBuySealCardView:YouMingBuySealCardView()
end

function YouMingBuySealCardView:LoadCallback()
    self.Board:SetData(self:GetType(),Language.YouMing.SealCardBuyTitle, Vector2.New(682,379))
    self.CardList:SetData(Config.ghost_hunt_auto.hunt_item_sell)
end

---------BuySealCardView-------------------------------
YouMingBuyCardInfoItem = YouMingBuyCardInfoItem or DeclareMono("YouMingBuyCardInfoItem", UIWidgetBaseItem)
function YouMingBuyCardInfoItem:YouMingBuyCardInfoItem()
    self.data_instance = YouMingData.Instance
end

function YouMingBuyCardInfoItem:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    if self.data_care_handle ~= nil then
        self.data_instance.seal_smart:Uncare(self.data_care_handle)
        self.data_care_handle = nil
    end
end

function YouMingBuyCardInfoItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Init(self.data.reward.item_id, 1,1)
    self.ItemCell:SetData(item)
    UH.SetText(self.CardName, item:Name())
    self:FlushLimitText()
    UH.SetText(self.PriceText, self.data.price)
    if self.data.is_need_chong_zhi_price == 1 then
        UH.SpriteName(self.IconImg, 99999)
    else
        UH.SpriteName(self.IconImg, 65534)
    end
    if self.data_care_handle == nil then
        self.data_care_handle = self.data_instance.seal_smart:Care(function()
            self:FlushLimitText()
        end,"flush_card_count")
    end
end

function YouMingBuyCardInfoItem:SendBuy()
    if self.buy_care_handle ~= nil then
        ServerActivityData.Instance:GetCmdBuyItemCheckRet():Uncare(self.buy_care_handle)
        self.buy_care_handle = nil
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_NETHER_WORLD_SEAL_CARD, self.data.price)
end

function YouMingBuyCardInfoItem:FlushLimitText()
    local limit_str = ""
    if self.data.limit_times > 0 then
        local buy_times = self.data_instance:GetCardBuyTime(self.data.seq)
        limit_str = Format(Language.YouMing.SealCardBuyLimit, buy_times, self.data.limit_times)
    end
    UH.SetText(self.BuyLimitText, limit_str)
end

function YouMingBuyCardInfoItem:OnClickBuy()
    if self.data.is_need_chong_zhi_price == 1 then
        if self.buy_care_handle == nil then
            self.buy_care_handle = ServerActivityData.Instance:GetCmdBuyItemCheckRet():Care(function()
                self:SendBuy()
            end)
        end
        YouMingCtrl.Instance:SendNetherWorldSealReq(0, self.data.seq)
    elseif MallCtrl.IsNotCurrency(CurrencyType.Gold,self.data.price) then
        YouMingCtrl.Instance:SendNetherWorldSealReq(1, self.data.seq)
    end
end