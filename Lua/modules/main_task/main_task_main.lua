
MainTaskFlushPanel = MainTaskFlushPanel or DeclareMono("MainTaskFlushPanel", UIWFlushPanel)
-- ChapterNum ==== Text
-- ChapterName ==== Text
-- ChapterDesc1 ==== Text
-- ChapterDesc2 ==== Text
-- PathObjs ==== Array[gameObject]
-- PathCells ==== Array[MainTaskPathCell]

function MainTaskFlushPanel:MainTaskFlushPanel()
    self.data = MainTaskData.Instance

    self.data_cares = {
        {data = self.data.cur_view_data,func = self.FlushMainPanel,keys =  {"page"}},
        {data = self.data.cur_view_data,func = self.FlushMainPanel,keys =  {"is_flush"}},
        {data = self.data.chapter_info,func = self.FlushMainPanel,keys = {"chapter_reward_flag"}},
        {data = self.data.cur_view_data,func = self.FlushMainPanel,keys = {"mode_type"}},
        {data = self.data.cur_view_data,func = self.FlushPreView,keys = {"show_pre_flag"}},
        {data = self.data.cur_view_data,func = self.FlushPreView,keys = {"pre_data_list"}},
    }

    -- 启动时设定页面为1,模式切换为1（普通
    self.data.cur_view_data.is_flush = 1
    self.data:SetViewPage(self.data:GetQuickLoclPage() ~= 0 and self.data:GetQuickLoclPage() or 1)
    self.data:SetViewMode(self.data:GetQuickLoclMode() ~= nil and self.data:GetQuickLoclMode() or 1)

    self.is_init = false
    self.map_temp_group = {}

    GuideManager.Instance:RegisterGetGuideUi("BtnEliteChange", 
    function()
        return self.ButtonEliteObj , function() 
            self:OnClickMode(2)
        end 
    end)

    GuideManager.Instance:RegisterGetGuideUi("FirstEliteBoss", 
    function()
        local operate_pos = self.PathCells[1].BossList[1]

        return operate_pos.EliteObj , function() 
            self.PathCells[1].BossList[1]:OnClick(0)
        end 
    end)
end 

