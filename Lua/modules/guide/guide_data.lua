GuideData = GuideData or BaseClass()
--用于检查，并方便修改
GuideData.GuideList =
{
    [25] = true,--宠物洗练
    [14] = true,--竞技场指引
    [46] = true,--破防指引
    [43] = true,--法宝指引
}

--指引结束需要执行的func
GuideData.FinishFunc = {
    [57] = function()
        RewardData.Instance:CheckLevelReward()
    end
}

--判断指引需要停止的条件func
GuideData.StopFunc = {

}
function GuideData:__init()
    if GuideData.Instance ~= nil then
        Debuger.LogError("[GuildCtrl] attempt to create singleton twice!")
        return
    end
    GuideData.Instance = self
    self.all_guide = Config.function_guide_auto
    self.guide_list = self.all_guide.guide_list
    self.step_list = self.all_guide.guide
    self.work_shop_foring = self.all_guide.work_shop_foring
    self.skill_ids = self.all_guide.skill_id
    self.skill_ids2 = self.all_guide.skill_id_2
    self.learn_skill = self.all_guide.guide_skill_id
    self.pet_skill = self.all_guide.pet_skill[1]
    self.choose_pet = self.all_guide.choose_pet
    self.new_mode_guide = self.all_guide.new_guide
    Cfg.Register("GuideCfgById",Cfg.RealTimeSearch(self.guide_list,"id"))
    self.guide_cfg_list = {}
    self.step_cfg_list = {}
    self.finish_task_guides = {}
    self.listen_finish_task_guides = {}



    self:checkConfig()
    self.guide_flag = {}
    self.notice_info = SmartData.New()
    self.notice_flush_info = SmartData.New({flush = 0})
    self.open_active_guide = SmartData.New({open = false})
    self.change_pet_guide_trigger_times = {}
    self.yinyang_eye_open = false
    self.yinyang_eye_effect = SmartData.New({id = 0, id2 = 0})
    self.guihuo_arrow_open = false
    self.rolling_task_box = SmartData.New({start = false})
end

function GuideData:checkConfig()
    for _, value in ipairs(self.guide_list) do
        if self.guide_cfg_list[value.id] == nil then
            self.guide_cfg_list[value.id] = {}
        end
        self.guide_cfg_list[value.id] = value
        if value.trigger_type == GuideTriggerType.FinishTask then
            self.finish_task_guides[value.trigger_param_1] = value
            table.insert(self.listen_finish_task_guides, value.trigger_param_1)
        end
    end
    for _, value in ipairs(self.step_list) do
        if self.step_cfg_list[value.step_id] == nil then
            self.step_cfg_list[value.step_id] = {}
        end
        if self.step_cfg_list[value.step_id][value.index] then
            LogError("指引配置出现重复step_id = index =",value.step_id, value.index)
        else
            if IS_STANDALONE() then
                if value.step_id == 74 and value.index == 4 then

                else
                    value.guide_auto = 3
                end
            end
            self.step_cfg_list[value.step_id][value.index] = value
        end
    end

    self.funopen_notice_sn_list = KeyList(Config.function_notice_auto.notice,"sn")
    self.funopen_notice_id_list = KeyList(Config.function_notice_auto.notice,"notice_id")
    self.notice_mission_id_list = KeyList(Config.function_notice_auto.notice_mission,"notice_id")
end

function GuideData:GuideCfgByTrigger(trigger_type, p1, p2, p3)
    for key, value in pairs(self.guide_cfg_list) do
        if value.trigger_type == trigger_type then
            if value.trigger_param_1 == p1 then
                if value.trigger_type == GuideTriggerType.EnterBattle then
                    if value.trigger_param_2 == BattleData.Instance:BaseData().cur_round then
                        return value
                    end
                else
                    return value
                end
            end
        end
    end
    return nil
end

function GuideData:GuideCfg(guideid)
    local guide = self.guide_cfg_list[guideid]
    local step = nil
    if not guide then
        LogError("不存在指引", guideid)
    else
        step = self.step_cfg_list[guide.step_id]
    end
    return guide, step
