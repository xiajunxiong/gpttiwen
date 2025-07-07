CompetitionRankView = CompetitionRankView or DeclareView("CompetitionRankView", "serveractivity/competition_rank_view", Mod.RandActivity.CompetitionRank)

VIEW_DECLARE_LEVEL(CompetitionRankView, ViewLevel.L0)
function CompetitionRankView:LoadCallback()
    CompetitionRanklCtrl.Instance:SendReqAllRankInfo()
end
function CompetitionRankView:OnClickClose()
    ViewMgr:CloseView(CompetitionRankView)
end

function CompetitionRankView:CompetitionRankView()
    CompetitionRanklCtrl.Instance:SendCompetitionRankType(ServeractivityConfig.CompetitionRank.Type.ArenaRank)
end

--===========================CompetitionRankPanel===========================
CompetitionRankPanel = CompetitionRankPanel or DeclareMono("CompetitionRankPanel", UIWFlushPanel)
function CompetitionRankPanel:CompetitionRankPanel()
    self.data = CompetitionRanklData.Instance
    self.data_cares = {
        {data = self.data.info_data, func = self.OnFlush, init_call = false,keys = {"info"}},
        {data = self.data.info_data, func = self.OnFlush, init_call = false,keys = {"flush"}},
        {data = self.data.rank_list, func = self.OnFlush, init_call = false},
        {data = self.data.info_data, func = self.FlushModelView, init_call = false,keys = {"role"}},
        {data = SocietyData.Instance.rank_all_data,func = self.OnFlush,init_call = false},
    }
end
function CompetitionRankPanel:OnClickTip()
    PublicPopupCtrl.Instance:HelpTip(336)
end

function CompetitionRankPanel:Awake()
    UIWFlushPanel.Awake(self)
    local list = self.data:GetBtnList()
    self.BtnList:SetDataList(list)
    self:OnClickItem(list[1])
    self:FlushRewardGiftView()
end

function CompetitionRankPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self,[[ModelGame]])
end

function CompetitionRankPanel:OnFlush()
    self:FlushRankView()
    self:FlushRewardView()
    self:FlushRewardGiftView()
    self:FlushModelView()
    for k,item in pairs(self.BtnList.item_list) do
        item:FlushRedNum()
    end
end

function CompetitionRankPanel:OnClickItem(data)
    self.data:SetSelectData(data)
    CompetitionRanklCtrl.Instance:SendCompetitionRankType(data.type)
    self.Time:StampTime(data.time,TimeType.Type_Time_8,Language.CompetitionRank.TimeTip)
    self:FlushRankView()
    self:FlushRewardView()
    self:FlushModelView()
end

function CompetitionRankPanel:FlushModelView()
    local role_info = self.data:GetShowRoleData()
    local send_role_id = self.data:GetSelectRoleId()
    if role_info ~= nil and role_info.role_id == send_role_id or self.data:GetSelectType() == 4 then
        if not self.ModelGame then
            self.ModelGame = UIChDrawer.New()
            self.ModelShow:SetShowData({ui_obj = self.ModelGame, view = self})
        else
            if role_info.appearance then
                self.ModelGame:SetData(role_info.appearance)
            end
        end
        UH.SetText(self.Name,role_info.role_name)
        if self.data:GetTitleName(self.data:GetSelectType()) == nil then
            self.TitleImg:SetObjActive(false)
        else
            UH.SetIcon(self.TitleImg,self.data:GetTitleName(self.data:GetSelectType()))
            self.TitleImg:SetObjActive(true)
        end
        self.RightObj:SetActive(true)
        self.NoneFirst:SetActive(false)
    else
        self.RightObj:SetActive(false)
        self.NoneFirst:SetActive(true)
    end
end

function CompetitionRankPanel:FlushRewardView()
    local reward_list,list = self.data:GetGiftList(self.data:GetSelectType())
    local progress_num = self.role_info and self.role_info.rank_value or 0
    if self.data:GetSelectType() == ServeractivityConfig.CompetitionRank.Type.ArenaRank then
        progress_num = progress_num * -1
    end
    self.Progress:SetProgress(DataHelper.GetFillAmount(progress_num,list))
    self.GiftList:SetData(reward_list)
end

