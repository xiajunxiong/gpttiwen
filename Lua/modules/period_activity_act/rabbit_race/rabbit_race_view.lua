-- 玩法说明
RabbitRaceDescView = RabbitRaceDescView or DeclareView("RabbitRaceDescView", "period_activity_act/rabbit_race/rabbit_race_desc_view")
function RabbitRaceDescView:LoadCallback()
    local desc_id_1 = Config.rabbit_run_auto.other[1].desc1
    local desc_id_2 = Config.rabbit_run_auto.other[1].desc2
    local tab = RichTextHelper.ColorStr("ttttt", COLORSTR.Transparent)
    UH.SetText(self.DescL, tab .. Config.language_cfg_auto.textdesc[desc_id_1].desc)
    UH.SetText(self.DescR, tab .. Config.language_cfg_auto.textdesc[desc_id_2].desc)
end

function RabbitRaceDescView:OnCloseClick()
    ViewMgr:CloseView(RabbitRaceDescView)
end

-- 兔子布局
RabbitRaceReadyView = RabbitRaceReadyView or DeclareView("RabbitRaceReadyView", "period_activity_act/rabbit_race/rabbit_race_ready_view")
function RabbitRaceReadyView:RabbitRaceReadyView()
    
end

function RabbitRaceReadyView:LoadCallback()
    
end

function RabbitRaceReadyView:CloseCallback()
    RabbitRaceData.Instance:ClearSelectData()
end

function RabbitRaceReadyView:OnCloseClick()
    ViewMgr:CloseView(RabbitRaceReadyView)
end

RabbitRaceReadyPanel = RabbitRaceReadyPanel or DeclareMono("RabbitRaceReadyPanel", UIWFlushPanel)
function RabbitRaceReadyPanel:RabbitRaceReadyPanel()
    self.data = RabbitRaceData.Instance
    self.data_cares = {
        {data = self.data.select_rabbit_data, func = self.FlushPanel, init_call = false},
        {data = self.data.base_data, func = self.FlushPanel, init_call = false},
    }
    self.data.is_ok = false
end

function RabbitRaceReadyPanel:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetText(self.LName, RoleData.Instance:GetRoleName())
    self.LAvatar:SetData(RoleData.Instance:GetRoleData())
    self.TimeMeter:SetCallBack(BindTool.Bind(self.TimeEndCallback,self))

    local side_info = self.data:GetSideInfo()
    if side_info then
        local app = side_info.appearance
        local avatar_data = {
            avatar_type = app.avatar_type,
            avatar_id = app.avatar_id,
            avatar_quality = app.avatar_quality,
            --top_level = 
            avatar_level = side_info.level,
            role_id = side_info.uid,
        }
        self.RAvatar:SetData(avatar_data)
        UH.SetText(self.RName, side_info.name)
    end

    self:FlushPanel()
end

function RabbitRaceReadyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.TimeMeter:CloseCountDownTime()
end

function RabbitRaceReadyPanel:FlushOkBtn()
    local is_full = self.data:IsFullSelectData()
    self.OkBtn:SetActive(is_full)

    if is_full then
        self.Inter.Interactable = not self.data.is_ok
        UH.SetText(self.OkBtnText, self.data.is_ok and Language.RabbitRun.Btn3 or Language.RabbitRun.Btn4)
    end
end

function RabbitRaceReadyPanel:FlushPanel()
    local phase = self.data:GetPhase()
    UH.SetText(self.PhaseName, Language.RabbitRun.PKStateName[phase])
    self.BottomInfo:SetActive(phase ~= RabbitRaceCtrl.Phase.Gameing and phase ~= RabbitRaceCtrl.Phase.StartRun)
    
    if phase ~= self.last_phase then
        if phase == RabbitRaceCtrl.Phase.Gameing then
            self.TimeMeter:CloseCountDownTime()
            self.TimeMeter:TotalTime(5,TimeType.Type_Special_5)
        elseif phase == RabbitRaceCtrl.Phase.Set then
            self:StartTime(self.data.base_data.next_phase_time)
            UH.SetText(self.Tip, Language.RabbitRun.Tip5)
        elseif phase == RabbitRaceCtrl.Phase.Update then
            self:StartTime(self.data.base_data.next_phase_time)
            UH.SetText(self.Tip, Language.RabbitRun.Tip6)
        end
        ViewMgr:CloseView(RabbitRaceSelectView)
        self.data.is_ok = false
    end
    self.last_phase = phase

    local list_data = {
        {track = 0},
        {track = 1},
        {track = 2},
    }
    self.List:SetData(list_data)

    self:FlushOkBtn()
