----------------MainViewBattleSkillSel 技能选择框(自动，手动)------------

MainViewBattleSkillSel = MainViewBattleSkillSel or DeclareMono("MainViewBattleSkillSel", UIWFlushPanel)
---SkillList====UIWidgetList
---SkillIconContent===BattleSkillContentItem
---SkillListContent===BattleSkillContentItem
---OtherSkillContent===BattleSkillContentItem
---SkillIconContentTran===RectTransform
---SkillListContentTran===RectTransform

local base_cmd_skill_info = {
    {skill_id = -BATTLE_MOVE_TYPE.NORMAL_ATTACK, sp_name = "1", name = Language.Main.Battle.Attack, is_base = true, skillDesc = "普通攻击"},
    {skill_id = -BATTLE_MOVE_TYPE.NORMAL_DEFENCE, sp_name = "31", name = Language.Main.Battle.Defence, is_base = true, skillDesc = "防御"},
    -- {skill_id = -BATTLE_MOVE_TYPE.AI_SKILL, sp_name = "81", name = Language.Main.Battle.AISkill, is_base = true, skillDesc = "智能施法"}
    -- {skill_id = -BATTLE_MOVE_TYPE.CHANGE_POSITION, sp_name = "41", name = Language.Main.Battle.Position},
    -- {skill_id = -BATTLE_MOVE_TYPE.RUN_AWAY, sp_name = "51", name = Language.Main.Battle.Escape}
}

local ai_skill_cmd_info = {skill_id = -BATTLE_MOVE_TYPE.AI_SKILL, sp_name = "81", name = Language.Main.Battle.AISkill, is_base = true, skillDesc = "智能施法"}


function MainViewBattleSkillSel:MainViewBattleSkillSel()
    self.battle_data = BattleData.Instance
    self.data_cares = {
        {data = self.battle_data:ViewInfo(), func = self.FlushSkillList, keys = {"skill_sel_cmdi"}},
        {data = self.battle_data:ViewInfo(), func = self.FlushSkillList, keys = {"skill_sel_type"}},
        {data = self.battle_data:BaseData(), func = self.FlushSkillList, keys = {"auto"}},
        {data = self.battle_data:ChInfo(), func = self.FlushSkillList, keys = {3, 8}},
        {data = LiveData.Instance.card_use_all_info, func = self.FlushSkillList},
    }
    GuideManager.Instance:RegisterGetGuideUi(
        "FirstSkill",
        function()
            if MainViewBattleSkillSel.show_skill_list == true then
                self:OnClickChangeButton()
            end
            if self.SkillIconContent.grid_par:SetupCompleted() then
                local skill_id = GuideData.Instance:GetSkillId()
                if skill_id == nil then
                    GuideManager.Instance:ForceStop()
                    return
                end
                -- for key, value in pairs(self.ManualList:GetData()) do
                    for index, value2 in pairs(self.SkillIconContent:GetData().list) do
                        if value2.skill_id == skill_id then
                            -- if self.ManualList.item_list and self.ManualList.item_list[key].grid_par:SetupCompleted() then
                                return self.SkillIconContent.grid_par.item_list[index].ClickEvent
                            -- end
                        end
                    end
                -- end
                --如果没返回强行停止
                --GuideManager.Instance:ForceStop()
            end
            return nil, nil
        end
    )
    GuideManager.Instance:RegisterGetGuideUi(
        "SecondSkill",
        function()
            if MainViewBattleSkillSel.show_skill_list == true then
                self:OnClickChangeButton()
            end
            if self.SkillIconContent.grid_par:SetupCompleted() then
                local skill_id = GuideData.Instance:GetSkillId2()
                if skill_id == nil then
                    GuideManager.Instance:ForceStop()
                    return
                end
                -- for key, value in pairs(self.ManualList:GetData()) do
                    for index, value2 in pairs(self.SkillIconContent:GetData().list) do
                        if value2.skill_id == skill_id then
                            -- if self.ManualList.item_list and self.ManualList.item_list[key].grid_par:SetupCompleted() then
                                return self.SkillIconContent.grid_par.item_list[index].ClickEvent
                            -- end
                        end
                    end
                -- end
                --如果没返回强行停止
                --GuideManager.Instance:ForceStop()
            end
            return nil, nil
        end
    )
    GuideManager.Instance:RegisterGetGuideUi(
        "FirstPetSkill",
        function()
            if MainViewBattleSkillSel.show_skill_list == true then
                self:OnClickChangeButton()
            end
            if self.SkillIconContent.grid_par:SetupCompleted() then
                -- if self.ManualList.item_list and self.ManualList.item_list[1].grid_par:SetupCompleted() then
                    return self.SkillIconContent.grid_par.item_list[1].ClickEvent
                -- end
                --如果没返回强行停止
                --GuideManager.Instance:ForceStop()
            end
            return nil, nil
        end
    )
    GuideManager.Instance:RegisterGetGuideUi(
        "AISKillBtn",
        function()
            if MainViewBattleSkillSel.show_skill_list == true then
                self:OnClickChangeButton()
            end
            -- if self.ManualList:SetupCompleted() then
            --     for key, value in pairs(self.ManualList:GetData()) do
            --         for index, value2 in pairs(value.list) do
            --             if value2.skill_id == -BATTLE_MOVE_TYPE.AI_SKILL then
            --                 self.ManualList:AutoSlide(value, false)
            --                 if self.ManualList.item_list and self.ManualList.item_list[key].grid_par:SetupCompleted() then
            --                     return self.ManualList.item_list[key].grid_par.item_list[index].ClickEvent
            --                 end
            --             end
            --         end
            --     end
            -- end

            if self.OtherSkillContent.grid_par:SetupCompleted() then
                local other_skill_list = self.OtherSkillContent:GetData().list
                for index, value2 in pairs(other_skill_list) do
                    if value2.skill_id == -BATTLE_MOVE_TYPE.AI_SKILL then
                        return self.OtherSkillContent.grid_par.item_list[index].ClickEvent
                    end
                end
            end
            return nil, nil
        end
    )
