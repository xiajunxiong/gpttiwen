MainViewUsuallySub = MainViewUsuallySub or DeclareMono("MainViewUsuallySub", UIWFlushPanelGroup)

function MainViewUsuallySub:Awake()
    UIWFlushPanelGroup.Awake(self)
    if IS_STANDALONE() then
        UH.LocalScale(self.RootRect, STANDALONE_UI_VECTOR3)
    end
end

function MainViewUsuallySub:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.RbBtnAnim:Rebind()
    self.RbBtnAnim:SetBool("state", false)
end

local usually_line1_config
local function get_line1_config()
    -- 上下两行 由下到上排 pos=1右下
    if usually_line1_config == nil then
        usually_line1_config = {
            {view = RolePromoteView, text = Language.Main.Promote, sprite = "JinSheng", mod = Mod.RolePromote, open = Mod.RolePromote, pos = 1},
            {view = EquipView, text = Language.Main.ZhuangBei, sprite = "ZhuangBei", mod = Mod.Equip, open = Mod.Equip.Upgrade, pos = 1}, 
            {view = PartnerView, text = Language.Main.HuoBan, sprite = "HuoBan", mod = Mod.Partner, open = Mod.Partner, pos = 1},
        }
    end
    return usually_line1_config
end

local usually_line2_config
local function get_line2_config()
    -- 上下两行 由下到上排 pos=1右下
    if usually_line2_config == nil then
        usually_line2_config = {
            {view = BagView, text = Language.Main.BeiBao, sprite = "BeiBao", mod = Mod.Bag, pos = 1},
            -- {view = GuildList, text = Language.Main.JiaZu, sprite = "JiaZu", mod = Mod.Guild, open = Mod.Guild, pos = 1},
            {view = TreasureHuntView, text = Language.Main.XunBao, sprite = "XunBao", mod = Mod.TreasureHunt, open = Mod.TreasureHunt, pos = 1,
                lock_func = function()
                    local list = TreasureHuntData.Instance:GetOpenedTreasureID()
                    return next(list) ~= nil end},
            {view = KnightlyView, text = Language.Main.XiaYi, sprite = "XiaYi", mod = Mod.Knight, open = Mod.Knight.Main, pos = 1},
        }
    end
    return usually_line2_config
end
MainViewUsuallyLine1 = MainViewUsuallyLine1 or DeclareMono("MainViewUsuallyLine1", UIWFlushPanel)
function MainViewUsuallyLine1:MainViewUsuallyLine1()
    EventSys:Bind(GlobalEventKeys.RoleLevelChange, BindTool.Bind(self.FlushLine1, self))
    self.data_cares = {
        {data = MainViewData.Instance.view_data, func = self.FlushLine1, keys = {"flush_rb"}},
    }
end
function MainViewUsuallyLine1:FlushLine1()
    self.BtnList:SetData(get_line1_config())
end
function MainViewUsuallyLine1:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.BtnList:SetData(get_line1_config())
    self.BtnList:SetCompleteCallBack(function ()
        GuideManager.Instance:RegisterGetGuideUi("RolePromoteBtn2",function ()
            for k, v in pairs(self.BtnList:GetData()) do
                if v.sprite == "JinSheng" then
                    MainViewData.Instance:HideRbButton()
                    return self.BtnList.item_list[self.BtnList:GetSelectedIndex(v)],function ()
                        ViewMgr:OpenView(v.view)
                    end
                end
            end
        end)
    end)
end

MainViewUsuallyLine2 = MainViewUsuallyLine2 or DeclareMono("MainViewUsuallyLine2", UIWFlushPanel)
function MainViewUsuallyLine2:MainViewUsuallyLine2()
    EventSys:Bind(GlobalEventKeys.RoleLevelChange, BindTool.Bind(self.FlushLine2, self))
    self.data_cares = {
        {data = MainViewData.Instance.view_data, func = self.FlushLine2, keys = {"flush_rb"}},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Temporary), func = self.FlushLine2, init_call = false},
        {data = BagData.Instance.better_equip, func = self.FlushEquipBetter, keys = {"is_better"}},
    }
end
function MainViewUsuallyLine2:FlushEquipBetter()
    self.Is_equip_better:SetActive(BagData.Instance.better_equip.is_better)
end
function MainViewUsuallyLine2:FlushLine2()
    local line2 = DataHelper.TableCopy(get_line2_config())
    if BagData.Instance:CheckTempBagEmpty() then
        table.insert(line2, 2, {view = TempBag, text = Language.Main.LinShiBeiBao, sprite = "LinShiBeiBao"})
    end
    local num = table.nums(line2)
    for index = 1, self.BtnList:Length() do
        if index <= num then
            self.BtnList[index].gameObject:SetActive(true)
            self.BtnList[index]:OnRelease()
            self.BtnList[index]:SetData(line2[index])
        else
            self.BtnList[index].gameObject:SetActive(false)
        end
    end
end
function MainViewUsuallyLine2:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushLine2()
    GuideManager.Instance:RegisterGetGuideUi("KnightMain",function ()
        for index = 1, self.BtnList:Length() do
            local data = self.BtnList[index]:GetData()
            if data and data.sprite == "XiaYi" then
                return self.BtnList[index].gameObject, function ()
                    ViewMgr:OpenView(KnightlyView)
                end
            end
        end
    end)
end