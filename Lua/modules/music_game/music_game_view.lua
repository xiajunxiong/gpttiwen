
MusicGameView = MusicGameView or DeclareView("MusicGameView", MusicGameConfig.ResPath .. "music_game")
-- VIEW_DECLARE_LEVEL(MusicGameView, ViewLevel.L0)
VIEW_DECLARE_MASK(MusicGameView, ViewMask.BgBlock)

function MusicGameView:LoadCallback()
end

function MusicGameView:OnClickClose()
    ViewMgr:CloseView(MusicGameView)
end

MusicGameViewF = MusicGameViewF or DeclareMono("MusicGameViewF", UIWFlushPanel)

function MusicGameViewF:MusicGameViewF()
    self.data = MusicGameCtrl.Instance:Data()
    self.society_data = SocietyCtrl.Instance:Data()
    self.language = Language.MusicGame

    self.data_cares = {
        {data = self.data.music_game_sm, func = self.FlushScoreShow, keys = {"score"}},
        {data = self.data.music_game_sm, func = self.FlushScaleShow, keys = {"scale"}},
        {data = self.data.music_game_sm, func = self.FlushComboShow, keys = {"combo"}},
        {data = self.data.music_game_sm, func = self.FlushGameState, keys = {"state"}, init_call = false},
    }

    self.cur_music = self.data:CurMusic()
    self.time_rate = Time.deltaTime
end

function MusicGameViewF:Awake()
    UIWFlushPanel.Awake(self)

    if self.data.is_test then
        self:FlushCountDownTest()
    else
        self:InitShow()
        self:FlushRewardsShow()
        self:FlushCountDown()
    end
end

function MusicGameViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    TimeHelper:CancelTimer(self.timer_tta)
    TimeHelper:CancelTimer(self.timer_rft)

    self.cur_music:EndMusic()

    AudioMgr:StopEffectHandle(self.handle_audio)
    AudioMgr:SetBgVolume(self.cache_bg_vol or 0)

    if ViewMgr:IsOpen(DialogTipsView) then
        ViewMgr:CloseView(DialogTipsView)
    end
end

function MusicGameViewF:InitShow()
    self.ProgressTime:SetProgress(0)
end

function MusicGameViewF:FlushRewardsShow()
    local seek_master = self.cur_music:IsSeekMaster()
    local info = self.society_data.shitu_info_sm
    local shitu_chuan_gong_reward_list = self.society_data.shitu_chuan_gong_reward_list
    local shitu_chuan_gong_score_max = self.society_data.shitu_chuan_gong_score_max

    for i = 1, self.ShowRewards:Length() do
        self.ShowRewards[i]:SetData(shitu_chuan_gong_reward_list[i], shitu_chuan_gong_score_max, seek_master, info.week_chuan_gong_score_max)
    end
    self.ProgressRewardMax.sizeDelta = Vector2.New(math.min(1, info.week_chuan_gong_score_max / shitu_chuan_gong_score_max) * 623, 18)

    self.ScaleDesc.text = seek_master and self.language.ScaleDesc.master or self.language.ScaleDesc.apprentice
end

function MusicGameViewF:FlushCountDown()
    local count_down_num = self.cur_music:CountDownNum()
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
    end, count_down_num + 2, 1, false)
end

function MusicGameViewF:FlushCountDownTest()
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


function MusicGameViewF:FlushScoreShow()
    local info = self.data.music_game_sm
    local score = math.floor(info.score * info.scale)
    self.ProgressReward:SetProgress(info.score / self.society_data.shitu_chuan_gong_score_max)
    self.ProgressReward:SetText(string.format(self.language.ScoreShow, info.score))
end

function MusicGameViewF:FlushScaleShow()
    local info = self.data.music_game_sm
    self.ScaleShow.text = string.format(self.language.ScaleShow, info.scale)

    self:FlushScoreShow()
end

function MusicGameViewF:FlushComboShow()
    local combo = self.data.music_game_sm.combo
    self.ComboShow.text = string.format(self.language.ComboShow, combo)
    if combo > 0 then
        self.ComboAnim:Play()
        if 0 == combo % 10 then
            self.Combo10Show.text = combo
            self.Combo10Anim:Play()
        end
    end
    self.ComboAlpha.alpha = combo > 0 and 1 or 0
end

