
SocietyGiftView = SocietyGiftView or DeclareView("SocietyGiftView", SocietyConfig.ResPath .. "society_gift")
VIEW_DECLARE_MASK(SocietyGiftView,ViewMask.BgBlockCloseNoAnim)

function SocietyGiftView:SocietyGiftView()
	self.data = SocietyCtrl.Instance:Data()

	self.anim_opened = self.ShowAnim:ListenEvent("end", function ()
		self.BtnOpenObj:SetActive(false)
		self.GiftObj:SetActive(true)
	end)
end

function SocietyGiftView:LoadCallback(param_t)
	local gi = self.data.gift_info_sm
	local info = self.data.get_gift_info[gi.uuid_key]
	if info then
		local from_role_id = info.from_role_id
		if RoleData.Instance:GetRoleId() == info.from_role_id or SocietyConfig.GiftMsgType.fetch == gi.state or info.fetch_time > 0 then
			self.ShowAnim:SetTrigger(APH("opened"))
		end
	end
end

function SocietyGiftView:CloseCallback()
	self.ShowAnim:UnlistenEvent("end", self.anim_opened)
end

function SocietyGiftView:OnClickOpen()
	self.ShowAnim:SetTrigger(APH("open"))
	local info = self.data.get_gift_info[self.data.gift_info_sm.uuid_key]
	if info then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
		SocietyCtrl.Instance:SendGiftReqFetch(info.uuid)
	end
end

SocietyGiftViewF = SocietyGiftViewF or DeclareMono("SocietyGiftViewF", UIWFlushPanel)

function SocietyGiftViewF:SocietyGiftViewF()
	self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society.Share

	self.data_cares = {
		{data = self.data.gift_info_sm, func = self.FlushShow},
		{data = self.data.send_info_sm, func = self.FlushState, keys = {"flush_msg"}, init_call = false},
	}
end

function SocietyGiftViewF:FlushShow()
	local gi = self.data.gift_info_sm
	local info = self.data.get_gift_info[gi.uuid_key]
	if info then
		local from_role_id = info.from_role_id
		local avatar_type = 0
		local avatar_id = 0
		local name = ""
		local is_self = RoleData.Instance:GetRoleId() == from_role_id
		if is_self then
			avatar_type = RoleData.Instance:GetRoleAvatarType()
			avatar_id = RoleData.Instance:GetRoleAvatarId()
			avatar_quality = RoleData.Instance:GetRoleAvatarQuality()
			name = RoleData.Instance:GetRoleName()
		else
			local friend = self.data.friend_list[from_role_id]
			if friend then
				avatar_type = friend.info.avatar_type
				avatar_quality = friend.info.avatar_quality
				avatar_id = friend.info.avatar_id
				name = friend.info.name
			end
		end
		self.Cell:SetData(info.item)
		UH.SetAvatar(self.IconSp, avatar_type, avatar_id,avatar_quality)
		self.Name.text = name
		self.SendObj:SetActive(not is_self)

		self.GetShow.text = 0 == info.fetch_time and self.language.NotGet or self.language.Geted
		self.SendShow.text = is_self and self.language.SendGift or self.language.GetGift
	end
end

function SocietyGiftViewF:FlushState()
	local gi = self.data.gift_info_sm
	local info = self.data.get_gift_info[gi.uuid_key]
	if info then
		self.GetShow.text = 0 == info.fetch_time and self.language.NotGet or self.language.Geted
	end
end