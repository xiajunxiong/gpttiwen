MedalTaskStart = MedalTaskStart or DeclareView("MedalTaskStart", "medal/medal_task_start", Mod.MedalStart.Main)
function MedalTaskStart:MedalTaskStart()
    self.data = MedalTaskData.Instance
end
function MedalTaskStart:LoadCallback(index)
    self.role_name = RoleData.Instance:GetBaseData().name
    self.index = index
    local data = self.data:GetMailTask(self.index)
    local items = string.split(data.reward_show, ":")
    local item = Item.Create({item_id = tonumber(items[1]),num = tonumber(items[2]), is_bind = tonumber(items[3])})
    self.Cell:SetData(item)--SetDataByItemIdWithoutZeroNum(data.reward_show)
    local list = string.split(data.description, "|")
    local str
    for i = 1, table.nums(list) do
        if self.DescList[i] then
            str = list[i]
            if string.find(str, "<player_name>") then
                str = string.gsub(str, "<player_name>", self.role_name)
            end
            self.DescList[i].text = str
        end
    end
end
function MedalTaskStart:OnClickOpen()
    MedalTaskCtrl.Instance:SendMedalTaskPlayVideo(self.index)
    ViewMgr:CloseView(MedalTaskStart)
end
function MedalTaskStart:OnClickClose()
    ViewMgr:CloseView(MedalTaskStart)
end
