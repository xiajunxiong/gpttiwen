local Lang = Language.ShenYuProbe

ShenYuProbeView = ShenYuProbeView or DeclareView("ShenYuProbeView", "shen_yu_probe/shen_yu_probe", Mod.ShenYuProbe.Main)
-- 标签类型
ShenYuProbeView.TabType = {
    Level = 0,      -- 探索等级
    SYTask = 1,     -- 神域任务
    CampTask = 2,   -- 阵营任务
    RoleTask = 3,   -- 个人任务
}
function ShenYuProbeView:ShenYuProbeView()
    self.data = ShenYuProbeData.Instance
end

function ShenYuProbeView:LoadCallback(param_t)
    self:SelTab(ShenYuProbeView.TabType.Level)
    self.smart_ht = self.data.remind_data:Care(BindTool.Bind(self.FlushRemind, self))
    self:FlushRemind()
    if TableIsEmpty(self.data.role_data) then
        LogError("活动未开启就打开了界面")
        ViewMgr:CloseView(ShenYuProbeView)
    end
end

function ShenYuProbeView:CloseCallback()
    self.data.remind_data:Uncare(self.smart_ht)
end

function ShenYuProbeView:FlushRemind()
    for i = 1, self.RedPointList:Length() do
        local red_num = self.data:GetTabRemind(i - 1)
        self.RedPointList[i]:SetNum(red_num)
    end
end

function ShenYuProbeView:OnCloseClick()
    ViewMgr:CloseView(ShenYuProbeView)
end

function ShenYuProbeView:SelTab(tab_type)
    local index = tab_type + 1
    self.TabList[index].isOn = true
end

function ShenYuProbeView:OnTabClick(tab_type)
    self.tab_type = tab_type
    self.data:SetTabType(tab_type)
end

function ShenYuProbeView:OnTaskItemClick(data)
    local tab_type = data.seq or ShenYuProbeView.TabType.Level
    self:SelTab(tab_type)
end

function ShenYuProbeView:OnTipClick()
    if self.tab_type == ShenYuProbeView.TabType.CampTask then
        PublicPopupCtrl.Instance:HelpTip(381)
    elseif self.tab_type == ShenYuProbeView.TabType.SYTask then
        PublicPopupCtrl.Instance:HelpTip(382)
    elseif self.tab_type == ShenYuProbeView.TabType.RoleTask then
        PublicPopupCtrl.Instance:HelpTip(390)
    else
        PublicPopupCtrl.Instance:HelpTip(380)
    end
end


ShenYuProbeLevelPanel = ShenYuProbeLevelPanel or DeclareMono("ShenYuProbeLevelPanel", UIWFlushPanel)
function ShenYuProbeLevelPanel:ShenYuProbeLevelPanel()
    self.data = ShenYuProbeData.Instance
    self.data_cares = {
        {data = self.data.camp_data, func = self.FlushPanel, init_call = false},
        {data = self.data.role_data, func = self.FlushPanel, init_call = false},
    }
end

function ShenYuProbeLevelPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function ShenYuProbeLevelPanel:FlushPanel()
    local camp_data = self.data.camp_data
    local level = camp_data.level or 0
    local exp = camp_data.exp or 0
    local max_level = self.data:GetMaxLevel()
    local is_max = level >= max_level
    local level_cfg = self.data:GetLevelCfg(level)
    local next_level_cfg = level_cfg
    if not is_max then
        next_level_cfg = self.data:GetLevelCfg(level + 1)
    end

    UH.SetText(self.Level, level)
    self.Exp:SetObjActive(not is_max)
    self.MaxFlag:SetActive(is_max)
    if not is_max then
        UH.SetText(self.Exp, string.format("%s/%s", exp, next_level_cfg.experience))
    end

    local list_data = self.data.task_cfg
    self.TaskList:SetData(list_data)

    self.RedPoint:SetNum(self.data:GetAllLevelReawrdRemind())
end

function ShenYuProbeLevelPanel:OnBoxClick()
    ViewMgr:OpenView(ShenYuProbeLevelReward)
end

