------------------------  获取经验
SportsCardsExp = SportsCardsExp or DeclareView("SportsCardsExp", "reward/sports_cards_exp")
VIEW_DECLARE_LEVEL(SportsCardsExp, ViewLevel.L1)
function SportsCardsExp:SportsCardsExp()
    
end

function SportsCardsExp:LoadCallback(param_t)
    
end

function SportsCardsExp:OnClickClose()
    ViewMgr:CloseView(SportsCardsExp)    
end 

SportsCardsExpPanel = SportsCardsExpPanel or DeclareMono("SportsCardsExpPanel", UIWFlushPanel)

function SportsCardsExpPanel:SportsCardsExpPanel()
    self.data = SportsCardsData.Instance
    self.data_cares = {
        {data = self.data.view_data, func = self.OnFlush,  init_call = true},
    }

    self.list_type = 1
    self.ToggleDaily.isOn = true
end

function SportsCardsExpPanel:OnFlush()
    if self.is_init == nil then 
        self.is_init = true 

        self:FlushLeftList()
        self:FlushRightList()
    else 
        self:FlushLeftList(true)
        self:FlushRightList(true)
    end 
end

function SportsCardsExpPanel:FlushOther()
    local str = self.list_type == 1 and string.format(Language.SportCards.ExpTips,Config.jingjiyueka_auto.other[1].task_gold) or Language.SportCards.WeeksExpTips
    UH.SetText(self.exp_tips, str)
end

function SportsCardsExpPanel:FlushLeftList(Only_data)
    if not Only_data then 
        local left_list = self.data:GetTaskList(0)
        self.left_list:SetData(left_list)
    else 
        for k,v in pairs(self.left_list.item_list) do 
            v:FlushData()
        end 
    end 
end

function SportsCardsExpPanel:FlushRightList(Only_data)
    if not Only_data then 
        local right_list = self.data:GetTaskList(self.list_type)
        self.right_list:SetData(right_list)
    else 
        for k,v in pairs(self.right_list.item_list) do 
            v:FlushData()
        end 
    end 

    self:FlushOther()
end

function SportsCardsExpPanel:OnClickTask(param_int)
    self.list_type = param_int

    self:FlushRightList()
end 

SportsCardsExpCell =  SportsCardsExpCell or DeclareMono("SportsCardsExpCell", UIWidgetBaseItem)
function SportsCardsExpCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item_data = BagData.Instance:ItemBuild({item_id = Config.jingjiyueka_auto.other[1].task_exp,num = data.cfg.task_exp})
    self.call:SetData(item_data)
    
    self.btn_goto:SetActive(data.cfg.jump_interface > 0 and data.finish_flag == 0)
    if self.btn_reset then self.btn_reset:SetActive(data.cfg.task_group == SportsCardEmum.TYPE_DAY_TASK and data.finish_flag == 0 and data.refresh_num == 0) end 

    self:FlushShow()
end 

function SportsCardsExpCell:FlushData()
    local info = self.data.cfg.task_group ~= SportsCardEmum.TYPE_DAY_TASK and SportsCardsData.Instance:CatchTaskInfo(self.data.cfg.task_group,self.data.cfg.task_id)
        or SportsCardsData.Instance:DayilyTaskInfo(self.data.task_index)
    self.data.finish_flag = info.finish_flag
    self.data.refresh_num = info.refresh_num

    self.data.task_id = info.task_id
    self.data.cfg = SportsCardsData.Instance:CatchTaskCfg(self.data.cfg.task_group,info.task_id)

    self:FlushShow()
end 

function SportsCardsExpCell:FlushShow()
    UH.SetText(self.title, self.data.cfg.desc)
    local prog = SportsCardsData.Instance:CatchProgress(self.data.cfg.task_group,self.data.cfg.task_type)
    local is_special = self.data.cfg.task_type == 17
    local is_complete =  prog >= self.data.cfg.parameter 
    if self.data.cfg.task_type == 17 then is_complete = prog <= self.data.cfg.parameter end
    UH.SetText(self.prog, string.format(Language.SportCards.Progress,ColorStr(is_special and (is_complete and 1 or 0) or prog,is_complete and COLORSTR.Green9 or COLORSTR.Red8),
        is_special and 1 or self.data.cfg.parameter))

    self.is_finish:SetActive(self.data.finish_flag == 1)
    self.btn_goto:SetActive(self.data.finish_flag == 0 and self.data.cfg.jump_interface > 0)
    if self.btn_reset then self.btn_reset:SetActive(self.data.cfg.task_group == SportsCardEmum.TYPE_DAY_TASK and self.data.finish_flag == 0 and self.data.refresh_num == 0) end
