
PartnerLifeView = PartnerLifeView or DeclareView("PartnerLifeView", PartnerConfig.ResPath .. "partner_life")

function PartnerLifeView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(), "", Vector2.New(1085, 640))
end


PartnerLifeViewF = PartnerLifeViewF or DeclareMono("PartnerLifeViewF", UIWFlushPanel)

function PartnerLifeViewF:PartnerLifeViewF()
    self.flush_events = {EventProtocol.ToEventKey(SCPartnerEqSingleSlots)}
    
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Hole
    self.loop_setting = PartnerConfig.LoopSetting

    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.onFlush, init_call = false},
        {data = self.data.item_flush, func = self.onFlush, init_call = false},
        {data = self.data.partner_life_effect, func = self.LockEffect, init_call = false},
    }
end

function PartnerLifeViewF:OnEnable()
    -- self.data.cur_sel_tabbar.val = PartnerConfig.PartnerViewTabs.Life
    self:onFlush()
    self.data:LifeActiveRedPointShow()
end

function PartnerLifeViewF:onFlush()
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    local vo = data.vo
    local pe_list = data.pe_list
    local pe_suit_list = data.pe_suit_list
    local not_empty = false

    self.red_point_items = self.red_point_items or {}
    local partner_pe_list = self.data:GetPartnerPEList(data.info.job)
    for i = 1, PartnerConfig.PARTNER_PE_MAX do
        self.red_point_items[i] = self.red_point_items[i] or UiRedPoint.New(self.PEObjs[i], Vector3.New(40,40,0))
        local pe = pe_list[i]
        local ppe = partner_pe_list[i]
        local value = pe.item_id > 0
        local rpn = 0
        if PartnerConfig.LockState.lock == vo.is_unlocked or PartnerConfig.FightState.rest == vo.fight_state then
            rpn = 0
        else
            if data.life_equip_red_point_show and ppe.num > 0 and pe.vo.unlock_level >= ppe.min_level then
                if not value then
                    rpn = 1
                -- elseif ppe.max_level > pe.limit_level then
                --     rpn = 1
                -- elseif pe:PESuitProf() ~= RoleData.Instance:ProfType() and ppe.max_level == pe.limit_level then
                --     rpn = 1
                end
            end
        end
        self.red_point_items[i]:SetNum(rpn)
        not_empty = not_empty or value
        self.PEItems[i]:SetData(pe, vo)
    end

    self.SuitEmptyObj:SetActive(not not_empty)
    self.SuitList:SetData(pe_suit_list)

    local role_level = RoleData.Instance:GetRoleLevel()
    local hole_desc_co = Cfg.PartnerHoleDescCoByHoleLevel(math.ceil(role_level / 10) * 10)
    -- if nil ~= hole_desc_co then self.HoleDesc.text = string.format(self.language.TipsDesc, hole_desc_co.hole_level, hole_desc_co.hole_desc) end
end

function PartnerLifeViewF:LockEffect()
    if self.data.partner_life_effect.pos > 0 then
        self.LockEffects[self.data.partner_life_effect.pos]:Play(10010704)
        self.data.partner_life_effect.pos = 0
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.PartnerLockOpen)
    end
end

function PartnerLifeViewF:OnClickView()
    ViewMgr:OpenView(PartnerLifeSuitView)
end

function PartnerLifeViewF:OnClickTips()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[12].desc)
end

function PartnerLifeViewF:OnClickGet()
    -- ViewMgr:OpenViewByKey(Mod.FightFieldBoss.Main)
    ViewMgr:OpenView(MysteryStoreView)
end

function PartnerLifeViewF:OnClickStore()
    ViewMgr:OpenView(MysteryStoreView)
end

function PartnerLifeViewF:OnClickHandbook()
    ViewMgr:OpenView(PartnerLifeHandbookView)
end

PartnerLifeViewItem = PartnerLifeViewItem or DeclareMono("PartnerLifeViewItem", UIWidgetBaseItem)

function PartnerLifeViewItem:PartnerLifeViewItem()
    self.partner_data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Life
end

