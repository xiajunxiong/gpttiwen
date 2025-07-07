PartnerViewGift = PartnerViewGift or DeclareMono("PartnerViewGift", UIWFlushPanel)

function PartnerViewGift:PartnerViewGift()
    self.data = PartnerCtrl.Instance:Data()

    self.data_cares = {
        {data = self.data.item_flush, func = self.onFlush, init_call = false},
    }
    self.data.gift_flush.cur_select = 1
end 

function PartnerViewGift:OnEnable()
    self.data.cur_sel_tabbar.val = PartnerConfig.PartnerViewTabs.Gift
end

-- function PartnerViewGift:Awake()
--     self:onFlush()
-- end
function PartnerViewGift:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    
    if self.auto_cancel_block ~= nil then
        TimeHelper:CancelTimer(self.auto_cancel_block)
    end
end

function PartnerViewGift:onFlush()
    -- self.ProfPanel:ChangeProf()
    self.ProfPanel:ChangeFlushSelect(1)
    -- self.InfoPanel:Init()
end

function PartnerViewGift:OnClickBtnView()
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ClickGeneral2)
    ViewMgr:OpenView(PartnerUpGiftView)
end

function PartnerViewGift:OnClickBtnReset()
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ClickGeneral2)
    if PartnerData.Instance:CheckGiftCanBeReset() then return end 
    ViewMgr:OpenView(PartnerGiftResetView)
end

function PartnerViewGift:OnClickOnes()
    self:PlayClickAudio()
    if PartnerData.Instance:CheckGiftEnoughWithPop() then return end 

    self.block:SetActive(true)
    self.ProfPanel:PlayFlyEffect()

    -- 强化or激活
    self.mark = 1
end

function PartnerViewGift:OnClickEnds()
    self:PlayClickAudio()
    if PartnerData.Instance:CheckGiftEnoughWithPop() then return end 

    self.block:SetActive(true)
    self.ProfPanel:PlayFlyEffect()

    -- 一键强化
    self.mark = 2
end

function PartnerViewGift:PlayClickAudio()
    local level = self.data:GetPartnerGift(nil,self.data.gift_flush.cur_select)
    if level > 0 then 
        AudioMgr:PlayEffect(AudioType.UI,CommonAudio.SkillLevelUp)
    else 
        AudioMgr:PlayEffect(AudioType.UI,CommonAudio.SkillGetNew)
    end 
end

function PartnerViewGift:EffectFlyComplete()
    if  self.mark == 1 then 
        PartnerCtrl.Instance:SendPartnerGiftLevel(self.data.cur_sel_partner,self.data.gift_flush.cur_select)
        self.InfoPanel.mark = true
    elseif self.mark == 2 then 
        for i = 1,self.data:GetOnesTimeGift() do 
            PartnerCtrl.Instance:SendPartnerGiftLevel(self.data.cur_sel_partner,self.data.gift_flush.cur_select)
            if i == self.data:GetOnesTimeGift() then 
                self.InfoPanel.mark = true
            end 
        end 
    end

    for i = 1,3 do 
	    self.ProfPanel.effect_list[i]:AllHandleCancel()
	end

    TimeHelper:CancelTimer(self.auto_cancel_block)
    self.auto_cancel_block = TimeHelper:AddDelayTimer(function () 
        self:OnAttrLightComplete()
    end,5)
end

function PartnerViewGift:OnAttrLightComplete()
    if self.auto_cancel_block ~= nil then
        TimeHelper:CancelTimer(self.auto_cancel_block)
    end
    self.block:SetActive(false)    
end

-- 天赋板块
PartnerGiftProfPanel = PartnerGiftProfPanel or DeclareMono("PartnerGiftProfPanel", UIWFlushPanel)
function  PartnerGiftProfPanel:PartnerGiftProfPanel()
    self.data = PartnerCtrl.Instance:Data()

    self.data_cares = {
        {data = self.data.item_flush, func = self.OnFlushProfView },--init_call = false},
        {data = self.data.gift_flush, func = self.OnFlushProfView, keys = {"call"},init_call = false},
        {data = self.data.gift_flush, func = self.ChangeFlyEffEnds ,init_call = false},
        {data = BagData.Instance.item_grid,func = self.OnFlushProfView, init_call = false},
        {data = self.data.item_flush, func = self.OnFlushRemind },
    }

    self.init_flag = false

    self.partner_id_anim = 0
    
    -- FlyStarEffectCell
    local param_t = {}
        
    for i = 1,3 do 
        if i < 3 then 
            self.effect_list[i]:SetData({mono = self})
        else 
            self.effect_list[i]:SetData({mono = self,end_func = function ()
                self.EffectTool:Play(2161055,self.effect_list[i].EndPos)
            end})
        end 
    end 
    self.timer = {}
    self.cur_job = 1

