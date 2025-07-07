local function concatTaskChainText(language, ...)
    return string.format(language[GameMath.Ramdon(1, #language)], ...)
end
local function concatTaskChainTextLT(language, limit_time, ...)
    return string.format(language[limit_time % (#language) + 1], ...)
end
function TaskData:UpdateTaskChainTalk(task, taskvo, info, pre_info)
    local total_num = info.cur_task_max_count
    local cur_num = info.task_pass_count + 1
    local accept_text
    local npc_seq = info.task_param[1]
    local npc_cfg = Config.npc_dynamic_auto.npc_dynamic_cfg[npc_seq]
    taskvo.task_name = Language.TaskChain.Name[1] .. string.format("(%d/%d)", cur_num, total_num)
    taskvo.npc_id = npc_seq
    if npc_cfg then
        local npc_co = Config.npc_dynamic_auto.npc_dynamic_cfg[npc_seq]
        local npc_name = npc_co.name
        taskvo.task_desc = string.format(Language.TaskChain.Goal.normal[1], npc_name)
        taskvo.task_goal = string.format(Language.TaskChain.Goal.normal[1], npc_name)
        taskvo.task_goal_menu = string.format(Language.TaskChain.Goal.menu[1], npc_name)
        if pre_info and pre_info.task_type == TASK_CHAIN_TASK_TYPE.FIGHT then
            local scene_co = SceneData.Instance:GetSceneCfg(npc_co.scene_id)
            accept_text =
                string.format(Language.TaskChain.Accept[TASK_CHAIN_TASK_TYPE.TALK].spec, scene_co.name, npc_name)
        else
            accept_text = concatTaskChainText(Language.TaskChain.Accept[TASK_CHAIN_TASK_TYPE.TALK], npc_name)
        end
        --仍得注册一个按钮
        self.task_chain_btn_handle =
            BehaviorData.Instance:CustomNpcTalkBtnsSeq(
            npc_seq,
            function(btn_list, obj_vo)
                table.insert(
                    btn_list,
                    NpcDialogBehBtnData.FuncCreate(
                        Language.TaskChain.Btn[1],
                        function()
                            -- TaskCtrl.Instance:CSTaskChainOp(TASK_CHAIN_OP_TYPE.TALK)
                            self:doTaskChainTalk(info)
                        end,
                        false
                    )
                )
                local can_skip, skip_btn = task:SkipBtn()
                if can_skip then
                    table.insert(btn_list, skip_btn)
                end
            end
        )
    else
        taskvo.task_desc = "缺少NPC配置 " .. npc_seq
        taskvo.task_goal = "缺少NPC配置 " .. npc_seq
    end

    --一般来说对话是不可跳过的，所以寻路到对应NPC后即可发送协议
    --如果不是new那就显示
    self:doTaskChainStart(info, pre_info, accept_text)
end
function TaskData:doTaskChainStart(info, pre_info, accept_text)
    if info.is_new == 1 then
        local cur_talker = info.task_param[1]
        local talk_list
        if info.pass_count == 0 or info.pass_count == 50 or info.pass_count == 100 then
            cur_talker = Config.task_chain_auto.other[1].start_npc
        elseif pre_info then
            cur_talker = pre_info.task_param[1]
            if pre_info.task_type ~= TASK_CHAIN_TASK_TYPE.FIGHT then
                cur_talker = BehaviorData.Instance:NpcSeqToNpcId(cur_talker)
            end
        end
        talk_list = {{talker = cur_talker, talk_text = accept_text}}
        if pre_info and info.task_type == TASK_CHAIN_TASK_TYPE.WAIT then
            cur_talker = BehaviorData.Instance:NpcSeqToNpcId(info.task_param[4])
            talk_list = {
                {
                    talker = cur_talker,
                    talk_text = concatTaskChainText(Language.TaskChain.Finish[pre_info.task_type])
                },
                {
                    talker = cur_talker,
                    talk_text = accept_text
                }
            }
        end
        NpcDialogCtrl.Instance:NpcDialogView(
            {
                talk_list = talk_list,
                beh_btn_list = {}
            },
            function()
                if info.task_type == TASK_CHAIN_TASK_TYPE.TALK then
                    self:doTaskChainTalk(info)
                elseif info.task_type == TASK_CHAIN_TASK_TYPE.ITEM then
                    self:doTaskChainItem(info)
                elseif info.task_type == TASK_CHAIN_TASK_TYPE.FIGHT then
                    self:doTaskChainFight(info)
                elseif info.task_type == TASK_CHAIN_TASK_TYPE.PET then
                    self:doTaskChainPet(info)
                --[[ elseif info.task_type == TASK_CHAIN_TASK_TYPE.WAIT then
                        self:doTaskChainWait(info) ]]
                end
            end
        )
    else
        if pre_info and pre_info.task_type == TASK_CHAIN_TASK_TYPE.WAIT and info.task_type == TASK_CHAIN_TASK_TYPE.WAIT then
            local item_id = info.task_param[2]
            local time_limit = info.task_param[3]
            if TimeHelper.GetServerTime() > time_limit and 0 ~= BagData.Instance:GetNumByItemId(item_id) then
                self:doTaskChainWait(info)
            end
        end
    end
end
function TaskData:doTaskChainTalk(info)
    local cur_talker = info.task_param[1]
    if info.task_type ~= TASK_CHAIN_TASK_TYPE.FIGHT then
        cur_talker = BehaviorData.Instance:NpcSeqToNpcId(cur_talker)
    end
    SceneCtrl.Instance:Logic():AutoToNpc(
        info.task_param[1],
        function()
            BehaviorData.Instance.auto_behavior = 2
            NpcDialogCtrl.Instance:NpcDialogView(
                {
                    talk_list = {
                        {
                            talker = cur_talker,
                            talk_text = concatTaskChainText(Language.TaskChain.Finish[info.task_type])
                        },
                        talker = cur_talker
                    },
                    beh_btn_list = {}
                },
                function()
                    BehaviorData.Instance.auto_behavior = 0
                    TaskCtrl.Instance:CSTaskChainOp(TASK_CHAIN_OP_TYPE.TALK)
                end
            )
        end,
        false,
        true
    )
end

function TaskData:UpdateTaskChainItem(task, taskvo, info, pre_info)
    local total_num = info.cur_task_max_count
    local cur_num = info.task_pass_count + 1
    local npc_seq = info.task_param[1]
    local item_id = info.task_param[2]
    local num = info.task_param[3]
    local accept_text
    local npc_cfg = Config.npc_dynamic_auto.npc_dynamic_cfg[npc_seq]
    taskvo.task_name = Language.TaskChain.Name[2] .. string.format("(%d/%d)", cur_num, total_num)
    taskvo.npc_id = npc_seq
    if npc_cfg then
        local npc_name = npc_cfg.name
        local drop_scene_name = SceneData.Instance:GetSceneCfg(info.task_param[4]).name
        local desc =
            string.format(Language.TaskChain.Goal.normal[2], npc_name, Item.GetName(item_id)) ..
            ((self.chain_task_reward_list[item_id] or {}).get_tip or "")
        local desc_menu =
            string.format(Language.TaskChain.Goal.menu[2], npc_name, Item.GetName(item_id)) ..
            ((self.chain_task_reward_list[item_id] or {}).get_tip or "")
        taskvo.task_desc = desc
        taskvo.task_goal = desc
        taskvo.task_goal_menu = desc_menu
        info.special = {
            desc = desc,
            desc_t = string.format(Language.TaskChain.Goal.normal.t2, drop_scene_name),
            desc_tf = Language.TaskChain.Goal.normal.t2f,
            desc_menu = desc_menu,
            desc_menu_t = string.format(Language.TaskChain.Goal.menu.t2, drop_scene_name),
            desc_menu_tf = Language.TaskChain.Goal.menu.t2f
        }

        if pre_info and pre_info.task_type == TASK_CHAIN_TASK_TYPE.FIGHT then
            accept_text =
                string.format(Language.TaskChain.Accept[TASK_CHAIN_TASK_TYPE.ITEM][1], npc_name, Item.GetName(item_id))
        else
            accept_text =
                concatTaskChainText(
                Language.TaskChain.Accept[TASK_CHAIN_TASK_TYPE.ITEM],
                npc_name,
                Item.GetName(item_id)
            )
        end

        self.task_chain_btn_handle =
            BehaviorData.Instance:CustomNpcTalkBtnsSeq(
            npc_seq,
            function(btn_list, obj_vo)
                table.insert(
                    btn_list,
                    NpcDialogBehBtnData.FuncCreate(
                        Language.TaskChain.Btn[2],
                        function()
                            self:doTaskChainItem(info)
                        end,
                        true
                    )
                )
                local can_skip, skip_btn = task:SkipBtn()
                if can_skip then
                    table.insert(btn_list, skip_btn)
                end
            end
        )
    else
        taskvo.task_desc = "缺少NPC配置 " .. npc_seq
        taskvo.task_goal = "缺少NPC配置 " .. npc_seq
    end
    self:doTaskChainStart(info, pre_info, accept_text)
end
function TaskData:doTaskChainItem(info)
    --有病啊！ 先要检查有没有，然后还要检查买没买，有了之后再寻路，再弹提交。。。。
    if self.bag_data_care_handle_item then
        BagData.Instance:UnCare(self.bag_data_care_handle_item)
        self.bag_data_care_handle_item = nil
    end
    local npc_seq = info.task_param[1]
    local item_id = info.task_param[2]
    if BagData.Instance:GetNumByItemId(item_id) ~= 0 then
        SceneCtrl.Instance:Logic():AutoToNpc(
            npc_seq,
            function()
                TaskChainItemSel.item_id = item_id
                TaskChainItemSel.pet_id = 0
                TaskChainItemSel.npc_seq = npc_seq
                TaskChainItemSel.from_type = TaskConfig.TaskChainItemSelFrom.task_chain
                BehaviorData.Instance.auto_behavior = 2
                ViewMgr:OpenView(TaskChainItemSel)
            end,
            false,
            true
        )
    else
        self.bag_data_care_handle_item =
            BagData.Instance:Care(
            function()
                if BagData.Instance:GetNumByItemId(item_id) ~= 0 then
                    self:doTaskChainItem(info)
                end
            end
        )
        local is_medicine = ItemType.Medicine == Item.GetType(item_id)
        self.task_chain_sub_continue = true
        if self.bag_data_care_handle_item then
            BagData.Instance:UnCare(self.bag_data_care_handle_item)
            self.bag_data_care_handle_item = nil
        end
        BehaviorData.Instance.auto_behavior = 2
        if is_medicine then
            -- WorkshopData.Instance:SetLifeItemQuickJump(item_id)
            -- ViewMgr:OpenViewByKey(Mod.Workshop.NeoCompose)
            CommerceData.Instance:SetJumpIndex(item_id) --原来是工坊界面合成，现在跳转到市场直接购买
            MallData.Instance:SetJumpItemNum(1)
            ViewMgr:OpenViewByKey(Mod.Mall.Shop)
            return
        end
        if Item.GetType(item_id) == 18 then
            TradeData.Instance:SetSearchItemId(item_id)
            ViewMgr:OpenViewByKey(Mod.Mall.Trade)
            return
        end
        TaskData.shop_auto_close = true
        MallCtrl.Instance:Jump(item_id)
    end
end
function TaskData:UpdateTaskChainFight(task, taskvo, info, pre_info)
    local total_num = info.cur_task_max_count
    local cur_num = info.task_pass_count + 1
    local accept_text
    local npc_id = info.task_param[1]
    for k, v in pairs(Config.npc_auto.npc_list) do
        if v.id == npc_id then
            taskvo.task_name = Language.TaskChain.Name[3] .. string.format("(%d/%d)", cur_num, total_num)
            taskvo.npc_id = nil
            local monster_name = v.name
            local scene_name = SceneData.Instance:GetSceneCfg(info.task_param[2]).name
            taskvo.task_desc = string.format(Language.TaskChain.Goal.normal[3], scene_name, monster_name)
            taskvo.task_goal = string.format(Language.TaskChain.Goal.normal[3], scene_name, monster_name)
            taskvo.task_goal_menu = string.format(Language.TaskChain.Goal.menu[3], scene_name, monster_name)

            accept_text =
                concatTaskChainText(Language.TaskChain.Accept[TASK_CHAIN_TASK_TYPE.FIGHT], monster_name, scene_name)
            break
        end
    end
    self:doTaskChainStart(info, pre_info, accept_text)
end
function TaskData:doTaskChainFight(info)
    local scene_id = info.task_param[2]
    -- local x = math.floor(info.task_param[3] / 10000)
    -- local y = info.task_param[3] % 10000
    SceneLogic.Instance:AutoToPos(
        scene_id,
        nil,
        function()
            Runner.Instance:RunUntilTrue(
                function()
                    if TaskCtrl.Instance.task_chain_monster_vo then
                        BehaviorData.Instance.auto_behavior = 3
                        local task_info = TaskData.Instance:GetTaskInfoByTaskId(CUSTOM_OBJID.TASK_CHAIN)
                        if task_info then
                            NpcDialogCtrl.Instance.data:SetCurTaskInfo(task_info)
                        end
                        SceneLogic.Instance:AutoToNpc(TaskCtrl.Instance.task_chain_monster_vo, nil, true, true)
                        return true
                    end
                end
            )
        end,
        true,
        true
    )
end

function TaskData:UpdateTaskChainPet(task, taskvo, info, pre_info)
    local total_num = info.cur_task_max_count
    local cur_num = info.task_pass_count + 1
    local npc_seq = info.task_param[1]
    local item_id = info.task_param[2]
    local pet_id = info.task_param[3]
    local pet_name = Pet.New {pet_id = pet_id}:Name()
    local accept_text
    local npc_cfg = Config.npc_dynamic_auto.npc_dynamic_cfg[npc_seq]
    taskvo.task_name = Language.TaskChain.Name[4]
    taskvo.npc_id = npc_seq
    if npc_cfg then
        local npc_name = npc_cfg.name
        taskvo.task_name = Language.TaskChain.Name[4] .. string.format("(%d/%d)", cur_num, total_num)
        taskvo.task_desc = string.format(Language.TaskChain.Goal.normal[4], npc_name, pet_name)
        taskvo.task_goal = string.format(Language.TaskChain.Goal.normal[4], npc_name, pet_name)
        taskvo.task_goal_menu = string.format(Language.TaskChain.Goal.menu[4], npc_name, pet_name)

        if pre_info and pre_info.task_type == TASK_CHAIN_TASK_TYPE.FIGHT then
            accept_text = string.format(Language.TaskChain.Accept[TASK_CHAIN_TASK_TYPE.PET][1], npc_name, pet_name)
        else
            accept_text = concatTaskChainText(Language.TaskChain.Accept[TASK_CHAIN_TASK_TYPE.PET], npc_name, pet_name)
        end

        self.task_chain_btn_handle =
            BehaviorData.Instance:CustomNpcTalkBtnsSeq(
            npc_seq,
            function(btn_list, obj_vo)
                table.insert(
                    btn_list,
                    NpcDialogBehBtnData.FuncCreate(
                        Language.TaskChain.Btn[4],
                        function()
                            self:doTaskChainPet(info)
                        end,
                        true
                    )
                )
                local can_skip, skip_btn = task:SkipBtn()
                if can_skip then
                    table.insert(btn_list, skip_btn)
                end
            end
        )
    else
        taskvo.task_desc = "缺少NPC配置 " .. npc_seq
        taskvo.task_goal = "缺少NPC配置 " .. npc_seq
    end
    self:doTaskChainStart(info, pre_info, accept_text)
end

function TaskData:doTaskChainPet(info)
    local npc_seq = info.task_param[1]
    local item_id = info.task_param[2]
    local pet_id = info.task_param[3]
    local pet_name = Pet.New {pet_id = pet_id}:Name()
    local items = {}
    local mutant_pet = PetData.Instance:GetMutantPetCfgByNormalId(pet_id).pet_id or 0
    if pet_id > 0 then
        for _, pet in pairs(PetData.Instance.pet_list) do
            if (pet:ID() == pet_id or pet:ID() == mutant_pet) and pet:CanSubmit() then
                table.insert(items, pet)
            end
        end
    end
    if next(items) then
        if ViewMgr:IsOpen(PetGain) then
            ViewMgr:CloseView(PetGain)
        end
        --到目标
        SceneLogic.Instance:AutoToNpc(
            npc_seq,
            function()
                TaskChainItemSel.item_id = item_id
                TaskChainItemSel.npc_seq = npc_seq
                TaskChainItemSel.pet_id = pet_id
                TaskChainItemSel.from_type = TaskConfig.TaskChainItemSelFrom.task_chain
                BehaviorData.Instance.auto_behavior = 2
                ViewMgr:OpenView(TaskChainItemSel)
            end,
            nil,
            true
        )
    else
        --判断一下仓库
        local storage_have = false
        if pet_id > 0 then
            for _, pet in pairs(PetData.Instance.pet_storage_list) do
                if pet:ID() == pet_id then
                    storage_have = true
                    break
                end
            end
        end
        local co = Cfg.ChainPetGroupByPI(pet_id)
        if nil == co then
            LogError(string.format("商人的烦恼 pet_id:%s 找不到巡逻场景", pet_id))
            return
        end
        local scene_id = co.scene_id
        local goto_scene = function()
            self.task_chain_sub_continue = true
            BehaviorCtrl.ExeByCfgVo(
                {
                    behavior_type = 9, --behavior_type 是 beh_id_to_tab.key
                    param1 = scene_id,
                    param2 = "",
                    param3 = ""
                }
            )
            ViewMgr:OpenView(FightAutoSealSetView, {scene_id = scene_id, target_pets = {{pet_id = pet_id, num = 1}}})
            ViewMgr:CloseView(DialogTipsView)
        end
        -- if storage_have == false then
            local goto_buy = function()
                TradeData.Instance:SetSearchItemId(pet_id)
                ViewMgr:OpenViewByKey(Mod.Mall.Trade)
                ViewMgr:CloseView(DialogTipsView)
            end
            BehaviorData.Instance.auto_behavior = 2
            PublicPopupCtrl.Instance:DialogTips {
                content = string.format(
                    Language.TaskChain.NotEnoughTips[4],
                    pet_name,
                    SceneData.Instance:GetSceneCfg(scene_id).name
                ),
                confirm = {func = goto_scene, name = Language.TaskChain.GetPet[1]},
                cancel = {func = goto_buy, name = Language.TaskChain.GetPet[2]},
                close = true,
            }
            -- else
                --     PublicPopupCtrl.Instance:DialogTips {
                    --         content = string.format(
                        --             Language.TaskChain.NotEnoughTips[5],
                        --             pet_name,
                        --             SceneData.Instance:GetSceneCfg(scene_id).name
                        --         ),
                        --         confirm = {
                            --             func = function()
                                --                 self.task_chain_sub_continue = true
        --                 ViewMgr:CloseView(DialogTipsView)
        --                 ViewMgr:OpenView(PetStorage)
        --             end
        --         },
        --         cancel = {func = goto_scene}
        --     }
        -- end
    end
end

function TaskData:UpdateTaskChainWait(task, taskvo, info, pre_info)
    local total_num = info.cur_task_max_count
    local cur_num = info.task_pass_count + 1
    local accept_text = ""
    local npc_from_seq = info.task_param[4]
    local item_id = info.task_param[2]
    local time_limit = info.task_param[3]
    local npc_to_seq = info.task_param[1]
    local npc_from_cfg = Config.npc_dynamic_auto.npc_dynamic_cfg[npc_from_seq]
    local npc_to_cfg = Config.npc_dynamic_auto.npc_dynamic_cfg[npc_to_seq]
    taskvo.task_name = Language.TaskChain.Name[5] .. string.format("(%d/%d)", cur_num, total_num)
    -- taskvo.npc_id = npc_seq
    if npc_from_cfg and npc_to_cfg then
        local npc_from_name = npc_from_cfg.name
        local npc_to_name = npc_to_cfg.name
        local desc =
            string.format(Language.TaskChain.Goal.normal[5], npc_from_name, Item.GetName(item_id), npc_to_cfg.name)
        local desc_menu =
            string.format(Language.TaskChain.Goal.menu[5], npc_from_name, Item.GetName(item_id), npc_to_cfg.name)
        taskvo.task_desc = desc
        taskvo.task_goal = desc
        taskvo.task_goal_menu = desc_menu
        info.special = {
            desc = desc,
            desc_tf = Language.TaskChain.Goal.normal.t5f,
            desc_menu = desc_menu,
            desc_menu_tf = Language.TaskChain.Goal.menu.t5f
        }

        -- if pre_info and pre_info.task_type == TASK_CHAIN_TASK_TYPE.FIGHT then
        --     accept_text = string.format(Language.TaskChain.Accept[TASK_CHAIN_TASK_TYPE.ITEM][1], npc_name, Item.GetName(item_id))
        -- else
        --     accept_text = concatTaskChainText(Language.TaskChain.Accept[TASK_CHAIN_TASK_TYPE.ITEM], npc_name, Item.GetName(item_id))
        -- end
        accept_text =
            concatTaskChainTextLT(
            Language.TaskChain.Accept[TASK_CHAIN_TASK_TYPE.WAIT].waiting,
            time_limit,
            Item.GetName(item_id),
            npc_to_cfg.name
        )

        self.task_chain_btn_handle =
            BehaviorData.Instance:CustomNpcTalkBtnsSeq(
            0 == BagData.Instance:GetNumByItemId(item_id) and npc_from_seq or npc_to_seq,
            function(btn_list, obj_vo)
                table.insert(
                    btn_list,
                    NpcDialogBehBtnData.FuncCreate(
                        Language.TaskChain.Btn[5],
                        function()
                            self:doTaskChainWait(info)
                        end,
                        function()
                            return BagData.Instance:GetNumByItemId(item_id) > 0
                        end
                    )
                )
                -- local can_skip, skip_btn = task:SkipBtn()
                -- if can_skip then table.insert(btn_list, skip_btn) end
            end
        )
    else
        taskvo.task_desc = string.format("缺少NPC配置 %s %s", npc_from_seq, npc_to_seq)
        taskvo.task_goal = string.format("缺少NPC配置 %s %s", npc_from_seq, npc_to_seq)
    end
    self:doTaskChainStart(info, pre_info, accept_text)
end
function TaskData:doTaskChainWait(info)
    local show_text = ""
    local npc_from_seq = info.task_param[4]
    local item_id = info.task_param[2]
    local time_limit = info.task_param[3]
    local npc_to_seq = info.task_param[1]
    local npc_seq = 0 == BagData.Instance:GetNumByItemId(info.task_param[2]) and npc_from_seq or npc_to_seq
    local npc_to_cfg = Config.npc_dynamic_auto.npc_dynamic_cfg[npc_to_seq]
    SceneLogic.Instance:AutoToNpc(
        npc_seq,
        function()
            local end_func = nil
            if TimeHelper.GetServerTime() > time_limit then
                if 0 == BagData.Instance:GetNumByItemId(item_id) then
                    show_text =
                        concatTaskChainTextLT(
                        Language.TaskChain.Accept[TASK_CHAIN_TASK_TYPE.WAIT].done,
                        time_limit,
                        Item.GetName(item_id),
                        npc_to_cfg.name
                    )
                    end_func = function ()
                        TaskCtrl.Instance:CSTaskChainOp(TASK_CHAIN_OP_TYPE.WAIT_ITEM)
                    end
                else
                    TaskChainItemSel.item_id = item_id
                    TaskChainItemSel.pet_id = 0
                    TaskChainItemSel.npc_seq = npc_to_seq
                    TaskChainItemSel.from_type = TaskConfig.TaskChainItemSelFrom.task_chain
                    BehaviorData.Instance.auto_behavior = 2
                    ViewMgr:OpenView(TaskChainItemSel)
                    return
                end
            else
                show_text =
                    concatTaskChainTextLT(
                    Language.TaskChain.Accept[TASK_CHAIN_TASK_TYPE.WAIT].waiting,
                    time_limit,
                    Item.GetName(item_id),
                    npc_to_cfg.name
                )
            end
            BehaviorData.Instance.auto_behavior = 2
            local cur_talker = BehaviorData.Instance:NpcSeqToNpcId(npc_from_seq)
            NpcDialogCtrl.Instance:NpcDialogView(
                {
                    talk_list = {
                        {
                            talker = cur_talker,
                            talk_text = show_text
                        },
                        talker = cur_talker
                    },
                    beh_btn_list = {}
                },
                end_func
            )
        end,
        nil,
        true
    )
end
