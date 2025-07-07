
ClothAromaView = ClothAromaView or DeclareView("ClothAromaView","serveractivity/cloth_aroma", Mod.RandActivity.ClothAroma)
function ClothAromaView:ClothAromaView()
    self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
end 
VIEW_DECLARE_LEVEL(ClothAromaView, ViewLevel.L0)
function ClothAromaView:LoadCallback()
    ClothAromaCtrl.Instance:SendInfo()
end

function ClothAromaView:OnClickClose()
    ViewMgr:CloseView(ClothAromaView)
end

ClothAromaPanel = ClothAromaPanel or DeclareMono("ClothAromaPanel", UIWFlushPanel)
function ClothAromaPanel:ClothAromaPanel()
    self.data = ClothAromaData.Instance
    self.ctrl = ClothAromaCtrl.Instance
    self.data_cares = {
        {data = self.data.reward_info, func = self.FlushPanel,init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(),func = self.SendBuy, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
    }

    self.step_count = 5
    self:ShowModel()

    self.LeftModAnim:ListenEvent("anim_state",
    function (eve_param,anim_st_info)
        if eve_param == "change_mod" then 
            self:ChangeMod()
        end 
    end)

    -- LogError("!@?",self.data:GetItemNum())

    UH.SetText(self.item_num, self.data:GetItemNum())
    UH.SetText(self.times_num, self.data:GetDrawItemNum())
    self.btn_draw:SetActive(not self.data:DrawComplete() and self.data:GetItemEnough())
    self.btn_buy:SetActive(not self.data:DrawComplete() and not self.data:GetItemEnough())

    self.cfg = self.data:GetBuyCfg()
    local price = self.cfg.price
    local sprite_name
    if self.cfg.price_type == 3 then
        price = price / 10
        sprite_name = "99999"
    else
        sprite_name = HuoBi[self.cfg.price_type] .. ""
    end

    UH.SetText(self.BuyPrice,price)
    UH.SpriteName(self.BuySp, sprite_name)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_JIN_YI_TIAN_XIANG,TimeType.Type_Time_8,Language.ClothAroma.ActTime)
end 

function ClothAromaPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self, "ui_obj_show")
    Delete(self, "ui_role_show")
  
    if self.ht ~= nil then
        TimeHelper:CancelTimer(self.ht)
        self.ht = nil
    end

    self.data:ClearMark()
end

function ClothAromaPanel:FlushPanel(param)
    if self.init == nil then 
        self.init = true
        self.roll_mod:FlushView()
        return 
    end 

    if self.data:IsMarking() and param.flush ~= nil then 
        self:ShowAnimShow()
    end 

    UH.SetText(self.item_num, self.data:GetItemNum())
    UH.SetText(self.times_num, self.data:GetDrawItemNum())
    
    -- self.btn_draw:SetActive(not self.data:DrawComplete())
    self.btn_draw:SetActive(not self.data:DrawComplete() and self.data:GetItemEnough())
    self.btn_buy:SetActive(not self.data:DrawComplete() and not self.data:GetItemEnough())


    self.cfg = self.data:GetBuyCfg()
    local price = self.cfg.price
    local sprite_name
    if self.cfg.price_type == 3 then
        price = price / 10
        sprite_name = "99999"
    else
        sprite_name = HuoBi[self.cfg.price_type] .. ""
    end

    UH.SetText(self.BuyPrice,price)
    UH.SpriteName(self.BuySp, sprite_name)
end 

function ClothAromaPanel:OnToggleAuto()
    local is_active = not GetActive(self.AutoImg)
    self.AutoImg:SetActive(is_active)
end

function ClothAromaPanel:ShowAnimShow()
    local jump_flag = GetActive(self.AutoImg)

    if jump_flag then 
        self.roll_mod:JumpAnimShow()
        return 
    end 
    self.roll_mod:ShowAnim()
end

