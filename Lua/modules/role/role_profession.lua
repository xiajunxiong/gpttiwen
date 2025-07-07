RoleProfessionView = RoleProfessionView or DeclareView("RoleProfessionView", "role_professtion")-- ,Mod.ProfesstionChange.Change)

VIEW_DECLARE_LEVEL(RoleProfessionView,ViewLevel.L1)
VIEW_DECLARE_MASK(RoleProfessionView,ViewMask.BgBlockNoAnim)

RoleProfessionView.TitlePos = {
    ["QiGongShi"] = Vector3.New(-6, 182, 0),
    ["JianXiaKe"] = Vector3.New(-9, 179, 0),
    ["KuangZhanShi"] = Vector3.New(-2, 171, 0),
    ["TieWei"] = Vector3.New(-6, 145, 0),
    ["ShenSheShou"] = Vector3.New(-10, 182, 0),
    ["XingShuShi"] = Vector3.New(-5, 182, 0),
    ["YiShuShi"] = Vector3.New(-5, 182, 0),
    ["FuZhouShi"] = Vector3.New(-6, 176, 0),
}

function RoleProfessionView:RoleProfessionView()
    
end

function RoleProfessionView:LoadCallback()
    self.Currency:DefaultSet()
end

function RoleProfessionView:Close()
    ViewMgr:CloseView(RoleProfessionView)
end

RoleProfesstionPanel = RoleProfesstionPanel or DeclareMono("RoleProfesstionPanel", UIWFlushPanel)
---ModelShow====ModelUIShow
---RoleProfName====UIImageSpriteSet

function RoleProfesstionPanel:RoleProfesstionPanel()
    RoleCtrl.Instance:SendRequestChangeProfTime()
    self.base_data = RoleData.Instance:GetBaseData()
    self.profession_view_data = RoleData.Instance:GetProfessionViewData()
    self.change_prof_times = RoleData.Instance.change_prof_times
    self.data_cares = {
        {data = self.base_data, keys = {"profession"}, func = self.FlushInfo},
        {data = self.profession_view_data, keys = {"select_profession"}, func = self.FlushPreview},
        {data = self.change_prof_times,func = self.FlushNum},
        {data = SkillData.Instance.prof_view_skill_info,func = self.FlushSkillPanel}
    }
    self.change_consume = 0
    -- self.ui_obj = nil

    if self.ui_obj == nil then
        self.ui_obj = UIChDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self})
    end
end

function RoleProfesstionPanel:FlushInfo()
    local change_prof_data = RoleData.Instance:GetChangeProfData()
    self.ProfList:SetData(change_prof_data)
    self.ProfList:ClickItem(1)
    local prof_name = Language.Common.Prof[self.base_data.profession]
    local sprite_name = RoleData.GetProfSp(self.base_data.profession,true)

    -- self.RoleProfName.text = prof_name
    self.ProfName1.text = prof_name
    self.RoleProfIconObj:SetActive(self.base_data.profession ~= 0)
    if self.base_data.profession ~= 0 then
        UH.SpriteName(self.RoleProfIcon,RoleData.GetProfSp(self.base_data.profession,true))
    end
    self.RoleCurIcon.SpriteName = sprite_name.."_icon"
    self.RoleCurName.text = prof_name
end

function RoleProfesstionPanel:FlushNum()
    -- Debuger.LogError("change...")
    local num = RoleData.Instance.change_prof_times.num
    local config = Config.career_advancements_auto.other[1]
    self.change_consume = num < 4 and math.pow(2,num) or 16
    self.change_consume = self.change_consume * config.magnification

    if self.change_consume > config.transfer_cost_max then 
        self.change_consume = config.transfer_cost_max
    elseif self.change_consume < config.transfer_cost_min then 
        self.change_consume = config.transfer_cost_min
    end 

    self.NumText.text = tostring(self.change_consume)
end

function RoleProfesstionPanel.ProfToWeaponId(prof)

end

