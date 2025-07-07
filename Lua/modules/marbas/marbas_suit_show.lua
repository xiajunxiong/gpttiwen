-- 造化装备·套装展示
MarbasSuitShowView = MarbasSuitShowView or DeclareView("MarbasSuitShowView", "marbas/marbas_suit_show")
VIEW_DECLARE_MASK(MarbasSuitShowView,ViewMask.BgBlockClose)
function MarbasSuitShowView:MarbasSuitShowView()
    self.data = MarbasData.Instance
end

function MarbasSuitShowView:LoadCallback()

    self.show_info = self.data:TypeSuitShowGetInfo()
    self.EffectTool:Play("30000816")

    if self.show_info == nil then 
        ViewMgr:CloseView(MarbasSuitShowView)
    end 
end 

function MarbasSuitShowView:CloseCallback()
    -- WorkshopData.Instance:UnMarkDrawOffEquip()
    self.data:ClearSuitEnableChanges()
end

function MarbasSuitShowView:OnFlush()
    if self.show_info == nil then return end 

    self.Suit3:SetActive(self.show_info.suit_num == 3)
    self.Suit5:SetActive(self.show_info.suit_num == 5)
    self.Suit8:SetActive(self.show_info.suit_num == 8)

    if self["suit_"..self.show_info.suit_num] ~= nil then
        for i = 1,self["suit_"..self.show_info.suit_num]:Length() do 
            if self.show_info.cell_list[i] ~= nil then 
               self["suit_"..self.show_info.suit_num][i]:SetData(self.show_info.cell_list[i])
            end  
        end 
    end 

    local list = MarbasData.Instance:GetMarbasSuitEffingInSuitShow()
    self.EquipSetList:SetData(list)
    for k,v in pairs(list) do 
        if self.show_info.mark.suit_num >= v.suit_num and self.show_info.mark.color >= v.color then 
            self.EquipSetList:JumpVerticalIndex(k,50)
            break
        end 
    end 

end

MarbasSuitShowCell= MarbasSuitShowCell or DeclareMono("MarbasSuitShowCell",UIWidgetBaseItem)
function MarbasSuitShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.Cell:SetData(data)

    UH.SetText(self.LevelText, data:LevelShow()..Language.Common.Level)
end