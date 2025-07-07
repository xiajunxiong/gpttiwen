
ArenaMatchConfig.LeftItemsPos = {
	[1] = Vector2.New(154, -21.5),
	[2] = Vector2.New(77, 33.5),
	[3] = Vector2.New(231, 12),
	[4] = Vector2.New(154, 67),
	[5] = Vector2.New(308, 45.5),
	[6] = Vector2.New(231, 100.5),
}

ArenaMatchWinnerView = ArenaMatchWinnerView or DeclareView("ArenaMatchWinnerView", ArenaMatchConfig.ResPath .. "arena_match_winner")

function ArenaMatchWinnerView:ArenaMatchWinnerView()
	self.data = ArenaMatchCtrl.Instance:Data()
	self.language = Language.ArenaMatch.Winner
end

function ArenaMatchWinnerView:LoadCallback()
	local per_info = self.data.cur_sel_info:PerInfo()
	self.Board:SetData(self:GetType(), string.format(self.language.title, per_info:Rank(), per_info:Name()))
end

-- function ArenaMatchWinnerView:CloseCallback()
-- 	if ViewMgr:IsOpen(ArenaMatchRankView) then return end
-- 	ViewMgr:OpenView(ArenaMatchInfoView)
-- end

ArenaMatchWinnerViewDetail = ArenaMatchWinnerViewDetail or DeclareMono("ArenaMatchWinnerViewDetail", UIWFlushPanel)

function ArenaMatchWinnerViewDetail:ArenaMatchWinnerViewDetail()
	self.data = ArenaMatchCtrl.Instance:Data()
	self.cur_sel_pos = 0
end

function ArenaMatchWinnerViewDetail:Awake()
	UIWFlushPanel.Awake(self)

	self:FlushDetail()
end

function ArenaMatchWinnerViewDetail:FlushDetail()
	local per_info = self.data.cur_sel_info:PerInfo()
	local pet_list = per_info:PetList()
	local partner_list = per_info:PartnerList()
	local is_five_arena = per_info:IsFiveArena()

	for i = 1, self.BigShows:Length() do
		self.BigShows[i]:SetActive(is_five_arena)
	end

	if not is_five_arena then
		for i = 1, self.LeftItemsPos:Length() do
			self.LeftItemsPos[i].localPosition = ArenaMatchConfig.LeftItemsPos[i]
		end
	end

	for i = 1, self.LeftItems:Length() do
		local pos = 1 == i % 2 and i + 1 or i - 1
		self.LeftItems[i]:SetData(pet_list[pos] or partner_list[pos])
	end

	for i = 1, self.RightItems:Length() do
		local pos = 1 == i % 2 and i + 1 or i - 1
		self.RightItems[i]:SetData(pet_list[pos] or partner_list[pos])
	end
	self:OnClickItem(1)
end

function ArenaMatchWinnerViewDetail:OnClickItem(pos)
	if self.cur_sel_pos > 0 and self.cur_sel_pos ~= pos then
		self.LeftItems[self.cur_sel_pos]:ClickItem(false)
		self.RightItems[self.cur_sel_pos]:ClickItem(false)
	end
	self.cur_sel_pos = pos
	self.LeftItems[self.cur_sel_pos]:ClickItem(true)
	self.RightItems[self.cur_sel_pos]:ClickItem(true)
end

ArenaMatchWinnerViewLeftItem = DeclareMono("ArenaMatchWinnerViewLeftItem", UIWidgetBaseItem)

function ArenaMatchWinnerViewLeftItem:ArenaMatchWinnerViewLeftItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
end

function ArenaMatchWinnerViewLeftItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local is_empty = nil == data
	self.InfoObj:SetActive(not is_empty)
	if is_empty then return end
	local is_pet = nil ~= data.pet
	local is_partner = nil ~= data.partner
	self.PetObj:SetActive(is_pet)
	self.PartnerObj:SetActive(is_partner)
end

function ArenaMatchWinnerViewLeftItem:ClickItem(value)
	self.SelObj:SetActive(value)
end

ArenaMatchWinnerViewRightItem = DeclareMono("ArenaMatchWinnerViewRightItem", UIWidgetBaseItem)

function ArenaMatchWinnerViewRightItem:ArenaMatchWinnerViewRightItem()
	self.arena_match_data = ArenaMatchCtrl.Instance:Data()
end

function ArenaMatchWinnerViewRightItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local is_empty = nil == data
	self.InfoObj:SetActive(not is_empty)
	if is_empty then return end
	local is_pet = nil ~= data.pet
	local is_partner = nil ~= data.partner
	self.IconPet:SetObjActive(is_pet)
	self.IconPartner:SetObjActive(is_partner)
	if is_pet then self:FlushPet(data) end
	if is_partner then self:FlushPartner(data) end
end

function ArenaMatchWinnerViewRightItem:FlushPet(data)
	local pet = data.pet
	local vo = data.vo
	self.IconPet:SetData(data)
	self.Name.text = vo.name or pet:Name()
	self.Level.text = "Lv." .. (vo.level or pet:Level())
end

function ArenaMatchWinnerViewRightItem:FlushPartner(data)
	local partner = data.partner
	local vo = data.vo
	self.IconPartner:SetData(data)
	self.Name.text = partner.name
	self.Level.text = "Lv." .. (vo.level or RoleData.Instance:GetRoleLevel())
end

function ArenaMatchWinnerViewRightItem:ClickItem(value)
	self.SelObj:SetActive(value)
end