end

-- 开始倒计时
function RabbitRaceReadyPanel:StartTime(timestamp)
    self.TimeMeter:CloseCountDownTime()
    self.TimeMeter:StampTime(timestamp or 0,TimeType.Type_Special_5)
end

function RabbitRaceReadyPanel:TimeEndCallback()
    local phase = self.data:GetPhase()
    if phase == RabbitRaceCtrl.Phase.Gameing or phase == RabbitRaceCtrl.Phase.StartRun then
        ViewMgr:CloseView(RabbitRaceReadyView)
        ViewMgr:OpenView(RabbitRaceSceneReady)
    end
end

function RabbitRaceReadyPanel:OnOkClick()
    local is_full = self.data:IsFullSelectData()
    if is_full then
        RabbitRaceCtrl.Instance:SaveRabbitInfo()
        self.data.is_ok = true
        self:FlushPanel()
        RabbitRaceCtrl.Instance:SendRabbitRaceReq(RabbitRaceCtrl.ReqType.SetOk)
    end
end

RabbitRaceReadyItem = RabbitRaceReadyItem or DeclareMono("RabbitRaceReadyItem", UIWidgetBaseItem)
function RabbitRaceReadyItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.gameObject.name = "Item" .. data.track
    UH.SetText(self.Name, Language.RabbitRun.WayName[data.track])
    local rabbit = RabbitRaceData.Instance:GetSelectRibbit(data.track)
    self.SelfInfo:SetData({rabbit = rabbit})

    local phase = RabbitRaceData.Instance:GetPhase()
    if phase == RabbitRaceCtrl.Phase.Set then
        self.SideInfo:SetData({rabbit = nil})
    else
        local side_rabbit = RabbitRaceData.Instance:GetTrackRibbit(data.track, 1)
        self.SideInfo:SetData({rabbit = side_rabbit})
    end
end

function RabbitRaceReadyItem:OnSelectClick()
    ViewMgr:OpenView(RabbitRaceSelectView, {track_type = self.data.track})
end

RabbitRaceReadyItemInfo = RabbitRaceReadyItemInfo or DeclareMono("RabbitRaceReadyItemInfo")
function RabbitRaceReadyItemInfo:SetData(data)
    local rabbit_id = data.rabbit
    self.State1:SetActive(rabbit_id == nil and not self.IsSelf)
    self.State2:SetActive(rabbit_id == nil and self.IsSelf)
    self.State3:SetActive(rabbit_id ~= nil)
    if rabbit_id ~= nil then
        local phase = RabbitRaceData.Instance:GetPhase()
        self.UpdateBtn:SetActive(self.IsSelf and not RabbitRaceData.Instance.is_ok and phase ~= RabbitRaceCtrl.Phase.Gameing and phase ~= RabbitRaceCtrl.Phase.StartRun)
        local rabbit_cfg = RabbitRaceData.Instance:GetRabbitCfg(rabbit_id)
        if rabbit_cfg == nil then
            LogError("rabbit_cfg 为空", rabbit_id)
            return
        end
        UH.SetText(self.Name, rabbit_cfg.name)
        if self.IsSelf then
            UH.SetText(self.Desc, rabbit_cfg.desc)
        else
            UH.SetText(self.Desc, Language.RabbitRun.Tip7)
        end

        local img_name
        if self.IsSelf then
            img_name = RabbitRaceData.Instance.is_main and "Tu" .. rabbit_id or "TuTu" .. rabbit_id
        else
            img_name = RabbitRaceData.Instance.is_main and "TuTu" .. rabbit_id or "Tu" .. rabbit_id
        end
        UH.SpriteName(self.Icon, img_name)
        UH.SpriteName(self.BG, "TuZiDi" .. rabbit_id)
    end
end

