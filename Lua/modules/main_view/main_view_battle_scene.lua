---------------------------MainViewBattleScene 战斗界面中的场景UI------------------------------
MainViewBattleScene = MainViewBattleScene or DeclareMono("MainViewBattleScene", UIWFlushPanel)
---Popups=======UIWObjPool[](BattlePopupBase)
---ChStatePool==UIWObjPool(BattleSceneRoleState)
---ChClickerPool==UIWObjPool(BattleSceneViewChClicker)
---RoundCountDown=SpriteText
---BottomInfoPool==UIWObjPool(BattleSceneRoleState)
function MainViewBattleScene:MainViewBattleScene()
    self.base_data = MainViewData.Instance:BaseData()
    self.battle_data = BattleData.Instance
    -- self.bullet_comments = self.battle_data.bullet_comments
    local popup_data = self.battle_data:PopupData()
    -- local ch_info = self.battle_data:ChInfo()
    local base_data = self.battle_data:BaseData()
    self.data_cares = {
        {data = popup_data, func = self.OnHpPopup},
        {data = self.battle_data.view_data, func = self.FlushChInfoRootObj, keys = {"flush_info"}},
        {data = self.base_data, func = self.OnChangeBattleState, keys = {"is_battle"}},
        {data = base_data, func = self.FlushCountdown, keys = {"cmd_left_time", "round_phase", "auto", "cam_type"}},
        {data = base_data, func = self.FlushBehavior, keys = {"command_i", "round_phase", "auto"}},
        -- {data = self.bullet_comments, func = self.FlushBulletComments},
        -- {data = base_data, func = self.BulletSwitch, keys = {"bullet_on"}}
    }
    self.info_ui = {} --key:ch_info,value:ChStatePoolItem
    self.grid_ui = {} --key:grid_index,value:ChStatePoolItem

    -- self.CountDownAnim:WriteDefaultValues()
    -- LogError("WriteDefaultValues!!!!!!!!!!!!!!!")

    GuideManager.Instance:RegisterGetGuideUi(
        "Enemy13",
        function()
            if self.grid_ui[13] and self.grid_ui[13]:IsActiveAndEnabled() then
                return self.grid_ui[13].EventClick
            end
            return nil, nil
        end
    )
    GuideManager.Instance:RegisterGetGuideUi(
        "Enemy19",
        function()
            if self.grid_ui[19] and self.grid_ui[19]:IsActiveAndEnabled() then
                return self.grid_ui[19].EventClick
            end
            return nil, nil
        end
    )
    GuideManager.Instance:RegisterGetGuideUi(
        "Enemy18",
        function()
            if self.grid_ui[18] and self.grid_ui[18]:IsActiveAndEnabled() then
                return self.grid_ui[18].EventClick
            end
            return nil, nil
        end
    )
    GuideManager.Instance:RegisterGetGuideUi("Enemy3", function()
        if self.grid_ui[3] and self.grid_ui[3]:IsActiveAndEnabled() then
            return self.grid_ui[3].EventClick
        end
        return nil, nil
    end)
    GuideManager.Instance:RegisterGetGuideUi("Enemy5", function()
        if self.grid_ui[5] and self.grid_ui[5]:IsActiveAndEnabled() then
            return self.grid_ui[5].EventClick
        end
        return nil, nil
    end)
    GuideManager.Instance:RegisterGetGuideUi("Enemy8", function()
        if self.grid_ui[8] and self.grid_ui[8]:IsActiveAndEnabled() then
            return self.grid_ui[8].EventClick
        end
        return nil, nil
    end)

    GuideManager.Instance:RegisterGetGuideUi("BattleMyPos", function()
        local index = self.battle_data:PlayerIdx()
        if self.grid_ui[index]:IsActiveAndEnabled() then
            return self.grid_ui[index].EventClick
        end
        return nil, nil
    end)
end
local beh_move_sp =
{
    "_LocRoleMove",
    "_LocPetMove",
    "_LocRoleMove1",
    "_LocRoleMove2"
}
function MainViewBattleScene:FlushBehavior()
    --非自动显示
    if BattleData.Instance:BaseData().battle_mode == BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE
    or BattleData.Instance:BaseData().battle_mode == BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY then
        self.BehSpList[1].gameObject:SetActive(false)
        self.BehSpList[2].gameObject:SetActive(false)
        self.BehBg:SetActive(false)
        return
    end
    self.BehSpList[1].gameObject:SetActive(not self.battle_data:BaseData().auto)
    self.BehSpList[2].gameObject:SetActive(not self.battle_data:BaseData().auto)
    self.BehBg:SetActive(not self.battle_data:BaseData().auto)
    local flag = self.battle_data:IsChAlive(self.battle_data:PlayerPetIdx())
    local pet_ch = self.battle_data:GetChInfo(self.battle_data:PlayerPetIdx())
    local command_i = self.battle_data:BaseData().command_i
    if pet_ch == nil or pet_ch.curhp > 0 then
    else
        flag = true
    end
    if flag == false then
        UH.SpriteName(self.BehSpList[1], beh_move_sp[command_i + 2])
        UH.SpriteName(self.BehSpList[2], beh_move_sp[command_i + 2])
    else
        UH.SpriteName(self.BehSpList[1], beh_move_sp[command_i])
        UH.SpriteName(self.BehSpList[2], beh_move_sp[command_i])
    end
end
function MainViewBattleScene:ClearCountDownEffect()
    -- LogErrorHL("默认状态 ClearCountDownEffect")
    -- 这里设了trigger，但下一帧才更新动画，但那时候动画机已经被关了
    self.CountDownAnim:SetTrigger("tri_default")
    if self.on_count_down_done then
        TimeHelper:CancelTimer(self.on_count_down_done)
        self.on_count_down_done = nil
    end
    -- self.CountDownAnim.enabled = false
    self.CountDownBG:SetActive(false)
    self.CountDownGO:SetActive(false)
    self.pre_show_str = nil
end

function MainViewBattleScene:OnDisable()
    -- LogError("OnDisable")
    self:ClearCountDownEffect()
end

function MainViewBattleScene:OnChangeBattleState()
    if not self.base_data.is_battle then
        self.Popups[1]:RecycleActiveObjs()
    end
end

function MainViewBattleScene:OnHpPopup()
    if GmData.Instance.view_data.hide_popup == true then
        return
    end
    local popup_datas = self.battle_data:PopPopupData()
	if table.nums(popup_datas) > 0 and BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_GUILD_BOSS then
		local damage = GuildData.Instance:CalculateGuildTrialDamage(popup_datas)
	end
    for _, v in pairs(popup_datas) do
        -- 也就是test_popup
        self.Popups[1]:Spwan(v)
    end
end

--add&remove role info ui
function MainViewBattleScene:FlushChInfoRootObj()
    local ch_info = self.battle_data:ChInfo()

    local add_ch_infos = {}
    local remove_uis = {}
    for info, ui in pairs(self.info_ui) do
        remove_uis[info] = ui--self.grid_ui[info.grid_idx]
    end
    for idx, info in pairs(ch_info) do
        if remove_uis[info] ~= nil then
            remove_uis[info] = nil
        else
            table.insert(add_ch_infos, info)
        end
    end

    -- 神龙
    local dgn_list = self.battle_data:GetDragonList()
    if not TableIsEmpty(dgn_list) then
        for i,v in pairs(dgn_list) do
            if remove_uis[v] ~= nil then
                remove_uis[v] = nil
            else
                table.insert(add_ch_infos, v)
            end
        end
    end

    ---remove unused ui
    for info, ui in pairs(remove_uis) do
        -- LogError("remove", info.grid_idx, info.name)
        ui:RecycleSelf()
        self.info_ui[info] = nil
        self.grid_ui[info.grid_idx] = nil
    end
    ---add new info
    for _, info in pairs(add_ch_infos) do
        -- LogError("add", info.grid_idx, info.name)'
        local ui
        if info.is_bottom_info then
            ui = self.BottomInfoPool:Spwan(info)
        else
            ui = self.ChStatePool:Spwan(info)
        end
        self.info_ui[info] = ui
        self.grid_ui[info.grid_idx] = ui
    end

   --[[  self.ChStatePool:Reorder(
        function(a, b)
            local function get_score(item)
                local idx = item.data.grid_idx
                local score = idx
                if idx <= 5 then
                    score = score + 100000
                elseif idx <= 10 then
                    score = score + 10000
                elseif idx <= 15 then
                    score = score + 1000
                elseif idx <= 20 then
                    score = score + 100
                end
                return score
            end
            return get_score(a) > get_score(b)
        end
    ) ]]
end

function MainViewBattleScene:FlushChHp(ch_info)
    local ui = self.info_ui[ch_info]
    if ui ~= nil then
        ui:FlushHp()
    end
end

function MainViewBattleScene:FlushChMp(ch_info)
    local ui = self.info_ui[ch_info]
    if ui ~= nil then
        ui:FlushMp()
    end
end

function MainViewBattleScene:FlushCountdown(old_vals)
    -- self.CountDownEffect:SetActive(false)
    -- self.CountDownBG:SetActive(false)
    -- self.CountDownGO:SetActive(false)
    -- LogError("FlushCountdown")

    local base_data = self.battle_data:BaseData()
    if base_data.round_phase ~= BATTLE_ROUND_PHASE.Command or GuideManager.Instance:IsGuide() then
        self.RoundCountDown1Parent:SetActive(false)
        self.RoundCountDown2Parent:SetActive(false)
        return
    end

    local show_str = nil
    local flag = false
    local is_zero = false
    -- if old_vals and old_vals.auto == false and base_data.auto == true then
    --     BattleCtrl.Instance:CancelCountdownTimer()
    --     return
    -- end
    if base_data.round_phase == BATTLE_ROUND_PHASE.Command or base_data.round_phase == BATTLE_ROUND_PHASE.Server then
        if BattleData.Instance:BaseData().cur_round <= 1 then
            self.RoundCountDown1Parent:SetActive(true)
            self.RoundCountDown2Parent:SetActive(false)
        else
            self.RoundCountDown1Parent:SetActive(false)
            self.RoundCountDown2Parent:SetActive(true)
        end

        if old_vals and old_vals.auto ~= nil then
            if base_data.auto then
                -- BattleCtrl.Instance:CancelCountdownTimer()
                -- self.RoundCountDown.text = ""
                return
            else
                -- LogError("!!!!!!!!!!!!!!!!!!!!!")
                self:ClearCountDownEffect()
            end
        end
        local ct_offset = BattleData.Instance:GetCountDownOffset()
        local show_num = base_data.cmd_left_time - ct_offset
        -- LogError(base_data.cmd_left_time, ct_offset, show_num)
        if show_num < 0 then
            self.RoundCountDown1Parent:SetActive(false)
            self.RoundCountDown2Parent:SetActive(false)
            -- self.RoundCountDown.text = ""
            -- self.RoundCountDown2.text = ""
            -- self:ClearCountDownEffect()
            return
        end
        if show_num > 9 then
            show_str = string.format("%d%d", show_num / 10, show_num % 10)
        elseif show_num == 0 then
            show_str = ""
            is_zero = true
        else
            show_str = tostring(show_num)
        end
        flag = show_num <= 3
    end
    self.RoundCountDown.text = show_str
    if is_zero then
        --self.RoundCountDown2.text = "0"
    else
        self.RoundCountDown2.text = show_str
    end
    local scene_data = self.battle_data:SceneData()
    if scene_data then
        if scene_data:CamType() == 0 then
            if BattleData.Instance:BaseData().cur_round <= 1 then
                self.RoundCountDown1Parent:SetActive(true)
                self.RoundCountDown2Parent:SetActive(false)
            end
        else
            self.RoundCountDown1Parent:SetActive(false)
            self.RoundCountDown2Parent:SetActive(true)
        end
    end
    if not flag then
        -- self:ClearCountDownEffect()
        -- LogError("not flag")
    elseif show_str and (not self.pre_show_str or self.pre_show_str ~= show_str) then
        -- LogError("!!!")
        -- self.CountDownAnim.enabled = true

        -- self.CountDownEffect:SetActive(true)
        -- self.CountDownBG:SetActive(true)
        -- if show_str == "3" then
        --     -- self.CountDownBGAnim:Play()
        -- end
        if is_zero then
            -- self.CountDownAnim:Stop("countdown_321")
            -- self.CountDownAnim:Play("countdown_go")
            -- self.CountDownGO:SetActive(true)
            -- LogErrorHL("tri_go")
            self.CountDownAnim:SetTrigger("tri_go")
            if BattleData.Instance:BaseData().cur_round <= 1 then
                AudioMgr:PlayEffect(AudioType.Battle, "begin_fight")
            end
            self.on_count_down_done =
                TimeHelper:AddDelayTimer(
                function()
                    BattleData.Instance:AddBattleEffect(BattleEffectKey.Move)
                end,
                0.55
            )
        else
            -- if show_str == "1" then
            --     self.on_count_down_done =
            --         TimeHelper:AddDelayTimer(
            --         function()
            --             BattleData.Instance:AddBattleEffect(BattleEffectKey.Move)
            --         end,
            --         0.55
            --     )
            -- end
            -- self.CountDownAnim:Stop("countdown_go")
            -- self.CountDownAnim:Play("countdown_321")
            -- LogError("tri_321")
            if BattleData.Instance:BaseData().cur_round <= 1 then
                AudioMgr:PlayEffect(AudioType.Battle, "countdown" .. show_str)
            end
            self.CountDownAnim:SetTrigger("tri_321")
        end
        self.pre_show_str = show_str
    end

    -- 暂时不显示这个倒计时
    self.RoundCountDown2Parent:SetActive(false)
end

