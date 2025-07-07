ActFlowerTouchView = ActFlowerTouchView or DeclareView("ActFlowerTouchView","serveractivity/flower_touch/flower_touch", Mod.RandActivity.FlowerTouch)
function ActFlowerTouchView:ActFlowerTouchView()

end

function ActFlowerTouchView:LoadCallback()
    --  请求信息
    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
        rand_activity_type = ACTIVITY_TYPE.RAND_YUN_ZE_BLOOMING_RECORD,
        opera_type = 0,
    })

        
end

function ActFlowerTouchView:OnClickBack()
    if ActFlowerTouchData.Instance:CheckIsGameStart() then 
        local info = {
            content = Language.FlowerTouch.CancelWarn,
            cancel = {
                name = nil,
                func = nil
            },
            confirm = {
                name = nil,
                func = function ()
                    ViewMgr:CloseView(ActFlowerTouchView)
                    -- 强制结束
                    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
                        rand_activity_type = ACTIVITY_TYPE.RAND_YUN_ZE_BLOOMING_RECORD,
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
    ViewMgr:CloseView(ActFlowerTouchView)
end

-- 沟通链接组
---------------------------------------ActFlowerTouchPanel-------------------------------------
ActFlowerTouchPanel = ActFlowerTouchPanel or DeclareMono("ActFlowerTouchPanel", UIWFlushPanel)
function ActFlowerTouchPanel:ActFlowerTouchPanel()
    self.data = ActFlowerTouchData.Instance
    -- self.reward_list:SetData()
    self.data_cares = {
		{data = self.data.flower_info, func = self.FLushPanel},
	}

    -- self.btn_edit:SetActive( DEBUG_MODE == true )

    self.act_timer:CreateActTime(ACTIVITY_TYPE.RAND_YUN_ZE_BLOOMING_RECORD,TimeType.Type_Special_4,Language.FlowerTouch.ActTime)
end 

function ActFlowerTouchPanel:FLushPanel()
    UH.SetText(self.point, Language.FlowerTouch.CurIntegral..self.data.flower_info.cur_intergral)
    local list = self.data:GetRewardList()

    for i = 1,5 do 
        self.reward_list[i]:SetData(list[i])
    end 
    
    UH.SetText(self.play_time, Language.FlowerTouch.PlayTime..self.data:GetTodayTime().."/"..Config.yunzefanhualu_auto.other[1].frequency)
end

-- 点击奖励
function ActFlowerTouchPanel:OnClickReward(data)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    if self.data:CheckIsGameStart() then
        -- PublicPopupCtrl.Instance:Center(Language.HappyLink.GamePlaying)
        return 
    end 

    if data.flag then 
        ServerActivityCtrl.Instance:SendRandActivityOperaReq({
            rand_activity_type = ACTIVITY_TYPE.RAND_YUN_ZE_BLOOMING_RECORD,
            opera_type = 3,
            param_1 = data.index - 1,
        })
        return 
    end 

    local pos = self.reward_list[data.index].pos
    pos:SetParent(self.total,true)
    local item_list = {}
    for k,v in pairs(data.reward_item or {}) do 
        local vo = Item.Create(v)
        table.insert(item_list,vo)
    end 
    local create_pos = pos:GetComponent(typeof(UnityEngine.RectTransform)).localPosition
    self:OpenShowItem({item_list = item_list,pos = Vector3.New(create_pos.x,create_pos.y,0),pivot = 0})
    pos:SetParent(self.reward_list[data.index].pos_parent,true) 
end

function ActFlowerTouchPanel:OpenShowItem(param_t)
    self.showitem:SetData(param_t)
    self.showitem.gameObject:SetActive(true)
end

function ActFlowerTouchPanel:OnClickCloseShowItem()
    self.showitem.gameObject:SetActive(false)
end

-- 点击
function ActFlowerTouchPanel:OnClickTips()
    PublicPopupCtrl.Instance:HelpTip(Config.yunzefanhualu_auto.other[1].desc) 
end

function ActFlowerTouchPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function ActFlowerTouchPanel:OnClickEdit()
    self.edit_part:SetActive(true)
    self.game_part.gameObject:SetActive(false)
end

--- 打地鼠主控件组
--------------------FlowerTouchTouchPart----------------------------
ActFlowerTouchTouchPart = ActFlowerTouchTouchPart or DeclareMono("ActFlowerTouchTouchPart", UIWFlushPanel)
function ActFlowerTouchTouchPart:ActFlowerTouchTouchPart()
    self.data = ActFlowerTouchData.Instance

    self.data_cares = {
		-- {data = self.data.gameplay_data, func = self.FLushGameStatus},
        {data = self.data.flower_info, func = self.FLushPanel},
        -- {data = self.data.gameplay_data, func = self.FLushPanel},
        {data = self.data.gameplay_data, func = self.FLushPoint},
	}

    self.flower_list:SetData(self.data:GetFlowerList())

    self.game_timer:SetShowCallBack(function (t)
        return Language.HappyLink.ChallengeLimit..(t.min*60+t.s)..Language.HappyLink.Sceond
    end)

    self.cur_game_status = self.data:CheckIsGameStart()
end 

function ActFlowerTouchTouchPart:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.play_loop)
    self.play_loop = nil 

    self.data:GameComplete()
end

function ActFlowerTouchTouchPart:FLushPanel()
    self:FLushGameStatus()
end

function ActFlowerTouchTouchPart:FLushGameStatus()
    local is_playing = self.data:CheckIsGameStart()

    if self.cur_game_status ~= is_playing then 
        -- 由游戏进行到游戏结束
        if self.cur_game_status == true and is_playing == false then 
            self:GameComplete()
            PublicPopupCtrl.Instance:Center(Language.FlowerTouch.GameOver)
        end 

        self.cur_game_status = is_playing
    end 

    self.point_show:SetActive(is_playing)
    self.point_num.text = self.data.gameplay_data.cur_point
    self.btn_start:SetActive(not is_playing)
    self.idle_show:SetActive(not is_playing)
    self.game_state:SetActive(is_playing)

    if is_playing then 
        self:GameStart()
    end 
end

function ActFlowerTouchTouchPart:FLushPoint()
    self.point_num.text = self.data.gameplay_data.cur_point
end

function ActFlowerTouchTouchPart:OnClickStart()
    -- self:GameStart()
    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
        rand_activity_type = ACTIVITY_TYPE.RAND_YUN_ZE_BLOOMING_RECORD,
        opera_type = 1,
    })
