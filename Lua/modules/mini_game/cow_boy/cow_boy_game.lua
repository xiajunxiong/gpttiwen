CowBoyGameView = CowBoyGameView or DeclareView("CowBoyGameView", "mini_game/cow_boy_game")
VIEW_DECLARE_LEVEL(CowBoyGameView, ViewLevel.Lt)
VIEW_DECLARE_MASK(CowBoyGameView, ViewMask.BgBlock)
function CowBoyGameView:CowBoyGameView()
    -- 小游戏牛郎织女游戏界面
end

function CowBoyGameView:LoadCallback(param)
    if param and param.close_level_view then
        ViewMgr:CloseView(CowBoyLevelView)
    end
    CowBoyCtrl.Instance.mini_game_battle = false
end

function CowBoyGameView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(CowBoyData.Instance:Other().game_desc or "")
end

----------------------------CowBoyGamePanel----------------------------
CowBoyGamePanel = CowBoyGamePanel or DeclareMono("CowBoyGamePanel", UIWFlushPanel)
function CowBoyGamePanel:CowBoyGamePanel()
    self.Data = CowBoyData.Instance
    self.language = Language.CowBoy
    
    self.data_cares = {
        {data = self.Data.game_smdata, func = self.CalculateShowGirlEff, keys = {"reset_girl_eff"}, init_call = false}
        -- {data = self.data, func = self.OpenGetView, init_call = false},
    }
end

function CowBoyGamePanel:FlushLeft()
    for i = 1,4 do
        self.LeftList[i]:SetActive(self.Data:NowHasType(i))
        self.LeftListEmpty[i]:SetActive(not self.Data:NowHasType(i))
    end
    -- self.LeftList[CowBoyCfg.CellType.Weapon]:SetActive(self.Data:NowHasType(CowBoyCfg.CellType.Weapon))
    -- self.LeftList[CowBoyCfg.CellType.Pet]:SetActive(self.Data:NowHasType(CowBoyCfg.CellType.Pet))
    -- self.LeftList[CowBoyCfg.CellType.Partner]:SetActive(self.Data:NowHasType(CowBoyCfg.CellType.Partner))
    -- self.LeftListEmpty[CowBoyCfg.CellType.Weapon]:SetActive(not self.Data:NowHasType(CowBoyCfg.CellType.Weapon))
    -- self.LeftListEmpty[CowBoyCfg.CellType.Pet]:SetActive(not self.Data:NowHasType(CowBoyCfg.CellType.Pet))
    -- self.LeftListEmpty[CowBoyCfg.CellType.Partner]:SetActive(not self.Data:NowHasType(CowBoyCfg.CellType.Partner))
    if self.Data:NowHasType(CowBoyCfg.CellType.Skill) then
        self.LeftList[CowBoyCfg.CellType.Skill]:SetActive(true)
        local detail = self.Data:GetLevelDetailByEvent(CowBoyCfg.CellType.Skill)
        local skill_cfg = Cfg.SkillDescPartnerAndRole(detail.cell_info.match_id, 1)
        UH.SetIcon(self.SkillIcon, skill_cfg.icon_id, ICON_TYPE.SKILL)
        -- self.LeftListEmpty[CowBoyCfg.CellType.Skill]:SetActive(false)
    -- else
    --     self.LeftList[CowBoyCfg.CellType.Skill]:SetActive(false)
    --     self.LeftListEmpty[CowBoyCfg.CellType.Skill]:SetActive(true)
    end
end

function CowBoyGamePanel:OnCloseClick()
    if self.is_need_guide then
        return
    end
    ViewMgr:OpenView(CowBoyReturnView, {reset_func = function ()
        self:ResetGame(true)
    end})
    -- ViewMgr:OpenView(CowBoyLevelView)
    -- ViewMgr:CloseView(CowBoyGameView)
end

function CowBoyGamePanel:Awake()
    UIWFlushPanel.Awake(self)
    if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end
    self.ScrollRect.onValueChanged:AddListener(function(v2)
        self:OnScrollRectValueChanged(v2)
    end)
    TimeHelper:AddDelayFrameTimer(function ()
        self:GuideCheck()
    end,3)
    -- self:GuideCheck()
    
    self:ResetGame(true)
end