end 

function PartnerGiftProfPanel:Init()
    self:OnClickPoint(1)
end

function PartnerGiftProfPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end

    for i = 1,3 do 
        if self.timer[i] then 
            TimeHelper:CancelTimer(self.timer[i])
            self.timer[i] = nil
        end 
    end 
end

function PartnerGiftProfPanel:ChangeFlyEffEnds()
    if self.data.gift_flush.cur_select > 10 then return end 
    for i = 1,3 do 
        if self.timer[i] then 
            TimeHelper:CancelTimer(self.timer[i])
            self.timer[i] = nil
        end 

        UH.LocalPosG( self.effect_list[i].EndPos, 
            self.prof_list[self.cur_job].star_list[self.data.gift_flush.cur_select].gameObject:GetLocalPosition())
    end 
end

--刷新红点提示
function PartnerGiftProfPanel:OnFlushRemind()
    local partner_info = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if partner_info == nil then return end 
    local cur_remind = partner_info:GetGiftRemind()
    self.remind_gou:SetActive(cur_remind)
end

function PartnerGiftProfPanel:OnClickRemind()
    local partner_info = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if partner_info == nil then return end 
    local cur_remind = partner_info:GetGiftRemind()
    partner_info:SetGiftRemind(not cur_remind)
    self.data:PartnerGiftRedPoint()
    
    self:OnFlushRemind()
end

function PartnerGiftProfPanel:PlayFlyEffect()
    for i = 1,3 do 
        self.timer[i] = TimeHelper:AddDelayTimer(function()
            if not ViewMgr:IsOpen(PartnerView) then return end 
            
            self.effect_list[i]:Play(2161054)
        end,0.1*(i-1))
    end 
end

-- 转换职业
-- 更新界面的布局和技能展示
function PartnerGiftProfPanel:ChangeProf()
    if self.data.cur_sel_partner < 0 then return end 
    local partner_info = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    self.cur_job = partner_info.info.job

    for i = 1,8 do 
        self.prof_list[i].gameObject:SetActive(self.cur_job == i)
    end 

    self.prof_list[self.cur_job]:FlushStarCfg()

    local gift_list = self.data:GetPartnerGiftBaseCfg()
    for i = 1,10 do 
        self.gift_list[i]:SetData(gift_list[i])
        self:SetGiftPos(self.gift_list[i],i)
    end 

    -- if not self.init_flag then 
        -- self:Init() 
    --     self.init_flag = true
    -- end 

    local sp_list = self.data:GetPartnerGiftSpEffectCfg()
    for i = 1,3 do 
        self.sp_skill_list[i]:SetData(sp_list[i])
    end 

    self.prof_list[self.cur_job]:FlushLinks()

    self:FlushModelShow()
end

-- 改变item的位置
function PartnerGiftProfPanel:SetGiftPos(item,index)
    item.gameObject:SetParent(self.prof_list[self.cur_job].star_list[index].lockpos,true)
    UH.AnchoredPosition(item.gameObject:GetComponent(typeof(UnityEngine.RectTransform)),Vector2.zero)
    item.gameObject:SetParent(self.GiftPoint,true)
end

function PartnerGiftProfPanel:FlushModelShow()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    if nil == data then return end
    local skin_id = (0 == data.vo.skin_id and self.data.cur_sel_skin.skin_tab) and data.skin_id_try or data.vo.skin_id
    local res_path = ""
    if skin_id > 0 and self.data.partner_skin_ids[skin_id] then
        local res_id = self.data.partner_skin_ids[skin_id].res_id
        res_path = string.format("actor/npc/%s.prefab", res_id)
    else
        local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
        res_path = string.format("actor/npc/%s.prefab", data.info.res_id)
    end
    if not self.ui_obj then
        self.ui_obj = UIObjDrawer.New()
        self.ui_obj:SetData(res_path)
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                load_callback = function ()
                    self.ui_obj:PlayAnim(ChAnimType.Show)
                end
            }
        )
    else
        self.ui_obj:SetData(res_path)
    end
    if self.partner_id_anim ~= self.data.cur_sel_partner then
        self.partner_id_anim = self.data.cur_sel_partner
        self.ui_obj:PlayAnim(ChAnimType.Show)
    end
