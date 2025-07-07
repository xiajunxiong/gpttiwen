
ChallengeTestView = ChallengeTestView or DeclareView("ChallengeTestView", ChallengeTestConfig.ResPath .. "challenge_test", Mod.Experience.ChallengeTest)
VIEW_DECLARE_LEVEL(ChallengeTestView,ViewLevel.L1)

function ChallengeTestView:ChallengeTestView()
	self.data = ChallengeTestCtrl.Instance:Data()
	self.language = Language.ChallengeTest

	self.anim_handle = self.BgAnim:ListenEvent(
		"view_state",
		function(eve_param, anim_st_info)
			if eve_param == "showed" then
				self.Canvas4:SetActive(true)
			end
		end
	)
end

function ChallengeTestView:LoadCallback(param_t)
	param_t = param_t or {}
	if param_t.is_fail then
		if 0 == GuideData.Instance:GetGuideFlag(FuncGuideFlagType.FUNC_GUIDE_LUNHUI_WANGCHUAN) then
			GuideManager.Instance:Start(90)
			RoleCtrl.Instance:SendFuncGuideOperator(FUNCTIONGUIDEOPTYPE.LUNHUI_WANGCHUAN)
		end
	end
end

function ChallengeTestView:CloseCallback()
	self.BgAnim:UnlistenEvent("view_state", self.anim_handle)
end

function ChallengeTestView:OnClickReturn()
	ViewMgr:CloseView(ChallengeTestView)
end

function ChallengeTestView:OnClickTips()
	local account_seq = self.data:AccountSeq()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[account_seq].desc})
end


ChallengeTestViewF = ChallengeTestViewF or DeclareMono("ChallengeTestViewF", UIWFlushPanel)

function ChallengeTestViewF:ChallengeTestViewF()
	self.data = ChallengeTestCtrl.Instance:Data()
	self.language = Language.ChallengeTest

	self.data_cares = {
		{data = self.data.info_sm, func = self.FlushShow},
		{data = self.data.show_butterfly, func = self.ShowButterfly, init_call = false},
	}
	self.is_init = true
	self.AutoToggle.isOn = 1 == UnityPlayerPrefs.GetInt(PrefKeys.ChallengeTestAuto(RoleData.Instance:GetRoleId()))
end

function ChallengeTestViewF:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.timer_ct_next)
	TimeHelper:CancelTimer(self.timer_dft)
end

function ChallengeTestViewF:FlushShow()
	local info_sm = self.data.info_sm

	TimeHelper:CancelTimer(self.timer_ct_next)
	self.timer_ct_next = TimeHelper:AddCountDownCT(function ()
		local next_time = info_sm.next_refresh_tamp - TimeHelper.GetServerTime()
		local ft = TimeHelper.FormatDHMS(next_time)
		self.RemainTime.text = string.format(self.language.TimeFormat, ft.day, ft.hour)
	end, function ()
		self.RemainTime.text = string.format(self.language.TimeFormat, 0, 0)
	end, info_sm.next_refresh_tamp)

	local details = self.data:GetInfoDetailByLayerId(info_sm.cur_layer, info_sm.battle_monster_id, info_sm.start_level)
	self.CurLayer.text = string.format(self.language.CurLayer, info_sm.cur_layer)
	self.MonsterName.text = details.name
	self.MonsterDesc.text = details.desc
	UH.SetIcon(self.MonsterIconSp, details.icon_id, ICON_TYPE.ITEM)
	if details.crystal > 0 then
        self.EleAttr:SetData(EleCrystal.GetElement(details.crystal))
    end
	self.EleAttr:SetObjActive(details.crystal > 0)
	self.EleAttrN:SetActive(0 == details.crystal)
	self.RewardsPass:SetData(details.rewards)

	local reward_list = self.data:GetLayerRewards(info_sm.start_level)
	if self.is_init then
		self.is_init = false
		self.timer_dft = TimeHelper:AddDelayFrameTimer(function ()
			for i = 1, self.RewardItems:Length() do
				self.RewardItems[i]:SetData(reward_list[i])
			end
		end)
	else
		for i = 1, self.RewardItems:Length() do
			self.RewardItems[i]:SetData(reward_list[i])
		end
	end