-- 点击开箱
function ClothAromaPanel:OnClickDraw()
    if not self.data:GetItemEnough() then 
        -- ViewMgr:OpenView(ClothAromaBuyTips)

        -- local info = {
        --     content = Language.ClothAroma.NeedBuy,
        --     cancel = {
        --         name = nil,
        --         func = function ()
        --             PublicPopupCtrl.Instance:CloseDialogTips()
        --         end
        --     },
        --     confirm = {
        --         name = nil,
        --         func = function ()
        --             self.ctrl:SendBuy()            
        --             PublicPopupCtrl.Instance:CloseDialogTips()
        --         end
        --     }
        -- }
        -- PublicPopupCtrl.Instance:DialogTips(info)
    else 
        -- LogError("num "..Item.GetNum(27847))
        self.roll_mod:StartShield()
        self.data:MarkTarget()
        self.ctrl:SendDraw()
    end 
end
function ClothAromaPanel:OnClickBuy()
    if not self.data:GetItemEnough() then 
        ViewMgr:OpenView(ClothAromaBuyTips)
    end
end

function ClothAromaPanel:SendBuy()
    -- LogError("!?@")
    local cfg = self.data:GetDrawCfg()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_JIN_YI_TIAN_XIANG, cfg.price)
end

function ClothAromaPanel:OnClickInfo(data)
    local info = {
		item_list = {data.item},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
    -- local co = Cfg.SurfaceInfoByActiveStuffId(data.item.item_id)
    -- local attr_list = IllusionData.Instance:GetAttrList(co.id)
	-- self.AttrList:SetData(attr_list)
	-- self.AttrListObj:SetActive(true)
end

function ClothAromaPanel:OnClickCoin()
    local coin_item = Item.Init(Config.jinyitianxiang_auto.other[1].reward_item.item_id)
    local info = {
		item_list = {coin_item},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function ClothAromaPanel:OnToggleAuto()
	local is_active = not GetActive(self.AutoImg)
    
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	self.AutoImg:SetActive(is_active)
end

function ClothAromaPanel:OnClickModCheck()
    if self.mod_list[self.mod_index].item.item_id <= 0 then 
        return 
    end 

    local item_info = self.mod_list[self.mod_index].item
    local detail = {
        item_list = {item_info}
    }
    ItemInfoCtrl.Instance:ItemInfoView(detail)
end

function ClothAromaPanel:ShowModel()
    if self.ui_obj_show == nil then
        self.ui_obj_show = UIObjDrawer.New()
        self.mod_obj_show:SetShowData({
            ui_obj = self.ui_obj_show,
            view = self,
            show_shadow = false,
            seq = 1,
        })
    end

    if self.ui_role_show == nil then
        self.ui_role_show = UIChDrawer.New()
        self.mod_role_show:SetShowData({
            ui_obj = self.ui_role_show,
            view = self,
            show_shadow = false,
            seq = 2,
        })
    end
    
    self.mod_list = self.data:GetModShowList()
    self.mod_index = 1
    self.update_count = 0
    self.ht = TimeHelper:AddRunTimer(BindTool.Bind(self.UpdataTime, self), 1)
    self:ChangeMod()
end

function ClothAromaPanel:UpdataTime()
    local change = false
    if self.update_count > self.step_count then 
        self.update_count = 0
        self.mod_index = self.mod_index + 1
        change = true
    else
        self.update_count = self.update_count + 1
    end 

    if not change then 
        return 
    end 

    if self.mod_index > #self.mod_list then 
        self.mod_index = 1
    end 

    self.LeftModAnim:SetTrigger(APH("change"))
end

function ClothAromaPanel:ChangeMod()
    local vo = self.mod_list[self.mod_index]
    self.mod_role_obj:SetActive(vo.type == 2)
    self.mod_obj_obj:SetActive(vo.type == 1)

    if vo.type == 1 then 
        self.ui_obj_show:SetData(vo.res)
    else 
        self.ui_role_show:SetData(vo.appearance)
    end     

    local name_orig = self.mod_list[self.mod_index].item:Name()
    local name_list = DataHelper.GetStringWordToTable(name_orig)
    local name_v = ""
    for i = 1,#name_list do 
        name_v = name_v..name_list[i]

        if i ~= #name_list then 
            name_v = name_v.."\n"
        end 
    end 

    UH.SetText(self.mod_name, name_v)
end

function IllusionExchangeViewOffset:OnClickAttrList(data)
	local attr_list = self.data:GetAttrList(data.id)
	self.AttrList:SetData(attr_list)
	self.AttrListObj:SetActive(true)
end

function ClothAromaPanel:OnClickCloseAttr()
    self.AttrListObj:SetActive(false)
end

ClothAromaRoll = ClothAromaRoll or DeclareMono("ClothAromaRoll", UIWFlushPanel)
function ClothAromaRoll:ClothAromaRoll()
    self.data = ClothAromaData.Instance
    -- self.data_cares = {
    --     {data = self.data.reward_info,func = self.FlushView,init_call = false},    
    -- }

    local list = self.data:GetFixShowList()
    local status_list = self.data:GetCellsStatus()
    for i= 1,9 do 
        self.cells[i]:SetData(list[i])
        self.cells[i]:SetSelect(false)
        self.cells[i]:SetComplete(status_list[i].got)
    end 
end 
function ClothAromaRoll:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.handle_jump ~= nil then
        TimeHelper:CancelTimer(self.handle_jump)
        self.handle_jump = nil
    end  
    if self.show_timer ~= nil then
        TimeHelper:CancelTimer(self.show_timer)
        self.show_timer = nil
    end  
end 

function ClothAromaRoll:FlushView()
    local status_list = self.data:GetCellsStatus()
    for i= 1,9 do 
        self.cells[i]:SetSelect(false)
        self.cells[i]:SetComplete(status_list[i].got)
    end 
end

function ClothAromaRoll:JumpAnimShow()
    local target = self.data:CheckTarget()
    for i= 1,9 do 
        self.cells[i]:SetSelect(target == i)
    end 
    if self.handle_jump ~= nil then
        TimeHelper:CancelTimer(self.handle_jump)
        self.handle_jump = nil
    end  
    self.block:SetActive(true)
    self.handle_jump = TimeHelper:AddDelayTimer(function ()
        self:FlushView()
        self:FlyText()
        self.block:SetActive(false)
        self.data:ClearMark()
    end, 2)
end

function ClothAromaRoll:FlyText()
    local list = self.data:GetShowList()
    local target = self.data:CheckTarget()
    
    local item_info = Item.Create(list[target].reward_item)
    PublicPopupCtrl.Instance:CenterI({item_id = item_info.item_id, num = item_info.num,
    color_str = item_info:ColorStr(),item = item_info,icon_id = item_info:IconId()})
    ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item_info, num = item_info.num}}

    self:ClearShield()
