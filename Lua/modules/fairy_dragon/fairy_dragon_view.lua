-- >>>>>>>>>>>>>>> 神龙 <<<<<<<<<<<<<<<<
FairyDragonView = FairyDragonView or DeclareView("FairyDragonView", "fairy_dragon/fairy_dragon_view", Mod.FairyDragon.Main)
function FairyDragonView:FairyDragonView()
    self.data = FairyDragonData.Instance
    self.tab_list_data = {
        {name = Language.FairyDragon.Tab[1], tab_objs = {self.FosterPanel,self.ModelInfo1}, open = Mod.FairyDragon.Foster, mod = TabMod(Mod.FairyDragon.Foster)},
        {name = Language.FairyDragon.Tab[2], tab_objs = {self.StarPanel,self.ModelInfo1}, open = Mod.FairyDragon.Star, mod = TabMod(Mod.FairyDragon.Star)},
        {name = Language.FairyDragon.Tab[3], tab_objs = {self.SkinPanel}, open = Mod.FairyDragon.Skin, mod = TabMod(Mod.FairyDragon.Skin)},
        {name = Language.FairyDragon.Tab[4], tab_objs = {self.SoulPanel, self.SoulMask}, open = Mod.FairyDragon.Soul, mod = TabMod(Mod.FairyDragon.Soul)},
        {name = Language.FairyDragon.Tab[5], tab_objs = {self.PrayPanel, self.SoulMask}, open = Mod.FairyDragon.Pray, mod = TabMod(Mod.FairyDragon.Pray)},
    }
    self.TabbarList:SetData(self.tab_list_data)
    self.Board = {Tabbar = self.TabbarList}
end

function FairyDragonView:OnCloseClick()
    ViewMgr:CloseView(FairyDragonView)
end

function FairyDragonView:LoadCallback(param_t)
    param_t = param_t or {}
    local index = self:GetTabIndex(param_t.mod_key or 0)
    self.TabbarList:ClickItem(index)
end

function FairyDragonView:GetTabIndex(mod_key)
    for i,v in pairs(self.tab_list_data) do
        if v.open == mod_key then
            return i
        end
    end
    return 1
end

function FairyDragonView:OnTabClick(data)
    self.data:SetSelTab(data)
    self.cur_tab_data = data
end

function FairyDragonView:BtnTipClick()
    PublicPopupCtrl.Instance:HelpTip(394)
end


-- =============== 神龙技能 =============
FairyDragonSkillItem = FairyDragonSkillItem or DeclareMono("FairyDragonSkillItem", UIWidgetBaseItem)
-- data === FairyDragonSkill
function FairyDragonSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetIcon(self.Icon, data:Icon(), ICON_TYPE.BUFF)
    local level = data:Level() or 0
    UH.SetText(self.Level, string.format("Lv.%s",level))
    self.LevelObj:SetActive(level > 0)
    UH.SetInter(self.Inter, data.is_gray ~= true)
    UH.SetText(self.Name, data:Name())
end

function FairyDragonSkillItem:OnClick()
    if self.IsNotEvent then
        return
    end
    if self.data.is_gray == true and self.data.lock_tip then
        PublicPopupCtrl.Instance:Error(self.data.lock_tip)
        --return
    end
    local pos = UH.GetRootPosition(self.Icon.gameObject)
    pos.y = pos.y + 55
    if self.X then
        pos.x = self.X
    end
    if self.Y then
        pos.y = self.Y
    end
    if self.data.show_type == 1 then
        ViewMgr:OpenView(FairyDragonSkillTip1, {skill = self.data, pos = pos})
    elseif self.data.show_type == 2 then
        ViewMgr:OpenView(FairyDragonSkillTip2, {skill = self.data, pos = pos})
    end
end


