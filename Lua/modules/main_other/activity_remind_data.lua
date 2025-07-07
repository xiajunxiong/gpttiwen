ActivityRemindData = ActivityRemindData or BaseClass()

function ActivityRemindData:__init()
    if ActivityRemindData.Instance ~= nil then
        Debuger.LogError("[ActivityRemindData] attempt to create singleton twice!")
        return
    end
    ActivityRemindData.Instance = self
    self.remind_list = {}
end

function ActivityRemindData:DeleteRemindItem(act_type)
    self.remind_list[act_type] = nil
end

--设置提醒列表
function ActivityRemindData:SetRemindList(act_info,stamp_time)
    if self:IsRemindShow(act_info,stamp_time) then
        self.remind_list[act_info.act_type] = {act_info = act_info,stamp_time = stamp_time}
    end
end

function ActivityRemindData:IsRemindShow(act_info,stamp_time)
    -- if RoleSettingData.IsShield(ROLE_SETTING_TYPE.SET_SHOW_ACTIVITY_MSG) then
    --     return false
    -- end
    if stamp_time == 0 then
        return false
    end
    if not next(act_info) or act_info == nil then
        return false
    end
    if not ActivityData.IsEnoughLevel(act_info.act_type) then
        return false
    end
    if act_info.act_type > 10000 then   --本地日常活动不与服务端互通数据，这里默认返回已打开
        return true
    end
    local re = ActivityData.Instance:GetActivityStatus(act_info.act_type) ~= ActStatusType.Close
    return re
end

function ActivityRemindData:GetRemindList()
    local list = {}
    for k,v in pairs(self.remind_list) do
        if not self:IsRemindShow(v.act_info,v.stamp_time) then
            self.remind_list[v.act_info.act_type] = nil
        else
            table.insert(list,v)
        end
    end
    table.sort(list,DataHelper.SortFunc([[stamp_time]]))
    return list
end