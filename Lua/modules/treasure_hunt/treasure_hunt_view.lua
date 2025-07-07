TreasureHuntView = TreasureHuntView or DeclareView("TreasureHuntView","treasure_hunt/treasure_hunt",Mod.TreasureHunt.Main)
VIEW_DECLARE_MASK(TreasureHuntView,ViewMask.Block)
VIEW_DECLARE_LEVEL(TreasureHuntView, ViewLevel.L0)
function TreasureHuntView:LoadCallback(param)
    if param then
        if param.auto_select then
            TreasureHuntData.Instance.auto_select = param.auto_select
        end
        if param.auto_xunqi_type then
            TreasureHuntData.Instance.auto_xunqi_type = param.auto_xunqi_type
        end
    end
    if TreasureHuntData.Instance:GetXunQiIsOpen() then
        local tab_index = param.open_param or 1
        self.Tabbar:SetData(
            {
                {name = Language.TreasureHunt.Tab1, tab_objs = {self.HuanLingBg, self.HuanLing, self.HuanLingTable}, mod = TabMod(Mod.TreasureHunt.Main), open = Mod.TreasureHunt.Main},
                {name = Language.TreasureHunt.Tab2, tab_objs = {self.XunQiBg, self.XunQi},mod = TabMod(Mod.TreasureHunt.XunQi), open = Mod.TreasureHunt.XunQi},
            }
        )
        self.Tabbar:ClickItem(tab_index)
    else
        self.HuanLingBg:SetActive(true)
        self.HuanLing:SetActive(true)
        self.HuanLingTable:SetActive(true)
        self.Tabbar:SetObjActive(false)
    end
    AudioMgr:PlayEffect(AudioType.UI,"treasure_hunt_open")
    self.data_instance = TreasureHuntData.Instance
    self.one_item = TreasureHuntData.Instance.other_info.once_item_id
    self.five_item = TreasureHuntData.Instance.other_info.five_item_id

    -- 寻宝能抽到皮肤，需要先临时存放皮肤是否激活
    PetData.Instance:SetTempPetHasSkinList()
end

function TreasureHuntView:TreasureHuntView()
    self.start_anim:SetTrigger("play")
end

function TreasureHuntView:OnClickRatePreview()
    if self.data_instance.exchange_request.sended_req == 2 then
        return
    end
    ViewMgr:OpenView(TreasureHuntRateView)
end

function TreasureHuntView:OnClickPetExchange()
    if self.data_instance.exchange_request.sended_req == 2 then
        return
    end
    ViewMgr:OpenView(PetHuntExchange)
end

function TreasureHuntView:OnClickReturnBtn()
    if self.data_instance.exchange_request.sended_req ~= 2 then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
        ViewMgr:CloseView(TreasureHuntView)
    end
end

function TreasureHuntView:OnePrizeDraw()
    if self.data_instance.exchange_request.sended_req == 2 then
        return
    end
    -- if BattleData.Instance:IsServerFighting() then
    --     PublicPopupCtrl.Instance:Center(Language.TreasureHunt.CannotDoInBattle)
    --     return false
    -- end
    local treasure_hunt_cfg = TreasureHuntData.Instance:GetCurSelectCfg()
    self.one_item = treasure_hunt_cfg.once_item_id

    local next_free_time = self.data_instance:GetFreeTimeStamp(self.data_instance.smart_data.cur_treasure_id, treasure_hunt_cfg.treasure_type) or 0
    local use_free = next_free_time ~= 0 and next_free_time < TimeCtrl.Instance:GetServerTime()

    local function judgeAndSend(item)
        if not self:JudgePetIsFull() then
            local function sendReq()
                AudioMgr:PlayEffect(AudioType.UI,"treasure_hunt_start")
                TreasureHuntData.Instance.smart_data.sended_req = true
                local next_free_time = self.data_instance:GetFreeTimeStamp(self.data_instance.smart_data.cur_treasure_id, treasure_hunt_cfg.treasure_type) or 0
                local use_free = next_free_time ~= 0 and next_free_time < TimeCtrl.Instance:GetServerTime()
                TreasureHuntCtrl.Instance:onCSTreasureHuntReq(1, self.data_instance.smart_data.cur_treasure_id, false,use_free and {} or {[1] = {col = ItemColumnType.Item, idx = item and item:GridIndex() or 0, num = item and 1 or 0}}, use_free)
                ViewMgr:CloseView(DialogTipsView)
            end
            if treasure_hunt_cfg.treasure_type ~= HUNT_TYPE.XUN_QI and treasure_hunt_cfg.treasure_id ~= 5 and not self:CheckPrayFull() then
                self:OpenPrayTip(sendReq)
            else
                sendReq()
            end
        end
    end

    local item = BagData.Instance:IsHasItemById(self.one_item)
    if item ~= nil then
        judgeAndSend(item)
    elseif use_free then
        judgeAndSend()
    else
        -- local param = {item_id = self.one_item, treasure_id = treasure_hunt_cfg.treasure_id, draw_times = 1}
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.one_item})})
        -- ViewMgr:OpenView(TreasureHuntQuickBuy,param)
    end
end

function TreasureHuntView:JudgePetIsFull()
    if HuntNotPet[self.data_instance.smart_data.cur_treasure_id]
        or TreasureHuntData.Instance:GetCurSelectCfg().treasure_type == HUNT_TYPE.XUN_QI then --抽奖类型为非宠物时
        return false
    end
    local pet_list = PetData.Instance:GetPetList()
    local max = PetData.Instance:GetPetMaxBag()
    if table.nums(pet_list) + TreasureHuntData.Instance.treasure_hunt_count > max then
        PublicPopupCtrl.Instance:Center(ErrorInfo[152])
        return true
    end
    return false
