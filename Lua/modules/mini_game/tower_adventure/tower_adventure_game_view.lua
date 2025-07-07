TowerAdventureGameView = TowerAdventureGameView or DeclareView("TowerAdventureGameView", "mini_game/tower_adventure_game")
VIEW_DECLARE_LEVEL(TowerAdventureGameView, ViewLevel.Lt)
VIEW_DECLARE_MASK(TowerAdventureGameView, ViewMask.BgBlock)
function TowerAdventureGameView:TowerAdventureGameView()
    -- 小游戏塔楼冒险游戏界面
end

----------------------------TowerAdventureGamePanel----------------------------
TowerAdventureGamePanel = TowerAdventureGamePanel or DeclareMono("TowerAdventureGamePanel", UIWFlushPanel)
function TowerAdventureGamePanel:TowerAdventureGamePanel()
    self.Data = TowerAdventureData.Instance
    self.language = Language.TowerAdventure
    
    self.data_cares = {
        -- {data = self.data, func = self.OpenGetView, init_call = false},
    }
end

function TowerAdventureGamePanel:OnCloseClick()
    if self.is_need_guide then
        return
    end
    local param = {}
    param.is_win = false
    param.again_fun = function()
        self:ResetGame(true)
    end
    ViewMgr:OpenView(TowerAdventureResultView, param)
end

function TowerAdventureGamePanel:Awake()
    UIWFlushPanel.Awake(self)
    if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end

    if self.role_obj == nil then
        self.role_obj = UIChDrawer.New()
        self.ModPerson:SetShowData({
            ui_obj = self.role_obj,
            view = self,
            seq = 1,
            show_shadow = false,
        })
    end
    -- 判断是否已经登录
    if LoginData.Instance:LoginState() == LoginStateType.Logined then
        local role_appearance = DataHelper.TableCopy(RoleData.Instance:GetApearance())
        role_appearance.battle_state = true
        self.role_obj:SetData(role_appearance)
    else
        -- 首次登陆随机一个角色和两个职业
        MiniGameData.Instance:CreateRandJobInfo()
        local rand_avartar_info, rand_jod_info = MiniGameData.Instance:GetRandInfo()
        local appearance = {
            weapon_id = rand_jod_info.job_id and RoleProfessionSimpleWeaponId[RoleData.ProfIdToProfType(rand_jod_info.job_id)] or 0,
            color = 0,
            prof_base = 0,
            avatar_type = rand_avartar_info.avartar_type,
            battle_state = true,
        }
        self.role_obj:SetData(appearance)
    end
    self:GuideCheck()
    self:ResetGame()
end

-- 检测是否需要指引
function TowerAdventureGamePanel:GuideCheck()
    -- 判断本次关卡是否需要进入指引
    self.is_need_guide = false
    -- 只有这一关是当前最新关卡才能进入指引
    local now_max_level = self.Data.level_data.now_level + 1
    local now_fight_seq = self.Data:NowFightSeq()
    if LoginData.Instance:LoginState() ~= LoginStateType.Logined and now_max_level == now_fight_seq and TowerAdventureTowerGameGuide[now_fight_seq] then
        self.is_need_guide = true
    end
    -- 当前的指引步骤
    self.guide_step = 0
    -- 指引需要去的层
    self.guide_need_go_layer = 1
    self:GuideNext()
end

-- 指引到下一步
function TowerAdventureGamePanel:GuideNext()
    self.guide_step = self.guide_step + 1
    local now_fight_seq = self.Data:NowFightSeq()
    if self.is_need_guide and not TableIsEmpty(TowerAdventureTowerGameGuide[now_fight_seq][self.guide_step]) then
        local guide_info = TowerAdventureTowerGameGuide[now_fight_seq][self.guide_step]
        self.ObjGuide:SetActive(true)
        self.ObjGuideEffect:SetActive(guide_info.Effect)
        UH.SetText(self.TxtGuide, self.language.Guide[guide_info.Txt])
        self.guide_need_go_layer = guide_info.Target
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
    else
        self.ObjGuide:SetActive(false)
    end
