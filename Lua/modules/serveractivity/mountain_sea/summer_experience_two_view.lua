SummerExperienceTwoView = SummerExperienceTwoView or DeclareView("SummerExperienceTwoView", "serveractivity/mountain_sea/summer_experience_two", Mod.MountainSea.SummerExperienceTwo)
VIEW_DECLARE_LEVEL(SummerExperienceTwoView, ViewLevel.L1)
VIEW_DECLARE_MASK(SummerExperienceTwoView, ViewMask.BgBlock)
function SummerExperienceTwoView:SummerExperienceTwoView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SUMMER_EXPERIENCE_TWO, TimeType.Type_Time_3)
end

function SummerExperienceTwoView:OnCloseClick()
    ViewMgr:CloseView(SummerExperienceTwoView)
end

function SummerExperienceTwoView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[132].desc)
end


----------------------------SummerExperienceTwoPanel----------------------------
SummerExperienceTwoPanel = SummerExperienceTwoPanel or DeclareMono("SummerExperienceTwoPanel", UIWFlushPanel)
function SummerExperienceTwoPanel:SummerExperienceTwoPanel()
    self.config = ServerActivityData.Instance:GetSummerExperienceTwoConfig()
    self.reward_group = Config.shanhaizhengfeng_auto.reward_group
    self.other = Config.shanhaizhengfeng_auto.other[1]
    self.language = Language.SummerExperience
    self.data = ServerActivityData.Instance.summer_experience_two_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = true},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushByItem, init_call = false}
    }
    self.now_show_data = nil
    self.config_num = #self.config
    self.BossList:SetData(self.config)
    if self.data.is_fight_open then
        self.BossList:SetectItem(self.data.fight_boss_num, true)
        self:FlushBossShow(self.config[self.data.fight_boss_num])
        self.data.is_fight_open = false
    else
        if self.data.first_flag_num == self.config_num then
            self.BossList:SetectItem(self.data.first_flag_num, true)
            self:FlushBossShow(self.config[self.data.first_flag_num])
        else
            local next_boss = self.data.first_flag_num + 1
            if self.config[next_boss].open_time then
                self.BossList:SetectItem(self.data.first_flag_num, true)
                self:FlushBossShow(self.config[self.data.first_flag_num])
            else
                self.BossList:SetectItem(next_boss, true)
                self:FlushBossShow(self.config[next_boss])
            end
        end
    end
    self:CalculateModel()
end

function SummerExperienceTwoPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function SummerExperienceTwoPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
end

-- 这里根据宽高比计算模型的缩放
function SummerExperienceTwoPanel:CalculateModel()
    local perfect_w = 1425
    local perfect_h = 750
    local perfect_aspect = perfect_w / perfect_h
    local screenSize = Vector2.New(UnityEngine.Screen.width, UnityEngine.Screen.height)
    local aspect = screenSize.x / screenSize.y
    local scale_cache = 1
    if aspect > perfect_aspect then
        scale_cache = aspect / perfect_aspect
    end
    UH.LocalScaleG(self.ObjModelShow, Vector3.New(scale_cache, scale_cache, scale_cache))
end

function SummerExperienceTwoPanel:FlushAll()
    local color = self.data.first_flag_num == self.config_num and COLORSTR.Green9 or COLORSTR.Red8
    UH.SetText(self.TxtFirstFlag, Format(Language.Common.Cstr, color, self.data.first_flag_num, self.config_num))
    local remind_num = ServerActivityData.Instance:GetSummerExperienceTwoRemind()
    self.RedPoint:SetNum(remind_num)
    self.ObjBoxEffect:SetActive(remind_num > 0)
    self.IHandlerBox.Interactable = self.data.is_fetch_box ~= 1
end

function SummerExperienceTwoPanel:FlushByItem()
    UH.SetIcon(self.ImgSetItem, self.other.consume_item, ICON_TYPE.ITEM)
    local has_num = Item.GetNum(self.other.consume_item)
    local is_enought = has_num >= self.other.num
    self.IHBtnChallange.Interactable = (self.data.open_day >= self.now_show_data.activity_time) and is_enought
    self.ObjBtnEffect:SetActive(self.IHBtnChallange.Interactable)
    UH.SetText(self.TxtItemHas, has_num)
end

function SummerExperienceTwoPanel:FlushBossShow(data)
    self.now_show_data = data
    local res_path = DrawerHelper.GetNpcPath(data.monster_config.res_id)
    self.ui_obj = self.ModelShow:UiObj()
    if not self.ui_obj then
        self.ui_obj = UIObjDrawer.CreateByPath(res_path)
        self.ModelShow:SetShowData({ui_obj = self.ui_obj, view = self, show_shadow = false})
    else
        self.ui_obj:SetData(res_path)
    end

    -- 刷新右边
    local ele_data = {
        [BattleAttriType.ElementWind] = data.monster_config.wind,
        [BattleAttriType.ElementEarth] = data.monster_config.earth,
        [BattleAttriType.ElementFire] = data.monster_config.fire,
        [BattleAttriType.ElementWater] = data.monster_config.water,
    }
    self.ElementAttri:SetData(ele_data)

    UH.SetIcon(self.ImgSetConsumeIcon, self.other.consume_item, ICON_TYPE.ITEM)
    UH.SetText(self.TxtConsumeNum, Format(self.language.Consume, self.other.num))
    UH.SetText(self.TxtName, data.monster_config.name)
    UH.SetText(self.TxtDesc, Format(self.language.Desc, data.desc))
    self:FlushRewardShow()
    self:FlushByItem()
