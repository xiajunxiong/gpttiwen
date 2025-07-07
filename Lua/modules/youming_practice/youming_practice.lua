YouMingPractice = YouMingPractice or DeclareView("YouMingPractice", "youming_practice/youming_practice")
VIEW_DECLARE_LEVEL(YouMingPractice, ViewLevel.L0)

function YouMingPractice:YouMingPractice()
    self.lang = Language.YouMingPractice
end
function YouMingPractice:LoadCallback()
    self.TabList:SetData({{
        name = self.lang.TabNames[1],
        tab_objs = {self.CommonLeft, self.TabBase},
        mod = TabMod(Mod.YouMingPractice.Base),
        open = Mod.YouMingPractice.Base
        -- open_tip = ErrorText[40]
    }, {
        name = self.lang.TabNames[2],
        tab_objs = {self.CommonLeft, self.TabSanHun},
        mod = TabMod(Mod.YouMingPractice.SanHun),
        open = Mod.YouMingPractice.SanHun
        -- open_tip = self.lang.PetTabbarNotOpen
    }, {
        name = self.lang.TabNames[3],
        tab_objs = {self.CommonLeft, self.TabQiPo},
        mod = TabMod(Mod.YouMingPractice.QiPo),
        open = Mod.YouMingPractice.QiPo
        -- open_tip = BindTool.Bind1(self.pet_data.GetPetStrTabbarTip, self.pet_data)
    }, {
        name = self.lang.TabNames[4],
        tab_objs = {self.CommonLeft, self.TabGaiZao},
        mod = TabMod(Mod.YouMingPractice.GaiZao),
        open = Mod.YouMingPractice.GaiZao
        -- open_tip = self.lang.PetTabbarNotOpen
    }, {
        name = self.lang.TabNames[5],
        tab_objs = {self.CommonLeft, self.TabJueXing},
        mod = TabMod(Mod.YouMingPractice.JueXing),
        open = Mod.YouMingPractice.JueXing
    }, {
        name = self.lang.TabNames[6],
        tab_objs = {self.CommonLeft, self.TabMingZhuang},
        open = Mod.YouMingPractice.MingZhuang,
        mod = TabMod(Mod.YouMingPractice.MingZhuang)
    }})
    self.TabList:ClickItem(1)
end

YMPTabbarItem = YMPTabbarItem or DeclareMono("YMPTabbarItem", UIWidgetBaseItem)

--[[
data = {
	name = "",			--text on tabbar button label
	tab_objs = nil,		--UnityGameObjects the table if active with this TabbarBtn
	mod = nil,			--红点检查用
	open = nil,			--功能开启用
	open_tip = ""		--未开启标签点击提示
	open_func = func 	--标签开启检查函数 结果返回true or false
	is_self_img = bool	--为true则按钮图片不进行更改
}]]
function YMPTabbarItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name)
    if self.Name2 and self.NameUnact2 then
        UH.SetText(self.Name2, data.name2)
        UH.SetText(self.NameUnact2, data.name2)
    end
    self.TabActivator.Actives:Clear()
    if data.tab_objs ~= nil then
        for _, item in pairs(data.tab_objs) do
            self.TabActivator.Actives:Add(item)
        end
    end
    if self.handle ~= nil then
        RemindCtrl.Instance:Remind():UnRegisterGroup(self.remind_handle_mod, self.handle)
        self.handle = nil
    end
    if data.mod then
        self.RedPoint:SetNum(0)
        self.remind_handle_mod = data.mod -- 记录注册Remind时的ModId
        self.handle = RemindCtrl.Instance:Remind():RegisterGroup(data.mod, function()
            if FunOpen.Instance:GetFunIsOpened(data.open) then
                self.RedPoint:SetNum(RemindCtrl.Instance:Remind():GetGroupNum(data.mod))
            end
        end, true)
    end
    if self.LockObj then
        if open == Mod.YouMingPractice.GaiZao2 or open == Mod.YouMingPractice.GaiZao3 then
            local youming = YouMingData.Instance:GetSelectYouMing()
            self.LockObj:SetActive(not youming:IsRemodeled(1))
        else
            self.LockObj:SetActive(false)
        end
    end
    --[[ if data.open_func then
        FunOpen.Instance:SelfCondition(data.open, data.open_func)
    end
    if data.open then
        FunOpen.Instance:Register(data.open, BindTool.Bind(self.TabbarOpenEvent, self))
    elseif self.TipBlock then
        self.TipBlock.enabled = false
    end ]]
end

function YMPTabbarItem:TabbarOpenEvent(opened)
    -- 检查自己是不是下一个等级开启的是就显示，不是就隐藏咯
    if opened == false then
        local result = FunOpen.Instance:NextTabbar(self.data.open)
        if result then
            self.gameObject:SetActive(result.client_id == self.data.open)
        end
    end
    self.BtnInter.Interactable = opened
    self.TipBlock.enabled = not opened
    if self.BtnSp and not self.data.is_self_img and not self.SelfImg then
        UH.SpriteName(self.BtnSp, opened and "tabbar_btn" or "tabbar_btn_lock")
    end
end

