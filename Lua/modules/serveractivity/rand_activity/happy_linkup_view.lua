HappyLinkupView = HappyLinkupView or DeclareView("HappyLinkupView","serveractivity/happy_linkup", Mod.RandActivity.HappyLinkUp)

function HappyLinkupView:HappyLinkUpView()

end

function HappyLinkupView:LoadCallback()
        --  请求信息
        ServerActivityCtrl.Instance:SendRandActivityOperaReq({
            rand_activity_type = ACTIVITY_TYPE.RAND_HAPPY_LINKUP,
            opera_type = 0,
        })

        if HappyLinkupData.Instance:IsGamePlaying() then 
            ServerActivityCtrl.Instance:SendRandActivityOperaReq({
                rand_activity_type = ACTIVITY_TYPE.RAND_HAPPY_LINKUP,
                opera_type = 2,
                param_1 = 0,
            })
        end 
    self.panel:FlushView()

end 

function HappyLinkupView:CloseCallback()

end

function HappyLinkupView:OnClickTips()
    PublicPopupCtrl.Instance:HelpTip(Config.happy_look_auto.other[1].desc)
end
---------------------------------------HappyLinkupPanel-------------------------------------
HappyLinkupPanel = HappyLinkupPanel or DeclareMono("HappyLinkupPanel", UIWFlushPanel)
function HappyLinkupPanel:HappyLinkupPanel()
    self.data = HappyLinkupData.Instance
    self.server_data = ServerActivityData.Instance

    self.data_cares = {
		{data = self.server_data:GetHappyLookInfo(), func = self.FLushPanel},
	}

    self.select_box = nil
end

function HappyLinkupPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
end

function HappyLinkupPanel:OnClickClose()
    if self.data:IsGamePlaying() then 
        local info = {
            content = Language.HappyLink.CancelWarn,
            cancel = {
                name = nil,
                func = nil
            },
            confirm = {
                name = nil,
                func = function ()
                    ViewMgr:CloseView(HappyLinkupView)
                    -- 强制结束
                    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
                        rand_activity_type = ACTIVITY_TYPE.RAND_HAPPY_LINKUP,
                        opera_type = 2,
                        param_1 = 0,
                    })
                    ViewMgr:CloseView(DialogTipsView)
                end
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)

        return 
    end 
    ViewMgr:CloseView(HappyLinkupView)
end

-- 跟随界面启动刷新
function HappyLinkupPanel:FlushView()
    local list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_HAPPY_LINKUP,Config.happy_look_auto.reward,"type")
    self.box_list:SetData(list)

    local item_list = {}
    for k,v in pairs(Config.happy_look_auto.other[1].preview) do 
        local vo = Item.Create(v)
        table.insert(item_list,vo)
    end 
    self.reward_show:SetData(item_list)

    UH.SetText(self.challenge_notime, string.format( Language.HappyLink.FlushChallenge,Config.happy_look_auto.other[1].frequency))    

    self.box_list:SetCompleteCallBack(function()
        local data = self.server_data:GetHappyLookInfo()
        local index = data.chanllenge_index > -1 and data.chanllenge_index + 1 or 1
        self.box_list:GetItem(index).ItemClick:Trigger()
        for k,v in pairs(self.box_list.item_list) do 
            v:InterFlush(k == index)
        end 

        for k,v in pairs(self.box_list.item_list) do
            v:ChangeGameStart()
        end 

        self.box_list:AutoSlide(self.box_list:GetItem(index):GetData(),false)
    end)

    self.act_timer:CreateActTime(ACTIVITY_TYPE.RAND_HAPPY_LINKUP,nil,Language.HappyLink.ActTime)
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

-- 点击领取按钮
function HappyLinkupPanel:OnClickButton()
    if self.data:IsRewardWait() then 
        local index,is_succ = self.data:GetGameSuccess()
        if is_succ == 0 then 
            ServerActivityCtrl.Instance:SendRandActivityOperaReq({
                rand_activity_type = ACTIVITY_TYPE.RAND_HAPPY_LINKUP,
                opera_type = 3,
            })
        else
            PublicPopupCtrl.Instance:Center(Language.HappyLink.NeedSuccessReward)
        end 
    end 
end

