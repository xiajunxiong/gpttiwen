
HarmonyHeavenMenuView = HarmonyHeavenMenuView or DeclareMono("HarmonyHeavenMenuView", UIWFlushPanel)

function HarmonyHeavenMenuView:HarmonyHeavenMenuView()
    self.data = HarmonyHeavenCtrl.Instance:Data()
    self.language = Language.HarmonyHeaven.Menu

    self.data_cares = {
		{data = self.data.fb_info.scene_info, func = self.FlushInfo},
		{data = self.data.fb_info.reward_info, func = self.FlushRewards},
    }
end

-- function HarmonyHeavenMenuView:OnDisable()
-- 	UIWFlushPanel.OnDisable(self)
-- 	-- TimeHelper:CancelTimer(self.timer_ct)
-- end

function HarmonyHeavenMenuView:FlushInfo()
    local scene_info = self.data.fb_info.scene_info
    if scene_info.init then return end
    local info = Cfg.PromevalParadiseBossFbId(scene_info.fb_id)
    local info2 = Cfg.PromevalParadiseTaskNpcId(scene_info.fb_id)
    local stage = scene_info.stage
    
    self.NameShow.text = string.format(self.language.NameShow, self.language.TypeShow[info.fb_type], info.fb_name)
    self.DescShow.text = string.format(self.language.DescShow, info2["step_" .. (stage + 1)] or "")

    for i = 1, self.Points:Length() do
        self.Points[i].SpriteName = (stage + 1) >= i and "TeShuDian" or "PuTongDian"
    end
    for i = 1, self.Lines:Length() do
        self.Lines[i].SpriteName = (stage + 1) > i and "FenGeXian1" or "FenGeXian2"
    end
    self.StateShow.text = self.language.StateShow[stage] or ""

    self.BtnsObj:SetActive(stage < 3)
    self.ArrowPos.localPosition = Vector2(-103 + math.min(2, stage) * 52, 38)
end

function HarmonyHeavenMenuView:FlushRewards()
    local reward_info = self.data.fb_info.reward_info
    if reward_info.init then return end
    local rewards = DataHelper.FormatItemList(reward_info.item_list, 1)
    table.sort(rewards,function(a,b)
                return a:Color() > b:Color()

        end
    )
    self.RewardList:SetData(rewards)
    self.EmptyObj:SetActive(0 == #rewards)
end

function HarmonyHeavenMenuView:OnClickReward()
	ViewMgr:OpenView(HarmonyHeavenMenuRewardView)
end

function HarmonyHeavenMenuView:OnClickFight()
    self.data:AutoToNpc()
end


HarmonyHeavenMenuRewardView = HarmonyHeavenMenuRewardView or DeclareView("HarmonyHeavenMenuRewardView", HarmonyHeavenConfig.ResPath .. "harmony_heaven_menu_reward")

function HarmonyHeavenMenuRewardView:HarmonyHeavenMenuRewardView()
    self.data = HarmonyHeavenCtrl.Instance:Data()
	self.language = Language.HarmonyHeaven.Menu
end

function HarmonyHeavenMenuRewardView:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.RewardTitle, Vector2.New(496,335))
end

function HarmonyHeavenMenuRewardView:OnFlush()
    local scene_info = self.data.fb_info.scene_info
    if scene_info.init then return end
    local stage = scene_info.stage
    local info = Cfg.PromevalParadiseBossFbId(scene_info.fb_id)
    local monsters = self.data:GetMonstersShow(info)
	self.MonsterList:SetData(monsters)
    self.MonsterList:ClickItem(math.min(stage + 1, 3))
end

function HarmonyHeavenMenuRewardView:OnClickMonster(monster)
    local scene_info = self.data.fb_info.scene_info
    if scene_info.init then return end
    local info = Cfg.PromevalParadiseBossFbId(scene_info.fb_id)
	local rewards = self.data:GetMonsterRewardsShow(info, monster.index)
	self.RewardList:SetData(rewards)
end

HarmonyHeavenMenuRewardViewMonsterItem = DeclareMono("HarmonyHeavenMenuRewardViewMonsterItem", UIWidgetBaseItem)

function HarmonyHeavenMenuRewardViewMonsterItem:HarmonyHeavenMenuRewardViewMonsterItem()
	self.hh_data = HarmonyHeavenCtrl.Instance:Data()
	self.language = Language.HarmonyHeaven.Main
end

function HarmonyHeavenMenuRewardViewMonsterItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function HarmonyHeavenMenuRewardViewMonsterItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetIcon(self.IconSp, data.icon_id, ICON_TYPE.ITEM)
    self.NameShow.text = data.name
end