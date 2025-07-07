-- 圣器详细信息
ShengQiDetailView = ShengQiDetailView or DeclareView("ShengQiDetailView", "sheng_qi/sheng_qi_detail_view")
function ShengQiDetailView:ShengQiDetailView()
    self.Board:SetData(self:GetType(),Language.ShengQi.Title8,Vector2.New(994, 639))
end

function ShengQiDetailView:LoadCallback(param_t)
    local sheng_qi = param_t.sheng_qi
    if not sheng_qi then
        return
    end
    self.AttriInfo:SetData(sheng_qi)

    for i = 1, 2 do
        local skill = sheng_qi:GetTalentSkill(i - 1)
        self.SkillList[i]:SetObjActive(not TableIsEmpty(skill) and skill.skill_id ~= 0)
        if not TableIsEmpty(skill) and skill.skill_id ~= 0 then
            self.SkillList[i]:SetData(skill)
        end
    end

    self.List:SetData(Config.shengqi_auto.att_dec)
end

function ShengQiDetailView:OnTipClick()
    ViewMgr:OpenView(ShengQiSkillPoolView, {tab_type = 1})
end



ShengQiDetailDescItem = ShengQiDetailDescItem or DeclareMono("ShengQiDetailDescItem", UIWidgetBaseItem)
function ShengQiDetailDescItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.att_name)
    UH.SetText(self.Desc, data.att_dec)
    self.ItemRect:SetSizeWithCurrentAnchors(1,30 + self.Desc.preferredHeight)
    local name_len = #RichTextHelper.ParseUTF8Str(data.att_name)
    self.LineRect:SetSizeWithCurrentAnchors(0,392 - (24 * name_len))
end