function HappyLinkupPanel:OnClickChallenge()
    if self.data:IsGamePlaying() then return end 

    if self.data:IsRewardWait() then 
        local data = self.server_data:GetHappyLookInfo()
        if data.challenge_is_succ == 0 then 
            PublicPopupCtrl.Instance:Center(Language.HappyLink.NeedFailReward)
        else 
            PublicPopupCtrl.Instance:Center(Language.HappyLink.NeedSuccessReward)
        end 
        return 
    end 

    if self.data:IsGameWait() then 
	    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
            rand_activity_type = ACTIVITY_TYPE.RAND_HAPPY_LINKUP,
            opera_type = 1,
            param_1 = self.select_box.seq1,
        })
    end 
end

function HappyLinkupPanel:OnClickBox(data)
    if self.data:IsGamePlaying() then
        PublicPopupCtrl.Instance:Center(Language.HappyLink.GamePlaying)
        return 
    end

    if self.data:IsGameWait() then
        local index = self.box_list.data_t[data]
        for k,v in pairs(self.box_list.item_list) do 
            v:InterFlush(k == index)
        end 

        self.select_box = data
        self.linkPart:PreView(tonumber(self.select_box.quantity))
    elseif self.data:IsRewardWait() then 
        local index,is_succ = self.data:GetGameSuccess()
        self.linkPart:PreView(tonumber(self.box_list:GetItem(index+1):GetData().quantity))
    end 

    -- -- 领取道具
    -- if self.data:IsRewardWait() then 
    --     local index,is_succ = self.data:GetGameSuccess()
    --     if is_succ == 1 and index == data.seq1 then 
    --         ServerActivityCtrl.Instance:SendRandActivityOperaReq({
    --             rand_activity_type = ACTIVITY_TYPE.RAND_HAPPY_LINKUP,
    --             opera_type = 3,
    --         })
    --     elseif is_succ == 0 then 
    --         PublicPopupCtrl.Instance:Center(Language.HappyLink.NeedFailReward)
    --     else 
    --         PublicPopupCtrl.Instance:Center(Language.HappyLink.NeedSuccessReward)
    --     end 
    -- end 
end

function HappyLinkupPanel:OnClickBoxPreView(data)
    if self.data:IsGamePlaying() then
        PublicPopupCtrl.Instance:Center(Language.HappyLink.GamePlaying)
        return 
    end 
    if self.data:IsGameWait() then
        local pos = self.box_list:GetItem(self.box_list.data_t[data]).pos
        pos:SetParent(self.total,true)
        local item_list = {}
        for k,v in pairs(data.reward_item) do 
            local vo = Item.Create(v)
            table.insert(item_list,vo)
        end 
        local create_pos = pos:GetComponent(typeof(UnityEngine.RectTransform)).localPosition
        self:OpenShowItem({item_list = item_list,pos = Vector3.New(create_pos.x,create_pos.y,0),pivot = 0})
        -- ViewMgr:OpenView(ShowItemView,{item_list = item_list,pos = Vector3.New(create_pos.x,create_pos.y,0),pivot = 0})
        pos:SetParent(self.box_list:GetItem(self.box_list.data_t[data]).pos_parent,true) 
    end

    if self.data:IsRewardWait() then
        local index,is_succ = self.data:GetGameSuccess()
        if is_succ == 1 and index == data.seq1 then 
            ServerActivityCtrl.Instance:SendRandActivityOperaReq({
                rand_activity_type = ACTIVITY_TYPE.RAND_HAPPY_LINKUP,
                opera_type = 3,
            })
        elseif is_succ == 0 then 
            PublicPopupCtrl.Instance:Center(Language.HappyLink.NeedFailReward)
        else 
            PublicPopupCtrl.Instance:Center(Language.HappyLink.NeedSuccessReward)
        end 
    end 
end

function HappyLinkupPanel:OpenShowItem(data)
    self.showitem_obj:SetActive(true)
    self.showitem_panel:SetData(data)
end

function HappyLinkupPanel:OnClickCloseShowItem()
    self.showitem_obj:SetActive(false)
end 

