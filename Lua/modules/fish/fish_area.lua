FishAreaPanel1 = FishAreaPanel1 or DeclareMono("FishAreaPanel1", UIWFlushPanel)
function FishAreaPanel1:Awake()
    UIWFlushPanel.Awake(self)
    self.data = FishData.Instance
    self.standing_area = nil
    self.fishing_area = {}
    self.role_pos_event = EventSys:Bind(GlobalEventKeys.RolePosChange, BindTool.Bind(self.OnMainRolePosChange, self))
end
function FishAreaPanel1:OnEnable()
    UIWFlushPanel.OnEnable(self)
    --开启检查
    
    for i = 1, self.AreaCollider:Length() do
        table.insert(self.fishing_area, Bounds.Create(self.AreaCollider[i].bounds))
    end
    self.handle_delay = TimeHelper:AddDelayTimer(function ()
        local pos = self.data:RamdonPoint()
        SceneLogic.Instance:AutoToPos(nil, {x = pos.pos_x,y = pos.pos_y}, nil, false, false)
    end,1)
end
function FishAreaPanel1:OnMainRolePosChange()
    self.standing_area = nil
    for index, value in ipairs(self.fishing_area) do
        if value:Contains(MainPlayer:GetPos()) then
            self.standing_area = self.AreaCollider[index]
            break
        end
    end
    self.data:Fish().fish_area = self.standing_area
end
function FishAreaPanel1:OnDestroy()
    TimeHelper:CancelTimer(self.handle_delay)
    EventSys:UnBind(self.role_pos_event)
    UIWFlushPanel.OnDestroy(self)
end

FishAreaPanel2 = FishAreaPanel2 or DeclareMono("FishAreaPanel2", UIWFlushPanel)
function FishAreaPanel2:Awake()
    UIWFlushPanel.Awake(self)
    self.data = FishData.Instance
    self.standing_area = nil
    self.fishing_area = {}
    self.role_pos_event = EventSys:Bind(GlobalEventKeys.RolePosChange, BindTool.Bind(self.OnMainRolePosChange, self))
end
function FishAreaPanel2:OnEnable()
    UIWFlushPanel.OnEnable(self)
    --开启检查
    
    for i = 1, self.AreaCollider:Length() do
        table.insert(self.fishing_area, Bounds.Create(self.AreaCollider[i].bounds))
    end
    self.handle_delay = TimeHelper:AddDelayTimer(function ()
        local pos = self.data:RamdonPoint()
        SceneLogic.Instance:AutoToPos(nil, {x = pos.pos_x,y = pos.pos_y}, nil, false, false)
    end,1)
end
function FishAreaPanel2:OnMainRolePosChange()
    self.standing_area = nil
    for index, value in ipairs(self.fishing_area) do
        if value:Contains(MainPlayer:GetPos()) then
            self.standing_area = self.AreaCollider[index]
            break
        end
    end
    self.data:Fish().fish_area = self.standing_area
end
function FishAreaPanel2:OnDestroy()
    TimeHelper:CancelTimer(self.handle_delay)
    EventSys:UnBind(self.role_pos_event)
    UIWFlushPanel.OnDestroy(self)
end

FishAreaPanel3 = FishAreaPanel3 or DeclareMono("FishAreaPanel3", UIWFlushPanel)
function FishAreaPanel3:Awake()
    UIWFlushPanel.Awake(self)
    self.data = FishData.Instance
    self.standing_area = nil
    self.fishing_area = {}
    self.role_pos_event = EventSys:Bind(GlobalEventKeys.RolePosChange, BindTool.Bind(self.OnMainRolePosChange, self))
end
function FishAreaPanel3:OnEnable()
    UIWFlushPanel.OnEnable(self)
    --开启检查
    
    for i = 1, self.AreaCollider:Length() do
        table.insert(self.fishing_area, Bounds.Create(self.AreaCollider[i].bounds))
    end
    self.handle_delay = TimeHelper:AddDelayTimer(function ()
        local pos = self.data:RamdonPoint()
        SceneLogic.Instance:AutoToPos(nil, {x = pos.pos_x,y = pos.pos_y}, nil, false, false)
    end,1)
end
function FishAreaPanel3:OnMainRolePosChange()
    self.standing_area = nil
    for index, value in ipairs(self.fishing_area) do
        if value:Contains(MainPlayer:GetPos()) then
            self.standing_area = self.AreaCollider[index]
            break
        end
    end
    self.data:Fish().fish_area = self.standing_area
end
function FishAreaPanel3:OnDestroy()
    TimeHelper:CancelTimer(self.handle_delay)
    EventSys:UnBind(self.role_pos_event)
    UIWFlushPanel.OnDestroy(self)
end