TombRaiderBattleView = TombRaiderBattleView or DeclareView("TombRaiderBattleView", "tomb_raider/tomb_raider_battle")
-- VIEW_DECLARE_LEVEL(TombRaiderBattleView, ViewLevel.Lu)
VIEW_DECLARE_MASK(TombRaiderBattleView, ViewMask.None)
function TombRaiderBattleView:TombRaiderBattleView()

end

function TombRaiderBattleView:OpenCallback()
    BattleLuaFunction.UIHide()
end 

function TombRaiderBattleView:CloseCallback()
	BattleLuaFunction.UIShow()
end

function TombRaiderBattleView:showtest()
    ViewMgr:CloseView(TombRaiderBattleView)
end

function TombRaiderBattleView:OnClickSynopsis()
    ViewMgr:OpenView(TombRaiderSynopsisView)
end

--===========================TombRaiderBattlePanel===========================
TombRaiderBattlePanel = TombRaiderBattlePanel or DeclareMono("TombRaiderBattlePanel", UIWFlushPanel)
function TombRaiderBattlePanel:TombRaiderBattlePanel()
    self.data = TombRaiderData.Instance
end

function TombRaiderBattlePanel:onFlush() end

-- 测试按钮 退出副本
function TombRaiderBattlePanel:OnClickBack()
    local battle_base_data = BattleData.Instance:BaseData()
	if BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE == battle_base_data.battle_mode then
		local info = {
			content = Language.Main.Battle.GiveUpTip,
			confirm = {
					func = function ()
					PublicPopupCtrl.Instance:CloseDialogTips()
					BattleCtrl.GiveUp()
					end
			}
		}
		PublicPopupCtrl.Instance:DialogTips(info)
		return
	end
	local mode = SceneData.Instance:GetMode()
	if mode == SceneModeType.FightGather then
		MainOtherCtrl.Instance:CloseGather()
	end
	if MainPlayer then
		MainPlayer:StopMove()
	end
	if SceneModeType.FightCrystal == mode then
		local fight_crystal_info = FightData.Instance:GetFightCrystalInfo()
		local cur_seq_info = FightData.Instance:GetFightCrystalConfigBySeq(fight_crystal_info.level_seq)
		local is_complete = false
		if cur_seq_info then
			local last_layer = fight_crystal_info.layer == cur_seq_info.layer
			is_complete = last_layer and fight_crystal_info.is_finish
		end
		PublicPopupCtrl.Instance:AlertTip(Language.Chat.BackTips .. (is_complete and "" or Language.Fight.CrystalDungen.level_not_complete),function()
			SceneCtrl.Instance:RequestLeaveFb()
		end)
	else
		PublicPopupCtrl.Instance:AlertTip(Language.Chat.BackTips .. FightCtrl.GetWarnTips(mode),function()
			SceneCtrl.Instance:RequestLeaveFb()
		end)
	end
end


--===========================TombSkillBar=====================================
TombSkillBar = TombSkillBar or DeclareMono("TombSkillBar", UIWFlushPanel)
function TombSkillBar:TombSkillBar()
    self.data = TombRaiderData.Instance
    self.bs_data = TombRaiderBattleShow.Instance
    
    self.data_cares = {
        -- {data = self.data.tomb_team,func = self.SkillDataFlush},
        {data = self.data.skill_info,func = self.FlushSkillInfo,init_call = false},
        {data = self.data.transmits_info,func = self.FlushDeliverPop,init_call = false}
    }
    self.skill_is_show = false
    self:SkillDataFlush()

    Runner.Instance:AddRunObj(self, 16)

    self.is_long_click = false

    self.buff_list = {}
end

function TombSkillBar:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.delive_show_delay ~= nil then
        TimeHelper:CancelTimer(self.delive_show_delay)
        self.delive_show_delay = nil
    end

    Runner.Instance:RemoveRunObj(self)
    TimeHelper:CancelTimer(self.timer_ct)
    for i = 1,3 do self.mainskills[i]:ClearData() end 

    TimeHelper:CancelTimer(self.time_func)
end 