end

--获取背包装备指引ID
function GuideData:GetBgaEquipId()
    local prof = RoleData.Instance:GetRoleProf()
    if prof ~= 0 then
        return self.all_guide.bag_equip[prof].item_id
    end
    return 0
end

--获取工坊锻造指引ID
function GuideData:GetForingId()
    local prof = RoleData.Instance:GetRoleProf()
    if prof ~= 0 then
        return self.work_shop_foring[prof].item_id
    end
    return 0
end
function GuideData:GetArrowLevelMax()
    if self.all_guide.pet_skill and self.all_guide.pet_skill[1] then
        return self.all_guide.pet_skill[1].break_task_overlevel or 30
    end
end

function GuideData:GetArrowLevelMin()
    if self.all_guide.pet_skill and self.all_guide.pet_skill[1] then
        return self.all_guide.pet_skill[1].break_task_openlevel or 30
    end
end

function GuideData:GetTaskEffectLevel()
    if self.all_guide.pet_skill and self.all_guide.pet_skill[1] then
        return self.all_guide.pet_skill[1].task_effect_level or 0
    end
end

function GuideData:GetPetPieceId()
    if self.all_guide.pet_skill and self.all_guide.pet_skill[1] then
        return self.all_guide.pet_skill[1].pet_suipian_id or 0
    end
end

function GuideData:GetResolveItem()
    if self.all_guide.pet_skill and self.all_guide.pet_skill[1] then
        return self.all_guide.pet_skill[1].fj_item_id or 0
    end
end

function GuideData:GetSealNeedCard()
    if self.all_guide.pet_skill and self.all_guide.pet_skill[1] then
        return self.all_guide.pet_skill[1].yinka_fengyinka
    end
end

function GuideData:GetSaoDangFuBenId()
    if self.all_guide.pet_skill and self.all_guide.pet_skill[1] then
        return self.all_guide.pet_skill[1].saodang_fuben_id or 0
    end
end

function GuideData:GetTreasureHuntPetHuntItem()
    if self.all_guide.pet_skill and self.all_guide.pet_skill[1] then
        return self.all_guide.pet_skill[1].xunbao_key or 0
    end
end

function GuideData:GetTreasureHuntPartnerItem(times)
    if self.all_guide.pet_skill and self.all_guide.pet_skill[1] then
        return times == 1 and self.all_guide.pet_skill[1].comsume_item_id or self.all_guide.pet_skill[1].comsume_item_id_2
    end
end
function GuideData:TreasureItem()
    return self.all_guide.pet_skill[1].gaoji_cangbaotu
end

function GuideData:IllustrateItem()
    return self.all_guide.pet_skill[1].tujian_seq
end

function GuideData:GetAttrFlyLimitLevel()
    return self.all_guide.pet_skill[1].pro_att_fly
end
function GuideData:GetFormationPiaoChuangLevel()
    return self.all_guide.pet_skill[1].piaochuang
end

function GuideData:GetGuidePetId()
    return self.pet_skill.pet_id_1
end

function GuideData:GetActFormationGuidePetId()
    return self.pet_skill.act_formation_pet_id
end

function GuideData:GetAutoMainTask()
    if IS_AUDIT_VERSION and GLOBAL_CONFIG:Flags().use_auto_create then -- not GLOBAL_CONFIG:PkgInfo().is_youyan then
        return false
    end
    
    -- if GuideManager.Instance:IsGuide() then
    --     return false
    -- end
    return RoleData.Instance:GetRoleLevel() <= self.pet_skill.auto_task_level, self.pet_skill.wait_s
end
--换宠指引可触发次数
function GuideData:PetWarGuideTimes()
    return self.pet_skill.pet_war_guide
end
function GuideData:GetSkillId()
    local prof = RoleData.Instance:GetRoleProf()
    if prof ~= 0 then
        return self.skill_ids[prof].skill_id
    end
    return nil
end
function GuideData:GetSkillId2()
    local prof = RoleData.Instance:GetRoleProf()
    if prof ~= 0 then
        return self.skill_ids2[prof].skill_id
    end
    return nil
