public static void foo();
  Code:
    0: ICONST_0                                
    1: ISTORE_0                                             
    2: ICONST_0                                             
    3: ISTORE_1                                             
    4: ILOAD_1                                 
    5: LDC 100000                                           
    7: IF_ICMPGE 36                                         
   10: ILOAD_1                                              
   11: LDC 66666                                            
   13: IF_ICMPLE 9                                          
   16: IINC 0 1                                             
   19: GOTO 18                                              
   22: ILOAD_1                                              
   23: LDC 33333                                            
   25: IF_ICMPLE 9                                          
   28: IINC 0 2                                             
   31: GOTO 6                                               
   34: IINC 0 3                                             
   37: IINC 1 1                                             
   40: GOTO -36                                             
   43: RETURN 