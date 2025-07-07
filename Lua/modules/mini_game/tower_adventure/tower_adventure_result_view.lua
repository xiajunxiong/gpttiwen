TowerAdventureResultView = TowerAdventureResultView or DeclareView("TowerAdventureResultView", "mini_game/tower_adventure_result")
VIEW_DECLARE_LEVEL(TowerAdventureResultView, ViewLevel.Lt)
VIEW_DECLARE_MASK(TowerAdventureResultView, ViewMask.None)
function TowerAdventureResultView:TowerAdventureResultView()
    -- 小游戏胜利失败界面,共用
    self.Data = TowerAdventureData.Instance
    -- 关闭时不需要打开关卡界面
    self.not_open_level = false
    self.is_special = false
    self.now_level = 0
    self.is_win = false
    self.need_auto_task = false
end

function TowerAdventureResultView:LoadCallback(param)
    self.again_fun = param.again_fun
    self.ObjWinShow:SetActive(param.is_win)
    -- 当前打的是否最新关卡
    self.is_now_max_level = false
    self.is_win = param.is_win
    if param.is_win then
        self.is_now_max_level = param.is_now_max_level
        self.now_level = param.now_level
        self.need_auto_task = self.is_now_max_level and self.now_level == self.Data:OtherSeq(3)
        -- 判断是否首次第x关创角关，是为特殊
        self.is_special = LoginData.Instance:LoginState() ~= LoginStateType.Logined and self.now_level == self.Data:OtherSeq(1)
        if self.is_special then
            self:OnSpShow()
        else
            self:DialogCheck(param.now_level)
        end
        self.ObjFirstShow:SetActive(self.is_now_max_level)
    end
    AudioMgr:PlayEffect(AudioType.UI, param.is_win and CommonAudio.QiangHuaChengGong or CommonAudio.QiangHuaShiBai)
    self.ObjNormal:SetActive(not self.is_special)
    self:DealItemList()
end

function TowerAdventureResultView:DealItemList()
    local now_use_level_data = self.Data:NowUseLevelData()
    local item_list = TableCopy(now_use_level_data.item_list)
    local rand_avartar_info, rand_jod_info = MiniGameData.Instance:GetRandInfo()
    local prof_id = RoleData.ProfIdToProfType(rand_jod_info.job_id or 0)
    if LoginData.Instance:LoginState() == LoginStateType.Logined then
        prof_id = RoleData.Instance:ProfType()
    end
    -- 这里需要处理职业道具的问题
    for k, v in pairs(item_list) do
        v.item_id = BagData.GetNewItemId(v.item_id, prof_id > 0 and prof_id or nil)
    end
    self.RewardList:SetData(DataHelper.FormatItemList(item_list))

end

function TowerAdventureResultView:OnCloseClick()
    if not self.not_open_level and not self.need_auto_task then
        ViewMgr:OpenView(TowerAdventureLevelView)
    end
    ViewMgr:CloseView(TowerAdventureGameView)
    ViewMgr:CloseView(TowerAdventureResultView)
end

function TowerAdventureResultView:OnAgainClick()
    if self.again_fun then
        self.again_fun()
    end
    ViewMgr:CloseView(TowerAdventureResultView)
end

function TowerAdventureResultView:OnSpShow()
    local item_id = self.Data:Other().show_item
    local close_func = function()
        self:DialogCheck(self.now_level)
    end
    TowerAdventureCtrl.Instance:OpenSpecGift(item_id, close_func)
end

function TowerAdventureResultView:CloseCallback()
    if self.need_auto_task then
        -- 执行自动任务前的特殊对话
        local other = self.Data:Other()
        if not StringIsEmpty(tostring(other.npc_id_2)) and not StringIsEmpty(tostring(other.txt_2)) then
            local end_func = function()
                -- 关掉界面的时候要开始自动任务
                TaskCtrl.Instance:AutoTaskImmediately()
            end
            MiniGameCtrl.Instance:OpenDialogView(other.npc_id_2, other.txt_2, end_func)
        else
            -- 关掉界面的时候要开始自动任务
            TaskCtrl.Instance:AutoTaskImmediately()
        end
    end
end

-- 检测是否需要对话，并且是否需要指引
function TowerAdventureResultView:DialogCheck(now_level)
    -- 打的不是最新关卡，就返回
    if not self.is_now_max_level then
        return
    end
    local dialog_config = MiniGameData.Instance:DialogConfig()
    local other = self.Data:Other()
    local talk_type = LoginData.Instance:LoginState() == LoginStateType.Logined and MiniGameTalkType.Normal or MiniGameTalkType.First
    if dialog_config[now_level] and dialog_config[now_level][talk_type] then
        local now_dialog = dialog_config[now_level][talk_type]
        if not StringIsEmpty(tostring(now_dialog.npc_id_2)) and not StringIsEmpty(tostring(now_dialog.txt_2)) then
            local end_func = function()
                if LoginData.Instance:LoginState() ~= LoginStateType.Logined then
                    -- 如果是打完第x关创角关，并且没登录，需要进入随机创角界面
                    if now_level == self.Data:OtherSeq(1)  then
                        ViewMgr:OpenView(TowerAdventureProfSelectView)
                        self.not_open_level = true
                        self:OnCloseClick()
                    end
                end
            end
            ViewMgr:CloseView(ItemInfoView)
            MiniGameCtrl.Instance:OpenDialogView(now_dialog.npc_id_2, now_dialog.txt_2, end_func)
        end
    end
end