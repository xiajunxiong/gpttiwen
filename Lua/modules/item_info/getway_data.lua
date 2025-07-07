GetWayData = GetWayData or BaseClass()

CurrencyToItemId = {
    [CommonItemId.SkyGlass] = CommonItemId.SkyGlassHundred,
    [CommonItemId.PartnerGiftNomal] = CommonItemId.PartnerGiftNomalTen,
    [CommonItemId.PartnerGiftRare] = CommonItemId.PartnerGiftRareTen,
    [56365] = 29378,
}

-- 对不同系统下的同一个道具的购买阈值的清算方法
CurrencyToItemNum = {
    [CommonItemId.PartnerGiftNomal] = function (need_num,mod)
        if need_num == nil then return 1 end 
        if mod == Mod.Store.Main then --阈值 100
            return need_num % 100 == 0 and math.floor(need_num /100) or math.floor(need_num /100) + 1
        elseif mod == Mod.Mall.Market then --阈值 10
            return need_num % 10 == 0 and math.floor(need_num /10) or math.floor(need_num /10) + 1
        end 
    end,
    [CommonItemId.PartnerGiftRare] = function (need_num,mod)
        if need_num == nil then return 1 end 
        if mod == Mod.Store.Main then 
            return need_num % 100 == 0 and math.floor(need_num /100) or math.floor(need_num /100) + 1
        elseif mod == Mod.Mall.Market then 
            return need_num % 10 == 0 and math.floor(need_num /10) or math.floor(need_num /10) + 1
        end 
    end,
}