end

function ActFlowerTouchTouchPart:GameStart()
    -- self.data.flower_info
    local total_time = tonumber(self.data.flower_info.challenge_end_timestamp) -- TimeHelper.GetServerTime() + Config.yunzefanhualu_auto.other[1].time 
    self.game_timer:StampTime(total_time,TimeType.Type_Special_5)
    self.game_loop = self.data:GetPlayList()
    self.time_pass = 0
    self.play_loop = TimeHelper:AddCountDownCT(BindTool.Bind(self.TouchPlayTimer,self),
    BindTool.Bind(self.CountDone,self),total_time, 1, true)
    self.data:GameStart()
    LogDG("?本次繁花轴!",self.game_loop)
end

function ActFlowerTouchTouchPart:TouchPlayTimer()
    -- LogDG("?operarte ! ",self.time_pass)
    local cur_step = self.game_loop[self.time_pass]
    if not ViewMgr:IsOpen(ActFlowerTouchView) then return end 

    if cur_step ~= nil then 
        for k,v in pairs(cur_step) do 
            LogDG("?当前的花点信息!",v.index,v)
            self.flower_list:GetItem(v.index):SetData(v)
            self.flower_list:GetItem(v.index):ShowItem()
        end 
    end 

    self.time_pass = self.time_pass + 1 
end

function ActFlowerTouchTouchPart:CountDone()
    -- self.data:GameComplete()
    -- self.game_timer:CloseCountDownTime()
end

-- 游戏完成
function ActFlowerTouchTouchPart:GameComplete()
    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
        rand_activity_type = ACTIVITY_TYPE.RAND_YUN_ZE_BLOOMING_RECORD,
        opera_type = 2,
        param_1 =  self.data.gameplay_data.cur_point,
    })
    self.data:GameComplete()
    self.game_timer:CloseCountDownTime()
end