-- 结算界面
RabbitRaceOverView = RabbitRaceOverView or DeclareView("RabbitRaceOverView", "period_activity_act/rabbit_race/rabbit_race_over_view")
function RabbitRaceOverView:LoadCallback()
    ViewMgr:CloseView(RabbitRaceOperView)
    UH.SetText(self.LName, RoleData.Instance:GetRoleName())
    self.LAvatar:SetData(RoleData.Instance:GetRoleData())

    local side_info = RabbitRaceData.Instance:GetSideInfo()
    if side_info then
        local app = side_info.appearance
        local avatar_data = {
            avatar_type = app.avatar_type,
            avatar_id = app.avatar_id,
            avatar_quality = app.avatar_quality,
            --top_level = 
            avatar_level = side_info.level,
            role_id = side_info.uid,
        }
        self.RAvatar:SetData(avatar_data)
        UH.SetText(self.RName, side_info.name)
    end
    local list_data = {
        {track = 0},
        {track = 1},
        {track = 2},
    }
    self.List:SetData(list_data)

    local role_id = RoleData.Instance:GetRoleId()
    self.WinState:SetActive(role_id == RabbitRaceData.Instance.win_role_id)
    self.FailState:SetActive(role_id ~= RabbitRaceData.Instance.win_role_id)

    -- 奖励
    if not TableIsEmpty(RabbitRaceData.Instance.reward_list) then
        self.Reward:SetActive(true)
        local reward_list = {}
        local other_cfg = Config.rabbit_run_auto.other[1]
        if role_id == RabbitRaceData.Instance.win_role_id then
            reward_list = DataHelper.FormatItemList(other_cfg.succ_reward)
        else
            reward_list = DataHelper.FormatItemList(other_cfg.fail_reward)
        end
        self.RewardList:SetData(reward_list)
        for i,v in pairs(RabbitRaceData.Instance.reward_list) do
            BagData.Instance:GiftShowUse(v)
        end
        RabbitRaceData.Instance.reward_list = {}
    else
        self.Reward:SetActive(false)
    end
    
    -- 20秒自动关闭界面
    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        self:OnCloseClick()
    end, 20)
end

function RabbitRaceOverView:CloseCallback()
    TimeHelper:CancelTimer(self.time_ht)
    -- if RabbitRaceData.Instance.is_main then
    --     SceneCtrl.Instance:RequestLeaveFb()
    -- end
    RabbitRaceCtrl.Instance:SendRabbitRaceReq(RabbitRaceCtrl.ReqType.Leave)
    RabbitRaceData.Instance:ClearData()
end

function RabbitRaceOverView:OnCloseClick()
    ViewMgr:CloseView(RabbitRaceOverView)
end

RabbitRaceOverItem = RabbitRaceOverItem or DeclareMono("RabbitRaceOverItem", UIWidgetBaseItem)
function RabbitRaceOverItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.gameObject.name = "Item" .. data.track
    local l_is_win = RabbitRaceData.Instance:IsWin(data.track)
    self.LWinFlag:SetActive(l_is_win == 1)
    UH.SpriteName(self.LBg, l_is_win == 1 and "TuCeng1" or "TuCeng2")
    UH.SetText(self.TrackName, Language.RabbitRun.WayName[data.track])
    local l_rabbit_id = RabbitRaceData.Instance:GetTrackRibbit(data.track)
    UH.SpriteName(self.LIconBg, "TuZiDi" .. l_rabbit_id)
    if RabbitRaceData.Instance.is_main then
        UH.SpriteName(self.LIcon, "Tu" .. l_rabbit_id)
    else
        UH.SpriteName(self.LIcon, "TuTu" .. l_rabbit_id)
    end
    local l_rabbit_cfg = RabbitRaceData.Instance:GetRabbitCfg(l_rabbit_id)
    UH.SetText(self.LName, l_rabbit_cfg.name)
    UH.SetText(self.LDesc, l_rabbit_cfg.desc)


    local side_info = RabbitRaceData.Instance:GetSideInfo()
    if side_info then
        local r_id_win = RabbitRaceData.Instance:IsWin(data.track, side_info.uid)
        self.RWinFlag:SetActive(r_id_win == 1)
        UH.SpriteName(self.RBg, r_id_win == 1 and "TuCeng1" or "TuCeng2")
        local r_rabbit_id = RabbitRaceData.Instance:GetTrackRibbit(data.track, 1)
        if RabbitRaceData.Instance.is_main then
            UH.SpriteName(self.RIcon, "TuTu" .. r_rabbit_id)
        else
            UH.SpriteName(self.RIcon, "Tu" .. r_rabbit_id)
        end
        UH.SpriteName(self.RIconBg, "TuZiDi" .. r_rabbit_id)
        local r_rabbit_cfg = RabbitRaceData.Instance:GetRabbitCfg(r_rabbit_id)
        if r_rabbit_cfg then
            UH.SetText(self.RName, r_rabbit_cfg.name)
            UH.SetText(self.RDesc, r_rabbit_cfg.desc)
        end
    else
        self.RWinFlag:SetActive(false)
    end