GetWayOpenPanelFunc = 
{
    [Mod.LandsBetween.Main] = function(info)
        for i,v in pairs(LandsBetweenCloseViewList) do
            ViewMgr:CloseView(GetView(v))
        end
        if not ViewMgr:IsOpen(LandsBetweenMainView) then 
            ViewMgr:OpenView(LandsBetweenPosterView)
        end 
        return true
    end,
    [Mod.LandsBetweenExtra.Shop] = function (info)
        if LandsBetweenData.Instance:IsOpen() and LandsBetweenData.Instance:GetCanAccess() then 
            LandsBetweenCtrl.Instance:TryOpenMain({wait_view = LBShopView})
        elseif LandsBetweenData.Instance:IsOpen() and  not self.data:GetCanAccess() then 
            PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.AccessFailTip)
        else 
            PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.BtnActUnOpen)
        end

        return true
    end,
    [Mod.LBFestival.main] = function (info)
        if LandsBetweenData.Instance:IsOpen() and LandsBetweenData.Instance:GetCanAccess() then 
            LandsBetweenCtrl.Instance:TryOpenMain({wait_view = LBFestivalView})
        elseif LandsBetweenData.Instance:IsOpen() and  not self.data:GetCanAccess() then 
            PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.AccessFailTip)
        else 
            PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.BtnActUnOpen)
        end

        return true
    end,
    [Mod.Reward.GodToken] = function (info)
        local param = {}
        if info.params ~= "" then
            param.tab_index = tonumber(info.params)
            GodTokenData.Instance:SetSelectTab(param.tab_index)
        end
        ViewMgr:OpenViewByKey(Mod.Reward.GodToken)
        return true
    end,
    [Mod.Live.Main] = function (info, item)
        local is_flush = false
        if ViewMgr:IsOpen(LiveView) then
            is_flush = true
        end
        local item_id = item.item_id
        LiveData.Instance:JumpItem(item_id,is_flush)
        ViewMgr:OpenViewByKey(Mod.Live.Main)
        ViewMgr:CloseView(GetWayView)
        return true
    end,
    [Mod.Fight.Dungeon] = function(info)
        local dungeon = FightData.Instance.dungeon_seq_list[info.params]
        if dungeon then
            FightData.Instance.cur_sel_types.dungeon_type = dungeon.scene_type_1
            FightData.Instance.cur_sel_dungeon_jump = dungeon
        end
    end,
    [Mod.Fight.Gather] = function(info)
        local config = FightData.Instance:GetStoneSceneDataBySeq(info.params)
        if config then
            FightData.Instance:SetGatherSelectType(config.scene_type_1)
            FightData.Instance.cur_sel_type = config.seq or 1
        end
    end,
    [Mod.Mall.Trade] = function(info,item)
        TradeData.Instance:SetSearchItemId(item.item_id)
        local type = TradeData.Instance:GetItemType(item.item_id)
        TradeData.Instance:SetSelectBigType(type)
    end,
    [Mod.Mall.Shop] = function(info,item)
        CommerceData.Instance:SetJumpIndex(item.item_id)
        MallData.Instance:SetJumpItemNum(item.vo.jump_need_num)
        ViewMgr:OpenViewByKey(Mod.Mall.Shop)
        return true
    end,
    [Mod.Store.Main] = function(info,item)
        if info.params == "" then
            local key,config = MallData.Instance:GetShopItemConfig(item.item_id,info.open_panel)
            if config.conver_type == GOLD_STORE_TYPE then
                StoreData.Instance:SetJumpItemId(item.item_id)
                ViewMgr:OpenViewByKey(Mod.Store.Gold)
                return true
            end
            StoreData.Instance:SetJumpIndex(config,config.item_id)
        else
            local _,shop_config = StoreData.Instance:GetShopTypeIndex(info.params)
            if shop_config and RoleData.Instance:GetRoleLevel() < shop_config.limit_level then
                PublicPopupCtrl.Instance:Center(Format(Language.Shop.LevelLimitTip,shop_config.limit_level))
                return true
            end
            StoreData.Instance:SetJumpIndex({conver_type = info.params},item.item_id)
        end
        local jump_need_num = item.vo and item.vo.jump_need_num or 1
        local currency_func = CurrencyToItemNum[item.item_id]
        MallData.Instance:SetJumpItemNum(currency_func and currency_func(jump_need_num,Mod.Store.Main) or jump_need_num)
    end,
    [Mod.Store.Gold] = function(info,item)
        if item then
            MallData.Instance:SetJumpItemNum(item.vo and item.vo.jump_need_num or 1)
            StoreData.Instance:SetJumpItemId(item.item_id)
        end
        ViewMgr:OpenViewByKey(Mod.Store.Gold)
        return true
    end,
    [Mod.Equip.Inser] = function(info,item)
        ViewMgr:OpenViewByKey(info.open_panel)
        ViewMgr:CloseView(ItemInfoView)
        ViewMgr:CloseView(BagView)
        ViewMgr:CloseView(GetWayView)
        return true
    end,
    [Mod.Workshop.NeoCompose] = function(info,item)
        WorkshopData.Instance:QuickComposeItem(item.item_id)
        ViewMgr:OpenViewByKey(info.open_panel)
        ViewMgr:CloseView(ItemInfoView)
        ViewMgr:CloseView(BagView)
        ViewMgr:CloseView(GetWayView)
        return true
    end,
    [ModToModule(Mod.FightFieldBoss.Main)] = function ()
        if ViewMgr:IsOpen(BicycleView) then
            ViewMgr:CloseView(BicycleView)
        end
    end,
    [ModToModule(Mod.FightFB.Material)] = function(info,item)
        if info.open_panel == Mod.FightFB.Fight then
            if tonumber(info.params) == nil then return false end 
            local config = FightData.Instance.dungeon_seq_list[info.params]
            local lock_flag = RoleData.Instance:GetRoleLevel() >= config.level_min

            if not lock_flag then 
                PublicPopupCtrl.Instance:Center(ErrorInfo[100008])
            else 
                FightData.Instance:FightFBYMZOpenSel(info.params)
                if ViewMgr:IsOpen(FightFBView) then
                    FightData.Instance:FightFBYMZAutoSel()
                end 
                ViewMgr:OpenViewByKey(Mod.FightFB.Fight)

                ViewMgr:CloseView(Workshop)
                ViewMgr:CloseView(GetWayView)
            end 
            return true
        end 
        local config = MaterialData.Instance:GetChapterConfig(info.params)

        if tonumber(info.params) == nil then return false end 
        if ViewMgr:IsOpen(MaterialChallengeView) then
            ViewMgr:CloseView(GetWayView)
            return true
        end
        local flag_first = MaterialData.Instance:IsLastItemFirstKill(config) --and config.chapter_level_limit <= RoleData.Instance:GetRoleLevel()
        if flag_first then 
            MaterialCtrl.Instance:OnBackChallenge(MaterialData.Instance:GetChapterShowData(config.chapter),config)
            ViewMgr:CloseView(Workshop)
            ViewMgr:CloseView(GetWayView)
            return true            
        end 
            
        -- if config.chapter_level_limit >  RoleData.Instance:GetRoleLevel() then 
        --     PublicPopupCtrl.Instance:Center(string.format(Language.Material.CantOpenByLevelLimit,config.chapter_level_limit))
        --     return true
        -- else 
            local last_config = MaterialData.Instance:GetChapterConfig(config.seq-1)
            PublicPopupCtrl.Instance:Center(string.format(Language.Material.CantOpenByFirstLimit
            ,string.format(Language.Material.CantLimitedShow,last_config.chapter+1,last_config.level+1)))
            return true
        -- end 
    end,
    [Mod.CloudArena.Main] = function ()
        if LoginData.Instance:IsOnCrossSever() then
            ViewMgr:OpenView(CloudArenaView)
        else
            CloudArenaCtrl.Instance.RequestCrossServer = true
            LoginCtrl.Instance:SendCrossStartReq()
        end
        if ViewMgr:IsOpen(DivineCostumeView) then ViewMgr:CloseView(DivineCostumeView) end 
        ViewMgr:CloseView(GetWayView)
        return true
    end,
    [Mod.Experience.ChallengeTest] = function()
        if TeamData.Instance:InTeam() then
            PublicPopupCtrl.Instance:Center(Language.ChallengeTest.TeamErrorTips)
            return true
        end
    end,
    [Mod.Mall.Market] = function(info,item)
        MallData.Instance:SetJumpItemNum(CurrencyToItemNum[item.item_id]
            and CurrencyToItemNum[item.item_id](item.vo.jump_need_num,Mod.Mall.Market) or item.vo.jump_need_num)
        MarketData.Instance:SetJumpIndex(CurrencyToItemId[item.item_id] or item.item_id)
        ViewMgr:OpenViewByKey(Mod.Mall.Market)
        ViewMgr:CloseView(GetWayView)
        return true
    end,
    [Mod.Mall.Exchange] = function()
        local param_t = {
            config = StoreData.Instance:GetItemConfig(CommonItemId.ImmortalCoinId,5),
            comfirm_func = function(data)
                ShoppingCtrl.Instance:SendStoreBuyReq(data)
                ViewMgr:CloseView(MoneyExchangeView)
            end
        }
        param_t.money_type = CurrencyType.ImmortalCoin
        ViewMgr:OpenView(MoneyExchangeView,param_t)
        ViewMgr:CloseView(GetWayView)
        return true
    end,
    [Mod.Battlefield.Main] = function(info,item)
        BattlefieldCtrl.Instance:OnBattlefield()
        ViewMgr:CloseView(ItemInfoView)
        ViewMgr:CloseView(GetWayView)
        return true
    end,
    [Mod.Battlefield.Shop] = function(info,item)
        if AnotherData.IsGuide() then
            BattlefieldCtrl.Instance:OnBattlefield()
        else
            ViewMgr:OpenViewByKey(info.open_panel)
        end
        ViewMgr:CloseView(ItemInfoView)
        ViewMgr:CloseView(GetWayView)
        return true
    end,
    [Mod.Challenge.RareDungeon] = function(info,item)
        if tonumber(info.params) == nil then return false end
        local config = FightData.Instance.dungeon_seq_list[info.params]
        local lock_flag = RoleData.Instance:GetRoleLevel() >= config.level_min
        if lock_flag then 
            FightData.Instance:FightFBRareDungeonOpenSel(info.params)
            ViewMgr:OpenViewByKey(Mod.Challenge.RareDungeon)
        else 
            PublicPopupCtrl.Instance:Center(ErrorInfo[100008])
        end
        return true
    end,
    [Mod.Reward.FortuneBag] = function(info,item)
        if not FunOpen.Instance:GetFunIsOpened(Mod.Reward.FortuneBag) then
            PublicPopupCtrl.Instance:Center(Language.Store.FortuneNoOpenTip)
            return true
        end
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_FORTUNE_BAG) then
            PublicPopupCtrl.Instance:Center(Language.Store.FortuneNoOpenTip)
            return true
        end
        ViewMgr:OpenViewByKey(Mod.Reward.FortuneBag)
        return true
    end,
    [Mod.Pet.PetAttr] = function ()
        if ViewMgr:IsOpen(PetPieces) then
            ViewMgr:CloseView(PetPieces)
        end
        ViewMgr:OpenViewByKey(Mod.Pet.PetAttr)
    end,
    [Mod.TreasureHunt.Main] = function ()
        if ViewMgr:IsOpen(PetPieces) then
            ViewMgr:CloseView(PetPieces)
        end
        ViewMgr:OpenViewByKey(Mod.TreasureHunt.Main)
    end,
    [Mod.SuperMap.Main] = function ()
        if ViewMgr:IsOpen(PetPieces) then
            ViewMgr:CloseView(PetPieces)
        end
        ViewMgr:OpenViewByKey(Mod.SuperMap.Main)
    end,
    [Mod.DeepDarkFantasy.Main] = function()
        if TeamData.Instance:InTeam() then 
            ViewMgr:CloseView(ItemInfoView)
            local info = {
                content = Language.DeepDarkFantasy.TeamErrorDialog,
                cancel = {
                    name = nil,
                    func = nil
                },
                confirm = {
                    name = nil,
                    func = function ()
                        TeamCtrl.Instance:SendExitTeam()
                        ViewMgr:OpenView(DeepDarkFantasyView)
                    end,
                }
            }
            PublicPopupCtrl.Instance:DialogTips(info)
            return true
        end 

        ViewMgr:CloseView(StoreView)
    end,
    [Mod.YouMing.Seal] = function()
        local scene_id = YouMingData.Instance:GetSealSceneId()
        local cb = function ()
            ViewMgr:CloseView(MingZhuangBagView)
            ViewMgr:CloseView(YouMingDictionariesView)
            ViewMgr:CloseView(YouMingResloveView)
            ViewMgr:CloseView(YouMingPractice)
            ViewMgr:CloseView(YouMingView)
        end
    
        YouMingCtrl.Instance:GoToSealScene(scene_id, cb)
    end,
}

