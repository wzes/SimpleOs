
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"


/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
    disp_str("-----\"kernel_main\" begins-----\n");

    struct task* p_task;
    struct proc* p_proc= proc_table;
    char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
    u16   selector_ldt = SELECTOR_LDT_FIRST;
    u8    privilege;
    u8    rpl;
    int   eflags;
    int   i, j;
    int   prio;

    //似乎可以加开机动画

    //启动进程
    for (i = 0; i < NR_TASKS+NR_PROCS; i++)
    /*for (i = 0; i < NR_TASKS; i++)*/
    {
        if (i < NR_TASKS)
        {   /* 任务 */
            p_task    = task_table + i;
            privilege = PRIVILEGE_TASK;
            rpl       = RPL_TASK;
            eflags    = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1   1 0010 0000 0010(2)*/
            prio      = 15;     //设定优先级为15
        }
        else
        {   /* 用户进程 */
            p_task    = user_proc_table + (i - NR_TASKS);
            privilege = PRIVILEGE_USER;
            rpl       = RPL_USER;
            eflags    = 0x202; /* IF=1, bit 2 is always 1              0010 0000 0010(2)*/
            prio      = 5;     //设定优先级为5
        }

        strcpy(p_proc->name, p_task->name); /* 设定进程名称 */
        p_proc->pid = i;            /* 设定pid */

        p_proc->ldt_sel = selector_ldt;

        memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
               sizeof(struct descriptor));
        p_proc->ldts[0].attr1 = DA_C | privilege << 5;
        memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
               sizeof(struct descriptor));
        p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
        p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

        p_proc->regs.eip = (u32)p_task->initial_eip;
        p_proc->regs.esp = (u32)p_task_stack;
        p_proc->regs.eflags = eflags;

        p_proc->p_flags = 0;
        p_proc->p_msg = 0;
        p_proc->p_recvfrom = NO_TASK;
        p_proc->p_sendto = NO_TASK;
        p_proc->has_int_msg = 0;
        p_proc->q_sending = 0;
        p_proc->next_sending = 0;

        for (j = 0; j < NR_FILES; j++)
            p_proc->filp[j] = 0;

        p_proc->ticks = p_proc->priority = prio;

        p_task_stack -= p_task->stacksize;
        p_proc++;
        p_task++;
        selector_ldt += 1 << 3;
    }

    //初始化进程
    k_reenter = 0;
    ticks = 0;

    p_proc_ready = proc_table;

    init_clock();
    init_keyboard();

    restart();

    while(1){}
}


/*****************************************************************************
 *                                get_ticks
 *****************************************************************************/
PUBLIC int get_ticks()
{
    MESSAGE msg;
    reset_msg(&msg);
    msg.type = GET_TICKS;
    send_recv(BOTH, TASK_SYS, &msg);
    return msg.RETVAL;
}


