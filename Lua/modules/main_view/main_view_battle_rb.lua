---------------------------MainViewBattleRB 管理战斗主界面中的右下角按钮群的展示和事件------------------------------

------skill_description

MainViewBattleRB = MainViewBattleRB or DeclareMono("MainViewBattleRB", UIWFlushPanel)

MainViewBattleRB.SimpleUILv = 30        --简易UI表现等级上限
----Auto===========Toggle
----Anim===========Animator	stage(int):0.输入指令状态,1.回合播放状态  next_cmd(trigger)切换动画 sel_tar(bool)是否在选择目标状态
----AutoFirstIcon==UIImageSpriteSet
----AutoFirstLv====Text
----AutoSecIcon====UIImageSpriteSet
----AutoSecLv======Text
----AutoFirstName====Text
----AutoSecName======Text

----PetBtn=========GameObject
----SkillBtn=======GameObject
----ItemBtn========GameObject
----AtkBtn=========GameObject
----Selector=======GameObject
----AutoBeh1=======LuaUIEventClick
----AutoBeh2=======LuaUIEventClick

----PetCmdDisables===GameObject[]	非主角指令时需要禁用的按钮的遮挡对象

----LowLvHideBtns====GameObject[] 30级之前需要屏蔽的按钮

---TargetSelTipName==Text 选择目标tips技能名标识
-- local tmp_auto_move_icon_name = {
-- 	[BATTLE_MOVE_TYPE.PERFORM_SKILL] = "技能",
-- 	[BATTLE_MOVE_TYPE.NORMAL_ATTACK] = "GongJi",
-- 	[BATTLE_MOVE_TYPE.NORMAL_DEFENCE] = "FangYu",
-- 	[BATTLE_MOVE_TYPE.USE_ITEM] = "道具",
-- 	[BATTLE_MOVE_TYPE.RUN_AWAY] = "TaoPao",
-- 	[BATTLE_MOVE_TYPE.CHANGE_POSITION] = "WeiZhi",
-- 	[BATTLE_MOVE_TYPE.CHANGE_PET] = "换宠",
-- 	[BATTLE_MOVE_TYPE.SEAL_PET] = "封宠",
-- }

local base_cmd_skill_info = {
    [BATTLE_MOVE_TYPE.NORMAL_ATTACK] = {sp_name = "1", name = Language.Main.Battle.Attack},
    [BATTLE_MOVE_TYPE.NORMAL_DEFENCE] = {sp_name = "31", name = Language.Main.Battle.Defence},
    [BATTLE_MOVE_TYPE.CHANGE_POSITION] = {sp_name = "41", name = Language.Main.Battle.Position},
    [BATTLE_MOVE_TYPE.RUN_AWAY] = {sp_name = "51", name = Language.Main.Battle.Escape}
}

function MainViewBattleRB:MainViewBattleRB()
    self.auto_eff_handle = nil
    self.battle_data = BattleData.Instance
    self.base_data = self.battle_data:BaseData()
    self.data_cares = {
        {data = self.base_data, func = self.OnAutoChange, keys = {"auto"}},
        {data = self.base_data, func = self.OnRoundPhaseChange, keys = {"round_phase"}},
        {data = self.base_data, func = self.OnCmdIChange, keys = {"command_i"}},
        {data = self.base_data, func = self.OnCmdIChangeUI, keys = {"command_i"}},
        -- {data = self.base_data.ch_info, func = self.OnCmdIChangeUI},
        {data = self.base_data, func = self.OnSelTargetStateChange, keys = {"sel_tar_type"}},
        {data = self.base_data, func = self.OnSelTargetSkillChange, keys = {"sel_tar_type"}},
        {data = self.battle_data:ViewInfo(), func = self.OnOpenedViewChange, keys = {"opened_sub"}},
        {data = self.base_data, func = self.OnCountDownChange, keys = {"cmd_left_time", "round_phase", "auto"}},
        {data = self.battle_data:AutoBehaviour(1), func = BindTool.Bind(self.OnAutoBehaviourChange, self, 1)},
        {data = self.battle_data:AutoBehaviour(2), func = BindTool.Bind(self.OnAutoBehaviourChange, self, 2)},
        {data = self.battle_data:ViewInfo(), func = BindTool.Bind(self.OnAutoMoveNoMp, self, 1), keys = {"move1_nomp"}, init_call = false},
        {data = self.battle_data:ViewInfo(), func = BindTool.Bind(self.OnAutoMoveNoMp, self, 2), keys = {"move2_nomp"}, init_call = false},
        {data = self.battle_data:ViewInfo(), func = BindTool.Bind(self.FlushStrategy, self), keys = {"select_strategy", "clear_all"}, init_call = false},
        -- {data = self.base_data, func = self.OnSelTargetStateChange, keys = {"sel_tar_type"}},
        {data = RoleData.Instance:GetBaseData(), func = BindTool.Bind(self.OnLvChange, self), keys = {"level"}},
    }
