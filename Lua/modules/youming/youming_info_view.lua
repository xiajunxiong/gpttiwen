YouMingInfoView = YouMingInfoView or DeclareView("YouMingInfoView", "youming/youming_info_view")
function YouMingInfoView:YouMingInfoView()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end
end

function YouMingInfoView:LoadCallback(param)
    self.Board:SetData(self:GetType(),Language.YouMing.Title1,Vector2.New(973, 561))
    local youming = param.youming
    if not youming then
        return
    end
    self.youming = youming
    self:FlushInfo(youming)
    self:OnClickType(1)
end

function YouMingInfoView:CloseCallback()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
	end
end

function YouMingInfoView:FlushInfo(youming)
    UH.SetText(self.Name, youming:Name())
    self.ui_obj:SetData(youming:ResPath())
    self.TianZi:SetData(youming:AptitudeList())
    local aptitude, max_aptitude = youming:Aptitude()
    UH.SetText(self.TianZiText, string.format(Language.YouMing.TianZi, aptitude, max_aptitude))
    local awake_level = youming:AwakeLevel()
    UH.SpriteName(self.AwakeImg, "PetNum" .. awake_level)
end

function YouMingInfoView:OnClickType(tab_type)
    self.InfoList[tab_type]:SetData(self.youming)
end


YouMingInfoViewAttri = YouMingInfoViewAttri or DeclareMono("YouMingInfoViewAttri")
function YouMingInfoViewAttri:SetData(data)
    local attri_list = data:AttriList()
    self.AttriList:SetData(attri_list)
end

YouMingInfoViewSkill = YouMingInfoViewSkill or DeclareMono("YouMingInfoViewSkill")
function YouMingInfoViewSkill:SetData(data)
    local skill_list = data:SkillList()
    self.SkillInfoList:SetData(skill_list)
    --UH.SetText(self.Tip, string.format("该幽冥最多可拥有%s个被动技能",15))
    if Config.language_cfg_auto.textdesc[408] then
        UH.SetText(self.Tip, Config.language_cfg_auto.textdesc[408].desc)
    else
        UH.SetText(self.Tip, "")
    end
end


YouMingInfoViewSkillItem = YouMingInfoViewSkillItem or DeclareMono("YouMingInfoViewSkillItem", UIWidgetBaseItem)
-- data === youmingskill
function YouMingInfoViewSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.SkillType, Language.YouMing.SkillType[data.skill_type] .. Language.YouMing.Skill)
    self.SkillCell:SetData(data)
    local skill_name = ColorStr(data:Name(), COLORSTR.Yellow16)
    UH.SetText(self.SkillDesc, string.format("%s:%s",skill_name, data:Desc()))
end