void shell(char *tty_name){
	 int fd;

    //int isLogin = 0;

    char rdbuf[128];
    char cmd[128];
    char arg1[128];
    char arg2[128];
    char buf[1024];

    int fd_stdin  = open(tty_name, O_RDWR);
    assert(fd_stdin  == 0);
    int fd_stdout = open(tty_name, O_RDWR);
    assert(fd_stdout == 1);

    clear();
   
   if(strcmp(tty_name, "/dev_tty0")==0){
   	printf("                        ==================================\n");
    printf("                                   Aquinux v1.0.0             \n");
    printf("                                 Kernel on Orange's \n\n");
    printf("                                     Welcome !\n");
    printf("                        ==================================\n");
   }
   	

   
    while (1) {  
        //必须要清空数组
        clearArr(rdbuf, 128);
        clearArr(cmd, 128);
        clearArr(arg1, 128);
        clearArr(arg2, 128);
        clearArr(buf, 1024);

        printf("%s@aqu:~$ ", "root");

        int r = read(fd_stdin, rdbuf, 128);

        if (strcmp(rdbuf, "") == 0)
            continue;

        //解析命令
        int i = 0;
        int j = 0;
        while(rdbuf[i] != ' ' && rdbuf[i] != 0)
        {
            cmd[i] = rdbuf[i];
            i++;
        }
        i++;
        while(rdbuf[i] != ' ' && rdbuf[i] != 0)
        {
            arg1[j] = rdbuf[i];
            i++;
            j++;
        }
        i++;
        j = 0;
        while(rdbuf[i] != ' ' && rdbuf[i] != 0)
        {
            arg2[j] = rdbuf[i];
            i++;
            j++;
        }
        //清空缓冲区
        rdbuf[r] = 0;

        if (strcmp(cmd, "process") == 0)
        {
            ProcessManage();
        }
        else if (strcmp(cmd, "help") == 0)
        {
            help();
        }      
        else if (strcmp(cmd, "clear") == 0)
        {
            printTitle();
        }
        else if (strcmp(cmd, "ls") == 0)
        {
            ls();
        }
        else if (strcmp(cmd, "touch") == 0)
        {
            fd = open(arg1, O_CREAT | O_RDWR);
            if (fd == -1)
            {
                printf("Failed to create file! Please check the filename!\n");
                continue ;
            }
            write(fd, buf, 1);
            printf("File created: %s (fd %d)\n", arg1, fd);
            close(fd);
        }
        else if (strcmp(cmd, "cat") == 0)
        {
            fd = open(arg1, O_RDWR);
            if (fd == -1)
            {
                printf("Failed to open file! Please check the filename!\n");
                continue ;
            }
            if (!verifyFilePass(arg1, fd_stdin))
            {
                printf("Authorization failed\n");
                continue;
            }
            read(fd, buf, 1024);
            close(fd);
            printf("%s\n", buf);
        }
        else if (strcmp(cmd, "vi") == 0)
        {
            fd = open(arg1, O_RDWR);
            if (fd == -1)
            {
                printf("Failed to open file! Please check the filename!\n");
                continue ;
            }
            if (!verifyFilePass(arg1, fd_stdin))
            {
                printf("Authorization failed\n");
                continue;
            }
            int tail = read(fd_stdin, rdbuf, 128);
            rdbuf[tail] = 0;

            write(fd, rdbuf, tail+1);
            close(fd);
        }
        else if (strcmp(cmd, "rm") == 0)
        {
            if (!verifyFilePass(arg1, fd_stdin))
            {
                printf("Authorization failed\n");
                continue;
            }
            int result;
            result = unlink(arg1);
            if (result == 0)
            {
                printf("File deleted!\n");
                continue;
            }
            else
            {
                printf("Failed to delete file! Please check the filename!\n");
                continue;
            }
        }
        else if (strcmp(cmd, "cp") == 0)
        {
            //首先获得文件内容
            fd = open(arg1, O_RDWR);
            if (fd == -1)
            {
                printf("File not exists! Please check the filename!\n");
                continue ;
            }
            if (!verifyFilePass(arg1, fd_stdin))
            {
                printf("Authorization failed\n");
                continue;
            }
            int tail = read(fd, buf, 1024);
            close(fd);
            /*然后创建文件*/
            fd = open(arg2, O_CREAT | O_RDWR);
            if (fd == -1)
            {
                //文件已存在，什么都不要做
            }
            else
            {
                //文件不存在，写一个空的进去
                char temp[1024];
                temp[0] = 0;
                write(fd, temp, 1);
                close(fd);
            }
            //给文件赋值
            fd = open(arg2, O_RDWR);
            write(fd, buf, tail+1);
            close(fd);
        }
        else if (strcmp(cmd, "mv") == 0)
        {
            //首先获得文件内容
            fd = open(arg1, O_RDWR);
            if (fd == -1)
            {
                printf("File not exists! Please check the filename!\n");
                continue ;
            }
            if (!verifyFilePass(arg1, fd_stdin))
            {
                printf("Authorization failed\n");
                continue;
            }
            int tail = read(fd, buf, 1024);
            close(fd);
            /*然后创建文件*/
            fd = open(arg2, O_CREAT | O_RDWR);
            if (fd == -1)
            {
                //文件已存在，什么都不要做
            }
            else
            {
                //文件不存在，写一个空的进去
                char temp[1024];
                temp[0] = 0;
                write(fd, temp, 1);
                close(fd);
            }
            //给文件赋值
            fd = open(arg2, O_RDWR);
            write(fd, buf, tail+1);
            close(fd);
            //最后删除文件
            unlink(arg1);
        }   
        else if (strcmp(cmd, "encrypt") == 0)
        {
            fd = open(arg1, O_RDWR);
            if (fd == -1)
            {
                printf("File not exists! Please check the filename!\n");
                continue ;
            }
            if (!verifyFilePass(arg1, fd_stdin))
            {
                printf("Authorization failed\n");
                continue;
            }
            doEncrypt(arg1, fd_stdin);
        }
        else if (strcmp(cmd, "test") == 0)
        {
            doTest(arg1);
        }
        else
            printf("Command not found, please check!\n");
    }
}

