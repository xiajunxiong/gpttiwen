
MusicAnecdoteView = MusicAnecdoteView or DeclareView("MusicAnecdoteView", MusicAnecdoteConfig.ResPath .. "music_anecdote",Mod.Anecdote.MusicAnecdote)
-- VIEW_DECLARE_LEVEL(MusicAnecdoteView, ViewLevel.L0)
VIEW_DECLARE_MASK(MusicAnecdoteView, ViewMask.BgBlock)

function MusicAnecdoteView:LoadCallback()
end

function MusicAnecdoteView:OnClickClose()
end


MusicAnecdoteViewF = MusicAnecdoteViewF or DeclareMono("MusicAnecdoteViewF", UIWFlushPanel)

function MusicAnecdoteViewF:MusicAnecdoteViewF()
    self.data = MusicAnecdoteData.Instance
    self.language = Language.MusicGame

    self.data_cares = {
        {data = self.data.music_anecdote_sm, func = self.FlushScoreShow, keys = {"score"}},
        -- {data = self.data.music_anecdote_sm, func = self.FlushScaleShow, keys = {"scale"}}, --没用了 去掉
        {data = self.data.music_anecdote_sm, func = self.FlushComboShow, keys = {"combo"}},
        {data = self.data.music_anecdote_sm, func = self.FlushGameState, keys = {"state"}, init_call = false},
        {data = self.data.chapter_info, func = self.FlushChapterShow, keys = {"chapter"}},
        {data = self.data.music_anecdote_sm, func = self.FlushLifeShow, keys = {"life"}, init_call = false},
    }

    self.cur_music = self.data:CurMusic()
    self.time_rate = Time.deltaTime

    self.obj_list = {}
    self.try_times = 0
end

function MusicAnecdoteViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:InitShow()
end

function MusicAnecdoteViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    TimeHelper:CancelTimer(self.timer_tta)
    TimeHelper:CancelTimer(self.timer_rft)

    self.cur_music:EndMusic()

    AudioMgr:StopEffectHandle(self.handle_audio)
    AudioMgr:SetBgVolume(self.cache_bg_vol or 0)

    if ViewMgr:IsOpen(DialogTipsView) then
        ViewMgr:CloseView(DialogTipsView)
    end

    -- 手动初始化
    self.data:RandomStart(true)
end

function MusicAnecdoteViewF:InitShow()
    self.ProgressTime:SetProgress(0)
end

-- 顶部积分槽（需要重做
function MusicAnecdoteViewF:FlushRewardsShow()

    local list = self.cur_music:CheckPoints()
    for i = 1,self.CheckPoints:Length() do 
        if list[i] ~= nil then 
            self.CheckPoints[i]:SetObjActive(true)
            self.CheckPoints[i]:SetData(list[i])
        else 
            self.CheckPoints[i]:SetObjActive(false)
        end 
    end 

    -- local seek_master = self.cur_music:IsSeekMaster()
    -- local info = self.society_data.shitu_info_sm
    -- local shitu_chuan_gong_reward_list = self.society_data.shitu_chuan_gong_reward_list
    -- local shitu_chuan_gong_score_max = self.society_data.shitu_chuan_gong_score_max

    -- for i = 1, self.ShowRewards:Length() do
    --     self.ShowRewards[i]:SetData(shitu_chuan_gong_reward_list[i], shitu_chuan_gong_score_max, seek_master, info.week_chuan_gong_score_max)
    -- end
    -- self.ProgressRewardMax.sizeDelta = Vector2.New(math.min(1, info.week_chuan_gong_score_max / shitu_chuan_gong_score_max) * 623, 18)

    -- self.ScaleDesc.text = seek_master and self.language.ScaleDesc.master or self.language.ScaleDesc.apprentice
end

--倒计时演出
function MusicAnecdoteViewF:FlushCountDown()
    -- local count_down_num = self.cur_music:CountDownNum()
    local count_down_num = 3
    TimeHelper:CancelTimer(self.timer_tta)
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
        self:GameStart()
    end, 5, 1, false)
