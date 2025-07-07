LevelGiftNotice = LevelGiftNotice or DeclareView("LevelGiftNotice","level_gift_notice/level_gift_notice", Mod.LevelGiftNotice.Main)
VIEW_DECLARE_MASK(LevelGiftNotice, ViewMask.BgBlockClose)
function LevelGiftNotice:LevelGiftNotice()
    self.is_can_get = false
    self.handle_care = RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.OnFlush, self), "level")
end

function LevelGiftNotice:CloseCallback()
    if self.handle_care then
        RoleData.Instance:GetBaseData():Uncare(self.handle_care)
        self.handle_care = nil
    end
end
function LevelGiftNotice:OnFlush()
    local is_show, show_data = RewardData.Instance:LevelGiftNoticeShowCheck()
    if is_show == true then
        UH.SpriteName(self.IconSp, show_data.icon)
        UH.SetText(self.Name, show_data.show_name)
        UH.SetText(self.Desc, show_data.show_descr)
        UH.SetText(self.Level, string.format(Language.LevelGiftNotice.OpenLevel, show_data.level))
        local level = RoleData.Instance:GetRoleLevel()
        self.is_can_get = level >= show_data.level
        UH.SetText(self.BtnTex, self.is_can_get and Language.LevelGiftNotice.Get or Language.LevelGiftNotice.OK)
        self.List:SetData(show_data.rewards)
        if self.is_can_get == true then
            self.handle_effect = self.EffectTool:Play(2161048)
        else
            if self.handle_effect then
                self.EffectTool:Stop(self.handle_effect)
                self.handle_effect = nil
            end
        end
    else
        ViewMgr:CloseView(LevelGiftNotice)
    end
end
function LevelGiftNotice:OnClickButton()
    if self.is_can_get then
        ViewMgr:OpenViewByKey(Mod.Reward.Upgrade)
    else 
        ViewMgr:OpenViewByKey(Mod.Activity.Daily)
    end
    ViewMgr:CloseView(LevelGiftNotice)
end
LevelGiftNoticeSub = LevelGiftNoticeSub or DeclareMono("LevelGiftNoticeSub",UIWFlushPanel)
function LevelGiftNoticeSub:LevelGiftNoticeSub()
    self.data_cares = {
		{data = RoleData.Instance:GetBaseData(), func = self.FlushPanel, keys = {"level"}},
		{data = RewardData.Instance.upgrade_change, func = self.FlushPanel, init_call = false},
    }
end

function LevelGiftNoticeSub:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function LevelGiftNoticeSub:FlushPanel()
    local is_show, show_data = RewardData.Instance:LevelGiftNoticeShowCheck()
    if is_show == true then
        -- LogDG(show_data)
        UH.SpriteName(self.IconSprite, show_data.icon)
        UH.SetText(self.Name, show_data.show_name)
        UH.SetText(self.Level, string.format(Language.LevelGiftNotice.LockLevel, show_data.level))
        local level = RoleData.Instance:GetRoleLevel()
        local is_can_get = level >= show_data.level
        self.RedPoint:SetActive(is_can_get)
    end
end
function LevelGiftNoticeSub:OnClick()
    ViewMgr:OpenView(LevelGiftNotice)
end