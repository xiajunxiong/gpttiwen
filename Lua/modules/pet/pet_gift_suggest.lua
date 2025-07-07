PetGiftSuggest = PetGiftSuggest or DeclareView("PetGiftSuggest", "pet/pet_gift_suggest")
VIEW_DECLARE_MASK(PetGiftSuggest, ViewMask.BgBlock)
-- BtnForget
function PetGiftSuggest:PetGiftSuggest()
    self.pet_data = PetData.Instance
end

function PetGiftSuggest:LoadCallback(param)
    self.Board:SetData(PetGiftSuggest,Language.Pet.GiftSuggest,Vector2.New(645, 539))
    local data_list = self.pet_data:GetSuggestGroup(self.pet_data:GetCurSelect():NormalId())
    self.SuggestGroupList:SetData(data_list)
end

------------------------------
PetGiftSuggestGroupItem = PetGiftSuggestGroupItem or DeclareMono("PetGiftSuggestGroupItem", UIWidgetBaseItem)
function PetGiftSuggestGroupItem:PetGiftSuggestGroupItem()
end

function PetGiftSuggestGroupItem:SetData(data)
    UH.SetText(self.WayNameText, data.way_name)
    UH.SetText(self.WayDescText, data.way_describe)
    if not StringIsEmpty(data.pet_appraise) then
        UH.SetText(self.PetDescText, data.pet_appraise)
        self.PetDescObj:SetActive(true)
    else
        self.PetDescObj:SetActive(false)
    end
    self.GiftList:SetData(data.gift_id_list)
    self.GiftListRect.sizeDelta = Vector2.New(610, #data.gift_id_list <= 6 and 128 or 236)
    self.data = data
end

function PetGiftSuggestGroupItem:OnClickGift(mono)
    PetData.Instance.hit_book_type.suggest_skill_id = mono:GetData().skill_id
    ViewMgr:OpenView(PetSkillTipPro)
end