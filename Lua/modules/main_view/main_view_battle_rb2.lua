MainViewBattleRB2 = MainViewBattleRB2 or DeclareMono("MainViewBattleRB2", UIWFlushPanel)
----Anim===========Animator	stage(int):0.输入指令状态,1.回合播放状态  next_cmd(trigger)切换动画 sel_tar(bool)是否在选择目标状态
function MainViewBattleRB2:MainViewBattleRB2()
    -- 战斗的基本操作
    self.auto_skill_effect = {nil, nil}
    self.auto_skill_asset = {-1, -1}
    self.skill_num = 0
    self.base_cmd_skill_info = {
        [BATTLE_MOVE_TYPE.NORMAL_ATTACK] = {sp_name = "1", name = Language.Main.Battle.Attack},
        [BATTLE_MOVE_TYPE.NORMAL_DEFENCE] = {sp_name = "31", name = Language.Main.Battle.Defence},
        [BATTLE_MOVE_TYPE.CHANGE_POSITION] = {sp_name = "41", name = Language.Main.Battle.Position},
        [BATTLE_MOVE_TYPE.RUN_AWAY] = {sp_name = "51", name = Language.Main.Battle.Escape}
    }
    self.normal =  {skill_id = -BATTLE_MOVE_TYPE.NORMAL_ATTACK, sp_name = "1", name = Language.Main.Battle.Attack, is_base = true, skillDesc = Language.Main.Battle.NormalAttack, skillTip = Language.Main.Battle.AttackTip}
    self.battle_data = BattleData.Instance
    self.base_data = self.battle_data:BaseData()
    self.data_cares = {
        {data = self.base_data, func = self.OnAutoChange, keys = {"auto"}},
        {data = self.base_data, func = self.OnCmdIChange, keys = {"command_i"}},
        -- {data = self.base_data, func = self.OnCmdIChangeUI, keys = {"command_i"}},
        {data = self.battle_data:AutoBehaviour(1), func = BindTool.Bind(self.OnAutoBehaviourChange, self, 1)},
        {data = self.battle_data:AutoBehaviour(2), func = BindTool.Bind(self.OnAutoBehaviourChange, self, 2)},
        {data = self.base_data, func = self.OnSelTargetStateChange, keys = {"sel_tar_type"}},
        {data = self.base_data, func = self.OnRoundPhaseChange, keys = {"round_phase"}},
        {data = self.battle_data:ViewInfo(), func = self.FlushSkillList, keys = {"skill_sel_cmdi"}},
    }
    self.sv2 = Vector2.zero
    self.ov2 = Vector2.zero
end