end
MainViewBattleSkillSel.NoHeartSkillHeight = 514     --没有OtherContent时的技能List高度
MainViewBattleSkillSel.HasHeartSkillHeight = 354.5  --有OtherContent时的技能List高度

function MainViewBattleSkillSel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.SelectObj:SetActive(false)
    TimeHelper:CancelTimer(self.handle_long)
    self.handle_long = nil
    self.is_long_click = false
end

function MainViewBattleSkillSel:OnClickChangeButton()
	--if MainViewBattleSkillSel.is_player_skill then
		MainViewBattleSkillSel.show_skill_list = not MainViewBattleSkillSel.show_skill_list
	--end
	self:FlushSkillList()
end

function MainViewBattleSkillSel:FlushSkillList(old)
    local base_data = self.battle_data:BaseData()
    local view_data = self.battle_data:ViewInfo()
    local is_auto = base_data.auto
    local skill_info = nil
    local is_player_skill = true
    local show_type = 0
    local skill_sel_type = view_data.skill_sel_type or 1

    if view_data.skill_sel_cmdi == 2 then --二动时
        local pet_ch = self.battle_data:GetChInfo(self.battle_data:PlayerPetIdx())
        if pet_ch ~= nil then
            is_player_skill = false
        end
    end
    if MainViewBattleSkillSel.show_skill_list then
		show_type = 1
	end
    MainViewBattleSkillSel.is_player_skill = is_player_skill

    if show_type == 1 then
		UH.SpriteName(self.ChangeShow,"skill_select_list")
	else
		UH.SpriteName(self.ChangeShow,"skill_select_normal")
	end

    local is_youming = false
    -- self.ManualScrollRect.enabled = is_player_skill
    if is_player_skill then
		skill_info = self.battle_data:PlayerSkillInfo()
    else
        -- UH.AnchoredPosition(self.SkillListConRect, Vector2.zero)
        local pet_list = self.battle_data:PetList()
        for _, v in pairs(pet_list) do
            if v.status == BATTLE_PET_STATUS.Zhan then
                skill_info = v.skill_list
                break
            elseif v.status == BATTLE_PET_STATUS.Wang and v.youming_info then
                --还要判断当前位置是否是幽冥
                local ch_info = self.battle_data:GetChInfo(self.battle_data:PlayerPetIdx())
                if ch_info and ch_info.character_type == BATTLE_CH_TYPE.YOU_MING then
                    skill_info = {v.youming_info.skill_info}
                    is_youming = true
                    break
                end
            end
        end
    end

	local data = {}
    --当前是人且是手动
    -- if is_player_skill then

        if skill_info ~= nil then
            data = DataHelper.TableCopy(skill_info)
        end

        --提前把飞升技能拿出来
        local idx = nil
        for index, value in ipairs(data) do
            if value.skill_id == FlyUpData.Instance:GetSkillId() then
                idx = index
            end
        end
        local flyup_skill = nil
        if idx ~= nil then
            flyup_skill = table.remove(data, idx)
        end
        if is_player_skill and (skill_sel_type == 1 or is_auto) then
            self:SortSkill(data, is_player_skill, show_type)            
        end
        if skill_sel_type == 1 or is_auto then
            self:AddUnlock(data, is_player_skill, show_type)
        end
        --这里还得判断是否有心法技能
        -- local index = 1
        -- local list = {}
        local skill_data = {name = Language.Fight.SkillTitle[is_player_skill == true and 1 or 3], list = data}--, format = is_player_skill and RoleData.Instance:GetProfession() or -1}
        if is_youming then
            skill_data.name = Language.Fight.SkillTitle[10]
        end
        self.NoneObj:SetActive(false)
        if skill_sel_type == 2 then
            data = self:HeartSkillList(show_type, is_player_skill)
            if #data > 0 then
                skill_data = {name = Language.Fight.SkillTitle[5], list = data}
            else
                skill_data = {name = Language.Fight.SkillTitle[5], list = {}}
                UH.SetText(self.NoneTex, Language.Main.Battle.XinFaNone)
                self.NoneObj:SetActive(true)
            end
        elseif skill_sel_type == 3 then
            if flyup_skill then                
                skill_data = {name = Language.Fight.SkillTitle[8], list = {flyup_skill}}
            else
                skill_data = {name = Language.Fight.SkillTitle[8], list = {}}
                UH.SetText(self.NoneTex, Language.Main.Battle.JueZhaoNone)
                self.NoneObj:SetActive(true)
            end
        end

         -- 检查是否有使用幻兽技能
         local huanshou_skill_info = HuanShouData.Instance:GetBattleSkillInfo(BattleData.Instance.huanshou_skill_info)
         if huanshou_skill_info and is_player_skill then
            huanshou_skill_info.show_type = MainViewBattleSkillSel.show_skill_list and 1 or 0
             if not other_skill_data then
                 other_skill_data = {name = Language.Fight.SkillTitle[4], list = {}}
             end
             table.insert(skill_data.list, 1, huanshou_skill_info)
         end

        --LogDG("skill_sel_type1", skill_sel_type, is_player_skill)
        local skill_con_tran = nil
        self.SkillIconContent:SetObjActive(show_type == 0)
        self.SkillListContent:SetObjActive(show_type ~= 0)
        if show_type == 0 then
            self.SkillIconContent:SetData(skill_data)
            skill_con_tran = self.SkillIconContentTran
        else
            self.SkillListContent:SetData(skill_data)
            skill_con_tran = self.SkillListContentTran
        end
        
        local other_skill_data = nil
        
        -- index = index + 1

        if is_auto then
            local base = self:BaseSkillList(show_type,is_player_skill)
            other_skill_data = {name = Language.Fight.SkillTitle[2], list = base}            
        else
            if is_player_skill then
                local heart_sklill = self.battle_data:PlayerHeartSkill()
                heart_sklill = DataHelper.TableCopy(heart_sklill)
                local count = self.battle_data:PlayerHeartSkillCount()
                if count > 0 then
                    if flyup_skill then
                        table.insert(heart_sklill, flyup_skill)
                    end
                    -- data = heart_sklill
                    self:SortSkill(heart_sklill, is_player_skill, show_type)
                    -- list[index] = {name = Language.Fight.SkillTitle[4], list = data}
                    -- index = index + 1
                    other_skill_data = {name = Language.Fight.SkillTitle[4], list = heart_sklill}
                else
                    local heart_sklill = {}
                    if flyup_skill then
                        flyup_skill.is_flyup_skill = true
                        table.insert(heart_sklill, flyup_skill)
                        self:SortSkill(heart_sklill, is_player_skill, show_type)
                        other_skill_data = {name = Language.Fight.SkillTitle[4], list = heart_sklill}
                    end
                end

                -- -- 检查是否有使用幻化卡技能
                -- local card_skill_info = LiveData.Instance:GetCardSkillInfo(BattleData.Instance.live_card_seq,true)
                -- if card_skill_info then
                --     if not other_skill_data then
                --         other_skill_data = {name = Language.Fight.SkillTitle[4], list = {}}
                --     end
                --     table.insert(other_skill_data.list, card_skill_info)
                -- end
            end
        end
        --LogError("other_skill_data",other_skill_data)
        if not is_auto and skill_sel_type == 2 or skill_sel_type == 3 then
            other_skill_data = nil
        end
        --12  42 -70 30  50 50
        if other_skill_data ~= nil then
            self.OtherSkillContent:SetObjActive(true)
            self.OtherSkillContent:SetData(other_skill_data)
            if is_auto then
                self.OtherGridLayout.spacing = Vector2.New(70, 30)
                self.OtherGridLayout.padding.left = 12
            else
                if is_player_skill then
                    self.OtherGridLayout.spacing = Vector2.New(20, 30)
                    self.OtherGridLayout.padding.left = 12
                end
            end
            skill_con_tran.sizeDelta = Vector2.New(skill_con_tran.sizeDelta.x, MainViewBattleSkillSel.HasHeartSkillHeight)
        else
            self.OtherSkillContent:SetObjActive(false)
            skill_con_tran.sizeDelta = Vector2.New(skill_con_tran.sizeDelta.x, MainViewBattleSkillSel.NoHeartSkillHeight)
        end
        -- if is_player_skill and index == 2 then
        --     list[1].had_not_heart = true
        -- end
        -- if is_auto then
        --     local base = self:BaseSkillList(show_type,is_player_skill)
        --     list[index] = {name = Language.Fight.SkillTitle[2], list = base}
        -- end

        -- if show_type == 0 then
        --     self.ListManualList:SetData({})
        --     self.ManualList:SetData(list)
        -- else
        --     self.ManualList:SetData({})
        --     self.ListManualList:SetData(list)
        -- end


    -- end
	

   --[[  local pos = self.ViewObj:GetLocalPosition()
    local skill_click = BattleData.Instance.skill_click
    pos.x = not skill_click and 0 or skill_click == 1 and 235 or 335

    self.ViewObj:SetLocalPosition(pos) ]]

    
    --[[ if skill_info ~= nil then
        for _, v in pairs(skill_info) do
            table.insert(data, v)
            -- if view_data.skill_sel_tab == 2 then --物理
            --     local skill_cfg = SkillData.GetConfig(v.skill_id, v.skill_level)
            --     if --skill_cfg.target_side_type ~= 1 and
            --         skill_cfg.is_magic == 0 then
            --         table.insert(data, v)
            --     end
            -- elseif view_data.skill_sel_tab == 3 then --魔法
            --     local skill_cfg = SkillData.GetConfig(v.skill_id, v.skill_level)
            --     if --skill_cfg.target_side_type ~= 1 and
            --         skill_cfg.is_magic == 1 then
            --         table.insert(data, v)
            --     end
            -- elseif view_data.skill_sel_tab == 4 then --辅助[deprecated]
            --     local skill_cfg = SkillData.GetConfig(v.skill_id, v.skill_level)
            --     if skill_cfg.target_side_type == 1 then
            --         table.insert(data, v)
            --     end
            -- else
            --     table.insert(data, v)
            -- end
        end
    end ]]

    -- if is_auto and view_data.skill_sel_tab == 1 then ---添加攻击，防御，逃跑，换位
    --     for _, v in pairs(base_cmd_skill_info) do
    --         table.insert(data, v)
    --     end
    -- end
		
    --[[ for k, v in pairs(data) do
		v.index2 = 99
		local skill_data = RoleCtrl.Instance.skill_data:getSkillDataById(v.skill_id)
		if skill_data and skill_data.index2 then
			v.index2 = skill_data.index2
		end		
		if is_player_skill then
			local skillDescCfg = Cfg.SkillDescPartnerAndRole(v.skill_id, v.skill_level)
			if skillDescCfg then
				v.skillDesc = skillDescCfg.description
			end
		end
		v.show_type = show_type
    end ]]
    -- table.sort(data, DataSorter.KeyLowerSorter("index2"))
    --[[ table.sort(data, function (a, b)
        if a.index2 ~= b.index2 then
            return a.index2 < b.index2
        else
            return a.skill_level > b.skill_level
        end
    end ) ]]
	

	
	
	--[[ if show_type == 0 then
		self.ShowNormal:SetActive(true)
		self.ShowList:SetActive(false)
		if is_auto then
			local auto_list = {}
			auto_list[1] = {name = Language.Fight.SkillTitle[1], list = data}
			if view_data.skill_sel_tab == 1 then
				auto_list[2] = {name = Language.Fight.SkillTitle[2], list = {}}
				for _, v in pairs(base_cmd_skill_info) do
                    if not is_player_skill and v.skill_id == -BATTLE_MOVE_TYPE.AI_SKILL then
                    else
                        table.insert(auto_list[2].list, v)
                    end
				end
				for k, v in pairs(auto_list[2].list) do
					v.index2 = 99
					local skill_data = RoleCtrl.Instance.skill_data:getSkillDataById(v.skill_id)
					if skill_data and skill_data.index2 then
						v.index2 = skill_data.index2
					end
					v.show_type = show_type
				end
			end
			self.AutoList:SetData(auto_list)
			self.AutoObj:SetActive(true)
			self.ManualObj:SetActive(false)
		else
			self.SkillList:SetData(data)
			self.AutoObj:SetActive(false)
			self.ManualObj:SetActive(true)
		end
	else
		self.ShowNormal:SetActive(false)
		self.ShowList:SetActive(true)
		if is_auto then
			self.ListAutoList:SetData(data)			
			local list = {}
			for _, v in pairs(base_cmd_skill_info) do
                if not is_player_skill and v.skill_id == -BATTLE_MOVE_TYPE.AI_SKILL then
                else
                    table.insert(list, v)
                end
			end
			for k, v in pairs(list) do
				v.index2 = 99
				local skill_data = RoleCtrl.Instance.skill_data:getSkillDataById(v.skill_id)
				if skill_data and skill_data.index2 then
					v.index2 = skill_data.index2
				end
				v.show_type = show_type
			end	
			self.ListAutoJiChuList:SetData(list)
			self.ListAutoObj:SetActive(true)
			self.ListManualObj:SetActive(false)
			self.ListAutoShow:SetActive(true)		
		else
			self.ListAutoObj:SetActive(false)
			self.ListManualObj:SetActive(true)
			self.ListManualList:SetData(data)
			self.ListAutoShow:SetActive(false)
		end
	end ]]
