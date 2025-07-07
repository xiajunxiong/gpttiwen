TaoQuanNaJiBattleView = TaoQuanNaJiBattleView or DeclareView("TaoQuanNaJiBattleView","serveractivity/taoquannaji/taoquannaji_battle_view")
VIEW_DECLARE_LEVEL(TaoQuanNaJiBattleView, ViewLevel.L0)
VIEW_DECLARE_MASK(TaoQuanNaJiBattleView, ViewMask.None)
function TaoQuanNaJiBattleView:TaoQuanNaJiBattleView()
    
end

function TaoQuanNaJiBattleView:OpenCallback()
    -- self.panel:CallInit()
end


--===========================TaoQuanNaJiBattlePanel===========================
TaoQuanNaJiBattlePanel = TaoQuanNaJiBattlePanel or DeclareMono("TaoQuanNaJiBattlePanel", UIWFlushPanel)
function TaoQuanNaJiBattlePanel:TaoQuanNaJiBattlePanel()
    self.show_data = TaoQuanNaJiBattleShow.Instance
    self.data = TaoQuanNaJiData.Instance
    self.data_cares = {
        {data = self.data.ring_info,func = self.MapFlush,keys = {"flush"},init_call = false},            
        {data = self.data.ring_info,func = self.FlushView,keys = {"flush"},init_call = false},            
        {data = self.data.score_info,func = self.FlushView,keys = {"flush"},init_call = false},            
        {data = self.data.play_info,func = self.FlushResult,keys = {"flush"},init_call = false},    
        {data = self.data.map_init,func = self.FlushPoint,keys = {"flush"},init_call = false},            
        {data = self.data.map_point,func = self.PointShow,keys = {"flush"},init_call = false},            
        {data = self.data.chapter_info,func = self.FlushView,keys = {"flush"},init_call = false},            
    }

    -- 入场的判断必须是还没到两次
    -- self.reward_mark = self.data.ring_info.today_play_times < Config.lasso_game_auto.other[1].reward_time

    -- -- self.point_show:SetActive(self.reward_mark)
    -- if not self.reward_mark then 
    --     PublicPopupCtrl.Instance:Center(Language.LuckyRing.NoReward)
    -- end 

    -- 开界面预先判断
    -- self.data.finish_info.need_reward = self.data.ring_info.today_play_times < Config.lasso_game_auto.other[1].reward_time and 1 or 0 
    -- self.data.finish_info.can_title = self.data.ring_info.flag_title == 0
    self:FlushView()
    -- self.editor_total:SetActive(DEBUG_MODE == true)
    self.editor_total:SetActive(false)
end

function TaoQuanNaJiBattlePanel:CallInit()
    self:OnClickEditor()
    self:MapFlush()
    self:FlushView()
end

function TaoQuanNaJiBattlePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.delay_timer ~= nil then 
        TimeHelper:CancelTimer(self.delay_timer)
        self.delay_timer = nil
    end 

    if self.end_timer ~= nil then 
        TimeHelper:CancelTimer(self.end_timer)
        self.end_timer = nil
    end 
end 

function TaoQuanNaJiBattlePanel:MapFlush()
    self.show_data:SpawnFlushMap()

    if not self.data:MapAvaile() then 
       self.ready_toflush = true
       return 
    end 

    if self.ready_toflush then 
        for k,v in pairs(self.show_data:GetMapItemMods()) do 
            self.point_list.item_list[k]:SetData(v) 
        end 
        self.ready_toflush = false
    end 
end

