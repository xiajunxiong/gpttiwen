--交易出售列表
----------------------------SellShopItem----------------------------
SellShopItem = SellShopItem or DeclareMono("SellShopItem", UIWidgetBaseItem)

function SellShopItem:SetData(data)
    local has_num = 0
    if data.item_id and data.item_id == 0 then
        self.Show:SetActive(false)
        UIWidgetBaseItem.SetData(self,nil)
    else
        self.Show:SetActive(true)
        data.item = Item.Create(data.item_data)
        if data.item:IsPet() then
            data.item_data.param.index = -1
            data.pet = Pet.New(data.item_data.param)
            self.Call:SetData(data.pet)
            UH.SetText(self.Name, data.pet:Name())
            if data.pet.IsLianYao then
                UH.Color(self.Name, data.pet:IsLianYao() and COLORS.Pink2 or COLORS.Red1)
                if self.YaoObj then
                    self.YaoObj:SetActive(data.pet:IsLianYao())
                end
            end
        else
            self.Call:SetData(data.item)
            UH.SetText(self.Name, data.item:Name())
        end
        UH.SetText(self.Num, data.price)
        UIWidgetBaseItem.SetData(self, data)
        --显示角标 优先显示取现图标
        local status = data.sold_num > 0 and 2 or data.cur_status
        UH.SetText(self.NLabel, Language.Mall.StatusName[status])
        UH.SpriteName(self.ILabel,Language.Mall.StatusImgName[status])
        self.Label:SetActive(data.cur_status == 1 or data.cur_status == 4 or data.sold_num > 0)
        has_num = (data.sold_num > 0 or data.cur_status == 4)and 1 or 0
        self.group_num = data.item:GroupNum()
        -- self.Group:SetActive(self.group_num ~= 1)
        -- UH.SetText(self.TGroup,Format(Language.Mall.Group,self.group_num))
        if self.group_num ~= 1 then
            local num = data.item:Num() / self.group_num
            CellFlushs.SetNum(self.Call,num .. Language.Mall.GroupTip)
        end
    end
    --检查并显示红点
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.Root, Vector3.New(165,50,0))
    end
    self.red_point:SetNum(has_num)
end

function SellShopItem:Click()
	self.ToggleClick:Trigger()
end

function SellShopItem:OnClickPop()
    ViewMgr:OpenView(NoticeView,self.data)
end

----------------------------TradeShopBtnItem----------------------------
TradeShopBtnItem = TradeShopBtnItem or DeclareMono("TradeShopBtnItem", UIWidgetBaseItem)

function TradeShopBtnItem:SetData(data)
    UH.SetText(self.Name, data.name)
    UIWidgetBaseItem.SetData(self, data)
end

----------------------------TradeTypeItem----------------------------
TradeTypeItem = TradeTypeItem or DeclareMono("TradeTypeItem", UIWidgetBaseItem)

function TradeTypeItem:SetData(data)
    UH.SetText(self.Name, data.name)
    UH.SpriteName(self.ImgType,data.icon_name .. "_off")
    UH.SpriteName(self.ImgSelect,data.icon_name.."_on")
    UIWidgetBaseItem.SetData(self, data)
end

function TradeTypeItem:Click()
	self.ToggleClick:Trigger()
end

--交易列表
--===========================SellItem=====================================
SellItem = SellItem or DeclareMono("SellItem", UIWidgetBaseItem)

function SellItem:SetData(data)
    data.item = Item.Create(data)
    if data.item:IsPet() then
        data.param.index = -1
        data.pet = Pet.New(data.param)
        self.Call:SetData(data.pet)
        UH.SetText(self.Name, data.pet:Name())
        UH.Color(self.Name, data.pet:IsLianYao() and COLORS.Pink2 or COLORS.Yellow13)
        if self.YaoObj then
            self.YaoObj:SetActive(data.pet:IsLianYao())
        end
    else
        self.Call:SetData(data.item)
        UH.SetText(self.Name, data.item:Name())
    end
    UH.SetText(self.Num, data.price)
    UH.SetGold(self.Gold,CurrencyType.Gold)
    self.group_num = data.item:GroupNum()
    self.Group:SetActive(self.group_num ~= 1)
    UH.SetText(self.TGroup,Format(Language.Mall.Group,self.group_num))
    if self.group_num ~= 1 then
        local num = data.param.num / self.group_num
        CellFlushs.SetNum(self.Call,num .. Language.Mall.GroupTip)
    end
    UIWidgetBaseItem.SetData(self, data)
end

--===========================SellSkillItem=====================================
SellSkillItem = SellSkillItem or DeclareMono("SellSkillItem", UIWidgetBaseItem)

