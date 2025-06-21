#include "mmap.h"
#include "string.h"
#include "text.h"
#include "vbe.h"

extern int random (void);

static void root_printwelcome (void);
static void root_initcmd (void);
__attribute__ ((noreturn)) void root_main (void);

const int line_size = 46;
const char main_theme[] = ",-.----.                              ___     "
                          "\\    /  \\                           ,--.'|_   "
                          ";   :    \\     ,---.      ,---.     |  | :,'  "
                          "|   | .\\ :    '   ,'\\    '   ,'\\    :  : ' :  "
                          ".   : |: |   /   /   |  /   /   | .;__,'  /   "
                          "|   |  \\ :  .   ; ,. : .   ; ,. : |  |   |    "
                          "|   : .  /  '   | |: : '   | |: : :__,'| :    "
                          ";   | |  \\  '   | .; : '   | .; :   '  : |__  "
                          "|   | ;\\  \\ |   :    | |   :    |   |  | '.'| "
                          ":   ' | \\.'  \\   \\  /   \\   \\  /    ;  :    ; "
                          ":   : :-'     `----'     `----'     |  ,   /  "
                          "|   |.'                              ---`-'   "
                          "`---'                                         ";

const char *greeting = "Welcome to Root.";

void
root_printwelcome (void)
{
  int width = 80, hwidth = width >> 1;
  int start = hwidth - line_size / 2;
  int greeting_size = root_strlen (greeting);
  root_printf ("\n");
  for (unsigned int i = 0; i < sizeof (main_theme) / line_size; i++)
    {
      for (int j = 0; j < start; j++)
        root_putchar_unsynced (' ');
      root_printf ("%.*s\n", line_size, main_theme + line_size * i);
    }
  for (int i = 0; i < hwidth - greeting_size / 2; i++)
    root_putchar_unsynced (' ');
  root_printf ("%s\n\n", greeting);
}

void
root_initcmd (void)
{
  // kprintf ("zOS> ");
}

void
root_main (void)
{
  root_inittext ();
  root_printwelcome ();
  root_init_mmap ();
  root_queryvbeinfo ();
  root_initcmd ();
  for (;;)
    ;
}