function HappyLinkupPanel:FLushPanel()
    local data = self.server_data:GetHappyLookInfo()
    local time = self.data:GetCurTimer()
    UH.SetText(self.challenge_notime, string.format( Language.HappyLink.FlushChallenge,Config.happy_look_auto.other[1].frequency))   
    UH.SetText(self.challenge_time, string.format( Language.HappyLink.CanChallenge,time,Config.happy_look_auto.other[1].frequency))   

    self.challenge_time.gameObject:SetActive(not self.data:IsGamePlaying() and time >0)
    self.challenge_limit.gameObject:SetActive(self.data:IsGamePlaying())
    self.challenge_notime.gameObject:SetActive(not self.data:IsGamePlaying() and time == 0)

    self.btn_button:SetActive(self.data:IsRewardWait() and data.challenge_is_succ == 0)
    self.btn_reset:SetActive(self.data:IsGamePlaying())

    self.box_obj:SetActive(self.data:IsGamePlaying() or (self.data:IsRewardWait() and data.challenge_is_succ == 0))

    for k,v in pairs(self.box_list.item_list) do
        v:ChangeGameStart()
    end 

    self.click_red = self.click_red or UiRedPoint.New(self.pic_red, Vector3.New(0,0,0))
    self.click_red:SetNum((data.challenge_is_succ == 0 and self.data:IsRewardWait()) and 1 or 0)

    if self.data:IsRewardWait() then 
        local item = self.box_list:GetItem(data.chanllenge_index + 1 )
        if item ~= nil then 
            self.linkPart:PreView(tonumber(self.box_list:GetItem(data.chanllenge_index+1):GetData().quantity))
        end 
        if data.challenge_is_succ == 0 then 
            PublicPopupCtrl.Instance:Center(Language.HappyLink.ChallengeFail)
        elseif data.challenge_is_succ == 1 then 
            PublicPopupCtrl.Instance:Center(Language.HappyLink.ChallengeSuccess)
        end 
    elseif self.data:IsGameWait() then 
        if self.select_box == nil and #self.box_list.item_list > 1 then 
            self.box_list:GetItem(1).ItemClick:Trigger()

            for k,v in pairs(self.box_list.item_list) do 
                v:InterFlush(k == 1)
            end 
        end 
    end 
end

HappyLinkupItemShow = HappyLinkupItemShow or DeclareMono("HappyLinkupItemShow", UIWFlushPanel)
function HappyLinkupItemShow:SetData(data)
    UH.LocalPosG(self.ObjList, data.pos or Vector3.zero)
    self.ShowList:SetData(data.item_list)
    if data.pivot then
        self.RectTrans.pivot = Vector2.New(data.pivot,0.5)
    end
end 

HappyLinkupItemShowCell = HappyLinkupItemShowCell or DeclareMono("HappyLinkupItemShowCell", UIWidgetBaseItem)
function HappyLinkupItemShowCell:HappyLinkupItemShowCell() end 
function HappyLinkupItemShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.call:SetData(data)
end

function HappyLinkupItemShowCell:OnClickItem()
    ItemInfoData.Instance:SetClickDataPos(UH.GetRootPosition(self.openpos))
    local click_func =  self.call:getClickDel()
    if click_func then
		click_func(self)
	end
end

HappyLinkupBoxCell = HappyLinkupBoxCell or DeclareMono("HappyLinkupBoxCell", UIWidgetBaseItem)
function HappyLinkupBoxCell:HappyLinkupBoxCell() end 
function HappyLinkupBoxCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    
    UH.SetText(self.name,self.data.name)
    UH.SpriteName(self.box_icon,"BaoHe"..self.data.picture)

    self.eff_list = {
        [1] = 4161011,
        [2] = 4161009,
        [3] = 4161010,
    }
end

function HappyLinkupBoxCell:ChangeGameStart()
    local server_data = ServerActivityData.Instance:GetHappyLookInfo()
    self.toggle.enabled = (server_data.chanllenge_index == self.data.seq1 and HappyLinkupData.Instance:IsGamePlaying()) 
        or (not HappyLinkupData.Instance:IsGamePlaying() and not HappyLinkupData.Instance:IsRewardWait() )

    local remind_click = HappyLinkupData.Instance:IsRewardWait() and server_data.chanllenge_index == self.data.seq1 and server_data.challenge_is_succ == 1
    self:RemindClick(remind_click)

    self.box_icon.gameObject:SetActive(not remind_click)
    self.box_effect:SetActive(remind_click)

    if remind_click then 
        self.eff = self.EffectTool:Play(self.eff_list[self.data.picture])
    elseif self.eff ~= nil then 
        self.EffectTool:Stop(self.eff)
    end 
end

function HappyLinkupBoxCell:InterFlush(flag)
    self.Inter.Interactable = flag
end

function HappyLinkupBoxCell:RemindClick(flag)
    self.click_red = self.click_red or UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
    self.click_red:SetNum(flag and 1 or 0)
end

