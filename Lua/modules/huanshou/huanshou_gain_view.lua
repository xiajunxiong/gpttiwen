-- 获得幻兽界面
HuanShouGainView = HuanShouGainView or DeclareView("HuanShouGainView", "huanshou/huan_shou_gain_view")

function HuanShouGainView:HuanShouGainView()
    self.data = HuanShouData.Instance
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = HuanShouModelInfo})
    end

    self.effect_cfg = {
        [ItemColor.Red] = 7165497,
        [ItemColor.Gold] = 7165496,
    }
end

function HuanShouGainView:LoadCallback(param_t)
    local huanshou = param_t.huanshou
    self.huanshou = huanshou
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(huanshou:ResId()))
    local name = ColorStr(huanshou:Name(), COLORSTR.Yellow7)
    UH.SetText(self.Name, string.format(Language.HuanShou.Text20,name))
    self.AttriList:SetData(huanshou:AttriList())
    local ex_skill = huanshou:ExSkill()
    self.NotSkillTip:SetActive(ex_skill == nil)
    self.ShowSkill:SetActive(ex_skill ~= nil)
    if ex_skill then
        self.SkillCell:SetData(ex_skill)
        UH.SetText(self.SkillDesc, ex_skill:Desc())
    end

    local qua = huanshou:Qua()
    UH.SpriteName(self.QuaImg, "HuanShouQua" .. qua)

    local effect = self.effect_cfg[qua]
    if effect then
        self.EffectTool:Play(effect)
    end
end

function HuanShouGainView:CloseCallback()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
end

function HuanShouGainView:OnCloseClick()
    ViewMgr:CloseView(HuanShouGainView)
end

function HuanShouGainView:OnGoToLookClick()
    self.data:JumpHuanShou(self.huanshou.id)
    ViewMgr:CloseView(HuanShouGainView)
end