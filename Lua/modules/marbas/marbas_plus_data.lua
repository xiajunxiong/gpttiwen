function MarbasData:GetEquipGetWay() 
    local get_way =  Config.zaohua_equip_up_auto.other[1].fast_get

    local list = {}
    local cell = BagData.Instance:GetWayCfg(get_way)
    table.insert(list,cell)
    return list
end 

-- 预先选中项
function MarbasData:SetPreGemResolveItem(item)
    self.pre_resolve_gem_item = item
end


function MarbasData:GetPreGemResolveItem()
    return self.pre_resolve_gem_item
end

function MarbasData:GetCanGemResolveItem()
    local col_grids = BagData.Instance.item_grid[ItemColumnType.Item]
    local new_list = {}
    local types = {ItemType.MarbasGem}
    for k, v in pairs(col_grids) do
        for k2, v2 in pairs(types) do
            if v.item_type == v2 then
                table.insert(new_list, v)
            end
        end
    end
    return new_list
end

function MarbasData:GetGemResolveList(gem_list)
    -- local return_list = {}
    local return_money = 0
    for k,v in pairs(gem_list) do 
        local decompose_cfg = self:GetDeComposeCfgByTarget(v.item_id)
        return_money = return_money + decompose_cfg.return_money_num
    end 

    return return_money
end