ShenYuProbeTaskItem = ShenYuProbeTaskItem or DeclareMono("ShenYuProbeTaskItem", UIWidgetBaseItem)
function ShenYuProbeTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local task_type = data.way_type
    local task_times = ShenYuProbeData.Instance:GetTaskTimes(task_type)
    UH.SpriteName(self.BG, "TaskBg" .. task_type)
    UH.SetText(self.Title, string.format("%s：%s",Language.ShenYuProbe.TaskType[task_type], data.act_title))
    UH.SetText(self.Count, string.format("(%s/%s)", task_times, data.task_number_max))

    local flush_time = ShenYuProbeData.Instance:GetTaskFlushTime(task_type)
    self.Time:StampTime(flush_time,TimeType.Type_Time_7)
end



-- 任务详细信息
ShenYuProbeTaskPanel = ShenYuProbeTaskPanel or DeclareMono("ShenYuProbeTaskPanel", UIWFlushPanel)
function ShenYuProbeTaskPanel:ShenYuProbeTaskPanel()
    self.data = ShenYuProbeData.Instance
    self.data_cares = {
        {data = self.data.tab_data, func = self.FlushPanel, init_call = false},
        {data = self.data.role_data, func = self.FlushPanel, init_call = false},
    }
    if self.ui_obj1 == nil then
		self.ui_obj1 = UIObjDrawer.New()
        self.ModelShow1:SetShowData(
			{
				ui_obj = self.ui_obj1,
                view = self,
                show_shadow = true,
                not_show_anim = true,
			}
		)
    end
    if self.ui_obj2 == nil then
		self.ui_obj2 = UIObjDrawer.New()
        self.ModelShow2:SetShowData(
			{
				ui_obj = self.ui_obj2,
                view = self,
                show_shadow = true,
                not_show_anim = true,
			}
		)
    end
end

function ShenYuProbeTaskPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Dialog:SetActive(false)
    self:FlushPanel()
end

function ShenYuProbeTaskPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj1 ~= nil then
		self.ui_obj1:DeleteMe()
    end
    if self.ui_obj2 ~= nil then
		self.ui_obj2:DeleteMe()
	end
end

function ShenYuProbeTaskPanel:FlushPanel()
    self:FlushInfo()
    self:FlushModel()
end

function ShenYuProbeTaskPanel:FlushInfo()
    local tab_type = self.data:GetTabType() or ShenYuProbeView.TabType.Level
    if tab_type == ShenYuProbeView.TabType.Level then
        return
    end
    local cfg = self.data:GetTaskCfg(tab_type)
    local task_type = cfg.way_type
    self.task_type = task_type
    local task_times = ShenYuProbeData.Instance:GetTaskTimes(task_type)
    UH.SetText(self.TaskName, cfg.act_title)
    UH.SetText(self.Count, string.format("(%s/%s)", task_times, cfg.task_number_max))
    local flush_time = ShenYuProbeData.Instance:GetTaskFlushTime(task_type)
    self.Time:StampTime(flush_time,TimeType.Type_Time_7)

    local role_t_num = self.data:GetRoleTargetNum(task_type)
    local sy_t_num = self.data:GetSYTargetNum(task_type)
    local desc
    if TableIsEmpty(cfg) then
        return
    end
    if task_type == ShenYuProbeCtrl.TaskType.Role then
        local value = role_t_num >= cfg.personal_target_number and cfg.personal_target_number or role_t_num
        desc = string.format(cfg.way_explain_1, value, cfg.personal_target_number)
    else
        local value1 = sy_t_num >= cfg.camp_target_number and cfg.camp_target_number or sy_t_num
        local value2 = role_t_num >= cfg.personal_target_number and cfg.personal_target_number or role_t_num
        desc = string.format(cfg.way_explain_1, value1,cfg.camp_target_number, value2, cfg.personal_target_number)
    end
    local tip = ColorStr(Lang.Text11, COLORSTR.Yellow1)
    desc = desc .. "\n" .. tip
    UH.SetText(self.Desc, desc)
    local flag = self.data:GetTaskRewardFlag(task_type)
    UH.SetInter(self.Inter, flag ~= -1)
    local btn_text = flag > 0 and Lang.Text12 or Lang.Text13
    if flag == -1 then
        btn_text = Lang.Text14
    end
    UH.SetText(self.BtnText, btn_text)
    self.RedPoint:SetNum(flag)
