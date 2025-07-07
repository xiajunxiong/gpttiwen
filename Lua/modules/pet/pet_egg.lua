PetEgg = PetEgg or DeclareView("PetEgg", "pet/pet_egg")
VIEW_DECLARE_LEVEL(PetEgg, ViewLevel.L1)
VIEW_DECLARE_MASK(PetEgg, ViewMask.Block)
function PetEgg:PetEgg()
    GuideManager.Instance:RegisterGetGuideUi("PetEggClose",
	function ()
		return self.GuideFrame,function ()
            ViewMgr:CloseView(PetEgg)
        end
	end)
end
function PetEgg:LoadCallback()
    local pet = PetData.Instance.new_pet
    if pet == nil then
        return
    end
    local v1 = pet:TotalGrowth()
    local v2 = pet:ConfigGrowth()
    if v2 - v1 <= 5 then
        AudioMgr:PlayEffect(AudioType.UI, "pet_egg2")
        self.EffectTool:Play(10010217)
        self.EffectTool:Play(10010228)
    else
        AudioMgr:PlayEffect(AudioType.UI, "pet_egg1")
        self.EffectTool:Play(10010218)
    end
end

PetEggPanel = PetEggPanel or DeclareMono("PetEggPanel", UIWFlushPanel)
function PetEggPanel:PetEggPanel()
    self.pet_data = PetData.Instance
    self.data_cares = {
        {data = self.pet_data.pet_list, func = self.FlushPetInfo, keys = {"index"}}
    }
    if self.ui_obj_egg == nil then
        self.ui_obj_egg = UIObjDrawer.New()
        self.ModelShow2:SetShowData({
            ui_obj = self.ui_obj_egg,
            view = self,
            seq = 2
        })
    end
    self.egg_model = {
        "dan_tong",
        "dan_yin",
        "dan_jin",
        "dan_jin",
        "dan_jin",
        "dan_jin",
        "dan_jin",
    }
    self.can_close = false
end
function PetEggPanel:FlushPetInfo()
    local pet = self.pet_data.new_pet
    if pet == nil then
        return
    end
    self.ui_obj_egg:SetData(DrawerHelper.GetUiModelPath(self.egg_model[pet:Quality()]))
    self.EleAttr:SetData(pet:EleAttr())
    self.EleAttr2:SetData(pet:EleAttr())
    UH.SpriteName(self.ImgRace, string.format("Race%s", pet:RaceId()))
    UH.SpriteName(self.ImgType, PetData.PetLevel2[pet:Quality()])
    UH.SetText(self.LblType, Language.Pet.RaceClass[pet:RaceId()])
    UH.SpriteName(self.ImgJie, PetData.PetLevel2[pet:Quality()])
    UH.SpriteName(self.ImgJieBg, string.format("JieBie%d", pet:Quality()))
    UH.SpriteName(self.ImgJieBg1, string.format("JieBie%d", pet:Quality()))
    UH.SpriteName(self.StrLevel, PetData.PetStrLevel[pet:StrLevel()])
    self.StrengthObj:SetActive(pet:StrLevel() ~= 1)
    self.Mutant:SetActive(pet.config.mutant_times >= 1)
    local v1 = pet:TotalGrowth()
    local v2 = pet:ConfigGrowth()
    UH.SetText(self.LblGroup, string.format("%s/%s", v1, v2))
    local name = string.splitzh(pet:Name())
    for i = 1, self.NameList:Length() do
        UH.SetText(self.NameList[i], name[i] or "")
    end
    self.AttrList:SetPet(pet)
    local total = {}
    for i, v in ipairs(pet:GetSkillAndGiftListNoEmpty()) do
        table.insert(total, v)
    end
    self.SkillList:SetData(total)

    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self,
            seq = 1
        })
    end
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet:ResId()))
    if self.show_tips_delay == nil then
        self.show_tips_delay = TimeHelper:AddDelayTimer(function ()
            self.can_close = true
            self.TipsObj:SetActive(true)
        end, 4.5)
    end
end

function PetEggPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
    if self.show_tips_delay ~= nil then
        TimeHelper:CancelTimer(self.show_tips_delay)
        self.show_tips_delay = nil
    end
	self.ui_obj = nil
    self.can_close = false
    self.pet_data.new_pet = nil
    PetCtrl.Instance:PopDelayPetShow()
end
function PetEggPanel:OnEffectEnd()
    self.BG.raycastTarget = true
end

function PetEggPanel:OnClickClose()
    if self.can_close then
        ViewMgr:CloseView(PetEgg)
    end