BattlePopupBase = BattlePopupBase or DeclareMono("BattlePopupBase", UIWObjPoolItem)
---Anim=======Animator
function BattlePopupBase:onCreate(pool)
    UIWObjPoolItem.onCreate(self, pool)
    self.Anim:ListenEvent(
        "end",
        function(eve_param, anim_st_info)
            self:RecycleSelf()
        end
    )
end

local function setPos(obj, x, y)
    local go = obj.gameObject
    local pos = go:GetLocalPosition()
    if x then
        pos.x = x
    end
    if y then
        pos.y = y
    end
    pos = go:SetLocalPosition(pos)
end

BattleSceneBulletComment = BattleSceneBulletComment or DeclareMono("BattleSceneBulletComment", UIWObjPoolItem)
local bullet_y = nil
--[[ function MainViewBattleScene:FlushBulletComments()
    for _, comment in ipairs(self.bullet_comments) do
        self.BulletScreen:Spwan(comment)
    end
    self.bullet_comments:Clear()
end ]]

--[[ function MainViewBattleScene:BulletSwitch()
    self.BulletScreen:SetObjActive(self.battle_data.base.bullet_on)
end ]]

-- function BattleSceneBulletComment:onCreate(pool)
--     UIWObjPoolItem.onCreate(self, pool)
-- end

function BattleSceneBulletComment:setData(data)
    local x = 0
    local y = 0
    if bullet_y == nil then
        bullet_y = GameMath.Ramdon(-100, 350)
    else
        bullet_y = bullet_y + 42 * GameMath.Ramdon(5)
        if bullet_y >= 350 then
            bullet_y = GameMath.Ramdon(-100, 100)
        end
    end
    y = bullet_y
    --修改 改为循环式出现
    Runner.Instance:RunUntilTrue(
        function()
            if x < -2500 then
                self:RecycleSelf()
                return true
            end
            x = x - 150 * Time.deltaTime
            self.TextPos.localPosition = Vector2.New(x, y)
        end,
        true
    )
    self.Text.text = data.text
end

-- function BattleSceneBulletComment:onRecycle()

-- end

TestPopupHp = TestPopupHp or DeclareMono("TestPopupHp", BattlePopupBase)
---Txt======Text

-- local SpriteHead = {}
function TestPopupHp:onCreate(pool)
    -- BattlePopupBase.onCreate(self, pool)
    UIWObjPoolItem.onCreate(self, pool)
    -- 颜色句柄
    -- self.Color = {}
    -- self.Color.Red = self.TextHandler:AddColorScheme(COLORS.Red8, COLORS.Red18, COLORS.Red1)
    -- self.Color.Orange = self.TextHandler:AddColorScheme(COLORS.Yellow1, COLORS.White, COLORS.Orange2)
    -- self.Color.Blue = self.TextHandler:AddColorScheme(COLORS.Blue2, COLORS.White, COLORS.Blue2)
    -- self.Color.Green = self.TextHandler:AddColorScheme(COLORS.Green8, COLORS.Orange1, COLORS.Green9)
    -- self.Color.Purple = self.TextHandler:AddColorScheme(COLORS.Purple2, COLORS.Blue4, COLORS.Purple1)
    -- self.Color.White = self.TextHandler:AddColorScheme(COLORS.Yellow19, COLORS.Yellow13, COLORS.Red17)
    -- LogError("****************AddColorScheme****************")
    self.ImageObj = self.Image.gameObject
    self.TextObj = self.Text.gameObject

    -- setPos(self.TextObj, nil, 120)
end

function TestPopupHp:PackContent(head, text)
    return string.format("%s@%s|", head, text)
end

function TestPopupHp:PackHpNum(num1, num2, head, minus)
    local content = ""

    if num1 >= 0 then
        content = content .. self:PackContent(head, "+" .. tostring(num1))
    else
        if minus then
            content = content .. self:PackContent(head, "-" .. tostring(-num1))
        else
            content = content .. self:PackContent(head, tostring(-num1))
        end
    end

    -- if num2 then
    --    if num2 > 0 then
            -- content = content .. self:PackContent("FuJiaHong", "(")
            -- content = content .. self:PackContent("Whole", "_locKeZhi")
            -- content = content .. self:PackContent("FuJiaHong", string.format("%d)", num2))
        -- elseif num2 < 0 then
        -- content = content .. self:PackContent("FuJiaZi", string.format("(%d)", num2))
        -- end
    -- end
    return content
end

function TestPopupHp:PackChar(char, schemeIndex)
    local content = ""
    -- local str = string.format("<size=40>%s</size><size=30>%s</size>", string.sub(char, 0, 3), string.sub(char, 4, string.len(char)))
    -- -- content = content .. self:PackContent(string.sub(char, 0, 3), true, schemeIndex, 40)
    -- -- content = content .. self:PackContent(string.sub(char, 4, string.len(char)), true, schemeIndex, 30)
    -- content = content .. self:PackContent(str, false, schemeIndex, 40)
    return content
end

function TestPopupHp:SwitchRadialBlur(enabled, world_pos)
    -- LogError("SwitchRadialBlur", enabled)
    local cam = SceneMgr:MainCamera()
    local rb = cam:GetOrAddComponent(typeof(Crystal.PostEffectRadialBlur))
    rb.enabled = enabled
    if world_pos then
        rb:SetFocusWorldPosition(world_pos)
    -- local screen_pos = cam:WorldToScreenPoint(world_pos);
    -- rb.FocusX = screen_pos.x
    -- rb.FocusY = screen_pos.y
    end
end


--data = BattlePopupData
local data_set_frequency = {}
local function DataSetFrequency(idx)
    if data_set_frequency[idx] == nil then
        data_set_frequency[idx] = {}
        data_set_frequency[idx].time = Time.time
        data_set_frequency[idx].y = 0
    else
        if Time.time - data_set_frequency[idx].time <= 2 then
            data_set_frequency[idx].y = data_set_frequency[idx].y + 35
            if data_set_frequency[idx].y > 400  then
                data_set_frequency[idx].y = 0
            end
        else
            data_set_frequency[idx] = nil
        end
    end
    return data_set_frequency[idx]
end
function TestPopupHp:setData(data)
    self.info = data
    -- local ui_pos = data.pos
    -- 记录多个且同一个人时进行一个堆叠式Y轴变化
    if data.ch.root_node then
        self.Follow.Target = data.ch.root_node.transform
        self.Follow.enabled = true
    end
    if data and data.ch and data.ch.GetIdx and data.ch.vo then
        local data_set = DataSetFrequency(data.ch:GetIdx())
        if data_set ~= nil then
            setPos(self.Objs, nil, data_set.y + 115)
        end
    end

    local content = ""
    local pre_str = ""
    local recycle_time = 0
    if data.type == BATTLE_HP_DESC.NORMAL and data.behit_desc == BATTLE_BE_HIT_DESC.BLOCK_1 then
        self.Text.text = Language.Main.Battle.Block_1
        recycle_time = 1
    elseif data.type == BATTLE_HP_DESC.NORMAL -- or data.type == BATTLE_HP_DESC.REAL_DAMAGE
    or data.type == BATTLE_HP_DESC.SACRIFICE_HP
    or data.type == BATTLE_HP_DESC.DESPERATE then
        content = content .. self:PackHpNum(data.num, data.num2, "ShangHai",true)
        if data.behit_desc and data.behit_desc == BATTLE_BE_HIT_DESC.DEFENCE then
            self.ImageShell:SetActive(true)
            self.Image.SpriteName = "_locFangYu"
            self.ImgLayoutElement.preferredWidth = 90
            self:PlayTextAnim()
        elseif data.num2 > 0 then
            self.ImageShell:SetActive(true)
            self.Image.SpriteName = "_locKeZhi2"
            self.ImgLayoutElement.preferredWidth = 70
            self:PlayTextAnim()
        end
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.REAL_DAMAGE then
        content = content .. self:PackHpNum(data.num, data.num2, "ZhenShang",true)
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.CRITICAL then
        pre_str = "必杀"
        content = content .. self:PackHpNum(data.num, data.num2, "BiSha")
        self.ImageShell:SetActive(true)
        self.Image.SpriteName = "_locXinBiShaDi"
        self.ImgLayoutElement.preferredWidth = 155

        if data.atker and data.behiter and not BattleData.IsEnemyIdx(data.atker) and BattleData.IsEnemyIdx(data.behiter) then
            BattleData.Instance:AddBattleEffect(
                BattleEffectKey.Hit,
                function()
                    self:SwitchRadialBlur(true, data.pos)
                end,
                function()
                    self:SwitchRadialBlur(false)
                end
            )
        end
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.POISON then
        -- if BattleTextDebug then
        pre_str = "中毒"
        content = content .. self:PackHpNum(data.num, data.num2, "ShangHai")
        --self:PackChar(pre_str, self.Color.White)
        -- end
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.BURN then
        pre_str = "灼烧"
        content = content .. self:PackHpNum(data.num, data.num2, "ShangHai")
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.COMBINE_ATTACK then
        pre_str = "合击"
        content = content .. self:PackHpNum(data.num, nil, "HeJi")
        self.ImageShell:SetActive(true)
        self.NumImg:SetObjActive(true)
        self.Image.SpriteName = "_locXinHeJiDi"
        self.ImgLayoutElement.preferredWidth = 155
        self.NumImg.SpriteName = data.num2
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.ADD_HP or data.type == BATTLE_HP_DESC.HP_CRITICAL then
        pre_str = "回复"
        content = content .. self:PackHpNum(data.num, data.num2, "Lv")
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.DAMAGE_ABSORB_HP then
        pre_str = "吸血"
        content = content .. self:PackHpNum(data.num, data.num2, "Lv")
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.BUFF then
        -- content = content .. self:PackContent(buff_name, true, self.Color.Blue, 40)
        if BattleTextDebug then
            local buff_cfg = BattleData.BuffConfig(data.num)
            local buff_name = buff_cfg ~= nil and buff_cfg.buff_name or tostring(data.num)
            self.Text.text = "!!!!!!!!!!!!!!!!!!!!!!!!添加" .. data.num .. buff_name
            recycle_time = 1
        end
    elseif data.type == BATTLE_HP_DESC.BUFF_REMOVE then
        -- content = content .. self:PackContent("！！移除！！" .. buff_name, true, self.Color.Blue, 40)
        -- if BattleTextDebug then
        --     local buff_cfg = BattleData.BuffConfig(data.num)
        --     local buff_name = buff_cfg ~= nil and buff_cfg.buff_name or tostring(data.num)
        --     text = "！！移除！！" .. buff_name
        -- end
        if BattleTextDebug then
            local buff_cfg = BattleData.BuffConfig(data.num)
            local buff_name = buff_cfg ~= nil and buff_cfg.buff_name or tostring(data.num)
            self.Text.text = "!!!!!!!!!!!!!!!!!!!!!!!!移除" .. data.num .. buff_name
            recycle_time = 1
        end
    elseif data.type == BATTLE_HP_DESC.BUFF_REPLACE then
        -- content = content .. self:PackContent("！！替换！！" .. buff_name, true, self.Color.Blue, 40)
        -- if BattleTextDebug then
        --     local buff_cfg = BattleData.BuffConfig(data.num)
        --     local buff_name = buff_cfg ~= nil and buff_cfg.buff_name or tostring(data.num)
        --     text = "！！替换！！" .. buff_name
        -- end
    elseif data.type == BATTLE_HP_DESC.BUFF_MERGE then
        -- content = content .. self:PackContent("！！合并！！" .. buff_name, true, self.Color.Blue, 40)
        -- if BattleTextDebug then
        --     local buff_cfg = BattleData.BuffConfig(data.num)
        --     local buff_name = buff_cfg ~= nil and buff_cfg.buff_name or tostring(data.num)
        --     text = "！！合并！！" .. buff_name
        -- end
    elseif data.type == BATTLE_HP_DESC.DODGE then
        self.ImageShell:SetActive(true)
        self.Image.SpriteName = "_locShanBi"
        self.ImgLayoutElement.preferredWidth = 90
        self:PlayTextAnim()
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.ATTACK_BACK then
        self.ImageShell:SetActive(true)
        self.Image.SpriteName = "_locFanJi"
        self.ImgLayoutElement.preferredWidth = 90
        self:PlayTextAnim()
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.SKILL then
        local skill_cfg = SkillData.GetConfig(data.num)
        if skill_cfg and not StringIsEmpty(skill_cfg.skill_name) then
            self.Text.text = skill_cfg.skill_name
        else
            local skill_name = SkillData.Instance:GetSkillDescName(data.num, 1)
            if not StringIsEmpty(skill_name) then
                self.Text.text = skill_name
            else
                self.Text.text = string.format("技能[%d]", data.num)
            end
        end
        recycle_time = 1
    elseif data.type == BATTLE_HP_DESC.SEAL_PET then
        if data.num == 1 then
            self.Text.text = ErrorText[95]
        else
            self.Text.text = ErrorText[96]
        end
        recycle_time = 1
    elseif data.type == BATTLE_HP_DESC.IN_VAIN then
        -- content = content .. self:PackChar(pre_str, self.Color.White)
        pre_str = "无效"
        self.EffectNode.transform.localPosition = Vector3.New(0, 0, 0)
        self.effect_handle = self.EffectTool:PlayByEffectId("10045618", self.EffectNode, true)
        recycle_time = nil
    elseif data.type == BATTLE_HP_DESC.CRACK_DEFENCE then
        pre_str = "破防"
        content = content .. self:PackHpNum(data.num, data.num2, "ShangHai")
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.MP_DECREASE then
        pre_str = "消蓝"
        content = content .. self:PackHpNum(data.num, nil, "Lan", true)
        recycle_time = 2
        setPos(self.Objs, 0, 85)
    elseif data.type == BATTLE_HP_DESC.BUBBLE then
        self.Bubble:SetActive(true)
        self.BubbleText.text = RichTextHelper.ParseAllFunc(data.num, true, RichTextFormat.FormatNpcBubbleSpecial)
        recycle_time = data.num2
    elseif data.type == BATTLE_HP_DESC.BUFF_INVALID then
        setPos(self.Objs, 0, 70)
        pre_str = "未生效"
        -- content = content .. self:PackHpNum(-1, -1)
        self.ImageShell:SetActive(true)
        self.Image.SpriteName = "_locWeiShengxiao"
        self.ImgLayoutElement.preferredWidth = 108
        self:PlayTextAnim()
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.XISHOU_DAMAGE then
        setPos(self.Objs, 0, 70)
        pre_str = "吸收"
        -- content = content .. self:PackHpNum(-1, -1)
        self.ImageShell:SetActive(true)
        self.Image.SpriteName = "_LocXiShou"
        self.ImgLayoutElement.preferredWidth = 53
        self:PlayTextAnim()
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.BUFF_IMMUNE then
        setPos(self.Objs, 0, 70)
        pre_str = "免疫"
        -- content = content .. self:PackHpNum(-1, -1)
        self.ImageShell:SetActive(true)
        self.Image.SpriteName = "_locMianYi"
        self.ImgLayoutElement.preferredWidth = 70
        self:PlayTextAnim()
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.BUFF_OPPOSE then
        pre_str = "抵抗成功"
        -- content = content .. self:PackHpNum(-1, -1)
        self.ImageShell:SetActive(true)
        self.Image.SpriteName = "_locDiKangChengGong"
        self.ImgLayoutElement.preferredWidth = 146
        self:PlayTextAnim()
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.MIAO_SHA then
        pre_str = "秒杀"
        -- content = content .. self:PackHpNum(-1, -1)
        self.ImageShell:SetActive(true)
        self.Image.SpriteName = "_LocMiaoSha"
        self.ImgLayoutElement.preferredWidth = 146
        self:PlayTextAnim()
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.SEED_EXPLODE then
        self.ImageShell:SetActive(true)
        self.Image.SpriteName = "_LocBaoZhong"
        self.ImgLayoutElement.preferredWidth = 146
        self:PlayTextAnim()
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.INTERVENE then
        self.ImageShell:SetActive(true)
        self.Image.SpriteName = "_LocShanXian"
        self.ImgLayoutElement.preferredWidth = 146
        self:PlayTextAnim()
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.SP_EFF then
        local eff_cfg = BagData.Instance:GetEquipEffectCfg(data.num)
        if eff_cfg then
            self.Text.text = eff_cfg.name
        -- else
        --     self.Text.text = string.format("技能[%d]", data.num)
        end
        recycle_time = 1
    elseif data.type == BATTLE_HP_DESC.BUFF_SHOW then
        local buff_cfg = BattleData.BuffConfig(data.num)
        local buff_name = buff_cfg ~= nil and buff_cfg.buff_name or ""
        self.Text.text = buff_name
        recycle_time = 1
    elseif data.type == BATTLE_HP_DESC.ACTION_FAIL then
        self.FailTip:SetActive(true)
        if not StringIsEmpty(data.num2) then
            UH.SetText(self.FailTipTex2, Language.Main.Battle.FailedTip2)
            UH.SetText(self.FailTipTex, Language.Main.Battle.FailedTip2)
            PublicPopupCtrl.Instance:Center(data.num2)
        else
            UH.SetText(self.FailTipTex2, string.format(Language.Main.Battle.FailedTip, Language.Main.Battle.FailState[data.num]))
            UH.SetText(self.FailTipTex, string.format(Language.Main.Battle.FailedTip, Language.Main.Battle.FailState[data.num]))
        end
        recycle_time = 2
    elseif data.type == BATTLE_HP_DESC.HALLOW_GIFT then
        local eff_cfg = Cfg.ShengQiTianFuSkill(data.num)
        if eff_cfg then
            self.Text.text = eff_cfg.skill_name
        -- else
        --     self.Text.text = string.format("技能[%d]", data.num)
        end
        recycle_time = 1
    end

    if content ~= "" then        
        self.TextHandler:ShowContent(content)
        self:PlayTextAnim()
    end

    if recycle_time then
        TimeHelper:AddDelayTimer(
            function()
                self:RecycleSelf()
            end,
            recycle_time
        )
    end