end

-- 选择界面
RabbitRaceSelectView = RabbitRaceSelectView or DeclareView("RabbitRaceSelectView", "period_activity_act/rabbit_race/ribbit_race_select_view")
VIEW_DECLARE_MASK(RabbitRaceSelectView,ViewMask.BgBlockClose)
function RabbitRaceSelectView:RabbitRaceSelectView()
    
end

function RabbitRaceSelectView:LoadCallback(param_t)
    param_t = param_t or {}
    local track_type = param_t.track_type
    local list_data = {
        {rabbit_type = 1, sel_track_type = track_type},
        {rabbit_type = 2, sel_track_type = track_type},
        {rabbit_type = 3, sel_track_type = track_type},
    }
    self.List:SetData(list_data)
    UH.SetText(self.Title, string.format(Language.RabbitRun.Tip8, Language.RabbitRun.WayName[track_type]))
end

RabbitRaceSelectItem = RabbitRaceSelectItem or DeclareMono("RabbitRaceSelectItem", UIWidgetBaseItem)
function RabbitRaceSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local rabbit_type = data.rabbit_type
    local track = RabbitRaceData.Instance:GetSelectTrack(rabbit_type)
    self.track = track
    local name = track ~= nil and Language.RabbitRun.WayName[track] or ""
    UH.SetText(self.Name, name)
    local img_name = RabbitRaceData.Instance.is_main and "Tu" .. rabbit_type or "TuTu" .. rabbit_type
    UH.SpriteName(self.RibbitImg, img_name)
    
    self.sel_rabbit = RabbitRaceData.Instance:GetSelectRibbit(data.sel_track_type)
    self.Flag:SetActive(track ~= nil)
    self.ReplaceBtn:SetActive((self.sel_rabbit ~= nil and self.sel_rabbit ~= rabbit_type) or (track ~= nil and track ~= data.sel_track_type))
    self.SelectBtn:SetActive(track == nil and self.sel_rabbit == nil)

    local rabbit_cfg = RabbitRaceData.Instance:GetRabbitCfg(rabbit_type)
    UH.SetText(self.RabbitName, rabbit_cfg.name)
    UH.SetText(self.RabbitDesc, rabbit_cfg.desc)
end

function RabbitRaceSelectItem:OnSelectClick()
    RabbitRaceData.Instance:SetSelectRibbit(self.data.sel_track_type, self.data.rabbit_type, true)
    ViewMgr:CloseView(RabbitRaceSelectView)
end

function RabbitRaceSelectItem:OnReplaceClick()
    if self.track then
        --RabbitRaceData.Instance:SetSelectRibbit(self.track, self.sel_rabbit)
        RabbitRaceData.Instance:SetSelectRibbit(self.track, nil, true)
    end
    RabbitRaceData.Instance:SetSelectRibbit(self.data.sel_track_type, self.data.rabbit_type, true)
    ViewMgr:CloseView(RabbitRaceSelectView)
end


-- 菜单栏
RabbitRaceMemu = RabbitRaceMemu or DeclareMono("RabbitRaceMemu", UIWFlushPanel)
function RabbitRaceMemu:RabbitRaceMemu()
    self.data = RabbitRaceData.Instance
    self.data_cares = {
        {data = self.data.base_data, func = self.FlushPanel, init_call = false},
        {data = self.data.prop_info, func = self.FlushPanel, init_call = false}
    }
end

