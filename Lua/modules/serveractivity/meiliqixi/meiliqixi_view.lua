ActMeiLiQiXiView = ActMeiLiQiXiView or DeclareView("ActMeiLiQiXiView", "serveractivity/meiliqixi/act_meiliqixi",Mod.QixiFestival.MeiLiQiXi)

function ActMeiLiQiXiView:ActMeiLiQiXiView()
    self.data = ActMeiLiQiXiData.Instance
end

function ActMeiLiQiXiView:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[340].desc})
end

function ActMeiLiQiXiView:OnClickClose()
    ViewMgr:CloseView(ActMeiLiQiXiView)
end

-------------------------------------------
ActMeiLiQiXiPanel = ActMeiLiQiXiPanel or DeclareMono("ActMeiLiQiXiPanel", UIWFlushPanel)

function ActMeiLiQiXiPanel:ActMeiLiQiXiPanel()
    self.data = ActMeiLiQiXiData.Instance

    self:SetActiveRemainingTimeMeter()
    self.AiMuRedPoint:SetNum(self.data:AiMuZhiRemind())
    self.MeiLiRedPoint:SetNum(self.data:MeiLiZhiRemind())
end

function ActMeiLiQiXiPanel:Awake()
    UIWFlushPanel.Awake(self)

    -- 活动剩余时间回调
    self.ActiveRemainingTimeMeter:SetCallBack(function ()
        ViewMgr:CloseView(ActMeiLiQiXiView)
    end)
end

function ActMeiLiQiXiPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function ActMeiLiQiXiPanel:SetActiveRemainingTimeMeter()
    self.ActiveRemainingTimeMeter:CloseCountDownTime()
    self.ActiveRemainingTimeMeter:SetShowCallBack(function (t)
        local t_str = ""
		if t.day > 0 then
			t_str = string.format(Language.MeiLiQiXi.Time1,t.day, t.hour,t.min)
		else
			t_str = string.format(Language.MeiLiQiXi.Time2,t.hour,t.min)
		end
        return t_str
	end)

    self.ActiveRemainingTimeMeter:CreateActTime(ACTIVITY_TYPE.RAND_MEILIQIXI,TimeType.Type_Time_3)
end


-- 爱慕值panel--------------------------------------------------
ActAiMuZhiPanel = ActAiMuZhiPanel or DeclareMono("ActAiMuZhiPanel", UIWFlushPanel)

function ActAiMuZhiPanel:ActAiMuZhiPanel()
    self.data = ActMeiLiQiXiData.Instance
    self.data_cares = {
        {data = self.data.meiliqixi_smart, func = self.FlushAiMuZhiItemList,init_call = false},
    }
    -- self.RedPoint:SetNum(self.data:AiMuZhiRemind())
end

function ActAiMuZhiPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushAiMuZhiItemList()
end

function ActAiMuZhiPanel:FlushAiMuZhiItemList()

    local aimuzhi_item_list = self.data:GetAiMuZhiItemList()
    self.RedPoint:SetNum(self.data:AiMuZhiRemind())

    table.sort(aimuzhi_item_list,function (a,b)
        -- 没领肯定在前面
        if self.data.meiliqixi_smart.admiration_map[a.seq]<self.data.meiliqixi_smart.admiration_map[b.seq] then
            return true
        -- 都领了或都没领比亲密度大小
        elseif self.data.meiliqixi_smart.admiration_map[a.seq] == self.data.meiliqixi_smart.admiration_map[b.seq] then
            if a.add_intimate < b.add_intimate then
                return true
            else
                return false
            end
        -- a领了b没a在后面
        else 
            return false
        end
    end
    )

    -- 设置panel的大item列表
    if aimuzhi_item_list then
        self.AiMuZhiItemList:SetData(aimuzhi_item_list)
    end
end

function ActAiMuZhiPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

-- 爱慕值item-------------------------------------------
ActAiMuZhiItem = ActAiMuZhiItem or DeclareMono("ActAiMuZhiItem", UIWidgetBaseItem)

function ActAiMuZhiItem:ActAiMuZhiItem()
    self.data_instance =  ActMeiLiQiXiData.Instance
end

function ActAiMuZhiItem:ShowItemGetRedPoint()
    if self.data_instance.meiliqixi_smart.admiration_value>=self.add_intimate 
    and self.data_instance.meiliqixi_smart.admiration_map[self.seq] == 0
    then        
        return 1
    else
        return 0
    end
end

function ActAiMuZhiItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    -- 亲密度
    self.add_intimate = data.add_intimate
    self.seq = data.seq
    -- LogError("self.data_instance.meiliqixi_smart.admiration_value",self.data_instance.meiliqixi_smart.admiration_value,"self.add_intimate",self.add_intimate)
    self.GoButton:SetActive(self.data_instance.meiliqixi_smart.admiration_value<self.add_intimate)
    self.GetButton:SetActive(self.data_instance.meiliqixi_smart.admiration_value>=self.add_intimate and self.data_instance.meiliqixi_smart.admiration_map[self.seq] == 0)
    self.HaveGet:SetActive(self.data_instance.meiliqixi_smart.admiration_value>=self.add_intimate and self.data_instance.meiliqixi_smart.admiration_map[self.seq] == 1)
    -- LogError("self.add_intimate",self.add_intimate)
    UH.SetText(self.LeiJiText, string.format(Language.MeiLiQiXi.TotalSend,self.add_intimate))
    UH.SetText(self.JinDuText, string.format(Language.MeiLiQiXi.ProgStr, self.data_instance.meiliqixi_smart.admiration_value, self.add_intimate))
    self.GetRedPoint:SetNum(self:ShowItemGetRedPoint())
    -- 获得奖励的reward_item
    self.reward_item = data.reward_item
    self:FlushRewardItemList(self.reward_item)
end

function ActAiMuZhiItem:FlushRewardItemList(reward_item)
    local reward_item_list = self.data_instance:GetRewardItemList(reward_item)
    self.RewardItemList:SetData(reward_item_list)
end

function ActAiMuZhiItem:OnClickGo()
    ViewMgr:CloseView(ActMeiLiQiXiView)
    ViewMgr:OpenView(SocietyView)
    
    ActMeiLiQiXiCtrl.Instance:SendMeiLiQiXiReq(ActMeiLiQiXiCtrl.Instance.REQ_TYPE.REQ_INFO)
end

function ActAiMuZhiItem:OnClickGet()
    ActMeiLiQiXiCtrl.Instance:SendMeiLiQiXiReq(ActMeiLiQiXiCtrl.Instance.REQ_TYPE.REQ_GET_REWARD,0,self.seq)
end


-- 魅力值panel-------------------------------
ActMeiLiZhiPanel = ActMeiLiZhiPanel or DeclareMono("ActMeiLiZhiPanel", UIWFlushPanel)

function ActMeiLiZhiPanel:ActMeiLiZhiPanel()
    self.data = ActMeiLiQiXiData.Instance
    self.data_cares = {
        {data = self.data.meiliqixi_smart, func = self.FlushMeiLiZhiItemList,init_call = false},
    }
    -- self.RedPoint:SetNum(self.data:MeiLiZhiRemind())
end

function ActMeiLiZhiPanel:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushMeiLiZhiItemList()
end


function ActMeiLiZhiPanel:FlushMeiLiZhiItemList()
    local meilizhi_item_list = self.data:GetMeiLiZhiItemList()

    self.RedPoint:SetNum(self.data:MeiLiZhiRemind())

    table.sort(meilizhi_item_list,function (a,b)

        -- 没领肯定在前面
        if self.data.meiliqixi_smart.charm_map[a.seq]<self.data.meiliqixi_smart.charm_map[b.seq] then
            return true
        -- 都领了或都没领比亲密度大小
        elseif self.data.meiliqixi_smart.charm_map[a.seq] == self.data.meiliqixi_smart.charm_map[b.seq] then
            if a.add_intimate < b.add_intimate then
                return true
            else
                return false
            end
        -- a领了b没a在后面
        else 
            return false
        end
    end
    )
    -- 设置panel的大item列表
    if meilizhi_item_list then
        self.MeiLiZhiItemList:SetData(meilizhi_item_list)
    end
end

function ActMeiLiZhiPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end


-- 魅力值item--------------------------------------
ActMeiLiZhiItem = ActMeiLiZhiItem or DeclareMono("ActMeiLiZhiItem", UIWidgetBaseItem)

function ActMeiLiZhiItem:ActMeiLiZhiItem()
    self.data_instance =  ActMeiLiQiXiData.Instance
end

function ActMeiLiZhiItem:ShowItemGetRedPoint()
    if self.data_instance.meiliqixi_smart.charm_value>=self.add_intimate 
    and self.data_instance.meiliqixi_smart.charm_map[self.seq] == 0
    then        
        return 1
    else
        return 0
    end
end

function ActMeiLiZhiItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    -- 亲密度
    self.add_intimate = data.add_intimate
    self.seq = data.seq

    self.GoButton:SetActive(self.data_instance.meiliqixi_smart.charm_value<self.add_intimate)
    self.GetButton:SetActive(self.data_instance.meiliqixi_smart.charm_value>=self.add_intimate and self.data_instance.meiliqixi_smart.charm_map[self.seq] == 0)
    self.HaveGet:SetActive(self.data_instance.meiliqixi_smart.charm_value>=self.add_intimate and self.data_instance.meiliqixi_smart.charm_map[self.seq] == 1)
    -- LogError("self.add_intimate",self.add_intimate)
    UH.SetText(self.LeiJiText, string.format(Language.MeiLiQiXi.TotalGet,self.add_intimate))
    UH.SetText(self.JinDuText, string.format(Language.MeiLiQiXi.ProgStr, self.data_instance.meiliqixi_smart.charm_value, self.add_intimate))
    self.GetRedPoint:SetNum(self:ShowItemGetRedPoint())
    -- 获得奖励的reward_item
    self.reward_item = data.reward_item
    self:FlushRewardItemList(self.reward_item)
end

function ActMeiLiZhiItem:FlushRewardItemList(reward_item)
    local reward_item_list = self.data_instance:GetRewardItemList(reward_item)
    self.RewardItemList:SetData(reward_item_list)
end

function ActMeiLiZhiItem:OnClickGo()
    ViewMgr:CloseView(ActMeiLiQiXiView)
    ViewMgr:OpenView(SocietyView)
    ActMeiLiQiXiCtrl.Instance:SendMeiLiQiXiReq(ActMeiLiQiXiCtrl.Instance.REQ_TYPE.REQ_INFO)
end

function ActMeiLiZhiItem:OnClickGet()
    ActMeiLiQiXiCtrl.Instance:SendMeiLiQiXiReq(ActMeiLiQiXiCtrl.Instance.REQ_TYPE.REQ_GET_REWARD,1,self.seq)
end



-- 魅力榜panel
ActMeiLiBangPanel = ActMeiLiBangPanel or DeclareMono("ActMeiLiBangPanel", UIWFlushPanel)

function ActMeiLiBangPanel:ActMeiLiBangPanel()
    self.data = ActMeiLiQiXiData.Instance
    self.data_cares = {
        {data = self.data.rank_data, func = self.FlushMeiLiBangItemList,init_call = false},
        {data = self.data.rank_data, func = self.FlushMyMeiLiBangItemList,init_call = false},
    }
    self.default_flushed = false
end

function ActMeiLiBangPanel:Awake()
    UIWFlushPanel.Awake(self)
    -- ActMeiLiQiXiCtrl.Instance:SendMeiLiQiXiRankReq()
    -- self:FlushMeiLiBangItemList()
    self:DefaultFLushMeiLiBangList()
end

function ActMeiLiBangPanel:DefaultFLushMeiLiBangList()
    if not self.default_flushed then
        
        local default_list = self.data:GetDefaultMeiLiBangItemList()
        self.MeiLiBangItemList:SetData(default_list)
        self.default_flushed = true
        -----DefaultflushMyInfo--------------------
        self.MyRankTextObj:SetActive(true)
        UH.SetText(self.MyRankText ,Language.MeiLiQiXi.NotInList)

        self.role =  RoleData.Instance:GetApearance()
        -- LogError("self.role",self.role)
        self.MyHead:SetData(self.role)

        self.rank_value = self.data.meiliqixi_smart.charm_value
        UH.SetText(self.MyGetRoseNum ,self.rank_value)

    end
end

function ActMeiLiBangPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)

    ActMeiLiQiXiCtrl.Instance:SendCrossMeiLiQiXiPersonRankReq()
end

function ActMeiLiBangPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.default_flushed = false
end

function ActMeiLiBangPanel:FlushMeiLiBangItemList()
    local reward_configuration = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MEILIQIXI, Config.meiliqixi_auto.reward_configuration)
    local meilibang_item_list = self.data:GetMeiLiBangItemList()

    local rank_info_length =  #meilibang_item_list
    if rank_info_length < 5 then
        for i = rank_info_length + 1, 5, 1 do
            local nobody_item = {
                rank = i,
                seq = i - 1,
                reward_item = reward_configuration[i].reward_item
            }

            table.insert(meilibang_item_list ,nobody_item)
        end
    end
    self.MeiLiBangItemList:SetData(meilibang_item_list)
end