-- 当主系统下的分系统的功能开启比主系统高时使用这个方法组控制获取途径
GetWayPlusFunOpen = {
    [Mod.Store.Main] = function(info,item)
        if info.params == "" then
            return false
        else
            local _,shop_config = StoreData.Instance:GetShopTypeIndex(info.params)
            if shop_config and RoleData.Instance:GetRoleLevel() < shop_config.limit_level then
                return true
            end
        end
    end,
}

-- Errorfunc 容错 bool 
-- BtnStyleFunc 按钮 {type=1黄色,0绿色,text=按钮文本}
-- TimerFunc 次数 {is_act = 是否展示，text=内容}
-- InterFunc 是否置灰 bool
-- BtnSpClickFunc 特殊业务逻辑，返回true则不执行特殊业务逻辑
-- care_data 刷新逻辑，在需要刷新次数的场合下需要有
GetWayFBFlush = {
    [Mod.FightFB.Material] = {
        Errorfunc = function(way_cfg)
            return tonumber(way_cfg.params) == nil
        end,
        BtnStyleFunc = function(way_cfg)
            local config = MaterialData.Instance:GetChapterConfig(way_cfg.params)
            local flag = MaterialData.Instance:GetConditonScore(config) == 3
            local param = {
                type = flag and 0 or 1,
                text = flag and Language.Common.Skip or Language.Common.GoTo,
            }
            return param
        end,
        TimerFunc = function(way_cfg)
            local config = MaterialData.Instance:GetChapterConfig(way_cfg.params)
            local param = {
                is_act = MaterialData.Instance:GetConditonScore(config) == 3,
                text = GetWayData.GetWayTimerFunc({
                    color_bool = MaterialData.Instance:GetSweepTimes(config) > 0,
                    cur_time = MaterialData.Instance:GetSweepTimes(config), 
                    total_time = MaterialData.Instance:GetHasMaxTimes(config),
                }),
            }
            return param
        end,
        InterFunc = function(way_cfg)
            local config = MaterialData.Instance:GetChapterConfig(way_cfg.params)
            return MaterialData.Instance:IsLastItemFirstKill(config) --and config.chapter_level_limit <= RoleData.Instance:GetRoleLevel()
        end,
        care_data = function ()
            return MaterialData.Instance.info_data
        end,      
        BtnSpClickFunc = function(way_cfg,item)
            -- 扫荡逻辑
            if tonumber(way_cfg.params) == nil then return false end 
            local config = MaterialData.Instance:GetChapterConfig(way_cfg.params)
            local skip_flag = MaterialData.Instance:GetConditonScore(config) == 3 
            if not (ViewMgr:IsOpen(SweepResultView) and ViewMgr:IsOpen(GetWayView)) and not skip_flag then 
                return true
            end 

            local function skip_fail_check()
                local opers = MaterialData.Instance
                local config = opers:GetChapterConfig(way_cfg.params)
                if opers:GetSweepTimes(config) == 0 then
                    -- 购买次数也无
                    if opers:GetBuyTimes(config) == 0 then 
                        PublicPopupCtrl.Instance:Center(Language.Material.NotHasBuyTimes)
                        return false
                    else 
                        PublicPopupCtrl.Instance:AlertTip(Format(Language.Material.BuyTimesTip,
                        opers:GetTimesPrice(config),opers:GetBuyTimes(config)),function()
                            MaterialCtrl.Instance:SendMaterialReq(3,config.seq)
                        end,CurrencyType.Gold)
                            return false
                    end
                elseif opers:GetFightStrengthNum() > FightData.Instance:GetFightStrengthNum() then 
                    ViewMgr:OpenView(FightCrystalSweepItemView)
                    return false
                else 
                    return true
                end 
            end

            if skip_fail_check() then 
                MaterialCtrl.Instance:SendMaterialReq(1,way_cfg.params)
                ViewMgr:CloseView(GetWayView)

                BagData.Instance:SetGetWaySweepMark({
                    is_mark = true,
                    item_id =item.item_id ,
                    func = function()
                        if skip_fail_check() then 
                            MaterialCtrl.Instance:SendMaterialReq(1,way_cfg.params)
                        end 
                    end
                })
            end

            return false
        end
    },
    [Mod.FightFB.Fight] = {
        Errorfunc = function(way_cfg)
            return tonumber(way_cfg.params) == nil
        end,
        BtnStyleFunc = function(way_cfg)
            local flag = false
            if tonumber(way_cfg.params) ~= nil then flag = FightCtrl.Instance:Data():IsFightCrystalPassFb(tonumber(way_cfg.params)) end 
            local param = {
                type = flag and 0 or 1,
                text = flag and Language.Common.Skip or Language.Common.GoTo,
            }
            return param
        end,
        TimerFunc = function(way_cfg)
            local config = FightData.Instance.dungeon_seq_list[way_cfg.params]
            local max_time = FightData.Instance:GetCrystalFbPassTimeMax()
            local extra_time = FightData.Instance.fight_fb_ymz.normal_fb_record_bt_list[way_cfg.params] or 0
            local sweep_time = max_time + extra_time - (FightData.Instance.fight_fb_ymz.normal_fb_record_list[way_cfg.params] or 0)
            local param = {
                is_act = config ~= nil and RoleData.Instance:GetRoleLevel() >= config.level_min and config.ymz_type == 1,
                text = config ~= nil and  GetWayData.GetWayTimerFunc({
                    color_bool = sweep_time > 0,
                    cur_time = sweep_time, 
                    total_time =  max_time + extra_time, 
                }) or "",
            }

            return param
        end,
        InterFunc = function(way_cfg)
            local config = FightData.Instance.dungeon_seq_list[way_cfg.params]
            return config ~= nil and RoleData.Instance:GetRoleLevel() >= config.level_min
        end,        
        care_data = function ()
            return FightCtrl.Instance:Data().fight_fb_ymz
        end,
        BtnSpClickFunc = function(way_cfg,item)
            if tonumber(way_cfg.params) == nil then return false end 
            local config = FightData.Instance.dungeon_seq_list[way_cfg.params]
            local skip_flag = FightCtrl.Instance:Data():IsFightCrystalPassFb(way_cfg.params)
            local max_time = FightData.Instance:GetCrystalFbPassTimeMax()
            local extra_time = FightData.Instance.fight_fb_ymz.normal_fb_record_bt_list[way_cfg.params] or 0
            local sweep_time = max_time + extra_time - (FightData.Instance.fight_fb_ymz.normal_fb_record_list[way_cfg.params] or 0)
            if not skip_flag then return true end 

            -- 检查次数并提醒购买
            local buyed_extra_time = (FightData.Instance.fight_fb_ymz.normal_fb_record_bt_list[way_cfg.params] or 0)
            local buy_time = FightData.Instance:GetCrystalFbExtraBuyTimes() - buyed_extra_time
            if sweep_time == 0 and buy_time > 0 then 
                PublicPopupCtrl.Instance:AlertTip(string.format(Language.Fight.YMZ.BuyTimesTip, FightData.Instance:GetCrystalFbExtraBuyConsume(), math.max(buy_time, 0)),function()
                    FightCtrl.Instance:SendCrystalFbReq(3, way_cfg.params)
                end,CurrencyType.Gold)

                return false
            elseif sweep_time == 0 and buy_time == 0 then 
                PublicPopupCtrl.Instance:Center(Language.DreamNotes.NotHasBuyTimes)
                return false
            end 

            -- 先检查体力
            if 0 == BagData.Instance:GetNumByItemId(config.consume.item_id) and not FightData.Instance:IsFightCrystalEnergyEnough() then
                ViewMgr:OpenView(FightCrystalSweepItemView)
                return false
            end 
            -- 检查能不能扫荡
            local value, tips = FightData.Instance:IsCanFightCrystalFbSweep(config)   
            if not value then
                PublicPopupCtrl.Instance:Center(tips)
                return false
            end
            -- 能打就扫荡
            FightCtrl.Instance:SendCrystalFbFastPassInfo(way_cfg.params, 1)
            return false
        end,
    },

    [Mod.Challenge.RareDungeon] = {
        Errorfunc = function(way_cfg)
            return tonumber(way_cfg.params) == nil or FightData.Instance.dungeon_seq_list[way_cfg.params] == nil
        end,
        BtnStyleFunc = function(way_cfg)
            return {type = 1,text = Language.Common.GoTo,}
        end,
        TimerFunc = function(way_cfg)
            local config = FightData.Instance.dungeon_seq_list[way_cfg.params]
            local info = FightCtrl.Instance:Data().fight_crystal_sweep
            return {
                is_act = config ~= nil and RoleData.Instance:GetRoleLevel() >= config.level_min,
                text = GetWayData.GetWayTimerFunc({
                    color_bool = info.new_xiyou_play_times_remain > 0,
                    cur_time = info.new_xiyou_play_times_remain or 0,  
                    total_time = info.new_xiyou_play_times or 0,
                }),
            }
        end,
        InterFunc = function(way_cfg)
            local config = FightData.Instance.dungeon_seq_list[way_cfg.params]
            return config ~= nil and RoleData.Instance:GetRoleLevel() >= config.level_min
        end,        
        care_data = function ()
            return FightData.Instance.fight_crystal_sweep
        end,
    },
}

