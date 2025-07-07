-- 基础信息
PetWeaponItemBaseInfo = PetWeaponItemBaseInfo or DeclareMono("PetWeaponItemBaseInfo", UIWidgetBaseItem)

-- data ：PetWeapon
function PetWeaponItemBaseInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushInfo()
end

function PetWeaponItemBaseInfo:FlushInfo()
    local data = self.data
    --local points_str = ""
    -- if data.QuaShowPoint then
    --     points_str = data:QuaShowPoint()
    -- end
    -- self.QuaPoints.text = points_str
    local cur_weapon = PetWeaponData.Instance:GetCurCtrlWeapon()
    local bag_item = BagData.Instance:GetItemByIndex(ItemColumnType.PetWeapon,self.data.index or -1)
    self.LockImg:SetObjActive(bag_item ~= nil and bag_item == self.data and self.data ~= cur_weapon)
    if self.data.index ~= nil and self.data.index >= 0 then
        UH.SpriteName(self.LockImg, data:IsLock() and "SuoZhu" or "SuoDaKai")
    end
    UH.SetText(self.Level, string.format(Language.PetWeapon.ItemInfoLevel,data:Level()))
    local refine_level = data:RefineLevel()
    UH.SetText(self.RefineLevel, string.format(Language.PetWeapon.ItemInfoRefineLevel, DataHelper.GetDaXie(refine_level)))
end

function PetWeaponItemBaseInfo:OnLockClick()
    if PetWeaponData.Instance:IsStuff(self.data) then
        PublicPopupCtrl.Instance:Center(Language.PetWeapon.Error9)
        return
    end
    local pet_index = self.data:PetIndex()
    local p1 = pet_index and 0 or 1
    local p2 = pet_index and pet_index or self.data.index
    if self.p3 == nil then
        self.p3 = self.data:IsLock() and 0 or 1
    end
    PetCtrl.SendPetOp(PET_OP_TYPE.LockWeapon, p1, p2, self.p3, self.data:GridPos())
    UH.SpriteName(self.LockImg, self.p3 == 1 and "SuoZhu" or "SuoDaKai")
    self.p3 = self.p3 == 1 and 0 or 1
end

function PetWeaponItemBaseInfo:OnClickPressDown()
    ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end

-- 内容信息 -- self.item1是当前选择的(右边)，self.item2 被比较的(左边)
PetWeaponItemInfo = PetWeaponItemInfo or DeclareMono("PetWeaponItemInfo", ItemInfoViewItem)

function PetWeaponItemInfo:OnEnable()
    ItemInfoViewItem.OnEnable(self)
    UH.SetText(self.Title, Language.PetWeapon.ItemAttriTitle)
    self.item = self.info.item_list[self.ShowType]
    self:FlushPanel()
end