function TombSkillBar:Update()
    for i = 1,3 do
        local flag,desc = self.data:GetOperateIneracter(0,self.mainskills[i]:GetData().cfg)
        self.mainskills[i].Inter.Interactable = flag
    end 
    local flag,desc = self.data:GetOperateIneracter(1)
    -- self.pick_inter.Interactable = flag
    self.pick_obj:SetActive(flag)

    --buff需要实时刷新
    if self.buff_flush then 
        self.skill_buff_list:SetData(self.buff_list)
        self.buff_flush = false
    end 

    if self.chat_part then 
        self.chat_part:SetActive(not ViewMgr:IsOpen(ChatView))
    end 

    -- LogError("!?@",self.item_skill.data)
    if self.item_skill.data.cfg.sn == -1 and self.skill_desc_obj.activeSelf then 
        self.skill_desc_obj:SetActive(false)
        TimeHelper:CancelTimer(self.time_func)
    end 
end 

function TombSkillBar:SkillDataFlush()
    local skill_list = self.bs_data:GetTeamSkillGroup()
    for i = 1,3 do 
        self.mainskills[i]:SetData(skill_list[i])
    end 
    self.item_skill:SetData(self.bs_data:GetItemSkillData())
end

function TombSkillBar:FlushSkillInfo()
    for i = 1,3 do 
        self.mainskills[i]:SetSkillInfo( self.data.skill_info.common_list[i] )
    end 
    self.item_skill:SetData(self.bs_data:GetItemSkillData())
    self.item_skill:SetSkillInfo( self.data.skill_info.item_skill )
    self:OnShowSkill(self.data:CheckSkillInfo())
end

function TombSkillBar:OnClickSkill(data)
    if not self.is_long_click then
        local inter,desc = self.data:GetOperateIneracter(0,data.sn)
        if desc ~= nil then 
            PublicPopupCtrl.Instance:Center(desc)  
            return 
        end 
        TombRaiderBattleShow.Instance:MainSkillShow(data)
    end 
end

function TombSkillBar:OnLongClick(data)
    if data.cfg.sn == -1 then return end 
    self.is_long_click = false
    TimeHelper:CancelTimer(self.time_func)
    local time = 0
    self.time_func =
        TimeHelper:AddRunTimer(
        function()
            time = time + 0.2
            if time > 1 then
                self.is_long_click = true

                self.skill_desc_obj:SetActive(true)
                UH.SetText(self.skill_name,data.cfg.skill_name)
                UH.SetText(self.skill_desc,data.cfg.skill_dsc_show)
            end
        end,
        0.2
    )
end 

function TombSkillBar:OnLongClickUp(data)
    if data.cfg.sn == -1 then return end 
    TimeHelper:CancelTimer(self.time_func)
    self.skill_desc_obj:SetActive(false)
end 

function TombSkillBar:OnLongBuffClick(data)
    if data.sn == -1 then return end 
    self.is_long_click = false
    TimeHelper:CancelTimer(self.time_func)
    local time = 0
    self.time_func =
        TimeHelper:AddRunTimer(
        function()
            time = time + 0.2
            if time > 0.4 then
                self.is_long_click = true

                self.skill_buff_obj:SetActive(true)
                self.skill_buff_showing = data
                UH.SetText(self.skill_buff_show,data.desc)
            end
        end,
        0.2)
end

function TombSkillBar:OnLongBuffClickUp(data)
    if data.sn == -1 then return end 
    TimeHelper:CancelTimer(self.time_func)
    self.skill_buff_showing = nil
    self.skill_buff_obj:SetActive(false)
end

function TombSkillBar:OnShowSkill(skill_info)
    if self.skill_is_show and skill_info.is_nil == nil then return end 
    if skill_info.is_nil then -- 技能被中断了
        local buff_info = self.data:CacheSkillBuffInfo({sn = self.showing_skill})
        self:SkillFinishPop(self.showing_skill)
        self:AddBuff(buff_info)
        -- TimeHelper:CancelTimer(self.timer_ct)
        self.skill_show:SetActive(false)
        self.skill_is_show = false
        MainPlayer:PlayAnim(ChAnimType.Idle)
        self.showing_skill = 0
        return 
    end

    local total_time = skill_info.tamp -- - TimeHelper.GetServerTime()
    local r_timer = skill_info.tamp - TimeHelper.GetServerTime()
    UH.SetText(self.skill_show_status,skill_info.moving_text)
    self.skill_show:SetActive(true)
    self.skill_is_show = true
    local prog_step = 1 / (math.floor(r_timer / 0.1))
    local prog = 0

    -- 播放采集动画
    if skill_info.sn == TombRaiderEnum.UnLock then
        MainPlayer:PlayAnim(ChAnimType.Gather)
    end 

    self.showing_skill = skill_info.sn

    self.timer_ct = TimeHelper:AddCountDownCT(function ()
        prog = prog + prog_step
        UH.FillAmount(self.skill_show_prog,  prog)
    end, 
    function ()
        -- LogError("!?@siu!")
        UH.FillAmount(self.skill_show_prog, 1)
        self.skill_show:SetActive(false)
        self.skill_is_show = false
        MainPlayer:PlayAnim(ChAnimType.Idle)
        self:SkillFinishPop(self.showing_skill)
        self.showing_skill = 0
        local buff_info = self.data:CacheSkillBuffInfo(skill_info)
        self:AddBuff(buff_info)
    end, 
    total_time+0.1, 0.1)