end

function TreasureHuntView:FivePrizeDraw()
    if self.data_instance.exchange_request.sended_req == 2 then
        return
    end
    -- if BattleData.Instance:IsServerFighting() then
    --     PublicPopupCtrl.Instance:Center(Language.TreasureHunt.CannotDoInBattle)
    --     return false
    -- end
    local treasure_hunt_cfg = TreasureHuntData.Instance:GetCurSelectCfg()
    self.five_item = treasure_hunt_cfg.five_item_id

    local item = BagData.Instance:IsHasItemById(self.five_item)
    local item_list = {item}
    -- -1代表有5连钥匙
    local item_num = item ~= nil and -1 or 0
    if item_num == 0 then
        item_num, item_list = BagData.Instance:GetAllItemsByItemId(treasure_hunt_cfg.once_item_id)
    end

    if item_num == -1 or item_num >= 5 then
        if not self:JudgePetIsFull() then
            local comsume_info = {}
            if item_num == -1 then
                comsume_info = {[1] = {col = ItemColumnType.Item, idx = item:GridIndex(), num = 1}}
            else
                local need_num = 5
                for k,v in pairs(item_list) do
                    local t = {}
                    t.col = ItemColumnType.Item
                    t.idx = v:GridIndex()
                    if need_num - v.num > 0 then
                        need_num = need_num - v.num
                        t.num = v.num
                        table.insert(comsume_info, t)
                    else
                        t.num = need_num
                        table.insert(comsume_info, t)
                        break
                    end
                end
            end
            function sendReq()
                AudioMgr:PlayEffect(AudioType.UI,"treasure_hunt_start")
                TreasureHuntCtrl.Instance:onCSTreasureHuntReq(5, self.data_instance.smart_data.cur_treasure_id, false, comsume_info)
                PublicPopupCtrl.Instance:CloseDialogTips()
            end
            if treasure_hunt_cfg.treasure_type ~= HUNT_TYPE.XUN_QI and not HuntNotPet[treasure_hunt_cfg.treasure_id] and not self:CheckPrayFull() then
                self:OpenPrayTip(sendReq)
            else
                sendReq()
            end
        end
    else
        -- local param = {item_id = self.five_item, treasure_id = treasure_hunt_cfg.treasure_id, draw_times = 5}
        -- ViewMgr:OpenView(TreasureHuntQuickBuy,param)
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.five_item})})
    end
end

function TreasureHuntView:CheckPrayFull()
    if RoleLevel() < TreasureHuntData.Instance.other_info.up_show_level then
        return true
    end
    local pray_count = 0
    for k,v in pairs(TreasureHuntData.Instance.pets_bless_info.xun_bao_bless_pet_id_list) do
        if v > 0 then
            pray_count = pray_count + 1
            if pray_count >= 2 then
                return true
            end
        end
    end
    return false
end

function TreasureHuntView:OpenPrayTip(confirm_fun)
    local tip_t = {
        content = Language.TreasureHunt.PrayDialogTipContent,
        confirm = {
            func = confirm_fun
        },
        tnr = DTTodayNotRemind.TreasureHuntPray
    }
    PublicPopupCtrl.Instance:DialogTips(tip_t)
end
----------------------------------------------------
TreasureHuntCommonFlush = TreasureHuntCommonFlush or DeclareMono("TreasureHuntCommonFlush", UIWFlushPanel)
function TreasureHuntCommonFlush:TreasureHuntCommonFlush()
    self.data_instance = TreasureHuntData.Instance
    self.data_cares = {
        {data = self.data_instance.smart_data, func = self.FlushAnim, keys = {"is_get"},init_call = false},
        {data = self.data_instance.smart_data, func = self.FlushCommonInfo, keys = {"cur_treasure_id"}},
        {data = self.data_instance.smart_data, func = self.FlushFreeInfo, keys = {"FreeRemind", init_call = false}},
        {data = BagData.Instance.item_grid[ItemColumnType.Item], func = self.FlushCommonInfo, init_call = false},
    }
end

function TreasureHuntCommonFlush:FlushCommonInfo()
    local treasure_hunt_cfg = self.data_instance:GetCurSelectCfg()

    local once_stuff = treasure_hunt_cfg.once_item_id
    local quintic_stuff = treasure_hunt_cfg.five_item_id
    UH.SpriteName(self.OnceImage, Item.GetConfig(once_stuff).icon_id)
    UH.SpriteName(self.QuinticImage, Item.GetConfig(quintic_stuff).icon_id)
    if treasure_hunt_cfg.is_limit == 0 and (Item.GetNum(once_stuff) >= 5 or Item.GetNum(quintic_stuff) >= 1)then
        if self.five_red_point == nil then
            self.five_red_point = UiRedPoint.New(self.Btn5, Vector3.New(128, 39))
        end
        self.five_red_point:SetNum(1)
    elseif self.five_red_point ~= nil then
        self.five_red_point:SetNum(0)
    end
    self.PetsName:SetActive(treasure_hunt_cfg.treasure_id == 6)
    self.PetAnim:SetActive(treasure_hunt_cfg.treasure_id == 6)
    self.PartnersName:SetActive(treasure_hunt_cfg.treasure_id == 5)
    self.PartnerAnim:SetActive(treasure_hunt_cfg.treasure_id == 5)
    if treasure_hunt_cfg.treasure_id == 5 or treasure_hunt_cfg.treasure_id == 6 then
        if self.nor_hunt_eff == nil then
            self.nor_hunt_eff = self.EffectTool:Play(3166001)
            self.EffectTool:Play(3166002)
        end
    end
    self.TableTreeObj:SetActive(treasure_hunt_cfg.treasure_id ~= 5 and treasure_hunt_cfg.treasure_id ~= 6)

    self:FlushFreeInfo()
