FabaoJieupView = FabaoJieupView or DeclareView("FabaoJieupView", "fabao/fabao_jieup")
VIEW_DECLARE_MASK(FabaoJieupView,ViewMask.BgBlockClose)
function FabaoJieupView:FabaoView()

end


FabaoJieupPanel = FabaoJieupPanel or DeclareMono("FabaoJieupPanel", UIWFlushPanel)
--Icon===========UIImageSpriteSet
--Name===========Text
--JieBg==========UIWTextureSetter
--EffTitleTxt1===Text
--EffTitleTxt2===Text
--EffVal1========Text
--EffVal2========Text
--Type1Obj=======GameObject
--Type2Obj=======GameObject
--XingXingList===UIImageSpriteSet[]
local sp = {
    [0] = "xingxinghui",
    [1] = "xingxingzi",
    [2] = "xingxinghuang",
    [3] = "xingxinghong",
    [4] = "xingxinghong",
}


function FabaoJieupPanel:FabaoJieupPanel()
    local info = FabaoData.Instance:PopJieupViewInfo()
    local cfg = FabaoData.Instance:FabaoAttrInfo(info.id)
    -- FabaoData.Instance:ViewData().jieup_info = nil

    -- LogError("info===",info,",cfg==",cfg)
    UH.SetIcon(self.Icon,cfg.big_icon)
    UH.SetText(self.Name,cfg.name)

    local next_jie = info.fabao_jie + 1
    local jie_type = math.floor(next_jie / 4) + 1
    self.JieBg:SetByPath("fabao/FaBaoYuanPan".. jie_type ..".png")

    --每4阶换换星星皮
    --余数亮几颗星
    local a = math.floor(next_jie / 4)
    local b = next_jie % 4
    local buttom_img = sp[a]
    local fill_img = sp[a + 1]
    for i = 1, self.XingXingList:Length() do
        UH.SpriteName(self.XingXingList[i], i <= b and fill_img or buttom_img)--a <= 0 and sp[1] or sp[a])
    end
    if cfg.type == 1 then
        self.Type1Obj:SetActive(true)
    else
        self.Type2Obj:SetActive(true)
    end
    if info.fabao_jie < 1 then
        UH.SetText(self.EffTitleTxt1,Language.Fabao.Initial)
    else
        UH.SetText(self.EffTitleTxt1,string.format(Language.Fabao.FabaoJieupEffTitle1,info.fabao_jie))
    end
    local advance1 = FabaoData.Instance:AdvanceCfg(cfg.id, info.fabao_jie)[1]
    local attri_show1 = (advance1.percentage_show == 1 and 
        ((advance1.advanced_addition / 1000) .. "%") or
        advance1.advanced_addition / 1000)
    local attri_show3 = (advance1.percentage_show == 1 and 
        ((advance1.advanced_addition2 / 1000) .. "%") or
        advance1.advanced_addition2 / 1000)        
    UH.SetText(self.EffVal1, string.format(cfg.desc_1,"", attri_show1, "", attri_show3))

    UH.SetText(self.EffTitleTxt2,string.format(Language.Fabao.FabaoJieupEffTitle2,next_jie))
    local advance2 = FabaoData.Instance:AdvanceCfg(cfg.id, next_jie)[1]
    local attri_show2 = (advance2.percentage_show == 1 and 
        ((advance2.advanced_addition / 1000) .. "%") or
        advance2.advanced_addition / 1000)
    local attri_show4 = (advance2.percentage_show == 1 and 
        ((advance2.advanced_addition2 / 1000) .. "%") or
        advance2.advanced_addition2 / 1000)        

    UH.SetText(self.EffVal2, string.format(cfg.desc_1,"", attri_show2,"",attri_show4))
                      
end