function CowBoyGamePanel:OnScrollRectValueChanged(v2)
    self:CalculateShowGirlEff()
end

function CowBoyGamePanel:CalculateShowGirlEff()
    if self.RectContent.anchoredPosition.x > self.Data.show_girl_x then
        self.RightGirl:SetActive(true)
    else
        self.RightGirl:SetActive(false)
    end
end


-- 检测是否需要指引
function CowBoyGamePanel:GuideCheck()
    -- 判断本次关卡是否需要进入指引
    self.is_need_guide = false
    -- 只有这一关是当前最新关卡才能进入指引
    local now_max_level = self.Data:GetLevelDataNowLevel() + 1
    local now_fight_seq = self.Data:NowFightSeq()
    if LoginData.Instance:LoginState() ~= LoginStateType.Logined and now_max_level == now_fight_seq and CowBoyGameGuide[now_fight_seq] then
        self.is_need_guide = true
        self.ScrollRect.enabled = false
    end
    -- 当前的指引步骤
    self.guide_step = 0
    -- 指引需要去的岛屿
    self.guide_need_go_island = 1
    self:GuideNext()
end

-- 指引到下一步
function CowBoyGamePanel:GuideNext()
    self.guide_step = self.guide_step + 1
    local now_fight_seq = self.Data:NowFightSeq()
    if self.is_need_guide and not TableIsEmpty(CowBoyGameGuide[now_fight_seq][self.guide_step]) then
        local guide_info = CowBoyGameGuide[now_fight_seq][self.guide_step]
        self.ObjGuide:SetActive(true)
        UH.SetText(self.TxtGuide, self.language.Guide[guide_info.Txt])
        self.guide_need_go_island = guide_info.Target
        self.ObjGuide1:SetActive(guide_info.show[1])
        self.ObjGuide2:SetActive(guide_info.show[2])
        UH.LocalPosG(self.ObjGuideTip, Vector3.New(guide_info.TxtPos.x, guide_info.TxtPos.y, 0))
        if not TableIsEmpty(guide_info.Obj1) then
            UH.LocalPosG(self.ObjGuide1, Vector3.New(guide_info.Obj1.x, guide_info.Obj1.y, 0))
            UH.SizeDelta(self.ObjGuide1, Vector2.New(guide_info.Obj1.w, guide_info.Obj1.h))
        end
        if not TableIsEmpty(guide_info.Obj2) then
            UH.LocalPosG(self.ObjGuide2, Vector3.New(guide_info.Obj2.x, guide_info.Obj2.y, 0))
            UH.SizeDelta(self.ObjGuide2, Vector2.New(guide_info.Obj2.w, guide_info.Obj2.h))
        end
        self.is_need_guide = true
    else
        self.ObjGuide:SetActive(false)
        self.is_need_guide = false
        self.ScrollRect.enabled = true
    end
end

function CowBoyGamePanel:ResetGame(is_reset)
    self.ScrollRect:StopMovement()
    local map_width = self.Data:GetMapWidth()
    local size = UH.SizeDelta2(self.RectContent)
    UH.SizeDelta2(self.RectContent, Vector2.New(map_width,size.y))
    if self.Data.pre_battle_anchoredPos ~= nil then
        UH.AnchoredPosition(self.RectContent, Vector2.New(self.Data.pre_battle_anchoredPos.x, 0))
        self.Data.pre_battle_anchoredPos = nil
    else
        self.ScrollRect.horizontalNormalizedPosition = 0
        -- UH.AnchoredPosition(self.RectContent, Vector2.New(367, 0))
    end
    
    self:SetIsCanDrag(true)
    self.Data:SetIsDraging(false)
    self.Data:SetEnterIslandMono()
    local need_block_dialog = false
    
    if is_reset and MiniGameData.Instance:BattleEndType() == MiniGameBattleEndType.None then
        self.now_level_detail = self.Data:GetLevelDetail(true)
    else
        self.now_level_detail = self.Data:GetLevelDetail()
        local end_type = MiniGameData.Instance:BattleEndType()
        if end_type == MiniGameBattleEndType.Win then
            --进入战斗场景的胜利
            local mono_data = self.Data:GetBattleData()
            if mono_data ~= nil then
                need_block_dialog = true
                ViewMgr:OpenView(CowBoyWinView, {add_hp = mono_data.hp,
                close_fun = function()
                    for k, v in pairs(self.IslandList.item_list) do
                        local item_data = v:GetData()
                        if item_data.cell_type == CowBoyCfg.CellType.Boss then
                            self.Data:SetEnterIslandMono(v)
                            break
                        end
                    end
                    self:GoNextIslandShow()
                    self:SetIsCanDrag(true)
                    self:CheckGoNextGuide()
                    self:AddHp(mono_data.hp)
                    self:UpdatePerson(true)
                end})
            else
                ViewMgr:OpenView(CowBoyWinView)
            end
            
        elseif end_type == MiniGameBattleEndType.Faild then
            ViewMgr:OpenView(CowBoyResultView, {is_win = false, again_fun = function()
                self:ResetGame(true)
            end})
        end
        MiniGameData.Instance:BattleEndType(MiniGameBattleEndType.None)
    end
    self.IslandList:SetData(self.now_level_detail)
    self.TxtPersonHpAdd:SetObjActive(false)

    TimeHelper:CancelTimer(self.ht_start)
    self.ht_start = TimeHelper:AddRunFrameTimer(function()
        self:UpdatePerson(need_block_dialog)
    end)
    self:FlushLeft()
    self.is_fight = false
