PetBefallView = PetBefallView or DeclareView("PetBefallView","serveractivity/pet_befall_view",Mod.Splendid.PetBefall)
PetBefallView.PetImgName = {
    [50000] = "_LocFengBaoZhiLing", -- 风暴之灵
    [50001] = "_LocDaDiZhiLing", -- 大地之灵
    [50002] = "_LocLieYanZhiLing", -- 烈焰之灵
    [50003] = "_LocHaiYangZhiLing", -- 海洋之灵
    [50176] = "_LocXiongMaoYongShi", -- 熊猫勇士
    [50096] = "_LocGangFengMoYing", -- 罡风魔影
    [50120] = "_LocLaYanFengHuang", -- 蓝炎凤凰
    [50188] = "_LocJinGuiJiaShi", -- 金龟甲士
    [50035] = "_LocHeiHuangDieJing", -- 黑煌蝶精
    [50094] = "_LocHuoYanMoYing", -- 火焰魔影
}
function PetBefallView:PetBefallView()
    self.ht = PublicPopupCtrl.Instance:AddInterceptRollingFunc(function(protocol)
        if ViewMgr:IsOpen(PetBefallView) == true then
            local role_id = RoleData.Instance:GetRoleId()
            if string.find(protocol.msg,tostring(role_id),1) ~= nil then
                ServerActivityData.Instance:SetPetBefallMarqueeMsg(
                    {
                        str = RichTextHelper.ParseAll(protocol.msg, false, RTParseType.TopLayerMarquee),
                        delay_time = PublicPopupCtrl.Instance.top_layer_marquee_data:NeedDelay(protocol.msg),
                        msg = protocol.msg
                    }
                )
                return true
            end
        end
    end)
end

function PetBefallView:CloseCallback()
    PublicPopupCtrl.Instance:RemoveInterceptRollingFunc(self.ht)
end

function PetBefallView:OnCloseClick()
    ViewMgr:CloseView(PetBefallView)
end

PetBefallPanel = PetBefallPanel or DeclareMono("PetBefallPanel",UIWFlushPanel)

function PetBefallPanel:PetBefallPanel()
    self.data = ServerActivityData.Instance
    self.cfg = self.data:GetPetBefallCfg()
    self.stuff_id = self.cfg.jackpot_configure[1].item_id
    self.need_stuff_num = self.cfg.jackpot_configure[1].item_num
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end
    self.uptate_model_time = 10
    self.show_models = self.data:GetPetBefallShowData()
    self.show_model_index = 1
    if #self.show_models > 1 then
        self.ModelIndexList:SetData(self.show_models)
        self.ModelIndexList:SetectItem(self.show_model_index,true)
    elseif #self.show_models == 1 then
        self:ShowModel()
    end
    self.item_index = 1
    self.data_cares = {
        {data = self.data:GetPetBefallData(),func = self.FlushDial,init_call = false,keys = {"info"}},
        {data = self.data:GetPetBefallData(),func = self.FlushEffect,init_call = false,keys = {"result"}},
        {data = BagData.Instance.item_grid,func = self.FlushStuff,init_call = false},
    }
    if Item.GetNum(self.stuff_id) >= self.need_stuff_num then
        self.data:RecordPetBefallRemind(RoleData.Instance:GetRoleId())
    end
    self:SetRemaindTime()
end

function PetBefallPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Items[self.item_index]:SetSelected(true)
    self:FlushPanel()
end

function PetBefallPanel:SetRemaindTime()
    self.ActTime:CloseCountDownTime()
    self.ActTime:SetShowCallBack(function (t)
        local t_str = ""
		if t.day > 0 then
			t_str = string.format(Language.PetBefall.Time1,t.day, t.hour,t.min)
		else
			t_str = string.format(Language.PetBefall.Time2,t.hour,t.min)
		end
        return t_str
	end)
	self.ActTime:CreateActTime(ACTIVITY_TYPE.RAND_PET_BEFALL,TimeType.Type_Special_4)
end

function PetBefallPanel:FlushPanel()
    self:FlushDial()
    self:FlushStuff()
end

function PetBefallPanel:FlushDial()
    -- 执行转盘动画时不刷新奖池信息
    if self.playing then
        return
    end
    local data = self.data:GetPetBefallData()
    local item_list_data = self.data:GetPetBefallGiftData()
    for i = 1 , self.Items:Length() do
        if item_list_data[i] then
            self.Items[i]:SetData(item_list_data[i])
        end
    end
    local reward_item_id = self.data:GetPetBefallPoolItemId()--self.cfg.jackpot_configure[1].jackpot_item_id
    self.RewardCell.gameObject:SetActive(reward_item_id > 0)
    self.GoldIconObj:SetActive(reward_item_id == 0)
    local pool_num = data.info.pool_gold_num
    if reward_item_id > 0 then
        self.RewardCell:SetData(Item.Create({item_id = reward_item_id}))
        pool_num = data.info.pool_item_num
    end
    UH.SetText(self.RewardPoolNum,pool_num)
