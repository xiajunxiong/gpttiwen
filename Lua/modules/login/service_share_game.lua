ServiceShareGame = ServiceShareGame or DeclareMono("ServiceShareGame", UIWFlushPanel)
function ServiceShareGame:ServiceShareGame()

end

function ServiceShareGame:OnDestroy()

end

function ServiceShareGame:OnClickShare()
    self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
    if self.mainCanvas then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end

    -- 字体截取
    -- local tex_lbl = UH.FeildShotScreen(self.ScreenMark,self.mainCanvasCom.worldCamera)

    -- -- 二维码获取
    local tex_code =  UH.CatchTexture2DFromImage(self.QRCode)

    -- 框底截取
    local tex_frame = UH.CatchTexture2DFromImage(self.QRBg)

    -- 字体拼合到框底上
    -- local lbl_pos_ = self.ScreenMark.transform.anchoredPosition
    -- local tex_temp = UH.PieceTexture(tex_frame,tex_lbl,lbl_pos_.x-1,lbl_pos_.y-1)

    -- -- 二维码拼合到框底上
    local code_pos_ = self.QRCode.gameObject:GetComponent(typeof(UnityEngine.Transform)).anchoredPosition
    local tex_temp = UH.PieceTexture(tex_frame,tex_code,code_pos_.x,code_pos_.y)

    -- -- 背景获取
    local tex_bg = UH.CatchTexture2DFromImage(self.ShareBg)

    -- -- 框底拼合到背景上
    local qrbg_pos_ = self.QRBg.gameObject:GetComponent(typeof(UnityEngine.Transform)).anchoredPosition
    tex_temp = UH.PieceTexture(tex_bg,tex_temp,qrbg_pos_.x,qrbg_pos_.y)

    UH.SavePhotoToPicture(tex_temp,"share_game")
end

function ServiceShareGame:OnClickCopyLink()
    UnityEngine.GUIUtility.systemCopyBuffer = self.Link.text
end