end

function TombSkillBar:SkillFinishPop(sn)
    if sn == TombRaiderEnum.UnLock then
        -- PublicPopupCtrl.Instance:Center(Language.TombRaider.BoxCatchSuccess..Config.treasure_contend_auto.other[1].box_score)  
    elseif sn == TombRaiderEnum.Catch then 
        -- LogError("!@?",self.bs_data:CheckWithTargetForPop(self.data:GetSkillParamt(self.data:GetSkillCfg(sn))),self.data:GetSkillParamt(self.data:GetSkillCfg(sn)))
        -- if self.bs_data:CheckWithTargetForPop(self.data:GetSkillParamt(self.data:GetSkillCfg(sn))) then 
        --     PublicPopupCtrl.Instance:Center(Language.TombRaider.enemyCatchSuccess..Config.treasure_contend_auto.other[1].capture_score)  
        -- end 
    elseif sn == TombRaiderEnum.Rescue then 
        -- PublicPopupCtrl.Instance:Center(Language.TombRaider.SaveParnterSuccess)  
    end 
end

function TombSkillBar:OnClickGuardMark()
    self.map_part:SetMarkData(7)
    self.map_part:OnClickMap(0)
end

function TombSkillBar:OnClickLockMark()
    self.map_part:SetMarkData(6)
    self.map_part:OnClickMap(0)
end

function TombSkillBar:OnClickFlagMark()
    self.map_part:SetMarkData(1)
    self.map_part:OnClickMap(0)
end

function TombSkillBar:OnClickPickUp()
    local catch_info = self.data:CatchNearestObj(1,Config.treasure_contend_auto.other[1].pick_distance)
    if catch_info.obj_id == 0 then 
        PublicPopupCtrl.Instance:Center(Language.TombRaider.NeedItem)  
        return 
    end 
    TombRaiderCtrl.Instance:SendSkillOperate({
        type = TombRaiderConfig.OperType.CatchItem,
        param1 = catch_info.obj_id,
    })
end

function TombSkillBar:FlushDeliverPop() 
    if self.data.transmits_info.uid > 0 then 
        local obj = SceneCtrl.Instance:Objs():GetCharaByUid(self.data.transmits_info.player_name)

        self.deliverPop:SetActive(true)
        UH.SetText(self.deliverTitle,string.format( Language.TombRaider.TransmitsInfoTitle,self.data.transmits_info.uid))

        self.delive_show_delay = TimeHelper:AddDelayTimer(function() 
            self:OnClickDeliverCancel()
        end,self.data.transmits_info.end_tamp - TimeHelper.GetServerTime())
    end 
end 

function TombSkillBar:OnClickDeliverMove()
    TombRaiderCtrl.Instance:SendSkillOperate({
        type = TombRaiderConfig.OperType.AccpetTransmist,
        param1 = self.data.transmits_info.uid,
    })
    self.deliverPop:SetActive(false)
end  

function TombSkillBar:OnClickDeliverCancel()
    self.deliverPop:SetActive(false)
    self.data.transmits_info.uid = 0
end 

function TombSkillBar:RemoveBuff(data)
    if data.sn == nil or data.buff_type == nil then return end 

    if self.skill_buff_showing ~= nil and data.sn == self.skill_buff_showing.sn and data.buff_type == self.skill_buff_showing.buff_type then 
        self:OnLongBuffClickUp(self.skill_buff_showing)
    end 

    for k,v in pairs(self.buff_list) do 
        if data.buff_type == v.buff_type and data.sn == v.sn then 
            table.remove(self.buff_list,k)
        end 
    end 

    self.buff_flush = true
end

