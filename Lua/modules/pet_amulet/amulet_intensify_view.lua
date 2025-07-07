AmuletIntensifyView = AmuletIntensifyView or DeclareView("AmuletIntensifyView", "pet_amulet/AmuletIntensifyView")
function AmuletIntensifyView:LoadCallback(param_t)
    local item = param_t.item
    if item then
        AmuletData.Instance:SetAmuletIntensifyData(1, item)
    end
    self.Board:SetData(self:GetType(),Language.Pet.PetEquip.Title1, Vector2.New(1096, 612))
end

function AmuletIntensifyView:OnTipClick()
    PublicPopupCtrl.Instance:HelpTip(405)
end



AmuletIntensifyPanel = AmuletIntensifyPanel or DeclareMono("AmuletIntensifyPanel", UIWFlushPanel)
function AmuletIntensifyPanel:AmuletIntensifyPanel()
    self.data = AmuletData.Instance
    self.data_cares = {
        {data = self.data.intensify_data, func = self.FlushPanel, init_call = false}
    }
end

function AmuletIntensifyPanel:Awake()
    UIWFlushPanel.Awake(self)  
    self:FlushPanel()
end

function AmuletIntensifyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_ht)
end

function AmuletIntensifyPanel:FlushPanel()
    for i = 1, self.ItemList:Length() do
        local data = self.data:GetAmuletIntensifyData(i)
        self.ItemList[i]:SetData(data)
    end

    local skill_info = self.data:GetIntensifySkillInfo()
    for i = 1, self.SkillInfoList:Length() do
        self.SkillInfoList[i]:SetData(skill_info[i] or {})
    end
end

function AmuletIntensifyPanel:OnBtnClick()
    if self.effect_ing then
        return
    end
    local a_item = self.data:GetAmuletIntensifyData(1)
    local b_item = self.data:GetAmuletIntensifyData(2)
    if TableIsEmpty(a_item) or TableIsEmpty(b_item) then
        PublicPopupCtrl.Instance:Center(Language.Pet.PetEquip.Tip3)
        return
    end
    local pet_index = a_item.param.pet_index
    local p4 = pet_index == nil and 0 or 1
    local p1 = pet_index or a_item.index
    local p2 = b_item:GridColumn()
    local p3 = b_item.index
    local func = function()
        for i = 1, self.EffectList:Length() do
            self.EffectList[i]:SetActive(true)
        end
        self.effect_ing = true
        TimeHelper:CancelTimer(self.time_ht)
        self.time_ht = Invoke(function ()
            self.effect_ing = false
            for i = 1, self.EffectList:Length() do
                self.EffectList[i]:SetActive(false)
            end
            PetCtrl.SendPetOp(PET_OP_TYPE.PetAmuletInstensify, p1, p2, p3, p4)
            self.data:ClearIntensifyData()
        end, 2)
    end
    if self.data:IsNeedTip(b_item) then
        PublicPopupCtrl.Instance:AlertTip(Language.Pet.PetEquip.AlertTip, function()
            func()
        end)
    else
        func()
    end
end


AmuletIntensifyItem = AmuletIntensifyItem or DeclareMono("AmuletIntensifyItem", UIWidgetBaseItem)
function AmuletIntensifyItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_none = TableIsEmpty(data)
    self.None:SetActive(is_none)
    self.ItemCell:SetObjActive(not is_none)
    if is_none then
        UH.SetText(self.ItemName, self.Pos == 1 and Language.Pet.PetEquip.Tip4 or Language.Pet.PetEquip.Tip5)
    else
        self.ItemCell:SetData(data)
        UH.SetText(self.ItemName, data:Name())
    end
end

function AmuletIntensifyItem:OnAddClick()
    if self.is_lock then
        return
    end
    PetData.Instance.pet_equip_select.equip = 4
    PetData.Instance.pet_equip_select.pos = self.Pos
    ViewMgr:OpenView(PetEquipSelectView)
end


AmuletIntensifySkillInfo = AmuletIntensifySkillInfo or DeclareMono("AmuletIntensifySkillInfo", UIWidgetBaseItem)
function AmuletIntensifySkillInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_none = TableIsEmpty(data)
    self.Content:SetActive(not is_none)
    self.Empty:SetActive(is_none)
    if not is_none then
        local skill_1 = data.cur_skill
        self.Skill1:SetData(skill_1)
        local skill_2 = data.next_skill
        self.Skill2:SetData(skill_2)
    end
end



AmuletSkillItem = AmuletSkillItem or DeclareMono("AmuletSkillItem", UIWidgetBaseItem)
function AmuletSkillItem:SetData(data) -- data ==== ShengQiSkill
    UIWidgetBaseItem.SetData(self, data)
    local skill_cfg = PetData.Instance:GetGiftDetail(data.skill_id, data.skill_level)
    UH.SetIcon(self.Icon, skill_cfg.icon_id, ICON_TYPE.SKILL)
    UH.SpriteName(self.Qua, PetData.GiftListItemFrameName[skill_cfg.page])
    local name = skill_cfg.skill_name
    UH.SetText(self.Name, name)
    UH.SetText(self.TypeText, Language.Pet.PetEquip.Text1)
    UH.SetText(self.Flag, Language.Pet.PetEquip.Text2)
    UH.SetText(self.Level, "Lv." .. data.skill_level)
end

function AmuletSkillItem:OnShowClick()
    PetData.Instance:SetClickSkill(self.data)
    ViewMgr:OpenView(PetSkillTip)
end