end
function MainViewBattleRB:Awake()
    UIWFlushPanel.Awake(self)
    self.Anim:ListenEvent("btn_immediately_enter", BindTool.Bind(self.OnCmdIChangeUI, self))
    GuideManager.Instance:RegisterGetGuideUi("AutoBeh1",function ()
        return self.AutoBeh1
    end)
    GuideManager.Instance:RegisterGetGuideUi("AutoBeh2",function ()
        return self.AutoBeh2
    end)
end


function MainViewBattleRB:OnLvChange()
    local act_flag = RoleData.Instance:GetRoleLevel() >= MainViewBattleRB.SimpleUILv
    for i = 1,self.LowLvHideBtns:Length() do
        self.LowLvHideBtns[i]:SetActive(act_flag)
    end
end

-- function MainViewBattleRB:OnEnable()
-- 	UIWFlushPanel.OnEnable(self)
-- 	-- self:OnRoundPhaseChange()
-- end

function MainViewBattleRB:OnDisable()
    --[[ GuideManager.Instance:UnRegisterGetGuideUi("AutoBeh1")
    GuideManager.Instance:UnRegisterGetGuideUi("AutoBeh2") ]]
    -- self:ClearSkillEffect()
    self.StrategyPanel:SetActive(false)
    self.StrategySel:SetActive(false)
    self.Anim:WriteDefaultValues()
end

function MainViewBattleRB:OnAutoChange()
    local auto = self.base_data.auto
    self.Auto.isOn = auto
    self.Anim:SetBool(APH("is_auto"), auto)
    self:OnAutoBehaviourChange(1)
    self:OnAutoBehaviourChange(2)
    if auto == true then
        if self.auto_eff_handle == nil then
            self.auto_eff_handle = self.EffectTool:Play(10010215)
        end
    end
    UH.SetText(self.AutoDesc, Language.Main.Battle.AutoDesc[auto and 1 or 2])
end

function MainViewBattleRB:OnRoundPhaseChange()
    local stage = 1
    if self.base_data.round_phase ~= BATTLE_ROUND_PHASE.Command then
        stage = 1
    else
        --[[ local player_idx = self.battle_data:PlayerIdx()
        if not self.battle_data:IsChAlive(player_idx) then
            local pet_idx = self.battle_data:PlayerPetIdx()
            if self.battle_data:IsChAlive(pet_idx) then
                self.base_data.command_i = 2--next
                stage = 3--3 宠物
            else
                stage = 1--hide
            end
        else
            stage = 0
        end ]]
        stage = 0--主角
    end
    if self.base_data.battle_mode == BATTLE_MODE.BATTLE_MODE_STORY then
        stage = 0 
    end
    -- Debuger.LogError("round_phase!!!!!!!!===========%d,set stage===%d,cmdid===%d",self.base_data.round_phase,stage,self.base_data.command_i)
    self.Anim:SetInteger(APH("stage"), stage)
end
-- 技能图标特效
function MainViewBattleRB:ClearSkillEffect(i)
    if self.effect1 and i == 1 then
        self.EffectTool:Stop(self.effect1)
        self.effect1 = nil
    end
    if self.effect2 and i == 2 then
        self.EffectTool:Stop(self.effect2)
        self.effect2 = nil
    end
