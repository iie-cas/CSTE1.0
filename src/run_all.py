# coding:utf-8


from cste import *
from FontSytle import use_style, print_style
from DefenseMechanism import *
import os
import cmd

PROMPT = 'CSTE> '
HELP = use_style("'help'", mode='bold', fore='red')
HELP_NAME = use_style("'help name'", mode='bold', fore='red')
VERSION = use_style('version 0.1(x86-64, ubuntu16.04)', mode='bold', fore='red')

cste = CSTE()
cste.init()
os.chdir(ROOTPATH)  # 目录切到根目录

class Ui(cmd.Cmd):

    doc_leader = '''\nCSTE, %s\n''' % VERSION
    doc_header = '''Type '%s' to find out more about the command name, command list:\n''' % HELP_NAME
    nohelp = 'help: no help topics match %s. Try help to list commands.'
    ruler = None

    def __init__(self):
        cmd.Cmd.__init__(self)
        self.intro = '\nWelcome to the CSTE shell. Type %s to list commands.\n' % HELP
        self.prompt = PROMPT

        # 这里列出了支持的所有命令，以及每条命令支持的参数
        self.func_and_args = {
            'show': ['samples'],
            'compile': ['all'] + cste.samples.keys(),
            'run': ['all'] + cste.samples.keys(),
            'aslr': ['status', 'on', 'off'],
            'bye': [''],
            'refresh': [''],
            'ls': [''],
            'vim': [''],
        }

    def arg_is_error(self, func_name, arg):
        """
        判断参数是否错误
        func_name: 函数名
        arg: 参数名
        如果命令对参数有要求，那么可以调用这个函数
        """
        if arg not in self.func_and_args[func_name]:
            print_style('\n[ERROR] Parameter Error.', fore='red')
            print_style("[TIP] Try 'help %s' for more information." % func_name, fore='green')
            return True
        else:
            return False

    def print_help_message(self, command):
        """
        打印帮助信息
        :param command: 命令
        :return: None
        """
        print('\n%s [args]' % command)
        print('[args] = %s\n' % (self.func_and_args['%s' % command]))
    
    def do_help(self, arg):
        'List available commands with "help" or detailed help with "help cmd".'
        if arg:
            # XXX check arg syntax
            try:
                func = getattr(self, 'help_' + arg)
            except AttributeError:
                try:
                    doc=getattr(self, 'do_' + arg).__doc__
                    if doc:
                        self.stdout.write("%s\n"%str(doc))
                        return
                except AttributeError:
                    pass
                self.stdout.write("%s\n"%str(self.nohelp % (arg,)))
                return
            func()
        else:
            names = self.get_names()
            cmds_doc = []
            cmds_undoc = []
            help = {}
            for name in names:
                if name[:5] == 'help_':
                    help[name[5:]]=1
            names.sort()
            # There can be duplicates if routines overridden
            prevname = ''
            for name in names:
                if name[:3] == 'do_':
                    if name == prevname:
                        continue
                    prevname = name
                    cmd=name[3:]
                    if cmd in help:
                        cmds_doc.append(cmd)
                        del help[cmd]
                    elif getattr(self, name).__doc__:
                        cmds_doc.append(cmd)
                    else:
                        cmds_undoc.append(cmd)
            print(self.doc_leader)
            print(self.doc_header)
            for index, cmd_name in enumerate(cmds_doc):
                print('%s. %s' %(index+1, cmd_name))
            print('')
    
    def do_refresh(self, arg):
        """
        刷新
        """
        if not self.arg_is_error('refresh', arg):
            global cste
            cste = CSTE()
            cste.init()

    def help_refresh(self, arg):
        self.print_help_message('refresh')

    def do_show(self, arg):
        """
        显示样例信息
        :param arg: show命令的参数
        :return: None
        """
        if not self.arg_is_error('show', arg):
            if arg == 'samples':
                cste.show_all_samples()

    def help_show(self):
        """
        打印show命令的帮助信息
        :return: None
        """
        self.print_help_message('show')

    def do_compile(self, arg):
        """
        编译样例
        :param arg: 编译命令的参数
        :return: None
        """
        if not self.arg_is_error('compile', arg):
            if arg == 'all':
                cste.compile_samples()
            else:
                cste.compile_sample(arg)
        os.chdir(ROOTPATH)

    def help_compile(self):
        self.print_help_message('compile')

    def do_run(self, arg):
        """
        执行样例
        """
        if not self.arg_is_error('run', arg):
            if arg == 'all':
                cste.run_samples()
            else:
                cste.run_sample(arg)
        os.chdir(ROOTPATH)

    def help_run(self):
        self.print_help_message('run')

    def do_aslr(self, arg):
        router = {
            'status': ASLR().show_status,
            'on': ASLR().on,
            'off': ASLR().off
        }
        if not self.arg_is_error('aslr', arg):
            router[arg]()

    def help_aslr(self):
        self.print_help_message('aslr')

    def do_bye(self, arg):
        if not self.arg_is_error('bye', arg):
            print('\nThank you for using CSTE! bye.')
            return True

    def help_bye(self):
        self.print_help_message('bye')

    def do_ls(self, arg):
        try:
            os.system('ls %s' %arg)
        except:
            print('[ERROR] 参数错误')

    def help_ls(self):
        self.print_help_message('ls')

    def do_cd(self, arg):
        try:
            os.chdir(arg)
        except:
            print('[ERROR] 参数错误')

    def help_cd(self):
        self.print_help_message('cd')

    def do_vim(self, arg):
        try:
            os.system('vim %s' %arg)
        except:
            print('[ERROR] 参数错误')
        self.do_refresh('')

    def help_vim(self):
        self.print_help_message('vim')

ui = Ui()
ui.cmdloop()