end

function TreasureHuntCommonFlush:FlushAnim()
    if not ViewMgr:IsOpen(TreasureHuntDetailView) then
        ViewMgr:OpenView(TreasureHuntDetailView)
    end
end

function TreasureHuntCommonFlush:FlushFreeInfo()
    local treasure_hunt_cfg = self.data_instance:GetCurSelectCfg()
    local next_free_time = self.data_instance:GetFreeTimeStamp(self.data_instance.smart_data.cur_treasure_id, treasure_hunt_cfg.treasure_type) or 0
    
    if self.red_point then
        self.red_point:SetNum(0)
    end
    -- LogError("FlushFreeInfo next_free_time",next_free_time,"cur_treasure_id",self.data_instance.smart_data.cur_treasure_id)
    if next_free_time ~= 0 and next_free_time <= TimeCtrl.Instance:GetServerTime() then
        UH.SpriteName(self.OnceImg, "_LocOnceFree")
    else
        UH.SpriteName(self.OnceImg, "_LocOnce")
    end
    self.FreeTimeMeter:SetCallBack(function ()
        if treasure_hunt_cfg.time_treasure_free and treasure_hunt_cfg.time_treasure_free > 0 then
            UH.SpriteName(self.OnceImg, "_LocOnceFree")
            if self.red_point == nil then
                self.red_point = UiRedPoint.New(self.Btn1, Vector3.New(128, 39))
            end
            self.red_point:SetNum(1)
        else
            UH.SpriteName(self.OnceImg, "_LocOnce")
        end
    end)
    self.FreeTimeMeter:SetShowCallBack(function (t)
        if t.hour == 0 then
            return string.format(Language.TreasureHunt.FreeTimeMeterMin, t.min, t.s)
        else
            return string.format(Language.TreasureHunt.FreeTimeMeter, t.hour, t.min)
        end
    end)
    
    if self.red_point then
        self.red_point:SetNum(next_free_time ~= 0 and next_free_time <= TimeCtrl.Instance:GetServerTime() and 1 or 0)
    end
    self.FreeTimeMeter:StampTime(next_free_time, nil, nil, "")
end

function TreasureHuntCommonFlush:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.red_point = nil
end
-----------------------------------------------------
TreasureHuntImageBg = TreasureHuntImageBg or DeclareMono("TreasureHuntImageBg",UIWFlushPanel)
function TreasureHuntImageBg:TreasureHuntImageBg()
    self.data_instance = TreasureHuntData.Instance
    self.data_cares = {
        {data = self.data_instance.smart_data, func = self.FlushBgImg, keys = {"cur_treasure_id","hunt_type"}},
    }
    self.is_load = false
end

function TreasureHuntImageBg:FlushBgImg()
    local bg_path = string.format("ui/textures/treasure_hunt/treasure_hunt_bg%s.png", self.data_instance.smart_data.cur_treasure_id)
    local cur_config = self.data_instance:GetCurSelectCfg()
    if self.data_instance:IsHunQi(self.data_instance.smart_data.cur_treasure_id) then
        bg_path = "ui/textures/treasure_hunt/treasure_hunt_bg6.png"
    elseif self.data_instance:IsShengQi(self.data_instance.smart_data.cur_treasure_id) then
        bg_path = "ui/textures/treasure_hunt/treasure_hunt_bg7.png"
    end
    ResMgr:LoadObject(bg_path,function(tex)
        if not self:Destroyed() then
            if self.BigBgRawImg then
                self.BigBgRawImg.texture = tex
                self.is_load = true
            end
        end
    end,false)
end

function TreasureHuntImageBg:IsLoaded()
    return self.is_load
