
NewSkillPopupView = NewSkillPopupView or DeclareView("NewSkillPopupView","role/new_skill_popup",0,ViewLayer.Top - 1)
VIEW_DECLARE_MASK(NewSkillPopupView,ViewMask.None)

--ShowTime=====int  显示时间(毫秒)
--SkillName====Text
--SkillDesc====Text
--SkillIcon====RawImage

function NewSkillPopupView:NewSkillPopupView()
    local popup_skill_t = SkillData.Instance:PopNewSkillPopup()
    -- LogError("NewSkillPopupView",popup_skill_id)
    local skill_cfg = nil
    local title_text = popup_skill_t.title_text or Language.NewSkillPopup.NewSkill
    local icon_type = ICON_TYPE.SKILL
    if popup_skill_t.is_life_skill then
        skill_cfg = Cfg.SkillDescLife(popup_skill_t.skill_id, popup_skill_t.skill_level)
        title_text = Language.NewSkillPopup.SkillLevelUp
    elseif popup_skill_t.is_heart_skill then
        skill_cfg = Cfg.SkillDescHeart(popup_skill_t.skill_id, popup_skill_t.skill_level)
    elseif popup_skill_t.is_star_map then
        skill_cfg = StarMapData.Instance:GetSkillCfg(popup_skill_t.skill_id, popup_skill_t.skill_level)
        icon_type = ICON_TYPE.BUFF
    elseif popup_skill_t.is_huanshou then
        local skill_type = popup_skill_t.skill_type or HuanShouData.SkillType.Exclusive
        skill_cfg = HuanShouData.Instance:GetSkillCfg(skill_type, popup_skill_t.skill_id, popup_skill_t.skill_level)
    else
        skill_cfg = Cfg.SkillDescPartnerAndRole(popup_skill_t.skill_id,1)
    end
    if skill_cfg ~= nil then
        -- LogError("====",skill_cfg)
        self.SkillName.text = skill_cfg.skill_name
        self.SkillDesc.text = skill_cfg.description or skill_cfg.skill_dec
        UH.SetIconToRawImage(self.SkillIcon, skill_cfg.icon_id or skill_cfg.skill_icon, icon_type)
        UH.SetText(self.TitleText, title_text)
    end
    self.close_timer = nil
end

function NewSkillPopupView:OpenCallback()
    self.close_timer = TimeHelper:AddDelayTimer(function()
            self.close_timer = nil
            -- TimeHelper:AddDelayFrameTimer(function()
                ViewMgr:CloseView(NewSkillPopupView) 
            -- end,3)
        end,self.ShowTime/1000)
    AudioMgr:PlayEffect(AudioType.UI,"skill_get_new")
end

function NewSkillPopupView:CloseCallback()
    if self.close_timer ~= nil then
        TimeHelper:CancelTimer(self.close_timer)
        self.close_timer = nil
    end
    if SkillData.Instance:WaitPopupNewSkillCount() ~= 0 then
        -- TimeHelper:AddDelayFrameTimer(function()
        --     if SkillData.Instance:WaitPopupNewSkillCount() ~= 0 then
                ViewMgr:OpenView(NewSkillPopupView)
        --     end
        -- end,3)
    end
end