end
--没蓝提示
function MainViewBattleRB:OnAutoMoveNoMp(i)
    local is_first = (i == 1)
    local devalue_tex = is_first and self.DeValue1 or self.DeValue2
    local beh = self.battle_data:AutoBehaviour(i)
    if beh.type == BATTLE_MOVE_TYPE.NORMAL_ATTACK then
        devalue_tex:SetActive(true)
        TimeHelper:AddDelayTimer(function ()
            devalue_tex:SetActive(false)
        end,2)
    end
end
function MainViewBattleRB:OnAutoBehaviourChange(i)
    if self.base_data.auto == false then
        return
    end
    local is_first = (i == 1)
    local beh = self.battle_data:AutoBehaviour(i)
    local icon = is_first and self.AutoFirstIcon or self.AutoSecIcon
    local lv = is_first and self.AutoFirstLv or self.AutoSecLv
    local name_txt = is_first and self.AutoFirstName or self.AutoSecName
    local ai = is_first and self.AutoAI1 or self.AutoAI2
    -- self:ClearSkillEffect()
    ai:SetActive(beh.type == BATTLE_MOVE_TYPE.AI_SKILL)
    local flag = self.battle_data:IsChAlive(self.battle_data:PlayerPetIdx())
    local pet_ch = self.battle_data:GetChInfo(self.battle_data:PlayerPetIdx())
    -- local role_ch = self.battle_data:GetChInfo(self.battle_data:PlayerIdx())
    if pet_ch == nil or pet_ch.curhp > 0 then
    else
        flag = true
    end
    self.Beh2Role:SetActive(not flag)
    self.Beh2Pet:SetActive(flag)
    self.AutoSecLv:SetObjActive(not flag)
    
    if beh.type == BATTLE_MOVE_TYPE.PERFORM_SKILL or beh.type == BATTLE_MOVE_TYPE.AI_SKILL
    and beh.skill_id > 0 and beh.skill_lv > 0 then
        beh.skill_lv = (beh.skill_lv ~= nil and beh.skill_lv <= 0) and 1 or beh.skill_lv
        local cfg = SkillData.GetConfig(beh.skill_id, beh.skill_lv)
        -- icon.text = cfg ~= nil and cfg.skill_name or nil
        name_txt.text = cfg ~= nil and cfg.skill_name or nil
        if IS_AUDIT_VERSION then
            local audit_skill_index = SkillData.GetAuditSkillIndex(beh.skill_id)
            if audit_skill_index then
                local img_path, stream_has = ChannelAgentConsts.GetUIStremingAsset("RoleSkillIcon"..audit_skill_index)
                if stream_has then
                    local raw_obj = is_first and self.AutoFirstIconRawObj or self.AutoSecIconRawObj
                    local icon_obj = is_first and self.AutoFirstIconObj or self.AutoSecIconObj
                    raw_obj:SetActive(true)
                    icon_obj:SetActive(false)
                    ChannelAgentConsts.SetRawImg(is_first and self.AutoFirstIconRaw or self.AutoSecIconRaw, img_path)
                else
                    UH.SetSkillIconToImage(icon, beh.skill_id, beh.skill_lv)
                end
            else
                UH.SetSkillIconToImage(icon, beh.skill_id, beh.skill_lv)
            end
        else
            UH.SetSkillIconToImage(icon, beh.skill_id, beh.skill_lv)
        end
        
        lv.text = string.format(Language.Main.Battle.Ji, beh.skill_lv)
        local dec_config = SkillData.GetDecConfig(beh.skill_id, beh.skill_lv)
        if VersionTag.HasTag(VersionTag.ClipAnima2D) and dec_config and type(dec_config.asset_id) == "number" then
            if is_first then
                if self.effect1 == nil then
                    self.effect1 = self.EffectTool:PlayByEffectId(dec_config.asset_id, self.AutoFirstEff)
                    self.asset_id_1 = dec_config.asset_id
                elseif self.asset_id_1 ~= dec_config.asset_id then
                    self:ClearSkillEffect(1)
                    self.effect1 = self.EffectTool:PlayByEffectId(dec_config.asset_id, self.AutoFirstEff)
                    self.asset_id_1 = dec_config.asset_id
                end
            else
                if self.effect2 == nil then
                    self.effect2 = self.EffectTool:PlayByEffectId(dec_config.asset_id, self.AutoSecEff)
                    self.asset_id_2 = dec_config.asset_id
                elseif self.asset_id_2 ~= dec_config.asset_id then
                    self:ClearSkillEffect(2)
                    self.effect2 = self.EffectTool:PlayByEffectId(dec_config.asset_id, self.AutoSecEff)
                    self.asset_id_2 = dec_config.asset_id
                end
            end
        else
            if is_first then
                self:ClearSkillEffect(1)
            else
                self:ClearSkillEffect(2)
            end
        end
        if dec_config and dec_config.corner_icon ~= "" then
            for idx = 1, 2 do
                self["SkillCorner"..i][idx]:SetActive(dec_config.corner_icon ~= "")
            end
            self["CornerName"..i].text = dec_config.corner_icon
        else
            for idx = 1, 2 do
                self["SkillCorner"..i][idx]:SetActive(false)
            end
        end
    else
        if is_first then
            self:ClearSkillEffect(1)
        else
            self:ClearSkillEffect(2)
        end
        for idx = 1, 2 do
            self["SkillCorner"..i][idx]:SetActive(false)
        end
        -- icon.text =  tmp_auto_move_icon_name[beh.type]
        local cmd_info = base_cmd_skill_info[beh.type]
        if cmd_info ~= nil then
            name_txt.text = cmd_info.name
            --icon.SpriteName = cmd_info.sp_name
            UH.SetIconToImage(icon, cmd_info.sp_name, ICON_TYPE.SKILL)
            lv.text = nil
        end
    end
