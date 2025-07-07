
HealMenuView = HealMenuView or DeclareMono("HealMenuView", UIWFlushPanel)

function HealMenuView:HealMenuView()
	self.data = TeamCtrl.Instance:Data()
	self.bag_data = BagData.Instance
	self.fight_data = FightData.Instance
	self.language = Language.Team.HealMenu

	self.data_cares = {
		{data = self.data:GetTeamInfo(), func = self.FlushMembers, init_call = false},
		{data = self.bag_data:GetColumnGrids(ItemColumnType.Supply), init_call = false, func = self.FlushHealItems},
		{data = self.data.heal_menu, func = self.FlushHealItems, keys = {"type"}},
		{data = self.data.heal_menu, func = self.FlushHealRightAnim, keys = {"type"}},
		{data = self.data.heal_menu, func = self.FlushHealObject, keys = {"object"}},
		{data = self.data.heal_menu, func = self.FlushMembers, keys = {"object"}},
		{data = self.data.heal_menu, func = self.FlushDetail, keys = {"member"}},
		{data = self.data.heal_menu, func = self.FlushLimits, keys = {"type"}},
		{data = self.fight_data.crystal_use_data, func = self.FlushLimits, init_call = false},
    }
end

function HealMenuView:Awake()
	UIWFlushPanel.Awake(self)
	self.CostGold.text = string.format(self.language.cost_desc, self.data:HealCostGold())
end

