NpcDialogView = NpcDialogView or DeclareView("NpcDialogView", NpcDialogConfig.ResPath .. "npc_dialog")
VIEW_DECLARE_MASK(NpcDialogView, ViewMask.None)
VIEW_DECLARE_LEVEL(NpcDialogView, ViewLevel.Lu)

function NpcDialogView:NpcDialogView()
    self.data = NpcDialogCtrl.Instance:Data()
    self.beh_data = BehaviorData.Instance
    self:FlushAutoAudioTask()
    self:FlushNotCloseDialog()
    self:RegisterGetGuideUi()
    self.is_typing = false
    self.hanle = FavorData.Instance.favor_data_change:Care(BindTool.Bind(self.NpcFavorShow, self))

end

function NpcDialogView:LoadCallback()
    SceneLogic.Instance.auto_to_npc = nil
    if self.ui_obj_npc == nil then
        self.ui_obj_npc = UIObjDrawer.New()
        self.ModelShowNpc:SetShowData(
            {
                ui_obj = self.ui_obj_npc,
                view = self,
                show_shadow = false,
            }
        )
    end
    if self.ui_obj_role == nil then
        if BATTLE_MODE.BATTLE_MODE_INITIAL_FIGHT == BattleData.Instance:BattleMode() then
            local appearance = DataHelper.TableCopy(RoleData.Instance:GetApearance())
            local co = Config.npc_demo_auto.image_id[appearance.avatar_type]
            if co then
                local weapons = string.split(co["job_"..appearance.prof_base],"|")
                appearance.special_show = {
                    show_weapon = true,
                    main_res = co.tihuan_waiguan,
                    main_color = co.color,
                    weapon_res1 = tonumber(weapons[1]),
                    weapon_res2 = weapons[2] and tonumber(weapons[2]) or nil,
                }
            end
            local main_role = UIChDrawer.New()
            main_role:SetData(appearance)
            self.ui_obj_role = main_role
        else
            self.ui_obj_role = UIChDrawer.CopyMainRoleData()
        end
        self.ModelShowRole:SetShowData(
            {
                ui_obj = self.ui_obj_role,
                view = self,
                show_shadow = false,
            }
        )
    end
    self.showed_res_id = {}
    self.is_not_auto = false
    TimeHelper:CancelTimer(self.handle_delay)
    self.handle_delay =
        TimeHelper:AddDelayTimer(
        function()
            self:AutoAccept()
        end,
        TeamConfig.AutoTime
    )
    MainOtherData.Instance:MutualViewFunc()
end

function NpcDialogView:AutoAccept()
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    self.handle_runner =
        Runner.Instance:RunUntilTrue(
        function()
            if self.RightFuncList:SetupCompleted() then
                if  SceneData.Instance:SceneId() == Config.main_catch_fox_auto.find_fox[1].scene_id and 
                    (not TaskData.Instance:GetTaskIsCompleted(Config.main_catch_fox_auto.other[1].task_id) 
                    or not TaskData.Instance:GetTaskIsCompleted(Config.main_catch_fox_auto.other[1].last_task_id)) then
                    self:OnClickBlock(true)
                    if self.RightFuncList.item_list[1] and self.RightFuncList.item_list[1].ClickEvent.enabled then
                        self.RightFuncList.item_list[1].ClickEvent:Trigger()
                    end
                end
                return true
            end
            return false
        end
    )
end

function NpcDialogView:AutoBeh()
    if self.beh_data.auto_behavior > 0 then
        TimeHelper:CancelTimer(self.timer_auto_beh)
        self.timer_auto_beh = TimeHelper:AddRunTimer(
        function()
            if 0 == self.beh_data.auto_behavior then
                TimeHelper:CancelTimer(self.timer_auto_beh)
                return
            end
            if not self.is_typing then
                self:OnClickBlock(true)
            end
        end,self.beh_data.auto_behavior,nil,false)
    elseif self.auto_audio_task > 0 then
        -- 主线支线任务自动对话，第一次检测6秒，如果语音没播完就1秒检查一次
        TimeHelper:CancelTimer(self.timer_auto_beh)
        self.timer_auto_beh = TimeHelper:AddRunTimer(
        function()
            if not self.is_typing then
                if nil ~= self.audio_handle and nil ~= self.audio_handle.AP then
                    if not AudioExtend.IsPlaying(self.audio_handle.AP) then
                        self:OnClickBlock(true)
                    else
                        TimeHelper:CancelTimer(self.timer_auto_beh)
                        self.timer_auto_beh = TimeHelper:AddRunTimer(
                        function()
                            if nil ~= self.audio_handle and nil ~= self.audio_handle.AP then
                                if not AudioExtend.IsPlaying(self.audio_handle.AP) then
                                    self:OnClickBlock(false)
                                end
                            end
                        end,1,nil,false) 
                    end
                else
                    self:OnClickBlock(true)
                end
            end
        end,self.auto_audio_task, nil,false)
    end
