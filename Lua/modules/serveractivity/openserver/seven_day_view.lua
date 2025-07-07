SevenDayView = SevenDayView or DeclareView("SevenDayView", "serveractivity/seven_day_view",Mod.RandActivity.SevenDay)

VIEW_DECLARE_LEVEL(SevenDayView,ViewLevel.L1)

function SevenDayView:SevenDayView()
    self.Panel[self:GetDayType()]:SetActive(true)
end

function SevenDayView:GetDayType()
    local type = SevenDayData.Instance:GetDayType() + 1
    return (type == 0 or type > 2) and 1 or type
end

function SevenDayView:CloseCallback()
    SevenDayData.Instance:CheckRedPoint()
end

--===========================SevenDayPanel===========================
SevenDayPanel = SevenDayPanel or DeclareMono("SevenDayPanel", UIWFlushPanel)

function SevenDayPanel:SevenDayPanel()
    self.data = SevenDayData.Instance
    self.data_cares = {
        {data = self.data.select_data,  func = self.FlushTaskView,  init_call = false},
        {data = self.data.reward_data,  func = self.FlushRewardView,init_call = false},
        {data = self.data.goal_data,  func = self.FlushTaskInfoView,init_call = false},
        {data = self.data.info_data,  func = self.FlushBtnListView,init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function SevenDayPanel:Awake()
    UIWFlushPanel.Awake(self)
    local reward_list,end_reward_data = self.data:GetRewardList()
    self.RewardList:SetData(reward_list)
    self.EndRewardItem:SetData(end_reward_data)
    self:FlushBtnListView()
    self:FlushTaskNumView()
end

function SevenDayPanel:FlushBtnListView()
    self.Image:SetActive(self.data:GetDayNum() + 1 < self.data:GetMaxDayNum())
    self.BtnList:SetDataList(self.data:GetGoalBtnData(),Valve(self.data:GetDayNum(),self.data:GetMaxDayNum()))
    self.Time:StampTime(self.data:GetTimeStamp(),TimeType.Type_Time_3,Language.OpenAct.SevenDay.DayNumTip[self.data:GetDayType()])
end

function SevenDayPanel:FlushTaskView()
    self.TaskList:SetData(self.data:GetGoalData())
    self:FlushGiftView(self.data:GetSelectType())
end

function SevenDayPanel:FlushTaskInfoView()
    for k,item in pairs(self.BtnList.item_list or {}) do
        if item ~= nil then
            item:SetRedNum()
        end
    end
    self:FlushTaskView()
end

function SevenDayPanel:FlushRewardView()
    self:FlushTaskNumView()
    for k,item in pairs(self.RewardList.item_list or {}) do
        if item ~= nil then
            item:FlushItemView()
        end
    end
    self.EndRewardItem:FlushItemView()
    self:FlushGiftView(self.data:GetSelectType())
end

function SevenDayPanel:FlushTaskNumView()
    local task_num,target_num = self.data:GetTaskNum()
    self.Progress:SetProgress(task_num / target_num)
    self.Progress:SetText(Format("%s/%s",task_num,target_num))
end

function SevenDayPanel:OnClickClose()
    ViewMgr:CloseView(SevenDayView)
end

function SevenDayPanel:OnClickGift()
    ServerActivityCtrl.Instance:SendCmdBuyItemCheckReq(
        BUY_ITEM_TYPE.TYPE_SEVEN_DAY_NEW,
        self.money_price,
        self.data:GetDayType(),
        self.data:GetSelectType(),
        ServerActivityData.Instance:GetOtherConfig("mysterious_cfg_ver")
    )
end

function SevenDayPanel:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_SEVEN_DAY_NEW, self.money_price * 10)
    ViewMgr:CloseView(SevenDayView)
end

function SevenDayPanel:FlushGiftView(start_time)
    local day_num = self.data:GetDayNum()
    local config = self.data:GetGiftConfig(start_time)
    if config then
        local item = Item.New(config.item)
        self.GiftItem:SetData(item)
        UH.SetText(self.GiftName,item:Name())
        UH.SetText(self.GiftNum,ColorStr(config.original_price / 10,COLORSTR.Yellow16))
        UH.SetText(self.BtnGift,Format(Language.Reward.Invest.Gold,config.price / 10))
        self.money_price = config.price / 10
        self.GiftInter.Interactable = day_num >= start_time and self.data:GetRewardFlag(10 + start_time) == 0
        self.Tips:SetActive(start_time == day_num + 1)
    end
    self.GiftPanel:SetActive(config ~= nil)
end

--===========================SevenDayBtn===========================
SevenDayBtn = SevenDayBtn or DeclareMono("SevenDayBtn", UIWidgetBaseItem)

function SevenDayBtn:SetData(data)
    for i=1,2 do
        local day = SevenDayData.Instance:GetDayType() == 0 and data.day or DataHelper.GetDaXie(data.day)
        UH.SetText(self.Name[i],Format(Language.OpenAct.SevenDay.DayNum,day))
    end
    local day_num = SevenDayData.Instance:GetDayNum()
    self.Lock:SetActive(day_num < data.day)
    if data.day == day_num + 1 then
        for i=1,2 do
            UH.SetText(self.Name[i],Language.OpenAct.SevenDay.TomorrowTip)
            UH.SetText(self.Finish[i],Format("（%s/%s）",0,SEVEN_DAY_GOAL_MAX_NUM))
        end
        self.Lock:SetActive(false)
    end
    if self.Inter then
        self.Inter.Interactable = not self.Lock.activeSelf
    end
    UIWidgetBaseItem.SetData(self, data)
    self:SetRedNum()
end

function SevenDayBtn:SetRedNum()
    local day_num = SevenDayData.Instance:GetDayNum()
    if day_num >= self.data.day then
        self.RedPos:SetNum(SevenDayData.Instance:GetTaskGroupRemin(self.data.day))
        local num,max_num = SevenDayData.Instance:GetFinishNum(self.data.day)
        for i=1,2 do
            if num == max_num then
                UH.SetText(self.Finish[i],Language.OpenAct.SevenDay.FinishTip)
            else
                UH.SetText(self.Finish[i],Format("（%s/%s）",num,max_num))
            end
        end
    else
        self.RedPos:SetNum(0)
        for i=1,2 do
            local num,max_num = SevenDayData.Instance:GetFinishNum(self.data.day)
            UH.SetText(self.Finish[i],Format("（%s/%s）",num,max_num))
        end
    end
end

function SevenDayBtn:OnClickItem()
    SevenDayData.Instance:SetSelectType(self.data.day)
end

function SevenDayBtn:OnClickLock()
    PublicPopupCtrl.Instance:Center(Format(Language.OpenAct.SevenDay.DayNumLock,self.data.day))
end

function SevenDayBtn:Click()
    self.Toggle:Trigger()
end

--===========================SevenDayTask===========================
SevenDayTask = SevenDayTask or DeclareMono("SevenDayTask", UIWidgetBaseItem)

function SevenDayTask:SetData(data)
    data.item = BagData.Instance:ItemBuild(SevenDayData.Instance:GetReardItem(data.reward_id))
    self.Call:SetData(data.item)
    UH.SetText(self.Desc,data.description)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushItemView()
end

function SevenDayTask:FlushItemView()
    self.Effect:SetActive(false)
    local task_info = self.data.task_info or self:GetInfo(self.data.start_time,self.data.seq)
    local progress,parameter = task_info.progress,self.data.parameter2
    if self.data.parameter ~= 0 then
        parameter = self.data.parameter
        progress = task_info.state == 0 and 0 or parameter
    end
    progress = Valve(progress,parameter)
    
    self.Progress:SetProgress(progress / parameter)
    self.Progress:SetText(Format("(%s/%s)",progress,parameter))
    --可领取时
    if task_info.state ~= 0 then
        local State = Language.OpenAct.SevenDay.TaskState
        UH.SetText(self.BtnName,State[task_info.state])
        self.BtnInter.Interactable = task_info.state ~= 2
        self.Effect:SetActive(task_info.state == 1)
    else
        self.BtnInter.Interactable = false
        self.Effect:SetActive(false)
        UH.SetText(self.BtnName,Language.OpenAct.SevenDay.TaskState[1])
    end
    --领取完毕
    if task_info.state == 2 then
        self.Progress:SetProgress(1)
        self.Progress:SetText(Format("(%s/%s)",parameter,parameter))
    end
    self.BtnClick:SetActive(task_info.state ~= 0 or self.data.open_panel == "")
    UH.LocalScale(self.BtnRect,Vector3.one)
    local day_num = SevenDayData.Instance:GetDayNum()
    self.EnterInter.Interactable = day_num >= self.data.start_time and self:IsGoOpen(self.data.open_panel)
    local enter_name = Language.DailyGuildTask.GoTo
    if not self:IsGoOpen(self.data.open_panel) then
        local level = FunOpen.Instance:GetFunOpenLevel(self.data.open_panel)
        enter_name = level == 1 and Language.OpenAct.SevenDay.TaskTips or Format(Language.Activity.ActItem.LevelLimit, level)
    end
    UH.SetText(self.EnterName,self.data.start_time == day_num + 1 and Language.OpenAct.SevenDay.TomorrowTip or enter_name)
end

function SevenDayTask:GetInfo(start_time,seq)
    return SevenDayData.Instance:GetTaskInfo(start_time,seq)
end

function SevenDayTask:OnClickEnter()
    local day_num = SevenDayData.Instance:GetDayNum()
    if self.data.start_time == day_num + 1 then
        PublicPopupCtrl.Instance:Center(Language.OpenAct.SevenDay.TomorrowTip)
        return
    end
    local open_panel = self.data.open_panel
    if open_panel ~= "" then
        if open_panel > 0 then
            local opened, open_tip = FunOpen.Instance:GetFunIsOpened(open_panel)
            if not opened then
                PublicPopupCtrl.Instance:Center(open_tip)
                return
            end
            if self.data.open_panel == Mod.CloudArena.Main then
                CloudArenaCtrl.Instance:OpenCloudArenaView()
                return
            elseif self.data.open_panel == Mod.FightFB.Fight then
                FightData.Instance.fight_fb_ymz.type = 2
            end
            ViewMgr:OpenViewByKey(self.data.open_panel)
        else
            local npc_id = math.abs(open_panel)
            local npc_config = VoMgr:NpcVoByNpcId(npc_id)
            SceneLogic.Instance:AutoToPos(npc_config[1].scene_id,nil,function()
                SceneLogic.Instance:AutoToNpc(npc_id)
            end,nil,true)
        end
    end
    ViewMgr:CloseView(SevenDayView)
end

function SevenDayTask:OnClickItem()
    SevenDayCtrl.Instance:SendAllReq(1,self.data.start_time,self.data.seq)
    SevenDayData.Instance:CheckRedPoint()
end

function SevenDayTask:IsGoOpen(open_panel)
    if open_panel ~= "" and open_panel > 0 then
        return FunOpen.Instance:GetFunIsOpened(open_panel)
    end
    return true
end

--===========================SevenDayReward===========================
SevenDayReward = SevenDayReward or DeclareMono("SevenDayReward", UIWidgetBaseItem)

function SevenDayReward:SetData(data)
    if data.reward_item then
        data.item = Item.New(data.reward_item[0])
        self.Call:SetData(data.item)
        UIWidgetBaseItem.SetData(self, data)
        self:FlushItemView()
    end
    self.Panel:SetActive(data.reward_item ~= nil)
end

function SevenDayReward:FlushItemView()
    if self.data == nil then return end
    local not_reward = SevenDayData.Instance:GetRewardFlag(self.data.seq) == 0
    local can_reward = SevenDayData.Instance:GetTaskNum() >= self.data.request_score
    self.Mark:SetActive(not not_reward)
    self.Effect:SetActive(not_reward and can_reward)
    local task_num = Valve(SevenDayData.Instance:GetTaskNum(),self.data.request_score)
    UH.SetText(self.Num,Format("%s/%s",task_num,self.data.request_score))
end

function SevenDayReward:OnClickItem()
    if self.data == nil then return end
    CellClicks.BagGridNoButton({GetData = function()
        return self.data.item
    end})
end

function SevenDayReward:OnClickReward()
    SevenDayCtrl.Instance:SendAllReq(2,self.data.seq)
end

--============================SevenModelPanel======================
SevenModelPanel = SevenModelPanel or DeclareMono("SevenModelPanel",UIWFlushPanel)

function SevenModelPanel:SevenModelPanel()
    self.data = SevenDayData.Instance
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self,
        })
    end
    if self.Root ~= nil then
        if (IS_IPHONEX or EditorSettings:SimularIphoneX())then
            UH.LocalPos(self.Root,Vector3.New(30,0,0))
        end
    end
