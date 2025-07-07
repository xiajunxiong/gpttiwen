TreasureHuntXunQiPanel = TreasureHuntXunQiPanel or DeclareMono("TreasureHuntXunQiPanel", UIWFlushPanel)
function TreasureHuntXunQiPanel:TreasureHuntXunQiPanel()
    self.data_instance = TreasureHuntData.Instance
    self.model_index = 0
    self.data_cares = {
        {data = self.data_instance.smart_data, func = self.FlushShow, keys = {"cur_treasure_id"}, init_call = false},
        {data = self.data_instance.treasure_group_times, func = self.FlushShow, init_call = false}
    }
end

function TreasureHuntXunQiPanel:Awake()
    UIWFlushPanel.Awake(self)
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = self,
                show_shadow = false,
			}
		)
    end
    self.shengqi_is_open = self.data_instance:JudgeXunQiShengQiIsOpen()
    self:CalculateCurId()
    self:FlushShow()
end

-- 第一次计算一遍选中的treasure_id
function TreasureHuntXunQiPanel:CalculateCurId()
    if not self.cur_treasure_id then
        local list = self.data_instance:GetOpenedTreasureID()
        local show_list = {}
        for k, v in pairs(list) do
            if TreasureHuntData.Instance:GetTreasureHuntConfig(v.treasure_id).treasure_type == HUNT_TYPE.XUN_QI then
                local xunqi_type = self.data_instance:XunQiType(v.treasure_id)
                if xunqi_type == XUN_QI_TYPE.HUN_QI then
                    table.insert(show_list, v)
                elseif xunqi_type == XUN_QI_TYPE.SHENG_QI then
                    if self.data_instance:GetXunQiShengQiIsOpen(v.treasure_id) then
                        table.insert(show_list, v)
                    end
                end
            end
        end
        if TableIsEmpty(show_list) then
            LogDG("寻器功能未开启")
            return
        end
        local auto_select = self.data_instance:GetAutoSelectXunQi(show_list)
        self.cur_treasure_id = show_list[auto_select].treasure_id
        self.data_instance:SetCurSelectId(self.cur_treasure_id)
        if not self.shengqi_is_open then
            local show_item_list = self.data_instance:GetXunQiPreviewList()
            self.RewardPreviewList:SetData(show_item_list)
        else
            self.ToggleList:SetDataList(show_list, auto_select)
        end
        self.ObjOnlyHunQi:SetActive(not self.shengqi_is_open)
        self.ObjRewardShow:SetActive(self.shengqi_is_open)
    end
end

function TreasureHuntXunQiPanel:FlushModel()
    self.model_index = self.model_index + 1
    if self.model_index > #self.model_data then
        self.model_index = 1
    end
    local model_id = self.model_data[self.model_index].show_res
    self.ui_obj:SetData(DrawerHelper.GetWeaponPath(model_id))
    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        self:FlushModel()
    end, 30)
end

function TreasureHuntXunQiPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if self.cur_treasure_id then
        self.data_instance:SetCurSelectId(self.cur_treasure_id)
    end
    self.data_instance.smart_data.hunt_type = HUNT_TYPE.XUN_QI
end

function TreasureHuntXunQiPanel:FlushShow()
    if self.cur_treasure_id then
        self.xunqi_type = self.data_instance:XunQiType()
        if self.xunqi_type > 0 then
            self.model_data = self.data_instance:XunQiShowMod()
            self:FlushLeftTimes()
            self:FlushModel()
            self:SetTimeStemp()
        end
    end
end

function TreasureHuntXunQiPanel:FlushLeftTimes()
    if self.data_instance:GetLeftTimes() ~= nil then
        local notice_str = Format(Language.TreasureHunt.XunQiHuntRemainXunQi[self.xunqi_type], self.data_instance:GetLeftTimes())
        UH.SetText(self.XunQiNoticeText, notice_str)
    else
        UH.SetText(self.XunQiNoticeText, "")
    end 
end