function SellSkillItem:SetData(data)
    UH.SetText(self.Name, data.skill_name or data.name)
    if data.icon_id then
        UH.SetIcon(self.Icon, data.icon_id, ICON_TYPE.SKILL)
    else
        UH.SkillIcon(self.Icon, data.skill_id or 0)
    end
    UIWidgetBaseItem.SetData(self, data)
end

function SellSkillItem:Click()
	self.ToggleClick:Trigger()
end

function SellSkillItem:OnClickSellItem()
    local gift_cfg = PetData.Instance:GetGiftDetail(self.data.skill_id)
    if gift_cfg and gift_cfg.name then
        PetData.Instance:SetClickSkill({gift_id = self.data.skill_id})
    else
        PetData.Instance:SetClickSkill({skill_cfg = self.data})
    end
    ViewMgr:OpenView(PetSkillTip)
end

--新修改 
--===========================TradeItem=====================================
TradeItem = TradeItem or DeclareMono("TradeItem", UIWidgetBaseItem)

function TradeItem:SetData(data)
    for i=1,3 do
        self.ItemList[i]:SetActive(data.flush_index == i)
    end
    if data.flush_index and data.flush_index ~= 0 then
        self.ItemCall[data.flush_index]:SetData(data)
    end
    if data.upshelve_time and TradeData.Instance:IsOpenLink(data.upshelve_time) then
        TradeData.Instance:SetSelectItemData(data)
        TradeData.Instance:SetOpenLinkData()
        self.show_time = TimeHelper:AddDelayTimer(function()
            if TradeData.Instance.shop_data.announce and TradeData.Instance.shop_data.announce == 1 then
                TradeCtrl.Instance:OnenPurchaseView()
            end
			self:SetSelected(true)
        end, 0.2)
    end
    UIWidgetBaseItem.SetData(self, data)
end

function TradeItem:OnRelease()
	UIWidgetBaseItem.OnRelease(self)
	TimeHelper:CancelTimer(self.show_time)
end

--点击关注
function TradeItem:OnClickFollow()
    if self.data.focus_index ~= nil then
        PublicPopupCtrl.Instance:Center(Language.Trade.FollowTip[2])
        TradeCtrl.Instance:SendRemoveFocusReq(self.data.focus_index)
    else
        if TradeData.Instance:IsMaxFocus() then
            PublicPopupCtrl.Instance:Center(Language.Trade.FollowTip[4])
            return
        end
        if TradeData.Instance:IsFocusList(self.data.upshelve_time) then
            PublicPopupCtrl.Instance:Center(Language.Trade.FollowTip[3])
            return
        end
        --PublicPopupCtrl.Instance:Center(Language.Trade.FollowTip[1])
        TradeCtrl.Instance:SendAddFocusReq(self.data.seller_uid,self.data.sale_index,self.data.upshelve_time)
    end
end

function TradeItem:OnClickRecord()
    ViewMgr:OpenView(DealRecordView,self.data.item_data.item_id)
end

function TradeItem:Click()
	self.ToggleClick:Trigger()
end

function TradeItem:FlushSearchView()
    if self.ItemCall[2] then
        self.ItemCall[2]:FlushSearchView()
    end
end

--一般物品刷新
--===========================TradeShopItem=====================================
TradeShopItem = TradeShopItem or DeclareMono("TradeShopItem", UIWidgetBaseItem)

function TradeShopItem:SetData(data)
    self.Time:SetTime("")
    self.Time:CloseCountDownTime()
    data.item = Item.Create(data.item_data)
    if data.item:IsPet() then
        data.item_data.param.index = -1
        data.pet = Pet.New(data.item_data.param)
        self.Call:SetData(data.pet)
        UH.SetText(self.Name, data.pet:Name())
        if data.pet.IsLianYao then
            UH.Color(self.Name, data.pet:IsLianYao() and COLORS.Pink2 or COLORS.Red2)
            if self.YaoObj then
                self.YaoObj:SetActive(data.pet:IsLianYao())
            end
        end
    else
        self.Call:SetData(data.item)
        UH.SetText(self.Name, data.item:Name())
    end
    UH.SetText(self.Num, data.price)
    local can_follow = self:IsRoleItem(data.item)
    self.BtnFollow:SetActive(can_follow)
    self.Follow:SetActive(can_follow and self:IsFocus(data))
    if TradeData.Instance:IsAnnounce() == 1 then
        self.Time:StampTime(data.next_status_time or 0)
    else
        self.Time:SetTime("")
    end
    self.group_num = data.item:GroupNum()
    self.Group:SetActive(self.group_num ~= 1)
    if self.group_num ~= 1 then
        local num = data.item:Num() / self.group_num
        CellFlushs.SetNum(self.Call,num .. Language.Mall.GroupTip)
    end
    UH.SetText(self.TGroup,Format(Language.Mall.Group,self.group_num))
