YouMingRemodel = YouMingRemodel or DeclareView("YouMingRemodel", "youming_remodel/youming_remodel")
VIEW_DECLARE_LEVEL(YouMingRemodel, ViewLevel.L1)
VIEW_DECLARE_MASK(YouMingRemodel, ViewMask.BgBlock)
function YouMingRemodel:YouMingRemodel()
    self.lang = Language.YouMingPractice
    self.data = YouMingData.Instance
end
function YouMingRemodel:LoadCallback()
    self.TabList:SetData({{
        name = self.lang.TabNames2[1],
        tab_objs = {self.Commom, self.Tab1},
        mod = TabMod(Mod.YouMingPractice.GaiZao1),
        open = Mod.YouMingPractice.GaiZao1
    }, {
        name = self.lang.TabNames2[2],
        tab_objs = {self.Commom, self.Tab2},
        mod = TabMod(Mod.YouMingPractice.GaiZao2),
        open = Mod.YouMingPractice.GaiZao2
    }, {
        name = self.lang.TabNames2[3],
        tab_objs = {self.Commom, self.Tab3},
        mod = TabMod(Mod.YouMingPractice.GaiZao3),
        open = Mod.YouMingPractice.GaiZao3
    }})
    self.TabList:ClickItem(1)
end
function YouMingRemodel:OnClickClose()
    ViewMgr:CloseView(YouMingRemodel)
end
function YouMingRemodel:CloseCallback()
    self.data:SetRemodelUse(nil)
    self.data:SetSkillUse(nil)
end

YouMingRemodelCommon = YouMingRemodelCommon or DeclareMono("YouMingRemodelCommon", UIWFlushPanel)
function YouMingRemodelCommon:YouMingRemodelCommon()
    self.data = YouMingData.Instance
    self.data_cares = {{
        data = self.data:GetRemodelYouMing(),
        func = self.FlushBaseInfo
    }}
end
function YouMingRemodelCommon:onFlush()
    local list = self.data:GetBagList()
    self.List:SetData(list)
    local sel_index = self:GetSelectIndex(list)
    self.List:SetectItem(sel_index, true)
end
function YouMingRemodelCommon:GetSelectIndex(list)
    local youming = self.data:GetSelectYouMing():Val()
    if TableIsEmpty(youming) then
        return 1
    end
    for i, v in pairs(list) do
        if v == youming then
            return i
        end
    end
end

function YouMingRemodelCommon:ClickItem(data)
    self.youming = data
    if self.youming.Aptitude == nil then
        return
    end
    self.data:SetRemodelYouMing(data)
end
function YouMingRemodelCommon:FlushBaseInfo()
    self.youming = self.data:GetRemodelYouMing()
    if self.youming.Aptitude == nil then
        return
    end
    local list = self.data:GetBagList()
    self.List:SetData(list)
    
    UH.SetText(self.PingFen, Format(Language.YouMingPractice.PingFen, self.youming:Score()))
    local cur, max = self.youming:Aptitude()
    local remodel_count = self.youming:TotalRemodelCount()
    --这里还要加上改造出来的点数
    if cur == max then
        cur = cur + remodel_count
        max = max + remodel_count
        UH.SetText(self.ZiZhi, Format(Language.YouMingPractice.TianZi2, cur, max))
    else
        cur = cur + remodel_count
        max = max + remodel_count
        UH.SetText(self.ZiZhi, Format(Language.YouMingPractice.TianZi, cur, max, max - cur))
    end
    local change_times = self.youming:Cfg().change_times
    local total = 0
    for i = 1, change_times do
        total = total + i * YouMingCfg.ADD_POINT_TYPE_MAX
    end
    UH.SetText(self.Value, Format(Language.YouMingPractice.GaiZaoValue, remodel_count, total))
end

YouMingRemodelCommonItem = YouMingRemodelCommonItem or DeclareMono("YouMingRemodelCommonItem", UIWidgetBaseItem)
function YouMingRemodelCommonItem:YouMingRemodelCommonItem()

end
function YouMingRemodelCommonItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    UH.SetText(self.Name, data:Name())
end

function YouMingRemodelCommonItem:Click()
    self.EventClick:Trigger()
end

function YouMingRemodelCommonItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self:Click()
    end
end
YouMingRemodelPanel = YouMingRemodelPanel or DeclareMono("YouMingRemodelPanel", UIWFlushPanel)

