
TaskViewAdventure = TaskViewAdventure or DeclareMono("TaskViewAdventure", UIWFlushPanel)

function TaskViewAdventure:TaskViewAdventure()
end

function TaskViewAdventure:Awake()
    UIWFlushPanel.Awake(self)
    -- local data = {
    --     {
    --         index = 1,
    --     },
    --     {
    --         index = 2,
    --     },
    --     {
    --         index = 3,
    --     },
    -- }
    local data = {}
    self.ShowList:SetData(data)
    self.EmptyObj:SetActive(nil == next(data))
end

function TaskViewAdventure:OnClickTaskItem(data)
    -- Debuger.LogError("OnClickTaskItem")
end

TaskViewAdventureItem = DeclareMono("TaskViewAdventureItem", UIWidgetBaseItem)

function TaskViewAdventureItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
end
