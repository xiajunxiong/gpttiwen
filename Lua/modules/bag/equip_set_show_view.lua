EquipSetShowView = EquipSetShowView or DeclareView("EquipSetShowView", "bag/equip_set_show")
VIEW_DECLARE_MASK(EquipSetShowView,ViewMask.BgBlockClose)
function EquipSetShowView:EquipSetShowView()
    self.AudioOpen = CommonAudio.GetDiamond
end 

function EquipSetShowView:LoadCallback(param_t)
    self.item_data = param_t.item_data
    self.EffectTool:Play("30000816")

    EquipSetData.Instance:CheckRelease()
end 

function EquipSetShowView:CloseCallback()
    WorkshopData.Instance:UnMarkDrawOffEquip()
end

function EquipSetShowView:OnFlush()
    self.EquipListObj:SetActive(self.item_data:IsEquip())    
    self.JewerlyListObj:SetActive(self.item_data:IsJewelry())

    local num = 0
    if self.item_data:IsEquip() then 
        local equip_list = BagData.Instance:GetEquipList()
        for i = 0,4 do 
            local flag = equip_list[i] and equip_list[i]:GetEquipSet() == self.item_data:GetEquipSet()
            self.Equiplist[i + 1]:SetData(flag and equip_list[i] or Item.New())
            if flag then 
                num = num + 1
            end 
        end 
    elseif self.item_data:IsJewelry() then 
        local jewelry_list = BagData.Instance:GetJewelryList()
        for i = 0,2 do 
            local flag = jewelry_list[i] and jewelry_list[i]:GetEquipSet() == self.item_data:GetEquipSet()
            self.JewerlyList[i + 1]:SetData(flag and jewelry_list[i] or Item.New())
            if flag then 
                num = num + 1
            end 
        end 
    end 
    
    local list = EquipSetData.Instance:GetItemInfoEquipSetShow(self.item_data:GetEquipSet(),true,self.item_data:Color())
    self.EquipSetList:SetData(list)
    local cfg = EquipSetData.Instance:FixSuitConfig(EquipSetData.Instance:CatchEquipSetConfig(self.item_data:GetEquipSet()))
    for k,v in pairs(cfg) do 
        if v.suit_compose_1 == num then 
            self.EquipSetList:JumpVerticalIndex(k,50)
        end 
    end 
end

