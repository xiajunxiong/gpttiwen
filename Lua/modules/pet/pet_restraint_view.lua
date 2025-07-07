PetRestraintView = PetRestraintView or DeclareView("PetRestraintView", "pet/pet_restraint")

function PetRestraintView:PetRestraintView()
    self.Board:SetData(self:GetType(), Language.Pet.RestraintTitle,Vector2.New(635, 386))
end