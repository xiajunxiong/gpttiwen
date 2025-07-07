-- ============= 星图属性总览
StarMapAttriView = StarMapAttriView or DeclareView("StarMapAttriView", "star_map/star_map_attri_view")
function StarMapAttriView:StarMapAttriView()
    self.data = StarMapData.Instance
    self.tab_data = {
        {map_type = StarMapCtrl.MapType.ROLE},
        {map_type = StarMapCtrl.MapType.PET},
        {map_type = StarMapCtrl.MapType.PARTNER},
    }
    self.t_type = StarMapCtrl.MapType.ROLE  --目标类型
end

-- star_type ==== StarMapCtrl.MapType
function StarMapAttriView:LoadCallback(param_t)
    local map_type = param_t.map_type or StarMapCtrl.MapType.Super
    self.map_type = map_type
    local title = string.format(Language.StarMap.Text5, Language.StarMap.StarMapType[map_type])
    self.Board:SetData(self:GetType(), title ,Vector2.New(752, 540))

    self:FlushPanel()
    if map_type == StarMapCtrl.MapType.Super then
        self.TabList:SetData(self.tab_data)
        self.TabList:SetectItem(self.t_type + 1, true)
    end
end

function StarMapAttriView:FlushPanel()
    local map_type = self.map_type
    local title = string.format(Language.StarMap.Text5, Language.StarMap.StarMapType[map_type])
    self.Board:ChangeTitle(title)
    local attri_list
    local skill_list
    local title2
    if map_type == StarMapCtrl.MapType.Super then
        attri_list,skill_list = self.data:GetSuperMapAttriAndSkillList(self.t_type)
        title2 = Language.StarMap.Text7
    else
        attri_list,skill_list = self.data:GetMapAttriAndSkillList(map_type)
        title2 = Language.StarMap.Text8
    end
    local group_list_data = {}
    for i = 1, #attri_list do
        local page = math.ceil(i / 3)
        if not group_list_data[page] then
            group_list_data[page] = {index = page, attri_list = {}}
        end
        table.insert(group_list_data[page].attri_list, attri_list[i])
    end
    if #group_list_data < 4 and #group_list_data > 0 then
        for i = #group_list_data + 1, 4 do
            table.insert(group_list_data, {index = i})
        end
    end
    self.AttriGroupList:SetData(group_list_data)
    self.SkillList:SetData(skill_list)
    UH.SetText(self.Title2, title2)
    if TableIsEmpty(skill_list) then
        local tip = map_type == StarMapCtrl.MapType.Super and Language.StarMap.Text9 or Language.StarMap.Text10
        UH.SetText(self.NoneTip, tip)
    else
        UH.SetText(self.NoneTip, "")
        UH.SetText(self.NoneTip2, "")
    end
    local none_tip2 = TableIsEmpty(attri_list) and Language.StarMap.Text11 or ""
    UH.SetText(self.NoneTip2, none_tip2)
end

function StarMapAttriView:OnTabClick(data)
    self.t_type = data.map_type
    self:FlushPanel()
end

StarMapAttriTabItem = StarMapAttriTabItem or DeclareMono("StarMapAttriTabItem", UIWidgetBaseItem)
function StarMapAttriTabItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local text = Language.StarMap.StarMapType[data.map_type] .. Language.StarMap.Text12
    UH.SetText(self.Text, text)
end

StarMapAttriGroupItem = StarMapAttriGroupItem or DeclareMono("StarMapAttriGroupItem", UIWidgetBaseItem)
function StarMapAttriGroupItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Image.enabled = data.index % 2 == 0
    self.List:SetData(data.attri_list or {})
end