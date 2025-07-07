
MarryPartyFightView = MarryPartyFightView or DeclareView("MarryPartyFightView", MarryPartyConfig.ResPath .. "marry_party_fight")
VIEW_DECLARE_LEVEL(MarryPartyFightView, ViewLevel.L1)

function MarryPartyFightView:LoadCallback()
end

function MarryPartyFightView:OnClickClose()
    -- ViewMgr:CloseView(MarryPartyFightView)
    MarryPartyCtrl.Instance:SendQingYuanShengHuiMatchClose()
end

MarryPartyFightViewF = MarryPartyFightViewF or DeclareMono("MarryPartyFightViewF", UIWFlushPanel)

function MarryPartyFightViewF:MarryPartyFightViewF()
    self.data = MarryPartyCtrl.Instance:Data()
    self.language = Language.MarryParty.Fight

    self.data_cares = {
		{data = self.data.marry_party_scene_info, func = self.FlushSceneInfo},
		{data = self.data.marry_party_role_info, func = self.FlushRoleInfo},
		{data = self.data.marry_party_fight_info.role_infos, func = self.FlushFightRoleInfos, init_call = false},
		{data = self.data.marry_party_fight_info.route, func = self.FlushFightRoute},
    }
end

function MarryPartyFightViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    TimeHelper:CancelTimer(self.timer_ct_link)
    TimeHelper:CancelTimer(self.timer_tt_match)
    -- TimeHelper:CancelTimer(self.timer_dt)
end

function MarryPartyFightViewF:Awake()
    self:FlushShow()

    UIWFlushPanel.Awake(self)
end

function MarryPartyFightViewF:FlushShow()
    local members = self.data:GetMemberRoleList()
    for i = 1, self.MemberItems1:Length() do
        self.MemberItems1[i]:SetData(members[i])
    end
    for i = 1, self.MemberItems2:Length() do
        self.MemberItems2[i]:SetData(nil)
    end

    self.BtnMatchShow.text = self.language.MatchBegin
    self.MatchObj:SetActive(false)
end

function MarryPartyFightViewF:FlushSceneInfo()
    TimeHelper:CancelTimer(self.timer_ct_link)
    local info = self.data.marry_party_scene_info
    if info.init then return end
    if MarryPartyConfig.LinkType.fight == info.link_seq and info.link_end_time - TimeHelper.GetServerTime() then
        self.timer_ct_link = TimeHelper:AddCountDownCT(function ()
            local last_time = info.link_end_time - TimeHelper.GetServerTime()
            local ft = TimeHelper.FormatDHMS(last_time)
            self.LinkTimeShow.text = string.format(self.language.LinkTimeShow, ft.min, ft.s)
        end, function ()
            self.LinkTimeShow.text = string.format(self.language.LinkTimeShow, 0, 0)
        end, info.link_end_time)
    else
        self.LinkTimeShow.text = string.format(self.language.LinkTimeShow, 0, 0)
    end
end

function MarryPartyFightViewF:FlushRoleInfo()
    local info = self.data.marry_party_role_info
    if info.init then return end
    self.ProgressShow:SetProgress(math.min(0, info.battle_rewards_times - 1), 2)
end

function MarryPartyFightViewF:FlushFightRoleInfos()
    local role_infos = self.data.marry_party_fight_info.role_infos
    if role_infos.init then return end
    for i = 1, self.MemberItems2:Length() do
        self.MemberItems2[i]:SetData(role_infos[i])
    end
    -- self.timer_dt = TimeHelper:AddDelayTimer(function ()
    --     ViewMgr:CloseView(MarryPartyFightView)
    -- end, 1)
end

function MarryPartyFightViewF:FlushFightRoute()
    TimeHelper:CancelTimer(self.timer_tt_match)
    local route = self.data.marry_party_fight_info.route
    if route.init then return end
    local is_match = 1 == route.is_match
    self.BtnMatchShow.text = is_match and self.language.MatchEnd or self.language.MatchBegin
    self.MatchObj:SetActive(is_match)
    if is_match then
        self.timer_tt_match = TimeHelper:AddCountDownTT(function ()
            local last_time = TimeHelper.GetServerTime() - route.begin_time
            local ft = TimeHelper.FormatDHMS(last_time)
            self.MatchTimeShow.text = string.format(self.language.MatchTimeShow, ft.min, ft.s)
        end, function ()
            self.MatchTimeShow.text = string.format(self.language.MatchTimeShow, 0, 0)
        end, 99999)
    end
    -- self.timer_dt = TimeHelper:AddDelayTimer(function ()
    --     ViewMgr:CloseView(MarryPartyFightView)
    -- end, 1)
end

function MarryPartyFightViewF:OnClickTips()
    local co = self.data.config_game_link[MarryPartyConfig.LinkType.fight]
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[co and co.link_des or 1].desc})
end

function MarryPartyFightViewF:OnClickMatch()
    local route = self.data.marry_party_fight_info.route
    local is_match = 1 == route.is_match
    MarryPartyCtrl.Instance:SendQingYuanShengHuiMatch(not is_match)
end

function MarryPartyFightViewF:OnClickReward(times)
    MarryPartyCtrl.Instance:SendQingYuanShengHuiBattleReward(times)
end

MarryPartyFightViewMemberItem = MarryPartyFightViewMemberItem or DeclareMono("MarryPartyFightViewMemberItem", UIWidgetBaseItem)
function MarryPartyFightViewMemberItem:MarryPartyFightViewMemberItem()
    self.marry_data = MarryPartyCtrl.Instance:Data()
    self.language = Language.MarryParty.Fight
end

function MarryPartyFightViewMemberItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_empty = nil == data
    self.EmptyObj:SetActive(is_empty)
    self.InfoObj:SetActive(not is_empty)
    if not is_empty then
        local prof = RoleData.ProfIdToProfType(data.prof)
        self.NameShow.text = data.name
        self.LevelShow.text = string.format(self.language.LevelShow, data.level)
        UH.SetAvatar(self.IconSp, data.avatar_type, data.avatar_id)
	    self.ProfSp.SpriteName = RoleData.GetProfSp(prof)
    end
end