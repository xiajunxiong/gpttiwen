LBDragonPluseView = LBDragonPluseView or DeclareView("LBDragonPluseView", "lb_dragon_pluse/lb_dragon_pluse")
VIEW_DECLARE_MASK(LBDragonPluseView,ViewMask.Block)
VIEW_DECLARE_LEVEL(LBDragonPluseView, ViewLevel.L0)
function LBDragonPluseView:LBDragonPluseView()

end

function LBDragonPluseView:OnClickClose()
    ViewMgr:CloseView(LBDragonPluseView)
end

LBDragonPlusePanel = LBDragonPlusePanel or DeclareMono("LBDragonPlusePanel", UIWFlushPanel)
function LBDragonPlusePanel:LBDragonPlusePanel()
    self.data = LBDragonPluseData.Instance

    self.data_cares = {
        {data = self.data.my_info, func = self.BossPanel},
        {data = self.data.my_info, func = self.MyInfoPanel},
        {data = self.data.base_info, func = self.BossPanel},
        {data = self.data.base_info, func = self.MyInfoPanel},
    }

    self.boss_list = {}
    self.monster_list = {}
    self.Timer:SetCallBack(function ()
        ViewMgr:CloseView(LBDragonPluseView)
    end)
end

function LBDragonPlusePanel:BossPanel()
    if self.data.base_info.status ~= 2 then 
        return 
    end 

    if self.data.my_info.flush == 0 or self.data.my_info.flush == nil then 
        return 
    end 

    if self.data.base_info.flush == 0 or self.data.base_info.flush == nil then 
        return 
    end 

    local param = self.data:GetBossList()

    if param.fail then 
        return 
    end 

    if #self.boss_list == 0 then 
        for k,v in pairs(param.boss) do 
            local item_obj = self.BossCell.gameObject:Clone(self.BossPoses[k])
            item_obj:SetActive(true)
            local item_mono = item_obj:GetLuaComponent(self.BossCell:GetTypeName())
            UH.LocalPosG(item_obj,Vector3.New(0,0,0))
            item_mono:SetData(v)
            table.insert(self.boss_list,item_mono)
        end 
    else 
        for k,v in pairs(self.boss_list) do 
            v:SetData(param.boss[k])
        end 
    end 

    if #self.monster_list == 0 then 
        for k,v in pairs(param.monster) do 
            local item_obj = self.BossCell.gameObject:Clone(self.MonsterPoses[k])
            item_obj:SetActive(true)
            local item_mono = item_obj:GetLuaComponent(self.BossCell:GetTypeName())
            UH.LocalPosG(item_obj,Vector3.New(0,0,0))
            item_mono:SetData(v)
            table.insert(self.monster_list,item_mono)
        end 
    else 
        for k,v in pairs(self.monster_list) do 
            v:SetData(param.monster[k])
        end 
    end 
end

function LBDragonPlusePanel:MyInfoPanel()
    if self.data.my_info.flush == 0 or self.data.my_info.flush == nil then 
        return 
    end 

    if self.data.base_info.flush == 0 or self.data.base_info.flush == nil then 
        return 
    end 

    local param = self.data:GetMyInfo()
    UH.SetText(self.MonsterNum,Language.LBDragonPluse.Last..param.monster_num)
    UH.SetText(self.BossNum,Language.LBDragonPluse.Last..param.boss_num)
    UH.SetText(self.MonsterReward,Language.LBDragonPluse.Last..param.monster_reward)
    UH.SetText(self.BossJoinReward,Language.LBDragonPluse.Last..param.boss_reward_times)
    UH.SetText(self.BossDefeatReward,Language.LBDragonPluse.Last..param.boss_remain_times)
    
    self.Timer:StampTime(param.stamp_time,TimeType.Type_Time_0,Language.LBDragonPluse.LastTime)
    
    self.InspireTimer.gameObject:SetActive(param.inspire_time > 0 )
    self.InspireTimer:StampTime(param.inspire_time,TimeType.Type_Time_0)
end

function LBDragonPlusePanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[379].desc})
end

function LBDragonPlusePanel:OnClickCall()
    local cd = self.data:GetCallFriendCD()
    if cd > 0 then 
        PublicPopupCtrl.Instance:Center(string.format(Language.LBDragonPluse.WaitForCallNext,cd))
        return 
    end 
    local str = string.format(Language.LBDragonPluse.CallFriend, RoleData.Instance:GetRoleName())
    self.data:SetCallFriendCD()
    LBDragonPluseCtrl.Instance:ReqCallHelper()
    -- LBDragonPluseCtrl.Instance:ReqDragonSharekBoss({})