FlowerTouchFlowerCell = FlowerTouchFlowerCell or DeclareMono("FlowerTouchFlowerCell", UIWidgetBaseItem)
function FlowerTouchFlowerCell:FlowerTouchFlowerCell()
    self.flower_data = ActFlowerTouchData.Instance

    self.point = 0
    self.point_num = 0
    self.anim:ListenEvent("phase1",
    function (eve_param,anim_st_info)
        if eve_param == "enter" then           
            -- LogError("!phase!? 1",self.data.index)
            self.point = 3 -- Config.yunzefanhualu_auto.other[1].score1
            self.point_num = Config.yunzefanhualu_auto.other[1].score1
            self.anim.speed = self.data.ph1_speed
        end 
    end) 

    self.anim:ListenEvent("phase2",
    function (eve_param,anim_st_info)
        if eve_param == "enter" then             
            -- LogError("!phase!? 2",self.data.index)
            self.point = 2--Config.yunzefanhualu_auto.other[1].score2
            self.point_num = Config.yunzefanhualu_auto.other[1].score2
            self.anim.speed =  self.data.ph2_speed
        end 
    end) 

    self.anim:ListenEvent("phase3",
    function (eve_param,anim_st_info)
        if eve_param == "enter" then           
            -- LogError("!phase!? 3",self.data.index)  
            self.point = 1--Config.yunzefanhualu_auto.other[1].score3
            self.point_num = Config.yunzefanhualu_auto.other[1].score3
            self.anim.speed =  self.data.ph3_speed
        end 
    end) 

    self.anim:ListenEvent("phase4",
    function (eve_param,anim_st_info)
        if eve_param == "enter" then      
            -- LogError("!phase!? 4",self.data.index)
            self.anim.speed = self.data.ph4_speed
        end 
        if eve_param == "exit" then  
            self.point = 0           
            for i = 1,3 do 
                self.points[i]:SetActive(false)
            end
        end 
    end) 

    self.anim:ListenEvent("phase5",
    function (eve_param,anim_st_info)
        if eve_param == "enter" then      
            -- LogError("!phase!? 5",self.data.index)
            self.anim.speed = self.data.ph4_speed
        end 

        if eve_param == "exit" then  
            self.point = 0           
            for i = 1,3 do 
                self.points[i]:SetActive(false)
            end
        end 
    end) 
end
function FlowerTouchFlowerCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    -- 标记位置
    if DEBUG_MODE == true then 
        -- UH.SetText(self.test_text, data.index)
    end 
    self.anim:SetInteger(APH("success"),0)
end

function FlowerTouchFlowerCell:ShowItem()
    self.anim:SetTrigger(APH("on"))
    self.wait_click = false
end 

function FlowerTouchFlowerCell:ShowPoint()
    -- LogError("?ShowPoint!",self.data.index)
    self.point_anim:SetTrigger(APH("on"))
    self.points[self.point]:SetActive(true)
    self.anim:SetInteger(APH("success"),1)

    self.wait_click = true
end 

function FlowerTouchFlowerCell:OnClickItem()
    -- LogError("click!!!",self.data.index)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.GetDiamond)
    if not self.wait_click then 
        self.flower_data:GamePlusClick(self.point_num)
    else 
        return 
    end 

    self:ShowPoint()
end

FlowerTouchRewardCell = FlowerTouchRewardCell or DeclareMono("FlowerTouchRewardCell", UIWidgetBaseItem)
function FlowerTouchRewardCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.ponit,self.data.integral..Language.FlowerTouch.Point)
    self.got:SetActive(self.data.is_got)

    local cur_num = ActFlowerTouchData.Instance.flower_info.cur_intergral
    local flag = cur_num >= self.data.integral and not self.data.is_got

    self.data.flag = flag
    self.effect_show:SetActive(flag)

    self.show_red = self.show_red or UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
    self.show_red:SetNum(flag and 1 or 0)
end 

FlowerTouchItemShow = FlowerTouchItemShow or DeclareMono("FlowerTouchItemShow", UIWFlushPanel)
function FlowerTouchItemShow:SetData(data)
    UH.LocalPosG(self.ObjList, data.pos or Vector3.zero)
    self.ShowList:SetData(data.item_list)
    if data.pivot then
        self.RectTrans.pivot = Vector2.New(data.pivot,0.5)
    end
end 

FlowerTouchItemShowCell = FlowerTouchItemShowCell or DeclareMono("FlowerTouchItemShowCell", UIWidgetBaseItem)
function FlowerTouchItemShowCell:FlowerTouchItemShowCell() end 
function FlowerTouchItemShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.call:SetData(data)
end

function FlowerTouchItemShowCell:OnClickItem()
    ItemInfoData.Instance:SetClickDataPos(UH.GetRootPosition(self.openpos))
    local click_func = self.call:getClickDel()
    if click_func then
		click_func(self)
	end
end


-- 编辑器
ActFlowerTouchEditPanel = ActFlowerTouchEditPanel or DeclareMono("ActFlowerTouchEditPanel", UIWFlushPanel)
function ActFlowerTouchEditPanel:ActFlowerTouchEditPanel()
    self.data = ActFlowerTouchData.Instance
    self.frame_save_list = {}
    self.operate_data = nil
    -- self.cur_flower_list = {}

    self:FlushList()
    self:FlushFrameList()
