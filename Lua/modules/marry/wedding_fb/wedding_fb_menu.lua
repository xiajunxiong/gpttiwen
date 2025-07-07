
MarryWeddingFBMenuView = MarryWeddingFBMenuView or DeclareMono("MarryWeddingFBMenuView", UIWFlushPanel)

function MarryWeddingFBMenuView:MarryWeddingFBMenuView()
    self.data = MarryCtrl.Instance:Data()
    self.language = Language.Marry.WeddingFBMenu

    self.data_cares = {
		{data = self.data.wedding_scene_info, func = self.FlushInfo},
		{data = self.data.wedding_role_info, func = self.FlushRewardShow, init_call = false},
		{data = self.data.wedding_role_info, func = self.FlushCandyShow, init_call = false},
    }
end

function MarryWeddingFBMenuView:OnDisable()
	UIWFlushPanel.OnDisable(self)
	TimeHelper:CancelTimer(self.timer_ct)
end

function MarryWeddingFBMenuView:FlushInfo()
    TimeHelper:CancelTimer(self.timer_ct)

    local info = self.data.wedding_scene_info
    if info.init then return end
    
    local co = self.data.config_wedding_party[info.link_seq]
    local wedding_heat_reward_str_show = self.data:GetWeddingFBHeatRewardStrShow(".")

    self.NameMr.text = string.format(self.language.NameMr, info.host_name_xl)
    self.NameMrs.text = string.format(self.language.NameMrs, info.host_name_xn)
    self.StateShow.text = string.format(self.language.StateShow, co and co.wedding_link or "")
    self.DescShow.text = string.format(self.language.DescShow, wedding_heat_reward_str_show)

    self.timer_ct = TimeHelper:AddCountDownCT(function ()
        local last_time = info.fb_end_time - TimeHelper.GetServerTime()
        local ft = TimeHelper.FormatDHMS(last_time)
        self.TimeShow.text = string.format(self.language.TimeShow, ft.min, ft.s)
    end, function ()
        self.TimeShow.text = string.format(self.language.TimeShow, 0, 0)
    end, info.fb_end_time)

    self:FlushRewardShow()
    self:FlushCandyShow()
    self:FlushBtnCar()
end

function MarryWeddingFBMenuView:FlushRewardShow()
    local is_show = self.data:GetWeddingFBMenuRewardShow()
    if is_show then
        self.can_get_reward = true
    end
    self.BtnRewardObj:SetActive(is_show)
end

function MarryWeddingFBMenuView:FlushCandyShow()
    local is_show = self.data:GetWeddingFBMenuCandyShow()
    if is_show then
        self.can_get_candy = true
    end
    self.BtnCandyObj:SetActive(is_show)
end

function MarryWeddingFBMenuView:FlushBtnCar()
	local is_show = false
    local info = self.data.wedding_scene_info
    if not info.init then
        is_show = 1 == info.is_host and 0 == info.xun_you_type
    end
	self.BtnCarObj:SetActive(is_show)
end

function MarryWeddingFBMenuView:OnClickTips()
    PublicPopupCtrl.Instance:HelpTip(224)
end

function MarryWeddingFBMenuView:OnClickReward()
    if not self.can_get_reward then return end
    self.can_get_reward = false
    local scene_info = self.data.wedding_scene_info
    local role_info = self.data.wedding_role_info
    if scene_info.init or role_info.init then return end
    for _, v in ipairs(self.data.config_heat_reward) do
        if scene_info.wedding_heat >= v.heat_request and 0 == (bit:_rshift(role_info[MarryConfig.FBCommonOperType.heat_reward] or 0, v.heat_type) % 2) then
            MarryCtrl.Instance:SendWeddingFBReqHeatReward(v.heat_type)
        end
    end
end

function MarryWeddingFBMenuView:OnClickCandy()
    if not self.can_get_candy then return end
    self.can_get_candy = false
    local scene_info = self.data.wedding_scene_info
    local role_info = self.data.wedding_role_info
    if scene_info.init or role_info.init then return end
    MarryCtrl.Instance:SendWeddingFBReqCandies()
end

function MarryWeddingFBMenuView:OnClickCar()
	ViewMgr:OpenView(MarryCarView)
end