function MusicGameViewF:FlushGameState()
    if MusicGameConfig.GameState.Playing == self.data.music_game_sm.state then
        local time = self.cur_music:Time()
        TimeHelper:CancelTimer(self.timer_tta)
        self.timer_tta = TimeHelper:AddCountDownTTA(function ()
            local cur_time = self.data.music_game_sm.cur_time + 1
            self.data.music_game_sm.cur_time = cur_time
            self.ProgressTime:SetProgress(cur_time / time)
        end, function ()
            self.CountDownObj:SetActive(false)
            self:GameEnd()
        end, time, 1)
        self.start_time = TimeHelper.GetServerTime()
        TimeHelper:CancelTimer(self.timer_rft)
        self.timer_rft = TimeHelper:AddRunFrameTimer(function ()
            self:FlushGameShow(TimeHelper.GetServerTime() - self.start_time)
        end, 10, 999999)
    elseif MusicGameConfig.GameState.End == self.data.music_game_sm.state then
        TimeHelper:CancelTimer(self.timer_tta)
        TimeHelper:CancelTimer(self.timer_rft)
        ViewMgr:CloseView(MusicGameView)
    end
end

function MusicGameViewF:FlushGameShow(cur_time)
    local cur_time = math.floor(cur_time * 10) * 10
    if self.cur_time ~= cur_time then
        self.cur_time = cur_time
        local button_pos = self.cur_music:ButtonPos(self.cur_time)
        if button_pos then
            self:SpawnItem(button_pos)
        end
    end
end

function MusicGameViewF:GameStart()
    self.data.music_game_sm.state = MusicGameConfig.GameState.Playing

    self.cache_bg_vol = AudioMgr:GetBgVolume()
    AudioMgr:SetBgVolume(0)
    self.handle_audio = AudioMgr:PlayEffectHandle(AudioType.Bg, self.cur_music:Audio())
end

function MusicGameViewF:GameEnd()
    self.data.music_game_sm.state = MusicGameConfig.GameState.End

    AudioMgr:StopEffectHandle(self.handle_audio)
    AudioMgr:SetBgVolume(self.cache_bg_vol or 0)
end

function MusicGameViewF:SpawnItem(button_pos)
    local type = button_pos.type
    local item_obj = self.ShowItems[type].gameObject:Clone(self.ShowParent)
    item_obj.name = button_pos.seq
    local item_mono = item_obj:GetLuaComponent(self.ShowItems[type]:GetTypeName())
    item_mono:SetData(button_pos)
end

function MusicGameViewF:OnClickLeave()
    PublicPopupCtrl.Instance:DialogTips{
        content = self.language.LeaveTips,
		confirm = {
			func = function()
				ViewMgr:CloseView(DialogTipsView)
                ViewMgr:CloseView(MusicGameView)
				SceneCtrl.Instance:RequestLeaveFb()
			end
		}
    }
end

MusicGameViewRewardItem = DeclareMono("MusicGameViewRewardItem", UIWidgetBaseItem)

function MusicGameViewRewardItem:MusicGameViewRewardItem()
	self.society_data = SocietyData.Instance
end

function MusicGameViewRewardItem:SetData(data, score_max, seek_master, score_record)
	UIWidgetBaseItem.SetData(self, data)
    if nil == data then return end
    local item_list = seek_master and data.apprentice_reward_show or data.master_reward_show
    if item_list and item_list[1] then
        self.item_show = item_list[1]
        UH.SetIcon(self.IconSp, Item.GetIconId(self.item_show.item_id))
    end
    self.NumShow.text = data.score
    self.ItemPos.localPosition = Vector2.New(-311.5 + 623 * (data.score / score_max), 0)
    self.ItemPos:SetObjActive(true)
end

function MusicGameViewRewardItem:OnClickItem()
    if self.item_show then
        local info = {
            item_list = {self.item_show},
            pos = Vector3.New(0, 140, 0)
        }
        ItemInfoCtrl.Instance:ItemInfoView(info)
    end
end

MusicGameViewShowItem1 = DeclareMono("MusicGameViewShowItem1", UIWidgetBaseItem)

function MusicGameViewShowItem1:MusicGameViewShowItem1()
    self.mg_data = MusicGameData.Instance
	self.society_data = SocietyData.Instance
    self.ItemState = MusicGameConfig.Item1State.Ready
    self.SpawnTime = 0
end

function MusicGameViewShowItem1:OnDestroy()
	TimeHelper:CancelTimer(self.timer_dt)
end


function MusicGameViewShowItem1:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self.ItemPos:SetObjActive(true)
    local button_co = Cfg.MusicGameButtonByType(data.type)
    local parent_size = self.ParentPos.rect
    self.ItemPos.localPosition = Vector2.New(- parent_size.width / 2 + parent_size.width * (data.pos_x / 100), - parent_size.height / 2 + parent_size.height * (data.pos_y / 100))
    self:JudgePosSet()
    self.timer_dt = TimeHelper:AddDelayTimer(function ()
        self:Disappearing()
    end, button_co.disappear_time / 100)
    self.EffectTool:Play(7161031)
    self.SpawnTime = math.floor(TimeHelper.GetServerTime() * 100)
    self.PerfectTime = button_co.perfect1
