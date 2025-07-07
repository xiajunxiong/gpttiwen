
ArenaMatchAddView = ArenaMatchAddView or DeclareView("ArenaMatchAddView", ArenaMatchConfig.ResPath .. "arena_match_add")
VIEW_DECLARE_MASK(ArenaMatchAddView,ViewMask.Block)

function ArenaMatchAddView:ArenaMatchAddView()
	self.data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch.Add
end

function ArenaMatchAddView:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.title, Vector2.New(853, 544))
end

function ArenaMatchAddView:OnFlush()
	local pet_list, partner_list, show_pet, show_partner = self.data:GetFightList()
	self.PetList:SetData(pet_list)
	self.PartnerList:SetData(partner_list)

	local tips = self.language.tips
	self.TipsObj:SetActive(not show_pet or not show_partner)
	if not show_pet then self.Tips.text = tips[1] end
	if not show_partner then self.Tips.text = tips[2] end

	self.PetTips:SetObjActive(0 == #pet_list)
	self.PartnerTips:SetObjActive(0 == #partner_list)
	self.PetTips.text = self.language.pet_tips[show_pet and 1 or 2]
	self.PartnerTips.text = self.language.partner_tips[show_partner and 1 or 2]
end

ArenaMatchAddViewItem = DeclareMono("ArenaMatchAddViewItem", ArenaMatchAddViewItem)

function ArenaMatchAddViewItem:ArenaMatchAddViewItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
end

ArenaMatchAddViewPetItem = DeclareMono("ArenaMatchAddViewPetItem", ArenaMatchAddViewItem)

function ArenaMatchAddViewPetItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local pet = data.pet
	local vo = data.vo
	self.ItemIcon:SetData(data)
	self.Name.text = pet:Name()
	self.Level.text = "Lv." .. pet:Level()
	self.ElemAttr:SetData(pet:EleAttr())

	local score, level = pet:GetScore()
	UH.SetText(self.TxtCap, score)
	UH.SpriteName(self.ImgCap, "scoreLevel" .. tostring(level))

	self.ZhanDai:SetObjActive(vo.is_zhan or vo.is_dai)
	self.LeiObj:SetActive(vo.is_lei)
	if vo.is_zhan then self.ZhanDai.SpriteName = PPStateSp.zhan end
	if vo.is_dai then self.ZhanDai.SpriteName = PPStateSp.dai end
end

function ArenaMatchAddViewPetItem:OnClickItem()
	self.arena_match_data.cur_sel_info:PetAdd(self.data, self.arena_match_data.cur_sel_info_to_pos)
	ViewMgr:CloseView(ArenaMatchAddView)
end

ArenaMatchAddViewPartnerItem = DeclareMono("ArenaMatchAddViewPartnerItem", ArenaMatchAddViewItem)

function ArenaMatchAddViewPartnerItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local partner = data.partner
	local vo = data.vo
	self.ItemIcon:SetData(data)
	self.Name.text = partner.name
	self.Level.text = "Lv." .. vo.level

	local info = PartnerData.Instance:GetPartnerInfoById(partner.partner_id)
	UH.SetText(self.TxtCap, info.vo.cap)
	UH.SpriteName(self.ImgCap, info.cap_sp)
	UH.SpriteName(self.StrLevel, PartnerInfo.StrLevelSp(vo.intensify_level))
	for i = 1, PartnerConfig.Attr_Ele_MAX do
        if #partner.element >= i then
            self.EleIcon[i]:SetObjActive(true)
            self.EleIcon[i].SpriteName = partner.element[i].type
            self.EleNum[i].text = partner.element[i].num
        else
            self.EleIcon[i]:SetObjActive(false)
        end
	end
	self.ZhanObj:SetActive(vo.is_zhan)
	self.LeiObj:SetActive(vo.is_lei)
end

function ArenaMatchAddViewPartnerItem:OnClickItem()
	self.arena_match_data.cur_sel_info:PartnerAdd(self.data, self.arena_match_data.cur_sel_info_to_pos)
	ViewMgr:CloseView(ArenaMatchAddView)
end