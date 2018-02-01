# coding:utf-8

import os
import re
import codecs
import time
import ConfigParser
import shutil
from FontSytle import use_style
from MakeFile import *
from DefenseMechanism import *
import threading
import socket
import psutil
import types
import datetime
import signal
from pwn import *

ROOTPATH = os.getcwd()[:-4]
WHITE_SPACE = re.compile('[\s]+')


class ConfigFileReader:
    def __init__(self, conf_file):
        self.secs = {}
        self.cf = ConfigParser.ConfigParser()
        self.cf.read(conf_file)
        for sec_name in self.cf.sections():
            self.secs[sec_name] = self.cf.items(sec_name)


class Config:
    def __init__(self):
        self.config = {}

    def get(self, cfg_name):
        return self.config[cfg_name]

    def edit(self, cfg_name, cfg_value):
        self.config[cfg_name] = cfg_value

    def set(self, sec):
        for item in sec:
            value = re.sub(WHITE_SPACE, ' ', item[1])
            self.config[item[0]] = value


class Attack:
    def __init__(self, attack_sec):
        self.conf = Config()
        self.conf.set(attack_sec)
        self.name = self.conf.config['name']  # 攻击名称
        self.target = self.conf.config['target']  # 攻击目标
        self.way = self.conf.config['way']  #攻击方式
        self.dep = self.conf.config['dep']  # 开关 DEP
        self.aslr = self.conf.config['aslr']  # 开关 ASLR
        self.sp = self.conf.config['stack_protector']  # 开关栈保护
        self.others = self.conf.config['others']  # 其他编译选项


class Vul:
    def __init__(self, vul_sec):
        self.conf = Config()
        self.conf.set(vul_sec)
        self.type = self.conf.config['type']  # 漏洞类型
        self.args = self.conf.config['args']  # 漏洞程序执行需要的参数
        self.port = 20000  # 默认端口 20000

class Analyse:
    def __init__(self, analyse_sec):
        self.conf = Config()
        self.conf.set(analyse_sec)
        self.analyse = self.conf.config['analyse']
        self.basp = self.conf.config['basp']
        self.qemu = self.conf.config['qemu']
        self.qemu_args = self.conf.config['qemu_args']


class CSTEDefense:
    def __init__(self, defense_sec):
        self.conf = Config()
        self.conf.set(defense_sec)
        self.dep = self.conf.config['dep']
        self.aslr = self.conf.config['aslr']
        self.sp = self.conf.config['stack_protector']


class CSTEEnable:
    def __init__(self, enable_sec):
        self.conf = Config()
        self.conf.set(enable_sec)
        self.en_defense_conf = self.conf.config['defense']
        self.en_analyse_conf = self.conf.config['analyse']


class Sample:
    def __init__(self, sample_name):
        self.name = sample_name
        self.path = '%s/samples/%s' % (ROOTPATH, sample_name)
        self.inc_path = '%s/include' % self.path
        self.exp_path = '%s/exploit' % self.path
        self.obj_path = '%s/obj' % self.path
        self.output_path = '%s/output' % self.path
        self.basp_path = '%s/basp' %self.path
        self.report_path = '%s/report' %self.path
        self.conf_file = '%s/my.conf' % self.path
        self.vul_report = '%s/vul.report' % self.report_path  # 漏洞程序运行报告
        self.attack_report = '%s/attack.report' % self.report_path  # 攻击程序运行报告
        self.qemu_report = '%s/qemu.report' % self.report_path  # qemu 分析报告
        self.compile_report = '%s/compile.report' % self.report_path  # 编译报告
        self.inc_files = []  # 所有的头文件的文件名
        self.src_files = []  # 所有的源文件的文件名
        self.obj_files = []  # 所有的中间文件的文件名
        self.cf = ConfigFileReader(self.conf_file)
        try:
            self.vul = Vul(self.cf.secs['vul'])
        except:
            print('%s 配置文件有错误' %(self.name))
        self.attack = Attack(self.cf.secs['attack'])
        self.analyse = Analyse(self.cf.secs['analyse'])
        self.qemu_process = None
        self.attack_process = None
        self.angr_process = None

    def init(self):
        Sample.mkdir(self.inc_path)
        Sample.mkdir(self.obj_path)
        Sample.mkdir(self.exp_path)
        Sample.mkdir(self.output_path)
        Sample.mkdir(self.report_path)
        Sample.mkdir(self.basp_path)
        self.set_files()

    @staticmethod
    def mkdir(path):
        if not os.path.exists(path):
            os.makedirs(path)

    def set_files(self):
        for file_t in os.listdir(self.inc_path):
            if os.path.splitext(file_t)[1] == '.h':
                self.inc_files.append(file_t)
        for file_t in os.listdir(self.path):
            if os.path.splitext(file_t)[1] == '.c':
                self.src_files.append(file_t)
                self.obj_files.append(file_t.replace('.c', '.o'))
        os.chdir(self.report_path)
        if 'vul.report' not in os.listdir('./'):
            os.mknod('vul.report')
        if 'attack.report' not in os.listdir('./'):
            os.mknod('attack.report')
        if 'qemu.report' not in os.listdir('./'):
            os.mknod('qemu.report')
        if 'compile.report' not in os.listdir('./'):
            os.mknod('compile.report')
        if len(self.src_files) == 0:
            print('[ERROR]:　%s 缺少C文件' % self.name)
        os.chdir(ROOTPATH)

