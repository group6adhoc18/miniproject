import threading
import time
import subprocess

timeout = 15
interface = 'enp1s0'
c = threading.Condition()
flag = False
devices = []
configFile = ""
detectionFile = ""


class TsharkSniffer(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self._stopevent = threading.Event(  )
    
    def run(self):
        global flag
        global devices
        global cs
        
        allMacs = []
        
        self.tshark = subprocess.Popen(['tshark -e eth.src -Tfields -j "eth.src" -i enp1s0'], shell=True, stdout=subprocess.PIPE, bufsize=1, universal_newlines=True)
        while self.tshark.poll() is None and not self._stopevent.isSet(  ):
            mymac = self.tshark.stdout.readline().replace('\n', '')
            if len(mymac) !=17:
                continue
            c.acquire()
            
            if mymac not in allMacs:
                print("new device in network", mymac)
                allMacs.append(mymac)
            
            for device in devices:
                if device['mac'] == mymac:
                    if device['time'] == 0:
                        print('Find device: ' + mymac)
                    now = time.time()
                    device['time'] = now
                    flag = True
            c.release()
            
            with open(detectionFile, 'w') as f:
                for mac in allMacs:
                    f.write("{}\n".format(mac))
                f.close()
    
    def join(self, timeout=None):
        """ Stop the thread. """
        self._stopevent.set(  )
        self.tshark.kill()
        threading.Thread.join(self, timeout)


class ListCleaner(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self._stopevent = threading.Event(  )
    
    def run(self):
        global flag
        global devices
        global configFile
        global c
        
        while not self._stopevent.isSet(  ):
            now = time.time()
            earlistPopUp = now - timeout
            
            #update list
            confFile = open(configFile, "r")
            newdevices= []
            for mac in confFile:
                mac = mac.replace('\n', '')
                if len(mac) !=17:
                    print("invalid mac!")
                    continue
                newdevices.append(mac)
            confFile.close()
            
            olddevices = list(map(lambda d: d['mac'], devices))
            skipdevices = set(olddevices) - set(newdevices)
            adddevices = set(newdevices) - set(olddevices)
            
            for mac in skipdevices:
                index = olddevices.index(mac)
                devices = devices[:index] + devices[index+1 :]
                print("delete unused trigger device: ", mac)
            
            for mac in adddevices:
                print("add new trigger device: ", mac)
                devices.append({'mac': mac, 'time': 0})
            
            c.acquire()
            flag = False
            for device in devices:
                if device['time'] > earlistPopUp:
                    flag = True
                else:
                    if device['time'] > 0:
                        device['time'] = 0
                        print('Device is gone: ' + device['mac'])
            c.release()
            
            with open(detectionFile, 'w') as f:
                for mac in devices:
                    f.write("{}\n".format(mac['mac']))
                f.close()
            
            time.sleep(5)
    
    def join(self, timeout=None):
        """ Stop the thread. """
        self._stopevent.set(  )
        threading.Thread.join(self, timeout)


class MACSniffer(object):
    def __init__(self, confile, detectFile): 
        global devices
        global configFile
        global detectionFile
        
        detectionFile = detectFile
        configFile = confile
        
        print("read MAC-Addresses from file " + configFile)
        confFile = open(configFile, "r")
        
        for mac in confFile:
            if len(mac) !=18:
                print("invalid mac!")
                continue
            
            print("add " + mac)
            
            devices.append({'mac': mac, 'time': 0})
        confFile.close()
        self.sniffer = TsharkSniffer()
        self.cleaner = ListCleaner()
        self.sniffer.start()
        self.cleaner.start()

    
    def detect_mac(self):
        global flag
        global c
        c.acquire()
        myflag = flag
        c.release()
        return myflag
    
    def exit(self):
        self.sniffer.join()
        self.cleaner.join()