GetWayParamFunc = {
    AutoToPos = function()
        ViewMgr:CloseView(FightView)
        ViewMgr:CloseView(FightAutoSealSetView)
    end,
    AutoToNpc = function()
        ViewMgr:CloseView(Workshop)
        ViewMgr:CloseView(PartnerView)
        ViewMgr:CloseView(FightView)
        ViewMgr:CloseView(FightAutoSealSetView)
        ViewMgr:CloseView(PetBreachView)
        ViewMgr:CloseView(PetView)
    end,
    CloseFunc = function()
        ViewMgr:CloseView(MedalGetWayView)
        ViewMgr:CloseView(ItemInfoView)
        ViewMgr:CloseView(BagView)
        ViewMgr:CloseView(GetWayView)
        ViewMgr:CloseView(Workshop)
        ViewMgr:CloseView(SpiritLevelView)
        ViewMgr:CloseView(SpiritResolveView)
        ViewMgr:CloseView(EquipView)
        ViewMgr:CloseView(BattlefieldMatch)
        ViewMgr:CloseView(BattlefieldShopView)
        ViewMgr:CloseView(BatchBuyView)
        ViewMgr:CloseView(RoleView)
        ViewMgr:CloseView(FlyUpView)
        ViewMgr:CloseView(YouMingPractice)
        ViewMgr:CloseView(YouMingView)
    end,
    GatherFunc = function()
        ViewMgr:CloseView(Workshop)
        ViewMgr:CloseView(SilkRoadView)
    end,
}

