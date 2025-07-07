SplaceAnecShopView = SplaceAnecShopView or DeclareView("SplaceAnecShopView","serveractivity/splace_anec_shop_view")
function SplaceAnecShopView:OnClickClose()
    ViewMgr:CloseView(SplaceAnecShopView)
end


SplaceAnecShopPanel = SplaceAnecShopPanel or DeclareMono("SplaceAnecShopPanel", UIWFlushPanel)
function SplaceAnecShopPanel:SplaceAnecShopPanel()
    self.data = SplaceAnecdoteData.Instance

    self.data_cares = {
        {data = self.data.shop_info, func = self.FlushPanel},
    }
end 

function SplaceAnecShopPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    TimeHelper:CancelTimer(self.end_ht)
end

function SplaceAnecShopPanel:FlushPanel()
    TimeHelper:CancelTimer(self.end_ht)
    self.item_list:SetData(self.data:GetShopList())
    self.list_num = #self.data:GetShopList()

    self.end_ht = TimeHelper:AddRunTimer(BindTool.Bind(self.FlushPageJump,self),0.2)
    
    self.list_total:SetActive(false)
    self.item_list:SetCompleteCallBack(function()
        self.list_total:SetActive(true)
        self.SellShow:Play()
    end)
end 

function SplaceAnecShopPanel:FlushPageJump( )
    local cur_page_offset = self.part_page_content:GetLocalPosition()
    self.btn_left:SetActive((not (cur_page_offset.x > -215))and self.list_num > 3)
    self.btn_right:SetActive((not (cur_page_offset.x < -(self.list_num-3)*215)and self.list_num > 3))
end

function SplaceAnecShopPanel:OnClickPage(index)
    local cur_page_offset = self.part_page_content:GetLocalPosition()
    local width = 218

    if index == 0 then 
        if cur_page_offset.x <= - width * 3 then
            cur_page_offset.x = cur_page_offset.x + width * 3
        else
            cur_page_offset.x = 0
        end 
    else 
        if cur_page_offset.x < -(self.list_num-3) * width and self.list_num - 6 > 0 and cur_page_offset.x >= -(self.list_num-6)*width then 
            cur_page_offset.x = cur_page_offset.x - width * 3 
        else
            cur_page_offset.x = -(self.list_num-3)*width
        end 
    end 
    
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    self.part_page_content:SetLocalPosition(cur_page_offset)
end

function SplaceAnecShopPanel:ClickBuy(data) 
    if SceneData.Instance:GetMode() == SCENE_TYPE.SOULS_ANECDOTE then
        SoulsAnecdoteCtrl.Instance:SendSplaceBuyItemReq({
            item_idx = data.buy_item_idx,
            npc_obj_id = SplaceAnecdoteData.Instance.quest_info.npc_id,
        })
    else
        ServerActivityCtrl.Instance:SendSplaceBuyItemReq({
            item_idx = data.buy_item_idx,
            npc_obj_id = SplaceAnecdoteData.Instance.quest_info.npc_id,
        })
    end
end 

----------------------------SplaceShopItem----------------------------
SplaceShopItem = SplaceShopItem or DeclareMono("SplaceShopItem", UIWidgetBaseItem)
function SplaceShopItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.gameObject.name = "sell_item" .. data.panel_index
    local item = Item.Create({item_id = data.item_id,is_bind = data.is_bind,num = data.num})
    self.item_cell:SetData(item)
    UH.SetText(self.item_name,item:Name())
    UH.SetText(self.price,data.price)
    self.is_discount:SetActive(data.discount > 0)
    UH.SetText(self.lbl_discount,(data.discount/10)..Language.SplaceAnecdote.Discount)
    UH.SetText(self.limited,data.buy_time - data.m_buy_time)
    UH.SpriteName(self.price_type,data.price_type == 0 and "YuanBao" or "coin")
    self.mask:SetActive(data.buy_time == data.m_buy_time)  
end 
