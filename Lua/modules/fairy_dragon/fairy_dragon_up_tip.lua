FairyDragonUpTip = FairyDragonUpTip or DeclareView("FairyDragonUpTip", "fairy_dragon/fairy_dragon_up_tip")
function FairyDragonUpTip:FairyDragonUpTip()
    self.param_t = {
        last_level = 0,
        cur_level = 0,
        level_text = Language.FairyDragon.Level,
        attri_list = {
            {attri_type = 0, attri_value_1 = 0, attri_value_2 = 0},
        },

        is_show_skill = false,
        skill_item,
        last_skill_desc = "",
        cur_skill_desc = "",
        item,
    }
end

function FairyDragonUpTip:OnCloseClick()
    ViewMgr:CloseView(FairyDragonUpTip)
end

function FairyDragonUpTip:LoadCallback(param_t)
    if not TableIsEmpty(param_t) then
        param_t.is_show_skill = param_t.is_show_skill or false
        param_t.level_text = param_t.level_text or self.param_t.level_text
        self.param_t = param_t
    end
    local cur_level = self.param_t.cur_level
    self.LevelInfo:SetActive(cur_level ~= nil)
    if cur_level then
        UH.SetText(self.LastLevel, self.param_t.last_level .. self.param_t.level_text)
        UH.SetText(self.CurLevel, self.param_t.cur_level .. self.param_t.level_text)
    end

    local attri_list = self.param_t.attri_list or {}
    table.sort(attri_list, function (a,b)
        local a_num = a.attri_type
        local b_num = b.attri_type
        if a.attri_value_2 > a.attri_value_1 then
            a_num = a_num + 100
        end
        if b.attri_value_2 > b.attri_value_1 then
            b_num = b_num + 100
        end
        return a_num > b_num
    end)

    self.AttriList:SetData(attri_list)

    self.ShowSkill:SetActive(self.param_t.is_show_skill)
    local info_h = self.param_t.is_show_skill and 428 or 308
    local pos_y = self.param_t.is_show_skill and -60 or -34
    self.InfoRect:SetSizeWithCurrentAnchors(1, info_h)
    UH.AnchoredPosition(self.InfoRect, Vector2.New(0,pos_y))

    UH.SetText(self.LastDesc, self.param_t.last_skill_desc or "")
    UH.SetText(self.CurDesc, self.param_t.cur_skill_desc or "")

    local skill_item = self.param_t.skill_item
    self.SkillItem:SetObjActive(skill_item ~= nil)
    if skill_item then
        self.SkillItem:SetData(skill_item)
    end

    local item = param_t.item
    self.ItemCell:SetObjActive(item ~= nil)
    if item then
        self.ItemCell:SetData(item)
    end
end

FairyDragonUpTipAttriItem = FairyDragonUpTipAttriItem or DeclareMono("FairyDragonUpTipAttriItem", UIWidgetBaseItem)
function FairyDragonUpTipAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Img.enabled = data.attri_value_2 > data.attri_value_1
    local attri_info1 = CommonStruct.AttrStruct(data.attri_type, data.attri_value_1)
    local attri_info2 = CommonStruct.AttrStruct(data.attri_type, data.attri_value_2)
    UH.SetText(self.AttriName, attri_info1.attr_name)
    if data.attri_value_1 == 0 then
        UH.SetText(self.AttriValue1, attri_info2.attr_percent_value)
        UH.SetText(self.AttriValue2, Language.FairyDragon.Text19)
        self.Img.enabled = false
    else
        UH.SetText(self.AttriValue1, attri_info1.attr_percent_value)
        UH.SetText(self.AttriValue2, attri_info2.attr_percent_value)
    end
end


-- 星级技能提升
FairyDragonStarSkillUp = FairyDragonStarSkillUp or DeclareView("FairyDragonStarSkillUp", "fairy_dragon/fairy_dragon_star_skill_up")
VIEW_DECLARE_MASK(FairyDragonStarSkillUp, ViewMask.BgBlockCloseNoAnim)
function FairyDragonStarSkillUp:LoadCallback(param_t)
    local skill = param_t.skill
    if not skill then
        return
    end
    UH.SetText(self.Name, skill:Name())
    UH.SetText(self.Desc, skill:Desc())
    self.SkillCell:SetData(skill)
    UH.SetText(self.Tips, string.format(Language.FairyDragon.Text20, skill:Name(), skill:Level()))
end


-- 神龙突破提示
FairyDragonBreachTip = FairyDragonBreachTip or DeclareView("FairyDragonBreachTip", "fairy_dragon/fairy_dragon_breach_tip")
VIEW_DECLARE_MASK(FairyDragonBreachTip, ViewMask.BgBlockCloseNoAnim)
function FairyDragonBreachTip:LoadCallback(param_t)
    local item = Item.Create({item_id = FairyDragon.Instance:ItemId()})
    self.ItemCell:SetData(item)
    UH.SetText(self.Name, FairyDragon.Instance:Name())
    local limit_level = FairyDragon.Instance:LimitLevel()
    limit_level = ColorStr(limit_level, COLORSTR.Red8)
    UH.SetText(self.Tip, string.format(Language.FairyDragon.Text21, limit_level))
end


-- 神龙升星提示
FairyDragonStarUpTip = FairyDragonStarUpTip or DeclareView("FairyDragonStarUpTip", "fairy_dragon/fairy_dragon_star_up_tip")
function FairyDragonStarUpTip:OnCloseClick()
    ViewMgr:CloseView(FairyDragonStarUpTip)
end

function FairyDragonStarUpTip:LoadCallback(param_t)
    local star_list = param_t.star_list or FairyDragon.Instance:GetStarUIList()
    self.StarList:SetData(star_list)
    local attri_list = param_t.attri_list or {}
    table.sort(attri_list, function (a,b)
        local a_num = a.attri_type
        local b_num = b.attri_type
        if a.attri_value_2 > a.attri_value_1 then
            a_num = a_num + 100
        end
        if b.attri_value_2 > b.attri_value_1 then
            b_num = b_num + 100
        end
        return a_num > b_num
    end)
    self.AttriList:SetData(attri_list)
end