end

function PetBefallPanel:ShowModel()
    if #self.show_models < 1 then
        LogError("《神兽降临》活动展示神兽的个数为0，请检查！GiftCfg = ",self.data:GetPetBefallGiftData())
        return
    end 
    if #self.show_models > 1 then
        TimeHelper:CancelTimer(self.time_func)
        self.time_func = Invoke(BindTool.Bind(self.AutoToggle,self),self.uptate_model_time)
    end
    local pet_id = self.show_models[self.show_model_index].pet_id
    local pet_cfg = Cfg.PetById(pet_id)
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet_cfg.res_id))
    self.ui_obj:PlayAnim(ChAnimType.Show)
    if PetBefallView.PetImgName[pet_id] then
        self.NameObj:SetActive(true)
        UH.SpriteName(self.NameIcon,PetBefallView.PetImgName[pet_id])
    else
        self.NameObj:SetActive(false)
    end
end

function PetBefallPanel:AutoToggle()
    self.show_model_index = self.show_model_index + 1
    self.show_model_index = self.show_model_index > #self.show_models and 1 or self.show_model_index
    self.ModelIndexList:SetectItem(self.show_model_index,true)
end

function PetBefallPanel:FlushStuff()
    local icon_id = Item.GetIconId(self.stuff_id)
    UH.SetIcon(self.StuffIcon,tostring(icon_id))
    UH.SetIcon(self.BuyIcon,tostring(icon_id))
    UH.SetText(self.StuffNum,Item.GetNum(self.stuff_id) .. "")
    UH.SetText(self.BuyName,Item.GetQuaName(self.stuff_id))
end

function PetBefallPanel:FlushEffect()
    local result = self.data:GetPetBefallData().result
    if result and next(result) then
        self:PlayDialEffect(result.seq + 1)
    end
end

function PetBefallPanel:OnDestroy()
    TimeHelper:CancelTimer(self.time_func)
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil
    if self.item_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
        self.item_timer = nil
    end
    self.ActTime:CloseCountDownTime()

     -- 关闭的时候注销拦截，清空拦截到的消息
    if self.playing then
        self:DialEffectEndFunc()
    end
    ServerActivityData.Instance:SetPetBefallMarqueeMsg(nil)
    SocietyData.Instance.is_mail_shield_tip = false

    UIWFlushPanel.OnDestroy(self)
end

function PetBefallPanel:OnNextModelClick(data)
    self.show_model_index = self.ModelIndexList:GetSelectedIndex(data)
    self:ShowModel()
end

function PetBefallPanel:OnPlayClick()
    if self.playing then
        return
    end
    local stuff_num = Item.GetNum(self.stuff_id)
    if stuff_num >= self.need_stuff_num then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_PET_BEFALL,0)
        SocietyData.Instance.is_mail_shield_tip = true
        self.pet_bag_remain_num = PetData.Instance:GetBagRemainNum()
    else
        PublicPopupCtrl.Instance:Error(string.format(Language.PetBefall.StuffTip,Item.GetName(self.stuff_id)))
        ViewMgr:OpenView(PetBefallStoreView)
    end
end

function PetBefallPanel:OnPetInfoClick()
    local pet_id = self.show_models[self.show_model_index].pet_id
    local pet = Pet.New({pet_id = pet_id})
    --ViewMgr:OpenView(PetDetailView,pet)
    PetCtrl.Instance:OpenActPetDetailView(pet)
end

function PetBefallPanel:OnHelpClick()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[99].desc})
end

function PetBefallPanel:OnStartDownClick()
    --self.PlayEffectObj:SetActive(false)
end

function PetBefallPanel:OnStartUpClick()
    --self.PlayEffectObj:SetActive(true)
end

