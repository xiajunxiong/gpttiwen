ShenYuRankView = ShenYuRankView or DeclareView("ShenYuRankView", "shen_yu_rank/shen_yu_rank",Mod.LandsBetweenExtra.Rank)
VIEW_DECLARE_LEVEL(ShenYuRankView, ViewLevel.Lt)
VIEW_DECLARE_MASK(ShenYuRankView, ViewMask.BgBlock)

function ShenYuRankView:ShenYuRankView()
end

function ShenYuRankView:LoadCallback(param_t)
end

function ShenYuRankView:OnCloseClick()
    ViewMgr:CloseView(ShenYuRankView)
end

-- ===========================ShenYuRankPanel===========================
ShenYuRankPanel = ShenYuRankPanel or DeclareMono("ShenYuRankPanel", UIWFlushPanel)

function ShenYuRankPanel:ShenYuRankPanel()
    self.language = Language.ShenYuRank
    self.data = ShenYuRankData.Instance
    self.data.rank_sel_data.cur_select_rank_type = 0
    self.data.rank_sel_data.cur_select_target_type = 0
    self.data_cares = {
        {data = self.data.rank_sel_data, func = self.FlushList,init_call = false,keys={"cur_select_rank_type","cur_select_target_type","cur_sel_data_change"}},
    }
    self.data:ResetUpdateTime()
    self:FlushTime()
    self:FlushList()
    local time=os.date(self.language.timef,LandsBetweenData.Instance.base_info.season_end_timestamp + JIE_SUNA_TIME_OFFSET)
    UH.SetText(self.TxtTip,string.format(self.language.tip,time))
    self:OnClickLeft(0)
    self:OnClickRight(0)
end

function ShenYuRankPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function ShenYuRankPanel:FlushList()
    local data = self.data:GetCurSelData()
    local list_data=data.rank_list
    self.List:SetData(list_data)
    self.NoRankTip:SetActive(#list_data == 0)
    self.Item_my:SetData(data.my_rank_info)
end

function ShenYuRankPanel:FlushTime()
    local cur_time = TimeCtrl.Instance:GetServerTime()
    if  self.data.refresh_time == nil  or self.data.refresh_time <= cur_time then
        self.data.refresh_time = cur_time + 300
    end
    local delayTime = self.data.refresh_time - cur_time
    -- self.time_handle:CloseCountDownTime()
    TimeHelper:CancelTimer(self.time_handle)
    self.time_handle = TimeHelper:AddDelayTimer(function()
        self.data:GetCurSelData()
        self:FlushTime()
    end, delayTime)
end



-- 选中左侧按钮回调
function ShenYuRankPanel:OnClickLeft(index)
    self.data.rank_sel_data.cur_select_rank_type = index 
    UH.SetText(self.Title2, self.language.rank_type_title[self.data.rank_sel_data.cur_select_rank_type])
end

-- 选中右侧按钮回调
function ShenYuRankPanel:OnClickRight(index)
    self.data.rank_sel_data.cur_select_target_type = index
    if self.data.rank_sel_data.cur_select_target_type == 3 then
        UH.SetText(self.Title1, self.language.target_title2)
    else
        UH.SetText(self.Title1, self.language.target_title)
    end
end


-- 展示奖励
function ShenYuRankPanel:OnRewardShow(data)
    local param = {rewards = {}} --{rewards=data.cfg.reward_show}
    if data.cfg.senior_gold_num ~= nil and data.cfg.senior_gold_num > 0 then 
        table.insert(param.rewards,{item_id = CommonItemId.Gold,num = data.cfg.senior_gold_num,is_bind = 0})
    end 
    if data.cfg.lingyu_num ~= nil and data.cfg.lingyu_num > 0 then 
        table.insert(param.rewards,{item_id = CommonItemId.ImmortalCoin,num = data.cfg.lingyu_num,is_bind = 0})
    end 

    for k,v in pairs(data.cfg.reward_show) do 
        table.insert(param.rewards,v)
    end 
    
    ViewMgr:OpenView(ShenYuRankRewardView,param)
end

function ShenYuRankPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_handle)
end

