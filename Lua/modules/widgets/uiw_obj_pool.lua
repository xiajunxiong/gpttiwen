UIWObjPool = UIWObjPool or DeclareMono("UIWObjPool")

----Item======UIWObjPoolItem or LuaMono

function UIWObjPool:UIWObjPool()
    -- 记录池中可用对象
    self.caches = {} --key items
    self.cache_count = 0
    self.active_objs = {}
end

function UIWObjPool:Spwan(data)
    local spwan_item = nil
    for k, _ in pairs(self.caches) do
        spwan_item = k
        self.caches[k] = nil
        break
    end

    if spwan_item == nil then
        local spwan_obj = self.Item.gameObject:Clone(self.gameObject)
        spwan_item = spwan_obj:GetLuaComponent(self.Item:GetTypeName())
        spwan_item:onCreate(self)
        spwan_item.game_obj = spwan_obj
    end
    spwan_item:SetObjActive(true)
    if data ~= nil then
        spwan_item:setData(data)
        spwan_item.data = data
    end
    self.active_objs[spwan_item] = true
    return spwan_item
end

function UIWObjPool:Reorder(sort_func)
    local obj_list = {}
    for obj, _ in pairs(self.active_objs) do
        table.insert(obj_list, obj)
    end
    table.sort(obj_list, sort_func)
    for i, obj in ipairs(obj_list) do
        obj.game_obj.transform:SetSiblingIndex(i)
    end
end

function UIWObjPool:Recycle(item)
    if self.caches[item] ~= nil then
        return
    end
    self.caches[item] = true
    self.active_objs[item] = nil
    item:onRecycle()
    item.gameObject:SetParent(self.gameObject)
    item:SetObjActive(false)
end

function UIWObjPool:GetActiveObjs()
    return self.active_objs
end

function UIWObjPool:RecycleActiveObjs()
    for k, v in pairs(self.active_objs) do
        self:Recycle(k)
    end
end

UIWObjPoolItem = UIWObjPoolItem or DeclareMono("UIWObjPoolItem")

function UIWObjPoolItem:onCreate(pool)
    self.pool = pool
    self.is_active = true
end

function UIWObjPoolItem:RecycleSelf()
    self.pool:Recycle(self)
    self.is_active = false
end

---override
function UIWObjPoolItem:setData(data)
end

---override
function UIWObjPoolItem:onRecycle()
end
