LBDragonPluseDetailView = LBDragonPluseDetailView or DeclareView("LBDragonPluseDetailView", "lb_dragon_pluse/lb_dragon_pluse_detail")
function LBDragonPluseDetailView:LBDragonPluseDetailView()

end

function LBDragonPluseDetailView:OnClickClose()
    ViewMgr:CloseView(LBDragonPluseDetailView)
end

function LBDragonPluseDetailView:LoadCallback(param_t)
    if param_t ~= nil then 
        self.Panel:FlushPanel()
    end 
end

LBDragonPluseDetailPanel = LBDragonPluseDetailPanel or DeclareMono("LBDragonPluseDetailPanel", UIWFlushPanel)
function LBDragonPluseDetailPanel:LBDragonPluseDetailPanel()

end

function LBDragonPluseDetailPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function LBDragonPluseDetailPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    LBDragonPluseData.Instance:SetOperateBoss()
end

function LBDragonPluseDetailPanel:FlushPanel()

    self.BossPart.gameObject:SetActive(LBDragonPluseData.Instance.operate_info.cache.type == 1)
    self.MonsterPart.gameObject:SetActive(LBDragonPluseData.Instance.operate_info.cache.type == 2)
end

LBDragonPluseDetailBossPart = LBDragonPluseDetailBossPart or DeclareMono("LBDragonPluseDetailBossPart", UIWFlushPanel)
function LBDragonPluseDetailBossPart:LBDragonPluseDetailBossPart()
    self.data = LBDragonPluseData.Instance
    self.data_cares = {
        {data = self.data.base_info, func = self.FlushPanel,init_call = false},
        {data = self.data.my_info, func = self.FlushPanel,init_call = false},
    }

    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end

    ActFormationCtrl.Instance:SendGetFormationInfo(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_GOD_SPACE)
end

function LBDragonPluseDetailBossPart:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil
end

function LBDragonPluseDetailBossPart:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushPanel()
end

function LBDragonPluseDetailBossPart:FlushPanel()

    self.ui_obj:SetData(self.data.operate_info.cache.model_path)
    UH.SetText(self.AttrTips,Config.language_cfg_auto.textdesc[375].desc)
    UH.SetText(self.ChallengeTips,Config.language_cfg_auto.textdesc[376].desc)

    UH.SetText(self.BossName,self.data.operate_info.cache.name)
    UH.SetText(self.LastTime,string.format(Language.LBDragonPluse.BossDetailTimes,self.data.operate_info.cache.times))

    local rate = self.data.operate_info.cache.life / 10000
    self.BossPro:SetProgress(rate)
    local show = rate * 100
    UH.SetText(self.BossProText,show.."%")
end

function LBDragonPluseDetailBossPart:OnClickShare()
    LBDragonPluseCtrl.Instance:ReqDragonSharekBoss({index = self.data.operate_info.cache.index-1})
end

function LBDragonPluseDetailBossPart:OnClickBuZhen()
    ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_GOD_SPACE)
end

function LBDragonPluseDetailBossPart:OnClickChallenge()
    -- LBDragonPluseCtrl.Instance:ReqDragonAttackBoss({index = self.data.operate_info.cache.index-1})
    self.data:SetOperateSendInfo({is_boss = true})
    self:OnClickBuZhen()
    -- ViewMgr:CloseView(LBDragonPluseDetailView)
    -- ViewMgr:CloseView(LBDragonPluseView)
    -- ViewMgr:CloseView(LandsBetweenMainView)
end

function LBDragonPluseDetailBossPart:OnClickBox()
    -- PublicPopupCtrl.Instance:Center("点击预览")
    ViewMgr:OpenView(LBDragonPluseRewardPreview)
end

LBDragonPluseDetailMonsterPart = LBDragonPluseDetailMonsterPart or DeclareMono("LBDragonPluseDetailMonsterPart", UIWFlushPanel)
function LBDragonPluseDetailMonsterPart:LBDragonPluseDetailMonsterPart()
    self.data = LBDragonPluseData.Instance
    self.data_cares = {
        {data = self.data.base_info, func = self.FlushPanel,init_call = false},
        {data = self.data.my_info, func = self.FlushPanel,init_call = false},
    }

    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end

    self.help_info = {{type =0},{type =0}}
    self.help_select_type = 0
    self.help_select_index = 1
    self.help_selected = nil

    ActFormationCtrl.Instance:SendGetFormationInfo(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_GOD_SPACE)
end

function LBDragonPluseDetailMonsterPart:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil
end

