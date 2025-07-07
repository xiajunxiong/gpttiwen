GuildShop = GuildShop or DeclareMono("GuildShop", UIWFlushPanel)

function GuildShop:GuildShop()
    self.data = GuildData.Instance
    self.lang = Language.Guild
    self.data_cares = {
        {data = self.data.shop_buy_list, func = self.FlushShopList, init_call = false},
        {data = self.data.guild_info, func = self.FlushShopList, init_call = false},
        {data = self.data.guild_shop_first_item, func = self.FlushShopList, keys = {"flush"}, init_call = false},
    }
    self.click_times = 0
end
function GuildShop:OnClickTip()
    ViewMgr:OpenView(GuildHonourDesc)
end
function GuildShop:onFlush()
    UH.SetText(self.Tips, Language.Guild.GuildShopTip)
    self.TypeList:SetData(Config.family_auto.page_type) --这个page_type在guild_data中有另外处理不一定是和表里原本的一致
    self.TypeList:ClickItem(1)
    self.ShopList:SetData(self.data:GetShopInfo())
    -- self.ShopList:ClickItem(1)
end
function GuildShop:FlushShopList()
    local data = self.data:GetShopInfo(self.cur_select or 0)
    self.ShopList:SetData(data)
    -- self.ShopList:ClickItem(1)
    if self.select_data then
        self.ShopList:ClickItemData(self.select_data)
    end
end

function GuildShop:OnClickItem(param)
    local data = self.data:GetShopInfo(param.page_type - 1)
    self.ShopList:SetData(data)
    self.ShopList:ClickItem(1)
    self.cur_select = param.page_type - 1
end
function GuildShop:OnClickShopItem(selected, data)
    -- LogDG("12346")
    if data.seq == 1 then
        self.click_times = self.click_times + 1
        if self.click_times == 2 and self.data.guild_shop_first_item.count > 0 then
            local num = TimeManager.DayStart() + 2
            UnityPlayerPrefs.SetInt(PrefKeys.GuildShopItem1Click2(), num)
            self.data.guild_shop_first_item.count = 0
            self.data.guild_shop_first_item.flush = not self.data.guild_shop_first_item.flush
        end
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
    --[[ if self.select_data then
        self.select_data.select_item:Switch(false)    
    end ]]
    self.select_data = data
    local num = self.data:GetShopBuyCount(data.seq)
    local level = self.data:GetGuildInfo().level or 0
    self.make_num = 1--data.buy_type == -1 and 1 or data.buy_num - num
    UH.SetText(self.Name, data.item:Name())
    UH.SetText(self.Level, string.format(self.lang.NeedLevel, data.level))

    -- UH.SetText(self.Price, string.format(self.lang.Price, data.original_cost, Language.Common.HuoBi[data.cost_type]))
    -- UH.SetText(self.Cost, string.format(self.lang.Cost, data.present_cost, Language.Common.HuoBi[data.cost_type]))
    UH.SetText(self.Count, data.buy_type == -1 and Language.Common.NoLimitTimes or string.format(self.lang.Count2[data.buy_type], data.buy_num - num >= 0 and data.buy_num - num or 0, data.buy_num))

    UH.SetText(self.SNum, data.gongxian_cost)
    -- UH.SetText(self.SNum, data.gongxian_cost)

    UH.SetText(self.JianJie, string.format(Language.Mall.Content, data.item:Desc()))
    UH.Color(self.Level, data.level > level and COLORS.Red8 or COLORS.Yellow13)
    self:FlushResult()
    for i = 1, self.HuoBiList:Length() do
        --UH.SpriteName(self.HuoBiList[i], 3)--HuoBi[data.cost_type])
        UH.SetGold(self.HuoBiList[i], 3)
    end
    --[[ self.select_data.select_item:Switch(true) ]]
    --[[ for i = 1, self.RateObjs:Length(), 1 do
        self.RateObjs[i]:SetActive(data.original_cost > data.present_cost)
    end ]]
end
function GuildShop:FlushResult()
    UH.SetText(self.BuyNum, self.make_num)
    UH.SetText(self.TNum, self.select_data.gongxian_cost * self.make_num)
end
function GuildShop:OnClickAdd(num)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    self.make_num = self.make_num + num
    local num = self.data:GetShopBuyCount(self.select_data.seq)
    local buy_num = self.select_data.buy_type == -1 and 99 or self.select_data.buy_num
    local can_buy = buy_num - num >= 0 and buy_num - num or 0
    if self.make_num < 0 then
        self.make_num = 0
    end
    if self.make_num > can_buy then
        self.make_num = can_buy
    end
    self:FlushResult()
end
function GuildShop:OnClickBuy()
    local info = self.data:GetGuildInfo()
    if info.level < self.select_data.level then
        PublicPopupCtrl.Instance:Center(ErrorText[31])
        return
    end
    if self.make_num == 0 then
        return
    end
    local param_t = {
        guild_id = info.guild_id,
        shop_seq = self.select_data.seq,
        count = self.make_num
    }
    GuildCtrl.SendGuildShopBuy(param_t)