HappyLinkupLinkPart = HappyLinkupLinkPart or DeclareMono("HappyLinkupLinkPart", UIWFlushPanel)
function HappyLinkupLinkPart:HappyLinkupLinkPart()
    self.data = HappyLinkupData.Instance
    self.server_data = ServerActivityData.Instance

    self.data_cares = {
		{data = self.server_data:GetHappyLookInfo(), func = self.FLushGameStatus},
	}

    self.select_group = {}
    self.game_state = 0

    self.grid_list:SetCompleteCallBack(function()
        if self.game_start_show then
            -- self.show_timer = TimeHelper:AddDelayTimer(function() 
                for k,v in pairs(self.grid_list.item_list) do 
                    v:FlushGame(1)
                end 
            -- end,1)
            self.game_start_show = false
        end 

        if self.pre_view_show then 
            for k,v in pairs(self.grid_list.item_list) do 
                v:FlushGame(0)
            end 
            self.pre_view_show = false
        end 
    end)
    self.grid_list.StepCount = 12
    self.link_list.StepCount = 20
    self.link_list:SetCompleteCallBack(
        function()
            for k,v in pairs(self.link_list.item_list) do 
                v:FlushLinkShow()
            end 
            for k,v in pairs(self.grid_list.item_list) do 
                v:FlushDisapper()
                v:FlushFail()
            end 
        end
    )

    self.game_timer:SetShowCallBack(function (t)
        return Language.HappyLink.ChallengeLimit..(t.min*60+t.s)..Language.HappyLink.Sceond
    end)

    -- self.is_game_over = true
    self.game_limt = {}
end

function HappyLinkupLinkPart:ForceStartGame()
    if not HappyLinkupData.Instance:IsGamePlaying() then 
        return 
    end 

    for k,v in pairs(self.grid_list.item_list) do 
        if v.animator:GetInteger("status") == 0 then 
            self:FLushGameStatus()
            break
        end 
    end 
end

function HappyLinkupLinkPart:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
    self:GameComplete()
end

function HappyLinkupLinkPart:FLushGameStatus()
    if self.data:IsGamePlaying() then 
        self:StartGameLimit()
        self:LinkStart(self.total.select_box)
    end 
    if self.data:IsRewardWait() then 
        self:GameComplete()
    end 
    if self.data:IsGameWait() then 

    end 
end

-- 游戏完成
function HappyLinkupLinkPart:GameComplete()
    self.data:ClearCache()
    self.game_timer:CloseCountDownTime()
end

-- 预览可以提前加载好全部item
function HappyLinkupLinkPart:PreView(type)
    local grid_size = HappyLinkupData.GridSize[type]
    local link_size = HappyLinkupData.LinkSize[type]
    local pre_view_data = self.data:PreViewGridData(grid_size.row,grid_size.col)
    self.grid_list:SetData(pre_view_data)
    self.ListGrid.constraintCount = grid_size.col

    self.pre_view_show = true

    local pre_link_data = self.data:MatchLinkData(type)
    self.link_list:SetData(pre_link_data)
    self.LinkGrid.constraintCount = link_size.col
end

-- 开始连连看游戏
function HappyLinkupLinkPart:LinkStart(box_data)
    if box_data == nil then 
        PublicPopupCtrl.Instance:Center(Language.HappyLink.SelectBoxEmpty)        
        return
    end 
    self.select_group = {}
    
    -- tonumber(self.select_box.quantity)
    local grid_size = HappyLinkupData.GridSize[tonumber(box_data.quantity)]
    local link_size = HappyLinkupData.LinkSize[tonumber(box_data.quantity)]
    self.gameplay_match = self.data:MatchGridData(grid_size.row,grid_size.col)
    -- self.gameplay_match-= self.start_match

    self.game_start_show = true

    self.grid_list:SetData(self.gameplay_match)
    self.ListGrid.constraintCount = grid_size.col

    self.link_match = self.data:MatchLinkData(tonumber(box_data.quantity))
    -- self.link_match = self.start_link_match
    self.link_list:SetData(self.link_match)

    self.LinkGrid.constraintCount = link_size.col
end

function HappyLinkupLinkPart:StartGameLimit()
    local data = self.server_data:GetHappyLookInfo()
    self.game_timer:StampTime(data.challenge_end_timestamp,TimeType.Type_Special_5)

    self.force_timer = TimeHelper:AddDelayTimer(function() 
        self:ForceStartGame()
    end,3)
end

