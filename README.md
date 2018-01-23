# Cpp-and-Threads
Some examples how C++ multithreading programming works, includes tips and adviced with good approches for achieving good and efficient results.

As a small task was chosen such task:

1. Create an application which calculates prime numbers for intervals:
    
    - Load intervals from specified xml-based file;
    
    - For each interval start standalone thread for calculating prime numbers;
    
    - Each calculation thread must store every calculated prime number to shared container list;
    
    - Save unique prime numbers to xml-based file (primes tag).

