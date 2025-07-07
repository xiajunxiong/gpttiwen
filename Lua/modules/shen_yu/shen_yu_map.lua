-- ============ 神域地图 ==========
ShenYuMap = ShenYuMap or DeclareView("ShenYuMap", "shen_yu/shen_yu_map")
function ShenYuMap:ShenYuMap()

end

function ShenYuMap:OnCloseClick()
    ViewMgr:CloseView(ShenYuMap)
end



ShenYuMapPanel = ShenYuMapPanel or DeclareMono("ShenYuMapPanel", UIWFlushPanel)
function ShenYuMapPanel:ShenYuMapPanel()
    self.data = ShenYuData.Instance
    self.content_w = 1950
    self.bounds = self.content_w / 2 - 300
    self.cell_size = 15
    self.min_index = 1
    self.max_index = math.ceil(self.content_w / 15)
    self.clone_obj = self.MapItem.gameObject
end 

function ShenYuMapPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:A1()
    --self:A2()
end

function ShenYuMapPanel:A1()
    self.data_list = {}
    self.item_list = {}
    self.obj_list = {}

    for i = 1, self.max_index do
        self.data_list[i] = {}
        for j = 1, self.max_index do
            local data = {}
            data.i = i
            data.j = j
            data.v = GameMath.Ramdon(0,1)
            self.data_list[i][j] = data
        end
    end

    
    for i = 1, 2116 do
        item_obj = self.clone_obj:Clone(self.ContentParent,false)
        item_obj:SetActive(false)
        table.insert(self.obj_list, item_obj)
        local lua_mono = item_obj:GetLuaComponent("ShenYuMapItem")
        table.insert(self.item_list, lua_mono)
    end
    self.run_ht = Runner.Instance:RunUntilTrue(BindTool.Bind(self.RunFunc, self),true)
end

function ShenYuMapPanel:A2()
    self.data_list = {}
    self.item_list = {}
    self.obj_list = {}

    for i = 1, self.max_index do
        self.data_list[i] = {}
        self.item_list[i] = {}
        self.obj_list[i] = {}
        for j = 1, self.max_index do
            local data = {}
            data.i = i
            data.j = j
            data.v = GameMath.Ramdon(0,1)
            self.data_list[i][j] = data

            item_obj = self.clone_obj:Clone(self.ContentParent,false)
            item_obj:SetActive(false)
            self.obj_list[i][j] = item_obj
            local lua_mono = item_obj:GetLuaComponent("ShenYuMapItem")
            lua_mono:SetData(data)
            self.item_list[i][j] = lua_mono


            local cell_x = i * self.cell_size - (self.cell_size / 2)
            cell_x = cell_x - self.content_w / 2
            local cell_y = j * self.cell_size - (self.cell_size / 2)
            cell_y = cell_y - self.content_w / 2
            local pos = Vector3.New(cell_x,cell_y,0)
            item_obj:SetLocalPosition(pos)
        end
    end

    self.run_ht = Runner.Instance:RunUntilTrue(BindTool.Bind(self.RunFunc, self),true)
end

function ShenYuMapPanel:B1(pos)
    local x = self.content_w / 2 - pos.x - 300
    local y = self.content_w / 2 - pos.y - 300
    local row_s,row_e = self:GetIndex(x)
    local col_s,col_e = self:GetIndex(y)
    if row_s == self.last_row_s and col_s == self.last_col_s then
        return
    end
    self.last_row_s = row_s
    self.last_col_s = col_s
    --LogError("row_s,row_e",row_s,row_e)
    --LogError("col_s,col_e",col_s,col_e)
    local obj_index = 0
    for i = row_s, row_e do
        for j = col_s, col_e do
            local cell_x = i * self.cell_size - (self.cell_size / 2)
            cell_x = cell_x - self.content_w / 2
            local cell_y = j * self.cell_size - (self.cell_size / 2)
            cell_y = cell_y - self.content_w / 2
            local pos = Vector3.New(cell_x,cell_y,0)
            obj_index = obj_index + 1
            local obj = self.obj_list[obj_index]
            obj:SetLocalPosition(pos)
            obj:SetActive(true)

            local mono = self.item_list[obj_index]
            local data = self.data_list[i][j]
            mono:SetData(data)
        end
    end
end

function ShenYuMapPanel:B2(pos)
    local x = self.content_w / 2 - pos.x - 300
    local y = self.content_w / 2 - pos.y - 300
    local row_s,row_e = self:GetIndex(x)
    local col_s,col_e = self:GetIndex(y)

    for i = 1, self.max_index do
        for j = 1, self.max_index do
            local obj = self.obj_list[i][j]
            if i >= row_s and i <= row_e and j >= col_s and j <= col_e and obj.activeSelf == false then
                obj:SetActive(true)
            elseif obj.activeSelf == true then
                obj:SetActive(false)
            end
        end
    end
end

function ShenYuMapPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
    end
end

function ShenYuMapPanel:RunFunc()
    local pos = self.ContentRect.localPosition
    if not self.last_pos or self.last_pos.x ~= pos.x or self.last_pos.y ~= pos.y then
        self.last_pos = pos
        local new_pos = self:ConvertPos(pos)
        self:HandlePos(new_pos)
    end
end

function ShenYuMapPanel:ConvertPos(pos)
    local new_pos = {}
    new_pos.x = math.ceil(pos.x)
    new_pos.y = math.ceil(pos.y)
    if new_pos.x > self.bounds then
        new_pos.x = self.bounds
    end
    if new_pos.x < -self.bounds then
        new_pos.x = -self.bounds
    end
    if new_pos.y > self.bounds then
        new_pos.y = self.bounds
    end
    if new_pos.y < -self.bounds then
        new_pos.y = -self.bounds
    end
    return new_pos
end

function ShenYuMapPanel:HandlePos(pos)
    self:B1(pos)
    --self:B2(pos)
end

-- 往外围多显示6圈格子
function ShenYuMapPanel:GetIndex(pos_v)
    local num = math.floor(pos_v / self.cell_size)
    local s_v = num - 3
    s_v = s_v < self.min_index and self.min_index or s_v
    local e_v = num + 43 - 1
    e_v = e_v > self.max_index and self.max_index or e_v
    return s_v, e_v
end


ShenYuMapItem = ShenYuMapItem or DeclareMono("ShenYuMapItem", UIWidgetBaseItem)
function ShenYuMapItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local color = data.v == 1 and COLORS.Green3 or COLORS.Yellow6
    UH.Color(self.Img, color)
end