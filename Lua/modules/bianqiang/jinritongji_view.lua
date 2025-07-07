JinRiTongJi = JinRiTongJi or DeclareView("JinRiTongJi", "bianqiang/jin_ri_tong_ji", Mod.BianQiang.TongJi)
function JinRiTongJi:JinRiTongJi()
    self.Board:SetData(self:GetType(), Language.BianQiang.Title, Vector2.New(963, 546))
    BianQiangCtrl.Instance:SendRoleInfo()
end

function JinRiTongJi:CloseCallback()
    BianQiangData.Instance.income_info:Uncare(self.care_handle)
    self.care_handle = nil
end

function JinRiTongJi:LoadCallback()
    self.care_handle = BianQiangData.Instance.income_info:Care(BindTool.Bind1(self.FlushView, self))
    self:FlushView()
end

function JinRiTongJi:FlushView()
    local info = BianQiangData.Instance:GetIncomeInfo()
    if info.role_count == nil then
        return
    end
    local type_list = {
        [1] = {seq = IncomeRoleType.DAY_EXP, icon = "ZhuZi", icon2 = "ZhuZiBlue"},
        [2] = {seq = IncomeRoleType.DAY_COIN_BIND, icon = "TongBi", icon2 = "TongBi"},
        [3] = {seq = IncomeRoleType.DAY_SILVER_COIN, icon = "LingYu", icon2 = "LingYu"}
    }
    for i = 1, 3 do
        UH.SetText(self.Names[i], RoleData.Instance:GetRoleName())
        local type_cfg = type_list[i]
        local cur_val = info.role_param_list[type_cfg.seq]
        local max_val = info.global_param_list[type_cfg.seq]
        UH.SetText(self.Mines[i], cur_val)
        max_val = math.max(max_val, 68)
        UH.SetText(self.AllNums[i], max_val)
        local config = BianQiangData.Instance:GetIncomeStageCfg(cur_val, math.max(max_val, 1))
        UH.SetText(self.Descs[i], config.description)
        UH.SpriteName(self.Value1s[i], type_cfg.icon .. config.icon1)
        UH.SpriteName(self.Value2s[i], type_cfg.icon2 .. config.icon2)
    end
end

function JinRiTongJi:OnClickExp()
    ViewMgr:OpenView(JinRiTongJiAlert, {title = Language.BianQiang.Exp, gain_type = IncomeRoleType.DAY_EXP + 1})
end
function JinRiTongJi:OnClickTongBi()
    ViewMgr:OpenView(JinRiTongJiAlert, {title = Language.BianQiang.TongBi, gain_type = IncomeRoleType.DAY_COIN_BIND + 1})
end
function JinRiTongJi:JinRiLingYu()
    ViewMgr:OpenView(JinRiTongJiAlert, {title = Language.BianQiang.LingYu, gain_type = IncomeRoleType.DAY_SILVER_COIN + 1})
end

JinRiTongJiAlert = JinRiTongJiAlert or DeclareView("JinRiTongJiAlert", "bianqiang/jin_ri_tong_ji_alert")
function JinRiTongJiAlert:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), param_t.title, Vector2.New(580, 542))

    local show_list = BianQiangData.Instance:GetGainShowList(param_t.gain_type)
    self.List:SetData(show_list)
end

