
# Base object
class dolphin:
    def __init__(self):
        pass


import dolphin

flyingMode = False;

def FrameCallback(cpuContext):
    # This is executed once per frame, right on vsync
    flyingMode = (cpu.mem[0x80230100].byte() == 3)
    pass

def FnHook(cpu):
    if(cpu.r5 == 1): # if arg2 == 1
        cpu.mem[cpu.r3].setWord(0) # set *arg0 to null
        cpu.r3 = -1 # return -1
        cpu.return() # force Return from function
        return
    # otherwise, do nothing

def InvertControls(controlData):
    # controller hook doesn't have access to cpu state, but we saved the current
    # control mode in vsync hook

    if (flyingMode):
        # invery y axis of player one 
        y = controlData.controller[0].y
        controlData.controller[0].y = 255 - y

dolphin.events.vsync(0).on(FrameCallback) # Called 0 cycles after vsync
dolphin.events.pc(0x80033024).on(FnHook)  # Called whenever pc = 0x80033024
dolphin.events.controllerRead.after(InvertControls) # called whenver the controller registers are latched

# 
dolphin.event.newTev.on()