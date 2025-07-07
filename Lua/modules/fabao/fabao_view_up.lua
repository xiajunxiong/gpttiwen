FabaoUpAttrView = FabaoUpAttrView or DeclareView("FabaoUpAttrView", "fabao/fabao_up_attr")

function FabaoUpAttrView:FabaoUpAttrView()
    self.data = FabaoData.Instance
    self.lang = Language.Fabao
end

function FabaoUpAttrView:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.UpAttrTitle, Vector2.New(654, 520))
end

function FabaoUpAttrView:OnFlush()
    local data = self.data:ViewData().select_data
    if data == nil then
        return
    end
    UH.SetText(self.UpLevel, self.lang.AllAttrAdd)
    local list = self.data:AttrAddition()
    local attr_name
    local attr_value
    local count = 1
    for attr_type, value in pairs(list) do
        attr_name = Language.Common.AttrList[attr_type]
        attr_value = DataHelper.Percent(attr_type, value)
        UH.SetText(self.Attrs[count], string.format(self.lang.AttrValue2, attr_name, attr_value))
        count = count + 1
    end
    for i = 1, self.Attrs:Length() do
        if i > count then
            UH.SetText(self.Attrs[i], "")
        end
    end
    self.SkillFCell:SetData(data)
    UH.SetText(self.SkillFName, data.name)
    if data.info.fabao_level <= 0 then
        UH.SetText(self.SkillFAttrsCur[1], Language.Fabao.NoActive)
        self.SkillFAttrsCurObj[2]:SetActive(false)
        return
    end
    local upgrade = self.data:UpgradeCfg(data.id, data.info.fabao_level)[1]
    local attr_type, value
    for i = 1, 2 do
        attr_type = upgrade["attr_type_" .. i]
        value = upgrade["attr_num_" .. i]
        if value > 0 then
            UH.SetText(
                self.SkillFAttrsCur[i],
                string.format(
                    Language.Partner.Up.skillf_attr,
                    Language.Common.AttrList[attr_type],
                    DataHelper.Percent(attr_type, value)
                )
            )
        else
            self.SkillFAttrsCurObj[i]:SetActive(false)
        end
    end

    --[[ 	local skill_f = data.skill_f
	self.SkillFCell:SetData(skill_f)
	self.SkillFName.text = skill_f.info.name
	-- self.SkillFLevel.text = nil == skill_f.skill_cur and self.language.skillf_unactive or string.format(self.language.skillf_level, skill_f.info.level)
	-- self.SkillLevelF.text = skill_f.info.level and string.format(Language.Partner.SkillInfo.skill_level_f, skill_f.info.level) or ""
	self.SkillLevelF.text = nil == skill_f.skill_cur and self.language.skillf_unactive or string.format(Language.Partner.SkillInfo.skill_level_f, skill_f.info.level)
	if nil ~= skill_f.info then
		self.SkillFCur:SetActive(true)
		for i = 1, self.SkillFAttrsCur:Length() do
			local attrs = skill_f.attrs_cur or skill_f.attrs_next
			local attr = attrs[i]
			if attr then
				self.SkillFAttrsCur[i].text = string.format(self.language.skillf_attr, attr.attr_name, attr.attr_value)
			end
		end
	else
		self.SkillFCur:SetActive(false)
	end ]]
    -- if nil ~= skill_f.skill_next then
    -- 	self.SkillFNext:SetActive(true)
    -- 	for i = 1, self.SkillFAttrsNext:Length() do
    -- 		local attr = skill_f.attrs_next[i]
    -- 		if attr then
    -- 			self.SkillFAttrsNext[i].text = string.format(self.language.skillf_attr, attr.attr_name, attr.attr_value)
    -- 		end
    -- 	end
    -- else
    -- 	self.SkillFNext:SetActive(false)
    -- end
    -- self.SkillFLine:SetActive(nil ~= skill_f.skill_cur and nil ~= skill_f.skill_next)
end