function ActMeiLiBangPanel:FlushMyMeiLiBangItemList()
    -- LogError("刷新我的排行榜")
    self.my_rank_index = self.data.rank_data.my_rank_index
    if self.my_rank_index == -1 then
        self.MyRankTextObj:SetActive(true)
        UH.SetText(self.MyRankText ,Language.MeiLiQiXi.NotInList)

        self.role =  RoleData.Instance:GetApearance()
        -- LogError("self.role",self.role)
        self.MyHead:SetData(self.role)

        self.rank_value = self.data.meiliqixi_smart.charm_value
        UH.SetText(self.MyGetRoseNum ,self.rank_value)
    else
        self.rank = self.data.rank_data.rank_info_m.rank
        if self.rank == 1 then
            self.MyOne:SetActive(true)
            self.MyRankTextObj:SetActive(false)
        elseif self.rank == 2 then
            self.MyTwo:SetActive(true)
            self.MyRankTextObj:SetActive(false)
        elseif self.rank == 3 then
            self.MyThree:SetActive(true)
            self.MyRankTextObj:SetActive(false)
        elseif self.rank <=50 and self.rank > 3 then
            self.MyRankTextObj:SetActive(true)
            UH.SetText(self.MyRankText ,self.rank)
        else
            self.MyRankTextObj:SetActive(true)
            UH.SetText(self.MyRankText ,Language.MeiLiQiXi.NotInList)
        end
    
        self.rank_value = self.data.rank_data.rank_info_m.rank_value
        UH.SetText(self.MyGetRoseNum ,self.rank_value)
        
        self.role = self.data.rank_data.rank_info_m.role
        self.MyHead:SetData(self.role)
    end
end

-- 魅力榜item------------------------------
ActMeiLiBangItem = ActMeiLiBangItem or DeclareMono("ActMeiLiBangItem", UIWidgetBaseItem)
function ActMeiLiBangItem:ActMeiLiBangItem()
    self.data_instance =  ActMeiLiQiXiData.Instance
end

function ActMeiLiBangItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.role = data.role
    -- LogError("data",data)
    if self.role ~= nil then
        self.Head:SetData(self.role)
        UH.SetText(self.NameAndID ,self.role.name)
        self.NobodyHead:SetActive(false)
    else
        UH.SetText(self.NameAndID ,Language.MeiLiQiXi.ListEmtpy)
        self.NobodyHead:SetActive(true)
    end

    self.rank_value = data.rank_value
    if self.rank_value  ~= nil then
        UH.SetText(self.GetRoseNum ,self.rank_value)
    else 
        UH.SetText(self.GetRoseNum ,0)
    end

    self.rank = data.rank
    self.seq = data.seq

    if self.rank == 1 then
        self.One:SetActive(true)
        self.RankTextObj:SetActive(false)
    elseif self.rank == 2 then
        self.Two:SetActive(true)
        self.RankTextObj:SetActive(false)
    elseif self.rank == 3 then
        self.Three:SetActive(true)
        self.RankTextObj:SetActive(false)
    else
        self.RankTextObj:SetActive(true)
        UH.SetText(self.RankText ,self.rank)
    end

    self.reward_item = data.reward_item
    self.GiftIcon:SetObjActive(true)
    if self.rank == 1 then
        UH.SpriteName(self.GiftIcon, self.data_instance.GiftIcon[2])
    elseif self.rank == 2 or self.rank == 3 then
        UH.SpriteName(self.GiftIcon, self.data_instance.GiftIcon[1])
    elseif self.rank >= 4 and self.rank <= 10 then
        UH.SpriteName(self.GiftIcon, self.data_instance.GiftIcon[4])
    elseif self.reward_item ~= nil and self.rank >=10 then
        UH.SpriteName(self.GiftIcon, self.data_instance.GiftIcon[3])
    else
        self.GiftIcon:SetObjActive(false)
    end
end


function ActMeiLiBangItem:OnClickGiftIcon(reward_item)
    if self.reward_item ~=nil then
        ViewMgr:OpenView(MeiLiQiXiRewardView, {item_list = self.reward_item})
    end
end

---------------------------------------------
MeiLiQiXiRewardView = MeiLiQiXiRewardView or DeclareView("MeiLiQiXiRewardView", "serveractivity/meiliqixi/meiliqixi_reward_view")
VIEW_DECLARE_MASK(MeiLiQiXiRewardView, ViewMask.BlockClose)
function MeiLiQiXiRewardView:LoadCallback(param_t)
    local fix_item_list = BagData.Instance:ItemListBuild(param_t.item_list or {})
    -- self.ShowList:SetData(param_t.item_list)
    self.list:SetData(fix_item_list)
end

function MeiLiQiXiRewardView:OnClickClose()
    ViewMgr:CloseView(MeiLiQiXiRewardView)
end


MeiLiQiXiRewardCell = MeiLiQiXiRewardCell or DeclareMono("MeiLiQiXiRewardCell",UIWidgetBaseItem)
function MeiLiQiXiRewardCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data) 

    self.cell:SetData(data)
    UH.SetText(self.name ,data:QuaName(true))
end 

