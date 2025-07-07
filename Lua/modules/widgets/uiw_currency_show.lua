UIWCurrencyShow = UIWCurrencyShow or  DeclareMono("UIWCurrencyShow",UIWFlushPanel)

local default_cfg = {
	value_color = COLORS.Yellow8,
	font_size = 22,
}

CurrencyTypeConfig = {
    [CurrencyType.Gold] = {
        sprite_name = CommonItemId.Gold,
        on_click_add = function()
            ViewMgr:OpenViewByKey(Mod.Store.Recharge)
        end
    },
    [CurrencyType.CoinBind] = {
        sprite_name = CommonItemId.Coin,
        on_click_add = function()
            local param_t = {
                config = StoreData.Instance:GetItemConfig(22014),
                comfirm_func = function(data)
                    ShoppingCtrl.Instance:SendStoreBuyReq(data)
                    ViewMgr:CloseView(MoneyExchangeView)
                end
            }
            param_t.money_type = CurrencyType.CoinBind
            param_t.has_times = StoreData.Instance:GetHasTimes(param_t.config)
            if param_t.has_times > 0 then
                ViewMgr:OpenView(MoneyExchangeView,param_t)
            else
                StoreData.Instance:SetJumpIndex({conver_type = 0})
                ViewMgr:OpenViewByKey(Mod.Store.Main)
            end
        end
    },
    [CurrencyType.ImmortalCoin] = {
        sprite_name = CommonItemId.ImmortalCoin,
        on_click_add = function()
            local param_t = {
                config = StoreData.Instance:GetItemConfig(CommonItemId.ImmortalCoinId,5),
                comfirm_func = function(data)
                    ShoppingCtrl.Instance:SendStoreBuyReq(data)
                    ViewMgr:CloseView(MoneyExchangeView)
                end
            }
            param_t.money_type = CurrencyType.ImmortalCoin
            ViewMgr:OpenView(MoneyExchangeView,param_t)
        end
    },
    [CurrencyType.GuildCotribution] = {sprite_name = CommonItemId.GongXian,
    on_click_add = function()
        MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = CommonItemId.GongXian})})
    end,
    },
    [CurrencyType.Chivalrous] = {sprite_name = 65537,
    on_click_add = function()
        MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = 65537})})
    end,},
    [CurrencyType.Treasure] = {
        sprite_name = CommonItemId.SingleDraw,
        on_click_add = function()
            ViewMgr:CloseView(TreasureHuntView)
            MallCtrl.Instance:Jump(CommonItemId.SingleDraw)
        end,
        check_num = function()
            local item = Item.Create({item_id = CommonItemId.SingleDraw})
            return item:Num()
        end
    },
    [CurrencyType.Treasures] = {
        sprite_name = CommonItemId.FifthCompany,
        on_click_add = function()
            ViewMgr:CloseView(TreasureHuntView)
            MallCtrl.Instance:Jump(CommonItemId.FifthCompany)
        end,
        check_num = function()
            local item = Item.Create({item_id = CommonItemId.FifthCompany})
            return item:Num()
        end
    },
    [CurrencyType.Colosseum] = {
        sprite_name = CommonItemId.ArenaPoint,
        hide_add = true,
        value_color = COLORS.White,
        font_size = 26
    },
    [CurrencyType.Arena] = {
        sprite_name = CommonItemId.ArenaMatchPoint,
        hide_add = true,
        value_color = COLORS.White,
        font_size = 26
    },
    [CurrencyType.TiePetRope] = {
        sprite_name = CommonItemId.TiePetRope,
        on_click_add = function ()
            ViewMgr:CloseView(TreasureHuntView)
            MallCtrl.Instance:Jump(CommonItemId.TiePetRope)
        end
    },
    [CurrencyType.TiePetRopes] = {
        sprite_name = CommonItemId.TiePetRopes,
        on_click_add = function ()
            ViewMgr:CloseView(TreasureHuntView)
            MallCtrl.Instance:Jump(CommonItemId.TiePetRopes)
        end
    },
    [CurrencyType.SkyGlass] = {
        sprite_name = CommonItemId.SkyGlass,
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.SkyGlass]})})
        end
    },
    [CurrencyType.LuckCoin] = {
        sprite_name = CommonItemId.LuckCoin,
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.LuckCoin]})})
        end
    },
    [CurrencyType.PartnerLevelItem] = {
        sprite_name = CommonItemId.PartnerLevelItem,
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.PartnerLevelItem]})})
        end
    },
    [CurrencyType.MysteryFlush] = {
        sprite_name = CommonItemId.MysteryFlush,
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.MysteryFlush]})})
        end
    },
    [CurrencyType.JadePendant] = {
        sprite_name = CommonItemId.JadePendant,
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.JadePendant]})})
        end
    },
    [CurrencyType.CloudArena] = {
        sprite_name = CommonItemId.CloudArena,
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.CloudArena]})})
        end
    },
    [CurrencyType.DustCoin] = {
        sprite_name = CommonItemId.DustCoin,
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.DustCoin]})})
        end
    },
    [CurrencyType.SummerMazeTicket] = {
        sprite_name = CommonItemId.SummerMazeTicket,
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.SummerMazeTicket]})})
        end
    },
    [CurrencyType.Anecdote] = {
        sprite_name = CommonItemId.Anecdote,
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.Anecdote]})})
        end
    },
    [CurrencyType.HorcruxTianchengKey] = {
        sprite_name = CommonItemId.HorcruxTianchengKey,
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.HorcruxTianchengKey]})})
        end
    },
    [CurrencyType.MayDayKey] = {
        sprite_name = CommonItemId.MayDayKey,
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = CommonItemId.MayDayKey})})
        end,
        check_num = function()
            local item = Item.Create({item_id = CommonItemId.MayDayKey})
            return item:Num()
        end
    },
    [CurrencyType.PetStampPrayItem] = {
        sprite_name = CommonItemId.PetStampPrayItem,
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.PetStampPrayItem]})})
        end,
        check_num = function ()
            local item = Item.Create({item_id = CommonItemId.PetStampPrayItem})
            return item:Num()
        end
    },
    [CurrencyType.FairyDragon] = {
        sprite_name = Item.GetIconId(CommonItemId.FairyDragon),
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = CommonItemId.FairyDragon})})
        end,
        -- check_num = function()
        --     local item = Item.Create({item_id = CommonItemId.FairyDragon})
        --     return item:Num()
        -- end
    },
    [CurrencyType.GodSpaceExploits] = {
        sprite_name = Item.GetIconId(CommonItemId.GodSpaceExploits),
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = CommonItemId.GodSpaceExploits})})
        end,
        -- check_num = function()
        --     local item = Item.Create({item_id = CommonItemId.GodSpaceExploits})
        --     return item:Num()
        -- end
    },
    [CurrencyType.GodSpaceEssence] = {
        sprite_name = CommonItemId.GodSpaceEssence,
        on_click_add = function ()
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = CommonItemId.GodSpaceEssence})})
        end,
        -- check_num = function()
        --     local item = Item.Create({item_id = CommonItemId.GodSpaceEssence})
        --     return item:Num()
        -- end
	},
    [CurrencyType.Diamond] = {
        sprite_name = CommonItemId.Diamond,
        -- on_click_add = function ()
        --     ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = CommonItemId.Diamond})})
        -- end,
        hide_add = true
	},
}

