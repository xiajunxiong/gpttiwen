HexagonFloor = HexagonFloor or DeclareMono("HexagonFloor", UIWFlushPanel)
function HexagonFloor:HexagonFloor()

end
function HexagonFloor:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.row = 8
    self.col = 7
    -- 对角线长度
    -- 对边长度
    -- 边长
    -- 中心点
    -- self.diagonal_length = 3.534
    -- self.opposite_edges_length = 3.06
    -- self.side_length = 1.767
    self.diagonal_length = 3.887
    self.opposite_edges_length = 3.366
    self.side_length = 1.944
    self.offset = 0
    -- 前后排距离 = 对边长度
    -- 前排到中心 = 1.5 * 对边长度
    -- 间隔距离 = （边长+ 对角长度）/2
    -- 偏移 =0.5*对边长度

    -- 六边形的位置计算
    -- 横排间隔= n * （边长+ 对角长度）/2
    -- 一高一低 = (n % 2 == 0 ? 1f : 0f) * 对边长度 / 2f 
    -- 纵排间隔 = l * 对边长度
    -- self:CreateHexagon()
    local data = {}
    local data2 = {}
    local is_floor = true
    local grid_idx = 1
    for i = 1, self.row do
        for j = 1, self.col do
            is_floor = true
            if (i == 2 or i == 3 or i == 6) and (j > 1 and j < 7) then
                table.insert(data2, {
                    row = i,
                    col = j,
                    pos = self:GetPos(i, j),
                    grid_idx = grid_idx,
                })
                is_floor = false
                grid_idx = grid_idx + 1
            end
            if i == 5 and (j == 3 or j == 5) then
                table.insert(data2, {
                    row = i,
                    col = j,
                    pos = self:GetPos(i, j),
                    grid_idx = grid_idx,
                })
                is_floor = false
                grid_idx = grid_idx + 1
            end
            if i == 7 and (j == 2 or j == 4 or j == 6) then
                table.insert(data2, {
                    row = i,
                    col = j,
                    pos = self:GetPos(i, j),
                    grid_idx = grid_idx,
                })
                is_floor = false
                grid_idx = grid_idx + 1
            end
            if is_floor then
                table.insert(data, {
                    row = i,
                    col = j,
                    pos = self:GetPos(i, j)
                })
            end
        end
    end
    local p1 = self:GetPos(self.row, self.col)
    local p2 = self:GetPos(self.row, self.col - 1)
    local p3 = Vector3.New(p1.x / 2, 0, p2.z / 2)
    self.ParentNode:SetLocalPosition(Vector3.zero:Sub(p3))
    self.List:SetData(data)
    self.List2:SetData(data2)
    -- 可站区域显示
    -- 2排的23456
    -- 3排的23456
    -- 5排的35
    -- 6排的23456
    -- 7排的246

    -- 获取蜂窝状坐标组
    -- 例如坐标 2|2 即组为 1排的 123 2排的123 3排的2
    -- 分单双
    -- 例如做表 2|3 即组为 2排的 234 3排的234 1排的3

    local hex_list = self:GetHexagon(5, 5)
    self.List4:SetData(hex_list)
end
function HexagonFloor:GetHexagon(i, j)
    local data = {}
    local a, b
    if j % 2 == 0 then
        for l = (i - 1), i do
            for k = 1, 3 do
                a = l
                b = j - 2 + k
                if a > 0 and b > 0 then
                    table.insert(data, {
                        row = a,
                        col = b,
                        pos = self:GetPos(a, b)
                    })
                end
            end
        end
        table.insert(data, {
            row = i + 1,
            col = j,
            pos = self:GetPos(i + 1, j)
        })
    else
        for l = i, i + 1 do
            for k = 1, 3 do
                a = l
                b = j - 2 + k
                if a > 0 and b > 0 then
                    table.insert(data, {
                        row = a,
                        col = b,
                        pos = self:GetPos(a, b)
                    })
                end
            end
        end
        table.insert(data, {
            row = i - 1,
            col = j,
            pos = self:GetPos(i - 1, j)
        })
    end
    return data
end
function HexagonFloor:GetPos(i, j)
    i = i - 1
    j = j - 1
    local x = j * (self.diagonal_length + self.side_length) / 2 + (j * self.offset);
    local z = i * (self.opposite_edges_length) + (j % 2 == 0 and 1 or 0) * self.opposite_edges_length / 2 +
                  (i * self.offset);
    return Vector3.New(x, 0, z)
end

HexagonFloorItem = HexagonFloorItem or DeclareMono("HexagonFloorItem", UIWidgetBaseItem)
HexagonFloorItem.default_color = Color.New(0.5, 0.5, 0.5, 0.5)
function HexagonFloorItem:HexagonFloorItem()

end
function HexagonFloorItem:SetData(data)
    self.gameObject.name = string.format("%s|%s", data.row, data.col)
    self.ItemTran.localPosition = data.pos
    self.ItemTran.localEulerAngles = Vector3.New(0, 90, 0)
    if self.IsGray then
        -- self.Render.sharedMaterial:SetColor("_TintColor", COLORS.Blue1)
    end
end
function HexagonFloorItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    if self.IsGray then
        self.Render.sharedMaterial:SetColor("_TintColor", HexagonFloorItem.default_color)
    end
end
