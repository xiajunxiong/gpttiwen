Conspiracy = Conspiracy or DeclareView("Conspiracy", "conspiracy/conspiracy", Mod.Conspiracy.Main)
VIEW_DECLARE_LEVEL(Conspiracy, ViewLevel.L1)

function Conspiracy:Conspiracy()
    self.lang = Language.Conspiracy
end
function Conspiracy:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Title, Vector2.New(1091, 574))
end

ConspiracyPanel = ConspiracyPanel or DeclareMono("ConspiracyPanel", UIWFlushPanel)
function ConspiracyPanel:ConspiracyPanel()
    self.data = ConspiracyData.Instance
    self.lang = Language.Conspiracy
    self.data_cares = {
        {data = self.data.pass_info, func = self.FlushBossList}
    }
end

function ConspiracyPanel:onFlush()
end
function ConspiracyPanel:FlushBossList()
    local data = self.data:BossList()
    self.BossList:SetData(data)
    self.BossList:ClickItem(1)
end
function ConspiracyPanel:OnClickBoss(index, data)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    local info = data[index]
    self.choose_info = info
    UH.SetText(self.Level, string.format(self.lang.Level, info.level))
    UH.SetText(self.Name, string.format(self.lang.Name, info.name))
    UH.SetText(self.Desc, info.desc)
    --显示首通奖励和奖励显示
    UH.SetText(self.RewardText, self.lang.Reward[info.first and 2 or 1])
    self.RewardList:SetData(info.first_reward)

    self.RewardListObj:SetActive(not info.first)
    self.RewardComplete:SetActive(info.first)

    local real_index = self.choose_info.seq * 7 + self.choose_info.index
    local video = BattleVideo.Instance:VideoCfg(BATTLE_MODE.BATTLE_MODE_MI_YI, real_index)
    if video then
        if self.handle_care then
            BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        end
        self.handle_care = BattleVideo.Instance:SimpleInfo():Care(BindTool.Bind(self.FlushGray, self, video))
        self:FlushGray(video)
        BattleCtrl.BattleReportSimpleInfo(video.monster_group_id, 0)
    end
end
function ConspiracyPanel:FlushGray(video)
    local can_play = BattleVideo.Instance:GetSimpleInfo(video.monster_group_id, 0)
	self.Gray.enabled = can_play == 0
end
function ConspiracyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	if self.handle_care then
        BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        self.handle_care = nil
    end
end
function ConspiracyPanel:OnClickGoTo()
    -- SceneLogic.Instance:AutoToPos(self.choose_info.scene_id,
    --     SceneCtrl.Instance:CurScene():PosToClint(self.choose_info.x, self.choose_info.y))
        -- 这里寻路之后还要直接打
    -- 判断是否同一层
    if self.choose_info.scene_id ~= SceneData.Instance:SceneId() then
        PublicPopupCtrl.Instance:Center(ErrorText[78])
        return
    end
    SceneLogic.Instance:AutoToNpc(CommonStruct.MonsterVo(self.choose_info),nil,true)
    ViewMgr:CloseView(Conspiracy)
end
function ConspiracyPanel:OnClickPlayVideo()
    local index = self.choose_info.seq * 7 + self.choose_info.index
    local video = BattleVideo.Instance:VideoCfg(BATTLE_MODE.BATTLE_MODE_MI_YI, index)
    if video then
        BattleVideo.Instance:PlayVideoCfg(video)
        BattleCtrl.BattleReportDetailInfo(video.monster_group_id)
    end
end



ConspiracyItem = ConspiracyItem or DeclareMono("ConspiracyItem", UIWidgetBaseItem)
function ConspiracyItem:ConspiracyItem()
    self.con_data = ConspiracyData.Instance
    self.lang = Language.Conspiracy
end
function ConspiracyItem:SetData(data)
    self.first_reward = {}
    
    for i = 1, self.BossClick:Length() do
        local info = data[i]
        data[i].first = self.con_data:FirstKill(info.seq + 1, i)
        data[i].isget = self.con_data:WeekPass(info.seq + 1, i)
        self.DeathList[i]:SetActive(data[i].first)
        local group = string.split(info.first_reward_id, "|")
        local group2 = self.con_data:PassReward(info.level_group_id)

        info.first_reward = {}
        info.week_reward = {}
        -- 修改 首杀+周奖励+钱和经验
        for _, v in ipairs(group) do
            local list = self.con_data:RewardList(tonumber(v))
            for _, value in ipairs(list) do
                if not data[i].first then
                    table.insert(info.first_reward, value)
                end
            end
        end
        table.insert(info.first_reward, Item.Create({item_id = CommonItemId.Exp, num = info.exp}))
        table.insert(info.first_reward, Item.Create({item_id = CommonItemId.Coin, num = info.coin}))
        -- for _, v in ipairs(group2) do
        --     local item = Item.Create({item_id = tonumber(v), num = 1})
        --     --[[ local list = self.con_data:RewardList(tonumber(v))
        --     for index, value in ipairs(list) do
        --         table.insert(info.week_reward, value)
        --     end ]]
        --     table.insert(info.first_reward, item)
        -- end
        local cfg = NpcHelper.NpcCo(info.npc_id)
        UH.SetIcon(self.IconList[i], cfg.npc_icon)
    end
    --剩余时间
    local w = tonumber(os.date("%w", os.time()))
    w = w == 0 and 7 or w
    local h = os.date("%H", os.time())
    UH.SetText(self.Time, string.format(self.lang.Time, 7 - w, 24 - h))
    UIWidgetBaseItem.SetData(self, data)
    --名字
    local scene = self.con_data:SceneInfo(data[1].scene_id)
    UH.SetText(self.Title, scene.name)
end

function ConspiracyItem:Click()
    if self.BossClick then
        self.BossClick[1]:Trigger()
    end
end
function ConspiracyItem:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[2].desc})
end
