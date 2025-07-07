ReputationShopView = ReputationShopView or DeclareView("ReputationShopView", "reputation/reputation_shop", Mod.Reputation.Main)

VIEW_DECLARE_LEVEL(ReputationShopView, ViewLevel.L1)

function ReputationShopView:ReputationShopView()
    self.Board:SetData(self:GetType(), PrestigeData.Instance:GetTitleName())
    self.Currency:DefaultSet()
    self.Board:SetCloseFunc(function()
        if PrestigeData.CancelTimer then
            PrestigeData.CancelTimer()
        end
    end)
end

function ReputationShopView:CloseCallback()
    PrestigeData.Instance:Initialization()
    PrestigeData.GeneralStore = nil
    PrestigeData.GuideIndex = nil
    TaskData.shop_auto_close = nil
    TaskData.shop_auto_open = nil
    -- MallData.IsHasSilkRoad = nil
    MallData.IsHasBountySubmission = nil
    LinkMarkData.Instance:Ignites(Mod.Reputation.Main)
    TaskData.Instance:TaskChainSubContinue()
    ReputationCtrl.Instance:ClearCache()
end

--===================================ReputationShopPanel===================================
ReputationShopPanel = ReputationShopPanel or DeclareMono("ReputationShopPanel", UIWFlushPanel)

function ReputationShopPanel:ReputationShopPanel()
    self.data = PrestigeData.Instance
    self.beh_data = BehaviorData.Instance
    self.data_cares = {
        {data = self.data.shop_info, func = self.FlushItemView, init_call = false},
        {data = self.data.rand_list_info, func = self.FlushItemView, init_call = false},
        {data = self.data.select_data, func = self.FlushTypeView, init_call = true, keys = {"type"}},
        {data = self.data.select_data, func = self.FlushItemView, init_call = false, keys = {"list"}},
        {data = BagData.Instance.item_grid,func = self.FlushSelectItem,init_call = false},

    }
    GuideManager.Instance:RegisterGetGuideUi("ReputationSelectBtn", function()
        if self.ReputationList and self.ReputationList.index_item then
            local index = PrestigeData.GuideIndex or 1
            local select_data = self.ReputationList:GetByIndex(index)
            if self.ReputationList.index_item[index] then
                return self.ReputationList.index_item[index].BtnGo
            end
            if select_data.has_times == 0 then 
                GuideManager.Instance:ForceStop()
            end 
        end
    end)

    GuideManager.Instance:RegisterGetGuideUi("ReputationLuYaoSelectItem", function()
        if self.ReputationList then
            local mono = self.ReputationList:GuideVertical(function (data) return data.seq == 1 end,2) --seq 1为第一个路遥碎片
            if mono then
                local mono_data = mono:GetData()
                return mono,function ()
                    mono:OnSelect(true)
                    if mono_data.has_times <= 0 then    --商品卖完了就终端指引
                        GuideManager.Instance:ForceStop()
                    end
                end
            end
        end
    end)

    GuideManager.Instance:RegisterGetGuideUi("ReputationClose", function()
        return self.CloseGuide,function ()
            ViewMgr:CloseView(ReputationShopView)
        end
    end)

    self.InputField.onValueChanged:AddListener(BindTool.Bind(self.OnClickNumSelect, self))
    PrestigeData.CancelTimer = function()
        TimeHelper:CancelTimer(self.timer_auto_beh)
        ViewMgr:CloseView(DialogTipsView)
    end
    if PrestigeData.GeneralStore then
        self.Time:SetShowCallBack(function(t)
            return Format(Language.UiTimeMeter.TimeStr, t.hour, t.min, t.s)
        end)
        local end_time = self.data:GetRandUpdateTime()
        self.Time:StampTime(end_time)
        self.Time:SetObjActive(true)
    else
        self.Time:SetObjActive(false)
    end

end

function ReputationShopPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.TypeList:SetDataList(self.data:GetTitleBtnList(), self.data:GetSelectType())
    self.village_data = self.data:GetVillageList() or {}
    if PrestigeData.GeneralStore then
        self.TypeListGrid.cellSize = Vector2.New(173,58)
    end
    if self.beh_data.auto_behavior > 0 then
        self.timer_auto_beh = TimeHelper:AddDelayTimer(function ()
            self:OnClickPurchase()
        end,
        self.beh_data.auto_behavior)
    end
    self:FlushProgress()
end

