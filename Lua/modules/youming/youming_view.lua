-- 幽冥系统
YouMingView = YouMingView or DeclareView("YouMingView", "youming/youming_view", Mod.YouMing.Main)
VIEW_DECLARE_LEVEL(YouMingView, ViewLevel.L0)
function YouMingView:YouMingView()
    self.data = YouMingData.Instance
end

function YouMingView:OnCloseClick()
    ViewMgr:CloseView(YouMingView)
end

function YouMingView:CloseCallback()
    self.data:SetSelectYouMing(nil)
end

function YouMingView:OnTipClick()
    PublicPopupCtrl.Instance:HelpTip(YouMingCfg.TIP_ID)
end

function YouMingView:OnClickSeal()
    local scene_id = YouMingData.Instance:GetSealSceneId()
    local cb = function ()
        ViewMgr:CloseView(YouMingView)
    end
    YouMingCtrl.Instance:GoToSealScene(scene_id, cb)
end

YouMingPanel = YouMingPanel or DeclareMono("YouMingPanel", UIWFlushPanel)
function YouMingPanel:YouMingPanel()
    self.data = YouMingData.Instance
    self.data_cares = {
        {data = self.data.select_youming, func = self.FlushYouMing, init_call = false},
        {data = self.data.remind_data.lottrey_remind, func = self.FlushLotteryRemind, init_call = false},
        {data = self.data.remind_data.compound_remind, func = self.FlushCompoundRemind, init_call = false}
    }
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end
end

function YouMingPanel:Awake()
    UIWFlushPanel.Awake(self)
   self:FlushPanel() 
end

function YouMingPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
	end
end

function YouMingPanel:FlushLotteryRemind()
    local remind = self.data:GetLottreyRemind()
    self.RedPointList[1]:SetNum(remind)
end

function YouMingPanel:FlushCompoundRemind()
    local remind = self.data:GetCompoundRemind()
    self.RedPointList[2]:SetNum(remind)
end

function YouMingPanel:FlushPanel()
    self:FlushYouMing()
    self:FlushLotteryRemind()
    self:FlushCompoundRemind()
end

function YouMingPanel:FlushYouMing()
    self.XiuLianRedPoint:SetNum(0)
    local youming = self.data:GetSelectYouMing()
    self.YouMingInfo:SetActive(not TableIsEmpty(youming))
    if TableIsEmpty(youming) then
        return
    end
    -- local tianzi_num, max_tianzi_num = youming:Aptitude()
    -- UH.SetText(self.TianZiNum, string.format("%s/%s", tianzi_num, max_tianzi_num))
    UH.SetText(self.ScoreNum, youming:Score())
    self.NameInfo:SetData(youming)
    self.PetInfo:SetData(youming)
    self.ui_obj:SetData(youming:ResPath())
    local skill_list = youming:SkillList()
    self.SkillList:SetData(skill_list)
    local add_buff = youming:AptitudeBonus()
    if add_buff then
        self.TianZiOn:SetActive(true)
        self.TianZiOff:SetActive(false)
    else
        self.TianZiOn:SetActive(false)
        self.TianZiOff:SetActive(true)
    end
end

function YouMingPanel:OnBtn1Click()
    ViewMgr:OpenView(YouMingLotteryView)
end

function YouMingPanel:OnBtn2Click()
    ViewMgr:OpenView(YouMingCompoundView)
end

function YouMingPanel:OnBtn3Click()
    ViewMgr:OpenView(YouMingResloveView)
end

function YouMingPanel:OnBtn4Click()
    ViewMgr:OpenView(YouMingDictionariesView)
end

function YouMingPanel:OnTianZiTipClick()
    PublicPopupCtrl.Instance:HelpTip(YouMingCfg.TIANZI_ID)
end
function YouMingPanel:OnClickXiuLian()
    ViewMgr:OpenView(YouMingPractice)
end

function YouMingPanel:OnGetWayClick()
    local get_way_info = {}
    local get_way_t = Split(Config.dim_pet_cfg_auto.other[1].get_way,"|")
    for k,v in pairs(get_way_t) do
        table.insert(get_way_info, BagData.Instance:GetWayCfg(tonumber(v)))
    end
    ViewMgr:OpenView(PetGuideTipView, {list = get_way_info, tip = Language.YouMing.Text2})
