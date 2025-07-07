RuinsView = RuinsView or DeclareView("RuinsView", "god_areas_ruins/ruins_view", Mod.GodAreasActs.RuinsScene)
local Input = UnityEngine.Input
VIEW_DECLARE_MASK(RuinsView, ViewMask.None)
VIEW_DECLARE_LEVEL(RuinsView, ViewLevel.Lu)
function RuinsView:RuinsView()
    
end
function RuinsView:LoadCallback()
    PublicPopupCtrl.Instance:OpenWarningView(nil, {desc = Config.language_cfg_auto.textdesc[373].desc, warning = Config.language_cfg_auto.textdesc[374].desc})
end

RuinsViewPanel = RuinsViewPanel or DeclareMono("RuinsViewPanel", UIWFlushPanel)
function RuinsViewPanel:RuinsViewPanel()
    --10层
    self.data = RuinsData.Instance
    self.player_data = RoleData.Instance
    self.player_base_data = self.player_data:GetBaseData()
    self.pet_data = PetData.Instance
    self.time_ctrl = TimeCtrl.Instance
    self.main_data = MainViewData.Instance
    self.data:InitSceneUiVos()

    self.cur_wifi = 2
    self.cur_time = 0
    
    self.cur_platform = 1
    self.is_auto_boss = false
    self.is_wait_flush_boss = false

    self.data_cares = {
        {data = self.player_base_data, func = self.FlushRoleHead, keys = {"appearance"}},
        {data = self.pet_data:GetOtherInfo(), func = self.FlushPetHead, keys = {"fight_index"}},
        {data = self.time_ctrl.delay_time, func = self.FlushWifi},
        {data = self.data:GetRoleInfo(), func = self.FlushRoleInfo},
        {data = self.data:GetRoleInfo(), func = self.FlushRoleListInfo},
        {data = self.data:GetSceneInfo(), func = self.FlushRoleListInfo, keys = {"flush_role"}},
        {data = self.data:GetSceneInfo(), func = self.FlushLayerItem, keys = {"flush_platform"}},
        {data = self.data:GetTimeInfo(), func = self.FlushLayerItemShow},
        {data = self.data:GetSceneInfo(), func = self.FlushSceneArrow, keys = {"flush_arrow"}},
        {data = self.data:GetBossInfo(), func = self.FlushBossInfo},
        {data = self.data:GetTimeInfo(), func = self.ShowBossSkill, keys = {"boss_skill_time"}},
        {data = self.data:GetTimeInfo(), func = self.FlushBossChallenge, keys = {"boss_can_challenge_time"}},
        {data = self.data:GetTimeInfo(), func = self.FlushAttackRoleTime, keys = {"attack_role_time"}},
        {data = self.data:GetTimeInfo(), func = self.FlushAttackBossTime, keys = {"attack_boss_time"}},
        {data = self.data:PopupData(), func = self.FlushBossHurt},
        {data = self.main_data:SceneObjVos(), func = self.FlushBoards},
        {data = self.data:GetActInfosm(), func = self.FlushActTime},
        {data = self.data.achi_data, func = self.FlushAchiRedPoint, keys = {"flush_flag"}},
    }
    self.pos_list = {
        {x = -9.50, z = 16.75},
        {x = 14.56, z = -33.77},
        {x = -21.55, z = -34.27},
        {x = -9.42, z = -76.57},
        {x = -0.14, z = -116},
        {x = -28.69, z = -109.5},
        {x = -16.57, z = -149.72},
        {x = 5.15, z = -181.92},
        {x = -34.87, z = -181.87},
        {x = -10.38, z = -231.25}
    }
    self.arrow_list = {1,2,3,4,5,6,7}
    self.move_speed = 2
end
function RuinsViewPanel:FlushAchiRedPoint()
    LogDG("归墟成就红点数量", self.data:AchiRedPoint())
    self.AchiRedPoint:SetNum(self.data:AchiRedPoint())