function LBDragonPluseDetailMonsterPart:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function LBDragonPluseDetailMonsterPart:FlushPanel()    
    self.ui_obj:SetData(self.data.operate_info.cache.model_path)
    -- local total_cfg = Config.divine_domain_system_auto.dragon_vein[1]
    -- local list = BagData.Instance:ItemListBuild(total_cfg.preview)

    -- if total_cfg.military_exploits_4 > 0 then 
    --     local item_military_exploits = Item.Init(CommonItemId.GodSpaceExploits,total_cfg.military_exploits_4)
    --     table.insert(list,item_military_exploits)      
    -- end     
    -- if total_cfg.quintessence_4 > 0 then 
    --     local item_quintessence = Item.Init(CommonItemId.GodSpaceExploits,total_cfg.quintessence_4)
    --     table.insert(list,item_quintessence)      
    -- end  

    -- LogError(":SD ",self.data.operate_info.cache.rewards)
    self.ExtralRewards:SetData(self.data.operate_info.cache.rewards)
    
    local cfg = Config.divine_domain_system_auto.dragon_vein[1]
    UH.SetText(self.TodayRewards,string.format(Language.LBDragonPluse.MonsterDetailTimes, 
        cfg.camp_monster_kill_reward_number - self.data.my_info.monster_reward_times))
    UH.SetText(self.MonsterName,self.data.operate_info.cache.name)

    UH.SetText(self.MonsterScore,self.data.operate_info.cache.score)
    UH.SetText(self.WarName,self.data.operate_info.cache.war_name)
    for i = 1,2 do 
        local cfg = LBDragonPluseData.Instance:GethelpInfoBySeq(self.data.operate_info.cache["seq_"..i])
        UH.SetText(self["ConditionSeq"..i],cfg.way_explain..Language.LBDragonPluse.HelpDesc[cfg.param1]) 
    end 

    
    -- UH.SetText(self.ConditionSeq1,)
    -- self.ConditionSeq1
    self:FlushHelper()
end 

function LBDragonPluseDetailMonsterPart:OnClickPlus(index)
    self.SelectPart:SetActive(true) 
    self.help_select_index = index
    self.help_select_type = index
    self.HelpSelectTabs[index].isOn = true

    self:FlushSelectPart()
end

function LBDragonPluseDetailMonsterPart:FlushSelectPart()
    local cfg = LBDragonPluseData.Instance:GethelpInfoBySeq(self.data.operate_info.cache["seq_"..self.help_select_type])

    local temp_member_list = {}

    if self.help_select_type == 1 then     
        for k,v in pairs(PartnerData.Instance:GetPartnerAllInfoLockSort()) do 
            if v.vo.is_unlocked == PartnerConfig.LockState.unlock and v.vo.quality >= cfg.param3 then 
                if not (self:CheckHelperUnique(self.help_info[self.help_select_index == 1 and 2 or 1].info,v)) then 
                    table.insert(temp_member_list,{show_type = 2 , item = v})
                end 
            end 
        end 
    else 
        for k,v in pairs(PetData.Instance:GetPetList()) do 
            if v:Quality()+4 >= cfg.param3 then 
                if not (self:CheckHelperUnique(self.help_info[self.help_select_index == 1 and 2 or 1].info,v)) then 
                    -- 借用的item的类型整好与龙脉的相反
                    table.insert(temp_member_list,{show_type = 1 , item = v})
                end 
            end 
        end 
    end 

    self.SelectList:SetData(temp_member_list)
    self.SelectList:SelectAll(false)
end

function LBDragonPluseDetailMonsterPart:CheckHelperUnique(data_1,data_2)
    if data_1 == nil or data_2 == nil then return false end 

    if self.help_select_type == 1 then 
        return data_1.id == data_2.id
    else 
        return data_1.pet_id == data_2.pet_id and data_1.pet_unique_id1 == data_2.pet_unique_id1 and data_1.pet_unique_id2 == data_2.pet_unique_id2
    end 

    return false
end

function LBDragonPluseDetailMonsterPart:FlushHelper()
    self.Help1:SetData(self.help_info[1])
    self.Help2:SetData(self.help_info[2])

    local num = 0
    for i = 1,2 do 
        if self.help_info[i].id ~= nil and self.help_info[i].id > 0 then 
            num = num + 1
        end 
    end 
    UH.SetText(self.ConditionNum,num .. "/2")    
end

function LBDragonPluseDetailMonsterPart:OnClickPanelClose()
    self.SelectPart:SetActive(false) 
end

function LBDragonPluseDetailMonsterPart:OnClickSelectSure()
    -- self.help_info[self.help_select_type] 
    -- local selected = self.SelectList:GetSelectedItem()
    if self.help_selected == nil then 
        return 
    end 
    
    local vo = {}
    vo.type = self.help_select_type
    vo.id = vo.type == 1 and self.help_selected.item:Id() or self.help_selected.item:ID()
    vo.info = self.help_selected.item
    self.help_info[self.help_select_index] = vo
    self:FlushHelper()

    self.ToggleGroup:SetAllTogglesOff()
    self:OnClickPanelClose()
