
RoleCreateConfig = {
	Sex = {
		women = 0,
		man = 1,
	},

	RoleSpName = {
		[1] = "_LocYueZhaoJun",
		[2] = "_LocHuaOu",
		[3] = "_LocMaoErNiang",
		[4] = "_LocTuHun",
		[5] = "_LocBanYue",
		[6] = "_LocJiaoNan",
	},

	DefaultProf = {
		[1] = 2,
		[2] = 3,
		[3] = 2,
		[4] = 2,
		[5] = 2,
		[6] = 2,
	},

	Weapon2 = {
		[42] = true,
		[43] = true,
		[53] = true,
		[64] = true,
	},

	WeaponShow = {
		[13] = 1,
		[21] = 2,
		[34] = 3,
		[11] = 4,
		[61] = 5,
	}

}

RoleCreateData = RoleCreateData or BaseClass()

function RoleCreateData:__init()
	if RoleCreateData.Instance ~= nil then
		Debuger.LogError("[RoleCreateData] attempt to create singleton twice!")
		return
	end
	RoleCreateData.Instance = self

	self:RoleCreateConfig()
	self:RoleCreateData()
end

function RoleCreateData:RoleCreateConfig()
	self.config = Config.image_change_auto
	-- self.config_create_role = self.config.create_role
	self.config_create_image = self.config.image_chan
	self.config_role = self.config.role
	self.config_prof = self.config.image_job

	self.role_name_config = Config.role_name_auto
	self.role_name_config_role_front = self.role_name_config.role_front
	self.role_name_config_common_middle = self.role_name_config.common_middle
	self.role_name_config_common_last = self.role_name_config.common_last
end

function RoleCreateData:RoleCreateData()
	self:InitRoleCharaList()
	self:InitRoleColorList()
	self:InitRoleProfList()
	self:InitRoleNameList()

	self:RCDataRoleList()

	self.create_view_data = SmartData.New({
		appear_id = 0,
		prof_id = 0,
		color_index = -1,
		advance_time = 1,
		sex = 0,
		role_desc = "",
		role_name = "",
		is_show_right = false,
		is_show_prof = false,
		is_show_skill_tips = false,
		skill_info_tips = "", 
		skill_info_name = "",
		skill_info_icon = "",
	})

	self.create_director_data = SmartData.New({
		appear_id = 0,
	})

	self.lock_create_director = true
	self.model_rotate = SmartData.New({is_show = false})
	self.model_rotate_info = {
		ready = false,
		obj = nil,
		parent = nil,
		root = nil,
	}
end

function RoleCreateData:ClearCreateData()
	self.create_view_data.appear_id = 0
	self.create_view_data.prof_id = 0
	self.create_view_data.color_index = -1
	self.create_view_data.sex = 0
	self.create_view_data.role_desc = ""
	self.create_view_data.advance_time = 1
	self.create_view_data.role_name = ""
	self.create_view_data.is_show_right = false
	self.create_view_data.is_show_prof = false
	self.create_view_data.is_show_skill_tips = false
	self.create_view_data.skill_info_tips = ""
	self.create_view_data.skill_info_name = ""
	self.create_view_data.skill_info_icon = ""
end

function RoleCreateData:RCDataRoleList()
	self.role_list = {}
	for k, v in pairs(self.config_role) do
		-- 屏蔽萝莉
		-- if 2 ~= v.role_id then
			table.insert(self.role_list, v)
		-- end
	end
	table.sort(self.role_list, function (a, b)
		return a.seq < b.seq
	end)
end

function RoleCreateData:GetRCDataRoleList()
	return self.role_list
end

function RoleCreateData:ClearDirectorData()
	self.create_director_data.appear_id = 0
end

function RoleCreateData:InitRoleCharaList()
	self.chara_list = {}
	for k, v in pairs(self.config_role) do
		-- 屏蔽萝莉
		-- if 2 ~= v.role_id then
			table.insert(self.chara_list, v)
		-- end
	end
	table.sort(self.chara_list, function (a, b)
		return a.seq < b.seq
	end)
end

function RoleCreateData:GetRoleCharaList()
	return self.chara_list
end

function RoleCreateData:GetCharaDataByAvatarType(avatar_type)
	for _, v in pairs(self.chara_list) do
		if v.role_id == avatar_type then
			return v
		end
	end
end

