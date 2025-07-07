ThrivingShopView = ThrivingShopView or DeclareView("ThrivingShopView", "serveractivity/thriving_shop_view", Mod.RandActivity.ThrivingShop)
VIEW_DECLARE_LEVEL(ThrivingShopView, ViewLevel.L1)
function ThrivingShopView:ThrivingShopView()
end

function ThrivingShopView:OnClickClose()
    ViewMgr:CloseView(ThrivingShopView)
end

ThrivingShopPanel = ThrivingShopPanel or DeclareMono("ThrivingShopPanel", UIWFlushPanel)
function ThrivingShopPanel:ThrivingShopPanel()
    self.data_cares = {
        {data = ServerActivityData.Instance.thriving_shop_data, func = self.FlushShopItem, init_call = false}
    }
    self:InitModelCfg()
end

function ThrivingShopPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_func)
    Delete(self, "ui_obj_show")
    Delete(self, "ui_role_show")
end

function ThrivingShopPanel:Awake()
    UIWFlushPanel.Awake(self)
    local show_list = self:GetShowItemList()
    self.List:SetData(show_list)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_THRIVING_SHOP, TimeType.Type_Special_4)
    -- UH.SetText(self.Desc, Config.language_cfg_auto.textdesc[53].desc)
    UH.SetText(self.Title, ActivityRandData.GetName(ACTIVITY_TYPE.RAND_THRIVING_SHOP))
    self.ToggleRemind.isOn = not ServerActivityData.Instance:IsThrivingRemind()

    if #show_list > 0 then
        self.consume_id = show_list[1].consume_item_id
        UH.SetIcon(self.Gold, Item.GetIconId(self.consume_id), ICON_TYPE.ITEM)
        UH.SetText(self.Num, Item.GetNum(self.consume_id))
    end

    self:ShowModel()
end

--刷新购买页面
function ThrivingShopPanel:FlushShopItem()
    self.List:SetData(self:GetShowItemList())
    if self.consume_id ~= nil then
        UH.SetText(self.Num, Item.GetNum(self.consume_id))
    end
end

function ThrivingShopPanel:OnClickGetItem()
    if PlanActivityCtrl.Instance:ActGetWayViewById(ACTIVITY_TYPE.RAND_THRIVING_SHOP, self.consume_id) then
        ViewMgr:CloseView(ThrivingShopView)
    end
end

function ThrivingShopPanel:GetShowItemList()
    local show_list = ServerActivityData.Instance:GetThrivingShowList()
    local function func_sort(data)
        local is_unlock, timestamp = ServerActivityData.Instance:IsThrivingItemUnlock(data)
        if not is_unlock then
            return 2000
        end
        local limit_times = data.limit_type == 1 and data.buy_times or data.today_times
        local can_buy = limit_times > (ServerActivityData.Instance:GetThrivingTimes(data.item_seq) or 0)
        if not can_buy then
            return 1500
        end
        --可购买前置
        if data.show == 1 and is_unlock and can_buy then
            if Item.GetNum(data.consume_item_id) >= data.consume_item_count then
                return 500 - data.item_seq - data.item_seq
            end
            return 800 - data.item_seq - data.item_seq
        end
        return 1000
    end
    table.sort(show_list, DataHelper.WeightSort("item_seq", func_sort))
    return show_list
end

function ThrivingShopPanel:InitModelCfg()
    self.uptate_model_time = 5
    self.show_model_index = 1
    self.model_list = {}
    for i, v in pairs(ServerActivityData.Instance:GetThrivingShowList()) do
        if v.show1 == 1 then
            table.insert(self.model_list, v)
        end
    end
    table.sort(self.model_list, DataHelper.SortFunc("item_seq"))

    self.ui_obj_show = UIObjDrawer.New()
    self.Model:SetShowData(
        {
            ui_obj = self.ui_obj_show,
            view = self,
            seq = 1,
            show_shadow = false
        }
    )
    self.ui_role_show = UIChDrawer.New()
    self.RoleModel:SetShowData(
        {
            ui_obj = self.ui_role_show,
            view = self,
            seq = 2,
            show_shadow = false
        }
    )
