GoldrMinecCountView = GoldrMinecCountView or DeclareView("GoldrMinecCountView", "serveractivity/goldr_minec/goldr_minec_count_view")
VIEW_DECLARE_MASK(GoldrMinecCountView, ViewMask.BgBlockClose)
function GoldrMinecCountView:LoadCallback(param_t)  end 
function GoldrMinecCountView:CloseCallback() end

--===========================GoldrMinecCountPanel===========================
GoldrMinecCountPanel = GoldrMinecCountPanel or DeclareMono("GoldrMinecCountPanel", UIWFlushPanel)
function GoldrMinecCountPanel:GoldrMinecCountPanel() 
    self.data = GoldRMinecData.Instance

    local item_show = ColorStr(string.format(Language.GoldRMinec.ItemShow,Item.GetName(self.data:GetOtherCfg().consume_item))
        ,Item.GetColorStr(self.data:GetOtherCfg().consume_item))
    local item_num = Item.GetNum(self.data:GetOtherCfg().consume_item)
    
    UH.SetText(self.num_tips,string.format(Language.GoldRMinec.SendTypeShow,item_show,item_num))

    self.activity_tex = {
        [0] = "serveractivity/goldr_minec/gm_QiLingZhanShi.png",
        "serveractivity/goldr_minec/ShuiLongLiHui.png",
        "serveractivity/goldr_minec/JinLongLiHui.png",
    }
    self.activity_title = {
        [0] = "_Loc_QiTianDaShengTiXingNin",
        "_LocTianShuiLingLongTiXingNinZi",
        "_LocTianWuJinLongTiXingNinZi"
    }
    self.activity_tex_pos = {
        [0] = Vector2.New(-277, 0),
        Vector2.New(-381, -57),
        Vector2.New(-346, -54)
    }
    local type = self.data:ActivityType()
    self.TexSetter:SetByPath(self.activity_tex[type])
    UH.AnchoredPosition(self.ImageRect, self.activity_tex_pos[type])
    UH.SpriteName(self.ImageSp, self.activity_title[type])
end 


function GoldrMinecCountPanel:OnClickTickToggle(flag)
    self.data:SetSendShow(not flag)
end

function GoldrMinecCountPanel:OnClickOne()
    self.data:SetSendType(1)
    GoldRMinecCtrl.Instance:SendDraw()
    ViewMgr:CloseView(GoldrMinecCountView)
    self.data:StartToShield()
end

function GoldrMinecCountPanel:OnClickTen()
    self.data:SetSendType(10)
    GoldRMinecCtrl.Instance:SendDraw()
    ViewMgr:CloseView(GoldrMinecCountView)
    self.data:StartToShield()
end

function GoldrMinecCountPanel:OnDestroy() 
    UIWFlushPanel.OnDestroy(self)
    -- 在不再展示的情况下选择单抽
    if self.data:GetSendType() == 1 and not self.data:GetSendShow() then 
        self.data:SetSendStren(true)
    end 

end 