end

function NpcDialogView:OpenCallback()
    EventSys:Fire(GlobalEventKeys.OpenDialogView)
    self:SetPostEffectEnable(true)
    self:NpcFavorShow()
end

function NpcDialogView:OnFlush()
    if self.data.need_auto_typing then
        self:OnClickSkipAll()
    end
    self:AutoBeh()
    local cur_talk_seq_info = self.data:GetCurTalkSeqInfo()
    if cur_talk_seq_info == nil then
        return
    end
    self.talk_id = cur_talk_seq_info.talk_id
    local button_list = self.data:GetButtonList()
    self.RightObj:SetActive(false)
    self.LeftObj:SetActive(false)
    self.Content.text = self:ShowCurTalkSeqInfo(cur_talk_seq_info)

    self:SetFuncList(button_list)
    self:BehaviorAndFuncs()
    --[[ if self.talk_id == 65000 or self.talk_id == 65003 then
        local shot_id = self.talk_id == 65000 and 1 or 3
        self.shot_effect = BattleShotEffect.New(shot_id)
    end ]]
end

function NpcDialogView:NpcFavorShow()
    local npc_seq = self.data:GetCurNpcSeq()
    if FavorData.Instance:GetFavorShow(npc_seq) then
        self.Favor:SetActive(true)
        FavorData.Instance:SetCurNpcData(npc_seq)
        UH.SetText(self.FavorNum, FavorData.Instance:GetNpcFavorLevel())
    else
        self.Favor:SetActive(false)
    end
    self.FavorNpcPanel:SetActive(FavorData.Instance.favor_data_change.is_show)
    self.RightObj:SetActive(not FavorData.Instance.favor_data_change.is_show)
end

function NpcDialogView:SetFuncList(button_list)
    local last_talk_seq_info = self.data:GetLastTalkSeqInfo()
    local is_npc_talk = 0 ~= last_talk_seq_info.talker_name or 4999 == last_talk_seq_info.talk_id
    if last_talk_seq_info.is_npc_talk == true then
        is_npc_talk = true
    end
    self.RightFuncList:SetData(is_npc_talk and button_list or {})
    self.LeftFuncList:SetData(is_npc_talk and {} or button_list)
    -- self.RightObj:SetActive(is_npc_talk)
    -- self.LeftObj:SetActive(not is_npc_talk)
end

function NpcDialogView:OnClickBlock(is_auto)
    self.is_not_auto = not is_auto
    if not is_auto then
        self:AutoBeh()
    end
    if self.need_close then
        if self.data.not_block_close_func == true then
            local custom_end_func = self.data:CustomEndFunc()
            if custom_end_func then
                custom_end_func()
            end
            self.data:SetCustomEndFunc(nil)
            return
        end
        if self.data.not_block_close == true then
            return
        end
        local cur_task_info = self.data:CurTaskInfo()
        if is_auto and cur_task_info ~= nil and cur_task_info:IsRequestDoing() then
            TimeHelper:CancelTimer(self.timer_auto_beh)
            TimeHelper:CancelTimer(self.timer_auto_beh_delay)
            self.timer_auto_beh_delay =
                TimeHelper:AddDelayTimer(
                function()
                    ViewMgr:CloseView(NpcDialogView)
                end,
                Config.bounty_tasklist_auto.other[1].auto_battle
            )
        else
            ViewMgr:CloseView(NpcDialogView)
        end
    else
        self:OnClickSkip()
    end
end

