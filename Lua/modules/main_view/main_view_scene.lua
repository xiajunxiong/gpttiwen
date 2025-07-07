
MainViewScene = MainViewScene or DeclareMono("MainViewScene", UIWFlushPanel)
---ChBoardPool==UIWObjPool(ViewSceneChBoard)
---NpcBoardPool==UIWObjPool(ViewSceneNpcBoard)
---NameBoardPool==UIWObjPool(ViewSceneNpcBoard)

function MainViewScene:MainViewScene()
    self.data = MainViewData.Instance
    self.data_cares = {
        {data = self.data:SceneObjVos(), func = self.FlushBoards},
        {data = SceneData.Instance.base, func = self.FlushLayer, init_call = false},
    }
    self.data:InitSceneUiVos()
end

function MainViewScene:createBoard(vo)
    if SceneConfig.RoleType[vo.obj_type] then
        return self.ChBoardPool:Spwan(vo)
    elseif SceneConfig.NpcBoardType[vo.obj_type] then
        return self.NpcBoardPool:Spwan(vo)
    elseif SceneConfig.NameBoardType[vo.obj_type] then
        return self.NameBoardPool:Spwan(vo)
    else
        return nil
    end
end

function MainViewScene:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushBoards()
end

function MainViewScene:FlushLayer()
    local scene_type = SceneData.Instance.base.scene_mode
    if scene_type == SceneModeType.RabbitRace then
        self.ChBoardPool.gameObject.transform:SetSiblingIndex(0)
    else
        self.ChBoardPool.gameObject.transform:SetSiblingIndex(2)
    end
end