function TaoQuanNaJiBattlePanel:FlushView()
    UH.SetText(self.num, Language.LuckyRing.LastNum..ColorStr(self.data.chapter_info.limit,COLORSTR.Blue12))
    UH.SetText(self.base_point,self.data.score_info.score or 0)

    local chapter = self.data.chapter_info.chapter

    self.chapter_num2:SetObjActive(chapter > 9)
    UH.SpriteName(self.chapter_num2,chapter > 9 and math.floor(chapter%10) or 0)
    UH.SpriteName(self.chapter_num1,chapter > 9 and math.floor(chapter/10) or chapter)

    UH.SetText(self.cur_target, Language.LuckyRing.TargetNum..ColorStr(self.data.chapter_info.need,COLORSTR.Green7))

    local tips_show = self.data:FlushHighPointShow()
    -- self.high_plus_show:SetActive(tips_show.flag and self.reward_mark)
    -- UH.SetText(self.high_plus_text, self.reward_mark and tips_show.word or Language.LuckyRing.TodayEnd)
    self.reward_id = tonumber(tips_show.cfg.show_reward)
    self.reward_icon.gameObject:SetActive(self.reward_mark and tonumber(tips_show.cfg.show_reward) > 0)
    if self.reward_mark and tonumber(tips_show.cfg.show_reward) > 0 then 
        UH.SetIcon(self.reward_icon, Item.GetIconId(tonumber(tips_show.cfg.show_reward)))
    end 
end

function TaoQuanNaJiBattlePanel:FlushResult() 
    if self.data.play_info.target.empty == true then 
        return 
    end 

    self.sp_tips:SetActive(self.data.play_info.target.sp_type > 0)
    UH.SetText(self.sp_txt_tips,self.data.play_info.target.word)
    self.delay_timer = TimeHelper:AddDelayTimer(function ()
        self.sp_tips:SetActive(false)
    end,3)

    self.UIEffectTool:Play(self.data.play_info.target.sp_type > 0 and 7165056 or 7165055 ,self.effect)
end 

function TaoQuanNaJiBattlePanel:FlushPoint()
    self.point_list:SetData(self.show_data:GetMapItemMods())

    self:CallInit()
end

function TaoQuanNaJiBattlePanel:PointShow()
    for k,v in pairs(self.data.map_point.list) do 
        if self.point_list.item_list[v] ~= nil then 
            self.point_list.item_list[v]:Show()
        end 
    end 
end

function TaoQuanNaJiBattlePanel:OnClickExit()
    GameUtil.StopGame()

    -- ViewMgr:CloseView(TaoQuanNaJiBattleView)
    -- SceneCtrl.Instance:GotoScene(GameScene)
end

function TaoQuanNaJiBattlePanel:OnClickReward()
    local item_info = Item.Init(self.reward_id)
    local detail = {
        item_list = {item_info}
    }
    ItemInfoCtrl.Instance:ItemInfoView(detail)
end

function TaoQuanNaJiBattlePanel:OnClickEditor()
    self.show_data:EditorFlush()
    self.show_data:FlushInputEditor(
        {
            accuracy = tonumber(self.editor2.text),
            offset = tonumber(self.editor1.text),
            min_stren = tonumber(self.editor3.text),
            angle_off = tonumber(self.editor4.text),
        }
    )
end

function TaoQuanNaJiBattlePanel:OnClickSimulate()
    self.show_data:ShowSimulate()
end

function TaoQuanNaJiBattlePanel:OnClickProtocol0()
    -- TaoQuanNaJiCtrl.Instance:SendLuckyRingReq({})    
end

function TaoQuanNaJiBattlePanel:OnClickProtocol1()
    -- TaoQuanNaJiCtrl.Instance:SendLuckyRingReq({req_type = 1})
    TaoQuanNaJiData.Instance:ResetInfo()
end

function TaoQuanNaJiBattlePanel:OnClickProtocol2()
    -- TaoQuanNaJiCtrl.Instance:SendLuckyRingReq({req_type = 2,param1 = 1,param2 = 1})    
    TaoQuanNaJiData.Instance:TaoZhongItem(1, 1)
end

function TaoQuanNaJiBattlePanel:OnClickProtocoltest()
    -- LogError("?check!1139!",self.data.ring_info.play_map)
    -- LogError("?check!1140!",self.data.score_info.score)