end
-----------------------------------------------------
TreasureHuntFlush = TreasureHuntFlush or DeclareMono("TreasureHuntFlush",UIWFlushPanel)
-- animator = Animator
-- Btn5 = GameObject
-- Btn1 = GameObject
-- ScrollRect = ScrollRect
-- ToggleList = UIWidgetList
function TreasureHuntFlush:TreasureHuntFlush()
    self.one_item = TreasureHuntData.Instance.other_info.once_item_id
    self.five_item = TreasureHuntData.Instance.other_info.five_item_id

    self.data_cares = {
        {data = TreasureHuntData.Instance.smart_data, func = self.FlushImg, keys = {"cur_treasure_id","delay_flush"}},
        {data = TreasureHuntData.Instance.exchange_request, func = self.PlayExchangePetEffect,keys = {"sended_req"}, init_call = false},
        {data = BagData.Instance.item_grid[ItemColumnType.Item], func = self.CheckAuto, init_call = false}
    }

    self.data_instance = TreasureHuntData.Instance
    self.effect_show_in_played = {}
    GuideManager.Instance:RegisterGetGuideUi("TreasureHuntPetHunt",function ()
        local temp_index = 1
        local mono
        for k,v in pairs(self.ToggleList.item_list) do
            --普通宠物召唤
            if v:GetData().treasure_id == 6 then
                self.ToggleList:JumpVerticalIndex(temp_index,150, 35)
                mono = v
                break
            end
            temp_index = temp_index + 1
        end
        if not mono then
            return nil, nil
        end
        self.ToggleList:ClickItem(temp_index)
        return mono
    end)
    GuideManager.Instance:RegisterGetGuideUi("TreasureHuntPartnerHunt",function ()
        local temp_index = 1
        local mono
        for k,v in pairs(self.ToggleList.item_list) do
            --伙伴召唤
            if v:GetData().treasure_id == 5 then
                -- self.ToggleList:JumpVerticalIndex(temp_index,120,20)
                mono = v
                break
            end
            temp_index = temp_index + 1
        end
        if not mono then
            return nil, nil
        end
        self.ToggleList:ClickItem(temp_index)
        return mono
    end)
    GuideManager.Instance:RegisterGetGuideUi("OnceButtonObj",function ()
        return self.Btn1, function ()
            local item_id = self.cur_treasure_id == 6 and GuideData.Instance:GetTreasureHuntPetHuntItem() or GuideData.Instance:GetTreasureHuntPartnerItem(1)
            local item = BagData.Instance:IsHasItemById(item_id)
            if item then
                self.data_instance.guide_id = GuideManager.Instance:Guide().id
                TreasureHuntCtrl.Instance:onCSTreasureHuntReq(1, self.cur_treasure_id, self.cur_treasure_id == 6 and 1 or 3, {[1] = {col = ItemColumnType.Item, idx = item.index, num = 1}})
            end
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("OnceButtonNormal",function ()
        return self.Btn1, function ()
            local item_id = GuideData.Instance:GetTreasureHuntPetHuntItem()
            local item = BagData.Instance:IsHasItemById(item_id)
            if item then
                TreasureHuntCtrl.Instance:onCSTreasureHuntReq(1, self.cur_treasure_id, 2, {[1] = {col = ItemColumnType.Item, idx = item.index, num = 1}})
            end
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("FiveButtonObj",function ()
        return self.Btn5, function ()
            local item_id = GuideData.Instance:GetTreasureHuntPartnerItem(5)
            local item = BagData.Instance:IsHasItemById(item_id)
            if item then
                self.data_instance.guide_id = GuideManager.Instance:Guide().id
                TreasureHuntCtrl.Instance:onCSTreasureHuntReq(5, self.cur_treasure_id, 4, {[1] = {col = ItemColumnType.Item, idx = item.index, num = 1}})
            end
        end
    end)
end

function TreasureHuntFlush:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if self.cur_treasure_id ~= nil then
        self.data_instance:SetCurSelectId(self.cur_treasure_id)
    end
    self.data_instance.smart_data.hunt_type = HUNT_TYPE.HUAN_LING
end

function TreasureHuntFlush:onFlush()
    local list = self.data_instance:GetOpenedTreasureID()
    local show_key_list = {}
    for k,_ in pairs(list) do
        table.insert(show_key_list,k)
    end
    table.sort( show_key_list, function (a,b)
        return a<b
    end )
    local show_list = {}
    for _,sk in pairs(show_key_list) do
        local v = list[sk]
        if TreasureHuntData.Instance:GetTreasureHuntConfig(v.treasure_id).treasure_type ~= HUNT_TYPE.XUN_QI then
            table.insert(show_list,v)
        end
    end
    self.ToggleList:SetData(show_list)
    if GuideManager.Instance:IsGuide() then
        self.data_instance.sort_id = 5
        self.delay_jump = TimeHelper:AddDelayFrameTimer(function ()
            local temp_index = 1
            for k,v in pairs(self.ToggleList.item_list) do
                if v:GetData().treasure_id == 6 then
                    -- self.ToggleList:JumpVerticalIndex(temp_index,150,20)
                    break
                end
                temp_index = temp_index + 1
            end
        end,3)
    end
    local auto_select = self.data_instance:GetAutoSelectHuanLing(show_list)
    self.ToggleList:ClickItem(auto_select)--(self.data_instance.sort_id)
    local count = 0
    for k,v in pairs(show_list) do
        count = count + 1
    end
    self.ScrollRect.enabled = count >= 4
end

function TreasureHuntFlush:CheckAuto()
    if self.data_instance.smart_data.auto_use then
        if not ViewMgr:IsOpen(TreasureHuntDetailView) then
            TreasureHuntCtrl.Instance:AutoTreasureHuntReq()
        else
            TreasureHuntData.Instance.smart_data.detail_auto = true
        end
        self.data_instance.smart_data.auto_use = false
    end
end

function TreasureHuntFlush:FlushImg()
    if self.data_instance.smart_data.cur_treasure_id <= 0 and self.data_instance.smart_data.cur_treasure_id > TreasureHuntData.MaxHuntTypeNum then
        return
    end
    self.old_index = self.data_instance.smart_data.cur_treasure_id
    local treasure_hunt_cfg = self.data_instance:GetCurSelectCfg()
    if treasure_hunt_cfg.treasure_type == HUNT_TYPE.XUN_QI then
        return
    end
    if treasure_hunt_cfg.get_id ~= 0 and RoleData.Instance:GetRoleLevel() >= treasure_hunt_cfg.displace_level then
        local text_index = treasure_hunt_cfg.treasure_id == 6 and 1 or 2
        UH.SetText(self.PetExchangeBtnText, Language.TreasureHunt.PetHuntExchangeBtnText[text_index])
        self.PetExchangeBtnObj:SetActive(true)
        -- self.PetsPrayObj:SetActive(true)
    else
        self.PetExchangeBtnObj:SetActive(false)
        -- self.PetsPrayObj:SetActive(false)
    end
    local petpray_open = RoleLevel() >= TreasureHuntData.Instance.other_info.up_show_level
    self.PetsPrayObj:SetActive(self.old_index ~= 5 and petpray_open)
    if petpray_open and not UnityPlayerPrefs.HasKey(PrefKeys.TreasureHuntPetPrayEff()) then
        self.EffectTool:Play(6165167, self.PrayEffPar1)
        self.EffectTool:Play(6165167, self.PrayEffPar2)
        PrefsInt(PrefKeys.TreasureHuntPetPrayEff(), 1)
    end

    self.is_limit = treasure_hunt_cfg.is_limit == 1
    UH.SpriteName(self.BosImg, self.is_limit and "Box1" or "Box2")
    self.EffectObj1:SetActive(not self.is_limit)
    self.EffectObj2:SetActive(self.is_limit)
    local flag = self.data_instance:GetFirstDrawFlag(self.old_index)
    local second_flag = self.data_instance:GetSecondDrawFlag(self.old_index)
    local notice_str = ""
    --普通宠物寻宝
    if self.old_index == 6 then
        if flag then
            if treasure_hunt_cfg.once_floors > 0 and not second_flag then
                if self.data_instance:GetGroupDrawTimes() ~= nil then
                    notice_str = string.format(Language.TreasureHunt.PetHuntRemain,treasure_hunt_cfg.once_floors - self.data_instance:GetGroupDrawTimes())
                end
            else
                notice_str = string.format(Language.TreasureHunt.PetHuntRemain, self.data_instance:GetLeftTimes())
            end
        else
            notice_str = Language.TreasureHunt.PetHuntFirst
        end

    --无特殊次数保底奖励或者已经拿到最多一次特殊次数保底奖励的
    elseif treasure_hunt_cfg.once_floors <= 0 or second_flag then
        if treasure_hunt_cfg.treasure_id == 5 then
            notice_str = string.format(Language.TreasureHunt.PartnerRemain, self.data_instance:GetLeftTimes())
        else
            local format_s = Language.TreasureHunt.PetHuntRemainSpecialOrGold
            notice_str = string.format(format_s, self.data_instance:GetLeftTimes())
        end
        --LogError("1 notice_str ", self.data_instance:GetLeftTimes(), self.data_instance:GetGroupDrawTimes())

    --有一次特殊次数保底且还未获得保底奖励的
    else
        local format_s = treasure_hunt_cfg.treasure_id ~= 5 and Language.TreasureHunt.PetHuntRemainSpecialOrGold or Language.TreasureHunt.PartnerRemain
        notice_str = string.format(format_s, treasure_hunt_cfg.once_floors - (self.data_instance:GetGroupDrawTimes() or 0))
        --LogError("2 notice_str ",self.data_instance:GetGroupDrawTimes())
    end

    UH.SetText(self.PetNoticeText, notice_str)
    -- self:FlushFreeInfo()
end


function TreasureHuntFlush:OnDestroy()
    self.data_instance:ResetCurSelectId()
    self.cur_mono = nil
    if self.delay_jump ~= nil then
        TimeHelper:CancelTimer(self.delay_jump)
        self.delay_jump = nil
    end
    if self.delay_flush_img ~= nil then
        TimeHelper:CancelTimer(self.delay_flush_img)
        self.delay_flush_img = nil
    end
    if self.sended_req_delay ~= nil then
        TimeHelper:CancelTimer(self.sended_req_delay)
        self.sended_req_delay = nil
    end
    TreasureHuntData.Instance.smart_data.is_guide_draw = false
    TreasureHuntData.Instance.exchange_request.sended_req = 0
    UIWFlushPanel.OnDestroy(self)
end

function TreasureHuntFlush:PlayExchangePetEffect(sended_req)
    if self.data_instance.exchange_request.sended_req ~= 1 and self.data_instance.exchange_request.sended_req ~= 2 then
        return
    end
    if self.data_instance.exchange_request.sended_req == 2 then
        self.ExchangeEffObj:SetActive(true)
        self.sended_req_delay = TimeHelper:AddDelayTimer(function ()
            self.ExchangeEffObj:SetActive(false)
            self.data_instance.exchange_request.sended_req = 0
            self.data_instance:ResetExchangeSmart()
            ViewMgr:OpenView(PetGain, {is_xilian = false})
        end,3)
    elseif self.data_instance.exchange_request.sended_req == 1 then
        local pet_info_r = PetData.Instance:GetPetInfoByType(self.data_instance.exchange_smart.comsume_type2, self.data_instance.exchange_smart.comsume_type_index2)
        local pet_info_l = PetData.Instance:GetPetInfoByType(self.data_instance.exchange_smart.comsume_type1, self.data_instance.exchange_smart.comsume_type_index1)
        UH.SetIcon(self.PetIconR, pet_info_r:IconId())
        UH.SetIcon(self.PetIconL, pet_info_l:IconId())
        TreasureHuntCtrl.Instance:SendCSXunBaoExchange()
    end
end

function TreasureHuntFlush:OnClickToggle(mono)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    if TreasureHuntData.Instance.smart_data.sended_req ~= true and TreasureHuntData.Instance.exchange_request.sended_req ~= 1 and TreasureHuntData.Instance.exchange_request.sended_req ~= 2 then
        self.data_instance:SetCurSelectId(mono:GetData().treasure_id)
        self.cur_treasure_id = mono:GetData().treasure_id
        self.cur_mono = mono
    elseif self.cur_mono ~= nil then
        self.cur_mono.Toggle.isOn = true
    end
end
---------------------------------
TreasureHuntPetsPrayPanel = TreasureHuntPetsPrayPanel or DeclareMono("TreasureHuntPetsPrayPanel",UIWFlushPanel)
function TreasureHuntPetsPrayPanel:TreasureHuntPetsPrayPanel()
    self.data_cares = {
        {data = TreasureHuntData.Instance.pets_bless_info, func = self.OnEnable, keys = {"xun_bao_bless_pet_id_list"},init_call = false},
    }
end

function TreasureHuntPetsPrayPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    local pet_id_list = TreasureHuntData.Instance:GetBlessInfo()
    for i=1,self.PetItemList:Length() do
        self.PetItemList[i]:SetData(pet_id_list[i + 1], i)
    end
end
---------------------------------
TreasureHuntPetPrayItem = TreasureHuntPetPrayItem or DeclareMono("TreasureHuntPetPrayItem", UIWidgetBaseItem)
function TreasureHuntPetPrayItem:TreasureHuntPetPrayItem()
end

function TreasureHuntPetPrayItem:SetData(data)
    if data ~= nil and data ~= 0 then
        local pet_cfg = PetData.Instance:GetPetCfg(data)
        UH.SetIcon(self.Icon, pet_cfg.icon_id)
        self.AddImgObj:SetActive(false)
    else
        UH.SetIcon(self.Icon)
        self.AddImgObj:SetActive(true)
    end
end

function TreasureHuntPetPrayItem:OnClickPet(param)
    -- LogError("OnClickPet",param,TreasureHuntData.Instance.exchange_request.sended_req)
    if TreasureHuntData.Instance.exchange_request.sended_req == 2 then
        return
    end
    TreasureHuntData.Instance.pets_bless_info.cur_pray_index = param
    TreasureHuntData.Instance.pets_bless_info.cur_pray_id = TreasureHuntData.Instance:GetBlessInfo()[param + 1]
    ViewMgr:OpenView(TreasureHuntPetPraySelect, param)
end
---------------------------------
TreasureHuntToggleItem = TreasureHuntToggleItem or DeclareMono("TreasureHuntToggleItem", UIWidgetBaseItem)
function TreasureHuntToggleItem:TreasureHuntToggleItem()
    self.data_instance = TreasureHuntData.Instance
end

function TreasureHuntToggleItem:SetData(data)
    --LogError("TreasureHuntToggleItem",data.treasure_id)
    UH.SpriteName(self.Img, "TogImg"..data.treasure_id)
    UH.SpriteName(self.Img2, "TogImg"..data.treasure_id)
    -- UH.SpriteName(self.SelectImg, "SelectFrame"..data.treasure_id)
    UH.SpriteName(self.TogNameImg, "_LocTogName"..data.treasure_id)
    UH.SpriteName(self.TogNameImg2, "_LocTogName"..data.treasure_id)
    local treasure_cfg = self.data_instance.treasure_id_config[data.treasure_id]
    local left_time = 0
    local left_time_str = ""
    if treasure_cfg.open_type == 1 then
        left_time = data.end_time - TimeHelper.GetServerTime()
    elseif treasure_cfg.open_type == 2 then
        left_time = data.left_time
    end
    local day = left_time / 86400
    local hour = left_time / 3600
    local min = left_time / 60
    if day < 1 then
        if hour < 1 then
            left_time_str = math.floor(min)..Language.Timer.Name.min
        else
            left_time_str = math.floor(hour)..Language.Timer.Name.hour
        end
    else
        left_time_str = math.floor(day)..Language.Timer.Name.day
    end
    self.SelectXiYou:SetActive(treasure_cfg.is_limit == 1)
    self.NormalXiYou:SetActive(treasure_cfg.is_limit == 1)
    if data.treasure_id ~= 5 and data.treasure_id ~= 6 then
        left_time_str = string.format(treasure_cfg.time_name, left_time_str)
    else
        left_time_str = ""
    end
    self.TextBg1:SetActive(data.treasure_id ~= 5 and data.treasure_id ~= 6)
    self.TextBg2:SetActive(data.treasure_id ~= 5 and data.treasure_id ~= 6)
    UH.SetText(self.EndTime,left_time_str)
    UH.SetText(self.EndTime2,left_time_str)
    if treasure_cfg.skin_id == 0 or StringIsEmpty(treasure_cfg.skin_id) then
        self.SkinItemObj:SetActive(false)
    else
        local item_cfg = Item.GetConfig(treasure_cfg.skin_id)
        if item_cfg then
            UH.SetIcon(self.SkinIcon, item_cfg.icon_id)
            UH.SetIcon(self.SkinIcon2, item_cfg.icon_id)
            self.EffectTool:Play(3165161, self.EffPar)
            self.EffectTool:Play(3165161, self.EffPar2)
            self.SkinItemObj:SetActive(true)
        else
            self.SkinItemObj:SetActive(false)
        end
    end
    self.data = data
end

function TreasureHuntToggleItem:Click()
    if TreasureHuntData.Instance.smart_data.sended_req ~= true and TreasureHuntData.Instance.smart_data.sended_req ~= 1 then
        self.Toggle.isOn = true
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    -- self.Toggle:Trigger()
end

function TreasureHuntToggleItem:OnClickSkinIcon()
    self.data_instance.click_skin = true
    local item_cfg = Item.GetConfig(self.data_instance.treasure_id_config[self.data.treasure_id].skin_id)
    local item = Item.Init(item_cfg.id)
    local info = {
		item_list = {item},
		func = function()
			return get_other_func_table(item)
		end,
	}
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

----------------------------------------------
TreasureHuntCurrency = TreasureHuntCurrency or DeclareMono("TreasureHuntCurrency", UIWFlushPanel)
function TreasureHuntCurrency:TreasureHuntCurrency()
    self.data_instance = TreasureHuntData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid[ItemColumnType.Item], func = self.FlushCurrency},
        {data = self.data_instance.smart_data, func = self.FlushCurrency, keys = {"cur_treasure_id", "hunt_type"}, init_call = false},
    }
    -- 1号位和3号位放了特效
    self.huan_ling_config_list = {
        [1] = {id = HuoBi[CurrencyType.Treasures], mall_type = MallData.Page.Limit},
        [2] = {id = HuoBi[CurrencyType.Treasure], mall_type = MallData.Page.Limit},
        [3] = {id = HuoBi[CurrencyType.PartnerCalls], mall_type = MallData.Page.Pray},
        [4] = {id = HuoBi[CurrencyType.PartnerCall], mall_type = MallData.Page.Pray},
        [5] = {id = HuoBi[CurrencyType.TiePetRopes], mall_type = MallData.Page.Limit},
        [6] = {id = HuoBi[CurrencyType.TiePetRope], mall_type = MallData.Page.Limit},
    }
    -- 根据寻器类型
    self.xun_qi_config_list = {
        [1] = {
            [3] = {id = HuoBi[CurrencyType.XunQiZhouFus], mall_type = MallData.Page.Limit},
            [4] = {id = HuoBi[CurrencyType.XunQiZhouFu], mall_type = MallData.Page.Limit},
        },
        [2] = {
            [3] = {id = HuoBi[CurrencyType.ShengQiYaoShi], mall_type = MallData.Page.Limit},
            [4] = {id = HuoBi[CurrencyType.ShengQiYaoShis], mall_type = MallData.Page.Limit},
        },
    }
