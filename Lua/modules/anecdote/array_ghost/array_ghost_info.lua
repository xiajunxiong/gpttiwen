
ArrayGhostGrid = ArrayGhostGrid or BaseClass()
function ArrayGhostGrid:__init(rect)
    self:ResetRect(rect)
end

function ArrayGhostGrid:SetGridInfo(info, index)
    self.grid_info = info
    if nil == info then
        self.grid_type = ArrayGhostConfig.GridType.None
        self.res_id = 0
    else
        self.grid_type = info.product_type
        self.res_id = info.res_id
    end
    self.index = index
    self.show_obj = nil
end

function ArrayGhostGrid:ResetGrid()
end

function ArrayGhostGrid:ResetRect(rect)
    self.item_side = rect.item_side
    self.ag_data = rect.data
end

function ArrayGhostGrid:GridType()
    return self.grid_type
end

function ArrayGhostGrid:CanMove(pos)
    local can_move = false
    if ArrayGhostConfig.GridType.Box == self.grid_type then
        local value, pos2 = self:GetPos()
        if value and GameMath.GetDistance(pos.x, pos.z, pos2.x, pos2.z) < 7 then
            can_move = true
            self.ag_data:SetArrowInfo(self.index, pos2.x - pos.x, pos2.z - pos.z)
        end
    end
    return can_move
end

function ArrayGhostGrid:ResId()
    return self.res_id or 0
    -- return 1033
end

function ArrayGhostGrid:SetShowObj(obj, point1)
    self.show_obj = obj
    obj.localPosition = Vector3.New(self.item_side * (math.floor((self.index - 1) / self.ag_data:CoWideNum())), 0 ,self.item_side * ((self.index - 1) % self.ag_data:CoWideNum()))
    obj.name = self.grid_info and self.grid_info.map_point or self.index
    obj.gameObject:SetActive(true)
end

function ArrayGhostGrid:GetPos()
    if nil ~= self.show_obj then
        return true, self.show_obj.position
    end
    return false
end

function ArrayGhostGrid:GetTrans()
    if nil ~= self.show_obj then
        return true, self.show_obj
    end
    return false
end

function ArrayGhostGrid:ToGridType()
    return ArrayGhostConfig.ToGirdType[self.grid_type]
end

function ArrayGhostGrid:IsGridType(grid_type)
    return grid_type == self.grid_type
end
