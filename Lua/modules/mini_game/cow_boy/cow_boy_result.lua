CowBoyWinView = CowBoyWinView or DeclareView("CowBoyWinView", "mini_game/cow_boy_win")
VIEW_DECLARE_MASK(CowBoyWinView, ViewMask.Block)
function CowBoyWinView:CowBoyWinView()
    self.close_fun = nil
end

function CowBoyWinView:LoadCallback(param_t)
    if not TableIsEmpty(param_t) then
        self.add_hp = param_t.add_hp
        if self.add_hp and self.add_hp > 0 then
            UH.SetText(self.TxtFightAdd, Format(Language.CowBoy.FightAdd, self.add_hp))
            self.TxtFightAdd:SetObjActive(true)
            self.FightAddBg:SetActive(true)
        else
            self.TxtFightAdd:SetObjActive(false)
            self.FightAddBg:SetActive(false)
        end
        self.close_fun = param_t.close_fun
    end
end

function CowBoyWinView:OnCloseClick()
    ViewMgr:CloseView(CowBoyWinView)
end

function CowBoyWinView:CloseCallback()
    if self.close_fun then
        self.close_fun()
    end
end

----------------------------CowBoyResultView----------------------------
CowBoyResultView = CowBoyResultView or DeclareView("CowBoyResultView", "mini_game/cow_boy_result")
VIEW_DECLARE_MASK(CowBoyResultView, ViewMask.Block)
function CowBoyResultView:CowBoyResultView()
    self.Data = CowBoyData.Instance
end

function CowBoyResultView:LoadCallback(param_t)
    self.is_win = param_t.is_win
    self.next_fun = param_t.next_fun
    self.again_fun = param_t.again_fun
    self.ObjWin:SetActive(self.is_win)
    if not self.is_win then
        if param_t.can_not_final then
            UH.SpriteName(self.FaildStrImg, "_LocGuLiWenZi2")
        else
            UH.SpriteName(self.FaildStrImg, "_LocGuLiWenZi")
        end
    end
    local now_level = self.Data:GetLevelDataNowLevel()
    self.ObjClose:SetActive(false)
    local showed_close = UnityPlayerPrefs.HasKey(PrefKeys.MiniGame2JudgeClose())
    if LoginData.Instance:LoginState() ~= LoginStateType.Logined and now_level == self.Data:OtherSeq(1) then
        -- 如果是打完第x关创角关，并且没登录，需要进入随机创角界面
        self.ObjReturn:SetActive(false)
        self.ObjNext:SetActive(false)
        self.ObjAgain:SetActive(false)
        self.ObjCreate:SetActive(true)
    elseif now_level == 3 and self.Data.now_use_level_data.level_type == 3 and not showed_close then
        self.ObjClose:SetActive(true)
        self.ObjReturn:SetActive(false)
        self.ObjNext:SetActive(false)
        self.ObjAgain:SetActive(false)
        self.ObjCreate:SetActive(false)
        UnityPlayerPrefs.SetInt(PrefKeys.MiniGame2JudgeClose(), 1)
    else
        self.ObjCreate:SetActive(false)
        self.ObjReturn:SetActive(true)
        if self.is_win and (not self.Data:NowIsMaxLevel()) then
            self.ObjNext:SetActive(true)
            self.ObjAgain:SetActive(false)
        else
            self.ObjNext:SetActive(false)
            self.ObjAgain:SetActive(true)
        end
    end
end

function CowBoyResultView:OnReturnClick()
    ViewMgr:CloseView(CowBoyResultView)
    -- ViewMgr:CloseView(CowBoyGameView)
    ViewMgr:OpenView(CowBoyLevelView,{close_game_view = true})
end

function CowBoyResultView:OnNextClick()
    ViewMgr:CloseView(CowBoyResultView)
    if self.next_fun then
        self.next_fun()
    end
end

function CowBoyResultView:OnAgainClick()
    ViewMgr:CloseView(CowBoyResultView)
    if self.again_fun then
        self.again_fun()
    end
end

function CowBoyResultView:OnCreateClick()
    ViewMgr:CloseView(CowBoyResultView)
    ViewMgr:CloseView(CowBoyGameView)
    ViewMgr:OpenView(CowBoyProfSelectView)
end

function CowBoyResultView:OnCloseClick()
    ViewMgr:CloseView(CowBoyResultView)
    ViewMgr:CloseView(CowBoyGameView)
end

----------------------------CowBoyGetView----------------------------
CowBoyGetView = CowBoyGetView or DeclareView("CowBoyGetView", "mini_game/cow_boy_get")
VIEW_DECLARE_MASK(CowBoyGetView, ViewMask.Block)
function CowBoyGetView:CowBoyGetView()
    self.close_fun = nil
end

function CowBoyGetView:LoadCallback(param_t)
    self.close_fun = param_t.close_fun
    self.data = param_t.data

    self.IconObj:SetActive(self.data.cell_type ~= CowBoyCfg.CellType.Skill)
    self.SkillIconObj:SetActive(self.data.cell_type == CowBoyCfg.CellType.Skill)

    if self.data.cell_type == CowBoyCfg.CellType.Skill then
        local skill_cfg = Cfg.SkillDescPartnerAndRole(self.data.cell_info.match_id, 1)
        UH.SetIcon(self.SkillIcon, skill_cfg.icon_id, ICON_TYPE.SKILL)
    else
        UH.SpriteName(self.Icon, "HuoDeWuPin"..self.data.cell_type)
    end
    UH.SetText(self.TxtName, self.data.cell_info.event_title)
    if self.data.cell_type == CowBoyCfg.CellType.Weapon then
        if self.data.cell_info.event_text then
            UH.SetText(self.TxtDesc, Format(self.data.cell_info.event_text, self.data.hp))
        elseif IS_EDITOR then
            UH.SetText(self.TxtDesc, "配置错误")
        else
            UH.SetText(self.TxtDesc, "")
        end
    else
        UH.SetText(self.TxtDesc, self.data.cell_info.event_text)
    end
end

function CowBoyGetView:OnCloseClick()
    ViewMgr:CloseView(CowBoyGetView)
end

function CowBoyGetView:CloseCallback()
    if self.close_fun then
        self.close_fun()
    end
    Delete(self, "mod_obj")
end
-----------------------------------
CowBoyBoxEffectView = CowBoyBoxEffectView or DeclareView("CowBoyBoxEffectView", "mini_game/cow_boy_box_effect")
VIEW_DECLARE_MASK(CowBoyGetView, ViewMask.Block)
function CowBoyBoxEffectView:CowBoyBoxEffectView()
end

function CowBoyBoxEffectView:LoadCallback(param)
    if param and param.close_func then
        self.call_func = param.close_func
    end
    self.effect_handle = self.EffectTool:Play("7165480")
end

function CowBoyBoxEffectView:OnEffectEnd()
    self.EffectTool:Stop(self.effect_handle)
    TimeHelper:AddDelayFrameTimer(function()
        ViewMgr:CloseView(CowBoyBoxEffectView)
        if self.call_func ~= nil then
            self.call_func()
            self.call_func = nil
        end
    end,2)
end