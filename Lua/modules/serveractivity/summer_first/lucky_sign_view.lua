LuckySignView = LuckySignView or DeclareView("LuckySignView", "serveractivity/summer_first/lucky_sign_view",Mod.SummerFirst.LuckySign)

VIEW_DECLARE_MASK(LuckySignView,ViewMask.BlockClose)
VIEW_DECLARE_LEVEL(LuckySignView,ViewLevel.L0)

function LuckySignView:LuckySignView()
    self.ShowPanel:SetActive(ServerActivityData.Instance:GetLuckySignId() ~= 0)
end

function LuckySignView:OnClickClose()
    ViewMgr:CloseView(LuckySignView)
end

LuckySignPanel = LuckySignPanel or DeclareMono("LuckySignPanel", UIWFlushPanel)

function LuckySignPanel:LuckySignPanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.lucky_sign_data,func = self.FlushSignView,init_call = false,keys = {"sign_id"}},
    }
end

function LuckySignPanel:FlushSignView()
    self.ShowPanel:SetActive(true)
end

function LuckySignPanel:OnClickItem(index)
    if self.data:GetLuckySignId() == 0 then
        self.ItemAnim[index]:Play()
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_LUCKY_SIGN,2)
    end
end

LuckySignShowPanel = LuckySignShowPanel or DeclareMono("LuckySignShowPanel", UIWFlushPanel)

function LuckySignShowPanel:LuckySignShowPanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.lucky_sign_data,func = self.FlushSignView,init_call = false,keys = {"is_lock"}},
    }
end

function LuckySignShowPanel:Awake()
    UIWFlushPanel.Awake(self)
    local config = self.data:GetLuckySignConfig()
    if config ~= nil then
        UH.SetText(self.Desc,self:GetDesc(config.sign))
        UH.SetText(self.Title,Format(Language.LuckySign.Title,config.sign_id,config.luck))
        local item_list = self.data:GetLuckySignRewatdList()
        for i,v in ipairs(item_list) do
            v.is_block = self.data:GetLuckySignId() ~= 0
        end
        self.Grid:SetData(item_list)
        UH.SetText(self.Content,config.sign_solution)
    end
    self:FlushParentView()
end

function LuckySignShowPanel:GetDesc(sign)
    local desc = string.gsub(sign,"，","，\n")
    desc = string.gsub(desc,"。","。\n")
    return desc
end

function LuckySignShowPanel:FlushSignView()
    if self.data:GetLuckySignLock() == 0 then
        self.LockAnim:Play()
    end
    self:FlushParentView()
end

function LuckySignShowPanel:FlushParentView()
    self.Parent:SetActive(self.data:GetLuckySignLock() == 1)
    UH.SetText(self.BtnName,Language.LuckySign.BtnName[self.data:GetLuckySignLock()])
    self.BtnClose:SetActive(self.data:GetLuckySignId() == 0)
    self.Finish:SetActive(self.data:GetLuckySignId() ~= 0)
end

function LuckySignShowPanel:OnClickReturn()
    self.Parent:SetActive(not self.Parent.activeSelf)
    if self.data:GetLuckySignId() ~= 0 then
        ViewMgr:CloseView(LuckySignView)
    end
end

function LuckySignShowPanel:OnClickItem()
    if self.data:GetLuckySignLock() == 0 then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_LUCKY_SIGN,1,1)
    else
        self.Parent:SetActive(true)
    end
end

function LuckySignShowPanel:OnClickClose()
    self.ShowPanel:SetActive(false)
    if self.data:GetLuckySignId() ~= 0 then
        ViewMgr:CloseView(LuckySignView)
    end
end

----------------------------LuckySignShowItem----------------------------
LuckySignShowItem = LuckySignShowItem or DeclareMono("LuckySignShowItem", UIWidgetBaseItem)

function LuckySignShowItem:SetData(data)
    self.Call:SetData(Item.New(data))
    self.Block:SetActive(data.is_block)
end