-- 刷新界面信息
function MainTaskFlushPanel:FlushMainPanel()
    -- 加载章节描述
    local cur_page_info = self.data:GetChapterAward()
    local cur_page_desc = string.split(cur_page_info.chapter_describe,"#")

    self.ChapterNum.text = cur_page_info.chapter_seq
    self.ChapterName.text = cur_page_info.chapter_name
    self.ChapterDesc1.text = cur_page_desc[1]
    self.ChapterDesc2.text = cur_page_desc[2]

    -- 加载当前章节的地图信息
    self:FLushMapShow()

    -- 加载当前章节的怪物信息
    self.PathCells[1]:SetData(self.data:GetChapeterMonster())

    -- 界面细节
    self.ButtonLeft:SetActive(self.data:GetViewPage() ~= 1)
    self.ButtonRight:SetActive(self.data:GetViewPage() < self.data.max_monster_page ) 

    local cur_mode_type = self.data:GetViewMode()
    if cur_mode_type == 1 then
        self.ButtonNomal.localScale =  Vector3.New(1, 1, 1)
        self.ButtonElite.localScale =  Vector3.New(0.9, 0.9, 1)
        UH.SpriteName(self.ButtonNomalSp, "nomal_select")
        UH.SpriteName(self.ButtonEliteSp, "elite_button")
        UH.LocalPosG(self.ButtonNomalText, Vector3.New(0, -88, 0))
        UH.LocalPosG(self.ButtonEliteText, Vector3.New(-2, -66, 0))
        self.EliteBg:SetActive(false)
        self.ChapterRewardObj:SetActive(true)
    else 
        self.ButtonNomal.localScale =  Vector3.New(0.9, 0.9, 1)
        self.ButtonElite.localScale =  Vector3.New(1, 1, 1)
        UH.SpriteName(self.ButtonNomalSp, "nomal_button")
        UH.SpriteName(self.ButtonEliteSp, "elite_select")
        UH.LocalPosG(self.ButtonNomalText, Vector3.New(-2, -66, 0))
        UH.LocalPosG(self.ButtonEliteText, Vector3.New(0, -88, 0))
        self.EliteBg:SetActive(true)
        self.ChapterRewardObj:SetActive(false)
    end 

    if self.red_point == nil then 
        self.red_point = UiRedPoint.New(self.ChapterRewardObj, Vector3.New(107, 64), false)
    end 

    if self.red_point_elitebutton == nil then 
        self.red_point_elitebutton = UiRedPoint.New(self.EliteRedBtn, Vector3.zero, false)
    end 

    if self.red_point_leftbutton == nil then 
        self.red_point_leftbutton = UiRedPoint.New(self.EliteRedLeft, Vector3.zero, false)
    end 

    if self.red_point_rightbutton == nil then
        self.red_point_rightbutton = UiRedPoint.New(self.EliteRedRight, Vector3.zero, false)
    end 

    local num = (self.data:GetChapterRewardIsGot(self.data:GetViewPage())and self.data:GetShouldBeChange() ) and 1 or 0
    self.red_point:SetNum(num)
    self.ChapterRedEff:SetActive(num > 0)

    self.data:ReflushEliteUnLockSave( cur_page_info.seq )
    self.data:ReflushEliteUnLockSave( cur_page_info.seq - 1 )
    self.data:ReflushEliteUnLockSave( cur_page_info.seq + 1 )

    local num_btn = self.data:GetEliteUnLockSave(cur_page_info.seq)and 1 or 0
    self.red_point_elitebutton:SetNum(num_btn)

    local flag_left_chapter_num = self.data:GetChapterRewardIsGot(cur_page_info.seq - 1) and  self.data:GetShouldBeChange(cur_page_info.seq - 1)
    local num_left = (self.data:GetEliteUnLockSave(cur_page_info.seq - 1) or flag_left_chapter_num ) and 1 or 0
    self.red_point_leftbutton:SetNum(num_left)

    local flag_right_chapter_num = self.data:GetChapterRewardIsGot(cur_page_info.seq + 1) and  self.data:GetShouldBeChange(cur_page_info.seq + 1)
    local num_right = (self.data:GetEliteUnLockSave(cur_page_info.seq + 1) or flag_right_chapter_num ) and 1 or 0
    self.red_point_rightbutton:SetNum(num_right)

    self.ButtonEliteInter.Interactable = self.data:GetShouldBeUnlockElite()
    self.ButtonEliteLock:SetActive(not self.data:GetShouldBeUnlockElite())
    local fun_flag = FunOpen.Instance:GetFunIsOpened(Mod.MainTask.EliteLock)
    self.ButtonEliteObj:SetActive(fun_flag)

    if cur_page_info.seq == 10 then 
        self.EliteFirstObj:SetActive(false)
        self.EliteSecondObj:SetActive(false)
        self.EliteTenObj:SetActive(true)
    elseif cur_page_info.seq < 10 then 
        self.EliteFirstObj:SetActive(false)
        self.EliteTenObj:SetActive(false)
        self.EliteSecondObj:SetActive(true)
        UH.SpriteName(self.EliteSecond, "_Loc_"..cur_page_info.seq)
    elseif cur_page_info.seq > 10 and cur_page_info.seq < 20 then 
        self.EliteFirstObj:SetActive(false)
        self.EliteTenObj:SetActive(true)
        self.EliteSecondObj:SetActive(true)
        UH.SpriteName(self.EliteSecond, "_Loc_"..math.floor(cur_page_info.seq%10))
    elseif cur_page_info.seq > 20 and cur_page_info.seq %10 ~= 0 then 
        self.EliteFirstObj:SetActive(true)
        self.EliteTenObj:SetActive(true)
        self.EliteSecondObj:SetActive(true)
        UH.SpriteName(self.EliteSecond, "_Loc_"..math.floor(cur_page_info.seq%10))
        UH.SpriteName(self.EliteFirst, "_Loc_"..math.floor(cur_page_info.seq/10))
    elseif cur_page_info.seq > 10 and cur_page_info.seq %10 == 0 then 
        self.EliteFirstObj:SetActive(true)
        self.EliteTenObj:SetActive(true)
        self.EliteSecondObj:SetActive(false)
        UH.SpriteName(self.EliteFirst, "_Loc_"..math.floor(cur_page_info.seq/10))
    end 

    self.ChapterRewInter.Interactable = self.data:GetChapterRewardIsGot(self.data:GetViewPage())
    self.ChapterRewIsGot:SetActive(not self.ChapterRewInter.Interactable)


    if not self.is_init then 
        if self.data:GetIsUnLock() then 
            self.elite_show_handle = TimeHelper:AddDelayTimer(function ()
                if self.EliteUnlockShow then 
                    self.EliteUnlockShow:SetActive(self.data:GetIsUnLock())
                end 
            end, 1)

            self.elite_effect_show_handle = TimeHelper:AddDelayTimer(function ()
                if self.EffectTool and self.data:GetIsUnLock() then 
                    self.EffectTool:Play(10010340)
                end 
            end,1.5)
        end 

        self.boss_effect_handle = TimeHelper:AddDelayTimer(function ()
            if self.PathCells[1] then 
                self.PathCells[1]:FlushBossEff()
            end 
        end,0.5)

        if self.data:GetIsUnLock() then 
            if self.close_event then
                TimeHelper:CancelTimer(self.close_event)
                self.close_event = nil
            end

            self.close_event = TimeHelper:AddDelayTimer(function ()
                self:CloseUnlockView()
            end, 10)
        end 

        self.is_init = true
    end 
