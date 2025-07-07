PetStrBuff = PetStrBuff or DeclareView("PetStrBuff", "pet/pet_strengthen_buff")
----BgRect=====RectTransform
local bg_height_orignal = 300
local one_txt_height = 60
function PetStrBuff:PetStrBuff()
    self.lang = Language.Pet
    self.data = PetData.Instance
end

function PetStrBuff:OnFlush()
    local pet = self.data:GetCurSelect()
    if pet == nil then
        return
    end
    local list = self.TextList
    local level = pet:BuffLevel()
    for i = 1, list:Length() do
        if i == 5 then
            local bg_size = self.BgRect.sizeDelta
            if PetFeiShengData.ModUnlocked(PetFeiShengData.Lv.Two) then
                list[i]:SetObjActive(true)
                bg_size.y = bg_height_orignal + one_txt_height
            else
                list[i]:SetObjActive(false)
                bg_size.y = bg_height_orignal
            end
            self.BgRect.sizeDelta = bg_size
        end
        list[i].text = self.lang.StrBuffList[i]
        self.ToggleList[i].isOn = (i == level)
    end
    self.Title.text = level == 0 and self.lang.BuffDetail[1] or string.format(self.lang.BuffDetail[2], level)
end

function PetStrBuff:OnClickMask()
    ViewMgr:CloseView(PetStrBuff)
end