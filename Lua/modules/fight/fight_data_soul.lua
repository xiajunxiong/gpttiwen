
function FightData:FightDataSoul()
    self.fight_soul_sm = SmartData.New{
        info = SmartData.New()
    }
    self.fight_soul_info = SmartData.New{
        seq = 0
    }

    self.fight_soul_has_remind = SmartData.New{
        value = true
    }

    self.fight_soul_list = {}
    for _, v in ipairs(Config.break_cfg_auto.soul_fight) do
        local monster = Cfg.MonsterById(v.monster_id)
        v.monster_icon = monster and monster.icon_id or 0
        v.monster_res = monster and monster.res_id or 0
        v.monster_name = monster and monster.name or ""
        self.fight_soul_list[v.seq] = v
    end
    self.soul_help_fight = Config.break_cfg_auto.help_fight

    ActFormationData.Instance:RegistFunctions(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_REALM_BREAK_FIGHT,
        {MainType = FightData.Instance, challenge_func = "SoulFightChallenge", pet_on_limit_func = "SoulPetOnLimitNum", assist_text_func = "SoulAssistTextFunc",
        assist_limit_text_func = "SoulAssistLimitTip", crystal_tip_func = "SoulElementRecommend"}
    )
end

function FightData:SetFightSoulInfo(protocol)
    self.fight_soul_info.seq = protocol.cur_fight_seq
    self.fight_soul_has_remind:Notify()
end

function FightData:GetFightSoulRewardShow(info)
    if nil == info.reward_list then
        local t = {}
        local function CheckReward(reward, item_id)
            if reward ~= 0 then
                local item_data = Item.New()
                item_data.item_id = item_id
                item_data.num = reward
                item_data.is_bind = 0
                table.insert(t, item_data)
            end
        end
        CheckReward(info.coin, CommonItemId.Coin)
        CheckReward(info.break_exp, CommonItemId.LingLiZhi)
        CheckReward(info.player_exp, CommonItemId.Exp)
        for i = 0, #info.break_item do
            local item_data = info.break_item[i]
            table.insert(t, Item.Init(item_data.item_id, item_data.num, item_data.is_bind))
        end
        info.reward_list = t
    end
    for _, reward in ipairs(info.reward_list) do
        reward.is_grey = self.fight_soul_info.seq >= info.seq
    end
    return info.reward_list
end
function FightData:FightSoulTianLeiRound()
    return Config.break_cfg_auto.other[1].round
end

function FightData:GetFightSoulCapShow()
    local value = 0
    local info = self.fight_soul_list[self.fight_soul_info.seq + 1]
    if info then
        return true, info.recommend_score
    end
    return false
end

function FightData:GetFightSoulList()
    local seq = self.fight_soul_info.seq
    local click_item = self.fight_soul_list[seq + 1] or self.fight_soul_list[seq]
    return self.fight_soul_list, click_item
end

function FightData:GetFightSoulRemind()
    -- return 0
    -- 屏蔽红点
    if not self.fight_soul_has_remind.value then
        return 0
    end
    local seq = self.fight_soul_info.seq
    local show_list, click_item = self:GetFightSoulList()
    -- 打完了不显示
    if (seq + 1) > #show_list then
        return 0
    else
        return RoleData.Instance:GetRoleLevel() >= click_item.level and 1 or 0
    end
end

function FightData:ResetFightRemindData()
    -- 0是勾选1是不勾
    self.fight_soul_has_remind.value = PrefsInt(PrefKeys.FightSoulRemind()) == 0
    self.fight_soul_has_remind:Notify()
end

-- 心灵之境获得奖励
function FightData:SetFightSoulRewardData(reward_list)
    self.fight_soul_reward_list = reward_list
end

-- 心灵之境获得奖励
function FightData:GetFightSoulRewardData()
    return self.fight_soul_reward_list or {}
end

-- 心灵之境获得宠物
function FightData:SetFightSoulRewardPetData(pet_info)
    self.fight_soul_reward_pet = pet_info
end

-- 心灵之境获得宠物
function FightData:GetFightSoulRewardPetData()
    return self.fight_soul_reward_pet or {}
end