-- 转盘动效，index转到哪个item上
function PetBefallPanel:PlayDialEffect(index)
    if self.NeedAnimaToggle.isOn == true then
        self.Items[index]:SetSelected(true)
        self:DialEffectEndFunc()
        return
    end
    self.PlayEffectObj:SetActive(false)
    self.playing = true
    self.speed = 0
    self.start_speed = 0            -- 开始速度
    self.min_speed = 0.08            -- 最小速度
    self.max_speed = 0.4              -- 最大速度
    self.accelerated_speed = 0.02   -- 加速度
    self.process_time = 3           -- 过程时间
    self.start_time = Time.time
    self.is_stop = false
    if self.item_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
    end
    self.item_timer = Runner.Instance:RunUntilTrue(BindTool.Bind(self.DialEffect,self,index))
end

function PetBefallPanel:DialEffect(index)
    -- 减速处理
    if Time.time - self.start_time >= self.process_time and not self.is_stop and self.accelerated_speed > 0 then
        self.accelerated_speed = -self.accelerated_speed
    end
    if not self.is_stop then
        self.speed = self.speed + self.accelerated_speed
        if self.accelerated_speed < 0 then
            -- 减到最小速度后，开始锁定目标吧
            if self.speed <= self.min_speed then
                self.speed = self.min_speed
                self.is_stop = true
            end
        else
            self.speed = self.speed > self.max_speed and self.max_speed or self.speed
        end
    end
    self.start_speed = self.start_speed + self.speed
    if self.start_speed >= 1 then
        self.item_index = self.item_index + 1
        self.item_index = self.item_index > self.Items:Length() and 1 or self.item_index
        -- UI出的图不是很精密，光标旋转的话有缝隙出来。item也比较少，所以对好item，使用Toggle实现转盘效果
        self.Items[self.item_index]:SetSelected(true)
        self.start_speed = 0
        -- 转盘停止
        if self.is_stop == true and self.item_index == index then
            self:DialEffectEndFunc()
            return true
        end
    end
end

function PetBefallPanel:DialEffectEndFunc()
    self.playing = nil
    self.PlayEffectObj:SetActive(true)
    self:FlushDial()
    local result = self.data:GetPetBefallData().result
    if result == nil or next(result) == nil then
        LogError("神兽降临转盘结果展示有问题，需要检查 result == nil")
        return
    end
    if result.gold_num > 0 then
        table.insert(result.items,{item_id = CommonItemId.Gold,num = result.gold_num})
    end
    self.data:GetPetBefallData().result = nil
    for i,v in pairs(result.items) do
        local marque_msg = self.data:GetPetBefallMarqueeMsg()
        if marque_msg ~= nil then
            PublicPopupCtrl.Instance:Marquee(marque_msg.str,marque_msg.delay_time)
            ChatCtrl.Instance:ChannelChatCustom{content = marque_msg.msg}
            self.data:SetPetBefallMarqueeMsg(nil)
        end
        if SocietyData.Instance.mail_shield_tip_list and next(SocietyData.Instance.mail_shield_tip_list) then
            SocietyData.Instance:ReleaseMailShieldTip()
            return
        end
        local item = Item.Create(v)
        if Item.GetShowType(v.item_id) == ShowType.Pet then
            if self.pet_bag_remain_num == nil or self.pet_bag_remain_num <= 0 then
                PublicPopupCtrl.Instance:Center(Language.Society.MailTips)
                return
            end
            ViewMgr:OpenView(PetGain, {is_xilian = false})
        else
            PublicPopupCtrl.Instance:CenterI({item_id = v.item_id , num = v.num,color_str = Item.GetColorStr(v.item_id)})
        end
        ChatCtrl.Instance:ChannelChatCustom({item_get = {item = item, num = v.num, is_grey = true,rumor_th = {state = ActStatusType.Open}}})
    end
    SocietyData.Instance.is_mail_shield_tip = false
end

function PetBefallPanel:OnBuyStuffClick()
    -- local mod_key = self.data:GetPetBefallStuffSource()
    -- if mod_key then
    --     ViewMgr:OpenViewByKey(mod_key)
    -- else
    --     PublicPopupCtrl.Instance:Error(Language.PetBefall.BuyStuffTip)
    -- end
    ViewMgr:OpenView(PetBefallStoreView)
end

