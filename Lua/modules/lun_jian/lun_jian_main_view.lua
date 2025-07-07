LunJianMainView = LunJianMainView or DeclareView("LunJianMainView", "lun_jian/lun_jian_main")
VIEW_DECLARE_LEVEL(LunJianMainView,ViewLevel.L1)
--TabToggles======LuaUIEventClick[]
--TabbarParObj====GameObject
--SeasonTxt=======Text
--ScheduleRP======UIWSubstitute[UIWRedPoint2]
LunJianMainView.Tab = {
    Schedule = 1,
    Rank = 2,
    Reward = 3,
}

function LunJianMainView.RankSpriteName(rank)
    return string.format("RankImg%d",rank)
end

function LunJianMainView:LunJianMainView()
    self.rp_smdata_handle = nil
end

function LunJianMainView:LoadCallback(param)
    if param ~= nil and param.open_tab ~= nil then
        self.TabToggles[param.open_tab]:Trigger()
    else
        self.TabToggles[LunJianMainView.Tab.Schedule]:Trigger()
    end
    self.TabbarParObj:SetActive(
        LunJianData.Instance:CurSeason() > 0 and   
        LunJianData.Instance:SeasonStatus() >= LunJianData.Status.Battle and
        LunJianData.Instance:HasSignuped())--战斗阶段才会显示标签页，其它时候都只显示奖励
    UH.SetText(self.SeasonTxt,string.format(Language.LunJian.Signup.SeasonCount,DataHelper.GetDaXie(LunJianData.Instance:CurSeason(),false)))

    self.rp_smdata_handle = LunJianData.Instance:MyInfo():Care(BindTool.Bind(self.ScheduleRedPoint,self),"win_box_reward_flag","win_num","daily_reward_fetch_flag")
    self:ScheduleRedPoint()
end

function LunJianMainView:CloseCallback()

end

function LunJianMainView:OnClickClose()
    ViewMgr:CloseView(LunJianMainView)
end

function LunJianMainView:OnTabToggleClick()

end

function LunJianMainView:ScheduleRedPoint()
    local rp_num = LunJianData.Instance:RemindFunc()
    self.ScheduleRP:SetNum(rp_num)
end

function LunJianMainView:OnDestroy()
    if self.rp_smdata_handle ~= nil then
        LunJianData.Instance:MyInfo():Uncare(self.rp_smdata_handle)
        self.rp_smdata_handle = nil
    end
end

function LunJianMainView:OnClickTips()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[269].desc})
end