end
function SevenModelPanel:Awake()
    UIWFlushPanel.Awake(self)
    local stuff_id = self.data.end_reward_data.reward_item[0].item_id
    if Item.GetType(stuff_id) == ItemType.Fashion then
        local show_cfg = self.data:GetMountShowCfgByStuffId(stuff_id)
        self.ui_obj:SetData(DrawerHelper.GetRidePath(show_cfg.res_id))
        UH.SetText(self.NameText,show_cfg.surface_name)
        UH.SetText(self.TypeText,Language.Illusion.FashionTypeName[show_cfg.type])
    elseif Item.GetType(stuff_id) == ItemType.Skin then
        local show_path,show_cfg = self.data:GetSkinPath(stuff_id)
        self.ui_obj:SetData(show_path)
        UH.SetText(self.NameText,show_cfg.name)
        UH.SetText(self.TypeText,Language.OpenAct.SevenDay.Skin)
    elseif Item.GetType(stuff_id) == 33 then
        local show_path,show_cfg = self.data:GetPartnerPath(stuff_id)
        self.ui_obj:SetData(show_path)
        UH.SetText(self.NameText,show_cfg.name)
        UH.SetText(self.TypeText,Language.Partner.Partner)
    end
end

function SevenModelPanel:OnDestroy()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
    UIWFlushPanel.OnDestroy(self)