end

function LBDragonPlusePanel:OnClickInspire()
    ViewMgr:OpenView(LBDragonPluseInspireView)
end

function LBDragonPlusePanel:OnClickViewCell(data)
    if data.type == 2 and LBDragonPluseData.Instance.my_info.kill_monster_num > data.index then 
        PublicPopupCtrl.Instance:Center(Language.LBDragonPluse.TargetKilled)
        return 
    elseif data.type == 2 and LBDragonPluseData.Instance.my_info.kill_monster_num+1 < data.index then 
        PublicPopupCtrl.Instance:Center(Language.LBDragonPluse.TargetCantKill)
        return
    end 

    if data.type == 1 and data.life == 0 then 
        PublicPopupCtrl.Instance:Center(Language.LBDragonPluse.TargetKilled)
        return 
    end 

    self.data:SetOperateBoss(data)
    ViewMgr:OpenView(LBDragonPluseDetailView)    
--     ViewMgr:OpenView(LBDragonPluseDetailView,{data = data})    
end

LBDragonPluseViewCell = LBDragonPluseViewCell or DeclareMono("LBDragonPluseViewCell", UIWidgetBaseItem)
function LBDragonPluseViewCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self.BossType:SetActive(data.type == 1)
    self.BossName.gameObject:SetActive(data.type == 1)
    self.BossTimes.gameObject:SetActive(data.type == 1)

    self.MonsterType:SetActive(data.type == 2)
    self.MonsterScore.gameObject:SetActive(data.type == 2)

    if data.type == 1 then 
        local rate = self.data.life / 10000
        self.BossLife:SetProgress(rate)

        local show = rate * 100
        UH.SetText(self.BossPercent,show.."%")

        UH.SetText(self.BossName,self.data.name)
        UH.SetText(self.BossTimes,Language.LBDragonPluse.BossTimeDesc..self.data.times)
    else 
        UH.SetText(self.MonsterName,self.data.name)
        UH.SetText(self.MonsterScore,self.data.score)
    end 


    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end

    self.ui_obj:SetData(self.data.model_path)

    self.Inter.Interactable = (data.type == 2 and LBDragonPluseData.Instance.my_info.kill_monster_num < self.data.index) or data.type == 1
    self.Done:SetActive((data.type == 2 and LBDragonPluseData.Instance.my_info.kill_monster_num >= self.data.index) 
        or (data.type == 1 and data.life == 0))

end 

function LBDragonPluseViewCell:OnRelease()
    UIWidgetBaseItem.OnRelease(self)

    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil
end

-- 
LBDragonPluseMapSp = LBDragonPluseMapSp or DeclareMono("LBDragonPluseMapSp", UIWFlushPanel)
function LBDragonPluseMapSp:LBDragonPluseMapSp()
    self.data = LBDragonPluseData.Instance
end

-- 
function LBDragonPluseMapSp:SetLinkMark(link_mark)
    self.link_mark = link_mark
    self:FlushIconShow()
end

function LBDragonPluseMapSp:FlushIconShow()
    UH.SpriteName(self.NameShow, self.data:GetDragonVienNameByLinkMark(self.link_mark))
end


LBDragonPluseIconSp = LBDragonPluseIconSp or DeclareMono("LBDragonPluseIconSp", UIWFlushPanel)
function LBDragonPluseIconSp:LBDragonPluseIconSp()
    self.data = LBDragonPluseData.Instance
    self.data_cares = {
        {data = self.data.base_info, func = self.FlushSp},
    }
end 

function LBDragonPluseIconSp:FlushSp()
    if self.data.base_info.anger_percent == nil then return end 
    self.Pro:SetProgress(self.data.base_info.anger_percent/10000)
    self.Pro:SetText((self.data.base_info.anger_percent/100).."%")

    local status_show = self.data:GetActIconStatusShow()
    self.Acting:SetActive(status_show == 2)
    self.Waiting:SetActive(status_show == 1)
    self.Pro.gameObject:SetActive(status_show == 1)
    self.BtnTips:SetActive(status_show == 1 or status_show == 2)
    self.Close:SetActive(status_show ~= 2 and status_show ~= 1)

    self.Timer.gameObject:SetActive(status_show == 2)
    self.Timer:StampTime(self.data.base_info.dragon_vein_close_timestamp,TimeType.Type_Time_0)
end

function LBDragonPluseIconSp:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[378].desc})
end