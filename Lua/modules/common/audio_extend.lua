
AudioExtend = {}

function AudioExtend.IsPlaying(ap)
    NirvanaLoad.CheckNirvanaRuntime()
    local NirvanaIAudioPlayer = typeof('Nirvana.IAudioPlayer')
    local property = tolua.getproperty(NirvanaIAudioPlayer, 'IsPlaying')
    local value = property:Get(ap, nil)
    property:Destroy()
    return value
end