end

function CowBoyGamePanel:UpdatePerson(block_grid_dialog)
    self.ObjPerson:SetActive(true)
    UH.SetText(self.TxtPersonHp, self.Data:GetPersonHp(true))
    self.ObjPerson.transform.position = self.IslandList:GetItem(self.Data:PersonPos()):GetPersonPos()
    if not self.Data:GetCanReachFinal() then
        ViewMgr:OpenView(CowBoyResultView, {is_win = false, can_not_final = true, again_fun = function()
            self:ResetGame(true)
        end})
    elseif not block_grid_dialog then
        local grid_dialog_cfg = self.Data:GetGridDialogCfg()
        if not TableIsEmpty(grid_dialog_cfg) then
            MiniGameCtrl.Instance:OpenDialogView(nil, nil, nil, grid_dialog_cfg, true, true)
        end
    end
    -- LogError("GetCanReachFinal",self.Data:GetCanReachFinal())
end

function CowBoyGamePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.Data:SetEnterIslandMono()
    TimeHelper:CancelTimer(self.ht_start)
    TimeHelper:CancelTimer(self.ht_fight)
    TimeHelper:CancelTimer(self.ht_hp_add)
    MiniGameData.Instance:BattleEndType(MiniGameBattleEndType.None)
    if not self.is_fight then
        self.Data:ResetNowUseLevelData()
    end
end

function CowBoyGamePanel:OnDragBegin()
	if not self.Data:GetIsCanDrag() then
		return
    end
    self.Data:SetIsDraging(true)
    self.BlockPerson.enabled = false
    self.ScrollRect.enabled = false
    self.Data:SetEnterIslandMono()
end

function CowBoyGamePanel:OnDrag()
	if not self.Data:GetIsCanDrag() then
		return
	end
	if not self.Data:GetIsDraging() then
		return
	end

	local _, pos = UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(
		self.RectContent,
		UnityEngine.Input.mousePosition,
        self.mainCanvasCom.worldCamera,
        nil)
    UH.AnchoredPosition(self.RectPerson, pos)
end

function CowBoyGamePanel:OnDragEnd()
	if not self.Data:GetIsCanDrag() then
		return
	end
	if not self.Data:GetIsDraging() then
		return
    end
    self.Data:SetIsDraging(false)
    self:JudgeIsCanGo()
    if not self.is_need_guide then
        self.ScrollRect.enabled = true
    end
end

