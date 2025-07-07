
SocietyViewDeclaration = SocietyViewDeclaration or DeclareMono("SocietyViewDeclaration", UIWFlushPanelGroup)

function SocietyViewDeclaration:SocietyViewDeclaration()
    self.flush_events = {EventProtocol.ToEventKey(SCShiTuSeekingInfo), EventProtocol.ToEventKey(SCShiTuRelationInfo)}

    self.data = SocietyCtrl.Instance:Data()
end

function SocietyViewDeclaration:OnEnable()
    UIWFlushPanelGroup.OnEnable(self)
    if self.data:IsNeedOpenMaqView() then
        ViewMgr:OpenView(SocietyMaqView)
    end
end

function SocietyViewDeclaration:onFlush()
    self.DeclarationObj:SetActive(nil ~= self.data.cur_sel_role and nil == self.data.cur_sel_role.info)
end

SocietyViewDeclarationDetail = SocietyViewDeclarationDetail or DeclareMono("SocietyViewDeclarationDetail", UIWFlushPanel)

function SocietyViewDeclarationDetail:SocietyViewDeclarationDetail()
    self.data = SocietyCtrl.Instance:Data()

    self.language = Language.Society.MasterApprentice
end

function SocietyViewDeclarationDetail:onFlush()
    local seeking_notice = self.data.ma_seeking_notice
    self.BtnDeclaration.text = StringIsEmpty(seeking_notice) and Language.Society.MasterApprentice.new_decl or Language.Society.MasterApprentice.edit_decl
end

function SocietyViewDeclarationDetail:OnClickApply()
    if LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:Center(0 == SocietyViewDeclarationDetail.seeking_type and self.language.cross_tips_error3 or self.language.cross_tips_error4)
        return
    end
    if nil == self.data.cur_sel_decl then
        PublicPopupCtrl.Instance:Center(self.language.apply_tips)
        return
    end
    SocietyCtrl.Instance:SendShiTuMakeRelationReq(self.data.cur_sel_decl, SocietyViewDeclarationDetail.seeking_type)
end

function SocietyViewDeclarationDetail:OnClickRule()
    PublicPopupCtrl.Instance:OpenTipsPopup{tips_str = Language.Society.MasterApprentice.rule}
end

SocietyViewDeclarationDeclList = SocietyViewDeclarationDeclList or DeclareMono("SocietyViewDeclarationDeclList", UIWFlushPanel)

function SocietyViewDeclarationDeclList:SocietyViewDeclarationDeclList()
    self.data = SocietyCtrl.Instance:Data()

    self.data_cares = {
		{data = self.data.maq_info, func = self.FlushShow, init_call = false},
    }
end

function SocietyViewDeclarationDeclList:onFlush()
    if nil == self.seeking_type then
        -- if self.data.is_master then
        --     self.BtnBaiShi.isOn = true
        -- else
        --     self.BtnShouTu.isOn = true
        -- end
        self:OnClickItem(SocietyConfig.ShiTuSeekingType.seek_apprentice)
    else
        self:FlushShow()
    end
end

function SocietyViewDeclarationDeclList:OnClickItem(seeking_type)
    SocietyViewDeclarationDetail.seeking_type = seeking_type
    self.seeking_type = seeking_type
    self:FlushShow()
end

function SocietyViewDeclarationDeclList:FlushShow()
    if nil == self.seeking_type then return end
    local data = self.data:GetMasterApprenticeAddList(self.seeking_type)
    self.DeclList:SetData(data)
    local is_empty = nil == next(data)
    self.EmptyObj:SetActive(is_empty)
    local seeking_type = self.data.is_master and SocietyConfig.ShiTuSeekingType.seek_apprentice or SocietyConfig.ShiTuSeekingType.seek_master
    self.BtnApplyInter.Interactable = seeking_type == self.seeking_type or SocietyConfig.ShiTuSeekingType.seek_master == self.seeking_type
    local is_graduate = RoleData.Instance:GetRoleLevel() >= self.data:GetGraduateLevel()
    -- self.BtnDeclInter.Interactable = (is_graduate and SocietyConfig.ShiTuSeekingType.seek_master == self.seeking_type) or (not is_graduate and SocietyConfig.ShiTuSeekingType.seek_apprentice == self.seeking_type)
    self.BtnDeclInter.Interactable = (is_graduate and SocietyConfig.ShiTuSeekingType.seek_master == self.seeking_type) or (SocietyConfig.ShiTuSeekingType.seek_apprentice == self.seeking_type)
