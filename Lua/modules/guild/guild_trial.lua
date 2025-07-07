GuildTrial = GuildTrial or DeclareView("GuildTrial", "guild/guild_trial", Mod.GuildTrial.Main)
-- BossPage = GameObject
-- BossPageBg = GameObject
-- ChapterBg1 = GameObject
-- ChapterRole = GameObject
-- ChapterBg = GameObject
-- ChapterPage = GameObject
-- GuildTrialChapterPanel = GuildTrialChapterPanel
function GuildTrial:GuildTrial()

end

function GuildTrial:LoadCallback()
    self.GuildTrialChapterPanel:FlushChapterPanel()
	GuildData.Instance.guild_trial_point_have_show = true
	GuildData.Instance:ShowTrialPoint()
end
------------------------------------------
GuildTrialFlush = GuildTrialFlush or DeclareMono("GuildTrialFlush", UIWFlushPanel)
function GuildTrialFlush:GuildTrialFlush()
    self.fogPositions = {
        [1] = Vector2.New(-137, -18),
        [2] = Vector2.New(157, -18),
        [3] = Vector2.New(677, -18),
        [4] = Vector2.New(1136, -18),
    }
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data.guild_trial_page, func = self.ShowPage, keys = {"index"}},
        {data = self.data.guild_trial_page, func = self.SetCountDown, keys = {"index"}},
        {data = self.data.guild_boss_info, func = self.FlushBossInfo},
        {data = self.data.boss, func = self.setRewardRedPoint, keys = {"flushRewardRedPoint"}},
    }
end

function GuildTrialFlush:ShowPage()
    -- index 0为关卡界面 1为boss界面
    -- LogError("ShowPage",self.data.guild_trial_page.index)
    self.BossPage:SetActive(self.data.guild_trial_page.index == 1)
    -- self.ChapterBg1:SetActive(self.data.guild_trial_page.index == 0)
    -- self.ChapterRole:SetActive(self.data.guild_trial_page.index == 0)
    -- self.ChapterBg:SetActive(self.data.guild_trial_page.index == 0)
    self.ChapterPage:SetActive(self.data.guild_trial_page.index == 0)
    if self.data.guild_trial_page.index == 1 then
        self.BossPageAnim:Play()
    end
    self:PlayRewardEffect()
end

function GuildTrialFlush:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetGuildTrialPage(0)
    self.TimeMeter:CloseCountDownTime()
    self.reward_effect_played = false
    self.play_reward_effect = false
end

function GuildTrialFlush:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    if self.data.guild_trial_page.index ~= 0 then
        self.data:SetGuildTrialPage(0)
    else
        ViewMgr:CloseView(GuildTrial)
    end
end

function GuildTrialFlush:SetCountDown()
    local startTimeStamp = os.time()
    local cur = TimeManager.FormatUnixTime2Date(startTimeStamp)
    local dayNum = cur.day
    if cur.hour >= 6 then
        dayNum = cur.day + 1
    end
    local endTimeStamp = os.time({day = dayNum, month = cur.month, year = cur.year, hour = 6, minute = 0, second = 0})
    UH.FillAmount(self.RecycelProg, 1 - (endTimeStamp - startTimeStamp) / 86400)
    UH.FillAmount(self.RecycelProgEff, 1 - (endTimeStamp - startTimeStamp) / 86400)
    self.TimeMeter:StampTime(
        endTimeStamp,
        TimeType.Type_Special_3,
        Language.Guild.TrialReset
    )
end

