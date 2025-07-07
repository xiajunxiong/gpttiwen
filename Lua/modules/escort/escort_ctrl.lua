-- 运镖
FILE_REQ("modules/escort/escort_config")
FILE_REQ("modules/escort/escort_data")
VIEW_REQ("modules/escort/escort_menu_view")
VIEW_REQ("modules/escort/escort_gvu_view")

EscortCtrl = EscortCtrl or BaseClass(BaseCtrl)

function EscortCtrl:__init()
    if EscortCtrl.Instance ~= nil then
        Debuger.LogError("[EscortCtrl] attempt to create singleton twice!")
        return
    end
    EscortCtrl.Instance = self
    self.data = EscortData.New()

    self:RegisterAllProtocols()

    -- BehaviorData.Instance:CustomNpcTalkBtnsSeq(self.data.escort_accept_npc_seq, BindTool.Bind(self.SetupBehaviourBtn, self))
end

function EscortCtrl:__delete()
    if EscortCtrl.Instance == self then
        EscortCtrl.Instance = nil
    end

    self:UnRegisterAllProtocols()
end

function EscortCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSEscortOp)
    self:RegisterProtocol(SCEscortAllInfo, "OnEscortAllInfo")
    self:RegisterProtocol(SCEscortResultNotic, "OnEscortResultNotic")
    self:RegisterProtocol(SCEscortStatusChangeNotice, "OnEscortStatusChangeNotice")
    FinishData.Instance:PassRegister(BATTLE_MODE.BATTLE_MODE_ESCORT + 1000,{
		value_func = function()
			return self.data:GetFinishValueData()
        end,
        desc_func = function()
			return self.data:GetEscortSuccTips()
        end,
        show_type = 2
	})
end

function EscortCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(SCEscortAllInfo)
    self:UnRegisterProtocol(SCEscortResultNotic)
    self:UnRegisterProtocol(SCEscortStatusChangeNotice)
    self.data:EscortData()
end

function EscortCtrl:OnUnloadGameLogic()
    MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
    if nil ~= EscortCtrl.Instance.task_btn_handle then
        BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(EscortCtrl.Instance.task_btn_handle)
        EscortCtrl.Instance.task_btn_handle = nil
    end
end

function EscortCtrl:OnEscortAllInfo(protocol)
    if self.data.escort_info.task_seq == 0 and self.data.escort_info.task_seq ~= protocol.escort_info.task_seq then
        PublicPopupCtrl.Instance:Center(Language.Duel.EscortTips1)
    end
    
    self.data:SetEscortAllInfo(protocol)
    RoleData.Instance:GetBaseData().escort_id = protocol.escort_info.task_seq
    if 0 ~= protocol.escort_info.task_seq then
        local npc_seq = protocol.escort_info.npc_seq
        if 0 ~= npc_seq then
            TimeHelper:AddDelayTimer(
                AutoInvokerCtrl.Instance:SetCommands({AutoCommandEscort.New(npc_seq)}, 1),
                1
            )
        end
        self.buff = MainViewData.Instance:AddBuff(
            {
                scene_buff = 0,
                source = 2,
				type = BuffType.CHANGE_MOVE_SPEED,
				change_per = Config.yunbiao_auto.other[1].slow_down,
            }
        )
	else
		MainViewData.Instance:RemoveBuff(self.buff)
        SceneCtrl.Instance:Logic():ClearMoveCache(true)
    end
end

function EscortCtrl:OnEscortResultNotic(protocol)
    local task_seq = self.data:SetEscortResultNotic(protocol)
    RoleData.Instance:GetBaseData().escort_id = 0
    local escort_info = self.data:GetEscortAllInfo()
    local daily_time = self.data:GetDailyTime()
    local info = self.data.config.yunbiao_config[self.data.escort_info.task_seq]

    if info ~= nil and info.progress_desc ~= 4 then 
        if 1 == protocol.is_win then
            PublicPopupCtrl.Instance:Center(Language.Escort.EscortFinishTips)
            local mode = 1 == protocol.is_win and (BATTLE_MODE.BATTLE_MODE_ESCORT + 1000) or BATTLE_MODE.BATTLE_MODE_ESCORT
        
            local close_func
            if escort_info.today_count < daily_time and 16 ~= task_seq then
                close_func = function ()
                -- PublicPopupCtrl.Instance:DialogTips{
                --     content = Language.Escort.ContinueTips,
                --     confirm = {
                --         func = function()
                --             ActivityData.Instance:GetActivityInfo(ActType.Escort):DoClickHandle()
                --             ViewMgr:CloseView(DialogTipsView)
                --         end
                --     }
                -- }
                    ActivityData.Instance:GetActivityInfo(ActType.Escort):DoClickHandle()
                end 
            elseif 16 ~= task_seq then
                close_func = function ()
                    ActivityCtrl.Instance:CheckAndOpenNextNotice()
                end
            end
            FinishData.Instance:SetCloseFunc(mode,close_func)
            FinishCtrl.Instance:Finish({is_win = protocol.is_win},mode)
        elseif escort_info.today_count == daily_time and 16 ~= task_seq  then
            ActivityCtrl.Instance:CheckAndOpenNextNotice()
        end
    end
    MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