end

--必须宠物、装备、灵饰才可以关注
function TradeShopItem:IsRoleItem(item)
    return item:IsPet() or item:IsEquip() or item:IsJewelry()
end

--是否关注
function TradeShopItem:IsFocus(data)
    return data.focus_index ~= nil or TradeData.Instance:IsFocusList(data.upshelve_time)
end

--交易商店分类item
--===========================TradeShopTypeItem=====================================
TradeShopTypeItem = TradeShopTypeItem or DeclareMono("TradeShopTypeItem", UIWidgetBaseItem)

function TradeShopTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if data.desc ~= "" then
        UH.SetText(self.Name,Format("%s（%s）",data.name,data.desc))
    else
        UH.SetText(self.Name,data.name)
    end
    UH.SpriteName(self.Icon,data.icon_name)
    UH.SetText(self.Desc,"")
end

function TradeShopTypeItem:FlushSearchView()
    if self.data == nil then return end
    local type_num = TradeData.Instance:GetBigSearchNum(self.data.seq)
    if self.data.item_type == 3 then
        local config = TradeData.Instance:GetItemTypeConfig(self.data.item_type)
        local type_name = self.data.name .. string.gsub(config.name,"  ","")
        UH.SetText(self.Desc,Format(Language.Trade.SellBigSearchTip,type_name,type_num))
    else
        UH.SetText(self.Desc,Format(Language.Trade.SellBigSearchTip,self.data.name,type_num)) 
    end
end

--交易商店分类item
--===========================TradeSearchItem=====================================
TradeSearchItem = TradeSearchItem or DeclareMono("TradeSearchItem", UIWidgetBaseItem)

function TradeSearchItem:SetData(data)
    local item_id = data.item_id
    UH.SetText(self.Name,data.name)
    self.Call:SetData(Item.Init(item_id))
end

--==============================TradeAttribute===============================
TradeAttribute = TradeAttribute or DeclareMono("TradeAttribute", UIWidgetBaseItem)

function TradeAttribute:SetData(data)
    UH.SetText(self.Name,data.name)
    UH.SetText(self.Desc,data.desc)
    self.BaseItem:SetData(data.attr_list)
    self.BaseSmelt:SetData(data.inc_list or {})
    UH.SetText(self.EffectDesc,data.effect_desc or "")
    UH.SetText(self.CreaterName,data.creater_name or "")

    self.Smelt:SetActive(data.inc_list ~= nil)
    self.Effect:SetActive(data.effect_desc ~= nil)
    self.Creater:SetActive(data.creater_name ~= nil)
end

--==============================TradeAttributeBaseItem===============================
TradeAttributeBaseItem = TradeAttributeBaseItem or DeclareMono("TradeAttributeBaseItem", UIWidgetBaseItem)

function TradeAttributeBaseItem:SetData(data)
    if data.attr_type then
        local name = Language.Common.AttrList[data.attr_type]
        local value = DataHelper.Percent(data.attr_type,data.attr_value)
        UH.SetText(self.Content,Format("<color=#%s>%s   + %s</color>",COLORSTR.Yellow13,name,value))
    else
        UH.SetText(self.Content,Language.Common.Wu)
    end
    self:FlushIconView(data.attr_type,data.attr_value)
end

function TradeAttributeBaseItem:FlushIconView(attr_type,attr_value)
    if self.ImgAdd == nil then return end
    if attr_type ~= nil then
        self.ImgAdd:SetActive(TradeData.Instance:HasPromote(attr_type,attr_value))
    else
        self.ImgAdd:SetActive(false)
    end
end

--==============================TradeAttributeSmeltItem===============================
TradeAttributeSmeltItem = TradeAttributeSmeltItem or DeclareMono("TradeAttributeSmeltItem", UIWidgetBaseItem)

function TradeAttributeSmeltItem:SetData(data)
    if data.attr_type then
        local name = Language.Common.AttrList[data.attr_type]
        local value = DataHelper.Percent(data.attr_type,data.attr_value)
        local color =  QualityColorStr[data.color]
        UH.SetText(self.Content,Format("<color=#%s>%s    +%s</color>",color,name,value))
        self.Point:SetActive(DataHelper.IsPoint(data.attr_type))--是否加点
    else
        self.Point:SetActive(false)
        UH.SetText(self.Content,Language.Common.Wu)
    end
    self:FlushIconView(data.attr_type,data.attr_value)
end

function TradeAttributeSmeltItem:FlushIconView(attr_type,attr_value)
    if self.Smelt == nil then return end
    if attr_type ~= nil then
        self.Smelt:SetActive(TradeData.Instance:HasSmelt(attr_type,attr_value))
    else
        self.Smelt:SetActive(false)
    end
end