-- 检查该物品是否皆为快速获取途径（直接扫荡
function GetWayData.CheckQuickGetWay(item_id)
    local config = Item.GetConfig(item_id)
    if config == nil then
        return false
    end
    if config.get_way == nil then
        return false
    end
    if config.get_way == "" then
        return false
    end
    if config.special_way == nil or config.special_way == 0 or config.special_way == 2 then 
        return false
    end 
    return true
end

--param color_bool 颜色判断 cur_time 当前次数 total_time 总次数 special_tetx 自定义缀词
function GetWayData.GetWayTimerFunc(param)
    if param.special_text ~= nil then 
        local COLOR = param.color_bool and COLORSTR.Green7 or COLORSTR.Red20
        return Format(param.special_tetx,COLOR,param.cur_time.."/"..param.total_time)
    else 
        local COLOR = param.color_bool and COLORSTR.Green7 or COLORSTR.Red20
        return Format(Language.Common.StuffTimesColor,COLOR,param.cur_time.."/"..param.total_time )
    end 
end 

--[[info:getway_auto item :item_cell]]
function GetWayData.OpenPanel(info,item)
    if info == nil then
        return
    end
    if not StringIsEmpty(info.npc) then
        if item then
            ReputationCtrl.Instance:SetOpenData(info,item)
        end
        SceneLogic.Instance:AutoToNpc(info.npc,nil,false,true)
        GetWayParamFunc.AutoToNpc()
    elseif not StringIsEmpty(info.scene_id) then
        SceneLogic.Instance:AutoToPos(info.scene_id,nil,nil,false,true)
        GetWayParamFunc.AutoToPos()
    else
        local open_panel = info.open_panel
        if StringIsEmpty(open_panel) and info.params ~= "" and GetView(info.params) then
            ViewMgr:OpenView(GetView(info.params))
            ViewMgr:CloseView(ItemInfoView)
            return
        end
        if StringIsEmpty(open_panel) then
            if info.id == 180 then  -- id=180是运营活动，特殊处理
                PublicPopupCtrl.Instance:Center(Language.Activity.ActivityCanGetTip)
            end
            return
        end
        local opened, open_tip = FunOpen.Instance:GetFunIsOpened(open_panel)
		if not opened then
			PublicPopupCtrl.Instance:Center(open_tip)
			return
		end
        if ItemInfoCtrl.Instance:Data():GetItemInfoData() then
            item = ItemInfoCtrl.Instance:Data():GetItemInfoData()
        end
        if ActivityRandGroupTab[ModToModule(open_panel)] then
            if GetWayData.ActivityRandFunc(info,item) then
                ViewMgr:CloseView(ItemInfoView)
                return
            end
        end
        local open_func = GetWayOpenPanelFunc[open_panel] or
        GetWayOpenPanelFunc[ModToModule(open_panel)]
        if open_func ~= nil and open_func(info,item) then
            ViewMgr:CloseView(ItemInfoView)
            ViewMgr:CloseView(GetWayView)
            return
        end
        ViewMgr:OpenViewByKey(open_panel)
    end
    GetWayParamFunc.CloseFunc(info)
