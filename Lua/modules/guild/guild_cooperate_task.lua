GuildCooperateTaskView = GuildCooperateTaskView or DeclareView("GuildCooperateTaskView", "guild/guild_cooperate_task", Mod.Guild.GuildCooperateTask)
VIEW_DECLARE_LEVEL(GuildCooperateTaskView, ViewLevel.L1)
function GuildCooperateTaskView:GuildCooperateTaskView() end 
function GuildCooperateTaskView:LoadCallback(param_t)
    GuildData.Instance:SelectCooperateOperateItem()

    self.Board:SetData(self:GetType(), Language.Guild.Tab15)
    self.Board:SetTabbarVisible(true)
    self.Currency:SetCurrency(CurrencyType.CoinBind, CurrencyType.ImmortalCoin, CurrencyType.Gold, CurrencyType.GuildCotribution)
    self.Board.Tabbar:SetData({
        {name = Language.Guild.Tab10, tab_objs = {self.TaskPartObj}},
        {name = Language.Guild.Tab16, tab_objs = {self.HelpPartObj},open = Mod.Guild.GuildCooperateTask ,mod = TabMod(Mod.Guild.GuildCooperateTask)},
    })
    self.Board.Tabbar:ClickItem(param_t.quick == nil and 1 or param_t.quick)
    GuildData.Instance:SetTodayEntered()
end 

GuildCooperateTaskPanel = GuildCooperateTaskPanel or DeclareMono("GuildCooperateTaskPanel", UIWFlushPanel)
function GuildCooperateTaskPanel:GuildCooperateTaskPanel()
    self.data = GuildData.Instance

    self.data_cares = {
        {data = self.data.cooperate_data,func = self.FlushPanel},
        {data = self.data.cooperate_operate,func = self.FlushPanel},
        {data = self.data.member_info,func = self.FlushPanel},
    }

    self.TaskList:SetCompleteCallBack(function()
        if self.data:GetSelectCooperateOperateItem() == nil then               
            self.TaskList:ClickItem(self.data:GetShouldShowingTask())
        else 
            -- self.TaskList:ClickItem(self.data.cooperate_operate.operate_task_id)
        end 
    end)
end

function GuildCooperateTaskPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SelectCooperateOperateItem()
end

function GuildCooperateTaskPanel:FlushPanel()
    local list = self.data:GetCooperateTaskList()
    self.TaskList:SetData(list)
    local detail = self.data:GetCooperateTaskDeatil()

    self.DetailTotal:SetActive(not detail.is_empty)
    self.EmptyTotal:SetActive(detail.is_empty)
    UH.SetText(self.TaskFlushTime,detail.flush_time)

    if detail.is_empty then
        self.EmptyTiming:StampTime(detail.refresh_time)
        return 
    end

    UH.SpriteName(self.TaskIcon, detail.icon)
    UH.SetText(self.TaskName, detail.name)
    UH.SetText(self.TaskDesc, detail.desc)
    self.TaskReward:SetData(detail.reward)
    UH.SetText(self.TaskOperate, detail.oper_str)
    UH.SetText(self.TaskFlushCost, detail.flush_cost)
    self.TaskStatusRunning:SetActive(detail.status_running)
    self.BtnHelp:SetActive(detail.show_help)
    self.BtnHelpInter.Interactable = detail.show_help_inter
    self.BtnHelpDone:SetActive(false)--detail.show_help_done)
    self.BtnOperate:SetActive(detail.show_oper)
    self.BtnAccept:SetActive(detail.show_accept)
end

function GuildCooperateTaskPanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[364].desc})
end

function GuildCooperateTaskPanel:OnClickTaksFlush()
    local buy_num = Config.family_daily_mission_auto.other[1].refresh_cost
    if not MallCtrl.IsNotCurrency(CurrencyType.ImmortalCoin,buy_num) then
        PublicPopupCtrl.Instance:Center(Language.Common.ImmortalCoinLack)
        MallData.JumpItemNum = MarketData.Instance:GetNeedGoldNum(buy_num)
        return 
    end

    local info = {
		content = string.format(Language.GuildCooperateTask.FlushDialog,(Language.Common.HuoBi[16]..Config.family_daily_mission_auto.other[1].refresh_cost)), 
		cancel = {
			name = nil,
			func = nil
		},
		confirm = {
			name = nil,
			func = function()
                GuildCtrl.SendGuildCooperateReq({
                    req_type = 4
                })
                PublicPopupCtrl.Instance:CloseDialogTips()
            end,
		}
	}

	PublicPopupCtrl.Instance:DialogTips(info)


end

