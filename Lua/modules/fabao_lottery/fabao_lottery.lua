FabaoLottery = FabaoLottery or DeclareView("FabaoLottery", "fabao_lottery/fabao_lottery", Mod.Fabao.Lottery)
VIEW_DECLARE_LEVEL(FabaoLottery, ViewLevel.L0)
function FabaoLottery:FabaoLottery()
end

FabaoLotteryPanel = FabaoLotteryPanel or DeclareMono("FabaoLotteryPanel", UIWFlushPanel)
--TimeRedPs=====UIWSubstitute[]
--NextBtnTxt====Text
function FabaoLotteryPanel:FabaoLotteryPanel()
    self.lang = Language.Fabao
    self.data = FabaoLotteryData.Instance
    self.data_cares = {
        {data = RoleData.Instance:GetCurrencyData(), func = self.FlushCurrValue},
        {data = BagData.Instance.item_grid, func = self.FlushCurrValue},
        {data = ChallengeTestCtrl.Instance.data.info_sm, func = self.FlushLockStatus},
        {data = BagData.Instance.item_grid, func = self.FlushCostItem},
        {data = self.data:DrawInfo(), func = self.FlushTime},
        {data = self.data:RewardInfo(), func = self.PlayDrawEffect, init_call = false},
        {data = self.data:EffectInfo(), func = self.OnActViewClose, init_call = false,keys = {"close_act_flag"}},
        {data = self.data:EffectInfo(), func = self.FlushShowRward, init_call = false,keys = {"show_reward_eff"}},
        {data = self.data:ViewData(), func = self.OnVisualClickTrigger, init_call = false,keys = {"visual_click_trigger"}},
    }
    self.fog = false
    self.handle_delay = {}
end
function FabaoLotteryPanel:FlushShowRward()
    -- LogDG("刷新奖励")
    self.Mask:SetActive(true)
    -- self.EffectObj[1]:SetActive(true)
    -- local list = self.data:RewardInfo()
    self.count = 1
    self:PlayEffect()
end

local function checkFabaoByItemId(fabao_id,item_id)
    if fabao_id and fabao_id ~= -1 then
        local cfg = FabaoData.Instance:FabaoAttrInfo(fabao_id)
        if cfg and cfg.talisman_virtual_item == item_id then
            return true
        end
    end
    return false
end 
function FabaoLotteryPanel:PlayEffect()
    self.BtnNext:SetActive(false)
    self.RetName:SetActive(false)
    local list = self.data:RewardInfo()
    local item = list[self.count]
    if item == nil then
        self:DropAnimEnd()
        return
    end
    local color = item:Color() - 2
    color = color < 1 and 1 or color
    color = color > 3 and 3 or color
    self.color = color
    FabaoLotteryData.Instance:EffectInfo().next_display = not FabaoLotteryData.Instance:EffectInfo().next_display 
    FabaoLotteryData.Instance:EffectInfo().next_display_color = color
    
    UH.SetText(self.RetNameTxt,item:Name())
    
    local info = self.data:FabaoRewardInfo()
    if checkFabaoByItemId(info.fabao_id,item.item_id) then
        UH.SetIcon(self.EffectIcon[self.color+3], self.data:IconId(item.item_id))
        self.EffectObj[self.color+3]:SetActive(true)
        FabaoLotteryData.Instance:EffectInfo().show_visual_flag = not FabaoLotteryData.Instance:EffectInfo().show_visual_flag 
    else
        UH.SetIcon(self.EffectIcon[self.color], self.data:IconId(item.item_id))
        self.EffectObj[self.color]:SetActive(true)
    end
    
    TimeHelper:CancelTimer(self.handle_delay["drop_delay"])

    self.handle_delay["drop_delay"] =
        TimeHelper:AddDelayTimer(
        function()
            if checkFabaoByItemId(info.fabao_id,item.item_id) then
                if info.fabao_index and info.fabao_index == -1 then
                    ViewMgr:OpenView(FabaoActive, {id = info.fabao_id})
                end
            end
            self:ShowClickNext()
        end,
        3
    )