function MainViewScene:FlushBoards()
    local add_ch_vos = {}
    local remove_uis = {}

    for vo, ui in pairs(self.data:GetSceneUiVos()) do
        remove_uis[vo] = ui
    end

    self.data:ForeachObjVo(function(vo)
        if remove_uis[vo] ~= nil-- and vo.obj_type ~= SceneObjType.MainRole 
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
        local ui = self:createBoard(vo)
        if ui ~= nil then
            self.data:SetSceneUiVos(vo,ui)
        end
    end
end

ViewSceneChBoard = ViewSceneChBoard or DeclareMono("ViewSceneChBoard", UIWObjPoolItem)
---Follow========UIFollowTarget
---Name==========Text
---Title=========Text
---Battleing=====GameObject
---BuffImg=======UIImageSpriteSet

function ViewSceneChBoard:ViewSceneChBoard()
    self.care_handles = {}
    self.ch_vo = nil
end

function ViewSceneChBoard:setData(data)
    self.ch_vo = data
    if self.ch_vo ~= nil and self.ch_vo.root_obj ~= nil and self.ch_vo.root_obj.transform ~= nil then
        self.Follow.Target = self.ch_vo.root_obj.transform
    else
        self.Follow.Target = nil
    end
    --初始化
    self.Battleing:SetActive(false)
    self.RYObj:SetActive(false)
    UH.SetText(self.Name, "")
    UH.SetText(self.Title, "")
    local flush_title_func = BindTool.Bind(self.FlushTitle, self)
    local flush_battleing_func = BindTool.Bind(self.FlushBattleing, self)
    if SmartData.Check(self.ch_vo) then
        self:careData(self.ch_vo, BindTool.Bind(self.FlushHeight, self), "SeatHeight")
        self:careData(self.ch_vo, BindTool.Bind(self.FlushName, self), "name", "top_level", "kill_value", "fly_flag")
        self:careData(self.ch_vo, flush_title_func, "title_id", "title_str", "facescroe_id", "guild_name", "guild_post")
        self:careData(self.ch_vo, BindTool.Bind(self.FlushArms, self), "arms")
        self:careData(self.ch_vo, flush_battleing_func, "behavior_state", "obj_id")
        self:careData(SceneData.Instance:Base(), flush_title_func, "scene_id", "scene_mode")
        self:careData(self.ch_vo, BindTool.Bind(self.FlushSwornTitle, self), "sworn_info")

        self:careData(self.ch_vo, BindTool.Bind(self.FlushSign, self), "da_wan_ka_level")
        
        self:FlushBattleing()
    else
        self:FlushTitle()
        self:FlushName()
        self:FlushBattleing()
        self:FlushHeight()
    end
    self:careData(MazeRunData.Instance:RuneCurseInfo(), flush_battleing_func, self.ch_vo.obj_id)
    self.role_setting_change_handle = EventSys:Bind(GlobalEventKeys.RoleSettingChange, BindTool.Bind(self.RoleSettingChange, self))
    self.PeakToken:SetActive(self.ch_vo.obj_type == SceneObjType.PeakToken)
end

local rong_yao_sp = {
    "JiaZuZhiZheng1",
    "JiaZuZhiZheng2",
    "JiaZuZhiZheng3"
}
function ViewSceneChBoard:FlushArms()
    if self.ch_vo == nil then
        return
    end
    if RoleSettingData.IsShield(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER_NAME) then
        -- self.ArmsObj:SetActive(false)
        self.RYObj:SetActive(false)
    else
        local guild_ry = GuildData.Instance:GetTopInfo(self.ch_vo.guild_name)
        if guild_ry and guild_ry > 0 then
            self.RYImg.enabled = true
            self.ArmsObj.enabled = false
            UH.SpriteName(self.RYSp, rong_yao_sp[guild_ry])
        else
            self.RYImg.enabled = false
            self.ArmsObj.enabled = true
        end
        self.ArmsTex.text = self.ch_vo.arms
        -- self.ArmsObj:SetActive(self.ch_vo.arms ~= "")
        -- self.ArmsObj:SetActive(self.ArmsTex.text ~= "")
        self.RYObj:SetActive(self.ch_vo.arms ~= "")
        self.RYObj:SetActive(self.ArmsTex.text ~= "")
    end
    
    if TombRaiderData.Instance:IsTombRaider() then 
        self.RYObj:SetActive(false)
    end 
end

function ViewSceneChBoard:FlushHeight()
    if self.ch_vo == nil then
        return
    end
    if self.ch_vo.SeatHeight then
        self.Follow.Offset = Vector3.New(0, self.ch_vo.SeatHeight / 10, 0)
    else
        self.Follow.Offset = Vector3.New(0, 3 * (self.ch_vo.res_size or 1), 0)
    end
end

function ViewSceneChBoard:RoleSettingChange()
    self:FlushName()
    self:FlushTitle()
    self:FlushArms()
    self:FlushSwornTitle()
end

function ViewSceneChBoard:careData(data, func, ...)
    self.care_handles[data:Care(func, ...)] = data
    func()
end

function ViewSceneChBoard:uncareData(cared_data)
    for handle, data in pairs(self.care_handles) do
        if data == cared_data then
            data:Uncare(handle)
        end
        self.care_handles[handle] = nil
        return
    end
end

function ViewSceneChBoard:onRecycle()
    self.FlyBgSub:SetObjActive(false)
    for handle, data in pairs(self.care_handles) do
        data:Uncare(handle)
    end
    self.care_handles = {}
    self.ch_vo = nil
    self.Follow.Target = nil
    self.Name.text = ""
    self.Title.text = ""
    if self.role_setting_change_handle then
        EventSys:UnBind(self.role_setting_change_handle)
    end
    if self.title_effect_handle ~= nil then
        self.EffectTool:Stop(self.title_effect_handle)
        self.title_effect_handle = nil
    end
end

function ViewSceneChBoard:FlushName()
    -- self.FlyBgSub:SetData({})
    if self.ch_vo == nil then
        return
    end
    if RoleSettingData.IsShield(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER_NAME) then
        self.Name.text = ""
        self.PeakLevelBg:SetActive(false)
        -- self.FlyBgSub:SetData({})
        self.FlyBgSub:SetObjActive(false)
    else
        if self.ch_vo.appearance.fly_flag and self.ch_vo.appearance.fly_flag > 0 then
            self.FlyBgSub:SetObjActive(true)
            self.FlyBgSub:SetData(self.ch_vo.appearance)
        else
            self.FlyBgSub:SetObjActive(false)
        end
        self.PeakLevelBg:SetActive(self.ch_vo.top_level ~= nil and self.ch_vo.top_level > 0 and self.ch_vo.appearance.fly_flag == 0) --PeakRoadData.Instance:IsPeakAndOpen())
        local in_only_fight = SceneData.Instance:GetMode() == SCENE_TYPE.ONLY_FIGHT
        if in_only_fight and self.ch_vo.obj_type == SceneObjType.Role then
            self.Name.text = Language.ItemInfo.NoneName
            UH.Color(self.Name, COLORS.Red10)
        else
            local duel_color = DuelData.Instance:GetNameColor(self.ch_vo.kill_value or 0)
            if duel_color ~= nil then
                self.Name.text = self.ch_vo.name
                UH.Color(self.Name, duel_color)
            else
                self.Name.text = self.ch_vo.name
                UH.Color(self.Name, COLORS.White)
            end
        end
    end

    if TombRaiderData.Instance:IsTombRaider() then 
        self.PeakLevelBg:SetActive(false)
    end 
end