end
function RuinsViewPanel:FlushActTime()
    local act_info = self.data:GetActInfosm().info
    if act_info.state == ActStatusType.Standy then
        --SetTime
        self.ActTime:StampTime(act_info.open_time, nil, Language.GodAreasActs.ActTime2)--Language.GodAreasActs.ActReady)
    elseif act_info.state == ActStatusType.Open then
        self.ActTime:StampTime(act_info.end_time, nil, Language.GodAreasActs.ActTime)
    else
        self:ShowActOver()
        --[[ self.ActTime:SetCallBack(function ()
            ViewMgr:CloseView(RuinsView)
            SceneCtrl.Instance:RequestLeaveFb()
        end)
        self.ActTime:StampTime(act_info.end_time, nil, Language.GodAreasActs.RuinsClose) ]]
    end
end

function RuinsViewPanel:FlushBossHurt()
    local data_list = self.data:PopPopupData()
    for _, v in pairs(data_list) do
        self.HurtPool:Spwan(v)
    end
end
function RuinsViewPanel:onFlush()
    
end
function RuinsViewPanel:OnDestroy()
    PublicPopupCtrl.Instance:CloseWarningView()
    self.ActTime:CloseCountDownTime()
    self.MoveObj = nil
    Runner.Instance:RemoveRunUntilTrue(self.handle_runnerskill)
    TimeHelper:CancelTimer(self.handle_showscene)
    TimeHelper:CancelTimer(self.handle_runframe)
    TimeHelper:CancelTimer(self.handle_coutdown)
    TimeHelper:CancelTimer(self.handle_coutdown2)
    TimeHelper:CancelTimer(self.status_flush_timer)
    RuinsData.Instance:ClearRuinsBoss()
    UIWFlushPanel.OnDestroy(self)
    self.check_once = nil
end
function RuinsViewPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushTimeAndBattery()
    RuinsData.Instance:CreateRuinsBoss()
end
function RuinsViewPanel:FlushBoards()
    local add_ch_vos = {}
    local remove_uis = {}

    for vo, ui in pairs(self.data:GetSceneUiVos()) do
        remove_uis[vo] = ui
    end

    self.main_data:ForeachObjVo(function(vo)
        if remove_uis[vo] ~= nil
        then
            remove_uis[vo] = nil
        else
            table.insert(add_ch_vos, vo)
        end
    end)
    ---remove unused ui
    for vo, ui in pairs(remove_uis) do
        ui:RecycleSelf()
        self.data:SetSceneUiVos(vo)
    end
    ---add new info
    for _, vo in pairs(add_ch_vos) do
        if SceneConfig.RoleType[vo.obj_type] then 
            local ui = self.RolePool:Spwan(vo)
            if ui ~= nil then
                self.data:SetSceneUiVos(vo,ui)
            end
        end
    end
end
function RuinsViewPanel:FlushTimeAndBattery()
    TimeHelper:CancelTimer(self.status_flush_timer)
    self.status_flush_timer = TimeHelper:AddRunTimer(function()
        self.DianLiangImg.fillAmount = Nirvana.DeviceTool.GetBatteryLevel() / 100
        local server_time = TimeManager:GetServerTime()
        if self.cur_time ~= tonumber(os.date("%M", server_time)) then
            UH.SetText(self.TimeTex, os.date("%H:%M", server_time))
            self.cur_time = tonumber(os.date("%M", server_time))
        end
        if self.AutoToggle.isActiveAndEnabled and self.check_once == nil then
            if self.data.is_auto_battle_boss == true then
                self.AutoToggle:Trigger()
            else
                self.ManualToggle:Trigger()
            end
            self.check_once = true
        end
    end,10,-1,true)
end

function RuinsViewPanel:FlushPetHead()
    local fight_pet_info = self.pet_data:GetFightPetInfo()
    if fight_pet_info == nil then
        self.PetHead:Clean()
        self.PetUnFightTxtObj:SetActive(true)
    else
        self.PetHead:SetDataByPet(fight_pet_info)
        self.PetUnFightTxtObj:SetActive(false)
    end
