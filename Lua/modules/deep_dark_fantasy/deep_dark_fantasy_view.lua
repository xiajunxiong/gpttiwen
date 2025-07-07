DeepDarkFantasyView = DeepDarkFantasyView or DeclareView("DeepDarkFantasyView", "deep_dark_fantasy/deep_dark_fantasy", Mod.DeepDarkFantasy.Main)
VIEW_DECLARE_MASK(DeepDarkFantasyView, ViewMask.Block)
VIEW_DECLARE_LEVEL(DeepDarkFantasyView, ViewLevel.Lu)
function DeepDarkFantasyView:DeepDarkFantasyView()
    self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1

    self.data = DeepDarkFantasyData.Instance
    self.data:InitView()

    MainOtherCtrl.Instance:CloseKeyUse()

    if not ViewMgr:IsOpen(VideoView) then 
        if self.data.is_layer_change == nil then 
            self.data.model_data.start = 1
        else
            self.data.model_data.start = self.data.is_layer_change and 1 or 2
        end
    end

    ActFormationCtrl.Instance:SendGetFormationInfo(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_NETHERWORLD)
end 

function DeepDarkFantasyView:LoadCallback()
    --LogError("loadCallback",Time.frameCount)
    self.delay_init_model_scene = TimeHelper:AddDelayFrameTimer(function ()
        self:InitModelScene()
        self:InitQualitySettings()
    end,1)
end

function DeepDarkFantasyView:OpenCallback()
    ViewMgr:CloseView(ChatView)
end

-- 目前按照云顶的规则进行预设初始化
function DeepDarkFantasyView:InitModelScene()
    -- LogError("?!@InitModelScene")
    ResMgr:LoadGameObject("actor/other/YMLY_1.prefab", function (obj)
        -- LogError("OnLoad YMLY",Time.frameCount)
        self.obj = obj
        self.obj.transform.localPosition = Vector3.New(1000,1000,1000)
        self.obj:SetParent(UnityGameObj.Find("ResidentObjectManager"))
        self.delay_closelight = TimeHelper:AddDelayFrameTimer(function()
            self:CloseLight()
        end,2)
    end, true, false)

end

function DeepDarkFantasyView:InitQualitySettings()
    -- LogError("?!?@InitQualitySettings")
    self:OnQualitySettings()
    -- self:OnQualitySettings()
    DeepDarkFantasyCtrl.CloseFunc = function ()
        -- LogError("?!@?CloseFunc")
        local quality = RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SET_PICTUER_QUALITY)
        if quality > 0 then
            GpQualityMgr:SetShadowEnable(true)
        end
        if self.light_coms then
            for i=0, self.light_coms.Length - 1 do
                if self.light_coms[i] ~= nil and not self.light_coms[i]:Equals(nil) then
                    self.light_coms[i].enabled = true
                end
            end
        end
        UnityEngine.RenderSettings.fog = self.fog
        UnityEngine.RenderSettings.ambientLight = self.ambientLight
        UnityEngine.QualitySettings.pixelLightCount = self.pixelLightCount
        UnityEngine.QualitySettings.anisotropicFiltering = self.anisotropicFiltering
        UnityEngine.QualitySettings.shadows = self.shadows
        UnityEngine.QualitySettings.shadowResolution = self.shadowResolution
        UnityEngine.QualitySettings.shadowCascades = self.shadowCascades
        UnityEngine.QualitySettings.shadowDistance = self.shadowDistance
    end

    self.scene_care_handle = SceneMgr:CareLoadedSceneIdChange(BindTool.Bind(self.OnQualitySettings,self, true))
end

function DeepDarkFantasyView:CloseLight()
    -- LogError("?!?@CloseLight")
    local quality_obj = SceneMgr.FindGameObjectWithTag("Quality")
    if quality_obj then
        self.light_coms = quality_obj:GetComponentsInChildren(typeof(UnityEngine.Light))
        for i = 0, self.light_coms.Length - 1 do
            self.light_coms[i].enabled = false
        end
    end
end