end

function ShenYuProbeTaskPanel:FlushModel()
    local tab_type = self.data:GetTabType() or ShenYuProbeView.TabType.Level
    if tab_type == ShenYuProbeView.TabType.Level then
        return
    end
    local cfg = self.data:GetTaskCfg(tab_type)
    local folder = cfg.route
    local res_ids = Split(cfg.ui_model, "|")
    local res_path = string.format("%s/%s.prefab", folder, res_ids[1])
    self.ui_obj1:SetData(res_path)
    self.ModelShow2:SetObjActive(res_ids[2] ~= nil)
    if res_ids[2] then
        res_path = string.format("%s/%s.prefab", folder, res_ids[2])
        self.ui_obj2:SetData(res_path)
    end

    local huobi_list = {}
    if cfg.military_exploits > 0 then
        local huobi = HuoBi[CurrencyType.GodSpaceExploits]
        table.insert(huobi_list, Item.Create({item_id = huobi, num = cfg.military_exploits}))
    end
    if cfg.quintessence > 0 then
        local huobi = HuoBi[CurrencyType.GodSpaceEssence]
        table.insert(huobi_list, Item.Create({item_id = huobi, num = cfg.quintessence}))
    end
    local reward_list = DataHelper.FormatItemList(cfg.task_reward)
    reward_list = AndList(huobi_list, reward_list)
    self.RewardList:SetData(reward_list)
end

