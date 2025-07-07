
PartnerCrystalView = PartnerCrystalView or DeclareView("PartnerCrystalView", PartnerConfig.ResPath .. "partner_crystal")

function PartnerCrystalView:PartnerCrystalView()
	self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner
end

function PartnerCrystalView:LoadCallback()
	self.Board:SetData(self:GetType(), 1 == self.data.cur_sel_crystal.role and self.language.CrystalReplace or self.language.MyCrystal, Vector2.New(810, 673))
end

function PartnerCrystalView:CloseCallback()
	self.data.crystal_open_form = PartnerData.CryStalOpenFormEmun.Normal
end

PartnerCrystalViewF = PartnerCrystalViewF or DeclareMono("PartnerCrystalViewF", UIWFlushPanel)

function PartnerCrystalViewF:PartnerCrystalViewF()
	self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner.Crystal

	self.data_cares = {
		{data = self.data.cur_sel_crystal, func = self.FlushShowList, keys = {"flush"}},
		{data = self.data.cur_sel_crystal, func = self.FlushItems},
		{data = BagData.Instance.item_grid, func = self.FlushItems, init_call = false},
	}
	if 1 == self.data.cur_sel_crystal.role then
		if self.data.crystal_open_form == PartnerData.CryStalOpenFormEmun.Mirage then
			self.data.cur_sel_crystal.partner = -1
			self.data.cur_sel_crystal.role = 0
		else
			self.data.cur_sel_crystal.partner = 0
		end
	end
end

function PartnerCrystalViewF:Awake()
	UIWFlushPanel.Awake(self)
	self.TypeList:SetDataList(self.data.partner_crystal_list)
	self.BtnEquip.text = 1 == self.data.cur_sel_crystal.role and self.language.equip_quick_role or self.language.equip_quick_partner
end

function PartnerCrystalViewF:FlushShowList()
	local data, first_partner = self.data:GetPartnerAllInfoLockSort()
	if self.data.crystal_open_form == PartnerData.CryStalOpenFormEmun.Mirage then
		data, first_partner = FightMirageTerritoryData.Instance:GetAllPartnerList()
	end
	if 0 == self.data.cur_sel_crystal.role then
		local jump_pos = self.data.cur_sel_crystal.partner < 0 and first_partner or self.data.cur_sel_crystal.partner
		self.ShowList:SetData(data)
		self.ShowList:ClickItemData(self.data:GetPartnerInfoById(jump_pos))
	else
		table.insert(data, 1, {is_role = true})
		local jump_pos = self.data.cur_sel_crystal.partner < 0 and first_partner or self.data.cur_sel_crystal.partner
		self.ShowList:SetData(data)
		self.ShowList:ClickItemData(0 == self.data.cur_sel_crystal.partner and data[1] or self.data:GetPartnerInfoById(jump_pos))
	end
end

function PartnerCrystalViewF:FlushItems()
	if -1 == self.data.cur_sel_crystal.partner then return end
	local crystal = 0
	local partner_equip, role_equip = false, false
	local used_times = 0
	if 0 == self.data.cur_sel_crystal.partner then
		crystal = BagData.Instance:ViewData().element_crystal_id
		used_times = BagData.Instance:ViewData().used_times
		role_equip = true
	else
		local data = self.data:GetPartnerInfoById(self.data.cur_sel_crystal.partner)
		if nil == data then return end
		crystal = data.vo.crystal
		used_times = data.vo.used_times
		partner_equip = true
	end
	local item_id = self.data.cur_sel_crystal.item_id
	local list = BagData.Instance:GetCrystalGrid(item_id, true)
	if crystal > 0 and (0 == item_id or crystal == item_id) then
        local item = Item.Create({item_id = crystal})
        item.partner_equip = partner_equip
        item.role_equip = role_equip
		item.param = {}
        item.param.used_times = used_times
        table.insert(list, 1, item)
	end
	local crytal_get = PartnerConfig.CrystalGet[item_id] or PartnerConfig.CrystalGet[0]
	self.BtnComposePos:SetObjActive(crytal_get[1])
	self.BtnBuyPos:SetObjActive(crytal_get[2])
	self.BtnComposePos.localPosition = Vector2.New(crytal_get[3], 0)
	self.BtnBuyPos.localPosition = Vector2.New(crytal_get[4], 0)
	self.is_empty = false
	self.ItemList:SetData(list)
	-- self.EmptyObj:SetActive(self.is_empty)
	self.BtnGreen.text = self.is_empty and self.language.compose or self.language.remove_all
	self.BtnEquipInter.Interactable = not self.is_empty