function TombSkillBar:AddBuff(data)
    if data.sn == nil or data.buff_type == nil then return end 
    
    local is_plus = false
    for k,v in pairs(self.buff_list) do 
        if data.buff_type == v.buff_type and data.sn == v.sn then 
            is_plus = true
        end 
    end

    if not is_plus then table.insert( self.buff_list, data ) end 

    self.buff_flush = true
    self.showing_skill = nil
end

TombMainSkillItem = TombMainSkillItem or DeclareMono("TombMainSkillItem", UIWidgetBaseItem)
function TombMainSkillItem:TombMainSkillItem(data)
    self.is_cd = false
end 
-- 注入基础信息
function TombMainSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    if data.cfg.sn ~= -1 then 
        self.SkillIcon.gameObject:SetActive(true)
        UH.SpriteName(self.SkillIcon, "skill_"..data.cfg.sn)
        if self.show_name then
            UH.SetText(self.SkillName,data.cfg.skill_name)
        end 
    else
        self.SkillIcon.gameObject:SetActive(false)
        if self.show_name then
            UH.SetText(self.SkillName,Language.Common.Wu)
        end 
    end 
end 

function TombMainSkillItem:ClearData()
    TimeHelper:CancelTimer(self.timer_ct)
end

function TombMainSkillItem:TryCD(temp_time)
    if self.is_cd or temp_time == 0 then 
        return 
    end 
    self.cdMask.gameObject:SetActive(true)
    local total_time = temp_time -- - TimeHelper.GetServerTime()
    local r_timer = temp_time - TimeHelper.GetServerTime()
    local prog = 1
    local prog_step = 1 / (math.floor(r_timer / 0.1)-1)
    local cur_num_time = r_timer
    self.is_cd = true
    self.timer_ct = TimeHelper:AddCountDownCT(function ()
        prog = prog - prog_step
        cur_num_time = cur_num_time - 0.1
        if cur_num_time > 0 then UH.SetText(self.cd_time,math.floor(cur_num_time)) end 
        UH.FillAmount(self.cdMask, prog)
    end, 
    function ()
        UH.FillAmount(self.cdMask, 0)
        self.cdMask.gameObject:SetActive(false)
        self.is_cd = false
    end, 
    total_time+0.1, 0.1)
end

function TombMainSkillItem:IsInCD()
    return self.is_cd
end

-- 刷新技能信息
function TombMainSkillItem:SetSkillInfo(info)
    self.skill_info = info
    self:TryCD(info.cool_down_tamp)
end


TombSkillBuffItem = TombSkillBuffItem or DeclareMono("TombSkillBuffItem", UIWidgetBaseItem)
function TombSkillBuffItem:TombSkillBuffItem()end 
function TombSkillBuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.SkillIcon, "skill_"..data.sn)
    if self.timer_ct == nil then 
        self:TryCD(data.temp_time)
    end 
end 

function TombSkillBuffItem:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    TimeHelper:CancelTimer(self.timer_ct)
end


function TombSkillBuffItem:TryCD(temp_time)
    TimeHelper:CancelTimer(self.timer_ct)

    self.cdMask.gameObject:SetActive(true)
    local total_time = temp_time -- - TimeHelper.GetServerTime()
    local r_timer = temp_time - TimeHelper.GetServerTime()
    local prog = 0
    local prog_step = 1 / (math.floor(r_timer / 0.1)-1)
    self.timer_ct = TimeHelper:AddCountDownCT(function ()
        if self.cdMask ~= nil and self.anim ~= nil then 
            prog = prog + prog_step

            UH.FillAmount(self.cdMask, prog)

            local cur = total_time - TimeHelper.GetServerTime()
            if cur <= 2 then self.anim:SetInteger(APH("End"),1) end 
        end 
    end, 
    function ()
        if self.cdMask ~= nil and self.anim ~= nil then 
            UH.FillAmount(self.cdMask, 1)
            self.cdMask.gameObject:SetActive(false)
            self:PlusBuff()
            self.anim:SetInteger(APH("End"),0)
            self.timer_ct = nil
        end 
    end, 
    total_time+0.1, 0.1)
end 

function TombSkillBuffItem:PlusBuff()
    if self.data.sn ~= TombRaiderEnum.BurstBoost then 
        self.skill_bar:RemoveBuff(self.data)
        return 
    end 

    local buff_info = TombRaiderData.Instance:CacheSkillBuffInfo({
        sn = self.data.sn,
        is_debuff = true,
    })
    self.skill_bar:AddBuff(buff_info)
    self.skill_bar:RemoveBuff(self.data)
end