end 

function MainTaskFlushPanel:FlushPreView()

    self.PreViewObj:SetActive(self.data:IsShowRewardPreView() and not (self.data:GetShouldBeChange() and MainTaskData.Instance:GetPreViewLocal() == -1) )
    self.ChapterRewardView:SetActive(self.data:IsShowRewardPreView() and self.data:GetShouldBeChange() 
        and self.data:GetChapterRewardIsGot(self.data:GetViewPage()) )

    if self.data:IsShowRewardPreView() then 
        self.RewardList:SetData(self.data:GetRewardPreViewList())
        self.RewardGetList:SetData(self.data:GetRewardPreViewList())
    end 

    local pos_xuhao = MainTaskData.Instance:GetPreViewLocal()
    if pos_xuhao == nil or pos_xuhao == -1 then 
        local lock_pos = self.gameObject.transform:InverseTransformPoint(self.PreViewPos.transform.position)
        UH.LocalPosG(self.PreViewObj, lock_pos)
    else 
        local real_pos = 1
        -- if real_pos == 0 then real_pos = 7 end 
        local list_ = self.data:GetChapeterMonster()

        for k,v in pairs(list_) do 
            if pos_xuhao == v.config.xuhao then 
                real_pos = v.config.step
            end 
        end 

        local lock_pos = self.gameObject.transform:InverseTransformPoint(self.PathCells[1].PreViewPosList[real_pos].transform.position)
        UH.LocalPosG(self.PreViewObj, lock_pos)
    end 
end

function MainTaskFlushPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetRewardPreView(false,{})
    self.data:ClearQuickLocal()

    if self.close_event then
        TimeHelper:CancelTimer(self.close_event)
        self.close_event = nil
    end

    if self.elite_show_handle then
        TimeHelper:CancelTimer(self.elite_show_handle)
        self.elite_show_handle = nil
    end
    if self.elite_effect_show_handle then
        TimeHelper:CancelTimer(self.elite_effect_show_handle)
        self.elite_effect_show_handle = nil
    end
    if self.boss_effect_handle then
        TimeHelper:CancelTimer(self.boss_effect_handle)
        self.boss_effect_handle = nil
    end
end

function MainTaskFlushPanel:OnClickChapterReward()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanZhanKai)
    if not MainTaskData.Instance:IsShowRewardPreView() and self.data:GetChapterRewardIsGot(self.data:GetViewPage()) then 
        MainTaskData.Instance:SetRewardPreView(not MainTaskData.Instance:IsShowRewardPreView()
        ,self.data:GetChapterAward().chapter_award)

        MainTaskData.Instance:SetPreViewLocal(-1)
    end 
end 

function MainTaskFlushPanel:CloseRewardView()
    self:ClosePreView()
    -- self.ChapterRewardView:SetActive(false)
end