function ViewSceneChBoard:FlushTitle()
    if self.ch_vo == nil then
        return
    end
    self.FbTitle:SetObjActive(false)
    self.TitleIcon:SetObjActive(false)
    if RoleSettingData.IsShield(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER_NAME) then
        self.Title:SetObjActive(false)
        self.FbTitle:SetObjActive(false)
        self.TitleIcon:SetObjActive(false)
        return
    else
        self.Title:SetObjActive(true)
    end

    local title_id = self.ch_vo.title_id
    if self.ch_vo.facescroe_id and self.ch_vo.facescroe_id ~= -1 then
        local config = Config.face_cfg_auto.resource[self.ch_vo.facescroe_id or 0]
        if config ~= nil and config.title_id and config.title_id ~= 0 then
            title_id = config.title_id
        end
    end

    if title_id and type(title_id) == "string" then
        self.Title:SetObjActive(true)
        self.Title.text = title_id
        --新增殿堂神王称号仅限殿堂场景
        if self.ch_vo.obj_type == SceneObjType.PalaceStatus then
            UH.SetText(self.Title, "")
            UH.SetText(self.TitleName3, title_id)
            self.Icon5:SetActive(true)
        else
            self.Icon5:SetActive(false)
        end
        return
    end
    
    local in_melee = GuildData.Instance:IsOpenGuildMelee(true)
    local in_only_fight = SceneData.Instance:GetMode() == SCENE_TYPE.ONLY_FIGHT
    if self.title_effect_handle ~= nil then
        self.EffectTool:Stop(self.title_effect_handle)
    end
    if not in_melee and title_id and title_id > 0 then
        local tit_co = TitleData.GetTitleConfig(title_id)
        if tit_co ~= nil and tit_co.res_id == 0 then
            self.Title.color = QualityColor[tit_co.quality]
            UH.SetText(self.Title, tit_co.name)
            self.Title:SetObjActive(true)
            self.TitleIcon:SetObjActive(false)
            self.TitleEffPar:SetActive(false)
            if TitleData.IsGuildTitle(title_id) and self.ch_vo.guild_post and self.ch_vo.guild_post > -1 then
                local cfg = GuildData.Instance:GetJobInfo(self.ch_vo.guild_post)
                UH.SetText(self.TitleName2, string.format(Language.Guild.TitleName, self.ch_vo.guild_name, cfg.member_name or ""))
                UH.SetText(self.Title, "")
            end
            if self.ch_vo.guild_post then
                self.Icon4:SetActive(TitleData.IsGuildTitle(title_id) and self.ch_vo.guild_post > -1)
            end
        elseif tit_co ~= nil and tit_co.title_id_1 and tit_co.title_id_1 ~= 0 then
            self.Title:SetObjActive(false)
            self.TitleIcon:SetObjActive(false)
            self.TitleEffPar:SetActive(true)
            self.title_effect_handle = self.EffectTool:PlayByEffectId(tit_co.title_id_1, self.TitleEffPar)
        elseif tit_co ~= nil and type(tit_co.res_id) == "string" then
            UH.SetIcon(self.TitleIcon, tit_co.res_id)
            self.Title:SetObjActive(false)
            self.TitleIcon:SetObjActive(true)
            self.TitleEffPar:SetActive(false)
        end
    else
        if in_melee then
            local guild_name = GuildData.Instance:GetGuildInfo().guild_name
            if self.ch_vo.obj_type == SceneObjType.MainRole then
                self.Title.text = guild_name
                UH.Color(self.Title, COLORS.Green7)
            else
                self.Title.text = self.ch_vo.guild_name
                UH.Color(self.Title, self.ch_vo.guild_name == guild_name and COLORS.Green7 or COLORS.Red10)
            end
            self.Title:SetObjActive(true)
            self.TitleIcon:SetObjActive(false)
            self.TitleEffPar:SetActive(false)
        elseif in_only_fight then
            self.Title:SetObjActive(false)
            self.TitleIcon:SetObjActive(false)
            self.TitleEffPar:SetActive(false)
        else
            self.Title:SetObjActive(false)
            self.TitleIcon:SetObjActive(false)
            self.TitleEffPar:SetActive(false)
        end
    end

    if TombRaiderData.Instance:IsTombRaider() then 
        self.Title:SetObjActive(false)
        self.TitleIcon:SetObjActive(false)
        self.FbTitle:SetObjActive(true)
        UH.SpriteName(self.FbTitle,TombRaiderData.Instance:GetTombFbName(self.ch_vo.role_id))
    end
end

function ViewSceneChBoard:FlushSign()
    if QQBigPlayerCtrl.IsQQPC() then
        UH.SpriteName(self.NameSign, "QQPlayerLv" .. (self.ch_vo.da_wan_ka_level or 0))
    else
        UH.SpriteName(self.NameSign)
    end
end