end

function MainViewBattleRB:OnCmdIChange()
    if self.base_data.command_i == 2 then
        self.Anim:SetTrigger(APH("next_cmd"))
        --宠物在场且死了时，没有第二次指令输入
        --[[ local pet_idx = self.battle_data:PlayerPetIdx()
        local pet_ch = self.battle_data:GetChInfo(pet_idx)
        -- if not BattleCtrl.Instance.skip_cmd_2 and (pet_ch == nil or pet_ch.curhp > 0) then
        if pet_ch == nil or pet_ch.curhp > 0 then
            self.Anim:SetTrigger(APH("next_cmd"))
        else
            self.battle_data:CmdComplete()
        end ]]
    -- else
    --     self.Anim:SetTrigger(APH("next_cmd"))
   --[[  else
        self.battle_data:CmdComplete() ]]
    end
    self.BtnCancel:SetActive(self.base_data.command_i == 2)
end

function MainViewBattleRB:flushBtnsEnable()
    local command_i = self.battle_data:BaseData().command_i
    if command_i == 1 then
        for i = 1, self.AdvancedCmdDisables:Length() do
            self.AdvancedCmdDisables[i]:SetActive(false)
        end
        self.EscapeDisable:SetActive(false)
        self.SealDisable:SetActive(not self:SealCondition())
    elseif command_i == 2 then
        local player_cmd = self.battle_data:CurCmdIdx() == self.battle_data:PlayerIdx()
        local last_cmd_type = self.battle_data:BaseData().last_cmd_type
        if player_cmd then
            local used_advanced =
                last_cmd_type == BATTLE_MOVE_TYPE.USE_FABAO or last_cmd_type == BATTLE_MOVE_TYPE.USE_ITEM or last_cmd_type == BATTLE_MOVE_TYPE.PERFORM_SKILL or
                last_cmd_type == BATTLE_MOVE_TYPE.SEAL_PET or
                last_cmd_type == BATTLE_MOVE_TYPE.CHANGE_POSITION or
                last_cmd_type == BATTLE_MOVE_TYPE.CHANGE_PET or 
                last_cmd_type == BATTLE_MOVE_TYPE.USE_FAKE_POTION or
                last_cmd_type == BATTLE_MOVE_TYPE.HEART_SKILL or
                last_cmd_type == BATTLE_MOVE_TYPE.CARD_SKILL or 
                last_cmd_type == BATTLE_MOVE_TYPE.HUANSHOU_SKILL or
                last_cmd_type == BATTLE_MOVE_TYPE.SEAL_YOUMING
            for i = 1, self.AdvancedCmdDisables:Length() do
                self.AdvancedCmdDisables[i]:SetActive(used_advanced)
            end
            self.EscapeDisable:SetActive(false)
            self.SealDisable:SetActive(not self:SealCondition())
        else
            local used_advanced = last_cmd_type == BATTLE_MOVE_TYPE.CHANGE_PET
            self.SealDisable:SetActive(false)
            self.SkillDisable:SetActive(not self:PetSkillCondition())
            for i = 1, self.PetCmdDisables:Length() do
                if used_advanced then
                    self.PetCmdDisables[i]:SetActive(true)
                else
                    self.PetCmdDisables[i]:SetActive(i ~= 3 and i ~= 4)
                end
            end
            self.EscapeDisable:SetActive(true)
        end
    end