end
function MainViewBattleSkillSel:HeartSkillList(show_type,is_player_skill)
    local data = {}
    if is_player_skill then
        local heart_sklill = self.battle_data:PlayerHeartSkill()
        data = DataHelper.TableCopy(heart_sklill)
    end
    return data
end
function MainViewBattleSkillSel:BaseSkillList(show_type,is_player_skill)
    local data = {}
    local skill_data
    for _, v in pairs(base_cmd_skill_info) do
        table.insert(data, v)
    end
    if is_player_skill then --只有主角有智能施法按钮
        table.insert(data,ai_skill_cmd_info)
    end

    for k, v in pairs(data) do
        v.index2 = 99
        skill_data = RoleCtrl.Instance.skill_data:getSkillDataById(v.skill_id)
        if skill_data and skill_data.index2 then
            v.index2 = skill_data.index2
        end
        v.show_type = show_type
    end
    return data
end
function MainViewBattleSkillSel:AddUnlock(data, is_player_skill, show_type)
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
					if  SkillData.RoleViewShowPassiveSkill[v.skill_id] then
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
    -- return data
end
function MainViewBattleSkillSel:SortSkill(data, is_player_skill, show_type)
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
    -- return data
end
function MainViewBattleSkillSel:OnTabSel(index)
    self.battle_data:ViewInfo().skill_sel_tab = index