function YMPTabbarItem:OnClickTip()
    local opened, open_tip = FunOpen.Instance:GetFunIsOpened(self.data.open)
    if opened then
        -- 特殊
        if self.BtnInter.Interactable == false then
            if self.data.open_tip then
                if type(self.data.open_tip) == "string" then
                    PublicPopupCtrl.Instance:Center(self.data.open_tip)
                elseif type(self.data.open_tip) == "function" then
                    PublicPopupCtrl.Instance:Center(self.data.open_tip())
                end
            else
                PublicPopupCtrl.Instance:Center(open_tip)
            end
        end
    else
        if self.data.open_tip then
            if type(self.data.open_tip) == "string" then
                PublicPopupCtrl.Instance:Center(self.data.open_tip)
            elseif type(self.data.open_tip) == "function" then
                PublicPopupCtrl.Instance:Center(self.data.open_tip())
            end
        else
            PublicPopupCtrl.Instance:Center(open_tip)
        end
    end
end

function YMPTabbarItem:OnClickTabbarLock()
    if open == Mod.YouMingPractice.GaiZao2 or open == Mod.YouMingPractice.GaiZao3 then
        local youming = YouMingData.Instance:GetSelectYouMing()
        -- self.LockObj:SetActive(not youming:IsRemodeled(1))
        if not youming:IsRemodeled(1) then
            PublicPopupCtrl.Instance:Center(Language.YouMingPractice.GaiZaoOnceAtleast)
        else
            self.LockObj:SetActive(false)
            self:Click()
        end
    else
        self.LockObj:SetActive(false)
    end
    
end

function YMPTabbarItem:Click(data)
    self.ClickEvent:Trigger()
end

function YMPTabbarItem:OnDisable()
    self.Toggle.isOn = false
    if self.handle ~= nil then
        RemindCtrl.Instance:Remind():UnRegisterGroup(self.remind_handle_mod, self.handle)
        self.handle = nil
    end
    FunOpen.Instance:UnRegister(self.data.open)
    if self.RedPoint then
        self.RedPoint:SetNum(0)
    end
end
function YMPTabbarItem:OnDestroy()
    FunOpen.Instance:UnRegister(self.data.open)
    if self.handle ~= nil then
        RemindCtrl.Instance:Remind():UnRegisterGroup(self.remind_handle_mod, self.handle)
        self.handle = nil
    end
end
YouMingPracticePanel = YouMingPracticePanel or DeclareMono("YouMingPracticePanel", UIWFlushPanel)
function YouMingPracticePanel:YouMingPracticePanel()

end
function YouMingPracticePanel:OnClickClose()
    ViewMgr:CloseView(YouMingPractice)
end

YouMingCommon = YouMingCommon or DeclareMono("YouMingCommon", UIWFlushPanel)
function YouMingCommon:YouMingCommon()
    self.data = YouMingData.Instance
    self.data_cares = {
        {data = self.data:GetSelectYouMing(), func = self.FlushBaseInfo}
    }
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end
end
function YouMingCommon:OnClickTip()
    PublicPopupCtrl.Instance:HelpTip(YouMingCfg.TIANZI_ID)
end
function YouMingCommon:FlushBaseInfo()
    local youming = self.data:GetSelectYouMing()
    if youming.Aptitude == nil then
        return
    end
    -- local cur, max = youming:Aptitude()
    -- UH.SetText(self.TianZhi, Format("%s/%s", cur, max))
    -- UH.SetText(self.PingFen, youming:Score())
    -- UH.SetText(self.Name, youming:Name())
    -- UH.SetText(self.Level, Format(Language.Common.LvDot, youming:Level()))
    -- UH.SpriteName(self.JueXing, Format("PetNum%s", youming:AwakeLevel()))
    self.NamePart:SetData(youming)
    self.ui_obj:SetData(youming:ResPath())
end
function YouMingCommon:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
	end
end

YouMingZiZhi = YouMingZiZhi or DeclareMono("YouMingZiZhi", UIWidgetBaseItem)
function YouMingZiZhi:YouMingZiZhi()
    
end
function YouMingZiZhi:SetData(data)
    -- 根据资质显示
    self.List:SetData(data)
end

-- 做最简单的功能
YouMingZiZhiItem = YouMingZiZhiItem or DeclareMono("YouMingZiZhiItem", UIWidgetBaseItem)
function YouMingZiZhiItem:YouMingZiZhiItem()
    self.v2 = Vector2.New(160, 16)
    self.lang = Language.YouMingPractice
end

function YouMingZiZhiItem:SetData(data)
    -- con=12,
    -- str=10,
    -- def=20,
    -- agi=26,
    -- magic=34,
    local attr_type = data.attr_type or data.attri_type
    local attr_value = data.attr_value or data.attri_value
    UH.SetText(self.Title, Language.Common.AttrList[attr_type])
    self.v2:Set(160 / 60 * attr_value, 16)
    UH.SizeDelta2(self.ProgRect, self.v2)
    UH.SetText(self.Value, DataHelper.Percent(attr_type, attr_value))
    if data.add_value and data.add_value > 0 then
        self.AddValueObj:SetActive(true)
        UH.SetText(self.AddValue, Format("+%s", data.add_value))
        local add_value_level = YouMingData.AddValueLevel(data.add_value)
        UH.SpriteName(self.AddBgImg,"FuJiaZhiDi"..add_value_level)
        UH.SpriteName(self.AddPointImg,"FuJiaZhiDiDian"..add_value_level)
    else
        self.AddValueObj:SetActive(false)
        -- UH.SetText(self.AddValue, "")
    end
end