end

-- 重开·倒计时演出
function MusicAnecdoteViewF:ReStartCountDown()
    local count_down_num = 3
    TimeHelper:CancelTimer(self.timer_tta)
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
        self:GameReStart()
    end, 5, 1, false)
end

function MusicAnecdoteViewF:OnClickWaitStart()
    self.data:FlushChapter(self.data:NeedJump(),self.try_times == 0)

    if self.try_times == 0 then 
        self.try_times = self.try_times + 1
    end 
    
    
    self.cur_music = self.data:CurMusic()
    

    self.WaitBlock:SetActive(false)
    self:FlushRewardsShow()
    self:FlushCountDown()

    self.SuccessFinish:SetActive(false)
    self.FailFinish:SetActive(false)
    self.WaitStart:SetActive(false)
    -- self.ShowParent:SetActive(true)
    self.ShowParentGroup.alpha = 1
end

function MusicAnecdoteViewF:FlushScoreShow()
    local info = self.data.music_anecdote_sm
    local score = math.floor(info.score * info.scale)
    local cur_rate = info.score / self.cur_music:MaxScore()
    self.ProgressReward:SetProgress(cur_rate)
    self.ProgressReward:SetText(string.format(self.language.ScoreShow, info.score))

    for i = 1,self.CheckPoints:Length() do 
        self.CheckPoints[i]:CheckLight(cur_rate)
    end 

end

-- 没用了 去掉
function MusicAnecdoteViewF:FlushScaleShow()
    local info = self.data.music_anecdote_sm
    self.ScaleShow.text = string.format(self.language.ScaleShow, info.scale)

    self:FlushScoreShow()
end

-- 连击达成（重做)
function MusicAnecdoteViewF:FlushComboShow()
    local combo = self.data.music_anecdote_sm.combo
    self.ComboShow.text = string.format(self.language.ComboShow, combo)
    if combo > 0 then
        self.ComboAnim:Play()
        for k,v in pairs(MusicAnecdoteConfig.TempCombo) do 
            if combo == v then 
                self.Combo10Show.text = combo
                self.Combo10Anim:Play()
                break
            end 
        end 
    end
    self.ComboAlpha.alpha = combo > 0 and 1 or 0
end

-- 关卡状态
function MusicAnecdoteViewF:FlushGameState()
    if MusicAnecdoteConfig.GameState.Playing == self.data.music_anecdote_sm.state then
        local time = self.cur_music:Time()
        TimeHelper:CancelTimer(self.timer_tta)
        self.timer_tta = TimeHelper:AddCountDownTTA(function ()
            local cur_time = self.data.music_anecdote_sm.cur_time + 1
            self.data.music_anecdote_sm.cur_time = cur_time
            self.ProgressTime:SetProgress(cur_time / time) -- 关卡进度
        end, function ()
            self.CountDownObj:SetActive(false)
            self:GameEnd()
        end, time, 1)
        self.start_time = TimeHelper.GetServerTime()
        TimeHelper:CancelTimer(self.timer_rft)
        self.timer_rft = TimeHelper:AddRunFrameTimer(function ()
            self:FlushAnecdoteShow(TimeHelper.GetServerTime() - self.start_time)
        end, 10, 999999)
    elseif MusicAnecdoteConfig.GameState.End == self.data.music_anecdote_sm.state then
        TimeHelper:CancelTimer(self.timer_tta)
        TimeHelper:CancelTimer(self.timer_rft)
        -- ViewMgr:CloseView(MusicAnecdoteView)
    end
end

-- 走时判定
-- 走时间 获取节拍，并生成节拍
function MusicAnecdoteViewF:FlushAnecdoteShow(cur_time)
    local cur_time = math.floor(cur_time * 10) * 10
    if self.cur_time ~= cur_time then
        self.cur_time = cur_time
        local button_poss = self.cur_music:ButtonPos(self.cur_time)
        if button_poss then
            for k,v in pairs(button_poss) do
                self:SpawnItem(v)
            end 
        end
    end
end

