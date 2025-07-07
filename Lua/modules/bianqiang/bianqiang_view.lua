BianQiangView = BianQiangView or DeclareView("BianQiangView", "bianqiang/bianqiang", Mod.BianQiang.Main)
VIEW_DECLARE_LEVEL(BianQiangView, ViewLevel.L1)

function BianQiangView:OnClickReturn()
    ViewMgr:CloseView(BianQiangView)
end

function BianQiangView:LoadCallback()
    self.BQPanel:OnOpen()
end

BianQiangPanel = BianQiangPanel or DeclareMono("BianQiangPanel", UIWFlushPanel)
function BianQiangPanel:BianQiangPanel()
    self.data = BianQiangCtrl.Instance:Data()
    self.cur_group_id = -1
end

function BianQiangPanel:OnOpen()
    local group_id = math.min(self.data:GetSelecteTab(), 3)
    self.PageList[group_id]:Trigger()
end

function BianQiangPanel:onFlush()
    if self.cur_group_id ~= -1 then
        local show_list = self.data:GetTaskList(self.cur_group_id)
        self.TaskList.AutoTop = false
        self.TaskList:SetData(show_list)
    end
end

function BianQiangPanel:OnSelectedPage(index)
    if index == 3 then
        return
    end
    local show_list = self.data:GetGroupList(index)
    local click_index = 1
    if self.data:GetGroupId() ~= nil then
        for i, v in pairs(show_list) do
            if v.group_id == self.data:GetGroupId() then
                click_index = i
                break
            end
        end
        self.data:SetGroupId(nil)
    end
    self.GroupToggle:SetAllTogglesOff()
    self.GroupList:SetData(show_list)
    self.GroupList:ClickItem(click_index)
end

function BianQiangPanel:OnSelectedGroup(data)
    local show_list = self.data:GetTaskList(data.group_id)
    self.TaskList.AutoTop = true
    self.TaskList:SetData(show_list)
    self.cur_group_id = data.group_id
    
    if self.cur_group_id == BianQiangGroupId.TYPE_ROLE_UP then
        local score, level = RoleData.Instance:GetRoleScore()
        self:ChangeScore(score, level, "_LocRenWuPingFen")
    elseif self.cur_group_id == BianQiangGroupId.TYPE_PET_UP then
        local pet_info = PetData.Instance:GetFightPetInfo()
        local score, level = 0, 0
        if pet_info ~= nil then
            score, level = pet_info:GetScore()
        end
        self:ChangeScore(score, level, "_LocChongWuPingFen")
    elseif self.cur_group_id == BianQiangGroupId.TYPE_PARTNER_UP then
        local score, level = 0, 0
        for _, info in pairs(PartnerCtrl.Instance:Data():GetPartnerAllInfo()) do
            if info:IsFight() then
                score = score + info:GetScore()
            end
        end
        self:ChangeScore(score, level, "_LocHuoBanPingFen")
    else
        self:ChangeScore(0)
    end
end

function BianQiangPanel:ChangeScore(score, level, sprite_name)
    self.Score.gameObject:SetActive(score > 0)
    if score > 0 then
        self.Score:SetScore(score, level)
        UH.SpriteName(self.PingFenImage, sprite_name)
    end
end

--组
BianQiangGroupItem = BianQiangGroupItem or DeclareMono("BianQiangGroupItem", UIWidgetBaseItem)
function BianQiangGroupItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.group_name)
end

function BianQiangGroupItem:Click()
    if self.ToggleClick ~= nil then
        self.ToggleClick:Trigger()
    end
end

--任务
BianQiangTaskItem = BianQiangTaskItem or DeclareMono("BianQiangTaskItem", UIWidgetBaseItem)
function BianQiangTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name)
    local desc_str = BianQiangData.Instance:GetTaskDesc(self.data)
    UH.SetText(self.Desc, desc_str)
    UH.SetText(self.BtnTextG, data.button_name)
    UH.SetText(self.BtnTextY, data.button_name)
    UH.SpriteName(self.Icon, data.icon_name)

    self.BtnGreen:SetActive(data.other_desc ~= "")
    self.BtnYellow:SetActive(data.other_desc == "")
    self.TuiJianFlag:SetActive(data.is_recommend == 1)

    if self:IsShowProg() then
        self.UpNode:SetActive(true)
        self.ImgState.gameObject:SetActive(self:IsShowState())
        self:FlushTaskProg()
    else
        self.UpNode:SetActive(false)
    end