function ReputationShopPanel:FlushProgress()
    if PrestigeData.GeneralStore == nil and self.village_data then
        local _,config = self.data:GetVillagePrestigeData()
        local village_name = self.village_data.name
        local info,next_config = self.data:GetVillagePrestigeNextData()
        local cur_num,need_num = info.prestige,next_config.need_prestige
        self.Progress:SetProgress(cur_num / need_num)
        self.Progress:SetText(Format("%s/%s",cur_num,need_num))
        UH.SetText(self.VillageTitle,Format(Language.Reputation.DonationShopTip,village_name,config.name))
    end
    self.VillageName:SetActive(PrestigeData.GeneralStore == nil)
end

function ReputationShopPanel:FlushTypeView(is_jump)
    local type = self.data:GetSelectType()
    local index = self.data:GetSelctItemIndex()
    local page_list = self.data:GetPageVillageSortList(type)
    if index > #page_list then
        index = 1
    end
    if PrestigeData.GuideIndex == nil then
        self.ReputationList:SetDataList(page_list,index)
    else
        self.ReputationList:SetDataList(page_list,1)
    end
    if index > 2 and is_jump then
        self.ReputationList:JumpVerticalIndex(math.ceil(index / 2),118)
    end
    self.NotItemTip:SetActive(#page_list == 0)
    local page_name = Config.prestige_cfg_auto.type_list[type].name
    UH.SetText(self.NotItemText, Format(Language.Reputation.NotItemTip,page_name))
end

function ReputationShopPanel:FlushItemView()
    local data = self.data:GetSelectData()
    UH.SetText(self.Name, data.item:QuaName())
    UH.SetText(self.Content, Format(Language.Mall.Content, data.item:Desc()))
    for i = 1, 2 do
        UH.SetGold(self.Gold[i], data.money_type)
    end
    self.money_type, self.make_price, self.make_num = data.money_type, data.price, MallData.JumpItemNum or 1

    self.make_num = self.make_num <= 0 and 1 or self.make_num
    MallData.JumpItemNum = nil
    UH.SetText(self.Price, self.make_price)
    if data.item:IsEquip() then
        local level = Equip.GetEquipLevel(data.item_id)
        local name = Format(Language.Reputation.EquipLevel, level)
        UH.SetText(self.Name, data.item:QuaName() .. name)
    end
    self:FlushTimesView()
    self:FlushNumView()
end

function ReputationShopPanel:FlushTimesView()
    local data = self.data:GetSelectData()
    if data.limit_times > 0 then
        local has_times = data.limit_times - self.data:GetLimitTimes()
        local has_color = has_times == 0 and COLORSTR.Red10 or COLORSTR.Yellow13
        local limit_type = data.limit_type or 1
        local limit_type = Language.Shop.LimitType[limit_type]
        if data.item_seq ~= nil then
            has_times = data.limit_times - PrestigeData.Instance:GetRandItemBuyTimes(data.rand_index)
        end
        UH.SetText(self.Times, Format("<color=#%s>%s：%s</color>", has_color, limit_type, has_times))
        if data.item_seq ~= nil then
            self:FlushTypeView(false)
        else
            if has_times == 0 and self.select_index == data.index then
                self:FlushTypeView(true)
            end
        end
    end
    if data.relation then
        local pData = self.data:GetPrestigeUpgradeData(data.relation)
        local pLevel = self.data:GetPrestigeLevel()
        local pColor = pLevel >= data.relation and COLORSTR.Yellow13 or COLORSTR.Red10
        UH.SetText(self.LimitLevel, Format(Language.Reputation.LimitLevel, pColor, pData.name or ""))
        local is_limit, limit_tip = ReputationCtrl.IsRequire(data.require_type, data.require_value)
        if pLevel >= data.relation and not is_limit then
            UH.SetText(self.LimitLevel, ColorStr(Format(Language.Reputation.RequireTip[data.require_type], data.require_value), COLORSTR.Red10))
        end
    end
    self.Reputation:SetActive(data.relation and (self.data:GetPrestigeLevel() < data.relation or not is_limit))
    self.Times:SetObjActive(data.limit_times > 0)
    self.limit_times = data.limit_times
    self.select_index = data.index
end


function ReputationShopPanel:FlushNumView()
    local total_price = self.make_num * self.make_price
    UH.SetText(self.Num, self.make_num)
    UH.SetText(self.TotalPrice, total_price)
    local num = RoleData.Instance:GetCurrency(self.money_type)
    UH.Color(self.TotalPrice, num < total_price and COLORS.Red10 or COLORS.White)
end

function ReputationShopPanel:FlushSelectItem()
    -- if MallData.IsHasSilkRoad == true then
    --     self.data:SetSelctItemIndex()
    --     self:FlushTypeView()
    -- end
end

function ReputationShopPanel:OnClickAddNum(num)
    if self.make_num + num <= 0 then
        return
    end
    if self.limit_times > 0 and self.make_num + num > self.limit_times - self.data:GetLimitTimes() then
        PublicPopupCtrl.Instance:Center(Language.Reputation.LimitNumTip)
        return
    end
    if self.make_num + num > GameEnum.SHOP_BUY_MAX_LIMIT_NUM then
        PublicPopupCtrl.Instance:Center(Language.Shop.LimitNumTip)
        return
    end
    self.make_num = self.make_num + num
    self:FlushNumView()
end

--点击数量
function ReputationShopPanel:OnClickNumSelect()
    if self.Num.text ~= "" then
        local num = tonumber(self.Num.text) or 0
        if num > 0 then
            if self.limit_times > 0 and num > self.limit_times - self.data:GetLimitTimes() then
                local limit_num = self.limit_times - self.data:GetLimitTimes()
                if limit_num <= 0 then
                    self.Num.text = 1
                else
                    self.Num.text = limit_num
                end
                return
            end
            if num > GameEnum.SHOP_BUY_MAX_LIMIT_NUM then
                self.Num.text = GameEnum.SHOP_BUY_MAX_LIMIT_NUM
                PublicPopupCtrl.Instance:Center(Language.Shop.LimitNumTip)
                return
            end
            self.make_num = num
            self:FlushNumView()
        else
            self.Num.text = 1
        end
    end
end

--点击购买
function ReputationShopPanel:OnClickPurchase()
    TimeHelper:CancelTimer(self.timer_auto_beh)
    local data = self.data:GetSelectData()
    if data == nil then 
        return 
    end
    if self.limit_times and self.limit_times > 0 and self.limit_times - self.data:GetLimitTimes() == 0 then
        local limit_type = Language.Shop.LimitType[data.limit_type]
        PublicPopupCtrl.Instance:Center(limit_type .. Language.Shop.NotHasTip)
        return
    end
    if data.relation ~= nil then
        if self.data:GetPrestigeLevel() < data.relation then
            PublicPopupCtrl.Instance:Center(Language.Exchange.NotHasLimitLevel)
            MainOtherCtrl.Instance:GetWayView({item = Item.New({item_id = self.village_data.item_id})})
            return
        else
            local is_limit,limit_tip = ReputationCtrl.IsRequire(data.require_type,data.require_value)
            if not is_limit then
                PublicPopupCtrl.Instance:Center(limit_tip)
                return
            end
        end
    end
    if data.item_seq then
        local buy_times = PrestigeData.Instance:GetRandItemBuyTimes(data.rand_index)
        local limit = data.limit_times - buy_times
        if limit <= 0 then
            PublicPopupCtrl.Instance:Center(Language.Shop.RandLimitTip)
            return
        end
    end

    if data.item == nil then
        return
    end
    local make_num = self.make_num or 1
    local type = Language.Common.HuoBi[data.money_type]
    local total_price = make_num * (self.make_price or 0)
    PublicPopupCtrl.Instance:AlertTip(
        Format(Language.Reputation.ReputationTip, total_price, type, make_num, data.item:QuaName()),
        function()
            if SilkRoadData.Instance:IsBountySubmission(data.item.item_id) then
                SilkRoadData.Instance:CheckBountySubmission()
                ViewMgr:CloseView(ReputationShopView)
            end
            if PrestigeData.GeneralStore then
                if data.item_seq then
                    RoleCtrl.Instance:CSShopBuyReq(data.rand_index, make_num,1)
                else
                    RoleCtrl.Instance:CSShopBuyReq(data.index, make_num)
                end
            else
                RoleCtrl.Instance:CSPrestigeShopingReq(data.index, make_num)
            end
            if TaskData.shop_auto_close then
                ViewMgr:CloseView(ReputationShopView)
            end
            if TaskData.shop_auto_open then
                ViewMgr:OpenView(SilkRoadView)
                TaskData.shop_auto_open = nil
            end
        end
    )
end

function ReputationShopPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_auto_beh)
end