-- 返回已通过的关卡数量和当前能通过的总关卡数量
function FightData:GetCanFightAndTotal()
    local can_fight_total_num = 0
    for i, v in ipairs(self.fight_soul_list) do
        if RoleLevel() >= v.level then
            can_fight_total_num = can_fight_total_num + 1
        end
    end
    return self.fight_soul_info.seq, can_fight_total_num
end
function FightData:SoulStoryPlaying()
    return self.is_soul_story_play == true
end
function FightData:SetSoulStoryPlaying(playing)
    self.is_soul_story_play = playing
    if playing then
        if not ViewMgr:IsOpen(GuideBlock) then
            ViewMgr:OpenView(GuideBlock)
        end
    else
        ViewMgr:CloseView(GuideBlock)
    end
end
-- 特效-出现BOSS-对话-进入战斗
function FightData:SoulStoryShow(info, func)
    if self.soul_boss then
        self.soul_boss:DeleteMe()
        self.soul_boss = nil
    end
    if info.special_perfor == 1 then
        -- LogDG(info)
        if self.is_soul_story_play == true then
            return
        end
        ViewMgr:CloseView(ActFormationView)
        ViewMgr:CloseView(FightSoulView)
        ViewMgr:CloseView(OfflineRewardView)
        self:SetSoulStoryPlaying(true)
        self:SoulShowEffect(function (pos, forward)
            local vo = CommonStruct.MonsterVo(info)
            vo.obj_type = SceneObjType.SoulBoss
            vo.res_id = info.monster_res
            vo.name = info.monster_name
            vo.not_affect_yinyang = true
            vo.info = info
            self.soul_boss = SceneObj.CreateObj(vo)
            self.soul_boss:SetForward(forward)
            self.soul_boss:SetPos(pos)
            if MainPlayer then
                MainPlayer:SetForward(-forward)
            end
            self.handle_effect4 = SE:Play(SceneEffectConfig.EffectList.SoulBossShow4, nil, nil, self.soul_boss.root_node)
        end)
    else
        ViewMgr:CloseView(ActFormationView)
        ViewMgr:CloseView(FightSoulView)
        ViewMgr:CloseView(OfflineRewardView)
        RoleCtrl.Instance:SendOfflineBreakFight(info.seq)
    end
end
function FightData:SoulShowEffect2(func)
    if self.soul_boss then
        self:SetSoulStoryPlaying(true)
        local opos = self.soul_boss:GetPos():Clone()
        -- local opos= self:CalcBossPos()
        -- opos = opos:Clone()
        local epos = MainPlayer:GetPos():Clone()
        opos.y = opos.y + 1.5
        epos.y = epos.y + 1.5
        self.handle_effect5 = SE:Play(SceneEffectConfig.EffectList.SoulBossShow3, nil, nil, self.soul_boss.root_node)--自删
        -- self.handle_effect5 = SE:Play(SceneEffectConfig.EffectList.SoulBossShow3, opos)--自删
        self.handle_effects2 = {}
        self.curve_parabola2 = {}
        self.handle_runframe2 = {}
        local cpos = (opos + epos) / 2
        local crpos = Vector3.one
        for i = 1, 5 do
            crpos:Set(cpos.x * GameMath.FloatRamdon(1,3), cpos.y * GameMath.FloatRamdon(1.1,2.2), cpos.z * GameMath.FloatRamdon(1,3))
            self.curve_parabola2[i] = Game.CurveParabola(opos, epos, crpos, 1)
        end
        local key = 1
        TimeHelper:AddRunFrameTimer(function ()
            self.handle_effects2[key] = SE:Play(SceneEffectConfig.EffectList.SoulBossShow2, opos, BindTool.Bind(self.UpdatePos2, self, key, func))
            key = key + 1
        end, 2, 10)
    end