-- 生成节拍
function MusicAnecdoteViewF:SpawnItem(button_pos)
    local type = button_pos.type
    local item_obj = self.ShowItems[type].gameObject:Clone(self.ShowParent)
    item_obj.name = button_pos.seq
    local item_mono = item_obj:GetLuaComponent(self.ShowItems[type]:GetTypeName())
    item_mono:SetData(button_pos)
    self.data:MarkLink(button_pos,item_mono)

    table.insert(self.obj_list,item_obj)
end

function MusicAnecdoteViewF:ClearItem()
    for k,v in pairs(self.obj_list) do 
        UnityGameObj.Destroy(v)
    end 
    
    self.obj_list = {}
end

function MusicAnecdoteViewF:ClearMarkItem(item)
    for k,v in pairs(self.obj_list) do 
        if v == item then 
            table.remove(self.obj_list,k)
            break
        end 
    end 
end

-- 游戏开始
function MusicAnecdoteViewF:GameStart()
    self.data.music_anecdote_sm.state = MusicAnecdoteConfig.GameState.Playing

    self.cache_bg_vol = AudioMgr:GetBgVolume()
    AudioMgr:SetBgVolume(0)
    self.handle_audio = AudioMgr:PlayEffectHandle(AudioType.Bg, self.cur_music:Audio())
end

-- 游戏再开
function MusicAnecdoteViewF:GameReStart()

end

function MusicAnecdoteViewF:FlushChapterShow()
    if self.data.chapter_info.chapter_show > 0 then 
        UH.SetText(self.CurChapter,string.format(Language.MusicStation.Desc.Chapters,DataHelper.GetDaXie(self.data.chapter_info.chapter_show)))
    end 
end

function MusicAnecdoteViewF:FlushLifeShow()
    self.LifeProgress:SetProgress(self.data.music_anecdote_sm.life / Config.audio_game_auto.other[1].hp)

    if self.data.music_anecdote_sm.life == 0 then 
        self:FinishGame()
    end 
end

-- 停止游戏
function MusicAnecdoteViewF:FinishGame()
    self.data.music_anecdote_sm.state = MusicAnecdoteConfig.GameState.End

    AudioMgr:StopEffectHandle(self.handle_audio)
    AudioMgr:SetBgVolume(self.cache_bg_vol or 0)

    -- self.ShowParent:SetActive(false)
    self.ShowParentGroup.alpha = 0
    self.WaitBlock:SetActive(true)
    self.FailFinish:SetActive(true)
end

-- 游戏结束
function MusicAnecdoteViewF:GameEnd()
    self.data.music_anecdote_sm.state = MusicAnecdoteConfig.GameState.End
 
    AudioMgr:StopEffectHandle(self.handle_audio)
    AudioMgr:SetBgVolume(self.cache_bg_vol or 0)

    self.WaitBlock:SetActive(true)
    self.SuccessFinish:SetActive(true)
end

function MusicAnecdoteViewF:OnClickReset()
    if self.data.music_anecdote_sm.state ~= MusicAnecdoteConfig.GameState.Playing then 
        return 
    end 
    self:FinishGame()
    self.data:ResetMark()
    self:ClearItem()
end

-- 离开按钮
function MusicAnecdoteViewF:OnClickLeave()
    -- ViewMgr:CloseView(MusicAnecdoteView)
    PublicPopupCtrl.Instance:DialogTips{
        content = self.language.AnecdoteLeaveTips,
		confirm = {
			func = function()
				ViewMgr:CloseView(DialogTipsView)
                ViewMgr:CloseView(MusicAnecdoteView)
			end
		}
    }
end

-- MusicAnecdoteViewRewardItem = DeclareMono("MusicAnecdoteViewRewardItem", UIWidgetBaseItem)

-- function MusicAnecdoteViewRewardItem:MusicAnecdoteViewRewardItem()
-- 	self.society_data = SocietyData.Instance
-- end