end

-- 刷新界面
-- 更新界面的数据 和 线段的点亮
function PartnerGiftProfPanel:OnFlushProfView()
    self:ChangeProf()
    for i = 1,10 do self.gift_list[i]:FlushProtocol() end 
    for i =1,3 do self.sp_skill_list[i]:FlushProtocol() end 

    self.prof_list[self.cur_job]:FlushLinks()
    local total = PartnerData.Instance:GetPartnerTotalLevel()
    self.totallevel.text = total
    self.totalleveltext.text = total

    self:ChangeFlyEffEnds()

    -- 物品数量
    local item_show = Item.Init(Config.partner_inborn_auto.other[1].strengthen_consume_item,0)
    self.StuffCall:SetData(item_show)
    UH.SetText(self.StuffNum,string.format(Language.ShengQi.Text16,item_show:Num()))
    UH.SetText(self.StuffName,item_show:Name())
end

function PartnerGiftProfPanel:ChangeFlushSelect(index)
    self.data.gift_flush.cur_select = index
    for i = 1,10 do 
        self.gift_list[i]:FlushSelect(index == i) 
        self.prof_list[self.cur_job].star_list[i]:Select(index == i)
    end 
    local sp_index = index - 10
    for i = 1,3 do self.sp_skill_list[i]:FlushSelect(sp_index == i) end 
end

function PartnerGiftProfPanel:OnClickPoint(index)

    self.data.gift_flush.cur_select = index
    for i = 1,10 do 
        self.gift_list[i]:FlushSelect(index == i) 
        self.prof_list[self.cur_job].star_list[i]:Select(index == i)
    end 
    local sp_index = index - 10
    for i = 1,3 do self.sp_skill_list[i]:FlushSelect(sp_index == i) end 

    self.pop_total:SetActive(true)
    self.pop_block:SetActive(true)
end

function PartnerGiftProfPanel:ShowFlyEffect()
    
end

-- 天赋连接汇总
PartnerGiftProfLinks = PartnerGiftProfLinks or DeclareMono("PartnerGiftProfLinks", UIWidgetBaseItem)
function PartnerGiftProfLinks:FlushStarCfg()
    local base = PartnerData.Instance:GetPartnerGiftBaseCfg()
    for i = 1,10 do 
        self.star_list[i]:SetData(base[i])
    end 
end
function  PartnerGiftProfLinks:FlushLinks() 
    for i = 1,10 do self.star_list[i]:Light() end 
end 

-- 天赋结点
PartnerGiftPointMaster = PartnerGiftPointMaster or DeclareMono("PartnerGiftPointMaster", UIWidgetBaseItem)
function PartnerGiftPointMaster:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.index = data.inborn_hole_id

    self:AutoLinkLinks()
end

function PartnerGiftPointMaster:Select(flag)
    if self.link_start then 
        for i = 1,3 do 
            if self.links_select[i]~=nil and self.links_list[i] ~= nil and not self.links_list[i].activeSelf then 
                self.links_select[i]:SetActive(flag)
            end 
        end 
    end 
end

-- 自动将光条连接到下一个点上
function PartnerGiftPointMaster:AutoLinkLinks()
    if self.link_control ~= nil and self.link_control == true then
        for i = 1,3 do 
            if self.links ~= nil and self.links[i] ~= nil then 
                local pos_self = self.gameObject:GetLocalPosition()
                local pos_tar = self.links_star[i].gameObject:GetLocalPosition()
            
                -- 三角函数
                local line_x = math.abs(pos_tar.x - pos_self.x)
                local line_y = math.abs(pos_tar.y - pos_self.y)
                local line_light = math.sqrt(line_x*line_x + line_y*line_y)

                -- 检查倾斜方案
                local is_left = (pos_tar.x - pos_self.x) < 0
                local is_bottom = (pos_tar.y - pos_self.y ) < 0
             
                local dir = math.deg(math.asin(line_y/line_light ))
                local dir = is_left and 180 - dir or dir
                local dir = is_bottom and - dir or dir

                UH.LocalEulerAngles(self.links[i]:GetComponent(typeof(UnityEngine.RectTransform)), Vector3(0,0,dir))
                UH.SizeDelta(self.links[i], Vector3.New(line_light, 8))
            end 
        end 
    end 
end

