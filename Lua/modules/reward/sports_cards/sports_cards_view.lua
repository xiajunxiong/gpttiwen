SportCardsView = SportCardsView or DeclareMono("SportCardsView", UIWFlushPanel)

function SportCardsView:SportCardsView()
    self.data = SportsCardsData.Instance
    self.data_cares = {
        {data = self.data.view_data, func = self.OnFlush,  init_call = true},
    }

    -- self.show_list:SetCompleteCallBack(function()
    --     self:OnDragShowList()
    -- end)
end

function SportCardsView:Awake()
    UIWFlushPanel.Awake(self)

    -- 请求信息
    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
        rand_activity_type = ACTIVITY_TYPE.RAND_JING_JI_YUE_KA,
        opera_type = 0,
    })

    self.act_timer:StampTime(SportsCardsData.Instance.base_info.next_switch_timestamp,TimeType.Type_Time_3,Language.SportCards.ActTime)
    self.act_timer:SetShowCallBack(function (t)
        local time_s = ""
        for k , v in pairs(t) do 
            if tonumber(v) > 0 and k ~= "s" then 
                time_s = time_s ..v..Language.HappyLink.time[k]
            end 
        end 
        return time_s
    end)
end

function SportCardsView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

end 

function SportCardsView:OnFlush()
    if self.is_init == nil then 
        self.is_init = true
        local list = self.data:GetSportsCardsList()
        self.show_list:SetData(list)

        self:FlushOther()
        self:OnDragShowList()
    else 
        self:FlushList()
    end 
end

-- 其他介面信息
function SportCardsView:FlushOther()
    local cfg = self.data:GetCurTaskNumCfg()
    if cfg then UH.SetText(self.title, cfg.activity_title) end 
    self.btn_level:SetActive(self.data:GetCanLevelBuyTime())
end

-- 刷新列表的状态
function SportCardsView:FlushList()
    for k,v in pairs(self.show_list.item_list) do 
        v:FlushData()
    end 

    self.btn_extral:SetActive(self.data.base_info.is_active_buy_flag == 0)
end

-- (list的可视长度+list的坐标变化)/ 单个项的宽 = 展示数量
function SportCardsView:OnDragShowList()
    local list_legth = self.show_rect.sizeDelta.x - self.list_rect.localPosition.x
    local show_num = math.floor(list_legth / (self.item_rect.sizeDelta.x + 4.7)) -- 4.7是间隔

    local level_pre = (math.floor(show_num / 5) + 1) * 5

    local operate = self.show_list.item_list[level_pre] == nil and self.data:GetSportsCardsList()[level_pre] or self.show_list.item_list[level_pre]:GetData()

    self.PreShow:SetData(operate)
end

function SportCardsView:OnClickLevelBuy()
    ViewMgr:OpenView(SportsCardsLevel)
end

function SportCardsView:OnClickExpCatch()
    ViewMgr:OpenView(SportsCardsExp)
end


function SportCardsView:OnClickExtralBuy()
    ViewMgr:OpenView(SportsCardsExtral)
end

SportShowCell =  SportShowCell or DeclareMono("SportShowCell", UIWidgetBaseItem)
function SportShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local level_info = SportsCardsData.Instance:GetLevelInfo()
    -- 普通奖励
    local flag_1 = data.cfg.con_reward_item.item_id ~= nil 
    self.operates[1]:SetActive(flag_1)
    if flag_1 then 
        local reward_1 = BagData.Instance:ItemBuild(data.cfg.con_reward_item)
        self.reward_cells[1]:SetData(reward_1)
    end
    -- 追加奖励
    local reward_2 = BagData.Instance:ItemBuild(data.cfg.add_reward_item[0])
    self.reward_cells[2]:SetData(reward_2)

    local flag_3 = data.cfg.add_reward_item[1] ~= nil 
    self.operates[3]:SetActive(flag_3)
    if flag_3 then 
        local reward_3 = BagData.Instance:ItemBuild(data.cfg.add_reward_item[1])
        self.reward_cells[3]:SetData(reward_3)
    end 

    UH.SetText(self.index, data.cfg.seq)

    -- 刷新动态信息
    self:FlushShow()
end 

-- 刷新动态信息
function SportShowCell:FlushData()
    self.data.common_flag = SportsCardsData.Instance:CheckIsCatch(self.data.cfg.seq,1)
    self.data.adv_flag = SportsCardsData.Instance:CheckIsCatch(self.data.cfg.seq,2)

    self:FlushShow()
end