-- 重置排序
function HappyLinkupLinkPart:OnClickReset()
    self.gameplay_match = self.data:ResetLinkMacth(self.gameplay_match or {})

    self.grid_list:SetData(self.gameplay_match)
end

function HappyLinkupLinkPart:OnClickLinkCell(data)
    if data.is_pre then return end 

    local index = self.grid_list:GetSelectedIndex(data)
    if self.grid_list.item_list[index] == nil then return end 
    
    self.grid_list.item_list[index]:SetSelect(true)

    if not self.data:AllIsSameCell(data,self.select_group) and not data.disapper then 
        table.insert(self.select_group,data)
    end 

    if #self.select_group > 1 then 
        local game_over = false
        local is_succ = false
        self.gameplay_match,self.link_match,game_over,is_succ = self.data:FlushGamePlayMatch(self.gameplay_match,self.link_match ,self.select_group)
        self.grid_list:SetData(self.gameplay_match)
        self.link_list:SetData(self.link_match)

        self.select_group = {}

        if game_over then 
            --  请求信息
            ServerActivityCtrl.Instance:SendRandActivityOperaReq({
                rand_activity_type = ACTIVITY_TYPE.RAND_HAPPY_LINKUP,
                opera_type = 2,
                param_1 = 1,
            })
        end 
    end 
end

function HappyLinkupLinkPart:StrenSuccess()
    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
        rand_activity_type = ACTIVITY_TYPE.RAND_HAPPY_LINKUP,
        opera_type = 2,
        param_1 = 1,
    })
end

function HappyLinkupLinkPart:StrenFail()
    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
        rand_activity_type = ACTIVITY_TYPE.RAND_HAPPY_LINKUP,
        opera_type = 2,
        param_1 = 0,
    })
end

HappyLinkupGridCell = HappyLinkupGridCell or DeclareMono("HappyLinkupGridCell", UIWidgetBaseItem)
function HappyLinkupGridCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    -- self.select:SetActive(false)
   
    self:ShowPreView(self.data.is_pre)

    self.gameObject.name = self.data.col .."-"..self.data.row.."-"..(self.data.disapper and "disapper" or "apper")
end

function HappyLinkupGridCell:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    TimeHelper:CancelTimer(self.show_timer)
    self.show_timer = nil
end 

function HappyLinkupGridCell:SetSelect(flag) 
    self.select:SetActive(flag)
end

function HappyLinkupGridCell:ShowPreView(flag)

    if not flag then UH.SpriteName(self.icon, "icon_"..self.data.icon) end 
end

-- 只有1 和 0 两种，1开始0结束（或未开启
function HappyLinkupGridCell:FlushGame(game_status)
    self.animator:SetInteger(APH("status"),game_status)
end

function HappyLinkupGridCell:FlushDisapper()
    self.show_timer = TimeHelper:AddDelayTimer(function() 
        if not ViewMgr:IsOpen(HappyLinkupView) then return end 
        if self.select ~= nil then 
            self.select:SetActive(false)
            self.total:SetActive(not self.data.disapper)
        end 
    end,0.4)
end 

function HappyLinkupGridCell:FlushFail()
    self.fail:SetActive(self.data.fail)
    self.show_timer = TimeHelper:AddDelayTimer(function() 
        if not ViewMgr:IsOpen(HappyLinkupView) then return end 
        self.select:SetActive(false)
        self.fail:SetActive(false)
        self.data.fail = false
    end,0.4) 
end

HappyLinkupLinkCell = HappyLinkupLinkCell or DeclareMono("HappyLinkupLinkCell", UIWidgetBaseItem)
function HappyLinkupLinkCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.gameObject.name = self.data.col .."-"..self.data.row.."-"..(self.data.disapper and "disapper" or "apper")
    self:EndLinkShow()
end

function HappyLinkupLinkCell:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    TimeHelper:CancelTimer(self.show_timer)
    self.show_timer = nil
end 