end

ActivityRandGroupTab = {
    [ModToModule(Mod.RandActivity.SevenDay)] = true,
    [ModToModule(Mod.Splendid.Main)] = true,
    [ModToModule(Mod.ServerActivity.Main)] = true,
    [ModToModule(Mod.SpringFestival.Main)] = true,
    [ModToModule(Mod.PlanActivity.Main)] = true,
    [ModToModule(Mod.NationalDay.Main)] = true,
    [ModToModule(Mod.MountainSea.Main)] = true,
    [ModToModule(Mod.SummerFirst.Main)] = true,
    [ModToModule(Mod.YunZe.Main)] = true,
    [ModToModule(Mod.TigerYear.Main)] = true,
}

function GetWayData.ActivityRandFunc(info,item)
    local config = ActivityRandData.Instance:GetConfigByKey(info.open_panel)
    if config and ActivityData.IsOpen(config.act_type) then
        if not ActivityRandData.Instance:GetRegisterOpen(info.open_panel) then
            return true
        end
    else
        PublicPopupCtrl.Instance:Center(nil == item and Language.Activity.NotOpenNoItemTip or Language.Activity.NotOpenItemTip)
        return true
    end
end

function GetWayData.GetWayIsOpen(cfg)
    if cfg.open_panel == Mod.FightFB.Material then 
        if tonumber(cfg.params) == nil then return false end 
        local config = MaterialData.Instance:GetChapterConfig(cfg.params)
        return MaterialData.Instance:IsLastItemFirstKill(config)
    elseif cfg.open_panel == Mod.FightFB.Fight then 
        if tonumber(cfg.params) == nil then return false end 
        local config = FightData.Instance.dungeon_seq_list[cfg.params]
        return RoleData.Instance:GetRoleLevel() >= config.level_min
    elseif cfg.open_panel == Mod.Challenge.RareDungeon then 
        if tonumber(cfg.params) == nil then return false end 
        local config = FightData.Instance.dungeon_seq_list[cfg.params]
        return RoleData.Instance:GetRoleLevel() >= config.level_min
    else 
        return true
    end 