end

function PartnerCrystalViewF:OnClickEquip()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.EquipEquipment)
	local list = BagData.Instance:GetCrystalGrid(self.data.cur_sel_crystal.item_id)
	local fight_order_list = self.data:GetPartnerFightOrderList():Val()
	local num = math.min(#list, #fight_order_list)
	local crystals = {}
	for i = 1, num do
		crystals[i] = list[i].index
	end
	PartnerCtrl.Instance:SendPartnerElementCrystalOperUseQuick(crystals)
end

function PartnerCrystalViewF:OnClickGreen()
	if self.is_empty then
		ViewMgr:OpenViewByKey(Mod.Workshop.NeoCompose)
    	-- ViewMgr:CloseView(PartnerCrystalView)
	else
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.BagTidy)
		PartnerCtrl.Instance:SendPartnerElementCrystalOperRemoveAll()
	end
end

function PartnerCrystalViewF:OnClickCompose()
	ViewMgr:OpenViewByKey(Mod.Workshop.NeoCompose)
    -- ViewMgr:CloseView(PartnerCrystalView)
end

function PartnerCrystalViewF:OnClickBuy()
	if 0 == self.data.cur_sel_crystal.item_id then
		ViewMgr:OpenViewByKey(Mod.Mall.Shop)
	else
		local buy_data = CommerceData.Instance:GetMourinhoCfgById(self.data.cur_sel_crystal.item_id)
		if buy_data then
			buy_data.content_func = function(data)
				ShopCtrl.Instance:SendGoldShopBuyReq(data.index,data.make_num)
				ViewMgr:CloseView(BatchBuyView)
			end
			ViewMgr:OpenView(BatchBuyView, buy_data)
		end
	end
end

PartnerCrystalViewAIP = DeclareMono("PartnerCrystalViewAIP", UIWidgetBaseItem)

function PartnerCrystalViewAIP:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Name.text = data.name
end

PartnerCrystalViewAIC = DeclareMono("PartnerCrystalViewAIC", UIWidgetBaseItem)

function PartnerCrystalViewAIC:PartnerCrystalViewAIC()
    self.partner_data = PartnerCtrl.Instance:Data()
end

function PartnerCrystalViewAIC:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Name.text = data.name
end

function PartnerCrystalViewAIC:Click()
    if self.Toggle then
		self.Toggle:Trigger()
	end
end

function PartnerCrystalViewAIC:OnClickItem()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    self.partner_data.cur_sel_crystal.item_id = self.data.item_id
end

PartnerCrystalViewItem = PartnerCrystalViewItem or DeclareMono("PartnerCrystalViewItem", UIWidgetBaseItem)

function PartnerCrystalViewItem:PartnerCrystalViewItem()
	self.partner_data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner.Crystal
end

function PartnerCrystalViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local is_equip = data.partner_equip or data.role_equip
    self.Cell:SetData(data)
    UH.SetText(self.Name, data:Name())
    self.EleAttr:SetData(data:Element())
    self.Equip:SetActive(is_equip)
    self.Label:SetActive(data:UsedTime() == 0)
	UH.SetText(self.UseNum, data:UsedTime() ~= 0 and string.format(Language.EleCrystal.UseNum, data:UsedTime()) or "")
	local btn_equip = self.language.btn_equip
	self.BtnEquip.text = is_equip and btn_equip.remove or btn_equip.equip