function PetBefallPanel:OnStuffIconClick()
    local info = {
        item_list = {Item.Create({item_id = self.cfg.jackpot_configure[1].item_id})}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
    ItemInfoData.Instance:SetClickDataPos(UH.GetRootPosition(self.BuyIcon.gameObject))
end

PetBefallRewardItem = PetBefallRewardItem or DeclareMono("PetBefallRewardItem",UIWidgetBaseItem)

function PetBefallRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.NumItem.gameObject:SetActive(data.reward_per > 0)
    if data.reward_per > 0 then
        self.NumItem:ShowNum(data.reward_per)
    else
        if data.gold_num > 0 then
            self.Cell:SetData(Item.Create({item_id = CommonItemId.Gold,num = data.gold_num}))
        else
            self.Cell:SetData(Item.Create(data.reward_item))
        end
    end
    if self.Rate then
        UH.SetText(self.Rate, string.format("%s%%", data.rate))
    end
end

PetBefallRewardNumItem = PetBefallRewardNumItem or DeclareMono("PetBefallRewardNumItem",UIWidgetBaseItem)
function PetBefallRewardNumItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self:ShowNum(data.num)
end

function PetBefallRewardNumItem:ShowNum(num)
    num = num or 0
    local nums = {[1] = 0}
    local img_len = self.ScaleImgs:Length()
    for i = 1, img_len do
        if num == 0 then
            break
        end
        nums[i] = num % 10
        num = (num - nums[i]) / 10
    end
    local is_per = false
    for i = 1 , img_len do
        local va = nums[#nums - i + 1]
        if va then
            local img_name = string.format("_%s",va)
            UH.SpriteName(self.ScaleImgs[i],img_name)
            self.ScaleImgs[i].gameObject:SetActive(true)
        elseif is_per == false then
            UH.SpriteName(self.ScaleImgs[i],"_%")
            is_per = true
            self.ScaleImgs[i].gameObject:SetActive(true)
        else
            UH.SpriteName(self.ScaleImgs[i],"")
            self.ScaleImgs[i].gameObject:SetActive(false)
        end
    end
end

PetBefallModelIndexItem = PetBefallModelIndexItem or DeclareMono("PetBefallModelIndexItem",UIWidgetBaseItem)

function PetBefallModelIndexItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
end

-- 神兽降临直购商店
PetBefallStoreView = PetBefallStoreView or DeclareView("PetBefallStoreView", "serveractivity/pet_befall_store_view")
VIEW_DECLARE_MASK(PetBefallStoreView,ViewMask.BlockClose)
function PetBefallStoreView:PetBefallStoreView()
    self.data = ServerActivityData.Instance
    self.ht = self.data.pet_befall_store_data:Care(BindTool.Bind(self.FlushPanel,self))
    self.ht_2 = ServerActivityData.Instance:GetCmdBuyItemCheckRet():Care(BindTool.Bind(self.SendBuy,self))
end

function PetBefallStoreView:LoadCallback()
    self.cfg = self.data:GetPetBefallStoreCfg()
    UH.SetText(self.FeeText, self.cfg.buy_money / 10)
    for i = 1, 3 do
        local item = Item.Create(self.cfg.reward_item[i - 1])
        self.Cells[i]:SetData(item)
        UH.SetText(self.CellNames[i], item:Name())
        if i == 1 then
            UH.SetText(self.CellNum, self.cfg.reward_item[i - 1].num)
        end
    end

    self:FlushPanel()
end

function PetBefallStoreView:FlushPanel()
    if self.cfg == nil then
        return
    end
    local limit_str = self.cfg.limit_type == 1 and Language.PetBefall.StoreLimit1 or Language.PetBefall.StoreLimit2
    local limit_num = self.cfg.limit_type == 1 and self.cfg.buy_times or self.cfg.today_times
    local progress_num = self.cfg.limit_type == 1 and self.data.pet_befall_store_data.act_buy_times or self.data.pet_befall_store_data.day_buy_times or 0
    self.limit_num = limit_num
    self.progress_num = progress_num
    local num_str = ColorStr(string.format("%s/%s", limit_num - progress_num, limit_num), COLORSTR.Green2)
    UH.SetText(self.LimitCountDesc,string.format(limit_str, num_str))

    self.Interactor.Interactable = progress_num < limit_num
end

function PetBefallStoreView:OnCloseClick()
    ViewMgr:CloseView(PetBefallStoreView)
end

function PetBefallStoreView:OnBuyClick()
    if self.progress_num >= self.limit_num then
        PublicPopupCtrl.Instance:Error(Language.PetBefall.StoreTip)
        return
    end
    local fee = self.cfg.buy_money / 10
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_PET_BEFALL_AUTO, fee, ACTIVITY_TYPE.RAND_PET_BEFALL, 1, self.cfg.seq, 0, 0)
end

function PetBefallStoreView:CloseCallback()
    self.data.pet_befall_store_data:Uncare(self.ht)
    ServerActivityData.Instance:GetCmdBuyItemCheckRet():Uncare(self.ht_2)
end

function PetBefallStoreView:SendBuy()
    if self.cfg == nil then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_PET_BEFALL_AUTO, self.cfg.buy_money)
end