function HappyLinkupLinkCell:FlushLinkShow()
    if self.data.link_info == nil then return end 
    if self.data.link_info.is_start or self.data.link_info.is_end then return end 

    self.out:SetActive(self.data.is_out)
    self.strength:SetActive(not self.data.is_out)

    -- 前一个发方向是反着求的
    local pass_dir = HappyLinkupData.Instance:GetStepLink(self.data,self.data.link_info.pass_info)
    local next_dir = HappyLinkupData.Instance:GetStepLink(self.data.link_info.next_info,self.data)

    if self.data.is_out then 
        local show_out = 1
        if self.data.row == 0 and (self.data.link_info.next_info.row == 0 or self.data.link_info.pass_info.row == 0 ) then 
            show_out = 2
        elseif self.data.row == self.data.t_row and (self.data.link_info.next_info.row == self.data.t_row or self.data.link_info.pass_info.row == self.data.t_row ) then
            show_out = 4
        elseif self.data.col == 0 and (self.data.link_info.next_info.col == 0 or self.data.link_info.pass_info.col == 0 ) then 
            show_out = 1
        elseif self.data.col == self.data.t_col and (self.data.link_info.next_info.col == self.data.t_col or self.data.link_info.pass_info.col == self.data.t_col ) then
            show_out = 3
        end 

        self.outpart[show_out].gameObject:SetActive(true)

        self.outpart[show_out]:ShowDir(pass_dir,next_dir) 
    else
        self.strength_part:ShowDir(pass_dir,next_dir)
    end 

    self.show_timer = TimeHelper:AddDelayTimer(function() 
        self:EndLinkShow()
    end,0.2)
end 

function HappyLinkupLinkCell:EndLinkShow()
    if not ViewMgr:IsOpen(HappyLinkupView) then return end 
    
    self.out:SetActive(false)
    self.strength:SetActive(false)
    for i =1,4 do 
        self.outpart[i].gameObject:SetActive(false)
    end 
end 

HappyLinkupLinkCellOutLink = HappyLinkupLinkCellOutLink or DeclareMono("HappyLinkupLinkCellOutLink", UIWidgetBaseItem)
function HappyLinkupLinkCellOutLink:Clear() 
    self.line_out:SetActive(false)
    self.line_pass:SetActive(false)

    if self.direction == 2 or self.direction == 4 then
        self.left:SetActive(false)
        self.right:SetActive(false)
    end 
    if self.direction == 1 or self.direction == 3 then 
        self.up:SetActive(false)
        self.down:SetActive(false)
    end 
end 

function HappyLinkupLinkCellOutLink:ShowDir(pass,next)
    self:Clear()

    if (pass == 3 or next == 1) and self.left ~= nil then 
        self.left:SetActive(true)
    end 

    if (pass == 4 or next == 2) and self.up ~= nil then 
        self.up:SetActive(true)
    end 

    if (pass == 1 or next == 3) and self.right ~= nil then 
        self.right:SetActive(true)
    end 
    
    if (pass == 2 or next == 4) and self.down ~= nil then 
        self.down:SetActive(true)
    end 

    if pass == next then 
        self.line_pass:SetActive(true)
    else 
        self.line_out:SetActive(true)
    end 
end 

HappyLinkupLinkCellStrenLink = HappyLinkupLinkCellStrenLink or DeclareMono("HappyLinkupLinkCellStrenLink", UIWidgetBaseItem)
function HappyLinkupLinkCellStrenLink:Clear() 
    self.row_pass:SetActive(false)
    self.col_pass:SetActive(false)
    self.up_left:SetActive(false)
    self.up_right:SetActive(false)
    self.down_right:SetActive(false)
    self.down_left:SetActive(false)
end 

function HappyLinkupLinkCellStrenLink:ShowDir(pass,next)
    self:Clear()
    if pass == next and (pass == 1 or pass == 3) then 
        self.col_pass:SetActive(true)
        return 
    end 
    if pass == next and (pass == 2 or pass == 4) then 
        self.row_pass:SetActive(true)
        return 
    end 

    -- local row_dir = ( pass == 2 or next == 2 ) and "up" or "down"
    -- local col_dir = ( pass == 1 or next == 1 ) and "left" or "right"

    -- self[row_dir.."_"..col_dir]:SetActive(true)
    self.up_right:SetActive((pass == 1 or next == 3 ) and (pass == 4 or next == 2)) 
    self.up_left:SetActive((pass == 3 or next == 1 ) and (pass == 4 or next == 2)) 
    self.down_right:SetActive((pass == 1 or next == 3 ) and (pass == 2 or next == 4)) 
    self.down_left:SetActive((pass == 3 or next == 1 ) and (pass == 2 or next == 4)) 
end 

HappyLinkupRewardPre = HappyLinkupRewardPre or DeclareMono("HappyLinkupRewardPre", UIWidgetBaseItem)
function HappyLinkupRewardPre:SetData(data)
    self.Item:SetData(data)
end