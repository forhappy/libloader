#ifdef CHECK_THREADDB_FUNCTION_EXISTS

void ps_pglobal_lookup() {}
void ps_pdread() {}
void ps_pdwrite() {}
void ps_lgetregs() {}
void ps_lsetregs() {}
void ps_lgetfpregs() {}
void ps_lsetfpregs() {}
void ps_get_thread_area() {}
void ps_getpid() {}

char CHECK_THREADDB_FUNCTION_EXISTS();
#ifdef __CLASSIC_C__
int main(){
  int ac;
  char*av[];
#else
int main(int ac, char*av[]){
#endif
  CHECK_THREADDB_FUNCTION_EXISTS();
  if(ac > 1000)
    {
    return *av[0];
    }
  return 0;
}

#else  /* CHECK_THREADDB_FUNCTION_EXISTS */

#  error "CHECK_THREADDB_FUNCTION_EXISTS has to specify the function"

#endif /* CHECK_THREADDB_FUNCTION_EXISTS */
