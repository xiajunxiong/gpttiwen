
SocietyViewRank = SocietyViewRank or DeclareMono("SocietyViewRank", UIWFlushPanel)

function SocietyViewRank:SocietyViewRank()
	self.data = SocietyCtrl.Instance:Data()
	self.data:ResetRankSM()
	
	self.data_cares = {
		{data = self.data.rank_sm, func = self.FlushRankTitle, keys = {"title_type"}},
		{data = self.data.rank_sm, func = self.FlushRankList, keys = {"type", "server"}, init_call = false},
		{data = self.data.rank_flush, func = self.FlushRankList, keys = {"list"}, init_call = false},
		{data = self.data.rank_sm, func = BindTool.Bind(SocietyCtrl.Instance.SendGetRankList, SocietyCtrl.Instance), keys = {"type", "server"}},
		{data = self.data.rank_refresh_time, func = self.FlushTime, init_call = true}
	}
end

function SocietyViewRank:Awake()
	UIWFlushPanel.Awake(self)
	self:FlushTypeList()
	self:FlushServerList()
end

function SocietyViewRank:FlushTypeList()
	self.TypeList:SetDataList(self.data:RankTypList())
end

function SocietyViewRank:FlushServerList()
	self.ServerList:SetDataList(self.data:RankServerList(), 2)
end

function SocietyViewRank:FlushRankTitle()
	local value = self.data.rank_sm.title_type % 2 + 1
	for i = 1, self.TitleTypes:Length() do
		self.TitleTypes[i]:SetActive(value == i)
		self.ListTypeObjs[i]:SetActive(value == i)
	end
	if 2 == value then self.TitleRoleValue.text = Language.Society.Rank.ValueNames[self.data.rank_sm.title_type] end
end

function SocietyViewRank:FlushRankList()
	local value = self.data.rank_sm.title_type % 2 + 1
	self.ListTypes[value]:SetData(self.data:GetRankInfos())
	self.ListTypes[value]:JumpToTop()
	local show_m, info_m = self.data:GetRankInfoM()
	self.ListTypeItems[value]:SetObjActive(show_m)
	if show_m and info_m then self.ListTypeItems[value]:SetData(info_m) end
end

function SocietyViewRank:FlushTime()
	self.UpdateTime:CloseCountDownTime()
	self.UpdateTime:TotalTime(self.data:GetRankRefreshTime(),TimeType.Type_Special_0)
	self.UpdateTime:SetCallBack(function ()
		SocietyCtrl.Instance:SendRankRefreshTimeReq()
		SocietyCtrl.Instance:SendGetRankList()
	end)
end
function SocietyViewRank:OnDestroy()
	self.UpdateTime:CloseCountDownTime()
	UIWFlushPanel.OnDestroy(self)
end

function SocietyViewRank:OnTipsClick()
	PublicPopupCtrl.Instance:HelpTip(337)
end


SocietyViewRankTypeItem = DeclareMono("SocietyViewRankTypeItem", UIWidgetBaseItem)

function SocietyViewRankTypeItem:SocietyViewRankTypeItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyViewRankTypeItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.TypeName.text = data.type_name
end

function SocietyViewRankTypeItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function SocietyViewRankTypeItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
	if selected then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
		self.society_data.rank_sm.type = self.data.type
		self.society_data.rank_sm.title_type = self.data.title_type
		-- SocietyCtrl.Instance:SendGetRankList()
    end
end

SocietyViewRankServerItem = DeclareMono("SocietyViewRankServerItem", UIWidgetBaseItem)

function SocietyViewRankServerItem:SocietyViewRankServerItem()
	self.society_data = SocietyCtrl.Instance:Data()
	self.language = Language.Society.Rank.ServerNames
end

function SocietyViewRankServerItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Server.text = string.format(self.language.Server, data.plat_type and data.server_id or self.language[data.server_id])
end

function SocietyViewRankServerItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function SocietyViewRankServerItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
	if selected then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
		self.society_data.rank_sm.server = self.data
    end
end

SocietyViewRankRoleItem = DeclareMono("SocietyViewRankRoleItem", UIWidgetBaseItem)

function SocietyViewRankRoleItem:SocietyViewRankRoleItem()
	self.society_data = SocietyCtrl.Instance:Data()
	self.info_data = RoleInfoCtrl.Instance:Data()
	self.language = Language.Society.Rank
end

function SocietyViewRankRoleItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local role = data.role
	local prof_b = RoleData.ProfIdToProfType(role.prof)
	self.RankSp.SpriteName = SocietyConfig.RankSp[data.rank] or SocietyConfig.RankSp.default
	if self.IsDown ~= nil and self.IsDown == true then
		self.RankSp.gameObject:SetActive(data.rank <= 3)
	end
	self.Rank.text = data.rank > 3 and data.rank or ""
	self.Name.text = role.name
	self.ProfSp.SpriteName = RoleData.GetProfSp(prof_b)
	if role.uid == RoleData.Instance:GetRoleId() then
		UH.SetAvatar(self.IconSp, RoleData.Instance:GetRoleAvatarType(),RoleData.Instance:GetRoleAvatarId(),RoleData.Instance:GetRoleAvatarQuality())
	else
		UH.SetAvatar(self.IconSp, role.avatar_type,role.avatar_id,role.avatar_quality)
	end
	self.PeakLevel:SetActive(role.top_level and role.top_level > 0)
	AvatarEffect.SetImageEnabled(self.PeakImg,role.avatar_quality)

	if self.FlyBgSub then
		self.FlyBgSub:SetData(role)
	end
	if self.society_data.rank_sm.title_type == 3 and role.top_level > 0 then
		UH.SetText(self.Value, string.format(Language.PeakRoad.Level2, role.top_level))
	else
		self.Value.text = data.rank_value
	end
end

function SocietyViewRankRoleItem:OnClickDetails()
	local uid = self.data.role.uid
	self.info_data.view_equip_uid = uid
	RoleInfoCtrl.Instance:SendReferRoleEquipment(uid)
end

SocietyViewRankRoleItemM = DeclareMono("SocietyViewRankRoleItemM", SocietyViewRankRoleItem)

function SocietyViewRankRoleItemM:SetData(data)
	SocietyViewRankRoleItem.SetData(self, data)
	self.RankImage.enabled = data.rank > 0
	if 0 == data.rank or data.rank > 999 then self.Rank.text = RichTextHelper.SizeStr(RichTextHelper.ColorStr(self.language.NoRank, COLORSTR.Red1), 22) end
end

SocietyViewRankPetItem = DeclareMono("SocietyViewRankPetItem", UIWidgetBaseItem)
--PetHeadCell==========UIWSubstitute[UIWPetHeadCell]
--RoleFlyBgSub=========UIWSubstitute[UIWFlyBg]
function SocietyViewRankPetItem:SocietyViewRankPetItem()
	self.society_data = SocietyCtrl.Instance:Data()
	self.info_data = RoleInfoCtrl.Instance:Data()
	self.language = Language.Society.Rank
end

function SocietyViewRankPetItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local role = data.role
	local pet = data.pet
	self.RankSp.SpriteName = SocietyConfig.RankSp[data.rank] or SocietyConfig.RankSp.default
	if self.IsDown ~= nil and self.IsDown == true then
		self.RankSp.gameObject:SetActive(data.rank <= 3)
	end
	if self.FlyBgSub then
		self.FlyBgSub:SetData(pet)
	end
	if self.RoleFlyBgSub then
		self.RoleFlyBgSub:SetData(role)
	end
	self.Rank.text = data.rank > 3 and data.rank or ""
	self.RoleName.text = role.name
	if pet.lian_yao_pet_id then
		UH.Color(self.PetName, pet.lian_yao_pet_id > 0 and COLORS.Pink2 or COLORS.Yellow9)
		if self.YaoObj then
			self.YaoObj:SetActive(pet.lian_yao_pet_id > 0)
		end
	end
	self.PetName.text = pet.name
	if role.uid == RoleData.Instance:GetRoleId() then
		UH.SetAvatar(self.IconSp, RoleData.Instance:GetRoleAvatarType(),RoleData.Instance:GetRoleAvatarId(),RoleData.Instance:GetRoleAvatarQuality())
	else
		UH.SetAvatar(self.IconSp, role.avatar_type,role.avatar_id,role.avatar_quality)
	end
	-- self.PetCell:SetData(pet)
	-- UH.SetIcon(self.PetIcon, pet.icon_id)
	-- UH.SpriteName(self.CatchIcon, pet.qua_icon)

	self.Value.text = data.rank_value
	-- UH.SetText(self.Level, pet.level)
    -- PetData.SetStrLevel(nil, self.StrLevel, pet.str_level)
	self.PetHeadCell:SetData({
        pet_id = pet.pet_id,
        lv = pet.level,
        fs_lv = pet.fly_flag,
        str_lv = pet.str_level,
        skin_id = pet.skin_id,
    })
end

function SocietyViewRankPetItem:OnClickDetails()
	-- ViewMgr:OpenView(PetDetailView, self.data.pet)
	if nil ~= self.data.details_func then
		Call(self.data.details_func)
	end
end

SocietyViewRankPetItemM = DeclareMono("SocietyViewRankPetItemM", SocietyViewRankPetItem)

function SocietyViewRankPetItemM:SetData(data)
	SocietyViewRankPetItem.SetData(self, data)
	
	self.RankImage.enabled = data.rank > 0
	if 0 == data.rank or data.rank > 999 then self.Rank.text = RichTextHelper.SizeStr(RichTextHelper.ColorStr(self.language.NoRank, COLORSTR.Red1), 22) end
end