JinRiTongJiAlertItem = JinRiTongJiAlertItem or DeclareMono("JinRiTongJiAlertItem", UIWidgetBaseItem)
function JinRiTongJiAlertItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name)
    UH.SetText(self.BtnTextY, data.button_name)
    UH.SetInter(self.BtnYellowInter, true)

    if data.open_type == 2 then
        local act_info = ActivityData.Instance:GetActivityInfo(data.open_param)
        self.ActIcon:SetObjActive(act_info ~= nil)
        self.Icon:SetObjActive(act_info == nil)
        if act_info ~= nil then
            UH.SetIcon(self.ActIcon, act_info.info.icon_id, ICON_TYPE.ACT)
        end
    else
        self.ActIcon:SetObjActive(false)
        self.Icon:SetObjActive(true)
        UH.SpriteName(self.Icon, data.icon_name)
    end

    self.AngleImage:SetObjActive(data.angle_type ~= 0)
    if data.angle_type ~= 0 then
        UH.SpriteName(self.AngleImage, BianQiangConfig.AngleType[data.angle_type].img_str)
        UH.SetText(self.AngleText, Language.BianQiang.AngleType[data.angle_type])
    end

    local is_finish = false
    if data.desc_type == 1 then --进度
        local max_val, cur_val = BianQiangData.Instance:GetTaskProgress(self.data)
        UH.SetText(self.Value, Format(Language.BianQiang.ProgStr, cur_val, max_val))
        is_finish = cur_val >= max_val
    elseif data.desc_type == 2 then --开启时间
        local act_info = ActivityData.Instance:GetActivityInfo(data.param1)
        if act_info == nil then
            LOG("今日统计展示活动开启时间，未找到活动配置-> ", data)
            return
        end
        UH.SetText(self.Value, act_info.info.begin_time .. "-" .. act_info.info.end_time)
        local max_val, cur_val = BianQiangData.Instance:GetTaskProgress(self.data)
        is_finish = cur_val >= max_val
        if not ActivityData.IsOpen(data.param1) then
            UH.SetInter(self.BtnYellowInter, false)
            UH.SetText(self.BtnTextY, Language.BianQiang.UnOpened)
        end
    elseif data.desc_type == 3 then --文字
        UH.SetText(self.Value, data.desc)
    end
    self.BtnYellow:SetActive(not is_finish)
    self.Finish:SetActive(is_finish)
end

function JinRiTongJiAlertItem:IsShowState()
    return BianQiangNoShowState[self.data.condition] == nil
end
function JinRiTongJiAlertItem:IsShowProg()
    return BianQiangNoShowProg[self.data.condition] == nil
end

function JinRiTongJiAlertItem:OnClickBtn()
    if self.data.open_panel ~= "" then
        local opened, open_tip = FunOpen.Instance:GetFunIsOpened(self.data.open_panel)
        if not opened then
            PublicPopupCtrl.Instance:Center(open_tip)
            return
        end
        if self.data.open_panel == Mod.CloudArena.Main then
            CloudArenaCtrl.Instance:OpenCloudArenaView()
        elseif self.data.open_panel == Mod.FightFB.Fight then
            FightData.Instance.fight_fb_ymz.type = 2
            ViewMgr:OpenViewByKey(self.data.open_panel)
        elseif self.data.open_panel == Mod.Mall.Market then
            MarketCtrl.Instance:OnMarketSell()
        else
            ViewMgr:OpenViewByKey(self.data.open_panel)
        end
        ViewMgr:CloseView(BianQiangView)
        ViewMgr:CloseView(RolePromoteView)
        ViewMgr:CloseView(JinRiTongJiAlert)
        ViewMgr:CloseView(JinRiTongJi)
    elseif self.data.commit_npc ~= "" then
        local npc_config = VoMgr:NpcVoByNpcId(self.data.commit_npc)
        if npc_config[1] == nil then
            return
        end
        local commit_npc = self.data.commit_npc
        local function func_call()
            SceneLogic.Instance:AutoToNpc(commit_npc)
        end
        SceneLogic.Instance:AutoToPos(npc_config[1].scene_id, nil, func_call, nil, true)
        ViewMgr:CloseView(BianQiangView)
        ViewMgr:CloseView(RolePromoteView)
        ViewMgr:CloseView(JinRiTongJiAlert)
        ViewMgr:CloseView(JinRiTongJi)
    elseif self.data.other_desc ~= "" then
        ViewMgr:OpenView(BianQiangAlert, {task_cfg = self.data})
    end
end