-- function MusicAnecdoteViewRewardItem:SetData(data, score_max, seek_master, score_record)
-- 	UIWidgetBaseItem.SetData(self, data)
--     if nil == data then return end
--     local item_list = seek_master and data.apprentice_reward_show or data.master_reward_show
--     if item_list and item_list[1] then
--         self.item_show = item_list[1]
--         UH.SetIcon(self.IconSp, Item.GetIconId(self.item_show.item_id))
--     end
--     self.NumShow.text = data.score
--     self.ItemPos.localPosition = Vector2.New(-311.5 + 623 * (data.score / score_max), 0)
--     self.ItemPos:SetObjActive(true)
-- end

-- function MusicAnecdoteViewRewardItem:OnClickItem()
--     if self.item_show then
--         local info = {
--             item_list = {self.item_show},
--             pos = Vector3.New(0, 140, 0)
--         }
--         ItemInfoCtrl.Instance:ItemInfoView(info)
--     end
-- end

MusicAnecdoteCheckPoint = DeclareMono("MusicAnecdoteCheckPoint", UIWidgetBaseItem)
function MusicAnecdoteCheckPoint:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.Point,data.point)

    self.Light:SetActive(false)
end 

function MusicAnecdoteCheckPoint:CheckLight(rate)
    if self.data == nil then return end 
    self.Light:SetActive(rate >= self.data.rate)
end

MusicAnecdoteViewShowItem1 = DeclareMono("MusicAnecdoteViewShowItem1", UIWidgetBaseItem)

function MusicAnecdoteViewShowItem1:MusicAnecdoteViewShowItem1()
    self.mg_data = MusicAnecdoteData.Instance
    self.ItemState = MusicAnecdoteConfig.Item1State.Ready
    self.SpawnTime = 0
end

function MusicAnecdoteViewShowItem1:OnDestroy()
	TimeHelper:CancelTimer(self.timer_dt)
end


function MusicAnecdoteViewShowItem1:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self.ItemPos:SetObjActive(true)
    local button_co = Cfg.MusicAnecdoteButtonByType(data.type)
    local parent_size = self.ParentPos.rect
    self.ItemPos.anchoredPosition = Vector2.New(data.pos_x,data.pos_y) -- Vector2.New(- parent_size.width / 2 + parent_size.width * (data.pos_x / 100), - parent_size.height / 2 + parent_size.height * (data.pos_y / 100))
    self:JudgePosSet()
    self.timer_dt = TimeHelper:AddDelayTimer(function ()
        self:Disappearing()
    end, button_co.disappear_time / 100)
    self.EffectTool:Play(7161031)
    self.SpawnTime = math.floor(TimeHelper.GetServerTime() * 100)
    self.PerfectTime = button_co.perfect1
end

function MusicAnecdoteViewShowItem1:Disappearing()
    if MusicAnecdoteConfig.Item1State.Ready == self.ItemState then
        self.ItemState = MusicAnecdoteConfig.Item1State.Disappearing
        self.EffectTool:Play(6165303)
        self.BtnConfirmObj:SetActive(false)
        local judge_show_type = self.mg_data:ClickShowMiss()
        self:JudgeShow(judge_show_type)
    end
end

function MusicAnecdoteViewShowItem1:OnEffectEvent(handle, time, event)
    if handle.effId == 6165303 then
        if EffectEvent.event_end == event then
            self:Disappear()
        end
    elseif handle.effId == 7161032 then
        if EffectEvent.event_end == event then
            self:Disappear()
        end
    end
end

function MusicAnecdoteViewShowItem1:Disappear()
    self.Panel:ClearMarkItem(self.ItemPos.gameObject)
    UnityGameObj.Destroy(self.ItemPos.gameObject)
end

function MusicAnecdoteViewShowItem1:OnClickConfirm()
    if MusicAnecdoteConfig.Item1State.Ready == self.ItemState then
        self.ItemState = MusicAnecdoteConfig.Item1State.Click
        self.EffectTool:Play(7161032)
        self.BtnConfirmObj:SetActive(false)
        local judge_show_type = self.mg_data:ClickShow(math.floor(TimeHelper.GetServerTime() * 100) - self.SpawnTime - self.PerfectTime)
        self:JudgeShow(judge_show_type)
    end