function DeepDarkFantasyView:OnQualitySettings(close_light)
    -- LogError("!@?OnQualitySettings")
    GpQualityMgr:SetShadowEnable(false)
    if close_light then
        self:CloseLight()
    end
    self.fog = UnityEngine.RenderSettings.fog
    self.ambientLight = UnityEngine.RenderSettings.ambientLight
    self.pixelLightCount = UnityEngine.QualitySettings.pixelLightCount
    self.anisotropicFiltering = UnityEngine.QualitySettings.anisotropicFiltering
    self.shadows = UnityEngine.QualitySettings.shadows
    self.shadowResolution = UnityEngine.QualitySettings.shadowResolution
    self.shadowCascades = UnityEngine.QualitySettings.shadowCascades
    self.shadowDistance = UnityEngine.QualitySettings.shadowDistance
    UnityEngine.RenderSettings.fog = false
    UnityEngine.RenderSettings.ambientLight = COLORS.CloudArena
    UnityEngine.QualitySettings.pixelLightCount = 20
    UnityEngine.QualitySettings.anisotropicFiltering = 2
    UnityEngine.QualitySettings.shadows = 3
    UnityEngine.QualitySettings.shadowDistance = 100
    UnityEngine.QualitySettings.shadowResolution = 2
    UnityEngine.QualitySettings.shadowCascades = 4
end

function DeepDarkFantasyView:CloseCallback()
    -- LogError("!@?CloseCallback")
    if DeepDarkFantasyCtrl.CloseFunc then
        DeepDarkFantasyCtrl.CloseFunc()
        DeepDarkFantasyCtrl.CloseFunc = nil
    end
    if self.obj then
        UnityGameObj.Destroy(self.obj)
    end
    if self.delay_init_model_scene ~= nil then
        TimeHelper:CancelTimer(self.delay_init_model_scene)
        self.delay_init_model_scene = nil
    end
    if self.delay_closelight ~= nil then
        TimeHelper:CancelTimer(self.delay_closelight)
        self.delay_closelight = nil
    end
    if nil ~= self.scene_care_handle then
        SceneMgr:UncareLoadedSceneIdChange(self.scene_care_handle)
        self.scene_care_handle = nil
    end

    self.data.model_data.start = 0
end

DeepDarkFantasyPanel = DeepDarkFantasyPanel or DeclareMono("DeepDarkFantasyPanel", UIWFlushPanel)
function DeepDarkFantasyPanel:DeepDarkFantasyPanel()
    self.data = DeepDarkFantasyData.Instance
    self.ctrl = DeepDarkFantasyCtrl.Instance
    self.data_cares = {
        {data = self.data.view_data, func = self.onFlush,keys = {"flush_num"} },
        {data = self.data.bubble_data,func = self.FlushBubble,init_call = false},
        {data = self.data.model_data,func = self.StartBubble,keys = {"is_idle"},init_call = false},
        {data = self.data.miling_data, func = self.FlushMiLing },
        {data = self.data.view_data, func = self.TryChallenge, keys = {"quick_challenge"},init_call = false},
        {data = self.data.view_data, func = self.OnClickBack ,keys = {"reset"},init_call = false}
    }

    self.show_target = nil
    self.init = false

    self.endtimer_obj:SetActive(false)
    -- self.endtimer:SetShowCallBack(function(t)
    --     return t.day > 0 and string.format(Language.UiTimeMeter.TimeStr6,t.day,t.hour)
	--     or string.format(Language.UiTimeMeter.Special4,t.hour,t.min)
    -- end)

    self.show_bubble = nil
    self.hide_bubble = nil
end

function DeepDarkFantasyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    for i = 1, 3 do 
        self.box_list[i]:OnRelease()
    end 

    if self.wait_handle ~= nil then
        TimeHelper:CancelTimer(self.wait_handle)
        self.wait_handle = nil
    end 

    if self.show_bubble ~= nil then
        TimeHelper:CancelTimer(self.show_bubble)
        self.show_bubble = nil
    end 
    
    if self.hide_bubble ~= nil then
        TimeHelper:CancelTimer(self.hide_bubble)
        self.hide_bubble = nil
    end 
end