end

function RuinsViewPanel:FlushRoleHead()
    self.RoleHead:SetData(RoleData.Instance:GetRoleData())
end

function RuinsViewPanel:FlushWifi()
    local delay = self.time_ctrl:GetDelayTime()
    local index = 4
    if delay == nil then
        index = 2
    end
    if delay and delay > Delay_Type.Nomal then
        index = 3
    end
    if delay and delay > Delay_Type.Bad then
        index = 2
        PublicPopupCtrl.Instance:Center("网络不稳定")
    end
    if self.cur_wifi ~= index then
        UH.SpriteName(self.WifiSp, "XiTongXinXi_XinHao" .. index)
        self.cur_wifi = index
    end
end


function RuinsViewPanel:OnClickBack()
    ViewMgr:CloseView(RuinsView)
    SceneCtrl.Instance:RequestLeaveFb()
    -- SceneCtrl.Instance:GotoScene(GameScene)
end

function RuinsViewPanel:OnClickTip()
    --[[ for i = 1, 10, 1 do
        self.LayerItems[i]:PlayTime()
    end ]]
    -- RuinsCtrl.Instance:ChangePlatform(6)
    -- self:FlushLayerItemShow()
    PublicPopupCtrl.Instance:HelpTip(391)
end

function RuinsViewPanel:OnClickPet()
    local count = table.nums(PetData.Instance:GetPetList())
    local count2 = table.nums(PetData.Instance:GetStorageList())
    if count == 0 and count2 > 0 then
        local open_storage = function ()
            ViewMgr:CloseView(DialogTipsView)
            ViewMgr:OpenView(PetView)
            TimeHelper:AddDelayTimer(function ()
                ViewMgr:OpenView(PetStorage)
            end, 0.5)
        end
        local open_pet = function ()
            ViewMgr:OpenView(PetView)
            ViewMgr:CloseView(DialogTipsView)
        end
        local info = {
            content = ErrorText[127],
            confirm = {
                func = open_storage
            },
            cancel = {
                func = open_pet
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end
    ViewMgr:OpenView(PetView)
end

function RuinsViewPanel:OnClickRole()
    ViewMgr:OpenView(RoleView)
end

function RuinsViewPanel:OnClickMap()
    ViewMgr:OpenView(RuinsMap)
end

function RuinsViewPanel:OnClickChallengeRank()
    RuinsCtrl.Instance:ReqRank(0)
    RuinsCtrl.Instance:ReqRank(1)
    RuinsCtrl.Instance:ReqRank(2)
    ViewMgr:OpenView(RuinsChallengeRank)
end
function RuinsViewPanel:OnClickAchievement()
    ViewMgr:OpenView(RuinsAchi)
end

function RuinsViewPanel:OnClickRank()
    RuinsCtrl.Instance:ReqRank(3)
    RuinsCtrl.Instance:ReqRank(4)
    ViewMgr:OpenView(RuinsRank)
end

function RuinsViewPanel:OnClickBuZhen()
    ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_GOD_SPACE, {hide_challenge = true})
end
function RuinsViewPanel:FlushLayerItem()
    if MainPlayer then
        self.MyPos.Target = MainPlayer.root_node.transform
    end
    local scene = self.data:GetSceneMono()
    if scene == nil then
        return
    end
    local LayerObjs = scene.LayerObjs
    local DoorObjs = scene.DoorObjs
    for i = 1, 10, 1 do
        self.LayerItems[i]:SetDoorEffect(DoorObjs[i])
        self.LayerItems[i]:SetTarget(LayerObjs[i])
        self.LayerItems[i]:HideAll()
    end
    -- self:FlushBossInfo()
    -- self:FlushBossChallenge()
end
function RuinsViewPanel:FlushLayerItemShow()
    --根据我当前所在台子进行路径显示等
    local role_info = self.data:GetRoleInfo()
    local platform_id = role_info.platform_id + 1
    local scene_config = self.data:SceneInfoConfig(platform_id - 1)
    local layer_id = scene_config.layer
    local time_info = self.data:GetTimeInfo()
    LogDG("platform_id", platform_id, scene_config)
    if scene_config then
        local scene = self.data:GetSceneMono()
        if scene then
            local ArrowObjs = scene.ArrowObjs
            for i = 1, ArrowObjs:Length() do
                ArrowObjs[i]:SetActive(false)
            end
            local LayerObjs = scene.LayerObjs
            if MainPlayer then
                if self.cur_platform ~= platform_id then
                    self.SceneName:SetActive(true)
                    UH.SetText(self.SceneNameTex, string.format(Language.GodAreasActs.EnterPlatform, scene_config.name))
                    TimeHelper:CancelTimer(self.handle_showscene)
                    self.handle_showscene = TimeHelper:AddDelayTimer(function ()
                        self.SceneName:SetActive(false)
                    end, 1)
                    self:OnClickMyPos()
                    local obj = LayerObjs[platform_id]
                    local pos = obj:GetPosition()
                    local y, suc = SceneCtrl.Instance:CurScene():GetPosHeight(pos.x, pos.z)
                    pos.y = y
                    MainPlayer:MoveTo(pos, true)
                    self.cur_platform = platform_id
                    if self.cur_platform == self.data:CanBattleBossPlatformId() + 1
                    and self.data.is_auto_battle_boss == true then
                        self:OnClickAttackBoss()
                    end
                end
            end
        end
        
        local temp = {}--可走台子
        local list = string.split(scene_config.can_goto_platform_id, "|")
        for k, v in pairs(list) do
            temp[tonumber(v) + 1] = true
        end
        local cantGoCd = self.data:ChangePlatformCD()
        local offset = TimeCtrl.Instance:GetServerTime() - time_info.goto_time
        if offset < cantGoCd then
            --显示倒计时然后显示可走
            for i = 1, 10, 1 do
                if temp[i] and temp[i] == true then
                    self.LayerItems[i]:PlayTime()
                else
                    self.LayerItems[i]:HideAll()
                end
            end
            return
        end
        --直接显示可走
        for i = 1, 10, 1 do
            if temp[i] and temp[i] == true then
                self.LayerItems[i]:ShowCanGo()
            else
                self.LayerItems[i]:HideAll()
            end
        end
        self.data:GetSceneInfo().flush_arrow = not self.data:GetSceneInfo().flush_arrow
    end
end
function RuinsViewPanel:FlushSceneArrow()
    local role_info = self.data:GetRoleInfo()
    local platform_id = role_info.platform_id + 1
    local scene_config = self.data:SceneInfoConfig(platform_id - 1)
    local layer_id = scene_config.layer
    local scene = self.data:GetSceneMono()
    if scene then
        local idx = layer_id + 1
        local ArrowObjs = scene.ArrowObjs
        for i = 1, ArrowObjs:Length() do
            ArrowObjs[i]:SetActive(idx == i)
        end
    end
end
function RuinsViewPanel:FlushBossInfo()
    local scene = self.data:GetSceneMono()
    if scene == nil then
        return
    end
    local LayerObjs = scene.LayerObjs
    self.BossInfo.Target = LayerObjs[LayerObjs:Length()].transform
    local boss_info = self.data:GetBossInfo()
    local boss_config = self.data:GetBossConfig()
    if boss_config then
        UH.SetText(self.BossName, boss_config.boss_name)
        UH.SetText(self.BossName2, boss_config.boss_name) 
    end
    local boss_idx = boss_info.boss_index
    UH.SetText(self.BattleRankName, string.format(Language.GodAreasActs.BattleDesc2, Language.GodAreasActs.BattleName[boss_idx]))
    UH.SetText(self.BattleRankDesc, string.format(Language.GodAreasActs.BattleDesc, Language.GodAreasActs.BattleName[boss_idx]))
    self.BossHpBar.fillAmount = ((10000 * boss_idx) + (10000 - boss_info.boss_remain_hp)) / 30000
    --[[ self.BossHead[1]:SetActive(boss_idx > 0)
    self.BossHead[2]:SetActive(boss_idx > 1)
    self.BossHead[3]:SetActive(boss_idx > 2) ]]
    for i = 1, self.BossHead:Length() do
        self.BossHead[i]:SetActive(boss_idx > (i - 1))
    end
    if self.BossHpBar.fillAmount >= 1 then
        for i = 1, self.BossHead:Length() do
            self.BossHead[i]:SetActive(true)
        end 
    end
    LogDG("FlushBossInfo")
    self.BossHpBar2.fillAmount = boss_info.boss_remain_hp / 10000
    self.BossHpBar3.fillAmount = boss_info.boss_remain_hp / 10000
    
    UH.SetText(self.BossHpTex, string.format(Language.GodAreasActs.Life, (boss_info.boss_remain_hp / 100)))
end
function RuinsViewPanel:FlushBossChallenge()
    LogDG("FlushBossChallenge")
    self.BossHpObj:SetActive(true)
    self.BossMsg:SetActive(false)
    local time_info = self.data:GetTimeInfo()
    local boss_can_challenge_time = time_info.boss_can_challenge_time
    local boss = self.data:GetRuinsBoss()
    local curr_time = TimeCtrl.Instance:GetServerTime()
    if boss_can_challenge_time == 0 then
        if boss then
            boss:SetVisible(true)
        end
    else
        if boss then
            boss:SetVisible(curr_time > boss_can_challenge_time)
        end
    end
    if curr_time < boss_can_challenge_time then
        self.is_wait_flush_boss = true
        self.BossMsg:SetActive(true)
        self.BossHpObj:SetActive(false)
        --进行一个倒计时
        self.ShowTime:SetCallBack(BindTool.Bind(self.ShowBoss, self))
        self.ShowTime:StampTime(boss_can_challenge_time)
    else
        self.BossHpObj:SetActive(true)
    end
end
function RuinsViewPanel:ShowBoss()
    self.BossMsg:SetActive(false)
    local boss = self.data:GetRuinsBoss()
    boss:SetVisible(true)
    self.BossHpObj:SetActive(true)
    self.is_wait_flush_boss = false
    self:CanBattleBoss()
end

function RuinsViewPanel:ShowBossSkill()
    local time_info = self.data:GetTimeInfo()
    local boss_skill_time = time_info.boss_skill_time
    if boss_skill_time == 0 then
        self.BossSkill:SetActive(false)
        return
    end

    local curr_time = TimeCtrl.Instance:GetServerTime()
    if boss_skill_time - curr_time < 0 then
        return
    end
    Runner.Instance:RemoveRunUntilTrue(self.handle_runnerskill)
    self.handle_runnerskill = Runner.Instance:RunUntilTrue(function ()
        curr_time = TimeCtrl.Instance:GetServerTime()
        if boss_skill_time - curr_time <= 4 then
            self:ShowBossSkill2()
            return true
        end
    end)
end
function RuinsViewPanel:ShowActOver()
    local all_time = self.data:GetOverTime()
    local time = all_time
    self.ActOver:SetActive(true)
    TimeHelper:CancelTimer(self.handle_coutdown2)
    self.handle_coutdown2 =  TimeHelper:AddCountDownTT(function ()
        self.ActOverST.text = time
        time = time - 1
        if time < 0 then
            self.ActOver:SetActive(false)
            ViewMgr:CloseView(RuinsView)
            SceneCtrl.Instance:RequestLeaveFb()
        end
    end, function ()
        
    end,all_time, 1, true)
end
function RuinsViewPanel:ShowBossSkill2()
    local time = 3
    self.BossSkill:SetActive(true)
    TimeHelper:CancelTimer(self.handle_coutdown)
    self.handle_coutdown =  TimeHelper:AddCountDownTT(function ()
        self.BossSkillST.text = time
        time = time - 1
        if time < 0 then
            self.BossSkill:SetActive(false)
        end
    end, function ()
        
    end,3, 1, true)
end
function RuinsViewPanel:OnClickMyPos()
    self:CheckScene()
    local role_info = self.data:GetRoleInfo()
    local id = role_info.platform_id + 1
    local info = self.pos_list[id]
    local p1 = self.MoveObj.localPosition
    local p2 = Vector3.New(info.x, 0, info.z)
    local total = 24
    local frame = 0
    TimeHelper:CancelTimer(self.handle_runframe)
    self.handle_runframe = TimeHelper:AddRunFrameTimer(function ()
        if self.MoveObj ~= nil and not self.MoveObj:Equals(nil) then
            self.MoveObj.localPosition = Vector3.Lerp(p1, p2, frame/total)
        end
        frame = frame + 1
    end,1, total, true)
end

function RuinsViewPanel:OnValueChange()
    self:CheckScene()
    if Input.GetMouseButton(0) and self.MoveObj ~= nil and not self.MoveObj:Equals(nil) then
        TimeHelper:CancelTimer(self.handle_runframe)
        local axisX = Input.GetAxis("Mouse X")
        local axisY = Input.GetAxis("Mouse Y")
        if axisX > 0 then
            axisX = axisX > 1 and 1 or axisX
        else
            axisX = axisX < -1 and -1 or axisX
        end
        if axisY > 0 then
            axisY = axisY > 1 and 1 or axisY
        else
            axisY = axisY < -1 and -1 or axisY
        end
        self.MoveObj:Translate(Vector3.right:Mul((axisX * self.move_speed + axisY * self.offsetX)));
        self.MoveObj:Translate(Vector3.forward:Mul((axisY * self.move_speed + axisX * self.offsetY)));
    end
end
function RuinsViewPanel:CheckScene()
    if self.VirCamObj == nil then
        self.offsetX = math.cos(math.rad(150))
        self.offsetY = math.sin(math.rad(150))
        self.VirCamObj = SceneMgr.FindGameObjectWithTag("MainVirtualCamera")
        self.VirCamMono = self.VirCamObj:GetComponent(typeof(Cinemachine.CinemachineVirtualCamera))
        self.MoveObj = self.VirCamObj.transform.parent
    end
end

function RuinsViewPanel:FlushRoleInfo()
    local role_info = self.data:GetRoleInfo()
    local full_value = self.data:FullActionValue()
    UH.SetText(self.MyPingFen, RoleData.Instance:GetSumScore())
    UH.SetText(self.MyJiFen, role_info.score)
    UH.SetText(self.MyBingLi, string.format(Language.GodAreasActs.BingLiZhi, role_info.action_value, full_value))
    self.BingLiBar.fillAmount = role_info.action_value / full_value
end
function RuinsViewPanel:FlushRoleListInfo()
    local role_info = self.data:GetRoleInfo()
    local server_id = self.data:ServerId()
    local platform_id = role_info.platform_id
    local list = self.data:GetSceneRoleList(platform_id)
    local key = 0
    local uuid = RoleData.Instance:UUID()--self.data:UUID()
    local role_num = 0
    for k, v in pairs(list) do
        if v.role_uuid == uuid then
            key = k
        end
        if v.server_id == server_id then
            role_num = role_num + 1
        end
    end
    table.remove(list, key)
    self.RoleList:SetData(list)
    UH.SetText(self.ServerRoleNum, string.format(Language.GodAreasActs.ServerRole, role_num))
end
function RuinsViewPanel:FlushAttackRoleTime()
    local time_info = self.data:GetTimeInfo()
    local attack_time = time_info.attack_role_time
    local curr_time = TimeCtrl.Instance:GetServerTime()
    local role_cd = self.data:GetBattleRoleCD()
    local offset = role_cd - (curr_time - attack_time)
    if offset > 0 then
        local items = self.RoleList.item_list
        for k, v in pairs(items) do
            v:PlayTime(offset)
        end
    else
        local items = self.RoleList.item_list
        for k, v in pairs(items) do
            v:CanBattle()
        end
    end
end
function RuinsViewPanel:FlushAttackBossTime()
    local time_info = self.data:GetTimeInfo()
    local attack_time = time_info.attack_boss_time
    local curr_time = TimeCtrl.Instance:GetServerTime()
    local boss_cd = self.data:GetBattleBossCD()
    local offset = boss_cd - (curr_time - attack_time)
    if offset > 0 then
        self.BossBattleBlock:SetActive(true)
        self.BossBattleTime:SetCallBack(BindTool.Bind(self.CanBattleBoss, self))
        self.BossBattleTime:TotalTime(offset, TimeType.Type_Special_5, nil, "")
    else
        self.BossBattleBlock:SetActive(false)
    end
end
function RuinsViewPanel:CanBattleBoss()
    self.BossBattleBlock:SetActive(false)
    --这里还要判断当前是否处于等待时间
    if self.is_wait_flush_boss == true then
        return
    end
    if self.data.is_auto_battle_boss == true then
        self:OnClickAttackBoss()
    end
end
function RuinsViewPanel:OnClickTestPos(id)
    self:CheckScene()
    local info = self.pos_list[id]
    local p1 = self.MoveObj.localPosition
    local p2 = Vector3.New(info.x, 0, info.z)
    local total = 24
    local frame = 0
    TimeHelper:CancelTimer(self.handle_runframe)
    self.handle_runframe = TimeHelper:AddRunFrameTimer(function ()
        if self.MoveObj ~= nil and not self.MoveObj:Equals(nil) then
            self.MoveObj.localPosition = Vector3.Lerp(p1, p2, frame/total)
        end
        frame = frame + 1
    end,1, total, true)    
end

function RuinsViewPanel:FlushBossItemInfo()
    
end

function RuinsViewPanel:OnToggleAuto()
    LogDG("auto")
    self.data.is_auto_battle_boss = true
end

function RuinsViewPanel:OnToggleManual()
    LogDG("manual")
    self.data.is_auto_battle_boss = false
end

function RuinsViewPanel:OnClickAttackBoss()
    if self.cur_platform == self.data:CanBattleBossPlatformId() + 1 then
        RuinsCtrl.Instance:ChallengeBoss()
    else
        PublicPopupCtrl.Instance:Center(Language.GodAreasActs.PosNoFight)
    end
end

function RuinsViewPanel:OnClickBlockBoss()
    local time_info = self.data:GetTimeInfo()
    local attack_time = time_info.attack_boss_time
    local curr_time = TimeCtrl.Instance:GetServerTime()
    local boss_cd = self.data:GetBattleBossCD()
    local offset = boss_cd - (curr_time - attack_time)
    PublicPopupCtrl.Instance:Center(string.format(Language.GodAreasActs.FightTime, math.floor(offset)))
end

function RuinsViewPanel:OnClickBlockRole()
    local time_info = self.data:GetTimeInfo()
    local attack_time = time_info.attack_role_time
    local curr_time = TimeCtrl.Instance:GetServerTime()
    local role_cd = self.data:GetBattleRoleCD()
    local offset = role_cd - (curr_time - attack_time)
    PublicPopupCtrl.Instance:Center(string.format(Language.GodAreasActs.FightTime, math.floor(offset)))
end
function RuinsViewPanel:OnClickBattleReport()
    ViewMgr:OpenView(GodAreasBattleReport, self.data.battle_report)    
end

RuinsLayerItem = RuinsLayerItem or DeclareMono("RuinsLayerItem", UIWidgetBaseItem)
function RuinsLayerItem:RuinsLayerItem()
    self.ruins_data = RuinsData.Instance
end
function RuinsLayerItem:OnDestroy()
    self.DoorEffect = nil
end
function RuinsLayerItem:SetDoorEffect(door)
    self.DoorEffect = door
end
function RuinsLayerItem:SetTarget(layer)
    self.UiFollow.Target = layer.transform
    self.Time:SetCallBack(BindTool.Bind(self.ShowCanGo, self))
    self:HideAll()
end
--倒计时
function RuinsLayerItem:PlayTime()
    self.Go:SetActive(false)
    self.Arrow:SetActive(true)
    local cd = self.ruins_data:ChangePlatformCD()
    self.Time:TotalTime(cd, TimeType.Type_Special_5)
    if self.DoorEffect then
        self.DoorEffect:SetActive(false)
    end
end
function RuinsLayerItem:ShowCanGo()
    self.ruins_data:GetSceneInfo().flush_arrow = not self.ruins_data:GetSceneInfo().flush_arrow
    self.Arrow:SetActive(false)
    self.Go:SetActive(true)
    if self.DoorEffect then
        self.DoorEffect:SetActive(true)
    end
end
function RuinsLayerItem:HideAll()
    self.Arrow:SetActive(false)
    self.Go:SetActive(false)
    if self.DoorEffect then
        self.DoorEffect:SetActive(false)
    end
end
function RuinsLayerItem:OnClickGo()
    RuinsCtrl.Instance:ChangePlatform(self.PlatfromId)
end

RuinsRoleItem = RuinsRoleItem or DeclareMono("RuinsRoleItem", UIWidgetBaseItem)
function RuinsRoleItem:RuinsRoleItem()
    self.ruins_data = RuinsData.Instance
end
function RuinsRoleItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.Name, data.role_name)
    UH.SetText(self.PingFen, data.cap)
    local full_value = self.ruins_data:FullActionValue()
    UH.SetText(self.BingLiZhi, string.format(Language.GodAreasActs.BingLiZhi, data.action_value, full_value))
    self.BingLiBar.fillAmount = data.action_value / full_value
