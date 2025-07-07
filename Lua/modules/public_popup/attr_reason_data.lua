AttrReasonData = AttrReasonData or BaseClass()
function AttrReasonData:__init()
    if AttrReasonData.Instance ~= nil then
        Debuger.LogError("[AttrReasonData] attempt to create singleton twice!")
        return
    end
    AttrReasonData.Instance = self

    self.reason_data = {}
    self.select_data = SmartData.New({reason_type = 0, param1 = 0, attr_type = 0, position = Vector3.zero})
end

--[[ param参数
    reason_type 属性来源列表类型(ATTRLIST_REASON_TYPE)
    param1      属性来源列表参数(角色0，宠物pet_index, 伙伴partner_id)
    attr_type   属性类型(BattleAttriType)
    position    窗口显示位置Vector2
]]
function AttrReasonData:SetSelectData(param)
    self.select_data.reason_type = param.reason_type
    self.select_data.param1 = param.param1
    self.select_data.attr_type = param.attr_type
    self.select_data.position = param.position
end

function AttrReasonData:GetSelectData()
    return self.select_data
end

function AttrReasonData:OnAttrListReason(protocol)
    if self.select_data ~= nil and self.select_data.reason_type ~= protocol.reason_type then
        return
    end
    self.reason_data.reason_type = protocol.reason_type
    self.reason_data.param1 = protocol.param1
    self.reason_data.reason_list = protocol.reason_list
end

function AttrReasonData:GetReasonData()
    if self.reason_data ~= nil and self.select_data ~= nil and self.reason_data.reason_type == self.select_data.reason_type then
        return self.reason_data
    end
end

--获取展示属性列表
function AttrReasonData:GetReasonShowList(reason_type, param1, attr_type)
    local data = self:GetReasonData()
    local show_list = {}
    if data ~= nil then
        for i, v in pairs(data.reason_list) do
            local value = v[attr_type] or 0
            if value ~= 0 then
                table.insert(show_list, {type = i, value = value})
            end
        end
        table.sort(show_list, DataSorter.KeyUpperSorter("value"))
    end
    return show_list
end

