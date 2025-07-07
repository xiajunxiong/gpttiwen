SweetSaltyWarView = SweetSaltyWarView or DeclareView("SweetSaltyWarView", "serveractivity/sweet_salty_war/sweet_salty_war", Mod.DragonBoat.SweetSaltyWar)
VIEW_DECLARE_LEVEL(SweetSaltyWarView, ViewLevel.L1)
VIEW_DECLARE_MASK(SweetSaltyWarView, ViewMask.BgBlock)
function SweetSaltyWarView:SweetSaltyWarView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SWEET_SALTY_WAR, TimeType.Type_Time_3)
    SweetSaltyWarCtrl.Instance:SendReq(SweetSaltyWarReq.Info)
end

function SweetSaltyWarView:LoadCallback(param_t)
    
end

function SweetSaltyWarView:CloseCallback()

end

function SweetSaltyWarView:OnCloseClick()
    ViewMgr:CloseView(SweetSaltyWarView)
end

function SweetSaltyWarView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[324].desc)
end

----------------------------SweetSaltyWarPanel----------------------------
SweetSaltyWarPanel = SweetSaltyWarPanel or DeclareMono("SweetSaltyWarPanel", UIWFlushPanel)
function SweetSaltyWarPanel:SweetSaltyWarPanel()
    self.Data = SweetSaltyWarData.Instance
    self.language = Language.SweetSaltyWar
    self.data = self.Data.info_data
    self.item_id = self.Data:ConsumeItem()
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems({self.item_id}, BindTool.Bind(self.FlushAll, self))
    end
end

function SweetSaltyWarPanel:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetText(self.TxtTip, self.Data:TopDesc())
    UH.SetIcon(self.Icon, Item.GetIconId(self.item_id), ICON_TYPE.ITEM)
    self:FlushAll()
end

function SweetSaltyWarPanel:FlushAll()
    local has_num = Item.GetNum(self.item_id)
    local set_flush = function(List, select_type)
        local remind = self.Data:SingleRemind(select_type)
        List[2].Interactable = remind > 0
        List[4]:SetNum(remind)
        List[5]:SetActive(remind > 0)
        UH.SetText(List[1], Format(self.language.Ticket[1], self.data.info.value[select_type]))
        if self.data.info.state == SweetSaltyWarState.Vote then
            UH.SetText(List[3], self.language.Btn[1])
        elseif self.data.info.state == SweetSaltyWarState.Count then
            UH.SetText(List[1], self.language.Ticket[2])
            UH.SetText(List[3], self.language.Btn[2])
        elseif self.data.info.state == SweetSaltyWarState.Result then
            UH.SetText(List[3], self.language.Btn[self.data.reward_flag == 0 and 3 or 4])
        end
    end
    set_flush(self.Left, SweetSaltyWarVote.Sweet)
    set_flush(self.Right, SweetSaltyWarVote.Salty)
    UH.SetText(self.TxtNum, has_num)
    local prog_num = self.data.info.value[2] / self.data.info.value[0]
    -- 计票的时候进度条放中间
    if self.data.info.value[0] == 0 or self.data.info.state == SweetSaltyWarState.Count then
        prog_num = 0.5
    end
    self.Progress:SetProgress(prog_num)
    UH.AnchoredPosition(self.RectEffectProg, Vector2.New(prog_num * 684, 0))

    self.ObjResultshow:SetActive(self.data.info.state == SweetSaltyWarState.Result)
    if self.data.info.state == SweetSaltyWarState.Result then
        self.Results[1]:SetActive(self.data.info.value[1] > self.data.info.value[2])
        self.Results[2]:SetActive(self.data.info.value[1] < self.data.info.value[2])
    end
end

function SweetSaltyWarPanel:OnShowClick()
    ViewMgr:OpenView(SweetSaltyWarShowRewardView)
end

function SweetSaltyWarPanel:OnVoteClick(select_type)
    local remind = self.Data:SingleRemind(select_type)
    if self.data.info.state == SweetSaltyWarState.Vote then
        if remind > 0 then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
            if self.data.select_type == 0 then
                PublicPopupCtrl.Instance:AlertTip(self.language.FirstTip[select_type], function()
                    SweetSaltyWarCtrl.Instance:SendReq(SweetSaltyWarReq.Vote, select_type)
                end)
            else
                SweetSaltyWarCtrl.Instance:SendReq(SweetSaltyWarReq.Vote, select_type)
            end
        else
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
            if self.data.select_type == 0 then
                self:OnItemClick()
            else
                if self.data.select_type ~= select_type then
                    PublicPopupCtrl.Instance:Center(self.language.SelectTip[1])
                else
                    self:OnItemClick()
                end
            end
        end
    elseif self.data.info.state == SweetSaltyWarState.Count then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        PublicPopupCtrl.Instance:Center(self.language.Ticket[2])
    elseif self.data.info.state == SweetSaltyWarState.Result then
        if remind > 0 then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
            SweetSaltyWarCtrl.Instance:SendReq(SweetSaltyWarReq.Reward)
        else
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
            if self.data.select_type ~= select_type then
                PublicPopupCtrl.Instance:Center(self.language.SelectTip[2])
            else
                PublicPopupCtrl.Instance:Center(self.language.Btn[4])
            end
        end
    end
end

function SweetSaltyWarPanel:OnItemClick()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.item_id})})
end

function SweetSaltyWarPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    BagData.Instance:RemoveListenItems(self.item_listen)
end

----------------------------SweetSaltyWarShowRewardView----------------------------
SweetSaltyWarShowRewardView = SweetSaltyWarShowRewardView or DeclareView("SweetSaltyWarShowRewardView", "serveractivity/sweet_salty_war/sweet_salty_war_show_reward")
VIEW_DECLARE_MASK(SweetSaltyWarShowRewardView, ViewMask.BgBlock)
function SweetSaltyWarShowRewardView:LoadCallback(param_t)
    local set_list = function(List, list)
        -- 因为下标从0开始所以要+1
        local num = #list + 1
        local list1, list2, list3 = {}, {}, {}
        if num > 3 then
            local first_num = num / 2
            first_num = first_num > math.floor(first_num) and math.floor(first_num) or first_num
            local second_num = num - first_num
            for i = 0, num - 1 do
                if i <= (first_num - 1) then
                    table.insert(list2, list[i])
                else
                    table.insert(list3, list[i])
                end
            end
        else
            list1 = list
        end
        List[1]:SetData(DataHelper.FormatItemList(list1))
        List[2]:SetData(DataHelper.FormatItemList(list2))
        List[3]:SetData(DataHelper.FormatItemList(list3))
    end
    local cfg = SweetSaltyWarData.Instance:BaseCfg()
    set_list(self.LeftList, cfg.suc_reward_item)
    set_list(self.RightList, cfg.fail_reward_item)
end

function SweetSaltyWarShowRewardView:OnCloseClick()
    ViewMgr:CloseView(SweetSaltyWarShowRewardView)
end