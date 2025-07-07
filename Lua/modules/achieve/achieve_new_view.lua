AchieveNewView = AchieveNewView or DeclareView("AchieveNewView","achieve/achieve_new",Mod.Achieve.Main)
VIEW_DECLARE_LEVEL(AchieveNewView,ViewLevel.L1)

function AchieveNewView:AchieveNewView()
    self.lang = Language.Achieve
    if AchieveNewData.Instance:ShowStageUpFlag() then
        self.tmp_block_open_anim = true
    end
end

function AchieveNewView:LoadCallback(param_t)
    param_t = param_t or {}
    if param_t.is_win then
        local item_data_list = AchieveNewData.Instance:GetPassStageReward()
        if item_data_list then
            PublicPopupCtrl.Instance:RewardExpand2(item_data_list, 1, false)
            local list = {}
            for _,v in pairs(item_data_list) do
                if v.item_id ~= CommonItemId.Exp then
                    table.insert(list, v)
                end
            end
            RoleCtrl.Instance:ReleaseDelayExp()
            PublicPopupCtrl.Instance:ShowCustom(list)
        end
    end
    AchieveNewData.Instance:SetPassStageReward(nil)

    self.Tabbar:SetData({
        {name = self.lang.AchieveSub,tab_objs = {self.AchieveSub}, open = Mod.Achieve.Target, mod = TabMod(Mod.Achieve.Target), is_self_img = true},
        {name = self.lang.CourseSub,tab_objs = {self.CourseSub}, open = Mod.Achieve.Course, mod = TabMod(Mod.Achieve.Course), is_self_img = true},
    })
    self.Tabbar:ClickItem(1)
end

function AchieveNewView:OnClose()
    ViewMgr:CloseView(AchieveNewView)
end

AchieveNewViewPanel = AchieveNewViewPanel or DeclareMono("AchieveNewViewPanel", UIWFlushPanel)
--TitleImage=====UIImageSpriteSet
--ChanllageRP====GameObject
--StageRewardRP==GameObject
--EffectTool=====UIEffectTool
--challenge_btn==InteractorHandler
--RewardList=====UIWList[UIWItemCell]       --阶段奖励
function AchieveNewViewPanel:AchieveNewViewPanel()
    self.data_cares = {
        {data = AchieveNewData.Instance.base, func = self.FullFlush},
        {data = AchieveNewData.Instance.view_data, func = self.FlushChanllageBtn,keys={"flush_chanllage_btn"}},
   }
   self.effect_timer = nil
end
function AchieveNewViewPanel:OnClickPlayVideo()
    local config = AchieveNewData.Instance:GetConfigOfChallenge()
    local index = (config.stage - 1) * 5 + config.star
    local video = BattleVideo.Instance:VideoCfg(BATTLE_MODE.BATTLE_MODE_ACHIEVE, index)
    if video then
        BattleVideo.Instance:PlayVideoCfg(video)
        BattleCtrl.BattleReportDetailInfo(video.monster_group_id)
        ViewMgr:CloseView(AchieveNewView)
    end
