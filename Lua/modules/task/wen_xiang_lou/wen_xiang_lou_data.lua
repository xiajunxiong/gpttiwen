
WenXiangLouData = WenXiangLouData or BaseClass()

function WenXiangLouData:__init()
    if WenXiangLouData.Instance ~= nil then
        Debuger.LogError("[WenXiangLouData] attempt to create singleton twice!")
        return
    end
    WenXiangLouData.Instance = self
    self:WenXiangLouConfig()
    self:WenXiangLouData()
end

function WenXiangLouData:WenXiangLouConfig()
    self.config = Config.brothel_murder_auto
    self.config_role_clue = self.config.role_clue
    self.config_item_clue = self.config.item_clue
    self.config_other = self.config.other[1]

    Cfg.Register("WenXiangLouNpcBySeq",Cfg.RealTimeSearch(self.config_role_clue,"seq"))
    Cfg.Register("WenXiangLouItemBySeq",Cfg.RealTimeSearch(self.config_item_clue,"seq"))
end

function WenXiangLouData:WenXiangLouData()
    self.npc_show_list = {}
    self.item_show_list = {}
    self.scene_id = self.config_other.scene_id

    self.npc_list = {}
    self.killer = {}
    self.special_npcs = {}

    for k, v in ipairs(self.config_role_clue) do
        v.npc_seqs = {
            [WenXiangLouConfig.LoadType.type0] = v.stage_1_seq,
            [WenXiangLouConfig.LoadType.type1] = v.stage_1_seq,
            [WenXiangLouConfig.LoadType.type2] = v.stage_3_seq,
            [WenXiangLouConfig.LoadType.type3] = v.stage_3_seq,
            [WenXiangLouConfig.LoadType.type4] = v.stage_3_seq,
        }
        v.npc_talks = {
            [WenXiangLouConfig.LoadType.type0] = v.stage_1,
            [WenXiangLouConfig.LoadType.type1] = v.stage_1,
            [WenXiangLouConfig.LoadType.type2] = v.stage_2,
            [WenXiangLouConfig.LoadType.type3] = v.stage_2,
            [WenXiangLouConfig.LoadType.type4] = v.stage_2,
        }
        local npc_co = NpcHelper.NpcCo(v.npc_id)
        v.icon_id = npc_co.npc_icon
        self.npc_list[v.npc_id] = v
        self.special_npcs[v.stage_3_seq] = v
        if 3 == v.identity then
            self.killer = v
        end
    end

    self.detail_info = SmartData.New{
        load_status = WenXiangLouConfig.LoadType.none,
        npc_sucspicion_list = SmartData.New(),
        npc_talk_list = SmartData.New(),
        npc_identity_list = SmartData.New(),
        npc_check_list = SmartData.New(),
        reward_list = SmartData.New(),
        info_type = WenXiangLouConfig.InfoType.check,
        info_sel_seq = -1,
        key_seq = -1,
    }
end

function WenXiangLouData:ResetDetailInfo()
    TimeHelper:CancelTimer(self.timer_rt)
    self.detail_info:Set{
        load_status = WenXiangLouConfig.LoadType.none,
        npc_sucspicion_list = SmartData.New(),
        npc_talk_list = SmartData.New(),
        npc_identity_list = SmartData.New(),
        npc_check_list = SmartData.New(),
        reward_list = SmartData.New(),
        info_type = WenXiangLouConfig.InfoType.check,
        info_sel_seq = -1,
        key_seq = -1,
    }
    for i = 1, #self.npc_show_list do
        SceneCtrl.Instance:DeleteNpc(self.npc_show_list[i])
    end
    self.npc_show_list = {}
    for i = 1, WenXiangLouConfig.MAX_ITEM_NUM do
        if nil ~= self.item_show_list[i] then
            SE:Release(self.item_show_list[i])
            self.item_show_list[i] = nil
        end
    end
    self.item_show_list = {}
end

function WenXiangLouData:SetWenXiangLouNpcInfo(protocol)
    local info = protocol.info
    self:SetInfoLoadState(info.load_status)
    self:SetInfoNpcSucspicionList(info.npc_sucspicion_list)
    self:SetInfoNpcIdentityList(info.npc_identity_list)
    self.detail_info.npc_talk_list:Set(info.npc_talk_list)
    self.detail_info.npc_check_list:Set(info.npc_check_list)