function NpcDialogView:CloseCallback()
    NpcDialogCtrl.Instance.block_flush = nil
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    --[[ for _, value in pairs(ClipGeter.cache_controller) do
        value:set_Item(AnimClip.Loop, nil)
        value:set_Item(AnimClip.LoopPD, nil)
    end ]]
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay2)
    TimeHelper:CancelTimer(self.handle_delay3)
    TimeHelper:CancelTimer(self.handle_delay4)
    TimeHelper:CancelTimer(self.handle_delay5)
    if self.wait_play_show_handle ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.wait_play_show_handle)
        self.wait_play_show_handle = nil
    end

    EventSys:Fire(GlobalEventKeys.CloseDialogView)
    self:SetPostEffectEnable(false)
    TimeHelper:CancelTimer(self.timer_auto_beh)
    TimeHelper:CancelTimer(self.timer_auto_beh_delay)

    if NpcDialogConfig.FuncState.done ~= self.data.func_state and not self.data.block_dofunc then
        self:DoFunc()
    end
    if self.shot_effect then
        --[[ self.shot_effect:SetEndFunc(
            function()
                local custom_end_func = self.data:CustomEndFunc()
                if custom_end_func then
                    custom_end_func()
                end
                self.data:SetCustomEndFunc(nil)
                self.shot_effect:DeleteMe()
                self.shot_effect = nil
            end
        )
        self.shot_effect:Play() ]]
    else
        local custom_end_func = self.data:CustomEndFunc()
        if custom_end_func then
            custom_end_func()
        end
        self.data:SetCustomEndFunc(nil)
    end
    if self.ui_obj_role then
        self.ui_obj_role:DeleteMe()
        self.ui_obj_role = nil
    end

    if self.ui_obj_npc then
        self.ui_obj_npc:DeleteMe()
        self.ui_obj_npc = nil
    end
    if self.bangjia_obj_npc then
        self.bangjia_obj_npc:DeleteMe()
        self.bangjia_obj_npc = nil
    end

    self:StopNpcAudio()

    --[[ if self.model_obj ~= nil then
        UnityGameObj.Destroy(self.model_obj)
    end ]]

    local npc_type_t = NpcHelper.NpcTypesById(self.data.cur_npc_talk_list.talker)
    if npc_type_t.normal then
        NpcDialogCtrl.Instance:SendRoleTalkToNpc()
    end

    self.showed_res_id = {}

    MainOtherCtrl.Instance:WaitViewFunc()
    self.data.cahce_chck_data = nil

    local sync_end_func = self.data:SyncEndFunc()
    if sync_end_func then
        sync_end_func()
    end
    self.data:SetSyncEndFunc(nil)
    FavorData.Instance.favor_data_change:Uncare(self.hanle)
end

function NpcDialogView:DoFunc()
    local cur_task_info = self.data:CurTaskInfo()
    if cur_task_info ~= nil then
        local t_st = cur_task_info:State()
        if cur_task_info.task_type == TaskConfig.TaskType.anecdote
            and t_st == TaskConfig.TaskState.undo then 
            self.data:SetCurTaskInfo(nil)
            return 
        end
        if cur_task_info.task_type == TaskConfig.TaskType.task_chain then
            cur_task_info:AutoNpcDialogChain()
            self.data:SetCurTaskInfo(nil)
            return 
        end 
        if t_st == TaskConfig.TaskState.done then
            if cur_task_info.task_type == TaskConfig.TaskType.mainline and cur_task_info.info.manual_status == 1 and not IS_STANDALONE() then
                cur_task_info:MainLineTaskNeedClick(true)
            else                
                cur_task_info:RequestCommit()
            end
        elseif t_st == TaskConfig.TaskState.undo then
            cur_task_info:RequestAccept()
        elseif t_st == TaskConfig.TaskState.doing then
            cur_task_info:RequestDoing(self.is_not_auto)
        end
    end
    self.data:SetCurTaskInfo(nil)
end

function NpcDialogView:SetPostEffectEnable(value)
    local story_state, cur_story = StoryData.Instance:StoryState()
    if not StoryData.Instance:IsPlaying() then
        if nil == self.post_effect then
            local main_camera = UnityGameObj.FindGameObjectWithTag("MainCamera")
            -- self.post_effect = main_camera:GetComponent(typeof(Crystal.PostEffectDepthOfField))
            self.post_effect = main_camera:GetComponent(typeof(Crystal.PostEffectGaussianBlur))
        end
        self.post_effect.enabled = value
    else
        cur_story.post_effect_state = value
    end
