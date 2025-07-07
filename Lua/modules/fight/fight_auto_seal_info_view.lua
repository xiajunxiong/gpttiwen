FightAutoSealInfoView = FightAutoSealInfoView or DeclareView("FightAutoSealInfoView","fight/fight_auto_seal_info_view")
VIEW_DECLARE_MASK(FightAutoSealInfoView,ViewMask.BgBlock)

function FightAutoSealInfoView:FightAutoSealInfoView()
	self.Board:SetData(self:GetType(),Language.Fight.Seal.InfoTitle, Vector2.New(378, 458))
	self.data = FightData.Instance
end

function FightAutoSealInfoView:LoadCallback(param_t)
	self.data:SetSealing(false)
	param_t = param_t or {}
	self.open_type = param_t.open_info_type or FightData.OPEN_AUTO_SEAL_INFO_TYPE.LOOK
	local info_data = self.data:GetAutoSealInfo()
	local card_List_data = {
		{item_id = 26078 , num = info_data.cur_seal_card_common or 0, max_num = info_data.max_seal_card_common or 0},
		{item_id = 26079 , num = info_data.cur_seal_card_silver or 0, max_num = info_data.max_seal_card_silver or 0},
		{item_id = 26080 , num = info_data.cur_seal_card_gold or 0, max_num = info_data.max_seal_card_gold or 0},
    }
    self.CardList:SetData(card_List_data)
    self.PetList:SetData(info_data.info_list or {})
    local btn_strings = Language.Fight.Seal.SealInfoBtn[self.open_type]
    UH.SetText(self.CancelBtnText,btn_strings[1])
    UH.SetText(self.OkBtnText,btn_strings[2])
    if self.open_type == FightData.OPEN_AUTO_SEAL_INFO_TYPE.ACCOMPLISH then
    	UH.SetText(self.Board.Title,Language.Fight.Seal.InfoTitle2)
    	FightCtrl.Instance:ClearAutoSealData()
    end
end

function FightAutoSealInfoView:OnLeftClick()
	if self.open_type == FightData.OPEN_AUTO_SEAL_INFO_TYPE.ACCOMPLISH then
		ViewMgr:OpenView(PetView)
	elseif self.open_type == FightData.OPEN_AUTO_SEAL_INFO_TYPE.INTERRUPT then
		FightCtrl.Instance:ClearAutoSealData()
	elseif self.open_type == FightData.OPEN_AUTO_SEAL_INFO_TYPE.LOOK then
		FightCtrl.Instance:ClearAutoSealData()
	end
	ViewMgr:CloseView(FightAutoSealInfoView)
end

function FightAutoSealInfoView:OnRightClick()
	if self.open_type == FightData.OPEN_AUTO_SEAL_INFO_TYPE.ACCOMPLISH then
		FightCtrl.Instance:ClearAutoSealData()
	elseif self.open_type == FightData.OPEN_AUTO_SEAL_INFO_TYPE.INTERRUPT then
		self.data.cur_sel_field.click_handle()
		if self.data:IsSetAutoSealPet() then
			self.data:SetSealing(true)
		end
	elseif self.open_type == FightData.OPEN_AUTO_SEAL_INFO_TYPE.LOOK then

	end
	ViewMgr:CloseView(FightAutoSealInfoView)
end

FightAutoSealInfoItem = FightAutoSealInfoItem or DeclareMono("FightAutoSealInfoItem",UIWidgetBaseItem)
function FightAutoSealInfoItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	local m_cfg = Cfg.MonsterById(data.monster_id)
	local icon_id = m_cfg.icon_id
	local pet_id = m_cfg.catch_pet_id
	UH.SetIcon(self.Icon,icon_id)
	UH.SetText(self.Name,Cfg.PetById(pet_id).name)
	--UH.SetText(self.Num,string.format("%s/%s",data.cur_catch_num,data.max_catch_num))
	UH.SetText(self.Num,string.format("x %s",data.cur_catch_num))
end

FightAutoSealCardInfoItem = FightAutoSealCardInfoItem or DeclareMono("FightAutoSealCardInfoItem",UIWidgetBaseItem)
function FightAutoSealCardInfoItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	self.Cell:SetData(Item.Create({item_id = data.item_id}))
	--UH.SetText(self.Num,string.format("%s/%s",data.num,data.max_num))
	UH.SetText(self.Num,string.format("%s",data.num))
end