function GuildTrialFlush:FlushBossInfo()
    local info = self.data:GuildBossInfo()
    if info.guild_boss_chapter_id == nil then
        return
    end
    local cfg = self.data:GuildBossConfig(info.guild_boss_chapter_id)
    self.BossPageBg:SetByPath("guild/trial_map"..((info.guild_boss_chapter_id % 4) + 1)..".png")
    local max_times = self.data:TrialMaxTimes()
    -- 显示名字
    UH.SetText(self.LblName, cfg.chapter_name)
    UH.SetText(self.LblCount, string.format(Language.Guild.TrialTime, max_times - info.guild_boss_play_times))
    local chapter_info = self.data:ChapterConfig(info.guild_boss_chapter_id)
    for i = 1, self.BossList:Length(), 1 do
        if info.guild_boss_seq >= chapter_info[i - 1].seq then
            self.BossList[i]:SetData(chapter_info[i - 1])
        else
            self.BossList[i]:ClearData()
        end
    end
    --迷雾的位置
    if info.guild_boss_seq < 2 then
        UH.AnchoredPosition(self.fogs1Rect, self.fogPositions[info.guild_boss_seq + 1])
        self.fogs1:SetActive(true)
        UH.AnchoredPosition(self.fogs2Rect, self.fogPositions[3])
        self.fogs2:SetActive(true)
    else
        self.fogs1:SetActive(false)
        if self.fogPositions[info.guild_boss_seq + 1] ~= nil then
            UH.AnchoredPosition(self.fogs2Rect, self.fogPositions[info.guild_boss_seq + 1])
            self.fogs2:SetActive(true)
        else
            self.fogs2:SetActive(false)
        end
        self.BossScrollView.horizontalNormalizedPosition = 0.66
    end
    self:setRewardRedPoint()
    
    self:SetCountDown()
end

function GuildTrialFlush:setRewardRedPoint()
    local info = self.data:GuildBossInfo()
    if info.guild_boss_seq == 5 and info.fetch_chapter_reward_times < 1 and info.guild_boss_join_flag ~= 0 then
        if self.reward_red == nil then
            self.reward_red = UiRedPoint.New(self.BtnReward, Vector3.New(30, 33, 0))
        end
        self.play_reward_effect = true
        self.reward_red:SetNum(1)
        self.RewardEffectObj:SetActive(true)
    else
        self.RewardEffectObj:SetActive(false)
        if self.reward_red ~= nil then
            self.reward_red:SetNum(0)
        end
    end
end

function GuildTrialFlush:OnClickRank()
    GuildCtrl.SendGuildBossReq(GUILD_BOSS_REQ_TYPE.RANK)
    ViewMgr:OpenView(GuildTrialRank)
end
function GuildTrialFlush:OnClickReward()
    local info = self.data:GuildBossInfo()
    if info.fetch_chapter_reward_times < 1 and info.guild_boss_seq == 5 and info.guild_boss_join_flag ~= 0 then
        ViewMgr:OpenView(GuildTrialRewardGet)
    else
        ViewMgr:OpenView(GuildTrialReward)
    end
end

function GuildTrialFlush:OnClickChapterReward()
    GuildData.Instance.select_view_open_flag = 2
    ViewMgr:OpenView(GuildTrialSelect)
end

function GuildTrialFlush:PlayRewardEffect()
    if self.play_reward_effect and not self.reward_effect_played then
        self.RewardEffect:Play("10010328")
        self.reward_effect_played = true
    end
end
----------------------------------------------------------------
GuildTrialBossItem = GuildTrialBossItem or DeclareMono("GuildTrialBossItem", UIWidgetBaseItem)
-- BloodObj = GameObject
-- BloodBar = UIWProgressBar
-- ModelShow = ModelUIShow
-- KillTextObj = GameObject
-- BossNameObj = GameObject
-- BossNameText = Text
-- ModelParent = GameObject
-- BgObj = GameObject
function GuildTrialBossItem:GuildTrialBossItem()
    self.guild_data = GuildData.Instance
	self.login_data = LoginData.Instance:BaseData()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                show_shadow = false
            }
        )
    end
end
function GuildTrialBossItem:SetData(data)
    local info = self.guild_data:GuildBossInfo()
    self.BloodObj:SetActive(info.guild_boss_seq == data.seq)
    self.KillTextObj:SetActive(info.guild_boss_seq > data.seq)
    self.BossNameObj:SetActive(info.guild_boss_seq <= data.seq)
    self.ElementObj:SetActive(info.guild_boss_seq <= data.seq)
    self.BgObj:SetActive(true)
    UH.SpriteName(self.BgSprite, info.guild_boss_seq > data.seq and "hasKill" or "notKill")
    local monster_id = Cfg.MonsterGroupById(data.boss_id).show_monster
    local monster_cfg = Cfg.MonsterById(monster_id)
    local monster_name = monster_cfg.name
    UH.SetText(self.BossNameText, monster_name)
    local cur, total = self.guild_data:BossBlood()
    self.BloodBar:SetProgress(info.remain_hp_percent / 10000)
    self.BloodBar:SetText(string.format("%d/%d", cur, total))
    self.Element:SetData(MonsterData.GetElement(monster_cfg), false, monster_cfg.is_chaos == 1)
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(ResHelper.MonsterResByGroup(data.boss_id)))
end

