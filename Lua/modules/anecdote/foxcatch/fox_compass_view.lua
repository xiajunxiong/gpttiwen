FoxCompassView = FoxCompassView or DeclareView("FoxCompassView","anecdote/fox_compass",0,ViewLayer.Normal - 1)
VIEW_DECLARE_LEVEL(FoxCompassView, ViewLevel.Lt)
VIEW_DECLARE_MASK(FoxCompassView, ViewMask.None)

FoxCompassPanel = FoxCompassPanel or DeclareMono("FoxCompassPanel", UIWFlushPanel)
function FoxCompassPanel:FoxCompassPanel()
    self.data = CatchFoxData.Instance
    self.data_cares = {
		{data = MainViewData.Instance.rb_toggle, func = self.OnMainToggeleChange,keys = {"isOn"}},
    }
    self.anim_events = {}
    self.is_play_anim = false
    self.last_index = nil
    self.invoke_timer = nil 
    self.skill_timer = nil

    self.auto_timer = nil
    self.timer_ct = nil
	AnecdoteData.Instance:CustomAutoHandle(AnecdoteType.FoxCatch,BindTool.Bind1(self.OnAutoFunc, self))
end

function FoxCompassPanel:OnAutoFunc()
    ViewMgr:CloseView(GuideFox)
    if ViewMgr:IsOpen(FoxCompassView) then 
        self:OnClickCactch()
    end 
    local fox = nil
    TimeHelper:AddDelayTimer(function()
        SceneCtrl.Instance:Objs():ForeachSNPC(
            function(npc_obj)
                if npc_obj.vo.id == Config.main_catch_fox_auto.level_monster_group[1].npc_id then 
                    fox = npc_obj
                    return true
                end 
                return false
            end 
        )
        if fox then 
            SceneLogic.Instance:AutoToNpc(fox.vo,nil,false,true)
        end 
    end, 3)
    local mainline = TaskData.Instance:GetMainlineAccepted()
    if mainline == nil then return end 
    local params = Split(mainline.info.tasks_dialog_beh, "|")
    local time = tonumber(params[4]) or params[4]
    if time ~= nil and time > 0 then 
        self.timer_ct = TimeHelper:AddRunTimer(function ()
            SceneCtrl.Instance:Objs():ForeachSNPC(
                function(npc_obj)
                    if npc_obj.vo.id == Config.main_catch_fox_auto.level_monster_group[1].npc_id then 
                        fox = npc_obj
                        return true
                    end 
                    return false
                end 
            )
            if SceneData.Instance:SceneId() == Config.main_catch_fox_auto.find_fox[1].scene_id and fox then 
                SceneLogic.Instance:AutoToNpc(fox.vo,nil,false,true)
            elseif SceneData.Instance:SceneId() ~= Config.main_catch_fox_auto.find_fox[1].scene_id then 
                if self.timer_ct ~= nil then 
                    TimeHelper:CancelTimer(self.timer_ct)
                    self.timer_ct = nil
                end 
            end 
        end,time)
    end    
end

function FoxCompassPanel:onFlush()
    for i = 1, self.LightObjs:Length() do
        local last_index = i
        if self.anim_events[last_index] == nil then
            self.anim_events[last_index] =
                self.LightAnims[last_index]:ListenEvent(
                "AttrFloat",
                function(eve_param)
                    if eve_param == "wait_end" then
                        -- self.LightAnims[last_index]:SetTrigger(APH("Hide"))
                    elseif eve_param == "hide_end" then
                        self.LightObjs[last_index]:SetActive(false)
                    end
                end
            )
        end
    end

    self:StartAction()
end

function FoxCompassPanel:FlushLight(angle)
    local x, y = math.modf(angle / 45)
    local index = 8 - x
    index = y < 0.5 and index + 1 or index
    index = index == 0 and 1 or index
    index = index > 8 and 1 or index
    self.LightObjs[index]:SetActive(true)
    if self.last_index and self.last_index ~= index then
        self.LightAnims[self.last_index]:SetTrigger(APH("Hide"))
    end
    self.last_index = index
end

function FoxCompassPanel:StartAction()
    TimeHelper:CancelTimer(self.handle_time1)
    TimeHelper:CancelTimer(self.handle_time2)
    local angle = 360
    self.handle_time1 =
        TimeHelper:AddCountDownTT(
        function()
            self:FlushLight(angle)
            local ang = Quaternion.AngleAxis(angle, Vector3.up)
            ang.z = ang.y
            ang.y = 0
            self.Arrow.localRotation = ang
            angle = angle - 360 / 24
            --15度
        end,
        function()
            angle = 360
            local final = self:FinalAngle()
            local frame = (angle - final) / 8
            local temp = frame
            self.handle_time2 =
                TimeHelper:AddCountDownTT(
                function()
                    if angle > final then
                        self:FlushLight(angle)
                        local ang = Quaternion.AngleAxis(angle, Vector3.up)
                        ang.z = ang.y
                        ang.y = 0
                        self.Arrow.localRotation = ang
                        angle = angle - ((temp / frame) * 15)
                        temp = temp - 1
                    end
                end,
                function()
                    self.QuanObj:SetActive(true)

                    self.invoke_timer = TimeHelper:AddRunTimer(function()
                        self:onMainRolePosChange()
                    end, 0.3)
                end,
                (1 / 24) * frame,
                (1 / 24),
                false
            )
        end,
        1,
        (1 / 24),
        false
    )