end

function TestPopupHp:Info()
    LogError(self.info)
end

function TestPopupHp:OnEffectFinished()
    -- self.EffectNode.transform.localPosition = Vector3.zero
    self:RecycleSelf()
end

function TestPopupHp:PlayTextAnim()
    self.TextHandlerAnim:SetObjActive(true)
    self.TextHandlerAnim:Play()
end

function TestPopupHp:onRecycle()
    if self.pos_runner then
        Runner.Instance:RemoveRunUntilTrue(self.pos_runner)
        self.pos_runner = nil
    end

    self.Text.text = ""
    self.NumImg:SetObjActive(false)
    -- self.Image:SetObjActive(false)
    self.ImageShell:SetActive(false)
    self.TextHandler:OnRecycle()
    self.TextHandlerAnim:SetObjActive(false)
    self.Bubble:SetActive(false)
    self.FailTip:SetActive(false)
    if self.effect_handle then
        self.EffectTool:Stop(self.effect_handle)
    end
    self.Follow.enabled = false
    self.Follow.Target = nil
    --[[ if self.info.type == BATTLE_HP_DESC.BUFF_INVALID or self.info.type == BATTLE_HP_DESC.BUFF_IMMUNE or
    self.info.type == BATTLE_HP_DESC.MP_DECREASE then
        setPos(self.Objs, 0, 115)
    end ]]
    setPos(self.Objs, nil, 115)
    -- self.ImageObj:SetLocalPosition(Vector3.zero)
    -- self.TextObj:SetLocalPosition(Vector3.zero)
    data_set_frequency = {}
end
PopupItem = PopupItem or DeclareMono("PopupItem", BattlePopupBase)

function PopupItem:PopupItem()
    
end
function PopupItem:onCreate(pool)
    UIWObjPoolItem.onCreate(self, pool)
end
function PopupItem:setData(data)
    
end

BattleSceneRoleState = BattleSceneRoleState or DeclareMono("BattleSceneRoleState", UIWObjPoolItem)
---NameFollow========UIFollowTarget
---Name==============Text
---BaseFollow========UIFollowTarget
---HpFill================Slider
---Mp================Silier
---Lv================Text
---Ele1Set===========UIImageSpriteSet
---Ele1Img===========Image
---Ele2Set===========UIImageSpriteSet
---Ele2Img===========Image
---EleChaos==========GameObject
--Sandglass==========GameObject 正在等待输入指令的沙漏
---ClickerFollow=====UIFollowTarget
---IndicatorImg========UIImageSpriteSet
---IndicatorStateSprite=UISelectableSpriteSet
---CatchAble=========GameObject
---RestrainSp======UIImageSpriteSet
---HpShield========Slider
local ele_img_name = {
    [BattleAttriElement.Water] = "ShuiShuXing",
    [BattleAttriElement.Fire] = "HuoShuXing",
    [BattleAttriElement.Wind] = "FengShuXing",
    [BattleAttriElement.Earth] = "DiShuXing",
    [BattleAttriElement.Water..BattleAttriElement.Fire] = "ShuiHuo",
    [BattleAttriElement.Fire..BattleAttriElement.Water] = "ShuiHuo",
    [BattleAttriElement.Fire..BattleAttriElement.Wind] = "HuoFeng",
    [BattleAttriElement.Wind..BattleAttriElement.Fire] = "HuoFeng",
    [BattleAttriElement.Earth..BattleAttriElement.Wind] = "FengDi",
    [BattleAttriElement.Wind..BattleAttriElement.Earth] = "FengDi",
    [BattleAttriElement.Earth..BattleAttriElement.Water] = "DiShui",
    [BattleAttriElement.Water..BattleAttriElement.Earth] = "DiShui",
}

local ele_eff_unuse_name = {
    [BattleAttriElement.Water] = "10010897",
    [BattleAttriElement.Fire] = "100108105",
    [BattleAttriElement.Wind] = "10010898",
    [BattleAttriElement.Earth] = "100108103",
    [BattleAttriElement.Water..BattleAttriElement.Fire] = "100108102",
    [BattleAttriElement.Fire..BattleAttriElement.Water] = "100108102",
    [BattleAttriElement.Fire..BattleAttriElement.Wind] = "100108101",
    [BattleAttriElement.Wind..BattleAttriElement.Fire] = "100108101",
    [BattleAttriElement.Earth..BattleAttriElement.Wind] = "100108100",
    [BattleAttriElement.Wind..BattleAttriElement.Earth] = "100108100",
    [BattleAttriElement.Earth..BattleAttriElement.Water] = "10010899",
    [BattleAttriElement.Water..BattleAttriElement.Earth] = "10010899", 
    ["Boss"] = "100108104",
}

local ele_eff_unuse_name2 = {
    [BattleAttriElement.Water] = "100108113",
    [BattleAttriElement.Fire] = "100108112",
    [BattleAttriElement.Wind] = "100108114",
    [BattleAttriElement.Earth] = "100108119",
    [BattleAttriElement.Water..BattleAttriElement.Fire] = "100108118",
    [BattleAttriElement.Fire..BattleAttriElement.Water] = "100108118",
    [BattleAttriElement.Fire..BattleAttriElement.Wind] = "100108117",
    [BattleAttriElement.Wind..BattleAttriElement.Fire] = "100108117",
    [BattleAttriElement.Earth..BattleAttriElement.Wind] = "100108116",
    [BattleAttriElement.Wind..BattleAttriElement.Earth] = "100108116",
    [BattleAttriElement.Earth..BattleAttriElement.Water] = "100108115",
    [BattleAttriElement.Water..BattleAttriElement.Earth] = "100108115",
    ["Boss"] = "100108120",
}

local IndicatorSprite = {
    Tou = "ChunTouMing", --透明
    Huang = "XuanMuBiao_Huang",
    Lv = "XuanMuBiao_Lv",
    Hong = "XuanMuBiao_Hong",
    Lan = "XuanMuBiao_Lan"
}
local InvisibleSprite = {
    Tou = "ChunTouMing", --透明
    Huang = "YinShen_Huang",
    Lv = "YinShen_Lv",
    Hong = "YinShen_Hong",
    Lan = "YinShen_Lan"
}
local NewModeSprite = {
    Tou = "ChunTouMing", --透明
    Huang = "NewMode_Lv",
    Lv = "NewMode_Lv",
    Hong = "NewMode_Hong",
    Lan = "NewMode_Lan",
}
--血条等
function BattleSceneRoleState:BattleSceneRoleState()
    self.care_handles = {}
    self.ch_info = nil
    self.buff_effect_handles = {}
    self.buff_ui = {} --key:buff_key value:BattleSceneBuffItem
    self.runners = {}
end

