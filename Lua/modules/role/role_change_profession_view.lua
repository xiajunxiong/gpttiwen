RoleChangeProfView = RoleChangeProfView or DeclareView("RoleChangeProfView", "role/role_change_profession")

VIEW_DECLARE_LEVEL(RoleChangeProfView, ViewLevel.L1)
VIEW_DECLARE_MASK(RoleChangeProfView, ViewMask.BlockClose)

function RoleChangeProfView:LoadCallback(param)
    self:FlushModelView()
    self:FlushChangeView()
    self.Teache:SetActive(self:IsInitial())
end

--刷新模型
function RoleChangeProfView:FlushModelView()
    if self.ui_obj == nil then
        self.ui_obj = UIChDrawer.CopyMainRoleData()
    end
    self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    UH.SetText(self.Name,RoleData.Instance:GetRoleName())
end

function RoleChangeProfView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self,"ui_obj")
end

--刷新转职成功界面
function RoleChangeProfView:FlushChangeView()
    local prof_id = RoleData.Instance:GetRoleProfId()
    local prof_grade = RolePromoteData.Instance:GetProfGrade()
    local cur_config = RolePromoteData.GetProfConfig(prof_id,prof_grade)
    local config = TitleData.GetTitleConfig(cur_config.get_title)
    UH.SetText(self.ProfName,config.name)
    self.AttrContain:SetData(DataHelper.FormAttrList(cur_config))
    --初始技能
    local skill_id,split_list = self:GetSkillId(cur_config.skill_id)
    local skill_cfg = Cfg.SkillDescPartnerAndRole(skill_id,1)
    UH.SetText(self.SkillName,skill_cfg.skill_name)
    UH.SetText(self.SkillDesc,skill_cfg.description)
    UH.SetIcon(self.SkillIcon,skill_cfg.icon_id,ICON_TYPE.SKILL)

    --技能列表
    local skill_list = SkillData.GetProfSkillList(math.floor(prof_id / 100))
    if self:IsInitial() then
        for i,v in ipairs(skill_list) do
            v.cur_max_level = self:GetMinLevel(split_list,v.skill_id)
            v.next_max_level = cur_config.max_skill_level
        end
        UH.SetText(self.CurLevel,Format("Lv.%s",0))
        UH.SetText(self.CurPower,0)
        --保存默认技能
        local param_t = {{}}
        param_t[1].first_move_type = BATTLE_MOVE_TYPE.PERFORM_SKILL
        param_t[1].first_skill_id = GuideData.Instance:GetSkillId()
        param_t[1].first_skill_level = 1
        FightCtrl.Instance:SendSetBattleAutoMove(param_t)
    else
        local last_config = RolePromoteData.GetProfConfig(prof_id - 1)
        for i,v in ipairs(skill_list) do
            v.cur_max_level = last_config.max_skill_level
            v.next_max_level = cur_config.max_skill_level
        end
        UH.SetText(self.CurLevel,Format("Lv.%s",last_config.max_equip_level))
        UH.SetText(self.CurPower,last_config.power_up)
    end
    table.sort(skill_list,function(a,b)
        return a.cur_max_level > b.cur_max_level 
    end)
    self.SkillList:SetData(skill_list)
    --装备等级
    UH.SetText(self.NextLevel,Format("Lv.%s",cur_config.max_equip_level))
    UH.SetText(self.NextPower,cur_config.power_up)
    local role_appearance = RoleData.Instance:GetApearance()
    local appearance = {
		weapon_id = RoleProfessionSimpleWeaponId[role_appearance.prof_base] or 0,
		color = role_appearance.color,
		prof_base = role_appearance.prof_base,
        avatar_type = role_appearance.avatar_type,
        advance_time = role_appearance.advance_time,        
    }
    self.ui_obj:SetData(appearance)
end

function RoleChangeProfView:GetSkillId(skill_id)
    if tonumber(skill_id) == nil then
        local split_list = Split(skill_id, "|")
        return tonumber(split_list[1]),split_list
    else
        return skill_id,{}
    end
end

function RoleChangeProfView:IsInitial()
    local prof_id = RoleData.Instance:GetRoleProfId()
    return prof_id % 100 == 1
end

--初始技能激活为1级
function RoleChangeProfView:GetMinLevel(split_list,skill_id)
    for k,v in pairs(split_list or {}) do
        if tonumber(v) == skill_id then
            return 1
        end
    end
    return 0
end

--========================RolePromoteAttrItem========================
RolePromoteAttrItem = RolePromoteAttrItem or DeclareMono("RolePromoteAttrItem", UIWidgetBaseItem)

function RolePromoteAttrItem:SetData(data)
    if data ~= nil then
        local value = DataHelper.Percent(data.attr_type,data.attr_value)
        UH.SetText(self.Name,data.attr_name)
        UH.SetText(self.Value,"+" ..value)
    end
    self.ItemObj:SetActive(data ~= nil)
end
--========================RolePromoteSkillItem========================
RolePromoteSkillItem = RolePromoteSkillItem or DeclareMono("RolePromoteSkillItem", UIWidgetBaseItem)

function RolePromoteSkillItem:SetData(data)
    UH.SetText(self.SkillName,data.skill_name)
    UH.SetText(self.CurLevel,data.cur_max_level and Format("Lv.%s",data.cur_max_level) or "")
    UH.SetText(self.NextLevel,data.next_max_level and Format("Lv.%s",data.next_max_level) or "")
    self.Image:SetActive(data.next_max_level ~= nil)
    local limit_level = RolePromoteData.Instance:GetPromoteSkillLimit(data.skill_id)
    if limit_level ~= 0 and limit_level <= data.next_max_level then
        self.Image:SetActive(false)
        UH.SetText(self.NextLevel,"")
        UH.SetText(self.CurLevel,Format("Lv.%s",limit_level))
    end
end