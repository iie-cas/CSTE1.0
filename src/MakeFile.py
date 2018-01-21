# coding: utf-8


class Makefile:
    """
    makefile
    """
    template = """\
IDIR = ./include
ODIR = obj
CFLAGS = -I$(IDIR)
CC = gcc

_DEPS = {inc_files}
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))

_OBJ = {obj_files}
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
\t$(CC) -c -g -o $@ $< $(CFLAGS) {compile_opts}

vul: $(OBJ)
\t$(CC) -g -o $@ $^ $(CFLAGS) {compile_opts}

.PHONY: clean
clean: 
\trm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
"""

    def __init__(self):
        self.values = {
            'inc_files': '',
            'obj_files': '',
            'compile_opts': ''
        }
        self.compile_nav = {
            'dep': '-z execstack',
            'sp': '-fno-stack-protector'
        }

    def create_makefile(self, cste, sample):
        self.values['inc_files'] = ' '.join(sample.inc_files)
        self.values['obj_files'] = ' '.join(sample.obj_files)
        compile_opts = []

        if cste.enable.en_defense_conf == 'on':
            if cste.defense.dep == 'off':
                compile_opts.append(self.compile_nav['dep'])
            if cste.defense.sp == 'off':
                compile_opts.append(self.compile_nav['sp'])
        else:
            if sample.attack.dep == 'off':
                compile_opts.append(self.compile_nav['dep'])
            if sample.attack.sp == 'off':
                compile_opts.append(self.compile_nav['sp'])

        # 各个样例扩展的编译选项
        if sample.attack.others != '':
            compile_opts.extend(sample.attack.others.split(','))
        self.values['compile_opts'] = ' '.join(compile_opts)

        with open(sample.path + '/vul_makefile', 'w') as f:
            f.write(Makefile.template.format(**self.values))