end

function WenXiangLouData:SetWenXiangLouReqSceneInfo(protocol)
    local info = protocol.info
    -- if info.load_status ~= self.detail_info.load_status then
        for i = 1, #self.npc_show_list do
            SceneCtrl.Instance:DeleteNpc(self.npc_show_list[i])
        end
        self.npc_show_list = {}
        for k, v in pairs(self.npc_list) do
            local seq = v.npc_seqs[info.load_status]
            if seq > 0 then
                SceneCtrl.Instance:AddNpc(seq)
                table.insert(self.npc_show_list, seq)
            end
        end
    -- end
    if 1 == info.has_find_identity and info.load_status < WenXiangLouConfig.LoadType.type3 then
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.killer.stage_3}, function ()
            WenXiangLouCtrl.Instance:SendWenXiangLouFight()
        end)
    end
    self:SetInfoLoadState(info.load_status)
    self:SetInfoRewardList(info.reward_list)
end

function WenXiangLouData:SetInfoLoadState(load_status)
    if WenXiangLouConfig.LoadType.type1 == load_status and WenXiangLouConfig.LoadType.type0 == self.detail_info.load_status then
        SceneLogic.Instance:ClearMoveCache()
        ViewMgr:OpenView(WenXiangLouShoutView)
    end
    if WenXiangLouConfig.LoadType.type2 == load_status and WenXiangLouConfig.LoadType.type1 == self.detail_info.load_status then
        SceneLogic.Instance:ClearMoveCache()
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.config_other.talk_2})
    end
    if WenXiangLouConfig.LoadType.type3 == load_status and self.detail_info.load_status < WenXiangLouConfig.LoadType.type3 then
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.config_other.prove_correct}, function ()
            WenXiangLouCtrl.Instance:SendWenXiangLouFight()
        end)
    end
    if WenXiangLouConfig.LoadType.type4 == load_status then
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.config_other.end_talk}, function ()
            AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.WenXiangLou)
            Invoke(function()
                local task_info = AnecdoteData.Instance:GetTaskInfoByType(AnecdoteType.WenXiangLou)
                if task_info then task_info:RequestDoing() end
            end,1)
        end)
    end
    self.detail_info.load_status = load_status
end

function WenXiangLouData:SetInfoRewardList(reward_list)
    self.detail_info.reward_list:Set(reward_list)
    if reward_list[self.detail_info.key_seq] then
        self.detail_info.key_seq = -1
    end
    self:FlushItemShowList(reward_list)
end

function WenXiangLouData:FlushItemShowList(reward_list)
    TimeHelper:CancelTimer(self.timer_rt)
    self.timer_rt = TimeHelper:AddRunTimer(function ()
        if SceneData.Instance:GetMode() == SCENE_TYPE.WEN_XIANG_LOU then
            TimeHelper:CancelTimer(self.timer_rt)
            for i = 1, WenXiangLouConfig.MAX_ITEM_NUM do
                if self.detail_info.load_status < WenXiangLouConfig.LoadType.type2 or reward_list[i] then
                    if self.item_show_list[i] then
                        SE:Release(self.item_show_list[i])
                        self.item_show_list[i] = nil
                    end
                elseif nil == self.item_show_list[i] then
                    local co = Cfg.WenXiangLouItemBySeq(i)
                    if co then
                        local pos = SceneCtrl.Instance:CurScene():PosToClint(co.x, co.y)
                        self.item_show_list[i] = SE:Play(SceneEffectConfig.EffectList.TaskWXLItemMono, pos)
                    end
                end
            end
        end
    end, 1)
end

function WenXiangLouData:SetInfoNpcSucspicionList(npc_sucspicion_list)
    for i = 0, WenXiangLouConfig.MAX_NPC_NUM - 1 do
        local pre_val = self.detail_info.npc_sucspicion_list[i]
        if pre_val and 0 ~= pre_val and pre_val ~= npc_sucspicion_list[i] then
            PublicPopupCtrl.Instance:Center(Language.Task.WXLInfo.sucspicion_change_tips)
            break
        end
    end
    self.detail_info.npc_sucspicion_list:Set(npc_sucspicion_list)
end

