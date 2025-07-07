-- 选择目标技能
BicycleTargetSkillView = BicycleTargetSkillView or DeclareView("BicycleTargetSkillView", "bicycle/new_skill/bicycle_target_skill_view")
function BicycleTargetSkillView:BicycleTargetSkillView()
    self.data = BicycleOtherData.Instance
    self.Board:SetData(self:GetType(), Language.Bicycle.TargetSkill, Vector2.New(636, 560))
end

function BicycleTargetSkillView:LoadCallback(param_t)
    --LogError("param_t", param_t)
    local skill_hole = param_t.skill_hole
    local check_func = param_t.check_func
    self.ok_func = param_t.ok_func
    local item_list = self.data:GetAllSkillBook()
    local list_data = {}
    for i,v in pairs(item_list) do
        local skill_info = self.data:GetItemSkillInfo(v.item_id)
        if skill_info.skill_hole == skill_hole then
            if check_func then
                if check_func(v) then
                    table.insert(list_data, v)
                end
            else
                table.insert(list_data, v)
            end
            
        end
    end

    table.sort(list_data, function (a,b)
        local a_skill_info = self.data:GetItemSkillInfo(a.item_id)
        local b_skill_info = self.data:GetItemSkillInfo(b.item_id)
        return a_skill_info.skill_level > b_skill_info.skill_level
    end)
    
    local is_none = TableIsEmpty(list_data)
    self.None:SetActive(is_none)
    self.Show:SetActive(not is_none)
    if not is_none then
        self.List:SetData(list_data)
        self.List:SetectItem(1, true)
    else
        local getway = string.split(self.data.cfg.other[1].scroll_get_way,"|")
        if next(getway) ~= nil and tonumber(getway[1]) then
            self.GetWayList:SetData(getway)
        else
            BuglyReport("灵骑目标技能获取途径配置错误")
        end
    end
end

function BicycleTargetSkillView:OnSelClick(data)
    self.sel_data = data
end

function BicycleTargetSkillView:OnBtnClick()
    if self.ok_func and self.sel_data then
        self.ok_func(self.sel_data)
        ViewMgr:CloseView(BicycleTargetSkillView)
    end
end

function BicycleTargetSkillView:OnGetClick()
    ViewMgr:OpenViewByKey(Mod.FightFieldBoss.Shxy)
end

----------------------------------------------
BicycleTargetSkillItem = BicycleTargetSkillItem or DeclareMono("BicycleTargetSkillItem", UIWidgetBaseItem)
function BicycleTargetSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(data)
    local skill_info = BicycleOtherData.Instance:GetItemSkillInfo(data.item_id)
    local skill_cfg = BicycleData.Instance:GetSkillEffect(skill_info.skill_id,skill_info.skill_level)
    UH.SetText(self.SkillName, skill_cfg.name)
    UH.SetText(self.Desc, skill_cfg.description)
end

-----------------------------------------------
BicycleTargetSkillGetWayItem = BicycleTargetSkillGetWayItem or DeclareMono("BicycleTargetSkillGetWayItem", GetWayViewItem)
function BicycleTargetSkillGetWayItem:BicycleTargetSkillGetWayItem()
end

function BicycleTargetSkillGetWayItem:OnClick()
    if self.way_cfg then
        local item = self:GetItem()
        if self.sp_func == nil or (self.sp_func ~= nil and self.sp_func(self.way_cfg,item)) then
		    GetWayData.OpenPanel(self.way_cfg,item)
            ViewMgr:CloseView(BicycleTargetSkillView)
        end
	else
        ItemInfoViewFuncItem[self.data.name](self)
	end
end