end

function SocietyViewDeclarationDeclList:OnClickDecl()
    local graduate_level = self.data:GetGraduateLevel()
    local is_graduate = RoleData.Instance:GetRoleLevel() >= graduate_level
    -- if is_graduate and SocietyConfig.ShiTuSeekingType.seek_apprentice == self.seeking_type then
    --     PublicPopupCtrl.Instance:Center(string.format(Language.Society.MasterApprentice.decl_error2, graduate_level))
    if not is_graduate and SocietyConfig.ShiTuSeekingType.seek_master == self.seeking_type then
        PublicPopupCtrl.Instance:Center(string.format(Language.Society.MasterApprentice.decl_error1, graduate_level))
    elseif SocietyConfig.ShiTuSeekingType.seek_master == self.seeking_type and not self.data.is_master then
        PublicPopupCtrl.Instance:Center(Language.Society.MasterApprentice.decl_error3)
    elseif LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:Center(StringIsEmpty(self.data.ma_seeking_notice) and Language.Society.MasterApprentice.cross_tips_error1 or Language.Society.MasterApprentice.cross_tips_error2)
    else
        ViewMgr:OpenView(SocietyViewMasterApprenticeDecl)
    end
end

SocietyViewDeclarationItem = DeclareMono("SocietyViewDeclarationItem", UIWidgetBaseItem)

function SocietyViewDeclarationItem:SocietyViewDeclarationItem()
    self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyViewDeclarationItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = data.info
    local prof_sp = RoleData.GetProfSp(info.prof)
    local seeking_type = self.society_data.is_master and SocietyConfig.ShiTuSeekingType.seek_apprentice or SocietyConfig.ShiTuSeekingType.seek_master
	self.Name.text = info.name
	self.Level.text = "Lv." .. info.level
	self.ProfI.SpriteName = prof_sp
	self.ProfI.gameObject:SetActive(nil ~= prof_sp)
	self.Family.text = info.family
    self.Content.text = info.seeking_notice
    self.Selector:SetObjActive((seeking_type == info.seeking_type or SocietyConfig.ShiTuSeekingType.seek_master == info.seeking_type) and not data:IsSelf())
    self.RecoObj:SetActive((seeking_type == info.seeking_type or SocietyConfig.ShiTuSeekingType.seek_master == info.seeking_type) and self.society_data.maq_avatars[info.avatar_type] and (not data:IsSelf()))
    UH.SetAvatar(self.IconSp, info.avatar_type,info.avatar_id,info.avatar_quality)
end

function SocietyViewDeclarationItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function SocietyViewDeclarationItem:OnSelect(selected)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
    UIWidgetBaseItem.OnSelect(self, selected)
    self.society_data.cur_sel_decl = selected and self.data.info.uid or nil
end

function SocietyViewDeclarationItem:OnClickIcon()
    self.society_data.society_dialog_info = {
        info = self.data.info,
        confirm = {
            name = SocietyConfig.ShiTuSeekingType.seek_apprentice == SocietyViewDeclarationDetail.seeking_type and Language.ItemInfo.RoleButtonName.ApprenticeInvite or Language.ItemInfo.RoleButtonName.MasterInvite,
            func = function ()
                SocietyCtrl.Instance:SendShiTuMakeRelationReq(self.data.info.uid, SocietyViewDeclarationDetail.seeking_type)
            end
        }
    }
    ViewMgr:OpenView(SocietyDialogView)
