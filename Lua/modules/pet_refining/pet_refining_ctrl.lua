
FILE_REQ("modules/pet_refining/pet_refining_data")
VIEW_REQ("modules/pet_refining/pet_refining_view")
VIEW_REQ("modules/pet_refining/pet_refining_left")
VIEW_REQ("modules/pet_refining/pet_refining_right")
VIEW_REQ("modules/pet_refining/pet_refining_preview")
VIEW_REQ("modules/pet_refining/pet_refining_result")
VIEW_REQ("modules/pet_refining/pet_refining_dialog")

PetRefiningCtrl = PetRefiningCtrl or BaseClass(BaseCtrl)
function PetRefiningCtrl:__init()
    self.data = PetRefiningData.New()


    self:RegisterProtocol(SCPetLianYaoCommonInfo, "SCPetLianYaoCommonInfo")
    
end
function PetRefiningCtrl:__delete()
    
end
function PetRefiningCtrl:SCPetLianYaoCommonInfo(protocol)
    self.data:SetUnlockInfo(protocol.is_unlock_lian_yao_furance_flag)
end