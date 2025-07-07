RoleAngrySoulPanel = RoleAngrySoulPanel or DeclareMono("RoleAngrySoulPanel", UIWFlushPanel)
function RoleAngrySoulPanel:RoleAngrySoulPanel()
    self.data = RoleAngrySoulData.Instance

    self.data_cares = {
        {data = self.data.nu_hun_info, func = self.FlushPanel },
        {data = self.data.nu_hun_info, func = self.FlushTogglePanel },
    }

    self.show_type = nil
    self.show_param = nil
end 

function RoleAngrySoulPanel:Awake()
    UIWFlushPanel.Awake(self)
    -- self:FlushTogglePanel()
end

function RoleAngrySoulPanel:onFlush()
    -- LogError("?fklush!")
end 

function RoleAngrySoulPanel:FlushPanel()
    if self.show_type == nil then 
        return 
    end 
    local show_param = self.data:GetAngrySoulDetail(self.show_type)
    self.show_param = show_param
    UH.SetText(self.SoulName, show_param.name .. "["..show_param.level..Language.Common.Level.."]")
    -- UH.SetText(self.SoulLevel, "Lv."..show_param.level)
    self.AttrList:SetData(show_param.cur_attr)
    UH.SetText(self.SoulSpEff,show_param.sp_show)

    local stuff_item = Item.Create({item_id = show_param.item_id})
    self.StuffItem:SetData(stuff_item)
    UH.SetText(self.StuffName,stuff_item:Name())
    local num = stuff_item:Num()
    local color = num >= show_param.num and COLORSTR.Green9 or COLORSTR.Red20
    UH.SetText(self.StuffNum,string.format(Language.Common.Cstr,color,num,show_param.num))

    self.LevelMax:SetActive(show_param.is_level_max)
    self.LevelNext:SetActive(not show_param.is_level_max)

    self.NextAttrList.gameObject:SetActive(not show_param.is_level_max)
    self.NextSoulSpEff.gameObject:SetActive(not show_param.is_level_max)
    if not show_param.is_level_max then
        self.NextAttrList:SetData(show_param.next_attr)
        UH.SetText(self.NextSoulSpEff,show_param.next_sp_show)    
    end

    UH.SetText(self.BtnText,show_param.oper_str)

end

function RoleAngrySoulPanel:FlushTogglePanel()
    local toggles = self.data:GetTypeGroups()
    for i = 1, 3 do
        self.ToggleShow[i]:SetData(toggles[i])
    end

    if self.show_type == nil then
        self:OnClickSkillShow(toggles[1])
    end
    -- 
end

function RoleAngrySoulPanel:OnClickSkillShow(data)
    self.show_type = data.index

    self:FlushPanel()
end

function RoleAngrySoulPanel:OnClickPreView()
    ViewMgr:OpenView(AngrySoulEffPreView ,{type = self.show_type-1 })
end

function RoleAngrySoulPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end 

function RoleAngrySoulPanel:OnClickUp()
    local cur_num = Item.GetNum(self.show_param.item_id)
    if cur_num < self.show_param.num then 
        PublicPopupCtrl.Instance:Center(Language.AngrySoul.OperLevelUpFail)
        return 
    end 

    self.data:SetLevelUpMark(self.show_type-1)
    RoleAngrySoulCtrl.Instance:SendReq(0, self.show_type-1)
end

RoleAngrySoulSkillShow = RoleAngrySoulSkillShow or DeclareMono("RoleAngrySoulSkillShow", UIWidgetBaseItem)
function RoleAngrySoulSkillShow:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.SoulLevel,data.level..Language.Common.Level)
    -- UH.SetIcon(self.SoulIcon,Item.GetIconId(data.item_id))
    -- UH.SpriteName(self.SoulIcon,Item.GetIconId(data.item_id))
    self.SoulAct:SetActive(data.level > 0)
    self.RedPoint:SetNum(RoleAngrySoulData.Instance:GetRemindForIndex(data.index - 1))
    UH.SetText(self.SoulName,Item.GetName(data.item_id))
end 

function RoleAngrySoulSkillShow:Click()
    if self.ItemClick then 
        self.ItemClick:Trigger()
    end 
end


RoleAngrySoulAttrItem = RoleAngrySoulAttrItem or DeclareMono("RoleAngrySoulAttrItem", UIWidgetBaseItem)
function RoleAngrySoulAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    
    UH.SetText(self.Type,data.type)
    UH.SetText(self.Value,data.value)
end 