/*======================================================================*
                               TestA
 *======================================================================*/

//A进程
void TestA()
{
    //0号终端
    char tty_name[] = "/dev_tty0";
    //char username[128];
    //char password[128];
   shell(tty_name);

}

/*======================================================================*
                               TestB
 *======================================================================
*/
//B进程
void TestB()
{
	char tty_name[] = "/dev_tty1";
	shell(tty_name);
	/*int fd_stdin  = open(tty_name, O_RDWR);
	assert(fd_stdin  == 0);
	int fd_stdout = open(tty_name, O_RDWR);
	assert(fd_stdout == 1);

	char rdbuf[128];

	while (1) {
		printf("$ ");
		int r = read(fd_stdin, rdbuf, 70);
		rdbuf[r] = 0;

		if (strcmp(rdbuf, "hello") == 0)
			printf("hello world!\n");
		else
			if (rdbuf[0])
				printf("{%s}\n", rdbuf);
	}*/

	assert(0); /* never arrive here */
   // spin("TestB");
}

//C进程
void TestC()
{
    spin("TestC");
}

/*****************************************************************************
 *                                panic
 *****************************************************************************/
PUBLIC void panic(const char *fmt, ...)
{
    int i;
    char buf[256];

    /* 4 is the size of fmt in the stack */
    va_list arg = (va_list)((char*)&fmt + 4);

    i = vsprintf(buf, fmt, arg);

    printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

    /* should never arrive here */
    __asm__ __volatile__("ud2");
}

/*****************************************************************************
 *                                Custom Command
 *****************************************************************************/
char* findpass(char *src)
{
    char pass[128];
    int flag = 0;
    char *p1, *p2;

    p1 = src;
    p2 = pass;

    while (p1 && *p1 != ' ')
    {
        if (*p1 == ':')
            flag = 1;

        if (flag && *p1 != ':')
        {
            *p2 = *p1;
            p2++;
        }
        p1++;
    }
    *p2 = '\0';

    return pass;
}

void clearArr(char *arr, int length)
{
    int i;
    for (i = 0; i < length; i++)
        arr[i] = 0;
}

void printTitle()
{
    clear(); 	

  //  disp_color_str("dddddddddddddddd\n", 0x9);
    if(current_console==0){
    	printf("                        ==================================\n");
    	printf("                                   Aquinux v1.0.0             \n");
    	printf("                                 Kernel on Orange's \n\n");
    	printf("                                     Welcome !\n");
    	printf("                        ==================================\n");
    }
    else{
    	printf("[TTY #%d]\n", current_console);
    }
    
}

void clear()
{	
	clear_screen(0,console_table[current_console].cursor);
    console_table[current_console].crtc_start = console_table[current_console].orig;
    console_table[current_console].cursor = console_table[current_console].orig;    
}