function ViewSceneChBoard:FlushSwornTitle()
    if self.ch_vo == nil then
        return
    end
    local is_show = false
    if self.ch_vo.sworn_info ~= nil and self.ch_vo.sworn_info.empty == false then
        is_show = true
    end
    if is_show and (RoleSettingData.IsShield(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER_NAME)
    or TombRaiderData.Instance:IsTombRaider()
    or GuildData.Instance:IsOpenGuildMelee(true)
    or SceneData.Instance:GetMode() == SCENE_TYPE.ONLY_FIGHT
    ) then
        is_show = false
    end
    --说是结义称号可以和称号共存，结义显示在称号底下
    if not is_show then
        UH.LocalPosG(self.Title.gameObject, Vector3.New(0, 26, 0))
        UH.LocalPosG(self.TitleIcon.gameObject, Vector3.New(0, 24, 0))
        UH.LocalPos(self.TitleEffParTran, Vector3.New(0, 44, 0))
        self.SwornTitle:SetObjActive(false)
    else
        UH.LocalPosG(self.Title.gameObject, Vector3.New(0, 52, 0))
        UH.LocalPosG(self.TitleIcon.gameObject, Vector3.New(0, 40, 0))
        UH.LocalPos(self.TitleEffParTran, Vector3.New(0, 65, 0))
        self.SwornTitle:SetObjActive(true)
        UH.Color(self.SwornTitle, COLORS.Yellow19)
        local sworn_info = self.ch_vo.sworn_info
        UH.SetText(self.SwornTitle, Format(Language.Sworn.SwornNameHave, sworn_info.zu_name.sworn_name, sworn_info.member_name))
    end
end

function ViewSceneChBoard:SetBubble(is_active,bubble)
    if is_active and bubble and bubble ~= "" then
        UH.SetText(self.PaoPao,RichTextHelper.ParseAllFunc(
        bubble, true, RichTextFormat.FormatNpcBubbleSpecial))
    end
    self.PPObj:SetActive(is_active)
end

local maze_rune_id2buff_img = {
    [MazeRunData.Rune.SpeedUp] = "MazeSpeedUp",
    [MazeRunData.Rune.SpeedDown] = "MazeSpeedDown",
    [MazeRunData.Rune.MoveFreeze] = "MazeMoveFreeze",
    [MazeRunData.Rune.BattleAvoid] = "MazeBattleAvoid",
    [MazeRunData.Rune.BattleOften] = "MazeBattleOften"
}

function ViewSceneChBoard:FlushBattleing(change_keys)
    if self.ch_vo == nil then
        return
    end
    if change_keys ~= nil and change_keys.obj_id ~= nil then
        self:uncareData(MazeRunData.Instance:RuneCurseInfo())
        self:careData(MazeRunData.Instance:RuneCurseInfo(), BindTool.Bind(self.FlushBattleing, self), self.ch_vo.obj_id)
    end
    self.Fishing:SetActive(self.ch_vo.behavior_state == ROLE_STATUS.FISHING)
    self.Battleing:SetActive(self.ch_vo.behavior_state == ROLE_STATUS.FIGHT)
    self.BuffImg:SetObjActive(false)
    if self.ch_vo.behavior_state == ROLE_STATUS.FIGHT then
    else
        self.Battleing:SetActive(false)
        local maze_rune_id = MazeRunData.Instance:ObjRuneCurseInfo(self.ch_vo.obj_id)
        if maze_rune_id ~= nil then
            self.BuffImg.SpriteName = maze_rune_id2buff_img[maze_rune_id]
            self.BuffImg:SetObjActive(true)
        else
            self.BuffImg:SetObjActive(false)
        end
    end
end

SpecialNpcBoardShow = {
    Front = {
        boss = true,
        elite_monster = true,
        custom = true,
        plant = true,
        wood = true,
        mineral = true,
        box_t = true,
        box_y = true,
        box_j = true,
        coin = true,
        bonfire = true,
        luck_box_c = true,
        luck_box_z = true,
        luck_box_g = true,
    },
    FrontBg = {
        box_t = "box",
        box_y = "box",
        box_j = "box",
        luck_box_c = "box",
        luck_box_z = "box",
        luck_box_g = "box",
    },
}

ViewSceneNpcBoard = ViewSceneNpcBoard or DeclareMono("ViewSceneNpcBoard", UIWObjPoolItem)
---Follow=========UIFollowTarget
---Name===========Text
---FuncImg========UIImageSpriteSet
---Title==========Text

function ViewSceneNpcBoard:ViewSceneNpcBoard()
    self.obj_vo = nil
    self.care_handles = {}
    -- self.effect_handles = {}
end

