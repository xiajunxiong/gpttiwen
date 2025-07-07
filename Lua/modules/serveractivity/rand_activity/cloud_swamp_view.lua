CloudSwampView = CloudSwampView or DeclareView("CloudSwampView","serveractivity/cloud_swamp_view",Mod.RandActivity.CloudSwamp)

VIEW_DECLARE_LEVEL(CloudSwampView, ViewLevel.L1)

function CloudSwampView:CloudSwampView()
    self.data = ServerActivityData.Instance
    self.handle = BagData.Instance:Care(BindTool.Bind(self.OnFlush,self))
end

function CloudSwampView:OnClickClose()
    ViewMgr:CloseView(CloudSwampView)
end

function CloudSwampView:CloseCallback()
    local data = self.select_data or {}
    self.data:SetCloudSwampSelectIndex(data.seq + 1)
    BagData.Instance:UnCare(self.handle)
    Delete(self,[[ModelGame]])
end

function CloudSwampView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.zaizhanyunze_auto.other[1].desc or "")
end

function CloudSwampView:LoadCallback(param_t)
    local index = self.data:GetCloudSwampIndex()
    self.List:SetDataList(Config.zaizhanyunze_auto.basics,index)
    if index and index > 4 then
        self.List:SetCompleteCallBack(function()
            self.List:ForceJumpVertical(128 * (index - 4))
        end)
    end
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_CLOUD_SWAMP,TimeType.Type_Special_4,Language.CloudSwamp.TimeTip)

    local act_name = ActivityRandData.GetName(ACTIVITY_TYPE.RAND_CLOUD_SWAMP)
    UH.SetText(self.Title1,string.sub(act_name,1,3))
    UH.SetText(self.Title2,string.sub(act_name,4,-1))
end

function CloudSwampView:OnClickItem(data)
    UH.SetText(self.Name,data.item:Name())
    local times = self.data:GetCloudSwampTimes(data.seq)
    local section_num = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_CLOUD_SWAMP)
    self.FirstItem:SetData({reward_id = data.reward1,is_reward = times > 0})
    self.RewardItem:SetData(self.data:GetCloudSwampRewardList(data,times))
    self.RateItem:SetData({reward_id = data.reward2,is_rate = true})
    self:FlushModelView(data.battle_icon)
    self:FlushNumView(data.cost_item or 1)
    UH.SetInter(self.BtnInter,section_num >= data.section_start)
    self.select_data = data
end

function CloudSwampView:OnFlush()
    if self.select_data then
        self:FlushNumView(self.select_data.cost_item or 1)
    end
end

function CloudSwampView:FlushModelView(monster_id)
    local npc_config = Cfg.MonsterById(monster_id)
    local path = DrawerHelper.GetNpcPath(npc_config.res_id)
    if not self.ModelGame then
        self.ModelGame = UIChDrawer.New()
        self.ModelGame:SetMainSolo(path)
        self.ModelShow:SetShowData({ui_obj = self.ModelGame, view = self})
    else
        self.ModelGame:SetMainSolo(path)
    end
end

function CloudSwampView:FlushNumView(need_num)
    local item_id = Config.zaizhanyunze_auto.other[1].cost_item
    UH.SetIcon(self.Icon,item_id,ICON_TYPE.ITEM)
    UH.SetText(self.Num,DataHelper.ConsumeNum(Item.GetNum(item_id),need_num))
    UH.SetText(self.BtnName,Language.CloudSwamp.BtnName[Item.GetNum(item_id) >= need_num and 1 or 2])
end

function CloudSwampView:OnClickEnter()
    local data = self.select_data or {}
    if not self.BtnInter.Interactable then
        PublicPopupCtrl.Instance:Error(Language.CloudSwamp.NotEnterTip)
        return
    end
    if Item.GetNum(Config.zaizhanyunze_auto.other[1].cost_item) < data.cost_item then
        local config = ServerActivityData.Instance:GetOpenConfig(ACTIVITY_TYPE.RAND_CLOUD_SWAMP,Config.zaizhanyunze_auto.jump)
        ViewMgr:OpenViewByKey(config[1].mod_key)
        return
    end
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_CLOUD_SWAMP,0,data.seq)
    ViewMgr:CloseView(CloudSwampView)
end

----------------------------CloudSwampItem----------------------------
CloudSwampItem = CloudSwampItem or DeclareMono("CloudSwampItem", UIWidgetBaseItem)
function CloudSwampItem:SetData(data)
    data.item = Monster.New({item_id = data.battle_icon,quality_name = "HuoBanPinZhiKuangHong"})
    self.Call:SetData(data.item)
    UH.SetText(self.Name,data.item:Name())
    UH.SetText(self.Desc,Format(Language.CloudSwamp.Title,DataHelper.GetDaXie(data.section_start)))
    local section_num = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_CLOUD_SWAMP)
    if section_num < data.section_start then
        local server_time = TimeHelper.GetServerTime()
        local need_time = data.section_start - section_num
        local stamp_time = TimeCtrl.Instance:ChangDataTime(server_time,need_time)
        self.Image:SetActive((stamp_time - server_time) <= TIME_DAY_SECOND_NUM)
        self.LimitTime:StampTime(stamp_time,TimeType.Type_Time_7)
    else
        self.Label:SetActive(ServerActivityData.Instance:GetCloudSwampTimes(data.seq)==0)
        self.LimitTime:SetTime("")
    end
    self.Mask:SetActive(section_num < data.section_start)
    UIWidgetBaseItem.SetData(self, data)
end

function CloudSwampItem:Click()
    self.ToggleClick:Trigger()
end

----------------------------CloudSwampRewardItem----------------------------
CloudSwampRewardItem = CloudSwampRewardItem or DeclareMono("CloudSwampRewardItem", UIWidgetBaseItem)
function CloudSwampRewardItem:SetData(data)
    if data.boss_times and data.times then
        local times = Valve(data.times,data.boss_times)
        UH.SetText(self.Desc,Format(Language.CloudSwamp.BossTimesTip,data.boss_times,times,data.boss_times))
    end
    if data.is_rate == true then
        self.Grid:SetData(ServerActivityData.Instance:GetCloudSwampRewardRateData(data.reward_id))
    else
        self.Grid:SetData(ServerActivityData.Instance:GetCloudSwampRewardData(data.reward_id))
    end
    self.HasRewardTip:SetActive(data.is_reward == true)
end