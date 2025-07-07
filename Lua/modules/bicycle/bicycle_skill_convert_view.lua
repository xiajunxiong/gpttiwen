-- 灵骑技能置换
BicycleSkillConvertView = BicycleSkillConvertView or DeclareView("BicycleSkillConvertView", "bicycle/new_skill/bicycle_skill_convert_view")

function BicycleSkillConvertView:BicycleSkillConvertView()
    self.data = BicycleOtherData.Instance
    self.Board:SetData(self:GetType(),Language.Bicycle.Title5,Vector2.New(934, 495))
end


function BicycleSkillConvertView:LoadCallback(param_t)
    self.l_skill = param_t.skill
    if not self.l_skill then
        return
    end
    self.LInfo:SetData(self.l_skill)
    self:FlushPanel()
end

function BicycleSkillConvertView:FlushPanel()
    local r_none = TableIsEmpty(self.stuff_data)
    self.AddObj:SetActive(r_none)
    self.Stuff:SetObjActive(not r_none)
    self.BtnObj:SetActive(not r_none)
    local item_name = r_none and Language.Bicycle.Text10 or self.stuff_data:QuaName()
    UH.SetText(self.ItemName, item_name)
    local cur_num = r_none and 0 or 1
    UH.SetText(self.NumText, DataHelper.ConsumeNum(cur_num, 1, true))
    if r_none then
        self.RInfo:SetData({})
    else
        local skill_info = self.data:GetItemSkillInfo(self.stuff_data.item_id)
        local new_level = self.l_skill:Level() > skill_info.skill_level and self.l_skill:Level() or skill_info.skill_level
        local r_skill = BicycleSkill.New({skill_id = skill_info.skill_id, level = new_level, skill_type = BicycleCfg.SKILL_TYPE.BASE})
        self.RInfo:SetData(r_skill)
        self.Stuff:SetData(self.stuff_data)
    end
   
end

function BicycleSkillConvertView:OnAddClick()
    if not self.l_skill then
        return
    end
    local ok_func = function (item)
        self.stuff_data = item
        self:FlushPanel()
    end
    local skill_hole = self.l_skill.index
    local check_func = function (item)
        local skill_info = self.data:GetItemSkillInfo(item.item_id)
        if skill_info.skill_id == self.l_skill.skill_id then--and skill_info.skill_level == self.l_skill:Level() then
            return false
        end
        return true
    end
    ViewMgr:OpenView(BicycleTargetSkillView, {skill_hole = skill_hole, ok_func = ok_func, check_func = check_func})
end

function BicycleSkillConvertView:OnBtnClick()
    if TableIsEmpty(self.stuff_data) then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Text10)
        return
    end
    if not self.l_skill then
        return
    end
    local bicycle = BicycleData.Instance:GetBicycle(self.l_skill.bicycle_id)
    local col_type = self.stuff_data.column_type
    local bag_index = self.stuff_data.index
    
    local cfg = self.stuff_data:Config()
    local new_level = self.l_skill:Level() > cfg.param3 and self.l_skill:Level() or cfg.param3
    local tip = string.format(Language.Bicycle.SkillChangeTip, cfg.param3, self.stuff_data:QuaName(), self.l_skill:Level(), self.l_skill:QuaName(), new_level, self.stuff_data:QuaName())
    PublicPopupCtrl.Instance:AlertTip(tip, function ()
        BicycleCtrl.Instance:ReqSkill(BicycleCfg.OP_TYPE_SKILL.SKILL_REPLACE, bicycle:Index(), self.l_skill.index, col_type, bag_index)
        ViewMgr:CloseView(BicycleSkillConvertView)
    end)
end


BicycleSkillConvertSkillInfo = BicycleSkillConvertSkillInfo or DeclareMono("BicycleSkillConvertSkillInfo", UIWidgetBaseItem)
function BicycleSkillConvertSkillInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_none = TableIsEmpty(data)
    self.Show:SetActive(not is_none)
    self.None:SetActive(is_none)
    if not is_none then
        self.SkillCell:SetData(data)
        UH.SetText(self.SkillDesc, data:Desc())
    end
end