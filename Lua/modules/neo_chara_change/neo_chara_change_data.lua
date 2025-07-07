NeoCharaChangeData = NeoCharaChangeData or BaseClass()

function NeoCharaChangeData:__init()
	if NeoCharaChangeData.Instance ~= nil then
		Debuger.LogError("[NeoCharaChangeData] attempt to create singleton twice!")
		return
	end
    NeoCharaChangeData.Instance = self
    
    self.prof_view_data = SmartData.New({
        appear_id = 0,
        prof_id = 0,
        color_index = -1,    
        app_index = 0, -- 点击的appitem位置
        is_show_attr = false,
        skill_info_tips = "",
        is_show_skill_tips = false,
        is_show_next = false,
        is_reast = false,
    })
    self.color_change_view_data = SmartData.New({
        color_index = 0,
    })

    self.image_change_cfg = Config.image_change_auto
end 

function NeoCharaChangeData:__delete()
    -- body
end

function NeoCharaChangeData:ClearProfViewData()
    self.prof_view_data = SmartData.New({
        appear_id = 0,
        prof_id = 0,
        color_index = -1, 
        app_index = 0,   
        is_show_attr = false,
        skill_info_tips = "",
        is_show_skill_tips = false,
        is_show_next = false,
        
        is_reast = false,
    })
end

function NeoCharaChangeData:GetProfViewData()
	return self.prof_view_data
end

-- 获取转职界面的形象信息（是否根据自定义阶段）
function NeoCharaChangeData:GetProfCharaList(stage)
    local protomotion_lv = stage or RoleData.Instance:ProfStage()
    local select_data = {}
    local index = 0
    for k, v in pairs(self.image_change_cfg.image_chan)do
        if protomotion_lv == v.camp_level then
            index = index + 1
            v.index = index
            table.insert(select_data,v)
        end
    end
    return select_data
end

--获取转职界面中形象对应的职业信息（是否根据自定义阶段和自定义角色类型）
function NeoCharaChangeData:GetProfCharaProfList(stage, appear_id)
    local protomotion_lv = stage or RoleData.Instance:ProfStage()
    local appear_id = appear_id or self.prof_view_data.appear_id
    local select_job = {}
    for k, v in pairs(self.image_change_cfg.image_job)do
        if protomotion_lv == v.camp_level and appear_id == v.avartar_type then
            local split_list = Split(v.job_id,"|")

            for i = 1,#split_list do 
                local vo = {}
                vo.index = i
                vo.job_id = tonumber(split_list[i])
                table.insert( select_job, vo )
            end 
            break
        end 
    end 
    return select_job
end

function NeoCharaChangeData:SetAppearId(appear_id)
    self.prof_view_data.appear_id = appear_id
end

function NeoCharaChangeData:SetProfId(prof_id)
    self.prof_view_data.prof_id = prof_id
end

function NeoCharaChangeData:ResetRoleAppData()
    self.prof_view_data.appear_id = 0
    self.prof_view_data.prof_id = 0
    self.prof_view_data.color_index = 0
    self.prof_view_data.app_index = 0
end

function NeoCharaChangeData:SetAppIndex(index)
    self.prof_view_data.app_index = index
end

function NeoCharaChangeData:SetColorIndex(color)
    self.prof_view_data.color_index = color - 1 < 0 and 0 or color - 1
end

function NeoCharaChangeData:SetIsShowAttr(flag)
    self.prof_view_data.is_show_attr = flag
end

function NeoCharaChangeData:GetNextAppearance( )
    local role_appearance = RoleData.Instance:GetApearance()

    if self.prof_view_data.appear_id == 0 or self.prof_view_data.prof_id == 0 or self.prof_view_data.color_index < 0 then 
        return false,nil
    end 

	local appearance = {
		weapon_id = RoleProfessionSimpleWeaponId[RoleData.ProfIdToProfType(self.prof_view_data.prof_id)] or 0,
		color = self.prof_view_data.color_index,
        prof_base = 0,
        advance_time = role_appearance.advance_time,
		avatar_type = self.prof_view_data.appear_id == 0 and role_appearance.avatar_type or math.floor(self.prof_view_data.appear_id)
    }
    return true,appearance
end

function NeoCharaChangeData:SetProfViewSkill(data)
    self.prof_view_data.skill_info_tips = data.tips_str or ""
    self.prof_view_data.is_show_skill_tips = data.is_show
end 

function NeoCharaChangeData:SetColorChangeColor(color)
    self.color_change_view_data.color_index = color - 1 < 0 and 0 or color - 1
end

function NeoCharaChangeData:ColorChangeData()
    return self.color_change_view_data
end

function NeoCharaChangeData:CacheAppearance(param)
    self.cahce_appearance = param
end

function NeoCharaChangeData:GetCacheAppearance()
    return self.cahce_appearance
end