end
-- BOSS出现前特效
function FightData:SoulShowEffect(func)
    self:SetSoulStoryPlaying(true)
    local pos, forward = self:CalcBossPos()
    local opos = MainPlayer:GetPos():Clone()
    local epos = pos:Clone()
    SE:Stop(self.handle_effect1)
    self.handle_effect1 = SE:Play(SceneEffectConfig.EffectList.SoulBossShow4, nil, nil, MainPlayer.root_node)
    TimeHelper:CancelTimer(self.handle_delay)
    self.handle_delay = TimeHelper:AddDelayTimer(function ()
        self:SetSoulStoryPlaying(true)
        opos.y = opos.y + 1.5
        epos.y = epos.y + 1.5
        self.handle_effects = {}
        self.curve_parabola = {}
        self.handle_runframe = {}
        local cpos = (opos + epos) / 2
        local crpos = Vector3.one
        for i = 1, 5 do
            crpos:Set(cpos.x * GameMath.FloatRamdon(1,3), cpos.y * GameMath.FloatRamdon(1.1,2.2), cpos.z * GameMath.FloatRamdon(1,3))
            self.curve_parabola[i] = Game.CurveParabola(opos, epos, crpos, 1)
        end
        local key = 1
        TimeHelper:AddRunFrameTimer(function ()
            self.handle_effects[key] = SE:Play(SceneEffectConfig.EffectList.SoulBossShow5, opos, BindTool.Bind(self.UpdatePos, self, pos, forward, key, func))
            key = key + 1
        end, 2, 10)
    end,1)
end
function FightData:UpdatePos(pos, forward, idx, func, obj, key, time)
    if obj then
        local frame = 1
        local total = 24
        local value = 0
        self.handle_runframe[key] = TimeHelper:AddRunFrameTimer(function ()
            value = Mathf.Clamp((frame/ total), 0, 1)
            if obj and not obj:Equals(nil) then
                obj.transform.localPosition = self.curve_parabola[idx]:GetPos(value)
            else
                TimeHelper:CancelTimer(self.handle_runframe[idx])
                SE:Stop(self.handle_effects[idx])
            end
            --Vector3.Lerp(opos, epos, value)
            frame = frame + 1
            if frame > total then
                SE:Stop(self.handle_effects[idx])
                if idx == 1 then
                    self.handle_effect3 = SE:Play(SceneEffectConfig.EffectList.SoulBossShow1, pos, function (obj1, key1, time)
                        if obj1 and key1 == "start" then
                            if func then
                                func(pos, forward)
                            end
                            SE:Stop(self.handle_effect1)
                        end
                    end)
                end
            end
        end, 1, total)
    end
end
function FightData:UpdatePos2(idx, func, obj, key, time)
    if obj then
        local frame = 1
        local total = 24
        local value = 0
        self.handle_runframe2[key] = TimeHelper:AddRunFrameTimer(function ()
            value = Mathf.Clamp((frame/ total), 0, 1)
            if obj and not obj:Equals(nil) then
                obj.transform.localPosition = self.curve_parabola2[idx]:GetPos(value)
            else
                TimeHelper:CancelTimer(self.handle_runframe2[idx])
                SE:Stop(self.handle_effects2[idx])
            end
            --Vector3.Lerp(opos, epos, value)
            frame = frame + 1
            if frame > total then
                SE:Stop(self.handle_effects2[idx])
                if idx == 5 then
                    self:SetSoulStoryPlaying(false)
                    SE:Stop(self.handle_effect5)
                    if func then
                        func()
                    end
                end
            end
        end, 1, total)
    end
end
function FightData:CalcBossPos()
    local pos1 = MainPlayer:GetPos()
    local pos2 = pos1 + Vector3.New(4, 0, 4)
    local y = SceneCtrl.Instance:CurScene():GetHeight(pos2.x, pos2.z)
    local forward = pos1 - pos2
    if y ~= 0 then
        return pos2, forward
    end
    pos2 = pos1 + Vector3.New(-4, 0, -4)
    y = SceneCtrl.Instance:CurScene():GetHeight(pos2.x, pos2.z)
    if y ~= 0 then
        forward = pos1 - pos2
        return pos2, forward
    end
    pos2 = pos1 + Vector3.New(-4, 0, 4)
    y = SceneCtrl.Instance:CurScene():GetHeight(pos2.x, pos2.z)
    if y ~= 0 then
        forward = pos1 - pos2
        return pos2, forward
    end
    pos2 = pos1 + Vector3.New(4, 0, -4)
    y = SceneCtrl.Instance:CurScene():GetHeight(pos2.x, pos2.z)
    if y ~= 0 then
        forward = pos1 - pos2
        return pos2, forward
    end
    return pos2, forward
