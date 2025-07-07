
ChallengeTestRankView = ChallengeTestRankView or DeclareView("ChallengeTestRankView", ChallengeTestConfig.ResPath .. "challenge_test_rank")
VIEW_DECLARE_MASK(ChallengeTestRankView, ViewMask.BgBlockNoAnim)

function ChallengeTestRankView:ChallengeTestRankView()
	self.data = ChallengeTestCtrl.Instance:Data()
	self.language = Language.ChallengeTest
end

function ChallengeTestRankView:LoadCallback()
	SocietyCtrl.Instance:SendGetPersonRankList(ChallengeTestConfig.RANK_TYPE)
end

function ChallengeTestRankView:OnClickClose()
	ViewMgr:CloseView(ChallengeTestRankView)
end

ChallengeTestRankViewF = ChallengeTestRankViewF or DeclareMono("ChallengeTestRankViewF", UIWFlushPanel)

function ChallengeTestRankViewF:ChallengeTestRankViewF()
	self.data = ChallengeTestCtrl.Instance:Data()
	self.language = Language.ChallengeTest

	self.data_cares = {
		{data = self.data.rank_list, func = self.FlushShow, init_call = false},
	}
end

function ChallengeTestRankViewF:FlushShow()
	self.RankList:SetData(self.data.rank_list:Val())
	self.RankInfoM:SetData(self.data.rank_info_m)
end


ChallengeTestRankViewItem = DeclareMono("ChallengeTestRankViewItem", UIWidgetBaseItem)

function ChallengeTestRankViewItem:ChallengeTestRankViewItem()
	self.language = Language.ChallengeTest
end

function ChallengeTestRankViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local role = data.role
	local prof_b = RoleData.ProfIdToProfType(role.prof)
	self.RankSp.SpriteName = SocietyConfig.RankSp[data.rank] or SocietyConfig.RankSp.default
	-- if self.IsDown ~= nil and self.IsDown == true then
	-- 	self.RankSp.gameObject:SetActive(data.rank <= 3)
	-- end
	self.Rank.text = data.rank > 3 and data.rank or ""
	self.Name.text = role.name
	if role.uid == RoleData.Instance:GetRoleId() then
		UH.SetAvatar(self.IconSp, RoleData.Instance:GetRoleAvatarType(),RoleData.Instance:GetRoleAvatarId(),RoleData.Instance:GetRoleAvatarQuality())
	else
		UH.SetAvatar(self.IconSp, role.avatar_type,role.avatar_id,role.avatar_quality)
	end
	self.Value.text = string.format(self.language.RankValue, data.rank_value)
end

ChallengeTestRankViewItemM = DeclareMono("ChallengeTestRankViewItemM", ChallengeTestRankViewItem)

function ChallengeTestRankViewItemM:SetData(data)
	ChallengeTestRankViewItem.SetData(self, data)
	self.RankImage.enabled = data.rank > 0 and data.rank <= 999
	if 0 == data.rank or data.rank > 999 then self.Rank.text = RichTextHelper.SizeStr(self.language.NoRank, 24) end
end