function MainTaskFlushPanel:OnGetAwardClick()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.OpenGift)
    local param_t = {}
    param_t.req_type = CHAPTER_BOSS_REQ_TYPE.TYPE_CLAIM_CHAPTER_REWARD
    param_t.param = self.data:GetViewPage()

    MainTaskCtrl.Instance:SendBossChapterReq(param_t)
    self.data:SetRewardPreView(false,{})
end

-- function MainTaskFlushPanel:OnClickChapterPreReward()

--     if self.data:GetShouldBeChange() and MainTaskData.Instance:GetPreViewLocal() == -1  then
--         local param_t = {}
--         param_t.req_type = CHAPTER_BOSS_REQ_TYPE.TYPE_CLAIM_CHAPTER_REWARD
--         param_t.param = self.data:GetViewPage()

--         MainTaskCtrl.Instance:SendBossChapterReq(param_t)
--         self.data:SetRewardPreView(false,{})
--     end 

--     self:ClosePreView()
-- end

function MainTaskFlushPanel:OnClickPage(num)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanZhanKai)
    local cur_page = self.data:GetViewPage()
    if cur_page + num > 0 and cur_page + num <= self.data.max_monster_page then 
        self.data:SetViewPage(cur_page + num)
        
        local cur_mode_type = self.data:GetViewMode()
        if cur_mode_type == 1 then 
            self.data:SetViewMode(cur_mode_type)
        else 
            local flag,tips_level = self.data:GetShouldBeUnlockElite()
            if not flag  then 
                local page_str = DataHelper.GetDaXie(cur_page)
                PublicPopupCtrl.Instance:Center(string.format(Language.MainTask.EliteUnLock,tips_level))
                self.data:SetViewMode(1)
            else 
                self.data:SetViewMode(cur_mode_type)
            end 
        end 
    end 
end 

-- mode_type 1 普通 2 精英
function MainTaskFlushPanel:OnClickMode(mode_type)
    local cur_mode_type = self.data:GetViewMode()
    if mode_type == cur_mode_type then 
        return 
    end 

    local flag,tips_level = self.data:GetShouldBeUnlockElite()
    if mode_type == 2 and flag then 
        self.data:SetViewMode(mode_type)
        self.data:CancelEliteUnLockSave(self.data:GetViewPage())
    elseif mode_type == 1 then  
        self.data:SetViewMode(mode_type)
    elseif mode_type == 2 and not flag then 
        local page_str = DataHelper.GetDaXie(self.data:GetViewPage())
        PublicPopupCtrl.Instance:Center(string.format(Language.MainTask.EliteUnLock,tips_level))
    end 
end

function MainTaskFlushPanel:ClosePreView()
    self.data:SetRewardPreView(false,{})
end

function MainTaskFlushPanel:CloseUnlockView()
    if self.EliteUnlockShowAnim ~= nil then 
        self.EliteUnlockShowAnim:SetBool(APH("complete"), true)
    end 
    self.EliteUnlockShowClose:SetActive(false)

    if self.close_event then
        TimeHelper:CancelTimer(self.close_event)
        self.close_event = nil
    end
end 