end

--===============================SevenDayMenuPanel===============================
SevenDayMenuPanel = SevenDayMenuPanel or DeclareMono("SevenDayMenuPanel", UIWFlushPanel)

function SevenDayMenuPanel:SevenDayMenuPanel()
    self.data = SevenDayData.Instance
    self.data_cares = {
        {data = self.data.reward_data,func = self.FlushMenuView,init_call = false},
        {data = self.data.goal_data,  func = self.FlushMenuView,init_call = false},
        {data = self.data.info_data,func = self.FlushMenuView,init_call = true},
        {data = self.data.info_data,func = self.FlushDescView,init_call = true},
        {data = RoleData.Instance:GetBaseData(), func = self.FlushItemView, init_call = false, keys = {"level"}},
        {data = RoleData.Instance:GetBaseData(), func = self.FlushDescView, init_call = false, keys = {"level"}},
    }
end

function SevenDayMenuPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushMenuView()
end

function SevenDayMenuPanel:FlushItemView()
    if FlyUpData.Instance:IsShowed() then
        self.BaseItem:SetActive(false)
        return
    end
    local is_open = self.data:IsSevenDayOpen()
    if GetActive(self.BaseItem) ~= is_open then
        TaskData.Instance:CheckMenuGuideFlush()
    end
    self.BaseItem:SetActive(is_open)