end
PetAttrList = PetAttrList or DeclareMono("PetAttrList", UIWFlushPanel)
--MaxLength 进度条长度
--ColorIndex 使用颜色组 1是浅色 JinDuTiaoDi JinDuTiao_JianGe2  2是深色 JinDuTiao_1 JinDuTiao_JianGe
--MaxHeight 进度条高度
--BgList
--SideList
local color_list = {
    [1] = {COLORSTR.Red12, COLORSTR.Yellow13, COLORS.Red12, COLORS.Yellow13},
    [2] = {COLORSTR.Yellow22, COLORSTR.Yellow5, COLORS.Yellow22, COLORS.Yellow5},
    [3] = {COLORSTR.Yellow5, COLORSTR.Yellow5, COLORS.Yellow5, COLORS.Yellow5}, --异兽临世使用
}
function PetAttrList:SetPet(pet,use_config)
    local qua_green = QualityColorStr[2]
    local qua_red = QualityColorStr[6]
    self.MaxLength = self.MaxLength == nil and 180 or self.MaxLength
    self.MaxHeight = self.MaxHeight == nil and 22 or self.MaxHeight
    if self.ColorIndex ~= 3 then
        self.ColorIndex = self.ColorIndex == 2 and 2 or 1
    end
    local total_growth_list = pet:TotalGrowthList()
    if use_config == true then
        total_growth_list = pet:ConfigGrowthList()
    end
    local config_growth_list = pet:ConfigGrowthList()
    for k, v in pairs(config_growth_list) do
        local index = PetData.GetZZIndex(k)
        self.ZZList[index].sizeDelta = Vector2.New(self.MaxLength, self.MaxHeight)
        self.SliderList[index].value = (6 - v + total_growth_list[k]) / 6
        local str_color = index == pet:MainAttr() and color_list[self.ColorIndex][1] or color_list[self.ColorIndex][2]
        UH.SetText(
            self.ValueList[index],
            string.format(
                Language.Pet.PetZZ2,
                str_color,
                tostring(total_growth_list[k]),
                string.format( "%.1f", tonumber(v))
            )
        )
        if self.Strengthen then
            local old_level, is_succ = PetData.Instance:OldStrLevel()
            -- local old_cfg_list = pet:ConfigGrowthList(old_level)
            local old_growth_list = pet:TotalGrowthList(old_level)

            local add = total_growth_list[k] - old_growth_list[k]
            add = string.format("%.1f", tonumber(add))
            add = tonumber(add)
            UH.SetText(
                self.ValueList[index],
                string.format(
                    Language.Pet.GrowthStr3,
                    total_growth_list[k],
                    add > 0 and qua_green or qua_red,
                    add > 0 and string.format("+%s", add) or add,
                    v,
                    add > 0 and qua_green or qua_red,
                    add > 0 and string.format("+%s", add) or add
                )
            )
        elseif self.StrengthenYao then
            
        else
            UH.SetText(
                self.ValueList[index],
                string.format(
                    Language.Pet.PetZZ2,
                    str_color,
                    tostring(total_growth_list[k]),
                    tostring(v)
                )
            )
            UH.SpriteName(self.BgList[index], (self.ColorIndex == 1) and "JinDuTiaoDi" or "JinDuTiao_1")
            local sidelist =self["SideList"..index]
            for i = 1, sidelist:Length() do
                UH.SpriteName(sidelist[i], (self.ColorIndex == 1) and "JinDuTiao_JianGe2" or "JinDuTiao_JianGe") 
            end
        end
        UH.Color(
            self.TitleList[index],
            index == pet:MainAttr() and color_list[self.ColorIndex or 1][3] or color_list[self.ColorIndex or 1][4]
        )
    end
    -- 强化特殊处理

    --黄22
end

-------------------------------------------------
PetSkillitem2 = PetSkillitem2 or DeclareMono("PetSkillitem2", UIWidgetBaseItem)
function PetSkillitem2:PetSkillitem2()
    -- body
end
function PetSkillitem2:SetData(data)
    local new_data = TableCopy(data)
    local skill_cfg = PetData.Instance:GetGiftDetail(data.skill_id)
    local desc_cfg = SkillData.GetDecConfig(data.skill_id, data.skill_level)
    if skill_cfg.name == nil then
        skill_cfg = SkillData.GetConfig(data.skill_id, data.skill_level)
        UH.SetText(self.Name, skill_cfg.skill_name)
        new_data.skill_cfg = new_data.skill_cfg or {}
        new_data.skill_cfg.skill_id = skill_cfg.skill_id or 0
        new_data.skill_cfg.skill_level = data.skill_level
        new_data.skill_cfg.description = desc_cfg.description
        new_data.skill_cfg.icon_id = desc_cfg.icon_id
        new_data.skill_cfg.skill_name = skill_cfg.skill_name
        new_data.skill_cfg.corner_icon = desc_cfg.corner_icon
        UH.SetIcon(self.Icon, desc_cfg.icon_id, ICON_TYPE.SKILL)
    else
        UH.SetText(self.Name, skill_cfg.name)
        new_data.skill_id = data.skill_id or 0
        new_data.desc = skill_cfg.desc_2
        new_data.name = skill_cfg.name
        new_data.corner_icon = desc_cfg and desc_cfg.corner_icon or nil
        new_data.skill_cfg = TableCopy(skill_cfg)
        UH.SetIcon(self.Icon, skill_cfg.icon_id, ICON_TYPE.SKILL)
    end
    -- if data.skill_cfg then
    --     UH.SetIcon(self.Icon, data.skill_cfg.icon_id, ICON_TYPE.SKILL)
    -- end
    -- if data.icon_id then
    --     UH.SetIcon(self.Icon, data.icon_id, ICON_TYPE.SKILL)
    --     UH.SetText(self.Name, data.name)
    --     UH.SetText(self.Qua, data.quality)
    -- end
    if not data.main_skill then
        --第一页的是初级，第二页的是高级
        local imgName = skill_cfg and tostring(skill_cfg.page)
        if imgName ~= nil then
            UH.SpriteName(self.SkillQuaImg, "PetSkillQua"..imgName)
            self.SkillQuaObj:SetActive(true)
        else
            self.SkillQuaObj:SetActive(false)
        end
    else
        self.SkillQuaObj:SetActive(false)
    end
    if desc_cfg then
        if desc_cfg.corner_icon ~= nil and desc_cfg.corner_icon ~= "" then
            UH.SetText(self.SkillCornerText, desc_cfg.corner_icon)
        end
        self.SkillCornerObj:SetActive(desc_cfg.corner_icon ~= nil and desc_cfg.corner_icon ~= "")
    else
        self.SkillCornerObj:SetActive(false)
    end
    self.SkillLock:SetActive(data.main_skill)
    UIWidgetBaseItem.SetData(self, new_data)
end

function PetSkillitem2:OnClickSkill(data)
    PetData.Instance:SetClickSkill(data)
    ViewMgr:OpenView(PetSkillTip)
end