end 

function ActFlowerTouchEditPanel:FlushList() 
    -- self.cur_flower_list = {}
    local flower_list = {}
    for i = 1,60 do 
        local vo = {}
        vo.index = i
        vo.ph1_speed = 1
        vo.ph2_speed = 1
        vo.ph3_speed = 1
        vo.ph4_speed = 1
        vo.is_save = false

        table.insert(flower_list,vo)
    end 

    self.flower_list:SetData(flower_list)
end

function ActFlowerTouchEditPanel:FlushFrameList()
    self.frame_list:SetData(self.frame_save_list)
end

function ActFlowerTouchEditPanel:OnClickReset()
    self:FlushList()
end

function ActFlowerTouchEditPanel:OnClickOutput()
    PublicPopupCtrl.Instance:Center("缓存已完成，记录已被复制到粘贴板")

    local save_str = ""
    for k,v in pairs(self.frame_save_list) do 
        save_str = save_str ..v.second..":"
        for i,j in pairs(v.data_list) do 
            save_str = save_str .. j.index ..","..j.ph1_speed..","..j.ph2_speed..","..j.ph3_speed..","..j.ph4_speed
            if i < #v.data_list then 
                save_str = save_str ..":"
            end  
        end 
        save_str = save_str.."|"
    end 
    self.data:CacheEditPlay(save_str)

    UnityEngine.GUIUtility.systemCopyBuffer = save_str
end

function ActFlowerTouchEditPanel:OnClickSave()
    local second = self.second_input.text

    local list = {}
    for k,v in pairs(self.flower_list.item_list) do 
        local data = v:GetData()
        if data.is_save then 
            table.insert(list,data)
        end 
    end 

    local vo = {
        data_list = list,
        second = tonumber(second)
    }
    self.frame_save_list[tonumber(second)] = vo

    self:FlushFrameList()

    self:OnClickReset()
end

function ActFlowerTouchEditPanel:OnClickFrameCell(data)
    local list = {}
    for k,v in pairs(data.data_list) do 
        list[v.index] = v
    end 

    for i = 1,60 do 
        if list[i] == nil then 
            local vo = {}
            vo.index = i
            vo.ph1_speed = 1
            vo.ph2_speed = 1
            vo.ph3_speed = 1
            vo.ph4_speed = 1
            vo.is_save = false

            list[i] = vo
        end 
    end 

    self.flower_list:SetData(list)
    self.second_input.text = data.second
end

function ActFlowerTouchEditPanel:OnClickPopOpen(data)

    UH.SetText(self.pop_title, "生成块 "..data.index.." 编辑")
    self.cell_ph1.text = data.ph1_speed
    self.cell_ph2.text = data.ph2_speed
    self.cell_ph3.text = data.ph3_speed
    self.cell_ph4.text = data.ph4_speed

    self.operate_data = data

    self.pop:SetActive(true)
end

function ActFlowerTouchEditPanel:OnClickPopClose()
    self.pop:SetActive(false)
end

function ActFlowerTouchEditPanel:OnClickPopSave()
    local item = self.flower_list:GetItem(self.operate_data.index)

    self.operate_data.ph1_speed = tonumber(self.cell_ph1.text)
    self.operate_data.ph2_speed = tonumber(self.cell_ph2.text)
    self.operate_data.ph3_speed = tonumber(self.cell_ph3.text)
    self.operate_data.ph4_speed = tonumber(self.cell_ph4.text)
    self.operate_data.is_save = true

    item:SetData(self.operate_data)

    self:OnClickPopClose()
end

-- 编辑块
ActFlowerTouchEditCell = ActFlowerTouchEditCell or DeclareMono("ActFlowerTouchEditCell", UIWidgetBaseItem)
function ActFlowerTouchEditCell:ActFlowerTouchEditCell()
    
end

function ActFlowerTouchEditCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    -- UH.SetText(self.test_text, data.index)
    self.plus:SetActive(self.data.is_save)
end 


ActFlowerTouchFrameEditCell = ActFlowerTouchFrameEditCell or DeclareMono("ActFlowerTouchFrameEditCell", UIWidgetBaseItem)
function ActFlowerTouchFrameEditCell:ActFlowerTouchFrameEditCell()
    
end

function ActFlowerTouchFrameEditCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.name, "第 "..data.second.."秒|"..#data.data_list.."个")
end