end

function ThrivingShopPanel:ShowModel()
    if self.model_list == nil or self.model_list[self.show_model_index] == nil then
        return
    end
    if #self.model_list > 1 then
        TimeHelper:CancelTimer(self.time_func)
        self.time_func = Invoke(BindTool.Bind(self.AutoToNext, self), self.uptate_model_time)
    end

    local item_id = self.model_list[self.show_model_index].exchange_item.item_id
    self.Model.gameObject:SetActive(false)
    self.RoleModel.gameObject:SetActive(false)
    self.ImgSetImgModel.gameObject:SetActive(false)
    self.AppearanceBubble.gameObject:SetActive(false)
    local item_cfg = Item.GetConfig(item_id)
    if item_cfg == nil then
        return
    elseif item_cfg.item_type == ItemType.Fashion then
        local surface_cfg = Cfg.SurfaceInfoById(item_cfg.param1)
        if surface_cfg and surface_cfg.res_id ~= 0 then
            if surface_cfg.type == IllusionConfig.FashionType.mount then
                -- 坐骑单独显示
                self.Model.gameObject:SetActive(true)
                self.ui_obj_show:SetData(DrawerHelper.GetRidePath(surface_cfg.res_id))
            else
                -- 武器、羽翼、角色时装
                local appearance = TableCopy(RoleData.Instance:GetApearance())
                if surface_cfg.type == IllusionConfig.FashionType.cloth then
                    appearance.color = 0
                    appearance.fashion_body_id = surface_cfg.res_id
                    appearance.special_appearance_type = 0
                    appearance.special_appearance_param = 0
                elseif surface_cfg.type == IllusionConfig.FashionType.weapon then
                    appearance.weapon_id = 2001 + RoleData.Instance:ProfType() * 100
                    appearance.fashion_weapon_id = surface_cfg.res_id
                end
                self.RoleModel.gameObject:SetActive(true)
                self.ui_role_show:SetData(appearance)
            end
        end
    elseif item_cfg.item_type == ItemType.Skin then
        local skin_cfg = nil
        if item_cfg.show_type == ShowType.PetSkin then
            skin_cfg = Cfg.PetSkinAttr(item_cfg.param1)
        elseif item_cfg.show_type == ShowType.PartnerSkin then
            skin_cfg = PartnerData.Instance.partner_skin_ids[item_cfg.param2]
        end
        if skin_cfg and skin_cfg.res_id then
            self.Model.gameObject:SetActive(true)
            self.ui_obj_show:SetData(DrawerHelper.GetNpcPath(skin_cfg.res_id))
        end
    elseif item_cfg.item_type == ItemType.Normal then
        if item_cfg.show_type == ShowType.Pet then
            self.Model.gameObject:SetActive(true)
            self.ui_obj_show:SetData(DrawerHelper.GetNpcPath(PetData.Instance:GetPetCfg(item_id).res_id))
        end
    elseif item_cfg.item_type == ItemType.Appearance then
        -- 颜值物品（头衔、气泡框、尾标）
        local appearance_cfg = Config.face_cfg_auto.resource[item_cfg.param1]
        if appearance_cfg and appearance_cfg.res_type and appearance_cfg.res_id then
            local icon_type = AppearanceConfig.IconType[appearance_cfg.res_type]
            local icon_pre = AppearanceConfig.IconPre[appearance_cfg.res_type]
            if appearance_cfg.res_type == AppearanceConfig.Type.Bubble then
                self.AppearanceBubble.gameObject:SetActive(true)
                self.AppearanceBubble:SetData(appearance_cfg.res_id, Vector2.New(174, 65))
            else
                self.ImgSetImgModel.gameObject:SetActive(true)
                UH.SetIcon(self.ImgSetImgModel, icon_pre .. appearance_cfg.res_id, icon_type)
            end
        end
    end
    UH.SetText(self.DisplayText, Format(Language.ThrivingShop.DisplayName, Item.GetName(item_id)))