function GuildTrialBossItem:ClearData()
    self.KillTextObj:SetActive(false)
    self.BossNameObj:SetActive(false)
    self.BgObj:SetActive(false)
    self.ModelParent:SetActive(false)
    self.BloodObj:SetActive(false)
    self.ElementObj:SetActive(false)
end

function GuildTrialBossItem:OnDestroy()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
end

function GuildTrialBossItem:OnClickBoss(seq)
    local max = self.guild_data:TrialMaxTimes()
    local cur = self.guild_data:GetActCount(ActType.GuidlTrial)
    if cur >= max then
       PublicPopupCtrl.Instance:Center(ErrorText[87])
        return
    end
    local info = self.guild_data:GuildBossInfo()
    if seq ~= info.guild_boss_seq then
        return
    end

	local team_list = TeamCtrl.Instance:Data():GetMemberList()
	--判断是否组队
	if TeamCtrl.Instance:Data():InTeam() then
		local param_t = {
		content = Language.Guild.TrialInTeam,
		confirm = {
					func = function()
						ViewMgr:CloseView(DialogTipsView)
						TeamCtrl.Instance:SendExitTeam()
						self:EnterFight()
					end
				  },
		cancel = {
					func = function()
						ViewMgr:CloseView(DialogTipsView)
						ViewMgr:CloseView(GuildTrial)
					end
				}
		}
        BehaviorData.Instance.auto_behavior = 0
		PublicPopupCtrl.Instance:DialogTips(param_t)	
	else
		self:EnterFight()
	end	
end

function GuildTrialBossItem:EnterFight()
	if self.login_data.is_on_crosssever  then
		LoginCtrl.Instance:SendBackToOriginServer()
		return
	end
	self.guild_data:EnterTrialFight()
end



----------------------------------------------------------------
GuildTrialChapterPanel = GuildTrialChapterPanel or DeclareMono("GuildTrialChapterPanel", UIWFlushPanel)
-- GuildName = GuildName
-- GuildRank = GuildRank
-- TotalProgress = UIWProgressBar
-- GuildContribute = Text
-- ChapterArray = Array(GuildTrialChapterItem)
function GuildTrialChapterPanel:GuildTrialChapterPanel()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data:GuildBossInfo(), func = self.FlushChapterPanel}
    }
end

function GuildTrialChapterPanel:FlushChapterPanel()
    --设置左上角公会信息
    local guildInfo = self.data:GetGuildInfo()
    UH.SetText(self.GuildName, guildInfo.guild_name)
    
    --ArrayItemSetData
    local info = self.data:GuildBossInfo()
    if info.guild_boss_chapter_id == nil then
        return
    end
    local data_list = {}
    for i=0, 5 do
        --目前表内仅有3章，以后扩展的话要改逻辑
        --2019.12.23根据需求扩展到6章
        local data = GuildData.GetChapterDefaultDate(i)
        if i <= info.guild_boss_chapter_id then
            data.has_open = true
            -- if i == info.guild_boss_chapter_id then
                data.guild_boss_seq = info.guild_boss_seq
                data.guild_boss_play_times = info.guild_boss_play_times
                data.guild_max_chapter = info.guild_max_chapter
                data.remain_hp_percent = info.remain_hp_percent
            -- end
        -- elseif info.guild_boss_seq
        elseif i <= info.guild_max_chapter then --20201022 修改下一章节显示条件（前一章只要打通过无论今天开启的这个前一章是否通关）  
        --elseif i == info.guild_boss_chapter_id + 1 and info.guild_boss_seq == 5 then--已经打完当前章节，下一章解锁
            data.has_open = true
        end
        table.insert(data_list, data)
    end
    for i=1, self.ChapterArray:Length() do
        self.ChapterArray[i]:SetData(data_list[i])
    end
end