function YouMingRemodelPanel:YouMingRemodelPanel()
    self.data = YouMingData.Instance
    self.data_cares = {{
        data = self.data:GetRemodelYouMing(),
        func = self.FlushBaseInfo
    },{
        data = self.data:GetRemodelUse(),
        func = self.FlushUseItemInfo,
        keys = {"item"}
    }}
    self.change_id = 1
    self.use_item_id = 0--YouMingCfg.ChangeItemId
    self.select_item_id = 0
end
function YouMingRemodelPanel:OnClickPreview()
    ViewMgr:OpenView(YouMingRemodelSkillPreview)
end
function YouMingRemodelPanel:OnClickTip()
    ViewMgr:OpenView(YouMingPingJi)
end
function YouMingRemodelPanel:OnClickTip2()
    ViewMgr:OpenView(YouMingRemodelRules)
end
function YouMingRemodelPanel:FlushBaseInfo()
    self.youming = self.data:GetRemodelYouMing()
    if self.youming.Aptitude == nil then
        return
    end
    -- 根据改造次数显示几改
    self.tab_list = {}
    local change_times = self.youming:Cfg().change_times
    for i = 1, change_times, 1 do
        self.tab_list[i] = {
            index = i
        }
    end
    self.List:SetData(self.tab_list)
    self.List:ClickItem(self.change_id)
    UH.SetText(self.RemodelCount, Format(Language.YouMingPractice.RemodelCount, self.youming:RemodelCount(), change_times))
    if self.select_item_id == 0 then
        self.AddItem:SetActive(true)
        self.CostCell:SetObjActive(false)
    else
        self.AddItem:SetActive(false)
        self.CostCell:SetObjActive(true)
        self.CostCell:SetDataByItemIdWithNeed(self.select_item_id, 1)
    end
end
function YouMingRemodelPanel:FlushUseItemInfo()
    local item = self.data:GetRemodelUse().item
    self.select_item_id = item == nil and 0 or item.item_id
    self.use_item_id = self.select_item_id
    if self.select_item_id == 0 then
        self.AddItem:SetActive(true)
        self.CostCell:SetObjActive(false)
    else
        self.AddItem:SetActive(false)
        self.CostCell:SetObjActive(true)
        self.CostCell:SetDataByItemIdWithNeed(self.select_item_id,1)
    end
    self.List:ClickItem(self.change_id)
end
function YouMingRemodelPanel:OnClickChange(data)
    -- 只有进行了前面的改造才能进行下次改造
    if data.index ~= 1 and self.youming:IsRemodelLock(data.index) then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.RemodelTip)
        if self.youming:IsRemodelLock(self.change_id) then
            self.List:ClickItem(1)
        else
            self.List:ClickItem(self.change_id)
        end
        return
    end
    self.change_id = data.index
    --如果不满足条件的物品置空
    local temp_id = self.data:RemodelBookChangeId(self.select_item_id)
    
    if self.change_id > temp_id then
        self.data:SetRemodelUse(nil)
    end
    --当前未改造或有改造信息显示model1
    local is_remodeled = self.youming:IsRemodeled(self.change_id)
    -- 显示几改的资质
    local attr_data = self.youming:AptitudeList()
    self.CurZiZhi:SetData(attr_data)
    -- 解锁技能数/当前改显示最大技能解锁数
    local cur, max = self.youming:ChangeSkillNums()
    UH.SetText(self.SkillValue, Format("%s/%s", cur, max))
    local change_data = self.youming:ChangeList(self.change_id)
    self.ChangeZiZhi:SetData(change_data)
    -- 根据选择的change_id 和 消耗品显示范围
    local item_id = self.use_item_id == 0 and YouMingCfg.ChangeItemId or self.use_item_id
    local config = Cfg.YouMingGaiZao(self.change_id - 1, item_id, false)
    if config then
        local group_id = 1
        local max = 0
        for i = 1, YouMingCfg.ADD_POINT_TYPE_MAX do
            group_id = config["change_get_talent_" .. (i - 1)]
            max = self.data:ChangeTalentMax(group_id)
            UH.SetText(self.RangeTexts[i], Format("%s-%s", 0, max))
        end
        UH.SetText(self.RangeTexts[6], Format("%s-%s", 0, self.data:ChangeSkillMax(config.change_get_skill_hole)))
    end
    local has, temp_id = self.youming:IsHasTemp()
    local is_remodel_ing = (has and self.change_id == temp_id)
    self.Model1:SetActive(not is_remodeled or is_remodel_ing)
    self.Model2:SetActive(is_remodeled and not is_remodel_ing)
    self.PingJia:SetShow(self.youming, self.change_id)
    self.SaveRoot:SetActive(has and self.change_id == temp_id)
    self.RemodelRoot:SetActive(self.youming:RemodelLevel(self.change_id) ~= 4)--wanmei
    self.BtnGaiZao:SetActive(not is_remodel_ing)
    --获取当前技能列表
    self.SkillList:SetData(self.youming:CurSkillList())
    self.PreviewList:SetData(self.youming:PreviewSkillList(self.change_id))
    self.SkillList2:SetData(self.youming:ChangeSkillList(self.change_id))
    UH.SetText(self.Count, Format(Language.YouMingPractice.RemodelZiZhi, self.youming:RemodelZiZhiCount(self.change_id)))    