end

function LBDragonPluseDetailMonsterPart:OnClickReset()
    -- PublicPopupCtrl.Instance:Center("点击重置")

    local need_num = Config.divine_domain_system_auto.dragon_vein[1].reset_helper_cond_cost
    if need_num > RoleData.Instance:GetCurrency(CurrencyType.Gold) then 
        PublicPopupCtrl.Instance:Center(Language.LBDragonPluse.ResetPopErrorTips)
        return 
    end 

    ViewMgr:OpenView(LBDragonPluseResetView,{
        confim_func = function()
            LBDragonPluseCtrl.Instance:ReqResetMonster({index = self.data.operate_info.cache.index-1})
            PublicPopupCtrl.Instance:CloseDialogTips()
        end,
        content = Language.LBDragonPluse.ResetPopTips, --string.format(Language.LBDragonPluse.ResetPopTips,need_num..RichTextHelper.SpriteStrAtlas(CommonItemId.Gold, 25, 25)), 
        cost = need_num,
    })
	
end

function LBDragonPluseDetailMonsterPart:OnClickBuZhen()
    ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_GOD_SPACE)
end

function LBDragonPluseDetailMonsterPart:OnClickEnter()
    -- PublicPopupCtrl.Instance:Center("点击挑战")
    local target = self.data.operate_info.cache.index-1 
    if self.data.my_info.kill_monster_num < target then 
        PublicPopupCtrl.Instance:Center(Language.LBDragonPluse.AttackMonsterEarly)
        return
    end
    
    if self.data.my_info.kill_monster_num > target then 
        PublicPopupCtrl.Instance:Center(Language.LBDragonPluse.AttackMonsterDone)
        return
    end

    local function send_msg()
        self.data:SetOperateSendInfo({
            is_boss = false,
            help_info = {
                [1] =  self.help_info[1],
                [2] =  self.help_info[2],
            }
        })

        self:OnClickBuZhen()
        ViewMgr:CloseView(DialogTipsView)
        -- LBDragonPluseCtrl.Instance:ReqDragonAttackMonster({index = self.data.operate_info.cache.index-1,id_1 = self.help_info[1].id,id_2 = self.help_info[2].id})
        -- ViewMgr:CloseView(LBDragonPluseDetailView)
        -- ViewMgr:CloseView(LBDragonPluseView)
        -- ViewMgr:CloseView(LandsBetweenMainView)
        -- ViewMgr:CloseView(DialogTipsView)
    end

    if self.help_info[1].id == nil or self.help_info[2].id == nil or self.help_info[1].id == 0 or self.help_info[2].id == 0 then
        local info = {
            content = Language.LBDragonPluse.HelpEmptyAttackTips,
            cancel = {
                name = nil,
            },
            confirm = {
                name = nil,
                func = send_msg
            },
            -- tnr = DTTodayNotRemind.LBDragonPluse,
        }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end

    send_msg()
end

function LBDragonPluseDetailMonsterPart:OnClickResetHelp()
    -- PublicPopupCtrl.Instance:Center("点击重置助战")

    for i = 1,2 do 
        self.help_info[i] = {
            id = 0,
            type = 0,
            info = nil,
        }
    end 

    self:FlushHelper()
end

function LBDragonPluseDetailMonsterPart:OnClickHelp()
    -- PublicPopupCtrl.Instance:Center("点击一键助战")

    local co = {}
    local vo = {}
    local cfg = LBDragonPluseData.Instance:GethelpInfoBySeq(self.data.operate_info.cache.seq_1)
    local l_cfg = LBDragonPluseData.Instance:GethelpInfoBySeq(self.data.operate_info.cache.seq_2)

    if #PartnerData.Instance:GetPartnerAllInfo() > 0 then   
        for k,v in pairs(PartnerData.Instance:GetPartnerAllInfo()) do 
            
            if PartnerConfig.LockState.unlock == v.vo.is_unlocked and v.id == cfg.param2 then 
                vo.type = 1
                vo.id = v:Id() 
                vo.info = v
                self.help_info[1] = vo
            end 
            
            if PartnerConfig.LockState.unlock == v.vo.is_unlocked and v.id == l_cfg.param2 then 
                co.type = 1
                co.id = v:Id() 
                co.info = v
                self.help_info[2] = co
            end 
        end 
    end 

    for k,v in pairs(PetData.Instance:GetPetList()) do 
        if v.pet_id == cfg.param2 then 
            vo.type = 2
            vo.id = v:ID() 
            vo.info = v
            self.help_info[1] = vo                
        end 

        if v.pet_id == l_cfg.param2 then 
            co.type = 2
            co.id = v:ID() 
            co.info = v
            self.help_info[2] = co                
        end 
    end 

    if vo.id == nil and co.id == nil then 
        PublicPopupCtrl.Instance:Center(Language.LBDragonPluse.NoHelpToOneKey)
    end 
    self:FlushHelper()