end

--===========================TaoQuanNaJiJoyStick===========================
-- 纳吉改·摇杆
TaoQuanNaJiJoyStick = TaoQuanNaJiJoyStick or  DeclareMono("TaoQuanNaJiJoyStick", UIWFlushPanel)
function TaoQuanNaJiJoyStick:TaoQuanNaJiJoyStick()
    self.joystick_angle = 0
    self.joystick_last_target_pos = Vector2.New(0, 0)	

    self.stick_off = Vector3.zero

    self.main_camera = UnityGameObj.FindGameObjectWithTag("MainCamera")

    self.data = TaoQuanNaJiData.Instance
    self.show_data = TaoQuanNaJiBattleShow.Instance

    self.stren_limit = 220
end 


function TaoQuanNaJiJoyStick:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.stick_update ~= nil then
        TimeHelper:CancelTimer(self.stick_update)
        self.stick_update = nil
    end 
end

-- 输入刷新 
function TaoQuanNaJiJoyStick:OnJoyStickDrag()
    if not self.data:MapAvaile() then
        return
    end
    -- if self.data ~= nil and self.data.ring_info.can_play_times == 0 then 
    --     return 
    -- end 
    if self.show_data:IsFlying() then 
        -- PublicPopupCtrl.Instance:Center(Language.LuckyRing.WaitFlying)
        return 
    end 

    if self.data:CheckLimit() or self.data:CheckPoint() then 
        return 
    end 

    self.stick_off = self:getStickOffset()
    
    self.base_show:SetActive(false)
    self.show:SetActive(true)

    self.click = true
end

-- 获取输入
function TaoQuanNaJiJoyStick:getStickOffset()

	if self.stick_init_pos == nil then         --初始化取到的位置会有问题，改在第一次收到事件的时候获取
		self.stick_init_pos = self.joystick:GetLocalPosition()
	end

    -- 输入方案：unity的屏幕鼠标输入/当前屏幕宽 * 当前界面尺寸


	self.input_t = Vector2(UnityEngine.Input.mousePosition.x / UnityEngine.Screen.width * self.main.rect.width,
        UnityEngine.Input.mousePosition.y / UnityEngine.Screen.height * self.main.rect.height)

    
    UH.LocalPosG(self.test, self.stick_init_pos)

	local touch_off = self.input_t - self.stick_init_pos

	local max_off_sqr = self.stren_limit * self.stren_limit
    -- 超距离限制
	if (touch_off:SqrMagnitude()) > max_off_sqr then
		touch_off = touch_off:Normalize() * self.stren_limit
	end
    -- 负距离限制
    if touch_off.y < 0 then 
        touch_off = Vector2(touch_off.x,0)
    end 
    -- 超角度限制
    local angle = math.deg(-math.atan2(touch_off.x, touch_off.y))
    if angle < -60  then 
        local limit_x = touch_off:Magnitude() * math.sin(math.rad(60))
        local limit_y = touch_off:Magnitude() * math.cos(math.rad(60))

        touch_off = Vector2(limit_x,limit_y)
    elseif angle > 60 then 
        local limit_x = touch_off:Magnitude() * math.sin(math.rad(-60))
        local limit_y = touch_off:Magnitude() * math.cos(math.rad(-60))

        touch_off = Vector2(limit_x,limit_y)
    end 

    return touch_off
end

-- 纠正 
function TaoQuanNaJiJoyStick:ResetPos()
    self.base_show:SetActive(true)
    self.show:SetActive(false)
end