local catch_type_to_effid = {
    10010293,
    10010294,
    10010295
}
local ToMySelf = {
    [12] = true,
    [13] = true,
    [14] = true,
    [15] = true,
    [17] = true,
    [18] = true,
}
local ToMyCamp = {
    [9] = true,
    [19] = true,
    [21] = true,
    [22] = true,
}
local ToEnemy = {
    [16] = true,
    [20] = true
}
---override
function BattleSceneRoleState:setData(data)
    self.ch_info = data
    self.UIBuffPool:RecycleActiveObjs()
    -- LogError("BattleSceneRoleState setData",data)
    if DEBUG_MODE then
        self.gameObject.name = data.name
    end
    if self.ch_info.fly_flag and self.ch_info.fly_flag > 0 then
        self.FlyBgSet:SetObjActive(true)
        self.NameGameObj:SetParent(self.FlyBgSet.gameObject)
        UH.AnchoredPosition(self.NameRect, Vector2.New(56, -19))
        UH.SpriteName(self.FlyBgSet, UIWFlyBg.RoleFlyS[self.ch_info.fly_flag])
    else
        self.NameGameObj:SetParent(self.NameObj)
        self.NameGameObj.transform:SetAsLastSibling()
        self.EffectShell:SetAsLastSibling()
        self.FlyBgSet:SetObjActive(false)
    end


    self.is_invisible = false
    self.is_boss = false
    self.is_chaos = false
    local ceid
    if self.ch_info.character_type == BATTLE_CH_TYPE.MONSTER then
        local cfg = Cfg.MonsterById(self.ch_info.character_id)
        if cfg then
            self.is_chaos = cfg.is_chaos == 1
            self.is_boss = cfg.is_boss == 1
        end
        if self.ch_info.level == 1 then
            if cfg.catch_pet_id ~= 0 then
                local pc = Cfg.PetById(cfg.catch_pet_id)
                if pc then
                    ceid = catch_type_to_effid[pc.catch_type]
                end
            else
                ceid = catch_type_to_effid[1]
            end
            self.catchable_handle = self.EffectTool:PlayByEffectId(ceid, self.CatchableEffectShell)
            self.CatchableEffectShell:SetActive(true)
        end
    end
    self.ceid = ceid
    if self.is_boss then
        --[[ self.LvBg:SetActive(false)
        self.LvBgBoss:SetActive(true) ]]
        self.HpRect.sizeDelta = Vector2.New(145, 16)
        self.HpFillRect.sizeDelta = Vector2.New(140, 12)
        self.HpDiffRect.sizeDelta = Vector2.New(140, 12)
        self.HpShieldRect.sizeDelta = Vector2.New(140, 12)
        self.FrozenShieldRect.sizeDelta = Vector2.New(141, 14)
        self.MpFillRect.sizeDelta = Vector2.New(140, 8)
        self.MpRect.sizeDelta = Vector2.New(142, 10)
        self.HpBGSet.SpriteName = "JingYingBOSSGuaiXueTiaoDi"
        self.HpDiffSet.SpriteName = "BOSSXueTiaoDiaoXueCeng"
        self.HpFillSet.SpriteName = "JingYingBOSSGuaiXueTiao"
        self.HpShieldSet.SpriteName = "JingYingBOSSGuaiXueTiaoBai"
        self.FrozenShieldSet.SpriteName = "BosSBingDongXueTiao"
        self.RestrainRT.anchoredPosition = Vector2.New(-80, 25)
    else
        --[[ if self.ch_info.character_type == BATTLE_CH_TYPE.PARTNER then
            self.LvBg:SetActive(false)
        else
            self.LvBg:SetActive(true)
        end
        self.LvBgBoss:SetActive(false) ]]
        self.HpRect.sizeDelta = Vector2.New(78, 10)
        self.HpFillRect.sizeDelta = Vector2.New(74, 8)
        self.HpDiffRect.sizeDelta = Vector2.New(74, 8)
        self.HpShieldRect.sizeDelta = Vector2.New(74, 8)
        self.FrozenShieldRect.sizeDelta = Vector2.New(73, 6)
        self.MpFillRect.sizeDelta = Vector2.New(76, 8)
        self.MpRect.sizeDelta = Vector2.New(78, 10)
        self.HpBGSet.SpriteName = "Nputongxuetiaodi"
        self.HpDiffSet.SpriteName = "Nputongxuetiaohuang"
        self.HpFillSet.SpriteName = "Nputongxuetiaohong"
        self.HpShieldSet.SpriteName = "Nputongxuetiaobai"
        self.FrozenShieldSet.SpriteName = "BingDongXueTiao"
        self.RestrainRT.anchoredPosition = Vector2.New(-55, 25)
    end
    self.LvBg:SetActive(false)
    self.LvBgBoss:SetActive(false)
    --[[ if ceid then
        self.LvBg:SetActive(false)
        self.LvBgBoss:SetActive(false)
    end ]]
    --[[ if FreedomCode.BattleUINew then ]]
        local hide_flag = BattleData.IsEnemyIdx(self.ch_info.grid_idx)
        --[[ if self:NeedShowHp() then
            hide_flag = false
        end ]]
        local mode_id = BattleData.Instance:BattleMode()
        if mode_id == BATTLE_MODE.BATTLE_MODE_GUILD_HONOR_BATTLE and 
        BattleData.Instance:BaseData().is_ob ~= 0 then
            hide_flag = true
        else
            hide_flag = BattleData.IsEnemyIdx(self.ch_info.grid_idx)
            if self:NeedShowHp() then
                hide_flag = false
            end
        end
        if GmData.Instance.view_data.hide_hp == true then
            hide_flag = true
        end
        --一般隐藏NewScheme 2 4 为血条和等级
        -- self.LvBg:SetActive(false)
        -- self.LvBgBoss:SetActive(false)
        self.NewScheme[2]:SetActive(not hide_flag)
        if self.ch_info.top_level and self.ch_info.top_level > 0 then
            UH.SetText(self.NewScheme[3], self.ch_info.top_level)
            self.PeakBg:SetActive(true)
        else
            UH.SetText(self.NewScheme[3], tostring(self.ch_info.level == -1 and "??" or self.ch_info.level))
            self.PeakBg:SetActive(false)
        end
        UH.AnchoredPosition(self.NewScheme[4].transform, self.is_boss and Vector2.New(135, -18) or Vector2.New(104,-18))
        self.NewScheme[4]:SetActive((not hide_flag)) --and self.ch_info.character_type ~= BATTLE_CH_TYPE.PARTNER))
        self.NewScheme[5]:SetObjActive((not hide_flag and 
        (self.ch_info.character_type == BATTLE_CH_TYPE.PET or self.ch_info.character_type == BATTLE_CH_TYPE.PET_SHADOW or
    self.ch_info.character_type == BATTLE_CH_TYPE.PARTNER or self.ch_info.character_type == BATTLE_CH_TYPE.PARTNER_ROBOT) and 
        self.ch_info.param ~= 0))

        self.NewScheme[5].SpriteName = "+" .. self.ch_info.param
    --[[ else
        self.Lv.text = tostring(self.ch_info.level == -1 and "??" or self.ch_info.level)
        self.LvBoss.text = tostring(self.ch_info.level == -1 and "??" or self.ch_info.level)
    end ]]
    self.NameObj:SetActive(true)
    self.EleObj:SetActive(true)
    self.MpObj:SetActive(self:NeedShowMp())
    self.SpObj:SetActive(self.ch_info.spirit_point and self.ch_info.spirit_point > -1)
    
    if self.ch_info.prof > 0 then
        if RoleData.ProfIdToProfType(self.ch_info.prof) == RoleProfessionType.QiGongShi 
        and not self.ch_info.temp_character_type 
        and (self.ch_info.character_type == BATTLE_CH_TYPE.ROLE or self.ch_info.character_type == BATTLE_CH_TYPE.ROLE_SHADOW) then
            self.RagePoint:SetObjActive(true)
            self.RagePoint:SetValue(8)
        else
            self.RagePoint:SetObjActive(false)
        end
    else
        self.RagePoint:SetObjActive(false)
    end
    self:FlushBase()
    if self.is_boss then
        self.Name.text = string.format("<color=#" .. COLORSTR.Red19 .. ">%s</color>", self.ch_info.name)
    elseif ceid then
        self.Name.text = string.format(Language.Common.PetName, self.ch_info.name)
    else
        if self.ch_info.name_color and self.ch_info.name_color == 1 then
            self.Name.text = string.format("<color=#" .. COLORSTR.Pink2 .. ">%s</color>", self.ch_info.name)
        else
            self.Name.text = tostring(self.ch_info.name)
        end
    end
    local battle_mode = BattleData.Instance:BattleMode()
    self.MainPetObj:SetActive(battle_mode == BATTLE_MODE.BATTLE_MODE_CLOUD_ARENA and (data.grid_idx == 3 or data.grid_idx == 8) and not data.is_player)
    -- .. "[" .. tostring(self.ch_info.grid_idx) .. "]"去掉名字后面的位置数字
    if FreedomCode.BattleUINew == false then
        if self.ch_info.character_type == BATTLE_CH_TYPE.PET or self.ch_info.character_type == BATTLE_CH_TYPE.PET_SHADOW then
            if self.ch_info.param == 0 then
                self.PetLevelSet:SetObjActive(false)
            else
                self.PetLevelSet:SetObjActive(true)
                self.PetLevelSet.SpriteName = "+" .. self.ch_info.param
            end
        end
    end

    self:FlushShow()

    if self.ch_info ~= nil then --registe
        self:careData(self.ch_info, BindTool.Bind(self.FlushTarget, self), "root_obj")
        self:careData(self.ch_info, BindTool.Bind(self.FlushHp, self), "maxhp", "curhp", "hp_shield")
        self:careData(self.ch_info, BindTool.Bind(self.FlushSp, self), "spirit_point")
        self:careData(self.ch_info, BindTool.Bind(self.FlushFrozenShield, self), "frozen_shield", "frozen_shield_full")
        self:careData(self.ch_info, BindTool.Bind(self.FlushElement, self), "element")
        self:careData(self.ch_info, BindTool.Bind(self.FlushStrategy, self), "strategy")
        self:careData(self.ch_info, BindTool.Bind(self.FlushSandglass, self), "curhp", "inputting")
        self:careData(self.ch_info, BindTool.Bind(self.FlushShow, self), "dont_show")
        self:careData(self.ch_info, BindTool.Bind(self.FlushBase, self), "dont_show_base")
        self:careData(self.ch_info, BindTool.Bind(self.FlushRage, self), "rage")
        self:careData(self.ch_info.buff_list, BindTool.Bind(self.FlushChBuff, self))
        self:careData(self.ch_info.ui_buff_list, BindTool.Bind(self.FlushBuff, self))
        self:careData(self.ch_info.attr_buff_list, BindTool.Bind(self.FlushAttrBuff, self))
        self:careData(self.ch_info, BindTool.Bind(self.FlushHpShow, self), "show_hp")
        if self:NeedShowMp() then
            self:careData(self.ch_info, BindTool.Bind(self.FlushMp, self), "maxmp", "curmp")
        end
    end
    local flush_indicator_act = BindTool.Bind(self.FlushIndicatorAcitve, self)
    self:careData(BattleData.Instance:BaseData(), flush_indicator_act, "round_phase", "auto")
    self:careData(BattleData.Instance:ViewInfo(), flush_indicator_act, "opened_sub", "select_strategy")

    self:careData(BattleData.Instance:BaseData(), BindTool.Bind(self.FlushIndicatorShow, self), "command_i", "sel_tar_type")
    self:careData(BattleData.Instance:BaseData(), BindTool.Bind(self.FlushSandglass, self), "command_i", "round_phase")
    self:careData(BattleData.Instance:BaseData(), BindTool.Bind(self.FlushTarget, self), "round_phase")
    self:careData(BattleData.Instance:BaseData(), BindTool.Bind(self.FlushRestrain, self), "command_i", "auto", "round_phase")
    self:careData(BattleData.Instance:BaseData(), BindTool.Bind(self.FlushIndicatorShow, self), "round_phase")

end
function BattleSceneRoleState:FlushFrozenShield()
    --刷新护盾显示
    self.ch_info.frozen_shield_full = self.ch_info.frozen_shield_full or 0
    if self.ch_info.frozen_shield_full == 0 then
        self.FrozenShield.value = 0
    else
        self.FrozenShield.value = self.ch_info.frozen_shield /self.ch_info.frozen_shield_full
    end
end
function BattleSceneRoleState:FlushHpShow()
    local hide_flag = BattleData.IsEnemyIdx(self.ch_info.grid_idx)
    if self:NeedShowHp() then
        return
    end
    if hide_flag then
        self.NewScheme[2]:SetActive(self.ch_info.show_hp == 1)
    end
end
function BattleSceneRoleState:FlushChBuff(old_val)
    if self.ch_info == nil then
        return
    end
    local buff_list = self.ch_info.buff_list
    local show_chaos = false
    local is_invisible = false
    for _, value in pairs(buff_list) do
        if type(value) == "number" and value == BattleHelper.AttrChange then
            show_chaos = true
        elseif type(value) == "table" and value.new_buff_id == BattleHelper.AttrChange then
            show_chaos = true
        end
        --[[ if type(value) == "number" and (value == BattleHelper.YinSheng or value == BattleHelper.YinSheng2) then
            is_invisible = true
        elseif type(value) == "table" and (value.new_buff_id == BattleHelper.YinSheng or value.new_buff_id == BattleHelper.YinSheng2) then
            is_invisible = true
        end ]]
    end
    self.is_invisible = BattleData.Instance:HasYinSheng(self.ch_info.grid_idx)
    local hide_chaos = false
    if old_val then
        for k,v in pairs(old_val) do
            if type(v) == "table" and v.new_buff_id and v.new_buff_id == BattleHelper.AttrChange then
                hide_chaos = true
            end
        end
    end
    if show_chaos and FreedomCode.BattleUINew == true then
        TimeHelper:CancelTimer(self.handle_delay)
        self.handle_delay = TimeHelper:AddDelayTimer(function ()
            self.NewScheme[1].SpriteName = "HunDunShuXing"
            self.EleChaos:SetActive(false)
            self.Ele1Set:SetObjActive(false)
            self.Ele2Set:SetObjActive(false)
        end,2)
    elseif hide_chaos then
        TimeHelper:CancelTimer(self.handle_delay)
        self:FlushElement()
    end
    local show_unuse = false
    for _, value in pairs(buff_list) do
        if type(value) == "number" and (value == 487 or value == 13) then
            show_unuse = true
        elseif type(value) == "table" and (value.new_buff_id == 487 or value.new_buff_id == 13) then
            show_unuse = true
        end
    end

    local temp = {}
    for _key, value in pairs(self.ch_info.element) do
        if value > 0 then
            table.insert(temp, {key = _key, val = value})
        end
    end

    local effect_id
    if self.is_chaos then 
        effect_id = ele_eff_unuse_name["Boss"]
    else 
        local effect_str = ""
        for k,v in pairs(temp) do 
            effect_str = effect_str .. v.key
        end 
        effect_id = ele_eff_unuse_name[tonumber(effect_str)] or ele_eff_unuse_name[tostring(effect_str)]
    end 

    self.EleObjs[2]:SetActive(show_unuse)
    if self.unuse_effect_handle2 then
        self.EffectTool:Stop(self.unuse_effect_handle2)
        self.unuse_effect_handle2 = nil
    end
    if show_unuse and effect_id ~= nil then
        self.unuse_effect_handle = self.EffectTool:PlayByEffectId(effect_id,self.EleObjs[2])
    elseif self.unuse_effect_handle then 
        self.EffectTool:Stop(self.unuse_effect_handle)
        self.unuse_effect_handle = nil
        if self.is_chaos then 
            effect_id = ele_eff_unuse_name2["Boss"]
        else
            local effect_str = ""
            for k,v in pairs(temp) do
                effect_str = effect_str .. v.key
            end
            effect_id = ele_eff_unuse_name2[tonumber(effect_str)] or ele_eff_unuse_name2[tostring(effect_str)]
        end
        if effect_id then
            self.unuse_effect_handle2 = self.EffectTool:PlayByEffectId(effect_id, self.EleObjs[1].gameObject)
        end
    end
end
function BattleSceneRoleState:FlushRage()
    self.RagePoint:SetShow(self.ch_info.rage or 0, true)
