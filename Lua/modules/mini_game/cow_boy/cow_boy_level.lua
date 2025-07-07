CowBoyLevelView = CowBoyLevelView or DeclareView("CowBoyLevelView", "mini_game/cow_boy_level", Mod.Challenge.CowBoy)
VIEW_DECLARE_LEVEL(CowBoyLevelView, ViewLevel.Lt)
VIEW_DECLARE_MASK(CowBoyLevelView, ViewMask.BgBlock)
function CowBoyLevelView:CowBoyLevelView()
end

function CowBoyLevelView:LoadCallback(param)
    if param and param.close_game_view then
        ViewMgr:CloseView(CowBoyGameView)
    end
    TaskCtrl.Instance:CancelAutoMainTimeHandle()
    if MainPlayer then
        MainPlayer:StopMove()
    end
end

function CowBoyLevelView:OnCloseClick()
    self.CloseBtnAnim:Play()
    ViewMgr:OpenView(ChallengeNaviView)
    ViewMgr:CloseView(CowBoyLevelView)
end

function CowBoyLevelView:CloseCallback()
    -- 关掉界面的时候要开始自动任务
    if LoginData.Instance:LoginState() == LoginStateType.Logined then
        TaskCtrl.Instance:OnMainRolePosChange()
    end
end

----------------------------CowBoyLevelPanel----------------------------
CowBoyLevelPanel = CowBoyLevelPanel or DeclareMono("CowBoyLevelPanel", UIWFlushPanel)
function CowBoyLevelPanel:CowBoyLevelPanel()
    self.Data = CowBoyData.Instance
    self.language = Language.CowBoy
    self.left_config = self.Data:GetLeftListConfig()
    self.level_config = self.Data:GetLevelConfig()
    self.Data:CalculateNowPage()
    self.data = self.Data.level_data
    self.data_cares = {
        {data = self.data, func = self.FlushPage, keys = {"now_page"}, init_call = false},
        {data = self.data, func = self.FlushByData, keys = {"now_level", "reward_flags"}, init_call = false},
    }
    -- 第一~六关指引按钮
    for i = 1, 6 do
        GuideManager.Instance:RegisterGetGuideUi("CowBoyLevelCenterItems" .. i,function ()
            return self.CowBoyLevelCenterItems[i].gameObject
        end)
    end
end

function CowBoyLevelPanel:Awake()
    UIWFlushPanel.Awake(self)
    -- 判断是否已经登录，没登录不显示返回按钮
    self.ObjClose:SetActive(LoginData.Instance:LoginState() == LoginStateType.Logined)
    self.LeftList:SetData(self.left_config)
    self.LeftList:SetectItem(self.data.now_page, true)
    self:FlushLeft()
    self:FlushPage()
    self:DialogCheck()
end

function CowBoyLevelPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

-- 检测是否需要对话，并且是否需要指引
function CowBoyLevelPanel:DialogCheck()
    local dialog_config, step_id = MiniGameData.Instance:DialogConfig()
    if not TableIsEmpty(dialog_config) then
        local end_func = function()
            if step_id ~= 0 then
                GuideManager.Instance:Start(step_id)
            end
        end
        MiniGameCtrl.Instance:OpenDialogView(nil,nil,end_func, dialog_config, true, true)
    end
end

function CowBoyLevelPanel:FlushPage()
    self:FlushLeft()
    self:FlushCenterLevel()
    self:FlushReward()
end

function CowBoyLevelPanel:FlushReward()
    if self.Data:RewardShow() then
        self.RewardObj:SetActive(true)
        local reward_state = self.Data:GetRewardState()
        self.RewardCanGetRed:SetActive(reward_state == CowBoyRewardState.CanGet)
        self.RewardCanGetL:SetActive(reward_state == CowBoyRewardState.CanGet)
        self.RewardCanGet:SetActive(reward_state == CowBoyRewardState.CanGet)
        self.RewardLock:SetActive(reward_state == CowBoyRewardState.NotEnough)
        self.RewardGot:SetActive(reward_state == CowBoyRewardState.Got)
        local reward_item = self.Data:GetRewardItem()
        if reward_item then
            self.RewardItemCell:SetData(reward_item)
            UH.SetText(self.RewardItemName, reward_item:Name())
        end
    else
        self.RewardObj:SetActive(false)
    end
end

function CowBoyLevelPanel:OnClickReward()
    local reward_state = self.Data:GetRewardState()
    if reward_state == CowBoyRewardState.NotEnough then
        PublicPopupCtrl.Instance:Center(Language.CowBoy.NotEnough)
    elseif reward_state == CowBoyRewardState.Got then
        PublicPopupCtrl.Instance:Center(Language.CowBoy.Got)
    else
        MiniGameCtrl.Instance:SendMiniGameGetPhaseReward(self.Data.level_data.now_page)
    end
end

function CowBoyLevelPanel:FlushCenterLevel()
    for i = 1, self.CowBoyLevelCenterItems:Length() do
        self.CowBoyLevelCenterItems[i]:SetData(self.level_config[self.data.now_page][i])
    end
end

function CowBoyLevelPanel:FlushByData()
    self:FlushCenterLevel()
    self:FlushReward()
end

function CowBoyLevelPanel:FlushLeft()
    for k, v in pairs(self.LeftList.item_list) do
        if v.FlushShow then
            v:FlushShow()
        end
    end
end

----------------------------CowBoyLevelLeftItem----------------------------
CowBoyLevelLeftItem = CowBoyLevelLeftItem or DeclareMono("CowBoyLevelLeftItem", UIWidgetBaseItem)
function CowBoyLevelLeftItem:CowBoyLevelLeftItem()
    self.Data = CowBoyData.Instance
end
function CowBoyLevelLeftItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.NorNameImg,"_LocTuHuangZi"..data[1].plies)
    UH.SpriteName(self.SelNameImg,"_LocMiaoBianHongZi"..data[1].plies)
    self:FlushShow()
end

function CowBoyLevelLeftItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.Data.level_data.now_page = self.data[1].plies
    end
end

function CowBoyLevelLeftItem:FlushShow()
    self.IHandler.Interactable = self.Data.level_data.now_level >= self.Data:PageMaxLevel() * (self.data[1].plies - 1)
    self.ObjAllClear:SetActive(self.Data.level_data.now_level >= self.Data:PageMaxLevel() * self.data[1].plies)
end

----------------------------CowBoyLevelCenterItem----------------------------
CowBoyLevelCenterItem = CowBoyLevelCenterItem or DeclareMono("CowBoyLevelCenterItem", UIWidgetBaseItem)
function CowBoyLevelCenterItem:CowBoyLevelCenterItem()
    self.Data = CowBoyData.Instance
end

function CowBoyLevelCenterItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Txt, data.level_name)
    local not_grey = (self.Data.level_data.now_level + 1) >= data.seq
    self.IHandler.Interactable = not_grey
    self.ObjNext:SetActive((self.Data.level_data.now_level + 1) == data.seq)
end

function CowBoyLevelCenterItem:OnLevelClick()
    if (self.Data.level_data.now_level + 1) >= self.data.seq then
        MiniGameData.Instance:SetLevelDetail(self.data)
        ViewMgr:OpenView(CowBoyGameView,{close_level_view = true})
        -- ViewMgr:CloseView(CowBoyLevelView)
    else
        PublicPopupCtrl.Instance:Center(Language.CowBoy.Tip[1])
    end
end