function HealMenuView:FlushMembers()
	local member_list = self.data:GetMemberList()
	for i = 1, TeamConfig.TEAM_MEMBER_MAX do
		local is_show = i <= #member_list and (0 == self.data.heal_menu.object or (member_list[i].info.pet_id and member_list[i].info.pet_id > 0))
		self.data.heal_menu.member = (i == self.data.heal_menu.member and not is_show) and 0 or self.data.heal_menu.member
		self.MemberItems[i].isOn = i == self.data.heal_menu.member
		self.MemberItems[i]:SetObjActive(is_show)
	end
	self.BtnTransPos.localPosition = Vector2.New(0, -29 - #member_list * 59)
end

function HealMenuView:FlushDetail(t)
	if t and t.member > 0 then
		self.MemberItems[t.member].isOn = false
	end
	self.DetailObj:SetActive(self.data.heal_menu.member > 0)
end

function HealMenuView:FlushHealItems()
	local heal_items = self.data:GetHealItems()
	self.HealItemList:SetData(heal_items)
	self.EmptyObj:SetActive(0 == #heal_items)
end

function HealMenuView:FlushHealRightAnim()
	self.HealRightAnim:SetTrigger(APH("Show"))
	if 0 == self.data.heal_menu.type then
		self.ToggleGroupHeal:SetAllTogglesOff()
	end
end

function HealMenuView:FlushHealObject()
	self.HealObjectSp.SpriteName = TeamConfig.HealMenuObjSp[self.data.heal_menu.object]
end

function HealMenuView:FlushLimits()
	local mode = SceneData.Instance:GetMode()
	if SceneModeType.FightCrystal == mode or SceneModeType.RushDungeon == mode then
		local remain, limit = ItemInfoData.GetMedicinalTimes(self.data:HealType2ItemId())
		self.RemainNum.text = string.format(self.language.remain_desc, remain, limit or 0)
	else
		self.RemainNum.text = ""
	end
end

function HealMenuView:OnClickTrans()
	self.data.heal_menu.member = 0
	self.data.heal_menu.object = (self.data.heal_menu.object + 1) % 2
end

function HealMenuView:OnClickClose()
	self.data:ResetHealMenu()
end

function HealMenuView:OnClickHeal()
	PublicPopupCtrl.Instance:DialogTips{
		content = string.format(Language.Team.HealMenu.gold_add_tips, self.data:HealCostGold(), Language.Team.HealMenu.gold_heal_types[self.data.heal_menu.type]),
		confirm = {
			func = function()
				if not MallCtrl.IsNotCurrency(CurrencyType.Gold,self.data:HealCostGold()) then
					return
				end
				local heal_menu = self.data.heal_menu
				local member_list = self.data.team_info.member_list
				local member = member_list[heal_menu.member]
				if nil == member then return end
				local info = member.info
				local sub_partner = info.sub_partner
				local is_self = member.is_self
				local is_partner = 1 == info.is_partner
				local is_pet = 1 == heal_menu.object
				local param1 = 0
				local param2 = 0
				local param3 = 0
				local param4 = heal_menu.type
				if is_self then
					if is_pet then
						param1 = 1
						param2 = PetData.Instance:GetFightIndex()
					else
						param1 = 0
					end
				else
					if is_pet then
						param1 = 3
						param2 = info.member_index
						param3 = 1
					else
						if self.data:IsLeader() or not self.data:InTeam() then
							if is_partner then
								param1 = 2
								param2 = info.partner_id
							else
								param1 = 3
								param2 = info.member_index
							end
						else
							if is_partner then
								param1 = 4
								param2 = info.partner_id
							else
								param1 = 3
								param2 = info.member_index
							end
						end
					end
				end
				if 0 == info.is_follow and sub_partner.id > 0 then
					if self.data:IsLeader() or not self.data:InTeam() then
						param1 = 2
						param2 = sub_partner.id
						param3 = 0
					else
						param1 = 4
						param2 = sub_partner.id
						param3 = 0
					end
				end
				TeamCtrl.Instance:SendUseGoldRecovery(param1, param2, param3, param4)
				ViewMgr:CloseView(DialogTipsView)
			end
		}
	}
end

HealMenuViewHealItem = DeclareMono("HealMenuViewHealItem", UIWidgetBaseItem)

function HealMenuViewHealItem:HealMenuViewHealItem()
	self.team_data = TeamCtrl.Instance:Data()
end

function HealMenuViewHealItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	UH.SetIcon(self.IconSp, data:IconId())
	self.ItemName.text = data:Name()
	self.ItemNum.text = data:Num()
	if RoleData.Instance:GetRoleLevel() < data:LimitLv() then
		self.Inter.Interactable = false
		UH.SetText(self.LimitText, string.format(Language.Common.Levels, self.data:LimitLv()))
		self.LimitObj:SetActive(true)
	else
		self.Inter.Interactable = true
		self.LimitObj:SetActive(false)
	end
end

function HealMenuViewHealItem:OnClickItem()
	if RoleData.Instance:GetRoleLevel() < self.data:LimitLv() then
		PublicPopupCtrl.Instance:Center(ErrorText[172])
		return
	end
	if SceneData.Instance:GetMode() == SceneModeType.FightCrystal and ItemInfoData.GetMedicinalTimes(self.data.item_id) == 0 then
		PublicPopupCtrl.Instance:Center(Language.ItemInfo.CrystalUseTimesTip)
		return
	end
	local heal_menu = self.team_data.heal_menu
	local member_list = self.team_data.team_info.member_list
	local member = member_list[heal_menu.member]
	if nil == member then return end
	local info = member.info
	local sub_partner = info.sub_partner
	local is_self = member.is_self
	local is_partner = 1 == info.is_partner
	local is_pet = 1 == heal_menu.object
	local param1 = 0
	local param2 = 0
	local param3 = 0
	if is_self then
		if is_pet then
			param1 = 1
			param2 = PetData.Instance:GetFightIndex()
		else
			param1 = 0
		end
	else
		if is_pet then
			param1 = 3
			param2 = info.member_index
			param3 = 1
		else
			if self.team_data:IsLeader() or not self.team_data:InTeam() then
				if is_partner then
					param1 = 2
					param2 = info.partner_id
				else
					param1 = 3
					param2 = info.member_index
				end
			else
				if is_partner then
					param1 = 4
					param2 = info.partner_id
				else
					param1 = 3
					param2 = info.member_index
				end
			end
		end
	end
	if 0 == info.is_follow and sub_partner.id > 0 then
		if self.team_data:IsLeader() or not self.team_data:InTeam() then
			param1 = 2
			param2 = sub_partner.id
			param3 = 0
		else
			param1 = 4
			param2 = sub_partner.id
			param3 = 0
		end
	end
	local index = BagData.Instance:GetBagIndexById(ItemColumnType.Supply, self.data.item_id)
	if nil == index then return end
	BagCtrl.Instance:SendUseItem{
		column = ItemColumnType.Supply,
		index = index - 1,
		param1 = param1,
		param2 = param2,
		param3 = param3,
	}
end


HealMenuViewMemberItem = DeclareMono("HealMenuViewMemberItem", UIWidgetBaseItem)

function HealMenuViewMemberItem:HealMenuViewMemberItem()
	self.team_data = TeamCtrl.Instance:Data()
	self.care_handles = {}
	self:careData(self.team_data.heal_menu, BindTool.Bind(self.FlushIconSp, self), "type")
end

function HealMenuViewMemberItem:OnDestroy()
    self:uncareData()
end

function HealMenuViewMemberItem:FlushIconSp()
	self.IconSp.SpriteName = TeamConfig.HealMenuIconSp[self.team_data.heal_menu.type]
end

function HealMenuViewMemberItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function HealMenuViewMemberItem:OnClickItem(member)
	self.team_data.heal_menu.member = member
end