function ViewSceneNpcBoard:setData(data)
    self:ClearData()
    if data.id == 0 then return end
    self.obj_vo = data
    local npc_co = NpcDialogData.GetNpcConfig(data)
    if npc_co == nil then
        LogError("[YouLanDebug]ViewSceneNpcBoard:setData npc_co is NULL!data=",data)
        return
    end

    local npc_types = data.npc_types or NpcHelper.NpcTypesById(data.id)
    self.Follow.Target = data.root_obj and data.root_obj.transform or nil
    local name = self:CheckSpecialName(npc_types, npc_co, data.seq) or npc_co.name
    local name2 = self:CheckSpecialName(npc_types, npc_co, data.seq, true) or npc_co.name
    local title_name = self:CheckSpecialTitle(npc_types, npc_co) or npc_co.tit_name
    UH.SetText(self.Name,name)
    UH.SetText(self.AlphaName, name2)
    UH.SetText(self.Title,title_name)
    self.Name:SetObjActive(name ~= "")
    self.IsSpecial:SetActive(self:IsShowSpecial(npc_types))
    self.npc_types = npc_types
    self.npc_co = npc_co
    self:CheckSpecials(npc_types,npc_co.name)
    self.IsAct:SetActive((npc_co.title_type or 0) > 0)
    self:CheckPP(self:CheckBubble(data) or npc_co.is_bubble)
    self:CheckHillSea(npc_co,name)
    self:CheckArenaMatch(npc_types)
    self:careData(ArenaMatchData.Instance.role_info_list,BindTool.Bind(self.FlushName, self))
    self:careData(AnecdoteData.Instance.task_cache_list,BindTool.Bind(self.FlushBubble, self))
    self:careData(FavorData.Instance.favor_data_change,BindTool.Bind(self.FlushFavorShow, self),"val")
    self:careData(RoleData.Instance:GetBaseData(), BindTool.Bind(self.FlushFavorShow, self), "level")
    if title_name == "" and not self.IsSpecial.activeSelf and not self.IsAct.activeSelf then
        self.Title:SetObjActive(false)
    else
        self.Title:SetObjActive(true)
    end
    if self:IsSoulBoss() then
        self:SoulBossPaoPao()
    end 
    self:FlushFavorShow()
end

function ViewSceneNpcBoard:FlushFavorShow()
    if self.obj_vo then
        local data = self.obj_vo
        local is_favor_show = FavorData.Instance:GetFavorShow(data.seq)
        if is_favor_show then
            local cfg = FavorData.Instance:GetFavorNpcConfig(data.seq)
            local favor_data = FavorData.Instance:GetFavorInfo(cfg)
            local favor_level = FavorData.Instance:GetNpcFavorLevel(favor_data.favor_value,cfg)
            UH.SetText(self.FavorLevel, favor_level)
        end
        self.Favor:SetActive(is_favor_show)
    end
end

function ViewSceneNpcBoard:IsSoulBoss()
    return self.obj_vo.obj_type and self.obj_vo.obj_type == SceneObjType.SoulBoss
end
function ViewSceneNpcBoard:SoulBossPaoPao()
    local info = self.obj_vo.info
    if info == nil then return end
    self:SetBubble(true, info.bubble_1)
    TimeHelper:CancelTimer(self.handle_delay_soul_paopao)
    self.handle_delay_soul_paopao = TimeHelper:AddDelayTimer(function ()
        local role_board = MainViewData.Instance:GetBoard("role_id",RoleData.Instance:GetRoleId())
        if role_board then
            role_board:SetBubble(true, info.bubble_2)
        end
        self:SetBubble(false)
        self.handle_delay_soul_paopao = TimeHelper:AddDelayTimer(function ()
            if role_board then
                role_board:SetBubble(false)
            end
            FightData.Instance:SetSoulStoryPlaying(false)
            RoleCtrl.Instance:SendOfflineBreakFight(info.seq)
        end,2)
    end, 1.5)
end

function ViewSceneNpcBoard:CheckBubble(obj_vo)
    local npc_bubble = AnecdoteData.Instance:GetTaskBubble(obj_vo)
    if npc_bubble then
        if npc_bubble.is_show_iden == 1 then
            UH.SpriteName(self.FuncImg,"TanHao")
            self.FuncImgObj:SetActive(true)
        else
            UH.SpriteName(self.FuncImg)
            self.FuncImgObj:SetActive(false)
        end
        return npc_bubble.bubble_id
    end
    return nil
end

function ViewSceneNpcBoard:FlushBubble()
    if self.obj_vo == nil then
        return
    end
    self:SetBubble(false)
    UH.SpriteName(self.FuncImg)
    self.FuncImgObj:SetActive(false)
    local bubble_id = self:CheckBubble(self.obj_vo)
    self:CheckPP(bubble_id or self.npc_co.is_bubble)
end