end
function FabaoLotteryPanel:DropAnimEnd()
    self.data:EffectInfo().stop = not self.data:EffectInfo().stop
    TimeHelper:CancelTimer(self.handle_delay["drop_delay"])
    self.EffectObj[self.color]:SetActive(false)
    self.EffectObj[self.color+3]:SetActive(false)
    self.Mask:SetActive(false)
    TimeHelper:CancelTimer(self.handle_delay["delay_show"])
    self.handle_delay["delay_show"] =
        TimeHelper:AddDelayTimer(
        function()
            self.Block:SetActive(false)
            self.TopObj:SetActive(true)
            self.DownObj:SetActive(true)
            self.StoveUiObj:SetActive(true)
            local list = self.data:RewardInfo()
            self.data:ShowDelayAttriChange()
            MainOtherCtrl.Instance:OpenGiftView(list:Val(), GIFT_OPEN_TYPE.CONGRATULATIONS, nil, "_LocChengGongHuoDe")
        end,
        1
    )
    self:CheckAchieve()
end
function FabaoLotteryPanel:ShowClickNext()
    TimeHelper:CancelTimer(self.handle_delay["drop_delay"])
    UH.SetText(self.NextBtnTxt,string.format(self.lang.NextBtnHint,self.count))
    self.BtnNext:SetActive(true)
    self.RetName:SetActive(true)
end
function FabaoLotteryPanel:ContinuePlay()
    self.EffectObj[self.color]:SetActive(false)
    self.EffectObj[self.color+3]:SetActive(false)
    self.count = self.count + 1
    self:PlayEffect()
end
function FabaoLotteryPanel:OnClickNext()
    TimeHelper:CancelTimer(self.handle_delay["drop_delay"])
    self:ContinuePlay()
end

function FabaoLotteryPanel:OnActViewClose()
    TimeHelper:CancelTimer(self.handle_delay["drop_delay"])
    self:ContinuePlay()
end

function FabaoLotteryPanel:OnClickSkip()
    self:DropAnimEnd()
end
function FabaoLotteryPanel:OnDestroy()
    UnityEngine.RenderSettings.fog = self.fog
    UnityEngine.QualitySettings.pixelLightCount = 1
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
    for k, v in pairs(self.handle_delay) do
        TimeHelper:CancelTimer(v)
    end
    self:CheckAchieve()
    UIWFlushPanel.OnDestroy(self)
end

-- 检查成就历程完成提醒
function FabaoLotteryPanel:CheckAchieve()
    if AchieveCtrl.Instance.fabao_finish_remind then
        ViewMgr:OpenView(AchieveCourseFinishRemind, AchieveCtrl.Instance.fabao_finish_remind)
        AchieveCtrl.Instance.fabao_finish_remind = nil
    end
end

function FabaoLotteryPanel:FlushCurrValue()
    local value = RoleData.Instance:GetCurrency(CurrencyType.Gold)
    UH.SetText(self.CurrValues[1], DataHelper.ConverMoney(value))
    value = BagData.Instance:GetNumByItemId(CommonItemId.OrangeStone)
    UH.SetText(self.CurrValues[2], value)
    value = BagData.Instance:GetNumByItemId(CommonItemId.PurpleStone)
    UH.SetText(self.CurrValues[3], value)
    value = BagData.Instance:GetNumByItemId(CommonItemId.BlueStone)
    UH.SetText(self.CurrValues[4], value)
end
function FabaoLotteryPanel:FlushLockStatus()
    self:FlushLockShow(1)
    self:FlushLockShow(2)
    self:FlushLockShow(3)
    self:FlushLockShow(4)
end
function FabaoLotteryPanel:FlushLockTip(index)
    UH.SetText(self.LockTip[index], string.format(self.lang.LockTip, self.data:LockLayer(index)))
end
function FabaoLotteryPanel:FlushLockShow(index)
    local lock = self.data:IsLock(index)
    self.LockObj[index]:SetActive(lock)
    self.LockTipObj[index]:SetActive(lock)
end
function FabaoLotteryPanel:FlushTime()
    self:FlushTimeShow(1)
    self:FlushTimeShow(2)
    self:FlushTimeShow(3)
    self:FlushTimeShow(4)