end
function GuideData:LearnSkillId()
    local prof = RoleData.Instance:GetRoleProf()
    if prof ~= 0 then
        return self.learn_skill[prof].skill_id
    end
    return nil
end
function GuideData:GetPetHitBookSkillItem()
    if self.all_guide.pet_skill and self.all_guide.pet_skill[1] then
        return self.all_guide.pet_skill[1].skill_item_id, self.all_guide.pet_skill[1].pet_skill_guide
    end
end

--获取法宝激活id
function GuideData:GetFabaoId()
   return self.pet_skill.talisman_id
end

--御三家id
function GuideData:CheckXiLianGuide()
    local list = PetData.Instance:GetPetList()
    for k,v in pairs(list) do
        for i,info in ipairs(self.choose_pet) do
            if info.show_pet == v:NormalId() then
                return false,k
            end
        end
    end
    return true
end

function GuideData:MPItemId()
    return self.pet_skill.huilan_id
end

function GuideData:AutoTalk()
    return self.pet_skill.auto_talk
end

--获取功能预告对应配置
function GuideData:GetFunctionNoticeData(mod_key)
    for k,v in pairs(Config.function_notice_auto.notice) do
        if v.modkey == mod_key then
            return v
        end
    end
end
function GuideData:GetNextNoticeData(notice_id)
    for k,v in pairs(Config.function_notice_auto.notice) do
        if v.notice_id == notice_id then
            return v
        end
    end
end
function GuideData:GetNextNoticeDataBySn(sn)
    return self.funopen_notice_sn_list[sn]
end

--获取当前的（未领取）的配置
-- 规则改变20230201
-- 引导任务+领取信息全部完成才会置灰
function GuideData:GetCurNoticeIndex()
    local check_list = {}
    for k,v in pairs( Config.function_notice_auto.notice) do
        if (v.res_id == nil or v.res_id == 0) and v.not_use == 0 then 
            table.insert(check_list, v)
        end
    end 
    
    table.sort(check_list,function (a,b)
        return a.end_level < b.end_level
    end)

    -- 可领取
    for i = 1,#check_list do 
        local cfg = check_list[i]
        local task_cfg = self.notice_mission_id_list[cfg.notice_id]
        if cfg.not_use == 0 and cfg.res_id == 0 and (self.notice_info:Val()[cfg.notice_id].status == 1 
            or (task_cfg ~= nil and self.notice_info:Val()[cfg.notice_id].task_status < 2 ) ) then 
            return i 
        end 
    end 

    -- 最新的未领取
    for i = 1,#check_list do 
        local cfg = check_list[i]
        local task_cfg = self.notice_mission_id_list[cfg.notice_id]
        if cfg.not_use == 0 and cfg.res_id == 0 and (self.notice_info:Val()[cfg.notice_id].status == 0
            or (task_cfg ~= nil and self.notice_info:Val()[cfg.notice_id].task_status == 0 ) ) then 
            return i 
        end 
    end 

    -- 自然沉底
    return #self.notice_info:Val() + 1
end
function GuideData:GetCurNoticeComplete()
    for i = 1,#self.notice_info:Val() do 
        if self.notice_info:Val()[i].status ~= 2 then 
            return false
        end 
    end 
    return true
end
function GuideData:CheckNext(notice_id)
    local next_cfg = GuideData.Instance:GetNextNoticeDataBySn(notice_id + 1)
    if next_cfg and next_cfg.not_use == 0 and next_cfg.res_id == 0 then
        local flag = GuideData.Instance:NoticeInfo(next_cfg.notice_id)
    else
        ViewMgr:CloseView(FunOpenNoticeView)
    end
end
function GuideData:SetNoticeInfo(params)
    self.notice_info:Set(params)
    self.notice_flush_info.flush = self.notice_flush_info.flush + 1
end
function GuideData:NoticeInfo(notice_id)
    return self.notice_info:Val()[notice_id].status or 0
end
function GuideData:NoticeGuideInfo(notice_id)
    return self.notice_info:Val()[notice_id].task_status or 0
