ConcentricKnotView = ConcentricKnotView or DeclareView("ConcentricKnotView", "period_activity_act/concentric_knot/concentric_knot_view", Mod.PeriodActivity.ConcentricKnot)
function ConcentricKnotView:CloseCallback()
    local other_cfg = ConcentricKnotData.Instance:OtherConfig()
    local result_info = ConcentricKnotData.Instance:GetResultInfo()
    local is_finish = result_info ~= nil and result_info.is_finish or 0
    if is_finish == 0 then
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = other_cfg.talk_fail_id})
    elseif result_info.has_reward then
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = other_cfg.talk_win_id})
    else
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = other_cfg.talk_win_id2})
        PublicPopupCtrl.Instance:Center(Language.ConcentricKnot.GameError1)
    end
end

function ConcentricKnotView:OnClickClose()
    PublicPopupCtrl.Instance:DialogTips {
        content = Language.ConcentricKnot.OutAlert,
        confirm = {
            func = function()
                ConcentricKnotData.Instance:OnGameOver({is_finish = 0})
                ConcentricKnotCtrl.Instance:SendGameEnd(0)
                ViewMgr:CloseView(ConcentricKnotView)
                PublicPopupCtrl.Instance:CloseDialogTips()
            end
        }
    }
end

ConcentricKnotPanel = ConcentricKnotPanel or DeclareMono("ConcentricKnotPanel", UIWFlushPanel)
function ConcentricKnotPanel:ConcentricKnotPanel()
    self.data = ConcentricKnotData.Instance
    self.card_num = 9

    self.data_cares = {
        {data = self.data.game_info, func = self.OnGameBegin, keys = {"begin_time"}, init_call = false},
        {data = self.data.game_info, func = self.OnGameOver, keys = {"is_finish"}, init_call = false},
        {data = self.data.card_change, func = self.OnCardChange, init_call = false},
        {data = self.data.card_succ, func = self.FlushResult, init_call = false},
        {data = TeamData.Instance:GetTeamInfo(), func = self.OnRoleLeave, init_call = false}
    }
end

function ConcentricKnotPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:InitCards()
    self:FlushMember()
    local index = TeamData.Instance:IsLeader() and 1 or 2
    UH.SetText(self.StartText, Language.ConcentricKnot.BeginState[index])
end

function ConcentricKnotPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.count_timer)
    TimeHelper:CancelTimer(self.run_timer)
    TimeHelper:CancelTimer(self.test_timer)
end

function ConcentricKnotPanel:OnRoleLeave()
    --检测游戏未开始，队友掉线，关闭界面
    if not TeamData.Instance:InTeam() then
        self:OutGame()
        return
    end
    local member_list = TeamCtrl.Instance:Data():GetTeamInfo():MemberListSort()
    if #member_list < 2 then
        self:OutGame()
        return
    end
    if self.game_begin == true then
        return
    end
    for i, v in ipairs(member_list) do
        if v.info.uid ~= RoleData.Instance:GetRoleId() and not v:IsOnline() then
            self:OutGame()
            return
        end
    end
end
function ConcentricKnotPanel:OutGame()
    ConcentricKnotData.Instance:OnGameOver({is_finish = 0})
    ViewMgr:CloseView(ConcentricKnotView)
end

--初始化角色信息
function ConcentricKnotPanel:FlushMember()
    if not TeamData.Instance:InTeam() then
        return
    end

    local member_list = TeamCtrl.Instance:Data():GetTeamInfo():MemberListSort()
    for i = 1, 2 do
        if member_list[i] ~= nil then
            local info = member_list[i].info
            local suffix = info.uid == RoleData.Instance:GetRoleId() and "R" or "L"
            -- local suffix = member_list[i]:IsLeader() and "L" or "R"
            local data = {
                avatar_type = info.appearance.avatar_type,
                avatar_id = info.appearance.avatar_id,
                avatar_quality = info.appearance.avatar_quality,
                avatar_level = info.level
            }
            self["Avatar" .. suffix]:SetData(data)
            UH.SetText(self["Name" .. suffix], self:NameStr(info.name))
        end
    end
end

function ConcentricKnotPanel:NameStr(name)
    local char_list = DataHelper.GetStringWordToTable(name)
    if #char_list > 4 then
        local show_name = ""
        for i = 1, 4 do
            show_name = show_name .. char_list[i]
        end
        return show_name .. "..."
    end
    return name
end

--初始化卡牌
function ConcentricKnotPanel:InitCards()
    self.CardListL = {}
    self.CardListR = {}
    for i = 1, self.card_num do
        self.CardListL[i] = self.CardPoolL[self.card_num - i + 1]
        self.CardListR[i] = self.CardPoolR[self.card_num - i + 1]
        self.CardListL[i]:InitCard(i, 1)
        self.CardListR[i]:InitCard(i, 2)
    end
end

--开始游戏
function ConcentricKnotPanel:OnGameBegin()
    self.game_begin = true
    local show_list = self.data:GetShowList()
    for i = 1, self.card_num do
        self.CardListL[i]:SetData(show_list[1][i])
        self.CardListR[i]:SetData(show_list[2][i])
    end
    self.BtnStart:SetActive(false)
    self:DeclCard()
