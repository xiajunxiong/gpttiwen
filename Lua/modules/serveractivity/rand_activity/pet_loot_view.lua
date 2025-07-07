PetLootView = PetLootView or DeclareView("PetLootView", "serveractivity/pet_loot_view", Mod.Splendid.PetLoot)
VIEW_DECLARE_MASK(PetLootView, ViewMask.BgBlock)
VIEW_DECLARE_LEVEL(PetLootView, ViewLevel.L1)

function PetLootView:PetLootView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_PET_LOOT, TimeType.Type_Special_4)
    self.Time:SetCallBack(function()
		PublicPopupCtrl.Instance:Center(Language.PetLoot.ActivityCloseTip)
		ViewMgr:CloseView(PetLootView)
	end)
end

function PetLootView:CloseCallback()
    self.Time:CloseCountDownTime()
end

function PetLootView:OnCloseClick()
    ViewMgr:CloseView(PetLootView)
end

function PetLootView:GoToPetClick()
    ViewMgr:OpenViewByKey(Mod.Pet.PetStrChange)
end


----------------------------PetLootPanel----------------------------
PetLootPanel = PetLootPanel or DeclareMono("PetLootPanel", UIWFlushPanel)

function PetLootPanel:PetLootPanel()
    self.pet_loot_data = ServerActivityData.Instance.pet_loot_data
    self.data_cares = {
        {data = self.pet_loot_data, func = self.OnFlushView, init_call = false},
    }
    self.now_page = 1
    self.one_page_count = 3
    self:OnFlushView()
end

-- 监听刷新界面
function PetLootPanel:OnFlushView()
    self.config = ServerActivityData.Instance:GetPetLootConfig()
    self.list_info = self.config.list_info
    if not self.max_mage then
        self.max_mage = math.ceil(#self.list_info / self.one_page_count)
    end
    self:GroupConfig()
    self:OnFlushPage()
    self:OnFlushBox()
end

-- 数据分组
function PetLootPanel:GroupConfig()
    self.page_config = {}
    for i = 1, self.max_mage do
        self.page_config[i] = {}
    end
    for i, v in ipairs(self.list_info) do
        table.insert(self.page_config[math.ceil(i / self.one_page_count)], v)
    end
end

-- 点击翻页，左-1，右1
function PetLootPanel:OnChangePageClick(change)
    self.now_page = self.now_page + change
    self.now_page = self.now_page > self.max_mage and self.max_mage or self.now_page
    self.now_page = self.now_page < 1 and 1 or self.now_page
    self:OnFlushPage()
end

-- 刷新页面显示
function PetLootPanel:OnFlushPage()
    self.btn_left:SetActive(self.now_page ~= 1)
    self.btn_right:SetActive(self.now_page ~= self.max_mage)
    self.ItemList:SetData(self.page_config[self.now_page])
end

-- 更新阶段展示奖励
function PetLootPanel:OnFlushReward()
    self.now_show_info, self.is_last_receive = ServerActivityData.Instance:GetPetLootShowInfo()
    self.obj_regular_item_mask:SetActive(self.is_last_receive)
    local item = Item.New(self.now_show_info.show_regular_item)
    self.RegularItemCell:SetData(item)
    UH.SetText(self.txt_regular_item_name, item:Name())
    self.RewardList:SetData(self.now_show_info.show_rand_item)
end

-- 更新宝箱
function PetLootPanel:OnFlushBox()
    self:OnFlushReward()
    local remind_num = ServerActivityData.Instance:GetPetLootShowRemind()
    self.RewardRedPoint:SetNum(remind_num)
    self.obj_box_effect:SetActive(remind_num == 1)
    UH.SetText(self.txt_reward, Format(Language.PetLoot.RewardProgress, self.config.task_finish_times, self.now_show_info.task_num))
end

-- 点击宝箱，如果可以领取则领取，否则展开奖励
function PetLootPanel:OnBoxClick()
    if ServerActivityData.Instance:GetPetLootShowRemind() == 0 then
        self.obj_block:SetActive(true)
        self.obj_reward_bg:SetActive(true)
    else
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_PET_LOOT, 1, 2, self.now_show_info.type)
    end
