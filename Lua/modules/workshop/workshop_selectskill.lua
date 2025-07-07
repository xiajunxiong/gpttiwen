WorkshopSelectSkill = WorkshopSelectSkill or DeclareView("WorkshopSelectSkill", "workshop/workshop_selectskill")
VIEW_DECLARE_MASK(WorkshopSelectSkill, ViewMask.Block)
function WorkshopSelectSkill:WorkshopSelectSkill()
    -- body
end
function WorkshopSelectSkill:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Workshop.SelectSkill)
    WorkshopData.Instance:ClearSkillSelect()
end
function WorkshopSelectSkill:OnFlush()
    local data = WorkshopData.Instance:SkillShowList()
    self.List:SetData(data)
end
function WorkshopSelectSkill:OnClickClose()
    ViewMgr:CloseView(WorkshopSelectSkill)
end


WorkshopSelectSkillItem = WorkshopSelectSkillItem or DeclareMono("WorkshopSelectSkillItem", UIWidgetBaseItem)
function WorkshopSelectSkillItem:WorkshopSelectSkillItem()
    self.result = 0
    self.ws_data = WorkshopData.Instance
end
function WorkshopSelectSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetDataByItemIdWithZeroNum(data.item_id)
    UH.SetText(self.Name, self.ItemCell:GetData():QuaName())
    UH.SetText(self.Num, self.result)
    local max = Item.GetNum(self.data.item_id)
    self.Inter.Interactable = max ~= 0
end
function WorkshopSelectSkillItem:OnClickAdd(num)
    local total_num = self.ws_data:SkillBookData().total_num
    local max_num = self.ws_data:SkillSelectMax()
    if total_num <= max_num then
        total_num = total_num - self.result
    end
    local max = Item.GetNum(self.data.item_id)
    self.result = self.result + num
    self.result = self.result < 0 and 0 or self.result
    self.result = self.result > max and max or self.result
    total_num = total_num + self.result
    if total_num > max_num then
        self.result = self.result - num
        return
    end
    UH.SetText(self.Num, self.result)
    self.ws_data:SkillBookData().total_num = total_num
    self.ws_data:SkillSelectList()[self.data.item_id] = {item_id = self.data.item_id, num = self.result}
end
