FightPetIslandSpecialItemView = FightPetIslandSpecialItemView or DeclareView("FightPetIslandSpecialItemView","fight/fight_pet_island_special_item")

function FightPetIslandSpecialItemView:FightPetIslandSpecialItemView()
	self.data_instance = FightData.Instance
end

function FightPetIslandSpecialItemView:LoadCallback()
	self.LeftList:SetData(Config.pet_island_auto.special_item)
	self.LeftList:ClickItem(1)
end

function FightPetIslandSpecialItemView:OnClickClose()
	ViewMgr:CloseView(FightPetIslandSpecialItemView)
	self.data_instance.pet_monopoly_smart.select_special_item_id = -1
	self.data_instance.pet_monopoly_smart:SetDirty("use_special_item")
end
--------------------------------------------------
FightPetIslandSpecialItemPanel = FightPetIslandSpecialItemPanel or DeclareMono("FightPetIslandSpecialItemPanel", UIWFlushPanel)
function FightPetIslandSpecialItemPanel:FightPetIslandSpecialItemPanel()
	self.data = FightData.Instance
	self.data_cares = {
		{data = self.data.pet_monopoly_smart, func = self.FlushRightPanel, keys = {"select_special_item_id","special_item_num"}},
	}
end

function FightPetIslandSpecialItemPanel:FlushRightPanel()
	local data = Config.pet_island_auto.special_item[self.data.pet_monopoly_smart.select_special_item_id + 1]
	if data then
		UH.SetText(self.ItemTitleName, data.name)
		UH.SetText(self.ItemDesc, data.desc)
		UH.SpriteName(self.ItemIconImg, "ItemIcon"..data.special_item_id)
		UH.SetText(self.ItemNum, string.format(Language.Fight.PetIslandSpecialItemNum, self.data:GetPetMonopolySpecialItemNum()))
	end
	self.BtnInteractor.Interactable = self.data:GetPetMonopolySpecialItemNum() > 0
end

function FightPetIslandSpecialItemPanel:OnClickUseItem()
	if self.data:GetPetMonopolySpecialItemNum() > 0 then
		if self.data.pet_monopoly_smart.select_special_item_id ~= PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_WISHCARD then
			if self.data.pet_monopoly_smart.select_special_item_id == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_DOUBLECARD then
				-- self.data.pet_monopoly_smart.double_left_times = self.data.pet_monopoly_smart.double_left_times + 1
				FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_SPECIAL_ITEM, PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_DOUBLECARD)
				return
			else
				self.data.pet_monopoly_smart.special_param = -1
				self.data.pet_monopoly_smart:SetDirty("use_special_item")
			end
		else
			ViewMgr:OpenView(FightPetIslandWishView)
		end
		ViewMgr:CloseView(FightPetIslandSpecialItemView)
	end
end

-------------------------------------------------
FightPetIslandLeftItem = FightPetIslandLeftItem or DeclareMono("FightPetIslandLeftItem", UIWidgetBaseItem)
function FightPetIslandLeftItem:FightPetIslandLeftItem()
	self.data_instance = FightData.Instance
	self.data_instance.pet_monopoly_smart:Uncare(self.care_handle)
end

function FightPetIslandLeftItem:Awake()
	self.care_handle = self.data_instance.pet_monopoly_smart:KeysCare({"special_item_num"},function ()
		if self.data then
			self:FlushNum()
		end
	end)
end

function FightPetIslandLeftItem:OnDestroy()
	self.data_instance.pet_monopoly_smart:Uncare(self.care_handle)
end

function FightPetIslandLeftItem:SetData(data)
	self.data = data
	UH.SpriteName(self.ItemIcon, "ItemIcon"..data.special_item_id)
	UH.SetText(self.NameText, data.name)
	self:FlushNum()
end

function FightPetIslandLeftItem:FlushNum()
	UH.SetText(self.NumText, "X"..self.data_instance:GetPetMonopolySpecialItemNum(self.data.special_item_id))
end

function FightPetIslandLeftItem:Click()
	if not self.Toggle.isOn then
		self.Toggle.isOn = true
	end
	self:OnClickItem()
end

function FightPetIslandLeftItem:OnClickItem()
	self.data_instance.pet_monopoly_smart.select_special_item_id = self.data.special_item_id
end
-------------------------------------------------------------
FightPetIslandWishView = FightPetIslandWishView or DeclareView("FightPetIslandWishView","fight/fight_pet_island_wish")
function FightPetIslandWishView:FightPetIslandWishView()
	self.data_instance = FightData.Instance
end

function FightPetIslandWishView:LoadCallback()
	local wish_item_list = self.data_instance:GetPetMonopolyCurLayerWishList()
	self.WishList:SetData(wish_item_list)
	self.WishList:ClickItem(1)
	UH.SetText(self.WishDesc,Language.Fight.PetIslandWishDesc)
end

function FightPetIslandWishView:OnClickUseItem()
	FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_SPECIAL_ITEM, PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_WISHCARD, self.data_instance.pet_monopoly_smart.special_param)
	self.data_instance.pet_monopoly_smart.select_special_item_id = -1
	self.data_instance.pet_monopoly_smart:SetDirty("use_special_item")
	ViewMgr:CloseView(FightPetIslandWishView)
end

function FightPetIslandWishView:OnClickClose()
	self.data_instance.pet_monopoly_smart.special_param = -1
	ViewMgr:CloseView(FightPetIslandWishView)
end
--------------------------------------------------
FightPetIslandWishItem = FightPetIslandWishItem or DeclareMono("FightPetIslandWishItem", UIWidgetBaseItem)
function FightPetIslandWishItem:FightPetIslandWishItem()
	self.data_instance = FightData.Instance
end

function FightPetIslandWishItem:SetData(data)
	self.data = data
	local item = Item.Create(data.reward_item)
	self.ItemCell:SetData(item)
	UH.SetText(self.NameText, item:Name())
end

function FightPetIslandWishItem:Click()
	if not self.Toggle.isOn then
		self.Toggle.isOn = true
	end
	self:OnClickItem()
end

function FightPetIslandWishItem:OnClickItem()
	self.data_instance.pet_monopoly_smart.special_param = self.data.seq
end