end

function GetWayData.GetWayList(item)
    local way_list = item:GetWay()
    if way_list ~= nil and next(way_list) ~= nil then
        table.sort(way_list,function(a,b)
            local a_sort, b_sort = tonumber(a),tonumber(b)
            local info_a = BagData.Instance:GetWayCfg(a)
            if tonumber(info_a.value_add) ~= nil then 
                a_sort = a_sort + info_a.value_add
            end 
            if StringIsEmpty(info_a.npc) and StringIsEmpty(info_a.open_panel) then 
                a_sort = a_sort + 10000
            end 
            local info_b = BagData.Instance:GetWayCfg(b)
            if tonumber(info_b.value_add) ~= nil then 
                b_sort = b_sort + info_b.value_add
            end 
            if StringIsEmpty(info_b.npc) and StringIsEmpty(info_b.open_panel) then 
                b_sort = b_sort + 10000
            end
            if not GetWayData.IsFunIsOpened(a,item) or not GetWayData.GetWayIsOpen(info_a) then
                a_sort = a_sort + 100000
            end
            if not GetWayData.IsFunIsOpened(b,item) or not GetWayData.GetWayIsOpen(info_b) then
                b_sort = b_sort + 100000
            end
            return a_sort < b_sort
        end)
        return way_list
    end
    return {}
