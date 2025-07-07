
SocietyViewMasterApprenticeGroup = SocietyViewMasterApprenticeGroup or DeclareMono("SocietyViewMasterApprenticeGroup", UIWFlushPanelGroup)

function SocietyViewMasterApprenticeGroup:SocietyViewMasterApprenticeGroup()
    self.flush_events = {EventProtocol.ToEventKey(SCShiTuRelationInfo)}

    self.data = SocietyCtrl.Instance:Data()
    
    SocietyCtrl.Instance:SendShiTuReqInfo()
end

function SocietyViewMasterApprenticeGroup:OnEnable()
    self.data.cur_sel_tab = SocietyConfig.SocietyType.master_appearance
    ViewMgr:FlushView(SocietyView, "SocietyViewChat")
    ViewMgr:FlushView(SocietyView, "SocietyViewDeclaration")
    ViewMgr:FlushView(SocietyView, "SocietyViewShiTuInfo")
    ViewMgr:FlushView(SocietyView, "SocietyViewMasterApprenticeGroup/SocietyViewMasterApprenticeGroupList")
    self.data.cur_sel_role = nil
end

SocietyViewMasterApprenticeGroupOther = SocietyViewMasterApprenticeGroupOther or DeclareMono("SocietyViewMasterApprenticeGroupOther", UIWFlushPanel)

function SocietyViewMasterApprenticeGroupOther:SocietyViewMasterApprenticeGroupOther()
    self.data = SocietyCtrl.Instance:Data()
    
    self.data_cares = {
        {data = self.data.new_master_red_point, func = self.NewMasterRedPoint },
    }
end

function SocietyViewMasterApprenticeGroupOther:onFlush()
end

function SocietyViewMasterApprenticeGroupOther:NewMasterRedPoint()
	self.new_master_point = self.new_master_point or UiRedPoint.New(self.BtnApplyObj, Vector3.New(77.7,24,0))
	self.new_master_point:SetNum(self.data.new_master_red_point.val and 1 or 0)
end

function SocietyViewMasterApprenticeGroupOther:OnClickSearch()
	local str = self.InputField.text
    if StringIsEmpty(str) then
        PublicPopupCtrl.Instance:Center(Language.Society.CommonTips.InputId)
        return
    end
end

function SocietyViewMasterApprenticeGroupOther:OnClickApply()
	ViewMgr:OpenView(SocietyViewMasterApprenticeApply)
end

function SocietyViewMasterApprenticeGroupOther:OnClickGoFB()
    -- SceneLogic.Instance:AutoToNpc(self.data:GetShiTuFBNpcSeq(), nil, nil, true)
    -- ViewMgr:CloseView(SocietyView)
    ViewMgr:OpenView(SocietyShiTuFBGuideView)
end

SocietyViewMasterApprenticeGroupList = SocietyViewMasterApprenticeGroupList or DeclareMono("SocietyViewMasterApprenticeGroupList", UIWFlushPanel)

function SocietyViewMasterApprenticeGroupList:SocietyViewMasterApprenticeGroupList()
    self.data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.MasterApprentice
    
    self.data_cares = {
        {data = self.data.shitu_achieve_red_point, func = self.AchieveRedPoint},
    }

    SocietyViewMasterApprenticeGroupList.seek_type = SocietyConfig.ShiTuSeekingType.seek_master
end

function SocietyViewMasterApprenticeGroupList:AchieveRedPoint()
    self.red_point_achieve = self.red_point_achieve or UiRedPoint.New(self.Title.gameObject, Vector3.New(212,25,0))
	self.red_point_achieve:SetNum(self.data.shitu_achieve_red_point.val and 1 or 0)
end

function SocietyViewMasterApprenticeGroupList:onFlush()
    self.ToggleGroupRole:SetAllTogglesOff()
    local role_list, sel_role = self.data:GetMasterApprenticeList(SocietyViewMasterApprenticeGroupList.seek_type)
    self.RoleList:SetData(role_list)
    self.RoleList:ClickItemData(sel_role or role_list[1])
    local is_master = self.data:IsMaster()
    local num = self.data.master_apprentice_num
    local title = SocietyConfig.ShiTuSeekingType.seek_master == SocietyViewMasterApprenticeGroupList.seek_type and string.format(self.language.title_master, self.data.master_apprentice_num1, SocietyConfig.MasterApprenticeMax.master) or string.format(self.language.title_apprentice, self.data.master_apprentice_num2, SocietyConfig.MasterApprenticeMax.apprentice)
    self.Title.text = title
    self.MaterTitle.text = string.format(self.language.title_master, self.data.master_apprentice_num1, SocietyConfig.MasterApprenticeMax.master)
    self.ApprenticeTitle.text = string.format(self.language.title_apprentice, self.data.master_apprentice_num2, SocietyConfig.MasterApprenticeMax.apprentice)
end

function SocietyViewMasterApprenticeGroupList:OnClickTitle()
    local is_show = not self.TypeListObj.activeSelf
    self.TypeListObj:SetActive(is_show)

    if is_show then
        local arp_m, arp_a = false, false
        for i = 1, SocietyConfig.MASTER_APPRENTICE_MAX do
            local master = self.data.master_apprentice_list_m[i]
            local apprentice = self.data.master_apprentice_list_a[i]
    
            arp_m = arp_m or master:AchieveRedPointVal()
            arp_a = arp_a or apprentice:AchieveRedPointVal()
        end

        self.red_point_achieve_m = self.red_point_achieve_m or UiRedPoint.New(self.TypeMasterObj, Vector3.New(185,25,0))
	    self.red_point_achieve_m:SetNum(arp_m and 1 or 0)
        self.red_point_achieve_a = self.red_point_achieve_a or UiRedPoint.New(self.TypeApprenticeObj, Vector3.New(185,25,0))
	    self.red_point_achieve_a:SetNum(arp_a and 1 or 0)
    end
