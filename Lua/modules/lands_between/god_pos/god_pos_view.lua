GodPosView = GodPosView or DeclareView("GodPosView", LandsBetweenConfig.PrefabPath.."god_pos/god_pos", Mod.LandsBetweenExtra.GodPos)
VIEW_DECLARE_LEVEL(GodPosView, ViewLevel.L1)
VIEW_DECLARE_MASK(GodPosView, ViewMask.BgBlock)
function GodPosView:GodPosView()
    self.Board:SetData(self:GetType(), Language.GodPos.Title[1])
end

function GodPosView:LoadCallback(param_t)
    
end

function GodPosView:CloseCallback()

end

function GodPosView:OnCloseClick()
    ViewMgr:CloseView(GodPosView)
end

function GodPosView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[393].desc)
end

----------------------------GodPosPanel----------------------------
GodPosPanel = GodPosPanel or DeclareMono("GodPosPanel", UIWFlushPanel)
function GodPosPanel:GodPosPanel()
    self.Data = GodPosData.Instance
    self.language = Language.GodPos
    self.data = self.Data.info_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
    --抛物线1
    self.ParabolaBoard:SetData({y_offset = 2, speed_time = 2, complete_func = function()
        self.ObjBigEffect:SetActive(false)
        self.ObjBigEffect:SetActive(true)
        self.ObjBlock:SetActive(false)
        self:SendTaskFinish()
    end})
end

function GodPosPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushAll()
end

function GodPosPanel:FlushAll()
    self.now_task_data = nil
    self.ObjBlock:SetActive(false)
    self.min_seq = self.Data:PosSeq()
    if self.last_seq ~= self.min_seq or not self.last_seq  then
        -- 只允许看到下一级的
        local max_seq = self.Data:MaxPosSeq()
        local max = self.min_seq + 1
        self.max_seq = math.min(max_seq, max)
        self.now_show_seq = self.min_seq
        -- 记录当前seq，如果升级了神位，就要变
        self.last_seq = self.Data:PosSeq()
    end

    self:OnChangeClick(0)
    self.task_data = self.Data:TaskData(self.now_show_seq)
    self.TaskList:SetData(self.task_data)
    local task_num = #self.task_data
    self.ScrollRect.enabled = task_num > 5
    if task_num ~=0  then
        self.Progress:SetProgress(self.Data:TaskFinishNum() / task_num)
        self.Progress:SetText(Format(Language.Common.Slash, self.Data:TaskFinishNum(), task_num));
        self.ObjGet:SetActive(self.Data:TaskFinishNum() == task_num);
    else
        self.ObjReward:SetActive(false);
        self.ObjMax:SetActive(true);
    end
end

function GodPosPanel:OnChangeClick(value)
    self.now_show_seq = self.now_show_seq + value
    if self.now_show_seq < self.min_seq then
        self.now_show_seq = self.min_seq
    end
    if self.now_show_seq > self.max_seq then
        self.now_show_seq = self.max_seq
    end
    local cfg = self.Data:GodPosCfg(self.min_seq)
    local show_cfg = self.Data:GodPosCfg(self.now_show_seq)
    UH.SpriteName(self.Icon, "ShenWei" .. self.now_show_seq)
    UH.SetText(self.TxtName, show_cfg.name)
    self.ObjBtnLeft:SetActive(self.now_show_seq ~= self.min_seq)
    self.ObjBtnRight:SetActive(self.now_show_seq ~= self.max_seq)

    local ap_max = show_cfg.ap_max - cfg.ap_max
    UH.SetText(self.TxtApMax, ap_max > 0 and Format(self.language.Desc[1], cfg.ap_max, ap_max) or cfg.ap_max)
    local ap_recover_s = show_cfg.ap_recover_s - cfg.ap_recover_s
    UH.SetText(self.TxtRecover, ap_recover_s < 0 and Format(self.language.Desc[2], cfg.ap_recover_s.."s", ap_recover_s) or cfg.ap_recover_s.."s")
    local occupy = show_cfg.occupy - cfg.occupy
    UH.SetText(self.TxtCount, occupy > 0 and Format(self.language.Desc[1], cfg.occupy, occupy) or cfg.occupy)
    local altar_level = show_cfg.altar_level - cfg.altar_level
    UH.SetText(self.TxtOccupy, altar_level > 0 and Format(self.language.Desc[1], cfg.altar_level, altar_level) or cfg.altar_level)
