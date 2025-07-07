TowerAdventureLevelView = TowerAdventureLevelView or DeclareView("TowerAdventureLevelView", "mini_game/tower_adventure_level", Mod.Challenge.TowerAdventure)
VIEW_DECLARE_LEVEL(TowerAdventureLevelView, ViewLevel.Lt)
VIEW_DECLARE_MASK(TowerAdventureLevelView, ViewMask.BgBlock)
function TowerAdventureLevelView:TowerAdventureLevelView()
    -- 塔楼小游戏选关界面
end

function TowerAdventureLevelView:OnCloseClick()
    ViewMgr:OpenView(ChallengeNaviView)
    ViewMgr:CloseView(TowerAdventureLevelView)
end

function TowerAdventureLevelView:CloseCallback()
    -- 关掉界面的时候要开始自动任务
    if LoginData.Instance:LoginState() == LoginStateType.Logined then
        TaskCtrl.Instance:OnMainRolePosChange()
    end
end

----------------------------TowerAdventureLevelPanel----------------------------
TowerAdventureLevelPanel = TowerAdventureLevelPanel or DeclareMono("TowerAdventureLevelPanel", UIWFlushPanel)
function TowerAdventureLevelPanel:TowerAdventureLevelPanel()
    self.Data = TowerAdventureData.Instance
    self.language = Language.TowerAdventure
    self.left_config = MiniGameData.Instance:GetLeftListConfig()
    self.level_config = MiniGameData.Instance:GetLevelConfig()
    self.Data:CalculateNowPage()
    self.data = self.Data.level_data
    self.data_cares = {
        {data = self.data, func = self.FlushPage, keys = {"now_page"}, init_call = false},
        {data = self.data, func = self.FlushByData, keys = {"now_level", "reward_flags"}, init_call = false},
    }
    self.is_load = false
    -- 第一~六关指引按钮
    for i = 1, 6 do
        GuideManager.Instance:RegisterGetGuideUi("TowerAdventureLevelCenterItems" .. i,function ()
            return self.TowerAdventureLevelCenterItems[i].gameObject
        end)
    end
end

function TowerAdventureLevelPanel:Awake()
    UIWFlushPanel.Awake(self)
    -- 判断是否已经登录，没登录不显示返回按钮
    self.ObjClose:SetActive(LoginData.Instance:LoginState() == LoginStateType.Logined)
    self.LeftList:SetData(self.left_config)
    self.LeftList:SetectItem(self.data.now_page, true)
    self:FlushLeftRemind()
    self:FlushPage()
    self:DialogCheck()
end

function TowerAdventureLevelPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

-- 检测是否需要对话，并且是否需要指引
function TowerAdventureLevelPanel:DialogCheck()
    local dialog_config = MiniGameData.Instance:DialogConfig()
    local now_level = self.data.now_level + 1
    local talk_type = LoginData.Instance:LoginState() == LoginStateType.Logined and MiniGameTalkType.Normal or MiniGameTalkType.First
    if dialog_config[now_level] and dialog_config[now_level][talk_type] then
        local now_dialog = dialog_config[now_level][talk_type]
        if not StringIsEmpty(tostring(now_dialog.npc_id_1)) and not StringIsEmpty(tostring(now_dialog.txt_1)) then
            local end_func = function()
                if not StringIsEmpty(tostring(now_dialog.step_id)) then
                    GuideManager.Instance:Start(now_dialog.step_id)
                end
            end
            MiniGameCtrl.Instance:OpenDialogView(now_dialog.npc_id_1, now_dialog.txt_1, end_func)
        end
    end
end

function TowerAdventureLevelPanel:FlushPage()
    self:FlushBgImg()
    self:FlushCenterLevel()
    self:FlushProgress()
end

function TowerAdventureLevelPanel:FlushCenterLevel()
    for i = 1, self.TowerAdventureLevelCenterItems:Length() do
        self.TowerAdventureLevelCenterItems[i]:SetData(self.level_config[self.data.now_page][i])
    end
end

function TowerAdventureLevelPanel:FlushByData()
    self:FlushLeftRemind()
    self:FlushCenterLevel()
    self:FlushProgress()
end

function TowerAdventureLevelPanel:FlushLeftRemind()
    for k, v in pairs(self.LeftList.item_list) do
        if v.FlushRemind then
            v:FlushRemind()
        end
    end
end