end

function BianQiangTaskItem:FlushTaskProg()
    local max_val, cur_val = BianQiangData.Instance:GetTaskProgress(self.data)
    self.ProgSlider.value = cur_val / max_val
    UH.SetText(self.ProgText, cur_val .. "/" .. max_val)
    if self:IsShowState() then
        if self.data.condition == BianQiangType.TYPE_ROLE_PROMOTE then
            UH.SpriteName(self.ImgState, "_LocScore" .. 1)
        else
            local index = BianQiangData.Instance:GetTaskProgressVal(self.data, max_val, cur_val)
            UH.SpriteName(self.ImgState, "_LocScore" .. index)
        end
    end
end

function BianQiangTaskItem:OnClickBtn()
    if self.data.open_panel ~= "" then
        local opened, open_tip = FunOpen.Instance:GetFunIsOpened(self.data.open_panel)
        if not opened then
            PublicPopupCtrl.Instance:Center(open_tip)
            return
        end
        if self.data.open_panel == Mod.CloudArena.Main then
            CloudArenaCtrl.Instance:OpenCloudArenaView()
        elseif self.data.open_panel == Mod.FightFB.Fight then
            FightData.Instance.fight_fb_ymz.type = 2
            ViewMgr:OpenViewByKey(self.data.open_panel)
        elseif self.data.open_panel == Mod.Mall.Market then
			MarketCtrl.Instance:OnMarketSell()
        else
            ViewMgr:OpenViewByKey(self.data.open_panel)
        end
        ViewMgr:CloseView(BianQiangView)
        if self.data.open_panel ~= Mod.RolePromote.Main then
            ViewMgr:CloseView(RolePromoteView)
        end
    elseif self.data.commit_npc ~= "" then
        local npc_config = VoMgr:NpcVoByNpcId(self.data.commit_npc)
        if npc_config[1] == nil then
            return
        end
        local commit_npc = self.data.commit_npc
        local function func_call()
            SceneLogic.Instance:AutoToNpc(commit_npc)
        end
        SceneLogic.Instance:AutoToPos(npc_config[1].scene_id, nil, func_call, nil, true)
        ViewMgr:CloseView(BianQiangView)
        ViewMgr:CloseView(RolePromoteView)
    elseif self.data.other_desc ~= "" then
        ViewMgr:OpenView(BianQiangAlert, {task_cfg = self.data})
    end
end

function BianQiangTaskItem:IsShowState()
    return BianQiangNoShowState[self.data.condition] == nil
end

function BianQiangTaskItem:IsShowProg()
    return BianQiangNoShowProg[self.data.condition] == nil
end






BianQiangMain = BianQiangMain or DeclareMono("BianQiangMain", UIWFlushPanel)
function BianQiangMain:BianQiangMain()
    self.data_cares = {
        {data = RoleData.Instance.base_data, func = self.FlushRemind, keys = {"level"}},
        {data = MainViewData.Instance.upgrade_list, func = self.FlushRemind, init_call = true},
        {data = SceneData.Instance:Base(), func = self.FlushRemind, init_call = true, keys = {"scene_mode"}},
    }
end

function BianQiangMain:Awake()
    UIWFlushPanel.Awake(self)
    if IS_STANDALONE() then
        UH.LocalScale(self.RootRect, STANDALONE_UI_VECTOR3)
    end
end

function BianQiangMain:FlushRemind()
    local is_open = FunOpen.Instance:GetFunIsOpened(Mod.BianQiang.Main)
    if not is_open then
        self.Info:SetActive(false)
        return
    end
    local remind_num = 0
    for k, v in pairs(MainViewData.Instance.upgrade_list) do
        if v.red then
            remind_num = 1
            break
        end
    end
    self.RedPoint:SetNum(remind_num)
    self.Info:SetActive(remind_num > 0)
end

function BianQiangMain:OnEnable()
    UIWFlushPanel.OnEnable(self)
end

function BianQiangMain:OnDisable()
    UIWFlushPanel.OnDisable(self)
    self:OnCloseClick()
end

function BianQiangMain:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function BianQiangMain:OnOpenClick()
    self.Context:SetActive(not self.Context.activeSelf)
end

function BianQiangMain:OnCloseClick()
    self.Context:SetActive(false)
end