end
function GuideData:NoticeGuideDone(notice_id)
    local s_flag =  GuideData.Instance:NoticeGuideInfo(notice_id) < FunPreNoticeEnum.Rewarded 
    local cfg = self.notice_mission_id_list[notice_id]
    return s_flag and cfg ~= nil
end
function GuideData:GetGuideFlag(flagType)
    return self.guide_flag[flagType] and self.guide_flag[flagType] or 1
end
function GuideData:CheckIsOpenActiveGuide()
    local level = RoleData.Instance:GetRoleLevel()
    local limit_level = ActiveData.Instance.other.remind_level
    if level > limit_level then
        return
    end
    local list = ActiveData.Instance:GetDailyList()
    local islock
    for index, value in ipairs(list) do
        if value.is_remind == 1 then
            islock = ActivityData.Instance:IsNoEnoughLevel(value.act_type,true)
            if not islock and ActivityData.Instance:GetActiveFlag(value.type) == 0 then
                MainViewData.Instance:ShowLeftButton()
                if UnityPlayerPrefs.GetInt(PrefKeys.ActiveGuide()) == 1 then
                    ViewMgr:OpenView(ActiveGuide)
                else
                    ViewMgr:OpenView(ActiveGuide,{step_id = 123})
                end
                break
            end
        end
    end
end
function GuideData:IsCanChangePet()
    local day = RoleData.Instance:CreatedDay()
    if day <= 7 then
        return true
    end
    local sum = 0
    for i = 1, 8, 1 do
        sum = sum + (self.change_pet_guide_trigger_times[i] or 0)
    end
    return sum < self:PetWarGuideTimes()
end
function GuideData:ChangePetCallTimes()
    local day = RoleData.Instance:CreatedDay()
    if day == 1 or day > 7 then
        return self:PetWarGuideTimes()
    end
    return 1
end
function GuideData:ChangePetTimesToday()
    local day = RoleData.Instance:CreatedDay()
    local index = day
    if index > 7 then
        index = 8
    end
    return self.change_pet_guide_trigger_times[index] or 0
end
function GuideData:IsCanCallBattleGuide()
    return BattleData.Instance:BaseData().round_phase ~= BATTLE_ROUND_PHASE.Client and SceneCtrl.Instance:CurScene():GetClassType() == BattleScene
end

function GuideData:FinishTaskGuide(task_id)
    return self.finish_task_guides[task_id]
end

function GuideData:StartRollingTask()
    self.rolling_task_box.start = true
end
function GuideData:StopRollingTask()
    self.rolling_task_box.start = false
end

function GuideData:TaskBoxGuide()
    return UnityPlayerPrefs.GetInt(PrefKeys.TaskBoxGuide()) == 0
end
function GuideData:StopTaskBoxGuide()
    UnityPlayerPrefs.SetInt(PrefKeys.TaskBoxGuide(), 1)
end
function GuideData:NewModeGuide(id)
    if RoleSettingData.IsNewMode() then
        if self.new_mode_guide[id] then
            return self.new_mode_guide[id].new_guide_id
        end
    end
    return id
end
function GuideData:GetFunopenNoticeGuideUnitParam(notice_id)
    local info = self.notice_info:Val()[notice_id]
    local cfg = self.notice_mission_id_list[notice_id]

    if cfg == nil then
        return {
            is_empty = true,
            done = true,
            complete = true,
        }
    end

    local rewards = BagData.Instance:ItemListBuild(cfg.task_reward or {})

    if cfg.task_lingyu > 0 then 
        local info = {
            item_id =  CommonItemId.ImmortalCoin,
            num = cfg.task_lingyu,
            is_bind = 0,
        }
        
        table.insert(rewards,Item.Create(info))
    end 

    return {
        done = info.task_status == 2,
        complete = info.task_status == 1,
        reward_list = rewards,
        task_total = cfg.param,
        task_desc = cfg.mission_desc,
        task_param = info.task_process,
        mod_key = cfg.modkey,
        notice_id = cfg.notice_id,
    }
end