end

SocietyViewDeclarationGame = SocietyViewDeclarationGame or DeclareMono("SocietyViewDeclarationGame", UIWFlushPanelGroup)

function SocietyViewDeclarationGame:SocietyViewDeclarationGame()
    self.data = SocietyCtrl.Instance:Data()
end

function SocietyViewDeclarationGame:onFlush()
    local shitu_game_show_list = self.data:GetShiTuGameShowList()
    for i = 1, self.ShowItems:Length() do
        self.ShowItems[i]:SetData(shitu_game_show_list[i])
    end
end

function SocietyViewDeclarationGame:OnClickAchieve()
    -- ViewMgr:OpenView(SocietyShiTuAchieveGuideView)
    self.ToggleAchieve.isOn = true
end

function SocietyViewDeclarationGame:OnClickShiTuFB()
    ViewMgr:OpenView(SocietyShiTuFBGuideView)
end

function SocietyViewDeclarationGame:OnClickShiTuCG()
    ViewMgr:OpenView(SocietyShiTuCGGuideView)
end


SocietyViewDeclarationGameItem = DeclareMono("SocietyViewDeclarationGameItem", UIWidgetBaseItem)

function SocietyViewDeclarationGameItem:SetData(data)
    self.Rewards:SetData(SocietyConfig.ShiTuSeekingType.seek_master == SocietyViewMasterApprenticeGroupList.seek_type and data.apprentice_reward_show or data.master_reward_show)
    self.NameShow.text = data.fun_name
    self.DescShow.text = data.desc_show
end

SocietyViewDeclarationAchieve = SocietyViewDeclarationAchieve or DeclareMono("SocietyViewDeclarationAchieve", UIWFlushPanel)

function SocietyViewDeclarationAchieve:SocietyViewDeclarationAchieve()
    -- self.flush_events = {EventProtocol.ToEventKey(SCShiTuRelationInfo)}

    self.data = SocietyCtrl.Instance:Data()
    --[[ self.data_cares = {
        {data = self.data.shitu_info_sm, func = self.onFlush, keys = {"mile_reward_flag"}}
    } ]]
    self.language = Language.Society.ShiTuInfoAchieve
end

function SocietyViewDeclarationAchieve:onFlush()
    local shitu_achieve_show_list = self.data:GetShiTuAchieveShowList2()
    self.ShowList:SetData(shitu_achieve_show_list)

    -- local role = self.data.cur_sel_role
    -- if role and role.info then
    --     self.TitleShow.text = string.format(self.language.TitleShow, role.info.name)
    -- end
end

SocietyViewDeclarationAchieveItem = DeclareMono("SocietyViewDeclarationAchieveItem", UIWidgetBaseItem)

function SocietyViewDeclarationAchieveItem:SocietyViewDeclarationAchieveItem()
    self.society_data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.ShiTuInfoAchieve
end

function SocietyViewDeclarationAchieveItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Rewards:SetData(SocietyConfig.ShiTuSeekingType.seek_master == SocietyViewMasterApprenticeGroupList.seek_type and data.apprentice_reward_show or data.master_reward_show)
    self.NameShow.text = data.name
    self.DescShow.text = string.format(self.language.DescShow[data.type], data.param)

    -- local value = self.society_data:GetShiTuInfoAchieveValue(data.type)
    -- local is_get = self.society_data:GetShiTuInfoAchieveIsGet(data.seq)
    -- local can_get = value >= data.param

    -- self.ProgressShow:SetProgress(value / data.param)
    -- self.ProgressShow:SetText(string.format(self.language.ProgressShow, value, data.param))

    -- self.GetedObj:SetActive(is_get)
    -- self.BtnGetInter:SetObjActive(not is_get)
    -- self.BtnGet.text = can_get and self.language.CanGet or self.language.NotCanGet
    -- self.BtnGetInter.Interactable = can_get
end