function TreasureHuntXunQiPanel:SetTimeStemp()
    local cur_config = self.data_instance:GetCurSelectCfg()
    if cur_config.is_limit == 0 or cur_config.treasure_type ~= HUNT_TYPE.XUN_QI then
        self.TimeMeter:CloseCountDownTime()
        self.TimeMeter:SetTime("")
    else
        local server_time_date = TimeCtrl.Instance:GetServerDate()
        self.start_timestamp = os.time({year = server_time_date.year, month = server_time_date.month, day = server_time_date.day , hour = cur_config.day_start_time / 100, min=0, sec=0})
        self.end_timestamp = os.time({year = server_time_date.year, month = server_time_date.month, day = server_time_date.day , hour =cur_config.day_end_time / 100, min=0, sec=0})
        local timestemp = self.end_timestamp
        --当天开启时间点前
        if server_time_date.hour < cur_config.day_start_time / 100 then
            timestemp = self.start_timestamp
        
        --当天结束时间点前
        elseif server_time_date.hour < cur_config.day_end_time / 100 then
            timestemp = self.end_timestamp
        
        --当天结束时间点后
        else
            self.start_timestamp = self.start_timestamp + 86400
            timestemp = self.start_timestamp
        end
        self.TimeMeter:SetCallBack(function ()
            self:SetTimeStemp()
        end)
        self.TimeMeter:SetShowCallBack(function (t)
            local server_time = TimeCtrl.Instance:GetServerTime()
            if server_time < self.start_timestamp or server_time > self.end_timestamp  then
                return Format(Language.TreasureHunt.XunQiOpenCountDown, t.hour, t.min, t.s)
            else
                return Format(Language.TreasureHunt.XunQiCloseCountDown, t.hour, t.min, t.s)
            end
        end)
        self.TimeMeter:StampTime(timestemp)
    end
end

function TreasureHuntXunQiPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
    end
    self.data_instance:ResetCurSelectId()
    self.data_instance.smart_data.hunt_type = HUNT_TYPE.HUAN_LING
    TimeHelper:CancelTimer(self.time_ht)
end

function TreasureHuntXunQiPanel:OnRewardShowClick()
    ViewMgr:OpenView(TreasureHuntXunQiRewardShowView)
end

function TreasureHuntXunQiPanel:OnClickToggle(value, mono)
    if value then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
        self.data_instance:SetCurSelectId(mono:GetData().treasure_id)
        self.cur_treasure_id = mono:GetData().treasure_id
    end
end

----------------------TreasureHuntXunQiToggleItem----------------------
TreasureHuntXunQiToggleItem = TreasureHuntXunQiToggleItem or DeclareMono("TreasureHuntXunQiToggleItem", UIWidgetBaseItem)
function TreasureHuntXunQiToggleItem:TreasureHuntXunQiToggleItem()
    self.data_instance = TreasureHuntData.Instance
end

function TreasureHuntXunQiToggleItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local xunqi_type = TreasureHuntData.Instance:XunQiType(data.treasure_id)
    UH.SpriteName(self.Img, "TogXunQiImg" .. xunqi_type)
    UH.SpriteName(self.Img2, "TogXunQiImg" .. xunqi_type)
    UH.SpriteName(self.TogNameImg, "_LocTogXunQiName" .. xunqi_type)
    UH.SpriteName(self.TogNameImg2, "_LocTogXunQiName" .. xunqi_type)
end

function TreasureHuntXunQiToggleItem:Click()
    self.Selector:Trigger()
end

----------------------------TreasureHuntXunQiRewardShowView----------------------------
TreasureHuntXunQiRewardShowView = TreasureHuntXunQiRewardShowView or DeclareView("TreasureHuntXunQiRewardShowView","treasure_hunt/treasure_hunt_xun_qi_reward_show")
VIEW_DECLARE_MASK(TreasureHuntXunQiRewardShowView,ViewMask.BgBlock)
function TreasureHuntXunQiRewardShowView:LoadCallback(param)
    local show_item_list = TreasureHuntData.Instance:GetXunQiPreviewList()
    self.RewardPreviewList:SetData(show_item_list)
end

function TreasureHuntXunQiRewardShowView:OnCloseClick()
    ViewMgr:CloseView(TreasureHuntXunQiRewardShowView)
end