end

function TowerAdventureGamePanel:ResetGame(is_reset)
    self.ObjEffectWin:SetActive(false)
    self.ObjEffectFaild:SetActive(false)
    self.role_obj:PlayAnim(ChAnimType.Idle)
    self:SetIsCanDrag(true)
    self.Data:SetIsDraging(false)
    self.Data:SetEnterLayerMono()
    self.Data:SetInTurnTower(false, 1)
    self.now_level_detail = self.Data:GetLevelDetail(is_reset)
    -- 用于记录读取关卡表中的哪个数据
    self.now_first_tower_index = 1
    self:SetTowerData()
    self.Data:SetPersonLayerMono(self.Tower[self.Data:GetInTurnTower()].BottomLayerItem)
    self.Data:SetEnterLayerPerson(self.Tower[self.Data:GetInTurnTower()].BottomLayerItem.RectPerson[1])
    self:UpdatePerson()
end

function TowerAdventureGamePanel:SetTowerData()
    for i = 0, self.Tower:Length() - 1 do
        UH.LocalPosG(self.Tower[self.Data:GetNextInTurnTower(i)].gameObject, Vector3.New(TowerAdventureTowerX[i + 1], 0, 0))
        self.Tower[self.Data:GetNextInTurnTower(i)]:SetData(self.now_level_detail[self.now_first_tower_index + i] or {})
    end
end

function TowerAdventureGamePanel:UpdatePerson()
    UH.SetText(self.TxtPersonHp, self.Data:GetPersonHp(true))
    self.ObjPerson.transform:SetParent(self.Data:GetEnterLayerPerson())
    UH.LocalPosG(self.ObjPerson, Vector3.zero)
end

function TowerAdventureGamePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self, "role_obj")
    self.Data:SetPersonLayerMono()
    self.Data:SetEnterLayerMono()
    TimeHelper:CancelTimer(self.ht_fight)
    TimeHelper:CancelTimer(self.ht_pick_up)
    TimeHelper:CancelTimer(self.ht_win)
    TimeHelper:CancelTimer(self.ht_far)
    TimeHelper:CancelTimer(self.ht_far2)
    TimeHelper:CancelTimer(self.ht_faild)
    TimeHelper:CancelTimer(self.ht_reset_tower)
    TimeHelper:CancelTimer(self.ht_enter_game)
    Runner.Instance:RemoveRunUntilTrue(self.ht_move_tower)
end

function TowerAdventureGamePanel:OnDragBegin()
	if not self.Data:GetIsCanDrag() then
		return
    end
    self.Data:SetIsDraging(true)
    -- 拖拽前先把自己层加入当前进入层
    self.Data:SetEnterLayerMono(self.Data:GetPersonLayerMono())
	self.BlockPerson.enabled = false
end

function TowerAdventureGamePanel:OnDrag()
	if not self.Data:GetIsCanDrag() then
		return
	end
	if not self.Data:GetIsDraging() then
		return
	end

	local _, pos = UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(
		self.Data:GetEnterLayerPerson(),
		UnityEngine.Input.mousePosition,
        self.mainCanvasCom.worldCamera,
        nil)
	UH.AnchoredPosition(self.RectPerson, pos)
end

function TowerAdventureGamePanel:OnDragEnd()
	if not self.Data:GetIsCanDrag() then
		return
	end
	if not self.Data:GetIsDraging() then
		return
	end
    self.Data:SetIsDraging(false)
	self:JudgeIsCanGo()
end