end

function NpcDialogView:OnClickSkip()
    local cur_talk_seq_info = self.data:GetCurTalkSeqInfo(true)
    if not self.ContentT:Skip() then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        if cur_talk_seq_info then
            self.data.cur_talk_seq = self.data.cur_talk_seq + 1
            self.is_typing = true
            self.ContentT:ShowContent(self:ShowCurTalkSeqInfo(cur_talk_seq_info), 0.02)
        else
            self.data.cur_talk_seq = 1
        end
    else
        self:BehaviorAndFuncs()
    end
end

function NpcDialogView:BehaviorAndFuncs()
    self.is_typing = false
    self.need_close = false
    if not self.data:GetCurTalkSeqInfo(true) then
        self.ArrowObj:SetActive(false)
        self.TopRightObj:SetActive(false)
        self.need_close = true
    else
        self.ArrowObj:SetActive(true)
        self.TopRightObj:SetActive(true)
    end
end

function NpcDialogView:ShowCurTalkSeqInfo(cur_talk_seq_info)
    self.data:SyncTalkInfo(cur_talk_seq_info)
    self:PlayNpcAudio(cur_talk_seq_info)
    self:ShowButtons(cur_talk_seq_info)
    if NpcDialogConfig.FuncState.undo == self.data.func_state and 1 == cur_talk_seq_info.later_talk then
        self.data.func_state = NpcDialogConfig.FuncState.ready
    end
    if NpcDialogConfig.FuncState.ready == self.data.func_state and not self.data.block_dofunc then
        self.data.func_state = NpcDialogConfig.FuncState.done
        self:DoFunc()
    end

    local is_npc_talk = 0 ~= cur_talk_seq_info.talker_name
    if cur_talk_seq_info.avatar_type and cur_talk_seq_info.avatar_type ~= "" then
        is_npc_talk = false
    end
    if 4999 == cur_talk_seq_info.talk_id and self.data.default_npc then
        is_npc_talk = true
    end
    -------------------心魔代码-------------------------------------
    if self.xin_mo_show == true then
        self.ui_obj_role:SetData(RoleData.Instance:GetApearance())
        self.xin_mo_show = false
    end
    local rolev2 = Vector2.New(217, 0)

    rolev2:Set(217, 0)
    UH.AnchoredPosition(self.RoleRect, rolev2)
    
    if FreedomCode.NpcViewXinMo == true and cur_talk_seq_info.talker_name == Language.Common.XinMo then
        local temp = DataHelper.TableCopy(cur_talk_seq_info)
        temp.advance_time = 1
        temp.color = 100
        temp.avatar_type = RoleData.Instance:GetRoleAvatarType()
        temp.weapon_id = 0
        is_npc_talk = false
        rolev2:Set(-217, 0)
        UH.AnchoredPosition(self.RoleRect, rolev2)
        self.NpcNameObj:SetActive(not is_npc_talk)
        self.RoleNameObj:SetActive(is_npc_talk)
        self.ModelShowNpcObj:SetActive(is_npc_talk)
        self.ModelShowRoleObj:SetActive(not is_npc_talk)
        local clip1 = Format("%s_xinmoidle", temp.avatar_type)
        local clip2 = Format("%s_xinmoidle_piaodai", temp.avatar_type)
        ClipGeter.GetClip(clip1)
        ClipGeter.GetClip(clip2)
        self.ui_obj_role:SetData(temp)
        TimeHelper:CancelTimer(self.handle_delay4)
        self.handle_delay4 = TimeHelper:AddDelayFrameTimer(function ()
            self.ui_obj_role:AnimParams({clip1, clip2})
            self.ui_obj_role:PlayAnim(ChAnimType.XinMoIdle)
        end,5)
        is_npc_talk = true
        self.xin_mo_show = true
    elseif cur_talk_seq_info.type == NpcDialogConfig.TalkSpecial.UTAShow then
        self.ModelShowNpcObj:SetActive(false)
        self.ModelShowRoleObj:SetActive(true)
        self.RightObj:SetActive(true)
        self.LeftObj:SetActive(false)
        local appearance = cur_talk_seq_info.appearance
        self.ui_obj_role:SetData(appearance)
    else
        self.NpcNameObj:SetActive(is_npc_talk)
        self.RoleNameObj:SetActive(not is_npc_talk)
        self.ModelShowNpcObj:SetActive(is_npc_talk)
        self.ModelShowRoleObj:SetActive(not is_npc_talk)
    end

    TimeHelper:CancelTimer(self.handle_delay2)
    TimeHelper:CancelTimer(self.handle_delay3)
    --[[ if FreedomCode.NpcViewXinMo == true and cur_talk_seq_info.talk_id == 33 then
        if not self.IsXinMoPlay then 
            self.EffectTool:Play("10010851")
            AudioMgr:PlayEffect(AudioType.UI, "story_xinmo")
            self.IsXinMoPlay = true
        end 
        local appearance = DataHelper.TableCopy(RoleData.Instance:GetApearance())
        appearance.weapon_id = 0
        local clip1 = Format("%s_shoushang", appearance.avatar_type)
        local clip2 = Format("%s_shoushang_piaodai", appearance.avatar_type)
        ClipGeter.GetClip(clip1)
        ClipGeter.GetClip(clip2)
        if cur_talk_seq_info.talk_seq == 1 then
            self.ui_obj_role:AnimParams({clip1, clip2})
            self.ui_obj_role:SetData(appearance)
            self.handle_delay2 =
                TimeHelper:AddDelayTimer(
                function()
                    self.ui_obj_role:PlayAnim(ChAnimType.ShouShang)
                end,
                2.5
            )
        elseif cur_talk_seq_info.talk_seq == 3 then
            self.ui_obj_role:AnimParams({clip1, clip2})
            self.ui_obj_role:SetData(appearance)
            self.handle_delay3 =
                TimeHelper:AddDelayTimer(
                function()
                    self.ui_obj_role:PlayAnim(ChAnimType.ShouShang)
                end,
                2.5
            )
        end
    end ]]
    --------------------------------------------------------
    -------------------茉莉绑架代码-------------------------------------
    local info = self.data:GetSpTalkInfo(cur_talk_seq_info)
    if FreedomCode.NpcViewBangJia == true then
        if info and info.type == NpcDialogConfig.TalkSpecial.KunBang then
            ClipGeter.GetClip(info.anim)
            if info.talk_seq == cur_talk_seq_info.talk_seq then
                self.bangjia_obj_npc = UIObjDrawer.New()
                self.ModelShowNpc:AddShowData(
                    {{ui_obj = self.bangjia_obj_npc, pos = Vector3.New(1.318, -1.142, -0.65)}}
                )
                self.bangjia_obj_npc:AnimParams({info.anim})
                self.bangjia_obj_npc:SetData(DrawerHelper.GetNpcPath(222))
                self.bangjia_obj_npc:PlayAnim(ChAnimType.KunBang)
            else
                if self.bangjia_obj_npc then
                    self.bangjia_obj_npc:DeleteMe()
                    self.bangjia_obj_npc = nil
                end
            end
        elseif info and info.type == NpcDialogConfig.TalkSpecial.KunBang2 then
            ClipGeter.GetClip(info.anim)
            if info.talk_seq == cur_talk_seq_info.talk_seq then
                self.bangjia_show = true
            else
                if self.bangjia_obj_npc then
                    self.bangjia_obj_npc:DeleteMe()
                    self.bangjia_obj_npc = nil
                end
            end
        elseif info and info.type == NpcDialogConfig.TalkSpecial.KunBang3 then
            ClipGeter.GetClip(info.anim)
            if info.talk_seq == cur_talk_seq_info.talk_seq then
                self.bangjia_show = true
            else
                if self.bangjia_obj_npc then
                    self.bangjia_obj_npc:DeleteMe()
                    self.bangjia_obj_npc = nil
                end
            end
        end
    end
    --------------------------------------------------------
    -------------------1016战斗对话--------------------------
    if info and info.type == NpcDialogConfig.TalkSpecial.ShouShang then
        TimeHelper:CancelTimer(self.handle_delay5)
        local appearance = DataHelper.TableCopy(RoleData.Instance:GetApearance())
        appearance.weapon_id = 0
        if BATTLE_MODE.BATTLE_MODE_INITIAL_FIGHT == BattleData.Instance:BattleMode() then
            local co = Config.npc_demo_auto.image_id[appearance.avatar_type]
            if co then
                local weapons = string.split(co["job_"..appearance.prof_base],"|")
                appearance.special_show = {
                    show_weapon = false,
                    main_res = co.tihuan_waiguan,
                    main_color = co.color,
                    weapon_res1 = tonumber(weapons[1]),
                    weapon_res2 = weapons[2] and tonumber(weapons[2]) or nil,
                }
            end
        end
        local clip1 = Format("%s_shoushang", appearance.avatar_type)
        local clip2 = Format("%s_shoushang_piaodai", appearance.avatar_type)


        if info.talk_seq == cur_talk_seq_info.talk_seq then
            self.ui_obj_role:AnimParams({clip1, clip2})
            self.ui_obj_role:SetData(appearance)
            -- self.handle_delay5 =
            --     TimeHelper:AddDelayTimer(
            --     function()
            --         self.ui_obj_role:PlayAnim(ChAnimType.ShouShang)
            --     end,
            --     0.2
            -- )
        end
        local clip_load_flag = {clip1 = false,clip2 = false}
        ClipGeter.GetClip(clip1,function() 
            clip_load_flag.clip1 = true
            if clip_load_flag.clip2 then
                if info.talk_seq == cur_talk_seq_info.talk_seq and self.ui_obj_role then
                    self.ui_obj_role:PlayAnim(ChAnimType.ShouShang)
                end 
            end
        end)
        ClipGeter.GetClip(clip2,function() 
            clip_load_flag.clip2 = true
            if clip_load_flag.clip1 then
                if info.talk_seq == cur_talk_seq_info.talk_seq and self.ui_obj_role then
                    self.ui_obj_role:PlayAnim(ChAnimType.ShouShang)
                end                
            end
        end)

    end

    if is_npc_talk then
        local npc_show_name = ""
        local res_id = 0
        -- 特殊逻辑 4999为动态npc默认对话id talker配的是玩家但是需要显示npc名字和模型
        if 4999 == cur_talk_seq_info.talk_id then
            npc_show_name = self.data.npc_list_config[self.data.default_npc].name
			res_id = self.data.npc_list_config[self.data.default_npc].res_id
        else
            npc_show_name = cur_talk_seq_info.talker_name
            res_id = cur_talk_seq_info.npc_res_id
            if cur_talk_seq_info.npc_res_id == nil then
                if self.data.npc_list_config[cur_talk_seq_info.talker] then
                    res_id = self.data.npc_list_config[cur_talk_seq_info.talker].res_id
                else
                    LogDG("npc 对话形象获取不到",cur_talk_seq_info.talker,"尝试获取怪物配置，可能导致形象不对")
                    local config = Cfg.MonsterById(cur_talk_seq_info.talker)
                    if config then
                        res_id = config.res_id
                    end
                end
            end
            
        end
        self.NpcName.text = Format("<color=#%s>%s</color>", COLORSTR.Yellow2, npc_show_name)
        if self.wait_play_show_handle ~= nil then
            Runner.Instance:RemoveRunUntilTrue(self.wait_play_show_handle)
            self.wait_play_show_handle = nil
        end
        self.ui_obj_npc:SetData(DrawerHelper.GetNpcPath(res_id))
        if type(res_id) == "string" or (res_id <= 9999 and res_id >= 1000) then
            self.ui_obj_npc:PlayAnim(ChAnimType.Idle)
        else
            if self.showed_res_id[res_id] then
                if self.bangjia_show == true then
                    self.ui_obj_npc:PlayAnim(ChAnimType.KunBang)
                    self.bangjia_show = false
                else
                    self.ui_obj_npc:PlayAnim(ChAnimType.Idle)
                end
            else
                self.showed_res_id[res_id] = true
                if self.bangjia_show == true then
                    self.ui_obj_npc:PlayAnim(ChAnimType.KunBang)
                    self.bangjia_show = false
                else
                    local play_show_func = function()
                            if self.ui_obj_npc:HasAnim(ChAnimType.Show2) then       --NPC框优先播Show2动画
                                self.ui_obj_npc:PlayAnim(ChAnimType.Show2)    
                            elseif self.ui_obj_npc:HasAnim(ChAnimType.Show) then
                                self.ui_obj_npc:PlayAnim(ChAnimType.Show)
                            end
                        end
                    if self.ui_obj_npc.main_body == nil or self.ui_obj_npc.main_anim == nil or 
                        self.ui_obj_npc.main_body:IsLoading() or self.ui_obj_npc.main_anim:IsOverriderLoading() then
                        self.wait_play_show_handle = Runner.Instance:RunUntilTrue(function() 
                            if self.ui_obj_npc.main_body ~= nil and self.ui_obj_npc.main_anim ~= nil and
                                not self.ui_obj_npc.main_body:IsLoading() and 
                                not self.ui_obj_npc.main_anim:IsOverriderLoading() then
                                self.wait_play_show_handle = nil
                                play_show_func()
                                return true
                            end
                            return false
                        end)
                    else
                        play_show_func()
                    end
                end
            end
        end
    else
        if type(cur_talk_seq_info.avatar_type) == "number" and cur_talk_seq_info.avatar_type ~= "" then
            self.ui_obj_role:SetData(cur_talk_seq_info)
            UH.SetText(self.RoleName,Format(Language.NpcDialog.RoleName, cur_talk_seq_info.talker_name))
        else
            UH.SetText(self.RoleName,Format(Language.NpcDialog.RoleName,RoleData.Instance:GetRoleName()))
        end
    end
    local str = cur_talk_seq_info.talk_text
    if string.find(str, "<player_name>") then
        str = string.gsub(str, "<player_name>", RoleData.Instance:GetRoleName())
    end
    if string.find(str, "<player_sex>") then
        local role_sex = RoleData.Instance:GetRoleSex()
        local sex_ta = Language.NpcDialog.SexTa[role_sex]
        str = string.gsub(str, "<player_sex>", sex_ta)
    end
    if cur_talk_seq_info.appearance then
        self.Player:SetData(cur_talk_seq_info)
    end
    self.Player:SetObjActive(cur_talk_seq_info.appearance ~= nli)
    return RichTextHelper.ParseAllFunc(str, true, RichTextFormat.FormatNpcBubbleSpecial)