end
--刷新克制显示
function BattleSceneRoleState:FlushRestrain()
    local battle_data = BattleData.Instance
    local base_data = battle_data:BaseData()
    if battle_data:IsOB() > 0 or base_data.battle_mode == BATTLE_MODE.BATTLE_MODE_STORY 
    or base_data.battle_mode == BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE
    or base_data.battle_mode == BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY then
        self.RestrainSp:SetObjActive(false)
        return
    end
    if base_data.round_phase ~= BATTLE_ROUND_PHASE.Command then
        self.RestrainSp:SetObjActive(false)
        return
    end
    if self:NeedShowBase() == false or not BattleData.IsEnemyIdx(self.ch_info.grid_idx) then
        self.RestrainSp:SetObjActive(false)
        return
    end
    if base_data.auto == false then
        if battle_data:CurCmdIdx() == 0 then
            self.RestrainSp:SetObjActive(false)
            return
        end
        local cmd_ch = battle_data:GetChInfo(battle_data:CurCmdIdx())
        if cmd_ch == nil then
            --#1570984 LuaException
            return
        end
        local element1 = cmd_ch.element
        if base_data.command_i > 0 then
            if self.is_chaos == true then
                UH.SpriteName(self.RestrainSp, "HongSe")
                self.RestrainSp:SetObjActive(true)
                --[[ setPos(self.RestrainSp, -115, 5) ]]
            else
                self:CheckShowRestrain(element1, self.ch_info.element)
            end
        else
            self.RestrainSp:SetObjActive(false)
        end
    end
end
function BattleSceneRoleState:CheckShowRestrain(element1, element2)
    local temp = {}
    local red_arrow = true
    local green_arrow = true
    for key, value in pairs(element1) do
        if value > 0 then
            red_arrow = false
        end
        local num = 0
        for key2, value2 in pairs(element2) do
            if value2 > 0 then
                green_arrow = false
            end
            num = num + (ElementFactor[key][key2] * value2)
        end
        temp[key] = value * num
    end
    local res = 0
    for key, value in pairs(temp) do
        res = res + value
    end
    if red_arrow and green_arrow then
        self.RestrainSp:SetObjActive(false)
    else
        if res == 0 then
            if green_arrow == true or red_arrow == true then
                UH.SpriteName(self.RestrainSp, green_arrow and "LvSe" or HongSe)
                self.RestrainSp:SetObjActive(true)
            else
                self.RestrainSp:SetObjActive(false)
            end
        else
            UH.SpriteName(self.RestrainSp, res < 0 and "HongSe" or "LvSe")
            self.RestrainSp:SetObjActive(true)
        end
    end
    --[[ if self.is_boss == true then
        setPos(self.RestrainSp, -115, 5)
    else
        setPos(self.RestrainSp, -77, 5)
    end ]]
end

function BattleSceneRoleState:CalcBaseHeight(on_calc_done)
    if self.ch_info.character_type == BATTLE_CH_TYPE.ROLE or self.ch_info.character_type == BATTLE_CH_TYPE.ROLE_SHADOW then
        on_calc_done(3)
        return
    end
    if self.calc_base_height_runner then
        Runner.Instance:RemoveRunUntilTrue(self.calc_base_height_runner)
    end
    self.calc_base_height_runner =
        Runner.Instance:RunUntilTrue(
        function()
            local ch = BattleCtrl.Instance.displayer:getCh(self.ch_info.grid_idx)
            if not ch then
                on_calc_done(0)
                return true
            end
            if not ch.body_show:IsLoading() then
                local main_prop = ch.body_show.main_body:GetModelCameraPropertyCom()
                if not main_prop then
                    LogError(self.ch_info.name, "未烘焙模型数据")
                    on_calc_done(0)
                    return true
                end
                --[[ ch.root_node.transform.localScale.y * ]]
                local h = (main_prop.boundMax.y - main_prop.boundMin.y)
                local offset = h
                offset = h > 4 and 3.8 or offset
                offset = h <= 2 and 2 or offset
                on_calc_done(offset)
                return true
            end
        end,
        true
    )
    -- local h = ch:GetHeight()
    -- if self.ch_info.character_type == BATTLE_CH_TYPE.ROLE or self.ch_info.character_type == BATTLE_CH_TYPE.ROLE_SHADOW then
    --     return 0
    -- end
    -- local root_obj = self.ch_info.root_obj
    -- if not root_obj then
    --     return 0
    -- end
    -- local props = root_obj:GetComponentsInChildren(typeof(Game.ModelCameraProperty), false)
    -- if props.Length == 0 then
    --     -- 开发阶段有的资源不全
    --     -- LogError(self.ch_info.name)
    --     return 0
    -- end
    -- local max_y
    -- local min_y
    -- for i = 0, props.Length - 1 do
    --     local prop = props[i]
    --     if not max_y or props[i].boundMax.y > max_y then
    --         max_y = props[i].boundMax.y
    --     end
    --     if not min_y or props[i].boundMin.y < min_y then
    --         min_y = props[i].boundMin.y
    --     end
    -- end
    -- LogError(self.ch_info.name, root_obj.transform.localScale.y * (max_y - min_y))
    -- if root_obj.transform.localScale.y * (max_y - min_y) > 2 then
    --     return 0
    -- else
    --     return -1.1
    -- end
end

function BattleSceneRoleState:FlushShow()
    -- self:SetObjActive(not self.ch_info.dont_show)
end
function BattleSceneRoleState:FlushBase()
    if self.ch_info == nil then return end
    if self:NeedShowBase() and not self.ch_info.dont_show_base then
        -- LogError(self.ch_info.grid_idx, "FlushBase", true, follow_target)
        local follow_target = self.ch_info.root_obj ~= nil and self.ch_info.root_obj.transform or nil
        self.BaseFollow.Target = follow_target
        self.Base:SetActive(true)
        if self.ch_info.curhp <= 0 then
            self.ch_info.dont_show_base = true            
            self.Base:SetActive(false)
        end
    else
        -- LogError(self.ch_info.grid_idx, "FlushBase", false)
        self.BaseFollow.Target = nil
        self.Base:SetActive(false)
    end
end

function BattleSceneRoleState:NeedShowMp()
    if self.ch_info.maxmp > 0 then
        --[[ if BattleData.Instance:IsPVP() then ]]
            return true
        --[[ else
            return not BattleData.IsEnemyIdx(self.ch_info.grid_idx)
        end ]]
    else
        return false
    end
end

function BattleSceneRoleState:NeedShowHp()
    local group_id = BattleData.Instance:BaseData().monster_group_id
    local is_pvp = BattleData.Instance:IsPVP()
    if group_id == 1001 or group_id == 1000 or group_id == 1006 then
        return true
    elseif is_pvp then
        return false
    end
    local mode_id = BattleData.Instance:BattleMode()
    local info = Config.sundries_auto.hp_disappear[mode_id]
    if info and info.is_disappear == 1 then
        return true
    end
    info = Config.monster_group_auto.monster_group[group_id]
    if info and info.show_hp == 1 then
        return true
    end
    return false
end

function BattleSceneRoleState:NeedShowBase()
    --[[ if BattleData.IsEnemyIdx(self.ch_info.grid_idx) then
        local battle_mode = BattleData.Instance:BattleMode()
        if BattleData.Instance:IsPVP() or battle_mode == BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE or battle_mode == BATTLE_MODE.BATTLE_MODE_ARENA then
            return false
        end
    end ]]
    return true
end

function BattleSceneRoleState:careData(data, func, ...)
    self.care_handles[data:Care(func, ...)] = data
    func()
end

function BattleSceneRoleState:onRecycle()
    if self.effect_huiyan then
        self.EffectTool:Stop(self.effect_huiyan)
        self.effect_huiyan = nil
    end
    TimeHelper:CancelTimer(self.handle_delay)
    for handle, data in pairs(self.care_handles) do
        data:Uncare(handle)
    end
    self.care_handles = {}
    -- self.ch_info = nil
    if self.unuse_effect_handle then
        self.EffectTool:Stop(self.unuse_effect_handle)
        self.unuse_effect_handle = nil
    end
    if self.unuse_effect_handle2 then
        self.EffectTool:Stop(self.unuse_effect_handle2)
        self.unuse_effect_handle2 = nil
    end
    if self.hp_diff_timer then
        Runner.Instance:RemoveRunUntilTrue(self.hp_diff_timer)
        self.hp_diff_timer = nil
    end
    self.UIBuffPool:RecycleActiveObjs()
    --[[ for buff_key, ui in pairs(self.buff_ui) do
        self.UIBuffPool:Recycle(ui)
        self.buff_ui[buff_key] = nil
    end ]]
    self.buff_ui = {}

    self.NameFollow.Target = nil
    self.BaseFollow.Target = nil
    self.ClickerFollow.Target = nil
    self.strategyFollow.Target = nil
    self.BuffFollow.Target = nil

    self.PetLevelSet:SetObjActive(false)

    self.strategyBG:SetActive(false)
    self.AttrBuff:SetActive(false)
    self.CatchableEffectShell:SetActive(false)
    if self.catchable_handle then
        self.EffectTool:Stop(self.catchable_handle)
    end
    for k, v in pairs(self.runners) do
        Runner.Instance:RemoveRunUntilTrue(v)
    end
    self.runners = {}

    if self.calc_base_height_runner then
        Runner.Instance:RemoveRunUntilTrue(self.calc_base_height_runner)
        self.calc_base_height_runner = nil
    end
end

function BattleSceneRoleState:FlushTarget()
    if self.ch_info == nil then
        return
    end
    local follow_target = self.ch_info.root_obj ~= nil and self.ch_info.root_obj.transform or nil
    self:CalcBaseHeight(
        function(height)
            self.BaseFollow.Offset = Vector3.New(0, height, 0)--默认2.8
            self.BuffFollow.Offset = Vector3.New(0, height + 0.7, 0)--默认2.8 + 0.7
        end
    )
    self.NameObj:SetActive(true)
    self.NameFollow.Target = follow_target
    if BattleData.Instance:BaseData().round_phase == BATTLE_ROUND_PHASE.Client then
        --[[ self.NameFollow.Target = nil
        self.NameFollow.enabled = false ]]
        -- self.EleObj:SetActive(false)
        --[[ if BattleData.IsEnemyIdx(self.ch_info.grid_idx) and self.is_boss == false and self.ceid == nil then
            self.NameObj:SetActive(false)
        end ]]
        self:FlushBase()
        self.ClickerFollow.Target = follow_target
        self.BuffFollow.Target = follow_target
    else
        self:FlushBase()
        self.EleObj:SetActive(true)
        self.ClickerFollow.Target = follow_target
        self.BuffFollow.Target = follow_target
        if follow_target ~= nil then
            self:FlushShow()
        end
    end
end

function BattleSceneRoleState:FlushElement()
    if self.is_chaos then
        if FreedomCode.BattleUINew == true then
            self.NewScheme[1].SpriteName = "HunDunShuXing"
            self.EleChaos:SetActive(false)
            self.Ele1Set:SetObjActive(false)
            self.Ele2Set:SetObjActive(false)
            return
        end
        self.EleChaos:SetActive(true)
        self.Ele1Set:SetObjActive(false)
        self.Ele2Set:SetObjActive(false)
        return
    else
        self.EleChaos:SetActive(false)
    end

    local order = {
        BattleAttriElement.Earth,
        BattleAttriElement.Water,
        BattleAttriElement.Fire,
        BattleAttriElement.Wind
    }

    local temp = {}

    -- 地水火风依次排列，但如果是风地属性，风在地前面，一个循环
    -- for i = 1, 4 do
    --     local key = order[i]
    --     local val = self.ch_info.element[key]
    --     if val > 0 then
    --         table.insert(temp, {key = key, val = val})
    --         for j = 1, 2 do
    --             local key = order[(i + j) - math.floor((i + j - 1) / 4) * 4]
    --             local val = self.ch_info.element[key]
    --             if val > 0 then
    --                 table.insert(temp, {key = key, val = val})
    --                 break
    --             end
    --         end
    --         break
    --     end
    -- end

    for _, key in ipairs(order) do
        local val = self.ch_info.element[key]
        if val > 0 then
            table.insert(temp, {key = key, val = val})
        end
    end

    if temp[1] and temp[2] and temp[1].key == BattleAttriElement.Earth and temp[2].key == BattleAttriElement.Wind then
        local t = temp[1]
        temp[1] = temp[2]
        temp[2] = t
    end

    if FreedomCode.BattleUINew then
        if #temp == 0 then
            self.NewScheme[1]:SetObjActive(false)
        else
            self.NewScheme[1]:SetObjActive(true)
            if #temp == 1 then
                self.NewScheme[1].SpriteName = ele_img_name[temp[1].key]
            else
                self.NewScheme[1].SpriteName = ele_img_name[temp[1].key .. temp[2].key]
            end
        end
        return
    end

    if #temp == 0 then
        self.Ele1Set:SetObjActive(false)
        self.Ele2Set:SetObjActive(false)
    else
        --修改为一个显示
        local ele_obj = self.is_boss and self.Ele1Set or self.Ele2Set
        local ele_obj2 = self.is_boss and self.Ele2Set or self.Ele1Set
        ele_obj:SetObjActive(true)
        ele_obj2:SetObjActive(false)
        if #temp == 1 then
            ele_obj.SpriteName = ele_img_name[temp[1].key]
        else
            ele_obj.SpriteName = ele_img_name[temp[1].key .. temp[2].key]
        end

        --[[ self.Ele1Set:SetObjActive(true)
        self.Ele1Set.SpriteName = ele_img_name[temp[1].key]
        self.Ele1Text.text = temp[1].val
        if #temp == 1 then
            -- setPos(self.Ele1Set, 0, 0)
            self.Ele2Set:SetObjActive(false)
        else
            -- setPos(self.Ele1Set, -16, 0)
            -- setPos(self.Ele2Set, 16, 0)
            self.Ele2Set.SpriteName = ele_img_name[temp[2].key]
            self.Ele2Text.text = temp[2].val
            self.Ele2Set:SetObjActive(true)
        end ]]
    end