function GuildCooperateTaskPanel:OnClickDelete()
    local detail = self.data:GetCooperateTaskDeatil()

    local info = {
		content =Language.GuildCooperateTask.DeleteDialog, 
		cancel = {
			name = nil,
			func = nil
		},
		confirm = {
			name = nil,
			func = function()
            
                GuildCtrl.SendGuildCooperateReq({
                    req_type = 1,
                    param1 = detail.index ,
                })
                PublicPopupCtrl.Instance:CloseDialogTips()
                local state = MainViewData.Instance:GetGameState()
                if state == MainViewGameState.escort then
                    EscortCtrl.Instance:SendEscortOp{
                        op_type = EscortConfig.OperType.give_up
                    }
                    SceneLogic.Instance:ClearMoveCache()
                    -- MainPlayer:StopMove()
                end
            end,
		}
	}


	PublicPopupCtrl.Instance:DialogTips(info)
end

function GuildCooperateTaskPanel:OnClickAccept()
    local detail = self.data:GetCooperateTaskDeatil()
    if detail.finish_done then
        PublicPopupCtrl.Instance:Center(Language.GuildCooperateTask.MaxTimeTask)
        return 
    end 

    GuildCtrl.SendGuildCooperateReq({
        req_type = 0,
        param1 = detail.index,
    })
end

function GuildCooperateTaskPanel:OnClickOperate()
    local detail = self.data:GetCooperateTaskDeatil()

    ViewMgr:CloseView(GuildCooperateTaskView)
    ViewMgr:CloseView(GuildView)

    TimeHelper:AddDelayTimer(function ()
        local task = TaskData.Instance:GetTaskInfoByTaskId(detail.task_id)
        local next_task = nil 
        if detail.task_id ~= detail.end_task_id then 
            next_task = TaskData.Instance:GetTaskInfoByTaskId(detail.end_task_id)
        end 

        if task then 
            AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task)}, 1)
            if detail.sub_type == 4 then   
                BehaviorData.Instance.auto_behavior = 2
            else 
            
                BehaviorData.Instance.auto_behavior = 0
            end 
        elseif next_task then 
            AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(next_task)}, 1)
            if detail.sub_type == 4 then   
                BehaviorData.Instance.auto_behavior = 2
            else 
            
                BehaviorData.Instance.auto_behavior = 0
            end 
        else 
            LogDG("任务配置异常")
        end 
    end, 1)
end

function GuildCooperateTaskPanel:OnClickHelp()
    if LoginData.Instance:BaseData().is_on_crosssever == true then 
        
        local info = {
            content = Language.Common.CrossTips,
            cancel = {
                name = nil,
                func = nil
            },
            confirm = {
                name = nil,
                func = function()
                    LoginCtrl.Instance:SendBackToOriginServer()
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end,
            }
        }
    
        PublicPopupCtrl.Instance:DialogTips(info)

        return
    end 

    local detail = self.data:GetCooperateTaskDeatil()
    if not detail.show_help_inter then 
        PublicPopupCtrl.Instance:Center(Language.GuildCooperateTask.HelpError)
        return 
    end

    GuildCtrl.SendGuildCooperateReq({
        req_type = 2,
    })
    local paramt = self.data:GetTypeHelpStr()
    ChatCtrl.Instance:SendChannelChatText(paramt.str,paramt.item, ChatConfig.ChannelType.guild)
end

function GuildCooperateTaskPanel:OnClickHelpDone()
    GuildCtrl.SendGuildCooperateReq({
        req_type = 7,
    })
end

function GuildCooperateTaskPanel:OnClickTaskCells(data)

    self.data:SelectCooperateOperateItem(data)
end

GuildCooperateHelpPanal = GuildCooperateHelpPanal or DeclareMono("GuildCooperateHelpPanal", UIWFlushPanel)
function GuildCooperateHelpPanal:GuildCooperateHelpPanal()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data.member_info, func = self.FlushPanel},
        {data = self.data.cooperate_operate, func = self.FlushPanel,keys = {"help_flush"},init_call = false}
    }
end

