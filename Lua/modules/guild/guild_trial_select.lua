GuildTrialSelect = GuildTrialSelect  or DeclareView("GuildTrialSelect", "guild/guild_trial_select")
function GuildTrialSelect:LoadCallback()
    local title_str = Language.Guild.TrialTarget
    if GuildData.Instance.select_view_open_flag == 2 then
        title_str = Language.Guild.TrialRewardTitle
    end
    self.Board:SetData(self:GetType(), title_str, Vector2.New(676, 500))
end

function GuildTrialSelect:CloseCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

GuildTrialSelectPanel = GuildTrialSelectPanel or DeclareMono("GuildTrialSelectPanel", UIWFlushPanel)
-- SelectList = UIWidgetList
-- Tips = Text
function GuildTrialSelectPanel:GuildTrialSelectPanel()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data.boss, func = self.FlushSelectInfo,key = {"flushselect"}}
    }
end

function GuildTrialSelectPanel:FlushSelectInfo()
    if GuildData.Instance.select_view_open_flag == 1 then
        UH.SetText(self.Tips, Language.Guild.TrialSelectTips)
    else
        UH.SetText(self.Tips, Language.Guild.TrialChapterRewardTip)
    end
    local info = self.data.family_boss_info
    local temp = {}
    for id, _ in pairs(info) do
        table.insert(temp, id)
    end
    self.SelectList:SetData(temp)
end

function GuildTrialSelectPanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[15].desc})
end
------------------------
GuildTrialSelectItem = GuildTrialSelectItem or DeclareMono("GuildTrialSelectItem", UIWidgetBaseItem)
function GuildTrialSelectItem:GuildTrialSelectItem()
    
end
function GuildTrialSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = GuildData.Instance:GuildBossInfo()
    local cfg = GuildData.Instance:GuildBossConfig(data)
    UH.SetText(self.LblName,string.format(Language.Guild.Chapter, DataHelper.GetDaXie(data + 1))..cfg.chapter_name)
    
    if GuildData.Instance.select_view_open_flag == 1 then
        self.CheckBtnObj:SetActive(true)
        UH.SetText(self.CheckText, Language.Guild.TrialNext)
    else
        self.CheckBtnObj:SetActive(false)
        if data == info.guild_boss_chapter_id then
            UH.SetText(self.CheckText, Language.Guild.TrialCur)
        else
            UH.SetText(self.CheckText, "")
        end
    end
    
    if info.guild_boss_select_chapter == -1 then
        self.SelectObj:SetActive(data == info.guild_boss_chapter_id)
    else
        self.SelectObj:SetActive(info.guild_boss_select_chapter == data)
    end
    local reward_data_list = GuildData.Instance:GetTrialSelectRewardList(data)
    local reward_item_list = {}
    for i,v in ipairs(reward_data_list) do
        table.insert(reward_item_list,Item.Init(v.item_id, v.num, v.is_bind))
    end
    self.RewardList:SetData(reward_item_list)
end
function GuildTrialSelectItem:OnClickSelect()
    -- 判断权限
    -- self.SelectObj:SetActive(not self.SelectObj.activeSelf)
    -- self.mVo = GuildData.Instance:GetMemberVo()
    -- if self.mVo.post ~= 0 and self.mVo ~= 1 then
    --     PublicPopupCtrl.Instance:Center(ErrorText[88])
    --     return
    -- end
    self.info = GuildData.Instance:GuildBossInfo()
    if self.data > self.info.guild_max_chapter then
        PublicPopupCtrl.Instance:Center(ErrorText[89])
        return
    end
    GuildCtrl.SendGuildBossReq(GUILD_BOSS_REQ_TYPE.SET_CHAPTER, self.data)
    GuildCtrl.SendGuildBossReq(GUILD_BOSS_REQ_TYPE.INFO)
end