function MainViewBattleRB2:Awake()
    UIWFlushPanel.Awake(self)
    --用于刷新变化
    self.Anim:ListenEvent("btn_immediately_enter", BindTool.Bind(self.OnCmdIChangeUI, self))
    GuideManager.Instance:RegisterGetGuideUi("BattleAutoBeh1",function ()
        return self.BattleAutoBeh1
    end)
    GuideManager.Instance:RegisterGetGuideUi("BattleAutoBeh2",function ()
        return self.BattleAutoBeh2
    end)
    GuideManager.Instance:RegisterGetGuideUi("BattleAutoBtn2",function ()
        return self.BattleAutoBtn2
    end)
    GuideManager.Instance:RegisterGetGuideUi("BattleFangYuBtn2",function ()
        return self.BattleFangYuBtn2
    end)
    GuideManager.Instance:RegisterGetGuideUi("BattleOtherBtn",function ()
        return self.BattleOtherBtn
    end)
    GuideManager.Instance:RegisterGetGuideUi("BattleSealBtn2",function ()
        if self.OpList:SetupCompleted() then
            return self:GuideOpList("FengYin")
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("BattleFabaoBtn2",function ()
        if self.OpList:SetupCompleted() then
            return self:GuideOpList("FaBao")
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("BattleItemBtn2",function ()
        if self.OpList:SetupCompleted() then
            return self:GuideOpList("WuPin")
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("BattlePetBtn2",function ()
        if self.OpList:SetupCompleted() then
            return self:GuideOpList("ChongWu")
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("BattleEscapeBtn2",function ()
        if self.OpList:SetupCompleted() then
            return self:GuideOpList("TaoPao")
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("BattleFirstSkill2",function ()
        if self.SkillList:SetupCompleted() then
            local skill_id = GuideData.Instance:GetSkillId()
            if skill_id == nil then
                GuideManager.Instance:ForceStop()
            else
                --获取两个位置
                local MVBS = LuaMonoBehaviour.FindMono(MainViewBattleScene)
                local step_cfg = GuideManager.Instance:StepCfg()
                if step_cfg and MVBS and MVBS.grid_ui[step_cfg.step_param_2] then
                    local btn = self:GuideSkillList(skill_id)
                    if btn then
                        local p2 = UH.GetRootPosition(MVBS.grid_ui[step_cfg.step_param_2].EventClick.gameObject)
                        local p1 = UH.GetRootPosition(btn)
                        GuideManager.Instance:GuideView():StartFingerAction(p1, p2)
                        return btn, function ()
                            BattleCtrl.Instance:SetMove(self.base_data.command_i, BATTLE_MOVE_TYPE.PERFORM_SKILL, step_cfg.step_param_2 - 1, skill_id, 1)
                            GuideManager.Instance:GuideView():StopFingerAction()
                        end
                    end
                else
                    GuideManager.Instance:ForceStop()
                end
            end
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("BattleSecondSkill2",function ()
        if self.SkillList:SetupCompleted() then
            local skill_id = GuideData.Instance:GetSkillId2()
            if skill_id == nil then
                GuideManager.Instance:ForceStop()
            else
                --获取两个位置
                local MVBS = LuaMonoBehaviour.FindMono(MainViewBattleScene)
                local step_cfg = GuideManager.Instance:StepCfg()
                if step_cfg and MVBS and MVBS.grid_ui[step_cfg.step_param_2] then
                    local btn = self:GuideSkillList(skill_id)
                    if btn then
                        local p2 = UH.GetRootPosition(MVBS.grid_ui[step_cfg.step_param_2].EventClick.gameObject)
                        local p1 = UH.GetRootPosition(btn)
                        GuideManager.Instance:GuideView():StartFingerAction(p1, p2)
                        return btn, function ()
                            BattleCtrl.Instance:SetMove(self.base_data.command_i, BATTLE_MOVE_TYPE.PERFORM_SKILL, step_cfg.step_param_2 - 1, skill_id, 1)
                            GuideManager.Instance:GuideView():StopFingerAction()
                        end
                    end
                else
                    GuideManager.Instance:ForceStop()
                end
                -- return self:GuideSkillList(skill_id)
            end
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi(
        "BattleFirstPetSkill2",
        function()
            if self.SkillList:SetupCompleted() then
                --获取两个位置
                local MVBS = LuaMonoBehaviour.FindMono(MainViewBattleScene)
                local step_cfg = GuideManager.Instance:StepCfg()
                if step_cfg and MVBS and MVBS.grid_ui[step_cfg.step_param_2] then
                    local btn = self.SkillList.item_list[1].ClickObj
                    if btn then
                        local p2 = UH.GetRootPosition(MVBS.grid_ui[step_cfg.step_param_2].EventClick.gameObject)
                        local p1 = UH.GetRootPosition(btn)
                        GuideManager.Instance:GuideView():StartFingerAction(p1, p2)
                        return btn, function ()
                            local skill_id = self.SkillList.item_list[1]:GetData().skill_id
                            BattleCtrl.Instance:SetMove(self.base_data.command_i, BATTLE_MOVE_TYPE.PERFORM_SKILL, step_cfg.step_param_2 - 1, skill_id, 1)
                            GuideManager.Instance:GuideView():StopFingerAction()
                        end
                    end
                else
                    GuideManager.Instance:ForceStop()
                end
            end
        end
    )
    GuideManager.Instance:RegisterGetGuideUi(
        "BattleNormalAttack2",
        function()
            if self.SkillList:SetupCompleted() then
                --获取两个位置
                local MVBS = LuaMonoBehaviour.FindMono(MainViewBattleScene)
                local step_cfg = GuideManager.Instance:StepCfg()
                if step_cfg and MVBS and MVBS.grid_ui[step_cfg.step_param_2] then
                    local btn = self:GuideSkillList(-BATTLE_MOVE_TYPE.NORMAL_ATTACK)
                    local p2 = UH.GetRootPosition(MVBS.grid_ui[step_cfg.step_param_2].EventClick.gameObject)
                    local p1 = UH.GetRootPosition(btn)
                    GuideManager.Instance:GuideView():StartFingerAction(p1, p2)
                    return btn, function ()
                        BattleCtrl.Instance:SetMove(self.base_data.command_i, BATTLE_MOVE_TYPE.NORMAL_ATTACK, step_cfg.step_param_2 - 1)
                        GuideManager.Instance:GuideView():StopFingerAction()
                    end
                else
                    GuideManager.Instance:ForceStop()
                end
            end
        end
    )
    --self:OnCmdIChangeUI()
    self.AISkill:SetActive(true)
end
function MainViewBattleRB2:GuideOpList(key)
    for i, v in pairs(self.OpList.item_list) do
        if v:GetData().sprite_name == key then
            return v.ClickObj
        end
    end
end
function MainViewBattleRB2:GuideSkillList(key)
    for k, v in pairs(self.SkillList.item_list) do
        if v:GetData().skill_id == key then
            return v.ClickObj, function ()
                
            end
        end
    end
end
function MainViewBattleRB2:OnDisable()
    --[[ GuideManager.Instance:UnRegisterGetGuideUi("AutoBeh1")
    GuideManager.Instance:UnRegisterGetGuideUi("AutoBeh2") ]]
    -- self:ClearSkillEffect()
    self.StrategyPanel:SetActive(false)
    self.StrategySel:SetActive(false)
    self.Anim:WriteDefaultValues()
end
--刷新技能列表和其他列表
function MainViewBattleRB2:OnCmdIChangeUI()
    local command_i = self.base_data.command_i
    local data1 = {}--Config.battle_operate_auto.operate_list
    local data2 = {}
    local is_open = false
    if command_i == 1 then
        for i, v in ipairs(Config.battle_operate_auto.operate_list) do
            if v.open and v.open ~= 0 then
                is_open = FunOpen.Instance:GetFunIsOpened(v.open)
            else
                is_open = true
            end
            if is_open then
                table.insert(data1, v)
            end
        end
        self.AISkill:SetActive(true)
    elseif command_i == 2 then
        --如果是宠物部分不显示
        local pet_ch = self.battle_data:GetChInfo(self.battle_data:PlayerPetIdx())
        if pet_ch ~= nil then
            for i, v in ipairs(Config.battle_operate_auto.operate_list) do
                if v.is_pet_can == 1 then
                    table.insert(data1, v)
                end
            end
            self.AISkill:SetActive(false)
        else
            for i, v in ipairs(Config.battle_operate_auto.operate_list) do
                if v.open and v.open ~= 0 then
                    is_open = FunOpen.Instance:GetFunIsOpened(v.open)
                else
                    is_open = true
                end
                if is_open then
                    table.insert(data1, v)
                end
            end
            --判断一动是否只进行了普攻防御逃跑
            self.AISkill:SetActive(self.base_data.last_cmd_type == BATTLE_MOVE_TYPE.NORMAL_ATTACK 
        or self.base_data.last_cmd_type == BATTLE_MOVE_TYPE.NORMAL_DEFENCE or self.base_data.last_cmd_type  == BATTLE_MOVE_TYPE.RUN_AWAY)
        end
    end
    -- LogDG("OnCmdIChangeUI")
    self.battle_data:SetSkillSelData(false, self.base_data.command_i)
    self.ToggleSkill.isOn = true
    self.OpList:SetCompleteCallBack(BindTool.Bind(self.OpListCallback, self))
    self.OpList:SetData(data1)
end
function MainViewBattleRB2:FlushSkillList()
    -- LogDG("FlushSkillList")
    local data2 = {}
    data2 = self:SkillData()
    --在最后新增一个普攻
    table.insert(data2, self.normal)
    self.skill_num = table.nums(data2)
    self.SkillList:SetCompleteCallBack(BindTool.Bind(self.SkillListCallback, self))
    self.SkillList:SetData(data2)
end
function MainViewBattleRB2:SkillData()
    local base_data = self.battle_data:BaseData()
    local view_data = self.battle_data:ViewInfo()
    local is_auto = base_data.auto
    local skill_info = nil
    local is_player_skill = true
    local show_type = 0

    if view_data.skill_sel_cmdi == 2 then -- 二动时
        local pet_ch = self.battle_data:GetChInfo(self.battle_data:PlayerPetIdx())
        if pet_ch ~= nil then
            is_player_skill = false
        else
            local last_cmd_type = base_data.last_cmd_type
            --检查一动是否进行
            if self.base_data.last_cmd_type ~= BATTLE_MOVE_TYPE.NORMAL_ATTACK and self.base_data.last_cmd_type ~= BATTLE_MOVE_TYPE.NORMAL_DEFENCE and self.base_data.last_cmd_type  ~= BATTLE_MOVE_TYPE.RUN_AWAY then
                return {}
            end
        end
    end
    
    MainViewBattleSkillSel.is_player_skill = is_player_skill
    if is_player_skill then
        skill_info = self.battle_data:PlayerSkillInfo()
    else
        local pet_list = self.battle_data:PetList()
        for _, v in pairs(pet_list) do
            if v.status == BATTLE_PET_STATUS.Zhan then
                skill_info = v.skill_list
                break
            end
        end
    end
    local data = {}
    if skill_info ~= nil then
        data = DataHelper.TableCopy(skill_info)
    end
    -- 这里还得判断是否有心法技能
    if is_player_skill then
        -- 提前把飞升技能拿出来
        local idx = nil
        for index, value in ipairs(data) do
            if value.skill_id == FlyUpData.Instance:GetSkillId() then
                idx = index
            end
        end
        local flyup_skill = nil
        if idx ~= nil then
            flyup_skill = table.remove(data, idx)
            flyup_skill.is_flyup_skill = true
        end
        --[[ if flyup_skill then
            table.insert(data, flyup_skill)
        end ]]
        --[[ local heart_sklill = self.battle_data:PlayerHeartSkill()
        heart_sklill = DataHelper.TableCopy(heart_sklill)
        local count = self.battle_data:PlayerHeartSkillCount()
        if count > 0 then
            for i, v in ipairs(heart_sklill) do
                table.insert(data, v)
            end
        end ]]
        self:SortSkill(data, is_player_skill, show_type)
        self:AddUnlock(data, is_player_skill, show_type)
        -- -- 检查是否有使用幻化卡技能
        -- local card_skill_info = LiveData.Instance:GetCardSkillInfo(BattleData.Instance.live_card_seq, true)
        -- if card_skill_info then
        --     table.insert(data, card_skill_info)
        -- end

        -- 检查是否有使用幻兽技能
        local huanshou_skill_info = HuanShouData.Instance:GetBattleSkillInfo(BattleData.Instance.huanshou_skill_info)
        if huanshou_skill_info and is_player_skill then
            table.insert(data, 1,huanshou_skill_info)
        end
    end
    return data
end


function MainViewBattleRB2:AddUnlock(data, is_player_skill, show_type)
    local group_id = BattleData.Instance:BaseData().monster_group_id
	local need_add_unlock = true
	for k, v in pairs(Config.npc_demo_auto.other) do
		if group_id == v.monster_group_id then
			need_add_unlock = false
		end
	end
	if is_player_skill and need_add_unlock then
		local profession = RoleData.Instance:GetRoleProf()
        local config = SkillData.Instance.careerProfSkillList[profession]
        if config == nil then
            LogError("Skill config is NULL AND PROF == "..tostring(profession))
            return
        end
        local ret_data = DataHelper.TableCopy(config[0])
		table.sort(ret_data, function (a, b)
			if a.skill_learn_level ~= b.skill_learn_level then
				return a.skill_learn_level < b.skill_learn_level
			else
				 return a.skill_id < b.skill_id
			end
		end)
		if ret_data then
			for k, v in pairs(ret_data) do
				local need_add = true
				 for k1, v1 in pairs(data) do
					if SkillData.RoleViewShowPassiveSkill[v.skill_id] then
						need_add = false
						break
					end
					if v1.skill_id == v.skill_id then
						need_add = false
						break
					end
				end			
				if need_add then
					local add_info = {}
					add_info.skill_id = v.skill_id
					add_info.skill_level = 1
					add_info.skill_learn_level = v.skill_learn_level
					add_info.index2 = v.skill_learn_level
					add_info.cantuse = true
					add_info.is_base = false
					add_info.show_type = show_type
					local skillDescCfg = Cfg.SkillDescPartnerAndRole(add_info.skill_id, add_info.skill_level)
					add_info.skillDesc = skillDescCfg.description
					table.insert(data,add_info)
				end
			end
		end
	end
end

function MainViewBattleRB2:SortSkill(data, is_player_skill, show_type)
    local skillDescCfg
    local skill_data
    for k, v in pairs(data) do
		v.index2 = 99
        skill_data= RoleCtrl.Instance.skill_data:getSkillDataById(v.skill_id)
		if skill_data and skill_data.index2 then
			v.index2 = skill_data.index2
		end
		if is_player_skill then
			skillDescCfg = Cfg.SkillDescPartnerAndRole(v.skill_id, v.skill_level, false)
			if skillDescCfg then
				v.skillDesc = skillDescCfg.description
            else
                skillDescCfg = Cfg.SkillDescHeart(v.skill_id, v.skill_level)
                if skillDescCfg then
                    v.skillDesc = skillDescCfg.description
                else
                    v.skillDesc = ""
                end
			end
            if v.skill_id == FlyUpData.Instance:GetSkillId() then
                v.index2 = 100
                v.is_flyup_skill = true
            end
		end
		v.show_type = show_type
        if v.is_heart_skill == true then
            if (v.remain_times and v.remain_times <= 0) or (v.is_vaild and v.is_vaild == 0) then
                v.cantuse = true
            end
            v.show_index = k
        end
    end
    table.sort(data, function (a, b)
        if a.index2 ~= b.index2 then
            return a.index2 < b.index2
        else
            return a.skill_level > b.skill_level
        end
    end)
end

function MainViewBattleRB2:OpListCallback()
    self.ov2:Set(498, self.OpListCRect.sizeDelta.y)
    UH.SizeDelta2(self.OpListRect, self.ov2)
end
function MainViewBattleRB2:SkillListCallback()
    self.sv2:Set(498, self.SkillListCRect.sizeDelta.y)
    UH.SizeDelta2(self.SkillListRect, self.sv2)
end
function MainViewBattleRB2:OnClickSkill()
    TimeHelper:AddDelayFrameTimer(function ()
        self:SkillListCallback()
    end, 4)
end
function MainViewBattleRB2:OnClickOther()
    TimeHelper:AddDelayFrameTimer(function ()
        self:OpListCallback()
    end, 4)
end

function MainViewBattleRB2:OnClickBtnAuto(is_auto)
    if GuideManager.Instance:IsGuide() and not GuideManager.Instance:GetCurStepCLickSame("BattleAutoBtn2") then
        return
    end
    if BattleData.IsStory() then return end
    self:PlayAutoEffect(is_auto)
    BattleCtrl.Instance:SendAutoBattle(is_auto)
    --演算阶段进行修改
    if self.base_data.round_phase == BATTLE_ROUND_PHASE.Client then
        PublicPopupCtrl.Instance:Center(Language.Main.Battle.AutoTip[is_auto and 1 or 2])
    end
end
function MainViewBattleRB2:PlayAutoEffect(is_auto)
    if not is_auto then
        if self.auto_eff_handle ~= nil then
            self.EffectTool:Stop(self.auto_eff_handle)
            self.auto_eff_handle = nil
        end
    else
        if self.auto_eff_handle == nil then
            self.auto_eff_handle = self.EffectTool:Play(10010215)
        end
    end
end

function MainViewBattleRB2:OnAutoChange()
    local auto = self.base_data.auto
    self.Auto.isOn = auto
    -- LogDG("OnAutoChange", auto)
    self.Anim:SetBool(APH("is_auto"), auto)
    self:OnAutoBehaviourChange(1)
    self:OnAutoBehaviourChange(2)
    self.battle_data:SetSkillSelData(false, self.base_data.command_i)
    if auto == true then
        self:PlayAutoEffect(auto)
    else
        self:FlushSkillList()
    end
    UH.SetText(self.AutoDesc, Language.Main.Battle.AutoDesc[auto and 1 or 2])
end

function MainViewBattleRB2:OnCmdIChange()
    if self.base_data.command_i == 2 then
        self.Anim:SetTrigger(APH("next_cmd"))
    end
end
function MainViewBattleRB2:OnSelTargetStateChange()
    if GuideManager.Instance:IsGuide() then return end
    local sel_tar_ty = self.base_data.sel_tar_type
    -- LogDG("OnSelTargetStateChange", sel_tar_ty)
    self.Anim:SetBool(APH("sel_tar"), sel_tar_ty ~= BATTLE_MOVE_TYPE.NONE) 
    --[[ local view_data = BattleData.Instance:ViewInfo()
    if RoleSettingData.IsNewMode() and view_data.skill_sel_type ~= 1 then
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
            elseif sel_tar_ty == BATTLE_MOVE_TYPE.SEAL_PET then
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
    end ]]
end
function MainViewBattleRB2:OnRoundPhaseChange()
    local stage = 1
    if self.base_data.round_phase ~= BATTLE_ROUND_PHASE.Command then
        stage = 1
    else
        stage = 0
    end
    if self.base_data.battle_mode == BATTLE_MODE.BATTLE_MODE_STORY then
        stage = 0 
    end
    -- LogDG("OnRoundPhaseChange", stage)
    self.Anim:SetInteger(APH("stage"), stage)
end

function MainViewBattleRB2:OnClickDefence()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    BattleCtrl.Instance:SetMove(self.base_data.command_i, BATTLE_MOVE_TYPE.NORMAL_DEFENCE)
end
function MainViewBattleRB2:OnClickAISkill()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    BattleCtrl.Instance:SetMove(self.base_data.command_i, BATTLE_MOVE_TYPE.AI_SKILL)
end

function MainViewBattleRB2:OnAutoBeh1Click()
    self.battle_data:SetSkillSelData(true, 1)
end

function MainViewBattleRB2:OnAutoBeh2Click()
    self.battle_data:SetSkillSelData(true, 2)
end

function MainViewBattleRB2:OnAutoBehaviourChange(i)
    if self.base_data.auto == false then
        return
    end
    local is_first = (i == 1)
    local beh = self.battle_data:AutoBehaviour(i)
    local icon = self.AutoIcons[i]
    local lv = self.AutoLevels[i]
    local name = self.AutoNames[i]
    local ai = self.AutoAIs[i]

    ai:SetActive(beh.type == BATTLE_MOVE_TYPE.AI_SKILL)
    local flag = self.battle_data:IsChAlive(self.battle_data:PlayerPetIdx())
    local pet_ch = self.battle_data:GetChInfo(self.battle_data:PlayerPetIdx())
    if pet_ch == nil or pet_ch.curhp > 0 then
    else
        flag = true
    end
    self.Beh2Role:SetActive(not flag)
    self.Beh2Pet:SetActive(flag)
    self.AutoLevels[2]:SetObjActive(not flag)
    
    if beh.type == BATTLE_MOVE_TYPE.PERFORM_SKILL or beh.type == BATTLE_MOVE_TYPE.AI_SKILL
    and beh.skill_id > 0 and beh.skill_lv > 0 then
        beh.skill_lv = (beh.skill_lv ~= nil and beh.skill_lv <= 0) and 1 or beh.skill_lv
        local cfg = SkillData.GetConfig(beh.skill_id, beh.skill_lv)
        UH.SetText(name, cfg ~= nil and cfg.skill_name or nil)
        UH.SetText(lv, string.format(Language.Main.Battle.Ji, beh.skill_lv))
        UH.SetSkillIconToImage(icon, beh.skill_id, beh.skill_lv)
        local eff_parent = self.AutoEffects[i]
        local dec_config = SkillData.GetDecConfig(beh.skill_id, beh.skill_lv)
        if VersionTag.HasTag(VersionTag.ClipAnima2D) and dec_config and type(dec_config.asset_id) == "number" then
            if self.auto_skill_effect[i] == nil then
                self.auto_skill_effect[i] = self.EffectTool:PlayByEffectId(dec_config.asset_id, eff_parent)
                self.auto_skill_asset[i] = dec_config.asset_id
            elseif self.auto_skill_asset[i] ~= dec_config.asset_id then
                self:ClearSkillEffect(i)
                self.auto_skill_effect[i] = self.EffectTool:PlayByEffectId(dec_config.asset_id, eff_parent)
                self.auto_skill_asset[i] = dec_config.asset_id
            end
        else
            self:ClearSkillEffect(i)
        end
        if dec_config and dec_config.corner_icon ~= "" then
            for idx = 1, 2 do
                self["SkillCorner"..i][idx]:SetActive(dec_config.corner_icon ~= "")
            end
            UH.SetText(self.CornerNames[i], dec_config.corner_icon)
        else
            for idx = 1, 2 do
                self["SkillCorner"..i][idx]:SetActive(false)
            end
        end
    else
        self:ClearSkillEffect(i)
        for idx = 1, 2 do
            self["SkillCorner"..i][idx]:SetActive(false)
        end
        local cmd_info = self.base_cmd_skill_info[beh.type]
        if cmd_info ~= nil then
            UH.SetText(name, cmd_info.name)
            UH.SetText(lv, "")
            UH.SetIconToImage(icon, cmd_info.sp_name, ICON_TYPE.SKILL)
        end
    end
end
function MainViewBattleRB2:ClearSkillEffect(i)
    if self.auto_skill_effect[i] then
        self.EffectTool:Stop(self.auto_skill_effect[i])
        self.auto_skill_effect[i] = nil
    end
end

function MainViewBattleRB2:OnClickStrategy()
    self.battle_data:ViewInfo().select_strategy = false
    if self.StrategySel.activeSelf then
        self.StrategyPanel:SetActive(false)
        self.StrategySel:SetActive(not self.StrategySel.activeSelf)
        BattleCtrl.Instance:ExitSelectTargetState()
    else
        self.StrategyPanel:SetActive(true)
        self.StrategySel:SetActive(not self.StrategySel.activeSelf)
    end
end
function MainViewBattleRB2:OnClickExit()
    BattleCtrl.Instance:ExitSelectTargetState()
end
function MainViewBattleRB2:OnClickBack()
    BattleCtrl.Instance:ExitSelectTargetState()
end

MainBattleOpItem = MainBattleOpItem or DeclareMono("MainBattleOpItem", UIWidgetBaseItem)
function MainBattleOpItem:MainBattleOpItem()
    self.battle_data = BattleData.Instance 
    self.base_data = BattleData.Instance:BaseData()
    self.arrow_length = Vector2.one
    self.arrow_rotate = Vector3.one
    self.arrow_dirpos = Vector3.one
end

function MainBattleOpItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.Icon, data.sprite_name)
    UH.SetText(self.Name, data.name)
    self:uncareData()
    self:careData(self.base_data, BindTool.Bind(self.SelTargetStateChange, self), "sel_tar_type")
end
function MainBattleOpItem:OnDestroy()
    self:uncareData()
    self:ClearCloneObj()
end
function MainBattleOpItem:ClearCloneObj()
    if self.clone_obj then
        UnityGameObj.Destroy(self.clone_obj)
        self.clone_obj = nil
    end
    self.clone_item = nil
end
function MainBattleOpItem:SelTargetStateChange()
    if self.base_data.sel_tar_type == BATTLE_MOVE_TYPE.NONE then
        self:ClearCloneObj()
    end
end
function MainBattleOpItem:OnClickItem()
    -- LogDG(self.data.name, self.data.move_type, self.base_data.command_i)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    local player_cmd = self.battle_data:CurCmdIdx() == self.battle_data:PlayerIdx()
        local last_cmd_type = self.battle_data:BaseData().last_cmd_type
        if player_cmd then
            if last_cmd_type == BATTLE_MOVE_TYPE.USE_FABAO or last_cmd_type == BATTLE_MOVE_TYPE.USE_ITEM or last_cmd_type == BATTLE_MOVE_TYPE.PERFORM_SKILL or
                last_cmd_type == BATTLE_MOVE_TYPE.SEAL_PET or
                last_cmd_type == BATTLE_MOVE_TYPE.SEAL_YOUMING or
                last_cmd_type == BATTLE_MOVE_TYPE.CHANGE_POSITION or
                last_cmd_type == BATTLE_MOVE_TYPE.CHANGE_PET or 
                last_cmd_type == BATTLE_MOVE_TYPE.USE_FAKE_POTION or
                last_cmd_type == BATTLE_MOVE_TYPE.HEART_SKILL or
                last_cmd_type == BATTLE_MOVE_TYPE.CARD_SKILL or
                last_cmd_type == BATTLE_MOVE_TYPE.HUANSHOU_SKILL then
                    PublicPopupCtrl.Instance:Center(Language.Main.Battle.CantUseOp)
                    return
            end
        end

    if self.data.move_type == BATTLE_MOVE_TYPE.NORMAL_DEFENCE then
        BattleCtrl.Instance:SetMove(self.base_data.command_i, BATTLE_MOVE_TYPE.NORMAL_DEFENCE)
    elseif self.data.move_type == BATTLE_MOVE_TYPE.NORMAL_ATTACK then
        if self.data.is_drag == 1 then
            PublicPopupCtrl.Instance:Center(Language.Main.Battle.NeedDrag)
        else
            BattleCtrl.Instance:EnterSelectTargetStateByAtk()
        end
    elseif self.data.move_type == BATTLE_MOVE_TYPE.USE_FABAO then
        self.battle_data:ViewInfo().opened_sub = BattleSubView.Fabao
    elseif self.data.move_type == BATTLE_MOVE_TYPE.USE_ITEM then
        self.battle_data:ViewInfo().opened_sub = BattleSubView.Item
    elseif self.data.move_type == BATTLE_MOVE_TYPE.CHANGE_PET then
        self.battle_data:ViewInfo().opened_sub = BattleSubView.Pet
    elseif self.data.move_type == BATTLE_MOVE_TYPE.CHANGE_POSITION then
        BattleCtrl.Instance:SetMove(self.base_data.command_i, BATTLE_MOVE_TYPE.CHANGE_POSITION)
    elseif self.data.move_type == BATTLE_MOVE_TYPE.RUN_AWAY then
        BattleCtrl.Instance:SetMove(self.base_data.command_i, BATTLE_MOVE_TYPE.RUN_AWAY)
    elseif self.data.move_type == BATTLE_MOVE_TYPE.SEAL_PET or self.data.move_type == BATTLE_MOVE_TYPE.SEAL_YOUMING then
        if self:SealCondition(true) then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
            self.battle_data:ViewInfo().opened_sub = BattleSubView.Seal
        end
    elseif self.data.move_type == BATTLE_MOVE_TYPE.HEART_SKILL then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
        self.battle_data:SetSkillSelData(true, self.base_data.command_i, 2)
    elseif self.data.move_type == BATTLE_MOVE_TYPE.PERFORM_SKILL
    and self.data.sprite_name == "JueZhao" then
        --且是心法
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
        self.battle_data:SetSkillSelData(true, self.base_data.command_i, 3)
    end
end
function MainBattleOpItem:SealCondition(pop)
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
    if self.data.move_type == BATTLE_MOVE_TYPE.SEAL_PET then
        if PetData.Instance.pet_list:Length() >= Config.pet_cfg_auto.other[1].max_pet_bag then
            if pop then
                PublicPopupCtrl.Instance:Center(ErrorText[54])
            end
            return false
        end
    end
    return true
end
function MainBattleOpItem:OnClickPressDown()
    if self.data.move_type == BATTLE_MOVE_TYPE.NORMAL_ATTACK
    and self.data.is_drag == 1 then
        self.clone_obj = self.gameObject:Clone(self.CloneParent)
        -- self.clone_obj:ResetTransform()
        self.clone_item = self.clone_obj:GetLuaComponent(self:GetTypeName())
        self.clone_item.Arrow:SetActive(true)
        self.arrow_pos = self.clone_item.ArrowRect.anchoredPosition
        BattleCtrl.Instance:EnterSelectTargetStateByAtk()
    end
end
function MainBattleOpItem:OnClickPressUp()
    if self.data.move_type == BATTLE_MOVE_TYPE.NORMAL_ATTACK then
        LogDG("MainBattleOpItem:OnClickPressUp()")
        BattleCtrl.Instance:ExitSelectTargetState(BattleSceneRoleState.PointerGridIdx)
        self:ClearCloneObj()
    end
end
function MainBattleOpItem:OnClickDrag()
    -- LogDG("OnClickDrag") 
    if self.clone_obj and self.clone_item then
        local mPos = UH.ScreenPointToViewPoint(self.clone_item.ItemRect)
        local dis = Vector2.Distance(mPos, self.arrow_pos)
        self.arrow_dirpos:Set(mPos.x - self.arrow_pos.x, mPos.y - self.arrow_pos.y, 0)
        local angle = self:GetTouchAngle(self.arrow_dirpos)
        self.arrow_length:Set(dis - 35, 32)
        self.clone_item.ArrowRect.sizeDelta = self.arrow_length
        self.arrow_rotate:Set(0, 0, -angle)
        self.clone_item.Arrow:SetLocalEulerAngles(self.arrow_rotate)
    end
end
function MainBattleOpItem:GetTouchAngle(dir_vector)
    local vector = dir_vector.normalized
    local x = vector.x
    local y = vector.y
    local num = math.atan2(x,y)         -- 弧度
    local angle = num * 180 / math.pi   -- 角度 = 弧度 * 360 / 2PI 
    angle = angle - 90--箭头图片的问题
    return angle
end


MainBattleSkillItem = MainBattleSkillItem or DeclareMono("MainBattleSkillItem", UIWidgetBaseItem)
function MainBattleSkillItem:MainBattleSkillItem()
    self.battle_data = BattleData.Instance
    self.base_data = BattleData.Instance:BaseData()
    self.arrow_length = Vector2.one
    self.arrow_rotate = Vector3.one
    self.arrow_dirpos = Vector3.one
    self.clone_pos = Vector3.one
end
function MainBattleSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.SkillCell:SetData(data)
    local config
    if data.skill_id > 0 then
        config = SkillData.GetConfig(data.skill_id, data.skill_level)
    end
    if StringIsEmpty(data.name) then
        UH.SetText(self.Name, config ~= nil and config.skill_name or tostring(data.skill_id))
    else
        UH.SetText(self.Name, data.name)
    end
    local desc_config = SkillData.Instance:GetSkillDescConfig(data.skill_id, data.skill_level)
    if desc_config then
        UH.SetText(self.Tip, desc_config.skillTip)
    else
        UH.SetText(self.Tip, data.skillTip or "")
    end
    self:uncareData()
    self:careData(self.base_data, BindTool.Bind(self.SelTargetStateChange, self), "sel_tar_type")
end
function MainBattleSkillItem:OnDestroy()
    self:uncareData()
    self:ClearCloneObj()
end
function MainBattleSkillItem:ClearCloneObj()
    if self.clone_obj then
        UnityGameObj.Destroy(self.clone_obj)
        self.clone_obj = nil
    end
    self.clone_item = nil
end
function MainBattleSkillItem:SelTargetStateChange()
    if self.base_data.sel_tar_type == BATTLE_MOVE_TYPE.NONE then
        self:ClearCloneObj()
    end
end

function MainBattleSkillItem:OnSelSkill(skill_info)
    if not StringIsEmpty(skill_info.tip_msg) then
        PublicPopupCtrl.Instance:Center(skill_info.tip_msg)
        return
    end
    if skill_info.skill_id < 0  then
        if skill_info.skill_id == -BATTLE_MOVE_TYPE.NORMAL_ATTACK then
            BattleCtrl.Instance:EnterSelectTargetStateByAtk()
        end
    else
        local cmd_i = self.battle_data:ViewInfo().skill_sel_cmdi
        local use_lv = skill_info.skill_level
        local skill_cfg = SkillData.GetConfig(skill_info.skill_id, use_lv)
        if skill_cfg.is_target_self == 2 then --not need sel target,target is self
            BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.PERFORM_SKILL, skill_info.skill_id, use_lv)
        elseif skill_info.is_heart_skill == true then
            BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.HEART_SKILL, skill_info.skill_id, use_lv)
        elseif skill_info.is_card_skill == true then
            BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.CARD_SKILL, skill_info.skill_id, use_lv)
        elseif skill_info.is_huanshou_skill == true then
            BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.HUANSHOU_SKILL, skill_info.skill_id, use_lv)
        else
            BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.PERFORM_SKILL, skill_info.skill_id, use_lv)
        end   
    end
end

function MainBattleSkillItem:OnClickItem()
    -- LogDG(self.data.name, self.data.move_type, self.base_data.command_i)
    if self.data.skill_id < 0  then
        if self.data.skill_id == -BATTLE_MOVE_TYPE.NORMAL_ATTACK then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
            PublicPopupCtrl.Instance:Center(Language.Main.Battle.NeedDrag)
        end
    else
        if self.SkillCell.MaskBg.activeSelf then
            -- LogDG("!111111111111", self.SkillCell.mask_msg)
            PublicPopupCtrl.Instance:Center(self.SkillCell.mask_msg)
            return
        end
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        PublicPopupCtrl.Instance:Center(Language.Main.Battle.NeedDrag)
    end
end
function MainBattleSkillItem:OnClickPressDown()
    if self.data.cantuse then
		return
	end
    --判断当前的位置如果大于等于-132 那么克隆物就下移一点
    -- LogDG(UH.GetRootPosition(self.gameObject))
    self.clone_obj = self.gameObject:Clone(self.CloneParent)
    if self.ItemRect.anchoredPosition.y >= -132 and self.Rb2Panel.skill_num > 8 then
        self.clone_pos = self.clone_obj:GetLocalPosition()
        self.clone_pos:Set(self.clone_pos.x, self.clone_pos.y - 150, 0)
        self.clone_obj:SetLocalPosition(self.clone_pos)
    end
    -- self.clone_obj:ResetTransform()
    self.clone_item = self.clone_obj:GetLuaComponent(self:GetTypeName())
    self.clone_item.Arrow:SetActive(true)
    self.arrow_pos = self.clone_item.ArrowRect.anchoredPosition
    self:OnSelSkill(self.data)
end
function MainBattleSkillItem:OnClickPressUp()
    if self.data.cantuse then
        self:ClearCloneObj()
		return
	end
    -- LogDG("MainBattleSkillItem:OnClickPressUp()")
    if BattleSceneRoleState.PointerGridIdx == -1 then
        PublicPopupCtrl.Instance:Center(Language.Main.Battle.CantUse)
        BattleSceneRoleState.PointerGridIdx = nil
        self:ClearCloneObj()
        BattleCtrl.Instance:ExitSelectTargetState()
        return
    end
    if self.data.skill_id < 0  then
        if self.data.skill_id == -BATTLE_MOVE_TYPE.NORMAL_ATTACK then
            BattleCtrl.Instance:ExitSelectTargetState(BattleSceneRoleState.PointerGridIdx)
        end
    else
        local use_lv = self.data.skill_level
        local skill_cfg = SkillData.GetConfig(self.data.skill_id, use_lv)
        if skill_cfg.is_target_self == 2 then --not need sel target,target is self
            BattleCtrl.Instance:ExitSelectTargetState(self.battle_data:PlayerIdx())
        else
            BattleCtrl.Instance:ExitSelectTargetState(BattleSceneRoleState.PointerGridIdx)
        end
    end
    BattleSceneRoleState.PointerGridIdx = nil
    self:ClearCloneObj()
end
function MainBattleSkillItem:OnClickDrag()
    -- LogDG("OnClickDrag") 
    if self.clone_obj and self.clone_item then
        local mPos = UH.ScreenPointToViewPoint(self.clone_item.DragRect)
        local dis = Vector2.Distance(mPos, self.arrow_pos)
        self.arrow_dirpos:Set(mPos.x - self.arrow_pos.x, mPos.y - self.arrow_pos.y, 0)
        local angle = self:GetTouchAngle(self.arrow_dirpos)
        angle = angle - 90
        self.arrow_length:Set(dis, 32)
        self.clone_item.ArrowRect.sizeDelta = self.arrow_length
        self.arrow_rotate:Set(0, 0, -angle)
        self.clone_item.Arrow:SetLocalEulerAngles(self.arrow_rotate)
    end
end
function MainBattleSkillItem:GetTouchAngle(dir_vector)
    local vector = dir_vector.normalized
    local x = vector.x
    local y = vector.y
    local num = math.atan2(x,y)         -- 弧度
    local angle = num * 180 / math.pi   -- 角度 = 弧度 * 360 / 2PI     
    return angle
end