function ReputationShopPanel:OnClickBatchDonate()
    SceneLogic.Instance:AutoToNpcs(self.village_data.juanzeng_npc or 0)
    ViewMgr:CloseView(ReputationShopView)
    ViewMgr:CloseView(BagView)
end

function ReputationShopPanel:OnClickHelp()
    ViewMgr:OpenView(ReputationShopHelpView,{village_id = self.village_data.village_id})
end

----------------------------ReputationBtnItem----------------------------
ReputationBtnItem = ReputationBtnItem or DeclareMono("ReputationBtnItem", UIWidgetBaseItem)

function ReputationBtnItem:SetData(data)
    UH.SetText(self.Name, data.name)
    UIWidgetBaseItem.SetData(self, data)
end

function ReputationBtnItem:OnClickItem()
    PrestigeData.Instance:SetSelectType(self.data.page)
end

function ReputationBtnItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

----------------------------ReputationItem----------------------------
ReputationItem = ReputationItem or DeclareMono("ReputationItem", UIWidgetBaseItem)
function ReputationItem:ReputationItem()
    self.silk_list = {
        {sort = 1, task_type = TaskConfig.TaskType.bounty, func = function(item_id)
            if SilkRoadData.Instance:IsBountySubmission(item_id) then
                return Language.Reputation.LabelList[2]
            end
        end},
        {sort = 2, task_type = TaskConfig.TaskType.prestige, func = function(item_id)
            if PrestigeData.Instance:IsPrestigeTaskSubmission(item_id) then
                return Language.Reputation.LabelList[3]
            end
        end},
        {sort = 3, task_type = TaskConfig.TaskType.family_daily, func = function(item_id)
            if GuildData.Instance:GetIsMissionItem(item_id) then
                return Language.Reputation.LabelList[4]
            end
        end},
        -- {sort = 3, task_type = TaskConfig.TaskType.silk_road, func = function(item_id)
        --     if SilkRoadData.Instance:IsShopSubmission(item_id) then
        --         return Language.Reputation.LabelList[1]
        --     end
        -- end},
    }
    table.sort(self.silk_list, DataHelper.WeightSort("sort", function(data)
        if ReputationCtrl.Instance:GetSelectTaskType() == data.task_type then
            return -1
        end
        return 10
    end))