--选择攻打目标
function GuildTrialChapterPanel:OnClickChapter()
    GuildData.Instance.select_view_open_flag = 1
    ViewMgr:OpenView(GuildTrialSelect)
end

function GuildTrialChapterPanel:OnClickRank()
    GuildCtrl.SendGuildBossReq(GUILD_BOSS_REQ_TYPE.GUILD_RANK)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    ViewMgr:OpenView(GuildTrialTotalRank)
end

function GuildTrialChapterPanel:OnClickTips()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[84].desc)
end
----------------------------------------------------------------
GuildTrialChapterItem = GuildTrialChapterItem or DeclareMono("GuildTrialChapterItem",UIWidgetBaseItem)
-- ChapterItemBg = UIImageSpreteSet
-- ChapterName = Text
-- ChapterItemPro = UIWProgressBar
-- NotPass = GameObject
function GuildTrialChapterItem:GuildTrialChapterItem()
end

function GuildTrialChapterItem:SetData(data)
    if data == nil then
        return
    end
    local cfg = GuildData.Instance:GuildBossConfig(data.guild_boss_chapter_id)
	self.KillBg:SetActive(false)
    --每章5个boss
    if data.has_open then
        if data.guild_boss_chapter_id < data.guild_max_chapter and (data.guild_boss_chapter_id ~= GuildData.Instance.guild_boss_info.guild_boss_chapter_id) then
            self.ChapterItemPro:SetProgress(1)
            self.ChapterItemPro:SetText("100%")
            self.Finish:SetActive(true)
        else
            self.ChapterItemPro:SetProgress(data.guild_boss_seq / 5)
            self.ChapterItemPro:SetText(tostring(data.guild_boss_seq / 5 * 100).."%")
            self.Finish:SetActive(data.guild_boss_seq == 5)
			if data.guild_boss_seq ~= 5 and (data.guild_boss_chapter_id == GuildData.Instance.guild_boss_info.guild_boss_chapter_id) then
				self.KillBg:SetActive(true)
			end
        end
        local chapterName = string.format(Language.Guild.ChapterDot, DataHelper.GetDaXie(cfg.chapter_id + 1), cfg.chapter_name)
        UH.SetText(self.ChapterName, chapterName)
    end
	local is_show_pro = false
	if data.guild_boss_chapter_id == GuildData.Instance.guild_boss_info.guild_boss_chapter_id then
		is_show_pro = true
	end
	self.ChapterItemProObj:SetActive(is_show_pro)
    self.NotPass:SetActive(data.has_open)
    self.NotOpen:SetActive(not data.has_open)
    --播放未解锁特效
    if data.guild_boss_chapter_id == GuildData.Instance.guild_boss_info.guild_boss_chapter_id + 1 then
        if GuildData.Instance.guild_boss_info.guild_boss_seq ~= GuildData.Instance.cache_guild_boss_seq then
            if data.has_open then
                if self.delayPlayEffect == nil then
                    self.delayPlayEffect = TimeHelper:AddDelayFrameTimer(function ()
                        self.Animation.enabled = true
                        self:PlayOpenEffect()
                    end,0)
                end
            end
        end
    end
    self.data = data
end

function GuildTrialChapterItem:OnDestroy()
    if self.delayPlayEffect ~= nil then
        TimeHelper:CancelTimer(self.delayPlayEffect)
        self.delayPlayEffect = nil
    end
    self.Animation.enabled = false
end

function GuildTrialChapterItem:PlayOpenEffect()
    if self.EffectTool then
        self.EffectTool:Play("10010304")
    end
    GuildData.Instance.cache_guild_boss_seq = GuildData.Instance.guild_boss_info.guild_boss_seq
end

function GuildTrialChapterItem:OnClickItem()
	if self.data == nil then
        return
    end
    if self.data.guild_boss_chapter_id ~= GuildData.Instance.guild_boss_info.guild_boss_chapter_id then
        if self.data.has_open then
            PublicPopupCtrl.Instance:Center(Language.Guild.TrialFloatingTipFormat[2])
        else
            PublicPopupCtrl.Instance:Center(Language.Guild.TrialFloatingTipFormat[1])
        end
    else
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL1)
        GuildData.Instance:SetGuildTrialPage(1)
    end
end