function RabbitRaceMemu:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function RabbitRaceMemu:FlushPanel()
    local phase = RabbitRaceData.Instance:GetPhase()
    if phase == RabbitRaceCtrl.Phase.Gameing or phase == RabbitRaceCtrl.Phase.StartRun then
        local info_text = ""
        for track = 0, 2 do
            local self_rabbit = self.data:GetTrackRibbit(track)
            local self_name

            if RabbitRaceData.Instance.is_main then
                self_name = ColorStr(self.data:GetRabbitCfg(self_rabbit).name, COLORSTR.Green8)
            else
                self_name = ColorStr(self.data:GetRabbitCfg(self_rabbit).name, COLORSTR.Red12)
            end

            local side_info = self.data:GetSideInfo() or {}
            local side_rabbit = self.data:GetTrackRibbit(track,1)
            local side_cfg = self.data:GetRabbitCfg(side_rabbit)
            local side_name = side_cfg == nil and "" or side_cfg.name

            if RabbitRaceData.Instance.is_main then
                side_name = ColorStr(side_name, COLORSTR.Red12)
            else
                side_name = ColorStr(side_name, COLORSTR.Green8)
            end

            local result_role_id = RabbitRaceData.Instance:GetTrackWinRoleId(track)
            local text
            if result_role_id and result_role_id ~= RoleData.Instance:GetRoleId() and result_role_id ~= side_info.uid then
                text = string.format(Language.RabbitRun.Tip9, self_name, side_name)
            else
                local result = result_role_id == RoleData.Instance:GetRoleId() and Language.RabbitRun.Result1 or Language.RabbitRun.Result2
                text = string.format(Language.RabbitRun.Tip12, self_name,result, side_name)
            end
            if track ~= 2 then
                text = text .. "\n"
            end
            info_text = info_text .. text
        end
        UH.SetText(self.InfoText, info_text)
        UH.SetText(self.ItemInfo, string.format(Language.RabbitRun.Tip10,self.data:ItemCount()))
    else
        UH.SetText(self.InfoText, "")
        UH.SetText(self.ItemInfo, "")
    end

    local other_cfg = Config.rabbit_run_auto.other[1]
    local win_reward_list = DataHelper.FormatItemList(other_cfg.succ_reward)
    local fail_reward_list = DataHelper.FormatItemList(other_cfg.fail_reward)
    self.RewardList1:SetData(win_reward_list)
    self.RewardList2:SetData(fail_reward_list)
end




RabbitRaceObjCh = RabbitRaceObjCh or BaseClass(SceneCh)

function RabbitRaceObjCh:__init()
    self.vo = nil
    self.is_des = false -- 是否到达终点
end

function RabbitRaceObjCh:InitEnd()
    self:SetMoveSpeed(self.vo.move_speed * 0.2)
    if self.vo.real_x and self.vo.real_y then
        local pos = SceneCtrl.Instance:CurScene():PosToClint(self.vo.real_x, self.vo.real_y)
        TimeHelper:AddRunFrameTimer(function ()
            self:MoveTo(pos, false)
        end,1,1)
    end
    TimeHelper:CancelTimer(self.bubble_time_ht)
    self.bubble_time_ht = Invoke(function ()
        if RabbitRaceData.Instance.obj_bubble_data[self.vo.obj_id] then
            local bubble = RabbitRaceData.Instance.obj_bubble_data[self.vo.obj_id]
            local npc_board = MainViewData.Instance:GetBoard([[obj_id]],self.vo.obj_id)
            if npc_board then
                npc_board:SetBubble(true, bubble)
            else
                LogError("npc_board == nil ",self.vo.obj_id)
            end
            RabbitRaceData.Instance.obj_bubble_data[self.vo.obj_id] = nil
        end
    end,1)
end

function RabbitRaceObjCh:__delete()
    --SceneObj.__delete(self)
    TimeHelper:CancelTimer(self.bubble_time_ht)
end

function RabbitRaceObjCh:InteractionBehavior()
    if self.vo.is_rest and RabbitRaceData.Instance:IsSelfRabbitNpc(self.vo.npc_id) then
        RabbitRaceData.Instance.oper_obj_id = self.vo.obj_id
        ViewMgr:OpenView(RabbitRaceOperView)
    end
end

function RabbitRaceObjCh:OnClickObjEvent()
    if self.is_des then
        return
    end
    if not self.vo.is_rest and RabbitRaceData.Instance:IsSelfRabbitNpc(self.vo.npc_id) then
        RabbitRaceData.Instance.oper_obj_id = self.vo.obj_id
        ViewMgr:OpenView(RabbitRaceOperView)
    end
end