end
function FabaoLotteryPanel:FlushTimeShow(index)
    local lock = self.data:IsLock(index)
    self.TimeObj[index]:SetActive(not lock)
    local info = self.data:GetDrawInfo(index)
    local is_finish = self.data:IsFinish(index)
    if info and info.material_num > 0 then
        self.TimeObj[index]:SetActive(true)
        if is_finish then       --炼制完成
            self.TimeRedPs[index]:SetNum(1)
            self.TimeMeter[index]:CloseCountDownTime()
            self.TimeMeter[index]:SetTime(self.lang.FinishTime)
        else                    --炼制中
            self.TimeRedPs[index]:SetNum(0)
            self.TimeMeter[index]:StampTime(info.end_time - info.speed_up_time)
        end
    elseif info and info.material_num == 0 then     --空闲中
        self.TimeRedPs[index]:SetNum(self.data:RemindIsMaterialEnough())
        self.TimeMeter[index]:SetTime(self.lang.FreeTime)
    end
end
function FabaoLotteryPanel:FlushCostItem()
    local item_id = self.data:SpeedItem()
    local item = Item.Init(item_id, Item.GetNum(item_id))
    local num = item:Num()
    UH.SetIcon(self.CostIcon, item:IconId())
    UH.SetText(self.CostName, item:Name())
    UH.SetText(self.CostNum, num)
    UH.Color(self.CostNum, num == 0 and COLORS.Red8 or COLORS.White)
end
function FabaoLotteryPanel:onFlush()
    self.fog = UnityEngine.RenderSettings.fog
    UnityEngine.QualitySettings.pixelLightCount = 20
    UnityEngine.RenderSettings.fog = false
    if not self.ui_obj then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self
            }
        )
        self.ui_obj:SetData("actor/other/fabao.prefab")

        -- TimeHelper:AddDelayTimer(
        --     function()
        --         self.ModelShow.model_camera_mono.Camera.farClipPlane = 50
        --     end,
        --     0.5
        -- )

    -- far = 50

    -- self.ui_obj:PlayAnim(ChAnimType.WeaponShow)
    end
    self:FlushLockTip(2)
    self:FlushLockTip(3)
    self:FlushLockTip(4)
end

function FabaoLotteryPanel:OnClickRwardPreview()
    ViewMgr:OpenView(FabaoRate)
end

function FabaoLotteryPanel:OnClickProg()
    ViewMgr:OpenView(FabaoDetail)
end

function FabaoLotteryPanel:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[128].desc})
end

function FabaoLotteryPanel:OnVisualClickTrigger()
    self:OnClickLuZi(self.data:ViewData().visual_click_index)
end

function FabaoLotteryPanel:OnClickLuZi(index)
    --如果已经在练了就打开
    -- ViewMgr:OpenView(FabaoFiller)
    self.data:SetIndex(index)
    local draw = self.data:GetDrawInfo(index)
    local is_finish = self.data:IsFinish(index)
    local is_lock, lock_layer = self.data:IsLock(index)
    if is_lock then
        PublicPopupCtrl.Instance:Center(string.format(self.lang.LockTip2, lock_layer))
        return
    end
    if draw.material_num > 0 then
        if is_finish then
            -- FabaoLotteryCtrl.Draw(index - 1)
            FabaoLotteryCtrl.Draw(index - 1)
        else
            local item = Item.Init(self.data:SpeedItem(), Item.GetNum(item_id))
            if item:Num() < 1 then
                PublicPopupCtrl.Instance:Center(string.format(Language.Fabao.ZeroNum, item:Name()))
            else
                ViewMgr:OpenView(FabaoCost)
            end
        end
    else
        ViewMgr:OpenView(FabaoFiller)
    end
end

function FabaoLotteryPanel:PlayDrawEffect()
    --LogDG("已完成点击获取奖励")
    self.data:EffectInfo().play = not self.data:EffectInfo().play
    self.Block:SetActive(true)
    self.TopObj:SetActive(false)
    self.DownObj:SetActive(false)
    self.StoveUiObj:SetActive(false)
end

function FabaoLotteryPanel:OnClickCost()
    local item_id = self.data:SpeedItem()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = item_id})})
end
function FabaoLotteryPanel:OnClickClose()
    ViewMgr:CloseView(FabaoLottery)
end
function FabaoLotteryPanel:OnClickAdd(type)
    if type == CurrencyType.Gold then
        ViewMgr:OpenViewByKey(Mod.Store.Recharge)
    else
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[type]})})
    end
end