end
function AchieveNewViewPanel:FullFlush()
    local data = AchieveNewData.Instance
    local cur_stage = data:GetCurStage(true)
    local cur_star = data:GetCurStar(true)
    local stage_reward_gettable = data:CanGetStageReward()
    self.StageRewardRP:SetActive(stage_reward_gettable)


    local list_data = {}
    if data:GetCurStage() > AchieveNewData.StageCount then  --当前阶段大于最高配置阶段，显示最后阶段完成的假数据
         for i=1,AchieveNewData.StarAchieveCount do
            local prog_data = AchieveNewData.Instance:GetOrCreateCompletedTaskProgData(i-1,cur_stage,cur_star)
            table.insert(list_data,prog_data)
        end
    else
        for i=1,AchieveNewData.StarAchieveCount do
            local prog_data = AchieveNewData.Instance:GetTaskProgData(i-1)
            if prog_data ~= nil then
                table.insert(list_data,prog_data)
            end
        end
    end

    self.achieve_list:SetData(list_data)

    --正常打开还是显示晋升特效
    if data:ShowStageUpFlag() then
        data:SetShowStateUpFlag(false)
        self:ShowStageStarGrowup()
    else
        self.star_txt.text = AchieveNewData.GetStageName(cur_stage,cur_star)
        self.TitleImage.SpriteName = AchieveNewData.GetTitleSpriteName(cur_stage)
        for i=1,cur_star do
            self.star_array[i]:SetActive(true)
        end
    end

    local cur_star_data = AchieveNewData.Instance:GetCurStarData()
    --刷新奖励预览列表
    local data_list = {
        --Item.Init(CommonItemId.Exp),
        Item.Init(CommonItemId.Coin),
    }
    for _,v in pairs(cur_star_data.reward_item) do
        table.insert(data_list,Item.Init(v.item_id))
    end
    self.RewardList:SetData(data_list)

    local config = AchieveNewData.Instance:GetConfigOfChallenge()
    local index = (config.stage - 1) * 5 + config.star
    local video = BattleVideo.Instance:VideoCfg(BATTLE_MODE.BATTLE_MODE_ACHIEVE, index)
    if video then
        if self.handle_care then
            BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        end
        self.handle_care = BattleVideo.Instance:SimpleInfo():Care(BindTool.Bind(self.FlushGray, self, video))
        self:FlushGray(video)
        BattleCtrl.BattleReportSimpleInfo(video.monster_group_id, 0)
    end
end
function AchieveNewViewPanel:FlushGray(video)
    local can_play = BattleVideo.Instance:GetSimpleInfo(video.monster_group_id, 0)
	self.Gray.enabled = can_play == 0
end
function AchieveNewViewPanel:FlushChanllageBtn()
    local chanllagable = AchieveNewData.Instance:CheckIsCanChallenge()
    self.challenge_btn.Interactable = chanllagable
    self.ChanllageRP:SetActive(chanllagable)
end

function AchieveNewViewPanel:ShowStageStarGrowup()
    -- AchieveCtrl.Instance:ReqeustBossChanllage()
    -- ViewMgr:CloseView(AchieveNewView)
    -- if true then return end
    local data = AchieveNewData.Instance
    local cur_stage = data:GetCurStage(true)
    local cur_star = data:GetCurStar(true)
    local pre_star = cur_star - 1
    local stage_up = pre_star == 0;
    self.star_txt.text = AchieveNewData.GetStageName(cur_stage, stage_up and AchieveNewData.StarCountPerStage or pre_star)
    for i=1,AchieveNewData.StarCountPerStage do
        self.star_array[i]:SetActive(i <= pre_star)
    end
    if stage_up then
        self.TitleImage.SpriteName = AchieveNewData.GetTitleSpriteName(cur_stage-1)
    end
    self.effect_timer = TimeHelper:AddDelayFrameTimer(function()
        self.effect_timer = nil
        if stage_up then
            self.EffectTool:Play(10010324)
        else
            self.EffectTool:Play(10010327,self.star_array[cur_star].transform.parent.gameObject)
        end
    end,6)
end

function AchieveNewViewPanel:OnDestroy()
    if self.effect_timer ~= nil then
        TimeHelper:CancelTimer(self.effect_timer)
        self.effect_timer = nil
    end
    if self.handle_care then
        BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        self.handle_care = nil
    end
    UIWFlushPanel.OnDestroy(self)
end

function AchieveNewViewPanel:OnEffectEvent(handle, time, key)
    -- LogError("OnEffectEvent",handle.effId,time,key)
    if key == "flash" then
        local data = AchieveNewData.Instance
        local eff_id = handle.effId
        local cur_stage = data:GetCurStage(true)
        local cur_star = data:GetCurStar(true)
        if eff_id == 10010327 then
            self.star_array[cur_star]:SetActive(true)
        elseif eff_id == 10010324 then
            self.TitleImage.SpriteName = AchieveNewData.GetTitleSpriteName(cur_stage)
        elseif eff_id == 10010325 then
            self.star_txt.text = AchieveNewData.GetStageName(cur_stage,cur_star)
        end
    elseif key == "next" then
        local eff_id = handle.effId
        if eff_id == 10010324 then
            local data = AchieveNewData.Instance
            local cur_star = data:GetCurStar(true)
            self.EffectTool:Play(10010327,self.star_array[cur_star].transform.parent.gameObject) 
        elseif eff_id == 10010327 then
            self.EffectTool:Play(10010325) 
        end
    end