function ShenYuProbeTaskPanel:OnGotoClick()

    local flag = self.data:GetTaskRewardFlag(self.task_type)
    if flag == 1 then
        -- 可领取情况
        ShenYuProbeCtrl.Instance:SendTemplateReq({
            oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_FETCH_EXPLORE_TASK_REWARD,
            param_1 = self.task_type,
        })
        return
    end

    local tab_type = self.data:GetTabType() or ShenYuProbeView.TabType.Level
    if tab_type == ShenYuProbeView.TabType.Level then
        return
    end
    self.Dialog:SetActive(true)
    if tab_type == ShenYuProbeView.TabType.SYTask then
        local list_data = {}
        for i = 1, ShenYuProbeCtrl.MaxBoss do
            local boss_id = i - 1
            local item = {boss_id = boss_id}
            item.l_text = Lang.ObjName3 .. i
            local is_die = self.data:GetBossIsDie(boss_id)
            if is_die then
                item.r_text = ColorStr(Lang.Text15, COLORSTR.Red8)
            elseif self.data:GetBossIsCanUpFight(boss_id) then
                item.r_text = Lang.Text16
            end
            local boss_data = self.data:GetBossData(boss_id)
            item.pos = {x = boss_data.row, y = boss_data.col}
            item.click_func = function (data)
                boss_id = data.boss_id
                if self.data:GetBossIsDie(boss_id) then
                    PublicPopupCtrl.Instance:Center(Lang.Tip3)
                    return
                end
                -- 前往Boss
                self:OnGoToEvent(data.pos)
                ViewMgr:CloseView(ShenYuProbeView)
            end
            table.insert(list_data, item)
        end
        self.DialogList:SetData(list_data)
    elseif tab_type == ShenYuProbeView.TabType.RoleTask then
        local list_data = {}
        local click_func = function (data)
            self:OnGoToEvent(data.pos)
            ViewMgr:CloseView(ShenYuProbeView)
        end
        for i,v in pairs(self.data.role_data.stones) do
            local item = {}
            item.l_text = Lang.ObjName1 .. i
            item.r_text = Lang.Text17
            item.click_func = click_func
            item.pos = {x = v.row, y = v.col}
            table.insert(list_data, item)
        end

        for i,v in pairs(self.data.role_data.grasses) do
            local item = {}
            item.l_text = Lang.ObjName2 .. i
            item.r_text = Lang.Text17
            item.click_func = click_func
            item.pos = {x = v.row, y = v.col}
            table.insert(list_data, item)
        end

        --个人任务不显示弹窗
        --self.DialogList:SetData(list_data)
        self.Dialog:SetActive(false)
        if not TableIsEmpty(list_data) then
            local index = GameMath.Ramdon(1,#list_data)
            click_func(list_data[index])
        end
    elseif tab_type == ShenYuProbeView.TabType.CampTask then
        local list_data = {}
        local click_func = function (data)
            self:OnGoToEvent(data.pos)
            ViewMgr:CloseView(ShenYuProbeView)
        end
        for i,v in pairs(self.data.well_pos_data) do
            local item = {}
            local index = v.well_type + 1
            item.l_text = Lang.ObjName4 .. index
            item.r_text = v.seal_count == v.need_count and "" or Lang.Text18
            item.click_func = click_func
            item.pos = {x = v.row, y = v.col}
            table.insert(list_data, item)
        end
        self.DialogList:SetData(list_data)
    end
end

-- link_mark神域格子的唯一key
function ShenYuProbeTaskPanel:OnGoToEvent(pos)
    if not pos.x or not pos.y then
        LogDG("前往目标位置有问题")
        return
    end
    LandsBetweenData.Instance:SetMapClick(pos)
    --LandsBetweenData.Instance:SetMapJumpClick(pos)
end

function ShenYuProbeTaskPanel:OnCloseDialogClick()
    self.Dialog:SetActive(false)
end


ShenYuProbeTaskDialogItem = ShenYuProbeTaskDialogItem or DeclareMono("ShenYuProbeTaskDialogItem", UIWidgetBaseItem)
function ShenYuProbeTaskDialogItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.name_click_func = data.click_func
    UH.SetText(self.LText, data.l_text or "")
    UH.SetText(self.RText, data.r_text or "")
end

function ShenYuProbeTaskDialogItem:OnNameClick()
    if self.name_click_func then
        self.name_click_func(self.data)
    end
end





-- 神域地图场景对象的ui挂件 shen_yu_probe_sub
ShenYuProbeObjUI = ShenYuProbeObjUI or DeclareMono("ShenYuProbeObjUI", UIWFlushPanel)
function ShenYuProbeObjUI:ShenYuProbeObjUI()
    self.data = LandsBetweenData.Instance
end

-- 
function ShenYuProbeObjUI:SetLinkMark(link_mark)
    --LOG("ShenYuProbeObjUI SetLink",link_mark)
    local param = self.data:GetResOfSpPicByLinkMark(link_mark) or {}
    local task_type = param.task_type
    self.Gather:SetObjActive(task_type == ShenYuProbeCtrl.TaskType.Role)
    self.Boss:SetObjActive(task_type == ShenYuProbeCtrl.TaskType.ShenYu)
    self.Well:SetObjActive(task_type == ShenYuProbeCtrl.TaskType.Camp)
    if task_type == ShenYuProbeCtrl.TaskType.Role then
        self.Gather:SetParam(param)
    elseif task_type == ShenYuProbeCtrl.TaskType.ShenYu then
        self.Boss:SetParam(param)
    elseif task_type == ShenYuProbeCtrl.TaskType.Camp then
        self.Well:SetParam(param)
    end
end



-- 神域探索采集
ShenYuProbeObjUIGather = ShenYuProbeObjUIGather or DeclareMono("ShenYuProbeObjUIGather", UIWFlushPanel)
function ShenYuProbeObjUIGather:ShenYuProbeObjUIGather()
    self.data = ShenYuProbeData.Instance
    self.data_cares = {
        {data = self.data.role_data, func = self.FlushShow, init_call = false},
        {data = LandsBetweenData.Instance.operating_info, func = self.FlushShow, init_call = true}
    }
    self.is_active = false
end

function ShenYuProbeObjUIGather:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.gatheing = false   -- 是否采集中
    self.progress_time = 0
    self.gather_time = 5
    self.is_active = true
    self:FlushShow()
end

function ShenYuProbeObjUIGather:OnDisable()
    self.is_active = false
end

function ShenYuProbeObjUIGather:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_ht)
end