function PartnerGiftPointMaster:Light()
    if PartnerData.Instance.cur_sel_partner < 0 then return end 
    if self.link_start then 
        for i = 1,3 do
            if self.links_list[i] ~= nil and self.links_star ~= nil and self.links_star[i] ~= nil then 
                local pre_level = self.links_star[i]:GetPreLevel(self.index)
                local gift_level = PartnerData.Instance:GetPartnerGift(nil,self.index)
                self.links_list[i]:SetActive(gift_level >= pre_level)
            end 
        end 
    end 
end

function PartnerGiftPointMaster:GetPreLevel(pre_hole)
    if self.data == nil then return 0 end 
    for i = 1,3 do 
        if self.data["pre_hole_"..i] == pre_hole then 
            return self.data["pre_level_"..i]
        end 
    end  
    return 0
end


-- 天赋点
PartnerGiftSkillPoint = PartnerGiftSkillPoint or DeclareMono("PartnerGiftSkillPoint", UIWidgetBaseItem)
function PartnerGiftSkillPoint:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.index = data.inborn_hole_id
    UH.SetIcon(self.Icon, data.hole_icon, ICON_TYPE.PARTNERGIFT)
end

function PartnerGiftSkillPoint:OnClickGiftPoint()
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ClickGeneral1)

    self.panel:OnClickPoint(self.index)
end

-- 随协议刷新的信息 
function PartnerGiftSkillPoint:FlushProtocol()
    local level = PartnerData.Instance:GetPartnerGift(nil,self.index)
    if self.index == nil then level = level[1] end 

    self.inter.Interactable = level > 0 
    UH.SetText(self.level, level.."/"..10)

    self.red_point = self.red_point or UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
    self.red_point:SetNum(PartnerData.Instance:CheckGiftPointRed(nil,self.index) and 1 or 0)
end

function PartnerGiftSkillPoint:FlushSelect(flag)
    self.select:SetActive(flag)
end

-- 天赋特效
PartnerGiftSpecialSkill = PartnerGiftSpecialSkill or DeclareMono("PartnerGiftSpecialSkill", UIWidgetBaseItem)
function PartnerGiftSpecialSkill:PartnerGiftSpecialSkill()

end 

function PartnerGiftSpecialSkill:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.index = data.sp_hole + 10

    UH.SetIcon(self.Icon, data.sp_icon, ICON_TYPE.BUFF)
    UH.SetText(self.level, data.inborn_level_ask..(Language.Partner.GiftUnLock))
end

function PartnerGiftSpecialSkill:OnClickEffectPoint()
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ClickGeneral1)    
    self.panel:OnClickPoint(self.index)
end

-- 随协议刷新的信息
function PartnerGiftSpecialSkill:FlushProtocol()
    -- self.inter.Interactable
    local total = PartnerData.Instance:GetPartnerTotalLevel()

    if total == nil or self.data == nil or self.data.inborn_level_ask == nil then return end 
    self.lock:SetActive(total < self.data.inborn_level_ask)
    -- self.total_mark = total
end

function PartnerGiftSpecialSkill:FlushSelect(flag)
    self.select:SetActive(flag)
end

-- 信息侧板
PartnerGiftInfoPanel = PartnerGiftInfoPanel or DeclareMono("PartnerGiftInfoPanel", UIWFlushPanel)
function  PartnerGiftInfoPanel:PartnerGiftInfoPanel()
    self.data = PartnerCtrl.Instance:Data()

    self.data_cares = {
        {data = self.data.gift_flush, func = self.OnFlushInfo, },--init_call = false},
        {data = self.data.item_flush, func = self.OnFlushInfo, init_call = false},
        {data = BagData.Instance.item_grid,func = self.OnFlushInfo, init_call = false},
    }

    self.level_mark = 0
    self.attr_eff = {}
end 

-- 归位为point0
function PartnerGiftInfoPanel:Init()
    self.data.gift_flush.cur_select = 1
end

function PartnerGiftInfoPanel:ShowFlushEffect()
    self.PopEffectTool:Play(2161056)
end

function PartnerGiftInfoPanel:OnClickPopBlock()
    self.pop_total:SetActive(false)
    self.pop_block:SetActive(false)
end