-- 判断是否可以移动过去
function TowerAdventureGamePanel:JudgeIsCanGo()
    if self.Data:GetEnterLayerMono() ~= nil then
        local mono_data = self.Data:GetEnterLayerMono():GetData()
        -- 需要拖到敌方
        if mono_data.tower_type ~= TowerAdventureTowerWho.My then
            -- 如果需要清除楼层的话需要延迟一帧
            local need_reset = false
            local new_index = 1
            -- 判断是否可以移动
            -- 只能移动到前三层，不能跨两塔移动，不能跨两层移动
            local person_layer_data = self.Data:GetPersonLayerMono():GetData()
            local value1 = person_layer_data.tower_index == mono_data.tower_index - 1 and mono_data.index <= 3
            local value2 = person_layer_data.tower_index == mono_data.tower_index and math.abs(person_layer_data.index - mono_data.index) <= 1 and mono_data.index <= 3
            -- 同一座塔上的移动，并且不同层
            if value2 and math.abs(person_layer_data.index - mono_data.index) == 1 then
                -- 如果当前层已打完
                if TableIsEmpty(person_layer_data.monster_1) and TableIsEmpty(person_layer_data.monster_2) then
                    need_reset = true
                    -- 自己的塔增加一层
                    self.Data:AddLayer(self.now_level_detail[self.now_first_tower_index])
                    -- 记录初始index，重设数据后查找并设置新的mono
                    new_index = self.Data:RemoveLayer(self.now_level_detail[self.now_first_tower_index + 1], person_layer_data.index, mono_data.first_index)
                    -- 清除该层，重设自己的塔和敌人塔的数据
                    self.Tower[self.Data:GetInTurnTower()]:SetData(self.now_level_detail[self.now_first_tower_index])
                    self.Tower[self.Data:GetNextInTurnTower()]:SetData(self.now_level_detail[self.now_first_tower_index + 1])
                end
            end
            if value1 or value2 then
                -- 如果有指引
                if self.is_need_guide then
                    -- 如果指引的层和要去的层一致
                    if self.guide_need_go_layer == mono_data.index then
                        -- 拖动正确
                        self.ObjGuide:SetActive(false)
                    else
                        self:UpdatePerson()
                        self:SetIsCanDrag(true)
                        return
                    end
                end
                if need_reset then
                    TimeHelper:CancelTimer(self.ht_reset_tower)
                    -- 重设塔后需要延迟一帧
                    self.ht_reset_tower = TimeHelper:AddRunFrameTimer(function()
                        if new_index == 1 then
                            self.Data:SetEnterLayerMono(self.Tower[self.Data:GetNextInTurnTower()].BottomLayerItem)
                        else
                            self.Data:SetEnterLayerMono(self.Tower[self.Data:GetNextInTurnTower()].CenterList:GetItem(new_index - 1))
                        end
                        mono_data = self.Data:GetEnterLayerMono():GetData()
                        -- 如果有第二怪
                        if not TableIsEmpty(mono_data.monster_2) then
                            self:StartFight(2)
                        -- 如果有第一怪
                        elseif not TableIsEmpty(mono_data.monster_1) then
                            self:StartFight(1)
                        end
                    end, 1, 1, false)
                    return
                else
                    -- 如果有第二怪
                    if not TableIsEmpty(mono_data.monster_2) then
                        self:StartFight(2)
                        return
                    -- 如果有第一怪
                    elseif not TableIsEmpty(mono_data.monster_1) then
                        self:StartFight(1)
                        return
                    end
                end
            end
        end
    end
    self:UpdatePerson()
    self:SetIsCanDrag(true)
end

