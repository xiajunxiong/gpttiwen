-- ========== 灵骑卷抽材料选择 ==========

BicycleSelSkillStuffView = BicycleSelSkillStuffView or DeclareView("BicycleSelSkillStuffView", "bicycle/new_skill/bicycle_sel_skill_stuff_view")
BicycleSelSkillStuffView.ShowType = {
    ComposeA = 1,   -- 合成卷轴操作，选择主卷
    ComposeB = 2,   -- 合成卷轴操作，选择副卷
    SkillUp = 3,    -- 灵技升级
}
function BicycleSelSkillStuffView:BicycleSelSkillStuffView()
    self.data = BicycleOtherData.Instance
end

function BicycleSelSkillStuffView:LoadCallback(param_t)
    local show_type = param_t.show_type or BicycleSelSkillStuffView.ShowType.ComposeA
    self.show_type = show_type
    local list_data = {}
    local tip = ""
    local title = ""
    if show_type == BicycleSelSkillStuffView.ShowType.ComposeA then
        local item_list = self.data:GetAllSkillBook()
        for i,v in pairs(item_list) do
            local cfg = v:Config()
            local skill_id = cfg.param1
            local level = cfg.param3
            local max_level = BicycleData.Instance:GetSkillMaxLevel(skill_id)
            if level < max_level then
                table.insert(list_data, {item = v, level = level})
            end
        end
        table.sort(list_data, function (a,b)
            return a.level > b.level
        end)
        tip = Language.Bicycle.Text1
        title = Language.Bicycle.Title1
    elseif show_type == BicycleSelSkillStuffView.ShowType.ComposeB then
        local a_stuff = self.data:GetAStuff()
        local a_cfg = a_stuff:Config()
        if not TableIsEmpty(a_stuff) then
            local item_list = self.data:GetAllSkillBook()
            for i,v in pairs(item_list) do
                local cfg = v:Config()
                if v:Color() == a_stuff:Color() and cfg.param3 == a_cfg.param3 then
                    if v == a_stuff then
                        local has_num = v:Num()--Item.GetNum(v.item_id)
                        if has_num > 1 then
                            local vo = {item_id = v.item_id, index = v.index, is_bind = v.is_bind, column_type = v.column_type}
                            vo.num = has_num - 1
                            local new_item = Item.Create(vo)
                            table.insert(list_data, {item = new_item})
                        end
                    else
                        table.insert(list_data, {item = v})
                    end
                end
            end
        end
        tip = Language.Bicycle.Text1
        title = Language.Bicycle.Title3
    elseif show_type == BicycleSelSkillStuffView.ShowType.SkillUp then
        local t_qua = param_t.t_qua
        local t_level = param_t.t_level
        self.ok_func = param_t.ok_func
        local item_list = self.data:GetAllSkillBook()
        for i,v in pairs(item_list) do
            local cfg = v:Config()
            if v:Color() == t_qua and cfg.param3 <= t_level then
                table.insert(list_data, {item = v,
                is_lock = cfg.param3 < t_level,
                lock_tip_2 = Language.Bicycle.Tip1,
                level = cfg.param3})
            end
        end
        local level_str = ColorStr(t_level + 1, COLORSTR.Green10)
        tip = string.format(Language.Bicycle.Tip2,level_str)
        table.sort(list_data, function (a,b)
            local a_num = not a.is_lock and 10000 or 0
            local b_num = not b.is_lock and 10000 or 0
            if a_num == b_num then
                return a.level < b.level
            end
            return a_num > b_num
        end)
        title = Language.Bicycle.Title4
    end
    self.Show:SetActive(not TableIsEmpty(list_data))
    self.None:SetActive(TableIsEmpty(list_data))
    if not TableIsEmpty(list_data) then
        self.List:SetData(list_data)
    end
    UH.SetText(self.Tip, tip)
    self.Board:SetData(self:GetType(),title,Vector2.New(636, 560))
end

function BicycleSelSkillStuffView:OnCloseCallback()

end

function BicycleSelSkillStuffView:OnItemSelClick(data)
    self.sel_item = data.item
end

function BicycleSelSkillStuffView:OnOkClick()
    local show_type = self.show_type
    if self.sel_item then
        if show_type == BicycleSelSkillStuffView.ShowType.ComposeA then
            self.data:SetAStuff(self.sel_item)
            self.data:SetBStuff(nil)
        elseif show_type == BicycleSelSkillStuffView.ShowType.ComposeB then
            self.data:SetBStuff(self.sel_item)
        elseif show_type == BicycleSelSkillStuffView.ShowType.SkillUp then
            if self.ok_func then
                self.ok_func(self.sel_item)
            end
        end
    end
    ViewMgr:CloseView(BicycleSelSkillStuffView)
end

function BicycleSelSkillStuffView:OnGetClick()
    ViewMgr:OpenViewByKey(Mod.FightFieldBoss.Shxy)
end



BicycleSelSkillStuffItem = BicycleSelSkillStuffItem or DeclareMono("BicycleSelSkillStuffItem", UIWidgetBaseItem)
function BicycleSelSkillStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = data.item
    self.ItemCell:SetData(item)
    UH.SetText(self.Name, item:Name())
    local is_lock = data.is_lock or false
    self.Lock:SetActive(is_lock)
    if is_lock then
        UH.SetText(self.LockTip, data.lock_tip or Language.Bicycle.Tip3)
    end
end

function BicycleSelSkillStuffItem:OnLockClick()
    if self.data.lock_tip_2 then
        PublicPopupCtrl.Instance:Error(self.data.lock_tip_2)
    end
end