void doTest(char *path)
{
    struct dir_entry *pde = find_entry(path);
    printl(pde->name);
    printl("\n");
    printl(pde->pass);
    printl("\n");
}

int verifyFilePass(char *path, int fd_stdin)
{
    char pass[128];

    struct dir_entry *pde = find_entry(path);

    /*printl(pde->pass);*/

    if (strcmp(pde->pass, "") == 0)
        return 1;

    printl("Please input the file password: ");
    read(fd_stdin, pass, 128);

    if (strcmp(pde->pass, pass) == 0)
        return 1;

    return 0;
}

void doEncrypt(char *path, int fd_stdin)
{
    //查找文件
    /*struct dir_entry *pde = find_entry(path);*/

    char pass[128] = {0};

    printl("Please input the new file password: ");
    read(fd_stdin, pass, 128);

    if (strcmp(pass, "") == 0)
    {
        /*printl("A blank password!\n");*/
        strcpy(pass, "");
    }
    //以下内容用于加密
    int i, j;

    char filename[MAX_PATH];
    memset(filename, 0, MAX_FILENAME_LEN);
    struct inode * dir_inode;

    if (strip_path(filename, path, &dir_inode) != 0)
        return 0;

    if (filename[0] == 0)   /* path: "/" */
        return dir_inode->i_num;

    /**
     * Search the dir for the file.
     */
    int dir_blk0_nr = dir_inode->i_start_sect;
    int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
    int nr_dir_entries =
      dir_inode->i_size / DIR_ENTRY_SIZE; /**
                           * including unused slots
                           * (the file has been deleted
                           * but the slot is still there)
                           */
    int m = 0;
    struct dir_entry * pde;
    for (i = 0; i < nr_dir_blks; i++) {
        RD_SECT(dir_inode->i_dev, dir_blk0_nr + i);
        pde = (struct dir_entry *)fsbuf;
        for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++,pde++)
        {
            if (memcmp(filename, pde->name, MAX_FILENAME_LEN) == 0)
            {
                //刷新文件
                strcpy(pde->pass, pass);
                WR_SECT(dir_inode->i_dev, dir_blk0_nr + i);
                return;
                /*return pde->inode_nr;*/
            }
            if (++m > nr_dir_entries)
                break;
        }
        if (m > nr_dir_entries) /* all entries have been iterated */
            break;
    }

}


void help()
{
    printf("=============================================================================\n");
    printf("Command List     :\n");
    printf("1.  help                          : Show this help message\n");
    printf("2.  clear                         : Clear the screen\n");
    printf("3.  process                       : A process manage,show you all process-info\n");
    printf("4.  ls                            : List all files\n");
    printf("5.  touch       [file]            : Create a new file\n");
    printf("6.  cat         [file]            : Print the file\n");
    printf("7.  vi          [file]            : Modify the content of the file\n");
    printf("8.  rm          [file]            : Delete a file\n");
    printf("9. cp          [SOURCE] [DEST]   : Copy a file\n");
    printf("10. mv          [SOURCE] [DEST]   : Move a file\n");   
    printf("11. encrypt     [file]            : Encrypt a file\n");
    printf("%d", current_console);
  //  printf("17. game                          : 2048 Game\n");
    printf("==============================================================================\n");
}

void ProcessManage()
{
    int i;
    printf("=============================================================================\n");
    printf("      myID      |    name       | spriority    | running?\n");
    //进程号，进程名，优先级，是否是系统进程，是否在运行
    printf("-----------------------------------------------------------------------------\n");
    for ( i = 0 ; i < NR_TASKS + NR_PROCS ; ++i )//逐个遍历
    {
        /*if ( proc_table[i].priority == 0) continue;//系统资源跳过*/
        printf("        %d           %s            %d                yes\n", proc_table[i].pid, proc_table[i].name, proc_table[i].priority);
    }
    printf("=============================================================================\n");
}