-- 开始战斗
function TowerAdventureGamePanel:StartFight(index)
    self.Data:SetPersonLayerMono(self.Data:GetEnterLayerMono())
    self.Data:SetEnterLayerPerson(self.Data:GetEnterLayerMono().RectPerson[index])
    -- 移动过去
    self:UpdatePerson()
    self:SetIsCanDrag(false)
    local monster_mono = self.Data:GetEnterLayerMono().Monster[index]
    local monster_data = monster_mono:GetData()
    -- 是队友就拾取，否则就打
    if monster_data.monster_type == TowerAdventureGameType.Teammate then
        self.ObjEffectWin:SetActive(true)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.LevelUp)
        TimeHelper:CancelTimer(self.ht_pick_up)
        self.ht_pick_up = Invoke(function()
            self.ObjEffectWin:SetActive(false)
            -- 更新当前血量，然后清除队友
            self.Data:AddPersonHp(monster_data.hp)
            self:UpdatePerson()
            for k, v in pairs(monster_data) do
                monster_data[k] = nil
            end
            monster_mono:SetData(monster_data)
            -- 需要判断该楼是否还有远程，有的话全部攻击一次主角
            local damage = self.Tower[self.Data:GetNextInTurnTower()]:CheckFar()
            if damage > 0 then
                -- 有远程伤害，需要敌人攻击完再扣血
                TimeHelper:CancelTimer(self.ht_far)
                self.ht_far = Invoke(function()
                    self.ObjEffectFaild:SetActive(true)
                    TimeHelper:CancelTimer(self.ht_far2)
                    self.ht_far2 = Invoke(function()
                        self.ObjEffectFaild:SetActive(false)
                        self.Data:AddPersonHp(-damage)
                        if self:CheckAlive() then
                            self:SetIsCanDrag(true)
                        end
                    end, 1)
                end, 1)
            else
                self:CheckEmpty()
            end
        end, 1)
    else
        self.role_obj:PlayAnim(ChAnimType.Attack)
        AudioMgr:PlayEffect(AudioType.Battle, CommonAudio.BaDaoZhan)
        monster_mono:Playanim(ChAnimType.Attack)
        TimeHelper:CancelTimer(self.ht_fight)
        self.ht_fight = Invoke(function()
            -- 判断谁赢谁输
            if self.Data:GetPersonHp() >= monster_data.hp then
                -- 赢了
                self.ObjEffectWin:SetActive(true)
                AudioMgr:PlayEffect(AudioType.UI, CommonAudio.LevelUp)
                monster_mono:Playanim(ChAnimType.Death)
                monster_mono:PlayEffect()
                -- 如果有指引
                if self.is_need_guide then
                    self:GuideNext()
                end
                TimeHelper:CancelTimer(self.ht_win)
                self.ht_win = Invoke(function()
                    self.ObjEffectWin:SetActive(false)
                    -- 更新当前血量，然后清除队友
                    self.Data:AddPersonHp(monster_data.hp)
                    self:UpdatePerson()
                    for k, v in pairs(monster_data) do
                        monster_data[k] = nil
                    end
                    monster_mono:SetData(monster_data)
                    -- 需要判断该楼是否还有远程，有的话全部攻击一次主角
                    local damage = self.Tower[self.Data:GetNextInTurnTower()]:CheckFar()
                    if damage > 0 then
                        -- 有远程伤害，需要敌人攻击完再扣血
                        TimeHelper:CancelTimer(self.ht_far)
                        self.ht_far = Invoke(function()
                            self.ObjEffectFaild:SetActive(true)
                            TimeHelper:CancelTimer(self.ht_far2)
                            self.ht_far2 = Invoke(function()
                                self.ObjEffectFaild:SetActive(false)
                                self.Data:AddPersonHp(-damage)
                                self:CheckAlive()
                            end, 1)
                        end, 1)
                    else
                        self:CheckEmpty()
                    end
                end, 1)
            else
                -- 输了
                self.ObjEffectFaild:SetActive(true)
                TimeHelper:CancelTimer(self.ht_faild)
                self.ht_faild = Invoke(function()
                    self.ObjEffectFaild:SetActive(false)
                    self.Data:SetPersonHp(0)
                    self:CheckAlive()
                end, 1)
            end
        end, 1)
    end
end

-- 检查主角是否活着
function TowerAdventureGamePanel:CheckAlive()
    local now_hp = self.Data:GetPersonHp()
    if now_hp <= 0 then
        now_hp = 0
    end
    self.Data:SetPersonHp(now_hp)
    self:UpdatePerson()
    if now_hp == 0 then
        AudioMgr:PlayEffect(AudioType.Battle, CommonAudio.Death)
        -- 这里需要等主角播完死亡才能继续
        self.role_obj.main_anim:WaitAnimEnd(ChAnimType.Death, function()
            local param = {}
            param.is_win = false
            param.again_fun = function()
                self:ResetGame(true)
            end
            ViewMgr:OpenView(TowerAdventureResultView, param)
            return false
        end)
        self.role_obj:PlayAnim(ChAnimType.Death)
    else
        self:SetIsCanDrag(true)
    end
