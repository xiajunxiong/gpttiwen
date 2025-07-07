-- 炼妖的副宠变异
PetLianYaoMutantView = PetLianYaoMutantView or DeclareView("PetLianYaoMutantView", "pet/pet_lian_yao_mutant_view")
function PetLianYaoMutantView:PetLianYaoMutantView()
    self.Board:SetData(self:GetType(),Language.Pet.PetLianYaoMutantTitle,Vector2.New(660, 502))
    self.ht = BagData.Instance:Care(BindTool.Bind(self.FlushPanel, self))
end

function PetLianYaoMutantView:LoadCallback(param_t)
    self.pet = param_t.pet
    self:FlushPanel()
end

function PetLianYaoMutantView:CloseCallback()
    BagData.Instance:UnCare(self.ht)
end

function PetLianYaoMutantView:FlushPanel()
    local pet = self.pet
    local yao_pet = pet:YaoPet()
    local yao_pet_skill_list = yao_pet:SkillList()
    local mu_lv = yao_pet:MutTimes()
    local mutant_id = PetData.Instance:GetMutantId(yao_pet.config.prototype_id, mu_lv + 1)
    local mutant_pet = Pet.New({pet_id = mutant_id, str_level = yao_pet:StrLevel()})
    local mutant_skill_list = mutant_pet:SkillList()
    local cfg = yao_pet:PetCfg()
    local item_id = cfg.change_consume_item
    local need_num = cfg.change_consume_num
    local has_num = Item.GetNum(item_id)
    local num_str = DataHelper.ConsumeNum(has_num, need_num, true)
    local item = Item.Create({item_id = item_id, num = num_str})
    self.is_can = has_num >= need_num
    self.item_id = item_id
    
    self.Item:SetData(item)
    self.LSkillList:SetData(yao_pet_skill_list)
    self.RSkillList:SetData(mutant_skill_list)
end

function PetLianYaoMutantView:OnBtnClick()
    if self.is_can then
        PetCtrl.SendPetOp(PET_OP_TYPE.PetMutantLianYao, self.pet.index)
        ViewMgr:CloseView(PetLianYaoMutantView)
    else
        PublicPopupCtrl.Instance:Error(Language.Common.NotItem)
        MainOtherCtrl.Instance:GetWayViewById(self.item_id)
    end
end

function PetLianYaoMutantView:OnClickGift(cell)
    PetGiftItem.CommonClickFunc(cell)
end