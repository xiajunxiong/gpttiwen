PetName = PetName or DeclareView("PetName", "pet/pet_name")
--Alert
--RootNode
--MoneyText
--Input
function PetName:PetName()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance
end
function PetName:LoadCallback()
    self.Alert:SetData(self:GetType(), Vector2.New(502, 253))
    -- self.RootNode:SetLocalPosition(Vector2.New(100,100,0))
end
function PetName:OnFlush()
    self.pet_index = self.pet_data:GetSelectPetIndex()
    local cost = self.pet_data:GetChangeNameCost()
    self.MoneyText.text = cost
end
function PetName:CloseCallback()
end

function PetName:OnClickToChange()
    local block, notice_str = LoginData.Instance:BlockChangeName()
    if block then
        PublicPopupCtrl.Instance:Center(notice_str)
        return
    end
    local function callback(str)
        PetCtrl.SendPetReName(self.pet_index, str)
        ViewMgr:CloseView(PetName)
    end
    
    if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(self.Input.text, 1, callback)
	else
        local name = self.Input.text
        local value, error_code = RoleCreateData.Instance:CheckRoleName(self.Input.text)
        if value then
            callback(name)
        else
            PublicPopupCtrl.Instance:Center(error_code)
        end
    end
end