function CompetitionRankPanel:FlushRankView()
    UH.SetText(self.TabScore,Language.CompetitionRank.TabScore[self.data:GetSelectType()] or Language.CompetitionRank.TabScore[1])
    local rank_list = self.data:GetRankList(self.data:GetSelectType())
    UH.SetText(self.Name,#rank_list == 0 and Language.TombRaider.WithoutPlayer or "")
    local send_role = rank_list[1]
    for i, v in ipairs(rank_list) do
        if v.uid ~= 0 then
            send_role = v
            self.data.select_type_show_model_index[self.data:GetSelectType()] = i
            break
        end
    end
    CompetitionRanklCtrl.Instance:SendCompetitionRoleSimpleInfo(send_role)--(rank_list[1])
    self.role_info = SocietyData.GetRoleRankInfo(rank_list)
    
    local info = self.data:GetRankInfo(self.data:GetSelectType())
    if info ~= nil and self.role_info.rank_value == 0 then
        self.role_info.rank_value = info.cur_value
    end
    if self.data:GetSelectType() == ServeractivityConfig.CompetitionRank.Type.ArenaRank then
        self.role_info.rank = ArenaCtrl.Instance.data.RoleInfo.rank
    end
    self.NotHasTip:SetActive(#rank_list == 0)
    self.RoleItem:SetData(self.role_info)
    self.RankList:SetData(rank_list)
end

function CompetitionRankPanel:OnClickEnter()
    local mod_key = ServeractivityConfig.CompetitionRank.OpenPanel[self.data:GetSelectType()]
    if self.data:GetSelectType() == ServeractivityConfig.CompetitionRank.Type.Comprehensive then
        BianQiangCtrl.Instance:OpenByGroupId(BianQiangGroupId.TYPE_ROLE_UP)
        else
        ViewMgr:OpenViewByKey(mod_key)
    end
    ViewMgr:CloseView(CompetitionRankView)
end

function CompetitionRankPanel:FlushRewardGiftView()
    self.Received:SetActive(self.data:IsFetchDayGiftFlag()==1)
    self.RewardEffect:SetActive(self.data:IsFetchDayGiftFlag()==0)
    self.RewardPiont:SetActive(self.data:IsFetchDayGiftFlag()==0)
end

function CompetitionRankPanel:OnClickReward()
    if GetActive(self.RewardEffect) then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.KF_COMPETITION_RANK, 3)
    else
        local pos = UH.GetRootPosition(self.RewardEffect)
        local reward_list = DataHelper.FormatItemList(Config.server_competition_auto.other[1].day_reward_item)
        ViewMgr:OpenView(SilkRoadPreview,{reward_list = reward_list,pos = Vector3.New(pos.x,pos.y-180,0)})
    end
end
--===========================CompetitionRankBtnItem===========================
CompetitionRankBtnItem = CompetitionRankBtnItem or DeclareMono("CompetitionRankBtnItem", UIWidgetBaseItem)
function CompetitionRankBtnItem:SetData(data)
    local TimeStateTip = Language.CompetitionRank.TimeStateTip
    local OffectNum = ServeractivityConfig.CompetitionRank.OffectNum
    local TimeTip = Format(Language.CompetitionRank.TimeOpenTip, data.day)
    local EndTimeTip = Format(Language.CompetitionRank.TimeEndTip, data.end_day)

    UH.LocalPos(self.Panel,Vector3.New(data.index % 2 == 0 and -(OffectNum/2) or 0,0,0))
    local version = data.version
    if version == 0 then
        UH.SetText(self.Name[1],Language.CompetitionRank.TabName[data.type])
        UH.SetText(self.Name[2],Language.CompetitionRank.TabName[data.type])
        --time 结算时间，=0表示已结束
        UH.LocalPosG(self.Label,Vector3.New(data.index % 2 == 0 and OffectNum or -OffectNum,0,0))
        UH.SetText(self.Time,data.time <= 0 and TimeStateTip[3] or TimeTip)
        if data.day <= 0 and data.time > 0 then
            local is_open = (data.time - TimeHelper.GetServerTime()) > TIME_DAY_SECOND_NUM * 2
            UH.SetText(self.LabelName,TimeStateTip[is_open and 1 or 2])
            UH.SpriteName(self.LabelImg,[[Label]] .. (is_open and 1 or 2))
        end
        self.Label:SetActive(data.day <= 0 and data.time > 0)
        self.Lock:SetActive(data.time <= 0 or data.day > 0)
        self.Block:SetActive(true)
    else
        UH.SetText(self.Name[1],Language.CompetitionRank.TabName2[data.type])
        UH.SetText(self.Name[2],Language.CompetitionRank.TabName2[data.type])
        UH.SetText(self.Time, data.time <= 0 and TimeStateTip[3] or EndTimeTip)
        self.Lock:SetActive(true)
        self.Block:SetActive(data.time <= 0)
        self.Label:SetActive(false)
    end
    UH.SetInter(self.BtnInter, data.time > 0)
    
    UIWidgetBaseItem.SetData(self, data)
    self:FlushRedNum()
end

function CompetitionRankBtnItem:FlushRedNum()
    local info = CompetitionRanklData.Instance:GetRankInfo(self.data.type) or {}
    local flag = CompetitionRanklData.Instance:GetCompetitionRankNum(self.data.type,info.cur_value)
    self.RedPoint:SetNum((self.data.day <= 0 and self.data.time > 0 and flag == true) and 1 or 0)
end

function CompetitionRankBtnItem:Click()
    self.ToggleClick:Trigger()
end

function CompetitionRankBtnItem:OnClickBlock()
    PublicPopupCtrl.Instance:Center(tostring(self.Time.text))
end

--===========================CompetitionRankItem===========================
CompetitionRankItem = CompetitionRankItem or DeclareMono("CompetitionRankItem", UIWidgetBaseItem)

function CompetitionRankItem:SetData(data)
    local role = data.role or {}
    local rank_type = data.rank_type or CompetitionRanklData.Instance:GetSelectType()
    local uid = data.uid or data.role_id or role.uid or role.role_id
    if uid == 0 and StringIsEmpty(data.name) then
        UH.SetText(self.Name, Language.CompetitionRank.NoInRank)
        UH.SetText(self.Need, Language.CompetitionRank.NeedTip[data.rank_type])
        -- LogDG(CompetitionRanklData.Instance:GetNeedValue(rank_type, data.rank))
        -- LogDG(data.rank_value)
        UH.SetText(self.NeedValue, CompetitionRanklData.Instance:GetNeedValue(rank_type, data.rank))
        UH.SetText(self.Value, "")
        self.Head:SetActive(false)
        self.NeedObj:SetActive(true)
        self.XingXing:SetActive(rank_type == ServeractivityConfig.CompetitionRank.Type.CourageChallengeRank)
    else
        self.NeedObj:SetActive(false)
        self.Head:SetActive(true)
        UH.SetText(self.Name,data.name or role.name)
        UH.SetText(self.Value,data.rank_value == 0 and "--" or data.rank_value)
        UH.SetAvatar(self.IconSp, data.avatar_type or role.avatar_type, data.avatar_id or role.avatar_id,data.avatar_quality or role.avatar_quality)
    end
    UH.SetText(self.Rank,data.rank > 3 and data.rank or (data.rank == 0 and Language.CourageChallenge.NotHasRankTips or ""))
    UH.SpriteName(self.RankSp,(data.rank ~= 0 and data.rank <= 3) and [[Rank]] .. data.rank or nil)
    local reward_list = CompetitionRanklData.Instance:GetRankRewardData(data.rank)
    self.NotHasTip:SetActive(#reward_list == 0)
    self.Grid:SetData(reward_list)
    UIWidgetBaseItem.SetData(self, data)
end

function CompetitionRankItem:OnClickItem()
    local role_id = self.data.uid or 0 --self.data.role_id or self.data.uid or self.data.role.uid
    if role_id ~= 0 then
        RoleInfoCtrl.Instance:Data():SetViewEquipUid(role_id)
        RoleInfoCtrl.Instance:SendReferRoleEquipment(role_id)
    end
end

--===========================CompetitionRankRewardItem===========================
CompetitionRankRewardItem = CompetitionRankRewardItem or DeclareMono("CompetitionRankRewardItem", UIWidgetBaseItem)

function CompetitionRankRewardItem:SetData(data)
    UH.SetText(self.Score,Format(Language.CompetitionRank.Score[data.reward_type],data.parameter_1))
    local info = CompetitionRanklData.Instance:GetRankInfo(data.rank_type)
    local flag = CompetitionRanklData.Instance:GetRankRewardFlag(data.rank_type,data.seq)
    local can_reward = flag == 0 and info and info.cur_value >= data.parameter_1
    if data.rank_type == ServeractivityConfig.CompetitionRank.Type.ArenaRank then
        can_reward = flag == 0 and info and info.cur_value <= data.parameter_1
    end
    self.Received:SetActive(flag == 1)
    self.RedPos:SetNum(can_reward and 1 or 0)
    UIWidgetBaseItem.SetData(self, data)

    local reward_list = DataHelper.FormatItemList(self.data.reward_item)
    if #reward_list == 1 then
        self.ItemCell:SetData(reward_list[1])
        self.ItemCall:SetData(reward_list[1])
    end
    UH.SetEnabled(self.Image,#reward_list > 1)
    self.ItemCell:SetObjActive(#reward_list == 1)
    self.ItemCall:SetObjActive(#reward_list == 1)
    self.Effect:SetActive(#reward_list > 1)
end

function CompetitionRankRewardItem:OnClickItem()
    if self.RedPos:GetNum() == 0 then
        local reward_list = DataHelper.FormatItemList(self.data.reward_item)
        if #reward_list == 1 then
            CellClicks.BagGridNoButton({GetData = function()
                return reward_list[1]
            end})
        else
            local pos = UH.GetRootPosition(self.gameObject)
            ViewMgr:OpenView(SilkRoadPreview,{reward_list = reward_list,pos = Vector3.New(pos.x,pos.y+150,0)})
        end
    else
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.KF_COMPETITION_RANK, 1, self.data.rank_type, self.data.seq, 0)
    end
end


CompetitionRankQiPao = CompetitionRankQiPao or DeclareMono("CompetitionRankQiPao", UIWFlushPanel)
function CompetitionRankQiPao:CompetitionRankQiPao()
    self.data = CompetitionRanklData.Instance
    self.data_cares = {
        {data = self.data.rank_list, func = self.OnFlush}
    }
    self.rank_list = {}
end
function CompetitionRankQiPao:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.DescObj:SetActive(false)
    --缓存一些当前榜单自己的排名
    local list
    local role_id = RoleData.Instance:GetRoleId()
    for i = 0, ServeractivityConfig.CompetitionRank.Type.Comprehensive do
        self.rank_list[i] = -1
        list = self.data:GetRankList(i)
        for k, v in pairs(list) do
            if v.uid == role_id then
                self.rank_list[i] = 1
            end
        end
    end
    -- LogDG("当前排名", self.rank_list)
    if ActivityData.IsOpen(ACTIVITY_TYPE.KF_COMPETITION_RANK) then
        --然后开启每30分钟检查发一次协议的逻辑
        TimeHelper:CancelTimer(self.handle_delay30)
        self.handle_delay30 = TimeHelper:AddRunTimer(function ()
            if not ActivityData.IsOpen(ACTIVITY_TYPE.KF_COMPETITION_RANK) then
                TimeHelper:CancelTimer(self.handle_delay30)
                self.handle_delay30 = nil
                return
            end
            CompetitionRanklCtrl.Instance:SendReqAllRankInfo()
        end, 1800, nil, true)
    end
end
function CompetitionRankQiPao:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_delay)
    self.handle_delay = nil
    TimeHelper:CancelTimer(self.handle_delay30)
end
function CompetitionRankQiPao:OnFlush()
    local version = self.data:Version()
    if version == 0 then
        return
    end
    if self.handle_delay then
        return
    end
    if not ActivityData.IsOpen(ACTIVITY_TYPE.KF_COMPETITION_RANK) then
        return
    end
    local show_list = {}
    local list
    local role_id = RoleData.Instance:GetRoleId()
    local is_had_me = false
    local is_up = false
    for i = 0, ServeractivityConfig.CompetitionRank.Type.Comprehensive do
        list = self.data:GetRankList(i)
        show_list[i] = false
        for k, v in pairs(list) do
            if v.uid == role_id then
                is_had_me = true
            end
        end
        if is_had_me and self.rank_list[i] == -1 then
            show_list[i] = true
            self.rank_list[i] = 1
        end
        if not is_had_me and self.rank_list[i] == 1 then
            show_list[i] = true
            self.rank_list[i] = -1
        end
    end
    -- 从show_list中筛选快结束的那个进行展示
    local act_list = self.data:GetBtnList()
    if act_list == nil then
        return
    end
    local show_type = -1
    local end_day = 99999
    for k, v in pairs(act_list) do
        if  show_list[v.type] == true and v.end_day ~= 0 and v.end_day < end_day then
            show_type = v.type
            end_day = v.end_day
        end
    end
    if show_type ~= -1 and show_list[show_type] == true then
        local desc = self.rank_list[show_type] == 1 and Language.CompetitionRank.QiPao or Language.CompetitionRank.QiPoa2
        -- UH.SetText(self.Desc1, Format(desc, Language.CompetitionRank.TabName[show_type]))
        UH.SetText(self.Desc2, Format(desc, Language.CompetitionRank.TabName[show_type]))
        self.DescObj:SetActive(true)
        self.handle_delay = TimeHelper:AddDelayTimer(function ()
            self.DescObj:SetActive(false)
            self.handle_delay = nil
        end, 5)
    end
end
function CompetitionRankQiPao:OnClickTest()
    local show_type = 5
    local show_list = {}
    show_list[show_type] = true 
    local act_list = self.data:GetBtnList()
    local show_type = -1
    local end_day = 99999
    for k, v in pairs(act_list) do
        if  show_list[v.type] == true and v.end_day ~= 100 and v.end_day < end_day then
            show_type = v.type
            end_day = v.end_day
        end
    end
    self.rank_list[show_type] = 1
    if show_type ~= -1 and show_list[show_type] == true then
        local desc = self.rank_list[show_type] == 1 and Language.CompetitionRank.QiPao or Language.CompetitionRank.QiPoa2
        -- UH.SetText(self.Desc1, Format(desc, Language.CompetitionRank.TabName[show_type]))
        UH.SetText(self.Desc2, Format(desc, Language.CompetitionRank.TabName[show_type]))
        self.DescObj:SetActive(true)
        self.handle_delay = TimeHelper:AddDelayTimer(function ()
            self.DescObj:SetActive(false)
            self.handle_delay = nil
        end, 5)
    end
end