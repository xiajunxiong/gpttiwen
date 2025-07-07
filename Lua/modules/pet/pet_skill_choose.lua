PetSkillChoose = PetSkillChoose or DeclareView("PetSkillChoose", "pet/pet_skill_choose")
function PetSkillChoose:PetSkillChoose()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance

    GuideManager.Instance:RegisterGetGuideUi("PetSkillPopFirst", function ()
		return self.SkillList.item_list[1], function ()
            self.SkillList.item_list[1].ItemClick:Trigger()
        end
	end)
end
function PetSkillChoose:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    GuideManager.Instance:UnRegisterGetGuideUi("PetSkillPopFirst")
end 
function PetSkillChoose:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.SkillChoose, Vector2.New(566, 548))
end
function PetSkillChoose:OnFlush()
    -- body

    local data = BagData.Instance:GetSkillGrid()
    self.None:SetActive(table.nums(data) == 0)
    self.SkillList:SetData(data)
end
function PetSkillChoose:OnClickSkillItem(data)
    local level = RoleData.Instance:GetRoleLevel()
    if data.limit_level > level then
        PublicPopupCtrl.Instance:Center(ErrorText[6])
        return
    end
    self.pet_data:SetSelectSkill(data)
    ViewMgr:CloseView(PetSkillChoose)
end
PetSkillChooseItem = PetSkillChooseItem or DeclareMono("PetSkillChooseItem", UIWidgetBaseItem)
function PetSkillChooseItem:PetSkillChooseItem()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance
end
function PetSkillChooseItem:SetData(data)
    -- 重新给一个新的data免得修改到背包里的数据
    -- param1 技能id param2 阶级 param3 限制等级
    local item_cfg = Item.GetConfig(data.item_id)
    local skill_id = item_cfg.param2 * PetData.SkillFactor + item_cfg.param1
    local skill_cfg = self.pet_data:GetSkillCfg(skill_id)
    
    local new_data = {}
    new_data.index = data.index
    new_data.name = item_cfg.name
    if skill_cfg then
        new_data.desc = skill_cfg.description
    else
        new_data.desc = "无配置"
        LogError("没技能配置",skill_id,data.item_id)
    end
    new_data.item_id = data.item_id
    new_data.skill_id = skill_id
    new_data.icon_id = skill_cfg.icon_id
    -- new_data.skill_cfg = skill_cfg
    
    new_data.level = item_cfg.param2
    new_data.limit_level = item_cfg.param3
    UIWidgetBaseItem.SetData(self, new_data)

    self.SkillCell:SetDataByItemId(new_data.item_id)
    UH.SetText(self.LblName, new_data.name)
    UH.SetText(self.LblDesc, new_data.desc)
    UH.SetText(self.LblLimit, string.format(self.lang.CanLearn, new_data.limit_level))
    local level = RoleData.Instance:GetRoleLevel()
    self.ToggleLimit.isOn = level >= new_data.limit_level
end
--[[ function PetSkillChooseItem:Click()
    Debuger.LogError("点击")
    self.ToggleItem.isOn = true
end ]]