function ViewSceneNpcBoard:SetBubble(is_active,bubble)
    TimeHelper:CancelTimer(self.timer_rt_pp)
    if is_active and bubble and bubble ~= "" then
        UH.SetText(self.PaoPao,RichTextHelper.ParseAllFunc(
        bubble, true, RichTextFormat.FormatNpcBubbleSpecial))
    end
    self.PPObj:SetActive(is_active)
end

function ViewSceneNpcBoard:SetBubble2(is_active, bubble)
    TimeHelper:CancelTimer(self.timer_rt_pp)
    if is_active and bubble and bubble ~= "" then
       UH.SetText(self.Bubble2Text, RichTextHelper.ParseAllFunc(bubble, true, RichTextFormat.FormatNpcBubbleSpecial))
    end
    self.Bubble2Obj:SetActive(is_active)
end

function ViewSceneNpcBoard:CheckArenaMatch(npc_types)
    local v3 = Vector3.zero
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
    end
    self.handle_runner = Runner.Instance:RunUntilTrue(function ()
        if self.obj_vo.body and self.obj_vo.body.main_body and self.obj_vo.body.GetHeight and self.obj_vo.body:GetHeight() then
            local y = self.obj_vo.body:GetHeight()
            y = y * (self.obj_vo.res_size or 1)
            y = y + 0.4
            y = y < 2 and 2 or y
            y = y > 4.4 and 4.4 or y
            y = npc_types.arena and 5 or y   -- 擂台的要高一点
            if self.obj_vo.id and 43026 == self.obj_vo.id then y = 6 end-- 共工人高马大
            v3.y = y
            self.Follow.Offset = v3
            self.CanvasGroup.alpha = 1
            return true               
        end
        return false
    end)
end

function ViewSceneNpcBoard:CheckSpecials(npc_types,name)
    if npc_types.box_bi then
        self.SpecialIcon:SetObjActive(false)
        self.SpecialBg.SpriteName = "box_bg"
        UH.LocalPosG(self.SpecialBg.gameObject,Vector3.New(10,0,0))
    else
        self.SpecialIcon:SetObjActive(true)
        for key, val in pairs(npc_types) do
            if val and SpecialNpcBoardShow.Front[key] then
                self.SpecialBg.SpriteName = (SpecialNpcBoardShow.FrontBg[key] or key) .. "_bg"
                self.SpecialIcon.SpriteName = key .. "_icon"
                self.SpecialName.text = key == "elite_monster" and Language.EliteMonster[key] or ""
            end
        end
    end
    if npc_types.boss and FightFieldBossData.Instance:IsSceneWild(self.obj_vo.id) then
        UH.SpriteName(self.SpecialBg,[[wild_icon]])
        UH.SetText(self.Name,ColorStr("<size=24>"  ..  name .. "</size>",COLORSTR.Red8))
        UH.LocalPosG(self.SpecialBg.gameObject,Vector3.zero)
    end
end

function ViewSceneNpcBoard:CheckSpecialName(npc_types, npc_co, npc_seq, alpha)
    local color
    local name = npc_co.name
    if npc_types.normal then
        if npc_types.custom then
            color = COLORSTR.Purple2
        end
    elseif npc_types.monster then
        if npc_types.boss then
            color = COLORSTR.Yellow1
        elseif npc_types.elite_monster then
            color = COLORSTR.Blue3
        end
    elseif npc_types.gather then
        if npc_types.plant then
            color = COLORSTR.Green10
        elseif npc_types.wood then
            color = COLORSTR.Yellow2
        elseif npc_types.mineral then
            color = COLORSTR.Blue8
        end
    elseif npc_types.box then
        color = COLORSTR.Yellow16
    elseif npc_types.coin then
        color = COLORSTR.Yellow2
    elseif npc_types.bonfire then
        color = COLORSTR.Yellow1
    elseif npc_types.arena then
        local co = ArenaMatchData.Instance:GetArenaListByNpcSeq(npc_seq)
        return RichTextHelper.SizeStr(co and co.leitai_name or npc_co.name, 22)
    end

    local obj_type = self.obj_vo.obj_type
    if obj_type ~= nil and (obj_type == SceneObjType.RabbitRace or obj_type == SceneObjType.SoulBoss) then
        return self.obj_vo.name
    end

    if color and alpha == true then
        color = color .. "00"
    end
    return color and RichTextHelper.SizeStr(RichTextHelper.ColorStr(name, color), 24) or nil
end

function ViewSceneNpcBoard:CheckSpecialTitle(npc_types, npc_co)
    if npc_types.boss then
        return RichTextHelper.SizeStr(npc_co.tit_name, 24)
    end
    if npc_co.title_type and npc_co.title_type ~= "" and npc_co.title_type > 0 then
        local act_info = ActivityData.Instance:GetActivityInfo(npc_co.title_type)
        if act_info then
            self.ActIconSp.SpriteName = "ActIcon" .. npc_co.title_type
            self.ActName.text = act_info.act_name
        else
            self.ActIconSp.SpriteName = "ActIcon" .. npc_co.title_type
            self.ActName.text = npc_co.tit_name
        end
        self.Title.rectTransform.sizeDelta = Vector2.New(160, 40)
        return ""
    end