end
function YouMingRemodelPanel:OnClickSave()
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.SAVE_OR_CANCEL_REFORM, self.youming.index, 1)
end
function YouMingRemodelPanel:OnClickCancel()
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.SAVE_OR_CANCEL_REFORM, self.youming.index, 0)
end
function YouMingRemodelPanel:OnClickReRemodel()
    if self.use_item_id ~= 0 then
        if Item.GetNum(self.use_item_id) == 0 then
            PublicPopupCtrl.Instance:Center(Language.YouMingPractice.GaiZaoZero)
            return
        end
    end
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.REPEAT_REFORM, self.youming.index, self.change_id - 1, self.use_item_id)
end
function YouMingRemodelPanel:OnClickGaiZao()
    if self.youming:IsHasReshape() then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.UnSaveOrCancel)
        return
    end
    local has, temp_id = self.youming:IsHasTemp()
    if has and self.change_id ~= temp_id then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.UnSaveOrCancel2)
        return
    end
    if self.use_item_id == 0 then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.GaiZaoNeed)
    else
        if Item.GetNum(self.use_item_id) == 0 then
            PublicPopupCtrl.Instance:Center(Language.YouMingPractice.GaiZaoZero)
            return
        end
        YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.REFORM, self.youming.index, self.change_id - 1, self.use_item_id)
    end
end
function YouMingRemodelPanel:OnClickAdd()
    --打开使用道具界面
    ViewMgr:OpenView(YouMingRemodelUse, {change_id = self.change_id})
end

YouMingRemodelItem = YouMingRemodelItem or DeclareMono("YouMingRemodelItem", UIWidgetBaseItem)
function YouMingRemodelItem:YouMingRemodelItem()
    self.youming_data = YouMingData.Instance
    -- self.qua_di = {"PingJiBiaoQian_Lv", "PingJiBiaoQian_Lan","PingJiBiaoQian_Zi","PingJiBiaoQian_Cheng"}
    self.qua_dis = {"_LocPingJiLvDi","_LocPingJiLanDi","_LocPingJiZiDi","_LocPingJiChengDi"}
end
function YouMingRemodelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    -- LogError("data",data)
    -- UH.SetText(self.Name, Language.YouMingPractice.RemodelTab[data.index])
    self.youming = self.youming_data:GetRemodelYouMing()
    if self.youming.Aptitude == nil then
        return
    end
    -- self.QuaObj:SetActive(not self.youming:IsRemodelLock(data.index))
    local level = self.youming:RemodelLevel(data.index)
    local remodeled = self.youming:IsRemodeled(data.index)
    -- UH.SpriteName(self.QuaDi, self.qua_di[level])
    -- for i = 1, 4 do
    --     self.LevelObjs[i]:SetActive(i == level)
    -- end
    level = not remodeled and 0 or level
    self.EmptyObj:SetActive(not remodeled)
    UH.SpriteName(self.LevelImg, self.qua_dis[level])
    local list = self.youming_data:RemodelBooks(data.index)
    self.RedPoint:SetNum((level == 4 or table.nums(list) == 0) and 0 or 1)
end
function YouMingRemodelItem:Click()
    self.EventClick:Trigger()
end
function YouMingRemodelItem:OnSelect()
    self:Click()
end


YouMingRemodelSkillCell = YouMingRemodelSkillCell or DeclareMono("YouMingRemodelSkillCell", UIWidgetBaseItem)
function YouMingRemodelSkillCell:YouMingRemodelSkillCell()
end
function YouMingRemodelSkillCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if data.skill_id == 0 then
        self.Icon:SetObjActive(false)
        self.WenHao:SetActive(data:IsWenHao())
        self.Lock:SetActive(data:IsLock())
    else
        self.Icon.enabled = true
        UH.SetIcon(self.Icon, data:Icon())--, ICON_TYPE.SKILL)
        self.Icon:SetObjActive(true)
        self.WenHao:SetActive(false)
        self.Lock:SetActive(false)
    end
    -- UH.SetText(self.Name, data:Name())
    UH.SetText(self.ChangeTex, Format(Language.YouMingPractice.RemodelTab[data:ChangeId()]))