function PartnerGiftInfoPanel:OnFlushInfo()
    if self.data.cur_sel_partner < 0 then return end 
    self.sp_show:SetActive(false)
    self.attr_show:SetActive(false)

    -- if self.data.gift_flush = 
    local index = self.data.gift_flush.cur_select
    local data = index > 10 and self.data:GetPartnerGiftSpEffectCfg() or self.data:GetPartnerGiftLevelCfg()
    local operate = index > 10 and data[index - 10] or data[index]

    -- 展示特效技能
    if index > 10 then 
        local total = PartnerData.Instance:GetPartnerTotalLevel()
        local act_str = total >= operate.inborn_level_ask and "" or Language.Partner.GiftSkillEffLock
        UH.SetText(self.name, operate.sp_name..act_str)
        UH.SetText(self.type_desc, Language.Partner.GiftSkillEff)
        UH.SetText(self.add_desc, Language.Partner.GiftSkillEffplus)
        UH.SetIcon(self.spIcon, operate.sp_icon, ICON_TYPE.BUFF)

        UH.SetText(self.sp_desc, operate.sp_des)
        UH.SetText(self.sp_limit, string.format(Language.Partner.GiftSkillEfflimit,operate.inborn_level_ask))

        self.sp_limit.gameObject:SetActive(act_str ~= "")
        self.sp_complete:SetActive(act_str == "")
        self.sp_show:SetActive(true)
        return 
    end

    for i = 1,2 do 
        if self.attr_eff[i] then 
            self.EffectTool:Stop(self.attr_eff[i])
        end 
    end 

    -- 展示天赋结点
    local base = self.data:GetPartnerGiftBaseCfg()[self.data.gift_flush.cur_select]
    local gift_level = PartnerData.Instance:GetPartnerGift(nil,index)
    self.attr_show:SetActive(true)
    local is_lock = gift_level > 0
    local act_str = is_lock and "" or Language.Partner.GiftSkillEffLock

    UH.SetIcon(self.giftIcon, base.hole_icon, ICON_TYPE.PARTNERGIFT)
    UH.SetText(self.name, base.hole_name..act_str)
    UH.SetText(self.type_desc, Language.Role.level..gift_level.."/"..10)
    UH.SetText(self.add_desc, Language.Partner.GiftSkillAttrplus)

    self.level_max:SetActive(gift_level >= 10)
    self.cost:SetActive(gift_level < 10)

    UH.SetText(self.cur_level, is_lock and Language.Partner.GiftPointLv..gift_level or Language.Partner.GiftPointLock)
    UH.SetText(self.next_level, Language.Partner.GiftPointLv..gift_level + 1)
    self.level_up:SetActive(gift_level<10)
    self.ResetInter.Interactable = is_lock
    UH.SpriteName(self.gifticon_bg, index <8 and "gift_low_bg" or "gift_high_bg")

    -- 等级标记
    local old_level = self.level_mark
    self.level_mark = gift_level

    local level_cfg = self.data:GetPartnerGiftLevelCfg()
    local cfg_oper = level_cfg[gift_level== 10 and 10 or gift_level + 1]

    self.attr_list[2].gameObject:SetActive(cfg_oper.attr_add_2 > 0)

    for i = 1 ,2 do 
        local vo = {}
        vo.gift_level = gift_level
        vo.index = i
        self.attr_list[i]:SetData(vo)

        -- 播放扫光特效
        if self.mark and self.level_mark > old_level then 
            if self.mark then 
                self.attr_eff[i] = self.PopEffectTool:Play(2161056,self.attr_list[i].gameObject)
            end 
            if i == 2 then 
                self.mark = false 
                -- self.block:SetActive(false)
            end 
        end 
    end 

    local partner_unlock = self.data:GetPartnerAllInfo()[self.data.cur_sel_partner].vo.is_unlocked == PartnerConfig.LockState.unlock
    self.lock_tips:SetActive(not is_lock and base.pre_level_1 > 0 and index > 1 and partner_unlock)
    if not is_lock and base.pre_level_1 > 0 then 
        UH.SetText(self.lock_tips_detail,string.format( Language.Partner.GiftPreLimit,base.pre_level_1))
    end 
    local enough_1 = false
    self.item_1_obj:SetActive(cfg_oper.strengthen_consume_item[0] ~= nil)
    if cfg_oper.strengthen_consume_item[0] ~= nil then 
        local cost_item_1 = Item.Create(cfg_oper.strengthen_consume_item[0])
        self.cost_item_1:SetData(cost_item_1)
        local cur_num_1 = Item.GetNum(cost_item_1.item_id)
        enough_1 = cur_num_1 >= cfg_oper.strengthen_consume_item[0].num
        local color_1 = enough_1 and COLORSTR.White or COLORSTR.Red8
        UH.SetText(self.cost_need_1,"<color=#"..color_1..">".. cur_num_1.."</color>/"..cfg_oper.strengthen_consume_item[0].num)
    else 
        enough_1 = true
    end
     
    local enough_2 = false
    self.item_2_obj:SetActive(cfg_oper.strengthen_consume_item[1] ~= nil)
    if cfg_oper.strengthen_consume_item[1] ~= nil then 
        local cost_item_2 = Item.Create(cfg_oper.strengthen_consume_item[1])
        self.cost_item_2:SetData(cost_item_2)
        local cur_num_2 = Item.GetNum(cost_item_2.item_id)
        enough_2 = cur_num_2 >= cfg_oper.strengthen_consume_item[1].num
        local color_2 = enough_2 and COLORSTR.White or COLORSTR.Red8
        UH.SetText(self.cost_need_2,"<color=#"..color_2..">".. cur_num_2 .."</color>/"..cfg_oper.strengthen_consume_item[1].num)
    else 
        enough_2 = true
    end 

    self.BtnEnds:SetActive(is_lock)
    UH.SetText(self.BtnOnesText,is_lock and Language.Partner.GiftBtnLevel or Language.Partner.GiftBtnAct)
    local info = self.data:GetPartnerGift()
    self.BtnEndsInter.Interactable = index == 1 or is_lock 
        or ((base.pre_hole_1 > 0 and base.pre_level_1 > 0) and info[base.pre_hole_1] >= base.pre_level_1)  
        or ((base.pre_hole_2 > 0 and base.pre_level_2 > 0) and info[base.pre_hole_2] >= base.pre_level_2)-- enough_1 and enough_2
    self.BtnOnesInter.Interactable = index == 1 or is_lock 
        or ((base.pre_hole_1 > 0 and base.pre_level_1 > 0) and info[base.pre_hole_1] >= base.pre_level_1) 
        or ((base.pre_hole_2 > 0 and base.pre_level_2 > 0) and info[base.pre_hole_2] >= base.pre_level_2)  -- enough_1 and enough_2

    self.red_point_one = self.red_point_one or UiRedPoint.New(self.red_pos_one, Vector3.New(0,0,0))
    self.red_point_one:SetNum(self.data:CheckGiftPointRed() and 1 or 0)

    self.red_point_ends = self.red_point_ends or UiRedPoint.New(self.red_pos_end, Vector3.New(0,0,0))
    self.red_point_ends:SetNum(self.data:CheckGiftPointRed() and 1 or 0)