end

function ViewSceneNpcBoard:PPActive()
    if self.show_bubble then
        if MainPlayer then
            local pos_p = MainPlayer:GetPos()
            if GameMath.GetDistance(self.obj_vo.x, self.obj_vo.y, pos_p.x, pos_p.z, false) < 50 then
                self.in_view_flush = false
            else
                self.in_view_flush = true
                self.show_bubble = false
            end
        end
    end
    -- self.FuncImgObj:SetActive(self.show_img_func and self.show_bubble)
    self.RespObj:SetActive(self.npc_types.shop and not self.show_bubble)
    self.PPObj:SetActive(self.show_bubble)
end

--in_view_flush 是否离开
--show_bubble 气泡框是否显示
function ViewSceneNpcBoard:CheckPP(is_bubble)
    if 0 == is_bubble then return end
    if MainViewData.Instance:IsBattle() then return end
    if self:IsSoulBoss() then return end
    self.time_pp = 0
    self.in_view_flush = true
    self.show_bubble = false
    TimeHelper:CancelTimer(self.timer_rt_pp)
    local pp_flush = function (value)
        self.show_bubble = value or not self.show_bubble
        self.time_pp = self.show_bubble and 6 or -9
        if self.show_bubble then 
            local bubble = NpcDialogCtrl.Instance.data:RandBubbleById(is_bubble)
            self.PaoPao.text = RichTextHelper.ParseAllFunc(bubble, true, RichTextFormat.FormatNpcBubbleSpecial)
        end
        self:PPActive()
    end
    self.timer_rt_pp = TimeHelper:AddRunTimer(function ()
        if 0 == self.time_pp then
            pp_flush(false)
        end
        self.time_pp = self.time_pp > 0 and self.time_pp - 1 or self.time_pp + 1
        if not self.show_bubble and self.in_view_flush then
            pp_flush(true)
        end
    end, 1)
end

function ViewSceneNpcBoard:CheckHillSea(npc_co,npc_name)
    if npc_co.npc_type == 1100 then
        local name = FightFieldBossData.Instance:GetHillSeaName(npc_co.id)
        if name and name ~= "" then
            UH.SetText(self.Name,npc_name .. RichTextHelper.SizeStr(name, 24))
            UH.SpriteName(self.SpecialIcon,"hill_sea_icon")
            self.Name.alignment = 3
            --[[ local grade_type = FightFieldBossData.Instance:GetGetHillSeaType(npc_co.id)
            SE:Stop(self.effect_handle)
            self.effect_handle = SE:Play(SceneEffectConfig.EffectList["HillSea"..grade_type],Vector3.zero,nil,self.obj_vo.root_obj)
            table.insert(self.effect_handles,self.effect_handle) ]]
        end
    --[[ elseif npc_co.npc_type == 1200 then --北斗七星
        local start_main_cfg = Cfg.BigDipperStarManByNpcSeq(npc_co.id)
        if start_main_cfg ~= nil then
            local star_main_type = start_main_cfg.type
            local grade_type = star_main_type == 1 and "3" or "2"
            SE:Stop(self.effect_handle)
            self.effect_handle = SE:Play(SceneEffectConfig.EffectList["HillSea"..grade_type],Vector3.zero,nil,self.obj_vo.root_obj)
            table.insert(self.effect_handles,self.effect_handle)
        end ]]
    end
end

function ViewSceneNpcBoard:SetImgFunc()
    local img_t = {}
    local npc_co = NpcHelper.NpcCo(self.obj_vo.id, false)
    if npc_co == nil then
        print("没有NPC配置 npcid:", self.obj_vo.id)
        return
    end
    local flag = npc_co == nil or npc_co.icon_id ~= "ShangDianTuBiao"
    flag = flag and npc_co.is_show == 1
    self.show_img_func = TaskCtrl.Instance:GetMainViewFuncImgByNpcId(self.obj_vo.id, img_t) and flag
    self:PPActive()
    if img_t.sp then
        self.FuncImg.SpriteName = img_t.sp
    end
end

function ViewSceneNpcBoard:FlushName()
    if self.obj_vo == nil then
        return
    end
    if SceneData.Instance:GetMode() == SceneModeType.ArenaMatch then
        local rank = ArenaMatchData.Instance:GetRoleRankByNpcSeq(self.obj_vo.seq)
        self.ArenaMatchTips.text = rank > 0 and string.format(Language.ArenaMatch.NpcName, rank) or ""
    end
end