function ShenYuProbeObjUIGather:SetParam(param)
    self.param = param
    local link_mark = param.link_mark
    self.link_mark = link_mark
    local pos = Split(link_mark, "|")
    UH.SetText(self.Name, string.format("%s(%s,%s)", self.param.name, pos[1],pos[2]))
    self:FlushShow()
end

function ShenYuProbeObjUIGather:FlushShow()
    if not self.is_active then
        return
    end
    --LogDG("FlushShow",self.gatheing)
    local oper_link_mark = "0"
    if LandsBetweenData.Instance.operating_info.obj ~= nil then 
        local map_pic = LandsBetweenData.Instance:GetMapPicClick()
        oper_link_mark = map_pic.static.link_mark
    end 
    
    self.Ing:SetActive(self.gatheing)
    self.Ctrl:SetActive(not self.gatheing and self.link_mark == oper_link_mark)
    self.ProgressText:SetObjActive(self.gatheing or self.link_mark ~= oper_link_mark)
    self:FlushInfo()
end

function ShenYuProbeObjUIGather:FlushInfo()
    local cfg = self.data:GetTaskCfg(ShenYuProbeView.TabType.RoleTask)
    local role_t_num = self.data:GetRoleTargetNum(cfg.way_type)
    UH.SetText(self.ProgressText, string.format(Lang.Text19, role_t_num,cfg.personal_target_number))
end

function ShenYuProbeObjUIGather:FlushProgress()
    if not self.gatheing then
        return
    end
    local time = self.progress_time
    if time >= self.gather_time then
        time = self.gather_time
        TimeHelper:CancelTimer(self.time_ht)
        --self.Ing:SetActive(false)
        --self.Ctrl:SetActive(false)
        --发送采集请求
        ShenYuProbeCtrl.Instance:SendTemplateReq({
            oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_EXPLORE_GATHER,
            param_1 = self.param.server_type,
            param_2 = self.param.row,
            param_3 = self.param.col,
        })
        local link_mark = self.param.link_mark
        ShenYuProbeCtrl.ClearObj(link_mark, true)
        self.progress_time = 0
        self.gatheing = false
    end
    self.ProgressBar:SetProgress(time / self.gather_time)
    -- local int_time = math.floor(time)
    -- UH.SetText(self.ProgressText, string.format("采集进度：%s/%s", int_time,self.gather_time))
end

function ShenYuProbeObjUIGather:OnBtnClick()
    if self.gatheing then
        return
    end

    self.gatheing = true
    self:FlushShow()

    TimeHelper:CancelTimer(self.time_ht)
    local interval = 0.1
    self:FlushProgress()
    self.time_ht = TimeHelper:AddRunTimer(function()
        self.progress_time = self.progress_time + interval
        self:FlushProgress()
    end,interval)

    LandsBetweenData.Instance:ForceCloseOperate()
end



-- 神域探索bossUI
ShenYuProbeObjUIBoss = ShenYuProbeObjUIBoss or DeclareMono("ShenYuProbeObjUIBoss", UIWFlushPanel)
function ShenYuProbeObjUIBoss:ShenYuProbeObjUIBoss()
    self.data = ShenYuProbeData.Instance
    self.data_cares = {
        {data = self.data.boss_data, func = self.FlushInfo, init_call = true},
        {data = LandsBetweenData.Instance.operating_info, func = self.FlushInfo, keys = {"map_pic"}, init_call = true},
    }
    self.is_active = false
end

function ShenYuProbeObjUIBoss:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_ht)
end

function ShenYuProbeObjUIBoss:OnEnable()
    self.is_active = true
end

function ShenYuProbeObjUIBoss:OnDisable()
    self.is_active = false
end

function ShenYuProbeObjUIBoss:SetParam(param)
    self.param = param
    self:FlushInfo()
end