local function default_add_btn_onclick(cur_type)

end

function UIWCurrencyShow:UIWCurrencyShow()
    self.data_care1 = nil
    self.data_care2 = nil
    self.show_types = {[1] = nil, [2] = nil, [3] = nil,[4] = nil}
    self.show_effect = {}
    self.effect_list = {[CurrencyType.CoinBind] = 10010001, [CurrencyType.Gold] = 10010002, [CurrencyType.ImmortalCoin] = 10010001}
    self:GuideInit()
end

function UIWCurrencyShow:GuideInit()
    GuideManager.Instance:RegisterGetGuideUi("MallCurrencyLingYu",function ()
        if self.GuideGame then
            return self.GuideGame
        end
    end)
end

function UIWCurrencyShow:flushIndex(index, data,old_value)
    local ty = self.show_types[index]
    if ty ~= nil and CurrencyTypeConfig[ty] ~= nil then
        local value_txt = self.Value[index]
        self.Obj[index]:SetActive(true)
        local ty_cofig = CurrencyTypeConfig[ty]
        if ty_cofig.check_num then
            value_txt.text = tostring(DataHelper.ConverMoney(ty_cofig.check_num()))
        else
            local new_value = data:GetCurrency(ty)
            if ty == CurrencyType.CoinBind or ty == CurrencyType.Gold or ty == CurrencyType.ImmortalCoin then
                if old_value == nil then
                    old_value = tonumber(value_txt.text)
                end
                if old_value and old_value > 0 and old_value < 10000000 and new_value < 10000000 and old_value - new_value ~= 0 then
                    self.show_effect[ty] = self.show_effect[ty] or {}
                    --在这个范围内可以做逻辑
                    if next(self.show_effect[ty]) then
                        TimeHelper:CancelTimer(self.show_effect[ty].handle)
                        self.show_effect[ty].value_txt.text = self.show_effect[ty].new_value
                        self.show_effect[ty] = {}
                    end
                    self.EffectTool:Play(self.effect_list[ty], self.EffectPos[index])
                    self.EffectTool:Play(10010003, self.Obj[index])
                    self.show_effect[ty].old_value = old_value
                    self.show_effect[ty].new_value = new_value
                    self.show_effect[ty].offset = (old_value - new_value) / 10
                    self.show_effect[ty].value_txt = value_txt
                    self.show_effect[ty].count = 0
                    self.show_effect[ty].handle =
                        TimeHelper:AddRunFrameTimer(
                        function()
                            self.show_effect[ty].value_txt.text = math.floor(self.show_effect[ty].old_value - self.show_effect[ty].offset)
                            self.show_effect[ty].old_value = tonumber(self.show_effect[ty].value_txt.text)
                            self.show_effect[ty].count = self.show_effect[ty].count + 1
                            if self.show_effect[ty].count == 10 then
                                self.show_effect[ty].old_value = self.show_effect[ty].new_value
                                self.show_effect[ty].value_txt.text = self.show_effect[ty].new_value
                            end
                        end,
                        3,
                        30
                    )
                else
                    if old_value and old_value > 0 and old_value - new_value ~= 0 then
                        self.EffectTool:Play(self.effect_list[ty], self.EffectPos[index])
                        self.EffectTool:Play(10010003, self.Obj[index])
                    end
                    value_txt.text = tostring(DataHelper.ConverMoney(new_value))
                end
            else
                value_txt.text = tostring(DataHelper.ConverMoney(new_value))
            end
        end
        value_txt.color = ty_cofig.value_color and ty_cofig.value_color or default_cfg.value_color
        value_txt.fontSize = ty_cofig.font_size and ty_cofig.font_size or default_cfg.font_size
        value_txt.alignment = ty_cofig.hide_add and UnityEngine.TextAnchor.MiddleCenter or UnityEngine.TextAnchor.MiddleRight
        self.BtnAdd[index]:SetActive(not ty_cofig.hide_add)
        self.Icon[index].SpriteName = StringIsEmpty(ty_cofig.sprite_name) and "HuoBi_T" or ty_cofig.sprite_name
    else
        self.Obj[index]:SetActive(false)
	end
