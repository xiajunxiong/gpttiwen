HuanShouFuShenShow = HuanShouFuShenShow or DeclareView("HuanShouFuShenShow", "huanshou/huanshou_fushen_show")
function HuanShouFuShenShow:HuanShouFuShenShow()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = HuanShouModelInfo})
    end
end

function HuanShouFuShenShow:LoadCallback(param_t)
    local huanshou = param_t.huanshou
    if TableIsEmpty(huanshou) then
        return
    end
    self.huanshou = huanshou
    self:FlushPanel()
end

function HuanShouFuShenShow:FlushPanel()
    local huanshou = self.huanshou
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(huanshou:ResId()))

    local is_not_book = huanshou:IsNotBookSkill()
    self.BookSkillList:SetObjActive(not is_not_book)
    self.NotBookSkill:SetActive(is_not_book)
    if not is_not_book then
        local skill_list = huanshou:BookSkillList()
        self.BookSkillList:SetData(skill_list)
    end

    local ex_skill = huanshou:ExSkill()
    self.NotExSkill:SetActive(ex_skill == nil)
    self.ExSkill:SetObjActive(ex_skill ~= nil)
    if ex_skill then
        self.ExSkill:SetData(ex_skill)
    end

    local attri_list = huanshou:StarAttriList()
    self.AttriList:SetData(attri_list)
    self.NotAttri:SetActive(TableIsEmpty(attri_list))

    local qua_name,name,color = HuanShou.GetQuaName(huanshou.id)
    qua_name = ColorStr("【".. name .. "】", color)
    qua_name = string.format(Language.HuanShou.Text21, qua_name)
    UH.SetText(self.Name, qua_name)
end

function HuanShouFuShenShow:CloseCallback()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
end

function HuanShouFuShenShow:OnCloseClick()
    ViewMgr:CloseView(HuanShouFuShenShow)
end