function ShenYuProbeObjUIBoss:FlushInfo()
    if TableIsEmpty(self.param) then
        return
    end

    if not self.is_active then
        return
    end

    local link_mark = self.param.link_mark
    local oper_link_mark = "0"
    if LandsBetweenData.Instance.operating_info.obj ~= nil then 
        local map_pic = LandsBetweenData.Instance:GetMapPicClick()
        oper_link_mark = map_pic.static.link_mark
    end 

    local boss_id = self.param.id
    local boss_data = self.data:GetBossData(boss_id)
    if TableIsEmpty(boss_data) then
        LOG("bossData为空", boss_id)
        LOG("boss",self.data.boss_data)
        return
    end

    self.ProgressBar:SetObjActive(oper_link_mark ~= link_mark)
    self.Btn:SetActive(oper_link_mark == link_mark)

    local progress_num = boss_data.boss_remain_hp / boss_data.boss_max_hp
    self.ProgressBar:SetProgress(progress_num)
    local pos = Split(link_mark, "|")
    UH.SetText(self.Name, string.format("%s(%s,%s)", self.param.name, pos[1],pos[2]))

    if self.last_remain_hp and boss_data.boss_remain_hp ~= self.last_remain_hp then
        local harm = boss_data.boss_remain_hp - self.last_remain_hp
        local hp_str = self:PackContent(harm)
        self.TextHandler:ShowContent(hp_str)
        self.TextHandlerAnim:SetObjActive(true)
        self.TextHandlerAnim:Play()
        TimeHelper:CancelTimer(self.time_ht)
        self.time_ht = Invoke(function ()
            self.TextHandlerAnim:SetObjActive(false)
        end,2)
    end
    self.last_remain_hp = boss_data.boss_remain_hp
end

function ShenYuProbeObjUIBoss:PackContent(num)
    if num > 0 then
        num = "+" .. num
    end
    return string.format("ShangHai@%s|", num)
end

function ShenYuProbeObjUIBoss:OnBtnClick()
    self.data:SetSelBossId(self.param.id)
    ViewMgr:OpenView(ShenYuProbeUpFight)
end


-- 神域探索锁妖井ui
ShenYuProbeObjUIWell = ShenYuProbeObjUIWell or DeclareMono("ShenYuProbeObjUIWell", UIWFlushPanel)
function ShenYuProbeObjUIWell:ShenYuProbeObjUIWell()
    self.data = ShenYuProbeData.Instance
    self.data_cares = {
        {data = self.data.well_pos_data, func = self.FlushInfo, init_call = true},
        {data = LandsBetweenData.Instance.operating_info, func = self.FlushInfo, init_call = true},
    }
    self.is_active = false
end

function ShenYuProbeObjUIWell:OnEnable()
    self.is_active = true
end

function ShenYuProbeObjUIWell:OnDisable()
    self.is_active = false
end

function ShenYuProbeObjUIWell:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function ShenYuProbeObjUIWell:SetParam(param)
    self.param = param
    self:FlushInfo()
end

function ShenYuProbeObjUIWell:FlushInfo()
    if TableIsEmpty(self.param) then
        return
    end
    if not self.is_active then
        return
    end

    local link_mark = self.param.link_mark
    local oper_link_mark = "0"
    if LandsBetweenData.Instance.operating_info.obj ~= nil then 
        local map_pic = LandsBetweenData.Instance:GetMapPicClick()
        oper_link_mark = map_pic.static.link_mark
    end 

    self.ProgressBar:SetObjActive(oper_link_mark ~= link_mark)
    self.Btn:SetActive(oper_link_mark == link_mark)

    local well_type = self.param.well_type
    local well_data = self.data.well_pos_data[well_type]
    local is_finish = well_data.seal_count >= well_data.need_count
    self.Finish:SetActive(is_finish)
    self.NotFinish:SetActive(not is_finish)
    if not is_finish then
        UH.SetText(self.ProgressText, string.format(Lang.Text20, well_data.seal_count, well_data.need_count))
        self.ProgressBar:SetProgress(well_data.seal_count/well_data.need_count)
        local pos = Split(link_mark, "|")
        UH.SetText(self.Name, string.format("%s(%s,%s)", self.param.name, pos[1],pos[2]))
    end
end

function ShenYuProbeObjUIWell:OnBtnClick()
    local well_type = self.param.well_type
    ViewMgr:OpenView(ShenYuProbeTower,{well_type = well_type})
end