function PetWeaponItemInfo:FlushPanel()
    local attri_list = self.item:AttriListSort() or {}
    local item_2 = self.info.item_list[2]
    local has_attri = next(attri_list) ~= nil
    self.NotAttriTip:SetObjActive(not has_attri)
    self.AttriList:SetObjActive(has_attri)
    if not has_attri then
        UH.SetText(self.NotAttriTip, Language.PetWeapon.NotAttriTip)
    else
        -- 是右边的，则比较一下左边的
        if self.ShowType == 1 and item_2 then
            local new_attri_list = TableCopy(attri_list)
            local item_2_attri_list = item_2:AttriList()
            for i,v in pairs(new_attri_list) do
                if item_2_attri_list[v.attri_type] and v.attri_value ~= item_2_attri_list[v.attri_type].attri_value then
                    v.show_arrow = true
                    v.is_up = v.attri_value > item_2_attri_list[v.attri_type].attri_value
                end
            end
            self.AttriList:SetData(new_attri_list)
        else
            --LogError("属性：",attri_list)
            self.AttriList:SetData(attri_list)
        end
    end

    local weapon_type, pet_id = self.item:WeaponType()
    local effect_desc = self.item:RefineEffectDesc()
    local refine_attri_desc = self.item:RefineAttriDesc(2)
    effect_desc = effect_desc .. "\n" .. refine_attri_desc
    if weapon_type == PetWeaponType.Only then
        self.OnelyTtile:SetObjActive(true)
        local title = self.item:OnelyDesc()
        local pet_index = self.item:PetIndex()
        local mutant_id = PetData.Instance:GetMutantId(pet_id)
        local mutant_id_2 = PetData.Instance:GetMutantId(pet_id,2)
        local use_pet_id = self.item:PetId()
        if not self.item:IsLianYao() then
            if not self.item.is_book and use_pet_id ~= pet_id and use_pet_id ~= mutant_id and use_pet_id ~= mutant_id_2 then
                self.Flag:SetActive(true)
                title = ColorStr(title, COLORSTR.Gray3)
                effect_desc = ColorStr(effect_desc, COLORSTR.Gray3)
            else
                self.Flag:SetActive(false)
            end
        else
            local use_pet_id2 = self.item:YaoPetId()
            if not self.item.is_book and (use_pet_id ~= pet_id and use_pet_id ~= mutant_id and use_pet_id ~= mutant_id_2)
            and (use_pet_id2 ~= pet_id and use_pet_id2 ~= mutant_id and use_pet_id2 ~= mutant_id_2) then
                self.Flag:SetActive(true)
                title = ColorStr(title, COLORSTR.Gray3)
                effect_desc = ColorStr(effect_desc, COLORSTR.Gray3)
            else
                --假如装备了两个一样的也不生效。。。。。。。。。
                if self:SameWhoIsBetter() then
                    self.Flag:SetActive(true)
                    title = ColorStr(title, COLORSTR.Gray3)
                    effect_desc = ColorStr(effect_desc, COLORSTR.Gray3)
                else
                    self.Flag:SetActive(false)
                end
            end
        end
        UH.SetText(self.OnelyDesc, title)
    else
        self.OnelyTtile:SetObjActive(false)
        self.Flag:SetActive(false)
    end
    UH.SetText(self.EffectDesc, effect_desc)
end
function PetWeaponItemInfo:SameWhoIsBetter()
    local pet = self.item:GetPet()
    if not pet then
        local pet_index = self.item:PetIndex()
        pet = PetData.Instance:GetPetInfo(pet_index)
    end
    if not pet then
        return false
    end
    
    if self.item.weapon_idx == 1 then
        local weapon1 = pet:Weapon()
        if weapon1 and self.item.item_id == weapon1.item_id then
            return weapon1:RefineLevel() >= self.item:RefineLevel()
        end
    else
        local weapon2 = pet:Weapon2()
        if weapon2 and self.item.item_id == weapon2.item_id then
            return weapon2:RefineLevel() > self.item:RefineLevel()
        end
    end
    return false
end

PetWeaponItemInfoAttri = PetWeaponItemInfoAttri or DeclareMono("PetWeaponItemInfoAttri", UIWidgetBaseItem)
function PetWeaponItemInfoAttri:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.Arrow then
        self.Arrow:SetObjActive(data.show_arrow ~= nil)
    end
	if data.show_arrow then 
	    UH.SpriteName(self.Arrow, data.is_up and "up_s" or "down_s")		
	end
    UH.SetText(self.AttriName,Language.AttrItems.AttrNames[data.attri_type])
    if DataHelper.IsPercent(data.attri_type) then
        UH.SetText(self.AttriValue,string.format("+%s%%",data.attri_value/10))
    else
        UH.SetText(self.AttriValue,string.format("+%s",data.attri_value))
    end
end


-- 铸灵道具基本信息
SpiritStoneItemBaseInfo = SpiritStoneItemBaseInfo or DeclareMono("SpiritStoneItemBaseInfo", UIWidgetBaseItem)
function SpiritStoneItemBaseInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtLevel, Format(Language.Equip.EquipStoneLevel, data.param.level + 1))
    local m_type = EquipData.Instance:GetSpiritLevelData(data.item_id, data.param.level).spirit_stone_type
    local m_type_str = Language.Equip.StoneType[m_type]
    UH.SetText(self.TxtType, Format(Language.Equip.EquipStoneType, m_type_str))
end

------------------ 藏品收录的：IllPetWeaponItemCall -----------------
IllPetWeaponItemCall = IllPetWeaponItemCall or DeclareMono("IllPetWeaponItemCall", UIWidgetBaseItem)
function IllPetWeaponItemCall:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetIcon(self.Icon,data:IconId())
	UH.SpriteName(self.QuaIcon,COLOR_FRAME_SPRITE[Item.GetColor(data.item_id)])
end

function IllPetWeaponItemCall:MakeGrey(grey)
    self.Inter.Interactable = not grey
    self.Mark:SetActive(grey)
end