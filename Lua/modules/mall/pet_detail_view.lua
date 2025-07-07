--宠物详细
PetDetailView = PetDetailView or DeclareView("PetDetailView", "mall/PetDetailView")

function PetDetailView:PetDetailView()
    self.Board:SetData(self:GetType(), Language.Mall.PetDetai,Vector2.New(973, 561))
end

function PetDetailView:LoadCallback(data)
    if data ~= nil then
        self:SetData(data)
    end
    --有些界面展示不要部分按钮，隐藏掉。
    if data.view_btn_hide ~= nil then
        for i, v in pairs(data.view_btn_hide) do
            if v == "BtnDetail" then --详细属性
                self.TabBtnList[2]:SetActive(false)
            elseif v == "BtnStamp" then --神印
                self.TabBtnList[4]:SetActive(false)
            elseif v == "PetWeapon" then --魂器
                self.PetWeaponInfo.gameObject:SetActive(false)
                self.PetWeaponInfo2:SetObjActive(false)
            end
        end
    end

    if not TableIsEmpty(data.show_tabs) then
        for i,v in pairs(data.show_tabs) do
            self.TabBtnList[v]:SetActive(true)
        end
    end

    if not TableIsEmpty(data.hide_tabs) then
        for i,v in pairs(data.hide_tabs) do
            self.TabBtnList[v]:SetActive(false)
        end
    end

    if data.hide_weapon then
        self.PetWeaponInfo:SetObjActive(false)
        self.PetWeaponInfo2:SetObjActive(false)
    end
end

function PetDetailView:SetData(data)
    self.pet = data
    self.use_config = self.pet.skill_list == nil        --根据技能列表是否为空判定为整套是否适用配置表中的数据
    UH.SetText(self.PetName, self.pet:Name())
    UH.Color(self.PetName, self.pet:IsLianYao() and COLORS.Pink2 or COLORS.Yellow13)
	if self.YaoObj then
		self.YaoObj:SetActive(self.pet:IsLianYao())
	end
    -- self.PetName.color = QualityColor[Item.GetColor(self.pet:ID()) or 1]
    UH.SpriteName(self.ImgType, Format("Race%s", self.pet.config.race))
    UH.SetText(self.LblType, Language.Pet.RaceClass[self.pet.config.race])
    UH.SpriteName(self.ImgPet, PetData.PetLevel[self.pet.config.quality])
    UH.SpriteName(self.ImgValue, PetData.PetStrLevel[self.pet.str_level])

    local growth_value = self.use_config and self.pet:ConfigGrowth() or self.pet:TotalGrowth()
    UH.SetText(self.LblValue,Format(Language.Pet.PetValue, growth_value, self.pet:ConfigGrowth()))
    UH.SetText(self.LblLevel,Format(Language.Pet.LevelStr3,self.pet:Level()))
    self.Mutant:SetActive(self.pet.config.mutant_times >= 1)
    self:FlushModelView()
    self:FlushAttrBaseView()
    self:FlushPetStamp()
    self:FlushPetSkin()
end

function PetDetailView:FlushAttrBaseView()
    self.ElemAttr:SetData(self.pet:EleAttr())
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

function PetDetailView:FlushDetailView()
    local attr_list = PetData.Instance:GetPetAttrInfo(self.pet.index) or self.pet:TotalAttr()
    for i, v in ipairs(Correction) do
        if v == BattleAttriType.MagicAttack or v == BattleAttriType.MagicDefense then
            UH.SetText(self.LeftPart[i], attr_list[v] or 0)
        else
            UH.SetText(self.LeftPart[i], Percent(attr_list[v] or 0))
        end
    end
    for i, v in ipairs(Resistance) do
        UH.SetText(self.RightPart[i], Percent(attr_list[v] or 0))
    end
end

function PetDetailView:FlushModelView()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.Model:SetShowData({ui_obj = self.ui_obj,view = self})
    end
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(self.pet:ResId()),self.pet:FeiShengTimes())
    self.SignNameItem:SetType(self.pet:LimitType())

    local weapon_1 = self.pet:Weapon()
    if not TableIsEmpty(weapon_1) then
        weapon_1:SetPet(self.pet)
    end
    local weapon_2 = self.pet:Weapon2()
    if not TableIsEmpty(weapon_2) then
        weapon_2:SetPet(self.pet)
    end

    self.PetWeaponInfo:SetData({is_show = true, pet_weapon = weapon_1})
    self.PetWeaponInfo2:SetObjActive(self.pet:IsLianYao())
    self.PetWeaponInfo2:SetData({is_show = true, pet_weapon = weapon_2})
end