end

function GetWayData.IsFunIsOpened(data,item)
    local info = BagData.Instance:GetWayCfg(data)
    --当open_panel为空时直接返回true，如声望商店找npc的就没有open_panel
    if StringIsEmpty(info.open_panel) then
        return true
    end
    local plus_open_func = GetWayPlusFunOpen[info.open_panel] or GetWayPlusFunOpen[ModToModule(info.open_panel)]
    return FunOpen.Instance:GetFunIsOpened(info.open_panel) and not (plus_open_func~=nil and plus_open_func(info,item))
end

--外部跳转到对应界面接口
--id：获取途径id或mod_key item_id:需要跳转的道具id need_num:需要跳转道具id的数量 固定数量
function GetWayData.OnOpenPanel(id,item_id,need_num)
	local item = nil
	if item_id and item_id ~= 0 then
		item = Item.Init(item_id)
	end
	if item and need_num and need_num ~= 0 then
		if Item.GetNum(item_id) < need_num then
			item.vo.jump_need_num = need_num - Item.GetNum(item_id)
		end
	end
	if id > 1000 then
		local open_panel = id
		local opened, open_tip = FunOpen.Instance:GetFunIsOpened(open_panel)
		if not opened then
			PublicPopupCtrl.Instance:Center(open_tip)
			return
		end
		local open_func = GetWayOpenPanelFunc[open_panel] or
        GetWayOpenPanelFunc[ModToModule(open_panel)]
        if open_func ~= nil and open_func(info,item) then
            return
        end
        ViewMgr:OpenViewByKey(open_panel)
	else
		local info = BagData.Instance:GetWayCfg(id)
		GetWayData.OpenPanel(info,item)
	end
end