end

function TreasureHuntCurrency:FlushCurrency(data)
    if self.IconList == nil then
        LogError("IconList is nil return")
        return
    end
    if self.data_instance.smart_data.hunt_type == HUNT_TYPE.HUAN_LING then
        for i = 1, self.NumTextList:Length() do
            UH.SetText(self.NumTextList[i], Item.GetNum(self.huan_ling_config_list[i].id))
            UH.SetIcon(self.IconList[i], Item.GetConfig(self.huan_ling_config_list[i].id).icon_id)
            self.NumObjList[i]:SetActive(true)
        end
    elseif self.data_instance.smart_data.hunt_type == HUNT_TYPE.XUN_QI then
        local detail_count = 1
        local xunqi_type = self.data_instance:XunQiType()
        local xunqi_config_list = self.xun_qi_config_list[xunqi_type]
        for i = 1, self.NumObjList:Length() do
            if xunqi_config_list[i] then
                if self.OnDetailView then
                    UH.SetText(self.NumTextList[detail_count], Item.GetNum(xunqi_config_list[i].id))
                    UH.SetIcon(self.IconList[detail_count], Item.GetConfig(xunqi_config_list[i].id).icon_id)
                    self.NumObjList[detail_count]:SetActive(true)
                    self.NumObjList[i]:SetActive(false)
                    detail_count = detail_count + 1
                else
                    UH.SetText(self.NumTextList[i], Item.GetNum(xunqi_config_list[i].id))
                    UH.SetIcon(self.IconList[i], Item.GetConfig(xunqi_config_list[i].id).icon_id)
                    self.NumObjList[i]:SetActive(true)
                end
            else
                self.NumObjList[i]:SetActive(false)
            end
        end
    end
