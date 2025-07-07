FILE_REQ("modules/pet_bagde/pet_bagde_data")
VIEW_REQ("modules/pet_bagde/pet_bagde_item")
VIEW_REQ("modules/pet_bagde/pet_bagde_compound")
VIEW_REQ("modules/pet_bagde/pet_bagde_resolve")
VIEW_REQ("modules/pet_bagde/pet_bagde_select")

-- =================== 宠物徽章 ======================
PetBagdeCtrl = PetBagdeCtrl or BaseClass(BaseCtrl)

function PetBagdeCtrl:__init()
    if PetBagdeCtrl.Instance ~= nil then
		Debuger.LogError("[PetBagdeCtrl] attempt to create singleton twice!")
		return
	end
    PetBagdeCtrl.Instance = self
    self.data = PetBagdeData.New()

    self:RegisterProtocol(CSPetBadgeResolve)
end

function PetBagdeCtrl:__delete()
    PetBagdeCtrl.Instance = nil
    self.data = nil
end

-- 分解
function PetBagdeCtrl:SenResolve(item_info_list)
    local protocol = GetProtocol(CSPetBadgeResolve)
    protocol.item_info_list = item_info_list or {}
    SendProtocol(protocol)
end