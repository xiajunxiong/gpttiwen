PetRefiningRight = PetRefiningRight or DeclareView("PetRefiningRight", "pet_refining/pet_refining_right")
VIEW_DECLARE_MASK(PetRefiningRight, ViewMask.None)
PetRefiningRightPanel = PetRefiningRightPanel or DeclareMono("PetRefiningRightPanel", UIWFlushPanel)
function PetRefiningRightPanel:PetRefiningRightPanel()
    self.data = PetRefiningData.Instance
    self.data_cares = {
        {data = self.data:ViewData(), func = self.StoveChange, keys = {"stove_idx"}, init_call = false},
        {data = self.data:ViewData(), func = self.ReSet, keys = {"select_right"}, init_call = false}
    }
    self.select_pet = nil
end
function PetRefiningRightPanel:ReSet()
    self.Anima:Play("zhankai")
    self.data:SetPet2(nil)
    self:FlushList()
end
function PetRefiningRightPanel:StoveChange()
    self.data:SetPet2(nil)
    ViewMgr:CloseView(PetRefiningRight)
end
function PetRefiningRightPanel:OnClickClose()
    self.data:SetPet2(nil)
    ViewMgr:CloseView(PetRefiningRight)
end
function PetRefiningRightPanel:OnClickSet()
    local pet1 = self.data:Pet1()
    if pet1 ~= nil and pet1:NormalId() == self.select_pet:NormalId() then
        PublicPopupCtrl.Instance:Center(Language.PetRefining.SamePetTip)
        return
    end
    if self.select_pet:IsLianYao() then
        local info = {
            content = Language.PetRefining.Tip1,
            confirm = {
                func = function()
                    self.data:SetPet2(self.select_pet)
                    self.Anima:Play("shousuo")
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end
    --设置Pet2
    self.data:SetPet2(self.select_pet)
    self.Anima:Play("shousuo")
end
function PetRefiningRightPanel:FlushList()
    -- local pets = PetData.Instance:GetPetList()
    local pets = self.data:SecPetList()
    self.PetList:SetData(pets)
end
function PetRefiningRightPanel:onFlush()
    self.data:SetPet2(nil)
    self:FlushList()
    self.PetList:ClickItem(1)
end
function PetRefiningRightPanel:PetIsNil()
    self.PetList:SetData({})
    self.List1:SetData({})
    self.List2:SetData({})
    for i = 1, self.ValueTexts:Length() do
        UH.SetText(self.ValueTexts[i], "-/-")
    end
end
function PetRefiningRightPanel:OnClickSelect(cell)
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
        local list2 = {}--每5个一组
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
        UH.SetText(self.ValueTexts[index], string.format("%s/%s", total[key], value))
    end
end
function PetRefiningRightPanel:OnClickGift(cell)
    PetGiftItem.CommonClickFunc(cell)
end
PetRefiningRightItem = PetRefiningRightItem or DeclareMono("PetRefiningRightItem", UIWidgetBaseItem)
function PetRefiningRightItem:PetRefiningRightItem()
    
end
function PetRefiningRightItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetDataByPet(data)
end

function PetRefiningRightItem:Click()
    self.EventClick:Trigger()
    self.Selector.isOn = true
end