end
GuildShopItem = GuildShopItem or DeclareMono("GuildShopItem", UIWidgetBaseItem)
function GuildShopItem:GuildShopItem()
    self.guild_data = GuildData.Instance
    self.lang = Language.Guild
end
function GuildShopItem:SetData(data)
    local item = Item.New(data)
    data.item = item
    data.select_item = self
    UIWidgetBaseItem.SetData(self, data)
    local count = self.guild_data:GetShopBuyCount(data.seq)
    local level = self.guild_data:GetGuildInfo().level or 0
    local flag = level >= data.level
    self.UnlockObj:SetActive(flag)
    self.LockObj:SetActive(not flag)
    UH.SetText(self.LblCount, data.buy_type == -1 and Language.Common.NoLimitTimes or string.format(Language.Common.StuffTimes, data.buy_num - count))
    UH.SetText(self.Name, item:Name())
    UH.SetText(self.Level, string.format(Language.Guild.Level, data.level))
    UH.SetText(self.GongXian, data.gongxian_cost)
    UH.SetText(self.Cost, data.present_cost)
    UH.SetText(self.LblLimit, string.format(Language.Guild.Level, data.level))
    self.Cost1:SetActive(data.gongxian_cost ~= 0)
    self.Cost2:SetActive(data.present_cost ~= 0)
    self.gameObject:SetActive(data.present_cost ~= 0 or data.gongxian_cost ~= 0)
    -- local num = self.guild_data:GetShopBuyCount(data.seq)
    -- self.Over:SetActive(data.buy_num - num == 0)
    self.Cell:SetData(item)
    UH.SpriteName(self.HuoBi, HuoBi[data.cost_type])
    self:Switch(false)
end

function GuildShopItem:Click()
    --[[ if self.ItemClick then
        self.ItemClick:Trigger()
    end ]]
    if self.ToggleClick then
        self.ToggleClick:Trigger()
    end
end
function GuildShopItem:Switch(flag)
    for i = 1, self.ToggleList:Length() do
        self.ToggleList[i].isOn = flag
    end
end
----------------------------GuildBtnItem----------------------------
GuildBtnItem = GuildBtnItem or DeclareMono("GuildBtnItem", UIWidgetBaseItem)

function GuildBtnItem:SetData(data)
    UH.SetText(self.Name, data.name)
    UIWidgetBaseItem.SetData(self, data)
end
function GuildBtnItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end


----------------------------GuildReputationItem----------------------------
GuildReputationItem = GuildReputationItem or DeclareMono("GuildReputationItem", UIWidgetBaseItem)
function GuildReputationItem:GuildReputationItem()
    self.guild_data = GuildData.Instance
end

function GuildReputationItem:SetData(data)
    data.item = Item.Create(data)
    UH.SetText(self.Name, data.item:Name())
    UH.SetGold(self.Gold, 3)

    if data.cost_discount ~= 100 and data.is_fixed_discount then
        self.RedPoint:SetNum(self.guild_data.guild_shop_first_item.count)
    end
    UH.SetText(self.Num, data.gongxian_cost)
    self.Limit:SetActive(data.buy_num > 0)
    self.Discount:SetActive(data.cost_discount ~= 100)
    UH.SetText(self.DiscountTex, string.format(Language.Guild.ItemDiscount, data.cost_discount / 10))
    if data.buy_num > 0 then
        local count = self.guild_data:GetShopBuyCount(data.seq)
        local has_times = data.buy_num - count
        UH.SetText(self.LimitNum, Format(Language.Shop.LimitNum, data.buy_num))
        self.Inter.Interactable = has_times > 0
        self.Panel:SetActive(has_times > 0)
        self.SellOut:SetActive(has_times <= 0)
    else
        self.Inter.Interactable = true
        self.Panel:SetActive(true)
        self.SellOut:SetActive(false)
    end
    --家族等级不足
    local level = self.guild_data:GetGuildInfo().level or 0
    self.LimitTip:SetActive(level < data.level)
    self.Cost:SetActive(level >= data.level)
    data.item.is_grey = level < data.level
    self.Call:SetData(data.item)
    -- UH.SpriteName(self.ProfIcon,ProfSettingB[RoleData.Instance:GetRoleProf()])
    -- self.ProfIcon:SetObjActive(data.item:ShowType() == 17 and PrestigeData.Instance:IsShopProfItem(data.item.item_id))
    UIWidgetBaseItem.SetData(self, data)
end

function GuildReputationItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    --[[ if selected then
        PrestigeData.Instance:SetSelectData(self.data)
    end ]]
end
function GuildReputationItem:Click()
    --[[ if self.ItemClick then
        self.ItemClick:Trigger()
    end ]]
    if self.ToggleClick then
        self.ToggleClick:Trigger()
    end
end