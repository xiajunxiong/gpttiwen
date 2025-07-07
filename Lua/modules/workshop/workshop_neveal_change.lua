WorkshopNevealChange = WorkshopNevealChange or DeclareView("WorkshopNevealChange", "workshop/workshop_neveal_change")
function WorkshopNevealChange:WorkshopNevealChange()

end
function WorkshopNevealChange:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Workshop.Neveal.Scheme, Vector2.New(756, 422))
end

WorkshopNevealChangePanel = WorkshopNevealChangePanel or DeclareMono("WorkshopNevealChangePanel", UIWFlushPanel)
function WorkshopNevealChangePanel:WorkshopNevealChangePanel()
    self.data = WorkshopData.Instance
    self.data_cares = {
        {data = self.data:GetNevealInfo(), func = self.FlushSchemeList}
    }
end
function WorkshopNevealChangePanel:FlushSchemeList(old_val)
    local equip = self.data:GetNevealSelect()

    if old_val and old_val.select then
        local old_list = old_val.select.param.unlock_plan
        local new_list = equip.param.unlock_plan
        for i = 0, 3 do
            if old_list[i] ~= new_list[i] then
                --播放特效
                self.EffectTool:Play(4164000, self.EffectNodes[i+1])
                break
            end
        end
    end

    self.List:SetData(equip.param.dian_hua_param)
end


WorkshopNevealChangeItem = WorkshopNevealChangeItem or DeclareMono("WorkshopNevealChangeItem", UIWidgetBaseItem)
function WorkshopNevealChangeItem:WorkshopNevealChangeItem()
    self.shop_data = WorkshopData.Instance
end
function WorkshopNevealChangeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Title, Format(Language.Workshop.Neveal.SchemeTitle, DataHelper.GetDaXie(data.index + 1)))
    --检查是否解锁
    local equip = self.shop_data:GetNevealSelect()
    local is_unlock = equip:SchemeUnlock(data.index)
    self.Lock:SetActive(not is_unlock)
    self.Using:SetActive(equip:CurNevealScheme() == data.index)
    self.BtnUse:SetActive(not (equip:CurNevealScheme() == data.index) and is_unlock)
    self.enter_data = equip:Neveal()
    if data.effect_id <= 0 then
        self.Icon:SetObjActive(false)
        self.NoSkill:SetActive(true)
        --启用无特效
        if is_unlock then
            UH.SetText(self.Name, Language.Workshop.Neveal.UnlockUnactive)
        else
            UH.SetText(self.Name, "")
        end
    else
        self.NoSkill:SetActive(false)
        local total_level = BagData.Instance:GetNevealSkillLevel(data.effect_id)
        local skill_cfg = total_level > 0 and
                              WorkshopData.Instance:GetNevealSkillCfgByUnlockLevel(data.effect_id, total_level) or
                              WorkshopData.Instance:GetNevealSkillCfg(data.effect_id)
        local equip_num, equip_total = BagData.Instance:GetNevealSkillNum(data.effect_id)
        self.SkllInter.Interactable = total_level > 0
        UH.SetIcon(self.Icon, skill_cfg.skill_icon)
        UH.SetText(self.Name, total_level > 0 and
            ColorStr(Language.Workshop.Neveal.Lv .. skill_cfg.skill_level .. skill_cfg.skill_name, COLORSTR.Green9) or
            skill_cfg.skill_name .. "(" .. equip_num .. "/" .. equip_total .. ")")
    end
end
function WorkshopNevealChangeItem:OnClickNevealSkillShow()
    local equip = self.shop_data:GetNevealSelect()
    WorkshopData.Instance:SetNevealClickSkill(equip:Neveal())
    ViewMgr:OpenView(WorkshopNevealSkillTip)
end
function WorkshopNevealChangeItem:OnClickUse()
    local equip = self.shop_data:GetNevealSelect()
    WorkshopCtrl.Instance:UseDianHuaScheme(equip.in_bag, equip.index, self.data.index)
end
function WorkshopNevealChangeItem:OnClickUnlock()
    ViewMgr:OpenView(WorkshopNevealUnlock)
end


WorkshopNevealUnlock = WorkshopNevealUnlock or DeclareView("WorkshopNevealUnlock","workshop/workshop_neveal_unlock")
function WorkshopNevealUnlock:WorkshopNevealUnlock()
    self.data = WorkshopData.Instance
end
function WorkshopNevealUnlock:LoadCallback()
    local equip = self.data:GetNevealSelect()
    local list = equip.param.unlock_plan
    for i = 0, 3 do
        if list[i] == 0 then        
            UH.SetText(self.Title, Format(Language.Workshop.Neveal.SchemeTitle, DataHelper.GetDaXie(i + 1)))
            break
        end
    end
    local item =  self.data:NevealUnlockItem()
    self.cost_item = Item.Create(item)
    self.data_cares_item = BagData.Instance:ListenItems({item.item_id}, BindTool.Bind(self.FlushItem, self))
    self:FlushItem()
end
function WorkshopNevealUnlock:FlushItem()
    local item =  self.data:NevealUnlockItem()
    UH.SetIcon(self.Icon, Item.GetIconId(item.item_id))
    local num = Item.GetNum(item.item_id)
    if num >= item.num then
        UH.SetText(self.Count, Format("<color=#".. COLORSTR.Green4 .. ">%s</color>/%s", num, item.num))
    else
        UH.SetText(self.Count, Format("<color=#".. COLORSTR.Red8 .. ">%s</color>/%s", num, item.num))
    end
end
function WorkshopNevealUnlock:CloseCallback()
    BagData.Instance:RemoveListenItems(self.data_cares_item)
end
function WorkshopNevealUnlock:OnClickItem()
    local info = {
		item_list = {self.cost_item},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end
function WorkshopNevealUnlock:OnClickUnlock()
    local item =  self.data:NevealUnlockItem()
    if Item.GetNum(item.item_id) >= item.num then
        ViewMgr:CloseView(WorkshopNevealUnlock)
    end
    local equip = self.data:GetNevealSelect()
    WorkshopCtrl.Instance:UnlockDianHuaScheme(equip.in_bag, equip.index)
end
function WorkshopNevealUnlock:OnClickClose()
    ViewMgr:CloseView(WorkshopNevealUnlock)
end