end

function PartnerCrystalViewItem:OnClickEquip()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.EquipEquipment)
	if self.data.partner_equip then
		PartnerCtrl.Instance:SendPartnerElementCrystalOperRemove(self.partner_data.cur_sel_crystal.partner)
	elseif self.data.role_equip then
		BagCtrl.Instance:SendEquipmentOperaReq({
			oper_type = EQUIP_OPERA_TYPE.OPERA_TYPE_TAKEOFF_CRYSTAL
		})
	elseif 0 == self.partner_data.cur_sel_crystal.partner then
		local param_t = {
			column = self.data.column_type,
			index = self.data.index
		}
		BagCtrl.Instance:SendUseItem(param_t)
	else
		PartnerCtrl.Instance:SendPartnerElementCrystalOperUse(self.partner_data.cur_sel_crystal.partner, self.data.index)
	end
end

PartnerCrystalViewShowItem = DeclareMono("PartnerCrystalViewShowItem", UIWidgetBaseItem)

function PartnerCrystalViewShowItem:PartnerCrystalViewShowItem()
	self.partner_data = PartnerCtrl.Instance:Data()
end

function PartnerCrystalViewShowItem:OnDestroy()
    self:uncareData()
end

function PartnerCrystalViewShowItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self:uncareData()
	self.care_handles = {}
	
	if data.is_role then
		self:SetDataRole(data)
	else
		self:SetDataPartner(data)
	end
end

function PartnerCrystalViewShowItem:SetDataRole(data)
	-- self:careData(self.partner_data.quality_flush, BindTool.Bind(self.PartnerQuality, self))
	self:careData(RoleData.Instance:AllBattleAttri(), BindTool.Bind(self.FlushCapRole, self))
	self:careData(BagData.Instance:ViewData(), BindTool.Bind(self.FlushCrystalRole, self), "element_crystal_id")

    local prof_sp = RoleData.GetProfSp(RoleData.Instance:GetRoleProfId())
    self.Name.text = string.format("<color=#%s>%s</color>", COLORSTR.Yellow13 , RoleData.Instance:GetRoleName())
    self.Prof.SpriteName = prof_sp
    UH.SetAvatar(self.IconSp, RoleData.Instance:GetRoleAvatarType(),RoleData.Instance:GetRoleAvatarId(),RoleData.Instance:GetRoleAvatarQuality())

    self.Level.text = string.format("<color=#%s>Lv.%s</color>", COLORSTR.Yellow16, RoleData.Instance:GetRoleLevel())

    self.Lock:SetActive(false)
    self.Normal:SetActive(true)
    -- self.Cap1.text = vo.cap
    -- self.Cap2.SpriteName = data.cap_sp

    local fight_state = PartnerConfig.FightState
    -- self.BtnFight:SetActive(vo.fight_state == fight_state.rest)
    -- self.BtnRest:SetActive(vo.fight_state == fight_state.fight)
    -- self.FightSp:SetObjActive(vo.is_unlocked == lock_state.unlock and (vo.fight_state == fight_state.fight or 1 == vo.is_arena_match))
    -- self.FightSp.SpriteName = 1 == vo.is_arena_match and PartnerConfig.FightSp.Lei or PartnerConfig.FightSp.Zhan

    self.FightSp:SetObjActive(false)
    -- self.FightSp.SpriteName = 1 == vo.is_arena_match and PartnerConfig.FightSp.Lei or PartnerConfig.FightSp.Zhan
    self.LeiObj:SetActive(false)

    self.ItemInter.Interactable = (true)

	PartnerInfo.SetQuality(self.Quality, nil)
end

