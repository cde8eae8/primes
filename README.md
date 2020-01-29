# primes
```
USAGE:                                        
-b number                                     
      set BOUND to number (default = 100)
-t max|number                                 
      set search mode(default = max):                        
      max - search all primes in [1, BOUND]   
      number - search first BOUND primes
-o filename                                   
      output found numbers to file (default to terminal)            
-f filter_name                                
      output primes that satisfy filter (default disabled)      
      available filters are:                  
          super - super primes                
          sophie-germain - sophie-germain primes
-h                                            
      print help 
```

# build
```
mkdir build
cd build
cmake ..
make
```

# run
## program
```./main [arguments]```
## tests
```./prime-tests```