end
function MainViewBattleRB:PetSkillCondition()
    local pet_idx = self.battle_data:PlayerPetIdx()
    local pet_ch = self.battle_data:GetChInfo(pet_idx)
    if pet_ch == nil or pet_ch.curhp > 0 then
        return true
    else
        if pet_ch and pet_ch.curhp == 0 then
            return true
        end
        return false
    end
end
function MainViewBattleRB:OnCmdIChangeUI(eve_param, anim_st_info)
    self:flushBtnsEnable()
end

function MainViewBattleRB:OnSelTargetStateChange()
    if GuideManager.Instance:IsGuide() then return end
    local sel_tar_ty = self.base_data.sel_tar_type
    self.Anim:SetBool(APH("sel_tar"), sel_tar_ty ~= BATTLE_MOVE_TYPE.NONE)
    local scene_data = BattleData.Instance:SceneData()
    if scene_data ~= nil then
        if sel_tar_ty == BATTLE_MOVE_TYPE.NORMAL_ATTACK then
            scene_data:CameraSwitch(2)
        elseif sel_tar_ty == BATTLE_MOVE_TYPE.PERFORM_SKILL or sel_tar_ty == BATTLE_MOVE_TYPE.HEART_SKILL then
            local skill_cfg = SkillData.GetConfig(self.base_data.sel_tar_param1, self.base_data.sel_tar_param2)
            scene_data:CameraSwitch(skill_cfg.target_side_type == 1 and 1 or 2)
        elseif sel_tar_ty == BATTLE_MOVE_TYPE.USE_ITEM then
            local use_item = MainViewBattleItemSel.item_id
            scene_data:CameraSwitch(SWITCHITEM[use_item] and 2 or 1)
        elseif sel_tar_ty == BATTLE_MOVE_TYPE.USE_FAKE_POTION then
            local use_item = MainViewBattleItemSel.item_id
            scene_data:CameraSwitch(SWITCHITEM[use_item] and 2 or 1)
        elseif sel_tar_ty == BATTLE_MOVE_TYPE.SEAL_PET or sel_tar_ty == BATTLE_MOVE_TYPE.SEAL_YOUMING then
            scene_data:CameraSwitch(2)
        elseif sel_tar_ty == BATTLE_MOVE_TYPE.USE_FABAO then
            local fabao_id = MainViewBattleFabaoItem.fabao_id
            local cfg = Config.talisman_cfg_auto.talisman_attribute[fabao_id]
            scene_data:CameraSwitch(cfg.target_side_type)
        elseif sel_tar_ty == BATTLE_MOVE_TYPE.STRATEGY then
            scene_data:CameraSwitch(0)
        else
            scene_data:CameraSwitch(0)
        end
    end
end

function MainViewBattleRB:OnClickCamSwitch()
    local scene_data = BattleData.Instance:SceneData()
    if scene_data ~= nil then
        scene_data:SelCameraChange()
    end
end

