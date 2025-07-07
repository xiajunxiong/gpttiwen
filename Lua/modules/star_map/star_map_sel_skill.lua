StarMapSelSkillView = StarMapSelSkillView or DeclareView("StarMapSelSkillView", "star_map/star_map_sel_skill_view")
function StarMapSelSkillView:StarMapSelSkillView()
    self.data = StarMapData.Instance
    self.Board:SetData(self:GetType(), Language.StarMap.Title1 ,Vector2.New(688, 600))
end

function StarMapSelSkillView:LoadCallback(param_t)
    local map_type = param_t.map_type or StarMapCtrl.MapType.ROLE
    local hole_index = param_t.hole_index or 0
    StarMapSelSkillView.HoleIndex = hole_index
    self.map_type = map_type
    local list_data = self.data:GetSuperSkillListData(map_type)
    self.List:SetData(list_data)
end



StarMapSelSkillItem = StarMapSelSkillItem or DeclareMono("StarMapSelSkillItem", UIWidgetBaseItem)
-- data = Config.starmap_auto.superstar
function StarMapSelSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local level = StarMapData.Instance:GetSuperStarLevel(data.star_id)
    local skill_cfg = StarMapData.Instance:GetSkillCfg(data.param1, data.param2)
    local btn_text = level > 0 and Language.StarMap.Btn1 or Language.StarMap.Btn2
    self.is_used = false
    if level > 0 then
        self.is_used = StarMapData.Instance:IsUsedSkillBySuperSeq(self.data.star_id)
        if self.is_used then
            btn_text = Language.StarMap.Btn3
        end
    end
    UH.SetInter(self.Inter, not self.is_used)
    UH.SetText(self.BtnText, btn_text)
    UH.SpriteName(self.SkillIcon, skill_cfg.icon_id)
    UH.SetText(self.SkillName, skill_cfg.skill_name)
    UH.SetText(self.Desc, skill_cfg.description)
    UH.SetInter(self.SkillInter, level > 0)
end

function StarMapSelSkillItem:BtnClick()
    if self.is_used then
        PublicPopupCtrl.Instance:Center(Language.StarMap.Tip2)
        return
    end
    local level = StarMapData.Instance:GetSuperStarLevel(self.data.star_id)
    if level > 0 then
        StarMapCtrl.Instance:SendReq(StarMapCtrl.OpType.InstallSkill, self.data.target, StarMapSelSkillView.HoleIndex, self.data.star_id)
    else
        StarMapData.Instance:SetJumpSuper(self.data.star_id)
        ViewMgr:OpenView(StarMapSuperView)
    end
    ViewMgr:CloseView(StarMapSelSkillView)
end