FabaoLotteryOpenAnim = FabaoLotteryOpenAnim or DeclareMono("FabaoLotteryOpenAnim", UIWFlushPanel)
--FireAnims=====Animation[]
function FabaoLotteryOpenAnim:FabaoLotteryOpenAnim()
    self.cur_show_circle = nil
    self.data = FabaoLotteryData.Instance
    self.data_cares = {
        -- {data = self.data:ViewData(), func = self.FlushTest, keys = {"select"}},
        -- {data = self.data:StoveInfo(), func = self.FlushTest, keys = {"select"}},
        {data = self.data:EffectInfo(), func = self.FlushPlay, keys = {"play"}, init_call = false},
        {data = self.data:EffectInfo(), func = self.ReInit, keys = {"stop"}, init_call = false},
        {data = self.data:EffectInfo(), func = self.ShowQualityCircle, keys = {"next_display"}, init_call = false},
        {data = self.data:EffectInfo(), func = self.ShowVisualQuality, keys = {"show_visual_flag"}, init_call = false},
        {data = self.data:DrawInfo(), func = self.OnDrawDataChange,init_call = false},

        -- {data = self.data:RewardInfo(), func = self.FlushTest, keys = {"play"}, init_call = false},
    }
    self.handle_delay = {}
    self.circle_anim_handles = {}
end

--[[ function FabaoLotteryOpenAnim:Awake()
    UIWFlushPanel.Awake(self)
    for i = 1, 4 do
        self:Init(i)
    end
end ]]

function FabaoLotteryOpenAnim:OnDrawDataChange()
    self:FlushStoveFire(1)
    self:FlushStoveFire(2)
    self:FlushStoveFire(3)
    self:FlushStoveFire(4)
end

function FabaoLotteryOpenAnim:FlushStoveFire(index)
    local lock = self.data:IsLock(index)
    if lock then
        return
    end

    local info = self.data:GetDrawInfo(index)
    local is_finish = self.data:IsFinish(index)
    local anim = self.FireAnims[index]
    if info and info.material_num > 0 then
        if is_finish then
            anim:SetObjActive(true)
            anim:Play("changjintexiao_2175013C")
        else
            anim:SetObjActive(true)
        end
    elseif info and info.material_num == 0 then
        anim:SetObjActive(false)
    end
end

function FabaoLotteryOpenAnim:OnEnable()
    UIWFlushPanel.OnEnable(self)
    for i = 1, 4 do
        self:Init(i)
    end
    self:OnDrawDataChange()
    for i=1, self.SpEffs:Length() do
        self.SpEffs[i]:SetActive(false)
    end
    for key, value in pairs(self.handle_delay) do
        TimeHelper:CancelTimer(value)
    end
end
function FabaoLotteryOpenAnim:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    for key, value in pairs(self.handle_delay) do
        TimeHelper:CancelTimer(value)
    end
end
function FabaoLotteryOpenAnim:FlushPlay()
    local info = self.data:StoveInfo()
    local index = info.index
    self.Anim[index]:Play("fabao_Scene_801_A1")
    self.Effect[index]:SetActive(true)
    self.EffectAnim[index]:Play("changjingtexiao_2175001_801")
    TimeHelper:CancelTimer(self.handle_delay["get_reward"])
    self.handle_delay["get_reward"] =
        TimeHelper:AddDelayTimer(
        function()
            ChatCtrl.Instance.stop = true
            MainOtherCtrl.Instance.stop = true
            PublicPopupCtrl.Instance.stop = true
            -- FabaoLotteryCtrl.Draw(index - 1)
            self.PlatformAnim:Play("animation_fabao_801")
            self.data:EffectInfo().show_reward_eff = not self.data:EffectInfo().show_reward_eff
        end,
        2.35
    )
end

function FabaoLotteryOpenAnim:ShowVisualQuality()
    local cir_col = FabaoLotteryData.Instance:EffectInfo().next_display_color
    self.SpEffs[cir_col]:SetActive(false)
    self.SpEffs[cir_col]:SetActive(true)
end


function FabaoLotteryOpenAnim:ShowQualityCircle()
    self:ClearCurShowCircle()
    local cir_col = FabaoLotteryData.Instance:EffectInfo().next_display_color
    local clone_obj = UnityGameObj.Clone(self.QualityCircle[cir_col].gameObject,self.QualityCircle[cir_col].transform.parent.gameObject)
    self.cur_show_circle = clone_obj:GetComponent(typeof(UnityAnim))
    self.cur_show_circle:SetObjActive(true)
    -- QualityCircle
