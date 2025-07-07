FILE_REQ("modules/live/live_data")

VIEW_REQ("modules/live/live_view")
VIEW_REQ("modules/live/live_refine_view")
VIEW_REQ("modules/live/live_quickbuy")
VIEW_REQ("modules/live/live_up_level_view")
VIEW_REQ("modules/live/prop_item_use_dialog")
LiveCtrl = LiveCtrl or BaseClass(BaseCtrl)
LiveCtrl.LIVE_SKILL_OP_TYPE = {
    SKILL_LIFE_ALL_INFO = 0,		--!< 请求生活技能全部信息
    SKILl_LIFE_LEVEL_UP = 1,		--!< 请求升级生活技能 P1:skill_type
    MAKE_ITEM = 2,                  -- 请求制作道具 p1:sn, p2(0,1)有无使用辅助工具, p3:count p4:是否成功
    PROPS_INFO = 3,                 -- 请求生活食物类型信息
    USE_CARD = 4,                   -- 请求是否使用变身卡形象 p1(0:不使用，1使用)
    CHANGE_ROLE = 5,                -- 请求幻化 p1:seq
    UN_CHANGE_ROLE = 6,             -- 请求解除幻化          
}

LiveCtrl.LIVE_SKILL_TYPE = {
    PICK_FLOWER = 0,    -- 采摘
    FISHING = 1,        -- 钓鱼
    LUMBERING = 2,      -- 砍伐
    DIG = 3,            -- 挖矿
    MEDICINE = 4,       -- 药师
    COOK = 5,           -- 厨师
}

-- 生活锁定未开放的技能
LiveCtrl.LOCK_SKILL_TYPE = {
    [LiveCtrl.LIVE_SKILL_TYPE.LUMBERING] = true,
}

LiveCtrl.PROP_ITEM_TYPE = {
    CARD = 0,
    FOOD_1 = 1,
    FOOD_2 = 2,
    FOOD_3 = 3,
    FOOD_4 = 4,
    FOOD_5 = 5,
    FOOD_6 = 6,
}
LiveCtrl.TASK_TYPE = {
    CARD = 1,   -- 变身卡任务
    FOOD = 2,   -- 美食任务
    MAX = 2,
}

LiveCtrl.CurrItemId = 24391
        
function LiveCtrl:__init()
    if LiveCtrl.Instance ~= nil then
		Debuger.LogError("[LiveCtrl] attempt to create singleton twice!")
		return
	end
	LiveCtrl.Instance = self
    self.data = LiveData.New()
	self.data = LiveData.Instance
end

function LiveCtrl:__delete()
    
end

function LiveCtrl:OnInit()
    self:RegisterAllProtocol()
    RemindCtrl.Instance:Remind():Register(
        Mod.Live.Main,
        self.data.gather_energy,
        function ()
            return self.data:GetRemind() or 0
        end
    )
end

function LiveCtrl:OnUnloadGameLogic()
    self.data.gather_energy.is_remind = true
    self.data.battled_unfix_card = true
end

function LiveCtrl:RegisterAllProtocol()
    self:RegisterProtocol(CSLiveSkillReq)
    self:RegisterProtocol(SCLiveSkillAllInfo, "OnSCLiveSkillAllInfo")
    self:RegisterProtocol(SCLiveSkillInfo, "OnSCLiveSkillInfo")
    self:RegisterProtocol(SCLivePropsAllInfo, "OnSCLivePropsAllInfo")
    self:RegisterProtocol(SCLivePropsInfo, "OnSCLivePropsInfo")
    self:RegisterProtocol(SCLiveCardInfo, "OnSCLiveCardInfo")
    self:RegisterProtocol(SCLifeSkillRecipeInfo, "OnLifeSkillRecipeInfo")
end

-- 请求生活技能操作，op_type : LiveCtrl.LIVE_SKILL_OP_TYPE
function LiveCtrl:SendSkillReq(op_type, p1, p2, p3,p4)
    local protocol = GetProtocol(CSLiveSkillReq)
    protocol.op_type = op_type or 0
    protocol.p1 = p1 or 0
    protocol.p2 = p2 or 0
    protocol.p3 = p3 or 0
    protocol.p4 = p4 or 0
    SendProtocol(protocol)
end

-- 生活技能全部信息
function LiveCtrl:OnSCLiveSkillAllInfo(protocol)
    self.data.skill_all_info:Set(protocol.info_list)
end

-- 生活技能单个信息
function LiveCtrl:OnSCLiveSkillInfo(protocol)
    local skill_type = protocol.skill_type
    self.data.skill_all_info[skill_type] = protocol.skill_info
end

-- 生活食物类型信息
function LiveCtrl:OnSCLivePropsAllInfo(protocol)
    self.data.prop_use_all_info:Set(protocol.info_list)
    self.data.card_use_all_info:Set(protocol.card_list)
    --self:CheckPropList()      -- 屏蔽任务显示
end

