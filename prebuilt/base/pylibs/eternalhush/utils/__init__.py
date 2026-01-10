import _eternalhush
import time
import asyncio
import subprocess

def SystimeToDatetime(ms):
    return _eternalhush.ctxObj.systime_to_datetime(ms)

class Timer:
    def __init__(self):
        self.start_time = 0
        
    def set_time(self, t):
        self.start_time = t
    
    def start(self):
        self.start_time = time.time()
        
    def time(self):
        return int(time.time()-self.start_time)
    
class CallbackTimer:
    def __init__(self, timeout, callback, parameter=None):
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        self._timeout = timeout
        self._callback = callback
        self._param = parameter
        self._task = asyncio.ensure_future(self._job())

    async def _job(self):
        await asyncio.sleep(self._timeout)
        if self._param != None:
            await self._callback(self._param)
        else:
            await self._callback()

    def stop(self):
        self._task.cancel()

def KillProcess(pid):
    subprocess.call(['taskkill', '/F', '/T', '/PID',  str(pid)])