end

function RuinsRoleItem:OnClickAttackRole()
    local role_info = self.ruins_data:GetRoleInfo()
    if role_info.platform_id  == 0 then
        PublicPopupCtrl.Instance:Center(Language.GodAreasActs.DontFight)
    else
        RuinsCtrl.Instance:ChallengeRole(self.data.plat_type, self.data.role_id)
    end
end

function RuinsRoleItem:PlayTime(cd)
    self.Block:SetActive(true)
    -- local cd = self.ruins_data:GetBattleRoleCD()
    self.Time:SetCallBack(BindTool.Bind(self.CanBattle, self))
    self.Time:TotalTime(cd, TimeType.Type_Special_5, nil, "")
end
function RuinsRoleItem:CanBattle()
    self.Block:SetActive(false)
end

BossHurtItem = BossHurtItem or DeclareMono("BossHurtItem", UIWObjPoolItem)
function BossHurtItem:setData(data)
    self.Hurt.text = "-"..data.total_hurt
end
function BossHurtItem:OnAnimEnd()
    self:RecycleSelf()
end
function BossHurtItem:onRecycle()
    self.Hurt.text = nil
end



RuinsRoleInfoItem = RuinsRoleInfoItem or DeclareMono("RuinsRoleInfoItem", UIWObjPoolItem)
function RuinsRoleInfoItem:RuinsRoleInfoItem()
    
end
function RuinsRoleInfoItem:setData(data)
    --显示名字
    self.ch_vo = data
    if self.ch_vo ~= nil and self.ch_vo.root_obj ~= nil and self.ch_vo.root_obj.transform ~= nil then
        self.Follow.Target = self.ch_vo.root_obj.transform
    else
        self.Follow.Target = nil
    end
    UH.SpriteName(self.Icon, RuinsData.FactionIcon[data.faction_id or 0])
    UH.SetText(self.Name, self.ch_vo.name)
end
function RuinsRoleInfoItem:onRecycle()
    self.Follow.Target = nil
end
