QueueHelper = QueueHelper or BaseClass()

function QueueHelper:__init()
    if QueueHelper.Instance ~= nil then
        Debuger.LogError("[QueueHelper] attempt to create singleton twice!")
        return
    end
    QueueHelper.Instance = self
end
--@G字符串记录 聊天记录 获奖记录 .. 本地存储解决方案
--获取队列信息Queue
function QueueHelper:GetStringList(key,count)
    if self[key] == nil then
        self[key] = self:GetStringPrefs(key,count)
    end
    return self[key]
end

function QueueHelper:GetStringPrefs(key,count)
    local list = {[0] = count}
    for i=1,count do
        local str = UnityPlayerPrefs.GetString(key..i)
        if str == "" then break end
        list[i] = str
    end
    return list
end

function QueueHelper:AddStringList(key,str)
    if #self[key] >= self[key][0] then
        table.remove(self[key])
    end
    table.insert(self[key],str)
end

function QueueHelper:SeveStringPrefs(key)
    for i=1,#self[key] do
        UnityPlayerPrefs.SetString(key..i,self[key][i])
    end
end

--队列解决方案
QueueItem = QueueItem or BaseClass()

function QueueItem:__init(key,func)
    self.key = key
    self.func = func
    self.data = SmartData.New{list = {}}
end

--入队 末尾插入
function QueueItem:Add(param)
    if self:IsNotSame(param) then
        table.insert(self.data.list,param)
    end
end

--出队 首位删除
function QueueItem:Remove()
    if #self.data.list ~= 0 then
        table.remove(self.data.list)
    else
        self:Clear()
    end
end

function QueueItem:IsNotSame(param)
    if #self.data.list == 0 then
        return true
    end
    for k,v in pairs(self.data.list or {}) do
        if self.key ~= nil then
            if v[self.key] == param[self.key] then
                return false
            end
        else
            if v == param then
                return false
            end
        end
    end
    return true
end

function QueueItem:Care(func)
    return self.data:Care(func)
end

function QueueItem:UnCare(handle)
    self.data:Uncare(handle)
end

function QueueItem:GetParam()
    return self.data.list[1]
end

function QueueItem:GetList()
    return self.data.list
end

function QueueItem:Length()
    return #self.data.list
end

function QueueItem:Clear()
    self.data.list = {}
    if self.func then
        self.func()
    end
end