function PetDetailView:FlushPetStamp()
    self.cur_blueprint = self.pet:StampBlueprint()
    if not PetData.Instance:IsInBag(self.pet) then
        self.cur_blueprint.stamp_index_list = {0,0,0,0,0}
        self.cur_blueprint.pet_index = nil
    end
    self.PetStampInfo:SetData({blueprint = self.cur_blueprint})
end

function PetDetailView:FlushPetSkin()
    self.PetSkinPanel:SetPet(self.pet, self.ui_obj)
end

function PetDetailView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	Delete(self,"ui_obj")
end

--点击刷新
function PetDetailView:OnClickType(type)
    if self.click_type[type] == nil then
        if type == 2 then
            self:FlushDetailView()
        elseif type == 3 then
            local total = {}
            --若宠物数据中无已经学会的技能列表则取配置表中的默认技能列表（与宠物百科中展示的相同）
            --例：寻宝中概率展示中的宠物图标
            local skill_list = self.use_config and self.pet:GetSkillAndGiftGroupList() or self.pet:GetSkillAndGiftListNoEmpty()
            -- if self.pet.skill_list == nil and DEBUG_MODE then
            --     Debuger.LogWarning("PetDetailView self.pet.skill_list is nil，show skill_list in config")
            -- end
            for _, v in ipairs(skill_list) do
                v.owner_pet = self.pet
                table.insert(total, v)
            end
            self.SkillList:SetData(total)
        end
        self.click_type[type] = true
    end
end

function PetDetailView:OnClickGift(cell)
    local data = cell:GetData()
    if data.skill_id and data.skill_id ~= 0 or (data.gift_id and data.gift_id ~= 0) then
        PetData.Instance:SetClickSkill(data)
        ViewMgr:OpenView(PetSkillTip)
    end
end




PetDetailSkinPanel = PetDetailSkinPanel or DeclareMono("PetDetailSkinPanel", UIWFlushPanel)
function PetDetailSkinPanel:PetDetailSkinPanel()
    self.pet_data = PetData.Instance
end

function PetDetailSkinPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function PetDetailSkinPanel:SetPet(pet,ui_obj)
    self.pet = pet
    self.ui_obj = ui_obj
    local skin_list = self.pet_data:GetPetSkinCfgList(pet:ID())
    local is_none = TableIsEmpty(skin_list)
    self.Show:SetActive(not is_none)
    self.None:SetActive(is_none)
    if not is_none then
        self.SkinList:SetData(skin_list)
        self.SkinList:SetectItem(1, true)
        self.IndexList:SetData(skin_list)
    end
end

function PetDetailSkinPanel:OnSelClick(data, value)
    if not self.ui_obj then
        return
    end

    local index = self.SkinList:GetSelectedIndex(data)
    self.IndexList:SetectItem(index, value)

    if value then
        local potential = data.potential
        local str = ColorStr("+" .. potential, COLORSTR.Green4)
        local desc = string.format(Language.Pet.PetDetailSkinDesc,str)
        UH.SetText(self.Desc1, desc)

        self.ui_obj:SetData(DrawerHelper.GetNpcPath(data.res_id),self.pet:FeiShengTimes())
        UH.SetText(self.BtnText, Language.Pet.PetDetailBtn2)
        self.use_data = data
    else
        UH.SetText(self.BtnText,Language.Pet.PetDetailBtn1)
        self.ui_obj:SetData(DrawerHelper.GetNpcPath(self.pet:ResId()),self.pet:FeiShengTimes())
        self.use_data = nil
    end 
end

function PetDetailSkinPanel:OnBtnClick()
    if self.use_data then
        local index = self.SkinList:GetSelectedIndex(self.use_data)
        self.SkinList:SetectItem(index, false)
    else
        self.SkinList:SetectItem(1, true)
    end
end



PetDetailSkinItem = PetDetailSkinItem or DeclareMono("PetDetailSkinItem", UIWidgetBaseItem)
function PetDetailSkinItem:PetDetailSkinItem()
    self.pet_data = PetData.Instance
end

function PetDetailSkinItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.SkinImgOn:SetByPath(string.format("pet_skin/%s.png",self.data.res_id))
    self.SkinImgOff:SetByPath(string.format("pet_skin/%s.png",self.data.res_id))

    self.SignOn:SetObjActive(not StringIsEmpty(self.data.limit_sign))
    self.SignOff:SetObjActive(not StringIsEmpty(self.data.limit_sign))

    UH.SetText(self.NameOn, data.name)
    UH.SetText(self.NameOff, data.name)

    if not StringIsEmpty(self.data.limit_sign) then
        self.SignOn:SetData(self.data)
        self.SignOff:SetData(self.data)
    end
end


PetDetailSkinIndexItem = PetDetailSkinIndexItem or DeclareMono("PetDetailSkinIndexItem", UIWidgetBaseItem)
function PetDetailSkinIndexItem:SetData(data)
end