-- ===========================ShenYuRankItem===========================
ShenYuRankItem = ShenYuRankItem or DeclareMono("ShenYuRankItem", UIWidgetBaseItem)
function ShenYuRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local rank = data.rank
    local camp = data.faction or 0
    local num = data.rank_value or 0
    local name = data.name and data.name .. "\n" or ""
    local server = ""
    if data.server_id then
        local server_info = LoginData.Instance:GetServerInfoById(data.server_id)
        if server_info ~= nil then
            if IS_EDITOR then
                server = string.format(Language.Login.ServerNameEditor, server_info.id, server_info.name)
            else
                server = string.format(Language.Login.ServerName, server_info.name)
            end
        end
        server = server.."\n"
    end
    self.data.cfg = ShenYuRankData.Instance:GetRankCfg(rank)
    local reward = self.data.cfg and Item.Init(self.data.cfg.reward_show_outside,1,0) or {}
    -- if self.data.cfg then 
    --     if self.data.cfg.senior_gold_num ~= nil and self.data.cfg.senior_gold_num > 0 then 
    --         table.insert(reward,Item.Init(CommonItemId.Gold,self.data.cfg.senior_gold_num,0))
    --     end 
    --     if self.data.cfg.lingyu_num ~= nil and self.data.cfg.lingyu_num > 0 then 
    --         table.insert(reward,Item.Init(CommonItemId.ImmortalCoin,self.data.cfg.lingyu_num,0))
    --     end 
    --     table.insert(reward,Item.Init(self.data.cfg.reward_show_outside,1,0))
    -- end 
    if rank <= 3 then
        self.BG_Rank:SetObjActive(true)
        UH.SpriteName(self.BG_Rank, "rank" .. rank)
        self.Img_Rank:SetObjActive(true)
        UH.SpriteName(self.Img_Rank, TombRaiderConfig.RankSp[rank])
        self.Txt_Rank:SetObjActive(false)
    else
        self.BG_Rank:SetObjActive(false)
        self.Img_Rank:SetObjActive(false)
        self.Txt_Rank:SetObjActive(true)--[[  ]]
        UH.SetText(self.Txt_Rank, rank)
    end
    UH.SetText(self.Txt_Num, num)
    UH.SetText(self.Txt_Server, string.format(Language.ShenYuRank.rankInfo, name, server, Config.ranking_list_auto.camp_id[camp + 1].id))
    self.Reward:SetData(reward)
    -- self.RewardList:SetData(reward)
    if ShenYuRankData.Instance.rank_sel_data.cur_select_target_type == GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_FACTION then
        -- UH.SpriteName(self.Img_Head, Config.ranking_list_auto.camp_id[camp + 1].icon_id)
        -- UH.SetIcon(self.Img_Head,Config.ranking_list_auto.camp_id[camp + 1].icon_id, ICON_TYPE.ACT)
        UH.SpriteName(self.Img_Camp,Config.ranking_list_auto.camp_id[camp + 1].icon_id)
        self.Img_Camp:SetObjActive(true)
        self.Img_Head:SetObjActive(false)
    else
        local head_type = data.avatar_type or 2
        local head_id = data.headshot_id or 0
        UH.SetAvatar(self.Img_Head, head_type, head_id, 0)
        self.Img_Camp:SetObjActive(false)
        self.Img_Head:SetObjActive(true)
    end
end

-- ===========================ShenYuRankItemMy===========================
ShenYuRankItemMy = ShenYuRankItemMy or DeclareMono("ShenYuRankItemMy", UIWidgetBaseItem)
function ShenYuRankItemMy:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if data.head_id then
        UH.SetAvatar(self.Img_Head, data.head_type, data.head_id, data.head_quality)
        self.Img_Camp:SetObjActive(false)
        self.Img_Head:SetObjActive(true)
    else
        UH.SpriteName(self.Img_Camp, data.campImg)
        self.Img_Camp:SetObjActive(true)
        self.Img_Head:SetObjActive(false)
    end
    UH.SetText(self.Txt_Server, data.myInfo)
    UH.SetText(self.Txt_Rank, data.rank)
    UH.SetText(self.Txt_Num, data.value)
end