end

function GodPosPanel:OnIconClick()
    ViewMgr:OpenView(GodPosListShowView)
end

function GodPosPanel:OnBoxClick()
    ViewMgr:OpenView(GodPosListShowView)
end

function GodPosPanel:OnGetClick(mono)
    self.now_task_data = mono:GetData()
    self.ObjStartPos.transform.position = mono:GetStartPos()
    self.ObjBlock:SetActive(true)
    self.ParabolaBoard:Play()
end

function GodPosPanel:SendTaskFinish()
    if self.now_task_data then
        GodPosCtrl.Instance:SendReq(self.now_task_data.task_id)
    end
end

function GodPosPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

--------------GodPosTaskItem--------------
GodPosTaskItem = GodPosTaskItem or DeclareMono("GodPosTaskItem", UIWidgetBaseItem)
function GodPosTaskItem:GodPosTaskItem()
    self.Data = GodPosData.Instance
end

function GodPosTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.is_can_get = self.Data:IsTaskCanGet(data)
    UH.SetText(self.TxtDesc, data.task_desc)
    local param = self.Data:TaskParam(data)
    self.Progress:SetProgress(param / data.need_num)
    if data.need_num ~=0 then
        self.Progress:SetProgress(param / data.need_num)
        self.Progress:SetText(Format(Language.Common.Slash, param, data.need_num))
    end
    -- self.Cell:SetData(Item.Create(data.task_reward))
    self.RewardsList:SetData(data.rewards)
    self.ListRect.enabled = #data.rewards > 1
    self.ObjGet:SetActive(self.is_can_get)
    self.Remind:SetNum(self.is_can_get and 1 or 0)
    self.ObjGeted:SetActive(self.Data:IsTaskGeted(data))
    self.ObjCannotGet:SetActive(self.Data:TaskFlag(data) == 0)
end

function GodPosTaskItem:GetStartPos()
    return self.ObjGet.transform.position
end

---------------------- 神位主界面标签 ---------------------
GodPosMainSubPanel = GodPosMainSubPanel or DeclareMono("GodPosMainSubPanel", UIWFlushPanel)
function GodPosMainSubPanel:GodPosMainSubPanel()
    self.data = GodPosData.Instance
    self.data_cares = {
        {data = self.data.info_data, func = self.FlushPanel},
        {data = self.data.info_data, func = self.RedFlush},
    }
end 

function GodPosMainSubPanel:FlushPanel()
    local cur_seq = self.data:PosSeq()
    local show_cfg = self.data:GodPosCfg(cur_seq)

    local task_data = self.data:TaskData(cur_seq)
    local task_num = #task_data

    UH.SpriteName(self.GodHeadShow,"ShenWei" ..cur_seq)
    UH.SetText(self.GodHeadDesc,show_cfg.name)
    UH.SetText(self.GodDesc,show_cfg.name)
    if task_num ~=0 then
        self.GodProgress:SetProgress(self.data:TaskFinishNum() / task_num)
        self.GodProgress:SetText(Format(Language.Common.Slash, self.data:TaskFinishNum(), task_num)) ;
    else
        self.GodProgress:SetObjActive(false);  
        self.ObjMax:SetActive(true);  
    end
end

function GodPosMainSubPanel:OnClickHead()
    ViewMgr:OpenView(GodPosView)
end

function GodPosMainSubPanel:RedFlush()
    if self.RedPos ~= nil then 
        self.red_point = self.red_point or UiRedPoint.New(self.RedPos, Vector3.New(0,0,0))
        self.red_point:SetNum(self.data:TaskCanGetNum() > 0 and 1 or 0)
    end 
end