function DeepDarkFantasyPanel:onFlush()
    if self.data.view_data.send_data == nil and self.data.view_data.init_data == nil then 
        return 
    end 

    local operate_data = self.data:CacheOperateData()

    UH.SetText(self.cur_layer, string.format( Language.DeepDarkFantasy.LayerShow,DataHelper.GetDaXie(operate_data.index)))
    UH.SetText(self.cur_name, operate_data.monster_name)
    UH.SetIcon(self.cur_icon, operate_data.monster_icon, ICON_TYPE.ITEM)
    if operate_data.monster_recommend > 0 then
        self.cur_recommend:SetData(EleCrystal.GetElement(operate_data.monster_recommend))
    end
    
    for i = 1, 3 do 
        if operate_data.reward_list[i] ~= nil then 
            self.box_list[i]:SetData(operate_data.reward_list[i])
        end 
    end 

    -- self.endtimer:StampTime(self.data:CheckNextWeekTime(),TimeType.Type_Time_6)

    local info = self.data:GetChallengeBossList()

    self.ultimate_show_red_1 = self.ultimate_show_red_1 or UiRedPoint.New(self.ultimate_red_1, Vector3.New(0,0,0))
    self.ultimate_show_red_1:SetNum(self.data:CheckBossBoxIsRed(info.last_one.final_reward) and 1 or 0)

    self.ultimate_show_red_2 = self.ultimate_show_red_2 or UiRedPoint.New(self.ultimate_red_2, Vector3.New(0,0,0))
    self.ultimate_show_red_2:SetNum(self.data:CheckBossBoxIsRed(info.last_one.final_reward) and 1 or 0)

    UH.SetText(self.limit_times,Language.DeepDarkFantasy.LimitTime..self.data:GetTodayTimes())
    
end

-- 泡泡启动
function DeepDarkFantasyPanel:StartBubble()
    local operate_data = self.data:CacheOperateData()
    if self.data.model_data.is_idle == 0 then 
        self.PPshow:SetActive(false)
        if self.show_bubble ~= nil then
            TimeHelper:CancelTimer(self.show_bubble)
            self.show_bubble = nil
        end 
        
        if self.hide_bubble ~= nil then
            TimeHelper:CancelTimer(self.hide_bubble)
            self.hide_bubble = nil
        end 
        return 
    end 

    self.data:SetBubble(operate_data.monster_bubble_id)
end

-- 泡泡刷新
function DeepDarkFantasyPanel:FlushBubble()
    self.PPshow:SetActive(true)
    UH.SetText(self.PPtext, 
    RichTextHelper.ParseAllFunc(self.data:GetBubble()[self.data.bubble_data.talk_id].talk_text, true, RichTextFormat.FormatNpcBubbleSpecial))
    self.show_bubble = TimeHelper:AddDelayTimer(function()
        self.PPshow:SetActive(false)
        self.hide_bubble = TimeHelper:AddDelayTimer(function()
            self.data:TurnBubble()
        end,Config.netherworld_purgatory_auto.other[1].show_time)
    end, Config.netherworld_purgatory_auto.other[1].hold_time)
end

function DeepDarkFantasyPanel:FlushMiLing()
    UH.SetText(self.mid_key, self.data.miling_data.mid_key)
    UH.SetText(self.high_key,self.data.miling_data.high_key)
end

-- 关闭
function DeepDarkFantasyPanel:OnClickBack()
    -- if DeepDarkFantasyCtrl.CloseFunc then
    --     DeepDarkFantasyCtrl.CloseFunc()
    --     DeepDarkFantasyCtrl.CloseFunc = nil
    -- end
    ViewMgr:CloseView(DeepDarkFantasyView)
end

-- 排行榜
function DeepDarkFantasyPanel:OnClickRank()
    ViewMgr:OpenView(DeepDarkFantasyRankView)
end

-- 布阵
function DeepDarkFantasyPanel:OnClickBuZhen()
    ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_NETHERWORLD)
end

-- 提示
function DeepDarkFantasyPanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[137].desc})
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end