function RoleProfesstionPanel:FlushPreview()
    local select_profession = self.profession_view_data.select_profession
    local prof_name = Language.Common.Prof[select_profession]
    self.ProfName2.text = prof_name
    if select_profession < 1 then
        return
    end
    
    local sprite_name = RoleData.GetProfSp(select_profession,true)
    self.RoleProfName.SpriteName = "_Loc_"..sprite_name
    UH.LocalPos(self.RoleProfNameRect,RoleProfessionView.TitlePos[sprite_name])
    self.RoleProfNameImage:SetNativeSize()
    
    local desc_config = RoleData.Instance:GetProfessionDesc(select_profession)
    self.TipsLabel1.text = desc_config.weapon
    self.TipsLabel2.text = desc_config.armor_type
    self.TipsLabel3.text = desc_config.point
    self.TipsLabel4.text = desc_config.desc

    local skill_data = SkillData.GetProfSkillList(select_profession)
    self.SkillList:SetData(skill_data)

    local role_appearance = RoleData.Instance:GetApearance()
	local appearance = {
		weapon_id = RoleProfessionSimpleWeaponId[select_profession] or 0,
		color = role_appearance.color,
		prof_base = role_appearance.prof_base,
        avatar_type = role_appearance.avatar_type,
        advance_time = role_appearance.advance_time,
    }
    
    self.ui_obj:SetData(appearance)
    self.ui_obj:PlayAnim(ChAnimType.WeaponShow)
    local audio = Cfg.AudioProf(role_appearance.avatar_type,select_profession)
    AudioMgr:StopEffectHandle(self.handle_audio1)
    AudioMgr:StopEffectHandle(self.handle_audio2)
    self.handle_audio1 = AudioMgr:PlayEffectHandle(AudioType.UI, audio.show_audio)
    self.handle_audio2 = AudioMgr:PlayEffectHandle(AudioType.UI, audio.show_voice)
end

function RoleProfesstionPanel:ChangeProfesstion()
    if self.base_data.profession_id <= 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[44])
        return
    end
    if ActivityData.Instance:GetActivityStatus(ActType.ChiefPeakBattle) ~= ActStatusType.Close then
        if ChiefElectionData.Instance:IsChief()then
            PublicPopupCtrl.Instance:Center(Language.ChiefPeakBattle.ErrorLog[4])
            return
        end
    end
    local change_num = self.profession_view_data.select_profession - self.base_data.profession
    local change_prof_id = self.base_data.profession_id + change_num * 100

    local prof_name_1 = Language.Common.Prof[self.base_data.profession]
    local prof_name_2 = Language.Common.Prof[self.profession_view_data.select_profession]

    local role_appearance = RoleData.Instance:GetApearance()

    PublicPopupCtrl.Instance:DialogTips{
        content = Language.Role.PopupTips0,
        confirm = {
            func = function ()
                PublicPopupCtrl.Instance:FlushDialogTips{
                    content = string.format(Language.Role.PopupTips1,prof_name_1,prof_name_2,self.change_consume),
                    confirm = {
                        func = function ()
                            RoleCtrl.Instance:SendRoleCareerChangeReq({
                                to_profession = change_prof_id,
                                avartar_type = role_appearance.avatar_type, 
                                color = role_appearance.color,

                            })
                            RoleCtrl.Instance:SendRequestChangeProfTime()
                            PublicPopupCtrl.Instance:CloseDialogTips()
                        end
                    }
                }
            end
        }
    }

end

function RoleProfesstionPanel:OpenPreview()
    -- 这个地方策划还没决定要显示什么东西 ChangeProfPreview
end

function RoleProfesstionPanel:FlushSkillPanel()
    local role_skill_view =  SkillData.Instance.prof_view_skill_info
    self.SkillPanel:SetActive(role_skill_view.onclick)    
    self.SkillTipsText.text = role_skill_view.tips_str

    if not role_skill_view.onclick then return end 

    local offset_x = 205
    local offset_y = 72
    local pos = nil
    local first_pos_y =  self.RectWidget.localPosition.y - self.RectViewPort.localPosition.y + role_skill_view.pos.y
    local second_pos_y = self.RectParent.localPosition.y + (first_pos_y - role_skill_view.height / 2 ) + offset_y 

    local pos = Vector2.New(offset_x,second_pos_y)
	self.SkillTipsRect.localPosition = pos
end

function RoleProfesstionPanel:CloseSkillPanel()
    -- 
    SkillData.Instance:OperateRoleProfViewSkill({onclick = false})
end 

function RoleProfesstionPanel:OnDestroy()
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
    UIWFlushPanel.OnDestroy(self)
end


RoleProfessionSkillItem = RoleProfessionSkillItem or DeclareMono("RoleProfessionSkillItem", UIWidgetBaseItem)

function RoleProfessionSkillItem:RoleProfessionSkillItem()
    -- body
end

function RoleProfessionSkillItem:SetData(data)
    self.SkillName.text = data.skill_name
    -- self.SkillIcon.SpriteName = 
end