end

function YouMingRemodelSkillCell:OnShowClick()
    --[[ if not self.IsCanClick then
        return
    end ]]
    if self.data == nil then
        return
    end
    if self.data.skill_id ~= 0 then
        local pos = UH.GetRootPosition(self.Icon.gameObject)
        ViewMgr:OpenView(YouMingSkillTip, {skill = self.data, pos = pos})
    end
end


--==改造图使用
YouMingRemodelUse = YouMingRemodelUse or DeclareView("YouMingRemodelUse", "youming_remodel/youming_remodel_use")
function YouMingRemodelUse:YouMingRemodelUse()
    self.data = YouMingData.Instance
    self.select_item = nil
end
function YouMingRemodelUse:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.YouMingPractice.RemodelUse, Vector2.New(560, 440))
    self.data:SetRemodelUse(nil)
    -- self.EmptyTip
    self.change_id = param_t.change_id
    local list = self.data:RemodelBooks(self.change_id)
    --显示改造书 新增条件1改改造书不能用于2改

    self.List:SetData(list)
    self.EmptyTip:SetActive(table.nums(list) == 0)
end

function YouMingRemodelUse:OnClickOk()
    self.data:SetRemodelUse(self.select_item)
    ViewMgr:CloseView(YouMingRemodelUse)
end

function YouMingRemodelUse:OnGetWayClick()
    local get_way_info = {}
    local get_way_t = Split(Config.dim_pet_cfg_auto.other[1].change_book_get_way,"|")
    for k,v in pairs(get_way_t) do
        table.insert(get_way_info, BagData.Instance:GetWayCfg(tonumber(v)))
    end
    ViewMgr:OpenView(PetGuideTipView, {list = get_way_info, tip = Language.YouMingPractice.Text2})
end
function YouMingRemodelUse:OnClickItem(data)
    if self.select_item and self.select_item == data then
        MainOtherCtrl.Instance:GetWayView({item = data})
    end
    self.select_item = data
end

YouMingRemodelUseItem = YouMingRemodelUseItem or DeclareMono("YouMingRemodelUseItem", UIWidgetBaseItem)
function YouMingRemodelUseItem:YouMingRemodelUseItem()
    
end
function YouMingRemodelUseItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetDataByItemIdWithZeroNum(data.item_id)
end
function YouMingRemodelUseItem:Click()
    self.EventClick:Trigger()
end

function YouMingRemodelUseItem:OnSelect(selected)
    if selected then
        self:Click()
    end
end

YouMingRemodelPingJia = YouMingRemodelPingJia or DeclareMono("YouMingRemodelPingJia", UIWidgetBaseItem)
function YouMingRemodelPingJia:YouMingRemodelPingJia()
    self.qua_dis = {"_LocPingJiLvDi","_LocPingJiLanDi","_LocPingJiZiDi","_LocPingJiChengDi"}
    -- self.qua_points = {"PingJiLvDian","PingJiLanDian","PingJiZiDian","PingJiChengDian"}
end
function YouMingRemodelPingJia:SetShow(youming, change_id)
    local level = youming:RemodelLevel(change_id)
     UH.SpriteName(self.QuaDi, self.qua_dis[level])
    --  UH.SpriteName(self.QuaPoint1, self.qua_points[level])
    --  UH.SpriteName(self.QuaPoint2, self.qua_points[level])
    --  UH.SetText(self.Desc, Language.YouMingPractice.RemodelLevel[level])
end

--==改造技能预览
YouMingRemodelSkillPreview = YouMingRemodelSkillPreview or DeclareView("YouMingRemodelSkillPreview", "youming_remodel/youming_skill_preview")
function YouMingRemodelSkillPreview:YouMingRemodelSkillPreview()
    self.data = YouMingData.Instance
end
function YouMingRemodelSkillPreview:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.YouMingPractice.Text8, Vector2.New(812, 507))
    self.Items[1]:Trigger()
end
function YouMingRemodelSkillPreview:OnClickItem(flag)
    local data = self.data:GetRemodelSkillList(flag)
    self.List:SetData(data)
end


YouMingRemodelSkillPreviewItem = YouMingRemodelSkillPreviewItem or DeclareMono("YouMingRemodelSkillPreviewItem", UIWidgetBaseItem)
function YouMingRemodelSkillPreviewItem:YouMingRemodelSkillPreviewItem()
    
end
function YouMingRemodelSkillPreviewItem:SetData(data)
    self.Cell:SetData(data)
    UH.SetText(self.Name, data:Name())
end