end
--结算走这个
function FightData:SoulDisappear(func)
    if self.soul_boss then
        self:SetSoulStoryPlaying(true)
        local function showEffEnd()
            self.soul_boss:Disappear(function ()
                self:ClearSoulBoss()
                func()
            end)
        end
        self:SoulShowEffect2(showEffEnd)--特效播完再弹窗
    else
        if func then
            func()
        end
    end
end
function FightData:SoulAssistTextFunc(assist_num)
    --判断
    local day = RoleData.Instance:CreateDayEx()
    if day >= self.soul_help_fight[#self.soul_help_fight].creat_role_days then
        local config = self.soul_help_fight[#self.soul_help_fight]
        return string.format(Language.Fight.SoulLockPet2, assist_num, config.pet_num)
    else
        local config = self.soul_help_fight[day]
        local t0 = TimeManager.FormatUnixTime2Date(TimeHelper.GetServerTime())
        local desc = string.format(Language.Fight.SoulLockPet, 0, 24 - t0.hour, config.pet_num + 1)
        return desc..string.format(Language.Fight.SoulLockPet2, assist_num, config.pet_num)
    end
end
function FightData:SoulPetOnLimitNum()
    local day = RoleData.Instance:CreateDayEx()
    if day >= FightData.Instance.soul_help_fight[#FightData.Instance.soul_help_fight].creat_role_days then
        return FightData.Instance.soul_help_fight[#FightData.Instance.soul_help_fight].pet_num
    else
        local config = FightData.Instance.soul_help_fight[day]
        return config.pet_num
    end
end
-- 挑战
function FightData:SoulFightChallenge()
    -- MainViewData.Instance:HideTask()
    MainViewData.Instance:HideLtButton()
    FightData.Instance:SoulStoryShow(FightData.Instance.fight_soul_sm.info)
end
function FightData:ClearSoulBoss()
    SE:Stop(self.handle_effect4)
    SE:Stop(self.handle_effect1)
    if self.handle_effects then
        for k, v in pairs(self.handle_effects) do
            SE:Stop(v)
        end
    end
    if self.handle_effects2 then
        for k, v in pairs(self.handle_effects2) do
            SE:Stop(v)
        end
    end
    if self.handle_runframe then
        for k, v in pairs(self.handle_runframe) do
            TimeHelper:CancelTimer(v)
        end
    end
    if self.handle_runframe2 then
        for k, v in pairs(self.handle_runframe2) do
            TimeHelper:CancelTimer(v)
        end
    end
    -- LogError("ClearSoulBoss")
    if self.soul_boss then
        self.soul_boss:DeleteMe()
        self.soul_boss = nil
    end
end
function FightData:ClearSoulData()
    self:ClearSoulBoss()
    self:SetSoulStoryPlaying(false)
end

function FightData:SoulAssistLimitTip()
    local tips_table = {}
    local day = RoleData.Instance:CreateDayEx()
    if day >= FightData.Instance.soul_help_fight[#FightData.Instance.soul_help_fight].creat_role_days then
    else
        local t0 = TimeManager.FormatUnixTime2Date(TimeHelper.GetServerTime())
        for i,v in ipairs(FightData.Instance.soul_help_fight) do
            if day < v.creat_role_days then
                if #tips_table < 1 then
                    table.insert(tips_table, Language.Fight.SoulLockPet4)
                else
                    table.insert(tips_table, string.format(Language.Fight.SoulLockPet3, v.creat_role_days - day - 1, 24 - t0.hour))
                end
            end
        end
        -- 
        -- if day == 1 then
        --     return {string.format(Language.Fight.SoulLockPet3, 0, 24 - t0.hour), string.format(Language.Fight.SoulLockPet3, 1, 24 - t0.hour)}
        -- else
        --     return {string.format(Language.Fight.SoulLockPet3, 0, 24 - t0.hour)}
        -- end
    end
    return tips_table
end

-- 返回推荐灵珠道具id
function FightData:SoulElementRecommend()
    local info = self.fight_soul_sm.info
    local monster = Cfg.MonsterById(info.monster_id)
    if not TableIsEmpty(monster) then
        local element_list = MonsterData.GetElement(monster)
        if not TableIsEmpty(element_list) then
            for k, v in pairs(element_list) do
                if BattleRestraintElement[k] then
                    return BattleAttriElementId[BattleRestraintElement[k]]
                end
            end
        end
    end
    return BattleAttriElementId[40]
end