function RoleCreateData:InitRoleColorList()
	self.chara_color_list = {}
	for k, v in pairs(self.config_create_image) do
		if v.camp_level == 1 then 
			self.chara_color_list[v.avartar_type] = self.chara_color_list[v.avartar_type] or {}

			local cur_color_list = Split(v.color_list,",")
			local neo_color_list = {}
			local index = 1
			for i,j in pairs(cur_color_list) do 
				local vo = {}
				vo.color_id = j
				vo.color_index = index
				vo.avartar_type = v.avartar_type
	
				table.insert( neo_color_list, vo )
	
				index = index + 1
			end 
			
			self.chara_color_list[v.avartar_type].color_list = neo_color_list
		end 

	end
end

function RoleCreateData:GetRCDataAdvanceList()
	return self.advance_color_list[self.cur_sel_role.role_id]
end

function RoleCreateData:InitRoleProfList()
	self.chara_prof_list = {}
	for k,v in pairs(self.config_prof) do 
		if v.camp_level == 1 then 
			self.chara_prof_list[v.avartar_type] = self.chara_prof_list[v.avartar_type] or {}

			local cur_prof_list = Split(v.job_id,"|")
			local neo_prof_list = {}
			local index = 1
			for i,j in pairs(cur_prof_list) do 
				local vo = {}
				vo.prof_id = tonumber(j)
				vo.prof_index = index
				vo.avartar_type = v.avartar_type
	
				table.insert( neo_prof_list, vo )
	
				index = index + 1
			end 

			self.chara_prof_list[v.avartar_type].prof_list = neo_prof_list
		end 
	end 

end 

function RoleCreateData:GetRoleProfList(avartar_type)
	return self.chara_prof_list[avartar_type]
end

function RoleCreateData:GetRoleColorList(avartar_type)
	return self.chara_color_list[avartar_type]
end

function RoleCreateData:GetRoleColorListDetail(avartar_type,advance_time)
	for k, v in pairs(self.config_create_image) do
		if v.avartar_type == avartar_type and advance_time == v.camp_level then 

			local cur_color_list = Split(v.color_list,",")
			local neo_color_list = {}
			local index = 1
			for i,j in pairs(cur_color_list) do 
				local vo = {}
				vo.color_id = j
				vo.color_index = index
				vo.avartar_type = v.avartar_type
	
				table.insert( neo_color_list, vo )
	
				index = index + 1
			end 
			
			return neo_color_list
		end 
	end 
	return nil
end

-- function RoleCreateData:GetRCDataColorList()
-- 	return self.cur_sel_advance.color_list
-- end

function RoleCreateData:InitRoleNameList()
	self.role_name_config_first = {
		[RoleCreateConfig.Sex.women] = self.role_name_config.female_first,
		[RoleCreateConfig.Sex.man] = self.role_name_config.male_first,
	}
	self.role_name_config_middle = {
		[RoleCreateConfig.Sex.women] = self.role_name_config.female_middle,
		[RoleCreateConfig.Sex.man] = self.role_name_config.male_middle,
	}
	self.role_name_config_last = {
		[RoleCreateConfig.Sex.women] = self.role_name_config.female_last,
		[RoleCreateConfig.Sex.man] = self.role_name_config.male_last,
	}
	self.role_name = SmartData.New{
		name = ""
	}
end

function RoleCreateData:SetRandomName(t)
	local flag = true
	local f, m, l = "", "" ,""

	self.name_str_id = t.name_str_id

	local f_co = self.role_name_config_first[t.front_sex][t.front_id]
	local m_co = self.role_name_config_middle[t.middle_sex][t.middle_id]
	local l_co = self.role_name_config_last[t.back_sex][t.back_id]
	f = f_co and f_co.name or ""
	m = m_co and m_co.name or ""
	l = l_co and l_co.name or ""

	local role_name = GetLocalType() == LocalType.KOR and f..l or f..m..l
	if string.len(role_name) < 16 and not ChatFilter.Instance:IsIllegal(role_name, true) then
		flag = false
		self.create_view_data.role_name =  ChatFilter.Instance:FilterUserName(role_name)
	end
	return flag
end


function RoleCreateData:CheckRoleName(role_name)
	local flag = false
	local error_code = ""
	if role_name ~= self.role_name.name then
		self.role_name.name = role_name
		self.name_str_id = 0
	end
	if StringIsEmpty(role_name) then
		error_code = Language.RoleCreate.ErrorCode.EditName
	elseif DataHelper.GetStringWordNum(role_name) > 6 then
		error_code = Language.RoleCreate.ErrorCode.NameToLong
	elseif ChatFilter.Instance:IsIllegal(role_name, true) then
		error_code = Language.Chat.ErrorCode.IllegalContent
	else
		flag = true
	end
	return flag, error_code