-- 判断是否可以移动过去
function CowBoyGamePanel:JudgeIsCanGo()
    local mono = self.Data:GetEnterIslandMono()
    if mono ~= nil then
        local mono_data = mono:GetData()
        local is_can = false
        -- 判断是否可走
        if (not mono_data.is_break) and (not mono_data.is_mask) then
            for k, v in pairs(mono_data.can_pos) do
                if v == self.Data:PersonPos() then
                    is_can = true
                    break
                end
            end
        end
        -- 如果有指引
        if self.is_need_guide then
            -- 如果指引的岛屿和要去的层一致
            is_can = self.guide_need_go_island == mono_data.id
            self.ObjGuide:SetActive(not is_can)
            if not is_can then
                PublicPopupCtrl.Instance:Center(self.language.Tip[1])
            end
        end
        if is_can then
            self.ObjPerson.transform.position = mono:GetPersonPos()
            if mono_data.cell_type ~= CowBoyCfg.CellType.Empty or mono_data.id == self.Data:EndPos() then
                self:DealNewIsland()
                return
            end
            self:GoNextIslandShow()
            -- 设置主角当前新的岛
            self:CheckGoNextGuide()
        else
            PublicPopupCtrl.Instance:Center(self.language.Tip[2])
        end
    end
    self:UpdatePerson()
    self:SetIsCanDrag(true)
end

-- 去下一个岛
function CowBoyGamePanel:GoNextIslandShow()
    local mono = self.Data:GetEnterIslandMono()
    if mono ~= nil then
        local mono_data = mono:GetData()
        local last_id = self.Data:PersonPos()
        -- 可以走的话要把前一岛干掉
        self.now_level_detail[last_id].is_break = true
        self.IslandList:GetItem(last_id):FlushShow()
        -- 把周围岛遮罩取消
        -- for k,v in pairs(self.now_level_detail) do
        --     LogError("k",k)
        -- end
        for k, v in pairs(mono_data.can_pos) do
            -- LogError("v",v)
            self.now_level_detail[v].is_mask = false
            self.IslandList:GetItem(v):FlushShow()
        end
        self.Data:PersonPos(mono_data.id)
        self.IslandList:GetItem(mono_data.id):FlushShow()
    end
end

-- 进入新岛，处理表现
function CowBoyGamePanel:DealNewIsland()
    local mono = self.Data:GetEnterIslandMono()
    local mono_data = mono:GetData()
    local cell_type = mono_data.cell_type
    
    if cell_type == CowBoyCfg.CellType.Enemy or cell_type == CowBoyCfg.CellType.Boss then
        -- 播放打动特效
        self.ObjPerson:SetActive(false)
        mono:FightEffect(true)
        TimeHelper:CancelTimer(self.ht_fight)
        self.ht_fight = Invoke(function()
            if cell_type == CowBoyCfg.CellType.Enemy then
                self.ObjPerson:SetActive(true)
                mono:FightEffect(false)
                if self.Data:GetPersonHp() >= mono_data.hp then
                    -- 小战斗赢了
                    self:GoNextIslandShow()
                    ViewMgr:OpenView(CowBoyWinView, {add_hp = mono_data.hp,
                    close_fun = function()
                        -- self:GoNextIslandShow()
                        self:SetIsCanDrag(true)
                        self:CheckGoNextGuide()
                        self:AddHp(mono_data.hp)
                        self:UpdatePerson()
                    end})
                else
                    -- 小战斗输了
                    ViewMgr:OpenView(CowBoyResultView, {is_win = false, again_fun = function()
                        self:ResetGame(true)
                    end})
                end
            else
                self.is_fight = true
                self.Data:SetBattleData(mono_data)
                self.Data.pre_battle_anchoredPos = self.RectContent.anchoredPosition
                CowBoyCtrl.Instance:GoBattle()
                ViewMgr:CloseView(CowBoyGameView)
            end
        end, 1)
        return
    elseif cell_type == CowBoyCfg.CellType.Empty and mono_data.id == self.Data:EndPos() then
        self:GoNextIslandShow()
        -- 终点岛
        self:Win()
        
        local dialog_end_fun = function ()
            ViewMgr:OpenView(CowBoyResultView, {is_win = true, next_fun = function()
                local next_lv = self.Data:NowFightSeq() + 1
                if next_lv <= self.Data:LevelMax() then
                    local cfg = Cfg.CowBoyLevelConfig(next_lv)
                    MiniGameData.Instance:SetLevelDetail(cfg)
                    self:ResetGame()
                end
            end})
        end
        local grid_dialog_cfg = self.Data:GetGridDialogCfg()
        if not TableIsEmpty(grid_dialog_cfg) then
            MiniGameCtrl.Instance:OpenDialogView(nil, nil, dialog_end_fun, grid_dialog_cfg, true, true)
        else
            dialog_end_fun()
        end
        
    else
        -- 武器宠物伙伴技能
        local call_func = function ()
            ViewMgr:OpenView(CowBoyGetView, {data = self.now_level_detail[mono_data.id],
            close_fun = function()
                self:GoNextIslandShow()
                self:FlushLeft()
                if cell_type == CowBoyCfg.CellType.Weapon
                    or cell_type == CowBoyCfg.CellType.Pet
                    or cell_type == CowBoyCfg.CellType.Partner then
                    self:AddHp(mono_data.hp)
                end
                self:UpdatePerson()
                self:SetIsCanDrag(true)
            end})
        end

        if cell_type == CowBoyCfg.CellType.Skill then
            ViewMgr:OpenView(CowBoyBoxEffectView,{close_func = call_func})
        else
            call_func()
        end
    end