function MainTaskFlushPanel:FLushMapShow()
    local cur_page_info = self.data:GetChapterAward()
    local cur_pre_page_info,cur_after_page_info = self.data:GetAdjacentChapter(true),self.data:GetAdjacentChapter(false)
    if not self.is_init then 
        self.map_temp_group[1] = { temp_pos = 1,seq = cur_page_info.seq}
        self.map_temp_group[2] = { temp_pos = 2,seq = cur_after_page_info.seq }
        self.map_temp_group[3] = { temp_pos = 3 ,seq = cur_pre_page_info and cur_pre_page_info.seq or 0 }
        
        if not self.MaoPicTemps[1]:TextureIsNil() then
            self.MaoPicTemps[1]:SetByPath( string.format( MapConfig.Mini.png_path ,cur_page_info.chapter_bg ))
        end

        if not self.MaoPicTemps[2]:TextureIsNil() then
            self.MaoPicTemps[2]:SetByPath( string.format( MapConfig.Mini.png_path ,cur_after_page_info.chapter_bg ))
        end
        if not self.MaoPicTemps[3]:TextureIsNil() and cur_pre_page_info then
            self.MaoPicTemps[3]:SetByPath( string.format( MapConfig.Mini.png_path ,cur_pre_page_info.chapter_bg ))
        end

        self.MaoPicTempObjs[1]:SetActive(true)
        self.MaoPicTempObjs[2]:SetActive(false)
        self.MaoPicTempObjs[3]:SetActive(false)
    else
        --缓存调出 
        local cur_temp = -1
        for k,v in pairs(self.map_temp_group) do 
            local is_cur_page = v.seq == cur_page_info.seq
            self.MaoPicTempObjs[k]:SetActive(is_cur_page)
            if is_cur_page then 
                v.temp_pos = k 
                v.seq = cur_page_info.seq
                cur_temp = k
            end 
        end 

        -- 再缓存
        local flag_load_key = true
        for k,v in pairs(self.map_temp_group) do 
            local handle_info = flag_load_key and cur_pre_page_info or cur_after_page_info
            if k ~= cur_temp and handle_info then 
                if not self.MaoPicTemps[k]:TextureIsNil() then
                    self.MaoPicTemps[k]:SetByPath( string.format( MapConfig.Mini.png_path ,handle_info.chapter_bg ))
                    v.temp_pos = k 
                    v.seq = handle_info.seq

                    flag_load_key = not flag_load_key
                end
            end 
        end 
    end 
end

-----------
MainTaskPathCell = MainTaskPathCell or DeclareMono("MainTaskPathCell",UIWidgetBaseItem)

function MainTaskPathCell:SetData(data)
    self.data = data
    for i = 1 , 7 do 
        self.BossList[i]:SetData(data[i])
    end 

    for i = 1 , 7 do 
        self.RewardList[i]:SetData(data[i])
    end 
end 

function MainTaskPathCell:FlushBossEff()
    for i = 1,7 do 
        self.BossList[i]:FlushEffect()
    end 
end

MainTaskBossCell = MainTaskBossCell or DeclareMono("MainTaskBossCell",UIWidgetBaseItem)

function MainTaskBossCell:SetData(data)
    self.data = data

    -- 怪物组头像
    UH.SetIcon(self.NomalBossIcon, data.config.mosnter_icon, ICON_TYPE.ITEM)
    UH.SetIcon(self.EliteBossIcon, data.config.mosnter_icon, ICON_TYPE.ITEM)
    self.BossName.text = data.config.boss_name

    -- -- 是否精英 
    -- if self.data.status == MainTaskData.ChapterStatus.None 
    --     or self.data.status == MainTaskData.ChapterStatus.Lock then 
    --     self.NomalObj:SetActive(true)
    --     self.EliteObj:SetActive(false)
    -- else 
    --     self.NomalObj:SetActive(false)
    --     self.EliteObj:SetActive(true)
    -- end 

    local cur_mode_type = MainTaskData.Instance:GetViewMode()
    if cur_mode_type ==  1 then 
        self.NomalObj:SetActive(true)
        self.EliteObj:SetActive(false)
    else 
        self.NomalObj:SetActive(false)
        self.EliteObj:SetActive(true)
    end 

    -- 是否未解锁
    local str = string.format( Language.MainTask.EliteUnLock, self.data.config.elite_level )
    if self.data.status == MainTaskData.ChapterStatus.None or self.data.status == MainTaskData.ChapterStatus.Lock then 
        str = Language.MainTask.NeedKillNomal
    end 
    self.IsLocked.text = str -- self.data.config.unlock_conditions
    self.NomalHandle.Interactable = self.data.status ~= MainTaskData.ChapterStatus.Lock 
        and self.data.status ~= MainTaskData.ChapterStatus.Nomal_Complete 
        and self.data.status ~= MainTaskData.ChapterStatus.Elite_Complete

    local lock_flag = self.data.config.elite_level > RoleData.Instance:GetBaseData().level
        or self.data.status == MainTaskData.ChapterStatus.None or self.data.status == MainTaskData.ChapterStatus.Lock
    self.IsLockObj:SetActive(lock_flag and cur_mode_type ~= 1)

    self.EliteHandle.Interactable = self.data.status ~= MainTaskData.ChapterStatus.Lock 
        and self.data.status ~= MainTaskData.ChapterStatus.Elite_Complete and not ( lock_flag and cur_mode_type ~= 1 )


    -- 是否已击杀
    self.IsKilled:SetActive((self.data.status == MainTaskData.ChapterStatus.Nomal_Complete and cur_mode_type ==  1)
        or (cur_mode_type ==  2 and self.data.status == MainTaskData.ChapterStatus.Elite_Complete ))

    local _xuhao = MainTaskData.Instance:GetSelectedPos() 
    self.SelectEff:SetActive(self.data.config.xuhao == _xuhao)

    self:FlushEffect()
