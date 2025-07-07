RuinsAchi = RuinsAchi or DeclareView("RuinsAchi", "god_areas_ruins/ruins_achi")

function RuinsAchi:RuinsAchi()
    self.data = RuinsData.Instance
    self.Board:SetData(self:GetType(), Language.GodAreasActs.Achieve)
    self.handle_care = self.data.achi_data:Care(BindTool.Bind(self.FlushList, self))
end
function RuinsAchi:LoadCallback(param_t)
    local data = self.data:AchievementInfo()
    self:SortAchi(data)
   
    self.List:SetData(data)
end
function RuinsAchi:SortAchi(data)
    local value = 0
    local is_get = false
    for k, v in ipairs(data) do
        v.sort_num = 0
        value = self.data:GetAchiValue(v.achieve_type)
        is_get = self.data:AchiIsGet(v.achieve_type, v.ach_level)
        if not is_get and value >= v.ach_param1 then
            v.sort_num = 10000
        end
        if is_get then
            v.sort_num = -1
        end
    end
    table.sort(data, DataSorter.KeyUpperSorter("sort_num"))
end
function RuinsAchi:FlushList()
    local data = self.data:AchievementInfo()
    self:SortAchi(data)
    self.List:SetData(data)
end
function RuinsAchi:CloseCallback()
    self.data.achi_data:Uncare(self.handle_care)
end

RuinsAchiItem = RuinsAchiItem or DeclareMono("RuinsAchiItem", UIWidgetBaseItem)
function RuinsAchiItem:RuinsAchiItem()
    self.ruins_data = RuinsData.Instance
end

function RuinsAchiItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Title, data.ach_name)
    UH.SetText(self.Desc, data.ach_dsc)
    local value = self.ruins_data:GetAchiValue(data.achieve_type)
    local item = Item.Create(data.achieve_reward[0])
    self.Cell:SetData(item)
    UH.SetText(self.JinDu, string.format("%s/%s", value, data.ach_param1))
    self.BarImg.fillAmount = value / data.ach_param1
    local is_get = self.ruins_data:AchiIsGet(self.data.achieve_type, self.data.ach_level)
    local is_can_get = (not is_get and self.BarImg.fillAmount >= 1)
    self.GetEffect:SetActive(is_can_get)
    self.GetBlock:SetActive(is_can_get)
    self.IsGetObj:SetActive(is_get)
end
function RuinsAchiItem:OnClickGetReward()
    RuinsCtrl.Instance:ReqAchiReward(self.data.achieve_type, self.data.ach_level)
end