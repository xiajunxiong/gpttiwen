FlyUpSuccView = FlyUpSuccView or DeclareView("FlyUpSuccView", "fly_up/fly_up_succ_view")

VIEW_DECLARE_LEVEL(FlyUpSuccView,ViewLevel.L0)
VIEW_DECLARE_MASK(FlyUpSuccView,ViewMask.BlockClose)

function FlyUpSuccView:FlyUpSuccView()
    self.data = FlyUpData.Instance
end

function FlyUpSuccView:LoadCallback(param_t)
    local prof = RoleData.Instance:GetRoleProf()
    UH.SetText(self.Name,RoleData.Instance:GetRoleName())
    UH.SetText(self.Level,Format("Lv.%s",RoleData.Instance:GetRoleLevel()))
    UH.SetText(self.ProfName,Language.Common.Prof[prof])
    UH.SpriteName(self.RoleProf, ProfSettingB[prof])
    self.AttrList:SetData(self.data:GetAttrSuccData())
    self:FlushElementView()
    -- self:FlushSkillView()
    self:FlushModelUIShow()
    self:FlushTitleView()
end

function FlyUpSuccView:FlushElementView()
    local data = {
        [BattleAttriType.ElementWind] = RoleData.Instance:GetBattleAttri(BattleAttriType.ElementWind),
        [BattleAttriType.ElementEarth] = RoleData.Instance:GetBattleAttri(BattleAttriType.ElementEarth),
        [BattleAttriType.ElementFire] = RoleData.Instance:GetBattleAttri(BattleAttriType.ElementFire),
        [BattleAttriType.ElementWater] = RoleData.Instance:GetBattleAttri(BattleAttriType.ElementWater)
    }
    self.Element:SetData(data)
end

function FlyUpSuccView:FlushModelUIShow()
    local role_appearance = TableCopy(RoleData.Instance:GetApearance())
    self.cur_appearance = {
		weapon_id = RoleProfessionSimpleWeaponId[role_appearance.prof_base] or 0,
		color = role_appearance.color,
		prof_base = role_appearance.prof_base,
        avatar_type = role_appearance.avatar_type,
        advance_time = role_appearance.advance_time
    }
    if self.ModelGame == nil then
        self.ModelGame = UIChDrawer.CreateByAppearance(self.cur_appearance)
        self.ModelUIShow:SetShowData({ui_obj = self.ModelGame,view = self,load_callback = function()
            self.ModelGame:PlayAnim(ChAnimType.WeaponShow)
        end})
    else
        self.ModelGame:SetData(self.cur_appearance)
    end
end

function FlyUpSuccView:FlushSkillView()
    if FlyUpStageType.Low == FlyUpData.Instance:GetStage() then
        local skill_config = SkillData.GetDecConfig(self.data:GetSkillId())
        UH.SetText(self.SkillName,skill_config.show_name)
        UH.SetText(self.SkillDesc,skill_config.description)
        UH.SetIcon(self.SkillIcon,skill_config.icon_id,ICON_TYPE.SKILL)
        UH.SetText(self.SkillTitle,Language.FlyUp.SkillSuccOpenTip[1])
        self.Cell:SetObjActive(false)
    else
        UH.SetText(self.SkillTitle,Language.FlyUp.SkillSuccOpenTip[2])
        UH.SetText(self.SkillName,Language.FlyUp.SkillOpenTip[3])
        UH.SetText(self.SkillDesc,Language.FlyUp.SkillTip)
        -- self.Cell:SetData(Item.Create({item_id = 65796}))
        self.Cell:ClearCare()
        self.Cell:ClearFlush()
        self.Cell.data = Item.Create({item_id = 56013})
        CellFlushs.SetIconQuality(self.Cell, self.Cell.data.item_id)
        CellFlushs.SetIcon(self.Cell, 56013)
        CellFlushs.SetQuaSpAnim(self.Cell)

        self.Cell:SetObjActive(true)
    end
end

function FlyUpSuccView:FlushTitleView()
    local stage = FlyUpData.Instance:GetStage()
    local config = FlyUpData.Instance:GetOtherConfig()
    local title_id = config["feisheng_title" .. stage]
    self.TitleIcon:SetData(TitleData.GetTitleConfig(title_id))
end

function FlyUpSuccView:CloseCallback()
    Delete(self,[[ModelGame]])
end

----------------------------FlyUpAttrSuccItem----------------------------
FlyUpAttrSuccItem = FlyUpAttrSuccItem or DeclareMono("FlyUpAttrSuccItem", UIWidgetBaseItem)

function FlyUpAttrSuccItem:SetData(data)
    local old_value = data.old_value or 0
    local new_value = data.new_value or 0

    if DataHelper.IsPercent(data.attr_type) then
		old_value = Percent(old_value)
		new_value = Percent(new_value)
    end
    UH.SetText(self.Name,data.attr_name)
    UH.SetText(self.OldNum,old_value)
    UH.SetText(self.NewNum,new_value)
end