-- 生活食物类型信息
function LiveCtrl:OnSCLivePropsInfo(protocol)
    local prop_type = protocol.prop_type
    if prop_type == LiveCtrl.PROP_ITEM_TYPE.CARD then
        if MainViewData.Instance:IsBattle() then
            PublicPopupCtrl.Instance:Center(ErrorText[110])
        end

        local seq = protocol.prop_info.seq
        local info = {seq = seq, point_num = protocol.param1}
        self.data.card_use_all_info[seq] = info

        -- 获取新技能
        if seq > 0 then
            local last_skill_id = LiveData.Instance:GetCardSkillId()
            local cfg = LiveData.Instance:GetPropCfg(seq)
            if cfg and cfg.skill_id > 0 and cfg.skill_id ~= last_skill_id then
                LiveData.Instance:SetCardSkillId(cfg.skill_id)
                if protocol.is_noitce ~= 0 then
                    Invoke(function ()
                        SkillData.Instance:PushNewSkillPopup({skill_id = cfg.skill_id,skill_level = 1})
                        ViewMgr:OpenView(NewSkillPopupView)
                    end, 1.5)
                end
            end
        else
            local skill_id = LiveData.Instance:GetCardSkillId()
            if skill_id > 0 then
                Invoke(function ()
                    SkillData.Instance:PushNewSkillPopup({skill_id = skill_id,skill_level = 1, title_text = Language.NewSkillPopup.LoseSkill})
                    ViewMgr:OpenView(NewSkillPopupView)
                end, 1.5)
                LiveData.Instance:SetCardSkillId(0)
            end
        end
    end
    self.data.prop_use_all_info[prop_type] = protocol.prop_info
    
    --self:CheckPropList()      -- 屏蔽任务显示
end

function LiveCtrl:OnLifeSkillRecipeInfo(protocol)
    self.data:OnLifeSkillRecipeInfo(protocol)
end


-- 战斗中使用幻化技能的幻化点变化
function LiveCtrl:OnSCLiveCardInfo(protocol)
    --LogError("战斗中使用幻化技能的幻化点变化",protocol)
    local info = {seq = protocol.seq, point_num = protocol.point_num}
    self.data.card_use_all_info[protocol.seq] = info
end

-- 检查生效的道具
function LiveCtrl:CheckPropList()
    local server_time = TimeCtrl.Instance:GetServerTime()
    local can_task = {}
    -- 时间到期的筛选掉
    for i,v in pairs(self.data.prop_use_all_info) do
        if not TableIsEmpty(v) and v.seq ~= 0 then
            local task_type
            if i == LiveCtrl.PROP_ITEM_TYPE.CARD then
                task_type = LiveCtrl.TASK_TYPE.CARD
            else
                task_type = LiveCtrl.TASK_TYPE.FOOD
            end
            can_task[task_type] = v
        end
    end
    local show_attri_btn = false
    for i = 1, LiveCtrl.TASK_TYPE.MAX do
        if can_task[i] then
            local task = LivePropTask.New()
            task = task:TaskInfo()
            local vo = CommonStruct.CustomTask()
            if i == LiveCtrl.TASK_TYPE.CARD then
                local cfg = self.data:GetPropCfg(can_task[i].seq)
                vo.task_name = string.format(Language.Live.Ctrl.Txt1, cfg.name)
            else
                vo.task_name = string.format(Language.Live.Ctrl.Txt2)
            end
            vo.task_id = CUSTOM_OBJID.LIVE_PROP + i
            vo.end_timestamp = can_task[i].end_timestamp
            task:TaskVo(vo)
            if not show_attri_btn then
                task.show_attri_btn = true
                show_attri_btn = true
            end
            task.live_task_type = i
            local smart_task = SmartData.New(task)
            TaskData.Instance:TaskInfoUpdateByTaskId(vo.task_id, smart_task)
            self.data.task_list[i] = smart_task
        elseif self.data.task_list[i] then  -- 有失效的任务清掉
            TaskData.Instance:TaskInfoDeleteByTaskId(self.data.task_list[i].task_id)
            self.data.task_list[i] = nil
        end
    end
    self:FlushTask()
end

-- 刷新生活任务
function LiveCtrl:FlushTask()
    if not TableIsEmpty(self.data.task_list) then
        TimeHelper:CancelTimer(self.time_ht)
        self.time_ht = TimeHelper:AddRunTimer(function ()
            for i,v in pairs(self.data.task_list) do
                v.remind = not v.remind
            end
        end,1)
    else
        TimeHelper:CancelTimer(self.time_ht)
    end
end

-- 是否使用变身卡形象
function LiveCtrl:UseCardModel(is_use)
    local p1 = is_use and 1 or 0
    self:SendSkillReq(LiveCtrl.LIVE_SKILL_OP_TYPE.USE_CARD, p1)
end