end

function SocietyViewMasterApprenticeGroupList:OnClickMaster()
    self.TypeListObj:SetActive(false)
    SocietyViewMasterApprenticeGroupList.seek_type = SocietyConfig.ShiTuSeekingType.seek_master
    self:onFlush()
end

function SocietyViewMasterApprenticeGroupList:OnClickApprentice()
    self.TypeListObj:SetActive(false)
    SocietyViewMasterApprenticeGroupList.seek_type = SocietyConfig.ShiTuSeekingType.seek_apprentice
    self:onFlush()
end

SocietyViewMasterApprenticeGroupItem = DeclareMono("SocietyViewMasterApprenticeGroupItem", UIWidgetBaseItem)

function SocietyViewMasterApprenticeGroupItem:SocietyViewMasterApprenticeGroupItem()
    self.society_data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.MasterApprentice
end

function SocietyViewMasterApprenticeGroupItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local is_master = self.society_data:IsMaster()
    local info = data.info
    self.EmptyObj:SetActive(nil == info)
    self.InfoObj:SetActive(nil ~= info)
    if nil == info then
        self.FindDesc.text = SocietyConfig.ShiTuSeekingType.seek_apprentice == SocietyViewMasterApprenticeGroupList.seek_type and self.language.find_apprentice or self.language.find_master
    else
        local is_online = 1 == info.is_online
	    local prof_sp = RoleData.GetProfSp(info.prof)
        self.Name.text = info.name
        self.Level.text = info.level
        -- self.Love.text = info.love
        self.ProfI.SpriteName = prof_sp
        self.ProfI.gameObject:SetActive(nil ~= prof_sp)
        self.MaskObj:SetActive(not is_online)
        self.RoleState.text = is_online and Language.Society.RoleOnline or TimeHelper.Ago(info.login_time)
        UH.SetAvatar(self.IconSp, info.avatar_type, info.avatar_id,info.avatar_quality)
        local ft = TimeHelper.FormatDHMS(TimeHelper.GetServerTime() - (info.shitu_time or 0))
        self.ShiTuTime.text = string.format(self.language.ShiTuTime, ft.day)
        if self.Break then
            self.Break:SetActive(info.relation_break_time_type == 1 and info.relation_break_time > 0)
        end
    end
end
function SocietyViewMasterApprenticeGroupItem:OnClickBreak()
    self:Click()
    local role_info = {}
    local info = self.data.info
    role_info.info = function()
        return RoleInfoData.RoleInfoNew{func_info = info, uid = info.uid, name = info.name, level = info.level, prof = info.prof, family = info.family, avatar_type = info.avatar_type, avatar_id = info.avatar_id, top_level = info.top_level}
    end
    RoleInfoData.Instance:SetRoleInfo(role_info)
    ViewMgr:OpenView(SocietyMasterCooling)
end

function SocietyViewMasterApprenticeGroupItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function SocietyViewMasterApprenticeGroupItem:OnClickItem()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    -- ViewMgr:FlushView(SocietyView, "SocietyViewShiTuInfo/SocietyViewChat/SocietyViewChatRecordList")
    if self.society_data.cur_sel_role == self.data then return end
    -- if nil == self.data.info then
    --     PublicPopupCtrl.Instance:Center(self.language.empty_tips)
    -- end
	self.society_data.cur_sel_role = self.data
    ViewMgr:FlushView(SocietyView, "SocietyViewChat")
	ViewMgr:FlushView(SocietyView, "SocietyViewDeclaration")
	ViewMgr:FlushView(SocietyView, "SocietyViewShiTuInfo")
end

function SocietyViewMasterApprenticeGroupItem:OnClickAdd()
    if nil == self.data.info then
        PublicPopupCtrl.Instance:Center(self.language.empty_tips)
    end
end

function SocietyViewMasterApprenticeGroupItem:OnClickMore()
    self:Click()
	local info = self.data.info
	local config = DataHelper.TableCopy(SocietyConfig.MasterApprenticeFunctions[1])
    local is_friend = self.society_data:IsFriend(info.uid)
    local is_master = self.society_data:IsMaster()
    local level = RoleData.Instance:GetBaseData().level
    local graduate_level = self.society_data:GetGraduateLevel()
    if GuildCtrl.Instance:Data():GetGuildID() > 0 then table.insert(config, 3, "FamilyInvite") end
    -- if is_master and info.level >= graduate_level then
    --     table.insert(config, 2, "BiYe")
    -- elseif not is_master and level >= graduate_level then
    --     table.insert(config, 2, "ChuShi")
    -- end
    local value, type = self.society_data:IsMasterOrApprentice(info.uid)
    if value and not (info.relation_break_time_type == 1 and info.relation_break_time > 0) then
        if 0 == type then
            table.insert(config, 2, "ChuShi")
        else
            table.insert(config, 2, "BiYe")
        end
    end
    if not is_friend then table.insert(config, 2, "AddFriend") end
    if not (info.relation_break_time_type == 0 and info.relation_break_time > 0) then
        -- 当前处于被拒72小时后自动解除不显示解除师徒按钮
        table.insert(config, 3 , "RelationBroken")
    end
    
	RoleInfoCtrl.Instance:RoleInfoView{
		info = function ()
				return RoleInfoData.RoleInfoNew{func_info = info, uid = info.uid, name = info.name, level = info.level, prof = info.prof, family = info.family, avatar_type = info.avatar_type, avatar_id = info.avatar_id, top_level = info.top_level}
			end,
		funcs = function()
			return config
		end
	}
end