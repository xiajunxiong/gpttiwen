AppearanceData = AppearanceData or BaseClass()

function AppearanceData:__init()
	if AppearanceData.Instance ~= nil then
		Debuger.LogError("[AppearanceData] attempt to create singleton twice!")
		return
	end
	AppearanceData.Instance = self
	self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,BindTool.Bind(self.RoleNoticeComplete, self))
    self.profession_handle = EventSys:Bind(GlobalEventKeys.RoleProfessionChange,BindTool.Bind(self.CheckAppearanceData, self))
	self:Initialization()
	self:InitAppearanceData()
end

function AppearanceData:__delete()
	AppearanceData.Instance = nil
end

function AppearanceData:Initialization()
	self.select_data = SmartData.New({type = -1,data = {}})
end

function AppearanceData:RoleNoticeComplete()
	self:CheckAppearanceData()
	self:CheckRemindTipsFlag()
end

function AppearanceData:InitAppearanceData()
	self.face_add_config = {}
	for k,v in pairs(Config.face_cfg_auto.level) do
		if self.face_add_config[v.level] == nil then
			self.face_add_config[v.level] = {}
		end
		table.insert(self.face_add_config[v.level],{
			job = v.job,
			exp = v.exp,
			add_list = DataHelper.FormAttrList(v)
		})
	end
	self.appearance_info = SmartData.New{face_level = 0,cur_exp = 0,equip_list = {},active_flag = {}}
	self.appearance_red_piont = SmartData.New({val = false})

	self.tail_layout_list = {}
	for k, v in pairs(Config.face_cfg_auto.tail_layout) do
		local layout = string.split(v.res_layout, "|")
		self.tail_layout_list[v.res_id] = {
			res_id = v.res_id,
			w = layout[1],
			h = layout[2],
		}
	end
	self.stamp_data = SmartData.New({list = {}})

	self.remind_tips_list = {}
	self.remind_type_list = {[0] = {},{},{}}
	self.remind_red_point = SmartData.New({val = false})
	self.title_red_point = SmartData.New({val = false})
	self.tail_tag_red_point = SmartData.New({val = false})
	self.bubble_red_point = SmartData.New({val = false})
end

function AppearanceData:CheckAppearanceData()
	self.face_resource_list = {[0] = {},{},{}}
	local prof = RoleData.Instance:GetRoleProf()
	for k,v in pairs(Config.face_cfg_auto.resource) do
		if v.is_show == 1 then
			if v.limit_prof ~= 0 then 
				if v.limit_prof == prof then
					table.insert(self.face_resource_list[v.res_type],v)
				end
			elseif v.res_type == AppearanceConfig.Type.Title and not QQBigPlayerCtrl.IsQQPC() then --QQPC包之外的包不显示QQ包中的特殊称号
				if v.title_id < 509 or v.title_id > 516 then
					table.insert(self.face_resource_list[v.res_type],v)
				end
			else
				table.insert(self.face_resource_list[v.res_type],v)
			end
		end
	end
	for k,v in pairs(self.face_resource_list) do
		table.sort(v,function(a,b)return a.sort < b.sort end)
	end
end

function AppearanceData:CheckRemindTipsFlag()
	for k,v in pairs(Config.face_cfg_auto.resource) do
		if v.is_show ~= 0 and v.remind_off == 0 then
			if UnityPlayerPrefs.GetInt(PrefKeys.AppearanceTips(v.seq)) == 1 then
				self:SetRemindTipsFlag(v.seq,true)
			end
		end
	end
end

function AppearanceData:SetSelectData(data)
	self.select_data.data = data
end

function AppearanceData:GetSelectData()
	return self.select_data.data
end

function AppearanceData:SetSelectType(type)
	self.select_data.type = type
end

function AppearanceData:GetSelectType()
	return self.select_data.type
end

----------------------服务端数据下发----------------------
function AppearanceData:SetFaceScoreAllInfo(protocol)
	self.appearance_info.face_level = protocol.face_level
	self.appearance_info.cur_exp = protocol.cur_exp
	self.appearance_info.equip_list = protocol.equip_list
	self.appearance_info.active_flag = protocol.active_flag
end

function AppearanceData:SetFaceListGridInfo(protocol)
	self.appearance_info.equip_list[protocol.res_type] = protocol.res_seq ~= -1 and Config.face_cfg_auto.resource[protocol.res_seq] or nil
	self.appearance_red_piont.val = not self.appearance_red_piont.val
end

function AppearanceData:SetFaceResStateInfo(protocol)
	if protocol.status == 1 and self:IsRemindTips(protocol.res_seq) and self:GetActiveFlag(protocol.res_seq) == 0 then
		self:SetRemindTipsFlag(protocol.res_seq,true)
	end
	self.appearance_info.active_flag[protocol.res_seq] = protocol.status
end

function AppearanceData:SetFaceLevelAndExps(protocol)
	self.appearance_info.face_level = protocol.face_level
	self.appearance_info.cur_exp = protocol.cur_exp
end

function AppearanceData:SetFaceTimeAllInfo(protocol)
	self.stamp_data.list = protocol.stamp_list
end

function AppearanceData:SetFaceTimeSingleInfo(protocol)
	local stamp_time = protocol.stamp_time
	self.stamp_data.list[protocol.seq] = stamp_time ~= 0 and stamp_time or nil