end

function TreasureHuntCurrency:OnClickAdd(index)
    if TreasureHuntData.Instance.exchange_request.sended_req == 2 then
        return
    end
    if not ViewMgr:IsOpen(TreasureHuntDetailView) then
        ViewMgr:CloseView(TreasureHuntView)
    end
    local cfg
    if self.data_instance.smart_data.hunt_type == HUNT_TYPE.HUAN_LING then
        cfg = self.huan_ling_config_list[index]
    elseif self.data_instance.smart_data.hunt_type == HUNT_TYPE.XUN_QI then
        local xunqi_type = self.data_instance:XunQiType()
        local xunqi_config_list = self.xun_qi_config_list[xunqi_type]
        if self.OnDetailView then
            cfg = xunqi_config_list[index + 2] and xunqi_config_list[index + 2] or cfg
        else
            cfg = xunqi_config_list[index]
        end
    end
    if cfg then
        if cfg.mall_type then
            ShoppingCtrl.Instance:OpenStoreView(cfg.mall_type)
        else
            MallCtrl.Instance:Jump(cfg.id)
        end
    end
end
--------------------------------------------
TreasureHuntQuickBuy = TreasureHuntQuickBuy or DeclareView("TreasureHuntQuickBuy","treasure_hunt/treasure_hunt_quick_buy")
VIEW_DECLARE_MASK(TreasureHuntQuickBuy,ViewMask.BgBlock)
function TreasureHuntQuickBuy:TreasureHuntQuickBuy()
    self.buy_num = 1
    self.is_empty = false
    self.param = nil
    self.data_instance = TreasureHuntData.Instance