function SportShowCell:FlushShow()
    local level_info = SportsCardsData.Instance:GetLevelInfo()
    self.catchs[1]:SetActive(self.data.common_flag)
    self.catchs[2]:SetActive(self.data.adv_flag)

    local flag_3 = self.data.cfg.add_reward_item[1] ~= nil 
    if flag_3 then 
        self.catchs[3]:SetActive(self.data.adv_flag)
    end 

    self.click_boxs[1]:SetActive(not self.data.common_flag and level_info.level >= self.data.cfg.seq)
    self.click_boxs[2]:SetActive(not self.data.adv_flag and level_info.level >= self.data.cfg.seq and SportsCardsData.Instance.base_info.is_active_buy_flag == 1)
    self.click_boxs[3]:SetActive(not self.data.adv_flag and level_info.level >= self.data.cfg.seq and SportsCardsData.Instance.base_info.is_active_buy_flag == 1)

    self.high_mask:SetActive(SportsCardsData.Instance.base_info.is_active_buy_flag == 0 or level_info.level < self.data.cfg.seq)
    self.nomal_mask:SetActive(level_info.level < self.data.cfg.seq)
end

-- 点击则申请
function SportShowCell:OnClickItem(index)
    if index == 1 then 
        ServerActivityCtrl.Instance:SendRandActivityOperaReq({
            rand_activity_type = ACTIVITY_TYPE.RAND_JING_JI_YUE_KA,
            opera_type = 1,
            param_1 = self.data.cfg.seq,
        })
    elseif index == 2 then
        ServerActivityCtrl.Instance:SendRandActivityOperaReq({
            rand_activity_type = ACTIVITY_TYPE.RAND_JING_JI_YUE_KA,
            opera_type = 2,
            param_1 = self.data.cfg.seq,
        })
    end 
end


SportShowPreCell = SportShowPreCell or DeclareMono("SportShowPreCell", UIWidgetBaseItem)
function SportShowPreCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local level_info = SportsCardsData.Instance:GetLevelInfo()
    -- 普通奖励
    local flag_1 = data.cfg.con_reward_item.item_id ~= nil 
    self.operates[1]:SetActive(flag_1)
    if flag_1 then 
        local reward_1 = BagData.Instance:ItemBuild(data.cfg.con_reward_item)
        self.reward_cells[1]:SetData(reward_1)
    end
    -- 追加奖励
    local reward_2 = BagData.Instance:ItemBuild(data.cfg.add_reward_item[0])
    self.reward_cells[2]:SetData(reward_2)

    local flag_3 = data.cfg.add_reward_item[1] ~= nil 
    self.operates[3]:SetActive(flag_3)
    if flag_3 then 
        local reward_3 = BagData.Instance:ItemBuild(data.cfg.add_reward_item[1])
        self.reward_cells[3]:SetData(reward_3)
    end 

    UH.SetText(self.pre_level, string.format(Language.SportCards.PreShowName,data.cfg.seq))

    -- 刷新动态信息
    self:FlushShow()
end 

-- 刷新动态信息
function SportShowPreCell:FlushData()
    self.data.common_flag = SportsCardsData.Instance:CheckIsCatch(self.data.cfg.seq,1)
    self.data.adv_flag = SportsCardsData.Instance:CheckIsCatch(self.data.cfg.seq,2)

    self:FlushShow()
end

function SportShowPreCell:FlushShow()
    local level_info = SportsCardsData.Instance:GetLevelInfo()
    -- if self.reward_cells[1].MengBan then 
    --     self.reward_cells[1].MengBan:SetActive(self.data.common_flag) 
    -- end 
    self.catchs[1]:SetActive(self.data.common_flag)

    -- if self.reward_cells[2].MengBan then 
    --     self.reward_cells[2].MengBan:SetActive(self.data.adv_flag) 
    -- end 
    self.catchs[2]:SetActive(self.data.adv_flag)

    local flag_3 = self.data.cfg.add_reward_item[1] ~= nil 
    if flag_3 then 
        -- if self.reward_cells[3].MengBan then 
        --     self.reward_cells[3].MengBan:SetActive(self.data.adv_flag) 
        -- end 
        self.catchs[3]:SetActive(self.data.adv_flag)
    end 
end

SportsCardsProg =  SportsCardsProg or DeclareMono("SportsCardsProg", UIWFlushPanel)
function SportsCardsProg:SportsCardsProg()
    self.data = SportsCardsData.Instance
    self.data_cares = {
        {data = self.data.view_data, func = self.OnFlush,  init_call = true},
    }
end 

function SportsCardsProg:OnFlush()
    local level_info = self.data:GetLevelInfo()
    UH.SetText(self.level,level_info.level)
    UH.SetText(self.exp_need,level_info.total - level_info.exp)
    UH.SetText(self.exp_prog,ColorStr(level_info.exp,level_info.exp >= level_info.total and COLORSTR.Yellow9 or COLORSTR.Red8).."/"..level_info.total)
    self.exp_bar:SetProgress(level_info.exp/level_info.total)
end