end 

function MainTaskBossCell:FlushEffect()

    if self.EffectTool == nil or 
        self.EffectTool:Equals(nil) == true or
        self.EffectTool.isActiveAndEnabled == false then return end 

    if self.NomalHandle.Interactable and self.effect_nomal == nil then 
        self.effect_nomal = self.EffectTool:Play(10010341)
    elseif not self.NomalHandle.Interactable and self.effect_nomal ~= nil then 
        self.EffectTool:Stop(self.effect_nomal)
        self.effect_nomal = nil
    end 

    if self.EliteHandle.Interactable and self.effect_elite == nil then 
        self.effect_elite = self.EffectTool:Play(10010342)
    elseif not self.EliteHandle.Interactable and self.effect_elite ~= nil then 
        self.EffectTool:Stop(self.effect_elite)
        self.effect_elite = nil
    end 
end

-- 1,普通
-- 2,精英
function MainTaskBossCell:OnClick(boss_type)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    if SceneData.Instance:GetMode() ~= SceneModeType.None then
        PublicPopupCtrl.Instance:Center(Language.Chat.BackActTips)
        return
    end

    local param_t = {}
    param_t.req_type = CHAPTER_BOSS_REQ_TYPE.TYPE_CLAIM_START_BATTLE
    param_t.param = boss_type == 1 and self.data.config.ordinary_monster or self.data.config.elite_monster

    if boss_type == 0 and RoleData.Instance:GetBaseData().is_in_team then 
        ViewMgr:CloseView(MainTaskView)
        PublicPopupCtrl.Instance:Center(Language.MainTask.NeedSolo)
        return
    end 

    MainTaskCtrl.Instance:SendBossChapterReq(param_t)

    if boss_type == 0 then 
        MainTaskData.Instance:SetIsEliteBattle(true,self.data.config.map_id_cfg) 
    else
        if self.data.config.zd_map_name ~= 0 then 
            MainTaskData.Instance:SetIsNomalBattle(true,self.data.config.zd_map_name) 
        end 
    end 

    if self.data.status ~= MainTaskData.ChapterStatus.Lock or self.data.status ~= MainTaskData.ChapterStatus.Elite_Complete then 
        ViewMgr:CloseView(MainTaskView)
    end 
end 


MainTaskRewardCell = MainTaskRewardCell or DeclareMono("MainTaskRewardCell",UIWidgetBaseItem)