end

function TreasureHuntQuickBuy:LoadCallback(param)
    if param and param.item_id then
        self.param = param
        --InfoText
        if param.draw_times == 1 then
            UH.SetText(self.InfoText, string.format(Language.TreasureHunt.desc, QualityColorStr[Item.GetColor(param.item_id)],Item.GetName(param.item_id)))
        else
            local treasure_hunt_cfg = TreasureHuntData.Instance:GetTreasureHuntConfig(param.treasure_id)
            UH.SetText(self.InfoText, string.format(Language.TreasureHunt.desc1,
                        QualityColorStr[Item.GetColor(treasure_hunt_cfg.once_item_id)],
                        Item.GetName(treasure_hunt_cfg.once_item_id),
                        QualityColorStr[Item.GetColor(param.item_id)],
                        Item.GetName(param.item_id)
                        ))
        end
        --Item
        local item = Item.Init(param.item_id)
        self.ItemCell:SetData(item)
        --消耗类型
        self.store_cfg = nil 
        if param.treasure_id == 5 then
            self.store_cfg = Cfg.StoreCfg(4, param.item_id)[1]
        else
            self.store_cfg = Cfg.StoreCfg(0, param.item_id)[1]
        end
        if not self.store_cfg then
            LogError("寻宝：未找到快速购买道具配置")
            ViewMgr:CloseView(TreasureHuntQuickBuy)
            return
        end
        UH.SpriteName(self.CurrencyImg, self.store_cfg.price_type == 0 and HuoBi[CurrencyType.Gold] or HuoBi[CurrencyType.Blessing])
        --限购数量
        self.buyed_count = StoreData.Instance:GetLimitTimes(self.store_cfg.conver_type, self.store_cfg.seq)
        if self.store_cfg.limit_convert_count == 0 then
            UH.SetText(self.BuyLimitText,"")
        else
            UH.SetText(self.BuyLimitText, Language.Shop.LimitTypeTip[self.store_cfg.limit_type]..Language.Common.MaoHao..string.format(Language.Common.Xstr, COLORSTR.Green3, self.store_cfg.limit_convert_count - self.buyed_count))
        end
        --购买数量
        self:setCostText()
        --按钮文字
        UH.SetText(self.ConfirmText, self.store_cfg.price_type == 0 and Language.TreasureHunt.BuyUse or Language.TreasureHunt.ExchangeUse)
    else
        self.is_empty = true
        UH.SetText(self.InfoText,"")
        UH.SetText(self.BuyLimitText,"")
        UH.SetText(self.CostNumText,"")
        UH.SetText(self.ConfirmText,Language.Common.Confirm)
        UH.SetText(self.BuyNumText,1)
        self.ItemCell:SetData()
    end