end

function BattleSceneRoleState:FlushHp(pre_kv)
    if self.ch_info == nil then return end
    if self.ch_info.curhp <= 0 then
        -- self.NameObj:SetActive(false)
        self.EleObj:SetActive(false)
    end
    local value
    local shield_val = 0
    if self.ch_info.maxhp == 0 then
        value = 1
    elseif self.ch_info.curhp <= 0 then
        value = 0
    else
        self.ch_info.hp_shield = self.ch_info.hp_shield or 0
        if self.ch_info.hp_shield <= 0 then      --没有护盾
            value = self.ch_info.curhp / self.ch_info.maxhp
        else
            if self.ch_info.hp_shield + self.ch_info.curhp >= self.ch_info.maxhp then
                shield_val = 1
                value = self.ch_info.curhp / (self.ch_info.hp_shield + self.ch_info.curhp)
            else
                shield_val = (self.ch_info.hp_shield + self.ch_info.curhp) / self.ch_info.maxhp
                value = self.ch_info.curhp / self.ch_info.maxhp
            end
        end
    end
    self.HpShield.value = shield_val
--    LogDG(self.ch_info.grid_idx, value)
    self.HpFill.value = value
    if pre_kv and pre_kv.curhp and pre_kv.curhp > self.ch_info.curhp and self.ch_info.curhp > 0 then
        if self.hp_diff_timer then
            Runner.Instance:RemoveRunUntilTrue(self.hp_diff_timer)
        end

        local start_time = Time.time
        local start_val = self.HpDiff.value
        self.hp_diff_timer =
            Runner.Instance:RunUntilTrue(
            function()
                local time = Time.time - start_time
                if time < 0.6 then
                    self.HpDiff.value = start_val
                elseif time < 0.8 then
                    self.HpDiff.value = (value - start_val) / 0.2 * (time - 0.6) + start_val
                else
                    self.HpDiff.value = value
                    if value <= 0 then
                        self.ch_info.dont_show_base = true
                    else
                        self.ch_info.dont_show_base = false
                    end
                    return true
                end
            end
        )
    else
        self.HpDiff.value = value
        if value <= 0 then
            self.ch_info.dont_show_base = true
            if self.hp_diff_timer then
                Runner.Instance:RemoveRunUntilTrue(self.hp_diff_timer)
                self.hp_diff_timer = nil
            end
        else
            self.ch_info.dont_show_base = false
        end
    end
end

function BattleSceneRoleState:FlushMp()
    self.Mp.value = self.ch_info.maxmp ~= 0 and self.ch_info.curmp / self.ch_info.maxmp or 0
end

function BattleSceneRoleState:FlushSp()
    self.SpObj:SetActive(false)
    --[[ if self.ch_info.spirit_point > -1 then
        self.SpObj:SetActive(true)
        local fill = self.ch_info.spirit_point / FightData.FightOtherDate().max_sp
        fill = fill > 1 and 1 or fill
        self.SpFill.value = fill
    else
        self.SpObj:SetActive(false)
    end ]]
end

function BattleSceneRoleState:isSelTargetShow()
    local base_data = BattleData.Instance:BaseData()
    return (base_data.auto == false and base_data.round_phase == BATTLE_ROUND_PHASE.Command and BattleData.Instance:ViewInfo().opened_sub == BattleSubView.None) or
    BattleData.Instance:ViewInfo().select_strategy == true
end

