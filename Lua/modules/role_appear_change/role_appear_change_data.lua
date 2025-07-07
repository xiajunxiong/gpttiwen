RoleAppearChangeData = RoleAppearChangeData or BaseClass()

function RoleAppearChangeData:__init()
    if RoleAppearChangeData.Instance ~= nil then
        Debuger.LogError("[RoleData] attempt to create singleton twice!")
    end
    RoleAppearChangeData.Instance = self
	self.appear_data = SmartData.New({
        cur_left_select = 0,
        cur_color = 0,
    })
    -- self.cur_left_select = 0         --当前选择的形象(未选择颜色)
end

function RoleAppearChangeData:__delete()
    if RoleAppearChangeData.Instance == self then
        RoleAppearChangeData.Instance = nil
    end
end

-- 精确查询
function RoleAppearChangeData:GetRoleAppCfgByAppear(appear)
    local config = Config.image_change_auto.image_chan

    for k,v in pairs(config) do
        if v.camp_level == appear.advance_time and v.avartar_type == appear.avatar_type then 
            return v
        end 
    end 
end

function RoleAppearChangeData:GetSeletData()
    local protomotion_lv = RoleData.Instance:ProfStage()
    --Debuger.LogError(tostring(protomotion_lv))
    local config = Config.image_change_auto.image_chan
    local select_data = {}
    local index = 0
    for k, v in pairs(config)do
        if protomotion_lv == v.camp_level and v.avartar_type ~= 2 then -- and v.avartar_type ~= 6 then
            index = index + 1
            v.index = index
            table.insert(select_data,v)
        end
    end

    return select_data
end

function RoleAppearChangeData:GetSelectRoleAppCfg()
    local data_list = self:GetSeletData()

    for k,v in pairs(data_list) do 
        if v.avartar_type == self.appear_data.cur_left_select then 
            return v
        end 
    end 
    return nil
end

function RoleAppearChangeData:GetAppearSendCfg(send_id)
    local config = Config.image_change_auto.image_chan

    local select_show_id = math.floor( send_id / 100)
    local select_color = math.floor( send_id % 100)
    local select_image_chan = {}
    for k,v in pairs(config) do 
        if v.image_show_id == select_show_id then 
            select_image_chan = v
            break
        end 
    end 

    select_image_chan.select_color = select_color - 1 >= 0 and select_color - 1 or 0

    return select_image_chan
end 

function RoleAppearChangeData:GetAppearSendAppearance()
    -- local select_image_chan = self:GetAppearSendCfg(send_id)
    local role_appearance = RoleData.Instance:GetApearance()

	local appearance = {
		weapon_id = 0,
		color = self.appear_data.cur_color,
        prof_base = 0,
        advance_time = role_appearance.advance_time,
		avatar_type = self.appear_data.cur_left_select == nil and role_appearance.avatar_type or math.floor(self.appear_data.cur_left_select)
    }
    return appearance
end

-- function RoleAppearChangeData:GetAppearSendID(color)
--     if color == nil then color = self.appear_data.cur_color end 
--     return self.appear_data.cur_left_select * 100 + color
-- end

function RoleAppearChangeData:SetCurColor(color)
    self.appear_data.cur_color = color - 1 < 0 and 0 or color - 1
end