end

function NpcDialogView:OnClickSkipAll()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    local cur_talk_seq_info = self.data:GetCurTalkSeqInfo(false, true)
    self.Content.text = self:ShowCurTalkSeqInfo(cur_talk_seq_info)
    self:BehaviorAndFuncs()
    -- if self.IsNotCloseDialog == true then
        local list = self.data:GetButtonList() or {}
        if #list > 0 then
            TimeHelper:CancelTimer(self.timer_auto_beh)
            return
        end
    -- end
    ViewMgr:CloseView(NpcDialogView)
end

function NpcDialogView:ShowButtons(talk_seq_info)
    if self.data:GetLastTalkSeqInfo() ~= talk_seq_info then
        return
    end
    local is_npc_talk = 0 ~= talk_seq_info.talker_name or 4999 == talk_seq_info.talk_id
    self.RightObj:SetActive(is_npc_talk)
    self.LeftObj:SetActive(not is_npc_talk)
end

function NpcDialogView:OnClickItem(data)
    TimeHelper:CancelTimer(self.handle_delay)
    self.is_not_auto = true
    local detail_info = data.detail_info
    if detail_info and detail_info.act_type then
        if ActivityData.Instance:IsNoEnoughLevel(detail_info.act_type) then
            return
        end
    end
    if detail_info then
        local old_data = self.data.cur_sel_btn:Val()
        local old_info = {}
        if old_data.detail_info then
            old_info = old_data.detail_info
        end
        self.DetailObjs[detail_info.type]:SetSiblingIndex(data.btn_index)
        if old_info.type and old_info ~= detail_info then
            self.DetailObjs[old_info.type]:SetActive(false)
        end
        local value = detail_info ~= old_info
        self.data.cur_sel_btn:Set(detail_info ~= old_info and data or {type_old = detail_info.type})
    else
        data:Exe()
    end