end

-- 检查是否已经是空塔
function TowerAdventureGamePanel:CheckEmpty()
    if self.Tower[self.Data:GetNextInTurnTower()]:CheckEmpty() then
        -- 该塔为空塔，判断是否还有下一座
        if not TableIsEmpty(self.now_level_detail[self.now_first_tower_index + 2]) then
            -- 第一座塔需要变成自己的
            self.Tower[self.Data:GetNextInTurnTower()]:ChangeAppear(TowerAdventureTowerWho.My)
            -- 是空塔的话开始移动
            Runner.Instance:RemoveRunUntilTrue(self.ht_move_tower)
            self.ht_move_tower = RunUntilTrue(function()
                local local_pos = self.Tower[self.Data:GetNextInTurnTower()].gameObject.transform.localPosition
                if local_pos.x - self.Data:Speed() > TowerAdventureTowerX[1] then
                    for i = 1, self.Tower:Length() do
                        local l_pos = self.Tower[i].gameObject.transform.localPosition
                        UH.LocalPosG(self.Tower[i].gameObject, Vector3.New(l_pos.x - self.Data:Speed(), l_pos.y, l_pos.z))
                    end
                    return false
                else
                    for i = 1, self.Tower:Length() do
                        local l_pos = self.Tower[i].gameObject.transform.localPosition
                        UH.LocalPosG(self.Tower[self.Data:GetNextInTurnTower(i)].gameObject, Vector3.New(TowerAdventureTowerX[i], l_pos.y, l_pos.z))
                    end
                    -- 移动完成
                    self.Data:SetInTurnTower(true)
                    self.now_first_tower_index = self.now_first_tower_index + 1
                    -- 第三座塔需要重新创建
                    self.Tower[self.Data:GetNextInTurnTower(2)]:SetData(self.now_level_detail[self.now_first_tower_index + 2] or {})
                    self:SetIsCanDrag(true)
                    return true
                end
            end)
        else
            local now_level = self.Data.level_data.now_level + 1
            if LoginData.Instance:LoginState() ~= LoginStateType.Logined and now_level == self.Data:OtherSeq(2) then
                -- 首次第x关登录进入游戏
                self.ObjPersonHp:SetActive(false)
                self.ObjPerson.transform:SetParent(self.RectPersonShow)
                UH.LocalPosG(self.ObjPerson, Vector3.zero)
                self.ObjEnterGame:SetActive(true)
                TimeHelper:CancelTimer(self.ht_enter_game)
                self.ht_enter_game = Invoke(function()
                    -- 角色登录
                    LoginCtrl.Instance:RequestRoleLogin(LoginData.Instance:GetNewCreateRoles().id)
                    -- 重置是否需要玩小游戏标志
                    LoginData.Instance:SetIsPlayMiniGame(false)
                    ViewMgr:CloseView(TowerAdventureGameView)
                end, 2.5)
            elseif LoginData.Instance:LoginState() ~= LoginStateType.Logined and now_level == self.Data:OtherSeq(4) and now_level ~= self.Data:OtherSeq(1) then
                -- 首次第x关不显示奖励，如果创角关和不显示关卡冲突，那以创角关为主，还是会显示奖励
                ViewMgr:OpenView(TowerAdventureLevelView)
                ViewMgr:CloseView(TowerAdventureGameView)
            else
                local param = {}
                param.is_win = true
                param.now_level = now_level
                param.is_now_max_level = param.now_level == self.Data:NowFightSeq()
                ViewMgr:OpenView(TowerAdventureResultView, param)
            end
            self:Win()
        end
    else
        self:SetIsCanDrag(true)
    end
end