end

function ReputationItem:SetData(data)
    data.item = Item.New(data)
    UH.SetText(self.Name, data.item:Name())
    UH.SetGold(self.Gold, data.money_type)
    UH.SetText(self.Num, data.price)
    self.Limit:SetActive(data.limit_times > 0 and data.item_seq == nil)
    if data.limit_times > 0 and data.item_seq == nil  then
        UH.SetText(self.LimitNum, Format(Language.Shop.LimitNum, data.limit_times))
        self.Inter.Interactable = data.has_times and data.has_times > 0 or false
        self.Panel:SetActive(data.has_times > 0)
    else
        self.Inter.Interactable = true
        self.Panel:SetActive(true)
    end
    local is_require_limit = ReputationCtrl.IsRequire(data.require_type, data.require_value)
    local is_limit = data.relation and (PrestigeData.Instance:GetPrestigeLevel() < data.relation)
    UH.SetText(self.LimitLevel, Language.Reputation.RequireLimitTip)
    self.LimitTip:SetActive(is_limit or not is_require_limit)
    self.Image:SetActive(not self.LimitTip.activeSelf)

    if not is_limit and not is_require_limit then
        UH.SetText(self.LimitLevel, Format(Language.Reputation.RequireLimit[data.require_type], data.require_value))
    end

    data.item.is_grey = is_limit or data.has_times == 0
    self.Call:SetData(data.item)
    UH.SpriteName(self.ProfIcon, ProfSettingB[RoleData.Instance:GetRoleProf()])
    self.ProfIcon:SetObjActive(data.item:ShowType() == 17 and PrestigeData.Instance:IsShopProfItem(data.item.item_id))

    local silk_str = self:GetSilkRoadStr(data)
    if silk_str then
        self.SilkRoad:SetActive(true)
        UH.SetText(self.SilkRoadName, silk_str)
    else
        self.SilkRoad:SetActive(false)
    end
    UIWidgetBaseItem.SetData(self, data)

    if not data.item_seq then
        self.ObjLimitTime:SetActive(false)
        UH.SetText(self.LimitBuy, "")
    else
        self.ObjLimitTime:SetActive(true)
        local buy_times = PrestigeData.Instance:GetRandItemBuyTimes(self.data.rand_index)
        local limit = self.data.limit_times - buy_times
        UH.SetText(self.LimitBuy, Language.Reputation.LimitBuy..limit)
    end
end

function ReputationItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        PrestigeData.Instance:SetSelectData(self.data)
    end
end

--#50775 如果多个任务需求同一个道具，则判断当前追踪任务，优先显示追踪的任务角标，购买后按优先级恢复显示。门派任务》声望任务》远洋贸易
function ReputationItem:GetSilkRoadStr(data)
    for i, v in ipairs(self.silk_list) do
        local val = v.func(data.item_id)
        if val ~= nil then
            return val
        end
    end
    return nil
end