function RabbitRaceObjCh:OnMovedEvent()
    if self.is_des then
        return
    end
    --LogError(string.format("移动结束%s赛道的%s", self.vo.track,self.vo.obj_id))
    if self.vo.track then
        RabbitRaceData.Instance.track_move_info[self.vo.track] = true
    end
    RabbitRaceCtrl.Instance:CheckOver()
    self.is_des = true
end

function RabbitRaceObjCh:CheckDes()
    if self.is_des then
        return
    end
    if self.vo.des_pos then
       local dis_num = GameMath.GetDistance(self.vo.des_pos.x, self.vo.des_pos.z, self:GetPos().x, self:GetPos().z, false)
       if dis_num <= 1 then
            self:OnMovedEvent()
       end
    end
end

-- 赛前准备
RabbitRaceSceneReady = RabbitRaceSceneReady or DeclareView("RabbitRaceSceneReady", "period_activity_act/rabbit_race/rabbit_race_scene_ready_view")
VIEW_DECLARE_MASK(RabbitRaceSceneReady,ViewMask.Block)
function RabbitRaceSceneReady:LoadCallback()
    local count_down_num = 3
    self.timer_tta = TimeHelper:AddCountDownTTA(function ()
        self.CountDownObj:SetActive(true)
        if 0 == count_down_num then
            self.CountDownNum.text = ""
            self.CountDownAnim:SetTrigger("tri_go")
        else
            self.CountDownNum.text = count_down_num
            self.CountDownAnim:SetTrigger("tri_321")
        end
        count_down_num = count_down_num - 1
    end, function ()
        self.CountDownObj:SetActive(false)
        ViewMgr:CloseView(RabbitRaceSceneReady)
    end, count_down_num + 2, 1, false)
end

function RabbitRaceSceneReady:CloseCallback()
    TimeHelper:CancelTimer(self.timer_tta)
end


-- 兔子操作
RabbitRaceOperView = RabbitRaceOperView or DeclareView("RabbitRaceOperView", "period_activity_act/rabbit_race/rabbit_race_oper")
VIEW_DECLARE_MASK(RabbitRaceOperView,ViewMask.None)
function RabbitRaceOperView:LoadCallback(param_t)
    UH.SetText(self.Tip, string.format(Language.RabbitRun.Btn8, RabbitRaceData.Instance:ItemCount()))
    self.is_food = false    -- 是否已经喂过胡萝卜了。防止快点
end

-- 赞扬
function RabbitRaceOperView:OnOper1()
    self.obj_id = RabbitRaceData.Instance.oper_obj_id
    if self.obj_id then
        if RabbitRaceData.Instance.obj_list[self.obj_id] == 1 then
            return
        end
        RabbitRaceData.Instance.oper_record = RabbitRaceCtrl.Enc.Praise
        RabbitRaceCtrl.Instance:SendRabbitRaceReq(RabbitRaceCtrl.ReqType.Operate, RabbitRaceCtrl.Enc.Praise, self.obj_id)
        ViewMgr:CloseView(RabbitRaceOperView)
    end
end

-- 激将
function RabbitRaceOperView:OnOper2()
    self.obj_id = RabbitRaceData.Instance.oper_obj_id
    if self.obj_id then
        if RabbitRaceData.Instance.obj_list[self.obj_id] == 1 then
            return
        end
        RabbitRaceData.Instance.oper_record = RabbitRaceCtrl.Enc.Goad
        RabbitRaceCtrl.Instance:SendRabbitRaceReq(RabbitRaceCtrl.ReqType.Operate, RabbitRaceCtrl.Enc.Goad, self.obj_id)
        ViewMgr:CloseView(RabbitRaceOperView)
    end
end

-- 喂萝卜
function RabbitRaceOperView:OnOper3()
    if self.is_food then
        return false
    end
    self.obj_id = RabbitRaceData.Instance.oper_obj_id
    if self.obj_id then
        if RabbitRaceData.Instance:ItemCount() > 0 then
            RabbitRaceData.Instance.oper_record = RabbitRaceCtrl.Enc.Food
            RabbitRaceCtrl.Instance:SendRabbitRaceReq(RabbitRaceCtrl.ReqType.Operate, RabbitRaceCtrl.Enc.Food, self.obj_id)
            ViewMgr:CloseView(RabbitRaceOperView)
            self.is_food = true
        else
            PublicPopupCtrl.Instance:Error(Language.RabbitRun.Tip11)
        end
    end
end