-- 挑战
function DeepDarkFantasyPanel:OnClickChallenge()
    if self.data:GetTodayTimes() <= 0 then 
        PublicPopupCtrl.Instance:Center(Language.DeepDarkFantasy.TodayTimeAllPass)
        return 
    end 


    local function send_msg()
        self:OnClickBuZhen()
        PublicPopupCtrl.Instance:CloseDialogTips()
    end

    ActFormationData.Instance:SetCurFormationInfo(2)
    if ActFormationData.Instance:GetAssistCount() < 4 then
        local info = {
            content = Language.DeepDarkFantasy.PetErrorDialog,
            cancel = {
                name = nil,
                func = function ()
                    ActFormationData.Instance.formation_smart.auto_save_type = -1
                    PublicPopupCtrl.Instance:CloseDialogTips()
                    self:TryChallenge()
                end
            },
            confirm = {
                name = nil,
                func = send_msg
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end

    self:TryChallenge()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function DeepDarkFantasyPanel:TryChallenge()
    if self.data.view_data.send_data == nil then return end 

    self.data:Challenge()
    self.wait_handle = TimeHelper:AddDelayTimer(function()
        self.dark:SetActive(true)
        DeepDarkFantasyCtrl.Instance:SendOperateReq(1,self.data.view_data.send_data.chapter,self.data.view_data.send_data.challenge_id)
    end, tonumber(self.battle_black))
end

function DeepDarkFantasyPanel:OnClickBoxRoll()
    ViewMgr:OpenView(DeepDarkFantasyBoxRoll)
end

-- 点亮终极宝箱界面时刷新终极浏览信息
function DeepDarkFantasyPanel:OnClickUltimate()
   self.ultimate_show:SetActive(true)
   local info = self.data:GetChallengeBossList()
   local list = {}
   for k,v in pairs(info.last_one.final_reward.reward) do 
       local item = Item.Create(v)
       table.insert(list,item)
   end 
   self.ultimateShowList:SetData(list)   
   local flag_inter = self.data:CheckBossBoxIsInter(info.last_one.final_reward)
   local flag_red = self.data:CheckBossBoxIsRed(info.last_one.final_reward) 
   self.btn_ultimate_inter.Interactable = flag_inter
   UH.SetText(self.ultimate_text, not flag_inter and Language.Common.BtnState[1] or (flag_red and Language.Common.BtnState[2] or Language.Common.Confirm))
   AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end

function DeepDarkFantasyPanel:OnClickSureUltimate()
    local info = self.data:GetChallengeBossList()
    local flag_inter = self.data:CheckBossBoxIsInter(info.last_one.final_reward)
    local flag_red = self.data:CheckBossBoxIsRed(info.last_one.final_reward) 

    if flag_red and flag_inter then 
        DeepDarkFantasyCtrl.Instance:SendOperateReq(2,info.last_one.final_reward.chapter,info.last_one.final_reward.challenge_id,0)
    end 
    if not flag_inter then 
        PublicPopupCtrl.Instance:Center(Language.DeepDarkFantasy.UltimateCompleteClick)
        return 
    end 
    self:OnClickUltimateClose()
end

function DeepDarkFantasyPanel:OnClickUltimateClose()
    self.ultimate_show:SetActive(false)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

-- 点击下方的箱子
function DeepDarkFantasyPanel:OnClickRewardBox(data)
    DeepDarkFantasyCtrl.Instance:SendOperateReq(2,data.chapter,data.challenge_id,data.index)
    if data.index > 1 then self.data:MiLingMark({is_gold = data.index > 2}) end 

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

-- 左半边界面统合
DarkFantasyLeftDown =  DarkFantasyLeftDown or DeclareMono("DarkFantasyLeftDown", UIWFlushPanel)
function DarkFantasyLeftDown:DarkFantasyLeftDown()
    self.data = DeepDarkFantasyData.Instance
    self.ctrl = DeepDarkFantasyCtrl.Instance
    self:FlushBossShow()

    self.data_cares = {
        {data = self.data.model_data, func = self.AdjustModel},
        {data = self.data.view_data, func = self.onPop , keys = {"flush_num"} ,init_call = false},
        {data = self.data.view_data, func = self.FlushBossListByChapter , keys = {"chapters"} ,init_call = false},
    }
    
    self.BlackAnim:ListenEvent("anim_state",function (eve_param,anim_st_info)
        if eve_param == "complete" then 
            self.data:TransMitsComplete()
        end 
    end)

    self.chapter_list:SetCompleteCallBack(function()
        if self.init == nil then   
            local init_index = self.data:GetInitChapter()
            local operate_index = init_index > 14 and  init_index - 14 or (init_index < 3 and 1 or init_index - 2)
            self.chapter_list:GetItem(init_index).ItemClick:Trigger()
            self.chapter_list:AutoSlide(self.chapter_list:GetItem(operate_index):GetData(),true)
        end 
    end)

    self.BossList:SetCompleteCallBack(function()
        if not self.wait_init then   
            local init_index = self.data:GetInitIndex()
            if MAX_NETHER_WORLD_GATORY_TASK_NUM == init_index then 
                self.selectgroup:SetAllTogglesOff()
                self:OnClickItem(self:GetData(),true)
            else 
                self.BossList:GetItem(init_index).ItemClick:Trigger()
                local operate_index = init_index > 2 and  init_index - 2 or (init_index > 1 and init_index - 1 or init_index)
                self.BossList:AutoSlide(self.BossList:GetItem(operate_index):GetData(),false)
            end 
            self.wait_init = true
        end 
    end)
    self.temp_data = nil
    self.red_points = {}
    self.boss_red_points = {}
    self.wait_handle = nil
    self.black_handle = nil

    self.wait_init = false

    -- self.n_unlock_timer:SetShowCallBack(function(t)
    --     return t.day > 0 and string.format(Language.UiTimeMeter.TimeStr6,t.day,t.hour)
	--     or string.format(Language.UiTimeMeter.Special4,t.hour,t.min)
    -- end)

end 

function DarkFantasyLeftDown:AdjustModel()
    if self.data.model_data.is_go_next == 1 then 
        self.wait_handle = TimeHelper:AddDelayTimer(function()
            self.Black:SetActive(true)
        end, tonumber(self.Black_Time))
    else 
        if self.wait_handle ~= nil then
            TimeHelper:CancelTimer(self.wait_handle)
            self.wait_handle = nil
        end 
    end 

    if self.data.model_data.is_next_enter == 1 then 
        self.BlackAnim:SetTrigger("complete")
        self.black_handle = TimeHelper:AddDelayTimer(function()
            self.Black:SetActive(false)
        end, 0.3)
    else
        if self.black_handle ~= nil then
            TimeHelper:CancelTimer(self.black_handle)
            self.black_handle = nil
        end 
    end 

    self.OperBlock:SetActive(self.data.model_data.cur_state ~= 0)

    local list = self.data:GetChallengeBossList()
    if not self.data:CheckBossIsLock(list.last_one) and self.handle == nil 
        and self.EffectTool.isActiveAndEnabled then 
        self.handle = self.EffectTool:Play(3165004)
    elseif self.handle ~= nil then 
        self.EffectTool:Stop(self.handle)
        self.handle = nil
    end 
end

function DarkFantasyLeftDown:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data.view_data.send_data = nil 
    self.black_handle = nil
    self.wait_handle = nil
    self.data.bubble_data.id = 0
    self.data.is_layer_change = nil
    self.data:ClearInit()
end 

function DarkFantasyLeftDown:FlushBossListByChapter()
    local list = self.data:GetChallengeBossList()
    self.BossList:SetData(list.challenge_list)
    self.BossItem:SetData(list.last_one)
end

function DarkFantasyLeftDown:FlushBossShow()
    local list = self.data:GetChallengeBossList()
    local chapter_list = self.data:GetChallengeChapterList()
    self.chapter_list:SetData(chapter_list)
    self.BossList:SetData(list.challenge_list)
    self.BossItem:SetData(list.last_one)
end

-- 检查bosslist的越界并将其拉回
function DarkFantasyLeftDown:BossListOutSideCheckIn(data)
    local item = self.BossList:GetItem(data.index)
    local item_pos = item.gameObject:GetLocalPosition()
    local item_size = UH.SizeDelta(item.gameObject)
    local content_pos = self.BossContent:GetLocalPosition()
    local content_size = UH.SizeDelta(self.BossList.gameObject)
    local pass_pos = UH.SizeDelta(self.pass)

    -- 上半越界检查
    local item_top = item_pos.y + (item_size.y/2)
    local flag_top_out = item_top + content_pos.y
    if flag_top_out > 0 then 
        self.BossList:ForceJumpVertical(content_pos.y - flag_top_out)
        return 
    end 
    -- 下半越界检查
    local is_last = #self.BossList.item_list == data.index
    local item_bottom = item_pos.y - (item_size.y/2)
    local flag_bottom_out = item_bottom + (content_pos.y + content_size.y) - (is_last and 0 or pass_pos.y)
    if flag_bottom_out < 0 then 
        self.BossList:ForceJumpVertical(content_pos.y - flag_bottom_out)
        return 
    end 
end

function DarkFantasyLeftDown:ResetPopPanel()
    if self.box_show.activeSelf then 
        AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ItemInfoClose)
        self.box_show:SetActive(false)
    end 
    self.bossPop.gameObject:SetActive(false)
    self.nomalPop.gameObject:SetActive(false)
    self.ClickArea:SetActive(false)
    self.pass:SetActive(true)
end

-- 传送
function DarkFantasyLeftDown:OnClickTransMits(data)
    if self.data:CheckIsSameData(data) then 
        PublicPopupCtrl.Instance:Center(Language.DeepDarkFantasy.SameLayer)
        return 
    end 

    if self.data:CheckBossIsLock(data) then
        PublicPopupCtrl.Instance:Center(Language.DeepDarkFantasy.HadNotUnLock)
        return 
    end 
    self.data:TransMits(data)
    self:ResetPopPanel()
end

function DarkFantasyLeftDown:OnClickBlock()
    if self.box_show.activeSelf then 
        AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ItemInfoClose)
        self.box_show:SetActive(false)
    else
        self:ResetPopPanel()
    end 