end

function MusicAnecdoteViewShowItem1:JudgeShow(type)
    local eff_id = MusicAnecdoteConfig.JudgeType2EffId[type]
    if eff_id then
        self.EffectTool:Play(eff_id)
    end
end

function MusicAnecdoteViewShowItem1:JudgePosSet()
    if nil == self.data then return end
    local pos_x = self.data.pos_x
    local pos_y = self.data.pos_y
    self.JudgePos.localPosition = Vector2.New(pos_x > 50 and -100 or 100, pos_y > 50 and -100 or 100)
end


MusicAnecdoteViewShowItem2 = DeclareMono("MusicAnecdoteViewShowItem2", UIWidgetBaseItem)

function MusicAnecdoteViewShowItem2:MusicAnecdoteViewShowItem2()
    self.mg_data = MusicAnecdoteData.Instance
    self.ItemState = MusicAnecdoteConfig.Item2State.Ready
    self.SpawnTime = 0

    if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end
end

function MusicAnecdoteViewShowItem2:OnDestroy()
	TimeHelper:CancelTimer(self.timer_dt)
	TimeHelper:CancelTimer(self.timer_dtp1)
	TimeHelper:CancelTimer(self.timer_dtp2)
    -- Runner.Instance:RemoveRunObj(self)
end


function MusicAnecdoteViewShowItem2:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self.ItemPos:SetObjActive(true)
    local button_co = Cfg.MusicAnecdoteButtonByType(data.type)
    local parent_size = self.ParentPos.rect
    self.ItemPos.anchoredPosition = Vector2.New(data.pos_x,data.pos_y)  -- Vector2.New(- parent_size.width / 2 + parent_size.width * (data.pos_x / 100), - parent_size.height / 2 + parent_size.height * (data.pos_y / 100))
    self:JudgePosSet()
    self.timer_dt = TimeHelper:AddDelayTimer(function ()
        self:Disappearing()
    end, button_co.disappear_time / 100)
    self.timer_dtp1 = TimeHelper:AddDelayTimer(function ()
        self:Sliding()
    end, button_co.perfect1 / 100)
    self.timer_dtp2 = TimeHelper:AddDelayTimer(function ()
        self.EffectTool:Play(7161042)
        if MusicAnecdoteConfig.Item2State.PressDown == self.ItemState then
            self:ShowClick(false)

            local localPoint = UH.ScreenPointToViewPoint(self.ContentRect, self.mainCanvasCom)
            if GameMath.Distance(Vector2.New(self.ItemPos.localPosition.x + self.BtnConfirmPos.localPosition.x,
                self.ItemPos.localPosition.y + self.BtnConfirmPos.localPosition.y), localPoint, true) > 100 then
                self.ItemState = MusicAnecdoteConfig.Item2State.PressUp
                self:ShowClick(false)
                local judge_show_type = self.mg_data:ClickShowMiss()
                self:JudgeShow(judge_show_type)
            else
                -- local judge_show_type = self.mg_data:ClickShow2(math.floor(TimeHelper.GetServerTime() * 100) - self.SpawnTime - self.PerfectTime2)
                -- self:JudgeShow(judge_show_type)
                local judge_show_type = self.mg_data:ClickShow2(0)
                self:JudgeShow(judge_show_type)
            end
        elseif MusicAnecdoteConfig.Item2State.Sliding == self.ItemState then
            local judge_show_type = self.mg_data:ClickShowMiss()
            self:JudgeShow(judge_show_type)
        end
        self.ItemState = MusicAnecdoteConfig.Item2State.Sliding
    end, button_co.perfect2 / 100)
    self.EffectTool:Play(7161039)
    self.SpawnTime = math.floor(TimeHelper.GetServerTime() * 100)
    self.PerfectTime1 = button_co.perfect1
    self.PerfectTime2 = button_co.perfect2
end

function MusicAnecdoteViewShowItem2:Disappearing()
    self.ItemState = MusicAnecdoteConfig.Item2State.Disappearing
    self.EffectTool:Play(7161044)
    self.BtnConfirmPos:SetObjActive(false)
    self.ItemImg.enabled = false