function BattleSceneRoleState:OnClick()
    if self:isSelTargetShow() == true then
        
        local battle_data = BattleData.Instance
        local base_data = battle_data:BaseData()
        if RoleSettingData.IsNewMode() then
            if base_data.sel_tar_type == BATTLE_MOVE_TYPE.NONE or (base_data.sel_tar_type == BATTLE_MOVE_TYPE.PERFORM_SKILL
            and base_data.sel_tar_param1 ~= FlyUpData.Instance:GetSkillId()) then
                PublicPopupCtrl.Instance:Center(Language.Main.Battle.NeedDrag)
                return
            end
        end
        local sp_na
        if RoleSettingData.IsNewMode() then
            sp_na = string.sub(self.IndicatorStateSprite.PressedSprite, 1, #NewModeSprite.Lan)
        else
            if self.is_invisible then
                sp_na = string.sub(self.IndicatorStateSprite.PressedSprite, 1, #InvisibleSprite.Lan)
            else
                sp_na = string.sub(self.IndicatorStateSprite.PressedSprite, 1, #IndicatorSprite.Lan)
            end
        end
        if sp_na ~= IndicatorSprite.Lan and sp_na ~= InvisibleSprite.Lan
        and sp_na ~= NewModeSprite.Lan
        then
            if base_data.sel_tar_type == BATTLE_MOVE_TYPE.NONE and battle_data:CurCmdIdx() == self.ch_info.grid_idx then
                return
            end
            if base_data.sel_tar_type == BATTLE_MOVE_TYPE.SEAL_PET then
                local monster_cfg = Cfg.MonsterById(self.ch_info.character_id)
                if monster_cfg then
                    local pet_cfg = Cfg.PetById(monster_cfg.catch_pet_id)
                    if pet_cfg and base_data.sel_tar_param1 + 1 < pet_cfg.catch_type then
                        PublicPopupCtrl.Instance:Center(ErrorText[55])
                        return
                    end
                end
                local num = PetData.Instance:GetBagRemainNum()
                if num == 0 then
                    PublicPopupCtrl.Instance:Center(ErrorText[160])
                    return
                end
            end
            local displayer = BattleCtrl.Instance.displayer
            local ch = displayer:getCh(battle_data:CurCmdIdx())
            if ch ~= nil then
                ch:CmdErrorCheck()
            end
            BattleCtrl.Instance:ExitSelectTargetState(self.ch_info.grid_idx)
        else
            if base_data.sel_tar_type == BATTLE_MOVE_TYPE.USE_FABAO then
                if MainViewBattleFabaoItem.fabao_id == 10 then
                    PublicPopupCtrl.Instance:Center(ErrorText[162])
                elseif MainViewBattleFabaoItem.fabao_id == 11 then
                    PublicPopupCtrl.Instance:Center(ErrorText[165])
                elseif ToMyCamp[MainViewBattleFabaoItem.fabao_id] then
                    PublicPopupCtrl.Instance:Center(ErrorText[167])
                elseif ToEnemy[MainViewBattleFabaoItem.fabao_id] then
                    PublicPopupCtrl.Instance:Center(ErrorText[166])
                elseif ToMySelf[MainViewBattleFabaoItem.fabao_id] then
                    PublicPopupCtrl.Instance:Center(ErrorText[164])
                end
            elseif base_data.sel_tar_type == BATTLE_MOVE_TYPE.HEART_SKILL then
                local skill_id = base_data.sel_tar_param1
                local skill_cfg = SkillData.GetConfig(skill_id, base_data.sel_tar_param2)
                if skill_cfg.is_target_dead == 1 then
                    PublicPopupCtrl.Instance:Center(ErrorText[186])
                elseif skill_id == 3004 then
                    PublicPopupCtrl.Instance:Center(ErrorText[188])
                else
                    PublicPopupCtrl.Instance:Center(ErrorText[187])
                end
            elseif base_data.sel_tar_type ~= BATTLE_MOVE_TYPE.STRATEGY then
                PublicPopupCtrl.Instance:Center(ErrorText[141])
            end
        end
        BattleData.Instance:ViewInfo().select_strategy = false
    end
end
--展示详细信息
function BattleSceneRoleState:OnClickDetail()
    if BattleData.IsStory() then
        return
    end
    local base_data = BattleData.Instance:BaseData()
    local displayer = BattleCtrl.Instance.displayer
    if self:NeedShowBase() and base_data.round_phase == BATTLE_ROUND_PHASE.Client then
        local ch = displayer:getCh(self.ch_info.grid_idx)
        ViewMgr:OpenView(BattleChdetail, {obj = ch, info = self.ch_info})
    end
end

function BattleSceneRoleState:OnPressDown()
    -- LogDG("BattleSceneRoleState:OnPressDown()")
    self:ShowSkillRange()
end

function BattleSceneRoleState:OnPressUp()
    -- LogDG("BattleSceneRoleState:OnPressUp()")
    self:FlushSkillRangeShow()
end

function BattleSceneRoleState:OnPointerExit()
    local base_data = BattleData.Instance:BaseData()
    local view_data = BattleData.Instance:ViewInfo()
    --[[ LogDG(view_data.skill_sel_type)
    if RoleSettingData.IsNewMode() and view_data.skill_sel_type ~= 1 then
        return
    end ]]
    if RoleSettingData.IsNewMode() and (base_data.sel_tar_type == BATTLE_MOVE_TYPE.NORMAL_ATTACK
    or base_data.sel_tar_type == BATTLE_MOVE_TYPE.PERFORM_SKILL 
    or base_data.sel_tar_type == BATTLE_MOVE_TYPE.HEART_SKILL 
    or base_data.sel_tar_type == BATTLE_MOVE_TYPE.CARD_SKILL
    or base_data.sel_tar_type == BATTLE_MOVE_TYPE.HUANSHOU_SKILL) then
        self:FlushIndicatorShow()
        BattleSceneRoleState.PointerGridIdx = nil
    end
    self:FlushSkillRangeShow()
end
function BattleSceneRoleState:OnPointerEnter()
    local base_data = BattleData.Instance:BaseData()
    local view_data = BattleData.Instance:ViewInfo()
    --[[ LogDG(view_data.skill_sel_type)
    if RoleSettingData.IsNewMode() and view_data.skill_sel_type ~= 1 then
        return
    end ]]
    if RoleSettingData.IsNewMode() and (base_data.sel_tar_type == BATTLE_MOVE_TYPE.NORMAL_ATTACK
    or base_data.sel_tar_type == BATTLE_MOVE_TYPE.PERFORM_SKILL 
    or base_data.sel_tar_type == BATTLE_MOVE_TYPE.HEART_SKILL 
    or base_data.sel_tar_type == BATTLE_MOVE_TYPE.CARD_SKILL
    or base_data.sel_tar_type == BATTLE_MOVE_TYPE.HUANSHOU_SKILL) then
        if self.IndicatorImg.SpriteName == "NewMode_Lv(Clone)" then
            BattleSceneRoleState.PointerGridIdx = self.ch_info.grid_idx
            self:SetIndicatorImg(NewModeSprite.Lan)
        else
            if BattleData.IsEnemyIdx(self.ch_info.grid_idx) then
                BattleSceneRoleState.PointerGridIdx = -1
                self:SetIndicatorImg(NewModeSprite.Hong)
            else
                BattleSceneRoleState.PointerGridIdx = self.ch_info.grid_idx
            end
        end
    end
    if RoleSettingData.IsNewMode() then
        self:ShowSkillRange()
    end
end

function BattleSceneRoleState:ShowSkillRange()
    local base_data = BattleData.Instance:BaseData()
    if base_data.sel_tar_type ~= BATTLE_MOVE_TYPE.PERFORM_SKILL and
    base_data.sel_tar_type ~= BATTLE_MOVE_TYPE.HEART_SKILL
    and base_data.sel_tar_type ~= BATTLE_MOVE_TYPE.CARD_SKILL
    and base_data.sel_tar_type ~= BATTLE_MOVE_TYPE.HUANSHOU_SKILL  then ---normal attack
        -- LogError("Return", base_data.sel_tar_type)
        return
    end
    local skill_id = base_data.sel_tar_param1
    local skill_cfg = SkillData.GetConfig(skill_id, base_data.sel_tar_param2)
    local grid_idx = self.ch_info.grid_idx
    local target_side_type = skill_cfg.target_side_type
    if (target_side_type == 1 and BattleData.IsEnemyIdx(grid_idx)) or (target_side_type == 2 and (not BattleData.IsEnemyIdx(grid_idx))) then
        return
    end
    
    local skill_type = SkillData.GetSkillType(skill_id)
    -- LogErrorHL("ShowSkillRange", skill_cfg)
    self.skill_range_show_list = {}

    if skill_type == SKILL_TYPE.POWER then
        -- T字形
        for k, v in pairs(self.pool:GetActiveObjs()) do
            if k ~= self and BattleData.PowerSkillJudge(grid_idx, k.ch_info.grid_idx) then
                table.insert(self.skill_range_show_list, k)
            end
        end
    elseif skill_type == SKILL_TYPE.SUPER_POWER then
        -- 同阵营全覆盖
        for k, v in pairs(self.pool:GetActiveObjs()) do
            if k ~= self and BattleData.AreInSameSide(grid_idx, k.ch_info.grid_idx) then
                table.insert(self.skill_range_show_list, k)
            end
        end
    elseif skill_type == SKILL_TYPE.LINE then
        for k, v in pairs(self.pool:GetActiveObjs()) do
            if skill_cfg.line_type == 1 then
                -- 同一行全覆盖
                if k ~= self and BattleData.AreInSameRow(grid_idx, k.ch_info.grid_idx) then
                    table.insert(self.skill_range_show_list, k)
                end
            else
                -- 同一排全覆盖
                if k ~= self and BattleData.AreInSameCol(grid_idx, k.ch_info.grid_idx) then
                    table.insert(self.skill_range_show_list, k)
                end
            end
        end
    end
    self:FlushSkillRangeShow(true)
end

function BattleSceneRoleState:FlushSkillRangeShow(is_show)
    if not self.skill_range_show_list or not next(self.skill_range_show_list) then
        return
    end
    -- local spname = string.gsub(self.Button.image.overrideSprite.name, "%(Clone%)", "")
    if RoleSettingData.IsNewMode() then
        local spname = string.gsub(self.IndicatorImg.SpriteName, "%(Clone%)", "")
        for k, v in pairs(self.skill_range_show_list) do
            -- v.IndicatorImg.SpriteName = spname
            v:SetIndicatorImg(spname)
        end
    else
        local spname = is_show and string.gsub(self.IndicatorStateSprite.PressedSprite, "%(Clone%)", "") or string.gsub(self.IndicatorImg.SpriteName, "%(Clone%)", "")
        for k, v in pairs(self.skill_range_show_list) do
            if not v.is_invisible then
                -- v.IndicatorImg.SpriteName = self:ChangeIndicator(spname)
                v:SetIndicatorImg(self:ChangeIndicator(spname))
            else
                -- v.IndicatorImg.SpriteName = spname
                v:SetIndicatorImg(spname)
            end
        end
    end
end

--刷新圈圈的点击效果
function BattleSceneRoleState:FlushIndicatorShow()
    if self.ch_info == nil then
        return
    end
    local battle_data = BattleData.Instance
    local base_data = battle_data:BaseData()
    local sel_able = true
    local self_idx = self.ch_info.grid_idx
    local self_is_Enemy = BattleData.IsEnemyIdx(self_idx)
    local target_side_type = 0 ---0:全部可选(显示敌方圈圈),1:友方(显示友方圈圈),2:敌方(显示敌方圈圈)
    if battle_data:HasYinSheng(self_idx) and battle_data:HasHuiYan() and 
        self.is_invisible and base_data.round_phase == BATTLE_ROUND_PHASE.Command then
        if self.effect_huiyan then
            self.EffectTool:Stop(self.effect_huiyan)
        end
        self.effect_huiyan = self.EffectTool:PlayByEffectId("5031003", self.IndicatorImg.gameObject, false)
    else
        if self.effect_huiyan then
            self.EffectTool:Stop(self.effect_huiyan)
            self.effect_huiyan = nil
        end
    end
    if
        base_data.sel_tar_type == BATTLE_MOVE_TYPE.PERFORM_SKILL or base_data.sel_tar_type == BATTLE_MOVE_TYPE.NORMAL_ATTACK or base_data.sel_tar_type == BATTLE_MOVE_TYPE.USE_FABAO or
            base_data.sel_tar_type == BATTLE_MOVE_TYPE.NONE or base_data.sel_tar_type == BATTLE_MOVE_TYPE.HEART_SKILL or base_data.sel_tar_type == BATTLE_MOVE_TYPE.CARD_SKILL or base_data.sel_tar_type == BATTLE_MOVE_TYPE.HUANSHOU_SKILL
     then
        local cur_cmd_idx = battle_data:CurCmdIdx()
        local judge_dis_rule = false --是否要判断近战规则(主角且武器为近战武器[不为弓]时)
        local is_target_dead = false ---如果角色已死是否可以被选择
        local self_target_flag = 0 --0.不能以自己为目标;1.可以以自己为目标;2.强制以自己为目标
        local is_target_pet = false

        if base_data.sel_tar_type == BATTLE_MOVE_TYPE.PERFORM_SKILL or base_data.sel_tar_type == BATTLE_MOVE_TYPE.HEART_SKILL or base_data.sel_tar_type == BATTLE_MOVE_TYPE.CARD_SKILL or base_data.sel_tar_type == BATTLE_MOVE_TYPE.HUANSHOU_SKILL then
            local skill_id = base_data.sel_tar_param1
            local skill_cfg = SkillData.GetConfig(skill_id, base_data.sel_tar_param2)
            judge_dis_rule = (skill_cfg.is_ignore_distance_rule == 0)
            target_side_type = skill_cfg.target_side_type
            -- is_target_dead = (skill_cfg.is_target_dead ~= 0)
            is_target_dead = skill_cfg.is_target_dead
            --0活1死2都可
            self_target_flag = skill_cfg.is_target_self
            if skill_id == 3004 or skill_id == 1504 then--后面有空用表判断
                is_target_pet = true
            end
        else
            if self.cur_cmd_idx == battle_data:PlayerIdx() then
                local player_ch = battle_data:GetChInfo(self.cur_cmd_idx)
                judge_dis_rule = (player_ch == nil or player_ch.weapon_type ~= WEAPON_TYPE.BOW)
            else
                judge_dis_rule = true
            end
            target_side_type = 0
            is_target_dead = 0
            self_target_flag = 0
        end

        -- LogErrorHL("BeingSeal?",base_data.last_cmd_type, base_data.last_cmd_p1, self_idx)
        local seal_flag = base_data.last_cmd_type == BATTLE_MOVE_TYPE.SEAL_PET or base_data.last_cmd_type == BATTLE_MOVE_TYPE.SEAL_YOUMING
        if seal_flag and base_data.last_cmd_p1 and self_idx - 1 == base_data.last_cmd_p1 then
            sel_able = false
        end

        if is_target_dead == 1 then
            sel_able = not battle_data:IsChAlive(self_idx)
        elseif is_target_dead == 0 then
            sel_able = battle_data:IsChAlive(self_idx)
        else
            sel_able = true
        end
        
        if sel_able then
            if judge_dis_rule and not battle_data:IsFitDisRule(cur_cmd_idx, self_idx) then
                sel_able = false
            elseif target_side_type == 1 and self_is_Enemy then
                sel_able = false
            elseif target_side_type == 2 and not self_is_Enemy then
                sel_able = false
            elseif self_target_flag == 2 then
                sel_able = self_idx == cur_cmd_idx
            elseif self_target_flag == 0 then
                sel_able = self_idx ~= cur_cmd_idx
            end
        end
        if is_target_pet then
            sel_able = (self.ch_info.character_type == BATTLE_CH_TYPE.PET or self.ch_info.character_type == BATTLE_CH_TYPE.PET_SHADOW)
        end
        if base_data.sel_tar_type == BATTLE_MOVE_TYPE.USE_FABAO then
            --使用法宝的时候要判断
            if ToMySelf[MainViewBattleFabaoItem.fabao_id] then
                sel_able = self_idx == cur_cmd_idx
            --[[ elseif MainViewBattleFabaoItem.fabao_id == 16 then
                sel_able = not (self_idx == cur_cmd_idx) ]]
            elseif MainViewBattleFabaoItem.fabao_id == 11 then
                --对敌方宠物
                if self_is_Enemy and (self.ch_info.character_type == 1 or self.ch_info.character_type == 5) then
                    sel_able = true
                else
                    sel_able = false
                end
            elseif MainViewBattleFabaoItem.fabao_id == 10 then
                --对敌方角色
                if self_is_Enemy and (self.ch_info.character_type == 0 or self.ch_info.character_type == 4) then
                    sel_able = true
                else
                    sel_able = false
                end
            elseif ToEnemy[MainViewBattleFabaoItem.fabao_id] then--16 20 
                --对敌方全体
                if self_is_Enemy then
                    sel_able = true
                else
                    sel_able = false
                end
            elseif ToMyCamp[MainViewBattleFabaoItem.fabao_id] then--9 19 21 22
                --对我方全体
                if self_is_Enemy then
                    sel_able = false
                else
                    sel_able = true
                end
            end
        end
    elseif base_data.sel_tar_type == BATTLE_MOVE_TYPE.USE_ITEM or base_data.sel_tar_type == BATTLE_MOVE_TYPE.USE_FAKE_POTION then
        target_side_type = 1
        if self_is_Enemy then
            sel_able = false
        else
            sel_able = false
            if MainViewBattleItemSel.item_id then
                if battle_data:IsChAlive(self_idx) then
                    sel_able = true
                    if MainViewBattleItemSel.item_id == CommonItemId.XiangYaoBian then
                        sel_able = false
                    end
                else
                    sel_able = true
                    --[[ if MainViewBattleItemSel.item_id == CommonItemId.JiuZhuanHuiHun then
                        sel_able = true
                    end ]]
                end
            else
                if battle_data:IsChAlive(self_idx) then
                    sel_able = true
                end
            end
        end
    elseif base_data.sel_tar_type == BATTLE_MOVE_TYPE.STRATEGY then
        -- end
        -- if self_is_Enemy then
        sel_able = true
    elseif base_data.sel_tar_type == BATTLE_MOVE_TYPE.SEAL_PET or base_data.sel_tar_type == BATTLE_MOVE_TYPE.SEAL_YOUMING then
        if self.ch_info.level ~= 1 or not BattleData.IsEnemyIdx(self.ch_info.grid_idx) then
            sel_able = false
        else
            sel_able = true
        end
    else
        sel_able = false
    end

    if BattleData.Instance:IsOB() ~= 0 then
        sel_able = false
    end

    if base_data.round_phase ~= BATTLE_ROUND_PHASE.Command and base_data.sel_tar_type ~= BATTLE_MOVE_TYPE.STRATEGY then
        sel_able = false
    end
    if RoleSettingData.IsNewMode() and base_data.sel_tar_type == BATTLE_MOVE_TYPE.NONE then
        sel_able = false
    end
    local normal_sp, pressed_sp
    if sel_able then
        pressed_sp = IndicatorSprite.Hong
        if base_data.sel_tar_type ~= BATTLE_MOVE_TYPE.STRATEGY and target_side_type == 0 and not self_is_Enemy then
            normal_sp = IndicatorSprite.Tou
        else
            normal_sp = self_is_Enemy and IndicatorSprite.Huang or IndicatorSprite.Lv
        end
    else
        normal_sp = IndicatorSprite.Tou
        pressed_sp = IndicatorSprite.Lan
        if base_data.sel_tar_type == BATTLE_MOVE_TYPE.USE_ITEM and MainViewBattleItemSel.item_id == CommonItemId.XiangYaoBian then
            normal_sp = self_is_Enemy and IndicatorSprite.Huang or IndicatorSprite.Tou
            pressed_sp = IndicatorSprite.Hong
        end
    end
    if RoleSettingData.IsNewMode() then
        normal_sp = self:ChangeNewMode(normal_sp)
        pressed_sp = self:ChangeNewMode(pressed_sp)
    else
        if self.is_invisible then
            normal_sp = self:ChangeInvisible(normal_sp)
            pressed_sp = self:ChangeInvisible(pressed_sp)
        end
    end
    -- self.IndicatorImg.SpriteName = normal_sp
    -- self.IndicatorStateSprite.PressedSprite = pressed_sp
    self:SetIndicatorImg(normal_sp)
    self:SetPressedSprite(pressed_sp)
    -- LogErrorHL("FlushIndicatorShow")
end
function BattleSceneRoleState:SetIndicatorImg(sp_name)
    self.IndicatorImg.SpriteName = sp_name
    self.TanHao:SetActive(sp_name == NewModeSprite.Hong)
end
function BattleSceneRoleState:SetPressedSprite(pressed_sp)
    self.IndicatorStateSprite.PressedSprite = pressed_sp
    -- self.TanHao:SetActive(pressed_sp == NewModeSprite.Hong)
end
function BattleSceneRoleState:ChangeInvisible(sp_name)
    for key, value in pairs(IndicatorSprite) do
        if value == sp_name then
            return InvisibleSprite[key]
        end
    end
    return sp_name
end
--黄可选 蓝是按下不可选 红是可选
function BattleSceneRoleState:ChangeIndicator(sp_name)
    for key, value in pairs(InvisibleSprite) do
        if value == sp_name then
            return IndicatorSprite[key]
        end
    end
    return sp_name
end
--绿可选 蓝选中 红不可选
function BattleSceneRoleState:ChangeNewMode(sp_name)
    if RoleSettingData.IsNewMode() then
        for key, value in pairs(IndicatorSprite) do
            if value == sp_name then
                return NewModeSprite[key]
            end
        end
        return sp_name
    end
end

function BattleSceneRoleState:FlushStrategy()
    if self.ch_info.strategy then
        self.strategyFollow.Target = self.ch_info.root_obj ~= nil and self.ch_info.root_obj.transform or nil
        local offset = self.BaseFollow.Offset
        offset.y = offset.y - 0.5
        self.strategyFollow.Offset = offset
        -- LogDG(self.ch_info.name, self.ch_info.strategy)
        local str = ""
        local strategy = self.ch_info.strategy
        if strategy.strategy_type and strategy.strategy_type > 0 then
            if TeamData.Instance:InTeam() and strategy.name == TeamData.Instance:GetLeaderInfo().name then
                str = Language.Main.Battle.BaseStrategy .. Language.Main.Battle.Strategy[strategy.strategy_type]            
            else
                str = Language.Main.Battle.Strategy[strategy.strategy_type]            
            end
        end
        if strategy.strategy_type and strategy.strategy_type == 0 and strategy.istr then
            str = strategy.istr
        end
        if TeamData.Instance:InTeam() then 
            UH.SpriteName(self.StrategySp, strategy.name == TeamData.Instance:GetLeaderInfo().name and "NeiRongKuang3" or "NeiRongKuang1" )
        else 
            UH.SpriteName(self.StrategySp, "NeiRongKuang3")
        end 
        if str == "" then
            self.strategyFollow.Target = nil
            self.strategyBG:SetActive(false)
        else
            UH.SetText(self.strategyTXT, str)
            UH.SetText(self.strategyName, strategy.name or "")
            self.strategyBG:SetActive(true)
        end
    else
        self.strategyFollow.Target = nil
        self.strategyBG:SetActive(false)
    end
end

-- BattleUIBuff = {
--     [6] = {eff_id = 10080601, sp_name = "_locHunLuan"},
--     [2] = {eff_id = 10080602, sp_name = "_locZhongDu"},
--     [3] = {eff_id = 10080603, sp_name = "_locShiHua"},
--     [5] = {eff_id = 10080604, sp_name = "_locJiuZui"},
--     [4] = {eff_id = 10080605, sp_name = "_locHunShui"},
--     [7] = {eff_id = 10080615, sp_name = "_locZuZhou"},
--     [368] = {eff_id = 10080606, sp_name = "_locChenMo"},
--     [369] = {eff_id = 10080607, sp_name = "_locQieZhan"},
--     [327] = {eff_id = 10080608, sp_name = "_locZhuoShao"},
--     [328] = {eff_id = 10080609, sp_name = "_locDiXian"},
--     [318] = {eff_id = 10080610, sp_name = "_locPoFang"},
--     [332] = {eff_id = 10080612, sp_name = "_locWuDi"},
--     [376] = {eff_id = 10080612, sp_name = "_locWuDi"},
--     [381] = {eff_id = 10080612, sp_name = "_locWuDi"},
--     [329] = {eff_id = 10080611, sp_name = "_locBingFeng"}
-- }

BattleUIBuffType = {
    [1] = {eff_id = 10080601, sp_name = "_locHunLuan"},
    [2] = {eff_id = 10080602, sp_name = "_locZhongDu"},
    [3] = {eff_id = 10080603, sp_name = "_locShiHua"},
    [4] = {eff_id = 10080604, sp_name = "_locJiuZui"},
    [5] = {eff_id = 10080605, sp_name = "_locHunShui"},
    [6] = {eff_id = 10080615, sp_name = "_locZuZhou"},
    [7] = {eff_id = 10080606, sp_name = "_locChenMo"},
    [8] = {eff_id = 10080607, sp_name = "_locQieZhan"},
    [9] = {eff_id = 10080608, sp_name = "_locZhuoShao"},
    [10] = {eff_id = 10080609, sp_name = "_locDiXian"},
    [11] = {eff_id = 10080610, sp_name = "_locPoFang"},
    [12] = {eff_id = 10080612, sp_name = "_locWuDi"},
    [13] = {eff_id = 10080611, sp_name = "_locBingFeng"},
    [14] = {eff_id = 10080001, sp_name = "_locZhongShang"},
    [15] = {eff_id = 10040024, sp_name = nil}
    --嘲讽
}

function BattleSceneRoleState:FlushBuff(change_kv)
    local buff_list = self.ch_info.ui_buff_list
    -- key:buff_key value:buff_id
    if not buff_list then
        return
    end
    if not change_kv then
        return
    end
    for buff_key, ui in pairs(self.buff_ui) do
        self.UIBuffPool:Recycle(ui)
        self.buff_ui[buff_key] = nil
    end

    for buff_key, buff_id in pairs(buff_list) do
        if buff_id ~= nil then
            self.buff_ui[buff_key] = self.UIBuffPool:Spwan(buff_id)
        end
    end
end

local blue_attr = {
    [10] = true,        --免[法术]
    [12] = true,        --伤[法术]
    [19] = true,        --攻[法术]
    [20] = true,        --防[法术]
    [21] = true,        --必[必杀伤害]
}

function BattleSceneRoleState:FlushAttrBuff(change_kv)
    local buff_list = self.ch_info.attr_buff_list
    if not buff_list or not next(buff_list) then
        self.AttrBuff:SetActive(false)
        return
    end

    self.AttrBuff:SetActive(true)
    local index = 0
    for k, v in pairs(buff_list) do
        if v > 0 and index < 6 then
            index = index + 1
            self.AttrBuffBGSet[index]:SetObjActive(true)
            if k > 0 then
                -- local color = blue_attr[i] ~= nil and COLORSTR.Blue11 or COLORSTR.Yellow12
                -- self.AttrBuffText[index].text = string.format("<color=#%s>%s</color>", color, Language.Main.Battle.BuffAttr[i])
                local i = k - 1
                self.AttrBuffBGSet[index].SpriteName = "LvDi"
                if blue_attr[i] then
                    self.AttrBuffArrowSet[index].SpriteName = "LvJianTou2"
                    self.AttrBuffText[index].text = string.format("<color=#%s>%s</color>", COLORSTR.Blue8, Language.Main.Battle.BuffAttr[i])
                else
                    self.AttrBuffArrowSet[index].SpriteName = "LvJianTou"
                    self.AttrBuffText[index].text = string.format("<color=#%s>%s</color>", COLORSTR.Yellow12, Language.Main.Battle.BuffAttr[i])
                end
            else
                -- local color = blue_attr[i] ~= nil and COLORSTR.Blue11 or COLORSTR.Yellow12
                -- self.AttrBuffText[index].text = string.format("<color=#%s>%s</color>", color, Language.Main.Battle.BuffAttr[i])
                local i = -k - 1
                self.AttrBuffBGSet[index].SpriteName = "HongDi"
                if blue_attr[i] then
                    self.AttrBuffArrowSet[index].SpriteName = "HongJianTou2"
                    self.AttrBuffText[index].text = string.format("<color=#%s>%s</color>", COLORSTR.Blue8, Language.Main.Battle.BuffAttr[i])
                else
                    self.AttrBuffArrowSet[index].SpriteName = "HongJianTou"
                    self.AttrBuffText[index].text = string.format("<color=#%s>%s</color>", COLORSTR.Yellow12, Language.Main.Battle.BuffAttr[i])
                end
            end
        end
    end
    if index < 6 then
        for i = index + 1, 6 do
            self.AttrBuffBGSet[i]:SetObjActive(false)
        end
    end
end

function BattleSceneRoleState:FlushSandglass()
    if self.ch_info == nil then
        return
    end
    local show = true
    if self.ch_info.curhp <= 0 then
        show = false
    else
        if BattleData.Instance:BaseData().round_phase == BATTLE_ROUND_PHASE.Client then
            show = false
        elseif self.ch_info.character_type == 0 and not self.ch_info.temp_character_type then
            -- if BattleData.Instance:PlayerIdx() == self.ch_info.grid_idx and BattleData.Instance:CurCmdIdx() ~= self.ch_info.grid_idx then
            --     show = false
            -- end
            show = self.ch_info.inputting
        elseif self.ch_info.character_type == 1 then
            -- show = false
            show = self.ch_info.inputting
        else
            -- elseif BattleData.Instance:PlayerIdx() == self.ch_info.grid_idx then
            --     if BattleData.Instance:CurCmdIdx() ~= self.ch_info.grid_idx then
            --         show = false
            --     end
            -- elseif BattleData.Instance:PlayerPetIdx() == self.ch_info.grid_idx then
            --     -- show = false
            -- else
            --     show = false
            show = false
        end
    end
    local character_id = self.ch_info.character_id
    if self.ch_info.character_type == 1 and BattleData.Instance:PlayerPetIdx() == self.ch_info.grid_idx then
        character_id = BattleData.Instance:GetUidByGridIdx(BattleData.Instance:PlayerIdx())
    elseif self.ch_info.character_type == 1 then
        local idx = BattleData.IsFrontIdx(self.ch_info.grid_idx) and (self.ch_info.grid_idx - 5) or (self.ch_info.grid_idx + 5)
        character_id = BattleData.Instance:GetUidByGridIdx(idx)
    end
    local is_auto = BattleData.Instance:GetAutoInfo(character_id)
    if is_auto == 1 then
        self.Sandglass2:SetActive(show)
        self.Sandglass:SetActive(false)
    else
        self.Sandglass:SetActive(show)
        self.Sandglass2:SetActive(false)
    end
    -- self:FlushBuff()
end

--刷新圈圈是否显示
function BattleSceneRoleState:FlushIndicatorAcitve()
    self.IndicatorImg:SetObjActive(self:isSelTargetShow())
    self.DetailObj:SetActive(not self:isSelTargetShow())
end

BattleSceneBuffItem = BattleSceneBuffItem or DeclareMono("BattleSceneBuffItem", UIWObjPoolItem)
function BattleSceneBuffItem:BattleSceneBuffItem()
    
end
function BattleSceneBuffItem:setData(data)
    local buff_id = data
    local buff_cfg = BattleData.BuffConfig(buff_id)
    local cfg = BattleUIBuffType[buff_cfg.ui_type]
    UH.SpriteName(self.BuffDescSp, cfg.sp_name)
    self.handle_effect = self.EffectTool:PlayByEffectId(cfg.eff_id, self.EffectParent)
    self.Anim:Play()
end
function BattleSceneBuffItem:onRecycle()
    if self.handle_effect then
        self.EffectTool:Stop(self.handle_effect)
        self.handle_effect = nil
    end
end

BattleMoveBoardcast = BattleMoveBoardcast or DeclareMono("BattleMoveBoardcast", UIWFlushPanel)
function BattleMoveBoardcast:BattleMoveBoardcast()
    self.battle_data = BattleData.Instance
    local popup_data = self.battle_data:MovePopupData()
    self.need_wait_list = {}
    self.data_cares = {
        {data = popup_data, func = self.FlushMoveShow}
    }
end
function BattleMoveBoardcast:FlushMoveShow()
    if GmData.Instance.view_data.hide_popup == true then
        return
    end
    --[[ if self.battle_data:BaseData().round_phase == BATTLE_ROUND_PHASE.Client then
        return
    end ]]
    local popup_datas = self.battle_data:PopMovePopupData()
    -- local item
    for _, v in pairs(popup_datas) do
        self.MovePool:Spwan(v)
        --[[ if self.need_wait_list[v.grid_idx] ~= nil then
        else
            self.need_wait_list[v.grid_idx] = item
        end ]]
    end
end

BattleMoveBoardcastItem = BattleMoveBoardcastItem or DeclareMono("BattleMoveBoardcastItem", UIWObjPoolItem)
function BattleMoveBoardcastItem:BattleMoveBoardcastItem()
    self.pos1 = Vector2.zero
    self.pos2 = Vector2.zero
end
function BattleMoveBoardcastItem:setData(data)
    if data.move_type == BATTLE_MOVE_TYPE.USE_FABAO_SER then
        local list = BattleData.Instance:FabaoList()
        local cfg1 = FabaoData.Instance:FabaoAttrInfo(data.param1)
        if cfg1 then
            UH.SetIcon(self.Icon, cfg1.icon, ICON_TYPE.FABAO)
        else
            self:RecycleSelf()
        end
    elseif data.move_type == BATTLE_MOVE_TYPE.NORMAL_ATTACK then
        UH.SetIcon(self.Icon, "1", ICON_TYPE.SKILL)
        self.Icon.enabled = true
    elseif data.move_type == BATTLE_MOVE_TYPE.NORMAL_DEFENCE then
        UH.SetIcon(self.Icon, "31", ICON_TYPE.SKILL)
        self.Icon.enabled = true
    else
        UH.SetSkillIconToImage(self.Icon, data.param1, ICON_TYPE.SKILL)
    end
    -- self.ParentPanel.MainBattleScene.grid_ui[]
    self:StartMove(data.grid_idx, data.target_grid_idx)
end
function BattleMoveBoardcastItem:StartMove(idx1, idx2)
    if self.ParentPanel.MainBattleScene.grid_ui[idx1] == nil then
        LogDG(idx1, "无了")
        self:RecycleSelf()
        return
    end
    if self.ParentPanel.MainBattleScene.grid_ui[idx2] == nil then
        LogDG(idx2, "无了")
        self:RecycleSelf()
        return
    end
    local startPos = self.ParentPanel.MainBattleScene.grid_ui[idx1].BaseFollow.gameObject:GetLocalPosition()
    local endPos = self.ParentPanel.MainBattleScene.grid_ui[idx2].BaseFollow.gameObject:GetLocalPosition()
    self.pos1:Set(startPos.x, startPos.y)
    self.pos2:Set(endPos.x, endPos.y)
    local dis = Vector2.Distance(self.pos1, self.pos2)
    UH.AnchoredPosition(self.IconRect, self.pos1)
    local total = dis > 250 and (10 + 6) or (6 + 6)
    local frame = 0
    local wait_frame = 3
    local real_frame = 0
    local real_total = total - wait_frame
    self.handle_move = TimeHelper:AddRunFrameTimer(function ()
        if frame > wait_frame then
            UH.AnchoredPosition(self.IconRect, Vector2.Lerp(self.pos1, self.pos2, real_frame/real_total))
            real_frame = real_frame + 1
        end
        frame = frame + 1
        if frame > total then
            self:RecycleSelf()
        end
    end, 1, total, true)
end
function BattleMoveBoardcastItem:onRecycle()
    TimeHelper:CancelTimer(self.handle_move)
end


-- >>>>>> CH底部信息 <<<<<
BattleSceneCHBottomInfo = BattleSceneCHBottomInfo or DeclareMono("BattleSceneCHBottomInfo", UIWObjPoolItem)
function BattleSceneCHBottomInfo:setData(data)
    --LOG("CH底部信息",data)
    UH.SetText(self.Name, data.name or "")
    self.data = data
    self:careData(data, BindTool.Bind(self.FlushInfo, self))
end

function BattleSceneCHBottomInfo:FlushInfo()
    local data = self.data

    self.RagePoint:SetObjActive(data.rage_point ~= nil)
    if data.rage_point then
        self.RagePoint:SetValue(data.rage_point)
        self.RagePoint:SetShow(data.rage_value,true)
    end

    local follow_target = data.root_obj ~= nil and data.root_obj.transform or nil
    self.Follow.Target = follow_target
end

function BattleSceneCHBottomInfo:careData(data, func, ...)
    if not self.care_handles then
        self.care_handles = {}
    end
    self.care_handles[data:Care(func, ...)] = data
    func()
end

function BattleSceneCHBottomInfo:onRecycle()
    for handle, data in pairs(self.care_handles) do
        data:Uncare(handle)
    end
    self.care_handles = {}
    self.Follow.Target = nil
end