end
function MainViewBattleSkillSel:OnSelDown(info)
	if info.cantuse then
		return
	end
    if MainViewBattleSkillSel.is_player_skill then
        self.is_long_click = false
        local time = Time.realtimeSinceStartup
        self.handle_long =
            TimeHelper:AddRunTimer(
            function()
                if Time.realtimeSinceStartup - time >= 1 then
                    TimeHelper:CancelTimer(self.handle_long)
                    self.handle_long = nil
                    self.is_long_click = true
                    self.SelectObj:SetActive(true)
                    local data = SkillData.GetConfigUntil(info.skill_id, info.skill_level)
                    for index, value in ipairs(data) do
                        if value.skill_id == FlyUpData.Instance:GetSkillId() then
                            value.is_flyup_skill = true
                        end
                    end
                    self.SkillList2:SetData(data)
                end
            end,
            0.2
        )
    end
end
function MainViewBattleSkillSel:OnSelUp()
    TimeHelper:CancelTimer(self.handle_long)
    self.handle_long = nil
end

--长按技能选择框里图标回调不刷列表
function MainViewBattleSkillSel:OnSelDown2(info)
	if info.cantuse then
		return
	end
    if MainViewBattleSkillSel.is_player_skill then
        self.is_long_click = false
        local time = Time.realtimeSinceStartup
        self.handle_long =
            TimeHelper:AddRunTimer(
            function()
                if Time.realtimeSinceStartup - time >= 1 then
                    TimeHelper:CancelTimer(self.handle_long)
                    self.handle_long = nil
                    self.is_long_click = true
                    self.SelectObj:SetActive(true)
                end
            end,
            0.2
        )
    end