end

function YouMingPanel:OnTianZiInfoClick()
    local youming = self.data:GetSelectYouMing()
    if TableIsEmpty(youming) then
        return
    end
    self.TianZiInfo.gameObject:SetActive(true)
    self.TianZiInfo:SetData(youming)
end



YouMingBagPanel = YouMingBagPanel or DeclareMono("YouMingBagPanel", UIWFlushPanel)
function YouMingBagPanel:YouMingBagPanel()
    self.data = YouMingData.Instance
    self.data_cares = {
        {data = self.data.youming_list, func = self.FlushPanel, init_call = false},
        {data = self.data.select_youming, func = self.FlushPanel, init_call = false}
    }
end

function YouMingBagPanel:Awake()
    UIWFlushPanel.Awake(self)
   self:FlushPanel() 
end

function YouMingBagPanel:FlushPanel()
    local list = self.data:GetBagList()
    self.List:SetData(list)
    local sel_index = self:GetSelectIndex(list)
    self.List:SetectItem(sel_index,true)
    self.EmptyTip:SetActive(TableIsEmpty(list))
    UH.SetText(self.Count, string.format("%s/%s", self.data:BagItemCount(), self.data:BagMaxCount()))
end

function YouMingBagPanel:GetSelectIndex(list)
    local youming = self.data:GetSelectYouMing():Val()
    if TableIsEmpty(youming) then
        return 1
    end
    for i,v in pairs(list) do
        if v == youming then
            return i
        end
    end
end


YouMingInfoItem = YouMingInfoItem or DeclareMono("YouMingInfoItem", UIWidgetBaseItem)
-- data === YouMing
function YouMingInfoItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local qua = data:Qua()
    local level = data:Level()

    UH.SpriteName(self.QuaDi, "YouMingDi" .. qua)
    self.ItemCell:SetData(data)
    UH.SetText(self.Name, data:Name())
    UH.SetText(self.Level, "Lv." .. level)
    UH.SetText(self.Score, string.format(Language.YouMing.Score, data:Score()))
    self.PetFlag:SetActive(data:IsBePet())
end


YouMingBagItem = YouMingBagItem or DeclareMono("YouMingBagItem", UIWidgetBaseItem)
-- data === YouMing
function YouMingBagItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.InfoItem:SetData(data)
    self.PetFlag:SetActive(data:IsBePet())
end

function YouMingBagItem:OnSelectClick(selected)
    if selected then
        YouMingData.Instance:SetSelectYouMing(self.data)
    end
    UIWidgetBaseItem.OnSelect(self,selected)
end

----------------------------------------------
YouMingNamePart = YouMingNamePart or DeclareMono("YouMingNamePart")
-- data === youming
function YouMingNamePart:SetData(data)
    UH.SetText(self.Name, data:Name())
    UH.SetText(self.Level, Format(Language.Common.LvDot, data:Level()))
    local awake_level = data:AwakeLevel()
    -- UH.SpriteName(self.AwakeImg, "PetNum" .. awake_level)
    UH.SetText(self.AwakeNum, "+"..awake_level)
    local tianzi_num, max_tianzi_num = data:Aptitude()
    UH.SetText(self.TianZiNum, string.format("%s/%s", tianzi_num, max_tianzi_num))
end

function YouMingNamePart:OnTianZiTipClick()
    PublicPopupCtrl.Instance:HelpTip(YouMingCfg.TIANZI_ID)
end
-------------------------------------------

YouMingBePetInfo = YouMingBePetInfo or DeclareMono("YouMingBePetInfo", UIWFlushPanel)
function YouMingBePetInfo:YouMingBePetInfo()
end

function YouMingBePetInfo:Awake()
    UIWFlushPanel.Awake(self)
    self.ClickObj:SetActive(self.CanClick == true)
end

function YouMingBePetInfo:FlushRemind()
    local red_num = YouMingData.Instance:GetYouMingBlessRemind(self.data)
    self.RedPoint:SetNum(red_num)
end