end

PartnerGiftInfoPanelAttrShow = PartnerGiftInfoPanelAttrShow or DeclareMono("PartnerGiftInfoPanelAttrShow", UIWidgetBaseItem)
function PartnerGiftInfoPanelAttrShow:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local level_cfg = PartnerData.Instance:GetPartnerGiftLevelCfg()
    self.UpAttr:SetActive(data.gift_level > 0 and data.gift_level < 10)
    self.UnlockAttr:SetActive(data.gift_level == 0 or data.gift_level == 10)

    local oper_lv = data.gift_level == 0 and 1 or data.gift_level
    local type = level_cfg[oper_lv]["attr_type_"..data.index]
    local value = DataHelper.IsPercent(type) and Percent(level_cfg[oper_lv]["attr_add_"..data.index]) or level_cfg[oper_lv]["attr_add_"..data.index]

    local type_n = oper_lv == 10 and  level_cfg[oper_lv]["attr_type_"..data.index] or level_cfg[oper_lv+1]["attr_type_"..data.index]
    local value_n = oper_lv == 10 and (DataHelper.IsPercent(type) and Percent(level_cfg[oper_lv]["attr_add_"..data.index]) or level_cfg[oper_lv]["attr_add_"..data.index]) 
        or (DataHelper.IsPercent(type) and Percent(level_cfg[oper_lv+1]["attr_add_"..data.index]) or level_cfg[oper_lv+1]["attr_add_"..data.index])
    
    if data.gift_level == 0 then 
        UH.SetText(self.unl_attr_type,Language.Common.AttrList[type])
        UH.SetText(self.unl_attr_value,"+"..value)
        UH.SetText(self.max_attr_value,"")
    elseif data.gift_level == 10 then 
        UH.SetText(self.unl_attr_type,Language.Common.AttrList[type])
        UH.SetText(self.unl_attr_value,"")
        UH.SetText(self.max_attr_value,"+"..value)
    else 

        UH.SetText(self.up_attr_type,Language.Common.AttrList[type])
        UH.SetText(self.up_attr_value,"+"..value)
        UH.SetText(self.up_attr_next_value,"+"..value_n)
    end 
end