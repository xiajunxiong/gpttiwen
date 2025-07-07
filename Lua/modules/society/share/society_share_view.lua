
SocietyShareView = SocietyShareView or DeclareView("SocietyShareView", SocietyConfig.ResPath .. "society_share")

function SocietyShareView:SocietyShareView()
	self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society
end

function SocietyShareView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(), self.language.SocietyShare, Vector2.New(565, 606))
end

SocietyShareViewF = SocietyShareViewF or DeclareMono("SocietyShareViewF", UIWFlushPanel)

function SocietyShareViewF:SocietyShareViewF()
	self.data = SocietyCtrl.Instance:Data()
	self.language = Language.GodLandTraces
end

function SocietyShareViewF:Awake()
	UIWFlushPanel.Awake(self)

	self:FlushShow()
end

function SocietyShareViewF:FlushShow()
	local role_list = self.data:GetFriendListByType(SocietyConfig.FriendGroupType.friend, true)
	self.EmptyObj:SetActive(#role_list == 0)
    self.RoleList:SetData(role_list)
end

function SocietyShareViewF:OnClickAddFriend()
	ViewMgr:OpenView(SocietyViewFriendAdd)
end

SocietyShareViewItem = DeclareMono("SocietyShareViewItem", UIWidgetBaseItem)

function SocietyShareViewItem:SocietyShareViewItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyShareViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data.info
	self.Name.text = info.name
	self.Level.text = "Lv." .. info.level
	UH.SetAvatar(self.IconSp, info.avatar_type, info.avatar_id,info.avatar_quality)
end

function SocietyShareViewItem:OnClickShare()
	self.society_data:SendShareText(self.data)
	ViewMgr:CloseView(SocietyShareView)
end