end

function DarkFantasyLeftDown:onPop(param)
    local operate_data = self:GetData() == nil and self.data:CacheOperateData() or self:GetData()

    if operate_data == nil then 
        self.data:InitView()
        self.data:InitComplete()
    end 
    
    -- 这个点击用于刷新宝箱展示的宝箱上的红点
    self:OnClickItem(operate_data,true)
    if operate_data.index <= MAX_NETHER_WORLD_GATORY_TASK_NUM - 1 then 
        if self.BossList ~= nil and self.BossList:GetSelectedIndex(operate_data) ~= nil then 
            local item = self.BossList:GetItem(self.BossList:GetSelectedIndex(operate_data))
            if item ~= nil then item:ActRedPoint() end 
        end 
    end 

    local list = self.data:GetChallengeBossList()
    if not self.data:CheckBossIsLock(list.last_one) and self.handle == nil 
        and self.EffectTool.isActiveAndEnabled then 
        self.handle = self.EffectTool:Play(3165004)
    elseif self.handle ~= nil then 
        self.EffectTool:Stop(self.handle)
        self.handle = nil
    end 

    self:FlushBossShow()
end 


function DarkFantasyLeftDown:OnClickChapter(data)
    if self.data:GetChapterIsLock(data) then 
        PublicPopupCtrl.Instance:Center(Language.DeepDarkFantasy.ChapterLock)
        return 
    end 
    self.data:SetChapter(data)
