MengChongJiaDaoObj = MengChongJiaDaoObj or DeclareMono("MengChongJiaDaoObj", UIWidgetBaseItem)
function MengChongJiaDaoObj:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
end

function MengChongJiaDaoObj:ChangeFace(index)
    for i = 1, 3 do
        self.FaceList[i]:SetActive(index == i)
    end
end

function MengChongJiaDaoObj:ShowIdle()
    self:ChangeFace(1)
    self.NameBoard:SetActive(true)
    self.PKState.gameObject:SetActive(false)
    UH.SetInter(self.ObjInter, true)
end

function MengChongJiaDaoObj:ShowWin()
    self:ChangeFace(2)
    self.NameBoard:SetActive(false)
    self.PKState.gameObject:SetActive(true)
    UH.SpriteName(self.PKState, "_Loc_Win")
    UH.SetInter(self.ObjInter, true)
end

function MengChongJiaDaoObj:ShowFail()
    self:ChangeFace(3)
    self.NameBoard:SetActive(false)
    self.PKState.gameObject:SetActive(true)
    UH.SpriteName(self.PKState, "_Loc_Fail")
    UH.SetInter(self.ObjInter, false)
end

function MengChongJiaDaoObj:ShowPing()
    self:ChangeFace(1)
    self.NameBoard:SetActive(false)
    self.PKState.gameObject:SetActive(false)
    UH.SetInter(self.ObjInter, true)
end

function MengChongJiaDaoObj:SetActive(active)
    self.gameObject:SetActive(active)
end

function MengChongJiaDaoObj:ShowArrow(is_up, is_show)
    self.StateImage.gameObject:SetActive(is_show)
    self.CombatText.gameObject:SetLocalPosition(Vector3.New(is_show and -12 or 0, 30, 0))
    if is_show then
        UH.SpriteName(self.StateImage, is_up and "ShangJianTou" or "XiaJianTou")
    end
end

-----自己宠物mono
MengChongJiaDaoRoleObj = MengChongJiaDaoRoleObj or DeclareMono("MengChongJiaDaoRoleObj", MengChongJiaDaoObj)
function MengChongJiaDaoRoleObj:SetData(data)
    MengChongJiaDaoObj.SetData(self, data)
    UH.SetText(self.NameText, Format(Language.MengChongJiaDao.NameStr, MengChongJiaDaoData.Instance:OtherConfig().name))

    local role_combat = MengChongJiaDaoData.Instance:GetTotalCombat()
    local is_up = role_combat > data.combat
    local color_str = ColorStr(role_combat, is_up and COLORSTR.Green4 or COLORSTR.Red8)
    UH.SetText(self.CombatText, Format(Language.MengChongJiaDao.CombatStr, color_str))
    self:ShowArrow(is_up, data.combat ~= role_combat)
end

-----敌方宠物mono
MengChongJiaDaoEnemyObj = MengChongJiaDaoEnemyObj or DeclareMono("MengChongJiaDaoEnemyObj", MengChongJiaDaoObj)
function MengChongJiaDaoEnemyObj:SetData(data)
    MengChongJiaDaoObj.SetData(self, data)
    UH.SetText(self.NameText, ColorStr(data.name, COLORSTR.Yellow2))
    UH.SpriteName(self.PetImage, "Monster" .. data.model)

    local role_combat = MengChongJiaDaoData.Instance:GetTotalCombat()
    local is_up = data.combat > role_combat
    local color_str = ColorStr(data.combat, is_up and COLORSTR.Green4 or COLORSTR.Red8)
    UH.SetText(self.CombatText, Format(Language.MengChongJiaDao.CombatStr, color_str))
    self:ShowArrow(is_up, data.combat ~= role_combat)
end