end

function MusicGameViewShowItem1:Disappearing()
    if MusicGameConfig.Item1State.Ready == self.ItemState then
        self.ItemState = MusicGameConfig.Item1State.Disappearing
        self.EffectTool:Play(7161030)
        self.BtnConfirmObj:SetActive(false)
        local judge_show_type = self.mg_data:ClickShowMiss()
        self:JudgeShow(judge_show_type)
    end
end

function MusicGameViewShowItem1:OnEffectEvent(handle, time, event)
    if handle.effId == 7161030 then
        if EffectEvent.event_end == event then
            self:Disappear()
        end
    elseif handle.effId == 7161032 then
        if EffectEvent.event_end == event then
            self:Disappear()
        end
    end
end

function MusicGameViewShowItem1:Disappear()
    UnityGameObj.Destroy(self.ItemPos.gameObject)
end

function MusicGameViewShowItem1:OnClickConfirm()
    if MusicGameConfig.Item1State.Ready == self.ItemState then
        self.ItemState = MusicGameConfig.Item1State.Click
        self.EffectTool:Play(7161032)
        self.BtnConfirmObj:SetActive(false)
        local judge_show_type = self.mg_data:ClickShow(math.floor(TimeHelper.GetServerTime() * 100) - self.SpawnTime - self.PerfectTime)
        self:JudgeShow(judge_show_type)
    end
end

function MusicGameViewShowItem1:JudgeShow(type)
    local eff_id = MusicGameConfig.JudgeType2EffId[type]
    if eff_id then
        self.EffectTool:Play(eff_id)
    end
end

function MusicGameViewShowItem1:JudgePosSet()
    if nil == self.data then return end
    local pos_x = self.data.pos_x
    local pos_y = self.data.pos_y
    self.JudgePos.localPosition = Vector2.New(pos_x > 50 and -100 or 100, pos_y > 50 and -100 or 100)
end


MusicGameViewShowItem2 = DeclareMono("MusicGameViewShowItem2", UIWidgetBaseItem)

function MusicGameViewShowItem2:MusicGameViewShowItem2()
    self.mg_data = MusicGameData.Instance
	self.society_data = SocietyData.Instance
    self.ItemState = MusicGameConfig.Item2State.Ready
    self.SpawnTime = 0

    if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end
end

function MusicGameViewShowItem2:OnDestroy()
	TimeHelper:CancelTimer(self.timer_dt)
	TimeHelper:CancelTimer(self.timer_dtp1)
	TimeHelper:CancelTimer(self.timer_dtp2)
    -- Runner.Instance:RemoveRunObj(self)
end


function MusicGameViewShowItem2:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self.ItemPos:SetObjActive(true)
    local button_co = Cfg.MusicGameButtonByType(data.type)
    local parent_size = self.ParentPos.rect
    self.ItemPos.localPosition = Vector2.New(- parent_size.width / 2 + parent_size.width * (data.pos_x / 100), - parent_size.height / 2 + parent_size.height * (data.pos_y / 100))
    self:JudgePosSet()
    self.timer_dt = TimeHelper:AddDelayTimer(function ()
        self:Disappearing()
    end, button_co.disappear_time / 100)
    self.timer_dtp1 = TimeHelper:AddDelayTimer(function ()
        self:Sliding()
    end, button_co.perfect1 / 100)
    self.timer_dtp2 = TimeHelper:AddDelayTimer(function ()
        self.EffectTool:Play(7161042)
        if MusicGameConfig.Item2State.PressDown == self.ItemState then
            self:ShowClick(false)

            local localPoint = UH.ScreenPointToViewPoint(self.ContentRect, self.mainCanvasCom)
            if GameMath.Distance(Vector2.New(self.ItemPos.localPosition.x + self.BtnConfirmPos.localPosition.x,
                self.ItemPos.localPosition.y + self.BtnConfirmPos.localPosition.y), localPoint, true) > 100 then
                self.ItemState = MusicGameConfig.Item2State.PressUp
                self:ShowClick(false)
                local judge_show_type = self.mg_data:ClickShowMiss()
                self:JudgeShow(judge_show_type)
            else
                -- local judge_show_type = self.mg_data:ClickShow2(math.floor(TimeHelper.GetServerTime() * 100) - self.SpawnTime - self.PerfectTime2)
                -- self:JudgeShow(judge_show_type)
                local judge_show_type = self.mg_data:ClickShow2(0)
                self:JudgeShow(judge_show_type)
            end
        elseif MusicGameConfig.Item2State.Sliding == self.ItemState then
            local judge_show_type = self.mg_data:ClickShowMiss()
            self:JudgeShow(judge_show_type)
        end
        self.ItemState = MusicGameConfig.Item2State.Sliding
    end, button_co.perfect2 / 100)
    self.EffectTool:Play(7161039)
    self.SpawnTime = math.floor(TimeHelper.GetServerTime() * 100)
    self.PerfectTime1 = button_co.perfect1
    self.PerfectTime2 = button_co.perfect2
