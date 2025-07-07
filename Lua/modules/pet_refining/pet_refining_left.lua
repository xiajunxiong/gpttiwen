PetRefiningLeft = PetRefiningLeft or DeclareView('PetRefiningLeft', 'pet_refining/pet_refining_left')
VIEW_DECLARE_MASK(PetRefiningLeft, ViewMask.None)
PetRefiningLeftPanel = PetRefiningLeftPanel or DeclareMono('PetRefiningLeftPanel', UIWFlushPanel)
function PetRefiningLeftPanel:PetRefiningLeftPanel()
    self.data = PetRefiningData.Instance
    self.data_cares = {
        {data = self.data:ViewData(), func = self.StoveChange, keys = {'stove_idx'}, init_call = false},
        {data = self.data:ViewData(), func = self.ReSet, keys = {'select_left'}, init_call = false}
    }
    self.select_pet = nil
end
function PetRefiningLeftPanel:ReSet()
    self.Anima:Play('zhankai')
    self.data:SetPet1(nil)
    self:FlushList()
end
function PetRefiningLeftPanel:StoveChange()
    self.data:SetPet1(nil)
    ViewMgr:CloseView(PetRefiningLeft)
end
function PetRefiningLeftPanel:OnClickClose()
    self.data:SetPet1(nil)
    ViewMgr:CloseView(PetRefiningLeft)
end
function PetRefiningLeftPanel:OnClickSet()
    --判断是否是同类型的宠物，不可炼妖
    local pet2 = self.data:Pet2()
    if pet2 ~= nil and pet2:NormalId() == self.select_pet:NormalId() then
        PublicPopupCtrl.Instance:Center(Language.PetRefining.SamePetTip)
        return
    end
    if self.select_pet:IsLianYao() then
        local info = {
            content = Language.PetRefining.Tip2,
            confirm = {
                func = function()
                    self.data:SetPet1(self.select_pet)
                    self.Anima:Play('shousuo')
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end
    --设置pet1
    self.data:SetPet1(self.select_pet)
    self.Anima:Play('shousuo')
    --播放收縮動畫
end
function PetRefiningLeftPanel:FlushList()
    -- local pets = PetData.Instance:GetPetList()
    local pets = self.data:MainPetList()
    --筛选宠物列表
    self.PetList:SetData(pets)
end
function PetRefiningLeftPanel:onFlush()
    self.data:SetPet1(nil)
    self:FlushList()
    self.PetList:ClickItem(1)
end
function PetRefiningLeftPanel:PetIsNil()
    self.PetList:SetData({})
    self.List1:SetData({})
    self.List2:SetData({})
    for i = 1, self.ValueTexts:Length() do
        UH.SetText(self.ValueTexts[i], '-/-')
    end
end
function PetRefiningLeftPanel:OnClickSelect(cell)
    local pet = cell:GetData()
    self.select_pet = pet
    local main_skill = pet:GetMainSkillList({})
    local skills = pet:GetLearnedGiftList({})
    skills = pet:LianYaoSkillList(skills)
    if #skills <= 4 then
        self.List2:SetData(skills)
        self.List3:SetData({})
    else
        local list = {}
        local list2 = {}
        --每5个一组
        local l2idx = 1
        for i = 1, #skills do
            if i <= 4 then
                table.insert(list, skills[i])
            else
                l2idx = math.max(math.floor((i - 4) / 5), l2idx)
                list2[l2idx] = list2[l2idx] or {}
                table.insert(list2[l2idx], skills[i])
            end
        end
        self.List2:SetData(list)
        self.List3:SetData(list2)
    end
    self.List1:SetData(main_skill)
    local config = pet:ConfigGrowthList()
    local total = pet:TotalGrowthList()
    local index = 1
    for key, value in pairs(config) do
        index = PetData.GetZZIndex(key)
        UH.SetText(self.ValueTexts[index], string.format('%s/%s', total[key], value))
    end
end
function PetRefiningLeftPanel:OnClickGift(cell)
    PetGiftItem.CommonClickFunc(cell)
end

PetRefiningLeftItem = PetRefiningLeftItem or DeclareMono('PetRefiningLeftItem', UIWidgetBaseItem)
function PetRefiningLeftItem:PetRefiningLeftItem()
end
function PetRefiningLeftItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetDataByPet(data)
end
function PetRefiningLeftItem:Click()
    self.EventClick:Trigger()
    self.Selector.isOn = true
end