end 

function SportsCardsExpCell:OnClickGoTo()
    ViewMgr:OpenViewByKey(self.data.cfg.jump_interface)
    
    ViewMgr:CloseView(SportsCardsExp)    
    ViewMgr:CloseView(RewardView)    
end

function SportsCardsExpCell:OnClickReset()

    local info = {
            content = string.format( Language.SportCards.ResetTips,Config.jingjiyueka_auto.other[1].task_gold ),
            cancel = {
                name = nil,
                func = nil
            },
            confirm = {
                name = nil,
                func = function ()
                    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
                        rand_activity_type = ACTIVITY_TYPE.RAND_JING_JI_YUE_KA,
                        opera_type = 4,
                        param_1 = self.data.task_index
                    })
                ViewMgr:CloseView(DialogTipsView)
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)


end

------------------------  购买等级
SportsCardsLevel = SportsCardsLevel or  DeclareView("SportsCardsLevel", "reward/sports_cards_level")
VIEW_DECLARE_LEVEL(SportsCardsLevel, ViewLevel.L1)
function SportsCardsLevel:SportsCardsLevel() end
function SportsCardsLevel:OnClickClose() ViewMgr:CloseView(SportsCardsLevel) end 

function SportsCardsLevel:LoadCallback()
    if SportsCardsData.Instance:IsLevelMax() then 
        ViewMgr:CloseView(SportsCardsLevel)
        PublicPopupCtrl.Instance:Center(Language.SportCards.MaxLevel)
    end 
end

SportsCardsLevelPanel = SportsCardsLevelPanel or DeclareMono("SportsCardsLevelPanel", UIWFlushPanel)
function SportsCardsLevelPanel:SportsCardsLevelPanel()
    self.data = SportsCardsData.Instance
    self.data_cares = {
        {data = self.data.view_data,func = self.onFlush, init_call = false},
    }
    
    self.plus_level = 1
    self.is_max = self.data:IsLevelMax()
end 

function SportsCardsLevelPanel:onFlush()
    UH.SetText(self.cur_level,self.data.base_info.card_level)
    UH.SetText(self.next_level,(self.data.base_info.card_level+ self.plus_level)..Language.Common.Level)
    local level_info = self.data:GetLevelInfo()
    self.exp_prog:SetProgress(level_info.exp/level_info.total)

    if self.data:IsLevelMax() then 
        ViewMgr:CloseView(SportsCardsLevel)
        PublicPopupCtrl.Instance:Center(Language.SportCards.MaxLevel)
        return
    end 

    self:FlushList()
end 

function SportsCardsLevelPanel:FlushList()
    local list = {}
    local result_level = self.data.base_info.card_level + self.plus_level
    local max_plus = self.data:MaxLevel() - self.data.base_info.card_level
    for i = self.data.base_info.card_level + 1, result_level do 
        local cfg = self.data:CatchLevelCfg(i)
        if cfg.con_reward_item.item_id ~= nil then 
            local vo = BagData.Instance:ItemBuild(cfg.con_reward_item)
            -- local mark_pos = -1
            -- for key,value in pairs(list) do 
            --     if value.item_id == vo.item_id then 
            --         mark_pos = key
            --     end 
            -- end 
            -- if mark_pos > 0 then 
            --     list[mark_pos].num = list[mark_pos].num + vo.num
            -- else 
                table.insert(list,vo)
            -- end 
        end 

        if self.data.base_info.is_active_buy_flag == 1 then 
            for k,v in pairs(cfg.add_reward_item) do 
                local vo = BagData.Instance:ItemBuild(v)
                -- local mark_pos = -1
                -- for key,value in pairs(list) do 
                --     if value.item_id == vo.item_id then 
                --         mark_pos = key
                --     end 
                -- end 
                -- if mark_pos > 0 then 
                --     list[mark_pos].num = list[mark_pos].num + vo.num
                -- else 
                    table.insert(list,vo)
                -- end 
            end 
        end 
    end 
    self.reward_list:SetData(list)

    local step = Config.jingjiyueka_auto.other[1].buy_gold
    UH.SetText(self.price,step*self.plus_level)
    UH.SetText(self.next_level,(self.data.base_info.card_level+ self.plus_level)..Language.Common.Level)
    UH.SetText(self.plus_change,string.format( Language.SportCards.ChangeShow,self.data.base_info.card_level ,result_level))

    self.level_prog:SetProgress(self.plus_level/max_plus)

    self.BtnInter.Interactable = self.plus_level > 0