end

function SevenDayMenuPanel:FlushMenuView()
    if FlyUpData.Instance:IsShowed() then
        self.BaseItem:SetActive(false)
        return
    end
    self:FlushItemView()
    self:FlushNameView()
    self:FlushProgress()
end

function SevenDayMenuPanel:FlushProgress()
    local task_num,target_num = self.data:GetTaskNum()
    self.Progress:SetProgress(task_num / target_num)
    self.Progress:SetText(Format(Language.OpenAct.SevenDay.ProgressTip,task_num,target_num))
end

function SevenDayMenuPanel:FlushNameView()
    local type = self.data:GetDayType()
    local config = ActivityRandData.Instance:GetConfigByKey(Mod.RandActivity.SevenDay)
    local text_list = Split(config.text,"|")
    local data = text_list[type+1]
    if data ~= nil then
        UH.SetText(self.Name,Format("[%s]",data or ""))
        UH.SpriteName(self.ImgSet[1],[[SevenDayI]]..type)
        UH.SpriteName(self.ImgSet[2],[[SevenDayB]]..type)
        UH.SpriteName(self.ImgSet[3],type == 0 and [[SevenDayL0]] or "")
        UH.SpriteName(self.ImgSet[4],type == 1 and [[SevenDayL1]] or "")
        UH.SpriteName(self.ImgSet[5],type == 1 and [[SevenDayR1]] or "")
    end
end

function SevenDayMenuPanel:FlushDescView()
    if FlyUpData.Instance:IsShowed() then
        self.BaseItem:SetActive(false)
        return
    end
    local is_open = self.data:IsSevenDayOpen()
    if not is_open then
        self.DescGame:SetActive(false)
        return
    end
    local config = DataHelper.GetSearchCond(Config.randactivityconfig_1_auto.mysterious_trial_popup,function(data)
        return data.type == self.data:GetDayType() and data.grade == RoleLevel()
    end)
    if config then
        UH.SetText(self.Desc,config.describe)
    end
    if self.role_level ~= RoleLevel() then
        self.DescGame:SetActive(config ~= nil)
        self.role_level = RoleLevel()
    end
end

function SevenDayMenuPanel:OnClickItem()
    ViewMgr:OpenViewByKey(Mod.RandActivity.SevenDay)
    self.DescGame:SetActive(false)
end

function SevenDayMenuPanel:OnClickClose()
    self.DescGame:SetActive(false)
end