end

function ConcentricKnotPanel:DeclCard()
    local cur_times = 0
    local function func_update()
        cur_times = cur_times + 1
        if cur_times <= self.card_num then
            self.CardListL[cur_times].gameObject:SetParent(self.CardNodeL[cur_times])
            self.CardListR[cur_times].gameObject:SetParent(self.CardNodeR[cur_times])
            self.CardListL[cur_times]:ToPosition()
            self.CardListR[cur_times]:ToPosition()
        elseif cur_times == self.card_num + 5 then
            for i = 1, self.card_num do
                self.CardListL[i]:ToFront(true)
                self.CardListR[i]:ToFront(true)
            end
            self:StartReadyTimer()
        end
    end
    self.run_timer = TimeHelper:AddRunTimer(func_update, 0.1, self.card_num + 4, true)
end

function ConcentricKnotPanel:OnGameOver()
    TimeHelper:CancelTimer(self.count_timer)
    self.is_win = self.data.game_info.is_finish == 1
    self:ShowResult(self.is_win)
end

function ConcentricKnotPanel:OnCardChange()
    local suffix = self.data.card_change.camp == 1 and "L" or "R"
    local index = self.data.card_change.index
    self["CardList" .. suffix][index]:ToFront()
end

function ConcentricKnotPanel:FlushResult()
    local cs = self.data.card_succ
    for i = 1, 2 do
        local CardList = self["CardList" .. (i == 1 and "L" or "R")]
        if cs.camp_list ~= nil then
            local info = cs.camp_list[i]
            if info then
                if info.hide then
                    CardList[info.index]:HideObj()
                else
                    CardList[info.index]:ToBehind()
                end
            end
        end
        if cs.camp_effect ~= nil then
            local info = cs.camp_effect[i]
            if info then
                CardList[info.index]:ShowEffect()
            end
        end
    end

    if cs.num == self.card_num then
        TimeHelper:CancelTimer(self.count_timer)
        ConcentricKnotCtrl.Instance:SendGameEnd(1)
    end
end

--准备倒计时
function ConcentricKnotPanel:StartReadyTimer()
    if self.count_timer ~= nil then
        return
    end
    self.ReadyTimer:SetActive(true)
    self.play_time = self.data:RedayTime()
    local func_update = BindTool.Bind1(self.OnReadyTimeUpdate, self)
    local func_finish = BindTool.Bind1(self.OnReadyTimeFinish, self)
    self.count_timer = TimeHelper:AddCountDownTT(func_update, func_finish, self.play_time, 1, true)
end
function ConcentricKnotPanel:OnReadyTimeUpdate()
    UH.SpriteName(self.ReadyImage, "DaoJiShiShuZi" .. self.play_time)
    self.play_time = self.play_time - 1
end
function ConcentricKnotPanel:OnReadyTimeFinish()
    self.ReadyTimer:SetActive(false)
    for i = 1, self.card_num do
        self.CardListL[i]:ToBehind()
        self.CardListR[i]:ToBehind()
    end
    self:StartGameTimer()
end

--开始倒计时
function ConcentricKnotPanel:StartGameTimer()
    self.GameTimer:SetActive(true)
    self.play_time = self.data:GameTime()
    local func_update = BindTool.Bind1(self.OnGameTimeUpdate, self)
    local func_finish = BindTool.Bind1(self.OnGameTimeFinish, self)
    self.count_timer = TimeHelper:AddCountDownTT(func_update, func_finish, self.play_time, 1, true)
    -- self:CardTest()
