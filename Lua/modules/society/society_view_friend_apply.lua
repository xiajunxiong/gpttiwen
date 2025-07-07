
SocietyViewFriendApply = SocietyViewFriendApply or DeclareView("SocietyViewFriendApply", SocietyConfig.ResPath .. "society_friend_apply")

function SocietyViewFriendApply:SocietyViewFriendApply()
	self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society
end

function SocietyViewFriendApply:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.SocietyFriendApply, Vector2.New(905, 498))
end

function SocietyViewFriendApply:OnClickRefuse()
	local role_list = self.data:GetFriendApplyList()
	for _, v in pairs(role_list) do
		local info = v.info
		SocietyCtrl.Instance:SendFriendApply(info.uid, false)
	end
end

function SocietyViewFriendApply:OnClickAgree()
	local role_list = self.data:GetFriendApplyList()
	for _, v in pairs(role_list) do
		local info = v.info
		SocietyCtrl.Instance:SendFriendApply(info.uid, true)
	end
end

SocietyViewFriendApplyList = SocietyViewFriendApplyList or DeclareMono("SocietyViewFriendApplyList", UIWFlushPanel)

function SocietyViewFriendApplyList:SocietyViewFriendApplyList()
	self.flush_events = {EventProtocol.ToEventKey(SCAddFriendRoute)}

	self.data = SocietyCtrl.Instance:Data()
end

function SocietyViewFriendApplyList:onFlush()
    local role_list = self.data:GetFriendApplyList()
    self.RoleList:SetData(role_list)
end

SocietyViewFriendApplyItem = DeclareMono("SocietyViewFriendApplyItem", UIWidgetBaseItem)

function SocietyViewFriendApplyItem:SocietyViewFriendApplyItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyViewFriendApplyItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data.info
	local prof_sp = RoleData.GetProfSp(info.prof)
	UH.SetText(self.Name, info.name)
	UH.SpriteName(self.ProfI, prof_sp)
	self.ProfI.gameObject:SetActive(nil ~= prof_sp)
    UH.SetAvatar(self.IconSp, info.avatar_type,info.avatar_id,info.avatar_quality)
	if info.top_level and info.top_level > 0 then
		UH.SetText(self.Level, string.format(Language.PeakRoad.Level2, info.top_level))
		self.PeakLevel:SetActive(true)
	else
		UH.SetText(self.Level, "Lv." .. info.level)
		self.PeakLevel:SetActive(false)
	end
	AvatarEffect.SetImageEnabled(self.PeakImg,info.avatar_quality)
end

function SocietyViewFriendApplyItem:OnClickAgree()
	local info = self.data.info
	if nil ~= info then
		SocietyCtrl.Instance:SendFriendApply(info.uid ,true)
	end
end

function SocietyViewFriendApplyItem:OnClickRefuse()
	local info = self.data.info
	if nil ~= info then
		SocietyCtrl.Instance:SendFriendApply(info.uid ,false)
	end
end