end

-- 点击boss时展示boss的奖励信息
function DarkFantasyLeftDown:OnClickItem(data,not_click)
    if not self.init then 
        self.init = true
    end 

    if not self.wait_init then 
        return 
    end 

    if not_click and not (self.bossPop.gameObject.activeSelf or self.nomalPop.gameObject.activeSelf) then 
        return 
    end 

    if self.bossPop.gameObject.activeSelf or self.nomalPop.gameObject.activeSelf then 
        self:ResetPopPanel()
    end 

    local item = self.BossList:GetItem(data.index)
    local trans_mit = self.data:CheckBossIsUnknow(data)

    if item ~= nil then 
        self.pass:SetActive(not (#self.BossList.item_list == data.index))
        self:BossListOutSideCheckIn(data) 
    end 
    if item == nil then 
        self.bossPop.gameObject:SetActive(true)
        -- 信息刷新
        for i = 1,self.b_box_num do  -- 这个值在预设上
            self.b_box_inters[i].Interactable = self.data:CheckBossBoxIsInter(data.reward_list[i])
            self.boss_red_points[i] = self.boss_red_points[i] or UiRedPoint.New(self.b_box_inters[i].gameObject, Vector3.New(30,30,0))
            self.boss_red_points[i]:SetNum(self.data:CheckBossBoxIsRed(data.reward_list[i]) and 1 or 0)
        end
        -- flushtime
        local flag = self.data:CheckBossIsLock(data)
        self.b_unlock_obj:SetActive(flag)
        self.b_trans_btn:SetActive(not flag)

        self.selectgroup:SetAllTogglesOff()
    else 
        self.nomalPop.gameObject:SetActive(true)
        self.nomalPop.gameObject:SetParent(item.gameObject,false)
        UH.LocalPosG(self.nomalPop.gameObject, Vector3.New(0,18,0))
        self.nomalPop.gameObject:SetParent(self.PopParent,true)

        -- 信息刷新
        for i = 1,self.n_box_num do  -- 这个值也在预设上
            self.n_box_inters[i].Interactable = self.data:CheckBossBoxIsInter(data.reward_list[i])
            self.red_points[i] = self.red_points[i] or UiRedPoint.New(self.n_box_inters[i].gameObject, Vector3.New(30,30,0))
            self.red_points[i]:SetNum(self.data:CheckBossBoxIsRed(data.reward_list[i]) and 1 or 0)
        end 
        -- flushtime
        self.n_unlock_obj:SetActive( trans_mit)
        self.n_trans_btn:SetActive(not trans_mit)


        -- if not trans_mit then 
        UH.SetText(self.n_unlock_timer, Language.DeepDarkFantasy.BossLockShort)
        -- end 
        -- if timeshow ~= nil then 
        --     self.n_unlock_timer:StampTime(timeshow.stamp_time,timeshow.show_type,timeshow.text)
        -- end 
    end 

    self.temp_data = data

	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.ClickArea:SetActive(true)
end

-- 点击宝箱时展示宝箱预览
function DarkFantasyLeftDown:OnClickBox(index)
    if not self.data:CheckBossBoxIsInter(self.temp_data.reward_list[index]) then 
        PublicPopupCtrl.Instance:Center(Language.DeepDarkFantasy.GotBoxTips)
    end 

    if self.data:CheckBossBoxIsRed(self.temp_data.reward_list[index]) then 
        self.ctrl:SendOperateReq(2,self.temp_data.reward_list[index].chapter,self.temp_data.reward_list[index].challenge_id,index)
        if index > 1 then self.data:MiLingMark({is_gold = index > 2}) end 
        return 
    end 

    local is_nomal = self.nomalPop.gameObject.activeSelf 

    local nomal_pos = self.nomalPop.gameObject:GetLocalPosition()
    local nev = (is_nomal and nomal_pos.y >0) and -1 or 1

    self.box_show:SetActive(true)
    self.box_show:SetParent(is_nomal and self.n_show_pos or self.b_show_pos,false)
    UH.LocalPosG(self.box_show, Vector3.New(0,160*nev),0)

    UH.SetText(self.show_name, Language.DeepDarkFantasy.RewardBox[index])

    local list = {}
    for k,v in pairs(self.temp_data.reward_list[index].reward) do 
        local item = Item.Create(v)
        table.insert(list,item)
    end 
    self.BoxShowList:SetData(list)
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ItemInfoOpen)
end

function DarkFantasyLeftDown:GetData()
    return self.temp_data
end

-- 界面下方的奖励展示的cell
DarkFantasyBoxShow = DarkFantasyBoxShow or DeclareMono("DarkFantasyBoxShow", UIWidgetBaseItem)
function DarkFantasyBoxShow:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local info = Config.score_condition_auto.condition[data.condition]
    UH.SetText(self.desc, info.desc)

    local list = {}
    for k,v in pairs(self.data.reward) do 
        local item = Item.Create(v)
        table.insert(list,item)
    end 
    self.reward_list:SetData(list)

    local list = {}
    if info.condition_type == 5 and self.partner_list ~= nil then 
        local vo_1 = {icon = info.param_1,type = 1}
        table.insert(list,vo_1)
    elseif info.condition_type == 12 and self.partner_list ~= nil then 
        local vo_2 = {icon = info.param_1,type = 2}
        table.insert(list,vo_2)
    end 

    if self.partner_list ~= nil then 
        self.partner_list.gameObject:SetActive(#list > 0 )
        if #list > 0 then self.partner_list:SetData(list) end 
    end 

    --注册红点
	if not self.is_care then
        self.is_care = true
        self:careData(DeepDarkFantasyData.Instance.challenge_data, BindTool.Bind(self.ActRedPoint, self))
    else 
        self:ActRedPoint()
	end
end 

function DarkFantasyBoxShow:ActRedPoint()
    self.red_point = self.red_point or UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
	self.red_point:SetNum(DeepDarkFantasyData.Instance:CheckBossBoxIsRed(self.data) and 1 or 0)

    local complete_flag = not DeepDarkFantasyData.Instance:CheckBossBoxIsInter(self.data)
    self.is_complete:SetActive(complete_flag)
    self.box_inter.Interactable = not complete_flag

    for k,v in pairs(self.reward_list.item_list) do 
        v:FlushInter(complete_flag)
    end 
end

function DarkFantasyBoxShow:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
	if not self.is_care then return end
    self:uncareData()
end

-- 奖励物品展示的cell
DeepDarkRewardCell = DeepDarkRewardCell or DeclareMono("DeepDarkRewardCell", UIWidgetBaseItem)
function DeepDarkRewardCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.num ~= nil then 
        self.Cell:SetData(Item.Create({item_id = data.item_id}))
        UH.SetText(self.num, data.num)
        self.num.gameObject:SetActive(true)
    else 
        self.Cell:SetData(data)
    end 
end

function DeepDarkRewardCell:FlushInter(flag)
    self.inter.Interactable = flag
end

-- boss展示的cell
DarkFantasyBossCell = DarkFantasyBossCell or DeclareMono("DarkFantasyBossCell", UIWidgetBaseItem)
function DarkFantasyBossCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    local is_unknow = DeepDarkFantasyData.Instance:CheckBossIsUnknow(data) 
    local is_unlock = DeepDarkFantasyData.Instance:CheckBossIsLock(data)

    if not self.IsLast then 
        UH.SetIcon(self.icon, data.monster_icon, ICON_TYPE.ITEM)
        -- 锁定是还不能打
        self.is_lock:SetActive(is_unlock or is_unknow)
        -- 蒙面功能删除
        self.unknown:SetActive(false)
    else 
        is_unknow = DeepDarkFantasyData.Instance:CheckBossIsLock(data)        
        self.is_lock:SetActive(is_unlock or is_unknow)
    end
    if self.ObjStarts then
        self.ObjStarts:SetActive(not is_unlock)
    end
    self.inter.Interactable = not is_unknow
    if self.star_inters ~= nil then 
        for i = 1,self.star_inters:Length() do 
            self.star_inters[i].Interactable = DeepDarkFantasyData.Instance:CheckBossBoxPass({chapter = data.chapter,challenge_id = data.challenge_id,index = i})
        end 
    end 

    self:ActRedPoint()
end 

function DarkFantasyBossCell:ActRedPoint()
    -- self.red_point = self.red_point or UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
	self.red_point:SetNum(DeepDarkFantasyData.Instance:CheckBossIsRed(self.data) and 1 or 0)
    
    if self.prefect == nil then return end 
    self.prefect:SetActive(DeepDarkFantasyData.Instance:CheckBossIsCompleteMission(self.data))
end

function DarkFantasyBossCell:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
	if not self.is_care then return end
    self:uncareData()
end

-- 携带伙伴的cell
DeepDarkPartnerCell = DeepDarkPartnerCell or DeclareMono("DeepDarkPartnerCell", UIWidgetBaseItem)
function DeepDarkPartnerCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    if data.type == 1 then 
        local info = PartnerData.Instance:GetPartnerInfoById(self.data.icon)
        UH.SetIcon(self.Icon, info.info.icon_id, ICON_TYPE.ITEM)
    else 
        local icon = PetData.GetItemIconId(self.data.icon)
        UH.SetIcon(self.Icon, icon, ICON_TYPE.ITEM)
    end 
end 

DeepDarkFantasyChapterCell = DeepDarkFantasyChapterCell or DeclareMono("DeepDarkFantasyChapterCell", UIWidgetBaseItem)
function DeepDarkFantasyChapterCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    local flag_lock = DeepDarkFantasyData.Instance:GetChapterIsLock(data)
    UH.SpriteName(self.nomal_sp, flag_lock and "ddf_WeiJieSuo" or "ddf_ZhengChang")
    local param = DeepDarkFantasyData.Instance:GetChapterParam(data)
    self.nomal_info:SetData(param)
    self.select_info:SetData(param)
    self.nomal_lock:SetActive(flag_lock)

    self.inter.Interactable = not flag_lock

    if self.red_point == nil then
		self.red_point = UiRedPoint.New(self.red_pos)
	end

    self.red_point:SetNum(DeepDarkFantasyData.Instance:GetChapterRemind(data))
end 

-- function DeepDarkFantasyChapterCell:()
    
-- end

DeepDarkFantasyChapterInfo = DeepDarkFantasyChapterInfo or DeclareMono("DeepDarkFantasyChapterInfo", UIWidgetBaseItem)
function DeepDarkFantasyChapterInfo:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.chapter , string.format(Language.DeepDarkFantasy.ChapterNum,data.chapter))
    UH.SetText(self.cur_num , data.cur_num)
    UH.SetText(self.max_num , "/"..data.max_num)

    self.cur_num.gameObject:SetActive(not data.is_lock)
    self.max_num.gameObject:SetActive(not data.is_lock)
    self.star_1:SetActive(not data.is_lock)
    self.star_2:SetActive(not data.is_lock)
end 