function WenXiangLouData:SetInfoNpcIdentityList(npc_identity_list)
    for i = 0, WenXiangLouConfig.MAX_NPC_NUM - 1 do
        local pre_val = self.detail_info.npc_identity_list[i]
        if pre_val and pre_val ~= npc_identity_list[i] and 1 == npc_identity_list[i] then
            local co = Cfg.WenXiangLouNpcBySeq(i)
            if 3 ~= co.identity then
                NpcDialogCtrl.Instance:NpcDialogView({talk_id = co.stage_3}, function ()
                    PublicPopupCtrl.Instance:Center(Language.Task.WXLInfo.error_tips)
                end)
            end
            break
        end
    end
    self.detail_info.npc_identity_list:Set(npc_identity_list)
end

function WenXiangLouData:IsWenXiangLouNpc(param_t,obj_vo)
    if not MainViewCtrl.Instance:IsInGameState(MainViewGameState.wen_xiang_lou) then
        return false
    end
    if obj_vo and self.npc_list[obj_vo.id] or self.special_npcs[obj_vo.seq]then
        local co = self.npc_list[obj_vo.id] or self.special_npcs[obj_vo.seq]
        local seq = co.seq
        local is_talk = false
        if self.detail_info.load_status <= WenXiangLouConfig.LoadType.type1 then
            is_talk = 0 == (self.detail_info.npc_talk_list[seq] or 1)
        else
            is_talk = 0 == (self.detail_info.npc_check_list[seq] or 1)
        end
        if is_talk then
            NpcDialogCtrl.Instance:NpcDialogView({talk_id = co.npc_talks[self.detail_info.load_status]},
                function ()
                    WenXiangLouCtrl.Instance:SendWenXiangLouTalk(seq)
                end
            )
            return true
        end
    end
end

function WenXiangLouData:GetInfoList()
    local npc_info_list = {}
    for k, v in pairs(self.npc_list) do
        if v.suspicion > 0 then
            table.insert(npc_info_list, v)
        end
    end
    table.sort(npc_info_list, function (a, b)
        if not a or not b then return a end
        local ai = self.detail_info.npc_identity_list[a.seq]
        local bi = self.detail_info.npc_identity_list[b.seq]
        if ai == bi then
            return a.seq < b.seq
        end
        return ai < bi
    end)
    return npc_info_list
end

function WenXiangLouData:SetDetailInfoInfoType(info_type)
    self.detail_info.info_type = info_type
end

function WenXiangLouData:IsNpcCanReport(seq)
    return self.detail_info.npc_sucspicion_list[seq] >= self.config_other.min_suspicion, self.config_other.min_suspicion
end

function WenXiangLouData:CheckKey(pos)
    if self.detail_info.load_status < WenXiangLouConfig.LoadType.type2 then
        self.detail_info.key_seq = -1
        return
    end
    local distance = self.config_other.item_radius
    for _, item in ipairs(self.config_item_clue) do
        if not self.detail_info.reward_list[item.seq] then
            local target = SceneCtrl.Instance:CurScene():PosToClint(item.x, item.y)
            if GameMath.GetDistance(pos.x, pos.z, target.x, target.z) <= distance then
                self.detail_info.key_seq = item.seq
                return
            end
        end
    end
    self.detail_info.key_seq = -1
end

function WenXiangLouData:GetKeysShow()
    local keys_show = ""
    local space = RichTextHelper.ColorStr("AAAA", COLORSTR.Transparent)
    for i = 0, WenXiangLouConfig.MAX_NPC_NUM - 1 do
        if self.detail_info.npc_check_list[i] > 0 then
            local co = Cfg.WenXiangLouNpcBySeq(i)
            keys_show = keys_show .. string.format("%s%s\n", space, co.note)
        end
    end
    for i = 1, WenXiangLouConfig.MAX_ITEM_NUM do
        if self.detail_info.reward_list[i]then
            local co = Cfg.WenXiangLouItemBySeq(i)
            keys_show = keys_show .. string.format("%s%s\n", space, co.note)
        end
    end
    return string.sub(keys_show, 1, -2)
end

function WenXiangLouData:GetNpcMinSuspicion()
    return self.config_other.min_suspicion
end

function WenXiangLouData:FirstEnterTalk()
    NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.config_other.talk_1})
end