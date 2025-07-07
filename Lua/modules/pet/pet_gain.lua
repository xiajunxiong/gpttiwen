PetGain = PetGain or DeclareView("PetGain", "pet/pet_gain")
VIEW_DECLARE_LEVEL(PetGain, ViewLevel.L1)
-- VIEW_DECLARE_MASK(PetGain, ViewMask.BlockClose)
function PetGain:PetGain()
    GuideManager.Instance:RegisterGetGuideUi("PetEggClose",
    function ()
        return self.GuideFrame,function ()
            ViewMgr:CloseView(PetGain)
            ViewMgr:CloseView(BagView)
        end
	end)
end

function PetGain:LoadCallback(data)
    self.close_call = nil
    self.NormalObj:SetActive(not data.is_xilian)
    if data then
        if data.is_xilian then
            if self.ui_obj == nil then
                self.ui_obj = UIObjDrawer.New()
                self.ModelShow:SetShowData({
                    ui_obj = self.ui_obj,
                    view = self
                })
            end
            local pet_info = PetData.Instance.new_pet
            local normal_pet_cfg = PetData.Instance:GetPetCfg(pet_info.config.prototype_id)
            self.ui_obj:SetData(DrawerHelper.GetNpcPath(normal_pet_cfg.res_id),pet_info:FeiShengTimes())
            self.EffectTool:Play("10010862")
            self.LimitSignItem:SetType(pet_info:LimitType())
        end
        if data.close_call then
            self.close_call = data.close_call
        end
    end
    UH.SpriteName(self.GongXiSprite, data.is_xilian and "_LocChongWuBianYi" or "_LocGongXiHuoDe")
    if RoleLevel() < PC_NEW_ROLE_STATE_LEVEL and IS_STANDALONE() then
        self.pc_auto_close_delay_handle = TimeHelper:AddDelayTimer(function ()
            ViewMgr:CloseView(PetGain)
        end,3)
    end
end

function PetGain:OnMutantEffEnd()
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(PetData.Instance.new_pet:ResId()),PetData.Instance.new_pet:FeiShengTimes())
    self.NormalBgObj:SetActive(false)
    self.MutantBgObj:SetActive(true)
    self.delay_hide = TimeHelper:AddDelayTimer(function ()
        self.NormalObj:SetActive(true)
        self.XilianObj:SetActive(false)
    end, self.DelayHide / 10.0)
end

function PetGain:CloseCallback()
    if self.close_call ~= nil then
        self.close_call()
    end
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
    if self.delay_hide ~= nil then
        TimeHelper:CancelTimer(self.delay_hide)
        self.delay_hide = nil
    end
    if self.pc_auto_close_delay_handle then
        TimeHelper:CancelTimer(self.pc_auto_close_delay_handle)
        self.pc_auto_close_delay_handle = nil
    end
    -- 判断下有无宠物福利奖励
    RewardCtrl.Instance:PlayPetIntensifyRewardDialog()
end
-----------------------------------------------------
PetGainPanel = PetGainPanel or DeclareMono("PetGainPanel", UIWFlushPanel)
function PetGainPanel:PetGainPanel()
    self.lang = Language.Pet
    self.data = PetData.Instance

    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self
        })
    end
end

function PetGainPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if TreasureHuntData.Instance.smart_data.is_guide_draw and QQBigPlayerCtrl.IsQQPC() then
        if self.close_delay_handle == nil then
            self.close_delay_handle = TimeHelper:AddDelayTimer(function ()
                self:OnClickMask()
            end,3)
        end
    end
end

function PetGainPanel:onFlush()
    local pet = self.data.new_pet
    if pet == nil then
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, "pet_gain")
    UH.SetText(self.Name, pet:Name())
    UH.SetText(self.Level, string.format("Lv.%d", pet:Level()))
    local v1 = pet:TotalGrowth()
    local v2 = pet:ConfigGrowth()
    UH.SetText(self.Group, string.format("%s/%s", v1, v2))
    self.EleAttr:SetData(pet:EleAttr())
    if v2 - v1 <= 5 then
        self.handle_delay = TimeHelper:AddDelayTimer(function ()
            self.EffectTool:Play(10010228)
        end, 0.9)
    end
    PetData.SetStrLevel(nil, self.StrLevelSp, pet:StrLevel())
    UH.SpriteName(self.QuaSp, PetData.PetLevel2[pet.config.quality])
    self.Mutant:SetActive(pet:IsMutant())
    self.MutantBg:SetActive(pet:IsMutant())
    self.NormalBg:SetActive(not pet:IsMutant() and pet:Quality() < 5)
    self.ShenShowBg:SetActive(pet:Quality() == 5)

    local total_growth_list = pet:TotalGrowthList()
    local config_growth_list = pet:ConfigGrowthList()
    self.AttrList:SetPet(pet)

    local total = {}
    for _, v in ipairs(pet:GetSkillAndGiftListNoEmpty()) do
        table.insert(total, v)
    end
    self.SkillList:SetData(total)
    
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet:ResId()),pet:FeiShengTimes())
    self.LimitSignItem:SetType(pet:LimitType())
end

function PetGainPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.close_delay_handle ~= nil then
        TimeHelper:CancelTimer(self.close_delay_handle)
        self.close_delay_handle = nil
    end
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
    self.ui_obj = nil
    if self.handle_delay then
        TimeHelper:CancelTimer(self.handle_delay)
    end
    self.handle_delay = nil
    self.data.new_pet = nil
    if FinishData.Instance:GetBattleMode() == BATTLE_MODE.BATTLE_MODE_PET_MONOPOLY and PetCtrl.Instance.delay_pet_list[1] == nil then
        FightCtrl.Instance:OpenPetIsland()
    end
    PetCtrl.Instance:PopDelayPetShow()
    if TreasureHuntData.Instance.treasure_hunt_count == 5 then
        TreasureHuntData.Instance.smart_data.is_change = not TreasureHuntData.Instance.smart_data.is_change
    end
    
end

function PetGainPanel:OnClickSkill(data)
    self.data:SetClickSkill(data)
    ViewMgr:OpenView(PetSkillTip)
end

function PetGainPanel:OnClickMask()
    if TreasureHuntData.Instance.smart_data.is_guide_draw then
        ViewMgr:CloseView(TreasureHuntView)
        ViewMgr:CloseView(TreasureHuntDetailView)
        TreasureHuntData.Instance.smart_data.is_guide_draw = false
    end
    ViewMgr:CloseView(PetGain)
end