class CSTE:

    def __init__(self):
        self.conf_file = '%s/cste.conf' % ROOTPATH
        self.log_file = '%s/cste.log' % ROOTPATH
        self.samples_path = '%s/samples' % ROOTPATH
        self.tool_path = '%s/tools' % ROOTPATH
        self.samples = {}
        self.ports = {}

    @staticmethod
    def run_cmd(cmd):
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        return p

    def init(self):
        self.cf = ConfigFileReader(self.conf_file)
        self.defense = CSTEDefense(self.cf.secs['defense'])
        self.analyse = Analyse(self.cf.secs['analyse'])
        self.enable = CSTEEnable(self.cf.secs['enable'])
        self.makefile = Makefile()
        self.aslr = ASLR()
        self.set_samples()

    def set_samples(self):
        """
        初始化所有 sample，并将所有 sample 放入字典
        """
        sample_names = os.listdir(self.samples_path)
        for sample_name in sample_names:
            if not os.path.isfile(self.samples_path + sample_name):
                sample = Sample(sample_name)
                sample.init()
                self.samples[sample_name] = sample

    def create_makefile(self, sample_name):
        """
        创建 makefile 文件
        """
        sample = self.samples[sample_name]
        self.makefile.create_makefile(self, sample)

    def is_exist_binary(self, sample_name):
        """
        判断是否存在可执行文件
        """
        sample = self.samples[sample_name]
        if 'vul' in os.listdir(sample.path):
            return True

    def compile_sample(self, sample_name):
        """
        编译样例
        """
        sample = self.samples[sample_name]
        self.create_makefile(sample_name)
        os.chdir(sample.path)
        print('%s compile %s start.' %(use_style('[compile]', fore='green'), sample.name))
        p1 = CSTE.run_cmd('make clean -f vul_makefile')
        (stdoutput1, erroutput1) = p1.communicate()
        fw = open(sample.compile_report, 'w')
        for line in stdoutput1:
            fw.write(line)
        for line in erroutput1:
            fw.write(line)
        p2 = CSTE.run_cmd('make -f vul_makefile')
        (stdoutput2, erroutput2) = p2.communicate()
        for line in stdoutput2:
            fw.write(line)
        for line in erroutput2:
            fw.write(line)
        print(use_style('[command]:', fore='cyan'))
        print(stdoutput1.strip())
        print(stdoutput2.strip())
        print('%s compile %s done.\n' % (use_style('[compile]', fore='green'), sample.name))
        os.chdir(ROOTPATH)

    def compile_samples(self):
        """
        编译所有样例
        """
        # 预先分配端口
        self.assign_port()
        for sample_name in self.samples:
            self.compile_sample(sample_name)

    def basp(self, sample_name):
        sample = self.samples[sample_name]
        os.chdir(sample.path)
        print('%s basp analyse start.' % (use_style('[basp]', fore='green')))
        p = CSTE.run_cmd('%s/basp -o %s vul' %(self.tool_path, sample.basp_path))
        p.communicate()
        print('%s basp analyse done.' % (use_style('[basp]', fore='green')))
        os.chdir(ROOTPATH)

    def qemu(self, sample_name, analyse_config):
        sample = self.samples[sample_name]
        os.chdir(sample.path)
        flag_call = True
        flag_func = True
        if 'vul.call' not in os.listdir(sample.basp_path):
                print("%s file 'vul.call' not exist." % (use_style('[qemu]', fore='red')))
                flag_call = False
        if 'vul.func' not in os.listdir(sample.basp_path):
                print("%s file 'vul.func' not exist." % (use_style('[qemu]', fore='red')))
                flag_func = False
        # QEMU 依赖文件存在
        if flag_call and flag_func:
            print('%s qemu analyse start.' % (use_style('[qemu]', fore='green')))
            print('[QEMU CMD] %s %s >%s 2>%s' % (analyse_config.qemu_args, sample.vul.args,
                                                     sample.vul_report, sample.qemu_report))
            p = CSTE.run_cmd('%s %s >%s 2>%s' % (analyse_config.qemu_args, sample.vul.args,
                                                     sample.vul_report, sample.qemu_report))
            sample.qemu_process = p
            self.samples[sample_name] = sample
            p.communicate()
            print('%s qemu analyse done.' % (use_style('[qemu]', fore='green')))
        os.chdir(ROOTPATH)

    def run_analyse(self, sample_name, analyse_config):
        if analyse_config.basp == 'on':
            self.basp(sample_name)
        if analyse_config.qemu == 'on':
            t_qemu = threading.Thread(target=self.qemu, args=(sample_name, analyse_config))
            t_qemu.start()
            # 延时3秒，等漏洞程序监听在端口以后，再执行攻击脚本，因为漏洞程序设计的原因，无法检测漏洞程序是否监听在了端口。所以只能通过延时。
            time.sleep(2)

    def run_vul(self, sample_name):
        """
        执行漏洞程序
        sample_name: 漏洞程序的名称（所在的文件夹名）
        """
        sample = self.samples[sample_name]
        os.chdir(sample.path)
        p = CSTE.run_cmd('./vul > %s' % (sample.vul_report))
        sample.vul_process = p
        self.samples[sample_name] = sample
        os.chdir(ROOTPATH)

    def run_noanalyse(self, sample_name):
        """
        不进行分析，直接执行漏洞程序
        """
        t = threading.Thread(target=self.run_vul, args=(sample_name,))
        t.start()
        time.sleep(2)

    def run_attack(self, sample_name):
        """
        执行攻击
        """
        sample = self.samples[sample_name]
        os.chdir(sample.exp_path)
        print('%s attack start.' % (use_style('[attack]', fore='green')))
        p = CSTE.run_cmd('python %s  > %s 2>&1' % (sample.attack.way, sample.attack_report))
        sample.attack_process = p
        self.samples[sample_name] = sample
        p.communicate()
        print('%s attack done.' % (use_style('[attack]', fore='green')))
        os.chdir(ROOTPATH)

    def run_sample(self, sample_name):
        """
        执行一个例子
        """
        sample = self.samples[sample_name]
        os.chdir(sample.path)
        print('%s %s start.' % (use_style('[cste]', fore='green'), sample_name))
        # 清空之前的输出
        self.clear_output(sample_name)
        # 判断二进制文件是否存在
        if not self.is_exist_binary(sample_name):
            print('[TIP] 可执行文件不存在，自动编译执行')
            self.compile_sample(sample_name)
        # 配置 ASLR
        if self.enable.en_defense_conf == 'on':
            aslr_config = self.defense.aslr
        else:
            aslr_config = sample.attack.aslr
        if aslr_config == 'on':
            self.aslr.on()
        else:
            self.aslr.off()
        # 打印 ASLR 状态
        if self.aslr.status() == 2:
            print('[TIP] ASLR ON')
        else:
            print('[TIP] ASLR OFF')
        # analyse
        if self.enable.en_analyse_conf == 'on':
            print('[TIP] 分析过程启用了全局配置，配置以 cste.conf 为主')
            analyse_config = self.analyse
        else:
            print('[TIP] 分析过程启用了局部配置，配置以各样例的 my.conf 为主')
            analyse_config = sample.analyse
        # 如果开启了分析功能
        if analyse_config.analyse == 'on':
            self.run_analyse(sample_name, analyse_config)
        else:
            self.run_noanalyse(sample_name)
        # 后期加的延时
        time.sleep(2)
        # 执行攻击
        t_attack = threading.Thread(target=self.run_attack, args=(sample_name,))
        t_attack.start()
        # 延时3秒，等待攻击完成
        time.sleep(2)
        sample = self.samples[sample_name]  # 刷新 sample，这里刷新是为了获取 attack_process
        #try:
         #   sample.qemu_process.kill()
          #  print('kill qemu process')
        #except:
         #   pass
        #try:
         #   sample.vul_process.kill()
          #  print('kill vul process')
        #except:
         #   pass
        #try:
         #   sample.attack_process.kill()
          #  print('kill attack process')
        #except:
         #   pass
        os.chdir(ROOTPATH)

    def port_is_inuse(self, port):
        """
        判断端口是否被占用
        """
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        flag = True
        try:
            s.connect(('127.0.0.1', int(port)))
            s.shutdown(2)
            flag = True
        except:
            flag = False
        return flag

    def assign_port(self):
        """
        分配端口
        """
        print("[TIP] 正在为所有程序分配端口")
        port_list = [i for i in range(41000, 42000)]
        flag = re.compile(u'服务器端口号需提取')
        port_pattern = re.compile(u'[0-9]{4,5}')
        for sample_name in self.samples:
            sample = self.samples[sample_name]
            for port in port_list:
                # 判断端口是否被占用
                if self.port_is_inuse(port):
                    port_list.remove(port)
                else:
                    os.chdir(sample.path)
                    # 原本假设漏洞程序可以有多个源文件，但是从漏洞程序设计的实际情况来看，只有一个，这里还没有进行修改
                    # 用一个文件进行过渡
                    c_file = sample.src_files[0]
                    fr = codecs.open(c_file, 'r', encoding='utf-8')
                    fw = codecs.open(c_file + '.copy', 'w', encoding='utf8')
                    lines = [line for line in fr.readlines()]
                    for i, line in enumerate(lines):
                        if re.search(flag, line):
                            lines[i] = re.sub(port_pattern, str(port), line)
                    for line in lines:
                        fw.write(line)
                    fw.close()
                    os.system('rm %s' % c_file)
                    os.system('mv %s %s' %(c_file + '.copy', c_file))

                    os.chdir(sample.exp_path)
                    fr = codecs.open(sample.attack.way, 'r', encoding='utf-8')
                    fw = codecs.open(sample.attack.way+'.copy', 'w', encoding='utf8')
                    lines = [line for line in fr.readlines()]
                    for i, line in enumerate(lines):
                        if re.search(flag, line):
                            lines[i] = re.sub(port_pattern, str(port), line)
                    for line in lines:
                        fw.write(line)
                    fw.close()
                    os.system('rm %s' %sample.attack.way)
                    os.system('mv %s %s' %(sample.attack.way + '.copy', sample.attack.way))
                    port_list.remove(port)
                    self.ports[sample_name] = port
                    break
        for sample_name in self.samples:
            print('[PORT]: %s, %s' %(sample_name, self.ports[sample_name]))
        print("[TIP] 端口分配结束")
        os.chdir(ROOTPATH)

    def run_samples(self):
        """
        执行所有漏洞程序
        """
        # 预先分配端口
        self.assign_port()
        for sample_name in self.samples:
            self.run_sample(sample_name)
        # 统计
        self.count()

    def show_all_samples(self):
        """
        打印所有例子名称
        """
        print('')
        for i, sample_name in enumerate(self.samples.keys()):
            print ('%s. %s' %(use_style('%s' %i, fore='green'), sample_name))
        print('')

    def count(self):
        """
        这个函数目前的处理方式非常的简单粗暴，以后再做修改
        """
        fw = codecs.open(self.log_file, 'w', encoding='utf-8')
        defense = None
        if self.enable.en_defense_conf == 'on':
            defense = self.defense
            fw.write('DEFENSE: \n')
            fw.write('\n')
            fw.write('1. ASLR: %s\n' %(defense.aslr))
            fw.write('2. DEP: %s\n' %(defense.dep))
            fw.write('3. STACK PROTECT: %s\n' %(defense.sp))
            fw.write('\n')
        success_num = 0
        failed_num = 0
        for sample_name in self.samples:
            sample = self.samples[sample_name]
            # 如果攻击目标是 print
            if sample.attack.target == 'print':
                fr = open(sample.vul_report, 'r')
                lines = fr.readlines()
                FLAG = False
                for line in lines:
                    if 'hello' in line or 'aaaaaaaaaa' in line:
                        fw.write('[%s] attack success!\n' % sample_name)
                        success_num += 1
                        FLAG = True
                        break
                if not FLAG:
                    fw.write('[%s] attack failed!\n' % sample_name)
                    failed_num += 1

            # 如果攻击的目标是 create
            elif sample.attack.target == 'create':
                if 'out.txt' in os.listdir(sample.output_path):
                    fw.write('[%s] attack success!\n' % sample_name)
                    success_num += 1
                else:
                    fw.write('[%s] attack failed!\n' % sample_name)
                    failed_num += 1

            # 如果攻击的目标是 ls
            elif sample.attack.target == 'ls':
                fr = open(sample.vul_report, 'r')
                lines = fr.readlines()
                FLAG = False
                for line in lines:
                    if 'my.conf' in line:
                        fw.write('[%s] attack success!\n' % sample_name)
                        success_num += 1
                        FLAG = True
                        break
                if not FLAG:
                    fw.write('[%s] attack failed!\n' % sample_name)
                    failed_num += 1
        fw.write('\nSUCCESS NUM: %s\n' %success_num)
        fw.write('FAILED NUM: %s\n' %failed_num)
        fw.write('SUCCESS RATE: %s\n' %(float(success_num)/(success_num + failed_num)))
        fw.close()

    def clear_output(self, sample_name):
        """
        清空之前的输出，包括：
            - output 文件夹下的所有内容
            - report 文件夹下的所有内容
            - basp 文件夹下的所有内容
        """
        sample = self.samples[sample_name]
        # output 文件夹
        shutil.rmtree(sample.output_path)
        os.mkdir(sample.output_path)
        # basp 文件夹
        shutil.rmtree(sample.basp_path)
        os.mkdir(sample.basp_path)
        # report 文件夹
        shutil.rmtree(sample.report_path)
        os.mkdir(sample.report_path)
