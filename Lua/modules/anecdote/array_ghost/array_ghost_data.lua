
ArrayGhostData = ArrayGhostData or BaseClass()
function ArrayGhostData:__init()
    if ArrayGhostData.Instance ~= nil then
        Debuger.LogError("[ArrayGhostData] attempt to create singleton twice!")
        return
    end
    ArrayGhostData.Instance = self

    self:ArrayGhostConfig()
    self:ArrayGhostData()
end

function ArrayGhostData:ArrayGhostConfig()
    self.config = Config.array_ghost_auto
    self.config_other = self.config.other[1]
    self.config_map_point = self.config.map_point
end

function ArrayGhostData:ArrayGhostData()
    self.grid_list = {}
    self.goal_list = {}

    for i = 1, self:CoLengthNum() * self:CoWideNum() do
        local grid = ArrayGhostGrid.New({item_side = self:CoItemSide(), data = self})
        self.grid_list[i] = grid
        grid:SetGridInfo(self.config_map_point[i], i)
        if grid:IsGridType(ArrayGhostConfig.GridType.Goal) then
            self.goal_list[i] = true
        end
    end

    self.arrow_info = SmartData.New({
        index = 0,
        x = 0,
        z = 0,
    })
    self.end_info = SmartData.New({
        is_succ = false
    })
    self.page_info = SmartData.New({value = 1})
end

function ArrayGhostData:ResetGrid()
    for i = 1, self:CoLengthNum() * self:CoWideNum() do
        local grid = self.grid_list[i]
        grid:SetGridInfo(self.config_map_point[i], i)
    end
    self.end_info.is_succ = false
    self.is_moving = false
    self.page_info.value = 1
end

function ArrayGhostData:CoItemSide()
    return self.config_other.item_side
end

function ArrayGhostData:CoLengthNum()
    return self.config_other.length_num
end

function ArrayGhostData:CoWideNum()
    return self.config_other.wide_num
end

function ArrayGhostData:SetArrowInfo(index, x, z)
    local ai = self.arrow_info
    local value = math.abs(x) > math.abs(z)
    x = value and (x > 0 and 1 or -1) or 0
    z = value and 0 or (z > 0 and 1 or -1)
    -- 边界
    local w = math.floor((index - 1) / self:CoWideNum()) + 1
    local l = (index - 1) % self:CoWideNum() + 1
    if index > 0 then
        local to_index = index + (x * self:CoWideNum()) + z
        local to_grid = self.grid_list[to_index]
        if to_grid and to_grid:ToGridType() then
            if z > 0 and 0 == index % self:CoWideNum() then
                index = 0
            elseif z < 0 and 1 == index % self:CoWideNum() then
                index = 0
            else
                local in_index = index - (x * self:CoWideNum()) - z
                local in_grid = self.grid_list[in_index]
                if z > 0 and 2 == to_index % self:CoWideNum() then
                    in_grid = nil
                elseif z < 0 and 5 == to_index % self:CoWideNum() then
                    in_grid = nil
                end
                -- 站在有箱子的格子不可推动其他箱子
                if nil ~= in_grid and in_grid:IsGridType(ArrayGhostConfig.GridType.Box) then
                    index = 0
                end
            end
        else
            index = 0
        end
    end
    if ai.index ~= index or ai.x ~= x or ai.z ~= z then
        self.arrow_info.index = index
        self.arrow_info.x = x
        self.arrow_info.z = z
    end
end

function ArrayGhostData:GridMove(index, x, z)
    self.arrow_info.index = 0
    local to_index = index + (x * self:CoWideNum()) + z
    local grid = self.grid_list[index]
    local to_grid = self.grid_list[to_index]
    local judge = true
    if to_grid and to_grid:ToGridType() then
        if z > 0 and 0 == index % self:CoWideNum() then
        elseif z < 0 and 1 == index % self:CoWideNum() then 
        elseif grid then
            judge = false
            self.grid_list[to_grid.index] = grid
            self.grid_list[grid.index] = to_grid
            grid.index = to_index
            to_grid.index = index
            local item_side = self:CoItemSide()
            local wide_num = self:CoWideNum()
            local from_x = item_side * (math.floor((index - 1) / wide_num))
            local from_z = item_side * ((index - 1) % wide_num)
            local to_x = item_side * (math.floor((to_index - 1) / wide_num))
            local to_z = item_side * ((to_index - 1) % wide_num)
            local mark_x = (to_x - from_x) > 0 and 1 or -1
            local mark_z = (to_z - from_z) > 0 and 1 or -1
            if nil ~= self.move_handle then
                Runner.Instance:RemoveRunUntilTrue(self.move_handle)
                self.move_handle = nil
            end
            self.is_moving = true
            self.move_handle = RunUntilTrue(function ()
                if (grid.show_obj.localPosition.x - to_x) *  -mark_x > 0 then
                    grid.show_obj.localPosition = Vector3.New(grid.show_obj.localPosition.x + mark_x *  ArrayGhostConfig.BOX_MOVE_SPEED * Time.deltaTime, 0, to_z)
                    return false
                elseif (grid.show_obj.localPosition.z - to_z) * -mark_z > 0 then
                    grid.show_obj.localPosition = Vector3.New(to_x, 0, grid.show_obj.localPosition.z + mark_z * ArrayGhostConfig.BOX_MOVE_SPEED * Time.deltaTime)
                    return false
                end
                grid.show_obj.localPosition = Vector3.New(to_x, 0, to_z)
                self:GridMove(to_index, x, z)
                return true
            end)
            return
        end
    end
    self.is_moving = false
    if judge then
        local is_succ = true
        for _, grid in ipairs(self.grid_list) do
            if grid:IsGridType(ArrayGhostConfig.GridType.Box) then
                if not self.goal_list[grid.index] then
                    is_succ = false
                end
            end
        end
        self.end_info.is_succ = is_succ
    end
end