end

--获取总加成列表
function AppearanceData:GetMainAddList()
	return self.face_add_list
end

--获取颜值下发数据
function AppearanceData:GetAppearancInfo()
	return self.appearance_info
end

--获取装备
function AppearanceData:GetEquipData(res_type)
	return self.appearance_info.equip_list[res_type]
end

-- -1 没有穿戴任何东西
function AppearanceData:GetEquipResSeq(res_type)
	if self:GetEquipData(res_type) then
		return self:GetEquipData(res_type).seq or 0
	end
	return -1
end

function AppearanceData:IsEquipData(res_type,seq)
	local config = self.appearance_info.equip_list[res_type]
	if config then
		return config.seq == seq
	end
	return false
end

--获取激活
function AppearanceData:GetActiveFlag(seq)
	return self.appearance_info.active_flag[seq or 0] or 0
end

--获取颜值配置
function AppearanceData:GetAppearancList(res_type,type)
	local list = {}
	for i,v in ipairs(self.face_resource_list[res_type or 0] or {}) do
		if type == 1 then
			table.insert(list,v)
		else
			if self:GetActiveFlag(v.seq) == 1 then
				table.insert(list,v)
			end
		end
	end
	return list
end

function AppearanceData:GetAppearancTitleList(type)
	local cfg = self.face_resource_list[AppearanceConfig.Type.Title] or {}
	local list = {}
	for i,v in pairs(cfg) do
		local title_cfg = TitleData.GetTitleConfig(v.title_id)
		if title_cfg and title_cfg.title_type == type then
			table.insert(list, v)
		end
	end

	table.sort(list, function (a,b)
		local a_num = self:GetActiveFlag(a.seq)
		local b_num = self:GetActiveFlag(b.seq)
		if a_num == b_num then
			a_num = self:IsEquipData(AppearanceConfig.Type.Title,a.seq) and 100 or 0
			b_num = self:IsEquipData(AppearanceConfig.Type.Title,b.seq) and 100 or 0
			if a_num == b_num then 
				return a.sort < b.sort
			end
			return a_num > b_num
		end
		return a_num > b_num
	end)
	return list
end

function AppearanceData:GetTitleConfig(title_id)
	local res_type = AppearanceConfig.Type.Title
	for k,v in pairs(self.face_resource_list[res_type] or {}) do
		if v.title_id == title_id then
			return v
		end
	end
end

function AppearanceData:GetConfigByResId(res_type,res_id)
	for k,v in pairs(self.face_resource_list[res_type] or {}) do
		if v.res_id == res_id then
			return v
		end
	end
end

function AppearanceData:GetCurAddData(level)
	local prof = RoleData.Instance:GetRoleProf()
	for k,v in pairs(self.face_add_config[level] or {}) do
		if v.job == prof then
			return v
		end
	end
end

function AppearanceData:GetAddList(level)
	return self.face_add_config[level] or {}
end

--获取道具时间戳
function AppearanceData:GetFaceStampTime(seq)
	return self.stamp_data.list[seq] or 0
end

--是否提醒
function AppearanceData:IsRemindTips(seq)
	local config = Config.face_cfg_auto.resource[seq]
	return config and config.remind_off == 0 or false
end

--获取资源类型
function AppearanceData:GetType(seq)
	return Config.face_cfg_auto.resource[seq].res_type or 0
end

function AppearanceData:SetRemindTipsFlag(seq,flag)
	self:SetRedPointNum(seq,flag)
    self.remind_tips_list[seq] = flag
    UnityPlayerPrefs.SetInt(PrefKeys.AppearanceTips(seq),flag and 1 or 0)
end

function AppearanceData:GetRemindTipsFlag(seq)
    return self.remind_tips_list[seq]
end

function AppearanceData:SetRedPointNum(seq,flag)
	local type = self:GetType(seq)
	self.remind_type_list[type][seq] = flag or nil
	local red_pre = AppearanceConfig.IconPre[type].."red_point"
	self[red_pre].val = self:IsHasRedPoint(type)
	self.remind_red_point.val = not self.remind_red_point.val
end

function AppearanceData:IsHasRedPoint(type)
	for k,v in pairs(self.remind_type_list[type]) do
		if v == true then
			return true
		end
	end
	return false
end

--红点
function AppearanceData:GetBubbleRedPointNum()
	return self.bubble_red_point.val and 1 or 0
end

function AppearanceData:GetTitleRedPointNum()
	return self.title_red_point.val and 1 or 0
end

function AppearanceData:GetTailTagRedPointNum()
	return self.tail_tag_red_point.val and 1 or 0
end

function AppearanceData:GetRedPointNum()
	return self:GetBubbleRedPointNum() + self:GetTitleRedPointNum() + self:GetTailTagRedPointNum()
end

--关闭之后所有的红点消失
function AppearanceData:ClearRedPoint()
	for k,v in pairs(self.remind_tips_list) do
		UnityPlayerPrefs.SetInt(PrefKeys.AppearanceTips(k),0)
	end
	self.remind_tips_list = {}
	self.remind_type_list = {[0] = {},{},{}}
	self.bubble_red_point.val = false
	self.title_red_point.val = false
	self.tail_tag_red_point.val = false
	self.remind_red_point.val = not self.remind_red_point.val
end