end

function CowBoyGamePanel:AddHp(value)
    self.Data:AddPersonHp(value)
    self:ShowAdd(value)
end

function CowBoyGamePanel:CheckGoNextGuide()
    -- 如果有指引
    if self.is_need_guide then
        self:GuideNext()
    end
end

-- 主角飘字
function CowBoyGamePanel:ShowAdd(value)
    self.TxtPersonHpAdd:SetObjActive(true)
    UH.SetText(self.TxtPersonHpAdd, "+"..value)
    self.AnimPersonHpAdd:Play()
    TimeHelper:CancelTimer(self.ht_hp_add)
    self.ht_hp_add = Invoke(function()
        self.TxtPersonHpAdd:SetObjActive(false)
    end, 0.5)
end

function CowBoyGamePanel:Win()
    -- 判断打的是不是最新的关卡
    if (self.Data:GetLevelDataNowLevel() + 1) == self.Data:NowFightSeq() then
        self.Data:SetLevelDataNowLevel(self.Data:GetLevelDataNowLevel() + 1)
        -- 判断是否已经登录
        if LoginData.Instance:LoginState() == LoginStateType.Logined then
            MiniGameCtrl.Instance:SendMiniGameFinishNextStage()
        end
    end
end

function CowBoyGamePanel:SetIsCanDrag(value)
    self.Data:SetIsCanDrag(value)
    self.ObjBlock:SetActive(not value)
    self.BlockPerson.enabled = value
end

----------------------------CowBoyGameIslandItem----------------------------
CowBoyGameIslandItem = CowBoyGameIslandItem or DeclareMono("CowBoyGameIslandItem", UIWidgetBaseItem)
function CowBoyGameIslandItem:CowBoyGameIslandItem()
    self.Data = CowBoyData.Instance
end
function CowBoyGameIslandItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.LocalPosG(self.Root, data.pos)
    if self.data.id == self.Data:EndPos() then
        local end_pos_anchoredPos = self.RootRect.anchoredPosition
        self.Data.show_girl_x = 1425 / 2 - (end_pos_anchoredPos.x - (self.Data:GetMapWidth() / 2)) + (UnityEngine.Screen.width > 1425 and (UnityEngine.Screen.width - 1425 ) / 4 or 0) - 200
        self.timer_delay_frame = TimeHelper:AddDelayFrameTimer(function ()
            self.Data.game_smdata:SetDirty("reset_girl_eff")
        end,2)
    end
    self.ObjBlock:SetActive(data.is_mask and self.data.id ~= self.Data:EndPos())
    -- self.ObjIsland1:SetActive(data.cell_type ~= CowBoyCfg.CellType.Boss)
    -- self.ObjIsland2:SetActive(data.cell_type == CowBoyCfg.CellType.Boss)
    self.ObjIsland1:SetActive(data.grid_res == 1)
    self.ObjIsland2:SetActive(data.grid_res == 2)
    self.ObjNode:SetActive(data.is_show == 1 and not data.is_break)
    -- if data.cell_type == CowBoyCfg.CellType.Empty then
    --     UH.SetText(self.TxtHp, data.hp)
    -- else
    --     UH.SetText(self.TxtHp, "+"..data.hp)
    -- end
    local is_enemy = data.cell_type == CowBoyCfg.CellType.Enemy or data.cell_type == CowBoyCfg.CellType.Boss
    self.ObjSword:SetActive(is_enemy)
    UH.SetText(self.TxtHp, data.hp)
    UH.SetText(self.ItemHpTxt, "+" .. data.hp)
    self.ObjHp:SetActive(data.hp > 0 and (not data.is_mask) and is_enemy)
    self.ItemHp:SetActive(data.hp > 0 and (not data.is_mask) and not is_enemy)
    self:ItemShow()
    self:FightEffect(false)
    self:FlushShow()
    if IS_EDITOR then
        self.EditorPos:SetActive(true)
        UH.SetText(self.EditorPosTxt, data.id)
    end
