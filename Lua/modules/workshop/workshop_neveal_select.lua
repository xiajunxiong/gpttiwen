WorkshopNevealSelect = WorkshopNevealSelect or DeclareView("WorkshopNevealSelect", "workshop/workshop_neveal_select")
function WorkshopNevealSelect:WorkshopNevealSelect() end

WorkshopNevealSelectPanel = WorkshopNevealSelectPanel or DeclareView("WorkshopNevealSelectPanel", "workshop/workshop_neveal_select")
function WorkshopNevealSelectPanel:WorkshopNevealSelectPanel() 
    local data = BagData.Instance:GetGridByType({ItemType.Neveal})

    local res_list = {}
    for k,v in pairs(data) do 
        local item = Item.Create(v)
        local info = item.Neveal ~= nil and item:Neveal() or nil
        if info ~= nil then 
            local cfg = WorkshopData.Instance:GetNevealCfg(info.effect_id,info.effect_level)
            if v:IsNeveal() and cfg ~= nil then 
                table.insert(res_list,v)
            end 
        end 
    end 
    self.select_list:SetData(res_list)
end

function WorkshopNevealSelectPanel:OnClickPopClose()
    ViewMgr:CloseView(WorkshopNevealSelect)
end

function WorkshopNevealSelectPanel:OnClickSelect()
    local data = self.select_list:GetSelectedData()
    WorkshopData.Instance:SetNevealStoneSelectStone(data[1])

    self:OnClickPopClose()
end