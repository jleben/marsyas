
class Defs:
    def __init__(self):
        self.BPM_MIN = 40
        self.BPM_MAX = 180  # marsyas
        
        self.OSS_WINDOWSIZE = 256
        self.OSS_HOPSIZE = 128
        
        self.OSS_LOWPASS_CUTOFF = 6.0 # Hz
        #self.OSS_LOWPASS_CUTOFF = 0 # Hz
        self.OSS_LOWPASS_N = 15
        
        self.BH_WINDOWSIZE = 2048
        self.BH_HOPSIZE = 128
        
        self.BP_WINDOWSIZE = 2048
        self.BP_HOPSIZE = 128

        self.DOUBLE_TYPE = 2
 
        self.WRITE_ONSETS = 1
        self.WRITE_BH = 1
        self.WRITE_BP = 1

        self.OPTIONS_ONSET = 2
        self.OPTIONS_BH = 2
        self.OPTIONS_BP = 2
        
        self.extra = []