end

function CowBoyGameIslandItem:ItemShow()
    if self.data.cell_type == CowBoyCfg.CellType.Empty then
        self.ObjGirl:SetActive(self.data.id == self.Data:EndPos())
    else
        self.ObjGirl:SetActive(false)
    end
    for i = 1, self.ObjItems:Length() do
        self.ObjItems[i]:SetActive(i == self.data.cell_type)
    end
    if self.data.cell_type == CowBoyCfg.CellType.Enemy then
        self.ObjItems[CowBoyCfg.CellType.Enemy]:SetActive(self.data.event_param == CowBoyCfg.EnemyType.Boy)
        self.ObjEnemy2:SetActive(self.data.event_param == CowBoyCfg.EnemyType.Girl)
    else
        self.ObjEnemy2:SetActive(false)
    end
end

function CowBoyGameIslandItem:OnPointerEnter()
    if not CowBoyData.Instance:GetIsDraging() then
        return
    end
	CowBoyData.Instance:SetEnterIslandMono(self)
end

function CowBoyGameIslandItem:OnPointerExit()
    if not CowBoyData.Instance:GetIsDraging() then
        return
    end
	CowBoyData.Instance:SetEnterIslandMono(nil)
end

function CowBoyGameIslandItem:FlushShow()
    self.ObjBlock:SetActive(self.data.is_mask and self.data.id ~= self.Data:EndPos())
    self.ObjNode:SetActive(self.data.is_show == 1 and not self.data.is_break)
    self.ObjItem:SetActive(self.data.id ~= self.Data:PersonPos() and (not self.data.is_mask) or self.data.id == self.Data:EndPos())
    local is_enemy = self.data.cell_type == CowBoyCfg.CellType.Enemy or self.data.cell_type == CowBoyCfg.CellType.Boss
    self.ObjHp:SetActive(self.data.hp > 0 and (not self.data.is_mask) and is_enemy)
    self.ItemHp:SetActive(self.data.hp > 0 and (not self.data.is_mask) and not is_enemy)
end

function CowBoyGameIslandItem:GetPersonPos()
    return self.PersonPos.transform.position
end

function CowBoyGameIslandItem:FightEffect(value)
    self.ObjItem:SetActive(self.data.id ~= self.Data:PersonPos())
    self.ObjFightEffect:SetActive(value)
end

function CowBoyGameIslandItem:OnDestroy()
end
---------------------------------------------------
CowBoyReturnView = CowBoyReturnView or DeclareView("CowBoyReturnView","mini_game/cow_boy_return")
VIEW_DECLARE_MASK(CowBoyReturnView, ViewMask.Block)
function CowBoyReturnView:CowBoyReturnView()
    self.Data = CowBoyData.Instance
end

function CowBoyReturnView:LoadCallback(param)
    UH.SetText(self.NameText, Format(Language.CowBoy.NameTextFormat, self.Data.now_use_level_data.level_type))
    -- UH.SpriteName(self.NameImg,"_LocShenHongZi"..self.data.level_data.now_page)
    if param and param.reset_func then
        self.reset_func = param.reset_func
    else
        self.reset_func = nil
    end
end

function CowBoyReturnView:OnClickReturn()
    ViewMgr:CloseView(CowBoyReturnView)
    ViewMgr:OpenView(CowBoyLevelView,{close_game_view = true})
    -- ViewMgr:CloseView(CowBoyGameView, )
end

function CowBoyReturnView:OnClickRestart()
    ViewMgr:CloseView(CowBoyReturnView)
    if self.reset_func then
        self.reset_func()
    end
end

function CowBoyReturnView:OnClickDaMengBan()
    ViewMgr:CloseView(CowBoyReturnView)
end