end

function FabaoLotteryOpenAnim:Init(index)
    self.Anim[index]:Play("fabao_Scene_801_A1")
    local item = self.Anim[index]:get_Item("fabao_Scene_801_A1")
    item.time = 0
    self.Anim[index]:Sample()
    self.Anim[index]:Stop()
end

function FabaoLotteryOpenAnim:ClearCurShowCircle()
    if  self.cur_show_circle ~= nil then
        self.cur_show_circle:SetTrigger(APH("end"))
        local anim_cache = self.cur_show_circle
        self.circle_anim_handles[anim_cache] = anim_cache:ListenEvent(
                "end",
                function(key, anim_st_info)
                    if not anim_cache:Equals(nil) then
                        UnityGameObj.Destroy(anim_cache.gameObject)
                        anim_cache:UnlistenEvent("end",self.circle_anim_handles[anim_cache])
                        self.circle_anim_handles[anim_cache] = nil
                    end
                end
            )
        self.cur_show_circle = nil
    end
end

function FabaoLotteryOpenAnim:OnDestroy()
    for anim,hand in pairs(self.circle_anim_handles) do
        anim:UnlistenEvent("end",hand)
    end
    self.circle_anim_handles = {}
    UIWFlushPanel.OnDestroy(self)
end

function FabaoLotteryOpenAnim:ReInit()
    self:ClearCurShowCircle()  
    ChatCtrl.Instance.stop = false
    MainOtherCtrl.Instance.stop = false
    PublicPopupCtrl.Instance.stop = false
    local info = self.data:StoveInfo()
    local index = info.index
    self.Anim[index]:Play("fabao_Scene_802_A1")
    self.Effect[index]:SetActive(true)
    self.EffectAnim[index]:Play("changjingtexiao_2175001_802")
    self.PlatformAnim:Play("animation_fabao_802")
    TimeHelper:AddDelayTimer(
        function()
            self.Effect[index]:SetActive(false)
        end,
        1
    )
end

FabaoActive = FabaoActive or DeclareView("FabaoActive", "fabao_lottery/fabao_active")
--Desc==========Text
--Icon==========Image
--BeiDongObj====GameObject
--ZhuDongObj====GameObject
--Attrs=========Text[]
--Values========Text[]
--Name==========Text
VIEW_DECLARE_LEVEL(FabaoActive, ViewLevel.L0)
VIEW_DECLARE_MASK(FabaoActive, ViewMask.Block)
function FabaoActive:FabaoActive()
    self.data = FabaoData.Instance
end
function FabaoActive:LoadCallback(param_t)
    local id = param_t.id
    local upgrade = self.data:UpgradeCfg(id, 1)[1]
    local data = self.data:FabaoAttrInfo(id)
    local advance = self.data:AdvanceCfg(id, 0)[1]
    local is_percent = advance.percentage_show == 1
    local cur = is_percent and ((advance.advanced_addition / 1000) .. "%") or advance.advanced_addition / 1000
    local cur2 = is_percent and ((advance.advanced_addition2 / 1000) .. "%") or advance.advanced_addition2 / 1000
    for i = 1, 2 do
        local attr_type = upgrade["attr_type_" .. i]
        local value = upgrade["attr_num_" .. i]
        if value > 0 then
            UH.SetText(self.Attrs[i], Language.AttrItems.AttrNames[attr_type])
            UH.SetText(self.Values[i], "+" .. tostring(DataHelper.Percent(attr_type, value)))
        else
            UH.SetText(self.Attrs[i], "")
            UH.SetText(self.Values[i], "")
        end
    end
    UH.SetIcon(self.Icon, data.big_icon)
    UH.SetText(self.Desc, string.format(data.desc_1, cur, "", cur2, ""))
    if data.type == 1 then
        self.ZhuDongObj:SetActive(true)
    else
        self.BeiDongObj:SetActive(true)
    end
    UH.SetText(self.Name,data.name)
end
function FabaoActive:CloseCallback()
    local ei = FabaoLotteryData.Instance:EffectInfo();
    ei.close_act_flag = not ei.close_act_flag
    FabaoLotteryData.Instance:ClearReward()
end


function FabaoActive:OnCloseClick()
    ViewMgr:CloseView(FabaoActive)
end

function FabaoActive:OnEffectBoom()
    self.Icon.enabled = true
end