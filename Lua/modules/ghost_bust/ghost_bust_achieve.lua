GhostBustAchieve = GhostBustAchieve or DeclareView("GhostBustAchieve", "ghost_bust/ghost_bust_achievement", Mod.GhostBust.Main)

function GhostBustAchieve:GhostBustAchieve()
    self.data = GhostBustData.Instance
    
end 

function GhostBustAchieve:OpenCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    self.FlushPanel.pages[1]:StatusChange(1)
    self.FlushPanel.pages[1]:ClickItem()
end

function GhostBustAchieve:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(GhostBustAchieve)
end

GhostBustAchievePanel = GhostBustAchievePanel or DeclareMono("GhostBustAchievePanel", UIWFlushPanel)
function GhostBustAchievePanel:GhostBustAchievePanel()
    self.data = GhostBustData.Instance

    self.data_cares = {
        {data = self.data.reward_info,func = self.FlushView,init_call = false},        
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(),func = self.SendBuy, init_call = false},
    }

     self.init = false
    self.init_data = nil

    -- self.list:SetCompleteCallBack(function()
    --     if not self.init and self.init_data ~= nil then 
    --         self.list:AutoSlide(self.init_data,false)
    --         self.init = true
    --     end 
    -- end)
end 

function GhostBustAchievePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.Start_Wait ~= nil then
        TimeHelper:CancelTimer(self.Start_Wait)
        self.Start_Wait = nil
    end

    if self.jump_wait ~= nil then
        TimeHelper:CancelTimer(self.jump_wait)
        self.jump_wait = nil
    end

end

function GhostBustAchievePanel:FlushView()
    self.pages[2]:StatusChange(self.data:IsUnLock() and 1 or 0)

    local list,select_data = self.data:GetAchieveList(self.page)
    self.init_data = select_data

    -- LogError("?策划你当个人吧！这里",#list,"个项怎么可能快得起来")
    self.list:SetData(list)

    self.btn_onekey:SetActive(self.data.reward_info.cmd_mark[1] == 0 and self.page == 2)
    self.cost_tips.gameObject:SetActive(self.data.reward_info.cmd_mark[1] == 0 and self.page == 2)
    -- self.unlock_tips:SetActive(self.data.reward_info.cmd_mark[1] == 1)

    UH.SetText(self.cost_tips, string.format(Language.GhostBust.CostTips,Config.pet_tried_auto.other[1].rmb/10))
    UH.SetText(self.btn_cost, Config.pet_tried_auto.other[1].rmb/10)
    self.nomaltips:SetActive(self.page == 1 or (self.data.reward_info.cmd_mark[1] == 1 and self.page == 2)) 


    -- if self.Init_Wait == nil then 
    --     self.Init_Wait= TimeHelper:AddDelayFrameTimer(function ()
            
    --     end,2)
    -- end 

    if not self.init_wait then 
        self.list_anim:Play()
        self.init_wait = true
    end 

    if self.Start_Wait == nil then 
        self.Start_Wait= TimeHelper:AddDelayTimer(function ()
            if self.init_data ~= nil then 
                self.list:JumpVerticalIndex(self.init_data.seq,135,2)
            end 
        end,1)
    else
        if self.jump_wait ~= nil then
            TimeHelper:CancelTimer(self.jump_wait)
            self.jump_wait = nil
        end
        
        self.list_scroll.inertia = false
        if self.init_data ~= nil then 
            self.list:JumpVerticalIndex(self.init_data.seq,135,2)
            self.jump_wait =  TimeHelper:AddDelayFrameTimer(function ()
                self.list_scroll.inertia = true
            end,2)
        end 
    end 
end

function GhostBustAchievePanel:OnClickPage(index)
    -- if self.pages[index]:IsLock() then 
    --     PublicPopupCtrl.Instance:Center(Language.GhostBust.LockAchieve)
    --     return 
    -- end 
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    self.page = index

    self:FlushView()
end

function GhostBustAchievePanel:OnClickOneKey()
    local flag,param = self.data:CheckCanBuy()
    if not flag then 
        PublicPopupCtrl.Instance:Center(string.format(Language.GhostBust.AchieveBuyTips,param))
        return 
    end    


    if self.data.reward_info.cmd_mark[1] == 1 then 
        return 
    end

    if not ServerActivityCtrl:IsCanItemBuy(BUY_ITEM_TYPE.TYPE_YAO_XING_SHI_LIAN, Config.pet_tried_auto.other[1].rmb/10) then 
        ServerActivityCtrl.Instance:SendCmdBuyItemCheckReq(
            BUY_ITEM_TYPE.TYPE_YAO_XING_SHI_LIAN,Config.pet_tried_auto.other[1].rmb,1,1,0)
    else 
        ServerActivityCtrl.Instance:SendBuyCmdReq(BUY_ITEM_TYPE.TYPE_YAO_XING_SHI_LIAN, Config.pet_tried_auto.other[1].rmb/10,1,1)
    end 
end

function GhostBustAchievePanel:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_YAO_XING_SHI_LIAN, Config.pet_tried_auto.other[1].rmb)
end

function GhostBustAchievePanel:OnClickAchieve(data)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    local info = self.data:GetAchieveStatus({seq = data.seq,index = data.type})
    local can_get = info.is_pass and info.achieve_1 and info.achieve_2
    local is_sp = data.achievement_2 ~= nil
    if not can_get then 
        if is_sp then 
            if not info.achieve_2 then 
                PublicPopupCtrl.Instance:Center(Language.GhostBust.AchieveBuyLack)
            else
                PublicPopupCtrl.Instance:Center(Language.GhostBust.AchieveLack)
            end 
        else
            PublicPopupCtrl.Instance:Center(Language.GhostBust.AchieveLack)
        end 
        return 
    end 
    local operate = is_sp and 3 or 2
    GhostBustCtrl.Instance:SendGhostBustSeq(operate,data.seq)
end

GhostBustAchieve = GhostBustAchieve or DeclareMono("GhostBustAchieve")
-- status == 1 开启 status == 0 关闭
function GhostBustAchieve:StatusChange(status)
    self.inter.Interactable = status == 1
    self.lock:SetActive(status ~= 1)
end

function GhostBustAchieve:ClickItem()
    self.ItemClick:Trigger()
end

function GhostBustAchieve:IsLock()
    return self.lock.activeSelf == true
end

GhostBustAchieveItem = GhostBustAchieveItem or DeclareMono("GhostBustAchieveItem", UIWidgetBaseItem)
function GhostBustAchieveItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.RenameGame(self,Format("achieve_cell%s",data.seq))
    UH.SetText(self.title, data.name)
    self.reward_list:SetData(BagData.Instance:ItemListBuild(data.extra_reward))

    local param = GhostBustData.Instance:GetAchieveStatus({seq = data.seq,index = data.type})
    -- LogError("cufks",data.name,param)
    local can_get = param.is_pass and param.achieve_1 and param.achieve_2
    self.btn_inter.Interactable = can_get
    UH.SetText(self.btn_text, can_get and Language.GhostBust.Ready or Language.GhostBust.UnReady )

    self.btn:SetActive(not param.is_catched)
    self.complete:SetActive(param.is_catched)

    self.list_able.enabled = #data.extra_reward >= 4
end 

GhostBustAchieveRewardItem = GhostBustAchieveRewardItem or DeclareMono("GhostBustAchieveRewardItem", UIWidgetBaseItem)
function GhostBustAchieveRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.Call:SetData(data)
    UH.SetText(self.name, data:QuaName(true))
end