end

function MusicGameViewShowItem2:Disappearing()
    self.ItemState = MusicGameConfig.Item2State.Disappearing
    self.EffectTool:Play(7161044)
    self.BtnConfirmPos:SetObjActive(false)
    self.ItemImg.enabled = false
end

function MusicGameViewShowItem2:Sliding()
    if MusicGameConfig.Item2State.Ready == self.ItemState then
        local judge_show_type = self.mg_data:ClickShowMiss()
        self:JudgeShow(judge_show_type)
    end
    self.ItemState = MusicGameConfig.Item2State.PressDown == self.ItemState and MusicGameConfig.Item2State.PressDown or MusicGameConfig.Item2State.Sliding
    -- self.EffectTool:Play(7161030)
    self.ItemAnim:Play()
    self.EffectTool:Play(7161040)
end

function MusicGameViewShowItem2:OnEffectEvent(handle, time, event)
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

function MusicGameViewShowItem2:Disappear()
    UnityGameObj.Destroy(self.ItemPos.gameObject)
end

function MusicGameViewShowItem2:OnPressDown()
    if MusicGameConfig.Item2State.Ready == self.ItemState then
        self.ItemState = MusicGameConfig.Item2State.PressDown
        self:ShowClick(true)
        local judge_show_type = self.mg_data:ClickShow2(math.floor(TimeHelper.GetServerTime() * 100) - self.SpawnTime - self.PerfectTime1)
        self:JudgeShow(judge_show_type)
    end
end

function MusicGameViewShowItem2:OnPressUp()
    if MusicGameConfig.Item2State.PressDown == self.ItemState then
        self.ItemState = MusicGameConfig.Item2State.PressUp
        self:ShowClick(false)
        
        local localPoint = UH.ScreenPointToViewPoint(self.ContentRect, self.mainCanvasCom)
        if GameMath.Distance(Vector2.New(self.ItemPos.localPosition.x + self.BtnConfirmPos.localPosition.x,
        self.ItemPos.localPosition.y + self.BtnConfirmPos.localPosition.y), localPoint, true) > 100 then
            self.ItemState = MusicGameConfig.Item2State.PressUp
            self:ShowClick(false)
            local judge_show_type = self.mg_data:ClickShowMiss()
            self:JudgeShow(judge_show_type)
        else
            local judge_show_type = self.mg_data:ClickShow2(math.floor(TimeHelper.GetServerTime() * 100) - self.SpawnTime - self.PerfectTime2)
            self:JudgeShow(judge_show_type)
        end
    end
end

function MusicGameViewShowItem2:JudgeShow(type)
    local eff_id = MusicGameConfig.JudgeType2EffId[type]
    if eff_id then
        self.EffectTool:Play(eff_id)
    end
end

function MusicGameViewShowItem2:ShowClick(value)
    self.ClickShow:SetActive(value)
    self.ConfirmImg.enabled = not value

    -- if value then
    --     Runner.Instance:AddRunObj(self, 3)
    -- else
    --     Runner.Instance:RemoveRunObj(self)
    -- end
end

function MusicGameViewShowItem2:Update()
    if MusicGameConfig.Item2State.PressDown == self.ItemState then
        local localPoint = UH.ScreenPointToViewPoint(self.ContentRect, self.mainCanvasCom)
        if GameMath.Distance(Vector2.New(self.ItemPos.localPosition.x + self.BtnConfirmPos.localPosition.x,
                self.ItemPos.localPosition.y + self.BtnConfirmPos.localPosition.y), localPoint, true) > 100 then
            self.ItemState = MusicGameConfig.Item2State.PressUp
            self:ShowClick(false)
            local judge_show_type = self.mg_data:ClickShowMiss()
            self:JudgeShow(judge_show_type)
        end
    end
end

function MusicGameViewShowItem2:JudgePosSet()
    if nil == self.data then return end
    local pos_x = self.data.pos_x
    local pos_y = self.data.pos_y
    self.JudgePos.localPosition = Vector2.New(0 ~= self.PosX and self.PosX or (pos_x > 50 and -100 or 100), 0 ~= self.PosY and self.PosY or (pos_y > 50 and -100 or 100))
end