end

function SportsCardsLevelPanel:OnClickLevelPlus(oper)
    local max_plus = self.data:MaxLevel() - self.data.base_info.card_level

    local result = self.plus_level + oper
    if result < 0 then 
        PublicPopupCtrl.Instance:Center(Language.SportCards.BuyLimitClickMin)
        return 
    elseif result > max_plus then 
        PublicPopupCtrl.Instance:Center(Language.SportCards.BuyLimitClickMax)
        return 
    else 
        self.plus_level = result
    end 
   
    self:FlushList()
end


function SportsCardsLevelPanel:OnClickRed()
    if self.plus_level == 0 then 
        PublicPopupCtrl.Instance:Center(Language.SportCards.NoSelectLevel)
        return 
    end 

    if self.is_max  then 
        PublicPopupCtrl.Instance:Center(Language.SportCards.MaxLevel)
        return 
    end 

    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
        rand_activity_type = ACTIVITY_TYPE.RAND_JING_JI_YUE_KA,
        opera_type = 3,
        param_1 = self.plus_level
    })

    self.plus_level = 1
end

------------------------  解锁额外奖励
SportsCardsExtral = SportsCardsExtral or DeclareView("SportsCardsExtral", "reward/sports_cards_extral")
VIEW_DECLARE_LEVEL(SportsCardsExtral, ViewLevel.L1)
function SportsCardsExtral:SportsCardsExtral() end
function SportsCardsExtral:OnClickClose() ViewMgr:CloseView(SportsCardsExtral) end 

SportsCardsExtralPanel = SportsCardsExtralPanel or DeclareMono("SportsCardsExtralPanel", UIWFlushPanel)
function SportsCardsExtralPanel:SportsCardsExtralPanel()
    self.data = SportsCardsData.Instance
    self.data_cares = {
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(),func = self.SendBuy, init_call = false},
    }

    self.buy_flag = 0
end

function SportsCardsExtralPanel:onFlush()
    local cfg = self.data:CatchCfg()
    local list = {}
    for k,v in pairs(cfg) do 
        for i,j in pairs(v.add_reward_item) do 
            local vo = BagData.Instance:ItemBuild(j)
            -- local mark_pos = -1
            -- for key,value in pairs(list) do 
            --     if value.item_id == vo.item_id then 
            --         mark_pos = key
            --     end 
            -- end 
            -- if mark_pos > 0 then 
            --     list[mark_pos].num = list[mark_pos].num + vo.num
            -- else 
                table.insert(list,vo)
            -- end 
        end 
    end 

    self.reward_list:SetData(list)

    self.buy_money = Config.jingjiyueka_auto.other[1].buy_price
    self.BtnInter.Interactable = self.data.base_info.is_active_buy_flag == 0

    UH.SetText(self.btn_num,self.buy_money/10)
end


function SportsCardsExtralPanel:OnClickBuy()
    if  self.data.base_info.is_active_buy_flag == 1 then 
        PublicPopupCtrl.Instance:Center(Language.SportCards.BuyedTips)
        return 
    end 

    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
        rand_activity_type = ACTIVITY_TYPE.RAND_JING_JI_YUE_KA,
        opera_type = 5
    })

end

function SportsCardsExtralPanel:SendBuy()
    if self.buy_flag == 0 then 
        ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_JINGJI_YUEKA, self.buy_money)
        self.buy_flag = 1
    end 
    ViewMgr:CloseView(SportsCardsExtral)
end