end

function ThrivingShopPanel:OnSelectToggle(param)
    ServerActivityData.Instance:SetIsThrivingRemind(param)
    self.ToggleRemind.isOn = not ServerActivityData.Instance:IsThrivingRemind()
end

function ThrivingShopPanel:AutoToNext()
    self.show_model_index = self.show_model_index + 1
    self.show_model_index = self.show_model_index > self:GetMaxUnlockModelIndex() and 1 or self.show_model_index
    self:ShowModel()
end

function ThrivingShopPanel:OnClickShow(data)
    local data_index = nil
    for i, v in ipairs(self.model_list) do
        if v.item_seq == data.item_seq then
            data_index = i
        end
    end
    if self.show_model_index == data_index then
        TimeHelper:CancelTimer(self.time_func)
        self.time_func = Invoke(BindTool.Bind(self.AutoToNext, self), self.uptate_model_time)
        return
    end
    self.show_model_index = data_index
    self:ShowModel()
end

function ThrivingShopPanel:GetMaxUnlockModelIndex()
    for i, v in ipairs(self.model_list) do
        local is_unlock, timestamp = ServerActivityData.Instance:IsThrivingItemUnlock(v)
        if not is_unlock then
            return i - 1
        end
    end
    return #self.model_list
end

----------------------------ThrivingShopItem----------------------------
ThrivingShopItem = ThrivingShopItem or DeclareMono("ThrivingShopItem", UIWidgetBaseItem)
function ThrivingShopItem:ThrivingShopItem()
    local function func_show(t)
        local hour = tonumber(t.day) * 24 + tonumber(t.hour)
        hour = Format("%02d", hour)
        return string.format(Language.UiTimeMeter.TimeStr, hour, t.min, t.s)
    end
    self.Timer:SetShowCallBack(func_show)
    local function func_complete()
        -- self:SetData(self:GetData())
        ServerActivityData.Instance.thriving_shop_data:Notify()
    end
    self.Timer:SetCallBack(func_complete)
end

function ThrivingShopItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    data.item = Item.New(data.exchange_item)
    self.Call:SetData(data.item)
    UH.SetText(self.Name, data.item:Name())
    UH.SetIcon(self.Gold, Item.GetIconId(data.consume_item_id), ICON_TYPE.ITEM)
    UH.SetText(self.Num, data.consume_item_count)
    -- self.Num.color = Item.GetNum(data.consume_item_id) < data.consume_item_count and COLORS.Red7 or COLORS.White

    local count = ServerActivityData.Instance:GetThrivingTimes(data.item_seq) or 0
    local limit_times = data.limit_type == 1 and data.buy_times or data.today_times
    local limit_desc = (limit_times - count) .. "/" .. limit_times
    limit_desc = Format(Language.Shop.LimitExchange[data.limit_type], limit_desc)
    UH.SetText(self.TimesText, limit_desc)

    local is_unlock, timestamp = ServerActivityData.Instance:IsThrivingItemUnlock(data)
    self.MaskObj:SetActive(not is_unlock)
    self.CellEffect:SetActive(is_unlock)
    -- self.BtnExchange:SetActive(is_unlock)
    -- self.TimesText.gameObject:SetActive(is_unlock)
    if not is_unlock then
        self.Timer:StampTime(timestamp, TimeType.Type_Time_0, Language.ThrivingShop.UnlockTime)
    end
    self.BtnExchangeInter.Interactable = limit_times > count
end

function ThrivingShopItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
end

function ThrivingShopItem:OnClickItem()
    if self.data ~= nil and self.data.show1 == 1 then
        local is_unlock, timestamp = ServerActivityData.Instance:IsThrivingItemUnlock(self.data)
        if is_unlock then
            self.ParentPanel:OnClickShow(self.data)
        end
    end
end

function ThrivingShopItem:OnClickExchange()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_THRIVING_SHOP, 1, self.data.item_seq or 0, 1)
end
