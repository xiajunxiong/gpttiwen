
MarryPartyMenuView = MarryPartyMenuView or DeclareMono("MarryPartyMenuView", UIWFlushPanel)

function MarryPartyMenuView:MarryPartyMenuView()
    self.data = MarryPartyCtrl.Instance:Data()
    self.language = Language.MarryParty.Menu

    self.data_cares = {
		{data = self.data.marry_party_scene_info, func = self.FlushInfo},
    }
end

function MarryPartyMenuView:OnDisable()
	UIWFlushPanel.OnDisable(self)
	TimeHelper:CancelTimer(self.timer_ct)
end

function MarryPartyMenuView:FlushInfo()
    TimeHelper:CancelTimer(self.timer_ct)

    local info = self.data.marry_party_scene_info
    if info.init then return end
    
    local co = self.data.config_game_link[info.link_seq]
    if nil == co then return end

    self.NameShow.text = string.format(self.language.NameShow, co.wedding_link)
    self.NumShow.text = string.format(self.language.NumShow, info.role_num)
    self.DescShow.text = Config.language_cfg_auto.textdesc[self.data:ConfigOtherLanguageCfg()].desc

    self.timer_ct = TimeHelper:AddCountDownCT(function ()
        local last_time = info.link_end_time - TimeHelper.GetServerTime()
        local ft = TimeHelper.FormatDHMS(last_time)
        self.TimeShow.text = string.format(self.language.TimeShow, ft.min, ft.s)
    end, function ()
        self.TimeShow.text = string.format(self.language.TimeShow, 0, 0)
    end, info.link_end_time)
end

function MarryPartyMenuView:OnClickGame()
	ViewMgr:OpenView(MarryPartyGameView)
end