function TaoQuanNaJiJoyStick:OnJoyStickPressUp()
    if not self.data:MapAvaile() then
        return
    end
    -- if self.data ~= nil then and self.data.ring_info.can_play_times == 0 then 
    --     return 
    -- end 

    if self.show_data:IsFlying() then 
        PublicPopupCtrl.Instance:Center(Language.LuckyRing.WaitFlying)
        return 
    end 

    if self.data:CheckLimit() or self.data:CheckPoint() then 
        return 
    end 

    self.show_data:PlayRingFly()

    local timer = 4 
    local mark_x = self.stick_off.x/timer
    local mark_y = self.stick_off.y/timer
    self.ShowTimer = TimeHelper:AddRunFrameTimer(function()
        self.stick_off.x = self.stick_off.x - mark_x
        self.stick_off.y = self.stick_off.y - mark_y
        timer = timer - 1
        if timer == 0 then
            self.stick_off = Vector3.zero
        end 
    end, 1,timer)

    self.click = false
    self:ResetPos()
end

function TaoQuanNaJiJoyStick:OnJoyStickPressDown()
    if not self.data:MapAvaile() then
        return
    end
    -- if self.data ~= nil then self.data.ring_info.can_play_times == 0 then 
    --     return 
    -- end 
    
    if self.show_data:IsFlying() then 
        PublicPopupCtrl.Instance:Center(Language.LuckyRing.WaitFlying)
        return 
    end 

    if self.data:CheckLimit() or self.data:CheckPoint() then 
        return 
    end 

    if self.click then
        self:OnJoyStickPressUp()
        return 
    end 

    if self.stick_update ~= nil then return end 

    self.stick_update = TimeHelper:AddRunTimer(function()
        local distance = self.stick_off.x *self.stick_off.x + self.stick_off.y * self.stick_off.y
        -- if distance < 1000 then return end 

        
        local dir_jp_vec = Vector3.New(self.stick_off.x,self.stick_off.y,0)
        local angle = math.deg(-math.atan2(dir_jp_vec.x, dir_jp_vec.y))

        local avable = self.stick_off.y >0 
        local leg = avable and math.sqrt(math.pow(self.stick_off.x,2) + math.pow(self.stick_off.y,2)) or 0.01
        local ver = math.deg(math.asin(leg/self.stick_off.y))
        local real_rate = leg/self.stren_limit
        local stren = (1 - self.show_data.sim_data.min_stren)*real_rate + self.show_data.sim_data.min_stren

        if leg > 0 then 
            self.stren_line.fillAmount = real_rate
        else 
            self.stren_line.fillAmount = 0
        end 


        self.show:SetLocalPosition( self.stick_off)
        self.look_mark.transform:LookAt(self.show.transform)
        UH.LocalPosG(self.destance, Vector3.New(0,-leg,0))
        UH.LocalEulerAngles(self.joy_p.transform,Vector3.New(0,0,angle))

        if not ViewMgr:IsOpen(TaoQuanNaJiBattleView) then 
            if self.stick_update ~= nil then
                TimeHelper:CancelTimer(self.stick_update)
                self.stick_update = nil
            end 
            
            return 
        end 

        self.show_data:Input({
            angle = angle,
            stren = stren,
        })

    end,0.03)
end

--===========================TaoQuanNaJiPointShowCell===========================
TaoQuanNaJiPointShowCell = TaoQuanNaJiPointShowCell or DeclareMono("TaoQuanNaJiPointShowCell", UIWidgetBaseItem)
function TaoQuanNaJiPointShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local str_point = TaoQuanNaJiData.Instance:GetItemPointByItemId(data.data.item_id)
    UH.SetText(self.str, str_point > 0 and Language.LuckyRing.PointShow..TaoQuanNaJiData.Instance:GetItemPointByItemId(data.data.item_id) or "")

    self.Follow.Target = TaoQuanNaJiBattleShow.Instance:GetMapItemObj(data:GetPos()).transform
    self:InitShow()
    UH.SetText(self.tester,"")--data.data.x.." "..data.data.y.." "..data:GetPos())
end

function TaoQuanNaJiPointShowCell:Show()
    self.str.gameObject:SetActive(true)
end

function TaoQuanNaJiPointShowCell:InitShow()
    self.str.gameObject:SetActive(false)
end