end

function ChallengeTestViewF:ShowButterfly()
	self.Canvas4:SetActive(false)
	self.Canvas4:SetActive(true)
end

function ChallengeTestViewF:OnClickFight()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.LoginStartClick)
	local info_sm = self.data.info_sm
	if info_sm.pass_layer == info_sm.cur_layer then
		PublicPopupCtrl.Instance:Center(self.language.PassTips)
		return
	end
	self:FightAnimStart()
    -- ChallengeTestCtrl.Instance:SendLunHuiWangChuanBattle()
end

function ChallengeTestViewF:FightAnimStart()
	self.EffectTool:Play(2161022)
	self.AspectRatio.enabled = false
	self.BgAnim:Play()
end

function ChallengeTestViewF:OnClickRank()
	ViewMgr:OpenView(ChallengeTestRankView)
end

function ChallengeTestViewF:OnClickFormation()
	ViewMgr:OpenView(FormationView)
end

function ChallengeTestViewF:OnEffectEnd()
	-- ViewMgr:CloseView(ChallengeTestView)
	ChallengeTestCtrl.Instance:SendLunHuiWangChuanBattle()
end

function ChallengeTestViewF:OnToggleAuto(value)
	if value then
		PublicPopupCtrl.Instance:Center(self.language.AutoFightTips)
	end
	UnityPlayerPrefs.SetInt(PrefKeys.ChallengeTestAuto(RoleData.Instance:GetRoleId()), self.AutoToggle.isOn and 1 or 0)
end


ChallengeTestViewRewardItem = DeclareMono("ChallengeTestViewRewardItem", UIWidgetBaseItem)

function ChallengeTestViewRewardItem:ChallengeTestViewRewardItem()
	self.challenge_test_data = ChallengeTestCtrl.Instance:Data()
	self.language = Language.ChallengeTest
	self.effect_handles = {}
end

function ChallengeTestViewRewardItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	if nil == data then return end
	local reward_flags = self.challenge_test_data.info_sm.reward_flags
	local pass_layer = self.challenge_test_data.info_sm.pass_layer
	local is_get = 1 == reward_flags[data.layer]
	
	if nil == data.rewards_show then
		self.challenge_test_data:GetLayerRewardsShow(data)
	end
	self.CircleSp.SpriteName = is_get and "YuanQuan1" or "YuanQuan2"
	self.LayerShow.text = RichTextHelper.ColorStr(string.format(self.language.LayerShow, data.layer), is_get and COLORSTR.Yellow12 or COLORSTR.Red13)
	self.LineObj:SetActive(data.layer < self.challenge_test_data.layer_max)
	self.IsGetObj:SetActive(is_get)
	local show_effect = not is_get and pass_layer >= self.data.layer
	self.BtnGetObj:SetActive(show_effect)
	for i = 1, self.RewardCells:Length() do
		local reward = data.rewards_show[i]
		self.RewardCells[i]:SetObjActive(nil ~= reward)
		if self.effect_handles[i] then
			self.UIEffectTools[i]:Stop(self.effect_handles[i])
			self.effect_handles[i] = nil
		end
		if reward then
			self.RewardCells[i]:SetData(reward)
			if show_effect then
				self.effect_handles[i] = self.UIEffectTools[i]:Play(2161004)
			end
		end
	end
	self.red_point_item = self.red_point_item or UiRedPoint.New(self.CircleSp.gameObject, Vector3.New(25,25,0))
	self.red_point_item:SetNum(show_effect and 1 or 0)
end

function ChallengeTestViewRewardItem:OnDestroy()
	for i = 1, self.RewardCells:Length() do
		if self.effect_handles[i] then
			self.UIEffectTools[i]:Stop(self.effect_handles[i])
			self.effect_handles[i] = nil
		end
	end
end

function ChallengeTestViewRewardItem:OnClickGet()
	ChallengeTestCtrl.Instance:SendLunHuiWangChuanReward(self.data.layer)
end