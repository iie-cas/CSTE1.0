# coding: utf-8

import subprocess


class ASLR:

    def __init__(self):
        pass

    def status(self):
        p = subprocess.Popen('cat /proc/sys/kernel/randomize_va_space', stdout=subprocess.PIPE, shell=True)
        ans = p.stdout.read()
        return int(ans[0])

    def show_status(self):
        if self.status() == 2:
            print 'ASLR STATUS: ON'
        else:
            print 'ASLR STATUS: OFF'

    def on(self):
        if self.status() == 2:
            return
        print 'ASLR>>ON, may need password.\n'
        p = subprocess.Popen('sudo sysctl -w kernel.randomize_va_space=2', stdin=subprocess.PIPE, shell=True)
        p.wait()

    def off(self):
        if self.status() == 0:
            return
        print 'ASLR>>OFF, may need password.\n'
        p = subprocess.Popen('sudo sysctl -w kernel.randomize_va_space=0', stdin=subprocess.PIPE, shell=True)
        p.wait()


