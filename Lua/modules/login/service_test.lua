ServiceTest = ServiceTest or DeclareView("ServiceTest", "login/service_test",0,ViewLayer.Top)
function ServiceTest:ServiceTest()

end 

function ServiceTest:LoadCallback(param_t)
    self.board:SetData(self:GetType(),Language.Login.LoginUserProtocolTitle)
    
end 