function PartnerCrystalViewShowItem:SetDataPartner(data)
    self:careData(self.partner_data.quality_flush, BindTool.Bind(self.PartnerQuality, self))
    self:careData(data.flush_keys, BindTool.Bind(self.FlushCapPartner, self), "flush_cap")

    local info = data.info
    local vo = data.vo
    local prof_sp = RoleData.GetProfSp(info.job)
    local lock_state = PartnerConfig.LockState

    self.Name.text = string.format("<color=#%s>%s</color>", vo.is_unlocked == lock_state.unlock and COLORSTR.Yellow13 or COLORSTR.Gray4, info.name)
    self.Prof.SpriteName = prof_sp
    UH.SetIcon(self.IconSp, PartnerData.IconId(data), ICON_TYPE.ITEM)
	AvatarEffect.SetQuaAnimEffect(self.IconSp.gameObject,data.vo.soar_count)
    local level = RoleData.Instance:GetBaseData().level
    self.Level.text = string.format("<color=#%s>%s</color>", vo.is_unlocked == lock_state.unlock and COLORSTR.Yellow16 or COLORSTR.Gray4, "Lv.".. level)

    self.Lock:SetActive(vo.is_unlocked == lock_state.lock)
    self.Normal:SetActive(vo.is_unlocked == lock_state.unlock)
    -- self.Cap1.text = vo.cap
    -- self.Cap2.SpriteName = data.cap_sp

    local fight_state = PartnerConfig.FightState
    -- self.BtnFight:SetActive(vo.fight_state == fight_state.rest)
    -- self.BtnRest:SetActive(vo.fight_state == fight_state.fight)
    -- self.FightSp:SetObjActive(vo.is_unlocked == lock_state.unlock and (vo.fight_state == fight_state.fight or 1 == vo.is_arena_match))
    -- self.FightSp.SpriteName = 1 == vo.is_arena_match and PartnerConfig.FightSp.Lei or PartnerConfig.FightSp.Zhan
	local show_fight_sp = vo.is_unlocked == lock_state.unlock and vo.fight_state == fight_state.fight
	if self.partner_data.crystal_open_form == PartnerData.CryStalOpenFormEmun.Mirage then
		show_fight_sp = FightMirageTerritoryData.Instance:PartnerOnFight(data.id) ~= nil
	end
    self.FightSp:SetObjActive(show_fight_sp)
    -- self.FightSp.SpriteName = 1 == vo.is_arena_match and PartnerConfig.FightSp.Lei or PartnerConfig.FightSp.Zhan
    self.LeiObj:SetActive(vo.is_unlocked == lock_state.unlock and 1 == vo.is_arena_match)

    self.ItemInter.Interactable = (vo.is_unlocked == lock_state.unlock)

    local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(data.id)
	PartnerInfo.SetQuality(self.Quality, info_quality.quality)
    if vo.crystal > 0 then
        self.EleAttr:SetData(EleCrystal.GetElement(vo.crystal))
    end
    self.EleAttr:SetObjActive(vo.crystal > 0)
end

function PartnerCrystalViewShowItem:FlushCrystalRole()
	local role_score,score_level = RoleData.Instance:GetRoleScore()
    self.Cap1.text = role_score
	self.Cap2.SpriteName = "scoreLevel".. score_level
end

function PartnerCrystalViewShowItem:FlushCapRole()
	local crystal = BagData.Instance:ViewData().element_crystal_id
    if crystal > 0 then
        self.EleAttr:SetData(EleCrystal.GetElement(crystal))
    end
    self.EleAttr:SetObjActive(crystal > 0)
end

function PartnerCrystalViewShowItem:FlushCapPartner()
    self.Cap1.text = self.data.vo.cap
	self.Cap2.SpriteName = self.data.cap_sp
end

function PartnerCrystalViewShowItem:PartnerQuality()
    local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(self.data.id)
    PartnerInfo.SetQuality(self.Quality, info_quality.quality)
end

function PartnerCrystalViewShowItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
    end
end

function PartnerCrystalViewShowItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
	if selected then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        self.partner_data.cur_sel_crystal.partner = self.data.is_role and 0 or self.data.id
    end
end

function PartnerCrystalViewShowItem:OnClickFight()
    PartnerCtrl.Instance:SendPartnerFight(self.data.id)
end