end

-- 点击蒙蔽关闭展示
function PetLootPanel:OnBlockClick()
    self.obj_block:SetActive(false)
    self.obj_reward_bg:SetActive(false)
end


----------------------------PetLootItem----------------------------
PetLootItem = PetLootItem or DeclareMono("PetLootItem", UIWidgetBaseItem)

function PetLootItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushTitleView()
    self:SetProgress()
    self.Cell:SetData(Item.New(data.reward_item))
    self.RedPoint:SetNum(data.task_info.can_get_reward_times > 0 and 1 or 0)
    self.ReceiveHandle.Interactable = data.task_info.can_get_reward_times > 0
    self.ReceiveGray.enabled = data.task_info.can_get_reward_times == 0
    UH.SetText(self.txt_max, Format(Language.PetLoot.ItemProgress, data.task_info.has_get_reward_times, data.reward_times))
    self.obg_max_bg:SetActive(data.reward_times > data.task_info.has_get_reward_times)
end

-- 设置名字
function PetLootItem:FlushTitleView()
    self.TitleList:SetData({})
    local names_list = {}
    local split_list = Split(self.data.pet_type, "|")
    for i, v in ipairs(split_list) do
        names_list[i] = {title = Language.PetLoot.PetType[tonumber(v)], is_show = i ~= #split_list}
    end
    self.TitleList:SetData(names_list)
    UH.SetText(self.txt_title2, Format(Language.PetLoot.IntensifyType[self.data.int_type], self.data.parameter))
end

-- 设置进度条
function PetLootItem:SetProgress()
    -- 根据情况强化次数和强化等级的显示不一样，1强化次数，2强化等级
    local rate = 1
    local pro = ""
    if self.data.int_type == 1 then
        -- 判断是否有领取完
        if self.data.task_info.has_get_reward_times >= self.data.reward_times then
            pro = string.format("%s/%s", self.data.parameter, self.data.parameter)
        else
            if self.data.task_info.can_get_reward_times == 0 then
                rate = self.data.task_info.param / self.data.parameter
            end
            pro = string.format("%s/%s", self.data.task_info.can_get_reward_times * self.data.parameter + self.data.task_info.param, self.data.parameter)
            -- 判断是否已经可以领取全部奖励了
            if self.data.task_info.has_get_reward_times + self.data.task_info.can_get_reward_times >= self.data.reward_times then
                pro = string.format("%s/%s", self.data.task_info.can_get_reward_times * self.data.parameter, self.data.parameter)
            end
        end
    elseif self.data.int_type == 2 then
        if self.data.task_info.can_get_reward_times == 0 and self.data.reward_times > self.data.task_info.has_get_reward_times then
            rate = 0
        end
        -- 判断是否有领取完
        if self.data.task_info.has_get_reward_times >= self.data.reward_times then
            pro = string.format("%s/%s", 1, 1)
        else
            pro = string.format("%s/%s", self.data.task_info.can_get_reward_times, 1)
        end
    else
        LogError("宠物夺宝表中强化类型不能为" .. self.data.int_type)
        return
    end
    self.Progress:SetProgress(rate > 1 and 1 or rate)
    self.Progress:SetText(pro)
end

-- 点击领取
function PetLootItem:OnReceiveClick()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_PET_LOOT, 1, 1, self.data.seq)
end

----------------------------PetLootTitleItem----------------------------
PetLootTitleItem = PetLootTitleItem or DeclareMono("PetLootTitleItem", UIWidgetBaseItem)
function PetLootTitleItem:SetData(data)
    UH.SetText(self.Type,data.title)
    self.Or:SetActive(data.is_show)
end

----------------------------PetLootRewardItem----------------------------
PetLootRewardItem = PetLootRewardItem or DeclareMono("PetLootRewardItem", UIWidgetBaseItem)
function PetLootRewardItem:SetData(data)
    local item = Item.New(data)
    self.ItemCell:SetData(item)
    UH.SetText(self.txt_name, item:Name())
end