end

function NpcDialogView:PlayNpcAudio(talk_seq_info)
    self:StopNpcAudio()
    if talk_seq_info and not StringIsEmpty(talk_seq_info.audio) then
        self.audio_handle = AudioMgr:PlayEffectNpcHandle(AudioType.Npc, talk_seq_info.audio .. (0 == talk_seq_info.talker and Format("_%s", RoleData.Instance:GetRoleAvatarType()) or ""),
        not StringIsEmpty(tostring(talk_seq_info.audio_day)) and talk_seq_info.audio_day or 0)
    end
end

function NpcDialogView:StopNpcAudio()
    if nil ~= self.audio_handle then
        AudioMgr:StopEffectHandle(self.audio_handle)
        self.audio_handle = nil
    end
end

--是否自动对话 任务 AutoAudioTask
function NpcDialogView:FlushAutoAudioTask()
    if self.data.need_click_close then
        self.auto_audio_task =  0
    else
        local cur_task_info = self.data:CurTaskInfo()
        self.auto_audio_task = (RoleData.Instance:GetRoleLevel() <= GuideData.Instance:AutoTalk() or (nil ~= cur_task_info and cur_task_info:AutoAudioTask())) and NpcDialogConfig.AUTO_AUDIO_TASK or 0
    end
    self.data.need_click_close = false