end

function MainViewBattleSkillSel:OnSelUp2()
    TimeHelper:CancelTimer(self.handle_long)
    self.handle_long = nil
end

function MainViewBattleSkillSel:OnSelSkill(skill_info)
	if skill_info.cantuse then
		return
	end
    if self.handle_long or self.is_long_click then
        return
    end
    if not StringIsEmpty(skill_info.tip_msg) then
        PublicPopupCtrl.Instance:Center(skill_info.tip_msg)
    end

    if not StringIsEmpty(skill_info.mask_msg) then
        PublicPopupCtrl.Instance:Center(skill_info.mask_msg)
        return
    end
    
    self.SelectObj:SetActive(false)
    local base_data = self.battle_data:BaseData()
    local cmd_i = self.battle_data:ViewInfo().skill_sel_cmdi
    local use_lv = skill_info.skill_level
    
    -- local ch = self.battle_data:GetChInfo(self.battle_data:PlayerIdx())
    -- if skill_cfg and (base_data.last_cmd_type ~= BATTLE_MOVE_TYPE.USE_ITEM or not MainViewBattleItemSel.equip) and skill_cfg.weapon_type ~= 0 and ch.weapon_type ~= skill_cfg.weapon_type then
    --     PublicPopupCtrl.Instance:Center(string.format(Language.Main.Battle.NeedWeapon, Language.Main.Battle.Weapon[skill_cfg.weapon_type]))
    -- else
    if base_data.auto then
        if skill_info.skill_id < 0 then
            BattleCtrl.Instance:SetAutoMove(cmd_i, -skill_info.skill_id)
        else
            local pet_ch = self.battle_data:GetChInfo(self.battle_data:PlayerPetIdx())
            local has_pet = pet_ch ~= nil
            if not has_pet then
                --一回合只能释放一个技能
                local other_cmdi = cmd_i == 2 and 1 or 2
                local other_beh = self.battle_data:AutoBehaviour(other_cmdi)
                if other_beh.type == BATTLE_MOVE_TYPE.PERFORM_SKILL then
                    BattleCtrl.Instance:SetAutoMoveLocal(other_cmdi, BATTLE_MOVE_TYPE.NORMAL_ATTACK)
                end
            end
            BattleCtrl.Instance:SetAutoMove(cmd_i, BATTLE_MOVE_TYPE.PERFORM_SKILL, skill_info.skill_id, use_lv)
        end
    else
        local skill_cfg = SkillData.GetConfig(skill_info.skill_id, use_lv)
        if skill_cfg.is_target_self == 2 then --not need sel target,target is self
            BattleCtrl.Instance:SetMove(cmd_i, BATTLE_MOVE_TYPE.PERFORM_SKILL, self.battle_data:CurCmdIdx() - 1, skill_info.skill_id, use_lv)
        elseif skill_info.is_heart_skill == true then
            BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.HEART_SKILL, skill_info.skill_id, use_lv)
        elseif skill_info.is_card_skill == true then
            -- 变身卡技能
            BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.CARD_SKILL, skill_info.skill_id, use_lv)
        elseif skill_info.is_huanshou_skill == true then
            BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.HUANSHOU_SKILL, skill_info.skill_id, use_lv)
        else
            BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.PERFORM_SKILL, skill_info.skill_id, use_lv)
        end
    end
    self.battle_data:SetSkillSelData(false)
    TimeHelper:CancelTimer(self.handle_long)
    self.handle_long = nil
    self.is_long_click = false
end

function MainViewBattleSkillSel:OnClickClose()
    self.battle_data:SetSkillSelData(false)
    TimeHelper:CancelTimer(self.handle_long)
    self.handle_long = nil
    self.is_long_click = false
end

---grid_par=====UIWidgetList
BattleSkillContentItem = BattleSkillContentItem or DeclareMono("BattleSkillContentItem", UIWidgetBaseItem)
function BattleSkillContentItem:BattleSkillContentItem()
    -- self.normal_v2 = Vector2.New(424.5468, 246.4625)
    -- self.unnormal_v2 = Vector2.New(424.5469, 390)
    -- self.normal_v21 = Vector2.New(424.5468, 293)
    -- self.unnormal_v21 = Vector2.New(424.5469, 436.9554)