function EscortCtrl:OnEscortStatusChangeNotice(protocol)
    VoMgr:ChangeRoleEscortId(protocol.obj_id, protocol.escort_id)
end

function EscortCtrl:SendEscortOp(param_t)
    local protocol = GetProtocol(CSEscortOp)
    protocol.op_type = param_t.op_type or 0
    protocol.param1 = param_t.param or 0
    protocol.param2 = param_t.param2 or 0
    SendProtocol(protocol)
end

function EscortCtrl:RequestEscort(param_t)
    local role_level = RoleData.Instance:GetRoleLevel()
    local daily_time = self.data:GetDailyTime()
    local escort_info = self.data:GetEscortAllInfo()
    local min_level = self.data:GetMinLevelByType(param_t.param)
    if EscortConfig.OperType.commit == param_t.op_type then
        self:SendEscortOp(param_t)
        return
    end
    if escort_info.today_count >= daily_time then
        PublicPopupCtrl.Instance:Center(Language.Escort.EscortNoTimesTips)
        return
    end
    if escort_info.task_seq > 0 then
        PublicPopupCtrl.Instance:Center(Language.Escort.EscortIngTips)
        return
    end
    if role_level < min_level then
        PublicPopupCtrl.Instance:Center(string.format(Language.Escort.LevelTips, min_level))
        return
    end

    local consume = self.data:GetConsumeByType(param_t.param)
    local coin_bind = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    local info1 = {
        content = Language.Escort.InTeamTips,
        confirm = {
            func = function()
                if coin_bind < consume then
                    PublicPopupCtrl.Instance:Center(Language.Escort.CoinBindTips)
                else
                    TeamCtrl.Instance:SendExitTeam()
                    self:SendEscortOp(param_t)
                end
                ViewMgr:CloseView(DialogTipsView)
            end
        }
    }
    local info2 = {
        content = string.format(Language.Escort.ConsumeTips, consume),
        confirm = {
            func = function()
                if coin_bind < consume then
                    PublicPopupCtrl.Instance:Center(Language.Escort.CoinBindTips)
                else
                    self:SendEscortOp(param_t)
                end
                ViewMgr:CloseView(DialogTipsView)
            end
        }
    }
    if EscortConfig.OperType.accept == param_t.op_type then
        PublicPopupCtrl.Instance:DialogTips(TeamData.Instance:InTeam() and info1 or info2)
    end
end

function EscortCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    if nil == obj_vo then
        return
    end
    if obj_vo.seq == self.data.escort_accept_npc_seq then
        local role_level = RoleData.Instance:GetRoleLevel()
        for i = 1, EscortConfig.ESCORT_TYPE_MAX do
            local min_level = self.data:GetMinLevelByType(i)
            local rewards, deposit = self.data:GetDetailInfo(i, role_level)
            local detail_tips = Language.Escort.DetailTips[i]
            local button = NpcDialogBehBtnData.FuncCreate(
                Language.Escort.EscortDisStrs.escort[i],
                function ()
                    if role_level >= min_level then
                        self:RequestEscort{
                            op_type = EscortConfig.OperType.accept,
                            param = i
                        }
                    else
                        PublicPopupCtrl.Instance:Center(string.format(Language.NpcDialog.level_tips, min_level))
                    end
                end,
                true, role_level >= min_level and NpcDialogData.CreateDetailInfo1{tips = {string.format(detail_tips[1], deposit), detail_tips[2]}, rewards = rewards} or nil
            )
            button:SetIconType(NpcIconType.Act)
            button:SetIsGrey(role_level < min_level)
            table.insert(
                btn_list,
                button
            )
        end
    end
end

function EscortCtrl:IsEscorting()
    return self.data.escort_info and self.data.escort_info.task_seq > 0
end
