MarrySkillView = MarrySkillView or DeclareView("MarrySkillView", "marry/marry_skill")

function MarrySkillView:MarrySkillView()
    self.data = MarryData.Instance
    self.Board:SetData(self:GetType(), Language.Marry.TitleNameList[2], Vector2.New(820, 531),true)
end

function MarrySkillView:LoadCallback(param_t)
    self.List:SetData(Config.get_married_auto.marry_skill)
end

function MarrySkillView:CloseCallback()
    local skill_flag = self.data:GetSelfInfo().skill_flag or {}
    local is_has_set = false
    for k,v in pairs(self.data.skill_flag_list) do
        if skill_flag[k] ~= v then
            is_has_set = true
        end
    end
    if is_has_set == true then
        MarryCtrl.Instance:SendMarryReq(MarryConfig.REQUEST_TYPE.SKILL_NOT_FLAG,self.data:GetSkillFlagList())
        PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[23])
    end
end

----------------------------MarrySkillItem----------------------------
MarrySkillItem = MarrySkillItem or DeclareMono("MarrySkillItem", UIWidgetBaseItem)

function MarrySkillItem:SetData(data)
    local intimacy_num = MarryData.Instance:GetIntimacyNum()
    UH.SetText(self.SkillName,data.skill_name)
    UH.SetText(self.SkillDesc,data.description)
    UH.SetIcon(self.SkillIcon,data.icon_id,ICON_TYPE.SKILL)
    UH.SetText(self.Intimacy,Format(Language.Marry.SkillIntimacyTip,intimacy_num,data.marry_intimacy))
    self.Toggle.isOn = self:GetFlag(data.seq) == 0
    MarryData.Instance:SetSkillFlagList(data.seq,self.Toggle.isOn and 0 or 1)
    UIWidgetBaseItem.SetData(self, data)
end

function MarrySkillItem:GetFlag(seq)
    local info = MarryData.Instance:GetSelfInfo()
    return info.skill_flag and (info.skill_flag[32 - seq] or 0) or 0
end

function MarrySkillItem:OnClickToggle()
    if self.data then
        MarryData.Instance:SetSkillFlagList(self.data.seq,self.Toggle.isOn and 0 or 1)
    end
end