function PartnerLifeViewItem:SetData(data, vo)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    local empty = 0 == data.item_id
    -- local locked = 0 == data.vo.unlock_level
    local locked = PartnerConfig.LockState.lock == vo.is_unlocked
    self.AttrsObj:SetActive(not empty)
    if not empty then
        local attr_list = data.attr_list
        for i = 1, PartnerConfig.PARTNER_EQUIP_VOC_NUM do
            local attr = attr_list[i]
            self.Attrs[i]:SetObjActive(0 ~= attr.attr_value)
            self.Attrs[i].text = string.format(self.language.AttrShow, Language.Common.AttrList[attr.attr_type], DataHelper.IsPercent(attr.attr_type) and Percent(attr.attr_value) or attr.attr_value)
        end
    end
    -- self.Attr.text = string.format(self.language.attr_desc, Language.Common.AttrList[data.attr_type]) .. " +" .. (DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value) or data.attr_value)
    -- self.AttrNum.text = "+" .. data.attr_value
    self.ItemImage.enabled = not empty
    -- self.UnlockObj:SetActive(data.vo.unlock_level + 10 <= RoleData.Instance:GetRoleLevel())
    self.SuoObj:SetActive(locked)
    self.AddObj:SetActive(not locked and empty)
    self.ActiveEffect:SetActive(locked and RoleData.Instance:GetCurrency(CurrencyType.LuckCoin) >= 160)
end

function PartnerLifeViewItem:OnClickUnlock(type)
    local succ, cur_sel_pe_hole = self.partner_data:GetPEHoleCo(self.partner_data.cur_sel_partner, type, self.data.vo.unlock_level)
    if succ then
        self.partner_data.cur_sel_pe_hole = cur_sel_pe_hole
        ViewMgr:OpenView(PartnerHoleView)
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    end
end

function PartnerLifeViewItem:OnClickCellItem(type)
    local partner = self.partner_data:GetPartnerInfoById(self.partner_data.cur_sel_partner)
	if partner and PartnerConfig.LockState.lock == partner.vo.is_unlocked then
		PublicPopupCtrl.Instance:Error(Language.Partner.UnLockTips)
    else
        if 0 == self.data.vo.unlock_level then
            self:OnClickUnlock(type)
            -- PublicPopupCtrl.Instance:Error(Language.Partner.UnLockPETips)
        else
            self.partner_data.cur_sel_pe_type = type
            ViewMgr:OpenView(PartnerLifeEquipView)
            if partner.life_equip_red_point_show then
                partner.life_equip_red_point_show = false
                self.partner_data:PartnerLifeRedPoint()
                ViewMgr:FlushView(PartnerView, PartnerLifeViewF:GetTypeName())
            end
			--AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
        end
    end
end

PartnerLifeViewSuitItem = PartnerLifeViewSuitItem or DeclareMono("PartnerLifeViewSuitItem", UIWidgetBaseItem)

function PartnerLifeViewSuitItem:PartnerLifeViewSuitItem()
    self.language = Language.Partner.Life
end

function PartnerLifeViewSuitItem:SetData(data)
    local is_a = data.is_a
    local color = data.color or 3
    self.ActiveObj:SetActive(is_a)
    self.ActiveNObj:SetActive(not is_a)
    -- self.LineSp.SpriteName = is_a and "JiHuoDi1" or "WeiJiHuoDi1"
    -- self.BgSp.SpriteName = is_a and "JiHuoDi" or "WeiJiHuoDi"
    self.BgSp.SpriteName = "JiHuo_c" .. color
    self.Name.text = RichTextHelper.ColorStr(data.name, COLORSTR[PartnerConfig.LifeColor[color]])
    -- self.Desc.text = RichTextHelper.ColorStr(data.name .. self.language.suit_desc, is_a and COLORSTR.Red3 or COLORSTR.Gray4)
    --     .. RichTextHelper.ColorStr(data.desc .. (is_a and "" or self.language.suit_nota), is_a and COLORSTR.Green7 or COLORSTR.Gray4)
    -- self.Desc.text = RichTextHelper.ColorStr(data.desc .. (is_a and "" or self.language.suit_nota), is_a and COLORSTR.Green7 or COLORSTR.Gray4)
    self.Desc.text = RichTextHelper.ColorStr(data.desc , is_a and COLORSTR.Green10 or COLORSTR.Gray2)
end


PartnerLifeEquipView = PartnerLifeEquipView or DeclareView("PartnerLifeEquipView", PartnerConfig.ResPath .. "partner_life_show")

function PartnerLifeEquipView:PartnerLifeEquipView()
	self.flush_events = {EventProtocol.ToEventKey(SCPartnerEqSingleSlots)}

	self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner
	self.data.cur_sel_pe = nil
end

function PartnerLifeEquipView:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.LifeCard, Vector2.New(470, 641))
end

function PartnerLifeEquipView:CloseCallback()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end