function ViewSceneNpcBoard:GetAlignment(name,is_monster)
    local alignment = is_monster and 3 or 4
    if name and name ~= "" and string.len(name) < 12 then
        alignment = 4
    end
    return alignment
end

function ViewSceneNpcBoard:IsShowSpecial(npc_types)
    local show_special = npc_types.monster or npc_types.gather or npc_types.box 
    or npc_types.coin or npc_types.bonfire or npc_types.custom
    show_special = show_special and not npc_types.gather_block
    return show_special
end

function ViewSceneNpcBoard:ClearData()
    self.obj_vo = nil
    self.Name.text = ""
    self.Title.text = ""
    self.IsAct:SetActive(false)
    self.PPObj:SetActive(false)
    self.Bubble2Obj:SetActive(false)
    self.CanvasGroup.alpha = 1
    self.show_img_func = false
    self.Follow.Target = nil
    self.ArenaMatchTips.text = ""
    self.Title.rectTransform.sizeDelta = Vector2.New(160, 30)
    self.FuncImgObj:SetActive(false)
    self.RespObj:SetActive(false)
    self.Name.alignment = 4
end

function ViewSceneNpcBoard:careData(data, func, ...)
    self.care_handles[data:Care(func, ...)] = data
    func()
end

function ViewSceneNpcBoard:onRecycle()
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
    end
    for handle, data in pairs(self.care_handles) do
        data:Uncare(handle)
    end
    self.care_handles = {}
    TimeHelper:CancelTimer(self.timer_rt_pp)
    TimeHelper:CancelTimer(self.handle_delay_soul_paopao)
    TimeHelper:CancelTimer(self.timer_interval)
    self:ClearData()
    -- if self.effect_handle and self.effect_handle.obj then
        --[[ SE:Release(self.effect_handle)
        self.effect_handle = nil ]]
    -- end
    --[[ for index, value in ipairs(self.effect_handles) do
        SE:Release(value)
    end ]]
    --[[ self.effect_handle = nil
    self.effect_handles = {} ]]
end

ViewSceneNameBoard = ViewSceneNameBoard or DeclareMono("ViewSceneNameBoard", UIWObjPoolItem)
---Follow=========UIFollowTarget
---Name===========Text
-- PetQua=========UIImageSpriteSet
-- PetQuaObj======GameObject
function ViewSceneNameBoard:ViewSceneNameBoard()
    self.obj_vo = nil
end

function ViewSceneNameBoard:setData(data)
    self.obj_vo = data
    self.Follow.Target = self.obj_vo.root_obj ~= nil and self.obj_vo.root_obj.transform or nil
    UH.Color(self.Name, COLORS.Blue11)
    self.Name.text = self.obj_vo.name
    
    if self.obj_vo.obj_type == SceneObjType.Pet then
        if self.obj_vo.fly_flag > 0 then
            self.FlyBgSub:SetObjActive(true)
            self.FlyBgSub:SetData(self.obj_vo)
        else
            self.FlyBgSub:SetObjActive(false)
        end
        -- local cfg = PetData.Instance:GetPetCfg()
        UH.SpriteName(self.PetQua, PetData.PetLevel[self.obj_vo.pet_qua])
        self.PetQuaObj:SetActive(true)
        if self.obj_vo.rank >= 0 and self.obj_vo.rank <= 9 then
            self.PetRankObj:SetActive(true)
            UH.SetText(self.PetRankTxt, self.obj_vo.rank + 1 )
        else
            self.PetRankObj:SetActive(false)
        end
    else
		-- self.FlyBgSub:SetData({})
        self.FlyBgSub:SetObjActive(false)
        self.PetQuaObj:SetActive(false)
        self.PetRankObj:SetActive(false)
    end
    self.role_setting_change_handle = EventSys:Bind(GlobalEventKeys.RoleSettingChange, BindTool.Bind(self.RoleSettingChange, self))
    self:RoleSettingChange()
end

function ViewSceneNameBoard:RoleSettingChange()
    if RoleSettingData.IsShield(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER_NAME) then
        self.Name.text = ""
        self.PetQuaObj:SetActive(false)
        self.FlyBgSub:SetData({})
    else
        self.Name.text = self.obj_vo.name
        if self.obj_vo.obj_type == SceneObjType.Pet then
            self.PetQuaObj:SetActive(true)
            self.FlyBgSub:SetData(self.obj_vo)
        end
    end
end

function ViewSceneNameBoard:onRecycle()
    self.obj_vo = nil
    self.Follow.Target = nil
    self.Name.text = ""
    self.PetQuaObj:SetActive(false)
    self.PetRankObj:SetActive(false)
    if self.role_setting_change_handle then
        EventSys:UnBind(self.role_setting_change_handle)
    end
end