end

function MusicAnecdoteViewShowItem2:Sliding()
    if MusicAnecdoteConfig.Item2State.Ready == self.ItemState then
        local judge_show_type = self.mg_data:ClickShowMiss()
        self:JudgeShow(judge_show_type)
    end
    self.ItemState = MusicAnecdoteConfig.Item2State.PressDown == self.ItemState and MusicAnecdoteConfig.Item2State.PressDown or MusicAnecdoteConfig.Item2State.Sliding
    -- self.EffectTool:Play(6165303)
    self.ItemAnim:Play()
    self.EffectTool:Play(7161040)
end

function MusicAnecdoteViewShowItem2:OnEffectEvent(handle, time, event)
    if handle.effId == 7161044 then
        if EffectEvent.event_end == event then
            self:Disappear()
        end
    -- elseif handle.effId == 7161032 then
    --     if EffectEvent.event_end == event then
    --         self:Disappear()
    --     end
    end
end

function MusicAnecdoteViewShowItem2:Disappear()
    self.Panel:ClearMarkItem(self.ItemPos.gameObject)
    UnityGameObj.Destroy(self.ItemPos.gameObject)
end

function MusicAnecdoteViewShowItem2:OnPressDown()
    if MusicAnecdoteConfig.Item2State.Ready == self.ItemState then
        self.ItemState = MusicAnecdoteConfig.Item2State.PressDown
        self:ShowClick(true)
        local judge_show_type = self.mg_data:ClickShow2(math.floor(TimeHelper.GetServerTime() * 100) - self.SpawnTime - self.PerfectTime1)
        self:JudgeShow(judge_show_type)
    end
end

function MusicAnecdoteViewShowItem2:OnPressUp()
    if MusicAnecdoteConfig.Item2State.PressDown == self.ItemState then
        self.ItemState = MusicAnecdoteConfig.Item2State.PressUp
        self:ShowClick(false)
        
        local localPoint = UH.ScreenPointToViewPoint(self.ContentRect, self.mainCanvasCom)
        if GameMath.Distance(Vector2.New(self.ItemPos.localPosition.x + self.BtnConfirmPos.localPosition.x,
        self.ItemPos.localPosition.y + self.BtnConfirmPos.localPosition.y), localPoint, true) > 100 then
            self.ItemState = MusicAnecdoteConfig.Item2State.PressUp
            self:ShowClick(false)
            local judge_show_type = self.mg_data:ClickShowMiss()
            self:JudgeShow(judge_show_type)
        else
            local judge_show_type = self.mg_data:ClickShow2(math.floor(TimeHelper.GetServerTime() * 100) - self.SpawnTime - self.PerfectTime2)
            self:JudgeShow(judge_show_type)
        end
    end
end

function MusicAnecdoteViewShowItem2:JudgeShow(type)
    local eff_id = MusicAnecdoteConfig.JudgeType2EffId[type]
    if eff_id then
        self.EffectTool:Play(eff_id)
    end
end

function MusicAnecdoteViewShowItem2:ShowClick(value)
    self.ClickShow:SetActive(value)
    self.ConfirmImg.enabled = not value

    -- if value then
    --     Runner.Instance:AddRunObj(self, 3)
    -- else
    --     Runner.Instance:RemoveRunObj(self)
    -- end
end

function MusicAnecdoteViewShowItem2:Update()
    if MusicAnecdoteConfig.Item2State.PressDown == self.ItemState then
        local localPoint = UH.ScreenPointToViewPoint(self.ContentRect, self.mainCanvasCom)
        if AnecdoteMath.Distance(Vector2.New(self.ItemPos.localPosition.x + self.BtnConfirmPos.localPosition.x,
                self.ItemPos.localPosition.y + self.BtnConfirmPos.localPosition.y), localPoint, true) > 100 then
            self.ItemState = MusicAnecdoteConfig.Item2State.PressUp
            self:ShowClick(false)
            local judge_show_type = self.mg_data:ClickShowMiss()
            self:JudgeShow(judge_show_type)
        end
    end