function GuildCooperateHelpPanal:FlushPanel()
    local list = self.data:GetCooperateHelpList()
    
    self.Kong:SetActive(#list <= 0 )
    self.HelpList:SetData(list)
end

function GuildCooperateHelpPanal:OnClickGive(data)
    local task_cfg = Config.family_daily_mission_auto.mission_list[data.role_guild_task_solicit_task_id]
    local target_item = Item.Init(task_cfg.c_param1,task_cfg.c_param2)

    if Item.GetBigType(task_cfg.c_param1) == ITEM_BIG_TYPE.PET then
        TaskChainItemSel.item_id = 0
        TaskChainItemSel.pet_id = task_cfg.c_param1
		TaskChainItemSel.custom_func = function()
            GuildCtrl.SendGuildCooperateReq({
                req_type = 3,
                param1 = data.uid,
                param2 = TaskChainItemSel.cur_sel_data.index
            })
			ViewMgr:CloseView(TaskChainItemSel)
			end
        ViewMgr:OpenView(TaskChainItemSel)
    else
        local ihave = BagData.Instance:GetNumByItemId(target_item.item_id) + BagData.Instance:GetStorageNumByItemId(target_item.item_id)
        if ihave == 0 then
            PublicPopupCtrl.Instance:Center(string.format(Language.GuildCooperateTask.LackItemError,Item.GetName(target_item.item_id)))
            return 
        end
        GuildCtrl.SendGuildCooperateReq({
            req_type = 3,
            param1 = data.uid,
        })
    end 
end

function GuildCooperateHelpPanal:OnClickFetch()
    if not self.data:GetMyCooHelpDone() then 
        PublicPopupCtrl.Instance:Center(Language.GuildCooperateTask.HelpNotDone)
        return 
    end 
    GuildCtrl.SendGuildCooperateReq({
        req_type = 7,
    })
end

---------------GuildCooperateTaskCell---------------
GuildCooperateTaskCell = GuildCooperateTaskCell or DeclareMono("GuildCooperateTaskCell", UIWidgetBaseItem)
function GuildCooperateTaskCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushShow()
end

function GuildCooperateTaskCell:FlushShow()
    local task_cfg = self.data.task_id > 0 and Config.family_daily_mission_auto.mission_list[self.data.task_id] or nil
    local task_info_cfg = GuildData.Instance:GetCooperateTaskInfoCfg(self.data.task_id)

    local waiting = self.data.refresh_time > 0 and self.data.refresh_time - TimeHelper.GetServerTime() >0
    self.TaskWaiting:SetActive(waiting)
    if waiting then 
        self.WaitTimer:StampTime(self.data.refresh_time)
    end 

    self.TaskIcon.gameObject:SetActive(not waiting)
    self.TaskDiffcult.gameObject:SetActive(task_info_cfg ~= nil and not waiting)
    if task_info_cfg~= nil then 
        UH.SpriteName(self.TaskDiffcult, GuildData.Instance:GetCooperateDiffcultStr(task_info_cfg.color))
        UH.SpriteName(self.TaskIcon, task_info_cfg.mission_icon)
    end
    
    UH.SetText(self.TaskName,(task_cfg ~= nil and not waiting) and task_cfg.task_name or Language.GuildCooperateTask.WaitingForNewTask )

    self.TaskRunning:SetActive(GuildData.Instance:GetCooperateTaksIsRunning(self.data.index))
end

function GuildCooperateTaskCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

--- GuildCooperateTaskRewards ---
GuildCooperateTaskRewards = GuildCooperateTaskRewards or DeclareMono("GuildCooperateTaskRewards", UIWidgetBaseItem)
function GuildCooperateTaskRewards:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.Calls:SetData(data)
end

-- GuildCooperateHelpCell -- 
GuildCooperateHelpCell = GuildCooperateHelpCell or DeclareMono("GuildCooperateHelpCell", UIWidgetBaseItem)
function GuildCooperateHelpCell:GuildCooperateHelpCell()
    self.lang = Language.Guild
    self.guild_data = GuildData.Instance
end
function GuildCooperateHelpCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	self:SetPlayInfo()
	self:SetItemInfo()
end

function GuildCooperateHelpCell:SetPlayInfo()

	local player_name = self.data.name --"Player"
	local avatar_type = self.data.appearance.avatar_type
	local avatar_id = self.data.appearance.avatar_id
	local avatar_quality = 0

	local color_type = COLORS.Red9
	UH.Color(self.Name,color_type)
	UH.SetText(self.Name,player_name)
	-- UH.SetText(self.Help,help_times_show)
	local avatardata = {
		appearance = {avatar_type = avatar_type,avatar_id = avatar_id,avatar_quality = avatar_quality}
	}
	self.Avatar:SetData(avatardata)
end

function GuildCooperateHelpCell:SetItemInfo()
    local task_cfg = Config.family_daily_mission_auto.mission_list[self.data.role_guild_task_solicit_task_id]
    local target_item = Item.Init(task_cfg.c_param1,task_cfg.c_param2)
	self.ItemTex:SetData(target_item)
    local item_name = target_item:QuaName(true)
    
	local ihave = BagData.Instance:GetNumByItemId(target_item.item_id) + BagData.Instance:GetStorageNumByItemId(target_item.item_id)
    if target_item:IsPet() then 
        ihave = PetData.Instance:GetPetNumByItemId(target_item.item_id, true)
    end 
	local ihave_show = string.format(self.lang.JiYuanItemHave,ihave)
	UH.SetText(self.ItemName,item_name)
	UH.SetText(self.ItemHave,ihave_show)

    self.GiveInter.Interactable = self.guild_data:GetMyCooHelpDone()

    local role_id = RoleData.Instance:GetRoleId()
    self.Button1:SetActive(self.data.uid ~= role_id)
    self.GiveInter.gameObject:SetActive(role_id == self.data.uid)
    self.GiveRed:SetActive(self.guild_data:GetMyCooHelpDone())
end