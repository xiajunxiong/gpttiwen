GuildInbornUpView = GuildInbornUpView or DeclareView("GuildInbornUpView", "guild/guild_inborn_up")
VIEW_DECLARE_MASK(GuildInbornUpView, ViewMask.BgBlockClose)
function GuildInbornUpView:LoadCallback(param_t)
    self.SkillItem:SetData(param_t)

    local skill_level = param_t.skill_level --打开之前的等级
    if skill_level == 0 then
        UH.SetText(self.LastDesc, Language.Guild.GuildInbornNo)
    else
        local skill_cfg = SkillData.Instance:GetInbornSkillDesc(param_t.inborn_skill, skill_level) or {}
        UH.SetText(self.LastDesc, skill_cfg.description or "")
    end
    local skill_cfg = SkillData.Instance:GetInbornSkillDesc(param_t.inborn_skill, skill_level + 1) or {}
    UH.SetText(self.CurrDesc, skill_cfg.description or "")

    local level_str = Format(Language.Guild.GuildInbornLv, skill_level + 1)
    UH.SetText(self.SkillName, skill_cfg.skill_name .. level_str)
end

function GuildInbornUpView:OnClickClose()
    ViewMgr:CloseView(GuildInbornUpView)
end

--图标
GuildInbornUpIcon = GuildInbornUpIcon or DeclareMono("GuildInbornUpIcon", UIWidgetBaseItem)
function GuildInbornUpIcon:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local skill_level = data.skill_level
    local skill_cfg = SkillData.Instance:GetInbornSkillDesc(self.data.inborn_skill, math.max(skill_level, 1)) or {}
    UH.SetIcon(self.Icon, skill_cfg.icon_id, ICON_TYPE.SKILL)
    self.Grade:SetActive(data.grade == 1)
    UH.SetText(self.LevelText, math.max(skill_level, 1))

    local is_active = true --GuildData.Instance:IsInbornSkillActive(data.inborn_type, data.type_seq)
    self.Mask:SetActive(not is_active)
    self.Actived:SetActive(is_active)
end