end

function FoxCompassPanel:OnClickA()
    local state = self.Anim:get_Item("long1")
    state.speed = 2
end
function FoxCompassPanel:OnClickB()
    local state = self.Anim:get_Item("long1")
    state.speed = 1.5
end
function FoxCompassPanel:OnClickC()
    local state = self.Anim:get_Item("long1")
    state.speed = 1
end
function FoxCompassPanel:MapItemChange()
    self:onMainRolePosChange()
end
function FoxCompassPanel:OnDestroy()
    self.anim_events = {}
    self.last_index = nil
    UIWFlushPanel.OnDestroy(self)
    EventSys:UnBind(self.role_pos_event)
    TimeHelper:CancelTimer(self.handle_time1)
    TimeHelper:CancelTimer(self.handle_time2)

    TimeHelper:CancelTimer(self.invoke_timer)
    TimeHelper:CancelTimer(self.skill_timer)
    TimeHelper:CancelTimer(self.auto_timer)
    -- if self.timer_ct ~= nil then 
    --     TimeHelper:CancelTimer(self.timer_ct)
    --     self.timer_ct = nil
    -- end 
end
function FoxCompassPanel:FinalAngle()
    local fox_obj = SceneCtrl.Instance:Objs().client_objs[self.data.fox_obj_id]
    -- 直接取配置
    local patrol_list = CatchFoxData.Instance:GetPatrolList()
    local bir_pos = string.split(patrol_list[1].birth_xyz, "|")
    local cl_p,sam_set = SceneCtrl.Instance:CurScene():PosToClint(bir_pos[1], bir_pos[2])

    if MainPlayer == nil then return 0 end
    local pos = MainPlayer:GetPos()
    local target = fox_obj ~= nil and fox_obj:GetPos() or cl_p
    local xrad = math.atan2(target.x - pos.x, target.z - pos.z)
    local angle = xrad / math.pi * 180
    angle = 360 - angle + 90
    angle = angle < 0 and 360 + angle or angle
    angle = angle % 360
    return angle
end
-----0-----
--90---270-
----180----
function FoxCompassPanel:onMainRolePosChange()
    local fox_obj = SceneCtrl.Instance:Objs().client_objs[self.data.fox_obj_id]
    if fox_obj == nil then 
        return 
    end 
    if MainPlayer == nil then 
        return
    end
    local pos = MainPlayer:GetPos()
    local target = fox_obj:GetPos()
    local angle = self:FinalAngle()
    self:FlushLight(angle)
    local ang = Quaternion.AngleAxis(angle, Vector3.up)
    ang.z = ang.y
    ang.y = 0
    self.Arrow.localRotation = ang
    local dis = GameMath.GetDistance(pos.x, pos.z, target.x, target.z)
    self.LengthDesc.text = string.format(Language.Common.Distance, math.floor(math.sqrt(dis)))
    if dis < 1000 and dis > 200 then
        for i = 1, 3 do
            self.LongObjs[i]:SetActive(i == 1)
        end
    elseif dis < 200 and dis > 20 then
        for i = 1, 3 do
            self.LongObjs[i]:SetActive(i == 2)
        end
    elseif dis < 20 then
        for i = 1, 3 do
            self.LongObjs[i]:SetActive(i == 3)
        end
    end
end

function FoxCompassPanel:OnMainToggeleChange()
    self.BtnCatch:SetActive(not MainViewData.Instance.rb_toggle.isOn)
end

function FoxCompassPanel:OnClickCD()
    PublicPopupCtrl.Instance:Center(Language.Anecdote.FoxCatchSkill)
end

function FoxCompassPanel:OnClickCactch()
    self:FlushCD()

    self.data:SkillShow()

    if self.data:CheckSkillCatch() then return end 

    TimeHelper:CancelTimer(self.invoke_timer)

    self.data:CachedFox(0)---1 * math.floor(self:FinalAngle()))

    ViewMgr:CloseView(FoxCompassView)
end

function FoxCompassPanel:FlushCD()
    local cd = self.data:CachSkilCD()
    local timer = 1
    local interval = 0.1
    local per = interval/cd

    self.skill_timer = TimeHelper:AddCountDownTT(function()
        if not ViewMgr:IsOpen(FoxCompassView) then return end 
        self.CatchCdPic.gameObject:SetActive(true)
        self.PressEff:SetActive(false)
        self.CatchCdPic.fillAmount = timer - per 
        timer = timer - per
    end,
    function ()
        if not ViewMgr:IsOpen(FoxCompassView) then return end 
        self.is_cd = false
        self.CatchCdPic.gameObject:SetActive(false)
        self.PressEff:SetActive(true)
    end,cd,interval)
end