function PartnerLifeEquipView:OnFlush()
	self.BtnEquipInter.Interactable = false
	if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
	local vo = data.vo
	local pes = vo.pes
	local pe_list = data.pe_list

	local cur_sel_pe_type = self.data.cur_sel_pe_type
	local btn_equip = self.language.Life.btn_equip
	self.BtnEquip.text = btn_equip.equip
	
	local list = BagData.Instance:GetPartnerEquipGrid(self.data.cur_sel_pe_type)
	local pe = pes[cur_sel_pe_type] 
	for _, item in ipairs(list) do
		item.unlock_level = pe.unlock_level
	end
	if pes[cur_sel_pe_type].item_id ~= 0 then table.insert(list, 1, pe_list[cur_sel_pe_type]) end
	self.EmptyObj:SetActive(0 == #list)

	table.sort(list, function(a, b)
        local itemA = (nil == a.unlock_level or a.unlock_level >= a.limit_level) and 10000 or 0
        local itemB = (nil == b.unlock_level or b.unlock_level >= b.limit_level) and 10000 or 0
        if (itemA +  a:Color() + a:LimitLv()*10) > (itemB + b:Color()+ b:LimitLv()*10)  then
            return true
        else
            return false
        end
    end)

	self.EquipList:SetData(list)
end

function PartnerLifeEquipView:OnClickEquip()
	if nil ~= self.data.cur_sel_pe then
		if self.data.cur_sel_pe.index then
			AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ZhuangBeiFuShuJia)
			PartnerCtrl.Instance:SendPartnerEqOperatorReq(PartnerConfig.PEOperType.equip, self.data.cur_sel_partner, self.data.cur_sel_pe.index)
		else
			AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ZhuangBeiFuShuJian)
			PartnerCtrl.Instance:SendPartnerEqOperatorReq(PartnerConfig.PEOperType.remove, self.data.cur_sel_partner, self.data.cur_sel_pe_type - 1)
		end
		self.data.cur_sel_pe = nil
	end
	ViewMgr:CloseView(PartnerLifeEquipView)
end

PartnerLifeEquipViewItem = DeclareMono("PartnerLifeEquipViewItem", UIWidgetBaseItem)
function PartnerLifeEquipViewItem:PartnerLifeEquipViewItem()
	self.partner_data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner.Life
end

function PartnerLifeEquipViewItem:ExpendSize(data, axis)
	return data.attr_num > 2 and 270 or 224
end

function PartnerLifeEquipViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	local is_actived = nil == data.unlock_level or data.unlock_level >= data.limit_level
	self.Inter.Interactable = is_actived
	-- data.is_grey = not is_actived

	local detail, suit = self.partner_data:GetPEConfigByItemId(data.item_id)
	-- local attr_type = data.attr_type
	-- local attr_value = data.attr_value
	self.Cell:SetData(data)
	self.Cell:MakeGrey(not is_actived)
	self.Name.text = data:QuaName()
	self.Level.text = is_actived and string.format(self.language.level, data:Level()) or string.format(self.language.level_limit, data.limit_level)
	-- self.Attr.text = Language.Common.AttrList[attr_type] .. "+" .. (DataHelper.IsPercent(attr_type) and Percent(attr_value) or attr_value)
	self.Suit.text = string.format(self.language.suit, suit.desc)
	self.LevelTip:SetActive(not is_actived)
	self.BlockObj:SetActive(not is_actived)
	self.EquipObj:SetActive(not SmartData.Check(data))

	local attr_list = data.attr_list
	for i = 1, PartnerConfig.PARTNER_EQUIP_VOC_NUM do
		local attr = attr_list[i]
		self.AttrObjs[i]:SetActive(0 ~= attr.attr_value and nil ~= Language.Common.AttrList[attr.attr_type])
		if nil ~= Language.Common.AttrList[attr.attr_type] then
			self.Attrs[i].text = Language.Common.AttrList[attr.attr_type] .. "+" .. (DataHelper.IsPercent(attr.attr_type) and Percent(attr.attr_value) or attr.attr_value)
		end
	end
end

function PartnerLifeEquipViewItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function PartnerLifeEquipViewItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        local data = self.partner_data:GetPartnerInfoById(self.partner_data.cur_sel_partner)
		local vo = data.vo
		local pes = vo.pes
		local cur_sel_pe_type = self.partner_data.cur_sel_pe_type
		local btn_equip = self.language.btn_equip
		local is_equip = 0 ~= pes[cur_sel_pe_type].item_id

		if is_equip then
			self.BtnEquip.text = btn_equip[self.data.index and "replace" or "remove"]
		else
			self.BtnEquip.text = btn_equip.equip
		end
		self.BtnEquipInter.Interactable = true
		self.partner_data.cur_sel_pe = self.data
    end
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function PartnerLifeEquipViewItem:OnClickBlock()
    PublicPopupCtrl.Instance:Center(self.language.block_tips)
end