-- data === youming
function YouMingBePetInfo:SetData(data)
    self.data = data
    local pet = data:BePet()
    self.ItemCell:SetObjActive(pet ~= nil)
    self.RemoveBtn:SetActive(pet ~= nil)
    if pet then
        self.ItemCell:SetData(pet)
        UH.SetText(self.Name, pet:Name())
    else
        UH.SetText(self.Name, Language.YouMing.FuHun)
    end
    self:FlushRemind()
end

function YouMingBePetInfo:OnClick()
    local pos = UH.GetRootPosition(self.ClickObj)
    pos.x = pos.x + 200
    pos.y = pos.y + 250
    ViewMgr:OpenView(YouMingPetSelectView, {pos = pos})
end

function YouMingBePetInfo:OnRemoveClick()
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.CANCEL_POSSESS, self.data.index)
end




-- 幽冥技能格子
YouMingSkillCell = YouMingSkillCell or DeclareMono("YouMingSkillCell", UIWidgetBaseItem)
-- data === YouMingSkill
function YouMingSkillCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetIcon(self.Icon, data:Icon(), data.IconType and data:IconType() or nil)--, ICON_TYPE.SKILL)
    UH.SetText(self.Name, data:Name())
    if self.data.skill_type == nil then
        self.TypeObj:SetActive(false)
    else
        self.TypeObj:SetActive(true)
        UH.SetText(self.Type, Language.YouMing.SkillType[data.skill_type])
    end
    UH.SetText(self.Level, "Lv." .. data.skill_level)
    if self.IsNeedGray == true then
        self.Inter.Interactable = not data.is_unactive
    else
        self.Inter.Interactable = true
    end
end

function YouMingSkillCell:OnShowClick()
    if not self.IsCanClick then
        return
    end
    local pos = UH.GetRootPosition(self.Icon.gameObject)
    ViewMgr:OpenView(YouMingSkillTip, {skill = self.data, pos = pos})
end


-- 圣器技能详情弹框
YouMingSkillTip = YouMingSkillTip or DeclareView("YouMingSkillTip", "youming/youming_skill_tip")
VIEW_DECLARE_MASK(YouMingSkillTip, ViewMask.BlockClose)
function YouMingSkillTip:YouMingSkillTip()
    
end

function YouMingSkillTip:LoadCallback(param_t)
    param_t = param_t or {}
    local skill = param_t.skill
    if not skill then
        return
    end
    
    UH.SetText(self.LblName, skill:Name())
    UH.SetText(self.LblDesc, skill:Desc())
    self.SkillItem:SetData(skill)
    if param_t.pos ~= nil  then
        self:CheckPos(param_t.pos)
        UH.LocalPos(self.PosRect,param_t.pos)
    end 
end

function YouMingSkillTip:CheckPos(pos)
    if pos.x > 340 then
        pos.x = 340
    end
    if pos.x < -340 then
        pos.x = -340
    end
    if pos.y > 88 then
        pos.y = 88
    end
    if pos.y < -450 then
        pos.y = -450
    end
end


YouMingTianZiInfo = YouMingTianZiInfo or DeclareMono("YouMingTianZiInfo")
function YouMingTianZiInfo:SetData(youming)
    local config = youming:AptitudeBonus()
    if config then
        UH.SetText(self.State, Language.YouMingPractice.ActiveBuff)
        UH.SetText(self.AddText, Format(Language.YouMingPractice.AddBuffDesc, config.talent_addup))
    else
        UH.SetText(self.State, Language.YouMingPractice.UnactiveBuff)
        UH.SetText(self.AddText, Format(Language.YouMingPractice.AddBuffDesc, 0))
    end
    local desc = ""
    for i, v in ipairs(Config.dim_pet_cfg_auto.talent_buff) do
        if v.talent_differ == 0 then
            desc = desc .. Format(Language.YouMingPractice.BuffDesc[1], v.talent_addup)
        else
            desc = desc ..Format(Language.YouMingPractice.BuffDesc[2], v.talent_differ, v.talent_addup)
        end
    end 
    UH.SetText(self.Desc, desc)
end

function YouMingTianZiInfo:OnCloseClick()
    self.gameObject:SetActive(false)
end