FairyDragonSkillTip1 = FairyDragonSkillTip1 or DeclareView("FairyDragonSkillTip1", "fairy_dragon/fairy_dragon_skill_tip1")
VIEW_DECLARE_MASK(FairyDragonSkillTip1,ViewMask.BlockClose)
function FairyDragonSkillTip1:LoadCallback(param_t)
    local skill = param_t.skill
    self.skill = param_t.skill
    self.SkillItem:SetData(skill)
    UH.SetText(self.Name, skill:Name())
    UH.SetText(self.Desc, skill:Desc())
    local pos = param_t.pos
    if pos then
        --self:CheckPos(pos)
        UH.LocalPos(self.PosRect,pos)
    end

    local desc_h = self.Desc.preferredHeight
    local h = 94 + desc_h
    h = h < 114 and 114 or h
    self.InfoRect:SetSizeWithCurrentAnchors(1,h)
end


function FairyDragonSkillTip1:CheckPos(pos)
    if pos.x > 400 then
        pos.x = 400
    end
    if pos.x < -500 then
        pos.x = -500
    end
end


FairyDragonSkillTip2 = FairyDragonSkillTip2 or DeclareView("FairyDragonSkillTip2", "fairy_dragon/fairy_dragon_skill_tip2")
VIEW_DECLARE_MASK(FairyDragonSkillTip2,ViewMask.BlockClose)
function FairyDragonSkillTip2:LoadCallback(param_t)
    local skill_1 = param_t.skill
    if skill_1.show_type ~= 2 then
        return
    end

    local pos = param_t.pos
    if pos then
        self:CheckPos(pos)
        UH.LocalPos(self.PosRect,pos)
    end

    local skill_2, unlock_cfg = skill_1:NextSkill()
    self.SkillItem:SetData(skill_1)
    UH.SetText(self.Name, skill_1:Name())
    UH.SetText(self.Level, string.format(Language.FairyDragon.Text1, skill_1:Level()))
    if skill_1.lock_tip then
        local tip = ColorStr(string.format("(%s)",skill_1.lock_tip), COLORSTR.Red8)
        UH.SetText(self.CurDesc, string.format("%s %s", skill_1:Desc(), tip))
    else
        UH.SetText(self.CurDesc, skill_1:Desc())
    end
    if skill_2 then
        UH.SetText(self.NextDesc, skill_2:Desc())
    else
        UH.SetText(self.NextDesc, Language.FairyDragon.MaxLevelText)
    end

    self.Title3:SetActive(unlock_cfg ~= nil)
    self.TipDesc:SetObjActive(unlock_cfg ~= nil)
    if unlock_cfg ~= nil then
        UH.SetText(self.TipDesc, string.format(Language.FairyDragon.Text22, unlock_cfg.dragon_star,unlock_cfg.point))
    end

    local desc_h = self.CurDesc.preferredHeight
    self.CurDescRect:SetSizeWithCurrentAnchors(1,desc_h + 5)
    desc_h = self.NextDesc.preferredHeight
    self.NextDescRect:SetSizeWithCurrentAnchors(1,desc_h + 5)
end

function FairyDragonSkillTip2:CheckPos(pos)
    if pos.x > 400 then
        pos.x = 400
    end
    if pos.x < -500 then
        pos.x = -500
    end
end



-- 功能开启展示界面
FairyDragonOpenView = FairyDragonOpenView or DeclareView("FairyDragonOpenView", "fairy_dragon/fairy_dragon_open_show")
VIEW_DECLARE_LEVEL(FairyDragonOpenView, ViewLevel.Lu)
function FairyDragonOpenView:LoadCallback()
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = FairyDragonSkinPanel,
                show_shadow = true,
			}
		)
    end
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(FairyDragon.Instance:ResId()))
    UH.SetText(self.Name, Language.FairyDragon.Name)
end

function FairyDragonOpenView:CloseCallback()
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
end

function FairyDragonOpenView:OnBtnClick()
    ViewMgr:CloseView(FairyDragonOpenView)
    ViewMgr:OpenViewByKey(Mod.FairyDragon.Foster)
end