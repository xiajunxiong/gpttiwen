
FormationPetView = FormationPetView or DeclareView("FormationPetView", FormationConfig.ResPath .. "formation_pet")

function FormationPetView:FormationPetView()
end

function FormationPetView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(), Language.Formation.PetChange, Vector2.New(560, 516))
end

FormationPetViewF = FormationPetViewF or DeclareMono("FormationPetViewF", UIWFlushPanel)

function FormationPetViewF:FormationPetViewF()
	self.data = FormationCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data.cur_formation_info, func = self.FlushFormationList},
		{data = PetData.Instance.pet_other_info, func = self.FlushFormationList, init_call = false},
	}
end

function FormationPetViewF:FlushFormationList()
	local sel_seq = self.data.cur_formation_info.sel_seq
	local sel_formation = self.data.formation_list[sel_seq]
	local is_succ, pet_list = sel_formation:GetPetList()
	if is_succ then
		self.PetList:SetData(pet_list)
	end
end

FormationPetViewItem = DeclareMono("FormationPetViewItem", UIWidgetBaseItem)

function FormationPetViewItem:FormationPetViewItem()
	self.formation_data = FormationCtrl.Instance:Data()
	self.pet_data = PetData.Instance
end

function FormationPetViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local pet = data.item
    self.Cell:SetData(pet)
	self.Name.text = pet:Name()
	self.Level.text = "Lv." .. pet:Level()
	UH.SpriteName(self.LevelSprite, PetData.PetLevel[pet.config.quality])
	PetData.SetStrLevel(nil, self.StrLevel, pet:StrLevel())
	local state, is_follow = PetData.Instance:GetPetState(data.pet_index)
	UH.SpriteName(self.PetState, PetData.State[state])
	for i = 1, 3 do
		self.SelObjs[i]:SetActive(state == i)
		self.BtnObjs[i]:SetActive(state ~= i)
	end
end

function FormationPetViewItem:OnClickChange()
	self.formation_data:PetChange(self.data)
	ViewMgr:CloseView(FormationPetView)
end

function FormationPetViewItem:OnClickState(state)
    local pet = self.data.item
	local cur_state = self.pet_data:GetPetState(pet.index)
	if PetState.Fight == state then
		self.formation_data:PetChange(self.data)
		return
	elseif PetState.Fight == cur_state then
		self.formation_data:PPRemove(self.data, state)
		return
	end
	if cur_state == state then
        return
    end
    if state ~= PetState.Relax and cur_state == PetState.Train then
        PublicPopupCtrl.Instance:Center(ErrorText[45])
        return
    end
    local p2 = state
    if state == PetState.StandBy then
        local flag = self.pet_data:GetStandbyCanUse()
        if not flag and cur_state ~= PET_STATE_OP.FIGHT then
            PublicPopupCtrl.Instance:Center(Format(Language.Pet.PetOnFightMax,PetData.Instance:GetMaxPetOn()))
            return
        end
        p2 = PET_STATE_OP.WAIT1
    end
    if state == PetState.Relax then
        p2 = PET_STATE_OP.RELAX
	end
    PetCtrl.SendPetOp(PET_OP_TYPE.SetPetState, pet.index, p2)
    -- if p2 == PET_STATE_OP.RELAX and pet.lock == 1 then
    --     PetCtrl.SendPetOp(PET_OP_TYPE.Lock, pet.index)
    -- end
    if SceneCtrl.Instance:CurScene():GetClassType() == BattleScene then
        PublicPopupCtrl.Instance:Center(ErrorText[110])
    end
end