end

function MusicAnecdoteViewShowItem2:JudgePosSet()
    if nil == self.data then return end
    local pos_x = self.data.pos_x
    local pos_y = self.data.pos_y
    self.JudgePos.localPosition = Vector2.New(0 ~= self.PosX and self.PosX or (pos_x > 50 and -100 or 100), 0 ~= self.PosY and self.PosY or (pos_y > 50 and -100 or 100))
end

MusicAnecdoteViewShowItem3 = DeclareMono("MusicAnecdoteViewShowItem3", UIWidgetBaseItem)

function MusicAnecdoteViewShowItem3:MusicAnecdoteViewShowItem3()
    self.mg_data = MusicAnecdoteData.Instance
    self.ItemState = MusicAnecdoteConfig.Item1State.Ready
    self.SpawnTime = 0
end 

function MusicAnecdoteViewShowItem3:OnDestroy()
	TimeHelper:CancelTimer(self.timer_dt)
end

function MusicAnecdoteViewShowItem3:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self.ItemPos:SetObjActive(true)
    local button_co = Cfg.MusicAnecdoteButtonByType(data.type)
    local parent_size = self.ParentPos.rect
    self.ItemPos.anchoredPosition = Vector2.New(data.pos_x,data.pos_y) --Vector2.New(- parent_size.width / 2 + parent_size.width * (data.pos_x / 100), - parent_size.height / 2 + parent_size.height * (data.pos_y / 100))
    self:JudgePosSet()
    self.timer_dt = TimeHelper:AddDelayTimer(function ()
        self:Disappearing()
    end, button_co.disappear_time / 100)
    self.EffectTool:Play(6165302)
    self.SpawnTime = math.floor(TimeHelper.GetServerTime() * 100)
    self.PerfectTime = button_co.perfect1
end

function MusicAnecdoteViewShowItem3:Disappearing()
    if MusicAnecdoteConfig.Item1State.Ready == self.ItemState then
        self.ItemState = MusicAnecdoteConfig.Item1State.Disappearing
        self.EffectTool:Play(6165301)
        self.BtnConfirmObj:SetActive(false)
        local judge_show_type = self.mg_data:ClickShowMiss()
        self:JudgeShow(judge_show_type)
    end
end

function MusicAnecdoteViewShowItem3:OnEffectEvent(handle, time, event)
    if handle.effId == 6165301 then
        if EffectEvent.event_end == event then
            self:Disappear()
        end
    elseif handle.effId == 7161032 then
        if EffectEvent.event_end == event then
            self:Disappear()
        end
    end
end

function MusicAnecdoteViewShowItem3:Disappear()
    self.Panel:ClearMarkItem(self.ItemPos.gameObject)
    self.mg_data:ClearMarkLink(self.data)
    UnityGameObj.Destroy(self.ItemPos.gameObject)
end

function MusicAnecdoteViewShowItem3:OnClickConfirm()
    if MusicAnecdoteConfig.Item1State.Ready == self.ItemState then
        self.ItemState = MusicAnecdoteConfig.Item1State.Click
        self.EffectTool:Play(7161032)
        self.BtnConfirmObj:SetActive(false)
        local judge_show_type = self.mg_data:ClickShow3(math.floor(TimeHelper.GetServerTime() * 100) - self.SpawnTime - self.PerfectTime,self.data)
        -- self:JudgeShow(judge_show_type)
    end
end

function MusicAnecdoteViewShowItem3:JudgeShow(type)
    local eff_id = MusicAnecdoteConfig.JudgeType2EffId[type]
    if eff_id then
        self.EffectTool:Play(eff_id)
    end
end

function MusicAnecdoteViewShowItem3:JudgePosSet()
    if nil == self.data then return end
    local pos_x = self.data.pos_x
    local pos_y = self.data.pos_y
    self.JudgePos.localPosition = Vector2.New(pos_x > 50 and -100 or 100, pos_y > 50 and -100 or 100)
end