function LiveCtrl:UseFood(item)
    local use_func = function (item)
        local item_param_t = {item_id = item.item_id,column = item.column_type,index = item.index,num = 1}
        BagCtrl.Instance:SendUseItem(item_param_t)
    end
    local add_cfg = self.data:GetAddCfgByItemId(item.item_id)
    if add_cfg then
        local add_info = self.data.prop_use_all_info[add_cfg.type] 
        if add_info and add_info.seq > 0 then
            PublicPopupCtrl.Instance:AlertTip(Language.Live.Tip11,function ()
                use_func(item)
            end)
        else
            use_func(item)
        end
    end
end

-- 使用变身卡
function LiveCtrl:UseCard(item, num)
    -- num = num or 1

    local cancel_func = function ()
        ViewMgr:CloseView(DialogTipsView)
        -- 遗迹夺宝期间不允许使用变身卡
        if TombRaiderData.Instance:InTombRaider() then 
            PublicPopupCtrl.Instance:Center(Language.TombRaider.CardCancel)
            return 
        end 
        self:UseFunc(item)
    end

    local ok_func = function ()
        self.is_use_card_model = 1
        ViewMgr:CloseView(DialogTipsView)
        -- 遗迹夺宝期间不允许使用变身卡
        if TombRaiderData.Instance:InTombRaider() then 
            PublicPopupCtrl.Instance:Center(Language.TombRaider.CardCancel)
            return 
        end 
        self:UseFunc(item)
    end

    local param_t = {
        content = Language.Live.Ctrl.Txt3,
        cancel = {
            name = Language.Live.Ctrl.Txt4,
            func = cancel_func
        },
        confirm = {
            func =  ok_func
        },
        close = true
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)

    -- if item then
    --     local item_param_t = {item_id = item.item_id,column = item.column_type,index = item.index,num = num}
    --     BagCtrl.Instance:SendUseItem(item_param_t)
    -- end
end

function LiveCtrl:UseFunc(item)
    ViewMgr:CloseView(BagView)
    self.card_handle = SE:Play(SceneEffectConfig.EffectList.LiveModelCard, nil, nil, MainPlayer.root_node)
    Invoke(function ()
        self:UseCardModel(self.is_use_card_model == 1)
        SE:Stop(self.card_handle)
        if item then
            local item_param_t = {item_id = item.item_id,column = item.column_type,index = item.index,num = 1}
            BagCtrl.Instance:SendUseItem(item_param_t)
            self.is_use_card_model = 0
        end
    end,1)
end

--时间格式化静态工具(没有合适的可以手动添加) 天/时/分/秒
function LiveCtrl:FormatDHMS(time)
    local t = {hour = 0, min = 0, s = 0}
    if time > 0 then
        local day = math.floor(time / 86400)
        t.hour = math.floor((time / 3600) % 24) + 0 * day
        if t.hour < 10 then
            t.hour = "0" .. t.hour
        end
        t.min = math.floor((time / 60) % 60)
        if t.min < 10 then
            t.min = "0" .. t.min
        end
        t.s = math.floor(time % 60)
        if t.s < 10 then
            t.s = "0" .. t.s
        end
    end
    return t
end

-- 活力不足提醒
function LiveCtrl:CheckEnergy(num)
    num = num or 0
    local has_num = FightData.Instance.gather_energy.val or 0
    if has_num < num then
        PublicPopupCtrl.Instance:Center(Language.Live.Ctrl.Txt5)
        ViewMgr:OpenView(ItemConsumeTipView,{item_id = LiveCtrl.CurrItemId})
        return false
    else
        return true
    end
end

-- 炼制厨具
function LiveCtrl:UseRefine(skill_type, sn, count)
    local npc_id
    local gather_type
    local effect
    if skill_type == LiveCtrl.LIVE_SKILL_TYPE.MEDICINE then
        npc_id = self.data.live_cfg.other[1].npc_danlu
        gather_type = 9
        effect = SceneEffectConfig.EffectList.LiveStove
    else
        npc_id = self.data.live_cfg.other[1].npc_pengren
        gather_type = 10
        effect = SceneEffectConfig.EffectList.LiveKitchen
    end
    local npc_vo = VoMgr:NpcVoBySeq(npc_id)
    local pos = SceneLogic.Instance:ObjectPos(npc_vo)
    self.card_handle = SE:Play(effect, pos)
    GatherData.Instance:SetGatherInfo({time = 5, desc = gather_type})
    ViewMgr:OpenView(GatherView)
    self.data:SetIsRefine(true)
    Invoke(function ()
        self:SendSkillReq(LiveCtrl.LIVE_SKILL_OP_TYPE.MAKE_ITEM, sn, 1, count)
        LiveData.Instance.refine_count = count
        SE:Stop(self.card_handle)
        self.data:SetIsRefine(false)
    end,5)
end

-- 检查是否需要解除幻化卡
function LiveCtrl:CheckUnficCard()
    if self.data.battled_unfix_card then
        self:SendSkillReq(LiveCtrl.LIVE_SKILL_OP_TYPE.UN_CHANGE_ROLE)
    end
end