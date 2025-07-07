
MainDownWeddingFB = MainDownWeddingFB or DeclareMono("MainDownWeddingFB", UIWFlushPanel)

function MainDownWeddingFB:MainDownWeddingFB()
	self.data = MarryCtrl.Instance:Data()
    self.language = Language.Marry.WeddingFBMenu

    self.data_cares = {
		{data = self.data.wedding_scene_info, func = self.FlushInfo},
		{data = self.data.marry_invite_info, func = self.FlushRemind, keys = {"apply_list"}},
		{data = self.data.marry_invite_info, func = self.FlushRemind, keys = {"invite_list"}, init_call = false},
    }
end

function MainDownWeddingFB:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.timer_dt)
    TimeHelper:CancelTimer(self.timer_ct)
end

function MainDownWeddingFB:OnDisable()
    UIWFlushPanel.OnDisable(self)
	TimeHelper:CancelTimer(self.timer_dt)
    TimeHelper:CancelTimer(self.timer_ct)
end

function MainDownWeddingFB:FlushInfo()
    TimeHelper:CancelTimer(self.timer_ct)
	local info = self.data.wedding_scene_info
    if info.init then return end

	local heat_reward_max = self.data:GetWeddingFBHeatRewardMax()
	
	self.BtnInviteObj:SetActive(1 == info.is_host)
	self.ProgressShow:SetProgress(info.wedding_heat / heat_reward_max)
    self.ProgressShow:SetText(info.wedding_heat .. " / ".. heat_reward_max)

	local link_end_time = info.link_end_time
    if link_end_time > TimeHelper.GetServerTime() then
        self.ShowObj:SetActive(true)
        self.timer_ct = TimeHelper:AddCountDownCT(function ()
            local ft = TimeHelper.FormatDHMS(link_end_time - TimeHelper.GetServerTime())
		    self.RemainTime.text = string.format(self.language.TopRemainTime, ft.min, ft.s)
        end, function ()
            self.ShowObj:SetActive(false)
        end, link_end_time)
    else
        self.ShowObj:SetActive(false)
    end
    local co = MarryData.Instance.config_wedding_party[info.link_seq]
    self.TypeName.text = co and co.wedding_link or ""
end

function MainDownWeddingFB:FlushRemind()
	self.red_point = self.red_point or UiRedPoint.New(self.BtnInviteObj, Vector3.New(60,20,0))
	self.red_point:SetNum(self.data:WeddingApplyRedPoint() and 1 or 0)
end

function MainDownWeddingFB:OnClickBless()
	ViewMgr:OpenView(WeddingFBBlessView)
end

function MainDownWeddingFB:OnClickGame()
	ViewMgr:OpenView(WeddingFBGameView)
end

function MainDownWeddingFB:OnClickInvite()
	ViewMgr:OpenView(MarryInviteView)
end

function MainDownWeddingFB:OnClickTips()
    local info = self.data.wedding_scene_info
    if info.init then return end
    local co = MarryData.Instance.config_wedding_party[info.link_seq]
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[co and co.link_des or 1].desc})
end


local marry_party_btn_func = {
	[2] = function ()
        MarryPartyCtrl.Instance:SendQingYuanShengHuiQuestionOpen()
    end,
    [3] = function ()
        -- ViewMgr:OpenView(MarryPartyFightView)
        MarryPartyCtrl.Instance:SendQingYuanShengHuiMatchOpen()
    end,
    [4] = function ()
        ViewMgr:OpenView(MarryPartyBlessView)
    end,
}

MainDownMarryParty = MainDownMarryParty or DeclareMono("MainDownMarryParty", UIWFlushPanel)

function MainDownMarryParty:MainDownMarryParty()
	self.data = MarryPartyCtrl.Instance:Data()
    self.language = Language.MarryParty.Menu

    self.data_cares = {
		{data = self.data.marry_party_scene_info, func = self.FlushInfo},
    }
end

function MainDownMarryParty:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_ct)
end

function MainDownMarryParty:OnDisable()
    UIWFlushPanel.OnDisable(self)
    TimeHelper:CancelTimer(self.timer_ct)
end

function MainDownMarryParty:FlushInfo()
    TimeHelper:CancelTimer(self.timer_ct)
	local info = self.data.marry_party_scene_info
    if info.init then return end
	
    local btn_show = self.language.DownBtnShow
    local btn_name = btn_show[info.link_seq]
    self.BtnShow.text = btn_name or ""
	self.BtnShowObj:SetActive(nil ~= btn_name)

    local co_next = self.data.config_game_link[info.link_seq + 1]

	local link_end_time = info.link_end_time
    if link_end_time > TimeHelper.GetServerTime() then
        self.timer_ct = TimeHelper:AddCountDownCT(function ()
            local rt = link_end_time - TimeHelper.GetServerTime()
            if rt < 11 and btn_show[info.link_seq + 1] then
                self.TimeShowObj:SetActive(true)
                self.TimeShow.text = string.format(self.language.DownTimeShow, math.floor(rt), co_next.wedding_link)
            else
                self.TimeShowObj:SetActive(false)
            end
        end, function ()
            self.TimeShowObj:SetActive(false)
        end, link_end_time)
    else
        self.TimeShowObj:SetActive(false)
    end
end

function MainDownMarryParty:OnClickBtn()
    local info = self.data.marry_party_scene_info
    if marry_party_btn_func[info.link_seq] then
        Call(marry_party_btn_func[info.link_seq])
    end
end