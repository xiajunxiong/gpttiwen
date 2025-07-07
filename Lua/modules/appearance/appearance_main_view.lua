AppearanceMainView = AppearanceMainView or DeclareMono("AppearanceMainView", UIWFlushPanel)

function AppearanceMainView:AppearanceMainView()
	self.data = AppearanceData.Instance
    self.data_cares = {
        {data = self.data.appearance_red_piont, func = self.FlushAppearance},
    }
end

function AppearanceMainView:Awake()
	UIWFlushPanel.Awake(self)
	self.ui_obj = UIChDrawer.CopyMainRoleData()
    self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
end

function AppearanceMainView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self,"ui_obj")
end

function AppearanceMainView:FlushAppearance()
	local info = self.data:GetAppearancInfo()
	local face_level = info.face_level
	for i=1,GameEnum.APPEARANCE_MAX_LEVEL_NUM do
		self.StarList[i]:SetActive(i <= face_level)
	end
	for i=1,self.CallList:Length() do
		self.CallList[i]:SetData({res_type = i-1})
	end
	UH.SetText(self.Level,Format(Language.Appearance.Level,face_level))
	local config = self.data:GetCurAddData(face_level)
	self.Progress:SetProgress(info.cur_exp/config.exp)
	self.Progress:SetText(Format("%s/%s",info.cur_exp,config.exp))
	self:FlushAddView(config)
end

function AppearanceMainView:FlushAddView(data)
	if data ~= nil then
		local add_name = ""
		local attr_num = #data.add_list
		local next_name = ""
		for i,v in ipairs(data.add_list) do
			local attr_name = v.attr_name .. "+" .. v.attr_value
			if i <= 3 then
				add_name = add_name .. "        " .. attr_name
			else
				next_name = next_name .. "        " .. attr_name
			end
		end
		self.AddImage:SetActive(attr_num > 3)
		self.NotAddTip:SetActive(add_name == "")
		UH.SetText(self.NextName,next_name)
		UH.SetText(self.AddName,add_name)
	end
end

function AppearanceMainView:OnClickHelp()
	ViewMgr:OpenView(AppearancePopupView)
end

----------------------------AppearanceMainAddItem----------------------------
AppearanceMainAddItem = AppearanceMainAddItem or DeclareMono("AppearanceMainAddItem", UIWidgetBaseItem)

function AppearanceMainAddItem:SetData(data)
	if data.add_name then
		UH.SetText(self.AddName,data.add_name)
	else
		local add_name = ""
		for i,v in ipairs(data.add_list) do
			local attr_name = v.attr_name .. "+" .. v.attr_value
			add_name = add_name .. attr_name
		end
		UH.SetText(self.AddName,add_name)
	end
	UH.SetText(self.Level,data.level .. Language.Common.Level)
	UH.SpriteName(self.Image,data.level % 2 == 0 and "Tiao2" or "Tiao1")
	self.Toggle.isOn = data.level == AppearanceData.Instance:GetAppearancInfo().face_level
end

----------------------------AppearanceMainItem----------------------------
AppearanceMainItem = AppearanceMainItem or DeclareMono("AppearanceMainItem", UIWidgetBaseItem)

function AppearanceMainItem:SetData(data)
	local config = AppearanceData.Instance:GetEquipData(data.res_type)
	if config ~= nil then
		if self.Icon then
			if config.res_id ~= 0 then
				local pre = AppearanceConfig.IconPre[data.res_type]
				local icon_type = AppearanceConfig.IconType[data.res_type]
				UH.SetIcon(self.Icon,pre .. config.res_id,icon_type)
				if self.Title then
					UH.SetText(self.Title,"")
				end
			else
				UH.SetIcon(self.Icon,nil)
				UH.SetText(self.Title,TitleData.GetQuaName(config.title_id))
			end
		else
			self.Bubble:SetData(config.res_id,Vector2.New(174,65))
		end
		if self.Icon4 then
			if TitleData.IsGuildTitle(config.title_id) then
				UH.SetText(self.TitleName2, config.name)
				UH.SetText(self.Title, "")
				self.Icon4:SetActive(true)
			else
				self.Icon4:SetActive(false)
			end
			if TitleData.IsGuildTitle(config.title_id) and GuildData.Instance:GetGuildID() > 0 then
				UH.SetText(self.TitleName2, string.format(Language.Guild.TitleName, GuildData.Instance:GetGuildName() , GuildData.Instance:GetJobName(GuildData.Instance:GetMemberVo().post)))
			end
		end
		self.IconObj:SetActive(true)
		UH.SetText(self.Tips,"")
	else
		local type_name = Language.Appearance.TypeName[data.res_type]
		UH.SetText(self.Tips,"未穿戴")
		self.IconObj:SetActive(false)
	end
end