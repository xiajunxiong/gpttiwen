BattleDetail = BattleDetail or DeclareView("BattleDetail", "battle/battle_detail", 0, ViewLayer.Normal + 1)
VIEW_DECLARE_MASK(BattleDetail, ViewMask.BlockClose)
function BattleDetail:BattleDetail()
    self.battle_data = BattleData.Instance
    self.handle_data = self.battle_data:BattleDetail():Care(BindTool.Bind(self.FlushData, self))
    Runner.Instance:AddRunObj(self, 8)
    self.is_move = false
    self.is_start = true
end
function BattleDetail:OnFlush()
    local data = self.battle_data:BattleDetail()
    self.ListNone:SetActive(table.nums(data) == 0)
    self.List:SetData(data)
end
function BattleDetail:Update()
    if self.is_start == true then
        return
    end
    local per = self.ScrollRect.verticalNormalizedPosition
    per = tonumber(string.format("%.3f", per))
    if per <= 0.1 then
        self.is_move = false
    else
        self.is_move = true
    end
end
function BattleDetail:OnDestroy()
    self.battle_data:BattleDetail():Uncare(self.handle_data)
    Runner.Instance:RemoveRunObj(self)
end
function BattleDetail:FlushData()
    local data = self.battle_data:BattleDetail()
    self.ListNone:SetActive(table.nums(data) == 0)
    if self.is_move == false or self.is_start == true then
        self.is_start = true
        self.List:SetData(data)
        self.List:JumpToDown()
        self.ToDown:SetActive(false)
        self.is_start = false
    else
        local per = self.ScrollRect.verticalNormalizedPosition
        local old_num = table.nums(self.List:GetDataIndex() or {})
        if per == 1 and old_num < 15 then
            self.List:SetData(data)
            self.ToDown:SetActive(false)
            return
        end
        local new_num = table.nums(data)
        if old_num < new_num then
            self.DownTex.text = string.format(ErrorText[121], new_num - old_num)
            self.ToDown:SetActive(true)
        end
    end
end
function BattleDetail:OnClickDown()
    local data = self.battle_data:BattleDetail()
    self.List:SetData(data)
    self.List:JumpToDown()
    self.ToDown:SetActive(false)
    self.is_move = false
end
function BattleDetail:CloseCallback()
    self.battle_data:BattleDetail():Uncare(self.handle_data)
    Runner.Instance:RemoveRunObj(self)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

BattleDetailItem = BattleDetailItem or DeclareMono("BattleDetailItem", UIWidgetBaseItem)
function BattleDetailItem:BattleDetailItem()
end
function BattleDetailItem:SetData(data)
    UH.SetText(self.Content, data)
end

--解析战斗信息的类
BattleDetailPlayer = BattleDetailPlayer or BaseClass()
function BattleDetailPlayer:__init(eve_type, grid_idx, eve_data)
    self.eve_desc_list = {}

    self.first_step = {eve_type = eve_type, grid_idx = grid_idx, data = eve_data}
    self.step_list = {}
    self.result_list = {}
    for k, v in pairs(BATTLE_DATA_EVENT) do
        self.result_list[v] = {}
    end
end
function BattleDetailPlayer:PushChild(eve_type, grid_idx, eve_data)
    local step = {eve_type = eve_type, grid_idx = grid_idx, data = eve_data}
    table.insert(self.step_list, step)