function MainViewBattleRB:OnSelTargetSkillChange()
    if self.base_data.sel_tar_type == BATTLE_MOVE_TYPE.PERFORM_SKILL or self.base_data.sel_tar_type == BATTLE_MOVE_TYPE.HEART_SKILL then
        local skill_cfg = SkillData.GetConfig(self.base_data.sel_tar_param1)
        self.TargetSelTipName.text = skill_cfg.skill_name
        UH.SetText(self.TargetSelTipName, skill_cfg.skill_name)--不知道他就是用上面那句不显示用下面这句显示
    elseif self.base_data.sel_tar_type == BATTLE_MOVE_TYPE.USE_ITEM then
        local item
        local column_grids = BagData.Instance:GetColumnGrids(self.base_data.sel_tar_param1)
        if column_grids ~= nil then
            item = column_grids[self.base_data.sel_tar_param2]
        end
        if item == nil then
            self.TargetSelTipName.text = string.format("UnkownGrid[%d,%d]", self.base_data.sel_tar_param1, self.base_data.sel_tar_param2)
        else
            self.TargetSelTipName.text = item:Name()
        end
    elseif self.base_data.sel_tar_type == BATTLE_MOVE_TYPE.USE_FAKE_POTION then
        local item = self.base_data.sel_tar_param3
        if item == nil then
            self.TargetSelTipName.text = string.format("UnkownGrid[%d,%d]", self.base_data.sel_tar_param1, self.base_data.sel_tar_param2)
        else
            self.TargetSelTipName.text = item:Name()
        end
    elseif self.base_data.sel_tar_type == BATTLE_MOVE_TYPE.SEAL_PET or self.base_data.sel_tar_type == BATTLE_MOVE_TYPE.SEAL_YOUMING then
        local item
        local column_grids = BagData.Instance:GetColumnGrids(self.base_data.sel_tar_param1)
        if column_grids ~= nil then
            item = column_grids[self.base_data.sel_tar_param2]
        end
        if item == nil then
            -- self.TargetSelTipName.text = string.format("UnkownGrid[%d,%d]",self.base_data.sel_tar_param1,self.base_data.sel_tar_param2)
        else
            self.TargetSelTipName.text = item:Name()
        end
    elseif self.base_data.sel_tar_type == BATTLE_MOVE_TYPE.STRATEGY then
        self.TargetSelTipName.text = Language.Main.Battle.Strategy[self.base_data.sel_tar_param1]
    else
        self.TargetSelTipName.text = Language.Main.Battle.Attack
    end
end

function MainViewBattleRB:OnOpenedViewChange()
    local sub_t = self.battle_data:ViewInfo().opened_sub
    local sel_par = nil
    if sub_t == BattleSubView.Skill then
        sel_par = self.SkillBtn
    elseif sub_t == BattleSubView.Pet then
        sel_par = self.PetBtn
    elseif sub_t == BattleSubView.Item then
        sel_par = self.ItemBtn
    elseif sub_t == BattleSubView.Fabao then
        sel_par = self.FabaoBtn
    else
        sel_par = self.AtkBtn
    end
    self.Selector:SetParent(sel_par, false)
    self.Selector:SetAsFirstSibling()
end

function MainViewBattleRB:OnAutoBeh1Click()
    self.battle_data:SetSkillSelData(true, 1)
end

function MainViewBattleRB:OnAutoBeh2Click()
    self.battle_data:SetSkillSelData(true, 2)
end

function MainViewBattleRB:OnAutoStateChange(is_auto)
    if is_auto then
        if self.auto_eff_handle == nil then
            self.auto_eff_handle = self.EffectTool:Play(10010215)
        end
    else
        if self.auto_eff_handle ~= nil then
            self.EffectTool:Stop(self.auto_eff_handle)
            self.auto_eff_handle = nil
        end
    end
    if GuideManager.Instance:IsGuide() and not GuideManager.Instance:GetCurStepCLickSame("BattleAutoBtn") then
        return
    end
    if BattleData.IsStory() then return end
    BattleCtrl.Instance:SendAutoBattle(is_auto)
    --演算阶段进行修改
    if self.base_data.round_phase == BATTLE_ROUND_PHASE.Client then
        PublicPopupCtrl.Instance:Center(Language.Main.Battle.AutoTip[is_auto and 1 or 2])
    end
end

function MainViewBattleRB:OnCountDownChange()
    --self.base_data.round_phase ~= BATTLE_ROUND_PHASE.Command or 
    self.AutoImg.raycastTarget = true-- self.base_data.cmd_left_time > BattleData.Instance:GetCountDownOffset()
end

function MainViewBattleRB:OnClickEscape()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    -- Debuger.LogError("MainViewBattleRB:OnClickEscape")
    BattleCtrl.Instance:SetMove(self.base_data.command_i, BATTLE_MOVE_TYPE.RUN_AWAY)
