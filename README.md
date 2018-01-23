# Cpp-and-Threads
Some examples how C++ multithreading programming works, includes tips and adviced with good approches for achieving good and efficient results.

As a small task was chosen such problem:

1. Create an application which calculates prime numbers for intervals:
    
    - Load intervals from specified xml-based file;
    
    - For each interval start standalone thread for calculating prime numbers;
    
    - Each calculation thread must store every calculated prime number to shared container list;
    
    - Save unique prime numbers to xml-based file (primes tag).
    
Example of source xml file:

![example](https://user-images.githubusercontent.com/28359156/35287817-eff8d79e-006b-11e8-8838-4e80f16622f4.PNG)

USEFUl LINKS:

* A short playlist that covers basics of C++11 multithreading

 https://www.youtube.com/watch?v=LL8wkskDlbs
 
 * Also a small article about C++11 multithreading in two parts
 
 https://habrahabr.ru/post/182610/
 
 https://habrahabr.ru/post/182626/
 
 * Answers on common question on the multithreading topic
 
 http://www.acodersjourney.com/2017/01/c11-multithreading-tutorial-via-faq-thread-management-basics/ 