function TowerAdventureGamePanel:Win()
    -- 判断打的是不是最新的关卡
    if (self.Data.level_data.now_level + 1) == self.Data:NowFightSeq() then
        self.Data:SetLevelDataNowLevel(self.Data.level_data.now_level + 1)
        -- 判断是否已经登录
        if LoginData.Instance:LoginState() == LoginStateType.Logined then
            MiniGameCtrl.Instance:SendMiniGameFinishNextStage()
        end
    end
end

function TowerAdventureGamePanel:SetIsCanDrag(value)
    self.Data:SetIsCanDrag(value)
    self.ObjBlock:SetActive(not value)
    self.BlockPerson.enabled = value
end

----------------------------TowerAdventureGameTowerItem----------------------------
TowerAdventureGameTowerItem = TowerAdventureGameTowerItem or DeclareMono("TowerAdventureGameTowerItem", UIWidgetBaseItem)
function TowerAdventureGameTowerItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if not TableIsEmpty(data) then
        self.ObjSelf:SetActive(true)
        UH.SpriteName(self.ImgSetTop, TowerAdventureTowerType[data.tower_type] .. TowerAdventureTowerPos.Top)
        self.BottomLayerItem:SetData(data.layer_list[1])
        local t = {}
        for i = 2, #data.layer_list do
            table.insert(t, data.layer_list[i])
        end
        self.CenterList:SetData(t)
    else
        self.ObjSelf:SetActive(false)
    end
end

-- 改变外观
function TowerAdventureGameTowerItem:ChangeAppear(m_type)
    self.data.tower_type = m_type
    self.BottomLayerItem:ChangeAppear(m_type)
    for k, v in pairs(self.CenterList.item_list) do
        v:ChangeAppear(m_type)
    end
end

-- 刷新外观
function TowerAdventureGameTowerItem:FlushAppear()
    if not TableIsEmpty(self.data) then
        UH.SpriteName(self.ImgSetTop, TowerAdventureTowerType[self.data.tower_type] .. TowerAdventureTowerPos.Top)
    end
end

-- 检查该塔所有远程，并攻击一次，并返回所有远程造成的伤害
function TowerAdventureGameTowerItem:CheckFar()
    local damage = 0
    for k, v in pairs(self.data.layer_list) do
        if not TableIsEmpty(v.monster_2) and v.monster_2.monster_type == TowerAdventureGameType.FarMonster then
            if k == 1 then
                self.BottomLayerItem.Monster[2]:Playanim(ChAnimType.Attack)
            else
                self.CenterList:GetItem(k - 1).Monster[2]:Playanim(ChAnimType.Attack)
            end
            damage = damage + v.monster_2.hp
        end
        if not TableIsEmpty(v.monster_1) and v.monster_1.monster_type == TowerAdventureGameType.FarMonster then
            if k == 1 then
                self.BottomLayerItem.Monster[1]:Playanim(ChAnimType.Attack)
            else
                self.CenterList:GetItem(k - 1).Monster[1]:Playanim(ChAnimType.Attack)
            end
            damage = damage + v.monster_1.hp
        end
    end
    return damage
end

-- 检查塔是否为空
function TowerAdventureGameTowerItem:CheckEmpty()
    return TableIsEmpty(self.CenterList:GetData()) and TableIsEmpty(self.data.layer_list[1].monster_1) and TableIsEmpty(self.data.layer_list[1].monster_2)
end

function TowerAdventureGameTowerItem:OnDestroy()
    
end