end

--对话中是否不要关闭界面 任务 IsNotCloseDialog
function NpcDialogView:FlushNotCloseDialog()
    self.IsNotCloseDialog = false
    local cur_task_info = self.data:CurTaskInfo()
    if cur_task_info and cur_task_info.IsNotCloseDialog then
        self.IsNotCloseDialog = cur_task_info:IsNotCloseDialog()
    end
end

function NpcDialogView:RegisterGetGuideUi()
    GuideManager.Instance:RegisterGetGuideUi("GuildTaskNPC",function()
        if self.RightFuncList:SetupCompleted() then
            local item = self.RightFuncList.item_list[1]
            if item and item.ListTran.localPosition.x == 0 then
                return self.RightFuncList.item_list[1].ClickEvent
            end
        end
        return nil, nil            
    end)
end

function NpcDialogView:OpenFavorView()
   FavorData.Instance.favor_data_change.is_show=true
end


NpcDialogViewRight = NpcDialogViewRight or DeclareMono("NpcDialogViewRight", UIWFlushPanel)

function NpcDialogViewRight:NpcDialogViewRight()
    self.data = NpcDialogCtrl.Instance:Data()

    self.data_cares = {
        {data = self.data.cur_sel_btn, func = self.FlushDetails, init_call = false}
    }