end

function SummerExperienceTwoPanel:FlushRewardShow()
    local reward_data1 = {}
    local reward_data2 = {}
    local get_tab = function(item, is_first)
        local tab = {}
        tab.item = item
        return tab
    end
    local add_reward_exp_coin = function(list, reward, is_first)
        for k, w in pairs(self.reward_group) do
            if w.reward_group == reward then
                for i, v in pairs(w.reward_item) do
                    table.insert(list, get_tab(v, is_first))
                end
                if w.coin > 0 then
                    table.insert(list, get_tab({item_id = CommonItemId.Coin, is_bind = 1, num = w.coin}, is_first))
                end
                if w.exp > 0 then
                    table.insert(list, {item_id = CommonItemId.Exp, is_bind = 1, num = w.exp}, is_first)
                end
            end
        end
    end
    -- 首通奖励
    add_reward_exp_coin(reward_data1, self.now_show_data.first_reward_group, true)
    -- 普通奖励
    add_reward_exp_coin(reward_data2, self.now_show_data.random_reward_group, false)
    self.RewardList1:SetData(reward_data1)
    self.RewardList2:SetData(reward_data2)
    self.ObjGeted:SetActive(self.data.first_flag[self.now_show_data.activity_time] == 1)
end

function SummerExperienceTwoPanel:Challange()
    if Item.GetNum(self.other.consume_item) < self.other.num then
        PublicPopupCtrl.Instance:Center(self.language.ItemNotEnought, 1, 1)
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.other.consume_item})})
        return
    end
    if self.now_show_data.open_time then
        PublicPopupCtrl.Instance:Center(Format(self.language.NeedDayUnLock, self.now_show_data.open_time.month, self.now_show_data.open_time.day))
        return
    elseif self.data.first_flag[self.now_show_data.activity_time - 1] == 0 then
        PublicPopupCtrl.Instance:Center(self.language.NeedWinLast)
        return
    end
    self.data.is_fight_open = true
    self.data.fight_boss_num = self.now_show_data.activity_time
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SUMMER_EXPERIENCE_TWO, 0, self.now_show_data.activity_time)
end

function SummerExperienceTwoPanel:OnClickBox()
    if self.data.is_fetch_box == 1 then
        PublicPopupCtrl.Instance:Center(self.language.ReceiveTip)
    else
        if ServerActivityData.Instance:GetSummerExperienceTwoRemind() == 1 then
            ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SUMMER_EXPERIENCE_TWO, 1)
        else
            local _, reward = DataHelper.SimpleIndex(self.reward_group, "reward_group", self.other.all_pass_reward_group)
            local tab = {
                pos = UH.GetRootPosition(self.ObjBoxShowPos),
                item_list = DataHelper.FormatItemList(reward.reward_item),
            }
            ViewMgr:OpenView(ShowItemView, tab)
        end
    end
end

function SummerExperienceTwoPanel:OnClickItem()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.other.consume_item})})
end


----------------------------SummerExperienceBossTwoItem----------------------------
SummerExperienceBossTwoItem = SummerExperienceBossTwoItem or DeclareMono("SummerExperienceBossTwoItem", UIWidgetBaseItem)
function SummerExperienceBossTwoItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.gameObject.name = "BossItem_" .. data.activity_time
    data.monster_config = Cfg.MonsterById(data.show_monster_id)
    UH.SetIcon(self.ImgSetIcon, data.monster_config.icon_id, ICON_TYPE.ITEM)
    if data.open_time then
        UH.SetText(self.TxtName, Format(Language.SummerExperience.BossOpenTip, data.open_time.month, data.open_time.day))
    else
        UH.SetText(self.TxtName, data.monster_config.name)
    end
    self:FlushSelect()
end

function SummerExperienceBossTwoItem:FlushSelect()
    if self.data.open_time then
        UH.SpriteName(self.ImgSetBG, "NotOpen")
    else
        UH.SpriteName(self.ImgSetBG, "Normal")
    end
end

function SummerExperienceBossTwoItem:OnSelect(selected)
    self:FlushSelect()
end


----------------------------SummerExperienceRewardTwoItem----------------------------
SummerExperienceRewardTwoItem = SummerExperienceRewardTwoItem or DeclareMono("SummerExperienceRewardTwoItem", UIWidgetBaseItem)
function SummerExperienceRewardTwoItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(Item.Create(data.item))
end