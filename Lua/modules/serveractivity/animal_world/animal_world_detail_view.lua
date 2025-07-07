--宠物详细（异兽临世专用）
AnimalWorldDetailView = AnimalWorldDetailView or DeclareView("AnimalWorldDetailView", "animal_world/animal_world_detail")

function AnimalWorldDetailView:AnimalWorldDetailView()

end

function AnimalWorldDetailView:LoadCallback(data)
    self.use_config = data.skill_list == nil        --根据技能列表是否为空判定为整套是否适用配置表中的数据
    if data ~= nil then
        self:SetData(data)
    end

    -- 这里只拿宠物的主动技能
    local total = {}
    local skill_list = self.pet:GetMainSkillList({})
    for _, v in ipairs(skill_list) do
        v.owner_pet = self.pet
        table.insert(total, v)
    end
    for i = 1, self.SkillList:Length() do
        if total[i] then
            self.SkillList[i]:SetData(total[i])
        else
            self.SkillList[i]:SetObjActive(false)
        end
    end
    
    local pet_cfg = self.pet:PetCfg()
    self.PetSign:SetType(pet_cfg.limit_type)

    self:SelectEffectShow()
end

function AnimalWorldDetailView:SelectEffectShow()
    local exchange_group = AnimalWorldData.Instance:ExchangeGroup()
    for i = 1, self.Effects:Length() do
        self.Effects[i]:SetActive(i == exchange_group)
    end
end

function AnimalWorldDetailView:SetData(data)
    self.pet = data
    
    UH.SetText(self.PetName, self.pet:Name())
    UH.SpriteName(self.ImgPet, PetData.PetLevel2[self.pet.config.quality])

    self:FlushModelView()
    self:FlushAttrBaseView()
end

function AnimalWorldDetailView:FlushAttrBaseView()
    self.AttrList:SetPet(self.pet, self.use_config)
    for k, v in pairs(PetData.Instance:RealAttr(self.pet.index, self.pet:TotalAttr()) or {}) do
        if tostring(k) == "maxhp" and not self.use_config then
            UH.SetText(self["Lbl" .. k],self.pet:HP() == 0 and v or self.pet:HP())
        elseif tostring(k) == "maxmp" and not self.use_config then
            UH.SetText(self["Lbl" .. k],self.pet:MP() == 0 and v or self.pet:MP())
        else
            UH.SetText(self["Lbl" .. k], v)
        end
    end
    self.click_type = {}
end

function AnimalWorldDetailView:FlushModelView()

    self.PetWeaponInfo:SetData({is_show = true, pet_weapon = self.pet:ExclusiveWeapon()})
end

function AnimalWorldDetailView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function AnimalWorldDetailView:OnClickGift(cell)
    local data = cell:GetData()
    if data.skill_id and data.skill_id ~= 0 or (data.gift_id and data.gift_id ~= 0) then
        PetData.Instance:SetClickSkill(data)
        ViewMgr:OpenView(PetSkillTip)
    end
end

function AnimalWorldDetailView:OnCloseClick()
    ViewMgr:CloseView(AnimalWorldDetailView)
end