end
function BattleSkillContentItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.title_text.text = data.name
    self.grid_par.ContentParent:SetActive(#data.list > 0)
    self.grid_par:SetData(data.list)

    -- if data.had_not_heart == true then
    --     if self.Rect then
    --         self.Rect.sizeDelta = self.unnormal_v2
    --         self.RootRect.sizeDelta = self.unnormal_v21
    --     end
    -- else
    --     if self.Rect then
    --         self.Rect.sizeDelta = self.normal_v2
    --         self.RootRect.sizeDelta = self.normal_v21
    --     end
    -- end

    --[[ if self.LayoutGroup and data.format then
        self.LayoutGroup.spacing = data.format == RoleProfessionType.QiGongShi and self.unnormal_v2 or self.normal_v2
        self.LayoutGroup.padding.top = data.format == RoleProfessionType.QiGongShi and 19 or 1
    end ]]
end

-- UnityEngine.Debug.LogError("UIWidgetBaseItem===" .. tostring(UIWidgetBaseItem))
BattleSkillSelItem = BattleSkillSelItem or DeclareMono("BattleSkillSelItem", UIWidgetBaseItem)
-------Name=======Text
-------Icon=======UIImageSpriteSet
-------Lv=========Text
-------ZhiDing======GameObject

---data===={skill_id,skill_level,cooldown_num}
-- function BattleSkillSelItem:OnDisable()
--     if self.effect then
--         self.EffectTool:Stop(self.effect)
--         self.effect = nil
--     end
-- end

function BattleSkillSelItem:OnDestroy()
    if self.effect then
        self.EffectTool:Stop(self.effect)
        self.effect = nil
    end
end

function BattleSkillSelItem:SetData(data)
    --LOG("技能data", data)
    UIWidgetBaseItem.SetData(self, data)
    --[[ if data.index2 and data.index2 < 99 and data.index2 > 0 then
        self.ZhiDing:SetActive(true)
    else
        self.ZhiDing:SetActive(false)
    end ]]
    self.FlyUpSkillFrame:SetActive(false)--data.is_flyup_skill == true)
	if  data.is_flyup_skill == true then
        UH.SetText(self.FlyUpSkillLevel, string.format("Lv.%s", data.skill_level))
    end
    if self.TipObj then
        self.TipObj:SetActive(data.tip_text ~= nil)
        if self.TipText and data.tip_text then
            UH.SetText(self.TipText, data.tip_text)
        end
    end
    if self.TypeObj then
        self.TypeObj:SetActive(false)
        self.TypeObj:SetActive(data.show_index == 1 or data.is_flyup_skill == true or data.is_card_skill or data.is_huanshou_skill)
        local type_text = data.type_text or (data.is_flyup_skill and ColorStr(Language.Fight.SkillTitle[6], COLORSTR.Blue10) or ColorStr(Language.Fight.SkillTitle[5], COLORSTR.Yellow15))
        UH.SetText(self.TypeTex, type_text)
        local type_bg = data.type_bg or (data.is_flyup_skill and "JiNengJiaoBiao2" or "JiNengJiaoBiao1")
        if self.TypeBg then
            UH.SpriteName(self.TypeBg, type_bg)
        end
    end
    local config
    if data.skill_id > 0 then
        config = SkillData.GetConfig(data.skill_id, data.skill_level)
    end
    if StringIsEmpty(data.name) then
        self.Name.text = config ~= nil and config.skill_name or tostring(data.skill_id)
    else
        self.Name.text = data.name
    end
    if config then
        if self.LongClick == true then
            if not StringIsEmpty(config.consume_sp) and config.consume_sp > 0 then
                UH.SetText(self.MpTex, string.format( Language.Partner.SkillReplace.consume_sp, config.consume_sp))
            else
                UH.SetText(self.MpTex, string.format( Language.Partner.SkillReplace.consume_mp, config.consume_mp))
            end
        end
    end

    if self.effect then
        self.EffectTool:Stop(self.effect)
        self.effect = nil
    end

    local dec_config = SkillData.GetDecConfig(data.skill_id, data.skill_level)
    if VersionTag.HasTag(VersionTag.ClipAnima2D) and dec_config and type(dec_config.asset_id) == "number" then
        self.effect = self.EffectTool:PlayByEffectId(dec_config.asset_id, self.EffectNode)
    end
    if dec_config and not StringIsEmpty(dec_config.corner_icon) then
        if MainViewBattleSkillSel.is_player_skill then
            for idx = 1, 2 do
                if self.SkillCorner3 then
                    self.SkillCorner3[idx]:SetActive(dec_config.corner_icon ~= "")
                end
                self.SkillCorner[idx]:SetActive(false)
            end
            if self.CornerName3 then
                self.CornerName3.text = dec_config.corner_icon
            end
        else
            for idx = 1, 2 do
                self.SkillCorner[idx]:SetActive(dec_config.corner_icon ~= "")
                if self.SkillCorner3 then
                    self.SkillCorner3[idx]:SetActive(false)
                end
            end
            self.CornerName.text = dec_config.corner_icon
        end
    else
        for idx = 1, 2 do
            self.SkillCorner[idx]:SetActive(false)
            if self.SkillCorner3 then
                self.SkillCorner3[idx]:SetActive(false)
            end
        end
    end

    if MainViewBattleSkillSel.is_player_skill then
        local lv_str = nil
        if data.skill_level ~= nil and data.skill_level > 0 then
            lv_str = string.format(Language.Main.Battle.Ji, data.skill_level)
        end
        UH.SetText(self.Lv, lv_str)
    else
        UH.SetText(self.Lv, "")
    end
    if StringIsEmpty(data.sp_name) then
        -- self.Icon.SpriteName = nil
        if IS_AUDIT_VERSION then
            local audit_skill_index = SkillData.GetAuditSkillIndex(data.skill_id)
            if audit_skill_index then
                local img_path, stream_has = ChannelAgentConsts.GetUIStremingAsset("RoleSkillIcon"..audit_skill_index)
                if stream_has then
                    self.IconRawObj:SetActive(true)
                    self.IconObj:SetActive(false)
                    ChannelAgentConsts.SetRawImg(self.IconRaw, img_path)
                else
                    self.IconRawObj:SetActive(false)
                    self.IconObj:SetActive(true)
                    UH.SetSkillIconToImage(self.Icon, data.skill_id, data.skill_level)
                    UH.SetSkillIconToImage(self.Mask, data.skill_id, data.skill_level)
                end
            else
                self.IconRawObj:SetActive(false)
                self.IconObj:SetActive(true)
                UH.SetSkillIconToImage(self.Icon, data.skill_id, data.skill_level)
                UH.SetSkillIconToImage(self.Mask, data.skill_id, data.skill_level)
            end
        else
            UH.SetSkillIconToImage(self.Icon, data.skill_id, data.skill_level)
            UH.SetSkillIconToImage(self.Mask, data.skill_id, data.skill_level)
        end
    else
        -- self.Icon.SpriteName = data.sp_name
        self.IconRawObj:SetActive(false)
        self.IconObj:SetActive(true)
        UH.SetIconToImage(self.Icon, data.sp_name, ICON_TYPE.SKILL)
        UH.SetIconToImage(self.Mask, data.sp_name, ICON_TYPE.SKILL)
    end
    if self.RageList then
        self.RageList:SetObjActive(false)
    end
    local flag = false
    local gray = true
    if not data.is_base and config and not data.cantuse then
        self.battle_data = BattleData.Instance
        local ch = self.battle_data:GetChInfo(MainViewBattleSkillSel.is_player_skill and self.battle_data:PlayerIdx() or self.battle_data:PlayerPetIdx())
        if ch then
            local consume_mp
            if config.pet_consume_mp == 0 then
                consume_mp = config.consume_mp
            else
                local pet_ch = BattleData.Instance:GetChInfo(BattleData.Instance:PlayerPetIdx())
                if pet_ch ~= nil then
                    consume_mp = (config.pet_consume_mp / 100) * pet_ch.level
                else
                    consume_mp = 0
                    -- BuglyReport("Error pet is nil skill_name = ", config.skill_name, "id = ", data.skill_id)
                end
            end
            if consume_mp > ch.curmp then
                flag = true
                gray = false
                self.MaskText.text = Language.Main.Battle.NoEnoughMana
                -- self.mask_msg = Language.Main.Battle.NeedMana
            elseif not StringIsEmpty(config.consume_sp) and ch.spirit_point > -1 and config.consume_sp > ch.spirit_point then
                flag = true
                gray = false
                self.MaskText.text = Language.Main.Battle.NoEnoughSp
            elseif data.is_card_mask then
                flag = true
                gray = true
                self.MaskText.text = Language.Role.CardNotPointTip
                self.mask_msg = Language.Role.CardNotPointMsg
            elseif data.is_huanshou_mask then
                flag = true
                gray = true
                self.MaskText.text = data.mask_tip or Language.Role.HuanHuaCDTip1
                self.mask_msg = Language.Role.HuanHuaCDTip2
            end
            if config.consume_rage_point ~= 0 and config.consume_rage_point > (ch.rage or 0) then
                data.tip_msg = Language.Main.Battle.RageTip
                -- flag = true
                -- self.MaskText.text = Language.Main.Battle.NoRage
                -- self.mask_msg = Language.Main.Battle.RageLimit
            --怒气不足tip
            -- elseif not StringIsEmpty(config.consume_sp) and config.consume_sp > (ch.spirit_point or 0) then
            --     data.tip_msg = Language.Main.Battle.SpiritPointTip
            else
                data.tip_msg = nil
            end
            if self.RageList and config.consume_rage_point and config.consume_rage_point > 0 then
                self.RageList:SetObjActive(true)
                self.RageList:SetValue(config.consume_rage_point)
            end
            if self.RageObjs and config.consume_rage_point and config.consume_rage_point > 0 then
                self.RageObjs[1]:SetActive(true)
                self.RageObjs[2]:SetValue(config.consume_rage_point)
                UH.AnchoredPosition(self.RageObjs[3], Vector2.New(104.44,-3))
            elseif self.RageObjs and config.consume_rage_point and config.consume_rage_point <= 0 then
                self.RageObjs[1]:SetActive(false)
                UH.AnchoredPosition(self.RageObjs[3], Vector2.New(104.44,-21))
            end
            --新增血蓝百分比消耗
            if not StringIsEmpty(config.use_mp_hp) and config.use_mp_hp > 0 then
                local consume_mp = ch.maxmp * config.use_mp_hp / 100
                local consume_hp = ch.maxhp * config.use_mp_hp / 100
                if consume_mp > ch.curmp and consume_hp > ch.curhp then
                    flag = true
                    gray = false
                    self.MaskText.text = Language.Main.Battle.NoConditions
                elseif consume_mp > ch.curmp then
                    flag = true
                    gray = false
                    self.MaskText.text = Language.Main.Battle.NoEnoughMana
                elseif consume_hp > ch.curhp then
                    flag = true
                    gray = false
                    self.MaskText.text = Language.Main.Battle.NoEnoughHp
                end
            end
            -- local skill_cfg = SkillData.GetConfig(data.skill_id, data.skill_level)
            if (self.battle_data.last_cmd_type ~= BATTLE_MOVE_TYPE.USE_ITEM or not MainViewBattleItemSel.equip) and config.weapon_type ~= 0 and ch.weapon_type ~= config.weapon_type then
                flag = true
                self.MaskText.text = Language.Main.Battle.NoWeapon
                self.mask_msg = string.format(Language.Main.Battle.NeedWeapon, Language.Main.Battle.Weapon[config.weapon_type])
            end

            local displayer = BattleCtrl.Instance.displayer
            local player_ch = displayer:getCh(self.battle_data:CurCmdIdx())

            if player_ch and config and flag == false then
                local state_list = player_ch:BuffState()
                if config.is_magic == 0 and state_list[6] == true then
                    flag = true
                    self.MaskText.text = Language.Main.Battle.CantSelect
                    self.mask_msg = Language.Main.Battle.Timidity
                elseif config.is_magic == 1 and state_list[5] == true then
                    flag = true
                    self.MaskText.text = Language.Main.Battle.CantSelect
                    self.mask_msg = Language.Main.Battle.Silent
                --[[ elseif state_list[10] then
                    flag = true
                    self.MaskText.text = Language.Main.Battle.CantSelect
                    self.mask_msg = "在石化状态下无法行动，技能释放失败" ]]
               --[[  elseif state_list[9] then
                    flag = true
                    self.MaskText.text = Language.Main.Battle.CantSelect
                    self.mask_msg = "受到诅咒的影响，本次行动失败" ]]
                --[[ elseif state_list[8] then
                    flag = true
                    self.MaskText.text = Language.Main.Battle.CantSelect
                    self.mask_msg = "混乱状态下会强制普攻，技能释放失败" ]]
                end
            end
            --[[ if player_ch and player_ch.ui_buff_list and config then
                for k, v in pairs(player_ch.ui_buff_list) do
                    local buff_cfg = BattleData.BuffConfig(v)
                    if buff_cfg then
                        if config.is_magic == 0 and buff_cfg.ui_type == 8 then
                            flag = true
                            self.MaskText.text = Language.Main.Battle.CantSelect
                            self.mask_msg = Language.Main.Battle.Timidity
                            break
                        end
                        if config.is_magic == 1 and buff_cfg.ui_type == 7 then
                            flag = true
                            self.MaskText.text = Language.Main.Battle.CantSelect
                            self.mask_msg = Language.Main.Battle.Silent
                            break
                        end
                    end
                end
            end ]]

        end
    end
    self.Mask:SetObjActive(flag)
    self.Mask.enabled = gray
    if flag and gray then
        if dec_config and not StringIsEmpty(dec_config.corner_icon) then
            for idx = 1, 2 do
                self.SkillCorner2[idx]:SetActive(dec_config.corner_icon ~= "")
            end
            self.CornerName2.text = dec_config.corner_icon
        else
            for idx = 1, 2 do
                self.SkillCorner2[idx]:SetActive(false)
            end
        end
    else
        for idx = 1, 2 do
            self.SkillCorner2[idx]:SetActive(false)
        end
    end
    self.ClickEvent.enabled = (not flag or not gray)
    self.ClickUpEvent.enabled = (not flag or not gray)
    self.ClickDownEvent.enabled = (not flag or not gray)
    if self.ShowName then
        self.ShowName:SetActive(true)
    end
    
    local show_type = MainViewBattleSkillSel.show_skill_list and 1 or 0
	if (data.show_type == 1 or show_type == 1) and self.RagePoint then
		self.RagePoint:SetActive(false)
		if data.is_base then
            local name = string.format("<color=#F7CF6F>%s:</color>",self.Name.text)
			self.ShowName:SetActive(false)
            self.SkillDesc.text = name..data.skillDesc
		else
			local name = string.format("<color=#F7CF6F>%s:</color>",self.Name.text)
			local skill_desc = ""
			if  MainViewBattleSkillSel.is_player_skill then
                skill_desc = data.skillDesc or ""
            elseif data.is_huanshou_skill then
                skill_desc = data.skillDesc or ""
			else
				skill_desc = dec_config.description
			end
            self.ShowName:SetActive(false)
			self.SkillDesc.text = name..skill_desc
		end
	end
	
	if self.InteractorHandler then
		if data.cantuse then
			self.Lv.text = ""
			local level = RoleData.Instance:GetRoleLevel()
			if data.skill_learn_level and level < data.skill_learn_level then
				self.MaskBg:SetActive(true)
                UH.SetText(self.MaskBgText, string.format(Language.Role.RoleLevelCanLearn, data.skill_learn_level))
                self.mask_msg = Language.Role.RoleSkillCantUse2
            elseif data.remain_times and data.remain_times <= 0 then
                self.MaskBg:SetActive(true)
                UH.SetText(self.MaskBgText, Language.Role.RemainTimeYet)
                self.mask_msg = Language.Role.RemainTimeNoUse
                self.ClickEvent.enabled = false
                self.ClickUpEvent.enabled = false
                self.ClickDownEvent.enabled = false
            elseif data.is_vaild and data.is_vaild == 0 then
                self.MaskBg:SetActive(true)
                UH.SetText(self.MaskBgText, Language.Role.RemainTimeYet)
                self.mask_msg = Language.Role.RemainTimeNoUse2
                self.ClickEvent.enabled = false
                self.ClickUpEvent.enabled = false
                self.ClickDownEvent.enabled = false
            else
                -- self.MaskBg:SetActive(false)
				self.MaskBg:SetActive(true)
                UH.SetText(self.MaskBgText, Language.Role.RoleLevelLearnYet)
                self.mask_msg = Language.Role.RoleSkillGoToLearn
                self.ClickEvent.enabled = false
                self.ClickUpEvent.enabled = false
                self.ClickDownEvent.enabled = false
			end
			self.InteractorHandler.Interactable = false
        elseif data.remain_times and data.remain_times <= 0 and data.remain_times ~= -1 then
            self.Mask:SetObjActive(false)
            self.MaskBg:SetActive(true)
            UH.SetText(self.MaskBgText, Language.Role.RoleSkillCantUse)
            self.mask_msg = Language.Role.RoleSkillRemainTimesYet
            self.ClickEvent.enabled = false
            self.ClickUpEvent.enabled = false
            self.ClickDownEvent.enabled = false
            self.InteractorHandler.Interactable = false
		else
			self.MaskBg:SetActive(false)
			self.InteractorHandler.Interactable = true
		end
	end
end

function BattleSkillSelItem:OnClickMaskBg()
    PublicPopupCtrl.Instance:Center(self.mask_msg)
end

function BattleSkillSelItem:OnClickMask()
    PublicPopupCtrl.Instance:Center(self.mask_msg)
end
