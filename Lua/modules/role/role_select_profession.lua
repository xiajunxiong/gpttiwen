RoleSelectProfessionView = RoleSelectProfessionView or DeclareView("RoleSelectProfessionView", "role_select_professtion",Mod.Professtion.Select)

VIEW_DECLARE_LEVEL(RoleSelectProfessionView,ViewLevel.L1)
VIEW_DECLARE_MASK(RoleSelectProfessionView,ViewMask.BgBlockNoAnim)

function RoleSelectProfessionView:RoleSelectProfessionView()
end

function RoleSelectProfessionView:LoadCallback()
    self.Currency:DefaultSet()
end

function RoleSelectProfessionView:Close()
    ViewMgr:CloseView(RoleSelectProfessionView)
end

RoleSelectProfesstionPanel = RoleSelectProfesstionPanel or DeclareMono("RoleSelectProfesstionPanel", UIWFlushPanel)
---ModelShow====ModelUIShow

function RoleSelectProfesstionPanel:RoleSelectProfesstionPanel()
    self.base_data = RoleData.Instance:GetBaseData()
    self.profession_view_data = RoleData.Instance:GetProfessionViewData()
    self.data_cares = {
        -- {data = self.base_data, keys = {"profession"}, func = self.FlushInfo},
        {data = self.profession_view_data, keys = {"select_profession"}, func = self.FlushPreview},
        {data = SkillData.Instance.prof_view_skill_info,func = self.FlushSkillPanel}
    }
    -- self.ui_obj = nil

    if self.ui_obj == nil then
        self.ui_obj = UIChDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self})
    end
end
function RoleSelectProfesstionPanel:onFlush()
    self:FlushInfo()
end
function RoleSelectProfesstionPanel:FlushInfo()
    local change_prof_data = RoleData.Instance:GetChangeProfData()
    self.ProfList:SetData(change_prof_data)
    self.ProfList:ClickItem(1)
end

function RoleSelectProfesstionPanel:FlushPreview()
    local select_profession = self.profession_view_data.select_profession
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

function RoleSelectProfesstionPanel:FlushSkillPanel()
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

function RoleSelectProfesstionPanel:CloseSkillPanel()
    -- 
    SkillData.Instance:OperateRoleProfViewSkill({onclick = false})
end 


function RoleSelectProfesstionPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
end

function RoleSelectProfesstionPanel:ChangeProfesstion()
    local select_profession = self.profession_view_data.select_profession * 100 + 1

    local prof_name = Language.Common.Prof[self.profession_view_data.select_profession]

    local function confirm_func()
        PublicPopupCtrl.Instance:CloseDialogTips()
        ViewMgr:CloseView(RoleSelectProfessionView)
        RoleCtrl.Instance:SendRoleCareerAdvance(select_profession,RoleData.Instance:GetApearance().color)
    end

    local info_config = {
        content = string.format(Language.Role.PopupTips2,prof_name),
        confirm = {
            func = confirm_func,
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info_config)
end