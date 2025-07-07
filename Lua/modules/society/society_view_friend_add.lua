
SocietyViewFriendAdd = SocietyViewFriendAdd or DeclareView("SocietyViewFriendAdd", SocietyConfig.ResPath .. "society_friend_add")

function SocietyViewFriendAdd:SocietyViewFriendAdd()
    self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society
end

function SocietyViewFriendAdd:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.SocietyFriendAdd, Vector2.New(905, 602))
end

function SocietyViewFriendAdd:OnClickSearch()
    local str = self.InputField.text
    if StringIsEmpty(str) then
        PublicPopupCtrl.Instance:Center(self.language.CommonTips.InputNameId)
        return
    end
    SocietyCtrl.Instance:SendFuzzySearchRoleByName(str)
end

function SocietyViewFriendAdd:OnClickMore()
    -- self.data:FriendAddListEmpty()
    ViewMgr:FlushView(SocietyViewFriendAdd)
    SocietyCtrl.Instance:SendRefreshFriendRecommend()
end

function SocietyViewFriendAdd:OnClickApply()
    -- PublicPopupCtrl.Instance:Center(self.language.CommonTips.OneKeyApplySend)
    self.data:FriendAddListEmpty()
    ViewMgr:FlushView(SocietyViewFriendAdd)
    SocietyCtrl.Instance:SendFriendAddList(true)
end

SocietyViewFriendAddList = SocietyViewFriendAddList or DeclareMono("SocietyViewFriendAddList", UIWFlushPanel)

function SocietyViewFriendAddList:SocietyViewFriendAddList()
    self.flush_events = {EventProtocol.ToEventKey(SCFriendRecommendList), EventProtocol.ToEventKey(SCFuzzySearchRolesInfo)}

	self.data = SocietyCtrl.Instance:Data()
end

function SocietyViewFriendAddList:onFlush()
    local role_list = self.data:GetFriendAddList()
    self.RoleList:SetData(role_list)
    self.EmptyObj:SetActive(0 == #role_list)
end

SocietyViewFriendAddItem = DeclareMono("SocietyViewFriendAddItem", UIWidgetBaseItem)

function SocietyViewFriendAddItem:SocietyViewFriendAddItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyViewFriendAddItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    local info = data.info
    local prof_sp = RoleData.GetProfSp(info.prof)
    local role_level = RoleData.Instance:GetRoleLevel()
	UH.SetText(self.Name, info.name)
	UH.SpriteName(self.ProfI, prof_sp)
    self.ProfI.gameObject:SetActive(nil ~= prof_sp)
    self.RecoObj:SetActive(math.abs(info.level - role_level) <= 5)
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

function SocietyViewFriendAddItem:OnClickAdd()
    local info = self.data.info
    self.society_data:FriendAddRemoveByUid(info.uid)
    ViewMgr:FlushView(SocietyViewFriendAdd)
    SocietyCtrl.Instance:SendFriendAdd(info.uid)
end