
SocietyViewShiTuInfo = SocietyViewShiTuInfo or DeclareMono("SocietyViewShiTuInfo", UIWFlushPanelGroup)

function SocietyViewShiTuInfo:SocietyViewShiTuInfo()
    self.flush_events = {EventProtocol.ToEventKey(SCShiTuFbCommonInfo)}

    self.data = SocietyCtrl.Instance:Data()
end

function SocietyViewShiTuInfo:OnEnable()
    UIWFlushPanelGroup.OnEnable(self)
end

function SocietyViewShiTuInfo:onFlush()
    self.OffsetObj:SetActive(nil ~= self.data.cur_sel_role and nil ~= self.data.cur_sel_role.info)

    self.red_point_achieve = self.red_point_achieve or UiRedPoint.New(self.AchieveObj, Vector3.New(60,20,0))
    if self.data.cur_sel_role then
        self.red_point_achieve:SetNum(self.data.cur_sel_role and self.data.cur_sel_role:AchieveRedPointVal() and 1 or 0)
    end
end

SocietyViewShiTuInfoGame = SocietyViewShiTuInfoGame or DeclareMono("SocietyViewShiTuInfoGame", UIWFlushPanelGroup)

function SocietyViewShiTuInfoGame:SocietyViewShiTuInfoGame()
    -- self.flush_events = {EventProtocol.ToEventKey(SCShiTuFbCommonInfo)}

    self.data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.ShiTuInfoGame
end

function SocietyViewShiTuInfoGame:onFlush()
    local shitu_game_show_list = self.data:GetShiTuGameShowList()
    self.ShowItem1:SetData(shitu_game_show_list[2])
    self.ShowItem2:SetData(shitu_game_show_list[3])
    
    local info = self.data.shitu_info_sm
    self.TimeShow1.text = string.format(self.language.TimeShow1, info.is_get_shi_tu_reward)
    self.TimeShow2.text = RoleData.Instance:GetRoleLevel() < SocietyConfig.CHUANG_GONG_OPEN and self.language.OpenTipsLevel or string.format(self.language.TimeShow2, info.week_chuan_gong_progress, #Config.mentor_cfg_auto.game_reward - 1)
end

function SocietyViewShiTuInfoGame:OnClickTips1()
    ViewMgr:OpenView(SocietyShiTuFBGuideView)
end

function SocietyViewShiTuInfoGame:OnClickTips2()
    if RoleData.Instance:GetRoleLevel() < SocietyConfig.CHUANG_GONG_OPEN then
        PublicPopupCtrl.Instance:Center(self.language.OpenTipsShow)
        return
    end
    ViewMgr:OpenView(SocietyShiTuCGGuideView)
end

function SocietyViewShiTuInfoGame:OnClickFight()
    if RoleData.Instance:GetRoleLevel() < SocietyConfig.CHUANG_GONG_OPEN then
        PublicPopupCtrl.Instance:Center(self.language.OpenTipsShow)
        return
    end
    SceneLogic.Instance:AutoToNpc(self.data:GetShiTuFBNpcSeq(), nil, nil, true)
    ViewMgr:CloseView(SocietyView)
end

SocietyViewShiTuInfoGameItem = DeclareMono("SocietyViewShiTuInfoGameItem", UIWidgetBaseItem)

function SocietyViewShiTuInfoGameItem:SocietyViewShiTuInfoGameItem()
    self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyViewShiTuInfoGameItem:SetData(data)
    self.Rewards:SetData(SocietyConfig.ShiTuSeekingType.seek_master == SocietyViewMasterApprenticeGroupList.seek_type and data.apprentice_reward_show or data.master_reward_show)
    self.NameShow.text = data.fun_name
end

function SocietyViewShiTuInfoGameItem:OnClickInvite()
    ViewMgr:OpenView(TeamInviteShiTuView)
end

function SocietyViewShiTuInfoGameItem:OnClickFight()
    SceneLogic.Instance:AutoToNpc(self.society_data:GetShiTuFBNpcSeq(), nil, nil, true)
    ViewMgr:CloseView(SocietyView)
end

SocietyViewShiTuInfoAchieve = SocietyViewShiTuInfoAchieve or DeclareMono("SocietyViewShiTuInfoAchieve", UIWFlushPanel)

function SocietyViewShiTuInfoAchieve:SocietyViewShiTuInfoAchieve()
    -- self.flush_events = {EventProtocol.ToEventKey(SCShiTuFbCommonInfo)}
    self.data = SocietyCtrl.Instance:Data()
    self.data_cares = {
        {data = self.data.shitu_info_sm, func = self.onFlush, keys = {"mile_reward_flag"}}
    }
    self.language = Language.Society.ShiTuInfoAchieve
end

function SocietyViewShiTuInfoAchieve:onFlush()
    local shitu_achieve_show_list = self.data:GetShiTuAchieveShowList()
    self.ShowList:SetData(shitu_achieve_show_list)

    local role = self.data.cur_sel_role
    if role and role.info then
        self.TitleShow.text = string.format(self.language.TitleShow, role.info.name)
    end
end

SocietyViewShiTuInfoAchieveItem = DeclareMono("SocietyViewShiTuInfoAchieveItem", UIWidgetBaseItem)

function SocietyViewShiTuInfoAchieveItem:SocietyViewShiTuInfoAchieveItem()
    self.society_data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.ShiTuInfoAchieve
end

function SocietyViewShiTuInfoAchieveItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Rewards:SetData(SocietyConfig.ShiTuSeekingType.seek_master == SocietyViewMasterApprenticeGroupList.seek_type and data.apprentice_reward_show or data.master_reward_show)
    self.NameShow.text = data.name
    self.DescShow.text = string.format(self.language.DescShow[data.type], data.param)

    local value = self.society_data:GetShiTuInfoAchieveValue(data.type)
    local is_get = self.society_data:GetShiTuInfoAchieveIsGet(data.seq)
    local can_get = value >= data.param

    self.ProgressShow:SetProgress(value / data.param)
    self.ProgressShow:SetText(string.format(self.language.ProgressShow, value, data.param))

    self.GetedObj:SetActive(is_get)
    self.BtnGetInter:SetObjActive(not is_get)
    self.BtnGet.text = can_get and self.language.CanGet or self.language.NotCanGet
    self.BtnGetInter.Interactable = can_get

    self.red_point = self.red_point or UiRedPoint.New(self.BtnGetInter.gameObject, Vector3.New(45,25,0))
    if 1 == data.type then
        self.red_point:SetNum((can_get and not is_get and self.society_data.shitu_info_sm.day_mile_cap_fetch_num < SocietyConfig.ACHIEVE_TIMES_MAX) and 1 or 0)
    else
        self.red_point:SetNum((can_get and not is_get and self.society_data.shitu_info_sm.day_mile_pass_day_fetch_num < SocietyConfig.ACHIEVE_TIMES_MAX) and 1 or 0)
    end
end

function SocietyViewShiTuInfoAchieveItem:OnClickGet()
    local role = self.society_data.cur_sel_role
    if role and role.info then
        SocietyCtrl.Instance:SendShiTuChuanGongFBReqFetchMile(role.info.uid, self.data.seq)
    end
end