end

function TreasureHuntQuickBuy:setCostText()
    UH.SetText(self.BuyNumText, self.buy_num)
    UH.SetText(self.CostNumText, self.buy_num * self.store_cfg.price)
end

function TreasureHuntQuickBuy:OnClickAdd()
    self.buy_num = self.buy_num + 1 + self.buyed_count > self.store_cfg.limit_convert_count and self.buy_num or self.buy_num + 1
    self:setCostText()
end

function TreasureHuntQuickBuy:OnClickSub()
    self.buy_num = self.buy_num - 1 > 0 and self.buy_num - 1 or 1
    self:setCostText()
end

function TreasureHuntQuickBuy:OnClickCancel()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    ViewMgr:CloseView(TreasureHuntQuickBuy)
end

function TreasureHuntQuickBuy:OnClickBuy()
    if not self.is_empty then
        if self.buyed_count >= self.store_cfg.limit_convert_count then
            local limit_show = Format(Language.Shop.LimitTypeTip[self.store_cfg.limit_type], self.store_cfg.conver_type == 0 and Language.TreasureHunt.buy or Language.TreasureHunt.exchange)
            PublicPopupCtrl.Instance:Center(limit_show .. Language.Shop.NotHasTip)
            return
        end
        if self.store_cfg.price_type == 0 then
            if RoleData.Instance:GetCurrency(CurrencyType.Gold) < self.buy_num * self.store_cfg.price then
                local info = {
                    content = Language.Mall.NotCurrency[3],
                    close = true,
                    cancel = {
                        name = Language.Mall.NotCurrency[4],
                        func = function()
                            TradeData.Instance:SetJumpItemData(Item.Init())
                            ViewMgr:OpenViewByKey(Mod.Mall.Trade)
                            PublicPopupCtrl.Instance:CloseDialogTips()
                        end
                    },
                    confirm = {
                        name = Language.Mall.NotCurrency[5],
                        func = function()
                            ViewMgr:OpenViewByKey(Mod.Store.Recharge)
                            PublicPopupCtrl.Instance:CloseDialogTips()
                        end
                    },
                }
                if GLOBAL_CONFIG:Flags().block_recharge then
                    info.no_confirm = true
                end
                PublicPopupCtrl.Instance:DialogTips(info)
                return
            end
        else
            if RoleData.Instance:GetCurrency(CurrencyType.Blessing) < self.buy_num * self.store_cfg.price then
                MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = HuoBi[CurrencyType.Blessing]})})
                return
            end
        end
        TreasureHuntData.Instance.smart_data.auto_use = true
        TreasureHuntData.Instance.auto_buy_data = {draw_times = self.param and self.param.draw_times, treasure_id = TreasureHuntData.Instance.smart_data.cur_treasure_id, item_id = self.store_cfg.item_id}
        ShoppingCtrl:SendShopBuyReq(self.store_cfg.conver_type,self.store_cfg.seq,self.store_cfg.item_id, self.buy_num)
        ViewMgr:CloseView(TreasureHuntQuickBuy)
    end
end

----------------------------------------------
TreasureHuntActivity = TreasureHuntActivity or DeclareMono("TreasureHuntActivity", UIWFlushPanel)
function TreasureHuntActivity:TreasureHuntActivity()
    self.Data = TreasureHuntData.Instance
    self.data = self.Data.smart_data
    self.data_cares = {
        {data = self.data, func = self.FlushShow, keys = {"cur_treasure_id", "hunt_type"}, init_call = false},
        {data = ActivityData.Instance:GetActivityEvent(), func = self.FlushShow},
    }
end

function TreasureHuntActivity:FlushShow()
    self.ObjBtn:SetActive(false)
    local treasure_hunt_cfg = self.Data:GetCurSelectCfg()
    self.now_act = self.Data:JumpActivity()
    if not TableIsEmpty(treasure_hunt_cfg) and not TableIsEmpty(self.now_act) then
        self.ObjBtn:SetActive(true)
        UH.SetIcon(self.Icon, Item.GetIconId(treasure_hunt_cfg.five_item_id), ICON_TYPE.ITEM)
        UH.SetText(self.TxtDesc, self.now_act.desc)
    end
end

function TreasureHuntActivity:OnGoToClick()
    if not TableIsEmpty(self.now_act) then
        if self.now_act.jump_interface then
            ViewMgr:OpenViewByKey(self.now_act.jump_interface)
        end
    end
end