function MainTaskRewardCell:SetData(data)
    self.data = data

    local temp_status = MainTaskData.ChapterStatus
    self.BoxInteract.Interactable = self.data.status ~= temp_status.Lock 

    -- LogError("self.data",self.data.config.boss_name,self.data.status,self.data.status_info.reward_status)
    -- if self.data.status == temp_status.None or self.data.status == temp_status.Lock then 
    --     UH.SpriteName(self.BoxPic,"nomal_box")
    -- elseif self.data.status == temp_status.Nomal_Complete and self.data.status_info.reward_status == MainTaskData.RewardStatus.Wait then 
    --     UH.SpriteName(self.BoxPic,"nomal_wait")
    -- elseif self.data.status == temp_status.Nomal_Complete and self.data.status_info.reward_status == MainTaskData.RewardStatus.Got then 
    --     UH.SpriteName(self.BoxPic,"elite_box")
    -- elseif self.data.status == temp_status.Elite_Complete and self.data.status_info.reward_status == MainTaskData.RewardStatus.Wait then 
    --     UH.SpriteName(self.BoxPic,"elite_wait")
    -- elseif self.data.status == temp_status.Elite_Complete and self.data.status_info.reward_status == MainTaskData.RewardStatus.Got then 
    --     UH.SpriteName(self.BoxPic,"elite_box_complete")
    -- end 

    local cur_mode_type = MainTaskData.Instance:GetViewMode()
    if cur_mode_type == 1 then 
        if self.data.status == temp_status.None or self.data.status == temp_status.Lock then 
            UH.SpriteName(self.BoxPic,"nomal_box")
            UH.SpriteName(self.BoxStaticPic,"nomal_box")
        elseif self.data.status == temp_status.Nomal_Complete and self.data.status_info.reward_status == MainTaskData.RewardStatus.Wait then 
            UH.SpriteName(self.BoxPic,"nomal_wait")
            UH.SpriteName(self.BoxStaticPic,"nomal_wait")
        elseif  self.data.status == temp_status.Nomal_Complete and self.data.status_info.reward_status == MainTaskData.RewardStatus.Got then 
            UH.SpriteName(self.BoxPic,"nomal_complete")
            UH.SpriteName(self.BoxStaticPic,"nomal_complete")
        end 
    else 
        if self.data.status == temp_status.Elite_Complete and self.data.status_info.reward_status == MainTaskData.RewardStatus.Wait then 
            UH.SpriteName(self.BoxPic,"elite_wait")
            UH.SpriteName(self.BoxStaticPic,"elite_wait")
        elseif self.data.status == temp_status.Elite_Complete and self.data.status_info.reward_status == MainTaskData.RewardStatus.Got then 
            UH.SpriteName(self.BoxPic,"elite_box_complete")
            UH.SpriteName(self.BoxStaticPic,"elite_box_complete")
        else 
            UH.SpriteName(self.BoxPic,"elite_box")
            UH.SpriteName(self.BoxStaticPic,"elite_box")
        end 
    end 

    -- if self.data.status == MainTaskData.ChapterStatus.None or self.data.status == MainTaskData.ChapterStatus.Lock or 

    if self.BoxAnima and self.data.status_info and self.data.status_info.reward_status == MainTaskData.RewardStatus.Wait then
        self.BoxStaticObj:SetActive(false)
        self.BoxPicObj:SetActive(true)
        -- self.BoxAnima.enabled = true
    elseif self.BoxAnima then 
        self.BoxStaticObj:SetActive(true)
        self.BoxPicObj:SetActive(false)
        -- self.BoxAnima.enabled = false
    end    

    -- LogError("data?",self.data)

    if #self.data.config.ordinary_award == 0 and cur_mode_type == 1 then 
        self.BoxInteract.Interactable = false
        self.BoxPicObj:SetActive(false)
        self.BoxStaticObj:SetActive(false)
    elseif  #self.data.config.elite_award == 0 and cur_mode_type == 2  then 
        self.BoxInteract.Interactable = false
        self.BoxPicObj:SetActive(false)
        self.BoxStaticObj:SetActive(false)
    end 
end 

-- 1,普通
-- 2,精英
function MainTaskRewardCell:OnClick()
    local temp_status = MainTaskData.ChapterStatus
    local cur_mode_type = MainTaskData.Instance:GetViewMode()
    local param_flag = cur_mode_type == 1 and self.data.config.ordinary_monster or self.data.config.elite_monster

    local param_t = {}
    param_t.req_type = CHAPTER_BOSS_REQ_TYPE.TYPE_CLAIM_BOSS_REWARD
    param_t.param = param_flag

    if MainTaskData.Instance:GetIsWaitReward(param_flag) then 
        MainTaskData.Instance:SetRewardPreView(not MainTaskData.Instance:IsShowRewardPreView()
            ,cur_mode_type == 1 and self.data.config.ordinary_award or self.data.config.elite_award)

        MainTaskData.Instance:SetPreViewLocal(self.data.config.xuhao)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanZhanKai)
        return 
    end 

    if (self.data.status == temp_status.Nomal_Complete and self.data.status_info.reward_status == MainTaskData.RewardStatus.Got)
    or ( self.data.status == temp_status.Elite_Complete and self.data.status_info.reward_status == MainTaskData.RewardStatus.Got) then 
        PublicPopupCtrl.Instance:Error(Language.MainTask.GotCurReward)
        return 
    end 
    MainTaskCtrl.Instance:SendBossChapterReq(param_t)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.OpenGift)
end 