end
--模拟对方点牌
function ConcentricKnotPanel:CardTest()
    local function func_other()
        local list = {}
        for i = 1, 9 do
            if self.CardListL[i].card_state ~= 2 then
                table.insert(list, i)
            end
        end
        if #list > 0 then
            local protocol = {uid = 1}
            protocol.card_id = list[GameMath.Ramdon(1, #list)]
            if ConcentricKnotData.Instance.anim_cards[self.CardListL[protocol.card_id].card_index] == nil then
                if self.CardListL[protocol.card_id].block_click == false then
                    ConcentricKnotData.Instance:OnCardChange(protocol)
                end
            end
        end
    end
    self.test_timer = TimeHelper:AddRunTimer(func_other, 0.8)
end

function ConcentricKnotPanel:OnGameTimeUpdate()
    UH.SetText(self.GameTimeText, self.play_time .. "S")
    self.play_time = self.play_time - 1
end
function ConcentricKnotPanel:OnGameTimeFinish()
    UH.SetText(self.GameTimeText, "0S")
    if TeamData.Instance:IsLeader() then
        ConcentricKnotCtrl.Instance:SendGameEnd(0)
    end
end

function ConcentricKnotPanel:ShowResult(is_win)
    local result_info = ConcentricKnotData.Instance:GetResultInfo()
    result_info.use_time = self.data:GameTime() - (self.play_time or 0)

    if not is_win then
        VIEW_DECLARE_MASK(ConcentricKnotFinishView, ViewMask.BgBlockClose)
    else
        VIEW_DECLARE_MASK(ConcentricKnotFinishView, ViewMask.BgBlock)
    end
    ViewMgr:OpenView(ConcentricKnotFinishView)
end

--开始发牌
function ConcentricKnotPanel:OnClickStart()
    if not TeamData.Instance:IsLeader() then
        return
    end
    self.BtnStart:SetActive(false)
    local cards_grade = ConcentricKnotData.Instance:RandomCardsGroup()
    ConcentricKnotCtrl.Instance:SendStart(cards_grade)
end

function ConcentricKnotPanel:OnClickResult()
    if self.is_win then
        return
    end
    ViewMgr:CloseView(ConcentricKnotView)
end

function ConcentricKnotPanel:OnClickOK()
    ViewMgr:CloseView(ConcentricKnotView)
end

ConcentricKnotItem = DeclareMono("ConcentricKnotItem", UIWidgetBaseItem)
function ConcentricKnotItem:ConcentricKnotItem()
    self.mm_data = ConcentricKnotData.Instance
    self.card_index = 1
    self.card_state = 0 -- 1为预览
    self.block_click = true

    self.anim_handle_card = self.CardAnim:ListenEvent("end", BindTool.Bind1(self.OnAnimEnd, self))
end

function ConcentricKnotItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    self.CardAnim:UnlistenEvent("end", self.anim_handle_card)
    TimeHelper:CancelTimer(self.run_timer)
end

function ConcentricKnotItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.CardSp, "_LocCard" .. self.data)
end

function ConcentricKnotItem:InitCard(index, card_camp)
    self.card_index = index
    self.card_camp = card_camp
end

function ConcentricKnotItem:OnAnimEnd(eve_param)
    if "showed" == eve_param then
        ConcentricKnotData.Instance:SetIsAnimOver(self.card_camp, self.card_index, true)
        if self.card_state == 1 then
            return
        end
        self.block_click = false
        local to_behind, hide = self.mm_data:CardShowed(self.card_camp, self.card_index, self.data)
        if hide then
            self:HideObj()
        elseif to_behind then
            self:ToBehind()
        end
    elseif "show" == eve_param then
        if self.card_state == 1 then
            self.EffectShow:SetActive(true)
            return
        end
        if self.mm_data:CardShow(self.card_camp, self.card_index, self.data) then
            self:ShowEffect()
        end
    elseif "hide" == eve_param then
        ConcentricKnotData.Instance:SetIsAnimOver(self.card_camp, self.card_index, true)
        if self.card_state == 1 then
            self.card_state = 0
            self.EffectShow:SetActive(false)
        end
        self.block_click = false
    end
end

function ConcentricKnotItem:ToFront(first_show)
    ConcentricKnotData.Instance:SetIsAnimOver(self.card_camp, self.card_index, false)
    self.CardAnim:SetTrigger(APH("show"))
    self.block_click = true
    if first_show then
        self.card_state = 1
    end
end

function ConcentricKnotItem:ToBehind()
    ConcentricKnotData.Instance:SetIsAnimOver(self.card_camp, self.card_index, false)
    self.CardAnim:SetTrigger(APH("hide"))
    self.block_click = true
end

function ConcentricKnotItem:HideObj()
    self.ItemObj:SetActive(false)
    self.card_state = 2
end

function ConcentricKnotItem:OnClickCard()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    if self.card_camp ~= ConcentricKnotData.Instance:GetCamp() then
        PublicPopupCtrl.Instance:Center(Language.ConcentricKnot.CardError1)
        return
    end
    if self.block_click then
        return
    end
    if not ConcentricKnotData.Instance:IsAnimOver(self.card_camp, self.card_index) then
        return
    end
    if not ConcentricKnotData.Instance:CanClickCard(self.card_index) then
        return
    end
    ConcentricKnotCtrl.Instance:SendOpenCard(self.card_index)
end

function ConcentricKnotItem:ShowEffect()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.JinJie1)
    self.EffectT:SetActive(true)
end

function ConcentricKnotItem:ToPosition()
    -- self.gameObject:SetActive(true)
    self.total_times = 5
    local move_dis = (self.ItemObj.transform.localPosition - Vector3.New(0, 0, 0)) / self.total_times
    local function func_update()
        UH.LocalPosG(self.ItemObj, self.ItemObj.transform.localPosition - move_dis)
    end
    self.run_timer = TimeHelper:AddRunFrameTimer(func_update, 1, (self.total_times - 1) * 1, true)
end

function ConcentricKnotItem:ToPosition1()
    self.total_time = 0.3
    self.lerp_time = 0.05
    local move_dis = (self.ItemObj.transform.localPosition - Vector3.New(0, 0, 0)) / self.total_time * self.lerp_time
    local function func_update()
        UH.LocalPosG(self.ItemObj, self.ItemObj.transform.localPosition - move_dis)
    end
    self.run_timer = TimeHelper:AddRunTimer(func_update, self.lerp_time, 5, true)
end
