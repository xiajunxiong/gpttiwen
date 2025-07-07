AllSoulsStorybookReward = AllSoulsStorybookReward or DeclareView("AllSoulsStorybookReward", "all_souls_storybook/all_souls_reward")
VIEW_DECLARE_MASK(AllSoulsStorybookReward, ViewMask.BgBlock)
function AllSoulsStorybookReward:AllSoulsStorybookReward()
end
------------------------------------------
AllSoulsStorybookRewardPanel = AllSoulsStorybookRewardPanel or DeclareMono("AllSoulsStorybookRewardPanel", UIWFlushPanel)
function AllSoulsStorybookRewardPanel:AllSoulsStorybookRewardPanel()
    self.data = AllSoulsStorybookData.Instance
    self.reward_type_content_rect = {
        [AllSoulsRewardType.Sample] = Vector2.New(1500, 348),
        [AllSoulsRewardType.Mult] = Vector2.New(1500, 397),
    }
end
--箱子展示，倒计时自动打开（或手动打开） 播放箱子打开特效 然后播奖励展示
function AllSoulsStorybookRewardPanel:onFlush()
    UH.SizeDelta(self.ContentRect, self.reward_type_content_rect[self.data.reward_type])
    UH.SpriteName(self.TitleImg, AllSoulsRewardTitleImgName[self.data.reward_title_type])
    for i = 1, self.RewardLists:Length() do
        self.RewardLists[i]:SetObjActive(self.data.reward_type == i)
        if self.data.reward_type == i then
            self.RewardLists[i]:SetData(self.data:RewardList())
        end
    end
    
    self.handle_start = self.EffectTool:Play(6165115)
    UH.SetText(self.TipsText, Language.AllSoulsStorybook.OpenBoxTip[self.data.reward_title_type])
    self.handle_canopen = TimeHelper:AddDelayTimer(function ()
        self.OpenObj:SetActive(true)
    end, 2)
    if self.data.reward_title_type == AllSoulsRewardTitleType.GetReward then
        self.auto_open_handle = TimeHelper:AddDelayTimer(function ()
            self:OnClickOpen()
        end,4)
    end
    self.HuoBiDescObj:SetActive(false)
    local huobi_desc = self.data.huobi_desc
    if huobi_desc then
        self.HuoBiDescObj:SetActive(true)
        UH.SpriteName(self.HuoBiIcon, HuoBi[huobi_desc.type])
        UH.SetText(self.HuoBiDesc, huobi_desc.num)
    end
end
function AllSoulsStorybookRewardPanel:OnClickOpen()
    self.OpenObj:SetActive(false)
    if self.auto_open_handle ~= nil then
        TimeHelper:CancelTimer(self.auto_open_handle)
    end
    self.auto_close_handle = TimeHelper:AddDelayTimer(function ()
        self:OnClickClose()
    end,10)
    if self.handle_start then
        self.EffectTool:Stop(self.handle_start)
    end
    self.EffectTool:Play(6165114)
    UH.SetText(self.TipsText, "")
    
    self.handle_showreward = TimeHelper:AddDelayTimer(function ()
        -- self.ContentAnim:Play()
        self.ContentObj:SetActive(true)
        if not self.data.hide_fly_text then
            if self.data.reward_type == AllSoulsRewardType.Sample then
                for _,v in pairs(self.data:RewardList()) do
                    ChatCtrl.Instance:ChannelChatCustom {item_get = {item = Item.Create(v), num = v.num}}
                    PublicPopupCtrl.Instance:CenterI(Item.Create(v))
                end
            elseif self.data.reward_type == AllSoulsRewardType.Mult then
                for _,v in pairs(self.data:RewardList()) do
                    for _, vo in pairs(v.list or {}) do
                        ChatCtrl.Instance:ChannelChatCustom {item_get = {item = Item.Create(vo), num = vo.num}}
                        PublicPopupCtrl.Instance:CenterI(Item.Create(vo))
                    end
                end
            end
        end
    end, 1.5)

    self.handle_canclose = TimeHelper:AddDelayTimer(function ()
        UH.SetText(self.TipsText, Language.AllSoulsStorybook.AnyCloseTip)
        self.CloseObj:SetActive(true)
    end, 2)
end
function AllSoulsStorybookRewardPanel:OnClickClose()
    if self.auto_close_handle ~= nil then
        TimeHelper:CancelTimer(self.auto_close_handle)
    end
    ViewMgr:CloseView(AllSoulsStorybookReward)
end
function AllSoulsStorybookRewardPanel:OnDestroy()
    TimeHelper:CancelTimer(self.handle_canopen)
    TimeHelper:CancelTimer(self.handle_canclose)
    TimeHelper:CancelTimer(self.handle_showreward)
    TimeHelper:CancelTimer(self.auto_open_handle)
    TimeHelper:CancelTimer(self.auto_close_handle)
    self.data:doTask(self.data:CurTask())
    UIWFlushPanel.OnDestroy(self)

    -- TimeHelper:AddDelayTimer(function ()
    --     SceneCtrl.Instance:RequestLeaveFb()
    -- end, 1)
    if AllSoulsStorybookCtrl.finish_close_func then
        AllSoulsStorybookCtrl.finish_close_func()
        AllSoulsStorybookCtrl.finish_close_func = nil
    end
end

------------------------------------------
MultRewardLevelItem = MultRewardLevelItem or DeclareMono("MultRewardLevelItem", UIWidgetBaseItem)
function MultRewardLevelItem:MultRewardLevelItem()
end

function MultRewardLevelItem:SetData(data)
    if not StringIsEmpty(data.title_text) then
        UH.SetText(self.TitleText, data.title_text)
        self.TitleTextObj:SetActive(true)
    else
        self.TitleTextObj:SetActive(false)
    end
    if not TableIsEmpty(data.list) then
        self.RewardItemList:SetData(data.list)
    end
end




AllSoulsStorybookRewardsView = AllSoulsStorybookRewardsView or DeclareView("AllSoulsStorybookRewardsView", "all_souls_storybook/all_souls_rewards_view")
VIEW_DECLARE_MASK(AllSoulsStorybookRewardsView, ViewMask.BgBlockClose)
function AllSoulsStorybookRewardsView:AllSoulsStorybookRewardsView()
    self.Board:SetData(self:GetType(), Language.AllSoulsStorybook.Title, Vector2.New(583.0021, 362.0227))
end

function AllSoulsStorybookRewardsView:LoadCallback(data)
    self.List:SetData(data)
end