end
function BattleDetailPlayer:AllSet()
    local sponsor = self:ChInfo(self.first_step.grid_idx)
    sponsor = sponsor == nil and {name = "神秘人"} or {name = sponsor.name}
    local eve_type = self.first_step.eve_type
    --执行者有可能不是角色而是系统
    if not BattleData.IsEnemyIdx(self.first_step.grid_idx) then
        sponsor.name = string.format( "<color=#"..COLORSTR.Yellow16..">%s</color>", sponsor.name)
    end
    -- LogError("准备完成",self.step_list)
    if eve_type == BATTLE_DATA_EVENT.SELF_DESTRUCT then
        --[[ local config = self:SkillConfig(self.first_step.data.skill_id)
        local res = string.format(Language.BattleDetail[8], sponsor.name, config.skill_name)
        self.result_list[eve_type] = res ]]
    elseif eve_type == BATTLE_DATA_EVENT.CHANGE_POSITION then
        local isFront = BattleData.IsFrontIdx(self.first_step.grid_idx)
        local res = string.format(Language.BattleDetail[eve_type][isFront and 2 or 1], sponsor.name)
        table.insert(self.result_list[eve_type], res)
    elseif eve_type == BATTLE_DATA_EVENT.RETRIEVE_PET then
        local is_front = BattleData.IsFrontIdx(self.first_step.grid_idx)
        local retrieve_idx = self.first_step.data.caller_grid_idx--is_front and (self.first_step.grid_idx - 5) or (self.first_step.grid_idx + 5)
        local performer = self:ChInfo(retrieve_idx)
        local temp = {}
        temp.name = performer.name
        if not BattleData.IsEnemyIdx(retrieve_idx) then
            temp.name = string.format( "<color=#"..COLORSTR.Yellow16..">%s</color>", performer.name)
        end
        local res = string.format(Language.BattleDetail[eve_type], temp.name, sponsor.name)
        if retrieve_idx ~= self.first_step.grid_idx then            
            table.insert(self.result_list[eve_type], res)
        end
    elseif eve_type == BATTLE_DATA_EVENT.PERFORM_COMBO_SKILL then
        --[[ local config = self:SkillConfig(self.first_step.data.skill_id)
        local res = string.format(Language.BattleDetail[8], sponsor.name, config.skill_name)
        self.result_list[eve_type] = res ]]
    elseif eve_type == BATTLE_DATA_EVENT.SEAL_PET then --1成功0失败
        local performer = self:ChInfo(self.first_step.data.target_grid_idx)
        local temp = {}
        temp.name = performer.name
        if not BattleData.IsEnemyIdx(self.first_step.grid_idx) then
            temp.name = string.format( "<color=#"..COLORSTR.Yellow16..">%s</color>", performer.name)
        end
        local res =
            string.format(
            Language.BattleDetail[eve_type][self.first_step.data.is_succ],
            sponsor.name,
            Item.GetQuaName(self.first_step.data.seal_card_item_id),
            temp.name
        )
        table.insert(self.result_list[eve_type], res)
    elseif eve_type == BATTLE_DATA_EVENT.PERFORM_SKILL then
    --[[  local config = self:SkillConfig(self.first_step.data.skill_id)
        local res = string.format(Language.BattleDetail[eve_type],sponsor.name,config.skill_name)
        self.result_list[eve_type] = res ]]
    elseif eve_type == BATTLE_DATA_EVENT.SKILL_SPECIAL_EFFECT then
        --判断是否是心法特效
        local sp_id = self.first_step.data.sp_id
        local skill_cfg = Cfg.SkillPassiveSpID(sp_id, false)
        if skill_cfg then
            skill_cfg = Cfg.HeartSkillByID(skill_cfg.skill_id, false)
            if skill_cfg then
                local res = string.format("%s释放了心法【%s】", sponsor.name, skill_cfg.heart_skill_name)
                table.insert(self.result_list[eve_type], res)
            end
        end
    elseif eve_type == BATTLE_DATA_EVENT.MODIFY_HP then
        if self.first_step.data.modify_num < 0 then
            local res = string.format("%s血量减少了<color=#%s>%s</color>", sponsor.name, COLORSTR.Red10, -self.first_step.data.modify_num)
            table.insert(self.result_list[eve_type], res)
        end
    end
    for key, value in ipairs(self.step_list) do
        if value.eve_type == BATTLE_DATA_EVENT.MODIFY_HP then
            --当是掉血的时候 判断是什么原因掉血
            local performer = self:ChInfo(value.grid_idx)
            local data_performer = self:DataChInfo(value.grid_idx)
            data_performer = data_performer == nil and {name = "神秘人"} or {name = data_performer.name}
            performer = performer == nil and {name = "神秘人"} or {name = performer.name}
            if performer.name ~= data_performer.name then
                performer.name = data_performer.name
            end
            if not BattleData.IsEnemyIdx(value.grid_idx) then
                performer.name = string.format( "<color=#"..COLORSTR.Yellow16..">%s</color>", performer.name)
            end
            if eve_type == BATTLE_DATA_EVENT.PERFORM_SKILL then
                local config = self:SkillConfig(self.first_step.data.skill_id)
                if value.data.modify_desc == BATTLE_HP_DESC.ADD_HP or value.data.modify_desc == BATTLE_HP_DESC.HP_CRITICAL then --加血
                    local res =
                        string.format(
                        Language.BattleDetail[value.eve_type][3],
                        sponsor.name,
                        config.skill_name,
                        performer.name,
                        value.data.modify_num
                    )
                    table.insert(self.result_list[value.eve_type], res)
                elseif value.data.modify_desc == BATTLE_HP_DESC.DAMAGE_ABSORB_HP then
                    local res =
                        string.format(
                        Language.BattleDetail[value.eve_type][6],
                        sponsor.name,
                        config.skill_name,
                        value.data.modify_num
                    )
                    table.insert(self.result_list[value.eve_type], res)
                else
                    if sponsor.name ~= performer.name then
                        --负的就是被克制，正就是你克制别人
                        local flag = tonumber(value.data.attr_injure_num) < 0
                        if value.data.attr_injure_num == 0 then
                            local res =
                                string.format(
                                Language.BattleDetail[value.eve_type][2],
                                sponsor.name,
                                config.skill_name,
                                performer.name,
                                -value.data.modify_num,
                                flag and COLORSTR.Red10 or COLORSTR.Green10,
                                value.data.attr_injure_num
                            )
                             --flag and value.data.attr_injure_num or -value.data.attr_injure_num)
                            table.insert(self.result_list[value.eve_type], res)
                        else
                            local res =
                                string.format(
                                Language.BattleDetail[value.eve_type][1],
                                sponsor.name,
                                config.skill_name,
                                performer.name,
                                -value.data.modify_num,
                                flag and COLORSTR.Red10 or COLORSTR.Green10,
                                value.data.attr_injure_num
                            )
                            table.insert(self.result_list[value.eve_type], res)
                        end
                    else
                        local last_step = self.step_list[key - 1]
                        if last_step and last_step.eve_type == BATTLE_DATA_EVENT.SKILL_HIT then
                            config = self:SkillConfig(last_step.data.skill_id)
                            sponsor = self:ChInfo(last_step.grid_idx)
                            sponsor = sponsor == nil and {name = "神秘人"} or {name = sponsor.name}
                            if not BattleData.IsEnemyIdx(last_step.grid_idx) then
                                sponsor.name = string.format( "<color=#"..COLORSTR.Yellow16..">%s</color>", sponsor.name)
                            end
                            local flag = value.data.attr_injure_num < 0
                            if value.data.attr_injure_num == 0 then
                                local res =
                                    string.format(
                                    Language.BattleDetail[value.eve_type][2],
                                    sponsor.name,
                                    config.skill_name,
                                    performer.name,
                                    -value.data.modify_num,
                                    flag and COLORSTR.Red10 or COLORSTR.Green10,
                                    flag and value.data.attr_injure_num or -value.data.attr_injure_num
                                )
                                table.insert(self.result_list[value.eve_type], res)
                            else
                                local res =
                                    string.format(
                                    Language.BattleDetail[value.eve_type][1],
                                    sponsor.name,
                                    config.skill_name,
                                    performer.name,
                                    -value.data.modify_num,
                                    flag and COLORSTR.Red10 or COLORSTR.Green10,
                                    value.data.attr_injure_num
                                )
                                table.insert(self.result_list[value.eve_type], res)
                            end
                        end
                    end
                end
            elseif eve_type == BATTLE_DATA_EVENT.PERFORM_COMBO_SKILL then
                local config = self:SkillConfig(self.first_step.data.skill_id)
                local flag = value.data.attr_injure_num < 0
                if value.data.modify_desc == BATTLE_HP_DESC.ADD_HP or value.data.modify_desc == BATTLE_HP_DESC.HP_CRITICAL then --加血
                    local res =
                        string.format(
                        Language.BattleDetail[value.eve_type][3],
                        sponsor.name,
                        config.skill_name,
                        performer.name,
                        value.data.modify_num
                    )
                    table.insert(self.result_list[value.eve_type], res)
                elseif value.data.modify_desc == BATTLE_HP_DESC.DAMAGE_ABSORB_HP then--吸血
                    local res =
                        string.format(
                        Language.BattleDetail[value.eve_type][7],
                        sponsor.name,
                        -- config.skill_name,
                        -- performer.name,
                        value.data.modify_num
                    )
                    table.insert(self.result_list[value.eve_type], res)
                else
                    if value.data.attr_injure_num == 0 then
                        local res =
                            string.format(
                            Language.BattleDetail[value.eve_type][2],
                            sponsor.name,
                            config.skill_name,
                            performer.name,
                            -value.data.modify_num,
                            flag and COLORSTR.Red10 or COLORSTR.Green10,
                            flag and value.data.attr_injure_num or -value.data.attr_injure_num
                        )
                        table.insert(self.result_list[value.eve_type], res)
                    else
                        local res =
                            string.format(
                            Language.BattleDetail[value.eve_type][1],
                            sponsor.name,
                            config.skill_name,
                            performer.name,
                            -value.data.modify_num,
                            flag and COLORSTR.Red10 or COLORSTR.Green10,
                            value.data.attr_injure_num
                        )
                        table.insert(self.result_list[value.eve_type], res)
                    end
                end
            elseif eve_type == BATTLE_DATA_EVENT.SELF_DESTRUCT then --自爆
                -- local config = self:SkillConfig(self.first_step.data.skill_id)
                local res =
                    string.format(
                    Language.BattleDetail[value.eve_type][2],
                    sponsor.name,
                    "自爆",
                    performer.name,
                    -value.data.modify_num
                )
                table.insert(self.result_list[value.eve_type], res)
            elseif eve_type == BATTLE_DATA_EVENT.USE_ITEM then --使用物品造成血量变化
            -- LogError(sponsor.name, "对", performer.name, "使用物品", self.first_step.data.item_id, "恢复了", value.data.modify_num, "点血量")
            end
        elseif value.eve_type == BATTLE_DATA_EVENT.MODIFY_MP then
            if eve_type == BATTLE_DATA_EVENT.PERFORM_SKILL then
                local config = self:SkillConfig(self.first_step.data.skill_id)
                local performer = self:ChInfo(value.grid_idx)
                performer = performer == nil and {name = "神秘人"} or {name = performer.name}
                if not BattleData.IsEnemyIdx(value.grid_idx) then
                    performer.name = string.format( "<color=#"..COLORSTR.Yellow16..">%s</color>", performer.name)
                end
                local res =
                    string.format(
                    Language.BattleDetail[value.eve_type][value.data.modify_desc ~= 7 and 1 or 2],
                    sponsor.name,
                    config.skill_name,
                    performer.name,
                    value.data.modify_desc ~= 7 and -value.data.modify_num or value.data.modify_num
                )
                table.insert(self.result_list[value.eve_type], res)
            end
        elseif value.eve_type == BATTLE_DATA_EVENT.DODGE then
            if self.first_step.eve_type == BATTLE_DATA_EVENT.USE_ITEM then
            else
                local config = self:SkillConfig(self.first_step.data.skill_id)
                local performer = self:ChInfo(value.grid_idx)
                performer = performer == nil and {name = "神秘人"} or {name = performer.name}
                if not BattleData.IsEnemyIdx(value.grid_idx) then
                    performer.name = string.format( "<color=#"..COLORSTR.Yellow16..">%s</color>", performer.name)
                end
                local res =
                    string.format(
                    Language.BattleDetail[value.eve_type],
                    performer.name,
                    sponsor.name,
                    config.skill_name
                )
                table.insert(self.result_list[value.eve_type], res)
            end
        elseif value.eve_type == BATTLE_DATA_EVENT.SUMMON_BATTLE_CHARACTER then
            if eve_type == BATTLE_DATA_EVENT.SUMMON then
                local res =
                    string.format(Language.BattleDetail[value.eve_type], sponsor.name, value.data.character_info.name)
                table.insert(self.result_list[value.eve_type], res)
            end
        elseif value.eve_type == BATTLE_DATA_EVENT.ADD_BUFF_TO_LIST then
            if eve_type == BATTLE_DATA_EVENT.PERFORM_SKILL then
                local config = self:SkillConfig(self.first_step.data.skill_id)
                if config then
                    --这里如果要就得分 自己对自己使用 防御 对其他人使用
                    local res = string.format(Language.BattleDetail[value.eve_type], sponsor.name, config.skill_name)
                    table.insert(self.result_list[value.eve_type], res)
                end
            end
        elseif value.eve_type == BATTLE_DATA_EVENT.CHANGE_EQUIPMENT then
            local res = string.format(Language.BattleDetail[value.eve_type], sponsor.name)
            table.insert(self.result_list[value.eve_type], res)
        elseif value.eve_type == BATTLE_DATA_EVENT.SKILL_HIT then
        --技能命中 --具体命中了谁
        --[[ local config = self:SkillConfig(value.data.skill_id)
            local res = string.format( Language.BattleDetail[value.eve_type],sponsor.name,config.skill_name)
            table.insert(self.result_list[value.eve_type], res ) ]]
        end
    end
end
function BattleDetailPlayer:SkillConfig(skill_id)
    return SkillData.GetConfig(skill_id)
end
function BattleDetailPlayer:ChInfo(grid_idx)
    return BattleData.Instance:GetChInfo(grid_idx)
end
function BattleDetailPlayer:DataChInfo(grid_idx)
    return BattleData.Instance:GetDataChInfo(grid_idx)
end
function BattleDetailPlayer:PushDesc(eve_type, desc)
    self.eve_desc_list[eve_type] = self.eve_desc_list[eve_type] or {}
    table.insert(self.eve_desc_list[eve_type], desc)
end
-- 将解析完的文本进行显示
function BattleDetailPlayer:Play(eve_type)
    --将文字提交给界面显示
    if self.eve_desc_list[eve_type] then
        local str = table.remove(self.eve_desc_list[eve_type])
        if nil ~= str then
            BattleData.Instance:InsertDetail(
                string.format(Language.BattleDetail.Show, tostring(os.date("%H:%M:%S ", os.time())), str)
            )
        end
    end
end