function TowerAdventureLevelPanel:FlushProgress()
    for i = 1, self.TowerAdventureLevelButtomItems:Length() do
        self.TowerAdventureLevelButtomItems[i]:SetData(self.left_config[self.data.now_page][i])
    end
    if self.data.now_level < self.Data:GetLevelByPageAndLevel(self.data.now_page, 1) then
        self.Progress:SetProgress(0)
    elseif self.data.now_level >= self.Data:GetLevelByPageAndLevel(self.data.now_page, self.Data:PageMaxLevel()) then
        self.Progress:SetProgress(1)
    else
        local page, level = self.Data:GetPageAndLevelByLevel(self.data.now_level)
        self.Progress:SetProgress(level / self.Data:PageMaxLevel())
    end
end

function TowerAdventureLevelPanel:FlushBgImg()
    local bg_path = Format("ui/textures/tower_adventure/tower_adventure_bg_%s.png", self.data.now_page)
    ResMgr:LoadObject(bg_path, function(tex)
        if not self:Destroyed() then
            if self.RawImgBg1 then
                self.RawImgBg1.texture = tex
                self.is_load = true
            end
        end
    end, false)
end

function TowerAdventureLevelPanel:IsLoaded()
    return self.is_load
end

----------------------------TowerAdventureLevelLeftItem----------------------------
TowerAdventureLevelLeftItem = TowerAdventureLevelLeftItem or DeclareMono("TowerAdventureLevelLeftItem", UIWidgetBaseItem)
function TowerAdventureLevelLeftItem:TowerAdventureLevelLeftItem()
    self.Data = TowerAdventureData.Instance
end
function TowerAdventureLevelLeftItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, Language.TowerAdventure.LevelName[data[1].plies])
    self:FlushRemind()
end

function TowerAdventureLevelLeftItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.Data.level_data.now_page = self.data[1].plies
    end
end

function TowerAdventureLevelLeftItem:FlushRemind()
    self.RedPoint:SetNum(self.Data:LeftRemind(self.data[1].plies))
    self.IHandler.Interactable = self.Data.level_data.now_level >= self.Data:PageMaxLevel() * (self.data[1].plies - 1)
    self.ObjAllClear:SetActive(self.Data.level_data.now_level >= self.Data:PageMaxLevel() * self.data[1].plies)
end

----------------------------TowerAdventureLevelCenterItem----------------------------
TowerAdventureLevelCenterItem = TowerAdventureLevelCenterItem or DeclareMono("TowerAdventureLevelCenterItem", UIWidgetBaseItem)
function TowerAdventureLevelCenterItem:TowerAdventureLevelCenterItem()
    self.Data = TowerAdventureData.Instance
end

function TowerAdventureLevelCenterItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Txt, data.plies_seq)
    if self.Points then
        for i = 1, self.Points:Length() do
            UH.SpriteName(self.Points[i], "Dian" .. ((self.Data.level_data.now_level + 1) >= data.seq and 1 or 2))
        end
    end
    local show_limit = (RoleLevel() or 1) >= self.data.levels
    local not_grey = show_limit and (self.Data.level_data.now_level + 1) >= data.seq
    self.IHandler.Interactable = not_grey
    UH.SetText(self.TxtLevelLimit, Format(Language.TowerAdventure.LevelLimit, data.levels))
    self.TxtLevelLimit:SetObjActive(not show_limit)
    self.ObjClear:SetActive((self.Data.level_data.now_level + 1) > data.seq)
end

function TowerAdventureLevelCenterItem:OnLevelClick()
    if (self.Data.level_data.now_level + 1) >= self.data.seq then
        if (RoleLevel() or 1) >= self.data.levels then
            MiniGameData.Instance:SetLevelDetail(self.data)
            ViewMgr:OpenView(TowerAdventureGameView)
            ViewMgr:CloseView(TowerAdventureLevelView)
        else
            PublicPopupCtrl.Instance:Center(Format(Language.TowerAdventure.Tip[2], self.data.levels))
        end
    else
        PublicPopupCtrl.Instance:Center(Language.TowerAdventure.Tip[1])
    end
end

----------------------------TowerAdventureLevelButtomItem----------------------------
TowerAdventureLevelButtomItem = TowerAdventureLevelButtomItem or DeclareMono("TowerAdventureLevelButtomItem", UIWidgetBaseItem)
function TowerAdventureLevelButtomItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_receive = TowerAdventureData.Instance:RewardFlagsByPageNum(data.plies, data.award_num)
    data.reward_item.is_grey = is_receive == 1
    self.Cell:SetData(Item.Create(data.reward_item))
    UH.SetText(self.Txt, Format(Language.TowerAdventure.NeedReach, data.show_seq))
    local remind = TowerAdventureData.Instance:ButtomRemind(data.plies, data.award_num)
    self.RedPoint:SetNum(remind)
    self.ObjReceive:SetActive(remind > 0)
end

function TowerAdventureLevelButtomItem:OnReceiveClick()
    MiniGameCtrl.Instance:SendMiniGameGetPhaseReward(self.data.seq)
end