end

function MainViewBattleRB:OnClickSeal()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    self.battle_data:ViewInfo().opened_sub = BattleSubView.Seal
end

function MainViewBattleRB:OnClickSealDisable()
    self:SealCondition(true)
end

function MainViewBattleRB:SealCondition(pop)
    -- LogErrorHL("SealCondition1")
    local flag = false
    for _, v in pairs(BattleData.Instance.ch_info) do
        -- LogErrorHL("SealCondition Level", v.level)
        if v.character_type == 2 and v.level == 1 then
            flag = true
        end
    end
    if not flag then
        -- LogErrorHL("SealCondition2")
        if pop then
            PublicPopupCtrl.Instance:Center(ErrorText[53])
        end
        return false
    end
    if PetData.Instance.pet_list:Length() >= Config.pet_cfg_auto.other[1].max_pet_bag then
        if pop then
            PublicPopupCtrl.Instance:Center(ErrorText[54])
        end
        return false
    end
    return true
end

function MainViewBattleRB:OnClickPos()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    BattleCtrl.Instance:SetMove(self.base_data.command_i, BATTLE_MOVE_TYPE.CHANGE_POSITION)
    -- Debuger.LogError("MainViewBattleRB:OnClickPos")
end

function MainViewBattleRB:OnClickPet()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    -- Debuger.LogError("MainViewBattleRB:OnClickPet")
    self.battle_data:ViewInfo().opened_sub = BattleSubView.Pet
end

function MainViewBattleRB:OnClickItem()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    -- Debuger.LogError("MainViewBattleRB:OnClickItem")
    self.battle_data:ViewInfo().opened_sub = BattleSubView.Item
end

function MainViewBattleRB:OnClickFaBao()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    -- Debuger.LogError("MainViewBattleRB:OnClickItem")
    self.battle_data:ViewInfo().opened_sub = BattleSubView.Fabao
end
function MainViewBattleRB:FlushStrategy()
    local select = self.battle_data:ViewInfo().select_strategy
    local clear_all = self.battle_data:ViewInfo().clear_all
    if select == true then
        self.StrategySel:SetActive(not select)
    end
    if clear_all == true then
        self.StrategySel:SetActive(not clear_all)
        BattleData.Instance:ViewInfo().clear_all = false
    end
end
function MainViewBattleRB:OnClickStrategy()
    self.battle_data:ViewInfo().select_strategy = false
    if self.StrategySel.activeSelf then
        self.StrategyPanel:SetActive(false)
        self.StrategySel:SetActive(not self.StrategySel.activeSelf)
    else
        self.StrategyPanel:SetActive(true)
        self.StrategySel:SetActive(not self.StrategySel.activeSelf)
    end
end

function MainViewBattleRB:OnClickDefence()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    BattleCtrl.Instance:SetMove(self.base_data.command_i, BATTLE_MOVE_TYPE.NORMAL_DEFENCE)
    -- Debuger.LogError("MainViewBattleRB:OnClickDefence")
end

function MainViewBattleRB:OnClickAttack()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    -- Debuger.LogError("MainViewBattleRB:OnClickAttack")
    BattleCtrl.Instance:EnterSelectTargetStateByAtk()
end

function MainViewBattleRB:OnClickSkill()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    -- Debuger.LogError("MainViewBattleRB:OnClickSkill")
    self.battle_data:SetSkillSelData(true)
end

function MainViewBattleRB:OnClickBack()
    BattleCtrl.Instance:ExitSelectTargetState()
end
function MainViewBattleRB:OnClickCancel()
    if self.base_data.command_i == 2 then
        BattleCtrl.CSRoleUndo()
        self.base_data.command_i = 1
        self.Anim:SetTrigger(APH("next_cmd"))
        -- 撤回MP药的使用消耗
        if self.base_data.last_cmd_type == BATTLE_MOVE_TYPE.USE_ITEM and self.battle_data:GetMpCanUseTimeMarkCancel() then
            self.battle_data:SetMpCanUseTime(self.base_data.mp_item_can_use_tiems + 1)
        end 
    end
end