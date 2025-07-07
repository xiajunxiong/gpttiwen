CourageChallengeAchievement = CourageChallengeAchievement or DeclareView("CourageChallengeAchievement", "courage_challenge/courage_challenge_achievement")
function CourageChallengeAchievement:CourageChallengeAchievement()
    
end
function CourageChallengeAchievement:LoadCallback()
    
end

CourageChallengeAchievementPanel = CourageChallengeAchievementPanel or DeclareMono("CourageChallengeAchievementPanel", UIWFlushPanel)
function CourageChallengeAchievementPanel:CourageChallengeAchievementPanel()
    self.data = CourageChallengeData.Instance
    self.data_cares = {
        {data = self.data:GetAchiData(), func = self.FlushAchi},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(),func = self.SendBuy, init_call = false},
    }
end
function CourageChallengeAchievementPanel:FlushAchi()
    self:onFlush()
end

function CourageChallengeAchievementPanel:onFlush()
    --获取是否解锁
    local info = self.data:GetAchiData()
    local is_lock = (info.token_is_buy or 0) == 0
    self.LockTexObj:SetActive(is_lock)
    -- self.BtnOneKey:SetActive(not is_lock)
    if self.cache_lock == nil then
        self.cache_lock = is_lock
        if not is_lock then
            self.LockAnim:Play()
        end
    else
        if self.cache_lock ~= is_lock and not is_lock then
            self.LockAnim:Play()
            self.EffectTool:Play(7165465)
            self.cache_lock = is_lock
        end
    end
    UH.SetText(self.Tip, string.format(Language.CourageChallenge.AchiTip, self.data.other_info.reward_prize))
    UH.SetText(self.LockTex, is_lock and string.format(Language.CourageChallenge.AchiPrize, self.data.other_info.reward_prize) or "")
    UH.SpriteName(self.LockIcon, HuoBi[self.data.other_info.price_type])
    -- self.Lock:SetActive(is_lock)
    -- self.List:SetData(self.data:GetAchiList())
    local list = self.data:GetAchiList()
    local total = self.data:GetTotalStar2()
    local idx = 0
    local is_can_get1 = false
    local is_can_get2 = false
    for i, v in ipairs(list) do
        is_can_get1 = self.data:AchiCanGet1(v)
        is_can_get2 = self.data:AchiCanGet2(v)
        if is_can_get1 or is_can_get2 then
            idx = i
            -- LogDG(v.star_num)
            break
        end
    end
    if idx == 0 then
        for i, v in ipairs(list) do
            if v.star_num >= total then
                idx = i
                -- LogDG(idx, v.star_num)
                break
            end
        end
    end
    local value = 0
    if idx <= 3 then
        -- idx = idx - idx
        value = 0
    else
        value = (idx/#list) - 0.029
    end
    self.List2:SetData(list)
    -- self.List2:JumpToRight(idx/#list)
    UH.AnchoredPosition(self.ContentRect, Vector2.New(-16560* value, 0))
end
function CourageChallengeAchievementPanel:OnClickTest()
    local idx = 76
    if idx <= 3 then
        idx = idx - idx
    -- else
    --     idx = idx - 3.5
    end
    local list = self.data:GetAchiList()
    LogDG(list[idx].star_num)
    -- self.List2:SetData(list)
    local value = (idx/#list) - 0.029
    LogDG(idx/#list, #list, value)
    LogDG(-16560* value)
    self.List2:JumpToRight(value)
end
function CourageChallengeAchievementPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.cache_lock = nil
end
function CourageChallengeAchievementPanel:OnClickBuy()
    if self.data:GetAchiData().token_is_buy == 1 then
        return
    end
    local param_t = {
        ok_func = function ()
            ServerActivityCtrl.Instance:SendCmdBuyItemCheckReq(BUY_ITEM_TYPE.TYPE_NEW_COURAGE_CHALLENGE, self.data.other_info.reward_prize)
        end,
        content_text = string.format(Language.CourageChallenge.BuySuper, self.data.other_info.reward_prize),
    }
    ViewMgr:OpenView(GodTokenTipView,param_t)
end
function CourageChallengeAchievementPanel:OnClikOneKey()
    CourageChallengeCtrl.Instance:SendNewCourageChallengeReq(4)
end
function CourageChallengeAchievementPanel:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_NEW_COURAGE_CHALLENGE, self.data.other_info.reward_prize * 10)
end
function CourageChallengeAchievementPanel:OnClickClose()
    ViewMgr:CloseView(CourageChallengeAchievement)
end

CourageChallengeAchievementItem = CourageChallengeAchievementItem or DeclareMono("CourageChallengeAchievementItem", UIWidgetBaseItem)
function CourageChallengeAchievementItem:CourageChallengeAchievementItem()
    self.cour_data = CourageChallengeData.Instance
end
function CourageChallengeAchievementItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = self.cour_data:GetAchiData()
    UH.SetText(self.Desc, string.format(Language.CourageChallenge.AchiGet, data.star_num))
    local item = Item.Create(data.reward[0])
    self.Cell1:SetData(item)
    local item2 = Item.Create(data.higher_reward[0])
    self.Cell2:SetData(item2)
    local item3 = Item.Create(data.higher_reward[1])
    self.Cell3:SetData(item3)
    local is_get1 = self.cour_data:IsGet1(self.data.reward_seq)
    local is_get2 = self.cour_data:IsGet2(self.data.reward_seq)
    local is_lock = (info.token_is_buy or 0) == 0
    local is_can_get1 = self.cour_data:AchiCanGet1(self.data)
    local is_can_get2 = self.cour_data:AchiCanGet2(self.data)
    
    self.Gou1:SetActive(is_get1)
    self.Gou2:SetActive(is_get2)
    self.Click1:SetActive(is_can_get1)
    self.Click2:SetActive(is_can_get2)
    self.RedPoint:SetNum(is_can_get1 and 1 or 0)
    self.Lock:SetActive(not is_get2 and not is_can_get2)
    self.Lock1:SetActive(not is_get1 and not is_can_get1)
end

function CourageChallengeAchievementItem:OnClickGet1()
    CourageChallengeCtrl:SendNewCourageChallengeReq(2, self.data.reward_seq)
end
function CourageChallengeAchievementItem:OnClickGet2()
    local info = self.cour_data:GetAchiData()
    --[[ local is_lock = (info.token_is_buy or 0) == 0
    if is_lock then
        PublicPopupCtrl.Instance:Center(Language.CourageChallenge.AchiLock)
        CellClicks.BagGridNoButton(self.Cell2)
        return
    end ]]
    local is_get1 = self.cour_data:IsGet1(self.data.reward_seq)
    if not is_get1 then
        CourageChallengeCtrl:SendNewCourageChallengeReq(2, self.data.reward_seq)
    end
    CourageChallengeCtrl:SendNewCourageChallengeReq(3, self.data.reward_seq)
end