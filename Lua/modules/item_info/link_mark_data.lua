LinkMarkData = LinkMarkData or BaseClass()

function LinkMarkData:__init()
    if LinkMarkData.Instance ~= nil then
        Debuger.LogError("[LinkMarkData] attempt to create singleton twice!")
        return
    end
    LinkMarkData.Instance = self

    self.Intro_info = {}
end

-- mark first
function LinkMarkData:MarkIntro(param_t)
    self.Intro_info.param = param_t
end 

-- check for sure target
function LinkMarkData:MidCheck()
    if self.Intro_info.endtro_mod == nil then 
        self:ClearMark()
    end 
end 

function LinkMarkData:MarkIntroPlus(_name,val)
    if self.Intro_info.param then 
        self.Intro_info[_name] = val
    end 
end 

-- special plus 
function LinkMarkData:PlusIngites(_mod)

end 

function LinkMarkData:ClearMark() 
    self.Intro_info = {}
end 

-- complete the linkmark
function LinkMarkData:Ignites(_mod)
    if _mod == self.Intro_info.endtro_mod then 
        self:PlusIngites(_mod)
        if self.Intro_info.param and self.Intro_info.param.end_func ~= nil then 
            self.Intro_info.param.end_func()
        end 
    end 
    self:ClearMark() 
end 