----------------------------TowerAdventureGameLayerItem----------------------------
TowerAdventureGameLayerItem = TowerAdventureGameLayerItem or DeclareMono("TowerAdventureGameLayerItem", UIWidgetBaseItem)
function TowerAdventureGameLayerItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if not TableIsEmpty(data) then
        self.ImgSetSelf:SetObjActive(true)
        if data.index == 1 then
            UH.SpriteName(self.ImgSetSelf, TowerAdventureTowerType[data.tower_type] .. TowerAdventureTowerPos.Bottom)
        else
            UH.SpriteName(self.ImgSetSelf, TowerAdventureTowerType[data.tower_type] .. TowerAdventureTowerPos.Center)
        end
    else
        self.ImgSetSelf:SetObjActive(false)
    end
    self.ObjBlock:SetActive(data.index > 3 and data.tower_type == TowerAdventureTowerWho.Monster)
    local need = data.index - 3
    if Language.TowerAdventure.ClearNumLevels[need] then
        UH.SetText(self.TxtNeedClear, Language.TowerAdventure.ClearNumLevels[need])
    else
        UH.SetText(self.TxtNeedClear, Format(Language.TowerAdventure.ClearNumLevel, DataHelper.GetDaXie(need)))
    end
    self.Monster[1]:SetData(data.monster_1)
    self.Monster[2]:SetData(data.monster_2)
end

function TowerAdventureGameLayerItem:ChangeAppear(m_type)
    self.data.tower_type = m_type
    self:FlushAppear()
end

-- 刷新外观
function TowerAdventureGameLayerItem:FlushAppear()
    if not TableIsEmpty(self.data) then
        if self.data.index == 1 then
            UH.SpriteName(self.ImgSetSelf, TowerAdventureTowerType[self.data.tower_type] .. TowerAdventureTowerPos.Bottom)
        else
            UH.SpriteName(self.ImgSetSelf, TowerAdventureTowerType[self.data.tower_type] .. TowerAdventureTowerPos.Center)
        end
    end
end

function TowerAdventureGameLayerItem:OnPointerEnter()
    if not TowerAdventureData.Instance:GetIsDraging() then
        return
    end
	TowerAdventureData.Instance:SetEnterLayerMono(self)
end

function TowerAdventureGameLayerItem:OnPointerExit()
    if not TowerAdventureData.Instance:GetIsDraging() then
        return
    end
	TowerAdventureData.Instance:SetEnterLayerMono(nil)
end

function TowerAdventureGameLayerItem:OnDestroy()
    
end

----------------------------TowerAdventureGameMonsterItem----------------------------
TowerAdventureGameMonsterItem = TowerAdventureGameMonsterItem or DeclareMono("TowerAdventureGameMonsterItem", UIWidgetBaseItem)
function TowerAdventureGameMonsterItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.mod_obj == nil then
        self.mod_obj = UIObjDrawer.New()
        self.Model:SetShowData({
            ui_obj = self.mod_obj,
            view = self,
            seq = 1,
            show_shadow = false,
        })
    end
    if not TableIsEmpty(data) then
        self:SetObjActive(true)
        self.mod_obj:SetData(DrawerHelper.GetNpcPath(data.res_id))
        self:UpdateHp()
        self:Playanim(ChAnimType.Idle)
    else
        self:SetObjActive(false)
    end
    self:StopEffect()
    self.TxtMonsterHp.color = data.monster_type == TowerAdventureGameType.Teammate and COLORS.Blue6 or COLORS.Red7
end

function TowerAdventureGameMonsterItem:Playanim(anim)
    if not TableIsEmpty(self.data) then
        self.mod_obj:PlayAnim(anim)
    end
end

function TowerAdventureGameMonsterItem:PlayEffect()
    self.ObjEffect:SetActive(true)
    TimeHelper:CancelTimer(self.ht_die)
    self.ht_die = Invoke(function()
        self.ObjEffect:SetActive(false)
    end, 1)
end

function TowerAdventureGameMonsterItem:StopEffect()
    TimeHelper:CancelTimer(self.ht_die)
    self.ObjEffect:SetActive(false)
end

function TowerAdventureGameMonsterItem:UpdateHp(hp)
    if hp then
        self.data.hp = hp
    end
    UH.SetText(self.TxtMonsterHp, TowerAdventureData.Instance:DealHpNum(self.data.hp))
end

function TowerAdventureGameMonsterItem:SetObjActive(value)
    self.ObjSelf:SetActive(value)
end

function TowerAdventureGameMonsterItem:OnDestroy()
    Delete(self, "mod_obj")
    TimeHelper:CancelTimer(self.ht_die)
end