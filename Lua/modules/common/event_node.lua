local process_center

EventNodeProcessCenter = EventNodeProcessCenter or BaseClass(BaseCtrl)

function EventNodeProcessCenter:__init()
    process_center = self
    self.process_queue = {}
    -- self.added_list = {}
    self.late_update = BindTool.Bind1(self.LateUpdate, self)
    LateUpdateBeat:Add(self.late_update)
end

function EventNodeProcessCenter:__delete()
    self.process_queue = nil
    -- self.added_list = nil
    LateUpdateBeat:Remove(self.late_update)
end

function EventNodeProcessCenter:LateUpdate()
    -- self.is_processing = true
    -- LogError("LateUpdate")
    local cur = 1
    -- local right = #self.process_queue
    -- local limit = 1000
    while cur <= #self.process_queue do
        -- LogError(cur, #self.process_queue)
        local node = self.process_queue[cur]
        if not node.is_deleted and node.is_dirty then
            local child_all_processed = true
            for child, _ in pairs(node.child_list) do
                if child.is_dirty then
                    child_all_processed = false
                end
            end
            if child_all_processed then
                node:Handle()
                node.is_dirty = false
            else
                -- self.process_queue[#self.process_queue + 1] = node
                -- right = right + 1
                table.insert(self.process_queue, node)
            end
        end
        cur = cur + 1
        -- limit = limit - 1
        -- if limit < 0 then
        --     break
        -- end
    end

    -- local queue = {}
    -- for i, node in ipairs(self.process_queue) do
    --     if not delete_indexes[i] then
    --         table.insert(queue, node)
    --     end
    -- end
    self.process_queue = {}
    -- self.added_list = {}
    -- self.is_processing = false
end

function EventNodeProcessCenter:Push(node)
    -- if self.is_processing or not self.added_list[node] then
    node.is_dirty = true
    table.insert(self.process_queue, node)
    -- self.added_list[node] = true
    -- end
end

EventNode = EventNode or BaseClass()

function EventNode:__init(action)
    self.key = nil
    self.val = nil
    self.action = action
    self.child_list = {}
    self.parent_list = {}
end

function EventNode:__delete()
    -- LogErrorHL("__delete")
    self.is_deleted = true
    self.val = nil
    self.action = nil
    for child, _ in pairs(self.child_list) do
        if child.parent_list then
            child.parent_list[self] = nil
        end
    end
    self.child_list = nil
    for parent, handle in pairs(self.parent_list) do
        if parent.child_list then
            process_center:Push(parent)
            parent.child_list[self] = nil
        end
    end
    self.parent_list = {}
end

function EventNode:SetKey(key)
    self.key = key
end

function EventNode:GetKey()
    return self.key
end

function EventNode:Set(val)
    -- LogError("Set", val)
    if self.val ~= val then
        -- LogError("Dirty", val)
        self.val = val
        self:SetDirty()
    end
end

function EventNode:Get()
    return self.val
end

function EventNode:SetDirty()
    process_center:Push(self)
    for parent, _ in pairs(self.parent_list) do
        process_center:Push(parent)
    end
end

function EventNode:Handle()
    if self.action then
        -- for child, _ in pairs(self.child_list) do
        --     LogError(child.val)
        -- end
        for child, _ in pairs(self.child_list) do
            if not child.val then
                -- LogError("self.action(false)")
                -- LogError("self.action(false)",tostring(child),child)
                self.action(false, self.child_list)
                return
            end
        end
        --[[ for child, _ in pairs(self.child_list) do
            LogError("self.action(true)",tostring(child))
        end ]]
        self.action(true, self.child_list)
    end
end

function EventNode:AddChild(node)
    -- LogError("AddChild",tostring(node))
    node.parent_list[self] = true
    self.child_list[node] = true
    self:SetDirty()
end
-- EventNodeAllTrue = EventNodeAllTrue or BaseClass(EventNode)

-- function EventNodeAllTrue:Handle()
--     LogError("Handle")
--     if self.action then
--         -- for child, _ in pairs(self.child_list) do
--         --     LogError(child.val)
--         -- end
--         for child, _ in pairs(self.child_list) do
--             if not child.val then
--                 -- LogError("self.action(false)")
--                 self.action(false)
--                 return
--             end
--         end
--         -- LogError("self.action(true)")
--         self.action(true)
--     end
-- end

-- EventNodeLoad = EventNodeLoad or BaseClass(EventNode)

-- function EventNodeLoad:Handle()
--     LogError("Handle")
--     if self.action then
--         -- for child, _ in pairs(self.child_list) do
--         --     LogError(child.val)
--         -- end
--         for child, _ in pairs(self.child_list) do
--             if not child.val then
--                 -- LogError("self.action(false)")
--                 self.action(false)
--                 return
--             end
--         end
--         -- LogError("self.action(true)")
--         self.action(true)
--     end
-- end

-- 子节点数值变化
-- 子节点增删

-- function EventNode:RemoveChild(node)
--     self.child_list[node] = nil
-- end

-- EventNodeRoot = EventNodeRoot or BaseClass(EventNode)

-- EventNodeNormal = EventNodeNormal or BaseClass(EventNode)

-- function EventNodeNormal:__init(val, action)
--     self.action = action
-- end

-- function EventNodeNormal:__delete()
--     self.action = nil
-- end

-- function EventNodeNormal:Handle()
--     LogErrorHL("HandleBegin", self.val)
--     local result
--     for c, _ in pairs(self.child_list) do
--         if c.val == nil then
--             return
--         end
--         LogError(c.val)
--         if result == nil then
--             result = c.val
--         elseif result ~= c.val then
--             return
--         end
--     end
--     if self.action then
--         result = self.action(result)
--     end
--     self:Set(result)
--     LogErrorHL("HandleEnd", self.val)
-- end