end

function UIWCurrencyShow:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.data_care1 = RoleData.Instance:GetCurrencyData():Care(BindTool.Bind(self.FL, self))
    self.data_care2 = BagData.Instance.item_grid:Care(BindTool.Bind(self.FL, self))
end

function UIWCurrencyShow:OnDisable()
    if self.data_care1 ~= nil then
        RoleData.Instance:GetCurrencyData():Uncare(self.data_care1)
        self.data_care1 = nil
    end
    if self.data_care2 ~= nil then
        BagData.Instance.item_grid:Uncare(self.data_care2)
        self.data_care2 = nil
    end
    for k, v in pairs(self.show_effect) do
        TimeHelper:CancelTimer(v.handle)
    end
    self.show_effect = {}
    for index = 1, 3, 1 do
        self.Value[index].text = "0"
    end
end

function UIWCurrencyShow:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    for k, v in pairs(self.show_effect) do
        v.value_txt.text = "0"
        TimeHelper:CancelTimer(v.handle)
    end
    self.show_effect = {}
end

function UIWCurrencyShow:FL(text_arg)
    local data = RoleData.Instance
    local old_val = text_arg ~= nil and self.show_types[1] ~= nil and text_arg[self.show_types[1]] or nil
    if old_val ~= nil and type(old_val) ~= "number" then old_val = nil end
    self:flushIndex(1, data, old_val)
    old_val = text_arg ~= nil and self.show_types[2] ~= nil and text_arg[self.show_types[2]] or nil
    if old_val ~= nil and type(old_val) ~= "number" then old_val = nil end
    self:flushIndex(2, data, old_val)
    old_val = text_arg ~= nil and self.show_types[3] ~= nil and text_arg[self.show_types[3]] or nil
    if old_val ~= nil and type(old_val) ~= "number" then old_val = nil end
    self:flushIndex(3, data, old_val)
    old_val = text_arg ~= nil and self.show_types[4] ~= nil and text_arg[self.show_types[4]] or nil
    if old_val ~= nil and type(old_val) ~= "number" then old_val = nil end
    self:flushIndex(4, data, old_val)
end

function UIWCurrencyShow:SetData(type1, type2, type3, hide_normal)
    self.show_types[1] = type1
    self.show_types[2] = type2
    self.show_types[3] = type3
    self.show_types[4] = nil
    self.Normal:SetActive(type3 ~= nil and not hide_normal)
    self:FL()
end

function UIWCurrencyShow:DefaultSet()
    self:SetCurrency(CurrencyType.CoinBind,CurrencyType.ImmortalCoin,CurrencyType.Gold)
end

function UIWCurrencyShow:OnClickAdd(index)
    local ty = self.show_types[index]
    if ty ~= nil then
        local ty_cofig = CurrencyTypeConfig[ty]
        if ty_cofig.on_click_add ~= nil then
            ty_cofig.on_click_add()
        else
            default_add_btn_onclick(ty)
        end
    end
end

function UIWCurrencyShow:SetCurrency(...)
    local type_list = {...}
    for i = 1, 4 do
        self.show_types[i] = nil
    end
    for i=1,self.Obj:Length() do
        self.show_types[i] = type_list[i]
    end
    self:FL()
    self.Normal:SetActive(false)
end