end

function RoleCreateData:ShowRight(is_show)
	if nil ~= is_show then
		self.create_view_data.is_show_right = is_show
	else
		is_show = self.create_view_data.is_show_right
	end

    return is_show
end

function RoleCreateData:SetAppearId(appear_id)
	self.create_view_data.appear_id = appear_id
end

function RoleCreateData:SetDirectorAppearId(appear_id)
	self.create_director_data.appear_id = appear_id
end

function RoleCreateData:ResetAppearId()
	self.create_view_data.appear_id = 0
	self.create_director_data.appear_id = 0
end

function RoleCreateData:SetSex(_sex)
	self.create_view_data.sex = _sex
end

function RoleCreateData:SetProfId(prof_id)
	self.create_view_data.prof_id = prof_id
end

function RoleCreateData:GetProfId()
	return self.create_view_data.prof_id 
end

function RoleCreateData:SetColorIndex(color)
    self.create_view_data.color_index = color - 1 < 0 and 0 or color - 1
end

function RoleCreateData:SetAdvanceTime(advance_time)
    self.create_view_data.advance_time = advance_time
end

function RoleCreateData:SetIsShowProf(_flag)
	self.create_view_data.is_show_prof = _flag
end 

function RoleCreateData:SetRoleCharaData(_data)
	self:SetAppearId(_data.role_id)
	self:SetSex(_data.sex)
end

function RoleCreateData:SetDirectorCharaData(_data)
	self:SetDirectorAppearId(_data.role_id)
end

function RoleCreateData:GetCreateAppearance( )

	local appearance = {
		weapon_id = RoleProfessionSimpleWeaponId[RoleData.ProfIdToProfType(self.create_view_data.prof_id)] or 0,
		color = self.create_view_data.color_index,
        prof_base = 0,
        advance_time = self.create_view_data.advance_time ,
		avatar_type = self.create_view_data.appear_id 
    }

	if appearance.color == -1 or appearance.avatar_type == 0 then return nil end 

    return appearance
end

function RoleCreateData:SetProfViewSkill(data)
    self.create_view_data.skill_info_tips = data.tips_str or ""
    self.create_view_data.is_show_skill_tips = data.is_show
	self.create_view_data.skill_info_name = data.skill_name 
	self.create_view_data.skill_info_icon = data.skill_icon
end 

function RoleCreateData:LockUpDirector(flag)
	self.lock_create_director = flag
end

function RoleCreateData:UnLockDirector()
	self.lock_create_director = nil
end

function RoleCreateData:GetIsLockDirector()
	return self.lock_create_director
end

function RoleCreateData:DelayBlack(flag)
	self.lock_delay = flag
end

function RoleCreateData:IsDelayBlack()
	return self.lock_delay 
end

function RoleCreateData:SetRecommendJob(data)
	self.recommend_jobs = data
end

function RoleCreateData:GetRecommendJob()
	return self.recommend_jobs
end

function RoleCreateData:ModelRotateShow(is_show, obj, parent, obj_p)
	is_show = is_show and self.create_view_data.is_show_prof
	self:ResetModelRotateInfo()
	if is_show then
		self.model_rotate_info = {
			ready = true,
			obj_t = obj.transform,
			obj_p = obj_p.transform,
			parent = parent.gameObject,
			root = parent.gameObject:GetLocalEulerAngles(),
		}
		obj.transform:SetParent(parent, true)
	end
	self.model_rotate:Set{is_show = is_show}
end

function RoleCreateData:ResetModelRotateInfo()
	local mri = self.model_rotate_info
	if mri.ready then
		mri.parent:SetLocalEulerAngles(mri.root)
		mri.obj_t:SetParent(mri.obj_p)
		mri.obj_t.gameObject:ResetTransform()
		self.model_rotate_info = {
			ready = false
		}
	end
end

function RoleCreateData:GetAvatarCanRandomChooseProf(avatar_type)
	local random_index = math.random(1, #self.chara_prof_list[avatar_type].prof_list)
	--LogError("random_index",random_index,self.chara_prof_list[avatar_type].prof_list[random_index], self.chara_prof_list[avatar_type])
	return self.chara_prof_list[avatar_type].prof_list[random_index].prof_id
end