end

function AchieveNewViewPanel:OnClickChanllage()
    TeamData.Instance:CheckSingle(function ()
        AchieveCtrl.Instance:ReqeustBossChanllage()
        ViewMgr:CloseView(AchieveNewView)
        if not TeamData.Instance:InTeam() then
            local config = AchieveNewData.Instance:GetConfigOfChallenge()
            if config ~= nil then AchieveNewData.Instance:SetChanllageMapId(config.map_id_cfg) end 
        end
        -- 存一下当前阶段的奖励，胜利后打开成就界面则显示
        local data = AchieveNewData.Instance:GetCurStarData()
        if data then
            local item_list_data = {}
            --table.insert(item_list_data,Item.Create({item_id = CommonItemId.Exp, num = data.reward_exp}))
            table.insert(item_list_data,Item.Create({item_id = CommonItemId.Coin, num = data.reward_coin}))
            for _,v in pairs(data.reward_item) do
                table.insert(item_list_data,Item.Create(v))
            end
            AchieveNewData.Instance:SetPassStageReward(item_list_data)
        end    
    end)
end

function AchieveNewViewPanel:ClickTip()
    ViewMgr:OpenView(AchieveNewRoad)
end

function AchieveNewViewPanel:ClickHead()
    ViewMgr:OpenView(AchieveNewCenghao)
end

AchieveNewItem = AchieveNewItem or DeclareMono("AchieveNewItem",UIWidgetBaseItem)
--RedPoint=======GameObject
--IconSprite=====UIImageSprite
--CompleteBgObj==GameObject
--UncomBgObj=====GameObject
--RewardedBgObj==GameObject
--EffectPlayer===UIEffectTool

function AchieveNewItem:AchieveNewItem()
    self.effect_handle = nil
end

function AchieveNewItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.cfg = AchieveNewData.Instance:GetAchieveCfgByIndex(data.cfg_idx)
    self.name.text = self.cfg.name
    local is_complete = AchieveNewData.Instance:CheckAchieveIsComplete(self.cfg.index)
    local award_getted = data.award_flag

    self.RedPoint:SetActive(is_complete and not award_getted)
    self.Icon:SetActive(not award_getted)
    if award_getted then
        self.RewardedBgObj:SetActive(true)
        self.UncomBgObj:SetActive(false)
        self.CompleteBgObj:SetActive(false)
        if self.effect_handle ~= nil then
            self.EffectPlayer:Stop(self.effect_handle)
            self.effect_handle = nil
        end
    elseif is_complete then
        self.RewardedBgObj:SetActive(false)
        self.UncomBgObj:SetActive(false)
        self.CompleteBgObj:SetActive(true)
        self.IconSprite.SpriteName = self.cfg.icon
        if self.effect_handle == nil then
            self.effect_handle = self.EffectPlayer:Play("5161025")
        end
   else
        self.RewardedBgObj:SetActive(false)
        self.UncomBgObj:SetActive(true)
        self.CompleteBgObj:SetActive(false)
        self.IconSprite.SpriteName = self.cfg.icon
        local value = AchieveNewData.Instance:GetAchieveCurProg(self.cfg.index)
        local max = AchieveNewData.Instance:GetAchieveMaxProg(self.cfg.index)
        if max ~= "" and max ~= 0 then self.progress:SetProgress(value / tonumber(max)) end
        self.progress:SetText(tostring(value).." / "..tostring(max))
        if self.effect_handle ~= nil then
            self.EffectPlayer:Stop(self.effect_handle)
            self.effect_handle = nil
        end
    end
end

function AchieveNewItem:OnClick()
    self:SaveClickSeq(self.cfg.stage,self.cfg.star,self.cfg.seq)
    ViewMgr:OpenView(AchieveNewDetail)
end

function AchieveNewItem:SaveClickSeq(stage,star,pindex)
    AchieveNewData.Instance.click_stage = stage
    AchieveNewData.Instance.click_star = star
    AchieveNewData.Instance.click_pindex = pindex
end