end

function ClothAromaRoll:ShowAnim()
    if self.is_animing then 
        return 
    end 

    self.is_animing = true
    local target = self.data:CheckTarget()
    local roll_show_list = self:SimulateAnim(target)

    self.block:SetActive(true)

    local cur_index = 0
    local time_step = 0.1
    local total_time = time_step*#roll_show_list
    local pass_time = 0
    local pass_auido = false
    self.show_timer = TimeHelper:AddCountDownTT(function()
        if self.cells ~= nil then 
            cur_index = cur_index + 1
            for i = 1,9 do 
                self.cells[i]:SetSelect(roll_show_list[cur_index] == i)  
            end 
            pass_time = time_step*cur_index

            if (total_time - pass_time) <= 3 and not pass_auido then 
                AudioMgr:PlayEffect(AudioType.UI, "gongfangzhizao")
                pass_auido = true
            end 
        end 
    end,
    function ()
        if self.cells ~= nil then 
            for i = 1,9 do 
                self.cells[i]:SetSelect(target == i)
            end 

            
            if self.handle_jump ~= nil then
                TimeHelper:CancelTimer(self.handle_jump)
                self.handle_jump = nil
            end  
            self.handle_jump = TimeHelper:AddDelayTimer(function ()
                self:FlushView()
                self:FlyText()
                self.data:ClearMark()
                self.block:SetActive(false)
                self.is_animing = false
            end, 2)
        end 
    end,0.1*#roll_show_list,0.1)
end

-- 上锁
function ClothAromaRoll:StartShield()

    SocietyData.Instance.is_mail_shield_tip = true
    AppearanceCtrl.Instance.is_shield_appear_show = true
    RoleCtrl.Instance.is_shield_title_show = true
end

-- 解除屏蔽
function ClothAromaRoll:ClearShield()
    SocietyData.Instance.is_mail_shield_tip = false
    if SocietyData.Instance.mail_shield_tip_list and next(SocietyData.Instance.mail_shield_tip_list) then
        SocietyData.Instance:ReleaseMailShieldTip()
    end
    AppearanceCtrl.Instance:ReleaseShieldParam()
    RoleCtrl.Instance:ReleaseShieldTitle()
end

function ClothAromaRoll:SimulateAnim(show_seq)
    local status_list = self.data:GetCellsMarkStatus()
    local simulate_list = {}
    -- 正常旋转两圈
    for ex = 1,20 do 
        for j = 1,2 do 
            for i = 1,9 do 
                if status_list[i].got == false then 
                    table.insert(simulate_list,i)
                end 
            end 
        end 
        if #simulate_list >30 then 
            break
        end
    end
    -- 悬停上层
    -- if show_seq <= 5 then 
        for i = 1,show_seq do 
            -- for j = 1,2 do 
                if status_list[i].got == false then 
                    table.insert(simulate_list,i)
                end 
            -- end 
        end 
    -- 悬停下层
    -- else
    --     for i = 1,5 do 
    --         if status_list[i].got == false then 
    --             table.insert(simulate_list,i)
    --         end
    --     end 
    --     for i = 5,show_seq do 
    --         -- for j = 1,2 do 
    --             if status_list[i].got == false then 
    --                 table.insert(simulate_list,i)
    --             end 
    --         -- end 
    --     end 
    -- end 
    -- LogError("!?@",simulate_list)
    return simulate_list
end


ClothAromaRollCell = ClothAromaRollCell or DeclareMono("ClothAromaRollCell", UIWidgetBaseItem)
function ClothAromaRollCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local flag_show_pic = false
    if data.mod ~= nil and data.mod.ill_show <= IllusionConfig.FashionType.mount then 
        local png_path = IllusionData.Instance:GetShowPngPath{
            id = data.mod.res_info.id, 
            type = data.mod.ill_show, 
            avatar_type = RoleData.Instance:GetRoleAvatarType(), 
            prof_type = RoleData.Instance:ProfType()
        }
        flag_show_pic = png_path ~= nil
        self.pic_show.gameObject:SetActive(flag_show_pic)
	    if flag_show_pic then  
            self.pic_show:SetByPath(png_path)
        end
    elseif data.mod ~= nil then 
        -- self.pic_show.gameObject:SetActive(false)
        local png_path
        if data.mod.ill_show == IllusionConfig.FashionType.partner then 
            png_path = string.format(PartnerConfig.PngPath, data.mod.res_info.partner_skin_id)
        elseif data.mod.ill_show == IllusionConfig.FashionType.pet then 
            png_path = string.format("pet_skin/%s.png", data.mod.res_info.res_id)
        end 
        flag_show_pic = png_path ~= nil
        self.pic_show.gameObject:SetActive(flag_show_pic)
        if flag_show_pic then  
            self.pic_show:SetByPath(png_path)
        end
    end 

    self.item_show.gameObject:SetActive(not flag_show_pic)
    if not flag_show_pic then 
        self.item_show:SetData(data.item)
    end 
    if data.mod then 
        UH.SetText(self.title, Language.ClothAroma.type[data.mod.ill_show])
    end 
    -- if self.btn_info ~= nil then 
    --     self.btn_info:SetActive(data.mod.ill_show <= IllusionConfig.FashionType.mount)
    -- end 
    if not StringIsEmpty(data.cfg.show_rate) then
        UH.SetText(self.ShowRateText, data.cfg.show_rate)
    end
end

function ClothAromaRollCell:OnRelease()
    UIWidgetBaseItem.OnRelease(self)

end

function ClothAromaRollCell:SetSelect(flag)
    self.select:SetActive(flag)
end

function ClothAromaRollCell:SetComplete(flag)
    self.got:SetActive(flag)
end

function ClothAromaRollCell:OnClickShow()
    local info = {
		item_list = {self.data.item},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end