end

function LBDragonPluseDetailMonsterPart:OnClickEffCell(data,value)
    if value then 
        self.help_selected = data
    end 
end

function LBDragonPluseDetailMonsterPart:OnTabClick(index)
    self.help_select_type = index

    self:FlushSelectPart()
end


-- {id,type,info} id 宠or伙的id号，type1是伙伴2是宠物 info 是具体信息组
LBDragonPluseDetailMonsterHelpShow = LBDragonPluseDetailMonsterHelpShow or DeclareMono("LBDragonPluseDetailMonsterHelpShow", UIWidgetBaseItem)
function LBDragonPluseDetailMonsterHelpShow:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    self.PartnerObj:SetActive(data.type == 1 and data.id > 0 )
    self.PetHead.gameObject:SetActive(data.type == 2 and data.id > 0)
    self.Empty:SetActive(data.type == 0 )


    if data.type == 1 then 
        local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(data.id)
        PartnerInfo.SetQuality(self.PartnerQua, info_quality.quality)
        UH.SetIcon(self.PartnerIcon, PartnerData.IconId(data.info), ICON_TYPE.ITEM)
    end 

    if data.type == 2 then 
        self.PetHead:SetDataByPet(data.info)
    end 
end 

-- LBDragonPluseDetailMonsterHelpSelect = LBDragonPluseDetailMonsterHelpSelect or DeclareMono("LBDragonPluseDetailMonsterHelpSelect", UIWidgetBaseItem)
-- function LBDragonPluseDetailMonsterHelpSelect:SetData(data)
-- 	UIWidgetBaseItem.SetData(self, data)

--     self.PartnerObj:SetActive(self.Panel.help_select_type == 1 )
--     self.PetHead.gameObject:SetActive(self.Panel.help_select_type == 2)

--     if self.Panel.help_select_type == 1 then 
--         local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(data.id)
--         PartnerInfo.SetQuality(self.PartnerQua, info_quality.quality)
--         UH.SetIcon(self.PartnerIcon, PartnerData.IconId(data), ICON_TYPE.ITEM)
--         UH.SetText(self.Name,ColorStr(data.info.name,PartnerQualityDeepColor[info_quality.quality]))
--     end 

--     if self.Panel.help_select_type == 2 then 
--         self.PetHead:SetDataByPet(data)
--         UH.SetText(self.Name,data:QuaName(true))
--     end 
-- end 


LBDragonPluseDetailView = LBDragonPluseDetailView or DeclareView("LBDragonPluseDetailView", "lb_dragon_pluse/lb_dragon_pluse_detail")
function LBDragonPluseDetailView:LBDragonPluseDetailView()

end

function LBDragonPluseDetailView:OnClickClose()
    ViewMgr:CloseView(LBDragonPluseDetailView)
end


-- 奖励预览
LBDragonPluseRewardPreview = LBDragonPluseRewardPreview or DeclareView("LBDragonPluseRewardPreview", "lb_dragon_pluse/lb_dragon_pluse_reward_preview")
function LBDragonPluseRewardPreview:LBDragonPluseRewardPreview()

end

function LBDragonPluseRewardPreview:OnClickClose()
    ViewMgr:CloseView(LBDragonPluseRewardPreview)
end

function LBDragonPluseRewardPreview:LoadCallback(param_t)
    if param_t ~= nil then 
        self.Panel:FlushPanel()
    end 
end

LBDragonPluseRewardPreviewFlush = LBDragonPluseRewardPreviewFlush or DeclareMono("LBDragonPluseRewardPreviewFlush", UIWFlushPanel)
function LBDragonPluseRewardPreviewFlush:LBDragonPluseRewardPreviewFlush()
    self.data = LBDragonPluseData.Instance
end

function LBDragonPluseRewardPreviewFlush:FlushPanel()
    local list = self.data:GetBossRewardList()
    self.RewardList:SetData(list)
end

LBDragonPluseRewardPreviewCell = LBDragonPluseRewardPreviewCell or DeclareMono("LBDragonPluseRewardPreviewCell", UIWidgetBaseItem)
function LBDragonPluseRewardPreviewCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Title,data.title)
    self.ItemList:SetData(data.item_list)
end 

LBDragonPluseDetailMonsterRewardShow = LBDragonPluseDetailMonsterRewardShow or DeclareMono("LBDragonPluseDetailMonsterRewardShow", UIWidgetBaseItem)
function LBDragonPluseDetailMonsterRewardShow:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Extra:SetActive(data.is_extra)

    self.Call:SetData(data.item)
end