end

function NpcDialogViewRight:FlushDetails()
    local data = self.data.cur_sel_btn
    if data.type_old then
        self.DetailObjs[data.type_old]:SetActive(false)
        self.ListRect.sizeDelta = Vector2.New(435, 360)
    else
        local detail_info = data.detail_info
        self.DetailObjs[detail_info.type]:SetActive(true)
        self.ListRect.sizeDelta = Vector2.New(435, 500)
    end
end

--data:NpcDialogBtnData
NpcDialogViewFuncItem = DeclareMono("NpcDialogViewFuncItem", UIWidgetBaseItem)

function NpcDialogViewFuncItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Name.text = data:DisStr()

    local icon_sp = data.IconSp and data:IconSp() or nil
    local is_grey = data.IsGrey and data:IsGrey() or false
    if icon_sp then
        self.IconSp.SpriteName = icon_sp
        self.IconSp:SetObjActive(true)
    else
        self.IconSp:SetObjActive(false)
    end
    if self.ItemInter then
        self.ItemInter.Interactable = not is_grey
    end
end

----------------------------NpcDialogPlayerItem----------------------------
NpcDialogPlayerItem = NpcDialogPlayerItem or DeclareMono("NpcDialogPlayerItem", UIWidgetBaseItem)

function NpcDialogPlayerItem:SetData(data)
    UH.SetText(self.Name,Format(Language.NpcDialog.RoleName,data.name))
    self.List:SetData(NpcDialogCtrl.Instance:Data():GetButtonList())
    for i=1,self.HideList:Length() do
        self.HideList[i]:SetActive(false)
    end
    for i=1,self.ShowList:Length() do
        self.ShowList[i]:SetActive(true)
    end
    self:FlushModelUIShow(data.appearance)
end

function NpcDialogPlayerItem:FlushModelUIShow(appearance)
    if self.ModelGame == nil then
        self.ModelGame = UIChDrawer.CreateByAppearance(appearance)
        self.ModelUIShow:SetShowData({ui_obj = self.ModelGame,view = NpcDialogView,show_shadow = false})
    else
        self.ModelGame:SetData(appearance)
    end
end

function NpcDialogPlayerItem:OnDestroy()
    Delete(self,[[ModelGame]])
end